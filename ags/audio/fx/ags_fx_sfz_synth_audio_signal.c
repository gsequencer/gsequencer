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

#include <ags/audio/fx/ags_fx_sfz_synth_audio_signal.h>

#include <ags/audio/ags_port.h>

#include <ags/audio/fx/ags_fx_sfz_synth_audio.h>
#include <ags/audio/fx/ags_fx_sfz_synth_audio_processor.h>
#include <ags/audio/fx/ags_fx_sfz_synth_channel.h>
#include <ags/audio/fx/ags_fx_sfz_synth_channel_processor.h>
#include <ags/audio/fx/ags_fx_sfz_synth_recycling.h>

#include <ags/i18n.h>

void ags_fx_sfz_synth_audio_signal_class_init(AgsFxSFZSynthAudioSignalClass *fx_sfz_synth_audio_signal);
void ags_fx_sfz_synth_audio_signal_init(AgsFxSFZSynthAudioSignal *fx_sfz_synth_audio_signal);
void ags_fx_sfz_synth_audio_signal_dispose(GObject *gobject);
void ags_fx_sfz_synth_audio_signal_finalize(GObject *gobject);

void ags_fx_sfz_synth_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					       AgsAudioSignal *source,
					       AgsNote *note,
					       gboolean pattern_mode,
					       guint x0, guint x1,
					       guint y,
					       gdouble delay_counter, guint64 offset_counter,
					       guint frame_count,
					       gdouble delay, guint buffer_size);
void ags_fx_sfz_synth_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
						 AgsAudioSignal *source,
						 AgsNote *note,
						 guint x0, guint x1,
						 guint y);

/**
 * SECTION:ags_fx_sfz_synth_audio_signal
 * @short_description: fx SFZ synth audio signal
 * @title: AgsFxSFZSynthAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_sfz_synth_audio_signal.h
 *
 * The #AgsFxSFZSynthAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_sfz_synth_audio_signal_parent_class = NULL;

const gchar *ags_fx_sfz_synth_audio_signal_plugin_name = "ags-fx-sfz-synth";

GType
ags_fx_sfz_synth_audio_signal_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_sfz_synth_audio_signal = 0;

    static const GTypeInfo ags_fx_sfz_synth_audio_signal_info = {
      sizeof (AgsFxSFZSynthAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_sfz_synth_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxSFZSynthAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_sfz_synth_audio_signal_init,
    };

    ags_type_fx_sfz_synth_audio_signal = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL,
								"AgsFxSFZSynthAudioSignal",
								&ags_fx_sfz_synth_audio_signal_info,
								0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_sfz_synth_audio_signal);
  }

  return g_define_type_id__static;
}

void
ags_fx_sfz_synth_audio_signal_class_init(AgsFxSFZSynthAudioSignalClass *fx_sfz_synth_audio_signal)
{
  GObjectClass *gobject;
  AgsFxNotationAudioSignalClass *fx_notation_audio_signal;

  ags_fx_sfz_synth_audio_signal_parent_class = g_type_class_peek_parent(fx_sfz_synth_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_sfz_synth_audio_signal;

  gobject->dispose = ags_fx_sfz_synth_audio_signal_dispose;
  gobject->finalize = ags_fx_sfz_synth_audio_signal_finalize;

  /* AgsFxNotationAudioSignalClass */
  fx_notation_audio_signal = (AgsFxNotationAudioSignalClass *) fx_sfz_synth_audio_signal;
  
  fx_notation_audio_signal->stream_feed = ags_fx_sfz_synth_audio_signal_stream_feed;
  fx_notation_audio_signal->notify_remove = ags_fx_sfz_synth_audio_signal_notify_remove;
}

void
ags_fx_sfz_synth_audio_signal_init(AgsFxSFZSynthAudioSignal *fx_sfz_synth_audio_signal)
{
  AGS_RECALL(fx_sfz_synth_audio_signal)->name = "ags-fx-sfz-synth";
  AGS_RECALL(fx_sfz_synth_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_sfz_synth_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_sfz_synth_audio_signal)->xml_type = "ags-fx-sfz-synth-audio-signal";
}

void
ags_fx_sfz_synth_audio_signal_dispose(GObject *gobject)
{
  AgsFxSFZSynthAudioSignal *fx_sfz_synth_audio_signal;
  
  fx_sfz_synth_audio_signal = AGS_FX_SFZ_SYNTH_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_sfz_synth_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_sfz_synth_audio_signal_finalize(GObject *gobject)
{
  AgsFxSFZSynthAudioSignal *fx_sfz_synth_audio_signal;
  
  fx_sfz_synth_audio_signal = AGS_FX_SFZ_SYNTH_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_sfz_synth_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_sfz_synth_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
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
  AgsFxSFZSynthAudio *fx_sfz_synth_audio;
  AgsFxSFZSynthAudioProcessor *fx_sfz_synth_audio_processor;
  AgsFxSFZSynthChannelProcessor *fx_sfz_synth_channel_processor;
  AgsFxSFZSynthRecycling *fx_sfz_synth_recycling;
  AgsFxSFZSynthAudioSignal *fx_sfz_synth_audio_signal;
  
  guint sound_scope;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;
  guint x0_256th, x1_256th;
  guint64 note_256th_offset_lower;
  gdouble note_256th_delay;
  gdouble octave;
  gdouble key;
  gboolean vibrato_enabled;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint format;
  guint samplerate;
  guint audio_buffer_util_format;
  guint copy_mode;
  
  gboolean note_256th_mode;

  gboolean chorus_enabled;

  GRecMutex *source_stream_mutex;
  GRecMutex *fx_sfz_synth_audio_mutex;
  GRecMutex *fx_sfz_synth_audio_processor_mutex;

  fx_sfz_synth_audio_signal = (AgsFxSFZSynthAudioSignal *) fx_notation_audio_signal;
  
  audio = NULL;
  
  fx_sfz_synth_audio = NULL;
  fx_sfz_synth_audio_processor = NULL;
  
  fx_sfz_synth_channel_processor = NULL;

  fx_sfz_synth_recycling = NULL;

  sound_scope = ags_recall_get_sound_scope((AgsRecall *) fx_notation_audio_signal);

  audio_channel = 0;
  
  audio_start_mapping = 0;
  midi_start_mapping = 0;

  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  note_256th_mode = ags_fx_notation_audio_get_note_256th_mode((AgsFxNotationAudio *) fx_sfz_synth_audio);
  
  chorus_enabled = FALSE;
  
  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_sfz_synth_recycling,
	       NULL);

  g_object_get(fx_sfz_synth_recycling,
	       "parent", &fx_sfz_synth_channel_processor,
	       NULL);
  
  g_object_get(fx_sfz_synth_channel_processor,
	       "recall-audio", &fx_sfz_synth_audio,
	       "recall-audio-run", &fx_sfz_synth_audio_processor,
	       NULL);

  g_object_get(fx_sfz_synth_audio,
	       "audio", &audio,
	       NULL);

  g_object_get(fx_sfz_synth_audio_processor,
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

  audio_buffer_util_format = ags_audio_buffer_util_format_from_soundcard(&(fx_sfz_synth_audio_signal->audio_buffer_util),
									 format);

  copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(&(fx_sfz_synth_audio_signal->audio_buffer_util),
							      audio_buffer_util_format,
							      audio_buffer_util_format);
  
  /* get synth mutex */
  fx_sfz_synth_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_sfz_synth_audio);
  fx_sfz_synth_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_sfz_synth_audio_processor);

  g_rec_mutex_lock(fx_sfz_synth_audio_processor_mutex);

  note_256th_offset_lower = AGS_FX_NOTATION_AUDIO_PROCESSOR(fx_sfz_synth_audio_processor)->note_256th_offset_lower;

  note_256th_delay = AGS_FX_NOTATION_AUDIO_PROCESSOR(fx_sfz_synth_audio_processor)->note_256th_delay;
  
  g_rec_mutex_unlock(fx_sfz_synth_audio_processor_mutex);

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

  octave = 0.0;
  key = 0.0;

  vibrato_enabled = FALSE;
  vibrato_gain = 1.0;
  vibrato_lfo_depth = 1.0;
  vibrato_lfo_freq = 8.172;
  vibrato_tuning = 0.0;
  
  if(fx_sfz_synth_audio != NULL){
    AgsPort *port;

    AgsFxSFZSynthAudioChannelData *channel_data;
    
    GValue value = {0,};

    channel_data = fx_sfz_synth_audio->scope_data[sound_scope]->channel_data[audio_channel];
    
    /* synth octave */
    g_object_get(fx_sfz_synth_audio,
		 "synth-octave", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      octave = (gdouble) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);
    
    /* synth key */    
    g_object_get(fx_sfz_synth_audio,
		 "synth-key", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      key = (gdouble) g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    ags_sfz_synth_util_set_note(channel_data->synth,
				(octave * 12.0) + key);

    /* synth volume */    
    g_object_get(fx_sfz_synth_audio,
		 "synth-volume", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_sfz_synth_util_set_volume(channel_data->synth,
				    (gdouble) g_value_get_float(&value));
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* chorus enabled */    
    g_object_get(fx_sfz_synth_audio,
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
      g_object_get(fx_sfz_synth_audio,
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
      g_object_get(fx_sfz_synth_audio,
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
      g_object_get(fx_sfz_synth_audio,
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
      g_object_get(fx_sfz_synth_audio,
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
      g_object_get(fx_sfz_synth_audio,
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
      g_object_get(fx_sfz_synth_audio,
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
      g_object_get(fx_sfz_synth_audio,
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

    /* vibrato enabled */
    g_object_get(fx_sfz_synth_audio,
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
    g_object_get(fx_sfz_synth_audio,
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
    g_object_get(fx_sfz_synth_audio,
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
    g_object_get(fx_sfz_synth_audio,
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
    g_object_get(fx_sfz_synth_audio,
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
  }

  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxSFZSynthAudioScopeData *scope_data;
    AgsFxSFZSynthAudioChannelData *channel_data;
    AgsFxSFZSynthAudioInputData *input_data;

    g_rec_mutex_lock(fx_sfz_synth_audio_mutex);

    scope_data = fx_sfz_synth_audio->scope_data[sound_scope];

    channel_data = scope_data->channel_data[audio_channel];

    input_data = channel_data->input_data[midi_note];

    g_rec_mutex_unlock(fx_sfz_synth_audio_mutex);

    if((gint) floor(delay_counter) == 0 &&
       x0 == offset_counter){
      g_rec_mutex_lock(fx_sfz_synth_audio_mutex);

      input_data->key_on += 1;

      g_rec_mutex_unlock(fx_sfz_synth_audio_mutex);
    }
    
    ags_sfz_synth_util_set_source(channel_data->synth,
				  source->stream_current->data);
    ags_sfz_synth_util_set_source_stride(channel_data->synth,
					 1);

    ags_sfz_synth_util_set_buffer_length(channel_data->synth,
					 buffer_size);
    ags_sfz_synth_util_set_format(channel_data->synth,
				  format);
    ags_sfz_synth_util_set_samplerate(channel_data->synth,
				      samplerate);

    ags_sfz_synth_util_set_midi_key(channel_data->synth,
				    midi_note);
    ags_sfz_synth_util_set_note(channel_data->synth,
				(octave * 12.0) + key);

    if(!note_256th_mode){
      ags_sfz_synth_util_set_frame_count(channel_data->synth,
					 floor(((offset_counter - x0) * delay + delay_counter + 1.0) * buffer_size));

      ags_sfz_synth_util_set_offset(channel_data->synth,
				    floor(((offset_counter - x0) * delay + delay_counter) * buffer_size));
    }else{
      g_rec_mutex_lock(fx_sfz_synth_audio_processor_mutex);

      note_256th_offset_lower = AGS_FX_NOTATION_AUDIO_PROCESSOR(fx_sfz_synth_audio_processor)->note_256th_offset_lower;

      note_256th_delay = AGS_FX_NOTATION_AUDIO_PROCESSOR(fx_sfz_synth_audio_processor)->note_256th_delay;

      g_rec_mutex_unlock(fx_sfz_synth_audio_processor_mutex);

      ags_sfz_synth_util_set_frame_count(channel_data->synth,
					 (guint) floor((double) (note_256th_offset_lower - x0_256th) * note_256th_delay * (double) buffer_size) + (guint) floor(delay * (double) buffer_size));

      ags_sfz_synth_util_set_offset(channel_data->synth,
				    (guint) floor(((double) (offset_counter - x0) * delay + delay_counter) * (double) buffer_size));

      ags_sfz_synth_util_set_offset_256th(channel_data->synth,
					  (guint) floor((double) (note_256th_offset_lower - x0_256th) * note_256th_delay * (double) buffer_size));
    }

    ags_common_pitch_util_set_vibrato_enabled(channel_data->synth->pitch_util,
					      channel_data->synth->pitch_type,
					      vibrato_enabled);

    ags_common_pitch_util_set_vibrato_gain(channel_data->synth->pitch_util,
					   channel_data->synth->pitch_type,
					   vibrato_gain);
    
    ags_common_pitch_util_set_vibrato_lfo_depth(channel_data->synth->pitch_util,
						channel_data->synth->pitch_type,
						vibrato_lfo_depth);

    ags_common_pitch_util_set_vibrato_lfo_freq(channel_data->synth->pitch_util,
					       channel_data->synth->pitch_type,
					       vibrato_lfo_freq);

    ags_common_pitch_util_set_vibrato_tuning(channel_data->synth->pitch_util,
					     channel_data->synth->pitch_type,
					     vibrato_tuning);

    ags_common_pitch_util_set_offset(channel_data->synth->pitch_util,
				     channel_data->synth->pitch_type,
				     ((offset_counter - x0) * delay + delay_counter) * buffer_size);

    ags_audio_buffer_util_clear_buffer(&(fx_sfz_synth_audio_signal->audio_buffer_util),
				       source->stream_current->data, 1,
				       buffer_size, audio_buffer_util_format);

    g_rec_mutex_lock(source_stream_mutex);

    ags_sfz_synth_util_compute(channel_data->synth);

    g_rec_mutex_unlock(source_stream_mutex);

    ags_sfz_synth_util_set_source(channel_data->synth,
				  NULL);

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
      
      g_rec_mutex_lock(source_stream_mutex);

      ags_chorus_util_compute(channel_data->chorus_util);

      g_rec_mutex_unlock(source_stream_mutex);

      ags_audio_buffer_util_clear_buffer(&(fx_sfz_synth_audio_signal->audio_buffer_util),
					 source->stream_current->data, 1,
					 buffer_size, audio_buffer_util_format);

      ags_audio_buffer_util_copy_buffer_to_buffer(&(fx_sfz_synth_audio_signal->audio_buffer_util),
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
  
  if(fx_sfz_synth_audio != NULL){
    g_object_unref(fx_sfz_synth_audio);
  }

  if(fx_sfz_synth_audio_processor != NULL){
    g_object_unref(fx_sfz_synth_audio_processor);
  }
  
  if(fx_sfz_synth_channel_processor != NULL){
    g_object_unref(fx_sfz_synth_channel_processor);
  }
  
  if(fx_sfz_synth_recycling != NULL){
    g_object_unref(fx_sfz_synth_recycling);
  }
}

void
ags_fx_sfz_synth_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					    AgsAudioSignal *source,
					    AgsNote *note,
					    guint x0, guint x1,
					    guint y)
{
  AgsAudio *audio;
  AgsFxSFZSynthAudio *fx_sfz_synth_audio;
  AgsFxSFZSynthAudioProcessor *fx_sfz_synth_audio_processor;
  AgsFxSFZSynthChannelProcessor *fx_sfz_synth_channel_processor;
  AgsFxSFZSynthRecycling *fx_sfz_synth_recycling;

  guint sound_scope;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;

  GRecMutex *fx_sfz_synth_audio_mutex;

  audio = NULL;
  
  fx_sfz_synth_audio = NULL;
  fx_sfz_synth_audio_processor = NULL;

  fx_sfz_synth_channel_processor = NULL;

  fx_sfz_synth_recycling = NULL;

  sound_scope = ags_recall_get_sound_scope((AgsRecall *) fx_notation_audio_signal);

  audio_channel = 0;

  audio_start_mapping = 0;
  midi_start_mapping = 0;

  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_sfz_synth_recycling,
	       NULL);

  g_object_get(fx_sfz_synth_recycling,
	       "parent", &fx_sfz_synth_channel_processor,
	       NULL);
  
  g_object_get(fx_sfz_synth_channel_processor,
	       "recall-audio", &fx_sfz_synth_audio,
	       "recall-audio-run", &fx_sfz_synth_audio_processor,
	       NULL);

  g_object_get(fx_sfz_synth_audio,
	       "audio", &audio,
	       NULL);

  g_object_get(fx_sfz_synth_audio_processor,
	       "audio-channel", &audio_channel,
	       NULL);

  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  fx_sfz_synth_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_sfz_synth_audio);

  midi_note = (y - audio_start_mapping + midi_start_mapping);

  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxSFZSynthAudioScopeData *scope_data;
    AgsFxSFZSynthAudioChannelData *channel_data;
    AgsFxSFZSynthAudioInputData *input_data;

    g_rec_mutex_lock(fx_sfz_synth_audio_mutex);
      
    scope_data = fx_sfz_synth_audio->scope_data[sound_scope];

    channel_data = scope_data->channel_data[audio_channel];

    input_data = channel_data->input_data[midi_note];

    input_data->key_on -= 1;
      
    g_rec_mutex_unlock(fx_sfz_synth_audio_mutex);
  }
  
  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(fx_sfz_synth_audio != NULL){
    g_object_unref(fx_sfz_synth_audio);
  }

  if(fx_sfz_synth_audio_processor != NULL){
    g_object_unref(fx_sfz_synth_audio_processor);
  }
  
  if(fx_sfz_synth_channel_processor != NULL){
    g_object_unref(fx_sfz_synth_channel_processor);
  }
  
  if(fx_sfz_synth_recycling != NULL){
    g_object_unref(fx_sfz_synth_recycling);
  }
}

/**
 * ags_fx_sfz_synth_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxSFZSynthAudioSignal
 *
 * Returns: the new #AgsFxSFZSynthAudioSignal
 *
 * Since: 3.17.0
 */
AgsFxSFZSynthAudioSignal*
ags_fx_sfz_synth_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxSFZSynthAudioSignal *fx_sfz_synth_audio_signal;

  fx_sfz_synth_audio_signal = (AgsFxSFZSynthAudioSignal *) g_object_new(AGS_TYPE_FX_SFZ_SYNTH_AUDIO_SIGNAL,
									"source", audio_signal,
									NULL);

  return(fx_sfz_synth_audio_signal);
}
