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

#include <ags/audio/fx/ags_fx_vst3_audio_signal.h>

#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_vst3_plugin.h>

#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/fx/ags_fx_vst3_audio.h>
#include <ags/audio/fx/ags_fx_vst3_audio_processor.h>
#include <ags/audio/fx/ags_fx_vst3_channel.h>
#include <ags/audio/fx/ags_fx_vst3_channel_processor.h>
#include <ags/audio/fx/ags_fx_vst3_recycling.h>

#include <ags/i18n.h>

void ags_fx_vst3_audio_signal_class_init(AgsFxVst3AudioSignalClass *fx_vst3_audio_signal);
void ags_fx_vst3_audio_signal_init(AgsFxVst3AudioSignal *fx_vst3_audio_signal);
void ags_fx_vst3_audio_signal_dispose(GObject *gobject);
void ags_fx_vst3_audio_signal_finalize(GObject *gobject);

void ags_fx_vst3_audio_signal_real_run_inter(AgsRecall *recall);

void ags_fx_vst3_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					  AgsAudioSignal *source,
					  AgsNote *note,
					  gboolean pattern_mode,
					  guint x0, guint x1,
					  guint y,
					  gdouble delay_counter, guint64 offset_counter,
					  guint frame_count,
					  gdouble delay, guint buffer_size);
void ags_fx_vst3_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					    AgsAudioSignal *source,
					    AgsNote *note,
					    guint x0, guint x1,
					    guint y);

/**
 * SECTION:ags_fx_vst3_audio_signal
 * @short_description: fx vst3 audio signal
 * @title: AgsFxVst3AudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_vst3_audio_signal.h
 *
 * The #AgsFxVst3AudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_vst3_audio_signal_recall_audio_signal_class = NULL;
static gpointer ags_fx_vst3_audio_signal_parent_class = NULL;

const gchar *ags_fx_vst3_audio_signal_plugin_name = "ags-fx-vst3";

GType
ags_fx_vst3_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_vst3_audio_signal = 0;

    static const GTypeInfo ags_fx_vst3_audio_signal_info = {
      sizeof (AgsFxVst3AudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_vst3_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxVst3AudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_vst3_audio_signal_init,
    };

    ags_type_fx_vst3_audio_signal = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL,
							   "AgsFxVst3AudioSignal",
							   &ags_fx_vst3_audio_signal_info,
							   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_vst3_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_vst3_audio_signal_class_init(AgsFxVst3AudioSignalClass *fx_vst3_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  AgsFxNotationAudioSignalClass *fx_notation_audio_signal;

  ags_fx_vst3_audio_signal_recall_audio_signal_class = g_type_class_peek(AGS_TYPE_RECALL_AUDIO_SIGNAL);
  ags_fx_vst3_audio_signal_parent_class = g_type_class_peek_parent(fx_vst3_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_vst3_audio_signal;

  gobject->dispose = ags_fx_vst3_audio_signal_dispose;
  gobject->finalize = ags_fx_vst3_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_vst3_audio_signal;
  
  recall->run_inter = ags_fx_vst3_audio_signal_real_run_inter;
  
  /* AgsFxNotationAudioSignalClass */
  fx_notation_audio_signal = (AgsFxNotationAudioSignalClass *) fx_vst3_audio_signal;
  
  fx_notation_audio_signal->stream_feed = ags_fx_vst3_audio_signal_stream_feed;
  fx_notation_audio_signal->notify_remove = ags_fx_vst3_audio_signal_notify_remove;
}

void
ags_fx_vst3_audio_signal_init(AgsFxVst3AudioSignal *fx_vst3_audio_signal)
{
  AGS_RECALL(fx_vst3_audio_signal)->name = "ags-fx-vst3";
  AGS_RECALL(fx_vst3_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_vst3_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_vst3_audio_signal)->xml_type = "ags-fx-vst3-audio-signal";
}

void
ags_fx_vst3_audio_signal_dispose(GObject *gobject)
{
  AgsFxVst3AudioSignal *fx_vst3_audio_signal;
  
  fx_vst3_audio_signal = AGS_FX_VST3_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_vst3_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_vst3_audio_signal_finalize(GObject *gobject)
{
  AgsFxVst3AudioSignal *fx_vst3_audio_signal;
  
  fx_vst3_audio_signal = AGS_FX_VST3_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_vst3_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_vst3_audio_signal_real_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;
  AgsFxVst3Audio *fx_vst3_audio;
  AgsFxVst3Channel *fx_vst3_channel;
  AgsFxVst3ChannelProcessor *fx_vst3_channel_processor;
  AgsFxVst3Recycling *fx_vst3_recycling;
  AgsVst3Plugin *vst3_plugin;
  GObject *output_soundcard;
  
  guint sound_scope;
  guint buffer_size;
  guint format;
  guint copy_mode_out, copy_mode_in;
  guint i;
  
  GRecMutex *source_stream_mutex;
  GRecMutex *fx_vst3_audio_mutex;
  GRecMutex *fx_vst3_channel_mutex;
  GRecMutex *base_plugin_mutex;

  fx_vst3_channel = NULL;
  fx_vst3_channel_processor = NULL;

  fx_vst3_recycling = NULL;

  source = NULL;
  output_soundcard = NULL;
  
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  sound_scope = ags_recall_get_sound_scope(recall);

  g_object_get(recall,
	       "parent", &fx_vst3_recycling,
	       NULL);

  g_object_get(fx_vst3_recycling,
	       "parent", &fx_vst3_channel_processor,
	       NULL);

  g_object_get(fx_vst3_channel_processor,
	       "recall-audio", &fx_vst3_audio,
	       NULL);

  /* get VST3 plugin */
  fx_vst3_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  g_rec_mutex_lock(fx_vst3_audio_mutex);

  vst3_plugin = fx_vst3_audio->vst3_plugin;
  
  g_rec_mutex_unlock(fx_vst3_audio_mutex);

  if(vst3_plugin != NULL &&
     ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    if(fx_vst3_audio != NULL){
      g_object_unref(fx_vst3_audio);
    }
  
    if(fx_vst3_channel_processor != NULL){
      g_object_unref(fx_vst3_channel_processor);
    }
  
    if(fx_vst3_recycling != NULL){
      g_object_unref(fx_vst3_recycling);
    }  
    
    /* call parent */
    AGS_RECALL_CLASS(ags_fx_vst3_audio_signal_parent_class)->run_inter(recall);

    return;
  }

  g_object_get(recall,
	       "source", &source,
	       "output-soundcard", &output_soundcard,
	       NULL);

  g_object_get(fx_vst3_channel_processor,
	       "recall-channel", &fx_vst3_channel,
	       NULL);
  
  g_object_get(source,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);

  /* get VST3 plugin */
  fx_vst3_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_channel);

  g_rec_mutex_lock(fx_vst3_channel_mutex);

  vst3_plugin = fx_vst3_channel->vst3_plugin;
  
  g_rec_mutex_unlock(fx_vst3_channel_mutex);

  /* process data */
  source_stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(vst3_plugin);
  
  copy_mode_out = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						      AGS_AUDIO_BUFFER_UTIL_FLOAT);
  
  copy_mode_in = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						     ags_audio_buffer_util_format_from_soundcard(format));

  if(fx_vst3_channel->input_data[sound_scope]->iaudio_processor != NULL){
    AgsFxVst3ChannelInputData *input_data;

    g_rec_mutex_lock(fx_vst3_channel_mutex);

    input_data = fx_vst3_channel->input_data[sound_scope];

    if(input_data->output != NULL){
      ags_audio_buffer_util_clear_float(input_data->output, 1,
					fx_vst3_channel->output_port_count * buffer_size);
    }

    if(input_data->input != NULL){
      ags_audio_buffer_util_clear_float(input_data->input, 1,
					fx_vst3_channel->input_port_count * buffer_size);
    }

    g_rec_mutex_lock(source_stream_mutex);
    
    if(input_data->input != NULL &&
       fx_vst3_channel->input_port_count >= 1 &&
       source->stream_current != NULL){
      ags_audio_buffer_util_copy_buffer_to_buffer(input_data->input, fx_vst3_channel->input_port_count, 0,
						  source->stream_current->data, 1, 0,
						  buffer_size, copy_mode_in);
    }

    for(i = 0; i < AGS_FX_VST3_CHANNEL_MAX_PARAMETER_CHANGES && input_data->parameter_changes[i].param_id != ~0; i++){
      AgsVstParameterValueQueue *parameter_value_queue;
	
      gint32 index;

      index = 0;
      parameter_value_queue = ags_vst_parameter_changes_add_parameter_data(input_data->input_parameter_changes,
									   &(input_data->parameter_changes[i].param_id), &index);

      index = 0;
      ags_vst_parameter_value_queue_add_point(parameter_value_queue,
					      0, input_data->parameter_changes[i].param_value,
					      &index);
    }

    input_data->parameter_changes[0].param_id = ~0;

    ags_vst_process_context_set_system_time(input_data->process_context,
					    ags_soundcard_util_calc_system_time(output_soundcard));
    ags_vst_process_context_set_project_time_samples(input_data->process_context,
						     ags_soundcard_util_calc_time_samples(output_soundcard));
    ags_vst_process_context_set_continous_time_samples(input_data->process_context,
						       ags_soundcard_util_calc_time_samples_absolute(output_soundcard));

    ags_vst_iaudio_processor_process(input_data->iaudio_processor,
				     input_data->process_data);  
    
    if(input_data->output != NULL &&
       fx_vst3_channel->output_port_count >= 1 &&
       source->stream_current != NULL){
      //NOTE:JK: only mono input, additional channels discarded
      ags_audio_buffer_util_clear_buffer(source->stream_current->data, 1,
					 buffer_size, ags_audio_buffer_util_format_from_soundcard(format));
      
      ags_audio_buffer_util_copy_buffer_to_buffer(source->stream_current->data, 1, 0,
						  input_data->output, fx_vst3_channel->output_port_count, 0,
						  buffer_size, copy_mode_out);
    }
    
    g_rec_mutex_unlock(source_stream_mutex);
    
    g_rec_mutex_unlock(fx_vst3_channel_mutex);
  }
  
  if(source == NULL ||
     source->stream_current == NULL){
    ags_recall_done(recall);
  }
  
  /* unref */
  if(source != NULL){
    g_object_unref(source);
  }

  if(fx_vst3_audio != NULL){
    g_object_unref(fx_vst3_audio);
  }
  
  if(fx_vst3_channel != NULL){
    g_object_unref(fx_vst3_channel);
  }
  
  if(fx_vst3_channel_processor != NULL){
    g_object_unref(fx_vst3_channel_processor);
  }
  
  if(fx_vst3_recycling != NULL){
    g_object_unref(fx_vst3_recycling);
  }  
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_vst3_audio_signal_recall_audio_signal_class)->run_inter(recall);
}

void
ags_fx_vst3_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
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
  AgsFxVst3Audio *fx_vst3_audio;
  AgsFxVst3AudioProcessor *fx_vst3_audio_processor;
  AgsFxVst3ChannelProcessor *fx_vst3_channel_processor;
  AgsFxVst3Recycling *fx_vst3_recycling;
  AgsFxVst3AudioSignal *fx_vst3_audio_signal;
  AgsVst3Plugin *vst3_plugin;
  GObject *output_soundcard;
  
  gboolean is_live_instrument;
  guint sound_scope;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;
  guint format;
  guint copy_mode_out;
  
  GRecMutex *source_stream_mutex;
  GRecMutex *fx_vst3_audio_mutex;
  GRecMutex *base_plugin_mutex;

  audio = NULL;

  output_soundcard = NULL;
  
  fx_vst3_audio = NULL;
  fx_vst3_audio_processor = NULL;
  
  fx_vst3_channel_processor = NULL;

  fx_vst3_recycling = NULL;

  sound_scope = ags_recall_get_sound_scope(fx_notation_audio_signal);

  audio_channel = 0;
  
  audio_start_mapping = 0;
  midi_start_mapping = 0;
  
  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_vst3_recycling,
	       NULL);

  g_object_get(fx_vst3_recycling,
	       "parent", &fx_vst3_channel_processor,
	       NULL);
  
  g_object_get(fx_vst3_channel_processor,
	       "recall-audio", &fx_vst3_audio,
	       "recall-audio-run", &fx_vst3_audio_processor,
	       NULL);

  g_object_get(fx_vst3_audio,
	       "audio", &audio,
	       "ouput-soundcard", &output_soundcard,
	       NULL);

  g_object_get(fx_vst3_audio_processor,
	       "audio-channel", &audio_channel,
	       NULL);
  
  g_object_get(source,
	       "format", &format,
	       NULL);

  /* get VST3 plugin */
  fx_vst3_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  is_live_instrument = ags_fx_vst3_audio_test_flags(fx_vst3_audio, AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);

  g_rec_mutex_lock(fx_vst3_audio_mutex);

  vst3_plugin = fx_vst3_audio->vst3_plugin;
  
  g_rec_mutex_unlock(fx_vst3_audio_mutex);

  /* process data */
  source_stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(vst3_plugin);

  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
    midi_note = (128 - y - 1 - audio_start_mapping + midi_start_mapping);
  }else{
    midi_note = (y - audio_start_mapping + midi_start_mapping);
  }
  
  copy_mode_out = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						      AGS_AUDIO_BUFFER_UTIL_FLOAT);

  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxVst3AudioScopeData *scope_data;
    AgsFxVst3AudioChannelData *channel_data;
    AgsFxVst3AudioInputData *input_data;

    g_rec_mutex_lock(fx_vst3_audio_mutex);

    scope_data = fx_vst3_audio->scope_data[sound_scope];

    channel_data = scope_data->channel_data[audio_channel];

    input_data = channel_data->input_data[midi_note];

    input_data->event_buffer->data.note.note = midi_note;

    g_rec_mutex_unlock(fx_vst3_audio_mutex);

    if(delay_counter == 0.0 &&
       x0 == offset_counter){
      g_rec_mutex_lock(fx_vst3_audio_mutex);
      
      input_data->key_on += 1;
      
      g_rec_mutex_unlock(fx_vst3_audio_mutex);
    }

    g_rec_mutex_lock(fx_vst3_audio_mutex);

    if(is_live_instrument){
      AgsVstEvent *note_off;

      guint i;
      
//	g_message("play channel data x0 = %d, y = %d", x0, y);
	
      ags_vst_process_context_set_system_time(channel_data->process_context,
					      ags_soundcard_util_calc_system_time(output_soundcard));
      ags_vst_process_context_set_project_time_samples(channel_data->process_context,
						       ags_soundcard_util_calc_time_samples(output_soundcard));
      ags_vst_process_context_set_continous_time_samples(channel_data->process_context,
							 ags_soundcard_util_calc_time_samples_absolute(output_soundcard));
      
      ags_vst_ievent_list_add_event(channel_data->input_event,
				    ags_vst_note_on_event_alloc(0,
								midi_note,
								0.0,
								1.0,
								buffer_size, // (x1 - x0) * (delay * buffer_size),
								-1));

      note_off = ags_vst_note_off_event_alloc(0,
					      midi_note,
					      0.0,
					      1.0,
					      -1,
					      -1);
      
      ags_vst_event_set_sample_offset(note_off,
				      buffer_size);

      ags_vst_ievent_list_add_event(channel_data->input_event,
				    note_off);

      for(i = 0; i < AGS_FX_VST3_AUDIO_MAX_PARAMETER_CHANGES && channel_data->parameter_changes[i].param_id != ~0; i++){
	AgsVstParameterValueQueue *parameter_value_queue;
	
	gint32 index;

	index = 0;
	parameter_value_queue = ags_vst_parameter_changes_add_parameter_data(channel_data->input_parameter_changes,
									     &(channel_data->parameter_changes[i].param_id), &index);

	index = 0;
	ags_vst_parameter_value_queue_add_point(parameter_value_queue,
						0, channel_data->parameter_changes[i].param_value,
						&index);
      }

      channel_data->parameter_changes[0].param_id = ~0;
    }else{
      AgsVstEvent *note_off;

      guint i;
      
//	g_message("play input data x0 = %d, y = %d", x0, y);

      ags_vst_process_context_set_system_time(input_data->process_context,
					      ags_soundcard_util_calc_system_time(output_soundcard));
      ags_vst_process_context_set_project_time_samples(input_data->process_context,
						       ags_soundcard_util_calc_time_samples(output_soundcard));
      ags_vst_process_context_set_continous_time_samples(input_data->process_context,
							 ags_soundcard_util_calc_time_samples_absolute(output_soundcard));

      ags_vst_ievent_list_add_event(input_data->input_event,
				    ags_vst_note_on_event_alloc(0,
								midi_note,
								0.0,
								1.0,
								buffer_size, // (x1 - x0) * (delay * buffer_size),
								-1));

      note_off = ags_vst_note_off_event_alloc(0,
					      midi_note,
					      0.0,
					      1.0,
					      -1,
					      -1);
      
      ags_vst_event_set_sample_offset(note_off,
				      buffer_size);
      
      ags_vst_ievent_list_add_event(input_data->input_event,
				    note_off);

      for(i = 0; i < AGS_FX_VST3_AUDIO_MAX_PARAMETER_CHANGES && input_data->parameter_changes[i].param_id != ~0; i++){
	AgsVstParameterValueQueue *parameter_value_queue;
	
      	gint32 index;
	
	index = 0;
	parameter_value_queue = ags_vst_parameter_changes_add_parameter_data(input_data->input_parameter_changes,
									     &(input_data->parameter_changes[i].param_id), &index);

	index = 0;
	ags_vst_parameter_value_queue_add_point(parameter_value_queue,
						0, input_data->parameter_changes[i].param_value,
						&index);
      }

      input_data->parameter_changes[0].param_id = ~0;
    }            

    g_rec_mutex_unlock(fx_vst3_audio_mutex);

    
    if(is_live_instrument){
      g_rec_mutex_lock(fx_vst3_audio_mutex);
      
      if(channel_data->output != NULL){
	ags_audio_buffer_util_clear_float(channel_data->output, 1,
					  fx_vst3_audio->output_port_count * buffer_size);
      }
      
      if(channel_data->input != NULL){
	ags_audio_buffer_util_clear_float(channel_data->input, 1,
					  fx_vst3_audio->input_port_count * buffer_size);
      }

      if(channel_data->iaudio_processor != NULL){
	ags_vst_iaudio_processor_process(channel_data->iaudio_processor,
					 channel_data->process_data);  
      }

      g_rec_mutex_lock(source_stream_mutex);

      if(channel_data->output != NULL &&
	 fx_vst3_audio->output_port_count >= 1 &&
	 source->stream_current != NULL){
	ags_audio_buffer_util_clear_buffer(source->stream_current->data, 1,
					   buffer_size, ags_audio_buffer_util_format_from_soundcard(format));

	//NOTE:JK: only mono input, additional channels discarded
	ags_audio_buffer_util_copy_buffer_to_buffer(source->stream_current->data, 1, 0,
						    channel_data->output, fx_vst3_audio->output_port_count, 0,
						    buffer_size, copy_mode_out);
      }
	  
      g_rec_mutex_unlock(source_stream_mutex);

      g_rec_mutex_unlock(fx_vst3_audio_mutex);
    }else{
      g_rec_mutex_lock(fx_vst3_audio_mutex);
      
      if(input_data->output != NULL){
	ags_audio_buffer_util_clear_float(input_data->output, 1,
					  fx_vst3_audio->output_port_count * buffer_size);
      }
      
      if(input_data->input != NULL){
	ags_audio_buffer_util_clear_float(input_data->input, 1,
					  fx_vst3_audio->input_port_count * buffer_size);
      }

      if(input_data->iaudio_processor != NULL){
	ags_vst_iaudio_processor_process(input_data->iaudio_processor,
					 input_data->process_data);  
      }
      
      g_rec_mutex_lock(source_stream_mutex);

      if(input_data->output != NULL &&
	 fx_vst3_audio->output_port_count >= 1 &&
	 source->stream_current != NULL){
	ags_audio_buffer_util_clear_buffer(source->stream_current->data, 1,
					   buffer_size, ags_audio_buffer_util_format_from_soundcard(format));

	//NOTE:JK: only mono input, additional channels discarded
	ags_audio_buffer_util_copy_buffer_to_buffer(source->stream_current->data, 1, 0,
						    input_data->output, fx_vst3_audio->output_port_count, 0,
						    buffer_size, copy_mode_out);
      }
	  
      g_rec_mutex_unlock(source_stream_mutex);

      g_rec_mutex_unlock(fx_vst3_audio_mutex);
    }

    g_rec_mutex_lock(fx_vst3_audio_mutex);

    if(is_live_instrument){
      ags_vst_event_list_clear(channel_data->input_event);

      ags_vst_parameter_changes_clear_queue(channel_data->input_parameter_changes);
    }else{
      ags_vst_event_list_clear(input_data->input_event);

      ags_vst_parameter_changes_clear_queue(input_data->input_parameter_changes);
    }            

    g_rec_mutex_unlock(fx_vst3_audio_mutex);
  }
  
  /* unref */
  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(fx_vst3_audio != NULL){
    g_object_unref(fx_vst3_audio);
  }

  if(fx_vst3_audio_processor != NULL){
    g_object_unref(fx_vst3_audio_processor);
  }
  
  if(fx_vst3_channel_processor != NULL){
    g_object_unref(fx_vst3_channel_processor);
  }
  
  if(fx_vst3_recycling != NULL){
    g_object_unref(fx_vst3_recycling);
  }
}

void
ags_fx_vst3_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
				       AgsAudioSignal *source,
				       AgsNote *note,
				       guint x0, guint x1,
				       guint y)
{
  AgsAudio *audio;
  AgsFxVst3Audio *fx_vst3_audio;
  AgsFxVst3AudioProcessor *fx_vst3_audio_processor;
  AgsFxVst3ChannelProcessor *fx_vst3_channel_processor;
  AgsFxVst3Recycling *fx_vst3_recycling;

  gboolean is_live_instrument;
  guint sound_scope;
  guint audio_channel;
  guint buffer_size;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;

  GRecMutex *fx_vst3_audio_mutex;

  audio = NULL;
  
  fx_vst3_audio = NULL;
  fx_vst3_audio_processor = NULL;

  fx_vst3_channel_processor = NULL;

  fx_vst3_recycling = NULL;

  sound_scope = ags_recall_get_sound_scope(fx_notation_audio_signal);

  audio_channel = 0;

  audio_start_mapping = 0;
  midi_start_mapping = 0;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  g_object_get(source,
	       "buffer-size", &buffer_size,
	       NULL);

  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_vst3_recycling,
	       NULL);

  g_object_get(fx_vst3_recycling,
	       "parent", &fx_vst3_channel_processor,
	       NULL);
  
  g_object_get(fx_vst3_channel_processor,
	       "recall-audio", &fx_vst3_audio,
	       "recall-audio-run", &fx_vst3_audio_processor,
	       NULL);

  g_object_get(fx_vst3_audio,
	       "audio", &audio,
	       NULL);

  g_object_get(fx_vst3_audio_processor,
	       "audio-channel", &audio_channel,
	       NULL);

  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  fx_vst3_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  is_live_instrument = ags_fx_vst3_audio_test_flags(fx_vst3_audio, AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);

  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
    midi_note = (128 - y - 1 - audio_start_mapping + midi_start_mapping);
  }else{
    midi_note = (y - audio_start_mapping + midi_start_mapping);
  }

  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxVst3AudioScopeData *scope_data;
    AgsFxVst3AudioChannelData *channel_data;
    AgsFxVst3AudioInputData *input_data;

    g_rec_mutex_lock(fx_vst3_audio_mutex);
      
    scope_data = fx_vst3_audio->scope_data[sound_scope];

    channel_data = scope_data->channel_data[audio_channel];

    input_data = channel_data->input_data[midi_note];

    input_data->key_on -= 1;
    
    g_rec_mutex_unlock(fx_vst3_audio_mutex);
  }
  
  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(fx_vst3_audio != NULL){
    g_object_unref(fx_vst3_audio);
  }

  if(fx_vst3_audio_processor != NULL){
    g_object_unref(fx_vst3_audio_processor);
  }
  
  if(fx_vst3_channel_processor != NULL){
    g_object_unref(fx_vst3_channel_processor);
  }
  
  if(fx_vst3_recycling != NULL){
    g_object_unref(fx_vst3_recycling);
  }
}

/**
 * ags_fx_vst3_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxVst3AudioSignal
 *
 * Returns: the new #AgsFxVst3AudioSignal
 *
 * Since: 3.10.5
 */
AgsFxVst3AudioSignal*
ags_fx_vst3_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxVst3AudioSignal *fx_vst3_audio_signal;

  fx_vst3_audio_signal = (AgsFxVst3AudioSignal *) g_object_new(AGS_TYPE_FX_VST3_AUDIO_SIGNAL,
							       "source", audio_signal,
							       NULL);

  return(fx_vst3_audio_signal);
}
