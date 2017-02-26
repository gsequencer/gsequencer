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

#include <ags/server/ags_server_application_context.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>

#include <ags/thread/ags_concurrency_provider.h>
#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_thread-posix.h>
#include <ags/thread/ags_generic_main_loop.h>
#include <ags/thread/ags_thread_pool.h>
#include <ags/thread/ags_task_thread.h>

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
AgsThread* ags_server_application_context_get_task_thread(AgsConcurrencyProvider *concurrency_provider);
AgsThreadPool* ags_server_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider);

gboolean ags_server_application_context_is_operating(AgsServiceProvider *service_provider);
AgsServerStatus* ags_server_application_context_server_status(AgsServiceProvider *service_provider);
void ags_server_application_context_set_registry(AgsServiceProvider *service_provider,
						 GObject *registry);
GObject* ags_server_application_context_get_registry(AgsServiceProvider *service_provider);
void ags_server_application_context_set_server(AgsServiceProvider *service_provider,
					       GList *server);
GList* ags_server_application_context_get_server(AgsServiceProvider *service_provider);
void ags_server_application_context_set_certificate_manager(AgsServiceProvider *service_provider,
							    AgsCertificateManager *certificate_manager);
AgsCertificateManager* ags_server_application_context_get_certificate_manager(AgsServiceProvider *service_provider);
void ags_server_application_context_set_password_store_manager(AgsServiceProvider *service_provider,
							       AgsPasswordStoreManager *password_store_manager);
AgsPasswordStoreManager* ags_server_application_context_get_password_store_manager(AgsServiceProvider *service_provider);
void ags_server_application_context_set_authentication_manager(AgsServiceProvider *service_provider,
							       AgsAuthenticationManager *authentication_manager);
AgsAuthenticationManager* ags_server_application_context_get_authentication_manager(AgsServiceProvider *service_provider);

static gpointer ags_server_application_context_parent_class = NULL;
static AgsConnectableInterface* ags_server_application_context_parent_connectable_interface;

GType
ags_server_application_context_get_type()
{
  static GType ags_type_server_application_context = 0;

  if(!ags_type_server_application_context){
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
								 "AgsServerApplicationContext\0",
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
  }

  return (ags_type_server_application_context);
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
  application_context->register_types = NULL;
}

void
ags_server_application_context_concurrency_provider_interface_init(AgsConcurrencyProviderInterface *concurrency_provider)
{
  concurrency_provider->get_main_loop = ags_server_application_context_get_main_loop;
  concurrency_provider->get_task_thread = ags_server_application_context_get_task_thread;
  concurrency_provider->get_thread_pool = ags_server_application_context_get_thread_pool;
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

  service_provider->set_certificate_manager = ags_server_application_context_set_certificate_manager;
  service_provider->get_certificate_manager = ags_server_application_context_get_certificate_manager;

  service_provider-> = ags_server_application_context_;
  service_provider-> = ags_server_application_context_;

  service_provider-> = ags_server_application_context_;
  service_provider-> = ags_server_application_context_;
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
  server_application_context->flags = 0;

  server_application_context->version = AGS_SERVER_DEFAULT_VERSION;
  server_application_context->build_id = AGS_SERVER_BUILD_ID;

#ifdef AGS_WITH_XMLRPC_C
  server_application_context->env = (xmlrpc_env *) malloc(sizeof(xmlrpc_env));
#else
  server_application_context->env = NULL;
#endif
  
  /**/
  AGS_APPLICATION_CONTEXT(audio_application_context)->log = NULL;

  /* set config */
  config = ags_config_get_instance();
  AGS_APPLICATION_CONTEXT(audio_application_context)->config = config;
  g_object_set(config,
	       "application-context\0", server_application_context,
	       NULL);

  /* registry */
  server_application_context->registry = ags_registry_new();

  /* server */
  server_application_context->server = NULL;

  /* manager */
  server_application_context->certificate_manager = ags_certificate_manager_get_instance();
  server_application_context->password_store_manager = ags_password_store_manager_get_instance();
  server_application_context->authentication_manager = ags_authentication_manager_get_instance();
  
  /* AgsGenericMainLoop */
  generic_main_loop = ags_generic_main_loop_new((GObject *) server_application_context);
  g_object_set(server_application_context,
	       "main-loop\0", generic_main_loop,
	       NULL);

  g_object_ref(generic_main_loop);
  ags_connectable_connect(AGS_CONNECTABLE(generic_main_loop));

  /* AgsTaskThread */
  AGS_APPLICATION_CONTEXT(server_application_context)->task_thread = (GObject *) ags_task_thread_new();
  ags_main_loop_set_async_queue(AGS_MAIN_LOOP(generic_main_loop),
				AGS_APPLICATION_CONTEXT(server_application_context)->task_thread);
  ags_thread_add_child_extended(AGS_THREAD(generic_main_loop),
				AGS_THREAD(AGS_APPLICATION_CONTEXT(server_application_context)->task_thread),
				TRUE, TRUE);

  /* AgsThreadPool */
  server_application_context->thread_pool = AGS_TASK_THREAD(AGS_APPLICATION_CONTEXT(server_application_context)->task_thread)->thread_pool;
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
  return((AgsThread *) AGS_APPLICATION_CONTEXT(concurrency_provider)->main_loop);
}

AgsThread*
ags_server_application_context_get_task_thread(AgsConcurrencyProvider *concurrency_provider)
{
  return((AgsThread *) AGS_APPLICATION_CONTEXT(concurrency_provider)->task_thread);
}

AgsThreadPool*
ags_server_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider)
{
  return(AGS_SERVER_APPLICATION_CONTEXT(concurrency_provider)->thread_pool);
}

gboolean
ags_server_application_context_is_operating(AgsServiceProvider *service_provider)
{
  //TODO:JK: implement me
}

AgsServerStatus*
ags_server_application_context_server_status(AgsServiceProvider *service_provider)
{
  //TODO:JK: implement me
}

void
ags_server_application_context_set_registry(AgsServiceProvider *service_provider,
					    GObject *registry)
{
  //TODO:JK: implement me
}

GObject*
ags_server_application_context_get_registry(AgsServiceProvider *service_provider)
{
  //TODO:JK: implement me
}

void
ags_server_application_context_set_server(AgsServiceProvider *service_provider,
					  GList *server)
{
  //TODO:JK: implement me
}

GList*
ags_server_application_context_get_server(AgsServiceProvider *service_provider)
{
  //TODO:JK: implement me
}

void
ags_server_application_context_set_certificate_manager(AgsServiceProvider *service_provider,
						       AgsCertificateManager *certificate_manager)
{
  //TODO:JK: implement me
}

AgsCertificateManager*
ags_server_application_context_get_certificate_manager(AgsServiceProvider *service_provider)
{
  //TODO:JK: implement me
}

void
ags_server_application_context_set_password_store_manager(AgsServiceProvider *service_provider,
							  AgsPasswordStoreManager *password_store_manager)
{
  //TODO:JK: implement me
}

AgsPasswordStoreManager*
ags_server_application_context_get_password_store_manager(AgsServiceProvider *service_provider)
{
  //TODO:JK: implement me
}

void
ags_server_application_context_set_authentication_manager(AgsServiceProvider *service_provider,
							  AgsAuthenticationManager *authentication_manager)
{
  //TODO:JK: implement me
}

AgsAuthenticationManager*
ags_server_application_context_get_authentication_manager(AgsServiceProvider *service_provider)
{
  //TODO:JK: implement me
}

AgsServerApplicationContext*
ags_server_application_context_new()
{
  AgsServerApplicationContext *server_application_context;

  server_application_context = (AgsServerApplicationContext *) g_object_new(AGS_TYPE_SERVER_APPLICATION_CONTEXT,
									    NULL);

  return(server_application_context);
}
