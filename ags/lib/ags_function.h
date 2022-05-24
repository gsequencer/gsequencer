/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_FUNCTION_H__
#define __AGS_FUNCTION_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_complex.h>
#include <ags/lib/ags_conversion.h>
#include <ags/lib/ags_solver_matrix.h>
#include <ags/lib/ags_math_util.h>

G_BEGIN_DECLS

#define AGS_TYPE_FUNCTION                (ags_function_get_type())
#define AGS_FUNCTION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FUNCTION, AgsFunction))
#define AGS_FUNCTION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FUNCTION, AgsFunctionClass))
#define AGS_IS_FUNCTION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FUNCTION))
#define AGS_IS_FUNCTION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FUNCTION))
#define AGS_FUNCTION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_FUNCTION, AgsFunctionClass))

#define AGS_FUNCTION_GET_OBJ_MUTEX(obj) (&(((AgsFunction *) obj)->obj_mutex))

typedef struct _AgsFunction AgsFunction;
typedef struct _AgsFunctionClass AgsFunctionClass;

struct _AgsFunction
{
  GObject gobject;

  guint flags;
  
  GRecMutex obj_mutex;

  gboolean is_pushing;

  gchar **equation;
  
  gchar **transformed_equation;

  gchar *source_function;
  gchar *normalized_function;
  
  gchar **symbol;

  GList *solver_matrix;

  guint pivot_table_count;
  guint *row_count;
  guint *column_count;
  AgsComplex*** pivot_table;
  
  guint solver_level;
  AgsComplex **solver_vector;
};

struct _AgsFunctionClass
{
  GObjectClass gobject;
};

GType ags_function_get_type(void);

void ags_function_add_matrix(AgsFunction *function,
			     AgsSolverMatrix *solver_matrix);
void ags_function_remove_matrix(AgsFunction *function,
				AgsSolverMatrix *solver_matrix);

AgsFunction* ags_function_new(gchar *source_function);

G_END_DECLS

#endif /*__AGS_FUNCTION_H__*/
