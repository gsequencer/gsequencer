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

  osc_meter_controller->monitor_thread = (pthread_t *) malloc(sizeof(pthread_t));
  
  osc_meter_controller->monitor_port = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							     g_object_unref,
							     g_object_unref);
  g_hash_table_ref(osc_meter_controller->monitor_port);
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
  
  if(osc_meter_controller->monitor_port != NULL){
    g_hash_table_unref(osc_meter_controller->monitor_port);

    osc_meter_controller->monitor_port = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_meter_controller_parent_class)->dispose(gobject);
}

void
ags_osc_meter_controller_finalize(GObject *gobject)
{
  AgsOscMeterController *osc_meter_controller;

  osc_meter_controller = AGS_OSC_METER_CONTROLLER(gobject);
  
  if(osc_meter_controller->monitor_port != NULL){
    g_hash_table_unref(osc_meter_controller->monitor_port);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_meter_controller_parent_class)->finalize(gobject);
}

void*
ags_osc_meter_controller_monitor_thread(void *ptr)
{
  AgsOscMeterController *osc_meter_controller;

  osc_meter_controller = AGS_OSC_METER_CONTROLLER(ptr);


  
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

  /* get osc_meter_controller mutex */
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

  /* get osc_meter_controller mutex */
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

  /* get osc_meter_controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_meter_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(osc_controller_mutex);

  osc_meter_controller->flags &= (~flags);

  pthread_mutex_unlock(osc_controller_mutex);
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
