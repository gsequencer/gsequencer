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

#include <ags/server/security/ags_xml_password_store.h>

#include <ags/server/security/ags_password_store.h>

#define __USE_GNU
#define _GNU_SOURCE
#include <crypt.h>
#include <unistd.h>

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
  static GType ags_type_xml_password_store = 0;

  if(!ags_type_xml_password_store){
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
							 "AgsXmlPasswordStore\0",
							 &ags_xml_password_store_info,
							 0);

    g_type_add_interface_static(ags_type_xml_password_store,
				AGS_TYPE_PASSWORD_STORE,
				&ags_password_store_interface_info);
  }

  return (ags_type_xml_password_store);
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

  G_OBJECT_CLASS(ags_xml_password_store_parent_class)->finalize(gobject);
}

void
ags_xml_password_store_set_login_name(AgsPasswordStore *password_store,
				      GObject *security_context,
				      gchar *user_uuid,
				      gchar *security_token,
				      gchar *login_name,
				      GError **error)
{
  //TODO:JK: implement me
}

gchar*
ags_xml_password_store_get_login_name(AgsPasswordStore *password_store,
				      GObject *security_context,
				      gchar *user_uuid,
				      gchar *security_token,
				      GError **error)
{
  //TODO:JK: implement me
}

void
ags_xml_password_store_set_password(AgsPasswordStore *password_store,
				    GObject *security_context,
				    gchar *user,
				    gchar *security_token,
				    gchar *password,
				    GError **error)
{
  //TODO:JK: implement me
}

gchar*
ags_xml_password_store_get_password(AgsPasswordStore *password_store,
				    GObject *security_context,
				    gchar *user,
				    gchar *security_token,
				    GError **error)
{
  //TODO:JK: implement me
}

gchar*
ags_xml_password_store_encrypt_password(AgsPasswordStore *password_store,
					gchar *password,
					gchar *salt,
					GError **error)
{
  struct crypt_data *data;

  gchar *password_hash;
  
  data = (struct crypt_data *) malloc(sizeof(struct crypt_data));
  data->initialized = 0;

  password_hash = crypt_r(password, salt,
			  data);

  return(password_hash);
}

xmlNode*
ags_xml_password_store_find_login(AgsXmlPasswordStore *xml_password_store,
				  gchar *login)
{
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *user_node;
  
  xmlChar *xpath;

  guint i;
  
  if(!AGS_IS_XML_PASSWORD_STORE(xml_password_store) ||
     login == NULL){
    return(NULL);
  }    

  /* retrieve user node */
  xpath = g_strdup_printf("(//ags-srv-user)/ags-srv-user-login[content()='%s']",
			  login);

  /* Create xpath evaluation context */
  xpath_context = xmlXPathNewContext(xml_password_store->doc);

  if(xpath_context == NULL) {
    g_warning("Error: unable to create new XPath context\0");

    return(NULL);
  }

  /* Evaluate xpath expression */
  xpath_object = xmlXPathEval(xpath,
			      xpath_context);

  if(xpath_object == NULL) {
    g_warning("Error: unable to evaluate xpath expression\0");
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
 * Since: 1.0.0
 */
AgsXmlPasswordStore*
ags_xml_password_store_new()
{
  AgsXmlPasswordStore *xml_password_store;

  xml_password_store = (AgsXmlPasswordStore *) g_object_new(AGS_TYPE_XML_PASSWORD_STORE,
							    NULL);

  return(xml_password_store);
}
