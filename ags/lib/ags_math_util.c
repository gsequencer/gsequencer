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

#include <string.h>

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
 * ags_math_util_find_parenthesis_all:
 * @str: the string
 * @open_position: (out): open position array return location
 * @close_position: (out): close position array return location
 * @open_position_count: (out): open position count return location
 * @close_position_count: (out): close position count return location
 * 
 * Find all parenthesis.
 * 
 * Since: 3.2.0
 */
void
ags_math_util_find_parenthesis_all(gchar *str,
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
 * ags_math_util_find_exponent_parenthesis:
 * @str: the string
 * @exponent_open_position: (out): exponent open position array return location
 * @exponent_close_position: (out): exponent close position array return location
 * @exponent_open_position_count: (out): exponent open position count return location
 * @exponent_close_position_count: (out): exponent close position count return location
 * 
 * Find exponent parenthesis.
 * 
 * Since: 3.2.0
 */
void
ags_math_util_find_exponent_parenthesis(gchar *str,
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
    guint nested_parenthesis;
    gboolean is_exponent;

    current_exponent_open_pos = -1;
    current_exponent_close_pos = -1;
      
    is_exponent = FALSE;

    /* scan prev */
    if(iter > str){
      for(tmp_iter = iter - 1; tmp_iter[0] == ' ' && tmp_iter > str; tmp_iter--);
	
      if(tmp_iter[0] == '^'){
	is_exponent = TRUE;

	current_exponent_open_pos = iter - str;
      }
    }

    /* scan next */
    if(is_exponent){
      nested_parenthesis = 0;
	
      for(tmp_iter = iter + 1; tmp_iter[0] != '\0'; tmp_iter++){
	if(tmp_iter[0] == '('){
	  nested_parenthesis++;
	}

	if(tmp_iter[0] == ')'){
	  if(nested_parenthesis == 0){
	    current_exponent_close_pos = tmp_iter - str;

	    break;
	  }else{
	    nested_parenthesis--;
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
 * ags_math_util_find_function_parenthesis:
 * @str: the string
 * @function_open_position: (out): function open position array return location
 * @function_close_position: (out): function close position array return location
 * @function_open_position_count: (out): function open position count return location
 * @function_close_position_count: (out): function close position count return location
 * 
 * Find function parenthesis.
 * 
 * Since: 3.2.0
 */
void
ags_math_util_find_function_parenthesis(gchar *str,
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

  /* find parenthesis */
  g_regex_match(function_regex, str, 0, &function_match_info);
  
  while(g_match_info_matches(function_match_info)){
    gchar *tmp_iter;

    gint start_pos, end_pos;
    gint current_function_open_pos, current_function_close_pos;
    guint nested_parenthesis;
    
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
    
    nested_parenthesis = 0;
	
    for(tmp_iter = str + current_function_open_pos + 1; tmp_iter[0] != '\0'; tmp_iter++){
      if(tmp_iter[0] == '('){
	nested_parenthesis++;
      }

      if(tmp_iter[0] == ')'){
	if(nested_parenthesis == 0){
	  current_function_close_pos = tmp_iter - str;

	  break;
	}else{
	  nested_parenthesis--;
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
 * ags_math_util_find_term_parenthesis:
 * @str: the string
 * @term_open_position: (out): term open position array return location
 * @term_close_position: (out): term close position array return location
 * @term_open_position_count: (out): term open position count return location
 * @term_close_position_count: (out): term close position count return location
 * 
 * Find term parenthesis.
 * 
 * Since: 3.2.0
 */
void
ags_math_util_find_term_parenthesis(gchar *str,
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
  
  ags_math_util_find_parenthesis_all(str,
				     &open_pos, &close_pos,
				     &open_pos_count, &close_pos_count);
  
  ags_math_util_find_exponent_parenthesis(str,
					  &exponent_open_pos, &exponent_close_pos,
					  &exponent_open_pos_count, &exponent_close_pos_count);

  ags_math_util_find_function_parenthesis(str,
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
 * ags_math_util_match_sign:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 * 
 * Match sign.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.6.0
 */
gboolean
ags_math_util_match_sign(gchar *offset,
			 gchar *end_ptr,
			 gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;
  
  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    if(offset[0] == '-' || offset[0] == '+'){
      match[0] = offset;
      match[1] = offset + 1;

      success = TRUE;
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_math_util_match_coefficient:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 * 
 * Match coefficient including optional sign.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.6.0
 */
gboolean
ags_math_util_match_coefficient(gchar *offset,
				gchar *end_ptr,
				gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gchar *iter;
  gchar *iter_start_offset, *iter_end_offset;

  gboolean has_sign;
  gboolean has_numeric;
  gboolean has_float;
  gboolean success;
  
  match[0] = NULL;
  match[1] = NULL;

  has_sign = FALSE;
  has_numeric = FALSE;
  has_float = FALSE;
  
  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    iter = offset;

    has_sign = ags_math_util_match_sign(offset,
					end_ptr,
					&iter_start_offset, &iter_end_offset);

    if(has_sign){
      iter = iter_end_offset;
    }
    
    if(!strncmp(iter,
		AGS_SYMBOLIC_EULER,
		strlen(AGS_SYMBOLIC_EULER))){
      match[0] = offset;
      match[1] = iter + strlen(AGS_SYMBOLIC_EULER);

      success = TRUE;
    }else if(!strncmp(iter,
		      AGS_SYMBOLIC_PI,
		      strlen(AGS_SYMBOLIC_PI))){
      match[0] = offset;
      match[1] = iter + strlen(AGS_SYMBOLIC_PI);

      success = TRUE;
    }else if(!strncmp(iter,
		      AGS_SYMBOLIC_PI,
		      strlen(AGS_SYMBOLIC_PI))){
      match[0] = offset;
      match[1] = iter + strlen(AGS_SYMBOLIC_PI);

      success = TRUE;
    }else if(!strncmp(iter,
		      AGS_SYMBOLIC_INFINIT,
		      strlen(AGS_SYMBOLIC_INFINIT))){
      match[0] = offset;
      match[1] = iter + strlen(AGS_SYMBOLIC_INFINIT);

      success = TRUE;
    }else if(!strncmp(iter,
		      AGS_SYMBOLIC_COMPLEX_UNIT,
		      strlen(AGS_SYMBOLIC_COMPLEX_UNIT))){
      match[0] = offset;
      match[1] = iter + strlen(AGS_SYMBOLIC_COMPLEX_UNIT);

      success = TRUE;
    }else{
      for(; iter < end_ptr; iter++){
	if(iter[0] >= '0' && iter[0] <= '9'){
	  has_numeric = TRUE;
	}else{
	  if(!has_float &&
	     iter[0] = '.'){
	    has_float = TRUE;
	  }else{
	    break;
	  }
	}
      }

      if(has_numeric){
	match[0] = offset;
	match[1] = iter;

	success = TRUE;
      }
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_math_util_match_symbol:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 * 
 * Match symbol including optional sign.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.6.0
 */
gboolean
ags_math_util_match_symbol(gchar *offset,
			   gchar *end_ptr,
			   gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gchar *iter;
  gchar *iter_start_offset, *iter_end_offset;

  gboolean has_sign;
  gboolean has_subscript;
  gboolean success;
  
  match[0] = NULL;
  match[1] = NULL;

  has_sign = FALSE;
  has_subscript = FALSE;
  
  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    iter = offset;

    has_sign = ags_math_util_match_sign(offset,
					end_ptr,
					&iter_start_offset, &iter_end_offset);

    if(has_sign){
      iter = iter_end_offset;
    }

    if((iter[0] >= 'a' && iter[0] <= 'z') ||
       (iter[0] >= 'A' && iter[0] <= 'Z')){
      success = TRUE;

      iter++;

      /* check subscript */
      for(; iter < end_ptr; iter++){
	gunichar subscript_x;

	static const gunichar subscript_0 = g_utf8_get_char(AGS_SUBSCRIPT_0);
	static const gunichar subscript_9 = g_utf8_get_char(AGS_SUBSCRIPT_9);

	subscript_x = g_utf8_get_char(iter);
	
	if(subscript_x >= subscript_0 && subscript_x <= subscript_9){
	  has_subscript = TRUE;
	}else{
	  break;
	}
      }

      match[0] = offset;
      match[1] = iter;

      success = TRUE;
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

gboolean
ags_math_util_match_exponent(gchar *offset,
			     gchar *end_ptr,
			     gchar **start_offset, gchar **end_offset)
{
  //TODO:JK: implement me
  
  return(FALSE);
}

gboolean
ags_math_util_match_operator(gchar *offset,
			     gchar *end_ptr,
			     gchar **start_offset, gchar **end_offset)
{
  //TODO:JK: implement me
  
  return(FALSE);
}

gboolean
ags_math_util_match_function(gchar *offset,
			     gchar *end_ptr,
			     gchar **start_offset, gchar **end_offset)
{
  //TODO:JK: implement me
  
  return(FALSE);
}

/**
 * ags_math_util_find_function:
 * @str: the string
 *  
 * Find next function.
 * 
 * Returns: the string offset matching function, otherwise %NULL
 * 
 * Since: 3.2.0
 */
gchar*
ags_math_util_find_function(gchar *str)
{
  GMatchInfo *function_match_info;

  gchar *retval;

  GError *error;  
  
  static const GRegex *function_regex = NULL;

  static const gchar *function_pattern = "(log|exp|sin|cos|tan|asin|acos|atan|floor|ceil|round)";

  if(str == NULL){
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

  g_mutex_unlock(&regex_mutex);

  /* find function */
  retval = NULL;
  
  g_regex_match(function_regex, str, 0, &function_match_info);

  if(g_match_info_matches(function_match_info)){
    gint start_pos, end_pos;

    g_match_info_fetch_pos(function_match_info,
			   0,
			   &start_pos, &end_pos);

    retval = str + end_pos;
  }
  
  g_match_info_free(function_match_info);
    
  return(retval);
}

/**
 * ags_math_util_find_symbol:
 * @str: the string
 *  
 * Find next symbol.
 * 
 * Returns: the string offset matching symbol, otherwise %NULL
 * 
 * Since: 3.2.0
 */
gchar*
ags_math_util_find_symbol(gchar *str)
{
  GMatchInfo *function_match_info;
  GMatchInfo *symbol_match_info;

  gchar *retval;
  gchar *tmp_str;

  gint prev, next;

  GError *error;  
  
  static const GRegex *function_regex = NULL;
  static const GRegex *symbol_regex = NULL;

  static const gchar *function_pattern = "(log|exp|sin|cos|tan|asin|acos|atan|floor|ceil|round)";
  static const gchar *symbol_pattern = "([a-zA-Z][0-9]*)";

  if(str == NULL){
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

  if(symbol_regex == NULL){
    error = NULL;
    symbol_regex = g_regex_new(symbol_pattern,
			       (G_REGEX_EXTENDED),
			       0,
			       &error);
    
    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
  }

  g_mutex_unlock(&regex_mutex);
  
  retval = NULL;

  next = -1;
  prev = -1;
    
  g_regex_match(function_regex, str, 0, &function_match_info);
 
  while(g_match_info_matches(function_match_info)){
    gint function_start_pos, function_end_pos;

    tmp_str = NULL;
    
    function_start_pos = -1;
    function_end_pos = -1;

    g_match_info_fetch_pos(function_match_info,
			   0,
			   &function_start_pos, &function_end_pos);

    next = function_start_pos;
    
    if(prev == -1){
      tmp_str = g_strdup_printf("%*.s",
				function_start_pos, str);
    }else{
      tmp_str = g_strdup_printf("%*.s",
				(function_start_pos - prev), str + prev);
    }
    
    if(tmp_str != NULL){
      g_regex_match(symbol_regex, tmp_str, 0, &symbol_match_info);

      if(g_match_info_matches(symbol_match_info)){
	gint symbol_start_pos, symbol_end_pos;

	g_match_info_fetch_pos(symbol_match_info,
			       0,
			       &symbol_start_pos, &symbol_end_pos);

	if(prev != -1){
	  retval = str + prev + symbol_start_pos;
	}else{
	  retval = str + symbol_start_pos;
	}
      }

      g_match_info_free(symbol_match_info);

      g_free(tmp_str);
    }

    if(retval != NULL){
      break;
    }

    prev = next;
    next = -1;
    
    g_match_info_next(function_match_info,
		      NULL);
  }

  g_match_info_free(function_match_info);

  if(retval == NULL){
    tmp_str = NULL;

    if(prev == -1){
      tmp_str = g_strdup(str);
    }else{
      tmp_str = g_strdup_printf("%*.s",
				(strlen(str) - prev), str + prev);
    }
    
    if(tmp_str != NULL){
      g_regex_match(symbol_regex, tmp_str, 0, &symbol_match_info);

      if(g_match_info_matches(symbol_match_info)){
	gint symbol_start_pos, symbol_end_pos;

	g_match_info_fetch_pos(symbol_match_info,
			       0,
			       &symbol_start_pos, &symbol_end_pos);

	if(prev != -1){
	  retval = str + prev + symbol_start_pos;
	}else{
	  retval = str + symbol_start_pos;
	}
      }
      
      g_match_info_free(symbol_match_info);
    }
    
    g_free(tmp_str);  
  }
  
  return(retval);
}

/**
 * ags_math_util_find_symbol_all:
 * @str: the string
 * 
 * Find all symbols.
 * 
 * Returns: the string vector containing the symbols
 * 
 * Since: 3.2.0
 */
gchar**
ags_math_util_find_symbol_all(gchar *str)
{
  GMatchInfo *function_match_info;
  GMatchInfo *literal_match_info;
  
  gchar **symbol_arr;

  gchar *current_literal;

  gint prev, next;
  guint n_literals;

  GError *error;

  static const GRegex *function_regex = NULL;
  static const GRegex *literal_regex = NULL;

  static const gchar *function_pattern = "(log|exp|sin|cos|tan|asin|acos|atan|floor|ceil|round)|([\\s\\+\\-%\\*\\/\\(\\)\\^\\|\\=])";
  static const gchar *literal_pattern = "([a-zA-Z][0-9]*)";

  if(str == NULL){
    return(NULL);
  }
  
  symbol_arr = NULL;

  n_literals = 0;
    
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
	  if(start_pos - next > 0){
	    current_literal = g_strndup(str + next,
					start_pos - next);
	  }
	}
	
	prev = start_pos;
      }
      
      next = end_pos;
    }else{
      if(start_pos - next > 0){
	current_literal = g_strndup(str + next,
				    start_pos - next);
      }
      
      next = end_pos;
      prev = start_pos;
    }

    if(current_literal != NULL){
      g_regex_match(literal_regex, current_literal, 0, &literal_match_info);
    
      while(g_match_info_matches(literal_match_info)){
	gchar *tmp_literal;

	tmp_literal = g_match_info_fetch(literal_match_info,
					 0);
	
	if(symbol_arr == NULL){
	  symbol_arr = (gchar **) g_malloc(2 * sizeof(gchar *));

	  symbol_arr[0] = g_strdup(tmp_literal);
	  symbol_arr[1] = NULL;

#ifdef AGS_DEBUG	    
	  g_message("found %s", symbol_arr[0]);
#endif
	  
	  n_literals++;
	}else{
	  if(!g_strv_contains(symbol_arr,
			      tmp_literal)){
	    symbol_arr = (gchar **) g_realloc(symbol_arr,
					      (n_literals + 2) * sizeof(gchar *));

	    symbol_arr[n_literals] = g_strdup(tmp_literal);
	    symbol_arr[n_literals + 1] = NULL;

#ifdef AGS_DEBUG	    
	    g_message("found %s", symbol_arr[n_literals]);
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
				       0);
	
      if(symbol_arr == NULL){
	symbol_arr = (gchar **) g_malloc(2 * sizeof(gchar *));

	symbol_arr[0] = g_strdup(tmp_literal);
	symbol_arr[1] = NULL;

#ifdef AGS_DEBUG	    
	g_message("found %s", symbol_arr[0]);
#endif
	  
	n_literals++;
      }else{
	if(!g_strv_contains(symbol_arr,
			    tmp_literal)){
	  symbol_arr = (gchar **) g_realloc(symbol_arr,
					    (n_literals + 2) * sizeof(gchar *));

	  symbol_arr[n_literals] = g_strdup(tmp_literal);
	  symbol_arr[n_literals + 1] = NULL;

#ifdef AGS_DEBUG	    
	  g_message("found %s", symbol_arr[n_literals]);
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

  return(symbol_arr);
}

/**
 * ags_math_util_is_term:
 * @term: the term
 * 
 * Test if @term is a term.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.0
 */
gboolean
ags_math_util_is_term(gchar *term)
{
  //TODO:JK: implement me

  return(FALSE);
}

/**
 * ags_math_util_rewrite_numeric:
 * @numeric_str: the numeric string
 * 
 * Rewrite numeric string.
 * 
 * Returns: the optimized string vector
 * 
 * Since: 3.2.0
 */
gchar**
ags_math_util_rewrite_numeric(gchar *numeric_str,
			      gboolean preserve_constants)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_math_util_split_polynomial:
 * @polynomial: the polynomial
 * @factor: (out): the return location of factors
 * @factor_exponent: (out): the return location of factor exponents
 *
 * Split @polynomial into coefficient, powers of symbols and summand.
 * 
 * Since: 3.2.0
 */
void
ags_math_util_split_polynomial(gchar *polynomial,
			       gchar ***factor, gchar ***factor_exponent)
{
  GMatchInfo *sign_match_info;
  GMatchInfo *multiply_match_info;
  GMatchInfo *numeric_match_info;
  GMatchInfo *constants_match_info;
  GMatchInfo *exponent_match_info;
  GMatchInfo *symbol_match_info;

  gchar **numeric_factor;
  gchar **numeric_factor_exponent;
  gchar **symbol_factor;
  gchar **symbol_factor_exponent;
  gchar *polynomial_sign;

  gchar *iter;
  
  guint factor_length;
  guint i, j;
  gboolean has_function;
  gboolean has_symbol;
  gboolean success;
  
  GError *error;
  
  static const GRegex *sign_regex = NULL;
  static const GRegex *multiply_regex = NULL;
  static const GRegex *numeric_regex = NULL;
  static const GRegex *constants_regex = NULL;
  static const GRegex *exponent_regex = NULL;
  static const GRegex *symbol_regex = NULL;
  
  /* groups: #1 sign */
  static const gchar *sign_pattern = "^[\\s]*([\\+\\-])";

  /* groups: #1 multiply */
  static const gchar *multiply_pattern = "^[\\s]*([\\*])";
  
  /* groups: #1-2 numeric base and fraction,  */
  static const gchar *numeric_pattern = "^[\\s]*([0-9]+(\\.[0-9]+)?)";
  
  /* groups: #1 constants */
  static const gchar *constants_pattern = "^[\\s]*([ℯ𝜋𝑖∞])";

  /* groups: #1 exponent operator, #2 exponent */
  static const gchar *exponent_pattern = "^[\\s]*(\\^)[\\s]*(\\([^)(]*+(?:(?R)[^)(]*)*+\\))";
  
  /* groups: #1 symbol */
  static const gchar *symbol_pattern = "^[\\s]*([a-zA-Z][0-9]*)";

  if(polynomial == NULL){
    goto ags_math_util_split_polynomial_RETURN_NULL;
  }

  has_function = (ags_math_util_find_function(polynomial) != NULL) ? TRUE: FALSE;

  if(has_function){
    g_critical("polynomial contains function, rewrite first");

    goto ags_math_util_split_polynomial_RETURN_NULL;
  }

  numeric_factor = NULL;
  numeric_factor_exponent = NULL;
  
  symbol_factor = NULL;
  symbol_factor_exponent = NULL;

  polynomial_sign = NULL;

  iter = NULL;

  factor_length = 0;
  
  has_function = FALSE;
  has_symbol = FALSE;
  
  /* compile regex */
  g_mutex_lock(&regex_mutex);

  if(sign_regex == NULL){
    error = NULL;
    sign_regex = g_regex_new(sign_pattern,
			     (G_REGEX_EXTENDED),
			     0,
			     &error);
    
    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
  }

  if(multiply_regex == NULL){
    error = NULL;
    multiply_regex = g_regex_new(multiply_pattern,
				 (G_REGEX_EXTENDED),
				 0,
				 &error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
  }

  if(numeric_regex == NULL){
    error = NULL;
    numeric_regex = g_regex_new(numeric_pattern,
				(G_REGEX_EXTENDED),
				0,
				&error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
  }

  if(constants_regex == NULL){
    error = NULL;
    constants_regex = g_regex_new(constants_pattern,
				  (G_REGEX_EXTENDED),
				  0,
				  &error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
  }

  if(exponent_regex == NULL){
    error = NULL;
    exponent_regex = g_regex_new(exponent_pattern,
				 (G_REGEX_EXTENDED),
				 0,
				 &error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
  }

  if(symbol_regex == NULL){
    error = NULL;
    symbol_regex = g_regex_new(symbol_pattern,
			       (G_REGEX_EXTENDED),
			       0,
			       &error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
  }
  
  g_mutex_unlock(&regex_mutex);

  has_symbol = (ags_math_util_find_symbol(polynomial) != NULL) ? TRUE: FALSE;

  /* match sign */
  iter = polynomial;
  
  polynomial_sign = NULL;
    
  g_regex_match(sign_regex, iter, 0, &sign_match_info);

  if(g_match_info_matches(sign_match_info)){
    gchar *sign_group_0;
    gchar *sign_group_1;

    sign_group_0 = g_match_info_fetch(sign_match_info,
				      0);
    sign_group_1 = g_match_info_fetch(sign_match_info,
				      1);

    polynomial_sign = sign_group_1;
      
    iter += strlen(sign_group_0);
      
    g_match_info_free(sign_match_info);
      
    g_free(sign_group_0);
  }else{
    polynomial_sign = g_strdup("+");
  }

  /* match numeric or constants including exponent */
  success = TRUE;
  
  i = 0;
    
  while(success){
    gboolean multiply_success, numeric_success, constants_success, exponent_success;

    multiply_success = FALSE;
    numeric_success = FALSE;
    constants_success = FALSE;
    exponent_success = FALSE;

    /* mulitply */
    if(numeric_factor != NULL){
      g_regex_match(multiply_regex, iter, 0, &multiply_match_info);

      if(g_match_info_matches(multiply_match_info)){
	gint start_pos, end_pos;

	g_match_info_fetch_pos(multiply_match_info,
			       0,
			       &start_pos,
			       &end_pos);

	multiply_success = TRUE;

	iter += end_pos;
      }
    
      g_match_info_free(multiply_match_info);
    }

    /* numeric */
    g_regex_match(numeric_regex, iter, 0, &numeric_match_info);

    if(g_match_info_matches(numeric_match_info)){
      gchar *numeric_group_0;
      gchar *numeric_group_1;

      numeric_group_0 = g_match_info_fetch(numeric_match_info,
					   0);
      numeric_group_1 = g_match_info_fetch(numeric_match_info,
					   1);

      if(numeric_factor == NULL){
	numeric_factor = (gchar **) g_malloc(2 * sizeof(gchar *));
	numeric_factor_exponent = (gchar **) g_malloc(2 * sizeof(gchar *));
      }else{
	numeric_factor = (gchar **) g_realloc(numeric_factor,
					      (i + 2) * sizeof(gchar *));
	numeric_factor_exponent = (gchar **) g_realloc(numeric_factor_exponent,
						       (i + 2) * sizeof(gchar *));
      }
      
      numeric_factor[i] = numeric_group_1;
      numeric_factor[i + 1] = NULL;
	
      iter += strlen(numeric_group_0);
	
      numeric_success = TRUE;

      g_free(numeric_group_0);
    }
    
    g_match_info_free(numeric_match_info);

    /* constants */
    if(!numeric_success){
      g_regex_match(constants_regex, iter, 0, &constants_match_info);

      if(g_match_info_matches(constants_match_info)){
	gchar *constants_group_0;
	gchar *constants_group_1;

	constants_group_0 = g_match_info_fetch(constants_match_info,
					       0);
	constants_group_1 = g_match_info_fetch(constants_match_info,
					       1);


	if(numeric_factor == NULL){
	  numeric_factor = (gchar **) g_malloc(2 * sizeof(gchar *));
	  numeric_factor_exponent = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  numeric_factor = (gchar **) g_realloc(numeric_factor,
						(i + 2) * sizeof(gchar *));
	  numeric_factor_exponent = (gchar **) g_realloc(numeric_factor_exponent,
							 (i + 2) * sizeof(gchar *));
	}

	numeric_factor[i] = constants_group_1;
	numeric_factor[i + 1] = NULL;
	
	iter += strlen(constants_group_0);
	
	constants_success = TRUE;
    
	g_free(constants_group_0);
      }

      g_match_info_free(constants_match_info);
    }

    /* exponent */
    if(numeric_success ||
       constants_success){
      g_regex_match(exponent_regex, iter, 0, &exponent_match_info);

      if(g_match_info_matches(exponent_match_info)){
	gchar *exponent_group_0;
	gchar *exponent_group_2;
	  
	exponent_group_0 = g_match_info_fetch(exponent_match_info,
					      0);
	exponent_group_2 = g_match_info_fetch(exponent_match_info,
					      2);

	numeric_factor_exponent[i] = g_strndup(exponent_group_2 + 1,
					       strlen(exponent_group_2) - 2);
	numeric_factor_exponent[i + 1] = NULL;
	
	iter += strlen(exponent_group_0);
	
	exponent_success = TRUE;

	g_free(exponent_group_0);
	g_free(exponent_group_2);
      }
    
      g_match_info_free(exponent_match_info);
    }

    if((numeric_success ||
	constants_success) &&
       !exponent_success){
      numeric_factor_exponent[i] = g_strdup("1");
      numeric_factor_exponent[i + 1] = NULL;	
    }
      
    /* check success */
    if(!numeric_success &&
       !constants_success){
      success = FALSE;
    }

    /* iterate */
    i++;
  }

  /* match numeric or constants including exponent */
  if(has_symbol){
    success = TRUE;

    i = 0;
    
    while(success){
      gboolean multiply_success, symbol_success, exponent_success;

      multiply_success = FALSE;
      symbol_success = FALSE;
      exponent_success = FALSE;

      /* mulitply */
      if(numeric_factor != NULL ||
	 symbol_factor != NULL){
	g_regex_match(multiply_regex, iter, 0, &multiply_match_info);

	if(g_match_info_matches(multiply_match_info)){
	  gint start_pos, end_pos;

	  g_match_info_fetch_pos(multiply_match_info,
				 0,
				 &start_pos,
				 &end_pos);

	  multiply_success = TRUE;

	  iter += end_pos;
	}
    
	g_match_info_free(multiply_match_info);
      }
      
      /* symbol */
      g_regex_match(symbol_regex, iter, 0, &symbol_match_info);

      if(g_match_info_matches(symbol_match_info)){
	gchar *symbol_group_0;
	gchar *symbol_group_1;

	symbol_group_0 = g_match_info_fetch(symbol_match_info,
					    0);
	symbol_group_1 = g_match_info_fetch(symbol_match_info,
					    1);


	if(symbol_factor == NULL){
	  symbol_factor = (gchar **) g_malloc(2 * sizeof(gchar *));
	  symbol_factor_exponent = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  symbol_factor = (gchar **) g_realloc(symbol_factor,
					       (i + 2) * sizeof(gchar *));
	  symbol_factor_exponent = (gchar **) g_realloc(symbol_factor_exponent,
							(i + 2) * sizeof(gchar *));
	}

	symbol_factor[i] = symbol_group_1;
	symbol_factor[i + 1] = NULL;
	
	iter += strlen(symbol_group_0);
	
	symbol_success = TRUE;

	g_free(symbol_group_0);
      }
    
      g_match_info_free(symbol_match_info);

      /* exponent */
      if(symbol_success){
	g_regex_match(exponent_regex, iter, 0, &exponent_match_info);

	if(g_match_info_matches(exponent_match_info)){
	  gchar *exponent_group_0;
	  gchar *exponent_group_2;
	  
	  exponent_group_0 = g_match_info_fetch(exponent_match_info,
						0);
	  exponent_group_2 = g_match_info_fetch(exponent_match_info,
						2);

	  symbol_factor_exponent[i] = g_strndup(exponent_group_2 + 1,
						strlen(exponent_group_2) - 2);
	  symbol_factor_exponent[i + 1] = NULL;
	
	  iter += strlen(exponent_group_0);
	
	  exponent_success = TRUE;

	  g_free(exponent_group_0);
	  g_free(exponent_group_2);
	}
    
	g_match_info_free(exponent_match_info);
      }

      if(symbol_success &&
	 !exponent_success){
	symbol_factor_exponent[i] = g_strdup("1");
	symbol_factor_exponent[i + 1] = NULL;	
      }
      
      /* check success */
      if(!symbol_success){
	success = FALSE;
      }

      /* iterate */
      i++;
    }
  }  

  factor_length = 0;

  factor_length += ((numeric_factor != NULL) ? (g_strv_length(numeric_factor)): 0);
  factor_length += ((symbol_factor != NULL) ? (g_strv_length(symbol_factor)): 0);

  if(factor_length > 0){
    gboolean first_success;

    first_success = TRUE;
    
    if(!g_ascii_strncasecmp(polynomial_sign, "-", 1)){
      first_success = FALSE;
    }
    
    if(factor != NULL){
      factor[0] = (gchar **) g_malloc((factor_length + 1) * sizeof(gchar *));

      i = 0;
      
      if(numeric_factor != NULL){
	for(j = 0; numeric_factor[j] != NULL; j++, i++){
	  if(!first_success){
	    factor[0][i] = g_strdup_printf("-%s",
					   numeric_factor[j]);

	    g_free(numeric_factor[j]);
    
	    first_success = TRUE;
	  }else{
	    factor[0][i] = numeric_factor[j];
	  }
	}

	g_free(numeric_factor);
      }

      if(symbol_factor != NULL){
	for(j = 0; symbol_factor[j] != NULL; j++, i++){
	  if(!first_success){
	    factor[0][i] = g_strdup_printf("-%s",
					   symbol_factor[j]);

	    g_free(symbol_factor[j]);
	    
	    first_success = TRUE;
	  }else{
	    factor[0][i] = symbol_factor[j];
	  }
	}

	g_free(symbol_factor);
      }

      factor[0][factor_length] = NULL;
    }

    if(factor_exponent != NULL){
      factor_exponent[0] = (gchar **) g_malloc((factor_length + 1) * sizeof(gchar *));

      i = 0;
      
      if(numeric_factor_exponent != NULL){
	for(j = 0; numeric_factor_exponent[j] != NULL; j++, i++){
	  factor_exponent[0][i] = numeric_factor_exponent[j];
	}

	g_free(numeric_factor_exponent);
      }

      if(symbol_factor_exponent != NULL){
	for(j = 0; symbol_factor_exponent[j] != NULL; j++, i++){
	  factor_exponent[0][i] = symbol_factor_exponent[j];
	}

	g_free(symbol_factor_exponent);
      }

      factor_exponent[0][factor_length] = NULL;
    }
    
    return;
  }

  /* return NULL */
ags_math_util_split_polynomial_RETURN_NULL:
  if(factor != NULL){
    factor[0] = NULL;
  }

  if(factor_exponent != NULL){
    factor_exponent[0] = NULL;
  }
}

/**
 * ags_math_util_split_sum:
 * @sum: the sum
 * @summand: (out): the return location of summands
 *
 * Split @sum into summands.
 * 
 * Since: 3.2.0
 */
void
ags_math_util_split_sum(gchar *sum,
			gchar ***summand)
{
  GMatchInfo *operator_match_info;
  GMatchInfo *numeric_match_info;
  GMatchInfo *symbol_match_info;
  GMatchInfo *exponent_match_info;

  gchar **polynomial_summand;
  
  gchar *iter;
  gchar *prev;
  
  gint open_parenthesis, close_parenthesis;
  gint power;
  gint constants;
  gint numeric;
  gint symbol;
  guint i;
  gboolean has_function;
  gboolean success;
  
  GError *error;
  
  static const GRegex *operator_regex = NULL;
  static const GRegex *numeric_regex = NULL;
  static const GRegex *symbol_regex = NULL;
  static const GRegex *exponent_regex = NULL;

  /* groups: #1 operator */
  static const gchar *operator_pattern = "^[\\s]*([\\+\\-])";
  
  /* groups: #1-2 numeric base and fraction,  */
  static const gchar *numeric_pattern = "^([0-9]+(\\.[0-9]+)?)";
  
  /* groups: #1 symbol */
  static const gchar *symbol_pattern = "^([a-zA-Z][0-9]*)";

  /* groups: #1 exponent operator, #2 exponent */
  static const gchar *exponent_pattern = "^(\\^)[\\s]*(\\([^)(]*+(?:(?R)[^)(]*)*+\\))";

  if(sum == NULL){
    goto ags_math_util_split_sum_RETURN_NULL;
  }

  has_function = (ags_math_util_find_function(sum) != NULL) ? TRUE: FALSE;

  if(has_function){
    g_critical("sum contains function, rewrite first");

    goto ags_math_util_split_sum_RETURN_NULL;
  }
  
  /* compile regex */
  g_mutex_lock(&regex_mutex);

  if(operator_regex == NULL){
    error = NULL;
    operator_regex = g_regex_new(operator_pattern,
				 (G_REGEX_EXTENDED),
				 0,
				 &error);
    
    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
  }

  if(numeric_regex == NULL){
    error = NULL;
    numeric_regex = g_regex_new(numeric_pattern,
				(G_REGEX_EXTENDED),
				0,
				&error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
  }

  if(symbol_regex == NULL){
    error = NULL;
    symbol_regex = g_regex_new(symbol_pattern,
			       (G_REGEX_EXTENDED),
			       0,
			       &error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
  }

  if(exponent_regex == NULL){
    error = NULL;
    exponent_regex = g_regex_new(exponent_pattern,
				 (G_REGEX_EXTENDED),
				 0,
				 &error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
  }

  g_mutex_unlock(&regex_mutex);

  polynomial_summand = NULL;

  iter =
    prev = sum;
  
  open_parenthesis = 0;
  close_parenthesis = 0;
  
  power = 0;

  numeric = 0;
  constants = 0;
  symbol = 0;
  
  i = 0;  

  success = TRUE;
  
  while(success){
    gchar *tmp_iter;

    gint operator_start_pos, operator_end_pos;

    gboolean found_polynomial;    
    gboolean operator_success;
    gboolean polynomial_success;
    
    operator_start_pos = -1;
    operator_end_pos = -1;
    
    operator_success = FALSE;
    polynomial_success = FALSE;
    
    /* operator */
    g_regex_match(operator_regex, iter, 0, &operator_match_info);

    if(g_match_info_matches(operator_match_info)){
      g_match_info_fetch_pos(operator_match_info,
			     0,
			     &operator_start_pos,
			     &operator_end_pos);
	
      operator_success = TRUE;
    }
    
    g_match_info_free(operator_match_info);

    /* scan parenthesis */    
    found_polynomial = FALSE;
      
    for(tmp_iter = iter; tmp_iter < iter + operator_start_pos; tmp_iter++){      
      if((tmp_iter[0] >= 'a' && tmp_iter[0] <= 'z') ||
	 (tmp_iter[0] >= 'A' && tmp_iter[0] <= 'Z')){
	g_regex_match(symbol_regex, tmp_iter, 0, &symbol_match_info);

	if(g_match_info_matches(symbol_match_info)){
	  gint start_pos, end_pos;

	  start_pos = -1;
	  end_pos = -1;

	  g_match_info_fetch_pos(symbol_match_info,
				 0,
				 &start_pos,
				 &end_pos);

	  tmp_iter += (end_pos - 1);
	}
    
	g_match_info_free(symbol_match_info);
	    
	symbol++;
	    
	found_polynomial = TRUE;
      }else if(tmp_iter[0] >= '0' && tmp_iter[0] <= '9'){
	g_regex_match(numeric_regex, tmp_iter, 0, &numeric_match_info);

	if(g_match_info_matches(numeric_match_info)){
	  gint start_pos, end_pos;

	  g_match_info_fetch_pos(numeric_match_info,
				 0,
				 &start_pos,
				 &end_pos);

	  tmp_iter += (end_pos - 1);
	}
    
	g_match_info_free(numeric_match_info);

	numeric++;
	  
	found_polynomial = TRUE;
      }else if(!g_ascii_strncasecmp(tmp_iter,
				    AGS_SYMBOLIC_EULER,
				    strlen(AGS_SYMBOLIC_EULER))){
	constants++;
	    
	found_polynomial = TRUE;
	  
	tmp_iter += (strlen(AGS_SYMBOLIC_EULER) - 1);
      }else if(!g_ascii_strncasecmp(tmp_iter,
				    AGS_SYMBOLIC_PI,
				    strlen(AGS_SYMBOLIC_PI))){
	constants++;
	    
	found_polynomial = TRUE;
	  
	tmp_iter += (strlen(AGS_SYMBOLIC_PI) - 1);
      }else if(!g_ascii_strncasecmp(tmp_iter,
				    AGS_SYMBOLIC_COMPLEX_UNIT,
				    strlen(AGS_SYMBOLIC_COMPLEX_UNIT))){
	constants++;
	    
	found_polynomial = TRUE;
	  
	tmp_iter += (strlen(AGS_SYMBOLIC_COMPLEX_UNIT) - 1);
      }else if(tmp_iter[0] == '('){
	open_parenthesis++;
      }else if(tmp_iter[0] == ')'){
	close_parenthesis++;
      }else if(tmp_iter[0] == '^'){
	g_regex_match(exponent_regex, tmp_iter, 0, &exponent_match_info);

	if(g_match_info_matches(exponent_match_info)){
	  gint start_pos, end_pos;

	  start_pos = -1;
	  end_pos = -1;

	  g_match_info_fetch_pos(exponent_match_info,
				 0,
				 &start_pos,
				 &end_pos);

	  tmp_iter += (end_pos - 1);
	}
    
	g_match_info_free(exponent_match_info);
	  
	power++;
      }
      
      if((symbol > 0 ||
	  numeric > 0 ||
	  constants > 0) &&
	 open_parenthesis == close_parenthesis){
	polynomial_success = TRUE;
	  
	break;
      }
    }

    if(polynomial_success){
      if(prev != sum){
	polynomial_summand = (gchar **) g_realloc(polynomial_summand,
						  (i + 2) * sizeof(gchar *));
      }else{
	polynomial_summand = (gchar **) g_malloc(2 * sizeof(gchar *));
      }

      if(operator_success){
	polynomial_summand[i] = g_strdup_printf("%*.s",
						operator_start_pos, prev);
      }else{
	polynomial_summand[i] = g_strdup(sum);

	success = FALSE;
      }

      polynomial_summand[i + 1] = NULL;
      
      if(operator_success){
	iter += operator_end_pos;
      }
      
      prev = iter;

      i++;
    }else{
      iter += operator_end_pos;
    }
  }  

  if(summand != NULL){
    summand[0] = polynomial_summand;
  }

  /* return NULL */
ags_math_util_split_sum_RETURN_NULL:

  if(summand != NULL){
    summand[0] = NULL;
  }
}
