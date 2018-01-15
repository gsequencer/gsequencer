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

#include <ags/audio/recall/ags_rt_stream_channel_run.h>
#include <ags/audio/recall/ags_rt_stream_recycling.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/task/ags_cancel_recall.h>

void ags_rt_stream_channel_run_class_init(AgsRtStreamChannelRunClass *rt_stream_channel_run);
void ags_rt_stream_channel_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_rt_stream_channel_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_rt_stream_channel_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_rt_stream_channel_run_init(AgsRtStreamChannelRun *rt_stream_channel_run);
void ags_rt_stream_channel_run_connect(AgsConnectable *connectable);
void ags_rt_stream_channel_run_disconnect(AgsConnectable *connectable);
void ags_rt_stream_channel_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_rt_stream_channel_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_rt_stream_channel_run_finalize(GObject *gobject);

void ags_rt_stream_channel_run_check_rt_stream(AgsRecall *recall);
AgsRecall* ags_rt_stream_channel_run_duplicate(AgsRecall *recall,
					       AgsRecallID *recall_id,
					       guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_rt_stream_channel_run
 * @short_description: rt_streams channel
 * @title: AgsRtStreamChannelRun
 * @section_id:
 * @include: ags/audio/recall/ags_rt_stream_channel_run.h
 *
 * The #AgsRtStreamChannelRun class streams the audio signal template.
 */

static gpointer ags_rt_stream_channel_run_parent_class = NULL;
static AgsConnectableInterface *ags_rt_stream_channel_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_rt_stream_channel_run_parent_dynamic_connectable_interface;
static AgsPluginInterface *ags_rt_stream_channel_run_parent_plugin_interface;

GType
ags_rt_stream_channel_run_get_type()
{
  static GType ags_type_rt_stream_channel_run = 0;

  if(!ags_type_rt_stream_channel_run){
    static const GTypeInfo ags_rt_stream_channel_run_info = {
      sizeof (AgsRtStreamChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_rt_stream_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRtStreamChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_rt_stream_channel_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_rt_stream_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_rt_stream_channel_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_rt_stream_channel_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };    

    ags_type_rt_stream_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
							    "AgsRtStreamChannelRun",
							    &ags_rt_stream_channel_run_info,
							    0);

    g_type_add_interface_static(ags_type_rt_stream_channel_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_rt_stream_channel_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);

    g_type_add_interface_static(ags_type_rt_stream_channel_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return (ags_type_rt_stream_channel_run);
}

void
ags_rt_stream_channel_run_class_init(AgsRtStreamChannelRunClass *rt_stream_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_rt_stream_channel_run_parent_class = g_type_class_peek_parent(rt_stream_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) rt_stream_channel_run;

  gobject->finalize = ags_rt_stream_channel_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) rt_stream_channel_run;

  recall->check_rt_stream = ags_rt_stream_channel_run_check_rt_stream;
  recall->duplicate = ags_rt_stream_channel_run_duplicate;
}

void
ags_rt_stream_channel_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_rt_stream_channel_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_rt_stream_channel_run_connect;
  connectable->disconnect = ags_rt_stream_channel_run_disconnect;
}

void
ags_rt_stream_channel_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_rt_stream_channel_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_rt_stream_channel_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_rt_stream_channel_run_disconnect_dynamic;
}

void
ags_rt_stream_channel_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_rt_stream_channel_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);
}

void
ags_rt_stream_channel_run_init(AgsRtStreamChannelRun *rt_stream_channel_run)
{
  AGS_RECALL(rt_stream_channel_run)->name = "ags-rt_stream";
  AGS_RECALL(rt_stream_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(rt_stream_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(rt_stream_channel_run)->xml_type = "ags-rt_stream-channel-run";
  AGS_RECALL(rt_stream_channel_run)->port = NULL;

  AGS_RECALL(rt_stream_channel_run)->flags |= AGS_RECALL_INPUT_ORIENTATED;
  AGS_RECALL(rt_stream_channel_run)->child_type = AGS_TYPE_RT_STREAM_RECYCLING;
}

void
ags_rt_stream_channel_run_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_rt_stream_channel_run_parent_class)->finalize(gobject);
}

void
ags_rt_stream_channel_run_connect(AgsConnectable *connectable)
{
  /* call parent */
  ags_rt_stream_channel_run_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_rt_stream_channel_run_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_rt_stream_channel_run_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_rt_stream_channel_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_rt_stream_channel_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_rt_stream_channel_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsChannel *channel;
  AgsRtStreamChannelRun *rt_stream_channel_run;

  ags_rt_stream_channel_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_rt_stream_channel_run_check_rt_stream(AgsRecall *recall)
{
  AgsChannel *source;
  AgsRecycling *first_recycling, *last_recycling;
  AgsRecycling *recycling, *end_recycling;

  AgsRtStreamChannelRun *rt_stream_channel_run;

  pthread_mutex_t *recycling_mutex;
  
  rt_stream_channel_run = recall;

  source = AGS_RECALL_CHANNEL_RUN(rt_stream_channel_run)->source;

  /* get first and last recycling */
  pthread_mutex_lock(source->obj_mutex);
  
  first_recycling = source->first_recycling;
  last_recycling = source->last_recycling;

  pthread_mutex_unlock(source->obj_mutex);

  /* get end */
  pthread_mutex_lock(last_recycling->obj_mutex);
  
  end_recycling = last_recycling->next;

  pthread_mutex_unlock(last_recycling->obj_mutex);

  /* */
  recycling = first_recycling;
  
  while(recycling != end_recycling){
    AgsAudioSignal *audio_signal;
    
    recycling_mutex = recycling->obj_mutex;

    pthread_mutex_lock(recycling_mutex);

    audio_signal = ags_audio_signal_new(recycling->soundcard,
					recycling,
					recall->recall_id);
    ags_audio_signal_stream_resize(audio_signal,
				   1);
    
    pthread_mutex_unlock(recycling_mutex);

    ags_recycling_add_audio_signal(recycling,
				   audio_signal);

    pthread_mutex_lock(recycling_mutex);
    
    recycling = recycling->next;
    
    pthread_mutex_unlock(recycling_mutex);
  }
}

AgsRecall*
ags_rt_stream_channel_run_duplicate(AgsRecall *recall,
				    AgsRecallID *recall_id,
				    guint *n_params, GParameter *parameter)
{
  AgsRtStreamChannelRun *copy;

  copy = (AgsRtStreamChannelRun *) AGS_RECALL_CLASS(ags_rt_stream_channel_run_parent_class)->duplicate(recall,
												       recall_id,
												       n_params, parameter);

  return((AgsRecall *) copy);
}

/**
 * ags_rt_stream_channel_run_new:
 *
 * Creates an #AgsRtStreamChannelRun
 *
 * Returns: a new #AgsRtStreamChannelRun
 *
 * Since: 1.4.0
 */
AgsRtStreamChannelRun*
ags_rt_stream_channel_run_new()
{
  AgsRtStreamChannelRun *rt_stream_channel_run;

  rt_stream_channel_run = (AgsRtStreamChannelRun *) g_object_new(AGS_TYPE_RT_STREAM_CHANNEL_RUN,
								 NULL);
  
  return(rt_stream_channel_run);
}
