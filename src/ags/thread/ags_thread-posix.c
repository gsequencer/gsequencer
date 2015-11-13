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

void ags_thread_real_start(AgsThread *thread);
void* ags_thread_timer(void *ptr);
void* ags_thread_loop(void *ptr);
void ags_thread_real_timelock(AgsThread *thread);
void* ags_thread_timelock_loop(void *ptr);
void ags_thread_real_stop(AgsThread *thread);

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
  START,
  RUN,
  SUSPEND,
  RESUME,
  TIMELOCK,
  STOP,
  LAST_SIGNAL,
};

static gpointer ags_thread_parent_class = NULL;
static guint thread_signals[LAST_SIGNAL];

// __thread AgsThread *ags_thread_self = NULL;

/* Key for the thread-specific AgsThread */
static pthread_key_t ags_thread_key;

/* Once-only initialisation of the key */
static pthread_once_t ags_thread_key_once = PTHREAD_ONCE_INIT;

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
   * Since: 0.4
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
  thread->start = ags_thread_real_start;
  thread->run = NULL;
  thread->suspend = NULL;
  thread->resume = NULL;
  thread->timelock = ags_thread_real_timelock;
  thread->stop = ags_thread_real_stop;

  /* signals */
  /**
   * AgsThread::start:
   * @thread: the object playing.
   *
   * The ::start signal is invoked as thread started.
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
   * @thread: the object playing.
   *
   * The ::run signal is invoked during run loop.
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
   * @thread: the object playing.
   *
   * The ::suspend signal is invoked during suspending.
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
   * @thread: the object playing.
   * @recall_id: the appropriate #AgsRecallID
   *
   * The ::resume signal is invoked during resuming.
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
   * @thread: the object playing.
   * @recall_id: the appropriate #AgsRecallID
   *
   * The ::timelock signal is invoked as standard compution
   * time exceeded.
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
   * @thread: the object playing.
   * @recall_id: the appropriate #AgsRecallID
   *
   * The ::stop signal is invoked as @thread stopped.
   */
  thread_signals[STOP] =
    g_signal_new("stop\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, stop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
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
  pthread_mutexattr_t attr;

  /* insert audio loop mutex */
  //FIXME:JK: memory leak
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr,
			    PTHREAD_MUTEX_RECURSIVE);

  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     &attr);

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

  thread->thread = (pthread_t *) malloc(sizeof(pthread_t));

  pthread_attr_init(&(thread->thread_attr));

  thread->freq = AGS_THREAD_DEFAULT_JIFFIE;

  pthread_mutexattr_init(&(thread->mutexattr));
  pthread_mutexattr_settype(&(thread->mutexattr), PTHREAD_MUTEX_RECURSIVE);

  thread->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(thread->mutex, &(thread->mutexattr));

  thread->cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(thread->cond, NULL);

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
  
  thread->barrier = (pthread_barrier_t **) malloc(2 * sizeof(pthread_barrier_t *));
  thread->barrier[0] = (pthread_barrier_t *) malloc(sizeof(pthread_barrier_t));
  thread->barrier[1] = (pthread_barrier_t *) malloc(sizeof(pthread_barrier_t));
  
  thread->first_barrier = TRUE;
  thread->wait_count[0] = 1;
  thread->wait_count[1] = 1;

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
				   (AGS_THREAD_DEFAULT_JIFFIE + 1));

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

  g_atomic_pointer_set(&(thread->parent),
		       NULL);
  g_atomic_pointer_set(&(thread->children),
		       NULL);
  
  g_atomic_pointer_set(&(thread->next),
		       NULL);
  g_atomic_pointer_set(&(thread->prev),
		       NULL);

  thread->main_loop = NULL;
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
  
  if((AGS_THREAD_CONNECTED & (thread->flags)) != 0){
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
  AgsThread *thread;
  void *stackaddr;
  size_t stacksize;

  thread = AGS_THREAD(gobject);

  pthread_attr_getstack(&(thread->thread_attr),
			&stackaddr, &stacksize);

  pthread_attr_destroy(&(thread->thread_attr));

  pthread_mutexattr_destroy(&(thread->mutexattr));
  pthread_mutex_destroy(thread->mutex);
  pthread_cond_destroy(thread->cond);

  pthread_mutex_destroy(thread->start_mutex);
  pthread_cond_destroy(thread->start_cond);

  pthread_mutex_destroy(thread->timelock_mutex);
  pthread_cond_destroy(thread->timelock_cond);

  pthread_mutex_destroy(thread->greedy_mutex);
  pthread_cond_destroy(thread->greedy_cond);

  pthread_mutex_destroy(thread->suspend_mutex);
  free(thread->suspend_mutex);

  pthread_mutex_destroy(thread->timer_mutex);
  pthread_cond_destroy(thread->timer_cond);
  free(thread->timer_cond);
  
  /* call parent */
  G_OBJECT_CLASS(ags_thread_parent_class)->finalize(gobject);

  free(stackaddr);
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

AgsAccountingTable*
ags_accounting_table_alloc(AgsThread *thread)
{
  AgsAccountingTable *accounting_table;

  accounting_table = (AgsAccountingTable *) malloc(sizeof(AgsAccountingTable));

  accounting_table->thread = thread;

  return(accounting_table);
}

void
ags_accounting_table_set_sanity(GList *table,
				AgsThread *thread, gdouble sanity)
{
  if(table == NULL){
    return;
  }

  table = g_list_first(table);

  while(table != NULL){
    if(AGS_ACCOUNTING_TABLE(table->data)->thread == thread){
      break;
    }

    table = table->next;
  }

  if(table != NULL){
    AGS_ACCOUNTING_TABLE(table->data)->sanity == sanity;
  }
}

/**
 * ags_thread_set_sync:
 * @thread: an #AgsThread
 * @tic: the tic as sync occured.
 * 
 * Unsets AGS_THREAD_WAIT_0, AGS_THREAD_WAIT_1 or AGS_THREAD_WAIT_2.
 * Additionaly the thread is woken up by this function if waiting.
 *
 * Since: 0.4
 */
void
ags_thread_set_sync(AgsThread *thread, guint tic)
{
  guint flags;
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

  flags = g_atomic_int_get(&(thread->flags));

  switch(tic){
  case 0:
    {
      if((AGS_THREAD_WAIT_0 & flags) != 0){
	g_atomic_int_and(&(thread->flags),
			 (~AGS_THREAD_WAIT_0));
	waiting = TRUE;
      }
    }
    break;
  case 1:
    {
      if((AGS_THREAD_WAIT_1 & flags) != 0){
	g_atomic_int_and(&(thread->flags),
			 (~AGS_THREAD_WAIT_1));
	waiting = TRUE;
      }
    }
    break;
  case 2:
    {
      if((AGS_THREAD_WAIT_2 & flags) != 0){
	g_atomic_int_and(&(thread->flags),
			 (~AGS_THREAD_WAIT_2));
	waiting = TRUE;
      }
    }
    break;
  }

  if(waiting){
    gboolean success;
    
    success = pthread_mutex_trylock(thread->mutex);

    if(!success){
      if(broadcast){
	pthread_cond_broadcast(thread->cond);
      }else{
	pthread_cond_signal(thread->cond);
      }
    }

    if(!success){
      pthread_mutex_unlock(thread->mutex);
    }
  }
}

/**
 * ags_thread_set_sync:
 * @thread: an #AgsThread
 * @tic: the tic as sync occured.
 * 
 * Calls ags_thread_set_sync() on all threads.
 *
 * Since: 0.4
 */
void
ags_thread_set_sync_all(AgsThread *thread, guint tic)
{
  AgsThread *main_loop;

  auto void ags_thread_set_sync_all_recursive(AgsThread *thread, guint tic);

  void ags_thread_set_sync_all_recursive(AgsThread *thread, guint tic){
    AgsThread *child;

    ags_thread_set_sync(thread, tic);

    child = g_atomic_pointer_get(&(thread->children));

    while(child != NULL){
      ags_thread_set_sync_all_recursive(child, tic);
      
      child = g_atomic_pointer_get(&(child->next));
    }
  }

  main_loop = ags_thread_get_toplevel(thread);

  ags_thread_set_sync_all_recursive(main_loop, tic);
}

/**
 * ags_thread_lock:
 * @thread: an #AgsThread
 * 
 * Locks the threads own mutex and sets the appropriate flag.
 *
 * Since: 0.4
 */
void
ags_thread_lock(AgsThread *thread)
{
  AgsThread *main_loop;

  if(thread == NULL){
    return;
  }
  
  main_loop = ags_thread_get_toplevel(thread);

  if(AGS_IS_MAIN_LOOP(main_loop)){
    pthread_mutex_lock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
  }
  
  if(main_loop == thread){
    pthread_mutex_lock(thread->mutex);
    g_atomic_int_or(&(thread->flags),
		     (AGS_THREAD_LOCKED));
  }else{
    pthread_mutex_lock(thread->mutex);
    g_atomic_int_or(&(thread->flags),
		     (AGS_THREAD_LOCKED));
  }

  if(AGS_IS_MAIN_LOOP(main_loop)){
    pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
  }
}

/**
 * ags_thread_trylock:
 * @thread: an #AgsThread
 * 
 * Locks the threads own mutex if available and sets the
 * appropriate flag and returning %TRUE. Otherwise return %FALSE
 * without lock.
 *
 * Since: 0.4
 */
gboolean
ags_thread_trylock(AgsThread *thread)
{
  AgsThread *main_loop;
  guint val;

  if(thread == NULL){
    return(FALSE);
  }
    
  main_loop = ags_thread_get_toplevel(thread);

  if(AGS_IS_MAIN_LOOP(main_loop)){
    pthread_mutex_lock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
  }
  
  if(main_loop == thread){
    if(pthread_mutex_trylock(thread->mutex) != 0){
      if(AGS_IS_MAIN_LOOP(main_loop)){
	pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
      }
      
      return(FALSE);
    }

    g_atomic_int_or(&(thread->flags),
		     (AGS_THREAD_LOCKED));
  }else{

    if(pthread_mutex_trylock(thread->mutex) != 0){
      if(AGS_IS_MAIN_LOOP(main_loop)){
	pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
      }
      
      return(FALSE);
    }

    g_atomic_int_or(&(thread->flags),
		     (AGS_THREAD_LOCKED));
  }

  if(AGS_IS_MAIN_LOOP(main_loop)){
    pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
  }
  
  return(TRUE);
}

/**
 * ags_thread_unlock:
 * @thread: an #AgsThread
 *
 * Unlocks the threads own mutex and unsets the appropriate flag.
 *
 * Since: 0.4
 */
void
ags_thread_unlock(AgsThread *thread)
{
  if(thread == NULL){
    return;
  }

  g_atomic_int_and(&(thread->flags),
		   (~AGS_THREAD_LOCKED));

  pthread_mutex_unlock(thread->mutex);
}

/**
 * ags_thread_get_toplevel:
 * @thread: an #AgsThread
 *
 * Retrieve toplevel thread.
 *
 * Returns: the toplevevel #AgsThread
 *
 * Since: 0.4
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
 * Since: 0.4
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
 * Since: 0.4
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
 * Since: 0.4
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

  //  pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
}

/**
 * ags_thread_add_child:
 * @thread: an #AgsThread
 * @child: the child to remove
 * 
 * Add child to thread.
 *
 * Since: 0.4
 */
void
ags_thread_add_child(AgsThread *thread, AgsThread *child)
{
  ags_thread_add_child_extended(thread, child,
				FALSE, TRUE);
}

/**
 * ags_thread_add_child:
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
  AgsThread *toplevel;

  if(thread == NULL || child == NULL){
    return;
  }

  toplevel = ags_thread_get_toplevel(thread);

  if(g_atomic_pointer_get(&(child->parent)) != NULL){
    ags_thread_remove_child(child->parent, child);
  }

  /*  */
  if(AGS_IS_MAIN_LOOP(toplevel)){
    pthread_mutex_lock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(toplevel)));
  }

  if(g_atomic_pointer_get(&(thread->children)) == NULL){
    g_atomic_pointer_set(&(thread->children),
			 child);
    g_atomic_pointer_set(&(child->parent),
			 thread);
  }else{
    AgsThread *sibling;

    sibling = ags_thread_last(g_atomic_pointer_get(&(thread->children)));

    g_atomic_pointer_set(&(sibling->next),
			 child);
    g_atomic_pointer_set(&(child->prev),
			 sibling);
    g_atomic_pointer_set(&(child->parent),
			 thread);
  }
    
  if(AGS_IS_MAIN_LOOP(toplevel)){
    pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(toplevel)));
  }
  
  if(!no_start){
    if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(thread->flags)))) != 0){
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
 * Since: 0.4
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
 * Since: 0.4
 */
gboolean
ags_thread_sibling_is_locked(AgsThread *thread)
{
  if(thread == NULL){
    return(FALSE);
  }

  thread = ags_thread_first(thread);

  while(g_atomic_pointer_get(&(thread->next)) != NULL){
    if((AGS_THREAD_LOCKED & (g_atomic_int_get(&(thread->flags)))) != 0){
      return(TRUE);
    }

    thread = g_atomic_pointer_get(&(thread->next));
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
 * Since: 0.4
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
  guint flags;
  gboolean retval;

  toplevel = ags_thread_get_toplevel(current);

  //  pthread_mutex_lock(&(current->mutex));

  flags = g_atomic_int_get(&(current->flags));
  retval = FALSE;

  if((AGS_THREAD_RUNNING & flags) == 0){
    retval = TRUE;
  }

  if((AGS_THREAD_INITIAL_RUN & flags) != 0){
    retval = TRUE;
  }

  if((AGS_THREAD_READY & flags) != 0){
    retval = TRUE;
  }

  if(retval){
    //    pthread_mutex_unlock(&(current->mutex));

    return(TRUE);
  }

  if(tic > 2){
    tic = tic % 3;
  }

  switch(tic){
  case 0:
    {
      if((AGS_THREAD_WAIT_0 & flags) == 0){
	retval = TRUE;
      }
    }
    break;
  case 1:
    {
      if((AGS_THREAD_WAIT_1 & flags) == 0){
	retval = TRUE;
      }
    }
    break;
  case 2:
    {
      if((AGS_THREAD_WAIT_2 & flags) == 0){
	retval = TRUE;
      }
    }
    break;
  }

  //  pthread_mutex_unlock(&(current->mutex));

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
    guint flags;
    gboolean retval;

    toplevel = ags_thread_get_toplevel(current);

    //  pthread_mutex_lock(&(current->mutex));

    flags = g_atomic_int_get(&(current->flags));
    retval = FALSE;

    if((AGS_THREAD_RUNNING & flags) == 0){
      retval = TRUE;
    }

    if((AGS_THREAD_INITIAL_RUN & flags) != 0){
      retval = TRUE;
    }

    if((AGS_THREAD_READY & flags) != 0){
      retval = TRUE;
    }

    if(retval){
      //    pthread_mutex_unlock(&(current->mutex));

      return(TRUE);
    }

    if(tic > 2){
      tic = tic % 3;
    }

    switch(tic){
    case 0:
      {
	if((AGS_THREAD_WAIT_0 & flags) != 0){
	  retval = TRUE;
	}
      }
      break;
    case 1:
      {
	if((AGS_THREAD_WAIT_1 & flags) != 0){
	  retval = TRUE;
	}
      }
      break;
    case 2:
      {
	if((AGS_THREAD_WAIT_2 & flags) != 0){
	  retval = TRUE;
	}
      }
      break;
    }

    //  pthread_mutex_unlock(&(current->mutex));
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
 * Since: 0.4
 */
AgsThread*
ags_thread_next_parent_locked(AgsThread *thread, AgsThread *parent)
{
  AgsThread *current;

  current = g_atomic_pointer_get(&(thread->parent));

  while(current != parent){
    if((AGS_THREAD_WAITING_FOR_CHILDREN & (g_atomic_int_get(&(current->flags)))) != 0){
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
 * Since: 0.4
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

    if((AGS_THREAD_WAITING_FOR_SIBLING & (g_atomic_int_get(&(thread->flags)))) != 0){
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
 * Since: 0.4
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

      if((AGS_THREAD_WAITING_FOR_PARENT & (g_atomic_int_get(&(current->flags)))) != 0){
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
 * Since: 0.4
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
    g_atomic_int_or(&(current->flags),
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
 * Since: 0.4
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
    g_atomic_int_or(&(current->flags),
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
 * Since: 0.4
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
      g_atomic_int_or(&(current->flags),
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
 * Since: 0.4
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
    g_atomic_int_and(&(current->flags),
		     (~AGS_THREAD_WAITING_FOR_CHILDREN));

    if((AGS_THREAD_BROADCAST_PARENT & (g_atomic_int_get(&(thread->flags)))) == 0){
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
 * Since: 0.4
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

    g_atomic_int_and(&(current->flags),
		     (~AGS_THREAD_WAITING_FOR_SIBLING));

    if((AGS_THREAD_BROADCAST_SIBLING & (g_atomic_int_get(&(thread->flags)))) == 0){
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
 * Since: 0.4
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

      g_atomic_int_and(&(current->flags),
		       (~AGS_THREAD_WAITING_FOR_PARENT));

      if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) == 0 &&
	 !AGS_IS_MAIN_LOOP(thread)){

	if((AGS_THREAD_BROADCAST_CHILDREN & (g_atomic_int_get(&(thread->flags)))) == 0){
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
 * Since: 0.4
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
	  (AGS_THREAD_WAITING_FOR_CHILDREN & (g_atomic_int_get(&(current->flags)))) == 0) ||
	 g_atomic_pointer_get(&(current->parent)) != parent)){
    pthread_cond_wait(current->cond,
		      current->mutex);
    
    if(!((AGS_THREAD_IDLE & (g_atomic_int_get(&(current->flags)))) != 0 ||
	 (AGS_THREAD_WAITING_FOR_CHILDREN & (g_atomic_int_get(&(current->flags)))) == 0)){
      current = g_atomic_pointer_get(&(current->parent));
    }
  }

  /* unset flag */
  g_atomic_int_and(&(thread->flags),
		   (~AGS_THREAD_WAITING_FOR_PARENT));
}

/**
 * ags_thread_wait_sibling:
 * @thread: an #AgsThread
 *
 * Wait on sibling tree structure.
 *
 * Since: 0.4
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
	  (AGS_THREAD_WAITING_FOR_SIBLING & (g_atomic_int_get(&(current->flags)))) == 0) ||
	 g_atomic_pointer_get(&(current->next)) != NULL)){
    if(current == thread){
      current = g_atomic_pointer_get(&(current->next));
      
      continue;
    }
    
    pthread_cond_wait(current->cond,
		      current->mutex);
    
    if(!((AGS_THREAD_IDLE & (g_atomic_int_get(&(current->flags)))) != 0 ||
	 (AGS_THREAD_WAITING_FOR_SIBLING & (g_atomic_int_get(&(current->flags)))) == 0)){
      current = g_atomic_pointer_get(&(current->next));
    }
  }

  /* unset flags */
  g_atomic_int_and(&(thread->flags),
		   (~AGS_THREAD_WAITING_FOR_SIBLING));
}

/**
 * ags_thread_wait_children:
 * @thread: an #AgsThread
 *
 * Wait on child tree structure.
 *
 * Since: 0.4
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
	    (AGS_THREAD_WAITING_FOR_PARENT & (g_atomic_int_get(&(child->flags)))) == 0) ||
	   g_atomic_pointer_get(&(child->next)) != NULL)){
      if(initial_run){
	ags_thread_wait_children_recursive(g_atomic_pointer_get(&(child->children)));

	initial_run = FALSE;
      }

      pthread_cond_wait(child->cond,
			child->mutex);
     
      if(!((AGS_THREAD_IDLE & (g_atomic_int_get(&(child->flags)))) != 0 ||
	   (AGS_THREAD_WAITING_FOR_PARENT & (g_atomic_int_get(&(child->flags)))) == 0)){
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
  g_atomic_int_and(&(thread->flags),
		   (~AGS_THREAD_WAITING_FOR_CHILDREN));
}

/**
 * ags_thread_signal_parent:
 * @thread: an #AgsThread
 * @broadcast: whether to perforam a signal or to broadcast
 *
 * Signals the tree in higher levels.
 *
 * Since: 0.4
 */
void
ags_thread_signal_parent(AgsThread *thread, AgsThread *parent,
			 gboolean broadcast)
{
  AgsThread *current;

  current = g_atomic_pointer_get(&(thread->parent));

  while(current != NULL){
    if((AGS_THREAD_WAIT_FOR_CHILDREN & (g_atomic_int_get(&(current->flags)))) != 0){
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
 * Since: 0.4
 */
void
ags_thread_signal_sibling(AgsThread *thread, gboolean broadcast)
{
  AgsThread *current;

  current = ags_thread_first(thread);

  while(current != NULL){
    if((AGS_THREAD_WAIT_FOR_SIBLING & (g_atomic_int_get(&(current->flags)))) != 0){
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
 * Since: 0.4
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
      if((AGS_THREAD_WAIT_FOR_PARENT & (g_atomic_int_get(&(current->flags)))) != 0){
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

void
ags_thread_real_start(AgsThread *thread)
{
  AgsMainLoop *main_loop;
  guint val;

  if(thread == NULL){
    return;
  }

  main_loop = AGS_MAIN_LOOP(ags_thread_get_toplevel(thread));

#ifdef AGS_DEBUG
  g_message("thread start: %s\0", G_OBJECT_TYPE_NAME(thread));
#endif

  /* */
  val = g_atomic_int_get(&(thread->flags));
  
  if((AGS_THREAD_TIMELOCK_RUN & val) != 0){
    pthread_create(thread->timelock_thread, NULL,
    		   ags_thread_timelock_loop, thread);
  }

  /*  */
  pthread_create(thread->thread, &(thread->thread_attr),
		 &ags_thread_loop, thread);
}

/**
 * ags_thread_start:
 * @thread: the #AgsThread instance
 *
 * Start the thread.
 *
 * Since: 0.4
 */
void
ags_thread_start(AgsThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[START], 0);
  g_object_unref(G_OBJECT(thread));
}

void*
ags_thread_timer(void *ptr)
{
  
}

void*
ags_thread_loop(void *ptr)
{
  AgsThread *thread, *main_loop;
  GObject *async_queue;

  pthread_mutex_t *run_mutex;
  pthread_cond_t *run_cond;

  gboolean is_in_sync;
  gboolean wait_for_parent, wait_for_sibling, wait_for_children;
  guint current_tic, next_tic;
  guint tic;
  guint val, running, locked_greedy;
  guint counter, delay;
  guint i, i_stop;

  struct timespec time_prev, time_now;
  pthread_mutex_t *run_mutex;
  pthread_cond_t *run_cond;

  auto void ags_thread_loop_sync(AgsThread *thread);
  auto void ags_thread_loop_wait_starting();
  auto void ags_thread_loop_wait_async();
  
  void ags_thread_loop_sync(AgsThread *thread){    
    /* sync */
    if(current_tic = 2){
      next_tic = 0;
    }else if(current_tic = 0){
      next_tic = 1;
    }else if(current_tic = 1){
      next_tic = 2;
    }

    if(next_tic = 2){
      tic = 0;
    }else if(next_tic = 0){
      tic = 1;
    }else if(next_tic = 1){
      tic = 2;
    }

    pthread_mutex_lock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));

    switch(current_tic){
    case 0:
      {
	g_atomic_int_or(&(thread->flags),
			AGS_THREAD_WAIT_0);
      }
      break;
    case 1:
      {
	g_atomic_int_or(&(thread->flags),
			AGS_THREAD_WAIT_1);
      }
      break;
    case 2:
      {
	g_atomic_int_or(&(thread->flags),
			AGS_THREAD_WAIT_2);
      }
      break;
    }

    if(!ags_thread_is_tree_ready(thread,
				 current_tic)){
      //      ags_thread_hangcheck(main_loop);
    
      while(!ags_thread_is_current_ready(thread,
					 current_tic)){
	pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
	pthread_cond_wait(thread->cond,
			  thread->mutex);
	pthread_mutex_lock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
      }

      pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));

      ags_main_loop_set_last_sync(AGS_MAIN_LOOP(main_loop), current_tic);
      ags_main_loop_set_tic(AGS_MAIN_LOOP(main_loop), next_tic);
    }else{
      /* async-queue */
      ags_async_queue_set_run(AGS_ASYNC_QUEUE(async_queue),
			      FALSE);

      /* thread tree */
      ags_thread_set_sync_all(main_loop, current_tic);
      pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
      
      ags_main_loop_set_last_sync(AGS_MAIN_LOOP(main_loop), current_tic);
      ags_main_loop_set_tic(AGS_MAIN_LOOP(main_loop), next_tic);
   }

    current_tic = next_tic;
  }

  void ags_thread_loop_wait_starting(){
    AgsThread *child;
    GList *list;

    list = g_atomic_pointer_get(&(thread->start_queue));
    g_atomic_pointer_set(&(thread->start_queue),
			 NULL);
    
    while(list != NULL){
      child = (AgsThread *) list->data;
      
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

      list = list->next;
    }
  }

  void ags_thread_loop_wait_async(){
    if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(AGS_THREAD(thread)->flags)))) != 0){
      return;
    }
    
    /* async-queue */
    if(!AGS_IS_ASYNC_QUEUE(thread)){
      if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(AGS_THREAD(async_queue)->flags)))) != 0){
	pthread_mutex_lock(run_mutex);

	//g_message("blocked\0");

	if(!ags_async_queue_is_run(AGS_ASYNC_QUEUE(async_queue))){
	  //  g_message("wait\0");

	  while(!ags_async_queue_is_run(AGS_ASYNC_QUEUE(async_queue))){
	    pthread_cond_wait(run_cond,
			      run_mutex);
	  }
	}

	pthread_mutex_unlock(run_mutex);
      }
    }else{
      //      g_message("not blocked\0");
    }
  }
  
  //  ags_thread_self =
  thread = (AgsThread *) ptr;
  pthread_once(&ags_thread_key_once, ags_thread_self_create);
  pthread_setspecific(&ags_thread_key, thread);
  
  main_loop = ags_thread_get_toplevel(thread);

  /* async-queue */
  async_queue = ags_main_loop_get_async_queue(main_loop);

  run_mutex = ags_async_queue_get_run_mutex(AGS_ASYNC_QUEUE(async_queue));
  run_cond = ags_async_queue_get_run_cond(AGS_ASYNC_QUEUE(async_queue));

  /*  */
  current_tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));

  g_atomic_int_or(&(thread->flags),
		  (AGS_THREAD_RUNNING |
		   AGS_THREAD_INITIAL_RUN));
  
  running = g_atomic_int_get(&(thread->flags));

  counter = 0;
  
  /*  */
#ifndef AGS_USE_TIMER
  if(g_atomic_pointer_get(&(thread->parent)) == NULL){
    clock_gettime(CLOCK_MONOTONIC, &time_prev);
  }
#endif
  
  while((AGS_THREAD_RUNNING & running) != 0){
    /* wait for starting threads */
    ags_thread_loop_wait_starting();
    ags_thread_loop_wait_async();
    
    /*  */
    if(thread->freq >= 1.0){
      delay = AGS_THREAD_MAX_PRECISION / thread->freq / (1000.0 / AGS_THREAD_MAX_PRECISION);

      i_stop = 1;
    }else{
      delay = AGS_THREAD_MAX_PRECISION / thread->freq / (1000.0 / AGS_THREAD_MAX_PRECISION);
      
      i_stop = 1;
    }

    /* if still running */    
    running = g_atomic_int_get(&(thread->flags));

    /* idle */
    if(g_atomic_pointer_get(&(thread->parent)) == NULL){
#ifdef AGS_USE_TIMER
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
#else
      long time_spent;

      static const long time_unit = NSEC_PER_SEC / AGS_THREAD_MAX_PRECISION;

      clock_gettime(CLOCK_MONOTONIC, &time_now);

      if(time_now.tv_sec > time_prev.tv_sec){
	time_spent = (time_now.tv_nsec) + (NSEC_PER_SEC - time_prev.tv_nsec);
      }else{
	time_spent = time_now.tv_nsec - time_prev.tv_nsec;
      }

      if(time_spent < time_unit){
	struct timespec timed_sleep = {
	  0,
	  0,
	};

	if(time_spent < time_unit){
	  timed_sleep.tv_nsec = time_unit - time_spent;

	  nanosleep(&timed_sleep, NULL);
	}
      }

      clock_gettime(CLOCK_MONOTONIC, &time_prev);
#endif
    }

    /* iteration */
    if(delay >= 1.0){
      counter++;

      if(counter < delay){
	counter++;

	if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0){
	  /* unset initial run */
	  /* signal AgsAudioLoop */
	  pthread_mutex_lock(thread->start_mutex);

	  g_atomic_int_and(&(thread->flags),
			   (~AGS_THREAD_INITIAL_RUN));
	  
	  g_atomic_int_set(&(thread->start_done),
			   TRUE);

	  if(g_atomic_int_get(&(thread->start_wait)) == TRUE){
	    pthread_cond_broadcast(thread->start_cond);
	  }
	  
	  pthread_mutex_unlock(thread->start_mutex);
	}else{
	  /* run in hierarchy */
	  pthread_mutex_lock(thread->mutex);

	  ags_thread_loop_sync(thread);

	  pthread_mutex_unlock(thread->mutex);

	  /* wait for async-queue */
	  ags_thread_loop_wait_async();
	}

	//	pthread_yield();

	continue;
      }else{
	counter = 0;
      }
    }else{
      if(counter < delay){
	counter++;

	if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0){
	  /* unset initial run */
	  g_atomic_int_and(&(thread->flags),
			   (~AGS_THREAD_INITIAL_RUN));
	  g_atomic_int_and(&(thread->flags),
			   (~AGS_THREAD_WAIT_0));

	  /* signal AgsAudioLoop */
	  pthread_mutex_lock(thread->start_mutex);

	  g_atomic_int_and(&(thread->flags),
			   (~AGS_THREAD_INITIAL_RUN));
	  
	  g_atomic_int_set(&(thread->start_done),
			   TRUE);

	  if(g_atomic_int_get(&(thread->start_wait)) == TRUE){
	    pthread_cond_broadcast(thread->start_cond);
	  }
	  
	  pthread_mutex_unlock(thread->start_mutex);
	}else{
	  /* run in hierarchy */
	  pthread_mutex_lock(thread->mutex);

	  ags_thread_loop_sync(thread);

	  pthread_mutex_unlock(thread->mutex);

	  /* wait for async-queue */
	  ags_thread_loop_wait_async();
	}

	//	pthread_yield();

	continue;
      }else{
	counter = 0;
      }
    }

    for(i = 0; i < i_stop && (AGS_THREAD_RUNNING & running) != 0; i++){
      running = g_atomic_int_get(&(thread->flags));

      /* barrier */
      if((AGS_THREAD_WAITING_FOR_BARRIER & (g_atomic_int_get(&(thread->flags)))) != 0){
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

      if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) == 0){
	/* run in hierarchy */
	pthread_mutex_lock(thread->mutex);
	
	ags_thread_loop_sync(thread);
	
	pthread_mutex_unlock(thread->mutex);

	/* wait for async-queue */
	ags_thread_loop_wait_async();
      }

      /* */
      switch(current_tic){
      case 2:
	{
	  current_tic = 0;
	  break;
	}
      case 1:
	{
	  current_tic = 2;
	  break;
	}
      case 0:
	{
	  current_tic = 1;
	  break;
	}
      }

      /* set idle flag */
      g_atomic_int_or(&(thread->flags),
		      AGS_THREAD_IDLE);

      if((AGS_THREAD_WAIT_FOR_PARENT & (g_atomic_int_get(&(thread->flags)))) != 0){
	wait_for_parent = TRUE;

	g_atomic_int_or(&(thread->flags),
			AGS_THREAD_WAITING_FOR_PARENT);
	ags_thread_lock_parent(thread, NULL);
      }else{
	wait_for_parent = FALSE;
      }

      /* lock sibling */
      if((AGS_THREAD_WAIT_FOR_SIBLING & (g_atomic_int_get(&(thread->flags)))) != 0){
	wait_for_sibling = TRUE;

	g_atomic_int_or(&(thread->flags),
			AGS_THREAD_WAITING_FOR_SIBLING);
	ags_thread_lock_sibling(thread);
      }else{
	wait_for_sibling = FALSE;
      }

      /* lock_children */
      if((AGS_THREAD_WAIT_FOR_CHILDREN & (g_atomic_int_get(&(thread->flags)))) != 0){
	wait_for_children = TRUE;

	g_atomic_int_or(&(thread->flags),
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


      /* */
      pthread_mutex_lock(thread->timelock_mutex);

      val = g_atomic_int_get(&(thread->flags));

      if((AGS_THREAD_TIMELOCK_RUN & val) != 0){
	guint locked;

	locked = g_atomic_int_get(&(thread->flags));
				
	g_atomic_int_and(&(thread->flags),
			 (~AGS_THREAD_TIMELOCK_WAIT));

	if((AGS_THREAD_TIMELOCK_WAIT & locked) != 0){	
	  pthread_cond_signal(thread->timelock_cond);
	}
      }

      pthread_mutex_unlock(thread->timelock_mutex);

      /* run */
      ags_thread_run(thread);
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

      if(thread->freq >= 1.0){
	/* unset initial run */
	if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0){
	  g_atomic_int_and(&(thread->flags),
			   (~AGS_THREAD_INITIAL_RUN));
	  g_atomic_int_and(&(thread->flags),
			   (~AGS_THREAD_WAIT_0));

	  /* signal AgsAudioLoop */
	  pthread_mutex_lock(thread->start_mutex);

	  g_atomic_int_and(&(thread->flags),
			   (~AGS_THREAD_INITIAL_RUN));
	  
	  g_atomic_int_set(&(thread->start_done),
			   TRUE);

	  if(g_atomic_int_get(&(thread->start_wait)) == TRUE){
	    pthread_cond_broadcast(thread->start_cond);
	  }
	  
	  pthread_mutex_unlock(thread->start_mutex);
	}
      }
    }

    if(AGS_IS_ASYNC_QUEUE(thread)){
      /* async queue */
      pthread_mutex_lock(run_mutex);
	
      ags_async_queue_set_run(AGS_ASYNC_QUEUE(async_queue),
			      TRUE);
	
      pthread_cond_broadcast(run_cond);
      
      pthread_mutex_unlock(run_mutex);
    }

    pthread_yield();
  }

  if(current_tic = 2){
    next_tic = 0;
  }else if(current_tic = 0){
    next_tic = 1;
  }else if(current_tic = 1){
    next_tic = 2;
  }

  if(next_tic = 2){
    tic = 0;
  }else if(next_tic = 0){
    tic = 1;
  }else if(next_tic = 1){
    tic = 2;
  }

  pthread_mutex_lock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));

  if((AGS_THREAD_UNREF_ON_EXIT & (g_atomic_int_get(&(thread->flags)))) != 0){
    ags_thread_remove_child(g_atomic_pointer_get(&(thread->parent)),
    			    thread);
  }

  if(ags_thread_is_tree_ready(main_loop,
			      current_tic) &&
     current_tic == ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop))){

    ags_thread_set_sync_all(main_loop, current_tic);

    pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));

    ags_main_loop_set_last_sync(AGS_MAIN_LOOP(main_loop), current_tic);
    ags_main_loop_set_tic(AGS_MAIN_LOOP(main_loop), next_tic);
  }else{
    pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
  }

#ifdef AGS_DEBUG
  g_message("thread finished\0");
#endif  

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
 * Since: 0.4
 */
void
ags_thread_run(AgsThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[RUN], 0);
  g_object_unref(G_OBJECT(thread));
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
  guint val;

  thread = AGS_THREAD(ptr);

  val = g_atomic_int_get(&(thread->flags));
  
  pthread_mutex_lock(thread->timelock_mutex);

  g_atomic_int_or(&(thread->flags),
		  AGS_THREAD_TIMELOCK_WAIT);

  while((AGS_THREAD_RUNNING & (val)) != 0){

    g_atomic_int_or(&(thread->flags),
		    AGS_THREAD_TIMELOCK_WAIT);

    val = g_atomic_int_get(&(thread->flags));

    while((AGS_THREAD_TIMELOCK_WAIT & (val)) != 0){
      retval = pthread_cond_wait(thread->timelock_cond,
				 thread->timelock_mutex);

      val = g_atomic_int_get(&(thread->flags));
    }

    nanosleep(&(thread->timelock), NULL);
    // ndelay(thread->timelock.tv_nsec);

    val = g_atomic_int_get(&(thread->flags));

    if((AGS_THREAD_WAIT_0 & val) != 0){
#ifdef AGS_DEBUG
      g_message("thread in realtime\0");
#endif
    }else{
#ifdef AGS_DEBUG
      g_message("thread timelock\0");
#endif
      ags_thread_timelock(thread);
    }

    val = g_atomic_int_get(&(thread->flags));
  }

  pthread_mutex_unlock(thread->timelock_mutex);
}

void
ags_thread_real_timelock(AgsThread *thread)
{
  AgsThread *main_loop;
  GList *greedy_locks;
  guint locked_greedy, val;
  guint tic, next_tic;
      
  pthread_mutex_lock(thread->greedy_mutex);

  tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));

  if(tic = 2){
    next_tic = 0;
  }else if(tic = 0){
    next_tic = 1;
  }else if(tic = 1){
    next_tic = 2;
  }

#if defined(AGS_PTHREAD_SUSPEND) || defined(AGS_LINUX_SIGNALS)
  val = g_atomic_int_get(&(thread->flags));

  if((AGS_THREAD_SKIP_NON_GREEDY & val) != 0){
#endif
    /*
     * bad choice because throughput will suffer but it's your only choice as long
     * pthread_suspend and pthread_resume will be missing.
     */

    ags_thread_lock(main_loop);
    greedy_locks = thread->greedy_locks;

    while(greedy_locks != NULL){
      pthread_mutex_lock(AGS_THREAD(greedy_locks->data)->greedy_mutex);
      
      g_atomic_int_or(&(AGS_THREAD(greedy_locks->data)->flags),
		      (AGS_THREAD_WAIT_0 | AGS_THREAD_SKIPPED_BY_TIMELOCK));

      pthread_mutex_unlock(AGS_THREAD(greedy_locks->data)->greedy_mutex);

      greedy_locks = greedy_locks->next;
    }

    ags_thread_unlock(main_loop);

    if(!ags_thread_is_tree_ready(thread,
				 next_tic)){
      g_atomic_int_or(&(thread->flags),
		      AGS_THREAD_WAIT_0);

      while(!ags_thread_is_current_ready(thread,
					 next_tic)){
	pthread_cond_wait(thread->cond,
			  thread->greedy_mutex);
      }
    }else{
      ags_main_loop_set_last_sync(AGS_MAIN_LOOP(main_loop), tic);
      ags_main_loop_set_tic(AGS_MAIN_LOOP(main_loop), next_tic);
      ags_thread_set_sync_all(main_loop, tic);
    }
#if defined(AGS_PTHREAD_SUSPEND) || defined(AGS_LINUX_SIGNALS)
  }else{
    g_atomic_int_or(&(thread->flags),
		    AGS_THREAD_TIMELOCK_RESUME);
    
    /* throughput is mandatory */
#ifdef AGS_PTHREAD_SUSPEND
    pthread_suspend(thread->thread);
#else
    pthread_kill(thread_id, AGS_THREAD_SUSPEND_SIG);
#endif

    /* allow non greedy to continue */
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

    /* skip syncing for greedy */
    main_loop = ags_thread_get_toplevel(thread);

    if(!ags_thread_is_tree_ready(thread,
				 next_tic)){
      g_atomic_int_or(&(thread->flags),
		      AGS_THREAD_WAIT_0);

      while(!ags_thread_is_current_ready(thread,
					 next_tic)){
	pthread_cond_wait(thread->cond,
			  thread->greedy_mutex);
      }
    }else{
      ags_main_loop_set_last_sync(AGS_MAIN_LOOP(main_loop), tic);
      ags_main_loop_set_tic(AGS_MAIN_LOOP(main_loop), next_tic);
      ags_thread_set_sync_all(main_loop, tic);
    }

    /* your chance */
#ifdef AGS_PTHREAD_SUSPEND
    pthread_resume(thread->thread));
#else
    pthread_kill(thread_id, AGS_THREAD_RESUME_SIG);
#endif
  }
#endif
  pthread_mutex_unlock(thread->greedy_mutex);
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
}

/**
 * ags_thread_stop:
 * @thread: the #AgsThread instance
 * 
 * Stop the threads loop by unsetting AGS_THREAD_RUNNING flag.
 *
 * Since: 0.4
 */
void
ags_thread_stop(AgsThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[STOP], 0);
  g_object_unref(G_OBJECT(thread));
}

/**
 * ags_thread_hangcheck:
 * @thread: the #AgsThread instance
 *
 * Performs hangcheck of thread.
 *
 * Since: 0.4
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
    guint flags;

    flags = g_atomic_int_get(&(thread->flags));

    if((AGS_THREAD_WAIT_0 & flags) != 0){
      synced[0] = TRUE;
    }

    if((AGS_THREAD_WAIT_1 & flags) != 0){
      synced[1] = TRUE;
    }

    if((AGS_THREAD_WAIT_2 & flags) != 0){
      synced[2] = TRUE;
    }

    /* iterate tree */
    child = thread->children;

    while(child != NULL){
      ags_thread_hangcheck_recursive(child);

      child = g_atomic_pointer_get(&(child->next));
    }
  }
  void ags_thread_hangcheck_unsync_all(AgsThread *thread, gboolean broadcast){
    AgsThread *child;
    guint flags;

    flags = g_atomic_int_get(&(thread->flags));
    g_atomic_int_and(&(thread->flags),
		     (~(AGS_THREAD_WAIT_0 |
			AGS_THREAD_WAIT_1 |
			AGS_THREAD_WAIT_2)));

    if(AGS_THREAD_WAIT_0 & flags){
      if(broadcast){
	pthread_cond_broadcast(thread->cond);
      }else{
	pthread_cond_signal(thread->cond);
      }
    }

    if(AGS_THREAD_WAIT_1 & flags){
      if(broadcast){
	pthread_cond_broadcast(thread->cond);
      }else{
	pthread_cond_signal(thread->cond);
      }
    }

    if(AGS_THREAD_WAIT_2 & flags){
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
 * ags_thread_new:
 * @data: an #GObject
 *
 * Create a new #AgsThread you may provide a #gpointer as @data
 * to your thread routine.
 *
 * Since: 0.4
 */
AgsThread*
ags_thread_new(gpointer data)
{
  AgsThread *thread;

  thread = (AgsThread *) g_object_new(AGS_TYPE_THREAD,
				      NULL);

  thread->data = data;

  return(thread);
}
