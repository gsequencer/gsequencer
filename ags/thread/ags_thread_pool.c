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

#define AGS_THREAD_POOL_DEFAULT_MAX_UNUSED_THREADS 12
#define AGS_THREAD_POOL_DEFAULT_MAX_THREADS 1024

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

  pthread_mutexattr_t *attr;

  g_atomic_int_set(&(thread_pool->flags),
		   0);

  g_atomic_int_set(&(thread_pool->max_unused_threads),
		   AGS_THREAD_POOL_DEFAULT_MAX_UNUSED_THREADS);
  g_atomic_int_set(&(thread_pool->max_threads),
		   AGS_THREAD_POOL_DEFAULT_MAX_THREADS);

  thread_pool->thread = (pthread_t *) malloc(sizeof(pthread_t));

  //FIXME:JK: memory leak
  attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));

  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  thread_pool->creation_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(thread_pool->creation_mutex, attr);

  thread_pool->creation_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(thread_pool->creation_cond, NULL);

  g_atomic_int_set(&(thread_pool->queued),
		   0);

  thread_pool->parent = NULL;
  list = NULL;

  for(i = 0; i < g_atomic_int_get(&(thread_pool->max_unused_threads)); i++){
    thread = (AgsThread *) ags_returnable_thread_new((GObject *) thread_pool);
    g_object_ref(thread);
    
    list = g_list_prepend(list, thread);
  }

  g_atomic_pointer_set(&(thread_pool->returnable_thread),
		       list);

  //FIXME:JK: memory leak
  attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));

  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  thread_pool->idle_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(thread_pool->idle_mutex, attr);
  
  thread_pool->idle_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(thread_pool->idle_cond, NULL);
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
  AgsThread *returnable_thread;
  AgsMutexManager *mutex_manager;

  GList *start_queue;
  
  guint n_threads;
  guint i, i_stop;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *parent_mutex;

  thread_pool = AGS_THREAD_POOL(ptr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  parent_mutex = ags_mutex_manager_lookup(mutex_manager,
					  (GObject *) thread_pool->parent);
  
  pthread_mutex_unlock(application_mutex);

  /* real-time setup */
  if((AGS_THREAD_POOL_RT_SETUP & (g_atomic_int_get(&(thread_pool->flags)))) == 0){
    struct sched_param param;
    
    /* Declare ourself as a real time task */
    param.sched_priority = AGS_THREAD_POOL_RT_PRIORITY;
      
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
      perror("sched_setscheduler failed\0");
    }

    g_atomic_int_or(&(thread_pool->flags),
		    AGS_THREAD_POOL_RT_SETUP);
  }

  
#ifdef AGS_DEBUG
  g_message("ags_thread_pool_creation_thread\0");
#endif
  
  while((AGS_THREAD_POOL_RUNNING & (g_atomic_int_get(&(thread_pool->flags)))) != 0){
#ifdef AGS_DEBUG
    g_message("ags_thread_pool_creation_thread@loopStart\0");
#endif
    
    pthread_mutex_lock(thread_pool->idle_mutex);

    if(!g_atomic_int_get(&(thread_pool->create_threads))){
      g_atomic_int_set(&(thread_pool->idle),
		       TRUE);
      
      while(g_atomic_int_get(&(thread_pool->idle)) &&
	    !g_atomic_int_get(&(thread_pool->create_threads))){
	pthread_cond_wait(thread_pool->idle_cond,
			  thread_pool->idle_mutex);
      }

    }
    
    g_atomic_int_set(&(thread_pool->idle),
		     FALSE);
    g_atomic_int_set(&(thread_pool->create_threads),
		     FALSE);
    
    pthread_mutex_unlock(thread_pool->idle_mutex);

    pthread_mutex_lock(thread_pool->creation_mutex);

    n_threads = g_list_length(g_atomic_pointer_get(&(thread_pool->returnable_thread)));
    
    if(n_threads < g_atomic_int_get(&(thread_pool->max_threads)) &&
       n_threads < g_atomic_int_get(&(thread_pool->max_unused_threads))){
      i_stop = g_atomic_int_get(&(thread_pool->max_unused_threads)) - n_threads;
    }else{
      i_stop = 0;
    }

    start_queue = NULL;
    
    for(i = 0; i < i_stop; i++){
      guint val;

      returnable_thread = (AgsThread *) ags_returnable_thread_new((GObject *) thread_pool);
      g_object_ref(returnable_thread);
      ags_thread_add_child_extended(thread_pool->parent,
				    returnable_thread,
				    TRUE, TRUE);
      ags_connectable_connect(AGS_CONNECTABLE(returnable_thread));

      ags_thread_start(returnable_thread);

      /* wait returnable_thread */
      pthread_mutex_lock(returnable_thread->start_mutex);

      g_atomic_int_set(&(returnable_thread->start_wait),
		       TRUE);
	
      if(g_atomic_int_get(&(returnable_thread->start_wait)) == TRUE &&
	 g_atomic_int_get(&(returnable_thread->start_done)) == FALSE){
	while(g_atomic_int_get(&(returnable_thread->start_wait)) == TRUE &&
	      g_atomic_int_get(&(returnable_thread->start_done)) == FALSE){
	  pthread_cond_wait(returnable_thread->start_cond,
			    returnable_thread->start_mutex);
	}
      }

      pthread_mutex_unlock(returnable_thread->start_mutex);

      //      start_queue = g_list_prepend(start_queue,
      //			   returnable_thread);
      
      g_atomic_pointer_set(&(thread_pool->returnable_thread),
			   g_list_prepend(g_atomic_pointer_get(&(thread_pool->returnable_thread)),
					  returnable_thread));
    }

    if(g_atomic_int_get(&(thread_pool->queued)) != 0){
      pthread_cond_signal(thread_pool->creation_cond);      
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
  AgsThread *returnable_thread;

  GList *list;
  
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  
  returnable_thread = NULL;

  pthread_mutex_lock(&mutex);

  /*  */
  pthread_mutex_lock(thread_pool->creation_mutex);

  if(g_atomic_pointer_get(&(thread_pool->returnable_thread)) == NULL){
    g_atomic_int_inc(&(thread_pool->queued));

    while(g_atomic_int_get(&(thread_pool->queued)) != 0 &&
	  g_atomic_pointer_get(&(thread_pool->returnable_thread)) == NULL){
      pthread_cond_wait(thread_pool->creation_cond,
			thread_pool->creation_mutex);
    }
  }

  list = g_list_last(g_atomic_pointer_get(&(thread_pool->returnable_thread)));
  returnable_thread = list->data;
  g_atomic_pointer_set(&(thread_pool->returnable_thread),
		       g_list_remove(g_atomic_pointer_get(&(thread_pool->returnable_thread)),
							  returnable_thread));

  g_atomic_int_dec_and_test(&(thread_pool->queued));
  pthread_mutex_unlock(thread_pool->creation_mutex);

  pthread_mutex_unlock(&mutex);
  
  /* signal create threads */
  pthread_mutex_lock(thread_pool->idle_mutex);

  g_atomic_int_set(&(thread_pool->create_threads),
		   TRUE);
  
  if(g_atomic_int_get(&(thread_pool->idle))){
    pthread_cond_signal(thread_pool->idle_cond);
  }
  
  pthread_mutex_unlock(thread_pool->idle_mutex);

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
					  (GObject *) thread_pool->parent);
  
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
				  TRUE, TRUE);
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
