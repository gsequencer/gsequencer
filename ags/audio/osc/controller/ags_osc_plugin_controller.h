/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_OSC_PLUGIN_CONTROLLER_H__
#define __AGS_OSC_PLUGIN_CONTROLLER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/osc/ags_osc_connection.h>

G_BEGIN_DECLS

#define AGS_TYPE_OSC_PLUGIN_CONTROLLER                    (ags_osc_plugin_controller_get_type())
#define AGS_OSC_PLUGIN_CONTROLLER(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_PLUGIN_CONTROLLER, AgsOscPluginController))
#define AGS_OSC_PLUGIN_CONTROLLER_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_OSC_PLUGIN_CONTROLLER, AgsOscPluginControllerInterface))
#define AGS_IS_OSC_PLUGIN_CONTROLLER(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_OSC_PLUGIN_CONTROLLER))
#define AGS_IS_OSC_PLUGIN_CONTROLLER_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_OSC_PLUGIN_CONTROLLER))
#define AGS_OSC_PLUGIN_CONTROLLER_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_OSC_PLUGIN_CONTROLLER, AgsOscPluginControllerInterface))

typedef struct _AgsOscPluginController AgsOscPluginController;
typedef struct _AgsOscPluginControllerInterface AgsOscPluginControllerInterface;

struct _AgsOscPluginControllerInterface
{
  GTypeInterface ginterface;

  gpointer (*do_request)(AgsOscPluginController *osc_plugin_controller,
			 AgsOscConnection *osc_connection,
			 guchar *message, guint message_size);
};

GType ags_osc_plugin_controller_get_type();

gpointer ags_osc_plugin_controller_do_request(AgsOscPluginController *osc_plugin_controller,
					      AgsOscConnection *osc_connection,
					      guchar *message, guint message_size);

G_END_DECLS

#endif /*__AGS_OSC_PLUGIN_CONTROLLER_H__*/
