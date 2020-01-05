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

#include <ags/server/security/ags_xml_authentication.h>

#include <ags/lib/ags_uuid.h>

#include <ags/server/security/ags_authentication_manager.h>
#include <ags/server/security/ags_business_group_manager.h>
#include <ags/server/security/ags_password_store_manager.h>
#include <ags/server/security/ags_authentication.h>
#include <ags/server/security/ags_business_group.h>
#include <ags/server/security/ags_xml_business_group.h>
#include <ags/server/security/ags_xml_password_store.h>
#include <ags/server/security/ags_auth_security_context.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

void ags_xml_authentication_class_init(AgsXmlAuthenticationClass *xml_authentication);
void ags_xml_authentication_authentication_interface_init(AgsAuthenticationInterface *authentication);
void ags_xml_authentication_init(AgsXmlAuthentication *xml_authentication);
void ags_xml_authentication_finalize(GObject *gobject);

gchar** ags_xml_authentication_get_authentication_module(AgsAuthentication *authentication);
gboolean ags_xml_authentication_login(AgsAuthentication *authentication,
				      gchar *login,
				      gchar *password,
				      gchar **user_uuid,
				      gchar **security_token,
				      GError **error);
gboolean ags_xml_authentication_logout(AgsAuthentication *authentication,
				       GObject *security_context,
				       gchar *login,
				       gchar *security_token,
				       GError **error); 
gchar* ags_xml_authentication_generate_token(AgsAuthentication *authentication,
					     GError **error);
gchar* ags_xml_authentication_get_digest(AgsAuthentication *authentication,
					 gchar *realm,
					 gchar *login,
					 gchar *security_token,
					 GError **error);
gboolean ags_xml_authentication_is_session_active(AgsAuthentication *authentication,
						  GObject *security_context,
						  gchar *user_uuid,
						  gchar *security_token,
						  GError **error);

/**
 * SECTION:ags_xml_authentication
 * @short_description: authentication by XML file
 * @title: AgsXmlAuthentication
 * @section_id:
 * @include: ags/server/security/ags_xml_authentication.h
 *
 * The #AgsXmlAuthentication is an object to authenticate to Advanced Gtk+ Sequencer's
 * server.
 */

static gpointer ags_xml_authentication_parent_class = NULL;

GType
ags_xml_authentication_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_xml_authentication = 0;

    static const GTypeInfo ags_xml_authentication_info = {
      sizeof (AgsXmlAuthenticationClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_xml_authentication_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsXmlAuthentication),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_xml_authentication_init,
    };

    static const GInterfaceInfo ags_authentication_interface_info = {
      (GInterfaceInitFunc) ags_xml_authentication_authentication_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_xml_authentication = g_type_register_static(G_TYPE_OBJECT,
							 "AgsXmlAuthentication",
							 &ags_xml_authentication_info,
							 0);

    g_type_add_interface_static(ags_type_xml_authentication,
				AGS_TYPE_AUTHENTICATION,
				&ags_authentication_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_xml_authentication);
  }

  return g_define_type_id__volatile;
}

void
ags_xml_authentication_class_init(AgsXmlAuthenticationClass *xml_authentication)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_xml_authentication_parent_class = g_type_class_peek_parent(xml_authentication);

  /* GObjectClass */
  gobject = (GObjectClass *) xml_authentication;

  gobject->finalize = ags_xml_authentication_finalize;
}

void
ags_xml_authentication_authentication_interface_init(AgsAuthenticationInterface *authentication)
{
  authentication->get_authentication_module = ags_xml_authentication_get_authentication_module;
  
  authentication->login = ags_xml_authentication_login;
  
  authentication->logout = ags_xml_authentication_logout;
  
  authentication->generate_token = ags_xml_authentication_generate_token;
  
  authentication->get_digest = ags_xml_authentication_get_digest;
  
  authentication->is_session_active = ags_xml_authentication_is_session_active;
}

void
ags_xml_authentication_init(AgsXmlAuthentication *xml_authentication)
{
  g_rec_mutex_init(&(xml_authentication->obj_mutex));

  xml_authentication->filename = NULL;
  xml_authentication->encoding = NULL;
  xml_authentication->dtd = NULL;

  xml_authentication->doc = NULL;
  xml_authentication->root_node = NULL;
}

void
ags_xml_authentication_finalize(GObject *gobject)
{
  AgsXmlAuthentication *xml_authentication;

  xml_authentication = AGS_XML_AUTHENTICATION(gobject);

  g_free(xml_authentication->filename);
  g_free(xml_authentication->encoding);
  g_free(xml_authentication->dtd);
  
  if(xml_authentication->doc != NULL){
    xmlFreeDoc(xml_authentication->doc);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_xml_authentication_parent_class)->finalize(gobject);
}

gchar**
ags_xml_authentication_get_authentication_module(AgsAuthentication *authentication)
{
  static gchar **authentication_module = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(authentication_module == NULL){
    authentication_module = (gchar **) malloc(4 * sizeof(gchar *));

    authentication_module[0] = "ags-basic-authentication";
    authentication_module[1] = "ags-digest-authentication";
    authentication_module[2] = "ags-xml-authentication";
    authentication_module[3] = NULL;
  }
  
  g_mutex_unlock(&mutex);

  return(authentication_module);
}

gboolean
ags_xml_authentication_login(AgsAuthentication *authentication,
			     gchar *login,
			     gchar *password,
			     gchar **user_uuid,
			     gchar **security_token,
			     GError **error)
{
  AgsAuthenticationManager *authentication_manager;
  AgsPasswordStoreManager *password_store_manager;
  AgsXmlAuthentication *xml_authentication;
  AgsXmlPasswordStore *xml_password_store;
  AgsSecurityContext *security_context;
  
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *auth_node;
  xmlNode *user_node;
  xmlNode *child;
  
  GList *start_password_store, *password_store;
  
  gchar *current_user_uuid;
  gchar *current_security_token;
  gchar *xpath;

  guint i;
  gboolean success;

  GRecMutex *xml_authentication_mutex;
  GRecMutex *xml_password_store_mutex;
  
  if(user_uuid != NULL){
    user_uuid[0] = NULL;
  }

  if(security_token != NULL){
    security_token[0] = NULL;
  }
  
  if(login == NULL ||
     password == NULL){
    return(FALSE);
  }    
  
  xml_authentication = AGS_XML_AUTHENTICATION(authentication);

  if(xml_authentication->doc == NULL ||
     xml_authentication->root_node == NULL){
    return(FALSE);
  }

  password_store_manager = ags_password_store_manager_get_instance();

  security_context = NULL;

  current_user_uuid = NULL;
  current_security_token = NULL;

  /* password store */
  xml_password_store = NULL;
    
  password_store =
    start_password_store = ags_password_store_manager_get_password_store(password_store_manager);

  while(password_store != NULL){
    if(AGS_IS_XML_PASSWORD_STORE(password_store->data)){
      xml_password_store = password_store->data;
	
      break;
    }
      
    password_store = password_store->next;
  }

  /* uuid */
  user_node = NULL;

  if(xml_password_store != NULL){
    xml_password_store_mutex = AGS_XML_PASSWORD_STORE_GET_OBJ_MUTEX(xml_password_store);
      
    xpath = g_strdup_printf("/ags-server-password-store/ags-srv-user-list/ags-srv-user/ags-srv-user-login[text() = '%s']",
			    login);

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
  }
    
  g_list_free_full(start_password_store,
		   g_object_unref);

  if(user_node != NULL){
    g_rec_mutex_lock(xml_password_store_mutex);

    child = user_node->children;

    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"ags-srv-user-uuid",
				18)){
	  xmlChar *tmp_user_uuid;
	  
	  tmp_user_uuid = xmlNodeGetContent(child);

	  current_user_uuid = g_strdup(tmp_user_uuid);

	  xmlFree(tmp_user_uuid);
	  
	  break;
	}
      }
	
      child = child->next;
    }
      
    g_rec_mutex_unlock(xml_password_store_mutex);
  }

  success = FALSE;
  
  if(current_user_uuid != NULL){
    success = ags_password_store_manager_check_password(password_store_manager,
							current_user_uuid,
							password);
  }
  
  if(success){
    /* session */
    auth_node = NULL;
    
    xml_authentication_mutex = AGS_XML_AUTHENTICATION_GET_OBJ_MUTEX(xml_authentication);

    xpath = g_strdup_printf("/ags-server-authentication/ags-srv-auth-list/ags-srv-auth/ags-srv-user-uuid[text() = '%s']",
			    current_user_uuid);
    
    g_rec_mutex_lock(xml_authentication_mutex);
    
    xpath_context = xmlXPathNewContext(xml_authentication->doc);
    xpath_object = xmlXPathEval(xpath,
				xpath_context);

    if(xpath_object->nodesetval != NULL){
      node = xpath_object->nodesetval->nodeTab;

      for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
	if(node[i]->type == XML_ELEMENT_NODE){
	  auth_node = node[i]->parent;

	  break;
	}
      }
    }

    g_rec_mutex_unlock(xml_authentication_mutex);

    xmlXPathFreeObject(xpath_object);
    xmlXPathFreeContext(xpath_context);

    g_free(xpath);

    /* login info */
    if(auth_node != NULL){
      AgsLoginInfo *login_info;

      xmlNode *auth_group_list_node;
      
      authentication_manager = ags_authentication_manager_get_instance();

      /* get business group */
      auth_group_list_node = NULL;

      child = auth_node->children;

      while(child != NULL){
	if(child->type == XML_ELEMENT_NODE){
	  if(!g_ascii_strncasecmp(child->name,
				  "ags-srv-auth-group-list",
				  24)){
	    auth_group_list_node = child;

	    break;
	  }
	}
	
	child = child->next;
      }

      /* login info */
      login_info = ags_authentication_manager_lookup_login(authentication_manager,
							   login);

      if(login_info == NULL){
	AgsBusinessGroupManager *business_group_manager;

	GList *start_business_group, *business_group;

	business_group_manager = ags_business_group_manager_get_instance();

	/* login info */
	login_info = ags_login_info_alloc();

	login_info->active_session_count = 1;

	login_info->user_uuid = g_strdup(current_user_uuid);

	/* security context */
	security_context = 
	  login_info->security_context = ags_security_context_new();

	/* parse business group */
	business_group =
	  start_business_group = ags_business_group_manager_get_business_group(business_group_manager);
	
	while(business_group != NULL){
	  if(AGS_IS_XML_BUSINESS_GROUP(business_group->data)){
	    GRecMutex *xml_business_group_mutex;	    
	
	    xml_business_group_mutex = AGS_XML_BUSINESS_GROUP_GET_OBJ_MUTEX(business_group->data);

	    g_rec_mutex_lock(xml_business_group_mutex);
	    
	    ags_security_context_parse_business_group(login_info->security_context,
						      AGS_XML_BUSINESS_GROUP(business_group->data)->doc,
						      current_user_uuid);

	    g_rec_mutex_unlock(xml_business_group_mutex);
	  }
	  
	  business_group = business_group->next;
	}

	/* insert login */
	ags_authentication_manager_insert_login(authentication_manager,
						login,
						login_info);

	ags_login_info_unref(login_info);
      }else{
	login_info->active_session_count += 1;
      }      
    }

    /* session */
    if(auth_node != NULL){
      xmlNode *session_list_node;
      xmlNode *group_list_node;
      xmlNode *session_node;
      xmlNode *group_node;

      gchar **business_group;
      
      session_list_node = NULL;
      session_node = NULL;
      
      g_rec_mutex_lock(xml_authentication_mutex);

      child = auth_node->children;

      while(child != NULL){
	if(child->type == XML_ELEMENT_NODE){
	  if(!g_ascii_strncasecmp(child->name,
				  "ags-srv-auth-session-list",
				  26)){
	    session_list_node = child;

	    break;
	  }
	}
	
	child = child->next;
      }

      if(session_list_node == NULL){
	session_list_node = xmlNewNode(NULL,
				       "ags-srv-auth-session-list");
	xmlAddChild(auth_node,
		    session_list_node);	
      }

      if(session_list_node != NULL){
	session_node = xmlNewNode(NULL,
				  "ags-srv-auth-session");
	xmlAddChild(session_list_node,
		    session_node);

	current_security_token = g_uuid_string_random();

	xmlNodeSetContent(session_node,
			  current_security_token);
      }

      /* session */
      if(session_node != NULL){
	GDateTime *date_time;

	gchar *str;
	
	date_time = g_date_time_new_now_utc();
	
	str = g_date_time_format_iso8601(date_time);
	
	xmlNewProp(session_node,
		   "last-active",
		   str);

	g_date_time_unref(date_time);
	
	g_free(str);
      }
      
      g_rec_mutex_unlock(xml_authentication_mutex);

      /* persist groups */
      group_list_node = NULL;
      group_node = NULL;

      business_group = ags_security_context_get_business_group(security_context);

      if(business_group != NULL){      
	g_rec_mutex_lock(xml_authentication_mutex);
	
	child = auth_node->children;

	while(child != NULL){
	  if(child->type == XML_ELEMENT_NODE){
	    if(!g_ascii_strncasecmp(child->name,
				    "ags-srv-auth-group-list",
				    26)){
	      group_list_node = child;

	      break;
	    }
	  }
	
	  child = child->next;
	}

	if(group_list_node == NULL){
	  group_list_node = xmlNewNode(NULL,
				       "ags-srv-auth-group-list");
	  xmlAddChild(auth_node,
		      group_list_node);	
	}

	if(group_list_node != NULL){
	  gchar **iter;

	  iter = business_group;

	  for(; iter[0] != NULL; iter++){
	    group_node = xmlNewNode(NULL,
				    "ags-srv-auth-group");
	    xmlAddChild(group_list_node,
			group_node);
	  
	    xmlNodeSetContent(group_node,
			      iter[0]);
	  }
	}
      
	g_rec_mutex_unlock(xml_authentication_mutex);

	g_strfreev(business_group);
      }
    }
  }

  if(current_user_uuid == NULL ||
     current_security_token == NULL){
    success = FALSE;
  }
  
  if(user_uuid != NULL){
    user_uuid[0] = current_user_uuid;
  }

  if(security_token != NULL){
    security_token[0] = current_security_token;
  }
  
  return(success);
}

gboolean
ags_xml_authentication_logout(AgsAuthentication *authentication,
			      GObject *security_context,
			      gchar *login,
			      gchar *security_token,
			      GError **error)
{
  AgsAuthenticationManager *authentication_manager;
  AgsPasswordStoreManager *password_store_manager;
  AgsXmlAuthentication *xml_authentication;

  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *auth_node;
  xmlNode *child;

  AgsLoginInfo *login_info;

  GList *start_password_store, *password_store;

  gchar *current_user_uuid;
  gchar *xpath;

  guint i;
  gboolean is_session_active;
  
  GRecMutex *authentication_manager_mutex;
  GRecMutex *xml_authentication_mutex;

  if(login == NULL ||
     security_token == NULL){
    return(FALSE);
  }

  xml_authentication = AGS_XML_AUTHENTICATION(authentication);

  if(xml_authentication->doc == NULL ||
     xml_authentication->root_node == NULL){
    return(FALSE);
  }

  xml_authentication_mutex = AGS_XML_AUTHENTICATION_GET_OBJ_MUTEX(xml_authentication);
  
  current_user_uuid = NULL;

  is_session_active = FALSE;
  
  authentication_manager = ags_authentication_manager_get_instance();
  
  authentication_manager_mutex = AGS_AUTHENTICATION_MANAGER_GET_OBJ_MUTEX(authentication_manager);

  /* get login info */
  login_info = ags_authentication_manager_lookup_login(authentication_manager,
						       login);

  if(login_info != NULL){
    g_rec_mutex_lock(authentication_manager_mutex);
  
    current_user_uuid = g_strdup(login_info->user_uuid);
    
    g_rec_mutex_unlock(authentication_manager_mutex);
  }else{
    return(FALSE);
  }
    
  is_session_active = ags_authentication_manager_is_session_active(authentication_manager,
								   security_context,
								   current_user_uuid,
								   security_token);
  
  if(!is_session_active){
    g_free(current_user_uuid);
    
    return(FALSE);
  }
  
  /* session */
  auth_node = NULL;
    
  xml_authentication_mutex = AGS_XML_AUTHENTICATION_GET_OBJ_MUTEX(xml_authentication);
    
  xpath = g_strdup_printf("/ags-server-authentication/ags-srv-auth-list/ags-srv-auth/ags-srv-user-uuid[text() = '%s']",
			  current_user_uuid);
    
  g_rec_mutex_lock(xml_authentication_mutex);
    
  xpath_context = xmlXPathNewContext(xml_authentication->doc);
  xpath_object = xmlXPathEval(xpath,
			      xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	auth_node = node[i]->parent;

	break;
      }
    }
  }

  g_rec_mutex_unlock(xml_authentication_mutex);

  xmlXPathFreeObject(xpath_object);
  xmlXPathFreeContext(xpath_context);

  g_free(xpath);

  if(auth_node != NULL){
    xmlNode *session_list_node;
    xmlNode *session_node;

    session_list_node = NULL;
    session_node = NULL;
      
    g_rec_mutex_lock(xml_authentication_mutex);

    /* find session list */
    child = auth_node->children;

    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"ags-srv-auth-session-list",
				26)){
	  session_list_node = child;

	  break;
	}
      }
	
      child = child->next;
    }

    /* find session */
    if(session_list_node != NULL){	
      child = session_list_node->children;

      while(child != NULL){
	if(child->type == XML_ELEMENT_NODE){
	  if(!g_ascii_strncasecmp(child->name,
				  "ags-srv-auth-session",
				  21)){
	    xmlChar *current_security_token;

	    current_security_token = xmlNodeGetContent(child);

	    if(!g_strcmp0(security_token,
			  current_security_token)){
	      session_node = child;
	    }

	    xmlFree(current_security_token);

	    if(session_node != NULL){		
	      break;
	    }
	  }
	}
	
	child = child->next;
      }
    }

    if(session_node != NULL){
      xmlUnlinkNode(session_node);
      xmlFreeNode(session_node);
    }
      
    g_rec_mutex_unlock(xml_authentication_mutex);

    /* login info - decrement active session count */
    g_rec_mutex_lock(authentication_manager_mutex);
	
    login_info->active_session_count -= 1;

    if(login_info->active_session_count <= 0){
      ags_authentication_manager_remove_login(authentication_manager,
					      login);
    }

    g_rec_mutex_unlock(authentication_manager_mutex);
  }

  ags_login_info_unref(login_info);
  
  g_free(current_user_uuid);

  return(is_session_active);
}

gchar*
ags_xml_authentication_generate_token(AgsAuthentication *authentication,
				      GError **error)
{
  return(g_uuid_string_random());
}

gchar*
ags_xml_authentication_get_digest(AgsAuthentication *authentication,
				  gchar *realm,
				  gchar *login,
				  gchar *security_token,
				  GError **error)
{
  AgsPasswordStoreManager *password_store_manager;
  AgsXmlAuthentication *xml_authentication;
  AgsXmlPasswordStore *xml_password_store;

  GList *start_password_store, *password_store;

  gchar *password;
  gchar *digest;

  GError *this_error;
  
  if(realm == NULL ||
     login == NULL ||
     security_token == NULL){
    return(NULL);
  }
  
  xml_authentication = AGS_XML_AUTHENTICATION(authentication);

  password_store_manager = ags_password_store_manager_get_instance();

  /* password store */
  xml_password_store = NULL;
    
  password_store =
    start_password_store = ags_password_store_manager_get_password_store(password_store_manager);

  while(password_store != NULL){
    if(AGS_IS_XML_PASSWORD_STORE(password_store->data)){
      xml_password_store = password_store->data;
	
      break;
    }
      
    password_store = password_store->next;
  }

  digest = NULL;

  if(xml_password_store != NULL){
    this_error = NULL;
    password = ags_password_store_get_password(AGS_PASSWORD_STORE(xml_password_store),
					       NULL,
					       login,
					       security_token,
					       &this_error);

    if(this_error != NULL){
      g_warning("%s", this_error->message);
    
      g_error_free(this_error);
    }
  
    digest = soup_auth_domain_digest_encode_password(login,
						     realm,
						     password);

    g_free(password);
  }
  
  return(digest);
}

gboolean
ags_xml_authentication_is_session_active(AgsAuthentication *authentication,
					 GObject *security_context,
					 gchar *user_uuid,
					 gchar *security_token,
					 GError **error)
{
  AgsXmlAuthentication *xml_authentication;
  AgsAuthenticationManager *authentication_manager;

  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *auth_node;
  xmlNode *child;

  GList *start_password_store, *password_store;

  gchar *xpath;
  
  guint i;
  gboolean success;
  
  GRecMutex *authentication_manager_mutex;
  GRecMutex *xml_authentication_mutex;

  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     user_uuid == NULL ||
     security_token == NULL){
    return(FALSE);
  }

  xml_authentication = AGS_XML_AUTHENTICATION(authentication);

  if(xml_authentication->doc == NULL ||
     xml_authentication->root_node == NULL){
    return(FALSE);
  }

  xml_authentication_mutex = AGS_XML_AUTHENTICATION_GET_OBJ_MUTEX(xml_authentication);
  
  success = FALSE;
  
  authentication_manager = ags_authentication_manager_get_instance();

  authentication_manager_mutex = AGS_AUTHENTICATION_MANAGER_GET_OBJ_MUTEX(authentication_manager);
    
  auth_node = NULL;
  
  xpath = g_strdup_printf("/ags-server-authentication/ags-srv-auth-list/ags-srv-auth/ags-srv-user-uuid[text() = '%s']",
			  user_uuid);
    
  g_rec_mutex_lock(xml_authentication_mutex);
    
  xpath_context = xmlXPathNewContext(xml_authentication->doc);
  xpath_object = xmlXPathEval(xpath,
			      xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	auth_node = node[i]->parent;

	break;
      }
    }
  }

  g_rec_mutex_unlock(xml_authentication_mutex);

  xmlXPathFreeObject(xpath_object);
  xmlXPathFreeContext(xpath_context);

  g_free(xpath);

  if(auth_node != NULL){
    xmlNode *session_list_node;
    xmlNode *session_node;

    session_list_node = NULL;
    session_node = NULL;
      
    g_rec_mutex_lock(xml_authentication_mutex);

    /* find session list */
    child = auth_node->children;

    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"ags-srv-auth-session-list",
				26)){
	  session_list_node = child;

	  break;
	}
      }
	
      child = child->next;
    }

    /* find session */
    if(session_list_node != NULL){	
      child = session_list_node->children;

      while(child != NULL){
	if(child->type == XML_ELEMENT_NODE){
	  if(!g_ascii_strncasecmp(child->name,
				  "ags-srv-auth-session",
				  21)){
	    xmlChar *current_security_token;

	    current_security_token = xmlNodeGetContent(child);

	    if(!g_strcmp0(security_token,
			  current_security_token)){
	      session_node = child;
	    }

	    xmlFree(current_security_token);

	    if(session_node != NULL){		
	      break;
	    }
	  }
	}
	
	child = child->next;
      }
    }

    if(session_node != NULL){
      GDateTime *date_time, *last_active;

      gchar *str;

      gint64 session_timeout;
      
      date_time = g_date_time_new_now_utc();

      str = xmlGetProp(session_node,
		       "last-active");

      last_active = g_date_time_new_from_iso8601(str,
						 NULL);

      xmlFree(str);

      session_timeout = ags_authentication_manager_get_session_timeout(authentication_manager);
      
      if(g_date_time_to_unix(last_active) + session_timeout > g_date_time_to_unix(date_time)){
	success = TRUE;
      }

      g_date_time_unref(date_time);
      g_date_time_unref(last_active);
    }
  }

  return(success);
}

/**
 * ags_xml_authentication_open_filename:
 * @xml_authentication: the #AgsXmlAuthentication
 * @filename: the filename
 * 
 * Open @filename.
 * 
 * Since: 3.0.0
 */
void
ags_xml_authentication_open_filename(AgsXmlAuthentication *xml_authentication,
				     gchar *filename)
{
  xmlDoc *doc;

  GRecMutex *xml_authentication_mutex;

  if(!AGS_IS_XML_AUTHENTICATION(xml_authentication) ||
     filename == NULL){
    return;
  }
  
  xml_authentication_mutex = AGS_XML_AUTHENTICATION_GET_OBJ_MUTEX(xml_authentication);

  /* open XML */
  doc = xmlReadFile(filename,
		    NULL,
		    0);

  g_rec_mutex_lock(xml_authentication_mutex);

  xml_authentication->filename = g_strdup(filename);

  xml_authentication->doc = doc;
  
  if(doc == NULL){
    g_warning("AgsXmlAuthentication - failed to read XML document %s", filename);
  }else{
    /* get the root node */
    xml_authentication->root_node = xmlDocGetRootElement(doc);
  }

  g_rec_mutex_unlock(xml_authentication_mutex);
}

/**
 * ags_xml_authentication_find_user_uuid:
 * @xml_authentication: the #AgsXmlAuthentication
 * @user_uuid: the user UUID
 * 
 * Find ags-srv-auth xmlNode containing @user_uuid.
 * 
 * Returns: (transfer none): the matching xmlNode or %NULL
 * 
 * Since: 3.0.0
 */
xmlNode*
ags_xml_authentication_find_user_uuid(AgsXmlAuthentication *xml_authentication,
				      gchar *user_uuid)
{
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *auth_node;
  
  gchar *xpath;

  guint i;

  GRecMutex *xml_authentication_mutex;
  
  if(!AGS_IS_XML_AUTHENTICATION(xml_authentication) ||
     user_uuid == NULL){
    return(NULL);
  }    

  xml_authentication_mutex = AGS_XML_AUTHENTICATION_GET_OBJ_MUTEX(xml_authentication);

  /* retrieve auth node */
  xpath = g_strdup_printf("(/ags-server-authentication/ags-srv-auth-list/ags-srv-auth)/ags-srv-user-uuid[content()='%s']",
			  user_uuid);

  g_rec_mutex_lock(xml_authentication_mutex);

  /* Create xpath evaluation context */
  xpath_context = xmlXPathNewContext(xml_authentication->doc);

  if(xpath_context == NULL){
    g_rec_mutex_unlock(xml_authentication_mutex);

    g_warning("Error: unable to create new XPath context");

    return(NULL);
  }

  /* Evaluate xpath expression */
  xpath_object = xmlXPathEval(xpath,
			      xpath_context);

  if(xpath_object == NULL){
    g_rec_mutex_unlock(xml_authentication_mutex);

    g_warning("Error: unable to evaluate xpath expression");

    xmlXPathFreeContext(xpath_context); 

    return(NULL);
  }

  /* find node */
  node = xpath_object->nodesetval->nodeTab;
  auth_node = NULL;
  
  for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
    if(node[i]->type == XML_ELEMENT_NODE){
      auth_node = node[i];
      
      break;
    }
  }

  g_rec_mutex_unlock(xml_authentication_mutex);

  /* free xpath and return */
  g_free(xpath);

  return(auth_node);
}

/**
 * ags_xml_authentication_new:
 *
 * Create #AgsXmlAuthentication.
 *
 * Returns: the new #AgsXmlAuthentication instance
 *
 * Since: 3.0.0
 */
AgsXmlAuthentication*
ags_xml_authentication_new()
{
  AgsXmlAuthentication *xml_authentication;

  xml_authentication = (AgsXmlAuthentication *) g_object_new(AGS_TYPE_XML_AUTHENTICATION,
							     NULL);

  return(xml_authentication);
}
