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

#ifndef __AGS_PASSWORD_STORE_H__
#define __AGS_PASSWORD_STORE_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_PASSWORD_STORE                    (ags_password_store_get_type())
#define AGS_PASSWORD_STORE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PASSWORD_STORE, AgsPasswordStore))
#define AGS_PASSWORD_STORE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_PASSWORD_STORE, AgsPasswordStoreInterface))
#define AGS_IS_PASSWORD_STORE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PASSWORD_STORE))
#define AGS_IS_PASSWORD_STORE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_PASSWORD_STORE))
#define AGS_PASSWORD_STORE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_PASSWORD_STORE, AgsPasswordStoreInterface))

typedef struct _AgsPasswordStore AgsPasswordStore;
typedef struct _AgsPasswordStoreInterface AgsPasswordStoreInterface;

typedef enum{
  AGS_SECURITY_CONTEXT_RPC_READ       = 1,
  AGS_SECURITY_CONTEXT_RPC_WRITE      = 1 <<  1,
  AGS_SECURITY_CONTEXT_RPC_EXECUTE    = 1 <<  2,
  AGS_SECURITY_CONTEXT_ACCOUNT_READ   = 1 <<  3,
  AGS_SECURITY_CONTEXT_ACCOUNT_WRITE  = 1 <<  4,
}AgsSecurityContextMode;

struct _AgsPasswordStoreInterface
{
  GTypeInterface interface;

  void (*set_login_name)(AgsPasswordStore *password_store,
			 GObject *security_context,
			 gchar *user_uuid,
			 gchar *security_token,
			 gchar *login_name,
			 GError **error);
  gchar* (*get_login_name)(AgsPasswordStore *password_store,
			   GObject *security_context,
			   gchar *user_uuid,
			   gchar *security_token,
			   GError **error);

  void (*set_password)(AgsPasswordStore *password_store,
		       GObject *security_context,
		       gchar *login,
		       gchar *security_token,
		       gchar *password,
		       GError **error);
  gchar* (*get_password)(AgsPasswordStore *password_store,
			 GObject *security_context,
			 gchar *login,
			 gchar *security_token,
			 GError **error);
  
  gchar* (*encrypt_password)(AgsPasswordStore *password_store,
			     gchar *password,
			     gchar *salt,
			     GError **error);
};

GType ags_password_store_get_type();

void ags_password_store_set_login_name(AgsPasswordStore *password_store,
				       GObject *security_context,
				       gchar *user,
				       gchar *security_token,
				       gchar *login_name,
				       GError **error);
gchar* ags_password_store_get_login_name(AgsPasswordStore *password_store,
					 GObject *security_context,
					 gchar *user,
					 gchar *security_token,
					 GError **error);

void ags_password_store_set_password(AgsPasswordStore *password_store,
				     GObject *security_context,
				     gchar *login,
				     gchar *security_token,
				     gchar *password,
				     GError **error);
gchar* ags_password_store_get_password(AgsPasswordStore *password_store,
				       GObject *security_context,
				       gchar *login,
				       gchar *security_token,
				       GError **error);

gchar* ags_password_store_encrypt_password(AgsPasswordStore *password_store,
					   gchar *password,
					   gchar *salt,
					   GError **error);

#endif /*__AGS_PASSWORD_STORE_H__*/
