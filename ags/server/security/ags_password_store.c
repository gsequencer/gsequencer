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

#include <ags/server/security/ags_password_store.h>

void ags_password_store_base_init(AgsPasswordStoreInterface *ginterface);

/**
 * SECTION:ags_password_store
 * @short_description: password storage
 * @title: AgsPasswordStore
 * @section_id: AgsPasswordStore
 * @include: ags/server/security/ags_password_store.h
 *
 * The #AgsPasswordStore interface gives you password storage.
 */

GType
ags_password_store_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_password_store = 0;

    static const GTypeInfo ags_password_store_info = {
      sizeof(AgsPasswordStoreInterface),
      (GBaseInitFunc) ags_password_store_base_init,
      NULL, /* base_finalize */
    };

    ags_type_password_store = g_type_register_static(G_TYPE_INTERFACE,
						     "AgsPasswordStore", &ags_password_store_info,
						     0);

    g_once_init_leave(&g_define_type_id__static, ags_type_password_store);
  }

  return(g_define_type_id__static);
}

void
ags_password_store_base_init(AgsPasswordStoreInterface *ginterface)
{
  /* empty */
}

/**
 * ags_password_store_set_login_name:
 * @password_store: the #AgsPasswordStore
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the users unique identifier
 * @security_token: the security token
 * @login_name: the login name
 * @error: the #GError-struct
 *
 * Set login name.
 * 
 * Since: 3.0.0
 */
void
ags_password_store_set_login_name(AgsPasswordStore *password_store,
				  GObject *security_context,
				  gchar *user_uuid,
				  gchar *security_token,
				  gchar *login_name,
				  GError **error)
{
  AgsPasswordStoreInterface *password_store_interface;

  g_return_if_fail(AGS_IS_PASSWORD_STORE(password_store));
  password_store_interface = AGS_PASSWORD_STORE_GET_INTERFACE(password_store);
  g_return_if_fail(password_store_interface->set_login_name);
  password_store_interface->set_login_name(password_store,
					   security_context,
					   user_uuid,
					   security_token,
					   login_name,
					   error);
}

/**
 * ags_password_store_get_login_name:
 * @password_store: the #AgsPasswordStore
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the users unique identifier
 * @security_token: the security token
 * @error: the #GError-struct
 *
 * Get login name.
 * 
 * Returns: (transfer full): the login name
 * 
 * Since: 3.0.0
 */
gchar*
ags_password_store_get_login_name(AgsPasswordStore *password_store,
				  GObject *security_context,
				  gchar *user_uuid,
				  gchar *security_token,
				  GError **error)
{
  AgsPasswordStoreInterface *password_store_interface;

  g_return_val_if_fail(AGS_IS_PASSWORD_STORE(password_store), NULL);
  password_store_interface = AGS_PASSWORD_STORE_GET_INTERFACE(password_store);
  g_return_val_if_fail(password_store_interface->get_login_name, NULL);

  return(password_store_interface->get_login_name(password_store,
						  security_context,
						  user_uuid,
						  security_token,
						  error));
}

/**
 * ags_password_store_set_password:
 * @password_store: the #AgsPasswordStore
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the user UUID
 * @security_token: the security token
 * @password: the password
 * @error: the #GError-struct
 *
 * Set password.
 * 
 * Since: 3.0.0
 */
void
ags_password_store_set_password(AgsPasswordStore *password_store,
				GObject *security_context,
				gchar *user_uuid,
				gchar *security_token,
				gchar *password,
				GError **error)
{
  AgsPasswordStoreInterface *password_store_interface;

  g_return_if_fail(AGS_IS_PASSWORD_STORE(password_store));
  password_store_interface = AGS_PASSWORD_STORE_GET_INTERFACE(password_store);
  g_return_if_fail(password_store_interface->set_password);
  password_store_interface->set_password(password_store,
					 security_context,
					 user_uuid,
					 security_token,
					 password,
					 error);
}

/**
 * ags_password_store_get_password:
 * @password_store: the #AgsPasswordStore
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the user UUID
 * @security_token: the security token
 * @error: the #GError-struct
 *
 * Get password.
 * 
 * Returns: (transfer full): the password
 * 
 * Since: 3.0.0
 */
gchar*
ags_password_store_get_password(AgsPasswordStore *password_store,
				GObject *security_context,
				gchar *user_uuid,
				gchar *security_token,
				GError **error)
{
  AgsPasswordStoreInterface *password_store_interface;

  g_return_val_if_fail(AGS_IS_PASSWORD_STORE(password_store), NULL);
  password_store_interface = AGS_PASSWORD_STORE_GET_INTERFACE(password_store);
  g_return_val_if_fail(password_store_interface->get_password, NULL);

  return(password_store_interface->get_password(password_store,
						security_context,
						user_uuid,
						security_token,
						error));
}

/**
 * ags_password_store_encrypt_password:
 * @password_store: the #AgsPasswordStore
 * @password: the password
 * @salt: your salt
 * @error: the #GError-struct
 *
 * Encrypt password.
 *
 * Returns: (transfer full): the encrypted bytes
 *
 * Since: 3.0.0
 */
gchar*
ags_password_store_encrypt_password(AgsPasswordStore *password_store,
				    gchar *password,
				    gchar *salt,
				    GError **error)
{
  AgsPasswordStoreInterface *password_store_interface;

  g_return_val_if_fail(AGS_IS_PASSWORD_STORE(password_store), NULL);
  password_store_interface = AGS_PASSWORD_STORE_GET_INTERFACE(password_store);
  g_return_val_if_fail(password_store_interface->encrypt_password, NULL);

  return(password_store_interface->encrypt_password(password_store,
						    password,
						    salt,
						    error));
}
