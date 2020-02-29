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

#ifndef __AGS_SOLVER_TERM_H__
#define __AGS_SOLVER_TERM_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_SOLVER_TERM                (ags_solver_term_get_type())
#define AGS_SOLVER_TERM(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SOLVER_TERM, AgsSolverTerm))
#define AGS_SOLVER_TERM_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SOLVER_TERM, AgsSolverTermClass))
#define AGS_IS_SOLVER_TERM(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SOLVER_TERM))
#define AGS_IS_SOLVER_TERM_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SOLVER_TERM))
#define AGS_SOLVER_TERM_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_SOLVER_TERM, AgsSolverTermClass))

#define AGS_SOLVER_TERM_GET_OBJ_MUTEX(obj) (&(((AgsSolverTerm *) obj)->obj_mutex))

typedef struct _AgsSolverTerm AgsSolverTerm;
typedef struct _AgsSolverTermClass AgsSolverTermClass;

#define AGS_SOLVER_TERM_ERROR (ags_solver_term_error_quark())

typedef enum{
  AGS_SOLVER_TERM_ERROR_SYMBOL_MISMATCH,
  AGS_SOLVER_TERM_ERROR_EXPONENT_MISMATCH,
  AGS_SOLVER_TERM_ERROR_DIVISION_BY_ZERO,
}AgsSolverTermError;

struct _AgsSolverTerm
{
  GObject gobject;
  
  guint flags;
  
  GRecMutex obj_mutex;

  gchar *term;

  gchar *coefficient;
  gchar **symbol;
  gchar **exponent;

  AgsComplex coefficient_value;
  AgsComplex *exponent_value;
  AgsComplex summand_value;
};

struct _AgsSolverTermClass
{
  GObjectClass gobject;
};

GType ags_solver_term_get_type(void);

GQuark ags_solver_term_error_quark();

void ags_solver_term_update(AgsSolverTerm *solver_term);
void ags_solver_term_parse(AgsSolverTerm *solver_term,
			   gchar *term);

AgsSolverTerm* ags_solver_term_add(AgsSolverTerm *term_a,
				   AgsSolverTerm *term_b,
				   GError **error);
AgsSolverTerm* ags_solver_term_subtract(AgsSolverTerm *term_a,
					AgsSolverTerm *term_b,
					GError **error);

AgsSolverTerm* ags_solver_term_multiply(AgsSolverTerm *term_a,
					AgsSolverTerm *term_b,
					GError **error);
AgsSolverTerm* ags_solver_term_divide(AgsSolverTerm *term_a,
				      AgsSolverTerm *term_b,
				      GError **error);

AgsSolverTerm* ags_solver_term_raise_power(AgsSolverTerm *term_a,
					   AgsSolverTerm *term_b,
					   GError **error);
AgsSolverTerm* ags_solver_term_extract_root(AgsSolverTerm *term_a,
					    AgsSolverTerm *term_b,
					    GError **error);

AgsSolverTerm* ags_solver_term_new();

G_END_DECLS

#endif /*__AGS_SOLVER_TERM_H__*/
