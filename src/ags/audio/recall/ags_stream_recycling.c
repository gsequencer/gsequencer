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

#include <ags/audio/recall/ags_stream_recycling.h>
#include <ags/audio/recall/ags_stream_audio_signal.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_dynamic_connectable.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_devout.h>

#include <ags/audio/task/ags_remove_audio_signal.h>

void ags_stream_recycling_class_init(AgsStreamRecyclingClass *stream_recycling);
void ags_stream_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_stream_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_stream_recycling_init(AgsStreamRecycling *stream_recycling);
void ags_stream_recycling_connect(AgsConnectable *connectable);
void ags_stream_recycling_disconnect(AgsConnectable *connectable);
void ags_stream_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_stream_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_stream_recycling_finalize(GObject *gobject);

AgsRecall* ags_stream_recycling_duplicate(AgsRecall *recall,
					  AgsRecallID *recall_id,
					  guint *n_params, GParameter *parameter);

void ags_stream_recycling_source_add_audio_signal_callback(AgsRecycling *source,
							   AgsAudioSignal *audio_signal,
							   AgsStreamRecycling *stream_recycling);
void ags_stream_recycling_source_remove_audio_signal_callback(AgsRecycling *source,
							      AgsAudioSignal *audio_signal,
							      AgsStreamRecycling *stream_recycling);

/**
 * SECTION:ags_stream_recycling
 * @short_description: streams recycling
 * @title: AgsStreamRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_stream_recycling.h
 *
 * The #AgsStreamRecycling streams the recycling with appropriate #AgsRecallID.
 */

static gpointer ags_stream_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_stream_recycling_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_stream_recycling_parent_dynamic_connectable_interface;

GType
ags_stream_recycling_get_type()
{
  static GType ags_type_stream_recycling = 0;

  if(!ags_type_stream_recycling){
    static const GTypeInfo ags_stream_recycling_info = {
      sizeof (AgsStreamRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_stream_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStreamRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_stream_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_stream_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_stream_recycling_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_stream_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						       "AgsStreamRecycling\0",
						       &ags_stream_recycling_info,
						       0);

    g_type_add_interface_static(ags_type_stream_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_stream_recycling,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return (ags_type_stream_recycling);
}

void
ags_stream_recycling_class_init(AgsStreamRecyclingClass *stream_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_stream_recycling_parent_class = g_type_class_peek_parent(stream_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) stream_recycling;

  gobject->finalize = ags_stream_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) stream_recycling;

  recall->duplicate = ags_stream_recycling_duplicate;
}

void
ags_stream_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_stream_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_stream_recycling_connect;
  connectable->disconnect = ags_stream_recycling_disconnect;
}

void
ags_stream_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_stream_recycling_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_stream_recycling_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_stream_recycling_disconnect_dynamic;
}

void
ags_stream_recycling_init(AgsStreamRecycling *stream_recycling)
{
  AGS_RECALL(stream_recycling)->name = "ags-stream\0";
  AGS_RECALL(stream_recycling)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(stream_recycling)->build_id = AGS_BUILD_ID;
  AGS_RECALL(stream_recycling)->xml_type = "ags-stream-recycling\0";
  AGS_RECALL(stream_recycling)->port = NULL;

  AGS_RECALL(stream_recycling)->child_type = AGS_TYPE_STREAM_AUDIO_SIGNAL;

  AGS_RECALL_RECYCLING(stream_recycling)->flags |= (AGS_RECALL_RECYCLING_MAP_CHILD_SOURCE);
}

void
ags_stream_recycling_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_stream_recycling_parent_class)->finalize(gobject);
}

void
ags_stream_recycling_connect(AgsConnectable *connectable)
{
  AgsStreamRecycling *stream_recycling;

  stream_recycling = AGS_STREAM_RECYCLING(connectable);

  ags_stream_recycling_parent_connectable_interface->connect(connectable);

  //  g_signal_connect(AGS_RECALL_RECYCLING(stream_recycling)->source, "add_audio_signal\0",
  //		   G_CALLBACK(ags_stream_recycling_source_add_audio_signal_callback), stream_recycling);
  //  g_signal_connect(AGS_RECALL_RECYCLING(stream_recycling)->source, "remove_audio_signal\0",
  //		   G_CALLBACK(ags_stream_recycling_source_remove_audio_signal_callback), stream_recycling);
}

void
ags_stream_recycling_disconnect(AgsConnectable *connectable)
{
  ags_stream_recycling_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_stream_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsStreamRecycling *stream_recycling;

  ags_stream_recycling_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);
}

void
ags_stream_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  ags_stream_recycling_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);
}

AgsRecall*
ags_stream_recycling_duplicate(AgsRecall *recall,
			       AgsRecallID *recall_id,
			       guint *n_params, GParameter *parameter)
{
  AgsStreamRecycling *copy;

  copy = (AgsStreamRecycling *) AGS_RECALL_CLASS(ags_stream_recycling_parent_class)->duplicate(recall,
											       recall_id,
											       n_params, parameter);


  return((AgsRecall *) copy);
}

void
ags_stream_recycling_source_add_audio_signal_callback(AgsRecycling *source,
						      AgsAudioSignal *audio_signal,
						      AgsStreamRecycling *stream_recycling)
{
  //  g_object_ref(audio_signal);
}

void
ags_stream_recycling_source_remove_audio_signal_callback(AgsRecycling *source,
							 AgsAudioSignal *audio_signal,
							 AgsStreamRecycling *stream_recycling)
{
  AgsRemoveAudioSignal *remove_audio_signal;
  //  AgsTaskThread *task_thread;
  //  AgsUnrefAudioSignal *unref_audio_signal;

  //  task_thread = AGS_AUDIO_LOOP(AGS_MAIN(AGS_DEVOUT(AGS_RECALL(gobject)->devout)->ags_main)->main_loop)->task_thread;

  //  unref_audio_signal = ags_unref_audio_signal_new(AGS_RECALL_AUDIO_SIGNAL(gobject)->source);
  //  ags_task_thread_append_task(task_thread,
  //			      unref_audio_signal);
}

/**
 * ags_stream_recycling_new:
 * @recycling: the #AgsRecycling
 *
 * Creates an #AgsStreamRecycling
 *
 * Returns: a new #AgsStreamRecycling
 *
 * Since: 0.4
 */
AgsStreamRecycling*
ags_stream_recycling_new(AgsRecycling *recycling)
{
  AgsStreamRecycling *stream_recycling;

  stream_recycling = (AgsStreamRecycling *) g_object_new(AGS_TYPE_STREAM_RECYCLING,
							 "source\0", recycling,
							 NULL);

  return(stream_recycling);
}
