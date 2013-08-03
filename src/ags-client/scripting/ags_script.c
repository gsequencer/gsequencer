/* AGS - Advanced GTK Sequencer
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

#include <ags-client/scripting/ags_script.h>

#include <ags-lib/object/ags_connectable.h>

void ags_script_class_init(AgsScriptClass *script);
void ags_script_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_script_init(AgsScript *script);
void ags_script_connect(AgsConnectable *connectable);
void ags_script_disconnect(AgsConnectable *connectable);
void ags_script_finalize(GObject *gobject);



static gpointer ags_script_parent_class = NULL;

GType
ags_script_get_type()
{
  static GType ags_type_script = 0;

  if(!ags_type_script){
    static const GTypeInfo ags_script_info = {
      sizeof (AgsScriptClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_script_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsScript),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_script_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_script_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_script = g_type_register_static(G_TYPE_OBJECT,
					     "AgsScript\0",
					     &ags_script_info,
					     0);

    g_type_add_interface_static(ags_type_script,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_script);
}

void
ags_script_class_init(AgsScriptClass *script)
{
  GObjectClass *gobject;

  ags_script_parent_class = g_type_class_peek_parent(script);

  /* GObjectClass */
  gobject = (GObjectClass *) script;

  gobject->finalize = ags_script_finalize;
}

void
ags_script_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_script_connect;
  connectable->disconnect = ags_script_disconnect;
}

void
ags_script_init(AgsScript *script)
{
  script->xml_script_factory = ags_xml_script_factory_new();
  ags_xml_script_factory_create_prototype(script->xml_script_factory);

  script->filename = NULL;
  script->file = NULL;
}

void
ags_script_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_script_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_script_finalize(GObject *gobject)
{
  AgsScript *script;

  script = AGS_SCRIPT(gobject);

  G_OBJECT_CLASS(ags_script_parent_class)->finalize(gobject);
}

void
ags_script_read(AgsScript *script)
{
  //TODO:JK: implement me
}

void
ags_script_write(AgsScript *script)
{
  //TODO:JK: implement me
}

void
ags_script_launch(AgsScript *script)
{
  //TODO:JK: implement me
}

AgsScript*
ags_script_new()
{
  AgsScript *script;

  script = (AgsScript *) g_object_new(AGS_TYPE_SCRIPT,
				      NULL);

  return(script);
}
