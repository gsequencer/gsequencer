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

#include <ags/lib/ags_solver_term.h>

#include <stdlib.h>
#include <complex.h>

#include <ags/i18n.h>

void ags_solver_term_class_init(AgsSolverTermClass *solver_term);
void ags_solver_term_init (AgsSolverTerm *solver_term);
void ags_solver_term_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_solver_term_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_solver_term_finalize(GObject *gobject);

/**
 * SECTION:ags_solver_term
 * @short_description: solver term
 * @title: AgsSolverTerm
 * @section_id:
 * @include: ags/lib/ags_solver_term.h
 *
 * The #AgsSolverTerm contains the solver terms.
 */

enum{
  PROP_0,
  PROP_TERM,
  PROP_COEFFICIENT,
  PROP_SYMBOL,
  PROP_EXPONENT,
  PROP_COEFFICIENT_VALUE,
  PROP_EXPONENT_VALUE,
};

static gpointer ags_solver_term_parent_class = NULL;

GType
ags_solver_term_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_solver_term = 0;

    static const GTypeInfo ags_solver_term_info = {
      sizeof (AgsSolverTermClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_solver_term_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSolverTerm),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_solver_term_init,
    };

    ags_type_solver_term = g_type_register_static(G_TYPE_OBJECT,
						  "AgsSolverTerm",
						  &ags_solver_term_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_solver_term);
  }

  return g_define_type_id__volatile;
}

void
ags_solver_term_class_init(AgsSolverTermClass *solver_term)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_solver_term_parent_class = g_type_class_peek_parent(solver_term);

  /* GObjectClass */
  gobject = (GObjectClass *) solver_term;

  gobject->set_property = ags_solver_term_set_property;
  gobject->get_property = ags_solver_term_get_property;
  
  gobject->finalize = ags_solver_term_finalize;

  /* properties */
  /**
   * AgsSolverTerm:term:
   *
   * The assigned term.
   * 
   * Since: 3.2.0
   */
  param_spec = g_param_spec_string("term",
				   i18n_pspec("term of solver term"),
				   i18n_pspec("The term this solver term is assigned to"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TERM,
				  param_spec);

  /**
   * AgsSolverTerm:coefficient:
   *
   * The assigned coefficient.
   * 
   * Since: 3.2.0
   */
  param_spec = g_param_spec_string("coefficient",
				   i18n_pspec("coefficient of solver term"),
				   i18n_pspec("The coefficient this solver term is assigned to"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COEFFICIENT,
				  param_spec);

  /**
   * AgsSolverTerm:symbol:
   *
   * The assigned symbol.
   * 
   * Since: 3.2.0
   */
  param_spec = g_param_spec_string("symbol",
				   i18n_pspec("symbol of solver term"),
				   i18n_pspec("The symbol this solver term is assigned to"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYMBOL,
				  param_spec);

  /**
   * AgsSolverTerm:exponent:
   *
   * The assigned exponent.
   * 
   * Since: 3.2.0
   */
  param_spec = g_param_spec_string("exponent",
				   i18n_pspec("exponent of solver term"),
				   i18n_pspec("The exponent this solver term is assigned to"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EXPONENT,
				  param_spec);

  /**
   * AgsSolverTerm:coefficient-value:
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
   * AgsSolverTerm:exponent-value:
   *
   * Exponent value.
   * 
   * Since: 3.2.0
   */
  param_spec = g_param_spec_boxed("exponent-value",
				  i18n_pspec("exponent value"),
				  i18n_pspec("The exponent value"),
				  AGS_TYPE_COMPLEX,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EXPONENT_VALUE,
				  param_spec);
}

void
ags_solver_term_init(AgsSolverTerm *solver_term)
{
  complex z;

  solver_term->flags = 0;
  
  g_rec_mutex_init(&(solver_term->obj_mutex));

  solver_term->term = NULL;

  solver_term->coefficient = NULL;
  solver_term->symbol = NULL;
  solver_term->exponent = NULL;

  z = 1.0 + I * 0.0;
  ags_complex_set(&(solver_term->coefficient_value),
		  z);

  z = 0.0 + I * 0.0;
  ags_complex_set(&(solver_term->exponent_value),
		  z);
}

void
ags_solver_term_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsSolverTerm *solver_term;

  GRecMutex *solver_term_mutex;
  
  solver_term = AGS_SOLVER_TERM(gobject);

  /* solver term mutex */
  solver_term_mutex = AGS_SOLVER_TERM_GET_OBJ_MUTEX(solver_term);
  
  switch(prop_id){
  case PROP_TERM:
  {
    gchar *term;

    term = (gchar *) g_value_get_string(value);

    g_rec_mutex_lock(solver_term_mutex);

    if(solver_term->term == term){
      g_rec_mutex_unlock(solver_term_mutex);

      return;
    }
      
    if(solver_term->term != NULL){
      g_free(solver_term->term);
    }

    solver_term->term = g_strdup(term);

    g_rec_mutex_unlock(solver_term_mutex);
  }
  break;
  case PROP_COEFFICIENT:
  {
    gchar *coefficient;

    coefficient = (gchar *) g_value_get_string(value);

    g_rec_mutex_lock(solver_term_mutex);

    if(solver_term->coefficient == coefficient){
      g_rec_mutex_unlock(solver_term_mutex);

      return;
    }
      
    if(solver_term->coefficient != NULL){
      g_free(solver_term->coefficient);
    }

    solver_term->coefficient = g_strdup(coefficient);

    g_rec_mutex_unlock(solver_term_mutex);
  }
  break;
  case PROP_SYMBOL:
  {
    gchar *symbol;

    symbol = (gchar *) g_value_get_string(value);

    g_rec_mutex_lock(solver_term_mutex);

    if(solver_term->symbol == symbol){
      g_rec_mutex_unlock(solver_term_mutex);

      return;
    }
      
    if(solver_term->symbol != NULL){
      g_free(solver_term->symbol);
    }

    solver_term->symbol = g_strdup(symbol);

    g_rec_mutex_unlock(solver_term_mutex);
  }
  break;
  case PROP_EXPONENT:
  {
    gchar *exponent;

    exponent = (gchar *) g_value_get_string(value);

    g_rec_mutex_lock(solver_term_mutex);

    if(solver_term->exponent == exponent){
      g_rec_mutex_unlock(solver_term_mutex);

      return;
    }
      
    if(solver_term->exponent != NULL){
      g_free(solver_term->exponent);
    }

    solver_term->exponent = g_strdup(exponent);

    g_rec_mutex_unlock(solver_term_mutex);
  }
  break;
  case PROP_COEFFICIENT_VALUE:
  {
    AgsComplex *coefficient_value;

    coefficient_value = (AgsComplex *) g_value_get_boxed(value);

    g_rec_mutex_lock(solver_term_mutex);

    ags_complex_set(&(solver_term->coefficient_value),
		    ags_complex_get(coefficient_value));

    g_rec_mutex_unlock(solver_term_mutex);
  }
  break;    
  case PROP_EXPONENT_VALUE:
  {
    AgsComplex *exponent_value;

    exponent_value = (AgsComplex *) g_value_get_boxed(value);

    g_rec_mutex_lock(solver_term_mutex);

    ags_complex_set(&(solver_term->exponent_value),
		    ags_complex_get(exponent_value));

    g_rec_mutex_unlock(solver_term_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_solver_term_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsSolverTerm *solver_term;

  GRecMutex *solver_term_mutex;

  solver_term = AGS_SOLVER_TERM(gobject);

  /* solver term mutex */
  solver_term_mutex = AGS_SOLVER_TERM_GET_OBJ_MUTEX(solver_term);

  switch(prop_id){
  case PROP_TERM:
  {
    g_rec_mutex_lock(solver_term_mutex);

    g_value_set_string(value, solver_term->term);

    g_rec_mutex_unlock(solver_term_mutex);
  }
  break;
  case PROP_COEFFICIENT:
  {
    g_rec_mutex_lock(solver_term_mutex);

    g_value_set_string(value, solver_term->coefficient);

    g_rec_mutex_unlock(solver_term_mutex);
  }
  break;
  case PROP_SYMBOL:
  {
    g_rec_mutex_lock(solver_term_mutex);

    g_value_set_string(value, solver_term->symbol);

    g_rec_mutex_unlock(solver_term_mutex);
  }
  break;
  case PROP_EXPONENT:
  {
    g_rec_mutex_lock(solver_term_mutex);

    g_value_set_string(value, solver_term->exponent);

    g_rec_mutex_unlock(solver_term_mutex);
  }
  break;
  case PROP_COEFFICIENT_VALUE:
  {
    g_rec_mutex_lock(solver_term_mutex);

    g_value_set_boxed(value, &(solver_term->coefficient_value));

    g_rec_mutex_unlock(solver_term_mutex);
  }
  break;
  case PROP_EXPONENT_VALUE:
  {
    g_rec_mutex_lock(solver_term_mutex);

    g_value_set_boxed(value, &(solver_term->exponent_value));

    g_rec_mutex_unlock(solver_term_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_solver_term_finalize(GObject *gobject)
{
  AgsSolverTerm *solver_term;

  solver_term = AGS_SOLVER_TERM(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_solver_term_parent_class)->finalize(gobject);
}

void
ags_solver_term_parse(AgsSolverTerm *solver_term,
		      gchar *term)
{
  if(!AGS_IS_SOLVER_TERM(solver_term)){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_solver_term_new:
 *
 * Instantiate a new #AgsSolverTerm.
 *
 * Returns: the new instance
 *
 * Since: 3.0.0
 */
AgsSolverTerm*
ags_solver_term_new()
{
  AgsSolverTerm *solver_term;
  
  solver_term = g_object_new(AGS_TYPE_SOLVER_TERM,
			     NULL);

  return(solver_term);
}
