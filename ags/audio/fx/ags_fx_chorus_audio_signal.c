/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_chorus_audio_signal.h>

#include <ags/audio/ags_port.h>

#include <ags/audio/fx/ags_fx_chorus_audio.h>
#include <ags/audio/fx/ags_fx_chorus_channel.h>
#include <ags/audio/fx/ags_fx_chorus_channel_processor.h>
#include <ags/audio/fx/ags_fx_chorus_recycling.h>

#include <ags/i18n.h>

void ags_fx_chorus_audio_signal_class_init(AgsFxChorusAudioSignalClass *fx_chorus_audio_signal);
void ags_fx_chorus_audio_signal_init(AgsFxChorusAudioSignal *fx_chorus_audio_signal);
void ags_fx_chorus_audio_signal_dispose(GObject *gobject);
void ags_fx_chorus_audio_signal_finalize(GObject *gobject);

void ags_fx_chorus_audio_signal_real_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_chorus_audio_signal
 * @short_description: fx chorus audio signal
 * @title: AgsFxChorusAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_chorus_audio_signal.h
 *
 * The #AgsFxChorusAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_chorus_audio_signal_parent_class = NULL;

const gchar *ags_fx_chorus_audio_signal_plugin_name = "ags-fx-chorus";

GType
ags_fx_chorus_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_chorus_audio_signal = 0;

    static const GTypeInfo ags_fx_chorus_audio_signal_info = {
      sizeof (AgsFxChorusAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_chorus_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxChorusAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_chorus_audio_signal_init,
    };

    ags_type_fx_chorus_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							     "AgsFxChorusAudioSignal",
							     &ags_fx_chorus_audio_signal_info,
							     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_chorus_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_chorus_audio_signal_class_init(AgsFxChorusAudioSignalClass *fx_chorus_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_chorus_audio_signal_parent_class = g_type_class_peek_parent(fx_chorus_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_chorus_audio_signal;

  gobject->dispose = ags_fx_chorus_audio_signal_dispose;
  gobject->finalize = ags_fx_chorus_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_chorus_audio_signal;
  
  recall->run_inter = ags_fx_chorus_audio_signal_real_run_inter;
}

void
ags_fx_chorus_audio_signal_init(AgsFxChorusAudioSignal *fx_chorus_audio_signal)
{
  AGS_RECALL(fx_chorus_audio_signal)->name = "ags-fx-chorus";
  AGS_RECALL(fx_chorus_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_chorus_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_chorus_audio_signal)->xml_type = "ags-fx-chorus-audio-signal";

  fx_chorus_audio_signal->chorus_util.hq_pitch_util = &(fx_chorus_audio_signal->hq_pitch_util);
  fx_chorus_audio_signal->chorus_util.hq_pitch_util->linear_interpolate_util = &(fx_chorus_audio_signal->linear_interpolate_util);
}

void
ags_fx_chorus_audio_signal_dispose(GObject *gobject)
{
  AgsFxChorusAudioSignal *fx_chorus_audio_signal;
  
  fx_chorus_audio_signal = AGS_FX_CHORUS_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_chorus_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_chorus_audio_signal_finalize(GObject *gobject)
{
  AgsFxChorusAudioSignal *fx_chorus_audio_signal;
  
  fx_chorus_audio_signal = AGS_FX_CHORUS_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_chorus_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_chorus_audio_signal_real_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;
  AgsFxChorusAudio *fx_chorus_audio;
  AgsFxChorusChannel *fx_chorus_channel;
  AgsFxChorusChannelProcessor *fx_chorus_channel_processor;
  AgsFxChorusRecycling *fx_chorus_recycling;
  AgsFxChorusAudioSignal *fx_chorus_audio_signal;
  
  guint buffer_size;
  guint format;
  guint samplerate;
  gboolean enabled;
  gdouble input_volume;
  gdouble output_volume;
  guint lfo_oscillator;
  gdouble lfo_frequency;
  gdouble depth;
  gdouble mix;
  gdouble delay;
  
  GRecMutex *stream_mutex;
  
  source = NULL;

  fx_chorus_audio = NULL;
  fx_chorus_channel = NULL;
  fx_chorus_channel_processor = NULL;
  fx_chorus_recycling = NULL;
  fx_chorus_audio_signal = (AgsFxChorusAudioSignal *) recall;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  enabled = FALSE;

  input_volume = 1.0;
  output_volume = 1.0;

  lfo_oscillator = AGS_SYNTH_OSCILLATOR_SIN;
  lfo_frequency = 10.0;

  depth = 0.0;
  mix = 0.0;
  delay = 0.0;  

  g_object_get(recall,
	       "parent", &fx_chorus_recycling,
	       "source", &source,
	       NULL);

  g_object_get(fx_chorus_recycling,
	       "parent", &fx_chorus_channel_processor,
	       NULL);

  g_object_get(fx_chorus_channel_processor,
	       "recall-audio", &fx_chorus_audio,
	       "recall-channel", &fx_chorus_channel,
	       NULL);

  g_object_get(source,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       "samplerate", &samplerate,
	       NULL);

  if(fx_chorus_channel != NULL){
    AgsPort *port;

    GValue value = {0,};
        
    /* enabled */    
    g_object_get(fx_chorus_channel,
		 "enabled", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);
      
      enabled = (g_value_get_float(&value) == 0.0) ? FALSE: TRUE;

      g_object_unref(port);
    }

    g_value_unset(&value);

    /* input-volume */    
    g_object_get(fx_chorus_channel,
		 "input-volume", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);
      
      input_volume = g_value_get_float(&value);

      g_object_unref(port);
    }

    g_value_unset(&value);

    /* output-volume */    
    g_object_get(fx_chorus_channel,
		 "output-volume", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);
      
      output_volume = g_value_get_float(&value);

      g_object_unref(port);
    }

    g_value_unset(&value);

    /* lfo-oscillator */    
    g_object_get(fx_chorus_channel,
		 "lfo-oscillator", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);
      
      lfo_oscillator = (guint) g_value_get_float(&value);

      g_object_unref(port);
    }

    g_value_unset(&value);

    /* lfo-frequency */    
    g_object_get(fx_chorus_channel,
		 "lfo-frequency", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);
      
      lfo_frequency = g_value_get_float(&value);

      g_object_unref(port);
    }

    g_value_unset(&value);

    /* depth */    
    g_object_get(fx_chorus_channel,
		 "depth", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);
      
      depth = g_value_get_float(&value);

      g_object_unref(port);
    }

    g_value_unset(&value);

    /* mix */    
    g_object_get(fx_chorus_channel,
		 "mix", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);
      
      mix = (g_value_get_float(&value) == 0.0) ? FALSE: TRUE;

      g_object_unref(port);
    }

    g_value_unset(&value);

    /* delay */    
    g_object_get(fx_chorus_channel,
		 "delay", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);
      
      delay = (g_value_get_float(&value) == 0.0) ? FALSE: TRUE;

      g_object_unref(port);
    }

    g_value_unset(&value);
  }
  
  if(enabled &&
     source != NULL &&
     source->stream_current != NULL){
    stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);
    
    fx_chorus_audio_signal->chorus_util.destination = source->stream_current->data;
    fx_chorus_audio_signal->chorus_util.destination_stride = 1;
	
    fx_chorus_audio_signal->chorus_util.source = source->stream_current->data;
    fx_chorus_audio_signal->chorus_util.source_stride = 1;
	
    fx_chorus_audio_signal->chorus_util.buffer_length = buffer_size;
    fx_chorus_audio_signal->chorus_util.format = format;
    fx_chorus_audio_signal->chorus_util.samplerate = samplerate;
	
    fx_chorus_audio_signal->chorus_util.input_volume = input_volume;
    fx_chorus_audio_signal->chorus_util.output_volume = output_volume;

    fx_chorus_audio_signal->chorus_util.lfo_oscillator = lfo_oscillator;      
    fx_chorus_audio_signal->chorus_util.lfo_frequency = lfo_frequency;
      
    fx_chorus_audio_signal->chorus_util.depth = depth;
    fx_chorus_audio_signal->chorus_util.mix = mix;
    fx_chorus_audio_signal->chorus_util.delay = delay;

    g_rec_mutex_lock(stream_mutex);

    ags_chorus_util_compute(&(fx_chorus_audio_signal->chorus_util));
    
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
  
  if(fx_chorus_audio != NULL){
    g_object_unref(fx_chorus_audio);
  }
  
  if(fx_chorus_channel != NULL){
    g_object_unref(fx_chorus_channel);
  }

  if(fx_chorus_channel_processor != NULL){
    g_object_unref(fx_chorus_channel_processor);
  }

  if(fx_chorus_recycling != NULL){
    g_object_unref(fx_chorus_recycling);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_chorus_audio_signal_parent_class)->run_inter(recall);
}

/**
 * ags_fx_chorus_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxChorusAudioSignal
 *
 * Returns: the new #AgsFxChorusAudioSignal
 *
 * Since: 3.14.0
 */
AgsFxChorusAudioSignal*
ags_fx_chorus_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxChorusAudioSignal *fx_chorus_audio_signal;

  fx_chorus_audio_signal = (AgsFxChorusAudioSignal *) g_object_new(AGS_TYPE_FX_CHORUS_AUDIO_SIGNAL,
								   "source", audio_signal,
								   NULL);

  return(fx_chorus_audio_signal);
}
