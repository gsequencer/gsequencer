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

#ifndef __AGS_SOLVER_VECTOR_H__
#define __AGS_SOLVER_VECTOR_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_complex.h>
#include <ags/lib/ags_solver_polynomial.h>

G_BEGIN_DECLS

#define AGS_TYPE_SOLVER_VECTOR                (ags_solver_vector_get_type())
#define AGS_SOLVER_VECTOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SOLVER_VECTOR, AgsSolverVector))
#define AGS_SOLVER_VECTOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SOLVER_VECTOR, AgsSolverVectorClass))
#define AGS_IS_SOLVER_VECTOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SOLVER_VECTOR))
#define AGS_IS_SOLVER_VECTOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SOLVER_VECTOR))
#define AGS_SOLVER_VECTOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_SOLVER_VECTOR, AgsSolverVectorClass))

#define AGS_SOLVER_VECTOR_GET_OBJ_MUTEX(obj) (&(((AgsSolverVector *) obj)->obj_mutex))

typedef struct _AgsSolverVector AgsSolverVector;
typedef struct _AgsSolverVectorClass AgsSolverVectorClass;

struct _AgsSolverVector
{
  GObject gobject;
  
  guint flags;
  
  GRecMutex obj_mutex;

  gchar **polynomial_history;

  gchar *source_polynomial;

  AgsSolverPolynomial **polynomial_column;
  guint polynomial_count;
};

struct _AgsSolverVectorClass
{
  GObjectClass gobject;
};

GType ags_solver_vector_get_type(void);

gchar* ags_solver_vector_get_source_polynomial(AgsSolverVector *solver_vector);
void ags_solver_vector_set_source_polynomial(AgsSolverVector *solver_vector,
					     gchar *source_polynomial);

guint ags_solver_vector_get_polynomial_count(AgsSolverVector *solver_vector);

void ags_solver_vector_insert_polynomial(AgsSolverVector *solver_vector,
					 AgsSolverPolynomial *solver_polynomial,
					 gint position);
void ags_solver_vector_remove_polynomial(AgsSolverVector *solver_vector,
					 AgsSolverPolynomial *solver_polynomial);

AgsSolverVector* ags_solver_vector_new();

G_END_DECLS

#endif /*__AGS_SOLVER_VECTOR_H__*/
