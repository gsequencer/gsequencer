/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

void ags_authentication_base_init(AgsAuthenticationInterface *ginterface);

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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_authentication = 0;

    static const GTypeInfo ags_authentication_info = {
      sizeof(AgsAuthenticationInterface),
      (GBaseInitFunc) ags_authentication_base_init,
      NULL, /* base_finalize */
    };

    ags_type_authentication = g_type_register_static(G_TYPE_INTERFACE,
						     "AgsAuthentication", &ags_authentication_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_authentication);
  }

  return g_define_type_id__volatile;
}

void
ags_authentication_base_init(AgsAuthenticationInterface *ginterface)
{
  /* empty */
}

/**
 * ags_authentication_get_authentication_module:
 * @authentication: the #AgsAuthentication
 * 
 * Available authentication modules.
 *
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer full): a %NULL terminated array of strings of available authentication modules
 *
 * Since: 3.0.0
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
 * @user_uuid: (out) (transfer full): return location of the user's uuid
 * @security_token: (out) (transfer full): return location of the security token
 * @error: the #GError-struct
 * 
 * Login.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_authentication_login(AgsAuthentication *authentication,
			 gchar *login,
			 gchar *password,
			 gchar **user_uuid,
			 gchar **security_token,
			 GError **error)
{
  AgsAuthenticationInterface *authentication_interface;

  g_return_val_if_fail(AGS_IS_AUTHENTICATION(authentication), NULL);
  authentication_interface = AGS_AUTHENTICATION_GET_INTERFACE(authentication);
  g_return_val_if_fail(authentication_interface->login, NULL);

  return(authentication_interface->login(authentication,
					 login,
					 password,
					 user_uuid,
					 security_token,
					 error));
}

/**
 * ags_authentication_get_digest:
 * @authentication: the #AgsAuthentication
 * @realm: the realm
 * @login: the login
 * @security_token: the security token
 * @error: the #GError-struct
 * 
 * Get digest of @login.
 *
 * Returns: (transfer full): the encrypted password
 *
 * Since: 3.0.0
 */
gchar*
ags_authentication_get_digest(AgsAuthentication *authentication,
			      gchar *realm,
			      gchar *login,
			      gchar *security_token,
			      GError **error)
{
  AgsAuthenticationInterface *authentication_interface;

  g_return_val_if_fail(AGS_IS_AUTHENTICATION(authentication), NULL);
  authentication_interface = AGS_AUTHENTICATION_GET_INTERFACE(authentication);
  g_return_val_if_fail(authentication_interface->get_digest, NULL);

  return(authentication_interface->get_digest(authentication,
					      realm,
					      login,
					      security_token,
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
 * Since: 3.0.0
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
 * Returns: (transfer full): the generated token
 * 
 * Since: 3.0.0
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
 * ags_authentication_is_session_active:
 * @authentication: the #AgsAuthentication
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the user's UUID
 * @security_token: the security token
 * @error: the #GError-struct
 * 
 * Check session.
 *
 * Returns: %TRUE if session active, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_authentication_is_session_active(AgsAuthentication *authentication,
				     GObject *security_context,
				     gchar *user_uuid,
				     gchar *security_token,
				     GError **error)
{
  AgsAuthenticationInterface *authentication_interface;

  g_return_val_if_fail(AGS_IS_AUTHENTICATION(authentication), FALSE);
  authentication_interface = AGS_AUTHENTICATION_GET_INTERFACE(authentication);
  g_return_val_if_fail(authentication_interface->is_session_active, FALSE);

  return(authentication_interface->is_session_active(authentication,
						     security_context,
						     user_uuid,
						     security_token,
						     error));
}
