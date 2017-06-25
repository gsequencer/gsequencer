/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/lib/ags_turtle.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <regex.h>

#include <pthread.h>

#include <ags/lib/ags_regex.h>

#include <sys/stat.h>
#include <unistd.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

/**
 * SECTION:ags_turtle
 * @short_description: Terse RDF Triple Language
 * @title: AgsTurtle
 * @section_id:
 * @include: ags/lib/ags_turtle.h
 *
 * The #AgsTurtle object converts Terse RDF Triple Language files
 * into XML.
 */

void ags_turtle_class_init(AgsTurtleClass *turtle);
void ags_turtle_init (AgsTurtle *turtle);
void ags_turtle_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_turtle_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_turtle_finalize(GObject *gobject);

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_XML_DOC,
};

static gpointer ags_turtle_parent_class = NULL;

static pthread_mutex_t regex_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_turtle_get_type(void)
{
  static GType ags_type_turtle = 0;

  if(!ags_type_turtle){
    static const GTypeInfo ags_turtle_info = {
      sizeof (AgsTurtleClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_turtle_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTurtle),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_turtle_init,
    };

    ags_type_turtle = g_type_register_static(G_TYPE_OBJECT,
					     "AgsTurtle",
					     &ags_turtle_info,
					     0);
  }

  return (ags_type_turtle);
}

void
ags_turtle_class_init(AgsTurtleClass *turtle)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_turtle_parent_class = g_type_class_peek_parent(turtle);

  /* GObjectClass */
  gobject = (GObjectClass *) turtle;

  gobject->set_property = ags_turtle_set_property;
  gobject->get_property = ags_turtle_get_property;
  
  gobject->finalize = ags_turtle_finalize;

  /* properties */
  /**
   * AgsTurtle:filename:
   *
   * The assigned filename.
   * 
   * Since: 0.7.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("filename of turtle"),
				   i18n_pspec("The filename this turtle is assigned to"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsTurtle:xml-doc:
   *
   * The assigned xml-doc.
   * 
   * Since: 0.7.3
   */
  param_spec = g_param_spec_pointer("xml-doc",
				    i18n_pspec("xml document of turtle"),
				    i18n_pspec("The xml document turtle was converted to"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_XML_DOC,
				  param_spec);
}

void
ags_turtle_init(AgsTurtle *turtle)
{
  turtle->flags = AGS_TURTLE_TOLOWER;
  
  turtle->filename = NULL;

  turtle->doc = NULL;
}

void
ags_turtle_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsTurtle *turtle;

  turtle = AGS_TURTLE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = (gchar *) g_value_get_string(value);

      if(turtle->filename == filename){
	return;
      }
      
      if(turtle->filename != NULL){
	g_free(turtle->filename);
      }

      turtle->filename = g_strdup(filename);
    }
    break;
  case PROP_XML_DOC:
    {
      xmlDoc *doc;

      doc = (xmlDoc *) g_value_get_pointer(value);
      
      turtle->doc = doc;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_turtle_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsTurtle *turtle;

  turtle = AGS_TURTLE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, turtle->filename);
    }
    break;
  case PROP_XML_DOC:
    {
      g_value_set_pointer(value, turtle->doc);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_turtle_finalize(GObject *gobject)
{
  AgsTurtle *turtle;
  gchar **str;
  
  turtle = AGS_TURTLE(gobject);

  if(turtle->filename != NULL){
    g_free(turtle->filename);
  }

  if(turtle->doc != NULL){
    xmlFreeDoc(turtle->doc);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_turtle_parent_class)->finalize(gobject);
}

/**
 * ags_turtle_read_iriref:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read iriref value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_iriref(gchar *offset,
		       gchar *end_ptr)
{
  regmatch_t match_arr[1];
    
  gchar *str;

  static regex_t iriref_regex;
    
  static gboolean regex_compiled = FALSE;

  //FIXME:JK: do like in specs explained
  static const gchar *iriref_pattern = "^(<([^>\x20]*)>)";

  static const size_t max_matches = 1;
  
  str = NULL;

  pthread_mutex_lock(&regex_mutex);
  
  if(!regex_compiled){
    regex_compiled = TRUE;
    
    ags_regcomp(&iriref_regex, iriref_pattern, REG_EXTENDED);
  }

  pthread_mutex_unlock(&regex_mutex);

  if(ags_regexec(&iriref_regex, offset, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);

#ifdef AGS_DEBUG
    g_message("iriref %s", str);
#endif
  }
  
  return(str);
}

/**
 * ags_turtle_read_pname_ns:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read prefixed-name namespace value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_pname_ns(gchar *offset,
			 gchar *end_ptr)
{
  gchar *pn_prefix;
  gchar *str;

  pn_prefix = ags_turtle_read_pn_prefix(offset,
					end_ptr);

  if(pn_prefix != NULL &&
     &(offset[strlen(pn_prefix)]) < end_ptr &&
     offset[strlen(pn_prefix)] == ':'){
    str = g_strdup_printf("%s:",
			  pn_prefix);
  }else{
    if(offset[0] == ':'){
      str = g_strdup(":");
    }else{
      str = NULL;
    }
  }

  if(pn_prefix != NULL){
    free(pn_prefix);
  }
  
  return(str);
}

/**
 * ags_turtle_read_pname_ln:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read prefixed-name localized name value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_pname_ln(gchar *offset,
			 gchar *end_ptr)
{
  gchar *pname_ns, *pn_local;
  gchar *str;

  str = NULL;

  pname_ns = ags_turtle_read_pname_ns(offset,
				      end_ptr);
  pn_local = NULL;
  
  if(pname_ns != NULL){
    offset += strlen(pname_ns);
    
    pn_local = ags_turtle_read_pn_local(offset,
					end_ptr);

    if(pn_local != NULL){
      str = g_strdup_printf("%s%s",
			    pname_ns,
			    pn_local);
    }
  }

  if(pname_ns != NULL){
    free(pname_ns);
  }

  if(pn_local != NULL){
    free(pn_local);
  }
  
  return(str);
}

/**
 * ags_turtle_read_blank_node_label:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read blank node label value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_blank_node_label(gchar *offset,
				 gchar *end_ptr)
{
  gchar *str;
  gchar *tmp, *tmp_str;
  
  gboolean initial_find, found_str;
  gboolean last_is_point;
  
  if(offset + 2 >= end_ptr){
    return(NULL);
  }
  
  str = NULL;

  tmp_str = NULL;

  if(!g_str_has_prefix(offset,
		       "_:")){
    return(NULL);
  }
  
  if((tmp = ags_turtle_read_pn_chars_u(offset + 2,
				       end_ptr)) != NULL ||
     g_ascii_isdigit(offset[2])){
    if(tmp == NULL){
      str = g_strdup_printf("_:%c", offset[2]);
      offset += 3;
    }else{
      str = g_strdup_printf("_:%s", tmp);
      offset += (2 + strlen(tmp));
	
      free(tmp);
    }
  }
  
  initial_find = TRUE;
  found_str = FALSE;
  
  last_is_point = FALSE;
    
  while(initial_find ||
	found_str){
    initial_find = FALSE;
    found_str = FALSE;
    
    last_is_point = FALSE;

    tmp = ags_turtle_read_pn_chars(offset,
				   end_ptr);

    if(tmp == NULL){
      if(*offset == '.'){
	tmp_str = g_strdup_printf("%s.",
				  str);

	free(str);

	str = tmp_str;

	offset++;
	
	found_str = TRUE;
	
	last_is_point = TRUE;
      }else{
	if(initial_find){
	  return(NULL);
	}else{
	  break;
	}
      }
    }else{
      tmp_str = g_strdup_printf("%s%s",
				str, tmp);
      offset += strlen(tmp);
      
      free(str);
      free(tmp);

      str = tmp_str;
      
      found_str = TRUE;
    }
  }

  if(last_is_point){
    g_warning("ags_turtle.c - syntax error");
  }
  
  return(str);
}

/**
 * ags_turtle_read_langtag:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read langtag value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_langtag(gchar *offset,
			gchar *end_ptr)
{
  regmatch_t match_arr[1];
  
  gchar *str;

  static regex_t langtag_regex;

  static gboolean regex_compiled = FALSE;
    
  static const char *langtag_pattern = "^(@[a-zA-Z]+(-[a-zA-Z0-9]+)*)";
  
  static const size_t max_matches = 1;
  
  str = NULL;

  pthread_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&langtag_regex, langtag_pattern, REG_EXTENDED);
  }

  pthread_mutex_unlock(&regex_mutex);

  if(ags_regexec(&langtag_regex, offset, max_matches, match_arr, 0) == 0){
    if(match_arr[0].rm_eo > match_arr[0].rm_so){
      str = g_strndup(offset,
		      match_arr[0].rm_eo - match_arr[0].rm_so);
    }
  }
  
  return(str);
}

/**
 * ags_turtle_read_boolean:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read boolean value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_boolean(gchar *offset,
			gchar *end_ptr)
{
  regmatch_t match_arr[1];
  
  gchar *str;
  
  static regex_t boolean_literal_regex;

  static gboolean regex_compiled = FALSE;
    
  static const char *boolean_literal_pattern = "^(true|false)";

  static const size_t max_matches = 1;
  
  str = NULL;

  pthread_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&boolean_literal_regex, boolean_literal_pattern, REG_EXTENDED);
  }

  pthread_mutex_unlock(&regex_mutex);

  if(ags_regexec(&boolean_literal_regex, offset, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

/**
 * ags_turtle_read_integer:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read integer value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_integer(gchar *offset,
			gchar *end_ptr)
{
  regmatch_t match_arr[1];
  
  gchar *str;
  
  static regex_t integer_literal_regex;

  static gboolean regex_compiled = FALSE;
    
  static const char *integer_literal_pattern = "^([+-]?[0-9]+)";

  static const size_t max_matches = 1;
  
  str = NULL;

  pthread_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&integer_literal_regex, integer_literal_pattern, REG_EXTENDED);
  }

  pthread_mutex_unlock(&regex_mutex);

  if(ags_regexec(&integer_literal_regex, offset, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

/**
 * ags_turtle_read_decimal:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read decimal value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_decimal(gchar *offset,
			gchar *end_ptr)
{
  regmatch_t match_arr[1];
  
  gchar *str;

  static regex_t decimal_literal_regex;

  static gboolean regex_compiled = FALSE;

  static const char *decimal_literal_pattern = "^([\\+\\-]?[0-9]*\\.[0-9]+)";

  static const size_t max_matches = 1;
    
  str = NULL;

  pthread_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&decimal_literal_regex, decimal_literal_pattern, REG_EXTENDED);
  }

  pthread_mutex_unlock(&regex_mutex);

  if(ags_regexec(&decimal_literal_regex, offset, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

/**
 * ags_turtle_read_double:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read double value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_double(gchar *offset,
		       gchar *end_ptr)
{
  regmatch_t match_arr[1];
  
  gchar *str;

  static regex_t double_literal_regex;
  
  static gboolean regex_compiled = FALSE;
  
  static const char *double_literal_pattern = "^([\\+\\-]?(([0-9]+\\.[0-9]*[eE][\\+\\-]?[0-9]+)|(\\.[0-9]+[eE][\\+\\-]?[0-9]+)|([0-9]+[eE][+-]?[0-9]+)))";

  static const size_t max_matches = 1;
    
  str = NULL;

  pthread_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&double_literal_regex, double_literal_pattern, REG_EXTENDED);
  }

  pthread_mutex_unlock(&regex_mutex);

  if(ags_regexec(&double_literal_regex, offset, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

/**
 * ags_turtle_read_exponent:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read exponent value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_exponent(gchar *offset,
			 gchar *end_ptr)
{
  regmatch_t match_arr[1];
  
  gchar *str;

  static regex_t exponent_literal_regex;

  static gboolean regex_compiled = FALSE;
  
  static const char *exponent_literal_pattern = "^([eE][+-]?[0-9]+)";

  static const size_t max_matches = 1;
    
  str = NULL;

  pthread_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;

    ags_regcomp(&exponent_literal_regex, exponent_literal_pattern, REG_EXTENDED);
  }

  pthread_mutex_unlock(&regex_mutex);

  if(ags_regexec(&exponent_literal_regex, offset, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

/**
 * ags_turtle_read_string:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read string value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_string(gchar *offset,
		       gchar *end_ptr)
{
  gchar *str;

  str = ags_turtle_read_string_literal_long_quote(offset,
						  end_ptr);
  
  if(str != NULL){
    return(str);
  }

  str = ags_turtle_read_string_literal_long_single_quote(offset,
							 end_ptr);
  
  if(str != NULL){
    return(str);
  }

  str = ags_turtle_read_string_literal_quote(offset,
					     end_ptr);
  
  if(str != NULL){
    return(str);
  }

  str = ags_turtle_read_string_literal_single_quote(offset,
						    end_ptr);
  
  if(str != NULL){
    return(str);
  }

  return(NULL);
}

/**
 * ags_turtle_read_string_literal_quote:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read string literal quote value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_string_literal_quote(gchar *offset,
				     gchar *end_ptr)
{
  regmatch_t match_arr[1];
  
  gchar *str;

  static regex_t string_literal_double_quote_regex;

  static gboolean regex_compiled = FALSE;
  
  static const char *string_literal_double_quote_pattern = "^(\"((([\\\\]['])|[^\"])*)\")";

  static const size_t max_matches = 1;

  if(offset >= end_ptr){
    return(NULL);
  }
  
  str = NULL;

  pthread_mutex_lock(&regex_mutex);
  
  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&string_literal_double_quote_regex, string_literal_double_quote_pattern, REG_EXTENDED);
  }

  pthread_mutex_unlock(&regex_mutex);

  if(ags_regexec(&string_literal_double_quote_regex, offset, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

/**
 * ags_turtle_read_string_literal_single_quote:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read string literal single quote value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_string_literal_single_quote(gchar *offset,
					    gchar *end_ptr)
{
  regmatch_t match_arr[1];
  
  gchar *str;

  static regex_t string_literal_single_quote_regex;

  static gboolean regex_compiled = FALSE;
  
  static const char *string_literal_single_quote_pattern = "^('((([\\\\]['])|[^'])*)')";

  static const size_t max_matches = 1;
    
  if(offset >= end_ptr){
    return(NULL);
  }
  
  str = NULL;

  pthread_mutex_lock(&regex_mutex);
  
  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&string_literal_single_quote_regex, string_literal_single_quote_pattern, REG_EXTENDED);
  }

  pthread_mutex_unlock(&regex_mutex);

  if(ags_regexec(&string_literal_single_quote_regex, offset, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

/**
 * ags_turtle_read_string_literal_long_quote:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read string literal long quote value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_string_literal_long_quote(gchar *offset,
					  gchar *end_ptr)
{
  gchar *str, *end;

  str = NULL;
  
  if(g_str_has_prefix(offset,
		      "\"\"\"")){
    end = offset + 3;
    
    while((end = strstr(end,
			"\"\"\"")) != NULL &&
	  *(end - 1) == '\\'){
      end++;
    }

    if(end != NULL){
      str = g_strndup(offset,
		      end - offset + 3);
    }
  }

  return(str);
}

/**
 * ags_turtle_read_string_literal_long_single_quote:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read string literal long single quote value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_string_literal_long_single_quote(gchar *offset,
						 gchar *end_ptr)
{
  gchar *str, *end;

  str = NULL;
  
  if(g_str_has_prefix(offset,
		      "'''")){
    end = offset + 3;
    
    while((end = strstr(end,
			"'''")) != NULL &&
	  *(end - 1) == '\\'){
      end++;
    }

    if(end != NULL){
      str = g_strndup(offset,
		      end - offset + 3);
    }
  }

  return(str);
}

/**
 * ags_turtle_read_uchar:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read uchar value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_uchar(gchar *offset,
		      gchar *end_ptr)
{
  gchar *str;

  guint hex_digit_count;
  guint i;
  gboolean success;

  if(offset + 1 >= end_ptr){
    return(NULL);
  }
  
  str = NULL;

  if(g_str_has_prefix(offset,
		      "\\u")){
    hex_digit_count = 4;
  }else if(g_str_has_prefix(offset,
			    "\\U")){
    hex_digit_count = 8;
  }else{
    return(NULL);
  }

  success = TRUE;
  
  for(i = 0; i < hex_digit_count; i++){
    if(!g_ascii_isxdigit(offset[i + 2])){
      success = FALSE;

      break;
    }
  }

  if(success){
    str = g_strndup(offset,
		    hex_digit_count + 2);
  }
  
  return(str);
}

/**
 * ags_turtle_read_echar:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read echar value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_echar(gchar *offset,
		      gchar *end_ptr)
{
  gchar *str;

  static const gchar *echar = "tbnrf\"";
  
  if(offset + 1 >= end_ptr){
    return(NULL);
  }
  
  str = NULL;

  if(*offset == '\\'){
    if(index(echar,
	     offset[1]) != NULL){
      str = g_strndup(offset,
		      2);
    }
  }

  return(str);
}

/**
 * ags_turtle_read_ws:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read ws value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_ws(gchar *offset,
		   gchar *end_ptr)
{
  gchar *str;

  static const gchar *ws = "\x20\x09\x0D\x0A";

  str = NULL;

  if(offset < end_ptr &&
     index(ws, *offset) != NULL){
    str = g_strndup(offset,
		    1);
  }
  
  return(str);
}

/**
 * ags_turtle_read_anon:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read anon value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_anon(gchar *offset,
		     gchar *end_ptr)
{
  gchar *str;
  gchar *tmp, *tmp_str;

  gboolean success;
  
  static const gchar *ws = "\x20\x09\x0D\x0A";

  if(offset >= end_ptr){
    return(NULL);
  }
  
  str = NULL;

  success = FALSE;
  
  if(*offset == '['){
    tmp = index(offset + 1,
		']');

    if(tmp != NULL){
      success = TRUE;
      
      for(tmp_str = offset + 1; tmp_str < tmp; tmp_str++){
	if(index(ws,
		 *tmp_str) == NULL){
	  success = FALSE;
	  
	  break;
	}
      }
    }
  }

  if(success){
    str = g_strndup(offset,
		    tmp - offset + 1);
  }
  
  return(str);
}

#define AGS_TURTLE_UTF8_RANGE_0 "([A-Za-z])"
#define AGS_TURTLE_UTF8_RANGE_1 "(\xC3[\x80-\x96])"
#define AGS_TURTLE_UTF8_RANGE_2 "(\xC3[\x98-\xB6])"
#define AGS_TURTLE_UTF8_RANGE_3 "((\xC3[\xB8-\xBF])|([\xC3-\xCA][\x80-\xBF])|(\xCB[\x80-\xBF]))"
#define AGS_TURTLE_UTF8_RANGE_4 "(\xCD[\xB0-\xBD])"
#define AGS_TURTLE_UTF8_RANGE_5 "((\xCD[\xBF-\xDF])|([\xCE-\xDF][\x80-\xBF])|([\xE0-\xE1][\x80-\xBF][\x80-\xBF]))"
#define AGS_TURTLE_UTF8_RANGE_6 "(\xE2\x80[\x8C-\x8D])"
#define AGS_TURTLE_UTF8_RANGE_7 "((\xE2\x81[\xB0-\xBF])|(\xE2[\x81-\x85][\x80-\xBF])|(\xE2\x86[\x80-\x8F]))"
#define AGS_TURTLE_UTF8_RANGE_8 "((\xE2[\xB0-\xBE][\x80-\xBF])|(\xE2\xBF[\x80-\xAF]))"
#define AGS_TURTLE_UTF8_RANGE_9 "((\xE3[\x80-\xBF][\x81-\xBF])|([\xE4-\xEC][\x80-\x9F][\x80-\xBF]))"
#define AGS_TURTLE_UTF8_RANGE_10 "((\xEF[\xA4-\xB6][\x80-\xBF])|(\xEF\xB7[\x80-\x8F]))"
#define AGS_TURTLE_UTF8_RANGE_11 "((\xEF\xB7[\xB0-\xBF])|(\xEF[\xB8-\xBE][\x80-\xBF])|(\xEF\xBF[\x80-\xBD]))"
#define AGS_TURTLE_UTF8_RANGE_12 "(([\xF0-\xF3][\x90-\xAF][\x80-\xBF][\x80-\xBF]))"

#define AGS_TURTLE_UTF8_RANGE_ALL "(" AGS_TURTLE_UTF8_RANGE_0 "|" \
  AGS_TURTLE_UTF8_RANGE_1 "|" \
  AGS_TURTLE_UTF8_RANGE_2 "|" \
  AGS_TURTLE_UTF8_RANGE_3 "|" \
  AGS_TURTLE_UTF8_RANGE_4 "|" \
  AGS_TURTLE_UTF8_RANGE_5 "|" \
  AGS_TURTLE_UTF8_RANGE_6 "|" \
  AGS_TURTLE_UTF8_RANGE_7 "|" \
  AGS_TURTLE_UTF8_RANGE_8 "|" \
  AGS_TURTLE_UTF8_RANGE_9 "|" \
  AGS_TURTLE_UTF8_RANGE_10 "|" \
  AGS_TURTLE_UTF8_RANGE_11 "|" \
  AGS_TURTLE_UTF8_RANGE_12 ")"

#define AGS_TURLTE_UTF8_RANGE_ALL_PATTERN "^" AGS_TURTLE_UTF8_RANGE_ALL

/**
 * ags_turtle_read_pn_chars_base:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read prefixed-name chars base value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_pn_chars_base(gchar *offset,
			      gchar *end_ptr)
{
  regmatch_t match_arr[1];
    
  gchar *str;
  
  static regex_t chars_base_regex;
    
  static gboolean regex_compiled = FALSE;

  static const char *chars_base_pattern = AGS_TURLTE_UTF8_RANGE_ALL_PATTERN;

  static const size_t max_matches = 1;

  if(offset == NULL ||
     offset >= end_ptr){
    return(NULL);
  }

  str = NULL;

  pthread_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;
    
    if(ags_regcomp(&chars_base_regex, chars_base_pattern, REG_EXTENDED)){
      g_warning("failed to compile regex: %s", chars_base_pattern);
    }
  }

  pthread_mutex_unlock(&regex_mutex);

  if(ags_regexec(&chars_base_regex, offset, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

/**
 * ags_turtle_read_pn_chars_u:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read prefixed-name chars underscore value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_pn_chars_u(gchar *offset,
			   gchar *end_ptr)
{
  gchar *str;

  str = ags_turtle_read_pn_chars_base(offset,
				      end_ptr);

  if(str == NULL &&
     offset < end_ptr &&
     *offset == '_'){
    str = g_strdup("_");
  }
  
  return(str);
}

/**
 * ags_turtle_read_pn_chars:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read prefixed-name chars value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_pn_chars(gchar *offset,
			 gchar *end_ptr)
{
  regmatch_t match_arr[1];
    
  gchar *str;
  
  static regex_t chars_regex;
    
  static gboolean regex_compiled = FALSE;
  
  static const char *chars_pattern = "^(([0-9])|(\xC2\xB7)|((\xCC[\x80-\xBF])|(\xCD[\x80-\xAF]))|((\xE2\x80\xBF)|(\xE2\x81\x80)))";

  static const size_t max_matches = 1;

  str = ags_turtle_read_pn_chars_u(offset,
				   end_ptr);

  if(str == NULL &&
     offset < end_ptr &&
     *offset == '-'){
    str = g_strdup("-");
  }

  if(str == NULL &&
     offset < end_ptr){
    pthread_mutex_lock(&regex_mutex);
    
    if(!regex_compiled){
      regex_compiled = TRUE;
    
      ags_regcomp(&chars_regex, chars_pattern, REG_EXTENDED);
    }

    pthread_mutex_unlock(&regex_mutex);

    if(ags_regexec(&chars_regex, offset, max_matches, match_arr, 0) == 0){
      str = g_strndup(offset,
		      match_arr[0].rm_eo - match_arr[0].rm_so);
    }
  }
  
  return(str);
}

/**
 * ags_turtle_read_pn_prefix:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read prefixe-name prefix value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_pn_prefix(gchar *offset,
			  gchar *end_ptr)
{
  gchar *str;
  gchar *tmp, *str_tmp;

  gboolean last_is_point;

  if(offset == NULL ||
     offset >= end_ptr){
    return(NULL);
  }
    
  str = ags_turtle_read_pn_chars_base(offset,
				      end_ptr);
  
  if(str != NULL){
    offset += strlen(str);
    last_is_point = FALSE;
    
    while((tmp = ags_turtle_read_pn_chars(offset,
					  end_ptr)) != NULL ||
	  *offset == '.'){
      if(tmp == NULL){
	last_is_point = TRUE;
      }else{
	last_is_point = FALSE;
      }

      if(tmp != NULL){
	str_tmp = g_strdup_printf("%s%s",
				  str, tmp);
	offset += strlen(tmp);

	free(str);
	free(tmp);

	str = str_tmp;
      }else{
	str_tmp = g_strdup_printf("%s.",
				  str);
	offset++;	

	free(str);

	str = str_tmp;
      }
    }
  }
  
  return(str);
}

/**
 * ags_turtle_read_pn_local:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read prefixed-name local value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_pn_local(gchar *offset,
			 gchar *end_ptr)
{
  gchar *str;
  gchar *tmp_str, *tmp;

  gboolean initial_find, found_str;
  gboolean last_is_point;

  if(offset >= end_ptr){
    return(NULL);
  }
  
  tmp_str = NULL;

  found_str = FALSE;
  
  tmp = ags_turtle_read_pn_chars_u(offset,
				   end_ptr);
        
  if(tmp == NULL &&
     *offset == ':'){
    tmp = g_strdup(":");
  }

  if(tmp == NULL &&
     g_ascii_isdigit(*offset)){
    tmp = g_strndup(offset,
		    1);
  }

  if(tmp == NULL){
    tmp = ags_turtle_read_plx(offset, end_ptr);
  }

  str = tmp;
  
  if(str != NULL){
    offset += strlen(str);

    initial_find = TRUE;
    last_is_point = FALSE;
    
    while(initial_find ||
	  found_str){
      found_str = FALSE;
      initial_find = FALSE;
    
      last_is_point = FALSE;

      tmp = ags_turtle_read_pn_chars(offset,
				     end_ptr);
      
      if(tmp == NULL &&
	 *offset == '.'){
	tmp = g_strdup(".");
	
	last_is_point = TRUE;
      }

      if(tmp == NULL &&
	 *offset == ':'){
	tmp = g_strdup(":");	
      }

      if(tmp == NULL){
	tmp = ags_turtle_read_plx(offset,
				  end_ptr);
      }
    
      /* concat new strings */
      if(tmp != NULL){
	found_str = TRUE;

	if(str != NULL){
	  tmp_str = g_strdup_printf("%s%s",
				    str,
				    tmp);
	  offset += strlen(tmp);
	
	  free(str);
	  free(tmp);
	
	  str = tmp_str;
	}else{
	  str = tmp;
	  offset += strlen(tmp);
	}
      }
    }
  }
  
  return(str);
}

/**
 * ags_turtle_read_plx:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read plx value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_plx(gchar *offset,
		    gchar *end_ptr)
{
  gchar *str;

  str = ags_turtle_read_percent(offset,
				end_ptr);

  if(str == NULL){
    str = ags_turtle_read_pn_local_esc(offset,
				       end_ptr);
  }
  
  return(str);
}

/**
 * ags_turtle_read_percent:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read percent value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_percent(gchar *offset,
			gchar *end_ptr)
{
  gchar *str;

  str = NULL;

  if(offset != NULL &&
     offset + 2 < end_ptr &&
     offset[0] == '%' &&
     g_ascii_isxdigit(offset[1]) &&
     g_ascii_isxdigit(offset[2])){
    str = g_strndup(offset,
		     3);
  }
  
  return(str);
}

/**
 * ags_turtle_read_hex:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read hex value.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_hex(gchar *offset,
		    gchar *end_ptr)
{
  gchar *str;

  str = NULL;

  if(g_ascii_isxdigit(*offset)){
    str = g_strndup(offset,
		    1);
  }
  
  return(str);
}

/**
 * ags_turtle_read_pn_local_esc:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read prefixed name local escapes.
 *
 * Returns: a string on success otherwise %NULL
 * 
 * Since: 0.7.4
 */
gchar*
ags_turtle_read_pn_local_esc(gchar *offset,
			     gchar *end_ptr)
{
  static const gchar *escapes = "_~.-!$&'()*+,;=/?#@%";
  gchar *str;

  str = NULL;

  if(offset != NULL &&
     offset + 1 < end_ptr &&
     *offset == '\\' &&
     (index(escapes,
	    offset[1])) != NULL){
    str = g_strndup(offset,
		    2);
  }
  
  return(str);
}

/**
 * ags_turtle_find_xpath:
 * @turtle: the #AgsTurtle
 * @xpath: a XPath expression as string
 *
 * Lookup XPath expression withing @turtle.
 *
 * Returns: a #GList-struct containing xmlNode
 *
 * Since: 0.7.0
 */
GList*
ags_turtle_find_xpath(AgsTurtle *turtle,
		      gchar *xpath)
{
  xmlXPathContext *xpath_context;
  xmlXPathObject *xpath_object;
  xmlNode **node;
  GList *list;

  guint i;
  
  xpath_context = xmlXPathNewContext(turtle->doc);
  xpath_object = xmlXPathEval((xmlChar *) xpath,
			      xpath_context);
  
  list = NULL;

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	list = g_list_prepend(list,
			      node[i]);
      }
    }
  }

  list = g_list_reverse(list);
  
  return(list);
}

/**
 * ags_turtle_find_xpath_with_context_node:
 * @turtle: the #AgsTurtle
 * @xpath: a XPath expression as string
 * @context_node: a #xmlNode-struct
 *
 * Lookup XPath expression from @context_node withing @turtle.
 *
 * Returns: a #GList-struct containing xmlNode
 *
 * Since: 0.7.0
 */
GList*
ags_turtle_find_xpath_with_context_node(AgsTurtle *turtle,
					gchar *xpath,
					xmlNode *context_node)
{
  xmlXPathContext *xpath_context;
  xmlXPathObject *xpath_object;
  xmlNode **node;
  GList *list;

  guint i;
  
  xpath_context = xmlXPathNewContext(turtle->doc);
  xpath_context->node = context_node;
  xpath_object = xmlXPathEval((xmlChar *) xpath,
			      xpath_context);
  
  list = NULL;

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	list = g_list_prepend(list,
			      node[i]);
      }
    }
  }

  if(list != NULL){
    list = g_list_reverse(list);
  }
  
  return(list);
}

gchar*
ags_turtle_string_convert(AgsTurtle *turtle, gchar *str)
{
  if((AGS_TURTLE_TOLOWER & (turtle->flags)) != 0){
    str = g_ascii_strdown(str,
			  -1);
  }
  
  return(str);
}

/**
 * ags_turtle_load:
 * @turtle: the #AgsTurtle
 * @error: a pointer to a #GError
 *
 * Loads a RDF triple file into an XML Document.
 *
 * Returns: a #xmlDoc pointer
 *
 * Since: 0.7.0
 */
xmlDoc*
ags_turtle_load(AgsTurtle *turtle,
		GError **error)
{
  xmlDoc *doc;
  xmlNode *root_node, *rdf_statement_node;
  FILE *file;

  struct stat *sb;
  
  gchar *buffer, *iter;

  size_t n_read;
  
  auto gchar* ags_turtle_load_skip_comments_and_blanks(gchar **iter);
  auto xmlNode* ags_turtle_load_read_iriref(gchar **iter);
  auto xmlNode* ags_turtle_load_read_anon(gchar **iter);
  auto xmlNode* ags_turtle_load_read_pname_ns(gchar **iter);
  auto xmlNode* ags_turtle_load_read_pname_ln(gchar **iter);
  auto xmlNode* ags_turtle_load_read_numeric(gchar **iter);
  auto xmlNode* ags_turtle_load_read_string(gchar **iter);
  auto xmlNode* ags_turtle_load_read_langtag(gchar **iter);
  auto xmlNode* ags_turtle_load_read_literal(gchar **iter);
  auto xmlNode* ags_turtle_load_read_iri(gchar **iter);
  auto xmlNode* ags_turtle_load_read_prefix_id(gchar **iter);
  auto xmlNode* ags_turtle_load_read_base(gchar **iter);
  auto xmlNode* ags_turtle_load_read_sparql_prefix(gchar **iter);
  auto xmlNode* ags_turtle_load_read_sparql_base(gchar **iter);
  auto xmlNode* ags_turtle_load_read_prefixed_name(gchar **iter);
  auto xmlNode* ags_turtle_load_read_blank_node(gchar **iter);
  auto xmlNode* ags_turtle_load_read_statement(gchar **iter);
  auto xmlNode* ags_turtle_load_read_verb(gchar **iter);
  auto xmlNode* ags_turtle_load_read_predicate(gchar **iter);
  auto xmlNode* ags_turtle_load_read_object(gchar **iter);
  auto xmlNode* ags_turtle_load_read_directive(gchar **iter);
  auto xmlNode* ags_turtle_load_read_triple(gchar **iter);
  auto xmlNode* ags_turtle_load_read_subject(gchar **iter);
  auto xmlNode* ags_turtle_load_read_object_list(gchar **iter);
  auto xmlNode* ags_turtle_load_read_collection(gchar **iter);
  auto xmlNode* ags_turtle_load_read_blank_node_property_list(gchar **iter);
  auto xmlNode* ags_turtle_load_read_predicate_object_list(gchar **iter);
  
  gchar* ags_turtle_load_skip_comments_and_blanks(gchar **iter){
    gchar *look_ahead;

    if(iter == NULL){
      return(NULL);
    }

    look_ahead = *iter;

    if(look_ahead == NULL){
      return(NULL);
    }
    
    /* skip whitespaces and comments */
    for(; (look_ahead < &(buffer[sb->st_size])) && *look_ahead != '\0'; look_ahead++){
      /* skip comments */
      if(buffer == look_ahead){
	if(*buffer == '#'){
	  look_ahead = index(look_ahead, '\n') + 1;
	  continue;
	}
      }else if(buffer[look_ahead - buffer - 1] == '\n' && *look_ahead == '#'){
	look_ahead = index(look_ahead, '\n') + 1;
	continue;
      }

      /* spaces */
      if(!(*look_ahead == ' ' || *look_ahead == '\t' || *look_ahead == '\n')){
	break;
      }
    }

    return(look_ahead);
  }

  xmlNode* ags_turtle_load_read_iriref(gchar **iter){
    xmlNode *node;

    gchar *look_ahead;
    gchar *str;
    
    node = NULL;
    look_ahead = *iter;
    
    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    /* read iriref */
    str = ags_turtle_read_iriref(look_ahead,
				 &(buffer[sb->st_size]));

    if(str != NULL){
      node = xmlNewNode(NULL,
			"rdf-iriref");
      xmlNodeSetContent(node,
			str);
      //      xmlNodeSetContent(node,
      //		g_ascii_strdown(str,
      //				-1));

#ifdef AGS_DEBUG
      g_message("iriref = %s", str);
#endif
      
      *iter = look_ahead + strlen(str);
    }
    
    return(node);
  }
  
  xmlNode* ags_turtle_load_read_anon(gchar **iter){
    xmlNode *node;

    gchar *look_ahead;
    gchar *str;
    
    node = NULL;
    look_ahead = *iter;
    
    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    /* read anon */
    str = ags_turtle_read_anon(look_ahead,
			       &(buffer[sb->st_size]));

    if(str != NULL){
      node = xmlNewNode(NULL,
			"rdf-anon");
      xmlNodeSetContent(node,
			g_ascii_strdown(str,
					-1));

#ifdef AGS_DEBUG
      g_message("anon - %s", str);
#endif
      
      *iter = look_ahead + strlen(str);
    }
    
    return(node);
  }

  xmlNode* ags_turtle_load_read_pname_ns(gchar **iter){
    xmlNode *node;

    gchar *look_ahead;
    gchar *str;
    
    node = NULL;
    look_ahead = *iter;
    
    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    /* read pname-ns */
    str = ags_turtle_read_pname_ns(look_ahead,
				   &(buffer[sb->st_size]));
    
    if(str != NULL){
      node = xmlNewNode(NULL,
			"rdf-pname-ns");
      xmlNodeSetContent(node,
			g_ascii_strdown(str,
					-1));

#ifdef AGS_DEBUG
      g_message("pname-ns - %s", str);
#endif
      
      *iter = look_ahead + strlen(str);
    }
    
    return(node);
  }
  
  xmlNode* ags_turtle_load_read_pname_ln(gchar **iter){
    xmlNode *node;

    gchar *look_ahead;
    gchar *str;
    
    node = NULL;
    look_ahead = *iter;
    
    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    /* read pname-ln */
    str = ags_turtle_read_pname_ln(look_ahead,
				   &(buffer[sb->st_size]));
    
    if(str != NULL){
      node = xmlNewNode(NULL,
			"rdf-pname-ln");
      xmlNodeSetContent(node,
			g_ascii_strdown(str,
					-1));

#ifdef AGS_DEBUG
      g_message("pname-ln - %s", str);
#endif
      
      *iter = look_ahead + strlen(str);
    }
    
    return(node);
  }

  xmlNode* ags_turtle_load_read_numeric(gchar **iter){
    xmlNode *node;

    gchar *look_ahead;
    gchar *str;
    
    node = NULL;
    look_ahead = *iter;
    
    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    /* read numeric */
    if(str == NULL){
      str = ags_turtle_read_decimal(look_ahead,
				    &(buffer[sb->st_size]));
    }

    if(str == NULL){
      str = ags_turtle_read_double(look_ahead,
				   &(buffer[sb->st_size]));      
    }

    if(str == NULL){
      str = ags_turtle_read_integer(look_ahead,
				    &(buffer[sb->st_size]));
    }

    if(str == NULL){
      str = ags_turtle_read_boolean(look_ahead,
				    &(buffer[sb->st_size]));      
    }
    
    if(str != NULL){
      node = xmlNewNode(NULL,
			"rdf-numeric");
      xmlNodeSetContent(node,
			g_ascii_strdown(str,
					-1));

#ifdef AGS_DEBUG
      g_message("numeric - %s", str);
#endif
      
      *iter = look_ahead + strlen(str);
    }
    
    return(node);
  }

  xmlNode* ags_turtle_load_read_string(gchar **iter){
    xmlNode *node;

    gchar *look_ahead;
    gchar *str;
    
    node = NULL;
    look_ahead = *iter;
    
    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    /* read pname-ns */
    str = ags_turtle_read_string(look_ahead,
				 &(buffer[sb->st_size]));
    
    if(str != NULL){
      node = xmlNewNode(NULL,
			"rdf-string");
      xmlNodeSetContent(node,
			xmlEncodeSpecialChars(doc,
					      str));

#ifdef AGS_DEBUG
      g_message("string - %s", str);
#endif
      
      *iter = look_ahead + strlen(str);
    }
    
    return(node);
  }

  xmlNode* ags_turtle_load_read_langtag(gchar **iter){
    xmlNode *node;

    gchar *look_ahead;
    gchar *str;
    
    node = NULL;
    look_ahead = *iter;
    
    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    /* read langtag */
    str = ags_turtle_read_langtag(look_ahead,
				  &(buffer[sb->st_size]));
    
    if(str != NULL){
      node = xmlNewNode(NULL,
			"rdf-langtag");
      xmlNodeSetContent(node,
			g_ascii_strdown(str,
					-1));

#ifdef AGS_DEBUG
      g_message("langtag - %s", str);
#endif
      
      *iter = look_ahead + strlen(str);
    }
    
    return(node);
  }

  xmlNode* ags_turtle_load_read_literal(gchar **iter){
    xmlNode *node;
    xmlNode *rdf_string_node, *rdf_langtag_node, *rdf_iri_node;
    
    gchar *look_ahead;
    
    node = NULL;
    look_ahead = *iter;

    /* read string */
    rdf_string_node = ags_turtle_load_read_string(&look_ahead);

    if(rdf_string_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-literal");
      xmlAddChild(node,
		  rdf_string_node);		  

      *iter = look_ahead;

      /* read optional langtag */
      rdf_langtag_node = ags_turtle_load_read_langtag(&look_ahead);

      if(rdf_langtag_node != NULL){
	xmlAddChild(node,
		    rdf_langtag_node);

	*iter = look_ahead;
      }else{
	/* skip blanks and comments */
	look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

	if(look_ahead + 2 < &(buffer[sb->st_size]) &&
	   look_ahead[0] == '^' &&
	   look_ahead[1] == '^'){
	  /* alternate r2ead optional iri */
	  rdf_iri_node = ags_turtle_load_read_iri(&look_ahead);
	
	  if(rdf_iri_node != NULL){
	    xmlAddChild(node,
			rdf_iri_node);
	  
	    *iter = look_ahead;
	  }
	}
      }
    }else{
      xmlNode *rdf_numeric_node;

      rdf_numeric_node = ags_turtle_load_read_numeric(&look_ahead);

      if(rdf_numeric_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-literal");
	xmlAddChild(node,
		    rdf_numeric_node);

	*iter = look_ahead;
      }      
    }
    
    return(node);
  }

  xmlNode* ags_turtle_load_read_iri(gchar **iter){
    xmlNode *node;
    xmlNode *rdf_iriref_node, *rdf_prefixed_name_node;
    
    gchar *look_ahead;
    
    node = NULL;
    look_ahead = *iter;

    /* read iriref */
    rdf_iriref_node = ags_turtle_load_read_iriref(&look_ahead);

    if(rdf_iriref_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-iri");
      xmlAddChild(node,
		  rdf_iriref_node);
      
      *iter = look_ahead;
    }else{
      /* alternate read prefixed name */
      rdf_prefixed_name_node = ags_turtle_load_read_prefixed_name(&look_ahead);

      if(rdf_prefixed_name_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-iri");
	xmlAddChild(node,
		    rdf_prefixed_name_node);
      
	*iter = look_ahead;
      }
    }
    
    return(node);
  }

  xmlNode* ags_turtle_load_read_prefix_id(gchar **iter){
    xmlNode *node;
    xmlNode *rdf_pname_ns_node, *rdf_iriref_node;

    regmatch_t match_arr[1];

    gchar *look_ahead;
    
    static regex_t prefix_id_regex;

    static gboolean regex_compiled = FALSE;

    static const char *prefix_id_pattern = "^(@prefix)";
    
    static const size_t max_matches = 1;

    node = NULL;
    look_ahead = *iter;

    pthread_mutex_lock(&regex_mutex);

    if(!regex_compiled){
      regex_compiled = TRUE;
      
      ags_regcomp(&prefix_id_regex, prefix_id_pattern, REG_EXTENDED);
    }

    pthread_mutex_unlock(&regex_mutex);

    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    /* match @prefix */
    if(ags_regexec(&prefix_id_regex, look_ahead, max_matches, match_arr, 0) == 0){
      look_ahead += (match_arr[0].rm_eo - match_arr[0].rm_so);
      
      rdf_pname_ns_node = ags_turtle_load_read_pname_ns(&look_ahead);
      
      rdf_iriref_node = ags_turtle_load_read_iriref(&look_ahead);

      /* create node if complete prefix id */
      if(rdf_pname_ns_node != NULL &&
	 rdf_iriref_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-prefix-id");
	xmlAddChild(node,
		    rdf_pname_ns_node);
	xmlAddChild(node,
		    rdf_iriref_node);
	
	*iter = look_ahead;
      }
    }
    
    return(node);
  }
  
  xmlNode* ags_turtle_load_read_base(gchar **iter){
    xmlNode *node;
    xmlNode *rdf_iriref_node;

    regmatch_t match_arr[1];

    gchar *look_ahead;
    
    static regex_t base_regex;

    static gboolean regex_compiled = FALSE;

    static const char *base_pattern = "^(@base)";
    
    static const size_t max_matches = 1;

    node = NULL;
    look_ahead = *iter;

    pthread_mutex_lock(&regex_mutex);

    if(!regex_compiled){
      regex_compiled = TRUE;
      
      ags_regcomp(&base_regex, base_pattern, REG_EXTENDED);      
    }

    pthread_mutex_unlock(&regex_mutex);

    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    /* match @base */
    if(ags_regexec(&base_regex, look_ahead, max_matches, match_arr, 0) == 0){
      look_ahead += (match_arr[0].rm_eo - match_arr[0].rm_so);
      
      rdf_iriref_node = ags_turtle_load_read_iriref(&look_ahead);

      /* create node if complete base */
      if(rdf_iriref_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-base");
	xmlAddChild(node,
		    rdf_iriref_node);
	
	*iter = look_ahead;
      }
    }
    
    return(node);
  }
  
  xmlNode* ags_turtle_load_read_sparql_prefix(gchar **iter){
    xmlNode *node;
    xmlNode *rdf_pname_ns_node, *rdf_iriref_node;

    regmatch_t match_arr[1];

    gchar *look_ahead;
    
    static regex_t sparql_prefix_regex;

    static gboolean regex_compiled = FALSE;

    static const char *sparql_prefix_pattern = "^(\"PREFIX\")";
    
    static const size_t max_matches = 1;

    node = NULL;
    look_ahead = *iter;

    pthread_mutex_lock(&regex_mutex);

    if(!regex_compiled){
      regex_compiled = TRUE;
      
      ags_regcomp(&sparql_prefix_regex, sparql_prefix_pattern, REG_EXTENDED);
    }

    pthread_mutex_unlock(&regex_mutex);

    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    /* match @prefix */
    if(ags_regexec(&sparql_prefix_regex, look_ahead, max_matches, match_arr, 0) == 0){
      look_ahead += (match_arr[0].rm_eo - match_arr[0].rm_so);
      
      rdf_pname_ns_node = ags_turtle_load_read_pname_ns(&look_ahead);
      
      rdf_iriref_node = ags_turtle_load_read_iriref(&look_ahead);;

      /* create node if complete sparql prefix */
      if(rdf_pname_ns_node != NULL &&
	 rdf_iriref_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-sparql-prefix");
	xmlAddChild(node,
		    rdf_pname_ns_node);
	xmlAddChild(node,
		    rdf_iriref_node);
	
	*iter = look_ahead;
      }
    }
    
    return(node);
  }
  
  xmlNode* ags_turtle_load_read_sparql_base(gchar **iter){
    xmlNode *node;
    xmlNode *rdf_iriref_node;

    regmatch_t match_arr[1];

    gchar *look_ahead;
    
    static regex_t sparql_base_regex;

    static gboolean regex_compiled = FALSE;

    static const char *sparql_base_pattern = "^(\"BASE\")";
    
    static const size_t max_matches = 1;

    node = NULL;
    look_ahead = *iter;

    pthread_mutex_lock(&regex_mutex);

    if(!regex_compiled){
      regex_compiled = TRUE;
      
      ags_regcomp(&sparql_base_regex, sparql_base_pattern, REG_EXTENDED);      
    }

    pthread_mutex_unlock(&regex_mutex);

    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    /* match @base */
    if(ags_regexec(&sparql_base_regex, look_ahead, max_matches, match_arr, 0) == 0){
      look_ahead += (match_arr[0].rm_eo - match_arr[0].rm_so);
      
      rdf_iriref_node = ags_turtle_load_read_iriref(&look_ahead);;

      /* create node if complete sparqle base */
      if(rdf_iriref_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-sparql-base");
	xmlAddChild(node,
		    rdf_iriref_node);
	
	*iter = look_ahead;
      }
    }

    return(node);
  }
  
  xmlNode* ags_turtle_load_read_prefixed_name(gchar **iter){
    xmlNode *node;
    xmlNode *rdf_pname_ns_node, *rdf_pname_ln_node;

    gchar *look_ahead;

    node = NULL;
    look_ahead = *iter;

    rdf_pname_ln_node = ags_turtle_load_read_pname_ln(&look_ahead);

    if(rdf_pname_ln_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-prefixed-name");
      xmlAddChild(node,
		  rdf_pname_ln_node);		  

      *iter = look_ahead;
    }else{
      rdf_pname_ns_node = ags_turtle_load_read_pname_ns(&look_ahead);

      if(rdf_pname_ns_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-prefixed-name");
	xmlAddChild(node,
		    rdf_pname_ns_node);

	*iter = look_ahead;
      }
    }

    return(node);
  }

  xmlNode* ags_turtle_load_read_blank_node(gchar **iter){
    xmlNode *node;
    xmlNode *rdf_anon_node;

    gchar *rdf_blank_node_label;
    gchar *look_ahead;
    
    node = NULL;
    look_ahead = *iter;    

    rdf_blank_node_label = ags_turtle_read_blank_node_label(look_ahead,
							    &(buffer[sb->st_size]));
    
    if(rdf_blank_node_label != NULL){
      node = xmlNewNode(NULL,
			"rdf-blank-node");
      xmlNodeSetContent(node,
			g_ascii_strdown(rdf_blank_node_label,
					-1));

      *iter = look_ahead;
    }else{
      rdf_anon_node = ags_turtle_load_read_anon(&look_ahead);

      if(rdf_anon_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-blank-node");
	xmlAddChild(node,
		    rdf_anon_node);

	*iter = look_ahead;
      }
    }
    
    return(node);
  }

  xmlNode* ags_turtle_load_read_statement(gchar **iter){
    xmlNode *node;
    xmlNode *rdf_directive_node, *rdf_triple_node;
    
    gchar *look_ahead;
    
    node = NULL;
    look_ahead = *iter;    

    rdf_directive_node = ags_turtle_load_read_directive(&look_ahead);

    if(rdf_directive_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-statement");
      xmlAddChild(node,
		  rdf_directive_node);		  

#ifdef AGS_DEBUG
      g_message("====");
#endif
      
      *iter = index(look_ahead,
		    '.') + 1;
    }else{
      rdf_triple_node = ags_turtle_load_read_triple(&look_ahead);

      if(rdf_triple_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-statement");
	xmlAddChild(node,
		    rdf_triple_node);

#ifdef AGS_DEBUG
	g_message("-----");
#endif
	
	*iter = index(look_ahead,
		      '.') + 1;
      }
    }
    
    return(node);
  }

  xmlNode* ags_turtle_load_read_verb(gchar **iter){
    xmlNode *node;
    xmlNode *predicate_node;
    
    gchar *look_ahead;

    node = NULL;
    look_ahead = *iter;

    /* predicate */
    predicate_node = ags_turtle_load_read_predicate(&look_ahead);
	  
    if(predicate_node != NULL){
#ifdef AGS_DEBUG
      g_message("read rdf-verb");
#endif
      
      node = xmlNewNode(NULL,
			"rdf-verb");
	  
      xmlAddChild(node,
		  predicate_node);
	
      *iter = look_ahead;
    }else{
      /* skip blanks and comments */
      look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);
      
      if(*look_ahead == 'a'){
#ifdef AGS_DEBUG
	g_message("read - verb: a");
#endif
	
	node = xmlNewNode(NULL,
			  "rdf-verb");
      
	/* verb - a */
	xmlNewProp(node,
		   "verb",
		   "a");

	look_ahead += 1;
	*iter = look_ahead;
      }
    }
    
    return(node);
  }

  xmlNode* ags_turtle_load_read_predicate(gchar **iter){
    xmlNode *node;
    xmlNode *iri_node;

    gchar *look_ahead;
    
    node = NULL;
    look_ahead = *iter;

    /* iri - IRIREF */
    iri_node = ags_turtle_load_read_iri(&look_ahead);
    
    if(iri_node != NULL){
#ifdef AGS_DEBUG
      g_message("read rdf-predicate");
#endif
      
      node = xmlNewNode(NULL,
			"rdf-predicate");
      xmlAddChild(node,
		  iri_node);

      *iter = look_ahead;      
    }
    
    return(node);
  }
  
  xmlNode* ags_turtle_load_read_object(gchar **iter){
    xmlNode *node;
    xmlNode *iri_node;
    xmlNode *blank_node;
    xmlNode *collection_node;
    xmlNode *blank_node_property_list_node;
    xmlNode *literal_node;
    
    gchar *look_ahead;
    
    node = NULL;
    iri_node = NULL;
    blank_node = NULL;
    collection_node = NULL;
    blank_node_property_list_node = NULL;
    literal_node = NULL;
    
    look_ahead = *iter;

    /* iri - IRIREF */
    iri_node = ags_turtle_load_read_iri(&look_ahead);

    if(iri_node != NULL){
      goto ags_turtle_load_read_object_CREATE_NODE;
    }

    /* read blank node */
    blank_node = ags_turtle_load_read_blank_node(&look_ahead);

    if(blank_node != NULL){
      goto ags_turtle_load_read_object_CREATE_NODE;
    }

    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    /* collection */
    if(*look_ahead == '('){
      collection_node = ags_turtle_load_read_collection(&look_ahead);
	
      goto ags_turtle_load_read_object_CREATE_NODE;
    }

    /* blank node property listimplemented ags_turtle_load_read_object() */
    if(*look_ahead == '['){
      blank_node_property_list_node = ags_turtle_load_read_blank_node_property_list(&look_ahead);
	
      goto ags_turtle_load_read_object_CREATE_NODE;
    }

    /* literal */
#ifdef AGS_DEBUG
    g_message("*** PASS ***");
#endif
    
    literal_node = ags_turtle_load_read_literal(&look_ahead);
    
    if(literal_node != NULL){
      goto ags_turtle_load_read_object_CREATE_NODE;
    }

    /* create node */
  ags_turtle_load_read_object_CREATE_NODE:

    if(iri_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-object");
      xmlAddChild(node,
		  iri_node);

      *iter = look_ahead;
    }else if(blank_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-object");
      xmlAddChild(node,
		  blank_node);

      *iter = look_ahead;
    }else if(collection_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-object");
      xmlAddChild(node,
		  collection_node);

      *iter = look_ahead;
    }else if(blank_node_property_list_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-object");
      xmlAddChild(node,
		  blank_node_property_list_node);

      *iter = look_ahead;
    }else if(literal_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-object");
      xmlAddChild(node,
		  literal_node);

      *iter = look_ahead;
    }

#ifdef AGS_DEBUG
    if(node != NULL){
      g_message("read rdf-object");
    }
#endif
    
    return(node);
  }

  xmlNode* ags_turtle_load_read_directive(gchar **iter){
    xmlNode *node;
    xmlNode *rdf_prefix_id_node, *rdf_base_node, *rdf_sparql_prefix_node, *rdf_sparql_base_node;
    
    gchar *look_ahead, *current;

    node = NULL;
    look_ahead = *iter;
    
    rdf_prefix_id_node = ags_turtle_load_read_prefix_id(&look_ahead);

    if(rdf_prefix_id_node != NULL){
      goto ags_turtle_load_read_directive_CREATE_NODE;
    }
    
    rdf_base_node = ags_turtle_load_read_base(&look_ahead);

    if(rdf_base_node != NULL){
      goto ags_turtle_load_read_directive_CREATE_NODE;
    }
    
    rdf_sparql_prefix_node = ags_turtle_load_read_sparql_prefix(&look_ahead);

    if(rdf_sparql_prefix_node != NULL){
      goto ags_turtle_load_read_directive_CREATE_NODE;
    }
    
    rdf_sparql_base_node = ags_turtle_load_read_sparql_base(&look_ahead);

    if(rdf_sparql_base_node != NULL){
      goto ags_turtle_load_read_directive_CREATE_NODE;
    }
    
    /* create node */
  ags_turtle_load_read_directive_CREATE_NODE:
    
    if(rdf_prefix_id_node != NULL){
#ifdef AGS_DEBUG
      g_message("read - rdf-directive");
#endif
      
      node = xmlNewNode(NULL,
			"rdf-directive");

      xmlAddChild(node,
		  rdf_prefix_id_node);

      *iter = look_ahead;
    }else if(rdf_base_node != NULL){
#ifdef AGS_DEBUG
      g_message("read - rdf-directive");
#endif
      
      node = xmlNewNode(NULL,
			"rdf-directive");

      xmlAddChild(node,
		  rdf_base_node);

      *iter = look_ahead;
    }else if(rdf_sparql_prefix_node != NULL){
#ifdef AGS_DEBUG
      g_message("read - rdf-directive");
#endif
      
      node = xmlNewNode(NULL,
			"rdf-directive");

      xmlAddChild(node,
		  rdf_sparql_prefix_node);

      *iter = look_ahead;
    }else if(rdf_sparql_base_node != NULL){
#ifdef AGS_DEBUG
      g_message("read - rdf-directive");
#endif
      
      node = xmlNewNode(NULL,
			"rdf-directive");

      xmlAddChild(node,
		  rdf_sparql_base_node);

      *iter = look_ahead;
    }
    
    return(node);
  }

  xmlNode* ags_turtle_load_read_triple(gchar **iter){
    xmlNode *node;
    xmlNode *subject_node,  *predicate_object_list_node_0, *blank_node_object_list_node,  *predicate_object_list_node_1;
    
    gchar *look_ahead, *current;

    node = NULL;
    look_ahead = *iter;
	
    /* subject */
    subject_node = ags_turtle_load_read_subject(&look_ahead);

    /* create triple node */
    if(subject_node != NULL){
      /* first predicate object list */
      predicate_object_list_node_0 = ags_turtle_load_read_predicate_object_list(&look_ahead);

      if(predicate_object_list_node_0 != NULL){
	node = xmlNewNode(NULL,
			  "rdf-triple");
	
	xmlAddChild(node,
		    subject_node);
	xmlAddChild(node,
		    predicate_object_list_node_0);

	/* second predicate object list */
	predicate_object_list_node_1 = ags_turtle_load_read_predicate_object_list(&look_ahead);
	
	if(predicate_object_list_node_1 != NULL){
	  xmlAddChild(node,
		      predicate_object_list_node_1);
	}
	
#ifdef AGS_DEBUG
	g_message("read rdf-triple");
#endif
	
      	*iter = look_ahead;
      }else{
	/* alternate first blank node object list */
	blank_node_object_list_node = ags_turtle_load_read_blank_node_property_list(&look_ahead);

	if(blank_node_object_list_node != NULL){
	  node = xmlNewNode(NULL,
			    "rdf-triple");
	  
	  xmlAddChild(node,
		      blank_node_object_list_node);
	  
	  /* predicate object list */
	  predicate_object_list_node_1 = ags_turtle_load_read_predicate_object_list(&look_ahead);
	
	  if(predicate_object_list_node_1 != NULL){
	    xmlAddChild(node,
			predicate_object_list_node_1);
	  }
	  
#ifdef AGS_DEBUG
	  g_message("read rdf-triple");
#endif
	  
	  *iter = look_ahead;
	}
      }
    }

    return(node);
  }

  xmlNode* ags_turtle_load_read_subject(gchar **iter){
    xmlNode *node;
    xmlNode *iri_node;
    xmlNode *blank_node;
    xmlNode *collection_node;

    gchar *look_ahead;
    
    node = NULL;

    iri_node =  NULL;
    blank_node = NULL;
    collection_node = NULL;
    
    look_ahead = *iter;

    /* iri - IRIREF */
    iri_node = ags_turtle_load_read_iri(&look_ahead);

    if(iri_node != NULL){
      goto ags_turtle_load_read_subject_CREATE_NODE;
    }

    /* read blank node */
    blank_node = ags_turtle_load_read_blank_node(&look_ahead);

    if(blank_node != NULL){
      goto ags_turtle_load_read_subject_CREATE_NODE;
    }
      
    /* collection */
    if(*look_ahead == '('){
      collection_node = ags_turtle_load_read_collection(&look_ahead);
	
      goto ags_turtle_load_read_subject_CREATE_NODE;
    }

    /* create node */
  ags_turtle_load_read_subject_CREATE_NODE:

    if(iri_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-subject");
      xmlAddChild(node,
		  iri_node);

      *iter = look_ahead;
    }else if(blank_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-subject");
      xmlAddChild(node,
		  blank_node);

      *iter = look_ahead;
    }else if(collection_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-subject");
      xmlAddChild(node,
		  collection_node);

      *iter = look_ahead;
    }

#ifdef AGS_DEBUG
    if(node != NULL){
      g_message("read rdf-subject");
    }
#endif
    
    return(node);
  }

  xmlNode* ags_turtle_load_read_object_list(gchar **iter)
  {
    xmlNode *node;
    xmlNode *object_node;
    
    gchar *look_ahead;
    gchar *start_ptr, *end_ptr;
    
    node = NULL;
    look_ahead = *iter;

    /* read first object */
    object_node = ags_turtle_load_read_object(&look_ahead);
    
    if(object_node != NULL){
      /* create node */
      node = xmlNewNode(NULL,
			"rdf-object-list");
      xmlAddChild(node,
		  object_node);

      /* iterate */
      look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);
	
      while(*look_ahead == ','){
	look_ahead++;
	
	object_node = ags_turtle_load_read_object(&look_ahead);

	if(object_node != NULL){
	  xmlAddChild(node,
		      object_node);

	  look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);
	}
      }
      
      *iter = look_ahead;
    }

#ifdef AGS_DEBUG
    if(node != NULL){
      g_message("read rdf-object-list");
    }
#endif
    
    return(node);
  }  
  
  xmlNode* ags_turtle_load_read_collection(gchar **iter)
  {
    xmlNode *node;
    xmlNode *object_node;
    
    gchar *look_ahead;
    gchar *start_ptr, *end_ptr;
    
    node = NULL;
    look_ahead = *iter;

    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);
    
    if(*look_ahead == '\0'){
      return(NULL);
    }

    if(*look_ahead == '('){
      start_ptr = look_ahead;
      look_ahead++;

      /* create node */
      node = xmlNewNode(NULL,
			"rdf-collection");
      
      /* read objects */
      while((object_node = ags_turtle_load_read_object(&look_ahead)) != NULL){
#ifdef AGS_DEBUG
	g_message("read rdf-collection");
#endif
	
	xmlAddChild(node,
		    object_node);
      }

      end_ptr = index(look_ahead,
		      ')');

      *iter = end_ptr + 1;
    }

    return(node);
  }

  xmlNode* ags_turtle_load_read_predicate_object_list(gchar **iter)
  {
    xmlNode *node;
    xmlNode *current_verb_node, *current_object_list_node;

    gchar *look_ahead;
    
    node = NULL;
    look_ahead = *iter;

    current_verb_node = ags_turtle_load_read_verb(&look_ahead);

    if(current_verb_node != NULL){
      current_object_list_node = ags_turtle_load_read_object_list(&look_ahead);

      if(current_object_list_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-predicate-object-list");

	xmlAddChild(node,
		    current_verb_node);
	xmlAddChild(node,
		    current_object_list_node);

	/* iterate */	
	look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);
	
	while(*look_ahead == ';'){
	  look_ahead++;
	  current_verb_node = ags_turtle_load_read_verb(&look_ahead);

	  if(current_verb_node != NULL){
	    current_object_list_node = ags_turtle_load_read_object_list(&look_ahead);
      
	    if(current_object_list_node != NULL){
	      xmlAddChild(node,
			  current_verb_node);
	      xmlAddChild(node,
			  current_object_list_node);
	      
	      look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);
	    }else{
	      break;
	    }
	  }else{
	    break;
	  }
	  
	}

	*iter = look_ahead;
      }
    }
    
    return(node);
  }
  
  xmlNode* ags_turtle_load_read_blank_node_property_list(gchar **iter)
  {
    xmlNode *node;
    xmlNode *predicate_object_list_node;

    gchar *look_ahead;
    gchar *start_ptr, *end_ptr;
    
    node = NULL;
    look_ahead = *iter;

    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    if(*look_ahead == '['){
      start_ptr = look_ahead;
      look_ahead++;
      
      predicate_object_list_node = ags_turtle_load_read_predicate_object_list(&look_ahead);

      if(predicate_object_list_node != NULL){
#ifdef AGS_DEBUG
	g_message("read rdf-blank-node-property-list");
#endif
	
	node = xmlNewNode(NULL,
			  "rdf-blank-node-property-list");

	xmlAddChild(node,
		    predicate_object_list_node);

	end_ptr = index(look_ahead,
			']');
	
	*iter = end_ptr + 1;
      }
    }
    
    return(node);
  }  

#ifdef AGS_DEBUG
  g_message("file: %s", turtle->filename);
#endif
  
  /* entry point - open file and read it */
  sb = (struct stat *) malloc(sizeof(struct stat));
  stat(turtle->filename,
       sb);
  file = fopen(turtle->filename,
	       "r");

  if(file == NULL){
    return(NULL);
  }

  buffer = (gchar *) malloc((sb->st_size + 1) * sizeof(gchar));

  if(buffer == NULL){
    return(NULL);
  }
  
  n_read = fread(buffer, sizeof(gchar), sb->st_size, file);

  if(n_read != sb->st_size){
    g_critical("number of read bytes doesn't match buffer size");
  }
  
  buffer[sb->st_size] = '\0';
  fclose(file);

  /* alloc document */
  turtle->doc = 
    doc = xmlNewDoc("1.0");
  root_node = xmlNewNode(NULL, "rdf-turtle-doc");
  xmlDocSetRootElement(doc, root_node);
  xmlNewProp(root_node,
	     "version",
	     AGS_TURTLE_DEFAULT_VERSION);
  
  iter = buffer;

  do{
    /* skip blanks and comments */
    iter = ags_turtle_load_skip_comments_and_blanks(&iter);

    rdf_statement_node = ags_turtle_load_read_statement(&iter);

    if(rdf_statement_node != NULL){
      xmlAddChild(root_node,
		  rdf_statement_node);
    }

    if(rdf_statement_node == NULL){
      iter++;
    }
  }while(iter < &(buffer[sb->st_size]));
  
  free(sb);
  free(buffer);

  return(doc);
}

/**
 * ags_turtle_new:
 * @filename: the filename as string
 *
 * Instantiate a new #AgsTurtle.
 *
 * Returns: the new instance
 *
 * Since: 0.7.0
 */
AgsTurtle*
ags_turtle_new(gchar *filename)
{
  AgsTurtle *turtle;
  gchar **str;
  
  turtle = g_object_new(AGS_TYPE_TURTLE,
			"filename", filename,
			NULL);

  return(turtle);
}
