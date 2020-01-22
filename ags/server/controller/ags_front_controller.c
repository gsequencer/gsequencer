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

#include <ags/server/controller/ags_front_controller.h>

#include <ags/util/ags_list_util.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_marshal.h>

#include <ags/server/ags_service_provider.h>
#include <ags/server/ags_server.h>
#include <ags/server/ags_registry.h>

#include <ags/server/security/ags_authentication_manager.h>
#include <ags/server/security/ags_security_context.h>

#include <ags/server/controller/ags_plugin_controller.h>

#include <string.h>

void ags_front_controller_class_init(AgsFrontControllerClass *front_controller);
void ags_front_controller_init(AgsFrontController *front_controller);
void ags_front_controller_finalize(GObject *gobject);

gpointer ags_front_controller_real_do_request(AgsFrontController *front_controller,
					      SoupMessage *msg,
					      GHashTable *query,
					      SoupClientContext *client,
					      GObject *security_context,
					      gchar *context_path,
					      gchar *login,
					      gchar *security_token);

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
  DO_REQUEST,
  LAST_SIGNAL,
};

static gpointer ags_front_controller_parent_class = NULL;
static guint front_controller_signals[LAST_SIGNAL];

GType
ags_front_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_front_controller = 0;

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
    
    ags_type_front_controller = g_type_register_static(AGS_TYPE_CONTROLLER,
						       "AgsFrontController",
						       &ags_front_controller_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_front_controller);
  }

  return g_define_type_id__volatile;
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
  front_controller->do_request = ags_front_controller_real_do_request;

  /* signals */
  /**
   * AgsFrontController::do-request:
   * @front_controller: the #AgsFrontController
   * @msg: the #SoupMessage
   * @query: the #GHashTable
   * @client: the #SoupClient
   * @security_context: the #AgsSecurityContext
   * @context_path: the context path to access
   * @user: the user's UUID
   * @security_token: the security token
   * 
   * Do a request on the front controller.
   * 
   * Returns: the response
   * 
   * Since: 3.0.0
   */
  front_controller_signals[DO_REQUEST] =
    g_signal_new("do-request",
		 G_TYPE_FROM_CLASS(front_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFrontControllerClass, do_request),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__OBJECT_POINTER_POINTER_OBJECT_STRING_STRING_STRING,
		 G_TYPE_POINTER, 7,
		 G_TYPE_OBJECT,
		 G_TYPE_POINTER,
		 G_TYPE_POINTER,
		 G_TYPE_OBJECT,
		 G_TYPE_STRING,
		 G_TYPE_STRING,
		 G_TYPE_STRING);
}

void
ags_front_controller_init(AgsFrontController *front_controller)
{
  //TODO:JK: implement me
}

void
ags_front_controller_finalize(GObject *gobject)
{
  AgsFrontController *front_controller;

  front_controller = AGS_FRONT_CONTROLLER(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_front_controller_parent_class)->finalize(gobject);
}

gpointer
ags_front_controller_real_do_request(AgsFrontController *front_controller,
				     SoupMessage *msg,
				     GHashTable *query,
				     SoupClientContext *client,
				     GObject *security_context,
				     gchar *path,
				     gchar *login,
				     gchar *security_token)
{
  AgsServer *server;
  AgsAuthenticationManager *authentication_manager;

  GList *start_controller, *controller;

  gpointer start_response;

  gchar *delimiter;    

  gboolean found_controller;
  
  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     path == NULL ||
     login == NULL ||
     security_token == NULL){
    return(NULL);
  }

  //TODO:JK: use certs

  start_response = NULL;

  g_object_get(front_controller,
	       "server", &server,
	       NULL);

  start_controller = NULL;
  g_object_get(server,
	       "controller", &start_controller,
	       NULL);

  controller = start_controller;

  delimiter = strrchr(path, '/');

  found_controller = FALSE;
  
  if(delimiter != NULL){  
    while(controller != NULL){
      gboolean success;

      GRecMutex *controller_mutex;

      /* get controller mutex */
      controller_mutex = AGS_CONTROLLER_GET_OBJ_MUTEX(controller->data);
    
      /* match path */
      success = FALSE;

      g_rec_mutex_lock(controller_mutex);

      if(AGS_CONTROLLER(controller->data)->context_path != NULL){
	success = !strncmp(AGS_CONTROLLER(controller->data)->context_path,
			   path,
			   delimiter - path - 1);
      }
      
      g_rec_mutex_unlock(controller_mutex);

      if(success){
	if(AGS_IS_PLUGIN_CONTROLLER(controller->data)){
	  found_controller = TRUE;
	  
	  start_response = ags_plugin_controller_do_request(AGS_PLUGIN_CONTROLLER(controller->data),
							    msg,
							    query,
							    client,
							    security_context,
							    path,
							    login,
							    security_token);
	}
      }

      controller = controller->next;
    }
  }

  if(!found_controller){
    soup_message_set_status(msg,
			    200);
    soup_message_set_response(msg,
			      "text/plain",
			      SOUP_MEMORY_STATIC,
			      "OK",
			      2);
  }
  
  return(start_response);
}

/**
 * ags_front_controller_do_request:
 * @front_controller: the #AgsFrontController
 * @msg: the #SoupMessage
 * @query: the #GHashTable
 * @client: the #SoupClientContext
 * @security_context: the #AgsSecurityContext
 * @path: the context path to access
 * @login: the login
 * @security_token: the security token
 *
 * Do a XML-RPC request for the given @path with @query.
 * 
 * Returns: the response on success, otherwise %NULL.
 *
 * Since: 3.0.0
 */
gpointer
ags_front_controller_do_request(AgsFrontController *front_controller,
				SoupMessage *msg,
				GHashTable *query,
				SoupClientContext *client,
				GObject *security_context,
				gchar *path,
				gchar *login,
				gchar *security_token)
{
  gpointer retval;

  g_return_val_if_fail(AGS_IS_FRONT_CONTROLLER(front_controller),
		       NULL);

  g_object_ref((GObject *) front_controller);
  g_signal_emit(G_OBJECT(front_controller),
		front_controller_signals[DO_REQUEST], 0,
		msg,
		query,
		client,
		security_context,
		path,
		login,
		security_token,
		&retval);
  g_object_unref((GObject *) front_controller);

  return(retval);
}

/**
 * ags_front_controller_new:
 * 
 * Instantiate new #AgsFrontController
 * 
 * Returns: the #AgsFrontController
 * 
 * Since: 3.0.0
 */
AgsFrontController*
ags_front_controller_new()
{
  AgsFrontController *front_controller;

  front_controller = (AgsFrontController *) g_object_new(AGS_TYPE_FRONT_CONTROLLER,
							 NULL);

  return(front_controller);
}
