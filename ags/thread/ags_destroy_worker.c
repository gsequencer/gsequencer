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

#include <ags/thread/ags_destroy_worker.h>

#include <ags/object/ags_connectable.h>

#include <stdlib.h>

void ags_destroy_worker_class_init(AgsDestroyWorkerClass *destroy_worker);
void ags_destroy_worker_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_destroy_worker_init(AgsDestroyWorker *destroy_worker);
void ags_destroy_worker_connect(AgsConnectable *connectable);
void ags_destroy_worker_disconnect(AgsConnectable *connectable);
void ags_destroy_worker_finalize(GObject *gobject);

void ags_destroy_worker_start(AgsThread *thread);
void ags_destroy_worker_stop(AgsThread *thread);

void ags_destroy_worker_do_poll(AgsWorkerThread *worker_thread);

/**
 * SECTION:ags_destroy_worker
 * @short_description: destroy worker
 * @title: AgsDestroyWorker
 * @section_id:
 * @include: ags/worker/ags_destroy_worker.h
 *
 * The #AgsDestroyWorker does non-realtime work. You might want
 * give it the responsibility to destroy your objects.
 */

static gpointer ags_destroy_worker_parent_class = NULL;

GType
ags_destroy_worker_get_type()
{
  static GType ags_type_destroy_worker = 0;

  if(!ags_type_destroy_worker){
    static const GTypeInfo ags_destroy_worker_info = {
      sizeof (AgsDestroyWorkerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_destroy_worker_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsDestroyWorker),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_destroy_worker_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_destroy_worker_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_destroy_worker = g_type_register_static(AGS_TYPE_WORKER_THREAD,
						     "AgsDestroyWorker",
						     &ags_destroy_worker_info,
						     0);
    
    g_type_add_interface_static(ags_type_destroy_worker,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_destroy_worker);
}

void
ags_destroy_worker_class_init(AgsDestroyWorkerClass *destroy_worker)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  AgsWorkerThreadClass *worker_thread;

  ags_destroy_worker_parent_class = g_type_class_peek_parent(destroy_worker);

  /* GObject */
  gobject = (GObjectClass *) destroy_worker;

  gobject->finalize = ags_destroy_worker_finalize;

  /* AgsThread */
  thread = (AgsThreadClass *) destroy_worker;

  thread->start = ags_destroy_worker_start;
  thread->run = NULL;
  thread->stop = ags_destroy_worker_stop;

  /* AgsDestroyWorker */
  worker_thread = (AgsWorkerThreadClass *) destroy_worker;

  worker_thread->do_poll = ags_destroy_worker_do_poll;
}

void
ags_destroy_worker_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_destroy_worker_connect;
  connectable->disconnect = ags_destroy_worker_disconnect;
}

void
ags_destroy_worker_init(AgsDestroyWorker *destroy_worker)
{
  destroy_worker->destroy_interval = (struct timespec *) malloc(sizeof(struct timespec));

  destroy_worker->destroy_interval->tv_sec = 1;
  destroy_worker->destroy_interval->tv_nsec = 0;
  
  /* lock destroy list */
  destroy_worker->destroy_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));

  pthread_mutexattr_init(destroy_worker->destroy_mutexattr);
  pthread_mutexattr_settype(destroy_worker->destroy_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

  destroy_worker->destroy_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(destroy_worker->destroy_mutex, destroy_worker->destroy_mutexattr);

  /* destroy list */
  destroy_worker->destroy_list = NULL;
}

void
ags_destroy_worker_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_destroy_worker_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_destroy_worker_finalize(GObject *gobject)
{
  AgsDestroyWorker *destroy_worker;

  destroy_worker = AGS_DESTROY_WORKER(gobject);

  if(destroy_worker->destroy_interval != NULL){
    free(destroy_worker->destroy_interval);
  }
  
  /* destroy mutex */
  pthread_mutex_destroy(destroy_worker->destroy_mutex);
  free(destroy_worker->destroy_mutex);

  /* call parent */
  G_OBJECT_CLASS(ags_destroy_worker_parent_class)->finalize(gobject);
}

void
ags_destroy_worker_start(AgsThread *thread)
{
  AgsWorkerThread *worker_thread;

  worker_thread = AGS_WORKER_THREAD(thread);

  g_atomic_int_or(&(worker_thread->flags),
		  AGS_WORKER_THREAD_RUNNING);

  pthread_create(worker_thread->worker_thread, worker_thread->worker_thread_attr,
		 ags_woker_thread_do_poll_loop, worker_thread);
}

void
ags_destroy_worker_stop(AgsThread *thread)
{
  AgsWorkerThread *worker_thread;

  worker_thread = AGS_WORKER_THREAD(thread);

  g_atomic_int_and(&(worker_thread->flags),
		   (~(AGS_WORKER_THREAD_RUNNING)));
}

void
ags_destroy_worker_do_poll(AgsWorkerThread *worker_thread)
{
  AgsDestroyWorker *destroy_worker;

  GList *list, *list_start;
  
  destroy_worker = AGS_DESTROY_WORKER(worker_thread);

  pthread_mutex_lock(destroy_worker->destroy_mutex);

  list_start =
    list = destroy_worker->destroy_list;
  destroy_worker->destroy_list = NULL;
  
  pthread_mutex_unlock(destroy_worker->destroy_mutex);

  while(list != NULL){
    AGS_DESTROY_ENTRY(list->data)->destroy_func(AGS_DESTROY_ENTRY(list->data)->ptr);

    list = list->next;
  }

  g_list_free(list_start);

  nanosleep(destroy_worker->destroy_interval,
	    NULL);
}

/**
 * ags_destroy_entry_alloc:
 * @ptr: a pointer
 * @destroy_func: the @ptr's destroy function
 * 
 * Allocated a destroy entry.
 * 
 * Returns: the allocated #AgsDestroyEntry
 * 
 * Since: 0.7.122.8
 */
AgsDestroyEntry*
ags_destroy_entry_alloc(gpointer ptr, AgsDestroyFunc destroy_func)
{
  AgsDestroyEntry *destroy_entry;

  destroy_entry = (AgsDestroyEntry *) malloc(sizeof(AgsDestroyEntry));

  destroy_entry->ptr = ptr;
  destroy_entry->destroy_func = destroy_func;
}

/**
 * ags_destroy_worker_add:
 * @destroy_worker:
 * @ptr:
 * @destroy_func:
 * 
 * Add @ptr for destruction using @destroy_func.
 * 
 * Since: 0.7.122.8
 */
void
ags_destroy_worker_add(AgsDestroyWorker *destroy_worker,
		       gpointer ptr, AgsDestroyFunc destroy_func)
{
  AgsDestroyEntry *destroy_entry;

  if(destroy_worker == NULL ||
     ptr == NULL ||
     destroy_func == NULL){
    return;
  }

  destroy_entry = ags_destroy_entry_alloc(ptr, destroy_func);
  
  pthread_mutex_lock(destroy_worker->destroy_mutex);

  destroy_worker->destroy_list = g_list_prepend(destroy_worker->destroy_list,
						destroy_entry);
  
  pthread_mutex_unlock(destroy_worker->destroy_mutex);
}

/**
 * ags_destroy_worker_new:
 *
 * Create a new #AgsDestroyWorker.
 *
 * Returns: the new #AgsDestroyWorker
 *
 * Since: 0.7.122.8
 */
AgsDestroyWorker*
ags_destroy_worker_new()
{
  AgsDestroyWorker *destroy_worker;
  
  destroy_worker = (AgsDestroyWorker *) g_object_new(AGS_TYPE_DESTROY_WORKER,
						     NULL);

  return(destroy_worker);
}
