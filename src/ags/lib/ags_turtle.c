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
#include <sys/stat.h>
#include <unistd.h>
#include <regex.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>

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
};

static gpointer ags_turtle_parent_class = NULL;

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
					     "AgsTurtle\0",
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
   * Since: 0.4.3
   */
  param_spec = g_param_spec_string("filename\0",
				   "filename of turtle\0",
				   "The filename this turtle is assigned to\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);
}

void
ags_turtle_init(AgsTurtle *turtle)
{
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
    g_value_set_string(value, turtle->filename);
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
 * Since: 0.4.3
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
  
  return(list);
}

gchar*
ags_turtle_read_iriref(gchar *offset,
		       gchar *end_ptr)
{
  regmatch_t match_arr[1];
    
  gchar *str;

  static regex_t iriref_regex;
    
  static gboolean regex_compiled = FALSE;

  static const gchar *iriref_pattern = "(\<[^\x00-\x20]\<\>\"\{\}\|\^`\\].*?<=\>)";

  static const size_t max_matches = 1;
  
  str = NULL;

  if(!regex_compiled){
    regex_compiled = TRUE;
    
    regcomp(&iriref_regex, iriref_pattern, REG_EXTENDED);
  }

  if(regexec(&iriref_regex, offset, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

gchar*
ags_turtle_read_pname_ns(gchar *offset,
			 gchar *end_ptr)
{
  gchar *pn_prefix;
  gchar *str;

  pn_prefix = ags_turtle_read_pn_prefix(offset,
					end_ptr);

  if(pn_prefix != NULL &&
     offset[strlen(pn_prefix)] < end_ptr &&
     offset[strlen(pn_prefix)] == ':'){
    str = g_strdup_printf("%s:\0",
			  pn_prefix);
  }else{
    str = NULL;
  }

  if(pn_prefix != NULL){
    free(pn_prefix);
  }
  
  return(str);
}

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
      str = g_strdup_printf("%s%s\0",
			    pname_ns,
			    pn_local);
    }
  }

  if(pname_ns != NULL){
    free(pname_ns);
  }

  if(pname_local != NULL){
    free(pn_local);
  }
  
  return(str);
}

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

  if(g_str_has_prefix(offset,
		      "_:\0") &&
     ((tmp = ags_turtle_read_pn_chars_u(offset + 2,
					end_ptr) != NULL) ||
      g_ascii_isdigit(offset[2]))){
    if(tmp == NULL){
      str = g_strdup_printf("_:%c\0", offset[2]);
      offset += 3;
    }else{
      str = g_strdup_printf("_:%s\0", tmp);
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
	tmp_str = g_strdup_printf("%s.\0",
				  str);

	free(str);

	str = tmp_str;

	offset++;
	
	found_str = TRUE;
	
	last_is_point = TRUE;
      }
    }else{
      tmp_str = g_strdup_printf("%s%s\0",
				str, tmp);
      offset += strlen(tmp);
      
      free(str);
      free(tmp);

      str = tmp_str;
      
      found_str = TRUE;
    }
  }

  if(last_is_point){
    g_warning("ags_turtle.c - syntax error\0");
  }
  
  return(str);
}

gchar*
ags_turtle_read_langtag(gchar *offset,
			gchar *end_ptr)
{
  gchar *str;

  static const char *langtag_pattern = "(@[a-zA-Z]+(-[a-zA-Z0-9]+))*";
  
  str = NULL;

  //TODO:JK: implement me
  
  return(str);
}

gchar*
ags_turtle_read_boolean(gchar *offset,
			gchar *end_ptr)
{
  regmatch_t match_arr[1];
  
  gchar *str;
  
  static regex_t boolean_literal_regex;

  static gboolean regex_compiled = FALSE;
    
  static const char *boolean_literal_pattern = "^(true|false)\0";

  static const size_t max_matches = 1;
  
  str = NULL;

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    regcomp(&boolean_literal_regex, boolean_literal_pattern, REG_EXTENDED);
  }

  if(regexec(&boolean_literal_regex, literal, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

gchar*
ags_turtle_read_integer(gchar *offset,
			gchar *end_ptr)
{
  regmatch_t match_arr[1];
  
  gchar *str;
  
  static regex_t integer_literal_regex;

  static gboolean regex_compiled = FALSE;
    
  static const char *integer_literal_pattern = "^([+-]?[0-9]+)\0";

  static const size_t max_matches = 1;
  
  str = NULL;

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    regcomp(&integer_literal_regex, integer_literal_pattern, REG_EXTENDED);
  }

  if(regexec(&integer_literal_regex, literal, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

gchar*
ags_turtle_read_decimal(gchar *offset,
			gchar *end_ptr)
{
  regmatch_t match_arr[1];
  
  gchar *str;

  static regex_t decimal_literal_regex;

  static gboolean regex_compiled = FALSE;

  static const char *decimal_literal_pattern = "^([+-]?[0-9]*.[0-9]+)\0";

  static const size_t max_matches = 1;
    
  str = NULL;

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    regcomp(&decimal_literal_regex, decimal_literal_pattern, REG_EXTENDED);
  }

  if(regexec(&decimal_literal_regex, literal, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

gchar*
ags_turtle_read_double(gchar *offset,
		       gchar *end_ptr)
{
  regmatch_t match_arr[1];
  
  gchar *str;

  static regex_t double_literal_regex;
  
  static gboolean regex_compiled = FALSE;
  
  static const char *double_literal_pattern = "^([+-]?([0-9]+.[0-9]*[eE] [+-]?[0-9]+)|(.[0-9]+[eE][+-]?[0-9]+)|([0-9]+[eE][+-]?[0-9]+))\0";

  static const size_t max_matches = 1;
    
  str = NULL;

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    regcomp(&double_literal_regex, double_literal_pattern, REG_EXTENDED);
  }

  if(regexec(&double_literal_regex, literal, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

gchar*
ags_turtle_read_exponent(gchar *offset,
			 gchar *end_ptr)
{
  regmatch_t match_arr[1];
  
  gchar *str;

  static regex_t exponent_literal_regex;

  static gboolean regex_compiled = FALSE;
  
  static const char *exponent_literal_pattern = "^([eE][+-]?[0-9]+)\0";

  static const size_t max_matches = 1;
    
  str = NULL;

  if(!regex_compiled){
    regex_compiled = TRUE;

    regcomp(&exponent_literal_regex, exponent_literal_pattern, REG_EXTENDED);
  }

  if(regexec(&exponent_literal_regex, literal, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

gchar*
ags_turtle_read_string(gchar *offset,
		       gchar *end_ptr)
{
  gchar *str;

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

  str = ags_turtle_read_string_literal_long_quote(offset,
						  end_ptr);
  
  if(str != NULL){
    return(str);
  }

  str = ags_turtle_read_string_literal_long_single:quote(offset,
							 end_ptr);
  
  if(str != NULL){
    return(str);
  }

  return(NULL);
}

gchar*
ags_turtle_read_string_literal_quote(gchar *offset,
				     gchar *end_ptr)
{
  regmatch_t match_arr[1];
  
  gchar *str;

  static regex_t string_literal_double_quote_regex;

  static gboolean regex_compiled = FALSE;
  
  static const char *string_literal_double_quote_pattern = "^(\"([^\"\|\n\r]|.*)(?<=\"))\0";

  static const size_t max_matches = 1;
    
  str = NULL;
  
  if(!regex_compiled){
    regex_compiled = TRUE;
      
    regcomp(&string_literal_single_quote_regex, string_literal_single_quote_pattern, REG_EXTENDED);
  }

  if(regexec(&string_literal_double_quote_regex, literal, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

gchar*
ags_turtle_read_string_literal_single_quote(gchar *offset,
					    gchar *end_ptr)
{
  regmatch_t match_arr[1];
  
  gchar *str;

  static regex_t string_literal_single_quote_regex;

  static gboolean regex_compiled = FALSE;
  
  static const char *string_literal_single_quote_pattern = "^('([^'\|\n\r]|.*)(?<='))\0";

  static const size_t max_matches = 1;
    
  str = NULL;

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    regcomp(&string_literal_single_quote_regex, string_literal_single_quote_pattern, REG_EXTENDED);
  }

  if(regexec(&string_literal_single_quote_regex, literal, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

gchar*
ags_turtle_read_string_literal_long_quote(gchar *offset,
					  gchar *end_ptr)
{
  regmatch_t match_arr[1];
  
  gchar *str;

  static regex_t string_literal_long_double_quote_regex;
  
  static gboolean regex_compiled = FALSE;
  
  static const char *string_literal_long_double_quote_pattern = "^(\"\"\"(((\")|(\"\"))?([^\"\\]|.*))(?<=\"\"\"))\0";

  static const size_t max_matches = 1;
    
  str = NULL;

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    regcomp(&string_literal_long_double_quote_regex, string_literal_long_double_quote_pattern, REG_EXTENDED);
  }

  if(regexec(&string_literal_long_double_quote_regex, literal, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

gchar*
ags_turtle_read_string_literal_long_single_quote(gchar *offset,
						 gchar *end_ptr)
{
  regmatch_t match_arr[1];
  
  gchar *str;

  static regex_t string_literal_long_single_quote_regex;
  
  static gboolean regex_compiled = FALSE;

  static const char *string_literal_long_single_quote_pattern = "^('''(((')|(''))?([^'\\]|.*))(?<='''))\0";

  static const size_t max_matches = 1;
    
  str = NULL;

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    regcomp(&string_literal_long_single_quote_regex, string_literal_long_single_quote_pattern, REG_EXTENDED);
  }

  if(regexec(&string_literal_long_single_quote_regex, literal, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

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

  if(!g_ascii_strncasecmp(offset,
			  "\\u\0",
			  2)){
    hex_digit_count = 4;
  }else if(!g_ascii_strncasecmp(offset,
				"\\U\0",
				2)){
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

gchar*
ags_turtle_read_echar(gchar *offset,
		      gchar *end_ptr)
{
  gchar *str;

  static const gchar *echar = "tbnrf\"\0";
  
  if(offset + 1 >= end_ptr){
    return(NULL);
  }
  
  str = NULL;

  if(offset == '\\'){
    if(index(echar,
	     offset[1]) != NULL){
      str = g_strndup(offset,
		      2);
    }
  }

  return(str);
}

gchar*
ags_turtle_read_ws(gchar *offset,
		   gchar *end_ptr)
{
  gchar *str;

  static const gchar *ws = "\x20\x09\x0D\x0A\0";

  str = NULL;

  if(offset < end_ptr &&
     index(ws, *offset) != NULL){
    str = g_strndup(offset,
		    1);
  }
  
  return(str);
}

gchar*
ags_turtle_read_anon(gchar *offset,
		     gchar *end_ptr)
{
  gchar *str;
  gchar *tmp, *tmp_str;

  gboolean success;
  
  static const gchar *ws = "\x20\x09\x0D\x0A\0";

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
		    tmp - offset);
  }
  
  return(str);
}

gchar*
ags_turtle_read_pn_chars_base(gchar *offset,
			      gchar *end_ptr)
{
  regmatch_t match_arr[1];
    
  gchar *str;
  
  static regex_t chars_base_regex;
    
  static gboolean regex_compiled = FALSE;

  static const char *chars_base_pattern = "^([A-Xa-x]|\x00[\xC0-\xD6]|\x00[\xD8-\xF6]|\x00[\xF8-\xFF]|\x01[\x00-\xFF]|\x02[\x00-\xFF]|\x03[\x70-\x7D]|\x20[\x0C-\x0D]|\x20[\x70-\xFF]|\x21[\x00-\x8F]|[\x2C-\x2E][\x00-\xFF]|\x2F[\x00-\xEF]|\x30[\x01-\xFF]|[\x31-\xD7][\x00-\xFF]|[\xF9-\xFC][\x00-\xFF]|[\xFD[\x00-\xCF]|\xFD[\xF0-\xFF]|\x0FE[\x00-\xFF]|\xFF[\x00-\xFD]|[\x01-0x0E][\x00-\xFF][\x00-\xFF])\0";

  static const size_t max_matches = 1;

  str = NULL;

  if(!regex_compiled){
    regex_compiled = TRUE;
    
    regcomp(&chars_base_regex, chars_base_pattern, REG_EXTENDED);
  }

  if(regexec(&chars_base_regex, offset, max_matches, match_arr, 0) == 0){
    str = g_strndup(offset,
		    match_arr[0].rm_eo - match_arr[0].rm_so);
  }
  
  return(str);
}

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
    str = g_strdup("_\0");
  }
  
  return(str);
}

gchar*
ags_turtle_read_pn_chars(gchar *offset,
			 gchar *end_ptr)
{
  regmatch_t match_arr[1];
    
  gchar *str;
  
  static regex_t chars_regex;
    
  static gboolean regex_compiled = FALSE;

  static const char *chars_pattern = "^([0-9]|\x00\0xB7|#x03[\x00-\x6F]|\x20[\x3F-\x40])\0";

  static const size_t max_matches = 1;

  str = ags_turtle_read_pn_chars_u(offset,
				   end_ptr);

  if(str == NULL &&
     offset < end_ptr &&
     *offset == '-'){
    str = g_strdup("-\0");
  }

  if(str == NULL &&
     offset < end_ptr){
    if(!regex_compiled){
      regex_compiled = TRUE;
    
      regcomp(&chars_regex, chars_pattern, REG_EXTENDED);
    }

    if(regexec(&chars_regex, offset, max_matches, match_arr, 0) == 0){
      str = g_strndup(offset,
		      match_arr[0].rm_eo - match_arr[0].rm_so);
    }
  }
  
  return(str);
}

gchar*
ags_turtle_read_pn_prefix(gchar *offset,
			  gchar *end_ptr)
{
  gchar *str;
  gchar *tmp, *str_tmp;

  gboolean last_is_point;
  
  str = ags_turtle_read_pn_chars_base(offset,
				      end_ptr);
  
  if(str != NULL){
    offset += strlen(str);
    last_is_point = FALSE;
    
    while((tmp = ags_turtle_read_pn_chars(offset,
					  end_ptr)) != NULL ||
	  *offset == '.'){
      if(tmp == NULL){
	tmp = g_strdup(".\0");
	
	last_is_point = TRUE;
      }else{
	last_is_point = FALSE;
      }

      if(str != NULL){
	str_tmp = g_strdup_printf("%s%s\0",
				  str, tmp);
	offset += strlen(tmp);

	free(str);
	free(tmp);

	str = str_tmp;
      }else{
	str = tmp;
	offset += strlen(tmp);
      }
    }

    if(last_is_point){
      g_warning("ags_turtle.c - syntax error\0");
    }
  }
  
  return(str);
}

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
    tmp = g_strdup(":\0");
  }

  if(tmp == NULL &&
     g_ascii_is_digit(*offset)){
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
	tmp = g_strdup(".\0");
	
	last_is_point = TRUE;
      }

      if(tmp == NULL &&
	 *offset == ':'){
	tmp = g_strdup(":\0");	
      }

      if(tmp == NULL){
	tmp = ag_turtle_read_plx(offset,
				 end_ptr);
      }
    
      /* concat new strings */
      if(tmp != NULL){
	found_str = TRUE;

	if(str != NULL){
	  tmp_str = g_strdup_printf("%s%s\0",
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

    if(last_is_point){
      g_warning("ags_turtle.c - syntax error\0");
    }
  }
  
  return(str);
}

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

gchar*
ags_turtle_read_hex(gchar *offset,
		    gchar *end_ptr)
{
  gchar *str;

  str = NULL;

  if(g_ascii_isxdigit(offset)){
    str = g_strndup(offset,
		    1);
  }
  
  return(str);
}

gchar*
ags_turtle_read_pn_local_esc(gchar *offset,
			     gchar *end_ptr)
{
  static const gchar *escapes = "_~.-!$&'()*+,;=/?#@%\0";
  gchar *str;

  str = NULL;

  if(offset != NULL &&
     offset + 1 < end_ptr &&
     *offset == '\\' &&
     index(escapes,
	   offset[1]) != NULL){
    str = g_strndup(offset,
		    2);
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
 * Since: 0.4.3
 */
void
ags_turtle_load(AgsTurtle *turtle,
		GError **error)
{
  xmlDoc *doc;
  xmlNode *root_node, *node;
  FILE *file;
  
  gchar *buffer, *iter;
  
  struct stat *sb;

  auto gchar* ags_turtle_load_skip_comments_and_blanks(gchar **iter);
  auto xmlNode* ags_turtle_load_read_iriref(gchar **iter);
  auto xmlNode* ags_turtle_load_read_anon(gchar **iter);
  auto xmlNode* ags_turtle_load_read_pname_ns(gchar **iter);
  auto xmlNode* ags_turtle_load_read_pname_nl(gchar **iter);
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

    look_ahead = *iter;
    
    /* skip whitespaces and comments */
    for(; (look_ahead < &(buffer[sb->st_size])) && *look_ahead != '\0'; look_ahead++){
      /* skip comments */
      if(buffer == look_ahead){
	if(*buffer == '#'){
	  look_ahead = index(look_ahead, '\n');
	  continue;
	}
      }else if(buffer[look_ahead - buffer - 1] == '\n' && *look_ahead == '#'){
	look_ahead = index(look_ahead, '\n');
	continue;
      }

      /* spaces */
      if(*look_ahead != ' ' && *look_ahead != '\t' && *look_ahead != '\n'){
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
    str = ags_turtle_read_iriref(iter,
				 &(buffer[sb->st_size]));

    if(str != NULL){
      node = xmlNewNode(NULL,
			"rdf-iriref\0");
      xmlNodeSetContent(node,
			str);

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
    str = ags_turtle_read_anon(iter,
			       &(buffer[sb->st_size]));

    if(str != NULL){
      node = xmlNewNode(NULL,
			"rdf-anon\0");
      xmlNodeSetContent(node,
			str);
      
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
    str = ags_turtle_read_pname_ns(iter,
				   &(buffer[sb->st_size]));
    
    if(str != NULL){
      node = xmlNewNode(NULL,
			"rdf-pname-ns\0");
      xmlNodeSetContent(node,
			str);

      *iter = look_ahead + strlen(str);
    }
    
    return(node);
  }
  
  xmlNode* ags_turtle_load_read_pname_nl(gchar **iter){
    xmlNode *node;

    gchar *look_ahead;
    gchar *str;
    
    node = NULL;
    look_ahead = *iter;
    
    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    /* read pname-nl */
    str = ags_turtle_read_pname_nl(iter,
				   &(buffer[sb->st_size]));
    
    if(str != NULL){
      node = xmlNewNode(NULL,
			"rdf-pname-nl\0");
      xmlNodeSetContent(node,
			str);

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
    str = ags_turtle_read_string(iter,
				 &(buffer[sb->st_size]));
    
    if(str != NULL){
      node = xmlNewNode(NULL,
			"rdf-string\0");
      xmlNodeSetContent(node,
			str);

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
    str = ags_turtle_read_langtag(iter,
				  &(buffer[sb->st_size]));
    
    if(str != NULL){
      node = xmlNewNode(NULL,
			"rdf-langtag\0");
      xmlNodeSetContent(node,
			str);

      *iter = look_ahead + strlen(str);
    }
    
    return(node);
  }

  xmlNode* ags_turtle_load_read_literal(gchar **iter){
    xmlNode *node;
    xmlNode *rdf_string_node, *rdf_langtag_node, *rdf_iri_node;
    
    gchar *look_ahead;
    
    node = NULL;
    look_ahead; = *iter;

    /* read string */
    rdf_string_node = ags_turtle_load_read_string(&look_ahead);

    if(rdf_string_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-literal\0");

      /* read optional langtag */
      rdf_lang_node = ags_turtle_load_read_langtag(&look_ahead);

      if(rdf_lang_node != NULL){
	xmlAddChild(node,
		    rdf_lang_node);
      }else{
	/* alternate read optional iri */
	rdf_iri_node = ags_turtle_load_read_iri(&look_ahead);
	
	if(rdf_iri_node != NULL){
	  xmlAddChild(node,
		      rdf_iri_node);
	  
	}
      }
      
      *iter = look_ahead;
    }
    
    return(node);
  }

  xmlNode* ags_turtle_load_read_iri(gchar **iter){
    xmlNode *node;
    xmlNode *rdf_iriref_node, *rdf_prefixed_name_node;
    
    gchar *look_ahead;
    
    node = NULL;
    pname_node = NULL;

    node = NULL;
    look_ahead; = *iter;

    /* read iriref */
    rdf_iriref_node = ags_turtle_load_read_iriref(&look_ahead);

    if(rdf_iriref_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-iri\0");
      xmlAddChild(node,
		  rdf_iriref_node);
    }else{
      /* alternate read prefixed name */
      rdf_prefixed_name_node = ags_turtle_load_read_prefixed_nametag(&look_ahead);

      if(rdf_prefixed_name_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-iri\0");
	xmlAddChild(node,
		    rdf_prefixed_name_node);
      }
      
      *iter = look_ahead;
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

    static const char *prefix_id_pattern = "^(\@prefix)\0";
    
    static const size_t max_matches = 1;

    node = NULL;
    look_ahead = *iter;

    if(!regex_compiled){
      regex_compiled = TRUE;
      
      regcomp(&prefix_id_regex, prefix_id_pattern, REG_EXTENDED);
    }

    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    /* match @prefix */
    if(regexec(&prefix_id_regex, look_ahead, max_matches, match_arr, 0) == 0){
      rdf_pname_ns_node = ags_turtle_load_read_pname_ns(&look_ahead);
      
      rdf_iriref_node = ags_turtle_load_read_iriref(&look_ahead);;

      /* create node if complete prefix id */
      if(rdf_pname_ns_node != NULL &&
	 rdf_iriref_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-prefix-id\0");
	xmlAddChild(node,
		    rdf_pname_ns_node);
	xmlAddChild(node,
		    rdf_iriref_node);
	
	*iter = look_ahead + (match_arr[0].rm_eo - match_arr[0].rm_so);
      }
    }
    
    return(node);
  }
  
  xmlNode* ags_turtle_load_read_base(gchar **iter){
    xmlNode *node;
    xmlNode *rdf_iriref_node;

    regmatch_t match_arr[1];
    
    static regex_t base_regex;

    static gboolean regex_compiled = FALSE;

    static const char *base_pattern = "^(\@base)\0";
    
    static const size_t max_matches = 1;

    node = NULL;
    look_ahead = *iter;

    if(!regex_compiled){
      regex_compiled = TRUE;
      
      regcomp(&base_regex, base_pattern, REG_EXTENDED);      
    }

    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    /* match @base */
    if(regexec(&base_regex, look_ahead, max_matches, match_arr, 0) == 0){
      rdf_iriref_node = ags_turtle_load_read_iriref(&look_ahead);;

      /* create node if complete base */
      if(rdf_iriref_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-base\0");
	xmlAddChild(node,
		    rdf_iriref_node);
	
	*iter = look_ahead + (match_arr[0].rm_eo - match_arr[0].rm_so);
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

    static const char *sparql_prefix_pattern = "^(\"PREFIX\")\0";
    
    static const size_t max_matches = 1;

    node = NULL;
    look_ahead = *iter;

    if(!regex_compiled){
      regex_compiled = TRUE;
      
      regcomp(&sparql_prefix_regex, sparql_prefix_pattern, REG_EXTENDED);
    }

    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    /* match @prefix */
    if(regexec(&sparql_prefix_regex, look_ahead, max_matches, match_arr, 0) == 0){
      rdf_pname_ns_node = ags_turtle_load_read_pname_ns(&look_ahead);
      
      rdf_iriref_node = ags_turtle_load_read_iriref(&look_ahead);;

      /* create node if complete sparql prefix */
      if(rdf_pname_ns_node != NULL &&
	 rdf_iriref_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-sparql-prefix\0");
	xmlAddChild(node,
		    rdf_pname_ns_node);
	xmlAddChild(node,
		    rdf_iriref_node);
	
	*iter = look_ahead + (match_arr[0].rm_eo - match_arr[0].rm_so);
      }
    }
    
    return(node);
  }
  
  xmlNode* ags_turtle_load_read_sparql_base(gchar **iter){
    xmlNode *node;
    xmlNode *rdf_iriref_node;

    regmatch_t match_arr[1];
    
    static regex_t sparql_base_regex;

    static gboolean regex_compiled = FALSE;

    static const char *sparql_base_pattern = "^(\"BASE\")\0";
    
    static const size_t max_matches = 1;

    node = NULL;
    look_ahead = *iter;

    if(!regex_compiled){
      regex_compiled = TRUE;
      
      regcomp(&sparql_base_regex, sparql_base_pattern, REG_EXTENDED);      
    }

    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    /* match @base */
    if(regexec(&sparql_base_regex, look_ahead, max_matches, match_arr, 0) == 0){
      rdf_iriref_node = ags_turtle_load_read_iriref(&look_ahead);;

      /* create node if complete sparqle base */
      if(rdf_iriref_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-sparql-base\0");
	xmlAddChild(node,
		    rdf_iriref_node);
	
	*iter = look_ahead + (match_arr[0].rm_eo - match_arr[0].rm_so);
      }
    }

    return(node);
  }
  
  xmlNode* ags_turtle_load_read_prefixed_name(gchar **iter){
    xmlNode *node;
    xmlNode *rdf_pname_ns_node, *rdf_pname_nl_node;

    gchar *look_ahead;

    node = NULL;
    look_ahead = *iter;

    rdf_pname_nl_node = ags_turtle_load_read_pname_nl(&look_ahead);

    if(rdf_pname_nl_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-prefixed-name\0");
      xmlAddChild(node,
		  rdf_pname_nl_node);		  

      *iter = look_ahead;
    }else{
      rdf_pname_ns_node = ags_turtle_load_read_pname_ns(&look_ahead);

      if(rdf_pname_ns_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-prefixed-name\0");
	xmlAddChild(node,
		    rdf_pname_ns_node);

	*iter = look_ahead;
      }
    }

    return(node);
  }

  xmlNode* ags_turtle_load_read_blank_node(gchar **iter){
    xmlNode *node;
    xmlNode *rdf_blank_node_label_node, *rdf_anon_node;
    
    gchar *look_ahead;
    
    node = NULL;
    look_ahead = *iter;    

    rdf_blank_node_label_node = ags_turtle_load_read_blank_node_label(&look_ahead);

    if(rdf_blank_node_label_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-blank-node\0");
      xmlAddChild(node,
		  rdf_blank_node_label_node);		  

      *iter = look_ahead;
    }else{
      rdf_anon_node = ags_turtle_load_read_anon(&look_ahead);

      if(rdf_anon_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-blank-node\0");
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
			"rdf-statement\0");
      xmlAddChild(node,
		  rdf_directive_node);		  

      *iter = look_ahead;
    }else{
      rdf_triple_node = ags_turtle_load_read_triple(&look_ahead);

      if(rdf_triple_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-statement\0");
	xmlAddChild(node,
		    rdf_triple_node);

	*iter = look_ahead;
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
      g_message("read rdf-verb\0");
	
      node = xmlNewNode(NULL,
			"rdf-verb\0");
	  
      xmlAddChild(node,
		  predicate_node);
	
      *iter = look_ahead;
    }else{
      /* skip blanks and comments */
      look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);
      
      if(*look_ahead == 'a'){
	g_message("read - verb: a\0");

	node = xmlNewNode(NULL,
			  "rdf-verb\0");
      
	/* verb - a */
	xmlNewProp(node,
		   "verb\0",
		   "a\0");

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
      g_message("read rdf-predicate\0");

      node = xmlNewNode(NULL,
			"rdf-predicate\0");
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
    g_message("*** PASS ***");

    literal_node = ags_turtle_load_read_literal(&look_ahead);
    
    if(literal_node != NULL){
      goto ags_turtle_load_read_object_CREATE_NODE;
    }

    /* create node */
  ags_turtle_load_read_object_CREATE_NODE:

    if(iri_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-object\0");
      xmlAddChild(node,
		  iri_node);

      *iter = look_ahead;
    }else if(blank_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-object\0");
      xmlAddChild(node,
		  blank_node);

      *iter = look_ahead;
    }else if(collection_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-object\0");
      xmlAddChild(node,
		  collection_node);

      *iter = look_ahead;
    }else if(blank_node_property_list_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-object\0");
      xmlAddChild(node,
		  blank_node_property_list_node);

      *iter = look_ahead;
    }else if(literal_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-object\0");
      xmlAddChild(node,
		  literal_node);

      *iter = look_ahead;
    }

    if(node != NULL){
      g_message("read rdf-object\0");
    }
    
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
    
    rdf_base_node = ags_turtle_load_read_base_node(&look_ahead);

    if(rdf_base_node != NULL){
      goto ags_turtle_load_read_directive_CREATE_NODE;
    }
    
    rdf_sparql_prefix_node = ags_turtle_read_sparql_prefix(&look_ahead);

    if(rdf_sparql_prefix_node != NULL){
      goto ags_turtle_load_read_directive_CREATE_NODE;
    }
    
    rdf_sparql_base_node = asg_turtle_read_sparql_base(&look_ahead);

    if(rdf_sparql_base_node != NULL){
      goto ags_turtle_load_read_directive_CREATE_NODE;
    }
    
    /* create node */
    if(rdf_prefix_id_node != NULL){
      g_message("read - rdf-directive\0");
      
      node = xmlNewNode(NULL,
			"rdf-directive\0");

      xmlAddChild(node,
		  rdf_prefix_id_node);

      *iter = look_ahead;
    }else if(rdf_base_node != NULL){
      g_message("read - rdf-directive\0");
      
      node = xmlNewNode(NULL,
			"rdf-directive\0");

      xmlAddChild(node,
		  rdf_base_node);

      *iter = look_ahead;
    }else if(rdf_sparql_prefix_node != NULL){
      g_message("read - rdf-directive\0");
      
      node = xmlNewNode(NULL,
			"rdf-directive\0");

      xmlAddChild(node,
		  rdf_sparql_prefix_node);

      *iter = look_ahead;
    }else if(rdf_spqarl_base_node != NULL){
      g_message("read - rdf-directive\0");
      
      node = xmlNewNode(NULL,
			"rdf-directive\0");

      xmlAddChild(node,
		  rdf_sparql_base_node);

      *iter = look_ahead;
    }
    
    return(node);
  }

  xmlNode* ags_turtle_load_read_triple(gchar **iter){
    xmlNode *node;
    xmlNode *subject_node;
    xmlNode *blank_node_object_list_node;
    xmlNode *predicate_object_list_node;
    
    gchar *look_ahead, *current;

    node = NULL;
    subject_node = NULL;
    blank_node_object_list_node = NULL;
    predicate_object_list_node = NULL;
    
    look_ahead = *iter;

    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    if(*look_ahead == '\0'){
      return(NULL);
    }
	
    /* subject */
    subject_node = ags_turtle_load_read_subject(&look_ahead);

    /* create triple node */
    if(subject_node != NULL){
      g_message("read rdf-triple\0");

      node = xmlNewNode(NULL,
			"rdf-triple\0");
      xmlAddChild(node,
		  subject_node);

      *iter = look_ahead;
    }

    /* predicate object list or blank property list */    
    if(node != NULL){
      predicate_object_list_node = ags_turtle_load_read_predicate_object_list(&look_ahead);

      if(predicate_object_list_node != NULL){
	xmlAddChild(node,
		    predicate_object_list_node);

	*iter = look_ahead;
      }else{
	if((look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead)) == '\0'){
	  return(NULL);
	}
	
	if(look_ahead == '['){
	  blank_node_object_list_node = ags_turtle_load_read_blank_node_property_list(&look_ahead);

	  while(*look_ahead != ']') look_ahead++;

	  xmlAddChild(node,
		      blank_node_object_list_node);
	  
	  *iter = look_ahead;
	}
      }

      /* predicate object list */
      predicate_object_list_node = ags_turtle_load_read_predicate_object_list(&look_ahead);

      if(predicate_object_list_node != NULL){
	xmlAddChild(node,
		    predicate_object_list_node);
	
	*iter = look_ahead;
      }
    }

    return(node);
  }

    xmlNode* ags_turtle_load_read_subject(gchar **iter){
    xmlNode *node;
    xmlNode *iri_node;
    xmlNode *pname_node;
    xmlNode *blank_node;
    xmlNode *collection_node;

    gchar *look_ahead;
    
    node = NULL;
    iri_node = NULL;
    pname_node = NULL;
    blank_node = NULL;
    collection_node = NULL;
    
    look_ahead = *iter;

    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    if(*look_ahead == '\0'){
      return(NULL);
    }

    /* iri - IRIREF */
    iri_node = ags_turtle_load_read_iri(&look_ahead);

    if(iri_node != NULL){
      goto ags_turtle_load_read_subject_CREATE_NODE;
    }

    /* read pname ln or ns */
    pname_node = ags_turtle_load_read_pname(&look_ahead);
      
    if(pname_node != NULL){
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
			"rdf-subject\0");
      xmlAddChild(node,
		  iri_node);

      *iter = look_ahead;
    }else if(pname_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-subject\0");
      xmlAddChild(node,
		  pname_node);

      *iter = look_ahead;
    }else if(blank_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-subject\0");
      xmlAddChild(node,
		  blank_node);

      *iter = look_ahead;
    }else if(collection_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-subject\0");
      xmlAddChild(node,
		  collection_node);

      *iter = look_ahead;
    }

    if(node != NULL){
      g_message("read rdf-subject\0");
    }
    
    return(node);
  }

  xmlNode* ags_turtle_load_read_object_list(gchar **iter)
  {
    xmlNode *node;
    xmlNode *current_object_node;

    gchar *look_ahead;
    
    node = NULL;
    current_object_node = NULL;

    look_ahead = *iter;

    while((look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead)) != '\0'){
      if(*look_ahead == ','){
	look_ahead++;
	continue;
      }

      current_object_node = ags_turtle_load_read_object(&look_ahead);

      if(current_object_node != NULL){
	if(node == NULL){
	  g_message("read rdf-object-list\0");
	  
	  node = xmlNewNode(NULL,
			    "rdf-object-list\0");
	}

	xmlAddChild(node,
		    current_object_node);

	*iter = look_ahead;
      }else{
	break;
      }
    }
    
    return(node);
  }  
  
  xmlNode* ags_turtle_load_read_collection(gchar **iter)
  {
    xmlNode *node;
    xmlNode *object_node;
    
    gchar *look_ahead, *current;

    node = xmlNewNode(NULL,
		      "rdf-collection\0");
    object_node = NULL;
    
    look_ahead = *iter;

    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);
    
    if(*look_ahead == '\0'){
      return(NULL);
    }

    if(look_ahead == '('){
      look_ahead++;

      g_message("read rdf-collection\0");

      while((look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead)) != '\0' &&
	    *look_ahead != ')'){
	current = look_ahead;

	/* literal */
	object_node = ags_turtle_load_read_object(&look_ahead);
	  
	if(object_node != NULL){
	  xmlAddChild(node,
		      object_node);
	}else{
	  if(*look_ahead == ')'){
	    break;
	  }
	  
	  iter = look_ahead++;
	  
	  g_warning("ags_turtle_load.c - unrecognized token");
	}
      }
      
      *iter = look_ahead + 1;
    }

    return(node);
  }

  xmlNode* ags_turtle_load_read_predicate_object_list(gchar **iter)
  {
    xmlNode *node;
    xmlNode *current_verb_node, *current_object_list_node;

    gchar *look_ahead;
    
    node = NULL;
    current_verb_node = NULL;
    current_object_list_node = NULL;
    
    look_ahead = *iter;

    while((look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead)) != '\0'){
      current_verb_node = ags_turtle_load_read_verb(&look_ahead);

      if(current_verb_node != NULL){
	g_message("read rdf-predicate-object-list\0");
	
	node = xmlNewNode(NULL,
			  "rdf-predicate-object-list\0");
	xmlAddChild(node,
		    current_verb_node);
	
	/* object list */
	current_object_list_node = NULL;
	current_object_list_node = ags_turtle_load_read_object_list(&look_ahead);

	if(current_object_list_node != NULL){
	  xmlAddChild(node,
		      current_object_list_node);

	  *iter == look_ahead;
	}
      }else{
	break;
      }

      /* iterate */
      look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);
    
      if(*look_ahead == ';'){
	look_ahead++;
      }else{	
	break;
      }
    }

    if(node != NULL){
      *iter = look_ahead;
    
      if(*look_ahead == '.'){
	*iter = look_ahead + 1;
      }
    }

    return(node);
  }
  
  xmlNode* ags_turtle_load_read_blank_node_property_list(gchar **iter)
  {
    xmlNode *node;
    xmlNode *predicate_object_list_node;

    gchar *look_ahead;
    
    node = NULL;
    predicate_object_list_node = NULL;
    
    look_ahead = *iter;
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    if(*look_ahead == '\0'){
      return(NULL);
    }

    if(*look_ahead == '['){
      look_ahead++;

      predicate_object_list_node = ags_turtle_load_read_predicate_object_list(&look_ahead);

      if(predicate_object_list_node != NULL){
	g_message("read rdf-blank-node-property-list\0");
	
	node = xmlNewNode(NULL,
			  "rdf-blank-node-property-list\0");

	xmlAddChild(node,
		    predicate_object_list_node);
	
	*iter = look_ahead;
      }
    }
    
    return(node);
  }  

  g_message("file: %s\0", turtle->filename);
  
  /* entry point - open file and read it */
  sb = (struct stat *) malloc(sizeof(struct stat));
  stat(turtle->filename,
       sb);
  file = fopen(turtle->filename,
	       "r\0");
  
  buffer = (gchar *) malloc((sb->st_size + 1) * sizeof(gchar));

  if(buffer == NULL){
    return;
  }
  
  fread(buffer, sizeof(gchar), sb->st_size, file);
  buffer[sb->st_size] = '\0';
  fclose(file);

  /* alloc document */
  turtle->doc = 
    doc = xmlNewDoc("1.0\0");
  root_node = xmlNewNode(NULL, "rdf-turtle-doc\0");
  xmlDocSetRootElement(doc, root_node);
  xmlNewProp(node,
	     "version\0",
	     AGS_TURTLE_DEFAULT_VERSION);
  
  iter = buffer;

  do{
    node = ags_turtle_load_read_statement(&iter);

    if(node != NULL){
      xmlAddChild(root_node,
		  node);
    }
  }while(node != NULL && iter < &(buffer[sb->st_size]));
  
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
 * Since: 0.4.3
 */
AgsTurtle*
ags_turtle_new(gchar *filename)
{
  AgsTurtle *turtle;
  gchar **str;
  
  turtle = g_object_new(AGS_TYPE_TURTLE,
			"filename\0", filename,
			NULL);

  return(turtle);
}
