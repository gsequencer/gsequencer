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

void ags_thread_pool_class_init(AgsThreadPoolClass *thread_pool);
void ags_thread_pool_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_thread_pool_init(AgsThreadPool *thread_pool);
void ags_thread_pool_connect(AgsConnectable *connectable);
void ags_thread_pool_disconnect(AgsConnectable *connectable);
void ags_thread_pool_finalize(GObject *gobject);

void* ags_thread_pool_creation_thread(void *ptr);

void ags_thread_pool_stop_callback(AgsThread *thread,
				   AgsThreadPool *thread_pool);

static gpointer ags_thread_pool_parent_class = NULL;

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

  ags_thread_pool_parent_class = g_type_class_peek_parent(thread_pool);

  /* GObjectClass */
  gobject = (GObjectClass *) thread_pool;

  gobject->finalize = ags_thread_pool_finalize;
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

  thread_pool->max_unused_threads = 2;
  thread_pool->max_threads = 4;

  list = NULL;

  for(i = 0; i < thread_pool->max_unused_threads; i++){
    thread = (AgsThread *) ags_returnable_thread_new();
    ags_thread_start(thread);
    ags_thread_suspend(thread);
    list = g_list_prepend(list, thread);
  }

  g_atomic_int_set(&thread_pool->newly_pulled,
		   0);
  g_atomic_int_set(&thread_pool->queued,
		   0);

  thread_pool->returnable_thread = list;
  thread_pool->running_thread = NULL;
}

void
ags_thread_pool_connect(AgsConnectable *connectable)
{
  AgsThreadPool *thread_pool;
  GList *list;

  thread_pool = AGS_THREAD_POOL(connectable);

  list = thread_pool->returnable_thread;

  //TODO:JK: implement me
}

void
ags_thread_pool_disconnect(AgsConnectable *connectable)
{
  AgsThreadPool *thread_pool;
  GList *list;

  thread_pool = AGS_THREAD_POOL(connectable);

  list = thread_pool->returnable_thread;

  //TODO:JK: implement me
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

void*
ags_thread_pool_creation_thread(void *ptr)
{
  AgsThreadPool *thread_pool;
  AgsThread *thread;
  guint i, i_stop;
  
  thread_pool = AGS_THREAD_POOL(ptr);

  while((AGS_THREAD_POOL_RUNNING & (g_atomic_int_get(&thread_pool->flags))) != 0){
    pthread_mutex_lock(&(thread_pool->creation_mutex));

    while(g_atomic_int_get(&thread_pool->newly_pulled) == 0){
      pthread_cond_wait(&(thread_pool->creation_cond),
			&(thread_pool->creation_mutex));
    }

    i_stop = g_atomic_int_get(&thread_pool->newly_pulled);
    g_atomic_int_set(&thread_pool->newly_pulled,
		     0);

    pthread_mutex_unlock(&(thread_pool->creation_mutex));

    for(i = 0; i < i_stop && g_list_length(thread_pool->returnable_thread) < thread_pool->max_threads; i++){
      thread = (AgsThread *) ags_returnable_thread_new();
      ags_thread_start(thread);
      ags_thread_suspend(thread);
      thread_pool->returnable_thread = g_list_prepend(thread_pool->returnable_thread, thread);
    }
  }
}

AgsThread*
ags_thread_pool_pull(AgsThreadPool *thread_pool)
{
  AgsReturnableThread *returnable_thread;
  GList *list;
  guint n_threads;

  auto void ags_thread_pool_pull_running();

  void ags_thread_pool_pull_running(){
    list = thread_pool->returnable_thread;

    while(list != NULL){
      returnable_thread = AGS_RETURNABLE_THREAD(list->data);

      if((AGS_RETURNABLE_THREAD_IN_USE & (g_atomic_int_get(&returnable_thread->flags))) == 0){
	g_atomic_int_or(&returnable_thread->flags,
			AGS_RETURNABLE_THREAD_IN_USE);
	thread_pool->running_thread = g_list_prepend(thread_pool->running_thread,
						     returnable_thread);

	break;
      }

      list = list->next;
    }

    if(n_threads + 1 < thread_pool->max_threads){
      g_atomic_int_inc(&thread_pool->newly_pulled);

      pthread_mutex_lock(&(thread_pool->creation_mutex));
      
      pthread_cond_signal(&(thread_pool->creation_cond));

      pthread_mutex_unlock(&(thread_pool->creation_mutex));
    }
  }

  pthread_mutex_lock(&(thread_pool->return_mutex));

  if((n_threads = g_list_length(thread_pool->running_thread)) < thread_pool->max_threads){
    ags_thread_pool_pull_running();
  }else{
    g_atomic_int_inc(&thread_pool->queued);

    while((n_threads = g_list_length(thread_pool->running_thread)) == thread_pool->max_threads){
      pthread_cond_wait(&(thread_pool->return_cond),
			&(thread_pool->return_mutex));
    }

    g_atomic_int_dec_and_test(&thread_pool->queued);
    ags_thread_pool_pull_running();
  }

  g_atomic_int_or(&returnable_thread->flags,
		  AGS_RETURNABLE_THREAD_IN_USE);

  pthread_mutex_unlock(&(thread_pool->return_mutex));

  return(AGS_THREAD(returnable_thread));
}

void
ags_thread_pool_stop_callback(AgsThread *thread,
			      AgsThreadPool *thread_pool)
{
  pthread_mutex_lock(&(thread_pool->return_mutex));

  thread_pool->running_thread = g_list_remove(thread_pool->running_thread,
					      thread);
  g_atomic_int_and(&(AGS_RETURNABLE_THREAD(thread)->flags),
		   (~AGS_RETURNABLE_THREAD_IN_USE));

  if(g_atomic_int_get(&thread_pool->queued) > 0){
    pthread_cond_signal(&(thread_pool->return_cond));
  }

  pthread_mutex_unlock(&(thread_pool->return_mutex));
}

AgsThreadPool*
ags_thread_pool_new()
{
  AgsThreadPool *thread_pool;

  thread_pool = (AgsThreadPool *) g_object_new(AGS_TYPE_THREAD_POOL,
					       NULL);

  return(thread_pool);
}
