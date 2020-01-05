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
 *
 * Yuri Victorovich <yuri@FreeBSD.org> (tiny change) - provided FreeBSD and
 *   DragonFly macros.
 */

#include <ags/server/security/ags_xml_password_store.h>

#include <ags/server/security/ags_authentication_manager.h>
#include <ags/server/security/ags_password_store.h>
#include <ags/server/security/ags_auth_security_context.h>

#include <unistd.h>

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__DragonFly__) && !defined(AGS_W32API)
#define __USE_GNU
#define _GNU_SOURCE
#include <crypt.h>
#endif

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

void ags_xml_password_store_class_init(AgsXmlPasswordStoreClass *xml_password_store);
void ags_xml_password_store_password_store_interface_init(AgsPasswordStoreInterface *password_store);
void ags_xml_password_store_init(AgsXmlPasswordStore *xml_password_store);
void ags_xml_password_store_finalize(GObject *gobject);

gchar* ags_xml_password_store_get_login_name(AgsPasswordStore *password_store,
					     GObject *security_context,
					     gchar *user_uuid,
					     gchar *security_token,
					     GError **error);
void ags_xml_password_store_set_login_name(AgsPasswordStore *password_store,
					   GObject *security_context,
					   gchar *user_uuid,
					   gchar *security_token,
					   gchar *login_name,
					   GError **error);
gchar* ags_xml_password_store_get_password(AgsPasswordStore *password_store,
					   GObject *security_context,
					   gchar *user,
					   gchar *security_token,
					   GError **error);
void ags_xml_password_store_set_password(AgsPasswordStore *password_store,
					 GObject *security_context,
					 gchar *user,
					 gchar *security_token,
					 gchar *password,
					 GError **error);
char* ags_xml_password_store_encrypt_password(AgsPasswordStore *password_store,
					      gchar *password,
					      gchar *salt,
					      GError **error);

/**
 * SECTION:ags_xml_password_store
 * @short_description: password store by XML file
 * @title: AgsXmlPasswordStore
 * @section_id:
 * @include: ags/server/security/ags_xml_password_store.h
 *
 * The #AgsXmlPasswordStore is an object to keep passwords.
 */

static gpointer ags_xml_password_store_parent_class = NULL;

GType
ags_xml_password_store_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_xml_password_store = 0;

    static const GTypeInfo ags_xml_password_store_info = {
      sizeof (AgsXmlPasswordStoreClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_xml_password_store_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsXmlPasswordStore),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_xml_password_store_init,
    };

    static const GInterfaceInfo ags_password_store_interface_info = {
      (GInterfaceInitFunc) ags_xml_password_store_password_store_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_xml_password_store = g_type_register_static(G_TYPE_OBJECT,
							 "AgsXmlPasswordStore",
							 &ags_xml_password_store_info,
							 0);

    g_type_add_interface_static(ags_type_xml_password_store,
				AGS_TYPE_PASSWORD_STORE,
				&ags_password_store_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_xml_password_store);
  }

  return g_define_type_id__volatile;
}

void
ags_xml_password_store_class_init(AgsXmlPasswordStoreClass *xml_password_store)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_xml_password_store_parent_class = g_type_class_peek_parent(xml_password_store);

  /* GObjectClass */
  gobject = (GObjectClass *) xml_password_store;

  gobject->finalize = ags_xml_password_store_finalize;
}

void
ags_xml_password_store_password_store_interface_init(AgsPasswordStoreInterface *password_store)
{
  password_store->get_login_name = ags_xml_password_store_get_login_name;
  password_store->set_login_name = ags_xml_password_store_set_login_name;

  password_store->get_password = ags_xml_password_store_get_password;
  password_store->set_password = ags_xml_password_store_set_password;

  password_store->encrypt_password = ags_xml_password_store_encrypt_password;
}

void
ags_xml_password_store_init(AgsXmlPasswordStore *xml_password_store)
{
  g_rec_mutex_init(&(xml_password_store->obj_mutex));

  xml_password_store->filename = NULL;
  xml_password_store->encoding = NULL;
  xml_password_store->dtd = NULL;

  xml_password_store->doc = NULL;
  xml_password_store->root_node = NULL;
}

void
ags_xml_password_store_finalize(GObject *gobject)
{
  AgsXmlPasswordStore *xml_password_store;

  xml_password_store = AGS_XML_PASSWORD_STORE(gobject);

  g_free(xml_password_store->filename);
  g_free(xml_password_store->encoding);
  g_free(xml_password_store->dtd);
  
  if(xml_password_store->doc != NULL){
    xmlFreeDoc(xml_password_store->doc);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_xml_password_store_parent_class)->finalize(gobject);
}

void
ags_xml_password_store_set_login_name(AgsPasswordStore *password_store,
				      GObject *security_context,
				      gchar *user_uuid,
				      gchar *security_token,
				      gchar *login,
				      GError **error)
{
  AgsXmlPasswordStore *xml_password_store;
  
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *user_node;
  xmlNode *login_node;
  xmlNode *child;
  
  gchar *xpath;
  
  guint i;
  
  GRecMutex *xml_password_store_mutex;

  /* authentication */
  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     user_uuid == NULL){
    return;
  }

  if(!AGS_IS_AUTH_SECURITY_CONTEXT(security_context)){
    if(!ags_authentication_manager_is_session_active(ags_authentication_manager_get_instance(),
						     security_context,
						     user_uuid,
						     security_token)){
      return;
    }
  }
      
  xml_password_store = AGS_XML_PASSWORD_STORE(password_store);

  if(xml_password_store->doc == NULL ||
     xml_password_store->root_node == NULL){
    return;
  }

  xml_password_store_mutex = AGS_XML_PASSWORD_STORE_GET_OBJ_MUTEX(xml_password_store);

  user_node = NULL;

  xpath = g_strdup_printf("/ags-server-password-store/ags-srv-user-list/ags-srv-user/ags-srv-user-uuid[text() = '%s']",
			  user_uuid);

  g_rec_mutex_lock(xml_password_store_mutex);
    
  xpath_context = xmlXPathNewContext(xml_password_store->doc);
  xpath_object = xmlXPathEval(xpath,
			      xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	user_node = node[i]->parent;

	break;
      }
    }
  }

  g_rec_mutex_unlock(xml_password_store_mutex);

  xmlXPathFreeObject(xpath_object);
  xmlXPathFreeContext(xpath_context);

  g_free(xpath);

  if(user_node != NULL){
    login_node = NULL;
    
    child = user_node->children;
    
    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"ags-srv-user-login",
				19)){
	  login_node = child;
	  
	  break;
	}
      }
	
      child = child->next;
    }

    if(login_node == NULL){
      login_node = xmlNewNode(NULL,
			      "ags-srv-user-login");
      xmlAddChild(user_node,
		  login_node);
    }

    xmlNodeSetContent(login_node,
		      login);
  }
}

gchar*
ags_xml_password_store_get_login_name(AgsPasswordStore *password_store,
				      GObject *security_context,
				      gchar *user_uuid,
				      gchar *security_token,
				      GError **error)
{
  AgsXmlPasswordStore *xml_password_store;
  
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *user_node;
  xmlNode *login_node;
  xmlNode *child;
  
  gchar *xpath;
  gchar *login;
  
  guint i;
  
  GRecMutex *xml_password_store_mutex;

  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     user_uuid == NULL){
    return(NULL);
  }    

  if(!AGS_IS_AUTH_SECURITY_CONTEXT(security_context)){
    if(!ags_authentication_manager_is_session_active(ags_authentication_manager_get_instance(),
						     security_context,
						     user_uuid,
						     security_token)){
      return(NULL);
    }
  }

  xml_password_store = AGS_XML_PASSWORD_STORE(password_store);

  if(xml_password_store->doc == NULL ||
     xml_password_store->root_node == NULL){
    return(NULL);
  }

  xml_password_store_mutex = AGS_XML_PASSWORD_STORE_GET_OBJ_MUTEX(xml_password_store);
  
  user_node = NULL;

  xpath = g_strdup_printf("/ags-server-password-store/ags-srv-user-list/ags-srv-user/ags-srv-user-uuid[text() = '%s']",
			  user_uuid);

  g_rec_mutex_lock(xml_password_store_mutex);
    
  xpath_context = xmlXPathNewContext(xml_password_store->doc);
  xpath_object = xmlXPathEval(xpath,
			      xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	user_node = node[i]->parent;

	break;
      }
    }
  }

  g_rec_mutex_unlock(xml_password_store_mutex);

  xmlXPathFreeObject(xpath_object);
  xmlXPathFreeContext(xpath_context);

  g_free(xpath);

  login = NULL;
  
  if(user_node != NULL){
    login_node = NULL;
    
    child = user_node->children;
    
    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"ags-srv-user-login",
				19)){
	  login_node = child;
	  
	  break;
	}
      }
	
      child = child->next;
    }

    if(login_node != NULL){
      xmlChar *tmp_login;
      
      tmp_login = xmlNodeGetContent(login_node);

      login = g_strdup(tmp_login);

      xmlFree(tmp_login);
    }
  }

  return(login);
}

void
ags_xml_password_store_set_password(AgsPasswordStore *password_store,
				    GObject *security_context,
				    gchar *user_uuid,
				    gchar *security_token,
				    gchar *password,
				    GError **error)
{
  AgsXmlPasswordStore *xml_password_store;
  
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *user_node;
  xmlNode *password_node;
  xmlNode *child;
  
  gchar *xpath;
  
  guint i;
  
  GRecMutex *xml_password_store_mutex;

  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     user_uuid == NULL){
    return;
  }    

  if(!AGS_IS_AUTH_SECURITY_CONTEXT(security_context)){
    if(!ags_authentication_manager_is_session_active(ags_authentication_manager_get_instance(),
						     security_context,
						     user_uuid,
						     security_token)){
      return;
    }
  }
  
  xml_password_store = AGS_XML_PASSWORD_STORE(password_store);

  if(xml_password_store->doc == NULL ||
     xml_password_store->root_node == NULL){
    return;
  }

  xml_password_store_mutex = AGS_XML_PASSWORD_STORE_GET_OBJ_MUTEX(xml_password_store);
  
  user_node = NULL;

  xpath = g_strdup_printf("/ags-server-password-store/ags-srv-user-list/ags-srv-user/ags-srv-user-uuid[text() = '%s']",
			  user_uuid);

  g_rec_mutex_lock(xml_password_store_mutex);
    
  xpath_context = xmlXPathNewContext(xml_password_store->doc);
  xpath_object = xmlXPathEval(xpath,
			      xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	user_node = node[i]->parent;

	break;
      }
    }
  }

  if(user_node != NULL){
    password_node = NULL;
    
    child = user_node->children;
    
    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"ags-srv-user-password",
				22)){
	  password_node = child;
	  
	  break;
	}
      }
	
      child = child->next;
    }

    if(password_node == NULL){
      password_node = xmlNewNode(NULL,
				 "ags-srv-user-password");
      xmlAddChild(user_node,
		  password_node);
    }

    xmlNodeSetContent(password_node,
		      password);
  }

  g_rec_mutex_unlock(xml_password_store_mutex);

  xmlXPathFreeObject(xpath_object);
  xmlXPathFreeContext(xpath_context);

  g_free(xpath);
}

gchar*
ags_xml_password_store_get_password(AgsPasswordStore *password_store,
				    GObject *security_context,
				    gchar *user_uuid,
				    gchar *security_token,
				    GError **error)
{
  AgsXmlPasswordStore *xml_password_store;
  
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *user_node;
  xmlNode *password_node;
  xmlNode *child;
  
  gchar *xpath;
  gchar *password;

  guint i;
  
  GRecMutex *xml_password_store_mutex;

  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     user_uuid == NULL){
    return(NULL);
  }    

  if(!AGS_IS_AUTH_SECURITY_CONTEXT(security_context)){
    if(!ags_authentication_manager_is_session_active(ags_authentication_manager_get_instance(),
						     security_context,
						     user_uuid,
						     security_token)){
      return(NULL);
    }
  }

  xml_password_store = AGS_XML_PASSWORD_STORE(password_store);

  if(xml_password_store->doc == NULL ||
     xml_password_store->root_node == NULL){
    return(NULL);
  }

  xml_password_store_mutex = AGS_XML_PASSWORD_STORE_GET_OBJ_MUTEX(xml_password_store);
  
  user_node = NULL;

  xpath = g_strdup_printf("/ags-server-password-store/ags-srv-user-list/ags-srv-user/ags-srv-user-uuid[text() = '%s']",
			  user_uuid);

  g_rec_mutex_lock(xml_password_store_mutex);
    
  xpath_context = xmlXPathNewContext(xml_password_store->doc);
  xpath_object = xmlXPathNodeEval(xml_password_store->root_node,
				  xpath,
				  xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	user_node = node[i]->parent;

	break;
      }
    }
  }

  password = NULL;
  
  if(user_node != NULL){
    password_node = NULL;
    
    child = user_node->children;
    
    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"ags-srv-user-password",
				22)){
	  password_node = child;
	  
	  break;
	}
      }
	
      child = child->next;
    }

    if(password_node != NULL){
      xmlChar *tmp_password;
      
      tmp_password = xmlNodeGetContent(password_node);

      password = g_strdup(tmp_password);

      xmlFree(tmp_password);
    }
  }

  g_rec_mutex_unlock(xml_password_store_mutex);

  xmlXPathFreeObject(xpath_object);
  xmlXPathFreeContext(xpath_context);

  g_free(xpath);

  return(password);
}

gchar*
ags_xml_password_store_encrypt_password(AgsPasswordStore *password_store,
					gchar *password,
					gchar *salt,
					GError **error)
{
#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__DragonFly__) && !defined(AGS_W32API)
  struct crypt_data *data;
#endif

  gchar *password_hash;
  
#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__DragonFly__) && !defined(AGS_W32API)
  data = (struct crypt_data *) malloc(sizeof(struct crypt_data));
  data->initialized = 0;

  password_hash = crypt_r(password, salt,
			  data);
#else
  password_hash = crypt(password, salt);
#endif

  return(password_hash);
}

/**
 * ags_xml_password_store_open_filename:
 * @xml_password_store: the #AgsXmlPasswordStore
 * @filename: the filename
 * 
 * Open @filename.
 * 
 * Since: 3.0.0
 */
void
ags_xml_password_store_open_filename(AgsXmlPasswordStore *xml_password_store,
				     gchar *filename)
{
  xmlDoc *doc;

  GRecMutex *xml_password_store_mutex;

  if(!AGS_IS_XML_PASSWORD_STORE(xml_password_store) ||
     filename == NULL){
    return;
  }    

  xml_password_store_mutex = AGS_XML_PASSWORD_STORE_GET_OBJ_MUTEX(xml_password_store);

  /* open XML */
  doc = xmlReadFile(filename,
		    NULL,
		    0);

  g_rec_mutex_lock(xml_password_store_mutex);

  xml_password_store->filename = g_strdup(filename);

  xml_password_store->doc = doc;
  
  if(doc == NULL){
    g_warning("AgsXmlPasswordStore - failed to read XML document %s", filename);
  }else{
    /* get the root node */
    xml_password_store->root_node = xmlDocGetRootElement(doc);
  }

  g_rec_mutex_unlock(xml_password_store_mutex);
}

/**
 * ags_xml_password_store_find_login:
 * @xml_password_store: the #AgsXmlPasswordStore
 * @login: the login
 * 
 * Find ags-srv-user xmlNode containing @login.
 * 
 * Returns: (transfer none): the matching xmlNode or %NULL
 * 
 * Since: 3.0.0
 */
xmlNode*
ags_xml_password_store_find_login(AgsXmlPasswordStore *xml_password_store,
				  gchar *login)
{
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *user_node;
  
  gchar *xpath;

  guint i;
  
  GRecMutex *xml_password_store_mutex;

  if(!AGS_IS_XML_PASSWORD_STORE(xml_password_store) ||
     login == NULL){
    return(NULL);
  }    

  xml_password_store_mutex = AGS_XML_PASSWORD_STORE_GET_OBJ_MUTEX(xml_password_store);

  g_rec_mutex_lock(xml_password_store_mutex);
  
  /* retrieve user node */
  xpath = g_strdup_printf("(/ags-server-password-store/ags-srv-user-list/ags-srv-user)/ags-srv-user-login[text() = '%s']",
			  login);

  /* Create xpath evaluation context */
  xpath_context = xmlXPathNewContext(xml_password_store->doc);

  if(xpath_context == NULL) {
    g_rec_mutex_unlock(xml_password_store_mutex);
    
    g_warning("Error: unable to create new XPath context");

    return(NULL);
  }

  /* Evaluate xpath expression */
  xpath_object = xmlXPathEval(xpath,
			      xpath_context);

  if(xpath_object == NULL) {
    g_rec_mutex_unlock(xml_password_store_mutex);

    g_warning("Error: unable to evaluate xpath expression");

    xmlXPathFreeContext(xpath_context); 

    return(NULL);
  }

  /* find node */
  node = xpath_object->nodesetval->nodeTab;
  user_node = NULL;
  
  for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
    if(node[i]->type == XML_ELEMENT_NODE){
      user_node = node[i];
      
      break;
    }
  }

  g_rec_mutex_unlock(xml_password_store_mutex);

  /* free xpath and return */
  g_free(xpath);

  return(user_node);
}

/**
 * ags_xml_password_store_new:
 *
 * Create #AgsXmlPasswordStore.
 *
 * Returns: the new #AgsXmlPasswordStore instance
 *
 * Since: 2.0.0
 */
AgsXmlPasswordStore*
ags_xml_password_store_new()
{
  AgsXmlPasswordStore *xml_password_store;

  xml_password_store = (AgsXmlPasswordStore *) g_object_new(AGS_TYPE_XML_PASSWORD_STORE,
							    NULL);

  return(xml_password_store);
}
