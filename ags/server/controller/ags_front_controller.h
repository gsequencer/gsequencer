/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_FRONT_CONTROLLER_H__
#define __AGS_FRONT_CONTROLLER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/server/controller/ags_controller.h>

#define AGS_TYPE_FRONT_CONTROLLER                (ags_front_controller_get_type())
#define AGS_FRONT_CONTROLLER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FRONT_CONTROLLER, AgsFrontController))
#define AGS_FRONT_CONTROLLER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_FRONT_CONTROLLER, AgsFrontControllerClass))
#define AGS_IS_FRONT_CONTROLLER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FRONT_CONTROLLER))
#define AGS_IS_FRONT_CONTROLLER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FRONT_CONTROLLER))
#define AGS_FRONT_CONTROLLER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_FRONT_CONTROLLER, AgsFrontControllerClass))

typedef struct _AgsFrontController AgsFrontController;
typedef struct _AgsFrontControllerClass AgsFrontControllerClass;

struct _AgsFrontController
{
  AgsController controller;
};

struct _AgsFrontControllerClass
{
  AgsControllerClass controller;
  
  gpointer (*authenticate)(AgsFrontController *front_controller,
			   gchar *login,
			   gchar *password,
			   gchar *certs);

  gpointer (*do_request)(AgsFrontController *front_controller,
			 gchar *context_path,
			 gchar *user_uuid,
			 gchar *security_token,
			 gchar *certs,
			 GParameter *params);
};

GType ags_front_controller_get_type();

gpointer ags_front_controller_authenticate(AgsFrontController *front_controller,
					   gchar *login,
					   gchar *password,
					   gchar *certs);

gpointer ags_front_controller_do_request(AgsFrontController *front_controller,
					 gchar *context_path,
					 gchar *user_uuid,
					 gchar *security_token,
					 gchar *certs,
					 GParameter *params);

AgsFrontController* ags_front_controller_new();

#endif /*__AGS_FRONT_CONTROLLER_H__*/
