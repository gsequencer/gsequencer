/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/server/controller/ags_front_controller.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_marshal.h>

#include <ags/server/ags_service_provider.h>
#include <ags/server/ags_server.h>
#include <ags/server/ags_registry.h>

#include <ags/server/security/ags_authentication_manager.h>

#ifdef AGS_WITH_XMLRPC_C
#include <xmlrpc-c/util.h>

#include <xmlrpc-c/base.h>
#include <xmlrpc-c/abyss.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_abyss.h>
#endif

void ags_front_controller_class_init(AgsFrontControllerClass *front_controller);
void ags_front_controller_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_front_controller_init(AgsFrontController *front_controller);
void ags_front_controller_add_to_registry(AgsConnectable *connectable);
void ags_front_controller_remove_from_registry(AgsConnectable *connectable);
void ags_front_controller_connect(AgsConnectable *connectable);
void ags_front_controller_disconnect(AgsConnectable *connectable);
void ags_front_controller_finalize(GObject *gobject);

#ifdef AGS_WITH_XMLRPC_C
xmlrpc_value* ags_front_controller_xmlrpc_request(xmlrpc_env *env,
						  xmlrpc_value *param_array,
						  void *server_info);
#endif

gpointer ags_front_controller_real_authenticate(AgsFrontController *front_controller,
						gchar *authentication_module,
						gchar *login,
						gchar *password,
						gchar *certs);
gpointer ags_front_controller_real_do_request(AgsFrontController *front_controller,
					      GObject *security_context,
					      gchar *context_path,
					      gchar *user_uuid,
					      gchar *security_token,
					      GParameter *params);

/**
 * SECTION:ags_front_controller
 * @short_description: handle all XMLRPC requests
 * @title: AgsFrontController
 * @section_id:
 * @include: ags/server/controller/ags_front_controller.h
 *
 * The #AgsFrontController is an object to handle XMLRPC requests.
 */

enum{
  AUTHENTICATE,
  DO_REQUEST,
  LAST_SIGNAL,
};

static gpointer ags_front_controller_parent_class = NULL;
static guint front_controller_signals[LAST_SIGNAL];

GType
ags_front_controller_get_type()
{
  static GType ags_type_front_controller = 0;

  if(!ags_type_front_controller){
    static const GTypeInfo ags_front_controller_info = {
      sizeof (AgsFrontControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_front_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFrontController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_front_controller_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_front_controller_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_front_controller = g_type_register_static(G_TYPE_OBJECT,
						       "AgsFrontController",
						       &ags_front_controller_info,
						       0);

    g_type_add_interface_static(ags_type_front_controller,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_front_controller);
}

void
ags_front_controller_class_init(AgsFrontControllerClass *front_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_front_controller_parent_class = g_type_class_peek_parent(front_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) front_controller;

  gobject->finalize = ags_front_controller_finalize;

  /* AgsFrontController */
  front_controller->authenticate = ags_front_controller_real_authenticate;
  front_controller->do_request = ags_front_controller_real_do_request;

  /* signals */
  /**
   * AgsFrontController::authenticate:
   * @front_controller: the #AgsFrontController
   * @authentication_module: the authentication module
   * @login: the login
   * @password: the password
   * @certs: certificate
   * 
   * Authenticate to the front controller.
   * 
   * Returns: on success a new #AgsSecurityContext, otherwise %NULL
   * 
   * Since: 1.0.0
   */
  front_controller_signals[AUTHENTICATE] =
    g_signal_new("authenticate",
		 G_TYPE_FROM_CLASS(front_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFrontControllerClass, authenticate),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__STRING_STRING_STRING_STRING,
		 G_TYPE_POINTER, 4,
		 G_TYPE_STRING,
		 G_TYPE_STRING,
		 G_TYPE_STRING,
		 G_TYPE_STRING);

  /**
   * AgsFrontController::do-request:
   * @front_controller: the #AgsFrontController
   * @security_context: the #AgsSecurityContext
   * @context_path: the context path to access
   * @user: the user's UUID
   * @security_token: the security token
   * @params: the #GParameter-struct
   * 
   * Do a request on the front controller.
   * 
   * Returns: the response
   * 
   * Since: 1.0.0
   */
  front_controller_signals[DO_REQUEST] =
    g_signal_new("do-request",
		 G_TYPE_FROM_CLASS(front_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFrontControllerClass, do_request),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__OBJECT_STRING_STRING_STRING_POINTER,
		 G_TYPE_POINTER, 5,
		 G_TYPE_OBJECT,
		 G_TYPE_STRING,
		 G_TYPE_STRING,
		 G_TYPE_STRING,
		 G_TYPE_POINTER);
}

void
ags_front_controller_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->add_to_registry = ags_front_controller_add_to_registry;
  connectable->remove_from_registry = ags_front_controller_remove_from_registry;
  connectable->connect = ags_front_controller_connect;
  connectable->disconnect = ags_front_controller_disconnect;
}

void
ags_front_controller_init(AgsFrontController *front_controller)
{
  //TODO:JK: implement me
}

void
ags_front_controller_add_to_registry(AgsConnectable *connectable)
{
  AgsServer *server;
  AgsRegistry *registry;
  
  AgsFrontController *front_controller;

  AgsApplicationContext *application_context;
  
  struct xmlrpc_method_info3 *method_info;

  front_controller = AGS_FRONT_CONTROLLER(connectable);
  server = AGS_SERVER(AGS_CONTROLLER(front_controller)->server);

  application_context = server->application_context;

  registry = ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));
  
#ifdef AGS_WITH_XMLRPC_C
  /* bulk */
  method_info = (struct xmlrpc_method_info3 *) malloc(sizeof(struct xmlrpc_method_info3));
  method_info->methodName = "ags_front_controller_xmlrpc_request";
  method_info->methodFunction = &ags_front_controller_xmlrpc_request;
  method_info->serverInfo = NULL;
  xmlrpc_registry_add_method3(ags_service_provider_get_env(AGS_SERVICE_PROVIDER(application_context)),
			      registry->registry,
			      method_info);
#endif /* AGS_WITH_XMLRPC_C */
}

void
ags_front_controller_remove_from_registry(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_front_controller_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_front_controller_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_front_controller_finalize(GObject *gobject)
{
  AgsFrontController *front_controller;

  front_controller = AGS_FRONT_CONTROLLER(gobject);

  G_OBJECT_CLASS(ags_front_controller_parent_class)->finalize(gobject);
}

#ifdef AGS_WITH_XMLRPC_C
xmlrpc_value*
ags_front_controller_xmlrpc_request(xmlrpc_env *env,
				    xmlrpc_value *param_array,
				    void *server_info)
{
  //TODO:JK: implement me
}
#endif

gpointer
ags_front_controller_real_authenticate(AgsFrontController *front_controller,
				       gchar *authentication_module,
				       gchar *login,
				       gchar *password,
				       gchar *certs)
{
  //TODO:JK: implement me
}

/**
 * ags_front_controller_authenticate:
 * @front_controller: the #AgsFrontController
 * @login:
 * @password:
 * @certs:
 *
 * Authenticate to the server.
 *
 * Returns: on success the #GParameter-struct containing user's uuid and security token,
 * otherwise %NULL.
 *
 * Since: 1.0.0
 */
gpointer
ags_front_controller_authenticate(AgsFrontController *front_controller,
				  gchar *authentication_module,
				  gchar *login,
				  gchar *password,
				  gchar *certs)
{
  gpointer retval;

  g_return_val_if_fail(AGS_IS_FRONT_CONTROLLER(front_controller),
		       NULL);

  g_object_ref((GObject *) front_controller);
  g_signal_emit(G_OBJECT(front_controller),
		front_controller_signals[AUTHENTICATE], 0,
		authentication_module,
		login,
		password,
		certs,
		&retval);
  g_object_unref((GObject *) front_controller);

  return(retval);
}

gpointer
ags_front_controller_real_do_request(AgsFrontController *front_controller,
				     GObject *security_context,
				     gchar *context_path,
				     gchar *login,
				     gchar *security_token,
				     GParameter *params)
{
  AgsAuthenticationManager *authentication_manager;
  
  if(context_path == NULL ||
     security_context == NULL ||
     login == NULL ||
     security_token == NULL){
    return(NULL);
  }
  
  //TODO:JK: use certs
  //TODO:JK: implement me
}

/**
 * ags_front_controller_do_request:
 * @front_controller: the #AgsFrontController
 * @security_context: the #AgsSecurityContext
 * @context_path: the context path to access
 * @login: the login
 * @security_token: the security token
 * @certs: the certs
 * @params: the #GParameter-struct containing parameters
 *
 * Do a XML-RPC request for the given @context_path with @params.
 * 
 * Returns: the response, on success the #GParameter-struct containing requested parameters, otherwise %NULL.
 *
 * Since: 1.0.0
 */
gpointer
ags_front_controller_do_request(AgsFrontController *front_controller,
				GObject *security_context,
				gchar *context_path,
				gchar *login,
				gchar *security_token,
				GParameter *params)
{
  gpointer retval;

  g_return_val_if_fail(AGS_IS_FRONT_CONTROLLER(front_controller),
		       NULL);

  g_object_ref((GObject *) front_controller);
  g_signal_emit(G_OBJECT(front_controller),
		front_controller_signals[DO_REQUEST], 0,
		context_path,
		login,
		security_token,
		params,
		&retval);
  g_object_unref((GObject *) front_controller);

  return(retval);
}

AgsFrontController*
ags_front_controller_new()
{
  AgsFrontController *front_controller;

  front_controller = (AgsFrontController *) g_object_new(AGS_TYPE_FRONT_CONTROLLER,
							 NULL);

  return(front_controller);
}
