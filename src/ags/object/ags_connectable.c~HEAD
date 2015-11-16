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

#include <ags/object/ags_connectable.h>

/**
 * SECTION:ags_connectable
 * @short_description: unique access to recalls
 * @title: AgsConnectable
 * @section_id:
 * @include: ags/object/ags_connectable.h
 *
 * The #AgsConnectable interface gives you a unique access to objects.
 */

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

void
ags_connectable_add_to_registry(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_if_fail(AGS_IS_CONNECTABLE(connectable));
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_if_fail(connectable_interface->add_to_registry);
  connectable_interface->add_to_registry(connectable);
}

void
ags_connectable_remove_from_registry(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_if_fail(AGS_IS_CONNECTABLE(connectable));
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_if_fail(connectable_interface->remove_from_registry);
  connectable_interface->remove_from_registry(connectable);
}

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

gboolean
ags_connectable_is_connected(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_val_if_fail(AGS_IS_CONNECTABLE(connectable), FALSE);
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_val_if_fail(connectable_interface->is_connected, FALSE);

  return(connectable_interface->is_connected(connectable));
}

void
ags_connectable_connect(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_if_fail(AGS_IS_CONNECTABLE(connectable));
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_if_fail(connectable_interface->connect);

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  connectable_interface->connect(connectable);
}

void
ags_connectable_disconnect(AgsConnectable *connectable)
{
  AgsConnectableInterface *connectable_interface;

  g_return_if_fail(AGS_IS_CONNECTABLE(connectable));
  connectable_interface = AGS_CONNECTABLE_GET_INTERFACE(connectable);
  g_return_if_fail(connectable_interface->disconnect);
  connectable_interface->connect(connectable);
}
