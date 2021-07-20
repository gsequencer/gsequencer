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

#ifndef __AGS_SOLVER_MATRIX_H__
#define __AGS_SOLVER_MATRIX_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_complex.h>
#include <ags/lib/ags_solver_vector.h>

G_BEGIN_DECLS

#define AGS_TYPE_SOLVER_MATRIX                (ags_solver_matrix_get_type())
#define AGS_SOLVER_MATRIX(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SOLVER_MATRIX, AgsSolverMatrix))
#define AGS_SOLVER_MATRIX_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SOLVER_MATRIX, AgsSolverMatrixClass))
#define AGS_IS_SOLVER_MATRIX(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SOLVER_MATRIX))
#define AGS_IS_SOLVER_MATRIX_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SOLVER_MATRIX))
#define AGS_SOLVER_MATRIX_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_SOLVER_MATRIX, AgsSolverMatrixClass))

#define AGS_SOLVER_MATRIX_GET_OBJ_MUTEX(obj) (&(((AgsSolverMatrix *) obj)->obj_mutex))

typedef struct _AgsSolverMatrix AgsSolverMatrix;
typedef struct _AgsSolverMatrixClass AgsSolverMatrixClass;

struct _AgsSolverMatrix
{
  GObject gobject;
  
  guint flags;
  
  GRecMutex obj_mutex;

  gchar **function_history;

  gchar *source_function;
  
  AgsSolverVector **term_table;
  guint row_count;
  guint column_count;
};

struct _AgsSolverMatrixClass
{
  GObjectClass gobject;
};

GType ags_solver_matrix_get_type(void);

gchar* ags_solver_matrix_get_source_function(AgsSolverMatrix *solver_matrix);
void ags_solver_matrix_set_source_function(AgsSolverMatrix *solver_matrix,
					   gchar *source_function);

guint ags_solver_matrix_get_row_count(AgsSolverMatrix *solver_matrix);

guint ags_solver_matrix_get_column_count(AgsSolverMatrix *solver_matrix);

void ags_solver_matrix_parse(AgsSolverMatrix *solver_matrix,
			     gchar *source_function);

void ags_solver_matrix_insert_vector(AgsSolverMatrix *solver_matrix,
				     AgsSolverVector *solver_vector,
				     gint position);
void ags_solver_matrix_remove_vector(AgsSolverMatrix *solver_matrix,
				     AgsSolverVector *solver_vector);

AgsSolverMatrix* ags_solver_matrix_new();

G_END_DECLS

#endif /*__AGS_SOLVER_MATRIX_H__*/
