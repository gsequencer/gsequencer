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
void ags_math_util_test_lookup_exponent();
void ags_math_util_test_lookup_function();
void ags_math_util_test_lookup_term();
void ags_math_util_test_add_term();
void ags_math_util_test_subtract_term();
void ags_math_util_test_multiply_term();
void ags_math_util_test_divide_term();
void ags_math_util_test_raise_power_term();
void ags_math_util_test_extract_root_term();

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
  gchar *binomic_str_7 = "ℯ^2 * log(a + b)";
  gchar *binomic_str_8 = "ℯ^2 * log(a - b)";
  gchar *binomic_str_9 = "a^2 + 2ab + b^2";

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

  /* assert binomic #9 */
  ags_math_util_find_parantheses_all(binomic_str_9,
				     &open_position, &close_position,
				     &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 0);
  CU_ASSERT(close_position_count == 0);

  CU_ASSERT(open_position == NULL);
  
  CU_ASSERT(close_position == NULL);
}

void
ags_math_util_test_find_exponent_parantheses()
{
  gchar *exponent_str_0 = "ℯ^(2𝜋𝑖) * log(x)";
  gchar *exponent_str_1 = "2^8 * 2^(-7)";
  
  gint *open_position, *close_position;
  
  guint open_position_count, close_position_count;

  /* assert exponent #0 */
  ags_math_util_find_exponent_parantheses(exponent_str_0,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 4);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 14);

  /* assert exponent #1 */
  ags_math_util_find_exponent_parantheses(exponent_str_1,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 8);
  
  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 11);
}

void
ags_math_util_test_find_function_parantheses()
{
  gchar *function_str_0 = "x0 * log(x1)";
  gchar *function_str_1 = "1.0 / exp(x0)";
  gchar *function_str_2 = "a0 * sin(1.0 / x0)";
  gchar *function_str_3 = "sin(1.0 / x0) * cos(1.0 / x1)";
  gchar *function_str_4 = "tan(c^2)";
  gchar *function_str_5 = "asin(x1 - x0) - (b^3)";
  gchar *function_str_6 = "acos(x1 - x0) + (a^(1.0 / 3.0))";
  gchar *function_str_7 = "a^(1.0 / 2.0) * atan(a * b)";
  gchar *function_str_8 = "(b - a) - floor(x1 - x0)";
  gchar *function_str_9 = "x1 + ceil(0.25 * x0)";
  gchar *function_str_10 = "round(1.0 / x0 - (b - a))";
  
  gint *open_position, *close_position;
  
  guint open_position_count, close_position_count;

  /* assert function #0 */
  ags_math_util_find_function_parantheses(function_str_0,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 8);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 11);

  /* assert function #1 */
  ags_math_util_find_function_parantheses(function_str_1,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 9);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 12);

  /* assert function #2 */
  ags_math_util_find_function_parantheses(function_str_2,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 8);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 17);

  /* assert function #3 */
  ags_math_util_find_function_parantheses(function_str_3,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 2);
  CU_ASSERT(close_position_count == 2);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 3 &&
	    open_position[1] == 19);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 12 &&
	    close_position[1] == 28);

  /* assert function #4 */
  ags_math_util_find_function_parantheses(function_str_4,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 3);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 7);

  /* assert function #5 */
  ags_math_util_find_function_parantheses(function_str_5,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 4);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 12);

  /* assert function #6 */
  ags_math_util_find_function_parantheses(function_str_6,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 4);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 12);

  /* assert function #7 */
  ags_math_util_find_function_parantheses(function_str_7,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 20);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 26);

  /* assert function #8 */
  ags_math_util_find_function_parantheses(function_str_8,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 15);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 23);
  
  /* assert function #9 */
  ags_math_util_find_function_parantheses(function_str_9,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 9);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 19);
  
  /* assert function #10 */
  ags_math_util_find_function_parantheses(function_str_10,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 5);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 24);
}

void
ags_math_util_test_find_term_parantheses()
{
  gchar *term_str_0 = "a + b";
  gchar *term_str_1 = "(a + b) * (a + b)";
  gchar *term_str_2 = "(a + b + c) * (a + b + c) * (a + b + c)";
  gchar *term_str_3 = "x * (1.0 / (a + b))";
  
  gint *open_position, *close_position;
  
  guint open_position_count, close_position_count;

  /* assert function #0 */
  ags_math_util_find_term_parantheses(term_str_0,
				      &open_position, &close_position,
				      &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 0);
  CU_ASSERT(close_position_count == 0);

  CU_ASSERT(open_position == NULL);

  CU_ASSERT(close_position == NULL);
  
  /* assert function #1 */
  ags_math_util_find_term_parantheses(term_str_1,
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

  /* assert function #2 */
  ags_math_util_find_term_parantheses(term_str_2,
				      &open_position, &close_position,
				      &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 3);
  CU_ASSERT(close_position_count == 3);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 0 &&
	    open_position[1] == 14 &&
	    open_position[2] == 28);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 10 &&
	    close_position[1] == 24 &&
	    close_position[2] == 38);

  /* assert function #3 */
  ags_math_util_find_term_parantheses(term_str_3,
				      &open_position, &close_position,
				      &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 2);
  CU_ASSERT(close_position_count == 2);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 4 &&
	    open_position[1] == 11);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 17 &&
	    close_position[1] == 18);
}

void
ags_math_util_test_find_literals()
{
  gchar **literals;

  gchar *literals_str_0 = "a0 + b0 * sin((x1 - x0) / f0)";

  guint symbol_count;

  literals = NULL;
  symbol_count = 0;
  
  literals = ags_math_util_find_literals(literals_str_0,
					 &symbol_count);

  CU_ASSERT(symbol_count == 5);
  
  CU_ASSERT(literals != NULL);

  CU_ASSERT(g_strv_contains(literals, "a0"));
  CU_ASSERT(g_strv_contains(literals, "b0"));

  CU_ASSERT(g_strv_contains(literals, "x0"));
  CU_ASSERT(g_strv_contains(literals, "x1"));

  CU_ASSERT(g_strv_contains(literals, "f0"));
}

void
ags_math_util_test_lookup_exponent()
{
  gchar *equation_str_0 = "2^(x0 - 8) = 0";
  gchar *equation_str_1 = "2^(x1 - x0) = 0";
  gchar *equation_str_2 = "2^(4 * x0 - 32) = 0";
  gchar *equation_str_3 = "2^(-x0 + 8) - 2^(4x0 - 32) = 0";
  gchar *equation_str_4 = "exp(-x0 + 8) = 0";
  gchar *term_0 = "x0 - 8";

  gint *term_position;
  guint term_count;
  
  /* assert equation string #0 */
  ags_math_util_lookup_exponent(equation_str_0,
				term_0,
				&term_position, &term_count);

  CU_ASSERT(term_position != NULL);
  CU_ASSERT(term_count == 1);

  CU_ASSERT(term_position[0] == 3);
  
  /* assert equation string #1 */
  ags_math_util_lookup_exponent(equation_str_1,
				term_0,
				&term_position, &term_count);

  CU_ASSERT(term_position == NULL);
  CU_ASSERT(term_count == 0);

  /* assert equation string #2 */
  ags_math_util_lookup_exponent(equation_str_2,
				term_0,
				&term_position, &term_count);

  CU_ASSERT(term_position != NULL);
  CU_ASSERT(term_count == 1);

  CU_ASSERT(term_position[0] == 3);
  
  /* assert equation string #3 */
  ags_math_util_lookup_exponent(equation_str_3,
				term_0,
				&term_position, &term_count);

  CU_ASSERT(term_position != NULL);
  CU_ASSERT(term_count == 2);

  CU_ASSERT(term_position[0] == 3);
  CU_ASSERT(term_position[1] == 17);

  /* assert equation string #4 */
  ags_math_util_lookup_exponent(equation_str_4,
				term_0,
				&term_position, &term_count);

  CU_ASSERT(term_position != NULL);
  CU_ASSERT(term_count == 1);

  CU_ASSERT(term_position[0] == 4);
}

void
ags_math_util_test_lookup_function()
{
  //TODO:JK: implement me
}

void
ags_math_util_test_lookup_term()
{
  //TODO:JK: implement me
}

void
ags_math_util_test_add_term()
{
  //TODO:JK: implement me
}

void
ags_math_util_test_subtract_term()
{
  //TODO:JK: implement me
}

void
ags_math_util_test_multiply_term()
{
  //TODO:JK: implement me
}

void
ags_math_util_test_divide_term()
{
  //TODO:JK: implement me
}

void
ags_math_util_test_raise_power_term()
{
  //TODO:JK: implement me
}

void
ags_math_util_test_extract_root_term()
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
     (CU_add_test(pSuite, "test of ags_math_util.c find literals", ags_math_util_test_find_literals) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c lookup exponent", ags_math_util_test_lookup_exponent) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c lookup function", ags_math_util_test_lookup_function) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c lookup term", ags_math_util_test_lookup_term) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c add term", ags_math_util_test_add_term) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c subtract term", ags_math_util_test_subtract_term) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c multiply term", ags_math_util_test_multiply_term) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c divide term", ags_math_util_test_divide_term) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c raise power term", ags_math_util_test_raise_power_term) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c extract root term", ags_math_util_test_extract_root_term) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
