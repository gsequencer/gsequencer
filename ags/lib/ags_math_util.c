/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/lib/ags_math_util.h>

/**
 * SECTION:ags_math_util
 * @short_description: math util
 * @title: AgsMathUtil
 * @section_id:
 * @include: ags/lib/ags_math_util.h
 *
 * Common math utility functions.
 */

static GMutex regex_mutex;

/**
 * ags_math_util_find_parantheses_all:
 * @str: the string
 * @open_position: (out): open position array return location
 * @close_position: (out): close position array return location
 * @open_position_count: (out): open position count return location
 * @close_position_count: (out): close position count return location
 * 
 * Find all parantheses.
 * 
 * Since: 3.2.0
 */
void
ags_math_util_find_parantheses_all(gchar *str,
				   gint **open_position, gint **close_position,
				   guint *open_position_count, guint *close_position_count)
{
  gint *open_pos, *close_pos;

  gchar *iter;

  guint open_pos_count, close_pos_count;

  if(str == NULL){
    if(open_position != NULL){
      open_position[0] = NULL;
    }

    if(close_position != NULL){
      close_position[0] = NULL;
    }

    if(open_position_count != NULL){
      open_position_count[0] = 0;
    }

    if(close_position_count != NULL){
      close_position_count[0] = 0;
    }

    return;
  }
  
  open_pos = NULL;
  open_pos_count = 0;

  close_pos = NULL;
  close_pos_count = 0;
    
  /* get open position */
  for(iter = str; (iter = strchr(iter, '(')) != NULL; iter++){
    if(open_pos == NULL){
      open_pos = g_malloc(sizeof(gint));
    }else{
      open_pos = g_realloc(open_pos,
			   (open_pos_count + 1) * sizeof(gint));
    }

    open_pos[open_pos_count] = iter - str;
    open_pos_count++;
  }
  
  /* get close position */
  for(iter = str; (iter = strchr(iter, ')')) != NULL; iter++){
    if(close_pos == NULL){
      close_pos = g_malloc(sizeof(gint));
    }else{
      close_pos = g_realloc(close_pos,
			    (close_pos_count + 1) * sizeof(gint));
    }

    close_pos[close_pos_count] = iter - str;
    close_pos_count++;
  }

  if(open_position != NULL){
    open_position[0] = open_pos;
  }

  if(close_position != NULL){
    close_position[0] = close_pos;
  }

  if(open_position_count != NULL){
    open_position_count[0] = open_pos_count;
  }

  if(close_position_count != NULL){
    close_position_count[0] = close_pos_count;
  }
}

/**
 * ags_math_util_find_exponent_parantheses:
 * @str: the string
 * @exponent_open_position: (out): exponent open position array return location
 * @exponent_close_position: (out): exponent close position array return location
 * @exponent_open_position_count: (out): exponent open position count return location
 * @exponent_close_position_count: (out): exponent close position count return location
 * 
 * Find exponent parantheses.
 * 
 * Since: 3.2.0
 */
void
ags_math_util_find_exponent_parantheses(gchar *str,
					gint **exponent_open_position, gint **exponent_close_position,
					guint *exponent_open_position_count, guint *exponent_close_position_count)
{
  gint *exponent_open_pos, *exponent_close_pos;

  gchar *iter;

  guint exponent_open_pos_count, exponent_close_pos_count;

  if(str == NULL){
    if(exponent_open_position != NULL){
      exponent_open_position[0] = NULL;
    }

    if(exponent_close_position != NULL){
      exponent_close_position[0] = NULL;
    }

    if(exponent_open_position_count != NULL){
      exponent_open_position_count[0] = 0;
    }

    if(exponent_close_position_count != NULL){
      exponent_close_position_count[0] = 0;
    }

    return;
  }

  exponent_open_pos = NULL;
  exponent_open_pos_count = 0;
  
  exponent_close_pos = NULL;
  exponent_close_pos_count = 0;
  
  /* get exponent open and close position */
  for(iter = str; (iter = strchr(iter, '(')) != NULL; iter++){
    gchar *tmp_iter;

    gint current_exponent_open_pos, current_exponent_close_pos;
    guint nested_parantheses;
    gboolean is_exponent;

    current_exponent_open_pos = -1;
    current_exponent_close_pos = -1;
      
    is_exponent = FALSE;

    /* scan prev */
    if(iter > str){
      for(tmp_iter = iter - 1; tmp_iter[0] == ' ' && tmp_iter >= str; tmp_iter--);
	
      if(tmp_iter[0] == '^'){
	is_exponent = TRUE;

	current_exponent_open_pos = iter - str;
      }
    }

    /* scan next */
    if(is_exponent){
      nested_parantheses = 0;
	
      for(tmp_iter = iter + 1; tmp_iter[0] != '\0'; tmp_iter++){
	if(tmp_iter[0] == '('){
	  nested_parantheses++;
	}

	if(tmp_iter[0] == ')'){
	  if(nested_parantheses == 0){
	    current_exponent_close_pos = tmp_iter - str;

	    break;
	  }else{
	    nested_parantheses--;
	  }
	}
      }
    }

    if(is_exponent){
      /* exponent open position */
      if(exponent_open_pos == NULL){
	exponent_open_pos = g_malloc(sizeof(gint));
      }else{
	exponent_open_pos = g_realloc(exponent_open_pos,
				      (exponent_open_pos_count + 1) * sizeof(gint));
      }

      exponent_open_pos[exponent_open_pos_count] = current_exponent_open_pos;
      exponent_open_pos_count++;

      /* exponent close position */
      if(exponent_close_pos == NULL){
	exponent_close_pos = g_malloc(sizeof(gint));
      }else{
	exponent_close_pos = g_realloc(exponent_close_pos,
				       (exponent_close_pos_count + 1) * sizeof(gint));
      }

      exponent_close_pos[exponent_close_pos_count] = current_exponent_close_pos;
      exponent_close_pos_count++;
    }
  }

  if(exponent_open_position != NULL){
    exponent_open_position[0] = exponent_open_pos;
  }

  if(exponent_close_position != NULL){
    exponent_close_position[0] = exponent_close_pos;
  }

  if(exponent_open_position_count != NULL){
    exponent_open_position_count[0] = exponent_open_pos_count;
  }

  if(exponent_close_position_count != NULL){
    exponent_close_position_count[0] = exponent_close_pos_count;
  }
}

/**
 * ags_math_util_find_function_parantheses:
 * @str: the string
 * @function_open_position: (out): function open position array return location
 * @function_close_position: (out): function close position array return location
 * @function_open_position_count: (out): function open position count return location
 * @function_close_position_count: (out): function close position count return location
 * 
 * Find function parantheses.
 * 
 * Since: 3.2.0
 */
void
ags_math_util_find_function_parantheses(gchar *str,
					gint **function_open_position, gint **function_close_position,
					guint *function_open_position_count, guint *function_close_position_count)
{
  GMatchInfo *function_match_info;

  gint *function_open_pos, *function_close_pos;

  guint function_open_pos_count, function_close_pos_count;

  GError *error;

  static const GRegex *function_regex = NULL;

  static const gchar *function_pattern = "(log|exp|sin|cos|tan|asin|acos|atan|floor|ceil|round)";

  if(str == NULL){
    if(function_open_position != NULL){
      function_open_position[0] = NULL;
    }

    if(function_close_position != NULL){
      function_close_position[0] = NULL;
    }

    if(function_open_position_count != NULL){
      function_open_position_count[0] = 0;
    }

    if(function_close_position_count != NULL){
      function_close_position_count[0] = 0;
    }

    return;
  }

  function_open_pos = NULL;
  function_open_pos_count = 0;
  
  function_close_pos = NULL;
  function_close_pos_count = 0;

  /* compile regex */
  g_mutex_lock(&regex_mutex);

  if(function_regex == NULL){
    error = NULL;
    function_regex = g_regex_new(function_pattern,
				 (G_REGEX_EXTENDED),
				 0,
				 &error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
  }

  g_mutex_unlock(&regex_mutex);

  /* find parantheses */
  g_regex_match(function_regex, str, 0, &function_match_info);
  
  while(g_match_info_matches(function_match_info)){
    gchar *tmp_iter;

    gint start_pos, end_pos;
    gint current_function_open_pos, current_function_close_pos;
    guint nested_parantheses;
    
    current_function_open_pos = -1;
    current_function_close_pos = -1;

    g_match_info_fetch_pos(function_match_info,
			   0,
			   &start_pos, &end_pos);

    for(tmp_iter = str + end_pos; tmp_iter[0] != '\0' && tmp_iter[0] == ' '; tmp_iter++);

    if(tmp_iter[0] == '('){
      current_function_open_pos = tmp_iter - str;
    }else{
      g_match_info_next(function_match_info,
			NULL);

      continue;
    }
    
    nested_parantheses = 0;
	
    for(tmp_iter = str + current_function_open_pos + 1; tmp_iter[0] != '\0'; tmp_iter++){
      if(tmp_iter[0] == '('){
	nested_parantheses++;
      }

      if(tmp_iter[0] == ')'){
	if(nested_parantheses == 0){
	  current_function_close_pos = tmp_iter - str;

	  break;
	}else{
	  nested_parantheses--;
	}
      }
    }

    if(current_function_open_pos != -1 &&
       current_function_close_pos != -1){
      /* function open position */
      if(function_open_pos == NULL){
	function_open_pos = g_malloc(sizeof(gint));
      }else{
	function_open_pos = g_realloc(function_open_pos,
				      (function_open_pos_count + 1) * sizeof(gint));
      }

      function_open_pos[function_open_pos_count] = current_function_open_pos;
      function_open_pos_count++;

      /* function close position */
      if(function_close_pos == NULL){
	function_close_pos = g_malloc(sizeof(gint));
      }else{
	function_close_pos = g_realloc(function_close_pos,
				       (function_close_pos_count + 1) * sizeof(gint));
      }

      function_close_pos[function_close_pos_count] = current_function_close_pos;
      function_close_pos_count++;
    }

    g_match_info_next(function_match_info,
		      NULL);
  }

  if(function_open_position != NULL){
    function_open_position[0] = function_open_pos;
  }

  if(function_close_position != NULL){
    function_close_position[0] = function_close_pos;
  }

  if(function_open_position_count != NULL){
    function_open_position_count[0] = function_open_pos_count;
  }

  if(function_close_position_count != NULL){
    function_close_position_count[0] = function_close_pos_count;
  }
}

/**
 * ags_math_util_find_term_parantheses:
 * @str: the string
 * @term_open_position: (out): term open position array return location
 * @term_close_position: (out): term close position array return location
 * @term_open_position_count: (out): term open position count return location
 * @term_close_position_count: (out): term close position count return location
 * 
 * Find term parantheses.
 * 
 * Since: 3.2.0
 */
void
ags_math_util_find_term_parantheses(gchar *str,
				    gint **term_open_position, gint **term_close_position,
				    guint *term_open_position_count, guint *term_close_position_count)
{
  gint *open_pos, *close_pos;
  gint *exponent_open_pos, *exponent_close_pos;
  gint *function_open_pos, *function_close_pos;
  gint *term_open_pos, *term_close_pos;

  guint open_pos_count, close_pos_count;
  guint exponent_open_pos_count, exponent_close_pos_count;
  guint function_open_pos_count, function_close_pos_count;
  guint term_open_pos_count, term_close_pos_count;
  guint i, j, k, l;
  
  if(str == NULL){
    if(term_open_position != NULL){
      term_open_position[0] = NULL;
    }

    if(term_close_position != NULL){
      term_close_position[0] = NULL;
    }

    if(term_open_position_count != NULL){
      term_open_position_count[0] = 0;
    }

    if(term_close_position_count != NULL){
      term_close_position_count[0] = 0;
    }

    return;
  }

  open_pos = NULL;
  close_pos = NULL;

  exponent_open_pos = NULL;
  exponent_close_pos = NULL;

  function_open_pos = NULL;
  function_close_pos = NULL;
  
  term_open_pos = NULL;
  term_close_pos = NULL;
  
  ags_math_util_find_parantheses_all(str,
				     &open_pos, &close_pos,
				     &open_pos_count, &close_pos_count);
  
  ags_math_util_find_exponent_parantheses(str,
					  &exponent_open_pos, &exponent_close_pos,
					  &exponent_open_pos_count, &exponent_close_pos_count);

  ags_math_util_find_function_parantheses(str,
					  &function_open_pos, &function_close_pos,
					  &function_open_pos_count, &function_close_pos_count);
    
  term_open_pos_count = open_pos_count - exponent_open_pos_count - function_open_pos_count;
  term_close_pos_count = close_pos_count - exponent_close_pos_count - function_close_pos_count;

  if(term_open_pos_count > 0){
    term_open_pos = g_malloc(term_open_pos_count * sizeof(gint));

    for(i = 0, j = 0, k = 0, l = 0; i < term_open_pos_count;){
      for(; j < open_pos_count && k < exponent_close_pos_count && (open_pos[j] == exponent_open_pos[k] || open_pos[j] == function_open_pos[l]); j++){
	if(open_pos[j] == exponent_open_pos[k]){
	  k++;
	}

	if(open_pos[j] == function_open_pos[l]){
	  l++;
	}
      }
      
      term_open_pos[i] = open_pos[j];	
      i++;
      j++;
    }
  }
  
  if(term_close_pos_count > 0){
    term_close_pos = g_malloc(term_close_pos_count * sizeof(gint));

    for(i = 0, j = 0, k = 0, l = 0; i < term_close_pos_count;){
      for(; j < close_pos_count && k < exponent_close_pos_count && (close_pos[j] == exponent_close_pos[k] || close_pos[j] == function_close_pos[l]); j++){
	if(close_pos[j] == exponent_close_pos[k]){
	  k++;
	}

	if(close_pos[j] == function_close_pos[l]){
	  l++;
	}
      }
      
      term_close_pos[i] = close_pos[j];	
      i++;
      j++;
    }
  }

  g_free(open_pos);
  g_free(close_pos);

  g_free(exponent_open_pos);
  g_free(exponent_close_pos);

  g_free(function_open_pos);
  g_free(function_close_pos);
  
  if(term_open_position != NULL){
    term_open_position[0] = term_open_pos;
  }

  if(term_close_position != NULL){
    term_close_position[0] = term_close_pos;
  }

  if(term_open_position_count != NULL){
    term_open_position_count[0] = term_open_pos_count;
  }

  if(term_close_position_count != NULL){
    term_close_position_count[0] = term_close_pos_count;
  }
}

/**
 * ags_math_util_find_literals:
 * @str: the string
 * @symbol_count: (out): the symbol count return location
 * 
 * Find literals.
 * 
 * Returns: the string vector containing the symbols
 * 
 * Since: 3.2.0
 */
gchar**
ags_math_util_find_literals(gchar *str,
			    guint *symbol_count)
{
  GMatchInfo *function_match_info;
  GMatchInfo *literal_match_info;
  
  gchar **literals;

  gchar *current_literal;

  gint prev, next;
  guint n_literals;

  GError *error;

  static const GRegex *function_regex = NULL;
  static const GRegex *literal_regex = NULL;

  static const gchar *function_pattern = "(log|exp|sin|cos|tan|asin|acos|atan|floor|ceil|round)|([\\s\\+\\-%\\*\\/\\(\\)\\^\\=])";
  static const gchar *literal_pattern = "([a-zA-Z][0-9]*)";

  if(str == NULL){
    if(symbol_count == NULL){
      symbol_count[0] = 0;
    }
    
    return(NULL);
  }
  
  /* compile regex */
  g_mutex_lock(&regex_mutex);

  if(function_regex == NULL){
    error = NULL;
    function_regex = g_regex_new(function_pattern,
				 (G_REGEX_EXTENDED),
				 0,
				 &error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
  }

  if(literal_regex == NULL){
    error = NULL;
    literal_regex = g_regex_new(literal_pattern,
				(G_REGEX_EXTENDED),
				0,
				&error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
  }

  g_mutex_unlock(&regex_mutex);

  /* find literals */
  literals = NULL;

  n_literals = 0;
  
  g_regex_match(function_regex, str, 0, &function_match_info);

#ifdef AGS_DEBUG	    
  g_message("check %s", str);
#endif

  next = -1;
  prev = -1;
  
  while(g_match_info_matches(function_match_info)){
    gint start_pos, end_pos;
    
    current_literal = NULL;
    g_match_info_fetch_pos(function_match_info,
			   0,
			   &start_pos, &end_pos);

    if(prev == -1){
      if(start_pos != 0){
	if(next == -1){
	  current_literal = g_strndup(str,
				      start_pos);
	}else{
	  current_literal = g_strndup(str + next,
				      start_pos - next);
	}
	
	prev = start_pos;
      }
      
      next = end_pos;
    }else{
      current_literal = g_strndup(str + next,
				  start_pos - next);

      next = end_pos;
      prev = start_pos;
    }

    if(current_literal != NULL){
      g_regex_match(literal_regex, current_literal, 0, &literal_match_info);
    
      while(g_match_info_matches(literal_match_info)){
	gchar *tmp_literal;

	tmp_literal = g_match_info_fetch(literal_match_info,
					 1);
	
	if(literals == NULL){
	  literals = (gchar **) g_malloc(2 * sizeof(gchar *));

	  literals[0] = g_strdup(tmp_literal);
	  literals[1] = NULL;

#ifdef AGS_DEBUG	    
	  g_message("found %s", literals[0]);
#endif
	  
	  n_literals++;
	}else{
	  if(!g_strv_contains(literals,
			      tmp_literal)){
	    literals = (gchar **) g_realloc(literals,
					    (n_literals + 2) * sizeof(gchar *));

	    literals[n_literals] = g_strdup(tmp_literal);
	    literals[n_literals + 1] = NULL;

#ifdef AGS_DEBUG	    
	    g_message("found %s", literals[n_literals]);
#endif
	    
	    n_literals++;
	  }
	}

	g_match_info_next(literal_match_info,
			  NULL);
      }
    
      g_match_info_free(literal_match_info);

      g_free(current_literal);
    }
    
    g_match_info_next(function_match_info,
		      NULL);
  }
  
  g_match_info_free(function_match_info);

  /* last match */
  current_literal = NULL;
  
  if(prev == -1){
    if(next == -1){
      current_literal = g_strdup(str);
    }else{
      if(strlen(str) - next > 0){
	current_literal = g_strndup(str + next,
				    strlen(str) - next);
      }
    }
  }else{
    if(strlen(str) - next > 0){
      current_literal = g_strndup(str + next,
				  strlen(str) - next);
    }
  }
  
  if(current_literal != NULL){
    g_regex_match(literal_regex, current_literal, 0, &literal_match_info);
    
    while(g_match_info_matches(literal_match_info)){    
      gchar *tmp_literal;

      tmp_literal = g_match_info_fetch(literal_match_info,
				       1);
	
      if(literals == NULL){
	literals = (gchar **) g_malloc(2 * sizeof(gchar *));

	literals[0] = g_strdup(tmp_literal);
	literals[1] = NULL;

#ifdef AGS_DEBUG	    
	g_message("found %s", literals[0]);
#endif
	  
	n_literals++;
      }else{
	if(!g_strv_contains(literals,
			    tmp_literal)){
	  literals = (gchar **) g_realloc(literals,
					  (n_literals + 2) * sizeof(gchar *));

	  literals[n_literals] = g_strdup(tmp_literal);
	  literals[n_literals + 1] = NULL;

#ifdef AGS_DEBUG	    
	  g_message("found %s", literals[n_literals]);
#endif
	    
	  n_literals++;
	}
      }

      g_match_info_next(literal_match_info,
			NULL);
    }
    
    g_match_info_free(literal_match_info);

    g_free(current_literal);
  }
  
  /* return symbols and its count*/
  if(symbol_count != NULL){
    symbol_count[0] = n_literals;
  }

  return(literals);
}
