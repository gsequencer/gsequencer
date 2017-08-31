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

#ifndef __AGS_LOCAL_REGISTRY_CONTROLLER_H__
#define __AGS_LOCAL_REGISTRY_CONTROLLER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/server/controller/ags_controller.h>

#include <ags/thread/ags_registry.h>
#include <ags/thread/ags_timestamp.h>

#define AGS_TYPE_LOCAL_REGISTRY_CONTROLLER                (ags_local_registry_controller_get_type())
#define AGS_LOCAL_REGISTRY_CONTROLLER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LOCAL_REGISTRY_CONTROLLER, AgsLocalRegistryController))
#define AGS_LOCAL_REGISTRY_CONTROLLER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_LOCAL_REGISTRY_CONTROLLER, AgsLocalRegistryControllerClass))
#define AGS_IS_LOCAL_REGISTRY_CONTROLLER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LOCAL_REGISTRY_CONTROLLER))
#define AGS_IS_LOCAL_REGISTRY_CONTROLLER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LOCAL_REGISTRY_CONTROLLER))
#define AGS_LOCAL_REGISTRY_CONTROLLER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_LOCAL_REGISTRY_CONTROLLER, AgsLocalRegistryControllerClass))

typedef struct _AgsLocalRegistryController AgsLocalRegistryController;
typedef struct _AgsLocalRegistryControllerClass AgsLocalRegistryControllerClass;

struct _AgsLocalRegistryController
{
  AgsController controller;
};

struct _AgsLocalRegistryControllerClass
{
  AgsControllerClass controller;

  gpointer (*entry_bulk)(AgsLocalRegistryController *local_registry_controller);
};

GType ags_local_registry_controller_get_type();

gpointer ags_local_registry_controller_entry_bulk(AgsLocalRegistryController *local_registry_controller);

AgsLocalRegistryController* ags_local_registry_controller_new();

#endif /*__AGS_LOCAL_REGISTRY_CONTROLLER_H__*/
