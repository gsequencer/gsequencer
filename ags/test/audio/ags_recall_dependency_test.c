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

int ags_recall_dependency_test_init_suite();
int ags_recall_dependency_test_clean_suite();

void ags_recall_dependency_test_find_dependency();
void ags_recall_dependency_test_find_dependency_by_provider();
void ags_recall_dependency_test_resolve();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recall_dependency_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recall_dependency_test_clean_suite()
{  
  return(0);
}

void
ags_recall_dependency_test_find_dependency()
{
  AgsRecall *recall[3];
  AgsRecallDependency *recall_dependency[3];
  
  GList *list, *current;

  list = NULL;
  
  /* dependency #0 */
  recall[0] = g_object_new(AGS_TYPE_RECALL,
			NULL);
  
  recall_dependency[0] = g_object_new(AGS_TYPE_RECALL_DEPENDENCY,
				      "dependency", recall[0],
				      NULL);
  list = g_list_prepend(list,
			recall_dependency[0]);
  
  /* dependency #1 */
  recall[1] = g_object_new(AGS_TYPE_RECALL,
			   NULL);
  
  recall_dependency[1] = g_object_new(AGS_TYPE_RECALL_DEPENDENCY,
				      "dependency", recall[1],
				      NULL);
  list = g_list_prepend(list,
			recall_dependency[1]);

  /* dependency #2 */
  recall[2] = g_object_new(AGS_TYPE_RECALL,
			   NULL);
  
  recall_dependency[2] = g_object_new(AGS_TYPE_RECALL_DEPENDENCY,
				      "dependency", recall[2],
				      NULL);
  list = g_list_prepend(list,
			recall_dependency[2]);

  /* find and assert */
  CU_ASSERT((current = ags_recall_dependency_find_dependency(list,
							     recall[0])) != NULL &&
	    AGS_RECALL_DEPENDENCY(current->data)->dependency == recall[0]);

  CU_ASSERT((current = ags_recall_dependency_find_dependency(list,
							     recall[1])) != NULL &&
	    AGS_RECALL_DEPENDENCY(current->data)->dependency == recall[1]);

  CU_ASSERT((current = ags_recall_dependency_find_dependency(list,
							     recall[2])) != NULL &&
	    AGS_RECALL_DEPENDENCY(current->data)->dependency == recall[2]);
}

void
ags_recall_dependency_test_find_dependency_by_provider()
{
  AgsChannel *channel[3];
  AgsAudio *audio[3];
  AgsRecall *recall;
  AgsRecallAudio *recall_audio;
  AgsRecallDependency *recall_dependency[6];
  
  GList *list, *current;

  list = NULL;

  /* dependency #0 */
  audio[0] = g_object_new(AGS_TYPE_AUDIO,
			  NULL);
  recall_audio = g_object_new(AGS_TYPE_RECALL_AUDIO,
			      "audio", audio[0],
			      NULL);
  recall = g_object_new(AGS_TYPE_RECALL_AUDIO_RUN,
			"recall-audio", recall_audio,
			NULL);
  
  recall_dependency[0] = g_object_new(AGS_TYPE_RECALL_DEPENDENCY,
				      "dependency", recall,
				      NULL);
  list = g_list_prepend(list,
			recall_dependency[0]);
  
  /* dependency #1 */
  channel[0] = g_object_new(AGS_TYPE_CHANNEL,
			    NULL);
  recall = g_object_new(AGS_TYPE_RECALL_CHANNEL_RUN,
			"source", channel[0],
			NULL);
  
  recall_dependency[1] = g_object_new(AGS_TYPE_RECALL_DEPENDENCY,
				      "dependency", recall,
				      NULL);
  list = g_list_prepend(list,
			recall_dependency[1]);

  /* dependency #2 */
  channel[1] = g_object_new(AGS_TYPE_CHANNEL,
			    NULL);
  recall = g_object_new(AGS_TYPE_RECALL_CHANNEL_RUN,
			"source", channel[1],
			NULL);
  
  recall_dependency[2] = g_object_new(AGS_TYPE_RECALL_DEPENDENCY,
				      "dependency", recall,
				      NULL);
  list = g_list_prepend(list,
			recall_dependency[2]);

  /* dependency #3 */
  audio[1] = g_object_new(AGS_TYPE_AUDIO,
			  NULL);
  recall_audio = g_object_new(AGS_TYPE_RECALL_AUDIO,
			      "audio", audio[1],
			      NULL);
  recall = g_object_new(AGS_TYPE_RECALL_AUDIO_RUN,
			"recall-audio", recall_audio,
			NULL);
  
  recall_dependency[3] = g_object_new(AGS_TYPE_RECALL_DEPENDENCY,
				      "dependency", recall,
				      NULL);
  list = g_list_prepend(list,
			recall_dependency[3]);

  /* dependency #4 */
  audio[2] = g_object_new(AGS_TYPE_AUDIO,
			  NULL);
  recall_audio = g_object_new(AGS_TYPE_RECALL_AUDIO,
			      "audio", audio[2],
			      NULL);
  recall = g_object_new(AGS_TYPE_RECALL_AUDIO_RUN,
			"recall-audio", recall_audio,
			NULL);
  
  recall_dependency[4] = g_object_new(AGS_TYPE_RECALL_DEPENDENCY,
				      "dependency", recall,
				      NULL);
  list = g_list_prepend(list,
			recall_dependency[4]);

  /* dependency #5 */
  channel[2] = g_object_new(AGS_TYPE_CHANNEL,
			    NULL);
  recall = g_object_new(AGS_TYPE_RECALL_CHANNEL_RUN,
			"source", channel[2],
			NULL);
  
  recall_dependency[5] = g_object_new(AGS_TYPE_RECALL_DEPENDENCY,
				      "dependency", recall,
				      NULL);
  list = g_list_prepend(list,
			recall_dependency[5]);

  /* find and assert - audio */
  CU_ASSERT((current = ags_recall_dependency_find_dependency_by_provider(list,
									 audio[0])) != NULL &&
	    AGS_RECALL_AUDIO_RUN(AGS_RECALL_DEPENDENCY(current->data)->dependency)->recall_audio->audio == audio[0]);

  CU_ASSERT((current = ags_recall_dependency_find_dependency_by_provider(list,
									 audio[1])) != NULL &&
	    AGS_RECALL_AUDIO_RUN(AGS_RECALL_DEPENDENCY(current->data)->dependency)->recall_audio->audio == audio[1]);

  CU_ASSERT((current = ags_recall_dependency_find_dependency_by_provider(list,
									 audio[2])) != NULL &&
	    AGS_RECALL_AUDIO_RUN(AGS_RECALL_DEPENDENCY(current->data)->dependency)->recall_audio->audio == audio[2]);

  /* find and assert - channel */
  CU_ASSERT((current = ags_recall_dependency_find_dependency_by_provider(list,
									 channel[0])) != NULL &&
	    AGS_RECALL_CHANNEL_RUN(AGS_RECALL_DEPENDENCY(current->data)->dependency)->source == channel[0]);

  CU_ASSERT((current = ags_recall_dependency_find_dependency_by_provider(list,
									 channel[1])) != NULL &&
	    AGS_RECALL_CHANNEL_RUN(AGS_RECALL_DEPENDENCY(current->data)->dependency)->source == channel[1]);

  CU_ASSERT((current = ags_recall_dependency_find_dependency_by_provider(list,
									 channel[2])) != NULL &&
	    AGS_RECALL_CHANNEL_RUN(AGS_RECALL_DEPENDENCY(current->data)->dependency)->source == channel[2]);
}

void
ags_recall_dependency_test_resolve()
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
  pSuite = CU_add_suite("AgsRecallDependencyTest", ags_recall_dependency_test_init_suite, ags_recall_dependency_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsRecallDependency find dependency", ags_recall_dependency_test_find_dependency) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecallDependency find dependency by provider", ags_recall_dependency_test_find_dependency_by_provider) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecallDependency resolve", ags_recall_dependency_test_resolve) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

