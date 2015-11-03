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

#include <ags/object/ags_distributed_manager.h>

void ags_distributed_manager_class_init(AgsDistributedManagerInterface *interface);

GType
ags_distributed_manager_get_type()
{
  static GType ags_type_distributed_manager = 0;

  if(!ags_type_distributed_manager){
    ags_type_distributed_manager = g_type_register_static_simple(G_TYPE_INTERFACE,
								 "AgsDistributedManager\0",
								 sizeof(AgsDistributedManagerInterface),
								 (GClassInitFunc) ags_distributed_manager_class_init,
								 0, NULL, 0);
  }

  return(ags_type_distributed_manager);
}

void
ags_distributed_manager_class_init(AgsDistributedManagerInterface *interface)
{
  /* empty */
}

/**
 * ags_distributed_manager_set_url:
 * @distributed_manager: The #AgsDistributedManager
 * @url: the url to set
 *
 * Sets the url of @distributed_manager.
 *
 * Since: 0.7.0
 */
void
ags_distributed_manager_set_url(AgsDistributedManager *distributed_manager,
				gchar *url)
{
  AgsDistributedManagerInterface *distributed_manager_interface;

  g_return_if_fail(AGS_IS_DISTRIBUTED_MANAGER(distributed_manager));
  distributed_manager_interface = AGS_DISTRIBUTED_MANAGER_GET_INTERFACE(distributed_manager);
  g_return_if_fail(distributed_manager_interface->set_url);
  
  distributed_manager_interface->set_url(distributed_manager,
					 url);
}

/**
 * ags_distributed_manager_get_url:
 * @distributed_manager: the #AgsDistributedManager
 *
 * Gets the URL of @distributed_manager.
 *
 * Returns: the URL as string
 *
 * Since: 0.7.0
 */
gchar*
ags_distributed_manager_get_url(AgsDistributedManager *distributed_manager)
{
  AgsDistributedManagerInterface *distributed_manager_interface;

  g_return_val_if_fail(AGS_IS_DISTRIBUTED_MANAGER(distributed_manager), NULL);
  distributed_manager_interface = AGS_DISTRIBUTED_MANAGER_GET_INTERFACE(distributed_manager);
  g_return_val_if_fail(distributed_manager_interface->get_url, NULL);

  return(distributed_manager_interface->get_url(distributed_manager));
}

/**
 * ags_distributed_manager_set_ports:
 * @distributed_manager: The #AgsDistributedManager
 * @ports: the ports to set
 *
 * Sets the ports of @distributed_manager.
 *
 * Since: 0.7.0
 */
void
ags_distributed_manager_set_ports(AgsDistributedManager *distributed_manager,
				  gchar **ports)
{
  AgsDistributedManagerInterface *distributed_manager_interface;

  g_return_if_fail(AGS_IS_DISTRIBUTED_MANAGER(distributed_manager));
  distributed_manager_interface = AGS_DISTRIBUTED_MANAGER_GET_INTERFACE(distributed_manager);
  g_return_if_fail(distributed_manager_interface->set_ports);
  
  distributed_manager_interface->set_ports(distributed_manager,
					   ports);
}

/**
 * ags_distributed_manager_get_ports:
 * @distributed_manager: the #AgsDistributedManager
 *
 * Gets the ports of @distributed_manager.
 *
 * Returns: the port as string %NULL-terminated array
 *
 * Since: 0.7.0
 */
gchar**
ags_distributed_manager_get_ports(AgsDistributedManager *distributed_manager)
{
  AgsDistributedManagerInterface *distributed_manager_interface;

  g_return_val_if_fail(AGS_IS_DISTRIBUTED_MANAGER(distributed_manager), NULL);
  distributed_manager_interface = AGS_DISTRIBUTED_MANAGER_GET_INTERFACE(distributed_manager);
  g_return_val_if_fail(distributed_manager_interface->get_ports, NULL);

  return(distributed_manager_interface->get_ports(distributed_manager));
}

/**
 * ags_distributed_manager_set_soundcard:
 * @distributed_manager: The #AgsDistributedManager
 * @uri: the location to fetch from
 * @soundcard: the soundcard to set
 *
 * Sets the soundcard at @uri.
 *
 * Since: 0.7.0
 */
void
ags_distributed_manager_set_soundcard(AgsDistributedManager *distributed_manager,
				      gchar *uri,
				      GObject *soundcard)
{
  AgsDistributedManagerInterface *distributed_manager_interface;

  g_return_if_fail(AGS_IS_DISTRIBUTED_MANAGER(distributed_manager));
  distributed_manager_interface = AGS_DISTRIBUTED_MANAGER_GET_INTERFACE(distributed_manager);
  g_return_if_fail(distributed_manager_interface->set_soundcard);
  
  distributed_manager_interface->set_soundcard(distributed_manager,
					       uri,
					       soundcard);
}

/**
 * ags_distributed_manager_get_soundcard:
 * @uri: the URI
 * @distributed_manager: the #AgsDistributedManager
 *
 * Gets the soundcard of @distributed_manager associated with @uri.
 *
 * Returns: the soundcard as #GObject
 *
 * Since: 0.7.0
 */
GObject*
ags_distributed_manager_get_soundcard(AgsDistributedManager *distributed_manager,
				      gchar *uri)
{
  AgsDistributedManagerInterface *distributed_manager_interface;

  g_return_val_if_fail(AGS_IS_DISTRIBUTED_MANAGER(distributed_manager), NULL);
  distributed_manager_interface = AGS_DISTRIBUTED_MANAGER_GET_INTERFACE(distributed_manager);
  g_return_val_if_fail(distributed_manager_interface->get_soundcard, NULL);

  return(distributed_manager_interface->get_soundcard(distributed_manager,
						      uri));
}

/**
 * ags_distributed_manager_set_sequencer:
 * @distributed_manager: The #AgsDistributedManager
 * @uri: the location to fetch from
 * @sequencer: the sequencer to set
 *
 * Sets the sequencer at @uri.
 *
 * Since: 0.7.0
 */
void
ags_distributed_manager_set_sequencer(AgsDistributedManager *distributed_manager,
				      gchar *uri,
				      GObject *sequencer)
{
  AgsDistributedManagerInterface *distributed_manager_interface;

  g_return_if_fail(AGS_IS_DISTRIBUTED_MANAGER(distributed_manager));
  distributed_manager_interface = AGS_DISTRIBUTED_MANAGER_GET_INTERFACE(distributed_manager);
  g_return_if_fail(distributed_manager_interface->set_sequencer);
  
  distributed_manager_interface->set_sequencer(distributed_manager,
					       uri,
					       sequencer);
}

/**
 * ags_distributed_manager_get_sequencer:
 * @uri: the URI
 * @distributed_manager: the #AgsDistributedManager
 *
 * Gets the sequencer of @distributed_manager associated with @uri.
 *
 * Returns: the sequencer as #GObject
 *
 * Since: 0.7.0
 */
GObject*
ags_distributed_manager_get_sequencer(AgsDistributedManager *distributed_manager,
				      gchar *uri)
{
  AgsDistributedManagerInterface *distributed_manager_interface;

  g_return_val_if_fail(AGS_IS_DISTRIBUTED_MANAGER(distributed_manager), NULL);
  distributed_manager_interface = AGS_DISTRIBUTED_MANAGER_GET_INTERFACE(distributed_manager);
  g_return_val_if_fail(distributed_manager_interface->get_sequencer, NULL);

  return(distributed_manager_interface->get_sequencer(distributed_manager,
						      uri));
}

