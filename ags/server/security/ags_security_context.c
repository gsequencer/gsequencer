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

#include <ags/server/security/ags_security_context.h>

#include <ags/i18n.h>

#include <stdlib.h>

void ags_security_context_class_init(AgsSecurityContextClass *security_context);
void ags_security_context_init(AgsSecurityContext *security_context);
void ags_security_context_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_security_context_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
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

enum{
  PROP_0,
  PROP_CERTS,
};

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

    ags_type_security_context = g_type_register_static(G_TYPE_OBJECT,
						       "AgsSecurityContext\0",
						       &ags_security_context_info,
						       0);
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

  gobject->set_property = ags_security_context_set_property;
  gobject->get_property = ags_security_context_get_property;

  gobject->finalize = ags_security_context_finalize;

  /* properties */
  /**
   * AgsSecurityContext:certs:
   *
   * The assigned certificates as string.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_string("certs",
				   i18n("certificates as string"),
				   i18n("The certificates as string"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CERTS,
				  param_spec);
}

void
ags_security_context_init(AgsSecurityContext *security_context)
{
  security_context->certs = NULL;

  security_context->permitted_context = NULL;
  security_context->server_context = NULL;
}

void
ags_security_context_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsSecurityContext *security_context;

  security_context = AGS_SECURITY_CONTEXT(gobject);
  
  switch(prop_id){
  case PROP_CERTS:
    {
      gchar *certs;

      certs = g_value_get_string(value);

      security_context->certs = g_strdup(certs);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_security_context_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsSecurityContext *security_context;

  security_context = AGS_SECURITY_CONTEXT(gobject);
  
  switch(prop_id){
  case PROP_CERTS:
    {
      g_value_set_string(value, security_context->certs);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_security_context_finalize(GObject *gobject)
{
  AgsSecurityContext *security_context;

  gchar **strv;
  
  security_context = AGS_SECURITY_CONTEXT(gobject);

  g_free(security_context->certs);

  /* permitted contex */
  if(security_context->permitted_context != NULL){
    strv = security_context->permitted_context;

    for(; *strv != NULL; strv++){
      free(*strv);
    }
    
    free(security_context->permitted_context);
  }

  /* server context */
  if(security_context->server_context != NULL){
    strv = security_context->server_context;

    for(; *strv != NULL; strv++){
      free(*strv);
    }
    
    free(security_context->server_context);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_security_context_parent_class)->finalize(gobject);
}

void
ags_security_context_add_server_context(AgsSecurityContext *security_context,
					gchar *server_context)
{
  //TODO:JK: implement me
}

gboolean
ags_security_context_remove_server_context(AgsSecurityContext *security_context,
					   gchar *server_context)
{
  //TODO:JK: implement me

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
