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

#ifndef __AGS_AUTH_SECURITY_CONTEXT_H__
#define __AGS_AUTH_SECURITY_CONTEXT_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#include <ags/server/security/ags_security_context.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUTH_SECURITY_CONTEXT                (ags_auth_security_context_get_type())
#define AGS_AUTH_SECURITY_CONTEXT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTH_SECURITY_CONTEXT, AgsAuthSecurityContext))
#define AGS_AUTH_SECURITY_CONTEXT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUTH_SECURITY_CONTEXT, AgsAuthSecurityContextClass))
#define AGS_IS_AUTH_SECURITY_CONTEXT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUTH_SECURITY_CONTEXT))
#define AGS_IS_AUTH_SECURITY_CONTEXT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUTH_SECURITY_CONTEXT))
#define AGS_AUTH_SECURITY_CONTEXT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_AUTH_SECURITY_CONTEXT, AgsAuthSecurityContextClass))

#define AGS_AUTH_SECURITY_CONTEXT_GET_OBJ_MUTEX(obj) (&(((AgsAuthSecurityContext *) obj)->obj_mutex))

typedef struct _AgsAuthSecurityContext AgsAuthSecurityContext;
typedef struct _AgsAuthSecurityContextClass AgsAuthSecurityContextClass;

struct _AgsAuthSecurityContext
{
  AgsSecurityContext security_context;
};

struct _AgsAuthSecurityContextClass
{
  AgsSecurityContextClass security_context;
};

GType ags_auth_security_context_get_type();

AgsAuthSecurityContext* ags_auth_security_context_get_instance();

AgsAuthSecurityContext* ags_auth_security_context_new();

G_END_DECLS

#endif /*__AGS_AUTH_SECURITY_CONTEXT_H__*/
