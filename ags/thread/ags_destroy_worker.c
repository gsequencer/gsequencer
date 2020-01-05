/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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
void ags_destroy_worker_init(AgsDestroyWorker *destroy_worker);
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

AgsDestroyWorker *ags_destroy_worker = NULL;

GType
ags_destroy_worker_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_destroy_worker = 0;

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

    ags_type_destroy_worker = g_type_register_static(AGS_TYPE_WORKER_THREAD,
						     "AgsDestroyWorker",
						     &ags_destroy_worker_info,
						     0);    

    g_once_init_leave(&g_define_type_id__volatile, ags_type_destroy_worker);
  }

  return g_define_type_id__volatile;
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
ags_destroy_worker_init(AgsDestroyWorker *destroy_worker)
{
  destroy_worker->destroy_interval = (struct timespec *) malloc(sizeof(struct timespec));

  destroy_worker->destroy_interval->tv_sec = 1;
  destroy_worker->destroy_interval->tv_nsec = 0;
  
  /* lock destroy list */
  g_rec_mutex_init(&(destroy_worker->destroy_mutex));

  /* destroy list */
  destroy_worker->destroy_list = NULL;
}

void
ags_destroy_worker_finalize(GObject *gobject)
{
  AgsDestroyWorker *destroy_worker;

  destroy_worker = AGS_DESTROY_WORKER(gobject);

  if(destroy_worker->destroy_interval != NULL){
    free(destroy_worker->destroy_interval);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_destroy_worker_parent_class)->finalize(gobject);
}

void
ags_destroy_worker_start(AgsThread *thread)
{
  AgsWorkerThread *worker_thread;

  worker_thread = AGS_WORKER_THREAD(thread);

  ags_worker_thread_set_status_flags(worker_thread, AGS_WORKER_THREAD_STATUS_RUNNING);

  worker_thread->worker_thread = g_thread_new("Advanced Gtk+ Sequencer - destroy worker",
					      ags_woker_thread_do_poll_loop,
					      worker_thread);
}

void
ags_destroy_worker_stop(AgsThread *thread)
{
  AgsWorkerThread *worker_thread;

  worker_thread = AGS_WORKER_THREAD(thread);

  ags_worker_thread_set_status_flags(worker_thread, AGS_WORKER_THREAD_STATUS_RUNNING);
}

void
ags_destroy_worker_do_poll(AgsWorkerThread *worker_thread)
{
  AgsDestroyWorker *destroy_worker;

  GList *list, *list_start;
  
  destroy_worker = AGS_DESTROY_WORKER(worker_thread);

  g_rec_mutex_lock(&(destroy_worker->destroy_mutex));

  list_start =
    list = destroy_worker->destroy_list;
  destroy_worker->destroy_list = NULL;
  
  g_rec_mutex_unlock(&(destroy_worker->destroy_mutex));

  while(list != NULL){
    AGS_DESTROY_ENTRY(list->data)->destroy_func(AGS_DESTROY_ENTRY(list->data)->ptr);

    list = list->next;
  }

  g_list_free_full(list_start,
		   g_free);

  nanosleep(destroy_worker->destroy_interval,
	    NULL);
}

/**
 * ags_destroy_entry_alloc:
 * @ptr: a pointer
 * @destroy_func: (scope call): the @ptr's destroy function
 * 
 * Allocated a destroy entry.
 * 
 * Returns: (type gpointer) (transfer none): the allocated #AgsDestroyEntry
 * 
 * Since: 3.0.0
 */
AgsDestroyEntry*
ags_destroy_entry_alloc(gpointer ptr, AgsDestroyFunc destroy_func)
{
  AgsDestroyEntry *destroy_entry;

  destroy_entry = (AgsDestroyEntry *) malloc(sizeof(AgsDestroyEntry));

  destroy_entry->ptr = ptr;
  destroy_entry->destroy_func = destroy_func;

  return(destroy_entry);
}

/**
 * ags_destroy_worker_add:
 * @destroy_worker: (type gpointer): the #AgsDestroyWorker
 * @ptr: the gpointer to destroy
 * @destroy_func: (scope call): the AgsDestroyFunc
 * 
 * Add @ptr for destruction using @destroy_func.
 * 
 * Since: 3.0.0
 */
void
ags_destroy_worker_add(AgsDestroyWorker *destroy_worker,
		       gpointer ptr, AgsDestroyFunc destroy_func)
{
  AgsDestroyEntry *destroy_entry;

  if(!AGS_IS_DESTROY_WORKER(destroy_worker) ||
     ptr == NULL ||
     destroy_func == NULL){
    return;
  }

  destroy_entry = ags_destroy_entry_alloc(ptr, destroy_func);
  
  g_rec_mutex_lock(&(destroy_worker->destroy_mutex));

  destroy_worker->destroy_list = g_list_prepend(destroy_worker->destroy_list,
						destroy_entry);
  
  g_rec_mutex_unlock(&(destroy_worker->destroy_mutex));
}

/**
 * ags_destroy_worker_get_instance:
 * 
 * Get your destroy worker instance.
 *
 * Returns: (transfer none): the #AgsDestroyWorker instance
 *
 * Since: 3.0.0
 */
AgsDestroyWorker*
ags_destroy_worker_get_instance()
{
  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(ags_destroy_worker == NULL){
    ags_destroy_worker = ags_destroy_worker_new();
  }

  g_mutex_unlock(&mutex);
  
  return(ags_destroy_worker);
}

/**
 * ags_destroy_worker_new:
 *
 * Create a new #AgsDestroyWorker.
 *
 * Returns: the new #AgsDestroyWorker
 *
 * Since: 3.0.0
 */
AgsDestroyWorker*
ags_destroy_worker_new()
{
  AgsDestroyWorker *destroy_worker;
  
  destroy_worker = (AgsDestroyWorker *) g_object_new(AGS_TYPE_DESTROY_WORKER,
						     NULL);

  return(destroy_worker);
}
