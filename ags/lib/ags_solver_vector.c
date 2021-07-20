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

#include <ags/lib/ags_solver_vector.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_solver_vector_class_init(AgsSolverVectorClass *solver_vector);
void ags_solver_vector_init (AgsSolverVector *solver_vector);
void ags_solver_vector_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_solver_vector_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_solver_vector_finalize(GObject *gobject);

/**
 * SECTION:ags_solver_vector
 * @short_description: solver vector
 * @title: AgsSolverVector
 * @section_id:
 * @include: ags/lib/ags_solver_vector.h
 *
 * The #AgsSolverVector contains the solver vectors.
 */

enum{
  PROP_0,
  PROP_SOURCE_POLYNOMIAL,
  PROP_POLYNOMIAL_COUNT,
};

static gpointer ags_solver_vector_parent_class = NULL;

GType
ags_solver_vector_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_solver_vector = 0;

    static const GTypeInfo ags_solver_vector_info = {
      sizeof (AgsSolverVectorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_solver_vector_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSolverVector),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_solver_vector_init,
    };

    ags_type_solver_vector = g_type_register_static(G_TYPE_OBJECT,
						    "AgsSolverVector",
						    &ags_solver_vector_info,
						    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_solver_vector);
  }

  return g_define_type_id__volatile;
}

void
ags_solver_vector_class_init(AgsSolverVectorClass *solver_vector)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_solver_vector_parent_class = g_type_class_peek_parent(solver_vector);

  /* GObjectClass */
  gobject = (GObjectClass *) solver_vector;

  gobject->set_property = ags_solver_vector_set_property;
  gobject->get_property = ags_solver_vector_get_property;
  
  gobject->finalize = ags_solver_vector_finalize;

  /* properties */
  /**
   * AgsSolverVector:source-polynomial:
   *
   * The assigned source-polynomial.
   * 
   * Since: 3.9.3
   */
  param_spec = g_param_spec_string("source-polynomial",
				   i18n_pspec("source polynomial of solver vector"),
				   i18n_pspec("The source polynomial this solver vector is derived from"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOURCE_POLYNOMIAL,
				  param_spec);

  /**
   * AgsSolverVector:polynomial-count:
   *
   * The polynomial count of the term table.
   * 
   * Since: 3.9.3
   */
  param_spec = g_param_spec_uint("polynomial-count",
				 i18n_pspec("polynomial count of solver vector"),
				 i18n_pspec("The polynomial count this solver vector has in polynomial column"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_POLYNOMIAL_COUNT,
				  param_spec);
}

void
ags_solver_vector_init(AgsSolverVector *solver_vector)
{
  solver_vector->flags = 0;
  
  g_rec_mutex_init(&(solver_vector->obj_mutex));

  solver_vector->polynomial_history = NULL;
  
  solver_vector->source_polynomial = NULL;
  
  solver_vector->polynomial_column = NULL;
  solver_vector->polynomial_count = 0;
}

void
ags_solver_vector_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsSolverVector *solver_vector;

  GRecMutex *solver_vector_mutex;

  solver_vector = AGS_SOLVER_VECTOR(gobject);

  /* solver vector mutex */
  solver_vector_mutex = AGS_SOLVER_VECTOR_GET_OBJ_MUTEX(solver_vector);

  switch(prop_id){
  case PROP_SOURCE_POLYNOMIAL:
  {
    gchar *source_polynomial;

    source_polynomial = (gchar *) g_value_get_string(value);

    g_rec_mutex_lock(solver_vector_mutex);

    if(solver_vector->source_polynomial == source_polynomial){
      g_rec_mutex_unlock(solver_vector_mutex);

      return;
    }
      
    if(solver_vector->source_polynomial != NULL){
      g_free(solver_vector->source_polynomial);
    }

    solver_vector->source_polynomial = g_strdup(source_polynomial);

    g_rec_mutex_unlock(solver_vector_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_solver_vector_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsSolverVector *solver_vector;

  GRecMutex *solver_vector_mutex;

  solver_vector = AGS_SOLVER_VECTOR(gobject);

  /* solver vector mutex */
  solver_vector_mutex = AGS_SOLVER_VECTOR_GET_OBJ_MUTEX(solver_vector);

  switch(prop_id){
  case PROP_SOURCE_POLYNOMIAL:
  {
    g_rec_mutex_lock(solver_vector_mutex);

    g_value_set_string(value, solver_vector->source_polynomial);

    g_rec_mutex_unlock(solver_vector_mutex);
  }
  break;
  case PROP_POLYNOMIAL_COUNT:
  {
    g_rec_mutex_lock(solver_vector_mutex);

    g_value_set_uint(value, solver_vector->polynomial_count);

    g_rec_mutex_unlock(solver_vector_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_solver_vector_finalize(GObject *gobject)
{
  AgsSolverVector *solver_vector;

  solver_vector = AGS_SOLVER_VECTOR(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_solver_vector_parent_class)->finalize(gobject);
}

/**
 * ags_solver_vector_get_source_polynomial:
 * @solver_vector: the #AgsSolverVector
 * 
 * Get source polynomial of @solver_vector.
 * 
 * Returns: the source polynomial
 * 
 * Since: 3.9.3
 */
gchar*
ags_solver_vector_get_source_polynomial(AgsSolverVector *solver_vector)
{
  gchar *source_polynomial;
  
  if(!AGS_IS_SOLVER_VECTOR(solver_vector)){
    return(NULL);
  }

  source_polynomial = NULL;
  
  g_object_get(solver_vector,
	       "source-polynomial", &source_polynomial,
	       NULL);

  return(source_polynomial);
}

/**
 * ags_solver_vector_set_source_polynomial:
 * @solver_vector: the #AgsSolverVector
 * @source_polynomial: the source polynomial
 * 
 * Set @source_polynomial of @solver_vector.
 * 
 * Since: 3.9.3
 */
void
ags_solver_vector_set_source_polynomial(AgsSolverVector *solver_vector,
				      gchar *source_polynomial)
{
  if(!AGS_IS_SOLVER_VECTOR(solver_vector)){
    return;
  }

  g_object_set(solver_vector,
	       "source-polynomial", source_polynomial,
	       NULL);
}

/**
 * ags_solver_vector_get_polynomial_count:
 * @solver_vector: the #AgsSolverVector
 * 
 * Get polynomial count of @solver_vector.
 * 
 * Returns: the polynomial count
 * 
 * Since: 3.9.3
 */
guint
ags_solver_vector_get_polynomial_count(AgsSolverVector *solver_vector)
{
  guint polynomial_count;
  
  if(!AGS_IS_SOLVER_VECTOR(solver_vector)){
    return(0);
  }

  polynomial_count = 0;

  g_object_get(solver_vector,
	       "polynomial-count", &polynomial_count,
	       NULL);

  return(polynomial_count);
}

/**
 * ags_solver_vector_insert_polynomial:
 * @solver_vector: the #AgsSolverVector
 * @solver_polynomial: the #AgsSolverPolynomial
 * @position: the position
 * 
 * Insert @solver_polynomial to @solver_vector.
 * 
 * Since: 3.2.0
 */
void
ags_solver_vector_insert_polynomial(AgsSolverVector *solver_vector,
				    AgsSolverPolynomial *solver_polynomial,
				    gint position)
{
  guint i, j;

  GRecMutex *solver_vector_mutex;
  
  if(!AGS_IS_SOLVER_VECTOR(solver_vector) ||
     !AGS_IS_SOLVER_POLYNOMIAL(solver_polynomial)){
    return;
  }

  solver_vector_mutex = AGS_SOLVER_VECTOR_GET_OBJ_MUTEX(solver_vector);

  g_rec_mutex_lock(solver_vector_mutex);
  
  if(position > solver_vector->polynomial_count){
    position = solver_vector->polynomial_count;
  }
  
  if(solver_vector->polynomial_column == NULL){
    solver_vector->polynomial_column = (AgsSolverPolynomial **) g_malloc(sizeof(AgsSolverPolynomial *));
  }else{
    solver_vector->polynomial_column = (AgsSolverPolynomial **) g_realloc(solver_vector->polynomial_column,
									  (solver_vector->polynomial_count + 1) * sizeof(AgsSolverPolynomial *));
  }

  g_object_ref(solver_polynomial);
  
  if(position < 0){
    solver_vector->polynomial_column[solver_vector->polynomial_count] = solver_polynomial;
  }else{
    for(i = 0, j = 0; i < solver_vector->polynomial_count + 1; i++){
      if(i > position){
	solver_vector->polynomial_column[i] = solver_vector->polynomial_column[j];
      }

      if(i != position){
	j++;
      }
    }

    solver_vector->polynomial_column[position] = solver_polynomial;
  }

  solver_vector->polynomial_count += 1;
  
  g_rec_mutex_unlock(solver_vector_mutex);  
}

/**
 * ags_solver_vector_remove_polynomial:
 * @solver_vector: the #AgsSolverVector
 * @solver_polynomial: the #AgsSolverPolynomial
 * 
 * Remove @solver_polynomial from @solver_vector.
 * 
 * Since: 3.2.0
 */
void
ags_solver_vector_remove_polynomial(AgsSolverVector *solver_vector,
				    AgsSolverPolynomial *solver_polynomial)
{
  gint position;
  guint i, j;
  
  GRecMutex *solver_vector_mutex;
  
  if(!AGS_IS_SOLVER_VECTOR(solver_vector) ||
     !AGS_IS_SOLVER_POLYNOMIAL(solver_polynomial)){
    return;
  }

  solver_vector_mutex = AGS_SOLVER_VECTOR_GET_OBJ_MUTEX(solver_vector);

  g_rec_mutex_lock(solver_vector_mutex);

  position = -1;
  
  for(i = 0; i < solver_vector->polynomial_count; i++){
    if(solver_vector->polynomial_column[i] == solver_polynomial){
      position = i;

      break;
    }
  }

  if(position >= 0){
    if(solver_vector->polynomial_count == 1){
      g_free(solver_vector->polynomial_column);

      solver_vector->polynomial_column = NULL;
    }else{
      for(i = 0, j = 0; i < solver_vector->polynomial_count; i++){
	if(i > position){
	  solver_vector->polynomial_column[j] = solver_vector->polynomial_column[i];
	}

	if(i != position){
	  j++;
	}
      }
      
      solver_vector->polynomial_column = (AgsSolverPolynomial **) g_realloc(solver_vector->polynomial_column,
									    (solver_vector->polynomial_count - 1) * sizeof(AgsSolverPolynomial *));
    }

    g_object_unref(solver_polynomial);
  }

  solver_vector->polynomial_count -= 1;
  
  g_rec_mutex_unlock(solver_vector_mutex);  
}

/**
 * ags_solver_vector_new:
 *
 * Instantiate a new #AgsSolverVector.
 *
 * Returns: the new instance
 *
 * Since: 3.0.0
 */
AgsSolverVector*
ags_solver_vector_new()
{
  AgsSolverVector *solver_vector;
  
  solver_vector = g_object_new(AGS_TYPE_SOLVER_VECTOR,
			       NULL);

  return(solver_vector);
}
