/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/object/ags_sound_server.h>

void ags_sound_server_class_init(AgsSoundServerInterface *interface);

/**
 * SECTION:ags_sound_server
 * @short_description: access sound servers
 * @title: AgsSoundServer
 * @section_id:
 * @include: ags/object/ags_sound_server.h
 *
 * The #AgsSoundServer interface gives you a unique access sound servers.
 */

GType
ags_sound_server_get_type()
{
  static GType ags_type_sound_server = 0;

  if(!ags_type_sound_server){
    ags_type_sound_server = g_type_register_static_simple(G_TYPE_INTERFACE,
							  "AgsSoundServer",
							  sizeof(AgsSoundServerInterface),
							  (GClassInitFunc) ags_sound_server_class_init,
							  0, NULL, 0);
  }

  return(ags_type_sound_server);
}

void
ags_sound_server_class_init(AgsSoundServerInterface *interface)
{
  /* empty */
}

/**
 * ags_sound_server_set_url:
 * @sound_server: The #AgsSoundServer
 * @url: the url to set
 *
 * Sets the url of @sound_server.
 *
 * Since: 2.0.0
 */
void
ags_sound_server_set_url(AgsSoundServer *sound_server,
			 gchar *url)
{
  AgsSoundServerInterface *sound_server_interface;

  g_return_if_fail(AGS_IS_SOUND_SERVER(sound_server));
  sound_server_interface = AGS_SOUND_SERVER_GET_INTERFACE(sound_server);
  g_return_if_fail(sound_server_interface->set_url);
  
  sound_server_interface->set_url(sound_server,
				  url);
}

/**
 * ags_sound_server_get_url:
 * @sound_server: the #AgsSoundServer
 *
 * Gets the URL of @sound_server.
 *
 * Returns: the URL as string
 *
 * Since: 2.0.0
 */
gchar*
ags_sound_server_get_url(AgsSoundServer *sound_server)
{
  AgsSoundServerInterface *sound_server_interface;

  g_return_val_if_fail(AGS_IS_SOUND_SERVER(sound_server), NULL);
  sound_server_interface = AGS_SOUND_SERVER_GET_INTERFACE(sound_server);
  g_return_val_if_fail(sound_server_interface->get_url, NULL);

  return(sound_server_interface->get_url(sound_server));
}

/**
 * ags_sound_server_set_port:
 * @sound_server: The #AgsSoundServer
 * @port: the ports to set
 * @port_count: the number of ports
 *
 * Sets the ports of @sound_server.
 *
 * Since: 2.0.0
 */
void
ags_sound_server_set_ports(AgsSoundServer *sound_server,
			   guint *port, guint port_count)
{
  AgsSoundServerInterface *sound_server_interface;

  g_return_if_fail(AGS_IS_SOUND_SERVER(sound_server));
  sound_server_interface = AGS_SOUND_SERVER_GET_INTERFACE(sound_server);
  g_return_if_fail(sound_server_interface->set_ports);
  
  sound_server_interface->set_ports(sound_server,
				    port, port_count);
}

/**
 * ags_sound_server_get_ports:
 * @sound_server: the #AgsSoundServer
 * @port_count: the number of ports returned
 *
 * Gets the ports of @sound_server.
 *
 * Returns: the port as string %NULL-terminated array
 *
 * Since: 2.0.0
 */
guint*
ags_sound_server_get_ports(AgsSoundServer *sound_server,
			   guint *port_count)
{
  AgsSoundServerInterface *sound_server_interface;

  g_return_val_if_fail(AGS_IS_SOUND_SERVER(sound_server), NULL);
  sound_server_interface = AGS_SOUND_SERVER_GET_INTERFACE(sound_server);
  g_return_val_if_fail(sound_server_interface->get_ports, NULL);

  return(sound_server_interface->get_ports(sound_server,
					   port_count));
}

/**
 * ags_sound_server_set_soundcard:
 * @sound_server: The #AgsSoundServer
 * @client_uuid: the location to fetch from
 * @soundcard: the soundcard to set
 *
 * Sets the soundcard at @client_uuid.
 *
 * Since: 2.0.0
 */
void
ags_sound_server_set_soundcard(AgsSoundServer *sound_server,
			       gchar *client_uuid,
			       GList *soundcard)
{
  AgsSoundServerInterface *sound_server_interface;

  g_return_if_fail(AGS_IS_SOUND_SERVER(sound_server));
  sound_server_interface = AGS_SOUND_SERVER_GET_INTERFACE(sound_server);
  g_return_if_fail(sound_server_interface->set_soundcard);
  
  sound_server_interface->set_soundcard(sound_server,
					client_uuid,
					soundcard);
}

/**
 * ags_sound_server_get_soundcard:
 * @client_uuid: the client uuid
 * @sound_server: the #AgsSoundServer
 *
 * Gets the soundcard of @sound_server associated with @client_uuid.
 *
 * Returns: the soundcard as #GList-struct
 *
 * Since: 2.0.0
 */
GList*
ags_sound_server_get_soundcard(AgsSoundServer *sound_server,
			       gchar *client_uuid)
{
  AgsSoundServerInterface *sound_server_interface;

  g_return_val_if_fail(AGS_IS_SOUND_SERVER(sound_server), NULL);
  sound_server_interface = AGS_SOUND_SERVER_GET_INTERFACE(sound_server);
  g_return_val_if_fail(sound_server_interface->get_soundcard, NULL);

  return(sound_server_interface->get_soundcard(sound_server,
					       client_uuid));
}

/**
 * ags_sound_server_set_sequencer:
 * @sound_server: The #AgsSoundServer
 * @client_uuid: the location to fetch from
 * @sequencer: the sequencer to set
 *
 * Sets the sequencer at @client_uuid.
 *
 * Since: 2.0.0
 */
void
ags_sound_server_set_sequencer(AgsSoundServer *sound_server,
			       gchar *client_uuid,
			       GList *sequencer)
{
  AgsSoundServerInterface *sound_server_interface;

  g_return_if_fail(AGS_IS_SOUND_SERVER(sound_server));
  sound_server_interface = AGS_SOUND_SERVER_GET_INTERFACE(sound_server);
  g_return_if_fail(sound_server_interface->set_sequencer);
  
  sound_server_interface->set_sequencer(sound_server,
					client_uuid,
					sequencer);
}

/**
 * ags_sound_server_get_sequencer:
 * @client_uuid: the client uuid
 * @sound_server: the #AgsSoundServer
 *
 * Gets the sequencer of @sound_server associated with @client_uuid.
 *
 * Returns: the sequencer as #GList-struct
 *
 * Since: 2.0.0
 */
GList*
ags_sound_server_get_sequencer(AgsSoundServer *sound_server,
			       gchar *client_uuid)
{
  AgsSoundServerInterface *sound_server_interface;

  g_return_val_if_fail(AGS_IS_SOUND_SERVER(sound_server), NULL);
  sound_server_interface = AGS_SOUND_SERVER_GET_INTERFACE(sound_server);
  g_return_val_if_fail(sound_server_interface->get_sequencer, NULL);

  return(sound_server_interface->get_sequencer(sound_server,
					       client_uuid));
}

/**
 * ags_sound_server_register_soundcard:
 * @sound_server: The #AgsSoundServer
 * @is_output: if %TRUE the used as sink, else as source
 *
 * Fetches @soundcard of @sound_server.
 *
 * Returns: a new #AgsSoundcard
 *
 * Since: 2.0.0
 */
GObject*
ags_sound_server_register_soundcard(AgsSoundServer *sound_server,
				    gboolean is_output)
{
  AgsSoundServerInterface *sound_server_interface;

  g_return_val_if_fail(AGS_IS_SOUND_SERVER(sound_server), NULL);
  sound_server_interface = AGS_SOUND_SERVER_GET_INTERFACE(sound_server);
  g_return_val_if_fail(sound_server_interface->register_soundcard, NULL);

  return(sound_server_interface->register_soundcard(sound_server,
						    is_output));
}

/**
 * ags_sound_server_unregister_soundcard:
 * @sound_server: The #AgsSoundServer
 * @soundcard: the #AgsSoundcard
 *
 * Releases @soundcard in @sound_server.
 *
 * Since: 2.0.0
 */
void
ags_sound_server_unregister_soundcard(AgsSoundServer *sound_server,
				      GObject *soundcard)
{
  AgsSoundServerInterface *sound_server_interface;

  g_return_if_fail(AGS_IS_SOUND_SERVER(sound_server));
  sound_server_interface = AGS_SOUND_SERVER_GET_INTERFACE(sound_server);
  g_return_if_fail(sound_server_interface->unregister_soundcard);
  
  sound_server_interface->unregister_soundcard(sound_server,
					       soundcard);
}

/**
 * ags_sound_server_register_sequencer:
 * @sound_server: The #AgsSoundServer
 * @is_output: if %TRUE the used as sink, else as source
 *
 * Fetches @sequencer of @sound_server.
 *
 * Returns: a new #AgsSequencer
 *
 * Since: 2.0.0
 */
GObject*
ags_sound_server_register_sequencer(AgsSoundServer *sound_server,
				    gboolean is_output)
{
  AgsSoundServerInterface *sound_server_interface;

  g_return_val_if_fail(AGS_IS_SOUND_SERVER(sound_server), NULL);
  sound_server_interface = AGS_SOUND_SERVER_GET_INTERFACE(sound_server);
  g_return_val_if_fail(sound_server_interface->register_sequencer, NULL);

  return(sound_server_interface->register_sequencer(sound_server,
						    is_output));
}

/**
 * ags_sound_server_unregister_sequencer:
 * @sound_server: The #AgsSoundServer
 * @sequencer: the #AgsSequencer
 *
 * Releases @sequencer in @sound_server.
 *
 * Since: 2.0.0
 */
void
ags_sound_server_unregister_sequencer(AgsSoundServer *sound_server,
				      GObject *sequencer)
{
  AgsSoundServerInterface *sound_server_interface;

  g_return_if_fail(AGS_IS_SOUND_SERVER(sound_server));
  sound_server_interface = AGS_SOUND_SERVER_GET_INTERFACE(sound_server);
  g_return_if_fail(sound_server_interface->unregister_sequencer);
  
  sound_server_interface->unregister_sequencer(sound_server,
					       sequencer);
}
