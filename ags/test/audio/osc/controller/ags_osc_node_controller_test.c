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

#include <stdlib.h>
#include <string.h>
#include <math.h>

int ags_osc_node_controller_test_init_suite();
int ags_osc_node_controller_test_clean_suite();

void ags_osc_node_controller_test_get_data();

#define AGS_OSC_NODE_CONTROLLER_TEST_CONFIG "[generic]\n" \
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
  "[recall]\n"					       \
  "auto-sense=true\n"				       \
  "\n"

AgsApplicationContext *application_context;

AgsAudio *panel;
AgsAudio *spectrometer;
AgsAudio *drum;

GObject *default_soundcard;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_node_controller_test_init_suite()
{
  AgsConfig *config;

  GList *start_audio;
  
  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_OSC_NODE_CONTROLLER_TEST_CONFIG,
			    strlen(AGS_OSC_NODE_CONTROLLER_TEST_CONFIG));

  application_context = ags_audio_application_context_new();
  g_object_ref(application_context);
  
  ags_application_context_prepare(application_context);
  ags_application_context_setup(application_context);

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  /* panel */
  panel = ags_audio_new(default_soundcard);
  g_object_ref(panel);

  g_object_set(panel,
	       "audio-name", "test-panel",
	       NULL);
  
  ags_audio_set_flags(panel,
		      (AGS_AUDIO_SYNC));

  ags_audio_set_audio_channels(panel,
			       2, 0);

  ags_audio_set_pads(panel,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(panel,
		     AGS_TYPE_INPUT,
		     1, 0);

  start_audio = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));
  ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(application_context),
			       g_list_prepend(start_audio,
					      panel));

  /* ags-play */
  ags_recall_factory_create(panel,
			    NULL, NULL,
			    "ags-play-master",
			    0, 2,
			    0, 1,
			    (AGS_RECALL_FACTORY_INPUT,
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* ags-mute */
  ags_recall_factory_create(panel,
			    NULL, NULL,
			    "ags-mute",
			    0, 2,
			    0, 1,
			    (AGS_RECALL_FACTORY_INPUT,
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* spectrometer */
  spectrometer = ags_audio_new(default_soundcard);
  g_object_ref(spectrometer);

  g_object_set(spectrometer,
	       "audio-name", "test-spectrometer",
	       NULL);
  
  ags_audio_set_flags(spectrometer,
		      (AGS_AUDIO_SYNC));

  ags_audio_set_audio_channels(spectrometer,
			       2, 0);

  ags_audio_set_pads(spectrometer,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(spectrometer,
		     AGS_TYPE_INPUT,
		     1, 0);

  start_audio = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));
  ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(application_context),
			       g_list_prepend(start_audio,
					      spectrometer));
  
  ags_recall_factory_create(spectrometer,
			    NULL, NULL,
			    "ags-analyse",
			    0, 2,
			    0, 1,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL |
			     AGS_RECALL_FACTORY_ADD),
			    0);

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

  /* ags-copy-pattern */
  ags_recall_factory_create(drum,
			    NULL, NULL,
			    "ags-copy-pattern",
			    0, 2,
			    0, 8,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_REMAP |
			     AGS_RECALL_FACTORY_RECALL),
			    0);

  /* ags-volume */
  ags_recall_factory_create(drum,
			    NULL, NULL,
			    "ags-volume",
			    0, 2, 
			    0, 8,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* ags-envelope */
  ags_recall_factory_create(drum,
			    NULL, NULL,
			    "ags-envelope",
			    0, 2,
			    0, 8,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_node_controller_test_clean_suite()
{
  return(0);
}

void
ags_osc_node_controller_test_get_data()
{  
  AgsChannel *channel;
  
  AgsOscConnection *osc_connection;

  AgsOscNodeController *osc_node_controller;

  GList *osc_response;

  guint length;
  guint padding;
  gboolean success;

  GValue value = {0,};

  static const unsigned char *volume_message = "/node\x00\x00\x00,s\x00\x00/AgsSoundProvider/AgsAudio[\"test-drum\"]/AgsInput[0-1]/AgsVolumeChannel[0]/AgsPort[\"./volume[0]\"]:value\x00\x00\x00\x00";

  static const guint volume_message_size = 108;

  osc_connection = ags_osc_connection_new(NULL);
  
  osc_node_controller = ags_osc_node_controller_new();

  /* drum */
  osc_response = ags_osc_node_controller_get_data(osc_node_controller,
						  osc_connection,
						  volume_message, volume_message_size);

  CU_ASSERT(osc_response != NULL);

  success = TRUE;
  
  while(osc_response != NULL){
    gchar *address_pattern;
    gchar *type_tag;
    
    ags_osc_buffer_util_get_message(AGS_OSC_RESPONSE(osc_response->data)->packet + 4,
				    &address_pattern, &type_tag);

    success = !g_strcmp0(address_pattern,
			 "/node");
    
    if(!success){
      break;
    }

    success = !g_strcmp0(type_tag,
			 ",sf");
    
    if(!success){
      g_message("%s", type_tag);
      
      break;
    }
  }
  
  CU_ASSERT(success);
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
  pSuite = CU_add_suite("AgsOscNodeControllerTest", ags_osc_node_controller_test_init_suite, ags_osc_node_controller_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsNodeController get data", ags_osc_node_controller_test_get_data) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
