/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_abyss_synth_audio_signal.h>

#include <ags/audio/ags_port.h>
#include <ags/audio/ags_abyss_synth_util.h>

#include <ags/audio/fx/ags_fx_abyss_synth_audio.h>
#include <ags/audio/fx/ags_fx_abyss_synth_audio_processor.h>
#include <ags/audio/fx/ags_fx_abyss_synth_channel.h>
#include <ags/audio/fx/ags_fx_abyss_synth_channel_processor.h>
#include <ags/audio/fx/ags_fx_abyss_synth_recycling.h>

#include <ags/i18n.h>

static void ags_fx_abyss_synth_audio_signal_class_init(AgsFxAbyssSynthAudioSignalClass *fx_abyss_synth_audio_signal);
static void ags_fx_abyss_synth_audio_signal_init(AgsFxAbyssSynthAudioSignal *fx_abyss_synth_audio_signal);
static void ags_fx_abyss_synth_audio_signal_dispose(GObject *gobject);
static void ags_fx_abyss_synth_audio_signal_finalize(GObject *gobject);

static void ags_fx_abyss_synth_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
							AgsAudioSignal *source,
							AgsNote *note,
							gboolean pattern_mode,
							guint x0, guint x1,
							guint y,
							gdouble delay_counter, guint64 offset_counter,
							guint frame_count,
							gdouble delay, guint buffer_size);
static void ags_fx_abyss_synth_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
							  AgsAudioSignal *source,
							  AgsNote *note,
							  guint x0, guint x1,
							  guint y);

/**
 * SECTION:ags_fx_abyss_synth_audio_signal
 * @short_description: fx abyss synth audio signal
 * @title: AgsFxAbyssSynthAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_abyss_synth_audio_signal.h
 *
 * The #AgsFxAbyssSynthAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_abyss_synth_audio_signal_parent_class = NULL;

static const gchar *ags_fx_abyss_synth_audio_signal_plugin_name = "ags-fx-abyss-synth";

GType
ags_fx_abyss_synth_audio_signal_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_abyss_synth_audio_signal = 0;

    static const GTypeInfo ags_fx_abyss_synth_audio_signal_info = {
      sizeof (AgsFxAbyssSynthAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_abyss_synth_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxAbyssSynthAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_abyss_synth_audio_signal_init,
    };

    ags_type_fx_abyss_synth_audio_signal = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL,
								  "AgsFxAbyssSynthAudioSignal",
								  &ags_fx_abyss_synth_audio_signal_info,
								  0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_abyss_synth_audio_signal);
  }

  return(g_define_type_id__static);
}

void
ags_fx_abyss_synth_audio_signal_class_init(AgsFxAbyssSynthAudioSignalClass *fx_abyss_synth_audio_signal)
{
  GObjectClass *gobject;
  AgsFxNotationAudioSignalClass *fx_notation_audio_signal;

  ags_fx_abyss_synth_audio_signal_parent_class = g_type_class_peek_parent(fx_abyss_synth_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_abyss_synth_audio_signal;

  gobject->dispose = ags_fx_abyss_synth_audio_signal_dispose;
  gobject->finalize = ags_fx_abyss_synth_audio_signal_finalize;

  /* AgsFxNotationAudioSignalClass */
  fx_notation_audio_signal = (AgsFxNotationAudioSignalClass *) fx_abyss_synth_audio_signal;
  
  fx_notation_audio_signal->stream_feed = ags_fx_abyss_synth_audio_signal_stream_feed;
  fx_notation_audio_signal->notify_remove = ags_fx_abyss_synth_audio_signal_notify_remove;
}

void
ags_fx_abyss_synth_audio_signal_init(AgsFxAbyssSynthAudioSignal *fx_abyss_synth_audio_signal)
{
  AGS_RECALL(fx_abyss_synth_audio_signal)->name = "ags-fx-abyss-synth";
  AGS_RECALL(fx_abyss_synth_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_abyss_synth_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_abyss_synth_audio_signal)->xml_type = "ags-fx-abyss-synth-audio-signal";
}

void
ags_fx_abyss_synth_audio_signal_dispose(GObject *gobject)
{
  AgsFxAbyssSynthAudioSignal *fx_abyss_synth_audio_signal;
  
  fx_abyss_synth_audio_signal = AGS_FX_ABYSS_SYNTH_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_abyss_synth_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_abyss_synth_audio_signal_finalize(GObject *gobject)
{
  AgsFxAbyssSynthAudioSignal *fx_abyss_synth_audio_signal;
  
  fx_abyss_synth_audio_signal = AGS_FX_ABYSS_SYNTH_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_abyss_synth_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_abyss_synth_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
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
  AgsFxAbyssSynthAudio *fx_abyss_synth_audio;
  AgsFxAbyssSynthAudioProcessor *fx_abyss_synth_audio_processor;
  AgsFxAbyssSynthChannelProcessor *fx_abyss_synth_channel_processor;
  AgsFxAbyssSynthRecycling *fx_abyss_synth_recycling;
  AgsFxAbyssSynthAudioSignal *fx_abyss_synth_audio_signal;
  
  guint64 note_256th_offset[16] = {0,};
  
  guint sound_scope;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;
  guint note_256th_offset_length;
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
  GRecMutex *fx_abyss_synth_audio_mutex;
  GRecMutex *fx_abyss_synth_audio_processor_mutex;

  fx_abyss_synth_audio_signal = (AgsFxAbyssSynthAudioSignal *) fx_notation_audio_signal;
  
  audio = NULL;
  
  fx_abyss_synth_audio = NULL;
  fx_abyss_synth_audio_processor = NULL;
  
  fx_abyss_synth_channel_processor = NULL;

  fx_abyss_synth_recycling = NULL;

  sound_scope = ags_recall_get_sound_scope((AgsRecall *) fx_notation_audio_signal);

  audio_channel = 0;
  
  audio_start_mapping = 0;
  midi_start_mapping = 0;

  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  octave = 0.0;
  key = 0.0;
  
  note_256th_mode = ags_fx_notation_audio_get_note_256th_mode((AgsFxNotationAudio *) fx_abyss_synth_audio);
  
  chorus_enabled = FALSE;
  
  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_abyss_synth_recycling,
	       NULL);

  g_object_get(fx_abyss_synth_recycling,
	       "parent", &fx_abyss_synth_channel_processor,
	       NULL);
  
  g_object_get(fx_abyss_synth_channel_processor,
	       "recall-audio", &fx_abyss_synth_audio,
	       "recall-audio-run", &fx_abyss_synth_audio_processor,
	       NULL);

  g_object_get(fx_abyss_synth_audio,
	       "audio", &audio,
	       NULL);

  g_object_get(fx_abyss_synth_audio_processor,
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

  audio_buffer_util_format = ags_audio_buffer_util_format_from_soundcard(&(fx_abyss_synth_audio_signal->audio_buffer_util),
									 format);

  copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(&(fx_abyss_synth_audio_signal->audio_buffer_util),
							      audio_buffer_util_format,
							      audio_buffer_util_format);
  
  /* get synth mutex */
  fx_abyss_synth_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_abyss_synth_audio);
  fx_abyss_synth_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_abyss_synth_audio_processor);

  g_rec_mutex_lock(fx_abyss_synth_audio_processor_mutex);

  note_256th_offset_length = 0;
    
  ags_frame_clock_get_note_256th_offset(AGS_FX_NOTATION_AUDIO_PROCESSOR(fx_abyss_synth_audio_processor)->frame_clock,
					&(note_256th_offset[0]),
					&note_256th_offset_length);
  
  note_256th_offset_lower = note_256th_offset[0];

  note_256th_delay = (gdouble) AGS_FX_NOTATION_AUDIO_PROCESSOR(fx_abyss_synth_audio_processor)->frame_clock->absolute_delay / 16.0;
  
  g_rec_mutex_unlock(fx_abyss_synth_audio_processor_mutex);

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

  if(fx_abyss_synth_audio != NULL){
    AgsPort *port;

    AgsFxAbyssSynthAudioChannelData *channel_data;

    GValue value = {0,};

    channel_data = fx_abyss_synth_audio->scope_data[sound_scope]->channel_data[audio_channel];
    
    /* synth-0 osc-0 oscillator */
    port = fx_abyss_synth_audio->synth_0_osc_0_oscillator;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_osc_0_oscillator(channel_data->abyss_synth_util_0,
						(guint) g_value_get_float(&value));
    }

    g_value_unset(&value);
    
    /* synth-0 osc-0 octave */
    port = fx_abyss_synth_audio->synth_0_osc_0_octave;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      octave = (gdouble) g_value_get_float(&value);
    }

    g_value_unset(&value);
    
    /* synth-0 osc-0 key */    
    port = fx_abyss_synth_audio->synth_0_osc_0_key;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      key = (gdouble) g_value_get_float(&value);
    }

    g_value_unset(&value);

    ags_abyss_synth_util_set_osc_0_frequency(channel_data->abyss_synth_util_0,
					     exp2(((octave * 12.0) + key + ((gdouble) midi_note - 48.0)) / 12.0) * 440.0);
      
    /* synth-0 osc-0 phase */    
    port = fx_abyss_synth_audio->synth_0_osc_0_phase;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_osc_0_phase(channel_data->abyss_synth_util_0,
					   ((gdouble) samplerate / ags_abyss_synth_util_get_osc_0_frequency(channel_data->abyss_synth_util_0)) * ((gdouble) g_value_get_float(&value) / (2.0 * M_PI)));
    }

    g_value_unset(&value);
      
    /* synth-0 osc-0 volume */    
    port = fx_abyss_synth_audio->synth_0_osc_0_volume;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_osc_0_volume(channel_data->abyss_synth_util_0,
					    (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 osc-0 low-pass sends */    
    port = fx_abyss_synth_audio->synth_0_osc_0_low_pass_sends;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_osc_0_low_pass_sends(channel_data->abyss_synth_util_0,
						    (gint64) g_value_get_float(&value));
    }

    g_value_unset(&value);
    
    /* synth-0 osc-1 oscillator */
    port = fx_abyss_synth_audio->synth_0_osc_1_oscillator;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_osc_1_oscillator(channel_data->abyss_synth_util_0,
						(guint) g_value_get_float(&value));
    }

    g_value_unset(&value);
    
    /* synth-0 osc-1 octave */
    port = fx_abyss_synth_audio->synth_0_osc_1_octave;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      octave = (gdouble) g_value_get_float(&value);
    }

    g_value_unset(&value);
    
    /* synth-0 osc-1 key */    
    port = fx_abyss_synth_audio->synth_0_osc_1_key;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      key = (gdouble) g_value_get_float(&value);
    }

    g_value_unset(&value);

    ags_abyss_synth_util_set_osc_1_frequency(channel_data->abyss_synth_util_0,
					     exp2(((octave * 12.0) + key + ((gdouble) midi_note - 48.0)) / 12.0) * 440.0);
      
    /* synth-0 osc-1 phase */    
    port = fx_abyss_synth_audio->synth_0_osc_1_phase;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_osc_1_phase(channel_data->abyss_synth_util_0,
					   ((gdouble) samplerate / ags_abyss_synth_util_get_osc_1_frequency(channel_data->abyss_synth_util_0)) * ((gdouble) g_value_get_float(&value) / (2.0 * M_PI)));
    }

    g_value_unset(&value);
      
    /* synth-0 osc-1 volume */    
    port = fx_abyss_synth_audio->synth_0_osc_1_volume;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_osc_1_volume(channel_data->abyss_synth_util_0,
					    (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 osc-1 low-pass sends */    
    port = fx_abyss_synth_audio->synth_0_osc_1_low_pass_sends;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_osc_1_low_pass_sends(channel_data->abyss_synth_util_0,
						    (gint64) g_value_get_float(&value));
    }

    g_value_unset(&value);
    
    /* synth-0 osc-2 oscillator */
    port = fx_abyss_synth_audio->synth_0_osc_2_oscillator;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_osc_2_oscillator(channel_data->abyss_synth_util_0,
						(guint) g_value_get_float(&value));
    }

    g_value_unset(&value);
    
    /* synth-0 osc-2 octave */
    port = fx_abyss_synth_audio->synth_0_osc_2_octave;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      octave = (gdouble) g_value_get_float(&value);
    }

    g_value_unset(&value);
    
    /* synth-0 osc-2 key */    
    port = fx_abyss_synth_audio->synth_0_osc_2_key;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      key = (gdouble) g_value_get_float(&value);
    }

    g_value_unset(&value);

    ags_abyss_synth_util_set_osc_2_frequency(channel_data->abyss_synth_util_0,
					     exp2(((octave * 12.0) + key + ((gdouble) midi_note - 48.0)) / 12.0) * 440.0);
      
    /* synth-0 osc-2 phase */    
    port = fx_abyss_synth_audio->synth_0_osc_2_phase;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_osc_2_phase(channel_data->abyss_synth_util_0,
					   ((gdouble) samplerate / ags_abyss_synth_util_get_osc_2_frequency(channel_data->abyss_synth_util_0)) * ((gdouble) g_value_get_float(&value) / (2.0 * M_PI)));
    }

    g_value_unset(&value);
      
    /* synth-0 osc-2 volume */    
    port = fx_abyss_synth_audio->synth_0_osc_2_volume;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_osc_2_volume(channel_data->abyss_synth_util_0,
					    (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 osc-2 low-pass sends */    
    port = fx_abyss_synth_audio->synth_0_osc_2_low_pass_sends;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_osc_2_low_pass_sends(channel_data->abyss_synth_util_0,
						    (gint64) g_value_get_float(&value));
    }

    g_value_unset(&value);
    
    /* synth-0 osc-3 oscillator */
    port = fx_abyss_synth_audio->synth_0_osc_3_oscillator;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_osc_3_oscillator(channel_data->abyss_synth_util_0,
						(guint) g_value_get_float(&value));
    }

    g_value_unset(&value);
    
    /* synth-0 osc-3 octave */
    port = fx_abyss_synth_audio->synth_0_osc_3_octave;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      octave = (gdouble) g_value_get_float(&value);
    }

    g_value_unset(&value);
    
    /* synth-0 osc-3 key */    
    port = fx_abyss_synth_audio->synth_0_osc_3_key;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      key = (gdouble) g_value_get_float(&value);
    }

    g_value_unset(&value);

    ags_abyss_synth_util_set_osc_3_frequency(channel_data->abyss_synth_util_0,
					     exp2(((octave * 12.0) + key + ((gdouble) midi_note - 48.0)) / 12.0) * 440.0);
      
    /* synth-0 osc-3 phase */    
    port = fx_abyss_synth_audio->synth_0_osc_3_phase;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_osc_3_phase(channel_data->abyss_synth_util_0,
					   ((gdouble) samplerate / ags_abyss_synth_util_get_osc_3_frequency(channel_data->abyss_synth_util_0)) * ((gdouble) g_value_get_float(&value) / (2.0 * M_PI)));
    }

    g_value_unset(&value);
      
    /* synth-0 osc-3 volume */    
    port = fx_abyss_synth_audio->synth_0_osc_3_volume;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_osc_3_volume(channel_data->abyss_synth_util_0,
					    (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 osc-3 low-pass sends */    
    port = fx_abyss_synth_audio->synth_0_osc_3_low_pass_sends;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_osc_3_low_pass_sends(channel_data->abyss_synth_util_0,
						    (gint64) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 ring-0 enabled */
    port = fx_abyss_synth_audio->synth_0_ring_0_enabled;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_ring_0_enabled(channel_data->abyss_synth_util_0,
					      (gboolean) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 ring-0 pitch tuning */
    port = fx_abyss_synth_audio->synth_0_ring_0_tuning;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_ring_0_pitch_tuning(channel_data->abyss_synth_util_0,
						   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 ring-0 drive */
    port = fx_abyss_synth_audio->synth_0_ring_0_drive;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_ring_0_drive(channel_data->abyss_synth_util_0,
					    (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 ring-0 mix */
    port = fx_abyss_synth_audio->synth_0_ring_0_mix;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_ring_0_mix(channel_data->abyss_synth_util_0,
					  (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 ring-0 gain */
    port = fx_abyss_synth_audio->synth_0_ring_0_gain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_ring_0_gain(channel_data->abyss_synth_util_0,
					   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 ring-1 enabled */
    port = fx_abyss_synth_audio->synth_0_ring_1_enabled;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_ring_1_enabled(channel_data->abyss_synth_util_0,
					      (gboolean) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 ring-1 pitch tuning */
    port = fx_abyss_synth_audio->synth_0_ring_1_tuning;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_ring_1_pitch_tuning(channel_data->abyss_synth_util_0,
						   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 ring-1 drive */
    port = fx_abyss_synth_audio->synth_0_ring_1_drive;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_ring_1_drive(channel_data->abyss_synth_util_0,
					    (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 ring-1 mix */
    port = fx_abyss_synth_audio->synth_0_ring_1_mix;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_ring_1_mix(channel_data->abyss_synth_util_0,
					  (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 ring-1 gain */
    port = fx_abyss_synth_audio->synth_0_ring_1_gain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_ring_1_gain(channel_data->abyss_synth_util_0,
					   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 pitch tuning */
    port = fx_abyss_synth_audio->synth_0_pitch_tuning;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_pitch_tuning(channel_data->abyss_synth_util_0,
					    (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 volume */
    port = fx_abyss_synth_audio->synth_0_volume;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_volume(channel_data->abyss_synth_util_0,
				      (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-0 attack */
    port = fx_abyss_synth_audio->synth_0_env_0_attack;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_0_attack(channel_data->abyss_synth_util_0,
					    (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-0 sustain */
    port = fx_abyss_synth_audio->synth_0_env_0_sustain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_0_sustain(channel_data->abyss_synth_util_0,
					     (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-0 decay */
    port = fx_abyss_synth_audio->synth_0_env_0_decay;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_0_decay(channel_data->abyss_synth_util_0,
					   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-0 release */
    port = fx_abyss_synth_audio->synth_0_env_0_release;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_0_release(channel_data->abyss_synth_util_0,
					     (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-0 gain */
    port = fx_abyss_synth_audio->synth_0_env_0_gain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_0_gain(channel_data->abyss_synth_util_0,
					  (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-0 frequency */
    port = fx_abyss_synth_audio->synth_0_env_0_frequency;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_0_frequency(channel_data->abyss_synth_util_0,
					       (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-0 sends */    
    port = fx_abyss_synth_audio->synth_0_env_0_sends;

    g_value_init(&value, G_TYPE_POINTER);

    if(port != NULL){      
      g_value_set_pointer(&value,
			  ags_abyss_synth_util_get_env_0_sends(channel_data->abyss_synth_util_0,
							       NULL));
    
      ags_port_safe_read(port,
			 &value);
    }

    g_value_unset(&value);
    
    /* synth-0 env-1 attack */
    port = fx_abyss_synth_audio->synth_0_env_1_attack;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_1_attack(channel_data->abyss_synth_util_0,
					    (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-1 sustain */
    port = fx_abyss_synth_audio->synth_0_env_1_sustain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_1_sustain(channel_data->abyss_synth_util_0,
					     (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-1 decay */
    port = fx_abyss_synth_audio->synth_0_env_1_decay;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_1_decay(channel_data->abyss_synth_util_0,
					   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-1 release */
    port = fx_abyss_synth_audio->synth_0_env_1_release;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_1_release(channel_data->abyss_synth_util_0,
					     (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-1 gain */
    port = fx_abyss_synth_audio->synth_0_env_1_gain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_1_gain(channel_data->abyss_synth_util_0,
					  (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-1 frequency */
    port = fx_abyss_synth_audio->synth_0_env_1_frequency;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_1_frequency(channel_data->abyss_synth_util_0,
					       (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-1 sends */    
    port = fx_abyss_synth_audio->synth_0_env_1_sends;

    g_value_init(&value, G_TYPE_POINTER);

    if(port != NULL){      
      g_value_set_pointer(&value,
			  ags_abyss_synth_util_get_env_1_sends(channel_data->abyss_synth_util_0,
							       NULL));
    
      ags_port_safe_read(port,
			 &value);
    }

    g_value_unset(&value);

    /* synth-0 env-2 attack */
    port = fx_abyss_synth_audio->synth_0_env_2_attack;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_2_attack(channel_data->abyss_synth_util_0,
					    (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-2 sustain */
    port = fx_abyss_synth_audio->synth_0_env_2_sustain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_2_sustain(channel_data->abyss_synth_util_0,
					     (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-2 decay */
    port = fx_abyss_synth_audio->synth_0_env_2_decay;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_2_decay(channel_data->abyss_synth_util_0,
					   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-2 release */
    port = fx_abyss_synth_audio->synth_0_env_2_release;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_2_release(channel_data->abyss_synth_util_0,
					     (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-2 gain */
    port = fx_abyss_synth_audio->synth_0_env_2_gain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_2_gain(channel_data->abyss_synth_util_0,
					  (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-2 frequency */
    port = fx_abyss_synth_audio->synth_0_env_2_frequency;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_2_frequency(channel_data->abyss_synth_util_0,
					       (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-2 sends */    
    port = fx_abyss_synth_audio->synth_0_env_2_sends;

    g_value_init(&value, G_TYPE_POINTER);

    if(port != NULL){      
      g_value_set_pointer(&value,
			  ags_abyss_synth_util_get_env_2_sends(channel_data->abyss_synth_util_0,
							       NULL));
    
      ags_port_safe_read(port,
			 &value);
    }

    g_value_unset(&value);

    /* synth-0 env-3 attack */
    port = fx_abyss_synth_audio->synth_0_env_3_attack;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_3_attack(channel_data->abyss_synth_util_0,
					    (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-3 sustain */
    port = fx_abyss_synth_audio->synth_0_env_3_sustain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_3_sustain(channel_data->abyss_synth_util_0,
					     (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-3 decay */
    port = fx_abyss_synth_audio->synth_0_env_3_decay;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_3_decay(channel_data->abyss_synth_util_0,
					   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-3 release */
    port = fx_abyss_synth_audio->synth_0_env_3_release;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_3_release(channel_data->abyss_synth_util_0,
					     (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-3 gain */
    port = fx_abyss_synth_audio->synth_0_env_3_gain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_3_gain(channel_data->abyss_synth_util_0,
					  (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-3 frequency */
    port = fx_abyss_synth_audio->synth_0_env_3_frequency;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_env_3_frequency(channel_data->abyss_synth_util_0,
					       (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 env-3 sends */    
    port = fx_abyss_synth_audio->synth_0_env_3_sends;

    g_value_init(&value, G_TYPE_POINTER);

    if(port != NULL){      
      g_value_set_pointer(&value,
			  ags_abyss_synth_util_get_env_3_sends(channel_data->abyss_synth_util_0,
							       NULL));
    
      ags_port_safe_read(port,
			 &value);
    }

    g_value_unset(&value);
    
    /* synth-0 lfo-0 frequency */
    port = fx_abyss_synth_audio->synth_0_lfo_0_frequency;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_lfo_0_frequency(channel_data->abyss_synth_util_0,
					       (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 lfo-0 depth */
    port = fx_abyss_synth_audio->synth_0_lfo_0_depth;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_lfo_0_depth(channel_data->abyss_synth_util_0,
					   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 lfo-0 tuning */
    port = fx_abyss_synth_audio->synth_0_lfo_0_tuning;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_lfo_0_tuning(channel_data->abyss_synth_util_0,
					    (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 lfo-0 sends */    
    port = fx_abyss_synth_audio->synth_0_lfo_0_sends;

    g_value_init(&value, G_TYPE_POINTER);

    if(port != NULL){      
      g_value_set_pointer(&value,
			  ags_abyss_synth_util_get_lfo_0_sends(channel_data->abyss_synth_util_0,
							       NULL));
    
      ags_port_safe_read(port,
			 &value);
    }

    g_value_unset(&value);
    
    /* synth-0 lfo-1 frequency */
    port = fx_abyss_synth_audio->synth_0_lfo_1_frequency;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_lfo_1_frequency(channel_data->abyss_synth_util_0,
					       (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 lfo-1 depth */
    port = fx_abyss_synth_audio->synth_0_lfo_1_depth;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_lfo_1_depth(channel_data->abyss_synth_util_0,
					   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 lfo-1 tuning */
    port = fx_abyss_synth_audio->synth_0_lfo_1_tuning;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_lfo_1_tuning(channel_data->abyss_synth_util_0,
					    (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 lfo-1 sends */    
    port = fx_abyss_synth_audio->synth_0_lfo_1_sends;

    g_value_init(&value, G_TYPE_POINTER);

    if(port != NULL){      
      g_value_set_pointer(&value,
			  ags_abyss_synth_util_get_lfo_1_sends(channel_data->abyss_synth_util_0,
							       NULL));
    
      ags_port_safe_read(port,
			 &value);
    }

    g_value_unset(&value);
    
    /* synth-0 lfo-2 frequency */
    port = fx_abyss_synth_audio->synth_0_lfo_2_frequency;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_lfo_2_frequency(channel_data->abyss_synth_util_0,
					       (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 lfo-2 depth */
    port = fx_abyss_synth_audio->synth_0_lfo_2_depth;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_lfo_2_depth(channel_data->abyss_synth_util_0,
					   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 lfo-2 tuning */
    port = fx_abyss_synth_audio->synth_0_lfo_2_tuning;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_lfo_2_tuning(channel_data->abyss_synth_util_0,
					    (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);    

    /* synth-0 lfo-2 sends */    
    port = fx_abyss_synth_audio->synth_0_lfo_2_sends;

    g_value_init(&value, G_TYPE_POINTER);

    if(port != NULL){      
      g_value_set_pointer(&value,
			  ags_abyss_synth_util_get_lfo_2_sends(channel_data->abyss_synth_util_0,
							       NULL));
    
      ags_port_safe_read(port,
			 &value);
    }

    g_value_unset(&value);

    /* synth-0 lfo-3 frequency */
    port = fx_abyss_synth_audio->synth_0_lfo_3_frequency;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_lfo_3_frequency(channel_data->abyss_synth_util_0,
					       (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 lfo-3 depth */
    port = fx_abyss_synth_audio->synth_0_lfo_3_depth;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_lfo_3_depth(channel_data->abyss_synth_util_0,
					   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 lfo-3 tuning */
    port = fx_abyss_synth_audio->synth_0_lfo_3_tuning;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_lfo_3_tuning(channel_data->abyss_synth_util_0,
					    (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 lfo-3 sends */    
    port = fx_abyss_synth_audio->synth_0_lfo_3_sends;

    g_value_init(&value, G_TYPE_POINTER);

    if(port != NULL){      
      g_value_set_pointer(&value,
			  ags_abyss_synth_util_get_lfo_3_sends(channel_data->abyss_synth_util_0,
							       NULL));
    
      ags_port_safe_read(port,
			 &value);
    }

    g_value_unset(&value);

    /* synth-0 seq-0 modulation 0 */
    port = fx_abyss_synth_audio->synth_0_seq_0_modulation_0;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_0_modulation(channel_data->abyss_synth_util_0,
						0,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-0 modulation 1 */
    port = fx_abyss_synth_audio->synth_0_seq_0_modulation_1;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_0_modulation(channel_data->abyss_synth_util_0,
						1,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-0 modulation 2 */
    port = fx_abyss_synth_audio->synth_0_seq_0_modulation_2;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_0_modulation(channel_data->abyss_synth_util_0,
						2,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-0 modulation 3 */
    port = fx_abyss_synth_audio->synth_0_seq_0_modulation_3;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_0_modulation(channel_data->abyss_synth_util_0,
						3,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-0 modulation 4 */
    port = fx_abyss_synth_audio->synth_0_seq_0_modulation_4;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_0_modulation(channel_data->abyss_synth_util_0,
						4,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-0 modulation 5 */
    port = fx_abyss_synth_audio->synth_0_seq_0_modulation_5;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_0_modulation(channel_data->abyss_synth_util_0,
						5,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-0 modulation 6 */
    port = fx_abyss_synth_audio->synth_0_seq_0_modulation_6;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_0_modulation(channel_data->abyss_synth_util_0,
						6,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-0 modulation 7 */
    port = fx_abyss_synth_audio->synth_0_seq_0_modulation_7;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_0_modulation(channel_data->abyss_synth_util_0,
						7,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-0 modulation 8 */
    port = fx_abyss_synth_audio->synth_0_seq_0_modulation_8;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_0_modulation(channel_data->abyss_synth_util_0,
						8,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-0 modulation 9 */
    port = fx_abyss_synth_audio->synth_0_seq_0_modulation_9;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_0_modulation(channel_data->abyss_synth_util_0,
						9,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-0 modulation 10 */
    port = fx_abyss_synth_audio->synth_0_seq_0_modulation_10;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_0_modulation(channel_data->abyss_synth_util_0,
						10,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-0 modulation 11 */
    port = fx_abyss_synth_audio->synth_0_seq_0_modulation_11;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_0_modulation(channel_data->abyss_synth_util_0,
						11,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-0 modulation 12 */
    port = fx_abyss_synth_audio->synth_0_seq_0_modulation_12;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_0_modulation(channel_data->abyss_synth_util_0,
						12,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-0 modulation 13 */
    port = fx_abyss_synth_audio->synth_0_seq_0_modulation_13;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_0_modulation(channel_data->abyss_synth_util_0,
						13,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-0 modulation 14 */
    port = fx_abyss_synth_audio->synth_0_seq_0_modulation_14;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_0_modulation(channel_data->abyss_synth_util_0,
						14,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-0 modulation 15 */
    port = fx_abyss_synth_audio->synth_0_seq_0_modulation_15;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_0_modulation(channel_data->abyss_synth_util_0,
						15,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-0 pingpong */
    port = fx_abyss_synth_audio->synth_0_seq_0_pingpong;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_0_pingpong(channel_data->abyss_synth_util_0,
					      (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-0 LFO frequency */
    port = fx_abyss_synth_audio->synth_0_seq_0_lfo_frequency;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_0_lfo_frequency(channel_data->abyss_synth_util_0,
						   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-0 sends */    
    port = fx_abyss_synth_audio->synth_0_seq_0_sends;

    g_value_init(&value, G_TYPE_POINTER);

    if(port != NULL){      
      g_value_set_pointer(&value,
			  ags_abyss_synth_util_get_seq_0_sends(channel_data->abyss_synth_util_0,
							       NULL));
    
      ags_port_safe_read(port,
			 &value);
    }

    g_value_unset(&value);

    /* synth-0 seq-1 modulation 0 */
    port = fx_abyss_synth_audio->synth_0_seq_1_modulation_0;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_1_modulation(channel_data->abyss_synth_util_0,
						0,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-1 modulation 1 */
    port = fx_abyss_synth_audio->synth_0_seq_1_modulation_1;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_1_modulation(channel_data->abyss_synth_util_0,
						1,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-1 modulation 2 */
    port = fx_abyss_synth_audio->synth_0_seq_1_modulation_2;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_1_modulation(channel_data->abyss_synth_util_0,
						2,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-1 modulation 3 */
    port = fx_abyss_synth_audio->synth_0_seq_1_modulation_3;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_1_modulation(channel_data->abyss_synth_util_0,
						3,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-1 modulation 4 */
    port = fx_abyss_synth_audio->synth_0_seq_1_modulation_4;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_1_modulation(channel_data->abyss_synth_util_0,
						4,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-1 modulation 5 */
    port = fx_abyss_synth_audio->synth_0_seq_1_modulation_5;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_1_modulation(channel_data->abyss_synth_util_0,
						5,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-1 modulation 6 */
    port = fx_abyss_synth_audio->synth_0_seq_1_modulation_6;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_1_modulation(channel_data->abyss_synth_util_0,
						6,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-1 modulation 7 */
    port = fx_abyss_synth_audio->synth_0_seq_1_modulation_7;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_1_modulation(channel_data->abyss_synth_util_0,
						7,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-1 modulation 8 */
    port = fx_abyss_synth_audio->synth_0_seq_1_modulation_8;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_1_modulation(channel_data->abyss_synth_util_0,
						8,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-1 modulation 9 */
    port = fx_abyss_synth_audio->synth_0_seq_1_modulation_9;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_1_modulation(channel_data->abyss_synth_util_0,
						9,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-1 modulation 10 */
    port = fx_abyss_synth_audio->synth_0_seq_1_modulation_10;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_1_modulation(channel_data->abyss_synth_util_0,
						10,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-1 modulation 11 */
    port = fx_abyss_synth_audio->synth_0_seq_1_modulation_11;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_1_modulation(channel_data->abyss_synth_util_0,
						11,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-1 modulation 12 */
    port = fx_abyss_synth_audio->synth_0_seq_1_modulation_12;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_1_modulation(channel_data->abyss_synth_util_0,
						12,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-1 modulation 13 */
    port = fx_abyss_synth_audio->synth_0_seq_1_modulation_13;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_1_modulation(channel_data->abyss_synth_util_0,
						13,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-1 modulation 14 */
    port = fx_abyss_synth_audio->synth_0_seq_1_modulation_14;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_1_modulation(channel_data->abyss_synth_util_0,
						14,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-1 modulation 15 */
    port = fx_abyss_synth_audio->synth_0_seq_1_modulation_15;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_1_modulation(channel_data->abyss_synth_util_0,
						15,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-1 pingpong */
    port = fx_abyss_synth_audio->synth_0_seq_1_pingpong;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_1_pingpong(channel_data->abyss_synth_util_0,
					      (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-1 LFO frequency */
    port = fx_abyss_synth_audio->synth_0_seq_1_lfo_frequency;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_1_lfo_frequency(channel_data->abyss_synth_util_0,
						   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-1 sends */    
    port = fx_abyss_synth_audio->synth_0_seq_1_sends;

    g_value_init(&value, G_TYPE_POINTER);

    if(port != NULL){      
      g_value_set_pointer(&value,
			  ags_abyss_synth_util_get_seq_1_sends(channel_data->abyss_synth_util_0,
							       NULL));
    
      ags_port_safe_read(port,
			 &value);
    }

    g_value_unset(&value);

    /* synth-0 seq-2 modulation 0 */
    port = fx_abyss_synth_audio->synth_0_seq_2_modulation_0;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_2_modulation(channel_data->abyss_synth_util_0,
						0,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-2 modulation 1 */
    port = fx_abyss_synth_audio->synth_0_seq_2_modulation_1;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_2_modulation(channel_data->abyss_synth_util_0,
						1,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-2 modulation 2 */
    port = fx_abyss_synth_audio->synth_0_seq_2_modulation_2;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_2_modulation(channel_data->abyss_synth_util_0,
						2,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-2 modulation 3 */
    port = fx_abyss_synth_audio->synth_0_seq_2_modulation_3;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_2_modulation(channel_data->abyss_synth_util_0,
						3,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-2 modulation 4 */
    port = fx_abyss_synth_audio->synth_0_seq_2_modulation_4;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_2_modulation(channel_data->abyss_synth_util_0,
						4,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-2 modulation 5 */
    port = fx_abyss_synth_audio->synth_0_seq_2_modulation_5;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_2_modulation(channel_data->abyss_synth_util_0,
						5,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-2 modulation 6 */
    port = fx_abyss_synth_audio->synth_0_seq_2_modulation_6;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_2_modulation(channel_data->abyss_synth_util_0,
						6,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-2 modulation 7 */
    port = fx_abyss_synth_audio->synth_0_seq_2_modulation_7;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_2_modulation(channel_data->abyss_synth_util_0,
						7,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-2 modulation 8 */
    port = fx_abyss_synth_audio->synth_0_seq_2_modulation_8;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_2_modulation(channel_data->abyss_synth_util_0,
						8,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-2 modulation 9 */
    port = fx_abyss_synth_audio->synth_0_seq_2_modulation_9;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_2_modulation(channel_data->abyss_synth_util_0,
						9,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-2 modulation 10 */
    port = fx_abyss_synth_audio->synth_0_seq_2_modulation_10;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_2_modulation(channel_data->abyss_synth_util_0,
						10,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-2 modulation 11 */
    port = fx_abyss_synth_audio->synth_0_seq_2_modulation_11;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_2_modulation(channel_data->abyss_synth_util_0,
						11,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-2 modulation 12 */
    port = fx_abyss_synth_audio->synth_0_seq_2_modulation_12;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_2_modulation(channel_data->abyss_synth_util_0,
						12,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-2 modulation 13 */
    port = fx_abyss_synth_audio->synth_0_seq_2_modulation_13;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_2_modulation(channel_data->abyss_synth_util_0,
						13,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-2 modulation 14 */
    port = fx_abyss_synth_audio->synth_0_seq_2_modulation_14;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_2_modulation(channel_data->abyss_synth_util_0,
						14,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-2 modulation 15 */
    port = fx_abyss_synth_audio->synth_0_seq_2_modulation_15;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_2_modulation(channel_data->abyss_synth_util_0,
						15,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-2 pingpong */
    port = fx_abyss_synth_audio->synth_0_seq_2_pingpong;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_2_pingpong(channel_data->abyss_synth_util_0,
					      (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-2 LFO frequency */
    port = fx_abyss_synth_audio->synth_0_seq_2_lfo_frequency;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_2_lfo_frequency(channel_data->abyss_synth_util_0,
						   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-2 sends */    
    port = fx_abyss_synth_audio->synth_0_seq_2_sends;

    g_value_init(&value, G_TYPE_POINTER);

    if(port != NULL){      
      g_value_set_pointer(&value,
			  ags_abyss_synth_util_get_seq_2_sends(channel_data->abyss_synth_util_0,
							       NULL));
    
      ags_port_safe_read(port,
			 &value);
    }

    g_value_unset(&value);

    /* synth-0 seq-3 modulation 0 */
    port = fx_abyss_synth_audio->synth_0_seq_3_modulation_0;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_3_modulation(channel_data->abyss_synth_util_0,
						0,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-3 modulation 1 */
    port = fx_abyss_synth_audio->synth_0_seq_3_modulation_1;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_3_modulation(channel_data->abyss_synth_util_0,
						1,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-3 modulation 2 */
    port = fx_abyss_synth_audio->synth_0_seq_3_modulation_2;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_3_modulation(channel_data->abyss_synth_util_0,
						2,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-3 modulation 3 */
    port = fx_abyss_synth_audio->synth_0_seq_3_modulation_3;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_3_modulation(channel_data->abyss_synth_util_0,
						3,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-3 modulation 4 */
    port = fx_abyss_synth_audio->synth_0_seq_3_modulation_4;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_3_modulation(channel_data->abyss_synth_util_0,
						4,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-3 modulation 5 */
    port = fx_abyss_synth_audio->synth_0_seq_3_modulation_5;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_3_modulation(channel_data->abyss_synth_util_0,
						5,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-3 modulation 6 */
    port = fx_abyss_synth_audio->synth_0_seq_3_modulation_6;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_3_modulation(channel_data->abyss_synth_util_0,
						6,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-3 modulation 7 */
    port = fx_abyss_synth_audio->synth_0_seq_3_modulation_7;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_3_modulation(channel_data->abyss_synth_util_0,
						7,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-3 modulation 8 */
    port = fx_abyss_synth_audio->synth_0_seq_3_modulation_8;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_3_modulation(channel_data->abyss_synth_util_0,
						8,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-3 modulation 9 */
    port = fx_abyss_synth_audio->synth_0_seq_3_modulation_9;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_3_modulation(channel_data->abyss_synth_util_0,
						9,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-3 modulation 10 */
    port = fx_abyss_synth_audio->synth_0_seq_3_modulation_10;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_3_modulation(channel_data->abyss_synth_util_0,
						10,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-3 modulation 11 */
    port = fx_abyss_synth_audio->synth_0_seq_3_modulation_11;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_3_modulation(channel_data->abyss_synth_util_0,
						11,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-3 modulation 12 */
    port = fx_abyss_synth_audio->synth_0_seq_3_modulation_12;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_3_modulation(channel_data->abyss_synth_util_0,
						12,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-3 modulation 13 */
    port = fx_abyss_synth_audio->synth_0_seq_3_modulation_13;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_3_modulation(channel_data->abyss_synth_util_0,
						13,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-3 modulation 14 */
    port = fx_abyss_synth_audio->synth_0_seq_3_modulation_14;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_3_modulation(channel_data->abyss_synth_util_0,
						14,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-3 modulation 15 */
    port = fx_abyss_synth_audio->synth_0_seq_3_modulation_15;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_3_modulation(channel_data->abyss_synth_util_0,
						15,
						(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-3 pingpong */
    port = fx_abyss_synth_audio->synth_0_seq_3_pingpong;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_3_pingpong(channel_data->abyss_synth_util_0,
					      (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-3 LFO frequency */
    port = fx_abyss_synth_audio->synth_0_seq_3_lfo_frequency;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_abyss_synth_util_set_seq_3_lfo_frequency(channel_data->abyss_synth_util_0,
						   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* synth-0 seq-3 sends */    
    port = fx_abyss_synth_audio->synth_0_seq_3_sends;

    g_value_init(&value, G_TYPE_POINTER);

    if(port != NULL){      
      g_value_set_pointer(&value,
			  ags_abyss_synth_util_get_seq_3_sends(channel_data->abyss_synth_util_0,
							       NULL));
    
      ags_port_safe_read(port,
			 &value);
    }

    g_value_unset(&value);
    
    /* synth-0 pink noise 0 frequency */
    port = fx_abyss_synth_audio->synth_0_pink_noise_0_frequency;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_noise_util_set_frequency(channel_data->abyss_synth_util_0->noise_0_util,
				   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);    
    
    /* synth-0 pink noise 0 gain */    
    port = fx_abyss_synth_audio->synth_0_pink_noise_0_gain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_noise_util_set_volume(channel_data->abyss_synth_util_0->noise_0_util,
				(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);    

    /* synth-0 pink noise 0 sends */    
    port = fx_abyss_synth_audio->synth_0_pink_noise_0_sends;

    g_value_init(&value, G_TYPE_POINTER);
    
    if(port != NULL){      
      g_value_set_pointer(&value,
			  ags_abyss_synth_util_get_noise_0_sends(channel_data->abyss_synth_util_0,
								 NULL));
    
      ags_port_safe_read(port,
			 &value);
    }
    
    g_value_unset(&value);
    
    /* synth-0 pink noise 1 frequency */
    port = fx_abyss_synth_audio->synth_0_pink_noise_1_frequency;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_noise_util_set_frequency(channel_data->abyss_synth_util_0->noise_1_util,
				   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);    
    
    /* synth-0 pink noise 1 gain */    
    port = fx_abyss_synth_audio->synth_0_pink_noise_1_gain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_noise_util_set_volume(channel_data->abyss_synth_util_0->noise_1_util,
				(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);    

    /* synth-0 pink noise 1 sends */    
    port = fx_abyss_synth_audio->synth_0_pink_noise_1_sends;

    g_value_init(&value, G_TYPE_POINTER);
    
    if(port != NULL){      
      g_value_set_pointer(&value,
			  ags_abyss_synth_util_get_noise_1_sends(channel_data->abyss_synth_util_0,
								 NULL));
    
      ags_port_safe_read(port,
			 &value);
    }
    
    g_value_unset(&value);

    /* low-pass 0 cut-off frequency */
    port = fx_abyss_synth_audio->low_pass_0_cut_off_frequency;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_low_pass_filter_util_set_cut_off_frequency(channel_data->abyss_synth_util_0->low_pass_filter_util_0,
						     (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);
    
    /* low-pass 0 filter gain */
    port = fx_abyss_synth_audio->low_pass_0_filter_gain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_low_pass_filter_util_set_filter_gain(channel_data->abyss_synth_util_0->low_pass_filter_util_0,
					       (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);
    
    /* low-pass 0 no-clip */    
    port = fx_abyss_synth_audio->low_pass_0_no_clip;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_low_pass_filter_util_set_no_clip(channel_data->abyss_synth_util_0->low_pass_filter_util_0,
					   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* low-pass 1 cut-off frequency */
    port = fx_abyss_synth_audio->low_pass_1_cut_off_frequency;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_low_pass_filter_util_set_cut_off_frequency(channel_data->abyss_synth_util_0->low_pass_filter_util_1,
						     (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);
    
    /* low-pass 1 filter gain */
    port = fx_abyss_synth_audio->low_pass_1_filter_gain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_low_pass_filter_util_set_filter_gain(channel_data->abyss_synth_util_0->low_pass_filter_util_1,
					       (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);
    
    /* low-pass 1 no-clip */    
    port = fx_abyss_synth_audio->low_pass_1_no_clip;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_low_pass_filter_util_set_no_clip(channel_data->abyss_synth_util_0->low_pass_filter_util_1,
					   (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* amplifier 0 amp 0 gain */
    port = fx_abyss_synth_audio->amplifier_0_amp_0_gain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_amplifier_util_set_amp_0_gain(channel_data->amplifier_util_0,
					(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* amplifier 0 amp 1 gain */    
    port = fx_abyss_synth_audio->amplifier_0_amp_1_gain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_amplifier_util_set_amp_1_gain(channel_data->amplifier_util_0,
					(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* amplifier 0 amp 2 gain */    
    port = fx_abyss_synth_audio->amplifier_0_amp_2_gain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_amplifier_util_set_amp_2_gain(channel_data->amplifier_util_0,
					(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* amplifier 0 amp 3 gain */    
    port = fx_abyss_synth_audio->amplifier_0_amp_3_gain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_amplifier_util_set_amp_3_gain(channel_data->amplifier_util_0,
					(gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* amplifier 0 filter gain */    
    port = fx_abyss_synth_audio->amplifier_0_filter_gain;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      ags_amplifier_util_set_filter_gain(channel_data->amplifier_util_0,
					 (gdouble) g_value_get_float(&value));
    }

    g_value_unset(&value);

    /* chorus enabled */
    port = fx_abyss_synth_audio->chorus_enabled;

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      chorus_enabled = (gboolean) g_value_get_float(&value);
    }

    g_value_unset(&value);

    if(chorus_enabled){
      /* chorus input-volume */
      port = fx_abyss_synth_audio->chorus_input_volume;

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	ags_chorus_util_set_input_volume(channel_data->chorus_util,
					 (gdouble) g_value_get_float(&value));
      }
      
      g_value_unset(&value);

      /* chorus output-volume */    
      port = fx_abyss_synth_audio->chorus_output_volume;

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	ags_chorus_util_set_output_volume(channel_data->chorus_util,
					  (gdouble) g_value_get_float(&value));
      }

      g_value_unset(&value);

      /* chorus lfo-oscillator */    
      port = fx_abyss_synth_audio->chorus_lfo_oscillator;

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	ags_chorus_util_set_lfo_oscillator(channel_data->chorus_util,
					   (guint) g_value_get_float(&value));
      }

      g_value_unset(&value);

      /* chorus lfo-frequency */    
      port = fx_abyss_synth_audio->chorus_lfo_frequency;

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	ags_chorus_util_set_lfo_frequency(channel_data->chorus_util,
					  (gdouble) g_value_get_float(&value));
      }

      g_value_unset(&value);

      /* chorus depth */    
      port = fx_abyss_synth_audio->chorus_depth;

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	ags_chorus_util_set_depth(channel_data->chorus_util,
				  (gdouble) g_value_get_float(&value));
      }

      g_value_unset(&value);

      /* chorus mix */    
      port = fx_abyss_synth_audio->chorus_mix;

      g_object_get(fx_abyss_synth_audio,
		   "chorus-mix", &port,
		   NULL);

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	ags_chorus_util_set_mix(channel_data->chorus_util,
				(gdouble) g_value_get_float(&value));
      }

      g_value_unset(&value);

      /* chorus delay */    
      port = fx_abyss_synth_audio->chorus_delay;

      g_value_init(&value, G_TYPE_FLOAT);
    
      if(port != NULL){      
	ags_port_safe_read(port,
			   &value);

	ags_chorus_util_set_delay(channel_data->chorus_util,
				  (gdouble) g_value_get_float(&value));
      }

      g_value_unset(&value);
    }
  }
  
  copy_mode_out = ags_audio_buffer_util_get_copy_mode_from_format(&(fx_abyss_synth_audio_signal->audio_buffer_util),
								  audio_buffer_util_format,
								  AGS_AUDIO_BUFFER_UTIL_FLOAT);

  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxAbyssSynthAudioScopeData *scope_data;
    AgsFxAbyssSynthAudioChannelData *channel_data;
    AgsFxAbyssSynthAudioInputData *input_data;

    g_rec_mutex_lock(fx_abyss_synth_audio_mutex);

    scope_data = fx_abyss_synth_audio->scope_data[sound_scope];

    channel_data = scope_data->channel_data[audio_channel];

    input_data = channel_data->input_data[midi_note];

    g_rec_mutex_unlock(fx_abyss_synth_audio_mutex);

    if((gint) floor(delay_counter) == 0 &&
       x0 == offset_counter){
      g_rec_mutex_lock(fx_abyss_synth_audio_mutex);

      input_data->key_on += 1;

      g_rec_mutex_unlock(fx_abyss_synth_audio_mutex);
    }

    ags_audio_buffer_util_clear_buffer(&(fx_abyss_synth_audio_signal->audio_buffer_util),
				       channel_data->synth_buffer_0, 1,
				       buffer_size, audio_buffer_util_format);

    ags_audio_buffer_util_clear_buffer(&(fx_abyss_synth_audio_signal->audio_buffer_util),
				       channel_data->abyss_synth_util_0->osc_0_buffer, 1,
				       buffer_size, audio_buffer_util_format);

    ags_audio_buffer_util_clear_buffer(&(fx_abyss_synth_audio_signal->audio_buffer_util),
				       channel_data->abyss_synth_util_0->osc_1_buffer, 1,
				       buffer_size, audio_buffer_util_format);

    ags_audio_buffer_util_clear_buffer(&(fx_abyss_synth_audio_signal->audio_buffer_util),
				       channel_data->abyss_synth_util_0->osc_2_buffer, 1,
				       buffer_size, audio_buffer_util_format);

    ags_audio_buffer_util_clear_buffer(&(fx_abyss_synth_audio_signal->audio_buffer_util),
				       channel_data->abyss_synth_util_0->osc_3_buffer, 1,
				       buffer_size, audio_buffer_util_format);
    
    /* abyss synth 0 */
    ags_abyss_synth_util_set_source(channel_data->abyss_synth_util_0,
				    channel_data->synth_buffer_0);
    ags_abyss_synth_util_set_source_stride(channel_data->abyss_synth_util_0,
					   1);

    ags_abyss_synth_util_set_buffer_length(channel_data->abyss_synth_util_0,
					   buffer_size);
    ags_abyss_synth_util_set_format(channel_data->abyss_synth_util_0,
				    format);
    ags_abyss_synth_util_set_samplerate(channel_data->abyss_synth_util_0,
					samplerate);

    //TODO:JK: implement me
    
    if(!note_256th_mode){
      ags_abyss_synth_util_set_frame_count(channel_data->abyss_synth_util_0,
					   floor(((offset_counter - x0) * delay + delay_counter + 1.0) * buffer_size));

      ags_abyss_synth_util_set_offset(channel_data->abyss_synth_util_0,
				      floor(((offset_counter - x0) * delay + delay_counter) * buffer_size));
    }else{
      ags_abyss_synth_util_set_frame_count(channel_data->abyss_synth_util_0,
					   (guint) floor((double) ((x1_256th - x0_256th) / 16.0) * delay * (double) buffer_size));

      ags_abyss_synth_util_set_offset(channel_data->abyss_synth_util_0,
				      (guint) floor(((double) (offset_counter - x0) * delay + delay_counter) * (double) buffer_size));

      ags_abyss_synth_util_set_offset_256th(channel_data->abyss_synth_util_0,
					    (guint) floor((double) (note_256th_offset_lower - x0_256th) * note_256th_delay * (double) buffer_size));
    }
    
    g_rec_mutex_lock(source_stream_mutex);

    ags_abyss_synth_util_compute(channel_data->abyss_synth_util_0);
        
    ags_abyss_synth_util_set_source(channel_data->abyss_synth_util_0,
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
    
    /* copy synth buffer */
    ags_audio_buffer_util_clear_buffer(&(fx_abyss_synth_audio_signal->audio_buffer_util),
				       source->stream_current->data, 1,
				       buffer_size, audio_buffer_util_format);

    ags_audio_buffer_util_copy_buffer_to_buffer(&(fx_abyss_synth_audio_signal->audio_buffer_util),
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

      ags_audio_buffer_util_clear_buffer(&(fx_abyss_synth_audio_signal->audio_buffer_util),
					 source->stream_current->data, 1,
					 buffer_size, audio_buffer_util_format);

      ags_audio_buffer_util_copy_buffer_to_buffer(&(fx_abyss_synth_audio_signal->audio_buffer_util),
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
  
  if(fx_abyss_synth_audio != NULL){
    g_object_unref(fx_abyss_synth_audio);
  }

  if(fx_abyss_synth_audio_processor != NULL){
    g_object_unref(fx_abyss_synth_audio_processor);
  }
  
  if(fx_abyss_synth_channel_processor != NULL){
    g_object_unref(fx_abyss_synth_channel_processor);
  }
  
  if(fx_abyss_synth_recycling != NULL){
    g_object_unref(fx_abyss_synth_recycling);
  }
}

void
ags_fx_abyss_synth_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					      AgsAudioSignal *source,
					      AgsNote *note,
					      guint x0, guint x1,
					      guint y)
{
  AgsAudio *audio;
  AgsFxAbyssSynthAudio *fx_abyss_synth_audio;
  AgsFxAbyssSynthAudioProcessor *fx_abyss_synth_audio_processor;
  AgsFxAbyssSynthChannelProcessor *fx_abyss_synth_channel_processor;
  AgsFxAbyssSynthRecycling *fx_abyss_synth_recycling;

  guint sound_scope;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;

  GRecMutex *fx_abyss_synth_audio_mutex;

  audio = NULL;
  
  fx_abyss_synth_audio = NULL;
  fx_abyss_synth_audio_processor = NULL;

  fx_abyss_synth_channel_processor = NULL;

  fx_abyss_synth_recycling = NULL;

  sound_scope = ags_recall_get_sound_scope((AgsRecall *) fx_notation_audio_signal);

  audio_channel = 0;

  audio_start_mapping = 0;
  midi_start_mapping = 0;

  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_abyss_synth_recycling,
	       NULL);

  g_object_get(fx_abyss_synth_recycling,
	       "parent", &fx_abyss_synth_channel_processor,
	       NULL);
  
  g_object_get(fx_abyss_synth_channel_processor,
	       "recall-audio", &fx_abyss_synth_audio,
	       "recall-audio-run", &fx_abyss_synth_audio_processor,
	       NULL);

  g_object_get(fx_abyss_synth_audio,
	       "audio", &audio,
	       NULL);

  g_object_get(fx_abyss_synth_audio_processor,
	       "audio-channel", &audio_channel,
	       NULL);

  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  fx_abyss_synth_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_abyss_synth_audio);

  midi_note = (y - audio_start_mapping + midi_start_mapping);

  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxAbyssSynthAudioScopeData *scope_data;
    AgsFxAbyssSynthAudioChannelData *channel_data;
    AgsFxAbyssSynthAudioInputData *input_data;

    g_rec_mutex_lock(fx_abyss_synth_audio_mutex);
      
    scope_data = fx_abyss_synth_audio->scope_data[sound_scope];

    channel_data = scope_data->channel_data[audio_channel];

    input_data = channel_data->input_data[midi_note];

    input_data->key_on -= 1;
      
    g_rec_mutex_unlock(fx_abyss_synth_audio_mutex);
  }
  
  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(fx_abyss_synth_audio != NULL){
    g_object_unref(fx_abyss_synth_audio);
  }

  if(fx_abyss_synth_audio_processor != NULL){
    g_object_unref(fx_abyss_synth_audio_processor);
  }
  
  if(fx_abyss_synth_channel_processor != NULL){
    g_object_unref(fx_abyss_synth_channel_processor);
  }
  
  if(fx_abyss_synth_recycling != NULL){
    g_object_unref(fx_abyss_synth_recycling);
  }
}

/**
 * ags_fx_abyss_synth_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxAbyssSynthAudioSignal
 *
 * Returns: the new #AgsFxAbyssSynthAudioSignal
 *
 * Since: 9.4.0
 */
AgsFxAbyssSynthAudioSignal*
ags_fx_abyss_synth_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxAbyssSynthAudioSignal *fx_abyss_synth_audio_signal;

  fx_abyss_synth_audio_signal = (AgsFxAbyssSynthAudioSignal *) g_object_new(AGS_TYPE_FX_ABYSS_SYNTH_AUDIO_SIGNAL,
									    "source", audio_signal,
									    NULL);

  return(fx_abyss_synth_audio_signal);
}
