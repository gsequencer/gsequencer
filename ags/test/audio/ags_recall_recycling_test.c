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

int ags_recall_recycling_test_init_suite();
int ags_recall_recycling_test_clean_suite();

void ags_recall_recycling_test_source_add_audio_signal();
void ags_recall_recycling_test_source_remove_audio_signal();

void ags_recall_recycling_test_source_remove_audio_signal_done_callback();

#define AGS_RECALL_AUDIO_SIGNAL_TEST_RUN_INTER_AUDIO_SIGNAL_LENGTH (24)

AgsDevout *devout;
AgsAudio *audio;

AgsApplicationContext *audio_application_context;

gboolean remove_audio_signal_done_callback_invoked = FALSE;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recall_recycling_test_init_suite()
{
  audio_application_context = ags_audio_application_context_new();
  g_object_ref(audio_application_context);
  
  ags_application_context_prepare(audio_application_context);
  ags_application_context_setup(audio_application_context);
  
  /* create soundcard */
  devout = g_object_new(AGS_TYPE_DEVOUT,
			NULL);
  g_object_ref(devout);

  /* create audio */
  audio = ags_audio_new(devout);
  g_object_ref(audio);

  ags_audio_set_flags(audio,
		      (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		       AGS_AUDIO_INPUT_HAS_RECYCLING));
  ags_audio_set_ability_flags(audio,
			      AGS_SOUND_ABILITY_PLAYBACK);

  /* create input/output */
  ags_audio_set_audio_channels(audio,
			       1, 0);

  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
 		     1, 0);

  ags_channel_set_ability_flags(audio->output,
				AGS_SOUND_ABILITY_PLAYBACK);

  ags_channel_set_ability_flags(audio->input,
				AGS_SOUND_ABILITY_PLAYBACK);

  ags_connectable_connect(AGS_CONNECTABLE(audio));
  
  ags_connectable_connect(AGS_CONNECTABLE(audio->output));
  ags_connectable_connect(AGS_CONNECTABLE(audio->input));
    
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recall_recycling_test_clean_suite()
{  
  g_object_run_dispose(devout);
  g_object_unref(devout);

  g_object_run_dispose(audio);
  g_object_unref(audio);

  return(0);
}

void
ags_recall_recycling_test_source_add_audio_signal()
{
  AgsChannel *output;
  AgsChannel *input;
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal;
  AgsRecallID *output_audio_recall_id;
  AgsRecallID *input_audio_recall_id;
  AgsRecallID *output_recall_id;
  AgsRecallID *input_recall_id;
  AgsRecyclingContext *output_recycling_context;
  AgsRecyclingContext *input_recycling_context;
  AgsRecallChannel *recall_channel;
  AgsRecallChannelRun *recall_channel_run;
  AgsRecallRecycling *recall;
  AgsRecallContainer *recall_container;
  
  GList *start_list, *list;
  GList *start_recall_recycling, *recall_recycling;
  GList *start_recall_audio_signal, *recall_audio_signal;

  output = audio->output;
  input = audio->input;

  recycling = input->first_recycling;

  /* output - create recall id and recycling context */
  output_audio_recall_id = ags_recall_id_new();
  ags_recall_id_set_sound_scope(output_audio_recall_id,
				AGS_SOUND_SCOPE_PLAYBACK);
  ags_audio_add_recall_id(audio,
			  output_audio_recall_id);

  output_recall_id = ags_recall_id_new();
  ags_recall_id_set_sound_scope(output_recall_id,
				AGS_SOUND_SCOPE_PLAYBACK);
  ags_channel_add_recall_id(output,
			    output_recall_id);
  
  output_recycling_context = ags_recycling_context_new(1);
  ags_recycling_context_replace(output_recycling_context,
				output->first_recycling,
				0);
  ags_audio_add_recycling_context(audio,
				  output_recycling_context);
  
  g_object_set(output_audio_recall_id,
	       "recycling-context", output_recycling_context,
	       NULL);
  g_object_set(output_recall_id,
	       "recycling-context", output_recycling_context,
	       NULL);

  g_object_set(output_recycling_context,
	       "recall-id", output_recall_id,
	       NULL);
  
  /* input - create recall id and recycling context */
  input_audio_recall_id = ags_recall_id_new();
  ags_recall_id_set_sound_scope(input_audio_recall_id,
				AGS_SOUND_SCOPE_PLAYBACK);
  ags_audio_add_recall_id(audio,
			  input_audio_recall_id);

  input_recall_id = ags_recall_id_new();
  ags_recall_id_set_sound_scope(input_recall_id,
				AGS_SOUND_SCOPE_PLAYBACK);
  ags_channel_add_recall_id(input,
			    input_recall_id);
  
  input_recycling_context = ags_recycling_context_new(1);
  ags_recycling_context_replace(input_recycling_context,
				recycling,
				0);
  ags_audio_add_recycling_context(audio,
				  input_recycling_context);
  
  g_object_set(input_audio_recall_id,
	       "recycling-context", input_recycling_context,
	       NULL);
  g_object_set(input_recall_id,
	       "recycling-context", input_recycling_context,
	       NULL);

  g_object_set(output_recycling_context,
	       "child", input_recycling_context,
	       NULL);

  g_object_set(input_recycling_context,
	       "recall-id", input_recall_id,
	       NULL);

  /* create recall */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(input,
				   (GObject *) recall_container);

  recall_channel = ags_recall_channel_new();
  ags_recall_set_flags(recall_channel,
		       AGS_RECALL_TEMPLATE);
  ags_recall_set_ability_flags(recall_channel,
			       (AGS_SOUND_ABILITY_PLAYBACK));
  g_object_set(recall_channel,
	       "source", input,
	       NULL);  
  ags_channel_add_recall(input,
			 recall_channel,
			 FALSE);
  ags_connectable_connect(AGS_CONNECTABLE(recall_channel));
  
  recall_channel_run = ags_recall_channel_run_new();
  ags_recall_set_flags(recall_channel_run,
		       AGS_RECALL_TEMPLATE);
  ags_recall_set_ability_flags(recall_channel_run,
			       (AGS_SOUND_ABILITY_PLAYBACK));
  g_object_set(recall_channel_run,
	       "child-type", AGS_TYPE_RECALL_RECYCLING,
	       "recall-channel", recall_channel,
	       "source", input,
	       "recall-container", recall_container,
	       NULL);  
  ags_channel_add_recall(input,
			 recall_channel_run,
			 FALSE);
  ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run));
  
  recall = ags_recall_recycling_new();
  g_object_set(recall,
	       "child-type", AGS_TYPE_RECALL_AUDIO_SIGNAL,
	       "source", recycling,
	       "recall-container", recall_container,
	       NULL);
  ags_recall_add_child(recall_channel_run,
		       recall);

  /* duplicate recall */
  ags_audio_duplicate_recall(audio,
			     input_audio_recall_id,
			     0, 0,
			     0);
  ags_channel_duplicate_recall(input,
			       input_recall_id);

  /* resolve recall */
  ags_audio_resolve_recall(audio,
			   input_audio_recall_id);
  ags_channel_resolve_recall(input,
			     input_recall_id);
  
  /* init recall */
  ags_audio_init_recall(audio,
			input_audio_recall_id, (AGS_SOUND_STAGING_CHECK_RT_DATA |
						AGS_SOUND_STAGING_RUN_INIT_PRE |
						AGS_SOUND_STAGING_RUN_INIT_INTER |
						AGS_SOUND_STAGING_RUN_INIT_POST));
  
  ags_channel_init_recall(input,
			  input_recall_id, (AGS_SOUND_STAGING_CHECK_RT_DATA |
					    AGS_SOUND_STAGING_RUN_INIT_PRE |
					    AGS_SOUND_STAGING_RUN_INIT_INTER |
					    AGS_SOUND_STAGING_RUN_INIT_POST));

  /* test */
  g_object_get(input,
	       "recall", &start_list,
	       NULL);

  list = ags_recall_find_type_with_recycling_context(start_list,
						     AGS_TYPE_RECALL_CHANNEL_RUN,
						     input_recycling_context);

  g_object_get(list->data,
	       "child", &start_recall_recycling,
	       NULL);
  
  recall_recycling = ags_recall_find_recycling_context(start_recall_recycling,
						       input_recycling_context);
    
  g_object_set(start_recall_recycling->data,
	       "child-type", AGS_TYPE_RECALL_AUDIO_SIGNAL,
	       NULL);
  
  /* create audio signal */  
  audio_signal = ags_audio_signal_new(devout,
				      recycling,
				      input_recall_id);
  ags_audio_signal_stream_resize(audio_signal,
				 AGS_RECALL_AUDIO_SIGNAL_TEST_RUN_INTER_AUDIO_SIGNAL_LENGTH);

  audio_signal->stream_current = audio_signal->stream;
  ags_recycling_add_audio_signal(recycling,
				 audio_signal);

  /* test */
  g_object_get(start_recall_recycling->data,
	       "child", &start_recall_audio_signal,
	       NULL);

  recall_audio_signal = ags_recall_find_provider_with_recycling_context(start_recall_audio_signal,
									audio_signal,
									input_recycling_context);

  CU_ASSERT(recall_audio_signal != NULL);
  CU_ASSERT(AGS_IS_RECALL_AUDIO_SIGNAL(recall_audio_signal->data));
}

void
ags_recall_recycling_test_source_remove_audio_signal_done_callback()
{
  remove_audio_signal_done_callback_invoked = TRUE;
}

void
ags_recall_recycling_test_source_remove_audio_signal()
{
  AgsChannel *output;
  AgsChannel *input;
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal;
  AgsRecallID *output_audio_recall_id;
  AgsRecallID *input_audio_recall_id;
  AgsRecallID *output_recall_id;
  AgsRecallID *input_recall_id;
  AgsRecyclingContext *output_recycling_context;
  AgsRecyclingContext *input_recycling_context;
  AgsRecallChannel *recall_channel;
  AgsRecallChannelRun *recall_channel_run;
  AgsRecallRecycling *recall;
  AgsRecallContainer *recall_container;
  
  GList *start_list, *list;
  GList *start_recall_recycling, *recall_recycling;
  GList *start_recall_audio_signal, *recall_audio_signal;
  
  output = audio->output;
  input = audio->input;

  recycling = input->first_recycling;

  /* output - create recall id and recycling context */
  output_audio_recall_id = ags_recall_id_new();
  ags_recall_id_set_sound_scope(output_audio_recall_id,
				AGS_SOUND_SCOPE_PLAYBACK);
  ags_audio_add_recall_id(audio,
			  output_audio_recall_id);

  output_recall_id = ags_recall_id_new();
  ags_recall_id_set_sound_scope(output_recall_id,
				AGS_SOUND_SCOPE_PLAYBACK);
  ags_channel_add_recall_id(output,
			    output_recall_id);
  
  output_recycling_context = ags_recycling_context_new(1);
  ags_recycling_context_replace(output_recycling_context,
				output->first_recycling,
				0);
  ags_audio_add_recycling_context(audio,
				  output_recycling_context);
  
  g_object_set(output_audio_recall_id,
	       "recycling-context", output_recycling_context,
	       NULL);
  g_object_set(output_recall_id,
	       "recycling-context", output_recycling_context,
	       NULL);

  g_object_set(output_recycling_context,
	       "recall-id", output_recall_id,
	       NULL);
  
  /* input - create recall id and recycling context */
  input_audio_recall_id = ags_recall_id_new();
  ags_recall_id_set_sound_scope(input_audio_recall_id,
				AGS_SOUND_SCOPE_PLAYBACK);
  ags_audio_add_recall_id(audio,
			  input_audio_recall_id);

  input_recall_id = ags_recall_id_new();
  ags_recall_id_set_sound_scope(input_recall_id,
				AGS_SOUND_SCOPE_PLAYBACK);
  ags_channel_add_recall_id(input,
			    input_recall_id);
  
  input_recycling_context = ags_recycling_context_new(1);
  ags_recycling_context_replace(input_recycling_context,
				recycling,
				0);
  ags_audio_add_recycling_context(audio,
				  input_recycling_context);
  
  g_object_set(input_audio_recall_id,
	       "recycling-context", input_recycling_context,
	       NULL);
  g_object_set(input_recall_id,
	       "recycling-context", input_recycling_context,
	       NULL);

  g_object_set(output_recycling_context,
	       "child", input_recycling_context,
	       NULL);

  g_object_set(input_recycling_context,
	       "recall-id", input_recall_id,
	       NULL);

  /* create recall */
  recall_container = ags_recall_container_new();
  ags_channel_add_recall_container(input,
				   (GObject *) recall_container);

  recall_channel = ags_recall_channel_new();
  ags_recall_set_flags(recall_channel,
		       AGS_RECALL_TEMPLATE);
  ags_recall_set_ability_flags(recall_channel,
			       (AGS_SOUND_ABILITY_PLAYBACK));
  g_object_set(recall_channel,
	       "source", input,
	       "recall-container", recall_container,
	       NULL);  
  ags_channel_add_recall(input,
			 recall_channel,
			 FALSE);
  ags_connectable_connect(AGS_CONNECTABLE(recall_channel));
    
  recall_channel_run = ags_recall_channel_run_new();
  ags_recall_set_flags(recall_channel_run,
		       AGS_RECALL_TEMPLATE);
  ags_recall_set_ability_flags(recall_channel_run,
			       (AGS_SOUND_ABILITY_PLAYBACK));
  g_object_set(recall_channel_run,
	       "child-type", AGS_TYPE_RECALL_RECYCLING,
	       "recall-channel", recall_channel,
	       "source", input,
	       "recall-container", recall_container,
	       NULL);  
  ags_channel_add_recall(input,
			 recall_channel_run,
			 FALSE);
  ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run));
  
  recall = ags_recall_recycling_new();
  g_object_set(recall,
	       "child-type", AGS_TYPE_RECALL_AUDIO_SIGNAL,
	       "source", recycling,
	       NULL);
  ags_recall_add_child(recall_channel_run,
		       recall);
  
  /* duplicate recall */
  ags_audio_duplicate_recall(audio,
			     input_audio_recall_id,
			     0, 0,
			     0);
  ags_channel_duplicate_recall(input,
			       input_recall_id);

  /* resolve recall */
  ags_audio_resolve_recall(audio,
			   input_audio_recall_id);
  ags_channel_resolve_recall(input,
			     input_recall_id);
  
  /* init recall */
  ags_audio_init_recall(audio,
			input_audio_recall_id, (AGS_SOUND_STAGING_CHECK_RT_DATA |
						AGS_SOUND_STAGING_RUN_INIT_PRE |
						AGS_SOUND_STAGING_RUN_INIT_INTER |
						AGS_SOUND_STAGING_RUN_INIT_POST));
  
  ags_channel_init_recall(input,
			  input_recall_id, (AGS_SOUND_STAGING_CHECK_RT_DATA |
					    AGS_SOUND_STAGING_RUN_INIT_PRE |
					    AGS_SOUND_STAGING_RUN_INIT_INTER |
					    AGS_SOUND_STAGING_RUN_INIT_POST));

  /* test */
  g_object_get(input,
	       "recall", &start_list,
	       NULL);

  list = ags_recall_find_type_with_recycling_context(start_list,
						     AGS_TYPE_RECALL_CHANNEL_RUN,
						     input_recycling_context);

  g_object_get(list->data,
	       "child", &start_recall_recycling,
	       NULL);
  
  recall_recycling = ags_recall_find_recycling_context(start_recall_recycling,
						       input_recycling_context);
  
  g_object_set(start_recall_recycling->data,
	       "child-type", AGS_TYPE_RECALL_AUDIO_SIGNAL,
	       NULL);
  
  /* create audio signal */  
  audio_signal = ags_audio_signal_new(devout,
				      recycling,
				      input_recall_id);
  ags_audio_signal_stream_resize(audio_signal,
				 AGS_RECALL_AUDIO_SIGNAL_TEST_RUN_INTER_AUDIO_SIGNAL_LENGTH);

  audio_signal->stream_current = audio_signal->stream;
  ags_recycling_add_audio_signal(recycling,
				 audio_signal);

  /* test */
  g_object_get(start_recall_recycling->data,
	       "child", &start_recall_audio_signal,
	       NULL);

  recall_audio_signal = ags_recall_find_provider_with_recycling_context(start_recall_audio_signal,
									audio_signal,
									input_recycling_context);

  CU_ASSERT(recall_audio_signal != NULL);
  CU_ASSERT(AGS_IS_RECALL_AUDIO_SIGNAL(recall_audio_signal->data));

  /* test remove */
  g_signal_connect(recall_audio_signal->data, "done",
		   G_CALLBACK(ags_recall_recycling_test_source_remove_audio_signal_done_callback), NULL);

  ags_recycling_remove_audio_signal(recycling,
				    audio_signal);
  
  CU_ASSERT(remove_audio_signal_done_callback_invoked == TRUE);
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C");
  putenv("LANG=C");

  putenv("LADSPA_PATH=\"\"");
  putenv("DSSI_PATH=\"\"");
  putenv("LV2_PATH=\"\"");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsRecallRecyclingTest", ags_recall_recycling_test_init_suite, ags_recall_recycling_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsRecallRecycling source add audio signal", ags_recall_recycling_test_source_add_audio_signal) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecallRecycling source remove audio signal", ags_recall_recycling_test_source_remove_audio_signal) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
