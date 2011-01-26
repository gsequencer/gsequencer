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

#include <ags/object/ags_tactable.h>

void ags_tactable_base_init(AgsTactableInterface *interface);

GType
ags_tactable_get_type()
{
  static GType ags_type_tactable = 0;

  if(!ags_type_tactable){
    static const GTypeInfo ags_tactable_info = {
      sizeof(AgsTactableInterface),
      (GBaseInitFunc) ags_tactable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_tactable = g_type_register_static(G_TYPE_INTERFACE,
					       "AgsTactable\0", &ags_tactable_info,
					       0);
  }

  return(ags_type_tactable);
}

void
ags_tactable_base_init(AgsTactableInterface *interface)
{
  /* empty */
}

void
ags_tactable_change_bpm(AgsTactable *tactable, double bpm, double old_bpm)
{
  AgsTactableInterface *tactable_interface;

  g_return_if_fail(AGS_IS_TACTABLE(tactable));
  tactable_interface = AGS_TACTABLE_GET_INTERFACE(tactable);
  g_return_if_fail(tactable_interface->change_bpm);
  tactable_interface->change_bpm(tactable, bpm, old_bpm);
}
