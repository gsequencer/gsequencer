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

#include <ags/thread/ags_audio_thread.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_channel_thread.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>

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

void ags_audio_thread_play_channel_super_threaded(AgsAudioThread *audio_thread, AgsDevoutPlay *devout_play);
void ags_audio_thread_sync_channel_super_threaded(AgsAudioThread *audio_thread, AgsDevoutPlay *devout_play);

/**
 * SECTION:ags_audio_thread
 * @short_description: audio thread
 * @title: AgsAudioThread
 * @section_id:
 * @include: ags/thread/ags_audio_thread.h
 *
 * The #AgsAudioThread acts as audio output thread to soundcard.
 */

enum{
  PROP_0,
  PROP_AUDIO,
};

static gpointer ags_audio_thread_parent_class = NULL;
static AgsConnectableInterface *ags_audio_thread_parent_connectable_interface;

extern pthread_mutex_t ags_application_mutex;

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
  AGS_THREAD(audio_thread)->freq = AGS_AUDIO_THREAD_DEFAULT_JIFFIE;

  g_atomic_int_set(&(audio_thread->flags),
		   0);

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
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

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
  /* reset status */
  g_atomic_int_and(&(AGS_AUDIO_THREAD(thread)->flags),
		   (~(AGS_AUDIO_THREAD_WAKEUP |
		      AGS_AUDIO_THREAD_WAITING |
		      AGS_AUDIO_THREAD_NOTIFY |
  		      AGS_AUDIO_THREAD_DONE)));

  AGS_THREAD_CLASS(ags_audio_thread_parent_class)->start(thread);
}

void
ags_audio_thread_run(AgsThread *thread)
{
  AgsAudio *audio;
  AgsChannel *output;
  AgsDevoutPlay *devout_play;
  
  AgsMutexManager *mutex_manager;
  AgsAudioThread *audio_thread;

  gint stage;
  
  pthread_mutex_t *audio_mutex;

  if(!thread->rt_setup){
    struct sched_param param;
    
    /* Declare ourself as a real time task */
    param.sched_priority = AGS_RT_PRIORITY;
      
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
      perror("sched_setscheduler failed\0");
    }

    thread->rt_setup = TRUE;
  }
  
  audio_thread = AGS_AUDIO_THREAD(thread);
  audio = audio_thread->audio;
  
  //  g_message(" --- a");
  
  /* start - wait until signaled */
  pthread_mutex_lock(audio_thread->wakeup_mutex);

  if((AGS_AUDIO_THREAD_WAKEUP & (g_atomic_int_get(&(audio_thread->flags)))) == 0){
    g_atomic_int_or(&(audio_thread->flags),
		    AGS_AUDIO_THREAD_WAITING);
    
    while((AGS_AUDIO_THREAD_WAKEUP & (g_atomic_int_get(&(audio_thread->flags)))) == 0){
      pthread_cond_wait(audio_thread->wakeup_cond,
			audio_thread->wakeup_mutex);
    }
  }
  
  g_atomic_int_and(&(audio_thread->flags),
		   (~AGS_AUDIO_THREAD_WAITING));
  g_atomic_int_and(&(audio_thread->flags),
		   (~AGS_AUDIO_THREAD_WAKEUP));
  
  pthread_mutex_unlock(audio_thread->wakeup_mutex);

  /* audio mutex */
  pthread_mutex_lock(&(ags_application_mutex));

  mutex_manager = ags_mutex_manager_get_instance();
  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
      
  pthread_mutex_unlock(&(ags_application_mutex));

  /* do audio processing */
  output = audio->output;

  while(output != NULL){
    devout_play = output->devout_play;
      
    if((AGS_DEVOUT_PLAY_SUPER_THREADED_CHANNEL & (g_atomic_int_get(&(devout_play->flags)))) != 0){
      ags_audio_thread_play_channel_super_threaded(audio_thread, devout_play);
    }else{
      for(stage = 0; stage < 3; stage++){
	if((AGS_DEVOUT_PLAY_PLAYBACK & (g_atomic_int_get(&(devout_play->flags)))) != 0){
	  ags_channel_recursive_play(output, devout_play->recall_id[0], stage);
	}

	if((AGS_DEVOUT_PLAY_SEQUENCER & (g_atomic_int_get(&(devout_play->flags)))) != 0){
	  ags_channel_recursive_play(output, devout_play->recall_id[1], stage);
	}

	if((AGS_DEVOUT_PLAY_NOTATION & (g_atomic_int_get(&(devout_play->flags)))) != 0){
	  ags_channel_recursive_play(output, devout_play->recall_id[2], stage);
	}
      }
    }
    
    output = output->next;
  }


  /* sync - wait the 3 stages */
  output = audio->output;

  while(output != NULL){
    devout_play = output->devout_play;
    
    if((AGS_DEVOUT_PLAY_SUPER_THREADED_CHANNEL & (g_atomic_int_get(&(devout_play->flags)))) != 0){
      ags_audio_thread_sync_channel_super_threaded(audio_thread, devout_play);
    }
    
    output = output->next;
  }
  
  /* sync */
  pthread_mutex_lock(audio_thread->done_mutex);

  g_atomic_int_or(&(audio_thread->flags),
		  AGS_AUDIO_THREAD_DONE);
	    
  if((AGS_AUDIO_THREAD_NOTIFY & (g_atomic_int_get(&(audio_thread->flags)))) != 0){
    pthread_cond_signal(audio_thread->done_cond);
  }
	    
  pthread_mutex_unlock(audio_thread->done_mutex);
}

void
ags_audio_thread_stop(AgsThread *thread)
{
  AgsAudioThread *audio_thread;
  AgsThread *child;
  
  audio_thread = AGS_AUDIO_THREAD(thread);

  child = thread->children;

  while(child != NULL){
    ags_thread_stop(child);
    
    child = child->next;
  }
  
  pthread_mutex_lock(audio_thread->wakeup_mutex);
  
  g_atomic_int_or(&(audio_thread->flags),
		  AGS_AUDIO_THREAD_WAKEUP);

  if((AGS_AUDIO_THREAD_WAITING & (g_atomic_int_get(&(audio_thread->flags)))) != 0){
    pthread_cond_signal(audio_thread->wakeup_cond);
  }
  
  pthread_mutex_unlock(audio_thread->wakeup_mutex);
  
  AGS_THREAD_CLASS(ags_audio_thread_parent_class)->stop(thread);
}

void
ags_audio_thread_play_channel_super_threaded(AgsAudioThread *audio_thread, AgsDevoutPlay *devout_play)
{
  gboolean playback, sequencer, notation;
	
  /* super threaded audio level */
  sequencer = FALSE;
  notation = FALSE;

  /* sequencer */
  if((AGS_DEVOUT_PLAY_SEQUENCER & (g_atomic_int_get(&(devout_play->flags)))) != 0){
    sequencer = TRUE;

    if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(devout_play->channel_thread[1]->flags)))) == 0){
      guint val;

      ags_thread_start(devout_play->channel_thread[1]);
  
      /* wait child */
      pthread_mutex_lock(devout_play->channel_thread[1]->start_mutex);

      val = g_atomic_int_get(&(devout_play->channel_thread[1]->flags));

      if((AGS_THREAD_INITIAL_RUN & val) != 0){
	while((AGS_THREAD_INITIAL_RUN & val) != 0){
	  pthread_cond_wait(devout_play->channel_thread[1]->start_cond,
			    devout_play->channel_thread[1]->start_mutex);

	  val = g_atomic_int_get(&(devout_play->channel_thread[1]->flags));
	}
      }

      pthread_mutex_unlock(devout_play->channel_thread[1]->start_mutex);
    }

    /* wakeup wait */
    pthread_mutex_lock(AGS_CHANNEL_THREAD(devout_play->channel_thread[1])->wakeup_mutex);

    g_atomic_int_or(&(AGS_CHANNEL_THREAD(devout_play->channel_thread[1])->flags),
		    AGS_CHANNEL_THREAD_WAKEUP);
	    
    if((AGS_CHANNEL_THREAD_WAITING & (g_atomic_int_get(&(AGS_CHANNEL_THREAD(devout_play->channel_thread[1])->flags)))) != 0){
      pthread_cond_signal(AGS_CHANNEL_THREAD(devout_play->channel_thread[1])->wakeup_cond);
    }
	    
    pthread_mutex_unlock(AGS_CHANNEL_THREAD(devout_play->channel_thread[1])->wakeup_mutex);
  }

  /* notation */
  if((AGS_DEVOUT_PLAY_NOTATION & (g_atomic_int_get(&(devout_play->flags)))) != 0){
    notation = TRUE;

    if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(devout_play->channel_thread[2]->flags)))) == 0){
      guint val;

      ags_thread_start(devout_play->channel_thread[2]);

      /* wait child */
      pthread_mutex_lock(devout_play->channel_thread[2]->start_mutex);

      val = g_atomic_int_get(&(devout_play->channel_thread[2]->flags));

      if((AGS_THREAD_INITIAL_RUN & val) != 0){
	while((AGS_THREAD_INITIAL_RUN & val) != 0){
	  pthread_cond_wait(devout_play->channel_thread[2]->start_cond,
			    devout_play->channel_thread[2]->start_mutex);

	  val = g_atomic_int_get(&(devout_play->channel_thread[2]->flags));
	}
      }

      pthread_mutex_unlock(devout_play->channel_thread[2]->start_mutex);
    }

    /* wakeup wait */
    pthread_mutex_lock(AGS_CHANNEL_THREAD(devout_play->channel_thread[2])->wakeup_mutex);
	  
    g_atomic_int_or(&(AGS_CHANNEL_THREAD(devout_play->channel_thread[2])->flags),
		    AGS_CHANNEL_THREAD_WAKEUP);
	  
    if((AGS_CHANNEL_THREAD_WAITING & (g_atomic_int_get(&(AGS_CHANNEL_THREAD(devout_play->channel_thread[2])->flags)))) != 0){
      pthread_cond_signal(AGS_CHANNEL_THREAD(devout_play->channel_thread[2])->wakeup_cond);
    }
	  
    pthread_mutex_unlock(AGS_CHANNEL_THREAD(devout_play->channel_thread[2])->wakeup_mutex);
  }
}

void
ags_audio_thread_sync_channel_super_threaded(AgsAudioThread *audio_thread, AgsDevoutPlay *devout_play)
{
  gboolean sequencer, notation;
  
  sequencer = FALSE;
  notation = FALSE;

  /* sequencer */
  if((AGS_DEVOUT_PLAY_SEQUENCER & (g_atomic_int_get(&(devout_play->flags)))) != 0){
    sequencer = TRUE;
  
    if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(devout_play->channel_thread[1]->flags)))) != 0 &&
       (AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(devout_play->channel_thread[1]->flags)))) == 0 &&
       (AGS_THREAD_WAIT_0 & (g_atomic_int_get(&(devout_play->channel_thread[1]->flags)))) == 0 &&
       (AGS_THREAD_WAIT_1 & (g_atomic_int_get(&(devout_play->channel_thread[1]->flags)))) == 0 &&
       (AGS_THREAD_WAIT_2 & (g_atomic_int_get(&(devout_play->channel_thread[1]->flags)))) == 0){
      pthread_mutex_lock(AGS_CHANNEL_THREAD(devout_play->channel_thread[1])->done_mutex);

      if((AGS_CHANNEL_THREAD_DONE & (g_atomic_int_get(&(AGS_CHANNEL_THREAD(devout_play->channel_thread[1])->flags)))) == 0){
	g_atomic_int_or(&(AGS_CHANNEL_THREAD(devout_play->channel_thread[1])->flags),
			AGS_CHANNEL_THREAD_NOTIFY);
    
	while((AGS_CHANNEL_THREAD_DONE & (g_atomic_int_get(&(AGS_CHANNEL_THREAD(devout_play->channel_thread[1])->flags)))) == 0){
	  pthread_cond_wait(AGS_CHANNEL_THREAD(devout_play->channel_thread[1])->done_cond,
			    AGS_CHANNEL_THREAD(devout_play->channel_thread[1])->done_mutex);
	}
      }

      g_atomic_int_and(&(AGS_CHANNEL_THREAD(devout_play->channel_thread[1])->flags),
		       (~AGS_CHANNEL_THREAD_NOTIFY));
      g_atomic_int_and(&(AGS_CHANNEL_THREAD(devout_play->channel_thread[1])->flags),
			 (~AGS_CHANNEL_THREAD_DONE));

      pthread_mutex_unlock(AGS_CHANNEL_THREAD(devout_play->channel_thread[1])->done_mutex);
    }
  }

  /* notation */
  if((AGS_DEVOUT_PLAY_NOTATION & (g_atomic_int_get(&(devout_play->flags)))) != 0){
    notation = TRUE;

    if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(devout_play->channel_thread[2]->flags)))) != 0 &&
       (AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(devout_play->channel_thread[2]->flags)))) == 0 &&
       (AGS_THREAD_WAIT_0 & (g_atomic_int_get(&(devout_play->channel_thread[2]->flags)))) == 0 &&
       (AGS_THREAD_WAIT_1 & (g_atomic_int_get(&(devout_play->channel_thread[2]->flags)))) == 0 &&
       (AGS_THREAD_WAIT_2 & (g_atomic_int_get(&(devout_play->channel_thread[2]->flags)))) == 0){
      pthread_mutex_lock(AGS_CHANNEL_THREAD(devout_play->channel_thread[2])->done_mutex);

      if((AGS_CHANNEL_THREAD_DONE & (g_atomic_int_get(&(AGS_CHANNEL_THREAD(devout_play->channel_thread[2])->flags)))) == 0){
	g_atomic_int_or(&(AGS_CHANNEL_THREAD(devout_play->channel_thread[2])->flags),
			AGS_CHANNEL_THREAD_NOTIFY);
    
	while((AGS_CHANNEL_THREAD_DONE & (g_atomic_int_get(&(AGS_CHANNEL_THREAD(devout_play->channel_thread[2])->flags)))) == 0){
	  pthread_cond_wait(AGS_CHANNEL_THREAD(devout_play->channel_thread[2])->done_cond,
			    AGS_CHANNEL_THREAD(devout_play->channel_thread[2])->done_mutex);
	}
      }
      
      g_atomic_int_and(&(AGS_CHANNEL_THREAD(devout_play->channel_thread[2])->flags),
			 (~AGS_CHANNEL_THREAD_NOTIFY));
      g_atomic_int_and(&(AGS_CHANNEL_THREAD(devout_play->channel_thread[2])->flags),
			 (~AGS_CHANNEL_THREAD_DONE));
      
      pthread_mutex_unlock(AGS_CHANNEL_THREAD(devout_play->channel_thread[2])->done_mutex);
    }
  }
}

/**
 * ags_audio_thread_new:
 * @devout: the #AgsDevout
 * @audio: the #AgsAudio
 *
   * Create a new #AgsAudioThread.
 *
 * Returns: the new #AgsAudioThread
 *
 * Since: 0.4.2
 */
AgsAudioThread*
ags_audio_thread_new(GObject *devout,
		     GObject *audio)
{
  AgsAudioThread *audio_thread;

  audio_thread = (AgsAudioThread *) g_object_new(AGS_TYPE_AUDIO_THREAD,
						 "devout\0", devout,
						 "audio\0", audio,
						 NULL);


  return(audio_thread);
}
