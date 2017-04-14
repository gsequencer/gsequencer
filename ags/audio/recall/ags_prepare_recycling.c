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

#include <ags/audio/recall/ags_prepare_recycling.h>
#include <ags/audio/recall/ags_prepare_audio_signal.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>

void ags_prepare_recycling_class_init(AgsPrepareRecyclingClass *prepare_recycling);
void ags_prepare_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_prepare_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_prepare_recycling_init(AgsPrepareRecycling *prepare_recycling);
void ags_prepare_recycling_connect(AgsConnectable *connectable);
void ags_prepare_recycling_disconnect(AgsConnectable *connectable);
void ags_prepare_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_prepare_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_prepare_recycling_finalize(GObject *gobject);

AgsRecall* ags_prepare_recycling_duplicate(AgsRecall *recall,
					   AgsRecallID *recall_id,
					   guint *n_params, GParameter *parameter);
void ags_prepare_recycling_remove(AgsRecall *recall);

/**
 * SECTION:ags_prepare_recycling
 * @short_description: prepares recycling
 * @title: AgsPrepareRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_prepare_recycling.h
 *
 * The #AgsPrepareRecycling class prepares the recycling.
 */

static gpointer ags_prepare_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_prepare_recycling_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_prepare_recycling_parent_dynamic_connectable_interface;

GType
ags_prepare_recycling_get_type()
{
  static GType ags_type_prepare_recycling = 0;

  if(!ags_type_prepare_recycling){
    static const GTypeInfo ags_prepare_recycling_info = {
      sizeof (AgsPrepareRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_prepare_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPrepareRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_prepare_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_prepare_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_prepare_recycling_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_prepare_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
							"AgsPrepareRecycling\0",
							&ags_prepare_recycling_info,
							0);

    g_type_add_interface_static(ags_type_prepare_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_prepare_recycling,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return (ags_type_prepare_recycling);
}

void
ags_prepare_recycling_class_init(AgsPrepareRecyclingClass *prepare_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_prepare_recycling_parent_class = g_type_class_peek_parent(prepare_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) prepare_recycling;

  gobject->finalize = ags_prepare_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) prepare_recycling;

  recall->duplicate = ags_prepare_recycling_duplicate;
}

void
ags_prepare_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_prepare_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_prepare_recycling_connect;
  connectable->disconnect = ags_prepare_recycling_disconnect;
}

void
ags_prepare_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_prepare_recycling_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_prepare_recycling_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_prepare_recycling_disconnect_dynamic;
}

void
ags_prepare_recycling_init(AgsPrepareRecycling *prepare_recycling)
{
  AGS_RECALL(prepare_recycling)->name = "ags-prepare\0";
  AGS_RECALL(prepare_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(prepare_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(prepare_recycling)->xml_type = "ags-prepare-recycling\0";
  AGS_RECALL(prepare_recycling)->port = NULL;

  AGS_RECALL(prepare_recycling)->child_type = AGS_TYPE_PREPARE_AUDIO_SIGNAL;

  AGS_RECALL_RECYCLING(prepare_recycling)->flags |= (AGS_RECALL_RECYCLING_MAP_CHILD_SOURCE);
}

void
ags_prepare_recycling_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_prepare_recycling_parent_class)->finalize(gobject);
}

void
ags_prepare_recycling_connect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  ags_prepare_recycling_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_prepare_recycling_disconnect(AgsConnectable *connectable)
{
  ags_prepare_recycling_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_prepare_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(dynamic_connectable)->flags)) != 0){
    return;
  }

  ags_prepare_recycling_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);
}

void
ags_prepare_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  ags_prepare_recycling_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);
}

AgsRecall*
ags_prepare_recycling_duplicate(AgsRecall *recall,
				AgsRecallID *recall_id,
				guint *n_params, GParameter *parameter)
{
  AgsPrepareRecycling *copy;

  copy = (AgsPrepareRecycling *) AGS_RECALL_CLASS(ags_prepare_recycling_parent_class)->duplicate(recall,
												 recall_id,
												 n_params, parameter);


  return((AgsRecall *) copy);
}

/**
 * ags_prepare_recycling_new:
 * @recycling: an #AgsRecycling
 *
 * Creates an #AgsPrepareRecycling
 *
 * Returns: a new #AgsPrepareRecycling
 *
 * Since: 0.7.122.8
 */
AgsPrepareRecycling*
ags_prepare_recycling_new(AgsRecycling *recycling)
{
  AgsPrepareRecycling *prepare_recycling;

  prepare_recycling = (AgsPrepareRecycling *) g_object_new(AGS_TYPE_PREPARE_RECYCLING,
							   "source\0", recycling,
							   NULL);

  return(prepare_recycling);
}
