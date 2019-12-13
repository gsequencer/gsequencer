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

#ifndef __AGS_CERTIFICATE_H__
#define __AGS_CERTIFICATE_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AGS_TYPE_CERTIFICATE                    (ags_certificate_get_type())
#define AGS_CERTIFICATE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CERTIFICATE, AgsCertificate))
#define AGS_CERTIFICATE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_CERTIFICATE, AgsCertificateInterface))
#define AGS_IS_CERTIFICATE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CERTIFICATE))
#define AGS_IS_CERTIFICATE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_CERTIFICATE))
#define AGS_CERTIFICATE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_CERTIFICATE, AgsCertificateInterface))

typedef struct _AgsCertificate AgsCertificate;
typedef struct _AgsCertificateInterface AgsCertificateInterface;

struct _AgsCertificateInterface
{
  GTypeInterface ginterface;

  gchar** (*get_cert_uuid)(AgsCertificate *certificate,
			   GObject *security_context,
			   gchar *user_uuid,
			   gchar *security_token,
			   GError **error);
  
  void (*set_domain)(AgsCertificate *certificate,
		     GObject *security_context,
		     gchar *user_uuid,
		     gchar *security_token,
		     gchar *cert_uuid,
		     gchar *domain,
		     GError **error);
  gchar* (*get_domain)(AgsCertificate *certificate,
		       GObject *security_context,
		       gchar *user_uuid,
		       gchar *security_token,
		       gchar *cert_uuid,
		       GError **error);

  void (*set_key_type)(AgsCertificate *certificate,
		       GObject *security_context,
		       gchar *user_uuid,
		       gchar *security_token,
		       gchar *cert_uuid,
		       gchar *key_type,
		       GError **error);
  gchar* (*get_key_type)(AgsCertificate *certificate,
			 GObject *security_context,
			 gchar *user_uuid,
			 gchar *security_token,
			 gchar *cert_uuid,
			 GError **error);

  void (*set_public_key_file)(AgsCertificate *certificate,
			      GObject *security_context,
			      gchar *user_uuid,
			      gchar *security_token,
			      gchar *cert_uuid,
			      gchar *public_key_file,
			      GError **error);
  gchar* (*get_public_key_file)(AgsCertificate *certificate,
				GObject *security_context,
				gchar *user_uuid,
				gchar *security_token,
				gchar *cert_uuid,
				GError **error);

  void (*set_private_key_file)(AgsCertificate *certificate,
			       GObject *security_context,
			       gchar *user_uuid,
			       gchar *security_token,
			       gchar *cert_uuid,
			       gchar *private_key_file,
			       GError **error);
  gchar* (*get_private_key_file)(AgsCertificate *certificate,
				 GObject *security_context,
				 gchar *user_uuid,
				 gchar *security_token,
				 gchar *cert_uuid,
				 GError **error);
};

GType ags_certificate_get_type();

gchar** ags_certificate_get_cert_uuid(AgsCertificate *certificate,
				      GObject *security_context,
				      gchar *user_uuid,
				      gchar *security_token,
				      GError **error);

void ags_certificate_set_domain(AgsCertificate *certificate,
				GObject *security_context,
				gchar *user_uuid,
				gchar *security_token,
				gchar *cert_uuid,
				gchar *domain,
				GError **error);
gchar* ags_certificate_get_domain(AgsCertificate *certificate,
				  GObject *security_context,
				  gchar *user_uuid,
				  gchar *security_token,
				  gchar *cert_uuid,
				  GError **error);

void ags_certificate_set_key_type(AgsCertificate *certificate,
				  GObject *security_context,
				  gchar *user_uuid,
				  gchar *security_token,
				  gchar *cert_uuid,
				  gchar *key_type,
				  GError **error);
gchar* ags_certificate_get_key_type(AgsCertificate *certificate,
				    GObject *security_context,
				    gchar *user_uuid,
				    gchar *security_token,
				    gchar *cert_uuid,
				    GError **error);

void ags_certificate_set_public_key_file(AgsCertificate *certificate,
					 GObject *security_context,
					 gchar *user_uuid,
					 gchar *security_token,
					 gchar *cert_uuid,
					 gchar *public_key_file,
					 GError **error);
gchar* ags_certificate_get_public_key_file(AgsCertificate *certificate,
					   GObject *security_context,
					   gchar *user_uuid,
					   gchar *security_token,
					   gchar *cert_uuid,
					   GError **error);

void ags_certificate_set_private_key_file(AgsCertificate *certificate,
					  GObject *security_context,
					  gchar *user_uuid,
					  gchar *security_token,
					  gchar *cert_uuid,
					  gchar *private_key_file,
					  GError **error);
gchar* ags_certificate_get_private_key_file(AgsCertificate *certificate,
					    GObject *security_context,
					    gchar *user_uuid,
					    gchar *security_token,
					    gchar *cert_uuid,
					    GError **error);

G_END_DECLS

#endif /*__AGS_CERTIFICATE_H__*/
