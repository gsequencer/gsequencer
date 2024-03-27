/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/libags.h>

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <math.h>

int ags_string_util_test_init_suite();
int ags_string_util_test_clean_suite();

void ags_string_util_test_escape_single_quote();
void ags_string_util_test_to_mcoded7();
void ags_string_util_test_from_mcoded7();

void ags_strv_test_length();
void ags_strv_test_contains();
void ags_strv_test_index();
void ags_strv_test_equal();

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_string_util_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_string_util_test_clean_suite()
{
  return(0);
}

void
ags_string_util_test_escape_single_quote()
{
  gchar *str;
  gchar *escaped_str;
  
  str = "this is 'important'";

  escaped_str = ags_string_util_escape_single_quote(str);
  
  CU_ASSERT(escaped_str != NULL);
  CU_ASSERT(strlen(escaped_str) > 0);
}

void
ags_string_util_test_to_mcoded7()
{
  gchar *str;
  gchar *encoded_str;
  
  str = "this is 'important'";

  encoded_str = ags_string_util_to_mcoded7(str);
  
  CU_ASSERT(encoded_str != NULL);
}

void
ags_string_util_test_from_mcoded7()
{
  gchar *str;
  gchar *encoded_str;
  
  encoded_str = ags_string_util_to_mcoded7("this is 'important'");

  str = ags_string_util_from_mcoded7(encoded_str);
  
  CU_ASSERT(str != NULL);
}

void
ags_strv_test_length()
{
  gchar* strv[] = {
    "strv test length",
    NULL,
  };

  gchar* strv_a[] = {
    "strv test length - a",
    "strv test length - a",
    "strv test length - a",
    "strv test length - a",
    "strv test length - a",
    NULL,
  };

  guint length;

  length = ags_strv_length(strv);

  CU_ASSERT(length == 1);

  length = ags_strv_length(strv_a);

  CU_ASSERT(length == 5);
}

void
ags_strv_test_contains()
{
  gchar* strv[] = {
    "strv test length",
    NULL,
  };

  gchar* strv_a[] = {
    "strv test length - a",
    "strv test length - b",
    "strv test length - c",
    "strv test length - d",
    "strv test length - e",
    NULL,
  };

  gboolean success;

  success = ags_strv_contains(strv,
			      "strv test length");

  CU_ASSERT(success == TRUE);

  success = ags_strv_contains(strv_a,
			      "strv test length - c");

  CU_ASSERT(success == TRUE);

  success = ags_strv_contains(strv_a,
			      "strv test length - e");

  CU_ASSERT(success == TRUE);

  success = ags_strv_contains(strv_a,
			      "strv test length - x");

  CU_ASSERT(success == FALSE);
}

void
ags_strv_test_index()
{
  gchar* strv[] = {
    "strv test length",
    NULL,
  };

  gchar* strv_a[] = {
    "strv test length - a",
    "strv test length - b",
    "strv test length - c",
    "strv test length - d",
    "strv test length - e",
    NULL,
  };

  gint position;

  position = ags_strv_index(strv_a,
			    "strv test length - c");

  CU_ASSERT(position == 2);

  position = ags_strv_index(strv_a,
			    "strv test length - e");

  CU_ASSERT(position == 4);

  position = ags_strv_index(strv_a,
			    "strv test length - x");

  CU_ASSERT(position == -1);
}

void
ags_strv_test_equal()
{
  gchar* strv_a[] = {
    "strv test length - a",
    "strv test length - b",
    "strv test length - c",
    "strv test length - d",
    "strv test length - e",
    NULL,
  };

  gchar* strv_b[] = {
    "strv test length - a",
    "strv test length - b",
    "strv test length - c",
    "strv test length - d",
    "strv test length - e",
    NULL,
  };

  gchar* strv_c[] = {
    "strv test length - f",
    "strv test length - f",
    "strv test length - f",
    "strv test length - f",
    "strv test length - f",
    NULL,
  };

  gboolean success;

  success = ags_strv_equal(strv_a,
			   strv_b);

  CU_ASSERT(success == TRUE);

  success = ags_strv_equal(strv_a,
			   strv_c);

  CU_ASSERT(success == FALSE);
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
  pSuite = CU_add_suite("AgsStringUtilTest", ags_string_util_test_init_suite, ags_string_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsStringUtil escape single quote", ags_string_util_test_escape_single_quote) == NULL) ||
     (CU_add_test(pSuite, "test of AgsStringUtil to mcoded7", ags_string_util_test_to_mcoded7) == NULL) ||
     (CU_add_test(pSuite, "test of AgsStringUtil from mcoded7", ags_string_util_test_from_mcoded7) == NULL) ||
     (CU_add_test(pSuite, "test of AgsStringUtil strv length", ags_strv_test_length) == NULL) ||
     (CU_add_test(pSuite, "test of AgsStringUtil strv contains", ags_strv_test_contains) == NULL) ||
     (CU_add_test(pSuite, "test of AgsStringUtil strv index", ags_strv_test_index) == NULL) ||
     (CU_add_test(pSuite, "test of AgsStringUtil strv equal", ags_strv_test_equal) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
