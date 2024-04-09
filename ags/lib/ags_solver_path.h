/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_SOLVER_PATH__
#define __AGS_SOLVER_PATH__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AGS_TYPE_SOLVER_PATH                (ags_solver_path_get_type())
#define AGS_SOLVER_PATH(ptr) ((AgsSolverPath *)(ptr))

typedef struct _AgsSolverPath AgsSolverPath;

struct _AgsSolverPath
{
  guint nth_column;
  guint nth_row_a;
  guint nth_row_b;
};

GType ags_solver_path_get_type(void);

AgsSolverPath* ags_solver_path_alloc(guint nth_column,
				     guint nth_row_a,
				     guint nth_row_b);

gpointer ags_solver_path_copy(AgsSolverPath *ptr);
void ags_solver_path_free(AgsSolverPath *ptr);

void ags_solver_path_set_nth_column(AgsSolverPath *solver_path,
				    guint nth_column);
guint ags_solver_path_get_nth_column(AgsSolverPath *solver_path);

void ags_solver_path_set_nth_row_a(AgsSolverPath *solver_path,
				   guint nth_row_a);
guint ags_solver_path_get_nth_row_a(AgsSolverPath *solver_path);

void ags_solver_path_set_nth_row_b(AgsSolverPath *solver_path,
				   guint nth_row_b);
guint ags_solver_path_get_nth_row_b(AgsSolverPath *solver_path);

G_END_DECLS

#endif /*__AGS_SOLVER_PATH__*/
