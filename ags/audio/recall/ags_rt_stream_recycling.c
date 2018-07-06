/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/recall/ags_rt_stream_recycling.h>
#include <ags/audio/recall/ags_rt_stream_audio_signal.h>

#include <ags/libags.h>

#include <ags/audio/task/ags_remove_audio_signal.h>

void ags_rt_stream_recycling_class_init(AgsRtStreamRecyclingClass *rt_stream_recycling);
void ags_rt_stream_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_rt_stream_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_rt_stream_recycling_init(AgsRtStreamRecycling *rt_stream_recycling);
void ags_rt_stream_recycling_connect(AgsConnectable *connectable);
void ags_rt_stream_recycling_disconnect(AgsConnectable *connectable);
void ags_rt_stream_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_rt_stream_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_rt_stream_recycling_finalize(GObject *gobject);

AgsRecall* ags_rt_stream_recycling_duplicate(AgsRecall *recall,
					     AgsRecallID *recall_id,
					     guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_rt_stream_recycling
 * @short_description: rt_streams recycling
 * @title: AgsRtStreamRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_rt_stream_recycling.h
 *
 * The #AgsRtStreamRecycling rt_streams the recycling with appropriate #AgsRecallID.
 */

static gpointer ags_rt_stream_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_rt_stream_recycling_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_rt_stream_recycling_parent_dynamic_connectable_interface;

GType
ags_rt_stream_recycling_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_rt_stream_recycling;

    static const GTypeInfo ags_rt_stream_recycling_info = {
      sizeof (AgsRtStreamRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_rt_stream_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRtStreamRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_rt_stream_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_rt_stream_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_rt_stream_recycling_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_rt_stream_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
							  "AgsRtStreamRecycling",
							  &ags_rt_stream_recycling_info,
							  0);

    g_type_add_interface_static(ags_type_rt_stream_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_rt_stream_recycling,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);

    g_once_init_leave (&g_define_type_id__volatile, ags_type_rt_stream_recycling);
  }

  return g_define_type_id__volatile;
}

void
ags_rt_stream_recycling_class_init(AgsRtStreamRecyclingClass *rt_stream_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_rt_stream_recycling_parent_class = g_type_class_peek_parent(rt_stream_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) rt_stream_recycling;

  gobject->finalize = ags_rt_stream_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) rt_stream_recycling;

  recall->duplicate = ags_rt_stream_recycling_duplicate;
}

void
ags_rt_stream_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_rt_stream_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_rt_stream_recycling_connect;
  connectable->disconnect = ags_rt_stream_recycling_disconnect;
}

void
ags_rt_stream_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_rt_stream_recycling_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_rt_stream_recycling_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_rt_stream_recycling_disconnect_dynamic;
}

void
ags_rt_stream_recycling_init(AgsRtStreamRecycling *rt_stream_recycling)
{
  AGS_RECALL(rt_stream_recycling)->name = "ags-rt_stream";
  AGS_RECALL(rt_stream_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(rt_stream_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(rt_stream_recycling)->xml_type = "ags-rt_stream-recycling";
  AGS_RECALL(rt_stream_recycling)->port = NULL;

  AGS_RECALL(rt_stream_recycling)->child_type = AGS_TYPE_RT_STREAM_AUDIO_SIGNAL;

  AGS_RECALL_RECYCLING(rt_stream_recycling)->flags |= (AGS_RECALL_RECYCLING_MAP_CHILD_SOURCE);
}

void
ags_rt_stream_recycling_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_rt_stream_recycling_parent_class)->finalize(gobject);
}

void
ags_rt_stream_recycling_connect(AgsConnectable *connectable)
{
  ags_rt_stream_recycling_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_rt_stream_recycling_disconnect(AgsConnectable *connectable)
{
  ags_rt_stream_recycling_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_rt_stream_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsRtStreamRecycling *rt_stream_recycling;

  ags_rt_stream_recycling_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);
}

void
ags_rt_stream_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  ags_rt_stream_recycling_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);
}

AgsRecall*
ags_rt_stream_recycling_duplicate(AgsRecall *recall,
				  AgsRecallID *recall_id,
				  guint *n_params, GParameter *parameter)
{
  AgsRtStreamRecycling *copy;

  copy = (AgsRtStreamRecycling *) AGS_RECALL_CLASS(ags_rt_stream_recycling_parent_class)->duplicate(recall,
												    recall_id,
												    n_params, parameter);


  return((AgsRecall *) copy);
}

/**
 * ags_rt_stream_recycling_new:
 * @recycling: the #AgsRecycling
 *
 * Creates an #AgsRtStreamRecycling
 *
 * Returns: a new #AgsRtStreamRecycling
 *
 * Since: 1.4.0
 */
AgsRtStreamRecycling*
ags_rt_stream_recycling_new(AgsRecycling *recycling)
{
  AgsRtStreamRecycling *rt_stream_recycling;

  rt_stream_recycling = (AgsRtStreamRecycling *) g_object_new(AGS_TYPE_RT_STREAM_RECYCLING,
							      "source", recycling,
							      NULL);

  return(rt_stream_recycling);
}
