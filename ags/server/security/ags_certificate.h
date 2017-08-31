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

#ifndef __AGS_CERTIFICATE_H__
#define __AGS_CERTIFICATE_H__

#include <glib.h>
#include <glib-object.h>

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
  GTypeInterface interface;

  gboolean (*verify)(AgsCertificate *certificate,
		     gchar *certs,
		     GError **error);
};

GType ags_certificate_get_type();

gboolean ags_certificate_verify(AgsCertificate *certificate,
				gchar *certs,
				GError **error);

#endif /*__AGS_CERTIFICATE_H__*/
