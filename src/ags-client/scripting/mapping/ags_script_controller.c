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

#include <ags-client/scripting/ags_script_controller.h>

#include <ags-lib/object/ags_connectable.h>

void ags_script_controller_class_init(AgsScriptControllerClass *script_controller);
void ags_script_controller_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_script_controller_init(AgsScriptController *script_controller);
void ags_script_controller_connect(AgsConnectable *connectable);
void ags_script_controller_disconnect(AgsConnectable *connectable);
void ags_script_controller_finalize(GObject *gobject);

static gpointer ags_script_controller_parent_class = NULL;

GType
ags_script_controller_get_type()
{
  static GType ags_type_script_controller = 0;

  if(!ags_type_script_controller){
    static const GTypeInfo ags_script_controller_info = {
      sizeof (AgsScriptControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_script_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsScriptController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_script_controller_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_script_controller_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_script_controller = g_type_register_static(G_TYPE_OBJECT,
							"AgsScriptController\0",
							&ags_script_controller_info,
							0);
    
    g_type_add_interface_static(ags_type_script_controller,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_script_controller);
}

void
ags_script_controller_class_init(AgsScriptControllerClass *script_controller)
{
  GObjectClass *gobject;

  ags_script_controller_parent_class = g_type_class_peek_parent(script_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) script_controller;

  gobject->finalize = ags_script_controller_finalize;
}

void
ags_script_controller_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_script_controller_connect;
  connectable->disconnect = ags_script_controller_disconnect;
}

void
ags_script_controller_init(AgsScriptController *script_controller)
{
  //TODO:JK: implement me
}

void
ags_script_controller_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_script_controller_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_script_controller_finalize(GObject *gobject)
{
  AgsScriptController *script_controller;

  script_controller = AGS_SCRIPT_CONTROLLER(gobject);

  G_OBJECT_CLASS(ags_script_controller_parent_class)->finalize(gobject);
}

AgsScriptController*
ags_script_controller_new()
{
  AgsScriptController *script_controller;

  script_controller = (AgsScriptController *) g_object_new(AGS_TYPE_SCRIPT_CONTROLLER,
							   NULL);

  return(script_controller);
}
