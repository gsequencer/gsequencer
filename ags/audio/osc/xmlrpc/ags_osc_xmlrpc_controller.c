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

#include <ags/audio/osc/xmlrpc/ags_osc_xmlrpc_controller.h>

#include <ags/audio/osc/ags_osc_xmlrpc_server.h>
#include <ags/audio/osc/ags_osc_message.h>
#include <ags/audio/osc/ags_osc_response.h>
#include <ags/audio/osc/ags_osc_util.h>
#include <ags/audio/osc/ags_osc_buffer_util.h>

#include <ags/audio/osc/controller/ags_osc_action_controller.h>
#include <ags/audio/osc/controller/ags_osc_config_controller.h>
#include <ags/audio/osc/controller/ags_osc_controller.h>
#include <ags/audio/osc/controller/ags_osc_info_controller.h>
#include <ags/audio/osc/controller/ags_osc_meter_controller.h>
#include <ags/audio/osc/controller/ags_osc_node_controller.h>
#include <ags/audio/osc/controller/ags_osc_plugin_controller.h>
#include <ags/audio/osc/controller/ags_osc_renew_controller.h>
#include <ags/audio/osc/controller/ags_osc_status_controller.h>

#include <string.h>

#include <ags/i18n.h>

void ags_osc_xmlrpc_controller_class_init(AgsOscXmlrpcControllerClass *osc_xmlrpc_controller);
void ags_osc_xmlrpc_controller_plugin_controller_interface_init(AgsPluginControllerInterface *plugin_controller);
void ags_osc_xmlrpc_controller_init(AgsOscXmlrpcController *osc_xmlrpc_controller);
void ags_osc_xmlrpc_controller_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec);
void ags_osc_xmlrpc_controller_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec);
void ags_osc_xmlrpc_controller_dispose(GObject *gobject);
void ags_osc_xmlrpc_controller_finalize(GObject *gobject);

void* ags_osc_xmlrpc_controller_delegate_thread(void *ptr);

void ags_osc_xmlrpc_controller_real_start_delegate(AgsOscXmlrpcController *osc_xmlrpc_controller);
void ags_osc_xmlrpc_controller_real_stop_delegate(AgsOscXmlrpcController *osc_xmlrpc_controller);

gpointer ags_osc_xmlrpc_controller_do_request(AgsPluginController *plugin_controller,
					      SoupMessage *msg,
					      GHashTable *query,
					      SoupClientContext *client,
					      GObject *security_context,
					      gchar *context_path,
					      gchar *login,
					      gchar *security_token);

enum{
  PROP_0,
  PROP_OSC_XMLRPC_SERVER,
};

enum{
  START_DELEGATE,
  STOP_DELEGATE,
  LAST_SIGNAL,
};

/**
 * SECTION:ags_osc_xmlrpc_controller
 * @short_description: handle OSC XMLRPC requests
 * @title: AgsOscXmlrpcController
 * @section_id:
 * @include: ags/audio/osc/controller/ags_osc_xmlrpc_controller.h
 *
 * The #AgsOscXmlrpcController is an object to handle XMLRPC requests.
 */

static gpointer ags_osc_xmlrpc_controller_parent_class = NULL;
static guint osc_xmlrpc_controller_signals[LAST_SIGNAL];

GType
ags_osc_xmlrpc_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_xmlrpc_controller = 0;

    static const GTypeInfo ags_osc_xmlrpc_controller_info = {
      sizeof (AgsOscXmlrpcControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_xmlrpc_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscXmlrpcController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_xmlrpc_controller_init,
    };

    static const GInterfaceInfo ags_plugin_controller_interface_info = {
      (GInterfaceInitFunc) ags_osc_xmlrpc_controller_plugin_controller_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_osc_xmlrpc_controller = g_type_register_static(AGS_TYPE_CONTROLLER,
							    "AgsOscXmlrpcController",
							    &ags_osc_xmlrpc_controller_info,
							    0);

    g_type_add_interface_static(ags_type_osc_xmlrpc_controller,
				AGS_TYPE_PLUGIN_CONTROLLER,
				&ags_plugin_controller_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_xmlrpc_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_xmlrpc_controller_class_init(AgsOscXmlrpcControllerClass *osc_xmlrpc_controller)
{
  GObjectClass *gobject;
  
  GParamSpec *param_spec;

  ags_osc_xmlrpc_controller_parent_class = g_type_class_peek_parent(osc_xmlrpc_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_xmlrpc_controller;

  gobject->dispose = ags_osc_xmlrpc_controller_dispose;
  gobject->finalize = ags_osc_xmlrpc_controller_finalize;

  /* properties */
  /**
   * AgsOscXmlrpcController:osc-xmlrpc-server:
   *
   * The assigned #AgsOscServer
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("osc-xmlrpc-server",
				   i18n("assigned OSC XMLRPC server"),
				   i18n("The assigned OSC XMLRPC server"),
				   AGS_TYPE_OSC_XMLRPC_SERVER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OSC_XMLRPC_SERVER,
				  param_spec);

  /* AgsOscXmlrpcController */
  osc_xmlrpc_controller->start_delegate = ags_osc_xmlrpc_controller_real_start_delegate;
  osc_xmlrpc_controller->stop_delegate = ags_osc_xmlrpc_controller_real_stop_delegate;

  /* signals */
  /**
   * AgsOscXmlrpcController::start-delegate:
   * @osc_xmlrpc_controller: the #AgsOscXmlrpcController
   *
   * The ::start-delegate signal is emited during start of delegating messages.
   *
   * Since: 3.0.0
   */
  osc_xmlrpc_controller_signals[START_DELEGATE] =
    g_signal_new("start-delegate",
		 G_TYPE_FROM_CLASS(osc_xmlrpc_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscXmlrpcControllerClass, start_delegate),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsOscXmlrpcController::stop-delegate:
   * @osc_xmlrpc_controller: the #AgsOscXmlrpcController
   *
   * The ::stop-delegate signal is emited during stop of delegating messages.
   *
   * Since: 3.0.0
   */
  osc_xmlrpc_controller_signals[STOP_DELEGATE] =
    g_signal_new("stop-delegate",
		 G_TYPE_FROM_CLASS(osc_xmlrpc_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscXmlrpcControllerClass, stop_delegate),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_osc_xmlrpc_controller_plugin_controller_interface_init(AgsPluginControllerInterface *plugin_controller)
{
  plugin_controller->do_request = ags_osc_xmlrpc_controller_do_request;
}

void
ags_osc_xmlrpc_controller_init(AgsOscXmlrpcController *osc_xmlrpc_controller)
{
  gchar *context_path;

  context_path = g_strdup_printf("%s/ags-osc-srv",
				 AGS_CONTROLLER_BASE_PATH);
  
  g_object_set(osc_xmlrpc_controller,
	       "context-path", context_path,
	       NULL);

  g_free(context_path);

  /* OSC XMLRPC server */
  osc_xmlrpc_controller->flags = 0;

  osc_xmlrpc_controller->osc_xmlrpc_server = NULL;

  osc_xmlrpc_controller->delegate_timeout = (struct timespec *) malloc(sizeof(struct timespec));

  osc_xmlrpc_controller->delegate_timeout = 0;

  g_atomic_int_set(&(osc_xmlrpc_controller->do_reset),
		   FALSE);

  g_mutex_init(&(osc_xmlrpc_controller->delegate_mutex));

  g_cond_init(&(osc_xmlrpc_controller->delegate_cond));
  
  osc_xmlrpc_controller->delegate_thread = NULL;

  osc_xmlrpc_controller->message = NULL;
}

void
ags_osc_xmlrpc_controller_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec)
{
  AgsOscXmlrpcController *osc_xmlrpc_controller;

  GRecMutex *controller_mutex;

  osc_xmlrpc_controller = AGS_OSC_XMLRPC_CONTROLLER(gobject);

  /* get osc controller mutex */
  controller_mutex = AGS_CONTROLLER_GET_OBJ_MUTEX(osc_xmlrpc_controller);
  
  switch(prop_id){
  case PROP_OSC_XMLRPC_SERVER:
  {
    AgsOscXmlrpcServer *osc_xmlrpc_server;

    osc_xmlrpc_server = (AgsOscServer *) g_value_get_object(value);

    g_rec_mutex_lock(controller_mutex);

    if(osc_xmlrpc_controller->osc_xmlrpc_server == (GObject *) osc_xmlrpc_server){
      g_rec_mutex_unlock(controller_mutex);

      return;
    }

    if(osc_xmlrpc_controller->osc_xmlrpc_server != NULL){
      g_object_unref(G_OBJECT(osc_xmlrpc_controller->osc_xmlrpc_server));
    }

    if(osc_xmlrpc_server != NULL){
      g_object_ref(G_OBJECT(osc_xmlrpc_server));
    }
      
    osc_xmlrpc_controller->osc_xmlrpc_server = (GObject *) osc_xmlrpc_server;

    g_rec_mutex_unlock(controller_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_xmlrpc_controller_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec)
{
  AgsOscXmlrpcController *osc_xmlrpc_controller;

  GRecMutex *controller_mutex;

  osc_xmlrpc_controller = AGS_OSC_XMLRPC_CONTROLLER(gobject);

  /* get osc controller mutex */
  controller_mutex = AGS_CONTROLLER_GET_OBJ_MUTEX(osc_xmlrpc_controller);
  
  switch(prop_id){
  case PROP_OSC_XMLRPC_SERVER:
  {
    g_rec_mutex_lock(controller_mutex);

    g_value_set_object(value, osc_xmlrpc_controller->osc_xmlrpc_server);

    g_rec_mutex_unlock(controller_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_xmlrpc_controller_dispose(GObject *gobject)
{
  AgsOscXmlrpcController *osc_xmlrpc_controller;

  osc_xmlrpc_controller = AGS_OSC_XMLRPC_CONTROLLER(gobject);
  
  if(osc_xmlrpc_controller->osc_xmlrpc_server != NULL){
    g_object_unref(osc_xmlrpc_controller->osc_xmlrpc_server);

    osc_xmlrpc_controller->osc_xmlrpc_server = NULL;
  }
  
  if(osc_xmlrpc_controller->message != NULL){
    g_list_free_full(osc_xmlrpc_controller->message,
		     (GDestroyNotify) g_object_unref);

    osc_xmlrpc_controller->message = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_osc_xmlrpc_controller_parent_class)->dispose(gobject);
}

void
ags_osc_xmlrpc_controller_finalize(GObject *gobject)
{
  AgsOscXmlrpcController *osc_xmlrpc_controller;

  osc_xmlrpc_controller = AGS_OSC_XMLRPC_CONTROLLER(gobject);
  
  if(osc_xmlrpc_controller->osc_xmlrpc_server != NULL){
    g_object_unref(osc_xmlrpc_controller->osc_xmlrpc_server);
  }

  if(osc_xmlrpc_controller->message != NULL){
    g_list_free_full(osc_xmlrpc_controller->message,
		     (GDestroyNotify) g_object_unref);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_osc_xmlrpc_controller_parent_class)->finalize(gobject);
}

void*
ags_osc_xmlrpc_controller_delegate_thread(void *ptr)
{
  //TODO:JK: implement me
  
  g_thread_exit(NULL);

  return(NULL);
}

/**
 * ags_osc_xmlrpc_controller_test_flags:
 * @osc_xmlrpc_controller: the #AgsOscXmlrpcController
 * @flags: the flags
 *
 * Test @flags to be set on @osc_xmlrpc_controller.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_osc_xmlrpc_controller_test_flags(AgsOscXmlrpcController *osc_xmlrpc_controller, guint flags)
{
  gboolean retval;  
  
  GRecMutex *controller_mutex;

  if(!AGS_IS_OSC_XMLRPC_CONTROLLER(osc_xmlrpc_controller)){
    return(FALSE);
  }

  /* get OSC xmlrpc controller mutex */
  controller_mutex = AGS_CONTROLLER_GET_OBJ_MUTEX(osc_xmlrpc_controller);

  /* test */
  g_rec_mutex_lock(controller_mutex);

  retval = (flags & (osc_xmlrpc_controller->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(controller_mutex);

  return(retval);
}

/**
 * ags_osc_xmlrpc_controller_set_flags:
 * @osc_xmlrpc_controller: the #AgsOscXmlrpcController
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 3.0.0
 */
void
ags_osc_xmlrpc_controller_set_flags(AgsOscXmlrpcController *osc_xmlrpc_controller, guint flags)
{
  GRecMutex *controller_mutex;

  if(!AGS_IS_OSC_XMLRPC_CONTROLLER(osc_xmlrpc_controller)){
    return;
  }

  /* get OSC xmlrpc controller mutex */
  controller_mutex = AGS_CONTROLLER_GET_OBJ_MUTEX(osc_xmlrpc_controller);
  
  /* set flags */
  g_rec_mutex_lock(controller_mutex);

  osc_xmlrpc_controller->flags |= flags;

  g_rec_mutex_unlock(controller_mutex);
}

/**
 * ags_osc_xmlrpc_controller_unset_flags:
 * @osc_xmlrpc_controller: the #AgsOscXmlrpcController
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 3.0.0
 */
void
ags_osc_xmlrpc_controller_unset_flags(AgsOscXmlrpcController *osc_xmlrpc_controller, guint flags)
{
  GRecMutex *controller_mutex;

  if(!AGS_IS_OSC_XMLRPC_CONTROLLER(osc_xmlrpc_controller)){
    return;
  }

  /* get OSC xmlrpc controller mutex */
  controller_mutex = AGS_CONTROLLER_GET_OBJ_MUTEX(osc_xmlrpc_controller);
  
  /* set flags */
  g_rec_mutex_lock(controller_mutex);

  osc_xmlrpc_controller->flags &= (~flags);

  g_rec_mutex_unlock(controller_mutex);
}

/**
 * ags_osc_xmlrpc_controller_add_message:
 * @osc_xmlrpc_controller: the #AgsOscXmlrpcController
 * @message: the #AgsOscMessage
 * 
 * Add @message to @osc_xmlrpc_controller.
 * 
 * Since: 3.0.0
 */
void
ags_osc_xmlrpc_controller_add_message(AgsOscXmlrpcController *osc_xmlrpc_controller,
				      GObject *message)
{
  GRecMutex *controller_mutex;

  if(!AGS_IS_OSC_XMLRPC_CONTROLLER(osc_xmlrpc_controller) ||
     message == NULL){
    return;
  }

  /* get OSC xmlrpc controller mutex */
  controller_mutex = AGS_CONTROLLER_GET_OBJ_MUTEX(osc_xmlrpc_controller);
  
  /* add */
  g_rec_mutex_lock(controller_mutex);

  if(g_list_find(osc_xmlrpc_controller->message, message) == NULL){
    osc_xmlrpc_controller->message = g_list_insert_sorted(osc_xmlrpc_controller->message,
							  message,
							  ags_osc_message_sort_func);
    g_object_ref(message);
  }
  
  g_rec_mutex_unlock(controller_mutex);
}

/**
 * ags_osc_xmlrpc_controller_remove_message:
 * @osc_xmlrpc_controller: the #AgsOscXmlrpcController
 * @message: the #AgsOscMessage
 * 
 * Remove @message from @osc_xmlrpc_controller.
 * 
 * Since: 3.0.0
 */
void
ags_osc_xmlrpc_controller_remove_message(AgsOscXmlrpcController *osc_xmlrpc_controller,
					 GObject *message)
{
  GRecMutex *controller_mutex;

  if(!AGS_IS_OSC_XMLRPC_CONTROLLER(osc_xmlrpc_controller) ||
     message == NULL){
    return;
  }

  /* get OSC xmlrpc controller mutex */
  controller_mutex = AGS_CONTROLLER_GET_OBJ_MUTEX(osc_xmlrpc_controller);

  /* remove */
  g_rec_mutex_lock(controller_mutex);

  if(g_list_find(osc_xmlrpc_controller->message, message) != NULL){
    osc_xmlrpc_controller->message = g_list_remove(osc_xmlrpc_controller->message,
						   message);
    g_object_unref(message);
  }
  
  g_rec_mutex_unlock(controller_mutex);
}

void
ags_osc_xmlrpc_controller_real_start_delegate(AgsOscXmlrpcController *osc_xmlrpc_controller)
{
  GRecMutex *controller_mutex;

  /* get OSC xmlrpc controller mutex */
  controller_mutex = AGS_CONTROLLER_GET_OBJ_MUTEX(osc_xmlrpc_controller);

  /* test if already started */
  g_rec_mutex_lock(controller_mutex);
    
  if(ags_osc_xmlrpc_controller_test_flags(osc_xmlrpc_controller, AGS_OSC_XMLRPC_CONTROLLER_DELEGATE_STARTED)){
    g_rec_mutex_unlock(controller_mutex);
    
    return;
  }

  ags_osc_xmlrpc_controller_set_flags(osc_xmlrpc_controller, AGS_OSC_XMLRPC_CONTROLLER_DELEGATE_STARTED);
  
  g_rec_mutex_unlock(controller_mutex);

  /* create delegate thread */
  osc_xmlrpc_controller->delegate_thread = g_thread_new("Advanced Gtk+ Sequencer OSC Server - delegate thread",
							ags_osc_xmlrpc_controller_delegate_thread,
							osc_xmlrpc_controller);
}

/**
 * ags_osc_xmlrpc_controller_start_delegate:
 * @osc_xmlrpc_controller: the #AgsOscXmlrpcController
 * 
 * Start delegating.
 * 
 * Since: 3.0.0
 */
void
ags_osc_xmlrpc_controller_start_delegate(AgsOscXmlrpcController *osc_xmlrpc_controller)
{
  g_return_if_fail(AGS_IS_OSC_XMLRPC_CONTROLLER(osc_xmlrpc_controller));
  
  g_object_ref((GObject *) osc_xmlrpc_controller);
  g_signal_emit(G_OBJECT(osc_xmlrpc_controller),
		osc_xmlrpc_controller_signals[START_DELEGATE], 0);
  g_object_unref((GObject *) osc_xmlrpc_controller);
}

void
ags_osc_xmlrpc_controller_real_stop_delegate(AgsOscXmlrpcController *osc_xmlrpc_controller)
{
  if(!ags_osc_xmlrpc_controller_test_flags(osc_xmlrpc_controller, AGS_OSC_XMLRPC_CONTROLLER_DELEGATE_RUNNING)){
    return;
  }

  ags_osc_xmlrpc_controller_set_flags(osc_xmlrpc_controller, AGS_OSC_XMLRPC_CONTROLLER_DELEGATE_TERMINATING);
  ags_osc_xmlrpc_controller_unset_flags(osc_xmlrpc_controller, AGS_OSC_XMLRPC_CONTROLLER_DELEGATE_RUNNING);

  /* join thread */
  g_thread_join(osc_xmlrpc_controller->delegate_thread);
  
  ags_osc_xmlrpc_controller_unset_flags(osc_xmlrpc_controller, (AGS_OSC_XMLRPC_CONTROLLER_DELEGATE_TERMINATING |
								AGS_OSC_XMLRPC_CONTROLLER_DELEGATE_STARTED));
}

/**
 * ags_osc_xmlrpc_controller_stop_delegate:
 * @osc_xmlrpc_controller: the #AgsOscXmlrpcController
 * 
 * Stop delegating.
 * 
 * Since: 3.0.0
 */
void
ags_osc_xmlrpc_controller_stop_delegate(AgsOscXmlrpcController *osc_xmlrpc_controller)
{
  g_return_if_fail(AGS_IS_OSC_XMLRPC_CONTROLLER(osc_xmlrpc_controller));
  
  g_object_ref((GObject *) osc_xmlrpc_controller);
  g_signal_emit(G_OBJECT(osc_xmlrpc_controller),
		osc_xmlrpc_controller_signals[STOP_DELEGATE], 0);
  g_object_unref((GObject *) osc_xmlrpc_controller);
}

gpointer
ags_osc_xmlrpc_controller_do_request(AgsPluginController *plugin_controller,
				     SoupMessage *msg,
				     GHashTable *query,
				     SoupClientContext *client,
				     GObject *security_context,
				     gchar *path,
				     gchar *login,
				     gchar *security_token)
{
  gpointer start_response;
  
  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     path == NULL ||
     login == NULL ||
     security_token == NULL){
    return(NULL);
  }

  //TODO:JK: use certs

  start_response = NULL;
  
  return(start_response);
}

/**
 * ags_osc_xmlrpc_controller_new:
 * 
 * Instantiate new #AgsOscXmlrpcController
 * 
 * Returns: the #AgsOscXmlrpcController
 * 
 * Since: 3.0.0
 */
AgsOscXmlrpcController*
ags_osc_xmlrpc_controller_new()
{
  AgsOscXmlrpcController *osc_xmlrpc_controller;

  osc_xmlrpc_controller = (AgsOscXmlrpcController *) g_object_new(AGS_TYPE_OSC_XMLRPC_CONTROLLER,
								  NULL);

  return(osc_xmlrpc_controller);
}
