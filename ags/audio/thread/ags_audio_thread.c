/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>

#include <ags/audio/thread/ags_channel_thread.h>

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
void ags_audio_thread_connect(AgsConnectable *connectable);
void ags_audio_thread_disconnect(AgsConnectable *connectable);
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
  PROP_SOUNDCARD,
  PROP_AUDIO,
};

static gpointer ags_audio_thread_parent_class = NULL;
static AgsConnectableInterface *ags_audio_thread_parent_connectable_interface;

GType
ags_audio_thread_get_type()
{
  static GType ags_type_audio_thread = 0;

  if(!ags_type_audio_thread){
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
						   "AgsAudioThread\0",
						   &ags_audio_thread_info,
						   0);
    
    g_type_add_interface_static(ags_type_audio_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_audio_thread);
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

  gobject->finalize = ags_audio_thread_finalize;

  /* properties */
  /**
   * AgsAudioThread:soundcard:
   *
   * The assigned #AgsAudio.
   * 
   * Since: 0.7.0
   */
  param_spec = g_param_spec_object("soundcard\0",
				   "soundcard assigned to\0",
				   "The AgsSoundcard it is assigned to.\0",
				   G_TYPE_OBJECT,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsAudioThread:audio:
   *
   * The assigned #AgsAudio.
   * 
   * Since: 0.4.2
   */
  param_spec = g_param_spec_object("audio\0",
				   "audio assigned to\0",
				   "The AgsAudio it is assigned to.\0",
				   AGS_TYPE_AUDIO,
				   G_PARAM_WRITABLE);
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

  connectable->connect = ags_audio_thread_connect;
  connectable->disconnect = ags_audio_thread_disconnect;
}

void
ags_audio_thread_init(AgsAudioThread *audio_thread)
{
  AgsThread *thread;

  AgsConfig *config;

  gchar *str0, *str1;
  
  thread = (AgsThread *) audio_thread;

  config = ags_config_get_instance();
  
  str0 = ags_config_get_value(config,
			AGS_CONFIG_SOUNDCARD,
			"samplerate\0");
  str1 = ags_config_get_value(config,
			AGS_CONFIG_SOUNDCARD,
			"buffer-size\0");

  if(str0 == NULL || str1 == NULL){
    thread->freq = AGS_AUDIO_THREAD_DEFAULT_JIFFIE;
  }else{
    guint samplerate;
    guint buffer_size;

    samplerate = g_ascii_strtoull(str0,
				  NULL,
				  10);
    buffer_size = g_ascii_strtoull(str1,
				   NULL,
				   10);

    thread->freq = ceil((gdouble) samplerate / (gdouble) buffer_size);
  }

  g_free(str0);
  g_free(str1);

  g_atomic_int_set(&(audio_thread->flags),
		   0);
  
  audio_thread->soundcard = NULL;
  
  /* start */
  pthread_mutexattr_init(&(audio_thread->wakeup_attr));
  pthread_mutexattr_settype(&(audio_thread->wakeup_attr),
			    PTHREAD_MUTEX_RECURSIVE);

  audio_thread->wakeup_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(audio_thread->wakeup_mutex,
		     &(audio_thread->wakeup_attr));
  
  audio_thread->wakeup_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(audio_thread->wakeup_cond, NULL);

  /* sync */
  pthread_mutexattr_init(&(audio_thread->done_attr));
  pthread_mutexattr_settype(&(audio_thread->done_attr),
			    PTHREAD_MUTEX_RECURSIVE);

  audio_thread->done_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(audio_thread->done_mutex,
		     &(audio_thread->done_attr));
  
  audio_thread->done_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(audio_thread->done_cond, NULL);
}

void
ags_audio_thread_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsAudioThread *audio_thread;

  audio_thread = AGS_AUDIO_THREAD(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = g_value_get_object(value);

      if(soundcard == audio_thread->soundcard){
	return;
      }

      if(audio_thread->soundcard != NULL){
	g_object_unref(audio_thread->soundcard);
      }

      if(soundcard != NULL){
	g_object_ref(soundcard);
      }

      audio_thread->soundcard = soundcard;
    }
    break;
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(audio == audio_thread->audio){
	return;
      }
      
      if(audio_thread->audio != NULL){
	g_object_unref(G_OBJECT(audio_thread->audio));
      }

      if(audio != NULL){
	g_object_ref(G_OBJECT(audio));
      }

      audio_thread->audio = G_OBJECT(audio);
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

  audio_thread = AGS_AUDIO_THREAD(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, audio_thread->soundcard);
    }
    break;
  case PROP_AUDIO:
    {
      g_value_set_object(value, G_OBJECT(audio_thread->audio));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_thread_connect(AgsConnectable *connectable)
{
  ags_audio_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_audio_thread_disconnect(AgsConnectable *connectable)
{
  ags_audio_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_audio_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_audio_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_audio_thread_start(AgsThread *thread)
{
  g_message("audio thread start\0");
  
  /* reset status */
  g_atomic_int_or(&(AGS_AUDIO_THREAD(thread)->flags),
		   (AGS_AUDIO_THREAD_WAIT |
		    AGS_AUDIO_THREAD_DONE |
		    AGS_AUDIO_THREAD_WAIT_SYNC |
		    AGS_AUDIO_THREAD_DONE_SYNC));

  AGS_THREAD_CLASS(ags_audio_thread_parent_class)->start(thread);
}

void
ags_audio_thread_run(AgsThread *thread)
{
  AgsAudio *audio;
  AgsChannel *output;
  AgsPlaybackDomain *playback_domain;
  AgsPlayback *playback;
  
  AgsMutexManager *mutex_manager;
  AgsAudioThread *audio_thread;

  gint stage;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *output_mutex;

  if(!thread->rt_setup){
    struct sched_param param;
    
    /* Declare ourself as a real time task */
    param.sched_priority = AGS_RT_PRIORITY;
      
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
      perror("sched_setscheduler failed\0");
    }

    thread->rt_setup = TRUE;
  }

  if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0){
    g_message("b\0");
    return;
  }
  
  audio_thread = AGS_AUDIO_THREAD(thread);
  
  //  thread->freq = AGS_SOUNDCARD(thread->soundcard)->delay[AGS_SOUNDCARD(thread->soundcard)->tic_counter] / AGS_SOUNDCARD(thread->soundcard)->delay_factor;
  
  audio = audio_thread->audio;
  playback_domain = audio->playback_domain;
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  //  g_message(" --- a");
  
  /* start - wait until signaled */
  pthread_mutex_lock(audio_thread->wakeup_mutex);
        
  if((AGS_AUDIO_THREAD_WAIT & (g_atomic_int_get(&(audio_thread->flags)))) != 0){
    g_atomic_int_and(&(audio_thread->flags),
		     (~AGS_AUDIO_THREAD_DONE));
    
    while((AGS_AUDIO_THREAD_WAIT & (g_atomic_int_get(&(audio_thread->flags)))) != 0 &&
	  (AGS_AUDIO_THREAD_DONE & (g_atomic_int_get(&(audio_thread->flags)))) == 0){
      pthread_cond_wait(audio_thread->wakeup_cond,
			audio_thread->wakeup_mutex);
    }
  }
    
  g_atomic_int_or(&(audio_thread->flags),
		  (AGS_AUDIO_THREAD_WAIT |
		   AGS_AUDIO_THREAD_DONE));
    
  pthread_mutex_unlock(audio_thread->wakeup_mutex);
  
  /* lookup audio mutex */
  pthread_mutex_lock(application_mutex);
    
  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
    
  pthread_mutex_unlock(application_mutex);

  /* emit tact callback */
  //  ags_audio_tact(audio);
  
  /* do audio processing */
  pthread_mutex_lock(audio_mutex);
  
  output = audio->output;

  pthread_mutex_unlock(audio_mutex);
  
  while(output != NULL){
    /* lookup output mutex */
    pthread_mutex_lock(application_mutex);

    output_mutex = ags_mutex_manager_lookup(mutex_manager,
					    (GObject *) output);
      
    pthread_mutex_unlock(application_mutex);

    /* get playback */
    pthread_mutex_lock(output_mutex);
    
    playback = output->playback;
      
    pthread_mutex_unlock(output_mutex);

    if((AGS_PLAYBACK_SUPER_THREADED_CHANNEL & (g_atomic_int_get(&(playback->flags)))) != 0){
      ags_audio_thread_play_channel_super_threaded(audio_thread, playback);
    }else{
      for(stage = 0; stage < 3; stage++){
	if(audio_thread == playback_domain->audio_thread[0]){
	  ags_channel_recursive_play(output, playback->recall_id[0], stage);
	}

	if(audio_thread == playback_domain->audio_thread[1]){
	  ags_channel_recursive_play(output, playback->recall_id[1], stage);
	}

	if(audio_thread == playback_domain->audio_thread[2]){
	  ags_channel_recursive_play(output, playback->recall_id[2], stage);
	}
      }
    }
    
    output = output->next;
  }


  /* sync - wait the 3 stages */
  pthread_mutex_lock(audio_mutex);
  
  output = audio->output;

  pthread_mutex_unlock(audio_mutex);

  while(output != NULL){
    /* lookup output mutex */
    pthread_mutex_lock(application_mutex);

    output_mutex = ags_mutex_manager_lookup(mutex_manager,
					    (GObject *) output);
      
    pthread_mutex_unlock(application_mutex);

    /* get playback */
    pthread_mutex_lock(output_mutex);
    
    playback = output->playback;
      
    pthread_mutex_unlock(output_mutex);

    if((AGS_PLAYBACK_SUPER_THREADED_CHANNEL & (g_atomic_int_get(&(playback->flags)))) != 0){
      ags_audio_thread_sync_channel_super_threaded(audio_thread, playback);
    }
    
    output = output->next;
  }
  
  /* sync */
  pthread_mutex_lock(audio_thread->done_mutex);

  g_atomic_int_and(&(audio_thread->flags),
		   (~AGS_AUDIO_THREAD_WAIT_SYNC));
  	    
  if((AGS_AUDIO_THREAD_DONE_SYNC & (g_atomic_int_get(&(audio_thread->flags)))) == 0){
    pthread_cond_signal(audio_thread->done_cond);
  }

  pthread_mutex_unlock(audio_thread->done_mutex);
}

void
ags_audio_thread_stop(AgsThread *thread)
{
  AgsAudioThread *audio_thread;
  AgsThread *child;

  g_message("audio thread stop\0");
  
  audio_thread = AGS_AUDIO_THREAD(thread);

  /* call parent */
  AGS_THREAD_CLASS(ags_audio_thread_parent_class)->stop(thread);

  /* stop channel */
  child = thread->children;

  while(child != NULL){
    ags_thread_stop(child);
    
    child = child->next;
  }

  /* ensure synced */
  pthread_mutex_lock(audio_thread->done_mutex);
  
  g_atomic_int_and(&(audio_thread->flags),
		   (~AGS_AUDIO_THREAD_WAIT_SYNC));

  if((AGS_AUDIO_THREAD_DONE_SYNC & (g_atomic_int_get(&(audio_thread->flags)))) == 0){
    pthread_cond_signal(audio_thread->done_cond);
  }
  
  pthread_mutex_unlock(audio_thread->done_mutex);
}

void
ags_audio_thread_play_channel_super_threaded(AgsAudioThread *audio_thread, AgsPlayback *playback)
{
  AgsAudio *audio;
  AgsPlaybackDomain *playback_domain;

  gboolean do_playback, do_sequencer, do_notation;

  audio = audio_thread->audio;
  playback_domain = audio->playback_domain;

  /* super threaded audio level */
  do_sequencer = FALSE;
  do_notation = FALSE;

  /* sequencer */
  if(audio_thread == playback_domain->audio_thread[1]){
    AgsThread *thread;
    AgsChannelThread *channel_thread;

    thread = playback->channel_thread[1];
    channel_thread = (AgsThread *) thread;
    
    do_sequencer = TRUE;

    if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(thread->flags)))) != 0){
      /* wakeup wait */
      pthread_mutex_lock(channel_thread->wakeup_mutex);

      g_atomic_int_and(&(channel_thread->flags),
		       (~AGS_CHANNEL_THREAD_WAIT));
	    
      if((AGS_CHANNEL_THREAD_DONE & (g_atomic_int_get(&(channel_thread->flags)))) == 0){
	pthread_cond_signal(channel_thread->wakeup_cond);
      }
	    
      pthread_mutex_unlock(channel_thread->wakeup_mutex);
    }
  }

  /* notation */
  if(audio_thread == playback_domain->audio_thread[2]){
    AgsThread *thread;
    AgsChannelThread *channel_thread;

    thread = playback->channel_thread[2];
    channel_thread = (AgsThread *) thread;

    do_notation = TRUE;

    if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(thread->flags)))) != 0){
      /* wakeup wait */
      pthread_mutex_lock(channel_thread->wakeup_mutex);
	  
      g_atomic_int_and(&(channel_thread->flags),
		       (~AGS_CHANNEL_THREAD_WAIT));
	  
      if((AGS_CHANNEL_THREAD_DONE & (g_atomic_int_get(&(channel_thread->flags)))) == 0){
	pthread_cond_signal(channel_thread->wakeup_cond);
      }
	  
      pthread_mutex_unlock(channel_thread->wakeup_mutex);
    }
  }
}

void
ags_audio_thread_sync_channel_super_threaded(AgsAudioThread *audio_thread, AgsPlayback *playback)
{
  AgsAudio *audio;
  AgsPlaybackDomain *playback_domain;

  gboolean do_sequencer, do_notation;

  audio = audio_thread->audio;
  playback_domain = audio->playback_domain;
  
  do_sequencer = FALSE;
  do_notation = FALSE;

  /* sequencer */
  if(audio_thread == playback_domain->audio_thread[1]){
    AgsChannelThread *channel_thread;

    channel_thread = (AgsChannelThread *) playback->channel_thread[1];
    do_sequencer = TRUE;

    pthread_mutex_lock(channel_thread->done_mutex);
  
    if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(playback->channel_thread[1]->flags)))) != 0){

      if((AGS_CHANNEL_THREAD_WAIT_SYNC & (g_atomic_int_get(&(channel_thread->flags)))) != 0){
	g_atomic_int_and(&(channel_thread->flags),
			 (~AGS_CHANNEL_THREAD_DONE_SYNC));
      
	while((AGS_CHANNEL_THREAD_DONE_SYNC & (g_atomic_int_get(&(channel_thread->flags)))) == 0 &&
	      (AGS_CHANNEL_THREAD_WAIT_SYNC & (g_atomic_int_get(&(channel_thread->flags)))) != 0){
	  pthread_cond_wait(channel_thread->done_cond,
			    channel_thread->done_mutex);
	}
      }
    }

    g_atomic_int_or(&(channel_thread->flags),
		    (AGS_CHANNEL_THREAD_WAIT_SYNC |
		     AGS_CHANNEL_THREAD_DONE_SYNC));

    pthread_mutex_unlock(channel_thread->done_mutex);
  }

  /* notation */
  if(audio_thread == playback_domain->audio_thread[2]){
    AgsChannelThread *channel_thread;
    
    channel_thread = (AgsChannelThread *) playback->channel_thread[2];
    do_notation = TRUE;

    pthread_mutex_lock(channel_thread->done_mutex);

    if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(playback->channel_thread[2]->flags)))) != 0){

      if((AGS_CHANNEL_THREAD_WAIT_SYNC & (g_atomic_int_get(&(channel_thread->flags)))) != 0){

	g_atomic_int_and(&(channel_thread->flags),
			 (~AGS_CHANNEL_THREAD_DONE_SYNC));
    
	while((AGS_CHANNEL_THREAD_DONE_SYNC & (g_atomic_int_get(&(channel_thread->flags)))) == 0 &&
	      (AGS_CHANNEL_THREAD_WAIT_SYNC & (g_atomic_int_get(&(channel_thread->flags)))) != 0){
	  pthread_cond_wait(channel_thread->done_cond,
			    channel_thread->done_mutex);
	}
      }
    }

    g_atomic_int_or(&(channel_thread->flags),
		     (AGS_CHANNEL_THREAD_WAIT_SYNC |
		      AGS_CHANNEL_THREAD_DONE_SYNC));
      
    pthread_mutex_unlock(channel_thread->done_mutex);
  }
}

/**
 * ags_audio_thread_new:
 * @soundcard: the #GObject
 * @audio: the #AgsAudio
 *
   * Create a new #AgsAudioThread.
 *
 * Returns: the new #AgsAudioThread
 *
 * Since: 0.4.2
 */
AgsAudioThread*
ags_audio_thread_new(GObject *soundcard,
		     GObject *audio)
{
  AgsAudioThread *audio_thread;

  audio_thread = (AgsAudioThread *) g_object_new(AGS_TYPE_AUDIO_THREAD,
						 "soundcard\0", soundcard,
						 "audio\0", audio,
						 NULL);


  return(audio_thread);
}
