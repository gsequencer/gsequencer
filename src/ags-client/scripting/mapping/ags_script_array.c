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

#include <ags-client/scripting/mapping/ags_script_array.h>

#include <libxml/tree.h>
#include <string.h>

#include <ags-lib/object/ags_connectable.h>

void ags_script_array_class_init(AgsScriptArrayClass *script_array);
void ags_script_array_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_script_array_init(AgsScriptArray *script_array);
void ags_script_array_connect(AgsConnectable *connectable);
void ags_script_array_disconnect(AgsConnectable *connectable);
void ags_script_array_finalize(GObject *gobject);

void ags_script_array_launch(AgsScriptObject *script_object);

static gpointer ags_script_array_parent_class = NULL;

GType
ags_script_array_get_type()
{
  static GType ags_type_script_array = 0;

  if(!ags_type_script_array){
    static const GTypeInfo ags_script_array_info = {
      sizeof (AgsScriptArrayClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_script_array_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsScriptArray),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_script_array_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_script_array_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_script_array = g_type_register_static(AGS_TYPE_SCRIPT_OBJECT,
						   "AgsScriptArray\0",
						   &ags_script_array_info,
						   0);
    
    g_type_add_interface_static(ags_type_script_array,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_script_array);
}

void
ags_script_array_class_init(AgsScriptArrayClass *script_array)
{
  GObjectClass *gobject;

  ags_script_array_parent_class = g_type_class_peek_parent(script_array);

  /* GObjectClass */
  gobject = (GObjectClass *) script_array;

  gobject->finalize = ags_script_array_finalize;

  /* AgsScriptArray */
}

void
ags_script_array_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_script_array_connect;
  connectable->disconnect = ags_script_array_disconnect;
}

void
ags_script_array_init(AgsScriptArray *script_array)
{
  script_array->flags |= (AGS_SCRIPT_ARRAY_CHAR |
			  AGS_SCRIPT_ARRAY_BASE64);

  script_array->array = NULL;
  script_array->dimension = 0;

  script_array->data = NULL;
  script_array->nodes = NULL;
  script_array->length = NULL;
}

void
ags_script_array_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_script_array_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_script_array_finalize(GObject *gobject)
{
  AgsScriptArray *script_array;

  script_array = AGS_SCRIPT_ARRAY(gobject);

  G_OBJECT_CLASS(ags_script_array_parent_class)->finalize(gobject);
}

AgsScriptObject*
ags_script_array_launch(AgsScriptObject *script_object, GError **error)
{
  AgsScriptArray *script_array;
  guint i;

  auto gpointer ags_script_array_alloc_recursive(AgsScriptArray *script_array, xmlNode *node);
  auto void ags_script_array_read(AgsScriptArray *script_array, xmlNode *node, gpointer array);

  gpointer ags_script_array_alloc_recursive(AgsScriptArray *script_array, xmlNode *node){
    xmlNode *current;
    gpointer start, array;
    guint length;
    guint i;

    current = node->children;
    length = strtoul(xmlGetProp(node, "length\0"));

    if(current == NULL){
      guint index;
      
      index = script_array->dimension;      
      script_array->dimension += 1;

      script_array->data = realloc(script_array->data, script_array->dimension * sizeof(gpointer));
      script_array->data[index] = NULL;

      script_array->node = realloc(script_array->node, script_array->dimension * sizeof(gpointer));
      script_array->node[index] = NULL;

      script_array->length = realloc(script_array->length, script_array->dimension * sizeof(gpointer));
      script_array->length[index] = 0;

      switch(script_array->mode){
      case AGS_SCRIPT_ARRAY_INT16:
	{
	  start =
	    array = (gpointer) malloc(length * sizeof(gint16));
	}
	break;
      case AGS_SCRIPT_ARRAY_UINT16:
	{
	  start =
	    array = (gpointer) malloc(length * sizeof(guint16));
	}
	break;
      case AGS_SCRIPT_ARRAY_INT32:
	{
	  start =
	    array = (gpointer) malloc(length * sizeof(gint32));
	}
	break;
      case AGS_SCRIPT_ARRAY_UINT32:
	{
	  start =
	    array = (gpointer) malloc(length * sizeof(guint32));
	}
	break;
      case AGS_SCRIPT_ARRAY_INT64:
	{
	  start =
	    array = (gpointer) malloc(length * sizeof(gint64));
	}
	break;
      case AGS_SCRIPT_ARRAY_UINT64:
	{
	  start =
	    array = (gpointer) malloc(length * sizeof(guint64));
	}
	break;
      case AGS_SCRIPT_ARRAY_CHAR:
	{
	  start =
	    array = (gpointer) malloc(length * sizeof(char));
	}
	break;
      case AGS_SCRIPT_ARRAY_POINTER:
	{
	  start =
	    array = (gpointer) malloc(length * sizeof(gpointer));
	}
	break;
      }

    }else{
      start =
	array = (gpointer) malloc(length * sizeof(gpointer));

      for(i = 0; i < length;){
	if(current->type != XML_ELEMENT){
	  current = current->next;
	  continue;
	}

	if(current == NULL){
	  guint index;

	  index = script_array->dimension;
	  script_array->dimension += 1;

	  (gpointer) (*array) = NULL;

	  script_array->data = realloc(script_array->data, script_array->dimension * sizeof(gpointer));
	  script_array->data[index] = NULL;

	  script_array->node = realloc(script_array->node, script_array->dimension * sizeof(gpointer));
	  script_array->node[index] = NULL;

	  script_array->length = realloc(script_array->length, script_array->dimension * sizeof(gpointer));
	  script_array->length[index] = 0;
	}else{
	  (gpointer) (*array) = ags_script_array_alloc_recursive(script_array, current);
	  current = current->next;
	}

	array++;
	i++;
      }
    }

    return(start);
  }
  void ags_script_array_read(AgsScriptArray *script_array, xmlNode *node, gpointer array){
    //TODO:JK: implement me
  }

  script_array = AGS_SCRIPT_ARRAY(script_object);

  if((AGS_SCRIPT_OBJECT_LAUNCHED & (script_object->flags)) == 0){
    script_array->array = ags_script_array_alloc_recursive(script_array, script_object->node);

    script_object->flags |= AGS_SCRIPT_OBJECT_LAUNCHED;
  }

  if((AGS_SCRIPT_OBJECT_RETVAL_RESET & (script_object->flags)) != 0){
    //TODO:JK: implement me

    script_object->flags &= (~AGS_SCRIPT_ARRAY_OBJECT_RETVAL_RESET);
  }

  return(script_object);
}

AgsScriptArray*
ags_script_array_new()
{
  AgsScriptArray *script_array;

  script_array = (AgsScriptArray *) g_object_new(AGS_TYPE_SCRIPT_ARRAY,
						 NULL);

  return(script_array);
}
