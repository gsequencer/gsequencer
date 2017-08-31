/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_LOCAL_TASK_CONTROLLER_H__
#define __AGS_LOCAL_TASK_CONTROLLER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/server/controller/ags_controller.h>

#include <ags/thread/ags_task.h>
#include <ags/thread/ags_timestamp.h>

#define AGS_TYPE_LOCAL_TASK_CONTROLLER                (ags_local_task_controller_get_type())
#define AGS_LOCAL_TASK_CONTROLLER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LOCAL_TASK_CONTROLLER, AgsLocalTaskController))
#define AGS_LOCAL_TASK_CONTROLLER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_LOCAL_TASK_CONTROLLER, AgsLocalTaskControllerClass))
#define AGS_IS_LOCAL_TASK_CONTROLLER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LOCAL_TASK_CONTROLLER))
#define AGS_IS_LOCAL_TASK_CONTROLLER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LOCAL_TASK_CONTROLLER))
#define AGS_LOCAL_TASK_CONTROLLER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_LOCAL_TASK_CONTROLLER, AgsLocalTaskControllerClass))

#define AGS_LOCAL_TASK_CONTROLLER_CONTEXT_PATH "/task"

typedef struct _AgsLocalTaskController AgsLocalTaskController;
typedef struct _AgsLocalTaskControllerClass AgsLocalTaskControllerClass;

struct _AgsLocalTaskController
{
  AgsController controller;
};

struct _AgsLocalTaskControllerClass
{
  AgsControllerClass controller;
  
  gpointer (*launch)(AgsLocalTaskController *local_task_controller,
		     AgsTask *task);
  gpointer (*launch_timed)(AgsLocalTaskController *local_task_controller,
			   AgsTask *task, AgsTimestamp *timestamp);
};

GType ags_local_task_controller_get_type();

gpointer ags_local_task_controller_launch(AgsLocalTaskController *local_task_controller,
					  AgsTask *task);
gpointer ags_local_task_controller_launch_timed(AgsLocalTaskController *local_task_controller,
						AgsTask *task, AgsTimestamp *timestamp);

AgsLocalTaskController* ags_local_task_controller_new();

#endif /*__AGS_LOCAL_TASK_CONTROLLER_H__*/
