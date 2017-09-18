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

#include <ags/thread/ags_task_thread.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_main_loop.h>
#include <ags/object/ags_async_queue.h>

#include <ags/thread/ags_concurrency_provider.h>
#include <ags/thread/ags_returnable_thread.h>

#include <sys/types.h>

#include <math.h>

void ags_task_thread_class_init(AgsTaskThreadClass *task_thread);
void ags_task_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_task_thread_async_queue_interface_init(AgsAsyncQueueInterface *async_queue);
void ags_task_thread_init(AgsTaskThread *task_thread);
void ags_task_thread_connect(AgsConnectable *connectable);
void ags_task_thread_disconnect(AgsConnectable *connectable);
void ags_task_thread_finalize(GObject *gobject);
void ags_task_thread_set_run_mutex(AgsAsyncQueue *async_queue, pthread_mutex_t *run_mutex);
pthread_mutex_t* ags_task_thread_get_run_mutex(AgsAsyncQueue *async_queue);
void ags_task_thread_set_run_cond(AgsAsyncQueue *async_queue, pthread_cond_t *run_cond);
pthread_cond_t* ags_task_thread_get_run_cond(AgsAsyncQueue *async_queue);
void ags_task_thread_set_run(AgsAsyncQueue *async_queue, gboolean is_run);
gboolean ags_task_thread_is_run(AgsAsyncQueue *async_queue);
void ags_task_thread_increment_wait_ref(AgsAsyncQueue *async_queue);
guint ags_task_thread_get_wait_ref(AgsAsyncQueue *async_queue);

void ags_task_thread_start(AgsThread *thread);
void ags_task_thread_run(AgsThread *thread);

void ags_task_thread_append_task_queue(AgsReturnableThread *returnable_thread, gpointer data);
void ags_task_thread_append_tasks_queue(AgsReturnableThread *returnable_thread, gpointer data);

/**
 * SECTION:ags_task_thread
 * @short_description: task thread
 * @title: AgsTaskThread
 * @section_id:
 * @include: ags/thread/ags_task_thread.h
 *
 * The #AgsTaskThread acts as task queue thread.
 */

enum{
  CLEAR_CACHE,
  LAST_SIGNAL,
};

static gpointer ags_task_thread_parent_class = NULL;
static AgsConnectableInterface *ags_task_thread_parent_connectable_interface;
static guint task_thread_signals[LAST_SIGNAL];

GType
ags_task_thread_get_type()
{
  static GType ags_type_task_thread = 0;

  if(!ags_type_task_thread){
    static const GTypeInfo ags_task_thread_info = {
      sizeof (AgsTaskThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_task_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTaskThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_task_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_task_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_async_queue_interface_info = {
      (GInterfaceInitFunc) ags_task_thread_async_queue_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_task_thread = g_type_register_static(AGS_TYPE_THREAD,
						  "AgsTaskThread",
						  &ags_task_thread_info,
						  0);
    
    g_type_add_interface_static(ags_type_task_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_task_thread,
				AGS_TYPE_ASYNC_QUEUE,
				&ags_async_queue_interface_info);
  }
  
  return (ags_type_task_thread);
}

void
ags_task_thread_class_init(AgsTaskThreadClass *task_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;

  ags_task_thread_parent_class = g_type_class_peek_parent(task_thread);

  /* GObject */
  gobject = (GObjectClass *) task_thread;

  gobject->finalize = ags_task_thread_finalize;

  /* AgsThread */
  thread = (AgsThreadClass *) task_thread;

  thread->start = ags_task_thread_start;
  thread->run = ags_task_thread_run;

  /* AgsTaskThread */
  task_thread->clear_cache = NULL;

  /* signals */
  /**
   * AgsTaskThread::clear-cache:
   * @task_thread: the object playing.
   *
   * The ::clear-cache signal is invoked to clear the cache libraries
   * might have been allocated.
   *
   * Since: 0.7.122
   */
  task_thread_signals[CLEAR_CACHE] =
    g_signal_new("clear-cache",
		 G_TYPE_FROM_CLASS (task_thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsTaskThreadClass, clear_cache),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

}

void
ags_task_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_task_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_task_thread_connect;
  connectable->disconnect = ags_task_thread_disconnect;
}

void
ags_task_thread_async_queue_interface_init(AgsAsyncQueueInterface *async_queue)
{
  async_queue->set_run_mutex = ags_task_thread_set_run_mutex;
  async_queue->get_run_mutex = ags_task_thread_get_run_mutex;

  async_queue->set_run_cond = ags_task_thread_set_run_cond;
  async_queue->get_run_cond = ags_task_thread_get_run_cond;

  async_queue->set_run = ags_task_thread_set_run;
  async_queue->is_run = ags_task_thread_is_run;

  async_queue->increment_wait_ref = ags_task_thread_increment_wait_ref;
  async_queue->get_wait_ref = ags_task_thread_get_wait_ref;
}

void
ags_task_thread_init(AgsTaskThread *task_thread)
{
  AgsThread *thread;

  thread = AGS_THREAD(task_thread);

  //  g_atomic_int_or(&(thread->flags),
  //		  AGS_THREAD_LOCK_GREEDY_RUN_MUTEX);

  thread->freq = AGS_TASK_THREAD_DEFAULT_JIFFIE;

  task_thread->flags = 0;
  
  /* async queue */
  g_atomic_int_set(&(task_thread->is_run),
		   FALSE);

  g_atomic_int_set(&(task_thread->wait_ref),
		   0);

  /* sync mutex and cond */
  task_thread->sync_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));

  pthread_mutexattr_init(task_thread->sync_mutexattr);
  pthread_mutexattr_settype(task_thread->sync_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(task_thread->sync_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  task_thread->sync_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(task_thread->sync_mutex, task_thread->sync_mutexattr);

  task_thread->sync_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(task_thread->sync_cond, NULL);

  /* extern sync mutex and cond */
  task_thread->extern_sync_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));

  pthread_mutexattr_init(task_thread->extern_sync_mutexattr);
  pthread_mutexattr_settype(task_thread->extern_sync_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(task_thread->extern_sync_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  task_thread->extern_sync_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(task_thread->extern_sync_mutex, task_thread->extern_sync_mutexattr);

  task_thread->extern_sync_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(task_thread->extern_sync_cond, NULL);
  
  /* run mutex and cond */
  task_thread->run_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));

  pthread_mutexattr_init(task_thread->run_mutexattr);
  pthread_mutexattr_settype(task_thread->run_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(task_thread->run_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  task_thread->run_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(task_thread->run_mutex, task_thread->run_mutexattr);

  task_thread->run_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(task_thread->run_cond, NULL);
  
  /* read mutex */
  task_thread->read_mutexattr = NULL;
  
  task_thread->read_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(task_thread->read_mutex, NULL);

  /* launch mutex */
  task_thread->launch_mutexattr = NULL;

  task_thread->launch_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(task_thread->launch_mutex, NULL);

  /* queue and exec */
  g_atomic_int_set(&(task_thread->queued),
		   0);
  g_atomic_int_set(&(task_thread->pending),
		   0);

  g_atomic_pointer_set(&(task_thread->exec), NULL);
  g_atomic_pointer_set(&(task_thread->queue),
		       NULL);

  /* cyclic task */
  task_thread->cyclic_task_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));

  pthread_mutexattr_init(task_thread->cyclic_task_mutexattr);
  pthread_mutexattr_settype(task_thread->cyclic_task_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(task_thread->cyclic_task_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  task_thread->cyclic_task_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(task_thread->cyclic_task_mutex, task_thread->cyclic_task_mutexattr);


  g_atomic_pointer_set(&(task_thread->cyclic_task),
		       NULL);
    
  /* thread pool */
  task_thread->thread_pool = ags_thread_pool_new((AgsThread *) task_thread);
  task_thread->thread_pool->parent = (AgsThread *) task_thread;
}

void
ags_task_thread_connect(AgsConnectable *connectable)
{
  /* empty */

  ags_task_thread_parent_connectable_interface->connect(connectable);
}

void
ags_task_thread_disconnect(AgsConnectable *connectable)
{
  ags_task_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_task_thread_finalize(GObject *gobject)
{
  AgsTaskThread *task_thread;

  task_thread = AGS_TASK_THREAD(gobject);

  /* sync mutex and cond */
  pthread_mutexattr_destroy(task_thread->sync_mutexattr);
  free(task_thread->sync_mutexattr);

  pthread_mutex_destroy(task_thread->sync_mutex);
  free(task_thread->sync_mutex);

  pthread_cond_destroy(task_thread->sync_cond);
  free(task_thread->sync_cond);

  /* extern sync mutex and cond */
  pthread_mutexattr_destroy(task_thread->extern_sync_mutexattr);
  free(task_thread->extern_sync_mutexattr);

  pthread_mutex_destroy(task_thread->extern_sync_mutex);
  free(task_thread->extern_sync_mutex);

  pthread_cond_destroy(task_thread->extern_sync_cond);
  free(task_thread->extern_sync_cond);
    
  /* run mutex and cond */
  pthread_mutexattr_destroy(task_thread->run_mutexattr);
  free(task_thread->run_mutexattr);

  pthread_mutex_destroy(task_thread->run_mutex);
  free(task_thread->run_mutex);

  pthread_cond_destroy(task_thread->run_cond);
  free(task_thread->run_cond);

  /* read mutex */
  pthread_mutex_destroy(task_thread->read_mutex);
  free(task_thread->read_mutex);
  
  /* launch mutex */
  pthread_mutex_destroy(task_thread->launch_mutex);
  free(task_thread->launch_mutex);

  /* free AgsTask lists */
  g_list_free_full(g_atomic_pointer_get(&(task_thread->exec)),
		   g_object_unref);
  
  g_list_free_full(g_atomic_pointer_get(&(task_thread->queue)),
		   g_object_unref);

  /* cyclic task */
  pthread_mutexattr_destroy(task_thread->cyclic_task_mutexattr);
  free(task_thread->cyclic_task_mutexattr);

  pthread_mutex_destroy(task_thread->cyclic_task_mutex);
  free(task_thread->cyclic_task_mutex);
  
  g_list_free_full(g_atomic_pointer_get(&(task_thread->cyclic_task)),
		   g_object_unref);

  /*  */
  G_OBJECT_CLASS(ags_task_thread_parent_class)->finalize(gobject);
}

void
ags_task_thread_set_run_mutex(AgsAsyncQueue *async_queue, pthread_mutex_t *run_mutex)
{
  AGS_TASK_THREAD(async_queue)->run_mutex = run_mutex;
}

pthread_mutex_t*
ags_task_thread_get_run_mutex(AgsAsyncQueue *async_queue)
{
  return(AGS_TASK_THREAD(async_queue)->run_mutex);
}

void
ags_task_thread_set_run_cond(AgsAsyncQueue *async_queue, pthread_cond_t *run_cond)
{
  AGS_TASK_THREAD(async_queue)->run_cond = run_cond;
}

pthread_cond_t*
ags_task_thread_get_run_cond(AgsAsyncQueue *async_queue)
{
  return(AGS_TASK_THREAD(async_queue)->run_cond);
}

void
ags_task_thread_set_run(AgsAsyncQueue *async_queue, gboolean is_run)
{
  g_atomic_int_set(&(AGS_TASK_THREAD(async_queue)->is_run),
		   is_run);
}

gboolean
ags_task_thread_is_run(AgsAsyncQueue *async_queue)
{
  return(g_atomic_int_get(&(AGS_TASK_THREAD(async_queue)->is_run)));
}

void
ags_task_thread_increment_wait_ref(AgsAsyncQueue *async_queue)
{
  g_atomic_int_inc(&(AGS_TASK_THREAD(async_queue)->wait_ref));
}

guint
ags_task_thread_get_wait_ref(AgsAsyncQueue *async_queue)
{
  return(g_atomic_int_get(&(AGS_TASK_THREAD(async_queue)->wait_ref)));
}

void
ags_task_thread_start(AgsThread *thread)
{
  if((AGS_THREAD_SINGLE_LOOP & (g_atomic_int_get(&(thread->flags)))) == 0){
    AGS_THREAD_CLASS(ags_task_thread_parent_class)->start(thread);
  }
}

void
ags_task_thread_run(AgsThread *thread)
{
  AgsTaskThread *task_thread;

  GList *list;

  guint flags;
  guint prev_pending;
  gboolean do_sync;
  static gboolean initialized = FALSE;

  task_thread = AGS_TASK_THREAD(thread);
  
  if(!initialized){
    //    play_idle.tv_sec = 0;
    //    play_idle.tv_nsec = 10 * round(sysconf(_SC_CLK_TCK) * (double) buffer_size  / (double) samplerate);
    //    idle = sysconf(_SC_CLK_TCK) * round(sysconf(_SC_CLK_TCK) * (double) buffer_size  / (double) samplerate / 8.0);

    initialized = TRUE;
  }

  /* real-time setup */
#ifdef AGS_WITH_RT
  if((AGS_THREAD_RT_SETUP & (g_atomic_int_get(&(thread->flags)))) == 0){
    struct sched_param param;
    
    /* Declare ourself as a real time task */
    param.sched_priority = AGS_TASK_THREAD_RT_PRIORITY;
      
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
      perror("sched_setscheduler failed");
    }

    g_atomic_int_or(&(thread->flags),
		    AGS_THREAD_RT_SETUP);
  }
#endif
  
  /*  */
  pthread_mutex_lock(task_thread->read_mutex);

  g_atomic_pointer_set(&(task_thread->exec),
		       g_atomic_pointer_get(&(task_thread->queue)));
  g_atomic_pointer_set(&(task_thread->queue),
		       NULL);

  list = g_atomic_pointer_get(&(task_thread->exec));
    
  prev_pending = g_atomic_int_get(&(task_thread->pending));
  g_atomic_int_set(&(task_thread->pending),
		   g_list_length(list));
  g_atomic_int_set(&(task_thread->queued),
		   g_atomic_int_get(&(task_thread->queued)) - prev_pending);

  pthread_mutex_unlock(task_thread->read_mutex);

  /*  */
  flags = g_atomic_int_get(&(task_thread->flags));
  do_sync = FALSE;

  if((AGS_TASK_THREAD_EXTERN_SYNC & flags) != 0){
    if((AGS_TASK_THREAD_EXTERN_READY & flags) != 0){
      g_atomic_int_or(&(task_thread->flags),
		      AGS_TASK_THREAD_EXTERN_AVAILABLE);

      do_sync = TRUE;
      
      pthread_mutex_lock(task_thread->sync_mutex);
    
      if((AGS_TASK_THREAD_SYNC_WAIT & (g_atomic_int_get(&(task_thread->flags)))) != 0 &&
	 (AGS_TASK_THREAD_SYNC_DONE & (g_atomic_int_get(&(task_thread->flags)))) == 0){
	g_atomic_int_and(&(task_thread->flags),
			 (~AGS_TASK_THREAD_SYNC_DONE));
      
	while((AGS_TASK_THREAD_SYNC_WAIT & (g_atomic_int_get(&(task_thread->flags)))) != 0 &&
	      (AGS_TASK_THREAD_SYNC_DONE & (g_atomic_int_get(&(task_thread->flags)))) == 0){
	  pthread_cond_wait(task_thread->sync_cond,
			    task_thread->sync_mutex);
	}
      }

      g_atomic_int_or(&(task_thread->flags),
		      (AGS_TASK_THREAD_SYNC_WAIT |
		       AGS_TASK_THREAD_SYNC_DONE));		       

      pthread_mutex_unlock(task_thread->sync_mutex);
    }
  }

  /* launch tasks */
  if(list != NULL){
    AgsTask *task;
    
    int i;
    
    pthread_mutex_lock(task_thread->launch_mutex);

    for(i = 0; i < g_atomic_int_get(&(task_thread->pending)); i++){
      task = AGS_TASK(list->data);

#ifdef AGS_DEBUG
      g_message("ags_task_thread - launching task: %s", G_OBJECT_TYPE_NAME(task));
#endif

      ags_task_launch(task);
      
      list = list->next;
    }
    
    pthread_mutex_unlock(task_thread->launch_mutex);
  }

  pthread_mutex_lock(task_thread->read_mutex);

  g_list_free_full(g_atomic_pointer_get(&(task_thread->exec)),
		   g_object_unref);
  g_atomic_pointer_set(&(task_thread->exec),
		       NULL);

  pthread_mutex_unlock(task_thread->read_mutex);

  /* cyclic task */
  pthread_mutex_lock(task_thread->cyclic_task_mutex);

  list = g_atomic_pointer_get(&(task_thread->cyclic_task));

  if(list != NULL){
    AgsTask *task;
    
    int i;

    while(list != NULL){
      task = AGS_TASK(list->data);

#ifdef AGS_DEBUG
      g_message("ags_task_thread - launching cyclic task: %s", G_OBJECT_TYPE_NAME(task));
#endif

      ags_task_launch(task);
      
      list = list->next;
    }
  }
  
  pthread_mutex_unlock(task_thread->cyclic_task_mutex);

  if(do_sync){
    pthread_mutex_lock(task_thread->extern_sync_mutex);

    g_atomic_int_and(&(task_thread->flags),
		     (~AGS_TASK_THREAD_EXTERN_SYNC_WAIT));
  
    if((AGS_TASK_THREAD_EXTERN_SYNC_DONE & (g_atomic_int_get(&(task_thread->flags)))) == 0){
      pthread_cond_signal(task_thread->extern_sync_cond);
    }

    pthread_mutex_unlock(task_thread->extern_sync_mutex);
  }
    
  /* async queue */
  pthread_mutex_lock(task_thread->run_mutex);
  
  ags_async_queue_set_run(AGS_ASYNC_QUEUE(task_thread),
			  TRUE);
	
  if(ags_async_queue_get_wait_ref(AGS_ASYNC_QUEUE(task_thread)) != 0){
    pthread_cond_broadcast(task_thread->run_cond);
  }

  g_atomic_int_set(&(task_thread->wait_ref),
		   0);
  
  pthread_mutex_unlock(task_thread->run_mutex);

  /* clean-up */
  ags_task_thread_clear_cache(task_thread);
}

void
ags_task_thread_append_task_queue(AgsReturnableThread *returnable_thread, gpointer data)
{
  AgsTask *task;
  AgsTaskThread *task_thread;

  AgsTaskThreadAppend *append;
  GList *tmplist;

  gboolean initial_wait;
  int ret;

  append = (AgsTaskThreadAppend *) g_atomic_pointer_get(&(returnable_thread->safe_data));

  task_thread = g_atomic_pointer_get(&(append->task_thread));
  task = AGS_TASK(g_atomic_pointer_get(&(append->data)));

  free(append);

  /* lock */
  pthread_mutex_lock(task_thread->read_mutex);

  /* append to queue */
  g_atomic_int_set(&(task_thread->queued),
		   g_atomic_int_get(&(task_thread->queued)) + 1);

  tmplist = g_atomic_pointer_get(&(task_thread->queue));
  g_atomic_pointer_set(&(task_thread->queue),
		       g_list_append(tmplist, task));

  /* unlock */
  pthread_mutex_unlock(task_thread->read_mutex);
  /*  */
  //  g_message("ags_task_thread_append_task_thread ------------------------- %d", devout->append_task_suspend);

  g_object_unref(returnable_thread);
}

/**
 * ags_task_thread_append_task:
 * @task_thread: an #AgsTaskThread
 * @task: an #AgsTask
 *
 * Adds the task to @task_thread.
 *
 * Since: 0.4
 */
void
ags_task_thread_append_task(AgsTaskThread *task_thread, AgsTask *task)
{
  AgsTaskThreadAppend *append;
  AgsThread *thread;

  guint flags;
  
#ifdef AGS_DEBUG
  g_message("append task");
#endif

  append = (AgsTaskThreadAppend *) malloc(sizeof(AgsTaskThreadAppend));

  g_atomic_pointer_set(&(append->task_thread),
		       task_thread);
  g_atomic_pointer_set(&(append->data),
		       task);

  thread = ags_thread_pool_pull(task_thread->thread_pool);
  //  g_object_ref(thread);
  
  pthread_mutex_lock(AGS_RETURNABLE_THREAD(thread)->reset_mutex);

  g_atomic_pointer_set(&(AGS_RETURNABLE_THREAD(thread)->safe_data),
		       append);

  ags_returnable_thread_connect_safe_run(AGS_RETURNABLE_THREAD(thread),
					 ags_task_thread_append_task_queue);

  g_atomic_int_or(&(AGS_RETURNABLE_THREAD(thread)->flags),
		  AGS_RETURNABLE_THREAD_IN_USE);
    
  pthread_mutex_unlock(AGS_RETURNABLE_THREAD(thread)->reset_mutex);
}

void
ags_task_thread_append_tasks_queue(AgsReturnableThread *returnable_thread, gpointer data)
{
  AgsTask *task;
  AgsTaskThread *task_thread;
  AgsTaskThreadAppend *append;
  GList *list, *tmplist;
  gboolean initial_wait;
  int ret;

  append = (AgsTaskThreadAppend *) g_atomic_pointer_get(&(returnable_thread->safe_data));

  task_thread = g_atomic_pointer_get(&(append->task_thread));
  list = (GList *) g_atomic_pointer_get(&(append->data));

  free(append);

  /* lock */
  pthread_mutex_lock(task_thread->read_mutex);

  /* append to queue */
  g_atomic_int_set(&(task_thread->queued),
		   g_atomic_int_get(&(task_thread->queued)) + g_list_length(list));

  tmplist = g_atomic_pointer_get(&(task_thread->queue));
  g_atomic_pointer_set(&(task_thread->queue),
		       g_list_concat(tmplist, list));

  /*  */
  pthread_mutex_unlock(task_thread->read_mutex);

  g_object_unref(returnable_thread);
}

/**
 * ags_task_thread_append_tasks:
 * @task_thread: an #AgsTaskThread
 * @list: a GList with #AgsTask as data
 *
 * Concats the list with @task_thread's internal task list. Don't
 * free the list you pass. It will be freed for you.
 *
 * Since: 0.4
 */
void
ags_task_thread_append_tasks(AgsTaskThread *task_thread, GList *list)
{
  AgsTaskThreadAppend *append;
  AgsThread *thread;

  guint flags;
  
#ifdef AGS_DEBUG
  g_message("append tasks");
#endif

  /* pull */
  append = (AgsTaskThreadAppend *) malloc(sizeof(AgsTaskThreadAppend));

  g_atomic_pointer_set(&(append->task_thread),
		       task_thread);
  g_atomic_pointer_set(&(append->data),
		       list);

  thread = ags_thread_pool_pull(task_thread->thread_pool);
  //  g_object_ref(thread);

  /* set safe data and run */
  pthread_mutex_lock(AGS_RETURNABLE_THREAD(thread)->reset_mutex);

  g_atomic_pointer_set(&(AGS_RETURNABLE_THREAD(thread)->safe_data),
		       append);
  
  ags_returnable_thread_connect_safe_run(AGS_RETURNABLE_THREAD(thread),
					 ags_task_thread_append_tasks_queue);
  g_atomic_int_or(&(AGS_RETURNABLE_THREAD(thread)->flags),
		  AGS_RETURNABLE_THREAD_IN_USE);
  
  pthread_mutex_unlock(AGS_RETURNABLE_THREAD(thread)->reset_mutex);
}

/**
 * ags_task_thread_append_cyclic_task:
 * @task_thread: the #AgsTaskThread
 * @task: the #AgsTask
 *
 * Add cyclic task.
 * 
 * Since: 0.7.86
 */
void
ags_task_thread_append_cyclic_task(AgsTaskThread *task_thread,
				   AgsTask *task)
{
  pthread_mutex_lock(task_thread->cyclic_task_mutex);

  g_atomic_pointer_set(&(task_thread->cyclic_task),
		       g_list_append(g_atomic_pointer_get(&(task_thread->cyclic_task)),
				     task));

  pthread_mutex_unlock(task_thread->cyclic_task_mutex);
}

/**
 * ags_task_thread_remove_cyclic_task:
 * @task_thread: the #AgsTaskThread
 * @task: the #AgsTask
 *
 * Remove cyclic task.
 * 
 * Since: 0.7.86
 */
void
ags_task_thread_remove_cyclic_task(AgsTaskThread *task_thread,
				   AgsTask *task)
{
  pthread_mutex_lock(task_thread->cyclic_task_mutex);

  g_atomic_pointer_set(&(task_thread->cyclic_task),
		       g_list_remove(g_atomic_pointer_get(&(task_thread->cyclic_task)),
				     task));

  pthread_mutex_unlock(task_thread->cyclic_task_mutex);  
}

/**
 * ags_task_thread_clear_cache:
 * @task_thread: the #AgsTaskThread
 *
 * Clear cache signal.
 *
 * Since: 0.7.28
 */
void
ags_task_thread_clear_cache(AgsTaskThread *task_thread)
{
  g_return_if_fail(AGS_IS_TASK_THREAD(task_thread));

  g_object_ref(task_thread);
  g_signal_emit(task_thread,
		task_thread_signals[CLEAR_CACHE], 0);
  g_object_unref(task_thread);
}

/**
 * ags_task_thread_new:
 *
 * Create a new #AgsTaskThread.
 *
 * Returns: the new #AgsTaskThread
 *
 * Since: 0.4
 */ 
AgsTaskThread*
ags_task_thread_new()
{
  AgsTaskThread *task_thread;

  task_thread = (AgsTaskThread *) g_object_new(AGS_TYPE_TASK_THREAD,
					       NULL);


  return(task_thread);
}
