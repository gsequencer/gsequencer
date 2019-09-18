/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/object/ags_connectable.h>

void ags_connectable_base_init(AgsConnectableInterface *ginterface);

/**
 * SECTION:ags_connectable
 * @short_description: unique access to objects
 * @title: AgsConnectable
 * @section_id: AgsConnectable
 * @include: ags/object/ags_connectable.h
 *
 * The #AgsConnectable interface gives you a unique access to all objects
 * and is responsible to set up signal handlers.
 */

GType
ags_connectable_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_connectable = 0;

    static const GTypeInfo ags_connectable_info = {
      sizeof(AgsConnectableInterface),
      (GBaseInitFunc) ags_connectable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_connectable = g_type_register_static(G_TYPE_INTERFACE,
						  "AgsConnectable", &ags_connectable_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_connectable);
  }

  return g_define_type_id__volatile;
}

void
ags_connectable_base_init(AgsConnectableInterface *ginterface)
{
  /* empty */
}

/**
 * ags_connectable_get_uuid:
 * @connectable: the #AgsConnectable
 *
 * Get UUID of @connectable.
 *
 * Returns: the assigned #AgsUUID
 *
 * Since: 2.0.0
 */
AgsUUID*
ags_connectable_get_uuid(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_val_if_fail(AGS_IS_CONNECTABLE(connectable), NULL);
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_val_if_fail(connectable_interface->get_uuid, NULL);

  return(connectable_interface->get_uuid(connectable));
}

/**
 * ags_connectable_has_resource:
 * @connectable: the #AgsConnectable
 *
 * Check the connectable to have resources.
 *
 * Returns: %TRUE if @connectable can be added to registry, otherwise %FALSE.
 *
 * Since: 2.0.0
 */
gboolean
ags_connectable_has_resource(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_val_if_fail(AGS_IS_CONNECTABLE(connectable), FALSE);
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_val_if_fail(connectable_interface->has_resource, FALSE);

  return(connectable_interface->has_resource(connectable));
}

/**
 * ags_connectable_is_ready:
 * @connectable: the #AgsConnectable
 *
 * Connect the connectable.
 *
 * Returns: %TRUE if is added to registry, otherwise %FALSE.
 *
 * Since: 2.0.0
 */
gboolean
ags_connectable_is_ready(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_val_if_fail(AGS_IS_CONNECTABLE(connectable), TRUE);
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_val_if_fail(connectable_interface->is_ready, TRUE);

  return(connectable_interface->is_ready(connectable));
}

/**
 * ags_connectable_add_to_registry:
 * @connectable: the #AgsConnectable
 *
 * Add connectable to registry.
 *
 * Since: 2.0.0
 */
void
ags_connectable_add_to_registry(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_if_fail(AGS_IS_CONNECTABLE(connectable));
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_if_fail(connectable_interface->add_to_registry);
  connectable_interface->add_to_registry(connectable);
}

/**
 * ags_connectable_remove_from_registry:
 * @connectable: the #AgsConnectable
 *
 * Remove connectable from registry.
 *
 * Since: 2.0.0
 */
void
ags_connectable_remove_from_registry(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_if_fail(AGS_IS_CONNECTABLE(connectable));
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_if_fail(connectable_interface->remove_from_registry);
  connectable_interface->remove_from_registry(connectable);
}

/**
 * ags_connectable_list_resource:
 * @connectable: the #AgsConnectable
 *
 * List resources as an XML element and return it.
 *
 * Returns: the #xmlNode-struct
 *
 * Since: 2.0.0
 */
xmlNode*
ags_connectable_list_resource(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_val_if_fail(AGS_IS_CONNECTABLE(connectable), NULL);
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_val_if_fail(connectable_interface->list_resource, NULL);

  return(connectable_interface->list_resource(connectable));
}

/**
 * ags_connectable_xml_compose:
 * @connectable: the #AgsConnectable
 *
 * Compose an XML element and return it.
 *
 * Returns: the #xmlNode-struct
 *
 * Since: 2.0.0
 */
xmlNode*
ags_connectable_xml_compose(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_val_if_fail(AGS_IS_CONNECTABLE(connectable), NULL);
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_val_if_fail(connectable_interface->xml_compose, NULL);

  return(connectable_interface->xml_compose(connectable));
}

/**
 * ags_connectable_xml_parse:
 * @connectable: the #AgsConnectable
 * @node: the #xmlNode-struct
 *
 * Parse @node as XML element and apply it.
 *
 * Since: 2.0.0
 */
void
ags_connectable_xml_parse(AgsConnectable *connectable,
			  xmlNode *node)
{
  AgsConnectableInterface *connectable_interface;

  g_return_if_fail(AGS_IS_CONNECTABLE(connectable));
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_if_fail(connectable_interface->xml_parse);
  connectable_interface->xml_parse(connectable,
				   node);
}

/**
 * ags_connectable_is_connected:
 * @connectable: the #AgsConnectable
 *
 * Check if the @connectable was connected.
 *
 * Returns: %TRUE if is connected, otherwise %FALSE.
 *
 * Since: 2.0.0
 */
gboolean
ags_connectable_is_connected(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_val_if_fail(AGS_IS_CONNECTABLE(connectable), FALSE);
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_val_if_fail(connectable_interface->is_connected, FALSE);

  return(connectable_interface->is_connected(connectable));
}

/**
 * ags_connectable_connect:
 * @connectable: the #AgsConnectable
 *
 * Connect the connectable.
 *
 * Since: 2.0.0
 */
void
ags_connectable_connect(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_if_fail(AGS_IS_CONNECTABLE(connectable));
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_if_fail(connectable_interface->connect);

  connectable_interface->connect(connectable);
}

/**
 * ags_connectable_disconnect:
 * @connectable: the #AgsConnectable
 *
 * Disconnect the connectable.
 *
 * Since: 2.0.0
 */
void
ags_connectable_disconnect(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_if_fail(AGS_IS_CONNECTABLE(connectable));
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_if_fail(connectable_interface->disconnect);
  connectable_interface->disconnect(connectable);
}

/**
 * ags_connectable_connect_connection:
 * @connectable: the #AgsConnectable
 * @connection: the connection
 *
 * Disconnect the connectable.
 *
 * Since: 2.0.0
 */
void
ags_connectable_connect_connection(AgsConnectable *connectable,
				   GObject *connection)
{
  AgsConnectableInterface *connectable_interface;

  g_return_if_fail(AGS_IS_CONNECTABLE(connectable));
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_if_fail(connectable_interface->connect_connection);
  connectable_interface->connect_connection(connectable,
					    connection);
}

/**
 * ags_connectable_disconnect_connection:
 * @connectable: the #AgsConnectable
 * @connection: the connection
 *
 * Disconnect the connectable.
 *
 * Since: 2.0.0
 */
void
ags_connectable_disconnect_connection(AgsConnectable *connectable,
				      GObject *connection)
{
  AgsConnectableInterface *connectable_interface;

  g_return_if_fail(AGS_IS_CONNECTABLE(connectable));
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_if_fail(connectable_interface->disconnect_connection);
  connectable_interface->disconnect_connection(connectable,
					       connection);
}
