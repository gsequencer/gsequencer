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

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_math_util_test_init_suite();
int ags_math_util_test_clean_suite();

void ags_math_util_test_find_parantheses_all();
void ags_math_util_test_find_exponent_parantheses();
void ags_math_util_test_find_function_parantheses();
void ags_math_util_test_find_term_parantheses();
void ags_math_util_test_find_literals();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_math_util_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_math_util_test_clean_suite()
{
  return(0);
}

void
ags_math_util_test_find_parantheses_all()
{
  gchar *binomic_str_0 = "(a + b) * (a + b)";
  gchar *binomic_str_1 = "(a + b) * (a - b)";
  gchar *binomic_str_2 = "(a - b) * (a - b)";
  gchar *binomic_str_3 = "(a + b)^2";
  gchar *binomic_str_4 = "(a - b)^2";
  gchar *binomic_str_5 = "exp(2 * log(a + b))";
  gchar *binomic_str_6 = "exp(2 * log(a - b))";
  gchar *binomic_str_7 = "ℯ^2 * log(a - b)";
  gchar *binomic_str_8 = "ℯ^2 * log(a - b)";

  gint *open_position, *close_position;
  
  guint open_position_count, close_position_count;

  /* assert binomic #0 */
  ags_math_util_find_parantheses_all(binomic_str_0,
				     &open_position, &close_position,
				     &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 2);
  CU_ASSERT(close_position_count == 2);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 0 &&
	    open_position[1] == 10);
  
  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 6 &&
	    close_position[1] == 16);
  
  /* assert binomic #1 */
  ags_math_util_find_parantheses_all(binomic_str_1,
				     &open_position, &close_position,
				     &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 2);
  CU_ASSERT(close_position_count == 2);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 0 &&
	    open_position[1] == 10);
  
  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 6 &&
	    close_position[1] == 16);

  /* assert binomic #2 */
  ags_math_util_find_parantheses_all(binomic_str_2,
				     &open_position, &close_position,
				     &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 2);
  CU_ASSERT(close_position_count == 2);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 0 &&
	    open_position[1] == 10);
  
  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 6 &&
	    close_position[1] == 16);

  /* assert binomic #3 */
  ags_math_util_find_parantheses_all(binomic_str_3,
				     &open_position, &close_position,
				     &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 0);
  
  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 6);

  /* assert binomic #4 */
  ags_math_util_find_parantheses_all(binomic_str_4,
				     &open_position, &close_position,
				     &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 0);
  
  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 6);
  
  /* assert binomic #5 */
  ags_math_util_find_parantheses_all(binomic_str_5,
				     &open_position, &close_position,
				     &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 2);
  CU_ASSERT(close_position_count == 2);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 3 &&
	    open_position[1] == 11);
  
  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 17 &&
	    close_position[1] == 18);

  /* assert binomic #6 */
  ags_math_util_find_parantheses_all(binomic_str_6,
				     &open_position, &close_position,
				     &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 2);
  CU_ASSERT(close_position_count == 2);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 3 &&
	    open_position[1] == 11);
  
  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 17 &&
	    close_position[1] == 18);

  /* assert binomic #7 */
  ags_math_util_find_parantheses_all(binomic_str_7,
				     &open_position, &close_position,
				     &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 11);
  
  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 17);

  /* assert binomic #8 */
  ags_math_util_find_parantheses_all(binomic_str_8,
				     &open_position, &close_position,
				     &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 11);
  
  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 17);
}

void
ags_math_util_test_find_exponent_parantheses()
{
  //TODO:JK: implement me
}

void
ags_math_util_test_find_function_parantheses()
{
  //TODO:JK: implement me
}

void
ags_math_util_test_find_term_parantheses()
{
  //TODO:JK: implement me
}

void
ags_math_util_test_find_literals()
{
  //TODO:JK: implement me
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
  pSuite = CU_add_suite("AgsMathUtilTest", ags_math_util_test_init_suite, ags_math_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_math_util.c find parantheses all", ags_math_util_test_find_parantheses_all) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c find exponent parantheses", ags_math_util_test_find_exponent_parantheses) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c find function parantheses", ags_math_util_test_find_function_parantheses) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c find term parantheses", ags_math_util_test_find_term_parantheses) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c find literals", ags_math_util_test_find_literals) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

