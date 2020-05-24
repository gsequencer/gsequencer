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

int ags_fx_peak_channel_processor_test_init_suite();
int ags_fx_peak_channel_processor_test_clean_suite();

void ags_fx_peak_channel_processor_test_new();
void ags_fx_peak_channel_processor_test_run_inter();

AgsApplicationContext *application_context;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fx_peak_channel_processor_test_init_suite()
{ 
  application_context = ags_audio_application_context_new();

  ags_application_context_prepare(application_context);
  ags_application_context_setup(application_context);

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fx_peak_channel_processor_test_clean_suite()
{  
  return(0);
}

void
ags_fx_peak_channel_processor_test_new()
{
  AgsChannel *channel;
  AgsFxPeakChannelProcessor *fx_peak_channel_processor;

  channel = g_object_new(AGS_TYPE_CHANNEL,
			 NULL);
  
  fx_peak_channel_processor = ags_fx_peak_channel_processor_new(channel);

  CU_ASSERT(fx_peak_channel_processor != NULL);
  CU_ASSERT(AGS_RECALL_CHANNEL_RUN(fx_peak_channel_processor)->source == channel);

  CU_ASSERT(AGS_RECALL(fx_peak_channel_processor)->child_type == AGS_TYPE_FX_PEAK_RECYCLING);
}

void
ags_fx_peak_channel_processor_test_run_inter()
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsRecallContainer *recall_container;
  AgsFxPeakAudio *fx_peak_audio;
  AgsFxPeakAudioProcessor *fx_peak_audio_processor;
  AgsFxPeakChannel *fx_peak_channel;
  AgsFxPeakChannelProcessor *fx_peak_channel_processor;
  
  /* audio */
  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);

  recall_container = ags_recall_container_new();
  ags_audio_add_recall_container(audio,
				 recall_container);
  
  fx_peak_audio = ags_fx_peak_audio_new(audio);
  ags_recall_set_sound_scope(fx_peak_audio, AGS_SOUND_SCOPE_PLAYBACK);
  ags_recall_container_add(recall_container,
			   fx_peak_audio);
  
  CU_ASSERT(fx_peak_audio != NULL);

  /* audio processor */  
  fx_peak_audio_processor = ags_fx_peak_audio_processor_new(audio);
  ags_recall_set_sound_scope(fx_peak_audio_processor, AGS_SOUND_SCOPE_PLAYBACK);

  g_object_set(fx_peak_audio_processor,
	       "recall-audio", fx_peak_audio,
	       NULL);
  
  ags_recall_container_add(recall_container,
			   fx_peak_audio_processor);

  CU_ASSERT(fx_peak_audio_processor != NULL);

  /* channel */
  channel = g_object_new(AGS_TYPE_CHANNEL,
			 NULL);

  ags_channel_add_recall_container(channel,
				   recall_container);
  
  fx_peak_channel = ags_fx_peak_channel_new(channel);
  ags_recall_set_sound_scope(fx_peak_channel, AGS_SOUND_SCOPE_PLAYBACK);

  g_object_set(fx_peak_channel,
	       "recall-audio", fx_peak_audio,
	       NULL);

  ags_recall_container_add(recall_container,
			   fx_peak_channel);

  CU_ASSERT(fx_peak_channel != NULL);

  /* channel processor */  
  fx_peak_channel_processor = ags_fx_peak_channel_processor_new(channel);
  ags_recall_set_sound_scope(fx_peak_channel_processor, AGS_SOUND_SCOPE_PLAYBACK);

  g_object_set(fx_peak_channel_processor,
	       "recall-audio", fx_peak_audio,
	       "recall-channel", fx_peak_channel,
	       NULL);

  ags_recall_container_add(recall_container,
			   fx_peak_channel_processor);

  CU_ASSERT(fx_peak_channel_processor != NULL);

  /* run inter - attempt #0 */  
  ags_recall_run_inter(fx_peak_channel_processor);

  /* run inter - attempt #1 */
  fx_peak_channel->peak_reseted = FALSE;
  
  ags_recall_run_inter(fx_peak_channel_processor);
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
  pSuite = CU_add_suite("AgsFxPeakChannelProcessorTest", ags_fx_peak_channel_processor_test_init_suite, ags_fx_peak_channel_processor_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsFxPeakChannelProcessor new", ags_fx_peak_channel_processor_test_new) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxPeakChannelProcessor run inter", ags_fx_peak_channel_processor_test_run_inter) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
