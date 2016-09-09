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

#include <ags/audio/ags_sound_provider.h>

#include <math.h>

void ags_sound_provider_class_init(AgsSoundProviderInterface *interface);

/**
 * SECTION:ags_sound_provider
 * @short_description: provide accoustics
 * @title: AgsSoundProvider
 * @section_id:
 * @include: ags/thread/ags_sound_provider.h
 *
 * The #AgsSoundProvider provides you the advanced audio capabilities.
 */

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

/**
 * ags_sound_provider_set_soundcard:
 * @sound_provider: the #AgsSoundProvider
 * @soundcard: a #GList-struc containing #AgsSoundcard
 * 
 * Set soundcards.
 *
 * Since: 0.7.4
 */
void
ags_sound_provider_set_soundcard(AgsSoundProvider *sound_provider,
				 GList *soundcard)
{
  AgsSoundProviderInterface *sound_provider_interface;

  g_return_if_fail(AGS_IS_SOUND_PROVIDER(sound_provider));
  sound_provider_interface = AGS_SOUND_PROVIDER_GET_INTERFACE(sound_provider);
  g_return_if_fail(sound_provider_interface->set_soundcard);

  sound_provider_interface->set_soundcard(sound_provider,
					  soundcard);
}

/**
 * ags_sound_provider_get_soundcard:
 * @sound_provider: the #AgsSoundProvider
 * 
 * Get soundcards.
 *
 * Returns: a #GList-struct containing #AgsSoundcard
 * 
 * Since: 0.7.4
 */
GList*
ags_sound_provider_get_soundcard(AgsSoundProvider *sound_provider)
{
  AgsSoundProviderInterface *sound_provider_interface;

  g_return_val_if_fail(AGS_IS_SOUND_PROVIDER(sound_provider), NULL);
  sound_provider_interface = AGS_SOUND_PROVIDER_GET_INTERFACE(sound_provider);
  g_return_val_if_fail(sound_provider_interface->get_soundcard, NULL);

  return(sound_provider_interface->get_soundcard(sound_provider));
}

/**
 * ags_sound_provider_set_default_soundcard_thread:
 * @sound_provider: the #AgsSoundProvider
 * @soundcard_thread: the default #AgsThread
 * 
 * set default soundcard thread.
 *
 * Returns: the #AgsThread
 * 
 * Since: 0.7.65
 */
void
ags_sound_provider_set_default_soundcard_thread(AgsSoundProvider *sound_provider,
						GObject *soundcard_thread)
{
  AgsSoundProviderInterface *sound_provider_interface;

  g_return_if_fail(AGS_IS_SOUND_PROVIDER(sound_provider));
  sound_provider_interface = AGS_SOUND_PROVIDER_GET_INTERFACE(sound_provider);
  g_return_if_fail(sound_provider_interface->set_default_soundcard_thread);

  sound_provider_interface->set_default_soundcard_thread(sound_provider,
							 soundcard_thread);
}

/**
 * ags_sound_provider_get_default_soundcard_thread:
 * @sound_provider: the #AgsSoundProvider
 * 
 * Get default soundcard thread.
 *
 * Returns: the #AgsThread
 * 
 * Since: 0.7.65
 */
GObject*
ags_sound_provider_get_default_soundcard_thread(AgsSoundProvider *sound_provider)
{
  AgsSoundProviderInterface *sound_provider_interface;

  g_return_val_if_fail(AGS_IS_SOUND_PROVIDER(sound_provider), NULL);
  sound_provider_interface = AGS_SOUND_PROVIDER_GET_INTERFACE(sound_provider);
  g_return_val_if_fail(sound_provider_interface->get_default_soundcard_thread, NULL);

  return(sound_provider_interface->get_default_soundcard_thread(sound_provider));
}

/**
 * ags_sound_provider_set_sequencer:
 * @sound_provider: the #AgsSoundProvider
 * @sequencer: a #GList-struc containing #AgsSequencer
 * 
 * Set sequencers.
 *
 * Since: 0.7.4
 */
void
ags_sound_provider_set_sequencer(AgsSoundProvider *sound_provider,
				 GList *sequencer)
{
  AgsSoundProviderInterface *sound_provider_interface;

  g_return_if_fail(AGS_IS_SOUND_PROVIDER(sound_provider));
  sound_provider_interface = AGS_SOUND_PROVIDER_GET_INTERFACE(sound_provider);
  g_return_if_fail(sound_provider_interface->set_sequencer);

  sound_provider_interface->set_sequencer(sound_provider,
					  sequencer);
}

/**
 * ags_sound_provider_get_sequencer:
 * @sound_provider: the #AgsSoundProvider
 * 
 * Get sequencers.
 *
 * Returns: a #GList-struct containing #AgsSequencer
 * 
 * Since: 0.7.4
 */
GList*
ags_sound_provider_get_sequencer(AgsSoundProvider *sound_provider)
{
  AgsSoundProviderInterface *sound_provider_interface;

  g_return_val_if_fail(AGS_IS_SOUND_PROVIDER(sound_provider), NULL);
  sound_provider_interface = AGS_SOUND_PROVIDER_GET_INTERFACE(sound_provider);
  g_return_val_if_fail(sound_provider_interface->get_sequencer, NULL);

  return(sound_provider_interface->get_sequencer(sound_provider));
}

/**
 * ags_sound_provider_set_distributed_manager:
 * @sound_provider: the #AgsSoundProvider
 * @distributed_manager: a #GList-struct containing #AgsDistributedManager
 *
 * Set distributed manager.
 * 
 * Since: 0.7.4
 */
void
ags_sound_provider_set_distributed_manager(AgsSoundProvider *sound_provider,
					   GList *distributed_manager)
{
  AgsSoundProviderInterface *sound_provider_interface;

  g_return_if_fail(AGS_IS_SOUND_PROVIDER(sound_provider));
  sound_provider_interface = AGS_SOUND_PROVIDER_GET_INTERFACE(sound_provider);
  g_return_if_fail(sound_provider_interface->set_distributed_manager);

  sound_provider_interface->set_distributed_manager(sound_provider,
						    distributed_manager);
}

/**
 * ags_sound_provider_get_distributed_manager:
 * @sound_provider: the #AgsSoundProvider
 *
 * Set distributed manager.
 * 
 * Returns: a #GList-struct containing #AgsDistributedManager
 *
 * Since: 0.7.4
 */
GList*
ags_sound_provider_get_distributed_manager(AgsSoundProvider *sound_provider)
{
  AgsSoundProviderInterface *sound_provider_interface;

  g_return_val_if_fail(AGS_IS_SOUND_PROVIDER(sound_provider), NULL);
  sound_provider_interface = AGS_SOUND_PROVIDER_GET_INTERFACE(sound_provider);
  g_return_val_if_fail(sound_provider_interface->get_distributed_manager, NULL);

  return(sound_provider_interface->get_distributed_manager(sound_provider));
}
