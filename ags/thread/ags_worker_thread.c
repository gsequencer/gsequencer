/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/thread/ags_worker_thread.h>

#include <ags/object/ags_connectable.h>

void ags_worker_thread_class_init(AgsWorkerThreadClass *worker_thread);
void ags_worker_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_worker_thread_init(AgsWorkerThread *worker_thread);
void ags_worker_thread_connect(AgsConnectable *connectable);
void ags_worker_thread_disconnect(AgsConnectable *connectable);
void ags_worker_thread_finalize(GObject *gobject);

void ags_worker_thread_start(AgsThread *thread);
void ags_worker_thread_run(AgsThread *thread);
void ags_worker_thread_stop(AgsThread *thread);

void* ags_woker_thread_do_poll_loop(void *ptr);

/**
 * SECTION:ags_worker_thread
 * @short_description: worker thread
 * @title: AgsWorkerThread
 * @section_id:
 * @include: ags/thread/ags_worker_thread.h
 *
 * The #AgsWorkerThread does non-realtime work. You might want
 * to synchronize to the run signal within your ::do_poll() method.
 */

enum{
  DO_POLL,
  LAST_SIGNAL,
};

static gpointer ags_worker_thread_parent_class = NULL;
static guint worker_thread_signals[LAST_SIGNAL];

GType
ags_worker_thread_get_type()
{
  static GType ags_type_worker_thread = 0;

  if(!ags_type_worker_thread){
    static const GTypeInfo ags_worker_thread_info = {
      sizeof (AgsWorkerThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_worker_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsWorkerThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_worker_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_worker_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_worker_thread = g_type_register_static(AGS_TYPE_THREAD,
						    "AgsWorkerThread",
						    &ags_worker_thread_info,
						    0);
    
    g_type_add_interface_static(ags_type_worker_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_worker_thread);
}

void
ags_worker_thread_class_init(AgsWorkerThreadClass *worker_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;

  ags_worker_thread_parent_class = g_type_class_peek_parent(worker_thread);

  /* GObject */
  gobject = (GObjectClass *) worker_thread;

  gobject->finalize = ags_worker_thread_finalize;

  /* AgsThread */
  thread = (AgsThreadClass *) worker_thread;

  thread->start = ags_worker_thread_start;
  thread->run = ags_worker_thread_run;
  thread->stop = ags_worker_thread_stop;

  /* AgsWorkerThread */
  worker_thread->do_poll = NULL;
  
  /* signals */
  /**
   * AgsWorkerThread::do-poll:
   * @thread: the #AgsWorkerThread
   *
   * The ::do_poll() signal runs independently of ::run() but
   * might be synchronized using a conditional lock.
   * 
   * Since: 1.0.0.8
   */
  worker_thread_signals[DO_POLL] =
    g_signal_new("do-poll",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsWorkerThreadClass, do_poll),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_worker_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_worker_thread_connect;
  connectable->disconnect = ags_worker_thread_disconnect;
}

void
ags_worker_thread_init(AgsWorkerThread *worker_thread)
{
  AgsThread *thread;

  thread = (AgsThread *) worker_thread;
  thread->freq = AGS_WORKER_THREAD_DEFAULT_JIFFIE;
  
  g_atomic_int_set(&(worker_thread->flags),
		   0);

  /* synchronization */
  worker_thread->run_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));

  pthread_mutexattr_init(worker_thread->run_mutexattr);
  pthread_mutexattr_settype(worker_thread->run_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

  worker_thread->run_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(worker_thread->run_mutex, worker_thread->run_mutexattr);

  worker_thread->run_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(worker_thread->run_cond, NULL);

  /* worker thread */
  worker_thread->worker_thread = (pthread_t *) malloc(sizeof(pthread_t));  
}

void
ags_worker_thread_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_worker_thread_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_worker_thread_finalize(GObject *gobject)
{
  AgsWorkerThread *worker_thread;
  
  gboolean running;
  gboolean do_exit;
  
  pthread_t *thread_ptr;
  pthread_attr_t *thread_attr;

  void *stackaddr;
  size_t stacksize;

  worker_thread = AGS_WORKER_THREAD(gobject);

  if(worker_thread == ags_thread_self()){
    do_exit = TRUE;
  }else{
    do_exit = FALSE;
  }
  
  thread_attr = worker_thread->worker_thread_attr;
  thread_ptr = worker_thread->worker_thread;

  running = ((AGS_WORKER_THREAD_RUNNING & (g_atomic_int_get(&(worker_thread->flags)))) != 0) ? TRUE: FALSE;

  /*  */
  pthread_attr_getstack(thread_attr,
			&stackaddr, &stacksize);
  
  /* run mutex and condition */
  pthread_mutex_destroy(worker_thread->run_mutex);
  free(worker_thread->run_mutex);

  pthread_cond_destroy(worker_thread->run_cond);
  free(worker_thread->run_cond);

  /* call parent */
  G_OBJECT_CLASS(ags_worker_thread_parent_class)->finalize(gobject);

  if(running){
    pthread_detach(*thread_ptr);
  }

  pthread_attr_destroy(thread_attr);
  free(thread_attr);

  //  free(*thread_ptr);
  free(thread_ptr);

  if(stackaddr != NULL){
    free(stackaddr);
  }
  
  if(do_exit){
    pthread_exit(NULL);
  }
}

void
ags_worker_thread_start(AgsThread *thread)
{
  AgsWorkerThread *worker_thread;

  worker_thread = AGS_WORKER_THREAD(thread);
  
  /*  */
  if((AGS_THREAD_SINGLE_LOOP & (g_atomic_int_get(&(thread->flags)))) == 0){
    AGS_THREAD_CLASS(ags_worker_thread_parent_class)->start(thread);
  }

  g_atomic_int_or(&(worker_thread->flags),
		  AGS_WORKER_THREAD_RUNNING);

  pthread_create(worker_thread->worker_thread, worker_thread->worker_thread_attr,
		 ags_woker_thread_do_poll_loop, worker_thread);
}

void
ags_worker_thread_run(AgsThread *thread)
{
  AgsWorkerThread *worker_thread;

  worker_thread = AGS_WORKER_THREAD(thread);
  
  /* synchronization point */
  if((AGS_WORKER_THREAD_RUN_WAIT & (g_atomic_int_get(&(worker_thread->flags)))) != 0 &&
     (AGS_WORKER_THREAD_RUN_DONE & (g_atomic_int_get(&(worker_thread->flags)))) == 0){
    pthread_mutex_lock(worker_thread->run_mutex);

    g_atomic_int_or(&(worker_thread->flags),
		    AGS_WORKER_THREAD_RUN_SYNC);
    
    while((AGS_WORKER_THREAD_RUN_WAIT & (g_atomic_int_get(&(worker_thread->flags)))) != 0 &&
	  (AGS_WORKER_THREAD_RUN_DONE & (g_atomic_int_get(&(worker_thread->flags)))) == 0){
      pthread_cond_wait(worker_thread->run_cond,
			worker_thread->run_mutex);
    }

    g_atomic_int_and(&(worker_thread->flags),
		     (~(AGS_WORKER_THREAD_RUN_WAIT |
			AGS_WORKER_THREAD_RUN_DONE)));

    pthread_mutex_unlock(worker_thread->run_mutex);
  }
}

void
ags_worker_thread_stop(AgsThread *thread)
{
  AgsWorkerThread *worker_thread;

  worker_thread = AGS_WORKER_THREAD(thread);

  g_atomic_int_and(&(worker_thread->flags),
		   (~(AGS_WORKER_THREAD_RUNNING)));
  
  /* call parent */
  AGS_THREAD_CLASS(ags_worker_thread_parent_class)->stop(thread);  
}

/**
 * ags_woker_thread_do_poll_loop:
 * @ptr: the #AgsWorkerThread
 * 
 * Do loop and invoke ags_worker_thread_do_poll() unless flag
 * AGS_WORKER_THREAD_RUNNING was unset.
 * 
 * Since: 1.0.0.8
 */
void*
ags_woker_thread_do_poll_loop(void *ptr)
{
  AgsWorkerThread *worker_thread;

  worker_thread = (AgsWorkerThread *) ptr;
  
  while((AGS_WORKER_THREAD_RUNNING & (g_atomic_int_get(&(worker_thread->flags)))) != 0){
    ags_worker_thread_do_poll(worker_thread);
  }
  
  pthread_exit(NULL);
}

/**
 * ags_worker_thread_do_poll:
 * @worker_thread: the #AgsWorkerThread
 *
 * Do poll your work. It is called of the worker thread.
 *
 * Since: 1.0.0.8
 */
void
ags_worker_thread_do_poll(AgsWorkerThread *worker_thread)
{
  g_return_if_fail(AGS_IS_WORKER_THREAD(worker_thread));
  g_object_ref(worker_thread);
  g_signal_emit(worker_thread,
		worker_thread_signals[DO_POLL], 0);
  g_object_unref(worker_thread);
}

/**
 * ags_worker_thread_new:
 *
 * Create a new #AgsWorkerThread.
 *
 * Returns: the new #AgsWorkerThread
 *
 * Since: 1.0.0.8
 */
AgsWorkerThread*
ags_worker_thread_new()
{
  AgsWorkerThread *worker_thread;
  
  worker_thread = (AgsWorkerThread *) g_object_new(AGS_TYPE_WORKER_THREAD,
						   NULL);

  return(worker_thread);
}
