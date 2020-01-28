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

#include <ags/thread/ags_thread_pool.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_priority.h>

#include <ags/thread/ags_returnable_thread.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_thread_pool_class_init(AgsThreadPoolClass *thread_pool);
void ags_thread_pool_init(AgsThreadPool *thread_pool);
void ags_thread_pool_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_thread_pool_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_thread_pool = 0;

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

    ags_type_thread_pool = g_type_register_static(G_TYPE_OBJECT,
						  "AgsThreadPool",
						  &ags_thread_pool_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_thread_pool);
  }

  return g_define_type_id__volatile;
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
  /**
   * AgsThreadPool:max-unused-threads:
   *
   * The maximum amount of unused threads available.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("max-unused-threads",
				 i18n_pspec("maximum unused threads"),
				 i18n_pspec("The maximum of unused threads"),
				 1, 65535,
				 AGS_THREAD_POOL_DEFAULT_MAX_UNUSED_THREADS,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAX_UNUSED_THREADS,
				  param_spec);

  /**
   * AgsThreadPool:max-threads:
   *
   * The maximum amount of threads available.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("max-threads",
				 i18n_pspec("maximum threads to use"),
				 i18n_pspec("The maximum of threads to be created"),
				 1, 65535,
				 AGS_THREAD_POOL_DEFAULT_MAX_THREADS,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAX_THREADS,
				  param_spec);

  /* AgsThreadPoolClass */
  thread_pool->start = ags_thread_pool_real_start;

  /* signals */
  /**
   * AgsThreadPool::start:
   * @thread_pool: the #AgsThreadPool
   *
   * The ::start() signal is invoked in order to started the pool.
   * 
   * Since: 3.0.0
   */
  thread_pool_signals[START] =
    g_signal_new("start",
		 G_TYPE_FROM_CLASS (thread_pool),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadPoolClass, start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_thread_pool_init(AgsThreadPool *thread_pool)
{
  AgsThread *thread;

  AgsConfig *config;

  GList *list;

  gchar *str;
  
  guint i;

  config = ags_config_get_instance();

  g_atomic_int_set(&(thread_pool->flags),
		   0);

  /* max unused threads */
  str = ags_config_get_value(config,
			     AGS_CONFIG_THREAD,
			     "thread-pool-max-unused-threads");
  
  if(str != NULL){
    g_atomic_int_set(&(thread_pool->max_unused_threads),
		     g_ascii_strtoull(str,
				      NULL,
				      10));
  }else{
    g_atomic_int_set(&(thread_pool->max_unused_threads),
		     AGS_THREAD_POOL_DEFAULT_MAX_UNUSED_THREADS);
  }

  /* max threads */
  str = ags_config_get_value(config,
			     AGS_CONFIG_THREAD,
			     "thread-pool-max-threads");
  
  if(str != NULL){
    g_atomic_int_set(&(thread_pool->max_threads),
		     g_ascii_strtoull(str,
				      NULL,
				      10));
  }else{  
    g_atomic_int_set(&(thread_pool->max_threads),
		     AGS_THREAD_POOL_DEFAULT_MAX_THREADS);
  }
  
  thread_pool->thread = NULL;

  /* creation mutex and condition */
  g_atomic_int_set(&(thread_pool->queued),
		   0);
  
  g_mutex_init(&(thread_pool->creation_mutex));

  g_cond_init(&(thread_pool->creation_cond));

  /* idle mutex and condition */
  g_atomic_int_set(&(thread_pool->create_threads),
		   FALSE);
  g_atomic_int_set(&(thread_pool->idle),
		   FALSE);

  g_mutex_init(&(thread_pool->idle_mutex));
  
  g_cond_init(&(thread_pool->idle_cond));

  /* returnable thread */
  thread_pool->parent = NULL;
  list = NULL;

  for(i = 0; i < g_atomic_int_get(&(thread_pool->max_unused_threads)); i++){
    thread = (AgsThread *) ags_returnable_thread_new((GObject *) thread_pool);
    g_object_ref(thread);
    
    list = g_list_prepend(list, thread);
  }

  g_atomic_pointer_set(&(thread_pool->returnable_thread),
		       list);
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
ags_thread_pool_finalize(GObject *gobject)
{
  AgsThreadPool *thread_pool;

  thread_pool = AGS_THREAD_POOL(gobject);

  /* returnable thread */
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

  GList *start_queue;
  
  guint n_threads;
  guint i, i_stop;

  GRecMutex *parent_mutex;

  thread_pool = AGS_THREAD_POOL(ptr);

  /* get parent mutex */
  if(thread_pool->parent != NULL){
    parent_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread_pool->parent);
  }else{
    parent_mutex = NULL;
  }

  /* real-time setup */
#ifdef AGS_WITH_RT
  if((AGS_THREAD_POOL_RT_SETUP & (g_atomic_int_get(&(thread_pool->flags)))) == 0){
    AgsPriority *priority;
    
    struct sched_param param;

    gchar *str;

    priority = ags_priority_get_instance();  
    
    /* Declare ourself as a real time task */
    param.sched_priority = 45;

    str = ags_priority_get_value(priority,
				 AGS_PRIORITY_RT_THREAD,
				 AGS_PRIORITY_KEY_LIBAGS);

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
    
    g_atomic_int_or(&(thread_pool->flags),
		    AGS_THREAD_POOL_RT_SETUP);
  }
#endif
  
#ifdef AGS_DEBUG
  g_message("ags_thread_pool_creation_thread");
#endif
  
  while((AGS_THREAD_POOL_RUNNING & (g_atomic_int_get(&(thread_pool->flags)))) != 0){
#ifdef AGS_DEBUG
    g_message("ags_thread_pool_creation_thread@loopStart");
#endif
    
    g_mutex_lock(&(thread_pool->idle_mutex));

    if(!g_atomic_int_get(&(thread_pool->create_threads))){
      g_atomic_int_set(&(thread_pool->idle),
		       TRUE);
      
      while(g_atomic_int_get(&(thread_pool->idle)) &&
	    !g_atomic_int_get(&(thread_pool->create_threads))){
	g_cond_wait(&(thread_pool->idle_cond),
		    &(thread_pool->idle_mutex));
      }

    }
    
    g_atomic_int_set(&(thread_pool->idle),
		     FALSE);
    g_atomic_int_set(&(thread_pool->create_threads),
		     FALSE);
    
    g_mutex_unlock(&(thread_pool->idle_mutex));

    g_mutex_lock(&(thread_pool->creation_mutex));

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

#if 0
      ags_thread_start(returnable_thread);

      /* wait returnable_thread */
      g_mutex_lock(&(returnable_thread->start_mutex));

      ags_thread_set_status_flags(returnable_thread, AGS_THREAD_STATUS_START_WAIT);
	
      if(ags_thread_test_status_flags(returnable_thread, AGS_THREAD_STATUS_START_WAIT) &&
	 !ags_thread_test_status_flags(returnable_thread, AGS_THREAD_STATUS_START_DONE)){
	while(ags_thread_test_status_flags(returnable_thread, AGS_THREAD_STATUS_START_WAIT) &&
	      !ags_thread_test_status_flags(returnable_thread, AGS_THREAD_STATUS_START_DONE)){
	  g_cond_wait(&(returnable_thread->start_cond),
		      &(returnable_thread->start_mutex));
	}
      }

      g_mutex_unlock(&(returnable_thread->start_mutex));
#endif
      
      start_queue = g_list_prepend(start_queue,
				   returnable_thread);
      
      g_atomic_pointer_set(&(thread_pool->returnable_thread),
			   g_list_prepend(g_atomic_pointer_get(&(thread_pool->returnable_thread)),
					  returnable_thread));
    }

    if(g_atomic_int_get(&(thread_pool->queued)) != 0){
      g_cond_signal(&(thread_pool->creation_cond));
    }
    
    g_mutex_unlock(&(thread_pool->creation_mutex));

    if(thread_pool->parent != NULL){
      g_rec_mutex_lock(parent_mutex);

      ags_thread_add_start_queue_all(thread_pool->parent,
				     start_queue);

      g_rec_mutex_unlock(parent_mutex);
    }
    
#ifdef AGS_DEBUG
    g_message("ags_thread_pool_creation_thread@loopEND");
#endif
  }

  return(NULL);
}

/**
 * ags_thread_pool_pull:
 * @thread_pool: the #AgsThreadPool
 *
 * Pull a previously instantiated #AgsReturnableThread. Note this
 * function may block until a new thread is available.
 *
 * Returns: (transfer full): a new #AgsThread
 *
 * Since: 3.0.0
 */    
AgsThread*
ags_thread_pool_pull(AgsThreadPool *thread_pool)
{
  AgsThread *returnable_thread;

  GList *list;
  
  static GMutex mutex;

  if(!AGS_IS_THREAD_POOL(thread_pool)){
    return(NULL);
  }
  
  returnable_thread = NULL;

  g_mutex_lock(&mutex);

  /*  */
  g_mutex_lock(&(thread_pool->creation_mutex));

  if(g_atomic_pointer_get(&(thread_pool->returnable_thread)) == NULL){
    g_atomic_int_inc(&(thread_pool->queued));

    while(g_atomic_int_get(&(thread_pool->queued)) != 0 &&
	  g_atomic_pointer_get(&(thread_pool->returnable_thread)) == NULL){
      g_cond_wait(&(thread_pool->creation_cond),
		  &(thread_pool->creation_mutex));
    }
  }

  list = g_list_last(g_atomic_pointer_get(&(thread_pool->returnable_thread)));
  returnable_thread = list->data;
  g_atomic_pointer_set(&(thread_pool->returnable_thread),
		       g_list_remove(g_atomic_pointer_get(&(thread_pool->returnable_thread)),
							  returnable_thread));

  g_atomic_int_dec_and_test(&(thread_pool->queued));
  g_mutex_unlock(&(thread_pool->creation_mutex));

  g_mutex_unlock(&mutex);
  
  /* signal create threads */
  if(g_list_length(g_atomic_pointer_get(&(thread_pool->returnable_thread))) < g_atomic_int_get(&(thread_pool->max_unused_threads)) / 2){
    g_mutex_lock(&(thread_pool->idle_mutex));

    g_atomic_int_set(&(thread_pool->create_threads),
		     TRUE);
  
    if(g_atomic_int_get(&(thread_pool->idle))){
      g_cond_signal(&(thread_pool->idle_cond));
    }
  
    g_mutex_unlock(&(thread_pool->idle_mutex));
  }
  
  return(AGS_THREAD(returnable_thread));
}

void
ags_thread_pool_real_start(AgsThreadPool *thread_pool)
{
  GList *list;
  GList *start_queue;
  
  gint n_threads;
  gint i;

  GRecMutex *parent_mutex;

  /* get parent mutex */
  if(thread_pool->parent != NULL){
    parent_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread_pool->parent);
  }else{
    parent_mutex = NULL;
  }

  /* set running */
  g_atomic_int_or(&(thread_pool->flags),
		  AGS_THREAD_POOL_RUNNING);

  thread_pool->thread = g_thread_new("Advanced Gtk+ Sequencer - thread pool",
				     ags_thread_pool_creation_thread,
				     thread_pool);

  list = g_atomic_pointer_get(&(thread_pool->returnable_thread));
  
  start_queue = NULL;

  while(list != NULL){
    if(thread_pool->parent != NULL){
      ags_thread_add_child_extended(AGS_THREAD(thread_pool->parent),
				    AGS_THREAD(list->data),
				    TRUE, TRUE);
    }else{
      ags_thread_start(list->data);
    }
    
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    //    ags_thread_start(AGS_THREAD(list->data));
    start_queue = g_list_prepend(start_queue,
				 list->data);

    list = list->next;
  }

  if(parent_mutex != NULL){
    g_rec_mutex_lock(parent_mutex);

    ags_thread_add_start_queue_all(thread_pool->parent,
				   start_queue);

    g_rec_mutex_unlock(parent_mutex);
  }
}

/**
 * ags_thread_pool_start:
 * @thread_pool: the #AgsThreadPool
 *
 * Start the thread pool.
 *
 * Since: 3.0.0
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
 * Since: 3.0.0
 */
AgsThreadPool*
ags_thread_pool_new(AgsThread *parent)
{
  AgsThreadPool *thread_pool;

  thread_pool = (AgsThreadPool *) g_object_new(AGS_TYPE_THREAD_POOL,
					       NULL);

  thread_pool->parent = parent;

  if(parent != NULL){
    g_object_ref(parent);
  }
  
  return(thread_pool);
}
