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

#include <ags/server/controller/ags_plugin_controller.h>

void ags_plugin_controller_class_init(AgsPluginControllerInterface *ginterface);

/**
 * SECTION:ags_plugin_controller
 * @short_description: provide abstract controller
 * @title: AgsPluginController
 * @section_id: AgsPluginController
 * @include: ags/server/controller/ags_plugin_controller.h
 *
 * The #AgsPluginController provides you an abstraction of controller implementation.
 */

GType
ags_plugin_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_plugin_controller = 0;

    ags_type_plugin_controller = g_type_register_static_simple(G_TYPE_INTERFACE,
							       "AgsPluginController",
							       sizeof(AgsPluginControllerInterface),
							       (GClassInitFunc) ags_plugin_controller_class_init,
							       0, NULL, 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_plugin_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_plugin_controller_class_init(AgsPluginControllerInterface *ginterface)
{
  /* empty */
}

/**
 * ags_plugin_controller_do_request:
 * @plugin_controller: the #AgsPluginController
 * @msg: the #SoupServerMessage
 * @query: the #GHashTable
 * @security_context: the #AgsSecurityContext
 * @path: the context path to access
 * @login: the login
 * @security_token: the security token
 * 
 * Do request.
 * 
 * Returns: the #GList-struct containing #AgsResponse
 * 
 * Since: 3.0.0
 */
gpointer
ags_plugin_controller_do_request(AgsPluginController *plugin_controller,
				 SoupServerMessage *msg,
				 GHashTable *query,
				 GObject *security_context,
				 gchar *path,
				 gchar *login,
				 gchar *security_token)
{
  AgsPluginControllerInterface *plugin_controller_interface;

  gpointer response;
  
  g_return_val_if_fail(AGS_IS_PLUGIN_CONTROLLER(plugin_controller), NULL);
  plugin_controller_interface = AGS_PLUGIN_CONTROLLER_GET_INTERFACE(plugin_controller);
  g_return_val_if_fail(plugin_controller_interface->do_request, NULL);

  response = plugin_controller_interface->do_request(plugin_controller,
						     msg,
						     query,
						     security_context,
						     path,
						     login,
						     security_token);

  return(response);
}
