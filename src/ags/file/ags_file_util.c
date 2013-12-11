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

#include <ags/file/ags_file_util.h>

#include <ags/util/ags_id_generator.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

void ags_file_util_read_value_resolve(AgsFileLookup *file_lookup,
				      GValue *value);
void ags_file_util_write_value_resolve(AgsFileLookup *file_lookup,
				       GValue *value);

void
ags_file_util_read_value(AgsFile *file,
			 xmlNode *node, gchar **id,
			 GValue *value, xmlChar **xpath)
{
  AgsFileLookup *file_lookup;
  xmlChar *type_str;
  xmlChar *value_str;
  xmlChar *content;
  GValue a = G_VALUE_INIT;

  if(id != NULL)
    *id = xmlGetProp(node, AGS_FILE_ID_PROP);

  type_str = xmlGetProp(node, AGS_FILE_TYPE_PROP);

  content = xmlNodeGetContent(node);

  if(!xmlStrcmp(type_str, AGS_FILE_BOOLEAN_PROP)){
    g_value_init(&a, G_TYPE_BOOLEAN);
    g_value_set_boolean(&a, g_ascii_strtoll(content, NULL, 10));

    if(value != NULL)
      g_value_copy(&a, value);
    else
      g_value_unset(&a);

    if(xpath != NULL)
      *xpath = NULL;
  }else if(!xmlStrcmp(type_str, AGS_FILE_CHAR_PROP)){
    g_value_init(&a, G_TYPE_CHAR);
    g_value_set_schar(&a, content[0]);

    if(value != NULL)
      g_value_copy(&a, value);
    else
      g_value_unset(&a);

    if(xpath != NULL)
      *xpath = NULL;
  }else if(!xmlStrcmp(type_str, AGS_FILE_INT64_PROP)){
    g_value_init(&a, G_TYPE_INT64);
    g_value_set_int64(&a, g_ascii_strtoll(content, NULL, 10));

    if(value != NULL)
      g_value_copy(&a, value);
    else
      g_value_unset(&a);

    if(xpath != NULL)
      *xpath = NULL;
  }else if(!xmlStrcmp(type_str, AGS_FILE_UINT64_PROP)){
    g_value_init(&a, G_TYPE_UINT64);
    g_value_set_uint64(&a, g_ascii_strtoull(content, NULL, 10));

    if(value != NULL)
      g_value_copy(&a, value);
    else
      g_value_unset(&a);

    if(xpath != NULL)
      *xpath = NULL;
  }else if(!xmlStrcmp(type_str, AGS_FILE_DOUBLE_PROP)){
    g_value_init(&a, G_TYPE_DOUBLE);
    g_value_set_double(&a, g_ascii_strtod(content, NULL));

    if(value != NULL)
      g_value_copy(&a, value);
    else
      g_value_unset(&a);

    if(xpath != NULL)
      *xpath = NULL;
  }else if(!xmlStrcmp(type_str, AGS_FILE_CHAR_POINTER_PROP)){
    g_value_init(&a, G_TYPE_STRING);
    g_value_set_static_string(&a, content);

    if(value != NULL)
      g_value_copy(&a, value);
    else
      g_value_unset(&a);

    if(xpath != NULL)
      *xpath = NULL;
  }else if(!xmlStrcmp(type_str, AGS_FILE_BOOLEAN_POINTER_PROP)){
    gchar **str_arr, **str_iter;
    gboolean *arr;
    guint i;

    str_arr = g_strsplit(content, " \0", -1);
    
    g_value_init(&a, G_TYPE_POINTER);

    arr = NULL;
    str_iter = str_arr;
    i = 0;

    while(*str_iter != NULL){
      if(arr == NULL){
	arr = (gboolean *) malloc(sizeof(gboolean));
      }else{
	arr = (gboolean *) realloc(arr, (i + 1) * sizeof(gboolean));
      }

      if(!xmlStrcmp(*str_iter, AGS_FILE_TRUE)){
	arr[i] = TRUE;
      }else{
	arr[i] = FALSE;
      }

      str_iter++;
      i++;
    }

    g_value_set_pointer(&a, (gpointer) arr);
    g_free(str_arr);

    if(value != NULL)
      g_value_copy(&a, value);
    else
      g_value_unset(&a);
    
    if(xpath != NULL)
      *xpath = NULL;
  }else if(!xmlStrcmp(type_str, AGS_FILE_INT64_POINTER_PROP)){
    gchar **str_arr, **str_iter;
    gint64 *arr;
    guint i;

    str_arr = g_strsplit(content, " \0", -1);
    
    g_value_init(&a, G_TYPE_POINTER);

    arr = NULL;
    str_iter = str_arr;
    i = 0;

    while(*str_iter != NULL){
      if(arr == NULL){
	arr = (gint64 *) malloc(sizeof(gint64));
      }else{
	arr = (gint64 *) realloc(arr, (i + 1) * sizeof(gint64));
      }

      arr[i] = g_ascii_strtoll(*str_iter, NULL, 10);

      str_iter++;
      i++;
    }
    
    g_value_set_pointer(&a, (gpointer) arr);
    g_free(str_arr);

    if(value != NULL)
      g_value_copy(&a, value);
    else
      g_value_unset(&a);
    
    if(xpath != NULL)
      *xpath = NULL;
  }else if(!xmlStrcmp(type_str, AGS_FILE_UINT64_POINTER_PROP)){
    gchar **str_arr, **str_iter;
    guint64 *arr;
    guint i;

    str_arr = g_strsplit(content, " \0", -1);

    g_value_init(&a, G_TYPE_POINTER);

    arr = NULL;
    str_iter = str_arr;
    i = 0;

    while(*str_iter != NULL){
      if(arr == NULL){
	arr = (guint64 *) malloc(sizeof(guint64));
      }else{
	arr = (guint64 *) realloc(arr, (i + 1) * sizeof(guint64));
      }

      arr[i] = g_ascii_strtoll(*str_iter, NULL, 10);

      str_iter++;
      i++;
    }
    
    g_value_set_pointer(&a, (gpointer) arr);
    g_free(str_arr);

    if(value != NULL)
      g_value_copy(&a, value);
    else
      g_value_unset(&a);
    
    if(xpath != NULL)
      *xpath = NULL;
  }else if(!xmlStrcmp(type_str, AGS_FILE_DOUBLE_POINTER_PROP)){
    gchar **str_arr, **str_iter;
    gdouble *arr;
    guint i;

    str_arr = g_strsplit(content, " \0", -1);

    g_value_init(&a, G_TYPE_POINTER);

    arr = NULL;
    str_iter = str_arr;
    i = 0;

    while(*str_iter != NULL){
      if(arr == NULL){
	arr = (gdouble *) malloc(sizeof(gdouble));
      }else{
	arr = (gdouble *) realloc(arr, (i + 1) * sizeof(gdouble));
      }

      arr[i] = g_ascii_strtod(*str_iter, NULL);

      str_iter++;
      i++;
    }
    
    g_value_set_pointer(&a, (gpointer) arr);
    g_free(str_arr);

    if(value != NULL)
      g_value_copy(&a, value);
    else
      g_value_unset(&a);
    
    if(xpath != NULL)
      *xpath = NULL;
  }else if(!xmlStrcmp(type_str, AGS_FILE_POINTER_PROP)){
    g_value_init(&a, G_TYPE_POINTER);

    g_value_set_pointer(&a, NULL);

    if(value != NULL)
      g_value_copy(&a, value);
    else
      g_value_unset(&a);

    /* gpointer */
    file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
						 "file\0", file,
						 "node\0", node,
						 "reference\0", value,
						 NULL);
    ags_file_add_lookup(file, (GObject *) file_lookup);
    g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		     G_CALLBACK(ags_file_util_read_value_resolve), value);

    /* xpath */
    if(xpath != NULL)
      *xpath = content;
  }else if(!xmlStrcmp(type_str, AGS_FILE_OBJECT_PROP)){
    g_value_init(&a, G_TYPE_OBJECT);

    g_value_set_object(&a, NULL);
    
    if(value != NULL)
      g_value_copy(&a, value);
    else
      g_value_unset(&a);

    /* GObject */
    file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
						 "file\0", file,
						 "node\0", node,
						 "reference\0", value,
						 NULL);
    ags_file_add_lookup(file, (GObject *) file_lookup);
    g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		     G_CALLBACK(ags_file_util_read_value_resolve), value);

    /* xpath */
    if(xpath != NULL)
      *xpath = content;
  }else{
    g_warning("ags_file_util_read_value: unsupported type\0");
  }
}

void
ags_file_util_read_value_resolve(AgsFileLookup *file_lookup,
				 GValue *value)
{
  AgsFileIdRef *id_ref;
  gchar *xpath;

  xpath = (gchar *) xmlNodeGetContent(file_lookup->node);

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_xpath(file_lookup->file, xpath);

  if(G_VALUE_HOLDS(value, G_TYPE_POINTER)){
    g_value_set_pointer(value, (gpointer) id_ref->ref);
  }else if(G_VALUE_HOLDS(value, G_TYPE_OBJECT)){
    g_value_set_object(value, (GObject *) id_ref->ref);
  }else{
    g_warning("ags_file_util_read_parameter_resolve: unknown type of GValue\0");
  }
}

xmlNode*
ags_file_util_write_value(AgsFile *file,
			  xmlNode *parent, gchar *id,
			  GValue *value, GType pointer_type, guint array_length)
{
  AgsFileLookup *file_lookup;
  xmlNode *node;
  xmlChar *type_str;
  xmlChar *content;

  if(value == NULL){
    return;
  }

  node = xmlNewNode(NULL,
		    "ags-value\0");  
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  content = NULL;

  switch(G_VALUE_TYPE(value)){
  case G_TYPE_CHAR:
    {
      content = BAD_CAST g_strdup_printf("%c\0", g_value_get_schar(value));
      type_str = AGS_FILE_CHAR_PROP;
    }
    break;
  case G_TYPE_BOOLEAN:
    {
      content = BAD_CAST g_strdup_printf("%d\0", g_value_get_boolean(value));
      type_str = AGS_FILE_BOOLEAN_PROP;
    }
    break;
  case G_TYPE_INT64:
    {
      content = BAD_CAST g_strdup_printf("%lld\0", g_value_get_int64(value));
      type_str = AGS_FILE_INT64_PROP;
    }
    break;
  case G_TYPE_UINT64:
    {
      content = BAD_CAST g_strdup_printf("%lld\0", g_value_get_uint64(value));
      type_str = AGS_FILE_UINT64_PROP;
    }
    break;
  case G_TYPE_DOUBLE:
    {
      content = BAD_CAST g_strdup_printf("%lf\0", g_value_get_double(value));
      type_str = AGS_FILE_DOUBLE_PROP;
    }
    break;
  case G_TYPE_STRING:
    {
      content = BAD_CAST g_strdup_printf("%s\0", g_value_get_string(value));
      type_str = AGS_FILE_CHAR_POINTER_PROP;
    }
    break;
  case G_TYPE_POINTER:
    {
      gchar *str;
      guint i;

      if(pointer_type == G_TYPE_CHAR){
	content = BAD_CAST g_strdup_printf("%s\0", g_value_get_string(value));
	type_str = AGS_FILE_CHAR_POINTER_PROP;
      }else if(pointer_type == G_TYPE_INT64){
	gint64 *arr;

	arr = (gint64 *) g_value_get_pointer(value);

	for(i = 0; i < array_length; i++){
	  if(i == 0){
	    content = g_strdup_printf("%d\0", arr[i]);
	  }else{
	    str = content;

	    content = g_strdup_printf("%s %d\0", str, arr[i]);

	    g_free(str);
	  }
	}

	type_str = AGS_FILE_INT64_POINTER_PROP;
      }else if(pointer_type == G_TYPE_UINT64){
	guint64 *arr;

	arr = (guint64 *) g_value_get_pointer(value);

	for(i = 0; i < array_length; i++){
	  if(i == 0){
	    content = g_strdup_printf("%d\0", arr[i]);
	  }else{
	    str = content;

	    content = g_strdup_printf("%s %d\0", str, arr[i]);

	    g_free(str);
	  }
	}

	type_str = AGS_FILE_UINT64_POINTER_PROP;
      }else if(pointer_type == G_TYPE_DOUBLE){
	gdouble *arr;

	arr = (gdouble *) g_value_get_pointer(value);

	for(i = 0; i < array_length; i++){
	  if(i == 0){
	    content = g_strdup_printf("%f\0", arr[i]);
	  }else{
	    str = content;

	    content = g_strdup_printf("%s %f\0", str, arr[i]);

	    g_free(str);
	  }
	}

	type_str = AGS_FILE_DOUBLE_POINTER_PROP;
      }else{
	file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
						     "file\0", file,
						     "node\0", node,
						     "reference\0", value,
						     NULL);
	ags_file_add_lookup(file, (GObject *) file_lookup);
	g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
			 G_CALLBACK(ags_file_util_write_value_resolve), value);
      }

      type_str = AGS_FILE_POINTER_PROP;
    }
    break;
  case G_TYPE_OBJECT:
    {
      file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
						   "file\0", file,
						   "node\0", node,
						   "reference\0", value,
						   NULL);
      ags_file_add_lookup(file, (GObject *) file_lookup);
      g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		       G_CALLBACK(ags_file_util_write_value_resolve), value);

      type_str = AGS_FILE_OBJECT_PROP;
    }
    break;
  default:
    g_warning("ags_file_util_write_value\0");
  }

  xmlNewProp(node,
	     AGS_FILE_TYPE_PROP,
	     type_str);

  xmlNodeSetContent(node, content);
  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_file_util_write_value_resolve(AgsFileLookup *file_lookup,
				  GValue *value)
{
  AgsFileIdRef *id_ref;
  gchar *id;

  if(G_VALUE_HOLDS_OBJECT(value)){
    id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file, g_value_get_object(value));
  }else{
    id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file, g_value_get_pointer(value));
  }

  id = xmlGetProp(id_ref->node, AGS_FILE_ID_PROP);

  xmlNewProp(file_lookup->node,
	     "link\0",
	     g_strdup_printf("xpath=*/[@id='%s']\0", id));

  g_value_unset(value);
  g_free(value);
}

void
ags_file_util_read_parameter(AgsFile *file,
			     xmlNode *node, gchar **id,
			     GParameter **parameter, gint *n_params, xmlChar ***xpath)
{
  xmlNode *child;
  GParameter *parameter_arr;
  gchar **name_arr, **name_iter;
  xmlChar **xpath_arr, **xpath_iter;
  guint i;

  name_arr = g_strsplit(xmlGetProp(node, AGS_FILE_NAME_PROP), " \0", -1);

  parameter_arr = NULL;
  name_iter = name_arr;
  xpath_arr = NULL;
  i = 0;
  
  child = node->children;

  while(name_iter != NULL){
    if(parameter_arr == NULL){
      parameter_arr = (GParameter *) g_new(GParameter, 1);
      xpath_arr = (xmlChar **) malloc(sizeof(xmlChar *));
    }else{
      parameter_arr = (GParameter *) g_renew(GParameter, parameter_arr, i + 1);
      xpath_arr = (xmlChar **) realloc(xpath_arr, (i + 1) * sizeof(xmlChar *));
    }

    xpath_iter = xpath_arr + i;

    parameter_arr[i].name = name_arr[i];
    ags_file_util_read_value(file,
			     child, NULL,
			     &(parameter_arr[i].value), (xmlChar **) &xpath_iter);

    child = child->next;
    name_iter++;
    i++;
  }

  g_free(name_arr);
  
  if(id != NULL)
    *id = (gchar *) xmlGetProp(node, AGS_FILE_ID_PROP);

  if(parameter != NULL)
    *parameter = parameter_arr;
  else
    g_free(parameter);

  if(n_params != NULL)
    *n_params = i;

  if(xpath != NULL)
    *xpath = xpath_arr;
  else
    free(xpath_arr);
}

xmlNode*
ags_file_util_write_parameter(AgsFile *file,
			      xmlNode *parent, gchar *id,
			      GParameter *parameter, gint n_params)
{
  xmlNode *node;
  xmlChar *name, *old_name;
  gchar *child_id;
  guint i;

  if(parameter == NULL){
    return;
  }

  node = xmlNewNode(NULL,
		    "ags-parameter\0");  

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  name = NULL;

  for(i = 0; i < n_params; i++){
    if(name == NULL){
      name = g_strdup(parameter[i].name);
    }else{
      old_name = name;
      name = (xmlChar *) g_strdup_printf("%s %s\0", name, parameter[i].name);
      free(old_name);
    }

    child_id = ags_id_generator_create_uuid();

    ags_file_util_write_value(file,
			      node, child_id,
			      &(parameter[i].value), G_TYPE_NONE, 0);
  }

  xmlNewProp(node,
	     AGS_FILE_NAME_PROP,
	     name);

  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_file_util_read_dependency(AgsFile *file,
			      xmlNode *node, gchar **id,
			      gchar **name, xmlChar **xpath)
{
  *id = xmlGetProp(node, AGS_FILE_ID_PROP);
  *name = xmlGetProp(node, AGS_FILE_NAME_PROP);
  *xpath = xmlGetProp(node, AGS_FILE_XPATH_PROP);
}

xmlNode*
ags_file_util_write_dependency(AgsFile *file,
			       xmlNode *parent, gchar *id,
			       gchar *name, xmlChar *xpath)
{
  xmlNode *node;

  node = xmlNewNode(NULL,
		    "ags-dependency\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_FILE_NAME_PROP,
	     name);

  xmlNewProp(node,
	     AGS_FILE_XPATH_PROP,
	     xpath);

  xmlAddChild(parent,
	      node);
  
  return(node);
}

void
ags_file_read_history(AgsFile *file, xmlNode *node, AgsHistory **history)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_history(AgsFile *file, xmlNode *parent, AgsHistory *history)
{
  //TODO:JK: implement me
}

void
ags_file_read_embedded_audio(AgsFile *file, xmlNode *node, AgsEmbeddedAudio **embedded_audio)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_embedded_audio(AgsFile *file, xmlNode *parent, AgsEmbeddedAudio *embedded_audio)
{
  //TODO:JK: implement me
}

void
ags_file_read_embedded_audio_list(AgsFile *file, xmlNode *node, GList **embedded_audio)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_embedded_audio_list(AgsFile *file, xmlNode *parent, GList *embedded_audio)
{
  //TODO:JK: implement me
}

void
ags_file_read_file_link(AgsFile *file, xmlNode *node, AgsFileLink **file_link)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_file_link(AgsFile *file, xmlNode *parent, AgsFileLink *file_link)
{
  //TODO:JK: implement me
}

void
ags_file_read_file_link_list(AgsFile *file, xmlNode *node, GList **file_link)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_file_link_list(AgsFile *file, xmlNode *parent, GList *file_link)
{
  //TODO:JK: implement me
}
