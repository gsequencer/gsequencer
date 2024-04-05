/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/lib/ags_solver_matrix.h>

#include <ags/lib/ags_string_util.h>
#include <ags/lib/ags_solver_path.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_solver_matrix_class_init(AgsSolverMatrixClass *solver_matrix);
void ags_solver_matrix_init (AgsSolverMatrix *solver_matrix);
void ags_solver_matrix_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_solver_matrix_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_solver_matrix_finalize(GObject *gobject);

gchar** ags_solver_matrix_to_symbolic_string(AgsSolverMatrix *solver_matrix,
					     guint *symbolic_strv_length);

void ags_solver_matrix_solve_all_by_column(AgsSolverMatrix *solver_matrix,
					   guint *nth_column, guint column_size);
void ags_solver_matrix_solve_symbolic(AgsSolverMatrix *solver_matrix);

/**
 * SECTION:ags_solver_matrix
 * @short_description: solver matrix
 * @title: AgsSolverMatrix
 * @section_id:
 * @include: ags/lib/ags_solver_matrix.h
 *
 * The #AgsSolverMatrix contains the solver vectors.
 */

enum{
  PROP_0,
  PROP_SOURCE_FUNCTION,
  PROP_ROW_COUNT,
  PROP_COLUMN_COUNT,
  PROP_SOLVER_PATH,
};

static gpointer ags_solver_matrix_parent_class = NULL;

GType
ags_solver_matrix_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_solver_matrix = 0;

    static const GTypeInfo ags_solver_matrix_info = {
      sizeof (AgsSolverMatrixClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_solver_matrix_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSolverMatrix),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_solver_matrix_init,
    };

    ags_type_solver_matrix = g_type_register_static(G_TYPE_OBJECT,
						    "AgsSolverMatrix",
						    &ags_solver_matrix_info,
						    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_solver_matrix);
  }

  return g_define_type_id__volatile;
}

void
ags_solver_matrix_class_init(AgsSolverMatrixClass *solver_matrix)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_solver_matrix_parent_class = g_type_class_peek_parent(solver_matrix);

  /* GObjectClass */
  gobject = (GObjectClass *) solver_matrix;

  gobject->set_property = ags_solver_matrix_set_property;
  gobject->get_property = ags_solver_matrix_get_property;
  
  gobject->finalize = ags_solver_matrix_finalize;

  /* properties */
  /**
   * AgsSolverMatrix:source-function:
   *
   * The assigned source-function.
   * 
   * Since: 3.9.3
   */
  param_spec = g_param_spec_string("source-function",
				   i18n_pspec("source function of solver matrix"),
				   i18n_pspec("The source function this solver matrix is derived from"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOURCE_FUNCTION,
				  param_spec);

  /**
   * AgsSolverMatrix:row-count:
   *
   * The row count of the term table.
   * 
   * Since: 3.9.3
   */
  param_spec = g_param_spec_uint("row-count",
				 i18n_pspec("row count of solver matrix"),
				 i18n_pspec("The row count this solver matrix has in term table"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ROW_COUNT,
				  param_spec);


  /**
   * AgsSolverMatrix:column-count:
   *
   * The column count of the term table.
   * 
   * Since: 3.9.3
   */
  param_spec = g_param_spec_uint("column-count",
				 i18n_pspec("column count of solver matrix"),
				 i18n_pspec("The column count this solver matrix has in term table"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_COLUMN_COUNT,
				  param_spec);

  /**
   * AgsSolverMatrix:solver-path:
   *
   * The assigned solver path.
   * 
   * Since: 6.7.1
   */
  param_spec = g_param_spec_pointer("solver-path",
				    i18n_pspec("solver path"),
				    i18n_pspec("The solver path"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOLVER_PATH,
				  param_spec);
}

void
ags_solver_matrix_init(AgsSolverMatrix *solver_matrix)
{
  solver_matrix->flags = 0;
  
  g_rec_mutex_init(&(solver_matrix->obj_mutex));

  solver_matrix->function_history = NULL;
  
  solver_matrix->source_function = NULL;
  
  solver_matrix->term_table = NULL;

  solver_matrix->row_count = 0;
  solver_matrix->column_count = 0;

  solver_matrix->solver_path = NULL;
}

void
ags_solver_matrix_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsSolverMatrix *solver_matrix;

  GRecMutex *solver_matrix_mutex;

  solver_matrix = AGS_SOLVER_MATRIX(gobject);

  /* solver matrix mutex */
  solver_matrix_mutex = AGS_SOLVER_MATRIX_GET_OBJ_MUTEX(solver_matrix);

  switch(prop_id){
  case PROP_SOURCE_FUNCTION:
  {
    gchar *source_function;

    source_function = (gchar *) g_value_get_string(value);

    g_rec_mutex_lock(solver_matrix_mutex);

    if(solver_matrix->source_function == source_function){
      g_rec_mutex_unlock(solver_matrix_mutex);

      return;
    }
      
    if(solver_matrix->source_function != NULL){
      g_free(solver_matrix->source_function);
    }

    solver_matrix->source_function = g_strdup(source_function);

    g_rec_mutex_unlock(solver_matrix_mutex);
  }
  break;
  case PROP_SOLVER_PATH:
  {
    GList *solver_path;

    solver_path = (GList *) g_value_get_pointer(value);

    g_rec_mutex_lock(solver_matrix_mutex);

    if(solver_matrix->solver_path == solver_path){
      g_rec_mutex_unlock(solver_matrix_mutex);

      return;
    }
    
    solver_matrix->solver_path = solver_path;

    g_rec_mutex_unlock(solver_matrix_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_solver_matrix_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsSolverMatrix *solver_matrix;

  GRecMutex *solver_matrix_mutex;

  solver_matrix = AGS_SOLVER_MATRIX(gobject);

  /* solver matrix mutex */
  solver_matrix_mutex = AGS_SOLVER_MATRIX_GET_OBJ_MUTEX(solver_matrix);

  switch(prop_id){
  case PROP_SOURCE_FUNCTION:
  {
    g_rec_mutex_lock(solver_matrix_mutex);

    g_value_set_string(value, solver_matrix->source_function);

    g_rec_mutex_unlock(solver_matrix_mutex);
  }
  break;
  case PROP_ROW_COUNT:
  {
    g_rec_mutex_lock(solver_matrix_mutex);

    g_value_set_uint(value, solver_matrix->row_count);

    g_rec_mutex_unlock(solver_matrix_mutex);
  }
  break;
  case PROP_COLUMN_COUNT:
  {
    g_rec_mutex_lock(solver_matrix_mutex);

    g_value_set_uint(value, solver_matrix->column_count);

    g_rec_mutex_unlock(solver_matrix_mutex);
  }
  break;
  case PROP_SOLVER_PATH:
  {
    g_rec_mutex_lock(solver_matrix_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(solver_matrix->solver_path,
					 (GCopyFunc) ags_solver_path_copy,
					 NULL));

    g_rec_mutex_unlock(solver_matrix_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_solver_matrix_finalize(GObject *gobject)
{
  AgsSolverMatrix *solver_matrix;

  guint i;
  
  solver_matrix = AGS_SOLVER_MATRIX(gobject);

  g_free(solver_matrix->source_function);

  for(i = 0; i < solver_matrix->column_count; i++){
    g_object_unref(solver_matrix->term_table[i]);
  }

  g_free(solver_matrix->term_table);

  g_list_free_full(solver_matrix->solver_path,
		   (GDestroyNotify) ags_solver_path_free);
  
  /* call parent */
  G_OBJECT_CLASS(ags_solver_matrix_parent_class)->finalize(gobject);
}

/**
 * ags_solver_matrix_get_source_function:
 * @solver_matrix: the #AgsSolverMatrix
 * 
 * Get source function of @solver_matrix.
 * 
 * Returns: the source function
 * 
 * Since: 3.9.3
 */
gchar*
ags_solver_matrix_get_source_function(AgsSolverMatrix *solver_matrix)
{
  gchar *source_function;
  
  if(!AGS_IS_SOLVER_MATRIX(solver_matrix)){
    return(NULL);
  }

  source_function = NULL;
  
  g_object_get(solver_matrix,
	       "source-function", &source_function,
	       NULL);

  return(source_function);
}

/**
 * ags_solver_matrix_set_source_function:
 * @solver_matrix: the #AgsSolverMatrix
 * @source_function: the source function
 * 
 * Set @source_function of @solver_matrix.
 * 
 * Since: 3.9.3
 */
void
ags_solver_matrix_set_source_function(AgsSolverMatrix *solver_matrix,
				      gchar *source_function)
{
  if(!AGS_IS_SOLVER_MATRIX(solver_matrix)){
    return;
  }

  g_object_set(solver_matrix,
	       "source-function", source_function,
	       NULL);
}

/**
 * ags_solver_matrix_get_row_count:
 * @solver_matrix: the #AgsSolverMatrix
 * 
 * Get row count of @solver_matrix.
 * 
 * Returns: the row count
 * 
 * Since: 3.9.3
 */
guint
ags_solver_matrix_get_row_count(AgsSolverMatrix *solver_matrix)
{
  guint row_count;
  
  if(!AGS_IS_SOLVER_MATRIX(solver_matrix)){
    return(0);
  }

  row_count = 0;

  g_object_get(solver_matrix,
	       "row-count", &row_count,
	       NULL);

  return(row_count);
}

/**
 * ags_solver_matrix_get_source_function:
 * @solver_matrix: the #AgsSolverMatrix
 * 
 * Get column count of @solver_matrix.
 * 
 * Returns: the column count
 * 
 * Since: 3.9.3
 */
guint
ags_solver_matrix_get_column_count(AgsSolverMatrix *solver_matrix)
{
  guint column_count;
  
  if(!AGS_IS_SOLVER_MATRIX(solver_matrix)){
    return(0);
  }

  column_count = 0;

  g_object_get(solver_matrix,
	       "column-count", &column_count,
	       NULL);

  return(column_count);
}

/**
 * ags_solver_matrix_get_solver_path:
 * @solver_matrix: the #AgsSolverMatrix
 * 
 * Get solver path of @solver_matrix.
 * 
 * Returns: (transfer full): the #GList-struct containing #AgsSolverPath
 * 
 * Since: 6.7.1
 */
GList*
ags_solver_matrix_get_solver_path(AgsSolverMatrix *solver_matrix)
{
  GList *solver_path;
  
  if(!AGS_IS_SOLVER_MATRIX(solver_matrix)){
    return(NULL);
  }

  solver_path = NULL;

  g_object_get(solver_matrix,
	       "solver-path", &solver_path,
	       NULL);

  return(solver_path);
}

/**
 * ags_solver_matrix_set_solver_path:
 * @solver_matrix: the #AgsSolverMatrix
 * @solver_path: (transfer full): the solver path
 * 
 * Get column count of @solver_matrix.
 * 
 * Since: 6.7.1
 */
void
ags_solver_matrix_set_solver_path(AgsSolverMatrix *solver_matrix,
				  GList *solver_path)
{
  if(!AGS_IS_SOLVER_MATRIX(solver_matrix)){
    return;
  }

  g_object_set(solver_matrix,
	       "solver-path", &solver_path,
	       NULL);
}

/**
 * ags_solver_matrix_to_string:
 * @solver_matrix: the #AgsSolverMatrix
 * 
 * To string of @solver_matrix.
 * 
 * Returns: (transfer full): the new string representation
 * 
 * Since: 5.5.1
 */
gchar*
ags_solver_matrix_to_string(AgsSolverMatrix *solver_matrix)
{
  gchar **function_history;
  gchar *str;
  gchar *function_str;
  gchar *iter;

  guint function_row_count;
  guint function_str_length;
  guint column_count;
  guint x, y;
  guint i;
  
  str = NULL;

  function_history = solver_matrix->function_history;

  if(function_history != NULL && function_history[0] != NULL){
    for(; function_history[1] != NULL; function_history++);
  }

  if(function_history == NULL ||
     function_history[0] == NULL){
    return(NULL);
  }

  iter =
    function_str = function_history[0];
  
  function_str_length = strlen(function_str);

  function_row_count = 0;

  while((iter = strchr(iter, '\n')) != NULL){
    function_row_count++;
    iter++;
  }

  iter = function_str;
    
  if((iter = strrchr(iter, '\n')) != NULL){
    iter++;

    if(iter < function_str + function_str_length){
      if(strstr(iter, " = 0") != NULL){
	function_row_count++;
      }
    }
  }
  
  column_count = ags_solver_matrix_get_column_count(solver_matrix);
  
  x = 0;
  y = 0;
  
  for(i = 0; i < function_row_count * column_count; i++){
    AgsSolverVector *solver_vector;
    AgsSolverPolynomial *solver_polynomial;

    gchar *polynomial;
    gchar *coefficient;
    
    GRecMutex *solver_matrix_mutex;
    GRecMutex *solver_vector_mutex;

    /*  */
    solver_matrix_mutex = AGS_SOLVER_MATRIX_GET_OBJ_MUTEX(solver_matrix);

    g_rec_mutex_lock(solver_matrix_mutex);
    
    solver_vector = solver_matrix->term_table[x];

    g_rec_mutex_unlock(solver_matrix_mutex);

    /*  */
    solver_vector_mutex = AGS_SOLVER_VECTOR_GET_OBJ_MUTEX(solver_vector);

    g_rec_mutex_lock(solver_vector_mutex);
    
    solver_polynomial = solver_vector->polynomial_column[y];

    g_rec_mutex_unlock(solver_vector_mutex);
    
    /*  */
    polynomial = ags_solver_polynomial_get_polynomial(solver_polynomial);

    if(polynomial != NULL){
      if(str == NULL){
	str = g_strdup_printf("%s", polynomial);
      }else{
	gchar *tmp;

	tmp = str;
	
	coefficient = ags_solver_polynomial_get_coefficient(solver_polynomial);
	
	if(coefficient != NULL && coefficient[0] == '-'){
	  str = g_strdup_printf("%s - %s", str, coefficient);
	}else{
	  str = g_strdup_printf("%s + %s", str, coefficient);
	}

	g_free(tmp);
	g_free(coefficient);
      }

      g_free(polynomial);
    }
    
    /* iterate */
    x++;

    if(x >= column_count){
      y++;
      x = 0;

      if(str != NULL){
	gchar *tmp;

	tmp = str;

	str = g_strdup_printf("%s = 0\n", str);
	
	g_free(tmp);
      }
    }
  }
  
  return(str);
}

/**
 * ags_solver_matrix_to_symbolic_string:
 * @solver_matrix: the #AgsSolverMatrix
 * 
 * To symbolic string of @solver_matrix.
 * 
 * Returns: (transfer full): the new symbolic string representation
 * 
 * Since: 5.5.1
 */
gchar**
ags_solver_matrix_to_symbolic_string(AgsSolverMatrix *solver_matrix,
				     guint *symbolic_strv_length)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_solver_matrix_parse:
 * @solver_matrix: the #AgsSolverMatrix
 * @source_function: the normalized source function
 * 
 * Parse @source_function and apply to @solver_matrix.
 * 
 * Since: 3.9.3
 */
void
ags_solver_matrix_parse(AgsSolverMatrix *solver_matrix,
			gchar *source_function)
{
  gchar *iter, *prev;
  gchar *equal_sign_offset;
  gchar *semicolon_sign_offset;

  guint nth_row;
  
  GRecMutex *solver_matrix_mutex;
  
  if(!AGS_IS_SOLVER_MATRIX(solver_matrix) ||
     source_function == NULL){
    return;
  }

  solver_matrix_mutex = AGS_SOLVER_MATRIX_GET_OBJ_MUTEX(solver_matrix);

  iter = source_function;
  prev = iter;

  equal_sign_offset = NULL;
  semicolon_sign_offset = NULL;

  nth_row = 0;
  
  while(iter < source_function + strlen(source_function) + 1){
    if(iter[0] == '('){
      gchar *tmp_iter;
      
      gint parenthesis_balance;

      tmp_iter = iter + 1;
      parenthesis_balance = 1;

      while(tmp_iter[0] != '\0' &&
	    parenthesis_balance > 0){
	if(tmp_iter[0] == '('){
	  parenthesis_balance++;
	}else if(tmp_iter[0] == ')'){
	  parenthesis_balance--;
	}

	tmp_iter++;
      }

      if(parenthesis_balance == 0){
	iter = tmp_iter;
      }else{
	g_warning("can't parse because malformed parenthesis");

	break;
      }
    }

    if(iter[0] == ')'){
      g_warning("can't parse because malformed parenthesis");

      break;
    }

    if(iter != prev){
      gboolean success;

      success = FALSE;
      
      if(iter[0] == '+'){
	success = TRUE;
      }
      
      if(iter[0] == '-'){
	success = TRUE;
      }

      if(iter[0] == '='){
	equal_sign_offset = iter;
      }

      if(iter[0] == ';'){
	success = TRUE;
      }

      if(iter[0] == '\0'){
	success = TRUE;
      }
      
      if(success){
	AgsSolverVector *solver_vector;
	AgsSolverVector *current_solver_vector;
	AgsSolverPolynomial *solver_polynomial;	
	AgsSolverPolynomial *current_solver_polynomial;
	
	gchar **symbol;
	gchar **exponent;

	gchar *polynomial;

	guint column_count;
	guint numeric_column;
	guint polynomial_count;
	guint i;
	  
	GRecMutex *current_solver_vector_mutex;
	
	g_object_get(solver_matrix,
		     "column-count", &column_count,
		     NULL);
	
	solver_polynomial = ags_solver_polynomial_new();

	polynomial = g_strndup(prev,
			       iter - prev);
	
	ags_solver_polynomial_parse(solver_polynomial,
				    polynomial);

	symbol = NULL;
	exponent = NULL;

	g_object_get(solver_polynomial,
		     "symbol", &symbol,
		     "exponent", &exponent,
		     NULL);
	
	solver_vector = NULL;
	current_solver_vector = NULL;
	
	numeric_column = -1;
	
	for(i = 0; i < column_count; i++){
	  gchar **current_symbol;
	  gchar **current_exponent;

	  /*  get current solver vector */
	  g_rec_mutex_lock(solver_matrix_mutex);
	  
	  current_solver_vector = solver_matrix->term_table[i];

	  g_object_ref(current_solver_vector);
	  
	  g_rec_mutex_unlock(solver_matrix_mutex);

	  /*  get current solver vector mutex */
	  current_solver_vector_mutex = AGS_SOLVER_VECTOR_GET_OBJ_MUTEX(current_solver_vector);

	  /*  get current solver polynomial */
	  g_rec_mutex_lock(current_solver_vector_mutex);
	  
	  current_solver_polynomial = (current_solver_vector->polynomial_column != NULL) ? current_solver_vector->polynomial_column[0]: NULL;

	  g_object_ref(current_solver_polynomial);

	  g_rec_mutex_unlock(current_solver_vector_mutex);

	  /* compare polynomial */
	  current_symbol = NULL;
	  current_exponent = NULL;
	  
	  g_object_get(current_solver_polynomial,
		       "symbol", &current_symbol,
		       "exponent", &current_exponent,
		       NULL);

	  if(symbol == NULL &&
	     current_symbol == NULL){
	    solver_vector = current_solver_vector;

	    g_object_ref(solver_vector);
	  }else if(ags_strv_equal(symbol, current_symbol) &&
		   ags_strv_equal(exponent, current_exponent)){
	    solver_vector = current_solver_vector;

	    g_object_ref(solver_vector);
	  }

	  if(current_symbol == NULL){
	    numeric_column = i;
	  }
	  
	  g_object_unref(current_solver_vector);
	  g_object_unref(current_solver_polynomial);
	  
	  g_strfreev(current_symbol);
	  g_strfreev(current_exponent);
	}
	
	if(solver_vector == NULL){
	  gint position;
	  guint i, j;
	  gboolean success;	  
	  
	  GRecMutex *solver_polynomial_mutex;
	  GRecMutex *current_solver_polynomial_mutex;
	  
	  position = -1;

	  success = FALSE;

	  solver_polynomial_mutex = AGS_SOLVER_POLYNOMIAL_GET_OBJ_MUTEX(solver_polynomial);

	  if(symbol != NULL){
	    for(i = 0; i < column_count && !success; i++){
	      gchar **current_symbol;

	      double _Complex exp_val, current_exp_val;
	    
	      /*  get current solver vector */
	      g_rec_mutex_lock(solver_matrix_mutex);
	  
	      current_solver_vector = solver_matrix->term_table[i];

	      g_object_ref(current_solver_vector);
	  
	      g_rec_mutex_unlock(solver_matrix_mutex);

	      /*  get current solver vector mutex */
	      current_solver_vector_mutex = AGS_SOLVER_VECTOR_GET_OBJ_MUTEX(current_solver_vector);

	      /*  get current solver polynomial */
	      g_rec_mutex_lock(current_solver_vector_mutex);
	  
	      current_solver_polynomial = (current_solver_vector->polynomial_column != NULL) ? current_solver_vector->polynomial_column[0]: NULL;

	      g_object_ref(current_solver_polynomial);

	      g_rec_mutex_unlock(current_solver_vector_mutex);

	      current_solver_polynomial_mutex = AGS_SOLVER_POLYNOMIAL_GET_OBJ_MUTEX(current_solver_polynomial);
	    
	      /* compare polynomial */
	      current_symbol = NULL;
	  
	      g_object_get(current_solver_polynomial,
			   "symbol", &current_symbol,
			   NULL);

	      /* insert sorted */
	      j = 0;
	    
	      if(current_symbol != NULL){
		for(; symbol[j] != NULL && current_symbol[j] != NULL; j++){
		  if(g_strcmp0(symbol[j], current_symbol[j]) < 0.0){
		    position = i;
		
		    success = TRUE;

		    break;
		  }

		  g_rec_mutex_lock(solver_polynomial_mutex);

		  exp_val = ags_complex_get(solver_polynomial->exponent_value + j);

		  g_rec_mutex_unlock(solver_polynomial_mutex);

		  g_rec_mutex_lock(current_solver_polynomial_mutex);

		  current_exp_val = ags_complex_get(current_solver_polynomial->exponent_value + j);
	      
		  g_rec_mutex_unlock(current_solver_polynomial_mutex);
	      
		  if(!g_strcmp0(symbol[j], current_symbol[j]) &&
		     cabs(exp_val) > cabs(current_exp_val)){
		    position = i;
		
		    success = TRUE;

		    break;
		  }	      
		}

		if(!success &&
		   (current_symbol == NULL || (symbol[j] != NULL && current_symbol[j] == NULL))){
		  position = i;
		
		  success = TRUE;
		}

		g_object_unref(current_solver_vector);
		g_object_unref(current_solver_polynomial);

		g_strfreev(current_symbol);
	      }
	    }
	  }
	  
	  solver_vector = ags_solver_vector_new();
	  g_object_set(solver_vector,
		       "source-polynomial", polynomial,
		       NULL);
	  
	  ags_solver_matrix_insert_vector(solver_matrix,
					  solver_vector,
					  position);

	  current_solver_polynomial = NULL;
	}

	polynomial_count = 0;

	g_object_get(solver_vector,
		     "polynomial-count", &polynomial_count,
		     NULL);
	
	if(nth_row < polynomial_count){
	  AgsSolverPolynomial *polynomial_a, *polynomial_b, *polynomial_c;

	  GError *error;
	  
	  error = NULL;

	  polynomial_a = current_solver_polynomial;
	  polynomial_b = solver_polynomial;
	  
	  if(equal_sign_offset == NULL ||
	     equal_sign_offset < semicolon_sign_offset){
	    polynomial_c = ags_solver_polynomial_add(polynomial_a,
						     polynomial_b,
						     &error);	  
	  }else{
	    polynomial_c = ags_solver_polynomial_subtract(polynomial_a,
							  polynomial_b,
							  &error);	  
	  }

	  /*  get current solver vector mutex */
	  current_solver_vector_mutex = AGS_SOLVER_VECTOR_GET_OBJ_MUTEX(current_solver_vector);
	  
	  g_rec_mutex_lock(current_solver_vector_mutex);
	  
	  solver_vector->polynomial_column[nth_row] = polynomial_c;
	  g_object_ref(polynomial_c);

	  g_rec_mutex_unlock(current_solver_vector_mutex);

	  current_solver_polynomial = polynomial_c;	  

	  g_object_unref(polynomial_a);
	}else{
	  ags_solver_vector_insert_polynomial(solver_vector,
					      solver_polynomial,
					      -1);
	}
	
	g_strfreev(symbol);
	g_strfreev(exponent);

	g_free(polynomial);

	prev = iter;
      }
      
      if(iter[0] == ';'){
	semicolon_sign_offset = iter;
	iter++;

	prev = iter;	
	nth_row++;
      }
    }
      
    if(iter[0] == '\0'){
      break;
    }
    
    iter++;
  }

  g_rec_mutex_lock(solver_matrix_mutex);

  solver_matrix->row_count = nth_row;

  g_rec_mutex_unlock(solver_matrix_mutex);
  
  g_object_set(solver_matrix,
	       "source-function", source_function,
	       NULL);
}

/**
 * ags_solver_matrix_insert_vector:
 * @solver_matrix: the #AgsSolverMatrix
 * @solver_vector: the #AgsSolverVector
 * @position: the position
 * 
 * Insert @solver_vector to @solver_matrix.
 * 
 * Since: 3.2.0
 */
void
ags_solver_matrix_insert_vector(AgsSolverMatrix *solver_matrix,
				AgsSolverVector *solver_vector,
				gint position)
{
  guint i, j;

  GRecMutex *solver_matrix_mutex;
  
  if(!AGS_IS_SOLVER_MATRIX(solver_matrix) ||
     !AGS_IS_SOLVER_VECTOR(solver_vector)){
    return;
  }

  solver_matrix_mutex = AGS_SOLVER_MATRIX_GET_OBJ_MUTEX(solver_matrix);

  g_rec_mutex_lock(solver_matrix_mutex);
  
  if(position > solver_matrix->column_count){
    position = solver_matrix->column_count;
  }
  
  if(solver_matrix->term_table == NULL){
    solver_matrix->term_table = (AgsSolverVector **) g_malloc(sizeof(AgsSolverVector *));
  }else{
    solver_matrix->term_table = (AgsSolverVector **) g_realloc(solver_matrix->term_table,
							       (solver_matrix->column_count + 1) * sizeof(AgsSolverVector *));
  }

  g_object_ref(solver_vector);
  
  if(position < 0){
    solver_matrix->term_table[solver_matrix->column_count] = solver_vector;
  }else{
    for(i = solver_matrix->column_count, j = solver_matrix->column_count - 1; i >= 0 && i > position; i--, j--){
      solver_matrix->term_table[i] = solver_matrix->term_table[j];
    }

    solver_matrix->term_table[position] = solver_vector;
  }

  solver_matrix->column_count += 1;
  
  g_rec_mutex_unlock(solver_matrix_mutex);  
}

/**
 * ags_solver_matrix_remove_vector:
 * @solver_matrix: the #AgsSolverMatrix
 * @solver_vector: the #AgsSolverVector
 * 
 * Remove @solver_vector from @solver_matrix.
 * 
 * Since: 3.2.0
 */
void
ags_solver_matrix_remove_vector(AgsSolverMatrix *solver_matrix,
				AgsSolverVector *solver_vector)
{
  gint position;
  guint i, j;
  
  GRecMutex *solver_matrix_mutex;
  
  if(!AGS_IS_SOLVER_MATRIX(solver_matrix) ||
     !AGS_IS_SOLVER_VECTOR(solver_vector)){
    return;
  }

  solver_matrix_mutex = AGS_SOLVER_MATRIX_GET_OBJ_MUTEX(solver_matrix);

  g_rec_mutex_lock(solver_matrix_mutex);

  position = -1;
  
  for(i = 0; i < solver_matrix->column_count; i++){
    if(solver_matrix->term_table[i] == solver_vector){
      position = i;

      break;
    }
  }

  if(position >= 0){
    if(solver_matrix->column_count == 1){
      g_free(solver_matrix->term_table);

      solver_matrix->term_table = NULL;
    }else{
      for(i = 0, j = 0; i < solver_matrix->column_count; i++){
	if(i > position){
	  solver_matrix->term_table[j] = solver_matrix->term_table[i];
	}

	if(i != position){
	  j++;
	}
      }
      
      solver_matrix->term_table = (AgsSolverVector **) g_realloc(solver_matrix->term_table,
								 (solver_matrix->column_count - 1) * sizeof(AgsSolverVector *));
    }

    g_object_unref(solver_vector);
  }

  solver_matrix->column_count -= 1;
  
  g_rec_mutex_unlock(solver_matrix_mutex);  
}

/**
 * ags_solver_matrix_eliminate:
 * @solver_matrix: the #AgsSolverMatrix
 * @nth_column: the nth column
 * @nth_row_a: the nth row as a
 * @nth_row_b: the nth row as b
 * 
 * Eliminate one column of @solver_matrix.
 * 
 * Since: 3.9.3
 */
void
ags_solver_matrix_eliminate(AgsSolverMatrix *solver_matrix,
			    guint nth_column,
			    guint nth_row_a, guint nth_row_b)
{
  AgsSolverVector *solver_vector;
  AgsSolverVector *current_vector;
  AgsSolverPolynomial *solver_polynomial_a, *solver_polynomial_b;
  AgsSolverPolynomial *solver_polynomial_factor;
  AgsSolverPolynomial *current_a, *current_b;
  AgsSolverPolynomial *current_b_factored;
  AgsSolverPolynomial *current_c;

  AgsComplex *a, *b;

  gchar *polynomial_factor;
  
  guint row_count, column_count;
  guint polynomial_count;
  double _Complex factor;
  guint i;
  
  GError *error;
  
  GRecMutex *solver_matrix_mutex;
  GRecMutex *solver_vector_mutex;
  GRecMutex *current_vector_mutex;

  if(!AGS_IS_SOLVER_MATRIX(solver_matrix)){
    return;
  }

  solver_matrix_mutex = AGS_SOLVER_MATRIX_GET_OBJ_MUTEX(solver_matrix);

  g_object_get(solver_matrix,
	       "row-count", &row_count,
	       "column-count", &column_count,
	       NULL);

  if(nth_column >= column_count){
    g_warning("solver matrix column index excess");
    
    return;
  }
  
  g_rec_mutex_lock(solver_matrix_mutex);

  solver_vector = solver_matrix->term_table[nth_column];
  g_object_ref(solver_vector);
    
  g_rec_mutex_unlock(solver_matrix_mutex);

  solver_vector_mutex = AGS_SOLVER_VECTOR_GET_OBJ_MUTEX(solver_vector);

  g_object_get(solver_vector,
	       "polynomial-count", &polynomial_count,
	       NULL);

  if(nth_row_a >= polynomial_count ||
     nth_row_b >= polynomial_count){  
    g_warning("solver matrix row index excess");
    
    if(solver_vector != NULL){
      g_object_unref(solver_vector);
    }

    return;
  }
  
  g_rec_mutex_lock(solver_vector_mutex);
  
  solver_polynomial_a = solver_vector->polynomial_column[nth_row_a];
  g_object_ref(solver_polynomial_a);
  
  solver_polynomial_b = solver_vector->polynomial_column[nth_row_b];
  g_object_ref(solver_polynomial_b);

  g_rec_mutex_unlock(solver_vector_mutex);

  a = ags_solver_polynomial_get_coefficient_value(solver_polynomial_a);
  b = ags_solver_polynomial_get_coefficient_value(solver_polynomial_b);

  if(a->real == 0.0 ||
     b->real == 0.0){
    g_warning("solver polynomial coefficient is 0.0");

    if(solver_vector != NULL){
      g_object_unref(solver_vector);
    }

    if(solver_polynomial_a != NULL){
      g_object_unref(solver_polynomial_a);
    }

    if(solver_polynomial_b != NULL){
      g_object_unref(solver_polynomial_b);
    }

    return;
  }
  
  factor = 1.0 / ags_complex_get(b) * ags_complex_get(a);
  
  if(cimag(factor) != 0.0){
    if(cimag(factor) >= 0.0){
      polynomial_factor = g_strdup_printf("%f+𝑖%f",
					  creal(factor), cimag(factor));
    }else{
      polynomial_factor = g_strdup_printf("%f-𝑖%f",
					  creal(factor), -1.0 * cimag(factor));
    }
  }else{
    polynomial_factor = g_strdup_printf("%f",
					creal(factor));
  }

  solver_polynomial_factor = ags_solver_polynomial_new();
  
  ags_solver_polynomial_parse(solver_polynomial_factor,
			      polynomial_factor);

  for(i = 0; i < column_count; i++){
    g_rec_mutex_lock(solver_matrix_mutex);
    
    current_vector = solver_matrix->term_table[i];
    g_object_ref(current_vector);
    
    g_rec_mutex_unlock(solver_matrix_mutex);

    current_vector_mutex = AGS_SOLVER_VECTOR_GET_OBJ_MUTEX(current_vector);

    g_rec_mutex_lock(current_vector_mutex);
  
    current_a = current_vector->polynomial_column[nth_row_a];
    g_object_ref(current_a);
  
    current_b = current_vector->polynomial_column[nth_row_b];
    g_object_ref(current_b);

    g_rec_mutex_unlock(current_vector_mutex);
    
    error = NULL;
    current_b_factored = ags_solver_polynomial_multiply(current_b,
							solver_polynomial_factor,
							&error);
  
    error = NULL;
    current_c = ags_solver_polynomial_subtract(current_a,
					       current_b_factored,
					       &error);

    ags_solver_vector_insert_polynomial(current_vector,
					current_c,
					-1);

    if(current_vector != NULL){
      g_object_unref(current_vector);
    }
    
    if(current_a != NULL){
      g_object_unref(current_a);
    }
  
    if(current_b != NULL){
      g_object_unref(current_b);
    }

    if(current_b_factored != NULL){
      g_object_unref(current_b_factored);
    }
  }
  
  if(solver_vector != NULL){
    g_object_unref(solver_vector);
  }
  
  if(solver_polynomial_a != NULL){
    g_object_unref(solver_polynomial_a);
  }
  
  if(solver_polynomial_b != NULL){
    g_object_unref(solver_polynomial_b);
  }

  if(solver_polynomial_factor != NULL){
    g_object_unref(solver_polynomial_factor);
  }
}

/**
 * ags_solver_matrix_solve_all_by_column:
 * @solver_matrix: the #AgsSolverMatrix
 * @nth_column: the nth column vector
 * @column_size: the column size
 * 
 * Solve all by column of @solver_matrix.
 * 
 * Since: 5.3.4
 */
void
ags_solver_matrix_solve_all_by_column(AgsSolverMatrix *solver_matrix,
				      guint *nth_column, guint column_size)
{
  guint column_count;
  guint row_count;
  guint i, i_stop;

  g_return_if_fail(AGS_IS_SOLVER_MATRIX(solver_matrix));
  g_return_if_fail(nth_column != NULL);
  g_return_if_fail(column_size > 0);
  
  column_count = ags_solver_matrix_get_column_count(solver_matrix);
  row_count = ags_solver_matrix_get_row_count(solver_matrix);

  i_stop = column_size;
  
  for(i = 0; i < i_stop; i++){
    guint current_column, current_row_a, current_row_b;

    current_column = nth_column[i % column_size];
    
    current_row_a = (guint) floor(i / column_count);
    current_row_b = (guint) floor(i / column_count) + (row_count - ((i + 1) % row_count));
    
    g_message("solve [%d] -> x,y [%d, %d %d]", i, current_column, current_row_a, current_row_b);
  }
}

/**
 * ags_solver_matrix_solve_default:
 * @solver_matrix: the #AgsSolverMatrix
 *
 * Default solve.
 * 
 * Since: 5.3.4 
 */
void
ags_solver_matrix_solve_default(AgsSolverMatrix *solver_matrix)
{
  guint *nth_column;

  guint column_count;
  guint column_size;
  guint i, i_stop;
  guint j;

  g_return_if_fail(AGS_IS_SOLVER_MATRIX(solver_matrix));
  
  column_count = ags_solver_matrix_get_column_count(solver_matrix);

  column_size = (column_count - 1) * (column_count - 2) / 2;

  nth_column = g_malloc(column_size * sizeof(guint));

  g_message("column_count = %d, column_size = %d", column_count, column_size);

  i_stop = column_size;

  for(i = 0, j = 0; i < i_stop; i++, j++){
    if(j == column_count - 2){
      j = 0;
    }

    if(i % (column_count - 2)  == column_count - 2){
      j++;
    }

    if(i == j){
      j++;
    }
    
    //    nth_column[i] = ((column_count - 1) * (guint) floor((double) i / (double) (column_count - 1)) - (i - ((column_count - 1) * (guint) floor((double) i / (double) (column_count - 1)))) + 1) % (column_count - 1);
    nth_column[i] = j;

    g_message("nth_column[i] = %d", nth_column[i]);
  }

  ags_solver_matrix_solve_all_by_column(solver_matrix,
					nth_column, column_size);
}

/**
 * ags_solver_matrix_solve_symbolic:
 * @solver_matrix: the #AgsSolverMatrix
 *
 * Symbolic solve.
 * 
 * Since: 5.5.1 
 */
void
ags_solver_matrix_solve_symbolic(AgsSolverMatrix *solver_matrix)
{
  //TODO:JK: implement me
}

/**
 * ags_solver_matrix_new:
 *
 * Instantiate a new #AgsSolverMatrix.
 *
 * Returns: the new instance
 *
 * Since: 3.0.0
 */
AgsSolverMatrix*
ags_solver_matrix_new()
{
  AgsSolverMatrix *solver_matrix;
  
  solver_matrix = g_object_new(AGS_TYPE_SOLVER_MATRIX,
			       NULL);

  return(solver_matrix);
}
