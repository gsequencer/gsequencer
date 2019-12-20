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

#include <ags/server/security/ags_xml_certificate.h>

#include <ags/server/security/ags_authentication_manager.h>
#include <ags/server/security/ags_certificate.h>
#include <ags/server/security/ags_auth_security_context.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

void ags_xml_certificate_class_init(AgsXmlCertificateClass *xml_certificate);
void ags_xml_certificate_certificate_interface_init(AgsCertificateInterface *certificate);
void ags_xml_certificate_init(AgsXmlCertificate *xml_certificate);
void ags_xml_certificate_finalize(GObject *gobject);

gchar** ags_xml_certificate_get_cert_uuid(AgsCertificate *certificate,
					  GObject *security_context,
					  gchar *user_uuid,
					  gchar *security_token,
					  GError **error);
void ags_xml_certificate_set_domain(AgsCertificate *certificate,
				    GObject *security_context,
				    gchar *user_uuid,
				    gchar *security_token,
				    gchar *cert_uuid,
				    gchar *domain,
				    GError **error);
gchar* ags_xml_certificate_get_domain(AgsCertificate *certificate,
				      GObject *security_context,
				      gchar *user_uuid,
				      gchar *security_token,
				      gchar *cert_uuid,
				      GError **error);
void ags_xml_certificate_set_key_type(AgsCertificate *certificate,
				      GObject *security_context,
				      gchar *user_uuid,
				      gchar *security_token,
				      gchar *cert_uuid,
				      gchar *key_type,
				      GError **error);
gchar* ags_xml_certificate_get_key_type(AgsCertificate *certificate,
					GObject *security_context,
					gchar *user_uuid,
					gchar *security_token,
					gchar *cert_uuid,
					GError **error);
void ags_xml_certificate_set_public_key_file(AgsCertificate *certificate,
					     GObject *security_context,
					     gchar *user_uuid,
					     gchar *security_token,
					     gchar *cert_uuid,
					     gchar *public_key_file,
					     GError **error);
gchar* ags_xml_certificate_get_public_key_file(AgsCertificate *certificate,
					       GObject *security_context,
					       gchar *user_uuid,
					       gchar *security_token,
					       gchar *cert_uuid,
					       GError **error);
void ags_xml_certificate_set_private_key_file(AgsCertificate *certificate,
					      GObject *security_context,
					      gchar *user_uuid,
					      gchar *security_token,
					      gchar *cert_uuid,
					      gchar *private_key_file,
					      GError **error);
gchar* ags_xml_certificate_get_private_key_file(AgsCertificate *certificate,
						GObject *security_context,
						gchar *user_uuid,
						gchar *security_token,
						gchar *cert_uuid,
						GError **error);

/**
 * SECTION:ags_xml_certificate
 * @short_description: certificate by XML file
 * @title: AgsXmlCertificate
 * @section_id:
 * @include: ags/server/security/ags_xml_certificate.h
 *
 * The #AgsXmlCertificate is an object to verify certificates.
 */

static gpointer ags_xml_certificate_parent_class = NULL;

GType
ags_xml_certificate_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_xml_certificate = 0;

    static const GTypeInfo ags_xml_certificate_info = {
      sizeof (AgsXmlCertificateClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_xml_certificate_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsXmlCertificate),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_xml_certificate_init,
    };

    static const GInterfaceInfo ags_certificate_interface_info = {
      (GInterfaceInitFunc) ags_xml_certificate_certificate_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_xml_certificate = g_type_register_static(G_TYPE_OBJECT,
						      "AgsXmlCertificate",
						      &ags_xml_certificate_info,
						      0);

    g_type_add_interface_static(ags_type_xml_certificate,
				AGS_TYPE_CERTIFICATE,
				&ags_certificate_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_xml_certificate);
  }

  return g_define_type_id__volatile;
}

void
ags_xml_certificate_class_init(AgsXmlCertificateClass *xml_certificate)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_xml_certificate_parent_class = g_type_class_peek_parent(xml_certificate);

  /* GObjectClass */
  gobject = (GObjectClass *) xml_certificate;

  gobject->finalize = ags_xml_certificate_finalize;
}

void
ags_xml_certificate_certificate_interface_init(AgsCertificateInterface *certificate)
{
  certificate->get_cert_uuid = ags_xml_certificate_get_cert_uuid;

  certificate->set_domain = ags_xml_certificate_set_domain;
  certificate->get_domain = ags_xml_certificate_get_domain;

  certificate->set_key_type = ags_xml_certificate_set_key_type;
  certificate->get_key_type = ags_xml_certificate_get_key_type;

  certificate->set_public_key_file = ags_xml_certificate_set_public_key_file;
  certificate->get_public_key_file = ags_xml_certificate_get_public_key_file;

  certificate->set_private_key_file = ags_xml_certificate_set_private_key_file;
  certificate->get_private_key_file = ags_xml_certificate_get_private_key_file;
}

void
ags_xml_certificate_init(AgsXmlCertificate *xml_certificate)
{
  g_rec_mutex_init(&(xml_certificate->obj_mutex));

  xml_certificate->filename = NULL;
  xml_certificate->encoding = NULL;
  xml_certificate->dtd = NULL;

  xml_certificate->doc = NULL;
  xml_certificate->root_node = NULL;
}

void
ags_xml_certificate_finalize(GObject *gobject)
{
  AgsXmlCertificate *xml_certificate;

  xml_certificate = AGS_XML_CERTIFICATE(gobject);

  g_free(xml_certificate->filename);
  g_free(xml_certificate->encoding);
  g_free(xml_certificate->dtd);
  
  if(xml_certificate->doc != NULL){
    xmlFreeDoc(xml_certificate->doc);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_xml_certificate_parent_class)->finalize(gobject);
}

gchar**
ags_xml_certificate_get_cert_uuid(AgsCertificate *certificate,
				  GObject *security_context,
				  gchar *user_uuid,
				  gchar *security_token,
				  GError **error)
{
  AgsXmlCertificate *xml_certificate;

  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  
  gchar *xpath;
  gchar **cert_uuid;
  
  guint i;
  guint j;
  
  GRecMutex *xml_certificate_mutex;

  if(!AGS_IS_SECURITY_CONTEXT(security_context)){
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

  xml_certificate = AGS_XML_CERTIFICATE(certificate);

  if(xml_certificate->doc == NULL ||
     xml_certificate->root_node == NULL){
    return(NULL);
  }

  xml_certificate_mutex = AGS_XML_CERTIFICATE_GET_OBJ_MUTEX(xml_certificate);

  cert_uuid = NULL;

  xpath = "/ags-server-certificate/ags-srv-cert-list/ags-srv-cert/ags-srv-cert-uuid";

  g_rec_mutex_lock(xml_certificate_mutex);
    
  xpath_context = xmlXPathNewContext(xml_certificate->doc);
  xpath_object = xmlXPathNodeEval(xml_certificate->root_node,
				  xpath,
				  xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0, j = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	xmlChar *current_user_uuid;
	
	current_user_uuid = xmlNodeGetContent(node[i]);

	if(j == 0){
	  cert_uuid = (gchar **) malloc(2 * sizeof(gchar *)); 
	}else{
	  cert_uuid = (gchar **) realloc(cert_uuid,
					 (j + 2) * sizeof(gchar *)); 
	}

	cert_uuid[j] = g_strdup(current_user_uuid);
	
	xmlFree(current_user_uuid);

	j++;
      }
    }

    if(j > 0){
      cert_uuid[j] = NULL;
    }
  }

  g_rec_mutex_unlock(xml_certificate_mutex);

  xmlXPathFreeObject(xpath_object);
  xmlXPathFreeContext(xpath_context);

  return(cert_uuid);
}

void
ags_xml_certificate_set_domain(AgsCertificate *certificate,
			       GObject *security_context,
			       gchar *user_uuid,
			       gchar *security_token,
			       gchar *cert_uuid,
			       gchar *domain,
			       GError **error)
{
  AgsXmlCertificate *xml_certificate;
  
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *cert_node;
  xmlNode *domain_node;
  xmlNode *child;
  
  gchar *xpath;
  
  guint i;

  GRecMutex *xml_certificate_mutex;

  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     cert_uuid == NULL){
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

  xml_certificate = AGS_XML_CERTIFICATE(certificate);

  if(xml_certificate->doc == NULL ||
     xml_certificate->root_node == NULL){
    return;
  }

  xml_certificate_mutex = AGS_XML_CERTIFICATE_GET_OBJ_MUTEX(xml_certificate);

  cert_node = NULL;

  xpath = g_strdup_printf("/ags-server-certificate/ags-srv-cert-list/ags-srv-cert/ags-srv-cert-uuid[text() = '%s']",
			  cert_uuid);

  g_rec_mutex_lock(xml_certificate_mutex);
    
  xpath_context = xmlXPathNewContext(xml_certificate->doc);
  xpath_object = xmlXPathEval(xpath,
			      xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	cert_node = node[i]->parent;

	break;
      }
    }
  }

  if(cert_node != NULL){
    domain_node = NULL;
    
    child = cert_node->children;
    
    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"ags-srv-cert-domain",
				20)){
	  domain_node = child;
	  
	  break;
	}
      }
	
      child = child->next;
    }

    if(domain_node == NULL){
      domain_node = xmlNewNode(NULL,
			       "ags-srv-cert-domain");
      xmlAddChild(cert_node,
		  domain_node);
    }

    xmlNodeSetContent(domain_node,
		      domain);
  }

  g_rec_mutex_unlock(xml_certificate_mutex);

  xmlXPathFreeObject(xpath_object);
  xmlXPathFreeContext(xpath_context);

  g_free(xpath);
}

gchar*
ags_xml_certificate_get_domain(AgsCertificate *certificate,
			       GObject *security_context,
			       gchar *user_uuid,
			       gchar *security_token,
			       gchar *cert_uuid,
			       GError **error)
{
  AgsXmlCertificate *xml_certificate;
  
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *cert_node;
  xmlNode *domain_node;
  xmlNode *child;
  
  gchar *xpath;
  gchar *domain;

  guint i;
  
  GRecMutex *xml_certificate_mutex;

  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     cert_uuid == NULL){
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

  xml_certificate = AGS_XML_CERTIFICATE(certificate);

  if(xml_certificate->doc == NULL ||
     xml_certificate->root_node == NULL){
    return(NULL);
  }

  xml_certificate_mutex = AGS_XML_CERTIFICATE_GET_OBJ_MUTEX(xml_certificate);
  
  cert_node = NULL;

  xpath = g_strdup_printf("/ags-server-certificate/ags-srv-cert-list/ags-srv-cert/ags-srv-cert-uuid[text() = '%s']",
			  cert_uuid);

  g_rec_mutex_lock(xml_certificate_mutex);
    
  xpath_context = xmlXPathNewContext(xml_certificate->doc);
  xpath_object = xmlXPathNodeEval(xml_certificate->root_node,
				  xpath,
				  xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	cert_node = node[i]->parent;

	break;
      }
    }
  }

  domain = NULL;
  
  if(cert_node != NULL){
    domain_node = NULL;
    
    child = cert_node->children;
    
    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"ags-srv-cert-domain",
				20)){
	  domain_node = child;
	  
	  break;
	}
      }
	
      child = child->next;
    }

    if(domain_node != NULL){
      xmlChar *tmp_domain;
      
      tmp_domain = xmlNodeGetContent(domain_node);

      domain = g_strdup(tmp_domain);

      xmlFree(tmp_domain);
    }
  }

  g_rec_mutex_unlock(xml_certificate_mutex);

  xmlXPathFreeObject(xpath_object);
  xmlXPathFreeContext(xpath_context);

  g_free(xpath);

  return(domain);
}

void
ags_xml_certificate_set_key_type(AgsCertificate *certificate,
				 GObject *security_context,
				 gchar *user_uuid,
				 gchar *security_token,
				 gchar *cert_uuid,
				 gchar *key_type,
				 GError **error)
{
  AgsXmlCertificate *xml_certificate;
  
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *cert_node;
  xmlNode *key_type_node;
  xmlNode *child;
  
  gchar *xpath;
  
  guint i;

  GRecMutex *xml_certificate_mutex;

  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     cert_uuid == NULL){
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

  xml_certificate = AGS_XML_CERTIFICATE(certificate);

  if(xml_certificate->doc == NULL ||
     xml_certificate->root_node == NULL){
    return;
  }

  xml_certificate_mutex = AGS_XML_CERTIFICATE_GET_OBJ_MUTEX(xml_certificate);

  cert_node = NULL;

  xpath = g_strdup_printf("/ags-server-certificate/ags-srv-cert-list/ags-srv-cert/ags-srv-cert-uuid[text() = '%s']",
			  cert_uuid);

  g_rec_mutex_lock(xml_certificate_mutex);
    
  xpath_context = xmlXPathNewContext(xml_certificate->doc);
  xpath_object = xmlXPathEval(xpath,
			      xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	cert_node = node[i]->parent;

	break;
      }
    }
  }

  if(cert_node != NULL){
    key_type_node = NULL;
    
    child = cert_node->children;
    
    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"ags-srv-cert-key-type",
				22)){
	  key_type_node = child;
	  
	  break;
	}
      }
	
      child = child->next;
    }

    if(key_type_node == NULL){
      key_type_node = xmlNewNode(NULL,
				 "ags-srv-cert-key-type");
      xmlAddChild(cert_node,
		  key_type_node);
    }

    xmlNodeSetContent(key_type_node,
		      key_type);
  }

  g_rec_mutex_unlock(xml_certificate_mutex);

  xmlXPathFreeObject(xpath_object);
  xmlXPathFreeContext(xpath_context);

  g_free(xpath);
}

gchar*
ags_xml_certificate_get_key_type(AgsCertificate *certificate,
				 GObject *security_context,
				 gchar *user_uuid,
				 gchar *security_token,
				 gchar *cert_uuid,
				 GError **error)
{
  AgsXmlCertificate *xml_certificate;
  
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *cert_node;
  xmlNode *key_type_node;
  xmlNode *child;
  
  gchar *xpath;
  gchar *key_type;

  guint i;
  
  GRecMutex *xml_certificate_mutex;

  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     cert_uuid == NULL){
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

  xml_certificate = AGS_XML_CERTIFICATE(certificate);

  if(xml_certificate->doc == NULL ||
     xml_certificate->root_node == NULL){
    return(NULL);
  }

  xml_certificate_mutex = AGS_XML_CERTIFICATE_GET_OBJ_MUTEX(xml_certificate);
  
  cert_node = NULL;

  xpath = g_strdup_printf("/ags-server-certificate/ags-srv-cert-list/ags-srv-cert/ags-srv-cert-uuid[text() = '%s']",
			  cert_uuid);

  g_rec_mutex_lock(xml_certificate_mutex);
    
  xpath_context = xmlXPathNewContext(xml_certificate->doc);
  xpath_object = xmlXPathNodeEval(xml_certificate->root_node,
				  xpath,
				  xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	cert_node = node[i]->parent;

	break;
      }
    }
  }

  key_type = NULL;
  
  if(cert_node != NULL){
    key_type_node = NULL;
    
    child = cert_node->children;
    
    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"ags-srv-cert-key-type",
				22)){
	  key_type_node = child;
	  
	  break;
	}
      }
	
      child = child->next;
    }

    if(key_type_node != NULL){
      xmlChar *tmp_key_type;
      
      tmp_key_type = xmlNodeGetContent(key_type_node);

      key_type = g_strdup(tmp_key_type);

      xmlFree(tmp_key_type);
    }
  }

  g_rec_mutex_unlock(xml_certificate_mutex);

  xmlXPathFreeObject(xpath_object);
  xmlXPathFreeContext(xpath_context);

  g_free(xpath);

  return(key_type);
}

void
ags_xml_certificate_set_public_key_file(AgsCertificate *certificate,
					GObject *security_context,
					gchar *user_uuid,
					gchar *security_token,
					gchar *cert_uuid,
					gchar *public_key_file,
					GError **error)
{
  AgsXmlCertificate *xml_certificate;
  
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *cert_node;
  xmlNode *public_key_file_node;
  xmlNode *child;
  
  gchar *xpath;
  
  guint i;

  GRecMutex *xml_certificate_mutex;

  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     cert_uuid == NULL){
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

  xml_certificate = AGS_XML_CERTIFICATE(certificate);

  if(xml_certificate->doc == NULL ||
     xml_certificate->root_node == NULL){
    return;
  }

  xml_certificate_mutex = AGS_XML_CERTIFICATE_GET_OBJ_MUTEX(xml_certificate);

  cert_node = NULL;

  xpath = g_strdup_printf("/ags-server-certificate/ags-srv-cert-list/ags-srv-cert/ags-srv-cert-uuid[text() = '%s']",
			  cert_uuid);

  g_rec_mutex_lock(xml_certificate_mutex);
    
  xpath_context = xmlXPathNewContext(xml_certificate->doc);
  xpath_object = xmlXPathEval(xpath,
			      xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	cert_node = node[i]->parent;

	break;
      }
    }
  }

  if(cert_node != NULL){
    public_key_file_node = NULL;
    
    child = cert_node->children;
    
    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"ags-srv-cert-public-key-file",
				29)){
	  public_key_file_node = child;
	  
	  break;
	}
      }
	
      child = child->next;
    }

    if(public_key_file_node == NULL){
      public_key_file_node = xmlNewNode(NULL,
					"ags-srv-cert-public-key-file");
      xmlAddChild(cert_node,
		  public_key_file_node);
    }

    xmlNodeSetContent(public_key_file_node,
		      public_key_file);
  }

  g_rec_mutex_unlock(xml_certificate_mutex);

  xmlXPathFreeObject(xpath_object);
  xmlXPathFreeContext(xpath_context);

  g_free(xpath);
}

gchar*
ags_xml_certificate_get_public_key_file(AgsCertificate *certificate,
					GObject *security_context,
					gchar *user_uuid,
					gchar *security_token,
					gchar *cert_uuid,
					GError **error)
{
  AgsXmlCertificate *xml_certificate;
  
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *cert_node;
  xmlNode *public_key_file_node;
  xmlNode *child;
  
  gchar *xpath;
  gchar *public_key_file;

  guint i;
  
  GRecMutex *xml_certificate_mutex;

  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     cert_uuid == NULL){
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

  xml_certificate = AGS_XML_CERTIFICATE(certificate);

  if(xml_certificate->doc == NULL ||
     xml_certificate->root_node == NULL){
    return(NULL);
  }

  xml_certificate_mutex = AGS_XML_CERTIFICATE_GET_OBJ_MUTEX(xml_certificate);
  
  cert_node = NULL;

  xpath = g_strdup_printf("/ags-server-certificate/ags-srv-cert-list/ags-srv-cert/ags-srv-cert-uuid[text() = '%s']",
			  cert_uuid);

  g_rec_mutex_lock(xml_certificate_mutex);
    
  xpath_context = xmlXPathNewContext(xml_certificate->doc);
  xpath_object = xmlXPathNodeEval(xml_certificate->root_node,
				  xpath,
				  xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	cert_node = node[i]->parent;

	break;
      }
    }
  }

  public_key_file = NULL;
  
  if(cert_node != NULL){
    public_key_file_node = NULL;
    
    child = cert_node->children;
    
    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"ags-srv-cert-public-key-file",
				29)){
	  public_key_file_node = child;
	  
	  break;
	}
      }
	
      child = child->next;
    }

    if(public_key_file_node != NULL){
      xmlChar *tmp_public_key_file;
      
      tmp_public_key_file = xmlNodeGetContent(public_key_file_node);

      public_key_file = g_strdup(tmp_public_key_file);

      xmlFree(tmp_public_key_file);
    }
  }

  g_rec_mutex_unlock(xml_certificate_mutex);

  xmlXPathFreeObject(xpath_object);
  xmlXPathFreeContext(xpath_context);

  g_free(xpath);

  return(public_key_file);
}

void
ags_xml_certificate_set_private_key_file(AgsCertificate *certificate,
					 GObject *security_context,
					 gchar *user_uuid,
					 gchar *security_token,
					 gchar *cert_uuid,
					 gchar *private_key_file,
					 GError **error)
{
  AgsXmlCertificate *xml_certificate;
  
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *cert_node;
  xmlNode *private_key_file_node;
  xmlNode *child;
  
  gchar *xpath;
  
  guint i;

  GRecMutex *xml_certificate_mutex;

  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     cert_uuid == NULL){
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

  xml_certificate = AGS_XML_CERTIFICATE(certificate);

  if(xml_certificate->doc == NULL ||
     xml_certificate->root_node == NULL){
    return;
  }

  xml_certificate_mutex = AGS_XML_CERTIFICATE_GET_OBJ_MUTEX(xml_certificate);

  cert_node = NULL;

  xpath = g_strdup_printf("/ags-server-certificate/ags-srv-cert-list/ags-srv-cert/ags-srv-cert-uuid[text() = '%s']",
			  cert_uuid);

  g_rec_mutex_lock(xml_certificate_mutex);
    
  xpath_context = xmlXPathNewContext(xml_certificate->doc);
  xpath_object = xmlXPathEval(xpath,
			      xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	cert_node = node[i]->parent;

	break;
      }
    }
  }

  if(cert_node != NULL){
    private_key_file_node = NULL;
    
    child = cert_node->children;
    
    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"ags-srv-cert-private-key-file",
				30)){
	  private_key_file_node = child;
	  
	  break;
	}
      }
	
      child = child->next;
    }

    if(private_key_file_node == NULL){
      private_key_file_node = xmlNewNode(NULL,
					 "ags-srv-cert-private-key-file");
      xmlAddChild(cert_node,
		  private_key_file_node);
    }

    xmlNodeSetContent(private_key_file_node,
		      private_key_file);
  }

  g_rec_mutex_unlock(xml_certificate_mutex);

  xmlXPathFreeObject(xpath_object);
  xmlXPathFreeContext(xpath_context);

  g_free(xpath);
}

gchar*
ags_xml_certificate_get_private_key_file(AgsCertificate *certificate,
					 GObject *security_context,
					 gchar *user_uuid,
					 gchar *security_token,
					 gchar *cert_uuid,
					 GError **error)
{
  AgsXmlCertificate *xml_certificate;
  
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  xmlNode *cert_node;
  xmlNode *private_key_file_node;
  xmlNode *child;
  
  gchar *xpath;
  gchar *private_key_file;

  guint i;
  
  GRecMutex *xml_certificate_mutex;

  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     cert_uuid == NULL){
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

  xml_certificate = AGS_XML_CERTIFICATE(certificate);

  if(xml_certificate->doc == NULL ||
     xml_certificate->root_node == NULL){
    return(NULL);
  }

  xml_certificate_mutex = AGS_XML_CERTIFICATE_GET_OBJ_MUTEX(xml_certificate);
  
  cert_node = NULL;

  xpath = g_strdup_printf("/ags-server-certificate/ags-srv-cert-list/ags-srv-cert/ags-srv-cert-uuid[text() = '%s']",
			  cert_uuid);

  g_rec_mutex_lock(xml_certificate_mutex);
    
  xpath_context = xmlXPathNewContext(xml_certificate->doc);
  xpath_object = xmlXPathNodeEval(xml_certificate->root_node,
				  xpath,
				  xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	cert_node = node[i]->parent;

	break;
      }
    }
  }

  private_key_file = NULL;
  
  if(cert_node != NULL){
    private_key_file_node = NULL;
    
    child = cert_node->children;
    
    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"ags-srv-cert-private-key-file",
				30)){
	  private_key_file_node = child;
	  
	  break;
	}
      }
	
      child = child->next;
    }

    if(private_key_file_node != NULL){
      xmlChar *tmp_private_key_file;
      
      tmp_private_key_file = xmlNodeGetContent(private_key_file_node);

      private_key_file = g_strdup(tmp_private_key_file);

      xmlFree(tmp_private_key_file);
    }
  }

  g_rec_mutex_unlock(xml_certificate_mutex);

  xmlXPathFreeObject(xpath_object);
  xmlXPathFreeContext(xpath_context);

  g_free(xpath);

  return(private_key_file);
}

/**
 * ags_xml_certificate_open_filename:
 * @xml_certificate: the #AgsXmlCertificate
 * @filename: the filename
 * 
 * Open @filename.
 * 
 * Since: 3.0.0
 */
void
ags_xml_certificate_open_filename(AgsXmlCertificate *xml_certificate,
				  gchar *filename)
{
  xmlDoc *doc;

  GRecMutex *xml_certificate_mutex;

  if(!AGS_IS_XML_CERTIFICATE(xml_certificate) ||
     filename == NULL){
    return;
  }
  
  xml_certificate_mutex = AGS_XML_CERTIFICATE_GET_OBJ_MUTEX(xml_certificate);

  /* open XML */
  doc = xmlReadFile(filename,
		    NULL,
		    0);

  g_rec_mutex_lock(xml_certificate_mutex);

  xml_certificate->filename = g_strdup(filename);

  xml_certificate->doc = doc;
  
  if(doc == NULL){
    g_warning("AgsXmlCertificate - failed to read XML document %s", filename);
  }else{
    /* get the root node */
    xml_certificate->root_node = xmlDocGetRootElement(doc);
  }

  g_rec_mutex_unlock(xml_certificate_mutex);
}

/**
 * ags_xml_certificate_new:
 *
 * Create #AgsXmlCertificate.
 *
 * Returns: the new #AgsXmlCertificate instance
 *
 * Since: 3.0.0
 */
AgsXmlCertificate*
ags_xml_certificate_new()
{
  AgsXmlCertificate *xml_certificate;

  xml_certificate = (AgsXmlCertificate *) g_object_new(AGS_TYPE_XML_CERTIFICATE,
						       NULL);

  return(xml_certificate);
}
