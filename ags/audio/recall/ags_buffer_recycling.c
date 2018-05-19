/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/audio/recall/ags_buffer_recycling.h>
#include <ags/audio/recall/ags_buffer_audio_signal.h>

#include <ags/libags.h>

void ags_buffer_recycling_class_init(AgsBufferRecyclingClass *buffer_recycling);
void ags_buffer_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_buffer_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_buffer_recycling_init(AgsBufferRecycling *buffer_recycling);
void ags_buffer_recycling_finalize(GObject *gobject);

AgsRecall* ags_buffer_recycling_duplicate(AgsRecall *recall,
					  AgsRecallID *recall_id,
					  guint *n_params, gchar **parameter_name, GValue *value);

/**
 * SECTION:ags_buffer_recycling
 * @short_description: buffer recycling
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
      sizeof(AgsBufferRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_buffer_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsBufferRecycling),
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
						       "AgsBufferRecycling",
						       &ags_buffer_recycling_info,
						       0);

    g_type_add_interface_static(ags_type_buffer_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_buffer_recycling);
}

void
ags_buffer_recycling_class_init(AgsBufferRecyclingClass *buffer_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

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
}

void
ags_buffer_recycling_init(AgsBufferRecycling *buffer_recycling)
{
  AGS_RECALL(buffer_recycling)->name = "ags-buffer";
  AGS_RECALL(buffer_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(buffer_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(buffer_recycling)->xml_type = "ags-buffer-recycling";

  AGS_RECALL(buffer_recycling)->port = NULL;

  AGS_RECALL(buffer_recycling)->child_type = AGS_TYPE_BUFFER_AUDIO_SIGNAL;
}

void
ags_buffer_recycling_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_buffer_recycling_parent_class)->finalize(gobject);
}

AgsRecall*
ags_buffer_recycling_duplicate(AgsRecall *recall,
			       AgsRecallID *recall_id,
			       guint *n_params, gchar **parameter_name, GValue *value)
{
  AgsBufferRecycling *copy_buffer_recycling;

  copy_buffer_recycling = (AgsBufferRecycling *) AGS_RECALL_CLASS(ags_buffer_recycling_parent_class)->duplicate(recall,
														recall_id,
														n_params, parameter_name, value);
  
  return((AgsRecall *) copy_buffer_recycling);
}

/**
 * ags_buffer_recycling_new:
 * @destination: the destination #AgsRecycling
 * @source: the source #AgsRecycling
 *
 * Create a new instance of #AgsBufferRecycling
 *
 * Returns: the new #AgsBufferRecycling
 *
 * Since: 2.0.0
 */
AgsBufferRecycling*
ags_buffer_recycling_new(AgsRecycling *destination,
			 AgsRecycling *source)
{
  AgsBufferRecycling *buffer_recycling;

  buffer_recycling = (AgsBufferRecycling *) g_object_new(AGS_TYPE_BUFFER_RECYCLING,
							 "destination", destination,
							 "source", source,
							 NULL);

  return(buffer_recycling);
}
