/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

void ags_certificate_base_init(AgsCertificateInterface *interface);

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
  static GType ags_type_certificate = 0;

  if(!ags_type_certificate){
    static const GTypeInfo ags_certificate_info = {
      sizeof(AgsCertificateInterface),
      (GBaseInitFunc) ags_certificate_base_init,
      NULL, /* base_finalize */
    };

    ags_type_certificate = g_type_register_static(G_TYPE_INTERFACE,
						  "AgsCertificate\0", &ags_certificate_info,
						  0);
  }

  return(ags_type_certificate);
}

void
ags_certificate_base_init(AgsCertificateInterface *interface)
{
  /* empty */
}

/**
 * ags_certificate_verify:
 * @certificate: the #AgsCertificate
 * @certs: the certificate data
 * @error: the #GError-struct
 * 
 * Verify @certs. 
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 1.0.0
 */
gboolean
ags_certificate_verify(AgsCertificate *certificate,
		       gchar *certs,
		       GError **error)
{
  AgsCertificateInterface *certificate_interface;

  g_return_val_if_fail(AGS_IS_CERTIFICATE(certificate), FALSE);
  certificate_interface = AGS_CERTIFICATE_GET_INTERFACE(certificate);
  g_return_val_if_fail(certificate_interface->verify, FALSE);

  return(certificate_interface->verify(certificate,
				       certs,
				       error));
}
