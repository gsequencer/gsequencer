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

#ifndef __AGS_CERTIFICATE_MANAGER_H__
#define __AGS_CERTIFICATE_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/server/security/ags_certificate.h>

G_BEGIN_DECLS

#define AGS_TYPE_CERTIFICATE_MANAGER                (ags_certificate_manager_get_type())
#define AGS_CERTIFICATE_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CERTIFICATE_MANAGER, AgsCertificateManager))
#define AGS_CERTIFICATE_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CERTIFICATE_MANAGER, AgsCertificateManagerClass))
#define AGS_IS_CERTIFICATE_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CERTIFICATE_MANAGER))
#define AGS_IS_CERTIFICATE_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CERTIFICATE_MANAGER))
#define AGS_CERTIFICATE_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_CERTIFICATE_MANAGER, AgsCertificateManagerClass))

#define AGS_CERTIFICATE_MANAGER_GET_OBJ_MUTEX(obj) (&(((AgsCertificateManager *) obj)->obj_mutex))

typedef struct _AgsCertificateManager AgsCertificateManager;
typedef struct _AgsCertificateManagerClass AgsCertificateManagerClass;

struct _AgsCertificateManager
{
  GObject gobject;

  GRecMutex obj_mutex;
  
  GList *certificate;
};

struct _AgsCertificateManagerClass
{
  GObjectClass gobject;
};

GType ags_certificate_manager_get_type(void);

GList* ags_certificate_manager_get_certificate(AgsCertificateManager *certificate_manager);

void ags_certificate_manager_add_certificate(AgsCertificateManager *certificate_manager,
					     GObject *certificate);
void ags_certificate_manager_remove_certificate(AgsCertificateManager *certificate_manager,
						GObject *certificate);

/*  */
void ags_certificate_manager_get_key_file(AgsCertificateManager *certificate_manager,
					  GObject *security_context,
					  gchar *user_uuid,
					  gchar *security_token,
					  gchar *domain,
					  gchar *key_type,
					  gchar **public_key_file,
					  gchar **private_key_file);

/*  */
AgsCertificateManager* ags_certificate_manager_get_instance();

AgsCertificateManager* ags_certificate_manager_new();

G_END_DECLS

#endif /*__AGS_CERTIFICATE_MANAGER_H__*/
