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

#ifndef __AGS_SECURITY_CONTEXT_H__
#define __AGS_SECURITY_CONTEXT_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_SECURITY_CONTEXT                (ags_security_context_get_type())
#define AGS_SECURITY_CONTEXT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SECURITY_CONTEXT, AgsSecurityContext))
#define AGS_SECURITY_CONTEXT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SECURITY_CONTEXT, AgsSecurityContextClass))
#define AGS_IS_SECURITY_CONTEXT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SECURITY_CONTEXT))
#define AGS_IS_SECURITY_CONTEXT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SECURITY_CONTEXT))
#define AGS_SECURITY_CONTEXT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SECURITY_CONTEXT, AgsSecurityContextClass))

typedef struct _AgsSecurityContext AgsSecurityContext;
typedef struct _AgsSecurityContextClass AgsSecurityContextClass;

struct _AgsSecurityContext
{
  GObject object;

  gchar **server_context;
};

struct _AgsSecurityContextClass
{
  GObjectClass object;
};

GType ags_security_context_get_type();

void ags_security_context_add_server_context(AgsSecurityContext *security_context,
					     gchar *server_context);
gboolean ags_security_context_remove_server_context(AgsSecurityContext *security_context,
						    gchar *server_context);

AgsSecurityContext* ags_security_context_new();

#endif /*__AGS_SECURITY_CONTEXT_H__*/
