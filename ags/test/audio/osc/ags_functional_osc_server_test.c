/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifdef __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

gpointer ags_functional_osc_server_test_add_thread(gpointer data);

int ags_functional_osc_server_test_init_suite();
int ags_functional_osc_server_test_clean_suite();

void ags_functional_osc_server_test_action_controller();
void ags_functional_osc_server_test_config_controller();
void ags_functional_osc_server_test_info_controller();
void ags_functional_osc_server_test_meter_controller();
void ags_functional_osc_server_test_node_controller();
void ags_functional_osc_server_test_renew_controller();
void ags_functional_osc_server_test_status_controller();

#define AGS_FUNCTIONAL_OSC_SERVER_TEST_CONFIG "[generic]\n"	\
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
  "thread-pool-max-unused-threads=8\n"				\
  "max-precision=125\n"						\
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
  "[sequencer-0]\n"						\
  "backend=alsa\n"						\
  "device=default\n"						\
  "\n"								\
  "[recall]\n"							\
  "auto-sense=true\n"						\
  "\n"

#define AGS_FUNCTIONAL_OSC_SERVER_TEST_APPLY_CONFIG_ARGUMENT "[generic]\n" \
  "autosave-thread=false\n"						\
  "simple-file=false\n"							\
  "disable-feature=experimental\n"					\
  "segmentation=4/4\n"							\
  "\n"									\
  "[thread]\n"								\
  "model=super-threaded\n"						\
  "super-threaded-scope=channel\n"					\
  "lock-global=ags-thread\n"						\
  "lock-parent=ags-recycling-thread\n"					\
  "thread-pool-max-unused-threads=8\n"					\
  "max-precision=125\n"							\
  "\n"									\
  "[soundcard-0]\n"							\
  "backend=alsa\n"							\
  "device=default\n"							\
  "samplerate=44100\n"							\
  "buffer-size=256\n"							\
  "pcm-channels=2\n"							\
  "dsp-channels=2\n"							\
  "format=16\n"								\
  "\n"									\
  "[recall]\n"								\
  "auto-sense=true\n"							\
  "\n"

#define AGS_FUNCTIONAL_OSC_SERVER_TEST_METER_PACKET_COUNT (16 * 30)

GThread *add_thread = NULL;

AgsApplicationContext *application_context;

AgsAudio *drum;

AgsOscServer *osc_server;
AgsOscClient *osc_client;

GObject *default_soundcard;

gpointer
ags_functional_osc_server_test_add_thread(gpointer data)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C");
  putenv("LANG=C");

  putenv("LADSPA_PATH=\"\"");
  putenv("DSSI_PATH=\"\"");
  putenv("LV2_PATH=\"\"");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    exit(CU_get_error());
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFunctionalOscServerTest", ags_functional_osc_server_test_init_suite, ags_functional_osc_server_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    exit(CU_get_error());
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
      
    exit(CU_get_error());
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  exit(CU_get_error());
}

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

  /* ags-mute */
  ags_recall_factory_create(drum,
			    NULL, NULL,
			    "ags-mute",
			    0, 2,
			    0, 8,
			    (AGS_RECALL_FACTORY_INPUT,
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

  ags_connectable_connect(AGS_CONNECTABLE(drum));
  
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
  guchar *buffer;
  guchar *packet;

  guint buffer_length;
  gboolean retval;
    
  static const guchar *start_soundcard_message = "/action\x00,ss\x00/AgsSoundProvider/AgsSoundcard\x00\x00start\x00\x00\x00";
  static const guchar *start_sequencer_message = "/action\x00,ss\x00/AgsSoundProvider/AgsSequencer\x00\x00start\x00\x00\x00";
  static const guchar *start_audio_message =     "/action\x00,ss\x00/AgsSoundProvider/AgsAudio[0]\x00\x00\x00start\x00\x00\x00";
  static const guchar *stop_soundcard_message = "/action\x00,ss\x00/AgsSoundProvider/AgsSoundcard\x00\x00stop\x00\x00\x00\x00";
  static const guchar *stop_sequencer_message = "/action\x00,ss\x00/AgsSoundProvider/AgsSequencer\x00\x00stop\x00\x00\x00\x00";
  static const guchar *stop_audio_message =     "/action\x00,ss\x00/AgsSoundProvider/AgsAudio[0]\x00\x00\x00stop\x00\x00\x00\x00";

  static const guint start_soundcard_message_size = 52;
  static const guint start_sequencer_message_size = 52;
  static const guint start_audio_message_size = 52;
  static const guint stop_soundcard_message_size = 52;
  static const guint stop_sequencer_message_size = 52;
  static const guint stop_audio_message_size = 52;

  CU_ASSERT(osc_server->ip4_fd != -1);
  CU_ASSERT(osc_client->ip4_fd != -1);
  
  /* start soundcard */
  packet = (guchar *) malloc((4 + start_soundcard_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				start_soundcard_message_size);
  memcpy(packet + 4, start_soundcard_message, (start_soundcard_message_size) * sizeof(guchar));

  buffer = ags_osc_util_slip_encode(packet,
				    4 + start_soundcard_message_size,
				    &buffer_length);

  CU_ASSERT(buffer_length - 2 >= 4 + start_soundcard_message_size);

  retval = ags_osc_client_write_bytes(osc_client,
				      buffer, buffer_length);

  CU_ASSERT(retval == TRUE);

  sleep(5);
  
  /* start sequencer */
  packet = (guchar *) malloc((4 + start_sequencer_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				start_sequencer_message_size);
  memcpy(packet + 4, start_sequencer_message, (start_sequencer_message_size) * sizeof(guchar));

  buffer = ags_osc_util_slip_encode(packet,
				    4 + start_sequencer_message_size,
				    &buffer_length);

  CU_ASSERT(buffer_length - 2 >= 4 + start_sequencer_message_size);

  retval = ags_osc_client_write_bytes(osc_client,
				      buffer, buffer_length);

  CU_ASSERT(retval == TRUE);

  sleep(5);
  
  /* start audio */
  packet = (guchar *) malloc((4 + start_audio_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				start_audio_message_size);
  memcpy(packet + 4, start_audio_message, (start_audio_message_size) * sizeof(guchar));

  buffer = ags_osc_util_slip_encode(packet,
				    4 + start_audio_message_size,
				    &buffer_length);

  CU_ASSERT(buffer_length - 2 >= 4 + start_audio_message_size);

  retval = ags_osc_client_write_bytes(osc_client,
				      buffer, buffer_length);

  CU_ASSERT(retval == TRUE);

  sleep(5);

  /* stop soundcard */
  packet = (guchar *) malloc((4 + stop_soundcard_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				stop_soundcard_message_size);
  memcpy(packet + 4, stop_soundcard_message, (stop_soundcard_message_size) * sizeof(guchar));

  buffer = ags_osc_util_slip_encode(packet,
				    4 + stop_soundcard_message_size,
				    &buffer_length);

  CU_ASSERT(buffer_length - 2 >= 4 + stop_soundcard_message_size);

  retval = ags_osc_client_write_bytes(osc_client,
				      buffer, buffer_length);

  CU_ASSERT(retval == TRUE);

  sleep(5);
  
  /* stop sequencer */
  packet = (guchar *) malloc((4 + stop_sequencer_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				stop_sequencer_message_size);
  memcpy(packet + 4, stop_sequencer_message, (stop_sequencer_message_size) * sizeof(guchar));

  buffer = ags_osc_util_slip_encode(packet,
				    4 + stop_sequencer_message_size,
				    &buffer_length);

  CU_ASSERT(buffer_length - 2 >= 4 + stop_sequencer_message_size);

  retval = ags_osc_client_write_bytes(osc_client,
				      buffer, buffer_length);

  CU_ASSERT(retval == TRUE);

  sleep(5);
  
  /* stop audio */
  packet = (guchar *) malloc((4 + stop_audio_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				stop_audio_message_size);
  memcpy(packet + 4, stop_audio_message, (stop_audio_message_size) * sizeof(guchar));

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
  guchar *message;
  guchar *buffer;
  guchar *packet;

  guint config_length;
  guint length;
  guint buffer_length;
  gboolean retval;

  static const guchar *config_message = "/config\x00,s\x00\x00";

  static const guint config_message_size = 12;

  CU_ASSERT(osc_server->ip4_fd != -1);
  CU_ASSERT(osc_client->ip4_fd != -1);

  /* apply config */
  config_length = strlen(AGS_FUNCTIONAL_OSC_SERVER_TEST_APPLY_CONFIG_ARGUMENT);

  length = config_message_size + (4 * ceil((double) (config_length + 1) / 4.0));

  message = (guchar *) malloc(length * sizeof(guchar));
  memset(message, 0, length * sizeof(guchar));

  memcpy(message, config_message, config_message_size * sizeof(guchar));
  memcpy(message + config_message_size, AGS_FUNCTIONAL_OSC_SERVER_TEST_APPLY_CONFIG_ARGUMENT, config_length * sizeof(guchar));

  packet = (guchar *) malloc((4 + length) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				length);
  memcpy(packet + 4, message, (length) * sizeof(guchar));

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
  guchar *buffer;
  guchar *packet;

  guint buffer_length;
  gboolean retval;

  static const guchar *info_message = "/info\x00\x00\x00";

  static const guint info_message_size = 8;

  CU_ASSERT(osc_server->ip4_fd != -1);
  CU_ASSERT(osc_client->ip4_fd != -1);

  packet = (guchar *) malloc((4 + info_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				info_message_size);
  memcpy(packet + 4, info_message, (info_message_size) * sizeof(guchar));

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
  struct timespec start_time;
  struct timespec timeout_delay;
  struct timespec idle_delay;
  
  guchar *buffer;
  guchar *packet;

  guint buffer_length;
  guint meter_packet_count;
  guint i;
  gboolean retval;

  static const guchar *enable_peak_message = "/meter\x00\x00,sT\x00/AgsSoundProvider/AgsAudio[\"test-drum\"]/AgsInput[0-15]/AgsPeakChannel[0]/AgsPort[\"./peak[0]\"]:value\x00";
  static const guchar *disable_peak_message = "/meter\x00\x00,sF\x00/AgsSoundProvider/AgsAudio[\"test-drum\"]/AgsInput[0-15]/AgsPeakChannel[0]/AgsPort[\"./peak[0]\"]:value\x00";

  static const guint enable_peak_message_size = 112;
  static const guint disable_peak_message_size = 112;

  CU_ASSERT(osc_server->ip4_fd != -1);
  CU_ASSERT(osc_client->ip4_fd != -1);

  /* enable meter */
  packet = (guchar *) malloc((4 + enable_peak_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				enable_peak_message_size);
  memcpy(packet + 4, enable_peak_message, (enable_peak_message_size) * sizeof(guchar));

  buffer = ags_osc_util_slip_encode(packet,
				    4 + enable_peak_message_size,
				    &buffer_length);

  CU_ASSERT(buffer_length - 2 >= 4 + enable_peak_message_size);

  retval = ags_osc_client_write_bytes(osc_client,
				      buffer, buffer_length);

  CU_ASSERT(retval == TRUE);

  /* read packets */
#ifdef __APPLE__
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
      
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
      
  start_time.tv_sec = mts.tv_sec;
  start_time.tv_nsec = mts.tv_nsec;
#else
  clock_gettime(CLOCK_MONOTONIC, &start_time);
#endif

  timeout_delay.tv_sec = 180;
  timeout_delay.tv_nsec = 0;
  
  idle_delay.tv_sec = 0;
  idle_delay.tv_nsec = NSEC_PER_SEC / 30;
  
  meter_packet_count = 0;
  i = 0;
  
  while(!ags_time_timeout_expired(&start_time,
				  &timeout_delay)){
    guchar *current_data;
    guchar *current_packet;
    gchar *address_pattern;
    
    guint data_length;
    
    current_data = ags_osc_client_read_bytes(osc_client,
					     &data_length);

    if(current_data != NULL){
      current_packet = ags_osc_util_slip_decode(current_data,
						data_length,
						NULL);

      if(data_length >= 8){
	ags_osc_buffer_util_get_message(current_packet + 4,
					&address_pattern, NULL);

	if(!g_strcmp0(address_pattern,
		      "/meter")){
	  meter_packet_count++;
	
	  i++;
	}
      }

      free(current_packet);
    }
    
    if(i == 16){
      i = 0;
      
      nanosleep(&idle_delay,
		NULL);
    }
  }
  
  CU_ASSERT(meter_packet_count >= AGS_FUNCTIONAL_OSC_SERVER_TEST_METER_PACKET_COUNT);

  /* disable meter */
  packet = (guchar *) malloc((4 + disable_peak_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				disable_peak_message_size);
  memcpy(packet + 4, disable_peak_message, (disable_peak_message_size) * sizeof(guchar));

  buffer = ags_osc_util_slip_encode(packet,
				    4 + disable_peak_message_size,
				    &buffer_length);

  CU_ASSERT(buffer_length - 2 >= 4 + disable_peak_message_size);

  retval = ags_osc_client_write_bytes(osc_client,
				      buffer, buffer_length);

  CU_ASSERT(retval == TRUE);
}

void
ags_functional_osc_server_test_node_controller()
{
  struct timespec start_time;
  struct timespec timeout_delay;

  guchar *buffer;
  guchar *packet;

  guint buffer_length;
  gboolean retval;
  gboolean success;
  
  static const guchar *volume_message = "/node\x00\x00\x00,s\x00\x00/AgsSoundProvider/AgsAudio[\"test-drum\"]/AgsInput[0-1]/AgsVolumeChannel[0]/AgsPort[\"./volume[0]\"]:value\x00\x00";

  static const guint volume_message_size = 116;

  CU_ASSERT(osc_server->ip4_fd != -1);
  CU_ASSERT(osc_client->ip4_fd != -1);

  /* volume message */
  packet = (guchar *) malloc((4 + volume_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				volume_message_size);
  memcpy(packet + 4, volume_message, (volume_message_size) * sizeof(guchar));

  buffer = ags_osc_util_slip_encode(packet,
				    4 + volume_message_size,
				    &buffer_length);

  CU_ASSERT(buffer_length - 2 >= 4 + volume_message_size);

  retval = ags_osc_client_write_bytes(osc_client,
				      buffer, buffer_length);

  CU_ASSERT(retval == TRUE);

  /* read volume */
#ifdef __APPLE__
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
      
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
      
  start_time.tv_sec = mts.tv_sec;
  start_time.tv_nsec = mts.tv_nsec;
#else
  clock_gettime(CLOCK_MONOTONIC, &start_time);
#endif

  timeout_delay.tv_sec = 10;
  timeout_delay.tv_nsec = 0;

  success = FALSE;
  
  while(!ags_time_timeout_expired(&start_time,
				  &timeout_delay)){
    guchar *current_data;
    guchar *current_packet;
    gchar *address_pattern;
    
    guint data_length;
    
    current_data = ags_osc_client_read_bytes(osc_client,
					     &data_length);

    if(current_data != NULL){
      current_packet = ags_osc_util_slip_decode(current_data,
						data_length,
						NULL);

      if(data_length >= 8){
	ags_osc_buffer_util_get_message(current_packet + 4,
					&address_pattern, NULL);

	if(!g_strcmp0(address_pattern,
		      "/node")){
	  success = TRUE;

	  break;
	}
      }
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_functional_osc_server_test_renew_controller()
{
  guchar *buffer;
  guchar *packet;

  guint buffer_length;
  gboolean retval;

  static const guchar *mute_message = "/renew\x00\x00,sf\x00/AgsSoundProvider/AgsAudio[\"test-panel\"]/AgsInput[0-1]/AgsMuteChannel[0]/AgsPort[\"./muted[0]\"]:value\x00\x00\x00\x00\x00\x00";

  static const guint mute_message_size = 120;

  CU_ASSERT(osc_server->ip4_fd != -1);
  CU_ASSERT(osc_client->ip4_fd != -1);

  /* mute message */
  packet = (guchar *) malloc((4 + mute_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				mute_message_size);
  memcpy(packet + 4, mute_message, (mute_message_size) * sizeof(guchar));

  buffer = ags_osc_util_slip_encode(packet,
				    4 + mute_message_size,
				    &buffer_length);

  CU_ASSERT(buffer_length - 2 >= 4 + mute_message_size);

  retval = ags_osc_client_write_bytes(osc_client,
				      buffer, buffer_length);

  CU_ASSERT(retval == TRUE);
}

void
ags_functional_osc_server_test_status_controller()
{
  guchar *buffer;
  guchar *packet;

  guint buffer_length;
  gboolean retval;

  static const guchar *status_message = "/status\x00,\x00\x00\x00";

  static const guint status_message_size = 12;

  CU_ASSERT(osc_server->ip4_fd != -1);
  CU_ASSERT(osc_client->ip4_fd != -1);

  /* status message */
  packet = (guchar *) malloc((4 + status_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				status_message_size);
  memcpy(packet + 4, status_message, (status_message_size) * sizeof(guchar));

  buffer = ags_osc_util_slip_encode(packet,
				    4 + status_message_size,
				    &buffer_length);

  CU_ASSERT(buffer_length - 2 >= 4 + status_message_size);

  retval = ags_osc_client_write_bytes(osc_client,
				      buffer, buffer_length);

  CU_ASSERT(retval == TRUE);
}

int
main(int argc, char **argv)
{
  add_thread = g_thread_new("libags_audio.so - functional OSC server test",
			    ags_functional_osc_server_test_add_thread,
			    NULL);

  g_main_loop_run(g_main_loop_new(g_main_context_default(),
				  FALSE));
  
  g_thread_join(add_thread);
  
  return(-1);
}
