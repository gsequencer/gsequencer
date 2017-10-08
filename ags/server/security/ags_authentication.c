/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/server/security/ags_authentication.h>

void ags_authentication_base_init(AgsAuthenticationInterface *interface);

/**
 * SECTION:ags_authentication
 * @short_description: base password authentication
 * @title: AgsAuthentication
 * @section_id:
 * @include: ags/server/security/ags_authentication.h
 *
 * The #AgsAuthentication interface gives you base authentication functions.
 */

GType
ags_authentication_get_type()
{
  static GType ags_type_authentication = 0;

  if(!ags_type_authentication){
    static const GTypeInfo ags_authentication_info = {
      sizeof(AgsAuthenticationInterface),
      (GBaseInitFunc) ags_authentication_base_init,
      NULL, /* base_finalize */
    };

    ags_type_authentication = g_type_register_static(G_TYPE_INTERFACE,
						     "AgsAuthentication\0", &ags_authentication_info,
						     0);
  }

  return(ags_type_authentication);
}

void
ags_authentication_base_init(AgsAuthenticationInterface *interface)
{
  /* empty */
}

/**
 * ags_authentication_get_authentication_module:
 * @authentication: the #AgsAuthentication
 * 
 * Available authentication modules.
 *
 * Returns: a %NULL terminated array of strings of available authentication modules
 *
 * Since: 1.0.0
 */
gchar**
ags_authentication_get_authentication_module(AgsAuthentication *authentication)
{
  AgsAuthenticationInterface *authentication_interface;

  g_return_val_if_fail(AGS_IS_AUTHENTICATION(authentication), NULL);
  authentication_interface = AGS_AUTHENTICATION_GET_INTERFACE(authentication);
  g_return_val_if_fail(authentication_interface->get_authentication_module, NULL);

  return(authentication_interface->get_authentication_module(authentication));
}

/**
 * ags_authentication_login:
 * @authentication: the #AgsAuthentication
 * @login: the login
 * @password: the password
 * @user_uuid: return location of the user's uuid
 * @security_token: return location of the security token
 * @error: the #GError-struct
 * 
 * Login.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 1.0.0
 */
gboolean
ags_authentication_login(AgsAuthentication *authentication,
			 gchar *login, gchar *password,
			 gchar **user_uuid, gchar **security_token,
			 GError **error)
{
  AgsAuthenticationInterface *authentication_interface;

  g_return_val_if_fail(AGS_IS_AUTHENTICATION(authentication), NULL);
  authentication_interface = AGS_AUTHENTICATION_GET_INTERFACE(authentication);
  g_return_val_if_fail(authentication_interface->login, NULL);

  return(authentication_interface->login(authentication,
					 login,
					 password,
					 user_uuid, security_token,
					 error));
}

/**
 * ags_authentication_logout:
 * @authentication: the #AgsAuthentication
 * @security_context: the #AgsSecurityContext
 * @login: the login
 * @security_token: the security token
 * @error: the #GError-struct
 * 
 * Logout.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 1.0.0
 */
gboolean
ags_authentication_logout(AgsAuthentication *authentication,
			  GObject *security_context,
			  gchar *login,
			  gchar *security_token,
			  GError **error)
{
  AgsAuthenticationInterface *authentication_interface;

  g_return_val_if_fail(AGS_IS_AUTHENTICATION(authentication), FALSE);
  authentication_interface = AGS_AUTHENTICATION_GET_INTERFACE(authentication);
  g_return_val_if_fail(authentication_interface->logout, FALSE);

  return(authentication_interface->logout(authentication,
					  security_context,
					  login,
					  security_token,
					  error));
}

/**
 * ags_authentication_generate_token:
 * @authentication: the #AgsAuthentication
 * @error: the #GError-struct
 * 
 * Generate token.
 *
 * Returns: the generated token
 * 
 * Since: 1.0.0
 */
gchar*
ags_authentication_generate_token(AgsAuthentication *authentication,
				  GError **error)
{
  AgsAuthenticationInterface *authentication_interface;

  g_return_val_if_fail(AGS_IS_AUTHENTICATION(authentication), NULL);
  authentication_interface = AGS_AUTHENTICATION_GET_INTERFACE(authentication);
  g_return_val_if_fail(authentication_interface->generate_token, NULL);

  return(authentication_interface->generate_token(authentication,
						  error));
}

/**
 * ags_authentication_get_groups:
 * @authentication: the #AgsAuthentication
 * @security_context: the #AgsSecurityContext
 * @login: the login
 * @security_token: the security token
 * @error: the #GError-struct
 * 
 * Get groups.
 * 
 * Returns: a %NULL-terminated string array containing groups
 * 
 * Since: 1.0.0
 */
gchar**
ags_authentication_get_groups(AgsAuthentication *authentication,
			      GObject *security_context,
			      gchar *login,
			      gchar *security_token,
			      GError **error)
{
  AgsAuthenticationInterface *authentication_interface;

  g_return_val_if_fail(AGS_IS_AUTHENTICATION(authentication), NULL);
  authentication_interface = AGS_AUTHENTICATION_GET_INTERFACE(authentication);
  g_return_val_if_fail(authentication_interface->get_groups, NULL);

  return(authentication_interface->get_groups(authentication,
					      security_context,
					      login,
					      security_token,
					      error));
}

/**
 * ags_authentication_get_permission:
 * @authentication: the #AgsAuthentication
 * @security_context: the #AgsSecurityContext
 * @login: the login
 * @security_token: the security token
 * @group_name: the group name to query
 * @error: the #GError-struct
 * 
 * Get permission of group.
 *
 * Returns: one or more of 'r', 'w' and 'x'.
 * 
 * Since: 1.0.0
 */
gchar*
ags_authentication_get_permission(AgsAuthentication *authentication,
				  GObject *security_context,
				  gchar *login,
				  gchar *security_token,
				  gchar *group_name,
				  GError **error)
{
  AgsAuthenticationInterface *authentication_interface;

  g_return_val_if_fail(AGS_IS_AUTHENTICATION(authentication), NULL);
  authentication_interface = AGS_AUTHENTICATION_GET_INTERFACE(authentication);
  g_return_val_if_fail(authentication_interface->get_permission, NULL);

  return(authentication_interface->get_permission(authentication,
						  security_context,
						  login,
						  security_token,
						  group_name,
						  error));
}

/**
 * ags_authentication_is_session_active:
 * @authentication: the #AgsAuthentication
 * @security_context: the #AgsSecurityContext
 * @login: the login
 * @security_token: the security token
 * @error: the #GError-struct
 * 
 * Check session.
 *
 * Returns: %TRUE if session active, otherwise %FALSE
 *
 * Since: 1.0.0
 */
gboolean
ags_authentication_is_session_active(AgsAuthentication *authentication,
				     GObject *security_context,
				     gchar *login,
				     gchar *security_token,
				     GError **error)
{
  AgsAuthenticationInterface *authentication_interface;

  g_return_val_if_fail(AGS_IS_AUTHENTICATION(authentication), FALSE);
  authentication_interface = AGS_AUTHENTICATION_GET_INTERFACE(authentication);
  g_return_val_if_fail(authentication_interface->is_session_active, FALSE);

  return(authentication_interface->is_session_active(authentication,
						     security_context,
						     login,
						     security_token,
						     error));
}
