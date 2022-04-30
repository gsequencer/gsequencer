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
#include <ags/audio/osc/ags_osc_xmlrpc_message.h>
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

#ifdef __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

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

gboolean ags_osc_xmlrpc_controller_delegate_timeout(AgsOscXmlrpcController *osc_xmlrpc_controller);

void ags_osc_xmlrpc_controller_real_start_delegate(AgsOscXmlrpcController *osc_xmlrpc_controller);
void ags_osc_xmlrpc_controller_real_stop_delegate(AgsOscXmlrpcController *osc_xmlrpc_controller);

gsize ags_osc_xmlrpc_controller_read_bundle(AgsOscXmlrpcController *osc_xmlrpc_controller,
					    AgsOscWebsocketConnection *osc_websocket_connection,
					    SoupMessage *msg,
					    GHashTable *query,
					    guchar *packet, gsize packet_size,
					    gsize offset);
gsize ags_osc_xmlrpc_controller_read_message(AgsOscXmlrpcController *osc_xmlrpc_controller,
					     AgsOscWebsocketConnection *osc_websocket_connection,
					     SoupMessage *msg,
					     GHashTable *query,
					     guchar *packet, gsize packet_size,
					     gsize offset,
					     gint32 tv_sec, gint32 tv_fraction, gboolean immediately);

gpointer ags_osc_xmlrpc_controller_do_request(AgsPluginController *plugin_controller,
					      SoupServerMessage *msg,
					      GHashTable *query,
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

  gobject->set_property = ags_osc_xmlrpc_controller_set_property;
  gobject->get_property = ags_osc_xmlrpc_controller_get_property;

  gobject->dispose = ags_osc_xmlrpc_controller_dispose;
  gobject->finalize = ags_osc_xmlrpc_controller_finalize;

  /* properties */
  /**
   * AgsOscXmlrpcController:osc-xmlrpc-server:
   *
   * The assigned #AgsOscXmlrpcServer
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

  context_path = g_strdup_printf("%s/ags-osc-over-xmlrpc",
				 AGS_CONTROLLER_BASE_PATH);
  
  g_object_set(osc_xmlrpc_controller,
	       "context-path", context_path,
	       NULL);

  g_free(context_path);

  /* OSC XMLRPC server */
  osc_xmlrpc_controller->flags = 0;

  osc_xmlrpc_controller->osc_xmlrpc_server = NULL;

  g_atomic_int_set(&(osc_xmlrpc_controller->do_reset),
		   FALSE);

  g_mutex_init(&(osc_xmlrpc_controller->delegate_mutex));

  g_cond_init(&(osc_xmlrpc_controller->delegate_cond));
  
  osc_xmlrpc_controller->message = NULL;

  osc_xmlrpc_controller->queued_response = NULL;
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

gboolean
ags_osc_xmlrpc_controller_delegate_timeout(AgsOscXmlrpcController *osc_xmlrpc_controller)
{
  AgsOscXmlrpcServer *osc_xmlrpc_server;

  AgsOscMessage *current;

  GList *start_osc_response, *osc_response;      
  GList *start_message, *message;
  GList *start_list, *list;
  GList *start_controller, *controller;
  
  gint64 time_now, time_next;
  gint64 current_time;
  
  GRecMutex *controller_mutex;

  g_object_get(osc_xmlrpc_controller,
	       "osc-xmlrpc-server", &osc_xmlrpc_server,
	       NULL);

  g_object_get(osc_xmlrpc_server,
	       "controller", &start_controller,
	       NULL);
  
  /* get OSC xmlrpc controller mutex */
  controller_mutex = AGS_CONTROLLER_GET_OBJ_MUTEX(osc_xmlrpc_controller);

  time_next = 0;

  /*  */
  g_mutex_lock(&(osc_xmlrpc_controller->delegate_mutex));

  time_now = g_get_monotonic_time();

  g_atomic_int_set(&(osc_xmlrpc_controller->do_reset),
		   FALSE);
    
  g_mutex_unlock(&(osc_xmlrpc_controller->delegate_mutex));


  /* check queued response */
  g_rec_mutex_lock(controller_mutex);

  start_osc_response = g_list_copy_deep(osc_xmlrpc_controller->queued_response,
					(GCopyFunc) g_object_ref,
					NULL);
	      
  g_rec_mutex_unlock(controller_mutex);
    
  osc_response = start_osc_response;
      
  while(osc_response != NULL){
    gint64 num_written;

    gboolean has_expired;

    GRecMutex *response_mutex;
      
    static const struct timespec timeout_delay = {
      30,
      0,
    };
      
    g_object_get(osc_response->data,
		 "osc-message", &current,
		 NULL);
	    
    num_written = ags_osc_connection_write_response(current->osc_connection,
						    osc_response->data);

    g_object_unref(current);

    response_mutex = AGS_OSC_RESPONSE_GET_OBJ_MUTEX(osc_response->data);

    g_rec_mutex_lock(response_mutex);
      
    has_expired = ags_time_timeout_expired(AGS_OSC_RESPONSE(osc_response->data)->creation_time, &timeout_delay);

    g_rec_mutex_unlock(response_mutex);
      
    if(num_written != -1 ||
       has_expired){
      g_rec_mutex_lock(controller_mutex);

      osc_xmlrpc_controller->queued_response = g_list_remove(osc_xmlrpc_controller->queued_response,
							     osc_response->data);
      g_object_unref(osc_response->data);
	      
      g_rec_mutex_unlock(controller_mutex);
    }
	    
    osc_response = osc_response->next;
  }    
	    
  g_list_free_full(start_osc_response,
		   g_object_unref);
    
  /* check delegate */
  g_rec_mutex_lock(controller_mutex);

  start_message = NULL;
  list =
    start_list = g_list_copy_deep(osc_xmlrpc_controller->message,
				  (GCopyFunc) g_object_ref,
				  NULL);
    
  g_rec_mutex_unlock(controller_mutex);

  while(list != NULL){
    gboolean immediately;
      
    GRecMutex *message_mutex;

    message_mutex = AGS_OSC_MESSAGE_GET_OBJ_MUTEX(list->data);

    g_object_get(list->data,
		 "immediately", &immediately,
		 NULL);
      
    if(immediately){
      start_message = g_list_prepend(start_message,
				     list->data);
      g_object_ref(list->data);
	
      ags_osc_xmlrpc_controller_remove_message(osc_xmlrpc_controller,
					       list->data);
    }else{
      g_rec_mutex_lock(message_mutex);
	
      current_time = AGS_OSC_MESSAGE(list->data)->tv_sec + AGS_OSC_MESSAGE(list->data)->tv_fraction / 4.294967296 * 1000.0;

      g_rec_mutex_unlock(message_mutex);
      
      if(current_time < time_now){
	start_message = g_list_prepend(start_message,
				       list->data);
	g_object_ref(list->data);
	  
	ags_osc_xmlrpc_controller_remove_message(osc_xmlrpc_controller,
						 list->data);
      }else{
	break;
      }
    }

    list = list->next;
  }

  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);
    
  message = 
    start_message = g_list_reverse(start_message);

  while(message != NULL){
    gchar *path;

    ags_osc_buffer_util_get_string(AGS_OSC_MESSAGE(message->data)->message,
				   &path, NULL);

    controller = start_controller;
    start_osc_response = NULL;
      
    while(controller != NULL){
      gboolean success;
	
      GRecMutex *mutex;

      /* get OSC controller mutex */
      mutex = AGS_OSC_CONTROLLER_GET_OBJ_MUTEX(controller->data);

      /* match path */
      g_rec_mutex_lock(mutex);
	
      success = !g_strcmp0(AGS_OSC_CONTROLLER(controller->data)->context_path,
			   path);

      g_rec_mutex_unlock(mutex);
	
      if(success){
	current = AGS_OSC_MESSAGE(message->data);
	  
	/* delegate */
	if(AGS_IS_OSC_ACTION_CONTROLLER(controller->data)){
	  start_osc_response = ags_osc_action_controller_run_action(controller->data,
								    current->osc_connection,
								    current->message, current->message_size);
	}else if(AGS_IS_OSC_CONFIG_CONTROLLER(controller->data)){
	  start_osc_response = ags_osc_config_controller_apply_config(controller->data,
								      current->osc_connection,
								      current->message, current->message_size);
	}else if(AGS_IS_OSC_INFO_CONTROLLER(controller->data)){
	  start_osc_response = ags_osc_info_controller_get_info(controller->data,
								current->osc_connection,
								current->message, current->message_size);
	}else if(AGS_IS_OSC_METER_CONTROLLER(controller->data)){
	  start_osc_response = ags_osc_meter_controller_monitor_meter(controller->data,
								      current->osc_connection,
								      current->message, current->message_size);
	}else if(AGS_IS_OSC_NODE_CONTROLLER(controller->data)){
	  start_osc_response = ags_osc_node_controller_get_data(controller->data,
								current->osc_connection,
								current->message, current->message_size);
	}else if(AGS_IS_OSC_RENEW_CONTROLLER(controller->data)){
	  start_osc_response = ags_osc_renew_controller_set_data(controller->data,
								 current->osc_connection,
								 current->message, current->message_size);
	}else if(AGS_IS_OSC_STATUS_CONTROLLER(controller->data)){
	  start_osc_response = ags_osc_status_controller_get_status(controller->data,
								    current->osc_connection,
								    current->message, current->message_size);
	}else if(AGS_IS_OSC_PLUGIN_CONTROLLER(controller->data)){
	  start_osc_response = ags_osc_plugin_controller_do_request(AGS_OSC_PLUGIN_CONTROLLER(controller->data),
								    current->osc_connection,
								    current->message, current->message_size);
	}

	/* write response */
	osc_response = start_osc_response;
      
	while(osc_response != NULL){
	  gint64 num_written;

#ifdef __APPLE__
	  clock_serv_t cclock;
	  mach_timespec_t mts;
#endif
	    
	  GRecMutex *response_mutex;
      
	  num_written = ags_osc_connection_write_response(current->osc_connection,
							  osc_response->data);

	  if(num_written == -1){
	    response_mutex = AGS_OSC_RESPONSE_GET_OBJ_MUTEX(osc_response->data);

	    g_rec_mutex_lock(response_mutex);
      
	    AGS_OSC_RESPONSE(osc_response->data)->creation_time = (struct timespec *) malloc(sizeof(struct timespec));

#ifdef __APPLE__
	    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
      
	    clock_get_time(cclock, &mts);
	    mach_port_deallocate(mach_task_self(), cclock);
      
	    AGS_OSC_RESPONSE(osc_response->data)->creation_time->tv_sec = mts.tv_sec;
	    AGS_OSC_RESPONSE(osc_response->data)->creation_time->tv_nsec = mts.tv_nsec;
#else
	    clock_gettime(CLOCK_MONOTONIC, AGS_OSC_RESPONSE(osc_response->data)->creation_time);
#endif

	    g_rec_mutex_unlock(response_mutex);
	      
	    g_object_set(osc_response->data,
			 "osc-message", current,
			 NULL);
	      
	    g_rec_mutex_lock(controller_mutex);

	    osc_xmlrpc_controller->queued_response = g_list_prepend(osc_xmlrpc_controller->queued_response,
								    osc_response->data);
	    g_object_ref(osc_response->data);
	      
	    g_rec_mutex_unlock(controller_mutex);
	  }
	    
	  osc_response = osc_response->next;
	}
	    
	g_list_free_full(start_osc_response,
			 g_object_unref);
	  
	break;
      }
	
      controller = controller->next;
    }

    message = message->next;
  }
    
  /* free messages */
  g_list_free_full(start_message,
		   (GDestroyNotify) g_object_unref);

  /* next */
  g_mutex_lock(&(osc_xmlrpc_controller->delegate_mutex));

  if(osc_xmlrpc_controller->message != NULL){
    time_next = AGS_OSC_MESSAGE(osc_xmlrpc_controller->message)->tv_sec + AGS_OSC_MESSAGE(osc_xmlrpc_controller->message)->tv_fraction / 4.294967296 * 1000.0;
  }else{
    time_now = g_get_monotonic_time();

    time_next = time_now + G_TIME_SPAN_SECOND / 30;
  }
    
  g_mutex_unlock(&(osc_xmlrpc_controller->delegate_mutex));

  g_object_unref(osc_xmlrpc_server);
  
  g_list_free_full(start_controller,
		   g_object_unref);
  
  if(!ags_osc_xmlrpc_controller_test_flags(osc_xmlrpc_controller, AGS_OSC_XMLRPC_CONTROLLER_DELEGATE_RUNNING)){
    return(G_SOURCE_REMOVE);
  }
    
  return(G_SOURCE_CONTINUE);
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
     !AGS_IS_OSC_MESSAGE(message)){
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
     !AGS_IS_OSC_MESSAGE(message)){
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
  AgsConfig *config;

  GMainContext *main_context;
  GSource *timeout_source;
  
  gchar *str;

  gdouble delegate_timeout;

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

  config = ags_config_get_instance();
  
  /* delegate timeout */
  ags_osc_xmlrpc_controller_set_flags(osc_xmlrpc_controller,
				      AGS_OSC_XMLRPC_CONTROLLER_DELEGATE_RUNNING);
  
  delegate_timeout = AGS_OSC_XMLRPC_CONTROLLER_DEFAULT_DELEGATE_TIMEOUT;

  str = ags_config_get_value(config,
			     AGS_CONFIG_OSC_SERVER,
			     "delegate-timeout");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_OSC_SERVER_0,
			       "delegate-timeout");
  }
  
  if(str != NULL){
    delegate_timeout = g_ascii_strtod(str,
				     NULL);
    
    free(str);
  }

  /* create delegate timeout */
  main_context = g_main_context_get_thread_default();

  timeout_source = g_timeout_source_new(delegate_timeout * G_TIME_SPAN_MILLISECOND);
  g_source_set_callback(timeout_source,
			ags_osc_xmlrpc_controller_delegate_timeout,
			osc_xmlrpc_controller,
			NULL);
  g_source_attach(timeout_source,
		  main_context);
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

gsize
ags_osc_xmlrpc_controller_read_bundle(AgsOscXmlrpcController *osc_xmlrpc_controller,
				      AgsOscWebsocketConnection *osc_websocket_connection,
				      SoupMessage *msg,
				      GHashTable *query,
				      guchar *packet, gsize packet_size,
				      gsize offset)
{
  gint32 tv_sec;
  gint32 tv_fraction;
  gboolean immediately;
  gsize read_count;
  gint32 length;

  read_count = 8;
    
  ags_osc_buffer_util_get_timetag(packet + offset + read_count,
				  &(tv_sec), &(tv_fraction), &(immediately));
  read_count += 8;

  for(; offset < packet_size;){
    ags_osc_buffer_util_get_int32(packet + offset + read_count,
				  &length);
    read_count += 4;

    if(!g_strcmp0(packet + offset + read_count, "#bundle")){      
      ags_osc_xmlrpc_controller_read_bundle(osc_xmlrpc_controller,
					    osc_websocket_connection,
					    msg,
					    query,
					    packet, packet_size,
					    offset + read_count);

      read_count += ((gsize) 4 * ceil((double) length / 4.0));
    }else if(packet[offset + read_count] == '/'){
      ags_osc_xmlrpc_controller_read_message(osc_xmlrpc_controller,
					     osc_websocket_connection,
					     msg,
					     query,
					     packet, packet_size,
					     offset + read_count,
					     tv_sec, tv_fraction, immediately);

      read_count += ((gsize) 4 * ceil((double) length / 4.0));
    }else{
      read_count += 1;
	
      g_warning("malformed data");
    }
  }
    
  return(read_count);
}

gsize
ags_osc_xmlrpc_controller_read_message(AgsOscXmlrpcController *osc_xmlrpc_controller,
				       AgsOscWebsocketConnection *osc_websocket_connection,
				       SoupMessage *msg,
				       GHashTable *query,
				       guchar *packet, gsize packet_size,
				       gsize offset,
				       gint32 tv_sec, gint32 tv_fraction, gboolean immediately)
{
  AgsOscMessage *osc_message;

  guchar *message;
  gchar *address_pattern;
  gchar *type_tag;
  
  gsize address_pattern_length;
  gsize type_tag_length;
  gsize data_length;
  gsize read_count;
  guint i;

  read_count = 0;

  ags_osc_buffer_util_get_string(packet + offset,
				 &address_pattern, &address_pattern_length);
    
  if(address_pattern == NULL){
    return(0);
  }
    
  read_count += (4 * (gsize) ceil((double) (address_pattern_length + 1) / 4.0));
  free(address_pattern);
    
  type_tag = NULL;

  if(packet_size > offset + read_count){
    if(packet[offset + read_count] == ','){
      ags_osc_buffer_util_get_string(packet + offset + read_count,
				     &type_tag, &type_tag_length);

      read_count += (4 * (gsize) ceil((double) (type_tag_length + 1) / 4.0));
    }
  }

  data_length = 0;
    
  if(type_tag != NULL){
    for(i = 1; i < type_tag_length; i++){
      switch(type_tag[i]){
      case AGS_OSC_UTIL_TYPE_TAG_STRING_TRUE:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_FALSE:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_NIL:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_INFINITE:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_ARRAY_START:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_ARRAY_END:
      {
	//empty
      }
      break;
      case AGS_OSC_UTIL_TYPE_TAG_STRING_CHAR:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_INT32:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_FLOAT:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_RGBA:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_MIDI:
      {
	data_length += 4;
      }
      break;
      case AGS_OSC_UTIL_TYPE_TAG_STRING_INT64:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_DOUBLE:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_TIMETAG:
      {
	data_length += 8;
      }
      break;
      case AGS_OSC_UTIL_TYPE_TAG_STRING_SYMBOL:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_STRING:
      {
	gsize length;

	length = strlen(packet + offset + read_count + data_length);

	data_length += (4 * (gsize) ceil((double) (length + 1) / 4.0));
      }
      break;
      case AGS_OSC_UTIL_TYPE_TAG_STRING_BLOB:
      {
	gint32 data_size;

	ags_osc_buffer_util_get_int32(packet + offset + read_count + data_length,
				      &data_size);

	data_length += data_size;
      }
      break;
      }
    }

    free(type_tag);
  }

  read_count += (4 * (gsize) ceil((double) data_length / 4.0));

  osc_message = ags_osc_xmlrpc_message_new();

  message = (guchar *) malloc(read_count * sizeof(guchar));
  memcpy(message,
	 packet + offset,
	 read_count * sizeof(guchar));

  g_object_set(osc_message,
	       "osc-connection", osc_websocket_connection,
	       "tv-sec", tv_sec,
	       "tv-fraction", tv_fraction,
	       "immediately", immediately,
	       "message-size", read_count,
	       "message", message,
	       "msg", msg,
	       "query", query,
	       NULL);    

  ags_osc_xmlrpc_controller_add_message(osc_xmlrpc_controller,
					osc_message);
    
  return(read_count);
}

gpointer
ags_osc_xmlrpc_controller_do_request(AgsPluginController *plugin_controller,
				     SoupServerMessage *msg,
				     GHashTable *query,
				     GObject *security_context,
				     gchar *path,
				     gchar *login,
				     gchar *security_token)
{
  AgsOscXmlrpcServer *osc_xmlrpc_server;
  AgsOscXmlrpcController *osc_xmlrpc_controller;
  AgsOscWebsocketConnection *osc_websocket_connection;
  
  xmlDoc *doc;
  xmlDoc *response_doc;
  xmlNode *root_node;
  xmlNode *osc_packet_node_list;
  xmlNode *osc_packet_node;
  xmlNode *response_root_node;
  xmlNode *response_redirect_node;

#if HAVE_GLIB_2_52  
#else  
  AgsUUID *resource_uuid;
#endif
  
  GBytes *request_body_data;

  xmlChar *buffer;
  gchar *data;
  guchar *packet;
  xmlChar *response_buffer;
  gchar *response_resource_id;

  gsize data_size;
  gsize packet_size;
  gint32 tv_sec;
  gint32 tv_fraction;
  gboolean immediately;
  gsize offset;
  int response_buffer_length;
  
  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     path == NULL ||
     login == NULL ||
     security_token == NULL){
    return(NULL);
  }

  g_object_get(msg,
	       "request-body-data", &request_body_data,
	       NULL);

  data = g_bytes_get_data(request_body_data,
			  &data_size);

  /* parse XML doc */
  buffer = xmlCharStrndup(data,
			  data_size);
  
  doc = xmlParseDoc(buffer);

  xmlFree(buffer);
  
  if(doc == NULL){
    return(NULL);
  }

  root_node = xmlDocGetRootElement(doc);

  if(root_node == NULL){
    return(NULL);
  }

  /*  */
  osc_xmlrpc_controller = AGS_OSC_XMLRPC_CONTROLLER(plugin_controller);
  
  g_object_get(osc_xmlrpc_controller,
	       "osc-xmlrpc-server", &osc_xmlrpc_server,
	       NULL);

#if HAVE_GLIB_2_52  
  response_resource_id = g_uuid_string_random();	    
#else
  resource_uuid = ags_uuid_alloc();
  ags_uuid_generate(resource_uuid);
  
  response_resource_id = ags_uuid_to_string(resource_uuid);

  ags_uuid_free(resource_uuid);
#endif

  osc_websocket_connection = ags_osc_websocket_connection_new(osc_xmlrpc_server);
  g_object_set(osc_websocket_connection,
	       "security-context", security_context,
	       "login", login,
	       "security-token", security_token,
	       "resource-id", response_resource_id,
	       NULL);

  ags_osc_server_add_connection(osc_xmlrpc_server,
				osc_websocket_connection);
  
  osc_packet_node_list = root_node->children;

  while(osc_packet_node_list != NULL){
    if(osc_packet_node_list->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(osc_packet_node_list->name,
		     "ags-osc-packet-list",
		     20)){
	osc_packet_node = osc_packet_node_list->children;
	
	while(osc_packet_node != NULL){
	  if(osc_packet_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(osc_packet_node->name,
			   "ags-osc-packet",
			   15)){
	      xmlChar *tmp_packet;

	      tmp_packet = xmlNodeGetContent(osc_packet_node);

	      packet = g_base64_decode(tmp_packet,
				       &packet_size);

	      tv_sec = 0;
	      tv_fraction = 0;
	      immediately = TRUE;

	      for(offset = 4; offset < packet_size;){
		gsize read_count;

#ifdef AGS_DEBUG    
		g_message("%d %d", offset, packet_size);
		g_message("%x[%c]", packet[offset], packet[offset]);
#endif
    
		if(!g_strcmp0(packet + offset, "#bundle")){      
		  read_count = ags_osc_xmlrpc_controller_read_bundle(osc_xmlrpc_controller,
								     osc_websocket_connection,
								     msg,
								     query,
								     packet, packet_size,
								     offset);
		}else if(packet[offset] == '/'){
		  read_count = ags_osc_xmlrpc_controller_read_message(osc_xmlrpc_controller,
								      osc_websocket_connection,
								      msg,
								      query,
								      packet, packet_size,
								      offset,
								      0, 0, TRUE);
		}else{
		  read_count = 1;

		  g_warning("malformed data");
		}

		if(read_count > 0){
		  offset += ((gsize) 4 * ceil((double) read_count / 4.0));
		}else{
		  offset += 1;
      
		  g_warning("malformed data");
		}
	      }

	      g_free(packet);
	    }
	  }
    
	  osc_packet_node = osc_packet_node->next;
	}
      }
    }
    
    osc_packet_node_list = osc_packet_node_list->next;
  }

  xmlFreeDoc(doc);

  /* response */
  g_message("PATH=%s", path);
  response_doc = xmlNewDoc("1.0");
	    
  response_root_node = xmlNewNode(NULL, "ags-osc-over-xmlrpc");
  xmlDocSetRootElement(response_doc, response_root_node);

  response_redirect_node = xmlNewNode(NULL,
				      "ags-srv-redirect");

  xmlNewProp(response_redirect_node,
	     "resource-id",
	     response_resource_id);

  xmlAddChild(response_root_node,
	      response_redirect_node);
  
  /* set body */
  xmlDocDumpFormatMemoryEnc(response_doc, &response_buffer, &response_buffer_length, "UTF-8", TRUE);

  soup_server_message_set_response(msg,
				   "text/xml; charset=UTF-8",
				   SOUP_MEMORY_COPY,
				   response_buffer,
				   response_buffer_length);

  soup_server_message_set_status(msg,
				 200,
				 NULL);

  xmlFree(response_buffer);
  xmlFreeDoc(response_doc);

  g_free(response_resource_id);  
  
  return(NULL);
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
