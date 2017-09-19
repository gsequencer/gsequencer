/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/test/X/libgsequencer.h>

#include "../gsequencer_setup_util.h"
#include "../ags_functional_test_util.h"

void ags_functional_ffplayer_test_add_test();

int ags_functional_ffplayer_test_init_suite();
int ags_functional_ffplayer_test_clean_suite();

#ifdef AGS_WITH_LIBINSTPATCH
void ags_functional_ffplayer_test_open_sf2();
void ags_functional_ffplayer_test_resize_pads();
void ags_functional_ffplayer_test_resize_audio_channels();
#endif

#define AGS_FUNCTIONAL_FFPLAYER_TEST_OPEN_SF2_PATH "/usr/share/sounds/sf2"
#define AGS_FUNCTIONAL_FFPLAYER_TEST_OPEN_SF2_FILENAME "/usr/share/sounds/sf2/FluidR3_GM.sf2"

#define AGS_FUNCTIONAL_FFPLAYER_TEST_RESIZE_OUTPUT_PADS (5)
#define AGS_FUNCTIONAL_FFPLAYER_TEST_RESIZE_INPUT_PADS (15)

#define AGS_FUNCTIONAL_FFPLAYER_TEST_RESIZE_AUDIO_CHANNELS (7)

#define AGS_FUNCTIONAL_FFPLAYER_TEST_CONFIG "[generic]\n"	\
  "autosave-thread=false\n"					\
  "simple-file=true\n"						\
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
  "device=hw:0,0\n"						\
  "samplerate=44100\n"						\
  "buffer-size=1024\n"						\
  "pcm-channels=2\n"						\
  "dsp-channels=2\n"						\
  "format=16\n"							\
  "\n"								\
  "[recall]\n"							\
  "auto-sense=true\n"						\
  "\n"

CU_pSuite pSuite = NULL;    
volatile gboolean is_available;

extern AgsApplicationContext *ags_application_context;

AgsGuiThread *gui_thread;
AgsTaskThread *task_thread;

void ags_functional_ffplayer_test_add_test()
{
  /* add the tests to the suite */
  if((CU_add_test(pSuite, "functional test of AgsFFPlayer open sf2\0", ags_functional_ffplayer_test_open_sf2) == NULL) ||
     (CU_add_test(pSuite, "functional test of AgsFFPlayer resize pads\0", ags_functional_ffplayer_test_resize_pads) == NULL) ||
     (CU_add_test(pSuite, "functional test of AgsFFPlayer resize audio channels\0", ags_functional_ffplayer_test_resize_audio_channels) == NULL)){
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
ags_functional_ffplayer_test_init_suite()
{
  /* get gui thread */
  gui_thread = ags_thread_find_type(ags_application_context->main_loop,
				    AGS_TYPE_GUI_THREAD);

  task_thread = ags_thread_find_type(ags_application_context->main_loop,
				     AGS_TYPE_TASK_THREAD);
    
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_ffplayer_test_clean_suite()
{  
  ags_thread_stop(gui_thread);  

  return(0);
}

void
ags_functional_ffplayer_test_open_sf2()
{
  AgsXorgApplicationContext *xorg_application_context;

  AgsFFPlayer *ffplayer;

  GList *list_start, *list;

  guint nth_machine;
  gboolean success;

  /* add ffplayer */
  success = ags_functional_test_util_add_machine(NULL,
						 "FPlayer");

  CU_ASSERT(success == TRUE);

  /*  */
  gdk_threads_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve ffplayer */
  nth_machine = 0;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  gdk_threads_leave();

  if(list != NULL &&
     AGS_IS_FFPLAYER(list->data)){
    ffplayer = list->data;
  }else{
    ffplayer = NULL;
  }
  
  CU_ASSERT(ffplayer != NULL);

  /* open dialog */
  success = ags_functional_test_util_ffplayer_open(0);

  CU_ASSERT(success == TRUE);

  /* open path */
  success = ags_functional_test_util_file_chooser_open_path(GTK_FILE_CHOOSER(ffplayer->open_dialog),
							    AGS_FUNCTIONAL_FFPLAYER_TEST_OPEN_SF2_PATH);
  CU_ASSERT(success == TRUE);

  /* select sf2 */
  success = ags_functional_test_util_file_chooser_select_filename(GTK_FILE_CHOOSER(ffplayer->open_dialog),
								  AGS_FUNCTIONAL_FFPLAYER_TEST_OPEN_SF2_FILENAME);

  CU_ASSERT(success == TRUE);

  /* response ok */
  success = ags_functional_test_util_dialog_ok(GTK_DIALOG(ffplayer->open_dialog));

  CU_ASSERT(success == TRUE);

  /* destroy ffplayer */
  success = ags_functional_test_util_machine_destroy(0);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_ffplayer_test_resize_pads()
{
  GtkDialog *properties;

  AgsXorgApplicationContext *xorg_application_context;
  
  AgsFFPlayer *ffplayer;

  GList *list_start, *list;

  guint nth_machine;
  guint resize_tab;
  gboolean success;

  /* add ffplayer */
  success = ags_functional_test_util_add_machine(NULL,
						 "FPlayer");

  CU_ASSERT(success == TRUE);

  /*  */
  gdk_threads_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve ffplayer */
  nth_machine = 0;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  gdk_threads_leave();

  if(list != NULL &&
     AGS_IS_FFPLAYER(list->data)){
    ffplayer = list->data;
  }else{
    ffplayer = NULL;
  }
  
  CU_ASSERT(ffplayer != NULL);

  /*
   * resize output and input pads
   */
  
  /* open properties */
  ags_functional_test_util_machine_properties_open(nth_machine);

  /* click tab */
  resize_tab = 4;
  
  ags_functional_test_util_machine_properties_click_tab(nth_machine,
							resize_tab);
  
  /* click enable */
  ags_functional_test_util_machine_properties_click_enable(nth_machine);

  /* set output pads */
  ags_functional_test_util_machine_properties_resize_outputs(nth_machine,
							     AGS_FUNCTIONAL_FFPLAYER_TEST_RESIZE_OUTPUT_PADS);

  /* set input pads */
  ags_functional_test_util_machine_properties_resize_inputs(nth_machine,
							    AGS_FUNCTIONAL_FFPLAYER_TEST_RESIZE_INPUT_PADS);

  /* response ok */
  pthread_mutex_lock(task_thread->launch_mutex);

  properties = AGS_MACHINE(ffplayer)->properties;
  
  pthread_mutex_unlock(task_thread->launch_mutex);

  ags_functional_test_util_dialog_ok(properties);

  /* destroy ffplayer */
  success = ags_functional_test_util_machine_destroy(0);
}

void
ags_functional_ffplayer_test_resize_audio_channels()
{
  GtkDialog *properties;

  AgsXorgApplicationContext *xorg_application_context;
  
  AgsFFPlayer *ffplayer;

  GList *list_start, *list;

  guint nth_machine;
  guint resize_tab;
  gboolean success;

  /* add ffplayer */
  success = ags_functional_test_util_add_machine(NULL,
						 "FPlayer");

  CU_ASSERT(success == TRUE);

  /*  */
  gdk_threads_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve ffplayer */
  nth_machine = 0;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  gdk_threads_leave();

  if(list != NULL &&
     AGS_IS_FFPLAYER(list->data)){
    ffplayer = list->data;
  }else{
    ffplayer = NULL;
  }
  
  CU_ASSERT(ffplayer != NULL);

  /*
   * resize audio channels
   */
  
  /* open properties */
  ags_functional_test_util_machine_properties_open(nth_machine);

  /* click tab */
  resize_tab = 4;
  
  ags_functional_test_util_machine_properties_click_tab(nth_machine,
							resize_tab);
  
  /* click enable */
  ags_functional_test_util_machine_properties_click_enable(nth_machine);

  /* set output audio_channels */
  ags_functional_test_util_machine_properties_resize_audio_channels(nth_machine,
								    AGS_FUNCTIONAL_FFPLAYER_TEST_RESIZE_AUDIO_CHANNELS);

  /* response ok */
  pthread_mutex_lock(task_thread->launch_mutex);

  properties = AGS_MACHINE(ffplayer)->properties;
  
  pthread_mutex_unlock(task_thread->launch_mutex);

  ags_functional_test_util_dialog_ok(properties);

  /* destroy ffplayer */
  success = ags_functional_test_util_machine_destroy(0);
}

int
main(int argc, char **argv)
{
#ifdef AGS_WITH_LIBINSTPATCH  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFuncitonalFFPlayerTest\0", ags_functional_ffplayer_test_init_suite, ags_functional_ffplayer_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  g_atomic_int_set(&is_available,
		   FALSE);
  
  ags_test_init(&argc, &argv,
		AGS_FUNCTIONAL_FFPLAYER_TEST_CONFIG);
  ags_functional_test_util_do_run(argc, argv,
				  ags_functional_ffplayer_test_add_test, &is_available);

  return(-1);
#else
  return(0);
#endif
}
