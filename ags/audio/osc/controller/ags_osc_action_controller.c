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

#include <ags/audio/osc/controller/ags_osc_action_controller.h>

#include <ags/libags.h>

#include <ags/i18n.h>

#include <stdlib.h>

void ags_osc_action_controller_class_init(AgsOscActionControllerClass *osc_action_controller);
void ags_osc_action_controller_init(AgsOscActionController *osc_action_controller);
void ags_osc_action_controller_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec);
void ags_osc_action_controller_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec);
void ags_osc_action_controller_dispose(GObject *gobject);
void ags_osc_action_controller_finalize(GObject *gobject);

gpointer ags_osc_action_controller_real_run_action(AgsOscActionController *osc_action_controller,
						   AgsOscConnection *osc_connection,
						   unsigned char *message, guint message_size);

/**
 * SECTION:ags_osc_action_controller
 * @short_description: OSC action controller
 * @title: AgsOscActionController
 * @section_id:
 * @include: ags/audio/osc/controller/ags_osc_action_controller.h
 *
 * The #AgsOscActionController implements the OSC action controller.
 */

enum{
  PROP_0,
};

enum{
  RUN_ACTION,
  LAST_SIGNAL,
};

static gpointer ags_osc_action_controller_parent_class = NULL;
static guint osc_action_controller_signals[LAST_SIGNAL];

GType
ags_osc_action_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_action_controller = 0;

    static const GTypeInfo ags_osc_action_controller_info = {
      sizeof (AgsOscActionControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_action_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscActionController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_action_controller_init,
    };
    
    ags_type_osc_action_controller = g_type_register_static(AGS_TYPE_OSC_CONTROLLER,
							    "AgsOscActionController",
							    &ags_osc_action_controller_info,
							    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_action_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_action_controller_class_init(AgsOscActionControllerClass *osc_action_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_osc_action_controller_parent_class = g_type_class_peek_parent(osc_action_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_action_controller;

  gobject->set_property = ags_osc_action_controller_set_property;
  gobject->get_property = ags_osc_action_controller_get_property;

  gobject->dispose = ags_osc_action_controller_dispose;
  gobject->finalize = ags_osc_action_controller_finalize;

  /* properties */

  /* AgsOscActionControllerClass */
  osc_action_controller->run_action = ags_osc_action_controller_real_run_action;

  /* signals */
  /**
   * AgsOscActionController::run-action:
   * @osc_action_controller: the #AgsOscActionController
   * @osc_connection: the #AgsOscConnection
   * @message: the message received
   * @message_size: the message size
   *
   * The ::run-action signal is emited during get data of action controller.
   *
   * Returns: the #AgsOscResponse
   * 
   * Since: 2.1.0
   */
  osc_action_controller_signals[RUN_ACTION] =
    g_signal_new("run-action",
		 G_TYPE_FROM_CLASS(osc_action_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscActionControllerClass, run_action),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__OBJECT_POINTER_UINT,
		 G_TYPE_POINTER, 3,
		 G_TYPE_OBJECT,
		 G_TYPE_POINTER,
		 G_TYPE_UINT);
}

void
ags_osc_action_controller_init(AgsOscActionController *osc_action_controller)
{
  g_object_set(osc_action_controller,
	       "context-path", "/action",
	       NULL);
}

void
ags_osc_action_controller_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec)
{
  AgsOscActionController *osc_action_controller;

  pthread_mutex_t *osc_controller_mutex;

  osc_action_controller = AGS_OSC_ACTION_CONTROLLER(gobject);

  /* get osc controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_action_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_action_controller_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec)
{
  AgsOscActionController *osc_action_controller;

  pthread_mutex_t *osc_controller_mutex;

  osc_action_controller = AGS_OSC_ACTION_CONTROLLER(gobject);

  /* get osc controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_action_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_action_controller_dispose(GObject *gobject)
{
  AgsOscActionController *osc_action_controller;

  osc_action_controller = AGS_OSC_ACTION_CONTROLLER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_action_controller_parent_class)->dispose(gobject);
}

void
ags_osc_action_controller_finalize(GObject *gobject)
{
  AgsOscActionController *osc_action_controller;

  osc_action_controller = AGS_OSC_ACTION_CONTROLLER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_action_controller_parent_class)->finalize(gobject);
}

gpointer
ags_osc_action_controller_real_run_action(AgsOscActionController *osc_action_controller,
					  AgsOscConnection *osc_connection,
					  unsigned char *message, guint message_size)
{
  //TODO:JK: implement me
}

/**
 * ags_osc_action_controller_run_action:
 * @osc_action_controller: the #AgsOscActionController
 * @osc_connection: the #AgsOscConnection
 * @message: the message received
 * @message_size: the message size
 * 
 * Get action.
 * 
 * Returns: the #AgsOscResponse
 * 
 * Since: 2.1.0
 */
gpointer
ags_osc_action_controller_run_action(AgsOscActionController *osc_action_controller,
				     AgsOscConnection *osc_connection,
				     unsigned char *message, guint message_size)
{
  gpointer osc_response;
  
  g_return_val_if_fail(AGS_IS_OSC_ACTION_CONTROLLER(osc_action_controller), NULL);
  
  g_object_ref((GObject *) osc_action_controller);
  g_signal_emit(G_OBJECT(osc_action_controller),
		osc_action_controller_signals[RUN_ACTION], 0,
		osc_connection,
		message, message_size,
		&osc_response);
  g_object_unref((GObject *) osc_action_controller);

  return(osc_response);
}

/**
 * ags_osc_action_controller_new:
 * 
 * Instantiate new #AgsOscActionController
 * 
 * Returns: the #AgsOscActionController
 * 
 * Since: 2.1.0
 */
AgsOscActionController*
ags_osc_action_controller_new()
{
  AgsOscActionController *osc_action_controller;

  osc_action_controller = (AgsOscActionController *) g_object_new(AGS_TYPE_OSC_ACTION_CONTROLLER,
								  NULL);

  return(osc_action_controller);
}
