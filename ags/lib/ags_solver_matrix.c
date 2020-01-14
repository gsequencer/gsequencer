/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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
  //TODO:JK: implement me
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
