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

#include <ags/audio/osc/controller/ags_osc_meter_controller.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_port.h>

#include <ags/audio/osc/ags_osc_response.h>
#include <ags/audio/osc/ags_osc_buffer_util.h>

#include <ags/i18n.h>

#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <sys/types.h>
#include <regex.h>

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

gpointer ags_osc_meter_controller_monitor_meter_audio(AgsOscMeterController *osc_meter_controller,
						      AgsOscConnection *osc_connection,
						      AgsAudio *audio,
						      unsigned char *message, guint message_size,
						      gchar *type_tag,
						      gchar *path, guint path_offset);
gpointer ags_osc_meter_controller_monitor_meter_channel(AgsOscMeterController *osc_meter_controller,
							AgsOscConnection *osc_connection,
							AgsChannel *channel,
							unsigned char *message, guint message_size,
							gchar *type_tag,
							gchar *path, guint path_offset);

gpointer ags_osc_meter_controller_monitor_meter_recall(AgsOscMeterController *osc_meter_controller,
						       AgsOscConnection *osc_connection,
						       AgsRecall *recall,
						       unsigned char *message, guint message_size,
						       gchar *type_tag,
						       gchar *path, guint path_offset);

gpointer ags_osc_meter_controller_monitor_meter_port(AgsOscMeterController *osc_meter_controller,
						     AgsOscConnection *osc_connection,
						     AgsRecall *parent,
						     AgsPort *port,
						     unsigned char *message, guint message_size,
						     gchar *type_tag,
						     gchar *path, guint path_offset);

void ags_osc_meter_controller_expand_path_audio(AgsAudio *audio,
						gchar *path,
						gchar ***strv);
void ags_osc_meter_controller_expand_path_channel(AgsChannel *channel,
						  gchar *path,
						  gchar ***strv);

void ags_osc_meter_controller_expand_path_recall(AgsRecall *recall,
						 gchar *path,
						 gchar ***strv);

void ags_osc_meter_controller_expand_path_port(AgsPort *port,
					       gchar *path,
					       gchar ***strv);

void ags_osc_meter_controller_expand_path(gchar *path,
					  gchar ***strv);

gpointer ags_osc_meter_controller_monitor_meter_enable(AgsOscMeterController *osc_meter_controller,
						       AgsOscConnection *osc_connection,
						       unsigned char *message, guint message_size,
						       gchar *type_tag,
						       gchar *path);
gpointer ags_osc_meter_controller_monitor_meter_disable(AgsOscMeterController *osc_meter_controller,
							AgsOscConnection *osc_connection,
							unsigned char *message, guint message_size,
							gchar *type_tag,
							gchar *path);

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

static pthread_mutex_t regex_mutex = PTHREAD_MUTEX_INITIALIZER;

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
		     (GDestroyNotify) ags_osc_meter_controller_monitor_free);

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
		     (GDestroyNotify) ags_osc_meter_controller_monitor_free);
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

    monitor = 
      start_monitor = g_list_copy(osc_meter_controller->monitor);

    while(monitor != NULL){
      ags_osc_meter_controller_monitor_ref(monitor->data);
      
      monitor = monitor->next;
    }
    
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
      
      pthread_mutex_lock(osc_controller_mutex);
      
      osc_connection = AGS_OSC_METER_CONTROLLER_MONITOR(monitor->data)->osc_connection;

      path = AGS_OSC_METER_CONTROLLER_MONITOR(monitor->data)->path;
      port = AGS_OSC_METER_CONTROLLER_MONITOR(monitor->data)->port;

      pthread_mutex_unlock(osc_controller_mutex);

      /*  */
      osc_response = ags_osc_response_new();
      
      packet = (unsigned char *) malloc(AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
      memset(packet, 0, AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
      
      g_object_set(osc_response,
		   "packet", packet,
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

	guint length;
	guint i;

	/* message type tag */
	if(packet_size + (4 * (guint) ceil((double) (port_value_length + 5) / 4.0)) > real_packet_size){
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_ERROR);

	  g_object_set(osc_response,
		       "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		       NULL);

	  ags_osc_connection_write_response(osc_connection,
					    (GObject *) osc_response);
	    
	  g_object_run_dispose(osc_response);
	  g_object_unref(osc_response);
	  
	  /* iterate */
	  monitor = monitor->next;

	  continue;
	}

	type_tag = packet + packet_size; // (gchar *) malloc((port_value_length + 5) * sizeof(gchar));

	type_tag[0] = ',';
	type_tag[1] = 's';
	type_tag[2] = '[';
	type_tag[port_value_length + 3] = ']';

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
					      (GObject *) osc_response);
	    
	    g_object_run_dispose(osc_response);
	    g_object_unref(osc_response);

	    /* iterate */
	    monitor = monitor->next;

	    continue;
	  }

	  pthread_mutex_lock(osc_controller_mutex);
	  
	  ags_osc_buffer_util_put_string(packet + packet_size,
					 path, -1);

	  pthread_mutex_unlock(osc_controller_mutex);

	  /* node argument */
	  packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

	  if(packet_size + (4 * (guint) ceil((double) (port_value_length * 8) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    ags_osc_connection_write_response(osc_connection,
					      (GObject *) osc_response);
	    
	    g_object_run_dispose(osc_response);
	    g_object_unref(osc_response);
	    
	    /* iterate */
	    monitor = monitor->next;

	    continue;
	  }
	  
	  pthread_mutex_lock(port_mutex);

	  for(i = 0; i < port_value_length; i++){
	    gdouble value;

	    value = port->port_value.ags_port_double_ptr[i];
	    
	    ags_osc_buffer_util_put_double(packet + packet_size + (i * 8),
					   value);
	  }

	  pthread_mutex_unlock(port_mutex);

	  packet_size += (port_value_length * 8);
	  
	  /* packet size */
	  ags_osc_buffer_util_put_int32(packet,
					packet_size);
	}else{
	  g_warning("unsupported port type");
	}  
      }else{
	if(port_value_type == G_TYPE_FLOAT){
	  gfloat value;
	  guint length;
	  
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
					      (GObject *) osc_response);
	    
	    g_object_run_dispose(osc_response);
	    g_object_unref(osc_response);
	    
	    /* iterate */
	    monitor = monitor->next;

	    continue;
	  }

	  pthread_mutex_lock(osc_controller_mutex);
	  
	  ags_osc_buffer_util_put_string(packet + packet_size,
					 path, -1);

	  pthread_mutex_unlock(osc_controller_mutex);

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
    
      g_object_set(osc_response,
		   "packet-size", packet_size,
		   NULL);

      /* write response */
      ags_osc_connection_write_response(osc_connection,
					(GObject *) osc_response);
      g_object_run_dispose(osc_response);
      g_object_unref(osc_response);
      
      /* iterate */
      monitor = monitor->next;
    }

    g_list_free_full(start_monitor,
		     (GDestroyNotify) ags_osc_meter_controller_monitor_unref);
    
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

  g_atomic_int_set(&(monitor->ref_count), 0);
  
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
  if(monitor == NULL){
    return;
  }

  if(monitor->osc_connection != NULL){
    g_object_unref(monitor->osc_connection);
  }
  
  g_free(monitor->path);

  if(monitor->port != NULL){
    g_object_unref(monitor->port);
  }
  
  free(monitor);
}

/**
 * ags_osc_meter_controller_monitor_ref:
 * @monitor: the #AgsOscMeterControllerMonitor-struct
 * 
 * Increase reference count of @monitor.
 * 
 * Since: 2.1.0
 */
void
ags_osc_meter_controller_monitor_ref(AgsOscMeterControllerMonitor *monitor)
{
  if(monitor == NULL){
    return;
  }
  
  g_atomic_int_inc(&(monitor->ref_count));
}

/**
 * ags_osc_meter_controller_monitor_unref:
 * @monitor: the #AgsOscMeterControllerMonitor-struct
 * 
 * Decrease reference count of @monitor. If ref count is less or equal 0
 * @monitor is freed.
 * 
 * Since: 2.1.0
 */
void
ags_osc_meter_controller_monitor_unref(AgsOscMeterControllerMonitor *monitor)
{
  if(monitor == NULL){
    return;
  }
  
  if(g_atomic_int_dec_and_test(&(monitor->ref_count)) ||
     g_atomic_int_get(&(monitor->ref_count)) < 0){
    ags_osc_meter_controller_monitor_free(monitor);
  }
}

/**
 * ags_osc_meter_controller_monitor_find_path:
 * @monitor: the #GList-struct containing #AgsOscMeterControllerMonitor-struct
 * @path: the path as string
 * 
 * Find @path in @monitor.
 * 
 * Returns: the next matching #GList-struct containing #AgsOscMeterControllerMonitor-struct
 * 
 * Since: 2.1.0
 */
GList*
ags_osc_meter_controller_monitor_find_path(GList *monitor,
					   gchar *path)
{
  if(monitor == NULL ||
     path == NULL){
    return(NULL);
  }

  while(monitor != NULL){
    if(!g_strcmp0(AGS_OSC_METER_CONTROLLER_MONITOR(monitor->data)->path,
		  path)){
      return(monitor);
    }

    monitor = monitor->next;
  }
  
  return(NULL);
}

/**
 * ags_osc_meter_controller_monitor_find_port:
 * @monitor: the #GList-struct containing #AgsOscMeterControllerMonitor-struct
 * @port: the #AgsPort
 * 
 * Find @port in @monitor.
 * 
 * Returns: the next matching #GList-struct containing #AgsOscMeterControllerMonitor-struct
 * 
 * Since: 2.1.0
 */
GList*
ags_osc_meter_controller_monitor_find_port(GList *monitor,
					   AgsPort *port)
{
  if(monitor == NULL ||
     port == NULL){
    return(NULL);
  }

  while(monitor != NULL){
    if(AGS_OSC_METER_CONTROLLER_MONITOR(monitor->data)->port == port){
      return(monitor);
    }

    monitor = monitor->next;
  }
  
  return(NULL);
}

/**
 * ags_osc_meter_controller_monitor_add_monitor:
 * @osc_meter_controller: the #AgsOscMeterController
 * @monitor: the #AgsOscMeterControllerMonitor-struct
 * 
 * Add @monitor to @osc_meter_controller.
 * 
 * Since: 2.1.0
 */
void
ags_osc_meter_controller_add_monitor(AgsOscMeterController *osc_meter_controller,
				     AgsOscMeterControllerMonitor *monitor)
{
  pthread_mutex_t *osc_controller_mutex;

  if(!AGS_IS_OSC_METER_CONTROLLER(osc_meter_controller)){
    return;
  }

  /* get OSC meter controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_meter_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());

  /* add monitor */
  pthread_mutex_lock(osc_controller_mutex);

  if(g_list_find(osc_meter_controller->monitor, monitor) == NULL){
    ags_osc_meter_controller_monitor_ref(monitor);
    
    osc_meter_controller->monitor = g_list_prepend(osc_meter_controller->monitor, monitor);
  }
  
  pthread_mutex_unlock(osc_controller_mutex);
}

/**
 * ags_osc_meter_controller_monitor_remove_monitor:
 * @osc_meter_controller: the #AgsOscMeterController
 * @monitor: the #AgsOscMeterControllerMonitor-struct
 * 
 * Remove @monitor from @osc_meter_controller.
 * 
 * Since: 2.1.0
 */
void
ags_osc_meter_controller_remove_monitor(AgsOscMeterController *osc_meter_controller,
					AgsOscMeterControllerMonitor *monitor)
{
  pthread_mutex_t *osc_controller_mutex;

  if(!AGS_IS_OSC_METER_CONTROLLER(osc_meter_controller)){
    return;
  }

  /* get OSC meter controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_meter_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());

  /* remove monitor */
  pthread_mutex_lock(osc_controller_mutex);

  if(g_list_find(osc_meter_controller->monitor, monitor) != NULL){
    osc_meter_controller->monitor = g_list_remove(osc_meter_controller->monitor, monitor);

    ags_osc_meter_controller_monitor_unref(monitor);
  }
  
  pthread_mutex_unlock(osc_controller_mutex);
}

/**
 * ags_osc_meter_controller_monitor_remove_monitor:
 * @osc_meter_controller: the #AgsOscMeterController
 * @osc_connection: the #AgsOscConnection
 * @port: the #AgsPort
 * 
 * Check if there is a monitor matching @osc_connection and @port in @osc_meter_controller.
 * 
 * Returns: %TRUE if found, otherwise %FALSE
 * 
 * Since: 2.1.0
 */
gboolean
ags_osc_meter_controller_contains_monitor(AgsOscMeterController *osc_meter_controller,
					  AgsOscConnection *osc_connection,
					  AgsPort *port)
{
  GList *monitor;
  
  pthread_mutex_t *osc_controller_mutex;

  if(!AGS_IS_OSC_METER_CONTROLLER(osc_meter_controller) ||
     !AGS_IS_OSC_CONNECTION(osc_connection) ||
     !AGS_IS_PORT(port)){
    return(FALSE);
  }
  
  /* get OSC meter controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_meter_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());

  /* check if contains OSC connection and port */
  pthread_mutex_lock(osc_controller_mutex);

  monitor = osc_meter_controller->monitor;

  while(monitor != NULL){
    if(AGS_OSC_METER_CONTROLLER_MONITOR(monitor->data)->osc_connection == osc_connection &&
       AGS_OSC_METER_CONTROLLER_MONITOR(monitor->data)->port == port){
      pthread_mutex_unlock(osc_controller_mutex);

      return(TRUE);
    }

    monitor = monitor->next;
  }
  
  pthread_mutex_unlock(osc_controller_mutex);

  return(FALSE);
}

void
ags_osc_meter_controller_real_start_monitor(AgsOscMeterController *osc_meter_controller)
{
  pthread_mutex_t *osc_controller_mutex;

  /* get OSC meter controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_meter_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());

  /* test if already started */
  pthread_mutex_lock(osc_controller_mutex);
    
  if(ags_osc_meter_controller_test_flags(osc_meter_controller, AGS_OSC_METER_CONTROLLER_MONITOR_STARTED)){
    pthread_mutex_unlock(osc_controller_mutex);
    
    return;
  }

  ags_osc_meter_controller_set_flags(osc_meter_controller, AGS_OSC_METER_CONTROLLER_MONITOR_STARTED);
  
  pthread_mutex_unlock(osc_controller_mutex);

  /* create monitor thread */
  pthread_create(osc_meter_controller->monitor_thread, NULL,
		 ags_osc_meter_controller_monitor_thread, osc_meter_controller);
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
		osc_meter_controller_signals[START_MONITOR], 0);
  g_object_unref((GObject *) osc_meter_controller);
}

void
ags_osc_meter_controller_real_stop_monitor(AgsOscMeterController *osc_meter_controller)
{
  if(!ags_osc_meter_controller_test_flags(osc_meter_controller, AGS_OSC_METER_CONTROLLER_MONITOR_RUNNING)){
    return;
  }

  ags_osc_meter_controller_set_flags(osc_meter_controller, AGS_OSC_METER_CONTROLLER_MONITOR_TERMINATING);
  ags_osc_meter_controller_unset_flags(osc_meter_controller, AGS_OSC_METER_CONTROLLER_MONITOR_RUNNING);

  /* join thread */
  pthread_join(osc_meter_controller->monitor_thread[0], NULL);
  
  ags_osc_meter_controller_unset_flags(osc_meter_controller, (AGS_OSC_METER_CONTROLLER_MONITOR_STARTED |
							      AGS_OSC_METER_CONTROLLER_MONITOR_TERMINATING));
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
		osc_meter_controller_signals[STOP_MONITOR], 0);
  g_object_unref((GObject *) osc_meter_controller);
}

gpointer
ags_osc_meter_controller_monitor_meter_audio(AgsOscMeterController *osc_meter_controller,
					     AgsOscConnection *osc_connection,
					     AgsAudio *audio,
					     unsigned char *message, guint message_size,
					     gchar *type_tag,
					     gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  AgsApplicationContext *application_context;  

  GList *start_response;

  unsigned char *packet;

  guint real_packet_size;
  guint packet_size;
  gint nth_audio;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }
  
  start_response = NULL;

  application_context = ags_application_context_get_instance();

  real_packet_size = 0;
  packet_size = 0;
  
  if(!strncmp(path + path_offset,
	      "/AgsOutput",
	      10) ||
     !strncmp(path + path_offset,
	      "/AgsInput",
	      9)){
    AgsChannel *start_channel;
    AgsChannel *channel, *next_channel;
      
    regmatch_t match_arr[2];

    static regex_t single_access_regex;
    static regex_t range_access_regex;
    static regex_t voluntary_access_regex;
    static regex_t more_access_regex;
    static regex_t wildcard_access_regex;
    
    static gboolean regex_compiled = FALSE;

    static const gchar *single_access_pattern = "^\\[([0-9]+)\\]";
    static const gchar *range_access_pattern = "^\\[([0-9]+)\\-([0-9]+)\\]";
    static const gchar *voluntary_access_pattern = "^\\[(\\?)\\]";
    static const gchar *more_access_pattern = "^\\[(\\+)\\]";
    static const gchar *wildcard_access_pattern = "^\\[(\\*)\\]";
    
    static const size_t max_matches = 2;
    static const size_t index_max_matches = 1;

    start_channel = NULL;
    
    if(!strncmp(path + path_offset,
		"/AgsOutput",
		10)){
      path_offset += 10;
      
      g_object_get(audio,
		   "output", &start_channel,
		   NULL);
    }else{
      path_offset += 9;
      
      g_object_get(audio,
		   "input", &start_channel,
		   NULL);
    }
     
    /* compile regex */
    pthread_mutex_lock(&regex_mutex);
  
    if(!regex_compiled){
      regex_compiled = TRUE;
      
      ags_regcomp(&single_access_regex, single_access_pattern, REG_EXTENDED);
      ags_regcomp(&range_access_regex, range_access_pattern, REG_EXTENDED);
      ags_regcomp(&voluntary_access_regex, voluntary_access_pattern, REG_EXTENDED);
      ags_regcomp(&more_access_regex, more_access_pattern, REG_EXTENDED);
      ags_regcomp(&wildcard_access_regex, wildcard_access_pattern, REG_EXTENDED);
    }

    pthread_mutex_unlock(&regex_mutex);

    if(ags_regexec(&single_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      gchar *endptr;
      
      guint i_stop;

      endptr = NULL;
      i_stop = g_ascii_strtoull(path + path_offset + 1,
				&endptr,
				10);
      
      channel = ags_channel_nth(start_channel,
				i_stop);

      path_offset += ((endptr + 1) - (path + path_offset));

      start_response = ags_osc_meter_controller_monitor_meter_channel(osc_meter_controller,
								      osc_connection,
								      channel,
								      message, message_size,
								      type_tag,
								      path, path_offset);

      /* unref */
      if(start_channel != NULL){
	g_object_unref(start_channel);
      }

      if(channel != NULL){
	g_object_unref(channel);
      }
    }else if(ags_regexec(&range_access_regex, path + path_offset, max_matches, match_arr, 0) == 0){
      gchar *endptr;
      
      guint i;
      guint i_start, i_stop;

      endptr = NULL;
      i_start = g_ascii_strtoull(path + path_offset + 1,
				 &endptr,
				 10);

      i_stop = g_ascii_strtoull(endptr + 1,
				&endptr,
				10);

      path_offset += ((endptr + 1) - (path + path_offset));

      channel = ags_channel_nth(start_channel,
				i_start);

      next_channel = NULL;
      
      for(i = i_start; i <= i_stop && channel != NULL; i++){
	GList *retval;
	
	retval = ags_osc_meter_controller_monitor_meter_channel(osc_meter_controller,
								osc_connection,
								channel,
								message, message_size,
								type_tag,
								path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}

	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      /* unref */
      if(start_channel != NULL){
	g_object_unref(start_channel);
      }

      if(next_channel != NULL){
	g_object_unref(next_channel);
      }
    }else if(ags_regexec(&voluntary_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      path_offset += 3;

      if(start_channel != NULL){
	GList *retval;
	
	retval = ags_osc_meter_controller_monitor_meter_channel(osc_meter_controller,
								osc_connection,
								start_channel,
								message, message_size,
								type_tag,
								path, path_offset);

	/* unref */
	g_object_unref(start_channel);
      }else{
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_OK);

	return(start_response);
      }
    }else if(ags_regexec(&more_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      path_offset += 3;

      if(start_channel == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		     NULL);

	return(start_response);
      }

      /* prepare */
      channel = start_channel;
      g_object_ref(channel);

      next_channel = NULL;
      
      while(channel != NULL){
	GList *retval;
	
	retval = ags_osc_meter_controller_monitor_meter_channel(osc_meter_controller,
								osc_connection,
								channel,
								message, message_size,
								type_tag,
								path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      /* unref */
      g_object_unref(start_channel);

      if(next_channel != NULL){
	g_object_unref(next_channel);
      }
    }else if(ags_regexec(&wildcard_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      path_offset += 3;

      if(start_channel == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_OK);

	return(start_response);
      }

      channel = start_channel;
      g_object_ref(channel);

      next_channel = NULL;
      
      while(channel != NULL){
	GList *retval;
	
	retval = ags_osc_meter_controller_monitor_meter_channel(osc_meter_controller,
								osc_connection,
								channel,
								message, message_size,
								type_tag,
								path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      /* unref */
      g_object_unref(start_channel);

      if(next_channel != NULL){
	g_object_unref(next_channel);
      }
    }else{
      osc_response = ags_osc_response_new();
      start_response = g_list_prepend(start_response,
				      osc_response);
      
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		   NULL);

      /* unref */
      g_object_unref(start_channel);
      
      return(start_response);
    }
  }else{
    GType recall_type;
    
    GList *start_play, *play;
    GList *start_recall, *recall;

    regmatch_t match_arr[3];

    gchar *type_name;    
    gchar *str;

    guint type_name_length;
    guint str_length;
    
    static regex_t recall_regex;
    static regex_t single_access_regex;
    static regex_t range_access_regex;
    static regex_t voluntary_access_regex;
    static regex_t more_access_regex;
    static regex_t wildcard_access_regex;
    
    static gboolean regex_compiled = FALSE;

    static const gchar *recall_pattern = "^\\/([a-zA-Z]+)(\\/|\\[[0-9]+\\]|\\[[0-9]+\\-[0-9]+\\]|\\[\\?\\]|\\[\\+\\]|\\[\\*\\]|:)";
    static const gchar *single_access_pattern = "^\\[([0-9]+)\\]";
    static const gchar *range_access_pattern = "^\\[([0-9]+)\\-([0-9]+)\\]";
    static const gchar *voluntary_access_pattern = "^\\[(\\?)\\]";
    static const gchar *more_access_pattern = "^\\[(\\+)\\]";
    static const gchar *wildcard_access_pattern = "^\\[(\\*)\\]";
    
    static const size_t max_matches = 3;
    static const size_t index_max_matches = 2;

    /* compile regex */
    pthread_mutex_lock(&regex_mutex);
  
    if(!regex_compiled){
      regex_compiled = TRUE;
    
      ags_regcomp(&recall_regex, recall_pattern, REG_EXTENDED);

      ags_regcomp(&single_access_regex, single_access_pattern, REG_EXTENDED);
      ags_regcomp(&range_access_regex, range_access_pattern, REG_EXTENDED);
      ags_regcomp(&voluntary_access_regex, voluntary_access_pattern, REG_EXTENDED);
      ags_regcomp(&more_access_regex, more_access_pattern, REG_EXTENDED);
      ags_regcomp(&wildcard_access_regex, wildcard_access_pattern, REG_EXTENDED);
    }

    pthread_mutex_unlock(&regex_mutex);

    if(ags_regexec(&recall_regex, path + path_offset, max_matches, match_arr, 0) == 0){
      type_name_length = match_arr[1].rm_eo - match_arr[1].rm_so;
      type_name = g_strndup(path + path_offset + 1,
			    type_name_length);

      str_length = match_arr[2].rm_eo - match_arr[2].rm_so;
      str = g_strndup(path + path_offset + 1 + type_name_length,
		      str_length);
    }else{
      osc_response = ags_osc_response_new();
      start_response = g_list_prepend(start_response,
				      osc_response);

      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_PATH,
		   NULL);

      return(start_response);
    }

    recall_type = g_type_from_name(type_name);
    
    g_object_get(audio,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);

    if(ags_regexec(&single_access_regex, str, index_max_matches, match_arr, 0) == 0){
      guint i;
      guint i_stop;

      path_offset += (type_name_length + 1) + str_length;
      
      i_stop = g_ascii_strtoull(str + 1,
				NULL,
				10);

      play = start_play;
      recall = start_recall;
      
      for(i = 0; i <= i_stop; i++){
	play = ags_recall_template_find_type(play, recall_type);
	recall = ags_recall_template_find_type(recall, recall_type);

	if(i + 1 < i_stop){
	  if(play != NULL){
	    play = play->next;
	  }
	  
	  if(recall != NULL){
	    recall = recall->next;
	  }
	}
      }
      
      if(play != NULL){
	AgsRecall *current;

	GList *retval;

	current = play->data;	
	retval = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
							       osc_connection,
							       current,
							       message, message_size,
							       type_tag,
							       path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}
      }
      
      if(recall != NULL){
	AgsRecall *current;

	GList *retval;

	current = recall->data;
	retval = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
							       osc_connection,
							       current,
							       message, message_size,
							       type_tag,
							       path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}
      }
    }else if(ags_regexec(&range_access_regex, str, max_matches, match_arr, 0) == 0){
      gchar *endptr;
      
      guint i;
      guint i_start, i_stop;

      path_offset += (type_name_length + 1) + str_length;

      endptr = NULL;
      i_start = g_ascii_strtoull(str + 1,
				 &endptr,
				 10);

      i_stop = g_ascii_strtoull(endptr + 1,
				NULL,
				10);
      
      play = start_play;
      recall = start_recall;
      
      for(i = 0; i <= i_stop; i++){
	play = ags_recall_template_find_type(play, recall_type);
	recall = ags_recall_template_find_type(recall, recall_type);

	if(i >= i_start){
	  if(play != NULL){
	    AgsRecall *current;

	    GList *retval;

	    current = play->data;	
	    retval = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
								   osc_connection,
								   current,
								   message, message_size,
								   type_tag,
								   path, path_offset);

	    if(start_response != NULL){
	      start_response = g_list_concat(start_response,
					     retval);
	    }else{
	      start_response = retval;
	    }
	  }
      
	  if(recall != NULL){
	    AgsRecall *current;

	    GList *retval;

	    current = recall->data;
	    retval = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
								   osc_connection,
								   current,
								   message, message_size,
								   type_tag,
								   path, path_offset);

	    if(start_response != NULL){
	      start_response = g_list_concat(start_response,
					     retval);
	    }else{
	      start_response = retval;
	    }
	  }
	}
	
	if(i + 1 < i_stop){
	  if(play != NULL){
	    play = play->next;
	  }
	  
	  if(recall != NULL){
	    recall = recall->next;
	  }
	}
      }
    }else if(ags_regexec(&voluntary_access_regex, str, index_max_matches, match_arr, 0) == 0){
      path_offset += (type_name_length + 1) + str_length;

      play = start_play;
      recall = start_recall;

      play = ags_recall_template_find_type(play, recall_type);
      recall = ags_recall_template_find_type(recall, recall_type);

      if(play != NULL){
	AgsRecall *current;

	current = play->data;	
	start_response = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
								       osc_connection,
								       current,
								       message, message_size,
								       type_tag,
								       path, path_offset);
      }
      
      if(recall != NULL){
	AgsRecall *current;

	current = recall->data;
	start_response = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
								       osc_connection,
								       current,
								       message, message_size,
								       type_tag,
								       path, path_offset);
      }

      if(play == NULL && recall == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_OK);

	g_list_free_full(start_play,
			 g_object_unref);
	g_list_free_full(start_recall,
			 g_object_unref);

	return(start_response);
      }
    }else if(ags_regexec(&more_access_regex, str, index_max_matches, match_arr, 0) == 0){
      path_offset += (type_name_length + 1) + str_length;

      play = start_play;
      recall = start_recall;

      play = ags_recall_template_find_type(play, recall_type);
      recall = ags_recall_template_find_type(recall, recall_type);

      if(play == NULL && recall == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		     NULL);

	g_list_free_full(start_play,
			 g_object_unref);
	g_list_free_full(start_recall,
			 g_object_unref);

	return(start_response);
      }
      
      if(play != NULL){
	AgsRecall *current;

	current = play->data;	
	start_response = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
								       osc_connection,
								       current,
								       message, message_size,
								       type_tag,
								       path, path_offset);
      }
      
      if(recall != NULL){
	AgsRecall *current;

	GList *retval;

	current = recall->data;
	retval = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
							       osc_connection,
							       current,
							       message, message_size,
							       type_tag,
							       path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}
      }
      
      while(play != NULL || recall != NULL){
	play = ags_recall_template_find_type(play, recall_type);
	recall = ags_recall_template_find_type(recall, recall_type);

	if(play != NULL){
	  AgsRecall *current;

	  GList *retval;

	  current = play->data;	
	  retval = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
								 osc_connection,
								 current,
								 message, message_size,
								 type_tag,
								 path, path_offset);

	  start_response = g_list_concat(start_response,
					 retval);
	}
      
	if(recall != NULL){
	  AgsRecall *current;

	  GList *retval;

	  current = recall->data;
	  retval = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
								 osc_connection,
								 current,
								 message, message_size,
								 type_tag,
								 path, path_offset);

	  start_response = g_list_concat(start_response,
					 retval);
	}

	if(play != NULL){
	  play = play->next;
	}
	  
	if(recall != NULL){
	  recall = recall->next;
	}
      }
    }else if(ags_regexec(&wildcard_access_regex, str, index_max_matches, match_arr, 0) == 0){
      path_offset += (type_name_length + 1) + str_length;

      play = start_play;
      recall = start_recall;

      while(play != NULL || recall != NULL){
	play = ags_recall_template_find_type(play, recall_type);
	recall = ags_recall_template_find_type(recall, recall_type);

	if(play != NULL){
	  AgsRecall *current;

	  GList *retval;
	  
	  current = play->data;	
	  retval = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
								 osc_connection,
								 current,
								 message, message_size,
								 type_tag,
								 path, path_offset);

	  if(start_response != NULL){
	    start_response = g_list_concat(start_response,
					   retval);
	  }else{
	    start_response = retval;
	  }
	}
      
	if(recall != NULL){
	  AgsRecall *current;

	  GList *retval;
	  
	  current = recall->data;
	  retval = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
								 osc_connection,
								 current,
								 message, message_size,
								 type_tag,
								 path, path_offset);

	  if(start_response != NULL){
	    start_response = g_list_concat(start_response,
					   retval);
	  }else{
	    start_response = retval;
	  }
	}

	if(play != NULL){
	  play = play->next;
	}
	  
	if(recall != NULL){
	  recall = recall->next;
	}
      }
    }else{
      osc_response = ags_osc_response_new();
      start_response = g_list_prepend(start_response,
				      osc_response);

      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		   NULL);

      g_list_free_full(start_play,
		       g_object_unref);
      g_list_free_full(start_recall,
		       g_object_unref);

      return(start_response);
    }

    g_list_free_full(start_play,
		     g_object_unref);
    g_list_free_full(start_recall,
		     g_object_unref);
  }

  return(start_response);
}

gpointer
ags_osc_meter_controller_monitor_meter_channel(AgsOscMeterController *osc_meter_controller,
					       AgsOscConnection *osc_connection,
					       AgsChannel *channel,
					       unsigned char *message, guint message_size,
					       gchar *type_tag,
					       gchar *path, guint path_offset)
{
  AgsAudio *audio;
  
  AgsOscResponse *osc_response;

  AgsApplicationContext *application_context;  

  GType recall_type;
  
  GList *start_response;
  GList *start_play, *play;
  GList *start_recall, *recall;

  regmatch_t match_arr[3];

  gchar *type_name;    
  gchar *str;

  guint type_name_length;
  guint str_length;
    
  static regex_t recall_regex;
  static regex_t single_access_regex;
  static regex_t range_access_regex;
  static regex_t voluntary_access_regex;
  static regex_t more_access_regex;
  static regex_t wildcard_access_regex;
    
  static gboolean regex_compiled = FALSE;

  static const gchar *recall_pattern = "^\\/([a-zA-Z]+)(\\/|\\[[0-9]+\\]|\\[[0-9]+\\-[0-9]+\\]|\\[\\?\\]|\\[\\+\\]|\\[\\*\\]|:)";
  static const gchar *single_access_pattern = "^\\[([0-9]+)\\]";
  static const gchar *range_access_pattern = "^\\[([0-9]+)\\-([0-9]+)\\]";
  static const gchar *voluntary_access_pattern = "^\\[(\\?)\\]";
  static const gchar *more_access_pattern = "^\\[(\\+)\\]";
  static const gchar *wildcard_access_pattern = "^\\[(\\*)\\]";
    
  static const size_t max_matches = 3;
  static const size_t index_max_matches = 2;
  
  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }
  
  start_response = NULL;

  /* compile regex */
  pthread_mutex_lock(&regex_mutex);
  
  if(!regex_compiled){
    regex_compiled = TRUE;
    
    ags_regcomp(&recall_regex, recall_pattern, REG_EXTENDED);

    ags_regcomp(&single_access_regex, single_access_pattern, REG_EXTENDED);
    ags_regcomp(&range_access_regex, range_access_pattern, REG_EXTENDED);
    ags_regcomp(&voluntary_access_regex, voluntary_access_pattern, REG_EXTENDED);
    ags_regcomp(&more_access_regex, more_access_pattern, REG_EXTENDED);
    ags_regcomp(&wildcard_access_regex, wildcard_access_pattern, REG_EXTENDED);
  }

  pthread_mutex_unlock(&regex_mutex);

  if(ags_regexec(&recall_regex, path + path_offset, max_matches, match_arr, 0) == 0){
    type_name_length = match_arr[1].rm_eo - match_arr[1].rm_so;
    type_name = g_strndup(path + path_offset + 1,
			  type_name_length);

    str_length = match_arr[2].rm_eo - match_arr[2].rm_so;
    str = g_strndup(path + path_offset + 1 + type_name_length,
		    str_length);
  }else{
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);

    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_PATH,
		 NULL);

    return(start_response);
  }

  recall_type = g_type_from_name(type_name);
    
  g_object_get(channel,
	       "play", &start_play,
	       "recall", &start_recall,
	       NULL);

  if(ags_regexec(&single_access_regex, str, index_max_matches, match_arr, 0) == 0){
    guint i;
    guint i_stop;

    path_offset += (type_name_length + 1) + str_length;
      
    i_stop = g_ascii_strtoull(str + 1,
			      NULL,
			      10);

    play = start_play;
    recall = start_recall;
      
    for(i = 0; i <= i_stop; i++){
      play = ags_recall_template_find_type(play, recall_type);
      recall = ags_recall_template_find_type(recall, recall_type);

      if(i + 1 < i_stop){
	if(play != NULL){
	  play = play->next;
	}
	  
	if(recall != NULL){
	  recall = recall->next;
	}
      }
    }
      
    if(play != NULL){
      AgsRecall *current;

      GList *retval;

      current = play->data;	
      retval = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
							     osc_connection,
							     current,
							     message, message_size,
							     type_tag,
							     path, path_offset);

      if(start_response != NULL){
	start_response = g_list_concat(start_response,
				       retval);
      }else{
	start_response = retval;
      }
    }
      
    if(recall != NULL){
      AgsRecall *current;

      GList *retval;

      current = recall->data;
      retval = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
							     osc_connection,
							     current,
							     message, message_size,
							     type_tag,
							     path, path_offset);

      if(start_response != NULL){
	start_response = g_list_concat(start_response,
				       retval);
      }else{
	start_response = retval;
      }
    }
  }else if(ags_regexec(&range_access_regex, str, max_matches, match_arr, 0) == 0){
    gchar *endptr;
      
    guint i;
    guint i_start, i_stop;

    path_offset += (type_name_length + 1) + str_length;

    endptr = NULL;
    i_start = g_ascii_strtoull(str + 1,
			       &endptr,
			       10);

    i_stop = g_ascii_strtoull(endptr + 1,
			      NULL,
			      10);
      
    play = start_play;
    recall = start_recall;
      
    for(i = 0; i <= i_stop; i++){
      play = ags_recall_template_find_type(play, recall_type);
      recall = ags_recall_template_find_type(recall, recall_type);

      if(i >= i_start){
	if(play != NULL){
	  AgsRecall *current;

	  GList *retval;

	  current = play->data;	
	  retval = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
								 osc_connection,
								 current,
								 message, message_size,
								 type_tag,
								 path, path_offset);

	  if(start_response != NULL){
	    start_response = g_list_concat(start_response,
					   retval);
	  }else{
	    start_response = retval;
	  }
	}
      
	if(recall != NULL){
	  AgsRecall *current;

	  GList *retval;

	  current = recall->data;
	  retval = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
								 osc_connection,
								 current,
								 message, message_size,
								 type_tag,
								 path, path_offset);

	  if(start_response != NULL){
	    start_response = g_list_concat(start_response,
					   retval);
	  }else{
	    start_response = retval;
	  }
	}
      }
	
      if(i + 1 < i_stop){
	if(play != NULL){
	  play = play->next;
	}
	  
	if(recall != NULL){
	  recall = recall->next;
	}
      }
    }
  }else if(ags_regexec(&voluntary_access_regex, str, index_max_matches, match_arr, 0) == 0){
    path_offset += (type_name_length + 1) + str_length;

    play = start_play;
    recall = start_recall;

    play = ags_recall_template_find_type(play, recall_type);
    recall = ags_recall_template_find_type(recall, recall_type);

    if(play != NULL){
      AgsRecall *current;

      current = play->data;	
      start_response = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
								     osc_connection,
								     current,
								     message, message_size,
								     type_tag,
								     path, path_offset);
    }
      
    if(recall != NULL){
      AgsRecall *current;

      current = recall->data;
      start_response = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
								     osc_connection,
								     current,
								     message, message_size,
								     type_tag,
								     path, path_offset);
    }

    if(play == NULL && recall == NULL){
      osc_response = ags_osc_response_new();
      start_response = g_list_prepend(start_response,
				      osc_response);
      
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_OK);

      g_list_free_full(start_play,
		       g_object_unref);
      g_list_free_full(start_recall,
		       g_object_unref);

      return(start_response);
    }
  }else if(ags_regexec(&more_access_regex, str, index_max_matches, match_arr, 0) == 0){
    path_offset += (type_name_length + 1) + str_length;

    play = start_play;
    recall = start_recall;

    play = ags_recall_template_find_type(play, recall_type);
    recall = ags_recall_template_find_type(recall, recall_type);

    if(play == NULL && recall == NULL){
      osc_response = ags_osc_response_new();
      start_response = g_list_prepend(start_response,
				      osc_response);
      
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		   NULL);

      g_list_free_full(start_play,
		       g_object_unref);
      g_list_free_full(start_recall,
		       g_object_unref);

      return(start_response);
    }
      
    if(play != NULL){
      AgsRecall *current;

      current = play->data;	
      start_response = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
								     osc_connection,
								     current,
								     message, message_size,
								     type_tag,
								     path, path_offset);
    }
      
    if(recall != NULL){
      AgsRecall *current;

      GList *retval;

      current = recall->data;
      retval = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
							     osc_connection,
							     current,
							     message, message_size,
							     type_tag,
							     path, path_offset);

      if(start_response != NULL){
	start_response = g_list_concat(start_response,
				       retval);
      }else{
	start_response = retval;
      }
    }
      
    while(play != NULL || recall != NULL){
      play = ags_recall_template_find_type(play, recall_type);
      recall = ags_recall_template_find_type(recall, recall_type);

      if(play != NULL){
	AgsRecall *current;

	GList *retval;

	current = play->data;	
	retval = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
							       osc_connection,
							       current,
							       message, message_size,
							       type_tag,
							       path, path_offset);

	start_response = g_list_concat(start_response,
				       retval);
      }
      
      if(recall != NULL){
	AgsRecall *current;

	GList *retval;

	current = recall->data;
	retval = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
							       osc_connection,
							       current,
							       message, message_size,
							       type_tag,
							       path, path_offset);

	start_response = g_list_concat(start_response,
				       retval);
      }

      if(play != NULL){
	play = play->next;
      }
	  
      if(recall != NULL){
	recall = recall->next;
      }
    }
  }else if(ags_regexec(&wildcard_access_regex, str, index_max_matches, match_arr, 0) == 0){
    path_offset += (type_name_length + 1) + str_length;

    play = start_play;
    recall = start_recall;

    while(play != NULL || recall != NULL){
      play = ags_recall_template_find_type(play, recall_type);
      recall = ags_recall_template_find_type(recall, recall_type);

      if(play != NULL){
	AgsRecall *current;

	GList *retval;
	  
	current = play->data;	
	retval = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
							       osc_connection,
							       current,
							       message, message_size,
							       type_tag,
							       path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}
      }
      
      if(recall != NULL){
	AgsRecall *current;

	GList *retval;
	  
	current = recall->data;
	retval = ags_osc_meter_controller_monitor_meter_recall(osc_meter_controller,
							       osc_connection,
							       current,
							       message, message_size,
							       type_tag,
							       path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}
      }

      if(play != NULL){
	play = play->next;
      }
	  
      if(recall != NULL){
	recall = recall->next;
      }
    }

    g_list_free_full(start_play,
		       g_object_unref);
    g_list_free_full(start_recall,
		       g_object_unref);
  }else{
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);

    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		 NULL);

    return(start_response);
  }

  return(start_response);
}

gpointer
ags_osc_meter_controller_monitor_meter_recall(AgsOscMeterController *osc_meter_controller,
					      AgsOscConnection *osc_connection,
					      AgsRecall *recall,
					      unsigned char *message, guint message_size,
					      gchar *type_tag,
					      gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  AgsThread *task_thread;
  
  AgsApplicationContext *application_context;

  GList *start_response;
  
  unsigned char *packet;

  guint path_length;
  guint real_packet_size;
  guint packet_size;
  
  if(!AGS_IS_RECALL(recall)){
    return(NULL);
  }

  start_response = NULL;
  
  application_context = ags_application_context_get_instance();

  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));

  real_packet_size = 0;
  packet_size = 0;

  if(!strncmp(path + path_offset,
	      ":",
	      1)){
    guint length;
    
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);

    packet = (unsigned char *) malloc(AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    memset(packet, 0, AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    
    g_object_set(osc_response,
		 "packet", packet,
		 "packet-size", packet_size,
		 NULL);
  
    real_packet_size = AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE;
    
    path_length = 4 * (guint) ceil((double) (strlen(path) + 1) / 4.0);
    path_offset += 1;
	
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_ARGUMENT,
		 NULL);

    g_object_unref(task_thread);
    
    return(start_response);
  }else if(!strncmp(path + path_offset,
		    "/AgsPort",
		    8)){
    GList *start_port, *port;
    
    regmatch_t match_arr[3];

    static regex_t single_access_regex;
    static regex_t range_access_regex;
    static regex_t voluntary_access_regex;
    static regex_t more_access_regex;
    static regex_t wildcard_access_regex;
    
    static gboolean regex_compiled = FALSE;

    static const gchar *single_access_pattern = "^\\[([0-9]+)\\]";
    static const gchar *range_access_pattern = "^\\[([0-9]+)\\-([0-9]+)\\]";
    static const gchar *voluntary_access_pattern = "^\\[(\\?)\\]";
    static const gchar *more_access_pattern = "^\\[(\\+)\\]";
    static const gchar *wildcard_access_pattern = "^\\[(\\*)\\]";
    
    static const size_t max_matches = 3;
    static const size_t index_max_matches = 2;

    path_offset += 8;

    /* compile regex */
    pthread_mutex_lock(&regex_mutex);
  
    if(!regex_compiled){
      regex_compiled = TRUE;
      
      ags_regcomp(&single_access_regex, single_access_pattern, REG_EXTENDED);
      ags_regcomp(&range_access_regex, range_access_pattern, REG_EXTENDED);
      ags_regcomp(&voluntary_access_regex, voluntary_access_pattern, REG_EXTENDED);
      ags_regcomp(&more_access_regex, more_access_pattern, REG_EXTENDED);
      ags_regcomp(&wildcard_access_regex, wildcard_access_pattern, REG_EXTENDED);
    }

    pthread_mutex_unlock(&regex_mutex);
    
    g_object_get(recall,
		 "port", &start_port,
		 NULL);

    port = start_port;
    
    if(ags_regexec(&single_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      AgsPort *current;
      
      gchar *endptr;
      
      guint i_stop;

      endptr = NULL;
      i_stop = g_ascii_strtoull(path + path_offset + 1,
				&endptr,
				10);
      
      current = g_list_nth_data(start_port,
				i_stop);

      path_offset += ((endptr + 1) - (path + path_offset));

      start_response = ags_osc_meter_controller_monitor_meter_port(osc_meter_controller,
								   osc_connection,
								   recall,
								   current,
								   message, message_size,
								   type_tag,
								   path, path_offset);
    }else if(ags_regexec(&range_access_regex, path + path_offset, max_matches, match_arr, 0) == 0){
      gchar *endptr;
      
      guint i;
      guint i_start, i_stop;

      endptr = NULL;
      i_start = g_ascii_strtoull(path + path_offset + 1,
				 &endptr,
				 10);

      i_stop = g_ascii_strtoull(endptr + 1,
				&endptr,
				10);

      path_offset += ((endptr + 1) - (path + path_offset));

      port = g_list_nth(start_port,
			i_start);
      
      for(i = i_start; i <= i_stop; i++){
	GList *retval;
	
	retval = ags_osc_meter_controller_monitor_meter_port(osc_meter_controller,
							     osc_connection,
							     recall,
							     port->data,
							     message, message_size,
							     type_tag,
							     path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}

	port = port->next;
      }
    }else if(ags_regexec(&voluntary_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      path_offset += 3;

      if(start_port != NULL){
	GList *retval;
	
	retval = ags_osc_meter_controller_monitor_meter_port(osc_meter_controller,
							     osc_connection,
							     recall,
							     start_port->data,
							     message, message_size,
							     type_tag,
							     path, path_offset);
      }else{
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_OK);

	g_list_free_full(start_port,
			 g_object_unref);

	g_object_unref(task_thread);
	
	return(start_response);
      }
    }else if(ags_regexec(&more_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      path_offset += 3;

      if(start_port == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		     NULL);

	g_list_free_full(start_port,
			 g_object_unref);

	g_object_unref(task_thread);
	
	return(start_response);
      }

      port = start_port;
      
      while(port != NULL){
	GList *retval;
	
	retval = ags_osc_meter_controller_monitor_meter_port(osc_meter_controller,
							     osc_connection,
							     recall,
							     port->data,
							     message, message_size,
							     type_tag,
							     path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}

	port = port->next;
      }
    }else if(ags_regexec(&wildcard_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      path_offset += 3;

      if(start_port == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_OK);

	g_list_free_full(start_port,
			 g_object_unref);

	g_object_unref(task_thread);
	
	return(start_response);
      }
      
      while(port != NULL){
	GList *retval;
	
	retval = ags_osc_meter_controller_monitor_meter_port(osc_meter_controller,
							     osc_connection,
							     recall,
							     port->data,
							     message, message_size,
							     type_tag,
							     path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}

	port = port->next;
      }
    }else if(path[path_offset] == '[' &&
	     path[path_offset + 1] == '"'){
      AgsPort *current;

      gchar *port_specifier;
      gchar *offset;

      guint length;

      if((offset = index(path + path_offset + 2, '"')) == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);

	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);
	  
	g_list_free_full(start_port,
			 g_object_unref);

	g_object_unref(task_thread);
	
	return(start_response);
      }

      length = offset - (path + path_offset + 2);

      port_specifier = (gchar *) malloc((length + 1) * sizeof(gchar));
      memcpy(port_specifier, path + path_offset + 2, (length) * sizeof(gchar));
      port_specifier[length] = '\0';

      current = NULL;
      port = ags_port_find_specifier(start_port,
				     port_specifier);
	
      if(port != NULL){
	current = port->data;
      }	

      path_offset += (length + 4);
      start_response  = ags_osc_meter_controller_monitor_meter_port(osc_meter_controller,
								    osc_connection,
								    recall,
								    current,
								    message, message_size,
								    type_tag,
								    path, path_offset);
    }else{
      g_list_free_full(start_port,
		       g_object_unref);
      
      osc_response = ags_osc_response_new();
      start_response = g_list_prepend(start_response,
				      osc_response);
      
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		   NULL);

      g_list_free_full(start_port,
		       g_object_unref);
      
      g_object_unref(task_thread);
      
      return(start_response);
    }    

    g_list_free_full(start_port,
		     g_object_unref);
  }else{
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);
      
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		 NULL);

    g_object_unref(task_thread);

    return(start_response);
  }

  g_object_unref(task_thread);
  
  return(start_response);
}

gpointer
ags_osc_meter_controller_monitor_meter_port(AgsOscMeterController *osc_meter_controller,
					    AgsOscConnection *osc_connection,
					    AgsRecall *parent,
					    AgsPort *port,
					    unsigned char *message, guint message_size,
					    gchar *type_tag,
					    gchar *path, guint path_offset)
{
  AgsAudio *audio;
  AgsChannel *channel;
  
  AgsOscResponse *osc_response;

  AgsApplicationContext *application_context;  

  GList *start_response;
  GList *start_list, *list;

  unsigned char *packet;
  gchar *current_path;
  gchar *specifier;
  
  guint real_packet_size;
  guint packet_size;
  gint nth_audio;
  gint nth_channel;
  gint nth_recall;
  gint nth_port;
  
  if(!AGS_IS_PORT(port)){
    return(NULL);
  }

  start_response = NULL;

  application_context = ags_application_context_get_instance();

  if(!strncmp(path + path_offset,
	      ":",
	      1)){
    guint length;
    guint i;
    
    audio = NULL;
    channel = NULL;
    
    if(AGS_IS_RECALL_CHANNEL(parent)){
      g_object_get(parent,
		   "source", &channel,
		   NULL);

      g_object_get(channel,
		   "audio", &audio,
		   "line", &nth_channel,
		   NULL);      

      g_object_get(channel,
		   "play", &start_list,
		   NULL);

      /* find nth recall */
      list = start_list;
      
      for(i = 0; list != NULL; i++){
	list = ags_recall_template_find_type(list,
					     G_OBJECT_TYPE(parent));

	if(list != NULL){
	  if(list->data == parent){
	    break;
	  }
	  
	  list = list->next;
	}
      }

      g_list_free_full(start_list,
		       g_object_unref);

      if(list != NULL){
	nth_recall = i;
      }else{
	g_object_get(channel,
		     "recall", &start_list,
		     NULL);

	list = start_list;
      
	for(i = 0; list != NULL; i++){
	  list = ags_recall_template_find_type(list,
					       G_OBJECT_TYPE(parent));

	  if(list != NULL){
	    if(list->data == parent){
	      break;
	    }
	  
	    list = list->next;
	  }
	}

	g_list_free_full(start_list,
			 g_object_unref);
      
	nth_recall = i;
      }

      g_object_unref(channel);

      g_object_unref(audio);
    }else if(AGS_IS_RECALL_AUDIO(parent)){
      g_object_get(parent,
		   "audio", &audio,
		   NULL);

      g_object_get(audio,
		   "play", &start_list,
		   NULL);

      /* find nth recall */
      list = start_list;
      
      for(i = 0; list != NULL; i++){
	list = ags_recall_template_find_type(list,
					     G_OBJECT_TYPE(parent));

	if(list != NULL){
	  if(list->data == parent){
	    break;
	  }
	  
	  list = list->next;
	}
      }

      g_list_free_full(start_list,
		       g_object_unref);
      
      if(list != NULL){
	nth_recall = i;
      }else{
	g_object_get(channel,
		     "recall", &start_list,
		     NULL);

	list = start_list;
      
	for(i = 0; list != NULL; i++){
	  list = ags_recall_template_find_type(list,
					       G_OBJECT_TYPE(parent));

	  if(list != NULL){
	    if(list->data == parent){
	      break;
	    }
	  
	    list = list->next;
	  }
	}

	g_list_free_full(start_list,
			 g_object_unref);
      
	nth_recall = i;
      }

      g_object_unref(audio);
    }

    g_object_get(parent,
		 "port", &start_list,
		 NULL);

    nth_port = g_list_index(start_list,
			    port);
    
    g_list_free_full(start_list,
		     g_object_unref);
      
    /* get nth audio */
    start_list = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

    nth_audio = g_list_index(start_list,
			     audio);

    g_list_free_full(start_list,
		     g_object_unref);

    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);

    packet = (unsigned char *) malloc(AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    memset(packet, 0, AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    
    g_object_set(osc_response,
		 "packet", packet,
		 "packet-size", packet_size,
		 NULL);
  
    real_packet_size = AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE;
    
    path_offset += 1;

    if(!strncmp(path + path_offset,
		"value",
		6)){
      GType port_value_type;

      guint port_value_length;
      gboolean port_value_is_pointer;
      
      pthread_mutex_t *port_mutex;

      /* get port mutex */
      pthread_mutex_lock(ags_port_get_class_mutex());
      
      port_mutex = port->obj_mutex;
      
      pthread_mutex_unlock(ags_port_get_class_mutex());

      /* create current path */
      pthread_mutex_lock(port_mutex);

      specifier = g_strdup(port->specifier);
      
      pthread_mutex_unlock(port_mutex);
      
      if(channel != NULL){
	current_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[%d]/%s[%d]/%s[%d]/AgsPort[%d]:value",
				       nth_audio,
				       ((AGS_IS_OUTPUT(channel)) ? "AgsOutput": "AgsInput"),
				       nth_channel,
				       G_OBJECT_TYPE_NAME(parent),
				       nth_recall,
				       nth_port);
      }else{
	current_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[%d]/%s[%d]/AgsPort[%d]:value",
				       nth_audio,
				       G_OBJECT_TYPE_NAME(parent),
				       nth_recall,
				       nth_port);
      }

      g_free(specifier);
    }

    if(!ags_osc_meter_controller_contains_monitor(osc_meter_controller,
						  osc_connection,
						  port)){
      AgsOscMeterControllerMonitor *monitor;
      
      /* allocate monitor */
      monitor = ags_osc_meter_controller_monitor_alloc();

      monitor->osc_connection = osc_connection;
      g_object_ref(osc_connection);

      monitor->path = g_strdup(current_path);

      monitor->port = port;
      g_object_ref(port);

      /* add monitor */
      ags_osc_meter_controller_add_monitor(osc_meter_controller,
					   monitor);
    }
  }

  return(start_response);
}

gpointer
ags_osc_meter_controller_monitor_meter_enable(AgsOscMeterController *osc_meter_controller,
					      AgsOscConnection *osc_connection,
					      unsigned char *message, guint message_size,
					      gchar *type_tag,
					      gchar *path)
{
  AgsOscResponse *osc_response;

  AgsApplicationContext *application_context;  
  
  GList *start_response;
  GList *start_audio, *audio;
  
  guint path_offset;

  regmatch_t match_arr[2];

  static regex_t single_access_regex;
  static regex_t range_access_regex;
  static regex_t voluntary_access_regex;
  static regex_t more_access_regex;
  static regex_t wildcard_access_regex;
    
  static gboolean regex_compiled = FALSE;

  static const gchar *single_access_pattern = "^\\[([0-9]+)\\]";
  static const gchar *range_access_pattern = "^\\[([0-9]+)\\-([0-9]+)\\]";
  static const gchar *voluntary_access_pattern = "^\\[(\\?)\\]";
  static const gchar *more_access_pattern = "^\\[(\\+)\\]";
  static const gchar *wildcard_access_pattern = "^\\[(\\*)\\]";
    
  static const size_t max_matches = 2;
  static const size_t index_max_matches = 1;

  start_response = NULL;  

  application_context = ags_application_context_get_instance();

  path_offset = 0;
    
  if(strncmp(path, "/AgsSoundProvider", 17) != 0){
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);

    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_ARGUMENT,
		 NULL);

    return(start_response);
  }
      
  path_offset += 17;
      
  if(strncmp(path + path_offset, "/AgsAudio", 9) != 0){
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);

    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_ARGUMENT,
		 NULL);

    return(start_response);
  }
  
  path_offset += 9;
      
  /* compile regex */
  pthread_mutex_lock(&regex_mutex);
  
  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&single_access_regex, single_access_pattern, REG_EXTENDED);
    ags_regcomp(&range_access_regex, range_access_pattern, REG_EXTENDED);
    ags_regcomp(&voluntary_access_regex, voluntary_access_pattern, REG_EXTENDED);
    ags_regcomp(&more_access_regex, more_access_pattern, REG_EXTENDED);
    ags_regcomp(&wildcard_access_regex, wildcard_access_pattern, REG_EXTENDED);
  }

  pthread_mutex_unlock(&regex_mutex);

  audio = 
    start_audio = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

  if(ags_regexec(&single_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
    AgsAudio *current;
      
    gchar *endptr;
      
    guint i_stop;

    endptr = NULL;
    i_stop = g_ascii_strtoull(path + path_offset + 1,
			      &endptr,
			      10);
      
    current = g_list_nth_data(start_audio,
			      i_stop);

    path_offset += ((endptr + 1) - (path + path_offset));

    start_response = ags_osc_meter_controller_monitor_meter_audio(osc_meter_controller,
								  osc_connection,
								  current,
								  message, message_size,
								  type_tag,
								  path, path_offset);
  }else if(ags_regexec(&range_access_regex, path + path_offset, max_matches, match_arr, 0) == 0){
    gchar *endptr;
      
    guint i;
    guint i_start, i_stop;

    endptr = NULL;
    i_start = g_ascii_strtoull(path + path_offset + 1,
			       &endptr,
			       10);

    i_stop = g_ascii_strtoull(endptr + 1,
			      &endptr,
			      10);

    path_offset += ((endptr + 1) - (path + path_offset));

    audio = g_list_nth(start_audio,
		       i_start);
      
    for(i = i_start; i <= i_stop; i++){
      GList *retval;
	
      retval = ags_osc_meter_controller_monitor_meter_audio(osc_meter_controller,
							    osc_connection,
							    audio->data,
							    message, message_size,
							    type_tag,
							    path, path_offset);

      if(start_response != NULL){
	start_response = g_list_concat(start_response,
				       retval);
      }else{
	start_response = retval;
      }

      audio = audio->next;
    }
  }else if(ags_regexec(&voluntary_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
    path_offset += 3;

    if(start_audio != NULL){
      GList *retval;
	
      retval = ags_osc_meter_controller_monitor_meter_audio(osc_meter_controller,
							    osc_connection,
							    start_audio->data,
							    message, message_size,
							    type_tag,
							    path, path_offset);
    }else{
      osc_response = ags_osc_response_new();
      start_response = g_list_prepend(start_response,
				      osc_response);
      
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_OK);

      g_list_free_full(start_audio,
		       g_object_unref);
      
      return(start_response);
    }
  }else if(ags_regexec(&more_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
    path_offset += 3;

    if(start_audio == NULL){
      osc_response = ags_osc_response_new();
      start_response = g_list_prepend(start_response,
				      osc_response);
      
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		   NULL);

      return(start_response);
    }

    audio = start_audio;
      
    while(audio != NULL){
      GList *retval;
	
      retval = ags_osc_meter_controller_monitor_meter_audio(osc_meter_controller,
							    osc_connection,
							    audio->data,
							    message, message_size,
							    type_tag,
							    path, path_offset);

      if(start_response != NULL){
	start_response = g_list_concat(start_response,
				       retval);
      }else{
	start_response = retval;
      }

      audio = audio->next;
    }
  }else if(ags_regexec(&wildcard_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
    path_offset += 3;

    if(start_audio == NULL){
      osc_response = ags_osc_response_new();
      start_response = g_list_prepend(start_response,
				      osc_response);
      
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_OK);

      return(start_response);
    }
      
    while(audio != NULL){
      GList *retval;
	
      retval = ags_osc_meter_controller_monitor_meter_audio(osc_meter_controller,
							    osc_connection,
							    audio->data,
							    message, message_size,
							    type_tag,
							    path, path_offset);

      if(start_response != NULL){
	start_response = g_list_concat(start_response,
				       retval);
      }else{
	start_response = retval;
      }

      audio = audio->next;
    }
  }else if(path[path_offset] == '[' &&
	   path[path_offset + 1] == '"'){
    AgsAudio *current;

    gchar *audio_name;
    gchar *offset;

    guint length;

    if((offset = index(path + path_offset + 2, '"')) == NULL){
      osc_response = ags_osc_response_new();
      start_response = g_list_prepend(start_response,
				      osc_response);

      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		   NULL);
	  
      g_list_free_full(start_audio,
		       g_object_unref);
      
      return(start_response);
    }

    length = offset - (path + path_offset + 2);

    audio_name = (gchar *) malloc((length + 1) * sizeof(gchar));
    memcpy(audio_name, path + path_offset + 2, (length) * sizeof(gchar));
    audio_name[length] = '\0';

    current = NULL;
    audio = ags_audio_find_name(start_audio,
				audio_name);
	
    if(audio != NULL){
      current = audio->data;
    }	

    g_free(audio_name);
    
    path_offset += (length + 4);
    start_response = ags_osc_meter_controller_monitor_meter_audio(osc_meter_controller,
								  osc_connection,
								  current,
								  message, message_size,
								  type_tag,
								  path, path_offset);
  }else{
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);
      
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		 NULL);

    g_list_free_full(start_audio,
		     g_object_unref);

    return(start_response);
  }    

  g_list_free_full(start_audio,
		   g_object_unref);
  
  return(start_response);
}

void
ags_osc_meter_controller_expand_path_audio(AgsAudio *audio,
					   gchar *path,
					   gchar ***strv)
{
  AgsChannel *start_channel;
  AgsChannel *channel, *next_channel;
  
  regmatch_t match_arr[3];

  gchar *prefix;
  gchar *offset;
  gchar *next_path;
  
  guint path_offset;
  guint i, i_start, i_stop;
  gboolean is_output;

  static regex_t single_access_regex;
  static regex_t range_access_regex;
  static regex_t voluntary_access_regex;
  static regex_t more_access_regex;
  static regex_t wildcard_access_regex;

  static regex_t recall_regex;
  static regex_t generic_single_access_regex;
  static regex_t generic_range_access_regex;
  static regex_t generic_voluntary_access_regex;
  static regex_t generic_more_access_regex;
  static regex_t generic_wildcard_access_regex;
    
  static gboolean regex_compiled = FALSE;

  static const gchar *single_access_pattern = "^/AgsSoundProvider/AgsAudio\\[[0-9]+\\]/(AgsOutput|AgsInput)\\[([0-9]+)\\]";
  static const gchar *range_access_pattern = "^/AgsSoundProvider/AgsAudio\\[[0-9]+\\]/(AgsOutput|AgsInput)\\[([0-9]+)\\-([0-9]+)\\]";
  static const gchar *voluntary_access_pattern = "^/AgsSoundProvider/AgsAudio\\[[0-9]+\\]/(AgsOutput|AgsInput)\\[(\\?)\\]";
  static const gchar *more_access_pattern = "^/AgsSoundProvider/AgsAudio\\[[0-9]+\\]/(AgsOutput|AgsInput)\\[(\\+)\\]";
  static const gchar *wildcard_access_pattern = "^/AgsSoundProvider/AgsAudio\\[[0-9]+\\]/(AgsOutput|AgsInput)\\[(\\*)\\]";
  
  static const gchar *recall_pattern = "^/AgsSoundProvider/AgsAudio\\[[0-9]+\\]/([a-zA-Z]+)(\\/|\\[[0-9]+\\]|\\[[0-9]+\\-[0-9]+\\]|\\[\\?\\]|\\[\\+\\]|\\[\\*\\]|:)";
  static const gchar *generic_single_access_pattern = "^/AgsSoundProvider/AgsAudio\\[[0-9]+\\]/([A-Za-z]+)\\[([0-9]+)\\]";
  static const gchar *generic_range_access_pattern = "^/AgsSoundProvider/AgsAudio\\[[0-9]+\\]/([A-Za-z]+)\\[([0-9]+)\\-([0-9]+)\\]";
  static const gchar *generic_voluntary_access_pattern = "^/AgsSoundProvider/AgsAudio\\[[0-9]+\\]/([A-Za-z]+)\\[(\\?)\\]";
  static const gchar *generic_more_access_pattern = "^/AgsSoundProvider/AgsAudio\\[[0-9]+\\]/([A-Za-z]+)\\[(\\+)\\]";
  static const gchar *generic_wildcard_access_pattern = "^/AgsSoundProvider/AgsAudio\\[[0-9]+\\]/([A-Za-z]+)\\[(\\*)\\]";

  static const size_t max_matches = 3;
  static const size_t index_max_matches = 2;

  if(audio == NULL ||
     path == NULL){
    return;
  }
  
  pthread_mutex_lock(&regex_mutex);
  
  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&single_access_regex, single_access_pattern, REG_EXTENDED);
    ags_regcomp(&range_access_regex, range_access_pattern, REG_EXTENDED);
    ags_regcomp(&voluntary_access_regex, voluntary_access_pattern, REG_EXTENDED);
    ags_regcomp(&more_access_regex, more_access_pattern, REG_EXTENDED);
    ags_regcomp(&wildcard_access_regex, wildcard_access_pattern, REG_EXTENDED);

    ags_regcomp(&recall_regex, recall_pattern, REG_EXTENDED);

    ags_regcomp(&generic_single_access_regex, generic_single_access_pattern, REG_EXTENDED);
    ags_regcomp(&generic_range_access_regex, generic_range_access_pattern, REG_EXTENDED);
    ags_regcomp(&generic_voluntary_access_regex, generic_voluntary_access_pattern, REG_EXTENDED);
    ags_regcomp(&generic_more_access_regex, generic_more_access_pattern, REG_EXTENDED);
    ags_regcomp(&generic_wildcard_access_regex, generic_wildcard_access_pattern, REG_EXTENDED);
  }

  pthread_mutex_unlock(&regex_mutex);

  start_channel = NULL;
  
  prefix = NULL;
  path_offset = 0;
  
  if((offset = strstr(path, "/AgsOutput")) != NULL){
    g_object_get(audio,
		 "output", &start_channel,
		 NULL);
    
    path_offset = offset - path + 10;

    prefix = g_strndup(path,
		       path_offset);
    
    is_output = TRUE;
  }else if((offset = strstr(path, "/AgsInput")) != NULL){
    g_object_get(audio,
		 "input", &start_channel,
		 NULL);

    path_offset = offset - path + 9;

    prefix = g_strndup(path,
		       path_offset);
    
    is_output = FALSE;
  }
  
  if(ags_regexec(&single_access_regex, path, index_max_matches, match_arr, 0) == 0){
    gchar *endptr;
      
    guint i_stop;

    endptr = NULL;
    i_stop = g_ascii_strtoull(path + path_offset + 1,
			      &endptr,
			      10);

    channel = ags_channel_nth(start_channel,
			      i_stop);

    next_path = g_strdup_printf("%s[%d]%s",
				prefix,
				i_stop,
				endptr + 1);
    
    ags_osc_meter_controller_expand_path_channel(channel,
						 next_path,
						 strv);

    g_free(next_path);

    /* unref */
    g_object_unref(start_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
  }else if(ags_regexec(&range_access_regex, path, max_matches, match_arr, 0) == 0){
    gchar *endptr;
      
    guint i;
    guint i_start, i_stop;

    endptr = NULL;
    i_start = g_ascii_strtoull(path + path_offset + 1,
			       &endptr,
			       10);

    i_stop = g_ascii_strtoull(endptr + 1,
			      &endptr,
			      10);

    path_offset += ((endptr + 1) - (path + path_offset));

    channel = ags_channel_nth(start_channel,
			      i_start);

    next_channel = NULL;
    
    for(i = i_start; i <= i_stop && channel != NULL; i++){
      next_path = g_strdup_printf("%s[%d]%s",
				  prefix,
				  i,
				  endptr + 1);
    
      ags_osc_meter_controller_expand_path_channel(channel,
						   next_path,
						   strv);

      g_free(next_path);

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    /* unref */
    g_object_unref(start_channel);

    if(next_channel != NULL){
      g_object_unref(next_channel);
    }
  }else if(ags_regexec(&voluntary_access_regex, path, index_max_matches, match_arr, 0) == 0){
    path_offset += 3;

    if(start_channel != NULL){
      next_path = g_strdup_printf("%s[0]%s",
				  prefix,
				  path + path_offset + 3);
    
      ags_osc_meter_controller_expand_path_channel(start_channel,
						   next_path,
						   strv);
      
      g_free(next_path);      

      /* unref */
      g_object_unref(start_channel);
    }
  }else if(ags_regexec(&more_access_regex, path, index_max_matches, match_arr, 0) == 0){
    guint i;
    
    path_offset += 3;

    if(start_channel == NULL){
      return;
    }

    channel = start_channel;
    g_object_ref(channel);
    
    for(i = 0; channel != NULL; i++){
      next_path = g_strdup_printf("%s[%d]%s",
				  prefix,
				  i,
				  path + path_offset + 3);
    
      ags_osc_meter_controller_expand_path_channel(channel,
						   next_path,
						   strv);

      g_free(next_path);

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }    

    /* unref */
    g_object_unref(start_channel);

    if(next_channel != NULL){
      g_object_unref(next_channel);
    }
  }else if(ags_regexec(&wildcard_access_regex, path, index_max_matches, match_arr, 0) == 0){
    guint i;
    
    channel = start_channel;

    if(channel != NULL){
      g_object_ref(channel);
    }
    
    next_channel = NULL;
    
    for(i = 0; channel != NULL; i++){
      next_path = g_strdup_printf("%s[%d]%s",
				  prefix,
				  i,
				  path + path_offset + 3);
    
      ags_osc_meter_controller_expand_path_channel(channel,
						   next_path,
						   strv);

      g_free(next_path);

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }    

    /* unref */
    if(start_channel != NULL){
      g_object_unref(start_channel);
    }
    
    if(next_channel != NULL){
      g_object_unref(next_channel);
    }
  }else{
    AgsRecall *current;

    GType recall_type;
    
    GList *start_play, *play;
    GList *start_recall, *recall;

    gchar *type_name;    

    guint type_name_length;

    if(ags_regexec(&recall_regex, path, max_matches, match_arr, 0) == 0){
      type_name_length = match_arr[1].rm_eo - match_arr[1].rm_so;
      type_name = g_strndup(path + match_arr[1].rm_so,
			    type_name_length);
    }else{
      return;
    }

    recall_type = g_type_from_name(type_name);

    g_object_get(audio,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);
   
    if(ags_regexec(&generic_single_access_regex, path, index_max_matches, match_arr, 0) == 0){
      gchar *endptr;
      
      guint i, i_stop;

      offset = path + match_arr[1].rm_eo;
      path_offset = offset - path;
      
      prefix = g_strndup(path,
			 path_offset);

      endptr = NULL;
      i_stop = g_ascii_strtoull(path + path_offset + 1,
				&endptr,
				10);

      current = NULL;
      
      play = ags_recall_template_find_type(start_play,
					   recall_type);
    
      for(i = 0; i < i_stop && play != NULL; i++){
	play = play->next;
	play = ags_recall_template_find_type(play,
					     recall_type);
      }
      
      if(play != NULL){
	current = play->data;
      }else{
	recall = ags_recall_template_find_type(start_recall,
					       recall_type);
    
	for(i = 0; i < i_stop && recall != NULL; i++){
	  recall = recall->next;
	  recall = ags_recall_template_find_type(recall,
						 recall_type);
	}
      
	if(recall != NULL){
	  current = recall->data;
	}
      }
      
      next_path = g_strdup_printf("%s[%d]%s",
				  prefix,
				  i_stop,
				  endptr + 1);

      ags_osc_meter_controller_expand_path_recall(current,
						  next_path,
						  strv);
      
      g_free(next_path);
    }else if(ags_regexec(&generic_range_access_regex, path, max_matches, match_arr, 0) == 0){
      AgsRecall *current;
    
      gchar *endptr;
      
      guint i;
      guint i_start, i_stop;

      offset = path + match_arr[1].rm_eo;
      path_offset = offset - path;
      
      prefix = g_strndup(path,
			 path_offset);

      endptr = NULL;
      i_start = g_ascii_strtoull(path + path_offset + 1,
				 &endptr,
				 10);

      i_stop = g_ascii_strtoull(endptr + 1,
				&endptr,
				10);

      play = start_play;
    
      for(i = 0; i < i_start && play != NULL; i++){
	play = ags_recall_template_find_type(play,
					     recall_type);

	if(i + 1 < i_start){
	  play = play->next;
	}
      }
      
      recall = start_recall;
    
      for(i = 0; i < i_start && play != NULL; i++){
	recall = ags_recall_template_find_type(recall,
					       recall_type);

	if(i + 1 < i_start){
	  recall = recall->next;
	}
      }
      
      for(i = i_start; i <= i_stop && (play != NULL || recall != NULL); i++){
	next_path = g_strdup_printf("%s[%d]%s",
				    prefix,
				    i_stop,
				    endptr + 1);

	current = NULL;
	
	if(play != NULL){
	  current = play->data;
	}else if(recall != NULL){
	  current = recall->data;
	}
	
	ags_osc_meter_controller_expand_path_recall(current,
						    next_path,
						    strv);
      
	g_free(next_path);

	if(play != NULL){
	  play = play->next;

	  play = ags_recall_template_find_type(play,
					       recall_type);
	}

	if(recall != NULL){
	  recall = recall->next;

	  recall = ags_recall_template_find_type(recall,
						 recall_type);
	}
      }
    }else if(ags_regexec(&generic_voluntary_access_regex, path, index_max_matches, match_arr, 0) == 0){
      AgsRecall *current;

      offset = path + match_arr[1].rm_eo;
      path_offset = offset - path;
      
      prefix = g_strndup(path,
			 path_offset);

      current = NULL;

      play = ags_recall_template_find_type(start_play,
					   recall_type);
      
      if(play != NULL){
	current = play->data;
      }else{
	recall = ags_recall_template_find_type(start_recall,
					       recall_type);

	if(recall != NULL){
	  current = recall->data;
	}
      }

      if(current != NULL){
	i_stop = 0;
	next_path = g_strdup_printf("%s[%d]%s",
				    prefix,
				    i_stop,
				    path + path_offset + 3);

	ags_osc_meter_controller_expand_path_recall(current,
						    next_path,
						    strv);
      
	g_free(next_path);
      }
    }else if(ags_regexec(&generic_more_access_regex, path, index_max_matches, match_arr, 0) == 0){
      AgsRecall *current;

      offset = path + match_arr[1].rm_eo;
      path_offset = offset - path;
      
      prefix = g_strndup(path,
			 path_offset);

      if((play = ags_recall_template_find_type(start_play, recall_type)) == NULL &&
	 (recall = ags_recall_template_find_type(start_recall, recall_type)) == NULL){
	g_list_free_full(start_play,
			 g_object_unref);
	g_list_free_full(start_recall,
			 g_object_unref);

	return;
      }
      
      for(i = 0; play != NULL || recall != NULL; i++){
	next_path = g_strdup_printf("%s[%d]%s",
				    prefix,
				    i,
				    path + path_offset + 3);

	current = NULL;
	
	if(play != NULL){
	  current = play->data;
	}else if(recall != NULL){
	  current = recall->data;
	}
	
	ags_osc_meter_controller_expand_path_recall(current,
						    next_path,
						    strv);
      
	g_free(next_path);

	if(play != NULL){
	  play = play->next;

	  play = ags_recall_template_find_type(play,
					       recall_type);
	}

	if(recall != NULL){
	  recall = recall->next;

	  recall = ags_recall_template_find_type(recall,
						 recall_type);
	}
      }
    }else if(ags_regexec(&generic_wildcard_access_regex, path, index_max_matches, match_arr, 0) == 0){
      AgsRecall *current;

      offset = path + match_arr[1].rm_eo;
      path_offset = offset - path;
      
      prefix = g_strndup(path,
			 path_offset);

      play = ags_recall_template_find_type(start_play, recall_type);    
      recall = ags_recall_template_find_type(start_recall, recall_type);
      
      for(i = 0; play != NULL || recall != NULL; i++){
	next_path = g_strdup_printf("%s[%d]%s",
				    prefix,
				    i,
				    path + path_offset + 3);

	current = NULL;
	
	if(play != NULL){
	  current = play->data;
	}else if(recall != NULL){
	  current = recall->data;
	}
	
	ags_osc_meter_controller_expand_path_recall(current,
						    next_path,
						    strv);
      
	g_free(next_path);

	if(play != NULL){
	  play = play->next;

	  play = ags_recall_template_find_type(play,
					       recall_type);
	}

	if(recall != NULL){
	  recall = recall->next;

	  recall = ags_recall_template_find_type(recall,
						 recall_type);
	}
      }
    }
    
    g_list_free_full(start_play,
		     g_object_unref);
    g_list_free_full(start_recall,
		     g_object_unref);
  }

  g_free(prefix);
}

void
ags_osc_meter_controller_expand_path_channel(AgsChannel *channel,
					     gchar *path,
					     gchar ***strv)
{
  AgsRecall *current;

  GType recall_type;
  
  regmatch_t match_arr[4];

  GList *start_play, *play;
  GList *start_recall, *recall;

  gchar *type_name;    
  gchar *prefix;
  gchar *offset;
  gchar *next_path;

  guint type_name_length;
  guint path_offset;
  guint i, i_start, i_stop;

  static regex_t recall_regex;
  static regex_t single_access_regex;
  static regex_t range_access_regex;
  static regex_t voluntary_access_regex;
  static regex_t more_access_regex;
  static regex_t wildcard_access_regex;
    
  static gboolean regex_compiled = FALSE;

  static const gchar *recall_pattern = "^/AgsSoundProvider/AgsAudio\\[[0-9]+\\]/(AgsOutput|AgsInput)\\[[0-9]+\\]/([a-zA-Z]+)(\\/|\\[[0-9]+\\]|\\[[0-9]+\\-[0-9]+\\]|\\[\\?\\]|\\[\\+\\]|\\[\\*\\]|:)";
  static const gchar *single_access_pattern = "^/AgsSoundProvider/AgsAudio\\[[0-9]+\\]/(AgsOutput|AgsInput)\\[[0-9]+\\]/([A-Za-z]+)\\[([0-9]+)\\]";
  static const gchar *range_access_pattern = "^/AgsSoundProvider/AgsAudio\\[[0-9]+\\]/(AgsOutput|AgsInput)\\[[0-9]+\\]/([A-Za-z]+)\\[([0-9]+)\\-([0-9]+)\\]";
  static const gchar *voluntary_access_pattern = "^/AgsSoundProvider/AgsAudio\\[[0-9]+\\]/(AgsOutput|AgsInput)\\[[0-9]+\\]/([A-Za-z]+)\\[(\\?)\\]";
  static const gchar *more_access_pattern = "^/AgsSoundProvider/AgsAudio\\[[0-9]+\\]/(AgsOutput|AgsInput)\\[[0-9]+\\]/([A-Za-z]+)\\[(\\+)\\]";
  static const gchar *wildcard_access_pattern = "^/AgsSoundProvider/AgsAudio\\[[0-9]+\\]/(AgsOutput|AgsInput)\\[[0-9]+\\]/([A-Za-z]+)\\[(\\*)\\]";
  
  static const size_t max_matches = 4;
  static const size_t index_max_matches = 3;

  if(channel == NULL ||
     path == NULL){
    return;
  }
  
  pthread_mutex_lock(&regex_mutex);
  
  if(!regex_compiled){
    regex_compiled = TRUE;

    ags_regcomp(&recall_regex, recall_pattern, REG_EXTENDED);
      
    ags_regcomp(&single_access_regex, single_access_pattern, REG_EXTENDED);
    ags_regcomp(&range_access_regex, range_access_pattern, REG_EXTENDED);
    ags_regcomp(&voluntary_access_regex, voluntary_access_pattern, REG_EXTENDED);
    ags_regcomp(&more_access_regex, more_access_pattern, REG_EXTENDED);
    ags_regcomp(&wildcard_access_regex, wildcard_access_pattern, REG_EXTENDED);
  }

  pthread_mutex_unlock(&regex_mutex);

  if(ags_regexec(&recall_regex, path, max_matches, match_arr, 0) == 0){
    type_name_length = match_arr[2].rm_eo - match_arr[2].rm_so;
    type_name = g_strndup(path + match_arr[2].rm_so,
			  type_name_length);
  }else{
    return;
  }
  
  recall_type = g_type_from_name(type_name);

  prefix = NULL;
  path_offset = 0;
  
  g_object_get(channel,
	       "play", &start_play,
	       "recall", &start_recall,
	       NULL);
    
  if(ags_regexec(&single_access_regex, path, index_max_matches, match_arr, 0) == 0){
    gchar *endptr;
      
    guint i, i_stop;

    offset = path + match_arr[2].rm_eo;
    path_offset = offset - path;
      
    prefix = g_strndup(path,
		       path_offset);

    endptr = NULL;
    i_stop = g_ascii_strtoull(path + path_offset + 1,
			      &endptr,
			      10);

    current = NULL;

    play = ags_recall_template_find_type(start_play,
					 recall_type);
    
    for(i = 0; i < i_stop && play != NULL; i++){
      play = play->next;
      play = ags_recall_template_find_type(play,
					   recall_type);
    }
      
    if(play != NULL){
      current = play->data;
    }else{
      recall = ags_recall_template_find_type(start_recall,
					     recall_type);
    
      for(i = 0; i < i_stop && recall != NULL; i++){
	recall = recall->next;
	recall = ags_recall_template_find_type(recall,
					       recall_type);
      }
      
      if(recall != NULL){
	current = recall->data;
      }
    }
      
    next_path = g_strdup_printf("%s[%d]%s",
				prefix,
				i_stop,
				endptr + 1);
    
    ags_osc_meter_controller_expand_path_recall(current,
						next_path,
						strv);
      
    g_free(next_path);
  }else if(ags_regexec(&range_access_regex, path, max_matches, match_arr, 0) == 0){
    AgsRecall *current;
    
    gchar *endptr;
      
    guint i;
    guint i_start, i_stop;

    offset = path + match_arr[2].rm_eo;
    path_offset = offset - path;
      
    prefix = g_strndup(path,
		       path_offset);

    endptr = NULL;
    i_start = g_ascii_strtoull(path + path_offset + 1,
			       &endptr,
			       10);

    i_stop = g_ascii_strtoull(endptr + 1,
			      &endptr,
			      10);

    play = start_play;
    
    for(i = 0; i < i_start && play != NULL; i++){
      play = ags_recall_template_find_type(play,
					   recall_type);

      if(i + 1 < i_start){
	play = play->next;
      }
    }
      
    recall = start_recall;
    
    for(i = 0; i < i_start && play != NULL; i++){
      recall = ags_recall_template_find_type(recall,
					     recall_type);

      if(i + 1 < i_start){
	recall = recall->next;
      }
    }
    
    for(i = i_start; i <= i_stop && (play != NULL || recall != NULL); i++){
      next_path = g_strdup_printf("%s[%d]%s",
				  prefix,
				  i_stop,
				  endptr + 1);

      current = NULL;
	
      if(play != NULL){
	current = play->data;
      }else if(recall != NULL){
	current = recall->data;
      }
	
      ags_osc_meter_controller_expand_path_recall(current,
						  next_path,
						  strv);
      
      g_free(next_path);

      if(play != NULL){
	play = play->next;

	play = ags_recall_template_find_type(play,
					     recall_type);
      }

      if(recall != NULL){
	recall = recall->next;

	recall = ags_recall_template_find_type(recall,
					       recall_type);
      }
    }
  }else if(ags_regexec(&voluntary_access_regex, path, index_max_matches, match_arr, 0) == 0){
    AgsRecall *current;

    offset = path + match_arr[2].rm_eo;
    path_offset = offset - path;
      
    prefix = g_strndup(path,
		       path_offset);

    current = NULL;

    play = ags_recall_template_find_type(start_play,
					 recall_type);
      
    if(play != NULL){
      current = play->data;
    }else{
      recall = ags_recall_template_find_type(start_recall,
					     recall_type);

      if(recall != NULL){
	current = recall->data;
      }
    }

    if(current != NULL){
      next_path = g_strdup_printf("%s[%d]%s",
				  prefix,
				  i_stop,
				  path + path_offset + 3);

      ags_osc_meter_controller_expand_path_recall(current,
						  next_path,
						  strv);
      
      g_free(next_path);
    }
  }else if(ags_regexec(&more_access_regex, path, index_max_matches, match_arr, 0) == 0){
    AgsRecall *current;

    offset = path + match_arr[2].rm_eo;
    path_offset = offset - path;
      
    prefix = g_strndup(path,
		       path_offset);

    if((play = ags_recall_template_find_type(start_play, recall_type)) == NULL &&
       (recall = ags_recall_template_find_type(start_recall, recall_type)) == NULL){
      g_list_free_full(start_play,
		       g_object_unref);
      g_list_free_full(start_recall,
		       g_object_unref);
      
      return;
    }      
      
    while(play != NULL || recall != NULL){
      next_path = g_strdup_printf("%s[%d]%s",
				  prefix,
				  i_stop,
				  path + path_offset + 3);

      current = NULL;
	
      if(play != NULL){
	current = play->data;
      }else if(recall != NULL){
	current = recall->data;
      }
	
      ags_osc_meter_controller_expand_path_recall(current,
						  next_path,
						  strv);
      
      g_free(next_path);

      if(play != NULL){
	play = play->next;

	play = ags_recall_template_find_type(play,
					     recall_type);
      }

      if(recall != NULL){
	recall = recall->next;

	recall = ags_recall_template_find_type(recall,
					       recall_type);
      }
    }
  }else if(ags_regexec(&wildcard_access_regex, path, index_max_matches, match_arr, 0) == 0){
    AgsRecall *current;

    offset = path + match_arr[2].rm_eo;
    path_offset = offset - path;
      
    prefix = g_strndup(path,
		       path_offset);

    play = ags_recall_template_find_type(start_play, recall_type);    
    recall = ags_recall_template_find_type(start_recall, recall_type);
      
    while(play != NULL || recall != NULL){
      next_path = g_strdup_printf("%s[%d]%s",
				  prefix,
				  i_stop,
				  path + path_offset + 3);

      current = NULL;
	
      if(play != NULL){
	current = play->data;
      }else if(recall != NULL){
	current = recall->data;
      }
	
      ags_osc_meter_controller_expand_path_recall(current,
						  next_path,
						  strv);
      
      g_free(next_path);

      if(play != NULL){
	play = play->next;

	play = ags_recall_template_find_type(play,
					     recall_type);
      }

      if(recall != NULL){
	recall = recall->next;

	recall = ags_recall_template_find_type(recall,
					       recall_type);
      }
    }
  }

  g_list_free_full(start_play,
		   g_object_unref);
  g_list_free_full(start_recall,
		   g_object_unref);

  g_free(prefix);
}

void
ags_osc_meter_controller_expand_path_recall(AgsRecall *recall,
					    gchar *path,
					    gchar ***strv)
{
  regmatch_t match_arr[4];

  GList *start_port, *port;

  gchar *prefix;
  gchar *offset;
  gchar *next_path;
  
  guint path_offset;
  guint i, i_start, i_stop;

  static regex_t single_access_regex;
  static regex_t range_access_regex;
  static regex_t voluntary_access_regex;
  static regex_t more_access_regex;
  static regex_t wildcard_access_regex;
    
  static gboolean regex_compiled = FALSE;

  static const gchar *single_access_pattern = "^/AgsSoundProvider(/AgsAudio\\[[0-9]+\\]|/AgsAudio\\[[0-9]+\\]/(AgsOutput|AgsInput)\\[[0-9]+\\])/[A-Za-z]+\\[[0-9]+\\]/AgsPort\\[([0-9]+)\\]";
  static const gchar *range_access_pattern = "^/AgsSoundProvider/(/AgsAudio\\[[0-9]+\\]|/AgsAudio\\[[0-9]+\\]/(AgsOutput|AgsInput)\\[[0-9]+\\])/[A-Za-z]+\\[[0-9]+\\]/AgsPort\\[([0-9]+)\\-([0-9]+)\\]";
  static const gchar *voluntary_access_pattern = "^/AgsSoundProvider/(/AgsAudio\\[[0-9]+\\]|/AgsAudio\\[[0-9]+\\]/(AgsOutput|AgsInput)\\[[0-9]+\\])/[A-Za-z]+\\[[0-9]+\\]/AgsPort\\[(\\?)\\]";
  static const gchar *more_access_pattern = "^/AgsSoundProvider/(/AgsAudio\\[[0-9]+\\]|/AgsAudio\\[[0-9]+\\]/(AgsOutput|AgsInput)\\[[0-9]+\\])/[A-Za-z]+\\[[0-9]+\\]/AgsPort\\[(\\+)\\]";
  static const gchar *wildcard_access_pattern = "^/AgsSoundProvider/(/AgsAudio\\[[0-9]+\\]|/AgsAudio\\[[0-9]+\\]/(AgsOutput|AgsInput)\\[[0-9]+\\])/[A-Za-z]+\\[[0-9]+\\]/AgsPort\\[(\\*)\\]";

  static const size_t max_matches = 4;
  static const size_t index_max_matches = 3;
  
  if(recall == NULL ||
     path == NULL){
    return;
  }
  
  pthread_mutex_lock(&regex_mutex);
  
  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&single_access_regex, single_access_pattern, REG_EXTENDED);
    ags_regcomp(&range_access_regex, range_access_pattern, REG_EXTENDED);
    ags_regcomp(&voluntary_access_regex, voluntary_access_pattern, REG_EXTENDED);
    ags_regcomp(&more_access_regex, more_access_pattern, REG_EXTENDED);
    ags_regcomp(&wildcard_access_regex, wildcard_access_pattern, REG_EXTENDED);
  }

  pthread_mutex_unlock(&regex_mutex);

  prefix = NULL;

  if((offset = strstr(path, "/AgsPort")) != NULL){
    path_offset = offset - path + 8;

    prefix = g_strndup(path,
		       path_offset);
  }
  
  g_object_get(recall,
	       "port", &start_port,
	       NULL);

  if(ags_regexec(&single_access_regex, path, index_max_matches, match_arr, 0) == 0){
    AgsPort *current;
      
    gchar *endptr;
      
    guint i_stop;

    endptr = NULL;
    i_stop = g_ascii_strtoull(path + path_offset + 1,
			      &endptr,
			      10);
      
    current = g_list_nth_data(start_port,
			      i_stop);

    next_path = g_strdup_printf("%s[%d]%s",
				prefix,
				i_stop,
				endptr + 1);
    
    ags_osc_meter_controller_expand_path_port(current,
					      next_path,
					      strv);

    g_free(next_path);
  }else if(ags_regexec(&range_access_regex, path, max_matches, match_arr, 0) == 0){
    gchar *endptr;
      
    guint i;
    guint i_start, i_stop;

    endptr = NULL;
    i_start = g_ascii_strtoull(path + path_offset + 1,
			       &endptr,
			       10);

    i_stop = g_ascii_strtoull(endptr + 1,
			      &endptr,
			      10);

    port = g_list_nth(start_port,
		      i_start);
      
    for(i = i_start; i <= i_stop && port != NULL; i++){
      next_path = g_strdup_printf("%s[%d]%s",
				  prefix,
				  i,
				  endptr + 1);
    
      ags_osc_meter_controller_expand_path_port(port->data,
						next_path,
						strv);

      g_free(next_path);

      port = port->next;
    }
  }else if(ags_regexec(&voluntary_access_regex, path, index_max_matches, match_arr, 0) == 0){
    if(start_port != NULL){
      next_path = g_strdup_printf("%s[0]%s",
				  prefix,
				  path + path_offset + 3);
    
      ags_osc_meter_controller_expand_path_port(start_port->data,
						next_path,
						strv);
      
      g_free(next_path);      
    }
  }else if(ags_regexec(&more_access_regex, path, index_max_matches, match_arr, 0) == 0){
    guint i;
    
    if(start_port == NULL){
      return;
    }

    port = start_port;
      
    for(i = 0; port != NULL; i++){
      next_path = g_strdup_printf("%s[%d]%s",
				  prefix,
				  i,
				  path + path_offset + 3);
    
      ags_osc_meter_controller_expand_path_port(port->data,
						next_path,
						strv);

      g_free(next_path);

      port = port->next;
    }    
  }else if(ags_regexec(&wildcard_access_regex, path, index_max_matches, match_arr, 0) == 0){
    guint i;

    port = start_port;
      
    for(i = 0; port != NULL; i++){
      next_path = g_strdup_printf("%s[%d]%s",
				  prefix,
				  i,
				  path + path_offset + 3);
    
      ags_osc_meter_controller_expand_path_port(port->data,
						next_path,
						strv);

      g_free(next_path);

      port = port->next;
    }    
  }else{
    if(path[path_offset] == '[' &&
       path[path_offset + 1] == '"'){
      AgsPort *current;

      gchar *specifier;
      gchar *offset;

      guint length;

      if((offset = index(path + path_offset + 2, '"')) == NULL){
	g_list_free_full(start_port,
			 g_object_unref);
	
	return;
      }

      length = offset - (path + path_offset + 2);

      specifier = g_strndup(path + path_offset + 2,
			    length);
      
      current = NULL;
      port = ags_port_find_specifier(start_port,
				     specifier);
	
      if(port != NULL){
	current = port->data;
      }

      g_free(specifier);
      
      next_path = g_strdup_printf("%s[%d]%s",
				  prefix,
				  g_list_index(start_port, current),
				  path + path_offset + length + 4);

      ags_osc_meter_controller_expand_path_port(current,
						next_path,
						strv);
    }
  }

  g_list_free_full(start_port,
		   g_object_unref);
}

void
ags_osc_meter_controller_expand_path_port(AgsPort *port,
					  gchar *path,
					  gchar ***strv)
{
  guint length;

  if(port == NULL ||
     path == NULL ||
     strv == NULL){
    return;
  }
  
  if(strv[0] == NULL){
    length = 0;
    
    strv[0] = (gchar **) malloc(2 * sizeof(gchar *));
  }else{
    length = g_strv_length(strv[0]);

    strv[0] = (gchar **) realloc(strv[0],
				 (length + 2) * sizeof(gchar *));
  }	

  strv[0][length] = g_strdup(path);
  strv[0][length + 1] = NULL;
}

void
ags_osc_meter_controller_expand_path(gchar *path,
				     gchar ***strv)
{
  AgsApplicationContext *application_context;

  GList *start_audio, *audio;

  regmatch_t match_arr[2];

  gchar *next_path;

  guint path_offset;
  guint i, i_start, i_stop;

  static regex_t single_access_regex;
  static regex_t range_access_regex;
  static regex_t voluntary_access_regex;
  static regex_t more_access_regex;
  static regex_t wildcard_access_regex;
    
  static gboolean regex_compiled = FALSE;

  static const gchar *single_access_pattern = "^/AgsSoundProvider/AgsAudio\\[([0-9]+)\\]";
  static const gchar *range_access_pattern = "^/AgsSoundProvider/AgsAudio\\[([0-9]+)\\-([0-9]+)\\]";
  static const gchar *voluntary_access_pattern = "^/AgsSoundProvider/AgsAudio\\[(\\?)\\]";
  static const gchar *more_access_pattern = "^/AgsSoundProvider/AgsAudio\\[(\\+)\\]";
  static const gchar *wildcard_access_pattern = "^/AgsSoundProvider/AgsAudio\\[(\\*)\\]";
  
  static const size_t max_matches = 2;
  static const size_t index_max_matches = 1;

  if(path == NULL){
    return;
  }

  application_context = ags_application_context_get_instance();
  
  pthread_mutex_lock(&regex_mutex);
  
  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&single_access_regex, single_access_pattern, REG_EXTENDED);
    ags_regcomp(&range_access_regex, range_access_pattern, REG_EXTENDED);
    ags_regcomp(&voluntary_access_regex, voluntary_access_pattern, REG_EXTENDED);
    ags_regcomp(&more_access_regex, more_access_pattern, REG_EXTENDED);
    ags_regcomp(&wildcard_access_regex, wildcard_access_pattern, REG_EXTENDED);
  }

  pthread_mutex_unlock(&regex_mutex);

  audio = 
    start_audio = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

  path_offset = strlen("/AgsSoundProvider/AgsAudio");
  
  if(ags_regexec(&single_access_regex, path, index_max_matches, match_arr, 0) == 0){
    AgsAudio *current;
    
    gchar *endptr;
      
    guint i_stop;

    endptr = NULL;
    i_stop = g_ascii_strtoull(path + path_offset + 1,
			      &endptr,
			      10);

    current = g_list_nth_data(start_audio,
			      i_stop);

    next_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[%d]%s",
				i_stop,
				endptr + 1);
    
    ags_osc_meter_controller_expand_path_audio(current,
					       next_path,
					       strv);

    g_free(next_path);
  }else if(ags_regexec(&range_access_regex, path, max_matches, match_arr, 0) == 0){
    gchar *endptr;
      
    guint i;
    guint i_start, i_stop;

    endptr = NULL;
    i_start = g_ascii_strtoull(path + path_offset + 1,
			       &endptr,
			       10);

    i_stop = g_ascii_strtoull(endptr + 1,
			      &endptr,
			      10);

    path_offset += ((endptr + 1) - (path + path_offset));

    audio = g_list_nth(start_audio,
		       i_start);
      
    for(i = i_start; i <= i_stop && audio != NULL; i++){
      next_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[%d]%s",
				  i,
				  endptr + 1);

      ags_osc_meter_controller_expand_path_audio(audio->data,
						 next_path,
						 strv);

      g_free(next_path);
      
      audio = audio->next;
    }
  }else if(ags_regexec(&voluntary_access_regex, path, index_max_matches, match_arr, 0) == 0){
    path_offset += 3;

    if(start_audio != NULL){
      next_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[0]%s",
				  path + path_offset);
      
      ags_osc_meter_controller_expand_path_audio(audio->data,
						 next_path,
						 strv);
      
      g_free(next_path);      
    }
  }else if(ags_regexec(&more_access_regex, path, index_max_matches, match_arr, 0) == 0){
    guint i;
    
    path_offset += 3;

    if(start_audio == NULL){
      return;
    }

    audio = start_audio;
      
    for(i = 0; audio != NULL; i++){
      next_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[%d]%s",
				  i,
				  path + path_offset);

      ags_osc_meter_controller_expand_path_audio(audio->data,
						 next_path,
						 strv);

      g_free(next_path);
      
      audio = audio->next;
    }    
  }else if(ags_regexec(&wildcard_access_regex, path, index_max_matches, match_arr, 0) == 0){
    guint i;
    
    audio = start_audio;
      
    for(i = 0; audio != NULL; i++){
      next_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[%d]%s",
				  i,
				  path + path_offset);

      ags_osc_meter_controller_expand_path_audio(audio->data,
						 next_path,
						 strv);

      g_free(next_path);
      
      audio = audio->next;
    }    
  }else{
    path_offset = strlen("/AgsSoundProvider/AgsAudio");

    if(path[path_offset] == '[' &&
       path[path_offset + 1] == '"'){
      AgsAudio *current;

      gchar *audio_name;
      gchar *offset;

      guint length;

      if((offset = index(path + path_offset + 2, '"')) == NULL){
	g_list_free_full(start_audio,
			 g_object_unref);
	
	return;
      }

      length = offset - (path + path_offset + 2);

      audio_name = (gchar *) malloc((length + 1) * sizeof(gchar));
      memcpy(audio_name, path + path_offset + 2, (length) * sizeof(gchar));
      audio_name[length] = '\0';

      current = NULL;
      audio = ags_audio_find_name(start_audio,
				  audio_name);
	
      if(audio != NULL){
	current = audio->data;
      }

      g_free(audio_name);

      next_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[%d]%s",
				  g_list_index(start_audio, current),
				  offset + 2);

      ags_osc_meter_controller_expand_path_audio(current,
						 next_path,
						 strv);
    }
  }

  g_list_free_full(start_audio,
		   g_object_unref);
}

gpointer
ags_osc_meter_controller_monitor_meter_disable(AgsOscMeterController *osc_meter_controller,
					       AgsOscConnection *osc_connection,
					       unsigned char *message, guint message_size,
					       gchar *type_tag,
					       gchar *path)
{
  AgsOscResponse *osc_response;

  AgsOscMeterControllerMonitor *current;
    
  GList *monitor;
  GList *start_response;
  
  gchar **strv, **iter;

  guint i;
  
  pthread_mutex_t *osc_controller_mutex;

  start_response = NULL;
  
  osc_response = ags_osc_response_new();
  start_response = g_list_prepend(start_response,
				  osc_response);
    
  ags_osc_response_set_flags(osc_response,
			     AGS_OSC_RESPONSE_OK);

  /* get OSC meter controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_meter_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());

  /* find path and connection */
  strv = NULL;
  ags_osc_meter_controller_expand_path(path,
				       &strv);

  if(strv != NULL){
    iter = strv;
    
    for(; iter[0] != NULL; iter++){
      /* attempt 2 times play/recall */
      for(i = 0; i < 2; i++){
	pthread_mutex_lock(osc_controller_mutex);

	monitor = osc_meter_controller->monitor;
	current = NULL;

	while((monitor = ags_osc_meter_controller_monitor_find_path(monitor,
								    iter[0])) != NULL){
	  if(AGS_OSC_METER_CONTROLLER_MONITOR(monitor->data)->osc_connection == osc_connection){
	    current = monitor->data;

	    break;
	  }

	  monitor = monitor->next;
	}

	pthread_mutex_unlock(osc_controller_mutex);

	/* remove monitor */
	if(current != NULL){
	  ags_osc_meter_controller_remove_monitor(osc_meter_controller,
						  current);
	}
      }
    }

    g_strfreev(strv);
  }
  
  return(start_response);
}

gpointer
ags_osc_meter_controller_real_monitor_meter(AgsOscMeterController *osc_meter_controller,
					    AgsOscConnection *osc_connection,
					    unsigned char *message, guint message_size)
{
  AgsApplicationContext *application_context;

  AgsOscResponse *osc_response;

  GList *start_response;

  gchar *type_tag;
  gchar *path;

  gboolean success;

  start_response = NULL;
  
  /* read type tag */
  ags_osc_buffer_util_get_string(message + 8,
				 &type_tag, NULL);

  success = (type_tag != NULL &&
	     strlen(type_tag) == 3 &&
	     !strncmp(type_tag, ",s", 2) &&
	     (type_tag[2] == 'T' || type_tag[2] == 'F')) ? TRUE: FALSE;

  if(!success){
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);
      
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		 NULL);

    if(type_tag != NULL){
      free(type_tag);
    }
    
    return(start_response);
  }
  
  /* read argument */
  ags_osc_buffer_util_get_string(message + 12,
				 &path, NULL);

  if(path == NULL){
    osc_response = ags_osc_response_new();  
    start_response = g_list_prepend(start_response,
				    osc_response);
      
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		 NULL);

    free(type_tag);
    
    return(start_response);
  }

  /* get sound provider */
  if(type_tag[2] == 'T'){
    start_response = ags_osc_meter_controller_monitor_meter_enable(osc_meter_controller,
								   osc_connection,
								   message, message_size,
								   type_tag,
								   path);
  }else if(type_tag[2] == 'F'){
    start_response = ags_osc_meter_controller_monitor_meter_disable(osc_meter_controller,
								    osc_connection,
								    message, message_size,
								    type_tag,
								    path);
  }

  free(type_tag);
  free(path);
  
  return(start_response);
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
 * Returns: the #GList-struct containing #AgsOscResponse
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
