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

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int ags_osc_action_controller_test_init_suite();
int ags_osc_action_controller_test_clean_suite();

void ags_osc_action_controller_test_run_action();

#define AGS_OSC_ACTION_CONTROLLER_TEST_CONFIG "[generic]\n" \
  "autosave-thread=false\n"			       \
  "simple-file=false\n"				       \
  "disable-feature=experimental\n"		       \
  "segmentation=4/4\n"				       \
  "\n"						       \
  "[thread]\n"					       \
  "model=super-threaded\n"			       \
  "super-threaded-scope=channel\n"		       \
  "lock-global=ags-thread\n"			       \
  "lock-parent=ags-recycling-thread\n"		       \
  "\n"						       \
  "[soundcard-0]\n"				       \
  "backend=alsa\n"                                     \
  "device=default\n"				       \
  "samplerate=44100\n"				       \
  "buffer-size=1024\n"				       \
  "pcm-channels=2\n"				       \
  "dsp-channels=2\n"				       \
  "format=16\n"					       \
  "\n"						       \
  "[sequencer-0]\n"				       \
  "backend=alsa\n"                                     \
  "device=default\n"				       \
  "\n"                                                 \
  "[recall]\n"					       \
  "auto-sense=true\n"				       \
  "\n"

AgsApplicationContext *application_context;

AgsAudio *drum;

GObject *default_soundcard;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_action_controller_test_init_suite()
{
  AgsConfig *config;

  GList *start_audio;
  
  ags_priority_load_defaults(ags_priority_get_instance());  

  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_OSC_ACTION_CONTROLLER_TEST_CONFIG,
			    strlen(AGS_OSC_ACTION_CONTROLLER_TEST_CONFIG));

  application_context = ags_audio_application_context_new();
  g_object_ref(application_context);
  
  ags_application_context_prepare(application_context);
  ags_application_context_setup(application_context);

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  /* drum */
  drum = ags_audio_new(default_soundcard);
  g_object_ref(drum);

  g_object_set(drum,
	       "audio-name", "test-drum",
	       NULL);
  
  ags_audio_set_flags(drum,
		      (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		       AGS_AUDIO_INPUT_HAS_RECYCLING |
		       AGS_AUDIO_SYNC |
		       AGS_AUDIO_ASYNC));

  ags_audio_set_ability_flags(drum, (AGS_SOUND_ABILITY_PLAYBACK |
				     AGS_SOUND_ABILITY_SEQUENCER |
				     AGS_SOUND_ABILITY_NOTATION));

  ags_audio_set_audio_channels(drum,
			       2, 0);

  ags_audio_set_pads(drum,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(drum,
		     AGS_TYPE_INPUT,
		     8, 0);

  start_audio = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));
  ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(application_context),
			       g_list_prepend(start_audio,
					      drum));

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_action_controller_test_clean_suite()
{
  return(0);
}

void
ags_osc_action_controller_test_run_action()
{  
  AgsOscConnection *osc_connection;

  AgsOscActionController *osc_action_controller;
  
  GList *osc_response;

  static const unsigned char *start_soundcard_message = "/action\x00,ss\x00/AgsSoundProvider/AgsSoundcard\x00\x00start\x00\x00\x00";
  static const unsigned char *start_sequencer_message = "/action\x00,ss\x00/AgsSoundProvider/AgsSequencer\x00\x00start\x00\x00\x00";
  static const unsigned char *start_audio_message =     "/action\x00,ss\x00/AgsSoundProvider/AgsAudio[0]\x00\x00\x00start\x00\x00\x00";
  static const unsigned char *stop_soundcard_message = "/action\x00,ss\x00/AgsSoundProvider/AgsSoundcard\x00\x00stop\x00\x00\x00\x00";
  static const unsigned char *stop_sequencer_message = "/action\x00,ss\x00/AgsSoundProvider/AgsSequencer\x00\x00stop\x00\x00\x00\x00";
  static const unsigned char *stop_audio_message =     "/action\x00,ss\x00/AgsSoundProvider/AgsAudio[0]\x00\x00\x00stop\x00\x00\x00\x00";

  static const guint start_soundcard_message_size = 52;
  static const guint start_sequencer_message_size = 52;
  static const guint start_audio_message_size = 52;
  static const guint stop_soundcard_message_size = 52;
  static const guint stop_sequencer_message_size = 52;
  static const guint stop_audio_message_size = 52;

  osc_connection = ags_osc_connection_new(NULL);
  
  osc_action_controller = ags_osc_action_controller_new();

  /* soundcard */
  osc_response = ags_osc_action_controller_run_action(osc_action_controller,
						      osc_connection,
						      start_soundcard_message, start_soundcard_message_size);

  CU_ASSERT(osc_response != NULL);

  /* sequencer */
  osc_response = ags_osc_action_controller_run_action(osc_action_controller,
						      osc_connection,
						      start_sequencer_message, start_sequencer_message_size);

  CU_ASSERT(osc_response != NULL);

  /* audio */
  osc_response = ags_osc_action_controller_run_action(osc_action_controller,
						      osc_connection,
						      start_audio_message, start_audio_message_size);

  CU_ASSERT(osc_response != NULL);

  /* wait a moment */
  sleep(10);

  /* soundcard */
  osc_response = ags_osc_action_controller_run_action(osc_action_controller,
						      osc_connection,
						      stop_soundcard_message, stop_soundcard_message_size);

  CU_ASSERT(osc_response != NULL);

  /* sequencer */
  osc_response = ags_osc_action_controller_run_action(osc_action_controller,
						      osc_connection,
						      stop_sequencer_message, stop_sequencer_message_size);

  CU_ASSERT(osc_response != NULL);

  /* audio */
  osc_response = ags_osc_action_controller_run_action(osc_action_controller,
						      osc_connection,
						      stop_audio_message, stop_audio_message_size);

  CU_ASSERT(osc_response != NULL);  
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
  pSuite = CU_add_suite("AgsOscActionControllerTest", ags_osc_action_controller_test_init_suite, ags_osc_action_controller_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsOscActionController run action", ags_osc_action_controller_test_run_action) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
