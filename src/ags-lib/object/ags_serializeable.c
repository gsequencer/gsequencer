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

#include <ags-lib/object/ags_serializeable.h>

void ags_serializeable_base_init(AgsSerializeableInterface *interface);

GType
ags_serializeable_get_type()
{
  static GType ags_type_serializeable = 0;

  if(!ags_type_serializeable){
    static const GTypeInfo ags_serializeable_info = {
      sizeof(AgsSerializeableInterface),
      (GBaseInitFunc) ags_serializeable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_serializeable = g_type_register_static(G_TYPE_INTERFACE,
						  "AgsSerializeable\0", &ags_serializeable_info,
						  0);
  }

  return(ags_type_serializeable);
}

void
ags_serializeable_base_init(AgsSerializeableInterface *interface)
{
  /* empty */
}

gchar*
ags_serializeable_serialize(AgsSerializeable *serializeable)
{
  AgsSerializeableInterface *serializeable_interface;

  g_return_val_if_fail(AGS_IS_SERIALIZEABLE(serializeable), NULL);
  serializeable_interface = AGS_SERIALIZEABLE_GET_INTERFACE(serializeable);
  g_return_val_if_fail(serializeable_interface->serialize, NULL);
  return(serializeable_interface->serialize(serializeable));
}
