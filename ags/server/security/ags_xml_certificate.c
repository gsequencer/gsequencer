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

#include <ags/server/security/ags_xml_certificate.h>

#include <ags/server/security/ags_certificate.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

void ags_xml_certificate_class_init(AgsXmlCertificateClass *xml_certificate);
void ags_xml_certificate_certificate_interface_init(AgsCertificateInterface *certificate);
void ags_xml_certificate_init(AgsXmlCertificate *xml_certificate);
void ags_xml_certificate_finalize(GObject *gobject);

gboolean ags_xml_certificate_verify(AgsCertificate *certificate,
				    gchar *certs,
				    GError **error);

/**
 * SECTION:ags_xml_certificate
 * @short_description: certificate by XML file
 * @title: AgsXmlCertificate
 * @section_id:
 * @include: ags/server/security/ags_xml_certificate.h
 *
 * The #AgsXmlCertificate is an object to verify certificates.
 */

static gpointer ags_xml_certificate_parent_class = NULL;

GType
ags_xml_certificate_get_type()
{
  static GType ags_type_xml_certificate = 0;

  if(!ags_type_xml_certificate){
    static const GTypeInfo ags_xml_certificate_info = {
      sizeof (AgsXmlCertificateClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_xml_certificate_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsXmlCertificate),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_xml_certificate_init,
    };

    static const GInterfaceInfo ags_certificate_interface_info = {
      (GInterfaceInitFunc) ags_xml_certificate_certificate_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_xml_certificate = g_type_register_static(G_TYPE_OBJECT,
						      "AgsXmlCertificate\0",
						      &ags_xml_certificate_info,
						      0);

    g_type_add_interface_static(ags_type_xml_certificate,
				AGS_TYPE_CERTIFICATE,
				&ags_certificate_interface_info);
  }

  return (ags_type_xml_certificate);
}

void
ags_xml_certificate_class_init(AgsXmlCertificateClass *xml_certificate)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_xml_certificate_parent_class = g_type_class_peek_parent(xml_certificate);

  /* GObjectClass */
  gobject = (GObjectClass *) xml_certificate;

  gobject->finalize = ags_xml_certificate_finalize;
}

void
ags_xml_certificate_certificate_interface_init(AgsCertificateInterface *certificate)
{
  certificate->verify = ags_xml_certificate_verify;  
}

void
ags_xml_certificate_init(AgsXmlCertificate *xml_certificate)
{
  xml_certificate->filename = NULL;
  xml_certificate->encoding = NULL;
  xml_certificate->dtd = NULL;

  xml_certificate->doc = NULL;
  xml_certificate->root_node = NULL;
}

void
ags_xml_certificate_finalize(GObject *gobject)
{
  AgsXmlCertificate *xml_certificate;

  xml_certificate = AGS_XML_CERTIFICATE(gobject);

  G_OBJECT_CLASS(ags_xml_certificate_parent_class)->finalize(gobject);
}

gboolean
ags_xml_certificate_verify(AgsCertificate *certificate,
			   gchar *certs,
			   GError **error)
{
  if(certs == NULL){
    return(FALSE);
  }    
  
  //TODO:JK: implement me

  return(FALSE);
}

/**
 * ags_xml_certificate_new:
 *
 * Create #AgsXmlCertificate.
 *
 * Returns: the new #AgsXmlCertificate instance
 *
 * Since: 1.0.0
 */
AgsXmlCertificate*
ags_xml_certificate_new()
{
  AgsXmlCertificate *xml_certificate;

  xml_certificate = (AgsXmlCertificate *) g_object_new(AGS_TYPE_XML_CERTIFICATE,
						       NULL);

  return(xml_certificate);
}
