/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/lib/ags_solver_polynomial.h>

#include <ags/lib/ags_math_util.h>
#include <ags/lib/ags_string_util.h>

#include <stdlib.h>
#include <complex.h>

#include <ags/i18n.h>

void ags_solver_polynomial_class_init(AgsSolverPolynomialClass *solver_polynomial);
void ags_solver_polynomial_init (AgsSolverPolynomial *solver_polynomial);
void ags_solver_polynomial_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_solver_polynomial_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_solver_polynomial_finalize(GObject *gobject);

/**
 * SECTION:ags_solver_polynomial
 * @short_description: solver polynomial
 * @title: AgsSolverPolynomial
 * @section_id:
 * @include: ags/lib/ags_solver_polynomial.h
 *
 * The #AgsSolverPolynomial contains the solver polynomials.
 */

enum{
  PROP_0,
  PROP_POLYNOMIAL,
  PROP_COEFFICIENT,
  PROP_SYMBOL,
  PROP_EXPONENT,
  PROP_COEFFICIENT_VALUE,
  PROP_EXPONENT_VALUE,
};

static gpointer ags_solver_polynomial_parent_class = NULL;

static GMutex regex_mutex;

GType
ags_solver_polynomial_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_solver_polynomial = 0;

    static const GTypeInfo ags_solver_polynomial_info = {
      sizeof (AgsSolverPolynomialClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_solver_polynomial_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSolverPolynomial),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_solver_polynomial_init,
    };

    ags_type_solver_polynomial = g_type_register_static(G_TYPE_OBJECT,
							"AgsSolverPolynomial",
							&ags_solver_polynomial_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_solver_polynomial);
  }

  return g_define_type_id__volatile;
}

void
ags_solver_polynomial_class_init(AgsSolverPolynomialClass *solver_polynomial)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_solver_polynomial_parent_class = g_type_class_peek_parent(solver_polynomial);

  /* GObjectClass */
  gobject = (GObjectClass *) solver_polynomial;

  gobject->set_property = ags_solver_polynomial_set_property;
  gobject->get_property = ags_solver_polynomial_get_property;
  
  gobject->finalize = ags_solver_polynomial_finalize;

  /* properties */
  /**
   * AgsSolverPolynomial:polynomial:
   *
   * The assigned polynomial.
   * 
   * Since: 3.2.0
   */
  param_spec = g_param_spec_string("polynomial",
				   i18n_pspec("polynomial of solver polynomial"),
				   i18n_pspec("The polynomial this solver polynomial is assigned to"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_POLYNOMIAL,
				  param_spec);

  /**
   * AgsSolverPolynomial:coefficient:
   *
   * The assigned coefficient.
   * 
   * Since: 3.2.0
   */
  param_spec = g_param_spec_string("coefficient",
				   i18n_pspec("coefficient of solver polynomial"),
				   i18n_pspec("The coefficient this solver polynomial is assigned to"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COEFFICIENT,
				  param_spec);

  /**
   * AgsSolverPolynomial:symbol:
   *
   * The assigned symbol.
   * 
   * Since: 3.2.0
   */
  param_spec = g_param_spec_pointer("symbol",
				    i18n_pspec("symbol of solver polynomial"),
				    i18n_pspec("The symbol this solver polynomial is assigned to"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYMBOL,
				  param_spec);

  /**
   * AgsSolverPolynomial:exponent:
   *
   * The assigned exponent.
   * 
   * Since: 3.2.0
   */
  param_spec = g_param_spec_pointer("exponent",
				    i18n_pspec("exponent of solver polynomial"),
				    i18n_pspec("The exponent this solver polynomial is assigned to"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_EXPONENT,
				  param_spec);

  /**
   * AgsSolverPolynomial:coefficient-value:
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
   * AgsSolverPolynomial:exponent-value:
   *
   * Exponent value.
   * 
   * Since: 3.2.0
   */
  param_spec = g_param_spec_pointer("exponent-value",
				    i18n_pspec("exponent value"),
				    i18n_pspec("The exponent value"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_EXPONENT_VALUE,
				  param_spec);
}

GQuark
ags_solver_polynomial_error_quark()
{
  return(g_quark_from_static_string("ags-solver-polynomial-error-quark"));
}

void
ags_solver_polynomial_init(AgsSolverPolynomial *solver_polynomial)
{
  double _Complex z;

  solver_polynomial->flags = 0;
  
  g_rec_mutex_init(&(solver_polynomial->obj_mutex));

  solver_polynomial->polynomial = NULL;

  solver_polynomial->coefficient = NULL;
  solver_polynomial->symbol = NULL;
  solver_polynomial->exponent = NULL;

  solver_polynomial->coefficient_value = g_new(AgsComplex,
					       1);
  
  z = 1.0 + I * 0.0;
  ags_complex_set(solver_polynomial->coefficient_value,
		  z);

  solver_polynomial->exponent_value = NULL;
}

void
ags_solver_polynomial_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsSolverPolynomial *solver_polynomial;

  GRecMutex *solver_polynomial_mutex;
  
  solver_polynomial = AGS_SOLVER_POLYNOMIAL(gobject);

  /* solver polynomial mutex */
  solver_polynomial_mutex = AGS_SOLVER_POLYNOMIAL_GET_OBJ_MUTEX(solver_polynomial);
  
  switch(prop_id){
  case PROP_POLYNOMIAL:
  {
    gchar *polynomial;

    polynomial = (gchar *) g_value_get_string(value);

    g_rec_mutex_lock(solver_polynomial_mutex);

    if(solver_polynomial->polynomial == polynomial){
      g_rec_mutex_unlock(solver_polynomial_mutex);

      return;
    }
      
    if(solver_polynomial->polynomial != NULL){
      g_free(solver_polynomial->polynomial);
    }

    solver_polynomial->polynomial = g_strdup(polynomial);

    g_rec_mutex_unlock(solver_polynomial_mutex);
  }
  break;
  case PROP_COEFFICIENT:
  {
    gchar *coefficient;

    coefficient = (gchar *) g_value_get_string(value);

    g_rec_mutex_lock(solver_polynomial_mutex);

    if(solver_polynomial->coefficient == coefficient){
      g_rec_mutex_unlock(solver_polynomial_mutex);

      return;
    }
      
    if(solver_polynomial->coefficient != NULL){
      g_free(solver_polynomial->coefficient);
    }

    solver_polynomial->coefficient = g_strdup(coefficient);

    g_rec_mutex_unlock(solver_polynomial_mutex);
  }
  break;
  case PROP_SYMBOL:
  {
    gchar *symbol;
    
    double _Complex z;

    guint length;

    symbol = (gchar *) g_value_get_pointer(value);

    g_rec_mutex_lock(solver_polynomial_mutex);

    if(solver_polynomial->symbol != NULL &&
       g_strv_contains(solver_polynomial->symbol, symbol)){
      g_rec_mutex_unlock(solver_polynomial_mutex);

      return;
    }

    if(solver_polynomial->symbol == NULL){
      length = 0;
      
      solver_polynomial->symbol = (gchar **) g_malloc(2 * sizeof(gchar *));      
      solver_polynomial->exponent = (gchar **) g_malloc(2 * sizeof(gchar *));

      solver_polynomial->exponent_value = g_new(AgsComplex,
						1);
    }else{
      length = g_strv_length(solver_polynomial->symbol);
      
      solver_polynomial->symbol = (gchar **) g_realloc(solver_polynomial->symbol,
						       (length + 2) * sizeof(gchar *));
      solver_polynomial->exponent = (gchar **) g_realloc(solver_polynomial->exponent,
							 (length + 2) * sizeof(gchar *));

      solver_polynomial->exponent_value = g_renew(AgsComplex,
						  solver_polynomial->exponent_value,
						  (length + 1));
    }
    
    solver_polynomial->symbol[length] = g_strdup(symbol);
    solver_polynomial->symbol[length + 1] = NULL;

    solver_polynomial->exponent[length] = g_strdup("1");
    solver_polynomial->exponent[length + 1] = NULL;

    z = 1.0 + I * 0.0;
    ags_complex_set(&(solver_polynomial->exponent_value[length]),
		    z);
    
    g_rec_mutex_unlock(solver_polynomial_mutex);
  }
  break;
  case PROP_COEFFICIENT_VALUE:
  {
    AgsComplex *coefficient_value;

    coefficient_value = (AgsComplex *) g_value_get_boxed(value);

    g_rec_mutex_lock(solver_polynomial_mutex);

    ags_complex_set(solver_polynomial->coefficient_value,
		    ags_complex_get(coefficient_value));

    g_rec_mutex_unlock(solver_polynomial_mutex);
  }
  break;    
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_solver_polynomial_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsSolverPolynomial *solver_polynomial;

  GRecMutex *solver_polynomial_mutex;

  solver_polynomial = AGS_SOLVER_POLYNOMIAL(gobject);

  /* solver polynomial mutex */
  solver_polynomial_mutex = AGS_SOLVER_POLYNOMIAL_GET_OBJ_MUTEX(solver_polynomial);

  switch(prop_id){
  case PROP_POLYNOMIAL:
  {
    g_rec_mutex_lock(solver_polynomial_mutex);

    g_value_set_string(value, solver_polynomial->polynomial);

    g_rec_mutex_unlock(solver_polynomial_mutex);
  }
  break;
  case PROP_COEFFICIENT:
  {
    g_rec_mutex_lock(solver_polynomial_mutex);

    g_value_set_string(value, solver_polynomial->coefficient);

    g_rec_mutex_unlock(solver_polynomial_mutex);
  }
  break;
  case PROP_SYMBOL:
  {
    g_rec_mutex_lock(solver_polynomial_mutex);

    g_value_set_pointer(value, g_strdupv(solver_polynomial->symbol));

    g_rec_mutex_unlock(solver_polynomial_mutex);
  }
  break;
  case PROP_EXPONENT:
  {
    g_rec_mutex_lock(solver_polynomial_mutex);

    g_value_set_pointer(value, g_strdupv(solver_polynomial->exponent));

    g_rec_mutex_unlock(solver_polynomial_mutex);
  }
  break;
  case PROP_COEFFICIENT_VALUE:
  {
    g_rec_mutex_lock(solver_polynomial_mutex);

    g_value_set_boxed(value, solver_polynomial->coefficient_value);

    g_rec_mutex_unlock(solver_polynomial_mutex);
  }
  break;
  case PROP_EXPONENT_VALUE:
  {
    g_rec_mutex_lock(solver_polynomial_mutex);

    g_value_set_pointer(value, solver_polynomial->exponent_value);

    g_rec_mutex_unlock(solver_polynomial_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_solver_polynomial_finalize(GObject *gobject)
{
  AgsSolverPolynomial *solver_polynomial;

  solver_polynomial = AGS_SOLVER_POLYNOMIAL(gobject);

  g_free(solver_polynomial->polynomial);

  g_free(solver_polynomial->coefficient);
  g_free(solver_polynomial->symbol);
  g_free(solver_polynomial->exponent);
  
  /* call parent */
  G_OBJECT_CLASS(ags_solver_polynomial_parent_class)->finalize(gobject);
}

/**
 * ags_solver_polynomial_set_polynomial:
 * @solver_polynomial: the #AgsSolverPolynomial
 * @polynomial: the polynomial
 * 
 * Set @polynomial of @solver_polynomial.
 * 
 * Since: 3.9.3
 */
void
ags_solver_polynomial_set_polynomial(AgsSolverPolynomial *solver_polynomial,
				     gchar *polynomial)
{
  if(!AGS_IS_SOLVER_POLYNOMIAL(solver_polynomial)){
    return;
  }

  g_object_set(solver_polynomial,
	       "polynomial", polynomial,
	       NULL);
}

/**
 * ags_solver_polynomial_get_polynomial:
 * @solver_polynomial: the #AgsSolverPolynomial
 * 
 * Get polynomial of @solver_polynomial.
 * 
 * Returns: the polynomial
 * 
 * Since: 3.9.3
 */
gchar*
ags_solver_polynomial_get_polynomial(AgsSolverPolynomial *solver_polynomial)
{
  gchar *polynomial;
  
  if(!AGS_IS_SOLVER_POLYNOMIAL(solver_polynomial)){
    return(NULL);
  }

  polynomial = NULL;
  
  g_object_get(solver_polynomial,
	       "polynomial", &polynomial,
	       NULL);

  return(polynomial);
}

/**
 * ags_solver_polynomial_set_coefficient:
 * @solver_polynomial: the #AgsSolverPolynomial
 * @coefficient: the coefficient
 * 
 * Set @coefficient of @solver_polynomial.
 * 
 * Since: 3.9.3
 */
void
ags_solver_polynomial_set_coefficient(AgsSolverPolynomial *solver_polynomial,
				      gchar *coefficient)
{
  if(!AGS_IS_SOLVER_POLYNOMIAL(solver_polynomial)){
    return;
  }

  g_object_set(solver_polynomial,
	       "coefficient", coefficient,
	       NULL);
}

/**
 * ags_solver_polynomial_get_coefficient:
 * @solver_polynomial: the #AgsSolverPolynomial
 * 
 * Get coefficient of @solver_polynomial.
 * 
 * Returns: the coefficient
 * 
 * Since: 3.9.3
 */
gchar*
ags_solver_polynomial_get_coefficient(AgsSolverPolynomial *solver_polynomial)
{
  gchar *coefficient;
  
  if(!AGS_IS_SOLVER_POLYNOMIAL(solver_polynomial)){
    return(NULL);
  }

  coefficient = NULL;
  
  g_object_get(solver_polynomial,
	       "coefficient", &coefficient,
	       NULL);

  return(coefficient);
}

/**
 * ags_solver_polynomial_get_symbol:
 * @solver_polynomial: the #AgsSolverPolynomial
 * 
 * Get symbol of @solver_polynomial.
 * 
 * Returns: the symbol
 * 
 * Since: 3.9.3
 */
gchar**
ags_solver_polynomial_get_symbol(AgsSolverPolynomial *solver_polynomial)
{
  gchar **symbol;
  
  if(!AGS_IS_SOLVER_POLYNOMIAL(solver_polynomial)){
    return(NULL);
  }

  symbol = NULL;
  
  g_object_get(solver_polynomial,
	       "symbol", &symbol,
	       NULL);

  return(symbol);
}

/**
 * ags_solver_polynomial_get_exponent:
 * @solver_polynomial: the #AgsSolverPolynomial
 * 
 * Get exponent of @solver_polynomial.
 * 
 * Returns: the exponent
 * 
 * Since: 3.9.3
 */
gchar**
ags_solver_polynomial_get_exponent(AgsSolverPolynomial *solver_polynomial)
{
  gchar **exponent;
  
  if(!AGS_IS_SOLVER_POLYNOMIAL(solver_polynomial)){
    return(NULL);
  }

  exponent = NULL;
  
  g_object_get(solver_polynomial,
	       "exponent", &exponent,
	       NULL);

  return(exponent);
}

/**
 * ags_solver_polynomial_set_coefficient_value:
 * @solver_polynomial: the #AgsSolverPolynomial
 * @coefficient_value: the coefficient value
 * 
 * Set @coefficient_value of @solver_polynomial.
 * 
 * Since: 3.9.3
 */
void
ags_solver_polynomial_set_coefficient_value(AgsSolverPolynomial *solver_polynomial,
					    AgsComplex *coefficient_value)
{
  if(!AGS_IS_SOLVER_POLYNOMIAL(solver_polynomial)){
    return;
  }

  g_object_set(solver_polynomial,
	       "coefficient-value", coefficient_value,
	       NULL);
}

/**
 * ags_solver_polynomial_get_coefficient_value:
 * @solver_polynomial: the #AgsSolverPolynomial
 * 
 * Get coefficient value of @solver_polynomial.
 * 
 * Returns: the coefficient value
 * 
 * Since: 3.9.3
 */
AgsComplex*
ags_solver_polynomial_get_coefficient_value(AgsSolverPolynomial *solver_polynomial)
{
  AgsComplex *coefficient_value;
  
  if(!AGS_IS_SOLVER_POLYNOMIAL(solver_polynomial)){
    return(NULL);
  }

  coefficient_value = NULL;
  
  g_object_get(solver_polynomial,
	       "coefficient-value", &coefficient_value,
	       NULL);

  return(coefficient_value);
}

/**
 * ags_solver_polynomial_get_exponent_value:
 * @solver_polynomial: the #AgsSolverPolynomial
 * 
 * Get exponent value of @solver_polynomial.
 * 
 * Returns: the exponent value
 * 
 * Since: 3.9.3
 */
AgsComplex*
ags_solver_polynomial_get_exponent_value(AgsSolverPolynomial *solver_polynomial)
{
  AgsComplex *exponent_value;
  
  if(!AGS_IS_SOLVER_POLYNOMIAL(solver_polynomial)){
    return(NULL);
  }

  exponent_value = NULL;
  
  g_object_get(solver_polynomial,
	       "exponent-value", &exponent_value,
	       NULL);

  return(exponent_value);
}

/**
 * ags_solver_polynomial_update:
 * @solver_polynomial: the #AgsSolverPolynomial
 * 
 * Update string representation of @solver_polynomial.
 * 
 * Since: 3.2.0
 */
void
ags_solver_polynomial_update(AgsSolverPolynomial *solver_polynomial)
{
  gchar **symbol;
  gchar **exponent;

  gchar *polynomial;
  gchar *coefficient;

  guint i;
  
  if(!AGS_IS_SOLVER_POLYNOMIAL(solver_polynomial)){
    return;
  }

  polynomial = NULL;

  symbol = NULL;
  exponent = NULL;
  
  coefficient = NULL;

  g_object_get(solver_polynomial,
	       "coefficient", &coefficient,
	       "symbol", &symbol,
	       "exponent", &exponent,
	       NULL);
  
  if(coefficient != NULL){
    polynomial = g_strdup(coefficient);
  }

  if(symbol != NULL){
    for(i = 0; symbol[i] != NULL; i++){
      if(polynomial == NULL){
	if(exponent[i] != NULL &&
	   strlen(exponent[i]) > 0){
	  polynomial = g_strdup_printf("%s^(%s)",
				       symbol[i],
				       exponent[i]);
	}else{
	  polynomial = g_strdup(symbol[i]);
	}
      }else{
	gchar *prev_polynomial;
	
	prev_polynomial = polynomial;  

	polynomial = g_strdup_printf("%s%s^(%s)",
				     polynomial,
				     symbol[i],
				     exponent[i]);

	g_free(prev_polynomial);
      }
    }
  }

  g_object_set(solver_polynomial,
	       "polynomial", polynomial,
	       NULL);

  g_strfreev(symbol);
  g_strfreev(exponent);

  g_free(coefficient);
  
  g_free(polynomial);
}

/**
 * ags_solver_polynomial_parse:
 * @solver_polynomial: the #AgsSolverPolynomial
 * @polynomial: the polynomial as string
 * 
 * Parse @polynomial and apply to @solver_polynomial.
 * 
 * Since: 3.2.0
 */
void
ags_solver_polynomial_parse(AgsSolverPolynomial *solver_polynomial,
			    gchar *polynomial)
{
  GMatchInfo *sign_match_info;
  GMatchInfo *coefficient_match_info;
  GMatchInfo *symbol_match_info;
  GMatchInfo *function_match_info;
  GMatchInfo *exponent_match_info;

  gchar **symbol;
  gchar **exponent;
  
  gchar *coefficient;
  
  AgsComplex *coefficient_value;
  AgsComplex *exponent_value;
  
  double _Complex z;

  guint offset, current_offset;
  guint symbol_count;
  guint exponent_count;
  guint i;
  gunichar c, match_c;
  gboolean success;
  gboolean is_signed;
  
  GError *error;

  GRecMutex *solver_polynomial_mutex;

  static GRegex *sign_regex = NULL;
  static GRegex *coefficient_regex = NULL;
  static GRegex *symbol_regex = NULL;
  static GRegex *function_regex = NULL;
  static GRegex *exponent_regex = NULL;

  /* groups: #1 sign */
  static const gchar *sign_pattern = "^([\\+\\-])";

  /* groups: #1 constants, #2 numeric with optional fraction */
  static const gchar *coefficient_pattern = "^([ℯ𝜋𝑖∞])|([0-9]+(\\.[0-9]+)?)";

  /* groups: #1 symbol */
  static const gchar *symbol_pattern = "^([a-zA-Z][0-9]*)";

  /* groups: #1 function */
  static const gchar *function_pattern = "(log|exp|sin|cos|tan|asin|acos|atan|floor|ceil|round)";

  /* groups: #1 exponent operator, #2 exponent */
  static const gchar *exponent_pattern = "^(\\^)(\\([^)(]*+(?:(?R)[^)(]*)*+\\))";
  
  if(!AGS_IS_SOLVER_POLYNOMIAL(solver_polynomial) ||
     polynomial == NULL){
    return;
  }

  solver_polynomial_mutex = AGS_SOLVER_POLYNOMIAL_GET_OBJ_MUTEX(solver_polynomial);

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

  if(coefficient_regex == NULL){
    error = NULL;
    coefficient_regex = g_regex_new(coefficient_pattern,
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

  offset = 0;
  
  g_regex_match(function_regex, polynomial, 0, &function_match_info);

  if(g_match_info_matches(function_match_info)){
    g_critical("malformed polynomial, found function rewrite first");    
    
    return;
  }

  /* parse sign */
  g_regex_match(sign_regex, polynomial, 0, &sign_match_info);

  if(g_match_info_matches(sign_match_info)){
    gint start_pos, end_pos;

    g_match_info_fetch_pos(sign_match_info,
			   0,
			   &start_pos, &end_pos);

    offset += end_pos;
  }

  g_match_info_free(sign_match_info);

  /* parse coefficient */
  is_signed = FALSE;
  
  g_regex_match(coefficient_regex, polynomial + offset, 0, &coefficient_match_info);

  while(g_match_info_matches(coefficient_match_info)){
    gint start_pos, end_pos;

    g_match_info_fetch_pos(coefficient_match_info,
			   0,
			   &start_pos, &end_pos);

    offset += end_pos;

    g_match_info_next(coefficient_match_info,
		      NULL);
  }

  g_match_info_free(coefficient_match_info);

  coefficient = NULL;

  if(offset == 0){
    coefficient = g_strdup("1"); 
  }else if(offset == 1){
    if(polynomial[0] == '+' ||
       polynomial[0] == '-'){
      coefficient = g_strdup_printf("%c%c",
				    polynomial[0],
				    '1');
      
      is_signed = TRUE;
    }else{
      coefficient = g_strndup(polynomial,
			      1); 
    }
  }else{
    if(polynomial[0] == '+' ||
       polynomial[0] == '-'){      
      is_signed = TRUE;
    }
    
    coefficient = g_strndup(polynomial,
			    offset); 
  }

  /* parse symbol */
  symbol = NULL;
  symbol_count = 0;

  current_offset = offset;
  
  g_regex_match(symbol_regex, polynomial + current_offset, 0, &symbol_match_info);

  while(g_match_info_matches(symbol_match_info)){
    gint start_pos, end_pos;

    g_match_info_fetch_pos(symbol_match_info,
			   0,
			   &start_pos, &end_pos);

    if(symbol == NULL){
      symbol = (gchar *) g_malloc(2 * sizeof(gchar *));
    }else{
      symbol = (gchar *) g_realloc(symbol,
				   (symbol_count + 2) * sizeof(gchar *));
    }

    symbol[symbol_count] = g_strdup_printf("%*.s",
					   end_pos - start_pos, polynomial + current_offset + start_pos);

    symbol[symbol_count + 1] = NULL;
    
    symbol_count++;
    
    current_offset += end_pos;
    
    g_match_info_next(symbol_match_info,
		      NULL);
  }

  g_match_info_free(symbol_match_info);

  /* coefficient */
  coefficient_value = (AgsComplex *) g_new(AgsComplex,
					   1);

  z = 1.0 + I * 0.0;

  current_offset = 0;

  if(is_signed){
    current_offset++;
  }

  /* match pi */
  c = g_utf8_get_char(coefficient + current_offset);

  match_c = g_utf8_get_char(AGS_SYMBOLIC_PI);

  if(c == match_c){
    z *= M_PI;
    
    offset += strlen(AGS_SYMBOLIC_PI);
  }

  /* match euler */
  if(offset < strlen(coefficient)){
    match_c = g_utf8_get_char(AGS_SYMBOLIC_EULER);

    if(c == match_c){
      z *= M_E;

      offset += strlen(AGS_SYMBOLIC_EULER);
    }
  }

  //TODO:JK: huh
  /* match more? */

  if(offset < strlen(coefficient)){
    gchar *endptr;

    gdouble val;
    
    endptr = NULL;

    val = g_ascii_strtod(coefficient + offset,
			 &endptr);

    if(endptr != NULL){
      z *= val;
    }
  }  

  if(coefficient[0] == '-'){
    z *= -1.0;
  }
  
  ags_complex_set(coefficient_value,
		  z);

  /* get base */
  exponent = NULL;
  exponent_count = 0;

  exponent_value = NULL;
  
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
  current_offset = offset;

  success = TRUE;
  
  while(success){
    /* parse symbol */
    g_regex_match(symbol_regex, polynomial + current_offset, 0, &symbol_match_info);
    
    if(g_match_info_matches(symbol_match_info)){
      gint start_pos, end_pos;
      
      g_match_info_fetch_pos(symbol_match_info,
			     0,
			     &start_pos, &end_pos);
      
      current_offset += end_pos;
    }else{
      success = FALSE;
    }
    
    g_match_info_free(symbol_match_info);

    if(!success){
      break;
    }
    
    /* parse exponent */
    if(exponent == NULL){
      exponent = (gchar *) g_malloc(2 * sizeof(gchar *));
    }else{
      exponent = (gchar *) g_realloc(exponent,
				     (exponent_count + 2) * sizeof(gchar *));
    }

    g_regex_match(exponent_regex, polynomial + current_offset, 0, &exponent_match_info);
    
    if(g_match_info_matches(exponent_match_info)){
      guint tmp_offset;
      gint start_pos, end_pos;
      gboolean tmp_is_signed;
      
      g_match_info_fetch_pos(exponent_match_info,
			     0,
			     &start_pos, &end_pos);
    
      exponent[exponent_count] = g_strdup_printf("%*.s",
						 end_pos - start_pos, polynomial + current_offset + start_pos);

      tmp_offset = current_offset;
      
      z = 1.0 + I * 0.0;

      tmp_is_signed = FALSE;
      
      if(polynomial[current_offset] == '+' ||
	 polynomial[current_offset] == '-'){
	tmp_offset++;

	tmp_is_signed = TRUE;
      }

      /* match pi */
      c = g_utf8_get_char(polynomial + tmp_offset);

      match_c = g_utf8_get_char(AGS_SYMBOLIC_PI);

      if(c == match_c){
	z *= M_PI;
    
	tmp_offset += strlen(AGS_SYMBOLIC_PI);
      }

      /* match euler */
      if(tmp_offset < end_pos){
	match_c = g_utf8_get_char(AGS_SYMBOLIC_EULER);

	if(c == match_c){
	  z *= M_E;

	  tmp_offset += strlen(AGS_SYMBOLIC_EULER);
	}
      }

      //TODO:JK: huh
      /* match more? */

      if(tmp_offset < end_pos){
	gchar *endptr;

	gdouble val;
    
	endptr = NULL;

	val = g_ascii_strtod(polynomial + tmp_offset,
			     &endptr);

	if(endptr != NULL){
	  z *= val;
	}
      }  

      if(polynomial[0] == '-'){
	z *= -1.0;
      }
      
      ags_complex_set(&(exponent_value[exponent_count]),
		      z);
      
      current_offset += end_pos;
    }else{
      exponent[exponent_count] = g_strdup("1");  
    }
	
    g_match_info_free(exponent_match_info);

    exponent[exponent_count + 1] = NULL;
        
    exponent_count++;
  }
  
  /* apply */
  g_rec_mutex_lock(solver_polynomial_mutex);

  solver_polynomial->polynomial = g_strdup(polynomial);

  solver_polynomial->coefficient = coefficient;

  solver_polynomial->symbol = symbol;
  solver_polynomial->exponent = exponent;
  
  solver_polynomial->coefficient_value = coefficient_value;
  solver_polynomial->exponent_value = exponent_value;

  g_rec_mutex_unlock(solver_polynomial_mutex);
}

/**
 * ags_solver_polynomial_add:
 * @polynomial_a: the first summand
 * @polynomial_b: the second summand
 * @error: return location of #GError-struct
 * 
 * Perform addition of @polynomial_a and @polynomial_b. Both summands need to have the very same
 * symbol and exponent, otherwise %NULL returned and error is appropriately set.
 * 
 * Returns: the newly instantiated #AgsSolverPolynomial or %NULL
 * 
 * Since: 3.2.0
 */
AgsSolverPolynomial*
ags_solver_polynomial_add(AgsSolverPolynomial *polynomial_a,
			  AgsSolverPolynomial *polynomial_b,
			  GError **error)
{
  AgsSolverPolynomial *solver_polynomial;
  
  gchar **symbol_a, **symbol_b;
  gchar **exponent_a, **exponent_b;

  double _Complex coefficient_a, coefficient_b;

  guint length;
  guint i;
  gboolean is_symbol_matching;
  gboolean is_exponent_matching;

  GRecMutex *polynomial_a_mutex;
  GRecMutex *polynomial_b_mutex;
  
  if(!AGS_IS_SOLVER_POLYNOMIAL(polynomial_a) ||
     !AGS_IS_SOLVER_POLYNOMIAL(polynomial_b)){
    return(NULL);
  }

  symbol_a = NULL;
  symbol_b = NULL;

  exponent_a = NULL;
  exponent_b = NULL;

  length = 0;
  
  is_symbol_matching = FALSE;
  is_exponent_matching = FALSE;
  
  g_object_get(polynomial_a,
	       "symbol", &symbol_a,
	       NULL);

  g_object_get(polynomial_b,
	       "symbol", &symbol_b,
	       NULL);

  is_symbol_matching = (symbol_a != NULL &&
			symbol_b != NULL &&
			ags_strv_equal(symbol_a,
				       symbol_b)) ? TRUE: FALSE;

  g_strfreev(symbol_a);
  g_strfreev(symbol_b);
  
  if(!is_symbol_matching){
    if(error != NULL){
      g_set_error(error,
		  AGS_SOLVER_POLYNOMIAL_ERROR,
		  AGS_SOLVER_POLYNOMIAL_ERROR_SYMBOL_MISMATCH,
		  "symbols don't match");
    }
    
    return(NULL);
  }

  g_object_get(polynomial_a,
	       "exponent", &exponent_a,
	       NULL);

  g_object_get(polynomial_b,
	       "exponent", &exponent_b,
	       NULL);

  is_exponent_matching = (exponent_a != NULL &&
			  exponent_b != NULL &&
			  ags_strv_equal(exponent_a,
					 exponent_b)) ? TRUE: FALSE;
  
  g_strfreev(exponent_a);
  g_strfreev(exponent_b);
  
  if(!is_exponent_matching){
    if(error != NULL){
      g_set_error(error,
		  AGS_SOLVER_POLYNOMIAL_ERROR,
		  AGS_SOLVER_POLYNOMIAL_ERROR_EXPONENT_MISMATCH,
		  "exponents don't match");
    }
    
    return(NULL);
  }

  symbol_a = NULL;
  symbol_b = NULL;

  exponent_a = NULL;
  exponent_b = NULL;
    
  solver_polynomial = ags_solver_polynomial_new();

  polynomial_a_mutex = AGS_SOLVER_POLYNOMIAL_GET_OBJ_MUTEX(polynomial_a);
  polynomial_b_mutex = AGS_SOLVER_POLYNOMIAL_GET_OBJ_MUTEX(polynomial_b);
  
  g_object_get(polynomial_a,
	       "symbol", &symbol_a,
	       "exponent", &exponent_a,
	       NULL);

  length = g_strv_length(symbol_a);

  if(length > 0){
    solver_polynomial->symbol = g_strdupv(symbol_a);
    solver_polynomial->exponent = g_strdupv(exponent_a);
    
    solver_polynomial->exponent_value = g_new(AgsComplex,
					      length);

    g_rec_mutex_lock(polynomial_a_mutex);
    
    for(i = 0; i < length; i++){
      ags_complex_set(&(solver_polynomial->exponent_value[i]),
		      ags_complex_get(&(polynomial_a->exponent_value[i])));
    }

    g_rec_mutex_unlock(polynomial_a_mutex);
  }
  
  /* get coeffiecient and summand of polynomial a */
  g_rec_mutex_lock(polynomial_a_mutex);

  coefficient_a = ags_complex_get(&(polynomial_a->coefficient_value));
  
  g_rec_mutex_unlock(polynomial_a_mutex);

  /* get coeffiecient and summand of polynomial b */
  g_rec_mutex_lock(polynomial_b_mutex);

  coefficient_b = ags_complex_get(&(polynomial_b->coefficient_value));

  g_rec_mutex_unlock(polynomial_b_mutex);

  /* add */
  ags_complex_set(solver_polynomial->coefficient_value,
		  coefficient_a + coefficient_b);

  if(solver_polynomial->coefficient_value->imag != 0.0){
    solver_polynomial->coefficient = g_strdup_printf("%f+𝑖%f", solver_polynomial->coefficient_value->real, solver_polynomial->coefficient_value->imag);
  }else{
    solver_polynomial->coefficient = g_strdup_printf("%f", solver_polynomial->coefficient_value->real);
  }

  ags_solver_polynomial_update(solver_polynomial);
  
  g_strfreev(symbol_a);
  g_strfreev(exponent_a);
  
  return(solver_polynomial);
}

/**
 * ags_solver_polynomial_subtract:
 * @polynomial_a: the minuend
 * @polynomial_b: the subtrahend
 * @error: return location of #GError-struct
 * 
 * Perform subtraction of @polynomial_a and @polynomial_b. Both minuend and subtrahend need to have the very same
 * symbol and exponent, otherwise %NULL returned and error is appropriately set.
 * 
 * Returns: the newly instantiated #AgsSolverPolynomial or %NULL
 * 
 * Since: 3.2.0
 */
AgsSolverPolynomial*
ags_solver_polynomial_subtract(AgsSolverPolynomial *polynomial_a,
			       AgsSolverPolynomial *polynomial_b,
			       GError **error)
{
  AgsSolverPolynomial *solver_polynomial;
  
  gchar **symbol_a, **symbol_b;
  gchar **exponent_a, **exponent_b;

  double _Complex coefficient_a, coefficient_b;

  guint length;
  guint i;
  gboolean is_symbol_matching;
  gboolean is_exponent_matching;

  GRecMutex *polynomial_a_mutex;
  GRecMutex *polynomial_b_mutex;
  
  if(!AGS_IS_SOLVER_POLYNOMIAL(polynomial_a) ||
     !AGS_IS_SOLVER_POLYNOMIAL(polynomial_b)){
    return(NULL);
  }

  symbol_a = NULL;
  symbol_b = NULL;

  exponent_a = NULL;
  exponent_b = NULL;

  length = 0;
  
  is_symbol_matching = FALSE;
  is_exponent_matching = FALSE;
  
  g_object_get(polynomial_a,
	       "symbol", &symbol_a,
	       NULL);

  g_object_get(polynomial_b,
	       "symbol", &symbol_b,
	       NULL);

  is_symbol_matching = (symbol_a != NULL &&
			symbol_b != NULL &&
			ags_strv_equal(symbol_a,
				       symbol_b)) ? TRUE: FALSE;

  g_strfreev(symbol_a);
  g_strfreev(symbol_b);
  
  if(!is_symbol_matching){
    if(error != NULL){
      g_set_error(error,
		  AGS_SOLVER_POLYNOMIAL_ERROR,
		  AGS_SOLVER_POLYNOMIAL_ERROR_SYMBOL_MISMATCH,
		  "symbols don't match");
    }
    
    return(NULL);
  }

  g_object_get(polynomial_a,
	       "exponent", &exponent_a,
	       NULL);

  g_object_get(polynomial_b,
	       "exponent", &exponent_b,
	       NULL);

  is_exponent_matching = (exponent_a != NULL &&
			  exponent_b != NULL &&
			  ags_strv_equal(exponent_a,
					 exponent_b)) ? TRUE: FALSE;
  
  g_strfreev(exponent_a);
  g_strfreev(exponent_b);
  
  if(!is_exponent_matching){
    if(error != NULL){
      g_set_error(error,
		  AGS_SOLVER_POLYNOMIAL_ERROR,
		  AGS_SOLVER_POLYNOMIAL_ERROR_EXPONENT_MISMATCH,
		  "exponents don't match");
    }
    
    return(NULL);
  }

  symbol_a = NULL;
  symbol_b = NULL;

  exponent_a = NULL;
  exponent_b = NULL;
    
  solver_polynomial = ags_solver_polynomial_new();

  polynomial_a_mutex = AGS_SOLVER_POLYNOMIAL_GET_OBJ_MUTEX(polynomial_a);
  polynomial_b_mutex = AGS_SOLVER_POLYNOMIAL_GET_OBJ_MUTEX(polynomial_b);
  
  g_object_get(polynomial_a,
	       "symbol", &symbol_a,
	       "exponent", &exponent_a,
	       NULL);

  length = g_strv_length(symbol_a);

  if(length > 0){
    solver_polynomial->symbol = g_strdupv(symbol_a);
    solver_polynomial->exponent = g_strdupv(exponent_a);
    
    solver_polynomial->exponent_value = g_new(AgsComplex,
					      length);

    g_rec_mutex_lock(polynomial_a_mutex);
    
    for(i = 0; i < length; i++){
      ags_complex_set(&(solver_polynomial->exponent_value[i]),
		      ags_complex_get(&(polynomial_a->exponent_value[i])));
    }

    g_rec_mutex_unlock(polynomial_a_mutex);
  }

  /* get coeffiecient and summand of polynomial a */
  g_rec_mutex_lock(polynomial_a_mutex);

  coefficient_a = ags_complex_get(&(polynomial_a->coefficient_value));
  
  g_rec_mutex_unlock(polynomial_a_mutex);

  /* get coeffiecient and summand of polynomial b */
  g_rec_mutex_lock(polynomial_b_mutex);

  coefficient_b = ags_complex_get(&(polynomial_b->coefficient_value));

  g_rec_mutex_unlock(polynomial_b_mutex);

  /* subtract */
  ags_complex_set(&(solver_polynomial->coefficient_value),
		  coefficient_a - coefficient_b);

  if(solver_polynomial->coefficient_value->imag != 0.0){
    solver_polynomial->coefficient = g_strdup_printf("%f+𝑖%f", solver_polynomial->coefficient_value->real, solver_polynomial->coefficient_value->imag);
  }else{
    solver_polynomial->coefficient = g_strdup_printf("%f", solver_polynomial->coefficient_value->real);
  }

  ags_solver_polynomial_update(solver_polynomial);
  
  g_strfreev(symbol_a);
  g_strfreev(exponent_a);
  
  return(solver_polynomial);
}

/**
 * ags_solver_polynomial_multiply:
 * @polynomial_a: the first factor
 * @polynomial_b: the second factor
 * @error: return location of #GError-struct
 * 
 * Perform multiplication of @polynomial_a and @polynomial_b.
 * 
 * Returns: the newly instantiated #AgsSolverPolynomial or %NULL
 * 
 * Since: 3.2.0
 */
AgsSolverPolynomial*
ags_solver_polynomial_multiply(AgsSolverPolynomial *polynomial_a,
			       AgsSolverPolynomial *polynomial_b,
			       GError **error)
{
  AgsSolverPolynomial *solver_polynomial;

  GMatchInfo *constant_exponent_match_info;

  gchar **symbol_a, **symbol_b;
  gchar **symbol;
  gchar **exponent_a, **exponent_b;
  gchar **exponent;
  
  double _Complex coefficient_value_a, coefficient_value_b;
  double _Complex exponent_value_a, exponent_value_b;
  guint a_length, b_length;
  guint i0, i1;
  gboolean is_exponent_constant;

  GRecMutex *polynomial_a_mutex;
  GRecMutex *polynomial_b_mutex;

  static GRegex *constant_exponent_regex = NULL;

  /* groups: #1 constants, #2 numeric with optional fraction */
  static const gchar *constant_exponent_pattern = "^([ℯ𝜋𝑖∞])|([0-9]+(\\.[0-9]+)?)(\\+([ℯ𝜋𝑖∞])|([0-9]+(\\.[0-9]+)?))?";
  
  if(!AGS_IS_SOLVER_POLYNOMIAL(polynomial_a) ||
     !AGS_IS_SOLVER_POLYNOMIAL(polynomial_b)){
    return(NULL);
  }

  /* compile regex */
  g_mutex_lock(&regex_mutex);

  if(constant_exponent_regex == NULL){
    GError *error;
    
    error = NULL;
    constant_exponent_regex = g_regex_new(constant_exponent_pattern,
					  (G_REGEX_EXTENDED),
					  0,
					  &error);
    
    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
  }

  g_mutex_unlock(&regex_mutex);

  symbol_a = NULL;
  symbol_b = NULL;

  exponent_a = NULL;
  exponent_b = NULL;
    
  solver_polynomial = ags_solver_polynomial_new();

  polynomial_a_mutex = AGS_SOLVER_POLYNOMIAL_GET_OBJ_MUTEX(polynomial_a);
  polynomial_b_mutex = AGS_SOLVER_POLYNOMIAL_GET_OBJ_MUTEX(polynomial_b);

  /* polynomial a */
  g_object_get(polynomial_a,
	       "symbol", &symbol_a,
	       "exponent", &exponent_a,
	       NULL);

  g_rec_mutex_lock(polynomial_a_mutex);
    
  coefficient_value_a = ags_complex_get(polynomial_a->coefficient_value);

  g_rec_mutex_unlock(polynomial_a_mutex);

  /* polynomial b */
  g_object_get(polynomial_b,
	       "symbol", &symbol_b,
	       "exponent", &exponent_b,
	       NULL);

  g_rec_mutex_lock(polynomial_b_mutex);
    
  coefficient_value_b = ags_complex_get(polynomial_b->coefficient_value);

  g_rec_mutex_unlock(polynomial_b_mutex);

  /* check exponent */
  a_length = g_strv_length(symbol_a);
  b_length = g_strv_length(symbol_b);

  is_exponent_constant = TRUE;

  for(i0 = 0; is_exponent_constant && i0 < a_length; i0++){
    if(exponent_a[i0] != NULL){
      g_regex_match(constant_exponent_regex, exponent_a[i0], 0, &constant_exponent_match_info);

      if(g_match_info_matches(constant_exponent_match_info)){
	gint start_pos, end_pos;

	g_match_info_fetch_pos(constant_exponent_match_info,
			       0,
			       &start_pos, &end_pos);

	if(end_pos < strlen(exponent_a[i0])){
	  is_exponent_constant = FALSE;
	}
      }else{
	is_exponent_constant = FALSE;
      }
      
      g_match_info_free(constant_exponent_match_info);
    }    
  }

  for(i0 = 0; is_exponent_constant && i0 < b_length; i0++){
    if(exponent_b[i0] != NULL){
      g_regex_match(constant_exponent_regex, exponent_b[i0], 0, &constant_exponent_match_info);

      if(g_match_info_matches(constant_exponent_match_info)){
	gint start_pos, end_pos;

	g_match_info_fetch_pos(constant_exponent_match_info,
			       0,
			       &start_pos, &end_pos);

	if(end_pos < strlen(exponent_b[i0])){
	  is_exponent_constant = FALSE;
	}
      }else{
	is_exponent_constant = FALSE;
      }
      
      g_match_info_free(constant_exponent_match_info);
    }    
  }
  
  if(!is_exponent_constant){
    g_strfreev(symbol_a);
    g_strfreev(exponent_a);

    g_strfreev(symbol_b);
    g_strfreev(exponent_b);
    
    if(error != NULL){
      g_set_error(error,
		  AGS_SOLVER_POLYNOMIAL_ERROR,
		  AGS_SOLVER_POLYNOMIAL_ERROR_EXPONENT_NOT_CONSTANT,
		  "exponent isn't constant");
    }
    
    return(NULL);
  }
  
  /* multiply */
  ags_complex_set(solver_polynomial->coefficient_value,
		  coefficient_value_a * coefficient_value_b);

  if(solver_polynomial->coefficient_value->imag != 0.0){
    solver_polynomial->coefficient = g_strdup_printf("%f+𝑖%f", solver_polynomial->coefficient_value->real, solver_polynomial->coefficient_value->imag);
  }else{
    solver_polynomial->coefficient = g_strdup_printf("%f", solver_polynomial->coefficient_value->real);
  }

  symbol = NULL;
  exponent = NULL;
  
  for(i0 = 0; i0 < a_length || i0 < b_length; i0++){
    gboolean symbol_matched;

    symbol_matched = FALSE;

    if(i0 < a_length){
      for(i1 = 0; i1 < b_length; i1++){
	if(!g_strcmp0(symbol_a[i0], symbol_b[i1])){
	  double _Complex z;
	  
	  /* exponent value a */
	  g_rec_mutex_lock(polynomial_a_mutex);
    
	  exponent_value_a = ags_complex_get(&(polynomial_a->exponent_value[i0]));

	  g_rec_mutex_unlock(polynomial_a_mutex);

	  /* exponent value b */
	  g_rec_mutex_lock(polynomial_b_mutex);
    
	  exponent_value_b = ags_complex_get(&(polynomial_b->exponent_value[i1]));

	  g_rec_mutex_unlock(polynomial_b_mutex);

	  /* multiply */
	  if(solver_polynomial->symbol == NULL){
	    solver_polynomial->symbol = (gchar **) g_malloc(2 * sizeof(gchar *));
	  }else{
	    solver_polynomial->symbol = (gchar **) g_realloc(solver_polynomial->symbol,
							     (i0 + 2) * sizeof(gchar *));
	  }

	  solver_polynomial->symbol[i0] = g_strdup(symbol_a[i0]);
	  solver_polynomial->symbol[i0 + 1] = NULL;
	  
	  if(solver_polynomial->exponent_value == NULL){
	    solver_polynomial->exponent_value = (AgsComplex *) g_new(AgsComplex,
								     1);
	  }else{
	    solver_polynomial->exponent_value = (AgsComplex *) g_renew(AgsComplex,
								       solver_polynomial->exponent_value,
								       i0 + 1);
	  }

	  z = exponent_value_a + exponent_value_b;
	  ags_complex_set(&(solver_polynomial->exponent_value[i0]),
			  z);
	  
	  if(solver_polynomial->exponent == NULL){
	    solver_polynomial->exponent = (gchar **) g_malloc(2 * sizeof(gchar *));
	  }else{
	    solver_polynomial->exponent = (gchar **) g_realloc(solver_polynomial->exponent,
							     (i0 + 2) * sizeof(gchar *));
	  }

	  if(cimag(z) != 0.0){
	    solver_polynomial->exponent[i0] = g_strdup_printf("%f+𝑖%f", creal(z), cimag(z));
	  }else{
	    solver_polynomial->exponent[i0] = g_strdup_printf("%f", creal(z));
	  }
	  
	  solver_polynomial->exponent[i0 + 1] = NULL;

	  symbol_matched = TRUE;

	  break;
	}
      }
    }
    
    if(!symbol_matched){
      if(i0 < a_length){
	if(solver_polynomial->exponent_value == NULL){
	  solver_polynomial->exponent_value = (AgsComplex *) g_new(AgsComplex,
								   1);
	}else{
	  solver_polynomial->exponent_value = (AgsComplex *) g_renew(AgsComplex,
								     solver_polynomial->exponent_value,
								     i0 + 1);
	}

	/* exponent value a */
	g_rec_mutex_lock(polynomial_a_mutex);
    
	exponent_value_a = ags_complex_get(&(polynomial_a->exponent_value[i0]));

	g_rec_mutex_unlock(polynomial_a_mutex);

	/* multiply */
	if(solver_polynomial->symbol == NULL){
	  solver_polynomial->symbol = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  solver_polynomial->symbol = (gchar **) g_realloc(solver_polynomial->symbol,
							   (i0 + 2) * sizeof(gchar *));
	}

	solver_polynomial->symbol[i0] = g_strdup(symbol_a[i0]);
	solver_polynomial->symbol[i0 + 1] = NULL;
	
	ags_complex_set(&(solver_polynomial->exponent_value[i0]),
			exponent_value_a);

	if(solver_polynomial->exponent == NULL){
	  solver_polynomial->exponent = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  solver_polynomial->exponent = (gchar **) g_realloc(solver_polynomial->exponent,
							     (i0 + 2) * sizeof(gchar *));
	}

	if(cimag(exponent_value_a) != 0.0){
	  solver_polynomial->exponent[i0] = g_strdup_printf("%f+𝑖%f", creal(exponent_value_a), cimag(exponent_value_a));
	}else{
	  solver_polynomial->exponent[i0] = g_strdup_printf("%f", creal(exponent_value_a));
	}
	
	solver_polynomial->exponent[i0 + 1] = NULL;
      }else{
	if(solver_polynomial->exponent_value == NULL){
	  solver_polynomial->exponent_value = (AgsComplex *) g_new(AgsComplex,
								   1);
	}else{
	  solver_polynomial->exponent_value = (AgsComplex *) g_renew(AgsComplex,
								     solver_polynomial->exponent_value,
								     i0 + 1);
	}

	/* exponent value b */
	g_rec_mutex_lock(polynomial_b_mutex);
    
	exponent_value_b = ags_complex_get(&(polynomial_b->exponent_value[i0]));

	g_rec_mutex_unlock(polynomial_b_mutex);

	/* multiply */
	if(solver_polynomial->symbol == NULL){
	  solver_polynomial->symbol = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  solver_polynomial->symbol = (gchar **) g_realloc(solver_polynomial->symbol,
							   (i0 + 2) * sizeof(gchar *));
	}

	solver_polynomial->symbol[i0] = g_strdup(symbol_b[i0]);
	solver_polynomial->symbol[i0 + 1] = NULL;

	ags_complex_set(&(solver_polynomial->exponent_value[i0]),
			exponent_value_b);

	if(solver_polynomial->exponent == NULL){
	  solver_polynomial->exponent = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  solver_polynomial->exponent = (gchar **) g_realloc(solver_polynomial->exponent,
							     (i0 + 2) * sizeof(gchar *));
	}

	if(cimag(exponent_value_b) != 0.0){
	  solver_polynomial->exponent[i0] = g_strdup_printf("%f+𝑖%f", creal(exponent_value_b), cimag(exponent_value_b));
	}else{
	  solver_polynomial->exponent[i0] = g_strdup_printf("%f", creal(exponent_value_b));
	}
	
	solver_polynomial->exponent[i0 + 1] = NULL;
      }
    }
  }

  ags_solver_polynomial_update(solver_polynomial);

  g_strfreev(symbol_a);
  g_strfreev(exponent_a);

  g_strfreev(symbol_b);
  g_strfreev(exponent_b);
  
  return(solver_polynomial);
}

/**
 * ags_solver_polynomial_divide:
 * @polynomial_a: the dividend
 * @polynomial_b: the divisor
 * @error: return location of #GError-struct
 * 
 * Perform division of @polynomial_a and @polynomial_b.
 * 
 * Returns: the newly instantiated #AgsSolverPolynomial or %NULL
 * 
 * Since: 3.2.0
 */
AgsSolverPolynomial*
ags_solver_polynomial_divide(AgsSolverPolynomial *polynomial_a,
			     AgsSolverPolynomial *polynomial_b,
			     GError **error)
{
  AgsSolverPolynomial *solver_polynomial;

  GMatchInfo *constant_exponent_match_info;

  gchar **symbol_a, **symbol_b;
  gchar **symbol;
  gchar **exponent_a, **exponent_b;
  gchar **exponent;
  
  double _Complex coefficient_value_a, coefficient_value_b;
  double _Complex exponent_value_a, exponent_value_b;
  guint a_length, b_length;
  guint i0, i1;
  gboolean is_exponent_constant;

  GRecMutex *polynomial_a_mutex;
  GRecMutex *polynomial_b_mutex;

  static GRegex *constant_exponent_regex = NULL;

  /* groups: #1 constants, #2 numeric with optional fraction */
  static const gchar *constant_exponent_pattern = "^([ℯ𝜋𝑖∞])|([0-9]+(\\.[0-9]+)?)(\\+([ℯ𝜋𝑖∞])|([0-9]+(\\.[0-9]+)?))?";
  
  if(!AGS_IS_SOLVER_POLYNOMIAL(polynomial_a) ||
     !AGS_IS_SOLVER_POLYNOMIAL(polynomial_b)){
    return(NULL);
  }

  /* compile regex */
  g_mutex_lock(&regex_mutex);

  if(constant_exponent_regex == NULL){
    GError *error;
    
    error = NULL;
    constant_exponent_regex = g_regex_new(constant_exponent_pattern,
					  (G_REGEX_EXTENDED),
					  0,
					  &error);
    
    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
  }

  g_mutex_unlock(&regex_mutex);

  symbol_a = NULL;
  symbol_b = NULL;

  exponent_a = NULL;
  exponent_b = NULL;
    
  solver_polynomial = ags_solver_polynomial_new();

  polynomial_a_mutex = AGS_SOLVER_POLYNOMIAL_GET_OBJ_MUTEX(polynomial_a);
  polynomial_b_mutex = AGS_SOLVER_POLYNOMIAL_GET_OBJ_MUTEX(polynomial_b);

  /* polynomial a */
  g_object_get(polynomial_a,
	       "symbol", &symbol_a,
	       "exponent", &exponent_a,
	       NULL);

  g_rec_mutex_lock(polynomial_a_mutex);
    
  coefficient_value_a = ags_complex_get(polynomial_a->coefficient_value);

  g_rec_mutex_unlock(polynomial_a_mutex);

  /* polynomial b */
  g_object_get(polynomial_b,
	       "symbol", &symbol_b,
	       "exponent", &exponent_b,
	       NULL);

  g_rec_mutex_lock(polynomial_b_mutex);
    
  coefficient_value_b = ags_complex_get(polynomial_b->coefficient_value);

  g_rec_mutex_unlock(polynomial_b_mutex);

  /* check exponent */
  a_length = g_strv_length(symbol_a);
  b_length = g_strv_length(symbol_b);

  is_exponent_constant = TRUE;

  for(i0 = 0; is_exponent_constant && i0 < a_length; i0++){
    if(exponent_a[i0] != NULL){
      g_regex_match(constant_exponent_regex, exponent_a[i0], 0, &constant_exponent_match_info);

      if(g_match_info_matches(constant_exponent_match_info)){
	gint start_pos, end_pos;

	g_match_info_fetch_pos(constant_exponent_match_info,
			       0,
			       &start_pos, &end_pos);

	if(end_pos < strlen(exponent_a[i0])){
	  is_exponent_constant = FALSE;
	}
      }else{
	is_exponent_constant = FALSE;
      }
      
      g_match_info_free(constant_exponent_match_info);
    }    
  }

  for(i0 = 0; is_exponent_constant && i0 < b_length; i0++){
    if(exponent_b[i0] != NULL){
      g_regex_match(constant_exponent_regex, exponent_b[i0], 0, &constant_exponent_match_info);

      if(g_match_info_matches(constant_exponent_match_info)){
	gint start_pos, end_pos;

	g_match_info_fetch_pos(constant_exponent_match_info,
			       0,
			       &start_pos, &end_pos);

	if(end_pos < strlen(exponent_b[i0])){
	  is_exponent_constant = FALSE;
	}
      }else{
	is_exponent_constant = FALSE;
      }
      
      g_match_info_free(constant_exponent_match_info);
    }    
  }
  
  if(!is_exponent_constant){
    g_strfreev(symbol_a);
    g_strfreev(exponent_a);

    g_strfreev(symbol_b);
    g_strfreev(exponent_b);
    
    if(error != NULL){
      g_set_error(error,
		  AGS_SOLVER_POLYNOMIAL_ERROR,
		  AGS_SOLVER_POLYNOMIAL_ERROR_EXPONENT_NOT_CONSTANT,
		  "exponent isn't constant");
    }
    
    return(NULL);
  }
  
  /* divide */
  ags_complex_set(solver_polynomial->coefficient_value,
		  coefficient_value_a / coefficient_value_b);

  if(solver_polynomial->coefficient_value->imag != 0.0){
    solver_polynomial->coefficient = g_strdup_printf("%f+𝑖%f", solver_polynomial->coefficient_value->real, solver_polynomial->coefficient_value->imag);
  }else{
    solver_polynomial->coefficient = g_strdup_printf("%f", solver_polynomial->coefficient_value->real);
  }

  symbol = NULL;
  exponent = NULL;
  
  for(i0 = 0; i0 < a_length || i0 < b_length; i0++){
    gboolean symbol_matched;

    symbol_matched = FALSE;

    if(i0 < a_length){
      for(i1 = 0; i1 < b_length; i1++){
	if(!g_strcmp0(symbol_a[i0], symbol_b[i1])){
	  double _Complex z;
	  
	  /* exponent value a */
	  g_rec_mutex_lock(polynomial_a_mutex);
    
	  exponent_value_a = ags_complex_get(&(polynomial_a->exponent_value[i0]));

	  g_rec_mutex_unlock(polynomial_a_mutex);

	  /* exponent value b */
	  g_rec_mutex_lock(polynomial_b_mutex);
    
	  exponent_value_b = ags_complex_get(&(polynomial_b->exponent_value[i1]));

	  g_rec_mutex_unlock(polynomial_b_mutex);

	  /* divide */
	  if(solver_polynomial->symbol == NULL){
	    solver_polynomial->symbol = (gchar **) g_malloc(2 * sizeof(gchar *));
	  }else{
	    solver_polynomial->symbol = (gchar **) g_realloc(solver_polynomial->symbol,
							     (i0 + 2) * sizeof(gchar *));
	  }

	  solver_polynomial->symbol[i0] = g_strdup(symbol_a[i0]);
	  solver_polynomial->symbol[i0 + 1] = NULL;
	  
	  if(solver_polynomial->exponent_value == NULL){
	    solver_polynomial->exponent_value = (AgsComplex *) g_new(AgsComplex,
								     1);
	  }else{
	    solver_polynomial->exponent_value = (AgsComplex *) g_renew(AgsComplex,
								       solver_polynomial->exponent_value,
								       i0 + 1);
	  }

	  z = exponent_value_a - exponent_value_b;
	  ags_complex_set(&(solver_polynomial->exponent_value[i0]),
			  z);
	  
	  if(solver_polynomial->exponent == NULL){
	    solver_polynomial->exponent = (gchar **) g_malloc(2 * sizeof(gchar *));
	  }else{
	    solver_polynomial->exponent = (gchar **) g_realloc(solver_polynomial->exponent,
							     (i0 + 2) * sizeof(gchar *));
	  }

	  if(cimag(z) != 0.0){
	    solver_polynomial->exponent[i0] = g_strdup_printf("%f+𝑖%f", creal(z), cimag(z));
	  }else{
	    solver_polynomial->exponent[i0] = g_strdup_printf("%f", creal(z));
	  }
	  
	  solver_polynomial->exponent[i0 + 1] = NULL;

	  symbol_matched = TRUE;

	  break;
	}
      }
    }
    
    if(!symbol_matched){
      if(i0 < a_length){
	if(solver_polynomial->exponent_value == NULL){
	  solver_polynomial->exponent_value = (AgsComplex *) g_new(AgsComplex,
								   1);
	}else{
	  solver_polynomial->exponent_value = (AgsComplex *) g_renew(AgsComplex,
								     solver_polynomial->exponent_value,
								     i0 + 1);
	}

	/* exponent value a */
	g_rec_mutex_lock(polynomial_a_mutex);
    
	exponent_value_a = ags_complex_get(&(polynomial_a->exponent_value[i0]));

	g_rec_mutex_unlock(polynomial_a_mutex);

	/* divide */
	if(solver_polynomial->symbol == NULL){
	  solver_polynomial->symbol = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  solver_polynomial->symbol = (gchar **) g_realloc(solver_polynomial->symbol,
							   (i0 + 2) * sizeof(gchar *));
	}

	solver_polynomial->symbol[i0] = g_strdup(symbol_a[i0]);
	solver_polynomial->symbol[i0 + 1] = NULL;
	
	ags_complex_set(&(solver_polynomial->exponent_value[i0]),
			exponent_value_a);

	if(solver_polynomial->exponent == NULL){
	  solver_polynomial->exponent = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  solver_polynomial->exponent = (gchar **) g_realloc(solver_polynomial->exponent,
							     (i0 + 2) * sizeof(gchar *));
	}

	if(cimag(exponent_value_a) != 0.0){
	  solver_polynomial->exponent[i0] = g_strdup_printf("%f+𝑖%f", creal(exponent_value_a), cimag(exponent_value_a));
	}else{
	  solver_polynomial->exponent[i0] = g_strdup_printf("%f", creal(exponent_value_a));
	}
	
	solver_polynomial->exponent[i0 + 1] = NULL;
      }else{
	if(solver_polynomial->exponent_value == NULL){
	  solver_polynomial->exponent_value = (AgsComplex *) g_new(AgsComplex,
								   1);
	}else{
	  solver_polynomial->exponent_value = (AgsComplex *) g_renew(AgsComplex,
								     solver_polynomial->exponent_value,
								     i0 + 1);
	}

	/* exponent value b */
	g_rec_mutex_lock(polynomial_b_mutex);
    
	exponent_value_b = ags_complex_get(&(polynomial_b->exponent_value[i0]));

	g_rec_mutex_unlock(polynomial_b_mutex);

	/* divide */
	if(solver_polynomial->symbol == NULL){
	  solver_polynomial->symbol = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  solver_polynomial->symbol = (gchar **) g_realloc(solver_polynomial->symbol,
							   (i0 + 2) * sizeof(gchar *));
	}

	solver_polynomial->symbol[i0] = g_strdup(symbol_b[i0]);
	solver_polynomial->symbol[i0 + 1] = NULL;

	ags_complex_set(&(solver_polynomial->exponent_value[i0]),
			-1.0 * exponent_value_b);

	if(solver_polynomial->exponent == NULL){
	  solver_polynomial->exponent = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  solver_polynomial->exponent = (gchar **) g_realloc(solver_polynomial->exponent,
							     (i0 + 2) * sizeof(gchar *));
	}

	if(cimag(exponent_value_b) != 0.0){
	  solver_polynomial->exponent[i0] = g_strdup_printf("%f+𝑖%f", creal(exponent_value_b), cimag(exponent_value_b));
	}else{
	  solver_polynomial->exponent[i0] = g_strdup_printf("%f", creal(exponent_value_b));
	}
	
	solver_polynomial->exponent[i0 + 1] = NULL;
      }
    }
  }

  ags_solver_polynomial_update(solver_polynomial);

  g_strfreev(symbol_a);
  g_strfreev(exponent_a);

  g_strfreev(symbol_b);
  g_strfreev(exponent_b);
  
  return(solver_polynomial);
}

/**
 * ags_solver_polynomial_raise_power:
 * @polynomial_a: the base
 * @polynomial_b: the exponent
 * @error: return location of #GError-struct
 * 
 * Perform raising power of @polynomial_a and @polynomial_b.
 * 
 * Returns: the newly instantiated #AgsSolverPolynomial or %NULL
 * 
 * Since: 3.2.0
 */
AgsSolverPolynomial*
ags_solver_polynomial_raise_power(AgsSolverPolynomial *polynomial_a,
				  AgsSolverPolynomial *polynomial_b,
				  GError **error)
{
  AgsSolverPolynomial *solver_polynomial;

  GMatchInfo *constant_exponent_match_info;

  gchar **symbol_a, **symbol_b;
  gchar **symbol;
  gchar **exponent_a, **exponent_b;
  gchar **exponent;
  
  double _Complex coefficient_value_a, coefficient_value_b;
  double _Complex exponent_value_a, exponent_value_b;
  guint a_length, b_length;
  guint i0, i1;
  gboolean is_exponent_constant;

  GRecMutex *polynomial_a_mutex;
  GRecMutex *polynomial_b_mutex;

  static GRegex *constant_exponent_regex = NULL;

  /* groups: #1 constants, #2 numeric with optional fraction */
  static const gchar *constant_exponent_pattern = "^([ℯ𝜋𝑖∞])|([0-9]+(\\.[0-9]+)?)(\\+([ℯ𝜋𝑖∞])|([0-9]+(\\.[0-9]+)?))?";
  
  if(!AGS_IS_SOLVER_POLYNOMIAL(polynomial_a) ||
     !AGS_IS_SOLVER_POLYNOMIAL(polynomial_b)){
    return(NULL);
  }

  /* compile regex */
  g_mutex_lock(&regex_mutex);

  if(constant_exponent_regex == NULL){
    GError *error;
    
    error = NULL;
    constant_exponent_regex = g_regex_new(constant_exponent_pattern,
					  (G_REGEX_EXTENDED),
					  0,
					  &error);
    
    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
  }

  g_mutex_unlock(&regex_mutex);

  symbol_a = NULL;
  symbol_b = NULL;

  exponent_a = NULL;
  exponent_b = NULL;
    
  solver_polynomial = ags_solver_polynomial_new();

  polynomial_a_mutex = AGS_SOLVER_POLYNOMIAL_GET_OBJ_MUTEX(polynomial_a);
  polynomial_b_mutex = AGS_SOLVER_POLYNOMIAL_GET_OBJ_MUTEX(polynomial_b);

  /* polynomial a */
  g_object_get(polynomial_a,
	       "symbol", &symbol_a,
	       "exponent", &exponent_a,
	       NULL);

  g_rec_mutex_lock(polynomial_a_mutex);
    
  coefficient_value_a = ags_complex_get(polynomial_a->coefficient_value);

  g_rec_mutex_unlock(polynomial_a_mutex);

  /* polynomial b */
  g_object_get(polynomial_b,
	       "symbol", &symbol_b,
	       "exponent", &exponent_b,
	       NULL);

  g_rec_mutex_lock(polynomial_b_mutex);
    
  coefficient_value_b = ags_complex_get(polynomial_b->coefficient_value);

  g_rec_mutex_unlock(polynomial_b_mutex);

  /* check */
  a_length = g_strv_length(symbol_a);
  b_length = g_strv_length(symbol_b);

  is_exponent_constant = TRUE;

  for(i0 = 0; is_exponent_constant && i0 < a_length; i0++){
    if(exponent_a[i0] != NULL){
      g_regex_match(constant_exponent_regex, exponent_a[i0], 0, &constant_exponent_match_info);

      if(g_match_info_matches(constant_exponent_match_info)){
	gint start_pos, end_pos;

	g_match_info_fetch_pos(constant_exponent_match_info,
			       0,
			       &start_pos, &end_pos);

	if(end_pos < strlen(exponent_a[i0])){
	  is_exponent_constant = FALSE;
	}
      }else{
	is_exponent_constant = FALSE;
      }
      
      g_match_info_free(constant_exponent_match_info);
    }    
  }

  for(i0 = 0; is_exponent_constant && i0 < b_length; i0++){
    if(exponent_b[i0] != NULL){
      g_regex_match(constant_exponent_regex, exponent_b[i0], 0, &constant_exponent_match_info);

      if(g_match_info_matches(constant_exponent_match_info)){
	gint start_pos, end_pos;

	g_match_info_fetch_pos(constant_exponent_match_info,
			       0,
			       &start_pos, &end_pos);

	if(end_pos < strlen(exponent_b[i0])){
	  is_exponent_constant = FALSE;
	}
      }else{
	is_exponent_constant = FALSE;
      }
      
      g_match_info_free(constant_exponent_match_info);
    }    
  }
  
  if(!is_exponent_constant){
    g_strfreev(symbol_a);
    g_strfreev(exponent_a);

    g_strfreev(symbol_b);
    g_strfreev(exponent_b);
    
    if(error != NULL){
      g_set_error(error,
		  AGS_SOLVER_POLYNOMIAL_ERROR,
		  AGS_SOLVER_POLYNOMIAL_ERROR_EXPONENT_NOT_CONSTANT,
		  "exponent isn't constant");
    }
    
    return(NULL);
  }
  
  /* raise power */
  ags_complex_set(solver_polynomial->coefficient_value,
		  cpow(coefficient_value_a, coefficient_value_b));

  if(solver_polynomial->coefficient_value->imag != 0.0){
    solver_polynomial->coefficient = g_strdup_printf("%f+𝑖%f", solver_polynomial->coefficient_value->real, solver_polynomial->coefficient_value->imag);
  }else{
    solver_polynomial->coefficient = g_strdup_printf("%f", solver_polynomial->coefficient_value->real);
  }

  symbol = NULL;
  exponent = NULL;
  
  for(i0 = 0; i0 < a_length || i0 < b_length; i0++){
    gboolean symbol_matched;

    symbol_matched = FALSE;

    if(i0 < a_length){
      for(i1 = 0; i1 < b_length; i1++){
	if(!g_strcmp0(symbol_a[i0], symbol_b[i1])){
	  double _Complex z;
	  
	  /* exponent value a */
	  g_rec_mutex_lock(polynomial_a_mutex);
    
	  exponent_value_a = ags_complex_get(&(polynomial_a->exponent_value[i0]));

	  g_rec_mutex_unlock(polynomial_a_mutex);

	  /* exponent value b */
	  g_rec_mutex_lock(polynomial_b_mutex);
    
	  exponent_value_b = ags_complex_get(&(polynomial_b->exponent_value[i1]));

	  g_rec_mutex_unlock(polynomial_b_mutex);

	  /* raise power */
	  if(solver_polynomial->symbol == NULL){
	    solver_polynomial->symbol = (gchar **) g_malloc(2 * sizeof(gchar *));
	  }else{
	    solver_polynomial->symbol = (gchar **) g_realloc(solver_polynomial->symbol,
							     (i0 + 2) * sizeof(gchar *));
	  }

	  solver_polynomial->symbol[i0] = g_strdup(symbol_a[i0]);
	  solver_polynomial->symbol[i0 + 1] = NULL;
	  
	  if(solver_polynomial->exponent_value == NULL){
	    solver_polynomial->exponent_value = (AgsComplex *) g_new(AgsComplex,
								     1);
	  }else{
	    solver_polynomial->exponent_value = (AgsComplex *) g_renew(AgsComplex,
								       solver_polynomial->exponent_value,
								       i0 + 1);
	  }

	  z = exponent_value_a * exponent_value_b;
	  ags_complex_set(&(solver_polynomial->exponent_value[i0]),
			  z);
	  
	  if(solver_polynomial->exponent == NULL){
	    solver_polynomial->exponent = (gchar **) g_malloc(2 * sizeof(gchar *));
	  }else{
	    solver_polynomial->exponent = (gchar **) g_realloc(solver_polynomial->exponent,
							     (i0 + 2) * sizeof(gchar *));
	  }

	  if(cimag(z) != 0.0){
	    solver_polynomial->exponent[i0] = g_strdup_printf("%f+𝑖%f", creal(z), cimag(z));
	  }else{
	    solver_polynomial->exponent[i0] = g_strdup_printf("%f", creal(z));
	  }
	  
	  solver_polynomial->exponent[i0 + 1] = NULL;

	  symbol_matched = TRUE;

	  break;
	}
      }
    }
    
    if(!symbol_matched){
      if(i0 < a_length){
	if(solver_polynomial->exponent_value == NULL){
	  solver_polynomial->exponent_value = (AgsComplex *) g_new(AgsComplex,
								   1);
	}else{
	  solver_polynomial->exponent_value = (AgsComplex *) g_renew(AgsComplex,
								     solver_polynomial->exponent_value,
								     i0 + 1);
	}

	/* exponent value a */
	g_rec_mutex_lock(polynomial_a_mutex);
    
	exponent_value_a = ags_complex_get(&(polynomial_a->exponent_value[i0]));

	g_rec_mutex_unlock(polynomial_a_mutex);

	/* raise power */
	if(solver_polynomial->symbol == NULL){
	  solver_polynomial->symbol = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  solver_polynomial->symbol = (gchar **) g_realloc(solver_polynomial->symbol,
							   (i0 + 2) * sizeof(gchar *));
	}

	solver_polynomial->symbol[i0] = g_strdup(symbol_a[i0]);
	solver_polynomial->symbol[i0 + 1] = NULL;
	
	ags_complex_set(&(solver_polynomial->exponent_value[i0]),
			exponent_value_a);

	if(solver_polynomial->exponent == NULL){
	  solver_polynomial->exponent = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  solver_polynomial->exponent = (gchar **) g_realloc(solver_polynomial->exponent,
							     (i0 + 2) * sizeof(gchar *));
	}

	if(cimag(exponent_value_a) != 0.0){
	  solver_polynomial->exponent[i0] = g_strdup_printf("%f+𝑖%f", creal(exponent_value_a), cimag(exponent_value_a));
	}else{
	  solver_polynomial->exponent[i0] = g_strdup_printf("%f", creal(exponent_value_a));
	}
	
	solver_polynomial->exponent[i0 + 1] = NULL;
      }else{
	if(solver_polynomial->exponent_value == NULL){
	  solver_polynomial->exponent_value = (AgsComplex *) g_new(AgsComplex,
								   1);
	}else{
	  solver_polynomial->exponent_value = (AgsComplex *) g_renew(AgsComplex,
								     solver_polynomial->exponent_value,
								     i0 + 1);
	}

	/* exponent value b */
	g_rec_mutex_lock(polynomial_b_mutex);
    
	exponent_value_b = ags_complex_get(&(polynomial_b->exponent_value[i0]));

	g_rec_mutex_unlock(polynomial_b_mutex);

	/* raise power */
	if(solver_polynomial->symbol == NULL){
	  solver_polynomial->symbol = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  solver_polynomial->symbol = (gchar **) g_realloc(solver_polynomial->symbol,
							   (i0 + 2) * sizeof(gchar *));
	}

	solver_polynomial->symbol[i0] = g_strdup(symbol_b[i0]);
	solver_polynomial->symbol[i0 + 1] = NULL;

	ags_complex_set(&(solver_polynomial->exponent_value[i0]),
			exponent_value_b);

	if(solver_polynomial->exponent == NULL){
	  solver_polynomial->exponent = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  solver_polynomial->exponent = (gchar **) g_realloc(solver_polynomial->exponent,
							     (i0 + 2) * sizeof(gchar *));
	}

	if(cimag(exponent_value_b) != 0.0){
	  solver_polynomial->exponent[i0] = g_strdup_printf("%f+𝑖%f", creal(exponent_value_b), cimag(exponent_value_b));
	}else{
	  solver_polynomial->exponent[i0] = g_strdup_printf("%f", creal(exponent_value_b));
	}
	
	solver_polynomial->exponent[i0 + 1] = NULL;
      }
    }
  }

  ags_solver_polynomial_update(solver_polynomial);

  g_strfreev(symbol_a);
  g_strfreev(exponent_a);

  g_strfreev(symbol_b);
  g_strfreev(exponent_b);
  
  return(solver_polynomial);
}

/**
 * ags_solver_polynomial_extract_root:
 * @polynomial_a: the base
 * @polynomial_b: the index of root
 * @error: return location of #GError-struct
 * 
 * Perform raising power of @polynomial_a and @polynomial_b.
 * 
 * Returns: the newly instantiated #AgsSolverPolynomial or %NULL
 * 
 * Since: 3.2.0
 */
AgsSolverPolynomial*
ags_solver_polynomial_extract_root(AgsSolverPolynomial *polynomial_a,
				   AgsSolverPolynomial *polynomial_b,
				   GError **error)
{
  AgsSolverPolynomial *solver_polynomial;

  GMatchInfo *constant_exponent_match_info;

  gchar **symbol_a, **symbol_b;
  gchar **symbol;
  gchar **exponent_a, **exponent_b;
  gchar **exponent;
  
  double _Complex coefficient_value_a, coefficient_value_b;
  double _Complex exponent_value_a, exponent_value_b;
  guint a_length, b_length;
  guint i0, i1;
  gboolean is_exponent_constant;

  GRecMutex *polynomial_a_mutex;
  GRecMutex *polynomial_b_mutex;

  static GRegex *constant_exponent_regex = NULL;

  /* groups: #1 constants, #2 numeric with optional fraction */
  static const gchar *constant_exponent_pattern = "^([ℯ𝜋𝑖∞])|([0-9]+(\\.[0-9]+)?)(\\+([ℯ𝜋𝑖∞])|([0-9]+(\\.[0-9]+)?))?";
  
  if(!AGS_IS_SOLVER_POLYNOMIAL(polynomial_a) ||
     !AGS_IS_SOLVER_POLYNOMIAL(polynomial_b)){
    return(NULL);
  }

  /* compile regex */
  g_mutex_lock(&regex_mutex);

  if(constant_exponent_regex == NULL){
    GError *error;
    
    error = NULL;
    constant_exponent_regex = g_regex_new(constant_exponent_pattern,
					  (G_REGEX_EXTENDED),
					  0,
					  &error);
    
    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
  }

  g_mutex_unlock(&regex_mutex);

  symbol_a = NULL;
  symbol_b = NULL;

  exponent_a = NULL;
  exponent_b = NULL;
    
  solver_polynomial = ags_solver_polynomial_new();

  polynomial_a_mutex = AGS_SOLVER_POLYNOMIAL_GET_OBJ_MUTEX(polynomial_a);
  polynomial_b_mutex = AGS_SOLVER_POLYNOMIAL_GET_OBJ_MUTEX(polynomial_b);

  /* polynomial a */
  g_object_get(polynomial_a,
	       "symbol", &symbol_a,
	       "exponent", &exponent_a,
	       NULL);

  g_rec_mutex_lock(polynomial_a_mutex);
    
  coefficient_value_a = ags_complex_get(polynomial_a->coefficient_value);

  g_rec_mutex_unlock(polynomial_a_mutex);

  /* polynomial b */
  g_object_get(polynomial_b,
	       "symbol", &symbol_b,
	       "exponent", &exponent_b,
	       NULL);

  g_rec_mutex_lock(polynomial_b_mutex);
    
  coefficient_value_b = ags_complex_get(polynomial_b->coefficient_value);

  g_rec_mutex_unlock(polynomial_b_mutex);

  /* check */
  a_length = g_strv_length(symbol_a);
  b_length = g_strv_length(symbol_b);

  is_exponent_constant = TRUE;

  for(i0 = 0; is_exponent_constant && i0 < a_length; i0++){
    if(exponent_a[i0] != NULL){
      g_regex_match(constant_exponent_regex, exponent_a[i0], 0, &constant_exponent_match_info);

      if(g_match_info_matches(constant_exponent_match_info)){
	gint start_pos, end_pos;

	g_match_info_fetch_pos(constant_exponent_match_info,
			       0,
			       &start_pos, &end_pos);

	if(end_pos < strlen(exponent_a[i0])){
	  is_exponent_constant = FALSE;
	}
      }else{
	is_exponent_constant = FALSE;
      }
      
      g_match_info_free(constant_exponent_match_info);
    }    
  }

  for(i0 = 0; is_exponent_constant && i0 < b_length; i0++){
    if(exponent_b[i0] != NULL){
      g_regex_match(constant_exponent_regex, exponent_b[i0], 0, &constant_exponent_match_info);

      if(g_match_info_matches(constant_exponent_match_info)){
	gint start_pos, end_pos;

	g_match_info_fetch_pos(constant_exponent_match_info,
			       0,
			       &start_pos, &end_pos);

	if(end_pos < strlen(exponent_b[i0])){
	  is_exponent_constant = FALSE;
	}
      }else{
	is_exponent_constant = FALSE;
      }
      
      g_match_info_free(constant_exponent_match_info);
    }    
  }
  
  if(!is_exponent_constant){
    g_strfreev(symbol_a);
    g_strfreev(exponent_a);

    g_strfreev(symbol_b);
    g_strfreev(exponent_b);
    
    if(error != NULL){
      g_set_error(error,
		  AGS_SOLVER_POLYNOMIAL_ERROR,
		  AGS_SOLVER_POLYNOMIAL_ERROR_EXPONENT_NOT_CONSTANT,
		  "exponent isn't constant");
    }
    
    return(NULL);
  }
  
  /* extract root */
  ags_complex_set(solver_polynomial->coefficient_value,
		  cpow(coefficient_value_a, 1.0 / coefficient_value_b));

  if(solver_polynomial->coefficient_value->imag != 0.0){
    solver_polynomial->coefficient = g_strdup_printf("%f+𝑖%f", solver_polynomial->coefficient_value->real, solver_polynomial->coefficient_value->imag);
  }else{
    solver_polynomial->coefficient = g_strdup_printf("%f", solver_polynomial->coefficient_value->real);
  }

  symbol = NULL;
  exponent = NULL;
  
  for(i0 = 0; i0 < a_length || i0 < b_length; i0++){
    gboolean symbol_matched;

    symbol_matched = FALSE;

    if(i0 < a_length){
      for(i1 = 0; i1 < b_length; i1++){
	if(!g_strcmp0(symbol_a[i0], symbol_b[i1])){
	  double _Complex z;
	  
	  /* exponent value a */
	  g_rec_mutex_lock(polynomial_a_mutex);
    
	  exponent_value_a = ags_complex_get(&(polynomial_a->exponent_value[i0]));

	  g_rec_mutex_unlock(polynomial_a_mutex);

	  /* exponent value b */
	  g_rec_mutex_lock(polynomial_b_mutex);
    
	  exponent_value_b = ags_complex_get(&(polynomial_b->exponent_value[i1]));

	  g_rec_mutex_unlock(polynomial_b_mutex);

	  /* extract root */
	  if(solver_polynomial->symbol == NULL){
	    solver_polynomial->symbol = (gchar **) g_malloc(2 * sizeof(gchar *));
	  }else{
	    solver_polynomial->symbol = (gchar **) g_realloc(solver_polynomial->symbol,
							     (i0 + 2) * sizeof(gchar *));
	  }

	  solver_polynomial->symbol[i0] = g_strdup(symbol_a[i0]);
	  solver_polynomial->symbol[i0 + 1] = NULL;
	  
	  if(solver_polynomial->exponent_value == NULL){
	    solver_polynomial->exponent_value = (AgsComplex *) g_new(AgsComplex,
								     1);
	  }else{
	    solver_polynomial->exponent_value = (AgsComplex *) g_renew(AgsComplex,
								       solver_polynomial->exponent_value,
								       i0 + 1);
	  }

	  z = exponent_value_a / exponent_value_b;
	  ags_complex_set(&(solver_polynomial->exponent_value[i0]),
			  z);
	  
	  if(solver_polynomial->exponent == NULL){
	    solver_polynomial->exponent = (gchar **) g_malloc(2 * sizeof(gchar *));
	  }else{
	    solver_polynomial->exponent = (gchar **) g_realloc(solver_polynomial->exponent,
							     (i0 + 2) * sizeof(gchar *));
	  }

	  if(cimag(z) != 0.0){
	    solver_polynomial->exponent[i0] = g_strdup_printf("%f+𝑖%f", creal(z), cimag(z));
	  }else{
	    solver_polynomial->exponent[i0] = g_strdup_printf("%f", creal(z));
	  }
	  
	  solver_polynomial->exponent[i0 + 1] = NULL;

	  symbol_matched = TRUE;

	  break;
	}
      }
    }
    
    if(!symbol_matched){
      if(i0 < a_length){
	if(solver_polynomial->exponent_value == NULL){
	  solver_polynomial->exponent_value = (AgsComplex *) g_new(AgsComplex,
								   1);
	}else{
	  solver_polynomial->exponent_value = (AgsComplex *) g_renew(AgsComplex,
								     solver_polynomial->exponent_value,
								     i0 + 1);
	}

	/* exponent value a */
	g_rec_mutex_lock(polynomial_a_mutex);
    
	exponent_value_a = ags_complex_get(&(polynomial_a->exponent_value[i0]));

	g_rec_mutex_unlock(polynomial_a_mutex);

	/* extract root */
	if(solver_polynomial->symbol == NULL){
	  solver_polynomial->symbol = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  solver_polynomial->symbol = (gchar **) g_realloc(solver_polynomial->symbol,
							   (i0 + 2) * sizeof(gchar *));
	}

	solver_polynomial->symbol[i0] = g_strdup(symbol_a[i0]);
	solver_polynomial->symbol[i0 + 1] = NULL;
	
	ags_complex_set(&(solver_polynomial->exponent_value[i0]),
			exponent_value_a);

	if(solver_polynomial->exponent == NULL){
	  solver_polynomial->exponent = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  solver_polynomial->exponent = (gchar **) g_realloc(solver_polynomial->exponent,
							     (i0 + 2) * sizeof(gchar *));
	}

	if(cimag(exponent_value_a) != 0.0){
	  solver_polynomial->exponent[i0] = g_strdup_printf("%f+𝑖%f", creal(exponent_value_a), cimag(exponent_value_a));
	}else{
	  solver_polynomial->exponent[i0] = g_strdup_printf("%f", creal(exponent_value_a));
	}
	
	solver_polynomial->exponent[i0 + 1] = NULL;
      }else{
	if(solver_polynomial->exponent_value == NULL){
	  solver_polynomial->exponent_value = (AgsComplex *) g_new(AgsComplex,
								   1);
	}else{
	  solver_polynomial->exponent_value = (AgsComplex *) g_renew(AgsComplex,
								     solver_polynomial->exponent_value,
								     i0 + 1);
	}

	/* exponent value b */
	g_rec_mutex_lock(polynomial_b_mutex);
    
	exponent_value_b = ags_complex_get(&(polynomial_b->exponent_value[i0]));

	g_rec_mutex_unlock(polynomial_b_mutex);

	/* extract root */
	if(solver_polynomial->symbol == NULL){
	  solver_polynomial->symbol = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  solver_polynomial->symbol = (gchar **) g_realloc(solver_polynomial->symbol,
							   (i0 + 2) * sizeof(gchar *));
	}

	solver_polynomial->symbol[i0] = g_strdup(symbol_b[i0]);
	solver_polynomial->symbol[i0 + 1] = NULL;

	ags_complex_set(&(solver_polynomial->exponent_value[i0]),
			1.0 / exponent_value_b);

	if(solver_polynomial->exponent == NULL){
	  solver_polynomial->exponent = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  solver_polynomial->exponent = (gchar **) g_realloc(solver_polynomial->exponent,
							     (i0 + 2) * sizeof(gchar *));
	}

	if(cimag(exponent_value_b) != 0.0){
	  solver_polynomial->exponent[i0] = g_strdup_printf("%f+𝑖%f", creal(exponent_value_b), cimag(exponent_value_b));
	}else{
	  solver_polynomial->exponent[i0] = g_strdup_printf("%f", creal(exponent_value_b));
	}
	
	solver_polynomial->exponent[i0 + 1] = NULL;
      }
    }
  }

  ags_solver_polynomial_update(solver_polynomial);

  g_strfreev(symbol_a);
  g_strfreev(exponent_a);

  g_strfreev(symbol_b);
  g_strfreev(exponent_b);
  
  return(solver_polynomial);
}

/**
 * ags_solver_polynomial_new:
 *
 * Instantiate a new #AgsSolverPolynomial.
 *
 * Returns: the new instance
 *
 * Since: 3.2.0
 */
AgsSolverPolynomial*
ags_solver_polynomial_new()
{
  AgsSolverPolynomial *solver_polynomial;
  
  solver_polynomial = g_object_new(AGS_TYPE_SOLVER_POLYNOMIAL,
				   NULL);

  return(solver_polynomial);
}
