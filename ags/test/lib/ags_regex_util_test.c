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

int ags_regex_util_test_init_suite();
int ags_regex_util_test_clean_suite();

void ags_regex_util_test_alloc();
void ags_regex_util_test_copy();
void ags_regex_util_test_free();
void ags_regex_util_test_get_app_encoding();
void ags_regex_util_test_get_encoding();
void ags_regex_util_test_compile();
void ags_regex_util_test_match_alloc();
void ags_regex_util_test_match_copy();
void ags_regex_util_test_match_free();
void ags_regex_util_test_match_get_offset();
void ags_regex_util_test_execute();
void ags_regex_util_test_execute_unichar();
void ags_regex_util_test_execute_unichar2();

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_regex_util_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_regex_util_test_clean_suite()
{
  return(0);
}

void
ags_regex_util_test_alloc()
{
  AgsRegexUtil *regex_util;

  regex_util = ags_regex_util_alloc(NULL,
				    NULL,
				    FALSE, FALSE);

  CU_ASSERT(regex_util != NULL);
}

void
ags_regex_util_test_copy()
{
  AgsRegexUtil *regex_util;
  AgsRegexUtil *copied_regex_util;

  regex_util = ags_regex_util_alloc(NULL,
				    NULL,
				    FALSE, FALSE);

  CU_ASSERT(regex_util != NULL);

  regex_util->app_encoding = g_strdup("UTF-8");
  regex_util->encoding = g_strdup("8859-15");
  
  copied_regex_util = ags_regex_util_copy(regex_util);

  CU_ASSERT(copied_regex_util != NULL);

  CU_ASSERT(!g_strcmp0(regex_util->app_encoding, copied_regex_util->app_encoding));
  CU_ASSERT(!g_strcmp0(regex_util->encoding, copied_regex_util->encoding));
}

void
ags_regex_util_test_free()
{
  AgsRegexUtil *regex_util;

  regex_util = ags_regex_util_alloc(NULL,
				    NULL,
				    FALSE, FALSE);

  CU_ASSERT(regex_util != NULL);

  ags_regex_util_free(regex_util);
}

void
ags_regex_util_test_get_app_encoding()
{
  AgsRegexUtil *regex_util;

  gchar *str;
  
  regex_util = ags_regex_util_alloc(NULL,
				    NULL,
				    FALSE, FALSE);

  CU_ASSERT(regex_util != NULL);

  regex_util->app_encoding = g_strdup("UTF-8");

  str = ags_regex_util_get_app_encoding(regex_util);

  CU_ASSERT(str != NULL && !g_strcmp0(str, "UTF-8"));

  g_free(str);
}

void
ags_regex_util_test_get_encoding()
{
  AgsRegexUtil *regex_util;

  gchar *str;
  
  regex_util = ags_regex_util_alloc(NULL,
				    NULL,
				    FALSE, FALSE);

  CU_ASSERT(regex_util != NULL);

  regex_util->encoding = g_strdup("UTF-8");

  str = ags_regex_util_get_encoding(regex_util);

  CU_ASSERT(str != NULL && !g_strcmp0(str, "UTF-8"));

  g_free(str);
}

void
ags_regex_util_test_compile()
{
  AgsRegexUtil *regex_util;

  gboolean success;

  GError *error;

  /*  */
  regex_util = ags_regex_util_alloc(NULL,
				    NULL,
				    FALSE, FALSE);

  CU_ASSERT(regex_util != NULL);

  error = NULL;
  success = ags_regex_util_compile(regex_util,
				   "^([a-zA-Z]+[0-9]*)", AGS_REGEX_UTIL_POSIX_EXTENDED_SYNTAX,
				   &error);

  ags_regex_util_free(regex_util);

  /* with app encoding */
  regex_util = ags_regex_util_alloc("UTF-8",
				    NULL,
				    FALSE, FALSE);

  CU_ASSERT(regex_util != NULL);

  error = NULL;
  success = ags_regex_util_compile(regex_util,
				   "^([a-zA-Z]+[0-9]*)", AGS_REGEX_UTIL_POSIX_EXTENDED_SYNTAX,
				   &error);

  ags_regex_util_free(regex_util);

  /* with app encoding and encoding */
  regex_util = ags_regex_util_alloc("UTF-8",
				    "8859-15",
				    FALSE, FALSE);

  CU_ASSERT(regex_util != NULL);

  error = NULL;
  success = ags_regex_util_compile(regex_util,
				   "^([a-zA-Z]+[0-9]*)", AGS_REGEX_UTIL_POSIX_EXTENDED_SYNTAX,
				   &error);

  ags_regex_util_free(regex_util);
}

void
ags_regex_util_test_match_alloc()
{
  AgsRegexUtil *regex_util;

  AgsRegexMatch *match;

  /*  */
  regex_util = ags_regex_util_alloc(NULL,
				    NULL,
				    FALSE, FALSE);

  CU_ASSERT(regex_util != NULL);

  match = ags_regex_util_match_alloc(regex_util,
				     1);

  CU_ASSERT(match != NULL);


  /*  */
  match = ags_regex_util_match_alloc(regex_util,
				     2);

  CU_ASSERT(match != NULL);
}

void
ags_regex_util_test_match_copy()
{
  AgsRegexUtil *regex_util;

  AgsRegexMatch *match;
  AgsRegexMatch *copy_match;

  /*  */
  regex_util = ags_regex_util_alloc(NULL,
				    NULL,
				    FALSE, FALSE);

  CU_ASSERT(regex_util != NULL);

  /*  */
  match = ags_regex_util_match_alloc(regex_util,
				     1);

  CU_ASSERT(match != NULL);

  copy_match = ags_regex_util_match_copy(regex_util,
					 match,
					 1);

  /*  */
  match = ags_regex_util_match_alloc(regex_util,
				     2);

  CU_ASSERT(match != NULL);

  copy_match = ags_regex_util_match_copy(regex_util,
					 match,
					 2);
}

void
ags_regex_util_test_match_free()
{
  AgsRegexUtil *regex_util;

  AgsRegexMatch *match;
  AgsRegexMatch *copy_match;

  /*  */
  regex_util = ags_regex_util_alloc(NULL,
				    NULL,
				    FALSE, FALSE);

  CU_ASSERT(regex_util != NULL);

  /*  */
  match = ags_regex_util_match_alloc(regex_util,
				     1);

  CU_ASSERT(match != NULL);

  ags_regex_util_match_free(regex_util,
			    match);
}

void
ags_regex_util_test_match_get_offset()
{
  AgsRegexUtil *regex_util;

  AgsRegexMatch *match;
  AgsRegexMatch *copy_match;

  gint start_match_offset, end_match_offset;
  
  /*  */
  regex_util = ags_regex_util_alloc(NULL,
				    NULL,
				    FALSE, FALSE);

  CU_ASSERT(regex_util != NULL);

  /*  */
  match = ags_regex_util_match_alloc(regex_util,
				     1);

  CU_ASSERT(match != NULL);

  match[0].start_match_offset = 0;
  match[0].end_match_offset = 8;
  
  start_match_offset = 0;
  end_match_offset = 0;
  
  ags_regex_util_match_get_offset(regex_util,
				  match,
				  0,
				  &start_match_offset, &end_match_offset);

  CU_ASSERT(start_match_offset == 0);
  CU_ASSERT(end_match_offset == 8);
  
  /*  */
  match = ags_regex_util_match_alloc(regex_util,
				     2);

  CU_ASSERT(match != NULL);

  match[0].start_match_offset = 0;
  match[0].end_match_offset = 8;

  match[1].start_match_offset = 9;
  match[1].end_match_offset = 16;

  start_match_offset = 0;
  end_match_offset = 0;
  
  ags_regex_util_match_get_offset(regex_util,
				  match,
				  0,
				  &start_match_offset, &end_match_offset);

  CU_ASSERT(start_match_offset == 0);
  CU_ASSERT(end_match_offset == 8);

  ags_regex_util_match_get_offset(regex_util,
				  match,
				  1,
				  &start_match_offset, &end_match_offset);

  CU_ASSERT(start_match_offset == 9);
  CU_ASSERT(end_match_offset == 16);
}

void
ags_regex_util_test_execute()
{
  AgsRegexUtil *regex_util;

  AgsRegexMatch *match;

  gchar *str;
  
  gint start_match_offset, end_match_offset;
  gboolean success;

  GError *error;
  
  /*  */
  regex_util = ags_regex_util_alloc(NULL,
				    NULL,
				    FALSE, FALSE);

  CU_ASSERT(regex_util != NULL);

  /*  */
  match = ags_regex_util_match_alloc(regex_util,
				     1);

  CU_ASSERT(match != NULL);

  error = NULL;
  success = ags_regex_util_compile(regex_util,
				   "^([a-zA-Z]+[0-9]*)", AGS_REGEX_UTIL_POSIX_EXTENDED_SYNTAX,
				   &error);
  
  CU_ASSERT(success == TRUE);
  
  success = ags_regex_util_execute(regex_util,
				   "abz99 a79", 1,
				   match,
				   0,
				   &error);
  
  CU_ASSERT(match[0].start_match_offset == 0);
  CU_ASSERT(match[0].end_match_offset == 5);
  
  ags_regex_util_free(regex_util);
}
 
void
ags_regex_util_test_execute_unichar()
{
  //TODO:JK: missing
}

void
ags_regex_util_test_execute_unichar2()
{
  //TODO:JK: missing
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
  pSuite = CU_add_suite("AgsRegexUtilTest", ags_regex_util_test_init_suite, ags_regex_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsRegexUtil alloc", ags_regex_util_test_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRegexUtil copy", ags_regex_util_test_copy) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRegexUtil free", ags_regex_util_test_free) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRegexUtil get app encoding", ags_regex_util_test_get_app_encoding) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRegexUtil get encoding", ags_regex_util_test_get_encoding) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRegexUtil compile", ags_regex_util_test_compile) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRegexUtil match alloc", ags_regex_util_test_match_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRegexUtil match copy", ags_regex_util_test_match_copy) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRegexUtil match free", ags_regex_util_test_match_free) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRegexUtil match get offset", ags_regex_util_test_match_get_offset) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRegexUtil execute", ags_regex_util_test_execute) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRegexUtil execute unichar", ags_regex_util_test_execute_unichar) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRegexUtil execute unichar2", ags_regex_util_test_execute_unichar2) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
