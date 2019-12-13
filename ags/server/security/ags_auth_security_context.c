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

#include <ags/server/security/ags_auth_security_context.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_auth_security_context_class_init(AgsAuthSecurityContextClass *auth_security_context);
void ags_auth_security_context_init(AgsAuthSecurityContext *auth_security_context);
void ags_auth_security_context_finalize(GObject *gobject);

/**
 * SECTION:ags_auth_security_context
 * @short_description: auth security context
 * @title: AgsAuthSecurityContext
 * @section_id:
 * @include: ags/server/security/ags_auth_security_context.h
 *
 * The #AgsAuthSecurityContext has got all available privileges to do anything.
 */

static gpointer ags_auth_security_context_parent_class = NULL;

GType
ags_auth_security_context_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_auth_security_context = 0;

    static const GTypeInfo ags_auth_security_context_info = {
      sizeof (AgsAuthSecurityContextClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_auth_security_context_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAuthSecurityContext),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_auth_security_context_init,
    };

    ags_type_auth_security_context = g_type_register_static(AGS_TYPE_SECURITY_CONTEXT,
							    "AgsAuthSecurityContext",
							    &ags_auth_security_context_info,
							    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_auth_security_context);
  }

  return g_define_type_id__volatile;
}

void
ags_auth_security_context_class_init(AgsAuthSecurityContextClass *auth_security_context)
{
  GObjectClass *gobject;

  ags_auth_security_context_parent_class = g_type_class_peek_parent(auth_security_context);

  /* GObjectClass */
  gobject = (GObjectClass *) auth_security_context;

  gobject->finalize = ags_auth_security_context_finalize;

  /* properties */
}

void
ags_auth_security_context_init(AgsAuthSecurityContext *auth_security_context)
{
  //empty
}

void
ags_auth_security_context_finalize(GObject *gobject)
{
  AgsAuthSecurityContext *auth_security_context;

  auth_security_context = AGS_AUTH_SECURITY_CONTEXT(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_auth_security_context_parent_class)->finalize(gobject);
}

/**
 * ags_auth_security_context_new:
 *
 * Create #AgsAuthSecurityContext.
 *
 * Returns: the new #AgsAuthSecurityContext instance
 *
 * Since: 3.0.0
 */
AgsAuthSecurityContext*
ags_auth_security_context_new()
{
  AgsAuthSecurityContext *auth_security_context;

  auth_security_context = (AgsAuthSecurityContext *) g_object_new(AGS_TYPE_AUTH_SECURITY_CONTEXT,
								  NULL);

  return(auth_security_context);
}
