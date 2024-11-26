/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_star_synth_audio_signal.h>

#include <ags/audio/ags_port.h>

#include <ags/audio/fx/ags_fx_star_synth_audio.h>
#include <ags/audio/fx/ags_fx_star_synth_audio_processor.h>
#include <ags/audio/fx/ags_fx_star_synth_channel.h>
#include <ags/audio/fx/ags_fx_star_synth_channel_processor.h>
#include <ags/audio/fx/ags_fx_star_synth_recycling.h>

#include <ags/i18n.h>

void ags_fx_star_synth_audio_signal_class_init(AgsFxStarSynthAudioSignalClass *fx_star_synth_audio_signal);
void ags_fx_star_synth_audio_signal_init(AgsFxStarSynthAudioSignal *fx_star_synth_audio_signal);
void ags_fx_star_synth_audio_signal_dispose(GObject *gobject);
void ags_fx_star_synth_audio_signal_finalize(GObject *gobject);

void ags_fx_star_synth_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
						AgsAudioSignal *source,
						AgsNote *note,
						gboolean pattern_mode,
						guint x0, guint x1,
						guint y,
						gdouble delay_counter, guint64 offset_counter,
						guint frame_count,
						gdouble delay, guint buffer_size);
void ags_fx_star_synth_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
						  AgsAudioSignal *source,
						  AgsNote *note,
						  guint x0, guint x1,
						  guint y);

/**
 * SECTION:ags_fx_star_synth_audio_signal
 * @short_description: fx star synth audio signal
 * @title: AgsFxStarSynthAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_star_synth_audio_signal.h
 *
 * The #AgsFxStarSynthAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_star_synth_audio_signal_parent_class = NULL;

const gchar *ags_fx_star_synth_audio_signal_plugin_name = "ags-fx-star-synth";

GType
ags_fx_star_synth_audio_signal_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_star_synth_audio_signal = 0;

    static const GTypeInfo ags_fx_star_synth_audio_signal_info = {
      sizeof (AgsFxStarSynthAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_star_synth_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxStarSynthAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_star_synth_audio_signal_init,
    };

    ags_type_fx_star_synth_audio_signal = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL,
								 "AgsFxStarSynthAudioSignal",
								 &ags_fx_star_synth_audio_signal_info,
								 0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_star_synth_audio_signal);
  }

  return(g_define_type_id__static);
}

void
ags_fx_star_synth_audio_signal_class_init(AgsFxStarSynthAudioSignalClass *fx_star_synth_audio_signal)
{
  GObjectClass *gobject;
  AgsFxNotationAudioSignalClass *fx_notation_audio_signal;

  ags_fx_star_synth_audio_signal_parent_class = g_type_class_peek_parent(fx_star_synth_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_star_synth_audio_signal;

  gobject->dispose = ags_fx_star_synth_audio_signal_dispose;
  gobject->finalize = ags_fx_star_synth_audio_signal_finalize;

  /* AgsFxNotationAudioSignalClass */
  fx_notation_audio_signal = (AgsFxNotationAudioSignalClass *) fx_star_synth_audio_signal;
  
  fx_notation_audio_signal->stream_feed = ags_fx_star_synth_audio_signal_stream_feed;
  fx_notation_audio_signal->notify_remove = ags_fx_star_synth_audio_signal_notify_remove;
}

void
ags_fx_star_synth_audio_signal_init(AgsFxStarSynthAudioSignal *fx_star_synth_audio_signal)
{
  AGS_RECALL(fx_star_synth_audio_signal)->name = "ags-fx-star-synth";
  AGS_RECALL(fx_star_synth_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_star_synth_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_star_synth_audio_signal)->xml_type = "ags-fx-star-synth-audio-signal";
}

void
ags_fx_star_synth_audio_signal_dispose(GObject *gobject)
{
  AgsFxStarSynthAudioSignal *fx_star_synth_audio_signal;
  
  fx_star_synth_audio_signal = AGS_FX_STAR_SYNTH_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_star_synth_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_star_synth_audio_signal_finalize(GObject *gobject)
{
  AgsFxStarSynthAudioSignal *fx_star_synth_audio_signal;
  
  fx_star_synth_audio_signal = AGS_FX_STAR_SYNTH_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_star_synth_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_star_synth_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
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
  AgsFxStarSynthAudio *fx_star_synth_audio;
  AgsFxStarSynthAudioProcessor *fx_star_synth_audio_processor;
  AgsFxStarSynthChannelProcessor *fx_star_synth_channel_processor;
  AgsFxStarSynthRecycling *fx_star_synth_recycling;
  AgsFxStarSynthAudioSignal *fx_star_synth_audio_signal;
  
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

  gboolean vibrato_enabled;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;

  gboolean synth_0_sync_enabled;
  gdouble synth_0_sync_relative_attack_factor;
  gdouble synth_0_sync_attack_0;
  gdouble synth_0_sync_phase_0;
  gdouble synth_0_sync_attack_1;
  gdouble synth_0_sync_phase_1;
  gdouble synth_0_sync_attack_2;
  gdouble synth_0_sync_phase_2;
  gdouble synth_0_sync_attack_3;
  gdouble synth_0_sync_phase_3;
  guint synth_0_sync_lfo_oscillator;
  gdouble synth_0_sync_lfo_frequency;

  gboolean synth_1_sync_enabled;
  gdouble synth_1_sync_relative_attack_factor;
  gdouble synth_1_sync_attack_0;
  gdouble synth_1_sync_phase_0;
  gdouble synth_1_sync_attack_1;
  gdouble synth_1_sync_phase_1;
  gdouble synth_1_sync_attack_2;
  gdouble synth_1_sync_phase_2;
  gdouble synth_1_sync_attack_3;
  gdouble synth_1_sync_phase_3;
  guint synth_1_sync_lfo_oscillator;
  gdouble synth_1_sync_lfo_frequency;
    
  gboolean note_256th_mode;

  gboolean chorus_enabled;

  GRecMutex *source_stream_mutex;
  GRecMutex *fx_star_synth_audio_mutex;
  GRecMutex *fx_star_synth_audio_processor_mutex;

  fx_star_synth_audio_signal = (AgsFxStarSynthAudioSignal *) fx_notation_audio_signal;
  
  audio = NULL;
  
  fx_star_synth_audio = NULL;
  fx_star_synth_audio_processor = NULL;
  
  fx_star_synth_channel_processor = NULL;

  fx_star_synth_recycling = NULL;

  sound_scope = ags_recall_get_sound_scope((AgsRecall *) fx_notation_audio_signal);

  audio_channel = 0;
  
  audio_start_mapping = 0;
  midi_start_mapping = 0;

  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  octave = 0.0;
  key = 0.0;
    
  vibrato_enabled = FALSE;
  vibrato_gain = 1.0;
  vibrato_lfo_depth = 1.0;
  vibrato_lfo_freq = 8.172;
  vibrato_tuning = 0.0;

  synth_0_sync_enabled = FALSE;

  synth_0_sync_relative_attack_factor = 1.0;

  synth_0_sync_attack_0 = 0.0;
  synth_0_sync_phase_0 = 0.0;

  synth_0_sync_attack_1 = 0.0;
  synth_0_sync_phase_1 = 0.0;

  synth_0_sync_attack_2 = 0.0;
  synth_0_sync_phase_2 = 0.0;

  synth_0_sync_attack_3 = 0.0;
  synth_0_sync_phase_3 = 0.0;

  synth_0_sync_lfo_oscillator = AGS_SYNTH_OSCILLATOR_SIN;
  synth_0_sync_lfo_frequency = 10.0;

  synth_1_sync_enabled = FALSE;

  synth_1_sync_relative_attack_factor = 1.0;

  synth_1_sync_attack_0 = 0.0;
  synth_1_sync_phase_0 = 0.0;

  synth_1_sync_attack_1 = 0.0;
  synth_1_sync_phase_1 = 0.0;

  synth_1_sync_attack_2 = 0.0;
  synth_1_sync_phase_2 = 0.0;

  synth_1_sync_attack_3 = 0.0;
  synth_1_sync_phase_3 = 0.0;

  synth_1_sync_lfo_oscillator = AGS_SYNTH_OSCILLATOR_SIN;
  synth_1_sync_lfo_frequency = 10.0;

  note_256th_mode = ags_fx_notation_audio_get_note_256th_mode((AgsFxNotationAudio *) fx_star_synth_audio);

  chorus_enabled = FALSE;
  
  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_star_synth_recycling,
	       NULL);

  g_object_get(fx_star_synth_recycling,
	       "parent", &fx_star_synth_channel_processor,
	       NULL);
  
  g_object_get(fx_star_synth_channel_processor,
	       "recall-audio", &fx_star_synth_audio,
	       "recall-audio-run", &fx_star_synth_audio_processor,
	       NULL);

  g_object_get(fx_star_synth_audio,
	       "audio", &audio,
	       NULL);

  g_object_get(fx_star_synth_audio_processor,
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

  audio_buffer_util_format = ags_audio_buffer_util_format_from_soundcard(&(fx_star_synth_audio_signal->audio_buffer_util),
									 format);

  copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(&(fx_star_synth_audio_signal->audio_buffer_util),
							      audio_buffer_util_format,
							      audio_buffer_util_format);
  
  /* get synth mutex */
  fx_star_synth_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_star_synth_audio);
  fx_star_synth_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_star_synth_audio_processor);

  g_rec_mutex_lock(fx_star_synth_audio_processor_mutex);

  note_256th_offset_lower = AGS_FX_NOTATION_AUDIO_PROCESSOR(fx_star_synth_audio_processor)->note_256th_offset_lower;

  note_256th_delay = AGS_FX_NOTATION_AUDIO_PROCESSOR(fx_star_synth_audio_processor)->note_256th_delay;
  
  g_rec_mutex_unlock(fx_star_synth_audio_processor_mutex);

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

  if(fx_star_synth_audio != NULL){
    AgsPort *port;

    AgsFxStarSynthAudioChannelData *channel_data;

    GValue value = {0,};

    channel_data = fx_star_synth_audio->scope_data[sound_scope]->channel_data[audio_channel];
    
    /* synth-0 oscillator */
    g_object_get(fx_star_synth_audio,
		 "synth-0-oscillator", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_star_synth_util_set_synth_oscillator_mode(channel_data->star_synth_0,
						    (guint) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* synth-0 octave */
    g_object_get(fx_star_synth_audio,
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
    g_object_get(fx_star_synth_audio,
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

    ags_star_synth_util_set_frequency(channel_data->star_synth_0,
				      exp2(((octave * 12.0) + key + ((gdouble) midi_note - 48.0)) / 12.0) * 440.0);

    //    g_message("synth 0 - osc = %d, frequency = %f, octave = %f, key= %f", channel_data->star_synth_0.synth_oscillator_mode, channel_data->star_synth_0.frequency, octave, key);
      
    /* synth-0 phase */    
    g_object_get(fx_star_synth_audio,
		 "synth-0-phase", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_star_synth_util_set_phase(channel_data->star_synth_0,
				    ((gdouble) samplerate / ags_star_synth_util_get_frequency(channel_data->star_synth_0)) * ((gdouble) g_value_get_float(&value) / (2.0 * M_PI)));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 volume */    
    g_object_get(fx_star_synth_audio,
		 "synth-0-volume", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_star_synth_util_set_volume(channel_data->star_synth_0,
				     (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 sync enabled */    
    g_object_get(fx_star_synth_audio,
		 "synth-0-sync-enabled", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      synth_0_sync_enabled = (gboolean) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    if(synth_0_sync_enabled){
      /* synth-0 sync relative attack factor */    
      g_object_get(fx_star_synth_audio,
		   "synth-0-sync-relative-attack-factor", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	synth_0_sync_relative_attack_factor = (gboolean) g_value_get_float(&value);
      
	g_object_unref(port);
      }

      g_value_unset(&value);
      
      /* synth-0 sync attack 0 */
      g_object_get(fx_star_synth_audio,
		   "synth-0-sync-attack-0", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	synth_0_sync_attack_0 = (gdouble) g_value_get_float(&value);
      
	g_object_unref(port);
      }

      g_value_unset(&value);
      
      /* synth-0 sync phase 0 */
      g_object_get(fx_star_synth_audio,
		   "synth-0-sync-phase-0", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	synth_0_sync_phase_0 = (gdouble) g_value_get_float(&value);
      
	g_object_unref(port);
      }

      g_value_unset(&value);

      /* synth-0 sync attack 1 */
      g_object_get(fx_star_synth_audio,
		   "synth-0-sync-attack-1", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	synth_0_sync_attack_1 = (gdouble) g_value_get_float(&value);
      
	g_object_unref(port);
      }

      g_value_unset(&value);
      
      /* synth-0 sync phase 1 */
      g_object_get(fx_star_synth_audio,
		   "synth-0-sync-phase-1", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	synth_0_sync_phase_1 = (gdouble) g_value_get_float(&value);
      
	g_object_unref(port);
      }

      g_value_unset(&value);

      /* synth-0 sync attack 2 */
      g_object_get(fx_star_synth_audio,
		   "synth-0-sync-attack-2", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	synth_0_sync_attack_2 = (gdouble) g_value_get_float(&value);
      
	g_object_unref(port);
      }

      g_value_unset(&value);
      
      /* synth-0 sync phase 2 */
      g_object_get(fx_star_synth_audio,
		   "synth-0-sync-phase-2", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	synth_0_sync_phase_2 = (gdouble) g_value_get_float(&value);
      
	g_object_unref(port);
      }

      g_value_unset(&value);

      /* synth-0 sync attack 3 */
      g_object_get(fx_star_synth_audio,
		   "synth-0-sync-attack-3", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	synth_0_sync_attack_3 = (gdouble) g_value_get_float(&value);
      
	g_object_unref(port);
      }

      g_value_unset(&value);
      
      /* synth-0 sync phase 3 */
      g_object_get(fx_star_synth_audio,
		   "synth-0-sync-phase-3", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	synth_0_sync_phase_3 = (gdouble) g_value_get_float(&value);
      
	g_object_unref(port);
      }

      g_value_unset(&value);
    }
    
    /* synth-0 LFO oscillator */    
    g_object_get(fx_star_synth_audio,
		 "synth-0-lfo-oscillator", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_star_synth_util_set_lfo_oscillator_mode(channel_data->star_synth_0,
						  (guint) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 LFO frequency */    
    g_object_get(fx_star_synth_audio,
		 "synth-0-lfo-frequency", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_star_synth_util_set_lfo_frequency(channel_data->star_synth_0,
					    (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 LFO depth */    
    g_object_get(fx_star_synth_audio,
		 "synth-0-lfo-depth", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_star_synth_util_set_lfo_depth(channel_data->star_synth_0,
					(gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-0 LFO tuning */    
    g_object_get(fx_star_synth_audio,
		 "synth-0-lfo-tuning", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_star_synth_util_set_tuning(channel_data->star_synth_0,
				     (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* synth-1 oscillator */    
    g_object_get(fx_star_synth_audio,
		 "synth-1-oscillator", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_star_synth_util_set_synth_oscillator_mode(channel_data->star_synth_1,
						    (guint) g_value_get_float(&value));
            
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* synth-1 octave */
    octave = 0.0;
    key = 0.0;
    
    g_object_get(fx_star_synth_audio,
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
    g_object_get(fx_star_synth_audio,
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

    ags_star_synth_util_set_frequency(channel_data->star_synth_1,
				      exp2(((octave * 12.0) + key + ((gdouble) midi_note - 48.0)) / 12.0) * 440.0);

    //    g_message("synth 2 - osc = %d, frequency = %f, octave = %f, key= %f", channel_data->star_synth_1.synth_oscillator_mode, channel_data->star_synth_1.frequency, octave, key);
      
    /* synth-1 phase */    
    g_object_get(fx_star_synth_audio,
		 "synth-1-phase", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_star_synth_util_set_phase(channel_data->star_synth_1,
				    ((gdouble) samplerate / ags_star_synth_util_get_frequency(channel_data->star_synth_1)) * ((gdouble) g_value_get_float(&value) / (2.0 * M_PI)));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-1 volume */    
    g_object_get(fx_star_synth_audio,
		 "synth-1-volume", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_star_synth_util_set_volume(channel_data->star_synth_1,
				     (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-1 sync enabled */    
    g_object_get(fx_star_synth_audio,
		 "synth-1-sync-enabled", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      synth_1_sync_enabled = (gboolean) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    if(synth_1_sync_enabled){
      /* synth-1 sync relative attack factor */    
      g_object_get(fx_star_synth_audio,
		   "synth-1-sync-relative-attack-factor", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	synth_1_sync_relative_attack_factor = (gboolean) g_value_get_float(&value);
      
	g_object_unref(port);
      }

      g_value_unset(&value);

      /* synth-1 sync attack 0 */
      g_object_get(fx_star_synth_audio,
		   "synth-1-sync-attack-0", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	synth_1_sync_attack_0 = (gdouble) g_value_get_float(&value);
      
	g_object_unref(port);
      }

      g_value_unset(&value);
      
      /* synth-1 sync phase 0 */
      g_object_get(fx_star_synth_audio,
		   "synth-1-sync-phase-0", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	synth_1_sync_phase_0 = (gdouble) g_value_get_float(&value);
      
	g_object_unref(port);
      }

      g_value_unset(&value);

      /* synth-1 sync attack 1 */
      g_object_get(fx_star_synth_audio,
		   "synth-1-sync-attack-1", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	synth_1_sync_attack_1 = (gdouble) g_value_get_float(&value);
      
	g_object_unref(port);
      }

      g_value_unset(&value);
      
      /* synth-1 sync phase 1 */
      g_object_get(fx_star_synth_audio,
		   "synth-1-sync-phase-1", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	synth_1_sync_phase_1 = (gdouble) g_value_get_float(&value);
      
	g_object_unref(port);
      }

      g_value_unset(&value);

      /* synth-1 sync attack 2 */
      g_object_get(fx_star_synth_audio,
		   "synth-1-sync-attack-2", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	synth_1_sync_attack_2 = (gdouble) g_value_get_float(&value);
      
	g_object_unref(port);
      }

      g_value_unset(&value);
      
      /* synth-1 sync phase 2 */
      g_object_get(fx_star_synth_audio,
		   "synth-1-sync-phase-2", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	synth_1_sync_phase_2 = (gdouble) g_value_get_float(&value);
      
	g_object_unref(port);
      }

      g_value_unset(&value);

      /* synth-1 sync attack 3 */
      g_object_get(fx_star_synth_audio,
		   "synth-1-sync-attack-3", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	synth_1_sync_attack_3 = (gdouble) g_value_get_float(&value);
      
	g_object_unref(port);
      }

      g_value_unset(&value);
      
      /* synth-1 sync phase 3 */
      g_object_get(fx_star_synth_audio,
		   "synth-1-sync-phase-3", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	synth_1_sync_phase_3 = (gdouble) g_value_get_float(&value);
      
	g_object_unref(port);
      }

      g_value_unset(&value);
    }
    
    /* synth-1 LFO oscillator */    
    g_object_get(fx_star_synth_audio,
		 "synth-1-lfo-oscillator", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_star_synth_util_set_lfo_oscillator_mode(channel_data->star_synth_1,
						  (guint) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-1 LFO frequency */    
    g_object_get(fx_star_synth_audio,
		 "synth-1-lfo-frequency", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_star_synth_util_set_lfo_frequency(channel_data->star_synth_1,
					    (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-1 LFO depth */    
    g_object_get(fx_star_synth_audio,
		 "synth-1-lfo-depth", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_star_synth_util_set_lfo_depth(channel_data->star_synth_1,
					(gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* synth-1 LFO tuning */    
    g_object_get(fx_star_synth_audio,
		 "synth-1-lfo-tuning", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_star_synth_util_set_tuning(channel_data->star_synth_1,
				     (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* noise volume */    
    g_object_get(fx_star_synth_audio,
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
    //    ags_common_pitch_util_set_base_key(channel_data->pitch_util,
    //				       channel_data->pitch_type,
    //				       (gdouble) midi_note - 48.0);
    
    /* pitch tuning */    
    g_object_get(fx_star_synth_audio,
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

    /* vibrato enabled */
    g_object_get(fx_star_synth_audio,
		 "vibrato-enabled", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      if(g_value_get_float(&value) != 0.0){
	vibrato_enabled = TRUE;
      }
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* vibrato gain */
    g_object_get(fx_star_synth_audio,
		 "vibrato-gain", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      vibrato_gain = (gdouble) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* vibrato LFO depth */
    g_object_get(fx_star_synth_audio,
		 "vibrato-lfo-depth", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      vibrato_lfo_depth = (gdouble) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* vibrato LFO freq */
    g_object_get(fx_star_synth_audio,
		 "vibrato-lfo-freq", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      vibrato_lfo_freq = (gdouble) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* vibrato tuning */
    g_object_get(fx_star_synth_audio,
		 "vibrato-tuning", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      vibrato_tuning = (gdouble) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* chorus enabled */    
    g_object_get(fx_star_synth_audio,
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
      g_object_get(fx_star_synth_audio,
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
      g_object_get(fx_star_synth_audio,
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
      g_object_get(fx_star_synth_audio,
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
      g_object_get(fx_star_synth_audio,
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
      g_object_get(fx_star_synth_audio,
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
      g_object_get(fx_star_synth_audio,
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
      g_object_get(fx_star_synth_audio,
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

  copy_mode_out = ags_audio_buffer_util_get_copy_mode_from_format(&(fx_star_synth_audio_signal->audio_buffer_util),
								  ags_audio_buffer_util_format_from_soundcard(&(fx_star_synth_audio_signal->audio_buffer_util),
													      format),
								  AGS_AUDIO_BUFFER_UTIL_FLOAT);

  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxStarSynthAudioScopeData *scope_data;
    AgsFxStarSynthAudioChannelData *channel_data;
    AgsFxStarSynthAudioInputData *input_data;

    g_rec_mutex_lock(fx_star_synth_audio_mutex);

    scope_data = fx_star_synth_audio->scope_data[sound_scope];

    channel_data = scope_data->channel_data[audio_channel];

    input_data = channel_data->input_data[midi_note];

    g_rec_mutex_unlock(fx_star_synth_audio_mutex);

    if((gint) floor(delay_counter) == 0 &&
       x0 == offset_counter){
      g_rec_mutex_lock(fx_star_synth_audio_mutex);

      input_data->key_on += 1;

      g_rec_mutex_unlock(fx_star_synth_audio_mutex);
    }

    /* star synth 0 */
    ags_star_synth_util_set_source(channel_data->star_synth_0,
				   source->stream_current->data);
    ags_star_synth_util_set_source_stride(channel_data->star_synth_0,
					  1);

    ags_star_synth_util_set_buffer_length(channel_data->star_synth_0,
					  buffer_size);
    ags_star_synth_util_set_format(channel_data->star_synth_0,
				   format);
    ags_star_synth_util_set_samplerate(channel_data->star_synth_0,
				       samplerate);

    if(!note_256th_mode){
      ags_star_synth_util_set_frame_count(channel_data->star_synth_0,
					  floor(((offset_counter - x0) * delay + delay_counter + 1.0) * buffer_size));

      ags_star_synth_util_set_offset(channel_data->star_synth_0,
				     floor(((offset_counter - x0) * delay + delay_counter) * buffer_size));
    }else{
      g_rec_mutex_lock(fx_star_synth_audio_processor_mutex);

      note_256th_offset_lower = AGS_FX_NOTATION_AUDIO_PROCESSOR(fx_star_synth_audio_processor)->note_256th_offset_lower;

      note_256th_delay = AGS_FX_NOTATION_AUDIO_PROCESSOR(fx_star_synth_audio_processor)->note_256th_delay;

      g_rec_mutex_unlock(fx_star_synth_audio_processor_mutex);

      ags_star_synth_util_set_frame_count(channel_data->star_synth_0,
					  (guint) floor((double) (note_256th_offset_lower - x0_256th) * note_256th_delay * (double) buffer_size) + (guint) floor(delay * (double) buffer_size));

      ags_star_synth_util_set_offset(channel_data->star_synth_0,
				     (guint) floor(((double) (offset_counter - x0) * delay + delay_counter) * (double) buffer_size));

      ags_star_synth_util_set_offset_256th(channel_data->star_synth_0,
					   (guint) floor((double) (note_256th_offset_lower - x0_256th) * note_256th_delay * (double) buffer_size));
    }
    
    ags_star_synth_util_set_frame_count(channel_data->star_synth_0,
					floor(((offset_counter - x0) * delay + delay_counter + 1.0) * buffer_size));
    ags_star_synth_util_set_offset(channel_data->star_synth_0,
				   floor(((offset_counter - x0) * delay + delay_counter) * buffer_size));
    
    if(synth_0_sync_enabled){
      guint offset;
      gdouble freq_period;
      guint current_sync, next_sync;
      guint sync_reset;
      guint word_size;
      guint i, i_reset, i_stop;

      offset = ags_star_synth_util_get_offset(channel_data->star_synth_0);
      
      freq_period = samplerate / ags_star_synth_util_get_frequency(channel_data->star_synth_0);

      word_size = sizeof(gint16);

      switch(format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
	{
	  word_size = sizeof(gint8);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  word_size = sizeof(gint16);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
	{
	  word_size = sizeof(gint32);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
	{
	  word_size = sizeof(gint32);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_64_BIT:
	{
	  word_size = sizeof(gint64);
	}
	break;
      case AGS_SOUNDCARD_FLOAT:
	{
	  word_size = sizeof(gfloat);
	}
	break;
      case AGS_SOUNDCARD_DOUBLE:
	{
	  word_size = sizeof(gdouble);
	}
	break;
      }
      
      for(i = 0, i_reset = 0, sync_reset = 0, current_sync = 0, next_sync = 0; i < offset + buffer_size;){
	if(i_reset + sync_reset == i){
	  switch(current_sync){
	  case 0:
	    {
	      if(synth_0_sync_attack_0 > 0.0){
		sync_reset = (1.0 - synth_0_sync_relative_attack_factor * ((gdouble) midi_note / 128.0)) * ((synth_0_sync_attack_0 / (2.0 * M_PI)) * freq_period);
		i_reset = i;
	      
		if(synth_0_sync_attack_1 > 0.0){
		  next_sync = 1;
		}
	      }
	    }
	    break;
	  case 1:
	    {
	      if(synth_0_sync_attack_1 > 0.0){
		sync_reset = (1.0 - synth_0_sync_relative_attack_factor * ((gdouble) midi_note / 128.0)) * ((synth_0_sync_attack_1 / (2.0 * M_PI)) * freq_period);
		i_reset = i;

		if(synth_0_sync_attack_2 > 0.0){
		  next_sync = 2;
		}
	      }
	    }
	    break;
	  case 2:
	    {
	      if(synth_0_sync_attack_2 > 0.0){
		sync_reset = (1.0 - synth_0_sync_relative_attack_factor * ((gdouble) midi_note / 128.0)) * ((synth_0_sync_attack_2 / (2.0 * M_PI)) * freq_period);
		i_reset = i;

		next_sync = 3;
	      }
	    }
	    break;
	  case 3:
	    {
	      if(synth_0_sync_attack_3 > 0.0){
		sync_reset = (1.0 - synth_0_sync_relative_attack_factor * ((gdouble) midi_note / 128.0)) * ((synth_0_sync_attack_2 / (2.0 * M_PI)) * freq_period);
		i_reset = i;

		next_sync = 0;
	      }
	    }
	    break;
	  }
	}

	//	g_message("synth 0 - offset = %d, i = %d, i_reset = %d, sync_reset = %d", channel_data->star_synth_0.offset, i, i_reset, sync_reset);
	
	if(i >= ags_star_synth_util_get_offset(channel_data->star_synth_0)){
	  ags_star_synth_util_set_offset(channel_data->star_synth_0,
					 i);

	  if(sync_reset != 0 && i_reset + sync_reset < buffer_size * (guint) floor(i / buffer_size) + buffer_size){
	    ags_star_synth_util_set_source(channel_data->star_synth_0,
					   ((guchar *) source->stream_current->data) + ((i % buffer_size) * word_size));

	    ags_star_synth_util_set_buffer_length(channel_data->star_synth_0,
						  (i_reset + sync_reset) - i);
	  }else{
	    ags_star_synth_util_set_source(channel_data->star_synth_0,
					   ((guchar *) source->stream_current->data) + ((i % buffer_size) * word_size));

	    ags_star_synth_util_set_buffer_length(channel_data->star_synth_0,
						  ((buffer_size * (guint) floor(i / buffer_size) + buffer_size) - i));
	  }

	  if(sync_reset != 0){
	    gdouble lfo_factor;
	    
	    lfo_factor = 1.0;	    
	    
	    switch(synth_0_sync_lfo_oscillator){
	    case AGS_SYNTH_OSCILLATOR_SIN:
	      {
		lfo_factor = (sin(i * 2.0 * M_PI * synth_0_sync_lfo_frequency / samplerate));
	      }
	      break;
	    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
	      {
		lfo_factor = (((fmod(((gdouble) i), samplerate / synth_0_sync_lfo_frequency) * 2.0 * synth_0_sync_lfo_frequency / samplerate) - 1.0));
	      }
	      break;
	    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
	      {
		lfo_factor = ((((i) * synth_0_sync_lfo_frequency / samplerate * 2.0) - (((double) (((i) * synth_0_sync_lfo_frequency / samplerate)) / 2.0) * 2.0) - 1.0));
	      }
	      break;
	    case AGS_SYNTH_OSCILLATOR_SQUARE:
	      {
		lfo_factor = ((sin((gdouble) (i) * 2.0 * M_PI * synth_0_sync_lfo_frequency / (gdouble) samplerate) >= 0.0 ? 1.0: -1.0));
	      }
	      break;
	    case AGS_SYNTH_OSCILLATOR_IMPULSE:
	      {
		lfo_factor = ((sin((gdouble) (i) * 2.0 * M_PI * synth_0_sync_lfo_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0));
	      }
	      break;
	    }      

	    switch(current_sync){
	    case 0:
	      {
		ags_star_synth_util_set_phase(channel_data->star_synth_0,
					      (synth_0_sync_phase_0 / (2.0 * M_PI)) * freq_period * lfo_factor);
	      }
	      break;
	    case 1:
	      {
		ags_star_synth_util_set_phase(channel_data->star_synth_0,
					      (synth_0_sync_phase_1 / (2.0 * M_PI)) * freq_period * lfo_factor);
	      }
	      break;
	    case 2:
	      {
		ags_star_synth_util_set_phase(channel_data->star_synth_0,
					      (synth_0_sync_phase_2 / (2.0 * M_PI)) * freq_period * lfo_factor);
	      }
	      break;
	    case 3:
	      {
		ags_star_synth_util_set_phase(channel_data->star_synth_0,
					      (synth_0_sync_phase_3 / (2.0 * M_PI)) * freq_period * lfo_factor);
	      }
	      break;
	    }
	  }
	  
	  g_rec_mutex_lock(source_stream_mutex);
	  
	  switch(ags_star_synth_util_get_synth_oscillator_mode(channel_data->star_synth_0)){
	  case AGS_SYNTH_OSCILLATOR_SIN:
	    {
	      ags_star_synth_util_compute_sin(channel_data->star_synth_0);
	    }
	    break;
	  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
	    {
	      ags_star_synth_util_compute_sawtooth(channel_data->star_synth_0);
	    }
	    break;
	  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
	    {
	      ags_star_synth_util_compute_triangle(channel_data->star_synth_0);
	    }
	    break;
	  case AGS_SYNTH_OSCILLATOR_SQUARE:
	    {
	      ags_star_synth_util_compute_square(channel_data->star_synth_0);
	    }
	    break;
	  case AGS_SYNTH_OSCILLATOR_IMPULSE:
	    {
	      ags_star_synth_util_compute_impulse(channel_data->star_synth_0);
	    }
	    break;
	  }
	}

	g_rec_mutex_unlock(source_stream_mutex);

	if(sync_reset != 0 && i_reset + sync_reset < buffer_size * (guint) floor(i / buffer_size) + buffer_size){
	  i = i_reset + sync_reset;

	  current_sync = next_sync;
	}else{
	  if(((buffer_size * (guint) floor(i / buffer_size) + buffer_size) - i) != 0){
	    i += ((buffer_size * (guint) floor(i / buffer_size) + buffer_size) - i);
	  }else{
	    i++;
	  }
	}
      }      
    }else{
      g_rec_mutex_lock(source_stream_mutex);

      switch(ags_star_synth_util_get_synth_oscillator_mode(channel_data->star_synth_0)){
      case AGS_SYNTH_OSCILLATOR_SIN:
	{
	  ags_star_synth_util_compute_sin(channel_data->star_synth_0);
	}
	break;
      case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
	{
	  ags_star_synth_util_compute_sawtooth(channel_data->star_synth_0);
	}
	break;
      case AGS_SYNTH_OSCILLATOR_TRIANGLE:
	{
	  ags_star_synth_util_compute_triangle(channel_data->star_synth_0);
	}
	break;
      case AGS_SYNTH_OSCILLATOR_SQUARE:
	{
	  ags_star_synth_util_compute_square(channel_data->star_synth_0);
	}
	break;
      case AGS_SYNTH_OSCILLATOR_IMPULSE:
	{
	  ags_star_synth_util_compute_impulse(channel_data->star_synth_0);
	}
	break;
      }

      g_rec_mutex_unlock(source_stream_mutex);
    }

    ags_star_synth_util_set_source(channel_data->star_synth_0,
				   NULL);
    
    /* star synth 1 */
    ags_star_synth_util_set_source(channel_data->star_synth_1,
				   source->stream_current->data);
    ags_star_synth_util_set_source_stride(channel_data->star_synth_1,
					  1);

    ags_star_synth_util_set_buffer_length(channel_data->star_synth_1,
					  buffer_size);
    ags_star_synth_util_set_format(channel_data->star_synth_1,
				   format);
    ags_star_synth_util_set_samplerate(channel_data->star_synth_1,
				       samplerate);

    if(!note_256th_mode){
      ags_star_synth_util_set_frame_count(channel_data->star_synth_1,
					  floor(((offset_counter - x0) * delay + delay_counter + 1.0) * buffer_size));

      ags_star_synth_util_set_offset(channel_data->star_synth_1,
				     floor(((offset_counter - x0) * delay + delay_counter) * buffer_size));
    }else{
      g_rec_mutex_lock(fx_star_synth_audio_processor_mutex);

      note_256th_offset_lower = AGS_FX_NOTATION_AUDIO_PROCESSOR(fx_star_synth_audio_processor)->note_256th_offset_lower;

      note_256th_delay = AGS_FX_NOTATION_AUDIO_PROCESSOR(fx_star_synth_audio_processor)->note_256th_delay;

      g_rec_mutex_unlock(fx_star_synth_audio_processor_mutex);

      ags_star_synth_util_set_frame_count(channel_data->star_synth_1,
					  (guint) floor((double) (note_256th_offset_lower - x0_256th) * note_256th_delay * (double) buffer_size) + (guint) floor(delay * (double) buffer_size));

      ags_star_synth_util_set_offset(channel_data->star_synth_1,
				     (guint) floor(((double) (offset_counter - x0) * delay + delay_counter) * (double) buffer_size));

      ags_star_synth_util_set_offset_256th(channel_data->star_synth_1,
					   (guint) floor((double) (note_256th_offset_lower - x0_256th) * note_256th_delay * (double) buffer_size));
    }

    g_rec_mutex_unlock(source_stream_mutex);

    if(synth_1_sync_enabled){
      guint offset;
      gdouble freq_period;
      guint current_sync, next_sync;
      guint sync_reset;
      guint word_size;
      guint i, i_reset, i_stop;

      offset = ags_star_synth_util_get_offset(channel_data->star_synth_1);
      
      freq_period = samplerate / ags_star_synth_util_get_frequency(channel_data->star_synth_1);

      word_size = sizeof(gint16);

      switch(format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
	{
	  word_size = sizeof(gint8);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  word_size = sizeof(gint16);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
	{
	  word_size = sizeof(gint32);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
	{
	  word_size = sizeof(gint32);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_64_BIT:
	{
	  word_size = sizeof(gint64);
	}
	break;
      case AGS_SOUNDCARD_FLOAT:
	{
	  word_size = sizeof(gfloat);
	}
	break;
      case AGS_SOUNDCARD_DOUBLE:
	{
	  word_size = sizeof(gdouble);
	}
	break;
      }
      
      for(i = 0, i_reset = 0, sync_reset = 0, current_sync = 0, next_sync = 0; i < offset + buffer_size;){
	if(i_reset + sync_reset == i){
	  switch(current_sync){
	  case 0:
	    {
	      if(synth_1_sync_attack_0 > 0.0){
		sync_reset = (1.0 - synth_1_sync_relative_attack_factor * ((gdouble) midi_note / 128.0)) * ((synth_1_sync_attack_0 / (2.0 * M_PI)) * freq_period);
		i_reset = i;
	      
		if(synth_1_sync_attack_1 > 0.0){
		  next_sync = 1;
		}
	      }
	    }
	    break;
	  case 1:
	    {
	      if(synth_1_sync_attack_1 > 0.0){
		sync_reset = (1.0 - synth_1_sync_relative_attack_factor * ((gdouble) midi_note / 128.0)) * ((synth_1_sync_attack_1 / (2.0 * M_PI)) * freq_period);
		i_reset = i;

		if(synth_1_sync_attack_2 > 0.0){
		  next_sync = 2;
		}
	      }
	    }
	    break;
	  case 2:
	    {
	      if(synth_1_sync_attack_2 > 0.0){
		sync_reset = (1.0 - synth_1_sync_relative_attack_factor * ((gdouble) midi_note / 128.0)) * ((synth_1_sync_attack_2 / (2.0 * M_PI)) * freq_period);
		i_reset = i;

		next_sync = 3;
	      }
	    }
	    break;
	  case 3:
	    {
	      if(synth_1_sync_attack_3 > 0.0){
		sync_reset = (1.0 - synth_1_sync_relative_attack_factor * ((gdouble) midi_note / 128.0)) * ((synth_1_sync_attack_3 / (2.0 * M_PI)) * freq_period);
		i_reset = i;

		next_sync = 0;
	      }
	    }
	    break;
	  }
	}

	//	g_message("synth 1 - offset = %d, i = %d, i_reset = %d, sync_reset = %d", channel_data->star_synth_1.offset, i, i_reset, sync_reset);
	
	if(i >= ags_star_synth_util_get_offset(channel_data->star_synth_1)){
	  ags_star_synth_util_set_offset(channel_data->star_synth_1,
					 i);

	  if(sync_reset != 0 && i_reset + sync_reset < buffer_size * (guint) floor(i / buffer_size) + buffer_size){
	    ags_star_synth_util_set_source(channel_data->star_synth_1,
					   ((guchar *) source->stream_current->data) + ((i % buffer_size) * word_size));

	    ags_star_synth_util_set_buffer_length(channel_data->star_synth_1,
						  (i_reset + sync_reset) - i);
	  }else{
	    ags_star_synth_util_set_source(channel_data->star_synth_1,
					   ((guchar *) source->stream_current->data) + ((i % buffer_size) * word_size));

	    ags_star_synth_util_set_buffer_length(channel_data->star_synth_1,
						  ((buffer_size * (guint) floor(i / buffer_size) + buffer_size) - i));
	  }

	  if(sync_reset != 0){
	    gdouble lfo_factor;

	    lfo_factor = 1.0;
	    
	    switch(synth_1_sync_lfo_oscillator){
	    case AGS_SYNTH_OSCILLATOR_SIN:
	      {
		lfo_factor = (sin(i * 2.0 * M_PI * synth_1_sync_lfo_frequency / samplerate));
	      }
	      break;
	    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
	      {
		lfo_factor = (((fmod(((gdouble) i), samplerate / synth_1_sync_lfo_frequency) * 2.0 * synth_1_sync_lfo_frequency / samplerate) - 1.0));
	      }
	      break;
	    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
	      {
		lfo_factor = ((((i) * synth_1_sync_lfo_frequency / samplerate * 2.0) - (((double) (((i) * synth_1_sync_lfo_frequency / samplerate)) / 2.0) * 2.0) - 1.0));
	      }
	      break;
	    case AGS_SYNTH_OSCILLATOR_SQUARE:
	      {
		lfo_factor = ((sin((gdouble) (i) * 2.0 * M_PI * synth_1_sync_lfo_frequency / (gdouble) samplerate) >= 0.0 ? 1.0: -1.0));
	      }
	      break;
	    case AGS_SYNTH_OSCILLATOR_IMPULSE:
	      {
		lfo_factor = ((sin((gdouble) (i) * 2.0 * M_PI * synth_1_sync_lfo_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0));
	      }
	      break;
	    }      

	    switch(current_sync){
	    case 0:
	      {
		ags_star_synth_util_set_phase(channel_data->star_synth_1,
					      (synth_1_sync_phase_0 / (2.0 * M_PI)) * freq_period * lfo_factor);
	      }
	      break;
	    case 1:
	      {
		ags_star_synth_util_set_phase(channel_data->star_synth_1,
					      (synth_1_sync_phase_1 / (2.0 * M_PI)) * freq_period * lfo_factor);
	      }
	      break;
	    case 2:
	      {
		ags_star_synth_util_set_phase(channel_data->star_synth_1,
					      (synth_1_sync_phase_2 / (2.0 * M_PI)) * freq_period * lfo_factor);
	      }
	      break;
	    case 3:
	      {
		ags_star_synth_util_set_phase(channel_data->star_synth_1,
					      (synth_1_sync_phase_3 / (2.0 * M_PI)) * freq_period * lfo_factor);
	      }
	      break;
	    }
	  }
	  
	  g_rec_mutex_lock(source_stream_mutex);
	  
	  switch(ags_star_synth_util_get_synth_oscillator_mode(channel_data->star_synth_1)){
	  case AGS_SYNTH_OSCILLATOR_SIN:
	    {
	      ags_star_synth_util_compute_sin(channel_data->star_synth_1);
	    }
	    break;
	  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
	    {
	      ags_star_synth_util_compute_sawtooth(channel_data->star_synth_1);
	    }
	    break;
	  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
	    {
	      ags_star_synth_util_compute_triangle(channel_data->star_synth_1);
	    }
	    break;
	  case AGS_SYNTH_OSCILLATOR_SQUARE:
	    {
	      ags_star_synth_util_compute_square(channel_data->star_synth_1);
	    }
	    break;
	  case AGS_SYNTH_OSCILLATOR_IMPULSE:
	    {
	      ags_star_synth_util_compute_impulse(channel_data->star_synth_1);
	    }
	    break;
	  }
	}

	g_rec_mutex_unlock(source_stream_mutex);

	if(sync_reset != 0 && i_reset + sync_reset < buffer_size * (guint) floor(i / buffer_size) + buffer_size){
	  i = i_reset + sync_reset;

	  current_sync = next_sync;
	}else{
	  if(((buffer_size * (guint) floor(i / buffer_size) + buffer_size) - i) != 0){
	    i += ((buffer_size * (guint) floor(i / buffer_size) + buffer_size) - i);
	  }else{
	    i++;
	  }
	}
      }      
    }else{
      g_rec_mutex_lock(source_stream_mutex);

      switch(ags_star_synth_util_get_synth_oscillator_mode(channel_data->star_synth_1)){
      case AGS_SYNTH_OSCILLATOR_SIN:
	{
	  ags_star_synth_util_compute_sin(channel_data->star_synth_1);
	}
	break;
      case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
	{
	  ags_star_synth_util_compute_sawtooth(channel_data->star_synth_1);
	}
	break;
      case AGS_SYNTH_OSCILLATOR_TRIANGLE:
	{
	  ags_star_synth_util_compute_triangle(channel_data->star_synth_1);
	}
	break;
      case AGS_SYNTH_OSCILLATOR_SQUARE:
	{
	  ags_star_synth_util_compute_square(channel_data->star_synth_1);
	}
	break;
      case AGS_SYNTH_OSCILLATOR_IMPULSE:
	{
	  ags_star_synth_util_compute_impulse(channel_data->star_synth_1);
	}
	break;
      }
    }
    
    ags_star_synth_util_set_source(channel_data->star_synth_1,
				   NULL);

    g_rec_mutex_lock(source_stream_mutex);
    
    switch(ags_star_synth_util_get_synth_oscillator_mode(channel_data->star_synth_1)){
    case AGS_SYNTH_OSCILLATOR_SIN:
      {
	ags_star_synth_util_compute_sin(channel_data->star_synth_1);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
      {
	ags_star_synth_util_compute_sawtooth(channel_data->star_synth_1);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
      {
	ags_star_synth_util_compute_triangle(channel_data->star_synth_1);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
      {
	ags_star_synth_util_compute_square(channel_data->star_synth_1);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
      {
	ags_star_synth_util_compute_impulse(channel_data->star_synth_1);
      }
      break;
    }

    g_rec_mutex_unlock(source_stream_mutex);

    ags_star_synth_util_set_source(channel_data->star_synth_1,
				   NULL);

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
      
      if(!note_256th_mode){
	ags_noise_util_set_frame_count(channel_data->noise_util,
				       floor(((offset_counter - x0) * delay + delay_counter + 1.0) * buffer_size));

	ags_noise_util_set_offset(channel_data->noise_util,
				  floor(((offset_counter - x0) * delay + delay_counter) * buffer_size));
      }else{
	g_rec_mutex_lock(fx_star_synth_audio_processor_mutex);

	note_256th_offset_lower = AGS_FX_NOTATION_AUDIO_PROCESSOR(fx_star_synth_audio_processor)->note_256th_offset_lower;

	note_256th_delay = AGS_FX_NOTATION_AUDIO_PROCESSOR(fx_star_synth_audio_processor)->note_256th_delay;

	g_rec_mutex_unlock(fx_star_synth_audio_processor_mutex);

	ags_noise_util_set_frame_count(channel_data->noise_util,
				       (guint) floor((double) (note_256th_offset_lower - x0_256th) * note_256th_delay * (double) buffer_size) + (guint) floor(delay * (double) buffer_size));

	ags_noise_util_set_offset(channel_data->noise_util,
				  (guint) floor(((double) (offset_counter - x0) * delay + delay_counter) * (double) buffer_size));

	ags_noise_util_set_offset_256th(channel_data->noise_util,
					(guint) floor((double) (note_256th_offset_lower - x0_256th) * note_256th_delay * (double) buffer_size));
      }
      
      ags_noise_util_compute(channel_data->noise_util);

      /* reset source and destination */
      ags_noise_util_set_source(channel_data->noise_util,
				NULL);
      
      ags_noise_util_set_destination(channel_data->noise_util,
				     NULL);
    }
    
    /* pitch */
    if(ags_common_pitch_util_get_tuning(channel_data->pitch_util,
					channel_data->pitch_type) != 0.0){
      ags_common_pitch_util_set_destination(channel_data->pitch_util,
					    channel_data->pitch_type,
					    source->stream_current->data);

      ags_common_pitch_util_set_source(channel_data->pitch_util,
				       channel_data->pitch_type,
				       source->stream_current->data);

      ags_common_pitch_util_set_samplerate(channel_data->pitch_util,
					   channel_data->pitch_type,
					   samplerate);
      ags_common_pitch_util_set_buffer_length(channel_data->pitch_util,
					      channel_data->pitch_type,
					      buffer_size);
      ags_common_pitch_util_set_format(channel_data->pitch_util,
				       channel_data->pitch_type,
				       format);
      
      g_rec_mutex_lock(source_stream_mutex);

      ags_common_pitch_util_pitch(channel_data->pitch_util,
				  channel_data->pitch_type);

      g_rec_mutex_unlock(source_stream_mutex);

      /* reset */
      ags_common_pitch_util_set_destination(channel_data->pitch_util,
					    channel_data->pitch_type,
					    NULL);

      ags_common_pitch_util_set_source(channel_data->pitch_util,
				       channel_data->pitch_type,
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

      
      /* compute chorus */
      g_rec_mutex_lock(source_stream_mutex);
      
      ags_chorus_util_compute(channel_data->chorus_util);

      g_rec_mutex_unlock(source_stream_mutex);

      ags_audio_buffer_util_clear_buffer(&(fx_star_synth_audio_signal->audio_buffer_util),
					 source->stream_current->data, 1,
					 buffer_size, audio_buffer_util_format);

      ags_audio_buffer_util_copy_buffer_to_buffer(&(fx_star_synth_audio_signal->audio_buffer_util),
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
  
  if(fx_star_synth_audio != NULL){
    g_object_unref(fx_star_synth_audio);
  }

  if(fx_star_synth_audio_processor != NULL){
    g_object_unref(fx_star_synth_audio_processor);
  }
  
  if(fx_star_synth_channel_processor != NULL){
    g_object_unref(fx_star_synth_channel_processor);
  }
  
  if(fx_star_synth_recycling != NULL){
    g_object_unref(fx_star_synth_recycling);
  }
}

void
ags_fx_star_synth_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					     AgsAudioSignal *source,
					     AgsNote *note,
					     guint x0, guint x1,
					     guint y)
{
  AgsAudio *audio;
  AgsFxStarSynthAudio *fx_star_synth_audio;
  AgsFxStarSynthAudioProcessor *fx_star_synth_audio_processor;
  AgsFxStarSynthChannelProcessor *fx_star_synth_channel_processor;
  AgsFxStarSynthRecycling *fx_star_synth_recycling;

  guint sound_scope;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;

  GRecMutex *fx_star_synth_audio_mutex;

  audio = NULL;
  
  fx_star_synth_audio = NULL;
  fx_star_synth_audio_processor = NULL;

  fx_star_synth_channel_processor = NULL;

  fx_star_synth_recycling = NULL;

  sound_scope = ags_recall_get_sound_scope((AgsRecall *) fx_notation_audio_signal);

  audio_channel = 0;

  audio_start_mapping = 0;
  midi_start_mapping = 0;

  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_star_synth_recycling,
	       NULL);

  g_object_get(fx_star_synth_recycling,
	       "parent", &fx_star_synth_channel_processor,
	       NULL);
  
  g_object_get(fx_star_synth_channel_processor,
	       "recall-audio", &fx_star_synth_audio,
	       "recall-audio-run", &fx_star_synth_audio_processor,
	       NULL);

  g_object_get(fx_star_synth_audio,
	       "audio", &audio,
	       NULL);

  g_object_get(fx_star_synth_audio_processor,
	       "audio-channel", &audio_channel,
	       NULL);

  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  fx_star_synth_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_star_synth_audio);

  midi_note = (y - audio_start_mapping + midi_start_mapping);

  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxStarSynthAudioScopeData *scope_data;
    AgsFxStarSynthAudioChannelData *channel_data;
    AgsFxStarSynthAudioInputData *input_data;

    g_rec_mutex_lock(fx_star_synth_audio_mutex);
      
    scope_data = fx_star_synth_audio->scope_data[sound_scope];

    channel_data = scope_data->channel_data[audio_channel];

    input_data = channel_data->input_data[midi_note];

    input_data->key_on -= 1;
      
    g_rec_mutex_unlock(fx_star_synth_audio_mutex);
  }
  
  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(fx_star_synth_audio != NULL){
    g_object_unref(fx_star_synth_audio);
  }

  if(fx_star_synth_audio_processor != NULL){
    g_object_unref(fx_star_synth_audio_processor);
  }
  
  if(fx_star_synth_channel_processor != NULL){
    g_object_unref(fx_star_synth_channel_processor);
  }
  
  if(fx_star_synth_recycling != NULL){
    g_object_unref(fx_star_synth_recycling);
  }
}

/**
 * ags_fx_star_synth_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxStarSynthAudioSignal
 *
 * Returns: the new #AgsFxStarSynthAudioSignal
 *
 * Since: 7.4.0
 */
AgsFxStarSynthAudioSignal*
ags_fx_star_synth_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxStarSynthAudioSignal *fx_star_synth_audio_signal;

  fx_star_synth_audio_signal = (AgsFxStarSynthAudioSignal *) g_object_new(AGS_TYPE_FX_STAR_SYNTH_AUDIO_SIGNAL,
									  "source", audio_signal,
									  NULL);

  return(fx_star_synth_audio_signal);
}
