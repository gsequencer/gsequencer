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
#include <ags/server/security/ags_security_context.h>

#include <ags/server/controller/ags_local_factory_controller.h>

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

gpointer ags_front_controller_delegate_local_factory_controller(AgsFrontController *front_controller,
								AgsLocalFactoryController *local_factory_controller,
								GObject *security_context,
								gchar *context_path,
								gchar *login,
								gchar *security_token,
								GParameter *params,
								guint n_params);

gpointer ags_front_controller_real_do_request(AgsFrontController *front_controller,
					      GObject *security_context,
					      gchar *context_path,
					      gchar *user_uuid,
					      gchar *security_token,
					      GParameter *params,
					      guint n_params);

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
  method_info->serverInfo = server->server_info;
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
  AgsServer *server;
  AgsRegistry *registry;

  AgsSecurityContext *security_context;
  
  AgsFrontController *front_controller;

  GParameter *parameter;
  GType gtype;

  GList *list;

  xmlrpc_value *item;
  gpointer response;

  gchar *param_name;
  gchar *context_path;
  gchar *login, *password, *certs;
  gchar *security_token;
  
  guint entry_count;
  guint n_params;
  guint i;

  auto void ags_front_controller_xmlrpc_request_read_certs();
  auto void ags_front_controller_xmlrpc_request_read_parameter();

  void ags_front_controller_xmlrpc_request_read_certs(){
    xmlrpc_array_read_item(env, param_array, 3, &item);
    xmlrpc_read_string(env, item, &security_token);
    xmlrpc_DECREF(item);

    /*
     * read certs
     */
    if(entry_count > 6){
      /* read parameter name */
      xmlrpc_array_read_item(env, param_array, 6, &item);
      xmlrpc_read_string(env, item, &param_name);
      xmlrpc_DECREF(item);

      /* read certs */
      if(param_name != NULL &&
	 !g_ascii_strncasecmp(param_name,
			      "certs",
			      6)){
	xmlrpc_array_read_item(env, param_array, 7, &item);
	xmlrpc_read_string(env, item, &certs);
	xmlrpc_DECREF(item);
      }

      i = 1;
    }
  }
  
  void ags_front_controller_xmlrpc_request_read_parameter(){    
    /*
     * read parameters
     */
    n_params = entry_count / 2 - 3;

    if(n_params > 0){
      parameter = g_new(GParameter, n_params);
    }else{
      parameter = NULL;
    }
  
    for(; i < n_params; i++){
      AgsRegistryEntry *registry_entry;
      gchar *registry_id;
      gchar *error;

      /* read parameter name */
      xmlrpc_array_read_item(env, param_array, 6 + i * 2, &item);
      xmlrpc_read_string(env, item, &param_name);
      xmlrpc_DECREF(item);

      parameter[i].name = param_name;
      parameter[i].value.g_type = G_TYPE_NONE;

      /* read registry id */
      xmlrpc_array_read_item(env, param_array, 7 + i * 2 + 1, &item);
      xmlrpc_read_string(env, item, &registry_id);
      xmlrpc_DECREF(item);

      /* find registry entry */
      registry_entry = ags_registry_entry_find(registry,
					       registry_id);

      /* copy GValue from registry entry to parameter array */
      g_value_init(&(parameter[i].value), G_VALUE_TYPE(&(registry_entry->entry)));
      g_value_copy(&(registry_entry->entry),
		   &(parameter[i].value));

      /* free not needed strings */
      g_free(param_name);
      g_free(registry_id);

      if(error){
	g_warning ("%s: %s", G_STRFUNC, error);
	g_free (error);
	g_value_unset (&parameter[i].value);
	break;
      }
    }
  }
  
  entry_count = xmlrpc_array_size(env, param_array);
  
  if(entry_count % 2 != 0 &&
     entry_count < 6){
    return(NULL);
  }

  server = ags_server_lookup(server_info);

  registry = ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(server->application_context));

  front_controller = NULL;
  list = ags_list_util_find_type(server->controller,
				 AGS_TYPE_FRONT_CONTROLLER);

  if(list != NULL){
    front_controller = AGS_FRONT_CONTROLLER(list->data);    
  }else{
    return(NULL);
  }

  /*
   * read context path
   */
  /* read parameter name */
  xmlrpc_array_read_item(env, param_array, 0, &item);
  xmlrpc_read_string(env, item, &param_name);
  xmlrpc_DECREF(item);

  /* read context_path */
  if(param_name != NULL &&
     !g_ascii_strncasecmp(param_name,
			  "context-path",
			  11)){
    xmlrpc_array_read_item(env, param_array, 1, &item);
    xmlrpc_read_string(env, item, &context_path);
    xmlrpc_DECREF(item);
  }else{
    return(NULL);
  }

  /*
   * read login
   */
  /* read parameter name */
  xmlrpc_array_read_item(env, param_array, 2, &item);
  xmlrpc_read_string(env, item, &param_name);
  xmlrpc_DECREF(item);

  /* read login */
  if(param_name != NULL &&
     !g_ascii_strncasecmp(param_name,
			  "login",
			  6)){
    xmlrpc_array_read_item(env, param_array, 3, &item);
    xmlrpc_read_string(env, item, &login);
    xmlrpc_DECREF(item);
  }else{
    return(NULL);
  }

  /*
   * read password/security-token
   */
  response = NULL;
  
  /* read parameter name */
  xmlrpc_array_read_item(env, param_array, 4, &item);
  xmlrpc_read_string(env, item, &param_name);
  xmlrpc_DECREF(item);

  certs = NULL;
  i = 0;
  
  /* read password/security-token */
  if(param_name != NULL &&
     !g_ascii_strncasecmp(param_name,
			  "password",
			  9)){
    xmlrpc_array_read_item(env, param_array, 5, &item);
    xmlrpc_read_string(env, item, &password);
    xmlrpc_DECREF(item);

    /*
     * read certs
     */
    certs = NULL;
    
    if(entry_count > 6){
      /* read parameter name */
      xmlrpc_array_read_item(env, param_array, 6, &item);
      xmlrpc_read_string(env, item, &param_name);
      xmlrpc_DECREF(item);

      /* read certs */
      if(param_name != NULL &&
	 !g_ascii_strncasecmp(param_name,
			      "certs",
			      6)){
	xmlrpc_array_read_item(env, param_array, 7, &item);
	xmlrpc_read_string(env, item, &certs);
	xmlrpc_DECREF(item);
      }
    }
    
    response = ags_front_controller_authenticate(front_controller,
						 server->auth_module,
						 login,
						 password,
						 certs);
  }else if(param_name != NULL &&
	   !g_ascii_strncasecmp(param_name,
				"security-token",
				15)){
    ags_front_controller_xmlrpc_request_read_certs();
    
    security_context = ags_security_context_new();
    g_object_set(security_context,
		 "certs", certs,
		 NULL);
    ags_security_context_add_server_context(security_context,
					    context_path);
    g_object_ref(security_context);    

    if(ags_authentication_manager_is_session_active(ags_authentication_manager_get_instance(),
						    security_context,
						    login,
						    security_token)){
      ags_front_controller_xmlrpc_request_read_parameter();
      
      response = ags_front_controller_do_request(front_controller,
						 security_context,
						 context_path,
						 login,
						 security_token,
						 parameter,
						 n_params);
    }

    g_object_unref(security_context);
  }else{
    return(NULL);
  }

  return(response);
}
#endif

gpointer
ags_front_controller_real_authenticate(AgsFrontController *front_controller,
				       gchar *authentication_module,
				       gchar *login,
				       gchar *password,
				       gchar *certs)
{
  gchar *context_path;

  context_path = g_strdup_printf("%s",
				 AGS_CONTROLLER_BASE_PATH);
  g_object_set(front_controller,
	       "context-path", context_path,
	       NULL);
  g_free(context_path);

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
ags_front_controller_delegate_local_factory_controller(AgsFrontController *front_controller,
						       AgsLocalFactoryController *local_factory_controller,
						       GObject *security_context,
						       gchar *context_path,
						       gchar *login,
						       gchar *security_token,
						       GParameter *params,
						       guint n_params)
{
  gpointer response;

  gchar *resource;

  response = NULL;

  resource = strstr(context_path,
		    AGS_CONTROLLER(local_factory_controller)->context_path);
  
  if(n_params < 1 ||
     resource == NULL){
    return(NULL);
  }

  resource += strlen(AGS_CONTROLLER(local_factory_controller)->context_path);
  
  if(ags_controller_query_security_context(local_factory_controller,
					   security_context, login)){
    if(!g_ascii_strncasecmp(resource,
			    AGS_LOCAL_FACTORY_CONTROLLER_RESOURCE_CREATE_INSTANCE,
			    strlen(AGS_LOCAL_FACTORY_CONTROLLER_RESOURCE_CREATE_INSTANCE))){
      if(g_strv_contains(AGS_SECURITY_CONTEXT(security_context)->permitted_context,
			 resource)){
	response = ags_local_factory_controller_create_instance(local_factory_controller,
								g_value_get_ulong(&(params[0].value)),
								((n_params > 1) ? &(params[1]): NULL),
								n_params - 1);
      }
    }
  }

  return(response);
}

gpointer
ags_front_controller_real_do_request(AgsFrontController *front_controller,
				     GObject *security_context,
				     gchar *context_path,
				     gchar *login,
				     gchar *security_token,
				     GParameter *params,
				     guint n_params)
{
  AgsServer *server;
  
  AgsAuthenticationManager *authentication_manager;

  AgsLocalFactoryController *local_factory_controller;

  GList *list;

  gpointer response;
  
  if(context_path == NULL ||
     security_context == NULL ||
     login == NULL ||
     security_token == NULL){
    return(NULL);
  }

  //TODO:JK: use certs

  server = AGS_CONTROLLER(front_controller)->server;

  local_factory_controller = NULL;
  list = ags_list_util_find_type(server->controller,
				 AGS_TYPE_LOCAL_FACTORY_CONTROLLER);

  if(list != NULL){
    local_factory_controller = AGS_LOCAL_FACTORY_CONTROLLER(list->data);    
  }
  
  response = NULL;
  
  if(local_factory_controller != NULL &&
     !g_ascii_strncasecmp(context_path,
			  AGS_CONTROLLER(local_factory_controller)->context_path,
			  strlen(AGS_CONTROLLER(local_factory_controller)->context_path))){
    response = ags_front_controller_delegate_local_factory_controller(front_controller,
								      local_factory_controller,
								      security_context,
								      context_path,
								      login,
								      security_token,
								      params,
								      n_params);
  }

  //TODO:JK: implement me

  return(response);
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
 * @n_params: the count of @params
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
				GParameter *params,
				guint n_params)
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
		n_params,
		&retval);
  g_object_unref((GObject *) front_controller);

  return(retval);
}

/**
 * ags_front__controller_new:
 * 
 * Instantiate new #AgsFrontController
 * 
 * Returns: the #AgsFrontController
 * 
 * Since: 1.0.0
 */
AgsFrontController*
ags_front_controller_new()
{
  AgsFrontController *front_controller;

  front_controller = (AgsFrontController *) g_object_new(AGS_TYPE_FRONT_CONTROLLER,
							 NULL);

  return(front_controller);
}
