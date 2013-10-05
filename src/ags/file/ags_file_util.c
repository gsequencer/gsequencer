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

void
ags_file_util_read_gvalue(xmlNode *node, gchar **id, GType *type, GValue **value)
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
    *type = G_TYPE_POINTER;

    //TODO:JK: implement me
  }else if(!xmlStrcmp(type_str, "guint[]\0")){
    *type = G_TYPE_POINTER;

    //TODO:JK: implement me
  }else if(!xmlStrcmp(type_str, "gdouble[]\0")){
    *type = G_TYPE_POINTER;

    //TODO:JK: implement me
  }else if(!xmlStrcmp(type_str, "gpointer\0")){
    *type = G_TYPE_POINTER;

    //TODO:JK: implement me
  }else if(!xmlStrcmp(type_str, "gobject\0")){
    *type = G_TYPE_OBJECT;

    //TODO:JK: implement me
  }
}

void
ags_file_util_write_gvalue(xmlNode *parent, gchar *id, GType type, GValue *value)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_callback(xmlNode *node, gchar **id, gchar **signal_name, gchar **callback_name)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_callback(xmlNode *parent, gchar *id, gchar *signal_name, gchar *callback_name)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_handler(xmlNode *node, gchar **id, gchar **name, AgsRecallHandler **handler, GList **parameter)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_handler_list(xmlNode *node, gchar **id, GList **handler)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_handler(xmlNode *parent, gchar *id, gchar *name)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_handler_list(xmlNode *parent, gchar *id, GList *handler, AgsRecallHandler *handler, GList *parameter)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_dependency(xmlNode *node, gchar **id, gchar **name, GList **values)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_dependency_list(xmlNode *node, gchar **id, GList **dependency)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_dependency(xmlNode *parent, gchar *id, gchar *name, GList *values)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_dependency_list(xmlNode *parent, gchar *id, GList *dependency)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_parameter(xmlNode *node, gchar **id, gchar **parameter, GList **values)
{
  //TODO:JK: implement me
}

void
ags_file_util_read_parameter_list(xmlNode *node, gchar **id, GList **parameter)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_parameter(xmlNode *parent, gchar *id, gchar *parameter, GList *values)
{
  //TODO:JK: implement me
}

void
ags_file_util_write_parameter_list(xmlNode *parent, gchar *id, GList *parameter)
{
  //TODO:JK: implement me
}

