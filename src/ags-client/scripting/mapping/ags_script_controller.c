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

#include <ags-client/scripting/mapping/ags_script_controller.h>

#include <ags/object/ags_connectable.h>

#include <ags-client/scripting/ags_xml_interpreter.h>
#include <ags-client/scripting/ags_xml_script_factory.h>

#include <libxml/tree.h>

#include <stdlib.h>
#include <string.h>

void ags_script_controller_class_init(AgsScriptControllerClass *script_controller);
void ags_script_controller_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_script_controller_init(AgsScriptController *script_controller);
void ags_script_controller_connect(AgsConnectable *connectable);
void ags_script_controller_disconnect(AgsConnectable *connectable);
void ags_script_controller_finalize(GObject *gobject);

AgsScriptObject* ags_script_controller_launch(AgsScriptObject *script_object, GError **error);

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
  AgsScriptObjectClass *script_object;
  GObjectClass *gobject;

  ags_script_controller_parent_class = g_type_class_peek_parent(script_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) script_controller;

  gobject->finalize = ags_script_controller_finalize;

  /* AgsScriptObjectClass */
  script_object = (AgsScriptObjectClass *) script_controller;

  script_object->launch = ags_script_controller_launch;
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

AgsScriptObject*
ags_script_controller_launch(AgsScriptObject *script_object, GError **error)
{
  AgsScriptObject *retval, *last_retval;
  xmlNode *current;
  guint z_index;

  z_index = strtoul(xmlGetProp(script_object->node, "z_index\0"), NULL, 10);
  z_index++;

  if((AGS_SCRIPT_OBJECT_LAUNCHED & (script_object->flags)) == 0){
    current = script_object->node->children;

    while(current != NULL){
      if(z_index != strtoul(xmlGetProp(retval->node, "z_index\0"), NULL, 10)){
	break;
      }

      if(XML_ELEMENT_NODE == current->type){
	ags_script_object_launch(retval, error);
      }

      last_retval = retval;
      retval = retval->retval;

      current = current->next;
    }

    /* check if stack is outdated */
    /* create new AgsScriptObject */
    if(current != NULL){
      AgsScriptObject *next_retval;
      GType type;

      while(current != NULL){
	if(z_index != strtoul(xmlGetProp(retval->node, "z_index\0"), NULL, 10)){
	  break;
	}

	if(XML_ELEMENT_NODE == current->type){
	  /* instantiate on the fly */
	  type = ags_xml_interpreter_type_from_name((gchar *) current->name);
	  next_retval = (AgsScriptObject *) g_object_new(type,
							 NULL);
	  
	  next_retval->script = retval->script;
	  
	  /* launch */
	  retval->retval = next_retval;
	  retval = next_retval;
	  
	  ags_script_object_launch(retval, error);

	  last_retval = retval;
	}

	current = current->next;
      }
    }

    /* report invalid z_index */
    if(current != NULL){
      while(current != NULL){
	if(XML_ELEMENT_NODE == current->type){
	  g_message("found inactive node: %s\0", current->name);
	}

	current = current->next;
      }
    }

    /* unref unneeded AgsScriptObject */
    if(z_index == strtoul(xmlGetProp(retval->node, "z_index\0"), NULL, 10)){
      AgsScriptObject *start, *next_retval;

      start = last_retval;

      while(z_index == strtoul(xmlGetProp(retval->node, "z_index\0"), NULL, 10)){
	next_retval = retval->retval;

	g_message("found lost object: %s\0", G_OBJECT_TYPE_NAME(retval));
	g_object_unref(G_OBJECT(retval));

	retval = next_retval;
      }
      
      start->retval = retval;
    }
  }

  return(last_retval);
}

AgsScriptController*
ags_script_controller_new()
{
  AgsScriptController *script_controller;

  script_controller = (AgsScriptController *) g_object_new(AGS_TYPE_SCRIPT_CONTROLLER,
							   NULL);

  return(script_controller);
}
