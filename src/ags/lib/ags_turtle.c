/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#define UTF8_2_BYTE_RANGE(str, endptr, lower, upper) (str + 1 < endptr && \
						      (((int) ((char *) str)[0] << 8) | ((int) ((char *) str)[1])) >= (int) ((lower[0] << 8) | (lower[1])) && \
						      (((int) ((char *) str)[0] << 8) | ((int) ((char *) str)[1])) < (int) ((upper[0] << 8) | (upper[1])))
#define UTF8_3_BYTE_RANGE(str, endptr, lower, upper) (str + 2 < endptr && \
						       (((int) ((char *) str)[0] << 16) | ((int) ((char *) str)[1] << 8) | ((int) ((char *) str)[2])) >= (int) ((lower[0] << 16) | (lower[1] << 8) | (lower[2])) && \
						       (((int) ((char *) str)[0] << 16) | ((int) ((char *) str)[1] << 8) | ((int) ((char *) str)[2])) < (int) ((upper[0] << 16) | (upper[1] << 8) | (upper[2])))

#define rdf_is_iriref(str, endptr) (rdf_is_uchar(str, endptr) && \
				    !(*str >= '\x00' && *str <= '\x20') && \
				    *str != '<' && \
				    *str != '>' && \
				    *str != '"' && \
				    *str != '{' && \
				    *str != '}' && \
				    *str != '^' && \
				    *str != '`' && \
				    *str != '\\')
#define rdf_is_iriref_extended_valid_start(str, endptr) (*str == '<')
#define rdf_is_iriref_extended_valid_end(str, endptr) (*str == '>')
#define rdf_is_pname_ns(str, endptr) (rdf_is_pn_prefix(str, endptr) || \
				      *str == ':')
#define rdf_is_pname_ln(str, endptr) (rdf_is_pname_ns(str, endptr) || \
				      rdf_is_pn_local(str, endptr))
#define rdf_is_blank_node_label(str, endptr) (rdf_is_pn_chars(str, endptr) || \
					      *str == '.')
#define rdf_is_blank_node_label_valid_start(str, endptr) ((&(str[2]) < endptr) && \
							  str[0] == '_' && \
							  str[1] == ':' && \
							  (rdf_is_pn_chars_u(&(str[2]), endptr) || \
							   g_ascii_isdigit(str[2])))
#define rdf_is_blank_node_label_valid_end(str, endptr) (rdf_is_pn_chars(str, endptr))
#define rdf_is_langtag(str, endptr) (g_ascii_isalnum(str[1]))
#define rdf_is_langtag_valid_start(str, endptr) ((&(str[1]) < endptr) && \
						 str[0] == '@' && \
						 g_ascii_isalpha(str[1]))
#define rdf_is_integer(str, endptr) (((*str == '+' ||			\
				       *str == '-') &&			\
				      g_ascii_isdigit(str[1])) ||	\
				     g_ascii_isdigit(str))
#define rdf_is_decimal(str, endptr, ip_index) (str)
#define rdf_is_double(str, endptr, ip_index) (((*str == '+' || \
						*str == '-') && \
					      g_ascii_isdigit(str[1]) || \
					      g_ascii_isdigit(*str))
#define rdf_is_double_extended_valid_start(str, endptr, ip_index) ((((*str == '+' || \
								      *str == '-') && \
								     g_ascii_isdigit(str[1])) || \
								    g_ascii_isdigit(str)) || \
								   *str == '.')
#define rdf_is_double_extended_valid_end(str, endptr) (g_ascii_isdigit(str) || \
						       rdf_is_exponent_valid_start(str, endptr))

#define rdf_is_exponent(str, endptr) (g_ascii_isdigit(*str))
#define rdf_is_exponent_extended_valid_start(str, endptr) ((&(str[1]) < endptr) && \
							   (*str == 'e' || \
							    *str == 'E') && \
							   (g_ascii_isdigit(str[1]) || \
							    ((str[1] == '+' || \
							      str[1] == '-') && \
							     (&(str[2]) < endptr) && \
							     g_ascii_isdigit(str[2]))))
#define rdf_is_string_literal_quote_delimiter(str, endptr) (*str == '"' &&	\
							    (&(str[1]) < endptr) && \
							    str[1] != '"')
#define rdf_is_string_literal_single_quote_delimiter(str, endptr) (*str == '\'' && \
								   (&(str[1]) < endptr) && \
								   str[1] != '\'')
#define rdf_is_string_literal_long_quote_delimiter(str, endptr) ((&(str[2]) < endptr) && \
								 str[0] == '"' && \
								 str[1] == '"' && \
								 str[2] == '"')
#define rdf_is_string_literal_long_single_quote_delimiter(str, endptr) ((&(str[2]) < endptr) && \
									str[0] == '\'' && \
									str[1] == '\'' && \
									str[2] == '\'')
#define rdf_is_uchar(str, endptr) (*str == '\\' &&			\
				   ((&(str[5]) < &(buffer[sb->st_size])) && str[1] == 'u' && \
				    (g_ascii_isxdigit(str[2] && g_ascii_isxdigit(str[3]) && g_ascii_isxdigit(str[4]) && g_ascii_isxdigit(str[5])))) || \
				   ((&(str[9]) < &(buffer[sb->st_size])) && str[1] == 'U' && \
				    (g_ascii_isxdigit(str[2] && g_ascii_isxdigit(str[3]) && g_ascii_isxdigit(str[4]) && g_ascii_isxdigit(str[5]) && g_ascii_isxdigit(str[6] && g_ascii_isxdigit(str[7]) && g_ascii_isxdigit(str[8]) && g_ascii_isxdigit(str[9]))))))
#define rdf_is_echar(str, endptr) (*str == '\\' &&		\
				   (&(str[1]) < endptr) &&	\
				   (*str == 't' ||		\
				    *str == 'b' ||		\
				    *str == 'n' ||		\
				    *str == 'r' ||		\
				    *str == 'f' ||		\
				    *str == '"' ||		\
				    *str == '\'' ||		\
				    *str == '\\'))
#define rdf_is_ws(str, endptr) (*str == '\x20' ||	\
				*str == '\x09' ||	\
				*str == '\x0D' ||	\
				*str == '\x0A')
#define rdf_is_anon(str, endptr) (rdf_is_ws(str, endptr))
#define rdf_is_anon_extended_valid_start(str, endptr) (*str == '[')
#define rdf_is_anon_extended_valid_end(str, endptr) (*str == ']')
#define rdf_is_pn_chars_base(str, endptr) (g_ascii_isalnum(*str) ||	\
					   (*str >= '\xC0' && *str < '\xD6') || \
					   (*str >= '\xD8' && *str < '\xF6') || \
					   (UTF8_2_BYTE_RANGE(str, endptr, "\x00\xF8", "\x02\xFF")) || \
					   (UTF8_2_BYTE_RANGE(str, endptr, "\x03\x70", "\x03\x7D")) || \
					   (UTF8_2_BYTE_RANGE(str, endptr, "\x03\x7F", "\x1F\xFF")) || \
					   (UTF8_2_BYTE_RANGE(str, endptr, "\x20\x0C", "\x20\x0D")) || \
					   (UTF8_2_BYTE_RANGE(str, endptr, "\x20\x70", "\x21\x8F")) || \
					   (UTF8_2_BYTE_RANGE(str, endptr, "\x2C\x00", "\x2F\xEF")) || \
					   (UTF8_2_BYTE_RANGE(str, endptr, "\x30\x01", "\xD7\xFF")) || \
					   (UTF8_2_BYTE_RANGE(str, endptr, "\xF9\x00", "\xFD\xCF")) || \
					   (UTF8_2_BYTE_RANGE(str, endptr, "\xFD\xF0", "\xFF\xFD")) || \
					   (UTF8_3_BYTE_RANGE(str, endptr, "\x01\x00\x00", "\x0E\xFF\xFF")))
#define rdf_is_pn_chars_u(str, endptr) (rdf_is_pn_chars_base(str, endptr) || \
					*str == '_')
#define rdf_is_pn_chars(str, endptr) (g_ascii_isdigit(*str) ||		\
				      *str == '-' ||			\
				      *str == '\xB7' ||		\
				      (UTF8_2_BYTE_RANGE(str, endptr, "\x03\x00", "\x03\x6F")) || \
				      (UTF8_2_BYTE_RANGE(str, endptr, "\x20\x3F", "\x20\x40")))
#define rdf_is_pn_prefix(str, endptr) (rdf_is_pn_chars_base(str, endptr))
#define rdf_is_pn_prefix_extended(str, endptr) (rdf_is_pn_prefix(str, endptr) || \
						rdf_is_pn_chars(str, endptr) || \
						*str == '.')
#define rdf_is_pn_prefix_extended_valid_end(str, endptr) (rdf_is_pn_prefix(str, endptr) || \
							  rdf_is_pn_chars(str, endptr))
#define rdf_is_pn_local(str, endptr) (rdf_is_pn_chars_u(str, endptr) ||	\
				      *str == ':' ||			\
				      g_ascii_isdigit(*str) ||		\
				      rdf_is_plx(str, endptr))
#define rdf_is_pn_local_extended(str, endptr) (rdf_is_pn_local(str, endptr) || \
					       rdf_is_pn_chars(str, endptr) || \
					       *str == '.' ||		\
					       *str == ':' ||		\
					       rdf_is_plx(str, endptr))
#define rdf_is_pn_local_extended_valid_end(str, endptr) (rdf_is_pn_local(str, endptr) || \
							 rdf_is_pn_chars(str, endptr) || \
							 *str == ':' ||	\
							 rdf_is_plx(str, endptr))
#define rdf_is_plx(str, endptr) (rdf_is_percent(str, endptr) || \
				 rdf_is_hex(str, endptr))
#define rdf_is_percent(str, endptr) (str == '%' &&			\
				     str[2] < endptr &&			\
				     rdf_is_hex(&(str[1]), endptr) &&	\
				     rdf_is_hex(&(str[2]), endptr))
#define rdf_is_hex(str, endptr) ((*str >= '0' && *str <= '9') ||  \
				 (*str >= 'a' && *str <= 'f') ||  \
				 (*str >= 'A' && *str <= 'F'))
#define rdf_is_pn_local_esc(str, endptr) (*str == '\\' &&		\
					  (&(str[1]) < endptr &&	\
					   (*str == '_' ||		\
					    *str == '~' ||		\
					    *str == '.' ||		\
					    *str == '-' ||		\
					    *str == '!' ||		\
					    *str == '$' ||		\
					    *str == '&' ||		\
					    *str == '\'' ||		\
					    *str == '(' ||		\
					    *str == ')' ||		\
					    *str == '*' ||		\
					    *str == '+' ||		\
					    *str == ',' ||		\
					    *str == ';' ||		\
					    *str == '=' ||		\
					    *str == '?' ||		\
					    *str == '#' ||		\
					    *str == '@' ||		\
					    *str == '%')))

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

AgsTurtleParserContext*
ags_turtle_parser_context_alloc()
{
  AgsTurtleParserContext *parser_context;

  parser_context = (AgsTurtleParserContext *) malloc(sizeof(AgsTurtleParserContext));

  parser_context->subject_delimiter = NULL;
  parser_context->verb_delimiter = NULL;
  parser_context->value_delimiter = NULL;

  parser_context->statement_node = NULL;
  
  return(parser_context);
}

void
ags_turtle_parser_context_free(AgsTurtleParserContext *parser_context)
{
  free(parser_context);
}

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
  //  auto gchar* ags_turtle_load_read_string(gchar **iter);
  auto xmlNode* ags_turtle_load_read_statement(gchar **iter);
  auto xmlNode* ags_turtle_load_read_directive(gchar **iter);
  auto xmlNode* ags_turtle_load_read_triple(gchar **iter);
  auto xmlNode* ags_turtle_load_read_collection(gchar **iter);
  auto xmlNode* ags_turtle_load_read_predicate_object_list(gchar **iter);
  auto xmlNode* ags_turtle_load_read_object_list(gchar **iter);
  auto xmlNode* ags_turtle_load_read_verb(gchar **iter);
  auto xmlNode* ags_turtle_load_read_subject(gchar **iter);
  auto xmlNode* ags_turtle_load_read_predicate(gchar **iter);
  auto xmlNode* ags_turtle_load_read_object(gchar **iter);
  auto xmlNode* ags_turtle_load_read_literal(gchar **iter);
  auto xmlNode* ags_turtle_load_read_blank_node_property_list(gchar **iter);
  auto xmlNode* ags_turtle_load_read_iri(gchar **iter);
  auto xmlNode* ags_turtle_load_read_pname(gchar **iter);
  auto xmlNode* ags_turtle_load_read_blank_node(gchar **iter);
  
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
  
  xmlNode* ags_turtle_load_read_statement(gchar **iter){
    xmlNode *node;

    gchar *look_ahead;

    look_ahead = *iter;
    
    if(look_ahead >= &(buffer[sb->st_size])){
      return(NULL);
    }

    node = NULL;

    while(node == NULL &&
	  (look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead)) != '\0' &&
	  look_ahead < &(buffer[sb->st_size])){
      if((node = ags_turtle_load_read_directive(&look_ahead)) == NULL){
	node = ags_turtle_load_read_triple(&look_ahead);

	if(node == NULL){
	  look_ahead++;
	}
      }
    }

    *iter = look_ahead;
    
    return(node);
  }
    
  xmlNode* ags_turtle_load_read_directive(gchar **iter){
    xmlNode *node;
    xmlNode *iri_node, *pname_node;
    
    gchar *prefix_id, *base_id;
    gchar *look_ahead, *current;

    regmatch_t match_arr[1];

    static gboolean regex_compiled = FALSE;

    static regex_t prefix_id_regex;
    static regex_t base_id_regex;
    
    static const char *prefix_id_pattern = "^(\@prefix|\"PREFIX\")\0";
    static const char *base_id_pattern = "^(\@base|\"BASE\")\0";
    static const size_t max_matches = 1;
    
    node = NULL;
    iri_node = NULL;
    pname_node = NULL;
    
    if(!regex_compiled){
      regex_compiled = TRUE;
      
      regcomp(&prefix_id_regex, prefix_id_pattern, REG_EXTENDED);
      regcomp(&base_id_regex, base_id_pattern, REG_EXTENDED);      
    }
    
    look_ahead = *iter;

    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    if(*look_ahead == '\0'){
      return(NULL);
    }
    
    if(regexec(&prefix_id_regex, look_ahead, max_matches, match_arr, 0) == 0){
      /* prefix directive */
      node = xmlNewNode(NULL,
			"rdf-directive\0");
      xmlNewProp(node,
		 "type\0",
		 "prefix\0");

      g_message("read - rdf-directive\0");
      
      if(*look_ahead == '@'){
	look_ahead += 7;
      }else{
	look_ahead += 8;
      }
      
      /* read pname ln or ns */
      pname_node = ags_turtle_load_read_pname(&look_ahead);
      xmlAddChild(node,
		  pname_node);
      
      /* read iri */
      iri_node = ags_turtle_load_read_iri(&look_ahead);
      xmlAddChild(node,
		  iri_node);

      *iter = index(look_ahead, '.') + 1;
    }else{
      if(regexec(&base_id_regex, look_ahead, max_matches, match_arr, 0) == 0){
	/* base directive */
	look_ahead = *iter;
	
	/* read pname ns */
	if(*look_ahead == '@'){
	  look_ahead += 5;
	}else{
	  look_ahead += 6;
	}
      
      	node = xmlNewNode(NULL,
			  "rdf-directive\0");
	xmlNewProp(node,
		   "type\0",
		   "base\0");

	iri_node = ags_turtle_load_read_iri(&look_ahead);
	xmlAddChild(node,
		    iri_node);
	
	*iter = index(look_ahead, '.') + 1;
      }
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
    
  xmlNode* ags_turtle_load_read_collection(gchar **iter)
  {
    xmlNode *node;
    xmlNode *iri_node;
    xmlNode *pname_node;
    xmlNode *blank_node;
    xmlNode *object_node;
    xmlNode *literal_node;
    
    gchar *look_ahead, *current;

    node = xmlNewNode(NULL,
		      "rdf-collection\0");
    iri_node = NULL;
    pname_node = NULL;
    blank_node = NULL;
    object_node = NULL;
    literal_node = NULL;
    
    look_ahead = *iter;

    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);
    
    if(*look_ahead == '\0'){
      return(NULL);
    }

    if(look_ahead == '('){
      look_ahead++;
      
      while((look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead)) != '\0'){
	current = look_ahead;
	
	/* iri - IRIREF */
	iri_node = ags_turtle_load_read_iri(&look_ahead);
	  
	if(iri_node != NULL){
	  xmlAddChild(node,
		      iri_node);
	    
	  break;
	}

	/* read pname ln or ns */
	pname_node = ags_turtle_load_read_pname(&look_ahead);
	  
	if(pname_node != NULL){
	  xmlAddChild(node,
		      pname_node);
	    
	  break;
	}

	/* read blank node */
	blank_node = ags_turtle_load_read_blank_node(&look_ahead);

	if(blank_node != NULL){
	  xmlAddChild(node,
		      blank_node);
	    
	  break;
	}

	/* collection */
	if(*look_ahead == '('){
	  object_node = ags_turtle_load_read_collection(&look_ahead);
	  xmlAddChild(node,
		      object_node);
	    
	  look_ahead = iter + 1;

	  break;
	}
	  
	/* blank node - property list*/
	if(*look_ahead == '['){
	  blank_node = ags_turtle_load_read_blank_node_property_list(&look_ahead);
	  xmlAddChild(node,
		      blank_node);

	  look_ahead = iter + 1;

	  break;
	}
	  

	/* literal */
	literal_node = ags_turtle_load_read_literal(&look_ahead);
	  
	if(literal_node != NULL){
	  xmlAddChild(node,
		      literal_node);
	  break;
	}else{
	  iter = look_ahead++;
	  
	  g_warning("ags_turtle_load.c - unrecognized token");
	}
      }
    }
    
    *iter = look_ahead;
    
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
  
  xmlNode* ags_turtle_load_read_verb(gchar **iter){
    xmlNode *node;
    xmlNode *iri_node;
    
    gchar *look_ahead;

    node = NULL;
    iri_node = NULL;
    
    look_ahead = *iter;
    
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    if(*look_ahead == '\0'){
      return(NULL);
    }
    
    if(rdf_is_iriref_extended_valid_start(look_ahead, &(buffer[sb->st_size]))){
      /* iri - IRIREF */
      iri_node = ags_turtle_load_read_iri(&look_ahead);
	  
      if(iri_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-verb\0");
	  
	xmlAddChild(node,
		    iri_node);
	
	*iter = look_ahead;
      }else{
	g_warning("ags_turtle.c - syntax error\0");
      }
    }else if(*look_ahead == 'a'){
      node = xmlNewNode(NULL,
			"rdf-verb\0");
      
      /* verb - a */
      xmlNewProp(node,
		 "verb\0",
		 "a\0");

      g_message("read - verb: a\0");

      look_ahead += 2;
      *iter = look_ahead;
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
    
    return(node);
  }
  
  xmlNode* ags_turtle_load_read_predicate(gchar **iter){
    xmlNode *node;
    xmlNode *iri_node;

    gchar *look_ahead;
    
    node = NULL;
    iri_node = NULL;

    look_ahead = *iter;

    /* iri - IRIREF */
    iri_node = ags_turtle_load_read_iri(&look_ahead);
    
    if(iri_node != NULL){
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

    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    if(*look_ahead == '\0'){
      return(NULL);
    }

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
    
    return(node);
  }
    
  xmlNode* ags_turtle_load_read_literal(gchar **iter){
    xmlNode *node;
    
    gchar *literal, *str;
    regmatch_t match_arr[1];
    gboolean found_boolean_true;
    
    static gboolean regex_compiled = FALSE;
    
    static regex_t integer_literal_regex;
    static regex_t decimal_literal_regex;
    static regex_t double_literal_regex;
    static regex_t string_literal_double_quot_regex;
    static regex_t string_literal_single_quot_regex;
    static regex_t string_literal_long_double_quot_regex;
    static regex_t string_literal_long_single_quot_regex;
    
    static const char *integer_literal_pattern = "^([+-]?[0-9]+)\0";
    static const char *decimal_literal_pattern = "^([+-]?[0-9]*.[0-9]+)\0";
    static const char *double_literal_pattern = "^([+-]?([0-9]+.[0-9]*[eE] [+-]?[0-9]+)|(.[0-9]+[eE][+-]?[0-9]+)|([0-9]+[eE][+-]?[0-9]+))\0";
    static const char *string_literal_double_quot_pattern = "^(\"([^#x22#x5C#xA#xD]|.)*\")\0";
    static const char *string_literal_single_quot_pattern = "^('([^#x27#x5C#xA#xD]|.)*')\0";
    static const char *string_literal_long_double_quot_pattern = "^(\"\"\"(((\")|(\"\"))?([^\"\\]|.))*\"\"\")\0";
    static const char *string_literal_long_single_quot_pattern = "^('''(((')|(''))?([^'\\]|.))*''')\0";
    static const size_t max_matches = 1;
    
    node = NULL;

    literal = *iter;

    /* skip blanks and comments */
    literal = ags_turtle_load_skip_comments_and_blanks(&literal);

    if(*literal == '\0'){
      return(NULL);
    }

    if(!regex_compiled){
      regex_compiled = TRUE;
      
      regcomp(&integer_literal_regex, integer_literal_pattern, REG_EXTENDED);
      regcomp(&decimal_literal_regex, decimal_literal_pattern, REG_EXTENDED);
      regcomp(&double_literal_regex, double_literal_pattern, REG_EXTENDED);
      regcomp(&string_literal_double_quot_regex, string_literal_double_quot_pattern, REG_EXTENDED);
      regcomp(&string_literal_single_quot_regex, string_literal_single_quot_pattern, REG_EXTENDED);
      regcomp(&string_literal_long_double_quot_regex, string_literal_long_double_quot_pattern, REG_EXTENDED);
      regcomp(&string_literal_long_single_quot_regex, string_literal_long_single_quot_pattern, REG_EXTENDED);
    }
    
    /* literal - boolean */
    if((found_boolean_true = !g_ascii_strncasecmp(literal, "true\0", 5)) ||
       !g_ascii_strncasecmp(literal, "false\0", 6)){
      node = xmlNewNode(NULL,
			"rdf-literal\0");
      xmlNewProp(node,
		 "type\0",
		 "boolean\0");

      if(found_boolean_true){
	xmlNodeSetContent(node,
			  "true\0");

	*iter += 4;
      }else{
	xmlNodeSetContent(node,
			  "false\0");

	*iter += 5;
      }
      
      return(node);
    }

    /* literal - integer */
    if(regexec(&integer_literal_regex, literal, max_matches, match_arr, 0) == 0){
      node = xmlNewNode(NULL,
			"rdf-literal\0");
      xmlNewProp(node,
		 "type\0",
		 "integer\0");

      str = g_strndup(literal,
		      match_arr[0].rm_eo - match_arr[0].rm_so);
      
      xmlNodeSetContent(node,
			str);

      *iter += strlen(str);
      
      return(node);
    }

    /* literal - decimal */
    if(regexec(&decimal_literal_regex, literal, max_matches, match_arr, 0) == 0){
      node = xmlNewNode(NULL,
			"rdf-literal\0");
      xmlNewProp(node,
		 "type\0",
		 "decimal\0");

      str = g_strndup(literal,
		      match_arr[0].rm_eo - match_arr[0].rm_so);
      
      xmlNodeSetContent(node,
			str);

      *iter += strlen(str);

      return(node);
    }

    /* literal - double */
    if(regexec(&double_literal_regex, literal, max_matches, match_arr, 0) == 0){
      node = xmlNewNode(NULL,
			"rdf-literal\0");
      xmlNewProp(node,
		 "type\0",
		 "double\0");

      str = g_strndup(literal,
		      match_arr[0].rm_eo - match_arr[0].rm_so);
      
      xmlNodeSetContent(node,
			str);

      *iter += strlen(str);

      return(node);      
    }

    /* literal - double */
    if(regexec(&string_literal_double_quot_regex, literal, max_matches, match_arr, 0) == 0 ||
       regexec(&string_literal_single_quot_regex, literal, max_matches, match_arr, 0) == 0 ||
       regexec(&string_literal_long_double_quot_regex, literal, max_matches, match_arr, 0) == 0 ||
       regexec(&string_literal_long_single_quot_regex, literal, max_matches, match_arr, 0) == 0){
      node = xmlNewNode(NULL,
			"rdf-literal\0");
      xmlNewProp(node,
		 "type\0",
		 "string\0");

      str = g_strndup(literal,
		      match_arr[0].rm_eo - match_arr[0].rm_so);
      
      xmlNodeSetContent(node,
			str);
      
      xmlNodeSetContent(node,
			str);

      *iter += strlen(str);

      return(node);
    }

    return(NULL);
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
	node = xmlNewNode(NULL,
			  "rdf-blank-node-property-list\0");

	xmlAddChild(node,
		    predicate_object_list_node);
	
	*iter = look_ahead;
      }
    }
    
    return(node);
  }
  
  xmlNode* ags_turtle_load_read_iri(gchar **iter){
    xmlNode *node;
    xmlNode *pname_node;

    gchar *look_ahead, *current;
    gchar *iriref;
    
    node = NULL;
    pname_node = NULL;

    look_ahead = *iter;
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    if(*look_ahead == '\0'){
      return(NULL);
    }
    
    /* read iriref */
    current = look_ahead;
      
    if(rdf_is_iriref_extended_valid_start(current, &(buffer[sb->st_size]))){
      current++;
	
      while(!rdf_is_iriref_extended_valid_end(current, &(buffer[sb->st_size]))){ //FIXME:JK: rdf_is_iriref(current, &(buffer[sb->st_size]))
	current++;
      }
	
      if(!rdf_is_iriref_extended_valid_end(current, &(buffer[sb->st_size]))){
	g_warning("ags_turtle.c - malformed iri\0");
      }

      if(current != look_ahead){
	node = xmlNewNode(NULL,
			  "rdf-iri\0");

	iriref = g_strndup(look_ahead + 1,
			   current - look_ahead - 1);
      	
	g_message("read - iriref: %s\0", iriref);

	xmlNewProp(node,
		   "iriref\0",
		   iriref);
      
	*iter = current + 1;
      }
    }else{
      pname_node = ags_turtle_load_read_pname(&current);

      if(pname_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-iri\0");

	xmlAddChild(node,
		    pname_node);

	*iter = current;
      }
    }
        
    return(node);
  }

  xmlNode* ags_turtle_load_read_pname(gchar **iter){
    xmlNode *node;

    gchar *look_ahead, *current;
    gchar *pname_ns;
    
    node = NULL;

    look_ahead = *iter;
    look_ahead = ags_turtle_load_skip_comments_and_blanks(&look_ahead);

    if(*look_ahead == '\0'){
      return(NULL);
    }
    
    /* read prefixed name */
    current = look_ahead;
    
    while(rdf_is_pname_ns(current, &(buffer[sb->st_size])) ||
	  rdf_is_pname_ln(current, &(buffer[sb->st_size]))){
      current++;
    }

    if(current != look_ahead){
      node = xmlNewNode(NULL,
			"rdf-pname\0");

      pname_ns = g_strndup(look_ahead,
			   current - look_ahead);

      xmlNewProp(node,
		 "pname-ns\0",
		 pname_ns);
      
      g_message("read - pname_ns: %s\0", pname_ns);

      *iter = current;
    }

    return(node);
  }
  
  xmlNode* ags_turtle_load_read_blank_node(gchar **iter){
    xmlNode *node;
    gchar *look_ahead, *current;
    gchar *blank_node_start, *blank_node_end;

    node = NULL;

    look_ahead = *iter;
    
    /* blank node - label */
    if(rdf_is_blank_node_label_valid_start(look_ahead, &(buffer[sb->st_size]))){
      current = look_ahead + 2;
      blank_node_start = current;
      
      while(rdf_is_blank_node_label(current, &(buffer[sb->st_size]))) current++;

      if(rdf_is_blank_node_label_valid_end(current - 1, &(buffer[sb->st_size]))){
	blank_node_end = current;
	
	node = xmlNewNode(NULL,
			  "rdf-blank-node\0");
	xmlNewProp(node,
		   "label\0",
		   g_strndup(blank_node_start,
			     blank_node_end - blank_node_start));
	
	look_ahead = current;
	*iter = look_ahead;
	
	g_message("read - blank node label: %s\0", xmlGetProp(node,
							      "label\0"));

	return(node);
      }else{
	g_warning("ags_turtle.c - syntax error\0");

	return(NULL);
      }
    }

    /* blank node - anon */
    if(rdf_is_anon_extended_valid_start(look_ahead, &(buffer[sb->st_size]))){
      current = look_ahead + 1;
      blank_node_start = current;
	
      while(rdf_is_ws(current, &(buffer[sb->st_size]))) current++;

      if(rdf_is_anon_extended_valid_end(current, &(buffer[sb->st_size]))){
	blank_node_end = current;

	node = xmlNewNode(NULL,
			  "rdf-blank-node\0");
	xmlNewProp(node,
		   "anon\0",
		   NULL);
	    
	look_ahead = current + 1;
	*iter = look_ahead;
	
	g_message("read - blank node anon: %s\0", xmlGetProp(node,
							     "anon\0"));

	return(node);
      }else{
	g_warning("ags_turtle.c - syntax error\0");

	return(NULL);
      }
    }

    return(NULL);
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
  root_node = xmlNewNode(NULL, "rdf-turtle\0");
  xmlDocSetRootElement(doc, root_node);
  
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
}

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
