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

#include <ags/object/ags_connectable.h>

#include <ags-client/scripting/ags_script.h>
#include <ags-client/scripting/ags_xml_script_factory.h>

#include <ags-client/scripting/mapping/ags_script_var.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

void ags_script_set_class_init(AgsScriptSetClass *script_set);
void ags_script_set_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_script_set_init(AgsScriptSet *script_set);
void ags_script_set_connect(AgsConnectable *connectable);
void ags_script_set_disconnect(AgsConnectable *connectable);
void ags_script_set_finalize(GObject *gobject);

AgsScriptObject* ags_script_set_launch(AgsScriptObject *script_object, GError **error);

xmlNode* ags_script_set_matrix_find_index(AgsScriptSet *script_set,
					  AgsScriptArray *matrix,
					  guint y);

void ags_script_set_matrix_get(AgsScriptSet *script_set,
			       AgsScriptArray *matrix,
			       AgsScriptVar *lvalue,
			       guint offset,
			       guint *x, guint *y);
void ags_script_set_matrix_put(AgsScriptSet *script_set,
			       AgsScriptArray *matrix,
			       AgsScriptVar *value,
			       guint offset,
			       guint *x, guint *y);

void ags_script_set_prime(AgsScriptSet *script_set,
			  AgsScriptVar *a,
			  AgsScriptArray *prime);
void ags_script_set_ggt(AgsScriptSet *script_set,
			AgsScriptVar *a,
			AgsScriptVar *b,
			AgsScriptVar *ggt);
void ags_script_set_kgv(AgsScriptSet *script_set,
			AgsScriptVar *a,
			AgsScriptVar *b,
			AgsScriptVar *kgv);
void ags_script_set_value(AgsScriptSet *script_set,
			  AgsScriptVar *value,
			  AgsScriptArray *lvalue);

void ags_script_set_default_index(AgsScriptSet *script_set,
				  AgsScriptArray *index);
void ags_script_set_matrix_move_index(AgsScriptSet *script_set,
				      AgsScriptArray *matrix,
				      AgsScriptArray *index,
				      guint row,
				      guint dest_x,
				      guint src_x);

void ags_script_set_matrix_sort(AgsScriptSet *script_set,
				AgsScriptArray *matrix,
				gboolean sort_column, gboolean sort_row,
				AgsScriptArray *index, guint depth,
				guint *x, guint *y);
void ags_script_set_matrix_sort_by_operands(AgsScriptSet *script_set,
					    AgsScriptArray *matrix,
					    AgsScriptArray *operands,
					    AgsScriptArray *index, guint depth,
					    guint *x, guint *y);

void ags_script_set_matrix_vector_mirror(AgsScriptSet *script_set,
					 AgsScriptArray *matrix,
					 AgsScriptArray *vector);

void ags_script_set_equation(AgsScriptSet *script_set,
			     AgsScriptArray *factorized,
			     AgsScriptArray *operands);

#define AGS_SCRIPT_SET_BASE64_UNIT (4)
#define AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT (3)

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
ags_script_set_launch(AgsScriptObject *script_object, GError **error)
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
				AgsScriptArray *script_array)
{
  gchar *string;

  auto gchar* ags_script_set_fill_column(AgsScriptArray *script_array, xmlNode *column);
  auto gchar* ags_script_set_fill_row(AgsScriptArray *script_array, xmlNode *row);

  gchar* ags_script_set_fill_column(AgsScriptArray *script_array, xmlNode *column){
    xmlChar *content, *tmp;
    guchar *value;
    union{
      gint16 *data_int16;
      guint16 *data_uint16;
      gint32 *data_int32;
      guint32 *data_uint32;
      gint64 *data_int64;
      guint64 *data_uint64;
      gdouble *data_double;
      gchar *data_char;
    }ptr;
    gchar *string;
    gsize retlength;
    guint i, i_stop;

    content = xmlNodeGetContent(column);

    value = g_base64_decode(content, &retlength);
    string = NULL;

    switch(script_array->mode){
    case AGS_SCRIPT_ARRAY_INT16:
      {
	ptr.data_int16 = (gint16 *) value;

	i_stop = retlength * AGS_SCRIPT_SET_BASE64_UNIT / sizeof(gint16);
      }
      break;
    case AGS_SCRIPT_ARRAY_UINT16:
      {
	ptr.data_uint16 = (gint16 *) value;

	i_stop = retlength * AGS_SCRIPT_SET_BASE64_UNIT / sizeof(guint16);
      }
      break;
    case AGS_SCRIPT_ARRAY_INT32:
      {
	ptr.data_int32 = (gint32 *) value;

	i_stop = retlength * AGS_SCRIPT_SET_BASE64_UNIT / sizeof(gint32);
      }
      break;
    case AGS_SCRIPT_ARRAY_UINT32:
      {
	ptr.data_uint32 = (guint32 *) value;

	i_stop = retlength * AGS_SCRIPT_SET_BASE64_UNIT / sizeof(guint32);
      }
      break;
    case AGS_SCRIPT_ARRAY_INT64:
      {
	ptr.data_int64 = (gint64 *) value;

	i_stop = retlength * AGS_SCRIPT_SET_BASE64_UNIT / sizeof(gint64);
      }
      break;
    case AGS_SCRIPT_ARRAY_UINT64:
      {
	ptr.data_uint64 = (guint64 *) value;

	i_stop = retlength * AGS_SCRIPT_SET_BASE64_UNIT / sizeof(guint64);
      }
      break;
    case AGS_SCRIPT_ARRAY_DOUBLE:
      {
	ptr.data_double = (gdouble *) value;

	i_stop = retlength * AGS_SCRIPT_SET_BASE64_UNIT / sizeof(gdouble);
      }
      break;
    case AGS_SCRIPT_ARRAY_CHAR:
      {
	ptr.data_char = (gchar *) value;

	g_message("unsupported data type: gchar\0");
      }
      break;
    case AGS_SCRIPT_ARRAY_POINTER:
      {
	g_message("unsupported data type: gpointer\0");
      }
      break;
    }

    for(i = 0; i < i_stop; i++){
      switch(script_array->mode){
      case AGS_SCRIPT_ARRAY_INT16:
	{
	  gint16 k;

	  memcpy(&k, &(ptr.data_int16[i * sizeof(gint16)]), sizeof(gint16));

	  tmp = string;

	  string = g_strdup_printf("%s %d:\0", string, k);
	  g_free(tmp);
	}
	break;
      case AGS_SCRIPT_ARRAY_UINT16:
	{
	  guint16 k;

	  memcpy(&k, &(ptr.data_uint16[i * sizeof(guint16)]), sizeof(guint16));

	  tmp = string;

	  string = g_strdup_printf("%s %d:\0", string, k);
	  g_free(tmp);
	}
	break;
      case AGS_SCRIPT_ARRAY_INT32:
	{
	  gint32 k;

	  memcpy(&k, &(ptr.data_int32[i * sizeof(gint32)]), sizeof(gint32));

	  tmp = string;

	  string = g_strdup_printf("%s %d:\0", string, k);
	  g_free(tmp);
	}
	break;
      case AGS_SCRIPT_ARRAY_UINT32:
	{
	  guint32 k;

	  memcpy(&k, &(ptr.data_uint32[i * sizeof(guint32)]), sizeof(guint32));

	  tmp = string;

	  string = g_strdup_printf("%s %d:\0", string, k);
	  g_free(tmp);
	}
	break;
      case AGS_SCRIPT_ARRAY_INT64:
	{
	  gint64 k;

	  memcpy(&k, &(ptr.data_int64[i * sizeof(gint64)]), sizeof(gint64));

	  tmp = string;

	  string = g_strdup_printf("%s %d:\0", string, k);
	  g_free(tmp);
	}
	break;
      case AGS_SCRIPT_ARRAY_UINT64:
	{
	  guint64 k;

	  memcpy(&k, &(ptr.data_uint64[i * sizeof(guint64)]), sizeof(guint64));

	  tmp = string;

	  string = g_strdup_printf("%s %d:\0", string, k);
	  g_free(tmp);
	}
	break;
      case AGS_SCRIPT_ARRAY_DOUBLE:
	{
	  gdouble k;

	  memcpy(&k, &(ptr.data_double[i * sizeof(gdouble)]), sizeof(gdouble));

	  tmp = string;

	  string = g_strdup_printf("%s %f:\0", string, k);
	  g_free(tmp);
	}
	break;
      case AGS_SCRIPT_ARRAY_CHAR:
	{
	  g_message("unsupported data type: gchar\0");
	}
	break;
      case AGS_SCRIPT_ARRAY_POINTER:
	{
	  g_message("unsupported data type: gpointer\0");
	}
	break;
      }
    }

    return(string);
  }
  gchar* ags_script_set_fill_row(AgsScriptArray *script_array, xmlNode *row){
    xmlNode *current;
    gchar *matrix, *column, *tmp;

    current = row;
    matrix = g_strdup("{\0");
    
    while(current != NULL){
      if(current->type == XML_ELEMENT_NODE){
	column = ags_script_set_fill_column(script_array, current);
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

  string = ags_script_set_fill_row(script_array, AGS_SCRIPT_OBJECT(script_array)->node);

  return(string);
}

AgsScriptArray*
ags_script_set_matrix_from_string(AgsScriptSet *script_set,
				  gchar *string)
{
  AgsXmlScriptFactory *xml_script_factory;
  AgsScriptArray *script_array;
  xmlNode *matrix;
  GError *error;

  auto void ags_script_set_matrix_from_string_read_col(AgsScriptArray *script_array,
						       xmlNode *node,
						       gchar *string);
  auto void ags_script_set_matrix_from_string_read_row(AgsScriptArray *script_array,
						       xmlNode *node,
						       gchar *string);
  auto void ags_script_set_matrix_from_string_validate(AgsScriptArray *script_array,
						       xmlNode *node);

  void ags_script_set_matrix_from_string_read_col(AgsScriptArray *script_array,
						  xmlNode *node,
						  gchar *string){
    guchar *col, *data;
    gchar *current, *prev;
    guint n_cols;

    col = NULL;

    current =
      prev = string;
    n_cols = 0;

    while((current = strchr(current, ':')) != NULL){

      switch(script_array->mode){
      case AGS_SCRIPT_ARRAY_INT16:
	{
	  gint16 k;

	  sscanf(prev, "%d:\0", &k);

	  if(col == NULL){
	    col = (guchar *) (gint16 *) malloc(sizeof(gint16));
	  }else{
	    col = (guchar *) (gint16 *) realloc(col,
						(n_cols + 1) * sizeof(gint16));
	  }

	  memcpy(col, &k, sizeof(gint16));
	}
	break;
      case AGS_SCRIPT_ARRAY_UINT16:
	{
	  guint16 k;

	  sscanf(prev, "%d:\0", &k);

	  if(col == NULL){
	    col = (guchar *) (guint16 *) malloc(sizeof(guint16));
	  }else{
	    col = (guchar *) (guint16 *) realloc(col,
						 (n_cols + 1) * sizeof(guint16));
	  }

	  memcpy(col, &k, sizeof(guint16));
	}
	break;
      case AGS_SCRIPT_ARRAY_INT32:
	{
	  gint32 k;

	  sscanf(prev, "%d:\0", &k);

	  if(col == NULL){
	    col = (guchar *) (gint32 *) malloc(sizeof(gint32));
	  }else{
	    col = (guchar *) (gint32 *) realloc(col,
						(n_cols + 1) * sizeof(gint32));
	  }

	  memcpy(col, &k, sizeof(gint32));
	}
	break;
      case AGS_SCRIPT_ARRAY_UINT32:
	{
	  guint32 k;

	  sscanf(prev, "%d:\0", &k);

	  if(col == NULL){
	    col = (guchar *) (guint32 *) malloc(sizeof(guint32));
	  }else{
	    col = (guchar *) (guint32 *) realloc(col,
						 (n_cols + 1) * sizeof(guint32));
	  }

	  memcpy(col, &k, sizeof(guint32));
	}
	break;
      case AGS_SCRIPT_ARRAY_INT64:
	{
	  gint64 k;

	  sscanf(prev, "%d:\0", &k);

	  if(col == NULL){
	    col = (guchar *) (gint64 *) malloc(sizeof(gint64));
	  }else{
	    col = (guchar *) (gint64 *) realloc(col,
						(n_cols + 1) * sizeof(gint64));
	  }

	  memcpy(col, &k, sizeof(gint64));
	}
	break;
      case AGS_SCRIPT_ARRAY_UINT64:
	{
	  guint64 k;

	  sscanf(prev, "%d:\0", &k);

	  if(col == NULL){
	    col = (guchar *) (guint64 *) malloc(sizeof(guint64));
	  }else{
	    col = (guchar *) (guint64 *) realloc(col,
						 (n_cols + 1) * sizeof(guint64));
	  }

	  memcpy(col, &k, sizeof(guint64));
	}
	break;
      case AGS_SCRIPT_ARRAY_DOUBLE:
	{
	  gdouble k;

	  sscanf(prev, "%f:\0", &k);

	  if(col == NULL){
	    col = (guchar *) (gdouble *) malloc(sizeof(gdouble));
	  }else{
	    col = (guchar *) (gdouble *) realloc(col,
						 (n_cols + 1) * sizeof(gdouble));
	  }

	  memcpy(col, &k, sizeof(gdouble));
	}
	break;
      case AGS_SCRIPT_ARRAY_CHAR:
	{
	  g_message("unsupported data type: gchar\0");
	}
	break;
      case AGS_SCRIPT_ARRAY_POINTER:
	{
	  g_message("unsupported data type: gpointer\0");
	}
	break;
      }

      current++;
      prev = current;
      n_cols++;
    }

    xmlNodeSetContent(node, (xmlChar *) g_base64_encode(col, n_cols));
    xmlSetProp(node, "length\0", (xmlChar *) g_strdup_printf("%d\0", n_cols));
  }
  void ags_script_set_matrix_from_string_read_row(AgsScriptArray *script_array,
						  xmlNode *node,
						  gchar *string){
    xmlNode *row;
    gchar *current, *prev;
    guint rows;

    current =
      prev = string;
    rows = 0;

    while((current = strchr(current, ';')) != NULL){
      row = ags_xml_script_factory_map(xml_script_factory,
				       "ags-array\0",
				       &error);
      ags_script_set_matrix_from_string_read_col(script_array,
						 row,
						 prev);
      xmlAddChild(node, row);

      current++;
      prev = current;
      rows++;
    }

    xmlSetProp(node, "length\0", (xmlChar *) g_strdup_printf("%d\0", rows));
  }
  void ags_script_set_matrix_from_string_validate(AgsScriptArray *script_array,
						  xmlNode *node){
    xmlNode *current;
    guchar *col;
    guint *cols;
    guint n_cols, n_rows;
    guint current_length;
    guint i, j, j_start;
    gsize retlength;

    n_rows = strtoul(xmlGetProp(node, "length\0"), NULL, 10);
    cols = (guint *) malloc(n_rows * sizeof(guint));

    current = node->children;
    n_cols = 0;

    /* read cols */
    for(i = 0; i < n_rows; i++){
      cols[i] = strtoul(xmlGetProp(current, "length\0"), NULL, 10);

      /* find longest row */
      if(n_cols < cols[i]){
	n_cols = cols[i];
      }

      current = current->next;
    }

    /* fill invalid rows */
    current = node->children;

    for(i = 0; i < n_rows; i++){
      col = g_base64_decode((guchar *) xmlNodeGetContent(node),
			    &retlength);
      j_start = strtoul(xmlGetProp(node, "length\0"), NULL, 10);

      switch(script_array->mode){
      case AGS_SCRIPT_ARRAY_INT16:
	{
	  gint16 k;

	  k = 0;
	    
	  if(col == NULL){
	    col = (guchar *) (gint16 *) malloc(n_cols * sizeof(gint16));
	  }else{
	    col = (guchar *) (gint16 *) realloc(col,
						n_cols * sizeof(gint16));
	  }

	  for(j = j_start; j < n_cols; j++){
	    memcpy(&(col[j * sizeof(gint16)]), &k, sizeof(gint16));
	  }
	}
	break;
      case AGS_SCRIPT_ARRAY_UINT16:
	{
	  guint16 k;

	  k = 0;
	    
	  if(col == NULL){
	    col = (guchar *) (guint16 *) malloc(n_cols * sizeof(guint16));
	  }else{
	    col = (guchar *) (guint16 *) realloc(col,
						 n_cols * sizeof(guint16));
	  }

	  for(j = j_start; j < n_cols; j++){
	    memcpy(&(col[j * sizeof(guint16)]), &k, sizeof(guint16));
	  }
	}
	break;
      case AGS_SCRIPT_ARRAY_INT32:
	{
	  gint32 k;

	  k = 0;
	    
	  if(col == NULL){
	    col = (guchar *) (gint32 *) malloc(n_cols * sizeof(gint32));
	  }else{
	    col = (guchar *) (gint32 *) realloc(col,
						n_cols * sizeof(gint32));
	  }

	  for(j = j_start; j < n_cols; j++){
	    memcpy(&(col[j * sizeof(gint32)]), &k, sizeof(gint32));
	  }
	}
	break;
      case AGS_SCRIPT_ARRAY_UINT32:
	{
	  guint32 k;

	  k = 0;
	    
	  if(col == NULL){
	    col = (guchar *) (guint32 *) malloc(n_cols * sizeof(guint32));
	  }else{
	    col = (guchar *) (guint32 *) realloc(col,
						 n_cols * sizeof(guint32));
	  }

	  for(j = j_start; j < n_cols; j++){
	    memcpy(&(col[j * sizeof(guint32)]), &k, sizeof(guint32));
	  }
	}
	break;
      case AGS_SCRIPT_ARRAY_INT64:
	{
	  gint64 k;

	  k = 0;
	    
	  if(col == NULL){
	    col = (guchar *) (gint64 *) malloc(n_cols * sizeof(gint64));
	  }else{
	    col = (guchar *) (gint64 *) realloc(col,
						n_cols * sizeof(gint64));
	  }

	  for(j = j_start; j < n_cols; j++){
	    memcpy(&(col[j * sizeof(gint64)]), &k, sizeof(gint64));
	  }
	}
	break;
      case AGS_SCRIPT_ARRAY_UINT64:
	{
	  guint64 k;

	  k = 0;
	    
	  if(col == NULL){
	    col = (guchar *) (guint64 *) malloc(n_cols * sizeof(guint64));
	  }else{
	    col = (guchar *) (guint64 *) realloc(col,
						 n_cols * sizeof(guint64));
	  }

	  for(j = j_start; j < n_cols; j++){
	    memcpy(&(col[j * sizeof(guint64)]), &k, sizeof(guint64));
	  }
	}
	break;
      case AGS_SCRIPT_ARRAY_DOUBLE:
	{
	  gdouble k;

	  k = 0.0;
	    
	  if(col == NULL){
	    col = (guchar *) (gdouble *) malloc(n_cols * sizeof(gdouble));
	  }else{
	    col = (guchar *) (gdouble *) realloc(col,
						 n_cols * sizeof(gdouble));
	  }

	  for(j = j_start; j < n_cols; j++){
	    memcpy(&(col[j * sizeof(gdouble)]), &k, sizeof(gdouble));
	  }
	}
	break;
      case AGS_SCRIPT_ARRAY_CHAR:
	{
	  g_message("unsupported data type: gchar\0");
	}
	break;
      case AGS_SCRIPT_ARRAY_POINTER:
	{
	  g_message("unsupported data type: gpointer\0");
	}
	break;
      }

      col = g_base64_encode(col,
			    n_cols);

      current = current->next;
    }
  }

  xml_script_factory = AGS_SCRIPT(AGS_SCRIPT_OBJECT(script_set)->script)->xml_script_factory;

  script_array = ags_script_array_new();
  script_array->flags &= (~AGS_SCRIPT_ARRAY_UTF8_ENCODED);
  script_array->flags |= AGS_SCRIPT_ARRAY_BASE64_ENCODED;
  script_array->mode = AGS_SCRIPT_ARRAY_DOUBLE;

  error = NULL;

  matrix = AGS_SCRIPT_OBJECT(script_array)->node;

  if(error != NULL){
    g_message("failed to instantiate xmlNode: ags-array\0");

    g_object_unref(script_array);

    return(NULL);
  }

  //TODO:JK: implement data type - see above
  ags_script_set_matrix_from_string_read_row(script_array,
					     matrix,
					     string);
  ags_script_set_matrix_from_string_validate(script_array,
					     matrix);

  return(script_array);
}

xmlNode*
ags_script_set_matrix_find_index(AgsScriptSet *script_set,
				 AgsScriptArray *matrix,
				 guint y)
{
  xmlNode *current;

  current = AGS_SCRIPT_OBJECT(matrix)->node->children;

  while(current != NULL){
    if(strtoul(xmlGetProp(current, "index\0"), NULL, 10) == y){
      return(current);
    }

    current = current->next;
  }

  return(NULL);
}

void
ags_script_set_matrix_get(AgsScriptSet *script_set,
			  AgsScriptArray *matrix,
			  AgsScriptVar *lvalue,
			  guint offset,
			  guint *x, guint *y)
{
  xmlNode *current, *row;
  guchar *data, *buffer;
  guint n_cols;
  guint state, save;
  guint putlength, x_frame;
  guint ret_x, ret_y;
  gsize retlength;
  
  row = AGS_SCRIPT_OBJECT(matrix)->node->children;

  if(xmlNodeGetContent(AGS_SCRIPT_OBJECT(lvalue)->node) != NULL){
    xmlNodeSetContent(AGS_SCRIPT_OBJECT(lvalue)->node,
		      NULL);
  }

  if(row != NULL){
    n_cols = strtoul(xmlGetProp(row, "length\0"), NULL, 10);
    
    ret_x = (guint) floor(offset / n_cols);
    ret_y = offset % n_cols;

    current = ags_script_set_matrix_find_index(script_set,
					       matrix,
					       ret_y);

    data = xmlNodeGetContent(current);
    
    switch(matrix->mode){
    case AGS_SCRIPT_ARRAY_INT16:
      {
	gint16 *k;

	putlength = (guint) round((gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
				  (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
				  sizeof(gint16)) *
	  AGS_SCRIPT_SET_BASE64_UNIT +
	  AGS_SCRIPT_SET_BASE64_UNIT;

	buffer = (guchar *) malloc(putlength);

	g_base64_decode_step(data,
			     putlength,
			     buffer,
			     &state, &save);

	x_frame = x[0] * AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT %
	  AGS_SCRIPT_SET_BASE64_UNIT;
	
	k = (gint16 *) &(buffer[x_frame]);
	xmlNodeSetContent(AGS_SCRIPT_OBJECT(lvalue)->node, g_base64_encode((guchar *) k,
									   sizeof(gint16)));
      }
      break;
    case AGS_SCRIPT_ARRAY_UINT16:
      {
	guint16 *k;

	putlength = (guint) round((gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
				  (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
				  sizeof(guint16)) *
	  AGS_SCRIPT_SET_BASE64_UNIT +
	  AGS_SCRIPT_SET_BASE64_UNIT;

	buffer = (guchar *) malloc(putlength);

	g_base64_decode_step(data,
			     putlength,
			     buffer,
			     &state, &save);

	x_frame = x[0] * AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT %
	  AGS_SCRIPT_SET_BASE64_UNIT;
	
	k = (guint16 *) &(buffer[x_frame]);
	xmlNodeSetContent(AGS_SCRIPT_OBJECT(lvalue)->node, g_base64_encode((guchar *) k,
									   sizeof(guint16)));
      }
      break;
    case AGS_SCRIPT_ARRAY_INT32:
      {
	gint32 *k;

	putlength = (guint) round((gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
				  (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
				  sizeof(gint32)) *
	  AGS_SCRIPT_SET_BASE64_UNIT +
	  AGS_SCRIPT_SET_BASE64_UNIT;

	buffer = (guchar *) malloc(putlength);

	g_base64_decode_step(data,
			     putlength,
			     buffer,
			     &state, &save);

	x_frame = x[0] * AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT %
	  AGS_SCRIPT_SET_BASE64_UNIT;
	
	k = (gint32 *) &(buffer[x_frame]);
	xmlNodeSetContent(AGS_SCRIPT_OBJECT(lvalue)->node, g_base64_encode((guchar *) k,
									   sizeof(gint32)));
      }
      break;
    case AGS_SCRIPT_ARRAY_UINT32:
      {
	guint32 *k;

	putlength = (guint) round((gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
				  (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
				  sizeof(guint32)) *
	  AGS_SCRIPT_SET_BASE64_UNIT +
	  AGS_SCRIPT_SET_BASE64_UNIT;

	buffer = (guchar *) malloc(putlength);

	g_base64_decode_step(data,
			     putlength,
			     buffer,
			     &state, &save);

	x_frame = x[0] * AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT %
	  AGS_SCRIPT_SET_BASE64_UNIT;
	
	k = (guint32 *) &(buffer[x_frame]);
	xmlNodeSetContent(AGS_SCRIPT_OBJECT(lvalue)->node, g_base64_encode((guchar *) k,
									   sizeof(guint32)));
      }
      break;
    case AGS_SCRIPT_ARRAY_INT64:
      {
	gint64 *k;

	putlength = (guint) round((gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
				  (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
				  sizeof(gint64)) *
	  AGS_SCRIPT_SET_BASE64_UNIT +
	  AGS_SCRIPT_SET_BASE64_UNIT;

	buffer = (guchar *) malloc(putlength);

	g_base64_decode_step(data,
			     putlength,
			     buffer,
			     &state, &save);

	x_frame = x[0] * AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT %
	  AGS_SCRIPT_SET_BASE64_UNIT;
	
	k = (gint64 *) &(buffer[x_frame]);
	xmlNodeSetContent(AGS_SCRIPT_OBJECT(lvalue)->node, g_base64_encode((guchar *) k,
									   sizeof(gint64)));
      }
      break;
    case AGS_SCRIPT_ARRAY_UINT64:
      {
	guint64 *k;

	putlength = (guint) round((gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
				  (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
				  sizeof(guint64)) *
	  AGS_SCRIPT_SET_BASE64_UNIT +
	  AGS_SCRIPT_SET_BASE64_UNIT;

	buffer = (guchar *) malloc(putlength);

	g_base64_decode_step(data,
			     putlength,
			     buffer,
			     &state, &save);

	x_frame = x[0] * AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT %
	  AGS_SCRIPT_SET_BASE64_UNIT;
	
	k = (guint64 *) &(buffer[x_frame]);
	xmlNodeSetContent(AGS_SCRIPT_OBJECT(lvalue)->node, g_base64_encode((guchar *) k,
									   sizeof(guint64)));
      }
      break;
    case AGS_SCRIPT_ARRAY_DOUBLE:
      {
	gdouble *k;

	putlength = (guint) round((gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
				  (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
				  sizeof(gdouble)) *
	  AGS_SCRIPT_SET_BASE64_UNIT +
	  AGS_SCRIPT_SET_BASE64_UNIT;

	buffer = (guchar *) malloc(putlength);

	g_base64_decode_step(data,
			     putlength,
			     buffer,
			     &state, &save);

	x_frame = x[0] * AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT %
	  AGS_SCRIPT_SET_BASE64_UNIT;
	
	k = (gdouble *) &(buffer[x_frame]);
	xmlNodeSetContent(AGS_SCRIPT_OBJECT(lvalue)->node, g_base64_encode((guchar *) k,
									   sizeof(gdouble)));
      }
      break;
    case AGS_SCRIPT_ARRAY_CHAR:
      {
	char *k;

	putlength = (guint) round((gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
				  (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
				  sizeof(char)) *
	  AGS_SCRIPT_SET_BASE64_UNIT +
	  AGS_SCRIPT_SET_BASE64_UNIT;

	buffer = (guchar *) malloc(putlength);

	g_base64_decode_step(data,
			     putlength,
			     buffer,
			     &state, &save);

	x_frame = x[0] * AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT %
	  AGS_SCRIPT_SET_BASE64_UNIT;
	
	k = (char *) &(buffer[x_frame]);
	xmlNodeSetContent(AGS_SCRIPT_OBJECT(lvalue)->node, g_base64_encode((guchar *) k,
									   sizeof(char)));
      }
      break;
    case AGS_SCRIPT_ARRAY_POINTER:
      {
	g_message("unsupported data type: gpointer\0");
      }
      break;
    }

    free(data);
  }else{
    ret_x = G_MAXUINT;
    ret_y = G_MAXUINT;
  }
  
  *x = ret_x;
  *y = ret_y;
}

void
ags_script_set_matrix_put(AgsScriptSet *script_set,
			  AgsScriptArray *matrix,
			  AgsScriptVar *value,
			  guint offset,
			  guint *x, guint *y)
{
  xmlNode *current, *row;
  xmlChar *data;
  guchar *tmp_col;
  guint buffer_offset;
  guint n_cols;
  guint ret_x, ret_y;
  gsize putlength;
  GError *error;
  
  row = AGS_SCRIPT_OBJECT(matrix)->node->children;

  if(row != NULL){
    n_cols = strtoul(xmlGetProp(row, "length\0"), NULL, 10);
    
    ret_x = (guint) floor(offset / n_cols);
    ret_y = offset % n_cols;

    current = ags_script_set_matrix_find_index(script_set,
					       matrix,
					       ret_y);

    data = xmlNodeGetContent(current);

    switch(matrix->mode){
    case AGS_SCRIPT_ARRAY_INT16:
      {
	guchar *value_data;
	gint16 *k_ptr;
	gint16 k;
	gint state;
	guint save;
	gsize retlength;

	putlength = (guint) round((gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
				  (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
				  sizeof(gint16)) *
	  AGS_SCRIPT_SET_BASE64_UNIT +
	  AGS_SCRIPT_SET_BASE64_UNIT;

	buffer_offset = (guint) floor((gdouble) x[0] *
			       (gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
			       (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
			       (gdouble) sizeof(gint16));
	
	/*  */
	k_ptr = (gint16 *) malloc(putlength);

	state = 0;
	save = 0;

	g_base64_decode_step(&(data[buffer_offset]),
			     putlength,
			     (guchar *) k_ptr,
			     &state,
			     &save);

	k_ptr[x[0] * AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT %
	      AGS_SCRIPT_SET_BASE64_UNIT] = k;

	tmp_col = g_base64_encode((guchar *) k_ptr,
				  putlength);
	free(k_ptr);
      }
      break;
    case AGS_SCRIPT_ARRAY_UINT16:
      {
	guchar *value_data;
	guint16 *k_ptr;
	guint16 k;
	gint state;
	guint save;
	gsize retlength;

	putlength = (guint) round((gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
				  (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
				  sizeof(guint16)) *
	  AGS_SCRIPT_SET_BASE64_UNIT +
	  AGS_SCRIPT_SET_BASE64_UNIT;

	buffer_offset = (guint) floor((gdouble) x[0] *
			       (gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
			       (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
			       (gdouble) sizeof(guint16));
	
	/*  */
	k_ptr = (guint16 *) malloc(putlength);

	state = 0;
	save = 0;

	g_base64_decode_step(&(data[buffer_offset]),
			     putlength,
			     (guchar *) k_ptr,
			     &state,
			     &save);

	k_ptr[x[0] * AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT %
	      AGS_SCRIPT_SET_BASE64_UNIT] = k;

	tmp_col = g_base64_encode((guchar *) k_ptr,
				  putlength);
	free(k_ptr);
      }
      break;
    case AGS_SCRIPT_ARRAY_INT32:
      {
	guchar *value_data;
	gint32 *k_ptr;
	gint32 k;
	gint state;
	guint save;
	gsize retlength;

	putlength = (guint) round((gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
				  (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
				  sizeof(gint32)) *
	  AGS_SCRIPT_SET_BASE64_UNIT +
	  AGS_SCRIPT_SET_BASE64_UNIT;

	buffer_offset = (guint) floor((gdouble) x[0] *
			       (gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
			       (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
			       (gdouble) sizeof(gint32));
	
	/*  */
	k_ptr = (gint32 *) malloc(putlength);

	state = 0;
	save = 0;

	g_base64_decode_step(&(data[buffer_offset]),
			     putlength,
			     (guchar *) k_ptr,
			     &state,
			     &save);

	k_ptr[x[0] * AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT %
	      AGS_SCRIPT_SET_BASE64_UNIT] = k;

	tmp_col = g_base64_encode((guchar *) k_ptr,
				  putlength);
	free(k_ptr);
      }
      break;
    case AGS_SCRIPT_ARRAY_UINT32:
      {
	guchar *value_data;
	guint32 *k_ptr;
	guint32 k;
	gint state;
	guint save;
	gsize retlength;

	putlength = (guint) round((gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
				  (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
				  sizeof(guint32)) *
	  AGS_SCRIPT_SET_BASE64_UNIT +
	  AGS_SCRIPT_SET_BASE64_UNIT;

	buffer_offset = (guint) floor((gdouble) x[0] *
			       (gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
			       (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
			       (gdouble) sizeof(guint32));
	
	/*  */
	k_ptr = (guint32 *) malloc(putlength);

	state = 0;
	save = 0;

	g_base64_decode_step(&(data[buffer_offset]),
			     putlength,
			     (guchar *) k_ptr,
			     &state,
			     &save);

	k_ptr[x[0] * AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT %
	      AGS_SCRIPT_SET_BASE64_UNIT] = k;

	tmp_col = g_base64_encode((guchar *) k_ptr,
				  putlength);
	free(k_ptr);
      }
      break;
    case AGS_SCRIPT_ARRAY_INT64:
      {
	guchar *value_data;
	gint64 *k_ptr;
	gint64 k;
	gint state;
	guint save;
	gsize retlength;

	putlength = (guint) round((gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
				  (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
				  sizeof(gint64)) *
	  AGS_SCRIPT_SET_BASE64_UNIT +
	  AGS_SCRIPT_SET_BASE64_UNIT;

	buffer_offset = (guint) floor((gdouble) x[0] *
			       (gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
			       (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
			       (gdouble) sizeof(gint64));
	
	/*  */
	k_ptr = (gint64 *) malloc(putlength);

	state = 0;
	save = 0;

	g_base64_decode_step(&(data[buffer_offset]),
			     putlength,
			     (guchar *) k_ptr,
			     &state,
			     &save);

	k_ptr[x[0] * AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT %
	      AGS_SCRIPT_SET_BASE64_UNIT] = k;

	tmp_col = g_base64_encode((guchar *) k_ptr,
				  putlength);
	free(k_ptr);
      }
      break;
    case AGS_SCRIPT_ARRAY_UINT64:
      {
	guchar *value_data;
	guint64 *k_ptr;
	guint64 k;
	gint state;
	guint save;
	gsize retlength;

	putlength = (guint) round((gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
				  (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
				  sizeof(guint64)) *
	  AGS_SCRIPT_SET_BASE64_UNIT +
	  AGS_SCRIPT_SET_BASE64_UNIT;

	buffer_offset = (guint) floor((gdouble) x[0] *
			       (gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
			       (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
			       (gdouble) sizeof(guint64));
	
	/*  */
	k_ptr = (guint64 *) malloc(putlength);

	state = 0;
	save = 0;

	g_base64_decode_step(&(data[buffer_offset]),
			     putlength,
			     (guchar *) k_ptr,
			     &state,
			     &save);

	k_ptr[x[0] * AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT %
	      AGS_SCRIPT_SET_BASE64_UNIT] = k;

	tmp_col = g_base64_encode((guchar *) k_ptr,
				  putlength);
	free(k_ptr);
      }
      break;
    case AGS_SCRIPT_ARRAY_DOUBLE:
      {
	guchar *value_data;
	gdouble *k_ptr;
	gdouble k;
	gint state;
	guint save;
	gsize retlength;

	putlength = (guint) round((gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
				  (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
				  sizeof(gdouble)) *
	  AGS_SCRIPT_SET_BASE64_UNIT +
	  AGS_SCRIPT_SET_BASE64_UNIT;

	buffer_offset = (guint) floor((gdouble) x[0] *
			       (gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
			       (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
			       (gdouble) sizeof(gdouble));
	
	/*  */
	k_ptr = (gdouble *) malloc(putlength);

	state = 0;
	save = 0;

	g_base64_decode_step(&(data[buffer_offset]),
			     putlength,
			     (guchar *) k_ptr,
			     &state,
			     &save);

	k_ptr[x[0] * AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT %
	      AGS_SCRIPT_SET_BASE64_UNIT] = k;

	tmp_col = g_base64_encode((guchar *) k_ptr,
				  putlength);
	free(k_ptr);
      }
      break;
    case AGS_SCRIPT_ARRAY_CHAR:
      {
	guchar *value_data;
	char *k_ptr;
	char k;
	gint state;
	guint save;
	gsize retlength;

	putlength = (guint) round((gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
				  (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
				  sizeof(char)) *
	  AGS_SCRIPT_SET_BASE64_UNIT +
	  AGS_SCRIPT_SET_BASE64_UNIT;

	buffer_offset = (guint) floor((gdouble) x[0] *
			       (gdouble) AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT /
			       (gdouble) AGS_SCRIPT_SET_BASE64_UNIT *
			       (gdouble) sizeof(char));
	
	/*  */
	k_ptr = (char *) malloc(putlength);

	state = 0;
	save = 0;

	g_base64_decode_step(&(data[buffer_offset]),
			     putlength,
			     (guchar *) k_ptr,
			     &state,
			     &save);

	k_ptr[x[0] * AGS_SCRIPT_SET_BASE64_BYTES_PER_UNIT %
	      AGS_SCRIPT_SET_BASE64_UNIT] = k;

	tmp_col = g_base64_encode((guchar *) k_ptr,
				  putlength);
	free(k_ptr);
      }
      break;
    case AGS_SCRIPT_ARRAY_POINTER:
      {
	g_message("unsupported data type: gpointer\0");
      }
      break;
    }
  }else{
    ret_x = G_MAXUINT;
    ret_y = G_MAXUINT;
  }

  memcpy(&(data[buffer_offset]), tmp_col, putlength);
  g_free(tmp_col);
  
  *x = ret_x;
  *y = ret_y;
}


void
ags_script_set_prime(AgsScriptSet *script_set,
		     AgsScriptVar *a,
		     AgsScriptArray *prime)
{
  //TODO:JK: implement me
}

void
ags_script_set_ggt(AgsScriptSet *script_set,
		   AgsScriptVar *a,
		   AgsScriptVar *b,
		   AgsScriptVar *ggt)
{
  //TODO:JK: implement me
}

void
ags_script_set_kgv(AgsScriptSet *script_set,
		   AgsScriptVar *a,
		   AgsScriptVar *b,
		   AgsScriptVar *kgv)
{
  //TODO:JK: implement me
}

void
ags_script_set_value(AgsScriptSet *script_set,
		     AgsScriptVar *value,
		     AgsScriptArray *lvalue)
{
  xmlNode *current;
  guchar *data;
  guint n_cols;
  gsize retlength, putlength;

  current = AGS_SCRIPT_OBJECT(script_set)->node->children;

  while(current != NULL){
    n_cols = strtoul(xmlGetProp(current, "length\0"), NULL, 10);

    switch(value->mode){
    case AGS_SCRIPT_ARRAY_INT16:
      {
	guchar *value;
	gint16 k;
	int c, mask;
	guint i, shift;
	
	data = (guchar *) malloc(n_cols * sizeof(gint16));
	putlength = n_cols * sizeof(gint16);

	value = g_base64_decode(xmlNodeGetContent(current), &retlength);
	memcpy(&k, value, sizeof(gint16));

	mask = (int) (gint16) 0xffff;

	for(i = 0; i < sizeof(int) / sizeof(gint16) && i < n_cols; i++){
	  shift = i * sizeof(gint16);
	  mask = mask << shift;
	  c = mask & (k << shift);
	}

	memset(data, c, (size_t) ceil((n_cols * sizeof(gint16)) / sizeof(int)));
      }
      break;
    case AGS_SCRIPT_ARRAY_UINT16:
      {
	guchar *value;
	guint16 k;
	int c, mask;
	guint i, shift;

	data = (guchar *) malloc(n_cols * sizeof(guint16));
	putlength = n_cols * sizeof(guint16);

	value = g_base64_decode(xmlNodeGetContent(current), &retlength);
	memcpy(&k, value, sizeof(guint16));

	mask = (int) (guint16) 0xffff;

	for(i = 0; i < sizeof(int) / sizeof(guint16) && i < n_cols; i++){
	  shift = i * sizeof(guint16);
	  mask = mask << shift;
	  c = mask & (k << shift);
	}

	memset(data, c, (size_t) ceil((n_cols * sizeof(guint16)) / sizeof(int)));
      }
      break;
    case AGS_SCRIPT_ARRAY_INT32:
      {
	guchar *value;
	gint32 k;
	int c, mask;
	guint i, shift;

	data = (guchar *) malloc(n_cols * sizeof(gint32));
	putlength = n_cols * sizeof(gint32);

	value = g_base64_decode(xmlNodeGetContent(current), &retlength);
	memcpy(&k, value, sizeof(gint32));

	mask = (int) (gint32) 0xffff;

	for(i = 0; i < sizeof(int) / sizeof(gint32) && i < n_cols; i++){
	  shift = i * sizeof(gint32);
	  mask = mask << shift;
	  c = mask & (k << shift);
	}

	memset(data, c, (size_t) ceil((n_cols * sizeof(gint32)) / sizeof(int)));
      }
      break;
    case AGS_SCRIPT_ARRAY_UINT32:
      {
	guchar *value;
	guint32 k;
	int c, mask;
	guint i, shift;

	data = (guchar *) malloc(n_cols * sizeof(guint32));
	putlength = n_cols * sizeof(guint32);

	value = g_base64_decode(xmlNodeGetContent(current), &retlength);
	memcpy(&k, value, sizeof(guint32));

	mask = (int) (guint32) 0xffff;

	for(i = 0; i < sizeof(int) / sizeof(guint32) && i < n_cols; i++){
	  shift = i * sizeof(guint32);
	  mask = mask << shift;
	  c = mask & (k << shift);
	}

	memset(data, c, (size_t) ceil((n_cols * sizeof(guint32)) / sizeof(int)));
      }
      break;
    case AGS_SCRIPT_ARRAY_INT64:
      {
	guchar *value;
	char *ptr;
	gint64 k, mask;
	char c;
	guint i, j, j_stop, shift;

	ptr =
	  data = (char *) malloc(n_cols * sizeof(gint64) * sizeof(char));
	putlength = n_cols * sizeof(gint64) * sizeof(char);

	value = g_base64_decode(xmlNodeGetContent(current), &retlength);
	memcpy(&k, value, sizeof(gint64));

	j_stop = sizeof(gint64) / sizeof(char);

	mask = (gint64) 0xff;
	
	for(i = 0; i < n_cols; i++){
	  mask = (gint64) 0xff;

	  for(j = 0; j < j_stop; j++){
	    c = (mask & k) >> j; 

	    ptr[i * j_stop + j] = c;

	    if(j + 1 < j_stop){
	      mask = mask << sizeof(char);
	    }
	  }
	}
      }
      break;
    case AGS_SCRIPT_ARRAY_UINT64:
      {
	guchar *value;
	char *ptr;
	guint offset;
	guint64 k, mask;
	char c;
	guint i, j, j_stop, shift;

	ptr =
	  data = (char *) malloc(n_cols * sizeof(guint64) * sizeof(char));
	putlength = n_cols * sizeof(guint64) * sizeof(char);

	value = g_base64_decode(xmlNodeGetContent(current), &retlength);
	memcpy(&k, value, sizeof(guint64));

	j_stop = sizeof(guint64) / sizeof(char);

	mask = (guint64) 0xff;
	
	for(i = 0; i < n_cols; i++){
	  mask = (gint64) 0xff;

	  for(j = 0; j < j_stop; j++){
	    c = (mask & k) >> j; 

	    ptr[i * j_stop + j] = c;

	    if(j + 1 < j_stop){
	      mask = mask << sizeof(char);
	    }
	  }
	}
      }
      break;
    case AGS_SCRIPT_ARRAY_DOUBLE:
      {
	gdouble *ptr;
	gdouble k;
	guint i;

	data = (char *) malloc(n_cols * sizeof(gdouble) * sizeof(guchar));
	ptr = (gdouble *) data;
	putlength = n_cols * sizeof(gdouble) * sizeof(guchar);

	for(i = 0; i < n_cols; i++){
	  ptr[i] = k;
	}
      }
      break;
    case AGS_SCRIPT_ARRAY_CHAR:
      {
	guchar *value;
	char k;
	int c, mask;
	guint i, shift;
	
	data = (guchar *) malloc(n_cols * sizeof(char));
	putlength = n_cols * sizeof(char);

	value = g_base64_decode(xmlNodeGetContent(current), &retlength);
	memcpy(&k, value, sizeof(char));

	mask = (int) (char) 0xffff;

	for(i = 0; i < sizeof(int) / sizeof(char) && i < n_cols; i++){
	  shift = i * sizeof(char);
	  mask = mask << shift;
	  c = mask & (k << shift);
	}

	memset(data, c, (size_t) ceil((n_cols * sizeof(char)) / sizeof(int)));	
      }
      break;
    case AGS_SCRIPT_ARRAY_POINTER:
      {
	g_message("unsupported data type: gpointer\0");
      }
      break;
    }

    xmlNodeSetContent(current, g_base64_encode(data, putlength));
    free(data);

    current = current->next;
  }
}

void
ags_script_set_default_index(AgsScriptSet *script_set,
			     AgsScriptArray *index)
{
  AgsXmlScriptFactory *xml_script_factory;
  AgsScriptVar *index_value;
  xmlNode *current;
  guint i, j, k;
  guint n_rows, n_cols;
  guint x, y;
  GError *error;

  xml_script_factory = AGS_SCRIPT(AGS_SCRIPT_OBJECT(script_set)->script)->xml_script_factory;

  index_value = ags_script_var_new();
  error = NULL;
  AGS_SCRIPT_OBJECT(index_value)->node = ags_xml_script_factory_map(xml_script_factory,
								    "ags-var\0",
								    &error);
  
  if(error != NULL){
    g_message("can't instantiate ags-var: %s\0", error->message);

    return;
  }

  n_rows = strtoul(xmlGetProp(AGS_SCRIPT_OBJECT(index)->node, "length\0"), NULL, 10);

  for(i = 0; i < n_rows; i++){
    current = ags_script_set_matrix_find_index(script_set,
					       index,
					       i);

    n_cols = strtoul(xmlGetProp(current, "length\0"), NULL, 10);

    for(j = 0; j < n_cols; j++){
      k = i * n_cols + j;

      xmlNodeSetContent(AGS_SCRIPT_OBJECT(index_value)->node, g_base64_encode((guchar *) (guint *) &k, sizeof(guint)));

      ags_script_set_matrix_put(script_set,
				index,
				index_value,
				i * n_cols + j,
				&x, &y);
    }
  }
}

void
ags_script_set_matrix_move_index(AgsScriptSet *script_set,
				 AgsScriptArray *matrix,
				 AgsScriptArray *index,
				 guint row,
				 guint dest_x,
				 guint src_x)
{
  AgsXmlScriptFactory *xml_script_factory;
  AgsScriptVar *src, *dest, **matrix_region_old, **index_region_old;
  AgsScriptVar *src_index, *dest_index;
  xmlNode *current;
  guint src_offset, dest_offset;
  guint n_cols;
  guint *y_old;
  guint i, i_stop;
  guint x, y;
  GError *error;

  xml_script_factory = AGS_SCRIPT(AGS_SCRIPT_OBJECT(script_set)->script)->xml_script_factory;
  
  current = ags_script_set_matrix_find_index(script_set,
					     matrix,
					     row);
  n_cols = strtoul(xmlGetProp(current, "length\0"), NULL, 10);

  /* move values in x direction */
  if(dest_x > src_x){
    i_stop = dest_x - src_x;
  }else{
    i_stop = src_x - dest_x;
  }

  /* collect old fields */
  matrix_region_old = (AgsScriptVar **) malloc(i_stop * sizeof(AgsScriptVar *));
  index_region_old = (AgsScriptVar **) malloc(i_stop * sizeof(AgsScriptVar *));
  
  for(i = 0; i < i_stop; i++){
    src_offset = row * n_cols + ((src_x < dest_x) ? src_x: dest_x) + i;

    /* indices */
    index_region_old[i] = ags_script_var_new();
    error = NULL;
    AGS_SCRIPT_OBJECT(index_region_old[i])->node = ags_xml_script_factory_map(xml_script_factory,
								       "ags-var\0",
								       &error);
  
    if(error != NULL){
      g_message("can't instantiate ags-var: %s\0", error->message);

      return;
    }

    ags_script_set_matrix_get(script_set,
			      matrix,
			      index_region_old[i],
			      src_offset,
			      &x, &y);

    /* matrix */
    matrix_region_old[i] = ags_script_var_new();
    error = NULL;
    AGS_SCRIPT_OBJECT(matrix_region_old[i])->node = ags_xml_script_factory_map(xml_script_factory,
									"ags-var\0",
									&error);
  
    if(error != NULL){
      g_message("can't instantiate ags-var: %s\0", error->message);

      return;
    }

    ags_script_set_matrix_get(script_set,
			      matrix,
			      matrix_region_old[i],
			      src_offset,
			      &x, &y);
  }

  /* shift indices */
  for(i = 0; i < i_stop - 1; i++){

    if(dest_x > src_x){
      src_offset = row * n_cols + src_x + i;
      dest_offset = src_offset + 1;
      
      ags_script_set_matrix_put(script_set,
				index,
				index_region_old[i],
				dest_offset,
				&x, &y);

      ags_script_set_matrix_put(script_set,
				matrix,
				matrix_region_old[i],
				dest_offset,
				&x, &y);
    }else{
      src_offset = row * n_cols + dest_x + i;
      dest_offset = src_offset;
      src_offset++;

      ags_script_set_matrix_put(script_set,
				index,
				index_region_old[i + 1],
				dest_offset,
				&x, &y);

      ags_script_set_matrix_put(script_set,
				matrix,
				matrix_region_old[i + 1],
				dest_offset,
				&x, &y);
    }
  }

  /* move index */
  if(dest_x > src_x){
    dest_offset = row * n_cols + src_x;

    ags_script_set_matrix_put(script_set,
			      index,
			      index_region_old[dest_x - src_x],
			      dest_offset,
			      &x, &y);

    ags_script_set_matrix_put(script_set,
			      matrix,
			      matrix_region_old[dest_x - src_x],
			      dest_offset,
			      &x, &y);
  }else{
    dest_offset = row * n_cols + dest_x;

    ags_script_set_matrix_put(script_set,
			      index,
			      index_region_old[0],
			      dest_offset,
			      &x, &y);

    ags_script_set_matrix_put(script_set,
			      matrix,
			      matrix_region_old[0],
			      dest_offset,
			      &x, &y);
  }
}

void
ags_script_set_matrix_sort(AgsScriptSet *script_set,
			   AgsScriptArray *matrix,
			   gboolean sort_column, gboolean sort_row,
			   AgsScriptArray *index, guint depth,
			   guint *x, guint *y)
{
  AgsScriptVar *next_value, *current_value;
  xmlNode *current_matrix, *current_index;
  guint n_rows, n_cols;
  guint offset, stop;
  guint i, j, k;
  guint row, src_x, dest_x;
  guint ret_x, ret_y;
  gsize retlength;

  n_rows = strtoul(xmlGetProp(AGS_SCRIPT_OBJECT(matrix)->node, "length\0"), NULL, 10);

  offset = 0;

  for(i = 0; i < n_rows; i++){

    /*  */
    current_matrix = ags_script_set_matrix_find_index(script_set,
						      matrix,
						      i);
    n_cols = strtoul(xmlGetProp(current_matrix, "length\0"), NULL, 10);

    current_index = ags_script_set_matrix_find_index(script_set,
						     index,
						     i);

    for(j = 0; j < n_cols; j++, offset++){
      ags_script_set_matrix_get(script_set,
				matrix,
				current_value,
				offset,
				&ret_x, &ret_y);

      for(k = 0; k < n_cols - j - 1; k++){
	ags_script_set_matrix_get(script_set,
				  matrix,
				  next_value,
				  offset + k + 1,
				  &ret_x, &ret_y);

	if(current_matrix == NULL ||
	   current_index == NULL){
	  *x = G_MAXUINT;
	  *y = G_MAXUINT;
	  return;
	}

	if(j >= depth){
	  break;
	}

	switch(matrix->mode){
	case AGS_SCRIPT_ARRAY_INT16:
	  {
	    gint16 *next_val, *current_val;
	    
	    current_val = (gint16 *) (guchar *) g_base64_decode(xmlNodeGetContent(AGS_SCRIPT_OBJECT(current_value)->node),
								&retlength);
	    next_val = (gint16 *) (guchar *) g_base64_decode(xmlNodeGetContent(AGS_SCRIPT_OBJECT(next_value)->node),
							     &retlength);

	    if(*next_val > *current_val){
	      current_value = next_value;
	      ags_script_set_matrix_move_index(script_set,
					       matrix,
					       index,
					       i,
					       j,
					       k);
	    }
	  }
	  break;
	case AGS_SCRIPT_ARRAY_UINT16:
	  {
	    guint16 *next_val, *current_val;

	    current_val = (guint16 *) (guchar *) g_base64_decode(xmlNodeGetContent(AGS_SCRIPT_OBJECT(current_value)->node),
								 &retlength);
	    next_val = (guint16 *) (guchar *) g_base64_decode(xmlNodeGetContent(AGS_SCRIPT_OBJECT(next_value)->node),
							      &retlength);

	    if(*next_val > *current_val){
	      current_value = next_value;
	      ags_script_set_matrix_move_index(script_set,
					       matrix,
					       index,
					       i,
					       j,
					       k);
	    }
	  }
	  break;
	case AGS_SCRIPT_ARRAY_INT32:
	  {
	    gint32 *next_val, *current_val;

	    current_val = (gint32 *) (guchar *) g_base64_decode(xmlNodeGetContent(AGS_SCRIPT_OBJECT(current_value)->node),
								&retlength);
	    next_val = (gint32 *) (guchar *) g_base64_decode(xmlNodeGetContent(AGS_SCRIPT_OBJECT(next_value)->node),
							     &retlength);
	    
	    if(*next_val > *current_val){
	      current_value = next_value;
	      ags_script_set_matrix_move_index(script_set,
					       matrix,
					       index,
					       i,
					       j,
					       k);
	    }
	  }
	  break;
	case AGS_SCRIPT_ARRAY_UINT32:
	  {
	    guint32 *next_val, *current_val;

	    current_val = (guint32 *) (guchar *) g_base64_decode(xmlNodeGetContent(AGS_SCRIPT_OBJECT(current_value)->node),
								 &retlength);
	    next_val = (guint32 *) (guchar *) g_base64_decode(xmlNodeGetContent(AGS_SCRIPT_OBJECT(next_value)->node),
							      &retlength);

	    if(*next_val > *current_val){
	      current_value = next_value;
	      ags_script_set_matrix_move_index(script_set,
					       matrix,
					       index,
					       i,
					       j,
					       k);
	    }
	  }
	  break;
	case AGS_SCRIPT_ARRAY_INT64:
	  {
	    gint64 *next_val, *current_val;

	    current_val = (gint64 *) (guchar *) g_base64_decode(xmlNodeGetContent(AGS_SCRIPT_OBJECT(current_value)->node),
								&retlength);
	    next_val = (gint64 *) (guchar *) g_base64_decode(xmlNodeGetContent(AGS_SCRIPT_OBJECT(next_value)->node),
							     &retlength);

	    if(*next_val > *current_val){
	      current_value = next_value;
	      ags_script_set_matrix_move_index(script_set,
					       matrix,
					       index,
					       i,
					       j,
					       k);
	    }
	  }
	  break;
	case AGS_SCRIPT_ARRAY_UINT64:
	  {
	    guint64 *next_val, *current_val;

	    current_val = (guint64 *) (guchar *) g_base64_decode(xmlNodeGetContent(AGS_SCRIPT_OBJECT(current_value)->node),
								 &retlength);
	    next_val = (guint64 *) (guchar *) g_base64_decode(xmlNodeGetContent(AGS_SCRIPT_OBJECT(next_value)->node),
							      &retlength);
	    
	    if(*next_val > *current_val){
	      current_value = next_value;
	      ags_script_set_matrix_move_index(script_set,
					       matrix,
					       index,
					       i,
					       j,
					       k);
	    }
	  }
	  break;
	case AGS_SCRIPT_ARRAY_DOUBLE:
	  {
	  }
	    gdouble *next_val, *current_val;

	    current_val = (gdouble *) (guchar *) g_base64_decode(xmlNodeGetContent(AGS_SCRIPT_OBJECT(current_value)->node),
								 &retlength);
	    next_val = (gdouble *) (guchar *) g_base64_decode(xmlNodeGetContent(AGS_SCRIPT_OBJECT(next_value)->node),
							      &retlength);

	    if(*next_val > *current_val){
	      current_value = next_value;
	      ags_script_set_matrix_move_index(script_set,
					       matrix,
					       index,
					       i,
					       j,
					       k);
	    }
	  break;
	case AGS_SCRIPT_ARRAY_CHAR:
	  {
	    g_message("unsupported data type: gchar\0");
	  }
	  break;
	case AGS_SCRIPT_ARRAY_POINTER:
	  {
	    g_message("unsupported data type: gpointer\0");
	  }
	  break;
	}
      }
    }
  }
}

void
ags_script_set_matrix_sort_by_operands(AgsScriptSet *script_set,
				       AgsScriptArray *matrix,
				       AgsScriptArray *operands,
				       AgsScriptArray *index, guint depth,
				       guint *x, guint *y)
{

}

void
ags_script_set_matrix_vector_mirror(AgsScriptSet *script_set,
				    AgsScriptArray *matrix,
				    AgsScriptArray *vector)
{

}

void
ags_script_set_equation(AgsScriptSet *script_set,
			AgsScriptArray *factorized,
			AgsScriptArray *operands)
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
