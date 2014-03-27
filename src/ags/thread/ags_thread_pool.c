/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/thread/ags_thread_pool.h>

#include <ags-lib/object/ags_connectable.h>

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

void ags_thread_pool_check_stop(AgsThreadPool *thread_pool);
void* ags_thread_pool_creation_thread(void *ptr);

void ags_thread_pool_real_start(AgsThreadPool *thread_pool);

#define AGS_THREAD_POOL_DEFAULT_MAX_UNUSED_THREADS 3
#define AGS_THREAD_POOL_DEFAULT_MAX_THREADS 8

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
				 3,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAX_UNUSED_THREADS,
				  param_spec);

  param_spec = g_param_spec_uint("max-threads\0",
				 "maximum threads to use\0",
				 "The maximum of threads to be created.\0",
				 1, 65535,
				 8,
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

  g_atomic_int_set(&thread_pool->flags,
		   0);

  g_atomic_int_set(&(thread_pool->max_unused_threads),
		   AGS_THREAD_POOL_DEFAULT_MAX_UNUSED_THREADS);
  g_atomic_int_set(&(thread_pool->max_threads),
		   AGS_THREAD_POOL_DEFAULT_MAX_THREADS);

  list = NULL;

  for(i = 0; i < g_atomic_int_get(&(thread_pool->max_unused_threads)); i++){
    thread = (AgsThread *) ags_returnable_thread_new(thread_pool);
    list = g_list_prepend(list, thread);
  }

  g_atomic_int_set(&(thread_pool->newly_pulled),
		   0);
  g_atomic_int_set(&(thread_pool->queued),
		   0);

  thread_pool->returnable_thread = list;
  thread_pool->running_thread = NULL;
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
      g_atomic_int_set(&thread_pool->max_threads,
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
  guint n_threads;
  guint i;

  thread_pool = AGS_THREAD_POOL(connectable);

  list = thread_pool->returnable_thread;
  n_threads = g_list_length(thread_pool->returnable_thread);
}

void
ags_thread_pool_finalize(GObject *gobject)
{
  AgsThreadPool *thread_pool;

  thread_pool = AGS_THREAD_POOL(gobject);

  ags_list_free_and_unref_link(thread_pool->returnable_thread);

  /* call parent */
  G_OBJECT_CLASS(ags_thread_pool_parent_class)->finalize(gobject);
}

void
ags_thread_pool_check_stop(AgsThreadPool *thread_pool)
{
  AgsReturnableThread *returnable_thread;
  GList *list, *list_next;
  guint count;

  g_message("ags_thread_pool_check_stop\0");

  pthread_mutex_lock(&(thread_pool->creation_mutex));

  list = thread_pool->returnable_thread;
  count = 0;

  while(list != NULL){
    list_next = list->next;

    returnable_thread = AGS_RETURNABLE_THREAD(list->data);

    if((AGS_RETURNABLE_THREAD_IN_USE & (g_atomic_int_get(&(returnable_thread->flags)))) != 0){
      count++;
    }

    if(count >= g_atomic_int_get(&(thread_pool->max_unused_threads))){
      thread_pool->returnable_thread = g_list_remove(thread_pool->returnable_thread,
						     returnable_thread);

      if((AGS_RETURNABLE_THREAD_IN_USE & (g_atomic_int_get(&(returnable_thread->flags)))) == 0){
	g_atomic_int_and(&(AGS_THREAD(returnable_thread)->flags),
			 (~AGS_THREAD_RUNNING));
	count--;
      }
    }

    list = list_next;
  }

  pthread_mutex_unlock(&(thread_pool->creation_mutex));
}

void*
ags_thread_pool_creation_thread(void *ptr)
{
  AgsThreadPool *thread_pool;
  AgsThread *thread;
  guint n_threads;
  guint i, i_stop;
  
  thread_pool = AGS_THREAD_POOL(ptr);

  while((AGS_THREAD_POOL_RUNNING & (g_atomic_int_get(&(thread_pool->flags)))) != 0){
    g_message("ags_thread_pool_creation_thread\0");
    pthread_mutex_lock(&(thread_pool->creation_mutex));

    g_atomic_int_or(&(thread_pool->flags),
		    AGS_THREAD_POOL_READY);

    while(g_atomic_int_get(&(thread_pool->newly_pulled)) == 0){
      pthread_cond_wait(&(thread_pool->creation_cond),
			&(thread_pool->creation_mutex));
    }

    n_threads = g_list_length(thread_pool->returnable_thread);

    i_stop = g_atomic_int_get(&(thread_pool->newly_pulled));
    g_atomic_int_set(&thread_pool->newly_pulled,
		     0);

    g_atomic_int_and(&(thread_pool->flags),
		     (~AGS_THREAD_POOL_READY));

    pthread_mutex_unlock(&(thread_pool->creation_mutex));

    for(i = 0; i < i_stop &&
	  g_list_length(thread_pool->returnable_thread) < g_atomic_int_get(&(thread_pool->max_threads));
	i++){
      thread = (AgsThread *) ags_returnable_thread_new(thread_pool);
      thread_pool->returnable_thread = g_list_prepend(thread_pool->returnable_thread, thread);

      ags_thread_add_child(AGS_THREAD(thread_pool->main_loop),
			   thread);
      ags_connectable_connect(AGS_CONNECTABLE(thread));

      n_threads++;
    }
  }
}

AgsThread*
ags_thread_pool_pull(AgsThreadPool *thread_pool)
{
  AgsReturnableThread *returnable_thread;
  GList *list;
  guint max_threads, n_threads;

  auto void ags_thread_pool_pull_running();

  void ags_thread_pool_pull_running(){
    list = thread_pool->returnable_thread;

    while(list != NULL){
      returnable_thread = AGS_RETURNABLE_THREAD(list->data);

      if((AGS_RETURNABLE_THREAD_IN_USE & (g_atomic_int_get(&(returnable_thread->flags)))) == 0){
	g_atomic_int_or(&(returnable_thread->flags),
			AGS_RETURNABLE_THREAD_IN_USE);
	thread_pool->running_thread = g_list_prepend(thread_pool->running_thread,
						     returnable_thread);

	break;
      }

      list = list->next;
    }

    //NOTE: shall not return null rather block thread until thread is available
    //    if(list == NULL){
    //      returnable_thread = NULL;
    //    }

    if(n_threads + 1 < max_threads){
      g_atomic_int_inc(&thread_pool->newly_pulled);

      pthread_mutex_lock(&(thread_pool->creation_mutex));
      
      if((AGS_THREAD_POOL_READY & (g_atomic_int_get(&(thread_pool->flags)))) != 0){
	pthread_cond_signal(&(thread_pool->creation_cond));
      }

      pthread_mutex_unlock(&(thread_pool->creation_mutex));
    }
  }

  g_message("ags_thread_pool_pull\0");
  returnable_thread = NULL;

  pthread_mutex_lock(&(thread_pool->return_mutex));

  max_threads = g_atomic_int_get(&thread_pool->max_threads);

  if((n_threads = g_list_length(thread_pool->running_thread)) < max_threads){
    ags_thread_pool_pull_running();
  }else{
    g_atomic_int_inc(&(thread_pool->queued));

    while((n_threads = g_list_length(thread_pool->running_thread)) >= max_threads){
      g_message("n_threads = g_list_length(thread_pool->running_thread)) >= max_threads\0");
      pthread_cond_wait(&(thread_pool->return_cond),
			&(thread_pool->return_mutex));
    }

    g_atomic_int_dec_and_test(&thread_pool->queued);
    ags_thread_pool_pull_running();
  }

  g_atomic_int_or(&(returnable_thread->flags),
		  AGS_RETURNABLE_THREAD_IN_USE);

  pthread_mutex_unlock(&(thread_pool->return_mutex));

  return(AGS_THREAD(returnable_thread));
}

void
ags_thread_pool_real_start(AgsThreadPool *thread_pool)
{
  GList *list;
  gint n_threads;
  gint i;

  g_atomic_int_or(&(thread_pool->flags),
		  AGS_THREAD_POOL_RUNNING);

  pthread_create(&(thread_pool->thread), NULL,
		 &(ags_thread_pool_creation_thread), thread_pool);

  list = thread_pool->returnable_thread;

  while(list != NULL){
    ags_thread_add_child(AGS_THREAD(thread_pool->main_loop),
			 AGS_THREAD(list->data));
    ags_thread_start(AGS_THREAD(list->data));

    list = list->next;
  }
}

void
ags_thread_pool_start(AgsThreadPool *thread_pool)
{
  g_return_if_fail(AGS_IS_THREAD_POOL(thread_pool));

  g_object_ref(G_OBJECT(thread_pool));
  g_signal_emit(G_OBJECT(thread_pool),
		thread_pool_signals[START], 0);
  g_object_unref(G_OBJECT(thread_pool));
}

AgsThreadPool*
ags_thread_pool_new(AgsMainLoop *main_loop)
{
  AgsThreadPool *thread_pool;

  thread_pool = (AgsThreadPool *) g_object_new(AGS_TYPE_THREAD_POOL,
					       NULL);

  thread_pool->main_loop = main_loop;

  return(thread_pool);
}
