/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/osc/controller/ags_osc_meter_controller.h>

#include <ags/libags.h>

#include <ags/i18n.h>

#include <stdlib.h>

void ags_osc_meter_controller_class_init(AgsOscMeterControllerClass *osc_meter_controller);
void ags_osc_meter_controller_init(AgsOscMeterController *osc_meter_controller);
void ags_osc_meter_controller_set_property(GObject *gobject,
					   guint prop_id,
					   const GValue *value,
					   GParamSpec *param_spec);
void ags_osc_meter_controller_get_property(GObject *gobject,
					   guint prop_id,
					   GValue *value,
					   GParamSpec *param_spec);
void ags_osc_meter_controller_dispose(GObject *gobject);
void ags_osc_meter_controller_finalize(GObject *gobject);

void* ags_osc_meter_controller_monitor_thread(void *ptr);

void ags_osc_meter_controller_real_start_monitor(AgsOscMeterController *osc_meter_controller);
void ags_osc_meter_controller_real_stop_monitor(AgsOscMeterController *osc_meter_controller);

gpointer ags_osc_meter_controller_real_monitor_meter(AgsOscMeterController *osc_meter_controller,
						     AgsOscConnection *osc_connection,
						     unsigned char *message, guint message_size);

/**
 * SECTION:ags_osc_meter_controller
 * @short_description: base osc_meter_controller
 * @title: AgsOscMeterController
 * @section_id:
 * @include: ags/audio/osc/controller/ags_osc_meter_controller.h
 *
 * The #AgsOscMeterController is a base object to implement osc_meter_controllers.
 */

enum{
  PROP_0,
};

enum{
  START_MONITOR,
  STOP_MONITOR,
  MONITOR_METER,
  LAST_SIGNAL,
};

static gpointer ags_osc_meter_controller_parent_class = NULL;
static guint osc_meter_controller_signals[LAST_SIGNAL];

GType
ags_osc_meter_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_meter_controller = 0;

    static const GTypeInfo ags_osc_meter_controller_info = {
      sizeof (AgsOscMeterControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_meter_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscMeterController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_meter_controller_init,
    };
    
    ags_type_osc_meter_controller = g_type_register_static(AGS_TYPE_OSC_CONTROLLER,
							   "AgsOscMeterController",
							   &ags_osc_meter_controller_info,
							   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_meter_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_meter_controller_class_init(AgsOscMeterControllerClass *osc_meter_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_osc_meter_controller_parent_class = g_type_class_peek_parent(osc_meter_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_meter_controller;

  gobject->set_property = ags_osc_meter_controller_set_property;
  gobject->get_property = ags_osc_meter_controller_get_property;

  gobject->dispose = ags_osc_meter_controller_dispose;
  gobject->finalize = ags_osc_meter_controller_finalize;

  /* properties */

  /* AgsOscMeterControllerClass */
  osc_meter_controller->start_monitor = ags_osc_meter_controller_real_start_monitor;
  osc_meter_controller->stop_monitor = ags_osc_meter_controller_real_stop_monitor;

  osc_meter_controller->monitor_meter = ags_osc_meter_controller_real_monitor_meter;

  /* signals */
  /**
   * AgsOscMeterController::start-monitor:
   * @osc_meter_controller: the #AgsOscMeterController
   *
   * The ::start-monitor signal is emited during start of monitoring meter.
   *
   * Since: 2.1.0
   */
  osc_meter_controller_signals[START_MONITOR] =
    g_signal_new("start-monitor",
		 G_TYPE_FROM_CLASS(osc_meter_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscMeterControllerClass, start_monitor),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsOscMeterController::stop-monitor:
   * @osc_meter_controller: the #AgsOscMeterController
   *
   * The ::stop-monitor signal is emited during stop of monitoring meter.
   *
   * Since: 2.1.0
   */
  osc_meter_controller_signals[STOP_MONITOR] =
    g_signal_new("stop-monitor",
		 G_TYPE_FROM_CLASS(osc_meter_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscMeterControllerClass, stop_monitor),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsOscMeterController::monitor-meter:
   * @osc_meter_controller: the #AgsOscMeterController
   * @osc_connection: the #AgsOscConnection
   * @message: the message received
   * @message_size: the message size
   *
   * The ::monitor-meter signal is emited during get data of meter controller.
   *
   * Returns: the #AgsOscResponse
   * 
   * Since: 2.1.0
   */
  osc_meter_controller_signals[MONITOR_METER] =
    g_signal_new("monitor-meter",
		 G_TYPE_FROM_CLASS(osc_meter_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscMeterControllerClass, monitor_meter),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__OBJECT_POINTER_UINT,
		 G_TYPE_POINTER, 3,
		 G_TYPE_OBJECT,
		 G_TYPE_POINTER,
		 G_TYPE_UINT);
}

void
ags_osc_meter_controller_init(AgsOscMeterController *osc_meter_controller)
{
  g_object_set(osc_meter_controller,
	       "context-path", "/meter",
	       NULL);

  osc_meter_controller->flags = 0;

  osc_meter_controller->monitor_timeout = (struct timespec *) malloc(sizeof(struct timespec));

  osc_meter_controller->monitor_timeout->tv_sec = 0;
  osc_meter_controller->monitor_timeout->tv_nsec = NSEC_PER_SEC / 30;
  
  osc_meter_controller->monitor_thread = (pthread_t *) malloc(sizeof(pthread_t));
  
  osc_meter_controller->monitor = NULL;
}

void
ags_osc_meter_controller_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec)
{
  AgsOscMeterController *osc_meter_controller;

  pthread_mutex_t *osc_controller_mutex;

  osc_meter_controller = AGS_OSC_METER_CONTROLLER(gobject);

  /* get osc controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_meter_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_meter_controller_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec)
{
  AgsOscMeterController *osc_meter_controller;

  pthread_mutex_t *osc_controller_mutex;

  osc_meter_controller = AGS_OSC_METER_CONTROLLER(gobject);

  /* get osc controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_meter_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_meter_controller_dispose(GObject *gobject)
{
  AgsOscMeterController *osc_meter_controller;

  osc_meter_controller = AGS_OSC_METER_CONTROLLER(gobject);
  
  if(osc_meter_controller->monitor != NULL){
    g_list_free_full(osc_meter_controller->monitor,
		     ags_osc_meter_controller_monitor_free);

    osc_meter_controller->monitor = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_meter_controller_parent_class)->dispose(gobject);
}

void
ags_osc_meter_controller_finalize(GObject *gobject)
{
  AgsOscMeterController *osc_meter_controller;

  osc_meter_controller = AGS_OSC_METER_CONTROLLER(gobject);

  free(osc_meter_controller->monitor_timeout);
  
  free(osc_meter_controller->monitor_thread);
  
  if(osc_meter_controller->monitor != NULL){
    g_list_free_full(osc_meter_controller->monitor,
		     ags_osc_meter_controller_monitor_free);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_meter_controller_parent_class)->finalize(gobject);
}

void*
ags_osc_meter_controller_monitor_thread(void *ptr)
{
  AgsOscMeterController *osc_meter_controller;

  GList *start_monitor, *monitor;
  
  pthread_mutex_t *osc_controller_mutex;

  osc_meter_controller = AGS_OSC_METER_CONTROLLER(ptr);

  /* get OSC meter controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_meter_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());

  /* run */
  ags_osc_meter_controller_set_flags(osc_meter_controller, AGS_OSC_METER_CONTROLLER_MONITOR_RUNNING);
  
  while(ags_osc_meter_controller_test_flags(osc_meter_controller, AGS_OSC_METER_CONTROLLER_MONITOR_RUNNING)){
    pthread_mutex_lock(osc_controller_mutex);

    start_monitor = g_list_copy(osc_meter_controller->monitor);
    
    pthread_mutex_unlock(osc_controller_mutex);

    /*  */
    monitor = start_monitor;

    while(monitor != NULL){
      AgsPort *port;
      
      AgsOscConnection *osc_connection;
      AgsOscResponse *osc_response;

      GType port_value_type;

      gchar *path;
      unsigned char *packet;

      guint port_value_length;
      gboolean port_value_is_pointer;
      guint real_packet_size;
      guint packet_size;
      
      pthread_mutex_t *port_mutex;

      osc_connection = AGS_OSC_METER_CONTROLLER_MONITOR(monitor->data)->osc_connection;

      path = AGS_OSC_METER_CONTROLLER_MONITOR(monitor->data)->path;
      port = AGS_OSC_METER_CONTROLLER_MONITOR(monitor->data)->port;

      /*  */
      osc_response = ags_osc_response_new();
      
      packet = (unsigned char *) malloc(AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
      memset(packet, 0, AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
      
      g_object_set(osc_response,
		   "packet", packet,
		   "packet-size", packet_size,
		   NULL);
      
      real_packet_size = AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE;

      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/meter", -1);
      
      packet_size += 8;

      /* get port mutex */
      pthread_mutex_lock(ags_port_get_class_mutex());
      
      port_mutex = port->obj_mutex;
      
      pthread_mutex_unlock(ags_port_get_class_mutex());

      /* check array type */
      g_object_get(port,
		   "port-value-is-pointer", &port_value_is_pointer,
		   "port-value-type", &port_value_type,
		   "port-value-length", &port_value_length,
		   NULL);

      if(port_value_is_pointer){
	gchar *type_tag;
	
	guint i;

	/* message type tag */
	if(packet_size + (4 * (guint) ceil((double) (port_value_length + 5) / 4.0)) > real_packet_size){
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_ERROR);

	  g_object_set(osc_response,
		       "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		       NULL);

	  ags_osc_connection_write_response(osc_connection,
					    osc_response);
	    
	  /* iterate */
	  monitor = monitor->next;

	  continue;
	}

	type_tag = (gchar *) malloc((port_value_length + 5) * sizeof(gchar));

	type_tag[0] = ',';
	type_tag[1] = 's';
	type_tag[2] = '[';
	type_tag[port_value_length + 4] = ']';

	if(port_value_type == G_TYPE_DOUBLE){
	  for(i = 0; i < port_value_length; i++){
	    packet[packet_size + 3 + i] = 'd';
	  }

	  /* node path */
	  packet_size += (4 * (guint) ceil((double) (port_value_length + 5) / 4.0));
	  
	  length = strlen(path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    ags_osc_connection_write_response(osc_connection,
					      osc_response);
	    
	    /* iterate */
	    monitor = monitor->next;

	    continue;
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 path, -1);

	  /* node argument */
	  packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

	  if(packet_size + (4 * (guint) ceil((double) (port_value_length * 8) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    ags_osc_connection_write_response(osc_connection,
					      osc_response);
	    
	    /* iterate */
	    monitor = monitor->next;

	    continue;
	  }
	  
	  pthread_mutex_lock(port_mutex);

	  for(i = 0; i < port_value_length; i++){
	    gdouble value;

	    value = port->port_value.ags_port_double_ptr[i];
	    
	    ags_osc_buffer_util_put_double(packet + packet_size + 3 + (i * 8),
					   value);
	  }

	  pthread_mutex_unlock(port_mutex);

	  /* packet size */
	  ags_osc_buffer_util_put_int32(packet,
					packet_size);
	}else{
	  g_warning("unsupported port type");
	}  
      }else{
	if(port_value_type == G_TYPE_FLOAT){
	  gfloat value;
	  
	  /* message type tag */
	  pthread_mutex_lock(port_mutex);

	  value = port->port_value.ags_port_float;
	  
	  pthread_mutex_unlock(port_mutex);

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 ",sf", -1);

	  /* node path */	    
	  packet_size += 4;
	  
	  length = strlen(path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 8 > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    ags_osc_connection_write_response(osc_connection,
					      osc_response);
	    
	    /* iterate */
	    monitor = monitor->next;

	    continue;
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 path, -1);

	  /* node argument */
	  packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

	  ags_osc_buffer_util_put_float(packet + packet_size,
					value);
	  
	  packet_size += 4;
	  
	  /* packet size */
	  ags_osc_buffer_util_put_int32(packet,
					packet_size);
	}else{
	  g_warning("unsupported port type");
	}
      }

      /* write response */
      ags_osc_connection_write_response(osc_connection,
					osc_response);
      
      /* iterate */
      monitor = monitor->next;
    }
    
    nanosleep(osc_meter_controller->monitor_timeout, NULL);
  }
  
  pthread_exit(NULL);
}

/**
 * ags_osc_meter_controller_test_flags:
 * @osc_meter_controller: the #AgsOscMeterController
 * @flags: the flags
 *
 * Test @flags to be set on @osc_meter_controller.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.1.0
 */
gboolean
ags_osc_meter_controller_test_flags(AgsOscMeterController *osc_meter_controller, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *osc_controller_mutex;

  if(!AGS_IS_OSC_METER_CONTROLLER(osc_meter_controller)){
    return(FALSE);
  }

  /* get OSC meter controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_meter_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());

  /* test */
  pthread_mutex_lock(osc_controller_mutex);

  retval = (flags & (osc_meter_controller->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(osc_controller_mutex);

  return(retval);
}

/**
 * ags_osc_meter_controller_set_flags:
 * @osc_meter_controller: the #AgsOscMeterController
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.1.0
 */
void
ags_osc_meter_controller_set_flags(AgsOscMeterController *osc_meter_controller, guint flags)
{
  pthread_mutex_t *osc_controller_mutex;

  if(!AGS_IS_OSC_METER_CONTROLLER(osc_meter_controller)){
    return;
  }

  /* get OSC meter controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_meter_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(osc_controller_mutex);

  osc_meter_controller->flags |= flags;

  pthread_mutex_unlock(osc_controller_mutex);
}

/**
 * ags_osc_meter_controller_unset_flags:
 * @osc_meter_controller: the #AgsOscMeterController
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.1.0
 */
void
ags_osc_meter_controller_unset_flags(AgsOscMeterController *osc_meter_controller, guint flags)
{
  pthread_mutex_t *osc_controller_mutex;

  if(!AGS_IS_OSC_METER_CONTROLLER(osc_meter_controller)){
    return;
  }

  /* get OSC meter controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_meter_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(osc_controller_mutex);

  osc_meter_controller->flags &= (~flags);

  pthread_mutex_unlock(osc_controller_mutex);
}

/**
 * ags_osc_meter_controller_monitor_alloc:
 * 
 * Allocate #AgsOscMeterControllerMonitor-struct.
 * 
 * Returns: the newly allocate #AgsOscMeterControllerMonitor-struct
 * 
 * Since: 2.1.0
 */
AgsOscMeterControllerMonitor*
ags_osc_meter_controller_monitor_alloc()
{
  AgsOscMeterControllerMonitor *monitor;

  monitor = (AgsOscMeterControllerMonitor *) malloc(sizeof(AgsOscMeterControllerMonitor));

  monitor->osc_connection = NULL;

  monitor->path =  NULL;
  monitor->port = NULL;

  return(monitor);
}

/**
 * ags_osc_meter_controller_monitor_free:
 * @monitor: the #AgsOscMeterControllerMonitor-struct
 * 
 * Free @monitor.
 * 
 * Since: 2.1.0
 */
void
ags_osc_meter_controller_monitor_free(AgsOscMeterControllerMonitor *monitor)
{  
  g_object_unref(monitor->osc_connection);

  g_free(monitor->path);
  g_object_unref(monitor->port);
  
  free(monitor);
}

void
ags_osc_meter_controller_real_start_monitor(AgsOscMeterController *osc_meter_controller)
{
  //TODO:JK: implement me
}

/**
 * ags_osc_meter_controller_start_monitor:
 * @osc_meter_controller: the #AgsOscMeterController
 * 
 * Start monitoring.
 * 
 * Since: 2.1.0
 */
void
ags_osc_meter_controller_start_monitor(AgsOscMeterController *osc_meter_controller)
{
  g_return_if_fail(AGS_IS_OSC_METER_CONTROLLER(osc_meter_controller));
  
  g_object_ref((GObject *) osc_meter_controller);
  g_signal_emit(G_OBJECT(osc_meter_controller),
		osc_meter_controller_signals[START_MONITOR], 0,
		osc_connection,
		message, message_size,
		&osc_response);
  g_object_unref((GObject *) osc_meter_controller);
}

void
ags_osc_meter_controller_real_stop_monitor(AgsOscMeterController *osc_meter_controller)
{
  //TODO:JK: implement me
}

/**
 * ags_osc_meter_controller_stop_monitor:
 * @osc_meter_controller: the #AgsOscMeterController
 * 
 * Stop monitoring.
 * 
 * Since: 2.1.0
 */
void
ags_osc_meter_controller_stop_monitor(AgsOscMeterController *osc_meter_controller)
{
  g_return_if_fail(AGS_IS_OSC_METER_CONTROLLER(osc_meter_controller));
  
  g_object_ref((GObject *) osc_meter_controller);
  g_signal_emit(G_OBJECT(osc_meter_controller),
		osc_meter_controller_signals[STOP_MONITOR], 0,
		osc_connection,
		message, message_size,
		&osc_response);
  g_object_unref((GObject *) osc_meter_controller);
}

gpointer
ags_osc_meter_controller_real_monitor_meter(AgsOscMeterController *osc_meter_controller,
					    AgsOscConnection *osc_connection,
					    unsigned char *message, guint message_size)
{
  //TODO:JK: implement me
}

/**
 * ags_osc_meter_controller_monitor_meter:
 * @osc_meter_controller: the #AgsOscMeterController
 * @osc_connection: the #AgsOscConnection
 * @message: the message received
 * @message_size: the message size
 * 
 * Get meter.
 * 
 * Returns: the #AgsOscResponse
 * 
 * Since: 2.1.0
 */
gpointer
ags_osc_meter_controller_monitor_meter(AgsOscMeterController *osc_meter_controller,
				       AgsOscConnection *osc_connection,
				       unsigned char *message, guint message_size)
{
  gpointer osc_response;
  
  g_return_val_if_fail(AGS_IS_OSC_METER_CONTROLLER(osc_meter_controller), NULL);
  
  g_object_ref((GObject *) osc_meter_controller);
  g_signal_emit(G_OBJECT(osc_meter_controller),
		osc_meter_controller_signals[MONITOR_METER], 0,
		osc_connection,
		message, message_size,
		&osc_response);
  g_object_unref((GObject *) osc_meter_controller);

  return(osc_response);
}

/**
 * ags_osc_meter_controller_new:
 * 
 * Instantiate new #AgsOscMeterController
 * 
 * Returns: the #AgsOscMeterController
 * 
 * Since: 2.1.0
 */
AgsOscMeterController*
ags_osc_meter_controller_new()
{
  AgsOscMeterController *osc_meter_controller;

  osc_meter_controller = (AgsOscMeterController *) g_object_new(AGS_TYPE_OSC_METER_CONTROLLER,
								NULL);

  return(osc_meter_controller);
}
