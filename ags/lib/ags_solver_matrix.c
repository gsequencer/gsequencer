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

#include <ags/lib/ags_solver_matrix.h>

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
}

void
ags_solver_matrix_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsSolverMatrix *solver_matrix;

  solver_matrix = AGS_SOLVER_MATRIX(gobject);

  switch(prop_id){
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

  solver_matrix = AGS_SOLVER_MATRIX(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_solver_matrix_finalize(GObject *gobject)
{
  AgsSolverMatrix *solver_matrix;

  solver_matrix = AGS_SOLVER_MATRIX(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_solver_matrix_parent_class)->finalize(gobject);
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
    for(i = 0, j = 0; i < solver_matrix->column_count + 1; i++){
      if(i > position){
	solver_matrix->term_table[i] = solver_matrix->term_table[j];
      }

      if(i != position){
	j++;
      }
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
