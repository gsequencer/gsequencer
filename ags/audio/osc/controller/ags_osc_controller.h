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

#ifndef __AGS_OSC_CONTROLLER_H__
#define __AGS_OSC_CONTROLLER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/osc/ags_osc_connection.h>

#define AGS_TYPE_OSC_CONTROLLER                (ags_osc_controller_get_type())
#define AGS_OSC_CONTROLLER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_CONTROLLER, AgsOscController))
#define AGS_OSC_CONTROLLER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_OSC_CONTROLLER, AgsOscControllerClass))
#define AGS_IS_OSC_CONTROLLER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_CONTROLLER))
#define AGS_IS_OSC_CONTROLLER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_CONTROLLER))
#define AGS_OSC_CONTROLLER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_OSC_CONTROLLER, AgsOscControllerClass))

#define AGS_OSC_CONTROLLER_GET_OBJ_MUTEX(obj) (((AgsOscController *) obj)->obj_mutex)

typedef struct _AgsOscController AgsOscController;
typedef struct _AgsOscControllerClass AgsOscControllerClass;

struct _AgsOscController
{
  GObject gobject;

  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  GObject *osc_server;

  gchar *context_path;
};

struct _AgsOscControllerClass
{
  GObjectClass gobject;
};

GType ags_osc_controller_get_type();

pthread_mutex_t* ags_osc_controller_get_class_mutex();

AgsOscController* ags_osc_controller_new();

#endif /*__AGS_OSC_CONTROLLER_H__*/
