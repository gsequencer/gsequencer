/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2018 Joël Krähemann
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
#include <ags/libags-audio.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <stdlib.h>

int ags_recall_id_test_init_suite();
int ags_recall_id_test_clean_suite();

void ags_recall_id_test_find_recycling_context();
void ags_recall_id_test_find_parent_recycling_context();

#define AGS_RECALL_ID_TEST_FIND_RECYCLING_CONTEXT_N_RECALL_ID (16)

#define AGS_RECALL_ID_TEST_FIND_PARENT_RECYCLING_CONTEXT_N_RECALL_ID (16)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recall_id_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recall_id_test_clean_suite()
{  
  return(0);
}

void
ags_recall_id_test_find_recycling_context()
{
  AgsRecallID **recall_id;
  AgsRecallID *current;
  AgsRecyclingContext **recycling_context;
  
  GList *list;

  guint i;
  gboolean success;
  
  recall_id = (AgsRecallID **) malloc(AGS_RECALL_ID_TEST_FIND_RECYCLING_CONTEXT_N_RECALL_ID * sizeof(AgsRecallID*));
  recycling_context = (AgsRecallID **) malloc(AGS_RECALL_ID_TEST_FIND_RECYCLING_CONTEXT_N_RECALL_ID * sizeof(AgsRecyclingContext*));

  list = NULL;
  
  for(i = 0; i < AGS_RECALL_ID_TEST_FIND_RECYCLING_CONTEXT_N_RECALL_ID; i++){
    recycling_context[i] = ags_recycling_context_new(0);

    recall_id[i] = ags_recall_id_new();
    g_object_set(recall_id[i],
		 "recycling-context", recycling_context[i],
		 NULL);

    list = g_list_prepend(list,
			  recall_id[i]);    
  }

  list = g_list_reverse(list);
  
  /* test */
  success = TRUE;

  for(i = 0; i < AGS_RECALL_ID_TEST_FIND_RECYCLING_CONTEXT_N_RECALL_ID; i++){
    current = ags_recall_id_find_recycling_context(list,
						   recycling_context[i]);

    if(current != recall_id[i]){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success);
}

void
ags_recall_id_test_find_parent_recycling_context()
{
  AgsRecallID **recall_id;
  AgsRecallID *current;
  AgsRecyclingContext **recycling_context;
  AgsRecyclingContext **parent_recycling_context;
  
  GList *list;

  guint i;
  gboolean success;
  
  recall_id = (AgsRecallID **) malloc(AGS_RECALL_ID_TEST_FIND_PARENT_RECYCLING_CONTEXT_N_RECALL_ID * sizeof(AgsRecallID*));
  recycling_context = (AgsRecallID **) malloc(AGS_RECALL_ID_TEST_FIND_PARENT_RECYCLING_CONTEXT_N_RECALL_ID * sizeof(AgsRecyclingContext*));
  parent_recycling_context = (AgsRecallID **) malloc(AGS_RECALL_ID_TEST_FIND_PARENT_RECYCLING_CONTEXT_N_RECALL_ID * sizeof(AgsRecyclingContext*));

  list = NULL;
  
  for(i = 0; i < AGS_RECALL_ID_TEST_FIND_PARENT_RECYCLING_CONTEXT_N_RECALL_ID; i++){
    parent_recycling_context[i] = ags_recycling_context_new(0);

    recycling_context[i] = ags_recycling_context_new(0);
    g_object_set(recycling_context[i],
		 "parent", parent_recycling_context[i],
		 NULL);
    
    recall_id[i] = ags_recall_id_new();
    g_object_set(recall_id[i],
		 "recycling-context", recycling_context[i],
		 NULL);

    list = g_list_prepend(list,
			  recall_id[i]);    
  }

  list = g_list_reverse(list);

  /* test */
  success = TRUE;

  for(i = 0; i < AGS_RECALL_ID_TEST_FIND_PARENT_RECYCLING_CONTEXT_N_RECALL_ID; i++){
    current = ags_recall_id_find_parent_recycling_context(list,
							  parent_recycling_context[i]);

    if(current != recall_id[i]){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success);
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
  pSuite = CU_add_suite("AgsRecallIDTest", ags_recall_id_test_init_suite, ags_recall_id_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsRecallID find reycling context", ags_recall_id_test_find_recycling_context) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecallID find parent reycling context", ags_recall_id_test_find_parent_recycling_context) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
