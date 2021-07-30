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

int ags_solver_vector_test_init_suite();
int ags_solver_vector_test_clean_suite();

void ags_solver_vector_test_get_source_polynomial();
void ags_solver_vector_test_set_source_polynomial();
void ags_solver_vector_test_get_polynomial_count();
void ags_solver_vector_test_insert_polynomial();
void ags_solver_vector_test_remove_polynomial();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_solver_vector_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_solver_vector_test_clean_suite()
{
  return(0);
}

void
ags_solver_vector_test_get_source_polynomial()
{
  AgsSolverVector *solver_vector;
  
  solver_vector = ags_solver_vector_new();

  ags_solver_vector_set_source_polynomial(solver_vector,
					  "-3.14a^(2)b");

  CU_ASSERT(solver_vector->source_polynomial != NULL &&
	    !g_strcmp0(solver_vector->source_polynomial, "-3.14a^(2)b"));
}

void
ags_solver_vector_test_set_source_polynomial()
{
  AgsSolverVector *solver_vector;

  gchar *source_polynomial;
  
  solver_vector = ags_solver_vector_new();

  solver_vector->source_polynomial = g_strdup("-3.14a^(2)b");

  source_polynomial = ags_solver_vector_get_source_polynomial(solver_vector);

  CU_ASSERT(source_polynomial != NULL &&
	    !g_strcmp0(source_polynomial, "-3.14a^(2)b"));
}

void
ags_solver_vector_test_get_polynomial_count()
{
  AgsSolverVector *solver_vector;

  guint polynomial_count;
  
  solver_vector = ags_solver_vector_new();

  solver_vector->polynomial_count = 2;
  
  polynomial_count = ags_solver_vector_get_polynomial_count(solver_vector);

  CU_ASSERT(polynomial_count == 2);
}

void
ags_solver_vector_test_insert_polynomial()
{
  AgsSolverVector *solver_vector;
  AgsSolverPolynomial *solver_polynomial;
  AgsSolverPolynomial* solver_polynomial_arr_orig[3];
  AgsSolverPolynomial* solver_polynomial_arr_new[3];

  solver_vector = ags_solver_vector_new();

  CU_ASSERT(solver_vector->polynomial_column == NULL);

  /* append */
  solver_polynomial =
    solver_polynomial_arr_orig[0] = ags_solver_polynomial_new();
  ags_solver_vector_insert_polynomial(solver_vector,
				      solver_polynomial,
				      -1);

  CU_ASSERT(solver_vector->polynomial_column != NULL);
  CU_ASSERT(solver_vector->polynomial_column[0] == solver_polynomial);

  solver_polynomial =
    solver_polynomial_arr_orig[1] =  ags_solver_polynomial_new();
  ags_solver_vector_insert_polynomial(solver_vector,
				      solver_polynomial,
				      -1);

  CU_ASSERT(solver_vector->polynomial_column != NULL);
  CU_ASSERT(solver_vector->polynomial_column[1] == solver_polynomial);

  solver_polynomial = 
    solver_polynomial_arr_orig[2] = ags_solver_polynomial_new();
  ags_solver_vector_insert_polynomial(solver_vector,
				      solver_polynomial,
				      -1);

  CU_ASSERT(solver_vector->polynomial_column != NULL);
  CU_ASSERT(solver_vector->polynomial_column[2] == solver_polynomial);

  /* insert */
  solver_polynomial = 
    solver_polynomial_arr_new[0] = ags_solver_polynomial_new();
  ags_solver_vector_insert_polynomial(solver_vector,
				      solver_polynomial,
				      0);

  CU_ASSERT(solver_vector->polynomial_column != NULL);
  CU_ASSERT(solver_vector->polynomial_column[0] == solver_polynomial);

  solver_polynomial = 
    solver_polynomial_arr_new[1] = ags_solver_polynomial_new();
  ags_solver_vector_insert_polynomial(solver_vector,
				      solver_polynomial,
				      2);

  CU_ASSERT(solver_vector->polynomial_column != NULL);
  CU_ASSERT(solver_vector->polynomial_column[2] == solver_polynomial);

  solver_polynomial = 
    solver_polynomial_arr_new[2] = ags_solver_polynomial_new();
  ags_solver_vector_insert_polynomial(solver_vector,
				      solver_polynomial,
				      4);

  CU_ASSERT(solver_vector->polynomial_column != NULL);
  CU_ASSERT(solver_vector->polynomial_column[4] == solver_polynomial);
}

void
ags_solver_vector_test_remove_polynomial()
{
  AgsSolverVector *solver_vector;
  AgsSolverPolynomial *item_0, *item_1;
  
  solver_vector = ags_solver_vector_new();
  
  item_0 = ags_solver_polynomial_new();
  ags_solver_vector_insert_polynomial(solver_vector,
				      item_0,
				      -1);
  
  item_1 = ags_solver_polynomial_new();
  ags_solver_vector_insert_polynomial(solver_vector,
				      item_1,
				      -1);

  CU_ASSERT(solver_vector->polynomial_column[0] == item_0);
  CU_ASSERT(solver_vector->polynomial_column[1] == item_1);

  ags_solver_vector_remove_polynomial(solver_vector,
				      item_0);

  CU_ASSERT(solver_vector->polynomial_column[0] == item_1);

  ags_solver_vector_remove_polynomial(solver_vector,
				      item_1);

  CU_ASSERT(solver_vector->polynomial_column == NULL);
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsSolverVectorTest", ags_solver_vector_test_init_suite, ags_solver_vector_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsSolverVector get source polynomial", ags_solver_vector_test_get_source_polynomial) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverVector set source polynomial", ags_solver_vector_test_set_source_polynomial) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverVector get polynomial count", ags_solver_vector_test_get_polynomial_count) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverVector insert polynomial", ags_solver_vector_test_insert_polynomial) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverVector remove polynomial", ags_solver_vector_test_remove_polynomial) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
