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

#include <ags/server/ags_password_store.h>

void ags_password_store_base_init(AgsPasswordStoreInterface *interface);

/**
 * SECTION:ags_password_store
 * @short_description: base password authentication
 * @title: AgsPasswordStore
 * @section_id: AgsPasswordStore
 * @include: ags/server/ags_password_store.h
 *
 * The #AgsPasswordStore interface gives you a unique access to all objects
 * and is responsible to set up signal handlers.
 */

GType
ags_password_store_get_type()
{
  static GType ags_type_password_store = 0;

  if(!ags_type_password_store){
    static const GTypeInfo ags_password_store_info = {
      sizeof(AgsPasswordStoreInterface),
      (GBaseInitFunc) ags_password_store_base_init,
      NULL, /* base_finalize */
    };

    ags_type_password_store = g_type_register_static(G_TYPE_INTERFACE,
						     "AgsPasswordStore\0", &ags_password_store_info,
						     0);
  }

  return(ags_type_password_store);
}

void
ags_password_store_base_init(AgsPasswordStoreInterface *interface)
{
  /* empty */
}

/**
 * ags_password_store_get_login_name:
 * @password_store: the #AgsPasswordStore
 * @user: the users unique identifier
 * @security_token: the security token
 * @security_context: the security context
 * @error: the #GError-struct
 *
 * Get login name.
 * 
 * Returns: the login name
 * 
 * Since: 1.0.0
 */
gchar*
ags_password_store_get_login_name(AgsPasswordStore *password_store,
				  gchar *user,
				  gchar *security_token,
				  guint security_context,
				  GError **error)
{
  AgsPasswordStoreInterface *password_store_interface;

  g_return_val_if_fail(AGS_IS_PASSWORD_STORE(password_store), NULL);
  password_store_interface = AGS_PASSWORD_STORE_GET_INTERFACE(password_store);
  g_return_val_if_fail(password_store_interface->get_login_name, NULL);

  return(password_store_interface->get_login_name(password_store,
						  user,
						  security_token,
						  security_context,
						  error));
}

/**
 * ags_password_store_set_login_name:
 * @password_store: the #AgsPasswordStore
 * @user: the users unique identifier
 * @security_token: the security token
 * @security_context: the security context
 * @login_name: the login name
 * @error: the #GError-struct
 *
 * Set login name.
 * 
 * Since: 1.0.0
 */
void
ags_password_store_set_login_name(AgsPasswordStore *password_store,
				  gchar *user,
				  gchar *security_token,
				  guint security_context,
				  gchar *login_name,
				  GError **error)
{
  AgsPasswordStoreInterface *password_store_interface;

  g_return_if_fail(AGS_IS_PASSWORD_STORE(password_store));
  password_store_interface = AGS_PASSWORD_STORE_GET_INTERFACE(password_store);
  g_return_if_fail(password_store_interface->set_login_name);
  password_store_interface->set_login_name(password_store,
					   user,
					   security_tocken,
					   security_context,
					   login_name,
					   error);
}

/**
 * ags_password_store_get_password:
 * @password_store: the #AgsPasswordStore
 * @user: the users unique identifier
 * @security_token: the security token
 * @security_context: the security context
 * @error: the #GError-struct
 *
 * Get login name.
 * 
 * Returns: the password
 * 
 * Since: 1.0.0
 */
gchar*
ags_password_store_get_password(AgsPasswordStore *password_store,
				gchar *user,
				gchar *security_token,
				guint security_context,
				GError **error)
{
  AgsPasswordStoreInterface *password_store_interface;

  g_return_val_if_fail(AGS_IS_PASSWORD_STORE(password_store), NULL);
  password_store_interface = AGS_PASSWORD_STORE_GET_INTERFACE(password_store);
  g_return_val_if_fail(password_store_interface->get_password, NULL);

  return(password_store_interface->get_password(password_store,
						user,
						security_token,
						security_context,
						error));
}

/**
 * ags_password_store_set_password:
 * @password_store: the #AgsPasswordStore
 * @user: the users unique identifier
 * @security_token: the security token
 * @security_context: the security context
 * @password: the password
 * @error: the #GError-struct
 *
 * Set login name.
 * 
 * Since: 1.0.0
 */
void
ags_password_store_set_password(AgsPasswordStore *password_store,
				gchar *user,
				gchar *security_token,
				guint security_context,
				gchar *password,
				GError **error)
{
  AgsPasswordStoreInterface *password_store_interface;

  g_return_if_fail(AGS_IS_PASSWORD_STORE(password_store));
  password_store_interface = AGS_PASSWORD_STORE_GET_INTERFACE(password_store);
  g_return_if_fail(password_store_interface->set_password);
  password_store_interface->set_password(password_store,
					 user,
					 security_tocken,
					 security_context,
					 password,
					 error);
}

/**
 * ags_password_store_encrypt_password:
 * @password_store: the #AgsPasswordStore
 * @password: the password
 * @error: the #GError-struct
 *
 * Encrypt password.
 *
 * Returns: the encrypted bytes
 *
 * Since: 1.0.0
 */
char*
ags_password_store_encrypt_password(AgsPasswordStore *password_store,
				    gchar *password,
				    GError **error)
{
  AgsPasswordStoreInterface *password_store_interface;

  g_return_val_if_fail(AGS_IS_PASSWORD_STORE(password_store), NULL);
  password_store_interface = AGS_PASSWORD_STORE_GET_INTERFACE(password_store);
  g_return_val_if_fail(password_store_interface->encrypt_password, NULL);

  return(password_store_interface->encrypt_password(password_store,
						    password,
						    error));
}

/**
 * ags_password_store_decrypt_password:
 * @password_store: the #AgsPasswordStore
 * @password: the password
 * @error: the #GError-struct
 *
 * Decrypt password.
 *
 * Returns: the plain text password
 *
 * Since: 1.0.0
 */
gchar*
ags_password_store_decrypt_password(AgsPasswordStore *password_store,
				    char *data,
				    GError **error)
{
  AgsPasswordStoreInterface *password_store_interface;

  g_return_val_if_fail(AGS_IS_PASSWORD_STORE(password_store), NULL);
  password_store_interface = AGS_PASSWORD_STORE_GET_INTERFACE(password_store);
  g_return_val_if_fail(password_store_interface->decrypt_password, NULL);

  return(password_store_interface->decrypt_password(password_store,
						    data,
						    error));
}
