/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/object/ags_plugin.h>

#include <math.h>

void ags_plugin_base_init(AgsPluginInterface *interface);

GType
ags_plugin_get_type()
{
  static GType ags_type_plugin = 0;

  if(!ags_type_plugin){
    static const GTypeInfo ags_plugin_info = {
      sizeof(AgsPluginInterface),
      (GBaseInitFunc) ags_plugin_base_init,
      NULL, /* base_finalize */
    };

    ags_type_plugin = g_type_register_static(G_TYPE_INTERFACE,
					     "AgsPlugin\0", &ags_plugin_info,
					     0);
  }

  return(ags_type_plugin);
}

void
ags_plugin_base_init(AgsPluginInterface *interface)
{
  /* empty */
}

gchar*
ags_plugin_get_name(AgsPlugin *plugin)
{
  AgsPluginInterface *plugin_interface;
  gchar *ret_val;

  g_return_val_if_fail(AGS_IS_PLUGIN(plugin), NULL);
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_val_if_fail(plugin_interface->get_name, NULL);
  ret_val = plugin_interface->get_name(plugin);

  return(ret_val);
}

void
ags_plugin_set_name(AgsPlugin *plugin, gchar *name)
{
  AgsPluginInterface *plugin_interface;

  g_return_if_fail(AGS_IS_PLUGIN(plugin));
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_if_fail(plugin_interface->set_name);
  plugin_interface->set_name(plugin, name);
}

gchar*
ags_plugin_get_version(AgsPlugin *plugin)
{
  AgsPluginInterface *plugin_interface;
  gchar *ret_val;

  g_return_val_if_fail(AGS_IS_PLUGIN(plugin), NULL);
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_val_if_fail(plugin_interface->get_version, NULL);
  ret_val = plugin_interface->get_version(plugin);

  return(ret_val);
}

void
ags_plugin_set_version(AgsPlugin *plugin, gchar *version)
{
  AgsPluginInterface *plugin_interface;

  g_return_if_fail(AGS_IS_PLUGIN(plugin));
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_if_fail(plugin_interface->set_version);
  plugin_interface->set_version(plugin, version);
}

gchar*
ags_plugin_get_build_id(AgsPlugin *plugin)
{
  AgsPluginInterface *plugin_interface;
  gchar *ret_val;

  g_return_val_if_fail(AGS_IS_PLUGIN(plugin), NULL);
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_val_if_fail(plugin_interface->get_build_id, NULL);
  ret_val = plugin_interface->get_build_id(plugin);

  return(ret_val);
}

void
ags_plugin_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AgsPluginInterface *plugin_interface;

  g_return_if_fail(AGS_IS_PLUGIN(plugin));
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_if_fail(plugin_interface->set_build_id);
  plugin_interface->set_build_id(plugin, build_id);
}

gchar*
ags_plugin_get_xml_type(AgsPlugin *plugin)
{
  AgsPluginInterface *plugin_interface;
  gchar *ret_val;

  g_return_val_if_fail(AGS_IS_PLUGIN(plugin), NULL);
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_val_if_fail(plugin_interface->get_xml_type, NULL);
  ret_val = plugin_interface->get_xml_type(plugin);

  return(ret_val);
}

void
ags_plugin_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AgsPluginInterface *plugin_interface;

  g_return_if_fail(AGS_IS_PLUGIN(plugin));
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_if_fail(plugin_interface->set_xml_type);
  plugin_interface->set_xml_type(plugin, xml_type);
}

GList*
ags_plugin_get_ports(AgsPlugin *plugin)
{
  AgsPluginInterface *plugin_interface;
  GList *ret_val;

  g_return_val_if_fail(AGS_IS_PLUGIN(plugin), NULL);
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_val_if_fail(plugin_interface->get_ports, NULL);
  ret_val = plugin_interface->get_ports(plugin);

  return(ret_val);
}

void
ags_plugin_set_ports(AgsPlugin *plugin, GList *ports)
{
  AgsPluginInterface *plugin_interface;

  g_return_if_fail(AGS_IS_PLUGIN(plugin));
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_if_fail(plugin_interface->set_ports);
  plugin_interface->set_ports(plugin, ports);
}

void
ags_plugin_read(AgsFile *file,
		xmlNode *node,
		AgsPlugin *plugin)
{
  AgsPluginInterface *plugin_interface;

  g_return_if_fail(AGS_IS_PLUGIN(plugin));
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_if_fail(plugin_interface->read);
  plugin_interface->read(plugin, node, plugin);
}

xmlNode*
ags_plugin_write(AgsFile *file,
		 xmlNode *parent,
		 AgsPlugin *plugin)
{
  AgsPluginInterface *plugin_interface;
  xmlNode *ret_val;

  g_return_val_if_fail(AGS_IS_PLUGIN(plugin), NULL);
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_val_if_fail(plugin_interface->write, NULL);
  ret_val = plugin_interface->write(plugin, parent, plugin);

  return(ret_val);
}
