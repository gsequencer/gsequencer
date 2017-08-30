/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/server/ags_service_provider.h>

#include <ags/server/ags_server.h>

#include <math.h>

void ags_service_provider_class_init(AgsServiceProviderInterface *interface);

GType
ags_service_provider_get_type()
{
  static GType ags_type_service_provider = 0;

  if(!ags_type_service_provider){
    ags_type_service_provider = g_type_register_static_simple(G_TYPE_INTERFACE,
							      "AgsServiceProvider\0",
							      sizeof(AgsServiceProviderInterface),
							      (GClassInitFunc) ags_service_provider_class_init,
							      0, NULL, 0);
  }

  return(ags_type_service_provider);
}

void
ags_service_provider_class_init(AgsServiceProviderInterface *interface)
{
  /* empty */
}

/**
 * ags_service_provider_is_operating:
 * @service_provider: the #AgsServiceProvider
 * 
 * Check if is operating.
 *
 * Returns: %TRUE if operating, otherwise %FALSE
 *
 * Since: 1.0.0
 */
gboolean
is_operating(AgsServiceProvider *service_provider)
{
  AgsServiceProviderInterface *service_provider_interface;

  g_return_val_if_fail(AGS_IS_SERVICE_PROVIDER(service_provider), NULL);
  service_provider_interface = AGS_SERVICE_PROVIDER_GET_INTERFACE(service_provider);
  g_return_val_if_fail(service_provider_interface->is_operating, NULL);

  return(service_provider_interface->is_operating(service_provider));
}

/**
 * ags_service_provider_server_status:
 * @service_provider: the #AgsServiceProvider
 * 
 * Get server status.
 *
 * Returns: the #AgsServerStatus
 *
 * Since: 1.0.0
 */
AgsServerStatus*
ags_service_provider_server_status(AgsServiceProvider *service_provider)
{
  AgsServiceProviderInterface *service_provider_interface;

  g_return_val_if_fail(AGS_IS_SERVICE_PROVIDER(service_provider), NULL);
  service_provider_interface = AGS_SERVICE_PROVIDER_GET_INTERFACE(service_provider);
  g_return_val_if_fail(service_provider_interface->server_status, NULL);

  return(service_provider_interface->server_status(service_provider));
}

/**
 * ags_service_provider_get_env:
 * @service_provider: the #AgsServiceProvider
 * 
 * Get xmlrpc_env.
 *
 * Returns: the xmlrpc_env
 *
 * Since: 1.0.0
 */
gpointer
ags_service_provider_get_env(AgsServiceProvider *service_provider)
{
  AgsServiceProviderInterface *service_provider_interface;

  g_return_val_if_fail(AGS_IS_SERVICE_PROVIDER(service_provider), NULL);
  service_provider_interface = AGS_SERVICE_PROVIDER_GET_INTERFACE(service_provider);
  g_return_val_if_fail(service_provider_interface->get_env, NULL);

  return(service_provider_interface->get_env(service_provider));
}

/**
 * ags_service_provider_set_registry:
 * @service_provider: the #AgsServiceProvider
 * @registry: the #AgsRegistry
 * 
 * Set registry.
 *
 * Since: 1.0.0
 */
void
ags_service_provider_set_registry(AgsServiceProvider *service_provider,
				  GObject *registry)
{
  AgsServiceProviderInterface *service_provider_interface;

  g_return_if_fail(AGS_IS_SERVICE_PROVIDER(service_provider));
  service_provider_interface = AGS_SERVICE_PROVIDER_GET_INTERFACE(service_provider);
  g_return_if_fail(service_provider_interface->set_registry);

  service_provider_interface->set_registry(service_provider,
					   registry);
}

/**
 * ags_service_provider_get_registry:
 * @service_provider: the #AgsServiceProvider
 * 
 * Get registry.
 *
 * Returns: the #AgsRegistry
 *
 * Since: 1.0.0
 */
GObject*
ags_service_provider_get_registry(AgsServiceProvider *service_provider)
{
  AgsServiceProviderInterface *service_provider_interface;

  g_return_val_if_fail(AGS_IS_SERVICE_PROVIDER(service_provider), NULL);
  service_provider_interface = AGS_SERVICE_PROVIDER_GET_INTERFACE(service_provider);
  g_return_val_if_fail(service_provider_interface->get_registry, NULL);

  return(service_provider_interface->get_registry(service_provider));
}

/**
 * ags_service_provider_set_server:
 * @service_provider: the #AgsServiceProvider
 * @server: the #GList-struct containing #AgsServer
 * 
 * Set server.
 *
 * Since: 1.0.0
 */
void
ags_service_provider_set_server(AgsServiceProvider *service_provider,
				GList *server)
{
  AgsServiceProviderInterface *service_provider_interface;

  g_return_if_fail(AGS_IS_SERVICE_PROVIDER(service_provider));
  service_provider_interface = AGS_SERVICE_PROVIDER_GET_INTERFACE(service_provider);
  g_return_if_fail(service_provider_interface->set_server);

  service_provider_interface->set_server(service_provider,
					 server);
}

/**
 * ags_service_provider_get_server:
 * @service_provider: the #AgsServiceProvider
 * 
 * Get server.
 *
 * Returns: the #GList-struct containing #AgsServer
 *
 * Since: 1.0.0
 */
GList*
ags_service_provider_get_server(AgsServiceProvider *service_provider)
{
  AgsServiceProviderInterface *service_provider_interface;

  g_return_val_if_fail(AGS_IS_SERVICE_PROVIDER(service_provider), NULL);
  service_provider_interface = AGS_SERVICE_PROVIDER_GET_INTERFACE(service_provider);
  g_return_val_if_fail(service_provider_interface->get_server, NULL);

  return(service_provider_interface->get_server(service_provider));
}

/**
 * ags_service_provider_set_certificate_manager:
 * @service_provider: the #AgsServiceProvider
 * @certificate_manager: the #AgsCertificateManager
 * 
 * Set certificate manager.
 *
 * Since: 1.0.0
 */
void
ags_service_provider_set_certificate_manager(AgsServiceProvider *service_provider,
					     AgsCertificateManager *certificate_manager)
{
  AgsServiceProviderInterface *service_provider_interface;

  g_return_if_fail(AGS_IS_SERVICE_PROVIDER(service_provider));
  service_provider_interface = AGS_SERVICE_PROVIDER_GET_INTERFACE(service_provider);
  g_return_if_fail(service_provider_interface->set_certificate_manager);

  service_provider_interface->set_certificate_manager(service_provider,
						      certificate_manager);
}

/**
 * ags_service_provider_get_certificate_manager:
 * @service_provider: the #AgsServiceProvider
 * 
 * Get certificate manager.
 *
 * Returns: the #AgsCertificateManager
 *
 * Since: 1.0.0
 */
AgsCertificateManager*
ags_service_provider_get_certificate_manager(AgsServiceProvider *service_provider)
{
  AgsServiceProviderInterface *service_provider_interface;

  g_return_val_if_fail(AGS_IS_SERVICE_PROVIDER(service_provider), NULL);
  service_provider_interface = AGS_SERVICE_PROVIDER_GET_INTERFACE(service_provider);
  g_return_val_if_fail(service_provider_interface->get_certificate_manager, NULL);

  return(service_provider_interface->get_certificate_manager(service_provider));
}

/**
 * ags_service_provider_set_password_store_manager:
 * @service_provider: the #AgsServiceProvider
 * @password_store_manager: the #AgsPasswordStoreManager
 * 
 * Set password store manager.
 *
 * Since: 1.0.0
 */
void
ags_service_provider_set_password_store_manager(AgsServiceProvider *service_provider,
						AgsPasswordStoreManager *password_store_manager)
{
  AgsServiceProviderInterface *service_provider_interface;

  g_return_if_fail(AGS_IS_SERVICE_PROVIDER(service_provider));
  service_provider_interface = AGS_SERVICE_PROVIDER_GET_INTERFACE(service_provider);
  g_return_if_fail(service_provider_interface->set_password_store_manager);

  service_provider_interface->set_password_store_manager(service_provider,
							 password_store_manager);
}

/**
 * ags_service_provider_get_password_store_manager:
 * @service_provider: the #AgsServiceProvider
 * 
 * Get password store manager.
 *
 * Returns: the #AgsPasswordStoreManager
 *
 * Since: 1.0.0
 */
AgsPasswordStoreManager*
ags_service_provider_get_password_store_manager(AgsServiceProvider *service_provider)
{
  AgsServiceProviderInterface *service_provider_interface;

  g_return_val_if_fail(AGS_IS_SERVICE_PROVIDER(service_provider), NULL);
  service_provider_interface = AGS_SERVICE_PROVIDER_GET_INTERFACE(service_provider);
  g_return_val_if_fail(service_provider_interface->get_password_store_manager, NULL);

  return(service_provider_interface->get_password_store_manager(service_provider));
}

/**
 * ags_service_provider_set_authentication_manager:
 * @service_provider: the #AgsServiceProvider
 * @authentication_manager: the #AgsAuthenticationManager
 * 
 * Set authentication manager.
 *
 * Since: 1.0.0
 */
void
ags_service_provider_set_authentication_manager(AgsServiceProvider *service_provider,
						AgsAuthenticationManager *authentication_manager)
{
  AgsServiceProviderInterface *service_provider_interface;

  g_return_if_fail(AGS_IS_SERVICE_PROVIDER(service_provider));
  service_provider_interface = AGS_SERVICE_PROVIDER_GET_INTERFACE(service_provider);
  g_return_if_fail(service_provider_interface->set_authentication_manager);

  service_provider_interface->set_authentication_manager(service_provider,
							 authentication_manager);
}

/**
 * ags_service_provider_get_authentication_manager:
 * @service_provider: the #AgsServiceProvider
 * 
 * Get authentication manager.
 *
 * Returns: the #AgsAuthenticationManager
 *
 * Since: 1.0.0
 */
AgsAuthenticationManager*
ags_service_provider_get_authentication_manager(AgsServiceProvider *service_provider)
{
  AgsServiceProviderInterface *service_provider_interface;

  g_return_val_if_fail(AGS_IS_SERVICE_PROVIDER(service_provider), NULL);
  service_provider_interface = AGS_SERVICE_PROVIDER_GET_INTERFACE(service_provider);
  g_return_val_if_fail(service_provider_interface->get_authentication_manager, NULL);

  return(service_provider_interface->get_authentication_manager(service_provider));
}
