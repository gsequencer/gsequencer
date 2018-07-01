/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/recall/ags_eq10_recycling.h>
#include <ags/audio/recall/ags_eq10_channel.h>
#include <ags/audio/recall/ags_eq10_audio_signal.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recall_id.h>

void ags_eq10_recycling_class_init(AgsEq10RecyclingClass *eq10_recycling);
void ags_eq10_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_eq10_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_eq10_recycling_init(AgsEq10Recycling *eq10_recycling);
void ags_eq10_recycling_connect(AgsConnectable *connectable);
void ags_eq10_recycling_disconnect(AgsConnectable *connectable);
void ags_eq10_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_eq10_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_eq10_recycling_finalize(GObject *gobject);

AgsRecall* ags_eq10_recycling_duplicate(AgsRecall *recall,
					AgsRecallID *recall_id,
					guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_eq10_recycling
 * @short_description: 10 band equalizer on recycling
 * @title: AgsEq10Recycling
 * @section_id:
 * @include: ags/audio/recall/ags_eq10_recycling.h
 *
 * The #AgsEq10Recycling class does a 10 band equalizer on the recycling.
 */

static gpointer ags_eq10_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_eq10_recycling_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_eq10_recycling_parent_dynamic_connectable_interface;

GType
ags_eq10_recycling_get_type()
{
  static GType ags_type_eq10_recycling = 0;

  if(!ags_type_eq10_recycling){
    static const GTypeInfo ags_eq10_recycling_info = {
      sizeof(AgsEq10RecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_eq10_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsEq10Recycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_eq10_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_eq10_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_eq10_recycling_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_eq10_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						     "AgsEq10Recycling",
						     &ags_eq10_recycling_info,
						     0);

    g_type_add_interface_static(ags_type_eq10_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_eq10_recycling,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return(ags_type_eq10_recycling);
}

void
ags_eq10_recycling_class_init(AgsEq10RecyclingClass *eq10_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_eq10_recycling_parent_class = g_type_class_peek_parent(eq10_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) eq10_recycling;

  gobject->finalize = ags_eq10_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) eq10_recycling;

  recall->duplicate = ags_eq10_recycling_duplicate;
}

void
ags_eq10_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_eq10_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_eq10_recycling_connect;
  connectable->disconnect = ags_eq10_recycling_disconnect;
}

void
ags_eq10_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_eq10_recycling_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_eq10_recycling_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_eq10_recycling_disconnect_dynamic;
}

void
ags_eq10_recycling_init(AgsEq10Recycling *eq10_recycling)
{
  AGS_RECALL(eq10_recycling)->name = "ags-eq10";
  AGS_RECALL(eq10_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(eq10_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(eq10_recycling)->xml_type = "ags-eq10-recycling";
  AGS_RECALL(eq10_recycling)->port = NULL;

  AGS_RECALL(eq10_recycling)->child_type = AGS_TYPE_EQ10_AUDIO_SIGNAL;

  AGS_RECALL_RECYCLING(eq10_recycling)->flags |= (AGS_RECALL_RECYCLING_MAP_CHILD_SOURCE);
}

void
ags_eq10_recycling_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_eq10_recycling_parent_class)->finalize(gobject);
}

void
ags_eq10_recycling_connect(AgsConnectable *connectable)
{ 
  /* call parent */
  ags_eq10_recycling_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_eq10_recycling_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_eq10_recycling_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_eq10_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_eq10_recycling_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_eq10_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_eq10_recycling_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* empty */
}

AgsRecall*
ags_eq10_recycling_duplicate(AgsRecall *recall,
			     AgsRecallID *recall_id,
			     guint *n_params, GParameter *parameter)
{
  AgsEq10Recycling *copy;

  copy = (AgsEq10Recycling *) AGS_RECALL_CLASS(ags_eq10_recycling_parent_class)->duplicate(recall,
											   recall_id,
											   n_params, parameter);

  return((AgsRecall *) copy);
}

/**
 * ags_eq10_recycling_new:
 * @recycling: the source #AgsRecycling
 *
 * Creates an #AgsEq10Recycling
 *
 * Returns: a new #AgsEq10Recycling
 *
 * Since: 1.5.0
 */
AgsEq10Recycling*
ags_eq10_recycling_new(AgsRecycling *recycling)
{
  AgsEq10Recycling *eq10_recycling;

  eq10_recycling = (AgsEq10Recycling *) g_object_new(AGS_TYPE_EQ10_RECYCLING,
						     "source", recycling,
						     NULL);

  return(eq10_recycling);
}
