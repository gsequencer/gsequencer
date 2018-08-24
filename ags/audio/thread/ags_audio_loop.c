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

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/libags.h>

#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>

#include <ags/audio/thread/ags_soundcard_thread.h>
#include <ags/audio/thread/ags_sequencer_thread.h>
#include <ags/audio/thread/ags_export_thread.h>
#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>

#include <ags/i18n.h>

void ags_audio_loop_class_init(AgsAudioLoopClass *audio_loop);
void ags_audio_loop_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_loop_main_loop_interface_init(AgsMainLoopInterface *main_loop);
void ags_audio_loop_init(AgsAudioLoop *audio_loop);
void ags_audio_loop_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_audio_loop_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_audio_loop_dispose(GObject *gobject);
void ags_audio_loop_finalize(GObject *gobject);

pthread_mutex_t* ags_audio_loop_get_tree_lock(AgsMainLoop *main_loop);
void ags_audio_loop_set_async_queue(AgsMainLoop *main_loop, GObject *async_queue);
GObject* ags_audio_loop_get_async_queue(AgsMainLoop *main_loop);
void ags_audio_loop_set_tic(AgsMainLoop *main_loop, guint tic);
guint ags_audio_loop_get_tic(AgsMainLoop *main_loop);
void ags_audio_loop_set_last_sync(AgsMainLoop *main_loop, guint last_sync);
guint ags_audio_loop_get_last_sync(AgsMainLoop *main_loop);
gboolean ags_audio_loop_monitor(AgsMainLoop *main_loop,
				guint time_cycle, guint *time_spent);
void ags_audio_loop_change_frequency(AgsMainLoop *main_loop,
				     gdouble frequency);

void ags_audio_loop_start(AgsThread *thread);
void ags_audio_loop_run(AgsThread *thread);

void ags_audio_loop_notify_frequency(GObject *gobject,
				     GParamSpec *pspec,
				     gpointer user_data);

void* ags_audio_loop_timing_thread(void *ptr);
void ags_audio_loop_play_channel(AgsAudioLoop *audio_loop);
void ags_audio_loop_play_channel_super_threaded(AgsAudioLoop *audio_loop,
						AgsPlayback *playback);
void ags_audio_loop_sync_channel_super_threaded(AgsAudioLoop *audio_loop,
						AgsPlayback *playback);
void ags_audio_loop_play_audio(AgsAudioLoop *audio_loop);
void ags_audio_loop_play_audio_super_threaded(AgsAudioLoop *audio_loop,
					      AgsPlaybackDomain *playback_domain);
void ags_audio_loop_sync_audio_super_threaded(AgsAudioLoop *audio_loop,
					      AgsPlaybackDomain *playback_domain);

/**
 * SECTION:ags_audio_loop
 * @short_description: audio loop
 * @title: AgsAudioLoop
 * @section_id:
 * @include: ags/audio/thread/ags_audio_loop.h
 *
 * The #AgsAudioLoop is suitable as #AgsMainLoop and does
 * audio processing.
 */

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_DEFAULT_OUTPUT_SOUNDCARD,
  PROP_PLAY_CHANNEL,
  PROP_PLAY_AUDIO,
};

static gpointer ags_audio_loop_parent_class = NULL;
static AgsConnectableInterface *ags_audio_loop_parent_connectable_interface;

GType
ags_audio_loop_get_type()
{
  static GType ags_type_audio_loop = 0;

  if(!ags_type_audio_loop){
    static const GTypeInfo ags_audio_loop_info = {
      sizeof (AgsAudioLoopClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_loop_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioLoop),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_loop_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_loop_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_main_loop_interface_info = {
      (GInterfaceInitFunc) ags_audio_loop_main_loop_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_loop = g_type_register_static(AGS_TYPE_THREAD,
						 "AgsAudioLoop",
						 &ags_audio_loop_info,
						 0);
    
    g_type_add_interface_static(ags_type_audio_loop,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_audio_loop,
				AGS_TYPE_MAIN_LOOP,
				&ags_main_loop_interface_info);
  }
  
  return (ags_type_audio_loop);
}

void
ags_audio_loop_class_init(AgsAudioLoopClass *audio_loop)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  GParamSpec *param_spec;

  ags_audio_loop_parent_class = g_type_class_peek_parent(audio_loop);

  /* GObject */
  gobject = (GObjectClass *) audio_loop;

  gobject->set_property = ags_audio_loop_set_property;
  gobject->get_property = ags_audio_loop_get_property;

  gobject->dispose = ags_audio_loop_dispose;
  gobject->finalize = ags_audio_loop_finalize;

  /* properties */
  /**
   * AgsDevout:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("application-context",
				   i18n_pspec("the application context object"),
				   i18n_pspec("The application context object"),
				   AGS_TYPE_APPLICATION_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /**
   * AgsAudioLoop:default-output-soundcard:
   *
   * The assigned default soundcard.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("default-output-soundcard",
				   i18n_pspec("default output soundcard assigned to"),
				   i18n_pspec("The default output AgsSoundcard it is assigned to"),
				   G_TYPE_OBJECT,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEFAULT_OUTPUT_SOUNDCARD,
				  param_spec);

  /**
   * AgsAudioLoop:play-channel:
   *
   * An #AgsChannel to add for playback.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("play-channel",
				    i18n_pspec("channel to run"),
				    i18n_pspec("A channel to run"),
				    G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY_CHANNEL,
				  param_spec);

  /**
   * AgsAudioLoop:play-audio:
   *
   * An #AgsAudio to add for playback.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("play-audio",
				    i18n_pspec("audio to run"),
				    i18n_pspec("A audio to run"),
				    G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY_AUDIO,
				  param_spec);

  /* AgsThread */
  thread = (AgsThreadClass *) audio_loop;
  
  thread->start = ags_audio_loop_start;
  thread->run = ags_audio_loop_run;

  /* AgsAudioLoop */
}

void
ags_audio_loop_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_audio_loop_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_audio_loop_main_loop_interface_init(AgsMainLoopInterface *main_loop)
{
  main_loop->get_tree_lock = ags_audio_loop_get_tree_lock;
  main_loop->set_async_queue = ags_audio_loop_set_async_queue;
  main_loop->get_async_queue = ags_audio_loop_get_async_queue;
  main_loop->set_tic = ags_audio_loop_set_tic;
  main_loop->get_tic = ags_audio_loop_get_tic;
  main_loop->set_last_sync = ags_audio_loop_set_last_sync;
  main_loop->get_last_sync = ags_audio_loop_get_last_sync;
  main_loop->interrupt = NULL;
  main_loop->monitor = ags_audio_loop_monitor;
  main_loop->change_frequency = ags_audio_loop_change_frequency;
}

void
ags_audio_loop_init(AgsAudioLoop *audio_loop)
{
  AgsMutexManager *mutex_manager;
  AgsThread *thread;

  AgsConfig *config;
  
  guint samplerate;
  guint buffer_size;

  pthread_mutex_t *application_mutex;

  thread = (AgsThread *) audio_loop;

  g_signal_connect_after(thread, "notify::frequency",
			 G_CALLBACK(ags_audio_loop_notify_frequency), NULL);

  g_atomic_int_or(&(thread->flags),
		  AGS_THREAD_TIMING);

  /* calculate frequency */
  config = ags_config_get_instance();

  samplerate = ags_soundcard_helper_config_get_samplerate(config);
  buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  
  thread->freq = ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;  

  audio_loop->flags = 0;
  
  g_atomic_int_set(&(audio_loop->tic), 0);
  g_atomic_int_set(&(audio_loop->last_sync), 0);

  audio_loop->time_cycle = NSEC_PER_SEC / thread->freq;
  g_atomic_int_set(&(audio_loop->time_spent), 0);
  
  audio_loop->application_context = NULL;
  audio_loop->default_output_soundcard = NULL;

  audio_loop->async_queue = NULL;
    
  /* tree lock mutex */
  audio_loop->tree_lock_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  
  pthread_mutexattr_init(audio_loop->tree_lock_mutexattr);
  pthread_mutexattr_settype(audio_loop->tree_lock_mutexattr, PTHREAD_MUTEX_RECURSIVE);

  audio_loop->tree_lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(audio_loop->tree_lock, audio_loop->tree_lock_mutexattr);
  
  /* recall mutex */
  audio_loop->recall_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(audio_loop->recall_mutex, NULL);

  /* timing thread */
  audio_loop->timing_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(audio_loop->timing_mutex, NULL);

  audio_loop->timing_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(audio_loop->timing_cond, NULL);

  audio_loop->timing_thread = (pthread_t *) malloc(sizeof(pthread_t));

  /* recall related lists */
  audio_loop->play_channel_ref = 0;
  audio_loop->play_channel = NULL;

  audio_loop->play_audio_ref = 0;
  audio_loop->play_audio = NULL;
}

void
ags_audio_loop_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsAudioLoop *audio_loop;

  pthread_mutex_t *thread_mutex;

  audio_loop = AGS_AUDIO_LOOP(gobject);

  /* get thread mutex */
  pthread_mutex_lock(ags_thread_get_class_mutex());
  
  thread_mutex = AGS_THREAD(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_thread_get_class_mutex());

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      pthread_mutex_lock(thread_mutex);

      if(audio_loop->application_context == (GObject *) application_context){
	pthread_mutex_unlock(thread_mutex);

	return;
      }

      if(audio_loop->application_context != NULL){
	g_object_unref(G_OBJECT(audio_loop->application_context));
      }

      if(application_context != NULL){
	g_object_ref(G_OBJECT(application_context));
      }
      
      audio_loop->application_context = (GObject *) application_context;

      pthread_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_DEFAULT_OUTPUT_SOUNDCARD:
    {
      GObject *default_output_soundcard;

      default_output_soundcard = g_value_get_object(value);

      pthread_mutex_lock(thread_mutex);

      if(audio_loop->default_output_soundcard == default_output_soundcard){
	pthread_mutex_unlock(thread_mutex);

	return;
      }

      if(audio_loop->default_output_soundcard != NULL){
	g_object_unref(audio_loop->default_output_soundcard);
      }

      if(default_output_soundcard != NULL){
	g_object_ref(default_output_soundcard);
      }

      audio_loop->default_output_soundcard = default_output_soundcard;

      pthread_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_PLAY_CHANNEL:
    {
      AgsPlayback *playback;

      playback = (AgsPlayback *) g_value_get_pointer(value);

      pthread_mutex_lock(thread_mutex);

      if(AGS_IS_PLAYBACK(playback) &&
	 g_list_find(audio_loop->play_channel, playback) == NULL){
	g_object_ref(playback);
	audio_loop->play_channel = g_list_prepend(audio_loop->play_channel,
						  playback);
	audio_loop->play_channel_ref = audio_loop->play_channel_ref + 1;
      }
    }
    break;
  case PROP_PLAY_AUDIO:
    {
      AgsPlaybackDomain *playback_domain;

      playback_domain = (AgsPlaybackDomain *) g_value_get_pointer(value);

      pthread_mutex_lock(thread_mutex);

      if(AGS_IS_PLAYBACK_DOMAIN(playback_domain) &&
	 g_list_find(audio_loop->play_audio, playback_domain) == NULL){
	audio_loop->play_audio = g_list_prepend(audio_loop->play_audio,
						playback_domain);
	g_object_ref(playback_domain);
	audio_loop->play_audio_ref = audio_loop->play_audio_ref + 1;
      }

      pthread_mutex_unlock(thread_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_loop_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsAudioLoop *audio_loop;

  pthread_mutex_t *thread_mutex;

  audio_loop = AGS_AUDIO_LOOP(gobject);

  /* get thread mutex */
  pthread_mutex_lock(ags_thread_get_class_mutex());
  
  thread_mutex = AGS_THREAD(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_thread_get_class_mutex());

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      pthread_mutex_lock(thread_mutex);

      g_value_set_object(value, audio_loop->application_context);

      pthread_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_DEFAULT_OUTPUT_SOUNDCARD:
    {
      pthread_mutex_lock(thread_mutex);

      g_value_set_object(value, audio_loop->default_output_soundcard);

      pthread_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_PLAY_CHANNEL:
    {
      pthread_mutex_lock(thread_mutex);

      g_value_set_pointer(value, g_list_copy(audio_loop->play_channel));

      pthread_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_PLAY_AUDIO:
    {
      pthread_mutex_lock(thread_mutex);

      g_value_set_pointer(value, g_list_copy(audio_loop->play_audio));

      pthread_mutex_unlock(thread_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_loop_dispose(GObject *gobject)
{
  AgsAudioLoop *audio_loop;

  audio_loop = AGS_AUDIO_LOOP(gobject);

  /* application context */
  if(audio_loop->application_context != NULL){
    g_object_unref(audio_loop->application_context);

    audio_loop->application_context = NULL;
  }

  /* soundcard */
  if(audio_loop->default_output_soundcard != NULL){
    g_object_unref(audio_loop->default_output_soundcard);

    audio_loop->default_output_soundcard = NULL;
  }

  /* async queue */
  if(audio_loop->async_queue != NULL){
    g_object_unref(audio_loop->async_queue);

    audio_loop->async_queue = NULL;
  }

  /* unref AgsPlayback lists */
  if(audio_loop->play_channel != NULL){
    g_list_free_full(audio_loop->play_channel,
		     g_object_unref);
    
    audio_loop->play_channel = NULL;
  }

  if(audio_loop->play_audio != NULL){
    g_list_free_full(audio_loop->play_audio,
		     g_object_unref);
    
    audio_loop->play_audio = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_loop_parent_class)->dispose(gobject);
}

void
ags_audio_loop_finalize(GObject *gobject)
{
  AgsAudioLoop *audio_loop;

  audio_loop = AGS_AUDIO_LOOP(gobject);

  /* application context */
  if(audio_loop->application_context != NULL){
    g_object_unref(audio_loop->application_context);
  }

  /* soundcard */
  if(audio_loop->default_output_soundcard != NULL){
    g_object_unref(audio_loop->default_output_soundcard);
  }

  /* async queue */
  if(audio_loop->async_queue != NULL){
    g_object_unref(audio_loop->async_queue);
  }

  /* tree lock and recall mutex */
  pthread_mutex_destroy(audio_loop->tree_lock);
  free(audio_loop->tree_lock);

  pthread_mutexattr_destroy(audio_loop->tree_lock_mutexattr);
  free(audio_loop->tree_lock_mutexattr);

  pthread_mutex_destroy(audio_loop->recall_mutex);
  free(audio_loop->recall_mutex);

  /* timing mutex and cond */
  pthread_mutex_destroy(audio_loop->timing_mutex);
  free(audio_loop->timing_mutex);

  pthread_cond_destroy(audio_loop->timing_cond);
  free(audio_loop->timing_cond);

  //FIXME:JK: destroy timing thread
  
  /* unref AgsPlayback lists */  
  g_list_free_full(audio_loop->play_channel,
		   g_object_unref);
  
  g_list_free_full(audio_loop->play_audio,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_audio_loop_parent_class)->finalize(gobject);
}

pthread_mutex_t*
ags_audio_loop_get_tree_lock(AgsMainLoop *main_loop)
{
  pthread_mutex_t *tree_lock;

  /* get tree lock mutex */
  pthread_mutex_lock(ags_thread_get_class_mutex());
  
  tree_lock = AGS_AUDIO_LOOP(main_loop)->tree_lock;
  
  pthread_mutex_unlock(ags_thread_get_class_mutex());
  
  return(tree_lock);
}

void
ags_audio_loop_set_async_queue(AgsMainLoop *main_loop, GObject *async_queue)
{
  pthread_mutex_t *thread_mutex;

  /* get thread mutex */
  pthread_mutex_lock(ags_thread_get_class_mutex());
  
  thread_mutex = AGS_THREAD(main_loop)->obj_mutex;
  
  pthread_mutex_unlock(ags_thread_get_class_mutex());

  /* set */
  pthread_mutex_lock(thread_mutex);
  
  if(AGS_AUDIO_LOOP(main_loop)->async_queue == async_queue){
    pthread_mutex_unlock(thread_mutex);
    
    return;
  }

  if(AGS_AUDIO_LOOP(main_loop)->async_queue != NULL){
    g_object_unref(AGS_AUDIO_LOOP(main_loop)->async_queue);
  }
  
  if(async_queue != NULL){
    g_object_ref(async_queue);
  }
  
  AGS_AUDIO_LOOP(main_loop)->async_queue = async_queue;

  pthread_mutex_unlock(thread_mutex);
}

GObject*
ags_audio_loop_get_async_queue(AgsMainLoop *main_loop)
{
  GObject *async_queue;
  
  pthread_mutex_t *thread_mutex;

  /* get thread mutex */
  pthread_mutex_lock(ags_thread_get_class_mutex());
  
  thread_mutex = AGS_THREAD(main_loop)->obj_mutex;
  
  pthread_mutex_unlock(ags_thread_get_class_mutex());

  /* get */
  pthread_mutex_lock(thread_mutex);
  
  async_queue = AGS_AUDIO_LOOP(main_loop)->async_queue;
  
  pthread_mutex_unlock(thread_mutex);

  return(async_queue);
}

void
ags_audio_loop_set_tic(AgsMainLoop *main_loop, guint tic)
{
  g_atomic_int_set(&(AGS_AUDIO_LOOP(main_loop)->tic),
		   tic);
}

guint
ags_audio_loop_get_tic(AgsMainLoop *main_loop)
{
  return(g_atomic_int_get(&(AGS_AUDIO_LOOP(main_loop)->tic)));
}

void
ags_audio_loop_set_last_sync(AgsMainLoop *main_loop, guint last_sync)
{
  g_atomic_int_set(&(AGS_AUDIO_LOOP(main_loop)->last_sync),
		   last_sync);
}

guint
ags_audio_loop_get_last_sync(AgsMainLoop *main_loop)
{
  gint val;

  val = g_atomic_int_get(&(AGS_AUDIO_LOOP(main_loop)->last_sync));

  return(val);
}

gboolean
ags_audio_loop_monitor(AgsMainLoop *main_loop,
		       guint time_cycle, guint *time_spent)
{
  if(g_atomic_int_get(&(AGS_AUDIO_LOOP(main_loop)->time_spent)) == 0){
    return(TRUE);
  }else{
    return(FALSE);
  }
}

void
ags_audio_loop_change_frequency(AgsMainLoop *main_loop,
				gdouble frequency)
{
  AgsThread *audio_loop, *thread;

  audio_loop = AGS_THREAD(main_loop);
  
  g_object_set(audio_loop,
	       "frequency", frequency,
	       NULL);

  /* reset soundcard thread */
  thread = audio_loop;

  while(ags_thread_find_type(thread, AGS_TYPE_SOUNDCARD_THREAD) != NULL){
    g_object_set(thread,
		 "frequency", frequency,
		 NULL);

    thread = g_atomic_pointer_get(&(thread->next));
  }

  /* reset sequencer thread */
  thread = audio_loop;

  while(ags_thread_find_type(thread, AGS_TYPE_SEQUENCER_THREAD) != NULL){
    g_object_set(thread,
		 "frequency", frequency,
		 NULL);

    thread = g_atomic_pointer_get(&(thread->next));
  }

  /* reset export thread */
  thread = audio_loop;

  while(ags_thread_find_type(thread, AGS_TYPE_EXPORT_THREAD) != NULL){
    g_object_set(thread,
		 "frequency", frequency,
		 NULL);

    thread = g_atomic_pointer_get(&(thread->next));
  }

  /* reset audio thread */
  thread = audio_loop;

  while(ags_thread_find_type(thread, AGS_TYPE_AUDIO_THREAD) != NULL){
    g_object_set(thread,
		 "frequency", frequency,
		 NULL);

    thread = g_atomic_pointer_get(&(thread->next));
  }

  /* reset channel thread */
  thread = audio_loop;

  while(ags_thread_find_type(thread, AGS_TYPE_CHANNEL_THREAD) != NULL){
    g_object_set(thread,
		 "frequency", frequency,
		 NULL);

    thread = g_atomic_pointer_get(&(thread->next));
  }
}

void
ags_audio_loop_start(AgsThread *thread)
{
  AgsAudioLoop *audio_loop;
  
  audio_loop = AGS_AUDIO_LOOP(thread);

  if((AGS_THREAD_SINGLE_LOOP & (g_atomic_int_get(&(thread->flags)))) == 0){
    /*  */
    AGS_THREAD_CLASS(ags_audio_loop_parent_class)->start(thread);

    //    pthread_create(audio_loop->timing_thread, NULL,
    //		   ags_audio_loop_timing_thread, audio_loop);
  }
}

void
ags_audio_loop_run(AgsThread *thread)
{
  AgsAudioLoop *audio_loop;

  AgsPollingThread *polling_thread;
  
  audio_loop = AGS_AUDIO_LOOP(thread);
  
  polling_thread = (AgsPollingThread *) ags_thread_find_type(thread,
							     AGS_TYPE_POLLING_THREAD);
  
  /* real-time setup */
  if((AGS_THREAD_RT_SETUP & (g_atomic_int_get(&(thread->flags)))) == 0){
#ifdef AGS_WITH_RT
    struct sched_param param;
    
    /* Declare ourself as a real time task */
    param.sched_priority = AGS_RT_PRIORITY;
      
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
      perror("sched_setscheduler failed");
    }
#endif

    g_atomic_int_or(&(thread->flags),
		    AGS_THREAD_RT_SETUP);
  }

  /* reset polling thread */
  if(polling_thread != NULL){
    pthread_mutex_lock(polling_thread->fd_mutex);

    if(g_atomic_int_get(&(polling_thread->omit_count)) != 0){
      g_atomic_int_dec_and_test(&(polling_thread->omit_count));

      if(g_atomic_int_get(&(polling_thread->omit_count)) == 0){
	g_atomic_int_and(&(polling_thread->flags),
			 (~AGS_POLLING_THREAD_OMIT));
      }
    }else{
      g_atomic_int_and(&(polling_thread->flags),
		       (~AGS_POLLING_THREAD_OMIT));
    }
    
    pthread_mutex_unlock(polling_thread->fd_mutex);
  }
  
  /* wake-up timing thread */
  pthread_mutex_lock(audio_loop->timing_mutex);
  
  g_atomic_int_set(&(audio_loop->time_spent),
		   0);

  g_atomic_int_or(&(audio_loop->timing_flags),
		  AGS_AUDIO_LOOP_TIMING_WAKEUP);
  pthread_cond_signal(audio_loop->timing_cond);

  pthread_mutex_unlock(audio_loop->timing_mutex);

  /*  */
  pthread_mutex_lock(audio_loop->recall_mutex);

  /* play channel */
  if((AGS_AUDIO_LOOP_PLAY_CHANNEL & (audio_loop->flags)) != 0){    
    ags_audio_loop_play_channel(audio_loop);

    if(audio_loop->play_channel_ref == 0 &&
       g_atomic_pointer_get(&(thread->start_queue)) == NULL){
      audio_loop->flags &= (~AGS_AUDIO_LOOP_PLAY_CHANNEL);
    }
  }

  /* play audio */
  if((AGS_AUDIO_LOOP_PLAY_AUDIO & (audio_loop->flags)) != 0){
    ags_audio_loop_play_audio(audio_loop);

    if(audio_loop->play_audio_ref == 0 &&
       g_atomic_pointer_get(&(thread->start_queue)) == NULL){
      audio_loop->flags &= (~AGS_AUDIO_LOOP_PLAY_AUDIO);
    }
  }
  
  pthread_mutex_unlock(audio_loop->recall_mutex);

  /* decide if we stop */
  if(audio_loop->play_channel_ref == 0 &&
     audio_loop->play_audio_ref == 0){
    AgsThread *soundcard_thread;
    AgsThread *sequencer_thread;
    AgsThread *export_thread;

    /* soundcard thread */
    soundcard_thread = thread;

    while((soundcard_thread = ags_thread_find_type(soundcard_thread,
						   AGS_TYPE_SOUNDCARD_THREAD)) != NULL){
      if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(soundcard_thread->flags)))) != 0){
	ags_thread_stop(soundcard_thread);
      }

      soundcard_thread = g_atomic_pointer_get(&(soundcard_thread->next));
    }

    /* sequencer thread */
    sequencer_thread = thread;

    while((sequencer_thread = ags_thread_find_type(sequencer_thread,
						   AGS_TYPE_SEQUENCER_THREAD)) != NULL){
      if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(sequencer_thread->flags)))) != 0){
	ags_thread_stop(sequencer_thread);
      }

      sequencer_thread = g_atomic_pointer_get(&(sequencer_thread->next));
    }

    /* export thread */
    export_thread = thread;

    while((export_thread = ags_thread_find_type(export_thread,
						AGS_TYPE_EXPORT_THREAD)) != NULL){
      if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(export_thread->flags)))) != 0){
	ags_thread_stop(export_thread);
      }

      export_thread = g_atomic_pointer_get(&(export_thread->next));
    }
  }
}

void
ags_audio_loop_notify_frequency(GObject *gobject,
				GParamSpec *pspec,
				gpointer user_data)
{
  AGS_AUDIO_LOOP(gobject)->time_cycle = NSEC_PER_SEC / AGS_THREAD(gobject)->freq;
}

void*
ags_audio_loop_timing_thread(void *ptr)
{
  AgsAudioLoop *audio_loop;

  AgsThread *thread;

  struct timespec idle;

  guint time_spent;
  
  pthread_mutex_t *timing_mutex;
  pthread_cond_t *timing_cond;
  
  audio_loop = AGS_AUDIO_LOOP(ptr);
  thread = (AgsThread *) audio_loop;

  timing_mutex = audio_loop->timing_mutex;
  timing_cond = audio_loop->timing_cond;
  
  while((AGS_THREAD_RUNNING & (g_atomic_int_get(&(thread->flags)))) != 0){
    pthread_mutex_lock(timing_mutex);

    if((AGS_AUDIO_LOOP_TIMING_WAKEUP & (g_atomic_int_get(&(audio_loop->timing_flags)))) == 0){
      g_atomic_int_or(&(audio_loop->timing_flags),
		      AGS_AUDIO_LOOP_TIMING_WAITING);
      
      while((AGS_AUDIO_LOOP_TIMING_WAKEUP & (g_atomic_int_get(&(audio_loop->timing_flags)))) == 0 &&
	    (AGS_AUDIO_LOOP_TIMING_WAITING & (g_atomic_int_get(&(audio_loop->timing_flags)))) != 0){
	pthread_cond_wait(timing_cond,
			  timing_mutex);
      }

      g_atomic_int_and(&(audio_loop->timing_flags),
		       ~(AGS_AUDIO_LOOP_TIMING_WAITING |
			 AGS_AUDIO_LOOP_TIMING_WAKEUP)); 
    }
    
    pthread_mutex_unlock(timing_mutex);

    idle.tv_sec = 0;
    idle.tv_nsec = audio_loop->time_cycle - 4000; //NOTE:JK: 4 usec tolerance
    
    nanosleep(&idle,
	      NULL);

    g_atomic_int_set(&(audio_loop->time_spent),
		     audio_loop->time_cycle);
    //    ags_main_loop_interrupt(AGS_MAIN_LOOP(thread),
    //			    AGS_THREAD_SUSPEND_SIG,
    //			    audio_loop->time_cycle, &time_spent);
    
    //    g_message("inter");    
  }
  
  pthread_exit(NULL);
}

/**
 * ags_audio_loop_play_channel:
 * @audio_loop: an #AgsAudioLoop
 *
 * Invokes ags_channel_recursive_run_stage() for all scopes containing #AgsRecallID.
 *
 * Since: 2.0.0
 */
void
ags_audio_loop_play_channel(AgsAudioLoop *audio_loop)
{
  AgsPlayback *playback;
  AgsChannel *channel;

  GList *list_play_start, *list_play;
  GList *recall_id;

  gint sound_scope;
  static const guint playback_staging_flags = (AGS_SOUND_STAGING_RESET |
					       AGS_SOUND_STAGING_FEED_INPUT_QUEUE |
					       AGS_SOUND_STAGING_AUTOMATE |
					       AGS_SOUND_STAGING_RUN_PRE |
					       AGS_SOUND_STAGING_RUN_INTER |
					       AGS_SOUND_STAGING_RUN_POST |
					       AGS_SOUND_STAGING_DO_FEEDBACK |
					       AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE);
  
  if(audio_loop->play_channel == NULL){
    if((AGS_AUDIO_LOOP_PLAY_CHANNEL_TERMINATING & (audio_loop->flags)) != 0){
      audio_loop->flags &= (~(AGS_AUDIO_LOOP_PLAY_CHANNEL |
			      AGS_AUDIO_LOOP_PLAY_CHANNEL_TERMINATING));
    }else{
      audio_loop->flags |= AGS_AUDIO_LOOP_PLAY_CHANNEL_TERMINATING;
    }
  }

  /* entry point */
  audio_loop->flags &= (~AGS_AUDIO_LOOP_PLAY_CHANNEL_TERMINATING);
  audio_loop->flags |= AGS_AUDIO_LOOP_PLAYING_CHANNEL;
  
  /* run the 3 stages */
  list_play =
    list_play_start = g_list_copy(audio_loop->play_channel);
  
  while(list_play != NULL){
    playback = (AgsPlayback *) list_play->data;

    g_object_get(playback,
		 "channel", &channel,
		 NULL);
    
    /* play */
    if(ags_playback_test_flags(playback, AGS_PLAYBACK_SUPER_THREADED_CHANNEL)){
      /* super threaded */
      ags_audio_loop_play_channel_super_threaded(audio_loop,
						 playback);
    }else{
      /* not super threaded */
      sound_scope = AGS_SOUND_SCOPE_PLAYBACK;
      
      if((recall_id = ags_channel_check_scope(channel, sound_scope)) != NULL){
	ags_channel_recursive_run_stage(channel,
					sound_scope, playback_staging_flags);

	g_list_free(recall_id);
      }
    }

    /* iterate */
    list_play = list_play->next;
  }

  /* sync channel */
  list_play = list_play_start;
  
  while(list_play != NULL){    
    playback = (AgsPlayback *) list_play->data;

    /* sync */
    if(ags_playback_test_flags(playback, AGS_PLAYBACK_SUPER_THREADED_CHANNEL)){
      /* super threaded */
      ags_audio_loop_sync_channel_super_threaded(audio_loop,
						 playback);
    }

    /* iterate */
    list_play = list_play->next;
  }

  g_list_free(list_play_start);
}

void
ags_audio_loop_play_channel_super_threaded(AgsAudioLoop *audio_loop,
					   AgsPlayback *playback)
{
  AgsChannel *channel;
  AgsChannelThread *channel_thread;

  AgsThread *thread;

  GList *recall_id;
  
  gint sound_scope;

  g_object_get(playback,
	       "channel", &channel,
	       NULL);
  
  for(sound_scope = 0; sound_scope < AGS_SOUND_SCOPE_LAST; sound_scope++){    
    if((recall_id = ags_channel_check_scope(channel, sound_scope)) != NULL){
      thread = ags_playback_get_channel_thread(playback,
					       sound_scope);
      channel_thread = (AgsChannelThread *) thread;
      
      if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(thread->flags)))) != 0 &&
	 (AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) == 0){
	/* wakeup wait */
	pthread_mutex_lock(channel_thread->wakeup_mutex);

	g_atomic_int_and(&(channel_thread->flags),
			 (~AGS_CHANNEL_THREAD_WAIT));
	    
	if((AGS_CHANNEL_THREAD_DONE & (g_atomic_int_get(&(channel_thread->flags)))) == 0){
	  pthread_cond_signal(channel_thread->wakeup_cond);
	}
	    
	pthread_mutex_unlock(channel_thread->wakeup_mutex);
      }

      g_list_free(recall_id);
    }
  }
}

void
ags_audio_loop_sync_channel_super_threaded(AgsAudioLoop *audio_loop,
					   AgsPlayback *playback)
{
  AgsChannel *channel;
  AgsChannelThread *channel_thread;

  AgsThread *thread;

  GList *recall_id;
  
  gint sound_scope;

  g_object_get(playback,
	       "channel", &channel,
	       NULL);

  for(sound_scope = 0; sound_scope < AGS_SOUND_SCOPE_LAST; sound_scope++){    
    if((recall_id = ags_channel_check_scope(channel, sound_scope)) != NULL){
      thread = ags_playback_get_channel_thread(playback,
					       sound_scope);
      channel_thread = (AgsChannelThread *) thread;
	
      pthread_mutex_lock(channel_thread->done_mutex);
  
      if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(thread->flags)))) != 0 &&
	 (AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) == 0 &&
	 (AGS_THREAD_INITIAL_SYNC & (g_atomic_int_get(&(thread->flags)))) == 0){      
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

      g_list_free(recall_id);
    }
  }
}

/**
 * ags_audio_loop_play_audio:
 * @audio_loop: an #AgsAudioLoop
 *
 * Invokes ags_audio_recursive_run_stage() for all scopes containing #AgsRecallID.
 *
 * Since: 2.0.0
 */
void
ags_audio_loop_play_audio(AgsAudioLoop *audio_loop)
{
  AgsPlaybackDomain *playback_domain;
  AgsAudio *audio;
  
  GList *list_play_domain_start, *list_play_domain;
  GList *recall_id;
  
  gint sound_scope;

  if(audio_loop->play_audio == NULL){
    if((AGS_AUDIO_LOOP_PLAY_AUDIO_TERMINATING & (audio_loop->flags)) != 0){
      audio_loop->flags &= (~(AGS_AUDIO_LOOP_PLAY_AUDIO |
			      AGS_AUDIO_LOOP_PLAY_AUDIO_TERMINATING));
    }else{
      audio_loop->flags |= AGS_AUDIO_LOOP_PLAY_AUDIO_TERMINATING;
    }
  }

  audio_loop->flags &= (~AGS_AUDIO_LOOP_PLAY_AUDIO_TERMINATING);
  audio_loop->flags |= AGS_AUDIO_LOOP_PLAYING_AUDIO;

  /* playing */
  list_play_domain =
    list_play_domain_start = g_list_copy(audio_loop->play_audio);

  while(list_play_domain != NULL){
    playback_domain = (AgsPlaybackDomain *) list_play_domain->data;

    g_object_get(playback_domain,
		 "audio", &audio,
		 NULL);
    
    /* play */
    if(ags_playback_domain_test_flags(playback_domain, AGS_PLAYBACK_DOMAIN_SUPER_THREADED_AUDIO)){
      /* super threaded */
      ags_audio_loop_play_audio_super_threaded(audio_loop,
					       playback_domain);
    }else{
      /* not super threaded */
      for(sound_scope = 0; sound_scope < AGS_SOUND_SCOPE_LAST; sound_scope++){
	if(sound_scope == AGS_SOUND_SCOPE_PLAYBACK){
	  continue;
	}
	
	if((recall_id = ags_audio_check_scope(audio, sound_scope)) != NULL){
	  ags_audio_recursive_run_stage(audio,
					sound_scope, (AGS_SOUND_STAGING_RESET |
						      AGS_SOUND_STAGING_FEED_INPUT_QUEUE |
						      AGS_SOUND_STAGING_AUTOMATE |
						      AGS_SOUND_STAGING_RUN_PRE));

	  ags_audio_recursive_run_stage(audio,
					sound_scope, (AGS_SOUND_STAGING_RUN_INTER));

	  ags_audio_recursive_run_stage(audio,
					sound_scope, (AGS_SOUND_STAGING_RUN_POST |
						      AGS_SOUND_STAGING_DO_FEEDBACK |
						      AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE));

	  g_list_free(recall_id);
	}
      }
    }
    
    /* iterate */
    list_play_domain = list_play_domain->next;
  }
  
  /* sync audio */
  list_play_domain = list_play_domain_start;

  while(list_play_domain != NULL){
    playback_domain = (AgsPlaybackDomain *) list_play_domain->data;

    /* sync */
    if(ags_playback_domain_test_flags(playback_domain, AGS_PLAYBACK_DOMAIN_SUPER_THREADED_AUDIO)){
      ags_audio_loop_sync_audio_super_threaded(audio_loop,
					       playback_domain);
    }
    
    /* iterate */
    list_play_domain = list_play_domain->next;
  }

  g_list_free(list_play_domain_start);
}
 
/**
 * ags_audio_loop_play_audio_super_threaded:
 * @audio_loop: the #AgsAudioLoop
 * @playback_domain: an #AgsPlaybackDomain
 *
 * Play audio super-threaded.
 *
 * Since: 2.0.0
 */
void
ags_audio_loop_play_audio_super_threaded(AgsAudioLoop *audio_loop, AgsPlaybackDomain *playback_domain)
{
  AgsAudio *audio;
  AgsAudioThread *audio_thread;
  
  AgsThread *thread;
  
  GList *recall_id;
  
  gint sound_scope;

  g_object_get(playback_domain,
	       "audio", &audio,
	       NULL);
  
  for(sound_scope = 0; sound_scope < AGS_SOUND_SCOPE_LAST; sound_scope++){    
    if((recall_id = ags_audio_check_scope(audio, sound_scope)) != NULL){
      thread = ags_playback_domain_get_audio_thread(playback_domain,
						    sound_scope);

      if(thread != NULL){
	audio_thread = (AgsAudioThread *) thread;
	
	if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(thread->flags)))) != 0 &&
	   (AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) == 0){
	  /* wakeup wait */
	  pthread_mutex_lock(audio_thread->wakeup_mutex);
      
	  g_atomic_int_and(&(audio_thread->flags),
			   (~AGS_AUDIO_THREAD_WAIT));
      
	  if((AGS_AUDIO_THREAD_DONE & (g_atomic_int_get(&(audio_thread->flags)))) == 0){
	    pthread_cond_signal(audio_thread->wakeup_cond);
	  }
      
	  pthread_mutex_unlock(audio_thread->wakeup_mutex);
	}
      }

      g_list_free(recall_id);
    }
  }
}

/**
 * ags_audio_loop_sync_audio_super_threaded:
 * @audio_loop: the #AgsAudioLoop
 * @playback_domain: an #AgsPlaybackDomain
 *
 * Sync audio super-threaded.
 *
 * Since: 2.0.0
 */
void
ags_audio_loop_sync_audio_super_threaded(AgsAudioLoop *audio_loop, AgsPlaybackDomain *playback_domain)
{
  AgsAudio *audio;
  AgsAudioThread *audio_thread;

  AgsThread *thread;

  GList *recall_id;
  
  gint sound_scope;

  g_object_get(playback_domain,
	       "audio", &audio,
	       NULL);
  
  for(sound_scope = 0; sound_scope < AGS_SOUND_SCOPE_LAST; sound_scope++){    
    if((recall_id = ags_audio_check_scope(audio, sound_scope)) != NULL){
      thread = ags_playback_domain_get_audio_thread(playback_domain,
						    sound_scope);

      if(thread != NULL){
	audio_thread = thread;

	pthread_mutex_lock(audio_thread->done_mutex);

	if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(thread->flags)))) != 0 &&
	   (AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) == 0){
	  if((AGS_AUDIO_THREAD_WAIT_SYNC & (g_atomic_int_get(&(audio_thread->flags)))) != 0){
	    g_atomic_int_and(&(audio_thread->flags),
			     (~AGS_AUDIO_THREAD_DONE_SYNC));

	    while((AGS_AUDIO_THREAD_DONE_SYNC & (g_atomic_int_get(&(audio_thread->flags)))) == 0 &&
		  (AGS_AUDIO_THREAD_WAIT_SYNC & (g_atomic_int_get(&(audio_thread->flags)))) != 0){
	      pthread_cond_wait(audio_thread->done_cond,
				audio_thread->done_mutex);
	    }
	  }
    
	  g_atomic_int_or(&(audio_thread->flags),
			  (AGS_AUDIO_THREAD_WAIT_SYNC |
			   AGS_AUDIO_THREAD_DONE_SYNC));
      	  
	  pthread_mutex_unlock(audio_thread->done_mutex);
	}
      }

      g_list_free(recall_id);
    }
  }
}

/**
 * ags_audio_loop_add_audio:
 * @audio_loop: the #AgsAudioLoop
 * @audio: an #AgsAudio
 *
 * Add audio for playback.
 *
 * Since: 2.0.0
 */
void
ags_audio_loop_add_audio(AgsAudioLoop *audio_loop, GObject *audio)
{
  if(g_list_find(audio_loop->play_audio,
		 AGS_AUDIO(audio)->playback_domain) == NULL){
    g_object_ref(G_OBJECT(audio));
    audio_loop->play_audio = g_list_prepend(audio_loop->play_audio,
					    AGS_AUDIO(audio)->playback_domain);

    audio_loop->play_audio_ref = audio_loop->play_audio_ref + 1;
  }
}

/**
 * ags_audio_loop_remove_audio:
 * @audio_loop: the #AgsAudioLoop
 * @audio: an #AgsAudio
 *
 * Remove audio of playback.
 *
 * Since: 2.0.0
 */
void
ags_audio_loop_remove_audio(AgsAudioLoop *audio_loop, GObject *audio)
{  
  if(g_list_find(audio_loop->play_audio,
		 AGS_AUDIO(audio)->playback_domain) != NULL){
    audio_loop->play_audio = g_list_remove(audio_loop->play_audio,
					   AGS_AUDIO(audio)->playback_domain);
    audio_loop->play_audio_ref = audio_loop->play_audio_ref - 1;
    
    g_object_unref(audio);
  }
}

/**
 * ags_audio_loop_add_channel:
 * @audio_loop: the #AgsAudioLoop
 * @channel: an #AgsChannel
 *
 * Add channel for playback.
 *
 * Since: 2.0.0
 */
void
ags_audio_loop_add_channel(AgsAudioLoop *audio_loop, GObject *channel)
{
  if(g_list_find(audio_loop->play_channel,
		 AGS_CHANNEL(channel)->playback) == NULL){
    g_object_ref(G_OBJECT(channel));
    audio_loop->play_channel = g_list_prepend(audio_loop->play_channel,
					      AGS_CHANNEL(channel)->playback);

    audio_loop->play_channel_ref = audio_loop->play_channel_ref + 1;
  }
}

/**
 * ags_audio_loop_remove_channel:
 * @audio_loop: the #AgsAudioLoop
 * @channel: an #AgsChannel
 *
 * Remove channel of playback.
 *
 * Since: 2.0.0
 */
void
ags_audio_loop_remove_channel(AgsAudioLoop *audio_loop, GObject *channel)
{
  if(g_list_find(audio_loop->play_channel,
		 AGS_CHANNEL(channel)->playback) != NULL){
    audio_loop->play_channel = g_list_remove(audio_loop->play_channel,
					     AGS_CHANNEL(channel)->playback);
    audio_loop->play_channel_ref = audio_loop->play_channel_ref - 1;

    g_object_unref(channel);
  }
}

/**
 * ags_audio_loop_new:
 * @soundcard: the #GObject
 * @application_context: the #AgsApplicationContext
 *
 * Create a new #AgsAudioLoop.
 *
 * Returns: the new #AgsAudioLoop
 *
 * Since: 2.0.0
 */
AgsAudioLoop*
ags_audio_loop_new(GObject *soundcard, GObject *application_context)
{
  AgsAudioLoop *audio_loop;

  audio_loop = (AgsAudioLoop *) g_object_new(AGS_TYPE_AUDIO_LOOP,
					     "default-output-soundcard", soundcard,
					     NULL);

  if(application_context != NULL){
    g_object_ref(G_OBJECT(application_context));
    audio_loop->application_context = application_context;
  }

  return(audio_loop);
}
