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

int ags_osc_meter_controller_test_init_suite();
int ags_osc_meter_controller_test_clean_suite();

void ags_osc_meter_controller_test_monitor_alloc();
void ags_osc_meter_controller_test_monitor_free();
void ags_osc_meter_controller_test_monitor_ref();
void ags_osc_meter_controller_test_monitor_unref();
void ags_osc_meter_controller_test_find_path();
void ags_osc_meter_controller_test_find_port();
void ags_osc_meter_controller_test_add_monitor();
void ags_osc_meter_controller_test_remove_monitor();
void ags_osc_meter_controller_test_contains_monitor();
void ags_osc_meter_controller_test_start_monitor();
void ags_osc_meter_controller_test_stop_monitor();
void ags_osc_meter_controller_test_monitor_meter();

#define AGS_OSC_METER_CONTROLLER_TEST_CONFIG "[generic]\n" \
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

AgsAudio *drum;

GObject *default_soundcard;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_meter_controller_test_init_suite()
{
  AgsConfig *config;

  GList *start_audio;
  
  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_OSC_METER_CONTROLLER_TEST_CONFIG,
			    strlen(AGS_OSC_METER_CONTROLLER_TEST_CONFIG));

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

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_meter_controller_test_clean_suite()
{
  return(0);
}
  
void
ags_osc_meter_controller_test_monitor_alloc()
{
  AgsOscMeterControllerMonitor *monitor;

  monitor = ags_osc_meter_controller_monitor_alloc();

  CU_ASSERT(monitor != NULL);
  
}

void
ags_osc_meter_controller_test_monitor_free()
{
  AgsOscMeterControllerMonitor *monitor;
  
  monitor = ags_osc_meter_controller_monitor_alloc();
  
  CU_ASSERT(monitor != NULL);

  ags_osc_meter_controller_monitor_free(monitor);

  //TODO:JK: improve me
}

void
ags_osc_meter_controller_test_monitor_ref()
{
  AgsOscMeterControllerMonitor *monitor;
  
  monitor = ags_osc_meter_controller_monitor_alloc();
  
  CU_ASSERT(monitor != NULL);

  ags_osc_meter_controller_monitor_ref(monitor);

  CU_ASSERT(g_atomic_int_get(&(monitor->ref_count)) == 1);
}

void
ags_osc_meter_controller_test_monitor_unref()
{
  AgsOscMeterControllerMonitor *monitor;
  
  monitor = ags_osc_meter_controller_monitor_alloc();
  g_atomic_int_set(&(monitor->ref_count),
		   2);
  
  CU_ASSERT(monitor != NULL);

  ags_osc_meter_controller_monitor_unref(monitor);

  CU_ASSERT(g_atomic_int_get(&(monitor->ref_count)) == 1);
}

void
ags_osc_meter_controller_test_find_path()
{
  AgsOscMeterControllerMonitor **monitor;
  
  GList *start_list, *list;

  guint i;

  monitor = (AgsOscMeterControllerMonitor **) malloc(7 * sizeof(AgsOscMeterControllerMonitor *));

  start_list = NULL;
  
  for(i = 0; i < 7; i++){
    monitor[i] = ags_osc_meter_controller_monitor_alloc();
    start_list = g_list_prepend(start_list,
				monitor[i]);
    
    monitor[i]->path = g_strdup_printf("/test[%d]", i);
  }

  start_list = g_list_reverse(start_list);

  list = ags_osc_meter_controller_monitor_find_path(start_list,
						    "/test[0]");

  CU_ASSERT(list->data == monitor[0]);
  
  list = ags_osc_meter_controller_monitor_find_path(start_list,
						    "/test[3]");

  CU_ASSERT(list->data == monitor[3]);

  list = ags_osc_meter_controller_monitor_find_path(start_list,
						    "/test[6]");

  CU_ASSERT(list->data == monitor[6]);
}

void
ags_osc_meter_controller_test_find_port()
{
  AgsPort **port;
  
  AgsOscMeterControllerMonitor **monitor;
  
  GList *start_list, *list;

  guint i;

  port = (AgsPort **) malloc(7 * sizeof(AgsPort *));

  monitor = (AgsOscMeterControllerMonitor **) malloc(7 * sizeof(AgsOscMeterControllerMonitor *));

  start_list = NULL;
  
  for(i = 0; i < 7; i++){
    port[i] = ags_port_new();
    
    monitor[i] = ags_osc_meter_controller_monitor_alloc();
    start_list = g_list_prepend(start_list,
				monitor[i]);
    
    monitor[i]->port = port[i];
  }

  start_list = g_list_reverse(start_list);

  list = ags_osc_meter_controller_monitor_find_port(start_list,
						    port[0]);

  CU_ASSERT(list->data == monitor[0]);
  
  list = ags_osc_meter_controller_monitor_find_port(start_list,
						    port[3]);

  CU_ASSERT(list->data == monitor[3]);

  list = ags_osc_meter_controller_monitor_find_port(start_list,
						    port[6]);

  CU_ASSERT(list->data == monitor[6]);
}

void
ags_osc_meter_controller_test_add_monitor()
{
  AgsOscConnection *osc_connection;

  AgsOscMeterController *osc_meter_controller;

  AgsOscMeterControllerMonitor *monitor;

  osc_connection = ags_osc_connection_new(NULL);
  osc_meter_controller = ags_osc_meter_controller_new();

  CU_ASSERT(osc_meter_controller->monitor == NULL);
  
  monitor = ags_osc_meter_controller_monitor_alloc();

  monitor->osc_connection = osc_connection;
  monitor->path = g_strdup("./test[0]");
  monitor->port = ags_port_new();
  
  ags_osc_meter_controller_add_monitor(osc_meter_controller,
				       monitor);

  CU_ASSERT(osc_meter_controller->monitor->data == monitor);

  ags_osc_meter_controller_add_monitor(osc_meter_controller,
				       monitor);

  CU_ASSERT(g_list_length(osc_meter_controller->monitor) == 1);
}

void
ags_osc_meter_controller_test_remove_monitor()
{
  AgsOscConnection *osc_connection;

  AgsOscMeterController *osc_meter_controller;

  AgsOscMeterControllerMonitor *monitor;

  osc_connection = ags_osc_connection_new(NULL);
  osc_meter_controller = ags_osc_meter_controller_new();

  CU_ASSERT(osc_meter_controller->monitor == NULL);
  
  monitor = ags_osc_meter_controller_monitor_alloc();

  monitor->osc_connection = osc_connection;
  monitor->path = g_strdup("./test[0]");
  monitor->port = ags_port_new();
  
  osc_meter_controller->monitor = g_list_prepend(osc_meter_controller->monitor,
						 monitor);

  ags_osc_meter_controller_remove_monitor(osc_meter_controller,
					  monitor);

  CU_ASSERT(osc_meter_controller->monitor == NULL);
}

void
ags_osc_meter_controller_test_contains_monitor()
{
  AgsOscConnection *osc_connection;

  AgsOscMeterController *osc_meter_controller;

  AgsOscMeterControllerMonitor *monitor;

  osc_connection = ags_osc_connection_new(NULL);
  osc_meter_controller = ags_osc_meter_controller_new();

  CU_ASSERT(osc_meter_controller->monitor == NULL);
  
  monitor = ags_osc_meter_controller_monitor_alloc();

  monitor->osc_connection = osc_connection;
  monitor->path = g_strdup("./test[0]");
  monitor->port = ags_port_new();
  
  osc_meter_controller->monitor = g_list_prepend(osc_meter_controller->monitor,
						 monitor);

  CU_ASSERT(ags_osc_meter_controller_contains_monitor(osc_meter_controller, monitor->osc_connection, monitor->port) == TRUE);

  CU_ASSERT(ags_osc_meter_controller_contains_monitor(osc_meter_controller, monitor->osc_connection, ags_port_new()) == FALSE);
  CU_ASSERT(ags_osc_meter_controller_contains_monitor(osc_meter_controller, ags_osc_connection_new(NULL), monitor->port) == FALSE);

  CU_ASSERT(ags_osc_meter_controller_contains_monitor(osc_meter_controller, ags_osc_connection_new(NULL), ags_port_new()) == FALSE);

  CU_ASSERT(ags_osc_meter_controller_contains_monitor(osc_meter_controller, NULL, NULL) == FALSE);
}

void
ags_osc_meter_controller_test_start_monitor()
{
  AgsOscMeterController *osc_meter_controller;
  
  struct timespec delay;

  guint i;
  
  osc_meter_controller = ags_osc_meter_controller_new();

  ags_osc_meter_controller_start_monitor(osc_meter_controller);

  CU_ASSERT(ags_osc_meter_controller_test_flags(osc_meter_controller, AGS_OSC_METER_CONTROLLER_MONITOR_STARTED) == TRUE);

  delay.tv_sec = 0;
  delay.tv_nsec = AGS_NSEC_PER_SEC / 30;

  i = 0;
  
  while(i < 3334 &&
       !ags_osc_meter_controller_test_flags(osc_meter_controller, AGS_OSC_METER_CONTROLLER_MONITOR_RUNNING)){
    nanosleep(&delay, NULL);
    i++;
  }

  CU_ASSERT(ags_osc_meter_controller_test_flags(osc_meter_controller, AGS_OSC_METER_CONTROLLER_MONITOR_RUNNING) == TRUE);
}

void
ags_osc_meter_controller_test_stop_monitor()
{
  AgsOscMeterController *osc_meter_controller;
  
  struct timespec delay;

  guint i;
  
  osc_meter_controller = ags_osc_meter_controller_new();

  ags_osc_meter_controller_start_monitor(osc_meter_controller);

  CU_ASSERT(ags_osc_meter_controller_test_flags(osc_meter_controller, AGS_OSC_METER_CONTROLLER_MONITOR_STARTED) == TRUE);

  delay.tv_sec = 0;
  delay.tv_nsec = AGS_NSEC_PER_SEC / 30;

  i = 0;
  
  while(i < 3334 &&
       !ags_osc_meter_controller_test_flags(osc_meter_controller, AGS_OSC_METER_CONTROLLER_MONITOR_RUNNING)){
    nanosleep(&delay, NULL);
    i++;
  }

  CU_ASSERT(ags_osc_meter_controller_test_flags(osc_meter_controller, AGS_OSC_METER_CONTROLLER_MONITOR_RUNNING) == TRUE);

  ags_osc_meter_controller_stop_monitor(osc_meter_controller);

  CU_ASSERT(ags_osc_meter_controller_test_flags(osc_meter_controller, AGS_OSC_METER_CONTROLLER_MONITOR_RUNNING) == FALSE);
  CU_ASSERT(ags_osc_meter_controller_test_flags(osc_meter_controller, AGS_OSC_METER_CONTROLLER_MONITOR_STARTED) == FALSE);
}

void
ags_osc_meter_controller_test_monitor_meter()
{
  AgsOscConnection *osc_connection;

  AgsOscMeterController *osc_meter_controller;

  GList *osc_response;

  static const unsigned char *enable_peak_message = "/meter\x00\x00,sT\x00/AgsSoundProvider/AgsAudio[\"test-drum\"]/AgsInput[0-15]/AgsPeakChannel[0]/AgsPort[\"./peak[0]\"]:value\x00";
  static const unsigned char *disable_peak_message = "/meter\x00\x00,sF\x00/AgsSoundProvider/AgsAudio[\"test-drum\"]/AgsInput[0-15]/AgsPeakChannel[0]/AgsPort[\"./peak[0]\"]:value\x00";

  static const guint enable_peak_message_size = 112;
  static const guint disable_peak_message_size = 112;

  osc_connection = ags_osc_connection_new(NULL);
  
  osc_meter_controller = ags_osc_meter_controller_new();

  /* drum */
  osc_response = ags_osc_meter_controller_monitor_meter(osc_meter_controller,
							osc_connection,
							enable_peak_message, enable_peak_message_size);

  CU_ASSERT(osc_response != NULL);

  osc_response = ags_osc_meter_controller_monitor_meter(osc_meter_controller,
							osc_connection,
							disable_peak_message, disable_peak_message_size);

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
  pSuite = CU_add_suite("AgsOscMeterControllerTest", ags_osc_meter_controller_test_init_suite, ags_osc_meter_controller_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsOscMeterController monitor alloc", ags_osc_meter_controller_test_monitor_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscMeterController monitor free", ags_osc_meter_controller_test_monitor_free) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscMeterController monitor ref", ags_osc_meter_controller_test_monitor_ref) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscMeterController monitor unref", ags_osc_meter_controller_test_monitor_unref) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscMeterController find path", ags_osc_meter_controller_test_find_path) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscMeterController find port", ags_osc_meter_controller_test_find_port) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscMeterController add monitor", ags_osc_meter_controller_test_add_monitor) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscMeterController remove monitor", ags_osc_meter_controller_test_remove_monitor) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscMeterController contains monitor", ags_osc_meter_controller_test_contains_monitor) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscMeterController start monitor", ags_osc_meter_controller_test_start_monitor) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscMeterController stop monitor", ags_osc_meter_controller_test_stop_monitor) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscMeterController monitor meter", ags_osc_meter_controller_test_monitor_meter) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
