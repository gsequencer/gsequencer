/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/object/ags_dynamic_connectable.h>

void ags_dynamic_connectable_base_init(AgsDynamicConnectableInterface *interface);

/**
 * SECTION:ags_dynamic_connectable
 * @short_description: unique access to recalls
 * @title: AgsDynamicConnectable
 * @section_id:
 * @include: ags/object/ags_dynamic_connectable.h
 *
 * The #AgsDynamicConnectable interface gives you a unique access to recalls.
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
 * Connect dynamic.
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
 * Disconnect dynamic.
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
