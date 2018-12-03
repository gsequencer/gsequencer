/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/libags.h>
#include <ags/libags-audio.h>

int ags_eq10_audio_signal_test_init_suite();
int ags_eq10_audio_signal_test_clean_suite();

void ags_eq10_audio_signal_test_run_inter();

void ags_eq10_audio_signal_test_run_inter_callback(AgsRecall *recall, gpointer data);

#define AGS_EQ10_AUDIO_SIGNAL_TEST_RUN_INTER_AUDIO_SIGNAL_LENGTH (24)

AgsDevout *devout;
AgsAudio *audio;

GList *recall;

guint test_run_inter_invoke_count = 0;

AgsApplicationContext *application_context;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_eq10_audio_signal_test_init_suite()
{
  application_context = ags_audio_application_context_new();
  g_object_ref(application_context);
  
  ags_application_context_prepare(application_context);
  ags_application_context_setup(application_context);
  
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
  
  /* create ags-eq10 */
  recall = ags_recall_factory_create(audio,
				     NULL, NULL,
				     "ags-eq10",
				     0, 1,
				     0, 1,
				     (AGS_RECALL_FACTORY_INPUT |
				      AGS_RECALL_FACTORY_ADD |
				      AGS_RECALL_FACTORY_PLAY |
				      AGS_RECALL_FACTORY_RECALL),
				     0);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_eq10_audio_signal_test_clean_suite()
{
  g_object_run_dispose(devout);
  g_object_unref(devout);

  g_object_run_dispose(audio);
  g_object_unref(audio);
  
  return(0);
}

void
ags_eq10_audio_signal_test_run_inter_callback(AgsRecall *recall, gpointer data)
{
  test_run_inter_invoke_count += 1;
}

void
ags_eq10_audio_signal_test_run_inter()
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

  GList *start_list, *list;
  GList *start_recall_recycling, *recall_recycling;
  GList *start_recall_audio_signal, *recall_audio_signal;

  guint i;
  
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
  
  /* create audio signal */  
  audio_signal = ags_audio_signal_new(devout,
				      recycling,
				      input_recall_id);
  ags_audio_signal_stream_resize(audio_signal,
				 AGS_EQ10_AUDIO_SIGNAL_TEST_RUN_INTER_AUDIO_SIGNAL_LENGTH);

  audio_signal->stream_current = audio_signal->stream;
  ags_recycling_add_audio_signal(recycling,
				 audio_signal);

  /* test */
  g_object_get(input,
	       "recall", &start_list,
	       NULL);

  list = ags_recall_find_type_with_recycling_context(start_list,
						     AGS_TYPE_EQ10_CHANNEL_RUN,
						     input_recycling_context);

  g_object_get(list->data,
	       "child", &start_recall_recycling,
	       NULL);
  
  recall_recycling = ags_recall_find_recycling_context(start_recall_recycling,
						       input_recycling_context);
  
  g_object_get(start_recall_recycling->data,
	       "child", &start_recall_audio_signal,
	       NULL);

  recall_audio_signal = ags_recall_find_provider_with_recycling_context(start_recall_audio_signal,
									audio_signal,
									input_recycling_context);

  CU_ASSERT(recall_audio_signal != NULL);
  CU_ASSERT(AGS_IS_EQ10_AUDIO_SIGNAL(recall_audio_signal->data));
  CU_ASSERT(AGS_RECALL_CLASS(AGS_EQ10_AUDIO_SIGNAL_GET_CLASS(recall_audio_signal->data))->run_inter != NULL);

  g_signal_connect(recall_audio_signal->data, "run-inter",
		   G_CALLBACK(ags_eq10_audio_signal_test_run_inter_callback), NULL);

  for(i = 0; i < AGS_EQ10_AUDIO_SIGNAL_TEST_RUN_INTER_AUDIO_SIGNAL_LENGTH; i++){
    ags_recall_run_inter(recall_audio_signal->data);

    if(audio_signal->stream_current != NULL){
      audio_signal->stream_current = audio_signal->stream_current->next;
    }
  }

  CU_ASSERT(test_run_inter_invoke_count == AGS_EQ10_AUDIO_SIGNAL_TEST_RUN_INTER_AUDIO_SIGNAL_LENGTH);
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C");
  putenv("LANG=C");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsEq10AudioSignalTest", ags_eq10_audio_signal_test_init_suite, ags_eq10_audio_signal_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsEq10AudioSignal run inter", ags_eq10_audio_signal_test_run_inter) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
