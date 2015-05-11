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
void ags_turtle_finalize(GObject *gobject);

gchar* ags_turtle_read_subject(AgsTurtle *turtle,
			       gchar *str,
			       gchar **end_ptr);
gchar* ags_turtle_read_verb(AgsTurtle *turtle,
			    gchar *str,
			    gchar **end_ptr);
gchar* ags_turtle_read_object(AgsTurtle *turtle,
			      gchar *str,
			      gchar **end_ptr);

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_FILTER,
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

  ags_turtle_parent_class = g_type_class_peek_parent(turtle);

  /* GObjectClass */
  gobject = (GObjectClass *) turtle;

  gobject->finalize = ags_turtle_finalize;
}

void
ags_turtle_init(AgsTurtle *turtle)
{
  turtle->filename = NULL;
  
  turtle->subject = NULL;

  turtle->doc = NULL;
  
  turtle->filter = NULL;
}

void
ags_turtle_finalize(GObject *gobject)
{
  AgsTurtle *turtle;
  gchar **str;
  
  turtle = AGS_TURTLE(gobject);

  g_strfreev(turtle->subject);

  g_strfreev(turtle->filter);
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

  node = xpath_object->nodesetval->nodeTab;

  for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
    if(node[i]->type == XML_ELEMENT_NODE){
      list = g_list_prepend(list,
			    node[i]);
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
  struct stat *sb;

  auto xmlNode* ags_turtle_load_read_triple();
  auto gchar* ags_turtle_load_read_subject();
  auto xmlNode* ags_turtle_load_read_verb();
  auto xmlNode* ags_turtle_load_read_value();

  xmlNode* ags_turtle_load_read_triple()
  {
    xmlNode *node, *node_list;
    xmlNode *verb, *value;
    gchar  *subject, *child_subject, *look_ahead;
    gchar *start;
    
    start = iter;
    subject = ags_turtle_load_read_subject();

    if(subject == NULL){
      return(NULL);
    }
        
    /* look ahead - skip whitespaces and comments */
    look_ahead = iter;
    
    for(; *look_ahead != '\0' && *look_ahead != '.'; look_ahead++){
      if((buffer == look_ahead && *buffer == '#') ||
	 (buffer[look_ahead - buffer - 1] == '\n' && *look_ahead == '#')){
	look_ahead = index(look_ahead, '\n');
	continue;
      }

      if(*look_ahead == '[' || *look_ahead == '(' || *look_ahead == ','){
	break;
      }
            
      if(*look_ahead == ';'){
	iter = index(iter, '\n');
	continue;
      }
      
      if(*look_ahead == '\n'){
	break;
      }

      if(!g_ascii_isspace(*look_ahead)){
	break;
      }
    }

    /* check if collection */
    if(*look_ahead != '[' && *look_ahead != '(' && *look_ahead != ','){
      verb = ags_turtle_load_read_verb();

      if(verb == NULL){
	g_warning("subject without verb\0");
	return(NULL);
      }
      
      node = xmlNewNode(NULL, "rdf-triple\0");
      xmlNewProp(node,
		 "subject\0",
		 subject);
      
      xmlAddChild(node,
		  verb);
      
      value = ags_turtle_load_read_value();
      xmlAddChild(verb,
		  value);

      return(node);
    }else{
      node_list = xmlNewNode(NULL, "rdf-list\0");
      child_subject = ags_turtle_load_read_subject();

      do{
	verb = ags_turtle_load_read_verb();

	if(verb != NULL){
	  node = xmlNewNode(NULL, "rdf-triple\0");	
	  xmlNewProp(node,
		     "subject\0",
		     child_subject);
	  xmlAddChild(node_list,
		      node);

	  xmlAddChild(node,
		      verb);
      
	  value = ags_turtle_load_read_value();
	  xmlAddChild(node,
		      value);
	}else{
	  node = NULL;
	}
      }while(node != NULL);

      return(node_list);
    }
  }
  
  gchar* ags_turtle_load_read_subject()
  {
    gchar *subject;
    gchar *start, *end;
  
    subject = NULL;
    start = NULL;
    
    /* skip whitespaces and comments */
    for(; *iter != '\0'; iter++){
      if((buffer == iter && *buffer == '#') ||
	 (buffer[iter - buffer - 1] == '\n' && *iter == '#')){
	iter = index(iter, '\n');
	continue;
      }

      /* skip line break */
      if(*iter == ';'){
	iter = index(iter, '\n');
	continue;
      }
      
      /* delimiter */
      if(*iter == ' ' && *iter == '\t'){
	start = iter;
	break;
      }
    }

    if(start == NULL){      
      return(NULL);
    }
    
    /* read subject */
    for(; !g_ascii_isspace(*iter) && *iter != ',' && *iter != ';' && *iter != '.'; iter++);

    end = iter;

    subject = g_strndup(start,
			end - start);
    
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

      /* skip line break */
      if(*iter == ';'){
	iter = index(iter, '\n');
	continue;
      }

      /* delimiter */
      if(*iter == ' ' && *iter == '\t'){
	start = iter;
	break;
      }
    }
    
    if(start == NULL){      
      return(NULL);
    }
    
    /* read verb */
    for(; !g_ascii_isspace(*iter)  && *iter != ','  && *iter != ';' && *iter != '\n' && *iter != '.'; iter++);

    end = iter;

    verb = g_strndup(start,
		     end - start);

    if(!g_ascii_strcasencmp(verb,
			    "a\0",
			    2)){
      type_modifier = TRUE;
    }

    node = xmlNewNode(NULL, "rdf-verb\0");
    
    if(type_modifier){
      xmlNewProp(node,
		 "has-type\0",
		 "TRUE\0");
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

    gchar *look_ahead;
    gchar *start, *end;
    gchar *type_str, *value_str;
    
    object = NULL;
    start = NULL;

    /* look ahead - skip whitespaces and comments */
    look_ahead = iter;
    
    for(; *look_ahead != '\0' && *look_ahead != '.'; look_ahead++){
      if((buffer == look_ahead && *buffer == '#') ||
	 (buffer[look_ahead - buffer - 1] == '\n' || *look_ahead == '#')){
	look_ahead = index(look_ahead, '\n');
	continue;
      }

      /* collection */
      if(*look_ahead == '[' || *look_ahead == '(' || *look_ahead == ','){
	break;
      }

      /* skip line break */
      if(*look_ahead == ';'){
	look_ahead = index(look_ahead, '\n');
	continue;
      }

      /* delimiter */
      if(*look_ahead == '\n'){
	break;
      }

      /* keyword */
      if(!g_ascii_isspace(*look_ahead)){
	for(; !g_ascii_isspace(*look_ahead); look_ahead++);
      }
    }

    /* check if collection */
    if(*look_ahead != '[' && *look_ahead != '(' && *look_ahead != ','){
      node = xmlNewNode(NULL, "rdf-value\0");

      /* skip spaces and read type/value */
      for(; *iter == ' ' || *iter == '\t'; iter++);

      start = iter;
      
      for(; (iter != start && *start == '"' && *iter == '"') ||
	    (*start != '"' && !g_ascii_isspace(*iter)); iter++);

      end = iter;

      /* skip spaces, check if end or read value */
      for(; *look_ahead == ' ' && *look_ahead == '\t'; look_ahead++);

      if(*look_ahead != '.' || *look_ahead != ';' || *look_ahead != '\n'){
	type_str = g_strndup(start,
			     end - start);
	xmlNewProp(node,
		   "type\0",
		   type_str);
	
	start = look_ahead;
      
	for(; (iter != start && *start == '"' && *iter == '"') ||
	      (*start != '"' && !g_ascii_isspace(*iter)); iter++);

	end = iter;

	value_str = g_strndup(start,
			      end - start);
	xmlNewProp(node,
		   "value\0",
		   value_str);
      }else{
	value_str = g_strndup(start,
			      end - start);
	xmlNewProp(node,
		   "value\0",
		   value_str);
      }

      return(node);
    }else{
      node_list = xmlNewNode(NULL, "rdf-list\0");

      do{
	/* skip spaces */
	for(; *iter == ' ' && *iter == '\t'; iter++);

	/* skip line break */
	if(*iter == ';'){
	  iter = index(iter, '\n');
	  continue;
	}

	/* current node */
	node = NULL;
	start = iter;
	
	if(*iter != '.' && *iter != '\n'){
	  node = xmlNewNode(NULL, "rdf-value\0");


	  for(; (iter != start && *start == '"' && *iter == '"') ||
		(*start != '"' && !g_ascii_isspace(*iter)); iter++);

	  end = iter;

	  /* look ahead for value */
	  for(; *look_ahead == ' ' && *look_ahead == '\t'; look_ahead++);

	  if(*look_ahead != '.' || *look_ahead != ';' || *look_ahead != '\n'){
	    type_str = g_strndup(start,
				 end - start);
	    xmlNewProp(node,
		       "type\0",
		       type_str);
	
	    start = look_ahead;

	    for(; (iter != start && *start == '"' && *iter == '"') ||
		  (*start != '"' && !g_ascii_isspace(*iter)); iter++);

	    end = iter;

	    value_str = g_strndup(start,
				  end - start);
	    xmlNewProp(node,
		       "value\0",
		       value_str);
	  }else{
	    value_str = g_strndup(start,
				  end - start);
	    xmlNewProp(node,
		       "value\0",
		       value_str);
	  }
	}
      }while(node != NULL);
      
      return(node_list);
    }
  }
  
  sb = (struct stat *) malloc(sizeof(struct stat));
  stat(turtle->filename,
       sb);
  file = fopen(turtle->filename,
	       "r\0");

  buffer = (gchar *) malloc((sb->st_size + 1) * sizeof(gchar));
  fread(buffer, sizeof(gchar), sb->st_size, file);
  buffer[sb->st_size] = '\0';
  fclose(file);

  free(sb);

  turtle->doc = 
    doc = xmlNewDoc("1.0\0");
  root_node = xmlNewNode(NULL, "rdf-turtle\0");
  xmlDocSetRootElement(doc, root_node);
  
  iter = buffer;

  do{
    node = ags_turtle_load_read_triple();

    if(node != NULL){
      xmlAddChild(root_node,
		  node);
    }
  }while(node != NULL);
  
  free(buffer);
}

AgsTurtle*
ags_turtle_new(gchar *filename,
	       gchar **filter)
{
  AgsTurtle *turtle;
  gchar **str;
  
  turtle = g_object_new(AGS_TYPE_TURTLE,
			"filename\0", filename,
			NULL);

  if(filter != NULL){
    str = filter;

    while(*str != NULL){
      g_object_set(turtle,
		   "filter\0", str,
		   NULL);
      str++;
    }
  }
  
  return(turtle);
}
