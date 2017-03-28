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

#include <ags/gsequencer_main.h>

#include <ags/X/ags_xorg_application_context.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <ags/test/X/gsequencer_setup_util.h>

#include "gsequencer_setup_util.h"

int ags_functional_note_edit_test_init_suite();
int ags_functional_note_edit_test_clean_suite();

void ags_functional_note_edit_test_quit_stub(AgsApplicationContext *application_context);

void ags_functional_note_edit_test_note_manual_setup();
void ags_functional_note_edit_test_note_file_setup();

#define AGS_FUNCTIONAL_NOTE_EDIT_TEST_MANUAL_SETUP_PLAYBACK_COUNT (8)
#define AGS_FUNCTIONAL_NOTE_EDIT_TEST_MANUAL_SETUP_PLAYBACK_DURATION (960)

#define AGS_FUNCTIONAL_NOTE_EDIT_TEST_FILE_SETUP_FILENAME "ags_functional_note_edit_test.xml\0"
#define AGS_FUNCTIONAL_NOTE_EDIT_TEST_FILE_SETUP_PLAYBACK_COUNT (8)
#define AGS_FUNCTIONAL_NOTE_EDIT_TEST_FILE_SETUP_PLAYBACK_DURATION (960)

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

extern struct sigaction ags_test_sigact;
extern gboolean ags_gui_ready;

extern AgsApplicationContext *ags_application_context;
extern volatile gboolean ags_show_start_animation;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_note_edit_test_init_suite()
{
  AgsConfig *config;

  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_FUNCTIONAL_NOTE_EDIT_TEST_CONFIG,
			    strlen(AGS_FUNCTIONAL_NOTE_EDIT_TEST_CONFIG));
    
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
  
  AgsGuiThread *gui_thread;
  
  struct timespec start_time, current_time;

  gchar *start_arg[] = {
    "./gsequencer\0"
  };
  
  guint i;
  gboolean expired;
  gboolean success;
  
  ags_application_context = NULL;

  ags_test_setup(1, start_arg);

  AGS_APPLICATION_CONTEXT_GET_CLASS(ags_application_context)->quit = ags_functional_note_edit_test_quit_stub;

  /* get gui thread */
  gui_thread = ags_thread_find_type(ags_application_context->main_loop,
				    AGS_TYPE_GUI_THREAD);

  /* get buttons */
  play_button = AGS_XORG_APPLICATION_CONTEXT(ags_application_context)->window->navigation->play;
  stop_button = AGS_XORG_APPLICATION_CONTEXT(ags_application_context)->window->navigation->stop;

  /* launch application */
  ags_test_launch_filename(AGS_FUNCTIONAL_NOTE_EDIT_TEST_FILE_SETUP_FILENAME,
			   FALSE);

  /* get initial time */
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  success = TRUE;
  
  /* do the work */
  while(!ags_gui_ready){
    usleep(500000);
  }

  usleep(10000000);
  
  for(i = 0; success && i < AGS_FUNCTIONAL_NOTE_EDIT_TEST_MANUAL_SETUP_PLAYBACK_COUNT; i++){
    expired = FALSE;
    
    g_message("start playback");
    gtk_button_clicked(play_button);

    while(!expired){  
      /* check expired */
      clock_gettime(CLOCK_MONOTONIC, &current_time);
      
      if(start_time.tv_sec + AGS_FUNCTIONAL_NOTE_EDIT_TEST_MANUAL_SETUP_PLAYBACK_DURATION < current_time.tv_sec){
	expired = TRUE;
      }
    }

    g_message("stop playback");
    gtk_button_clicked(stop_button);

    /* wait some time before next playback */
    usleep(5000000);

    if(!expired){
      success = FALSE;
    }
  }

  ags_thread_stop(gui_thread);
  
  CU_ASSERT(success == TRUE);
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  AgsConfig *config;

  pthread_t *animation_thread;

  struct sched_param param;
  struct rlimit rl;
  struct sigaction sa;
  struct passwd *pw;

  gchar *rc_filename;
  
  uid_t uid;
  int result;

  const rlim_t kStackSize = 64L * 1024L * 1024L;   // min stack size = 64 Mb

#ifdef AGS_USE_TIMER
  timer_t *timer_id
#endif
  
  putenv("LC_ALL=C\0");
  putenv("LANG=C\0");

  //  mtrace();
  atexit(ags_test_signal_cleanup);

  result = getrlimit(RLIMIT_STACK, &rl);

  /* set stack size 64M */
  if(result == 0){
    if(rl.rlim_cur < kStackSize){
      rl.rlim_cur = kStackSize;
      result = setrlimit(RLIMIT_STACK, &rl);

      if(result != 0){
	//TODO:JK
      }
    }
  }

  param.sched_priority = GSEQUENCER_RT_PRIORITY;
      
  if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
    perror("sched_setscheduler failed\0");
  }

  /* Ignore interactive and job-control signals.  */
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);
  signal(AGS_THREAD_RESUME_SIG, SIG_IGN);
  signal(AGS_THREAD_SUSPEND_SIG, SIG_IGN);

  ags_test_sigact.sa_handler = ags_test_signal_handler;
  sigemptyset(&ags_test_sigact.sa_mask);
  ags_test_sigact.sa_flags = 0;
  sigaction(SIGINT, &ags_test_sigact, (struct sigaction *) NULL);
  sigaction(SA_RESTART, &ags_test_sigact, (struct sigaction *) NULL);

  XInitThreads();
  
  uid = getuid();
  pw = getpwuid(uid);
    
  /* parse rc file */
  rc_filename = g_strdup_printf("gsequencer.share/styles/ags.rc\0",
				pw->pw_dir,
				AGS_DEFAULT_DIRECTORY);
  
  gtk_rc_parse(rc_filename);
  g_free(rc_filename);
  
  /**/
  LIBXML_TEST_VERSION;

  //ao_initialize();

  gdk_threads_enter();
  //  g_thread_init(NULL);
  gtk_init(&argc, &argv);

  g_object_set(gtk_settings_get_default(),
	       "gtk-theme-name\0", "Raleigh\0",
	       NULL);
  g_signal_handlers_block_matched(gtk_settings_get_default(),
				  G_SIGNAL_MATCH_DETAIL,
				  g_signal_lookup("set-property\0",
						  GTK_TYPE_SETTINGS),
				  g_quark_from_string("gtk-theme-name\0"),
				  NULL,
				  NULL,
				  NULL);
  
  ipatch_init();
  //  g_log_set_fatal_mask("GLib-GObject\0", // "Gtk\0" G_LOG_DOMAIN, // 
		       //		       G_LOG_LEVEL_CRITICAL); // G_LOG_LEVEL_WARNING

  /* animate */
  animation_thread = (pthread_t *) malloc(sizeof(pthread_t));
  g_atomic_int_set(&(ags_show_start_animation),
		   TRUE);
  
  ags_test_start_animation(animation_thread);
  
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

  gtk_init(NULL,
	   NULL);
  //  g_log_set_fatal_mask(G_LOG_DOMAIN, // "GLib-GObject\0", // "Gtk\0" G_LOG_DOMAIN,
  //		       G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING);

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "functional test of GSequencer setup by file and editing notes\0", ags_functional_note_edit_test_file_setup) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

