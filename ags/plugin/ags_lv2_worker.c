/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/libags.h>

#include <ags/i18n.h>

void ags_lv2_worker_class_init(AgsLv2WorkerClass *lv2_worker);
void ags_lv2_worker_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_lv2_worker_init(AgsLv2Worker *lv2_worker);
void ags_lv2_worker_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_lv2_worker_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_lv2_worker_dispose(GObject *gobject);
void ags_lv2_worker_finalize(GObject *gobject);

/**
 * SECTION:ags_lv2_worker
 * @short_description: LV2 worker thread
 * @title: AgsLv2Worker
 * @section_id:
 * @include: ags/plugin/ags_lv2_worker.h
 *
 * The #AgsLv2Worker acts as task queue thread.
 */

enum{
  PROP_0,
  PROP_HANDLE,
  PROP_WORK_SIZE,
  PROP_WORK_DATA,
  PROP_RESPONSE_DATA,
  PROP_RETURNABLE_THREAD,
  PROP_WORKER_THREAD,
};

static gpointer ags_lv2_worker_parent_class = NULL;
static AgsConnectableInterface *ags_lv2_worker_parent_connectable_interface;

static pthread_mutex_t ags_lv2_worker_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_lv2_worker_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_lv2_worker = 0;

    static const GTypeInfo ags_lv2_worker_info = {
      sizeof(AgsLv2WorkerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_worker_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLv2Worker),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_worker_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_lv2_worker_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_lv2_worker = g_type_register_static(G_TYPE_OBJECT,
						 "AgsLv2Worker",
						 &ags_lv2_worker_info,
						 0);
    
    g_type_add_interface_static(ags_type_lv2_worker,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_lv2_worker);
  }

  return g_define_type_id__volatile;
}

void
ags_lv2_worker_class_init(AgsLv2WorkerClass *lv2_worker)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_lv2_worker_parent_class = g_type_class_peek_parent(lv2_worker);

  /* GObject */
  gobject = (GObjectClass *) lv2_worker;

  gobject->set_property = ags_lv2_worker_set_property;
  gobject->get_property = ags_lv2_worker_get_property;

  gobject->dispose = ags_lv2_worker_dispose;
  gobject->finalize = ags_lv2_worker_finalize;

  /* properties */
  /**
   * AgsLv2Worker:handle:
   *
   * The assigned  #LV2_Handle-struct
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("handle",
				    i18n_pspec("handle"),
				    i18n_pspec("The handle"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_HANDLE,
				  param_spec);

  /**
   * AgsLv2Worker:work-size:
   *
   * The work size.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("work-size",
				 i18n_pspec("work size"),
				 i18n_pspec("The work size"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WORK_SIZE,
				  param_spec);

  /**
   * AgsLv2Worker:work-data:
   *
   * The assigned work data
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("work-data",
				    i18n_pspec("work data"),
				    i18n_pspec("The work data"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WORK_DATA,
				  param_spec);

  /**
   * AgsLv2Worker:response-data:
   *
   * The assigned #GList-struct containing #AgsLv2WorkerResponseData-struct
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("response-data",
				    i18n_pspec("response data"),
				    i18n_pspec("The response data"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RESPONSE_DATA,
				  param_spec);

  /**
   * AgsLv2Worker:returnable-thread:
   *
   * The assigned returnable-thread.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("returnable-thread",
				   i18n_pspec("returnable thread"),
				   i18n_pspec("The returnable thread"),
				   AGS_TYPE_RETURNABLE_THREAD,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RETURNABLE_THREAD,
				  param_spec);

  /**
   * AgsLv2Worker:worker-thread:
   *
   * The assigned worker-thread.
   * 
   * Since: 2.2.0
   */
  param_spec = g_param_spec_object("worker-thread",
				   i18n_pspec("worker thread"),
				   i18n_pspec("The worker thread"),
				   AGS_TYPE_WORKER_THREAD,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WORKER_THREAD,
				  param_spec);
}

void
ags_lv2_worker_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_lv2_worker_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_lv2_worker_init(AgsLv2Worker *lv2_worker)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  lv2_worker->flags = 0;

  /* add base plugin mutex */
  lv2_worker->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  lv2_worker->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /* fields */
  lv2_worker->handle = NULL;
  lv2_worker->worker_interface = NULL;

  lv2_worker->work_size = 0;
  lv2_worker->work_data = NULL;

  lv2_worker->response_data = NULL;
  
  lv2_worker->returnable_thread = NULL;
  lv2_worker->worker_thread = NULL;
}

void
ags_lv2_worker_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsLv2Worker *lv2_worker;

  pthread_mutex_t *lv2_worker_mutex;

  lv2_worker = AGS_LV2_WORKER(gobject);

  /* get base plugin mutex */
  pthread_mutex_lock(ags_lv2_worker_get_class_mutex());
  
  lv2_worker_mutex = lv2_worker->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_worker_get_class_mutex());

  switch(prop_id){
  case PROP_HANDLE:
    {
      pthread_mutex_lock(lv2_worker_mutex);

      lv2_worker->handle = g_value_get_pointer(value);

      pthread_mutex_unlock(lv2_worker_mutex);
    }
    break;
  case PROP_WORK_SIZE:
    {
      pthread_mutex_lock(lv2_worker_mutex);

      lv2_worker->work_size = g_value_get_uint(value);

      pthread_mutex_unlock(lv2_worker_mutex);
    }
    break;
  case PROP_WORK_DATA:
    {
      pthread_mutex_lock(lv2_worker_mutex);

      lv2_worker->work_data = g_value_get_pointer(value);

      pthread_mutex_unlock(lv2_worker_mutex);
    }
    break;
  case PROP_RESPONSE_DATA:
    {
      gpointer response_data;

      response_data = g_value_get_pointer(value);
      
      pthread_mutex_lock(lv2_worker_mutex);

      if(response_data == NULL ||
	 g_list_find(lv2_worker->response_data, response_data) != NULL){
	pthread_mutex_unlock(lv2_worker_mutex);

	return;
      }

      lv2_worker->response_data = g_list_prepend(lv2_worker->response_data,
						 response_data);

      pthread_mutex_unlock(lv2_worker_mutex);
    }
    break;
  case PROP_RETURNABLE_THREAD:
    {
      AgsReturnableThread *returnable_thread;

      returnable_thread = g_value_get_object(value);
      
      pthread_mutex_lock(lv2_worker_mutex);

      if(lv2_worker->returnable_thread == returnable_thread){
	pthread_mutex_unlock(lv2_worker_mutex);

	return;
      }

      if(lv2_worker->returnable_thread != NULL){
	g_object_unref(lv2_worker->returnable_thread);
      }

      if(returnable_thread != NULL){
	g_object_ref(returnable_thread);
      }

      lv2_worker->returnable_thread = returnable_thread;
      
      pthread_mutex_unlock(lv2_worker_mutex);
    }
    break;
  case PROP_WORKER_THREAD:
    {
      AgsWorkerThread *worker_thread;

      worker_thread = g_value_get_object(value);
      
      pthread_mutex_lock(lv2_worker_mutex);

      if(lv2_worker->worker_thread == worker_thread){
	pthread_mutex_unlock(lv2_worker_mutex);

	return;
      }

      if(lv2_worker->worker_thread != NULL){
	g_object_unref(lv2_worker->worker_thread);
      }

      if(worker_thread != NULL){
	g_object_ref(worker_thread);
      }

      lv2_worker->worker_thread = worker_thread;
      
      pthread_mutex_unlock(lv2_worker_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}
    
void
ags_lv2_worker_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsLv2Worker *lv2_worker;

  pthread_mutex_t *lv2_worker_mutex;

  lv2_worker = AGS_LV2_WORKER(gobject);

  /* get base plugin mutex */
  pthread_mutex_lock(ags_lv2_worker_get_class_mutex());
  
  lv2_worker_mutex = lv2_worker->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_worker_get_class_mutex());

  switch(prop_id){
  case PROP_HANDLE:
    {
      pthread_mutex_lock(lv2_worker_mutex);
      
      g_value_set_pointer(value, lv2_worker->handle);

      pthread_mutex_unlock(lv2_worker_mutex);
    }
    break;
  case PROP_WORK_SIZE:
    {
      pthread_mutex_lock(lv2_worker_mutex);
      
      g_value_set_uint(value, lv2_worker->work_size);

      pthread_mutex_unlock(lv2_worker_mutex);
    }
    break;
  case PROP_WORK_DATA:
    {
      pthread_mutex_lock(lv2_worker_mutex);
      
      g_value_set_pointer(value, lv2_worker->work_data);

      pthread_mutex_unlock(lv2_worker_mutex);
    }
    break;
  case PROP_RESPONSE_DATA:
    {
      pthread_mutex_lock(lv2_worker_mutex);
      
      g_value_set_pointer(value, g_list_copy_deep(lv2_worker->response_data,
						  (GCopyFunc) g_object_ref,
						  NULL));

      pthread_mutex_unlock(lv2_worker_mutex);
    }
    break;
  case PROP_RETURNABLE_THREAD:
    {
      pthread_mutex_lock(lv2_worker_mutex);
      
      g_value_set_object(value, lv2_worker->returnable_thread);

      pthread_mutex_unlock(lv2_worker_mutex);
    }
    break;
  case PROP_WORKER_THREAD:
    {
      pthread_mutex_lock(lv2_worker_mutex);
      
      g_value_set_object(value, lv2_worker->worker_thread);

      pthread_mutex_unlock(lv2_worker_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_worker_dispose(GObject *gobject)
{
  AgsLv2Worker *lv2_worker;

  lv2_worker = AGS_LV2_WORKER(gobject);

  if(lv2_worker->returnable_thread != NULL){
    g_object_unref(lv2_worker->returnable_thread);

    lv2_worker->returnable_thread = NULL;
  }
  
  if(lv2_worker->worker_thread != NULL){
    g_object_unref(lv2_worker->worker_thread);

    lv2_worker->worker_thread = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_lv2_worker_parent_class)->dispose(gobject);
}

void
ags_lv2_worker_finalize(GObject *gobject)
{
  AgsLv2Worker *lv2_worker;

  lv2_worker = AGS_LV2_WORKER(gobject);
  
  /* destroy object mutex */
  pthread_mutex_destroy(lv2_worker->obj_mutex);
  free(lv2_worker->obj_mutex);

  pthread_mutexattr_destroy(lv2_worker->obj_mutexattr);
  free(lv2_worker->obj_mutexattr);

  g_list_free_full(lv2_worker->response_data,
		   (GDestroyNotify) ags_lv2_worker_free_response_data);
  
  if(lv2_worker->returnable_thread != NULL){
    g_object_unref(lv2_worker->returnable_thread);
  }
  
  if(lv2_worker->worker_thread != NULL){
    g_object_unref(lv2_worker->worker_thread);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_lv2_worker_parent_class)->finalize(gobject);
}

/**
 * ags_lv2_worker_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_lv2_worker_get_class_mutex()
{
  return(&ags_lv2_worker_class_mutex);
}

/**
 * ags_lv2_worker_test_flags:
 * @lv2_worker: the #AgsLv2Worker
 * @flags: the flags
 * 
 * Test @flags to be set on @recall.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 2.0.0
 */
gboolean
ags_lv2_worker_test_flags(AgsLv2Worker *lv2_worker, guint flags)
{
  gboolean retval;
  
  pthread_mutex_t *lv2_worker_mutex;

  if(!AGS_IS_LV2_WORKER(lv2_worker)){
    return(FALSE);
  }
  
  /* get lv2 worker mutex */
  pthread_mutex_lock(ags_lv2_worker_get_class_mutex());
  
  lv2_worker_mutex = lv2_worker->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_worker_get_class_mutex());

  /* test flags */
  pthread_mutex_lock(lv2_worker_mutex);

  retval = ((flags & (lv2_worker->flags)) != 0) ? TRUE: FALSE;
  
  pthread_mutex_unlock(lv2_worker_mutex);

  return(retval);
}

/**
 * ags_lv2_worker_set_flags:
 * @lv2_worker: the #AgsLv2Worker
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.0.0
 */
void
ags_lv2_worker_set_flags(AgsLv2Worker *lv2_worker, guint flags)
{
  pthread_mutex_t *lv2_worker_mutex;

  if(!AGS_IS_LV2_WORKER(lv2_worker)){
    return;
  }
  
  /* get lv2 worker mutex */
  pthread_mutex_lock(ags_lv2_worker_get_class_mutex());
  
  lv2_worker_mutex = lv2_worker->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_worker_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(lv2_worker_mutex);

  lv2_worker->flags |= flags;
  
  pthread_mutex_unlock(lv2_worker_mutex);
}

/**
 * ags_lv2_worker_unset_flags:
 * @lv2_worker: the #AgsLv2Worker
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.0.0
 */
void
ags_lv2_worker_unset_flags(AgsLv2Worker *lv2_worker, guint flags)
{
  pthread_mutex_t *lv2_worker_mutex;

  if(!AGS_IS_LV2_WORKER(lv2_worker)){
    return;
  }
  
  /* get lv2 worker mutex */
  pthread_mutex_lock(ags_lv2_worker_get_class_mutex());
  
  lv2_worker_mutex = lv2_worker->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_worker_get_class_mutex());

  /* unset flags */
  pthread_mutex_lock(lv2_worker_mutex);

  lv2_worker->flags &= (~flags);
  
  pthread_mutex_unlock(lv2_worker_mutex);
}

/**
 * ags_lv2_worker_alloc_response_data:
 * 
 * Allocate a new #AgsLv2WorkerResponseData-struct.
 * 
 * Returns: the new #AgsLv2WorkerResponseData-struct
 * 
 * Since: 2.0.0
 */
AgsLv2WorkerResponseData*
ags_lv2_worker_alloc_response_data()
{
  AgsLv2WorkerResponseData *response_data;

  response_data = (AgsLv2WorkerResponseData *) malloc(sizeof(AgsLv2WorkerResponseData));

  response_data->data_size = 0;
  response_data->data = NULL;

  return(response_data);
}

/**
 * ags_lv2_worker_free_response_data:
 * @response_data: the #AgsLv2WorkerResponseData-struct
 * 
 * Free @response_data.
 * 
 * Since: 2.0.0
 */
void
ags_lv2_worker_free_response_data(AgsLv2WorkerResponseData *response_data)
{ 
  g_free(response_data);
}

/**
 * ags_lv2_worker_respond:
 * @handle: the LV2_Worker_Respond_Handle
 * @data_size: the data size
 * @data: the data
 *
 * Respond lv2 worker @handle.
 * 
 * Returns: LV2_Worker_Status
 * 
 * Since: 2.0.0
 */
LV2_Worker_Status
ags_lv2_worker_respond(LV2_Worker_Respond_Handle handle,
		       uint32_t data_size,
		       const void* data)
{
  AgsLv2Worker *lv2_worker;
  AgsLv2WorkerResponseData *response_data;

  pthread_mutex_t *lv2_worker_mutex;

  lv2_worker = AGS_LV2_WORKER(handle);
  
  /* get lv2 worker mutex */
  pthread_mutex_lock(ags_lv2_worker_get_class_mutex());
  
  lv2_worker_mutex = lv2_worker->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_worker_get_class_mutex());

  /* allocate response data */
  response_data = ags_lv2_worker_alloc_response_data();

  response_data->data_size = data_size;
  response_data->data = data;

  pthread_mutex_lock(lv2_worker_mutex);

  lv2_worker->response_data = g_list_prepend(lv2_worker->response_data,
					     response_data);

  pthread_mutex_unlock(lv2_worker_mutex);
  
  return(LV2_WORKER_SUCCESS);
}

/**
 * ags_lv2_worker_schedule_work:
 * @handle: the LV2_Worker_Respond_Handle
 * @data_size: the data size
 * @data: the data
 *
 * Schedule work of lv2 worker @handle.
 * 
 * Returns: LV2_Worker_Status
 * 
 * Since: 2.0.0
 */
LV2_Worker_Status
ags_lv2_worker_schedule_work(LV2_Worker_Schedule_Handle handle,
			     uint32_t data_size,
			     const void* data)
{
  AgsLv2Worker *lv2_worker;

  pthread_mutex_t *lv2_worker_mutex;

  lv2_worker = AGS_LV2_WORKER(handle);
  
  /* get lv2 worker mutex */
  pthread_mutex_lock(ags_lv2_worker_get_class_mutex());
  
  lv2_worker_mutex = lv2_worker->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_worker_get_class_mutex());

  /* apply work data and size */
  pthread_mutex_lock(lv2_worker_mutex);

  lv2_worker->work_size = (guint) data_size;
  lv2_worker->work_data = data;

  pthread_mutex_unlock(lv2_worker_mutex);

  /* set flags */
  ags_lv2_worker_set_flags(lv2_worker, AGS_LV2_WORKER_RUN);

  return(LV2_WORKER_SUCCESS);
}

/**
 * ags_lv2_worker_do_poll:
 * @worker_thread: the #AgsWorkerThread
 * @data: the data
 * 
 * Safe run callback.
 * 
 * Since: 2.2.0
 */
void
ags_lv2_worker_do_poll(AgsWorkerThread *worker_thread, gpointer data)
{
  AgsLv2Worker *lv2_worker;

  LV2_Handle handle;
  
  GList *response_data_start, *response_data;

  guint work_size;
  gpointer work_data;

  LV2_Worker_Status (*work)(LV2_Handle instance,
			    LV2_Worker_Respond_Function respond,
			    LV2_Worker_Respond_Handle handle,
			    uint32_t data_size,
			    const void* data);
  LV2_Worker_Status (*work_response)(LV2_Handle instance,
				     uint32_t size,
				     const void* body);
  LV2_Worker_Status (*end_run)(LV2_Handle instance);
  
  pthread_mutex_t *lv2_worker_mutex;

  lv2_worker = data;

  /* get lv2 worker mutex */
  pthread_mutex_lock(ags_lv2_worker_get_class_mutex());
  
  lv2_worker_mutex = lv2_worker->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_worker_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(lv2_worker_mutex);

  handle = lv2_worker->handle;
  
  work_size = lv2_worker->work_size;
  work_data = lv2_worker->work_data;

  if(lv2_worker->worker_interface == NULL){
    pthread_mutex_unlock(lv2_worker_mutex);
    
    return;    
  }
  
  work = lv2_worker->worker_interface->work;
  work_response = lv2_worker->worker_interface->work_response;
  end_run = lv2_worker->worker_interface->end_run;
  
  pthread_mutex_unlock(lv2_worker_mutex);

  /* work */
  if(ags_lv2_worker_test_flags(lv2_worker, AGS_LV2_WORKER_RUN)){
    work(handle,
	 ags_lv2_worker_respond,
	 lv2_worker,
	 (uint32_t) (work_size),
	 work_data);
    
    pthread_mutex_lock(lv2_worker_mutex);

    lv2_worker->work_size = 0;
    lv2_worker->work_data = NULL;

    pthread_mutex_unlock(lv2_worker_mutex);
    
    /* response data */
    pthread_mutex_lock(lv2_worker_mutex);

    response_data =
      response_data_start = g_list_copy(lv2_worker->response_data);

    pthread_mutex_lock(lv2_worker_mutex);
  
    while(response_data != NULL){
      work_response(handle,
		    AGS_LV2_WORKER_RESPONSE_DATA(response_data->data)->data_size,
		    AGS_LV2_WORKER_RESPONSE_DATA(response_data->data)->data);
      
      response_data = response_data->next;
    }

    /* free response data */
    pthread_mutex_lock(lv2_worker_mutex);
    
    g_list_free_full(response_data_start,
		     (GDestroyNotify) ags_lv2_worker_free_response_data);
    
    g_list_free(lv2_worker->response_data);
    lv2_worker->response_data = NULL;
    
    pthread_mutex_unlock(lv2_worker_mutex);

    /* end run */
    end_run(handle);

    /* reset */
    ags_lv2_worker_unset_flags(lv2_worker, AGS_LV2_WORKER_RUN);
  }
}

/**
 * ags_lv2_worker_safe_run:
 * @returnable_thread: the #AgsReturnableThread
 * @data: the data
 * 
 * Safe run callback.
 * 
 * Since: 2.0.0
 */
void
ags_lv2_worker_safe_run(AgsReturnableThread *returnable_thread, gpointer data)
{
  AgsLv2Worker *lv2_worker;

  LV2_Handle handle;
  
  GList *response_data_start, *response_data;

  guint work_size;
  gpointer work_data;

  LV2_Worker_Status (*work)(LV2_Handle instance,
			    LV2_Worker_Respond_Function respond,
			    LV2_Worker_Respond_Handle handle,
			    uint32_t data_size,
			    const void* data);
  LV2_Worker_Status (*work_response)(LV2_Handle instance,
				     uint32_t size,
				     const void* body);
  LV2_Worker_Status (*end_run)(LV2_Handle instance);
  
  pthread_mutex_t *lv2_worker_mutex;

  lv2_worker = g_atomic_pointer_get(&(returnable_thread->safe_data));

  /* get lv2 worker mutex */
  pthread_mutex_lock(ags_lv2_worker_get_class_mutex());
  
  lv2_worker_mutex = lv2_worker->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_worker_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(lv2_worker_mutex);

  handle = lv2_worker->handle;
  
  work_size = lv2_worker->work_size;
  work_data = lv2_worker->work_data;

  if(lv2_worker->worker_interface == NULL){
    return;    
  }
  
  work = lv2_worker->worker_interface->work;
  work_response = lv2_worker->worker_interface->work_response;
  end_run = lv2_worker->worker_interface->end_run;
  
  pthread_mutex_unlock(lv2_worker_mutex);

  /* work */
  if(ags_lv2_worker_test_flags(lv2_worker, AGS_LV2_WORKER_RUN)){
    work(handle,
	 ags_lv2_worker_respond,
	 lv2_worker,
	 (uint32_t) (work_size),
	 work_data);
    
    pthread_mutex_lock(lv2_worker_mutex);

    lv2_worker->work_size = 0;
    lv2_worker->work_data = NULL;

    pthread_mutex_unlock(lv2_worker_mutex);
    
    /* response data */
    pthread_mutex_lock(lv2_worker_mutex);

    response_data =
      response_data_start = g_list_copy(lv2_worker->response_data);

    pthread_mutex_lock(lv2_worker_mutex);
  
    while(response_data != NULL){
      work_response(handle,
		    AGS_LV2_WORKER_RESPONSE_DATA(response_data->data)->data_size,
		    AGS_LV2_WORKER_RESPONSE_DATA(response_data->data)->data);
      
      response_data = response_data->next;
    }

    /* free response data */
    pthread_mutex_lock(lv2_worker_mutex);
    
    g_list_free_full(response_data_start,
		     (GDestroyNotify) ags_lv2_worker_free_response_data);
    
    g_list_free(lv2_worker->response_data);
    lv2_worker->response_data = NULL;
    
    pthread_mutex_unlock(lv2_worker_mutex);

    /* end run */
    end_run(handle);

    /* reset */
    ags_lv2_worker_unset_flags(lv2_worker, AGS_LV2_WORKER_RUN);
  }
}

/**
 * ags_lv2_worker_interrupted_callback:
 * @thread: the #AgsReturnableThread
 * @sig: the signal number
 * @time_cycle: the time cycle
 * @time_spent: the time spent return location
 * @lv2_worker: the #AgsLv2Worker
 * 
 * Interrupted callback.
 * 
 * Since: 2.0.0
 */
void
ags_lv2_worker_interrupted_callback(AgsThread *thread,
				    int sig,
				    guint time_cycle, guint *time_spent,
				    AgsLv2Worker *lv2_worker)
{
  if(!ags_thread_test_sync_flags(thread, AGS_THREAD_INTERRUPTED)){
    g_atomic_int_or(&(thread->sync_flags),
    		    AGS_THREAD_INTERRUPTED);
    
    if(ags_lv2_worker_test_flags(lv2_worker, AGS_LV2_WORKER_RUN)){
#ifdef AGS_PTHREAD_SUSPEND
      pthread_suspend(thread->thread);
#else
      pthread_kill(*(thread->thread), AGS_THREAD_SUSPEND_SIG);
#endif
    }
  }
}

/**
 * ags_lv2_worker_new:
 * @returnable_thread: the #AgsReturnableThread
 *
 * Create a new instance of #AgsLv2Worker.
 *
 * Returns: the new #AgsLv2Worker
 *
 * Since: 2.0.0
 */ 
AgsLv2Worker*
ags_lv2_worker_new(AgsThread *returnable_thread)
{
  AgsLv2Worker *lv2_worker;

  lv2_worker = (AgsLv2Worker *) g_object_new(AGS_TYPE_LV2_WORKER,
					     "returnable-thread", returnable_thread,
					     NULL);

  return(lv2_worker);
}
