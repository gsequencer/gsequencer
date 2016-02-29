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

#include <ags/thread/ags_thread_pool.h>

#include <ags/object/ags_connectable.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_returnable_thread.h>

#include <stdlib.h>

void ags_thread_pool_class_init(AgsThreadPoolClass *thread_pool);
void ags_thread_pool_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_thread_pool_init(AgsThreadPool *thread_pool);
void ags_thread_pool_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_thread_pool_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_thread_pool_connect(AgsConnectable *connectable);
void ags_thread_pool_disconnect(AgsConnectable *connectable);
void ags_thread_pool_finalize(GObject *gobject);

void* ags_thread_pool_creation_thread(void *ptr);

void ags_thread_pool_real_start(AgsThreadPool *thread_pool);

/**
 * SECTION:ags_thread_pool
 * @short_description: thread pool
 * @title: AgsThreadPool
 * @section_id:
 * @include: ags/thread/ags_thread_pool.h
 *
 * The #AgsThreadPool acts as preinstantiated threads store.
 * This can achieve enormeous performance.
 */

#define AGS_THREAD_POOL_DEFAULT_MAX_UNUSED_THREADS 8
#define AGS_THREAD_POOL_DEFAULT_MAX_THREADS 128

enum{
  PROP_0,
  PROP_MAX_UNUSED_THREADS,
  PROP_MAX_THREADS,
};

enum{
  START,
  LAST_SIGNAL,
};

static gpointer ags_thread_pool_parent_class = NULL;
static guint thread_pool_signals[LAST_SIGNAL];

GType
ags_thread_pool_get_type()
{
  static GType ags_type_thread_pool = 0;

  if(!ags_type_thread_pool){
    static const GTypeInfo ags_thread_pool_info = {
      sizeof (AgsThreadPoolClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_thread_pool_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsThreadPool),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_thread_pool_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_thread_pool_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_thread_pool = g_type_register_static(G_TYPE_OBJECT,
						  "AgsThreadPool\0",
						  &ags_thread_pool_info,
						  0);

    g_type_add_interface_static(ags_type_thread_pool,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_thread_pool);
}

void
ags_thread_pool_class_init(AgsThreadPoolClass *thread_pool)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_thread_pool_parent_class = g_type_class_peek_parent(thread_pool);

  /* GObjectClass */
  gobject = (GObjectClass *) thread_pool;

  gobject->set_property = ags_thread_pool_set_property;
  gobject->get_property = ags_thread_pool_get_property;

  gobject->finalize = ags_thread_pool_finalize;

  /* properties */
  param_spec = g_param_spec_uint("max-unused-threads\0",
				 "maximum unused threads\0",
				 "The maximum of unused threads.\0",
				 1, 65535,
				 24,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAX_UNUSED_THREADS,
				  param_spec);

  param_spec = g_param_spec_uint("max-threads\0",
				 "maximum threads to use\0",
				 "The maximum of threads to be created.\0",
				 1, 65535,
				 1024,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAX_THREADS,
				  param_spec);

  /* AgsThreadPoolClass */
  thread_pool->start = ags_thread_pool_real_start;

  /* signals */
  thread_pool_signals[START] =
    g_signal_new("start\0",
		 G_TYPE_FROM_CLASS (thread_pool),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadPoolClass, start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_thread_pool_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_thread_pool_connect;
  connectable->disconnect = ags_thread_pool_disconnect;
}

void
ags_thread_pool_init(AgsThreadPool *thread_pool)
{
  AgsThread *thread;
  GList *list;
  guint i;

  g_atomic_int_set(&(thread_pool->flags),
		   0);

  g_atomic_int_set(&(thread_pool->max_unused_threads),
		   AGS_THREAD_POOL_DEFAULT_MAX_UNUSED_THREADS);
  g_atomic_int_set(&(thread_pool->max_threads),
		   AGS_THREAD_POOL_DEFAULT_MAX_THREADS);

  thread_pool->thread = (pthread_t *) malloc(sizeof(pthread_t));
  thread_pool->creation_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(thread_pool->creation_mutex, NULL);

  thread_pool->creation_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(thread_pool->creation_cond, NULL);

  g_atomic_int_set(&(thread_pool->n_threads),
		   g_atomic_int_get(&(thread_pool->max_unused_threads)));
  g_atomic_int_set(&(thread_pool->newly_pulled),
		   0);
  g_atomic_int_set(&(thread_pool->queued),
		   0);

  thread_pool->parent = NULL;
  list = NULL;

  for(i = 0; i < g_atomic_int_get(&(thread_pool->max_unused_threads)); i++){
    thread = (AgsThread *) ags_returnable_thread_new((GObject *) thread_pool);

    list = g_list_prepend(list, thread);
  }

  g_atomic_pointer_set(&(thread_pool->returnable_thread),
		       list);
  g_atomic_pointer_set(&(thread_pool->running_thread),
		       NULL);

  thread_pool->pull_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(thread_pool->pull_mutex, NULL);

  thread_pool->return_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(thread_pool->return_mutex, NULL);
  
  thread_pool->return_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(thread_pool->return_cond, NULL);
}

void
ags_thread_pool_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsThreadPool *thread_pool;

  thread_pool = AGS_THREAD_POOL(gobject);

  switch(prop_id){
  case PROP_MAX_UNUSED_THREADS:
    {
      g_atomic_int_set(&(thread_pool->max_unused_threads),
		       g_value_get_uint(value));
    }
    break;
  case PROP_MAX_THREADS:
    {
      g_atomic_int_set(&(thread_pool->max_threads),
		       g_value_get_uint(value));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_thread_pool_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsThreadPool *thread_pool;

  thread_pool = AGS_THREAD_POOL(gobject);

  switch(prop_id){
  case PROP_MAX_UNUSED_THREADS:
    {
      g_value_set_uint(value, g_atomic_int_get(&(thread_pool->max_unused_threads)));
    }
    break;
  case PROP_MAX_THREADS:
    {
      g_value_set_uint(value, g_atomic_int_get(&(thread_pool->max_threads)));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_thread_pool_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_thread_pool_disconnect(AgsConnectable *connectable)
{
  AgsThreadPool *thread_pool;
  GList *list;

  thread_pool = AGS_THREAD_POOL(connectable);

  list = g_atomic_pointer_get(&(thread_pool->returnable_thread));

  //TODO:JK: implement me
}

void
ags_thread_pool_finalize(GObject *gobject)
{
  AgsThreadPool *thread_pool;

  thread_pool = AGS_THREAD_POOL(gobject);

  g_list_free_full(g_atomic_pointer_get(&(thread_pool->returnable_thread)),
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_thread_pool_parent_class)->finalize(gobject);
}

void*
ags_thread_pool_creation_thread(void *ptr)
{
  AgsThreadPool *thread_pool;
  AgsThread *thread;
  AgsMutexManager *mutex_manager;
  
  GList *tmplist;
  GList *start_queue;

  guint n_threads, max_threads;
  guint i, i_stop;
  int err;
  gboolean timeout;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *parent_mutex;

  static struct timespec timedwait = {
    0,
    4000,
  };
  
  thread_pool = AGS_THREAD_POOL(ptr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  parent_mutex = ags_mutex_manager_lookup(mutex_manager,
					  thread_pool->parent);
  
  pthread_mutex_unlock(application_mutex);
  
#ifdef AGS_DEBUG
  g_message("ags_thread_pool_creation_thread\0");
#endif
  
  while((AGS_THREAD_POOL_RUNNING & (g_atomic_int_get(&(thread_pool->flags)))) != 0){
#ifdef AGS_DEBUG
    g_message("ags_thread_pool_creation_thread@loopStart\0");
#endif
    
    pthread_mutex_lock(thread_pool->creation_mutex);
    
    g_atomic_int_or(&(thread_pool->flags),
		    AGS_THREAD_POOL_READY);

    timeout = FALSE;
    
    while(g_atomic_int_get(&(thread_pool->newly_pulled)) == 0 &&
	  !timeout){
      err = pthread_cond_timedwait(thread_pool->creation_cond,
				   thread_pool->creation_mutex,
				   &timedwait);

      if(err == ETIMEDOUT){
	timeout = TRUE;
      }
    }

    g_atomic_int_and(&(thread_pool->flags),
		     (~AGS_THREAD_POOL_READY));

    /* clean pool */
    if(timeout){
      AgsReturnableThread *returnable_thread;
      
      GList *running_thread, *running_thread_next;
      GList *tmp_list;
      
      running_thread = g_atomic_pointer_get(&(thread_pool->running_thread));

      while(running_thread != NULL){
	running_thread_next = running_thread->next;
	returnable_thread = running_thread->data;

	if(returnable_thread != NULL &&
	   AGS_IS_RETURNABLE_THREAD(returnable_thread) &&
	   (AGS_RETURNABLE_THREAD_IN_USE & (g_atomic_int_get(&(returnable_thread->flags)))) == 0){
	  /* remove from thread-pool */
	  tmp_list = g_atomic_pointer_get(&(thread_pool->running_thread));
	  g_atomic_pointer_set(&(thread_pool->running_thread),
			       g_list_remove(tmp_list,
					     returnable_thread));

	  g_atomic_int_dec_and_test(&(thread_pool->n_threads));
	  
	  /* disconnect and remove from thread tree */
	  //	  ags_returnable_thread_disconnect(returnable_thread);
	  //	  ags_returnable_thread_disconnect_safe_run(returnable_thread);

	  /* unref */
	  g_object_unref(returnable_thread);
	}
	
	running_thread = running_thread_next;
      }
    }
        
    n_threads = g_atomic_int_get(&(thread_pool->n_threads));
    max_threads = g_atomic_int_get(&(thread_pool->max_threads));

    i_stop = g_atomic_int_get(&(thread_pool->newly_pulled));
    g_atomic_int_set(&(thread_pool->newly_pulled),
		     0);

    if(n_threads < AGS_THREAD_POOL_DEFAULT_MAX_UNUSED_THREADS){
      i_stop = (AGS_THREAD_POOL_DEFAULT_MAX_UNUSED_THREADS - n_threads) + i_stop;
    }
    
#ifdef AGS_DEBUG
    g_message("ags_thread_pool_creation_thread@loop0\0");
#endif

    start_queue = NULL;    
    
    if(n_threads < max_threads){
      for(i = 0; i < i_stop && n_threads < max_threads; i++){
	thread = (AgsThread *) ags_returnable_thread_new((GObject *) thread_pool);
	g_object_ref(thread);
	g_object_ref(thread);
	
	tmplist = g_atomic_pointer_get(&(thread_pool->returnable_thread));
	g_atomic_pointer_set(&(thread_pool->returnable_thread),
			     g_list_prepend(tmplist, thread));
	ags_thread_add_child_extended(AGS_THREAD(thread_pool->parent),
				      thread,
				      FALSE, FALSE);
	
	ags_connectable_connect(AGS_CONNECTABLE(thread));
	g_atomic_int_inc(&(thread_pool->n_threads));

	start_queue = g_list_prepend(start_queue,
				     thread);

	n_threads++;
      }
    }

    pthread_mutex_unlock(thread_pool->creation_mutex);

    pthread_mutex_lock(parent_mutex);
    
    if(start_queue != NULL){
      if(g_atomic_pointer_get(&(thread_pool->parent->start_queue)) != NULL){
	g_atomic_pointer_set(&(thread_pool->parent->start_queue),
			     g_list_concat(start_queue,
					   g_atomic_pointer_get(&(thread_pool->parent->start_queue))));
      }else{
	g_atomic_pointer_set(&(thread_pool->parent->start_queue),
			     start_queue);
      }
    }

    pthread_mutex_unlock(parent_mutex);

    pthread_mutex_lock(thread_pool->return_mutex);
    
    /* wake-up thread pool queue */      
    if(g_atomic_int_get(&(thread_pool->queued)) > 0){
      pthread_cond_signal(&(thread_pool->return_cond));
    }

    pthread_mutex_unlock(thread_pool->return_mutex);
    
#ifdef AGS_DEBUG
    g_message("ags_thread_pool_creation_thread@loopEND\0");
#endif
  }
}

/**
 * ags_thread_pool_pull:
 * @thread_pool: the #AgsThreadPool
 *
 * Pull a previously instantiated #AgsReturnableThread. Note this
 * function may block until a new thread is available.
 *
 * Returns: a new #AgsThread
 *
 * Since: 0.4
 */    
AgsThread*
ags_thread_pool_pull(AgsThreadPool *thread_pool)
{
  AgsReturnableThread *returnable_thread;
  GList *list, *tmplist;
  guint max_threads, n_threads;
  
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  auto void ags_thread_pool_pull_running();

  void ags_thread_pool_pull_running(){
    g_atomic_int_inc(&(thread_pool->newly_pulled));
      
    do{
      pthread_mutex_lock(thread_pool->creation_mutex);

      if((AGS_THREAD_POOL_READY & (g_atomic_int_get(&(thread_pool->flags)))) != 0 &&
	 g_atomic_int_get(&(thread_pool->newly_pulled)) != 0){
	pthread_cond_signal(thread_pool->creation_cond);
      }
      
      list = g_atomic_pointer_get(&(thread_pool->returnable_thread));

      while(list != NULL){
	returnable_thread = AGS_RETURNABLE_THREAD(list->data);

	if((AGS_RETURNABLE_THREAD_IN_USE & (g_atomic_int_get(&(returnable_thread->flags)))) == 0){
	  tmplist = g_atomic_pointer_get(&(thread_pool->returnable_thread));
	  g_atomic_pointer_set(&(thread_pool->returnable_thread),
			       g_list_remove(tmplist,
					     returnable_thread));
	  
	  tmplist = g_atomic_pointer_get(&(thread_pool->running_thread));
	  g_atomic_pointer_set(&(thread_pool->running_thread),
			       g_list_prepend(tmplist,
					      returnable_thread));

	  break;
	}

	list = list->next;
      }

      pthread_mutex_unlock(thread_pool->creation_mutex);
	
    }while(list == NULL);
  }

  returnable_thread = NULL;

  pthread_mutex_lock(thread_pool->pull_mutex);

  max_threads = g_atomic_int_get(&(thread_pool->max_threads));

  if((n_threads = g_atomic_int_get(&(thread_pool->n_threads))) < max_threads){
    ags_thread_pool_pull_running();
  }else{
    pthread_mutex_lock(thread_pool->return_mutex);

    g_atomic_int_inc(&(thread_pool->queued));

    while((n_threads = g_atomic_int_get(&(thread_pool->n_threads))) > max_threads){
      g_message("n_threads >= max_threads\0");
      
      pthread_cond_wait(thread_pool->return_cond,
			thread_pool->return_mutex);
    }

    g_atomic_int_dec_and_test(&(thread_pool->queued));

    pthread_mutex_unlock(thread_pool->return_mutex);

    ags_thread_pool_pull_running(); 
  }

  pthread_mutex_unlock(thread_pool->pull_mutex);

  return(AGS_THREAD(returnable_thread));
}

void
ags_thread_pool_real_start(AgsThreadPool *thread_pool)
{
  AgsMutexManager *mutex_manager;

  GList *list;
  GList *start_queue;
  
  gint n_threads;
  gint i;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *parent_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  parent_mutex = ags_mutex_manager_lookup(mutex_manager,
					  thread_pool->parent);
  
  pthread_mutex_unlock(application_mutex);

  g_atomic_int_or(&(thread_pool->flags),
		  AGS_THREAD_POOL_RUNNING);

  pthread_create(thread_pool->thread, NULL,
		 &(ags_thread_pool_creation_thread), thread_pool);

  list = g_atomic_pointer_get(&(thread_pool->returnable_thread));
  
  start_queue = NULL;

  while(list != NULL){
    ags_thread_add_child_extended(AGS_THREAD(thread_pool->parent),
				  AGS_THREAD(list->data),
				  FALSE, FALSE);
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    //    ags_thread_start(AGS_THREAD(list->data));
    start_queue = g_list_prepend(start_queue,
				 list->data);

    list = list->next;
  }

  pthread_mutex_lock(parent_mutex);

  if(start_queue != NULL){
    if(g_atomic_pointer_get(&(thread_pool->parent->start_queue)) != NULL){
      g_atomic_pointer_set(&(thread_pool->parent->start_queue),
			   g_list_concat(start_queue,
					 g_atomic_pointer_get(&(thread_pool->parent->start_queue))));
    }else{
      g_atomic_pointer_set(&(thread_pool->parent->start_queue),
			   start_queue);
    }
  }

  pthread_mutex_unlock(parent_mutex);
}

/**
 * ags_thread_pool_start:
 * @thread_pool: the #AgsThreadPool
 *
 * Start the thread pool.
 *
 * Since: 0.4
 */
void
ags_thread_pool_start(AgsThreadPool *thread_pool)
{
  g_return_if_fail(AGS_IS_THREAD_POOL(thread_pool));

  g_object_ref(G_OBJECT(thread_pool));
  g_signal_emit(G_OBJECT(thread_pool),
		thread_pool_signals[START], 0);
  g_object_unref(G_OBJECT(thread_pool));
}

/**
 * ags_thread_pool_new:
 * @parent: the parent #AgsThread of returnable threads
 *
 * Create a new #AgsThreadPool.
 *
 * Returns: the new #AgsThreadPool
 *
 * Since: 0.4
 */
AgsThreadPool*
ags_thread_pool_new(AgsThread *parent)
{
  AgsThreadPool *thread_pool;

  thread_pool = (AgsThreadPool *) g_object_new(AGS_TYPE_THREAD_POOL,
					       NULL);

  thread_pool->parent = parent;

  return(thread_pool);
}
