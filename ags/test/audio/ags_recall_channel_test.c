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
#include <ags/libags-audio.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_recall_channel_test_init_suite();
int ags_recall_channel_test_clean_suite();

void ags_recall_channel_test_find_channel();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recall_channel_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recall_channel_test_clean_suite()
{  
  return(0);
}

void
ags_recall_channel_test_find_channel()
{
  AgsChannel *source[3];
  AgsRecallChannel *recall_channel[3];
  
  GList *list, *current;

  list = NULL;

  /* recall channel #0 */
  source[0] = g_object_new(AGS_TYPE_CHANNEL,
			   NULL);
  
  recall_channel[0] = g_object_new(AGS_TYPE_RECALL_CHANNEL,
				   "source", source[0],
				   NULL);
  list = g_list_prepend(list,
			recall_channel[0]);
  
  /* recall channel #1 */
  source[1] = g_object_new(AGS_TYPE_CHANNEL,
			   NULL);
  
  recall_channel[1] = g_object_new(AGS_TYPE_RECALL_CHANNEL,
				   "source", source[1],
				   NULL);
  list = g_list_prepend(list,
			recall_channel[1]);

  /* recall channel #2 */
  source[2] = g_object_new(AGS_TYPE_CHANNEL,
			   NULL);
  
  recall_channel[2] = g_object_new(AGS_TYPE_RECALL_CHANNEL,
				   "source", source[2],
				   NULL);
  list = g_list_prepend(list,
			recall_channel[2]);

  /* assert */
  CU_ASSERT((current = ags_recall_channel_find_channel(list,
						       source[0])) != NULL &&
	    current->data == recall_channel[0]);

  CU_ASSERT((current = ags_recall_channel_find_channel(list,
						       source[1])) != NULL &&
	    current->data == recall_channel[1]);

  CU_ASSERT((current = ags_recall_channel_find_channel(list,
						       source[2])) != NULL &&
	    current->data == recall_channel[2]);
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
  pSuite = CU_add_suite("AgsRecallChannelTest", ags_recall_channel_test_init_suite, ags_recall_channel_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsRecallChannel find channel", ags_recall_channel_test_find_channel) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

