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

#include <ags/server/security/ags_security_context.h>

#include <ags/lib/ags_string_util.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

#include <stdlib.h>

#include <ags/i18n.h>

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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_security_context = 0;

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
						       "AgsSecurityContext",
						       &ags_security_context_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_security_context);
  }

  return g_define_type_id__volatile;
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
   * Since: 3.0.0
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
  g_rec_mutex_init(&(security_context->obj_mutex));

  security_context->certs = NULL;

  security_context->server_context_umask = (AGS_SECURITY_CONTEXT_RPC_READ |
					    AGS_SECURITY_CONTEXT_RPC_WRITE);
  
  security_context->business_group = NULL;
  
  security_context->server_context = NULL;
}

void
ags_security_context_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsSecurityContext *security_context;

  GRecMutex *security_context_mutex;
  
  security_context = AGS_SECURITY_CONTEXT(gobject);

  /* get security context mutex */
  security_context_mutex = AGS_SECURITY_CONTEXT_GET_OBJ_MUTEX(security_context);
  
  switch(prop_id){
  case PROP_CERTS:
    {
      gchar *certs;

      certs = g_value_get_string(value);

      g_rec_mutex_lock(security_context_mutex);
      
      security_context->certs = g_strdup(certs);

      g_rec_mutex_unlock(security_context_mutex);
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

  GRecMutex *security_context_mutex;

  security_context = AGS_SECURITY_CONTEXT(gobject);

  /* get security context mutex */
  security_context_mutex = AGS_SECURITY_CONTEXT_GET_OBJ_MUTEX(security_context);
  
  switch(prop_id){
  case PROP_CERTS:
    {
      g_rec_mutex_lock(security_context_mutex);

      g_value_set_string(value, security_context->certs);

      g_rec_mutex_unlock(security_context_mutex);
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

  /* business group */
  if(security_context->business_group != NULL){
    strv = security_context->business_group;

    for(; strv[0] != NULL; strv++){
      g_free(strv[0]);
    }
    
    free(security_context->business_group);
  }

  /* server context */
  if(security_context->server_context != NULL){
    strv = security_context->server_context;

    for(; strv[0] != NULL; strv++){
      g_free(strv[0]);
    }
    
    free(security_context->server_context);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_security_context_parent_class)->finalize(gobject);
}

/**
 * ags_security_context_parse_business_group:
 * @security_context: the #AgsSecurityContext
 * @xml_doc: the xmlDoc containing groups
 * @user_uuid: the user's UUID
 * 
 * Parse @business_group and apply to @security_context.
 * 
 * Since: 3.0.0
 */
void
ags_security_context_parse_business_group(AgsSecurityContext *security_context,
					  xmlDoc *xml_doc,
					  gchar *user_uuid)
{
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  
  gchar *xpath;

  guint i, j;
  
  GRecMutex *security_context_mutex;
  
  if(!AGS_IS_SECURITY_CONTEXT(security_context)){
    return;
  }

  /* get security context mutex */
  security_context_mutex = AGS_SECURITY_CONTEXT_GET_OBJ_MUTEX(security_context);

  /* clear business group */
  g_rec_mutex_lock(security_context_mutex);

  if(security_context->business_group != NULL){
    gchar **strv;
    
    strv = security_context->business_group;

    for(; strv[0] != NULL; strv++){
      g_free(strv[0]);
    }
    
    free(security_context->business_group);

    security_context->business_group = NULL;
  }
  
  g_rec_mutex_unlock(security_context_mutex);

  if(xml_doc == NULL ||
     user_uuid == NULL){
    return;
  }
  
  /* read server business group name */
  xpath = g_strdup_printf("(/ags-server-business-group/ags-srv-group-list/ags-srv-group/ags-srv-group-name)/../ags-srv-group-user-list/ags-srv-group-user[text() = '%s']",
			  user_uuid);
  
  g_rec_mutex_lock(security_context_mutex);

  xpath_context = xmlXPathNewContext(xml_doc);
  xpath_object = xmlXPathEval(xpath,
			      xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0, j = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	xmlChar *group_name;
	
	group_name = xmlNodeGetContent(node[i]);

	if(j == 0){
	  security_context->business_group = (gchar **) malloc(2 * sizeof(gchar *)); 
	}else{
	  security_context->business_group = (gchar **) realloc(security_context->business_group,
								(j + 2) * sizeof(gchar *)); 
	}

	security_context->business_group[j] = g_strdup(group_name);	
	
	xmlFree(group_name);

	j++;
      }
    }

    if(j > 0){
      security_context->business_group[j] = NULL;
    }
  }
  
  g_rec_mutex_unlock(security_context_mutex);

  xmlXPathFreeObject(xpath_object);
  xmlXPathFreeContext(xpath_context);

  g_free(xpath);
}

/**
 * ags_security_context_get_business_group:
 * @security_context: the #AgsSecurityContext
 * 
 * Get business group of @security_context.
 * 
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer full): the string vector containing business group or %NULL
 * 
 * Since: 3.0.0
 */
gchar**
ags_security_context_get_business_group(AgsSecurityContext *security_context)
{
  gchar **business_group;
  
  GRecMutex *security_context_mutex;

  if(!AGS_IS_SECURITY_CONTEXT(security_context)){
    return(NULL);
  }

  /* get security context mutex */
  security_context_mutex = AGS_SECURITY_CONTEXT_GET_OBJ_MUTEX(security_context);

  /* dup business group */
  business_group = NULL;

  g_rec_mutex_lock(security_context_mutex);

  if(security_context->business_group != NULL){
    business_group = g_strdupv(security_context->business_group);
  }
  
  g_rec_mutex_unlock(security_context_mutex);

  return(business_group);
}

/**
 * ags_security_context_add_server_context:
 * @security_context: the #AgsSecurityContext
 * @server_context: the server context
 * 
 * Add @server_context to @security_context.
 * 
 * Since: 3.0.0
 */
void
ags_security_context_add_server_context(AgsSecurityContext *security_context,
					gchar *server_context)
{
  guint i;
  
  GRecMutex *security_context_mutex;

  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     server_context == NULL){
    return;
  }

  /* get security context mutex */
  security_context_mutex = AGS_SECURITY_CONTEXT_GET_OBJ_MUTEX(security_context);

  /* add server context */
  g_rec_mutex_lock(security_context_mutex);

  i = 0;

  if(security_context->server_context != NULL){
    i = g_strv_length(security_context->server_context);
  }

  if(i == 0){
    security_context->server_context = (gchar **) malloc(2 * sizeof(gchar *));
  }else{
    security_context->server_context = (gchar **) realloc(security_context->server_context,
							  (i + 2) * sizeof(gchar *));
  }

  security_context->server_context[i] = g_strdup(server_context);
  security_context->server_context[i + 1] = NULL;
  
  g_rec_mutex_unlock(security_context_mutex);
}

/**
 * ags_security_context_remove_server_context:
 * @security_context: the #AgsSecurityContext
 * @server_context: the server context
 * 
 * Remove @server_context from @security_context.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_security_context_remove_server_context(AgsSecurityContext *security_context,
					   gchar *server_context)
{
  gint position;
  guint length;
  guint i, j;
  
  GRecMutex *security_context_mutex;

  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     server_context == NULL){
    return(FALSE);
  }

  /* get security context mutex */
  security_context_mutex = AGS_SECURITY_CONTEXT_GET_OBJ_MUTEX(security_context);

  /* remove server context */
  g_rec_mutex_lock(security_context_mutex);
  
  position = ags_strv_index(security_context->server_context,
			    server_context);

  if(position >= 0){
    length = g_strv_length(security_context->server_context);

    if(length == 1){
      g_free(security_context->server_context[0]);

      free(security_context->server_context);
      
      security_context->server_context = NULL;
    }else{
      gchar **strv;

      strv = (gchar **) malloc(length * sizeof(gchar *));
      
      for(i = 0, j = 0; i < length - 1; i++, j++){
	if(i == position){
	  g_free(security_context->server_context[j]);
	  
	  j++;
	}

	strv[i] = security_context->server_context[j];
      }

      strv[i] = NULL;

      /* free old strv */
      free(security_context->server_context);

      security_context->server_context = strv;
    }
  }
  
  g_rec_mutex_unlock(security_context_mutex);

  return(TRUE);
}

/**
 * ags_security_context_get_server_context:
 * @security_context: the #AgsSecurityContext
 * 
 * Get business group of @security_context.
 * 
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer full): the string vector containing business group or %NULL
 * 
 * Since: 3.0.0
 */
gchar**
ags_security_context_get_server_context(AgsSecurityContext *security_context)
{
  gchar **server_context;
  
  GRecMutex *security_context_mutex;

  if(!AGS_IS_SECURITY_CONTEXT(security_context)){
    return(NULL);
  }

  /* get security context mutex */
  security_context_mutex = AGS_SECURITY_CONTEXT_GET_OBJ_MUTEX(security_context);

  /* dup business group */
  server_context = NULL;

  g_rec_mutex_lock(security_context_mutex);

  if(security_context->server_context != NULL){
    server_context = g_strdupv(security_context->server_context);
  }
  
  g_rec_mutex_unlock(security_context_mutex);

  return(server_context);
}

/**
 * ags_security_context_new:
 *
 * Create #AgsSecurityContext.
 *
 * Returns: the new #AgsSecurityContext instance
 *
 * Since: 3.0.0
 */
AgsSecurityContext*
ags_security_context_new()
{
  AgsSecurityContext *security_context;

  security_context = (AgsSecurityContext *) g_object_new(AGS_TYPE_SECURITY_CONTEXT,
							 NULL);

  return(security_context);
}
