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

#include <math.h>

int ags_start_channel_test_init_suite();
int ags_start_channel_test_clean_suite();

void ags_start_channel_test_launch();

#define AGS_START_CHANNEL_TEST_CONFIG "[generic]\n"	\
  "autosave-thread=false\n"				\
  "simple-file=true\n"					\
  "disable-feature=experimental\n"			\
  "segmentation=4/4\n"					\
  "\n"							\
  "[thread]\n"						\
  "model=super-threaded\n"				\
  "super-threaded-scope=channel\n"			\
  "lock-global=ags-thread\n"				\
  "lock-parent=ags-recycling-thread\n"			\
  "\n"							\
  "[soundcard]\n"					\
  "backend=alsa\n"					\
  "device=default\n"					\
  "samplerate=48000\n"					\
  "buffer-size=1024\n"					\
  "pcm-channels=2\n"					\
  "dsp-channels=2\n"					\
  "format=16\n"						\
  "\n"							\
  "[recall]\n"						\
  "auto-sense=true\n"					\
  "\n"

AgsConfig *config;

AgsApplicationContext *application_context;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_start_channel_test_init_suite()
{
  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_START_CHANNEL_TEST_CONFIG,
			    strlen(AGS_START_CHANNEL_TEST_CONFIG));
  
  application_context = ags_audio_application_context_new();
  g_object_ref(application_context);

  ags_application_context_prepare(application_context);

  ags_application_context_setup(application_context);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_start_channel_test_clean_suite()
{  
  return(0);
}

void
ags_start_channel_test_launch()
{
  AgsAudio *audio;
  AgsChannel *channel;
  
  AgsStartChannel *start_channel;

  audio = ags_audio_new(NULL);
  ags_audio_set_flags(audio,
		      (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		       AGS_AUDIO_INPUT_HAS_RECYCLING));
  ags_audio_set_ability_flags(audio,
			      (AGS_SOUND_ABILITY_SEQUENCER |
			       AGS_SOUND_ABILITY_NOTATION |
			       AGS_SOUND_ABILITY_WAVE));
  
  ags_audio_set_audio_channels(audio,
			       2, 0);

  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     8, 0);

  g_object_get(audio,
	       "input", &channel,
	       NULL);
  
  start_channel = ags_start_channel_new(channel,
					-1);
  
  CU_ASSERT(AGS_IS_START_CHANNEL(start_channel));
  CU_ASSERT(start_channel->channel == channel);
  CU_ASSERT(start_channel->sound_scope == -1);

  /* launch */
  ags_task_launch(start_channel);

  CU_ASSERT(channel->recall_id != NULL);
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
  pSuite = CU_add_suite("AgsStartChannelTest", ags_start_channel_test_init_suite, ags_start_channel_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsStartChannel launch", ags_start_channel_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
