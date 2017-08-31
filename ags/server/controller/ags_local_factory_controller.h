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

#ifndef __AGS_LOCAL_FACTORY_CONTROLLER_H__
#define __AGS_LOCAL_FACTORY_CONTROLLER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/server/controller/ags_controller.h>

#define AGS_TYPE_LOCAL_FACTORY_CONTROLLER                (ags_local_factory_controller_get_type())
#define AGS_LOCAL_FACTORY_CONTROLLER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LOCAL_FACTORY_CONTROLLER, AgsLocalFactoryController))
#define AGS_LOCAL_FACTORY_CONTROLLER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_LOCAL_FACTORY_CONTROLLER, AgsLocalFactoryControllerClass))
#define AGS_IS_LOCAL_FACTORY_CONTROLLER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LOCAL_FACTORY_CONTROLLER))
#define AGS_IS_LOCAL_FACTORY_CONTROLLER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LOCAL_FACTORY_CONTROLLER))
#define AGS_LOCAL_FACTORY_CONTROLLER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_LOCAL_FACTORY_CONTROLLER, AgsLocalFactoryControllerClass))

#define AGS_LOCAL_FACTORY_CONTROLLER_CONTEXT_PATH "/local-factory"

typedef struct _AgsLocalFactoryController AgsLocalFactoryController;
typedef struct _AgsLocalFactoryControllerClass AgsLocalFactoryControllerClass;

struct _AgsLocalFactoryController
{
  AgsController controller;
};

struct _AgsLocalFactoryControllerClass
{
  AgsControllerClass controller;
  
  gpointer (*create_instance)(AgsLocalFactoryController *local_factory_controller,
			      GType gtype,
			      GParameter *parameter,
			      guint n_params);
};

GType ags_local_factory_controller_get_type();

gpointer ags_local_factory_controller_create_instance(AgsLocalFactoryController *local_factory_controller,
						      GType gtype,
						      GParameter *parameter,
						      guint n_params);

AgsLocalFactoryController* ags_local_factory_controller_new();

#endif /*__AGS_LOCAL_FACTORY_CONTROLLER_H__*/
