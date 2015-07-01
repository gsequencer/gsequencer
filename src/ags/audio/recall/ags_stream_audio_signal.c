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

#include <ags/audio/recall/ags_stream_channel.h>
#include <ags/audio/recall/ags_stream_channel_run.h>
#include <ags/audio/recall/ags_stream_recycling.h>
#include <ags/audio/recall/ags_stream_audio_signal.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/task/ags_unref_audio_signal.h>

void ags_stream_audio_signal_class_init(AgsStreamAudioSignalClass *stream_audio_signal);
void ags_stream_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_stream_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_stream_audio_signal_init(AgsStreamAudioSignal *stream_audio_signal);
void ags_stream_audio_signal_connect(AgsConnectable *connectable);
void ags_stream_audio_signal_disconnect(AgsConnectable *connectable);
void ags_stream_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_stream_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_stream_audio_signal_finalize(GObject *gobject);

void ags_stream_audio_signal_run_init_pre(AgsRecall *recall);
void ags_stream_audio_signal_run_post(AgsRecall *recall);
AgsRecall* ags_stream_audio_signal_duplicate(AgsRecall *recall,
					     AgsRecallID *recall_id,
					     guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_stream_audio_signal
 * @short_description: streams audio signal
 * @title: AgsStreamAudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_stream_audio_signal.h
 *
 * The #AgsStreamAudioSignal class streams the audio signal.
 */

static gpointer ags_stream_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_stream_audio_signal_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_stream_audio_signal_parent_dynamic_connectable_interface;

GType
ags_stream_audio_signal_get_type()
{
  static GType ags_type_stream_audio_signal = 0;

  if(!ags_type_stream_audio_signal){
    static const GTypeInfo ags_stream_audio_signal_info = {
      sizeof (AgsStreamAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_stream_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStreamAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_stream_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_stream_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_stream_audio_signal_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_stream_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							  "AgsStreamAudioSignal\0",
							  &ags_stream_audio_signal_info,
							  0);

    g_type_add_interface_static(ags_type_stream_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_stream_audio_signal,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return (ags_type_stream_audio_signal);
}

void
ags_stream_audio_signal_class_init(AgsStreamAudioSignalClass *stream_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_stream_audio_signal_parent_class = g_type_class_peek_parent(stream_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) stream_audio_signal;

  gobject->finalize = ags_stream_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) stream_audio_signal;

  recall->run_init_pre = ags_stream_audio_signal_run_init_pre;
  recall->run_post = ags_stream_audio_signal_run_post;
  recall->duplicate = ags_stream_audio_signal_duplicate;
}

void
ags_stream_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_stream_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_stream_audio_signal_connect;
  connectable->disconnect = ags_stream_audio_signal_disconnect;
}

void
ags_stream_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_stream_audio_signal_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_stream_audio_signal_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_stream_audio_signal_disconnect_dynamic;
}

void
ags_stream_audio_signal_init(AgsStreamAudioSignal *stream_audio_signal)
{
  AGS_RECALL(stream_audio_signal)->name = "ags-stream\0";
  AGS_RECALL(stream_audio_signal)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(stream_audio_signal)->build_id = AGS_BUILD_ID;
  AGS_RECALL(stream_audio_signal)->xml_type = "ags-stream-audio-signal\0";
  AGS_RECALL(stream_audio_signal)->port = NULL;

  AGS_RECALL(stream_audio_signal)->child_type = G_TYPE_NONE;
}

void
ags_stream_audio_signal_finalize(GObject *gobject)
{
  if(AGS_RECALL_AUDIO_SIGNAL(gobject)->source != NULL &&
     AGS_RECALL(gobject)->parent != NULL){
    ags_recycling_remove_audio_signal(AGS_RECALL_RECYCLING(AGS_RECALL(gobject)->parent)->source,
				      AGS_RECALL_AUDIO_SIGNAL(gobject)->source);
  }
  //  g_object_unref(AGS_RECALL_AUDIO_SIGNAL(gobject)->source);	
  //  g_object_unref(AGS_RECALL_AUDIO_SIGNAL(gobject)->source);	

  /* call parent */
  G_OBJECT_CLASS(ags_stream_audio_signal_parent_class)->finalize(gobject); 
}

void
ags_stream_audio_signal_connect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_stream_audio_signal_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_stream_audio_signal_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_stream_audio_signal_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_stream_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(dynamic_connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_stream_audio_signal_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_stream_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_stream_audio_signal_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_stream_audio_signal_run_init_pre(AgsRecall *recall)
{
  /* call parent */
  AGS_RECALL_CLASS(ags_stream_audio_signal_parent_class)->run_init_pre(recall);

  //  g_message("stream\0");
}

void
ags_stream_audio_signal_run_post(AgsRecall *recall)
{
  AgsStreamChannel *stream_channel;
  AgsStreamChannelRun *stream_channel_run;
  AgsStreamAudioSignal *stream_audio_signal;

  stream_audio_signal = AGS_STREAM_AUDIO_SIGNAL(recall);
  
  if(recall->parent != NULL &&
     recall->parent->parent != NULL){
    stream_channel_run = (AgsStreamChannelRun *) recall->parent->parent;
    stream_channel = (AgsStreamChannel *) AGS_RECALL_CHANNEL_RUN(stream_channel_run)->recall_channel;
  }else{
    stream_channel_run = NULL;
    stream_channel = NULL;
  }
  
  if(AGS_RECALL_AUDIO_SIGNAL(recall)->source->stream_current != NULL){
    if(stream_channel != NULL &&
       AGS_RECALL_AUDIO_SIGNAL(recall)->source->stream_current->next == NULL){
      GValue value = {0,};
      
      g_value_init(&value, G_TYPE_BOOLEAN);
      ags_port_safe_read(stream_channel->auto_sense,
			 &value);

      if(g_value_get_boolean(&value)){
	signed short *buffer;
	guint buffer_size;
	guint i;
	gboolean add_stream;

	buffer = AGS_RECALL_AUDIO_SIGNAL(recall)->source->stream_current->data;
	
	buffer_size = AGS_RECALL_AUDIO_SIGNAL(recall)->source->buffer_size;
	add_stream = FALSE;
	
	for(i = buffer_size - 1; i > buffer_size / 2; i--){
	  if(buffer[i] != 0){
	    add_stream = TRUE;
	    break;
	  }
	}
	
	if(add_stream){
	  ags_audio_signal_add_stream(AGS_RECALL_AUDIO_SIGNAL(recall)->source);
	}
      }
    }

    //    g_message("stream %x %x\0", AGS_RECALL_AUDIO_SIGNAL(recall)->source, AGS_RECALL_ID(AGS_RECALL_AUDIO_SIGNAL(recall)->source->recall_id)->recycling_container);
    AGS_RECALL_AUDIO_SIGNAL(recall)->source->stream_current = AGS_RECALL_AUDIO_SIGNAL(recall)->source->stream_current->next;

    /* call parent */
    AGS_RECALL_CLASS(ags_stream_audio_signal_parent_class)->run_post(recall);
  }else{
    /* call parent */
    AGS_RECALL_CLASS(ags_stream_audio_signal_parent_class)->run_post(recall);

    if(recall->parent != NULL){
      ags_recycling_remove_audio_signal(AGS_RECALL_RECYCLING(recall->parent)->source,
					AGS_RECALL_AUDIO_SIGNAL(recall)->source);
    }

    ags_recall_done(recall);
  }
}

AgsRecall*
ags_stream_audio_signal_duplicate(AgsRecall *recall,
				  AgsRecallID *recall_id,
				  guint *n_params, GParameter *parameter)
{
  AgsStreamAudioSignal *copy;

  copy = (AgsStreamAudioSignal *) AGS_RECALL_CLASS(ags_stream_audio_signal_parent_class)->duplicate(recall,
												    recall_id,
												    n_params, parameter);

  return((AgsRecall *) copy);
}

/**
 * ags_stream_audio_signal_new:
 * @audio_signal: an #AgsAudioSignal
 *
 * Creates an #AgsStreamAudioSignal
 *
 * Returns: a new #AgsStreamAudioSignal
 *
 * Since: 0.4
 */
AgsStreamAudioSignal*
ags_stream_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsStreamAudioSignal *stream_audio_signal;

  stream_audio_signal = (AgsStreamAudioSignal *) g_object_new(AGS_TYPE_STREAM_AUDIO_SIGNAL,
							      "source\0", audio_signal,
							      NULL);

  return(stream_audio_signal);
}
