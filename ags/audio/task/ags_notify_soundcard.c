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

#include <ags/audio/task/ags_notify_soundcard.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/i18n.h>

void ags_notify_soundcard_class_init(AgsNotifySoundcardClass *notify_soundcard);
void ags_notify_soundcard_init(AgsNotifySoundcard *notify_soundcard);
void ags_notify_soundcard_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_notify_soundcard_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_notify_soundcard_dispose(GObject *gobject);
void ags_notify_soundcard_finalize(GObject *gobject);

void ags_notify_soundcard_launch(AgsTask *task);

/**
 * SECTION:ags_notify_soundcard
 * @short_description: notify soundcard task
 * @title: AgsNotifySoundcard
 * @section_id:
 * @include: ags/audio/task/ags_notify_soundcard.h
 *
 * The #AgsNotifySoundcard task notifys soundcard.
 */

static gpointer ags_notify_soundcard_parent_class = NULL;

enum{
  PROP_0,
  PROP_SOUNDCARD_THREAD,
};

GType
ags_notify_soundcard_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_notify_soundcard = 0;

    static const GTypeInfo ags_notify_soundcard_info = {
      sizeof(AgsNotifySoundcardClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_notify_soundcard_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsNotifySoundcard),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_notify_soundcard_init,
    };

    ags_type_notify_soundcard = g_type_register_static(AGS_TYPE_TASK,
						       "AgsNotifySoundcard",
						       &ags_notify_soundcard_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_notify_soundcard);
  }

  return g_define_type_id__volatile;
}

void
ags_notify_soundcard_class_init(AgsNotifySoundcardClass *notify_soundcard)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_notify_soundcard_parent_class = g_type_class_peek_parent(notify_soundcard);

  /* gobject */
  gobject = (GObjectClass *) notify_soundcard;

  gobject->set_property = ags_notify_soundcard_set_property;
  gobject->get_property = ags_notify_soundcard_get_property;

  gobject->dispose = ags_notify_soundcard_dispose;
  gobject->finalize = ags_notify_soundcard_finalize;

  /* properties */
  /**
   * AgsNotifySoundcard:soundcard-thread:
   *
   * The assigned #AgsSoundcardThread
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("soundcard-thread",
				   i18n_pspec("soundcard thread of notify soundcard"),
				   i18n_pspec("The soundcard thread of notify soundcard task"),
				   AGS_TYPE_SOUNDCARD_THREAD,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD_THREAD,
				  param_spec);
  
  /* task */
  task = (AgsTaskClass *) notify_soundcard;

  task->launch = ags_notify_soundcard_launch;
}

void
ags_notify_soundcard_init(AgsNotifySoundcard *notify_soundcard)
{
  notify_soundcard->return_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));

  pthread_mutexattr_init(notify_soundcard->return_mutexattr);
  pthread_mutexattr_settype(notify_soundcard->return_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(notify_soundcard->return_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  notify_soundcard->return_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(notify_soundcard->return_mutex, notify_soundcard->return_mutexattr);

  notify_soundcard->return_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(notify_soundcard->return_cond, NULL);

  /* soundcard */  
  notify_soundcard->soundcard_thread = NULL;
}

void
ags_notify_soundcard_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsNotifySoundcard *notify_soundcard;

  notify_soundcard = AGS_NOTIFY_SOUNDCARD(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD_THREAD:
    {
      AgsSoundcardThread *soundcard_thread;

      soundcard_thread = (AgsSoundcardThread *) g_value_get_object(value);

      if(notify_soundcard->soundcard_thread == soundcard_thread){
	return;
      }

      if(notify_soundcard->soundcard_thread != NULL){
	g_object_unref(notify_soundcard->soundcard_thread);
      }

      if(soundcard_thread != NULL){
	g_object_ref(soundcard_thread);
      }

      notify_soundcard->soundcard_thread = soundcard_thread;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_notify_soundcard_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsNotifySoundcard *notify_soundcard;

  notify_soundcard = AGS_NOTIFY_SOUNDCARD(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD_THREAD:
    {
      g_value_set_object(value, notify_soundcard->soundcard_thread);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_notify_soundcard_dispose(GObject *gobject)
{
  AgsNotifySoundcard *notify_soundcard;

  notify_soundcard = AGS_NOTIFY_SOUNDCARD(gobject);

  if(notify_soundcard->soundcard_thread != NULL){
    g_object_unref(notify_soundcard->soundcard_thread);

    notify_soundcard->soundcard_thread = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_notify_soundcard_parent_class)->dispose(gobject);
}

void
ags_notify_soundcard_finalize(GObject *gobject)
{
  AgsNotifySoundcard *notify_soundcard;

  notify_soundcard = AGS_NOTIFY_SOUNDCARD(gobject);

  pthread_mutex_destroy(notify_soundcard->return_mutex);
  free(notify_soundcard->return_mutex);
  
  pthread_cond_destroy(notify_soundcard->return_cond);
  free(notify_soundcard->return_cond);

  if(notify_soundcard->soundcard_thread != NULL){
    g_object_unref(notify_soundcard->soundcard_thread);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_notify_soundcard_parent_class)->finalize(gobject);
}

void
ags_notify_soundcard_launch(AgsTask *task)
{
  AgsNotifySoundcard *notify_soundcard;

  AgsSoundcardThread *soundcard_thread;

  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;
  
  notify_soundcard = AGS_NOTIFY_SOUNDCARD(task);

  soundcard_thread = notify_soundcard->soundcard_thread;

  application_context = ags_application_context_get_instance();

  g_object_get(application_context,
	       "task-thread", &task_thread,
	       NULL);
  
  if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(AGS_THREAD(soundcard_thread)->flags)))) == 0 ||
     (AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(AGS_THREAD(soundcard_thread)->flags)))) != 0 ||
     AGS_THREAD(soundcard_thread)->tic_delay != 0){
    return;
  }

  /* release soundcard thread of async queue */
  pthread_mutex_lock(task_thread->run_mutex);

  g_atomic_int_or(&(AGS_THREAD(soundcard_thread)->sync_flags),
		  AGS_THREAD_DONE_ASYNC_QUEUE);

  if((AGS_THREAD_WAIT_ASYNC_QUEUE & (g_atomic_int_get(&(AGS_THREAD(soundcard_thread)->sync_flags)))) != 0){
    pthread_cond_broadcast(task_thread->run_cond);
  }
  
  pthread_mutex_unlock(task_thread->run_mutex);

  /* wait to complete */
  pthread_mutex_lock(notify_soundcard->return_mutex);

  if((AGS_NOTIFY_SOUNDCARD_DONE_RETURN & (g_atomic_int_get(&(notify_soundcard->flags)))) == 0){
    g_atomic_int_or(&(notify_soundcard->flags),
		    AGS_NOTIFY_SOUNDCARD_WAIT_RETURN);
    
    while((AGS_NOTIFY_SOUNDCARD_WAIT_RETURN & (g_atomic_int_get(&(notify_soundcard->flags)))) != 0 &&
	  (AGS_NOTIFY_SOUNDCARD_DONE_RETURN & (g_atomic_int_get(&(notify_soundcard->flags)))) == 0){
      pthread_cond_wait(notify_soundcard->return_cond,
			notify_soundcard->return_mutex);
    }
  }

  g_atomic_int_and(&(notify_soundcard->flags),
		   (~(AGS_NOTIFY_SOUNDCARD_WAIT_RETURN |
		      AGS_NOTIFY_SOUNDCARD_DONE_RETURN)));
  
  pthread_mutex_unlock(notify_soundcard->return_mutex);
}

/**
 * ags_notify_soundcard_new:
 * @soundcard_thread: the #AgsSoundcardThread
 *
 * Create a new instance of #AgsNotifySoundcard.
 *
 * Returns: the new #AgsNotifySoundcard
 *
 * Since: 2.0.0
 */
AgsNotifySoundcard*
ags_notify_soundcard_new(AgsSoundcardThread *soundcard_thread)
{
  AgsNotifySoundcard *notify_soundcard;

  notify_soundcard = (AgsNotifySoundcard *) g_object_new(AGS_TYPE_NOTIFY_SOUNDCARD,
							 "soundcard-thread", soundcard_thread,
							 NULL);

  return(notify_soundcard);
}
