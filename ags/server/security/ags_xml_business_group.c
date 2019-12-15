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

#include <ags/server/security/ags_xml_business_group.h>

#include <ags/server/security/ags_business_group.h>
#include <ags/server/security/ags_auth_security_context.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

void ags_xml_business_group_class_init(AgsXmlBusinessGroupClass *xml_business_group);
void ags_xml_business_group_business_group_interface_init(AgsBusinessGroupInterface *business_group);
void ags_xml_business_group_init(AgsXmlBusinessGroup *xml_business_group);
void ags_xml_business_group_finalize(GObject *gobject);

void ags_xml_business_group_set_group_name(AgsBusinessGroup *business_group,
					   GObject *security_context,
					   gchar *user_uuid,
					   gchar *security_token,
					   gchar *group_uuid,
					   gchar *group_name,
					   GError **error);
gchar* ags_xml_business_group_get_group_name(AgsBusinessGroup *business_group,
					     GObject *security_context,
					     gchar *user_uuid,
					     gchar *security_token,
					     gchar *group_uuid,
					     GError **error);  
void ags_xml_business_group_set_user(AgsBusinessGroup *business_group,
				     GObject *security_context,
				     gchar *user_uuid,
				     gchar *security_token,
				     gchar *group_uuid,
				     gchar **user,
				     GError **error);
gchar** ags_xml_business_group_get_user(AgsBusinessGroup *business_group,
					GObject *security_context,
					gchar *user_uuid,
					gchar *security_token,
					gchar *group_uuid,
					GError **error);

/**
 * SECTION:ags_xml_business_group
 * @short_description: password store by XML file
 * @title: AgsXmlBusinessGroup
 * @section_id:
 * @include: ags/server/security/ags_xml_business_group.h
 *
 * The #AgsXmlBusinessGroup is an object to keep passwords.
 */

static gpointer ags_xml_business_group_parent_class = NULL;

GType
ags_xml_business_group_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_xml_business_group = 0;

    static const GTypeInfo ags_xml_business_group_info = {
      sizeof (AgsXmlBusinessGroupClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_xml_business_group_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsXmlBusinessGroup),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_xml_business_group_init,
    };

    static const GInterfaceInfo ags_business_group_interface_info = {
      (GInterfaceInitFunc) ags_xml_business_group_business_group_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_xml_business_group = g_type_register_static(G_TYPE_OBJECT,
							 "AgsXmlBusinessGroup",
							 &ags_xml_business_group_info,
							 0);

    g_type_add_interface_static(ags_type_xml_business_group,
				AGS_TYPE_BUSINESS_GROUP,
				&ags_business_group_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_xml_business_group);
  }

  return g_define_type_id__volatile;
}

void
ags_xml_business_group_class_init(AgsXmlBusinessGroupClass *xml_business_group)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_xml_business_group_parent_class = g_type_class_peek_parent(xml_business_group);

  /* GObjectClass */
  gobject = (GObjectClass *) xml_business_group;

  gobject->finalize = ags_xml_business_group_finalize;
}

void
ags_xml_business_group_business_group_interface_init(AgsBusinessGroupInterface *business_group)
{
  business_group->get_group_name = ags_xml_business_group_get_group_name;
  business_group->set_group_name = ags_xml_business_group_set_group_name;

  business_group->get_user = ags_xml_business_group_get_user;
  business_group->set_user = ags_xml_business_group_set_user;
}

void
ags_xml_business_group_init(AgsXmlBusinessGroup *xml_business_group)
{
  g_rec_mutex_init(&(xml_business_group->obj_mutex));

  xml_business_group->filename = NULL;
  xml_business_group->encoding = NULL;
  xml_business_group->dtd = NULL;

  xml_business_group->doc = NULL;
  xml_business_group->root_node = NULL;
}

void
ags_xml_business_group_finalize(GObject *gobject)
{
  AgsXmlBusinessGroup *xml_business_group;

  xml_business_group = AGS_XML_BUSINESS_GROUP(gobject);

  g_free(xml_business_group->filename);
  g_free(xml_business_group->encoding);
  g_free(xml_business_group->dtd);
  
  if(xml_business_group->doc != NULL){
    xmlFreeDoc(xml_business_group->doc);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_xml_business_group_parent_class)->finalize(gobject);
}

void
ags_xml_business_group_set_group_name(AgsBusinessGroup *business_group,
				      GObject *security_context,
				      gchar *user_uuid,
				      gchar *security_token,
				      gchar *group_uuid,
				      gchar *group_name,
				      GError **error)
{
  //TODO:JK: implement me
}

gchar*
ags_xml_business_group_get_group_name(AgsBusinessGroup *business_group,
				      GObject *security_context,
				      gchar *user_uuid,
				      gchar *security_token,
				      gchar *group_uuid,
				      GError **error)
{
  gchar *group_name;

  if(!AGS_IS_AUTH_SECURITY_CONTEXT(security_context)){
    if(user_uuid == NULL ||
       security_token == NULL){
      return(NULL);
    }    
  }
  
  group_name = NULL;
  
  //TODO:JK: implement me

  return(group_name);
}

void
ags_xml_business_group_set_user(AgsBusinessGroup *business_group,
				GObject *security_context,
				gchar *user_uuid,
				gchar *security_token,
				gchar *group_uuid,
				gchar **user,
				GError **error)
{
  //TODO:JK: implement me
}

gchar**
ags_xml_business_group_get_user(AgsBusinessGroup *business_group,
				GObject *security_context,
				gchar *user_uuid,
				gchar *security_token,
				gchar *group_uuid,
				GError **error)
{
  gchar **user;

  user = NULL;
  
  //TODO:JK: implement me

  return(user);
}

/**
 * ags_xml_business_group_open_filename:
 * @xml_business_group: the #AgsXmlBusinessGroup
 * @filename: the filename
 * 
 * Open @filename.
 * 
 * Since: 3.0.0
 */
void
ags_xml_business_group_open_filename(AgsXmlBusinessGroup *xml_business_group,
				     gchar *filename)
{
  xmlDoc *doc;

  GRecMutex *xml_business_group_mutex;

  if(!AGS_IS_XML_BUSINESS_GROUP(xml_business_group) ||
     filename == NULL){
    return;
  }
  
  xml_business_group_mutex = AGS_XML_BUSINESS_GROUP_GET_OBJ_MUTEX(xml_business_group);

  /* open XML */
  doc = xmlReadFile(filename,
		    NULL,
		    0);

  g_rec_mutex_lock(xml_business_group_mutex);

  xml_business_group->filename = g_strdup(filename);

  xml_business_group->doc = doc;
  
  if(doc == NULL){
    g_warning("AgsXmlBusinessGroup - failed to read XML document %s", filename);
  }else{
    /* get the root node */
    xml_business_group->root_node = xmlDocGetRootElement(doc);
  }

  g_rec_mutex_unlock(xml_business_group_mutex);
}

/**
 * ags_xml_business_group_new:
 *
 * Create #AgsXmlBusinessGroup.
 *
 * Returns: the new #AgsXmlBusinessGroup instance
 *
 * Since: 3.0.0
 */
AgsXmlBusinessGroup*
ags_xml_business_group_new()
{
  AgsXmlBusinessGroup *xml_business_group;

  xml_business_group = (AgsXmlBusinessGroup *) g_object_new(AGS_TYPE_XML_BUSINESS_GROUP,
							    NULL);

  return(xml_business_group);
}
