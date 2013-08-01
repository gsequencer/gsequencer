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

#include <ags-client/scripting/ags_script_object.h>

#include <ags-lib/object/ags_connectable.h>

void ags_script_object_class_init(AgsScriptObjectClass *script_object);
void ags_script_object_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_script_object_init(AgsScriptObject *script_object);
void ags_script_object_connect(AgsConnectable *connectable);
void ags_script_object_disconnect(AgsConnectable *connectable);
void ags_script_object_finalize(GObject *gobject);

void ags_script_object_real_map_xml(AgsScriptObject *script_object);
AgsScriptObject* ags_script_real_object_launch(AgsScriptObject *script_object);

static gpointer ags_script_object_parent_class = NULL;

GType
ags_script_object_get_type()
{
  static GType ags_type_script_object = 0;

  if(!ags_type_script_object){
    static const GTypeInfo ags_script_object_info = {
      sizeof (AgsScriptObjectClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_script_object_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsScriptObject),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_script_object_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_script_object_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_script_object = g_type_register_static(G_TYPE_OBJECT,
						    "AgsScriptObject\0",
						    &ags_script_object_info,
						    0);
    
    g_type_add_interface_static(ags_type_script_object,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_script_object);
}

void
ags_script_object_class_init(AgsScriptObjectClass *script_object)
{
  GObjectClass *gobject;

  ags_script_object_parent_class = g_type_class_peek_parent(script_object);

  /* GObjectClass */
  gobject = (GObjectClass *) script_object;

  gobject->finalize = ags_script_object_finalize;

  /* AgsScriptObjectClass */
  script_object->map_xml = ags_script_object_real_map_xml;
  script_object->launch = ags_script_object_real_launch;
}

void
ags_script_object_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_script_object_connect;
  connectable->disconnect = ags_script_object_disconnect;
}

void
ags_script_object_init(AgsScriptObject *script_object)
{
  //TODO:JK: implement me
}

void
ags_script_object_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_script_object_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_script_object_finalize(GObject *gobject)
{
  AgsScriptObject *script_object;

  script_object = AGS_SCRIPT_OBJECT(gobject);

  G_OBJECT_CLASS(ags_script_object_parent_class)->finalize(gobject);
}

void
ags_script_object_real_map_xml(AgsScriptObject *script_object)
{
}

void
ags_script_object_map_xml(AgsScriptObject *script_object)
{
}

AgsScriptObject*
ags_script_real_object_launch(AgsScriptObject *script_object)
{
}

AgsScriptObject*
ags_script_object_launch(AgsScriptObject *script_object)
{
}

AgsScriptObject*
ags_script_object_new()
{
  AgsScriptObject *script_object;

  script_object = (AgsScriptObject *) g_object_new(AGS_TYPE_SCRIPT_OBJECT,
						   NULL);
  
  return(script_object);
}
