/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2016 Joël Krähemann
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

#include <regex.h>

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

#define ags_function_print_sin(str, term) (sprintf(str, "1 / 2 * %s * exp(- %s * (%s) * log(%s)) - 1 / 2 * %s * exp(%s * (%s) * log(%s))\0", \
						   AGS_SYMBOLIC_COMPLEX_UNIT, \
						   AGS_SYMBOLIC_COMPLEX_UNIT, \
						   term, \
						   AGS_SYMBOLIC_EULER,	\
						   AGS_SYMBOLIC_COMPLEX_UNIT, \
						   AGS_SYMBOLIC_COMPLEX_UNIT, \
						   term, \
						   AGS_SYMBOLIC_EULER))
#define ags_function_print_cos(str, term) (sprintf(str, "(%s * exp(- %s * (%s) * log(%s))) / 2 + (%s * exp(%s * (%s) * log(%s))) / 2\0", \
						   AGS_SYMBOLIC_COMPLEX_UNIT, \
						   AGS_SYMBOLIC_COMPLEX_UNIT, \
						   term, \
						   AGS_SYMBOLIC_EULER,	\
						   AGS_SYMBOLIC_COMPLEX_UNIT, \
						   AGS_SYMBOLIC_COMPLEX_UNIT, \
						   term, \
						   AGS_SYMBOLIC_EULER))
#define ags_function_print_tan(str, term) (sprintf(str, "(%s * (exp(- %s * (%s) * log(%s)) - exp(%s * (%s) *  log(%s)))) / (exp(- %s * (%s) *  log(%s)) + exp(%s * (%s) *  log(%s)))\0", \
						   AGS_SYMBOLIC_COMPLEX_UNIT, \
						   AGS_SYMBOLIC_COMPLEX_UNIT, \
						   term, \
						   AGS_SYMBOLIC_EULER, \
						   AGS_SYMBOLIC_COMPLEX_UNIT, \
						   AGS_SYMBOLIC_COMPLEX_UNIT, \
						   term, \
						   AGS_SYMBOLIC_EULER, \
						   AGS_SYMBOLIC_COMPLEX_UNIT, \
						   AGS_SYMBOLIC_COMPLEX_UNIT, \
						   term, \
						   AGS_SYMBOLIC_EULER, \
						   AGS_SYMBOLIC_COMPLEX_UNIT, \
						   AGS_SYMBOLIC_COMPLEX_UNIT, \
						   term, \
						   AGS_SYMBOLIC_EULER))

enum{
  PROP_0,
  PROP_SOURCE_FUNCTION,
  PROP_NORMALIZED_FUNCTION,
  PROP_PIVOT_TABLE,
  PROP_FUNCTION_VECTOR_TABLE,
  PROP_SOLVER_LEVEL,
};

static gpointer ags_function_parent_class = NULL;

GType
ags_function_get_type(void)
{
  static GType ags_type_function = 0;

  if(!ags_type_function){
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

    ags_type_function = g_type_register_static(AGS_TYPE_CONVERSION,
					       "AgsFunction\0",
					       &ags_function_info,
					       0);
  }

  return (ags_type_function);
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
   * Since: 0.7.2
   */
  param_spec = g_param_spec_string("source-function\0",
				   "function as string\0",
				   "The function to use to translate values\0",
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
   * Since: 0.7.2
   */
  param_spec = g_param_spec_string("normalized-function\0",
				   "normalized form of function as string\0",
				   "The normalized form of function to use to translate values\0",
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
   * Since: 0.7.2
   */
  param_spec = g_param_spec_pointer("pivot-table\0",
				    "pivot table representation\0",
				    "The original pivot table representation\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PIVOT_TABLE,
				  param_spec);
}

void
ags_function_init(AgsFunction *function)
{
  function->flags = 0;
  
  function->is_pushing = TRUE;
  function->equation = NULL;
  function->equation_count = 0;

  function->source_function = NULL;

  function->normalized_function = NULL;

  function->symbol = NULL;
  function->symbol_count = 0;

  function->n_cols = 0;
  function->n_rows = 0;
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

  function = AGS_FUNCTION(gobject);

  switch(prop_id){
  case PROP_SOURCE_FUNCTION:
    {
      gchar *source_function;

      source_function = (gchar *) g_value_get_string(value);

      if(function->source_function == source_function){
	return;
      }
      
      if(function->source_function != NULL){
	g_free(function->source_function);
      }

      function->source_function = g_strdup(source_function);
    }
    break;
  case PROP_NORMALIZED_FUNCTION:
    {
      gchar *normalized_function;

      normalized_function = (gchar *) g_value_get_string(value);

      if(function->normalized_function == normalized_function){
	return;
      }
      
      if(function->normalized_function != NULL){
	g_free(function->normalized_function);
      }

      function->normalized_function = g_strdup(normalized_function);
    }
    break;
  case PROP_PIVOT_TABLE:
    {
      AgsComplex ***pivot_table;

      pivot_table = (AgsComplex ***) g_value_get_pointer(value);

      if(pivot_table == function->pivot_table){
	return;
      }

      function->pivot_table = pivot_table;
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

  function = AGS_FUNCTION(gobject);

  switch(prop_id){
  case PROP_SOURCE_FUNCTION:
    g_value_set_string(value, function->source_function);
    break;
  case PROP_NORMALIZED_FUNCTION:
    g_value_set_string(value, function->normalized_function);
    break;
  case PROP_PIVOT_TABLE:
    g_value_set_pointer(value, function->pivot_table);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_function_finalize(GObject *gobject)
{
  AgsFunction *function;
  guint i, j;
  
  function = AGS_FUNCTION(gobject);

  if(function->equation != NULL){
    for(i = 0; i < function->equation_count; i++){
      free(function->equation[i]);
    }

    free(function->equation);
  }

  if(function->source_function != NULL){
    free(function->source_function);
  }

  if(function->normalized_function != NULL){
    free(function->normalized_function);
  }

  if(function->symbol != NULL){
    for(i = 0; i < function->symbol_count; i++){
      free(function->symbol[i]);
    }

    free(function->symbol);
  }

  if(function->pivot_table != NULL){
    for(i = 0; i < function->n_rows; i++){
      for(j = 0; j < function->n_cols; j++){
	ags_complex_free(function->pivot_table[i][j]);
      }

      free(function->pivot_table[i]);
    }

    free(function->pivot_table);
  }
}

/**
 * ags_function_find_literals:
 * @function: The #AgsFunction
 * @n_symbols: return location of symbols count
 *
 * Find literal symbols i.e. variable names.
 *
 * Returns: The string vector containing symbols
 *
 * Since: 0.7.2
 */
gchar**
ags_function_find_literals(AgsFunction *function,
			   guint *n_symbols)
{ 
  regmatch_t match_arr[1];

  gchar **literals;
  gchar *str;
  
  guint n_literals;
  
  static gboolean regex_compiled = FALSE;

  static regex_t literal_regex;

  static const char *literal_pattern = "^((?!log|exp|floor|ceil|round|sin|cos|tan|asin|acos|atan)([a-xA-X][0-9]*))\0";

  static const size_t max_matches = 1;

  literals = NULL;
  n_literals = 0;
  
  /* compile regex */
  if(!regex_compiled){
    regex_compiled = TRUE;

    regcomp(&literal_regex, literal_pattern, REG_EXTENDED);
  }

  /* find literals */
  str = function->source_function;
  
  while(str != NULL && *str != '\0'){
    if(regexec(&literal_regex, str, max_matches, match_arr, 0) == 0){
      literals = (gchar **) malloc((n_literals + 1) * sizeof(gchar *));

      literals[n_literals] = g_strndup(match_arr[0].rm_so,
				       match_arr[0].rm_eo - match_arr[0].rm_so);
      n_literals++;

      if(match_arr[0].rm_eo != '\0'){
	str = match_arr[0].rm_eo + 1;
      }else{
	break;
      }
    }else{
      break;
    }
  }

  /* return symbols and its count*/
  if(n_symbols != NULL){
    *n_symbols = n_literals;
  }

  return(literals);
}

/**
 * ags_function_literal_solve:
 * @function: the #AgsFunction
 * 
 * Solves :source-function literally, allocates the pivot table and
 * creates the normalized function.
 * 
 * Since: 0.7.2
 */
void
ags_function_literal_solve(AgsFunction *function)
{
  gchar *normalized_function;
  
  guint max_exponent, available_exponent;
  guint i, j;
  
  auto guint ags_function_literal_solve_find_max_exponent(gchar *source_function);

  guint ags_function_literal_solve_find_max_exponent(gchar *source_function){
    max_exponent = 0;


    return(max_exponent);
  }

  /* compute dimensions */
  max_exponent = function->symbol_count;
  available_exponent = ags_function_literal_solve_find_max_exponent(function->source_function);

  if(max_exponent < available_exponent){
    max_exponent = available_exponent;
  }
  
  /* allocate pivot table and function vector table */
  function->n_rows = function->symbol_count * max_exponent;
  function->n_cols = function->n_rows + 1;

  function->pivot_table = (AgsComplex ***) malloc(function->n_rows * sizeof(AgsComplex **));

  for(i = 0; i < function->n_rows; i++){
    function->pivot_table[i]  = (AgsComplex **) malloc(function->n_cols * sizeof(AgsComplex *));
    
    for(j = 0; j < function->n_cols; j++){
      function->pivot_table[i][j] = ags_complex_alloc();
    }
  }
  
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
 * Since: 0.7.2
 */
gboolean
ags_function_push_equation(AgsFunction *function,
			   gchar *equation)
{
  guint i;
  
  if(function == NULL){
    return(FALSE);
  }

  i = function->equation_count;
  
  if(function->equation_count == 0){
    function->equation = (gchar **) malloc(sizeof(gchar*));
  }else{
    function->equation = (gchar **) realloc(function->equation,
					    (i + 1) * sizeof(gchar*));
  }

  function->equation[i] = equation;
  function->equation_count += 1;
  
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
 * Since: 0.7.2
 */
void
ags_function_pop_equation(AgsFunction *function,
			  GError **error)
{
  if(function == NULL){
    return;
  }

  //TODO:JK: compute merged
  
  function->symbol = ags_function_find_literals(function,
						&(function->symbol_count));
  ags_function_literal_solve(function);
  
  function->is_pushing = FALSE;
}

/**
 * ags_function_get_expanded:
 * @function: the #AgsFunction
 * @symbols: the symbols to compute
 * @n_symbols: the count of symbols
 * 
 * Expands @symbols to normalized form.
 *
 * Returns: the normalized form as string
 *
 * Since: 0.7.2
 */
gchar*
ags_function_get_expanded(AgsFunction *function,
			  gchar **symbols,
			  guint count)
{
  //TODO:JK: implement me
  
  return(NULL);
}

/**
 * ags_funciton_get_normalized:
 * @function: the #AgsFunction
 *
 * Get internal normalized string.
 *
 * Returns: the normalized string
 *
 * Since: 0.7.2
 */
gchar*
ags_funciton_get_normalized(AgsFunction *function)
{
  if(function == NULL){
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
 * Since: 0.7.2
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
 * Since: 0.7.2
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
 * Verify :source-function to be %TRUE or %FALSE by substiution.
 *
 * Returns: %TRUE if function evaluates, otherwise %FALSE
 * 
 * Since: 0.7.2
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
 * Since: 0.7.2
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
 * ags_function_new:
 * @source_function: the source function
 *
 * Instantiate a new #AgsFunction.
 *
 * Returns: the new instance
 *
 * Since: 0.7.2
 */
AgsFunction*
ags_function_new(gchar *source_function)
{
  AgsFunction *function;
  
  function = g_object_new(AGS_TYPE_FUNCTION,
			  "source-function\0", source_function,
			  NULL);

  return(function);
}
