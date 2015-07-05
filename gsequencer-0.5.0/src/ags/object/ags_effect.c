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
#include <ags/object/ags_effect.h>

#include <stdio.h>

void ags_effect_base_init(AgsEffectInterface *interface);

GType
ags_effect_get_type()
{
  static GType ags_type_effect = 0;

  if(!ags_type_effect){
    static const GTypeInfo ags_effect_info = {
      sizeof(AgsEffectInterface),
      (GBaseInitFunc) ags_effect_base_init,
      NULL, /* base_finalize */
    };

    ags_type_effect = g_type_register_static(G_TYPE_INTERFACE,
					     "AgsEffect\0", &ags_effect_info,
					     0);
  }

  return(ags_type_effect);
}

void
ags_effect_base_init(AgsEffectInterface *interface)
{
  /* empty */
}
