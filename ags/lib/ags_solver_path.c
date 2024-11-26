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

#include <ags/lib/ags_solver_path.h>

#include <stdlib.h>

/**
 * SECTION:ags_solver_path
 * @short_description: boxed type of solver_path
 * @title: AgsSolverPath
 * @section_id:
 * @include: ags/lib/ags_solver_path.h
 *
 * Boxed type of solver_path data type.
 */

GType
ags_solver_path_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_solver_path = 0;

    ags_type_solver_path =
      g_boxed_type_register_static("AgsSolverPath",
				   (GBoxedCopyFunc) ags_solver_path_copy,
				   (GBoxedFreeFunc) ags_solver_path_free);

    g_once_init_leave(&g_define_type_id__static, ags_type_solver_path);
  }

  return(g_define_type_id__static);
}

/**
 * ags_solver_path_alloc:
 * @nth_column: the nth-column
 * @nth_row_a: nth-row a
 * @nth_row_b: nth-row b
 *
 * Allocate #AgsSolverPath-struct
 *
 * Returns: a new #AgsSolverPath-struct
 *
 * Since: 6.7.1
 */
AgsSolverPath*
ags_solver_path_alloc(guint nth_column,
		      guint nth_row_a,
		      guint nth_row_b)
{
  AgsSolverPath *ptr;

  ptr = (AgsSolverPath *) g_malloc(sizeof(AgsSolverPath));
  
  ptr[0].nth_column = nth_column;
  ptr[0].nth_row_a = nth_row_a;
  ptr[0].nth_row_b = nth_row_b;

  return(ptr);
}

/**
 * ags_solver_path_copy:
 * @ptr: the original #AgsSolverPath-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsSolverPath-struct
 *
 * Since: 6.7.1
 */
gpointer
ags_solver_path_copy(AgsSolverPath *ptr)
{
  AgsSolverPath *new_ptr;

  if(ptr == NULL){
    return(NULL);
  }
  
  new_ptr = (AgsSolverPath *) g_malloc(sizeof(AgsSolverPath));
  
  new_ptr->nth_column = ptr->nth_column;
  new_ptr->nth_row_a = ptr->nth_row_a;
  new_ptr->nth_row_b = ptr->nth_row_b;

  return(new_ptr);
}

/**
 * ags_solver_path_free:
 * @ptr: the #AgsSolverPath-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 6.7.1
 */
void
ags_solver_path_free(AgsSolverPath *ptr)
{
  g_free(ptr);
}

/**
 * ags_solver_path_get_nth_column:
 * @solver_path: the #AgsSolverPath
 * 
 * Get nth-column of @solver_path.
 *
 * Returns: nth-column as unsigned integer
 *
 * Since: 6.7.1
 */
guint
ags_solver_path_get_nth_column(AgsSolverPath *solver_path)
{
  g_return_val_if_fail(solver_path != NULL, 0);

  return(solver_path->nth_column);
}

/**
 * ags_solver_path_set_nth_column:
 * @solver_path: the #AgsSolverPath
 * @nth_column: the nth-column to set
 * 
 * Set nth-column of @solver_path.
 *
 * Since: 6.7.1
 */
void
ags_solver_path_set_nth_column(AgsSolverPath *solver_path, guint nth_column)
{
  g_return_if_fail(solver_path != NULL);

  solver_path->nth_column = nth_column;
}

/**
 * ags_solver_path_get_nth_row_a:
 * @solver_path: the #AgsSolverPath
 * 
 * Get nth-row a of @solver_path.
 *
 * Returns: nth-row a as unsigned integer
 *
 * Since: 6.7.1
 */
guint
ags_solver_path_get_nth_row_a(AgsSolverPath *solver_path)
{
  g_return_val_if_fail(solver_path != NULL, 0);

  return(solver_path->nth_row_a);
}

/**
 * ags_solver_path_set_nth_row_a:
 * @solver_path: the #AgsSolverPath
 * @nth_row_a: the nth-row a to set
 * 
 * Set nth-row a of @solver_path.
 *
 * Since: 6.7.1
 */
void
ags_solver_path_set_nth_row_a(AgsSolverPath *solver_path, guint nth_row_a)
{
  g_return_if_fail(solver_path != NULL);

  solver_path->nth_row_a = nth_row_a;
}

/**
 * ags_solver_path_get_nth_row_b:
 * @solver_path: the #AgsSolverPath
 * 
 * Get nth-row b of solver path.
 *
 * Returns: nth-row b as unsigned integer
 *
 * Since: 6.7.1
 */
guint
ags_solver_path_get_nth_row_b(AgsSolverPath *solver_path)
{
  g_return_val_if_fail(solver_path != NULL, 0);

  return(solver_path->nth_row_b);
}

/**
 * ags_solver_path_set_nth_row_b:
 * @solver_path: the #AgsSolverPath
 * @nth_row_b: the nth-row b to set
 * 
 * Set nth-row b of @solver_path.
 *
 * Since: 6.7.1
 */
void
ags_solver_path_set_nth_row_b(AgsSolverPath *solver_path, guint nth_row_b)
{
  g_return_if_fail(solver_path != NULL);

  solver_path->nth_row_b = nth_row_b;
}
