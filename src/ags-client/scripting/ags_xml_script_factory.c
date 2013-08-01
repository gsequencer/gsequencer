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

void ags_xml_script_factory_class_init(AgsXmlScriptFactoryClass *xml_script_factory);
void ags_xml_script_factory_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_xml_script_factory_init(AgsXmlScriptFactory *xml_script_factory);
void ags_xml_script_factory_connect(AgsConnectable *connectable);
void ags_xml_script_factory_disconnect(AgsConnectable *connectable);
void ags_xml_script_factory_finalize(GObject *gobject);

void ags_xml_script_factory_real_create_prototype(AgsXmlScriptFactory *xml_script_factory);

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
  xml_script_factory->map_xml = ags_xml_script_factory_real_map_xml;
  xml_script_factory->launch = ags_xml_script_factory_real_launch;

  /* signals */
  task_signals[MAP_XML] =
    g_signal_new("map_xml\0",
		 G_TYPE_FROM_CLASS(task),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsTaskClass, map_xml),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  task_signals[LAUNCH] =
    g_signal_new("launch\0",
		 G_TYPE_FROM_CLASS(task),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsTaskClass, launch),
		 NULL, NULL,
		 g_cclosure_user_marshal_OBJECT__VOID,
		 G_TYPE_OBJECT, 0);
}

void
ags_xml_script_factory_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_xml_script_factory_connect;
  connectable->disconnect = ags_xml_script_factory_disconnect;
}

void
ags_xml_script_factory_init(AgsXmlScriptFactory *xml_script_factory)
{
  xml_script_factory->node = NULL;
  xml_script_factory->id = NULL;

  xml_script_factory->retval = NULL;
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
}

void
ags_xml_script_factory_create_prototype(AgsXmlScriptFactory *xml_script_factory)
{
}

xmlNode*
ags_xml_script_factory_map(AgsXmlScriptFactory *xml_script_factory,
			   AgsScriptObject *script_object)
{
}

AgsXmlScriptFactory*
ags_xml_script_factory_new()
{
  AgsXmlScriptFactory *xml_script_factory;

  xml_script_factory = (AgsXmlScriptFactory *) g_object_new(AGS_TYPE_XML_SCRIPT_FACTORY,
							    NULL);
  
  return(xml_script_factory);
}
