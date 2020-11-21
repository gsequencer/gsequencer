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

#include <ags/thread/ags_thread_application_context.h>

#include <ags/util/ags_id_generator.h>

#include <ags/lib/ags_log.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_main_loop.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_id_ref.h>

#include <ags/thread/ags_concurrency_provider.h>
#include <ags/thread/ags_thread.h>
#include <ags/thread/ags_thread_pool.h>
#include <ags/thread/ags_generic_main_loop.h>
#include <ags/thread/ags_returnable_thread.h>
#include <ags/thread/ags_task_launcher.h>

#include <ags/i18n.h>

void ags_thread_application_context_class_init(AgsThreadApplicationContextClass *thread_application_context);
void ags_thread_application_context_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_thread_application_context_concurrency_provider_interface_init(AgsConcurrencyProviderInterface *concurrency_provider);
void ags_thread_application_context_init(AgsThreadApplicationContext *thread_application_context);
void ags_thread_application_context_set_property(GObject *gobject,
						 guint prop_id,
						 const GValue *value,
						 GParamSpec *param_spec);
void ags_thread_application_context_get_property(GObject *gobject,
						 guint prop_id,
						 GValue *value,
						 GParamSpec *param_spec);
void ags_thread_application_context_finalize(GObject *gobject);

void ags_thread_application_context_connect(AgsConnectable *connectable);
void ags_thread_application_context_disconnect(AgsConnectable *connectable);

AgsThread* ags_thread_application_context_get_main_loop(AgsConcurrencyProvider *concurrency_provider);
void ags_thread_application_context_set_main_loop(AgsConcurrencyProvider *concurrency_provider,
						  AgsThread *main_loop);
AgsTaskLauncher* ags_thread_application_context_get_task_launcher(AgsConcurrencyProvider *concurrency_provider);
void ags_thread_application_context_set_task_launcher(AgsConcurrencyProvider *concurrency_provider,
						      AgsTaskLauncher *task_launcher);
AgsThreadPool* ags_thread_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider);
void ags_thread_application_context_set_thread_pool(AgsConcurrencyProvider *concurrency_provider,
						    AgsThreadPool *thread_pool);
GList* ags_thread_application_context_get_worker(AgsConcurrencyProvider *concurrency_provider);
void ags_thread_application_context_set_worker(AgsConcurrencyProvider *concurrency_provider,
					       GList *worker);

void ags_thread_application_context_prepare(AgsApplicationContext *application_context);
void ags_thread_application_context_setup(AgsApplicationContext *application_context);

void ags_thread_application_context_set_value_callback(AgsConfig *config, gchar *group, gchar *key, gchar *value,
						       AgsThreadApplicationContext *thread_application_context);

/**
 * SECTION:ags_thread_application_context
 * @short_description: threaded application context
 * @title: AgsThreadApplicationContext
 * @section_id:
 * @include: ags/thread/ags_thread_application_context.h
 *
 * The #AgsThreadApplicationContext provides you multi-threaded functionality.
 */

enum{
  PROP_0,
  PROP_THREAD_POOL,
};

static gpointer ags_thread_application_context_parent_class = NULL;
static AgsConnectableInterface* ags_thread_application_context_parent_connectable_interface;

extern AgsApplicationContext *ags_application_context;

GType
ags_thread_application_context_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_thread_application_context = 0;

    static const GTypeInfo ags_thread_application_context_info = {
      sizeof (AgsThreadApplicationContextClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_thread_application_context_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsThreadApplicationContext),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_thread_application_context_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_thread_application_context_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_concurrency_provider_interface_info = {
      (GInterfaceInitFunc) ags_thread_application_context_concurrency_provider_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_thread_application_context = g_type_register_static(AGS_TYPE_APPLICATION_CONTEXT,
								"AgsThreadApplicationContext",
								&ags_thread_application_context_info,
								0);

    g_type_add_interface_static(ags_type_thread_application_context,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_thread_application_context,
				AGS_TYPE_CONCURRENCY_PROVIDER,
				&ags_concurrency_provider_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_thread_application_context);
  }

  return g_define_type_id__volatile;
}

GType
ags_thread_application_context_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_THREAD_APPLICATION_CONTEXT_SINGLE_THREAD, "AGS_THREAD_APPLICATION_CONTEXT_SINGLE_THREAD", "thread-application-context-single-thread" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsThreadApplicationContextFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

void
ags_thread_application_context_class_init(AgsThreadApplicationContextClass *thread_application_context)
{
  GObjectClass *gobject;
  AgsApplicationContextClass *application_context;
  GParamSpec *param_spec;
  
  ags_thread_application_context_parent_class = g_type_class_peek_parent(thread_application_context);

  /* GObjectClass */
  gobject = (GObjectClass *) thread_application_context;

  gobject->set_property = ags_thread_application_context_set_property;
  gobject->get_property = ags_thread_application_context_get_property;

  gobject->finalize = ags_thread_application_context_finalize;
  
  /* AgsApplicationContextClass */
  application_context = (AgsApplicationContextClass *) thread_application_context;

  application_context->prepare = ags_thread_application_context_prepare;
  application_context->setup = ags_thread_application_context_setup;

  /* properties */
  /**
   * AgsThreadApplicationContext:thread-pool:
   *
   * The assigned thread pool.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("thread-pool",
				   i18n_pspec("thread pool of thread application context"),
				   i18n_pspec("The thread pool which this thread application context assigned to"),
				   AGS_TYPE_THREAD_POOL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_THREAD_POOL,
				  param_spec);

  /* AgsThreadApplicationContextClass */
  application_context = (AgsApplicationContextClass *) thread_application_context;
  
  application_context->register_types = ags_thread_application_context_register_types;
}

void
ags_thread_application_context_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_thread_application_context_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_thread_application_context_connect;
  connectable->disconnect = ags_thread_application_context_disconnect;
}

void
ags_thread_application_context_concurrency_provider_interface_init(AgsConcurrencyProviderInterface *concurrency_provider)
{
  concurrency_provider->get_main_loop = ags_thread_application_context_get_main_loop;
  concurrency_provider->set_main_loop = ags_thread_application_context_set_main_loop;

  concurrency_provider->get_task_launcher = ags_thread_application_context_get_task_launcher;
  concurrency_provider->set_task_launcher = ags_thread_application_context_set_task_launcher;

  concurrency_provider->get_thread_pool = ags_thread_application_context_get_thread_pool;
  concurrency_provider->set_thread_pool = ags_thread_application_context_set_thread_pool;

  concurrency_provider->get_worker = ags_thread_application_context_get_worker;
  concurrency_provider->set_worker = ags_thread_application_context_set_worker;
}

void
ags_thread_application_context_init(AgsThreadApplicationContext *thread_application_context)
{
  AgsConfig *config;
  AgsLog *log;
  
  if(ags_application_context == NULL){
    ags_application_context = (AgsApplicationContext *) thread_application_context;
  }
  
  /* fundamental instances */
  config = ags_config_get_instance();

  AGS_APPLICATION_CONTEXT(thread_application_context)->config = config;
  g_object_ref(config);

  log = (GObject *) ags_log_get_instance();

  AGS_APPLICATION_CONTEXT(thread_application_context)->log = log;
  g_object_ref(log);
  
  /* Thread application context */  
  thread_application_context->thread_pool = NULL;
 
  thread_application_context->worker = NULL;
}

void
ags_thread_application_context_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec)
{
  AgsThreadApplicationContext *thread_application_context;

  thread_application_context = AGS_THREAD_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  case PROP_THREAD_POOL:
    {
      AgsThreadPool *thread_pool;
      
      thread_pool = (AgsThreadPool *) g_value_get_object(value);

      if(thread_pool == thread_application_context->thread_pool)
	return;

      if(thread_application_context->thread_pool != NULL)
	g_object_unref(thread_application_context->thread_pool);

      if(thread_pool != NULL)
	g_object_ref(G_OBJECT(thread_pool));

      thread_application_context->thread_pool = thread_pool;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_thread_application_context_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec)
{
  AgsThreadApplicationContext *thread_application_context;

  thread_application_context = AGS_THREAD_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  case PROP_THREAD_POOL:
    {
      g_value_set_object(value, thread_application_context->thread_pool);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_thread_application_context_finalize(GObject *gobject)
{
  AgsThreadApplicationContext *thread_application_context;

  thread_application_context = AGS_THREAD_APPLICATION_CONTEXT(gobject);

  if(thread_application_context->thread_pool != NULL){
    g_object_unref(thread_application_context->thread_pool);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_thread_application_context_parent_class)->finalize(gobject);
}

void
ags_thread_application_context_connect(AgsConnectable *connectable)
{
  AgsThreadApplicationContext *thread_application_context;

  thread_application_context = AGS_THREAD_APPLICATION_CONTEXT(connectable);

  if((AGS_APPLICATION_CONTEXT_CONNECTED & (AGS_APPLICATION_CONTEXT(thread_application_context)->flags)) != 0){
    return;
  }

  ags_thread_application_context_parent_connectable_interface->connect(connectable);

  ags_connectable_connect(AGS_CONNECTABLE(AGS_APPLICATION_CONTEXT(thread_application_context)->main_loop));
  ags_connectable_connect(AGS_CONNECTABLE(thread_application_context->thread_pool));
}

void
ags_thread_application_context_disconnect(AgsConnectable *connectable)
{
  AgsThreadApplicationContext *thread_application_context;

  thread_application_context = AGS_THREAD_APPLICATION_CONTEXT(connectable);

  if((AGS_APPLICATION_CONTEXT_CONNECTED & (AGS_APPLICATION_CONTEXT(thread_application_context)->flags)) == 0){
    return;
  }
  
  ags_connectable_disconnect(AGS_CONNECTABLE(AGS_APPLICATION_CONTEXT(thread_application_context)->main_loop));
  ags_connectable_disconnect(AGS_CONNECTABLE(thread_application_context->thread_pool));

  ags_thread_application_context_parent_connectable_interface->disconnect(connectable);
}

AgsThread*
ags_thread_application_context_get_main_loop(AgsConcurrencyProvider *concurrency_provider)
{
  AgsThread *main_loop;

  AgsApplicationContext *application_context;
  
  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  if(main_loop != NULL){
    g_object_ref(main_loop);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(main_loop);
}

void
ags_thread_application_context_set_main_loop(AgsConcurrencyProvider *concurrency_provider,
					    AgsThread *main_loop)
{
  AgsApplicationContext *application_context;
  
  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  if(application_context->main_loop == main_loop){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  if(application_context->main_loop != NULL){
    g_object_unref(application_context->main_loop);
  }
  
  if(main_loop != NULL){
    g_object_ref(main_loop);
  }
  
  application_context->main_loop = (GObject *) main_loop;
  
  g_rec_mutex_unlock(application_context_mutex);
}

AgsTaskLauncher*
ags_thread_application_context_get_task_launcher(AgsConcurrencyProvider *concurrency_provider)
{
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;
  
  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  task_launcher = (AgsThread *) application_context->task_launcher;

  if(task_launcher != NULL){
    g_object_ref(task_launcher);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(task_launcher);
}

void
ags_thread_application_context_set_task_launcher(AgsConcurrencyProvider *concurrency_provider,
						AgsTaskLauncher *task_launcher)
{
  AgsApplicationContext *application_context;
  
  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  if(application_context->task_launcher == task_launcher){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  if(application_context->task_launcher != NULL){
    g_object_unref(application_context->task_launcher);
  }
  
  if(task_launcher != NULL){
    g_object_ref(task_launcher);
  }
  
  application_context->task_launcher = (GObject *) task_launcher;
  
  g_rec_mutex_unlock(application_context_mutex);
}

AgsThreadPool*
ags_thread_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider)
{
  AgsThreadPool *thread_pool;
  
  AgsThreadApplicationContext *thread_application_context;
  
  GRecMutex *application_context_mutex;

  thread_application_context = AGS_THREAD_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(thread_application_context);

  /* get thread pool */
  g_rec_mutex_lock(application_context_mutex);

  thread_pool = thread_application_context->thread_pool;

  if(thread_pool != NULL){
    g_object_ref(thread_pool);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(thread_pool);
}

void
ags_thread_application_context_set_thread_pool(AgsConcurrencyProvider *concurrency_provider,
					      AgsThreadPool *thread_pool)
{
  AgsThreadApplicationContext *thread_application_context;
  
  GRecMutex *application_context_mutex;

  thread_application_context = AGS_THREAD_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(thread_application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  if(thread_application_context->thread_pool == thread_pool){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  if(thread_application_context->thread_pool != NULL){
    g_object_unref(thread_application_context->thread_pool);
  }
  
  if(thread_pool != NULL){
    g_object_ref(thread_pool);
  }
  
  thread_application_context->thread_pool = (GObject *) thread_pool;
  
  g_rec_mutex_unlock(application_context_mutex);
}

GList*
ags_thread_application_context_get_worker(AgsConcurrencyProvider *concurrency_provider)
{
  AgsThreadApplicationContext *thread_application_context;
  
  GList *worker;
  
  GRecMutex *application_context_mutex;

  thread_application_context = AGS_THREAD_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(thread_application_context);

  /* get worker */
  g_rec_mutex_lock(application_context_mutex);

  worker = g_list_copy_deep(thread_application_context->worker,
			    (GCopyFunc) g_object_ref,
			    NULL);
  
  g_rec_mutex_unlock(application_context_mutex);

  return(worker);
}

void
ags_thread_application_context_set_worker(AgsConcurrencyProvider *concurrency_provider,
					 GList *worker)
{
  AgsThreadApplicationContext *thread_application_context;
  
  GRecMutex *application_context_mutex;

  thread_application_context = AGS_THREAD_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(thread_application_context);

  /* set worker */
  g_rec_mutex_lock(application_context_mutex);

  if(thread_application_context->worker == worker){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  g_list_free_full(thread_application_context->worker,
		   g_object_unref);
  
  thread_application_context->worker = worker;

  g_rec_mutex_unlock(application_context_mutex);
}

void
ags_thread_application_context_prepare(AgsApplicationContext *application_context)
{
  AgsThreadApplicationContext *thread_application_context;

  AgsThread *generic_main_loop;
  AgsTaskLauncher *task_launcher;
  
  GList *start_queue;
  
  thread_application_context = (AgsThreadApplicationContext *) application_context;

  /* call parent */
  //  AGS_APPLICATION_CONTEXT_CLASS(ags_thread_application_context_parent_class)->prepare(application_context);
  
  /* register types */
  ags_application_context_register_types(application_context);

  /*
   * fundamental thread setup
   */
  /* AgsGenericMainLoop */
  generic_main_loop = (AgsThread *) ags_generic_main_loop_new();
  g_object_ref(generic_main_loop);
  
  application_context->main_loop = (GObject *) generic_main_loop;
  ags_connectable_connect(AGS_CONNECTABLE(generic_main_loop));

  /* AgsTaskLauncher */
  task_launcher = ags_task_launcher_new();
  g_object_ref(task_launcher);

  application_context->task_launcher = (GObject *) task_launcher;
  ags_connectable_connect(AGS_CONNECTABLE(task_launcher));  

  ags_task_launcher_attach(task_launcher,
			   g_main_context_default());
  
  /* start generic main loop and thread pool*/
  ags_thread_start(generic_main_loop);
  
  /* wait for generic thread loop */
  g_mutex_lock(AGS_THREAD_GET_START_MUTEX(generic_main_loop));

  if(ags_thread_test_status_flags(generic_main_loop, AGS_THREAD_STATUS_START_WAIT)){
    ags_thread_unset_status_flags(generic_main_loop, AGS_THREAD_STATUS_START_DONE);
      
    while(ags_thread_test_status_flags(generic_main_loop, AGS_THREAD_STATUS_START_WAIT) &&
	  !ags_thread_test_status_flags(generic_main_loop, AGS_THREAD_STATUS_START_DONE)){
      g_cond_wait(AGS_THREAD_GET_START_COND(generic_main_loop),
		  AGS_THREAD_GET_START_MUTEX(generic_main_loop));
    }
  }

  g_mutex_unlock(AGS_THREAD_GET_START_MUTEX(generic_main_loop));

#if 0
  /* main loop run */
  g_main_loop_run(g_main_loop_new(g_main_context_default(),
				  TRUE));
#endif
}

void
ags_thread_application_context_setup(AgsApplicationContext *application_context)
{
  AgsThreadApplicationContext *thread_application_context;

  AgsThread *main_loop;
  AgsTaskLauncher *task_launcher;

  AgsLog *log;
  AgsConfig *config;

  GList *list;  
  
  thread_application_context = (AgsThreadApplicationContext *) application_context;

  /* config and log */
  config = ags_config_get_instance();

  log = ags_log_get_instance();

  /* main loop and task launcher */
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));
  
  /* launch thread */
  ags_log_add_message(log,
		      "* Launch thread");
  
  /* AgsWorkerThread */
  thread_application_context->worker = NULL;
  
  /* AgsThreadPool */
  thread_application_context->thread_pool = NULL;

  /* unref */
  g_object_unref(main_loop);
  
  g_object_unref(task_launcher);
}

void
ags_thread_application_context_register_types(AgsApplicationContext *application_context)
{
  ags_thread_get_type();

  ags_task_launcher_get_type();

  ags_thread_pool_get_type();
  ags_returnable_thread_get_type();
}

void
ags_thread_application_context_set_value_callback(AgsConfig *config, gchar *group, gchar *key, gchar *value,
						  AgsThreadApplicationContext *thread_application_context)
{
  if(!g_ascii_strncasecmp(group,
			  AGS_CONFIG_GENERIC,
			  8)){
  }else if(!g_ascii_strncasecmp(group,
				AGS_CONFIG_THREAD,
				7)){
    if(!g_ascii_strncasecmp(key,
			    "model",
			    6)){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(key,
				  "lock-global",
				  11)){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(key,
				  "lock-parent",
				  11)){
      //TODO:JK: implement me
    }
  }
}

/**
 * ags_thread_application_context_new:
 *
 * Create a new instance of #AgsThreadApplicationContext.
 * 
 * Returns: the new #AgsThreadApplicationContext
 *
 * Since: 3.0.0
 */
AgsThreadApplicationContext*
ags_thread_application_context_new()
{
  AgsThreadApplicationContext *thread_application_context;

  thread_application_context = (AgsThreadApplicationContext *) g_object_new(AGS_TYPE_THREAD_APPLICATION_CONTEXT,
									    NULL);

  return(thread_application_context);
}
