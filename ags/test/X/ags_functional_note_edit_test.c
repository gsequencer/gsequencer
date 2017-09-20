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

#ifdef AGS_WITH_LIBINSTPATCH
#include <libinstpatch/libinstpatch.h>
#endif

#include <ags/gsequencer_main.h>

#include <ags/X/ags_xorg_application_context.h>

#include <ags/X/thread/ags_gui_thread.h>

#include "gsequencer_setup_util.h"

void ags_functional_note_edit_test_add_test();

int ags_functional_note_edit_test_init_suite();
int ags_functional_note_edit_test_clean_suite();

void ags_functional_note_edit_test_quit_stub(AgsApplicationContext *application_context);

void ags_functional_note_edit_test_file_setup();

#define AGS_FUNCTIONAL_NOTE_EDIT_TEST_FILE_SETUP_FILENAME SRCDIR "/" "ags_functional_note_edit_test.xml\0"
#define AGS_FUNCTIONAL_NOTE_EDIT_TEST_FILE_SETUP_PLAYBACK_COUNT (3)
#define AGS_FUNCTIONAL_NOTE_EDIT_TEST_FILE_SETUP_PLAYBACK_DURATION (30)

#define AGS_FUNCTIONAL_NOTE_EDIT_TEST_CONFIG "[generic]\n" \
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
  "device=hw:0,0\n"                                    \
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

extern AgsApplicationContext *ags_application_context;

AgsGuiThread *gui_thread;
AgsTaskThread *task_thread;

void
ags_functional_note_edit_test_add_test()
{
  /* add the tests to the suite */
  if((CU_add_test(pSuite, "functional test of GSequencer setup by file and editing notes\0", ags_functional_note_edit_test_file_setup) == NULL)){
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
ags_functional_note_edit_test_init_suite()
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
ags_functional_note_edit_test_clean_suite()
{  
  return(0);
}

void
ags_functional_note_edit_test_quit_stub(AgsApplicationContext *application_context)
{
  g_critical("quit stub\0");
}

void
ags_functional_note_edit_test_file_setup()
{
  GtkButton *play_button;
  GtkButton *stop_button;
  
  struct timespec start_time, current_time;
  
  guint i;
  gboolean expired;
  gboolean success;
  
  while(!g_atomic_int_get(&(AGS_XORG_APPLICATION_CONTEXT(ags_application_context)->file_ready))){
    usleep(500000);
  }

  usleep(500000);  
  
  /* get buttons */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  play_button = AGS_XORG_APPLICATION_CONTEXT(ags_application_context)->window->navigation->play;
  stop_button = AGS_XORG_APPLICATION_CONTEXT(ags_application_context)->window->navigation->stop;

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  /* get initial time */
  success = TRUE;
    
  for(i = 0; success && i < AGS_FUNCTIONAL_NOTE_EDIT_TEST_FILE_SETUP_PLAYBACK_COUNT; i++){
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    
    expired = FALSE;
    
    g_message("start playback");
    ags_functional_test_util_button_click(play_button);

    while(!expired){  
      /* check expired */
      clock_gettime(CLOCK_MONOTONIC, &current_time);
      
      if(start_time.tv_sec + AGS_FUNCTIONAL_NOTE_EDIT_TEST_FILE_SETUP_PLAYBACK_DURATION < current_time.tv_sec){
	expired = TRUE;
      }
    }

    g_message("stop playback");
    ags_functional_test_util_button_click(stop_button);

    /* wait some time before next playback */
    usleep(5000000);

    if(!expired){
      success = FALSE;
    }
  }

  CU_ASSERT(success == TRUE);
}

int
main(int argc, char **argv)
{
  char **new_argv;
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFuncitonalNoteEditTest\0", ags_functional_note_edit_test_init_suite, ags_functional_note_edit_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  g_atomic_int_set(&is_available,
		   FALSE);

  new_argv = (char **) malloc(argc + 2 * sizeof(char *));
  memcpy(new_argv, argv, argc * sizeof(char **));
  new_argv[argc] = "--filename";
  new_argv[argc + 1] = AGS_FUNCTIONAL_NOTE_EDIT_TEST_FILE_SETUP_FILENAME;
  new_argv[argc + 2] = NULL;
  argc += 2;
  
  ags_test_init(&argc, &new_argv,
		AGS_FUNCTIONAL_NOTE_EDIT_TEST_CONFIG);
  ags_functional_test_util_do_run(argc, new_argv,
				  ags_functional_note_edit_test_add_test, &is_available);

  return(-1);
}

