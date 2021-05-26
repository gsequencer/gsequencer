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

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_solver_polynomial_test_init_suite();
int ags_solver_polynomial_test_clean_suite();

void ags_solver_polynomial_test_set_property();
void ags_solver_polynomial_test_get_property();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_solver_polynomial_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_solver_polynomial_test_clean_suite()
{
  return(0);
}

void
ags_solver_polynomial_test_set_property()
{
  AgsSolverPolynomial *solver_polynomial;

  AgsComplex *coefficient_value;
  
  gchar *polynomial;
  gchar *coefficient;

  double _Complex z;
  
  solver_polynomial = ags_solver_polynomial_new();
  
  polynomial = g_strdup_printf("-2%sa^(%s)",
			       AGS_SYMBOLIC_PI,
			       AGS_SYMBOLIC_EULER);

  g_object_set(solver_polynomial,
	       "polynomial", polynomial,
	       NULL);

  CU_ASSERT(!g_ascii_strcasecmp(solver_polynomial->polynomial,
				polynomial));

  coefficient = g_strdup_printf("-2%s",
				AGS_SYMBOLIC_PI);
  
  g_object_set(solver_polynomial,
	       "coefficient", coefficient,
	       NULL);

  CU_ASSERT(!g_ascii_strcasecmp(solver_polynomial->coefficient,
				coefficient));

  g_object_set(solver_polynomial,
	       "symbol", "a",
	       NULL);
  
  CU_ASSERT(g_strv_contains(solver_polynomial->symbol, "a"));

  coefficient_value = g_new(AgsComplex,
			    1);

  z = 1.0 + I * 0.25;
  
  ags_complex_set(coefficient_value,
		  z);

  g_object_set(solver_polynomial,
	       "coefficient-value", coefficient_value,
	       NULL);

  CU_ASSERT(ags_complex_get(solver_polynomial->coefficient_value) == z);
}

void
ags_solver_polynomial_test_get_property()
{
  //TODO:JK: implement me
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C");
  putenv("LANG=C");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsSolverPolynomialTest", ags_solver_polynomial_test_init_suite, ags_solver_polynomial_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsSolverPolynomial set property", ags_solver_polynomial_test_set_property) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial get property", ags_solver_polynomial_test_get_property) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
