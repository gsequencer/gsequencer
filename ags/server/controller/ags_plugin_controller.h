/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_PLUGIN_CONTROLLER_H__
#define __AGS_PLUGIN_CONTROLLER_H__

#include <glib.h>
#include <glib-object.h>

#include <gio/gio.h>

#include <libsoup/soup.h>

G_BEGIN_DECLS

#define AGS_TYPE_PLUGIN_CONTROLLER                    (ags_plugin_controller_get_type())
#define AGS_PLUGIN_CONTROLLER(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLUGIN_CONTROLLER, AgsPluginController))
#define AGS_PLUGIN_CONTROLLER_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_PLUGIN_CONTROLLER, AgsPluginControllerInterface))
#define AGS_IS_PLUGIN_CONTROLLER(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PLUGIN_CONTROLLER))
#define AGS_IS_PLUGIN_CONTROLLER_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_PLUGIN_CONTROLLER))
#define AGS_PLUGIN_CONTROLLER_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_PLUGIN_CONTROLLER, AgsPluginControllerInterface))

typedef struct _AgsPluginController AgsPluginController;
typedef struct _AgsPluginControllerInterface AgsPluginControllerInterface;

struct _AgsPluginControllerInterface
{
  GTypeInterface ginterface;

  gpointer (*do_request)(AgsPluginController *plugin_controller,
			 SoupServerMessage *msg,
			 GHashTable *query,
			 GObject *security_context,
			 gchar *path,
			 gchar *login,
			 gchar *security_token);
};

GType ags_plugin_controller_get_type();

gpointer ags_plugin_controller_do_request(AgsPluginController *plugin_controller,
					  SoupServerMessage *msg,
					  GHashTable *query,
					  GObject *security_context,
					  gchar *path,
					  gchar *login,
					  gchar *security_token);

G_END_DECLS

#endif /*__AGS_PLUGIN_CONTROLLER_H__*/
