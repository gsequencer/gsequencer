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

#include <ags/audio/osc/controller/ags_osc_front_controller.h>

#include <ags/audio/osc/ags_osc_server.h>
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

#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include <ags/i18n.h>

void ags_osc_front_controller_class_init(AgsOscFrontControllerClass *osc_front_controller);
void ags_osc_front_controller_init(AgsOscFrontController *osc_front_controller);
void ags_osc_front_controller_set_property(GObject *gobject,
					   guint prop_id,
					   const GValue *value,
					   GParamSpec *param_spec);
void ags_osc_front_controller_get_property(GObject *gobject,
					   guint prop_id,
					   GValue *value,
					   GParamSpec *param_spec);
void ags_osc_front_controller_dispose(GObject *gobject);
void ags_osc_front_controller_finalize(GObject *gobject);

void* ags_osc_front_controller_delegate_thread(void *ptr);

void ags_osc_front_controller_real_start_delegate(AgsOscFrontController *osc_front_controller);
void ags_osc_front_controller_real_stop_delegate(AgsOscFrontController *osc_front_controller);

gpointer ags_osc_front_controller_real_do_request(AgsOscFrontController *osc_front_controller,
						  AgsOscConnection *osc_connection,
						  unsigned char *packet, guint packet_size);

/**
 * SECTION:ags_osc_front_controller
 * @short_description: OSC front controller
 * @title: AgsOscFrontController
 * @section_id:
 * @include: ags/audio/osc/controller/ags_osc_front_controller.h
 *
 * The #AgsOscFrontController implements the OSC front controller.
 */

enum{
  PROP_0,
};

enum{
  START_DELEGATE,
  STOP_DELEGATE,
  DO_REQUEST,
  LAST_SIGNAL,
};

static gpointer ags_osc_front_controller_parent_class = NULL;
static guint osc_front_controller_signals[LAST_SIGNAL];

GType
ags_osc_front_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_front_controller = 0;

    static const GTypeInfo ags_osc_front_controller_info = {
      sizeof (AgsOscFrontControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_front_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscFrontController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_front_controller_init,
    };
    
    ags_type_osc_front_controller = g_type_register_static(AGS_TYPE_OSC_CONTROLLER,
							   "AgsOscFrontController",
							   &ags_osc_front_controller_info,
							   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_front_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_front_controller_class_init(AgsOscFrontControllerClass *osc_front_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_osc_front_controller_parent_class = g_type_class_peek_parent(osc_front_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_front_controller;

  //  gobject->set_property = ags_osc_front_controller_set_property;
  //  gobject->get_property = ags_osc_front_controller_get_property;

  gobject->dispose = ags_osc_front_controller_dispose;
  gobject->finalize = ags_osc_front_controller_finalize;

  /* properties */

  /* AgsOscFrontControllerClass */
  osc_front_controller->start_delegate = ags_osc_front_controller_real_start_delegate;
  osc_front_controller->stop_delegate = ags_osc_front_controller_real_stop_delegate;

  osc_front_controller->do_request = ags_osc_front_controller_real_do_request;

  /* signals */
  /**
   * AgsOscFrontController::start-delegate:
   * @osc_front_controller: the #AgsOscFrontController
   *
   * The ::start-delegate signal is emited during start of delegating messages.
   *
   * Since: 2.1.0
   */
  osc_front_controller_signals[START_DELEGATE] =
    g_signal_new("start-delegate",
		 G_TYPE_FROM_CLASS(osc_front_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscFrontControllerClass, start_delegate),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsOscFrontController::stop-delegate:
   * @osc_front_controller: the #AgsOscFrontController
   *
   * The ::stop-delegate signal is emited during stop of delegating messages.
   *
   * Since: 2.1.0
   */
  osc_front_controller_signals[STOP_DELEGATE] =
    g_signal_new("stop-delegate",
		 G_TYPE_FROM_CLASS(osc_front_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscFrontControllerClass, stop_delegate),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsOscFrontController::do-request:
   * @osc_front_controller: the #AgsOscFrontController
   * @osc_connection: the #AgsOscConnection
   * @packet: the packet received
   * @packet_size: the packet size
   *
   * The ::do-request signal is emited during do request of front controller.
   *
   * Since: 2.1.0
   */
  osc_front_controller_signals[DO_REQUEST] =
    g_signal_new("do-request",
		 G_TYPE_FROM_CLASS(osc_front_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscFrontControllerClass, do_request),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__OBJECT_POINTER_UINT,
		 G_TYPE_POINTER, 3,
		 G_TYPE_OBJECT,
		 G_TYPE_POINTER,
		 G_TYPE_UINT);
}

void
ags_osc_front_controller_init(AgsOscFrontController *osc_front_controller)
{
  g_object_set(osc_front_controller,
	       "context-path", "/",
	       NULL);

  osc_front_controller->flags = 0;

  osc_front_controller->delegate_timeout = (struct timespec *) malloc(sizeof(struct timespec));

  osc_front_controller->delegate_timeout = 0;

  g_atomic_int_set(&(osc_front_controller->do_reset),
		   FALSE);

  g_mutex_init(&(osc_front_controller->delegate_mutex));

  g_cond_init(&(osc_front_controller->delegate_cond));
  
  osc_front_controller->delegate_thread = NULL;
  
  osc_front_controller->message = NULL;
}

void
ags_osc_front_controller_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec)
{
  AgsOscFrontController *osc_front_controller;

  GRecMutex *osc_controller_mutex;

  osc_front_controller = AGS_OSC_FRONT_CONTROLLER(gobject);

  /* get osc controller mutex */
  osc_controller_mutex = AGS_OSC_CONTROLLER_GET_OBJ_MUTEX(osc_front_controller);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_front_controller_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec)
{
  AgsOscFrontController *osc_front_controller;

  GRecMutex *osc_controller_mutex;

  osc_front_controller = AGS_OSC_FRONT_CONTROLLER(gobject);

  /* get osc controller mutex */
  osc_controller_mutex = AGS_OSC_CONTROLLER_GET_OBJ_MUTEX(osc_front_controller);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_front_controller_dispose(GObject *gobject)
{
  AgsOscFrontController *osc_front_controller;

  osc_front_controller = AGS_OSC_FRONT_CONTROLLER(gobject);
  
  if(osc_front_controller->message != NULL){
    g_list_free_full(osc_front_controller->message,
		     (GDestroyNotify) g_object_unref);

    osc_front_controller->message = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_osc_front_controller_parent_class)->dispose(gobject);
}

void
ags_osc_front_controller_finalize(GObject *gobject)
{
  AgsOscFrontController *osc_front_controller;

  osc_front_controller = AGS_OSC_FRONT_CONTROLLER(gobject);

  free(osc_front_controller->delegate_timeout);
    
  if(osc_front_controller->message != NULL){
    g_list_free_full(osc_front_controller->message,
		     (GDestroyNotify) g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_front_controller_parent_class)->finalize(gobject);
}

void*
ags_osc_front_controller_delegate_thread(void *ptr)
{
  AgsOscServer *osc_server;
  AgsOscFrontController *osc_front_controller;

  GList *start_controller, *controller;
  
  gint64 time_now, time_next;
  gint64 current_time;
  
  GRecMutex *osc_controller_mutex;

  osc_front_controller = AGS_OSC_FRONT_CONTROLLER(ptr);

  g_object_get(osc_front_controller,
	       "osc-server", &osc_server,
	       NULL);

  g_object_get(osc_server,
	       "controller", &start_controller,
	       NULL);
  
  /* get OSC front controller mutex */
  osc_controller_mutex = AGS_OSC_CONTROLLER_GET_OBJ_MUTEX(osc_front_controller);

  time_next = 0;

  ags_osc_front_controller_set_flags(osc_front_controller,
				     AGS_OSC_FRONT_CONTROLLER_DELEGATE_RUNNING);
  
  while(ags_osc_front_controller_test_flags(osc_front_controller, AGS_OSC_FRONT_CONTROLLER_DELEGATE_RUNNING)){
    GList *start_message, *message;
    GList *start_list, *list;
    
    g_mutex_lock(&(osc_front_controller->delegate_mutex));

    time_now = g_get_monotonic_time();
    osc_front_controller->delegate_timeout = time_now + (G_TIME_SPAN_SECOND / 30);
    
    while(!g_atomic_int_get(&(osc_front_controller->do_reset)) &&
	  ((time_now < time_next) &&
	   (time_now < osc_front_controller->delegate_timeout))){
      g_cond_wait_until(&(osc_front_controller->delegate_cond),
			&(osc_front_controller->delegate_mutex),
			osc_front_controller->delegate_timeout);
      
      time_now = g_get_monotonic_time();
    }

    g_atomic_int_set(&(osc_front_controller->do_reset),
		     FALSE);
    
    g_mutex_unlock(&(osc_front_controller->delegate_mutex));

    /* check delegate */
    g_rec_mutex_lock(osc_controller_mutex);

    start_message = NULL;
    list =
      start_list = g_list_copy(osc_front_controller->message);

    while(list != NULL){
      if(AGS_OSC_MESSAGE(list->data)->immediately){
	start_message = g_list_prepend(start_message,
				       list->data);
	
	ags_osc_front_controller_remove_message(osc_front_controller,
						list->data);
      }else{
	current_time = AGS_OSC_MESSAGE(list->data)->tv_sec + AGS_OSC_MESSAGE(list->data)->tv_fraction / 4.294967296 * 1000.0;
      
	if(current_time < time_now){
	  start_message = g_list_prepend(start_message,
					 list->data);
	  
	  ags_osc_front_controller_remove_message(osc_front_controller,
						  list->data);
	}else{
	  break;
	}
      }

      list = list->next;
    }
    
    g_rec_mutex_unlock(osc_controller_mutex);

    g_list_free(start_list);
    
    message = 
      start_message = g_list_reverse(start_message);

    while(message != NULL){
      GList *start_osc_response, *osc_response;

      AgsOscMessage *current;
      
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
	  
	  break;
	}
	
	controller = controller->next;
      }

      /* write response */
      osc_response = start_osc_response;
      
      while(osc_response != NULL){
	ags_osc_connection_write_response(current->osc_connection,
					  osc_response->data);

	osc_response = osc_response->next;
      }

      g_list_free_full(start_osc_response,
		       g_object_unref);
      
      message = message->next;
    }

    /* free messages */
    g_list_free_full(start_message,
		     (GDestroyNotify) g_object_unref);

    /* next */
    g_mutex_lock(&(osc_front_controller->delegate_mutex));

    if(osc_front_controller->message != NULL){
      time_next = AGS_OSC_MESSAGE(osc_front_controller->message)->tv_sec + AGS_OSC_MESSAGE(osc_front_controller->message)->tv_fraction / 4.294967296 * 1000.0;
    }else{
      time_now = g_get_monotonic_time();

      time_next = time_now + G_TIME_SPAN_SECOND / 30;
    }
    
    g_mutex_unlock(&(osc_front_controller->delegate_mutex));
  }

  g_object_unref(osc_server);
  
  g_list_free_full(start_controller,
		   g_object_unref);
  
  g_thread_exit(NULL);

  return(NULL);
}

/**
 * ags_osc_front_controller_test_flags:
 * @osc_front_controller: the #AgsOscFrontController
 * @flags: the flags
 *
 * Test @flags to be set on @osc_front_controller.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.1.0
 */
gboolean
ags_osc_front_controller_test_flags(AgsOscFrontController *osc_front_controller, guint flags)
{
  gboolean retval;  
  
  GRecMutex *osc_controller_mutex;

  if(!AGS_IS_OSC_FRONT_CONTROLLER(osc_front_controller)){
    return(FALSE);
  }

  /* get OSC front controller mutex */
  osc_controller_mutex = AGS_OSC_CONTROLLER_GET_OBJ_MUTEX(osc_front_controller);

  /* test */
  g_rec_mutex_lock(osc_controller_mutex);

  retval = (flags & (osc_front_controller->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(osc_controller_mutex);

  return(retval);
}

/**
 * ags_osc_front_controller_set_flags:
 * @osc_front_controller: the #AgsOscFrontController
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.1.0
 */
void
ags_osc_front_controller_set_flags(AgsOscFrontController *osc_front_controller, guint flags)
{
  GRecMutex *osc_controller_mutex;

  if(!AGS_IS_OSC_FRONT_CONTROLLER(osc_front_controller)){
    return;
  }

  /* get OSC front controller mutex */
  osc_controller_mutex = AGS_OSC_CONTROLLER_GET_OBJ_MUTEX(osc_front_controller);
  
  /* set flags */
  g_rec_mutex_lock(osc_controller_mutex);

  osc_front_controller->flags |= flags;

  g_rec_mutex_unlock(osc_controller_mutex);
}

/**
 * ags_osc_front_controller_unset_flags:
 * @osc_front_controller: the #AgsOscFrontController
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.1.0
 */
void
ags_osc_front_controller_unset_flags(AgsOscFrontController *osc_front_controller, guint flags)
{
  GRecMutex *osc_controller_mutex;

  if(!AGS_IS_OSC_FRONT_CONTROLLER(osc_front_controller)){
    return;
  }

  /* get OSC front controller mutex */
  osc_controller_mutex = AGS_OSC_CONTROLLER_GET_OBJ_MUTEX(osc_front_controller);
  
  /* set flags */
  g_rec_mutex_lock(osc_controller_mutex);

  osc_front_controller->flags &= (~flags);

  g_rec_mutex_unlock(osc_controller_mutex);
}

/**
 * ags_osc_front_controller_add_message:
 * @osc_front_controller: the #AgsOscFrontController
 * @message: the #AgsOscMessage
 * 
 * Add @message to @osc_front_controller.
 * 
 * Since: 3.0.0
 */
void
ags_osc_front_controller_add_message(AgsOscFrontController *osc_front_controller,
				     GObject *message)
{
  GRecMutex *osc_controller_mutex;

  if(!AGS_IS_OSC_FRONT_CONTROLLER(osc_front_controller) ||
     message == NULL){
    return;
  }

  /* get OSC front controller mutex */
  osc_controller_mutex = AGS_OSC_CONTROLLER_GET_OBJ_MUTEX(osc_front_controller);
  
  /* add */
  g_rec_mutex_lock(osc_controller_mutex);

  if(g_list_find(osc_front_controller->message, message) == NULL){
    osc_front_controller->message = g_list_insert_sorted(osc_front_controller->message,
							 message,
							 ags_osc_message_sort_func);
    g_object_ref(message);
  }
  
  g_rec_mutex_unlock(osc_controller_mutex);
}

/**
 * ags_osc_front_controller_remove_message:
 * @osc_front_controller: the #AgsOscFrontController
 * @message: the #AgsOscMessage
 * 
 * Remove @message from @osc_front_controller.
 * 
 * Since: 3.0.0
 */
void
ags_osc_front_controller_remove_message(AgsOscFrontController *osc_front_controller,
					GObject *message)
{
  GRecMutex *osc_controller_mutex;

  if(!AGS_IS_OSC_FRONT_CONTROLLER(osc_front_controller) ||
     message == NULL){
    return;
  }

  /* get OSC front controller mutex */
  osc_controller_mutex = AGS_OSC_CONTROLLER_GET_OBJ_MUTEX(osc_front_controller);

  /* remove */
  g_rec_mutex_lock(osc_controller_mutex);

  if(g_list_find(osc_front_controller->message, message) != NULL){
    osc_front_controller->message = g_list_remove(osc_front_controller->message,
						  message);
    g_object_unref(message);
  }
  
  g_rec_mutex_unlock(osc_controller_mutex);
}

void
ags_osc_front_controller_real_start_delegate(AgsOscFrontController *osc_front_controller)
{
  GRecMutex *osc_controller_mutex;

  /* get OSC front controller mutex */
  osc_controller_mutex = AGS_OSC_CONTROLLER_GET_OBJ_MUTEX(osc_front_controller);

  /* test if already started */
  g_rec_mutex_lock(osc_controller_mutex);
    
  if(ags_osc_front_controller_test_flags(osc_front_controller, AGS_OSC_FRONT_CONTROLLER_DELEGATE_STARTED)){
    g_rec_mutex_unlock(osc_controller_mutex);
    
    return;
  }

  ags_osc_front_controller_set_flags(osc_front_controller, AGS_OSC_FRONT_CONTROLLER_DELEGATE_STARTED);
  
  g_rec_mutex_unlock(osc_controller_mutex);

  /* create delegate thread */
  osc_front_controller->delegate_thread = g_thread_new("Advanced Gtk+ Sequencer OSC Server - delegate thread",
						       ags_osc_front_controller_delegate_thread,
						       osc_front_controller);
}

/**
 * ags_osc_front_controller_start_delegate:
 * @osc_front_controller: the #AgsOscFrontController
 * 
 * Start delegating.
 * 
 * Since: 2.1.0
 */
void
ags_osc_front_controller_start_delegate(AgsOscFrontController *osc_front_controller)
{
  g_return_if_fail(AGS_IS_OSC_FRONT_CONTROLLER(osc_front_controller));
  
  g_object_ref((GObject *) osc_front_controller);
  g_signal_emit(G_OBJECT(osc_front_controller),
		osc_front_controller_signals[START_DELEGATE], 0);
  g_object_unref((GObject *) osc_front_controller);
}

void
ags_osc_front_controller_real_stop_delegate(AgsOscFrontController *osc_front_controller)
{
  if(!ags_osc_front_controller_test_flags(osc_front_controller, AGS_OSC_FRONT_CONTROLLER_DELEGATE_RUNNING)){
    return;
  }

  ags_osc_front_controller_set_flags(osc_front_controller, AGS_OSC_FRONT_CONTROLLER_DELEGATE_TERMINATING);
  ags_osc_front_controller_unset_flags(osc_front_controller, AGS_OSC_FRONT_CONTROLLER_DELEGATE_RUNNING);

  /* join thread */
  g_thread_join(osc_front_controller->delegate_thread);
  
  ags_osc_front_controller_unset_flags(osc_front_controller, (AGS_OSC_FRONT_CONTROLLER_DELEGATE_TERMINATING |
							      AGS_OSC_FRONT_CONTROLLER_DELEGATE_STARTED));
}

/**
 * ags_osc_front_controller_stop_delegate:
 * @osc_front_controller: the #AgsOscFrontController
 * 
 * Stop delegating.
 * 
 * Since: 2.1.0
 */
void
ags_osc_front_controller_stop_delegate(AgsOscFrontController *osc_front_controller)
{
  g_return_if_fail(AGS_IS_OSC_FRONT_CONTROLLER(osc_front_controller));
  
  g_object_ref((GObject *) osc_front_controller);
  g_signal_emit(G_OBJECT(osc_front_controller),
		osc_front_controller_signals[STOP_DELEGATE], 0);
  g_object_unref((GObject *) osc_front_controller);
}

gpointer
ags_osc_front_controller_real_do_request(AgsOscFrontController *osc_front_controller,
					 AgsOscConnection *osc_connection,
					 unsigned char *packet, guint packet_size)
{
  gint32 tv_sec;
  gint32 tv_fraction;
  gboolean immediately;
  guint offset;

  auto guint ags_osc_front_controller_do_request_read_bundle(AgsOscFrontController *osc_front_controller,
							     AgsOscConnection *osc_connection,
							     unsigned char *packet, guint packet_size,
							     guint offset);
  auto guint ags_osc_front_controller_do_request_read_message(AgsOscFrontController *osc_front_controller,
							      AgsOscConnection *osc_connection,
							      unsigned char *packet, guint packet_size,
							      guint offset,
							      gint32 tv_sec, gint32 tv_fraction, gboolean immediately);

  guint ags_osc_front_controller_do_request_read_bundle(AgsOscFrontController *osc_front_controller,
							AgsOscConnection *osc_connection,
							unsigned char *packet, guint packet_size,
							guint offset)
  {
    gint32 tv_sec;
    gint32 tv_fraction;
    gboolean immediately;
    guint read_count;
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
	ags_osc_front_controller_do_request_read_bundle(osc_front_controller,
							osc_connection,
							packet, packet_size,
							offset + read_count);

	read_count += ((guint) 4 * ceil((double) length / 4.0));
      }else if(packet[offset + read_count] == '/'){
	ags_osc_front_controller_do_request_read_message(osc_front_controller,
							 osc_connection,
							 packet, packet_size,
							 offset + read_count,
							 tv_sec, tv_fraction, immediately);

	read_count += ((guint) 4 * ceil((double) length / 4.0));
      }else{
	read_count += 1;
	
	g_warning("malformed data");
      }
    }
    
    return(read_count);
  }
  
  guint ags_osc_front_controller_do_request_read_message(AgsOscFrontController *osc_front_controller,
							 AgsOscConnection *osc_connection,
							 unsigned char *packet, guint packet_size,
							 guint offset,
							 gint32 tv_sec, gint32 tv_fraction, gboolean immediately)
  {
    AgsOscMessage *osc_message;

    guchar *message;
    gchar *address_pattern;
    gchar *type_tag;

    gsize address_pattern_length;
    gsize type_tag_length;
    gsize data_length;
    guint read_count;
    guint i;

    read_count = 0;

    ags_osc_buffer_util_get_string(packet + offset,
				   &address_pattern, &address_pattern_length);
    
    if(address_pattern == NULL){
      return(0);
    }
    
    read_count += (4 * (guint) ceil((double) (address_pattern_length + 1) / 4.0));
    free(address_pattern);
    
    type_tag = NULL;

    if(packet_size > offset + read_count){
      if(packet[offset + read_count] == ','){
	ags_osc_buffer_util_get_string(packet + offset + read_count,
				       &type_tag, &type_tag_length);

	read_count += (4 * (guint) ceil((double) (type_tag_length + 1) / 4.0));
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
	    guint length;

	    length = strlen(packet + offset + read_count + data_length);

	    data_length += (4 * (guint) ceil((double) (length + 1) / 4.0));
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

    read_count += (4 * (guint) ceil((double) data_length / 4.0));

    osc_message = ags_osc_message_new();

    message = (guchar *) malloc(read_count * sizeof(guchar));
    memcpy(message,
	   packet + offset,
	   read_count * sizeof(guchar));

    g_object_set(osc_message,
		 "osc-connection", osc_connection,
		 "tv-sec", tv_sec,
		 "tv-fraction", tv_fraction,
		 "immediately", immediately,
		 "message-size", read_count,
		 "message", message,
		 NULL);    

    ags_osc_front_controller_add_message(osc_front_controller,
					 osc_message);
    
    return(read_count);
  }
    
  tv_sec = 0;
  tv_fraction = 0;
  immediately = TRUE;

  for(offset = 4; offset < packet_size;){
    guint read_count;

#ifdef AGS_DEBUG    
    g_message("%d %d", offset, packet_size);
    g_message("%x[%c]", packet[offset], packet[offset]);
#endif
    
    if(!g_strcmp0(packet + offset, "#bundle")){      
      read_count = ags_osc_front_controller_do_request_read_bundle(osc_front_controller,
								   osc_connection,
								   packet, packet_size,
								   offset);
    }else if(packet[offset] == '/'){
      read_count = ags_osc_front_controller_do_request_read_message(osc_front_controller,
								    osc_connection,
								    packet, packet_size,
								    offset,
								    0, 0, TRUE);
    }else{
      read_count = 1;

      g_warning("malformed data");
    }

    if(read_count > 0){
      offset += ((guint) 4 * ceil((double) read_count / 4.0));
    }else{
      offset += 1;
      
      g_warning("malformed data");
    }
  }

  return(NULL);
}

/**
 * ags_osc_front_controller_write_response:
 * @osc_front_controller: the #AgsOscFrontController
 * @osc_connection: the #AgsOscConnection
 * @packet: the packet received
 * @packet_size: the packet size
 * 
 * Do request.
 * 
 * Returns: %NULL
 * 
 * Since: 2.1.0
 */
gpointer
ags_osc_front_controller_do_request(AgsOscFrontController *osc_front_controller,
				    AgsOscConnection *osc_connection,
				    unsigned char *packet, guint packet_size)
{
  gpointer osc_response;
  
  g_return_val_if_fail(AGS_IS_OSC_FRONT_CONTROLLER(osc_front_controller), NULL);
  
  g_object_ref((GObject *) osc_front_controller);
  g_object_ref((GObject *) osc_connection);
  g_signal_emit(G_OBJECT(osc_front_controller),
		osc_front_controller_signals[DO_REQUEST], 0,
		osc_connection,
		packet, packet_size,
		&osc_response);
  g_object_unref((GObject *) osc_connection);
  g_object_unref((GObject *) osc_front_controller);

  return(osc_response);
}

/**
 * ags_osc_front_controller_new:
 * 
 * Instantiate new #AgsOscFrontController
 * 
 * Returns: the #AgsOscFrontController
 * 
 * Since: 2.1.0
 */
AgsOscFrontController*
ags_osc_front_controller_new()
{
  AgsOscFrontController *osc_front_controller;

  osc_front_controller = (AgsOscFrontController *) g_object_new(AGS_TYPE_OSC_FRONT_CONTROLLER,
								NULL);

  return(osc_front_controller);
}
