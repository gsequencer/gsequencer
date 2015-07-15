/* AGS Client - Advanced GTK Sequencer Client
 * Copyright (C) 2013 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags-client/scripting/ags_xml_script_factory.h>

#include <ags-lib/object/ags_connectable.h>

#include <libxml/xmlstring.h>

#include <string.h>

void ags_xml_script_factory_class_init(AgsXmlScriptFactoryClass *xml_script_factory);
void ags_xml_script_factory_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_xml_script_factory_init(AgsXmlScriptFactory *xml_script_factory);
void ags_xml_script_factory_connect(AgsConnectable *connectable);
void ags_xml_script_factory_disconnect(AgsConnectable *connectable);
void ags_xml_script_factory_finalize(GObject *gobject);

void ags_xml_script_factory_real_create_prototype(AgsXmlScriptFactory *xml_script_factory);

xmlNode* ags_xml_script_factory_find_prototype(AgsXmlScriptFactory *xml_script_factory,
					       gchar *xml_type);

enum{
  CREATE_PROTOTYPE,
  LAST_SIGNAL,
};

static gpointer ags_xml_script_factory_parent_class = NULL;
static guint xml_script_factory_signals[LAST_SIGNAL];

GType
ags_xml_script_factory_get_type()
{
  static GType ags_type_xml_script_factory = 0;

  if(!ags_type_xml_script_factory){
    static const GTypeInfo ags_xml_script_factory_info = {
      sizeof (AgsXmlScriptFactoryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_xml_script_factory_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsXmlScriptFactory),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_xml_script_factory_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_xml_script_factory_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_xml_script_factory = g_type_register_static(G_TYPE_OBJECT,
							 "AgsXmlScriptFactory\0",
							 &ags_xml_script_factory_info,
							 0);
    
    g_type_add_interface_static(ags_type_xml_script_factory,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_xml_script_factory);
}

void
ags_xml_script_factory_class_init(AgsXmlScriptFactoryClass *xml_script_factory)
{
  GObjectClass *gobject;

  ags_xml_script_factory_parent_class = g_type_class_peek_parent(xml_script_factory);

  /* GObjectClass */
  gobject = (GObjectClass *) xml_script_factory;

  gobject->finalize = ags_xml_script_factory_finalize;

  /* AgsXmlScriptFactoryClass */
  xml_script_factory->create_prototype = ags_xml_script_factory_real_create_prototype;

  /* signals */
  xml_script_factory_signals[CREATE_PROTOTYPE] =
    g_signal_new("create_prototype\0",
		 G_TYPE_FROM_CLASS(xml_script_factory),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsXmlScriptFactoryClass, create_prototype),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_xml_script_factory_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_xml_script_factory_connect;
  connectable->disconnect = ags_xml_script_factory_disconnect;
}

GQuark
ags_xml_script_factory_error_quark()
{
  return(g_quark_from_static_string("ags-xml-script-factory-error-quark\0"));
}

void
ags_xml_script_factory_init(AgsXmlScriptFactory *xml_script_factory)
{
  xml_script_factory->schema = xmlReadFile(AGS_XML_SCRIPT_FACTORY_DEFAULT_SCHEMA, NULL, 0);
  xml_script_factory->schema_factory = ags_schema_factory_new();

  xml_script_factory->prototype = NULL;
}

void
ags_xml_script_factory_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_xml_script_factory_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_xml_script_factory_finalize(GObject *gobject)
{
  AgsXmlScriptFactory *xml_script_factory;

  xml_script_factory = AGS_XML_SCRIPT_FACTORY(gobject);

  G_OBJECT_CLASS(ags_xml_script_factory_parent_class)->finalize(gobject);
}

void
ags_xml_script_factory_real_create_prototype(AgsXmlScriptFactory *xml_script_factory)
{
  xmlNode *root_node;
  xmlNode *current_node;
  GList *list;

  root_node = xmlDocGetRootElement(xml_script_factory->schema);

  current_node = root_node->children;
  list = NULL;

  while(current_node != NULL){
    list = g_list_prepend(list, ags_schema_factory_create_node(xml_script_factory->schema_factory,
							       current_node));

    current_node = current_node->next;
  }

  xml_script_factory->prototype = list;
}

void
ags_xml_script_factory_create_prototype(AgsXmlScriptFactory *xml_script_factory)
{
  g_return_if_fail(AGS_IS_XML_SCRIPT_FACTORY(xml_script_factory));

  g_object_ref(G_OBJECT(xml_script_factory));
  g_signal_emit(G_OBJECT(xml_script_factory),
		xml_script_factory_signals[CREATE_PROTOTYPE], 0);
  g_object_unref(G_OBJECT(xml_script_factory));
}

xmlNode*
ags_xml_script_factory_find_prototype(AgsXmlScriptFactory *xml_script_factory,
				      gchar *xml_type)
{
  xmlNode *current_node;
  GList *current;

  current = xml_script_factory->prototype;

  while(current != NULL){
    current_node = (xmlNode *) current->data;

    if(!xmlStrcmp(current_node->name, xml_type)){
      return(current_node);
    }

    current = current->next;
  }

  return(NULL);
}

xmlNode*
ags_xml_script_factory_map(AgsXmlScriptFactory *xml_script_factory,
			   gchar *name,
			   GError **error)
{
  static const char *prefix = "AgsScript\0";
  xmlNode *prototype, *retval;
  gchar *xml_type;
  int length, prefix_length;

  xml_type = name;

  retval = NULL;

  prototype = ags_xml_script_factory_find_prototype(xml_script_factory,
						    xml_type);
  retval = xmlCopyNode(prototype, 1);

  return(retval);
}

AgsXmlScriptFactory*
ags_xml_script_factory_new()
{
  AgsXmlScriptFactory *xml_script_factory;

  xml_script_factory = (AgsXmlScriptFactory *) g_object_new(AGS_TYPE_XML_SCRIPT_FACTORY,
							    NULL);
  
  return(xml_script_factory);
}
