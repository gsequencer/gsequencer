/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_playback.h>

#include <ags/audio/thread/ags_audio_loop.h>

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
  PROP_PROCESSING,
  PROP_TASK_LAUNCHER,
};

static gpointer ags_channel_thread_parent_class = NULL;
static AgsConnectableInterface *ags_channel_thread_parent_connectable_interface;

GType
ags_channel_thread_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_channel_thread = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_channel_thread);
  }

  return g_define_type_id__volatile;
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
   * AgsChannelThread:channel:
   *
   * The assigned #AgsChannel.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("channel assigned to"),
				   i18n_pspec("The AgsChannel it is assigned to."),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /**
   * AgsChannelThread:processing:
   *
   * The processing state.
   * 
   * Since: 3.11.0
   */
  param_spec = g_param_spec_boolean("processing",
				    i18n_pspec("do processing"),
				    i18n_pspec("Enable or disable processing"),
				    FALSE,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PROCESSING,
				  param_spec);

  /**
   * AgsChannelThread:task-launcher:
   *
   * The assigned #AgsTaskLauncher.
   * 
   * Since: 3.11.0
   */
  param_spec = g_param_spec_object("task-launcher",
				   i18n_pspec("task launcher using"),
				   i18n_pspec("The task launcher this thread uses"),
				   AGS_TYPE_TASK_LAUNCHER,
				   G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_TASK_LAUNCHER,
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

  gdouble frequency;
  guint samplerate;
  guint buffer_size;
  
  static const guint staging_program[] = {
    (AGS_SOUND_STAGING_AUTOMATE | AGS_SOUND_STAGING_RUN_INTER | AGS_SOUND_STAGING_FX),
  };

  thread = (AgsThread *) channel_thread;

  ags_thread_set_flags(thread, (AGS_THREAD_START_SYNCED_FREQ |
				AGS_THREAD_IMMEDIATE_SYNC));
  
  config = ags_config_get_instance();
  
  samplerate = ags_soundcard_helper_config_get_samplerate(config);
  buffer_size = ags_soundcard_helper_config_get_buffer_size(config);

  frequency = ((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

  g_object_set(thread,
	       "frequency", frequency,
	       NULL);

  g_atomic_int_set(&(channel_thread->status_flags),
		   0);

  channel_thread->default_output_soundcard = NULL;
  
  /* start */
  g_mutex_init(&(channel_thread->wakeup_mutex));
  
  g_cond_init(&(channel_thread->wakeup_cond));

  /* sync */
  g_mutex_init(&(channel_thread->done_mutex));
  
  g_cond_init(&(channel_thread->done_cond));

  /* channel and sound scope */
  channel_thread->channel = NULL;
  channel_thread->sound_scope = -1;

  /* staging program */
  channel_thread->do_fx_staging = TRUE;

  channel_thread->staging_program = (guint *) g_malloc(sizeof(guint));

  channel_thread->staging_program[0] = staging_program[0];
  
  channel_thread->staging_program_count = 1;

  channel_thread->processing = FALSE;

  channel_thread->task_launcher = ags_task_launcher_new();  
}

void
ags_channel_thread_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsChannelThread *channel_thread;

  GRecMutex *thread_mutex;

  channel_thread = AGS_CHANNEL_THREAD(gobject);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(channel_thread);

  switch(prop_id){
  case PROP_DEFAULT_OUTPUT_SOUNDCARD:
    {
      GObject *default_output_soundcard;

      default_output_soundcard = g_value_get_object(value);

      g_rec_mutex_lock(thread_mutex);

      if(channel_thread->default_output_soundcard == default_output_soundcard){
	g_rec_mutex_unlock(thread_mutex);

	return;
      }

      if(channel_thread->default_output_soundcard != NULL){
	g_object_unref(channel_thread->default_output_soundcard);
      }

      if(default_output_soundcard != NULL){
	g_object_ref(default_output_soundcard);
      }

      channel_thread->default_output_soundcard = default_output_soundcard;

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      g_rec_mutex_lock(thread_mutex);

      if(channel_thread->channel == (GObject *) channel){
	g_rec_mutex_unlock(thread_mutex);
	
	return;
      }
      
      if(channel_thread->channel != NULL){
	g_object_unref(G_OBJECT(channel_thread->channel));
      }

      if(channel != NULL){
	g_object_ref(G_OBJECT(channel));
      }

      channel_thread->channel = (GObject *) channel;

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_PROCESSING:
    {
      gboolean processing;

      processing = g_value_get_boolean(value);

      g_rec_mutex_lock(thread_mutex);

      channel_thread->processing = processing;

      g_rec_mutex_unlock(thread_mutex);
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

  GRecMutex *thread_mutex;

  channel_thread = AGS_CHANNEL_THREAD(gobject);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(channel_thread);

  switch(prop_id){
  case PROP_DEFAULT_OUTPUT_SOUNDCARD:
    {
      g_rec_mutex_lock(thread_mutex);

      g_value_set_object(value, channel_thread->default_output_soundcard);

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_CHANNEL:
    {
      g_rec_mutex_lock(thread_mutex);

      g_value_set_object(value, G_OBJECT(channel_thread->channel));

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_PROCESSING:
    {
      g_rec_mutex_lock(thread_mutex);

      g_value_set_boolean(value, channel_thread->processing);

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_TASK_LAUNCHER:
    {
      g_rec_mutex_lock(thread_mutex);

      g_value_set_object(value, G_OBJECT(channel_thread->task_launcher));

      g_rec_mutex_unlock(thread_mutex);
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
    gpointer tmp;

    tmp = channel_thread->default_output_soundcard;

    channel_thread->default_output_soundcard = NULL;

    g_object_unref(tmp);
  }

  /* channel */
  if(channel_thread->channel != NULL){
    gpointer tmp;

    tmp = channel_thread->channel;

    channel_thread->channel = NULL;

    g_object_unref(tmp);
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
    gpointer tmp;

    tmp = channel_thread->default_output_soundcard;

    channel_thread->default_output_soundcard = NULL;

    g_object_unref(tmp);
  }

  /* channel */
  if(channel_thread->channel != NULL){
    gpointer tmp;

    tmp = channel_thread->channel;

    channel_thread->channel = NULL;

    g_object_unref(tmp);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_channel_thread_parent_class)->finalize(gobject);
}

void
ags_channel_thread_start(AgsThread *thread)
{
#ifdef AGS_DEBUG
  g_message("channel thread start");
#endif
  
  /* reset status */
  ags_channel_thread_set_status_flags(thread, (AGS_CHANNEL_THREAD_STATUS_WAIT |
					       AGS_CHANNEL_THREAD_STATUS_DONE |
					       AGS_CHANNEL_THREAD_STATUS_WAIT_SYNC |
					       AGS_CHANNEL_THREAD_STATUS_DONE_SYNC));

  AGS_THREAD_CLASS(ags_channel_thread_parent_class)->start(thread);
}

void
ags_channel_thread_run(AgsThread *thread)
{
  AgsChannel *channel;
  AgsPlayback *playback;
  
  AgsAudioLoop *audio_loop;
  AgsChannelThread *channel_thread;

  AgsTaskLauncher *task_launcher;

  GList *recall_id;
  
  gint sound_scope;
  gboolean processing;

  GRecMutex *thread_mutex;

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

  audio_loop = thread->parent;

  if(!ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_SYNCED)){
    return;
  }

  channel_thread = AGS_CHANNEL_THREAD(thread);

  processing = FALSE;
  
  task_launcher = NULL;
  
  g_object_get(channel_thread,
	       "processing", &processing,
	       "task-launcher", &task_launcher,
	       NULL);
  
  ags_task_launcher_run(task_launcher);

  if(processing){
    channel = NULL;
  
    g_object_get(channel_thread,
		 "channel", &channel,
		 NULL);

    playback = NULL;
  
    g_object_get(channel,
		 "playback", &playback,
		 NULL);
  
    /* start - wait until signaled */
    g_mutex_lock(&(channel_thread->wakeup_mutex));

    if(!ags_channel_thread_test_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_DONE) &&
       ags_channel_thread_test_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_WAIT)){
      ags_channel_thread_unset_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_DONE);
    
      while(!ags_channel_thread_test_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_DONE) &&
	    ags_channel_thread_test_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_WAIT)){
	g_cond_wait(&(channel_thread->wakeup_cond),
		    &(channel_thread->wakeup_mutex));
      }
    }
  
    ags_channel_thread_set_status_flags(channel_thread, (AGS_CHANNEL_THREAD_STATUS_WAIT |
							 AGS_CHANNEL_THREAD_STATUS_DONE));
  
    g_mutex_unlock(&(channel_thread->wakeup_mutex));
  
    /* 
     * do audio processing
     */  
    if(channel_thread->sound_scope >= 0){
      sound_scope = channel_thread->sound_scope;

      if(sound_scope != AGS_SOUND_SCOPE_PLAYBACK ||
	 ags_playback_get_recall_id(playback, sound_scope) != NULL){    
	if((recall_id = ags_channel_check_scope(channel, sound_scope)) != NULL){
	  guint *staging_program;
	
	  guint staging_program_count;
	  guint nth;

	  staging_program = ags_channel_thread_get_staging_program(channel_thread,
								   &staging_program_count);
	
	  for(nth = 0; nth < staging_program_count; nth++){
	    ags_channel_recursive_run_stage(channel,
					    sound_scope, staging_program[nth]);
	  }

	  g_free(staging_program);
	  
	  g_list_free_full(recall_id,
			   g_object_unref);
	}
      }
    }else{
      gint nth_sound_scope;
    
      for(nth_sound_scope = 0; nth_sound_scope < AGS_SOUND_SCOPE_LAST; nth_sound_scope++){
	if(nth_sound_scope == AGS_SOUND_SCOPE_PLAYBACK ||
	   ags_playback_get_recall_id(playback, nth_sound_scope) == NULL){
	  continue;
	}

	if((recall_id = ags_channel_check_scope(channel, nth_sound_scope)) != NULL){
	  guint *staging_program;
	
	  guint staging_program_count;
	  guint nth;

	  staging_program = ags_channel_thread_get_staging_program(channel_thread,
								   &staging_program_count);
	
	  for(nth = 0; nth < staging_program_count; nth++){
	    ags_channel_recursive_run_stage(channel,
					    nth_sound_scope, staging_program[nth]);
	  }

	  g_free(staging_program);
	  
	  g_list_free_full(recall_id,
			   g_object_unref);
	}
      }
    }

    /* sync */
    g_mutex_lock(&(channel_thread->done_mutex));
  
    ags_channel_thread_unset_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_WAIT_SYNC);
  
    if(!ags_channel_thread_test_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_DONE_SYNC)){
      g_cond_signal(&(channel_thread->done_cond));
    }
  
    g_mutex_unlock(&(channel_thread->done_mutex));

    /* unref */
    if(channel != NULL){
      g_object_unref(channel);
    }

    if(playback != NULL){
      g_object_unref(playback);
    }
  }
  
  if(task_launcher != NULL){
    g_object_unref(task_launcher);
  }
}

void
ags_channel_thread_stop(AgsThread *thread)
{
  AgsChannelThread *channel_thread;

  if(!ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING)){
    return;
  }

  /*  */
  channel_thread = AGS_CHANNEL_THREAD(thread);

#ifdef AGS_DEBUG
  g_message("channel thread stop");
#endif
  
  /* call parent */
  AGS_THREAD_CLASS(ags_channel_thread_parent_class)->stop(thread);

  /* ensure synced */
  g_mutex_lock(&(channel_thread->done_mutex));

  ags_channel_thread_unset_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_WAIT_SYNC);

  if(!ags_channel_thread_test_status_flags(channel_thread, AGS_CHANNEL_THREAD_STATUS_DONE_SYNC)){
    g_cond_signal(&(channel_thread->done_cond));
  }

  g_mutex_unlock(&(channel_thread->done_mutex));
}

/**
 * ags_channel_thread_test_status_flags:
 * @channel_thread: the #AgsChannelThread
 * @status_flags: status flags
 * 
 * Test @status_flags of @channel_thread.
 * 
 * Returns: %TRUE if status flags set, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_channel_thread_test_status_flags(AgsChannelThread *channel_thread, guint status_flags)
{
  gboolean retval;
  
  if(!AGS_IS_CHANNEL_THREAD(channel_thread)){
    return(FALSE);
  }

  retval = ((status_flags & (g_atomic_int_get(&(channel_thread->status_flags)))) != 0) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_channel_thread_set_status_flags:
 * @channel_thread: the #AgsChannelThread
 * @status_flags: status flags
 * 
 * Set status flags.
 * 
 * Since: 3.0.0
 */
void
ags_channel_thread_set_status_flags(AgsChannelThread *channel_thread, guint status_flags)
{
  if(!AGS_IS_CHANNEL_THREAD(channel_thread)){
    return;
  }

  g_atomic_int_or(&(channel_thread->status_flags),
		  status_flags);
}

/**
 * ags_channel_thread_unset_status_flags:
 * @channel_thread: the #AgsChannelThread
 * @status_flags: status flags
 * 
 * Unset status flags.
 * 
 * Since: 3.0.0
 */
void
ags_channel_thread_unset_status_flags(AgsChannelThread *channel_thread, guint status_flags)
{
  if(!AGS_IS_CHANNEL_THREAD(channel_thread)){
    return;
  }

  g_atomic_int_and(&(channel_thread->status_flags),
		   (~status_flags));
}

/**
 * ags_channel_thread_get_processing:
 * @channel_thread: the #AgsChannelThread
 * 
 * Get processing.
 * 
 * Returns: %TRUE if do processing, otherwise %FALSE
 * 
 * Since: 3.11.0
 */
gboolean
ags_channel_thread_get_processing(AgsChannelThread *channel_thread)
{
  gboolean processing;
  
  if(!AGS_IS_CHANNEL_THREAD(channel_thread)){
    return(FALSE);
  }

  g_object_get(channel_thread,
	       "processing", &processing,
	       NULL);

  return(processing);
}

/**
 * ags_channel_thread_set_processing:
 * @channel_thread: the #AgsChannelThread
 * @processing: %TRUE enables and %FALSE disables processing
 * 
 * Set processing.
 * 
 * Since: 3.11.0
 */
void
ags_channel_thread_set_processing(AgsChannelThread *channel_thread,
				gboolean processing)
{
  if(!AGS_IS_CHANNEL_THREAD(channel_thread)){
    return;
  }

  g_object_set(channel_thread,
	       "processing", processing,
	       NULL);
}

/**
 * ags_channel_thread_get_task_launcher:
 * @channel_thread: the #AgsChannelThread
 * 
 * Get task launcher.
 * 
 * Returns: (transfer full): the #AgsTaskLauncher
 * 
 * Since: 3.11.0
 */
AgsTaskLauncher*
ags_channel_thread_get_task_launcher(AgsChannelThread *channel_thread)
{
  AgsTaskLauncher *task_launcher;

  if(!AGS_IS_CHANNEL_THREAD(channel_thread)){
    return(NULL);
  }
  
  task_launcher = NULL;
  
  g_object_get(channel_thread,
	       "task-launcher", &task_launcher,
	       NULL);

  return(task_launcher);
}

/**
 * ags_channel_thread_set_sound_scope:
 * @channel_thread: the #AgsChannelThread
 * @sound_scope: the sound scope
 * 
 * Set sound scope.
 * 
 * Since: 3.0.0
 */
void
ags_channel_thread_set_sound_scope(AgsChannelThread *channel_thread,
				   gint sound_scope)
{
  GRecMutex *thread_mutex;
  
  if(!AGS_IS_CHANNEL_THREAD(channel_thread)){
    return;
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(channel_thread);

  /* set scope */
  g_rec_mutex_lock(thread_mutex);

  channel_thread->sound_scope = sound_scope;
  
  g_rec_mutex_unlock(thread_mutex);
}

/**
 * ags_channel_thread_get_do_fx_staging:
 * @channel_thread: the #AgsChannelThread
 * 
 * Get do fx staging.
 * 
 * Returns: %TRUE if set, otherwise %FALSE
 * 
 * Since: 3.3.0
 */
gboolean
ags_channel_thread_get_do_fx_staging(AgsChannelThread *channel_thread)
{
  gboolean do_fx_staging;

  GRecMutex *thread_mutex;

  if(!AGS_IS_CHANNEL_THREAD(channel_thread)){
    return(FALSE);
  }
  
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(channel_thread);

  /* get do fx staging */
  g_rec_mutex_lock(thread_mutex);

  do_fx_staging = channel_thread->do_fx_staging;

  g_rec_mutex_unlock(thread_mutex);

  return(do_fx_staging);
}

/**
 * ags_channel_thread_set_do_fx_staging:
 * @channel_thread: the #AgsChannelThread
 * @do_fx_staging: %TRUE if do fx staging, else %FALSe
 * 
 * Set do fx staging.
 * 
 * Since: 3.3.0
 */
void
ags_channel_thread_set_do_fx_staging(AgsChannelThread *channel_thread, gboolean do_fx_staging)
{
  GRecMutex *thread_mutex;

  if(!AGS_IS_CHANNEL_THREAD(channel_thread)){
    return;
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(channel_thread);

  /* get do fx staging */
  g_rec_mutex_lock(thread_mutex);

  channel_thread->do_fx_staging = do_fx_staging;

  g_rec_mutex_unlock(thread_mutex);
}

/**
 * ags_channel_thread_get_staging_program:
 * @channel_thread: the #AgsChannelThread
 * @staging_program_count: (out): the staging program count
 * 
 * Get staging program.
 * 
 * Returns: (transfer full): the staging program
 * 
 * Since: 3.3.0
 */
guint*
ags_channel_thread_get_staging_program(AgsChannelThread *channel_thread,
				       guint *staging_program_count)
{
  guint *staging_program;

  GRecMutex *thread_mutex;

  if(!AGS_IS_CHANNEL_THREAD(channel_thread)){
    if(staging_program_count != NULL){
      staging_program_count[0] = 0;
    }
    
    return(NULL);
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(channel_thread);

  /* get staging program */
  staging_program = NULL;

  g_rec_mutex_lock(thread_mutex);

  if(channel_thread->staging_program_count > 0){
    staging_program = (guint *) g_malloc(channel_thread->staging_program_count * sizeof(guint));

    memcpy(staging_program, channel_thread->staging_program, channel_thread->staging_program_count * sizeof(guint));
  }

  if(staging_program_count != NULL){
    staging_program_count[0] = channel_thread->staging_program_count;
  }

  g_rec_mutex_unlock(thread_mutex);

  return(staging_program);
}

/**
 * ags_channel_thread_set_staging_program:
 * @channel_thread: the #AgsChannelThread
 * @staging_program: (transfer none): the staging program
 * @staging_program_count: the staging program count
 * 
 * Set staging program.
 * 
 * Since: 3.3.0
 */
void
ags_channel_thread_set_staging_program(AgsChannelThread *channel_thread,
				       guint *staging_program,
				       guint staging_program_count)
{
  GRecMutex *thread_mutex;

  if(!AGS_IS_CHANNEL_THREAD(channel_thread)){
    return;
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(channel_thread);

  /* set staging program */
  g_rec_mutex_lock(thread_mutex);

  g_free(channel_thread->staging_program);
  
  if(staging_program_count > 0){
    channel_thread->staging_program = (guint *) g_malloc(staging_program_count * sizeof(guint));

    memcpy(channel_thread->staging_program, staging_program, staging_program_count * sizeof(guint));
  }else{
    channel_thread->staging_program = NULL;
  }

  channel_thread->staging_program_count = staging_program_count;
  
  g_rec_mutex_unlock(thread_mutex);
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
 * Since: 3.0.0
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
