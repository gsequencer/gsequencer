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

#ifndef __AGS_AUTHENTICATION_H__
#define __AGS_AUTHENTICATION_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_AUTHENTICATION                    (ags_authentication_get_type())
#define AGS_AUTHENTICATION(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTHENTICATION, AgsAuthentication))
#define AGS_AUTHENTICATION_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_AUTHENTICATION, AgsAuthenticationInterface))
#define AGS_IS_AUTHENTICATION(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_AUTHENTICATION))
#define AGS_IS_AUTHENTICATION_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_AUTHENTICATION))
#define AGS_AUTHENTICATION_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_AUTHENTICATION, AgsAuthenticationInterface))

typedef struct _AgsAuthentication AgsAuthentication;
typedef struct _AgsAuthenticationInterface AgsAuthenticationInterface;

struct _AgsAuthenticationInterface
{
  GTypeInterface interface;

  gchar* (*login)(AgsAuthentication *authentication,
		  gchar *login, gchar *password,
		  GError **error);

  gboolean (*logout)(AgsAuthentication *authentication,
		     gchar *login, gchar *security_token,
		     GError **error);
  
  gchar* (*generate_token)(AgsAuthentication *authentication,
			   GError **error);

  gchar** (*get_groups)(AgsAuthentication *authentication,
			gchar *login, gchar *security_token,
			GError **error);

  gboolean (*get_permission)(AgsAuthentication *authentication,
			     gchar *login, gchar *security_token,
			     gchar *group_name,
			     GError **error);

  gboolean (*is_session_active)(AgsAuthentication *authentication,
				gchar *login, gchar *security_token,
				GError **error);
};

GType ags_authentication_get_type();

gchar* ags_authentication_login(AgsAuthentication *authentication,
				gchar *login, gchar *password,
				GError **error);

gboolean ags_authentication_logout(AgsAuthentication *authentication,
				   gchar *login, gchar *security_token,
				   GError **error);
  
gchar* ags_authentication_generate_token(AgsAuthentication *authentication,
					 GError **error);

gchar** ags_authentication_get_groups(AgsAuthentication *authentication,
				      gchar *login, gchar *security_token,
				      GError **error);

gboolean ags_authentication_get_permission(AgsAuthentication *authentication,
					   gchar *login, gchar *security_token,
					   gchar *group_name,
					   GError **error);

gboolean ags_authentication_is_session_active(AgsAuthentication *authentication,
					      gchar *login, gchar *security_token,
					      GError **error);

#endif /*__AGS_AUTHENTICATION_H__*/
