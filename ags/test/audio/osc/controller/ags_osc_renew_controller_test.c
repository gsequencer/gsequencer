/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

int ags_osc_renew_controller_test_init_suite();
int ags_osc_renew_controller_test_clean_suite();

void ags_osc_renew_controller_test_set_data();

#define AGS_OSC_RENEW_CONTROLLER_TEST_CONFIG "[generic]\n"	\
  "autosave-thread=false\n"					\
  "simple-file=false\n"						\
  "disable-feature=experimental\n"				\
  "segmentation=4/4\n"						\
  "\n"								\
  "[thread]\n"							\
  "model=super-threaded\n"					\
  "super-threaded-scope=channel\n"				\
  "lock-global=ags-thread\n"					\
  "lock-parent=ags-recycling-thread\n"				\
  "\n"								\
  "[soundcard-0]\n"						\
  "backend=alsa\n"						\
  "device=default\n"						\
  "samplerate=44100\n"						\
  "buffer-size=1024\n"						\
  "pcm-channels=2\n"						\
  "dsp-channels=2\n"						\
  "format=16\n"							\
  "\n"								\
  "[recall]\n"							\
  "auto-sense=true\n"						\
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
ags_osc_renew_controller_test_init_suite()
{
  AgsConfig *config;

  GList *start_audio;
  
  ags_priority_load_defaults(ags_priority_get_instance());  

  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_OSC_RENEW_CONTROLLER_TEST_CONFIG,
			    strlen(AGS_OSC_RENEW_CONTROLLER_TEST_CONFIG));

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

  /* ags-fx-playback */
  ags_fx_factory_create(panel,
			ags_recall_container_new(), ags_recall_container_new(),
			"ags-fx-playback",
			NULL,
			NULL,
			0, 2,
			0, 1,
			0,
			(AGS_FX_FACTORY_INPUT,
			 AGS_FX_FACTORY_PLAY |
			 AGS_FX_FACTORY_ADD),
			0);

  /* ags-fx-volume */
  ags_fx_factory_create(panel,
			ags_recall_container_new(), ags_recall_container_new(),
			"ags-fx-volume",
			NULL,
			NULL,
			0, 2,
			0, 1,
			0,
			(AGS_FX_FACTORY_INPUT,
			 AGS_FX_FACTORY_PLAY |
			 AGS_FX_FACTORY_ADD),
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
  
  ags_fx_factory_create(spectrometer,
			ags_recall_container_new(), ags_recall_container_new(),
			"ags-fx-analyse",
			NULL,
			NULL,
			0, 2,
			0, 1,
			0,
			(AGS_FX_FACTORY_INPUT |
			 AGS_FX_FACTORY_PLAY |
			 AGS_FX_FACTORY_RECALL |
			 AGS_FX_FACTORY_ADD),
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

  /* ags-fx-pattern */
  ags_fx_factory_create(drum,
			ags_recall_container_new(), ags_recall_container_new(),
			"ags-fx-pattern",
			NULL,
			NULL,
			0, 2,
			0, 8,
			0,
			(AGS_FX_FACTORY_INPUT |
			 AGS_FX_FACTORY_REMAP |
			 AGS_FX_FACTORY_RECALL),
			0);

  /* ags-fx-volume */
  ags_fx_factory_create(drum,
			ags_recall_container_new(), ags_recall_container_new(),
			"ags-fx-volume",
			NULL,
			NULL,
			0, 2, 
			0, 8,
			0,
			(AGS_FX_FACTORY_INPUT |
			 AGS_FX_FACTORY_PLAY |
			 AGS_FX_FACTORY_RECALL |
			 AGS_FX_FACTORY_ADD),
			0);

  /* ags-fx-envelope */
  ags_fx_factory_create(drum,
			ags_recall_container_new(), ags_recall_container_new(),
			"ags-fx-envelope",
			NULL,
			NULL,
			0, 2,
			0, 8,
			0,
			(AGS_FX_FACTORY_INPUT |
			 AGS_FX_FACTORY_PLAY |
			 AGS_FX_FACTORY_RECALL |
			 AGS_FX_FACTORY_ADD),
			0);

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_renew_controller_test_clean_suite()
{
  return(0);
}

void
ags_osc_renew_controller_test_set_data()
{
  AgsChannel *channel;
  
  AgsOscConnection *osc_connection;

  AgsOscRenewController *osc_renew_controller;

  GList *osc_response;

  guchar *message;
  guchar *magnitude_message;

  gdouble *magnitude_buffer;
  
  guint magnitude_message_size;
  guint cache_buffer_size;
  guint length;
  guint padding;
  guint i, j;
  gboolean success;
  
  GValue value = {0,};
  
  static const guchar *mute_message = "/renew\x00\x00,sf\x00/AgsSoundProvider/AgsAudio[\"test-panel\"]/AgsInput[0-1]/AgsFxVolumeChannel[0]/AgsPort[\"./muted[0]\"]:value\x00\x00\x00\x00\x00\x00\x00\x00";
  static const guchar *magnitude_path = "/AgsSoundProvider/AgsAudio[\"test-spectrometer\"]/AgsInput[0-1]/AgsFxAnalyseChannel[0]/AgsPort[\"./magnitude[0]\"]:value";

  static const guint mute_message_size = 128;
  
  osc_connection = ags_osc_connection_new(NULL);
  
  osc_renew_controller = ags_osc_renew_controller_new();

  /* panel */
  message = (guchar *) malloc(mute_message_size * sizeof(guchar));
  memcpy(message, mute_message, mute_message_size * sizeof(guchar));

  ags_osc_buffer_util_put_float(message + mute_message_size - 8,
				1.0);
  
  g_value_init(&value,
	       G_TYPE_FLOAT);

  g_object_get(panel,
	       "input", &channel,
	       NULL);
  success = TRUE;

  for(i = 0; i < 2 && success; i++){
    GList *start_play, *play;
    GList *start_port, *port;

    g_object_get(channel,
		 "play", &start_play,
		 NULL);

    play = ags_recall_template_find_type(start_play,
					 AGS_TYPE_FX_VOLUME_CHANNEL);

    g_object_get(play->data,
		 "port", &start_port,
		 NULL);

    port = ags_port_find_specifier(start_port,
				   "./muted[0]");

    ags_port_safe_read(port->data,
		       &value);

    if(g_value_get_float(&value) == 1.0){
      success = FALSE;

      break;
    }
    
    g_object_get(channel,
		 "next", &channel,
		 NULL);
  }
  
  CU_ASSERT(success);
  
  osc_response = ags_osc_renew_controller_set_data(osc_renew_controller,
						   osc_connection,
						   message, mute_message_size);

  CU_ASSERT(osc_response != NULL);
  
  g_object_get(panel,
	       "input", &channel,
	       NULL);
  success = TRUE;

  for(i = 0; i < 2 && success; i++){
    GList *start_play, *play;
    GList *start_port, *port;

    g_object_get(channel,
		 "play", &start_play,
		 NULL);

    play = ags_recall_template_find_type(start_play,
					 AGS_TYPE_FX_VOLUME_CHANNEL);

    g_object_get(play->data,
		 "port", &start_port,
		 NULL);

    port = ags_port_find_specifier(start_port,
				   "./muted[0]");

    ags_port_safe_read(port->data,
		       &value);

    if(g_value_get_float(&value) != 1.0){
      success = FALSE;

      break;
    }
    
    g_object_get(channel,
		 "next", &channel,
		 NULL);
  }
  
  CU_ASSERT(success);

  /* spectrometer */
  magnitude_message = (guchar *) malloc(AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(guchar));

  cache_buffer_size = ags_soundcard_helper_config_get_buffer_size(ags_config_get_instance());

  memcpy(magnitude_message, "/renew\x00\x00,s[", 11 * sizeof(guchar));
  magnitude_message_size = 11;

  for(i = 0; i < cache_buffer_size; i++){
    magnitude_message[11 + i] = 'd';
  }

  magnitude_message[11 + i] = ']';
  magnitude_message_size += (cache_buffer_size + 1);

  padding = (4 * (guint) ceil((cache_buffer_size + 5) / 4.0)) - (cache_buffer_size + 4);

  for(i = 0; i < padding; i++){
    magnitude_message[magnitude_message_size + i] = '\x00';
  }

  magnitude_message_size += padding;

  length = strlen(magnitude_path);
  memcpy(magnitude_message + magnitude_message_size, magnitude_path, (length) * sizeof(guchar));

  padding = (4 * (guint) ceil((length + 1) / 4.0)) - length;

  magnitude_message_size += length;
  
  for(i = 0; i < padding; i++){
    magnitude_message[magnitude_message_size + i] = '\x00';
  }
  
  magnitude_message_size += padding;
  
  for(i = 0; i < cache_buffer_size; i++){
    ags_osc_buffer_util_put_double(magnitude_message + magnitude_message_size + (i * 8),
				   4.0);
  }

  magnitude_message_size += (cache_buffer_size * 8);

  g_value_unset(&value);
  g_value_init(&value,
	       G_TYPE_POINTER);
  
  magnitude_buffer = (gdouble *) malloc(cache_buffer_size * sizeof(gdouble));
  g_value_set_pointer(&value,
		      magnitude_buffer);
  
  g_object_get(spectrometer,
	       "input", &channel,
	       NULL);
  success = TRUE;

  for(i = 0; i < 2 && success; i++){
    GList *start_play, *play;
    GList *start_port, *port;
    
    g_object_get(channel,
		 "play", &start_play,
		 NULL);

    play = ags_recall_template_find_type(start_play,
					 AGS_TYPE_FX_ANALYSE_CHANNEL);

    g_object_get(play->data,
		 "port", &start_port,
		 NULL);

    port = ags_port_find_specifier(start_port,
				   "./magnitude[0]");
    
    ags_port_safe_read(port->data,
		       &value);

    for(j = 0; j < cache_buffer_size; j++){
      if(magnitude_buffer[j] != 0.0){
	success = FALSE;

	break;
      }
    }
    
    g_object_get(channel,
		 "next", &channel,
		 NULL);
  }
  
  CU_ASSERT(success);
  
  osc_response = ags_osc_renew_controller_set_data(osc_renew_controller,
						   osc_connection,
						   magnitude_message, magnitude_message_size);
  
  g_object_get(spectrometer,
	       "input", &channel,
	       NULL);
  success = TRUE;

  for(i = 0; i < 2 && success; i++){
    GList *start_play, *play;
    GList *start_port, *port;

    g_object_get(channel,
		 "play", &start_play,
		 NULL);

    play = ags_recall_template_find_type(start_play,
					 AGS_TYPE_FX_ANALYSE_CHANNEL);

    g_object_get(play->data,
		 "port", &start_port,
		 NULL);

    port = ags_port_find_specifier(start_port,
				   "./magnitude[0]");

    ags_port_safe_read(port->data,
		       &value);

    for(j = 0; j < cache_buffer_size; j++){
      if(magnitude_buffer[j] != 4.0){
	success = FALSE;
	
	break;
      }
    }
    
    g_object_get(channel,
		 "next", &channel,
		 NULL);
  }
  
  CU_ASSERT(success);
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
  pSuite = CU_add_suite("AgsOscRenewControllerTest", ags_osc_renew_controller_test_init_suite, ags_osc_renew_controller_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsOscRenewController set data", ags_osc_renew_controller_test_set_data) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
