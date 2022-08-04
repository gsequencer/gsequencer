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
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/config.h>

#include <ags/gsequencer_main.h>

#include <ags/test/app/libgsequencer.h>

#include "ags_functional_test_util.h"

void ags_functional_loop_007_xml_test_add_test();

int ags_functional_loop_007_xml_test_init_suite();
int ags_functional_loop_007_xml_test_clean_suite();

void ags_functional_loop_007_xml_test_file_setup();

#define AGS_FUNCTIONAL_LOOP_007_XML_TEST_DEFAULT_IDLE_TIME (3.0 * G_USEC_PER_SEC)

#define AGS_FUNCTIONAL_LOOP_007_XML_TEST_FILE_SETUP_PLAYBACK_COUNT (1)
#define AGS_FUNCTIONAL_LOOP_007_XML_TEST_FILE_SETUP_PLAYBACK_DURATION (120)

#define AGS_FUNCTIONAL_LOOP_007_XML_TEST_FILE_SETUP_FILENAME AGS_SRC_DIR "/" "ags/test/app/examples/ags-loop-007.xml"

#define AGS_FUNCTIONAL_LOOP_007_XML_TEST_CONFIG "[generic]\n" \
  "autosave-thread=false\n"			       \
  "simple-file=true\n"				       \
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

CU_pSuite pSuite = NULL;
volatile gboolean is_available;
volatile gboolean is_terminated;

AgsGSequencerApplicationContext *gsequencer_application_context;

struct timespec ags_functional_loop_007_xml_test_default_timeout = {
  300,
  0,
};

void
ags_functional_loop_007_xml_test_add_test()
{
  /* add the tests to the suite */
  if((CU_add_test(pSuite, "functional test of GSequencer setup by file and play it", ags_functional_loop_007_xml_test_file_setup) == NULL)){
    CU_cleanup_registry();
      
    exit(CU_get_error());
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  ags_functional_test_util_quit();
  
  while(!g_atomic_int_get(&is_terminated)){
    g_usleep(G_USEC_PER_SEC / 60);
  }

  CU_cleanup_registry();
  
  exit(CU_get_error());
}

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_loop_007_xml_test_init_suite()
{
  gsequencer_application_context = ags_application_context_get_instance();

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_realized),
						      &ags_functional_loop_007_xml_test_default_timeout,
						      &(gsequencer_application_context->window));

  ags_functional_test_util_sync();

  /* window size */
  ags_functional_test_util_file_default_window_resize();

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LOOP_007_XML_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_loop_007_xml_test_clean_suite()
{  
  return(0);
}

void
ags_functional_loop_007_xml_test_file_setup()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  GtkButton *play_button;
  GtkButton *stop_button;
  
  struct timespec start_time, current_time;
  
  guint i;
  gboolean expired;
  gboolean success;

  /* get buttons */
  play_button = AGS_WINDOW(gsequencer_application_context->window)->navigation->play;
  stop_button = AGS_WINDOW(gsequencer_application_context->window)->navigation->stop;

  /* get initial time */
  success = TRUE;
    
  for(i = 0; success && i < AGS_FUNCTIONAL_LOOP_007_XML_TEST_FILE_SETUP_PLAYBACK_COUNT; i++){
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    
    expired = FALSE;
    
    g_message("start playback");

    ags_functional_test_util_button_click(play_button);

    while(!expired){  
      /* check expired */
      clock_gettime(CLOCK_MONOTONIC, &current_time);
      
      if(start_time.tv_sec + AGS_FUNCTIONAL_LOOP_007_XML_TEST_FILE_SETUP_PLAYBACK_DURATION < current_time.tv_sec){
	expired = TRUE;
      }
    }

    g_message("stop playback");
    
    ags_functional_test_util_button_click(stop_button);

    /* wait some time before next playback */
    g_usleep(5000000);

    if(!expired){
      success = FALSE;
    }
  }
}

int
main(int argc, char **argv)
{
  char **new_argv;
  gchar *str;
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFunctionalLoop007XmlTest", ags_functional_loop_007_xml_test_init_suite, ags_functional_loop_007_xml_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  g_atomic_int_set(&is_available,
		   FALSE);
  g_atomic_int_set(&is_terminated,
		   FALSE);

  new_argv = (char **) malloc((argc + 4) * sizeof(char *));
  memcpy(new_argv, argv, argc * sizeof(char **));
  new_argv[argc] = "--filename";
  new_argv[argc + 1] = AGS_FUNCTIONAL_LOOP_007_XML_TEST_FILE_SETUP_FILENAME;
  new_argv[argc + 2] = "--no-config";
  new_argv[argc + 3] = NULL;
  argc += 3;

#if defined(AGS_TEST_CONFIG)
  ags_functional_test_util_init(&argc, &new_argv,
				AGS_TEST_CONFIG);
#else
  if((str = getenv("AGS_TEST_CONFIG")) != NULL){
    ags_functional_test_util_init(&argc, &new_argv,
				  str);
  }else{
    ags_functional_test_util_init(&argc, &new_argv,
				  AGS_FUNCTIONAL_LOOP_007_XML_TEST_CONFIG);
  }
#endif
  
  ags_functional_test_util_do_run(argc, new_argv,
				  ags_functional_loop_007_xml_test_add_test, &is_available);

  g_atomic_int_set(&is_terminated,
		   TRUE);

  g_thread_join(ags_functional_test_util_test_runner_thread());
  
  return(-1);
}
