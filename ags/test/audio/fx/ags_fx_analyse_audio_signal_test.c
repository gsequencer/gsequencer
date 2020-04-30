/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <math.h>

int ags_fx_analyse_audio_signal_test_init_suite();
int ags_fx_analyse_audio_signal_test_clean_suite();

void ags_fx_analyse_audio_signal_test_new();
void ags_fx_analyse_audio_signal_test_run_inter();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fx_analyse_audio_signal_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fx_analyse_audio_signal_test_clean_suite()
{  
  return(0);
}

void
ags_fx_analyse_audio_signal_test_new()
{
  AgsAudioSignal *audio_signal;
  AgsFxAnalyseAudioSignal *fx_analyse_audio_signal;

  audio_signal = g_object_new(AGS_TYPE_AUDIO_SIGNAL,
			      NULL);
  
  fx_analyse_audio_signal = ags_fx_analyse_audio_signal_new(audio_signal);

  CU_ASSERT(fx_analyse_audio_signal != NULL);
  CU_ASSERT(AGS_RECALL_AUDIO_SIGNAL(fx_analyse_audio_signal)->source == audio_signal);
}

void
ags_fx_analyse_audio_signal_test_run_inter()
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal;
  AgsRecallContainer *recall_container;
  AgsFxAnalyseAudio *fx_analyse_audio;
  AgsFxAnalyseAudioProcessor *fx_analyse_audio_processor;
  AgsFxAnalyseChannel *fx_analyse_channel;
  AgsFxAnalyseChannelProcessor *fx_analyse_channel_processor;
  AgsFxAnalyseRecycling *fx_analyse_recycling;
  AgsFxAnalyseAudioSignal *fx_analyse_audio_signal;

  
  /* audio */
  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);

  recall_container = ags_recall_container_new();
  ags_audio_add_recall_container(audio,
				 recall_container);
  
  fx_analyse_audio = ags_fx_analyse_audio_new(audio);
  ags_recall_container_add(recall_container,
			   fx_analyse_audio);
  
  CU_ASSERT(fx_analyse_audio != NULL);

  /* audio processor */  
  fx_analyse_audio_processor = ags_fx_analyse_audio_processor_new(audio);

  g_object_set(fx_analyse_audio_processor,
	       "recall-audio", fx_analyse_audio,
	       NULL);
  
  ags_recall_container_add(recall_container,
			   fx_analyse_audio_processor);

  CU_ASSERT(fx_analyse_audio_processor != NULL);

  /* channel */
  channel = g_object_new(AGS_TYPE_CHANNEL,
			 NULL);

  ags_channel_add_recall_container(channel,
				   recall_container);
  
  fx_analyse_channel = ags_fx_analyse_channel_new(channel);

  g_object_set(fx_analyse_channel,
	       "recall-audio", fx_analyse_audio,
	       NULL);

  ags_recall_container_add(recall_container,
			   fx_analyse_channel);

  CU_ASSERT(fx_analyse_channel != NULL);

  /* channel processor */  
  fx_analyse_channel_processor = ags_fx_analyse_channel_processor_new(channel);

  g_object_set(fx_analyse_channel_processor,
	       "recall-audio", fx_analyse_audio,
	       "recall-channel", fx_analyse_channel,
	       NULL);

  ags_recall_container_add(recall_container,
			   fx_analyse_channel_processor);

  CU_ASSERT(fx_analyse_channel_processor != NULL);

  /* recycling */
  recycling = g_object_new(AGS_TYPE_RECYCLING,
			   NULL);
  
  fx_analyse_recycling = ags_fx_analyse_recycling_new(recycling);

  ags_recall_add_child(fx_analyse_channel_processor,
		       fx_analyse_recycling);
  
  CU_ASSERT(fx_analyse_recycling != NULL);

  /* audio signal */
  audio_signal = g_object_new(AGS_TYPE_AUDIO_SIGNAL,
			      NULL);
  
  fx_analyse_audio_signal = ags_fx_analyse_audio_signal_new(audio_signal);

  ags_recall_add_child(fx_analyse_recycling,
		       fx_analyse_audio_signal);

  CU_ASSERT(fx_analyse_audio_signal != NULL);

  /* run inter */
  //TODO:JK: improve me
  
  ags_recall_run_inter(fx_analyse_audio_signal);
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
  pSuite = CU_add_suite("AgsFxAnalyseAudioSignalTest", ags_fx_analyse_audio_signal_test_init_suite, ags_fx_analyse_audio_signal_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsFxAnalyseAudioSignal new", ags_fx_analyse_audio_signal_test_new) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxAnalyseAudioSignal run inter", ags_fx_analyse_audio_signal_test_run_inter) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
