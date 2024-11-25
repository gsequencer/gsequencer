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

#include <ags/object/ags_plugin.h>

#include <math.h>

void ags_plugin_base_init(AgsPluginInterface *ginterface);

/**
 * SECTION:ags_plugin
 * @short_description: interfacing plugins
 * @title: AgsPlugin
 * @section_id: AgsPlugin
 * @include: ags/object/ags_plugin.h
 *
 * The #AgsPlugin interface gives you a unique access to classes. It
 * can be used by #AgsFile and solves some of the serialization.
 */

GType
ags_plugin_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_plugin = 0;

    static const GTypeInfo ags_plugin_info = {
      sizeof(AgsPluginInterface),
      (GBaseInitFunc) ags_plugin_base_init,
      NULL, /* base_finalize */
    };

    ags_type_plugin = g_type_register_static(G_TYPE_INTERFACE,
					     "AgsPlugin", &ags_plugin_info,
					     0);

    g_once_init_leave(&g_define_type_id__static, ags_type_plugin);
  }

  return g_define_type_id__static;
}

void
ags_plugin_base_init(AgsPluginInterface *ginterface)
{
  /* empty */
}

/**
 * ags_plugin_get_name:
 * @plugin: an @AgsPlugin
 *
 * Retrieve the name of the plugin.
 *
 * Returns: the plugins name
 *
 * Since: 3.0.0
 */
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

/**
 * ags_plugin_set_name:
 * @plugin: an @AgsPlugin
 * @name: the name of plugin
 *
 * Set the name of the plugin.
 *
 * Since: 3.0.0
 */
void
ags_plugin_set_name(AgsPlugin *plugin, gchar *name)
{
  AgsPluginInterface *plugin_interface;

  g_return_if_fail(AGS_IS_PLUGIN(plugin));
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_if_fail(plugin_interface->set_name);
  plugin_interface->set_name(plugin, name);
}

/**
 * ags_plugin_get_version:
 * @plugin: an @AgsPlugin
 *
 * Retrieve the version of the plugin.
 *
 * Returns: the plugins version
 *
 * Since: 3.0.0
 */
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

/**
 * ags_plugin_set_version:
 * @plugin: an @AgsPlugin
 * @version: the version of plugin
 *
 * Set the version of the plugin.
 *
 * Since: 3.0.0
 */
void
ags_plugin_set_version(AgsPlugin *plugin, gchar *version)
{
  AgsPluginInterface *plugin_interface;

  g_return_if_fail(AGS_IS_PLUGIN(plugin));
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_if_fail(plugin_interface->set_version);
  plugin_interface->set_version(plugin, version);
}

/**
 * ags_plugin_get_build_id:
 * @plugin: an @AgsPlugin
 *
 * Retrieve the build id of the plugin.
 *
 * Returns: the plugins build id
 *
 * Since: 3.0.0
 */
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

/**
 * ags_plugin_set_build_id:
 * @plugin: an @AgsPlugin
 * @build_id: the build id of plugin
 *
 * Set the build id of the plugin.
 *
 * Since: 3.0.0
 */
void
ags_plugin_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AgsPluginInterface *plugin_interface;

  g_return_if_fail(AGS_IS_PLUGIN(plugin));
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_if_fail(plugin_interface->set_build_id);
  plugin_interface->set_build_id(plugin, build_id);
}

/**
 * ags_plugin_get_xml_type:
 * @plugin: an @AgsPlugin
 *
 * Retrieve the xml type of the plugin.
 *
 * Returns: the plugins xml type
 *
 * Since: 3.0.0
 */
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

/**
 * ags_plugin_set_xml_type:
 * @plugin: an @AgsPlugin
 * @xml_type: the build id of plugin
 *
 * Set the build id of the plugin.
 *
 * Since: 3.0.0
 */
void
ags_plugin_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AgsPluginInterface *plugin_interface;

  g_return_if_fail(AGS_IS_PLUGIN(plugin));
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_if_fail(plugin_interface->set_xml_type);
  plugin_interface->set_xml_type(plugin, xml_type);
}

/**
 * ags_plugin_get_ports:
 * @plugin: an @AgsPlugin
 *
 * Retrieve the ports of the plugin.
 *
 * Returns: (element-type GObject) (transfer full): the plugins ports
 *
 * Since: 3.0.0
 */
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

/**
 * ags_plugin_set_ports:
 * @plugin: an @AgsPlugin
 * @ports: (element-type GObject) (transfer none): the build id of plugin
 *
 * Set the build id of the plugin.
 *
 * Since: 3.0.0
 */
void
ags_plugin_set_ports(AgsPlugin *plugin, GList *ports)
{
  AgsPluginInterface *plugin_interface;

  g_return_if_fail(AGS_IS_PLUGIN(plugin));
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_if_fail(plugin_interface->set_ports);
  plugin_interface->set_ports(plugin, ports);
}

/**
 * ags_plugin_read:
 * @file: the #GObject
 * @node: the node
 * @plugin: the #AgsPlugin
 *
 * Read of file.
 *
 * Since: 3.0.0
 */
void
ags_plugin_read(GObject *file,
		xmlNode *node,
		AgsPlugin *plugin)
{
  AgsPluginInterface *plugin_interface;

  g_return_if_fail(AGS_IS_PLUGIN(plugin));
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_if_fail(plugin_interface->read);
  plugin_interface->read(file, node, plugin);
}


/**
 * ags_plugin_write:
 * @file: the #GObject
 * @parent: the parent node
 * @plugin: the #AgsPlugin
 *
 * Write to file.
 *
 * Returns: (transfer none): the new node you created
 *
 * Since: 3.0.0
 */
xmlNode*
ags_plugin_write(GObject *file,
		 xmlNode *parent,
		 AgsPlugin *plugin)
{
  AgsPluginInterface *plugin_interface;
  xmlNode *retval;

  g_return_val_if_fail(AGS_IS_PLUGIN(plugin), NULL);
  plugin_interface = AGS_PLUGIN_GET_INTERFACE(plugin);
  g_return_val_if_fail(plugin_interface->write, NULL);
  retval = plugin_interface->write(file, parent, plugin);

  return(retval);
}
