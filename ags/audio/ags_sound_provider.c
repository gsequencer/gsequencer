/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

void ags_sound_provider_class_init(AgsSoundProviderInterface *ginterface);

/**
 * SECTION:ags_sound_provider
 * @short_description: Sound provider interface
 * @title: AgsSoundProvider
 * @section_id: AgsSoundProvider
 * @include: ags/audio/ags_sound_provider.h
 *
 * The #AgsSoundProvider provides you the advanced audio capabilities.
 */

GType
ags_sound_provider_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sound_provider = 0;

    ags_type_sound_provider = g_type_register_static_simple(G_TYPE_INTERFACE,
							    "AgsSoundProvider",
							    sizeof(AgsSoundProviderInterface),
							    (GClassInitFunc) ags_sound_provider_class_init,
							    0, NULL, 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sound_provider);
  }

  return g_define_type_id__volatile;
}

void
ags_sound_provider_class_init(AgsSoundProviderInterface *ginterface)
{
  /* empty */
}

/**
 * ags_sound_provider_set_default_soundcard:
 * @sound_provider: the #AgsSoundProvider
 * @soundcard: the #GObject implementing #AgsSoundcard
 * 
 * Set default soundcard.
 * 
 * Since: 3.0.0
 */
void
ags_sound_provider_set_default_soundcard(AgsSoundProvider *sound_provider,
					 GObject *soundcard)
{
  AgsSoundProviderInterface *sound_provider_interface;

  g_return_if_fail(AGS_IS_SOUND_PROVIDER(sound_provider));
  sound_provider_interface = AGS_SOUND_PROVIDER_GET_INTERFACE(sound_provider);
  g_return_if_fail(sound_provider_interface->set_default_soundcard);

  sound_provider_interface->set_default_soundcard(sound_provider,
						  soundcard);
}

/**
 * ags_sound_provider_get_default_soundcard:
 * @sound_provider: the #AgsSoundProvider
 * 
 * Get default soundcard thread.
 *
 * Returns: (transfer full): the #GObject implementing #AgsSoundcard
 * 
 * Since: 3.0.0
 */
GObject*
ags_sound_provider_get_default_soundcard(AgsSoundProvider *sound_provider)
{
  AgsSoundProviderInterface *sound_provider_interface;

  g_return_val_if_fail(AGS_IS_SOUND_PROVIDER(sound_provider), NULL);
  sound_provider_interface = AGS_SOUND_PROVIDER_GET_INTERFACE(sound_provider);
  g_return_val_if_fail(sound_provider_interface->get_default_soundcard, NULL);

  return(sound_provider_interface->get_default_soundcard(sound_provider));
}

/**
 * ags_sound_provider_set_default_soundcard_thread:
 * @sound_provider: the #AgsSoundProvider
 * @soundcard_thread: the default #AgsThread
 * 
 * Set default soundcard thread.
 * 
 * Since: 3.0.0
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
 * Returns: (transfer full): the #AgsThread
 * 
 * Since: 3.0.0
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
 * ags_sound_provider_set_soundcard:
 * @sound_provider: the #AgsSoundProvider
 * @soundcard: (element-type GObject) (transfer full): a #GList-struct containing #AgsSoundcard
 * 
 * Set soundcards.
 *
 * Since: 3.0.0
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
 * Returns: (element-type GObject) (transfer full): a #GList-struct containing #AgsSoundcard
 * 
 * Since: 3.0.0
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
 * ags_sound_provider_set_sequencer:
 * @sound_provider: the #AgsSoundProvider
 * @sequencer: (element-type GObject) (transfer full): a #GList-struct containing #AgsSequencer
 * 
 * Set sequencers.
 *
 * Since: 3.0.0
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
 * Returns: (element-type GObject) (transfer full): a #GList-struct containing #AgsSequencer
 * 
 * Since: 3.0.0
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
 * ags_sound_provider_set_audio:
 * @sound_provider: the #AgsSoundProvider
 * @audio: (element-type AgsAudio.Audio) (transfer full): a #GList-struct containing #AgsAudio
 * 
 * Set audio objects.
 *
 * Since: 3.0.0
 */
void
ags_sound_provider_set_audio(AgsSoundProvider *sound_provider,
			     GList *audio)
{
  AgsSoundProviderInterface *sound_provider_interface;

  g_return_if_fail(AGS_IS_SOUND_PROVIDER(sound_provider));
  sound_provider_interface = AGS_SOUND_PROVIDER_GET_INTERFACE(sound_provider);
  g_return_if_fail(sound_provider_interface->set_audio);

  sound_provider_interface->set_audio(sound_provider,
				      audio);
}

/**
 * ags_sound_provider_get_audio:
 * @sound_provider: the #AgsSoundProvider
 * 
 * Get audio objects.
 *
 * Returns: (element-type AgsAudio.Audio) (transfer full): a #GList-struct containing #AgsAudio
 * 
 * Since: 3.0.0
 */
GList*
ags_sound_provider_get_audio(AgsSoundProvider *sound_provider)
{
  AgsSoundProviderInterface *sound_provider_interface;

  g_return_val_if_fail(AGS_IS_SOUND_PROVIDER(sound_provider), NULL);
  sound_provider_interface = AGS_SOUND_PROVIDER_GET_INTERFACE(sound_provider);
  g_return_val_if_fail(sound_provider_interface->get_audio, NULL);

  return(sound_provider_interface->get_audio(sound_provider));
}

/**
 * ags_sound_provider_set_sound_server:
 * @sound_provider: the #AgsSoundProvider
 * @sound_server: (element-type GObject) (transfer full): a #GList-struct containing #AgsSoundServer
 *
 * Get sound server.
 * 
 * Since: 3.0.0
 */
void
ags_sound_provider_set_sound_server(AgsSoundProvider *sound_provider,
				    GList *sound_server)
{
  AgsSoundProviderInterface *sound_provider_interface;

  g_return_if_fail(AGS_IS_SOUND_PROVIDER(sound_provider));
  sound_provider_interface = AGS_SOUND_PROVIDER_GET_INTERFACE(sound_provider);
  g_return_if_fail(sound_provider_interface->set_sound_server);

  sound_provider_interface->set_sound_server(sound_provider,
					     sound_server);
}

/**
 * ags_sound_provider_get_sound_server:
 * @sound_provider: the #AgsSoundProvider
 *
 * Set sound server.
 * 
 * Returns: (element-type GObject) (transfer full): a #GList-struct containing #AgsSoundServer
 *
 * Since: 3.0.0
 */
GList*
ags_sound_provider_get_sound_server(AgsSoundProvider *sound_provider)
{
  AgsSoundProviderInterface *sound_provider_interface;

  g_return_val_if_fail(AGS_IS_SOUND_PROVIDER(sound_provider), NULL);
  sound_provider_interface = AGS_SOUND_PROVIDER_GET_INTERFACE(sound_provider);
  g_return_val_if_fail(sound_provider_interface->get_sound_server, NULL);

  return(sound_provider_interface->get_sound_server(sound_provider));
}

/**
 * ags_sound_provider_set_osc_server:
 * @sound_provider: the #AgsSoundProvider
 * @osc_server: (element-type GObject) (transfer full): a #GList-struct containing #AgsOscServer
 *
 * Set OSC server.
 * 
 * Since: 3.0.0
 */
void
ags_sound_provider_set_osc_server(AgsSoundProvider *sound_provider,
				  GList *osc_server)
{
  AgsSoundProviderInterface *sound_provider_interface;

  g_return_if_fail(AGS_IS_SOUND_PROVIDER(sound_provider));
  sound_provider_interface = AGS_SOUND_PROVIDER_GET_INTERFACE(sound_provider);
  g_return_if_fail(sound_provider_interface->set_osc_server);

  sound_provider_interface->set_osc_server(sound_provider,
					   osc_server);
}

/**
 * ags_sound_provider_get_osc_server:
 * @sound_provider: the #AgsSoundProvider
 *
 * Set distributed manager.
 * 
 * Returns: (element-type GObject) (transfer full): a #GList-struct containing #AgsOscServer
 *
 * Since: 3.0.0
 */
GList*
ags_sound_provider_get_osc_server(AgsSoundProvider *sound_provider)
{
  AgsSoundProviderInterface *sound_provider_interface;

  g_return_val_if_fail(AGS_IS_SOUND_PROVIDER(sound_provider), NULL);
  sound_provider_interface = AGS_SOUND_PROVIDER_GET_INTERFACE(sound_provider);
  g_return_val_if_fail(sound_provider_interface->get_osc_server, NULL);

  return(sound_provider_interface->get_osc_server(sound_provider));
}
