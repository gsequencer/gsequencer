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

#include <ags/audio/fx/ags_fx_buffer_audio_signal.h>

#include <ags/audio/fx/ags_fx_buffer_audio.h>
#include <ags/audio/fx/ags_fx_buffer_audio_processor.h>
#include <ags/audio/fx/ags_fx_buffer_channel.h>
#include <ags/audio/fx/ags_fx_buffer_channel_processor.h>
#include <ags/audio/fx/ags_fx_buffer_recycling.h>

#include <ags/audio/task/ags_set_backlog.h>

#include <ags/i18n.h>

void ags_fx_buffer_audio_signal_class_init(AgsFxBufferAudioSignalClass *fx_buffer_audio_signal);
void ags_fx_buffer_audio_signal_init(AgsFxBufferAudioSignal *fx_buffer_audio_signal);
void ags_fx_buffer_audio_signal_dispose(GObject *gobject);
void ags_fx_buffer_audio_signal_finalize(GObject *gobject);

void ags_fx_buffer_audio_signal_real_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_buffer_audio_signal
 * @short_description: fx buffer audio signal
 * @title: AgsFxBufferAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_buffer_audio_signal.h
 *
 * The #AgsFxBufferAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_buffer_audio_signal_parent_class = NULL;

const gchar *ags_fx_buffer_audio_signal_plugin_name = "ags-fx-buffer";

GType
ags_fx_buffer_audio_signal_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_buffer_audio_signal = 0;

    static const GTypeInfo ags_fx_buffer_audio_signal_info = {
      sizeof (AgsFxBufferAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_buffer_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxBufferAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_buffer_audio_signal_init,
    };

    ags_type_fx_buffer_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							     "AgsFxBufferAudioSignal",
							     &ags_fx_buffer_audio_signal_info,
							     0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_buffer_audio_signal);
  }

  return(g_define_type_id__static);
}

void
ags_fx_buffer_audio_signal_class_init(AgsFxBufferAudioSignalClass *fx_buffer_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_buffer_audio_signal_parent_class = g_type_class_peek_parent(fx_buffer_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_buffer_audio_signal;

  gobject->dispose = ags_fx_buffer_audio_signal_dispose;
  gobject->finalize = ags_fx_buffer_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_buffer_audio_signal;
  
  recall->run_inter = ags_fx_buffer_audio_signal_real_run_inter;
}

void
ags_fx_buffer_audio_signal_init(AgsFxBufferAudioSignal *fx_buffer_audio_signal)
{
  AGS_RECALL(fx_buffer_audio_signal)->name = "ags-fx-buffer";
  AGS_RECALL(fx_buffer_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_buffer_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_buffer_audio_signal)->xml_type = "ags-fx-buffer-audio-signal";
}

void
ags_fx_buffer_audio_signal_dispose(GObject *gobject)
{
  AgsFxBufferAudioSignal *fx_buffer_audio_signal;
  
  fx_buffer_audio_signal = AGS_FX_BUFFER_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_buffer_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_buffer_audio_signal_finalize(GObject *gobject)
{
  AgsFxBufferAudioSignal *fx_buffer_audio_signal;
  
  fx_buffer_audio_signal = AGS_FX_BUFFER_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_buffer_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_buffer_audio_signal_real_run_inter(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsChannel *destination_channel;
  AgsChannel *input;
  AgsChannel *start_output, *output;
  AgsRecycling *source_recycling;
  AgsAudioSignal *source;

  AgsFxBufferAudio *fx_buffer_audio;
  AgsFxBufferAudioProcessor *fx_buffer_audio_processor;
  AgsFxBufferChannel *fx_buffer_channel;
  AgsFxBufferChannelProcessor *fx_buffer_channel_processor;
  AgsFxBufferRecycling *fx_buffer_recycling;
  AgsFxBufferAudioSignal *fx_buffer_audio_signal;

  AgsFxBufferAudioScopeData *scope_data;
  AgsFxBufferChannelInputData *input_data;

  AgsSetBacklog *set_backlog;
  
  GList *stream_destination, *stream_destination_next;
  GList *stream_source;

  void *buffer_destination, *buffer_destination_next;
  void *buffer_source;

  gint sound_scope;
  guint audio_channel;
  guint output_line;

  guint destination_buffer_size, source_buffer_size;
  guint destination_samplerate, source_samplerate;
  guint destination_format, source_format;

  guint attack;
  guint note_256th_attack;
  guint destination_attack;  
  guint destination_note_256th_attack;
  guint current_attack;  
  gboolean is_note_256th_format;
  
  guint copy_mode;
  gboolean resample;

  gboolean is_done;
  
  GRecMutex *source_stream_mutex;
  GRecMutex *destination_stream_mutex;
  GRecMutex *backlog_mutex;
  
  GRecMutex *fx_buffer_channel_mutex;
  GRecMutex *scope_data_mutex;
  GRecMutex *input_data_mutex;

  audio = NULL;
  
  output = NULL;
  input = NULL;  

  source_recycling = NULL;

  source = NULL;

  fx_buffer_audio = NULL;
  fx_buffer_audio_processor = NULL;

  fx_buffer_channel = NULL;
  fx_buffer_channel_processor = NULL;

  fx_buffer_recycling = NULL;

  fx_buffer_audio_signal = NULL;

  set_backlog = ags_set_backlog_get_instance();

  input_data = NULL;
  
  fx_buffer_audio_signal = AGS_FX_BUFFER_AUDIO_SIGNAL(recall);
  
  sound_scope = ags_recall_get_sound_scope(recall);

  audio_channel = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  destination_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  destination_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  destination_format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  attack = 0;
  destination_attack = 0;

  note_256th_attack = 0;
  destination_note_256th_attack = 0;
  
  current_attack = 0;

  is_note_256th_format = TRUE;
  
  g_object_get(recall,
	       "source", &source,
	       "parent", &fx_buffer_recycling,
	       NULL);

  source_stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);

  g_rec_mutex_lock(source_stream_mutex);
    
  is_done = (source == NULL || source->stream_current == NULL) ? TRUE: FALSE;

  g_rec_mutex_unlock(source_stream_mutex);

  if(source != NULL){
    g_object_get(source,
		 "attack", &attack,
		 "note-256th-attack", &note_256th_attack,
		 "samplerate", &source_samplerate,
		 "buffer-size", &source_buffer_size,
		 "format", &source_format,
		 NULL);
  }

  if(fx_buffer_recycling != NULL){
    g_object_get(fx_buffer_recycling,
		 "source", &source_recycling,
		 "parent", &fx_buffer_channel_processor,
		 NULL);
  }

  if(fx_buffer_channel_processor != NULL){
    g_object_get(fx_buffer_channel_processor,
		 "source", &input,
		 "recall-audio", &fx_buffer_audio,
		 "recall-audio-run", &fx_buffer_audio_processor,
		 "recall-channel", &fx_buffer_channel,
		 NULL);
  }

  if(fx_buffer_audio != NULL){
    g_object_get(fx_buffer_audio,
		 "audio", &audio,
		 NULL);
  }

  start_output = ags_audio_get_output(audio);
  
  if(fx_buffer_audio_processor != NULL){
    g_object_get(fx_buffer_audio_processor,
		 "audio-channel", &audio_channel,
		 NULL);
  }

  scope_data = fx_buffer_audio->scope_data[sound_scope];

  input_data = ags_fx_buffer_channel_get_input_data(fx_buffer_channel,
						    sound_scope);

  if(ags_recall_test_state_flags((AgsRecall *) fx_buffer_channel_processor,
				 AGS_SOUND_STATE_RUN_COMPLETED)){
    goto fx_buffer_audio_signal_run_inter_END;

    return;
  }

  if(sound_scope >= 0 &&
     sound_scope < AGS_SOUND_SCOPE_LAST &&
     fx_buffer_channel != NULL &&
     scope_data != NULL &&
     input_data != NULL){
    /* instantiate destination */
    fx_buffer_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_buffer_channel);

    output = ags_channel_nth(start_output,
			     audio_channel);
    
    scope_data_mutex = AGS_FX_BUFFER_AUDIO_SCOPE_DATA_GET_STRCT_MUTEX(scope_data);
    input_data_mutex = AGS_FX_BUFFER_CHANNEL_INPUT_DATA_GET_STRCT_MUTEX(input_data);
    
    while(output != NULL){
      AgsChannel *next_pad;
      AgsRecycling *recycling;
      AgsAudioSignal *destination;
      
      recycling = NULL;

      destination_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
      destination_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
      destination_format = AGS_SOUNDCARD_DEFAULT_FORMAT;
      
      g_object_get(output,
		   "first-recycling", &recycling,
		   "samplerate", &destination_samplerate,
		   "buffer-size", &destination_buffer_size,
		   "format", &destination_format,
		   NULL);

      g_rec_mutex_lock(scope_data_mutex);
      
      destination = g_hash_table_lookup(scope_data->destination,
					recycling);
      
      g_rec_mutex_unlock(scope_data_mutex);
      
      if(destination == NULL){
	AgsRecallID *parent_recall_id;
	
	GObject *output_soundcard;

	GList *start_list;
	
	output_soundcard = NULL;

	parent_recall_id = NULL;

	start_list = ags_channel_check_scope(output, sound_scope);
	
	g_object_get(recycling,
		     "output-soundcard", &output_soundcard,
		     NULL);

	/* get parent recall id */
	if(start_list != NULL){
	  parent_recall_id = start_list->data;

	  g_object_ref(parent_recall_id);
	}
    
	g_list_free_full(start_list,
			 g_object_unref);

	if(ags_recycling_context_find_child(parent_recall_id->recycling_context, source_recycling) < 0){
	  goto ags_fx_buffer_audio_signal_run_inter_NEXT_OUTPUT;
	}
	
	/* create new audio signal */
	destination = ags_audio_signal_new((GObject *) output_soundcard,
					   (GObject *) recycling,
					   (GObject *) parent_recall_id);
	g_object_set(destination,
		     "samplerate", destination_samplerate,
		     "buffer-size", destination_buffer_size,
		     "format", destination_format,
		     NULL);
	
	ags_audio_signal_set_flags(destination, AGS_AUDIO_SIGNAL_MASTER);
	ags_audio_signal_set_stream_mode(destination,
					 AGS_AUDIO_SIGNAL_STREAM_DUAL_BUFFERED);
	
	ags_audio_signal_stream_resize(destination,
				       2);

	destination->stream_current = destination->stream;

	ags_set_backlog_add(set_backlog,
			    destination);
	
#if 0 // AGS_DEBUG
	g_message("ags-fx-buffer - create destination audio signal: 0x%x", destination);
#endif
	g_rec_mutex_lock(scope_data_mutex);

	g_object_ref(recycling);
	g_object_ref(destination);
	g_hash_table_insert(scope_data->destination,
			    recycling,
			    destination);

	g_rec_mutex_unlock(scope_data_mutex);

	g_rec_mutex_lock(input_data_mutex);
	
	if(destination_samplerate != source_samplerate){
	  void *data;

	  guint allocated_buffer_length;

	  allocated_buffer_length = source_buffer_size;

	  if(allocated_buffer_length < destination_buffer_size){
	    allocated_buffer_length = destination_buffer_size;
	  }	  
	  
	  data = ags_stream_alloc(allocated_buffer_length,
				  source_format);
	  
	  g_object_ref(recycling);	  
	  g_hash_table_insert(input_data->resample_cache,
			      recycling,
			      data);
	}
	
	g_rec_mutex_unlock(input_data_mutex);

	ags_connectable_connect(AGS_CONNECTABLE(destination));  

	ags_recycling_add_audio_signal(recycling,
				       destination);

	/* unref */
      ags_fx_buffer_audio_signal_run_inter_NEXT_OUTPUT:
	
	if(output_soundcard != NULL){
	  g_object_unref(output_soundcard);
	}

	if(parent_recall_id != NULL){
	  g_object_unref(parent_recall_id);
	}
      }

      if(!is_done &&
	 destination != NULL){
	/* get mutex */
	destination_stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(destination);

	backlog_mutex = AGS_AUDIO_SIGNAL_GET_BACKLOG_MUTEX(destination);
	
	/* source */
	g_rec_mutex_lock(source_stream_mutex);
  
	stream_source = source->stream_current;

	g_rec_mutex_unlock(source_stream_mutex);

	/* destination */
	g_rec_mutex_lock(destination_stream_mutex);
  
	stream_destination = destination->stream_current;
	stream_destination_next = destination->stream_current->next;

	g_rec_mutex_unlock(destination_stream_mutex);

#ifdef AGS_DEBUG
	g_message("ags-fx-buffer 0x%x -> 0x%x", source, destination);
#endif
	
	copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(&(fx_buffer_audio_signal->audio_buffer_util),
								    ags_audio_buffer_util_format_from_soundcard(&(fx_buffer_audio_signal->audio_buffer_util),
														destination_format),
								    ags_audio_buffer_util_format_from_soundcard(&(fx_buffer_audio_signal->audio_buffer_util),
														source_format));
	resample = FALSE;

	if(source_samplerate != destination_samplerate){
	  resample = TRUE;
	}
	
	if(stream_destination->next == NULL){
	  ags_audio_signal_add_stream(destination);
	}

	g_rec_mutex_lock(source_stream_mutex);
    
	buffer_source = stream_source->data;

	g_rec_mutex_unlock(source_stream_mutex);

	/* just in time - resample */
	g_rec_mutex_lock(input_data_mutex);
	  
	if(source_samplerate != destination_samplerate){
	  void *tmp_buffer_source;

	  guint allocated_buffer_length;

	  allocated_buffer_length = source_buffer_size;

	  if(allocated_buffer_length < destination_buffer_size){
	    allocated_buffer_length = destination_buffer_size;
	  }
	  
	  tmp_buffer_source = g_hash_table_lookup(input_data->resample_cache,
						  recycling);

	  ags_audio_buffer_util_clear_buffer(&(fx_buffer_audio_signal->audio_buffer_util),
					     tmp_buffer_source, 1,
					     allocated_buffer_length, ags_audio_buffer_util_format_from_soundcard(&(fx_buffer_audio_signal->audio_buffer_util),
														  source_format));

	  g_rec_mutex_lock(source_stream_mutex);

	  ags_resample_util_init(&(fx_buffer_audio_signal->resample_util));
	  
	  ags_resample_util_set_format(&(fx_buffer_audio_signal->resample_util),
				       source_format);
	  ags_resample_util_set_buffer_length(&(fx_buffer_audio_signal->resample_util),
					      MAX(allocated_buffer_length, 4096));
	  ags_resample_util_set_samplerate(&(fx_buffer_audio_signal->resample_util),
					   source_samplerate);
	  ags_resample_util_set_target_samplerate(&(fx_buffer_audio_signal->resample_util),
						  destination_samplerate);

	  ags_resample_util_set_destination_stride(&(fx_buffer_audio_signal->resample_util),
						   1);
	  ags_resample_util_set_destination(&(fx_buffer_audio_signal->resample_util),
					    tmp_buffer_source);

	  ags_resample_util_set_source_stride(&(fx_buffer_audio_signal->resample_util),
					      1);
	  ags_resample_util_set_source(&(fx_buffer_audio_signal->resample_util),
				       buffer_source);

	  fx_buffer_audio_signal->resample_util.bypass_cache = TRUE;

	  ags_resample_util_compute(&(fx_buffer_audio_signal->resample_util));  

	  fx_buffer_audio_signal->resample_util.destination = NULL;
		  
	  fx_buffer_audio_signal->resample_util.source = NULL;

	  if(fx_buffer_audio_signal->resample_util.data_in != NULL){
	    ags_stream_free(fx_buffer_audio_signal->resample_util.data_in);
	
	    fx_buffer_audio_signal->resample_util.data_in = NULL;
	  }

	  if(fx_buffer_audio_signal->resample_util.data_out != NULL){
	    ags_stream_free(fx_buffer_audio_signal->resample_util.data_out);

	    fx_buffer_audio_signal->resample_util.data_out = NULL;
	  }

	  if(fx_buffer_audio_signal->resample_util.buffer != NULL){
	    ags_stream_free(fx_buffer_audio_signal->resample_util.buffer);

	    fx_buffer_audio_signal->resample_util.buffer = NULL;
	  }

	  g_rec_mutex_unlock(source_stream_mutex);
      
	  buffer_source = tmp_buffer_source;
      
	  resample = TRUE;
	}

	g_rec_mutex_unlock(input_data_mutex);
	
	if(ags_audio_signal_test_stream_mode(destination, AGS_AUDIO_SIGNAL_STREAM_DUAL_BUFFERED)){
	  /* backlog */
	  g_rec_mutex_lock(backlog_mutex);
	  
	  if(destination->has_backlog){
	    guint backlog_copy_mode;
	    
	    backlog_copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(&(fx_buffer_audio_signal->audio_buffer_util),
										ags_audio_buffer_util_format_from_soundcard(&(fx_buffer_audio_signal->audio_buffer_util),
															    destination_format),
										ags_audio_buffer_util_format_from_soundcard(&(fx_buffer_audio_signal->audio_buffer_util),
															    destination_format));

	    g_rec_mutex_lock(destination_stream_mutex);

	    ags_audio_buffer_util_clear_buffer(&(fx_buffer_audio_signal->audio_buffer_util),
					       stream_destination->data, 1,
					       destination_buffer_size, ags_audio_buffer_util_format_from_soundcard(&(fx_buffer_audio_signal->audio_buffer_util),
														    destination_format));
	    
	    ags_audio_buffer_util_copy_buffer_to_buffer(&(fx_buffer_audio_signal->audio_buffer_util),
							stream_destination->data, 1, 0,
							stream_destination_next->data, 1, 0,
							destination_buffer_size, backlog_copy_mode);

	    ags_audio_buffer_util_clear_buffer(&(fx_buffer_audio_signal->audio_buffer_util),
					       stream_destination_next->data, 1,
					       destination_buffer_size, ags_audio_buffer_util_format_from_soundcard(&(fx_buffer_audio_signal->audio_buffer_util),
														    destination_format));

	    g_rec_mutex_unlock(destination_stream_mutex);
	    
	    destination->has_backlog = FALSE;
	  }
	  
	  g_rec_mutex_unlock(backlog_mutex);
	}else if(ags_audio_signal_test_stream_mode(destination, AGS_AUDIO_SIGNAL_STREAM_CONTINUES_FEED)){
	  //empty
	}

	if(destination_samplerate == source_samplerate){
	  destination_attack = attack;
	  destination_note_256th_attack = note_256th_attack;
	}else{
	  destination_attack = destination_samplerate * attack / source_samplerate;
	  destination_note_256th_attack = destination_samplerate * note_256th_attack / source_samplerate;
	}

	if(is_note_256th_format){
	  current_attack = destination_note_256th_attack;
	}else{
	  current_attack = destination_attack;
	}
	
	g_rec_mutex_lock(source_stream_mutex);
	g_rec_mutex_lock(destination_stream_mutex);

	if(current_attack < destination_buffer_size){
	  ags_audio_buffer_util_copy_buffer_to_buffer(&(fx_buffer_audio_signal->audio_buffer_util),
						      stream_destination->data, 1, current_attack,
						      buffer_source, 1, 0,
						      destination_buffer_size - current_attack, copy_mode);

	  if(current_attack > 0){
	    ags_audio_buffer_util_copy_buffer_to_buffer(&(fx_buffer_audio_signal->audio_buffer_util),
							stream_destination_next->data, 1, 0,
							buffer_source, 1, destination_buffer_size - current_attack,
							current_attack, copy_mode);
	  }
	}
	
	g_rec_mutex_unlock(destination_stream_mutex);
	g_rec_mutex_unlock(source_stream_mutex);
      }
      
      /* iterate */
      next_pad = ags_channel_next_pad(output);

      g_object_unref(output);

      output = next_pad;
    }
  }
  
  if(ags_audio_signal_test_flags(source, AGS_AUDIO_SIGNAL_STREAM)){
    g_rec_mutex_lock(source_stream_mutex);

    if(source->stream_current != NULL){
      source->stream_current = source->stream_current->next;
    }

    g_rec_mutex_unlock(source_stream_mutex);
  }
    
  ags_recall_unset_flags(recall, AGS_RECALL_INITIAL_RUN);
  
  if(is_done){
    ags_recall_done(recall);

    ags_recycling_remove_audio_signal(source_recycling,
				      source);
  }

 fx_buffer_audio_signal_run_inter_END:

  /* unref */
  if(audio != NULL){
    g_object_unref(audio);
  }

  if(input != NULL){
    g_object_unref(input);
  }
  
  if(start_output != NULL){
    g_object_unref(start_output);
  }
  
  if(source_recycling != NULL){
    g_object_unref(source_recycling);
  }
  
  if(source != NULL){
    g_object_unref(source);
  }

  if(fx_buffer_audio != NULL){
    g_object_unref(fx_buffer_audio);
  }

  if(fx_buffer_audio_processor != NULL){
    g_object_unref(fx_buffer_audio_processor);
  }
  
  if(fx_buffer_channel_processor != NULL){
    g_object_unref(fx_buffer_channel_processor);
  }
  
  if(fx_buffer_recycling != NULL){
    g_object_unref(fx_buffer_recycling);
  }  
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_buffer_audio_signal_parent_class)->run_inter(recall);
}

/**
 * ags_fx_buffer_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxBufferAudioSignal
 *
 * Returns: the new #AgsFxBufferAudioSignal
 *
 * Since: 3.3.0
 */
AgsFxBufferAudioSignal*
ags_fx_buffer_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxBufferAudioSignal *fx_buffer_audio_signal;

  fx_buffer_audio_signal = (AgsFxBufferAudioSignal *) g_object_new(AGS_TYPE_FX_BUFFER_AUDIO_SIGNAL,
								   "source", audio_signal,
								   NULL);

  return(fx_buffer_audio_signal);
}
