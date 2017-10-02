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

#include <ags/object/ags_packable.h>

void ags_packable_base_init(AgsPackableInterface *interface);

/**
 * SECTION:ags_packable
 * @short_description: pack ressources
 * @title: AgsPackable
 * @section_id:
 * @include: ags/object/ags_packable.h
 *
 * The #AgsPackable interface gives you a unique access to packable classes.
 */

GType
ags_packable_get_type()
{
  static GType ags_type_packable = 0;

  if(!ags_type_packable){
    static const GTypeInfo ags_packable_info = {
      sizeof(AgsPackableInterface),
      (GBaseInitFunc) ags_packable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_packable = g_type_register_static(G_TYPE_INTERFACE,
					       "AgsPackable", &ags_packable_info,
					       0);
  }

  return(ags_type_packable);
}

void
ags_packable_base_init(AgsPackableInterface *interface)
{
  /* empty */
}

/**
 * ags_packable_pack:
 * @packable: the #AgsPackable
 * @container: the container
 *
 * Pack to container.
 *
 * Returns: %TRUE on success
 *
 * Since: 1.0.0
 */
gboolean
ags_packable_pack(AgsPackable *packable, GObject *container)
{
  AgsPackableInterface *packable_interface;

  g_return_val_if_fail(AGS_IS_PACKABLE(packable), TRUE);
  packable_interface = AGS_PACKABLE_GET_INTERFACE(packable);
  g_return_val_if_fail(packable_interface->pack, TRUE);

  return(packable_interface->pack(packable, container));
}

/**
 * ags_packable_unpack:
 * @packable: the #AgsPackable
 *
 * Unpack from container.
 *
 * Returns: %TRUE on success
 *
 * Since: 1.0.0
 */
gboolean
ags_packable_unpack(AgsPackable *packable)
{
  AgsPackableInterface *packable_interface;

  g_return_val_if_fail(AGS_IS_PACKABLE(packable), TRUE);
  packable_interface = AGS_PACKABLE_GET_INTERFACE(packable);
  g_return_val_if_fail(packable_interface->unpack, TRUE);

  return(packable_interface->unpack(packable));
}
