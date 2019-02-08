/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/libags.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_playback.h>

#include <math.h>

#include <ags/i18n.h>

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
void ags_channel_thread_dispose(GObject *gobject);
void ags_channel_thread_finalize(GObject *gobject);

void ags_channel_thread_start(AgsThread *thread);
void ags_channel_thread_run(AgsThread *thread);
void ags_channel_thread_stop(AgsThread *thread);

/**
 * SECTION:ags_channel_thread
 * @short_description: channel thread
 * @title: AgsChannelThread
 * @section_id:
 * @include: ags/audio/thread/ags_channel_thread.h
 *
 * The #AgsChannelThread acts as channel output thread to soundcard.
 */

enum{
  PROP_0,
  PROP_DEFAULT_OUTPUT_SOUNDCARD,
  PROP_CHANNEL,
  PROP_SOUND_SCOPE,
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
						     "AgsChannelThread",
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

  gobject->dispose = ags_channel_thread_dispose;
  gobject->finalize = ags_channel_thread_finalize;

  /* properties */
  /**
   * AgsChannelThread:default-output-soundcard:
   *
   * The assigned default soundcard.
   * 
   * Since: 2.0.0
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
   * AgsChannelThread:channel:
   *
   * The assigned #AgsChannel.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("channel assigned to"),
				   i18n_pspec("The AgsChannel it is assigned to."),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
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
}

void
ags_channel_thread_init(AgsChannelThread *channel_thread)
{
  AgsThread *thread;

  AgsConfig *config;

  guint samplerate;
  guint buffer_size;
  
  thread = (AgsThread *) channel_thread;

  g_atomic_int_or(&(thread->flags),
		  (AGS_THREAD_START_SYNCED_FREQ));
  
  config = ags_config_get_instance();
  
  samplerate = ags_soundcard_helper_config_get_samplerate(config);
  buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  
  thread->freq = ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;  

  g_atomic_int_set(&(channel_thread->flags),
		   0);

  channel_thread->default_output_soundcard = NULL;
  
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

  /* channel and sound scope */
  channel_thread->channel = NULL;
  channel_thread->sound_scope = -1;
}

void
ags_channel_thread_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsChannelThread *channel_thread;

  pthread_mutex_t *thread_mutex;

  channel_thread = AGS_CHANNEL_THREAD(gobject);

  /* get thread mutex */
  pthread_mutex_lock(ags_thread_get_class_mutex());
  
  thread_mutex = AGS_THREAD(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_thread_get_class_mutex());

  switch(prop_id){
  case PROP_DEFAULT_OUTPUT_SOUNDCARD:
    {
      GObject *default_output_soundcard;

      default_output_soundcard = g_value_get_object(value);

      pthread_mutex_lock(thread_mutex);

      if(channel_thread->default_output_soundcard == default_output_soundcard){
	pthread_mutex_unlock(thread_mutex);

	return;
      }

      if(channel_thread->default_output_soundcard != NULL){
	g_object_unref(channel_thread->default_output_soundcard);
      }

      if(default_output_soundcard != NULL){
	g_object_ref(default_output_soundcard);
      }

      channel_thread->default_output_soundcard = default_output_soundcard;

      pthread_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      pthread_mutex_lock(thread_mutex);

      if(channel_thread->channel == (GObject *) channel){
	pthread_mutex_unlock(thread_mutex);
	
	return;
      }
      
      if(channel_thread->channel != NULL){
	g_object_unref(G_OBJECT(channel_thread->channel));
      }

      if(channel != NULL){
	g_object_ref(G_OBJECT(channel));
      }

      channel_thread->channel = (GObject *) channel;

      pthread_mutex_unlock(thread_mutex);
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

  pthread_mutex_t *thread_mutex;

  channel_thread = AGS_CHANNEL_THREAD(gobject);

  /* get thread mutex */
  pthread_mutex_lock(ags_thread_get_class_mutex());
  
  thread_mutex = AGS_THREAD(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_thread_get_class_mutex());

  switch(prop_id){
  case PROP_DEFAULT_OUTPUT_SOUNDCARD:
    {
      pthread_mutex_lock(thread_mutex);

      g_value_set_object(value, channel_thread->default_output_soundcard);

      pthread_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_CHANNEL:
    {
      pthread_mutex_lock(thread_mutex);

      g_value_set_object(value, G_OBJECT(channel_thread->channel));

      pthread_mutex_unlock(thread_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_channel_thread_dispose(GObject *gobject)
{
  AgsChannelThread *channel_thread;

  channel_thread = AGS_CHANNEL_THREAD(gobject);

  /* soundcard */
  if(channel_thread->default_output_soundcard != NULL){
    g_object_unref(channel_thread->default_output_soundcard);

    channel_thread->default_output_soundcard = NULL;
  }

  /* channel */
  if(channel_thread->channel != NULL){
    g_object_unref(channel_thread->channel);

    channel_thread->channel = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_channel_thread_parent_class)->dispose(gobject);
}

void
ags_channel_thread_finalize(GObject *gobject)
{
  AgsChannelThread *channel_thread;

  channel_thread = AGS_CHANNEL_THREAD(gobject);

  /* soundcard */
  if(channel_thread->default_output_soundcard != NULL){
    g_object_unref(channel_thread->default_output_soundcard);
  }

  /* wakeup mutex and cond */
  pthread_mutex_destroy(channel_thread->wakeup_mutex);
  free(channel_thread->wakeup_mutex);
  
  pthread_cond_destroy(channel_thread->wakeup_cond);
  free(channel_thread->wakeup_cond);

  /* sync mutex and cond */
  pthread_mutex_destroy(channel_thread->done_mutex);
  free(channel_thread->done_mutex);
  
  pthread_cond_destroy(channel_thread->done_cond);
  free(channel_thread->done_cond);

  /* channel */
  if(channel_thread->channel != NULL){
    g_object_unref(channel_thread->channel);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_channel_thread_parent_class)->finalize(gobject);
}

void
ags_channel_thread_start(AgsThread *thread)
{
  g_message("channel thread start");
  
  /* reset status */
  g_atomic_int_or(&(AGS_CHANNEL_THREAD(thread)->flags),
		   (AGS_CHANNEL_THREAD_WAIT |
		    AGS_CHANNEL_THREAD_DONE |
		    AGS_CHANNEL_THREAD_WAIT_SYNC |
		    AGS_CHANNEL_THREAD_DONE_SYNC));
  
  if((AGS_THREAD_SINGLE_LOOP & (g_atomic_int_get(&(thread->flags)))) == 0){
    AGS_THREAD_CLASS(ags_channel_thread_parent_class)->start(thread);
  }
}

void
ags_channel_thread_run(AgsThread *thread)
{
  AgsChannel *channel;
  AgsPlayback *playback;
  
  AgsChannelThread *channel_thread;

  GList *recall_id;
  
  gint sound_scope;

#ifdef AGS_WITH_RT
  if((AGS_THREAD_RT_SETUP & (g_atomic_int_get(&(thread->flags)))) == 0){
    struct sched_param param;
    
    /* Declare ourself as a real time task */
    param.sched_priority = AGS_RT_PRIORITY;
      
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
      perror("sched_setscheduler failed");
    }

    g_atomic_int_or(&(thread->flags),
		    AGS_THREAD_RT_SETUP);
  }
#endif

  if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0 ||
     (AGS_THREAD_SYNCED & (g_atomic_int_get(&(thread->sync_flags)))) == 0){
    return;
  }

  channel_thread = AGS_CHANNEL_THREAD(thread);

  g_object_get(channel_thread,
	       "channel", &channel,
	       NULL);

  g_object_unref(channel);
  
  g_object_get(channel,
	       "playback", &playback,
	        NULL);

  g_object_unref(playback);
  
  /* start - wait until signaled */
  pthread_mutex_lock(channel_thread->wakeup_mutex);

  if((AGS_CHANNEL_THREAD_DONE & (g_atomic_int_get(&(channel_thread->flags)))) == 0 &&
     (AGS_CHANNEL_THREAD_WAIT & (g_atomic_int_get(&(channel_thread->flags)))) != 0){

    g_atomic_int_and(&(channel_thread->flags),
		     (~AGS_CHANNEL_THREAD_DONE));
    
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
  
  /* 
   * do audio processing
   */  
  if(channel_thread->sound_scope >= 0){
    sound_scope = channel_thread->sound_scope;

    if(ags_playback_get_recall_id(playback, sound_scope) != NULL){    
      if((recall_id = ags_channel_check_scope(channel, sound_scope)) != NULL){
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
	  
	g_list_free(recall_id);
      }
    }
  }else{
    for(sound_scope = 0; sound_scope < AGS_SOUND_SCOPE_LAST; sound_scope++){
      if(ags_playback_get_recall_id(playback, sound_scope) == NULL){
	continue;
      }

      if((recall_id = ags_channel_check_scope(channel, sound_scope)) != NULL){
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
	  
	g_list_free(recall_id);
      }
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

  if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(thread->flags)))) == 0){
    return;
  }

  /*  */
  channel_thread = AGS_CHANNEL_THREAD(thread);

  g_message("channel thread stop");
  
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
 * ags_channel_thread_set_sound_scope:
 * @channel_thread: the #AgsChannelThread
 * @sound_scope: the sound scope
 * 
 * Set sound scope.
 * 
 * Since: 2.0.0
 */
void
ags_channel_thread_set_sound_scope(AgsChannelThread *channel_thread,
				   gint sound_scope)
{
  pthread_mutex_t *thread_mutex;
  
  if(!AGS_IS_CHANNEL_THREAD(channel_thread)){
    return;
  }

  pthread_mutex_lock(ags_thread_get_class_mutex());
  
  thread_mutex = AGS_THREAD(channel_thread)->obj_mutex;

  pthread_mutex_unlock(ags_thread_get_class_mutex());

  /* set scope */
  pthread_mutex_lock(thread_mutex);

  channel_thread->sound_scope = sound_scope;
  
  pthread_mutex_unlock(thread_mutex);
}

/**
 * ags_channel_thread_new:
 * @default_output_soundcard: the #GObject
 * @channel: the #AgsChannel
 *
 * Create a new #AgsChannelThread.
 *
 * Returns: the new #AgsChannelThread
 *
 * Since: 2.0.0
 */
AgsChannelThread*
ags_channel_thread_new(GObject *default_output_soundcard,
		       GObject *channel)
{
  AgsChannelThread *channel_thread;

  channel_thread = (AgsChannelThread *) g_object_new(AGS_TYPE_CHANNEL_THREAD,
						     "default-output-soundcard", default_output_soundcard,
						     "channel", channel,
						     NULL);


  return(channel_thread);
}
