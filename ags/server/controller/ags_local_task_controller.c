/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/server/controller/ags_local_task_controller.h>

#include <ags/object/ags_marshal.h>

#include <ags/server/security/ags_authentication_manager.h>

void ags_local_task_controller_class_init(AgsLocalTaskControllerClass *local_task_controller);
void ags_local_task_controller_init(AgsLocalTaskController *local_task_controller);
void ags_local_task_controller_finalize(GObject *gobject);

/**
 * SECTION:ags_local_task_controller
 * @short_description: local task controller
 * @title: AgsLocalTaskController
 * @section_id:
 * @include: ags/server/controller/ags_local_task_controller.h
 *
 * The #AgsLocalTaskController is a controller.
 */

enum{
  CREATE_INSTANCE,
  LAST_SIGNAL,
};

static gpointer ags_local_task_controller_parent_class = NULL;
static guint local_task_controller_signals[LAST_SIGNAL];

GType
ags_local_task_controller_get_type()
{
  static GType ags_type_local_task_controller = 0;

  if(!ags_type_local_task_controller){
    static const GTypeInfo ags_local_task_controller_info = {
      sizeof (AgsLocalTaskControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_local_task_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLocalTaskController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_local_task_controller_init,
    };
    
    ags_type_local_task_controller = g_type_register_static(G_TYPE_OBJECT,
							    "AgsLocalTaskController\0",
							    &ags_local_task_controller_info,
							    0);
  }

  return (ags_type_local_task_controller);
}

void
ags_local_task_controller_class_init(AgsLocalTaskControllerClass *local_task_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_local_task_controller_parent_class = g_type_class_peek_parent(local_task_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) local_task_controller;

  gobject->finalize = ags_local_task_controller_finalize;

  /* AgsLocalTaskController */
  local_task_controller->create_instance = ags_local_task_controller_real_create_instance;

  /* signals */
  /**
   * AgsLocalTaskController::launch:
   * @local_task_controller: the #AgsLocalTaskController
   *
   * The ::launch signal is used to launch a task.
   *
   * Returns: the response
   * 
   * Since: 1.0.0
   */
  local_task_controller_signals[LAUNCH] =
    g_signal_new("launch\0",
		 G_TYPE_FROM_CLASS(local_task_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLocalTaskControllerClass, launch),
		 NULL, NULL,
		 g_cclosure_marshal_POINTER__OBJECT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsLocalTaskController::launch:
   * @local_task_controller: the #AgsLocalTaskController
   * @timestamp: the #AgsTimestamp specify the start time
   *
   * The ::launch-timed signal is used to launch a task delayed.
   *
   * Returns: the response
   * 
   * Since: 1.0.0
   */
  local_task_controller_signals[LAUNCH_TIMED] =
    g_signal_new("launch-timed\0",
		 G_TYPE_FROM_CLASS(local_task_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLocalTaskControllerClass, launch_timed),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__OBJECT_OBJECT,
		 G_TYPE_POINTER, 2,
		 G_TYPE_OBJECT,
		 G_TYPE_OBJECT);
}

void
ags_local_task_controller_init(AgsLocalTaskController *local_task_controller)
{
  //TODO:JK: implement me
}

void
ags_local_task_controller_finalize(GObject *gobject)
{
  AgsLocalTaskController *local_task_controller;

  local_task_controller = AGS_LOCAL_TASK_CONTROLLER(gobject);

  G_OBJECT_CLASS(ags_local_task_controller_parent_class)->finalize(gobject);
}

AgsLocalTaskController*
ags_local_task_controller_new()
{
  AgsLocalTaskController *local_task_controller;

  local_task_controller = (AgsLocalTaskController *) g_object_new(AGS_TYPE_LOCAL_TASK_CONTROLLER,
								  NULL);

  return(local_task_controller);
}
