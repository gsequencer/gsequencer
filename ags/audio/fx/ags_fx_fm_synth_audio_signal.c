/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_fm_synth_audio_signal.h>

#include <ags/audio/ags_port.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/fx/ags_fx_fm_synth_audio.h>
#include <ags/audio/fx/ags_fx_fm_synth_audio_processor.h>
#include <ags/audio/fx/ags_fx_fm_synth_channel.h>
#include <ags/audio/fx/ags_fx_fm_synth_channel_processor.h>
#include <ags/audio/fx/ags_fx_fm_synth_recycling.h>

#include <ags/i18n.h>

void ags_fx_fm_synth_audio_signal_class_init(AgsFxFMSynthAudioSignalClass *fx_fm_synth_audio_signal);
void ags_fx_fm_synth_audio_signal_init(AgsFxFMSynthAudioSignal *fx_fm_synth_audio_signal);
void ags_fx_fm_synth_audio_signal_dispose(GObject *gobject);
void ags_fx_fm_synth_audio_signal_finalize(GObject *gobject);

void ags_fx_fm_synth_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					      AgsAudioSignal *source,
					      AgsNote *note,
					      gboolean pattern_mode,
					      guint x0, guint x1,
					      guint y,
					      gdouble delay_counter, guint64 offset_counter,
					      guint frame_count,
					      gdouble delay, guint buffer_size);
void ags_fx_fm_synth_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
						AgsAudioSignal *source,
						AgsNote *note,
						guint x0, guint x1,
						guint y);

/**
 * SECTION:ags_fx_fm_synth_audio_signal
 * @short_description: fx FM synth audio signal
 * @title: AgsFxFMSynthAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_fm_synth_audio_signal.h
 *
 * The #AgsFxFMSynthAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_fm_synth_audio_signal_parent_class = NULL;

const gchar *ags_fx_fm_synth_audio_signal_plugin_name = "ags-fx-fm-synth";

GType
ags_fx_fm_synth_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_fm_synth_audio_signal = 0;

    static const GTypeInfo ags_fx_fm_synth_audio_signal_info = {
      sizeof (AgsFxFMSynthAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_fm_synth_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxFMSynthAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_fm_synth_audio_signal_init,
    };

    ags_type_fx_fm_synth_audio_signal = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL,
							       "AgsFxFMSynthAudioSignal",
							       &ags_fx_fm_synth_audio_signal_info,
							       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_fm_synth_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_fm_synth_audio_signal_class_init(AgsFxFMSynthAudioSignalClass *fx_fm_synth_audio_signal)
{
  GObjectClass *gobject;
  AgsFxNotationAudioSignalClass *fx_notation_audio_signal;

  ags_fx_fm_synth_audio_signal_parent_class = g_type_class_peek_parent(fx_fm_synth_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_fm_synth_audio_signal;

  gobject->dispose = ags_fx_fm_synth_audio_signal_dispose;
  gobject->finalize = ags_fx_fm_synth_audio_signal_finalize;

  /* AgsFxNotationAudioSignalClass */
  fx_notation_audio_signal = (AgsFxNotationAudioSignalClass *) fx_fm_synth_audio_signal;
  
  fx_notation_audio_signal->stream_feed = ags_fx_fm_synth_audio_signal_stream_feed;
  fx_notation_audio_signal->notify_remove = ags_fx_fm_synth_audio_signal_notify_remove;
}

void
ags_fx_fm_synth_audio_signal_init(AgsFxFMSynthAudioSignal *fx_fm_synth_audio_signal)
{
  AGS_RECALL(fx_fm_synth_audio_signal)->name = "ags-fx-fm-synth";
  AGS_RECALL(fx_fm_synth_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_fm_synth_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_fm_synth_audio_signal)->xml_type = "ags-fx-fm-synth-audio-signal";
}

void
ags_fx_fm_synth_audio_signal_dispose(GObject *gobject)
{
  AgsFxFMSynthAudioSignal *fx_fm_synth_audio_signal;
  
  fx_fm_synth_audio_signal = AGS_FX_FM_SYNTH_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_fm_synth_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_fm_synth_audio_signal_finalize(GObject *gobject)
{
  AgsFxFMSynthAudioSignal *fx_fm_synth_audio_signal;
  
  fx_fm_synth_audio_signal = AGS_FX_FM_SYNTH_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_fm_synth_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_fm_synth_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					 AgsAudioSignal *source,
					 AgsNote *note,
					 gboolean pattern_mode,
					 guint x0, guint x1,
					 guint y,
					 gdouble delay_counter, guint64 offset_counter,
					 guint frame_count,
					 gdouble delay, guint buffer_size)
{
  AgsAudio *audio;
  AgsFxFMSynthAudio *fx_fm_synth_audio;
  AgsFxFMSynthAudioProcessor *fx_fm_synth_audio_processor;
  AgsFxFMSynthChannelProcessor *fx_fm_synth_channel_processor;
  AgsFxFMSynthRecycling *fx_fm_synth_recycling;
  AgsFxFMSynthAudioSignal *fx_fm_synth_audio_signal;
  
  guint sound_scope;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;
  guint format;
  guint samplerate;
  guint copy_mode_out;
  guint audio_buffer_util_format;
  guint copy_mode;
  
  gboolean sequencer_enabled;
  gint sequencer_sign;
  
  gboolean low_pass_enabled;

  gboolean high_pass_enabled;

  gboolean chorus_enabled;

  GRecMutex *source_stream_mutex;
  GRecMutex *fx_fm_synth_audio_mutex;

  audio = NULL;
  
  fx_fm_synth_audio = NULL;
  fx_fm_synth_audio_processor = NULL;
  
  fx_fm_synth_channel_processor = NULL;

  fx_fm_synth_recycling = NULL;

  sound_scope = ags_recall_get_sound_scope(fx_notation_audio_signal);

  audio_channel = 0;
  
  audio_start_mapping = 0;
  midi_start_mapping = 0;

  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  sequencer_enabled = FALSE;
  sequencer_sign = 0;

  low_pass_enabled = FALSE;

  high_pass_enabled = FALSE;

  chorus_enabled = FALSE;
  
  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_fm_synth_recycling,
	       NULL);

  g_object_get(fx_fm_synth_recycling,
	       "parent", &fx_fm_synth_channel_processor,
	       NULL);
  
  g_object_get(fx_fm_synth_channel_processor,
	       "recall-audio", &fx_fm_synth_audio,
	       "recall-audio-run", &fx_fm_synth_audio_processor,
	       NULL);

  g_object_get(fx_fm_synth_audio,
	       "audio", &audio,
	       NULL);

  g_object_get(fx_fm_synth_audio_processor,
	       "audio-channel", &audio_channel,
	       NULL);
  
  g_object_get(source,
	       "format", &format,
	       "samplerate", &samplerate,
	       NULL);

  audio_buffer_util_format = ags_audio_buffer_util_format_from_soundcard(format);

  copy_mode = ags_audio_buffer_util_get_copy_mode(audio_buffer_util_format,
						  audio_buffer_util_format);
  
  /* get synth mutex */
  fx_fm_synth_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_fm_synth_audio);

  /* process data */
  source_stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);

  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
    midi_note = (128 - y - 1 - audio_start_mapping + midi_start_mapping);
  }else{
    midi_note = (y - audio_start_mapping + midi_start_mapping);
  }

  if(fx_fm_synth_audio != NULL){
    AgsPort *port;

    AgsFxFMSynthAudioChannelData *channel_data;

    gdouble octave;
    gdouble key;
    
    GValue value = {0,};

    channel_data = fx_fm_synth_audio->scope_data[sound_scope]->channel_data[audio_channel];
    
    /* synth-0 oscillator */    
    g_object_get(fx_fm_synth_audio,
		 "synth-0-oscillator", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_synth_oscillator_mode(channel_data->synth_0,
						  (guint) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* synth-0 octave */
    octave = 0.0;
    key = 0.0;
    
    g_object_get(fx_fm_synth_audio,
		 "synth-0-octave", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      octave = (gdouble) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* synth-0 key */    
    g_object_get(fx_fm_synth_audio,
		 "synth-0-key", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      key = (gdouble) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    ags_fm_synth_util_set_frequency(channel_data->synth_0,
				    exp2(((octave * 12.0) + key + ((gdouble) midi_note - 48.0)) / 12.0) * 440.0);

//    g_message("synth 0 - osc = %d, frequency = %f, octave = %f, key= %f", channel_data->synth_0.synth_oscillator_mode, channel_data->synth_0.frequency, octave, key);
      
    /* synth-0 phase */    
    g_object_get(fx_fm_synth_audio,
		 "synth-0-phase", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_phase(channel_data->synth_0,
				  ((gdouble) samplerate / ags_fm_synth_util_get_frequency(channel_data->synth_0)) * ((gdouble) g_value_get_float(&value) / (2.0 * M_PI)));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 volume */    
    g_object_get(fx_fm_synth_audio,
		 "synth-0-volume", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_volume(channel_data->synth_0,
				   (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 LFO oscillator */    
    g_object_get(fx_fm_synth_audio,
		 "synth-0-lfo-oscillator", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_lfo_oscillator_mode(channel_data->synth_0,
						(guint) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 LFO frequency */    
    g_object_get(fx_fm_synth_audio,
		 "synth-0-lfo-frequency", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_lfo_frequency(channel_data->synth_0,
					  (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 LFO depth */    
    g_object_get(fx_fm_synth_audio,
		 "synth-0-lfo-depth", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_lfo_depth(channel_data->synth_0,
				      (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 LFO tuning */    
    g_object_get(fx_fm_synth_audio,
		 "synth-0-lfo-tuning", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_tuning(channel_data->synth_0,
				   (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* synth-1 oscillator */    
    g_object_get(fx_fm_synth_audio,
		 "synth-1-oscillator", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_synth_oscillator_mode(channel_data->synth_1,
						  (guint) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* synth-1 octave */
    octave = 0.0;
    key = 0.0;
    
    g_object_get(fx_fm_synth_audio,
		 "synth-1-octave", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      octave = (gdouble) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* synth-1 key */    
    g_object_get(fx_fm_synth_audio,
		 "synth-1-key", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      key = (gdouble) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    ags_fm_synth_util_set_frequency(channel_data->synth_1,
				    exp2(((octave * 12.0) + key + ((gdouble) midi_note - 48.0)) / 12.0) * 440.0);

//    g_message("synth 1 - osc = %d, frequency = %f, octave = %f, key= %f", channel_data->synth_1.synth_oscillator_mode, channel_data->synth_1.frequency, octave, key);
      
    /* synth-1 phase */    
    g_object_get(fx_fm_synth_audio,
		 "synth-1-phase", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_phase(channel_data->synth_1,
				  ((gdouble) samplerate / ags_fm_synth_util_get_frequency(channel_data->synth_1)) * ((gdouble) g_value_get_float(&value) / (2.0 * M_PI)));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-1 volume */    
    g_object_get(fx_fm_synth_audio,
		 "synth-1-volume", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_volume(channel_data->synth_0,
				   (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-1 LFO oscillator */    
    g_object_get(fx_fm_synth_audio,
		 "synth-1-lfo-oscillator", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_lfo_oscillator_mode(channel_data->synth_1,
						(guint) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-1 LFO frequency */    
    g_object_get(fx_fm_synth_audio,
		 "synth-1-lfo-frequency", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_lfo_frequency(channel_data->synth_1,
					  (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-1 LFO depth */    
    g_object_get(fx_fm_synth_audio,
		 "synth-1-lfo-depth", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_lfo_depth(channel_data->synth_1,
				      (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-1 LFO tuning */    
    g_object_get(fx_fm_synth_audio,
		 "synth-1-lfo-tuning", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_tuning(channel_data->synth_1,
				   (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* synth-2 oscillator */    
    g_object_get(fx_fm_synth_audio,
		 "synth-2-oscillator", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_synth_oscillator_mode(channel_data->synth_2,
						  (guint) g_value_get_float(&value));
            
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* synth-2 octave */
    octave = 0.0;
    key = 0.0;
    
    g_object_get(fx_fm_synth_audio,
		 "synth-2-octave", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      octave = (gdouble) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* synth-2 key */    
    g_object_get(fx_fm_synth_audio,
		 "synth-2-key", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      key = (gdouble) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    ags_fm_synth_util_set_frequency(channel_data->synth_2,
				    exp2(((octave * 12.0) + key + ((gdouble) midi_note - 48.0)) / 12.0) * 440.0);

//    g_message("synth 2 - osc = %d, frequency = %f, octave = %f, key= %f", channel_data->synth_2.synth_oscillator_mode, channel_data->synth_2.frequency, octave, key);
      
    /* synth-2 phase */    
    g_object_get(fx_fm_synth_audio,
		 "synth-2-phase", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_phase(channel_data->synth_2,
				  ((gdouble) samplerate / ags_fm_synth_util_get_frequency(channel_data->synth_2)) * ((gdouble) g_value_get_float(&value) / (2.0 * M_PI)));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-2 volume */    
    g_object_get(fx_fm_synth_audio,
		 "synth-2-volume", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_volume(channel_data->synth_2,
				   (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-2 LFO oscillator */    
    g_object_get(fx_fm_synth_audio,
		 "synth-2-lfo-oscillator", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_lfo_oscillator_mode(channel_data->synth_2,
						(guint) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-2 LFO frequency */    
    g_object_get(fx_fm_synth_audio,
		 "synth-2-lfo-frequency", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_lfo_frequency(channel_data->synth_2,
					  (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-2 LFO depth */    
    g_object_get(fx_fm_synth_audio,
		 "synth-2-lfo-depth", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_lfo_depth(channel_data->synth_2,
				      (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-2 LFO tuning */    
    g_object_get(fx_fm_synth_audio,
		 "synth-2-lfo-tuning", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_fm_synth_util_set_tuning(channel_data->synth_2,
				   (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* noise volume */    
    g_object_get(fx_fm_synth_audio,
		 "noise-gain", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_noise_util_set_volume(channel_data->noise_util,
				(gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* pitch base key */
    ags_common_pitch_util_set_base_key(channel_data->pitch_util,
				       channel_data->pitch_type,
				       (gdouble) midi_note - 48.0);
    
    /* pitch tuning */    
    g_object_get(fx_fm_synth_audio,
		 "pitch-tuning", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_common_pitch_util_set_tuning(channel_data->pitch_util,
				       channel_data->pitch_type,
				       (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* low-pass enabled */    
    g_object_get(fx_fm_synth_audio,
		 "low-pass-enabled", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      low_pass_enabled = (gboolean) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    if(low_pass_enabled){
      /* low-pass q-lin */    
      g_object_get(fx_fm_synth_audio,
		   "low-pass-q-lin", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	ags_fluid_iir_filter_util_set_q_lin(channel_data->low_pass_filter,
					    (gdouble) g_value_get_float(&value));
      
	g_object_unref(port);
      }

      g_value_unset(&value);

      /* low-pass filter-gain */    
      g_object_get(fx_fm_synth_audio,
		   "low-pass-filter-gain", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	ags_fluid_iir_filter_util_set_filter_gain(channel_data->low_pass_filter,
						  (gdouble) g_value_get_float(&value));
      
	g_object_unref(port);
      }

      g_value_unset(&value);
    }

    /* high-pass enabled */    
    g_object_get(fx_fm_synth_audio,
		 "high-pass-enabled", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      high_pass_enabled = (gboolean) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    if(high_pass_enabled){
      /* high-pass q-lin */    
      g_object_get(fx_fm_synth_audio,
		   "high-pass-q-lin", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	ags_fluid_iir_filter_util_set_q_lin(channel_data->high_pass_filter,
					    (gdouble) g_value_get_float(&value));
      
	g_object_unref(port);
      }

      g_value_unset(&value);

      /* high-pass filter-gain */    
      g_object_get(fx_fm_synth_audio,
		   "high-pass-filter-gain", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	ags_fluid_iir_filter_util_set_filter_gain(channel_data->high_pass_filter,
						  (gdouble) g_value_get_float(&value));
      
	g_object_unref(port);
      }

      g_value_unset(&value);
    }

    /* chorus enabled */    
    g_object_get(fx_fm_synth_audio,
		 "chorus-enabled", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      chorus_enabled = (gboolean) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    if(chorus_enabled){
      /* chorus input-volume */    
      g_object_get(fx_fm_synth_audio,
		   "chorus-input-volume", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	ags_chorus_util_set_input_volume(channel_data->chorus_util,
					 (gdouble) g_value_get_float(&value));
      
	g_object_unref(port);
      }

      g_value_unset(&value);

      /* chorus output-volume */    
      g_object_get(fx_fm_synth_audio,
		   "chorus-output-volume", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	ags_chorus_util_set_output_volume(channel_data->chorus_util,
					  (gdouble) g_value_get_float(&value));
      
	g_object_unref(port);
      }

      g_value_unset(&value);

      /* chorus lfo-oscillator */    
      g_object_get(fx_fm_synth_audio,
		   "chorus-lfo-oscillator", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	ags_chorus_util_set_lfo_oscillator(channel_data->chorus_util,
					   (guint) g_value_get_float(&value));
      
	g_object_unref(port);
      }

      g_value_unset(&value);

      /* chorus lfo-frequency */    
      g_object_get(fx_fm_synth_audio,
		   "chorus-lfo-frequency", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	ags_chorus_util_set_lfo_frequency(channel_data->chorus_util,
					  (gdouble) g_value_get_float(&value));
      
	g_object_unref(port);
      }

      g_value_unset(&value);

      /* chorus depth */    
      g_object_get(fx_fm_synth_audio,
		   "chorus-depth", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	ags_chorus_util_set_depth(channel_data->chorus_util,
				  (gdouble) g_value_get_float(&value));
      
	g_object_unref(port);
      }

      g_value_unset(&value);

      /* chorus mix */    
      g_object_get(fx_fm_synth_audio,
		   "chorus-mix", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	ags_chorus_util_set_mix(channel_data->chorus_util,
				(gdouble) g_value_get_float(&value));
      
	g_object_unref(port);
      }

      g_value_unset(&value);

      /* chorus delay */    
      g_object_get(fx_fm_synth_audio,
		   "chorus-delay", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	ags_chorus_util_set_delay(channel_data->chorus_util,
				  (gdouble) g_value_get_float(&value));
            
	g_object_unref(port);
      }

      g_value_unset(&value);
    }
  }

  copy_mode_out = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						      AGS_AUDIO_BUFFER_UTIL_FLOAT);

  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxFMSynthAudioScopeData *scope_data;
    AgsFxFMSynthAudioChannelData *channel_data;
    AgsFxFMSynthAudioInputData *input_data;

    g_rec_mutex_lock(fx_fm_synth_audio_mutex);

    scope_data = fx_fm_synth_audio->scope_data[sound_scope];

    channel_data = scope_data->channel_data[audio_channel];

    input_data = channel_data->input_data[midi_note];

    g_rec_mutex_unlock(fx_fm_synth_audio_mutex);

    if(delay_counter == 0.0 &&
       x0 == offset_counter){
      g_rec_mutex_lock(fx_fm_synth_audio_mutex);

      input_data->key_on += 1;

      g_rec_mutex_unlock(fx_fm_synth_audio_mutex);
    }

    /* FM synth 0 */
    ags_fm_synth_util_set_source(channel_data->synth_0,
				 source->stream_current->data);
    ags_fm_synth_util_set_source_stride(channel_data->synth_0,
					1);

    ags_fm_synth_util_set_buffer_length(channel_data->synth_0,
					buffer_size);
    ags_fm_synth_util_set_format(channel_data->synth_0,
				 format);
    ags_fm_synth_util_set_samplerate(channel_data->synth_0,
				     samplerate);

    ags_fm_synth_util_set_frame_count(channel_data->synth_0,
				      floor(((offset_counter - x0) * delay + delay_counter + 1.0) * buffer_size));
    ags_fm_synth_util_set_offset(channel_data->synth_0,
				 floor(((offset_counter - x0) * delay + delay_counter) * buffer_size));

    switch(ags_fm_synth_util_get_synth_oscillator_mode(channel_data->synth_0)){
    case AGS_SYNTH_OSCILLATOR_SIN:
    {
      ags_fm_synth_util_compute_sin(channel_data->synth_0);
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
    {
      ags_fm_synth_util_compute_sawtooth(channel_data->synth_0);
    }
    break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
    {
      ags_fm_synth_util_compute_triangle(channel_data->synth_0);
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
    {
      ags_fm_synth_util_compute_square(channel_data->synth_0);
    }
    break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
    {
      ags_fm_synth_util_compute_impulse(channel_data->synth_0);
    }
    break;
    }
    
    /* FM synth 1 */
    ags_fm_synth_util_set_source(channel_data->synth_1,
				 source->stream_current->data);
    ags_fm_synth_util_set_source_stride(channel_data->synth_1,
					1);

    ags_fm_synth_util_set_buffer_length(channel_data->synth_1,
					buffer_size);
    ags_fm_synth_util_set_format(channel_data->synth_1,
				 format);
    ags_fm_synth_util_set_samplerate(channel_data->synth_1,
				     samplerate);

    ags_fm_synth_util_set_frame_count(channel_data->synth_1,
				      floor(((offset_counter - x0) * delay + delay_counter + 1.0) * buffer_size));
    ags_fm_synth_util_set_offset(channel_data->synth_1,
				 floor(((offset_counter - x0) * delay + delay_counter) * buffer_size));

    switch(ags_fm_synth_util_get_synth_oscillator_mode(channel_data->synth_1)){
    case AGS_SYNTH_OSCILLATOR_SIN:
    {
      ags_fm_synth_util_compute_sin(channel_data->synth_1);
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
    {
      ags_fm_synth_util_compute_sawtooth(channel_data->synth_1);
    }
    break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
    {
      ags_fm_synth_util_compute_triangle(channel_data->synth_1);
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
    {
      ags_fm_synth_util_compute_square(channel_data->synth_1);
    }
    break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
    {
      ags_fm_synth_util_compute_impulse(channel_data->synth_1);
    }
    break;
    }
    
    /* FM synth 2 */
    ags_fm_synth_util_set_source(channel_data->synth_2,
				 source->stream_current->data);
    ags_fm_synth_util_set_source_stride(channel_data->synth_2,
					2);

    ags_fm_synth_util_set_buffer_length(channel_data->synth_2,
					buffer_size);
    ags_fm_synth_util_set_format(channel_data->synth_2,
				 format);
    ags_fm_synth_util_set_samplerate(channel_data->synth_2,
				     samplerate);

    ags_fm_synth_util_set_frame_count(channel_data->synth_2,
				      floor(((offset_counter - x0) * delay + delay_counter + 1.0) * buffer_size));
    ags_fm_synth_util_set_offset(channel_data->synth_2,
				 floor(((offset_counter - x0) * delay + delay_counter) * buffer_size));

    switch(ags_fm_synth_util_get_synth_oscillator_mode(channel_data->synth_2)){
    case AGS_SYNTH_OSCILLATOR_SIN:
    {
      ags_fm_synth_util_compute_sin(channel_data->synth_2);
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
    {
      ags_fm_synth_util_compute_sawtooth(channel_data->synth_2);
    }
    break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
    {
      ags_fm_synth_util_compute_triangle(channel_data->synth_2);
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
    {
      ags_fm_synth_util_compute_square(channel_data->synth_2);
    }
    break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
    {
      ags_fm_synth_util_compute_impulse(channel_data->synth_2);
    }
    break;
    }

    /* noise */
    if(ags_noise_util_get_volume(channel_data->noise_util) != 0.0){
      ags_noise_util_set_source(channel_data->noise_util,
				source->stream_current->data);

      ags_noise_util_set_destination(channel_data->noise_util,
				     source->stream_current->data);

      ags_noise_util_set_samplerate(channel_data->noise_util,
				    samplerate);
      ags_noise_util_set_buffer_length(channel_data->noise_util,
				       buffer_size);
      ags_noise_util_set_format(channel_data->noise_util,
				format);

      ags_noise_util_set_frequency(channel_data->noise_util,
				   exp2((midi_note - 48.0) / 12.0) * 440.0);
      
      ags_noise_util_set_frame_count(channel_data->noise_util,
				     floor(((offset_counter - x0) * delay + delay_counter + 1.0) * buffer_size));
      ags_noise_util_set_offset(channel_data->noise_util,
				floor(((offset_counter - x0) * delay + delay_counter) * buffer_size));
      
      ags_noise_util_compute(channel_data->noise_util);

      /* reset source and destination */
      ags_noise_util_set_source(channel_data->noise_util,
				NULL);
      
      ags_noise_util_set_destination(channel_data->noise_util,
				     NULL);
    }
    
    
    /* low-pass */
    if(low_pass_enabled){
      ags_fluid_iir_filter_util_set_source(channel_data->low_pass_filter,
					   source->stream_current->data);

      ags_fluid_iir_filter_util_set_destination(channel_data->low_pass_filter,
						source->stream_current->data);

      ags_fluid_iir_filter_util_process(channel_data->low_pass_filter);

      /* reset */
      ags_fluid_iir_filter_util_set_source(channel_data->low_pass_filter,
					   NULL);

      ags_fluid_iir_filter_util_set_destination(channel_data->low_pass_filter,
						NULL);
    }

    /* high-pass */
    if(high_pass_enabled){
      ags_fluid_iir_filter_util_set_source(channel_data->high_pass_filter,
					   source->stream_current->data);

      ags_fluid_iir_filter_util_set_destination(channel_data->high_pass_filter,
						source->stream_current->data);

      ags_fluid_iir_filter_util_process(channel_data->high_pass_filter);

      /* reset */
      ags_fluid_iir_filter_util_set_source(channel_data->low_pass_filter,
					   NULL);

      ags_fluid_iir_filter_util_set_destination(channel_data->low_pass_filter,
						NULL);
    }

    /* chorus */
    if(ags_chorus_util_get_depth(channel_data->chorus_util) != 0.0 &&
       chorus_enabled){
      ags_chorus_util_set_source(channel_data->chorus_util,
				 source->stream_current->data);

      ags_chorus_util_set_samplerate(channel_data->chorus_util,
				     samplerate);
      ags_chorus_util_set_buffer_length(channel_data->chorus_util,
					buffer_size);
      ags_chorus_util_set_format(channel_data->chorus_util,
				 format);

      ags_chorus_util_set_offset(channel_data->chorus_util,
				 floor(((offset_counter - x0) * delay + delay_counter) * buffer_size));

      ags_chorus_util_set_base_key(channel_data->chorus_util,
				   (gdouble) midi_note - 48.0);
      
      ags_chorus_util_compute(channel_data->chorus_util);

      ags_audio_buffer_util_clear_buffer(source->stream_current->data, 1,
					 buffer_size, audio_buffer_util_format);

      ags_audio_buffer_util_copy_buffer_to_buffer(source->stream_current->data, 1, 0,
						  ags_chorus_util_get_destination(channel_data->chorus_util), 1, 0,
						  buffer_size, copy_mode);

      /* reset */      
      ags_chorus_util_set_source(channel_data->chorus_util,
				 NULL);
    }
  }
  
  /* unref */
  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(fx_fm_synth_audio != NULL){
    g_object_unref(fx_fm_synth_audio);
  }

  if(fx_fm_synth_audio_processor != NULL){
    g_object_unref(fx_fm_synth_audio_processor);
  }
  
  if(fx_fm_synth_channel_processor != NULL){
    g_object_unref(fx_fm_synth_channel_processor);
  }
  
  if(fx_fm_synth_recycling != NULL){
    g_object_unref(fx_fm_synth_recycling);
  }
}

void
ags_fx_fm_synth_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					   AgsAudioSignal *source,
					   AgsNote *note,
					   guint x0, guint x1,
					   guint y)
{
  AgsAudio *audio;
  AgsFxFMSynthAudio *fx_fm_synth_audio;
  AgsFxFMSynthAudioProcessor *fx_fm_synth_audio_processor;
  AgsFxFMSynthChannelProcessor *fx_fm_synth_channel_processor;
  AgsFxFMSynthRecycling *fx_fm_synth_recycling;

  guint sound_scope;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;

  GRecMutex *fx_fm_synth_audio_mutex;

  audio = NULL;
  
  fx_fm_synth_audio = NULL;
  fx_fm_synth_audio_processor = NULL;

  fx_fm_synth_channel_processor = NULL;

  fx_fm_synth_recycling = NULL;

  sound_scope = ags_recall_get_sound_scope(fx_notation_audio_signal);

  audio_channel = 0;

  audio_start_mapping = 0;
  midi_start_mapping = 0;

  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_fm_synth_recycling,
	       NULL);

  g_object_get(fx_fm_synth_recycling,
	       "parent", &fx_fm_synth_channel_processor,
	       NULL);
  
  g_object_get(fx_fm_synth_channel_processor,
	       "recall-audio", &fx_fm_synth_audio,
	       "recall-audio-run", &fx_fm_synth_audio_processor,
	       NULL);

  g_object_get(fx_fm_synth_audio,
	       "audio", &audio,
	       NULL);

  g_object_get(fx_fm_synth_audio_processor,
	       "audio-channel", &audio_channel,
	       NULL);

  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  fx_fm_synth_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_fm_synth_audio);

  midi_note = (y - audio_start_mapping + midi_start_mapping);

  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxFMSynthAudioScopeData *scope_data;
    AgsFxFMSynthAudioChannelData *channel_data;
    AgsFxFMSynthAudioInputData *input_data;

    g_rec_mutex_lock(fx_fm_synth_audio_mutex);
      
    scope_data = fx_fm_synth_audio->scope_data[sound_scope];

    channel_data = scope_data->channel_data[audio_channel];

    input_data = channel_data->input_data[midi_note];

    input_data->key_on -= 1;
      
    g_rec_mutex_unlock(fx_fm_synth_audio_mutex);
  }
  
  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(fx_fm_synth_audio != NULL){
    g_object_unref(fx_fm_synth_audio);
  }

  if(fx_fm_synth_audio_processor != NULL){
    g_object_unref(fx_fm_synth_audio_processor);
  }
  
  if(fx_fm_synth_channel_processor != NULL){
    g_object_unref(fx_fm_synth_channel_processor);
  }
  
  if(fx_fm_synth_recycling != NULL){
    g_object_unref(fx_fm_synth_recycling);
  }
}

/**
 * ags_fx_fm_synth_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxFMSynthAudioSignal
 *
 * Returns: the new #AgsFxFMSynthAudioSignal
 *
 * Since: 3.14.0
 */
AgsFxFMSynthAudioSignal*
ags_fx_fm_synth_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxFMSynthAudioSignal *fx_fm_synth_audio_signal;

  fx_fm_synth_audio_signal = (AgsFxFMSynthAudioSignal *) g_object_new(AGS_TYPE_FX_FM_SYNTH_AUDIO_SIGNAL,
								      "source", audio_signal,
								      NULL);

  return(fx_fm_synth_audio_signal);
}
