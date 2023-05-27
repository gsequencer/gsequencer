/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_tremolo_audio_signal.h>

#include <ags/audio/ags_port.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_tremolo_util.h>

#include <ags/audio/fx/ags_fx_tremolo_audio.h>
#include <ags/audio/fx/ags_fx_tremolo_channel.h>
#include <ags/audio/fx/ags_fx_tremolo_channel_processor.h>
#include <ags/audio/fx/ags_fx_tremolo_recycling.h>

#include <ags/i18n.h>

void ags_fx_tremolo_audio_signal_class_init(AgsFxTremoloAudioSignalClass *fx_tremolo_audio_signal);
void ags_fx_tremolo_audio_signal_init(AgsFxTremoloAudioSignal *fx_tremolo_audio_signal);
void ags_fx_tremolo_audio_signal_dispose(GObject *gobject);
void ags_fx_tremolo_audio_signal_finalize(GObject *gobject);

void ags_fx_tremolo_audio_signal_real_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_tremolo_audio_signal
 * @short_description: fx tremolo audio signal
 * @title: AgsFxTremoloAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_tremolo_audio_signal.h
 *
 * The #AgsFxTremoloAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_tremolo_audio_signal_parent_class = NULL;

const gchar *ags_fx_tremolo_audio_signal_plugin_name = "ags-fx-tremolo";

GType
ags_fx_tremolo_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_tremolo_audio_signal = 0;

    static const GTypeInfo ags_fx_tremolo_audio_signal_info = {
      sizeof (AgsFxTremoloAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_tremolo_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxTremoloAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_tremolo_audio_signal_init,
    };

    ags_type_fx_tremolo_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							      "AgsFxTremoloAudioSignal",
							      &ags_fx_tremolo_audio_signal_info,
							      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_tremolo_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_tremolo_audio_signal_class_init(AgsFxTremoloAudioSignalClass *fx_tremolo_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_tremolo_audio_signal_parent_class = g_type_class_peek_parent(fx_tremolo_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_tremolo_audio_signal;

  gobject->dispose = ags_fx_tremolo_audio_signal_dispose;
  gobject->finalize = ags_fx_tremolo_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_tremolo_audio_signal;
  
  recall->run_inter = ags_fx_tremolo_audio_signal_real_run_inter;
}

void
ags_fx_tremolo_audio_signal_init(AgsFxTremoloAudioSignal *fx_tremolo_audio_signal)
{
  AGS_RECALL(fx_tremolo_audio_signal)->name = "ags-fx-tremolo";
  AGS_RECALL(fx_tremolo_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_tremolo_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_tremolo_audio_signal)->xml_type = "ags-fx-tremolo-audio-signal";
}

void
ags_fx_tremolo_audio_signal_dispose(GObject *gobject)
{
  AgsFxTremoloAudioSignal *fx_tremolo_audio_signal;
  
  fx_tremolo_audio_signal = AGS_FX_TREMOLO_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_tremolo_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_tremolo_audio_signal_finalize(GObject *gobject)
{
  AgsFxTremoloAudioSignal *fx_tremolo_audio_signal;
  
  fx_tremolo_audio_signal = AGS_FX_TREMOLO_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_tremolo_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_tremolo_audio_signal_real_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;
  AgsFxTremoloAudio *fx_tremolo_audio;
  AgsFxTremoloChannel *fx_tremolo_channel;
  AgsFxTremoloChannelProcessor *fx_tremolo_channel_processor;
  AgsFxTremoloRecycling *fx_tremolo_recycling;
  AgsFxTremoloAudioSignal *fx_tremolo_audio_signal;
  
  guint buffer_size;
  guint format;
  gdouble tremolo_gain;
  gdouble tremolo_lfo_freq;

  GRecMutex *stream_mutex;
  
  source = NULL;

  fx_tremolo_audio = NULL;
  fx_tremolo_channel = NULL;
  fx_tremolo_channel_processor = NULL;
  fx_tremolo_recycling = NULL;
  fx_tremolo_audio_signal = (AgsFxTremoloAudioSignal *) recall;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  tremolo_gain = 1.0;
  tremolo_lfo_freq = 6.0;

  g_object_get(recall,
	       "parent", &fx_tremolo_recycling,
	       "source", &source,
	       NULL);

  g_object_get(fx_tremolo_recycling,
	       "parent", &fx_tremolo_channel_processor,
	       NULL);

  g_object_get(fx_tremolo_channel_processor,
	       "recall-audio", &fx_tremolo_audio,
	       "recall-channel", &fx_tremolo_channel,
	       NULL);

  g_object_get(source,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);
  
  if(fx_tremolo_audio != NULL){
    AgsPort *port;

    GValue value = {0,};
    
    /* tremolo gain */    
    g_object_get(fx_tremolo_audio,
		 "tremolo-gain", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      tremolo_gain = g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);
  }

  if(fx_tremolo_audio != NULL){
    AgsPort *port;

    GValue value = {0,};
    
    /* tremolo LFO freq */
    g_object_get(fx_tremolo_audio,
		 "tremolo-lfo-freq", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      tremolo_lfo_freq = g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);
  }
  
  if(source != NULL &&
     source->stream_current != NULL){
    stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);
    
    fx_tremolo_audio_signal->tremolo_util.destination = source->stream_current->data;
    fx_tremolo_audio_signal->tremolo_util.destination_stride = 1;
	
    fx_tremolo_audio_signal->tremolo_util.source = source->stream_current->data;
    fx_tremolo_audio_signal->tremolo_util.source_stride = 1;
	
    fx_tremolo_audio_signal->tremolo_util.buffer_length = buffer_size;
    fx_tremolo_audio_signal->tremolo_util.format = format;
	
    fx_tremolo_audio_signal->tremolo_util.tremolo_gain = tremolo_gain;
    fx_tremolo_audio_signal->tremolo_util.tremolo_lfo_freq = tremolo_lfo_freq;
      
    g_rec_mutex_lock(stream_mutex);

    ags_tremolo_util_compute(&(fx_tremolo_audio_signal->tremolo_util));
    
    g_rec_mutex_unlock(stream_mutex);
  }

  if(source == NULL ||
     source->stream_current == NULL){
    ags_recall_done(recall);
  }

  /* unref */
  if(source != NULL){
    g_object_unref(source);
  }
  
  if(fx_tremolo_audio != NULL){
    g_object_unref(fx_tremolo_audio);
  }
  
  if(fx_tremolo_channel != NULL){
    g_object_unref(fx_tremolo_channel);
  }

  if(fx_tremolo_channel_processor != NULL){
    g_object_unref(fx_tremolo_channel_processor);
  }

  if(fx_tremolo_recycling != NULL){
    g_object_unref(fx_tremolo_recycling);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_tremolo_audio_signal_parent_class)->run_inter(recall);
}

/**
 * ags_fx_tremolo_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxTremoloAudioSignal
 *
 * Returns: the new #AgsFxTremoloAudioSignal
 *
 * Since: 5.2.0
 */
AgsFxTremoloAudioSignal*
ags_fx_tremolo_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxTremoloAudioSignal *fx_tremolo_audio_signal;

  fx_tremolo_audio_signal = (AgsFxTremoloAudioSignal *) g_object_new(AGS_TYPE_FX_TREMOLO_AUDIO_SIGNAL,
								     "source", audio_signal,
								     NULL);

  return(fx_tremolo_audio_signal);
}
