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

#include <ags/audio/osc/controller/ags_osc_plugin_controller.h>

void ags_osc_plugin_controller_class_init(AgsOscPluginControllerInterface *ginterface);

/**
 * SECTION:ags_osc_plugin_controller
 * @short_description: provide abstract OSC controller
 * @title: AgsOscPluginController
 * @section_id: AgsOscPluginController
 * @include: ags/audio/osc/controller/ags_osc_plugin_controller.h
 *
 * The #AgsOscPluginController provides you an abstraction of OSC controller implementation.
 */

GType
ags_osc_plugin_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_plugin_controller = 0;

    ags_type_osc_plugin_controller = g_type_register_static_simple(G_TYPE_INTERFACE,
								   "AgsOscPluginController",
								   sizeof(AgsOscPluginControllerInterface),
								   (GClassInitFunc) ags_osc_plugin_controller_class_init,
								   0, NULL, 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_plugin_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_plugin_controller_class_init(AgsOscPluginControllerInterface *ginterface)
{
  /* empty */
}

/**
 * ags_osc_plugin_controller_do_request:
 * @osc_plugin_controller: the #AgsOscPluginController
 * @osc_connection: the #AgsOscConnection
 * @message: the OSC message
 * @message_size: the message size
 * 
 * Do request.
 * 
 * Returns: the #GList-struct containing #AgsOscResponse
 * 
 * Since: 2.1.0
 */
gpointer
ags_osc_plugin_controller_do_request(AgsOscPluginController *osc_plugin_controller,
				     AgsOscConnection *osc_connection,
				     unsigned char *message, guint message_size)
{
  AgsOscPluginControllerInterface *osc_plugin_controller_interface;

  gpointer response;
  
  g_return_val_if_fail(AGS_IS_OSC_PLUGIN_CONTROLLER(osc_plugin_controller), NULL);
  osc_plugin_controller_interface = AGS_OSC_PLUGIN_CONTROLLER_GET_INTERFACE(osc_plugin_controller);
  g_return_val_if_fail(osc_plugin_controller_interface->do_request, NULL);

  response = osc_plugin_controller_interface->do_request(osc_plugin_controller,
							 osc_connection,
							 message, message_size);

  return(response);
}
