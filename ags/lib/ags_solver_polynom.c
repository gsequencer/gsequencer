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

#include <ags/lib/ags_solver_polynom.h>

#include <stdlib.h>
#include <complex.h>

#include <ags/i18n.h>

void ags_solver_polynom_class_init(AgsSolverPolynomClass *solver_polynom);
void ags_solver_polynom_init (AgsSolverPolynom *solver_polynom);
void ags_solver_polynom_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_solver_polynom_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_solver_polynom_finalize(GObject *gobject);

/**
 * SECTION:ags_solver_polynom
 * @short_description: solver polynom
 * @title: AgsSolverPolynom
 * @section_id:
 * @include: ags/lib/ags_solver_polynom.h
 *
 * The #AgsSolverPolynom contains the solver polynoms.
 */

enum{
  PROP_0,
  PROP_POLYNOM,
  PROP_COEFFICIENT,
  PROP_SYMBOL,
  PROP_EXPONENT,
  PROP_COEFFICIENT_VALUE,
  PROP_EXPONENT_VALUE,
};

static gpointer ags_solver_polynom_parent_class = NULL;

GType
ags_solver_polynom_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_solver_polynom = 0;

    static const GTypeInfo ags_solver_polynom_info = {
      sizeof (AgsSolverPolynomClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_solver_polynom_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSolverPolynom),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_solver_polynom_init,
    };

    ags_type_solver_polynom = g_type_register_static(G_TYPE_OBJECT,
						     "AgsSolverPolynom",
						     &ags_solver_polynom_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_solver_polynom);
  }

  return g_define_type_id__volatile;
}

void
ags_solver_polynom_class_init(AgsSolverPolynomClass *solver_polynom)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_solver_polynom_parent_class = g_type_class_peek_parent(solver_polynom);

  /* GObjectClass */
  gobject = (GObjectClass *) solver_polynom;

  gobject->set_property = ags_solver_polynom_set_property;
  gobject->get_property = ags_solver_polynom_get_property;
  
  gobject->finalize = ags_solver_polynom_finalize;

  /* properties */
  /**
   * AgsSolverPolynom:polynom:
   *
   * The assigned polynom.
   * 
   * Since: 3.2.0
   */
  param_spec = g_param_spec_string("polynom",
				   i18n_pspec("polynom of solver polynom"),
				   i18n_pspec("The polynom this solver polynom is assigned to"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_POLYNOM,
				  param_spec);

  /**
   * AgsSolverPolynom:coefficient:
   *
   * The assigned coefficient.
   * 
   * Since: 3.2.0
   */
  param_spec = g_param_spec_string("coefficient",
				   i18n_pspec("coefficient of solver polynom"),
				   i18n_pspec("The coefficient this solver polynom is assigned to"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COEFFICIENT,
				  param_spec);

  /**
   * AgsSolverPolynom:symbol:
   *
   * The assigned symbol.
   * 
   * Since: 3.2.0
   */
  param_spec = g_param_spec_pointer("symbol",
				    i18n_pspec("symbol of solver polynom"),
				    i18n_pspec("The symbol this solver polynom is assigned to"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYMBOL,
				  param_spec);

  /**
   * AgsSolverPolynom:exponent:
   *
   * The assigned exponent.
   * 
   * Since: 3.2.0
   */
  param_spec = g_param_spec_pointer("exponent",
				    i18n_pspec("exponent of solver polynom"),
				    i18n_pspec("The exponent this solver polynom is assigned to"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_EXPONENT,
				  param_spec);

  /**
   * AgsSolverPolynom:coefficient-value:
   *
   * Coefficient value.
   * 
   * Since: 3.2.0
   */
  param_spec = g_param_spec_boxed("coefficient-value",
				  i18n_pspec("coefficient value"),
				  i18n_pspec("The coefficient value"),
				  AGS_TYPE_COMPLEX,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COEFFICIENT_VALUE,
				  param_spec);


  /**
   * AgsSolverPolynom:exponent-value:
   *
   * Exponent value.
   * 
   * Since: 3.2.0
   */
  param_spec = g_param_spec_boxed("exponent-value",
				  i18n_pspec("exponent value"),
				  i18n_pspec("The exponent value"),
				  AGS_TYPE_COMPLEX,
				  G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_EXPONENT_VALUE,
				  param_spec);
}

GQuark
ags_solver_polynom_error_quark()
{
  return(g_quark_from_static_string("ags-solver-polynom-error-quark"));
}

void
ags_solver_polynom_init(AgsSolverPolynom *solver_polynom)
{
  complex z;

  solver_polynom->flags = 0;
  
  g_rec_mutex_init(&(solver_polynom->obj_mutex));

  solver_polynom->polynom = NULL;

  solver_polynom->coefficient = NULL;
  solver_polynom->symbol = NULL;
  solver_polynom->exponent = NULL;

  z = 1.0 + I * 0.0;
  ags_complex_set(&(solver_polynom->coefficient_value),
		  z);

  solver_polynom->exponent_value = NULL;
}

void
ags_solver_polynom_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsSolverPolynom *solver_polynom;

  GRecMutex *solver_polynom_mutex;
  
  solver_polynom = AGS_SOLVER_POLYNOM(gobject);

  /* solver polynom mutex */
  solver_polynom_mutex = AGS_SOLVER_POLYNOM_GET_OBJ_MUTEX(solver_polynom);
  
  switch(prop_id){
  case PROP_POLYNOM:
  {
    gchar *polynom;

    polynom = (gchar *) g_value_get_string(value);

    g_rec_mutex_lock(solver_polynom_mutex);

    if(solver_polynom->polynom == polynom){
      g_rec_mutex_unlock(solver_polynom_mutex);

      return;
    }
      
    if(solver_polynom->polynom != NULL){
      g_free(solver_polynom->polynom);
    }

    solver_polynom->polynom = g_strdup(polynom);

    g_rec_mutex_unlock(solver_polynom_mutex);
  }
  break;
  case PROP_COEFFICIENT:
  {
    gchar *coefficient;

    coefficient = (gchar *) g_value_get_string(value);

    g_rec_mutex_lock(solver_polynom_mutex);

    if(solver_polynom->coefficient == coefficient){
      g_rec_mutex_unlock(solver_polynom_mutex);

      return;
    }
      
    if(solver_polynom->coefficient != NULL){
      g_free(solver_polynom->coefficient);
    }

    solver_polynom->coefficient = g_strdup(coefficient);

    g_rec_mutex_unlock(solver_polynom_mutex);
  }
  break;
  case PROP_SYMBOL:
  {
    gchar *symbol;
    
    complex z;

    guint length;

    symbol = (gchar *) g_value_get_pointer(value);

    g_rec_mutex_lock(solver_polynom_mutex);

    if(g_strv_contains(solver_polynom->symbol, symbol)){
      g_rec_mutex_unlock(solver_polynom_mutex);

      return;
    }

    if(solver_polynom->symbol == NULL){
      length = 0;
      
      solver_polynom->symbol = (gchar **) g_malloc(2 * sizeof(gchar *));      
      solver_polynom->exponent = (gchar **) g_malloc(2 * sizeof(gchar *));

      solver_polynom->exponent_value = g_new(AgsComplex,
					     1);
    }else{
      length = g_strv_length(solver_polynom->symbol);
      
      solver_polynom->symbol = (gchar **) g_realloc(solver_polynom->symbol,
						    (length + 2) * sizeof(gchar *));
      solver_polynom->exponent = (gchar **) g_realloc(solver_polynom->exponent,
						      (length + 2) * sizeof(gchar *));

      solver_polynom->exponent_value = g_renew(AgsComplex,
					       solver_polynom->exponent_value,
					       (length + 1));
    }
    
    solver_polynom->symbol[length] = g_strdup(symbol);
    solver_polynom->symbol[length + 1] = NULL;

    solver_polynom->exponent[length] = g_strdup("1");
    solver_polynom->exponent[length + 1] = NULL;

    z = 1.0 + I * 0.0;
    ags_complex_set(&(solver_polynom->exponent_value[length]),
		    z);
    
    g_rec_mutex_unlock(solver_polynom_mutex);
  }
  break;
  case PROP_COEFFICIENT_VALUE:
  {
    AgsComplex *coefficient_value;

    coefficient_value = (AgsComplex *) g_value_get_boxed(value);

    g_rec_mutex_lock(solver_polynom_mutex);

    ags_complex_set(&(solver_polynom->coefficient_value),
		    ags_complex_get(coefficient_value));

    g_rec_mutex_unlock(solver_polynom_mutex);
  }
  break;    
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_solver_polynom_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsSolverPolynom *solver_polynom;

  GRecMutex *solver_polynom_mutex;

  solver_polynom = AGS_SOLVER_POLYNOM(gobject);

  /* solver polynom mutex */
  solver_polynom_mutex = AGS_SOLVER_POLYNOM_GET_OBJ_MUTEX(solver_polynom);

  switch(prop_id){
  case PROP_POLYNOM:
  {
    g_rec_mutex_lock(solver_polynom_mutex);

    g_value_set_string(value, solver_polynom->polynom);

    g_rec_mutex_unlock(solver_polynom_mutex);
  }
  break;
  case PROP_COEFFICIENT:
  {
    g_rec_mutex_lock(solver_polynom_mutex);

    g_value_set_string(value, solver_polynom->coefficient);

    g_rec_mutex_unlock(solver_polynom_mutex);
  }
  break;
  case PROP_SYMBOL:
  {
    g_rec_mutex_lock(solver_polynom_mutex);

    g_value_set_pointer(value, g_strdupv(solver_polynom->symbol));

    g_rec_mutex_unlock(solver_polynom_mutex);
  }
  break;
  case PROP_EXPONENT:
  {
    g_rec_mutex_lock(solver_polynom_mutex);

    g_value_set_pointer(value, g_strdupv(solver_polynom->exponent));

    g_rec_mutex_unlock(solver_polynom_mutex);
  }
  break;
  case PROP_COEFFICIENT_VALUE:
  {
    g_rec_mutex_lock(solver_polynom_mutex);

    g_value_set_boxed(value, &(solver_polynom->coefficient_value));

    g_rec_mutex_unlock(solver_polynom_mutex);
  }
  break;
  case PROP_EXPONENT_VALUE:
  {
    g_rec_mutex_lock(solver_polynom_mutex);

    g_value_set_boxed(value, &(solver_polynom->exponent_value));

    g_rec_mutex_unlock(solver_polynom_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_solver_polynom_finalize(GObject *gobject)
{
  AgsSolverPolynom *solver_polynom;

  solver_polynom = AGS_SOLVER_POLYNOM(gobject);

  g_free(solver_polynom->polynom);

  g_free(solver_polynom->coefficient);
  g_free(solver_polynom->symbol);
  g_free(solver_polynom->exponent);
  
  /* call parent */
  G_OBJECT_CLASS(ags_solver_polynom_parent_class)->finalize(gobject);
}

/**
 * ags_solver_polynom_update:
 * @solver_polynom: the #AgsSolverPolynom
 * 
 * Update string representation of @solver_polynom.
 * 
 * Since: 3.2.0
 */
void
ags_solver_polynom_update(AgsSolverPolynom *solver_polynom)
{
  if(!AGS_IS_SOLVER_POLYNOM(solver_polynom)){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_solver_polynom_parse:
 * @solver_polynom: the #AgsSolverPolynom
 * @polynom: the polynom as string
 * 
 * Parse @polynom and apply to @solver_polynom.
 * 
 * Since: 3.2.0
 */
void
ags_solver_polynom_parse(AgsSolverPolynom *solver_polynom,
			 gchar *polynom)
{
  gchar **symbol;
  gchar **exponent;
  
  gchar *coefficient;
  
  AgsComplex coefficient_value;
  AgsComplex *exponent_value;
  
  complex z;

  guint symbol_count;
  guint i;

  GRecMutex *solver_polynom_mutex;
  
  if(!AGS_IS_SOLVER_POLYNOM(solver_polynom) ||
     polynom == NULL){
    return;
  }

  solver_polynom_mutex = AGS_SOLVER_POLYNOM_GET_OBJ_MUTEX(solver_polynom);
  
  symbol = NULL;
  exponent = NULL;

  coefficient = NULL;

  exponent_value = NULL;
  
  symbol_count = 0;

  /* get base */
  //TODO:JK: implement me

  /* coefficient */
  z = 1.0 + I * 0.0;
  ags_complex_set(&(coefficient_value),
		  z);
  
  /* exponent */
  if(symbol_count > 0){
    exponent_value = g_new(AgsComplex,
			   symbol_count);    

    for(i = 0; i < symbol_count; i++){
      z = 1.0 + I * 0.0;
      
      ags_complex_set(&(exponent_value[i]),
		      z);
    }
  }

  /* parse */
  //TODO:JK: implement me
  
  /* apply */
  g_rec_mutex_lock(solver_polynom_mutex);

  solver_polynom->polynom = g_strdup(polynom);

  solver_polynom->coefficient = coefficient;

  solver_polynom->symbol = symbol;
  solver_polynom->exponent = exponent;
  
  ags_complex_set(&(solver_polynom->coefficient_value),
		  ags_complex_get(&coefficient_value));

  solver_polynom->exponent_value = exponent_value;

  g_rec_mutex_unlock(solver_polynom_mutex);
}

/**
 * ags_solver_polynom_add:
 * @polynom_a: the first summand
 * @polynom_b: the second summand
 * 
 * Perform addition of @polynom_a and @polynom_b. Both summands need to have the very same
 * symbol and exponent, otherwise %NULL returned and error is appropriately set.
 * 
 * Returns: the newly instantiated #AgsSolverPolynom or %NULL
 * 
 * Since: 3.2.0
 */
AgsSolverPolynom*
ags_solver_polynom_add(AgsSolverPolynom *polynom_a,
		       AgsSolverPolynom *polynom_b,
		       GError **error)
{
  AgsSolverPolynom *solver_polynom;
  
  gchar **symbol_a, **symbol_b;
  gchar **exponent_a, **exponent_b;

  complex coefficient_a, coefficient_b;

  guint length;
  guint i;
  gboolean is_symbol_matching;
  gboolean is_exponent_matching;

  GRecMutex *polynom_a_mutex;
  GRecMutex *polynom_b_mutex;
  
  if(!AGS_IS_SOLVER_POLYNOM(polynom_a) ||
     !AGS_IS_SOLVER_POLYNOM(polynom_b)){
    return(NULL);
  }

  symbol_a = NULL;
  symbol_b = NULL;

  exponent_a = NULL;
  exponent_b = NULL;

  length = 0;
  
  is_symbol_matching = FALSE;
  is_exponent_matching = FALSE;
  
  g_object_get(polynom_a,
	       "symbol", &symbol_a,
	       NULL);

  g_object_get(polynom_b,
	       "symbol", &symbol_b,
	       NULL);

  is_symbol_matching = (symbol_a != NULL &&
			symbol_b != NULL &&
			g_strv_equal(symbol_a,
				     symbol_b)) ? TRUE: FALSE;

  g_strfreev(symbol_a);
  g_strfreev(symbol_b);
  
  if(!is_symbol_matching){
    if(error != NULL){
      g_set_error(error,
		  AGS_SOLVER_POLYNOM_ERROR,
		  AGS_SOLVER_POLYNOM_ERROR_SYMBOL_MISMATCH,
		  "symbols don't match");
    }
    
    return(NULL);
  }

  g_object_get(polynom_a,
	       "exponent", &exponent_a,
	       NULL);

  g_object_get(polynom_b,
	       "exponent", &exponent_b,
	       NULL);

  is_exponent_matching = (exponent_a != NULL &&
			  exponent_b != NULL &&
			  g_strv_equal(exponent_a,
				       exponent_b)) ? TRUE: FALSE;
  
  g_strfreev(exponent_a);
  g_strfreev(exponent_b);
  
  if(!is_exponent_matching){
    if(error != NULL){
      g_set_error(error,
		  AGS_SOLVER_POLYNOM_ERROR,
		  AGS_SOLVER_POLYNOM_ERROR_EXPONENT_MISMATCH,
		  "exponents don't match");
    }
    
    return(NULL);
  }

  symbol_a = NULL;
  symbol_b = NULL;

  exponent_a = NULL;
  exponent_b = NULL;
    
  solver_polynom = ags_solver_polynom_new();

  polynom_a_mutex = AGS_SOLVER_POLYNOM_GET_OBJ_MUTEX(polynom_a);
  polynom_b_mutex = AGS_SOLVER_POLYNOM_GET_OBJ_MUTEX(polynom_b);
  
  g_object_get(polynom_a,
	       "symbol", &symbol_a,
	       "exponent", &exponent_a,
	       NULL);

  length = g_strv_length(symbol_a);

  if(length > 0){
    solver_polynom->symbol = g_strdupv(symbol_a);
    solver_polynom->exponent = g_strdupv(exponent_a);
    
    solver_polynom->exponent_value = g_new(AgsComplex,
					   length);

    g_rec_mutex_lock(polynom_a_mutex);
    
    for(i = 0; i < length; i++){
      ags_complex_set(&(solver_polynom->exponent_value[i]),
		      ags_complex_get(&(polynom_a->exponent_value[i])));
    }

    g_rec_mutex_unlock(polynom_a_mutex);
  }
  
  /* get coeffiecient and summand of polynom a */
  g_rec_mutex_lock(polynom_a_mutex);

  coefficient_a = ags_complex_get(&(polynom_a->coefficient_value));
  
  g_rec_mutex_unlock(polynom_a_mutex);

  /* get coeffiecient and summand of polynom b */
  g_rec_mutex_lock(polynom_b_mutex);

  coefficient_b = ags_complex_get(&(polynom_b->coefficient_value));

  g_rec_mutex_unlock(polynom_b_mutex);

  /* add */
  ags_complex_set(&(solver_polynom->coefficient_value),
		  coefficient_a + coefficient_b);

  ags_solver_polynom_update(solver_polynom);
  
  g_strfreev(symbol_a);
  g_strfreev(exponent_a);
  
  return(solver_polynom);
}

AgsSolverPolynom*
ags_solver_polynom_subtract(AgsSolverPolynom *polynom_a,
			    AgsSolverPolynom *polynom_b,
			    GError **error)
{
  AgsSolverPolynom *solver_polynom;
  
  gchar **symbol_a, **symbol_b;
  gchar **exponent_a, **exponent_b;

  complex coefficient_a, coefficient_b;

  guint length;
  guint i;
  gboolean is_symbol_matching;
  gboolean is_exponent_matching;

  GRecMutex *polynom_a_mutex;
  GRecMutex *polynom_b_mutex;
  
  if(!AGS_IS_SOLVER_POLYNOM(polynom_a) ||
     !AGS_IS_SOLVER_POLYNOM(polynom_b)){
    return(NULL);
  }

  symbol_a = NULL;
  symbol_b = NULL;

  exponent_a = NULL;
  exponent_b = NULL;

  length = 0;
  
  is_symbol_matching = FALSE;
  is_exponent_matching = FALSE;
  
  g_object_get(polynom_a,
	       "symbol", &symbol_a,
	       NULL);

  g_object_get(polynom_b,
	       "symbol", &symbol_b,
	       NULL);

  is_symbol_matching = (symbol_a != NULL &&
			symbol_b != NULL &&
			g_strv_equal(symbol_a,
				     symbol_b)) ? TRUE: FALSE;

  g_strfreev(symbol_a);
  g_strfreev(symbol_b);
  
  if(!is_symbol_matching){
    if(error != NULL){
      g_set_error(error,
		  AGS_SOLVER_POLYNOM_ERROR,
		  AGS_SOLVER_POLYNOM_ERROR_SYMBOL_MISMATCH,
		  "symbols don't match");
    }
    
    return(NULL);
  }

  g_object_get(polynom_a,
	       "exponent", &exponent_a,
	       NULL);

  g_object_get(polynom_b,
	       "exponent", &exponent_b,
	       NULL);

  is_exponent_matching = (exponent_a != NULL &&
			  exponent_b != NULL &&
			  g_strv_equal(exponent_a,
				       exponent_b)) ? TRUE: FALSE;
  
  g_strfreev(exponent_a);
  g_strfreev(exponent_b);
  
  if(!is_exponent_matching){
    if(error != NULL){
      g_set_error(error,
		  AGS_SOLVER_POLYNOM_ERROR,
		  AGS_SOLVER_POLYNOM_ERROR_EXPONENT_MISMATCH,
		  "exponents don't match");
    }
    
    return(NULL);
  }

  symbol_a = NULL;
  symbol_b = NULL;

  exponent_a = NULL;
  exponent_b = NULL;
    
  solver_polynom = ags_solver_polynom_new();

  polynom_a_mutex = AGS_SOLVER_POLYNOM_GET_OBJ_MUTEX(polynom_a);
  polynom_b_mutex = AGS_SOLVER_POLYNOM_GET_OBJ_MUTEX(polynom_b);
  
  g_object_get(polynom_a,
	       "symbol", &symbol_a,
	       "exponent", &exponent_a,
	       NULL);

  length = g_strv_length(symbol_a);

  if(length > 0){
    solver_polynom->symbol = g_strdupv(symbol_a);
    solver_polynom->exponent = g_strdupv(exponent_a);
    
    solver_polynom->exponent_value = g_new(AgsComplex,
					   length);

    g_rec_mutex_lock(polynom_a_mutex);
    
    for(i = 0; i < length; i++){
      ags_complex_set(&(solver_polynom->exponent_value[i]),
		      ags_complex_get(&(polynom_a->exponent_value[i])));
    }

    g_rec_mutex_unlock(polynom_a_mutex);
  }

  /* get coeffiecient and summand of polynom a */
  g_rec_mutex_lock(polynom_a_mutex);

  coefficient_a = ags_complex_get(&(polynom_a->coefficient_value));
  
  g_rec_mutex_unlock(polynom_a_mutex);

  /* get coeffiecient and summand of polynom b */
  g_rec_mutex_lock(polynom_b_mutex);

  coefficient_b = ags_complex_get(&(polynom_b->coefficient_value));

  g_rec_mutex_unlock(polynom_b_mutex);

  /* add */
  ags_complex_set(&(solver_polynom->coefficient_value),
		  coefficient_a - coefficient_b);

  ags_solver_polynom_update(solver_polynom);
  
  g_strfreev(symbol_a);
  g_strfreev(exponent_a);
  
  return(solver_polynom);
}

AgsSolverPolynom*
ags_solver_polynom_multiply(AgsSolverPolynom *polynom_a,
			    AgsSolverPolynom *polynom_b,
			    GError **error)
{
  AgsSolverPolynom *solver_polynom;
  
  if(!AGS_IS_SOLVER_POLYNOM(polynom_a) ||
     !AGS_IS_SOLVER_POLYNOM(polynom_b)){
    return(NULL);
  }

  //TODO:JK: implement me

  return(NULL);
}

AgsSolverPolynom*
ags_solver_polynom_divide(AgsSolverPolynom *polynom_a,
			  AgsSolverPolynom *polynom_b,
			  GError **error)
{
  AgsSolverPolynom *solver_polynom;
  
  if(!AGS_IS_SOLVER_POLYNOM(polynom_a) ||
     !AGS_IS_SOLVER_POLYNOM(polynom_b)){
    return(NULL);
  }

  //TODO:JK: implement me

  return(NULL);
}

AgsSolverPolynom*
ags_solver_polynom_raise_power(AgsSolverPolynom *polynom_a,
			       AgsSolverPolynom *polynom_b,
			       GError **error)
{
  AgsSolverPolynom *solver_polynom;
  
  if(!AGS_IS_SOLVER_POLYNOM(polynom_a) ||
     !AGS_IS_SOLVER_POLYNOM(polynom_b)){
    return(NULL);
  }

  //TODO:JK: implement me

  return(NULL);
}

AgsSolverPolynom*
ags_solver_polynom_extract_root(AgsSolverPolynom *polynom_a,
				AgsSolverPolynom *polynom_b,
				GError **error)
{
  AgsSolverPolynom *solver_polynom;
  
  if(!AGS_IS_SOLVER_POLYNOM(polynom_a) ||
     !AGS_IS_SOLVER_POLYNOM(polynom_b)){
    return(NULL);
  }

  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_solver_polynom_new:
 *
 * Instantiate a new #AgsSolverPolynom.
 *
 * Returns: the new instance
 *
 * Since: 3.0.0
 */
AgsSolverPolynom*
ags_solver_polynom_new()
{
  AgsSolverPolynom *solver_polynom;
  
  solver_polynom = g_object_new(AGS_TYPE_SOLVER_POLYNOM,
				NULL);

  return(solver_polynom);
}
