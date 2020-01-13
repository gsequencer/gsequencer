/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/thread/ags_audio_thread.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_channel_thread.h>

#include <math.h>

#include <ags/i18n.h>

void ags_audio_thread_class_init(AgsAudioThreadClass *audio_thread);
void ags_audio_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_thread_init(AgsAudioThread *audio_thread);
void ags_audio_thread_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_audio_thread_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_audio_thread_dispose(GObject *gobject);
void ags_audio_thread_finalize(GObject *gobject);

void ags_audio_thread_start(AgsThread *thread);
void ags_audio_thread_run(AgsThread *thread);
void ags_audio_thread_stop(AgsThread *thread);

void ags_audio_thread_play_channel_super_threaded(AgsAudioThread *audio_thread, AgsPlayback *playback);
void ags_audio_thread_sync_channel_super_threaded(AgsAudioThread *audio_thread, AgsPlayback *playback);

/**
 * SECTION:ags_audio_thread
 * @short_description: audio thread
 * @title: AgsAudioThread
 * @section_id:
 * @include: ags/audio/thread/ags_audio_thread.h
 *
 * The #AgsAudioThread acts as audio output thread to soundcard.
 */

enum{
  PROP_0,
  PROP_DEFAULT_OUTPUT_SOUNDCARD,
  PROP_AUDIO,
  PROP_SOUND_SCOPE,
};

static gpointer ags_audio_thread_parent_class = NULL;
static AgsConnectableInterface *ags_audio_thread_parent_connectable_interface;

GType
ags_audio_thread_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audio_thread = 0;

    static const GTypeInfo ags_audio_thread_info = {
      sizeof (AgsAudioThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_thread = g_type_register_static(AGS_TYPE_THREAD,
						   "AgsAudioThread",
						   &ags_audio_thread_info,
						   0);
    
    g_type_add_interface_static(ags_type_audio_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audio_thread);
  }

  return g_define_type_id__volatile;
}

void
ags_audio_thread_class_init(AgsAudioThreadClass *audio_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  GParamSpec *param_spec;
  
  ags_audio_thread_parent_class = g_type_class_peek_parent(audio_thread);

  /* GObject */
  gobject = (GObjectClass *) audio_thread;
    
  gobject->set_property = ags_audio_thread_set_property;
  gobject->get_property = ags_audio_thread_get_property;

  gobject->dispose = ags_audio_thread_dispose;
  gobject->finalize = ags_audio_thread_finalize;

  /* properties */
  /**
   * AgsAudioThread:default-output-soundcard:
   *
   * The assigned default soundcard.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("default-output-soundcard",
				   i18n_pspec("default output soundcard assigned to"),
				   i18n_pspec("The default output AgsSoundcard it is assigned to"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEFAULT_OUTPUT_SOUNDCARD,
				  param_spec);

  /**
   * AgsAudioThread:audio:
   *
   * The assigned #AgsAudio.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio assigned to"),
				   i18n_pspec("The AgsAudio it is assigned to"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /* AgsThread */
  thread = (AgsThreadClass *) audio_thread;

  thread->start = ags_audio_thread_start;
  thread->run = ags_audio_thread_run;
  thread->stop = ags_audio_thread_stop;
}

void
ags_audio_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_audio_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_audio_thread_init(AgsAudioThread *audio_thread)
{
  AgsThread *thread;

  AgsConfig *config;

  gdouble frequency;
  guint samplerate;
  guint buffer_size;
  
  thread = (AgsThread *) audio_thread;

  ags_thread_set_flags(thread, (AGS_THREAD_START_SYNCED_FREQ |
				AGS_THREAD_IMMEDIATE_SYNC));
  
  config = ags_config_get_instance();

  samplerate = ags_soundcard_helper_config_get_samplerate(config);
  buffer_size = ags_soundcard_helper_config_get_buffer_size(config);

  frequency = ((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

  g_object_set(thread,
	       "frequency", frequency,
	       NULL);

  g_atomic_int_set(&(audio_thread->status_flags),
		   0);
  
  audio_thread->default_output_soundcard = NULL;
  
  /* start */
  g_mutex_init(&(audio_thread->wakeup_mutex));
  
  g_cond_init(&(audio_thread->wakeup_cond));

  /* sync */
  g_mutex_init(&(audio_thread->done_mutex));
  
  g_cond_init(&(audio_thread->done_cond));

  /* audio and sound scope */
  audio_thread->audio = NULL;
  audio_thread->sound_scope = -1;

  audio_thread->sync_thread = NULL;
}

void
ags_audio_thread_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsAudioThread *audio_thread;

  GRecMutex *thread_mutex;

  audio_thread = AGS_AUDIO_THREAD(gobject);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(audio_thread);

  switch(prop_id){
  case PROP_DEFAULT_OUTPUT_SOUNDCARD:
    {
      GObject *default_output_soundcard;

      default_output_soundcard = g_value_get_object(value);

      g_rec_mutex_lock(thread_mutex);

      if(audio_thread->default_output_soundcard == default_output_soundcard){
	g_rec_mutex_unlock(thread_mutex);

	return;
      }

      if(audio_thread->default_output_soundcard != NULL){
	g_object_unref(audio_thread->default_output_soundcard);
      }

      if(default_output_soundcard != NULL){
	g_object_ref(default_output_soundcard);
      }

      audio_thread->default_output_soundcard = default_output_soundcard;

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      g_rec_mutex_lock(thread_mutex);

      if(audio_thread->audio == (GObject *) audio){
	g_rec_mutex_unlock(thread_mutex);
	
	return;
      }
      
      if(audio_thread->audio != NULL){
	g_object_unref(G_OBJECT(audio_thread->audio));
      }

      if(audio != NULL){
	g_object_ref(G_OBJECT(audio));
      }

      audio_thread->audio = (GObject *) audio;

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_thread_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsAudioThread *audio_thread;

  GRecMutex *thread_mutex;

  audio_thread = AGS_AUDIO_THREAD(gobject);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(audio_thread);

  switch(prop_id){
  case PROP_DEFAULT_OUTPUT_SOUNDCARD:
    {
      g_rec_mutex_lock(thread_mutex);

      g_value_set_object(value, audio_thread->default_output_soundcard);

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_AUDIO:
    {
      g_rec_mutex_lock(thread_mutex);

      g_value_set_object(value, G_OBJECT(audio_thread->audio));

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_thread_dispose(GObject *gobject)
{
  AgsAudioThread *audio_thread;

  audio_thread = AGS_AUDIO_THREAD(gobject);

  /* soundcard */
  if(audio_thread->default_output_soundcard != NULL){
    g_object_unref(audio_thread->default_output_soundcard);

    audio_thread->default_output_soundcard = NULL;
  }

  /* audio */
  if(audio_thread->audio != NULL){
    g_object_unref(audio_thread->audio);

    audio_thread->audio = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_audio_thread_parent_class)->dispose(gobject);
}

void
ags_audio_thread_finalize(GObject *gobject)
{
  AgsAudioThread *audio_thread;

  audio_thread = AGS_AUDIO_THREAD(gobject);

  /* soundcard */
  if(audio_thread->default_output_soundcard != NULL){
    g_object_unref(audio_thread->default_output_soundcard);
  }

  /* audio */
  if(audio_thread->audio != NULL){
    g_object_unref(audio_thread->audio);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_thread_parent_class)->finalize(gobject);
}

void
ags_audio_thread_start(AgsThread *thread)
{
#ifdef AGS_DEBUG
  g_message("audio thread start");
#endif
  
  /* reset status */
  ags_audio_thread_set_status_flags(thread, (AGS_AUDIO_THREAD_STATUS_WAIT |
					     AGS_AUDIO_THREAD_STATUS_DONE |
					     AGS_AUDIO_THREAD_STATUS_WAIT_SYNC |
					     AGS_AUDIO_THREAD_STATUS_DONE_SYNC));
  
  AGS_THREAD_CLASS(ags_audio_thread_parent_class)->start(thread);
}

void
ags_audio_thread_run(AgsThread *thread)
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsPlaybackDomain *playback_domain;

  AgsAudioLoop *audio_loop;
  AgsAudioThread *audio_thread;

  GList *output_playback_start, *input_playback_start, *playback;
  GList *recall_id;
  
  gint sound_scope;

  GRecMutex *thread_mutex;

//  g_message("do: audio %f", thread->tic_delay);
  
  /* real-time setup */
#ifdef AGS_WITH_RT
  if(!ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RT_SETUP)){
    AgsPriority *priority;
    
    struct sched_param param;

    gchar *str;

    priority = ags_priority_get_instance();
    
    /* Declare ourself as a real time task */
    param.sched_priority = 1;
      
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
      perror("sched_setscheduler failed");
    }

    str = ags_priority_get_value(priority,
				 AGS_PRIORITY_RT_THREAD,
				 AGS_PRIORITY_KEY_AUDIO);

    if(str != NULL){
      param.sched_priority = (int) g_ascii_strtoull(str,
						    NULL,
						    10);

      g_free(str);
    }

    ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_RT_SETUP);
  }
#endif

  audio_loop = thread->parent;

  if(!ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_SYNCED)){
    return;
  }
  
  audio_thread = AGS_AUDIO_THREAD(thread);  

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);
  
  g_object_get(audio_thread,
	       "audio", &audio,
	       NULL);

  g_object_get(audio,
	       "playback-domain", &playback_domain,
	        NULL);
  
  /* start - wait until signaled */
  g_rec_mutex_lock(thread_mutex);

  sound_scope = audio_thread->sound_scope;
  
  g_rec_mutex_unlock(thread_mutex);

  if(sound_scope != AGS_SOUND_SCOPE_PLAYBACK){
    g_mutex_lock(&(audio_thread->wakeup_mutex));
        
    if(ags_audio_thread_test_status_flags(audio_thread, AGS_AUDIO_THREAD_STATUS_WAIT)){
      ags_audio_thread_unset_status_flags(audio_thread, AGS_AUDIO_THREAD_STATUS_DONE);
    
      while(ags_audio_thread_test_status_flags(audio_thread, AGS_AUDIO_THREAD_STATUS_WAIT) &&
	    !ags_audio_thread_test_status_flags(audio_thread, AGS_AUDIO_THREAD_STATUS_DONE)){
	g_cond_wait(&(audio_thread->wakeup_cond),
		    &(audio_thread->wakeup_mutex));
      }
    }
    
    ags_audio_thread_set_status_flags(audio_thread, (AGS_AUDIO_THREAD_STATUS_WAIT |
						     AGS_AUDIO_THREAD_STATUS_DONE));
    
    g_mutex_unlock(&(audio_thread->wakeup_mutex));
  }
  
  /* emit tact callback */
  //  ags_audio_tact(audio);
  //  g_message("audio thread run");  
  /* 
   * do audio processing
   */
//  g_message("audio thread");
  
  /* input */
  g_rec_mutex_lock(thread_mutex);

  sound_scope = audio_thread->sound_scope;
  
  g_rec_mutex_unlock(thread_mutex);

  g_object_get(playback_domain,
	       "input-playback", &input_playback_start,
	       NULL);

  playback = input_playback_start;
  
  while(playback != NULL){
    if(ags_playback_test_flags(playback->data, AGS_PLAYBACK_SUPER_THREADED_CHANNEL)){
      ags_audio_thread_play_channel_super_threaded(audio_thread, playback->data);
    }else{
      g_object_get(playback->data,
		   "channel", &channel,
		   NULL);

      g_object_unref(channel);
      
      if(sound_scope >= 0){
	if(ags_playback_get_recall_id((AgsPlayback *) playback, sound_scope) == NULL){
	  playback = playback->next;
	  
	  continue;
	}
	
	if((recall_id = ags_channel_check_scope(channel, sound_scope)) != NULL){
#if 1
	  ags_channel_recursive_run_stage(channel,
					  sound_scope, (AGS_SOUND_STAGING_FEED_INPUT_QUEUE |
							AGS_SOUND_STAGING_AUTOMATE |
							AGS_SOUND_STAGING_RUN_PRE));

	  ags_channel_recursive_run_stage(channel,
					  sound_scope, (AGS_SOUND_STAGING_RUN_INTER));

	  ags_channel_recursive_run_stage(channel,
					  sound_scope, (AGS_SOUND_STAGING_RUN_POST |
							AGS_SOUND_STAGING_DO_FEEDBACK |
							AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE));
#else
	  ags_channel_recursive_run_stage(channel,
					  sound_scope, (AGS_SOUND_STAGING_AUTOMATE |
							AGS_SOUND_STAGING_RUN_PRE |
							AGS_SOUND_STAGING_RUN_INTER |
							AGS_SOUND_STAGING_RUN_POST));
#endif
	  
	  g_list_free_full(recall_id,
			   g_object_unref);
	}
      }else{
	for(sound_scope = 0; sound_scope < AGS_SOUND_SCOPE_LAST; sound_scope++){
	  if(ags_playback_get_recall_id((AgsPlayback *) playback, sound_scope) == NULL){
	    continue;
	  }
	  
	  if((recall_id = ags_channel_check_scope(channel, sound_scope)) != NULL){
#if 1
	    ags_channel_recursive_run_stage(channel,
					    sound_scope, (AGS_SOUND_STAGING_FEED_INPUT_QUEUE |
							  AGS_SOUND_STAGING_AUTOMATE |
							  AGS_SOUND_STAGING_RUN_PRE));

	    ags_channel_recursive_run_stage(channel,
					    sound_scope, (AGS_SOUND_STAGING_RUN_INTER));

	    ags_channel_recursive_run_stage(channel,
					    sound_scope, (AGS_SOUND_STAGING_RUN_POST |
							  AGS_SOUND_STAGING_DO_FEEDBACK |
							  AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE));
#else
	    ags_channel_recursive_run_stage(channel,
					    sound_scope, (AGS_SOUND_STAGING_AUTOMATE |
							  AGS_SOUND_STAGING_RUN_PRE |
							  AGS_SOUND_STAGING_RUN_INTER |
							  AGS_SOUND_STAGING_RUN_POST));
#endif
	  
	    g_list_free_full(recall_id,
			     g_object_unref);
	  }
	}
      }
    }
    
    playback = playback->next;
  }
  
  /* output */
  g_rec_mutex_lock(thread_mutex);

  sound_scope = audio_thread->sound_scope;
  
  g_rec_mutex_unlock(thread_mutex);

  g_object_get(playback_domain,
	       "output-playback", &output_playback_start,
	       NULL);

  playback = output_playback_start;
  
  while(playback != NULL){    
    if(ags_playback_test_flags(playback->data, AGS_PLAYBACK_SUPER_THREADED_CHANNEL)){
      ags_audio_thread_play_channel_super_threaded(audio_thread, playback->data);
    }else{      
      g_object_get(playback->data,
		   "channel", &channel,
		   NULL);

      g_object_unref(channel);
      
      if(sound_scope >= 0){
	if(ags_playback_get_recall_id((AgsPlayback *) playback->data, sound_scope) == NULL){
	  playback = playback->next;
	  
	  continue;
	}

	if((recall_id = ags_channel_check_scope(channel, sound_scope)) != NULL){
#if 1
	  ags_channel_recursive_run_stage(channel,
					  sound_scope, (AGS_SOUND_STAGING_RESET |
							AGS_SOUND_STAGING_FEED_INPUT_QUEUE |
							AGS_SOUND_STAGING_AUTOMATE |
							AGS_SOUND_STAGING_RUN_PRE));

	  ags_channel_recursive_run_stage(channel,
					  sound_scope, (AGS_SOUND_STAGING_RUN_INTER));

	  ags_channel_recursive_run_stage(channel,
					  sound_scope, (AGS_SOUND_STAGING_RUN_POST |
							AGS_SOUND_STAGING_DO_FEEDBACK |
							AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE));
#else
	  ags_channel_recursive_run_stage(channel,
					  sound_scope, (AGS_SOUND_STAGING_AUTOMATE |
							AGS_SOUND_STAGING_RUN_PRE |
							AGS_SOUND_STAGING_RUN_INTER |
							AGS_SOUND_STAGING_RUN_POST));
#endif
	  
	  g_list_free_full(recall_id,
			   g_object_unref);
	}
      }else{	
	for(sound_scope = 0; sound_scope < AGS_SOUND_SCOPE_LAST; sound_scope++){
	  if(ags_playback_get_recall_id((AgsPlayback *) playback->data, sound_scope) == NULL){
	    continue;
	  }

	  if((recall_id = ags_channel_check_scope(channel, sound_scope)) != NULL){
#if 1
	    ags_channel_recursive_run_stage(channel,
					    sound_scope, (AGS_SOUND_STAGING_RESET |
							  AGS_SOUND_STAGING_FEED_INPUT_QUEUE |
							  AGS_SOUND_STAGING_AUTOMATE |
							  AGS_SOUND_STAGING_RUN_PRE));

	    ags_channel_recursive_run_stage(channel,
					    sound_scope, (AGS_SOUND_STAGING_RUN_INTER));

	    ags_channel_recursive_run_stage(channel,
					    sound_scope, (AGS_SOUND_STAGING_RUN_POST |
							  AGS_SOUND_STAGING_DO_FEEDBACK |
							  AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE));
#else
	    ags_channel_recursive_run_stage(channel,
					    sound_scope, (AGS_SOUND_STAGING_AUTOMATE |
							  AGS_SOUND_STAGING_RUN_PRE |
							  AGS_SOUND_STAGING_RUN_INTER |
							  AGS_SOUND_STAGING_RUN_POST));
#endif
	  
	    g_list_free_full(recall_id,
			     g_object_unref);
	  }
	}
      }
    }
    
    playback = playback->next;
  }
  
  /* 
   * wait to be completed
   */
  g_rec_mutex_lock(thread_mutex);

  sound_scope = audio_thread->sound_scope;
  
  g_rec_mutex_unlock(thread_mutex);

  if(sound_scope != AGS_SOUND_SCOPE_PLAYBACK){
    /* input */
    playback = input_playback_start;

    while(playback != NULL){
      if(ags_playback_test_flags(playback->data, AGS_PLAYBACK_SUPER_THREADED_CHANNEL)){
	ags_audio_thread_sync_channel_super_threaded(audio_thread, playback->data);
      }
    
      playback = playback->next;
    }
  
    /* output */
    playback = output_playback_start;

    while(playback != NULL){
      if(ags_playback_test_flags(playback->data, AGS_PLAYBACK_SUPER_THREADED_CHANNEL)){
	ags_audio_thread_sync_channel_super_threaded(audio_thread, playback->data);
      }
    
      playback = playback->next;
    }
  }

  g_list_free_full(input_playback_start,
		   g_object_unref);
  
  g_list_free_full(output_playback_start,
		   g_object_unref);

  g_list_free(audio_thread->sync_thread);
  audio_thread->sync_thread = NULL;
  
  /* sync */
  if(sound_scope != AGS_SOUND_SCOPE_PLAYBACK){
    g_mutex_lock(&(audio_thread->done_mutex));

    ags_audio_thread_unset_status_flags(audio_thread, AGS_AUDIO_THREAD_STATUS_WAIT_SYNC);
  	    
    if(!ags_audio_thread_test_status_flags(audio_thread, AGS_AUDIO_THREAD_STATUS_DONE_SYNC)){
      g_cond_signal(&(audio_thread->done_cond));
    }

    g_mutex_unlock(&(audio_thread->done_mutex));
  }

  /* unref */
  g_object_unref(audio);

  g_object_unref(playback_domain);
}

void
ags_audio_thread_stop(AgsThread *thread)
{
  AgsAudioThread *audio_thread;
  
  AgsThread *child, *next_child;

  if(!ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING)){
    return;
  }

#ifdef AGS_DEBUG
  g_message("audio thread stop");    
#endif
  
  audio_thread = AGS_AUDIO_THREAD(thread);
  
  /* call parent */
  AGS_THREAD_CLASS(ags_audio_thread_parent_class)->stop(thread);

  /* stop channel */
  child = ags_thread_children(thread);

  while(child != NULL){
    ags_thread_stop(child);

    /* iterate */
    next_child = ags_thread_next(child);

    g_object_unref(child);

    child = next_child;
  }

  /* ensure synced */
  g_mutex_lock(&(audio_thread->done_mutex));

  ags_audio_thread_unset_status_flags(audio_thread, AGS_AUDIO_THREAD_STATUS_WAIT_SYNC);

  if(!ags_audio_thread_test_status_flags(audio_thread, AGS_AUDIO_THREAD_STATUS_DONE_SYNC)){
    g_cond_signal(&(audio_thread->done_cond));
  }
  
  g_mutex_unlock(&(audio_thread->done_mutex));
}

void
ags_audio_thread_play_channel_super_threaded(AgsAudioThread *audio_thread, AgsPlayback *playback)
{
  AgsChannel *channel;
  AgsChannelThread *channel_thread;

  AgsThread *thread;

  GList *recall_id;
  
  gint sound_scope;

  GRecMutex *thread_mutex;  

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(audio_thread);

  g_object_get(playback,
	       "channel", &channel,
	       NULL);

  g_rec_mutex_lock(thread_mutex);

  sound_scope = audio_thread->sound_scope;
  
  g_rec_mutex_unlock(thread_mutex);
      
  if(sound_scope >= 0){
    if(sound_scope != AGS_SOUND_SCOPE_PLAYBACK){
      if((recall_id = ags_channel_check_scope(channel, sound_scope)) != NULL){
	thread = ags_playback_get_channel_thread(playback,
						 sound_scope);

	if(thread != NULL){
	  channel_thread = (AgsChannelThread *) thread;

	  if(ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING) &&
	     ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_SYNCED)){
	    /* wakeup wait */
	    g_mutex_lock(&(channel_thread->wakeup_mutex));

	    ags_channel_thread_unset_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_WAIT);
	    
	    if(!ags_channel_thread_test_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_DONE)){
	      g_cond_signal(&(channel_thread->wakeup_cond));
	    }
	    
	    g_mutex_unlock(&(channel_thread->wakeup_mutex));

	    audio_thread->sync_thread = g_list_prepend(audio_thread->sync_thread,
						       channel_thread);
	  }
	}
      
	g_list_free_full(recall_id,
			 g_object_unref);
      }
    }    
  }else{
    for(sound_scope = 0; sound_scope < AGS_SOUND_SCOPE_LAST; sound_scope++){    
      if(sound_scope == AGS_SOUND_SCOPE_PLAYBACK){
	continue;
      }
      
      if((recall_id = ags_channel_check_scope(channel, sound_scope)) != NULL){
	thread = ags_playback_get_channel_thread(playback,
						 sound_scope);

	if(thread != NULL){
	  channel_thread = (AgsChannelThread *) thread;

	  if(ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING) &&
	     ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_SYNCED)){
	    /* wakeup wait */
	    g_mutex_lock(&(channel_thread->wakeup_mutex));

	    ags_channel_thread_unset_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_WAIT);
	    
	    if(!ags_channel_thread_test_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_DONE)){
	      g_cond_signal(&(channel_thread->wakeup_cond));
	    }
	    
	    g_mutex_unlock(&(channel_thread->wakeup_mutex));

	    audio_thread->sync_thread = g_list_prepend(audio_thread->sync_thread,
						       channel_thread);
	  }
	}
      
	g_list_free_full(recall_id,
			 g_object_unref);
      }
    }
  }

  /* unref */
  g_object_unref(channel);
}

void
ags_audio_thread_sync_channel_super_threaded(AgsAudioThread *audio_thread, AgsPlayback *playback)
{
  AgsChannel *channel;
  AgsChannelThread *channel_thread;

  AgsThread *thread;

  GList *recall_id;
  
  gint sound_scope;

  GRecMutex *thread_mutex;

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(audio_thread);

  g_rec_mutex_lock(thread_mutex);

  sound_scope = audio_thread->sound_scope;
  
  g_rec_mutex_unlock(thread_mutex);

  g_object_get(playback,
	       "channel", &channel,
	       NULL);
      
  if(sound_scope >= 0){
    if(sound_scope != AGS_SOUND_SCOPE_PLAYBACK){
      if((recall_id = ags_channel_check_scope(channel, sound_scope)) != NULL){
	thread = ags_playback_get_channel_thread(playback,
						 sound_scope);

	if(thread != NULL){
	  channel_thread = (AgsChannelThread *) thread;

	  g_mutex_lock(&(channel_thread->done_mutex));
  
	  if(g_list_find(audio_thread->sync_thread, channel_thread) != NULL &&
	     ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING) &&
	     ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_SYNCED)){
	    if(ags_channel_thread_test_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_WAIT_SYNC)){
	      ags_channel_thread_unset_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_DONE_SYNC);
      
	      while(!ags_channel_thread_test_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_DONE_SYNC) &&
		    ags_channel_thread_test_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_WAIT_SYNC)){
		g_cond_wait(&(channel_thread->done_cond),
			    &(channel_thread->done_mutex));
	      }
	    }
	  }

	  ags_channel_thread_set_status_flags(channel_thread, (AGS_CHANNEL_THREAD_STATUS_WAIT_SYNC |
							       AGS_CHANNEL_THREAD_STATUS_DONE_SYNC));

	  g_mutex_unlock(&(channel_thread->done_mutex));
	}
      
	g_list_free_full(recall_id,
			 g_object_unref);
      }
    }
  }else{
    for(sound_scope = 0; sound_scope < AGS_SOUND_SCOPE_LAST; sound_scope++){    
      if(sound_scope == AGS_SOUND_SCOPE_PLAYBACK){
	continue;
      }

      if((recall_id = ags_channel_check_scope(channel, sound_scope)) != NULL){
	thread = ags_playback_get_channel_thread(playback,
						 sound_scope);

	if(thread != NULL){
	  channel_thread = (AgsChannelThread *) thread;

	  g_mutex_lock(&(channel_thread->done_mutex));
  
	  if(g_list_find(audio_thread->sync_thread, channel_thread) != NULL &&
	     ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING) &&
	     ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_SYNCED)){
	    if(ags_channel_thread_test_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_WAIT_SYNC)){
	      ags_channel_thread_unset_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_DONE_SYNC);
      
	      while(!ags_channel_thread_test_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_DONE_SYNC) &&
		    ags_channel_thread_test_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_WAIT_SYNC)){
		g_cond_wait(&(channel_thread->done_cond),
			    &(channel_thread->done_mutex));
	      }
	    }
	  }

	  ags_channel_thread_set_status_flags(channel_thread, (AGS_CHANNEL_THREAD_STATUS_WAIT_SYNC |
							       AGS_CHANNEL_THREAD_STATUS_DONE_SYNC));

	  g_mutex_unlock(&(channel_thread->done_mutex));
	}
      
	g_list_free_full(recall_id,
			 g_object_unref);
      }
    }
  }

  /* unref */
  g_object_unref(channel);
}

/**
 * ags_audio_thread_test_status_flags:
 * @audio_thread: the #AgsAudioThread
 * @status_flags: status flags
 * 
 * Test @status_flags of @audio_thread.
 * 
 * Returns: %TRUE if status flags set, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_audio_thread_test_status_flags(AgsAudioThread *audio_thread, guint status_flags)
{
  gboolean retval;
  
  if(!AGS_IS_AUDIO_THREAD(audio_thread)){
    return(FALSE);
  }

  retval = ((status_flags & (g_atomic_int_get(&(audio_thread->status_flags)))) != 0) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_audio_thread_set_status_flags:
 * @audio_thread: the #AgsAudioThread
 * @status_flags: status flags
 * 
 * Set status flags.
 * 
 * Since: 3.0.0
 */
void
ags_audio_thread_set_status_flags(AgsAudioThread *audio_thread, guint status_flags)
{
  if(!AGS_IS_AUDIO_THREAD(audio_thread)){
    return;
  }

  g_atomic_int_or(&(audio_thread->status_flags),
		  status_flags);
}

/**
 * ags_audio_thread_unset_status_flags:
 * @audio_thread: the #AgsAudioThread
 * @status_flags: status flags
 * 
 * Unset status flags.
 * 
 * Since: 3.0.0
 */
void
ags_audio_thread_unset_status_flags(AgsAudioThread *audio_thread, guint status_flags)
{
  if(!AGS_IS_AUDIO_THREAD(audio_thread)){
    return;
  }

  g_atomic_int_and(&(audio_thread->status_flags),
		   (~status_flags));
}

/**
 * ags_audio_thread_set_sound_scope:
 * @audio_thread: the #AgsAudioThread
 * @sound_scope: the sound scope
 * 
 * Set sound scope.
 * 
 * Since: 3.0.0
 */
void
ags_audio_thread_set_sound_scope(AgsAudioThread *audio_thread,
				 gint sound_scope)
{
  GRecMutex *thread_mutex;
  
  if(!AGS_IS_AUDIO_THREAD(audio_thread)){
    return;
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(audio_thread);

  /* set scope */
  g_rec_mutex_lock(thread_mutex);

  audio_thread->sound_scope = sound_scope;
  
  g_rec_mutex_unlock(thread_mutex);
}

/**
 * ags_audio_thread_new:
 * @default_output_soundcard: the #GObject
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsAudioThread.
 *
 * Returns: the new #AgsAudioThread
 *
 * Since: 3.0.0
 */
AgsAudioThread*
ags_audio_thread_new(GObject *default_output_soundcard,
		     GObject *audio)
{
  AgsAudioThread *audio_thread;

  audio_thread = (AgsAudioThread *) g_object_new(AGS_TYPE_AUDIO_THREAD,
						 "default-output-soundcard", default_output_soundcard,
						 "audio", audio,
						 NULL);


  return(audio_thread);
}
