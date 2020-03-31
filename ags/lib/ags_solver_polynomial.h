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

#ifndef __AGS_SOLVER_POLYNOMIAL_H__
#define __AGS_SOLVER_POLYNOMIAL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_SOLVER_POLYNOMIAL                (ags_solver_polynomial_get_type())
#define AGS_SOLVER_POLYNOMIAL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SOLVER_POLYNOMIAL, AgsSolverPolynomial))
#define AGS_SOLVER_POLYNOMIAL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SOLVER_POLYNOMIAL, AgsSolverPolynomialClass))
#define AGS_IS_SOLVER_POLYNOMIAL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SOLVER_POLYNOMIAL))
#define AGS_IS_SOLVER_POLYNOMIAL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SOLVER_POLYNOMIAL))
#define AGS_SOLVER_POLYNOMIAL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_SOLVER_POLYNOMIAL, AgsSolverPolynomialClass))

#define AGS_SOLVER_POLYNOMIAL_GET_OBJ_MUTEX(obj) (&(((AgsSolverPolynomial *) obj)->obj_mutex))

typedef struct _AgsSolverPolynomial AgsSolverPolynomial;
typedef struct _AgsSolverPolynomialClass AgsSolverPolynomialClass;

#define AGS_SOLVER_POLYNOMIAL_ERROR (ags_solver_polynomial_error_quark())

typedef enum{
  AGS_SOLVER_POLYNOMIAL_ERROR_SYMBOL_MISMATCH,
  AGS_SOLVER_POLYNOMIAL_ERROR_EXPONENT_MISMATCH,
  AGS_SOLVER_POLYNOMIAL_ERROR_DIVISION_BY_ZERO,
  AGS_SOLVER_POLYNOMIAL_ERROR_EXPONENT_NOT_CONSTANT,
}AgsSolverPolynomialError;

struct _AgsSolverPolynomial
{
  GObject gobject;
  
  guint flags;
  
  GRecMutex obj_mutex;

  gchar *polynomial;

  gchar *coefficient;
  gchar **symbol;
  gchar **exponent;

  AgsComplex coefficient_value;
  AgsComplex *exponent_value;
};

struct _AgsSolverPolynomialClass
{
  GObjectClass gobject;
};

GType ags_solver_polynomial_get_type(void);

GQuark ags_solver_polynomial_error_quark();

void ags_solver_polynomial_update(AgsSolverPolynomial *solver_polynomial);
void ags_solver_polynomial_parse(AgsSolverPolynomial *solver_polynomial,
				 gchar *polynomial);

AgsSolverPolynomial* ags_solver_polynomial_add(AgsSolverPolynomial *polynomial_a,
					       AgsSolverPolynomial *polynomial_b,
					       GError **error);
AgsSolverPolynomial* ags_solver_polynomial_subtract(AgsSolverPolynomial *polynomial_a,
						    AgsSolverPolynomial *polynomial_b,
						    GError **error);

AgsSolverPolynomial* ags_solver_polynomial_multiply(AgsSolverPolynomial *polynomial_a,
						    AgsSolverPolynomial *polynomial_b,
						    GError **error);
AgsSolverPolynomial* ags_solver_polynomial_divide(AgsSolverPolynomial *polynomial_a,
						  AgsSolverPolynomial *polynomial_b,
						  GError **error);

AgsSolverPolynomial* ags_solver_polynomial_raise_power(AgsSolverPolynomial *polynomial_a,
						       AgsSolverPolynomial *polynomial_b,
						       GError **error);
AgsSolverPolynomial* ags_solver_polynomial_extract_root(AgsSolverPolynomial *polynomial_a,
							AgsSolverPolynomial *polynomial_b,
							GError **error);

AgsSolverPolynomial* ags_solver_polynomial_new();

G_END_DECLS

#endif /*__AGS_SOLVER_POLYNOMIAL_H__*/
