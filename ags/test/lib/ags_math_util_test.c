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

void ags_math_util_test_find_parenthesis_all();
void ags_math_util_test_find_exponent_parenthesis();
void ags_math_util_test_find_function_parenthesis();
void ags_math_util_test_find_term_parenthesis();
void ags_math_util_test_find_symbol_all();
void ags_math_util_test_is_term();
void ags_math_util_test_split_polynomial();

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
ags_math_util_test_find_parenthesis_all()
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
  ags_math_util_find_parenthesis_all(binomic_str_0,
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
  ags_math_util_find_parenthesis_all(binomic_str_1,
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
  ags_math_util_find_parenthesis_all(binomic_str_2,
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
  ags_math_util_find_parenthesis_all(binomic_str_3,
				     &open_position, &close_position,
				     &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 0);
  
  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 6);

  /* assert binomic #4 */
  ags_math_util_find_parenthesis_all(binomic_str_4,
				     &open_position, &close_position,
				     &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 0);
  
  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 6);
  
  /* assert binomic #5 */
  ags_math_util_find_parenthesis_all(binomic_str_5,
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
  ags_math_util_find_parenthesis_all(binomic_str_6,
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
  ags_math_util_find_parenthesis_all(binomic_str_7,
				     &open_position, &close_position,
				     &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 11);
  
  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 17);

  /* assert binomic #8 */
  ags_math_util_find_parenthesis_all(binomic_str_8,
				     &open_position, &close_position,
				     &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 11);
  
  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 17);

  /* assert binomic #9 */
  ags_math_util_find_parenthesis_all(binomic_str_9,
				     &open_position, &close_position,
				     &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 0);
  CU_ASSERT(close_position_count == 0);

  CU_ASSERT(open_position == NULL);
  
  CU_ASSERT(close_position == NULL);
}

void
ags_math_util_test_find_exponent_parenthesis()
{
  gchar *exponent_str_0 = "ℯ^(2𝜋𝑖) * log(x)";
  gchar *exponent_str_1 = "2^8 * 2^(-7)";
  
  gint *open_position, *close_position;
  
  guint open_position_count, close_position_count;

  /* assert exponent #0 */
  ags_math_util_find_exponent_parenthesis(exponent_str_0,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 4);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 14);

  /* assert exponent #1 */
  ags_math_util_find_exponent_parenthesis(exponent_str_1,
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
ags_math_util_test_find_function_parenthesis()
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
  ags_math_util_find_function_parenthesis(function_str_0,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 8);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 11);

  /* assert function #1 */
  ags_math_util_find_function_parenthesis(function_str_1,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 9);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 12);

  /* assert function #2 */
  ags_math_util_find_function_parenthesis(function_str_2,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 8);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 17);

  /* assert function #3 */
  ags_math_util_find_function_parenthesis(function_str_3,
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
  ags_math_util_find_function_parenthesis(function_str_4,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 3);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 7);

  /* assert function #5 */
  ags_math_util_find_function_parenthesis(function_str_5,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 4);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 12);

  /* assert function #6 */
  ags_math_util_find_function_parenthesis(function_str_6,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 4);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 12);

  /* assert function #7 */
  ags_math_util_find_function_parenthesis(function_str_7,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 20);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 26);

  /* assert function #8 */
  ags_math_util_find_function_parenthesis(function_str_8,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 15);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 23);
  
  /* assert function #9 */
  ags_math_util_find_function_parenthesis(function_str_9,
					  &open_position, &close_position,
					  &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 1);
  CU_ASSERT(close_position_count == 1);

  CU_ASSERT(open_position != NULL &&
	    open_position[0] == 9);

  CU_ASSERT(close_position != NULL &&
	    close_position[0] == 19);
  
  /* assert function #10 */
  ags_math_util_find_function_parenthesis(function_str_10,
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
ags_math_util_test_find_term_parenthesis()
{
  gchar *term_str_0 = "a + b";
  gchar *term_str_1 = "(a + b) * (a + b)";
  gchar *term_str_2 = "(a + b + c) * (a + b + c) * (a + b + c)";
  gchar *term_str_3 = "x * (1.0 / (a + b))";
  
  gint *open_position, *close_position;
  
  guint open_position_count, close_position_count;

  /* assert function #0 */
  ags_math_util_find_term_parenthesis(term_str_0,
				      &open_position, &close_position,
				      &open_position_count, &close_position_count);

  CU_ASSERT(open_position_count == 0);
  CU_ASSERT(close_position_count == 0);

  CU_ASSERT(open_position == NULL);

  CU_ASSERT(close_position == NULL);
  
  /* assert function #1 */
  ags_math_util_find_term_parenthesis(term_str_1,
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
  ags_math_util_find_term_parenthesis(term_str_2,
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
  ags_math_util_find_term_parenthesis(term_str_3,
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
ags_math_util_test_find_symbol_all()
{
  gchar **symbol_all;

  gchar *symbol_all_str_0 = "a0 + b0 * sin((x1 - x0) / f0)";

  symbol_all = NULL;
  
  symbol_all = ags_math_util_find_symbol_all(symbol_all_str_0);

  CU_ASSERT(symbol_all != NULL);
  
  CU_ASSERT(g_strv_length(symbol_all) == 5);
  
  CU_ASSERT(g_strv_contains(symbol_all, "a0"));
  CU_ASSERT(g_strv_contains(symbol_all, "b0"));

  CU_ASSERT(g_strv_contains(symbol_all, "x0"));
  CU_ASSERT(g_strv_contains(symbol_all, "x1"));

  CU_ASSERT(g_strv_contains(symbol_all, "f0"));
}

void
ags_math_util_test_is_term()
{
  gchar **iter;

  gboolean success;
  
  static const gchar *term[] = {
    "a",
    "-a",
    "ab",
    "-ab",
    "3ab",
    "3 * ab",
    "3.0ab",
    "3.0 * ab",
    "-3ab",
    "-3 * ab",
    "-3.0ab",
    "-3.0 * ab",
    "-3.0 * a * b",
    "3.0x0y0",
    "3.0 * x0y0",
    "3.0 * x0 * y0",
    "-3.0x0y0",
    "-3.0 * x0y0",
    "-3.0 * x0 * y0",
    "3.9a^(2)",
    "3.9 * a^(2)",
    "3.9a^(2.5)",
    "3.9 * a^(2.5)",
    "-3.9a^(-2)",
    "-3.9 * a^(-2)",
    "-3.9a^(-2)",
    "-3.9 * a^(-2)",
    "2a^(2𝜋)b^(-0.25𝜋)",
    "2 * a^(2𝜋)b^(-0.25𝜋)",
    "2 * a^(2𝜋) * b^(-0.25𝜋)",
    "-2a^(-2ℯ𝑖)b^(0.25𝜋)",
    "-2 * a^(-2ℯ𝑖)b^(0.25𝜋)",
    "-2 * a^(-2 * ℯ * 𝑖)b^(0.25 * 𝜋)",
    "-2 * a^(-2ℯ𝑖) * b^(0.25𝜋)",
    "-2 * a^(-2 * ℯ * 𝑖) * b^(0.25 * 𝜋)",
    "2 * x0^(2𝜋)y0^(-0.25𝜋)",
    "2 * x0^(2𝜋)y0^(-0.25 * 𝜋)",
    "-2 * x0^(-2ℯ𝑖) * y0^(0.25𝜋)",
    "-2 * x0^(-2ℯ𝑖) * y0^(0.25 * 𝜋)",
    "+2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋)",
    "2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) + 3",
    "2𝜋 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) + 3",
    "2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) + 2𝜋",
    "2 * 𝜋 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) + 2𝜋",
    "2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) + 2𝜋",
    "2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) + 2 * 𝜋",
    "2𝜋 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) + 2 * 𝜋",
    NULL,
  };

  static const gchar *func[] = {
    "sin(x0)",
    "sin(x0 + x1)",
    "sin(x0 * x1)",
    "2sin(x0)",
    "2.0sin(x0)",
    "2sin(x0 + x1)",
    "2.0sin(x0 + x1)",
    "2sin(x0 * x1)",
    "2.0sin(x0 * x1)",
    "-2sin(x0)",
    "-2sin(x0 + x1)",
    "-2sin(x0 * x1)",
    "2 * sin(x0)",
    "2 * sin(x0 + x1)",
    "2 * sin(x0 * x1)",
    "-2 * sin(x0)",
    "-2 * sin(x0 + x1)",
    "-2 * sin(x0 * x1)",
    "2.0 * sin(x0)",
    "2.0 * sin(x0 + x1)",
    "2.0 * sin(x0 * x1)",
    "-2.0 * sin(x0)",
    "-2.0 * sin(x0 + x1)",
    "-2.0 * sin(x0 * x1)",
    "+2.0 * sin(x0 * x1)",
    "2.0 * sin(x0 * x1) + 3",
    "2.0𝜋 * sin(x0 * x1) + 3",
    "2.0 * sin(x0 * x1) + 2𝜋",
    "2.0 * 𝜋 * sin(x0 * x1) + 2𝜋",
    "2.0 * sin(x0 * x1) + 2 * 𝜋",
    "2.0 * sin(x0 * x1) + 2𝜋",
    "2.0𝜋 * sin(x0 * x1) + 2𝜋",
    "2.0 * sin(x0 * x1) + 2 * 𝜋",
    "2.0 * 𝜋 * sin(x0 * x1) + 2 * 𝜋",
    NULL,
  };

#if 0
  /* match */
  success = TRUE;
  
  for(iter = term; iter[0] != NULL; iter++){
    if(!ags_math_util_is_term(iter[0])){
      g_message("assert failed - %s", iter[0]);
      
      success = FALSE;
    }
  }

  CU_ASSERT(success == TRUE);

  /* negative match */
  success = TRUE;
  
  for(iter = func; iter[0] != NULL; iter++){
    if(ags_math_util_is_term(iter[0])){
      g_message("negative assert failed - %s", iter[0]);
      
      success = FALSE;
    }
  }

  CU_ASSERT(success == TRUE);
#endif
}

struct _AgsStrvArr{
  gchar *strv[128];
};

void
ags_math_util_test_split_polynomial()
{
  guint i;
  gboolean success;
  
  static const gchar *polynomial[] = {
    "a", /* #000 */
    "-a", /* #001 */
    "ab", /* #002 */
    "-ab", /* #003 */
    "3ab", /* #004 */
    "3 * ab", /* #005 */
    "3.0ab", /* #006 */
    "3.0 * ab", /* #007 */
    "-3ab", /* #008 */
    "-3 * ab", /* #009 */
    "-3.0ab", /* #010 */
    "-3.0 * ab", /* #011 */
    "-3.0 * a * b", /* #012 */
    "3.0x0y0", /* #013 */
    "3.0 * x0y0", /* #014 */
    "3.0 * x0 * y0", /* #015 */
    "-3.0x0y0", /* #016 */
    "-3.0 * x0y0", /* #017 */
    "-3.0 * x0 * y0", /* #018 */
    "3.9a^(2)", /* #019 */
    "3.9 * a^(2)", /* #020 */
    "3.9a^(2.5)", /* #021 */
    "3.9 * a^(2.5)", /* #022 */
    "-3.9a^(-2)", /* #023 */
    "-3.9 * a^(-2)", /* #024 */
    "2a^(2𝜋)b^(-0.25𝜋)", /* #025 */
    "2 * a^(2𝜋)b^(-0.25𝜋)", /* #026 */
    "2 * a^(2𝜋) * b^(-0.25𝜋)", /* #027 */
    "-2a^(-2ℯ𝑖)b^(0.25𝜋)", /* #028 */
    "-2 * a^(-2ℯ𝑖)b^(0.25𝜋)", /* #029 */
    "-2 * a^(-2 * ℯ * 𝑖)b^(0.25 * 𝜋)", /* #030 */
    "-2 * a^(-2ℯ𝑖) * b^(0.25𝜋)", /* #031 */
    "-2 * a^(-2 * ℯ * 𝑖) * b^(0.25 * 𝜋)", /* #032 */
    "2 * x0^(2𝜋)y0^(-0.25𝜋)", /* #033 */
    "2 * x0^(2𝜋)y0^(-0.25 * 𝜋)", /* #034 */
    "-2 * x0^(-2ℯ𝑖) * y0^(0.25𝜋)", /* #035 */
    "-2 * x0^(-2ℯ𝑖) * y0^(0.25 * 𝜋)", /* #036 */
    "+2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋)", /* #037 */
    "2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋)", /* #038 */
    "2𝜋 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋)", /* #039 */
    "2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋)", /* #040 */
    "2 * 𝜋 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋)", /* #041 */
    "2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋)", /* #042 */
    "2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋)", /* #043 */
    "2𝜋 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋)", /* #044 */
    NULL,
  };

  struct _AgsStrvArr polynomial_factor[64] = {
    {"a", NULL}, /* #000 a */
    {"-a", NULL}, /* #001 -a */
    {"a", "b", NULL}, /* #002 ab */
    {"-a", "b", NULL}, /* #003 -ab */
    {"3", "a", "b", NULL}, /* #004 3ab */
    {"3", "a", "b", NULL}, /* #005 3 * ab */
    {"3.0", "a", "b", NULL}, /* #006 3.0ab */
    {"3.0", "a", "b", NULL}, /* #007 3.0 * ab */
    {"-3", "a", "b", NULL}, /* #008 -3ab */
    {"-3", "a", "b", NULL}, /* #009 -3 * ab */
    {"-3.0", "a", "b", NULL}, /* #010 -3.0ab */
    {"-3.0", "a", "b", NULL}, /* #011 -3.0 * ab */
    {"-3.0", "a", "b", NULL}, /* #012 -3.0 * a * b */
    {"3.0", "x0", "y0", NULL}, /* #013 3.0x0y0 */
    {"3.0", "x0", "y0", NULL}, /* #014 3.0 * x0y0 */
    {"3.0", "x0", "y0", NULL}, /* #015 3.0 * x0 * y0 */
    {"-3.0", "x0", "y0", NULL}, /* #016 -3.0x0y0 */
    {"-3.0", "x0", "y0", NULL}, /* #017 -3.0 * x0y0 */
    {"-3.0", "x0", "y0", NULL}, /* #018 -3.0 * x0 * y0 */
    {"3.9", "a", NULL}, /* #019 3.9a^(2) */
    {"3.9", "a", NULL}, /* #020 3.9 * a^(2) */
    {"3.9", "a", NULL}, /* #021 3.9a^(2.5) */
    {"3.9", "a", NULL}, /* #022 3.9 * a^(2.5) */
    {"-3.9", "a", NULL}, /* #023 -3.9a^(-2) */
    {"-3.9", "a", NULL}, /* #024 -3.9 * a^(-2) */
    {"2", "a", "b", NULL}, /* #025 2a^(2𝜋)b^(-0.25𝜋) */
    {"2", "a", "b", NULL}, /* #026 2 * a^(2𝜋)b^(-0.25𝜋) */
    {"2", "a", "b", NULL}, /* #027 2 * a^(2𝜋) * b^(-0.25𝜋) */
    {"-2", "a", "b", NULL}, /* #028 -2a^(-2ℯ𝑖)b^(0.25𝜋) */
    {"-2", "a", "b", NULL}, /* #029 -2 * a^(-2ℯ𝑖)b^(0.25𝜋) */
    {"-2", "a", "b", NULL}, /* #030 -2 * a^(-2 * ℯ * 𝑖)b^(0.25 * 𝜋) */
    {"-2", "a", "b", NULL}, /* #031 -2 * a^(-2ℯ𝑖) * b^(0.25𝜋) */
    {"-2", "a", "b", NULL}, /* #032 -2 * a^(-2 * ℯ * 𝑖) * b^(0.25 * 𝜋) */
    {"2", "x0", "y0", NULL}, /* #033 2 * x0^(2𝜋)y0^(-0.25𝜋) */
    {"2", "x0", "y0", NULL}, /* #034 2 * x0^(2𝜋)y0^(-0.25 * 𝜋) */
    {"-2", "x0", "y0", NULL}, /* #035 -2 * x0^(-2ℯ𝑖) * y0^(0.25𝜋) */
    {"-2", "x0", "y0", NULL}, /* #036 -2 * x0^(-2ℯ𝑖) * y0^(0.25 * 𝜋) */
    {"2", "x0", "y0", NULL}, /* #037 +2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) */
    {"2", "x0", "y0", NULL}, /* #038 2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) */
    {"2", "𝜋", "x0", "y0", NULL}, /* #039 2𝜋 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) */
    {"2", "x0", "y0", NULL}, /* #040 2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) */
    {"2", "𝜋", "x0", "y0", NULL}, /* #041 2 * 𝜋 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) */
    {"2", "x0", "y0", NULL}, /* #042 2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) */
    {"2", "x0", "y0", NULL}, /* #043 2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) */
    {"2", "𝜋", "x0", "y0", NULL}, /* #044 2𝜋 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) */
    NULL,
  };

  struct _AgsStrvArr polynomial_factor_exponent[64] = {
    {"1", NULL}, /* #000 a */
    {"1", NULL}, /* #001 -a */
    {"1", "1", NULL}, /* #002 ab */
    {"1", "1", NULL}, /* #003 -ab */
    {"1", "1", "1", NULL}, /* #004 3ab */
    {"1", "1", "1", NULL}, /* #005 3 * ab */
    {"1", "1", "1", NULL}, /* #006 3.0ab */
    {"1", "1", "1", NULL}, /* #007 3.0 * ab */
    {"1", "1", "1", NULL}, /* #008 -3ab */
    {"1", "1", "1", NULL}, /* #009 -3 * ab */
    {"1", "1", "1", NULL}, /* #010 -3.0ab */
    {"1", "1", "1", NULL}, /* #011 -3.0 * ab */
    {"1", "1", "1", NULL}, /* #012 -3.0 * a * b */
    {"1", "1", "1", NULL}, /* #013 3.0x0y0 */
    {"1", "1", "1", NULL}, /* #014 3.0 * x0y0 */
    {"1", "1", "1", NULL}, /* #015 3.0 * x0 * y0 */
    {"1", "1", "1", NULL}, /* #016 -3.0x0y0 */
    {"1", "1", "1", NULL}, /* #017 -3.0 * x0y0 */
    {"1", "1", "1", NULL}, /* #018 -3.0 * x0 * y0 */
    {"1", "2", NULL}, /* #019 3.9a^(2) */
    {"1", "2", NULL}, /* #020 3.9 * a^(2) */
    {"1", "2.5", NULL}, /* #021 3.9a^(2.5) */
    {"1", "2.5", NULL}, /* #022 3.9 * a^(2.5) */
    {"1", "-2", NULL}, /* #023 -3.9a^(-2) */
    {"1", "-2", NULL}, /* #024 -3.9 * a^(-2) */
    {"1", "2𝜋", "-0.25𝜋", NULL}, /* #025 2a^(2𝜋)b^(-0.25𝜋) */
    {"1", "2𝜋", "-0.25𝜋", NULL}, /* #026 2 * a^(2𝜋)b^(-0.25𝜋) */
    {"1", "2𝜋", "-0.25𝜋", NULL}, /* #027 2 * a^(2𝜋) * b^(-0.25𝜋) */
    {"1", "-2ℯ𝑖", "0.25𝜋", NULL}, /* #028 -2a^(-2ℯ𝑖)b^(0.25𝜋) */
    {"1", "-2ℯ𝑖", "0.25𝜋", NULL}, /* #029 -2 * a^(-2ℯ𝑖)b^(0.25𝜋) */
    {"1", "-2 * ℯ * 𝑖", "0.25 * 𝜋", NULL}, /* #030 -2 * a^(-2 * ℯ * 𝑖)b^(0.25 * 𝜋) */
    {"1", "-2ℯ𝑖", "0.25𝜋", NULL}, /* #031 -2 * a^(-2ℯ𝑖) * b^(0.25𝜋) */
    {"1", "-2 * ℯ * 𝑖", "0.25 * 𝜋", NULL}, /* #032 -2 * a^(-2 * ℯ * 𝑖) * b^(0.25 * 𝜋) */
    {"1", "2𝜋", "-0.25𝜋", NULL}, /* #033 2 * x0^(2𝜋)y0^(-0.25𝜋) */
    {"1", "2𝜋", "-0.25 * 𝜋", NULL}, /* #034 2 * x0^(2𝜋)y0^(-0.25 * 𝜋) */
    {"1", "-2ℯ𝑖", "0.25𝜋", NULL}, /* #035 -2 * x0^(-2ℯ𝑖) * y0^(0.25𝜋) */
    {"1", "-2ℯ𝑖", "0.25 * 𝜋", NULL}, /* #036 -2 * x0^(-2ℯ𝑖) * y0^(0.25 * 𝜋) */
    {"1", "-2ℯ𝑖", "+0.25 * 𝜋", NULL}, /* #037 +2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) */
    {"1", "-2ℯ𝑖", "+0.25 * 𝜋", NULL}, /* #038 2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) */
    {"1", "1", "-2ℯ𝑖", "+0.25 * 𝜋", NULL}, /* #039 2𝜋 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) */
    {"1", "-2ℯ𝑖", "+0.25 * 𝜋", NULL}, /* #040 2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) */
    {"1", "1", "-2ℯ𝑖", "+0.25 * 𝜋", NULL}, /* #041 2 * 𝜋 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) */
    {"1", "-2ℯ𝑖", "+0.25 * 𝜋", NULL}, /* #042 2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) */
    {"1", "-2ℯ𝑖", "+0.25 * 𝜋", NULL}, /* #043 2 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) */
    {"1", "1", "-2ℯ𝑖", "+0.25 * 𝜋", NULL}, /* #044 2𝜋 * x0^(-2ℯ𝑖) * y0^(+0.25 * 𝜋) */
    NULL,
  };

  success = TRUE;
  
  for(i = 0; polynomial[i] != NULL; i++){
    gchar **factor;
    gchar **factor_exponent;
    
    factor = NULL;
    factor_exponent = NULL;

    ags_math_util_split_polynomial(polynomial[i],
				&factor, &factor_exponent);

    if(factor == NULL ||
       polynomial_factor[i].strv == NULL ||
       !g_strv_equal(factor,
		     polynomial_factor[i].strv)){
      g_message("polynomial factor failed - %s", polynomial_factor[i]);
      
      success = FALSE;
    }

    if(factor_exponent == NULL ||
       polynomial_factor_exponent[i].strv == NULL ||
       !g_strv_equal(factor_exponent,
		     polynomial_factor_exponent[i].strv)){
      g_message("polynomial factor exponent failed - %s", polynomial_factor_exponent[i]);
      
      success = FALSE;
    }
  }

  CU_ASSERT(success == TRUE);
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
  if((CU_add_test(pSuite, "test of ags_math_util.c find parenthesis all", ags_math_util_test_find_parenthesis_all) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c find exponent parenthesis", ags_math_util_test_find_exponent_parenthesis) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c find function parenthesis", ags_math_util_test_find_function_parenthesis) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c find term parenthesis", ags_math_util_test_find_term_parenthesis) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c find symbol all", ags_math_util_test_find_symbol_all) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c is term", ags_math_util_test_is_term) == NULL) ||
     (CU_add_test(pSuite, "test of ags_math_util.c split polynomial", ags_math_util_test_split_polynomial) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
