/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

int ags_playback_test_init_suite();
int ags_playback_test_clean_suite();

void ags_playback_test_set_channel_thread();
void ags_playback_test_get_channel_thread();
void ags_playback_test_set_iterator_thread();
void ags_playback_test_get_iterator_thread();
void ags_playback_test_set_recycling_thread();
void ags_playback_test_get_recycling_thread();
void ags_playback_test_set_recall_id();
void ags_playback_test_get_recall_id();
void ags_playback_test_find_source();

#define AGS_PLAYBACK_TEST_CONFIG "[generic]\n"	\
  "autosave-thread=false\n"			\
  "simple-file=true\n"				\
  "disable-feature=experimental\n"		\
  "segmentation=4/4\n"				\
  "\n"						\
  "[thread]\n"					\
  "model=super-threaded\n"			\
  "super-threaded-scope=recycling\n"		\
  "lock-global=ags-thread\n"			\
  "lock-parent=ags-recycling-thread\n"		\
  "\n"						\
  "[recall]\n"					\
  "auto-sense=true\n"				\
  "\n"


/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_playback_test_init_suite()
{
  AgsConfig *config;

  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_PLAYBACK_TEST_CONFIG,
			    strlen(AGS_PLAYBACK_TEST_CONFIG));

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_playback_test_clean_suite()
{
  return(0);
}

void
ags_playback_test_set_channel_thread()
{
  AgsPlayback *playback;
  
  playback = g_object_new(AGS_TYPE_PLAYBACK,
			  NULL);

  /* set playback to NULL and assert */
  ags_playback_set_channel_thread(playback,
				  NULL,
				  AGS_PLAYBACK_SCOPE_PLAYBACK);
  CU_ASSERT(playback->channel_thread[AGS_PLAYBACK_SCOPE_PLAYBACK] == NULL);
    
  /* set sequencer to NULL and assert */
  ags_playback_set_channel_thread(playback,
				  NULL,
				  AGS_PLAYBACK_SCOPE_SEQUENCER);
  CU_ASSERT(playback->channel_thread[AGS_PLAYBACK_SCOPE_SEQUENCER] == NULL);

  /* set notation to NULL and assert */
  ags_playback_set_channel_thread(playback,
				  NULL,
				  AGS_PLAYBACK_SCOPE_NOTATION);
  CU_ASSERT(playback->channel_thread[AGS_PLAYBACK_SCOPE_NOTATION] == NULL);
}

void
ags_playback_test_get_channel_thread()
{
  AgsPlayback *playback;

  AgsThread *thread;
  
  playback = g_object_new(AGS_TYPE_PLAYBACK,
			  NULL);

  /* get playback to NULL and assert */
  thread = ags_playback_get_channel_thread(playback,
					   AGS_PLAYBACK_SCOPE_PLAYBACK);
  CU_ASSERT(playback->channel_thread[AGS_PLAYBACK_SCOPE_PLAYBACK] == thread);
  
  /* get sequencer to NULL and assert */
  thread = ags_playback_get_channel_thread(playback,
					   AGS_PLAYBACK_SCOPE_SEQUENCER);
  CU_ASSERT(playback->channel_thread[AGS_PLAYBACK_SCOPE_SEQUENCER] == thread);

  /* get notation to NULL and assert */
  thread = ags_playback_get_channel_thread(playback,
					   AGS_PLAYBACK_SCOPE_NOTATION);
  CU_ASSERT(playback->channel_thread[AGS_PLAYBACK_SCOPE_NOTATION] == thread);
}

void
ags_playback_test_set_iterator_thread()
{
  AgsPlayback *playback;
  
  playback = g_object_new(AGS_TYPE_PLAYBACK,
			  NULL);

  /* set playback to NULL and assert */
  ags_playback_set_iterator_thread(playback,
				   NULL,
				   AGS_PLAYBACK_SCOPE_PLAYBACK);
  CU_ASSERT(playback->iterator_thread[AGS_PLAYBACK_SCOPE_PLAYBACK] == NULL);
    
  /* set sequencer to NULL and assert */
  ags_playback_set_iterator_thread(playback,
				   NULL,
				   AGS_PLAYBACK_SCOPE_SEQUENCER);
  CU_ASSERT(playback->iterator_thread[AGS_PLAYBACK_SCOPE_SEQUENCER] == NULL);

  /* set notation to NULL and assert */
  ags_playback_set_iterator_thread(playback,
				   NULL,
				   AGS_PLAYBACK_SCOPE_NOTATION);
  CU_ASSERT(playback->iterator_thread[AGS_PLAYBACK_SCOPE_NOTATION] == NULL);
}

void
ags_playback_test_get_iterator_thread()
{
  AgsPlayback *playback;

  AgsThread *thread;
  
  playback = g_object_new(AGS_TYPE_PLAYBACK,
			  NULL);

  /* get playback to NULL and assert */
  thread = ags_playback_get_iterator_thread(playback,
					    AGS_PLAYBACK_SCOPE_PLAYBACK);
  CU_ASSERT(playback->iterator_thread[AGS_PLAYBACK_SCOPE_PLAYBACK] == thread);
  
  /* get sequencer to NULL and assert */
  thread = ags_playback_get_iterator_thread(playback,
					    AGS_PLAYBACK_SCOPE_SEQUENCER);
  CU_ASSERT(playback->iterator_thread[AGS_PLAYBACK_SCOPE_SEQUENCER] == thread);

  /* get notation to NULL and assert */
  thread = ags_playback_get_iterator_thread(playback,
					    AGS_PLAYBACK_SCOPE_NOTATION);
  CU_ASSERT(playback->iterator_thread[AGS_PLAYBACK_SCOPE_NOTATION] == thread);
}

void
ags_playback_test_set_recycling_thread()
{
  AgsPlayback *playback;
  
  playback = g_object_new(AGS_TYPE_PLAYBACK,
			  NULL);

  /* set playback to NULL and assert */
  ags_playback_set_recycling_thread(playback,
				    NULL,
				    AGS_PLAYBACK_SCOPE_PLAYBACK);
  CU_ASSERT(playback->recycling_thread[AGS_PLAYBACK_SCOPE_PLAYBACK] == NULL);
    
  /* set sequencer to NULL and assert */
  ags_playback_set_recycling_thread(playback,
				    NULL,
				    AGS_PLAYBACK_SCOPE_SEQUENCER);
  CU_ASSERT(playback->recycling_thread[AGS_PLAYBACK_SCOPE_SEQUENCER] == NULL);

  /* set notation to NULL and assert */
  ags_playback_set_recycling_thread(playback,
				    NULL,
				    AGS_PLAYBACK_SCOPE_NOTATION);
  CU_ASSERT(playback->recycling_thread[AGS_PLAYBACK_SCOPE_NOTATION] == NULL);
}

void
ags_playback_test_get_recycling_thread()
{
  AgsPlayback *playback;

  AgsThread *thread;
  
  playback = g_object_new(AGS_TYPE_PLAYBACK,
			  NULL);

  /* get playback to NULL and assert */
  thread = ags_playback_get_recycling_thread(playback,
					     AGS_PLAYBACK_SCOPE_PLAYBACK);
  CU_ASSERT(playback->recycling_thread[AGS_PLAYBACK_SCOPE_PLAYBACK] == thread);
  
  /* get sequencer to NULL and assert */
  thread = ags_playback_get_recycling_thread(playback,
					     AGS_PLAYBACK_SCOPE_SEQUENCER);
  CU_ASSERT(playback->recycling_thread[AGS_PLAYBACK_SCOPE_SEQUENCER] == thread);

  /* get notation to NULL and assert */
  thread = ags_playback_get_recycling_thread(playback,
					     AGS_PLAYBACK_SCOPE_NOTATION);
  CU_ASSERT(playback->recycling_thread[AGS_PLAYBACK_SCOPE_NOTATION] == thread);
}

void
ags_playback_test_set_recall_id()
{
  AgsPlayback *playback;
  AgsRecallID *recall_id[3];
  
  playback = g_object_new(AGS_TYPE_PLAYBACK,
			  NULL);

  recall_id[0] = g_object_new(AGS_TYPE_RECALL_ID,
			      NULL);

  recall_id[1] = g_object_new(AGS_TYPE_RECALL_ID,
			      NULL);

  recall_id[2] = g_object_new(AGS_TYPE_RECALL_ID,
			      NULL);
  
  /* set playback to NULL and assert */
  ags_playback_set_recall_id(playback,
			     recall_id[0],
			     AGS_PLAYBACK_SCOPE_PLAYBACK);
  CU_ASSERT(playback->recall_id[AGS_PLAYBACK_SCOPE_PLAYBACK] == recall_id[0]);
    
  /* set sequencer to NULL and assert */
  ags_playback_set_recall_id(playback,
			     recall_id[1],
			     AGS_PLAYBACK_SCOPE_SEQUENCER);
  CU_ASSERT(playback->recall_id[AGS_PLAYBACK_SCOPE_SEQUENCER] == recall_id[1]);

  /* set notation to NULL and assert */
  ags_playback_set_recall_id(playback,
			     recall_id[2],
			     AGS_PLAYBACK_SCOPE_NOTATION);
  CU_ASSERT(playback->recall_id[AGS_PLAYBACK_SCOPE_NOTATION] == recall_id[2]);
}

void
ags_playback_test_get_recall_id()
{
  AgsPlayback *playback;
  AgsRecallID *recall_id[3];
  
  playback = g_object_new(AGS_TYPE_PLAYBACK,
			  NULL);

  recall_id[0] = g_object_new(AGS_TYPE_RECALL_ID,
			      NULL);
  ags_playback_set_recall_id(playback,
			     recall_id[0],
			     AGS_PLAYBACK_SCOPE_PLAYBACK);

  recall_id[1] = g_object_new(AGS_TYPE_RECALL_ID,
			      NULL);
  ags_playback_set_recall_id(playback,
			     recall_id[1],
			     AGS_PLAYBACK_SCOPE_SEQUENCER);

  recall_id[2] = g_object_new(AGS_TYPE_RECALL_ID,
			      NULL);
  ags_playback_set_recall_id(playback,
			     recall_id[2],
			     AGS_PLAYBACK_SCOPE_NOTATION);

  /* assert get recall id */
  CU_ASSERT(ags_playback_get_recall_id(playback,
				       AGS_PLAYBACK_SCOPE_PLAYBACK) == recall_id[0]);

  CU_ASSERT(ags_playback_get_recall_id(playback,
				       AGS_PLAYBACK_SCOPE_SEQUENCER) == recall_id[1]);

  CU_ASSERT(ags_playback_get_recall_id(playback,
				       AGS_PLAYBACK_SCOPE_NOTATION) == recall_id[2]);
}

void
ags_playback_test_find_source()
{
  AgsChannel *channel[3];
  AgsPlayback *playback[3];

  GList *list;

  list = NULL;

  /* playback #0 */
  channel[0] = g_object_new(AGS_TYPE_INPUT,
			    NULL);
  
  playback[0] = g_object_new(AGS_TYPE_PLAYBACK,
			     "source", channel[0],
			     NULL);
  list = g_list_prepend(list,
			playback[0]);

  /* playback #1 */
  channel[1] = g_object_new(AGS_TYPE_INPUT,
			    NULL);
  
  playback[1] = g_object_new(AGS_TYPE_PLAYBACK,
			     "source", channel[1],
			     NULL);
  list = g_list_prepend(list,
			playback[1]);

  /* playback #2 */
  channel[2] = g_object_new(AGS_TYPE_INPUT,
			    NULL);
  
  playback[2] = g_object_new(AGS_TYPE_PLAYBACK,
			     "source", channel[2],
			     NULL);
  list = g_list_prepend(list,
			playback[2]);

  /* assert */
  CU_ASSERT(ags_playback_find_source(list,
				     channel[0]) == playback[0]);

  CU_ASSERT(ags_playback_find_source(list,
				     channel[1]) == playback[1]);

  CU_ASSERT(ags_playback_find_source(list,
				     channel[2]) == playback[2]);
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
  pSuite = CU_add_suite("AgsPlaybackTest", ags_playback_test_init_suite, ags_playback_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsPlayback set channel thread", ags_playback_test_set_channel_thread) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPlayback get channel thread", ags_playback_test_get_channel_thread) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPlayback set iterator thread", ags_playback_test_set_iterator_thread) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPlayback set iterator thread", ags_playback_test_get_iterator_thread) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPlayback set recycling thread", ags_playback_test_set_recycling_thread) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPlayback get recycling thread", ags_playback_test_get_recycling_thread) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPlayback set recall id", ags_playback_test_set_recall_id) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPlayback get recall id", ags_playback_test_get_recall_id) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPlayback get find source", ags_playback_test_find_source) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

