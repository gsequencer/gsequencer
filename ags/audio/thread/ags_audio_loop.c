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

#include <ags/audio/thread/ags_audio_loop.h>

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

GRecMutex* ags_audio_loop_get_tree_lock(AgsMainLoop *main_loop);
void ags_audio_loop_set_syncing(AgsMainLoop *main_loop, gboolean is_syncing);
gboolean ags_audio_loop_is_syncing(AgsMainLoop *main_loop);
void ags_audio_loop_set_critical_region(AgsMainLoop *main_loop, gboolean is_critical_region);
gboolean ags_audio_loop_is_critical_region(AgsMainLoop *main_loop);
void ags_audio_loop_inc_queued_critical_region(AgsMainLoop *main_loop);
void ags_audio_loop_dec_queued_critical_region(AgsMainLoop *main_loop);
guint ags_audio_loop_test_queued_critical_region(AgsMainLoop *main_loop);
void ags_audio_loop_change_frequency(AgsMainLoop *main_loop,
				     gdouble frequency);

void ags_audio_loop_start(AgsThread *thread);
void ags_audio_loop_run(AgsThread *thread);

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
  PROP_PLAY_CHANNEL,
  PROP_PLAY_AUDIO,
};

static gpointer ags_audio_loop_parent_class = NULL;
static AgsConnectableInterface *ags_audio_loop_parent_connectable_interface;

GType
ags_audio_loop_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audio_loop = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audio_loop);
  }

  return g_define_type_id__volatile;
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
   * AgsAudioLoop:play-channel: (type GList(AgsPlayback)) (transfer full)
   *
   * An #AgsChannel to add for playback.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("play-channel",
				    i18n_pspec("channel to run"),
				    i18n_pspec("A channel to run"),
				    G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY_CHANNEL,
				  param_spec);

  /**
   * AgsAudioLoop:play-audio: (type GList(AgsPlaybackDomain)) (transfer full)
   *
   * An #AgsAudio to add for playback.
   * 
   * Since: 3.0.0
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

  main_loop->set_syncing = ags_audio_loop_set_syncing;
  main_loop->is_syncing = ags_audio_loop_is_syncing;

  main_loop->set_critical_region = ags_audio_loop_set_critical_region;
  main_loop->is_critical_region = ags_audio_loop_is_critical_region;

  main_loop->inc_queued_critical_region = ags_audio_loop_inc_queued_critical_region;
  main_loop->dec_queued_critical_region = ags_audio_loop_dec_queued_critical_region;
  main_loop->test_queued_critical_region = ags_audio_loop_test_queued_critical_region;

  main_loop->change_frequency = ags_audio_loop_change_frequency;
}

void
ags_audio_loop_init(AgsAudioLoop *audio_loop)
{
  AgsThread *thread;

  AgsConfig *config;

  gdouble frequency;
  guint samplerate;
  guint buffer_size;
  guint i;
  
  thread = (AgsThread *) audio_loop;

  ags_thread_set_flags(thread, AGS_THREAD_TIME_ACCOUNTING);

  /* calculate frequency */
  config = ags_config_get_instance();

  samplerate = ags_soundcard_helper_config_get_samplerate(config);
  buffer_size = ags_soundcard_helper_config_get_buffer_size(config);

  frequency = ((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;
  g_object_set(thread,
	       "frequency", frequency,
	       NULL);

  audio_loop->flags = 0;

  /* tree lock mutex */
  g_rec_mutex_init(&(audio_loop->tree_lock));

  ags_main_loop_set_syncing(AGS_MAIN_LOOP(audio_loop), FALSE);

  ags_main_loop_set_critical_region(AGS_MAIN_LOOP(audio_loop), FALSE);
  g_atomic_int_set(&(audio_loop->critical_region_ref), 0);
  
  /* recall related lists */
  audio_loop->play_channel_ref = 0;
  audio_loop->play_channel = NULL;

  audio_loop->play_audio_ref = 0;
  audio_loop->play_audio = NULL;

  audio_loop->sync_thread = NULL;
}

void
ags_audio_loop_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsAudioLoop *audio_loop;

  GRecMutex *thread_mutex;

  audio_loop = AGS_AUDIO_LOOP(gobject);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(audio_loop);

  switch(prop_id){
  case PROP_PLAY_CHANNEL:
    {
      AgsPlayback *playback;

      playback = (AgsPlayback *) g_value_get_pointer(value);

      g_rec_mutex_lock(thread_mutex);

      if(AGS_IS_PLAYBACK(playback) &&
	 g_list_find(audio_loop->play_channel, playback) == NULL){
	g_object_ref(playback);
	audio_loop->play_channel = g_list_prepend(audio_loop->play_channel,
						  playback);
	audio_loop->play_channel_ref = audio_loop->play_channel_ref + 1;
      }

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_PLAY_AUDIO:
    {
      AgsPlaybackDomain *playback_domain;

      playback_domain = (AgsPlaybackDomain *) g_value_get_pointer(value);

      g_rec_mutex_lock(thread_mutex);

      if(AGS_IS_PLAYBACK_DOMAIN(playback_domain) &&
	 g_list_find(audio_loop->play_audio, playback_domain) == NULL){
	audio_loop->play_audio = g_list_prepend(audio_loop->play_audio,
						playback_domain);
	g_object_ref(playback_domain);
	audio_loop->play_audio_ref = audio_loop->play_audio_ref + 1;
      }

      g_rec_mutex_unlock(thread_mutex);
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

  GRecMutex *thread_mutex;

  audio_loop = AGS_AUDIO_LOOP(gobject);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(audio_loop);

  switch(prop_id){
  case PROP_PLAY_CHANNEL:
    {
      g_rec_mutex_lock(thread_mutex);

      g_value_set_pointer(value, g_list_copy_deep(audio_loop->play_channel,
						  (GCopyFunc) g_object_ref,
						  NULL));

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_PLAY_AUDIO:
    {
      g_rec_mutex_lock(thread_mutex);

      g_value_set_pointer(value, g_list_copy_deep(audio_loop->play_audio,
						  (GCopyFunc) g_object_ref,
						  NULL));

      g_rec_mutex_unlock(thread_mutex);
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
  
  /* unref AgsPlayback lists */  
  g_list_free_full(audio_loop->play_channel,
		   g_object_unref);
  
  g_list_free_full(audio_loop->play_audio,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_audio_loop_parent_class)->finalize(gobject);
}

GRecMutex*
ags_audio_loop_get_tree_lock(AgsMainLoop *main_loop)
{
  GRecMutex *tree_lock;

  /* get tree lock mutex */
  tree_lock = &(AGS_AUDIO_LOOP(main_loop)->tree_lock);
  
  return(tree_lock);
}

void
ags_audio_loop_set_syncing(AgsMainLoop *main_loop, gboolean is_syncing)
{
  AgsAudioLoop *audio_loop;

  audio_loop = AGS_AUDIO_LOOP(main_loop);

  /* set syncing */
  g_atomic_int_set(&(audio_loop->is_syncing), is_syncing);
}

gboolean
ags_audio_loop_is_syncing(AgsMainLoop *main_loop)
{
  AgsAudioLoop *audio_loop;

  gboolean is_syncing;

  audio_loop = AGS_AUDIO_LOOP(main_loop);

  /* is syncing */
  is_syncing = g_atomic_int_get(&(audio_loop->is_syncing));

  return(is_syncing);
}

void
ags_audio_loop_set_critical_region(AgsMainLoop *main_loop, gboolean is_critical_region)
{
  AgsAudioLoop *audio_loop;

  audio_loop = AGS_AUDIO_LOOP(main_loop);

  /* set critical region */
  g_atomic_int_set(&(audio_loop->is_critical_region), is_critical_region);
}

gboolean
ags_audio_loop_is_critical_region(AgsMainLoop *main_loop)
{
  AgsAudioLoop *audio_loop;

  gboolean is_critical_region;

  audio_loop = AGS_AUDIO_LOOP(main_loop);

  /* is critical region */
  is_critical_region = g_atomic_int_get(&(audio_loop->is_critical_region));

  return(is_critical_region);
}

void
ags_audio_loop_inc_queued_critical_region(AgsMainLoop *main_loop)
{
  AgsAudioLoop *audio_loop;

  audio_loop = AGS_AUDIO_LOOP(main_loop);

  /* increment critical region */
  g_atomic_int_inc(&(audio_loop->critical_region_ref));
}

void
ags_audio_loop_dec_queued_critical_region(AgsMainLoop *main_loop)
{
  AgsAudioLoop *audio_loop;

  audio_loop = AGS_AUDIO_LOOP(main_loop);

  /* decrement critical region */
  g_atomic_int_dec_and_test(&(audio_loop->critical_region_ref));
}

guint
ags_audio_loop_test_queued_critical_region(AgsMainLoop *main_loop)
{
  AgsAudioLoop *audio_loop;

  guint critical_region_ref;
  
  audio_loop = AGS_AUDIO_LOOP(main_loop);

  /* set critical region */
  critical_region_ref = g_atomic_int_get(&(audio_loop->is_critical_region));

  return(critical_region_ref);
}

void
ags_audio_loop_change_frequency(AgsMainLoop *main_loop,
				gdouble frequency)
{
  AgsThread *audio_loop;
  AgsThread *thread, *next_thread;

  audio_loop = AGS_THREAD(main_loop);
  
  g_object_set(audio_loop,
	       "frequency", frequency,
	       NULL);

  /* reset soundcard thread */
  thread = ags_thread_find_type(audio_loop, AGS_TYPE_SOUNDCARD_THREAD);

  while(thread != NULL){
    if(AGS_IS_SOUNDCARD_THREAD(thread)){
      g_object_set(thread,
		   "frequency", frequency,
		   NULL);
    }

    /* iterate */
    next_thread = ags_thread_next(thread);

    g_object_unref(thread);

    thread = next_thread;
  }

  /* reset sequencer thread */
  thread = ags_thread_find_type(audio_loop, AGS_TYPE_SEQUENCER_THREAD);

  while(thread != NULL){
    if(AGS_IS_SEQUENCER_THREAD(thread)){
      g_object_set(thread,
		   "frequency", frequency,
		   NULL);
    }
    
    /* iterate */
    next_thread = ags_thread_next(thread);

    g_object_unref(thread);

    thread = next_thread;
  }

  /* reset export thread */
  thread = ags_thread_find_type(audio_loop, AGS_TYPE_EXPORT_THREAD);

  while(thread != NULL){
    if(AGS_IS_EXPORT_THREAD(thread)){
      g_object_set(thread,
		   "frequency", frequency,
		   NULL);
    }
    
    /* iterate */
    next_thread = ags_thread_next(thread);

    g_object_unref(thread);

    thread = next_thread;
  }

  /* reset audio thread */
  thread = ags_thread_find_type(audio_loop, AGS_TYPE_AUDIO_THREAD);

  while(thread != NULL){
    if(AGS_IS_AUDIO_THREAD(thread)){
      AgsThread *child, *next_child;
    
      g_object_set(thread,
		   "frequency", frequency,
		   NULL);

      /* reset channel thread */
      child = ags_thread_find_type(audio_loop, AGS_TYPE_CHANNEL_THREAD);

      while(child != NULL){
	g_object_set(child,
		     "frequency", frequency,
		     NULL);

	/* iterate */
	next_child = ags_thread_next(child);

	g_object_unref(child);

	child = next_child;
      }
    }
    
    /* iterate */
    next_thread = ags_thread_next(thread);

    g_object_unref(thread);

    thread = next_thread;
  }
}

void
ags_audio_loop_start(AgsThread *thread)
{
  AgsAudioLoop *audio_loop;
  
  GRecMutex *thread_mutex;

  audio_loop = AGS_AUDIO_LOOP(thread);

  /* set status synced */
  ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_SYNCED);

  /* call parent */
  AGS_THREAD_CLASS(ags_audio_loop_parent_class)->start(thread);
}

void
ags_audio_loop_run(AgsThread *thread)
{
  AgsAudioLoop *audio_loop;

  GList *start_queue;
  
  guint play_audio_ref, play_channel_ref;
  
  GRecMutex *thread_mutex;

//  g_message("do: audio loop %f", thread->tic_delay);
  
  audio_loop = AGS_AUDIO_LOOP(thread);

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);
  
  /* real-time setup */
#ifdef AGS_WITH_RT
  if(!ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RT_SETUP)){
    AgsPriority *priority;
    
    struct sched_param param;

    gchar *str;

    priority = ags_priority_get_instance();
    
    /* Declare ourself as a real time task */
    param.sched_priority = 45;

    str = ags_priority_get_value(priority,
				 AGS_PRIORITY_RT_THREAD,
				 AGS_PRIORITY_KEY_AUDIO);

    if(str != NULL){
      param.sched_priority = (int) g_ascii_strtoull(str,
						    NULL,
						    10);
    }
    
    if(str == NULL ||
       ((!g_ascii_strncasecmp(str,
			      "0",
			      2)) != TRUE)){
      if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
	perror("sched_setscheduler failed");
      }
    }

    g_free(str);
    
    ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_RT_SETUP);
  }
#endif

  /* get some fields */
  g_rec_mutex_lock(thread_mutex);

  start_queue = thread->start_queue;

  play_audio_ref = audio_loop->play_audio_ref;
  play_channel_ref = audio_loop->play_channel_ref;
  
  g_rec_mutex_unlock(thread_mutex);
  
  /* play channel */
  if(ags_audio_loop_test_flags(audio_loop, AGS_AUDIO_LOOP_PLAY_CHANNEL)){
    ags_audio_loop_play_channel(audio_loop);

    if(play_channel_ref == 0 &&
       start_queue == NULL){
      ags_audio_loop_unset_flags(audio_loop, AGS_AUDIO_LOOP_PLAY_CHANNEL);
    }
  }

  /* play audio */
  if(ags_audio_loop_test_flags(audio_loop, AGS_AUDIO_LOOP_PLAY_AUDIO)){
    ags_audio_loop_play_audio(audio_loop);

    if(play_audio_ref == 0 &&
       start_queue == NULL){
      ags_audio_loop_unset_flags(audio_loop, AGS_AUDIO_LOOP_PLAY_AUDIO);
    }
  }

  /* decide if we stop */
  if(play_channel_ref == 0 &&
     play_audio_ref == 0){
    AgsThread *soundcard_thread, *next_soundcard_thread;
    AgsThread *sequencer_thread, *next_sequencer_thread;
    AgsThread *export_thread, *next_export_thread;

    gdouble frequency;

    /* soundcard thread */
    soundcard_thread = ags_thread_find_type(thread,
					    AGS_TYPE_SOUNDCARD_THREAD);

    while(soundcard_thread != NULL){
      if(AGS_IS_SOUNDCARD_THREAD(soundcard_thread)){
	if(ags_thread_test_status_flags(soundcard_thread, AGS_THREAD_STATUS_RUNNING)){
	  ags_thread_stop(soundcard_thread);
	}
      }

      /* iterate */
      next_soundcard_thread = ags_thread_next(soundcard_thread);

      g_object_unref(soundcard_thread);

      soundcard_thread = next_soundcard_thread;
    }

    /* sequencer thread */
    sequencer_thread = ags_thread_find_type(thread,
					    AGS_TYPE_SEQUENCER_THREAD);

    while(sequencer_thread != NULL){
      if(AGS_IS_SEQUENCER_THREAD(sequencer_thread)){
	if(ags_thread_test_status_flags(sequencer_thread, AGS_THREAD_STATUS_RUNNING)){
	  ags_thread_stop(sequencer_thread);
	}
      }

      /* iterate */
      next_sequencer_thread = ags_thread_next(sequencer_thread);

      g_object_unref(sequencer_thread);

      sequencer_thread = next_sequencer_thread;
    }

    /* export thread */
    export_thread = ags_thread_find_type(thread,
					 AGS_TYPE_EXPORT_THREAD);

    while(export_thread != NULL){
      if(AGS_IS_EXPORT_THREAD(export_thread)){
	if(ags_thread_test_status_flags(export_thread, AGS_THREAD_STATUS_RUNNING)){
	  ags_thread_stop(export_thread);
	}
      }

      /* iterate */
      next_export_thread = ags_thread_next(export_thread);

      g_object_unref(export_thread);

      export_thread = next_export_thread;
    }

    g_object_get(audio_loop,
		 "frequency", &frequency,
		 NULL);

    if(!ags_thread_test_flags(audio_loop, AGS_THREAD_TIME_ACCOUNTING)){
      g_usleep((guint) (G_USEC_PER_SEC / frequency) - 4);
    }
  }
}

/**
 * ags_audio_loop_play_channel:
 * @audio_loop: an #AgsAudioLoop
 *
 * Invokes ags_channel_recursive_run_stage() for all scopes containing #AgsRecallID.
 *
 * Since: 3.0.0
 */
void
ags_audio_loop_play_channel(AgsAudioLoop *audio_loop)
{
  AgsPlayback *playback;
  AgsChannel *channel;

  GList *start_play_channel, *play_channel;
  GList *recall_id;

  gint sound_scope;

  GRecMutex *thread_mutex;

#if 1  
  static const guint playback_staging_flags = (AGS_SOUND_STAGING_RESET |
					       AGS_SOUND_STAGING_FEED_INPUT_QUEUE |
					       AGS_SOUND_STAGING_AUTOMATE |
					       AGS_SOUND_STAGING_RUN_PRE |
					       AGS_SOUND_STAGING_RUN_INTER |
					       AGS_SOUND_STAGING_RUN_POST |
					       AGS_SOUND_STAGING_DO_FEEDBACK |
					       AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE);
#else
  static const guint playback_staging_flags = (AGS_SOUND_STAGING_AUTOMATE |
					       AGS_SOUND_STAGING_RUN_PRE |
					       AGS_SOUND_STAGING_RUN_INTER |
					       AGS_SOUND_STAGING_RUN_POST);
#endif
  
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(audio_loop);

  /* get play channel */
  g_rec_mutex_lock(thread_mutex);

  start_play_channel = g_list_copy_deep(audio_loop->play_channel,
					(GCopyFunc) g_object_ref,
					NULL);
  
  g_rec_mutex_unlock(thread_mutex);

  if(start_play_channel == NULL){
    if(ags_audio_loop_test_flags(audio_loop, AGS_AUDIO_LOOP_PLAY_CHANNEL_TERMINATING)){
      ags_audio_loop_unset_flags(audio_loop, (AGS_AUDIO_LOOP_PLAY_CHANNEL |
					      AGS_AUDIO_LOOP_PLAY_CHANNEL_TERMINATING));
    }else{
      ags_audio_loop_set_flags(audio_loop, AGS_AUDIO_LOOP_PLAY_CHANNEL_TERMINATING);
    }
  }

  //FIXME:JK: missing else
  ags_audio_loop_unset_flags(audio_loop, AGS_AUDIO_LOOP_PLAY_CHANNEL_TERMINATING);
  ags_audio_loop_set_flags(audio_loop, AGS_AUDIO_LOOP_PLAYING_CHANNEL);
  
  /* run the 3 stages */
  play_channel = start_play_channel;

  while(play_channel != NULL){
    playback = (AgsPlayback *) play_channel->data;

    channel = NULL;
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
      
      if(ags_playback_get_recall_id(playback, sound_scope) == NULL){
	g_object_unref(channel);
	
	play_channel = play_channel->next;
	
	continue;
      }
    
      if((recall_id = ags_channel_check_scope(channel, sound_scope)) != NULL){
	ags_channel_recursive_run_stage(channel,
					sound_scope, playback_staging_flags);

	g_list_free_full(recall_id,
			 g_object_unref);
      }
    }

    g_object_unref(channel);

    /* iterate */
    play_channel = play_channel->next;
  }

  /* sync channel */
  play_channel = start_play_channel;
  
  while(play_channel != NULL){    
    playback = (AgsPlayback *) play_channel->data;

    /* sync */
    if(ags_playback_test_flags(playback, AGS_PLAYBACK_SUPER_THREADED_CHANNEL)){
      /* super threaded */
      ags_audio_loop_sync_channel_super_threaded(audio_loop,
						 playback);
    }

    /* iterate */
    play_channel = play_channel->next;
  }

  g_list_free_full(start_play_channel,
		   g_object_unref);
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

  channel = NULL;
  g_object_get(playback,
	       "channel", &channel,
	       NULL);

  g_object_unref(channel);
    
  sound_scope = AGS_SOUND_SCOPE_PLAYBACK;
  
  if((recall_id = ags_channel_check_scope(channel, sound_scope)) != NULL){
    thread = ags_playback_get_channel_thread(playback,
					     sound_scope);

    if(thread != NULL){
      channel_thread = (AgsChannelThread *) thread;
      
      if(ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING) &&
	 ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_SYNCED)){
	/* wakeup wait */
	g_mutex_lock(&(channel_thread->wakeup_mutex));

	ags_channel_thread_unset_status_flags(channel_thread,
					      AGS_CHANNEL_THREAD_STATUS_WAIT);
	    
	if(!ags_channel_thread_test_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_DONE)){
	  g_cond_signal(&(channel_thread->wakeup_cond));
	}
	    
	g_mutex_unlock(&(channel_thread->wakeup_mutex));

	audio_loop->sync_thread = g_list_prepend(audio_loop->sync_thread,
						 channel_thread);
      }
    }
      
    g_list_free_full(recall_id,
		     g_object_unref);
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

  channel = NULL;
  g_object_get(playback,
	       "channel", &channel,
	       NULL);

  g_object_unref(channel);
    
  sound_scope = AGS_SOUND_SCOPE_PLAYBACK;

  if((recall_id = ags_channel_check_scope(channel, sound_scope)) != NULL){
    thread = ags_playback_get_channel_thread(playback,
					     sound_scope);

    if(thread != NULL){
      channel_thread = (AgsChannelThread *) thread;
	
      g_mutex_lock(&(channel_thread->done_mutex));
  
      if(g_list_find(audio_loop->sync_thread, channel_thread) != NULL &&
	 ags_thread_test_status_flags(channel_thread, AGS_THREAD_STATUS_RUNNING) &&
	 ags_thread_test_status_flags(channel_thread, AGS_THREAD_STATUS_SYNCED)){
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

/**
 * ags_audio_loop_play_audio:
 * @audio_loop: an #AgsAudioLoop
 *
 * Invokes ags_audio_recursive_run_stage() for all scopes containing #AgsRecallID.
 *
 * Since: 3.0.0
 */
void
ags_audio_loop_play_audio(AgsAudioLoop *audio_loop)
{
  AgsPlaybackDomain *playback_domain;
  AgsAudio *audio;

  GList *start_play_audio, *play_audio;
  GList *recall_id;
  
  gint sound_scope;

  GRecMutex *thread_mutex;

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(audio_loop);

  /* get play audio */
  g_rec_mutex_lock(thread_mutex);

  start_play_audio = g_list_copy_deep(audio_loop->play_audio,
				      (GCopyFunc) g_object_ref,
				      NULL);
  
  g_rec_mutex_unlock(thread_mutex);
  
  if(start_play_audio == NULL){
    if(ags_audio_loop_test_flags(audio_loop, AGS_AUDIO_LOOP_PLAY_AUDIO_TERMINATING)){
      ags_audio_loop_unset_flags(audio_loop, (AGS_AUDIO_LOOP_PLAY_AUDIO |
					      AGS_AUDIO_LOOP_PLAY_AUDIO_TERMINATING));
    }else{
      ags_audio_loop_set_flags(audio_loop, AGS_AUDIO_LOOP_PLAY_AUDIO_TERMINATING);
    }
  }

  //FIXME:JK: missing else
  ags_audio_loop_unset_flags(audio_loop, AGS_AUDIO_LOOP_PLAY_AUDIO_TERMINATING);
  ags_audio_loop_set_flags(audio_loop, AGS_AUDIO_LOOP_PLAYING_AUDIO);

  /* playing */
  play_audio = start_play_audio;

  while(play_audio != NULL){
    playback_domain = (AgsPlaybackDomain *) play_audio->data;

    audio = NULL;
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
#if 1
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
#else
	  ags_audio_recursive_run_stage(audio,
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

    g_object_unref(audio);
    
    /* iterate */
    play_audio = play_audio->next;
  }
  
  /* sync audio */
  play_audio = start_play_audio;

  while(play_audio != NULL){
    playback_domain = (AgsPlaybackDomain *) play_audio->data;

    /* sync */
    if(ags_playback_domain_test_flags(playback_domain, AGS_PLAYBACK_DOMAIN_SUPER_THREADED_AUDIO)){
      ags_audio_loop_sync_audio_super_threaded(audio_loop,
					       playback_domain);
    }
    
    /* iterate */
    play_audio = play_audio->next;
  }

  g_list_free_full(start_play_audio,
		   g_object_unref);

  g_list_free(audio_loop->sync_thread);
  audio_loop->sync_thread = NULL;
}
 
/**
 * ags_audio_loop_play_audio_super_threaded:
 * @audio_loop: the #AgsAudioLoop
 * @playback_domain: an #AgsPlaybackDomain
 *
 * Play audio super-threaded.
 *
 * Since: 3.0.0
 */
void
ags_audio_loop_play_audio_super_threaded(AgsAudioLoop *audio_loop, AgsPlaybackDomain *playback_domain)
{
  AgsAudio *audio;
  AgsAudioThread *audio_thread;
  
  AgsThread *thread;
  
  GList *recall_id;
  
  gint sound_scope;

  audio = NULL;
  g_object_get(playback_domain,
	       "audio", &audio,
	       NULL);
  
  for(sound_scope = 0; sound_scope < AGS_SOUND_SCOPE_LAST; sound_scope++){    
    if(sound_scope == AGS_SOUND_SCOPE_PLAYBACK){
      continue;
    }

    if((recall_id = ags_audio_check_scope(audio, sound_scope)) != NULL){
      thread = ags_playback_domain_get_audio_thread(playback_domain,
						    sound_scope);

      if(thread != NULL){
	audio_thread = (AgsAudioThread *) thread;
	
	if(ags_thread_test_status_flags(audio_thread, AGS_THREAD_STATUS_RUNNING) &&
	   ags_thread_test_status_flags(audio_thread, AGS_THREAD_STATUS_SYNCED)){
	  /* wakeup wait */
	  g_mutex_lock(&(audio_thread->wakeup_mutex));

	  ags_audio_thread_unset_status_flags(audio_thread, AGS_AUDIO_THREAD_STATUS_WAIT);
      
	  if(!ags_audio_thread_test_status_flags(audio_thread, AGS_AUDIO_THREAD_STATUS_DONE)){
	    g_cond_signal(&(audio_thread->wakeup_cond));
	  }
      
	  g_mutex_unlock(&(audio_thread->wakeup_mutex));

	  audio_loop->sync_thread = g_list_prepend(audio_loop->sync_thread,
						   audio_thread);
	}
      }
      
      g_list_free_full(recall_id,
		       g_object_unref);
    }
  }

  g_object_unref(audio);
}

/**
 * ags_audio_loop_sync_audio_super_threaded:
 * @audio_loop: the #AgsAudioLoop
 * @playback_domain: an #AgsPlaybackDomain
 *
 * Sync audio super-threaded.
 *
 * Since: 3.0.0
 */
void
ags_audio_loop_sync_audio_super_threaded(AgsAudioLoop *audio_loop, AgsPlaybackDomain *playback_domain)
{
  AgsAudio *audio;
  AgsAudioThread *audio_thread;

  AgsThread *thread;

  GList *recall_id;
  
  gint sound_scope;

  audio = NULL;
  g_object_get(playback_domain,
	       "audio", &audio,
	       NULL);
    
  for(sound_scope = 0; sound_scope < AGS_SOUND_SCOPE_LAST; sound_scope++){    
    if(sound_scope == AGS_SOUND_SCOPE_PLAYBACK){
      continue;
    }

    if((recall_id = ags_audio_check_scope(audio, sound_scope)) != NULL){
      thread = ags_playback_domain_get_audio_thread(playback_domain,
						    sound_scope);

      if(thread != NULL){
	audio_thread = (AgsAudioThread *) thread;

	if(g_list_find(audio_loop->sync_thread, audio_thread) != NULL &&
	   ags_thread_test_status_flags(audio_thread, AGS_THREAD_STATUS_RUNNING) &&
	   ags_thread_test_status_flags(audio_thread, AGS_THREAD_STATUS_SYNCED)){
	  g_mutex_lock(&(audio_thread->done_mutex));
	  
	  if(ags_audio_thread_test_status_flags(audio_thread, AGS_AUDIO_THREAD_STATUS_WAIT_SYNC)){
	    ags_audio_thread_unset_status_flags(audio_thread, AGS_AUDIO_THREAD_STATUS_DONE_SYNC);
	    
	    while(!ags_audio_thread_test_status_flags(audio_thread, AGS_AUDIO_THREAD_STATUS_DONE_SYNC) &&
		  ags_audio_thread_test_status_flags(audio_thread, AGS_AUDIO_THREAD_STATUS_WAIT_SYNC)){
	      g_cond_wait(&(audio_thread->done_cond),
			  &(audio_thread->done_mutex));
	    }
	  }

	  ags_audio_thread_set_status_flags(audio_thread, (AGS_AUDIO_THREAD_STATUS_WAIT_SYNC |
							   AGS_AUDIO_THREAD_STATUS_DONE_SYNC));
      	  
	  g_mutex_unlock(&(audio_thread->done_mutex));
	}
      }
      
      g_list_free_full(recall_id,
		       g_object_unref);
    }
  }

  g_object_unref(audio);
}

/**
 * ags_audio_loop_test_flags:
 * @audio_loop: the #AgsAudioLoop
 * @flags: the flags
 * 
 * Test @flags to be set on @audio_loop.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_audio_loop_test_flags(AgsAudioLoop *audio_loop, guint flags)
{
  gboolean retval;
  
  GRecMutex *audio_loop_mutex;

  if(!AGS_IS_AUDIO_LOOP(audio_loop)){
    return(FALSE);
  }
  
  /* get audio loop mutex */
  audio_loop_mutex = AGS_THREAD_GET_OBJ_MUTEX(audio_loop);

  /* test flags */
  g_rec_mutex_lock(audio_loop_mutex);

  retval = ((flags & (audio_loop->flags)) != 0) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(audio_loop_mutex);

  return(retval);
}

/**
 * ags_audio_loop_set_flags:
 * @audio_loop: the #AgsAudioLoop
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 3.0.0
 */
void
ags_audio_loop_set_flags(AgsAudioLoop *audio_loop, guint flags)
{
  GRecMutex *audio_loop_mutex;

  if(!AGS_IS_AUDIO_LOOP(audio_loop)){
    return;
  }
  
  /* get audio loop mutex */
  audio_loop_mutex = AGS_THREAD_GET_OBJ_MUTEX(audio_loop);

  /* set flags */
  g_rec_mutex_lock(audio_loop_mutex);

  audio_loop->flags |= flags;
  
  g_rec_mutex_unlock(audio_loop_mutex);
}

/**
 * ags_audio_loop_unset_flags:
 * @audio_loop: the #AgsAudioLoop
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 3.0.0
 */
void
ags_audio_loop_unset_flags(AgsAudioLoop *audio_loop, guint flags)
{
  GRecMutex *audio_loop_mutex;

  if(!AGS_IS_AUDIO_LOOP(audio_loop)){
    return;
  }
  
  /* get audio loop mutex */
  audio_loop_mutex = AGS_THREAD_GET_OBJ_MUTEX(audio_loop);

  /* unset flags */
  g_rec_mutex_lock(audio_loop_mutex);

  audio_loop->flags &= (~flags);
  
  g_rec_mutex_unlock(audio_loop_mutex);
}

/**
 * ags_audio_loop_add_audio:
 * @audio_loop: the #AgsAudioLoop
 * @audio: an #AgsAudio
 *
 * Add audio for playback.
 *
 * Since: 3.0.0
 */
void
ags_audio_loop_add_audio(AgsAudioLoop *audio_loop, GObject *audio)
{
  AgsPlaybackDomain *playback_domain;

  GRecMutex *thread_mutex;
  
  if(!AGS_IS_AUDIO_LOOP(audio_loop) ||
     !AGS_IS_AUDIO(audio)){
    return;
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(audio_loop);
  
  g_object_get(audio,
	       "playback-domain", &playback_domain,
	       NULL);

  g_rec_mutex_lock(thread_mutex);
  
  if(g_list_find(audio_loop->play_audio,
		 playback_domain) == NULL){
    audio_loop->play_audio = g_list_prepend(audio_loop->play_audio,
					    playback_domain);

    audio_loop->play_audio_ref = audio_loop->play_audio_ref + 1;
  }else{
    if(playback_domain != NULL){
      g_object_unref(playback_domain);
    }
  }
  
  g_rec_mutex_unlock(thread_mutex);  
}

/**
 * ags_audio_loop_remove_audio:
 * @audio_loop: the #AgsAudioLoop
 * @audio: an #AgsAudio
 *
 * Remove audio of playback.
 *
 * Since: 3.0.0
 */
void
ags_audio_loop_remove_audio(AgsAudioLoop *audio_loop, GObject *audio)
{  
  AgsPlaybackDomain *playback_domain;

  GRecMutex *thread_mutex;

  if(!AGS_IS_AUDIO_LOOP(audio_loop) ||
     !AGS_IS_AUDIO(audio)){
    return;
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(audio_loop);
  
  g_object_get(audio,
	       "playback-domain", &playback_domain,
	       NULL);

  g_rec_mutex_lock(thread_mutex);

  if(g_list_find(audio_loop->play_audio,
		 playback_domain) != NULL){
    audio_loop->play_audio = g_list_remove(audio_loop->play_audio,
					   playback_domain);
    audio_loop->play_audio_ref = audio_loop->play_audio_ref - 1;
    
    g_object_unref(playback_domain);
  }

  g_rec_mutex_unlock(thread_mutex);  

  if(playback_domain != NULL){
    g_object_unref(playback_domain);
  }
}

/**
 * ags_audio_loop_add_channel:
 * @audio_loop: the #AgsAudioLoop
 * @channel: an #AgsChannel
 *
 * Add channel for playback.
 *
 * Since: 3.0.0
 */
void
ags_audio_loop_add_channel(AgsAudioLoop *audio_loop, GObject *channel)
{
  AgsPlayback *playback;
  
  GRecMutex *thread_mutex;

  if(!AGS_IS_AUDIO_LOOP(audio_loop) ||
     !AGS_IS_CHANNEL(channel)){
    return;
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(audio_loop);  

  g_object_get(channel,
	       "playback", &playback,
	       NULL);

  g_rec_mutex_lock(thread_mutex);

  if(g_list_find(audio_loop->play_channel,
		 playback) == NULL){    
    audio_loop->play_channel = g_list_prepend(audio_loop->play_channel,
					      playback);

    audio_loop->play_channel_ref = audio_loop->play_channel_ref + 1;
  }else{
    if(playback != NULL){
      g_object_unref(playback);
    }
  }

  g_rec_mutex_unlock(thread_mutex);  
}

/**
 * ags_audio_loop_remove_channel:
 * @audio_loop: the #AgsAudioLoop
 * @channel: an #AgsChannel
 *
 * Remove channel of playback.
 *
 * Since: 3.0.0
 */
void
ags_audio_loop_remove_channel(AgsAudioLoop *audio_loop, GObject *channel)
{
  AgsPlayback *playback;

  GRecMutex *thread_mutex;

  if(!AGS_IS_AUDIO_LOOP(audio_loop) ||
     !AGS_IS_CHANNEL(channel)){
    return;
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(audio_loop);
  
  g_object_get(channel,
	       "playback", &playback,
	       NULL);

  g_rec_mutex_lock(thread_mutex);

  if(g_list_find(audio_loop->play_channel,
		 playback) != NULL){
    audio_loop->play_channel = g_list_remove(audio_loop->play_channel,
					     playback);
    audio_loop->play_channel_ref = audio_loop->play_channel_ref - 1;

    g_object_unref(playback);
  }

  g_rec_mutex_unlock(thread_mutex);  

  if(playback != NULL){
    g_object_unref(playback);
  }
}

/**
 * ags_audio_loop_new:
 *
 * Create a new #AgsAudioLoop.
 *
 * Returns: the new #AgsAudioLoop
 *
 * Since: 3.0.0
 */
AgsAudioLoop*
ags_audio_loop_new()
{
  AgsAudioLoop *audio_loop;

  audio_loop = (AgsAudioLoop *) g_object_new(AGS_TYPE_AUDIO_LOOP,
					     NULL);

  return(audio_loop);
}
