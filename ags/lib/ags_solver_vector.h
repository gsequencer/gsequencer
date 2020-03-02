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

#ifndef __AGS_SOLVER_VECTOR_H__
#define __AGS_SOLVER_VECTOR_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_complex.h>
#include <ags/lib/ags_solver_polynom.h>

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

  gchar **polynom_history;

  gchar *source_polynom;

  AgsSolverPolynom **polynom_column;
  guint polynom_count;
};

struct _AgsSolverVectorClass
{
  GObjectClass gobject;
};

GType ags_solver_vector_get_type(void);

void ags_solver_vector_insert_polynom(AgsSolverVector *solver_vector,
				      AgsSolverPolynom *solver_polynom,
				      gint position);
void ags_solver_vector_remove_polynom(AgsSolverVector *solver_vector,
				   AgsSolverPolynom *solver_polynom);

AgsSolverVector* ags_solver_vector_new();

G_END_DECLS

#endif /*__AGS_SOLVER_VECTOR_H__*/
