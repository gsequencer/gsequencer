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

#ifndef __AGS_AUTHENTICATION_MANAGER_H__
#define __AGS_AUTHENTICATION_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/server/security/ags_authentication.h>

#define AGS_TYPE_AUTHENTICATION_MANAGER                (ags_authentication_manager_get_type())
#define AGS_AUTHENTICATION_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTHENTICATION_MANAGER, AgsAuthenticationManager))
#define AGS_AUTHENTICATION_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUTHENTICATION_MANAGER, AgsAuthenticationManagerClass))
#define AGS_IS_AUTHENTICATION_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUTHENTICATION_MANAGER))
#define AGS_IS_AUTHENTICATION_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUTHENTICATION_MANAGER))
#define AGS_AUTHENTICATION_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_AUTHENTICATION_MANAGER, AgsAuthenticationManagerClass))

typedef struct _AgsAuthenticationManager AgsAuthenticationManager;
typedef struct _AgsAuthenticationManagerClass AgsAuthenticationManagerClass;

struct _AgsAuthenticationManager
{
  GObject object;
  
  GList *authentication;
};

struct _AgsAuthenticationManagerClass
{
  GObjectClass object;
};

GType ags_authentication_manager_get_type(void);

GList* ags_authentication_manager_get_authentication(AgsAuthenticationManager *authentication_manager);

void ags_authentication_manager_add_authentication(AgsAuthenticationManager *authentication_manager,
						   GObject *authentication);
void ags_authentication_manager_remove_authentication(AgsAuthenticationManager *authentication_manager,
						      GObject *authentication);

/*  */
gboolean ags_authentication_manager_login(AgsAuthenticationManager *authentication_manager,
					  gchar *authentication_module,
					  gchar *login,
					  gchar *password,
					  gchar **user_uuid,
					  gchar **security_token);

gboolean ags_authentication_manager_is_session_active(AgsAuthenticationManager *authentication_manager,
						      GObject *security_context,
						      gchar *login,
						      gchar *security_token);

/*  */
AgsAuthenticationManager* ags_authentication_manager_get_instance();

AgsAuthenticationManager* ags_authentication_manager_new();

#endif /*__AGS_AUTHENTICATION_MANAGER_H__*/
