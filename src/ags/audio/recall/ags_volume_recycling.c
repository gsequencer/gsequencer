/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/audio/recall/ags_volume_recycling.h>
#include <ags/audio/recall/ags_volume_channel.h>
#include <ags/audio/recall/ags_volume_audio_signal.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/lib/ags_parameter.h>

#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recall_id.h>

void ags_volume_recycling_class_init(AgsVolumeRecyclingClass *volume_recycling);
void ags_volume_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_volume_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_volume_recycling_init(AgsVolumeRecycling *volume_recycling);
void ags_volume_recycling_connect(AgsConnectable *connectable);
void ags_volume_recycling_disconnect(AgsConnectable *connectable);
void ags_volume_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_volume_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_volume_recycling_finalize(GObject *gobject);

AgsRecall* ags_volume_recycling_duplicate(AgsRecall *recall,
					  AgsRecallID *recall_id,
					  guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_volume_recycling
 * @short_description: volumes recycling
 * @title: AgsVolumeRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_volume_recycling.h
 *
 * The #AgsVolumeRecycling class volumes the recycling.
 */

static gpointer ags_volume_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_volume_recycling_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_volume_recycling_parent_dynamic_connectable_interface;

GType
ags_volume_recycling_get_type()
{
  static GType ags_type_volume_recycling = 0;

  if(!ags_type_volume_recycling){
    static const GTypeInfo ags_volume_recycling_info = {
      sizeof (AgsVolumeRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_volume_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVolumeRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_volume_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_volume_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_volume_recycling_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_volume_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						       "AgsVolumeRecycling\0",
						       &ags_volume_recycling_info,
						       0);

    g_type_add_interface_static(ags_type_volume_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_volume_recycling,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return (ags_type_volume_recycling);
}

void
ags_volume_recycling_class_init(AgsVolumeRecyclingClass *volume_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_volume_recycling_parent_class = g_type_class_peek_parent(volume_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) volume_recycling;

  gobject->finalize = ags_volume_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) volume_recycling;

  recall->duplicate = ags_volume_recycling_duplicate;
}

void
ags_volume_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_volume_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_volume_recycling_connect;
  connectable->disconnect = ags_volume_recycling_disconnect;
}

void
ags_volume_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_volume_recycling_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_volume_recycling_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_volume_recycling_disconnect_dynamic;
}

void
ags_volume_recycling_init(AgsVolumeRecycling *volume_recycling)
{
  AGS_RECALL(volume_recycling)->name = "ags-volume\0";
  AGS_RECALL(volume_recycling)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(volume_recycling)->build_id = AGS_BUILD_ID;
  AGS_RECALL(volume_recycling)->xml_type = "ags-volume-recycling\0";
  AGS_RECALL(volume_recycling)->port = NULL;

  AGS_RECALL(volume_recycling)->child_type = AGS_TYPE_VOLUME_AUDIO_SIGNAL;

  AGS_RECALL_RECYCLING(volume_recycling)->flags |= (AGS_RECALL_RECYCLING_MAP_CHILD_SOURCE);
}

void
ags_volume_recycling_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_volume_recycling_parent_class)->finalize(gobject);
}

void
ags_volume_recycling_connect(AgsConnectable *connectable)
{ 
  /* call parent */
  ags_volume_recycling_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_volume_recycling_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_volume_recycling_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_volume_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_volume_recycling_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_volume_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_volume_recycling_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* empty */
}

AgsRecall*
ags_volume_recycling_duplicate(AgsRecall *recall,
			       AgsRecallID *recall_id,
			       guint *n_params, GParameter *parameter)
{
  AgsVolumeRecycling *copy;

  copy = (AgsVolumeRecycling *) AGS_RECALL_CLASS(ags_volume_recycling_parent_class)->duplicate(recall,
											       recall_id,
											       n_params, parameter);

  return((AgsRecall *) copy);
}

/**
 * ags_volume_recycling_new:
 * @recycling: the source #AgsRecycling
 *
 * Creates an #AgsVolumeRecycling
 *
 * Returns: a new #AgsVolumeRecycling
 *
 * Since: 0.4
 */
AgsVolumeRecycling*
ags_volume_recycling_new(AgsRecycling *recycling)
{
  AgsVolumeRecycling *volume_recycling;

  volume_recycling = (AgsVolumeRecycling *) g_object_new(AGS_TYPE_VOLUME_RECYCLING,
							 "source\0", recycling,
							 NULL);

  return(volume_recycling);
}

