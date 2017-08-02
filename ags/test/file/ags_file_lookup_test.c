/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2017 Joël Krähemann
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

int ags_file_lookup_test_init_suite();
int ags_file_lookup_test_clean_suite();

void ags_file_lookup_test_find_by_node();
void ags_file_lookup_test_find_by_reference();
void ags_file_lookup_test_resolve();

void ags_file_lookup_test_stub_resolve(AgsFileLookup *file_lookup);

gboolean stub_resolve;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_file_lookup_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_file_lookup_test_clean_suite()
{
  return(0);
}

void
ags_file_lookup_test_find_by_node()
{
  AgsFileLookup *file_lookup[3];

  xmlNode *node[3];
  
  GList *list;

  list = NULL;

  /* file lookup #0 */
  node[0] = xmlNewNode(NULL,
		       "ags-test");
  
  file_lookup[0] = g_object_new(AGS_TYPE_FILE_LOOKUP,
				"node", node[0],
				NULL);
  list = g_list_prepend(list,
			file_lookup[0]);

  /* file lookup #1 */
  node[1] = xmlNewNode(NULL,
		       "ags-test");
  
  file_lookup[1] = g_object_new(AGS_TYPE_FILE_LOOKUP,
				"node", node[1],
				NULL);
  list = g_list_prepend(list,
			file_lookup[1]);

  /* file lookup #2 */
  node[2] = xmlNewNode(NULL,
		       "ags-test");
  
  file_lookup[2] = g_object_new(AGS_TYPE_FILE_LOOKUP,
				"node", node[2],
				NULL);
  list = g_list_prepend(list,
			file_lookup[2]);

  /* assert find by node */
  CU_ASSERT(ags_file_lookup_find_by_node(list,
					 node[0]) != NULL);

  CU_ASSERT(ags_file_lookup_find_by_node(list,
					 node[1]) != NULL);

  CU_ASSERT(ags_file_lookup_find_by_node(list,
					 node[2]) != NULL);
}

void
ags_file_lookup_test_find_by_reference()
{
  AgsFileLookup *file_lookup[3];

  AgsTurtle *turtle[3];
  
  GList *list;

  list = NULL;

  /* file lookup #0 */
  turtle[0] = g_object_new(AGS_TYPE_TURTLE,
			   NULL);
  
  file_lookup[0] = g_object_new(AGS_TYPE_FILE_LOOKUP,
				"reference", turtle[0],
				NULL);
  list = g_list_prepend(list,
			file_lookup[0]);

  /* file lookup #1 */
  turtle[1] = g_object_new(AGS_TYPE_TURTLE,
			   NULL);
  
  file_lookup[1] = g_object_new(AGS_TYPE_FILE_LOOKUP,
				"reference", turtle[1],
				NULL);
  list = g_list_prepend(list,
			file_lookup[1]);

  /* file lookup #2 */
  turtle[2] = g_object_new(AGS_TYPE_TURTLE,
			   NULL);
  
  file_lookup[2] = g_object_new(AGS_TYPE_FILE_LOOKUP,
				"reference", turtle[2],
				NULL);
  list = g_list_prepend(list,
			file_lookup[2]);

  /* assert find by reference */
  CU_ASSERT(ags_file_lookup_find_by_reference(list,
					      turtle[0]) != NULL);

  CU_ASSERT(ags_file_lookup_find_by_reference(list,
					      turtle[1]) != NULL);

  CU_ASSERT(ags_file_lookup_find_by_reference(list,
					      turtle[2]) != NULL);
}

void
ags_file_lookup_test_resolve()
{
  AgsFileLookup *file_lookup;

  gpointer ptr;
  
  file_lookup = g_object_new(AGS_TYPE_FILE_LOOKUP,
			     NULL);

  ptr = AGS_FILE_LOOKUP_GET_CLASS(file_lookup)->resolve;
  AGS_FILE_LOOKUP_GET_CLASS(file_lookup)->resolve = ags_file_lookup_test_stub_resolve;

  /* assert resolve */
  ags_file_lookup_resolve(file_lookup);

  CU_ASSERT(stub_resolve == TRUE);
  AGS_FILE_LOOKUP_GET_CLASS(file_lookup)->resolve = ptr;
}

void
ags_file_lookup_test_stub_resolve(AgsFileLookup *file_lookup)
{
  stub_resolve = TRUE;
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
  pSuite = CU_add_suite("AgsFileLookupTest", ags_file_lookup_test_init_suite, ags_file_lookup_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsFileLookup find by node", ags_file_lookup_test_find_by_node) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFileLookup find by reference", ags_file_lookup_test_find_by_reference) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFileLookup resolve", ags_file_lookup_test_resolve) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
