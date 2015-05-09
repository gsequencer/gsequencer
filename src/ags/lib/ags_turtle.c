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

  turtle->filter = NULL;
  turtle->hash_table = g_hash_table_new_full(g_str_hash, (GEqualFunc) g_ascii_strcasecmp,
					     (GDestroyNotify) g_free,
					     (GDestroyNotify) g_free);
}

void
ags_turtle_finalize(GObject *gobject)
{
  AgsTurtle *turtle;
  gchar **str;
  
  turtle = AGS_TURTLE(gobject);

  g_strfreev(turtle->subject);

  g_strfreev(turtle->filter);
  g_hash_table_destroy(turtle->hash_table);
}

gchar*
ags_turtle_read_subject(AgsTurtle *turtle,
			gchar *str,
			char **end_ptr)
{
  gchar *subject;
  gchar *iter;
  
  subject = NULL;

  if(str != NULL){
    gchar *start, *end;

    iter = str;
    start = NULL;
    
    /* skip whitespaces and comments */
    for(; *iter != '\0'; iter++){
      if((str == iter && *str == '#') ||
	 (str[iter - str - 1] == '\n' || *iter == '#')){
	iter = index(iter, '\n');
	iter++;
	continue;
      }
      
      if(*iter != ' '){
	start = iter;
	break;
      }
    }

    if(start == NULL){
      if(end_ptr != NULL){
	*end_ptr = NULL;
      }
      
      return(NULL);
    }
    
    /* read subject */
    for(; !g_ascii_is_space(*iter); iter++);

    end = iter;
  }

  if(end_ptr != NULL){
    *end_ptr = end;
  }
  
  return(subject);
}

gchar*
ags_turtle_read_verb(AgsTurtle *turtle,
		     gchar *str,
		     gchar **end_ptr)
{
  gchar *verb;
  gchar *iter;
  
  verb = NULL;

  if(str != NULL){
    gchar *start, *end;

    iter = str;
    start = NULL;
    
    /* skip whitespaces and comments */
    for(; *iter != '\0'; iter++){
      if((str == iter && *str == '#') ||
	 (str[iter - str - 1] == '\n' || *iter == '#')){
	iter = index(iter, '\n');
	iter++;
	continue;
      }
      
      if(*iter != ' '){
	start = iter;
	break;
      }
    }
    
    if(start == NULL){
      if(end_ptr != NULL){
	*end_ptr = NULL;
      }
      
      return(NULL);
    }
    
    /* read verb */
    for(; !g_ascii_is_space(*iter); iter++);

    end = iter;
  }

  if(end_ptr != NULL){
    *end_ptr = end;
  }
  
  return(verb);
}

gchar*
ags_turtle_read_object(AgsTurtle *turtle,
		       gchar *str,
		       gchar **end_ptr)
{
  gchar *object;
  gchar *iter;
  
  object = NULL;

  if(str != NULL){
    gchar *start, *end, *look_ahead;

    iter = str;
    start = NULL;
    
  ags_turtle_read_object_READ_MORE:
    
    /* skip whitespaces and comments */
    for(; *iter != '\0'; iter++){
      if((str == iter && *str == '#') ||
	 (str[iter - str - 1] == '\n' || *iter == '#')){
	iter = index(iter, '\n');
	iter++;
	continue;
      }
      
      if(*iter != ' '){
	start = iter;

	break;
      }
    }

    if(start == NULL){
      if(end_ptr != NULL){
	*end_ptr = NULL;
      }
      
      return(NULL);
    }
    
    /* read object */
    for(; !g_ascii_is_space(*iter); iter++);
    
    /* look ahead - skip whitespaces and comments */
    look_ahead = iter;
    
    for(; *look_ahead != '\0' && *look_ahead != '.'; look_ahead++){
      if((str == look_ahead && *str == '#') ||
	 (str[look_ahead - str - 1] == '\n' || *look_ahead == '#')){
	look_ahead = index(look_ahead, '\n');
	look_ahead++;
	continue;
      }
      
      if(*look_ahead == ','){
	iter = look_ahead;
	goto ags_turtle_read_object_READ_MORE;
      }
    }

    /* end */
    end = iter;
  }

  if(end_ptr != NULL){
    *end_ptr = end;
  }
  
  return(object);
}

gboolean
ags_turtle_insert(AgsTurtle *turtle,
		  gchar *key, gchar *value)
{
  return(g_hash_table_insert(turtle->hash_table,
			     key, value));
}

gboolean
ags_turtle_remove(AgsTurtle *turtle,
		  gchar *key)
{
  g_hash_table_remove(turtle->hash_table,
		      key);
  
  return(TRUE);
}

gchar*
ags_turtle_lookup(AgsTurtle *turtle,
		  gchar *key)
{
  gchar *value;

  value = (gchar *) g_hash_table_lookup(turtle->hash_table,
					key);

  return(value);
}

gchar*
ags_turtle_value_as_string(AgsTurtle *turtle,
			   gchar *key,
			   gchar **verb)
{
  gchar *value, *str, *current, *current_end_ptr;

  str = ags_turtle_lookup(turtle,
			  key);

  current = ags_turtle_read_verb(turtle,
				 str,
				 &current_end_ptr);
  
  if(verb != NULL){
    *verb = g_strndup(current, current_end_ptr - current);
  }

  str = current_end_ptr;
  current = ags_turtle_read_object(turtle,
				   str,
				   &current_end_ptr);

  value = g_strndup(current,
		    current_end_ptr - current);
  
  return(value);
}

gchar*
ags_turtle_value_with_verb_as_string(AgsTurtle *turtle,
				     gchar *key,
				     gchar *verb)
{
  gchar *value, *str, *current, *current_end_ptr, *current_verb, *current_verb_end_ptr;

  str = ags_turtle_lookup(turtle,
			  key);

  do{
    current = ags_turtle_read_verb(turtle,
				   str,
				   &current_end_ptr);
    
    if(verb != NULL){
      current_verb = current;
      current_verb_end_ptr = current_end_ptr;
    }
    
    str = current_end_ptr;
    current = ags_turtle_read_object(turtle,
				     str,
				     &current_end_ptr);

    str = current_end_ptr;
  }while(g_ascii_strncasecmp(verb,
			     current_verb,
			     current_verb_end_ptr - current_verb));
  
  if(verb != NULL){
    *verb = g_strndup(current,
		      current_verb_end_ptr - current_verb);
  }
  
  value = g_strndup(current,
		    current_end_ptr - current);
  
  return(value);
}

gchar**
ags_turtle_value_as_array(AgsTurtle *turtle,
			  gchar *key,
			  gchar **verb)
{
  gchar **value, *str, *current, *current_end_ptr;
  gchar *tmp0, *tmp1;
  guint i;
  gboolean string_on;

  str = ags_turtle_lookup(turtle,
			  key);

  /* read verb */
  current = ags_turtle_read_verb(turtle,
				 str,
				 &current_end_ptr);
  
  if(verb != NULL){
    *verb = g_strndup(current, current_end_ptr - current);
  }

  /* read object */
  value = NULL;
  i = 0;
  
  str = current_end_ptr;
  current = ags_turtle_read_object(turtle,
				   str,
				   &current_end_ptr);

  tmp0 = g_strndup(current,
		   current_end_ptr - current);

  tmp1 = g_strstrip(tmp0);
  g_free(tmp0);

  if(*tmp1 == '['){
    iter = tmp1 + 1;

  ags_turtle_value_as_array_JSON:
    
    current = NULL;
    current_end_ptr = NULL;
    
    for(; *iter != '\0'; iter++){
      if(*iter == '[' ||
	 *iter == ']'){
	continue;
      }

      if(g_ascii_is_space(*iter) ||
	 *iter == ','){
	if(current != NULL){
	  if(value == NULL){
	    value = (gchar **) malloc(sizeof(gchar *));
	    value[0] = g_strndup(current,
				 iter - current);
	  }else{
	    value = (gchar **) realloc((i + 1) * sizeof(gchar *));
	    value[i] = g_strndup(current,
				 iter - current);
	  }

	  if(*(value[i]) == '"' ||
	     *(value[i]) == '\''){
	    tmp0 = value[i];
	    
	    value[i] = g_strndup(value[i],
				 strlen(value[i]) - 2);
	    g_free(tmp0);
	  }
	  
	  i++;
	  
	  goto ags_turtle_value_as_array_JSON;
	}
      }else{
	current = iter;
      }
    }

    if(i > 0){
      value = (gchar **) realloc(value,
				 (i + 1) * sizeof(gchar *));
      value[i] = NULL;
    }
    
    g_free(tmp1);    
  }else if(*tmp1 == '('){
    iter = tmp1 + 1;

  ags_turtle_value_as_array_LIST:

    current = NULL;
    current_end_ptr = NULL;

    for(; *iter != '\0'; iter++){
      if(g_ascii_is_space(*iter) ||
	 *iter == ','){
	if(current != NULL){
	  if(value == NULL){
	    value = (gchar **) malloc(sizeof(gchar *));
	    value[0] = g_strndup(current,
				 iter - current);
	  }else{
	    value = (gchar **) realloc(value,
				       (i + 1) * sizeof(gchar *));
	    value[i] = g_strndup(current,
				 iter - current);
	  }

	  if(*(value[i]) == '"' ||
	     *(value[i]) == '\''){
	    tmp0 = value[i];
	    
	    value[i] = g_strndup(value[i],
				 strlen(value[i]) - 2);
	    g_free(tmp0);
	  }
	  
	  i++;
	  
	  goto ags_turtle_value_as_array_LIST;
	}
      }else{
	current = iter;
      }
    }

    if(i > 0){
      value = (gchar **) realloc(value,
				 (i + 1) * sizeof(gchar *));
      value[i] = NULL;
    }
  }else{
    value = (gchar **) malloc(2 * sizeof(gchar *));
    value[0] = g_strndup(current,
			 current_end_ptr - current);
    value[1] = NULL;
  }
  
  return(value);
}

gchar**
ags_turtle_value_with_verb_as_array(AgsTurtle *turtle,
				    gchar *key,
				    gchar *verb)
{
  gchar **value, *str, *current, *current_end_ptr, *current_verb, *current_verb_end_ptr;
  gchar *tmp0, *tmp1;
  guint i;
  gboolean string_on;

  str = ags_turtle_lookup(turtle,
			  key);

  do{
    /* read verb */
    current_verb = ags_turtle_read_verb(turtle,
				   str,
				   &current_verb_end_ptr);
  
    current_verb = g_strndup(current_verb, current_verb_end_ptr - current_verb);

    if(g_ascii_strncasecmp(verb,
			   current_verb,
			   strlen(current_verb))){
      g_free(verb);
      ags_turtle_read_object(turtle,
			     str,
			     &current_end_ptr);
      str = current_end_ptr;
      
      continue;
    }

    g_free(verb);

    /* read object */
    value = NULL;
    i = 0;
  
    str = current_end_ptr;
    current = ags_turtle_read_object(turtle,
				     str,
				     &current_end_ptr);

    tmp0 = g_strndup(current,
		     current_end_ptr - current);

    tmp1 = g_strstrip(tmp0);
    g_free(tmp0);

    if(*tmp1 == '['){
      iter = tmp1 + 1;

    ags_turtle_value_as_array_JSON:
    
      current = NULL;
      current_end_ptr = NULL;
    
      for(; *iter != '\0'; iter++){
	if(*iter == '[' ||
	   *iter == ']'){
	  continue;
	}

	if(g_ascii_is_space(*iter) ||
	   *iter == ','){
	  if(current != NULL){
	    if(value == NULL){
	      value = (gchar **) malloc(sizeof(gchar *));
	      value[0] = g_strndup(current,
				   iter - current);
	    }else{
	      value = (gchar **) realloc((i + 1) * sizeof(gchar *));
	      value[i] = g_strndup(current,
				   iter - current);
	    }

	    if(*(value[i]) == '"' ||
	       *(value[i]) == '\''){
	      tmp0 = value[i];
	    
	      value[i] = g_strndup(value[i],
				   strlen(value[i]) - 2);
	      g_free(tmp0);
	    }
	  
	    i++;
	  
	    goto ags_turtle_value_as_array_JSON;
	  }
	}else{
	  current = iter;
	}
      }

      if(i > 0){
	value = (gchar **) realloc(value,
				   (i + 1) * sizeof(gchar *));
	value[i] = NULL;
      }
    
      g_free(tmp1);    
    }else if(*tmp1 == '('){
      iter = tmp1 + 1;

    ags_turtle_value_as_array_LIST:

      current = NULL;
      current_end_ptr = NULL;

      for(; *iter != '\0'; iter++){
	if(g_ascii_is_space(*iter) ||
	   *iter == ','){
	  if(current != NULL){
	    if(value == NULL){
	      value = (gchar **) malloc(sizeof(gchar *));
	      value[0] = g_strndup(current,
				   iter - current);
	    }else{
	      value = (gchar **) realloc(value,
					 (i + 1) * sizeof(gchar *));
	      value[i] = g_strndup(current,
				   iter - current);
	    }

	    if(*(value[i]) == '"' ||
	       *(value[i]) == '\''){
	      tmp0 = value[i];
	    
	      value[i] = g_strndup(value[i],
				   strlen(value[i]) - 2);
	      g_free(tmp0);
	    }
	  
	    i++;
	  
	    goto ags_turtle_value_as_array_LIST;
	  }
	}else{
	  current = iter;
	}
      }

      if(i > 0){
	value = (gchar **) realloc(value,
				   (i + 1) * sizeof(gchar *));
	value[i] = NULL;
      }
    }else{
      value = (gchar **) malloc(2 * sizeof(gchar *));
      value[0] = g_strndup(current,
			   current_end_ptr - current);
      value[1] = NULL;
    }
  }while();

  return(value);
}

gchar**
ags_turtle_list_subjects(AgsTurtle *turtle)
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_turtle_load(AgsTurtle *turtle)
{
  auto void ags_turtle_load_triple(gchar *current_path,
				   gchar **ret_key, gchar **ret_value);

  void ags_turtle_load_triple(gchar *current_path,
			      gchar **ret_key, gchar **ret_value){
    //TODO:JK: implement me
  }
  
  //TODO:JK: implement me
}

void
ags_turtle_substitute(AgsTurtle *turtle)
{
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
