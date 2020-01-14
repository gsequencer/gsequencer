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

#ifndef __AGS_XML_CERTIFICATE_H__
#define __AGS_XML_CERTIFICATE_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

G_BEGIN_DECLS

#define AGS_TYPE_XML_CERTIFICATE                (ags_xml_certificate_get_type())
#define AGS_XML_CERTIFICATE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_XML_CERTIFICATE, AgsXmlCertificate))
#define AGS_XML_CERTIFICATE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_XML_CERTIFICATE, AgsXmlCertificateClass))
#define AGS_IS_XML_CERTIFICATE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_XML_CERTIFICATE))
#define AGS_IS_XML_CERTIFICATE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_XML_CERTIFICATE))
#define AGS_XML_CERTIFICATE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_XML_CERTIFICATE, AgsXmlCertificateClass))

#define AGS_XML_CERTIFICATE_GET_OBJ_MUTEX(obj) (&(((AgsXmlCertificate *) obj)->obj_mutex))

typedef struct _AgsXmlCertificate AgsXmlCertificate;
typedef struct _AgsXmlCertificateClass AgsXmlCertificateClass;

struct _AgsXmlCertificate
{
  GObject gobject;

  GRecMutex obj_mutex;

  gchar *filename;
  gchar *encoding;
  gchar *dtd;

  xmlDoc *doc;
  xmlNode *root_node;
};

struct _AgsXmlCertificateClass
{
  GObjectClass gobject;
};

GType ags_xml_certificate_get_type();

void ags_xml_certificate_open_filename(AgsXmlCertificate *xml_certificate,
				       gchar *filename);

AgsXmlCertificate* ags_xml_certificate_new();

G_END_DECLS

#endif /*__AGS_XML_CERTIFICATE_H__*/
