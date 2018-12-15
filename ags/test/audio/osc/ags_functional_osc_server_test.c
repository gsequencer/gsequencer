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
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

int ags_functional_osc_server_test_init_suite();
int ags_functional_osc_server_test_clean_suite();

void ags_functional_osc_server_test_action_controller();
void ags_functional_osc_server_test_config_controller();
void ags_functional_osc_server_test_info_controller();
void ags_functional_osc_server_test_meter_controller();
void ags_functional_osc_server_test_node_controller();
void ags_functional_osc_server_test_renew_controller();
void ags_functional_osc_server_test_status_controller();

#define AGS_FUNCTIONAL_OSC_SERVER_TEST_CONFIG "[generic]\n" \
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

#define AGS_FUNCTIONAL_OSC_SERVER_TEST_APPLY_CONFIG_ARGUMENT "[generic]\n" \
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
  "buffer-size=256\n"				       \
  "pcm-channels=2\n"				       \
  "dsp-channels=2\n"				       \
  "format=16\n"					       \
  "\n"						       \
  "[recall]\n"					       \
  "auto-sense=true\n"				       \
  "\n"

AgsApplicationContext *application_context;

AgsAudio *drum;

AgsOscServer *osc_server;
AgsOscClient *osc_client;

GObject *default_soundcard;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_osc_server_test_init_suite()
{
  AgsConfig *config;

  GList *start_audio;
  
  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_FUNCTIONAL_OSC_SERVER_TEST_CONFIG,
			    strlen(AGS_FUNCTIONAL_OSC_SERVER_TEST_CONFIG));

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

  /* ags-peak */
  ags_recall_factory_create(drum,
			    NULL, NULL,
			    "ags-peak",
			    0, 2,
			    0, 8,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* OSC server */
  signal(SIGPIPE, SIG_IGN);
  
  osc_server = ags_osc_server_new();
  ags_osc_server_set_flags(osc_server,
			   (AGS_OSC_SERVER_INET4 |
			    AGS_OSC_SERVER_TCP));

  ags_osc_server_add_default_controller(osc_server);
  
  ags_osc_server_start(osc_server);

  sleep(5);
  
  /* OSC client */
  osc_client = ags_osc_client_new();
  ags_osc_client_set_flags(osc_client,
			   (AGS_OSC_CLIENT_INET4 |
			    AGS_OSC_CLIENT_TCP));

  ags_osc_client_connect(osc_client);

  sleep(5);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_osc_server_test_clean_suite()
{
  close(osc_client->ip4_fd);

  ags_osc_server_stop(osc_server);

  return(0);
}

void
ags_functional_osc_server_test_action_controller()
{
  unsigned char *buffer;
  unsigned char *packet;

  guint buffer_length;
  gboolean retval;
    
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

  CU_ASSERT(osc_server->ip4_fd != -1);
  CU_ASSERT(osc_client->ip4_fd != -1);
  
  /* start soundcard */
  packet = (unsigned char *) malloc((4 + start_soundcard_message_size) * sizeof(unsigned char));

  ags_osc_buffer_util_put_int32(packet,
				start_soundcard_message_size);
  memcpy(packet + 4, start_soundcard_message, (start_soundcard_message_size) * sizeof(unsigned char));

  buffer = ags_osc_util_slip_encode(packet,
				    4 + start_soundcard_message_size,
				    &buffer_length);

  CU_ASSERT(buffer_length - 2 >= 4 + start_soundcard_message_size);

  retval = ags_osc_client_write_bytes(osc_client,
				      buffer, buffer_length);

  CU_ASSERT(retval == TRUE);

  sleep(5);
  
  /* start sequencer */
  packet = (unsigned char *) malloc((4 + start_sequencer_message_size) * sizeof(unsigned char));

  ags_osc_buffer_util_put_int32(packet,
				start_sequencer_message_size);
  memcpy(packet + 4, start_sequencer_message, (start_sequencer_message_size) * sizeof(unsigned char));

  buffer = ags_osc_util_slip_encode(packet,
				    4 + start_sequencer_message_size,
				    &buffer_length);

  CU_ASSERT(buffer_length - 2 >= 4 + start_sequencer_message_size);

  retval = ags_osc_client_write_bytes(osc_client,
				      buffer, buffer_length);

  CU_ASSERT(retval == TRUE);

  sleep(5);
  
  /* start audio */
  packet = (unsigned char *) malloc((4 + start_audio_message_size) * sizeof(unsigned char));

  ags_osc_buffer_util_put_int32(packet,
				start_audio_message_size);
  memcpy(packet + 4, start_audio_message, (start_audio_message_size) * sizeof(unsigned char));

  buffer = ags_osc_util_slip_encode(packet,
				    4 + start_audio_message_size,
				    &buffer_length);

  CU_ASSERT(buffer_length - 2 >= 4 + start_audio_message_size);

  retval = ags_osc_client_write_bytes(osc_client,
				      buffer, buffer_length);

  CU_ASSERT(retval == TRUE);

  sleep(5);

  /* stop soundcard */
  packet = (unsigned char *) malloc((4 + stop_soundcard_message_size) * sizeof(unsigned char));

  ags_osc_buffer_util_put_int32(packet,
				stop_soundcard_message_size);
  memcpy(packet + 4, stop_soundcard_message, (stop_soundcard_message_size) * sizeof(unsigned char));

  buffer = ags_osc_util_slip_encode(packet,
				    4 + stop_soundcard_message_size,
				    &buffer_length);

  CU_ASSERT(buffer_length - 2 >= 4 + stop_soundcard_message_size);

  retval = ags_osc_client_write_bytes(osc_client,
				      buffer, buffer_length);

  CU_ASSERT(retval == TRUE);

  sleep(5);
  
  /* stop sequencer */
  packet = (unsigned char *) malloc((4 + stop_sequencer_message_size) * sizeof(unsigned char));

  ags_osc_buffer_util_put_int32(packet,
				stop_sequencer_message_size);
  memcpy(packet + 4, stop_sequencer_message, (stop_sequencer_message_size) * sizeof(unsigned char));

  buffer = ags_osc_util_slip_encode(packet,
				    4 + stop_sequencer_message_size,
				    &buffer_length);

  CU_ASSERT(buffer_length - 2 >= 4 + stop_sequencer_message_size);

  retval = ags_osc_client_write_bytes(osc_client,
				      buffer, buffer_length);

  CU_ASSERT(retval == TRUE);

  sleep(5);
  
  /* stop audio */
  packet = (unsigned char *) malloc((4 + stop_audio_message_size) * sizeof(unsigned char));

  ags_osc_buffer_util_put_int32(packet,
				stop_audio_message_size);
  memcpy(packet + 4, stop_audio_message, (stop_audio_message_size) * sizeof(unsigned char));

  buffer = ags_osc_util_slip_encode(packet,
				    4 + stop_audio_message_size,
				    &buffer_length);

  CU_ASSERT(buffer_length - 2 >= 4 + stop_audio_message_size);

  retval = ags_osc_client_write_bytes(osc_client,
				      buffer, buffer_length);

  CU_ASSERT(retval == TRUE);

  sleep(5);
}

void
ags_functional_osc_server_test_config_controller()
{
  unsigned char *message;
  unsigned char *buffer;
  unsigned char *packet;

  guint config_length;
  guint length;
  guint buffer_length;
  gboolean retval;

  static const unsigned char *config_message = "/config\x00,s\x00\x00";

  static const guint config_message_size = 12;

  CU_ASSERT(osc_server->ip4_fd != -1);
  CU_ASSERT(osc_client->ip4_fd != -1);

  /* apply config */
  config_length = strlen(AGS_FUNCTIONAL_OSC_SERVER_TEST_APPLY_CONFIG_ARGUMENT);

  length = config_message_size + (4 * ceil((double) (config_length + 1) / 4.0));

  message = (unsigned char *) malloc(length * sizeof(unsigned char));
  memset(message, 0, length * sizeof(unsigned char));

  memcpy(message, config_message, config_message_size * sizeof(unsigned char));
  memcpy(message + config_message_size, AGS_FUNCTIONAL_OSC_SERVER_TEST_APPLY_CONFIG_ARGUMENT, config_length * sizeof(unsigned char));

  packet = (unsigned char *) malloc((4 + length) * sizeof(unsigned char));

  ags_osc_buffer_util_put_int32(packet,
				length);
  memcpy(packet + 4, message, (length) * sizeof(unsigned char));

  buffer = ags_osc_util_slip_encode(packet,
				    4 + length,
				    &buffer_length);

  CU_ASSERT(buffer_length - 2 >= 4 + length);

  retval = ags_osc_client_write_bytes(osc_client,
				      buffer, buffer_length);

  CU_ASSERT(retval == TRUE);

  sleep(5);
}

void
ags_functional_osc_server_test_info_controller()
{
  unsigned char *buffer;
  unsigned char *packet;

  guint buffer_length;
  gboolean retval;

  static const unsigned char *info_message = "/info\x00\x00\x00";

  static const guint info_message_size = 8;

  CU_ASSERT(osc_server->ip4_fd != -1);
  CU_ASSERT(osc_client->ip4_fd != -1);

  packet = (unsigned char *) malloc((4 + info_message_size) * sizeof(unsigned char));

  ags_osc_buffer_util_put_int32(packet,
				info_message_size);
  memcpy(packet + 4, info_message, (info_message_size) * sizeof(unsigned char));

  buffer = ags_osc_util_slip_encode(packet,
				    4 + info_message_size,
				    &buffer_length);

  CU_ASSERT(buffer_length - 2 >= 4 + info_message_size);

  retval = ags_osc_client_write_bytes(osc_client,
				      buffer, buffer_length);

  CU_ASSERT(retval == TRUE);

  sleep(5);
}

void
ags_functional_osc_server_test_meter_controller()
{
  //TODO:JK: implement me
}

void
ags_functional_osc_server_test_node_controller()
{
  //TODO:JK: implement me
}

void
ags_functional_osc_server_test_renew_controller()
{
  //TODO:JK: implement me
}

void
ags_functional_osc_server_test_status_controller()
{
  //TODO:JK: implement me
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
  pSuite = CU_add_suite("AgsFunctionalOscServerTest", ags_functional_osc_server_test_init_suite, ags_functional_osc_server_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsOscServer providing action controller", ags_functional_osc_server_test_action_controller) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscServer providing config controller", ags_functional_osc_server_test_config_controller) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscServer providing info controller", ags_functional_osc_server_test_info_controller) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscServer providing meter controller", ags_functional_osc_server_test_meter_controller) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscServer providing node controller", ags_functional_osc_server_test_node_controller) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscServer providing renew controller", ags_functional_osc_server_test_renew_controller) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscServer providing status controller", ags_functional_osc_server_test_status_controller) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

