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

#include <ags/audio/osc/controller/ags_osc_front_controller.h>

#include <ags/libags.h>

#include <ags/i18n.h>

#include <stdlib.h>

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

  gobject->set_property = ags_osc_front_controller_set_property;
  gobject->get_property = ags_osc_front_controller_get_property;

  gobject->dispose = ags_osc_front_controller_dispose;
  gobject->finalize = ags_osc_front_controller_finalize;

  /* properties */

  /* AgsOscFrontControllerClass */
  osc_front_controller->do_request = ags_osc_front_controller_real_do_request;

  /* signals */
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
  g_object_set(osc_info_controller,
	       "context-path", "/",
	       NULL);
}

void
ags_osc_front_controller_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec)
{
  AgsOscFrontController *osc_front_controller;

  pthread_mutex_t *osc_controller_mutex;

  osc_front_controller = AGS_OSC_FRONT_CONTROLLER(gobject);

  /* get osc controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_front_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());
  
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

  pthread_mutex_t *osc_controller_mutex;

  osc_front_controller = AGS_OSC_FRONT_CONTROLLER(gobject);

  /* get osc controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_front_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());
  
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
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_front_controller_parent_class)->dispose(gobject);
}

void
ags_osc_front_controller_finalize(GObject *gobject)
{
  AgsOscFrontController *osc_front_controller;

  osc_front_controller = AGS_OSC_FRONT_CONTROLLER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_front_controller_parent_class)->finalize(gobject);
}

gpointer
ags_osc_front_controller_real_do_request(AgsOscFrontController *osc_front_controller,
					 AgsOscConnection *osc_connection,
					 unsigned char *packet, guint packet_size)
{
  //TODO:JK: implement me
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
 * Returns: the #AgsOscResponse
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
  g_signal_emit(G_OBJECT(osc_front_controller),
		osc_front_controller_signals[DO_REQUEST], 0,
		osc_connection,
		packet, packet_size,
		&osc_response);
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
