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

int ags_recall_container_test_init_suite();
int ags_recall_container_test_clean_suite();

void ags_recall_container_test_get_recall_audio();
void ags_recall_container_test_get_recall_audio_run();
void ags_recall_container_test_get_recall_channel();
void ags_recall_container_test_get_recall_channel_run();
void ags_recall_container_test_find();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recall_container_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recall_container_test_clean_suite()
{  
  return(0);
}

void
ags_recall_container_test_get_recall_audio()
{
  AgsRecall *recall, *current;
  AgsRecallContainer *recall_container;

  recall = g_object_new(AGS_TYPE_RECALL_AUDIO,
			NULL);
  
  recall_container = g_object_new(AGS_TYPE_RECALL_CONTAINER,
				  "recall-audio", recall,
				  NULL);

  /* get recall audio and assert*/
  current = ags_recall_container_get_recall_audio(recall_container);

  CU_ASSERT(current != NULL &&
	    AGS_IS_RECALL_AUDIO(current) == TRUE);
}

void
ags_recall_container_test_get_recall_audio_run()
{
  AgsRecall *recall[3];
  AgsRecallContainer *recall_container;

  GList *list, *current;
  
  recall_container = g_object_new(AGS_TYPE_RECALL_CONTAINER,
				  NULL);

  recall[0] = g_object_new(AGS_TYPE_RECALL_AUDIO_RUN,
			NULL);
  g_object_set(recall_container,
	       "recall-audio-run", recall[0],
	       NULL);

  recall[1] = g_object_new(AGS_TYPE_RECALL_AUDIO_RUN,
			NULL);
  g_object_set(recall_container,
	       "recall-audio-run", recall[1],
	       NULL);
  
  recall[2] = g_object_new(AGS_TYPE_RECALL_AUDIO_RUN,
			NULL);
  g_object_set(recall_container,
	       "recall-audio-run", recall[2],
	       NULL);

  /* get recall audio and assert*/
  list = ags_recall_container_get_recall_audio_run(recall_container);

  CU_ASSERT(list != NULL);
  
  CU_ASSERT(g_list_find(list,
			recall[0]) != NULL);

  CU_ASSERT(g_list_find(list,
			recall[1]) != NULL);

  CU_ASSERT(g_list_find(list,
			recall[2]) != NULL);
}

void
ags_recall_container_test_get_recall_channel()
{
  AgsRecall *recall[3];
  AgsRecallContainer *recall_container;

  GList *list, *current;
  
  recall_container = g_object_new(AGS_TYPE_RECALL_CONTAINER,
				  NULL);

  recall[0] = g_object_new(AGS_TYPE_RECALL_CHANNEL,
			NULL);
  g_object_set(recall_container,
	       "recall-channel", recall[0],
	       NULL);

  recall[1] = g_object_new(AGS_TYPE_RECALL_CHANNEL,
			NULL);
  g_object_set(recall_container,
	       "recall-channel", recall[1],
	       NULL);
  
  recall[2] = g_object_new(AGS_TYPE_RECALL_CHANNEL,
			NULL);
  g_object_set(recall_container,
	       "recall-channel", recall[2],
	       NULL);

  /* get recall channel and assert*/
  list = ags_recall_container_get_recall_channel(recall_container);

  CU_ASSERT(list != NULL);
  
  CU_ASSERT(g_list_find(list,
			recall[0]) != NULL);

  CU_ASSERT(g_list_find(list,
			recall[1]) != NULL);

  CU_ASSERT(g_list_find(list,
			recall[2]) != NULL);
}

void
ags_recall_container_test_get_recall_channel_run()
{
  AgsRecall *recall[3];
  AgsRecallContainer *recall_container;

  GList *list, *current;
  
  recall_container = g_object_new(AGS_TYPE_RECALL_CONTAINER,
				  NULL);

  recall[0] = g_object_new(AGS_TYPE_RECALL_CHANNEL_RUN,
			NULL);
  g_object_set(recall_container,
	       "recall-channel-run", recall[0],
	       NULL);

  recall[1] = g_object_new(AGS_TYPE_RECALL_CHANNEL_RUN,
			NULL);
  g_object_set(recall_container,
	       "recall-channel-run", recall[1],
	       NULL);
  
  recall[2] = g_object_new(AGS_TYPE_RECALL_CHANNEL_RUN,
			NULL);
  g_object_set(recall_container,
	       "recall-channel-run", recall[2],
	       NULL);

  /* get recall channel and assert*/
  list = ags_recall_container_get_recall_channel_run(recall_container);

  CU_ASSERT(list != NULL);
  
  CU_ASSERT(g_list_find(list,
			recall[0]) != NULL);

  CU_ASSERT(g_list_find(list,
			recall[1]) != NULL);

  CU_ASSERT(g_list_find(list,
			recall[2]) != NULL);
}

void
ags_recall_container_test_find()
{
  AgsRecall *recall[6];
  AgsRecallContainer *recall_container;

  GList *list, *current;
  
  /* assert #0 - type */
  list = NULL;

  /* ags-count-beats */
  recall_container = g_object_new(AGS_TYPE_RECALL_CONTAINER,
				  NULL);
  list = g_list_prepend(list,
			recall_container);
  
  recall[0] = g_object_new(AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
			   NULL);
  g_object_set(recall_container,
	       "recall-audio-run", recall[0],
	       NULL);

  /* ags-play */
  recall_container = g_object_new(AGS_TYPE_RECALL_CONTAINER,
				  NULL);
  list = g_list_prepend(list,
			recall_container);

  recall[1] = g_object_new(AGS_TYPE_PLAY_CHANNEL_RUN,
			   NULL);
  g_object_set(recall_container,
	       "recall-channel-run", recall[1],
	       NULL);

  /* ags-copy-pattern */
  recall_container = g_object_new(AGS_TYPE_RECALL_CONTAINER,
				  NULL);
  list = g_list_prepend(list,
			recall_container);
  
  recall[2] = g_object_new(AGS_TYPE_COPY_PATTERN_AUDIO_RUN,
			NULL);
  g_object_set(recall_container,
	       "recall-audio-run", recall[2],
	       NULL);

  recall[3] = g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL_RUN,
			NULL);
  g_object_set(recall_container,
	       "recall-channel-run", recall[3],
	       NULL);

  /* ags-buffer */
  recall_container = g_object_new(AGS_TYPE_RECALL_CONTAINER,
				  NULL);
  list = g_list_prepend(list,
			recall_container);

  recall[4] = g_object_new(AGS_TYPE_BUFFER_CHANNEL,
			NULL);
  g_object_set(recall_container,
	       "recall-channel", recall[4],
	       NULL);

  /* ags-delay */
  recall_container = g_object_new(AGS_TYPE_RECALL_CONTAINER,
				  NULL);
  list = g_list_prepend(list,
			recall_container);

  recall[5] = g_object_new(AGS_TYPE_DELAY_AUDIO,
			NULL);
  g_object_set(recall_container,
	       "recall-audio", recall[5],
	       NULL);

  /* find and assert ags-count-beats */
  current = ags_recall_container_find(list,
				      AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
				      AGS_RECALL_CONTAINER_FIND_TYPE,
				      NULL);

  CU_ASSERT(current != NULL &&
	    AGS_IS_RECALL_CONTAINER(current->data) == TRUE);
  CU_ASSERT(g_list_find(AGS_RECALL_CONTAINER(current->data)->recall_audio_run, recall[0]) != NULL);
  
  /* find and assert ags-play */
  current = ags_recall_container_find(list,
				      AGS_TYPE_PLAY_CHANNEL_RUN,
				      AGS_RECALL_CONTAINER_FIND_TYPE,
				      NULL);

  CU_ASSERT(current != NULL &&
	    AGS_IS_RECALL_CONTAINER(current->data) == TRUE);
  CU_ASSERT(g_list_find(AGS_RECALL_CONTAINER(current->data)->recall_channel_run, recall[1]) != NULL);

  /* find and assert ags-copy-pattern */
  current = ags_recall_container_find(list,
				      AGS_TYPE_COPY_PATTERN_AUDIO_RUN,
				      AGS_RECALL_CONTAINER_FIND_TYPE,
				      NULL);

  CU_ASSERT(current != NULL &&
	    AGS_IS_RECALL_CONTAINER(current->data) == TRUE &&
	    g_list_find(AGS_RECALL_CONTAINER(current->data)->recall_audio_run,
			recall[2]) != NULL);

  current = ags_recall_container_find(list,
				      AGS_TYPE_COPY_PATTERN_CHANNEL_RUN,
				      AGS_RECALL_CONTAINER_FIND_TYPE,
				      NULL);

  CU_ASSERT(current != NULL &&
	    AGS_IS_RECALL_CONTAINER(current->data) == TRUE &&
	    g_list_find(AGS_RECALL_CONTAINER(current->data)->recall_channel_run,
			recall[3]) != NULL);

  /* find and assert ags-buffer */
  current = ags_recall_container_find(list,
				      AGS_TYPE_BUFFER_CHANNEL,
				      AGS_RECALL_CONTAINER_FIND_TYPE,
				      NULL);

  CU_ASSERT(current != NULL &&
	    AGS_IS_RECALL_CONTAINER(current->data) == TRUE &&
	    g_list_find(AGS_RECALL_CONTAINER(current->data)->recall_channel,
			recall[4]) != NULL);
  
  /* find and assert ags-delay */
  current = ags_recall_container_find(list,
				      AGS_TYPE_DELAY_AUDIO,
				      AGS_RECALL_CONTAINER_FIND_TYPE,
				      NULL);

  CU_ASSERT(current != NULL &&
	    AGS_IS_RECALL_CONTAINER(current->data) == TRUE &&
	    AGS_RECALL_CONTAINER(current->data)->recall_audio == recall[5]);
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
  pSuite = CU_add_suite("AgsRecallContainerTest", ags_recall_container_test_init_suite, ags_recall_container_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsRecallContainer get recall audio", ags_recall_container_test_get_recall_audio) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecallContainer get recall audio run", ags_recall_container_test_get_recall_audio_run) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecallContainer get recall channel", ags_recall_container_test_get_recall_channel) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecallContainer get recall channel run", ags_recall_container_test_get_recall_channel_run) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecallContainer find", ags_recall_container_test_find) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

