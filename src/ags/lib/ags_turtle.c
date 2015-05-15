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
ags_turtle_load(AgsTurtle *turtle)
{
  xmlDoc *doc;
  xmlNode *root_node, *node;
  FILE *file;
  
  gchar *buffer, *iter;
  gchar *current, *current_end_ptr;
  gchar *subject, *verb, *object;
  guint nth_level;
  
  struct stat *sb;

  auto xmlNode* ags_turtle_load_read_triple();
  auto gchar* ags_turtle_load_read_subject();
  auto xmlNode* ags_turtle_load_read_verb();
  auto xmlNode* ags_turtle_load_read_value();

  xmlNode* ags_turtle_load_read_triple()
  {
    xmlNode *node_list, *node, *child_node_list;
    xmlNode *verb, *value;
    gchar  *subject;
    gchar *start, *look_ahead;
    guint current_level;

    if(iter >= &(buffer[sb->st_size])){
      return(NULL);
    }
    
    start = iter;
    subject = ags_turtle_load_read_subject();

    if(subject == NULL){
      return(NULL);
    }

    node_list = NULL;
    current_level = nth_level;
    
  ags_turtle_load_read_triple_START:
    
    /* skip whitespaces and comments */
    look_ahead = iter;
    
    for(; *look_ahead != '\0'; look_ahead++){
      if((buffer == look_ahead && *buffer == '#') ||
	 (buffer[look_ahead - buffer - 1] == '\n' && *look_ahead == '#')){
	look_ahead = index(look_ahead, '\n');
	continue;
      }

      if(*look_ahead == '\n'){
	continue;
      }
      
      if(*look_ahead == ';' || *look_ahead == '[' || *look_ahead == ']' || *look_ahead == ',' || *look_ahead == '(' || *look_ahead == ')'){
	continue;
      }
      
      /* delimiter */
      if(*look_ahead != ' ' && *look_ahead != '\t'){
	break;
      }
    }
    
    /* read verb */
    for(; !g_ascii_isspace(*look_ahead) && *look_ahead != '\0'; look_ahead++);

    /* skip spaces */
    for(; (*look_ahead == ' ' || *look_ahead == '\t') && *look_ahead != '\0'; look_ahead++);
    
    if(*look_ahead != '['){
      verb = ags_turtle_load_read_verb();

      if(verb == NULL){
	g_warning("subject[%s] without verb\0", subject);
      }else{
	g_message("verb-a\0");
    
	if(node_list == NULL){
	  node_list = xmlNewNode(NULL, "rdf-list\0");
	}
      
	node = xmlNewNode(NULL, "rdf-triple\0");
	xmlNewProp(node,
		   "subject\0",
		   subject);
	xmlAddChild(node_list,
		    node);
      
	xmlAddChild(node,
		    verb);
      
	value = ags_turtle_load_read_value();
	xmlAddChild(verb,
		    value);

	if((*iter == ';' || *iter == '[') && current_level == nth_level && iter < &(buffer[sb->st_size])){
	  iter++;
	  goto ags_turtle_load_read_triple_START;
	}
      }
    }else{
      /* check if collection */
      nth_level++;
      child_node_list = ags_turtle_load_read_triple();

      if(child_node_list != NULL){
	g_message("child node\0");

	if(node_list == NULL){
	  node_list = xmlNewNode(NULL, "rdf-list\0");
	}

	node = xmlNewNode(NULL, "rdf-triple\0");
	xmlNewProp(node,
		   "subject\0",
		   subject);
	xmlAddChild(node_list,
		    node);
	
	xmlAddChild(node,
		    child_node_list);

	if((*iter == ';' || *iter == '[') && current_level == nth_level && iter < &(buffer[sb->st_size])){
	  goto ags_turtle_load_read_triple_START;
	}
      }
    }

    return(node_list);
  }
  
  gchar* ags_turtle_load_read_subject()
  {
    gchar *subject;
    gchar *start, *end;

    if(iter >= &(buffer[sb->st_size])){
      return(NULL);
    }
    
    subject = NULL;
    start = NULL;
    
    /* skip whitespaces and comments */
    for(; iter < &(buffer[sb->st_size]); iter++){
      if((buffer == iter && *buffer == '#') ||
	 (buffer != iter && buffer[iter - buffer - 1] == '\n' && *iter == '#')){
	iter = index(iter, '\n');
	continue;
      }

      /* skip line break */
      if(*iter == '.'){
	continue;
      }

      if(*iter == '\n'){
	continue;
      }

      if(*iter == ';' || *iter == '[' || *iter == ']' || *iter == ',' || *iter == '(' || *iter == ')'){
	g_warning("read subject unexpected char: %c\0", *iter);
	continue;
      }

      /* delimiter */
      if(*iter != ' ' && *iter != '\t'){
	start = iter;
	break;
      }
    }

    if(start == NULL){      
      return(NULL);
    }
    
    /* read subject */
    for(; !g_ascii_isspace(*iter) && *iter != '\0'; iter++);

    end = iter;

    subject = g_strndup(start,
			end - start);

    g_message("subject %s\0", subject);
    
    return(subject);
  }
  
  xmlNode* ags_turtle_load_read_verb()
  {
    gchar *verb;
    gchar *start, *end;
    gboolean type_modifier;
    
    verb = NULL;
    start = NULL;

    type_modifier = FALSE;
    
    /* skip whitespaces and comments */
    for(; *iter != '\0'; iter++){
      if((buffer == iter && *buffer == '#') ||
	 (buffer[iter - buffer - 1] == '\n' && *iter == '#')){
	iter = index(iter, '\n');
	continue;
      }

      if(*iter == '\n'){
	continue;
      }
      
      if(*iter == ';' || *iter == '[' || *iter == ']' || *iter == ',' || *iter == '(' || *iter == ')'){
	g_warning("read verb unexpected char %c\0", *iter);
	continue;
      }
      
      /* delimiter */
      if(*iter != ' ' && *iter != '\t'){
	start = iter;
	break;
      }
    }
    
    if(start == NULL){      
      return(NULL);
    }
    
    /* read verb */
    for(; !g_ascii_isspace(*iter) && *iter != '\0'; iter++);

    end = iter;

    verb = g_strndup(start,
		     end - start);
    g_message("verb %s\0", verb);

    if(!g_ascii_strncasecmp(verb,
			    "a\0",
			    2)){
      type_modifier = TRUE;
    }

    node = xmlNewNode(NULL, "rdf-verb\0");
    
    if(type_modifier){
      xmlNewProp(node,
		 "has-type\0",
		 "true\0");
    }else{
      xmlNewProp(node,
		 "do\0",
		 verb);
    }
    
    return(node);
  }
  
  xmlNode* ags_turtle_load_read_value()
  {
    xmlNode *node, *node_list;

    gchar *start, *end, *start_literal, *end_literal;
    gchar *str;
    guint current_level;
    gboolean escaped_literal;
    gboolean more_collection;
    
    object = NULL;
    start = NULL;

    node_list = xmlNewNode(NULL, "rdf-list\0");
    node = NULL;
    current_level = nth_level;
    
    do{
      more_collection = FALSE;
      
      /* skip comments/spaces and read value */
      for(;  iter < &(buffer[sb->st_size]); iter++){
	if((buffer == iter && *buffer == '#') ||
	   (buffer[iter - buffer - 1] == '\n' && *iter == '#')){
	  iter = index(iter, '\n');
	  continue;
	}

	if(*iter == '['){
	  nth_level++;
	  continue;
	}
	
	if(*iter == '\n' || *iter == ','){
	  continue;
	}

	if(*iter != ' ' && *iter != '\t'){
	  break;
	}
      }
      
      /* current node */
      node = NULL;
      start = iter;
	
      if(iter < &(buffer[sb->st_size]) && *iter != '.' && *iter != ';'){
	node = xmlNewNode(NULL, "rdf-value\0");
	xmlAddChild(node_list,
		    node);
	
	/* read value  */
	str = NULL;
	
	start_literal = NULL;
	end_literal = NULL;

	/* read value */
	for(; iter < &(buffer[sb->st_size]); iter++){

	  if(*iter == '"'){
	    if(start_literal == NULL){
	      start_literal = iter;

	      if(iter[1] == '"' && iter[2] == '"'){
		escaped_literal = TRUE;
		iter += 2;
	      }else{
		escaped_literal = FALSE;
	      }
	    }else{
	      if(escaped_literal){
		if(iter[1] == '"' && iter[2] == '"'){
		  end_literal = iter;
		  iter += 2;
		}
	      }else{
		end_literal = iter;
	      }
	    }

	    continue;
	  }

	  if(start_literal != NULL && end_literal == NULL){
	    continue;
	  }

	  if(*iter == ' ' || *iter == '\t' || *iter == '\n'){
	    break;
	  }
	}

	end = iter;
	str = g_strndup(start,
			end - start);

	/* skip and read value if needed */
	for(;  iter < &(buffer[sb->st_size]); iter++){
	  if(*iter == '\n'){
	    continue;
	  }
	  
	  if(*iter == ']'){
	    nth_level--;
	    continue;
	  }

	  if(*iter == ','){
	    more_collection = TRUE;
	    continue;
	  }
	  
	  if(*iter == '(' || *iter == ')'){
	    g_warning("unexpected char %c\0", *iter);
	    continue;
	  }

	  if(*iter == ';' || *iter == '.'){
	    break;
	  }
	  
	  if(*iter == '['){
	    nth_level++;
	    break;
	  }

	  if(*iter != ' ' && *iter != '\t'){
	    break;
	  }
	}
	
	/* set value */
	xmlNewProp(node,
		   "value\0",
		   str);
	g_message("value_str %s\0", str);
      }
    }while(more_collection && *iter != '[' && iter < &(buffer[sb->st_size]));
    
    return(node_list);
  }

  g_message("file: %s\0", turtle->filename);
  
  /* entry point */
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

  turtle->doc = 
    doc = xmlNewDoc("1.0\0");
  root_node = xmlNewNode(NULL, "rdf-turtle\0");
  xmlDocSetRootElement(doc, root_node);
  
  iter = buffer;

  do{
    nth_level = 0;
    node = ags_turtle_load_read_triple();

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
