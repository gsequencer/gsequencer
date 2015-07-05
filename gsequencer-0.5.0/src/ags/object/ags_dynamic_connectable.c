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

#include <ags/object/ags_dynamic_connectable.h>

#include <stdio.h>

void ags_dynamic_connectable_base_init(AgsDynamicConnectableInterface *interface);

/**
 * SECTION:ags_dynamic_connectable
 * @short_description: unique access to recalls
 * @title: AgsDynamicConnectable
 * @section_id:
 * @include: ags/object/ags_dynamic_connectable.h
 *
 * The #AgsDynamicConnectable interface gives you a unique access to recalls. Its main
 * purpose is to setup up signal handlers with dynamic context for ::connect-dynamic.
 * A dynamic context would be a dependency that has to be resolved first within run
 * initialization.
 */

GType
ags_dynamic_connectable_get_type()
{
  static GType ags_type_dynamic_connectable = 0;

  if(!ags_type_dynamic_connectable){
    static const GTypeInfo ags_dynamic_connectable_info = {
      sizeof(AgsDynamicConnectableInterface),
      (GBaseInitFunc) ags_dynamic_connectable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_dynamic_connectable = g_type_register_static(G_TYPE_INTERFACE,
							  "AgsDynamicConnectable\0", &ags_dynamic_connectable_info,
							  0);
  }

  return(ags_type_dynamic_connectable);
}

void
ags_dynamic_connectable_base_init(AgsDynamicConnectableInterface *interface)
{
  /* empty */
}

/**
 * ags_dynamic_connectable_connect_dynamic:
 * @connectable: an #AgsDynamicConnectable
 *
 * Connect signal handlers within dynamic context.
 *
 * Since: 0.3
 */
void
ags_dynamic_connectable_connect_dynamic(AgsDynamicConnectable *connectable)
{
  AgsDynamicConnectableInterface *connectable_interface;

  g_return_if_fail(AGS_IS_DYNAMIC_CONNECTABLE(connectable));
  connectable_interface = AGS_DYNAMIC_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_if_fail(connectable_interface->connect_dynamic);
  connectable_interface->connect_dynamic(connectable);
}

/**
 * ags_dynamic_connectable_disconnect_dynamic:
 * @connectable: an #AgsDynamicConnectable
 *
 * Disconnect signal handlers within dynamic context.
 *
 * Since: 0.3
 */
void
ags_dynamic_connectable_disconnect_dynamic(AgsDynamicConnectable *connectable)
{
  AgsDynamicConnectableInterface *connectable_interface;

  g_return_if_fail(AGS_IS_DYNAMIC_CONNECTABLE(connectable));
  connectable_interface = AGS_DYNAMIC_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_if_fail(connectable_interface->disconnect_dynamic);
  connectable_interface->disconnect_dynamic(connectable);
}
