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

#include <ags/audio/thread/ags_channel_thread.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_playback.h>

void ags_channel_thread_class_init(AgsChannelThreadClass *channel_thread);
void ags_channel_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_channel_thread_init(AgsChannelThread *channel_thread);
void ags_channel_thread_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_channel_thread_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_channel_thread_connect(AgsConnectable *connectable);
void ags_channel_thread_disconnect(AgsConnectable *connectable);
void ags_channel_thread_finalize(GObject *gobject);

void ags_channel_thread_start(AgsThread *thread);
void ags_channel_thread_run(AgsThread *thread);
void ags_channel_thread_stop(AgsThread *thread);

/**
 * SECTION:ags_channel_thread
 * @short_description: channel thread
 * @title: AgsChannelThread
 * @section_id:
 * @include: ags/thread/ags_channel_thread.h
 *
 * The #AgsChannelThread acts as channel output thread to soundcard.
 */

enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_CHANNEL,
};

static gpointer ags_channel_thread_parent_class = NULL;
static AgsConnectableInterface *ags_channel_thread_parent_connectable_interface;

GType
ags_channel_thread_get_type()
{
  static GType ags_type_channel_thread = 0;

  if(!ags_type_channel_thread){
    static const GTypeInfo ags_channel_thread_info = {
      sizeof (AgsChannelThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_channel_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsChannelThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_channel_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_channel_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_channel_thread = g_type_register_static(AGS_TYPE_THREAD,
						     "AgsChannelThread\0",
						     &ags_channel_thread_info,
						     0);
    
    g_type_add_interface_static(ags_type_channel_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_channel_thread);
}

void
ags_channel_thread_class_init(AgsChannelThreadClass *channel_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  GParamSpec *param_spec;
  
  ags_channel_thread_parent_class = g_type_class_peek_parent(channel_thread);

  /* GObject */
  gobject = (GObjectClass *) channel_thread;
    
  gobject->set_property = ags_channel_thread_set_property;
  gobject->get_property = ags_channel_thread_get_property;

  gobject->finalize = ags_channel_thread_finalize;

  /* properties */
  /**
   * AgsChannelThread:soundcard:
   *
   * The assigned #AgsChannel.
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
   * AgsChannelThread:channel:
   *
   * The assigned #AgsChannel.
   * 
   * Since: 0.4.2
   */
  param_spec = g_param_spec_object("channel\0",
				   "channel assigned to\0",
				   "The AgsChannel it is assigned to.\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /* AgsThread */
  thread = (AgsThreadClass *) channel_thread;

  thread->start = ags_channel_thread_start;
  thread->run = ags_channel_thread_run;
  thread->stop = ags_channel_thread_stop;
}

void
ags_channel_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_channel_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_channel_thread_connect;
  connectable->disconnect = ags_channel_thread_disconnect;
}

void
ags_channel_thread_init(AgsChannelThread *channel_thread)
{
  AgsThread *thread;

  AgsConfig *config;

  gchar *str0, *str1;
  
  thread = (AgsThread *) channel_thread;

  config = ags_config_get_instance();
  
  str0 = ags_config_get_value(config,
			      AGS_CONFIG_SOUNDCARD,
			      "samplerate\0");
  str1 = ags_config_get_value(config,
			      AGS_CONFIG_SOUNDCARD,
			      "buffer_size\0");

  if(str0 == NULL || str1 == NULL){
    thread->freq = AGS_CHANNEL_THREAD_DEFAULT_JIFFIE;
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

  g_atomic_int_set(&(channel_thread->flags),
		   0);

  channel_thread->soundcard = NULL;
  
  /* start */
  pthread_mutexattr_init(&(channel_thread->wakeup_attr));
  pthread_mutexattr_settype(&(channel_thread->wakeup_attr),
			    PTHREAD_MUTEX_RECURSIVE);

  channel_thread->wakeup_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(channel_thread->wakeup_mutex,
		     &(channel_thread->wakeup_attr));
  
  channel_thread->wakeup_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(channel_thread->wakeup_cond, NULL);

  /* sync */
  pthread_mutexattr_init(&(channel_thread->done_attr));
  pthread_mutexattr_settype(&(channel_thread->done_attr),
			    PTHREAD_MUTEX_RECURSIVE);

  channel_thread->done_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(channel_thread->done_mutex,
		     &(channel_thread->done_attr));
  
  channel_thread->done_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(channel_thread->done_cond, NULL);
}

void
ags_channel_thread_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsChannelThread *channel_thread;

  channel_thread = AGS_CHANNEL_THREAD(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = g_value_get_object(value);

      if(channel_thread->soundcard != NULL){
	g_object_unref(channel_thread->soundcard);
      }

      if(soundcard != NULL){
	g_object_ref(soundcard);
      }

      channel_thread->soundcard = soundcard;
    }
    break;
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      if(channel_thread->channel != NULL){
	g_object_unref(G_OBJECT(channel_thread->channel));
      }

      if(channel != NULL){
	g_object_ref(G_OBJECT(channel));
      }

      channel_thread->channel = G_OBJECT(channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_channel_thread_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsChannelThread *channel_thread;

  channel_thread = AGS_CHANNEL_THREAD(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, channel_thread->soundcard);
    }
    break;
  case PROP_CHANNEL:
    {
      g_value_set_object(value, G_OBJECT(channel_thread->channel));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_channel_thread_connect(AgsConnectable *connectable)
{
  ags_channel_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_channel_thread_disconnect(AgsConnectable *connectable)
{
  ags_channel_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_channel_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_channel_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_channel_thread_start(AgsThread *thread)
{
  /* reset status */
  g_atomic_int_or(&(AGS_CHANNEL_THREAD(thread)->flags),
		   (AGS_CHANNEL_THREAD_WAIT |
		    AGS_CHANNEL_THREAD_DONE |
		    AGS_CHANNEL_THREAD_WAIT_SYNC |
		    AGS_CHANNEL_THREAD_DONE_SYNC));
  
  AGS_THREAD_CLASS(ags_channel_thread_parent_class)->start(thread);
}

void
ags_channel_thread_run(AgsThread *thread)
{
  AgsChannel *channel;
  AgsPlayback *playback;

  AgsMutexManager *mutex_manager;
  AgsChannelThread *channel_thread;
  AgsRecallID *current_recall_id;
  AgsThread *current_thread;
  
  gint stage;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;

  if(!thread->rt_setup){
    struct sched_param param;
    
    /* Declare ourself as a real time task */
    param.sched_priority = AGS_RT_PRIORITY;
      
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
      perror("sched_setscheduler failed\0");
    }

    thread->rt_setup = TRUE;
  }

  channel_thread = AGS_CHANNEL_THREAD(thread);

  if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0){
    pthread_mutex_lock(channel_thread->done_mutex);

    g_atomic_int_and(&(channel_thread->flags),
		     (~AGS_CHANNEL_THREAD_WAIT_SYNC));

    pthread_mutex_unlock(channel_thread->done_mutex);


    pthread_mutex_lock(channel_thread->wakeup_mutex);

    g_atomic_int_or(&(channel_thread->flags),
		    (AGS_CHANNEL_THREAD_DONE |
		     AGS_CHANNEL_THREAD_WAIT));

    pthread_mutex_unlock(channel_thread->wakeup_mutex);

    return;
  }
  
  //  thread->freq = AGS_SOUNDCARD(thread->soundcard)->delay[AGS_SOUNDCARD(thread->soundcard)->tic_counter] / AGS_SOUNDCARD(thread->soundcard)->delay_factor;

  channel = channel_thread->channel;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* lookup channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) channel);
      
  pthread_mutex_unlock(application_mutex);

  /* get soundcard play */
  pthread_mutex_lock(channel_mutex);

  playback = channel->playback;
  current_thread = playback->channel_thread[0];
  
  pthread_mutex_unlock(channel_mutex);
  
  /* start - wait until signaled */
  if(thread != current_thread){
    pthread_mutex_lock(channel_thread->wakeup_mutex);

    g_atomic_int_and(&(channel_thread->flags),
		     (~AGS_CHANNEL_THREAD_DONE));
  
    if((AGS_CHANNEL_THREAD_DONE & (g_atomic_int_get(&(channel_thread->flags)))) == 0 &&
       (AGS_CHANNEL_THREAD_WAIT & (g_atomic_int_get(&(channel_thread->flags)))) != 0){
      while((AGS_CHANNEL_THREAD_DONE & (g_atomic_int_get(&(channel_thread->flags)))) == 0 &&
	    (AGS_CHANNEL_THREAD_WAIT & (g_atomic_int_get(&(channel_thread->flags)))) != 0){
	pthread_cond_wait(channel_thread->wakeup_cond,
			  channel_thread->wakeup_mutex);
      }
    }
  
    g_atomic_int_or(&(channel_thread->flags),
		    (AGS_CHANNEL_THREAD_DONE |
		     AGS_CHANNEL_THREAD_WAIT));
  
    pthread_mutex_unlock(channel_thread->wakeup_mutex);
  }
    
  /* do channel processing */
  for(stage = 0; stage < 3; stage++){
    /* playback */
    pthread_mutex_lock(channel_mutex);

    current_thread = playback->channel_thread[0];
    current_recall_id = playback->recall_id[0];
    
    pthread_mutex_unlock(channel_mutex);

    if(thread == current_thread){
      ags_channel_recursive_play(channel, current_recall_id, stage);
    }

    /* sequencer */
    pthread_mutex_lock(channel_mutex);

    current_thread = playback->channel_thread[1];
    current_recall_id = playback->recall_id[1];
    
    pthread_mutex_unlock(channel_mutex);

    if(thread == current_thread){
      ags_channel_recursive_play(channel, current_recall_id, stage);
    }

    /* notation */
    pthread_mutex_lock(channel_mutex);

    current_thread = playback->channel_thread[2];
    current_recall_id = playback->recall_id[2];
    
    pthread_mutex_unlock(channel_mutex);

    if(thread == current_thread){
      ags_channel_recursive_play(channel, current_recall_id, stage);
    }
  }

  /* sync */
  pthread_mutex_lock(channel_thread->done_mutex);

  g_atomic_int_and(&(channel_thread->flags),
		   (~AGS_CHANNEL_THREAD_WAIT_SYNC));
	    
  if((AGS_CHANNEL_THREAD_DONE_SYNC & (g_atomic_int_get(&(channel_thread->flags)))) == 0){
    pthread_cond_signal(channel_thread->done_cond);
  }
  
  pthread_mutex_unlock(channel_thread->done_mutex);
}

void
ags_channel_thread_stop(AgsThread *thread)
{
  AgsChannelThread *channel_thread;

  /*  */
  channel_thread = AGS_CHANNEL_THREAD(thread);

  /* call parent */
  AGS_THREAD_CLASS(ags_channel_thread_parent_class)->stop(thread);

  /* ensure synced */
  pthread_mutex_lock(channel_thread->done_mutex);
  
  g_atomic_int_and(&(channel_thread->flags),
		   (~AGS_CHANNEL_THREAD_WAIT_SYNC));

  if((AGS_CHANNEL_THREAD_DONE_SYNC & (g_atomic_int_get(&(channel_thread->flags)))) == 0){
    pthread_cond_signal(channel_thread->done_cond);
  }

  pthread_mutex_unlock(channel_thread->done_mutex);
}

/**
 * ags_channel_thread_new:
 * @soundcard: the #GObject
 * @channel: the #AgsChannel
 *
 * Create a new #AgsChannelThread.
 *
 * Returns: the new #AgsChannelThread
 *
 * Since: 0.4.2
 */
AgsChannelThread*
ags_channel_thread_new(GObject *soundcard,
		       GObject *channel)
{
  AgsChannelThread *channel_thread;

  channel_thread = (AgsChannelThread *) g_object_new(AGS_TYPE_CHANNEL_THREAD,
						     "soundcard\0", soundcard,
						     "channel\0", channel,
						     NULL);


  return(channel_thread);
}
