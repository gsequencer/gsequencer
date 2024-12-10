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

#include <ags/audio/fx/ags_fx_lv2_audio_signal.h>

#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_lv2_plugin.h>

#include <ags/audio/midi/ags_midi_smf_util.h>

#include <ags/audio/fx/ags_fx_lv2_audio.h>
#include <ags/audio/fx/ags_fx_lv2_audio_processor.h>
#include <ags/audio/fx/ags_fx_lv2_channel.h>
#include <ags/audio/fx/ags_fx_lv2_channel_processor.h>
#include <ags/audio/fx/ags_fx_lv2_recycling.h>

#include <ags/i18n.h>

void ags_fx_lv2_audio_signal_class_init(AgsFxLv2AudioSignalClass *fx_lv2_audio_signal);
void ags_fx_lv2_audio_signal_init(AgsFxLv2AudioSignal *fx_lv2_audio_signal);
void ags_fx_lv2_audio_signal_dispose(GObject *gobject);
void ags_fx_lv2_audio_signal_finalize(GObject *gobject);

void ags_fx_lv2_audio_signal_real_run_inter(AgsRecall *recall);

void ags_fx_lv2_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					 AgsAudioSignal *source,
					 AgsNote *note,
					 gboolean pattern_mode,
					 guint x0, guint x1,
					 guint y,
					 gdouble delay_counter, guint64 offset_counter,
					 guint frame_count,
					 gdouble delay, guint buffer_size);
void ags_fx_lv2_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					   AgsAudioSignal *source,
					   AgsNote *note,
					   guint x0, guint x1,
					   guint y);

/**
 * SECTION:ags_fx_lv2_audio_signal
 * @short_description: fx lv2 audio signal
 * @title: AgsFxLv2AudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_lv2_audio_signal.h
 *
 * The #AgsFxLv2AudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_lv2_audio_signal_recall_audio_signal_class = NULL;
static gpointer ags_fx_lv2_audio_signal_parent_class = NULL;

const gchar *ags_fx_lv2_audio_signal_plugin_name = "ags-fx-lv2";

GType
ags_fx_lv2_audio_signal_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_lv2_audio_signal = 0;

    static const GTypeInfo ags_fx_lv2_audio_signal_info = {
      sizeof (AgsFxLv2AudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_lv2_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxLv2AudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_lv2_audio_signal_init,
    };

    ags_type_fx_lv2_audio_signal = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL,
							  "AgsFxLv2AudioSignal",
							  &ags_fx_lv2_audio_signal_info,
							  0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_lv2_audio_signal);
  }

  return(g_define_type_id__static);
}

void
ags_fx_lv2_audio_signal_class_init(AgsFxLv2AudioSignalClass *fx_lv2_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  AgsFxNotationAudioSignalClass *fx_notation_audio_signal;

  ags_fx_lv2_audio_signal_recall_audio_signal_class = g_type_class_peek(AGS_TYPE_RECALL_AUDIO_SIGNAL);
  ags_fx_lv2_audio_signal_parent_class = g_type_class_peek_parent(fx_lv2_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_lv2_audio_signal;

  gobject->dispose = ags_fx_lv2_audio_signal_dispose;
  gobject->finalize = ags_fx_lv2_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_lv2_audio_signal;
  
  recall->run_inter = ags_fx_lv2_audio_signal_real_run_inter;
  
  /* AgsFxNotationAudioSignalClass */
  fx_notation_audio_signal = (AgsFxNotationAudioSignalClass *) fx_lv2_audio_signal;
  
  fx_notation_audio_signal->stream_feed = ags_fx_lv2_audio_signal_stream_feed;
  fx_notation_audio_signal->notify_remove = ags_fx_lv2_audio_signal_notify_remove;
}

void
ags_fx_lv2_audio_signal_init(AgsFxLv2AudioSignal *fx_lv2_audio_signal)
{
  AGS_RECALL(fx_lv2_audio_signal)->name = "ags-fx-lv2";
  AGS_RECALL(fx_lv2_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_lv2_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_lv2_audio_signal)->xml_type = "ags-fx-lv2-audio-signal";
}

void
ags_fx_lv2_audio_signal_dispose(GObject *gobject)
{
  AgsFxLv2AudioSignal *fx_lv2_audio_signal;
  
  fx_lv2_audio_signal = AGS_FX_LV2_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_lv2_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_lv2_audio_signal_finalize(GObject *gobject)
{
  AgsFxLv2AudioSignal *fx_lv2_audio_signal;
  
  fx_lv2_audio_signal = AGS_FX_LV2_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_lv2_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_lv2_audio_signal_real_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;
  AgsFxLv2Audio *fx_lv2_audio;
  AgsFxLv2Channel *fx_lv2_channel;
  AgsFxLv2ChannelProcessor *fx_lv2_channel_processor;
  AgsFxLv2Recycling *fx_lv2_recycling;
  AgsFxLv2AudioSignal *fx_lv2_audio_signal;
  AgsLv2Plugin *lv2_plugin;

  guint sound_scope;
  guint buffer_size;
  guint format;
  guint copy_mode_out, copy_mode_in;
  
  void (*run)(LV2_Handle instance,
	      uint32_t sample_count);

  GRecMutex *source_stream_mutex;
  GRecMutex *fx_lv2_audio_mutex;
  GRecMutex *fx_lv2_channel_mutex;
  GRecMutex *base_plugin_mutex;

  fx_lv2_audio_signal = (AgsFxLv2AudioSignal *) recall;
  
  fx_lv2_channel = NULL;
  fx_lv2_channel_processor = NULL;

  fx_lv2_recycling = NULL;

  source = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  sound_scope = ags_recall_get_sound_scope(recall);

  g_object_get(recall,
	       "parent", &fx_lv2_recycling,
	       NULL);

  g_object_get(fx_lv2_recycling,
	       "parent", &fx_lv2_channel_processor,
	       NULL);

  g_object_get(fx_lv2_channel_processor,
	       "recall-audio", &fx_lv2_audio,
	       NULL);

  /* get LV2 plugin */
  fx_lv2_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_audio);

  g_rec_mutex_lock(fx_lv2_audio_mutex);

  lv2_plugin = fx_lv2_audio->lv2_plugin;
  
  g_rec_mutex_unlock(fx_lv2_audio_mutex);

  if(lv2_plugin != NULL &&
     ags_base_plugin_test_flags((AgsBasePlugin *) lv2_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    if(fx_lv2_audio != NULL){
      g_object_unref(fx_lv2_audio);
    }
  
    if(fx_lv2_channel_processor != NULL){
      g_object_unref(fx_lv2_channel_processor);
    }
  
    if(fx_lv2_recycling != NULL){
      g_object_unref(fx_lv2_recycling);
    }  
    
    /* call parent */
    AGS_RECALL_CLASS(ags_fx_lv2_audio_signal_parent_class)->run_inter(recall);

    return;
  }

  g_object_get(recall,
	       "source", &source,
	       NULL);

  g_object_get(fx_lv2_channel_processor,
	       "recall-channel", &fx_lv2_channel,
	       NULL);
  
  g_object_get(source,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);

  /* get LV2 plugin */
  fx_lv2_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_channel);

  g_rec_mutex_lock(fx_lv2_channel_mutex);

  lv2_plugin = fx_lv2_channel->lv2_plugin;
  
  g_rec_mutex_unlock(fx_lv2_channel_mutex);

  if(!AGS_IS_LV2_PLUGIN(lv2_plugin)){
    return;
  }
  
  /* process data */
  source_stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2_plugin);

  g_rec_mutex_lock(base_plugin_mutex);

  run = AGS_LV2_PLUGIN_DESCRIPTOR(AGS_BASE_PLUGIN(lv2_plugin)->plugin_descriptor)->run;
  
  g_rec_mutex_unlock(base_plugin_mutex);

  copy_mode_out = ags_audio_buffer_util_get_copy_mode_from_format(&(fx_lv2_audio_signal->audio_buffer_util),
								  ags_audio_buffer_util_format_from_soundcard(&(fx_lv2_audio_signal->audio_buffer_util),
													      format),
								  AGS_AUDIO_BUFFER_UTIL_FLOAT);
  
  copy_mode_in = ags_audio_buffer_util_get_copy_mode_from_format(&(fx_lv2_audio_signal->audio_buffer_util),
								 AGS_AUDIO_BUFFER_UTIL_FLOAT,
								 ags_audio_buffer_util_format_from_soundcard(&(fx_lv2_audio_signal->audio_buffer_util),
													     format));

  if(run != NULL){
    AgsFxLv2ChannelInputData *input_data;

    g_rec_mutex_lock(fx_lv2_channel_mutex);
    
    input_data = fx_lv2_channel->input_data[sound_scope];

    if(input_data->output != NULL){
      ags_audio_buffer_util_clear_float(&(fx_lv2_audio_signal->audio_buffer_util),
					input_data->output, 1,
					fx_lv2_channel->output_port_count * buffer_size);
    }

    if(input_data->input != NULL){
      ags_audio_buffer_util_clear_float(&(fx_lv2_audio_signal->audio_buffer_util),
					input_data->input, 1,
					fx_lv2_channel->input_port_count * buffer_size);
    }

    g_rec_mutex_lock(source_stream_mutex);
    
    if(input_data->input != NULL &&
       fx_lv2_channel->input_port_count >= 1 &&
       source->stream_current != NULL){
      ags_audio_buffer_util_copy_buffer_to_buffer(&(fx_lv2_audio_signal->audio_buffer_util),
						  input_data->input, fx_lv2_channel->input_port_count, 0,
						  source->stream_current->data, 1, 0,
						  buffer_size, copy_mode_in);
    }

    if(input_data->lv2_handle != NULL &&
       input_data->lv2_handle[0] != NULL){
      run(input_data->lv2_handle[0],
	  (uint32_t) (fx_lv2_channel->output_port_count * buffer_size));
    }
    
    if(input_data->output != NULL &&
       fx_lv2_channel->output_port_count >= 1 &&
       source->stream_current != NULL){
      //NOTE:JK: only mono input, additional channels discarded
      ags_audio_buffer_util_copy_buffer_to_buffer(&(fx_lv2_audio_signal->audio_buffer_util),
						  source->stream_current->data, 1, 0,
						  input_data->output, fx_lv2_channel->output_port_count, 0,
						  buffer_size, copy_mode_out);
    }
    
    g_rec_mutex_unlock(source_stream_mutex);
    
    g_rec_mutex_unlock(fx_lv2_channel_mutex);
  }

  if(source == NULL ||
     source->stream_current == NULL){
    ags_recall_done(recall);
  }
  
  /* unref */
  if(source != NULL){
    g_object_unref(source);
  }

  if(fx_lv2_audio != NULL){
    g_object_unref(fx_lv2_audio);
  }
  
  if(fx_lv2_channel != NULL){
    g_object_unref(fx_lv2_channel);
  }
  
  if(fx_lv2_channel_processor != NULL){
    g_object_unref(fx_lv2_channel_processor);
  }
  
  if(fx_lv2_recycling != NULL){
    g_object_unref(fx_lv2_recycling);
  }  
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_lv2_audio_signal_recall_audio_signal_class)->run_inter(recall);
}

void
ags_fx_lv2_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
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
  AgsFxLv2Audio *fx_lv2_audio;
  AgsFxLv2AudioProcessor *fx_lv2_audio_processor;
  AgsFxLv2ChannelProcessor *fx_lv2_channel_processor;
  AgsFxLv2Recycling *fx_lv2_recycling;
  AgsFxLv2AudioSignal *fx_lv2_audio_signal;
  
  AgsLv2Plugin *lv2_plugin;

  GObject *output_soundcard;

  AgsMidiSmfUtil midi_smf_util;

  gboolean is_live_instrument;
  guint sound_scope;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;
  guint format;
  guint copy_mode_out;

  void (*run)(LV2_Handle instance,
	      uint32_t sample_count);
  
  GRecMutex *source_stream_mutex;
  GRecMutex *fx_lv2_audio_mutex;
  GRecMutex *base_plugin_mutex;

  fx_lv2_audio_signal = (AgsFxLv2AudioSignal *) fx_notation_audio_signal;

  audio = NULL;
  
  fx_lv2_audio = NULL;
  fx_lv2_audio_processor = NULL;
  
  fx_lv2_channel_processor = NULL;

  fx_lv2_recycling = NULL;

  output_soundcard = NULL;
  
  midi_smf_util.major = 1;
  midi_smf_util.minor = 0;

  sound_scope = ags_recall_get_sound_scope((AgsRecall *) fx_notation_audio_signal);

  if(!(sound_scope == AGS_SOUND_SCOPE_PLAYBACK ||
       sound_scope == AGS_SOUND_SCOPE_NOTATION ||
       sound_scope == AGS_SOUND_SCOPE_MIDI)){
    return;
  }

  audio_channel = 0;
  
  audio_start_mapping = 0;
  midi_start_mapping = 0;
  
  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_lv2_recycling,
	       "output-soundcard", &output_soundcard,
	       NULL);

  g_object_get(fx_lv2_recycling,
	       "parent", &fx_lv2_channel_processor,
	       NULL);
  
  g_object_get(fx_lv2_channel_processor,
	       "recall-audio", &fx_lv2_audio,
	       "recall-audio-run", &fx_lv2_audio_processor,
	       NULL);

  g_object_get(fx_lv2_audio,
	       "audio", &audio,
	       NULL);

  g_object_get(fx_lv2_audio_processor,
	       "audio-channel", &audio_channel,
	       NULL);
  
  g_object_get(source,
	       "format", &format,
	       NULL);

  /* get LV2 plugin */
  fx_lv2_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_audio);

  is_live_instrument = ags_fx_lv2_audio_test_flags(fx_lv2_audio, AGS_FX_LV2_AUDIO_LIVE_INSTRUMENT);

  g_rec_mutex_lock(fx_lv2_audio_mutex);

  lv2_plugin = fx_lv2_audio->lv2_plugin;
  
  g_rec_mutex_unlock(fx_lv2_audio_mutex);

  /* process data */
  source_stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2_plugin);

  g_rec_mutex_lock(base_plugin_mutex);

  run = AGS_LV2_PLUGIN_DESCRIPTOR(AGS_BASE_PLUGIN(lv2_plugin)->plugin_descriptor)->run;
  
  g_rec_mutex_unlock(base_plugin_mutex);

  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
    midi_note = (128 - y - 1 - audio_start_mapping + midi_start_mapping);
  }else{
    midi_note = (y - audio_start_mapping + midi_start_mapping);
  }
  
  copy_mode_out = ags_audio_buffer_util_get_copy_mode_from_format(&(fx_lv2_audio_signal->audio_buffer_util),
								  ags_audio_buffer_util_format_from_soundcard(&(fx_lv2_audio_signal->audio_buffer_util),
													      format),
								  AGS_AUDIO_BUFFER_UTIL_FLOAT);

  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxLv2AudioScopeData *scope_data;
    AgsFxLv2AudioChannelData *channel_data;
    AgsFxLv2AudioInputData *input_data;

    g_rec_mutex_lock(fx_lv2_audio_mutex);

    scope_data = fx_lv2_audio->scope_data[sound_scope];

    channel_data = scope_data->channel_data[audio_channel];

    input_data = channel_data->input_data[midi_note];

    g_rec_mutex_unlock(fx_lv2_audio_mutex);

    if(input_data == NULL){
      goto ags_fx_lv2_audio_signal_stream_feed_END;
    }
    
    g_rec_mutex_lock(fx_lv2_audio_mutex);

    input_data->event_buffer->data.note.note = midi_note;

    g_message("midi key - %d", midi_note);
#if 0
#endif
      
    input_data->key_on = input_data->key_on + 1;
    
    if(is_live_instrument){
      if(fx_lv2_audio->has_midiin_event_port){
	ags_lv2_plugin_event_buffer_append_midi(channel_data->midiin_event_port,
						AGS_FX_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
						input_data->event_buffer,
						1);
      }

      if(fx_lv2_audio->has_midiin_atom_port){
	uint8_t midi_buffer[8];

	uint32_t offset;
	uint32_t midi_buffer_size;

	uint32_t capacity = AGS_FX_LV2_AUDIO_DEFAULT_MIDI_LENGHT;
	
	lv2_atom_forge_set_buffer(&(channel_data->forge), (uint8_t *) channel_data->midiin_atom_port, capacity);
	lv2_atom_forge_sequence_head(&(channel_data->forge), &(channel_data->frame), 0);
	
	offset = floor(((offset_counter - x0) * delay + delay_counter) * buffer_size);
	  
	midi_buffer_size = ags_midi_smf_util_decode(&midi_smf_util,
						    midi_buffer,
						    input_data->event_buffer);

	ags_fx_lv2_audio_forge_midi_message(fx_lv2_audio,
					    channel_data,
					    offset,
					    (const uint8_t* const) midi_buffer,
					    midi_buffer_size);

	//	  ags_lv2_plugin_atom_sequence_append_midi(input_data->midiin_atom_port,
	//					   AGS_FX_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
	//					   input_data->event_buffer,
	//					   1);
      }
    }else{
      if(fx_lv2_audio->has_midiin_event_port){
	ags_lv2_plugin_event_buffer_append_midi(input_data->midiin_event_port,
						AGS_FX_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
						input_data->event_buffer,
						1);
      }

      if(fx_lv2_audio->has_midiin_atom_port){
	uint8_t midi_buffer[8];

	uint32_t offset;
	uint32_t midi_buffer_size;

	uint32_t capacity = AGS_FX_LV2_AUDIO_DEFAULT_MIDI_LENGHT;

	lv2_atom_forge_set_buffer(&(channel_data->forge), (uint8_t *) channel_data->midiin_atom_port, capacity);

	lv2_atom_forge_sequence_head(&(channel_data->forge), &(channel_data->frame), 0);

	offset = floor(((offset_counter - x0) * delay + delay_counter) * buffer_size);
	  
	midi_buffer_size = ags_midi_smf_util_decode(&midi_smf_util,
						    midi_buffer,
						    input_data->event_buffer);

	ags_fx_lv2_audio_forge_midi_message(fx_lv2_audio,
					    channel_data,
					    offset,
					    (const uint8_t* const) midi_buffer,
					    midi_buffer_size);

	//	  ags_lv2_plugin_atom_sequence_append_midi(input_data->midiin_atom_port,
	//					   AGS_FX_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
	//					   input_data->event_buffer,
	//					   1);
      }
    }

    if(is_live_instrument){
      if(channel_data->output != NULL){
	ags_audio_buffer_util_clear_float(&(fx_lv2_audio_signal->audio_buffer_util),
					  channel_data->output, fx_lv2_audio->output_port_count,
					  buffer_size);
      }

      if(run != NULL){
	run(channel_data->lv2_handle[0],
	    (uint32_t) (fx_lv2_audio->output_port_count * buffer_size));
      }

      g_rec_mutex_lock(source_stream_mutex);

      if(channel_data->output != NULL &&
	 fx_lv2_audio->output_port_count >= 1 &&
	 source->stream_current != NULL){
	//NOTE:JK: only mono input, additional channels discarded
	ags_audio_buffer_util_copy_buffer_to_buffer(&(fx_lv2_audio_signal->audio_buffer_util),
						    source->stream_current->data, 1, 0,
						    channel_data->output, fx_lv2_audio->output_port_count, 0,
						    buffer_size, copy_mode_out);
      }
	  
      g_rec_mutex_unlock(source_stream_mutex);

      if(fx_lv2_audio->has_midiin_atom_port){
	lv2_atom_sequence_clear(channel_data->midiin_atom_port);
      }
    }else{
      if(input_data->output != NULL){
	ags_audio_buffer_util_clear_float(&(fx_lv2_audio_signal->audio_buffer_util),
					  input_data->output, fx_lv2_audio->output_port_count,
					  buffer_size);
      }

      if(run != NULL){
	run(input_data->lv2_handle[0],
	    (uint32_t) (fx_lv2_audio->output_port_count * buffer_size));
      }
      
      g_rec_mutex_lock(source_stream_mutex);

      if(input_data->output != NULL &&
	 fx_lv2_audio->output_port_count >= 1 &&
	 source->stream_current != NULL){
	//NOTE:JK: only mono input, additional channels discarded
	ags_audio_buffer_util_copy_buffer_to_buffer(&(fx_lv2_audio_signal->audio_buffer_util),
						    source->stream_current->data, 1, 0,
						    input_data->output, fx_lv2_audio->output_port_count, 0,
						    buffer_size, copy_mode_out);
      }
	  
      g_rec_mutex_unlock(source_stream_mutex);

      if(fx_lv2_audio->has_midiin_atom_port){
	lv2_atom_sequence_clear(channel_data->midiin_atom_port);
      }
    }
    
    g_rec_mutex_unlock(fx_lv2_audio_mutex);
  }

 ags_fx_lv2_audio_signal_stream_feed_END:
  
  /* unref */
  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(fx_lv2_audio != NULL){
    g_object_unref(fx_lv2_audio);
  }

  if(fx_lv2_audio_processor != NULL){
    g_object_unref(fx_lv2_audio_processor);
  }
  
  if(fx_lv2_channel_processor != NULL){
    g_object_unref(fx_lv2_channel_processor);
  }
  
  if(fx_lv2_recycling != NULL){
    g_object_unref(fx_lv2_recycling);
  }
}

void
ags_fx_lv2_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
				      AgsAudioSignal *source,
				      AgsNote *note,
				      guint x0, guint x1,
				      guint y)
{
  AgsAudio *audio;
  AgsFxLv2Audio *fx_lv2_audio;
  AgsFxLv2AudioProcessor *fx_lv2_audio_processor;
  AgsFxLv2ChannelProcessor *fx_lv2_channel_processor;
  AgsFxLv2Recycling *fx_lv2_recycling;

  guint sound_scope;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;

  GRecMutex *fx_lv2_audio_mutex;

  audio = NULL;
  
  fx_lv2_audio = NULL;
  fx_lv2_audio_processor = NULL;

  fx_lv2_channel_processor = NULL;

  fx_lv2_recycling = NULL;

  sound_scope = ags_recall_get_sound_scope((AgsRecall *) fx_notation_audio_signal);

  if(!(sound_scope == AGS_SOUND_SCOPE_PLAYBACK ||
       sound_scope == AGS_SOUND_SCOPE_NOTATION ||
       sound_scope == AGS_SOUND_SCOPE_MIDI)){
    return;
  }
  
  audio_channel = 0;

  audio_start_mapping = 0;
  midi_start_mapping = 0;

  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_lv2_recycling,
	       NULL);

  g_object_get(fx_lv2_recycling,
	       "parent", &fx_lv2_channel_processor,
	       NULL);
  
  g_object_get(fx_lv2_channel_processor,
	       "recall-audio", &fx_lv2_audio,
	       "recall-audio-run", &fx_lv2_audio_processor,
	       NULL);

  g_object_get(fx_lv2_audio,
	       "audio", &audio,
	       NULL);

  g_object_get(fx_lv2_audio_processor,
	       "audio-channel", &audio_channel,
	       NULL);

  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  fx_lv2_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_audio);

  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
    midi_note = (128 - y - 1 - audio_start_mapping + midi_start_mapping);
  }else{
    midi_note = (y - audio_start_mapping + midi_start_mapping);
  }

  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxLv2AudioScopeData *scope_data;
    AgsFxLv2AudioChannelData *channel_data;
    AgsFxLv2AudioInputData *input_data;

    g_rec_mutex_lock(fx_lv2_audio_mutex);
      
    scope_data = fx_lv2_audio->scope_data[sound_scope];

    channel_data = scope_data->channel_data[audio_channel];

    input_data = channel_data->input_data[midi_note];

    g_rec_mutex_unlock(fx_lv2_audio_mutex);

    if(input_data == NULL){
      goto ags_fx_lv2_audio_signal_notify_remove_END;
    }
    
    g_rec_mutex_lock(fx_lv2_audio_mutex);

    if(input_data->key_on > 0){
      input_data->key_on -= 1;
    
      if(ags_fx_lv2_audio_test_flags(fx_lv2_audio, AGS_FX_LV2_AUDIO_LIVE_INSTRUMENT)){
	if(fx_lv2_audio->has_midiin_event_port){
	  ags_lv2_plugin_event_buffer_remove_midi(channel_data->midiin_event_port,
						  AGS_FX_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
						  midi_note);
	}

	if(fx_lv2_audio->has_midiin_atom_port){
	  ags_lv2_plugin_atom_sequence_remove_midi(channel_data->midiin_atom_port,
						   AGS_FX_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
						   midi_note);
	}
      }else{
	if(fx_lv2_audio->has_midiin_event_port){
	  ags_lv2_plugin_event_buffer_remove_midi(input_data->midiin_event_port,
						  AGS_FX_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
						  midi_note);
	}

	if(fx_lv2_audio->has_midiin_atom_port){
	  ags_lv2_plugin_atom_sequence_remove_midi(input_data->midiin_atom_port,
						   AGS_FX_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
						   midi_note);
	}
      }
    }

    g_rec_mutex_unlock(fx_lv2_audio_mutex);
  }

 ags_fx_lv2_audio_signal_notify_remove_END:
  
  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(fx_lv2_audio != NULL){
    g_object_unref(fx_lv2_audio);
  }

  if(fx_lv2_audio_processor != NULL){
    g_object_unref(fx_lv2_audio_processor);
  }
  
  if(fx_lv2_channel_processor != NULL){
    g_object_unref(fx_lv2_channel_processor);
  }
  
  if(fx_lv2_recycling != NULL){
    g_object_unref(fx_lv2_recycling);
  }
}

/**
 * ags_fx_lv2_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxLv2AudioSignal
 *
 * Returns: the new #AgsFxLv2AudioSignal
 *
 * Since: 3.3.0
 */
AgsFxLv2AudioSignal*
ags_fx_lv2_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxLv2AudioSignal *fx_lv2_audio_signal;

  fx_lv2_audio_signal = (AgsFxLv2AudioSignal *) g_object_new(AGS_TYPE_FX_LV2_AUDIO_SIGNAL,
							     "source", audio_signal,
							     NULL);

  return(fx_lv2_audio_signal);
}
