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

int ags_solver_matrix_test_init_suite();
int ags_solver_matrix_test_clean_suite();

void ags_solver_matrix_test_get_source_function();
void ags_solver_matrix_test_set_source_function();
void ags_solver_matrix_test_get_row_count();
void ags_solver_matrix_test_get_column_count();
void ags_solver_matrix_test_parse();
void ags_solver_matrix_test_insert_vector();
void ags_solver_matrix_test_remove_vector();
void ags_solver_matrix_test_eliminate();

#define AGS_SOLVER_MATRIX_TEST_PARSE_EQUATION_0 "-2ab+a-4b+16"
#define AGS_SOLVER_MATRIX_TEST_PARSE_EQUATION_1 "-2a^(-3.14)b^(2.0)+a-4b+16"

#define AGS_SOLVER_MATRIX_TEST_ELIMINATE_EQUATION_0 "-2a^(4.0)b^(2.0)+a-4b+16;1.5a^(4.0)b^(2.0)+8a-9b+7"

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_solver_matrix_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_solver_matrix_test_clean_suite()
{
  return(0);
}

void
ags_solver_matrix_test_get_source_function()
{
  AgsSolverMatrix *solver_matrix;
  
  solver_matrix = ags_solver_matrix_new();

  ags_solver_matrix_set_source_function(solver_matrix,
					"-3.14a^(2)b+2a-8b+81");

  CU_ASSERT(solver_matrix->source_function != NULL &&
	    !g_strcmp0(solver_matrix->source_function, "-3.14a^(2)b+2a-8b+81"));
}

void
ags_solver_matrix_test_set_source_function()
{
  AgsSolverMatrix *solver_matrix;

  gchar *source_function;
  
  solver_matrix = ags_solver_matrix_new();

  solver_matrix->source_function = g_strdup("-3.14a^(2)b+2a-8b+81");

  source_function = ags_solver_matrix_get_source_function(solver_matrix);

  CU_ASSERT(source_function != NULL &&
	    !g_strcmp0(source_function, "-3.14a^(2)b+2a-8b+81"));
}

void
ags_solver_matrix_test_get_row_count()
{
  AgsSolverMatrix *solver_matrix;

  guint row_count;
  
  solver_matrix = ags_solver_matrix_new();

  solver_matrix->row_count = 2;
  
  row_count = ags_solver_matrix_get_row_count(solver_matrix);

  CU_ASSERT(row_count == 2);
}

void
ags_solver_matrix_test_get_column_count()
{
  AgsSolverMatrix *solver_matrix;

  guint column_count;
  
  solver_matrix = ags_solver_matrix_new();

  solver_matrix->column_count = 4;
  
  column_count = ags_solver_matrix_get_column_count(solver_matrix);

  CU_ASSERT(column_count == 4);
}

void
ags_solver_matrix_test_parse()
{
  AgsSolverMatrix *solver_matrix;
  
  solver_matrix = ags_solver_matrix_new();

  ags_solver_matrix_parse(solver_matrix,
			  AGS_SOLVER_MATRIX_TEST_PARSE_EQUATION_0);

  CU_ASSERT(solver_matrix->column_count == 4);

  solver_matrix = ags_solver_matrix_new();

  ags_solver_matrix_parse(solver_matrix,
			  AGS_SOLVER_MATRIX_TEST_PARSE_EQUATION_1);

  CU_ASSERT(solver_matrix->column_count == 4);
}

void
ags_solver_matrix_test_insert_vector()
{
  AgsSolverMatrix *solver_matrix;
  AgsSolverVector *solver_vector;
  AgsSolverVector* solver_vector_arr_orig[3];
  AgsSolverVector* solver_vector_arr_new[3];

  solver_matrix = ags_solver_matrix_new();

  CU_ASSERT(solver_matrix->term_table == NULL);

  /* append */
  solver_vector =
    solver_vector_arr_orig[0] = ags_solver_vector_new();
  ags_solver_matrix_insert_vector(solver_matrix,
				  solver_vector,
				  -1);

  CU_ASSERT(solver_matrix->term_table != NULL);
  CU_ASSERT(solver_matrix->term_table[0] == solver_vector);
  
  solver_vector =
    solver_vector_arr_orig[1] =  ags_solver_vector_new();
  ags_solver_matrix_insert_vector(solver_matrix,
				  solver_vector,
				  -1);

  CU_ASSERT(solver_matrix->term_table != NULL);
  CU_ASSERT(solver_matrix->term_table[1] == solver_vector);

  solver_vector = 
    solver_vector_arr_orig[2] = ags_solver_vector_new();
  ags_solver_matrix_insert_vector(solver_matrix,
				  solver_vector,
				  -1);

  CU_ASSERT(solver_matrix->term_table != NULL);
  CU_ASSERT(solver_matrix->term_table[2] == solver_vector);

  /* insert */
  solver_vector = 
    solver_vector_arr_new[0] = ags_solver_vector_new();
  ags_solver_matrix_insert_vector(solver_matrix,
				  solver_vector,
				  0);

  CU_ASSERT(solver_matrix->term_table != NULL);
  CU_ASSERT(solver_matrix->term_table[0] == solver_vector);

  solver_vector = 
    solver_vector_arr_new[1] = ags_solver_vector_new();
  ags_solver_matrix_insert_vector(solver_matrix,
				  solver_vector,
				  2);

  CU_ASSERT(solver_matrix->term_table != NULL);
  CU_ASSERT(solver_matrix->term_table[2] == solver_vector);

  solver_vector = 
    solver_vector_arr_new[2] = ags_solver_vector_new();
  ags_solver_matrix_insert_vector(solver_matrix,
				  solver_vector,
				  4);

  CU_ASSERT(solver_matrix->term_table != NULL);
  CU_ASSERT(solver_matrix->term_table[4] == solver_vector);
}

void
ags_solver_matrix_test_remove_vector()
{
  AgsSolverMatrix *solver_matrix;
  AgsSolverVector *col_0, *col_1;
  
  solver_matrix = ags_solver_matrix_new();
  
  col_0 = ags_solver_vector_new();
  ags_solver_matrix_insert_vector(solver_matrix,
				  col_0,
				  -1);
  
  col_1 = ags_solver_vector_new();
  ags_solver_matrix_insert_vector(solver_matrix,
				  col_1,
				  -1);

  CU_ASSERT(solver_matrix->term_table[0] == col_0);
  CU_ASSERT(solver_matrix->term_table[1] == col_1);

  ags_solver_matrix_remove_vector(solver_matrix,
				  col_0);

  CU_ASSERT(solver_matrix->term_table[0] == col_1);

  ags_solver_matrix_remove_vector(solver_matrix,
				  col_1);

  CU_ASSERT(solver_matrix->term_table == NULL);
}

void
ags_solver_matrix_test_eliminate()
{
  AgsSolverMatrix *solver_matrix;

  guint i;
  
  solver_matrix = ags_solver_matrix_new();

  ags_solver_matrix_parse(solver_matrix,
			  AGS_SOLVER_MATRIX_TEST_ELIMINATE_EQUATION_0);

  CU_ASSERT(solver_matrix->column_count == 4);

  for(i = 0; i < 4; i++){
    CU_ASSERT(solver_matrix->term_table[i]->polynomial_count == 2);
  }

  ags_solver_matrix_eliminate(solver_matrix,
			      0,
			      0, 1);
			      
  ags_solver_matrix_eliminate(solver_matrix,
			      1,
			      0, 1);

  ags_solver_matrix_eliminate(solver_matrix,
			      2,
			      0, 1);

  ags_solver_matrix_eliminate(solver_matrix,
			      3,
			      0, 1);
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
  pSuite = CU_add_suite("AgsSolverMatrixTest", ags_solver_matrix_test_init_suite, ags_solver_matrix_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsSolverMatrix get source function", ags_solver_matrix_test_get_source_function) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverMatrix set source function", ags_solver_matrix_test_set_source_function) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverMatrix get row count", ags_solver_matrix_test_get_row_count) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverMatrix get column count", ags_solver_matrix_test_get_column_count) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverMatrix parse", ags_solver_matrix_test_parse) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverMatrix insert vector", ags_solver_matrix_test_insert_vector) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverMatrix remove vector", ags_solver_matrix_test_remove_vector) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverMatrix eliminate", ags_solver_matrix_test_eliminate) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
