/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/audio/ags_sound_provider.h>

#include <math.h>

void ags_sound_provider_class_init(AgsSoundProviderInterface *interface);

GType
ags_sound_provider_get_type()
{
  static GType ags_type_sound_provider = 0;

  if(!ags_type_sound_provider){
    ags_type_sound_provider = g_type_register_static_simple(G_TYPE_INTERFACE,
							    "AgsSoundProvider\0",
							    sizeof(AgsSoundProviderInterface),
							    (GClassInitFunc) ags_sound_provider_class_init,
							    0, NULL, 0);
  }

  return(ags_type_sound_provider);
}

void
ags_sound_provider_class_init(AgsSoundProviderInterface *interface)
{
  /* empty */
}

GList*
ags_sound_provider_get_soundcard(AgsSoundProvider *sound_provider)
{
  AgsSoundProviderInterface *sound_provider_interface;

  g_return_val_if_fail(AGS_IS_SOUND_PROVIDER(sound_provider), NULL);
  sound_provider_interface = AGS_SOUND_PROVIDER_GET_INTERFACE(sound_provider);
  g_return_val_if_fail(sound_provider_interface->get_soundcard, NULL);

  return(sound_provider_interface->get_soundcard(sound_provider));
}
