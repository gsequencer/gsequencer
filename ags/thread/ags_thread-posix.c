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

#include <ags/thread/ags_thread-posix.h>

#include <ags/object/ags_connectable.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_main_loop.h>
#include <ags/object/ags_async_queue.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_returnable_thread.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include <errno.h>

void ags_thread_class_init(AgsThreadClass *thread);
void ags_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_thread_init(AgsThread *thread);
void ags_thread_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_thread_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_thread_connect(AgsConnectable *connectable);
void ags_thread_disconnect(AgsConnectable *connectable);
void ags_thread_finalize(GObject *gobject);

guint ags_thread_real_clock(AgsThread *thread);

void ags_thread_real_start(AgsThread *thread);
void* ags_thread_timer(void *ptr);
void* ags_thread_loop(void *ptr);
void ags_thread_real_timelock(AgsThread *thread);
void* ags_thread_timelock_loop(void *ptr);
void ags_thread_real_stop(AgsThread *thread);

void ags_thread_interrupt_callback(AgsMainLoop *main_loop,
				   int sig,
				   guint time_cycle, guint *time_spent,
				   AgsThread *thread);
static void ags_thread_self_create();

/**
 * SECTION:ags_thread-posix
 * @short_description: threads
 * @title: AgsThread
 * @section_id:
 * @include: ags/thread/ags_thread.h
 *
 * The #AgsThread base class. It supports organizing them within a tree,
 * perform syncing and frequencies.
 */

enum{
  PROP_0,
  PROP_FREQUENCY,
};

enum{
  CLOCK,
  START,
  RUN,
  SUSPEND,
  RESUME,
  TIMELOCK,
  STOP,
  INTERRUPTED,
  LAST_SIGNAL,
};

static gpointer ags_thread_parent_class = NULL;
static guint thread_signals[LAST_SIGNAL];

/* Key for the thread-specific AgsThread */
static pthread_key_t ags_thread_key;

/* Once-only initialisation of the key */
static pthread_once_t ags_thread_key_once = PTHREAD_ONCE_INIT;

static pthread_mutex_t class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_thread_get_type()
{
  static GType ags_type_thread = 0;

  if(!ags_type_thread){
    const GTypeInfo ags_thread_info = {
      sizeof (AgsThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_thread_init,
    };


    const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_thread = g_type_register_static(G_TYPE_OBJECT,
					     "AgsThread\0",
					     &ags_thread_info,
					     0);
        
    g_type_add_interface_static(ags_type_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_thread);
}

void
ags_thread_class_init(AgsThreadClass *thread)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_thread_parent_class = g_type_class_peek_parent(thread);
  
  /* GObject */
  gobject = (GObjectClass *) thread;
  
  gobject->set_property = ags_thread_set_property;
  gobject->get_property = ags_thread_get_property;

  gobject->finalize = ags_thread_finalize;

  /* properties */
  /**
   * AgsThread:frequency:
   *
   * The frequency to run at in Hz.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_double("frequency\0",
				   "JIFFIE\0",
				   "JIFFIE\0",
				   0.01,
				   1000.0,
				   1000.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FREQUENCY,
				  param_spec);

  /* AgsThread */
  thread->clock = ags_thread_real_clock;
  
  thread->start = ags_thread_real_start;
  thread->run = NULL;
  thread->suspend = NULL;
  thread->resume = NULL;
  thread->timelock = ags_thread_real_timelock;
  thread->stop = ags_thread_real_stop;
  thread->interrupted = NULL;
  
  /* signals */
  /**
   * AgsThread::clock:
   * @thread: the #AgsThread
   *
   * The ::clock signal is invoked every thread tic.
   *
   * Returns: the number of cycles to perform
   * 
   * Since: 0.6.0
   */
  thread_signals[CLOCK] =
    g_signal_new("clock\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, clock),
		 NULL, NULL,
		 g_cclosure_user_marshal_UINT__VOID,
		 G_TYPE_UINT, 0);


  /**
   * AgsThread::start:
   * @thread: the #AgsThread
   *
   * The ::start signal is invoked as thread started.
   * 
   * Since: 0.5.0
   */
  thread_signals[START] =
    g_signal_new("start\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsThread::run:
   * @thread: the #AgsThread
   *
   * The ::run signal is invoked during run loop.
   * 
   * Since: 0.5.0
   */
  thread_signals[RUN] =
    g_signal_new("run\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, run),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsThread::suspend:
   * @thread: the #AgsThread
   *
   * The ::suspend signal is invoked during suspending.
   * 
   * Since: 0.5.0
   */
  thread_signals[SUSPEND] =
    g_signal_new("suspend\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, suspend),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsThread::resume:
   * @thread: the #AgsThread
   * @recall_id: the appropriate #AgsRecallID
   *
   * The ::resume signal is invoked during resuming.
   * 
   * Since: 0.5.0
   */
  thread_signals[RESUME] =
    g_signal_new("resume\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, resume),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsThread::timelock:
   * @thread: the #AgsThread
   * @recall_id: the appropriate #AgsRecallID
   *
   * The ::timelock signal is invoked as standard compution
   * time exceeded.
   * 
   * Since: 0.5.0
   */
  thread_signals[TIMELOCK] =
    g_signal_new("timelock\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, timelock),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsThread::stop:
   * @thread: the #AgsThread
   * @recall_id: the appropriate #AgsRecallID
   *
   * The ::stop signal is invoked as @thread stopped.
   * 
   * Since: 0.5.0
   */
  thread_signals[STOP] =
    g_signal_new("stop\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, stop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsThread::interrupted:
   * @thread: the #AgsThread
   * @sig: the signal number
   * @time_cycle: the cycles duration
   * @time_spent: the time spent
   *
   * The ::interrupted signal is invoked as @thread should resume from interrupt.
   *
   * Returns: the time spent
   *
   * Since: 0.7.46
   */
  thread_signals[INTERRUPTED] =
    g_signal_new("interrupted\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, interrupted),
		 NULL, NULL,
		 g_cclosure_user_marshal_UINT__INT_UINT_POINTER,
		 G_TYPE_UINT, 3,
		 G_TYPE_INT, G_TYPE_UINT, G_TYPE_POINTER);
}

void
ags_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_thread_connect;
  connectable->disconnect = ags_thread_disconnect;
}

void
ags_thread_init(AgsThread *thread)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  int err;
  
  /* insert audio loop mutex */
  attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));

  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  err = pthread_mutexattr_setprotocol(attr,
				      PTHREAD_PRIO_INHERIT);

  if(err != 0){
    g_warning("no priority inheritance\0");
  }
#endif
  
  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) thread,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* fields */
  g_atomic_int_set(&(thread->flags),
		   0);
  g_atomic_int_set(&(thread->sync_flags),
		   0);
  
  /* clock */
  thread->delay = 0;
  thread->tic_delay = 0;
  thread->current_tic = 0;

  thread->cycle_iteration = 0;
  g_atomic_int_set(&(thread->time_late),
		   0);
  thread->computing_time = (struct timespec *) malloc(sizeof(struct timespec));
  thread->computing_time->tv_sec = 0;
  thread->computing_time->tv_nsec = 0;
  
  /* thread, mutex and cond */
  thread->thread = (pthread_t *) malloc(sizeof(pthread_t));

  thread->thread_attr = (pthread_attr_t *) malloc(sizeof(pthread_attr_t));
  pthread_attr_init(thread->thread_attr);

  thread->freq = AGS_THREAD_DEFAULT_JIFFIE;

  thread->mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));

  pthread_mutexattr_init(thread->mutexattr);
  pthread_mutexattr_settype(thread->mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(thread->mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  thread->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(thread->mutex, thread->mutexattr);

  thread->cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(thread->cond, NULL);

  /* start notify */
  g_atomic_pointer_set(&(thread->start_queue),
		       NULL);
  
  thread->start_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(thread->start_mutex, NULL);

  thread->start_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(thread->start_cond, NULL);

  g_atomic_int_set(&(thread->start_wait),
		   FALSE);
  g_atomic_int_set(&(thread->start_done),
		   FALSE);

  /* barrier */
  thread->barrier = (pthread_barrier_t **) malloc(2 * sizeof(pthread_barrier_t *));
  thread->barrier[0] = (pthread_barrier_t *) malloc(sizeof(pthread_barrier_t));
  thread->barrier[1] = (pthread_barrier_t *) malloc(sizeof(pthread_barrier_t));
  
  thread->first_barrier = TRUE;
  thread->wait_count[0] = 1;
  thread->wait_count[1] = 1;

  /* timelock */
  thread->timelock_thread = (pthread_t *) malloc(sizeof(pthread_t));

  thread->timelock_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(thread->timelock_mutex, NULL);

  thread->timelock_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(thread->timelock_cond, NULL);

  thread->greedy_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(thread->greedy_mutex, NULL);

  
  thread->greedy_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(thread->greedy_cond, NULL);
  g_atomic_int_set(&(thread->locked_greedy),
		   0);

  thread->timelock.tv_sec = 0;
  thread->timelock.tv_nsec = floor(NSEC_PER_SEC /
				   AGS_THREAD_HERTZ_JIFFIE / 5);

  thread->greedy_locks = NULL;

  g_atomic_int_set(&(thread->timer_wait),
		   FALSE);
  g_atomic_int_set(&(thread->timer_expired),
		   FALSE);

  thread->suspend_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(thread->suspend_mutex, NULL);

  thread->timer_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(thread->timer_mutex, NULL);

  thread->timer_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(thread->timer_cond, NULL);

  /* tree */
  g_atomic_pointer_set(&(thread->parent),
		       NULL);
  g_atomic_pointer_set(&(thread->children),
		       NULL);
  
  g_atomic_pointer_set(&(thread->next),
		       NULL);
  g_atomic_pointer_set(&(thread->prev),
		       NULL);

  thread->data = NULL;
}

void
ags_thread_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsThread *thread;

  thread = AGS_THREAD(gobject);

  switch(prop_id){
  case PROP_FREQUENCY:
    {
      gdouble freq;

      freq = g_value_get_double(value);

      if(freq == thread->freq){
	return;
      }

      thread->freq = freq;
      thread->delay = (guint) ceil((AGS_THREAD_HERTZ_JIFFIE / thread->freq) / (AGS_THREAD_HERTZ_JIFFIE / AGS_THREAD_MAX_PRECISION));
      thread->tic_delay = 0;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_thread_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsThread *thread;

  thread = AGS_THREAD(gobject);

  switch(prop_id){
  case PROP_FREQUENCY:
    {
      g_value_set_double(value, thread->freq);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_thread_connect(AgsConnectable *connectable)
{
  AgsThread *thread, *child;

  thread = AGS_THREAD(connectable);
  
  if((AGS_THREAD_CONNECTED & (g_atomic_int_get(&(thread->flags)))) != 0){
    return;
  }  

#ifdef AGS_DEBUG
  g_message("connecting thread\0");
#endif

  child = g_atomic_pointer_get(&(thread->children));

  while(child != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(child));

    child = g_atomic_pointer_get(&(child->next));
  }
}

void
ags_thread_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_thread_finalize(GObject *gobject)
{
  AgsThread *thread, *parent;
  AgsMutexManager *mutex_manager;

  gboolean running;
  
  pthread_t *thread_ptr;
  pthread_attr_t *thread_attr;
  pthread_mutex_t *application_mutex;

  void *stackaddr;
  size_t stacksize;

  thread = AGS_THREAD(gobject);

  thread_attr = thread->thread_attr;
  thread_ptr = thread->thread;

  running = ((AGS_THREAD_RUNNING & (g_atomic_int_get(&(thread->flags)))) != 0) ? TRUE: FALSE;
  
#ifdef AGS_DEBUG
  g_message("fin %s\0", G_OBJECT_TYPE_NAME(gobject));
#endif
  
  if((parent = g_atomic_pointer_get(&(thread->parent))) != NULL){
    pthread_mutex_lock(parent->start_mutex);

    g_atomic_pointer_set(&(parent->start_queue),
			 g_list_remove(g_atomic_pointer_get(&(parent->start_queue)),
				       thread));
    
    pthread_mutex_unlock(parent->start_mutex);
    
    ags_thread_remove_child(parent,
			    thread);
  }

  /*  */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_remove(mutex_manager,
			   (GObject *) thread);
  
  /*  */
  free(thread->computing_time);

  /*  */
  free(thread->barrier[0]);
  free(thread->barrier[1]);

  free(thread->barrier);
  
  /*  */
  pthread_attr_getstack(thread_attr,
			&stackaddr, &stacksize);
  
  pthread_mutexattr_destroy(thread->mutexattr);
  pthread_mutex_destroy(thread->mutex);
  free(thread->mutex);

  pthread_cond_destroy(thread->cond);
  free(thread->cond);

  /*  */
  pthread_mutex_lock(thread->start_mutex);

  pthread_cond_destroy(thread->start_cond);
  free(thread->start_cond);
  
  pthread_mutex_unlock(thread->start_mutex);
  
  pthread_mutex_destroy(thread->start_mutex);
  free(thread->start_mutex);
  
  pthread_mutex_destroy(thread->timelock_mutex);
  free(thread->timelock_mutex);
  
  pthread_cond_destroy(thread->timelock_cond);
  free(thread->timelock_cond);
  
  pthread_mutex_destroy(thread->greedy_mutex);
  free(thread->greedy_mutex);
  
  pthread_cond_destroy(thread->greedy_cond);
  free(thread->greedy_cond);
  
  pthread_mutex_destroy(thread->suspend_mutex);
  free(thread->suspend_mutex);

  /*  */
  pthread_mutex_destroy(thread->timer_mutex);
  free(thread->timer_mutex);

  pthread_cond_destroy(thread->timer_cond);
  free(thread->timer_cond);
  
  /* call parent */
  G_OBJECT_CLASS(ags_thread_parent_class)->finalize(gobject);

  if(running){
    pthread_detach(*thread_ptr);
  }
  
  pthread_mutex_unlock(application_mutex);

  pthread_attr_destroy(thread_attr);
  free(thread_attr);

  //  free(*thread_ptr);
  free(thread_ptr);

  if(stackaddr != NULL){
    free(stackaddr);
  }
  
  pthread_exit(NULL);
}

void
ags_thread_resume_handler(int sig)
{
  if(ags_thread_self() == NULL){
    return;
  }

#ifdef AGS_DEBUG
  g_message("thread resume\0");
#endif

  g_atomic_int_and(&(ags_thread_self()->flags),
		   (~AGS_THREAD_SUSPENDED));

  ags_thread_resume(ags_thread_self());
}

void
ags_thread_suspend_handler(int sig)
{
#ifdef AGS_DEBUG
  g_message("thread suspend\0");
#endif

  if(ags_thread_self == NULL){
    return;
  }

  if ((AGS_THREAD_SUSPENDED & (g_atomic_int_get(&(ags_thread_self()->flags)))) != 0) return;

  g_atomic_int_or(&(ags_thread_self()->flags),
		  AGS_THREAD_SUSPENDED);

  ags_thread_suspend(ags_thread_self());

  do sigsuspend(&(ags_thread_self()->wait_mask)); while ((AGS_THREAD_SUSPENDED & (g_atomic_int_get(&(ags_thread_self()->flags)))) != 0);
}

/**
 * ags_thread_set_sync:
 * @thread: an #AgsThread
 * @tic: the tic as sync occured.
 * 
 * Unsets AGS_THREAD_WAIT_0, AGS_THREAD_WAIT_1 or AGS_THREAD_WAIT_2.
 * Additionaly the thread is woken up by this function if waiting.
 *
 * Since: 0.4.0
 */
void
ags_thread_set_sync(AgsThread *thread, guint tic)
{
  guint next_tic;
  guint flags, sync_flags;
  gboolean broadcast;
  gboolean waiting;

  if(thread == NULL){
    return;
  }

  broadcast = FALSE;
  waiting = FALSE;

  if(tic > 2){
    tic = tic % 3;
  }

  pthread_mutex_lock(thread->mutex);

  flags = g_atomic_int_get(&(thread->flags));
  sync_flags = g_atomic_int_get(&(thread->sync_flags));

  /* return if no immediate sync and initial run */
  if((AGS_THREAD_IMMEDIATE_SYNC & (g_atomic_int_get(&(thread->flags)))) == 0){
    if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0){
      pthread_mutex_unlock(thread->mutex);
      
      return;
    }
  }

  /* get next tic and check waiting */
  switch(tic){
  case 0:
    {
      if((AGS_THREAD_WAIT_0 & sync_flags) != 0){
	next_tic = 1;
	waiting = TRUE;
      }
    }
    break;
  case 1:
    {
      if((AGS_THREAD_WAIT_1 & sync_flags) != 0){
	next_tic = 2;
	waiting = TRUE;
      }
    }
    break;
  case 2:
    {
      if((AGS_THREAD_WAIT_2 & sync_flags) != 0){
	next_tic = 0;
	waiting = TRUE;
      }
    }
    break;
  }

  /* unset wait flag */
  switch(tic){
  case 0:
    {
      if((AGS_THREAD_WAIT_0 & sync_flags) != 0){
	g_atomic_int_and(&(thread->sync_flags),
			 (~AGS_THREAD_WAIT_0));
      }
    }
    break;
  case 1:
    {
      if((AGS_THREAD_WAIT_1 & sync_flags) != 0){
	g_atomic_int_and(&(thread->sync_flags),
			 (~AGS_THREAD_WAIT_1));
      }
    }
    break;
  case 2:
    {
      if((AGS_THREAD_WAIT_2 & sync_flags) != 0){
	g_atomic_int_and(&(thread->sync_flags),
			 (~AGS_THREAD_WAIT_2));
      }
    }
    break;
  }

  /* signal waiting thread */
  if((AGS_THREAD_WAITING & (g_atomic_int_get(&(thread->flags)))) != 0){
    g_atomic_int_and(&(thread->flags),
		     (~AGS_THREAD_WAITING));

    if(broadcast){
      pthread_cond_broadcast(thread->cond);
    }else{
      pthread_cond_signal(thread->cond);
    }
  }    

  thread->current_tic = next_tic;
  g_atomic_int_and(&(thread->flags),
		   (~AGS_THREAD_WAITING));

  /*  */
  pthread_mutex_unlock(thread->mutex);
}

/**
 * ags_thread_set_sync_all:
 * @thread: an #AgsThread
 * @tic: the tic as sync occured.
 * 
 * Calls ags_thread_set_sync() on all threads.
 *
 * Since: 0.4.0
 */
void
ags_thread_set_sync_all(AgsThread *thread, guint tic)
{
  AgsThread *main_loop;

  auto void ags_thread_set_sync_all_reset(AgsThread *thread);
  auto void ags_thread_set_sync_all_recursive(AgsThread *thread, guint tic);

  void ags_thread_set_sync_all_reset(AgsThread *thread){
    AgsThread *child;

    /* reset chaos tree */
    g_atomic_int_and(&(thread->flags),
		    ~AGS_THREAD_IS_CHAOS_TREE);
    
    /* descend */
    child = g_atomic_pointer_get(&(thread->children));

    while(child != NULL){
      ags_thread_set_sync_all_reset(child);
      
      child = g_atomic_pointer_get(&(child->next));
    }
    
    /* increment delay counter and set run per cycle */
    if(thread->freq >= AGS_THREAD_MAX_PRECISION){
      thread->tic_delay = 0;
    }else{
      if(thread->tic_delay < thread->delay){
	thread->tic_delay++;
      }else{
	thread->tic_delay = 0;
      }
    }
  }
  
  void ags_thread_set_sync_all_recursive(AgsThread *thread, guint tic){
    AgsThread *child;

    ags_thread_set_sync(thread, tic);

    child = g_atomic_pointer_get(&(thread->children));

    while(child != NULL){
      ags_thread_set_sync_all_recursive(child, tic);
      
      child = g_atomic_pointer_get(&(child->next));
    }
  }

  /* entry point */
  main_loop = ags_thread_get_toplevel(thread);

  ags_thread_set_sync_all_reset(main_loop);
  ags_thread_set_sync_all_recursive(main_loop, tic);
}

/**
 * ags_thread_reset_all:
 * @thread: the #AgsThread
 * 
 * Reset all threads. E.g. suspended threads. A synchronization
 * stage after #AgsAsyncQueue run.
 * 
 * Since: 0.7.109
 */
void
ags_thread_reset_all(AgsThread *thread)
{
  AgsThread *main_loop;

  auto void ags_thread_reset_all_recursive(AgsThread *thread);

  void ags_thread_reset_all_recursive(AgsThread *thread){
    AgsThread *child;

    /* check main loop monitor if suspended and interrupted */
    if((AGS_THREAD_SUSPENDED & g_atomic_int_get(&(thread->flags))) == 0){
      if((AGS_THREAD_INTERRUPTED & g_atomic_int_get(&(thread->sync_flags))) != 0){
	if(ags_main_loop_monitor(AGS_MAIN_LOOP(main_loop),
				 0, NULL)){
	  g_atomic_int_and(&(thread->sync_flags),
			   ~(AGS_THREAD_INTERRUPTED |
			     AGS_THREAD_MONITORING));
	}
      }
    }else{ 
      if((AGS_THREAD_INTERRUPTED & g_atomic_int_get(&(thread->sync_flags))) != 0){
	if(ags_main_loop_monitor(AGS_MAIN_LOOP(main_loop),
				 0, NULL)){
	  if((AGS_THREAD_RESUME_INTERRUPTED & g_atomic_int_get(&(thread->sync_flags))) != 0 ||
	     (AGS_THREAD_RECOVER_INTERRUPTED & g_atomic_int_get(&(thread->sync_flags))) != 0){
#ifdef AGS_PTHREAD_RESUME
	    pthread_resume(thread->thread);
#else
	    pthread_kill(*(thread->thread), AGS_THREAD_RESUME_SIG);
#endif
	  }
	
	  if((AGS_THREAD_RECOVER_INTERRUPTED & g_atomic_int_get(&(thread->sync_flags))) != 0 ||
	     ((AGS_THREAD_RESUME_INTERRUPTED & g_atomic_int_get(&(thread->sync_flags))) != 0 &&
	      (AGS_THREAD_MONITORING & g_atomic_int_get(&(thread->sync_flags))) != 0)){
	    g_atomic_int_and(&(thread->sync_flags),
			     ~(AGS_THREAD_INTERRUPTED |
			       AGS_THREAD_MONITORING));
	  }else{
	    g_atomic_int_or(&(thread->sync_flags),
			    AGS_THREAD_MONITORING);
	  }
	}else{
	  g_atomic_int_or(&(thread->sync_flags),
			  AGS_THREAD_MONITORING);
	}
      }
    }

    /* descend */
    child = g_atomic_pointer_get(&(thread->children));

    while(child != NULL){
      ags_thread_reset_all_recursive(child);
      
      child = g_atomic_pointer_get(&(child->next));
    }
  }

  /* entry point */
  main_loop = ags_thread_get_toplevel(thread);

  ags_thread_reset_all_recursive(main_loop);
}

/**
 * ags_thread_lock:
 * @thread: an #AgsThread
 * 
 * Locks the threads own mutex and sets the appropriate flag.
 *
 * Since: 0.4.0
 */
void
ags_thread_lock(AgsThread *thread)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  if(thread == NULL){
    return;
  }

  /* lookup mutices */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) thread);
  
  pthread_mutex_unlock(application_mutex);

  /* lock */
  pthread_mutex_lock(mutex);
  g_atomic_int_or(&(thread->flags),
		  (AGS_THREAD_LOCKED));
}

/**
 * ags_thread_trylock:
 * @thread: an #AgsThread
 * 
 * Locks the threads own mutex if available and sets the
 * appropriate flag and returning %TRUE. Otherwise return %FALSE
 * without lock.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 0.4.0
 */
gboolean
ags_thread_trylock(AgsThread *thread)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  if(thread == NULL){
    return(FALSE);
  }

  /* lookup mutices */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) thread);
  
  pthread_mutex_unlock(application_mutex);

  /* lock */
  if(pthread_mutex_trylock(mutex) != 0){      
    return(FALSE);
  }

  g_atomic_int_or(&(thread->flags),
		  (AGS_THREAD_LOCKED));
  
  return(TRUE);
}

/**
 * ags_thread_unlock:
 * @thread: an #AgsThread
 *
 * Unlocks the threads own mutex and unsets the appropriate flag.
 *
 * Since: 0.4.0
 */
void
ags_thread_unlock(AgsThread *thread)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  if(thread == NULL){
    return;
  }

  /* lookup mutices */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) thread);
  
  pthread_mutex_unlock(application_mutex);

  /* unlock */
  g_atomic_int_and(&(thread->flags),
		   (~AGS_THREAD_LOCKED));

  pthread_mutex_unlock(mutex);
}

/**
 * ags_thread_get_toplevel:
 * @thread: an #AgsThread
 *
 * Retrieve toplevel thread.
 *
 * Returns: the toplevevel #AgsThread
 *
 * Since: 0.4.0
 */
AgsThread*
ags_thread_get_toplevel(AgsThread *thread)
{
  if(thread == NULL){
    return(NULL);
  }

  while(g_atomic_pointer_get(&(thread->parent)) != NULL){
    thread = g_atomic_pointer_get(&(thread->parent));
  }

  return(thread);
}

/**
 * ags_thread_first:
 * @thread: an #AgsThread
 *
 * Retrieve first sibling.
 *
 * Returns: the very first #AgsThread within same tree level
 *
 * Since: 0.4.0
 */
AgsThread*
ags_thread_first(AgsThread *thread)
{
  if(thread == NULL){
    return(NULL);
  }

  while(g_atomic_pointer_get(&(thread->prev)) != NULL){
    thread = g_atomic_pointer_get(&(thread->prev));
  }

  return(thread);
}

/**
 * ags_thread_last:
 * @thread: an #AgsThread
 * 
 * Retrieve last sibling.
 *
 * Returns: the very last @AgsThread within same tree level
 *
 * Since: 0.4.0
 */
AgsThread*
ags_thread_last(AgsThread *thread)
{
  if(thread == NULL){
    return(NULL);
  }

  while(g_atomic_pointer_get(&(thread->next)) != NULL){
    thread = g_atomic_pointer_get(&(thread->next));
  }

  return(thread);
}

/**
 * ags_thread_remove_child:
 * @thread: an #AgsThread
 * @child: the child to remove
 * 
 * Remove child of thread.
 *
 * Since: 0.4.0
 */
void
ags_thread_remove_child(AgsThread *thread, AgsThread *child)
{
  AgsThread *main_loop;
  AgsThread *prev, *next;
  
  if(thread == NULL || child == NULL){
    return;
  }

  main_loop = ags_thread_get_toplevel(thread);

  //  pthread_mutex_lock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));

  if(g_atomic_pointer_get(&(thread->children)) == child){
    g_atomic_pointer_set(&(thread->children),
			 g_atomic_pointer_get(&(child->next)));
  }

  //FIXME:JK: should be protected by a mutex
  if((prev = g_atomic_pointer_get(&(child->prev))) != NULL){
    g_atomic_pointer_set(&(prev->next),
			 g_atomic_pointer_get(&(child->next)));
  }

  if((next = g_atomic_pointer_get(&(child->next))) != NULL){
    g_atomic_pointer_set(&(next->prev),
			 g_atomic_pointer_get(&(child->prev)));
  }

  g_atomic_pointer_set(&(child->parent),
		       NULL);
  g_atomic_pointer_set(&(child->prev),
		       NULL);
  g_atomic_pointer_set(&(child->next),
		       NULL);

  g_object_unref(thread);
  g_object_unref(child);

  //  pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
}

/**
 * ags_thread_add_child:
 * @thread: an #AgsThread
 * @child: the child to remove
 * 
 * Add child to thread.
 *
 * Since: 0.4.0
 */
void
ags_thread_add_child(AgsThread *thread, AgsThread *child)
{
  ags_thread_add_child_extended(thread, child,
				FALSE, TRUE);
}

/**
 * ags_thread_add_child_extended:
 * @thread: an #AgsThread
 * @child: the child to remove
 * @no_start: don't start thread
 * @no_wait: don't wait until started
 * 
 * Add child to thread.
 *
 * Since: 0.4.2
 */
void
ags_thread_add_child_extended(AgsThread *thread, AgsThread *child,
			      gboolean no_start, gboolean no_wait)
{
  AgsThread *main_loop;
  
  if(thread == NULL ||
     child == NULL ||
     g_atomic_pointer_get(&(child->parent)) == thread){
    return;
  }

  main_loop = ags_thread_get_toplevel(thread);

  if(AGS_IS_MAIN_LOOP(main_loop)){
    pthread_mutex_lock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));

    g_signal_connect(AGS_MAIN_LOOP(main_loop), "interrupt\0",
		     G_CALLBACK(ags_thread_interrupt_callback), child);
  }
  
  g_object_ref(thread);
  g_object_ref(child);
  
  g_atomic_pointer_set(&(child->parent),
		       thread);

  if(g_atomic_pointer_get(&(thread->children)) == NULL){
    g_atomic_pointer_set(&(thread->children),
			 child);
  }else{
    AgsThread *sibling;

    sibling = ags_thread_last(g_atomic_pointer_get(&(thread->children)));

    g_atomic_pointer_set(&(sibling->next),
			 child);
    g_atomic_pointer_set(&(child->prev),
			 sibling);
  }
  
  if(AGS_IS_MAIN_LOOP(main_loop)){
    pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
  }
  
  if(!no_start){
    if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(thread->flags)))) == 0){
      /* start child */
      ags_thread_start(child);

      if(!no_wait){
	guint val;

	/* wait child */
	pthread_mutex_lock(child->start_mutex);

	g_atomic_int_set(&(child->start_wait),
			 TRUE);
	
	if(g_atomic_int_get(&(child->start_wait)) == TRUE &&
	   g_atomic_int_get(&(child->start_done)) == FALSE){
	  while(g_atomic_int_get(&(child->start_wait)) == TRUE &&
		g_atomic_int_get(&(child->start_done)) == FALSE){
	    pthread_cond_wait(child->start_cond,
			      child->start_mutex);
	  }
	}
	
	pthread_mutex_unlock(child->start_mutex);
      }
    }
  }
}

/**
 * ags_thread_parental_is_locked:
 * @thread: an #AgsThread
 * @parent: where to stop iteration
 *
 * Check the AGS_THREAD_LOCKED flag in parental levels.
 *
 * Returns: %TRUE if locked otherwise %FALSE
 *
 * Since: 0.4.0
 */
gboolean
ags_thread_parental_is_locked(AgsThread *thread, AgsThread *parent)
{
  AgsThread *current;

  if(thread == NULL){
    return(FALSE);
  }

  current = g_atomic_pointer_get(&(thread->parent));

  while(current != parent){
    if((AGS_THREAD_LOCKED & (g_atomic_int_get(&(current->flags)))) != 0){

      return(TRUE);
    }

    current = g_atomic_pointer_get(&(current->parent));
  }

  return(FALSE);
}

/**
 * ags_thread_sibling_is_locked:
 * @thread: an #AgsThread
 *
 * Check the AGS_THREAD_LOCKED flag within sibling.
 *
 * Returns: %TRUE if locked otherwise %FALSE
 *
 * Since: 0.4.0
 */
gboolean
ags_thread_sibling_is_locked(AgsThread *thread)
{
  AgsThread *iter;
  
  if(thread == NULL){
    return(FALSE);
  }

  iter = ags_thread_first(thread);

  while(iter != NULL){
    if(iter != thread &&
       (AGS_THREAD_LOCKED & (g_atomic_int_get(&(iter->flags)))) != 0){
      return(TRUE);
    }

    iter = g_atomic_pointer_get(&(iter->next));
  }

  return(FALSE);
}


/**
 * ags_thread_children_is_locked:
 * @thread: an #AgsThread
 *
 * Check the AGS_THREAD_LOCKED flag within children.
 *
 * Returns: %TRUE if locked otherwise %FALSE
 *
 * Since: 0.4.0
 */
gboolean
ags_thread_children_is_locked(AgsThread *thread)
{
  auto gboolean ags_thread_children_is_locked_recursive(AgsThread *thread);

  gboolean ags_thread_children_is_locked_recursive(AgsThread *thread){
    AgsThread *child;

    if(thread == NULL){
      return(FALSE);
    }

    if((AGS_THREAD_LOCKED & (g_atomic_int_get(&(thread->flags)))) != 0){
      return(TRUE);
    }

    child = g_atomic_pointer_get(&(thread->children));

    while(child != NULL){
      if(ags_thread_children_is_locked_recursive(child)){
	return(TRUE);
      }

      child = g_atomic_pointer_get(&(child->next));
    }

    return(FALSE);
  }

  if(thread == NULL){
    return(FALSE);
  }

  return(ags_thread_children_is_locked_recursive(thread));
}

gboolean
ags_thread_is_current_ready(AgsThread *current,
			    guint tic)
{

  AgsThread *toplevel;
  guint flags, sync_flags;
  gboolean retval;

  toplevel = ags_thread_get_toplevel(current);

  pthread_mutex_lock(current->mutex);

  flags = g_atomic_int_get(&(current->flags));
  sync_flags = g_atomic_int_get(&(current->sync_flags));

  retval = FALSE;

  if((AGS_THREAD_RUNNING & flags) == 0){
    retval = TRUE;
  }

  if((AGS_THREAD_IMMEDIATE_SYNC & flags) == 0){
    if((AGS_THREAD_INITIAL_RUN & flags) != 0){
      pthread_mutex_unlock(current->mutex);

      return(FALSE);
    }
  }
  
  if((AGS_THREAD_READY & flags) != 0){
    retval = TRUE;
  }

  if((AGS_THREAD_SUSPENDED & flags) != 0 &&
     (AGS_THREAD_INTERRUPTED & sync_flags) != 0){
    retval = TRUE;
  }

  if(retval){
    pthread_mutex_unlock(current->mutex);

    return(TRUE);
  }

  if(tic > 2){
    tic = tic % 3;
  }

  switch(tic){
  case 0:
    {
      if((AGS_THREAD_WAIT_0 & sync_flags) == 0){
	retval = TRUE;
      }
    }
    break;
  case 1:
    {
      if((AGS_THREAD_WAIT_1 & sync_flags) == 0){
	retval = TRUE;
      }
    }
    break;
  case 2:
    {
      if((AGS_THREAD_WAIT_2 & sync_flags) == 0){
	retval = TRUE;
      }
    }
    break;
  }

  pthread_mutex_unlock(current->mutex);

  return(retval);
}

gboolean
ags_thread_is_tree_ready(AgsThread *thread,
			 guint tic)
{
  AgsThread *main_loop;
  gboolean retval;

  auto gboolean ags_thread_is_tree_ready_current_tic(AgsThread *current);
  auto gboolean ags_thread_is_tree_ready_recursive(AgsThread *current);

  gboolean ags_thread_is_tree_ready_current_tic(AgsThread *current){
    AgsThread *toplevel;
    guint flags, sync_flags;
    gboolean retval;

    toplevel = ags_thread_get_toplevel(current);

    pthread_mutex_lock(current->mutex);

    flags = g_atomic_int_get(&(current->flags));
    sync_flags = g_atomic_int_get(&(current->sync_flags));
    
    retval = FALSE;

    if((AGS_THREAD_RUNNING & flags) == 0){
      retval = TRUE;
    }

    if((AGS_THREAD_IMMEDIATE_SYNC & (g_atomic_int_get(&(thread->flags)))) == 0){
      if((AGS_THREAD_INITIAL_RUN & flags) != 0){
	pthread_mutex_unlock(current->mutex);
	
	return(FALSE);
      }
    }
    
    if((AGS_THREAD_READY & flags) != 0){
      retval = TRUE;
    }

    if((AGS_THREAD_SUSPENDED & flags) != 0 &&
       (AGS_THREAD_INTERRUPTED & sync_flags) != 0){
      retval = TRUE;
    }

    if(retval){
      pthread_mutex_unlock(current->mutex);

      return(TRUE);
    }

    if(tic > 2){
      tic = tic % 3;
    }

    switch(tic){
    case 0:
      {
	if((AGS_THREAD_WAIT_0 & sync_flags) != 0){
	  retval = TRUE;
	}
      }
      break;
    case 1:
      {
	if((AGS_THREAD_WAIT_1 & sync_flags) != 0){
	  retval = TRUE;
	}
      }
      break;
    case 2:
      {
	if((AGS_THREAD_WAIT_2 & sync_flags) != 0){
	  retval = TRUE;
	}
      }
      break;
    }

    pthread_mutex_unlock(current->mutex);

    return(retval);
  }
  gboolean ags_thread_is_tree_ready_recursive(AgsThread *current){
    AgsThread *children;

    children = g_atomic_pointer_get(&(current->children));

    if(!ags_thread_is_tree_ready_current_tic(current)){
      return(FALSE);
    }

    while(children != NULL){
      if(!ags_thread_is_tree_ready_recursive(children)){
	return(FALSE);
      }

      children = g_atomic_pointer_get(&(children->next));
    }

    return(TRUE);
  }

  main_loop = ags_thread_get_toplevel(thread);

  retval = ags_thread_is_tree_ready_recursive(main_loop);

  return(retval);
}

/**
 * ags_thread_next_parent_locked:
 * @thread: an #AgsThread
 * @parent: the parent #AgsThread where to stop.
 * 
 * Retrieve next locked thread above @thread.
 *
 * Returns: next matching #AgsThread
 *
 * Since: 0.4.0
 */
AgsThread*
ags_thread_next_parent_locked(AgsThread *thread, AgsThread *parent)
{
  AgsThread *current;

  current = g_atomic_pointer_get(&(thread->parent));

  while(current != parent){
    if((AGS_THREAD_WAITING_FOR_CHILDREN & (g_atomic_int_get(&(current->sync_flags)))) != 0){
      return(current);
    }

    current = g_atomic_pointer_get(&(current->parent));
  }

  return(NULL);
}

/**
 * ags_thread_next_sibling_locked:
 * @thread: an #AgsThread
 *
 * Retrieve next locked thread neighbooring @thread
 *
 * Returns: next matching #AgsThread
 *
 * Since: 0.4.0
 */
AgsThread*
ags_thread_next_sibling_locked(AgsThread *thread)
{
  AgsThread *current;

  current = ags_thread_first(thread);

  while(current != NULL){
    if(current == thread){
      current = g_atomic_pointer_get(&(current->next));
      
      continue;
    }

    if((AGS_THREAD_WAITING_FOR_SIBLING & (g_atomic_int_get(&(thread->sync_flags)))) != 0){
      return(current);
    }

    current = g_atomic_pointer_get(&(current->next));
  }

  return(NULL);
}

/**
 * ags_thread_next_children_locked:
 * @thread: an #AgsThread
 * 
 * Retrieve next locked thread following @thread
 *
 * Returns: next matching #AgsThread
 *
 * Since: 0.4.0
 */
AgsThread*
ags_thread_next_children_locked(AgsThread *thread)
{
  auto AgsThread* ags_thread_next_children_locked_recursive(AgsThread *thread);

  AgsThread* ags_thread_next_children_locked_recursive(AgsThread *child){
    AgsThread *current;

    current = ags_thread_last(child);

    while(current != NULL){
      ags_thread_next_children_locked_recursive(g_atomic_pointer_get(&(current->children)));

      if((AGS_THREAD_WAITING_FOR_PARENT & (g_atomic_int_get(&(current->sync_flags)))) != 0){
	return(current);
      }

      current = g_atomic_pointer_get(&(current->prev));
    }

    return(NULL);
  }

  return(ags_thread_next_children_locked(g_atomic_pointer_get(&(thread->children))));
}

/**
 * ags_thread_lock_parent:
 * @thread: an #AgsThread
 * @parent: the parent #AgsThread where to stop.
 *
 * Lock parent tree structure.
 *
 * Since: 0.4.0
 */
void
ags_thread_lock_parent(AgsThread *thread, AgsThread *parent)
{
  AgsThread *current;

  if(thread == NULL){
    return;
  }

  ags_thread_lock(thread);

  current = g_atomic_pointer_get(&(thread->parent));

  while(current != parent){
    ags_thread_lock(current);
    g_atomic_int_or(&(current->sync_flags),
		    AGS_THREAD_WAITING_FOR_CHILDREN);

    current = g_atomic_pointer_get(&(current->parent));
  }
}

/**
 * ags_thread_lock_sibling:
 * @thread: an #AgsThread
 *
 * Lock sibling tree structure.
 *
 * Since: 0.4.0
 */
void
ags_thread_lock_sibling(AgsThread *thread)
{
  AgsThread *current;

  if(thread == NULL){
    return;
  }

  ags_thread_lock(thread);

  current = ags_thread_first(thread);

  while(current != NULL){
    if(current == thread){
      current = g_atomic_pointer_get(&(current->next));
    
      continue;
    }

    ags_thread_lock(current);
    g_atomic_int_or(&(current->sync_flags),
		    AGS_THREAD_WAITING_FOR_SIBLING);

    current = g_atomic_pointer_get(&(current->next));
  }
}

/**
 * ags_thread_lock_children:
 * @thread: an #AgsThread
 *
 * Lock child tree structure.
 *
 * Since: 0.4.0
 */
void
ags_thread_lock_children(AgsThread *thread)
{
  auto void ags_thread_lock_children_recursive(AgsThread *child);
  
  void ags_thread_lock_children_recursive(AgsThread *child){
    AgsThread *current;

    current = ags_thread_last(child);

    while(current != NULL){
      ags_thread_lock_children_recursive(g_atomic_pointer_get(&(current->children)));

      ags_thread_lock(current);
      g_atomic_int_or(&(current->sync_flags),
		      AGS_THREAD_WAITING_FOR_PARENT);
      
      current = g_atomic_pointer_get(&(current->prev));
    }
  }

  ags_thread_lock(thread);
  
  ags_thread_lock_children_recursive(g_atomic_pointer_get(&(thread->children)));
}

void
ags_thread_lock_all(AgsThread *thread)
{
  ags_thread_lock_parent(thread, NULL);
  ags_thread_lock_sibling(thread);
  ags_thread_lock_children(thread);
}

/**
 * ags_thread_unlock_parent:
 * @thread: an #AgsThread
 * @parent: the parent #AgsThread where to stop.
 *
 * Unlock parent tree structure.
 *
 * Since: 0.4.0
 */
void
ags_thread_unlock_parent(AgsThread *thread, AgsThread *parent)
{
  AgsThread *current;

  if(thread == NULL){
    return;
  }

  current = g_atomic_pointer_get(&(thread->parent));

  while(current != parent){
    g_atomic_int_and(&(current->sync_flags),
		     (~AGS_THREAD_WAITING_FOR_CHILDREN));

    if((AGS_THREAD_BROADCAST_PARENT & (g_atomic_int_get(&(thread->sync_flags)))) == 0){
      pthread_cond_signal(current->cond);
    }else{
      pthread_cond_broadcast(current->cond);
    }

    ags_thread_unlock(current);

    current = g_atomic_pointer_get(&(current->parent));
  }
}

/**
 * ags_thread_unlock_sibling:
 * @thread: an #AgsThread
 *
 * Unlock sibling tree structure.
 *
 * Since: 0.4.0
 */
void
ags_thread_unlock_sibling(AgsThread *thread)
{
  AgsThread *current;

  if(thread == NULL){
    return;
  }

  current = ags_thread_first(thread);

  while(current != NULL){
    if(current == thread){
      current = g_atomic_pointer_get(&(current->next));
    
      continue;
    }

    g_atomic_int_and(&(current->sync_flags),
		     (~AGS_THREAD_WAITING_FOR_SIBLING));

    if((AGS_THREAD_BROADCAST_SIBLING & (g_atomic_int_get(&(thread->sync_flags)))) == 0){
      pthread_cond_signal(current->cond);
    }else{
      pthread_cond_broadcast(current->cond);
    }

    ags_thread_unlock(current);

    current = g_atomic_pointer_get(&(current->next));
  }
}

/**
 * ags_thread_unlock_children:
 * @thread: an #AgsThread
 *
 * Unlock child tree structure.
 *
 * Since: 0.4.0
 */
void
ags_thread_unlock_children(AgsThread *thread)
{
  auto void ags_thread_unlock_children_recursive(AgsThread *child);
  
  void ags_thread_unlock_children_recursive(AgsThread *child){
    AgsThread *current;

    if(child == NULL){
      return;
    }

    current = ags_thread_last(child);

    while(current != NULL){
      ags_thread_unlock_children_recursive(g_atomic_pointer_get(&(current->children)));

      g_atomic_int_and(&(current->sync_flags),
		       (~AGS_THREAD_WAITING_FOR_PARENT));

      if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) == 0 &&
	 !AGS_IS_MAIN_LOOP(thread)){

	if((AGS_THREAD_BROADCAST_CHILDREN & (g_atomic_int_get(&(thread->sync_flags)))) == 0){
	  pthread_cond_signal(current->cond);
	}else{
	  pthread_cond_broadcast(current->cond);
	}
      }

      ags_thread_unlock(current);

      current = g_atomic_pointer_get(&(current->prev));
    }
  }
  
  ags_thread_unlock_children_recursive(g_atomic_pointer_get(&(thread->children)));
}

void
ags_thread_unlock_all(AgsThread *thread)
{
  ags_thread_unlock_parent(thread, NULL);
  ags_thread_unlock_sibling(thread);
  ags_thread_unlock_children(thread);
}

/**
 * ags_thread_wait_parent:
 * @thread: an #AgsThread
 * @parent: the parent #AgsThread where to stop.
 *
 * Wait on parent tree structure.
 *
 * Since: 0.4.0
 */
void
ags_thread_wait_parent(AgsThread *thread, AgsThread *parent)
{
  AgsThread *current;

  if(thread == NULL || thread == parent){
    return;
  }

  /* wait parent */
  current = g_atomic_pointer_get(&(thread->parent));
    
  while((current != NULL && current != parent) &&
	(((AGS_THREAD_IDLE & (g_atomic_int_get(&(current->flags)))) != 0 ||
	  (AGS_THREAD_WAITING_FOR_CHILDREN & (g_atomic_int_get(&(current->sync_flags)))) == 0) ||
	 g_atomic_pointer_get(&(current->parent)) != parent)){
    pthread_cond_wait(current->cond,
		      current->mutex);
    
    if(!((AGS_THREAD_IDLE & (g_atomic_int_get(&(current->flags)))) != 0 ||
	 (AGS_THREAD_WAITING_FOR_CHILDREN & (g_atomic_int_get(&(current->sync_flags)))) == 0)){
      current = g_atomic_pointer_get(&(current->parent));
    }
  }

  /* unset flag */
  g_atomic_int_and(&(thread->sync_flags),
		   (~AGS_THREAD_WAITING_FOR_PARENT));
}

/**
 * ags_thread_wait_sibling:
 * @thread: an #AgsThread
 *
 * Wait on sibling tree structure.
 *
 * Since: 0.4.0
 */
void
ags_thread_wait_sibling(AgsThread *thread)
{
  AgsThread *current;

  if(thread == NULL){
    return;
  }

  /* wait sibling */
  current = ags_thread_first(thread);
  
  while(current != NULL &&
	(((AGS_THREAD_IDLE & (g_atomic_int_get(&(current->flags)))) != 0 ||
	  (AGS_THREAD_WAITING_FOR_SIBLING & (g_atomic_int_get(&(current->sync_flags)))) == 0) ||
	 g_atomic_pointer_get(&(current->next)) != NULL)){
    if(current == thread){
      current = g_atomic_pointer_get(&(current->next));
      
      continue;
    }
    
    pthread_cond_wait(current->cond,
		      current->mutex);
    
    if(!((AGS_THREAD_IDLE & (g_atomic_int_get(&(current->flags)))) != 0 ||
	 (AGS_THREAD_WAITING_FOR_SIBLING & (g_atomic_int_get(&(current->sync_flags)))) == 0)){
      current = g_atomic_pointer_get(&(current->next));
    }
  }

  /* unset flags */
  g_atomic_int_and(&(thread->sync_flags),
		   (~AGS_THREAD_WAITING_FOR_SIBLING));
}

/**
 * ags_thread_wait_children:
 * @thread: an #AgsThread
 *
 * Wait on child tree structure.
 *
 * Since: 0.4.0
 */
void
ags_thread_wait_children(AgsThread *thread)
{
  auto void ags_thread_wait_children_recursive(AgsThread *child);
  
  void ags_thread_wait_children_recursive(AgsThread *child){
    gboolean initial_run;

    if(child == NULL){
      return;
    }

    initial_run = TRUE;

    while(child != NULL &&
	  (((AGS_THREAD_IDLE & (g_atomic_int_get(&(child->flags)))) != 0 ||
	    (AGS_THREAD_WAITING_FOR_PARENT & (g_atomic_int_get(&(child->sync_flags)))) == 0) ||
	   g_atomic_pointer_get(&(child->next)) != NULL)){
      if(initial_run){
	ags_thread_wait_children_recursive(g_atomic_pointer_get(&(child->children)));

	initial_run = FALSE;
      }

      pthread_cond_wait(child->cond,
			child->mutex);
     
      if(!((AGS_THREAD_IDLE & (g_atomic_int_get(&(child->flags)))) != 0 ||
	   (AGS_THREAD_WAITING_FOR_PARENT & (g_atomic_int_get(&(child->sync_flags)))) == 0)){
	child = g_atomic_pointer_get(&(child->next));

	initial_run = TRUE;
      }
    }
  }

  if(thread == NULL){
    return;
  }

  /* wait children */
  ags_thread_wait_children_recursive(g_atomic_pointer_get(&(thread->children)));

  /* unset flags */
  g_atomic_int_and(&(thread->sync_flags),
		   (~AGS_THREAD_WAITING_FOR_CHILDREN));
}

/**
 * ags_thread_signal_parent:
 * @thread: an #AgsThread
 * @parent: the parent #AgsThread
 * @broadcast: whether to perforam a signal or to broadcast
 *
 * Signals the tree in higher levels.
 *
 * Since: 0.4.0
 */
void
ags_thread_signal_parent(AgsThread *thread, AgsThread *parent,
			 gboolean broadcast)
{
  AgsThread *current;

  current = g_atomic_pointer_get(&(thread->parent));

  while(current != NULL){
    if((AGS_THREAD_WAIT_FOR_CHILDREN & (g_atomic_int_get(&(current->sync_flags)))) != 0){
      if(!broadcast){
	pthread_cond_signal(current->cond);
      }else{
	pthread_cond_broadcast(current->cond);
      }
    }

    current = g_atomic_pointer_get(&(current->parent));
  }
}

/**
 * ags_thread_signal_sibling:
 * @thread: an #AgsThread
 * @broadcast: whether to perforam a signal or to broadcast
 *
 * Signals the tree on same level.
 *
 * Since: 0.4.0
 */
void
ags_thread_signal_sibling(AgsThread *thread, gboolean broadcast)
{
  AgsThread *current;

  current = ags_thread_first(thread);

  while(current != NULL){
    if((AGS_THREAD_WAIT_FOR_SIBLING & (g_atomic_int_get(&(current->sync_flags)))) != 0){
      if(!broadcast){
	pthread_cond_signal(current->cond);
      }else{
	pthread_cond_broadcast(current->cond);
      }
    }

    current = g_atomic_pointer_get(&(current->next));
  }
}

/**
 * ags_thread_signal_children:
 * @thread: an #AgsThread
 * @broadcast: whether to perforam a signal or to broadcast
 *
 * Signals the tree in lower levels.
 *
 * Since: 0.4.0
 */
void
ags_thread_signal_children(AgsThread *thread, gboolean broadcast)
{
  auto void ags_thread_signal_children_recursive(AgsThread *thread, gboolean broadcast);

  void ags_thread_signal_children_recursive(AgsThread *thread, gboolean broadcast){
    AgsThread *current;

    if(thread == NULL){
      return;
    }

    current = thread;

    while(current != NULL){
      if((AGS_THREAD_WAIT_FOR_PARENT & (g_atomic_int_get(&(current->sync_flags)))) != 0){
	if(!broadcast){
	  pthread_cond_signal(current->cond);
	}else{
	  pthread_cond_broadcast(current->cond);
	}
      }
      
      ags_thread_signal_children_recursive(current, broadcast);

      current = g_atomic_pointer_get(&(current->next));
    }
  }

  ags_thread_signal_children(g_atomic_pointer_get(&(thread->children)), broadcast);
}

/**
 * ags_thread_clock:
 * @thread: the #AgsThread instance
 *
 * Clock the thread.
 *
 * Returns: the cycles to be performed
 * 
 * Since: 0.6.42
 */
guint
ags_thread_real_clock(AgsThread *thread)
{
  AgsThread *main_loop, *async_queue;
  AgsMutexManager *mutex_manager;
  
  struct timespec time_now;

  gdouble main_loop_delay;
  gdouble delay_per_hertz;
  guint steps;
  gboolean async_queue_running;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex, *main_loop_mutex;
  pthread_cond_t *run_cond;
  pthread_mutex_t *run_mutex;

  auto void ags_thread_clock_sync(AgsThread *thread);
  auto void ags_thread_clock_wait_async();
    
  void ags_thread_clock_sync(AgsThread *thread){
    guint next_tic, current_tic;

    /* sync */
    pthread_mutex_lock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));

    if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0){
      thread->current_tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));

      if((AGS_THREAD_START_SYNCED_FREQ & (g_atomic_int_get(&(thread->flags)))) != 0 &&
	 g_atomic_pointer_get(&(thread->parent)) != NULL){
	AgsThread *chaos_tree;
	
	chaos_tree = ags_thread_chaos_tree(thread);

	/* set tic delay */
	if((AGS_THREAD_INTERMEDIATE_PRE_SYNC & (g_atomic_int_get(&(thread->flags)))) != 0){
	  /* intermediate pre sync */
	  if(chaos_tree->tic_delay > 0){
	    thread->tic_delay = chaos_tree->tic_delay - 1;
	  }else{
	    thread->tic_delay = thread->delay;
	  }
	}else if((AGS_THREAD_INTERMEDIATE_POST_SYNC & (g_atomic_int_get(&(thread->flags)))) != 0){
	  /* intermediate post sync */
	  if(chaos_tree->tic_delay < thread->delay){
	    thread->tic_delay = chaos_tree->tic_delay + 1;
	  }else{
	    thread->tic_delay = 0;
	  }
	}else{
	  /* ordinary sync */
	  thread->tic_delay = chaos_tree->tic_delay;
	}
	
	g_atomic_int_or(&(thread->flags),
			AGS_THREAD_SYNCED_FREQ);
      }
      
      g_atomic_int_and(&(thread->flags),
		       (~AGS_THREAD_INITIAL_RUN));
    }

    /* thread tree */
    current_tic = thread->current_tic;
    
    if(current_tic == 2){
      next_tic = 0;
    }else if(current_tic == 0){
      next_tic = 1;
    }else if(current_tic == 1){
      next_tic = 2;
    }

    switch(thread->current_tic){
    case 0:
      {
	g_atomic_int_or(&(thread->sync_flags),
			AGS_THREAD_WAIT_0);
      }
      break;
    case 1:
      {
	g_atomic_int_or(&(thread->sync_flags),
			AGS_THREAD_WAIT_1);
      }
      break;
    case 2:
      {
	g_atomic_int_or(&(thread->sync_flags),
			AGS_THREAD_WAIT_2);
      }
      break;
    }
    
    if(!ags_thread_is_tree_ready(thread,
				 current_tic)){
      //      ags_thread_hangcheck(main_loop);
      pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
      
      if(!ags_thread_is_current_ready(thread,
				      current_tic)){
	pthread_mutex_lock(thread->mutex);

	g_atomic_int_or(&(thread->flags),
			AGS_THREAD_WAITING);
	
	while(!ags_thread_is_current_ready(thread,
					   current_tic) &&
	      (AGS_THREAD_WAITING & (g_atomic_int_get(&(thread->flags)))) != 0){
	  pthread_cond_wait(thread->cond,
			    thread->mutex);
	}

	pthread_mutex_unlock(thread->mutex);
      }
    }else{
      /* async-queue */
      if(async_queue != NULL){
	if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(async_queue->flags)))) == 0){
	  ags_async_queue_set_run(AGS_ASYNC_QUEUE(async_queue),
				  FALSE);
	}
      }

      async_queue_running = ((AGS_THREAD_RUNNING & (g_atomic_int_get(&(async_queue->flags)))) != 0) ? TRUE: FALSE;
      
      ags_main_loop_set_last_sync(AGS_MAIN_LOOP(main_loop), thread->current_tic);
      ags_main_loop_set_tic(AGS_MAIN_LOOP(main_loop), next_tic);

      ags_thread_set_sync_all(main_loop, thread->current_tic);

      ags_thread_reset_all(main_loop);

      pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
    }  
  }
  
  void ags_thread_clock_wait_async(){
    if(async_queue == NULL){
      return;
    }
    
    /* async-queue */
    if(!AGS_IS_ASYNC_QUEUE(thread)){
      if(async_queue_running){
	pthread_mutex_lock(run_mutex);
	//	g_message("blocked\0");
	
	if(!ags_async_queue_is_run(AGS_ASYNC_QUEUE(async_queue)) &&
	   (AGS_THREAD_DONE_ASYNC_QUEUE & (g_atomic_int_get(&(thread->sync_flags)))) == 0){
	  ags_async_queue_increment_wait_ref(AGS_ASYNC_QUEUE(async_queue));
	  //	  g_message("wait\0");
	  
	  g_atomic_int_or(&(thread->sync_flags),
			  AGS_THREAD_WAIT_ASYNC_QUEUE);
	  
	  while(!ags_async_queue_is_run(AGS_ASYNC_QUEUE(async_queue)) &&
		ags_async_queue_get_wait_ref(AGS_ASYNC_QUEUE(async_queue)) != 0 &&
		(AGS_THREAD_WAIT_ASYNC_QUEUE & (g_atomic_int_get(&(thread->sync_flags)))) != 0 &&
		(AGS_THREAD_DONE_ASYNC_QUEUE & (g_atomic_int_get(&(thread->sync_flags)))) == 0){
	    pthread_cond_wait(run_cond,
			      run_mutex);
	  }
	}

	g_atomic_int_and(&(thread->sync_flags),
			 (~(AGS_THREAD_WAIT_ASYNC_QUEUE |
			    AGS_THREAD_DONE_ASYNC_QUEUE)));

	pthread_mutex_unlock(run_mutex);
      }
    }else{
      //      g_message("not blocked\0");
    }
  }

  main_loop = ags_thread_get_toplevel(thread);  
  steps = 0;

  /* async-queue */
  if(AGS_IS_MAIN_LOOP(main_loop)){
    async_queue = (AgsThread *) ags_main_loop_get_async_queue(AGS_MAIN_LOOP(main_loop));
    async_queue_running = FALSE;

    if(async_queue != NULL){
      run_mutex = ags_async_queue_get_run_mutex(AGS_ASYNC_QUEUE(async_queue));
      run_cond = ags_async_queue_get_run_cond(AGS_ASYNC_QUEUE(async_queue));
    }
  }else{
    async_queue = NULL;
  }
  
  /* lookup mutices */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  main_loop_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) main_loop);
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) thread);
  
  pthread_mutex_unlock(application_mutex);

  /* calculate main loop delay */
  pthread_mutex_lock(main_loop_mutex);
  
  main_loop_delay = AGS_THREAD_MAX_PRECISION / main_loop->freq / (AGS_THREAD_HERTZ_JIFFIE / AGS_THREAD_MAX_PRECISION);
  
  pthread_mutex_unlock(main_loop_mutex);
  
  /* calculate thread delay */
  pthread_mutex_lock(mutex);

  thread->delay = (guint) ceil((AGS_THREAD_HERTZ_JIFFIE / thread->freq) / (AGS_THREAD_HERTZ_JIFFIE / AGS_THREAD_MAX_PRECISION));
  delay_per_hertz = AGS_THREAD_HERTZ_JIFFIE / AGS_THREAD_MAX_PRECISION;

  if(thread->delay < thread->tic_delay){
    thread->tic_delay = thread->delay;
  }
  
  if(g_atomic_pointer_get(&(thread->parent)) == NULL){
    //    g_message("%d %d %f\0", thread->tic_delay, thread->delay, thread->freq);
  }  
  
  pthread_mutex_unlock(mutex);

  /* check initial sync or immediate sync */
  if((AGS_THREAD_IMMEDIATE_SYNC & (g_atomic_int_get(&(thread->flags)))) == 0){
    if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0 &&
       (AGS_THREAD_INITIAL_SYNC & (g_atomic_int_get(&(thread->flags)))) != 0){
      clock_gettime(CLOCK_MONOTONIC, thread->computing_time);

      g_atomic_int_and(&(thread->flags),
		       (~AGS_THREAD_INITIAL_SYNC));

      return(1);
    }
  }else{
    if((AGS_THREAD_INITIAL_SYNC & (g_atomic_int_get(&(thread->flags)))) != 0){
      g_atomic_int_and(&(thread->flags),
		      (~AGS_THREAD_INITIAL_SYNC));
    }
  }
  
  /* idle */
#ifdef AGS_USE_TIMER
  if(g_atomic_pointer_get(&(thread->parent)) == NULL){
    pthread_mutex_lock(thread->timer_mutex);
    
    if(!g_atomic_int_get(&(thread->timer_expired))){
      g_atomic_int_set(&(thread->timer_wait),
		       TRUE);
      
      while(!g_atomic_int_get(&(thread->timer_expired))){
	pthread_cond_wait(thread->timer_cond,
			  thread->timer_mutex);
      }
    }

    g_atomic_int_set(&(thread->timer_wait),
		     FALSE);
    g_atomic_int_set(&(thread->timer_expired),
		     FALSE);
	
    pthread_mutex_unlock(thread->timer_mutex);
  }
#else
  if(g_atomic_pointer_get(&(thread->parent)) == NULL){
    gdouble time_spent, relative_time_spent;
    gdouble time_cycle;

    static const gdouble nsec_per_jiffie = NSEC_PER_SEC / AGS_THREAD_HERTZ_JIFFIE;
    
    if(thread->tic_delay == thread->delay &&
       (AGS_THREAD_TIMING & (g_atomic_int_get(&(thread->flags)))) != 0){
      struct timespec timed_sleep = {
	0,
	0,
      };

      clock_gettime(CLOCK_MONOTONIC, &time_now);
      
      if(time_now.tv_sec == thread->computing_time->tv_sec + 1){
	time_spent = (time_now.tv_nsec) + (NSEC_PER_SEC - thread->computing_time->tv_nsec);
      }else if(time_now.tv_sec > thread->computing_time->tv_sec + 1){
	time_spent = (time_now.tv_sec - thread->computing_time->tv_sec) * NSEC_PER_SEC;
	time_spent += (time_now.tv_nsec - thread->computing_time->tv_nsec);
      }else{
	time_spent = time_now.tv_nsec - thread->computing_time->tv_nsec;
      }

      time_cycle = NSEC_PER_SEC / thread->freq;
      
      relative_time_spent = time_cycle - time_spent - g_atomic_int_get(&(thread->time_late)) - AGS_THREAD_TOLERANCE;

      if(relative_time_spent < 0.0){
	g_atomic_int_set(&(thread->time_late),
			 (guint) ceil(-1.25 * relative_time_spent));
      }else if(relative_time_spent > 0.0 &&
	       relative_time_spent < time_cycle){
	g_atomic_int_set(&(thread->time_late),
			 0);
	timed_sleep.tv_nsec = (long) relative_time_spent - (1.0 / 45.0) * time_cycle;
      
	nanosleep(&timed_sleep, NULL);
      }

      clock_gettime(CLOCK_MONOTONIC, thread->computing_time);
    }
  }
#endif
  
  //  g_message("%s\0", G_OBJECT_TYPE_NAME(thread));

  /* sync */  
  /* run in hierarchy */
  //  pthread_mutex_lock(thread->mutex);
  ags_thread_clock_sync(thread);

  ags_thread_clock_wait_async();
    
  //  pthread_mutex_unlock(thread->mutex);
    /* increment delay counter and set run per cycle */
  if(thread->tic_delay == 0){
    if(thread->freq >= AGS_THREAD_MAX_PRECISION){
      steps = 1.0 / thread->delay;
    }else{
      steps = 1;
    }
  }else{
    steps = 0;
  }

  /* nth cycle */
  if(thread->cycle_iteration != AGS_THREAD_MAX_PRECISION){
    thread->cycle_iteration++;
  }else{
    thread->cycle_iteration = 0;
  }
  
  return(steps);
}

/**
 * ags_thread_clock:
 * @thread: the #AgsThread instance
 *
 * Clock the thread.
 *
 * Returns: the number of cycles to perform.
 * 
 * Since: 0.4.0
 */
guint
ags_thread_clock(AgsThread *thread)
{
  guint thread_signal;
  guint cycles;
  
  pthread_mutex_lock(&class_mutex);

  thread_signal = thread_signals[CLOCK];

  pthread_mutex_unlock(&class_mutex);
  
  g_return_val_if_fail(AGS_IS_THREAD(thread), 0);

  g_object_ref(thread);
  g_signal_emit(thread,
		thread_signal, 0,
		&cycles);
  g_object_unref(thread);

  return(cycles);
}

void
ags_thread_real_start(AgsThread *thread)
{
  AgsMainLoop *main_loop;
  guint val;

  main_loop = AGS_MAIN_LOOP(ags_thread_get_toplevel(thread));
  
  /* set/unset run flags */
  g_atomic_int_or(&(thread->flags),
		  (AGS_THREAD_RUNNING |
		   AGS_THREAD_INITIAL_RUN |
		   AGS_THREAD_INITIAL_SYNC));

  g_atomic_int_and(&(thread->flags),
		   (~(AGS_THREAD_RT_SETUP |
		      AGS_THREAD_WAITING)));

  g_atomic_int_and(&(thread->sync_flags),
		   (~(AGS_THREAD_WAIT_0 |
		      AGS_THREAD_WAIT_1 |
		      AGS_THREAD_WAIT_2)));

  g_atomic_int_and(&(thread->sync_flags),
		   (~(AGS_THREAD_SYNCED_FREQ)));

#ifdef AGS_DEBUG
  g_message("thread start: %s\0", G_OBJECT_TYPE_NAME(thread));
#endif

  /* */
  val = g_atomic_int_get(&(thread->sync_flags));
  
  if((AGS_THREAD_TIMELOCK_RUN & val) != 0){
    pthread_create(thread->timelock_thread, NULL,
    		   ags_thread_timelock_loop, thread);
  }

  /*  */
  pthread_create(thread->thread, thread->thread_attr,
		 ags_thread_loop, thread);
}

/**
 * ags_thread_add_start_queue:
 * @thread: the #AgsThread
 * @child: the child #AgsThread to start
 *
 * Add @child to @thread's start queue.
 *
 * Since: 0.7.56
 */
void
ags_thread_add_start_queue(AgsThread *thread,
			   AgsThread *child)
{
  GList *start_queue;
  
  pthread_mutex_lock(thread->start_mutex);
  
  start_queue = g_atomic_pointer_get(&(thread->start_queue));
  start_queue = g_list_prepend(start_queue,
			       child);

  g_atomic_pointer_set(&(thread->start_queue),
		       start_queue);
  
  pthread_mutex_unlock(thread->start_mutex);
}

/**
 * ags_thread_add_start_queue:
 * @thread: the #AgsThread
 * @child: the children as #GList-struct containing #AgsThread to start
 *
 * Add @child to @thread's start queue.
 *
 * Since: 0.7.56
 */
void
ags_thread_add_start_queue_all(AgsThread *thread,
			       GList *child)
{
  GList *start_queue;
  
  pthread_mutex_lock(thread->start_mutex);
  
  start_queue = g_atomic_pointer_get(&(thread->start_queue));
  start_queue = g_list_concat(start_queue,
			      g_list_copy(child));

  g_atomic_pointer_set(&(thread->start_queue),
		       start_queue);
  
  pthread_mutex_unlock(thread->start_mutex);
}

/**
 * ags_thread_start:
 * @thread: the #AgsThread instance
 *
 * Start the thread.
 *
 * Since: 0.4.0
 */
void
ags_thread_start(AgsThread *thread)
{
  guint thread_signal;
  
  pthread_mutex_lock(&class_mutex);

  thread_signal = thread_signals[START];
  
  pthread_mutex_unlock(&class_mutex);

  g_return_if_fail(AGS_IS_THREAD(thread));

  if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(thread->flags)))) != 0){
    return;
  }
  
  g_object_ref(thread);
  g_signal_emit(thread,
		thread_signal, 0);
  g_object_unref(thread);
}

void*
ags_thread_timer(void *ptr)
{
  //TODO:JK: implement me
}

void*
ags_thread_loop(void *ptr)
{
  AgsThread *thread, *main_loop;
  AgsThread *async_queue;
  AgsMutexManager *mutex_manager;
  AgsThread *queued_thread;
  
  GList *start_start_queue, *start_queue, *start_queue_next;

  guint val, running, locked_greedy;
  guint i, i_stop;
  gboolean wait_for_parent, wait_for_sibling, wait_for_children;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  thread = (AgsThread *) ptr;
  
  pthread_once(&ags_thread_key_once, ags_thread_self_create);
  pthread_setspecific(ags_thread_key, thread);
  
  main_loop = ags_thread_get_toplevel(thread);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) thread);
  
  pthread_mutex_unlock(application_mutex);
  
  pthread_mutex_lock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));

  thread->current_tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));

  pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));

  running = g_atomic_int_get(&(thread->flags));
  
  /* get start computing time */
#ifndef AGS_USE_TIMER
  if(g_atomic_pointer_get(&(thread->parent)) == NULL){
    thread->delay = 
      thread->tic_delay = (AGS_THREAD_HERTZ_JIFFIE / thread->freq) / (AGS_THREAD_HERTZ_JIFFIE / AGS_THREAD_MAX_PRECISION);
    clock_gettime(CLOCK_MONOTONIC, thread->computing_time);
  }
#endif
  
  while((AGS_THREAD_RUNNING & running) != 0){
    /* start queue */
    pthread_mutex_lock(mutex);

    start_start_queue = 
      start_queue = g_atomic_pointer_get(&(thread->start_queue));
    g_atomic_pointer_set(&(thread->start_queue),
			 NULL);

    pthread_mutex_unlock(mutex);

    while(start_queue != NULL){
      start_queue_next = start_queue->next;
      
      queued_thread = (AgsThread *) start_queue->data;

      /*  */
      ags_thread_start(queued_thread);

      pthread_mutex_lock(queued_thread->start_mutex);

      if(g_atomic_int_get(&(queued_thread->start_done)) == FALSE){

	pthread_mutex_lock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));

	queued_thread->tic_delay = thread->tic_delay;

	pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
	
	g_atomic_int_set(&(queued_thread->start_wait),
			 TRUE);
      
	while(g_atomic_int_get(&(queued_thread->start_wait)) == TRUE &&
	      g_atomic_int_get(&(queued_thread->start_done)) == FALSE){
	  pthread_cond_wait(queued_thread->start_cond,
			    queued_thread->start_mutex);
	}
      }
      
      pthread_mutex_unlock(queued_thread->start_mutex);

      start_queue = start_queue_next;
    }

    g_list_free(start_start_queue);
    
    /* notify start */
    /* signal AgsAudioLoop */
    pthread_mutex_lock(thread->start_mutex);
      
    g_atomic_int_set(&(thread->start_done),
		     TRUE);    
      
    if(g_atomic_int_get(&(thread->start_wait)) == TRUE){
      pthread_cond_broadcast(thread->start_cond);
    }
      
    pthread_mutex_unlock(thread->start_mutex);

    /* barrier */
    if((AGS_THREAD_WAITING_FOR_BARRIER & (g_atomic_int_get(&(thread->sync_flags)))) != 0){
      int wait_count;

      if(thread->first_barrier){
	/* retrieve wait count */
	ags_thread_lock(thread);

	wait_count = thread->wait_count[0];

	ags_thread_unlock(thread);

	/* init and wait */
	pthread_barrier_init(thread->barrier[0], NULL, wait_count);
	pthread_barrier_wait(thread->barrier[0]);
      }else{
	/* retrieve wait count */
	ags_thread_lock(thread);

	wait_count = thread->wait_count[1];

	ags_thread_unlock(thread);

	/* init and wait */
	pthread_barrier_init(thread->barrier[1], NULL, wait_count);
	pthread_barrier_wait(thread->barrier[1]);
      }
    }
      
    /* run clock synchronization */
    i_stop = ags_thread_clock(thread);

    /* if still running */    
    running = g_atomic_int_get(&(thread->flags));

    for(i = 0; i < i_stop && (AGS_THREAD_RUNNING & running) != 0; i++){
      /* do cycles */
      running = g_atomic_int_get(&(thread->flags));

      /* set idle flag */
      g_atomic_int_or(&(thread->flags),
		      AGS_THREAD_IDLE);

      if((AGS_THREAD_WAIT_FOR_PARENT & (g_atomic_int_get(&(thread->sync_flags)))) != 0){
	wait_for_parent = TRUE;

	g_atomic_int_or(&(thread->sync_flags),
			AGS_THREAD_WAITING_FOR_PARENT);
	ags_thread_lock_parent(thread, NULL);
      }else{
	wait_for_parent = FALSE;
      }

      /* lock sibling */
      if((AGS_THREAD_WAIT_FOR_SIBLING & (g_atomic_int_get(&(thread->sync_flags)))) != 0){
	wait_for_sibling = TRUE;

	g_atomic_int_or(&(thread->sync_flags),
			AGS_THREAD_WAITING_FOR_SIBLING);
	ags_thread_lock_sibling(thread);
      }else{
	wait_for_sibling = FALSE;
      }

      /* lock_children */
      if((AGS_THREAD_WAIT_FOR_CHILDREN & (g_atomic_int_get(&(thread->sync_flags)))) != 0){
	wait_for_children = TRUE;

	g_atomic_int_or(&(thread->sync_flags),
			AGS_THREAD_WAITING_FOR_CHILDREN);
	ags_thread_lock_children(thread);
      }else{
	wait_for_children = FALSE;
      }

      /* skip very first sync of AgsAudioLoop */
      if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) == 0 &&
	 !AGS_IS_MAIN_LOOP(thread)){

	/* wait parent */
	if(wait_for_parent){
	  ags_thread_wait_parent(thread, NULL);
	}

	/* wait sibling */
	if(wait_for_sibling){
	  ags_thread_wait_sibling(thread);
	}

	/* wait children */
	if(wait_for_children){
	  ags_thread_wait_children(thread);
	}
      }

      /* check for greedy to announce */
      if(thread->greedy_locks != NULL){
	GList *greedy_locks;

	greedy_locks = thread->greedy_locks;
      
	while(greedy_locks != NULL){
	  pthread_mutex_lock(AGS_THREAD(greedy_locks->data)->greedy_mutex);

	  locked_greedy = g_atomic_int_get(&(AGS_THREAD(greedy_locks->data)->locked_greedy));
	  locked_greedy++;

	  g_atomic_int_set(&(AGS_THREAD(greedy_locks->data)->locked_greedy),
			   locked_greedy);

	  pthread_mutex_unlock(AGS_THREAD(greedy_locks->data)->greedy_mutex);

	  greedy_locks = greedy_locks->next;
	}
      }

      /* greedy work around */
      pthread_mutex_lock(thread->greedy_mutex);

      locked_greedy = g_atomic_int_get(&(thread->locked_greedy));

      if(locked_greedy != 0){
	while(locked_greedy != 0){
	  pthread_cond_wait(thread->greedy_cond,
			    thread->greedy_mutex);
	
	  locked_greedy = g_atomic_int_get(&(thread->locked_greedy));
	}
      }

      pthread_mutex_unlock(thread->greedy_mutex);
      
      /* run */
      if((AGS_THREAD_INTERRUPTED & (g_atomic_int_get(&(thread->sync_flags)))) == 0 &&
	 (AGS_THREAD_MONITORING & (g_atomic_int_get(&(thread->sync_flags)))) == 0){
	/* timed lock (suspend) */
	pthread_mutex_lock(thread->timelock_mutex);

	val = g_atomic_int_get(&(thread->sync_flags));

	if((AGS_THREAD_TIMELOCK_RUN & val) != 0){
	  guint locked;

	  locked = g_atomic_int_get(&(thread->sync_flags));
				
	  g_atomic_int_and(&(thread->sync_flags),
			   (~AGS_THREAD_TIMELOCK_WAIT));

	  if((AGS_THREAD_TIMELOCK_WAIT & locked) != 0){	
	    pthread_cond_signal(thread->timelock_cond);
	  }
	}

	pthread_mutex_unlock(thread->timelock_mutex);
	
	ags_thread_run(thread);
      }
      //    g_printf("%s\n\0", G_OBJECT_TYPE_NAME(thread));

      /*  */
      running = g_atomic_int_get(&(thread->flags));

      /* check for greedy to release */
      if(thread->greedy_locks != NULL){
	GList *greedy_locks;

	greedy_locks = thread->greedy_locks;
      
	while(greedy_locks != NULL){
	  pthread_mutex_lock(AGS_THREAD(greedy_locks->data)->greedy_mutex);

	  locked_greedy = g_atomic_int_get(&(AGS_THREAD(greedy_locks->data)->locked_greedy));
	  locked_greedy--;

	  g_atomic_int_set(&(AGS_THREAD(greedy_locks->data)->locked_greedy),
			   locked_greedy);

	  pthread_cond_signal(AGS_THREAD(greedy_locks->data)->greedy_cond);

	  pthread_mutex_unlock(AGS_THREAD(greedy_locks->data)->greedy_mutex);

	  greedy_locks = greedy_locks->next;
	}
      }

      /* unset idle flag */
      g_atomic_int_and(&(thread->flags),
		       (~AGS_THREAD_IDLE));

      /* unlock parent */
      if(wait_for_parent){
	ags_thread_unlock_parent(thread, NULL);
      }

      /* unlock sibling */
      if(wait_for_sibling){
	ags_thread_unlock_sibling(thread);
      }

      /* unlock children */
      if(wait_for_children){
	ags_thread_unlock_children(thread);
      }
    }
    
    /* yield */
    if(main_loop->tic_delay != main_loop->delay &&
       thread->cycle_iteration % (guint) floor(AGS_THREAD_HERTZ_JIFFIE / AGS_THREAD_YIELD_JIFFIE) == 0){
      pthread_yield();
    }    
  }

  g_object_ref(thread);
  
  pthread_mutex_lock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));

  if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0  ||
     (AGS_THREAD_INITIAL_SYNC & (g_atomic_int_get(&(thread->flags)))) != 0){
    thread->current_tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));    

    g_atomic_int_and(&(thread->flags),
		     (~AGS_THREAD_INITIAL_RUN));

  }

  switch(thread->current_tic){
  case 0:
    {
      g_atomic_int_or(&(thread->sync_flags),
		      AGS_THREAD_WAIT_0);
    }
    break;
  case 1:
    {
      g_atomic_int_or(&(thread->sync_flags),
		      AGS_THREAD_WAIT_1);
    }
    break;
  case 2:
    {
      g_atomic_int_or(&(thread->sync_flags),
		      AGS_THREAD_WAIT_2);
    }
    break;
  }

  if(ags_thread_is_tree_ready(thread,
			      thread->current_tic)){
    guint next_tic, current_tic;

    current_tic = thread->current_tic;
    
    /* thread tree */
    if(current_tic == 2){
      next_tic = 0;
    }else if(current_tic == 0){
      next_tic = 1;
    }else if(current_tic == 1){
      next_tic = 2;
    }
    
    /* async-queue */
    if(async_queue != NULL){
      ags_async_queue_set_run(AGS_ASYNC_QUEUE(async_queue),
			      FALSE);
    }

    ags_main_loop_set_last_sync(AGS_MAIN_LOOP(main_loop), thread->current_tic);
    ags_main_loop_set_tic(AGS_MAIN_LOOP(main_loop), next_tic);

    ags_thread_set_sync_all(main_loop, thread->current_tic);    
  }

  if((AGS_THREAD_UNREF_ON_EXIT & (g_atomic_int_get(&(thread->flags)))) != 0){
    ags_thread_remove_child(g_atomic_pointer_get(&(thread->parent)),
			    thread);
    g_object_unref(thread);
  }

  pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
  
#ifdef AGS_DEBUG
  g_message("thread finished\0");
#endif  
  
  g_object_ref(thread);
  
  /* exit thread */
  pthread_exit(NULL);
}

/**
 * ags_thread_run:
 * @thread: the #AgsThread instance
 * 
 * Only for internal use of ags_thread_loop but you may want to set the your very own
 * class function namely your thread's routine.
 *
 * Since: 0.4.0
 */
void
ags_thread_run(AgsThread *thread)
{
  guint thread_signal;
  
  pthread_mutex_lock(&class_mutex);

  thread_signal = thread_signals[RUN];
  
  pthread_mutex_unlock(&class_mutex);

  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(thread);
  g_signal_emit(thread,
		thread_signal, 0);
  g_object_unref(thread);
}

void
ags_thread_suspend(AgsThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[SUSPEND], 0);
  g_object_unref(G_OBJECT(thread));
}

void
ags_thread_resume(AgsThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[RESUME], 0);
  g_object_unref(G_OBJECT(thread));
}

void*
ags_thread_timelock_loop(void *ptr)
{
  AgsThread *thread;
  int retval;
  guint flags, sync_flags;

  thread = AGS_THREAD(ptr);

  //FIXME:JK: not thread safe
  
  flags = g_atomic_int_get(&(thread->flags));
  
  sync_flags = g_atomic_int_get(&(thread->sync_flags));
  
  pthread_mutex_lock(thread->timelock_mutex);

  g_atomic_int_or(&(thread->sync_flags),
		  AGS_THREAD_TIMELOCK_WAIT);

  while((AGS_THREAD_RUNNING & (flags)) != 0){

    g_atomic_int_or(&(thread->sync_flags),
		    AGS_THREAD_TIMELOCK_WAIT);

    sync_flags = g_atomic_int_get(&(thread->sync_flags));

    while((AGS_THREAD_TIMELOCK_WAIT & (sync_flags)) != 0){
      retval = pthread_cond_wait(thread->timelock_cond,
				 thread->timelock_mutex);

      sync_flags = g_atomic_int_get(&(thread->sync_flags));
    }

    nanosleep(&(thread->timelock), NULL);
    // ndelay(thread->timelock.tv_nsec);

    sync_flags = g_atomic_int_get(&(thread->sync_flags));

    if((AGS_THREAD_WAIT_0 & sync_flags ||
	AGS_THREAD_WAIT_1 & sync_flags ||
	AGS_THREAD_WAIT_2 & sync_flags) != 0){
#ifdef AGS_DEBUG
      g_message("thread in realtime\0");
#endif
    }else{
#ifdef AGS_DEBUG
      g_message("thread timelock\0");
#endif
      ags_thread_timelock(thread);
    }

    flags = g_atomic_int_get(&(thread->flags));
  }

  pthread_mutex_unlock(thread->timelock_mutex);
}

void
ags_thread_real_timelock(AgsThread *thread)
{
  g_atomic_int_or(&(thread->sync_flags),
		  AGS_THREAD_INTERRUPTED);
  
  /* throughput is mandatory */
#ifdef AGS_PTHREAD_SUSPEND
    pthread_suspend(thread->thread);
#else
    pthread_kill(*(thread->thread), AGS_THREAD_SUSPEND_SIG);
#endif
}

void
ags_thread_timelock(AgsThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[TIMELOCK], 0);
  g_object_unref(G_OBJECT(thread));
}

void
ags_thread_real_stop(AgsThread *thread)
{
  g_atomic_int_and(&(thread->flags),
		   (~AGS_THREAD_RUNNING));

  pthread_detach(*(thread->thread));
}

/**
 * ags_thread_stop:
 * @thread: the #AgsThread instance
 * 
 * Stop the threads loop by unsetting AGS_THREAD_RUNNING flag.
 *
 * Since: 0.4.0
 */
void
ags_thread_stop(AgsThread *thread)
{
  guint thread_signal;

  pthread_mutex_lock(&class_mutex);

  thread_signal = thread_signals[STOP];

  pthread_mutex_unlock(&class_mutex);

  g_return_if_fail(AGS_IS_THREAD(thread));

  if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(thread->flags)))) == 0){
    return;
  }

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signal, 0);
  g_object_unref(G_OBJECT(thread));
}

/**
 * ags_thread_interrupted:
 * @thread: the #AgsThread
 * @sig: signal number
 * @time_cycle: duration of the time cycle
 * @time_spent: time spent since last cycle
 *
 * Notify to resume interrupted thread.
 * 
 * Returns: the time spent
 *
 * Since: 0.7.46
 */
guint
ags_thread_interrupted(AgsThread *thread,
		       int sig,
		       guint time_cycle, guint *time_spent)
{
  guint retval;
  guint thread_signal;

  return(0);
  
  pthread_mutex_lock(&class_mutex);

  thread_signal = thread_signals[INTERRUPTED];

  pthread_mutex_unlock(&class_mutex);

  g_return_val_if_fail(AGS_IS_THREAD(thread),
		       0);

  if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(thread->flags)))) == 0){
    return(0);
  }

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signal,
		0,
		sig,
		time_cycle,
		time_spent,
		&retval);
  g_object_unref(G_OBJECT(thread));

  return(retval);
}

/**
 * ags_thread_hangcheck:
 * @thread: the #AgsThread instance
 *
 * Performs hangcheck of thread.
 *
 * Since: 0.4.0
 */
void
ags_thread_hangcheck(AgsThread *thread)
{
  AgsThread *toplevel;
  gboolean synced[3];

  auto void ags_thread_hangcheck_recursive(AgsThread *thread);
  auto void ags_thread_hangcheck_unsync_all(AgsThread *thread, gboolean broadcast);

  void ags_thread_hangcheck_recursive(AgsThread *thread){
    AgsThread *child;
    guint sync_flags;

    sync_flags = g_atomic_int_get(&(thread->sync_flags));

    if((AGS_THREAD_WAIT_0 & sync_flags) != 0){
      synced[0] = TRUE;
    }

    if((AGS_THREAD_WAIT_1 & sync_flags) != 0){
      synced[1] = TRUE;
    }

    if((AGS_THREAD_WAIT_2 & sync_flags) != 0){
      synced[2] = TRUE;
    }

    /* iterate tree */
    child = g_atomic_pointer_get(&(thread->children));

    while(child != NULL){
      ags_thread_hangcheck_recursive(child);

      child = g_atomic_pointer_get(&(child->next));
    }
  }
  void ags_thread_hangcheck_unsync_all(AgsThread *thread, gboolean broadcast){
    AgsThread *child;
    guint sync_flags;

    sync_flags = g_atomic_int_get(&(thread->sync_flags));
    g_atomic_int_and(&(thread->sync_flags),
		     (~(AGS_THREAD_WAIT_0 |
			AGS_THREAD_WAIT_1 |
			AGS_THREAD_WAIT_2)));

    if(AGS_THREAD_WAIT_0 & sync_flags){
      if(broadcast){
	pthread_cond_broadcast(thread->cond);
      }else{
	pthread_cond_signal(thread->cond);
      }
    }

    if(AGS_THREAD_WAIT_1 & sync_flags){
      if(broadcast){
	pthread_cond_broadcast(thread->cond);
      }else{
	pthread_cond_signal(thread->cond);
      }
    }

    if(AGS_THREAD_WAIT_2 & sync_flags){
      if(broadcast){
	pthread_cond_broadcast(thread->cond);
      }else{
	pthread_cond_signal(thread->cond);
      }
    }

    /* iterate tree */
    child = g_atomic_pointer_get(&(thread->children));

    while(child != NULL){
      ags_thread_hangcheck_unsync_all(child, broadcast);

      child = g_atomic_pointer_get(&(child->next));
    }
  }

  /* detect memory corruption */
  synced[0] = FALSE;
  synced[1] = FALSE;
  synced[2] = FALSE;

  /* fill synced array */
  toplevel = ags_thread_get_toplevel(thread);
  ags_thread_hangcheck_recursive(toplevel);
  
  /*  */
  if(!((synced[0] && !synced[1] && !synced[2]) ||
       (!synced[0] && synced[1] && !synced[2]) ||
       (!synced[0] && !synced[1] && synced[2]))){
    g_warning("thread tree hung up\0");

    ags_thread_hangcheck_unsync_all(toplevel, FALSE);
  }
}

AgsThread*
ags_thread_find_type(AgsThread *thread, GType type)
{
  AgsThread *current, *retval;

  if(thread == NULL || type == G_TYPE_NONE){
    return(NULL);
  }

  if(g_type_is_a(G_OBJECT_TYPE(thread), type)){
    return(thread);
  }
  
  current = g_atomic_pointer_get(&(thread->children));

  while(current != NULL){
    if((retval = ags_thread_find_type(current, type)) != NULL){
      return(retval);
    }
    
    current = g_atomic_pointer_get(&(current->next));
  }

  
  return(NULL);
}

void
ags_thread_interrupt_callback(AgsMainLoop *main_loop,
			      int sig,
			      guint time_cycle, guint *time_spent,
			      AgsThread *thread)
{
  ags_thread_interrupted(thread,
			 sig,
			 time_cycle, time_spent);
}

static void
ags_thread_self_create()
{
  pthread_key_create(&ags_thread_key, NULL);
}

AgsThread*
ags_thread_self(void)
{
  return((AgsThread *) pthread_getspecific(ags_thread_key));
}

/**
 * ags_thread_chaos_tree:
 * @thread: the #AgsThread
 *
 * Escape chaos tree, get syncing point.
 *
 * Returns: the first ordered thread
 *
 * Since: 0.7.40
 */
AgsThread*
ags_thread_chaos_tree(AgsThread *thread)
{
  while((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0 &&
	g_atomic_pointer_get(&(thread->parent)) != NULL){
    thread = g_atomic_pointer_get(&(thread->parent));
  }

  return(thread);
}

/**
 * ags_thread_is_chaos_tree:
 * @thread: the #AgsThread
 * @tic_delay: the tic to sync with
 * @is_chaos_tree: if %TRUE AGS_THREAD_IS_CHAOS_TREE flag is set 
 *
 * Check chaos tree and flag threads.
 *
 * Returns: %TRUE if tree is in chaos, otherwise %FALSE
 *
 * Since: 0.7.43
 */
gboolean
ags_thread_is_chaos_tree(AgsThread *thread,
			 guint tic_delay,
			 gboolean is_chaos_tree)
{
  AgsThread *child;
  
  gboolean retval;

  ags_thread_lock(thread);
  
  retval = is_chaos_tree;

  if(!is_chaos_tree){
    if((AGS_THREAD_INITIAL_SYNC & (g_atomic_int_get(&(thread->flags)))) != 0 ||
       (AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0){
      retval = TRUE;
    }
    
    if(!retval &&
       (AGS_THREAD_START_SYNCED_FREQ & (g_atomic_int_get(&(thread->flags)))) != 0 &&
       (AGS_THREAD_SYNCED_FREQ & (g_atomic_int_get(&(thread->flags)))) == 0){
      retval = TRUE;
    }

    if(!retval){
      child = g_atomic_pointer_get(&(thread->children));
      
      while(!retval &&
	    child != NULL){
	retval = ags_thread_is_chaos_tree(child,
					  tic_delay,
					  FALSE);

	child = g_atomic_pointer_get(&(child->next));
      }
    }
  }

  if(is_chaos_tree ||
     retval){
    /* flag thread */
    g_atomic_int_or(&(thread->flags),
		    AGS_THREAD_IS_CHAOS_TREE);
    
    /* apply recursive */
    child = g_atomic_pointer_get(&(thread->children));
      
    while(child != NULL){
      ags_thread_is_chaos_tree(child,
			       tic_delay,
			       TRUE);

      child = g_atomic_pointer_get(&(child->next));
    }    
  }
  
  ags_thread_unlock(thread);

  return(retval);
}

/**
 * ags_thread_new:
 * @data: an #GObject
 *
 * Create a new #AgsThread you may provide a #gpointer as @data
 * to your thread routine.
 *
 * Returns: the new #AgsThread
 * 
 * Since: 0.4.0
 */
AgsThread*
ags_thread_new(gpointer data)
{
  AgsThread *thread;

  pthread_mutex_lock(&class_mutex);

  thread = (AgsThread *) g_object_new(AGS_TYPE_THREAD,
				      NULL);

  pthread_mutex_unlock(&class_mutex);

  thread->data = data;

  return(thread);
}
