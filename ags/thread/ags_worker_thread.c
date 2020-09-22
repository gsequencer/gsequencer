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

#include <ags/thread/ags_worker_thread.h>

#include <stdlib.h>

void ags_worker_thread_class_init(AgsWorkerThreadClass *worker_thread);
void ags_worker_thread_init(AgsWorkerThread *worker_thread);
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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_worker_thread = 0;

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

    ags_type_worker_thread = g_type_register_static(AGS_TYPE_THREAD,
						    "AgsWorkerThread",
						    &ags_worker_thread_info,
						    0);    

    g_once_init_leave(&g_define_type_id__volatile, ags_type_worker_thread);
  }

  return g_define_type_id__volatile;
}

GType
ags_worker_thread_status_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_WORKER_THREAD_STATUS_RUNNING, "AGS_WORKER_THREAD_STATUS_RUNNING", "worker-thread-status-running" },
      { AGS_WORKER_THREAD_STATUS_RUN_WAIT, "AGS_WORKER_THREAD_STATUS_RUN_WAIT", "worker-thread-status-run-wait" },
      { AGS_WORKER_THREAD_STATUS_RUN_DONE, "AGS_WORKER_THREAD_STATUS_RUN_DONE", "worker-thread-status-run-done" },
      { AGS_WORKER_THREAD_STATUS_RUN_SYNC, "AGS_WORKER_THREAD_STATUS_RUN_SYNC", "worker-thread-status-run-sync" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsWorkerThreadStatusFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
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
   * Since: 3.0.0
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
ags_worker_thread_init(AgsWorkerThread *worker_thread)
{
  AgsThread *thread;

  thread = (AgsThread *) worker_thread;

  g_object_set(thread,
	       "frequency", AGS_WORKER_THREAD_DEFAULT_JIFFIE,
	       NULL);
  
  g_atomic_int_set(&(worker_thread->status_flags),
		   0);

  /* synchronization */
  g_mutex_init(&(worker_thread->run_mutex));

  g_cond_init(&(worker_thread->run_cond));

  /* worker thread */
  worker_thread->worker_thread = NULL;
}

void
ags_worker_thread_finalize(GObject *gobject)
{
  AgsWorkerThread *worker_thread;
  
  gboolean running;
  gboolean do_exit;
  
  worker_thread = AGS_WORKER_THREAD(gobject);

  if(worker_thread == ags_thread_self()){
    do_exit = TRUE;
  }else{
    do_exit = FALSE;
  }

  running = ags_worker_thread_test_status_flags(worker_thread, AGS_WORKER_THREAD_STATUS_RUNNING);

  /* call parent */
  G_OBJECT_CLASS(ags_worker_thread_parent_class)->finalize(gobject);
  
  if(do_exit){
    g_thread_exit(NULL);
  }
}

void
ags_worker_thread_start(AgsThread *thread)
{
  AgsWorkerThread *worker_thread;

  worker_thread = AGS_WORKER_THREAD(thread);
  
  /*  */
  AGS_THREAD_CLASS(ags_worker_thread_parent_class)->start(thread);

  ags_worker_thread_set_status_flags(worker_thread, AGS_WORKER_THREAD_STATUS_RUNNING);

  worker_thread->worker_thread = g_thread_new("Advanced Gtk+ Sequencer - worker",
					      ags_woker_thread_do_poll_loop,
					      worker_thread);
}

void
ags_worker_thread_run(AgsThread *thread)
{
  AgsWorkerThread *worker_thread;

  worker_thread = AGS_WORKER_THREAD(thread);
  
  /* synchronization point */
  if(ags_worker_thread_test_status_flags(worker_thread, AGS_WORKER_THREAD_STATUS_RUN_WAIT) &&
     !ags_worker_thread_test_status_flags(worker_thread, AGS_WORKER_THREAD_STATUS_RUN_DONE)){
    g_mutex_lock(&(worker_thread->run_mutex));

    ags_worker_thread_set_status_flags(worker_thread, AGS_WORKER_THREAD_STATUS_RUN_SYNC);
    
    while(ags_worker_thread_test_status_flags(worker_thread, AGS_WORKER_THREAD_STATUS_RUN_WAIT) &&
	  !ags_worker_thread_test_status_flags(worker_thread, AGS_WORKER_THREAD_STATUS_RUN_DONE)){
      g_cond_wait(&(worker_thread->run_cond),
		  &(worker_thread->run_mutex));
    }

    ags_worker_thread_set_status_flags(worker_thread, (AGS_WORKER_THREAD_STATUS_RUN_WAIT |
						       AGS_WORKER_THREAD_STATUS_RUN_DONE));

    g_mutex_unlock(&(worker_thread->run_mutex));
  }
}

void
ags_worker_thread_stop(AgsThread *thread)
{
  AgsWorkerThread *worker_thread;

  worker_thread = AGS_WORKER_THREAD(thread);

  ags_worker_thread_unset_status_flags(worker_thread, AGS_WORKER_THREAD_STATUS_RUNNING);
  
  /* call parent */
  AGS_THREAD_CLASS(ags_worker_thread_parent_class)->stop(thread);  
}

/**
 * ags_worker_thread_test_status_flags:
 * @worker_thread: the #AgsWorkerThread
 * @status_flags: status flags
 * 
 * Test @status_flags of @worker_thread.
 * 
 * Returns: %TRUE if status flags set, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_worker_thread_test_status_flags(AgsWorkerThread *worker_thread, guint status_flags)
{
  gboolean retval;
  
  if(!AGS_IS_WORKER_THREAD(worker_thread)){
    return(FALSE);
  }

  retval = ((status_flags & (g_atomic_int_get(&(worker_thread->status_flags)))) != 0) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_worker_thread_set_status_flags:
 * @worker_thread: the #AgsWorkerThread
 * @status_flags: status flags
 * 
 * Set status flags.
 * 
 * Since: 3.0.0
 */
void
ags_worker_thread_set_status_flags(AgsWorkerThread *worker_thread, guint status_flags)
{
  if(!AGS_IS_WORKER_THREAD(worker_thread)){
    return;
  }

  g_atomic_int_or(&(worker_thread->status_flags),
		  status_flags);
}

/**
 * ags_worker_thread_unset_status_flags:
 * @worker_thread: the #AgsWorkerThread
 * @status_flags: status flags
 * 
 * Unset status flags.
 * 
 * Since: 3.0.0
 */
void
ags_worker_thread_unset_status_flags(AgsWorkerThread *worker_thread, guint status_flags)
{
  if(!AGS_IS_WORKER_THREAD(worker_thread)){
    return;
  }

  g_atomic_int_and(&(worker_thread->status_flags),
		   (~status_flags));
}

/**
 * ags_woker_thread_do_poll_loop:
 * @ptr: the #AgsWorkerThread
 * 
 * Do loop and invoke ags_worker_thread_do_poll() unless flag
 * AGS_WORKER_THREAD_RUNNING was unset.
 * 
 * Since: 3.0.0
 */
void*
ags_woker_thread_do_poll_loop(void *ptr)
{
  AgsWorkerThread *worker_thread;

  worker_thread = (AgsWorkerThread *) ptr;
  
  while(ags_worker_thread_test_status_flags(worker_thread, AGS_WORKER_THREAD_STATUS_RUNNING)){
    ags_worker_thread_do_poll(worker_thread);
  }
  
  g_thread_exit(NULL);

  return(NULL);
}

/**
 * ags_worker_thread_do_poll:
 * @worker_thread: the #AgsWorkerThread
 *
 * Do poll your work. It is called of the worker thread.
 *
 * Since: 3.0.0
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
 * Create a new instance of #AgsWorkerThread.
 *
 * Returns: the new #AgsWorkerThread
 *
 * Since: 3.0.0
 */
AgsWorkerThread*
ags_worker_thread_new()
{
  AgsWorkerThread *worker_thread;
  
  worker_thread = (AgsWorkerThread *) g_object_new(AGS_TYPE_WORKER_THREAD,
						   NULL);

  return(worker_thread);
}
