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

#ifndef __AGS_SOLVER_POLYNOM_H__
#define __AGS_SOLVER_POLYNOM_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_SOLVER_POLYNOM                (ags_solver_polynom_get_type())
#define AGS_SOLVER_POLYNOM(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SOLVER_POLYNOM, AgsSolverPolynom))
#define AGS_SOLVER_POLYNOM_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SOLVER_POLYNOM, AgsSolverPolynomClass))
#define AGS_IS_SOLVER_POLYNOM(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SOLVER_POLYNOM))
#define AGS_IS_SOLVER_POLYNOM_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SOLVER_POLYNOM))
#define AGS_SOLVER_POLYNOM_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_SOLVER_POLYNOM, AgsSolverPolynomClass))

#define AGS_SOLVER_POLYNOM_GET_OBJ_MUTEX(obj) (&(((AgsSolverPolynom *) obj)->obj_mutex))

typedef struct _AgsSolverPolynom AgsSolverPolynom;
typedef struct _AgsSolverPolynomClass AgsSolverPolynomClass;

#define AGS_SOLVER_POLYNOM_ERROR (ags_solver_polynom_error_quark())

typedef enum{
  AGS_SOLVER_POLYNOM_ERROR_SYMBOL_MISMATCH,
  AGS_SOLVER_POLYNOM_ERROR_EXPONENT_MISMATCH,
  AGS_SOLVER_POLYNOM_ERROR_DIVISION_BY_ZERO,
}AgsSolverPolynomError;

struct _AgsSolverPolynom
{
  GObject gobject;
  
  guint flags;
  
  GRecMutex obj_mutex;

  gchar *polynom;

  gchar *coefficient;
  gchar **symbol;
  gchar **exponent;

  AgsComplex coefficient_value;
  AgsComplex *exponent_value;
};

struct _AgsSolverPolynomClass
{
  GObjectClass gobject;
};

GType ags_solver_polynom_get_type(void);

GQuark ags_solver_polynom_error_quark();

void ags_solver_polynom_update(AgsSolverPolynom *solver_polynom);
void ags_solver_polynom_parse(AgsSolverPolynom *solver_polynom,
			      gchar *polynom);

AgsSolverPolynom* ags_solver_polynom_add(AgsSolverPolynom *polynom_a,
					 AgsSolverPolynom *polynom_b,
					 GError **error);
AgsSolverPolynom* ags_solver_polynom_subtract(AgsSolverPolynom *polynom_a,
					      AgsSolverPolynom *polynom_b,
					      GError **error);

AgsSolverPolynom* ags_solver_polynom_multiply(AgsSolverPolynom *polynom_a,
					      AgsSolverPolynom *polynom_b,
					      GError **error);
AgsSolverPolynom* ags_solver_polynom_divide(AgsSolverPolynom *polynom_a,
					    AgsSolverPolynom *polynom_b,
					    GError **error);

AgsSolverPolynom* ags_solver_polynom_raise_power(AgsSolverPolynom *polynom_a,
						 AgsSolverPolynom *polynom_b,
						 GError **error);
AgsSolverPolynom* ags_solver_polynom_extract_root(AgsSolverPolynom *polynom_a,
						  AgsSolverPolynom *polynom_b,
						  GError **error);

AgsSolverPolynom* ags_solver_polynom_new();

G_END_DECLS

#endif /*__AGS_SOLVER_POLYNOM_H__*/
