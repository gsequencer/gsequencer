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
  AgsChannel *start_output, *output;
  AgsChannel *input;
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal;
  AgsRecallID *recall_id;
  AgsRecyclingContext *recycling_context;

  AgsFxBufferAudio *fx_buffer_audio;
  AgsFxBufferAudioProcessor *fx_buffer_audio_processor;
  AgsFxBufferChannelProcessor *fx_buffer_channel_processor;
  AgsFxBufferRecycling *fx_buffer_recycling;
  AgsFxBufferAudioSignal *fx_buffer_audio_signal;
  
  AgsSetBacklog *set_backlog;

  AgsFxBufferAudioScopeData *scope_data;

  GList *stream;
  GList *stream_destination, *stream_destination_next;

  gpointer buffer;
  
  gint sound_scope;
  guint audio_channel;
  guint buffer_size;
  guint samplerate;
  AgsSoundcardFormat format;
  guint attack;
  guint note_256th_attack;
  guint destination_attack;  
  guint destination_note_256th_attack;
  guint current_attack;
  guint copy_mode;
  gboolean resample;
  gboolean is_done;
  gboolean is_note_256th_format;

  GRecMutex *stream_mutex;
  GRecMutex *destination_stream_mutex;
  GRecMutex *backlog_mutex;
  
  GRecMutex *fx_buffer_channel_mutex;
  GRecMutex *scope_data_mutex;
  
  fx_buffer_audio_signal = AGS_FX_BUFFER_AUDIO_SIGNAL(recall);
  
  audio = NULL;
  
  start_output = NULL;

  output = NULL;
  
  recycling = NULL;  

  audio_signal = NULL;
  
  is_done = TRUE;

  stream_mutex = NULL;

  recall_id = NULL;

  recycling_context = NULL;

  fx_buffer_audio = NULL;
  fx_buffer_audio_processor = NULL;

  fx_buffer_channel_processor = NULL;

  fx_buffer_recycling = NULL;

  fx_buffer_audio_signal = NULL;
  
  set_backlog = ags_set_backlog_get_instance();

  sound_scope = ags_recall_get_sound_scope(recall);
  
  audio_channel = 0;
  
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  attack = 0;
  destination_attack = 0;

  note_256th_attack = 0;
  destination_note_256th_attack = 0;
  
  current_attack = 0;

  is_note_256th_format = TRUE;
  
  g_object_get(recall,
	       "source", &audio_signal,
	       "parent", &fx_buffer_recycling,
	       "recall-id", &recall_id,
	       NULL);

  if(audio_signal != NULL){
    stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(audio_signal);
    
    g_object_get(audio_signal,
		 "attack", &attack,
		 "note-256th-attack", &note_256th_attack,
		 "samplerate", &samplerate,
		 "buffer-size", &buffer_size,
		 "format", &format,
		 NULL);

    g_rec_mutex_lock(stream_mutex);
    
    is_done = (audio_signal == NULL || audio_signal->stream_current == NULL) ? TRUE: FALSE;

    g_rec_mutex_unlock(stream_mutex);
  }

  if(recall_id != NULL){
    g_object_get(recall_id,
		 "recycling-context", &recycling_context,
		 NULL);
  }
  
  if(fx_buffer_recycling != NULL){
    g_object_get(fx_buffer_recycling,
		 "source", &recycling,
		 "parent", &fx_buffer_channel_processor,
		 NULL);
  }

  if(fx_buffer_channel_processor != NULL){
    g_object_get(fx_buffer_channel_processor,
		 "source", &input,
		 "recall-audio", &fx_buffer_audio,
		 "recall-audio-run", &fx_buffer_audio_processor,
		 NULL);
  }

  if(fx_buffer_audio != NULL){
    g_object_get(fx_buffer_audio,
		 "audio", &audio,
		 NULL);
  }

  scope_data = fx_buffer_audio->scope_data[sound_scope];

  scope_data_mutex = AGS_FX_BUFFER_AUDIO_SCOPE_DATA_GET_STRCT_MUTEX(scope_data);
  
  if(audio != NULL){
    g_object_get(audio,
		 "output", &start_output,
		 NULL);
  }

  if(fx_buffer_audio_processor != NULL){
    g_object_get(fx_buffer_audio_processor,
		 "audio-channel", &audio_channel,
		 NULL);
  }
  
  if(sound_scope >= 0 &&
     sound_scope < AGS_SOUND_SCOPE_LAST){
    output = ags_channel_nth(start_output,
			     audio_channel);

    while(output != NULL){
      AgsChannel *next_pad;
      AgsRecycling *destination_recycling;
      AgsAudioSignal *destination_audio_signal;
      AgsRecallID *output_recall_id;

      GList *start_list, *list;

      guint destination_buffer_size;
      guint destination_samplerate;
      guint destination_format;
      
      list = 
	start_list = ags_channel_check_scope(output,
					     sound_scope);

      output_recall_id = ags_recall_id_find_recycling_context(start_list,
							      recycling_context->parent);
      
      if(output_recall_id != NULL){
	g_list_free_full(start_list,
			 (GDestroyNotify) g_object_unref);

	/* iterate */
	next_pad = ags_channel_next_pad(output);
	
	g_object_unref(output);
	
	output = next_pad;
	
	continue;
      }
      
      destination_recycling = NULL;
	
      destination_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
      destination_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
      destination_format = AGS_SOUNDCARD_DEFAULT_FORMAT;
	
      g_object_get(output,
		   "first-recycling", &destination_recycling,
		   "samplerate", &destination_samplerate,
		   "buffer-size", &destination_buffer_size,
		   "format", &destination_format,
		   NULL);

      destination_audio_signal = g_hash_table_lookup(scope_data->destination,
						     destination_recycling);

      if(destination_audio_signal == NULL){
	GObject *output_soundcard;

	output_soundcard = NULL;
	
	g_object_get(destination_recycling,
		     "output-soundcard", &output_soundcard,
		     NULL);

	/* create new audio signal */
	destination_audio_signal = ags_audio_signal_new((GObject *) output_soundcard,
							(GObject *) destination_recycling,
							(GObject *) output_recall_id);
	g_object_set(destination_audio_signal,
		     "samplerate", destination_samplerate,
		     "buffer-size", destination_buffer_size,
		     "format", destination_format,
		     NULL);
	
	ags_audio_signal_set_flags(destination_audio_signal,
				   AGS_AUDIO_SIGNAL_MASTER);
	ags_audio_signal_set_stream_mode(destination_audio_signal,
					 AGS_AUDIO_SIGNAL_STREAM_DUAL_BUFFERED);
	
	ags_audio_signal_stream_resize(destination_audio_signal,
				       2);

	destination_audio_signal->stream_current = destination_audio_signal->stream;

	ags_set_backlog_add(set_backlog,
			    destination_audio_signal);
	
#ifdef AGS_DEBUG
	g_message("ags-fx-buffer - create destination 0x%x", destination_audio_signal);
#endif

	g_object_ref(destination_recycling);
	g_object_ref(destination_audio_signal);
	  
	g_hash_table_insert(scope_data->destination,
			    destination_recycling,
			    destination_audio_signal);

	if(destination_samplerate != samplerate){
	  void *data;

	  guint allocated_buffer_length;

	  allocated_buffer_length = buffer_size;

	  if(allocated_buffer_length < destination_buffer_size){
	    allocated_buffer_length = destination_buffer_size;
	  }	  
	  
	  data = ags_stream_alloc(allocated_buffer_length,
				  format);
	  
	  g_object_ref(destination_recycling);	  
	  g_hash_table_insert(scope_data->resample_cache,
			      destination_recycling,
			      data);
	}

	ags_connectable_connect(AGS_CONNECTABLE(destination_audio_signal));  

	ags_recycling_add_audio_signal(destination_recycling,
				       destination_audio_signal);

	/* unref */
	if(output_soundcard != NULL){
	  g_object_unref(output_soundcard);
	}
      }

      /*  */
      if(destination_audio_signal != NULL &&
	 !is_done){
	/* get mutex */
	destination_stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(destination_audio_signal);

	backlog_mutex = AGS_AUDIO_SIGNAL_GET_BACKLOG_MUTEX(destination_audio_signal);
	
	/* audio signal */
	g_rec_mutex_lock(stream_mutex);
  
	stream = audio_signal->stream_current;

	g_rec_mutex_unlock(stream_mutex);
	
	/* destination audio signal */
	g_rec_mutex_lock(destination_stream_mutex);
  
	stream_destination = destination_audio_signal->stream_current;
	stream_destination_next = destination_audio_signal->stream_current->next;

	g_rec_mutex_unlock(destination_stream_mutex);

#ifdef AGS_DEBUG
	g_message("ags-fx-buffer 0x%x -> 0x%x", audio_signal, destination_audio_signal);
#endif
	
	copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(&(fx_buffer_audio_signal->audio_buffer_util),
								    ags_audio_buffer_util_format_from_soundcard(&(fx_buffer_audio_signal->audio_buffer_util),
														destination_format),
								    ags_audio_buffer_util_format_from_soundcard(&(fx_buffer_audio_signal->audio_buffer_util),
														format));
	resample = FALSE;

	if(samplerate != destination_samplerate){
	  resample = TRUE;
	}
	
	if(stream_destination->next == NULL){
	  ags_audio_signal_add_stream(destination_audio_signal);
	}

	g_rec_mutex_lock(stream_mutex);
    
	buffer = stream->data;

	g_rec_mutex_unlock(stream_mutex);

	/* just in time - resample */
	g_rec_mutex_lock(scope_data_mutex);
	  
	if(samplerate != destination_samplerate){
	  void *tmp_buffer;

	  guint allocated_buffer_length;

	  allocated_buffer_length = buffer_size;

	  if(allocated_buffer_length < destination_buffer_size){
	    allocated_buffer_length = destination_buffer_size;
	  }
	  
	  tmp_buffer = g_hash_table_lookup(scope_data->resample_cache,
						  recycling);

	  ags_audio_buffer_util_clear_buffer(&(fx_buffer_audio_signal->audio_buffer_util),
					     tmp_buffer, 1,
					     allocated_buffer_length, ags_audio_buffer_util_format_from_soundcard(&(fx_buffer_audio_signal->audio_buffer_util),
														  format));

	  g_rec_mutex_lock(stream_mutex);

	  ags_resample_util_init(&(fx_buffer_audio_signal->resample_util));
	  
	  ags_resample_util_set_format(&(fx_buffer_audio_signal->resample_util),
				       format);
	  ags_resample_util_set_buffer_length(&(fx_buffer_audio_signal->resample_util),
					      MAX(allocated_buffer_length, 4096));
	  ags_resample_util_set_samplerate(&(fx_buffer_audio_signal->resample_util),
					   samplerate);
	  ags_resample_util_set_target_samplerate(&(fx_buffer_audio_signal->resample_util),
						  destination_samplerate);

	  ags_resample_util_set_destination_stride(&(fx_buffer_audio_signal->resample_util),
						   1);
	  ags_resample_util_set_destination(&(fx_buffer_audio_signal->resample_util),
					    tmp_buffer);

	  ags_resample_util_set_source_stride(&(fx_buffer_audio_signal->resample_util),
					      1);
	  ags_resample_util_set_source(&(fx_buffer_audio_signal->resample_util),
				       buffer);

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

	  g_rec_mutex_unlock(stream_mutex);
      
	  buffer = tmp_buffer;
      
	  resample = TRUE;
	}

	g_rec_mutex_unlock(scope_data_mutex);
	
	if(ags_audio_signal_test_stream_mode(destination_audio_signal, AGS_AUDIO_SIGNAL_STREAM_DUAL_BUFFERED)){
	  /* backlog */
	  g_rec_mutex_lock(backlog_mutex);
	  
	  if(destination_audio_signal->has_backlog){
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
	    
	    destination_audio_signal->has_backlog = FALSE;
	  }
	  
	  g_rec_mutex_unlock(backlog_mutex);
	}else if(ags_audio_signal_test_stream_mode(destination_audio_signal, AGS_AUDIO_SIGNAL_STREAM_CONTINUES_FEED)){
	  //empty
	}

	if(destination_samplerate == samplerate){
	  destination_attack = attack;
	  destination_note_256th_attack = note_256th_attack;
	}else{
	  destination_attack = destination_samplerate * attack / samplerate;
	  destination_note_256th_attack = destination_samplerate * note_256th_attack / samplerate;
	}

	if(is_note_256th_format){
	  current_attack = destination_note_256th_attack;
	}else{
	  current_attack = destination_attack;
	}
	
	g_rec_mutex_lock(stream_mutex);
	g_rec_mutex_lock(destination_stream_mutex);

	if(current_attack < destination_buffer_size){
	  ags_audio_buffer_util_copy_buffer_to_buffer(&(fx_buffer_audio_signal->audio_buffer_util),
						      stream_destination->data, 1, current_attack,
						      buffer, 1, 0,
						      destination_buffer_size - current_attack, copy_mode);

	  if(current_attack > 0){
	    ags_audio_buffer_util_copy_buffer_to_buffer(&(fx_buffer_audio_signal->audio_buffer_util),
							stream_destination_next->data, 1, 0,
							buffer, 1, destination_buffer_size - current_attack,
							current_attack, copy_mode);
	  }
	}
	
	g_rec_mutex_unlock(destination_stream_mutex);
	g_rec_mutex_unlock(stream_mutex);
      }
	
      /*  */
      if(destination_recycling != NULL){
	g_object_unref(destination_recycling);
      }
      
      g_list_free_full(start_list,
		       (GDestroyNotify) g_object_unref);

      /* iterate */
      next_pad = ags_channel_next_pad(output);

      g_object_unref(output);

      output = next_pad;
    }
  }

  if(ags_audio_signal_test_flags(audio_signal,
				 AGS_AUDIO_SIGNAL_STREAM)){
    g_rec_mutex_lock(stream_mutex);

    if(audio_signal->stream_current != NULL){
      audio_signal->stream_current = audio_signal->stream_current->next;
    }

    g_rec_mutex_unlock(stream_mutex);
  }
    
  ags_recall_unset_flags(recall, AGS_RECALL_INITIAL_RUN);
  
  if(is_done){
    ags_recall_done(recall);

    ags_recycling_remove_audio_signal(recycling,
				      audio_signal);
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
  
  if(recycling != NULL){
    g_object_unref(recycling);
  }
  
  if(audio_signal != NULL){
    g_object_unref(audio_signal);
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
