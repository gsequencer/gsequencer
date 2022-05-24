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

#include <ags/audio/fx/ags_fx_two_pass_aliase_audio_signal.h>

#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_phase_shift_util.h>
#include <ags/audio/ags_frequency_aliase_util.h>

#include <ags/audio/fx/ags_fx_two_pass_aliase_channel.h>
#include <ags/audio/fx/ags_fx_two_pass_aliase_channel_processor.h>
#include <ags/audio/fx/ags_fx_two_pass_aliase_recycling.h>

#include <ags/i18n.h>

void ags_fx_two_pass_aliase_audio_signal_class_init(AgsFxTwoPassAliaseAudioSignalClass *fx_two_pass_aliase_audio_signal);
void ags_fx_two_pass_aliase_audio_signal_init(AgsFxTwoPassAliaseAudioSignal *fx_two_pass_aliase_audio_signal);
void ags_fx_two_pass_aliase_audio_signal_dispose(GObject *gobject);
void ags_fx_two_pass_aliase_audio_signal_finalize(GObject *gobject);

void ags_fx_two_pass_aliase_audio_signal_real_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_two_pass_aliase_audio_signal
 * @short_description: fx two pass aliase audio signal
 * @title: AgsFxTwoPassAliaseAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_two_pass_aliase_audio_signal.h
 *
 * The #AgsFxTwoPassAliaseAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_two_pass_aliase_audio_signal_parent_class = NULL;

const gchar *ags_fx_two_pass_aliase_audio_signal_plugin_name = "ags-fx-two-pass-aliase";

GType
ags_fx_two_pass_aliase_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_two_pass_aliase_audio_signal = 0;

    static const GTypeInfo ags_fx_two_pass_aliase_audio_signal_info = {
      sizeof (AgsFxTwoPassAliaseAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_two_pass_aliase_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxTwoPassAliaseAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_two_pass_aliase_audio_signal_init,
    };

    ags_type_fx_two_pass_aliase_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
								      "AgsFxTwoPassAliaseAudioSignal",
								      &ags_fx_two_pass_aliase_audio_signal_info,
								      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_two_pass_aliase_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_two_pass_aliase_audio_signal_class_init(AgsFxTwoPassAliaseAudioSignalClass *fx_two_pass_aliase_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_two_pass_aliase_audio_signal_parent_class = g_type_class_peek_parent(fx_two_pass_aliase_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_two_pass_aliase_audio_signal;

  gobject->dispose = ags_fx_two_pass_aliase_audio_signal_dispose;
  gobject->finalize = ags_fx_two_pass_aliase_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_two_pass_aliase_audio_signal;
  
  recall->run_inter = ags_fx_two_pass_aliase_audio_signal_real_run_inter;
}

void
ags_fx_two_pass_aliase_audio_signal_init(AgsFxTwoPassAliaseAudioSignal *fx_two_pass_aliase_audio_signal)
{
  AGS_RECALL(fx_two_pass_aliase_audio_signal)->name = "ags-fx-two-pass-aliase";
  AGS_RECALL(fx_two_pass_aliase_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_two_pass_aliase_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_two_pass_aliase_audio_signal)->xml_type = "ags-fx-two-pass-aliase-audio-signal";
}

void
ags_fx_two_pass_aliase_audio_signal_dispose(GObject *gobject)
{
  AgsFxTwoPassAliaseAudioSignal *fx_two_pass_aliase_audio_signal;
  
  fx_two_pass_aliase_audio_signal = AGS_FX_TWO_PASS_ALIASE_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_two_pass_aliase_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_two_pass_aliase_audio_signal_finalize(GObject *gobject)
{
  AgsFxTwoPassAliaseAudioSignal *fx_two_pass_aliase_audio_signal;
  
  fx_two_pass_aliase_audio_signal = AGS_FX_TWO_PASS_ALIASE_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_two_pass_aliase_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_two_pass_aliase_audio_signal_real_run_inter(AgsRecall *recall)
{
  AgsFxTwoPassAliaseChannel *fx_two_pass_aliase_channel;
  AgsFxTwoPassAliaseChannelProcessor *fx_two_pass_aliase_channel_processor;
  AgsFxTwoPassAliaseRecycling *fx_two_pass_aliase_recycling;
  AgsAudio *audio;
  AgsChannel *channel;
  AgsAudioSignal *source;
  AgsPort *port;

  gint sound_scope;
  guint pad;
  guint midi_start_mapping;
  gint midi_note;
  guint buffer_size;
  guint samplerate;
  guint format;
  guint copy_mode_out, copy_mode_in;
  gboolean enabled;
  gdouble base_freq;
  gdouble a_amount;
  gdouble a_phase;
  gdouble b_amount;
  gdouble b_phase;

  GRecMutex *fx_two_pass_aliase_channel_mutex;
  GRecMutex *stream_mutex;

  sound_scope = ags_recall_get_sound_scope(recall);

  fx_two_pass_aliase_channel = NULL;
  fx_two_pass_aliase_channel_mutex = NULL;

  fx_two_pass_aliase_channel_processor = NULL;

  fx_two_pass_aliase_recycling = NULL;

  audio = NULL;

  channel = NULL;
  
  source = NULL;
  
  pad = 0;
  midi_start_mapping = 0;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  g_object_get(recall,
	       "parent", &fx_two_pass_aliase_recycling,
	       "source", &source,
	       NULL);

  g_object_get(fx_two_pass_aliase_recycling,
	       "parent", &fx_two_pass_aliase_channel_processor,
	       NULL);

  g_object_get(fx_two_pass_aliase_channel_processor,
	       "recall-channel", &fx_two_pass_aliase_channel,
	       NULL);

  g_object_get(fx_two_pass_aliase_channel,
	       "source", &channel,
	       NULL);

  g_object_get(channel,
	       "audio", &audio,
	       "pad", &pad,
	       NULL);

  g_object_get(source,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       "format", &format,
	       NULL);

  g_object_get(audio,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
    midi_note = (128 - pad - 1 + midi_start_mapping);
  }else{
    midi_note = (pad + midi_start_mapping);
  }

  copy_mode_out = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						      AGS_AUDIO_BUFFER_UTIL_DOUBLE);
  copy_mode_in = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						     ags_audio_buffer_util_format_from_soundcard(format));

  enabled = FALSE;
  
  base_freq = exp2(((double) midi_note - 69.0) / 12.0) * 440.0;
  
  a_amount = 0.0;
  a_phase = 0.0;

  b_amount = 0.0;
  b_phase = 0.0;

  port = NULL;
  g_object_get(fx_two_pass_aliase_channel,
	       "enabled", &port,
	       NULL);

  if(port != NULL){
    GValue value = G_VALUE_INIT;

    g_value_init(&value,
		 G_TYPE_FLOAT);
    
    ags_port_safe_read(port,
		       &value);

    enabled = (g_value_get_float(&value) != 0.0) ? TRUE: FALSE;

    g_object_unref(port);
  }

  port = NULL;
  g_object_get(fx_two_pass_aliase_channel,
	       "a-amount", &port,
	       NULL);

  if(port != NULL){
    GValue value = G_VALUE_INIT;

    g_value_init(&value,
		 G_TYPE_FLOAT);
    
    ags_port_safe_read(port,
		       &value);

    a_amount = g_value_get_float(&value);

    g_object_unref(port);
  }

  port = NULL;
  g_object_get(fx_two_pass_aliase_channel,
	       "a-phase", &port,
	       NULL);

  if(port != NULL){
    GValue value = G_VALUE_INIT;

    g_value_init(&value,
		 G_TYPE_FLOAT);
    
    ags_port_safe_read(port,
		       &value);

    a_phase = g_value_get_float(&value);

    g_object_unref(port);
  }

  port = NULL;
  g_object_get(fx_two_pass_aliase_channel,
	       "b-amount", &port,
	       NULL);

  if(port != NULL){
    GValue value = G_VALUE_INIT;

    g_value_init(&value,
		 G_TYPE_FLOAT);
    
    ags_port_safe_read(port,
		       &value);

    b_amount = g_value_get_float(&value);

    g_object_unref(port);
  }

  port = NULL;
  g_object_get(fx_two_pass_aliase_channel,
	       "b-phase", &port,
	       NULL);

  if(port != NULL){
    GValue value = G_VALUE_INIT;

    g_value_init(&value,
		 G_TYPE_FLOAT);
    
    ags_port_safe_read(port,
		       &value);

    b_phase = g_value_get_float(&value);

    g_object_unref(port);
  }
  
  if(enabled &&
     fx_two_pass_aliase_channel != NULL &&
     source != NULL &&
     source->stream_current != NULL &&
     sound_scope >= 0 &&
     sound_scope < AGS_SOUND_SCOPE_LAST){
    AgsPhaseShiftUtil phase_shift_util;
    AgsFrequencyAliaseUtil frequency_aliase_util;

    fx_two_pass_aliase_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_two_pass_aliase_channel);
    stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);

    g_rec_mutex_lock(fx_two_pass_aliase_channel_mutex);

    ags_audio_buffer_util_clear_buffer(fx_two_pass_aliase_channel->input_data[sound_scope]->orig_buffer, 1,
				       buffer_size, AGS_AUDIO_BUFFER_UTIL_DOUBLE);

    g_rec_mutex_lock(stream_mutex);

    ags_audio_buffer_util_copy_buffer_to_buffer(fx_two_pass_aliase_channel->input_data[sound_scope]->orig_buffer, 1, 0,
						source->stream_current->data, 1, 0,
						buffer_size, copy_mode_in);
    
    g_rec_mutex_unlock(stream_mutex);

    /* phase shift - a buffer */
    phase_shift_util.buffer_length = buffer_size;
    phase_shift_util.format = AGS_SOUNDCARD_DOUBLE;
    phase_shift_util.samplerate = samplerate;
    
    phase_shift_util.destination = fx_two_pass_aliase_channel->input_data[sound_scope]->a_buffer;
    phase_shift_util.destination_stride = 1;

    phase_shift_util.source = fx_two_pass_aliase_channel->input_data[sound_scope]->orig_buffer;
    phase_shift_util.source_stride = 1;

    phase_shift_util.frequency = base_freq;
    phase_shift_util.amount = a_amount;
    phase_shift_util.phase = a_phase;
    
    ags_phase_shift_util_process_double(&phase_shift_util);

    /* phase shift - b buffer */
    phase_shift_util.buffer_length = buffer_size;
    phase_shift_util.format = AGS_SOUNDCARD_DOUBLE;
    phase_shift_util.samplerate = samplerate;
    
    phase_shift_util.destination = fx_two_pass_aliase_channel->input_data[sound_scope]->b_buffer;
    phase_shift_util.destination_stride = 1;

    phase_shift_util.source = fx_two_pass_aliase_channel->input_data[sound_scope]->orig_buffer;
    phase_shift_util.source_stride = 1;

    phase_shift_util.frequency = base_freq;
    phase_shift_util.amount = b_amount;
    phase_shift_util.phase = b_phase;
    
    ags_phase_shift_util_process_double(&phase_shift_util);

    /* frequency aliase - a mix */
    frequency_aliase_util.buffer_length = buffer_size;
    frequency_aliase_util.format = AGS_SOUNDCARD_DOUBLE;
    frequency_aliase_util.samplerate = samplerate;
    
    frequency_aliase_util.destination = fx_two_pass_aliase_channel->input_data[sound_scope]->a_mix;
    frequency_aliase_util.destination_stride = 1;

    frequency_aliase_util.source = fx_two_pass_aliase_channel->input_data[sound_scope]->orig_buffer;
    frequency_aliase_util.source_stride = 1;

    frequency_aliase_util.phase_shifted_source = fx_two_pass_aliase_channel->input_data[sound_scope]->a_buffer;
    frequency_aliase_util.phase_shifted_source_stride = 1;

    ags_frequency_aliase_util_process_double(&frequency_aliase_util);

    /* frequency aliase - b mix */
    frequency_aliase_util.buffer_length = buffer_size;
    frequency_aliase_util.format = AGS_SOUNDCARD_DOUBLE;
    frequency_aliase_util.samplerate = samplerate;
    
    frequency_aliase_util.destination = fx_two_pass_aliase_channel->input_data[sound_scope]->b_mix;
    frequency_aliase_util.destination_stride = 1;

    frequency_aliase_util.source = fx_two_pass_aliase_channel->input_data[sound_scope]->orig_buffer;
    frequency_aliase_util.source_stride = 1;

    frequency_aliase_util.phase_shifted_source = fx_two_pass_aliase_channel->input_data[sound_scope]->b_buffer;
    frequency_aliase_util.phase_shifted_source_stride = 1;

    ags_frequency_aliase_util_process_double(&frequency_aliase_util);
    
    /* frequency aliase - final mix */
    frequency_aliase_util.buffer_length = buffer_size;
    frequency_aliase_util.format = AGS_SOUNDCARD_DOUBLE;
    frequency_aliase_util.samplerate = samplerate;
    
    frequency_aliase_util.destination = fx_two_pass_aliase_channel->input_data[sound_scope]->final_mix;
    frequency_aliase_util.destination_stride = 1;

    frequency_aliase_util.source = fx_two_pass_aliase_channel->input_data[sound_scope]->a_mix;
    frequency_aliase_util.source_stride = 1;

    frequency_aliase_util.phase_shifted_source = fx_two_pass_aliase_channel->input_data[sound_scope]->b_mix;
    frequency_aliase_util.phase_shifted_source_stride = 1;

    ags_frequency_aliase_util_process_double(&frequency_aliase_util);
    
    g_rec_mutex_lock(stream_mutex);

    ags_audio_buffer_util_clear_buffer(source->stream_current->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(format));

    ags_audio_buffer_util_copy_buffer_to_buffer(source->stream_current->data, 1, 0,
						fx_two_pass_aliase_channel->input_data[sound_scope]->final_mix, 1, 0,
						buffer_size, copy_mode_out);
    
    g_rec_mutex_unlock(stream_mutex);

    g_rec_mutex_unlock(fx_two_pass_aliase_channel_mutex);
  }
  
  /* done */
  if(source == NULL ||
     source->stream_current == NULL){
    ags_recall_done(recall);
  }

  /* unref */
  if(fx_two_pass_aliase_channel != NULL){
    g_object_unref(fx_two_pass_aliase_channel);
  }

  if(fx_two_pass_aliase_channel_processor != NULL){
    g_object_unref(fx_two_pass_aliase_channel_processor);
  }

  if(fx_two_pass_aliase_recycling != NULL){
    g_object_unref(fx_two_pass_aliase_recycling);
  }

  if(audio != NULL){
    g_object_unref(audio);
  }

  if(channel != NULL){
    g_object_unref(channel);
  }

  if(source != NULL){
    g_object_unref(source);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_two_pass_aliase_audio_signal_parent_class)->run_inter(recall);
}

/**
 * ags_fx_two_pass_aliase_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxTwoPassAliaseAudioSignal
 *
 * Returns: the new #AgsFxTwoPassAliaseAudioSignal
 *
 * Since: 3.8.0
 */
AgsFxTwoPassAliaseAudioSignal*
ags_fx_two_pass_aliase_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxTwoPassAliaseAudioSignal *fx_two_pass_aliase_audio_signal;

  fx_two_pass_aliase_audio_signal = (AgsFxTwoPassAliaseAudioSignal *) g_object_new(AGS_TYPE_FX_TWO_PASS_ALIASE_AUDIO_SIGNAL,
										   "source", audio_signal,
										   NULL);

  return(fx_two_pass_aliase_audio_signal);
}
