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

#include <ags-client/scripting/mapping/ags_script_set.h>

#include <ags-lib/object/ags_connectable.h>

void ags_script_set_class_init(AgsScriptSetClass *script_set);
void ags_script_set_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_script_set_init(AgsScriptSet *script_set);
void ags_script_set_connect(AgsConnectable *connectable);
void ags_script_set_disconnect(AgsConnectable *connectable);
void ags_script_set_finalize(GObject *gobject);

AgsScriptObject* ags_script_set_launch(AgsScriptObject *script_object);

void ags_script_set_value(AgsScriptSet *script_set,
			  AgsScriptVar *value,
			  AgsScriptStack *lvalue);

void ags_script_set_matrix_sort(AgsScriptSet *script_set,
				AgsScriptArray *matrix,
				gboolean sort_column, gboolean sort_row);
void ags_script_set_matrix_sort_by_operands(AgsScriptSet *script_set,
					    AgsScriptArray *matrix,
					    AgsScriptStack *operands,
					    guint depth);

void ags_script_set_matrix_vector_mirror(AgsScriptSet *script_set,
					 AgsScriptArray *matrix,
					 AgsScriptArray *vector);
void ags_script_set_matrix_fill(AgsScriptSet *script_set,
				AgsScriptStack *stack);

void ags_script_set_equation(AgsScriptSet *script_set,
			     AgsScriptStack *factorized,
			     AgsScriptStack *operands);

static gpointer ags_script_set_parent_class = NULL;

GType
ags_script_set_get_type()
{
  static GType ags_type_script_set = 0;

  if(!ags_type_script_set){
    static const GTypeInfo ags_script_set_info = {
      sizeof (AgsScriptSetClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_script_set_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsScriptSet),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_script_set_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_script_set_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_script_set = g_type_register_static(AGS_TYPE_SCRIPT_CONTROLLER,
						 "AgsScriptSet\0",
						 &ags_script_set_info,
						 0);
    
    g_type_add_interface_static(ags_type_script_set,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_script_set);
}

void
ags_script_set_class_init(AgsScriptSetClass *script_set)
{
  AgsScriptObjectClass *script_object;
  GObjectClass *gobject;

  ags_script_set_parent_class = g_type_class_peek_parent(script_set);

  /* GObjectClass */
  gobject = (GObjectClass *) script_set;

  gobject->finalize = ags_script_set_finalize;

  /* AgsScriptObjectClass */
  script_object = (AgsScriptObjectClass *) script_set;

  script_object->launch = ags_script_set_launch;
}

void
ags_script_set_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_script_set_connect;
  connectable->disconnect = ags_script_set_disconnect;
}

void
ags_script_set_init(AgsScriptSet *script_set)
{
  script_set->flags = 0;

  script_set->start =
    script_set->current = NULL;
}

void
ags_script_set_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_script_set_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_script_set_finalize(GObject *gobject)
{
  AgsScriptSet *script_set;

  script_set = AGS_SCRIPT_SET(gobject);

  if(script_set->start != NULL){
    free(script_set->start);
  }

  G_OBJECT_CLASS(ags_script_set_parent_class)->finalize(gobject);
}

AgsScriptObject*
ags_script_set_launch(AgsScriptObject *script_object)
{
  //TODO:JK: implement me

  return(script_object);
}

void
ags_script_set_from_string(AgsScriptSet *script_set, gchar *string)
{
  //TODO:JK: implement me
}

gchar*
ags_script_set_matrix_to_string(AgsScriptSet *script_set,
				AgsScriptArray *array)
{
  gchar *string;

  auto gchar* ags_script_set_fill_column(xmlNode *column);
  auto gchar* ags_script_set_fill_row(xmlNode *row);

  gchar* ags_script_set_fill_column(xmlNode *column){
    gchar *column, *tmp;
    xmlChar *content, *current;
    guchar *value;
    gsize retlength;
    guint i, i_stop;

    content = xmlNodeGetContent(column);

    value = g_base64_decode(content, &retlength);

    current = value;
    column = NULL;

    switch(script_array->mode){
    case AGS_SCRIPT_ARRAY_INT16:
      {
	i_stop = retlength / sizeof(gint16);
      }
      break;
    case AGS_SCRIPT_ARRAY_UINT16:
      {
	i_stop = retlength / sizeof(guint16);
      }
      break;
    case AGS_SCRIPT_ARRAY_INT32:
      {
	i_stop = retlength / sizeof(gint32);
      }
      break;
    case AGS_SCRIPT_ARRAY_UINT32:
      {
	i_stop = retlength / sizeof(guint32);
      }
      break;
    case AGS_SCRIPT_ARRAY_INT64:
      {
	i_stop = retlength / sizeof(gint64);
      }
      break;
    case AGS_SCRIPT_ARRAY_UINT64:
      {
	i_stop = retlength / sizeof(guint64);
      }
      break;
    case AGS_SCRIPT_ARRAY_DOUBLE:
      {
	i_stop = retlength / sizeof(gdouble);
      }
      break;
    case AGS_SCRIPT_ARRAY_CHAR:
      {
	i_stop = retlength / sizeof(gchar);
      }
      break;
    case AGS_SCRIPT_ARRAY_POINTER:
      {
	i_stop = retlength / sizeof(gpointer);
      }
      break;
    }

    for(i = 0; i < i_stop; i++){
      switch(script_array->mode){
      case AGS_SCRIPT_ARRAY_INT16:
	{
	  gint16 k;

	  memcpy(&k, current, sizeof(gint16));
	  current += sizeof(gint16);

	  tmp = column;

	  column = g_strdup_printf("%s %d:\0", column, k);
	  g_free(tmp);
	}
	break;
      case AGS_SCRIPT_ARRAY_UINT16:
	{
	  guint16 k;

	  memcpy(&k, current, sizeof(guint16));
	  current += sizeof(guint16);

	  tmp = column;

	  column = g_strdup_printf("%s %d:\0", column, k);
	  g_free(tmp);
	}
	break;
      case AGS_SCRIPT_ARRAY_INT32:
	{
	  gint32 k;

	  memcpy(&k, current, sizeof(gint32));
	  current += sizeof(gint32);

	  tmp = column;

	  column = g_strdup_printf("%s %d:\0", column, k);
	  g_free(tmp);
	}
	break;
      case AGS_SCRIPT_ARRAY_UINT32:
	{
	  guint32 k;

	  memcpy(&k, current, sizeof(guint32));
	  current += sizeof(guint32);

	  tmp = column;

	  column = g_strdup_printf("%s %d:\0", column, k);
	  g_free(tmp);
	}
	break;
      case AGS_SCRIPT_ARRAY_INT64:
	{
	  gint64 k;

	  memcpy(&k, current, sizeof(gint64));
	  current += sizeof(gint64);

	  tmp = column;

	  column = g_strdup_printf("%s %d:\0", column, k);
	  g_free(tmp);
	}
	break;
      case AGS_SCRIPT_ARRAY_UINT64:
	{
	  guint64 k;

	  memcpy(&k, current, sizeof(guint64));
	  current += sizeof(guint64);

	  tmp = column;

	  column = g_strdup_printf("%s %d:\0", column, k);
	  g_free(tmp);
	}
	break;
      case AGS_SCRIPT_ARRAY_DOUBLE:
	{
	  gdouble k;

	  memcpy(&k, current, sizeof(gdouble));
	  current += sizeof(gdouble);

	  tmp = column;

	  column = g_strdup_printf("%s %f:\0", column, k);
	  g_free(tmp);
	}
	break;
      case AGS_SCRIPT_ARRAY_CHAR:
	{
	  g_message("unsupported data type: gchar");
	}
	break;
      case AGS_SCRIPT_ARRAY_POINTER:
	{
	  g_message("unsupported data type: gpointer");
	}
	break;
      }
    }
  }
  gchar* ags_script_set_fill_row(xmlNode *row){
    xmlNode *current;
    gchar *matrix, *column, *tmp;

    current = row;
    matrix = g_strdup("{\0");
    
    while(current != NULL){
      if(current->type == XML_ELEMENT_NODE){
	column = ags_script_set_fill_column();
	tmp = matrix;

	matrix = g_strconcat(matrix, column, ";\0");
	g_free(tmp);
      }

      current = current->next;
    }

    tmp = matrix;

    matrix = g_strconcat(matrix, "}\0");
    g_free(tmp);

    return(matrix);
  }

  string = ags_script_set_fill_row(script_object->node);

  return(string);
}

AgsScriptArray*
ags_script_set_matrix_from_string(AgsScriptSet *script_set,
				  gchar *string)
{
  AgsXmlScriptFactory *xml_script_factory;
  AgsScriptArray *array;
  xmlNode *matrix;
  GError *error;

  auto void ags_script_set_matrix_from_string_read_col(xmlNode *node,
						       gchar *string);
  auto void ags_script_set_matrix_from_string_read_row(xmlNode *node,
						       gchar *string);
  auto void ags_script_set_matrix_from_string_validate(xmlNode *node);

  void ags_script_set_matrix_from_string_read_col(xmlNode *node,
						  gchar *string){
    //TODO:JK: implement me
  }
  void ags_script_set_matrix_from_string_read_row(xmlNode *node,
						  gchar *string){
    guint n_rows;

    //TODO:JK: implement me
  }

  void ags_script_set_matrix_from_string_validate(xmlNode *node){
    guint *cols;
    guint n_cols;

    //TODO:JK: implement me
  }

  xml_script_factory = AGS_SCRIPT(AGS_SCRIPT_OBJECT(script_set)->script)->xml_script_factory;

  array = ags_script_array_new();

  error = NULL;

  AGS_SCRIPT_OBJECT(array)->node = ags_xml_script_factory;
  matrix = ags_xml_script_factory_map(xml_script_factory,
				      "ags-array\0",
				      &error);

  if(error != NULL){
    g_message("failed to instantiate xmlNode: ags-array\0");

    g_object_unref(array);

    return(NULL);
  }

  ags_script_set_matrix_from_string_read_row(matrix,
					     string);
  ags_script_set_matrix_from_string_validate(matrix);

  return(array);
}

void
ags_script_set_value(AgsScriptSet *script_set,
		     AgsScriptVar *value,
		     AgsScriptStack *lvalue)
{

}

void
ags_script_set_matrix_sort(AgsScriptSet *script_set,
			   AgsScriptArray *matrix,
			   gboolean sort_column, gboolean sort_row)
{

}

void
ags_script_set_matrix_sort_by_operands(AgsScriptSet *script_set,
				       AgsScriptArray *matrix,
				       AgsScriptStack *operands,
				       guint depth)
{

}


void
ags_script_set_matrix_vector_mirror(AgsScriptSet *script_set,
				    AgsScriptArray *matrix,
				    AgsScriptArray *vector)
{

}

void
ags_script_set_matrix_fill(AgsScriptSet *script_set,
			   AgsScriptStack *stack)
{

}

void
ags_script_set_equation(AgsScriptSet *script_set,
			AgsScriptStack *factorized,
			AgsScriptStack *operands)
{

}

gboolean
ags_script_set_boolean_term(AgsScriptSet *script_set)
{
  gboolean term;

  term = FALSE;

  //TODO:JK: implement me

  return(term);
}

gint
ags_script_set_int_term(AgsScriptSet *script_set)
{
  gint term;

  term = -1;

  //TODO:JK: implement me

  return(term);
}

guint
ags_script_set_uint_term(AgsScriptSet *script_set)
{
  guint term;

  term = 0;

  //TODO:JK: implement me

  return(term);
}

gdouble
ags_script_set_double_term(AgsScriptSet *script_set)
{
  gdouble term;

  term = 1.0;

  //TODO:JK: implement me

  return(term);
}


AgsScriptSet*
ags_script_set_new()
{
  AgsScriptSet *script_set;

  script_set = (AgsScriptSet *) g_object_new(AGS_TYPE_SCRIPT_SET,
					     NULL);

  return(script_set);
}
