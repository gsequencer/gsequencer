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

#include <ags/object/ags_portlet.h>

void ags_portlet_base_init(AgsPortletInterface *ginterface);

/**
 * SECTION:ags_portlet
 * @short_description: interfacing portlets
 * @title: AgsPortlet
 * @section_id: AgsPortlet
 * @include: ags/object/ags_portlet.h
 *
 * The #AgsPortlet interface is intended to use with ports containing #GObject.
 * Whereas its should implement this interface.
 */

GType
ags_portlet_get_type()
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_portlet = 0;

    static const GTypeInfo ags_portlet_info = {
      sizeof(AgsPortletInterface),
      (GBaseInitFunc) ags_portlet_base_init,
      NULL, /* base_finalize */
    };

    ags_type_portlet = g_type_register_static(G_TYPE_INTERFACE,
					      "AgsPortlet", &ags_portlet_info,
					      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_portlet);
  }

  return g_define_type_id__volatile;
}

void
ags_portlet_base_init(AgsPortletInterface *ginterface)
{
  /* empty */
}

/**
 * ags_portlet_set_port:
 * @portlet: the #AgsPortlet
 * @port: the #GObject
 *
 * Set port.
 *
 * Since: 3.0.0
 */
void
ags_portlet_set_port(AgsPortlet *portlet, GObject *port)
{
  AgsPortletInterface *portlet_interface;

  g_return_if_fail(AGS_IS_PORTLET(portlet));
  portlet_interface = AGS_PORTLET_GET_INTERFACE(portlet);
  g_return_if_fail(portlet_interface->set_port);
  portlet_interface->set_port(portlet, port);
}

/**
 * ags_portlet_get_port:
 * @portlet: the #AgsPortlet
 *
 * Get port.
 *
 * Returns: (transfer full): the #GObject
 *
 * Since: 3.0.0
 */
GObject*
ags_portlet_get_port(AgsPortlet *portlet)
{
  AgsPortletInterface *portlet_interface;

  g_return_val_if_fail(AGS_IS_PORTLET(portlet), NULL);
  portlet_interface = AGS_PORTLET_GET_INTERFACE(portlet);
  g_return_val_if_fail(portlet_interface->get_port, NULL);
  
  return(portlet_interface->get_port(portlet));
}

/**
 * ags_portlet_list_safe_properties:
 * @portlet: the #AgsPortlet
 *
 * Retrieve thread-safe properties.
 *
 * Returns: (element-type utf8) (transfer full): a #GList containing properties
 *
 * Since: 3.0.0
 */
GList*
ags_portlet_list_safe_properties(AgsPortlet *portlet)
{
  AgsPortletInterface *portlet_interface;

  g_return_val_if_fail(AGS_IS_PORTLET(portlet), NULL);
  portlet_interface = AGS_PORTLET_GET_INTERFACE(portlet);
  g_return_val_if_fail(portlet_interface->list_safe_properties, NULL);
  
  return(portlet_interface->list_safe_properties(portlet));
}

/**
 * ags_portlet_safe_get_property:
 * @portlet: the #AgsPortlet
 * @property_name: propertie's name
 * @value: the #GValue
 *
 * Get property thread safe.
 *
 * Since: 3.0.0
 */
void
ags_portlet_safe_get_property(AgsPortlet *portlet, gchar *property_name, GValue *value)
{
  AgsPortletInterface *portlet_interface;

  g_return_if_fail(AGS_IS_PORTLET(portlet));
  portlet_interface = AGS_PORTLET_GET_INTERFACE(portlet);
  g_return_if_fail(portlet_interface->safe_get_property);
  portlet_interface->safe_get_property(portlet, property_name, value);
}

/**
 * ags_portlet_safe_set_property:
 * @portlet: the #AgsPortlet
 * @property_name: propertie's name
 * @value: the #GValue
 *
 * Set property thread safe.
 *
 * Since: 3.0.0
 */
void
ags_portlet_safe_set_property(AgsPortlet *portlet, gchar *property_name, GValue *value)
{
  AgsPortletInterface *portlet_interface;

  g_return_if_fail(AGS_IS_PORTLET(portlet));
  portlet_interface = AGS_PORTLET_GET_INTERFACE(portlet);
  g_return_if_fail(portlet_interface->safe_set_property);
  portlet_interface->safe_set_property(portlet, property_name, value);
}
