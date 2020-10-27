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

#include <ags/plugin/ags_lv2_worker.h>

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
  PROP_WORKER_THREAD,
};

static gpointer ags_lv2_worker_parent_class = NULL;
static AgsConnectableInterface *ags_lv2_worker_parent_connectable_interface;

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

GType
ags_lv2_worker_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_LV2_WORKER_RUN, "AGS_LV2_WORKER_RUN", "lv2-worker-run" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsLv2WorkerFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("response-data",
				    i18n_pspec("response data"),
				    i18n_pspec("The response data"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RESPONSE_DATA,
				  param_spec);

  /**
   * AgsLv2Worker:worker-thread:
   *
   * The assigned worker-thread.
   * 
   * Since: 3.0.0
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
  lv2_worker->flags = 0;

  /* add base plugin mutex */
  g_rec_mutex_init(&(lv2_worker->obj_mutex));

  /* fields */
  lv2_worker->handle = NULL;
  lv2_worker->worker_interface = NULL;

  lv2_worker->work_size = 0;
  lv2_worker->work_data = NULL;

  lv2_worker->response_data = NULL;
  
  lv2_worker->worker_thread = NULL;
}

void
ags_lv2_worker_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsLv2Worker *lv2_worker;

  GRecMutex *lv2_worker_mutex;

  lv2_worker = AGS_LV2_WORKER(gobject);

  /* get base plugin mutex */
  lv2_worker_mutex = AGS_LV2_WORKER_GET_OBJ_MUTEX(lv2_worker);

  switch(prop_id){
  case PROP_HANDLE:
    {
      g_rec_mutex_lock(lv2_worker_mutex);

      lv2_worker->handle = g_value_get_pointer(value);

      g_rec_mutex_unlock(lv2_worker_mutex);
    }
    break;
  case PROP_WORK_SIZE:
    {
      g_rec_mutex_lock(lv2_worker_mutex);

      lv2_worker->work_size = g_value_get_uint(value);

      g_rec_mutex_unlock(lv2_worker_mutex);
    }
    break;
  case PROP_WORK_DATA:
    {
      g_rec_mutex_lock(lv2_worker_mutex);

      lv2_worker->work_data = g_value_get_pointer(value);

      g_rec_mutex_unlock(lv2_worker_mutex);
    }
    break;
  case PROP_RESPONSE_DATA:
    {
      gpointer response_data;

      response_data = g_value_get_pointer(value);
      
      g_rec_mutex_lock(lv2_worker_mutex);

      if(response_data == NULL ||
	 g_list_find(lv2_worker->response_data, response_data) != NULL){
	g_rec_mutex_unlock(lv2_worker_mutex);

	return;
      }

      lv2_worker->response_data = g_list_prepend(lv2_worker->response_data,
						 response_data);

      g_rec_mutex_unlock(lv2_worker_mutex);
    }
    break;
  case PROP_WORKER_THREAD:
    {
      AgsWorkerThread *worker_thread;

      worker_thread = g_value_get_object(value);
      
      g_rec_mutex_lock(lv2_worker_mutex);

      if(lv2_worker->worker_thread == worker_thread){
	g_rec_mutex_unlock(lv2_worker_mutex);

	return;
      }

      if(lv2_worker->worker_thread != NULL){
	g_object_unref(lv2_worker->worker_thread);
      }

      if(worker_thread != NULL){
	g_object_ref(worker_thread);
      }

      lv2_worker->worker_thread = worker_thread;
      
      g_rec_mutex_unlock(lv2_worker_mutex);
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

  GRecMutex *lv2_worker_mutex;

  lv2_worker = AGS_LV2_WORKER(gobject);

  /* get base plugin mutex */
  lv2_worker_mutex = AGS_LV2_WORKER_GET_OBJ_MUTEX(lv2_worker);

  switch(prop_id){
  case PROP_HANDLE:
    {
      g_rec_mutex_lock(lv2_worker_mutex);
      
      g_value_set_pointer(value, lv2_worker->handle);

      g_rec_mutex_unlock(lv2_worker_mutex);
    }
    break;
  case PROP_WORK_SIZE:
    {
      g_rec_mutex_lock(lv2_worker_mutex);
      
      g_value_set_uint(value, lv2_worker->work_size);

      g_rec_mutex_unlock(lv2_worker_mutex);
    }
    break;
  case PROP_WORK_DATA:
    {
      g_rec_mutex_lock(lv2_worker_mutex);
      
      g_value_set_pointer(value, lv2_worker->work_data);

      g_rec_mutex_unlock(lv2_worker_mutex);
    }
    break;
  case PROP_RESPONSE_DATA:
    {
      g_rec_mutex_lock(lv2_worker_mutex);
      
      g_value_set_pointer(value, g_list_copy_deep(lv2_worker->response_data,
						  (GCopyFunc) g_object_ref,
						  NULL));

      g_rec_mutex_unlock(lv2_worker_mutex);
    }
    break;
  case PROP_WORKER_THREAD:
    {
      g_rec_mutex_lock(lv2_worker_mutex);
      
      g_value_set_object(value, lv2_worker->worker_thread);

      g_rec_mutex_unlock(lv2_worker_mutex);
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
  g_list_free_full(lv2_worker->response_data,
		   (GDestroyNotify) ags_lv2_worker_free_response_data);
  
  if(lv2_worker->worker_thread != NULL){
    g_object_unref(lv2_worker->worker_thread);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_lv2_worker_parent_class)->finalize(gobject);
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
 * Since: 3.0.0
 */
gboolean
ags_lv2_worker_test_flags(AgsLv2Worker *lv2_worker, guint flags)
{
  gboolean retval;
  
  GRecMutex *lv2_worker_mutex;

  if(!AGS_IS_LV2_WORKER(lv2_worker)){
    return(FALSE);
  }
  
  /* get lv2 worker mutex */
  lv2_worker_mutex = AGS_LV2_WORKER_GET_OBJ_MUTEX(lv2_worker);

  /* test flags */
  g_rec_mutex_lock(lv2_worker_mutex);

  retval = ((flags & (lv2_worker->flags)) != 0) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(lv2_worker_mutex);

  return(retval);
}

/**
 * ags_lv2_worker_set_flags:
 * @lv2_worker: the #AgsLv2Worker
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 3.0.0
 */
void
ags_lv2_worker_set_flags(AgsLv2Worker *lv2_worker, guint flags)
{
  GRecMutex *lv2_worker_mutex;

  if(!AGS_IS_LV2_WORKER(lv2_worker)){
    return;
  }
  
  /* get lv2 worker mutex */
  lv2_worker_mutex = AGS_LV2_WORKER_GET_OBJ_MUTEX(lv2_worker);

  /* set flags */
  g_rec_mutex_lock(lv2_worker_mutex);

  lv2_worker->flags |= flags;
  
  g_rec_mutex_unlock(lv2_worker_mutex);
}

/**
 * ags_lv2_worker_unset_flags:
 * @lv2_worker: the #AgsLv2Worker
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 3.0.0
 */
void
ags_lv2_worker_unset_flags(AgsLv2Worker *lv2_worker, guint flags)
{
  GRecMutex *lv2_worker_mutex;

  if(!AGS_IS_LV2_WORKER(lv2_worker)){
    return;
  }
  
  /* get lv2 worker mutex */
  lv2_worker_mutex = AGS_LV2_WORKER_GET_OBJ_MUTEX(lv2_worker);

  /* unset flags */
  g_rec_mutex_lock(lv2_worker_mutex);

  lv2_worker->flags &= (~flags);
  
  g_rec_mutex_unlock(lv2_worker_mutex);
}

/**
 * ags_lv2_worker_alloc_response_data:
 * 
 * Allocate a new #AgsLv2WorkerResponseData-struct.
 * 
 * Returns: the new #AgsLv2WorkerResponseData-struct
 * 
 * Since: 3.0.0
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
 * Since: 3.0.0
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
 * Since: 3.0.0
 */
LV2_Worker_Status
ags_lv2_worker_respond(LV2_Worker_Respond_Handle handle,
		       uint32_t data_size,
		       const void* data)
{
  AgsLv2Worker *lv2_worker;
  AgsLv2WorkerResponseData *response_data;

  GRecMutex *lv2_worker_mutex;

  lv2_worker = AGS_LV2_WORKER(handle);
  
  /* get lv2 worker mutex */
  lv2_worker_mutex = AGS_LV2_WORKER_GET_OBJ_MUTEX(lv2_worker);

  /* allocate response data */
  response_data = ags_lv2_worker_alloc_response_data();

  response_data->data_size = data_size;
  response_data->data = data;

  g_rec_mutex_lock(lv2_worker_mutex);

  lv2_worker->response_data = g_list_prepend(lv2_worker->response_data,
					     response_data);

  g_rec_mutex_unlock(lv2_worker_mutex);
  
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
 * Since: 3.0.0
 */
LV2_Worker_Status
ags_lv2_worker_schedule_work(LV2_Worker_Schedule_Handle handle,
			     uint32_t data_size,
			     const void* data)
{
  AgsLv2Worker *lv2_worker;

  GRecMutex *lv2_worker_mutex;

  lv2_worker = AGS_LV2_WORKER(handle);
  
  /* get lv2 worker mutex */
  lv2_worker_mutex = AGS_LV2_WORKER_GET_OBJ_MUTEX(lv2_worker);

  /* apply work data and size */
  g_rec_mutex_lock(lv2_worker_mutex);

  lv2_worker->work_size = (guint) data_size;
  lv2_worker->work_data = data;

  g_rec_mutex_unlock(lv2_worker_mutex);

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
 * Since: 3.0.0
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
  
  GRecMutex *lv2_worker_mutex;

  lv2_worker = data;

  /* get lv2 worker mutex */
  lv2_worker_mutex = AGS_LV2_WORKER_GET_OBJ_MUTEX(lv2_worker);

  /* get some fields */
  g_rec_mutex_lock(lv2_worker_mutex);

  handle = lv2_worker->handle;
  
  work_size = lv2_worker->work_size;
  work_data = lv2_worker->work_data;

  if(lv2_worker->worker_interface == NULL){
    g_rec_mutex_unlock(lv2_worker_mutex);
    
    return;    
  }
  
  work = lv2_worker->worker_interface->work;
  work_response = lv2_worker->worker_interface->work_response;
  end_run = lv2_worker->worker_interface->end_run;
  
  g_rec_mutex_unlock(lv2_worker_mutex);

  /* work */
  if(ags_lv2_worker_test_flags(lv2_worker, AGS_LV2_WORKER_RUN)){
    work(handle,
	 ags_lv2_worker_respond,
	 lv2_worker,
	 (uint32_t) (work_size),
	 work_data);
    
    g_rec_mutex_lock(lv2_worker_mutex);

    lv2_worker->work_size = 0;
    lv2_worker->work_data = NULL;

    g_rec_mutex_unlock(lv2_worker_mutex);
    
    /* response data */
    g_rec_mutex_lock(lv2_worker_mutex);

    response_data =
      response_data_start = g_list_copy(lv2_worker->response_data);

    g_rec_mutex_lock(lv2_worker_mutex);
  
    while(response_data != NULL){
      work_response(handle,
		    AGS_LV2_WORKER_RESPONSE_DATA(response_data->data)->data_size,
		    AGS_LV2_WORKER_RESPONSE_DATA(response_data->data)->data);
      
      response_data = response_data->next;
    }

    /* free response data */
    g_rec_mutex_lock(lv2_worker_mutex);
    
    g_list_free_full(response_data_start,
		     (GDestroyNotify) ags_lv2_worker_free_response_data);
    
    g_list_free(lv2_worker->response_data);
    lv2_worker->response_data = NULL;
    
    g_rec_mutex_unlock(lv2_worker_mutex);

    /* end run */
    end_run(handle);

    /* reset */
    ags_lv2_worker_unset_flags(lv2_worker, AGS_LV2_WORKER_RUN);
  }
}

/**
 * ags_lv2_worker_new:
 *
 * Create a new instance of #AgsLv2Worker.
 *
 * Returns: the new #AgsLv2Worker
 *
 * Since: 3.0.0
 */ 
AgsLv2Worker*
ags_lv2_worker_new()
{
  AgsLv2Worker *lv2_worker;

  lv2_worker = (AgsLv2Worker *) g_object_new(AGS_TYPE_LV2_WORKER,
					     NULL);

  return(lv2_worker);
}
