/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/server/ags_server_application_context.h>

#include <ags/lib/ags_log.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_main_loop.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_id_ref.h>

#include <ags/thread/ags_concurrency_provider.h>
#include <ags/thread/ags_thread.h>
#include <ags/thread/ags_generic_main_loop.h>
#include <ags/thread/ags_thread_pool.h>

#include <ags/server/ags_service_provider.h>

void ags_server_application_context_class_init(AgsServerApplicationContextClass *server_application_context);
void ags_server_application_context_concurrency_provider_interface_init(AgsConcurrencyProviderInterface *concurrency_provider);
void ags_server_application_context_service_provider_interface_init(AgsServiceProviderInterface *service_provider);
void ags_server_application_context_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_server_application_context_init(AgsServerApplicationContext *server_application_context);
void ags_server_application_context_set_property(GObject *gobject,
						 guint prop_id,
						 const GValue *value,
						 GParamSpec *param_spec);
void ags_server_application_context_get_property(GObject *gobject,
						 guint prop_id,
						 GValue *value,
						 GParamSpec *param_spec);
void ags_server_application_context_connect(AgsConnectable *connectable);
void ags_server_application_context_disconnect(AgsConnectable *connectable);
void ags_server_application_context_finalize(GObject *gobject);

AgsThread* ags_server_application_context_get_main_loop(AgsConcurrencyProvider *concurrency_provider);
void ags_server_application_context_set_main_loop(AgsConcurrencyProvider *concurrency_provider,
						  AgsThread *main_loop);
AgsTaskLauncher* ags_server_application_context_get_task_launcher(AgsConcurrencyProvider *concurrency_provider);
void ags_server_application_context_set_task_launcher(AgsConcurrencyProvider *concurrency_provider,
						      AgsTaskLauncher *task_launcher);
AgsThreadPool* ags_server_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider);
void ags_server_application_context_set_thread_pool(AgsConcurrencyProvider *concurrency_provider,
						    AgsThreadPool *thread_pool);
GList* ags_server_application_context_get_worker(AgsConcurrencyProvider *concurrency_provider);
void ags_server_application_context_set_worker(AgsConcurrencyProvider *concurrency_provider,
					       GList *worker);

gboolean ags_server_application_context_is_operating(AgsServiceProvider *service_provider);

AgsServerStatus* ags_server_application_context_server_status(AgsServiceProvider *service_provider);

void ags_server_application_context_set_registry(AgsServiceProvider *service_provider,
						 AgsRegistry *registry);
AgsRegistry* ags_server_application_context_get_registry(AgsServiceProvider *service_provider);

void ags_server_application_context_set_server(AgsServiceProvider *service_provider,
					       GList *server);
GList* ags_server_application_context_get_server(AgsServiceProvider *service_provider);

void ags_server_application_context_prepare(AgsApplicationContext *application_context);
void ags_server_application_context_setup(AgsApplicationContext *application_context);

void* ags_server_application_context_server_main_loop_thread(GMainLoop *main_loop);

static gpointer ags_server_application_context_parent_class = NULL;
static AgsConnectableInterface* ags_server_application_context_parent_connectable_interface;

extern AgsApplicationContext *ags_application_context;

GType
ags_server_application_context_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_server_application_context = 0;

    static const GTypeInfo ags_server_application_context_info = {
      sizeof (AgsServerApplicationContextClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_server_application_context_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsServerApplicationContext),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_server_application_context_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_server_application_context_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_concurrency_provider_interface_info = {
      (GInterfaceInitFunc) ags_server_application_context_concurrency_provider_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_service_provider_interface_info = {
      (GInterfaceInitFunc) ags_server_application_context_service_provider_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_server_application_context = g_type_register_static(AGS_TYPE_APPLICATION_CONTEXT,
								 "AgsServerApplicationContext",
								 &ags_server_application_context_info,
								 0);

    g_type_add_interface_static(ags_type_server_application_context,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_server_application_context,
				AGS_TYPE_CONCURRENCY_PROVIDER,
				&ags_concurrency_provider_interface_info);

    g_type_add_interface_static(ags_type_server_application_context,
				AGS_TYPE_SERVICE_PROVIDER,
				&ags_service_provider_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_server_application_context);
  }

  return g_define_type_id__volatile;
}

void
ags_server_application_context_class_init(AgsServerApplicationContextClass *server_application_context)
{
  GObjectClass *gobject;
  AgsApplicationContextClass *application_context;

  ags_server_application_context_parent_class = g_type_class_peek_parent(server_application_context);

  /* GObjectClass */
  gobject = (GObjectClass *) server_application_context;

  gobject->set_property = ags_server_application_context_set_property;
  gobject->get_property = ags_server_application_context_get_property;

  gobject->finalize = ags_server_application_context_finalize;

  /* AgsServerApplicationContextClass */
  application_context = (AgsApplicationContextClass *) server_application_context;
  
  application_context->load_config = NULL;

  application_context->prepare = ags_server_application_context_prepare;
  application_context->setup = ags_server_application_context_setup;

  application_context->register_types = NULL;
}

void
ags_server_application_context_concurrency_provider_interface_init(AgsConcurrencyProviderInterface *concurrency_provider)
{
  concurrency_provider->get_main_loop = ags_server_application_context_get_main_loop;
  concurrency_provider->set_main_loop = ags_server_application_context_set_main_loop;

  concurrency_provider->get_task_launcher = ags_server_application_context_get_task_launcher;
  concurrency_provider->set_task_launcher = ags_server_application_context_set_task_launcher;

  concurrency_provider->get_thread_pool = ags_server_application_context_get_thread_pool;
  concurrency_provider->set_thread_pool = ags_server_application_context_set_thread_pool;

  concurrency_provider->get_worker = ags_server_application_context_get_worker;
  concurrency_provider->set_worker = ags_server_application_context_set_worker;
}

void
ags_server_application_context_service_provider_interface_init(AgsServiceProviderInterface *service_provider)
{
  service_provider->is_operating = ags_server_application_context_is_operating;

  service_provider->server_status = ags_server_application_context_server_status;

  service_provider->set_registry = ags_server_application_context_set_registry;
  service_provider->get_registry = ags_server_application_context_get_registry;
  
  service_provider->set_server = ags_server_application_context_set_server;
  service_provider->get_server = ags_server_application_context_get_server;
}

void
ags_server_application_context_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_server_application_context_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_server_application_context_connect;
  connectable->disconnect = ags_server_application_context_disconnect;
}

void
ags_server_application_context_init(AgsServerApplicationContext *server_application_context)
{
  AgsConfig *config;
  AgsLog *log;
  
  if(ags_application_context == NULL){
    ags_application_context = (AgsApplicationContext *) server_application_context;
  }
  
  /* fundamental instances */
  config = ags_config_get_instance();

  AGS_APPLICATION_CONTEXT(server_application_context)->config = config;
  g_object_ref(config);

  log = (GObject *) ags_log_get_instance();

  AGS_APPLICATION_CONTEXT(server_application_context)->log = log;
  g_object_ref(log);
  
  /* server application context */  
  server_application_context->thread_pool = NULL;

  server_application_context->worker = NULL;
  
  server_application_context->is_operating = FALSE;

  server_application_context->server_status = NULL;

  server_application_context->registry = NULL;

  server_application_context->server = NULL;
}

void
ags_server_application_context_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec)
{
  AgsServerApplicationContext *server_application_context;

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_server_application_context_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec)
{
  AgsServerApplicationContext *server_application_context;

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_server_application_context_connect(AgsConnectable *connectable)
{
  AgsServerApplicationContext *server_application_context;

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(connectable);

  if((AGS_APPLICATION_CONTEXT_CONNECTED & (AGS_APPLICATION_CONTEXT(server_application_context)->flags)) != 0){
    return;
  }

  ags_server_application_context_parent_connectable_interface->connect(connectable);
}

void
ags_server_application_context_disconnect(AgsConnectable *connectable)
{
  AgsServerApplicationContext *server_application_context;

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(connectable);

  if((AGS_APPLICATION_CONTEXT_CONNECTED & (AGS_APPLICATION_CONTEXT(server_application_context)->flags)) == 0){
    return;
  }

  ags_server_application_context_parent_connectable_interface->disconnect(connectable);
}

void
ags_server_application_context_finalize(GObject *gobject)
{
  AgsServerApplicationContext *server_application_context;

  G_OBJECT_CLASS(ags_server_application_context_parent_class)->finalize(gobject);

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(gobject);
}

AgsThread*
ags_server_application_context_get_main_loop(AgsConcurrencyProvider *concurrency_provider)
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
ags_server_application_context_set_main_loop(AgsConcurrencyProvider *concurrency_provider,
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
ags_server_application_context_get_task_launcher(AgsConcurrencyProvider *concurrency_provider)
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
ags_server_application_context_set_task_launcher(AgsConcurrencyProvider *concurrency_provider,
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
ags_server_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider)
{
  AgsThreadPool *thread_pool;
  
  AgsServerApplicationContext *server_application_context;
  
  GRecMutex *application_context_mutex;

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(server_application_context);

  /* get thread pool */
  g_rec_mutex_lock(application_context_mutex);

  thread_pool = server_application_context->thread_pool;

  if(thread_pool != NULL){
    g_object_ref(thread_pool);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(thread_pool);
}

void
ags_server_application_context_set_thread_pool(AgsConcurrencyProvider *concurrency_provider,
					      AgsThreadPool *thread_pool)
{
  AgsServerApplicationContext *server_application_context;
  
  GRecMutex *application_context_mutex;

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(server_application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  if(server_application_context->thread_pool == thread_pool){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  if(server_application_context->thread_pool != NULL){
    g_object_unref(server_application_context->thread_pool);
  }
  
  if(thread_pool != NULL){
    g_object_ref(thread_pool);
  }
  
  server_application_context->thread_pool = (GObject *) thread_pool;
  
  g_rec_mutex_unlock(application_context_mutex);
}

GList*
ags_server_application_context_get_worker(AgsConcurrencyProvider *concurrency_provider)
{
  AgsServerApplicationContext *server_application_context;
  
  GList *worker;
  
  GRecMutex *application_context_mutex;

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(server_application_context);

  /* get worker */
  g_rec_mutex_lock(application_context_mutex);

  worker = g_list_copy_deep(server_application_context->worker,
			    (GCopyFunc) g_object_ref,
			    NULL);
  
  g_rec_mutex_unlock(application_context_mutex);

  return(worker);
}

void
ags_server_application_context_set_worker(AgsConcurrencyProvider *concurrency_provider,
					 GList *worker)
{
  AgsServerApplicationContext *server_application_context;
  
  GRecMutex *application_context_mutex;

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(server_application_context);

  /* set worker */
  g_rec_mutex_lock(application_context_mutex);

  if(server_application_context->worker == worker){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  g_list_free_full(server_application_context->worker,
		   g_object_unref);
  
  server_application_context->worker = worker;

  g_rec_mutex_unlock(application_context_mutex);
}

gboolean
ags_server_application_context_is_operating(AgsServiceProvider *service_provider)
{
  AgsServerApplicationContext *server_application_context;

  gboolean is_operating;
  
  GRecMutex *application_context_mutex;

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(service_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(server_application_context);

  /* get gui ready */
  g_rec_mutex_lock(application_context_mutex);

  is_operating = server_application_context->is_operating;
   
  g_rec_mutex_unlock(application_context_mutex);

  return(is_operating);
}

AgsServerStatus*
ags_server_application_context_server_status(AgsServiceProvider *service_provider)
{
  AgsServerStatus *server_status;
  
  AgsServerApplicationContext *server_application_context;
  
  GRecMutex *application_context_mutex;

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(service_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(server_application_context);

  /* get thread pool */
  g_rec_mutex_lock(application_context_mutex);

  server_status = server_application_context->server_status;

  if(server_status != NULL){
    g_object_ref(server_status);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(server_status);
}

AgsRegistry*
ags_server_application_context_get_registry(AgsServiceProvider *service_provider)
{
  AgsRegistry *registry;
  
  AgsServerApplicationContext *server_application_context;
  
  GRecMutex *application_context_mutex;

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(service_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(server_application_context);

  /* get thread pool */
  g_rec_mutex_lock(application_context_mutex);

  registry = server_application_context->registry;

  if(registry != NULL){
    g_object_ref(registry);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(registry);
}

void
ags_server_application_context_set_registry(AgsServiceProvider *service_provider,
					   AgsRegistry *registry)
{
  AgsServerApplicationContext *server_application_context;
  
  GRecMutex *application_context_mutex;

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(service_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(server_application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  if(server_application_context->registry == registry){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  if(server_application_context->registry != NULL){
    g_object_unref(server_application_context->registry);
  }
  
  if(registry != NULL){
    g_object_ref(registry);
  }
  
  server_application_context->registry = (GObject *) registry;
  
  g_rec_mutex_unlock(application_context_mutex);
}

GList*
ags_server_application_context_get_server(AgsServiceProvider *service_provider)
{
  AgsServerApplicationContext *server_application_context;
  
  GList *server;
  
  GRecMutex *application_context_mutex;

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(service_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(server_application_context);

  /* get server */
  g_rec_mutex_lock(application_context_mutex);

  server = g_list_copy_deep(server_application_context->server,
			    (GCopyFunc) g_object_ref,
			    NULL);
  
  g_rec_mutex_unlock(application_context_mutex);

  return(server);
}

void
ags_server_application_context_set_server(AgsServiceProvider *service_provider,
					 GList *server)
{
  AgsServerApplicationContext *server_application_context;
  
  GRecMutex *application_context_mutex;

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(service_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(server_application_context);

  /* set server */
  g_rec_mutex_lock(application_context_mutex);

  if(server_application_context->server == server){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  g_list_free_full(server_application_context->server,
		   g_object_unref);
  
  server_application_context->server = server;

  g_rec_mutex_unlock(application_context_mutex);
}

void
ags_server_application_context_prepare(AgsApplicationContext *application_context)
{
  AgsServerApplicationContext *server_application_context;

  AgsThread *generic_main_loop;
  AgsTaskLauncher *task_launcher;

  GMainContext *server_main_context;
  GMainLoop *main_loop;
    
  server_application_context = (AgsServerApplicationContext *) application_context;

  /* call parent */
  //  AGS_APPLICATION_CONTEXT_CLASS(ags_server_application_context_parent_class)->prepare(application_context);
  
  /* register types */
  ags_application_context_register_types(application_context);

  /*
   * fundamental thread setup
   */
  /* server main context and main loop */
  server_main_context = g_main_context_new();
  g_main_context_ref(server_main_context);

  server_application_context->server_main_context = server_main_context;

  main_loop = g_main_loop_new(server_main_context,
			      TRUE);

  g_thread_new("Advanced Gtk+ Sequencer - server main loop",
	       ags_server_application_context_server_main_loop_thread,
	       main_loop);

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
			   server_main_context);
}

void
ags_server_application_context_setup(AgsApplicationContext *application_context)
{
  AgsServerApplicationContext *server_application_context;

  AgsServer *server;

  AgsThread *main_loop;
  AgsTaskLauncher *task_launcher;

  AgsLog *log;
  AgsConfig *config;    
  
  gchar *server_group;
  gchar *str;

  guint i;

  GRecMutex *application_context_mutex;
  
  server_application_context = (AgsServerApplicationContext *) application_context;

  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);
  
  /* config and log */
  config = ags_config_get_instance();

  log = ags_log_get_instance();

  /* main loop and task launcher */
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  /* AgsWorkerThread */
  server_application_context->worker = NULL;
  
  /* AgsThreadPool */
  server_application_context->thread_pool = NULL;

  /* AgsServer */
  server_application_context->server = NULL;
  server = NULL;

  server_group = g_strdup("server");
  
  for(i = 0; ; i++){
    gchar *ip4, *ip6;

    guint server_port;
    gboolean auto_start;
    gboolean any_address;
    gboolean enable_ip4, enable_ip6;
    
    if(!g_key_file_has_group(config->key_file,
			     server_group)){
      if(i == 0){
	g_free(server_group);    
	server_group = g_strdup_printf("%s-%d",
				       AGS_CONFIG_SERVER,
				       i);
    	
	continue;
      }else{
	break;
      }
    }

    server = ags_server_new();

    server_application_context->server = g_list_append(server_application_context->server,
						       server);
    g_object_ref(server);

    /* realm */
    str = ags_config_get_value(config,
			       server_group,
			       "realm");
    
    if(str != NULL){
      g_object_set(server,
		   "realm", str,
		   NULL);
      
      g_free(str);
    }
    
    /* any address */
    any_address = FALSE;

    str = ags_config_get_value(config,
			       server_group,
			       "any-address");
    
    if(str != NULL){
      any_address = (!g_ascii_strncasecmp(str,
					  "true",
					  5)) ? TRUE: FALSE;
      g_free(str);
    }

    if(any_address){
      ags_server_set_flags(server,
			   (AGS_SERVER_ANY_ADDRESS));
    }

    /* enable ip4 and ip6 */
    enable_ip4 = FALSE;
    enable_ip6 = FALSE;

    str = ags_config_get_value(config,
			       server_group,
			       "enable-ip4");
    
    if(str != NULL){
      enable_ip4 = (!g_ascii_strncasecmp(str,
					 "true",
					 5)) ? TRUE: FALSE;
      g_free(str);
    }

    str = ags_config_get_value(config,
			       server_group,
			       "enable-ip6");

    if(str != NULL){
      enable_ip6 = (!g_ascii_strncasecmp(str,
					 "true",
					 5)) ? TRUE: FALSE;
      g_free(str);
    }

    if(enable_ip4){
      ags_server_set_flags(server,
			   (AGS_SERVER_INET4));
    }

    if(enable_ip6){
      ags_server_set_flags(server,
			   (AGS_SERVER_INET6));
    }

    /* ip4 and ip6 address */
    str = ags_config_get_value(config,
			       server_group,
			       "ip4-address");

    if(str != NULL){
      g_object_set(server,
		   "ip4", str,
		   NULL);
      
      g_free(str);
    }

    str = ags_config_get_value(config,
			       server_group,
			       "ip6-address");

    if(str != NULL){
      g_object_set(server,
		   "ip6", str,
		   NULL);
      
      g_free(str);
    }

    /* server port */
    str = ags_config_get_value(config,
			       server_group,
			       "server-port");

    if(str != NULL){
      server_port = (guint) g_ascii_strtoull(str,
					     NULL,
					     10);

      g_object_set(server,
		   "server-port", server_port,
		   NULL);
    }
    
    /* auto-start */
    auto_start = FALSE;
    
    str = ags_config_get_value(config,
			       server_group,
			       "auto-start");

    if(str != NULL){
      auto_start = (!g_ascii_strncasecmp(str,
					 "true",
					 5)) ? TRUE: FALSE;
      g_free(str);
    }

    if(auto_start){
      ags_server_set_flags(server, AGS_SERVER_AUTO_START);
    }

    g_free(server_group);    
    server_group = g_strdup_printf("%s-%d",
				   AGS_CONFIG_SERVER,
				   i);
  }

  if(server == NULL){
    server = ags_server_new();
    ags_server_set_flags(server,
			 (AGS_SERVER_INET4));

    server_application_context->server = g_list_append(server_application_context->server,
						       server);
    g_object_ref(server);
  }

  g_rec_mutex_lock(application_context_mutex);

  server_application_context->is_operating = TRUE;
   
  g_rec_mutex_unlock(application_context_mutex);
  
  /* unref */
  g_object_unref(main_loop);
  
  g_object_unref(task_launcher);
}

void*
ags_server_application_context_server_main_loop_thread(GMainLoop *main_loop)
{
  AgsApplicationContext *application_context;

  GList *start_list, *list;

  g_main_context_push_thread_default(g_main_loop_get_context(main_loop));
  
  application_context = ags_application_context_get_instance();

  while(!ags_service_provider_is_operating(AGS_SERVICE_PROVIDER(application_context))){
    usleep(G_USEC_PER_SEC / 30);
  }
  
  list = 
    start_list = ags_service_provider_get_server(AGS_SERVICE_PROVIDER(application_context));

  while(list != NULL){
    if(ags_server_test_flags(list->data, AGS_SERVER_AUTO_START)){
      ags_server_start(AGS_SERVER(list->data));
    }
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
  
  g_main_loop_run(main_loop);

  g_thread_exit(NULL);

  return(NULL);
}

/**
 * ags_server_application_context_new:
 * 
 * Instantiate server application context.
 * 
 * Returns: the new #AgsServerApplicationContext
 * 
 * Since: 3.0.0
 */
AgsServerApplicationContext*
ags_server_application_context_new()
{
  AgsServerApplicationContext *server_application_context;

  server_application_context = (AgsServerApplicationContext *) g_object_new(AGS_TYPE_SERVER_APPLICATION_CONTEXT,
									    NULL);

  return(server_application_context);
}
