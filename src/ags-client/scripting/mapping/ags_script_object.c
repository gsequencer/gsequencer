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

#include <ags-client/scripting/mapping/ags_script_object.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags-client/object/ags_marshal.h>

#include <ags-client/scripting/ags_script.h>

#include <ags-client/scripting/mapping/ags_script_var.h>

#include <libxml/tree.h>
#include <libxml/xmlstring.h>

#include <stdlib.h>
#include <string.h>

void ags_script_object_class_init(AgsScriptObjectClass *script_object);
void ags_script_object_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_script_object_init(AgsScriptObject *script_object);
void ags_script_object_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_script_object_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_script_object_connect(AgsConnectable *connectable);
void ags_script_object_disconnect(AgsConnectable *connectable);
void ags_script_object_finalize(GObject *gobject);

void ags_script_object_real_map_xml(AgsScriptObject *script_object);
AgsScriptObject* ags_script_object_real_launch(AgsScriptObject *script_object);
AgsScriptObject* ags_script_object_real_tostring(AgsScriptObject *script_object);
AgsScriptObject* ags_script_object_real_valueof(AgsScriptObject *script_object,
						GError **error);
gchar** ags_script_object_split_xpath(gchar *xpath, guint *name_length);
guint* ags_script_object_read_index(gchar *xpath, guint *index_length);
guint ags_script_object_count_retval(AgsScriptObject *script_object);
AgsScriptObject* ags_script_object_nth_retval(AgsScriptObject *script_object, guint nth);
AgsScriptObject* ags_script_object_find_flags_descending_first_match(AgsScriptObject *script_object,
								     guint flags,
								     guint z_index);
AgsScriptObject* ags_script_object_find_flags_descending_last_match(AgsScriptObject *script_object,
								    guint flags,
								    guint z_index);

enum{
  MAP_XML,
  LAUNCH,
  TOSTRING,
  VALUEOF,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_SCRIPT,
};

static gpointer ags_script_object_parent_class = NULL;
static guint script_object_signals[LAST_SIGNAL];

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
  GParamSpec *param_spec;

  ags_script_object_parent_class = g_type_class_peek_parent(script_object);

  /* GObjectClass */
  gobject = (GObjectClass *) script_object;

  gobject->set_property = ags_script_object_set_property;
  gobject->get_property = ags_script_object_get_property;

  gobject->finalize = ags_script_object_finalize;

  /* properties */
  param_spec = g_param_spec_object("script\0",
				   "script running in\0",
				   "The script where this script object belongs to\0",
				   AGS_TYPE_SCRIPT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCRIPT,
				  param_spec);

  /* AgsScriptObjectClass */
  script_object->map_xml = ags_script_object_real_map_xml;
  script_object->launch = ags_script_object_real_launch;
  script_object->tostring = ags_script_object_real_tostring;
  script_object->valueof = ags_script_object_real_valueof;

  /* signals */
  script_object_signals[MAP_XML] =
    g_signal_new("map_xml\0",
		 G_TYPE_FROM_CLASS(script_object),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsScriptObjectClass, map_xml),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  script_object_signals[LAUNCH] =
    g_signal_new("launch\0",
		 G_TYPE_FROM_CLASS(script_object),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsScriptObjectClass, launch),
		 NULL, NULL,
		 g_cclosure_user_marshal_OBJECT__VOID,
		 G_TYPE_OBJECT, 0);

  script_object_signals[TOSTRING] =
    g_signal_new("tostring\0",
		 G_TYPE_FROM_CLASS(script_object),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsScriptObjectClass, tostring),
		 NULL, NULL,
		 g_cclosure_user_marshal_OBJECT__VOID,
		 G_TYPE_OBJECT, 0);

  script_object_signals[VALUEOF] =
    g_signal_new("valueof\0",
		 G_TYPE_FROM_CLASS(script_object),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsScriptObjectClass, valueof),
		 NULL, NULL,
		 g_cclosure_user_marshal_OBJECT__VOID,
		 G_TYPE_OBJECT, 0);
}

void
ags_script_object_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_script_object_connect;
  connectable->disconnect = ags_script_object_disconnect;
}

GQuark
ags_script_object_error_quark()
{
  return(g_quark_from_static_string("ags-script-object-error-quark\0"));
}

void
ags_script_object_init(AgsScriptObject *script_object)
{
  script_object->node = NULL;
  script_object->id = NULL;

  script_object->retval = NULL;
}

void
ags_script_object_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsScriptObject *script_object;

  script_object = AGS_SCRIPT_OBJECT(gobject);

  switch(prop_id){
  case PROP_SCRIPT:
    {
      AgsScript *script;

      script = (AgsScript *) g_value_get_object(value);

      if(script == ((AgsScript *) script_object->script))
	return;

      if(script_object->script != NULL)
	g_object_unref(script_object->script);

      if(script != NULL)
	g_object_ref(G_OBJECT(script));

      script_object->script = (GObject *) script;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_script_object_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsScriptObject *script_object;

  script_object = AGS_SCRIPT_OBJECT(gobject);

  switch(prop_id){
  case PROP_SCRIPT:
    {
      g_value_set_object(value, script_object->script);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
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
  AgsScript *script;
  GError *error;

  script = AGS_SCRIPT(script_object->script);

  error = NULL;
  script_object->node = ags_xml_script_factory_map(script->xml_script_factory,
						   script_object,
						   &error);

  if(error != NULL){
    g_warning(error->message);
  }
}

void
ags_script_object_map_xml(AgsScriptObject *script_object)
{
  g_return_if_fail(AGS_IS_SCRIPT_OBJECT(script_object));

  g_object_ref(G_OBJECT(script_object));
  g_signal_emit(G_OBJECT(script_object),
		script_object_signals[MAP_XML], 0);
  g_object_unref(G_OBJECT(script_object));
}

AgsScriptObject*
ags_script_object_real_launch(AgsScriptObject *script_object)
{
  //TODO:JK: implement me
}

AgsScriptObject*
ags_script_object_launch(AgsScriptObject *script_object)
{
  AgsScriptObject *retval;

  g_return_val_if_fail(AGS_IS_SCRIPT_OBJECT(script_object), NULL);

  g_object_ref(G_OBJECT(script_object));
  g_signal_emit(G_OBJECT(script_object),
		script_object_signals[LAUNCH], 0,
		&retval);
  g_object_unref(G_OBJECT(script_object));

  return(retval);
}

AgsScriptObject*
ags_script_object_tostring(AgsScriptObject *script_object)
{
  AgsScriptObject *retval;

  g_return_val_if_fail(AGS_IS_SCRIPT_OBJECT(script_object), NULL);

  g_object_ref(G_OBJECT(script_object));
  g_signal_emit(G_OBJECT(script_object),
		script_object_signals[TOSTRING], 0,
		&retval);
  g_object_unref(G_OBJECT(script_object));

  return(retval);
}

AgsScriptObject*
ags_script_object_real_tostring(AgsScriptObject *script_object)
{
  AgsScriptVar *retval;

  if((AGS_SCRIPT_OBJECT_LAUNCHED & (script_object->flags)) != 0){
    retval = AGS_SCRIPT_VAR(ags_script_object_valueof(script_object));
  }else{
    retval = ags_script_var_new();

    xmlSetProp(AGS_SCRIPT_OBJECT(retval)->node, "type\0", "char\0");
  }

  return((AgsScriptObject *) retval);
}

AgsScriptObject*
ags_script_object_real_valueof(AgsScriptObject *script_object,
			       GError **error)
{
  AgsScriptObject *first_match, *last_match, *current;
  GList *node_list;
  guint retval_count;
  gchar *xpath;
  gchar **name;
  guint *index;
  guint index_length;
  guint name_length;
  xmlNode *node;
  guint i, j, k;
  guint z_index;
  guint current_node_count;
  gboolean is_node_after_first_match, is_node_after_last_match;

  /* entry */
  xpath = xmlGetProp(script_object->node, "retval\0");

  if((first_match = ags_script_object_find_flags_descending_first_match(script_object,
									AGS_SCRIPT_OBJECT_LAUNCHED,
									strtoul(xmlGetProp(script_object->node, "z_index\0"), NULL, 10))) == NULL){
    return(NULL);
  }

  last_match = ags_script_object_find_flags_descending_last_match(script_object,
								  AGS_SCRIPT_OBJECT_LAUNCHED,
								  strtoul(xmlGetProp(script_object->node, "z_index\0"), NULL, 10));

  retval_count = ags_script_object_count_retval(script_object);

  name = ags_script_object_split_xpath(xpath, &name_length);
  index = ags_script_object_read_index(xpath,
				       &index_length);

  if(index_length > retval_count){
    guint prefix_length;

    current = script_object;
    node_list = NULL;

    is_node_after_first_match = FALSE;
    is_node_after_last_match = TRUE;

    for(i = 0, j = 0; i < index_length; i++){

      if(current == first_match){
	is_node_after_first_match = TRUE;
      }

      if(current == NULL){
	/* set error */
	g_set_error(error,
		    AGS_SCRIPT_OBJECT_ERROR,
		    AGS_SCRIPT_OBJECT_INDEX_EXCEEDED,
		    "can't access index because it doesn't exist: %d of %d\0",
		    retval_count, retval_count);

	return(NULL);
      }

      /* start */
      if(name[i][1] == '/'){
	prefix_length = 2;

	while(current != NULL){
	  node = current->node;

	  if(xmlStrcmp(node->name, &(name[i][prefix_length])) == 0){
	    current = current->retval;
	    j++;
	  }else{
	    break;
	  }
	}

	if(current == NULL){
	  /* set error */
	  g_set_error(error,
		      AGS_SCRIPT_OBJECT_ERROR,
		      AGS_SCRIPT_OBJECT_INDEX_EXCEEDED,
		      "named child doesn't exist\0");

	  return(NULL);
	}
      }else{
	prefix_length = 1;

	if(!xmlStrcmp(node->name, &(name[i][prefix_length]))){
	  current = current->retval;
	  node = current->node;

	  j++;
	}else{
	  /* set error */
	  g_set_error(error,
		      AGS_SCRIPT_OBJECT_ERROR,
		      AGS_SCRIPT_OBJECT_INDEX_EXCEEDED,
		      "named child doesn't exist\0");
	}
      }

      /* position */
      z_index = strtoul(xmlGetProp(node, "z_index\0"), NULL, 10);

      for(k = 0; k < index[i] && current != NULL; j++){
	node = current->node;
	current = current->retval;

	if(!xmlStrcmp(node->name, &(name[i][prefix_length])) && z_index == strtoul(xmlGetProp(node, "z_index\0"), NULL, 10)){
	  k++;
	}
      }

      node_list = g_list_prepend(node_list,
				 node);

      if(current == last_match && 
	 k != index[i]){
	/* set error */
	g_set_error(error,
		    AGS_SCRIPT_OBJECT_ERROR,
		    AGS_SCRIPT_OBJECT_INDEX_EXCEEDED,
		    "can't access index because it doesn't exist: %d of %d\0",
		    retval_count, retval_count);

	return(NULL);
      }
    }

    if(!is_node_after_first_match){
      /* set error */
      g_set_error(error,
		  AGS_SCRIPT_OBJECT_ERROR,
		  AGS_SCRIPT_OBJECT_INDEX_EXCEEDED,
		  "can't access index because it doesn't exist: %d of %d\0",
		  -1, index_length);

      return(NULL);
    }
  }else{
    /* set error */
    g_set_error(error,
		AGS_SCRIPT_OBJECT_ERROR,
		AGS_SCRIPT_OBJECT_INDEX_EXCEEDED,
		"can't access index because it doesn't exist: %d of %d\0",
		-1, index_length);

    return(NULL);
  }

  if(j > 0){
    return(current);
  }else{
    return(first_match);
  }
}

AgsScriptObject*
ags_script_object_valueof(AgsScriptObject *script_object)
{
  AgsScriptObject *retval;

  g_return_val_if_fail(AGS_IS_SCRIPT_OBJECT(script_object), NULL);

  g_object_ref(G_OBJECT(script_object));
  g_signal_emit(G_OBJECT(script_object),
		script_object_signals[VALUEOF], 0,
		&retval);
  g_object_unref(G_OBJECT(script_object));

  return(retval);
}

gchar**
ags_script_object_split_xpath(gchar *xpath, guint *name_length)
{
  gchar **name;
  gchar *offset, *prev_offset;
  guint i, start;

  name = (gchar **) malloc(sizeof(gchar*));

  prev_offset = xpath;

  if(xpath[0] == '('){
    start++;
    prev_offset = &(xpath[1]);
      
    if(xpath[1] == '/'){
      start++;

      if(xpath[2] == '/'){
	start++;
      }
    }
  }else{
    if(xpath[0] == '/'){
      start++;

      if(xpath[1] == '/'){
	start++;
      }
    }
  }

  offset = &(xpath[start]);

  i = 1;

  while((offset = strchr(offset, '/')) != NULL && offset[0] != '\0'){
    name = (gchar **) realloc(name, (i) * sizeof(gchar *));

    name[i - 1] = g_strndup(offset, offset - prev_offset);

    if(offset[1] == '/'){
      offset = &(offset[2]);
    }else{
      offset = &(offset[1]);
    }

    i++;
  }

  *name_length = i;

  return(name);
}

guint*
ags_script_object_read_index(gchar *xpath, guint *index_length)
{
  guint *index;
  gchar *offset;
  guint i;

  index = NULL;
  offset = xpath;
  i = 0;

  while((offset = strchr(offset, '[')) != NULL && offset[0] != '\0'){
    if(i == 0){
      index = (guint *) malloc(sizeof(guint));
    }else{
      index = (guint *) realloc(index, (i + 1) * sizeof(guint));
    }

    sscanf(xpath, "%d\0", &(index[i]));

    offset = &(offset[1]);
    i++;
  }

  *index_length = i;

  return(index);
}

guint
ags_script_object_count_retval(AgsScriptObject *script_object)
{
  AgsScriptObject *current;
  guint i;

  current = script_object;

  for(i = 0; current != NULL; i++) current = current->retval;

  return(i);
}

AgsScriptObject*
ags_script_object_nth_retval(AgsScriptObject *script_object, guint nth)
{
  AgsScriptObject *current;
  guint i;

  current = script_object;

  for(i = 0; i < nth && current != NULL; i++) current = current->retval;

  return(current);
}

AgsScriptObject*
ags_script_object_find_flags_descending_first_match(AgsScriptObject *script_object,
						    guint flags,
						    guint z_index)
{
  AgsScriptObject *current, *value;
  guint inverse_flags;
  gboolean found_z_index;

  current = script_object->retval;
  inverse_flags = ~flags;

  found_z_index = FALSE;

  while(current != NULL){
    if(!found_z_index && z_index == strtoul(xmlGetProp(current->node, "z_index\0"), NULL, 10)){
      found_z_index = TRUE;
    }

    if(found_z_index && z_index > strtoul(xmlGetProp(current->node, "z_index\0"), NULL, 10)){
      break;
    }
    
    if(found_z_index && ((inverse_flags) & (flags | (script_object->flags))) == 0){
      return(current);
    }

    current = current->retval;
  }

  return(NULL);
}

AgsScriptObject*
ags_script_object_find_flags_descending_last_match(AgsScriptObject *script_object,
						   guint flags,
						   guint z_index)
{
  AgsScriptObject *current, *last_match, *value;
  guint inverse_flags;
  gboolean found_z_index;

  current = script_object->retval;
  inverse_flags = ~flags;
  
  last_match = NULL;
  found_z_index = FALSE;

  while(current != NULL){
    if(!found_z_index && z_index == strtoul(xmlGetProp(current->node, "z_index\0"), NULL, 10)){
      found_z_index = TRUE;
    }

    if(found_z_index && z_index > strtoul(xmlGetProp(current->node, "z_index\0"), NULL, 10)){
      break;
    }

    if(found_z_index && ((inverse_flags) & (flags | (script_object->flags))) == 0){
      last_match = current;
    }

    current = current->retval;
  }

  return(last_match);
}

AgsScriptObject*
ags_script_object_new(GObject *script)
{
  AgsScriptObject *script_object;

  script_object = (AgsScriptObject *) g_object_new(AGS_TYPE_SCRIPT_OBJECT,
						   "script\0", script,
						   NULL);
  
  return(script_object);
}
