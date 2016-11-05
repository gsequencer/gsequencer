/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2016 Joël Krähemann
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

#include <ags/lib/ags_function.h>

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_function_test_init_suite();
int ags_function_test_clean_suite();

void ags_function_test_find_literals();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_function_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_function_test_clean_suite()
{
  return(0);
}

void
ags_function_test_find_literals()
{
  AgsFunction *function;
  
  gchar *log_x = "log(x)\0";
  gchar *exp_x = "exp(x)\0";
  gchar *floor_y_per_x = "floor(y / x)\0";
  gchar *ceil_y_per_x = "ceil(y / x)\0";
  gchar *round = "round(0.75)\0";
  gchar *sin_per_x = "sin(1.0 / x)\0";
  gchar *cos_per_x_plus_pi_half = "cos(1.0 / x + (𝜋 / 2.0))\0";
  gchar *tan_x = "tan(x)\0";
  gchar *asin_x0_plus_y1 = "asin(x0 + x1)\0";
  gchar *acos_x0_plus_y1 = "acos(x0 + x1)\0";
  gchar *atan_x_plus_y = "atan(2x + 0.75y)\0";
  gchar **symbols;

  guint n_symbols;
  
  /* assert log x */
  function = ags_function_new(log_x);
  symbols = ags_function_find_literals(function,
				       &n_symbols);

  CU_ASSERT(n_symbols == 1 &&
	    !g_ascii_strncasecmp(symbols[0],
				 "x\0",
				 2));
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C\0");
  putenv("LANG=C\0");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFunctionTest\0", ags_function_test_init_suite, ags_function_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsFunction find literals\0", ags_function_test_find_literals) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
