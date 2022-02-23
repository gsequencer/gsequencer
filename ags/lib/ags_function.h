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

/**
 * AgsFunctionFlags:
 * @AGS_FUNCTION_LINEAR: the function is linear
 * @AGS_FUNCTION_EXPONENTIAL: the function is exponential
 * @AGS_FUNCTION_LOGARITHMIC: the function is logarithmic
 * @AGS_FUNCTION_IS_UNIQUE: the function is unique
 * @AGS_FUNCTION_SOLVE_PIVOT_TABLE: do solve using pivot table
 * @AGS_FUNCTION_SOLVE_MAXIMUM_COLON: solve using maximum colon strategy
 * @AGS_FUNCTION_SOLVE_GAUSS: solve using gauss strategy
 * 
 * Enum values to control the behavior or indicate internal state of #AgsFunction by
 * enable/disable as flags.
 */
typedef enum{
  AGS_FUNCTION_LINEAR               = 1,
  AGS_FUNCTION_EXPONENTIAL          = 1 <<  1,
  AGS_FUNCTION_LOGARITHMIC          = 1 <<  2,
  AGS_FUNCTION_IS_UNIQUE            = 1 <<  3,
  AGS_FUNCTION_SOLVE_PIVOT_TABLE    = 1 <<  4,
  AGS_FUNCTION_SOLVE_MAXIMUM_COLON  = 1 <<  5,
  AGS_FUNCTION_SOLVE_GAUSS          = 1 <<  6,
}AgsFunctionFlags;

struct _AgsFunction
{
  AgsConversion conversion;

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
  AgsConversionClass conversion;

  void (*literal_solve)(AgsFunction *function);
};

GType ags_function_get_type(void);

gchar** ags_function_collapse_parenthesis(AgsFunction *function,
					  guint *function_count);

gchar** ags_function_find_literals(AgsFunction *function,
				   guint *symbol_count);
void ags_function_literal_solve(AgsFunction *function);

gboolean ags_function_push_equation(AgsFunction *function,
				    gchar *equation);
void ags_function_pop_equation(AgsFunction *function,
			       GError **error);

gchar* ags_function_get_expanded(AgsFunction *function,
				 gchar **symbol,
				 guint symbol_count);
gchar* ags_function_get_normalized(AgsFunction *function);

AgsComplex* ags_function_compute_term(gchar *term,
				      gchar *substitute_symbol, AgsComplex *substitute_value);

AgsComplex** ags_function_symbolic_translate_value(AgsFunction *function,
						   gchar *symbol,
						   AgsComplex *value);

gboolean ags_function_substitute_values(AgsFunction *function,
					gchar *symbol, ...);
AgsComplex* ags_function_translate_value(AgsFunction *function,
					 AgsComplex *value);

void ags_function_add_matrix(AgsFunction *function,
			     AgsSolverMatrix *solver_matrix);
void ags_function_remove_matrix(AgsFunction *function,
				AgsSolverMatrix *solver_matrix);

AgsFunction* ags_function_new(gchar *source_function);

G_END_DECLS

#endif /*__AGS_FUNCTION_H__*/
