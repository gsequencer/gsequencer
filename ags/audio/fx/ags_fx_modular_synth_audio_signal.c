/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_modular_synth_audio_signal.h>

#include <ags/audio/ags_port.h>
#include <ags/audio/ags_modular_synth_util.h>

#include <ags/audio/fx/ags_fx_modular_synth_audio.h>
#include <ags/audio/fx/ags_fx_modular_synth_audio_processor.h>
#include <ags/audio/fx/ags_fx_modular_synth_channel.h>
#include <ags/audio/fx/ags_fx_modular_synth_channel_processor.h>
#include <ags/audio/fx/ags_fx_modular_synth_recycling.h>

#include <ags/i18n.h>

void ags_fx_modular_synth_audio_signal_class_init(AgsFxModularSynthAudioSignalClass *fx_modular_synth_audio_signal);
void ags_fx_modular_synth_audio_signal_init(AgsFxModularSynthAudioSignal *fx_modular_synth_audio_signal);
void ags_fx_modular_synth_audio_signal_dispose(GObject *gobject);
void ags_fx_modular_synth_audio_signal_finalize(GObject *gobject);

void ags_fx_modular_synth_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
						   AgsAudioSignal *source,
						   AgsNote *note,
						   gboolean pattern_mode,
						   guint x0, guint x1,
						   guint y,
						   gdouble delay_counter, guint64 offset_counter,
						   guint frame_count,
						   gdouble delay, guint buffer_size);
void ags_fx_modular_synth_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
						     AgsAudioSignal *source,
						     AgsNote *note,
						     guint x0, guint x1,
						     guint y);

/**
 * SECTION:ags_fx_modular_synth_audio_signal
 * @short_description: fx modular synth audio signal
 * @title: AgsFxModularSynthAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_modular_synth_audio_signal.h
 *
 * The #AgsFxModularSynthAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_modular_synth_audio_signal_parent_class = NULL;

const gchar *ags_fx_modular_synth_audio_signal_plugin_name = "ags-fx-modular-synth";

GType
ags_fx_modular_synth_audio_signal_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_modular_synth_audio_signal = 0;

    static const GTypeInfo ags_fx_modular_synth_audio_signal_info = {
      sizeof (AgsFxModularSynthAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_modular_synth_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxModularSynthAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_modular_synth_audio_signal_init,
    };

    ags_type_fx_modular_synth_audio_signal = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL,
								    "AgsFxModularSynthAudioSignal",
								    &ags_fx_modular_synth_audio_signal_info,
								    0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_modular_synth_audio_signal);
  }

  return(g_define_type_id__static);
}

void
ags_fx_modular_synth_audio_signal_class_init(AgsFxModularSynthAudioSignalClass *fx_modular_synth_audio_signal)
{
  GObjectClass *gobject;
  AgsFxNotationAudioSignalClass *fx_notation_audio_signal;

  ags_fx_modular_synth_audio_signal_parent_class = g_type_class_peek_parent(fx_modular_synth_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_modular_synth_audio_signal;

  gobject->dispose = ags_fx_modular_synth_audio_signal_dispose;
  gobject->finalize = ags_fx_modular_synth_audio_signal_finalize;

  /* AgsFxNotationAudioSignalClass */
  fx_notation_audio_signal = (AgsFxNotationAudioSignalClass *) fx_modular_synth_audio_signal;
  
  fx_notation_audio_signal->stream_feed = ags_fx_modular_synth_audio_signal_stream_feed;
  fx_notation_audio_signal->notify_remove = ags_fx_modular_synth_audio_signal_notify_remove;
}

void
ags_fx_modular_synth_audio_signal_init(AgsFxModularSynthAudioSignal *fx_modular_synth_audio_signal)
{
  AGS_RECALL(fx_modular_synth_audio_signal)->name = "ags-fx-modular-synth";
  AGS_RECALL(fx_modular_synth_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_modular_synth_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_modular_synth_audio_signal)->xml_type = "ags-fx-modular-synth-audio-signal";
}

void
ags_fx_modular_synth_audio_signal_dispose(GObject *gobject)
{
  AgsFxModularSynthAudioSignal *fx_modular_synth_audio_signal;
  
  fx_modular_synth_audio_signal = AGS_FX_MODULAR_SYNTH_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_modular_synth_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_modular_synth_audio_signal_finalize(GObject *gobject)
{
  AgsFxModularSynthAudioSignal *fx_modular_synth_audio_signal;
  
  fx_modular_synth_audio_signal = AGS_FX_MODULAR_SYNTH_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_modular_synth_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_modular_synth_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
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
  AgsFxModularSynthAudio *fx_modular_synth_audio;
  AgsFxModularSynthAudioProcessor *fx_modular_synth_audio_processor;
  AgsFxModularSynthChannelProcessor *fx_modular_synth_channel_processor;
  AgsFxModularSynthRecycling *fx_modular_synth_recycling;
  AgsFxModularSynthAudioSignal *fx_modular_synth_audio_signal;
  
  guint sound_scope;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;
  guint x0_256th, x1_256th;
  guint64 note_256th_offset_lower;
  gdouble note_256th_delay;
  guint format;
  guint samplerate;
  guint copy_mode_out;
  guint audio_buffer_util_format;
  guint copy_mode;

  gdouble octave;
  gdouble key;  
    
  gboolean note_256th_mode;

  gboolean chorus_enabled;

  GRecMutex *source_stream_mutex;
  GRecMutex *fx_modular_synth_audio_mutex;
  GRecMutex *fx_modular_synth_audio_processor_mutex;

  fx_modular_synth_audio_signal = (AgsFxModularSynthAudioSignal *) fx_notation_audio_signal;
  
  audio = NULL;
  
  fx_modular_synth_audio = NULL;
  fx_modular_synth_audio_processor = NULL;
  
  fx_modular_synth_channel_processor = NULL;

  fx_modular_synth_recycling = NULL;

  sound_scope = ags_recall_get_sound_scope((AgsRecall *) fx_notation_audio_signal);

  audio_channel = 0;
  
  audio_start_mapping = 0;
  midi_start_mapping = 0;

  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  octave = 0.0;
  key = 0.0;
  
  note_256th_mode = ags_fx_notation_audio_get_note_256th_mode((AgsFxNotationAudio *) fx_modular_synth_audio);
  
  chorus_enabled = FALSE;
  
  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_modular_synth_recycling,
	       NULL);

  g_object_get(fx_modular_synth_recycling,
	       "parent", &fx_modular_synth_channel_processor,
	       NULL);
  
  g_object_get(fx_modular_synth_channel_processor,
	       "recall-audio", &fx_modular_synth_audio,
	       "recall-audio-run", &fx_modular_synth_audio_processor,
	       NULL);

  g_object_get(fx_modular_synth_audio,
	       "audio", &audio,
	       NULL);

  g_object_get(fx_modular_synth_audio_processor,
	       "audio-channel", &audio_channel,
	       NULL);
  
  g_object_get(source,
	       "format", &format,
	       "samplerate", &samplerate,
	       NULL);

  g_object_get(note,
	       "x0-256th", &x0_256th,
	       "x1-256th", &x1_256th,
	       NULL);

  audio_buffer_util_format = ags_audio_buffer_util_format_from_soundcard(&(fx_modular_synth_audio_signal->audio_buffer_util),
									 format);

  copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(&(fx_modular_synth_audio_signal->audio_buffer_util),
							      audio_buffer_util_format,
							      audio_buffer_util_format);
  
  /* get synth mutex */
  fx_modular_synth_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_modular_synth_audio);
  fx_modular_synth_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_modular_synth_audio_processor);

  g_rec_mutex_lock(fx_modular_synth_audio_processor_mutex);

  note_256th_offset_lower = AGS_FX_NOTATION_AUDIO_PROCESSOR(fx_modular_synth_audio_processor)->note_256th_offset_lower;

  note_256th_delay = AGS_FX_NOTATION_AUDIO_PROCESSOR(fx_modular_synth_audio_processor)->note_256th_delay;
  
  g_rec_mutex_unlock(fx_modular_synth_audio_processor_mutex);

  /* process data */
  source_stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);

  audio_start_mapping = 0;
  midi_start_mapping = 0;
  
  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
    midi_note = (128 - y - 1 - audio_start_mapping + midi_start_mapping);
  }else{
    midi_note = (y - audio_start_mapping + midi_start_mapping);
  }

  if(fx_modular_synth_audio != NULL){
    AgsPort *port;

    AgsFxModularSynthAudioChannelData *channel_data;

    GValue value = {0,};

    channel_data = fx_modular_synth_audio->scope_data[sound_scope]->channel_data[audio_channel];
    
    /* synth-0 osc-0 oscillator */
    g_object_get(fx_modular_synth_audio,
		 "synth-0-osc-0-oscillator", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_osc_0_oscillator(channel_data->modular_synth_util_0,
						  (guint) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* synth-0 osc-0 octave */
    g_object_get(fx_modular_synth_audio,
		 "synth-0-osc-0-octave", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      octave = (gdouble) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* synth-0 osc-0 key */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-osc-0-key", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      key = (gdouble) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    ags_modular_synth_util_set_osc_0_frequency(channel_data->modular_synth_util_0,
					       exp2(((octave * 12.0) + key + ((gdouble) midi_note - 48.0)) / 12.0) * 440.0);

    //    g_message("synth 0 - osc = %d, frequency = %f, octave = %f, key= %f", channel_data->modular_synth_0.synth_oscillator_mode, channel_data->modular_synth_0.frequency, octave, key);
      
    /* synth-0 osc-0 phase */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-osc-0-phase", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_osc_0_phase(channel_data->modular_synth_util_0,
					     ((gdouble) samplerate / ags_modular_synth_util_get_osc_0_frequency(channel_data->modular_synth_util_0)) * ((gdouble) g_value_get_float(&value) / (2.0 * M_PI)));
      
      g_object_unref(port);
    }

    g_value_unset(&value);
      
    /* synth-0 osc-0 volume */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-osc-0-volume", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_osc_0_volume(channel_data->modular_synth_util_0,
					      (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* synth-0 osc-1 oscillator */
    g_object_get(fx_modular_synth_audio,
		 "synth-0-osc-1-oscillator", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_osc_1_oscillator(channel_data->modular_synth_util_0,
						  (guint) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* synth-0 osc-1 octave */
    g_object_get(fx_modular_synth_audio,
		 "synth-0-osc-1-octave", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      octave = (gdouble) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* synth-0 osc-1 key */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-osc-1-key", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      key = (gdouble) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    ags_modular_synth_util_set_osc_1_frequency(channel_data->modular_synth_util_0,
					       exp2(((octave * 12.0) + key + ((gdouble) midi_note - 48.0)) / 12.0) * 440.0);

    //    g_message("synth 0 - osc = %d, frequency = %f, octave = %f, key= %f", channel_data->modular_synth_0.synth_oscillator_mode, channel_data->modular_synth_0.frequency, octave, key);
      
    /* synth-0 osc-1 phase */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-osc-1-phase", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_osc_1_phase(channel_data->modular_synth_util_0,
					     ((gdouble) samplerate / ags_modular_synth_util_get_osc_1_frequency(channel_data->modular_synth_util_0)) * ((gdouble) g_value_get_float(&value) / (2.0 * M_PI)));
      
      g_object_unref(port);
    }

    g_value_unset(&value);
      
    /* synth-0 osc-1 volume */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-osc-1-volume", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_osc_1_volume(channel_data->modular_synth_util_0,
					      (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* pitch base key */
    //    ags_common_pitch_util_set_base_key(channel_data->pitch_util,
    //				       channel_data->pitch_type,
    //				       (gdouble) midi_note - 48.0);
    
    /* pitch tuning */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-pitch-tuning", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_pitch_tuning(channel_data->modular_synth_util_0,
					      (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 volume */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-volume", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_volume(channel_data->modular_synth_util_0,
					(gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);
      
    /* synth-0 env-0 attack */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-env-0-attack", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_env_0_attack(channel_data->modular_synth_util_0,
					      (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 env-0 decay */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-env-0-decay", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_env_0_decay(channel_data->modular_synth_util_0,
					     (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 env-0 sustain */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-env-0-sustain", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_env_0_sustain(channel_data->modular_synth_util_0,
					       (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 env-0 release */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-env-0-release", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_env_0_release(channel_data->modular_synth_util_0,
					       (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 env-0 gain */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-env-0-gain", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_env_0_gain(channel_data->modular_synth_util_0,
					    (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 env-0 frequency */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-env-0-frequency", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_env_0_frequency(channel_data->modular_synth_util_0,
						 (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 env-0 sends */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-env-0-sends", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_env_0_sends(channel_data->modular_synth_util_0,
					     (gint *) g_value_get_pointer(&value),
					     AGS_MODULAR_SYNTH_SENDS_COUNT);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 env-1 attack */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-env-1-attack", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_env_1_attack(channel_data->modular_synth_util_0,
					      (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 env-1 decay */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-env-1-decay", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_env_1_decay(channel_data->modular_synth_util_0,
					     (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 env-1 sustain */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-env-1-sustain", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_env_1_sustain(channel_data->modular_synth_util_0,
					       (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 env-1 release */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-env-1-release", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_env_1_release(channel_data->modular_synth_util_0,
					       (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 env-1 gain */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-env-1-gain", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_env_1_gain(channel_data->modular_synth_util_0,
					    (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 env-1 frequency */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-env-1-frequency", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_env_1_frequency(channel_data->modular_synth_util_0,
						 (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 env-1 sends */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-env-1-sends", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_env_1_sends(channel_data->modular_synth_util_0,
					     (gint *) g_value_get_pointer(&value),
					     AGS_MODULAR_SYNTH_SENDS_COUNT);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    //TODO:JK: implement me
    
    /* noise frequency */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-noise-frequency", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_noise_util_set_frequency(channel_data->modular_synth_util_0->noise_util,
				   (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);    
    
    /* noise gain */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-noise-gain", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_noise_util_set_volume(channel_data->modular_synth_util_0->noise_util,
				(gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);    

    /* synth-0 noise sends */    
    g_object_get(fx_modular_synth_audio,
		 "synth-0-noise-sends", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_modular_synth_util_set_noise_sends(channel_data->modular_synth_util_0,
					     (gint *) g_value_get_pointer(&value),
					     AGS_MODULAR_SYNTH_SENDS_COUNT);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* low-pass 0 cut-off frequency */    
    g_object_get(fx_modular_synth_audio,
		 "low-pass-0-cut-off-frequency", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_low_pass_filter_util_set_cut_off_frequency(channel_data->low_pass_filter_util_0,
						     (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* low-pass 0 filter gain */    
    g_object_get(fx_modular_synth_audio,
		 "low-pass-0-filter-gain", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_low_pass_filter_util_set_filter_gain(channel_data->low_pass_filter_util_0,
					       (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* low-pass 0 no-clip */    
    g_object_get(fx_modular_synth_audio,
		 "low-pass-0-no-clip", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_low_pass_filter_util_set_no_clip(channel_data->low_pass_filter_util_0,
					   (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* amplifier 0 amp 0 gain */    
    g_object_get(fx_modular_synth_audio,
		 "amplifier-0-amp-0-gain", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_amplifier_util_set_amp_0_gain(channel_data->amplifier_util_0,
					(gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* amplifier 0 amp 1 gain */    
    g_object_get(fx_modular_synth_audio,
		 "amplifier-0-amp-1-gain", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_amplifier_util_set_amp_1_gain(channel_data->amplifier_util_0,
					(gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* amplifier 0 amp 2 gain */    
    g_object_get(fx_modular_synth_audio,
		 "amplifier-0-amp-2-gain", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_amplifier_util_set_amp_2_gain(channel_data->amplifier_util_0,
					(gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* amplifier 0 amp 3 gain */    
    g_object_get(fx_modular_synth_audio,
		 "amplifier-0-amp-3-gain", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_amplifier_util_set_amp_3_gain(channel_data->amplifier_util_0,
					(gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* amplifier 0 filter gain */    
    g_object_get(fx_modular_synth_audio,
		 "amplifier-0-filter-gain", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_amplifier_util_set_filter_gain(channel_data->amplifier_util_0,
					 (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* chorus enabled */    
    g_object_get(fx_modular_synth_audio,
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
      g_object_get(fx_modular_synth_audio,
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
      g_object_get(fx_modular_synth_audio,
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
      g_object_get(fx_modular_synth_audio,
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
      g_object_get(fx_modular_synth_audio,
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
      g_object_get(fx_modular_synth_audio,
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
      g_object_get(fx_modular_synth_audio,
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
      g_object_get(fx_modular_synth_audio,
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

  copy_mode_out = ags_audio_buffer_util_get_copy_mode_from_format(&(fx_modular_synth_audio_signal->audio_buffer_util),
								  audio_buffer_util_format,
								  AGS_AUDIO_BUFFER_UTIL_FLOAT);

  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxModularSynthAudioScopeData *scope_data;
    AgsFxModularSynthAudioChannelData *channel_data;
    AgsFxModularSynthAudioInputData *input_data;

    g_rec_mutex_lock(fx_modular_synth_audio_mutex);

    scope_data = fx_modular_synth_audio->scope_data[sound_scope];

    channel_data = scope_data->channel_data[audio_channel];

    input_data = channel_data->input_data[midi_note];

    g_rec_mutex_unlock(fx_modular_synth_audio_mutex);

    if((gint) floor(delay_counter) == 0 &&
       x0 == offset_counter){
      g_rec_mutex_lock(fx_modular_synth_audio_mutex);

      input_data->key_on += 1;

      g_rec_mutex_unlock(fx_modular_synth_audio_mutex);
    }

    ags_audio_buffer_util_clear_buffer(&(fx_modular_synth_audio_signal->audio_buffer_util),
				       channel_data->synth_buffer_0, 1,
				       buffer_size, audio_buffer_util_format);
    
    /* modular synth 0 */
    ags_modular_synth_util_set_source(channel_data->modular_synth_util_0,
				      channel_data->synth_buffer_0);
    ags_modular_synth_util_set_source_stride(channel_data->modular_synth_util_0,
					     1);

    ags_modular_synth_util_set_buffer_length(channel_data->modular_synth_util_0,
					     buffer_size);
    ags_modular_synth_util_set_format(channel_data->modular_synth_util_0,
				      format);
    ags_modular_synth_util_set_samplerate(channel_data->modular_synth_util_0,
					  samplerate);

    //TODO:JK: implement me
    
    if(!note_256th_mode){
      ags_modular_synth_util_set_frame_count(channel_data->modular_synth_util_0,
					     floor(((offset_counter - x0) * delay + delay_counter + 1.0) * buffer_size));

      ags_modular_synth_util_set_offset(channel_data->modular_synth_util_0,
					floor(((offset_counter - x0) * delay + delay_counter) * buffer_size));
    }else{
      g_rec_mutex_lock(fx_modular_synth_audio_processor_mutex);

      note_256th_offset_lower = AGS_FX_NOTATION_AUDIO_PROCESSOR(fx_modular_synth_audio_processor)->note_256th_offset_lower;

      note_256th_delay = AGS_FX_NOTATION_AUDIO_PROCESSOR(fx_modular_synth_audio_processor)->note_256th_delay;

      g_rec_mutex_unlock(fx_modular_synth_audio_processor_mutex);

      ags_modular_synth_util_set_frame_count(channel_data->modular_synth_util_0,
					     (guint) floor((double) (note_256th_offset_lower - x0_256th) * note_256th_delay * (double) buffer_size) + (guint) floor(delay * (double) buffer_size));

      ags_modular_synth_util_set_offset(channel_data->modular_synth_util_0,
					(guint) floor(((double) (offset_counter - x0) * delay + delay_counter) * (double) buffer_size));

      ags_modular_synth_util_set_offset_256th(channel_data->modular_synth_util_0,
					      (guint) floor((double) (note_256th_offset_lower - x0_256th) * note_256th_delay * (double) buffer_size));
    }
    
    g_rec_mutex_lock(source_stream_mutex);

    ags_modular_synth_util_compute(channel_data->modular_synth_util_0);

    /* low-pass 0 */
    ags_low_pass_filter_util_set_source(channel_data->low_pass_filter_util_0,
					channel_data->synth_buffer_0);
    ags_low_pass_filter_util_set_source_stride(channel_data->low_pass_filter_util_0,
					       1);

    ags_low_pass_filter_util_set_destination(channel_data->low_pass_filter_util_0,
					     channel_data->synth_buffer_0);
    ags_low_pass_filter_util_set_destination_stride(channel_data->low_pass_filter_util_0,
						    1);

    ags_low_pass_filter_util_set_samplerate(channel_data->low_pass_filter_util_0,
					    samplerate);
    ags_low_pass_filter_util_set_buffer_length(channel_data->low_pass_filter_util_0,
					       buffer_size);
    ags_low_pass_filter_util_set_format(channel_data->low_pass_filter_util_0,
					format);
    
    ags_low_pass_filter_util_process(channel_data->low_pass_filter_util_0);

    /* reset source and destination */
    ags_low_pass_filter_util_set_source(channel_data->low_pass_filter_util_0,
					NULL);
      
    ags_low_pass_filter_util_set_destination(channel_data->low_pass_filter_util_0,
					     NULL);
    
    /* amplifier 0 */
    ags_amplifier_util_set_source(channel_data->amplifier_util_0,
				  channel_data->synth_buffer_0);
    ags_amplifier_util_set_source_stride(channel_data->amplifier_util_0,
					 1);

    ags_amplifier_util_set_destination(channel_data->amplifier_util_0,
				       channel_data->synth_buffer_0);
    ags_amplifier_util_set_destination_stride(channel_data->amplifier_util_0,
					      1);

    ags_amplifier_util_set_samplerate(channel_data->amplifier_util_0,
				      samplerate);
    ags_amplifier_util_set_buffer_length(channel_data->amplifier_util_0,
					 buffer_size);
    ags_amplifier_util_set_format(channel_data->amplifier_util_0,
				  format);
    
    ags_amplifier_util_process(channel_data->amplifier_util_0);
    
    /* reset source and destination */
    ags_amplifier_util_set_source(channel_data->amplifier_util_0,
				  NULL);
      
    ags_amplifier_util_set_destination(channel_data->amplifier_util_0,
				       NULL);

    ags_modular_synth_util_set_source(channel_data->modular_synth_util_0,
				      NULL);
    
    /* copy synth buffer */
    ags_audio_buffer_util_clear_buffer(&(fx_modular_synth_audio_signal->audio_buffer_util),
				       source->stream_current->data, 1,
				       buffer_size, audio_buffer_util_format);

    ags_audio_buffer_util_copy_buffer_to_buffer(&(fx_modular_synth_audio_signal->audio_buffer_util),
						source->stream_current->data, 1, 0,
						channel_data->synth_buffer_0, 1, 0,
						buffer_size, copy_mode);

    g_rec_mutex_unlock(source_stream_mutex);

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

      
      /* compute chorus */
      g_rec_mutex_lock(source_stream_mutex);
      
      ags_chorus_util_compute(channel_data->chorus_util);

      g_rec_mutex_unlock(source_stream_mutex);

      ags_audio_buffer_util_clear_buffer(&(fx_modular_synth_audio_signal->audio_buffer_util),
					 source->stream_current->data, 1,
					 buffer_size, audio_buffer_util_format);

      ags_audio_buffer_util_copy_buffer_to_buffer(&(fx_modular_synth_audio_signal->audio_buffer_util),
						  source->stream_current->data, 1, 0,
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
  
  if(fx_modular_synth_audio != NULL){
    g_object_unref(fx_modular_synth_audio);
  }

  if(fx_modular_synth_audio_processor != NULL){
    g_object_unref(fx_modular_synth_audio_processor);
  }
  
  if(fx_modular_synth_channel_processor != NULL){
    g_object_unref(fx_modular_synth_channel_processor);
  }
  
  if(fx_modular_synth_recycling != NULL){
    g_object_unref(fx_modular_synth_recycling);
  }
}

void
ags_fx_modular_synth_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
						AgsAudioSignal *source,
						AgsNote *note,
						guint x0, guint x1,
						guint y)
{
  AgsAudio *audio;
  AgsFxModularSynthAudio *fx_modular_synth_audio;
  AgsFxModularSynthAudioProcessor *fx_modular_synth_audio_processor;
  AgsFxModularSynthChannelProcessor *fx_modular_synth_channel_processor;
  AgsFxModularSynthRecycling *fx_modular_synth_recycling;

  guint sound_scope;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;

  GRecMutex *fx_modular_synth_audio_mutex;

  audio = NULL;
  
  fx_modular_synth_audio = NULL;
  fx_modular_synth_audio_processor = NULL;

  fx_modular_synth_channel_processor = NULL;

  fx_modular_synth_recycling = NULL;

  sound_scope = ags_recall_get_sound_scope((AgsRecall *) fx_notation_audio_signal);

  audio_channel = 0;

  audio_start_mapping = 0;
  midi_start_mapping = 0;

  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_modular_synth_recycling,
	       NULL);

  g_object_get(fx_modular_synth_recycling,
	       "parent", &fx_modular_synth_channel_processor,
	       NULL);
  
  g_object_get(fx_modular_synth_channel_processor,
	       "recall-audio", &fx_modular_synth_audio,
	       "recall-audio-run", &fx_modular_synth_audio_processor,
	       NULL);

  g_object_get(fx_modular_synth_audio,
	       "audio", &audio,
	       NULL);

  g_object_get(fx_modular_synth_audio_processor,
	       "audio-channel", &audio_channel,
	       NULL);

  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  fx_modular_synth_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_modular_synth_audio);

  midi_note = (y - audio_start_mapping + midi_start_mapping);

  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxModularSynthAudioScopeData *scope_data;
    AgsFxModularSynthAudioChannelData *channel_data;
    AgsFxModularSynthAudioInputData *input_data;

    g_rec_mutex_lock(fx_modular_synth_audio_mutex);
      
    scope_data = fx_modular_synth_audio->scope_data[sound_scope];

    channel_data = scope_data->channel_data[audio_channel];

    input_data = channel_data->input_data[midi_note];

    input_data->key_on -= 1;
      
    g_rec_mutex_unlock(fx_modular_synth_audio_mutex);
  }
  
  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(fx_modular_synth_audio != NULL){
    g_object_unref(fx_modular_synth_audio);
  }

  if(fx_modular_synth_audio_processor != NULL){
    g_object_unref(fx_modular_synth_audio_processor);
  }
  
  if(fx_modular_synth_channel_processor != NULL){
    g_object_unref(fx_modular_synth_channel_processor);
  }
  
  if(fx_modular_synth_recycling != NULL){
    g_object_unref(fx_modular_synth_recycling);
  }
}

/**
 * ags_fx_modular_synth_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxModularSynthAudioSignal
 *
 * Returns: the new #AgsFxModularSynthAudioSignal
 *
 * Since: 8.2.0
 */
AgsFxModularSynthAudioSignal*
ags_fx_modular_synth_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxModularSynthAudioSignal *fx_modular_synth_audio_signal;

  fx_modular_synth_audio_signal = (AgsFxModularSynthAudioSignal *) g_object_new(AGS_TYPE_FX_MODULAR_SYNTH_AUDIO_SIGNAL,
										"source", audio_signal,
										NULL);

  return(fx_modular_synth_audio_signal);
}
