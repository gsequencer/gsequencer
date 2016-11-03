/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recycling_context.h>

int ags_channel_test_init_suite();
int ags_channel_test_clean_suite();

void ags_channel_test_add_recall();
void ags_channel_test_add_recall_container();
void ags_channel_test_add_recall_id();
void ags_channel_test_duplicate_recall();
void ags_channel_test_init_recall();
void ags_channel_test_resolve_recall();

void ags_channel_test_run_init_pre_recall_callback(AgsRecall *recall,
						   gpointer data);
void ags_channel_test_resolve_recall_callback(AgsRecall *recall,
					      gpointer data);

guint test_init_recall_callback_hits_count = 0;
guint test_resolve_recall_callback_hits_count = 0;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_channel_test_init_suite()
{
  /* empty */
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_channel_test_clean_suite()
{
  /* empty */
  
  return(0);
}

void
ags_channel_test_add_recall()
{
  AgsChannel *channel;
  AgsRecall *recall0, *recall1;

  /* instantiate channel */
  channel = ags_channel_new(NULL);

  /* instantiate recall */
  recall0 = ags_recall_new();

  /* add recall to channel */
  ags_channel_add_recall(channel,
			 recall0,
			 TRUE);

  /* assert to be in channel->play */
  CU_ASSERT(g_list_find(channel->play,
			recall0) != NULL);

  /* instantiate recall */
  recall1 = ags_recall_new();

  /* add recall to channel */
  ags_channel_add_recall(channel,
			 recall1,
			 TRUE);

  /* assert to be in channel->recall */
  CU_ASSERT(g_list_find(channel->play,
			recall0) != NULL);
  CU_ASSERT(g_list_find(channel->play,
			recall1) != NULL);

  /* instantiate recall */
  recall0 = ags_recall_new();

  /* add recall to channel */
  ags_channel_add_recall(channel,
			 recall0,
			 FALSE);

  /* assert to be in channel->play */
  CU_ASSERT(g_list_find(channel->recall,
			recall0) != NULL);

  /* instantiate recall */
  recall1 = ags_recall_new();

  /* add recall to channel */
  ags_channel_add_recall(channel,
			 recall1,
			 FALSE);

  /* assert to be in channel->recall */
  CU_ASSERT(g_list_find(channel->recall,
			recall0) != NULL);
  CU_ASSERT(g_list_find(channel->recall,
			recall1) != NULL);
}

void
ags_channel_test_add_recall_container()
{
  AgsChannel *channel;
  AgsRecallContainer *recall_container0, *recall_container1;
  
  /* instantiate channel */
  channel = ags_channel_new(NULL);

  /* instantiate recall */
  recall_container0 = ags_recall_container_new(NULL);
  ags_channel_add_recall_container(channel,
				   recall_container0);
  
  /* assert to be in channel->recall_container */
  CU_ASSERT(g_list_find(channel->container,
			recall_container0) != NULL);

  /* instantiate recall */
  recall_container1 = ags_recall_container_new(NULL);
  ags_channel_add_recall_container(channel,
				   recall_container1);
  
  /* assert to be in channel->recall_container */
  CU_ASSERT(g_list_find(channel->container,
			recall_container0) != NULL);
  CU_ASSERT(g_list_find(channel->container,
			recall_container1) != NULL);
}

void
ags_channel_test_add_recall_id()
{
  AgsChannel *channel;
  AgsRecallID *recall_id0, *recall_id1;
  
  /* instantiate channel */
  channel = ags_channel_new(NULL);

  /* instantiate recall */
  recall_id0 = ags_recall_id_new(NULL);
  ags_channel_add_recall_id(channel,
			    recall_id0);
  
  /* assert to be in channel->recall_id */
  CU_ASSERT(g_list_find(channel->recall_id,
			recall_id0) != NULL);

  /* instantiate recall */
  recall_id1 = ags_recall_id_new(NULL);
  ags_channel_add_recall_id(channel,
			    recall_id1);
  
  /* assert to be in channel->recall_id */
  CU_ASSERT(g_list_find(channel->recall_id,
			recall_id0) != NULL);
  CU_ASSERT(g_list_find(channel->recall_id,
			recall_id1) != NULL);
}

void
ags_channel_test_duplicate_recall()
{
  AgsChannel *channel;
  AgsRecall *recall;
  AgsRecall *recall_channel_run;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;
  AgsRecallID *recall_id;
  
  /* instantiate channel */
  channel = ags_channel_new(NULL);

  /* case 1: playback recall */
  recall = ags_recall_new();
  recall->flags |= AGS_RECALL_TEMPLATE;
  ags_channel_add_recall(channel,
			 recall,
			 TRUE);
  
  recall_channel_run = ags_recall_channel_run_new();
  recall_channel_run->flags |= AGS_RECALL_TEMPLATE;
  ags_channel_add_recall(channel,
			 recall_channel_run,
			 TRUE);

  /* assert inital count */
  CU_ASSERT(g_list_length(channel->play) == 2);
  CU_ASSERT(g_list_length(channel->recall) == 0);

  /* instantiate recycling context and recall id */
  recycling_context = ags_recycling_context_new(0);

  recall_id = ags_recall_id_new(NULL);
  g_object_set(recall_id,
	       "recycling-context\0", recycling_context,
	       NULL);

  /* duplicate recall */
  ags_channel_duplicate_recall(channel,
			       recall_id);

  CU_ASSERT(g_list_length(channel->play) == 4);
  CU_ASSERT(g_list_length(channel->recall) == 0);
  
  /* case 2: true recall */
  recall = ags_recall_new();
  recall->flags |= AGS_RECALL_TEMPLATE;
  ags_channel_add_recall(channel,
			 recall,
			 FALSE);
  
  recall_channel_run = ags_recall_channel_run_new();
  recall_channel_run->flags |= AGS_RECALL_TEMPLATE;
  ags_channel_add_recall(channel,
			 recall_channel_run,
			 FALSE);

  /* assert inital count */
  CU_ASSERT(g_list_length(channel->play) == 4);
  CU_ASSERT(g_list_length(channel->recall) == 2);
  
  /* instantiate recycling context and recall id */
  parent_recycling_context = ags_recycling_context_new(0);
  
  recycling_context = ags_recycling_context_new(0);
  g_object_set(recycling_context,
	       "parent\0", parent_recycling_context,
	       NULL);

  recall_id = ags_recall_id_new(NULL);
  g_object_set(recall_id,
	       "recycling-context\0", recycling_context,
	       NULL);

  /* duplicate recall */
  ags_channel_duplicate_recall(channel,
			       recall_id);

  CU_ASSERT(g_list_length(channel->play) == 4);
  CU_ASSERT(g_list_length(channel->recall) == 4);
}

void
ags_channel_test_init_recall()
{
  AgsChannel *channel;
  AgsRecall *recall;
  AgsRecall *recall_channel_run;
  AgsRecyclingContext *recycling_context;
  AgsRecallID *recall_id;

  GList *list;
  
  /* instantiate channel */
  channel = ags_channel_new(NULL);

  /* instantiate recalls */
  recall = ags_recall_new();
  recall->flags |= AGS_RECALL_TEMPLATE;
  ags_channel_add_recall(channel,
			 recall,
			 TRUE);
  
  recall_channel_run = ags_recall_channel_run_new();
  recall_channel_run->flags |= AGS_RECALL_TEMPLATE;
  ags_channel_add_recall(channel,
			 recall_channel_run,
			 TRUE);

  /* instantiate recycling context and recall id */
  recycling_context = ags_recycling_context_new(0);

  recall_id = ags_recall_id_new(NULL);
  g_object_set(recall_id,
	       "recycling-context\0", recycling_context,
	       NULL);
  ags_channel_add_recall_id(channel,
			    recall_id);
  
  /* init recall */
  test_init_recall_callback_hits_count = 0;
  ags_channel_duplicate_recall(channel,
			       recall_id);
  ags_channel_resolve_recall(channel,
			     recall_id);

  list = channel->play;

  while(list != NULL){
    g_signal_connect(G_OBJECT(list->data), "run-init-pre\0",
		     G_CALLBACK(ags_channel_test_run_init_pre_recall_callback), NULL);

    list = list->next;
  }
  
  ags_channel_init_recall(channel, 0,
			  recall_id);

  CU_ASSERT(test_init_recall_callback_hits_count == 2);
}

void
ags_channel_test_resolve_recall()
{
  AgsChannel *channel;
  AgsRecall *master_recall_channel_run;
  AgsRecall *slave_recall_channel_run;
  AgsRecyclingContext *recycling_context;
  AgsRecallID *recall_id;
  
  /* instantiate channel */
  channel = ags_channel_new(NULL);

  /* instantiate recalls */
  slave_recall_channel_run = ags_recall_channel_run_new();
  slave_recall_channel_run->flags |= AGS_RECALL_TEMPLATE;
  ags_channel_add_recall(channel,
			 slave_recall_channel_run,
			 TRUE);

  g_signal_connect(G_OBJECT(slave_recall_channel_run), "resolve-dependencies\0",
		   G_CALLBACK(ags_channel_test_resolve_recall_callback), NULL);

  /* instantiate recycling context and recall id */
  recycling_context = ags_recycling_context_new(0);

  recall_id = ags_recall_id_new(NULL);
  g_object_set(recall_id,
	       "recycling-context\0", recycling_context,
	       NULL);
  
  /* setup recalls */
  g_object_set(slave_recall_channel_run,
	       "recall-id\0", recall_id,
	       NULL);

  /* resolve recall */
  test_resolve_recall_callback_hits_count = 0;
  ags_channel_resolve_recall(channel,
			     recall_id);
  
  CU_ASSERT(test_resolve_recall_callback_hits_count == 1);
}

void
ags_channel_test_run_init_pre_recall_callback(AgsRecall *recall,
					      gpointer data)
{
  test_init_recall_callback_hits_count++;
}

void
ags_channel_test_resolve_recall_callback(AgsRecall *recall,
					 gpointer data)
{
  test_resolve_recall_callback_hits_count++;
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
  pSuite = CU_add_suite("AgsChannelTest\0", ags_channel_test_init_suite, ags_channel_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsChannel add recall\0", ags_channel_test_add_recall) == NULL) ||
     (CU_add_test(pSuite, "test of AgsChannel add recall container\0", ags_channel_test_add_recall_container) == NULL) ||
     (CU_add_test(pSuite, "test of AgsChannel add recall id\0", ags_channel_test_add_recall_id) == NULL) ||
     (CU_add_test(pSuite, "test of AgsChannel add duplicate recall\0", ags_channel_test_duplicate_recall) == NULL) ||
     (CU_add_test(pSuite, "test of AgsChannel add resolve recall\0", ags_channel_test_resolve_recall) == NULL) ||
     (CU_add_test(pSuite, "test of AgsChannel add init recall\0", ags_channel_test_init_recall) == NULL)){
      CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

