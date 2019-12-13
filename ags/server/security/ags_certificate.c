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

#include <ags/server/security/ags_certificate.h>

void ags_certificate_base_init(AgsCertificateInterface *ginterface);

/**
 * SECTION:ags_certificate
 * @short_description: base password certificate
 * @title: AgsCertificate
 * @section_id: AgsCertificate
 * @include: ags/server/security/ags_certificate.h
 *
 * The #AgsCertificate interface gives you a unique access to all objects
 * and is responsible to set up signal handlers.
 */

GType
ags_certificate_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_certificate = 0;

    static const GTypeInfo ags_certificate_info = {
      sizeof(AgsCertificateInterface),
      (GBaseInitFunc) ags_certificate_base_init,
      NULL, /* base_finalize */
    };

    ags_type_certificate = g_type_register_static(G_TYPE_INTERFACE,
						  "AgsCertificate", &ags_certificate_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_certificate);
  }

  return g_define_type_id__volatile;
}

void
ags_certificate_base_init(AgsCertificateInterface *ginterface)
{
  /* empty */
}

/**
 * ags_certificate_get_cert_uuid:
 * @certificate: the #AgsCertificate
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the users unique identifier
 * @security_token: the security token
 * @error: the #GError-struct
 *
 * Get cert UUID as string vector.
 * 
 * Returns: the cert UUIDs as %NULL terminated string array
 * 
 * Since: 3.0.0
 */
gchar**
ags_certificate_get_cert_uuid(AgsCertificate *certificate,
			      GObject *security_context,
			      gchar *user_uuid,
			      gchar *security_token,
			      GError **error)
{
  AgsCertificateInterface *certificate_interface;

  g_return_val_if_fail(AGS_IS_CERTIFICATE(certificate), NULL);
  certificate_interface = AGS_CERTIFICATE_GET_INTERFACE(certificate);
  g_return_val_if_fail(certificate_interface->get_cert_uuid, NULL);

  return(certificate_interface->get_cert_uuid(certificate,
					      security_context,
					      user_uuid,
					      security_token,
					      error));
}

/**
 * ags_certificate_set_domain:
 * @certificate: the #AgsCertificate
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the users unique identifier
 * @security_token: the security token
 * @cert_uuid: the cert's UUID
 * @domain: the domain
 * @error: the #GError-struct
 *
 * Set domain.
 * 
 * Since: 3.0.0
 */
void
ags_certificate_set_domain(AgsCertificate *certificate,
			   GObject *security_context,
			   gchar *user_uuid,
			   gchar *security_token,
			   gchar *cert_uuid,
			   gchar *domain,
			   GError **error)
{
  AgsCertificateInterface *certificate_interface;

  g_return_if_fail(AGS_IS_CERTIFICATE(certificate));
  certificate_interface = AGS_CERTIFICATE_GET_INTERFACE(certificate);
  g_return_if_fail(certificate_interface->set_domain);
  certificate_interface->set_domain(certificate,
				    security_context,
				    user_uuid,
				    security_token,
				    cert_uuid,
				    domain,
				    error);
}

/**
 * ags_certificate_get_domain:
 * @certificate: the #AgsCertificate
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the users unique identifier
 * @security_token: the security token
 * @cert_uuid: the cert's UUID
 * @error: the #GError-struct
 *
 * Get domain.
 * 
 * Returns: the domain
 * 
 * Since: 3.0.0
 */
gchar*
ags_certificate_get_domain(AgsCertificate *certificate,
			   GObject *security_context,
			   gchar *user_uuid,
			   gchar *security_token,
			   gchar *cert_uuid,
			   GError **error)
{
  AgsCertificateInterface *certificate_interface;

  g_return_val_if_fail(AGS_IS_CERTIFICATE(certificate), NULL);
  certificate_interface = AGS_CERTIFICATE_GET_INTERFACE(certificate);
  g_return_val_if_fail(certificate_interface->get_domain, NULL);

  return(certificate_interface->get_domain(certificate,
					   security_context,
					   user_uuid,
					   security_token,
					   cert_uuid,
					   error));
}

/**
 * ags_certificate_set_key_type:
 * @certificate: the #AgsCertificate
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the users unique identifier
 * @security_token: the security token
 * @cert_uuid: the cert's UUID
 * @key_type: the key type
 * @error: the #GError-struct
 *
 * Set key type.
 * 
 * Since: 3.0.0
 */
void
ags_certificate_set_key_type(AgsCertificate *certificate,
			     GObject *security_context,
			     gchar *user_uuid,
			     gchar *security_token,
			     gchar *cert_uuid,
			     gchar *key_type,
			     GError **error)
{
  AgsCertificateInterface *certificate_interface;

  g_return_if_fail(AGS_IS_CERTIFICATE(certificate));
  certificate_interface = AGS_CERTIFICATE_GET_INTERFACE(certificate);
  g_return_if_fail(certificate_interface->set_key_type);
  certificate_interface->set_key_type(certificate,
				      security_context,
				      user_uuid,
				      security_token,
				      cert_uuid,
				      key_type,
				      error);
}

/**
 * ags_certificate_get_key_type:
 * @certificate: the #AgsCertificate
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the users unique identifier
 * @security_token: the security token
 * @cert_uuid: the cert's UUID
 * @error: the #GError-struct
 *
 * Get key type.
 * 
 * Returns: the key type
 * 
 * Since: 3.0.0
 */
gchar*
ags_certificate_get_key_type(AgsCertificate *certificate,
			     GObject *security_context,
			     gchar *user_uuid,
			     gchar *security_token,
			     gchar *cert_uuid,
			     GError **error)
{
  AgsCertificateInterface *certificate_interface;

  g_return_val_if_fail(AGS_IS_CERTIFICATE(certificate), NULL);
  certificate_interface = AGS_CERTIFICATE_GET_INTERFACE(certificate);
  g_return_val_if_fail(certificate_interface->get_key_type, NULL);

  return(certificate_interface->get_key_type(certificate,
					     security_context,
					     user_uuid,
					     security_token,
					     cert_uuid,
					     error));
}

/**
 * ags_certificate_set_public_key_file:
 * @certificate: the #AgsCertificate
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the users unique identifier
 * @security_token: the security token
 * @cert_uuid: the cert's UUID
 * @public_key_file: the public key file
 * @error: the #GError-struct
 *
 * Set public key file.
 * 
 * Since: 3.0.0
 */
void
ags_certificate_set_public_key_file(AgsCertificate *certificate,
				    GObject *security_context,
				    gchar *user_uuid,
				    gchar *security_token,
				    gchar *cert_uuid,
				    gchar *public_key_file,
				    GError **error)
{
  AgsCertificateInterface *certificate_interface;

  g_return_if_fail(AGS_IS_CERTIFICATE(certificate));
  certificate_interface = AGS_CERTIFICATE_GET_INTERFACE(certificate);
  g_return_if_fail(certificate_interface->set_public_key_file);
  certificate_interface->set_public_key_file(certificate,
					     security_context,
					     user_uuid,
					     security_token,
					     cert_uuid,
					     public_key_file,
					     error);
}

/**
 * ags_certificate_get_public_key_file:
 * @certificate: the #AgsCertificate
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the users unique identifier
 * @security_token: the security token
 * @cert_uuid: the cert's UUID
 * @error: the #GError-struct
 *
 * Get public key file.
 * 
 * Returns: the public key file
 * 
 * Since: 3.0.0
 */
gchar*
ags_certificate_get_public_key_file(AgsCertificate *certificate,
				    GObject *security_context,
				    gchar *user_uuid,
				    gchar *security_token,
				    gchar *cert_uuid,
				    GError **error)
{
  AgsCertificateInterface *certificate_interface;

  g_return_val_if_fail(AGS_IS_CERTIFICATE(certificate), NULL);
  certificate_interface = AGS_CERTIFICATE_GET_INTERFACE(certificate);
  g_return_val_if_fail(certificate_interface->get_public_key_file, NULL);

  return(certificate_interface->get_public_key_file(certificate,
						    security_context,
						    user_uuid,
						    security_token,
						    cert_uuid,
						    error));
}

/**
 * ags_certificate_set_private_key_file:
 * @certificate: the #AgsCertificate
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the users unique identifier
 * @security_token: the security token
 * @cert_uuid: the cert's UUID
 * @private_key_file: the private key file
 * @error: the #GError-struct
 *
 * Set private key file.
 * 
 * Since: 3.0.0
 */
void
ags_certificate_set_private_key_file(AgsCertificate *certificate,
				     GObject *security_context,
				     gchar *user_uuid,
				     gchar *security_token,
				     gchar *cert_uuid,
				     gchar *private_key_file,
				     GError **error)
{
  AgsCertificateInterface *certificate_interface;

  g_return_if_fail(AGS_IS_CERTIFICATE(certificate));
  certificate_interface = AGS_CERTIFICATE_GET_INTERFACE(certificate);
  g_return_if_fail(certificate_interface->set_private_key_file);
  certificate_interface->set_private_key_file(certificate,
					      security_context,
					      user_uuid,
					      security_token,
					      cert_uuid,
					      private_key_file,
					      error);
}

/**
 * ags_certificate_get_private_key_file:
 * @certificate: the #AgsCertificate
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the users unique identifier
 * @security_token: the security token
 * @cert_uuid: the cert's UUID
 * @error: the #GError-struct
 *
 * Get private key file.
 * 
 * Returns: the private key file
 * 
 * Since: 3.0.0
 */
gchar*
ags_certificate_get_private_key_file(AgsCertificate *certificate,
				     GObject *security_context,
				     gchar *user_uuid,
				     gchar *security_token,
				     gchar *cert_uuid,
				     GError **error)
{
  AgsCertificateInterface *certificate_interface;

  g_return_val_if_fail(AGS_IS_CERTIFICATE(certificate), NULL);
  certificate_interface = AGS_CERTIFICATE_GET_INTERFACE(certificate);
  g_return_val_if_fail(certificate_interface->get_private_key_file, NULL);

  return(certificate_interface->get_private_key_file(certificate,
						     security_context,
						     user_uuid,
						     security_token,
						     cert_uuid,
						     error));
}
