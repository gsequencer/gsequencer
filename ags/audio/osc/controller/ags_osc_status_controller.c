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

#include <ags/audio/osc/controller/ags_osc_status_controller.h>

#include <ags/libags.h>

#include <ags/audio/osc/ags_osc_response.h>
#include <ags/audio/osc/ags_osc_server.h>
#include <ags/audio/osc/ags_osc_buffer_util.h>

#include <ags/i18n.h>

#include <stdlib.h>
#include <math.h>

void ags_osc_status_controller_class_init(AgsOscStatusControllerClass *osc_status_controller);
void ags_osc_status_controller_init(AgsOscStatusController *osc_status_controller);
void ags_osc_status_controller_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec);
void ags_osc_status_controller_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec);
void ags_osc_status_controller_dispose(GObject *gobject);
void ags_osc_status_controller_finalize(GObject *gobject);

gpointer ags_osc_status_controller_real_get_status(AgsOscStatusController *status_controller,
						   AgsOscConnection *osc_connection,
						   guchar *message, guint message_size);

/**
 * SECTION:ags_osc_status_controller
 * @short_description: OSC status controller
 * @title: AgsOscStatusController
 * @section_id:
 * @include: ags/audio/osc/controller/ags_osc_status_controller.h
 *
 * The #AgsOscStatusController implements the OSC status controller.
 */

enum{
  PROP_0,
};

enum{
  GET_STATUS,
  LAST_SIGNAL,
};

static gpointer ags_osc_status_controller_parent_class = NULL;
static guint osc_status_controller_signals[LAST_SIGNAL];

GType
ags_osc_status_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_status_controller = 0;

    static const GTypeInfo ags_osc_status_controller_info = {
      sizeof (AgsOscStatusControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_status_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscStatusController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_status_controller_init,
    };
    
    ags_type_osc_status_controller = g_type_register_static(AGS_TYPE_OSC_CONTROLLER,
							    "AgsOscStatusController",
							    &ags_osc_status_controller_info,
							    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_status_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_status_controller_class_init(AgsOscStatusControllerClass *osc_status_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_osc_status_controller_parent_class = g_type_class_peek_parent(osc_status_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_status_controller;

  gobject->set_property = ags_osc_status_controller_set_property;
  gobject->get_property = ags_osc_status_controller_get_property;

  gobject->dispose = ags_osc_status_controller_dispose;
  gobject->finalize = ags_osc_status_controller_finalize;

  /* properties */

  /* AgsOscStatusControllerClass */
  osc_status_controller->get_status = ags_osc_status_controller_real_get_status;

  /* signals */
  /**
   * AgsOscStatusController::get-status:
   * @osc_status_controller: the #AgsOscStatusController
   * @osc_connection: the #AgsOscConnection
   * @message: the message received
   * @message_size: the message size
   *
   * The ::get-status signal is emited during get status of status controller.
   *
   * Returns: the #AgsOscResponse
   * 
   * Since: 2.1.0
   */
  osc_status_controller_signals[GET_STATUS] =
    g_signal_new("get-status",
		 G_TYPE_FROM_CLASS(osc_status_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscStatusControllerClass, get_status),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__OBJECT_POINTER_UINT,
		 G_TYPE_POINTER, 3,
		 G_TYPE_OBJECT,
		 G_TYPE_POINTER,
		 G_TYPE_UINT);
}

void
ags_osc_status_controller_init(AgsOscStatusController *osc_status_controller)
{
  g_object_set(osc_status_controller,
	       "context-path", "/status",
	       NULL);
}

void
ags_osc_status_controller_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec)
{
  AgsOscStatusController *osc_status_controller;

  pthread_mutex_t *osc_controller_mutex;

  osc_status_controller = AGS_OSC_STATUS_CONTROLLER(gobject);

  /* get osc controller mutex */
  osc_controller_mutex = AGS_OSC_CONTROLLER_GET_OBJ_MUTEX(osc_status_controller);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_status_controller_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec)
{
  AgsOscStatusController *osc_status_controller;

  pthread_mutex_t *osc_controller_mutex;

  osc_status_controller = AGS_OSC_STATUS_CONTROLLER(gobject);

  /* get osc controller mutex */
  osc_controller_mutex = AGS_OSC_CONTROLLER_GET_OBJ_MUTEX(osc_status_controller);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_status_controller_dispose(GObject *gobject)
{
  AgsOscStatusController *osc_status_controller;

  osc_status_controller = AGS_OSC_STATUS_CONTROLLER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_status_controller_parent_class)->dispose(gobject);
}

void
ags_osc_status_controller_finalize(GObject *gobject)
{
  AgsOscStatusController *osc_status_controller;

  osc_status_controller = AGS_OSC_STATUS_CONTROLLER(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_osc_status_controller_parent_class)->finalize(gobject);
}

gpointer
ags_osc_status_controller_real_get_status(AgsOscStatusController *osc_status_controller,
					  AgsOscConnection *osc_connection,
					  unsigned char *message, guint message_size)
{
  AgsOscServer *osc_server;
  AgsOscResponse *osc_response;
  
  GList *start_response;

  gchar *type_tag;
  gchar *status;
  unsigned char *packet;
  
  guint real_packet_size;
  guint packet_size;
  guint length;
  gboolean success;

  start_response = NULL;

  osc_response = ags_osc_response_new();
  start_response = g_list_prepend(start_response,
				  osc_response);
      
  /* read type tag */
  ags_osc_buffer_util_get_string(message + 8,
				 &type_tag, NULL);

  success = (type_tag != NULL &&
	     !strncmp(type_tag, ",", 2)) ? TRUE: FALSE;

  if(!success){
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

  g_object_get(osc_status_controller,
	       "osc-server", &osc_server,
	       NULL);

  real_packet_size = 0;
  packet_size = 0;

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
				 "/status", -1);
      
  packet_size += 8;

  ags_osc_buffer_util_put_string(packet + packet_size,
				 ",s", -1);
  
  /* status argument */
  packet_size += 4;

  if(ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_RUNNING)){
    status = "server up and running";
  }else if(ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_STARTED)){
    status = "server started";
  }else if(ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_TERMINATING)){
    status = "server terminating";
  }else{
    status = "server stopped";
  }
  
  length = strlen(status);
      
  ags_osc_buffer_util_put_string(packet + packet_size,
				 status, -1);

  packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

  /* packet size */
  ags_osc_buffer_util_put_int32(packet,
				packet_size);
  
  free(type_tag);
  
  return(start_response);
}

/**
 * ags_osc_status_controller_get_status:
 * @osc_status_controller: the #AgsOscStatusController
 * @osc_connection: the #AgsOscConnection
 * @message: the message received
 * @message_size: the message size
 * 
 * Get status.
 * 
 * Returns: the #GList-struct containing #AgsOscResponse
 * 
 * Since: 2.1.0
 */
gpointer
ags_osc_status_controller_get_status(AgsOscStatusController *osc_status_controller,
				     AgsOscConnection *osc_connection,
				     unsigned char *message, guint message_size)
{
  gpointer osc_response;
  
  g_return_val_if_fail(AGS_IS_OSC_STATUS_CONTROLLER(osc_status_controller), NULL);
  
  g_object_ref((GObject *) osc_status_controller);
  g_signal_emit(G_OBJECT(osc_status_controller),
		osc_status_controller_signals[GET_STATUS], 0,
		osc_connection,
		message, message_size,
		&osc_response);
  g_object_unref((GObject *) osc_status_controller);

  return(osc_response);
}

/**
 * ags_osc_status_controller_new:
 * 
 * Instantiate new #AgsOscStatusController
 * 
 * Returns: the #AgsOscStatusController
 * 
 * Since: 2.1.0
 */
AgsOscStatusController*
ags_osc_status_controller_new()
{
  AgsOscStatusController *osc_status_controller;

  osc_status_controller = (AgsOscStatusController *) g_object_new(AGS_TYPE_OSC_STATUS_CONTROLLER,
								  NULL);

  return(osc_status_controller);
}
