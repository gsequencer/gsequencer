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

#include <ags/audio/osc/controller/ags_osc_info_controller.h>

#include <ags/audio/osc/ags_osc_response.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_osc_info_controller_class_init(AgsOscInfoControllerClass *osc_info_controller);
void ags_osc_info_controller_init(AgsOscInfoController *osc_info_controller);
void ags_osc_info_controller_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_osc_info_controller_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_osc_info_controller_dispose(GObject *gobject);
void ags_osc_info_controller_finalize(GObject *gobject);

gpointer ags_osc_info_controller_real_get_info(AgsOscInfoController *osc_info_controller,
					       AgsOscConnection *osc_connection,
					       unsigned char *message, guint message_size);

/**
 * SECTION:ags_osc_info_controller
 * @short_description: OSC info controller
 * @title: AgsOscInfoController
 * @section_id:
 * @include: ags/audio/osc/controller/ags_osc_info_controller.h
 *
 * The #AgsOscInfoController implements the OSC info controller.
 */

enum{
  PROP_0,
};

enum{
  GET_INFO,
  LAST_SIGNAL,
};

static gpointer ags_osc_info_controller_parent_class = NULL;
static guint osc_info_controller_signals[LAST_SIGNAL];

GType
ags_osc_info_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_info_controller = 0;

    static const GTypeInfo ags_osc_info_controller_info = {
      sizeof (AgsOscInfoControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_info_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscInfoController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_info_controller_init,
    };
    
    ags_type_osc_info_controller = g_type_register_static(AGS_TYPE_OSC_CONTROLLER,
							  "AgsOscInfoController",
							  &ags_osc_info_controller_info,
							  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_info_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_info_controller_class_init(AgsOscInfoControllerClass *osc_info_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_osc_info_controller_parent_class = g_type_class_peek_parent(osc_info_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_info_controller;

  gobject->set_property = ags_osc_info_controller_set_property;
  gobject->get_property = ags_osc_info_controller_get_property;

  gobject->dispose = ags_osc_info_controller_dispose;
  gobject->finalize = ags_osc_info_controller_finalize;

  /* properties */

  /* AgsOscInfoControllerClass */
  osc_info_controller->get_info = ags_osc_info_controller_real_get_info;

  /* signals */
  /**
   * AgsOscInfoController::get-info:
   * @osc_info_controller: the #AgsOscInfoController
   * @osc_connection: the #AgsOscConnection
   * @message: the message received
   * @message_size: the message size
   *
   * The ::get-info signal is emited during get info of info controller.
   *
   * Since: 2.1.0
   */
  osc_info_controller_signals[GET_INFO] =
    g_signal_new("get-info",
		 G_TYPE_FROM_CLASS(osc_info_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscInfoControllerClass, get_info),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__OBJECT_POINTER_UINT,
		 G_TYPE_NONE, 0);
}

void
ags_osc_info_controller_init(AgsOscInfoController *osc_info_controller)
{
  //TODO:JK: implement me
}

void
ags_osc_info_controller_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec)
{
  AgsOscInfoController *osc_info_controller;

  pthread_mutex_t *osc_controller_mutex;

  osc_info_controller = AGS_OSC_INFO_CONTROLLER(gobject);

  /* get osc controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_info_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_info_controller_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec)
{
  AgsOscInfoController *osc_info_controller;

  pthread_mutex_t *osc_controller_mutex;

  osc_info_controller = AGS_OSC_INFO_CONTROLLER(gobject);

  /* get osc controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_info_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_info_controller_dispose(GObject *gobject)
{
  AgsOscInfoController *osc_info_controller;

  osc_info_controller = AGS_OSC_INFO_CONTROLLER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_info_controller_parent_class)->dispose(gobject);
}

void
ags_osc_info_controller_finalize(GObject *gobject)
{
  AgsOscInfoController *osc_info_controller;

  osc_info_controller = AGS_OSC_INFO_CONTROLLER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_info_controller_parent_class)->finalize(gobject);
}

gpointer
ags_osc_info_controller_real_get_info(AgsOscInfoController *info_controller,
				      AgsOscConnection *osc_connection,
				      unsigned char *message, guint message_size)
{
  AgsOscResponse *osc_response;

  unsigned char *packet;

  guint packet_size;
  
  static const unsigned char server_info_message[] = "/info\0\0\0,ssss\0\0\0V2.1.0\0\0osc-server\0\0Advanced Gtk+ Sequencer\02.1.0\0\0\0";

  osc_response = ags_osc_response_new();

  /* create packet */
  packet = (unsigned char *) malloc((4 * sizeof(unsigned char)) + sizeof(server_info_message));

  ags_osc_buffer_util_put_int32(packet,
				sizeof(server_info_message));
  memcpy(packet + 4, server_info_message, sizeof(server_info_message));

  /* set response packet */
  g_object_set(osc_response,
	       "packet", packet,
	       "packet-size", (4 * sizeof(unsigned char)) + sizeof(server_info_message),
	       NULL);

  return(osc_response);
}

/**
 * ags_osc_info_controller_get_info:
 * @osc_info_controller: the #AgsOscInfoController
 * @osc_connection: the #AgsOscConnection
 * @message: the message received
 * @message_size: the message size
 * 
 * Get info.
 * 
 * Returns: the #AgsOscResponse
 * 
 * Since: 2.1.0
 */
gpointer
ags_osc_info_controller_get_info(AgsOscInfoController *info_controller,
				 AgsOscConnection *osc_connection,
				 unsigned char *message, guint message_size)
{
  gpointer osc_response;
  
  g_return_val_if_fail(AGS_IS_OSC_INFO_CONTROLLER(osc_info_controller), NULL);
  
  g_object_ref((GObject *) osc_info_controller);
  g_signal_emit(G_OBJECT(osc_info_controller),
		osc_info_controller_signals[GET_INFO], 0,
		osc_connection,
		message, message_size,
		&osc_response);
  g_object_unref((GObject *) osc_info_controller);

  return(osc_response);
}

/**
 * ags_osc_info_controller_new:
 * 
 * Instantiate new #AgsOscInfoController
 * 
 * Returns: the #AgsOscInfoController
 * 
 * Since: 2.1.0
 */
AgsOscInfoController*
ags_osc_info_controller_new()
{
  AgsOscInfoController *osc_info_controller;

  osc_info_controller = (AgsOscInfoController *) g_object_new(AGS_TYPE_OSC_INFO_CONTROLLER,
							      NULL);

  return(osc_info_controller);
}
