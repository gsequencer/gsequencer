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

#include <ags/plugin/ags_lv2_worker.h>

#include <ags/object/ags_connectable.h>

void ags_lv2_worker_class_init(AgsLv2WorkerClass *lv2_worker);
void ags_lv2_worker_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_lv2_worker_init(AgsLv2Worker *lv2_worker);
void ags_lv2_worker_connect(AgsConnectable *connectable);
void ags_lv2_worker_disconnect(AgsConnectable *connectable);
void ags_lv2_worker_finalize(GObject *gobject);

/**
 * SECTION:ags_lv2_worker
 * @short_description: task thread
 * @title: AgsLv2Worker
 * @section_id:
 * @include: ags/plugin/ags_lv2_worker.h
 *
 * The #AgsLv2Worker acts as task queue thread.
 */

static gpointer ags_lv2_worker_parent_class = NULL;
static AgsConnectableInterface *ags_lv2_worker_parent_connectable_interface;

GType
ags_lv2_worker_get_type()
{
  static GType ags_type_lv2_worker = 0;

  if(!ags_type_lv2_worker){
    static const GTypeInfo ags_lv2_worker_info = {
      sizeof (AgsLv2WorkerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_worker_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2Worker),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_worker_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_lv2_worker_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_lv2_worker = g_type_register_static(G_TYPE_OBJECT,
						 "AgsLv2Worker\0",
						 &ags_lv2_worker_info,
						 0);
    
    g_type_add_interface_static(ags_type_lv2_worker,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_lv2_worker);
}

void
ags_lv2_worker_class_init(AgsLv2WorkerClass *lv2_worker)
{
  GObjectClass *gobject;

  ags_lv2_worker_parent_class = g_type_class_peek_parent(lv2_worker);

  /* GObject */
  gobject = (GObjectClass *) lv2_worker;

  gobject->finalize = ags_lv2_worker_finalize;
}

void
ags_lv2_worker_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_lv2_worker_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_lv2_worker_connect;
  connectable->disconnect = ags_lv2_worker_disconnect;
}

void
ags_lv2_worker_init(AgsLv2Worker *lv2_worker)
{
  g_atomic_int_set(&(lv2_worker->flags),
		   0);

  lv2_worker->handle = NULL;
  lv2_worker->worker_interface = NULL;

  lv2_worker->work_size = 0;
  lv2_worker->work_data = NULL;

  lv2_worker->response_data = NULL;
  
  lv2_worker->returnable_thread = NULL;
}

void
ags_lv2_worker_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_lv2_worker_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_lv2_worker_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_lv2_worker_parent_class)->finalize(gobject);
}

void
ags_lv2_worker_safe_run(AgsReturnableThread *returnable_thread, gpointer data)
{
  AgsLv2Worker *lv2_worker;
  GList *response_data;

  lv2_worker = g_atomic_pointer_get(&(returnable_thread->safe_data));

  if((AGS_LV2_WORKER_RUN & (g_atomic_int_get(&(lv2_worker->flags)))) != 0){
    /* work */
    lv2_worker->worker_interface->work(lv2_worker->handle,
				       ags_lv2_worker_respond,
				       lv2_worker,
				       lv2_worker->work_size,
				       lv2_worker->work_data);
    
    lv2_worker->work_size = 0;
    lv2_worker->work_data = NULL;
    
    /* response data */
    response_data = lv2_worker->response_data;
  
    while(response_data != NULL){
      lv2_worker->worker_interface->work_response(lv2_worker->handle,
						  AGS_LV2_WORKER_RESPONSE_DATA(response_data->data)->size,
						  AGS_LV2_WORKER_RESPONSE_DATA(response_data->data)->data);

      response_data = response_data->next;
    }

    g_list_free_full(lv2_worker->response_data,
		     (GDestroyNotify) ags_lv2_worker_free_response_data);
    lv2_worker->response_data = NULL;
  
    lv2_worker->worker_interface->end_run(lv2_worker->handle);

    /* reset */
    g_atomic_int_and(&(lv2_worker->flags),
		     (~AGS_LV2_WORKER_RUN));
  }
}

void
ags_lv2_worker_interrupted_callback(AgsThread *thread,
				    int sig,
				    guint time_cycle, guint *time_spent,
				    AgsLv2Worker *lv2_worker)
{
  if((AGS_THREAD_INTERRUPTED & (g_atomic_int_get(&(thread->sync_flags)))) == 0){
    g_atomic_int_or(&(thread->sync_flags),
    		    AGS_THREAD_INTERRUPTED);
    
    if((AGS_LV2_WORKER_RUN & (g_atomic_int_get(&(lv2_worker->flags)))) != 0){
#ifdef AGS_PTHREAD_SUSPEND
    pthread_suspend(thread->thread);
#else
    pthread_kill(*(thread->thread), AGS_THREAD_SUSPEND_SIG);
#endif
    }
  }
}

AgsLv2WorkerResponseData*
ags_lv2_worker_alloc_response_data()
{
  AgsLv2WorkerResponseData *response_data;

  response_data = (AgsLv2WorkerResponseData *) malloc(sizeof(AgsLv2WorkerResponseData));

  response_data->size = 0;
  response_data->data = NULL;

  return(response_data);
}

void
ags_lv2_worker_free_response_data(AgsLv2WorkerResponseData *response_data)
{
  free(response_data);
}

LV2_Worker_Status
ags_lv2_worker_respond(LV2_Worker_Respond_Handle handle,
		       uint32_t size,
		       const void* data)
{
  AgsLv2Worker *lv2_worker;
  AgsLv2WorkerResponseData *response_data;

  lv2_worker = (AgsLv2Worker *) handle;
  
  response_data = ags_lv2_worker_alloc_response_data();

  response_data->size = size;
  response_data->data = data;

  lv2_worker->response_data = g_list_prepend(lv2_worker->response_data,
					     response_data);
  
  return(LV2_WORKER_SUCCESS);
}

LV2_Worker_Status
ags_lv2_worker_schedule_work(LV2_Worker_Schedule_Handle handle,
			     uint32_t size,
			     const void* data)
{
  AgsLv2Worker *lv2_worker;

  lv2_worker = (AgsLv2Worker *) handle;

  lv2_worker->work_size = size;
  lv2_worker->work_data = data;

  g_atomic_int_or(&(lv2_worker->flags),
		  AGS_LV2_WORKER_RUN);

  return(LV2_WORKER_SUCCESS);
}

/**
 * ags_lv2_worker_new:
 * @returnable_thread: an #AgsReturnableThread
 *
 * Create a new #AgsLv2Worker.
 *
 * Returns: the new #AgsLv2Worker
 *
 * Since: 0.7.7
 */ 
AgsLv2Worker*
ags_lv2_worker_new(AgsThread *returnable_thread)
{
  AgsLv2Worker *lv2_worker;

  lv2_worker = (AgsLv2Worker *) g_object_new(AGS_TYPE_LV2_WORKER,
					     NULL);
  lv2_worker->returnable_thread = returnable_thread;

  return(lv2_worker);
}
