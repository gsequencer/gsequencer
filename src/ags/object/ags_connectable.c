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

#include <ags/object/ags_connectable.h>

void ags_connectable_base_init(AgsConnectableInterface *interface);

GType
ags_connectable_get_type()
{
  static GType ags_type_connectable = 0;

  if(!ags_type_connectable){
    static const GTypeInfo ags_connectable_info = {
      sizeof(AgsConnectableInterface),
      (GBaseInitFunc) ags_connectable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_connectable = g_type_register_static(G_TYPE_INTERFACE,
						  "AgsConnectable\0", &ags_connectable_info,
						  0);
  }

  return(ags_type_connectable);
}

void
ags_connectable_base_init(AgsConnectableInterface *interface)
{
  /* empty */
}

/**
 * ags_connectable_add_to_registry:
 * @connectable: the #AgsConnectable
 *
 * Add connectable to registry.
 *
 * Since: 0.4.0
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
 * Since: 0.4.0
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
 * ags_connectable_is_ready:
 * @connectable: the #AgsConnectable
 *
 * Connect the connectable.
 *
 * Returns: %TRUE if is added to registry, otherwise %FALSE.
 *
 * Since: 0.4.2
 */
gboolean
ags_connectable_is_ready(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_val_if_fail(AGS_IS_CONNECTABLE(connectable), FALSE);
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);

  if(connectable_interface->is_ready == NULL)
    return(TRUE);

  return(connectable_interface->is_ready(connectable));
}

/**
 * ags_connectable_is_connected:
 * @connectable: the #AgsConnectable
 *
 * Connect the connectable.
 *
 * Returns: %TRUE if is connected, otherwise %FALSE.
 *
 * Since: 0.4.2
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
 * Since: 0.4.0
 */
void
ags_connectable_connect(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_if_fail(AGS_IS_CONNECTABLE(connectable));
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_if_fail(connectable_interface->connect);

  //  if(!ags_connectable_is_ready(connectable)){
  //    return;
  //  }

  connectable_interface->connect(connectable);
}

/**
 * ags_connectable_disconnect:
 * @connectable: the #AgsConnectable
 *
 * Disconnect the connectable.
 *
 * Since: 0.4.0
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
