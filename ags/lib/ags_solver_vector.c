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
}

void
ags_solver_vector_init(AgsSolverVector *solver_vector)
{
  solver_vector->flags = 0;
  
  g_rec_mutex_init(&(solver_vector->obj_mutex));

  solver_vector->polynom_history = NULL;
  
  solver_vector->source_polynom = NULL;
  
  solver_vector->polynom_column = NULL;
  solver_vector->polynom_count = 0;
}

void
ags_solver_vector_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsSolverVector *solver_vector;

  solver_vector = AGS_SOLVER_VECTOR(gobject);

  switch(prop_id){
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

  solver_vector = AGS_SOLVER_VECTOR(gobject);

  switch(prop_id){
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
 * ags_solver_vector_insert_polynom:
 * @solver_vector: the #AgsSolverVector
 * @solver_polynom: the #AgsSolverPolynom
 * @position: the position
 * 
 * Insert @solver_polynom to @solver_vector.
 * 
 * Since: 3.2.0
 */
void
ags_solver_vector_insert_polynom(AgsSolverVector *solver_vector,
				 AgsSolverPolynom *solver_polynom,
				 gint position)
{
  guint i, j;

  GRecMutex *solver_vector_mutex;
  
  if(!AGS_IS_SOLVER_VECTOR(solver_vector) ||
     !AGS_IS_SOLVER_POLYNOM(solver_polynom)){
    return;
  }

  solver_vector_mutex = AGS_SOLVER_VECTOR_GET_OBJ_MUTEX(solver_vector);

  g_rec_mutex_lock(solver_vector_mutex);
  
  if(position > solver_vector->polynom_count){
    position = solver_vector->polynom_count;
  }
  
  if(solver_vector->polynom_column == NULL){
    solver_vector->polynom_column = (AgsSolverPolynom **) g_malloc(sizeof(AgsSolverPolynom *));
  }else{
    solver_vector->polynom_column = (AgsSolverPolynom **) g_realloc(solver_vector->polynom_column,
								    (solver_vector->polynom_count + 1) * sizeof(AgsSolverPolynom *));
  }

  g_object_ref(solver_polynom);
  
  if(position < 0){
    solver_vector->polynom_column[solver_vector->polynom_count] = solver_polynom;
  }else{
    for(i = 0, j = 0; i < solver_vector->polynom_count + 1; i++){
      if(i > position){
	solver_vector->polynom_column[i] = solver_vector->polynom_column[j];
      }

      if(i != position){
	j++;
      }
    }

    solver_vector->polynom_column[position] = solver_polynom;
  }

  solver_vector->polynom_count += 1;
  
  g_rec_mutex_unlock(solver_vector_mutex);  
}

/**
 * ags_solver_vector_remove_polynom:
 * @solver_vector: the #AgsSolverVector
 * @solver_polynom: the #AgsSolverPolynom
 * 
 * Remove @solver_polynom from @solver_vector.
 * 
 * Since: 3.2.0
 */
void
ags_solver_vector_remove_polynom(AgsSolverVector *solver_vector,
				 AgsSolverPolynom *solver_polynom)
{
  gint position;
  guint i, j;
  
  GRecMutex *solver_vector_mutex;
  
  if(!AGS_IS_SOLVER_VECTOR(solver_vector) ||
     !AGS_IS_SOLVER_POLYNOM(solver_polynom)){
    return;
  }

  solver_vector_mutex = AGS_SOLVER_VECTOR_GET_OBJ_MUTEX(solver_vector);

  g_rec_mutex_lock(solver_vector_mutex);

  position = -1;
  
  for(i = 0; i < solver_vector->polynom_count; i++){
    if(solver_vector->polynom_column[i] == solver_polynom){
      position = i;

      break;
    }
  }

  if(position >= 0){
    if(solver_vector->polynom_count == 1){
      g_free(solver_vector->polynom_column);

      solver_vector->polynom_column = NULL;
    }else{
      for(i = 0, j = 0; i < solver_vector->polynom_count; i++){
	if(i > position){
	  solver_vector->polynom_column[j] = solver_vector->polynom_column[i];
	}

	if(i != position){
	  j++;
	}
      }
      
      solver_vector->polynom_column = (AgsSolverPolynom **) g_realloc(solver_vector->polynom_column,
								      (solver_vector->polynom_count - 1) * sizeof(AgsSolverPolynom *));
    }

    g_object_unref(solver_polynom);
  }

  solver_vector->polynom_count -= 1;
  
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
