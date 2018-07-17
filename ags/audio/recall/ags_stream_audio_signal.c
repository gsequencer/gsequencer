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

#include <ags/audio/recall/ags_stream_channel.h>

#include <ags/libags.h>

#include <ags/audio/recall/ags_stream_channel_run.h>
#include <ags/audio/recall/ags_stream_recycling.h>
#include <ags/audio/recall/ags_stream_audio_signal.h>

#include <ags/audio/task/ags_unref_audio_signal.h>

void ags_stream_audio_signal_class_init(AgsStreamAudioSignalClass *stream_audio_signal);
void ags_stream_audio_signal_init(AgsStreamAudioSignal *stream_audio_signal);
void ags_stream_audio_signal_dispose(GObject *gobject);
void ags_stream_audio_signal_finalize(GObject *gobject);

void ags_stream_audio_signal_run_post(AgsRecall *recall);

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

    ags_type_stream_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							  "AgsStreamAudioSignal",
							  &ags_stream_audio_signal_info,
							  0);
  }

  return(ags_type_stream_audio_signal);
}

void
ags_stream_audio_signal_class_init(AgsStreamAudioSignalClass *stream_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_stream_audio_signal_parent_class = g_type_class_peek_parent(stream_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) stream_audio_signal;

  gobject->dispose = ags_stream_audio_signal_dispose;
  gobject->finalize = ags_stream_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) stream_audio_signal;

  recall->run_post = ags_stream_audio_signal_run_post;
}

void
ags_stream_audio_signal_init(AgsStreamAudioSignal *stream_audio_signal)
{
  AGS_RECALL(stream_audio_signal)->name = "ags-stream";
  AGS_RECALL(stream_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(stream_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(stream_audio_signal)->xml_type = "ags-stream-audio-signal";
  AGS_RECALL(stream_audio_signal)->port = NULL;

  AGS_RECALL(stream_audio_signal)->child_type = G_TYPE_NONE;

  stream_audio_signal->dispose_source = NULL;
}

void
ags_stream_audio_signal_dispose(GObject *gobject)
{
  AGS_STREAM_AUDIO_SIGNAL(gobject)->dispose_source = AGS_RECALL_AUDIO_SIGNAL(gobject)->source;

  /* call parent */
  G_OBJECT_CLASS(ags_stream_audio_signal_parent_class)->dispose(gobject); 
}

void
ags_stream_audio_signal_finalize(GObject *gobject)
{
  AgsAudioSignal *audio_signal;

  audio_signal = AGS_STREAM_AUDIO_SIGNAL(gobject)->dispose_source;
  
  if(audio_signal != NULL){
    AgsRecycling *recycling;

    recycling = audio_signal->recycling;
    
    if(recycling != NULL){
      ags_recycling_remove_audio_signal(recycling,
					audio_signal);
    }
    
    g_object_run_dispose(audio_signal);
    g_object_unref(audio_signal);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_stream_audio_signal_parent_class)->finalize(gobject); 
}

void
ags_stream_audio_signal_run_post(AgsRecall *recall)
{
  AgsAudioSignal *source;
  AgsStreamChannel *stream_channel;
  AgsStreamChannelRun *stream_channel_run;
  AgsStreamRecycling *stream_recycling;
  AgsStreamAudioSignal *stream_audio_signal;
  
  void (*parent_class_run_post)(AgsRecall *recall);

  stream_audio_signal = (AgsStreamAudioSignal *) recall;
  
  /* get parent class */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  parent_class_run_post = AGS_RECALL_CLASS(ags_stream_audio_signal_parent_class)->run_post;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  g_object_get(stream_audio_signal,
	       "parent", &stream_recycling,
	       "source", &source,
	       NULL);

  g_object_get(stream_recycling,
	       "parent", &stream_channel_run,
	       NULL);

  g_object_get(stream_channel_run,
	       "recall-channel", &stream_channel,
	       NULL);
  
  if(source->stream_current != NULL){
    if(source->stream_current->next == NULL){
      AgsPort *port;

      gboolean auto_sense;
      
      GValue value = {0,};

      g_object_get(stream_channel,
		   "auto-sense", &port,
		   NULL);
      
      g_value_init(&value,
		   G_TYPE_BOOLEAN);
      
      ags_port_safe_read(port,
			 &value);

      auto_sense = g_value_get_boolean(&value);

      g_value_unset(&value);
      
      if(auto_sense){
	void *buffer;
	guint buffer_size;
	guint format;
	guint i;
	gboolean add_stream;

	buffer = source->stream_current->data;

	g_object_get(source,
		     "buffer-size", &buffer_size,
		     "format", &format,
		     NULL);
	
	add_stream = FALSE;
	
	for(i = buffer_size - 1; i > buffer_size / 2 && !add_stream; i--){
	  switch(format){
	  case AGS_SOUNDCARD_SIGNED_8_BIT:
	    {
	      if(((gint8 *) buffer)[i] != 0){
		add_stream = TRUE;
	      }
	    }
	    break;
	  case AGS_SOUNDCARD_SIGNED_16_BIT:
	    {
	      if(((gint16 *) buffer)[i] != 0){
		add_stream = TRUE;
	      }
	    }
	    break;
	  case AGS_SOUNDCARD_SIGNED_24_BIT:
	    {
	      if(((gint32 *) buffer)[i] != 0){
		add_stream = TRUE;
	      }
	    }
	    break;
	  case AGS_SOUNDCARD_SIGNED_32_BIT:
	    {
	      if(((gint32 *) buffer)[i] != 0){
		add_stream = TRUE;
	      }
	    }
	    break;
	  case AGS_SOUNDCARD_SIGNED_64_BIT:
	    {
	      if(((gint64 *) buffer)[i] != 0){
		add_stream = TRUE;
	      }
	    }
	    break;
	  case AGS_SOUNDCARD_FLOAT:
	    {
	      if(((gfloat *) buffer)[i] != 0.0){
		add_stream = TRUE;
	      }
	    }
	    break;
	  case AGS_SOUNDCARD_DOUBLE:
	    {
	      if(((gdouble *) buffer)[i] != 0.0){
		add_stream = TRUE;
	      }
	    }
	    break;
	  default:
	    g_critical("unsupported soundcard format");
	  }
	}
	
	if(add_stream){
	  ags_audio_signal_add_stream(source);
	}
      }

      g_value_unset(&value);
    }

    source->stream_current = source->stream_current->next;

    /* call parent */
    parent_class_run_post(recall);
  }else{
    /* call parent */
    parent_class_run_post(recall);

    ags_recall_done(recall);
  }
}

/**
 * ags_stream_audio_signal_new:
 * @source: the #AgsAudioSignal
 *
 * Create a new instance of #AgsStreamAudioSignal
 *
 * Returns: the new #AgsStreamAudioSignal
 *
 * Since: 2.0.0
 */
AgsStreamAudioSignal*
ags_stream_audio_signal_new(AgsAudioSignal *source)
{
  AgsStreamAudioSignal *stream_audio_signal;

  stream_audio_signal = (AgsStreamAudioSignal *) g_object_new(AGS_TYPE_STREAM_AUDIO_SIGNAL,
							      "source", source,
							      NULL);

  return(stream_audio_signal);
}
