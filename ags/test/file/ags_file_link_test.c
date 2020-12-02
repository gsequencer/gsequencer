/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2015-2020 Joël Krähemann
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

int ags_file_link_test_init_suite();
int ags_file_link_test_clean_suite();

void ags_file_link_test_set_filename();
void ags_file_link_test_get_filename();
void ags_file_link_test_set_data();
void ags_file_link_test_get_data();

#define AGS_FILE_LINK_TEST_FILENAME "/home/test/ags-test-file.xml"
#define AGS_FILE_LINK_TEST_DATA "<ags-simple-file><ags-sf-config><![CDATA[[generic]<![CDATA[[generic][thread]\nmodel=super-threaded\nmax-precision=100\n]]</ags-sf-config></ags-simple-file>"

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_file_link_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_file_link_test_clean_suite()
{
  return(0);
}

void
ags_file_link_test_set_filename()
{
  AgsFileLink *file_link;

  file_link = ags_file_link_new();

  CU_ASSERT(file_link->filename == NULL);
  
  ags_file_link_set_filename(file_link,
			     AGS_FILE_LINK_TEST_FILENAME);

  CU_ASSERT(!g_strcmp0(file_link->filename,
		       AGS_FILE_LINK_TEST_FILENAME));
}

void
ags_file_link_test_get_filename()
{
  AgsFileLink *file_link;

  file_link = ags_file_link_new();

  file_link->filename = g_strdup(AGS_FILE_LINK_TEST_FILENAME);
  
  CU_ASSERT(!g_strcmp0(ags_file_link_get_filename(file_link),
		       AGS_FILE_LINK_TEST_FILENAME));
}

void
ags_file_link_test_set_data()
{
  AgsFileLink *file_link;

  file_link = ags_file_link_new();

  CU_ASSERT(file_link->data == NULL);
  
  ags_file_link_set_data(file_link,
			     AGS_FILE_LINK_TEST_DATA);

  CU_ASSERT(!g_strcmp0(file_link->data,
		       AGS_FILE_LINK_TEST_DATA));
}

void
ags_file_link_test_get_data()
{
  AgsFileLink *file_link;

  file_link = ags_file_link_new();

  file_link->data = g_strdup(AGS_FILE_LINK_TEST_DATA);
  
  CU_ASSERT(!g_strcmp0(ags_file_link_get_data(file_link),
		       AGS_FILE_LINK_TEST_DATA));
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
  pSuite = CU_add_suite("AgsFileLinkTest", ags_file_link_test_init_suite, ags_file_link_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsFileLink set filename", ags_file_link_test_set_filename) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFileLink get filename", ags_file_link_test_get_filename) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFileLink set data", ags_file_link_test_set_data) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFileLink get data", ags_file_link_test_get_data) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
