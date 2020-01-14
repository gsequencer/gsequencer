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

#ifndef __AGS_OSC_NODE_CONTROLLER_H__
#define __AGS_OSC_NODE_CONTROLLER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/osc/controller/ags_osc_controller.h>

G_BEGIN_DECLS

#define AGS_TYPE_OSC_NODE_CONTROLLER                (ags_osc_node_controller_get_type())
#define AGS_OSC_NODE_CONTROLLER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_NODE_CONTROLLER, AgsOscNodeController))
#define AGS_OSC_NODE_CONTROLLER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_OSC_NODE_CONTROLLER, AgsOscNodeControllerClass))
#define AGS_IS_OSC_NODE_CONTROLLER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_NODE_CONTROLLER))
#define AGS_IS_OSC_NODE_CONTROLLER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_NODE_CONTROLLER))
#define AGS_OSC_NODE_CONTROLLER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_OSC_NODE_CONTROLLER, AgsOscNodeControllerClass))

#define AGS_OSC_NODE_CONTROLLER_CONTEXT_PATH_LENGTH (8)

typedef struct _AgsOscNodeController AgsOscNodeController;
typedef struct _AgsOscNodeControllerClass AgsOscNodeControllerClass;

struct _AgsOscNodeController
{
  AgsOscController osc_controller;
};

struct _AgsOscNodeControllerClass
{
  AgsOscControllerClass osc_controller;

  gpointer (*get_data)(AgsOscNodeController *osc_node_controller,
		       AgsOscConnection *osc_connection,
		       unsigned char *message, guint message_size);
};

GType ags_osc_node_controller_get_type();

gpointer ags_osc_node_controller_get_data(AgsOscNodeController *osc_node_controller,
					  AgsOscConnection *osc_connection,
					  unsigned char *message, guint message_size);

AgsOscNodeController* ags_osc_node_controller_new();

G_END_DECLS

#endif /*__AGS_OSC_NODE_CONTROLLER_H__*/
