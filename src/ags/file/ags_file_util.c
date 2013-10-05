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
			  GType *type, GValue **value, xmlXPathContext **context)
{
  xmlChar *type_str;
  xmlChar *value_str;
  GValue value = G_VALUE_INIT;

  *id = xmlGetProp(node, AGS_FILE_ID_PROP);

  type_str = xmlGetProp(node, "type\0");
  type_str = xmlGetProp(value, "type\0");

  if(!xmlStrcmp(type_str, "gchar\0")){
    *type = G_TYPE_CHAR;

    g_value_init(&value, G_TYPE_CHAR);
    g_value_set_schar(&value, xmlGetProp(node, AGS_FILE_VALUE_PROP));

    *value = &value;
  }else if(!xmlStrcmp(type_str, "gint64\0")){
    *type = G_TYPE_INT;

    g_value_init(&value, G_TYPE_INT64);
    g_value_set_int(&value, g_ascii_strtoll(xmlGetProp(node, AGS_FILE_VALUE_PROP)));

    *value = &value;
  }else if(!xmlStrcmp(type_str, "guint64\0")){
    *type = G_TYPE_UINT;

    g_value_init(&value, G_TYPE_UINT64);
    g_value_set_static_string(&value, g_ascii_strtoull(xmlGetProp(node, AGS_FILE_VALUE_PROP)));

    *value = &value;
  }else if(!xmlStrcmp(type_str, "gdouble\0")){
    *type = G_TYPE_DOUBLE;

    g_value_init(&value, G_TYPE_DOUBLE);
    g_value_set_double(&value, g_ascii_strtod(xmlGetProp(node, AGS_FILE_VALUE_PROP)));

    *value = &value;
  }else if(!xmlStrcmp(type_str, "gchar[]\0")){
    *type = G_TYPE_STRING;

    g_value_init(&value, G_TYPE_STRING);
    g_value_set_static_string(&value, xmlGetProp(node, AGS_FILE_VALUE_PROP));

    *value = &value;
  }else if(!xmlStrcmp(type_str, "gint[]\0")){
    gchar **str_arr;
    gint *arr;

    *type = G_TYPE_POINTER;

    g_value_init(&value, G_TYPE_POINTER);

    //TODO:JK: implement me
  }else if(!xmlStrcmp(type_str, "guint[]\0")){
    gchar **str_arr;
    guint *arr;

    *type = G_TYPE_POINTER;

    g_value_init(&value, G_TYPE_POINTER);

    //TODO:JK: implement me
  }else if(!xmlStrcmp(type_str, "gdouble[]\0")){
    gchar **str_arr;
    gdouble *arr;

    *type = G_TYPE_POINTER;

    g_value_init(&value, G_TYPE_POINTER);

    //TODO:JK: implement me
  }else if(!xmlStrcmp(type_str, "gpointer\0")){
    xmlXPathContext *context;
    gpointer arr;

    *type = G_TYPE_POINTER;

    g_value_init(&value, G_TYPE_POINTER);

    //TODO:JK: implement me
  }else if(!xmlStrcmp(type_str, "gobject\0")){
    xmlXPathContext *context;

    *type = G_TYPE_OBJECT;

    g_value_init(&value, G_TYPE_OBJECT);

    //TODO:JK: implement me
  }
}

void
ags_file_util_write_gvalue(xmlNode *parent, gchar *id,
			   GValue *value, xmlXPathContext *context)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_parameter(xmlNode *node, gchar **id,
			     GParameter **parameter, gint *n_params, xmlXPathContext **context)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_parameter(xmlNode *parent, gchar *id,
			      GParameter *parameter, gint n_params, xmlXPathContext *context)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_callback(xmlNode *node, gchar **id,
			    gchar **signal_name, gchar **callback_name, xmlXPathContext **context)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_callback(xmlNode *parent, gchar *id,
			     gchar *signal_name, gchar *callback_name, xmlXPathContext *context)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_handler(xmlNode *node, gchar **id,
			   gchar **name, AgsRecallHandler **handler, GList **parameter, xmlXPathContext **context)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_handler_list(xmlNode *node, gchar **id,
				GList **handler, xmlXPathContext *context)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_handler(xmlNode *parent, gchar *id,
			    gchar *name, GList **context)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_handler_list(xmlNode *parent, gchar *id,
				 GList *handler, AgsRecallHandler *handler, GList *parameter, GList *context)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_dependency(xmlNode *node, gchar **id,
			      gchar **name, GList **values, xmlXPathContext **context)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_dependency_list(xmlNode *node, gchar **id,
				   GList **dependency, GList **context)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_dependency(xmlNode *parent, gchar *id,
			       gchar *name, GList *values, GList *context)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_dependency_list(xmlNode *parent, gchar *id,
				    GList *dependency, GList *context)
{
  //TODO:JK: implement me
}
