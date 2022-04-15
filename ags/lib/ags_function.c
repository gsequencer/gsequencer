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

#include <ags/lib/ags_function.h>

#include <ags/lib/ags_math_util.h>
#include <ags/lib/ags_regex.h>

#include <stdlib.h>

#include <sys/types.h>
#include <regex.h>

#include <ags/i18n.h>

void ags_function_class_init(AgsFunctionClass *function);
void ags_function_init (AgsFunction *function);
void ags_function_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec);
void ags_function_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec);
void ags_function_finalize(GObject *gobject);

gchar* ags_function_literal_solve_expand_functions(gchar *transformed_function);
gchar* ags_function_literal_solve_numeric_exponent_only(gchar *transformed_function);
guint ags_function_literal_solve_find_max_exponent(gchar *transformed_function);

/**
 * SECTION:ags_function
 * @short_description: Function to translate values
 * @title: AgsFunction
 * @section_id:
 * @include: ags/lib/ags_function.h
 *
 * The #AgsFunction translates values from linear math to a given
 * function.
 */

#define AGS_FUNCTION_EXPONENT_PATTERN "^((exp\\()(([0-9]|"	\
  AGS_SYMBOLIC_EULER "|"					\
  AGS_SYMBOLIC_PI "|"						\
  AGS_SYMBOLIC_INFINIT "|"					\
  AGS_SYMBOLIC_COMPLEX_UNIT ")+)(\\)))"

#define ags_function_print_sin(term) (g_strdup_printf("1 / 2 * %s * exp(- %s * (%s) * log(%s)) - 1 / 2 * %s * exp(%s * (%s) * log(%s))", \
						      AGS_SYMBOLIC_COMPLEX_UNIT, \
						      AGS_SYMBOLIC_COMPLEX_UNIT, \
						      term,		\
						      AGS_SYMBOLIC_EULER, \
						      AGS_SYMBOLIC_COMPLEX_UNIT, \
						      AGS_SYMBOLIC_COMPLEX_UNIT, \
						      term,		\
						      AGS_SYMBOLIC_EULER))

#define ags_function_print_cos(term) (g_strdup_printf("(%s * exp(- %s * (%s) * log(%s))) / 2 + (%s * exp(%s * (%s) * log(%s))) / 2", \
						      AGS_SYMBOLIC_COMPLEX_UNIT, \
						      AGS_SYMBOLIC_COMPLEX_UNIT, \
						      term,		\
						      AGS_SYMBOLIC_EULER, \
						      AGS_SYMBOLIC_COMPLEX_UNIT, \
						      AGS_SYMBOLIC_COMPLEX_UNIT, \
						      term,		\
						      AGS_SYMBOLIC_EULER))

#define ags_function_print_tan(term) (g_strdup_printf("(%s * (exp(- %s * (%s) * log(%s)) - exp(%s * (%s) *  log(%s)))) / (exp(- %s * (%s) *  log(%s)) + exp(%s * (%s) *  log(%s)))", \
						      AGS_SYMBOLIC_COMPLEX_UNIT, \
						      AGS_SYMBOLIC_COMPLEX_UNIT, \
						      term,		\
						      AGS_SYMBOLIC_EULER, \
						      AGS_SYMBOLIC_COMPLEX_UNIT, \
						      AGS_SYMBOLIC_COMPLEX_UNIT, \
						      term,		\
						      AGS_SYMBOLIC_EULER, \
						      AGS_SYMBOLIC_COMPLEX_UNIT, \
						      AGS_SYMBOLIC_COMPLEX_UNIT, \
						      term,		\
						      AGS_SYMBOLIC_EULER, \
						      AGS_SYMBOLIC_COMPLEX_UNIT, \
						      AGS_SYMBOLIC_COMPLEX_UNIT, \
						      term,		\
						      AGS_SYMBOLIC_EULER))

#define ags_function_print_asin(term) (g_strdup_printf("(-%s * (log(exp(0.5 * log(1 - exp(2 * (%s)))) + %s * (%s))))", \
						       AGS_SYMBOLIC_COMPLEX_UNIT, \
						       term,		\
						       AGS_SYMBOLIC_COMPLEX_UNIT, \
						       term))

#define ags_function_print_acos(term) (g_strdup_printf("(%s / 2 + %s * log(exp(0.5 * log(1 - exp(2 * (%s)))) + %s * (%s))))", \
						       AGS_SYMBOLIC_PI,	\
						       AGS_SYMBOLIC_COMPLEX_UNIT, \
						       AGS_SYMBOLIC_COMPLEX_UNIT, \
						       term,		\
						       AGS_SYMBOLIC_COMPLEX_UNIT, \
						       term))

#define ags_function_print_atan(term) (g_strdup_printf("(0.5 * %s * log(1 - %s * %s) - 0.5 * %s * log(1 + %s * %s))", \
						       AGS_SYMBOLIC_COMPLEX_UNIT, \
						       AGS_SYMBOLIC_COMPLEX_UNIT, \
						       term,		\
						       AGS_SYMBOLIC_COMPLEX_UNIT, \
						       AGS_SYMBOLIC_COMPLEX_UNIT, \
						       term))

enum{
  PROP_0,
  PROP_SOURCE_FUNCTION,
  PROP_NORMALIZED_FUNCTION,
  PROP_PIVOT_TABLE,
  PROP_FUNCTION_VECTOR_TABLE,
  PROP_SOLVER_LEVEL,
};

static gpointer ags_function_parent_class = NULL;

static GMutex regex_mutex;

GType
ags_function_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_function = 0;

    static const GTypeInfo ags_function_info = {
      sizeof (AgsFunctionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_function_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFunction),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_function_init,
    };

    ags_type_function = g_type_register_static(G_TYPE_OBJECT,
					       "AgsFunction",
					       &ags_function_info,
					       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_function);
  }

  return g_define_type_id__volatile;
}

void
ags_function_class_init(AgsFunctionClass *function)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_function_parent_class = g_type_class_peek_parent(function);

  /* GObjectClass */
  gobject = (GObjectClass *) function;

  gobject->set_property = ags_function_set_property;
  gobject->get_property = ags_function_get_property;
  
  gobject->finalize = ags_function_finalize;

  /* properties */
  /**
   * AgsFunction:source-function:
   *
   * The source function.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("source-function",
				   i18n_pspec("function as string"),
				   i18n_pspec("The function to use to translate values"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOURCE_FUNCTION,
				  param_spec);
  
  /**
   * AgsFunction:normalized-function:
   *
   * The normalized function.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("normalized-function",
				   i18n_pspec("normalized form of function as string"),
				   i18n_pspec("The normalized form of function to use to translate values"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NORMALIZED_FUNCTION,
				  param_spec);

  /**
   * AgsFunction:pivot-table:
   *
   * The pivot table.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("pivot-table",
				    i18n_pspec("pivot table representation"),
				    i18n_pspec("The original pivot table representation"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PIVOT_TABLE,
				  param_spec);
}

void
ags_function_init(AgsFunction *function)
{
  function->flags = 0;
  
  /* function mutex */
  g_rec_mutex_init(&(function->obj_mutex));

  function->is_pushing = TRUE;

  function->equation = NULL;

  function->transformed_equation = NULL;

  function->source_function = NULL;

  function->normalized_function = NULL;

  function->symbol = NULL;

  function->solver_matrix = NULL;

  function->pivot_table_count = 0;
  function->column_count = NULL;
  function->row_count = NULL;
  function->pivot_table = NULL;

  function->solver_level = 0;
  function->solver_vector = NULL;
}

void
ags_function_set_property(GObject *gobject,
			  guint prop_id,
			  const GValue *value,
			  GParamSpec *param_spec)
{
  AgsFunction *function;

  GRecMutex *function_mutex;
  
  function = AGS_FUNCTION(gobject);

  /* get function mutex */
  function_mutex = AGS_FUNCTION_GET_OBJ_MUTEX(function);
  
  switch(prop_id){
  case PROP_SOURCE_FUNCTION:
    {
      gchar *source_function;

      source_function = (gchar *) g_value_get_string(value);

      g_rec_mutex_lock(function_mutex);
      
      if(function->source_function == source_function){
	g_rec_mutex_unlock(function_mutex);

	return;
      }
      
      if(function->source_function != NULL){
	g_free(function->source_function);
      }

      function->source_function = g_strdup(source_function);

      g_rec_mutex_unlock(function_mutex);
    }
    break;
  case PROP_NORMALIZED_FUNCTION:
    {
      gchar *normalized_function;

      normalized_function = (gchar *) g_value_get_string(value);

      g_rec_mutex_lock(function_mutex);

      if(function->normalized_function == normalized_function){
	g_rec_mutex_unlock(function_mutex);

	return;
      }
      
      if(function->normalized_function != NULL){
	g_free(function->normalized_function);
      }

      function->normalized_function = g_strdup(normalized_function);

      g_rec_mutex_unlock(function_mutex);
    }
    break;
  case PROP_PIVOT_TABLE:
    {
      AgsComplex ***pivot_table;

      pivot_table = (AgsComplex ***) g_value_get_pointer(value);

      g_rec_mutex_lock(function_mutex);

      if(pivot_table == function->pivot_table){
	g_rec_mutex_unlock(function_mutex);

	return;
      }

      function->pivot_table = pivot_table;

      g_rec_mutex_unlock(function_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_function_get_property(GObject *gobject,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *param_spec)
{
  AgsFunction *function;

  GRecMutex *function_mutex;
  
  function = AGS_FUNCTION(gobject);

  /* get function mutex */
  function_mutex = AGS_FUNCTION_GET_OBJ_MUTEX(function);

  switch(prop_id){
  case PROP_SOURCE_FUNCTION:
  {
    g_rec_mutex_lock(function_mutex);
    
    g_value_set_string(value, function->source_function);

    g_rec_mutex_unlock(function_mutex);
  }
  break;
  case PROP_NORMALIZED_FUNCTION:
  {
    g_rec_mutex_lock(function_mutex);

    g_value_set_string(value, function->normalized_function);

    g_rec_mutex_unlock(function_mutex);
  }
  break;
  case PROP_PIVOT_TABLE:
  {
    g_rec_mutex_lock(function_mutex);

    g_value_set_pointer(value, function->pivot_table);

    g_rec_mutex_unlock(function_mutex);
  }
  break;
  default:
  {
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
  break;
  }
}

void
ags_function_finalize(GObject *gobject)
{
  AgsFunction *function;

  gchar **iter;
  
  guint i, j, k;
  
  function = AGS_FUNCTION(gobject);

  if(function->equation != NULL){
    for(iter = function->equation; iter[0] != NULL; iter++){
      g_free(iter[0]);
    }

    g_free(function->equation);
  }

  if(function->source_function != NULL){
    g_free(function->source_function);
  }

  if(function->normalized_function != NULL){
    g_free(function->normalized_function);
  }

  if(function->symbol != NULL){
    for(iter = function->symbol; iter[0] != NULL; iter++){
      g_free(iter[0]);
    }

    g_free(function->symbol);
  }

  if(function->pivot_table != NULL){
    for(i = 0; i < function->pivot_table_count; i++){
      for(j = 0; j < function->row_count[i]; j++){
	ags_complex_free(function->pivot_table[i][j]);

	g_free(function->pivot_table[i][j]);
      }

      g_free(function->pivot_table[i]);
    }

    g_free(function->pivot_table);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_function_parent_class)->finalize(gobject);
}

/**
 * ags_function_collapse_parenthesis:
 * @function: the @AgsFunction
 * @function_count: (out): return location of count of possible functions
 * 
 * Collapse parenthesis by respecting many possibilities.
 * 
 * Returns: the one-dimensional array of possible functions as strings
 * 
 * Since: 3.0.0
 */
gchar**
ags_function_collapse_parenthesis(AgsFunction *function,
				  guint *function_count)
{
  gchar **target_function;

  gchar *source_function;

  gint *term_open_pos, *term_close_pos;

  guint term_open_pos_count, term_close_pos_count;
  guint n_functions;
  
  if(!AGS_IS_FUNCTION(function)){
    if(function_count != NULL){
      function_count[0] = 0;
    }

    return(NULL);
  }

  g_object_get(function,
	       "source-function", &source_function,
	       NULL);
  
  target_function = NULL;

  n_functions = 0;

  ags_math_util_find_term_parenthesis(source_function,
				      &term_open_pos, &term_close_pos,
				      &term_open_pos_count, &term_close_pos_count);
  
  /* attempt #0 - first, closed parenthesis */
  //TODO:JK: implement me

  /* attempt #1 - first, innermost parenthesis */
  //TODO:JK: implement me

  if(function_count != NULL){
    function_count[0] = n_functions;
  }
  
  return(target_function);
}

/**
 * ags_function_find_literals:
 * @function: The #AgsFunction
 * @symbol_count: return location of symbols count
 *
 * Find literal symbols i.e. variable names.
 *
 * Returns: The string vector containing symbols
 *
 * Since: 3.0.0
 */
gchar**
ags_function_find_literals(AgsFunction *function,
			   guint *symbol_count)
{
  gchar **symbol_all;

  gchar *source_function;
    
  if(!AGS_IS_FUNCTION(function)){
    if(symbol_count != NULL){
      symbol_count[0] = 0;
    }

    return(NULL);
  }

  symbol_all = NULL;
  
  /* get some fields */
  g_object_get(function,
	       "source-function", &source_function,
	       NULL);

  symbol_all = ags_math_util_find_symbol_all(source_function);

  g_free(source_function);
  
  /* return symbols and its count*/
  if(symbol_count != NULL){
    symbol_count[0] = g_strv_length(symbol_all);
  }

  return(symbol_all);
}

gchar*
ags_function_literal_solve_expand_functions(gchar *transformed_function)
{
  regmatch_t match_arr[1];
    
  gchar *expanded_functions;
  gchar *offset, *close_offset;
  gchar *str;
  gchar *open_parenthesis, *close_parenthesis, *tmp_parenthesis;

  int regexec_result;
    
  static gboolean regex_compiled = FALSE;

  static regex_t function_regex;

  static const char *function_pattern = "^(sin|cos|tan|asin|acos|atan)";

  static const size_t max_matches = 1;
    
  /* compile regex */
  g_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;

    regcomp(&function_regex, function_pattern, REG_EXTENDED);
  }

  g_mutex_unlock(&regex_mutex);

  /*  */
  expanded_functions = NULL;

  str =
    offset = g_strdup(transformed_function);
  close_offset = NULL;

  close_parenthesis = NULL;
  regexec_result = 0;
    
  while(regexec_result != REG_NOMATCH){
    if((regexec_result = regexec(&function_regex, offset, max_matches, match_arr, 0)) == 0){
      if(close_offset == NULL ||
	 close_offset > match_arr[0].rm_so){
	offset += match_arr[0].rm_so;

	/* find close parenthesis */
	open_parenthesis = offset;
	  
	while((open_parenthesis = strchr(open_parenthesis, '(')) != NULL &&
	      close_parenthesis == NULL){
	  close_parenthesis = strchr(open_parenthesis, ')');
	  tmp_parenthesis = strchr(open_parenthesis, '(');

	  if(tmp_parenthesis < close_parenthesis){
	    close_parenthesis = NULL;
	  }
	}

	close_offset = close_parenthesis;
      }else{
	if(close_offset != NULL){
	  if(!g_strcmp0(offset,
			"sin")){
	  }else if(!g_strcmp0(offset,
			      "cos")){
	  }else if(!g_strcmp0(offset,
			      "tan")){
	  }else if(!g_strcmp0(offset,
			      "asin")){
	  }else if(!g_strcmp0(offset,
			      "acos")){
	  }else if(!g_strcmp0(offset,
			      "atan")){
	  }

	    
	}else{
	  //NOTE:JK: you should report parenthesis mismatch
	    
	  break;
	}
      }
    }
  }

  return(expanded_functions);
}

gchar*
ags_function_literal_solve_numeric_exponent_only(gchar *transformed_function)
{
  gchar *numeric_exponent_only;
    
  guint n_terms;

  numeric_exponent_only = NULL;

  return(numeric_exponent_only);
}
  
guint
ags_function_literal_solve_find_max_exponent(gchar *transformed_function)
{
  regmatch_t match_arr[5];

  guint max_exponent;

  static gboolean regex_compiled = FALSE;

  static regex_t exponent_regex;

  static const char *exponent_pattern = AGS_FUNCTION_EXPONENT_PATTERN;
    
  static const size_t max_matches = 5;
    
  max_exponent = 1;

  return(max_exponent);
}

/**
 * ags_function_literal_solve:
 * @function: the #AgsFunction
 * 
 * Solves :source-function literally, allocates the pivot table and
 * creates the normalized function.
 * 
 * Since: 3.0.0
 */
void
ags_function_literal_solve(AgsFunction *function)
{
  gchar *transformed_function;
  gchar *normalized_function;
  gchar *str;
  
  guint max_exponent, available_exponent;
  guint i, j;
  
  normalized_function = NULL;
  
  /* compute dimensions */
  transformed_function = g_strdup(function->source_function);
  max_exponent = g_strv_length(function->symbol);

  /* step #0 of normalization - eliminate trigonometric functions */
  str = transformed_function;
  transformed_function = ags_function_literal_solve_expand_functions(transformed_function);

  g_free(str);

  /* step #1 of normalization - numeric only exponents */
  str = transformed_function;
  transformed_function = ags_function_literal_solve_numeric_exponent_only(transformed_function);

  g_free(str);
  
  /* find maximum exponent */
  available_exponent = ags_function_literal_solve_find_max_exponent(transformed_function);

  if(max_exponent < available_exponent){
    max_exponent = available_exponent;
  }

  //TODO:JK: implement me

  /* parse and merge terms */
  //TODO:JK: implement me
  
  function->normalized_function = normalized_function;
}

/**
 * ags_function_push_equation:
 * @function: the #AgsFunction
 * @equation: an equation as string
 * 
 * Push an equation to the solver stack. 
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_function_push_equation(AgsFunction *function,
			   gchar *equation)
{
  gchar *str;

  guint equation_count;
  guint i;
  
  if(!AGS_IS_FUNCTION(function)){
    return(FALSE);
  }

  i =
    equation_count = g_strv_length(function->equation);
  
  if(equation_count == 0){
    function->equation = (gchar **) g_malloc(sizeof(gchar *));

    function->transformed_equation = (gchar **) g_malloc(sizeof(gchar *));
  }else{
    function->equation = (gchar **) g_realloc(function->equation,
					      (i + 1) * sizeof(gchar *));

    function->transformed_equation = (gchar **) g_realloc(function->equation,
							  (i + 1) * sizeof(gchar *));
  }

  /* assume normalized else use right side as term - use subtraction */
  if((str = strchr(equation, '=')) == NULL){
    function->equation[i] = g_strdup(equation);
  }else{
    gchar *offset;

    guint num_bytes;
    guint tmp0, tmp1;

    tmp0 = 
      num_bytes = strlen(equation);
    num_bytes += 5;
    
    function->equation[i] = (gchar *) g_malloc(num_bytes * sizeof(gchar));
    function->equation[i][num_bytes - 1] = '\0';

    offset = function->equation[i];

    *offset = '(';
    offset++;

    tmp1 = (str - equation);
    memcpy(offset, equation, tmp1 * sizeof(gchar));
    offset += tmp1;
    
    *offset = ')';
    offset++;

    *offset = '-';
    offset++;

    *offset = '(';
    offset++;

    tmp0 = tmp0 - tmp1 - 1;
    memcpy(offset, equation + tmp1 + 1, tmp0 * sizeof(gchar));

    *offset = ')';
    offset++;
  }
  
  function->transformed_equation[i] = NULL;
  
  return(TRUE);
}

/**
 * ags_function_pop_equation:
 * @function: the #AgsFunction
 * @error: a #GError
 * 
 * Pops the functions of the equation stack. Call this function as you're
 * finished with pushing equations.
 * 
 * Since: 3.0.0
 */
void
ags_function_pop_equation(AgsFunction *function,
			  GError **error)
{
  guint equation_count;
  guint symbol_count;
  guint length;
  guint i;
  
  if(!AGS_IS_FUNCTION(function)){
    return;
  }

  /* retrieve of all equations string length */
  equation_count = g_strv_length(function->equation);
  length = 0;
  
  for(i = 0; i < equation_count; i++){
    length += strlen(function->equation[i]);
  }

  /* allocate source function and memcpy equations - use addition */
  if(equation_count > 0){
    if(equation_count > 1){
      length += (equation_count * 2) + (equation_count - 1) + 1;
    }else{
      length += 1;
    }
    
    function->source_function = (gchar *) g_malloc(length * sizeof(gchar));
    function->source_function[length - 1] = '\0';
    
    if(equation_count > 1){
      gchar *offset;

      guint num_bytes;
      
      offset = function->source_function;
      
      for(i = 0; i < equation_count; i++){
	if(i != 0){
	  *offset = '+';
	  offset++;
	}

	*offset = '(';
	offset++;

	num_bytes = strlen(function->equation[i]);
	memcpy(offset, function->equation[i], num_bytes * sizeof(gchar));
	offset += num_bytes;

	*offset = ')';
	offset++;	
      }
    }else{
      memcpy(function->source_function, function->equation[0], strlen(function->equation[0]) * sizeof(gchar));
    }
  }else{
    function->source_function = NULL;

    return;
  }

  /* find literals and literal solve */
  function->symbol = ags_function_find_literals(function,
						&symbol_count);
  ags_function_literal_solve(function);
  
  function->is_pushing = FALSE;
}

/**
 * ags_function_get_expanded:
 * @function: the #AgsFunction
 * @symbol: the symbols to compute
 * @symbol_count: the count of symbols
 * 
 * Expands @symbols to normalized form.
 *
 * Returns: the normalized form as string
 *
 * Since: 3.0.0
 */
gchar*
ags_function_get_expanded(AgsFunction *function,
			  gchar **symbol,
			  guint symbol_count)
{
  //TODO:JK: implement me
  
  return(NULL);
}

/**
 * ags_function_get_normalized:
 * @function: the #AgsFunction
 *
 * Get internal normalized string.
 *
 * Returns: the normalized string
 *
 * Since: 3.0.0
 */
gchar*
ags_function_get_normalized(AgsFunction *function)
{
  if(!AGS_IS_FUNCTION(function)){
    return(NULL);
  }

  return(function->normalized_function);
}

/**
 * ags_function_compute_term:
 * @term: the term as string to compute
 * @substitute_symbol: a variable to substitute
 * @substitute_value: the #AgsComplex value representing @substitute_symbol
 * 
 * Compute term by substituting @substitute_symbol with @substitue_value and do basic
 * solving.
 * 
 * Returns: the #AgsComplex value resulted by substitution
 * 
 * Since: 3.0.0
 */
AgsComplex*
ags_function_compute_term(gchar *term,
			  gchar *substitute_symbol, AgsComplex *substitute_value)
{
  AgsComplex *complex_value;

  complex_value = ags_complex_alloc();
  
  //TODO:JK: implement me
  
  return(complex_value);
}

/**
 * ags_function_symbolic_translate_value:
 * @function: the #AgsFunction
 * @symbol: the symbol to translate
 * @value: the value to substitute
 * 
 * Symbolic translate to @value for @symbol and compute resulting
 * vector.
 * 
 * Returns: the new #AgsComplex vector
 * 
 * Since: 3.0.0
 */
AgsComplex**
ags_function_symbolic_translate_value(AgsFunction *function,
				      gchar *symbol,
				      AgsComplex *value)
{
  //TODO:JK: implement me
  
  return(NULL);
}

/**
 * ags_function_substitute_values:
 * @function: the #AgsFunction
 * @symbol: the first symbol as string, or %NULL if no more symbol and value pair.
 * @...: %NULL terminated symbol and value pairs of list.
 *
 * Verify :source-function to be %TRUE or %FALSE by substitution.
 *
 * Returns: %TRUE if function evaluates, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_function_substitute_values(AgsFunction *function,
			       gchar *symbol, ...)
{
  //TODO:JK: implement me
  
  return(TRUE);
}

/**
 * ags_function_translate_value:
 * @function: the #AgsFunction
 * @value: the #AgsComplex value to translate
 * 
 * Translates @value by source function.
 * 
 * Returns: the solution as #AgsComplex boxed-type.
 * 
 * Since: 3.0.0
 */
AgsComplex*
ags_function_translate_value(AgsFunction *function,
			     AgsComplex *value)
{
  AgsComplex *retval;

  retval = NULL;
  
  //TODO:JK: implement me

  return(retval);
}

/**
 * ags_function_add_matrix:
 * @function: the #AgsFunction
 * @solver_matrix: the #AgsSolverMatrix
 * 
 * Add @solver_matrix to @function.
 * 
 * Since: 3.2.0
 */
void
ags_function_add_matrix(AgsFunction *function,
			AgsSolverMatrix *solver_matrix)
{
  if(!AGS_IS_FUNCTION(function) ||
     !AGS_IS_SOLVER_MATRIX(solver_matrix)){
    return;
  }

  if(g_list_find(function->solver_matrix, solver_matrix) == NULL){
    g_object_ref(solver_matrix);

    function->solver_matrix = g_list_prepend(function->solver_matrix, solver_matrix);
  }
}

/**
 * ags_function_remove_matrix:
 * @function: the #AgsFunction
 * @solver_matrix: the #AgsSolverMatrix
 * 
 * Remove @solver_matrix from @function.
 * 
 * Since: 3.2.0
 */
void
ags_function_remove_matrix(AgsFunction *function,
			   AgsSolverMatrix *solver_matrix)
{
  if(!AGS_IS_FUNCTION(function) ||
     !AGS_IS_SOLVER_MATRIX(solver_matrix)){
    return;
  }

  if(g_list_find(function->solver_matrix, solver_matrix) != NULL){
    function->solver_matrix = g_list_remove(function->solver_matrix, solver_matrix);

    g_object_unref(solver_matrix);
  }
}

/**
 * ags_function_new:
 * @source_function: the source function
 *
 * Instantiate a new #AgsFunction.
 *
 * Returns: the new instance
 *
 * Since: 3.0.0
 */
AgsFunction*
ags_function_new(gchar *source_function)
{
  AgsFunction *function;
  
  function = g_object_new(AGS_TYPE_FUNCTION,
			  "source-function", source_function,
			  NULL);

  return(function);
}
