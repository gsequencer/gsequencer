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

#include <ags/server/security/ags_security_context.h>

void ags_security_context_class_init(AgsSecurityContextClass *security_context);
void ags_security_context_init(AgsSecurityContext *security_context);
void ags_security_context_finalize(GObject *gobject);

/**
 * SECTION:ags_security_context
 * @short_description: current security context
 * @title: AgsSecurityContext
 * @section_id:
 * @include: ags/server/security/ags_security_context.h
 *
 * The #AgsSecurityContext is an object to track active server contices.
 */

static gpointer ags_security_context_parent_class = NULL;

GType
ags_security_context_get_type()
{
  static GType ags_type_security_context = 0;

  if(!ags_type_security_context){
    static const GTypeInfo ags_security_context_info = {
      sizeof (AgsSecurityContextClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_security_context_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSecurityContext),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_security_context_init,
    };

    static const GInterfaceInfo ags_authentication_interface_info = {
      (GInterfaceInitFunc) ags_security_context_authentication_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_security_context = g_type_register_static(G_TYPE_OBJECT,
						       "AgsSecurityContext\0",
						       &ags_security_context_info,
						       0);

    g_type_add_interface_static(ags_type_security_context,
				AGS_TYPE_AUTHENTICATION,
				&ags_authentication_interface_info);
  }

  return (ags_type_security_context);
}

void
ags_security_context_class_init(AgsSecurityContextClass *security_context)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_security_context_parent_class = g_type_class_peek_parent(security_context);

  /* GObjectClass */
  gobject = (GObjectClass *) security_context;

  gobject->finalize = ags_security_context_finalize;
}

void
ags_security_context_init(AgsSecurityContext *security_context)
{
}

void
ags_security_context_finalize(GObject *gobject)
{
  AgsSecurityContext *security_context;

  security_context = AGS_SECURITY_CONTEXT(gobject);

  G_OBJECT_CLASS(ags_security_context_parent_class)->finalize(gobject);
}

void
ags_security_context_add_server_context(AgsSecurityContext *security_context,
					gchar *server_context)
{
}

gboolean
ags_security_context_remove_server_context(AgsSecurityContext *security_context,
					   gchar *server_context)
{

  return(TRUE);
}

/**
 * ags_security_context_new:
 *
 * Create #AgsSecurityContext.
 *
 * Returns: the new #AgsSecurityContext instance
 *
 * Since: 1.0.0
 */
AgsSecurityContext*
ags_security_context_new()
{
  AgsSecurityContext *security_context;

  security_context = (AgsSecurityContext *) g_object_new(AGS_TYPE_SECURITY_CONTEXT,
							 NULL);

  return(security_context);
}
