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

#ifndef __AGS_AUTHENTICATION_MANAGER_H__
#define __AGS_AUTHENTICATION_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/server/security/ags_authentication.h>
#include <ags/server/security/ags_security_context.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUTHENTICATION_MANAGER                (ags_authentication_manager_get_type())
#define AGS_AUTHENTICATION_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTHENTICATION_MANAGER, AgsAuthenticationManager))
#define AGS_AUTHENTICATION_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUTHENTICATION_MANAGER, AgsAuthenticationManagerClass))
#define AGS_IS_AUTHENTICATION_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUTHENTICATION_MANAGER))
#define AGS_IS_AUTHENTICATION_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUTHENTICATION_MANAGER))
#define AGS_AUTHENTICATION_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_AUTHENTICATION_MANAGER, AgsAuthenticationManagerClass))

#define AGS_AUTHENTICATION_MANAGER_GET_OBJ_MUTEX(obj) (&(((AgsAuthenticationManager *) obj)->obj_mutex))

#define AGS_AUTHENTICATION_MANAGER_DEFAULT_SESSION_TIMEOUT (2 * G_TIME_SPAN_HOUR)

typedef struct _AgsAuthenticationManager AgsAuthenticationManager;
typedef struct _AgsAuthenticationManagerClass AgsAuthenticationManagerClass;
typedef struct _AgsLoginInfo AgsLoginInfo;

struct _AgsAuthenticationManager
{
  GObject gobject;

  GRecMutex obj_mutex;
  
  GList *authentication;

  gint64 session_timeout;
  
  GHashTable *login;
};

struct _AgsAuthenticationManagerClass
{
  GObjectClass gobject;
};

struct _AgsLoginInfo
{
  gint ref_count;
  gint active_session_count;
  
  gchar *user_uuid;

  AgsSecurityContext *security_context;
};

GType ags_authentication_manager_get_type(void);

AgsLoginInfo* ags_login_info_alloc();
void ags_login_info_free(AgsLoginInfo *login_info);

void ags_login_info_ref(AgsLoginInfo *login_info);
void ags_login_info_unref(AgsLoginInfo *login_info);

GList* ags_authentication_manager_get_authentication(AgsAuthenticationManager *authentication_manager);

void ags_authentication_manager_add_authentication(AgsAuthenticationManager *authentication_manager,
						   GObject *authentication);
void ags_authentication_manager_remove_authentication(AgsAuthenticationManager *authentication_manager,
						      GObject *authentication);

gint64 ags_authentication_manager_get_session_timeout(AgsAuthenticationManager *authentication_manager);

/* login */
AgsLoginInfo* ags_authentication_manager_lookup_login(AgsAuthenticationManager *authentication_manager,
						      gchar *login);
void ags_authentication_manager_insert_login(AgsAuthenticationManager *authentication_manager,
					     gchar *login,
					     AgsLoginInfo *login_info);
void ags_authentication_manager_remove_login(AgsAuthenticationManager *authentication_manager,
					     gchar *login);

/*  */
gboolean ags_authentication_manager_login(AgsAuthenticationManager *authentication_manager,
					  gchar *authentication_module,
					  gchar *login,
					  gchar *password,
					  gchar **user_uuid,
					  gchar **security_token);

gboolean ags_authentication_manager_logout(AgsAuthenticationManager *authentication_manager,
					   GObject *security_context,
					   gchar *login,
					   gchar *security_token);

gchar* ags_authentication_manager_get_digest(AgsAuthenticationManager *authentication_manager,
					     gchar *authentication_module,
					     gchar *realm,
					     gchar *login,
					     gchar *security_token);

gboolean ags_authentication_manager_is_session_active(AgsAuthenticationManager *authentication_manager,
						      GObject *security_context,
						      gchar *login,
						      gchar *security_token);

/*  */
AgsAuthenticationManager* ags_authentication_manager_get_instance();

AgsAuthenticationManager* ags_authentication_manager_new();

G_END_DECLS

#endif /*__AGS_AUTHENTICATION_MANAGER_H__*/
