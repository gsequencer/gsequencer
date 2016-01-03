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

enum{
  PROP_0,
  PROP_SOURCE_FUNCTION,
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

    ags_type_function = g_type_register_static(G_TYPE_OBJECT,
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
}

void
ags_function_init(AgsFunction *function)
{
  function->flags = 0;
  
  function->is_pushing = TRUE;
  function->equation = NULL;
  function->equation_count = 0;

  function->source_function = NULL;

  function->n_cols = 0;
  function->n_rows = 0;
  function->pivot_table = NULL;

  function->symbol = NULL;
  function->symbol_count = 0;

  function->normalized_function = NULL;
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

  if(function->pivot_table != NULL){
    for(i = 0; i < function->n_rows; i++){
      for(j = 0; j < function->n_cols; j++){
	ags_complex_free(function->pivot_table[i][j]);
      }

      free(function->pivot_table[i]);
    }

    free(function->pivot_table);
  }

  if(function->symbol != NULL){
    for(i = 0; i < function->symbol_count; i++){
      free(function->symbol[i]);
    }

    free(function->symbol);
  }

  if(function->normalized_function != NULL){
    free(function->normalized_function);
  }
}

gchar**
ags_function_find_literals(AgsFunction *function,
			   guint *n_symbols)
{
  static gboolean regex_compiled = FALSE;

  static regex_t literal_regex;

  static const char *literal_pattern = "^([a-xA-X][0-9]*)\0";

  /* compile regex */
  if(!regex_compiled){
    regcomp(&literal_regex, literal_pattern, REG_EXTENDED);
    
    regex_compiled = TRUE;
  }
  
  //TODO:JK: implement me
}

void
ags_function_literal_solve(AgsFunction *function)
{
  //TODO:JK: implement me
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

  function->symbol = ags_function_find_literals(function,
						&(function->symbol_count));
  ags_function_literal_solve(function);
  
  function->is_pushing = FALSE;
}

gchar*
ags_function_get_merged(AgsFunction *function,
			gchar **symbols,
			guint count)
{
  //TODO:JK: implement me
}

gchar*
ags_funciton_get_normalized(AgsFunction *function)
{
  if(function == NULL){
    return(NULL);
  }

  return(function->normalized_function);
}

gboolean
ags_function_substitute_values(AgsFunction *function,
			       gchar *symbol, ...)
{
  //TODO:JK: implement me
  
  return(TRUE);
}

AgsComplex*
ags_function_translate_value(AgsFunction *function,
			     AgsComplex *value)
{
  //TODO:JK: implement me
}

AgsComplex**
ags_function_symbolic_translate_value(AgsFunction *function,
				      gchar *symbol,
				      AgsComplex *value)
{
  //TODO:JK: implement me
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
