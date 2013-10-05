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

#include <ags/file/ags_file_stock.h>

void
ags_file_util_read_gvalue(xmlNode *node, gchar **id,
			  GValue **value, xmlChar **xpath)
{
  xmlChar *type_str;
  xmlChar *value_str;
  GValue a = G_VALUE_INIT;

  *id = xmlGetProp(node, AGS_FILE_ID_PROP);

  type_str = xmlGetProp(node, "type\0");
  type_str = xmlGetProp(node, "type\0");

  if(!xmlStrcmp(type_str, "gchar\0")){
    g_value_init(&a, G_TYPE_CHAR);
    g_value_set_schar(&a, xmlGetProp(node, AGS_FILE_VALUE_PROP));

    *value = &a;
    *xpath = NULL;
  }else if(!xmlStrcmp(type_str, "gint64\0")){
    g_value_init(&a, G_TYPE_INT64);
    g_value_set_int(&a, g_ascii_strtoll(xmlGetProp(node, AGS_FILE_VALUE_PROP)));

    *value = &a;
    *xpath = NULL;
  }else if(!xmlStrcmp(type_str, "guint64\0")){
    g_value_init(&a, G_TYPE_UINT64);
    g_value_set_static_string(&a, g_ascii_strtoull(xmlGetProp(node, AGS_FILE_VALUE_PROP)));

    *value = &a;
    *xpath = NULL;
  }else if(!xmlStrcmp(type_str, "gdouble\0")){
    g_value_init(&a, G_TYPE_DOUBLE);
    g_value_set_double(&a, g_ascii_strtod(xmlGetProp(node, AGS_FILE_VALUE_PROP)));

    *value = &a;
    *xpath = NULL;
  }else if(!xmlStrcmp(type_str, "gchar[]\0")){
    g_value_init(&a, G_TYPE_STRING);
    g_value_set_static_string(&a, xmlGetProp(node, AGS_FILE_VALUE_PROP));

    *value = &a;
    *xpath = NULL;
  }else if(!xmlStrcmp(type_str, "gint[]\0")){
    gchar **str_arr, **str_iter;
    gint64 *arr;
    guint i;

    str_arr = g_strsplit(xmlGetProp(node, AGS_FILE_VALUE_PROP), " \0", -1);
    
    g_value_init(&a, G_TYPE_POINTER);

    arr = NULL;
    str_iter = str_arr;
    i = 0;

    while(*str_iter != NULL){
      if(arr == NULL){
	arr = (gint64 *) malloc(sizeof(gint64));
      }else{
	arr = (gint64 *) realloc((i + 1) * sizeof(gint64));
      }

      arr[i] = g_ascii_strtoll(*str_iter);

      str_iter++;
      i++;
    }
    
    *value = &a;
    *xpath = NULL;
  }else if(!xmlStrcmp(type_str, "guint[]\0")){
    gchar **str_arr, **str_iter;
    guint64 *arr;
    guint i;

    str_arr = g_strsplit(xmlGetProp(node, AGS_FILE_VALUE_PROP), " \0", -1);

    g_value_init(&a, G_TYPE_POINTER);

    arr = NULL;
    str_iter = str_arr;
    i = 0;

    while(*str_iter != NULL){
      if(arr == NULL){
	arr = (guint64 *) malloc(sizeof(guint64));
      }else{
	arr = (guint64 *) realloc((i + 1) * sizeof(guint64));
      }

      arr[i] = g_ascii_strtoll(*str_iter);

      str_iter++;
      i++;
    }
    
    *value = &a;
    *xpath = NULL;
  }else if(!xmlStrcmp(type_str, "gdouble[]\0")){
    gchar **str_arr, **str_iter;
    gdouble *arr;
    guint i;

    str_arr = g_strsplit(xmlGetProp(node, AGS_FILE_VALUE_PROP), " \0", -1);

    g_value_init(&a, G_TYPE_POINTER);

    arr = NULL;
    str_iter = str_arr;
    i = 0;

    while(*str_iter != NULL){
      if(arr == NULL){
	arr = (gdouble *) malloc(sizeof(gdouble));
      }else{
	arr = (gdouble *) realloc((i + 1) * sizeof(gdouble));
      }

      arr[i] = g_ascii_strtoll(*str_iter);

      str_iter++;
      i++;
    }
    
    *value = &a;
    *xpath = NULL;
  }else if(!xmlStrcmp(type_str, "gpointer\0")){
    xmlChar *xpath;

    g_value_init(&a, G_TYPE_POINTER);

    *value = NULL;
    *xpath = xmlGetProp(node, AGS_FILE_VALUE_PROP);
  }else if(!xmlStrcmp(type_str, "gobject\0")){
    xmlChar *xpath;

    g_value_init(&a, G_TYPE_OBJECT);

    *value = NULL;
    *xpath = xmlGetProp(node, AGS_FILE_VALUE_PROP);
  }
}

void
ags_file_util_write_gvalue(xmlNode *parent, gchar *id,
			   GValue *value, xmlChar *xpath)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_parameter(xmlNode *node, gchar **id,
			     GParameter **parameter, gint *n_params, xmlChar **xpath)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_parameter(xmlNode *parent, gchar *id,
			      GParameter *parameter, gint n_params, xmlChar *xpath)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_callback(xmlNode *node, gchar **id,
			    gchar **signal_name, gchar **callback_name, xmlChar **xpath)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_callback(xmlNode *parent, gchar *id,
			     gchar *signal_name, gchar *callback_name, xmlChar *xpath)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_handler(xmlNode *node, gchar **id,
			   gchar **name, AgsRecallHandler **handler, GList **parameter, xmlChar **xpath)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_handler_list(xmlNode *node, gchar **id,
				GList **handler, xmlChar *xpath)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_handler(xmlNode *parent, gchar *id,
			    gchar *name, GList **xpath)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_handler_list(xmlNode *parent, gchar *id,
				 GList *handler, AgsRecallHandler *handler, GList *parameter, GList *xpath)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_dependency(xmlNode *node, gchar **id,
			      gchar **name, GList **values, xmlChar **xpath)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_dependency_list(xmlNode *node, gchar **id,
				   GList **dependency, GList **xpath)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_dependency(xmlNode *parent, gchar *id,
			       gchar *name, GList *values, GList *xpath)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_dependency_list(xmlNode *parent, gchar *id,
				    GList *dependency, GList *xpath)
{
  //TODO:JK: implement me
}
