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
  gchar *iter, *end;
  
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
    for(; !g_ascii_isspace(*iter) && *iter != ',' && *iter != ';' && *iter != '.'; iter++);

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
  gchar *iter, *end;
  
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
    for(; !g_ascii_isspace(*iter)  && *iter != ','  && *iter != ';' && *iter != '.'; iter++);

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
  gchar *iter, *end;
  
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
    for(; !g_ascii_isspace(*iter); iter++);
    
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
  gchar *iter;
  gchar *literal;
  guint open_brackets;
  guint i;

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
    literal = NULL;
    open_brackets = 1;
    
    for(; *iter != '\0'; iter++){
      if(*iter == '['){
	if(literal == NULL){
	  open_brackets++;
	}

	continue;
      }

      if(*iter == ']'){
	if(literal == NULL){
	  open_brackets--;

	  if(open_brackets > 0){
	    continue;
	  }
	}else{
	  continue;
	}
      }

      if(*literal == *iter){
	literal = NULL;
      }
      
      if((g_ascii_isspace(*iter) ||
	  *iter == ',') &&
	 literal == NULL &&
	 open_brackets == 0){
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
	  
	  goto ags_turtle_value_as_array_JSON;
	}
      }else{
	current = iter;

	if(*iter == '"' ||
	   *iter == '\''){
	  literal = iter;
	}
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
    literal = NULL;
    open_brackets = 0;
    
    for(; *iter != '\0'; iter++){
      if(*iter == '['){
	if(literal == NULL){
	  open_brackets++;
	}
	
	continue;
      }

      if(*iter == ']'){
	if(literal == NULL){
	  open_brackets--;
	  
	  if(open_brackets > 0){
	    continue;
	  }
	}else{
	  continue;
	}
      }

      if(*literal == *iter){
	literal = NULL;
      }

      if((g_ascii_isspace(*iter) ||
	  *iter == ',') &&
	 literal == NULL &&
	 open_brackets == 0){
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

      	if(*iter == '"' ||
	   *iter == '\''){
	  literal = iter;
	}
      }
    }

    if(i > 0){
      value = (gchar **) realloc(value,
				 (i + 1) * sizeof(gchar *));
      value[i] = NULL;
    }
  }else{
    tmp0 = g_strndup(current,
		     current_end_ptr - current);
    value = g_strsplit(tmp0,
		       " , \0",
		       0);
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
  gchar *iter;
  gchar *literal;
  guint open_brackets;
  guint i;

  str = ags_turtle_lookup(turtle,
			  key);

  value = NULL;
  
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
      literal = NULL;
      open_brackets = 1;
      
      for(; *iter != '\0'; iter++){
	if(*iter == '['){
	  if(literal == NULL){
	    open_brackets++;
	  }
	  
	  continue;
	}

	if(*iter == ']'){
	  if(literal == NULL){
	    open_brackets--;
	    
	    if(open_brackets > 0){
	      continue;
	    }
	  }else{
	    continue;
	  }
	}
	
	if(*literal == *iter){
	  literal = NULL;
	}
      
	if((g_ascii_isspace(*iter) ||
	    *iter == ',') &&
	   literal == NULL &&
	   open_brackets == 0){
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
	  
	    goto ags_turtle_value_as_array_JSON;
	  }
	}else{
	  current = iter;

	  if(*iter == '"' ||
	   *iter == '\''){
	    literal = iter;
	  }
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
      literal = NULL;
      open_brackets = 0;
      
      for(; *iter != '\0'; iter++){

	if(*iter == '['){
	  if(literal == NULL){
	    open_brackets++;
	  }
	  
	  continue;
	}

	if(*iter == ']'){
	  if(literal == NULL){
	    open_brackets--;

	    if(open_brackets > 0){
	      continue;
	    }
	  }else{
	    continue;
	  }
	}

	if(*literal == *iter){
	  literal = NULL;
	}
      
	if((g_ascii_isspace(*iter) ||
	    *iter == ',')  &&
	   literal == NULL &&
	   open_brackets == 0){
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

	  if(*iter == '"' ||
	     *iter == '\''){
	    literal = iter;
	  }
	}
      }

      if(i > 0){
	value = (gchar **) realloc(value,
				   (i + 1) * sizeof(gchar *));
	value[i] = NULL;
      }
    }else{
      tmp0 = g_strndup(current,
		       current_end_ptr - current);
      value = g_strsplit(tmp0,
			 " , \0",
			 0);
    }
  }while(value == NULL);

  return(value);
}

gchar**
ags_turtle_list_subjects(AgsTurtle *turtle)
{
  gchar **subjects;
  guint length;
  
  subjects = g_hash_table_get_keys_as_array(turtle->hash_table,
					     &length);
  subjects = realloc(subjects,
		     (length + 1) * sizeof(gchar *));
  subjects[length] = NULL;
  
  return(subjects);
}

void
ags_turtle_load(AgsTurtle *turtle)
{
  FILE *file;
  gchar *buffer, *iter;
  gchar *current, *current_end_ptr;
  
  struct stat *sb;

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

  iter = buffer;

  do{
    current = NULL;
    current_end_ptr = NULL;

    ags_turtle_read_subject(turtle,
			    iter,
			    &current_end_ptr);
    iter = current_end_ptr++;
    
    ags_turtle_read_verb(turtle,
			 iter,
			 &current_end_ptr);
    iter = current_end_ptr++;
    
    ags_turtle_read_object(turtle,
			   iter,
			   &current_end_ptr);

    if(iter == '.'){
      iter = current_end_ptr++;
    }else{
      iter = index(iter,
		   ".\0");
    }
    
  }while(current != NULL);
  
  free(buffer);
}

void
ags_turtle_substitute(AgsTurtle *turtle)
{
  //TODO:JK: implement me
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
