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

#include <ags/audio/fx/ags_fx_vst3_audio_processor.h>

#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_vst3_plugin.h>

#include <ags/audio/ags_soundcard_util.h>

#include <ags/audio/task/ags_write_vst3_port.h>

#include <ags/audio/fx/ags_fx_vst3_audio.h>
#include <ags/audio/fx/ags_fx_vst3_audio_processor.h>

#include <ags/i18n.h>

void ags_fx_vst3_audio_processor_class_init(AgsFxVst3AudioProcessorClass *fx_vst3_audio_processor);
void ags_fx_vst3_audio_processor_init(AgsFxVst3AudioProcessor *fx_vst3_audio_processor);
void ags_fx_vst3_audio_processor_dispose(GObject *gobject);
void ags_fx_vst3_audio_processor_finalize(GObject *gobject);

void ags_fx_vst3_audio_processor_run_inter(AgsRecall *recall);
void ags_fx_vst3_audio_processor_done(AgsRecall *recall);

void ags_fx_vst3_audio_processor_key_on(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
					AgsNote *note,
					guint velocity,
					guint key_mode);

/**
 * SECTION:ags_fx_vst3_audio_processor
 * @short_description: fx vst3 audio processor
 * @title: AgsFxVst3AudioProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_vst3_audio_processor.h
 *
 * The #AgsFxVst3AudioProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_vst3_audio_processor_parent_class = NULL;

GType
ags_fx_vst3_audio_processor_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_vst3_audio_processor = 0;

    static const GTypeInfo ags_fx_vst3_audio_processor_info = {
      sizeof (AgsFxVst3AudioProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_vst3_audio_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_processor */
      sizeof (AgsFxVst3AudioProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_vst3_audio_processor_init,
    };

    ags_type_fx_vst3_audio_processor = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO_PROCESSOR,
							      "AgsFxVst3AudioProcessor",
							      &ags_fx_vst3_audio_processor_info,
							      0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_vst3_audio_processor);
  }

  return(g_define_type_id__static);
}

void
ags_fx_vst3_audio_processor_class_init(AgsFxVst3AudioProcessorClass *fx_vst3_audio_processor)
{
  AgsFxNotationAudioProcessorClass *fx_notation_audio_processor;
  AgsRecallClass *recall;
  GObjectClass *gobject;
  
  ags_fx_vst3_audio_processor_parent_class = g_type_class_peek_parent(fx_vst3_audio_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_vst3_audio_processor;

  gobject->dispose = ags_fx_vst3_audio_processor_dispose;
  gobject->finalize = ags_fx_vst3_audio_processor_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_vst3_audio_processor;

  recall->run_inter = ags_fx_vst3_audio_processor_run_inter;
  recall->done = ags_fx_vst3_audio_processor_done;
  
  /* AgsFxNotationAudioProcessorClass */
  fx_notation_audio_processor = (AgsFxNotationAudioProcessorClass *) fx_vst3_audio_processor;
  
  fx_notation_audio_processor->key_on = ags_fx_vst3_audio_processor_key_on;
}

void
ags_fx_vst3_audio_processor_init(AgsFxVst3AudioProcessor *fx_vst3_audio_processor)
{
  AGS_RECALL(fx_vst3_audio_processor)->name = "ags-fx-vst3";
  AGS_RECALL(fx_vst3_audio_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_vst3_audio_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_vst3_audio_processor)->xml_type = "ags-fx-vst3-audio-processor";

  fx_vst3_audio_processor->activated = FALSE;
}

void
ags_fx_vst3_audio_processor_dispose(GObject *gobject)
{
  AgsFxVst3AudioProcessor *fx_vst3_audio_processor;
  
  fx_vst3_audio_processor = AGS_FX_VST3_AUDIO_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_vst3_audio_processor_parent_class)->dispose(gobject);
}

void
ags_fx_vst3_audio_processor_finalize(GObject *gobject)
{
  AgsFxVst3AudioProcessor *fx_vst3_audio_processor;
  
  fx_vst3_audio_processor = AGS_FX_VST3_AUDIO_PROCESSOR(gobject);  
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_vst3_audio_processor_parent_class)->finalize(gobject);
}

void
ags_fx_vst3_audio_processor_run_init_pre(AgsRecall *recall)
{
  AgsFxVst3Audio *fx_vst3_audio;
  
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_vst3_audio_processor_parent_class)->run_init_pre(recall);
}
        
void
ags_fx_vst3_audio_processor_run_inter(AgsRecall *recall)
{
  AgsFxVst3Audio *fx_vst3_audio;

  AgsVst3Plugin *vst3_plugin;

  AgsFxVst3AudioScopeData *scope_data;

  GObject *output_soundcard;

  guint audio_channel;
  guint note_offset;
  gdouble delay;
  guint delay_counter;
  guint samplerate;
  guint buffer_size;
  guint project_time_samples;
  gdouble bpm;
  guint loop_left, loop_right;
  guint sound_scope;
  guint j, k;
  guint nth;
  gboolean is_live_instrument;
  gboolean activated;
  gboolean do_loop;
  
  GRecMutex *fx_vst3_audio_mutex;
  GRecMutex *fx_vst3_audio_processor_mutex;

  fx_vst3_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);
  
  fx_vst3_audio = NULL;
  output_soundcard = NULL;
  
  g_object_get(recall,
	       "recall-audio", &fx_vst3_audio,
	       "output-soundcard", &output_soundcard,
	       NULL);

  sound_scope = ags_recall_get_sound_scope(recall);

  fx_vst3_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);
  
  is_live_instrument = ags_fx_vst3_audio_test_flags(fx_vst3_audio, AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);

  g_rec_mutex_lock(fx_vst3_audio_mutex);

  vst3_plugin = fx_vst3_audio->vst3_plugin;
  
  g_rec_mutex_unlock(fx_vst3_audio_mutex);

  if(vst3_plugin == NULL){
    /* unref */
    if(fx_vst3_audio != NULL){
      g_object_unref(fx_vst3_audio);
    }

    if(output_soundcard != NULL){
      g_object_unref(output_soundcard);
    }

    return;
  }

  audio_channel = 0;

  g_object_get(recall,
	       "audio-channel", &audio_channel,
	       NULL);
  
  g_rec_mutex_lock(fx_vst3_audio_processor_mutex);

  activated = AGS_FX_VST3_AUDIO_PROCESSOR(recall)->activated;
  
  g_rec_mutex_unlock(fx_vst3_audio_processor_mutex);

  note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));

  delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(output_soundcard));
  delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(output_soundcard));

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate =  AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  ags_soundcard_get_presets(AGS_SOUNDCARD(output_soundcard),
			    NULL,
			    &samplerate,
			    &buffer_size,
			    NULL);

  ags_soundcard_get_loop(AGS_SOUNDCARD(output_soundcard),
			 &loop_left, &loop_right,
			 &do_loop);

  bpm = ags_soundcard_get_bpm(AGS_SOUNDCARD(output_soundcard));
      
  project_time_samples = ags_soundcard_util_calc_time_samples(output_soundcard);

  if(!activated &&
     ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){  
    /* get recall mutex */
    g_rec_mutex_lock(fx_vst3_audio_mutex);

    scope_data = fx_vst3_audio->scope_data[sound_scope];
  
    if(sound_scope == AGS_SOUND_SCOPE_PLAYBACK ||
       sound_scope == AGS_SOUND_SCOPE_NOTATION ||
       sound_scope == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxVst3AudioChannelData *channel_data;;

	channel_data = scope_data->channel_data[j];

	if(is_live_instrument){
	  AgsPort **iter;
	  
	  AgsWriteVst3Port *write_vst3_port;

	  gdouble val;
	  
	  ags_vst_process_context_set_state(channel_data->process_context,
					    AGS_VST_KPLAYING);
	
	  ags_base_plugin_activate((AgsBasePlugin *) vst3_plugin,
				   channel_data->icomponent);

//	  ags_vst_icomponent_activate_bus(channel_data->icomponent,
//					  AGS_VST_KAUDIO, AGS_VST_KINPUT,
//					  0,
//					  TRUE);

	  ags_vst_icomponent_activate_bus(channel_data->icomponent,
					  AGS_VST_KAUDIO, AGS_VST_KOUTPUT,
					  0,
					  TRUE);

	  ags_vst_icomponent_activate_bus(channel_data->icomponent,
					  AGS_VST_KEVENT, AGS_VST_KINPUT,
					  0,
					  TRUE);

	  /* apply port */
#if 0
	  iter = fx_vst3_audio->vst3_port;

	  for(; iter != NULL && iter[0] != NULL; iter++){
	    GValue value = G_VALUE_INIT;
	  
	    val = 1.0;

	    g_value_init(&value,
			 G_TYPE_DOUBLE);
	    
	    ags_port_safe_read(iter[0],
			       &value);

	    val = g_value_get_double(&value);
	    
	    write_vst3_port = ags_write_vst3_port_new(fx_vst3_audio,
						      iter[0],
						      val,
						      sound_scope,
						      audio_channel);
	    ags_task_launch(write_vst3_port);

	    g_value_unset(&value);
	      
	    g_object_unref(write_vst3_port);
	  }
#endif
	}

	if(!is_live_instrument){
	  for(k = 0; k < AGS_SEQUENCER_MAX_MIDI_KEYS; k++){
	    AgsFxVst3AudioInputData *input_data;

	    AgsPort **iter;
	  
	    AgsWriteVst3Port *write_vst3_port;

	    gdouble val;

	    input_data = channel_data->input_data[k];

	    ags_vst_process_context_set_state(input_data->process_context,
					      AGS_VST_KPLAYING);
	  
	    ags_base_plugin_activate((AgsBasePlugin *) vst3_plugin,
				     input_data->icomponent);

//	    ags_vst_icomponent_activate_bus(input_data->icomponent,
//					    AGS_VST_KAUDIO, AGS_VST_KINPUT,
//					    0,
//					    TRUE);

	    ags_vst_icomponent_activate_bus(input_data->icomponent,
					    AGS_VST_KAUDIO, AGS_VST_KOUTPUT,
					    0,
					    TRUE);

	    ags_vst_icomponent_activate_bus(input_data->icomponent,
					    AGS_VST_KEVENT, AGS_VST_KINPUT,
					    0,
					    TRUE);

#if 0
	    iter = fx_vst3_audio->vst3_port;

	    for(; iter != NULL && iter[0] != NULL; iter++){
	      GValue value = G_VALUE_INIT;
	  
	      val = 1.0;

	      g_value_init(&value,
			   G_TYPE_DOUBLE);
	    
	      ags_port_safe_read(iter[0],
				 &value);

	      val = g_value_get_double(&value);
	    
	      write_vst3_port = ags_write_vst3_port_new(fx_vst3_audio,
							iter[0],
							val,
							sound_scope,
							audio_channel);
	      ags_task_launch(write_vst3_port);

	      g_value_unset(&value);
	      
	      g_object_unref(write_vst3_port);
	    }
#endif
	  }
	}
      }
    }
  
    g_rec_mutex_unlock(fx_vst3_audio_mutex);

    /* apply default values */

    
    /* set activated */
    g_rec_mutex_lock(fx_vst3_audio_processor_mutex);

    AGS_FX_VST3_AUDIO_PROCESSOR(recall)->activated = TRUE;
  
    g_rec_mutex_unlock(fx_vst3_audio_processor_mutex);
  }
  
  g_rec_mutex_lock(fx_vst3_audio_mutex);

  scope_data = fx_vst3_audio->scope_data[sound_scope];
    
  if(sound_scope == AGS_SOUND_SCOPE_PLAYBACK ||
     sound_scope == AGS_SOUND_SCOPE_NOTATION ||
     sound_scope == AGS_SOUND_SCOPE_MIDI){
    for(j = 0; j < scope_data->audio_channels; j++){
      AgsFxVst3AudioChannelData *channel_data;;

      channel_data = scope_data->channel_data[j];

      ags_vst_process_context_set_samplerate(channel_data->process_context,
					     samplerate);
      
      ags_vst_process_context_set_system_time(channel_data->process_context,
					      ags_soundcard_util_calc_system_time(output_soundcard));

      ags_vst_process_context_set_project_time_samples(channel_data->process_context,
						       project_time_samples);

      ags_vst_process_context_set_continous_time_samples(channel_data->process_context,
							 ags_soundcard_util_calc_time_samples_absolute(output_soundcard));

      ags_vst_process_context_set_project_time_music(channel_data->process_context,
						     0.25 * note_offset);

      ags_vst_process_context_set_cycle_start_music(channel_data->process_context,
						    0.25 * loop_left);
      ags_vst_process_context_set_cycle_end_music(channel_data->process_context,
						  0.25 * loop_right);
      
      ags_vst_process_context_set_tempo(channel_data->process_context,
					bpm);

      ags_vst_process_context_set_state(channel_data->process_context,
					(AGS_VST_KPLAYING |
					 AGS_VST_KSYSTEM_TIME_VALID |
					 AGS_VST_KCONST_TIME_VALID |
					 AGS_VST_KPROJECT_TIME_MUSIC_VALID |
					 AGS_VST_kCycleValid |
					 AGS_VST_KTEMPO_VALID));
      
      if(is_live_instrument){
	for(nth = 0; nth < AGS_FX_VST3_AUDIO_MAX_PARAMETER_CHANGES && fx_vst3_audio->parameter_changes[nth].param_id != ~0; nth++){
	  channel_data->parameter_changes[nth].param_id = fx_vst3_audio->parameter_changes[nth].param_id;
	  channel_data->parameter_changes[nth].param_value = fx_vst3_audio->parameter_changes[nth].param_value;
	}
      }

      if(!is_live_instrument){
	for(k = 0; k < AGS_SEQUENCER_MAX_MIDI_KEYS; k++){
	  AgsFxVst3AudioInputData *input_data;

	  input_data = channel_data->input_data[k];

	  ags_vst_process_context_set_samplerate(input_data->process_context,
						 samplerate);
      
	  ags_vst_process_context_set_system_time(input_data->process_context,
						  ags_soundcard_util_calc_system_time(output_soundcard));

	  ags_vst_process_context_set_project_time_samples(input_data->process_context,
							   project_time_samples);
	  ags_vst_process_context_set_continous_time_samples(input_data->process_context,
							     ags_soundcard_util_calc_time_samples_absolute(output_soundcard));

	  ags_vst_process_context_set_project_time_music(input_data->process_context,
							 0.25 * note_offset);

	  ags_vst_process_context_set_cycle_start_music(input_data->process_context,
							0.25 * loop_left);
	  ags_vst_process_context_set_cycle_end_music(input_data->process_context,
						      0.25 * loop_right);
      
	  ags_vst_process_context_set_tempo(input_data->process_context,
					    bpm);

	  ags_vst_process_context_set_state(input_data->process_context,
					    (AGS_VST_KPLAYING |
					     AGS_VST_KSYSTEM_TIME_VALID |
					     AGS_VST_KCONST_TIME_VALID |
					     AGS_VST_KPROJECT_TIME_MUSIC_VALID |
					     AGS_VST_kCycleValid |
					     AGS_VST_KTEMPO_VALID));

	  for(nth = 0; nth < AGS_FX_VST3_AUDIO_MAX_PARAMETER_CHANGES && fx_vst3_audio->parameter_changes[nth].param_id != ~0; nth++){
	    input_data->parameter_changes[nth].param_id = fx_vst3_audio->parameter_changes[nth].param_id;
	    input_data->parameter_changes[nth].param_value = fx_vst3_audio->parameter_changes[nth].param_value;
	  }
	}
      }
    }
  }

  fx_vst3_audio->parameter_changes[0].param_id = ~0;
  
  g_rec_mutex_unlock(fx_vst3_audio_mutex);

  if(fx_vst3_audio != NULL){
    g_object_unref(fx_vst3_audio);
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  /* call parent */
  AGS_RECALL_CLASS(ags_fx_vst3_audio_processor_parent_class)->run_inter(recall);
}

void
ags_fx_vst3_audio_processor_done(AgsRecall *recall)
{  
  AgsFxVst3Audio *fx_vst3_audio;
  
  AgsVst3Plugin *vst3_plugin;

  AgsFxVst3AudioScopeData *scope_data;

  guint sound_scope;
  gboolean is_live_instrument;

  guint j, k;
  GRecMutex *recall_mutex;
  
  GRecMutex *fx_vst3_audio_mutex;
  
  fx_vst3_audio = NULL;

  g_object_get(recall,
	       "recall-audio", &fx_vst3_audio,
	       NULL);

  sound_scope = ags_recall_get_sound_scope(recall);

  /* get VST3 plugin */
  fx_vst3_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  is_live_instrument = ags_fx_vst3_audio_test_flags(fx_vst3_audio, AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);

  g_rec_mutex_lock(fx_vst3_audio_mutex);

  vst3_plugin = fx_vst3_audio->vst3_plugin;
  
  g_rec_mutex_unlock(fx_vst3_audio_mutex);

  if(vst3_plugin == NULL ||
     !ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    /* unref */
    if(fx_vst3_audio != NULL){
      g_object_unref(fx_vst3_audio);
    }

    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  g_rec_mutex_lock(recall_mutex);

  scope_data = fx_vst3_audio->scope_data[sound_scope];
  
  if(sound_scope == AGS_SOUND_SCOPE_PLAYBACK ||
     sound_scope == AGS_SOUND_SCOPE_NOTATION ||
     sound_scope == AGS_SOUND_SCOPE_MIDI){
    for(j = 0; j < scope_data->audio_channels; j++){
      AgsFxVst3AudioChannelData *channel_data;;

      channel_data = scope_data->channel_data[j];

      if(is_live_instrument){
	ags_vst_process_context_set_state(channel_data->process_context,
					  0);
	
	ags_base_plugin_deactivate((AgsBasePlugin *) vst3_plugin,
				   channel_data->icomponent);

//	ags_vst_icomponent_activate_bus(channel_data->icomponent,
//					AGS_VST_KAUDIO, AGS_VST_KINPUT,
//					0,
//					FALSE);

	ags_vst_icomponent_activate_bus(channel_data->icomponent,
					AGS_VST_KAUDIO, AGS_VST_KOUTPUT,
					0,
					FALSE);

	ags_vst_icomponent_activate_bus(channel_data->icomponent,
					AGS_VST_KEVENT, AGS_VST_KINPUT,
					0,
					FALSE);
      }

      if(!is_live_instrument){
	for(k = 0; k < AGS_SEQUENCER_MAX_MIDI_KEYS; k++){
	  AgsFxVst3AudioInputData *input_data;

	  input_data = channel_data->input_data[k];

	  ags_vst_process_context_set_state(input_data->process_context,
					    0);
	
	  ags_base_plugin_deactivate((AgsBasePlugin *) vst3_plugin,
				     input_data->icomponent);

//	  ags_vst_icomponent_activate_bus(input_data->icomponent,
//					  AGS_VST_KAUDIO, AGS_VST_KINPUT,
//					  0,
//					  FALSE);

	  ags_vst_icomponent_activate_bus(input_data->icomponent,
					  AGS_VST_KAUDIO, AGS_VST_KOUTPUT,
					  0,
					  FALSE);

	  ags_vst_icomponent_activate_bus(input_data->icomponent,
					  AGS_VST_KEVENT, AGS_VST_KINPUT,
					  0,
					  FALSE);
	}
      }
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);
      
  /* unref */
  if(fx_vst3_audio != NULL){
    g_object_unref(fx_vst3_audio);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_vst3_audio_processor_parent_class)->done(recall);
}

void
ags_fx_vst3_audio_processor_key_on(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
				   AgsNote *note,
				   guint velocity,
				   guint key_mode)
{
  AgsAudio *audio;
  AgsFxVst3Audio *fx_vst3_audio;

  guint sound_scope;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;
  guint y;
  
  GRecMutex *fx_vst3_audio_mutex;

  audio = NULL;
  
  audio_start_mapping = 0;
  midi_start_mapping = 0;

  y = 0;

  sound_scope = ags_recall_get_sound_scope((AgsRecall *) fx_notation_audio_processor);

  audio_channel = 0;
  
  g_object_get(fx_notation_audio_processor,
	       "audio", &audio,
	       "recall-audio", &fx_vst3_audio,
	       "audio-channel", &audio_channel,
	       NULL);

  fx_vst3_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  g_object_get(note,
	       "y", &y,
	       NULL);
  
  midi_note = (y - audio_start_mapping + midi_start_mapping);

  if(midi_note >= 0 &&
     midi_note < 128){
    if(ags_fx_vst3_audio_test_flags(fx_vst3_audio, AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT)){
      AgsFxVst3AudioScopeData *scope_data;
      AgsFxVst3AudioChannelData *channel_data;

      g_rec_mutex_lock(fx_vst3_audio_mutex);

      scope_data = fx_vst3_audio->scope_data[sound_scope];

      channel_data = scope_data->channel_data[audio_channel];

      //TODO:JK: implement me
      
      g_rec_mutex_unlock(fx_vst3_audio_mutex);
    }else{
      AgsFxVst3AudioScopeData *scope_data;
      AgsFxVst3AudioChannelData *channel_data;
      AgsFxVst3AudioInputData *input_data;

      g_rec_mutex_lock(fx_vst3_audio_mutex);

      scope_data = fx_vst3_audio->scope_data[sound_scope];

      channel_data = scope_data->channel_data[audio_channel];

      input_data = channel_data->input_data[midi_note];

      //TODO:JK: implement me
      
      g_rec_mutex_unlock(fx_vst3_audio_mutex);
    }
  }
  
  /* call parent */
  AGS_FX_NOTATION_AUDIO_PROCESSOR_CLASS(ags_fx_vst3_audio_processor_parent_class)->key_on(fx_notation_audio_processor,
											  note,
											  velocity,
											  key_mode);
}

/**
 * ags_fx_vst3_audio_processor_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxVst3AudioProcessor
 *
 * Returns: the new #AgsFxVst3AudioProcessor
 *
 * Since: 3.10.5
 */
AgsFxVst3AudioProcessor*
ags_fx_vst3_audio_processor_new(AgsAudio *audio)
{
  AgsFxVst3AudioProcessor *fx_vst3_audio_processor;

  fx_vst3_audio_processor = (AgsFxVst3AudioProcessor *) g_object_new(AGS_TYPE_FX_VST3_AUDIO_PROCESSOR,
								     "audio", audio,
								     NULL);

  return(fx_vst3_audio_processor);
}
