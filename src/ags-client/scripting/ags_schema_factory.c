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

#include <ags-client/scripting/ags_schema_factory.h>

#include <ags-lib/object/ags_connectable.h>

void ags_schema_factory_class_init(AgsSchemaFactoryClass *schema_factory);
void ags_schema_factory_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_schema_factory_init(AgsSchemaFactory *schema_factory);
void ags_schema_factory_connect(AgsConnectable *connectable);
void ags_schema_factory_disconnect(AgsConnectable *connectable);
void ags_schema_factory_finalize(GObject *gobject);

static gpointer ags_schema_factory_parent_class = NULL;

GType
ags_schema_factory_get_type()
{
  static GType ags_type_schema_factory = 0;

  if(!ags_type_schema_factory){
    static const GTypeInfo ags_schema_factory_info = {
      sizeof (AgsSchemaFactoryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_schema_factory_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSchemaFactory),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_schema_factory_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_schema_factory_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_schema_factory = g_type_register_static(G_TYPE_OBJECT,
							 "AgsSchemaFactory\0",
							 &ags_schema_factory_info,
							 0);
    
    g_type_add_interface_static(ags_type_schema_factory,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_schema_factory);
}

void
ags_schema_factory_class_init(AgsSchemaFactoryClass *schema_factory)
{
  GObjectClass *gobject;

  ags_schema_factory_parent_class = g_type_class_peek_parent(schema_factory);

  /* GObjectClass */
  gobject = (GObjectClass *) schema_factory;

  gobject->finalize = ags_schema_factory_finalize;
}

void
ags_schema_factory_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_schema_factory_connect;
  connectable->disconnect = ags_schema_factory_disconnect;
}

void
ags_schema_factory_init(AgsSchemaFactory *schema_factory)
{
}

void
ags_schema_factory_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_schema_factory_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_schema_factory_finalize(GObject *gobject)
{
  AgsSchemaFactory *schema_factory;

  schema_factory = AGS_SCHEMA_FACTORY(gobject);

  G_OBJECT_CLASS(ags_schema_factory_parent_class)->finalize(gobject);
}

xmlNode*
ags_schema_factory_create_node(AgsSchemaFactory *schema_factory,
			       xmlNode *node)
{
}

AgsSchemaFactory*
ags_schema_factory_new()
{
  AgsSchemaFactory *schema_factory;

  schema_factory = (AgsSchemaFactory *) g_object_new(AGS_TYPE_SCHEMA_FACTORY,
						     NULL);
  
  return(schema_factory);
}
