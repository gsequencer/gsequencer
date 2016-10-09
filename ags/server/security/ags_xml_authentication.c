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

#include <ags/server/security/ags_xml_authentication.h>

#include <ags/server/security/ags_authentication.h>

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

gchar* ags_xml_authentication_login(AgsAuthentication *authentication,
				    gchar *login, gchar *password,
				    GError **error);
gboolean ags_xml_authentication_logout(AgsAuthentication *authentication,
				       gchar *login, gchar *security_token,
				       GError **error); 
gchar* ags_xml_authentication_generate_token(AgsAuthentication *authentication,
					     GError **error);
gchar** ags_xml_authentication_get_groups(AgsAuthentication *authentication,
					  gchar *login, gchar *security_token,
					  GError **error);
gchar* ags_xml_authentication_get_permission(AgsAuthentication *authentication,
					     gchar *login, gchar *security_token,
					     gchar *group_name,
					     GError **error);
gboolean ags_xml_authentication_is_session_active(AgsAuthentication *authentication,
						  gchar *login, gchar *security_token,
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
  static GType ags_type_xml_authentication = 0;

  if(!ags_type_xml_authentication){
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
							 "AgsXmlAuthentication\0",
							 &ags_xml_authentication_info,
							 0);

    g_type_add_interface_static(ags_type_xml_authentication,
				AGS_TYPE_AUTHENTICATION,
				&ags_authentication_interface_info);
  }

  return (ags_type_xml_authentication);
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
  authentication->login = ags_xml_authentication_login;
  
  authentication->logout = ags_xml_authentication_logout;
  
  authentication->generate_token = ags_xml_authentication_generate_token;
  
  authentication->get_groups = ags_xml_authentication_get_groups;
  
  authentication->get_permission = ags_xml_authentication_get_permission;
  
  authentication->is_session_active = ags_xml_authentication_is_session_active;
}

void
ags_xml_authentication_init(AgsXmlAuthentication *xml_authentication)
{
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

  G_OBJECT_CLASS(ags_xml_authentication_parent_class)->finalize(gobject);
}

gchar*
ags_xml_authentication_login(AgsAuthentication *authentication,
			     gchar *login, gchar *password,
			     GError **error)
{
  AgsXmlAuthentication *xml_authentication;

  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *auth_node;
  xmlNode *user_node;
  
  gchar *security_token;
  xmlChar *xpath;

  guint i;
  
  if(login == NULL ||
     password == NULL){

    return(NULL);
  }    
  
  xml_authentication = AGS_XML_AUTHENTICATION(authentication);

  security_token = NULL;


  return(security_token);
}

gboolean
ags_xml_authentication_logout(AgsAuthentication *authentication,
			      gchar *login, gchar *security_token,
			      GError **error)
{
  //TODO:JK: implement me
}

gchar*
ags_xml_authentication_generate_token(AgsAuthentication *authentication,
				      GError **error)
{
  //TODO:JK: implement me
}

gchar**
ags_xml_authentication_get_groups(AgsAuthentication *authentication,
				  gchar *login, gchar *security_token,
				  GError **error)
{
  //TODO:JK: implement me
}

gchar*
ags_xml_authentication_get_permission(AgsAuthentication *authentication,
				      gchar *login, gchar *security_token,
				      gchar *group_name,
				      GError **error)
{
  //TODO:JK: implement me
}

gboolean
ags_xml_authentication_is_session_active(AgsAuthentication *authentication,
					 gchar *login, gchar *security_token,
					 GError **error)
{
  //TODO:JK: implement me
}

xmlNode*
ags_xml_authentication_find_user_uuid(AgsXmlAuthentication *xml_authentication,
				      gchar *user_uuid)
{
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *auth_node;
  
  xmlChar *xpath;

  guint i;
  
  if(!AGS_IS_XML_AUTHENTICATION(xml_authentication) ||
     user_uuid == NULL){
    return(NULL);
  }    

  /* retrieve auth node */
  xpath = g_strdup_printf("(//ags-srv-auth)/ags-srv-user-uuid[content()='%s']",
			  user_uuid);

  /* Create xpath evaluation context */
  xpath_context = xmlXPathNewContext(xml_authentication->doc);

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
  auth_node = NULL;
  
  for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
    if(node[i]->type == XML_ELEMENT_NODE){
      auth_node = node[i];
      
      break;
    }
  }

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
 * Since: 1.0.0
 */
AgsXmlAuthentication*
ags_xml_authentication_new()
{
  AgsXmlAuthentication *xml_authentication;

  xml_authentication = (AgsXmlAuthentication *) g_object_new(AGS_TYPE_XML_AUTHENTICATION,
							     NULL);

  return(xml_authentication);
}
