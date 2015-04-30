/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/recall/ags_buffer_recycling.h>
#include <ags/audio/recall/ags_buffer_audio_signal.h>

#include <ags/main.h>

#include <ags/object/ags_connectable.h>

#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/ags_devout.h>

#include <ags/audio/task/ags_remove_audio_signal.h>

void ags_buffer_recycling_class_init(AgsBufferRecyclingClass *buffer_recycling);
void ags_buffer_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_buffer_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_buffer_recycling_init(AgsBufferRecycling *buffer_recycling);
void ags_buffer_recycling_connect(AgsConnectable *connectable);
void ags_buffer_recycling_disconnect(AgsConnectable *connectable);
void ags_buffer_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_buffer_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_buffer_recycling_finalize(GObject *gobject);

AgsRecall* ags_buffer_recycling_duplicate(AgsRecall *recall,
					  AgsRecallID *recall_id,
					  guint *n_params, GParameter *parameter);
void ags_buffer_recycling_remove(AgsRecall *recall);

/**
 * SECTION:ags_buffer_recycling
 * @short_description: buffers recycling
 * @title: AgsBufferRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_buffer_recycling.h
 *
 * The #AgsBufferRecycling class buffers the recycling.
 */

static gpointer ags_buffer_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_buffer_recycling_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_buffer_recycling_parent_dynamic_connectable_interface;

GType
ags_buffer_recycling_get_type()
{
  static GType ags_type_buffer_recycling = 0;

  if(!ags_type_buffer_recycling){
    static const GTypeInfo ags_buffer_recycling_info = {
      sizeof (AgsBufferRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_buffer_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsBufferRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_buffer_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_buffer_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_buffer_recycling_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_buffer_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						       "AgsBufferRecycling\0",
						       &ags_buffer_recycling_info,
						       0);

    g_type_add_interface_static(ags_type_buffer_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_buffer_recycling,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return (ags_type_buffer_recycling);
}

void
ags_buffer_recycling_class_init(AgsBufferRecyclingClass *buffer_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_buffer_recycling_parent_class = g_type_class_peek_parent(buffer_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) buffer_recycling;

  gobject->finalize = ags_buffer_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) buffer_recycling;

  recall->duplicate = ags_buffer_recycling_duplicate;
}

void
ags_buffer_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_buffer_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_buffer_recycling_connect;
  connectable->disconnect = ags_buffer_recycling_disconnect;
}

void
ags_buffer_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_buffer_recycling_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_buffer_recycling_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_buffer_recycling_disconnect_dynamic;
}

void
ags_buffer_recycling_init(AgsBufferRecycling *buffer_recycling)
{
  AGS_RECALL(buffer_recycling)->name = "ags-buffer\0";
  AGS_RECALL(buffer_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(buffer_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(buffer_recycling)->xml_type = "ags-buffer-recycling\0";
  AGS_RECALL(buffer_recycling)->port = NULL;

  AGS_RECALL(buffer_recycling)->child_type = AGS_TYPE_BUFFER_AUDIO_SIGNAL;

  AGS_RECALL_RECYCLING(buffer_recycling)->flags |= (AGS_RECALL_RECYCLING_MAP_CHILD_DESTINATION |
						    AGS_RECALL_RECYCLING_CREATE_DESTINATION_ON_MAP_SOURCE);
}

void
ags_buffer_recycling_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_buffer_recycling_parent_class)->finalize(gobject);
}

void
ags_buffer_recycling_connect(AgsConnectable *connectable)
{
  ags_buffer_recycling_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_buffer_recycling_disconnect(AgsConnectable *connectable)
{
  ags_buffer_recycling_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_buffer_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  ags_buffer_recycling_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);
}

void
ags_buffer_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  ags_buffer_recycling_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);
}

AgsRecall*
ags_buffer_recycling_duplicate(AgsRecall *recall,
			       AgsRecallID *recall_id,
			       guint *n_params, GParameter *parameter)
{
  AgsBufferRecycling *copy;

  copy = (AgsBufferRecycling *) AGS_RECALL_CLASS(ags_buffer_recycling_parent_class)->duplicate(recall,
											       recall_id,
											       n_params, parameter);


  return((AgsRecall *) copy);
}

/**
 * ags_buffer_recycling_new:
 * @recycling: an #AgsRecycling
 *
 * Creates an #AgsBufferRecycling
 *
 * Returns: a new #AgsBufferRecycling
 *
 * Since: 0.4
 */
AgsBufferRecycling*
ags_buffer_recycling_new(AgsRecycling *recycling)
{
  AgsBufferRecycling *buffer_recycling;

  buffer_recycling = (AgsBufferRecycling *) g_object_new(AGS_TYPE_BUFFER_RECYCLING,
							 "source\0", recycling,
							 NULL);

  return(buffer_recycling);
}
