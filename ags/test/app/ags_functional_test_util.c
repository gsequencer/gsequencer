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

#include "config.h"

#include <ags/test/app/ags_functional_test_util.h>

//FIXME:JK: conflict of KeyCode
#define __AGS_VST_KEYCODES_H__

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/gsequencer_main.h>

#include <ags/test/app/libgsequencer.h>

#include <gdk/gdk.h>
#include <gdk/gdkevents.h>

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

#include <sys/time.h>
#include <sys/resource.h>

#define _GNU_SOURCE
#include <locale.h>

#include <stdlib.h>

#ifndef AGS_W32API
#include <sys/time.h>
#include <sys/resource.h>

#include <pwd.h>
#endif

#include <ags/i18n.h>

void* ags_functional_test_util_add_test_thread(void *ptr);
void* ags_functional_test_util_do_run_thread(void *ptr);

void ags_functional_test_util_assert_driver_program(guint n_params,
						    gchar **param_strv,
						    GValue *param);

gboolean ags_functional_test_util_driver_timeout(gpointer data);

void ags_functional_test_util_sync_driver_program(guint n_params,
						  gchar **param_strv,
						  GValue *param);

void ags_functional_test_util_idle_condition_and_timeout_driver_program(guint n_params,
									gchar **param_strv,
									GValue *param);
void ags_functional_test_util_leave_driver_program(guint n_params,
						   gchar **param_strv,
						   GValue *param);

void ags_functional_test_util_header_bar_menu_button_click_driver_program(guint n_params,
									  gchar **param_strv,
									  GValue *param);
void ags_functional_test_util_combo_box_click_driver_program(guint n_params,
							     gchar **param_strv,
							     GValue *param);
void ags_functional_test_util_button_click_driver_program(guint n_params,
							  gchar **param_strv,
							  GValue *param);
void ags_functional_test_util_toggle_button_click_driver_program(guint n_params,
								 gchar **param_strv,
								 GValue *param);
void ags_functional_test_util_check_button_click_driver_program(guint n_params,
								gchar **param_strv,
								GValue *param);
void ags_functional_test_util_menu_button_click_driver_program(guint n_params,
							       gchar **param_strv,
							       GValue *param);

void ags_functional_test_util_dialog_apply_driver_program(guint n_params,
							  gchar **param_strv,
							  GValue *param);
void ags_functional_test_util_dialog_ok_driver_program(guint n_params,
						       gchar **param_strv,
						       GValue *param);
void ags_functional_test_util_dialog_cancel_driver_program(guint n_params,
							   gchar **param_strv,
							   GValue *param);

void ags_functional_test_util_file_chooser_open_path_driver_program(guint n_params,
								    gchar **param_strv,
								    GValue *param);
void ags_functional_test_util_file_chooser_select_filename_driver_program(guint n_params,
									  gchar **param_strv,
									  GValue *param);
void ags_functional_test_util_file_chooser_select_filenames_driver_program(guint n_params,
									   gchar **param_strv,
									   GValue *param);
void ags_functional_test_util_file_chooser_select_all_driver_program(guint n_params,
								     gchar **param_strv,
								     GValue *param);

void ags_functional_test_util_file_default_window_resize_driver_program(guint n_params,
									gchar **param_strv,
									GValue *param);

void ags_functional_test_util_open_driver_program(guint n_params,
						  gchar **param_strv,
						  GValue *param);
void ags_functional_test_util_save_driver_program(guint n_params,
						  gchar **param_strv,
						  GValue *param);
void ags_functional_test_util_save_as_driver_program(guint n_params,
						     gchar **param_strv,
						     GValue *param);
void ags_functional_test_util_export_open_driver_program(guint n_params,
							 gchar **param_strv,
							 GValue *param);

void ags_functional_test_util_export_close_driver_program(guint n_params,
							  gchar **param_strv,
							  GValue *param);
void ags_functional_test_util_export_add_driver_program(guint n_params,
							gchar **param_strv,
							GValue *param);
void ags_functional_test_util_export_tact_driver_program(guint n_params,
							 gchar **param_strv,
							 GValue *param);
void ags_functional_test_util_export_remove_driver_program(guint n_params,
							   gchar **param_strv,
							   GValue *param);
void ags_functional_test_util_export_set_backend_driver_program(guint n_params,
								gchar **param_strv,
								GValue *param);
void ags_functional_test_util_export_set_device_driver_program(guint n_params,
							       gchar **param_strv,
							       GValue *param);
void ags_functional_test_util_export_set_filename_driver_program(guint n_params,
								 gchar **param_strv,
								 GValue *param);

void ags_functional_test_util_add_machine_driver_program(guint n_params,
							 gchar **param_strv,
							 GValue *param);

void ags_functional_test_util_preferences_open_driver_program(guint n_params,
							      gchar **param_strv,
							      GValue *param);
void ags_functional_test_util_preferences_close_driver_program(guint n_params,
							       gchar **param_strv,
							       GValue *param);

void ags_functional_test_util_composite_toolbar_cursor_click_driver_program(guint n_params,
									    gchar **param_strv,
									    GValue *param);
void ags_functional_test_util_composite_toolbar_edit_click_driver_program(guint n_params,
									  gchar **param_strv,
									  GValue *param);
void ags_functional_test_util_composite_toolbar_delete_click_driver_program(guint n_params,
									    gchar **param_strv,
									    GValue *param);
void ags_functional_test_util_composite_toolbar_select_click_driver_program(guint n_params,
									    gchar **param_strv,
									    GValue *param);

void ags_functional_test_util_composite_toolbar_paste_click_driver_program(guint n_params,
									   gchar **param_strv,
									   GValue *param);
void ags_functional_test_util_composite_toolbar_copy_click_driver_program(guint n_params,
									  gchar **param_strv,
									  GValue *param);
void ags_functional_test_util_composite_toolbar_cut_click_driver_program(guint n_params,
									 gchar **param_strv,
									 GValue *param);
void ags_functional_test_util_composite_toolbar_zoom_driver_program(guint n_params,
								    gchar **param_strv,
								    GValue *param);

void ags_functional_test_util_machine_selector_add_driver_program(guint n_params,
								  gchar **param_strv,
								  GValue *param);

void ags_functional_test_util_machine_selector_select_driver_program(guint n_params,
								     gchar **param_strv,
								     GValue *param);

void ags_functional_test_util_preferences_click_tab_driver_program(guint n_params,
								   gchar **param_strv,
								   GValue *param);

void ags_functional_test_util_notation_edit_add_point_driver_program(guint n_params,
								     gchar **param_strv,
								     GValue *param);

void ags_functional_test_util_audio_preferences_buffer_size_driver_program(guint n_params,
									   gchar **param_strv,
									   GValue *param);
void ags_functional_test_util_audio_preferences_samplerate_driver_program(guint n_params,
									  gchar **param_strv,
									  GValue *param);

void ags_functional_test_util_machine_menu_button_click_driver_program(guint n_params,
								       gchar **param_strv,
								       GValue *param);

void ags_functional_test_util_machine_move_up_driver_program(guint n_params,
							     gchar **param_strv,
							     GValue *param);
void ags_functional_test_util_machine_move_down_driver_program(guint n_params,
							       gchar **param_strv,
							       GValue *param);
void ags_functional_test_util_machine_hide_driver_program(guint n_params,
							  gchar **param_strv,
							  GValue *param);
void ags_functional_test_util_machine_show_driver_program(guint n_params,
							  gchar **param_strv,
							  GValue *param);
void ags_functional_test_util_machine_destroy_driver_program(guint n_params,
							     gchar **param_strv,
							     GValue *param);
void ags_functional_test_util_machine_rename_open_driver_program(guint n_params,
								 gchar **param_strv,
								 GValue *param);
void ags_functional_test_util_machine_editor_dialog_open_driver_program(guint n_params,
									gchar **param_strv,
									GValue *param);

void ags_functional_test_util_machine_editor_dialog_click_tab_driver_program(guint n_params,
									     gchar **param_strv,
									     GValue *param);
void ags_functional_test_util_machine_editor_dialog_click_enable_driver_program(guint n_params,
										gchar **param_strv,
										GValue *param);
void ags_functional_test_util_machine_editor_dialog_link_set_driver_program(guint n_params,
									    gchar **param_strv,
									    GValue *param);

void ags_functional_test_util_machine_editor_dialog_effect_add_driver_program(guint n_params,
									      gchar **param_strv,
									      GValue *param);
void ags_functional_test_util_machine_editor_dialog_effect_remove_driver_program(guint n_params,
										 gchar **param_strv,
										 GValue *param);
void ags_functional_test_util_machine_editor_dialog_effect_plugin_type_driver_program(guint n_params,
										      gchar **param_strv,
										      GValue *param);

void ags_functional_test_util_machine_editor_dialog_ladspa_filename_driver_program(guint n_params,
										   gchar **param_strv,
										   GValue *param);
void ags_functional_test_util_machine_editor_dialog_ladspa_effect_driver_program(guint n_params,
										 gchar **param_strv,
										 GValue *param);

void ags_functional_test_util_machine_editor_dialog_lv2_filename_driver_program(guint n_params,
										gchar **param_strv,
										GValue *param);

void ags_functional_test_util_machine_editor_dialog_bulk_add_driver_program(guint n_params,
									    gchar **param_strv,
									    GValue *param);
void ags_functional_test_util_machine_editor_dialog_bulk_link_driver_program(guint n_params,
									     gchar **param_strv,
									     GValue *param);
void ags_functional_test_util_machine_editor_dialog_bulk_first_line_driver_program(guint n_params,
										   gchar **param_strv,
										   GValue *param);
void ags_functional_test_util_machine_editor_dialog_bulk_link_line_driver_program(guint n_params,
										  gchar **param_strv,
										  GValue *param);
void ags_functional_test_util_machine_editor_dialog_bulk_count_driver_program(guint n_params,
									      gchar **param_strv,
									      GValue *param);

void ags_functional_test_util_machine_editor_dialog_resize_audio_channels_driver_program(guint n_params,
											 gchar **param_strv,
											 GValue *param);
void ags_functional_test_util_machine_editor_dialog_resize_inputs_driver_program(guint n_params,
										 gchar **param_strv,
										 GValue *param);
void ags_functional_test_util_machine_editor_dialog_resize_outputs_driver_program(guint n_params,
										  gchar **param_strv,
										  GValue *param);

#ifdef AGS_FAST_FUNCTIONAL_TESTS
#define AGS_FUNCTIONAL_TEST_UTIL_REACTION_TIME (125000)
#define AGS_FUNCTIONAL_TEST_UTIL_REACTION_TIME_LONG (750000)
#else
#define AGS_FUNCTIONAL_TEST_UTIL_REACTION_TIME (1000000)
#define AGS_FUNCTIONAL_TEST_UTIL_REACTION_TIME_LONG (2000000)
#endif

extern AgsApplicationContext *ags_application_context;

extern AgsLadspaManager *ags_ladspa_manager;
extern AgsDssiManager *ags_dssi_manager;
extern AgsLv2Manager *ags_lv2_manager;
extern AgsLv2uiManager *ags_lv2ui_manager;

GThread *ags_functional_test_util_thread;

AgsTaskLauncher *task_launcher;

struct _AddTest{
  AgsFunctionalTestUtilAddTest add_test;
  volatile gboolean *is_available;
};

struct timespec ags_functional_test_util_default_timeout = {
  59,
  0,
};

gint64 driver_stamp = 0;
gint64 reaction_time_stamp = -1;
gint64 reaction_idle_time = 0;

gboolean test_driver_yield = FALSE;

GRecMutex test_driver_mutex;
GList *test_driver_program = NULL;

GList *test_stack = NULL;

gboolean test_driver_program_glue = FALSE;

struct timespec idle_condition_start_time;

gboolean driver_program_status = TRUE;

void
ags_functional_test_util_init(int *argc, char ***argv,
			      gchar *conf_str)
{
  AgsConfig *config;
  AgsPriority *priority;
  GtkSettings *settings;
  
  gchar *filename;

  gboolean no_config;
  guint i;

#ifdef AGS_WITH_RT
  struct sched_param param;
  struct rlimit rl;
#endif
  struct passwd *pw;

  char **gst_argv;
  gchar *wdir, *config_file;
  gchar *rc_filename;
  gchar *base_dir;
  gchar *str, *data_dir;
  gchar path[PATH_MAX];

  int gst_argc;
  
  uint32_t size = sizeof(path);
  uid_t uid;
  int result;

#ifdef AGS_WITH_RT
  const rlim_t kStackSize = 64L * 1024L * 1024L;   // min stack size = 64 Mb
#endif
  
  base_dir = strdup(AGS_SRC_DIR);
  printf("base dir %s\n", base_dir);
  
  /* set some environment variables */
  sprintf(path, "%s/gsequencer.share/styles",
	  base_dir);
  data_dir = realpath(path,
		      NULL);
  str = malloc(PATH_MAX * sizeof(gchar));
  sprintf(str,
	  "AGS_CSS_FILENAME=%s/ags.css",
	  data_dir);
  putenv(str);

  sprintf(path, "%s/gsequencer.share/images",
	  base_dir);
  data_dir = realpath(path,
		      NULL);
  str = malloc(PATH_MAX * sizeof(gchar));
  sprintf(str,
	  "AGS_ANIMATION_FILENAME=%s/gsequencer-800x450.png",
	  data_dir);
  putenv(str);

  sprintf(path, "%s/gsequencer.share/images",
	  base_dir);
  data_dir = realpath(path,
		      NULL);
  str = malloc(PATH_MAX * sizeof(gchar));
  sprintf(str,
	  "AGS_LOGO_FILENAME=%s/ags.png",
	  data_dir);
  putenv(str);

  sprintf(path, "%s",
	  base_dir);
  data_dir = realpath(path,
		      NULL);
  str = malloc(PATH_MAX * sizeof(gchar));
  sprintf(str,
	  "AGS_LICENSE_FILENAME=%s/COPYING",
	  data_dir);
  putenv(str);

  /* gettext */
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);

  /* parameters */
  priority = ags_priority_get_instance();  
  ags_priority_load_defaults(priority);
  
  //  g_log_set_fatal_mask("GLib", // "Gtk" , // 
  //		       G_LOG_LEVEL_CRITICAL); // G_LOG_LEVEL_WARNING

#ifdef AGS_WITH_RT
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

  priority = ags_priority_get_instance();  

  param.sched_priority = 1;

  str = ags_priority_get_value(priority,
			       AGS_PRIORITY_RT_THREAD,
			       AGS_PRIORITY_KEY_GUI_MAIN_LOOP);

  if(str != NULL){
    param.sched_priority = (int) g_ascii_strtoull(str,
						  NULL,
						  10);

    g_free(str);
  }
      
  if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
    perror("sched_setscheduler failed");
  }
#endif

  //#ifdef AGS_WITH_X11
  XInitThreads();
  //#endif
  
  /* parse command line parameter */
  filename = NULL;
  no_config = FALSE;
  
  for(i = 0; i < argc[0];){
    if(!strncmp(argv[0][i], "--help", 7)){
      printf("GSequencer is an audio sequencer and notation editor\n\n");

      printf("Usage:\n\t%s\n\t%s\n\t%s\n\t%s\n\n",
	     "Report bugs to <jkraehemann@gmail.com>\n",
	     "--filename file     open file",
	     "--help              display this help and exit",
	     "--version           output version information and exit");
      
      exit(0);
    }else if(!strncmp(argv[0][i], "--version", 10)){
      printf("GSequencer %s\n\n", AGS_VERSION);
      
      printf("%s\n%s\n%s\n\n",
	     "Copyright (C) 2005-2020 Joël Krähemann",
	     "This is free software; see the source for copying conditions.  There is NO",
	     "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
      
      printf("Written by Joël Krähemann\n");

      exit(0);
    }else if(!strncmp(argv[0][i], "--filename", 11)){
      filename = g_strdup(argv[0][i + 1]);
      i += 2;
    }else if(!strncmp(argv[0][i], "--no-config", 12)){
      no_config = TRUE;
      i++;
    }else{
      i++;
    }
  }

  uid = getuid();
  pw = getpwuid(uid);
  
  /**/
  LIBXML_TEST_VERSION;

#if defined(AGS_WITH_GSTREAMER)
  gst_argc = 1;
  gst_argv = (char **) g_malloc(2 * sizeof(char *));

  gst_argv[0] = g_strdup(argv[0][0]);
  gst_argv[1] = NULL;
  
  gst_init(&gst_argc, &gst_argv);
#endif

  gtk_init();
  
#ifdef AGS_WITH_LIBINSTPATCH
  ipatch_init();
#endif
  
  settings = gtk_settings_get_default();

  g_object_set(settings,
	       "gtk-primary-button-warps-slider", FALSE,
	       NULL);
  g_signal_handlers_block_matched(settings,
				  G_SIGNAL_MATCH_DETAIL,
				  g_signal_lookup("set-property",
						  GTK_TYPE_SETTINGS),
				  g_quark_from_string("gtk-primary-button-warps-slider"),
				  NULL,
				  NULL,
				  NULL);
  

  /* setup */
  wdir = g_strdup_printf("%s/%s",
			 pw->pw_dir,
			 AGS_DEFAULT_DIRECTORY);

  config_file = g_strdup_printf("%s/%s",
				wdir,
				AGS_DEFAULT_CONFIG);

  config = ags_config_get_instance();

#if defined(AGS_TEST_CONFIG)
  g_message("AGS_TEST_CONFIG:\n %s", AGS_TEST_CONFIG);
      
  ags_config_load_from_data(config,
			    AGS_TEST_CONFIG, strlen(AGS_TEST_CONFIG));
#else
  if((str = getenv("AGS_TEST_CONFIG")) != NULL){
    g_message("AGS_TEST_CONFIG:\n %s", str);
    
    ags_config_load_from_data(config,
			      str, strlen(str));
  }else{
    if(conf_str != NULL){
      ags_config_load_from_data(config,
				conf_str,
				strlen(conf_str));
    }else{
      ags_config_load_from_file(config,
				config_file);
    }
  }
#endif

  g_free(wdir);
  g_free(config_file);  
}

GThread*
ags_functional_test_util_test_runner_thread()
{
  return(ags_functional_test_util_thread);
}

void*
ags_functional_test_util_do_run_thread(void *ptr)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  
  volatile gboolean *is_available;
  
  gsequencer_application_context = (AgsGSequencerApplicationContext *) ags_application_context_get_instance();

  is_available = ptr;
  
  //  g_atomic_int_set(&(gsequencer_application_context->gui_ready),
  //		   FALSE);

  /*  */
  while(g_atomic_int_get(&(gsequencer_application_context->show_animation))){
    usleep(500000);
  }

  usleep(1000000);

  /* get gui thread */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(gsequencer_application_context));
  
  ags_functional_test_util_notify_add_test(is_available);
  
  g_thread_exit(NULL);

  return(NULL);
}

void
ags_functional_test_util_do_run(int argc, char **argv,
				AgsFunctionalTestUtilAddTest add_test, volatile gboolean *is_available)
{
  AgsWindow *window;
  AgsGSequencerApplication *gsequencer_app;

  AgsApplicationContext *application_context;
  AgsLog *log;

  GThread *thread;

  gchar *application_id;

  GError *error;

  application_id = "org.nongnu.gsequencer.gsequencer";

  /* app */
  gsequencer_app = ags_gsequencer_application_new(application_id,
						  (G_APPLICATION_HANDLES_OPEN |
						   G_APPLICATION_NON_UNIQUE));

  error = NULL;
  g_application_register(G_APPLICATION(gsequencer_app),
			 NULL,
			 &error);    

  /* application context */
  log = ags_log_get_instance();

  ags_log_add_message(log,
		      "Welcome to Advanced Gtk+ Sequencer - Test");

  /* application context */ 
  thread = g_thread_new("libgsequencer.so - functional test",
			ags_functional_test_util_do_run_thread,
			is_available);

  ags_functional_test_util_add_driver(1.0 / 30.0);
  
  application_context = ags_application_context_get_instance();
  
  application_context->argc = argc;
  application_context->argv = argv;

  ags_application_context_prepare(application_context);

  ags_application_context_setup(application_context);

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  gtk_window_set_titlebar((GtkWindow *) window,
			  (GtkWidget *) AGS_WINDOW(window)->header_bar);

  gtk_application_add_window(gsequencer_app,
			     GTK_WINDOW(window));

  g_object_set(G_OBJECT(window),
	       "application", gsequencer_app,
	       NULL);

  ags_connectable_connect(AGS_CONNECTABLE(application_context));

  ags_functional_test_util_add_test(add_test, is_available);  
  
  g_application_run(G_APPLICATION(gsequencer_app),
		    0, NULL);
}

void*
ags_functional_test_util_add_test_thread(void *ptr)
{
  struct _AddTest *test;

  ags_functional_test_util_thread = g_thread_self();
  
  test = ptr;
  
  while(!g_atomic_int_get(test->is_available)){
    usleep(12500);
  }

  test->add_test();
  
  g_thread_exit(NULL);

  return(NULL);
}

void
ags_functional_test_util_add_test(AgsFunctionalTestUtilAddTest add_test,
				  volatile gboolean *is_available)
{
  struct _AddTest *test;

  GThread *thread;

  test = (struct _AddTest *) malloc(sizeof(struct _AddTest));

  test->add_test = add_test;
  test->is_available = is_available;

  thread = g_thread_new("libgsequencer.so - functional test",
			ags_functional_test_util_add_test_thread,
			test);
}

void
ags_functional_test_util_notify_add_test(volatile gboolean *is_available)
{
  g_atomic_int_set(is_available,
		   TRUE);
}

gboolean
ags_functional_test_util_driver_timeout(gpointer data)
{
  gint64 current_stamp;
  static const gint64 time_limit = 400;
  static const gint64 action_time = (gint64) (G_USEC_PER_SEC / 30.0);
  
  current_stamp = g_get_monotonic_time();

  do{
    if(reaction_time_stamp != -1){
      if(reaction_time_stamp + reaction_idle_time >= current_stamp){
	reaction_time_stamp = -1;

	reaction_idle_time = 0;
      }
    }
    
    if(reaction_time_stamp == -1){
      if(test_driver_program != NULL){
	AgsFunctionalTestUtilDriverProgram *driver_program;

	GList *list;

	gboolean success;

	g_rec_mutex_lock(&test_driver_mutex);
      
	list = g_list_last(test_driver_program);
	
	driver_program = list->data;

	g_rec_mutex_unlock(&test_driver_mutex);
      
	driver_program->driver_program_func(driver_program->n_params,
					    driver_program->param_strv,
					    driver_program->param);

	g_rec_mutex_lock(&test_driver_mutex);
	
	if(!test_driver_program_glue){
	  test_driver_program = g_list_remove(test_driver_program,
					      driver_program);
	}else{
	  test_driver_program_glue = FALSE;
	}

	g_rec_mutex_unlock(&test_driver_mutex);
      }
    }
  }while(!test_driver_yield && current_stamp + time_limit < driver_stamp + action_time && test_driver_program != NULL);

  if(current_stamp >= driver_stamp + action_time){
    driver_stamp = g_get_monotonic_time();
  }
  
  return(G_SOURCE_CONTINUE);
}

void
ags_functional_test_util_sync_driver_program(guint n_params,
					     gchar **param_strv,
					     GValue *param)
{
  volatile gint *is_done;
  
  is_done = g_value_get_pointer(param);

  g_atomic_int_set(is_done,
		   TRUE);
}

void
ags_functional_test_util_sync()
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  volatile gint is_done;
  
  g_atomic_int_set(&is_done,
		   FALSE);

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_sync_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("value");
  driver_program->param_strv[1] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_POINTER);
  g_value_set_pointer(driver_program->param,
		      &is_done);
  
  ags_functional_test_util_add_driver_program(driver_program);


  while(!g_atomic_int_get(&is_done)){
    g_usleep(1000000);
  }
}

void
ags_functional_test_util_add_driver(gdouble timeout_s)
{
  g_timeout_add((guint) (1000.0 * timeout_s),
		(GSourceFunc) ags_functional_test_util_driver_timeout,
		NULL);
}

void
ags_functional_test_util_add_driver_program(AgsFunctionalTestUtilDriverProgram *driver_program)
{
  g_rec_mutex_lock(&test_driver_mutex);
  
  test_driver_program = g_list_prepend(test_driver_program,
				       driver_program);

  g_rec_mutex_unlock(&test_driver_mutex);
}

GValue*
ags_functional_test_util_stack_get(gint position)
{
  return(g_list_nth_data(test_stack, position));
}

void
ags_functional_test_util_stack_add(GValue *value)
{
  test_stack = g_list_prepend(test_stack,
			      value);
}

void
ags_functional_test_util_stack_clear()
{
  test_stack = NULL;
}

void
ags_functional_test_util_assert_driver_program(guint n_params,
					       gchar **param_strv,
					       GValue *param)
{
  gchar *filename;
  gchar *function_str;
  gchar *condition_str;

  guint line;
  gboolean value;
  
  value = g_value_get_boolean(param);

  filename = g_value_get_string(param + 1);
  line = g_value_get_uint(param + 2);
  function_str = g_value_get_string(param + 3);
  condition_str = g_value_get_string(param + 4);
  
  CU_assertImplementation((value), line, condition_str, filename, "", CU_FALSE);
}

void
ags_functional_test_util_assert(gboolean value, gchar *filename, guint line, gchar *function_str, gchar *condition_str)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  gint64 time_stamp;
  
  time_stamp = g_get_monotonic_time();

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_assert_driver_program;
  
  driver_program->n_params = 5;

  /* param string vector */
  driver_program->param_strv = g_malloc(6 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("value");
  driver_program->param_strv[1] = g_strdup("filename");
  driver_program->param_strv[2] = g_strdup("line");
  driver_program->param_strv[3] = g_strdup("function_str");
  driver_program->param_strv[4] = g_strdup("condition_str");
  driver_program->param_strv[5] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 5);

  g_value_init(driver_program->param,
	       G_TYPE_BOOLEAN);
  g_value_set_boolean(driver_program->param,
		      value);

  g_value_init(driver_program->param + 1,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param + 1,
		     filename);

  g_value_init(driver_program->param + 2,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 2,
		   line);

  g_value_init(driver_program->param + 3,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param + 3,
		     function_str);

  g_value_init(driver_program->param + 4,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param + 4,
		     condition_str);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_clear_application_context()
{  
  ags_application_context = NULL;

  //  ags_mutex_manager = NULL;

  //  ags_ladspa_manager = NULL;
  //  ags_dssi_manager = NULL;
  //  ags_lv2_manager = NULL;
  //  ags_lv2ui_manager = NULL;
}

void
ags_functional_test_util_driver_yield()
{
  test_driver_yield = TRUE;
}

void
ags_functional_test_util_driver_program_glue()
{
  test_driver_program_glue = TRUE;
}

void
ags_functional_test_util_reaction_time()
{
  reaction_time_stamp = g_get_monotonic_time();

  reaction_idle_time = AGS_FUNCTIONAL_TEST_UTIL_REACTION_TIME;
}

void
ags_functional_test_util_reaction_time_long()
{
  reaction_time_stamp = g_get_monotonic_time();

  reaction_idle_time = AGS_FUNCTIONAL_TEST_UTIL_REACTION_TIME_LONG;
}

void
ags_functional_test_util_idle_driver_program(guint n_params,
					     gchar **param_strv,
					     GValue *param)
{
  gint64 time_stamp;
  gint64 idle_time;
  gint64 current_stamp;
  
  current_stamp = g_get_monotonic_time();

  time_stamp = g_value_get_int64(param);
  
  idle_time = g_value_get_int64(param + 1);

  if(time_stamp + idle_time > current_stamp){
    ags_functional_test_util_driver_program_glue();

    ags_functional_test_util_driver_yield();
  }else{
    g_usleep(4);
  }
}

void
ags_functional_test_util_idle(gint64 idle_time)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  gint64 time_stamp;
  
  time_stamp = g_get_monotonic_time();

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_idle_driver_program;
  
  driver_program->n_params = 2;

  /* param string vector */
  driver_program->param_strv = g_malloc(3 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("time_stamp");
  driver_program->param_strv[1] = g_strdup("idle_time");
  driver_program->param_strv[2] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 2);

  g_value_init(driver_program->param,
	       G_TYPE_INT64);
  g_value_set_int64(driver_program->param,
		    time_stamp);

  g_value_init(driver_program->param + 1,
	       G_TYPE_INT64);
  g_value_set_int64(driver_program->param + 1,
		    idle_time);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_idle_condition_and_timeout_driver_program(guint n_params,
								   gchar **param_strv,
								   GValue *param)
{
  AgsFunctionalTestUtilIdleCondition idle_condition;

  struct timespec current_time;
  struct timespec *timeout;
  
  gpointer data;
  
  idle_condition = g_value_get_pointer(param);
  
  timeout = g_value_get_pointer(param + 1);
  
  data = g_value_get_pointer(param + 2);

  clock_gettime(CLOCK_MONOTONIC,
                &current_time);

  if(!idle_condition(data)){
      g_usleep(4);
  }else{
    if(idle_condition_start_time.tv_sec + timeout->tv_sec > current_time.tv_sec){
      ags_functional_test_util_driver_program_glue();

      ags_functional_test_util_driver_yield();
    }else{
      g_warning("test condition timeout");
    }
  }
}

void
ags_functional_test_util_idle_condition_and_timeout(AgsFunctionalTestUtilIdleCondition idle_condition,
						    struct timespec *timeout,
						    gpointer data)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_idle_condition_and_timeout_driver_program;
  
  driver_program->n_params = 3;

  /* param string vector */
  driver_program->param_strv = g_malloc(4 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("idle_condition");
  driver_program->param_strv[1] = g_strdup("timeout");
  driver_program->param_strv[2] = g_strdup("data");  
  driver_program->param_strv[3] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 3);

  g_value_init(driver_program->param,
	       G_TYPE_POINTER);
  g_value_set_pointer(driver_program->param,
		      idle_condition);
  
  g_value_init(driver_program->param + 1,
	       G_TYPE_POINTER);
  g_value_set_pointer(driver_program->param + 1,
		      timeout);

  g_value_init(driver_program->param + 2,
	       G_TYPE_POINTER);
  g_value_set_pointer(driver_program->param + 2,
		      data);

  clock_gettime(CLOCK_MONOTONIC,
		&idle_condition_start_time);

  ags_functional_test_util_add_driver_program(driver_program);  
}

gboolean
ags_functional_test_util_idle_test_widget_visible(GtkWidget **widget)
{
  gboolean do_idle;

  do_idle = TRUE;
  
  if(*widget != NULL &&
     GTK_IS_WIDGET(*widget) &&
     gtk_widget_is_visible(*widget)){
    do_idle = FALSE;
  }

  return(do_idle);
}

gboolean
ags_functional_test_util_idle_test_widget_hidden(GtkWidget **widget)
{
  gboolean do_idle;

  do_idle = TRUE;
  
  if(*widget != NULL &&
     GTK_IS_WIDGET(*widget) &&
     !gtk_widget_is_visible(*widget)){
    do_idle = FALSE;
  }
  
  return(do_idle);
}

gboolean
ags_functional_test_util_idle_test_widget_realized(GtkWidget **widget)
{
  gboolean do_idle;

  do_idle = TRUE;
  
  if(*widget != NULL &&
     GTK_IS_WIDGET(*widget) &&
     gtk_widget_get_realized(*widget)){
    do_idle = FALSE;
  }
  
  return(do_idle);
}

gboolean
ags_functional_test_util_idle_test_null(GtkWidget **widget)
{
  gboolean do_idle;

  do_idle = TRUE;
  
  if(*widget == NULL){
    do_idle = FALSE;
  }
  
  return(do_idle);
}

gboolean
ags_functional_test_util_idle_test_list_length(AgsFunctionalTestUtilListLengthCondition *condition)
{
  gboolean do_idle;

  do_idle = TRUE;
  
  if(g_list_length(condition->start_list[0]) == condition->length){
    do_idle = FALSE;
  }
  
  return(do_idle);
}

void
ags_functional_test_util_leave_driver_program(guint n_params,
					      gchar **param_strv,
					      GValue *param)
{
  GtkWidget *window;

  window = g_value_get_object(param);

  gtk_window_destroy(window);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_leave(GtkWidget *window)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  if(!GTK_IS_WINDOW(window)){
    return;
  }

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_leave_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("window");
  driver_program->param_strv[1] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param,
		     window);

  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_fake_mouse_warp(gpointer display, guint screen, guint x, guint y)
{
  static const gulong delay = 1;

  XTestFakeMotionEvent((Display *) display, screen, x, y, delay);
  XSync((Display *) display, 0);
}

void
ags_functional_test_util_fake_mouse_button_press(gpointer display, guint button)
{
  static const gulong delay = 1;
  
  XTestFakeButtonEvent((Display *) display, button, 1, delay);
  XFlush((Display *) display);
}

void
ags_functional_test_util_fake_mouse_button_release(gpointer display, guint button)
{
  static const gulong delay = 1;
  
  XTestFakeButtonEvent((Display *) display, button, 0, delay);
  XFlush((Display *) display);
}

void
ags_functional_test_util_fake_mouse_button_click(gpointer display, guint button)
{
  static const gulong delay = 1;
  
  XTestFakeButtonEvent((Display *) display, button, 1, delay);
  XTestFakeButtonEvent((Display *) display, button, 0, delay);
  XFlush((Display *) display);
}

void
ags_functional_test_util_fake_button_click(GtkButton *button)
{
  g_signal_emit_by_name(button, "clicked");
}

void
ags_functional_test_util_fake_toggle_button_click(GtkToggleButton *toggle_button)
{
  gtk_toggle_button_set_active(toggle_button,
			       !gtk_toggle_button_get_active(toggle_button));
}

void
ags_functional_test_util_fake_check_button_click(GtkCheckButton *check_button)
{
  gtk_check_button_set_active(check_button,
			      !gtk_check_button_get_active(check_button));
}

void
ags_functional_test_util_header_bar_menu_button_click_driver_program(guint n_params,
								     gchar **param_strv,
								     GValue *param)
{
  GtkMenuButton *menu_button;
  
  gchar **item_path;
  gchar *action;

  gboolean success;

  menu_button = g_value_get_object(param);
  
  item_path = g_value_get_pointer(param + 1);
  
  action = g_value_get_string(param + 2);

  success = TRUE;
  
  if(!g_ascii_strncasecmp(action, "app.open", 9)){
    ags_app_action_util_open();
  }else if(!g_ascii_strncasecmp(action, "app.save", 9)){
    ags_app_action_util_save();
  }else if(!g_ascii_strncasecmp(action, "app.save_as", 12)){
    ags_app_action_util_save_as();
  }else if(!g_ascii_strncasecmp(action, "app.export", 11)){
    ags_app_action_util_export();
  }else if(!g_ascii_strncasecmp(action, "app.meta_data", 14)){
    ags_app_action_util_meta_data();
  }else if(!g_ascii_strncasecmp(action, "app.smf_import", 15)){
    ags_app_action_util_smf_import();
  }else if(!g_ascii_strncasecmp(action, "app.smf_export", 15)){
    ags_app_action_util_smf_export();
  }else if(!g_ascii_strncasecmp(action, "app.preferences", 16)){
    ags_app_action_util_preferences();
  }else if(!g_ascii_strncasecmp(action, "app.about", 10)){
    ags_app_action_util_about();
  }else if(!g_ascii_strncasecmp(action, "app.help", 9)){
    ags_app_action_util_help();
  }else if(!g_ascii_strncasecmp(action, "app.quit", 9)){
    ags_app_action_util_quit();
  }else if(!g_ascii_strncasecmp(action, "app.add_panel", 14)){
    ags_app_action_util_add_panel();
  }else if(!g_ascii_strncasecmp(action, "app.add_spectrometer", 20)){
    ags_app_action_util_add_spectrometer();
  }else if(!g_ascii_strncasecmp(action, "app.add_equalizer", 18)){
    ags_app_action_util_add_equalizer();
  }else if(!g_ascii_strncasecmp(action, "app.add_mixer", 13)){
    ags_app_action_util_add_mixer();
  }else if(!g_ascii_strncasecmp(action, "app.add_drum", 12)){
    ags_app_action_util_add_drum();
  }else if(!g_ascii_strncasecmp(action, "app.add_matrix", 14)){
    ags_app_action_util_add_matrix();
  }else if(!g_ascii_strncasecmp(action, "app.add_synth", 13)){
    ags_app_action_util_add_synth();
  }else if(!g_ascii_strncasecmp(action, "app.add_fm_synth", 16)){
    ags_app_action_util_add_fm_synth();
  }else if(!g_ascii_strncasecmp(action, "app.add_syncsynth", 17)){
    ags_app_action_util_add_syncsynth();
  }else if(!g_ascii_strncasecmp(action, "app.add_fm_syncsynth", 20)){
    ags_app_action_util_add_fm_syncsynth();
  }else if(!g_ascii_strncasecmp(action, "app.add_hybrid_synth", 20)){
    ags_app_action_util_add_hybrid_synth();
  }else if(!g_ascii_strncasecmp(action, "app.add_hybrid_fm_synth", 23)){
    ags_app_action_util_add_hybrid_fm_synth();
  }else if(!g_ascii_strncasecmp(action, "app.add_ffplayer", 16)){
    ags_app_action_util_add_ffplayer();
  }else if(!g_ascii_strncasecmp(action, "app.add_sf2_synth", 17)){
    ags_app_action_util_add_sf2_synth();
  }else if(!g_ascii_strncasecmp(action, "app.add_pitch_sampler", 21)){
    ags_app_action_util_add_pitch_sampler();
  }else if(!g_ascii_strncasecmp(action, "app.add_sfz_synth", 17)){
    ags_app_action_util_add_sfz_synth();
  }else if(!g_ascii_strncasecmp(action, "app.add_audiorec", 16)){
    ags_app_action_util_add_audiorec();
  }else if(!g_ascii_strncasecmp(action, "app.edit_notation", 17)){
    ags_app_action_util_edit_notation();
  }else if(!g_ascii_strncasecmp(action, "app.edit_automation", 19)){
    ags_app_action_util_edit_automation();
  }else if(!g_ascii_strncasecmp(action, "app.edit_wave", 14)){
    ags_app_action_util_edit_wave();
  }else{
    success = FALSE;
  }

  if(!success){
    g_warning("unknown action");
  }

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_header_bar_menu_button_click(GtkMenuButton *menu_button,
						      gchar **item_path,
						      gchar *action)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  if(!GTK_IS_MENU_BUTTON(menu_button)){
    return;
  }

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_header_bar_menu_button_click_driver_program;
  
  driver_program->n_params = 3;

  /* param string vector */
  driver_program->param_strv = g_malloc(4 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("menu_button");
  driver_program->param_strv[1] = g_strdup("item_path");
  driver_program->param_strv[2] = g_strdup("action");
  driver_program->param_strv[3] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 3);

  g_value_init(driver_program->param,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param,
		     menu_button);

  g_value_init(driver_program->param + 1,
	       G_TYPE_POINTER);
  g_value_set_pointer(driver_program->param + 1,
		      item_path);

  g_value_init(driver_program->param + 2,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param + 2,
		     action);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_combo_box_click_driver_program(guint n_params,
							gchar **param_strv,
							GValue *param)
{
  GtkComboBox *combo_box;
  
  guint nth;
  
  combo_box = g_value_get_object(param);
  
  nth = g_value_get_uint(param + 1);

  gtk_combo_box_set_active(combo_box,
			   nth);
	
  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_combo_box_click(GtkComboBox *combo_box,
					 guint nth)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;
  
  if(!GTK_IS_COMBO_BOX(combo_box)){
    return;
  }

  /*  */
  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_combo_box_click_driver_program;
  
  driver_program->n_params = 2;

  /* param string vector */
  driver_program->param_strv = g_malloc(3 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("combo_box");
  driver_program->param_strv[1] = g_strdup("nth");
  driver_program->param_strv[2] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 2);

  g_value_init(driver_program->param,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param,
		     combo_box);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   nth);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_button_click_driver_program(guint n_params,
						     gchar **param_strv,
						     GValue *param)
{
  GtkButton *button;
  
  button = g_value_get_object(param);

  /* emit signal */
  ags_functional_test_util_fake_button_click(button);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_button_click(GtkButton *button)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  if(!GTK_IS_BUTTON(button)){
    return;
  }

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_button_click_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("button");
  driver_program->param_strv[1] = NULL;
  
  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param,
		     button);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_toggle_button_click_driver_program(guint n_params,
							    gchar **param_strv,
							    GValue *param)
{
  GtkToggleButton *toggle_button;
  
  toggle_button = g_value_get_object(param);

  /* emit signal */
  ags_functional_test_util_fake_toggle_button_click(toggle_button);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_toggle_button_click(GtkToggleButton *toggle_button)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  if(!GTK_IS_TOGGLE_BUTTON(toggle_button)){
    return;
  }

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_toggle_button_click_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("toggle_button");
  driver_program->param_strv[1] = NULL;
  
  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param,
		     toggle_button);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_check_button_click_driver_program(guint n_params,
							   gchar **param_strv,
							   GValue *param)
{
  GtkCheckButton *check_button;
  
  check_button = g_value_get_object(param);

  /* emit signal */
  ags_functional_test_util_fake_check_button_click(check_button);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_check_button_click(GtkCheckButton *check_button)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  if(!GTK_IS_CHECK_BUTTON(check_button)){
    return;
  }

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_check_button_click_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("check_button");
  driver_program->param_strv[1] = NULL;
  
  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param,
		     check_button);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_menu_button_click_driver_program(guint n_params,
							  gchar **param_strv,
							  GValue *param)
{
  GtkMenuButton *menu_button;
  
  menu_button = g_value_get_object(param);

  /* emit signal */
  g_signal_emit_by_name(menu_button,
			"activate");

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_menu_button_click(GtkMenuButton *menu_button)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  if(!GTK_IS_MENU_BUTTON(menu_button)){
    return;
  }

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_menu_button_click_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("menu_button");
  driver_program->param_strv[1] = NULL;
  
  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param,
		     menu_button);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_dialog_apply_driver_program(guint n_params,
						     gchar **param_strv,
						     GValue *param)
{
  GtkDialog *dialog;
  
  dialog = g_value_get_object(param);

  gtk_dialog_response(dialog,
		      GTK_RESPONSE_APPLY);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_dialog_apply(GtkDialog *dialog)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  if(!GTK_IS_DIALOG(dialog)){
    return;
  }

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_dialog_apply_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("dialog");
  driver_program->param_strv[1] = NULL;
  
  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param,
		     dialog);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_dialog_ok_driver_program(guint n_params,
						  gchar **param_strv,
						  GValue *param)
{
  GtkDialog *dialog;
  
  dialog = g_value_get_object(param);

  gtk_dialog_response(dialog,
		      GTK_RESPONSE_OK);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_dialog_ok(GtkDialog *dialog)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;
  
  if(!GTK_IS_DIALOG(dialog)){
    return;
  }

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_dialog_ok_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("dialog");
  driver_program->param_strv[1] = NULL;
  
  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param,
		     dialog);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_dialog_cancel_driver_program(guint n_params,
						      gchar **param_strv,
						      GValue *param)
{
  GtkDialog *dialog;
  
  dialog = g_value_get_object(param);

  gtk_dialog_response(dialog,
		      GTK_RESPONSE_CANCEL);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_dialog_cancel(GtkDialog *dialog)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;
  
  if(!GTK_IS_DIALOG(dialog)){
    return;
  }

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_dialog_cancel_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("dialog");
  driver_program->param_strv[1] = NULL;
  
  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param,
		     dialog);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_file_chooser_open_path_driver_program(guint n_params,
							       gchar **param_strv,
							       GValue *param)
{
  GtkFileChooser *file_chooser;
  
  gchar *path;

  file_chooser = g_value_get_object(param);

  path = g_value_get_string(param + 1);

  gtk_file_chooser_set_current_folder(file_chooser,
				      g_file_new_for_path(path),
				      NULL);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_file_chooser_open_path(GtkFileChooser *file_chooser,
						gchar *path)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  if(!GTK_IS_FILE_CHOOSER(file_chooser) ||
     path == NULL){
    return;
  }

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_file_chooser_open_path_driver_program;
  
  driver_program->n_params = 2;

  /* param string vector */
  driver_program->param_strv = g_malloc(3 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("file_chooser");
  driver_program->param_strv[1] = g_strdup("path");
  driver_program->param_strv[2] = NULL;
  
  /* param value array */
  driver_program->param = g_new0(GValue,
				 2);

  g_value_init(driver_program->param,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param,
		     file_chooser);

  g_value_init(driver_program->param + 1,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param + 1,
		     path);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_file_chooser_select_filename_driver_program(guint n_params,
								     gchar **param_strv,
								     GValue *param)
{
  GtkFileChooser *file_chooser;
  
  gchar *filename;
  
  file_chooser = g_value_get_object(param);
  
  filename = g_value_get_string(param + 1);
  
  gtk_file_chooser_set_file(file_chooser,
			    g_file_new_for_path(filename),
			    NULL);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_file_chooser_select_filename(GtkFileChooser *file_chooser,
						      gchar *filename)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  if(!GTK_IS_FILE_CHOOSER(file_chooser) ||
     filename == NULL){
    return;
  }

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_file_chooser_select_filename_driver_program;
  
  driver_program->n_params = 2;

  /* param string vector */
  driver_program->param_strv = g_malloc(3 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("file_chooser");
  driver_program->param_strv[1] = g_strdup("filename");
  driver_program->param_strv[2] = NULL;
  
  /* param value array */
  driver_program->param = g_new0(GValue,
				 2);

  g_value_init(driver_program->param,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param,
		     file_chooser);

  g_value_init(driver_program->param + 1,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param + 1,
		     filename);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_file_chooser_select_filenames_driver_program(guint n_params,
								      gchar **param_strv,
								      GValue *param)
{
  GtkFileChooser *file_chooser;

  GSList *filename;
  
  file_chooser = g_value_get_object(param);
  
  filename = g_value_get_pointer(param + 1);
  
  while(filename != NULL){
    gtk_file_chooser_set_file(file_chooser,
			      g_file_new_for_path(filename->data),
			      NULL);

    filename = filename->next;
  }
  
  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_file_chooser_select_filenames(GtkFileChooser *file_chooser,
						       GSList *filename)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  if(!GTK_IS_FILE_CHOOSER(file_chooser) ||
     filename == NULL){
    return;
  }

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_file_chooser_select_filenames_driver_program;
  
  driver_program->n_params = 2;

  /* param string vector */
  driver_program->param_strv = g_malloc(3 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("file_chooser");
  driver_program->param_strv[1] = g_strdup("filename");
  driver_program->param_strv[2] = NULL;
  
  /* param value array */
  driver_program->param = g_new0(GValue,
				 2);

  g_value_init(driver_program->param,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param,
		     file_chooser);

  g_value_init(driver_program->param + 1,
	       G_TYPE_POINTER);
  g_value_set_pointer(driver_program->param + 1,
		      filename);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_file_chooser_select_all_driver_program(guint n_params,
								gchar **param_strv,
								GValue *param)
{
  GtkFileChooser *file_chooser;
  
  file_chooser = g_value_get_object(param);

#if 0
  gtk_file_chooser_select_all(GTK_FILE_CHOOSER(file_chooser));
#endif
  
  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_file_chooser_select_all(GtkFileChooser *file_chooser)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  if(!GTK_IS_FILE_CHOOSER(file_chooser)){
    return;
  }

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_file_chooser_select_all_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("file_chooser");
  driver_program->param_strv[1] = NULL;
  
  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param,
		     file_chooser);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_file_default_window_resize_driver_program(guint n_params,
								   gchar **param_strv,
								   GValue *param)
{
  AgsWindow *window;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  gtk_window_set_default_size(window,
			      1920 - 128, 1080 - 64);
    
  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_file_default_window_resize()
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_file_default_window_resize_driver_program;
  
  driver_program->n_params = 0;

  /* param string vector */
  driver_program->param_strv = NULL;
  
  /* param value array */
  driver_program->param = NULL;
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_open_driver_program(guint n_params,
					     gchar **param_strv,
					     GValue *param)
{
  AgsWindow *window;
  
  AgsApplicationContext *application_context;

  static gchar* path_strv[] = {
    "_Open",
    NULL
  };

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  ags_functional_test_util_header_bar_menu_button_click(window->app_button,
							path_strv,
							"app.open");

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_open()
{
  AgsFunctionalTestUtilDriverProgram *driver_program;
  
  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_open_driver_program;
  
  driver_program->n_params = 0;

  /* param string vector */
  driver_program->param_strv = NULL;
  
  /* param value array */
  driver_program->param = NULL;
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_save_driver_program(guint n_params,
					     gchar **param_strv,
					     GValue *param)
{
  AgsWindow *window;
  
  AgsApplicationContext *application_context;

  static gchar* path_strv[] = {
    "_Save",
    NULL
  };

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  ags_functional_test_util_header_bar_menu_button_click(window->app_button,
							path_strv,
							"app.save");

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_save()
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_save_driver_program;
  
  driver_program->n_params = 0;

  /* param string vector */
  driver_program->param_strv = NULL;
  
  /* param value array */
  driver_program->param = NULL;
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_save_as_driver_program(guint n_params,
						gchar **param_strv,
						GValue *param)
{
  AgsWindow *window;
  
  AgsApplicationContext *application_context;

  static gchar* path_strv[] = {
    "Save as",
    NULL
  };

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  ags_functional_test_util_header_bar_menu_button_click(window->app_button,
							path_strv,
							"app.save_as");

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_save_as()
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_save_as_driver_program;
  
  driver_program->n_params = 0;

  /* param string vector */
  driver_program->param_strv = NULL;
  
  /* param value array */
  driver_program->param = NULL;
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_export_open_driver_program(guint n_params,
						    gchar **param_strv,
						    GValue *param)
{
  AgsWindow *window;
  
  AgsApplicationContext *application_context;

  static gchar* path_strv[] = {
    "_Export",
    NULL
  };

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  ags_functional_test_util_header_bar_menu_button_click(window->app_button,
							path_strv,
							"app.export");

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_export_open()
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_open_driver_program;
  
  driver_program->n_params = 0;

  /* param string vector */
  driver_program->param_strv = NULL;
  
  /* param value array */
  driver_program->param = NULL;
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_export_close_driver_program(guint n_params,
						     gchar **param_strv,
						     GValue *param)
{
  AgsExportWindow *export_window;
  
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  export_window = ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context));

  ags_functional_test_util_dialog_close(export_window);
  
  ags_functional_test_util_reaction_time_long();

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_hidden),
						      &ags_functional_test_util_default_timeout,
						      &export_window);
}

void
ags_functional_test_util_export_close()
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_export_close_driver_program;
  
  driver_program->n_params = 0;

  /* param string vector */
  driver_program->param_strv = NULL;
  
  /* param value array */
  driver_program->param = NULL;
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_export_add_driver_program(guint n_params,
						   gchar **param_strv,
						   GValue *param)
{
  AgsExportWindow *export_window;
  GtkButton *add_button;

  AgsApplicationContext *application_context;

  AgsFunctionalTestUtilListLengthCondition condition;

  application_context = ags_application_context_get_instance();

  export_window = ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context));

  add_button = export_window->add;

  condition.start_list = &(export_window->export_soundcard);

  condition.length = g_list_length(export_window->export_soundcard) + 1;
    
  ags_functional_test_util_fake_button_click(add_button);

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_test_util_default_timeout,
						      &condition);
}

void
ags_functional_test_util_export_add()
{
  AgsFunctionalTestUtilDriverProgram *driver_program;
  
  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_export_add_driver_program;
  
  driver_program->n_params = 0;

  /* param string vector */
  driver_program->param_strv = NULL;
  
  /* param value array */
  driver_program->param = NULL;
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_export_tact_driver_program(guint n_params,
						    gchar **param_strv,
						    GValue *param)
{
  AgsExportWindow *export_window;

  AgsApplicationContext *application_context;

  gdouble tact;

  tact = g_value_get_double(param);

  application_context = ags_application_context_get_instance();

  export_window = ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context));

  gtk_spin_button_set_value(export_window->tact,
			    tact);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_export_tact(gdouble tact)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;
  
  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_export_tact_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("tact");
  driver_program->param_strv[1] = NULL;
  
  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_DOUBLE);
  g_value_set_double(driver_program->param,
		     tact);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_export_remove_driver_program(guint n_params,
						      gchar **param_strv,
						      GValue *param)
{
  AgsExportWindow *export_window;
  GtkButton *remove_button;

  AgsApplicationContext *application_context;

  AgsFunctionalTestUtilListLengthCondition condition;

  GList *list;

  guint nth;
  guint i;
  gboolean success;

  nth = g_value_get_uint(param);

  application_context = ags_application_context_get_instance();

  export_window = ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context));

  condition.start_list = &(export_window->export_soundcard);

  condition.length = g_list_length(export_window->export_soundcard) - 1;

  success = FALSE;

  if(export_window->export_soundcard != NULL){
    remove_button = NULL;
  
    list = export_window->export_soundcard;

    for(i = 0; list != NULL; i++){
      if(i == nth){
	remove_button = AGS_EXPORT_SOUNDCARD(list->data)->remove_button;

	success = TRUE;
      
	break;
      }

      list = list->next;
    }
  
    g_list_free(list);
  
    ags_functional_test_util_fake_button_click(remove_button);
    ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
							&ags_functional_test_util_default_timeout,
							&condition);
  }
}

void
ags_functional_test_util_export_remove(guint nth)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;
  
  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_export_remove_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth");
  driver_program->param_strv[1] = NULL;
  
  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_export_set_backend_driver_program(guint n_params,
							   gchar **param_strv,
							   GValue *param)
{
  AgsExportWindow *export_window;

  AgsApplicationContext *application_context;

  GList *start_list, *list;

  gchar *backend;

  guint nth;
  guint i;
  gboolean success;
  
  nth = g_value_get_uint(param);
  
  backend = g_value_get_string(param + 1);

  application_context = ags_application_context_get_instance();

  export_window = ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context));

  list = export_window->export_soundcard;
  
  success = FALSE;

  for(i = 0; list != NULL; i++){
    if(i == nth){
      AgsExportSoundcard *export_soundcard;
      GtkTreeModel *model;

      GtkTreeIter iter;

      gchar *value;

      guint active;
      
      export_soundcard = AGS_EXPORT_SOUNDCARD(list->data);

      model = gtk_combo_box_get_model(GTK_COMBO_BOX(export_soundcard->backend));
      active = 0;

      if(gtk_tree_model_get_iter_first(model, &iter)){
	do{
	  gtk_tree_model_get(model, &iter,
			     0, &value,
			     -1);

	  if(!g_strcmp0(backend,
			value)){
	    gtk_combo_box_set_active_iter((GtkComboBox *) export_soundcard->backend,
					  &iter);
	    success = TRUE;
	    
	    break;
	  }
	}while(gtk_tree_model_iter_next(model,
					&iter));
      }      
      
      break;
    }

    list = list->next;
  }
  
  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_export_set_backend(guint nth,
					    gchar *backend)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;
  
  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_export_set_backend_driver_program;
  
  driver_program->n_params = 2;

  /* param string vector */
  driver_program->param_strv = g_malloc(3 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth");
  driver_program->param_strv[1] = g_strdup("backend");
  driver_program->param_strv[2] = NULL;
  
  /* param value array */
  driver_program->param = g_new0(GValue,
				 2);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth);

  g_value_init(driver_program->param + 1,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param + 1,
		     backend);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_export_set_device_driver_program(guint n_params,
							  gchar **param_strv,
							  GValue *param)
{
  AgsExportWindow *export_window;

  AgsApplicationContext *application_context;

  GList *list;

  gchar *device;
  
  guint nth;
  guint i;
  gboolean success;

  nth = g_value_get_uint(param);
  
  device = g_value_get_string(param + 1);

  application_context = ags_application_context_get_instance();

  export_window = ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context));

  list = export_window->export_soundcard;
  
  success = FALSE;

  for(i = 0; list != NULL; i++){
    if(i == nth){
      AgsExportSoundcard *export_soundcard;
      GtkTreeModel *model;

      GtkTreeIter iter;

      gchar *value;

      guint active;

      export_soundcard = AGS_EXPORT_SOUNDCARD(list->data);

      model = gtk_combo_box_get_model(GTK_COMBO_BOX(export_soundcard->card));
      active = 0;

      if(gtk_tree_model_get_iter_first(model, &iter)){
	do{
	  gtk_tree_model_get(model, &iter,
			     0, &value,
			     -1);

	  if(!g_strcmp0(device,
			value)){
	    gtk_combo_box_set_active_iter((GtkComboBox *) export_soundcard->card,
					  &iter);
	    success = TRUE;
	    
	    break;
	  }
	}while(gtk_tree_model_iter_next(model,
					&iter));
      }      
      
      break;
    }

    list = list->next;
  }
  
  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_export_set_device(guint nth,
					   gchar *device)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_export_set_device_driver_program;
  
  driver_program->n_params = 2;

  /* param string vector */
  driver_program->param_strv = g_malloc(3 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth");
  driver_program->param_strv[1] = g_strdup("device");
  driver_program->param_strv[2] = NULL;
  
  /* param value array */
  driver_program->param = g_new0(GValue,
				 2);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth);

  g_value_init(driver_program->param + 1,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param + 1,
		     device);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_export_set_filename_driver_program(guint n_params,
							    gchar **param_strv,
							    GValue *param)
{
  AgsExportWindow *export_window;

  AgsApplicationContext *application_context;

  GList *list;

  gchar *filename;
  
  guint nth;
  guint i;
  gboolean success;

  nth = g_value_get_uint(param);
  
  filename = g_value_get_string(param + 1);

  application_context = ags_application_context_get_instance();

  export_window = ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context));

  list = export_window->export_soundcard;
  success = FALSE;

  for(i = 0; list != NULL; i++){
    if(i == nth){
      AgsExportSoundcard *export_soundcard;
      
      export_soundcard = AGS_EXPORT_SOUNDCARD(list->data);

      gtk_editable_set_text(GTK_EDITABLE(export_soundcard->filename),
			    filename);
      
      success = TRUE;
      
      break;
    }
    
    list = list->next;
  }
  
  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_export_set_filename(guint nth,
					     gchar *filename)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_export_set_filename_driver_program;
  
  driver_program->n_params = 2;

  /* param string vector */
  driver_program->param_strv = g_malloc(3 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth");
  driver_program->param_strv[1] = g_strdup("filename");
  driver_program->param_strv[2] = NULL;
  
  /* param value array */
  driver_program->param = g_new0(GValue,
				 2);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth);

  g_value_init(driver_program->param + 1,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param + 1,
		     filename);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_export_nth(guint nth)
{
  //TODO:JK:
}

void
ags_functional_test_util_export_set_format(guint nth,
					   gchar *format)
{
  //TODO:JK: 
}

void
ags_functional_test_util_export_do_export(guint nth,
					  gchar *format)
{
  //TODO:JK: 
}

void
ags_functional_test_util_quit_driver_program(guint n_params,
					     gchar **param_strv,
					     GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  AgsWindow *window;

  gsequencer_application_context = ags_application_context_get_instance();

  ags_application_context_quit(AGS_APPLICATION_CONTEXT(gsequencer_application_context));

  ags_functional_test_util_driver_yield();
}

void
ags_functional_test_util_quit()
{
  AgsFunctionalTestUtilDriverProgram *driver_program;
  
  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_quit_driver_program;
  
  driver_program->n_params = 0;

  /* param string vector */
  driver_program->param_strv = NULL;

  /* param value array */
  driver_program->param = NULL;
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_add_machine_driver_program(guint n_params,
						    gchar **param_strv,
						    GValue *param)
{
  AgsWindow *window;
  
  AgsApplicationContext *application_context;

  gchar *submenu;
  gchar *machine_name;
  gchar **path_strv;
  gchar *action;

  gboolean success;
  
  GValue *value;

  submenu = g_value_get_string(param);

  machine_name = g_value_get_string(param + 1);

  application_context = ags_application_context_get_instance();
  
  path_strv = NULL;
  action = NULL;

  success = FALSE;
  
  if(submenu == NULL){
    if(!g_ascii_strncasecmp(machine_name,
			    "Panel",
			    6)){
      static gchar* panel_path[] = {
	"Panel",
	NULL
      };
      
      path_strv = panel_path;
      
      action = "app.add_panel";

      success = TRUE;

      g_action_group_activate_action(G_ACTION_GROUP(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context))), "add_panel",
				     NULL);
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Spectrometer",
				  13)){
      static gchar* spectrometer_path[] = {
	"Spectrometer",
	NULL
      };
      
      path_strv = spectrometer_path;
      
      action = "app.add_spectrometer";

      success = TRUE;
      
      g_action_group_activate_action(G_ACTION_GROUP(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context))), "add_spectrometer",
				     NULL);
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Equalizer",
				  10)){
      static gchar* equalizer_path[] = {
	"Equalizer",
	NULL
      };
      
      path_strv = equalizer_path;
      
      action = "app.add_equalizer";

      success = TRUE;
      
      g_action_group_activate_action(G_ACTION_GROUP(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context))), "add_equalizer",
				     NULL);
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Mixer",
				  6)){
      static gchar* mixer_path[] = {
	"Mixer",
	NULL
      };
      
      path_strv = mixer_path;
      
      action = "app.add_mixer";

      success = TRUE;
      
      g_action_group_activate_action(G_ACTION_GROUP(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context))), "add_mixer",
				     NULL);
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Drum",
				  5)){
      static gchar* drum_path[] = {
	"Drum",
	NULL
      };
      
      path_strv = drum_path;
      
      action = "app.add_drum";

      success = TRUE;
      
      g_action_group_activate_action(G_ACTION_GROUP(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context))), "add_drum",
				     NULL);
     }else if(!g_ascii_strncasecmp(machine_name,
				  "Matrix",
				  13)){
      static gchar* matrix_path[] = {
	"Matrix",
	NULL
      };
      
      path_strv = matrix_path;
      
      action = "app.add_matrix";

      success = TRUE;
      
      g_action_group_activate_action(G_ACTION_GROUP(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context))), "add_matrix",
				     NULL);
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Synth",
				  6)){
      static gchar* synth_path[] = {
	"Synth",
	NULL
      };
      
      path_strv = synth_path;
      
      action = "app.add_synth";

      success = TRUE;
      
      g_action_group_activate_action(G_ACTION_GROUP(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context))), "add_synth",
				     NULL);
    }else if(!g_ascii_strncasecmp(machine_name,
				  "FM Synth",
				  9)){
      static gchar* fm_synth_path[] = {
	"FM Synth",
	NULL
      };
      
      path_strv = fm_synth_path;
      
      action = "app.add_fm_synth";

      success = TRUE;
      
      g_action_group_activate_action(G_ACTION_GROUP(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context))), "add_fm_synth",
				     NULL);
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Syncsynth",
				  10)){
      static gchar* syncsynth_path[] = {
	"Syncsynth",
	NULL
      };
      
      path_strv = syncsynth_path;
      
      action = "app.add_syncsynth";

      success = TRUE;
      
      g_action_group_activate_action(G_ACTION_GROUP(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context))), "add_syncsynth",
				     NULL);
    }else if(!g_ascii_strncasecmp(machine_name,
				  "FM Syncsynth",
				  13)){
      static gchar* fm_syncsynth_path[] = {
	"FM Syncsynth",
	NULL
      };
      
      path_strv = fm_syncsynth_path;
      
      action = "app.add_fm_syncsynth";

      success = TRUE;
      
      g_action_group_activate_action(G_ACTION_GROUP(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context))), "add_fm_syncsynth",
				     NULL);
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Hybrid Synth",
				  13)){
      static gchar* hybrid_synth_path[] = {
	"Hybrid Synth",
	NULL
      };
      
      path_strv = hybrid_synth_path;
      
      action = "app.add_hybrid_synth";

      success = TRUE;
      
      g_action_group_activate_action(G_ACTION_GROUP(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context))), "add_hybrid_synth",
				     NULL);
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Hybrid FM Synth",
				  16)){
      static gchar* hybrid_fm_synth_path[] = {
	"Hybrid FM Synth",
	NULL
      };
      
      path_strv = hybrid_fm_synth_path;
      
      action = "app.add_hybrid_fm_synth";

      success = TRUE;
      
      g_action_group_activate_action(G_ACTION_GROUP(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context))), "add_hybrid_fm_synth",
 				     NULL);
    }else if(!g_ascii_strncasecmp(machine_name,
				  "FPlayer",
				  8)){
      static gchar* ffplayer_path[] = {
	"FPlayer",
	NULL
      };
      
      path_strv = ffplayer_path;
      
      action = "app.add_ffplayer";

      success = TRUE;
      
      g_action_group_activate_action(G_ACTION_GROUP(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context))), "add_ffplayer",
 				     NULL);
    }else if(!g_ascii_strncasecmp(machine_name,
				  "SF2 Synth",
				  10)){
      static gchar* sf2_synth_path[] = {
	"SF2 Synth",
	NULL
      };
      
      path_strv = sf2_synth_path;
      
      action = "app.add_sf2_synth";

      success = TRUE;
      
      g_action_group_activate_action(G_ACTION_GROUP(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context))), "add_sf2_synth",
 				     NULL);
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Pitch Sampler",
				  8)){
      static gchar* pitch_sampler_path[] = {
	"FPlayer",
	NULL
      };
      
      path_strv = pitch_sampler_path;
      
      action = "app.add_pitch_sampler";

      success = TRUE;
      
      g_action_group_activate_action(G_ACTION_GROUP(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context))), "add_pitch_sampler",
 				     NULL);
    }else if(!g_ascii_strncasecmp(machine_name,
				  "SFZ Synth",
				  10)){
      static gchar* sfz_synth_path[] = {
	"SFZ Synth",
	NULL
      };
      
      path_strv = sfz_synth_path;
      
      action = "app.add_sfz_synth";

      success = TRUE;
      
      g_action_group_activate_action(G_ACTION_GROUP(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context))), "add_sfz_synth",
 				     NULL);
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Audiorec",
				  9)){
      static gchar* audiorec_path[] = {
	"SF2 Synth",
	NULL
      };
      
      path_strv = audiorec_path;
      
      action = "app.add_audiorec";

      success = TRUE;
      
      g_action_group_activate_action(G_ACTION_GROUP(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context))), "add_audiorec",
 				     NULL);
    }
  }

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  if(success){
    value = g_new0(GValue,
		   1);
    g_value_init(value,
		 G_TYPE_OBJECT);
    
    g_value_set_object(value,
		       window->machine->data);
  
    ags_functional_test_util_stack_add(value);
  }
  
  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_add_machine(gchar *submenu,
				     gchar *machine_name)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;
  
  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_add_machine_driver_program;
  
  driver_program->n_params = 2;

  /* param string vector */
  driver_program->param_strv = g_malloc(3 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("submenu");
  driver_program->param_strv[1] = g_strdup("machine_name");
  driver_program->param_strv[2] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 2);

  g_value_init(driver_program->param,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param,
		     submenu);

  g_value_init(driver_program->param + 1,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param + 1,
		     machine_name);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_preferences_open_driver_program(guint n_params,
							 gchar **param_strv,
							 GValue *param)
{
  AgsWindow *window;
  
  AgsApplicationContext *application_context;

  static gchar* path_strv[] = {
    "_Preferences",
    NULL
  };

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  ags_functional_test_util_header_bar_menu_button_click(window->app_button,
							path_strv,
							"app.preferences");

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_preferences_open()
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_preferences_open_driver_program;
  
  driver_program->n_params = 0;

  /* param string vector */
  driver_program->param_strv = NULL;
  
  /* param value array */
  driver_program->param = NULL;
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_preferences_close_driver_program(guint n_params,
							  gchar **param_strv,
							  GValue *param)
{
  AgsPreferences *preferences;
  
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  preferences = ags_ui_provider_get_preferences(AGS_UI_PROVIDER(application_context));

  ags_functional_test_util_dialog_close(preferences);
  
  ags_functional_test_util_reaction_time_long();
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_null),
						      &ags_functional_test_util_default_timeout,
						      &preferences);
}

void
ags_functional_test_util_preferences_close()
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_preferences_close_driver_program;
  
  driver_program->n_params = 0;

  /* param string vector */
  driver_program->param_strv = NULL;
  
  /* param value array */
  driver_program->param = NULL;
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_about_open()
{
  //TODO:JK: 
}

void
ags_functional_test_util_about_close()
{
  //TODO:JK: 
}

void
ags_functional_test_util_navigation_expand()
{
  //TODO:JK: 
}

void
ags_functional_test_util_navigation_bpm(gdouble bpm)
{
  //TODO:JK: 
}

void
ags_functional_test_util_navigation_rewind()
{
  //TODO:JK: 
}

void
ags_functional_test_util_navigation_prev()
{
  //TODO:JK: 
}

void
ags_functional_test_util_navigation_play()
{
  //TODO:JK: 
}

void
ags_functional_test_util_navigation_stop()
{
  //TODO:JK: 
}

void
ags_functional_test_util_navigation_next()
{
  //TODO:JK: 
}

void
ags_functional_test_util_navigation_forward()
{
  //TODO:JK: 
}

void
ags_functional_test_util_navigation_loop()
{
  //TODO:JK: 
}

void
ags_functional_test_util_navigation_position(gdouble position)
{
  //TODO:JK: 
}

void
ags_functional_test_util_navigation_loop_left(gdouble loop_left)
{
  //TODO:JK: 
}

void
ags_functional_test_util_navigation_loop_right(gdouble loop_right)
{
  //TODO:JK: 
}

void
ags_functional_test_util_navigation_exclude_sequencers()
{
  //TODO:JK: 
}

void
ags_functional_test_util_composite_toolbar_cursor_click_driver_program(guint n_params,
								       gchar **param_strv,
								       GValue *param)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;  
  GtkToggleButton *position;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  position = composite_toolbar->position;
  
  ags_functional_test_util_fake_toggle_button_click(position);
}

void
ags_functional_test_util_composite_toolbar_cursor_click()
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_composite_toolbar_cursor_click_driver_program;
  
  driver_program->n_params = 0;

  /* param string vector */
  driver_program->param_strv = NULL;
  
  /* param value array */
  driver_program->param = NULL;
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_composite_toolbar_edit_click_driver_program(guint n_params,
								     gchar **param_strv,
								     GValue *param)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;  
  GtkToggleButton *edit;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  edit = composite_toolbar->edit;
  
  ags_functional_test_util_fake_toggle_button_click(edit);
}

void
ags_functional_test_util_composite_toolbar_edit_click()
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_composite_toolbar_edit_click_driver_program;
  
  driver_program->n_params = 0;

  /* param string vector */
  driver_program->param_strv = NULL;
  
  /* param value array */
  driver_program->param = NULL;
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_composite_toolbar_delete_click_driver_program(guint n_params,
								       gchar **param_strv,
								       GValue *param)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;  
  GtkToggleButton *clear;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  clear = composite_toolbar->clear;

  ags_functional_test_util_fake_toggle_button_click(clear);
}

void
ags_functional_test_util_composite_toolbar_delete_click()
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_composite_toolbar_delete_click_driver_program;
  
  driver_program->n_params = 0;

  /* param string vector */
  driver_program->param_strv = NULL;

  /* param value array */
  driver_program->param = NULL;
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_composite_toolbar_select_click_driver_program(guint n_params,
								       gchar **param_strv,
								       GValue *param)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;  
  GtkToggleButton *select;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  select = composite_toolbar->select;

  ags_functional_test_util_fake_toggle_button_click(select);
}

void
ags_functional_test_util_composite_toolbar_select_click()
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_composite_toolbar_select_click_driver_program;
  
  driver_program->n_params = 0;

  /* param string vector */
  driver_program->param_strv = NULL;

  /* param value array */
  driver_program->param = NULL;
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_composite_toolbar_invert_click_driver_program(guint n_params,
								       gchar **param_strv,
								       GValue *param)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;  
  GtkButton *invert;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  invert = composite_toolbar->invert;

  ags_functional_test_util_fake_button_click(invert);
}

void
ags_functional_test_util_composite_toolbar_invert_click()
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_composite_toolbar_invert_click_driver_program;
  
  driver_program->n_params = 0;

  /* param string vector */
  driver_program->param_strv = NULL;

  /* param value array */
  driver_program->param = NULL;
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_composite_toolbar_paste_click_driver_program(guint n_params,
								      gchar **param_strv,
								      GValue *param)
{
  //TODO:JK: implement me
}

void
ags_functional_test_util_composite_toolbar_paste_click()
{
  //TODO:JK: implement me
}

void
ags_functional_test_util_composite_toolbar_copy_click_driver_program(guint n_params,
								     gchar **param_strv,
								     GValue *param)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;  
  GtkButton *copy;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  copy = composite_toolbar->copy;

  ags_functional_test_util_fake_button_click(copy);
}

void
ags_functional_test_util_composite_toolbar_copy_click()
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_composite_toolbar_copy_click_driver_program;
  
  driver_program->n_params = 0;

  /* param string vector */
  driver_program->param_strv = NULL;

  /* param value array */
  driver_program->param = NULL;
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_composite_toolbar_cut_click_driver_program(guint n_params,
								    gchar **param_strv,
								    GValue *param)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;  
  GtkButton *cut;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  cut = composite_toolbar->cut;

  ags_functional_test_util_fake_button_click(cut);
}

void
ags_functional_test_util_composite_toolbar_cut_click()
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_composite_toolbar_cut_click_driver_program;
  
  driver_program->n_params = 0;

  /* param string vector */
  driver_program->param_strv = NULL;

  /* param value array */
  driver_program->param = NULL;
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_composite_toolbar_zoom_driver_program(guint n_params,
							       gchar **param_strv,
							       GValue *param)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;  
  GtkComboBox *zoom;

  AgsApplicationContext *application_context;

  guint nth_zoom;

  nth_zoom = g_value_get_uint(param);
  
  application_context = ags_application_context_get_instance();
  
  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  zoom = composite_toolbar->zoom;
  
  gtk_combo_box_set_active(zoom,
			   nth_zoom);
}

void
ags_functional_test_util_composite_toolbar_zoom(guint nth_zoom)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_composite_toolbar_zoom_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_zoom");
  driver_program->param_strv[2] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_zoom);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_machine_selector_add_driver_program(guint n_params,
							     gchar **param_strv,
							     GValue *param)
{
  AgsWindow *window;
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;

  AgsApplicationContext *application_context;

  GList *start_list;

  gchar *action;
  
  guint nth_machine;

  nth_machine = g_value_get_uint(param);
  
  application_context = ags_application_context_get_instance();
  
  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  start_list = ags_window_get_machine(window);
  
  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);
  
  action = g_strdup_printf("machine_selector.add-%s",
			   machine->uid);
  
  gtk_widget_activate_action_variant(composite_editor->machine_selector,
				     action,
				     NULL);

  g_free(action);
}

void
ags_functional_test_util_machine_selector_add(guint nth_machine)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_selector_add_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[2] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_machine_selector_select_driver_program(guint n_params,
								gchar **param_strv,
								GValue *param)
{
  AgsCompositeEditor *composite_editor;

  AgsApplicationContext *application_context;

  GList *start_list, *list;
  
  gchar *machine_str;

  machine_str = g_value_get_string(param);
  
  application_context = ags_application_context_get_instance();
  
  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  list = 
    start_list = ags_machine_selector_get_machine_radio_button(composite_editor->machine_selector);

  while(list != NULL){
    if(!g_strcmp0(gtk_check_button_get_label(list->data),
		  machine_str)){
      gtk_check_button_set_active(list->data,
				  TRUE);
      
      break;
    }

    list = list->next;
  }
}

void
ags_functional_test_util_machine_selector_select(gchar *machine_str)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_selector_select_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("machine_str");
  driver_program->param_strv[2] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param,
		     machine_str);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_machine_selector_reverse_mapping()
{
  //TODO:JK: implement me
}

void
ags_functional_test_util_machine_selector_shift_piano(guint nth_shift)
{
  //TODO:JK: implement me
}

void
ags_functional_test_util_notation_edit_delete_point(guint x,
						    guint y)
{
  //TODO:JK: implement me
}

void
ags_functional_test_util_notation_edit_add_point_driver_program(guint n_params,
								gchar **param_strv,
								GValue *param)
{
  AgsWindow *window;
  AgsMachine *selected_machine;
  AgsCompositeEditor *composite_editor;

  AgsApplicationContext *application_context;

  GList *start_list;

  GtkAllocation allocation;

  gchar *action;

  gdouble zoom_factor;
  guint channel_count;
  gdouble viewport_x, viewport_y;
  gdouble event_x, event_y;
  guint x0;
  guint x1;
  guint y;

  x0 = g_value_get_uint(param);
  x1 = g_value_get_uint(param + 1);

  y = g_value_get_uint(param + 2);
  
  application_context = ags_application_context_get_instance();
  
  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  selected_machine = composite_editor->selected_machine;

  g_object_get(selected_machine->audio,
	       "input-pads", &channel_count,
	       NULL);
  
  gtk_widget_get_allocation(GTK_WIDGET(AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->drawing_area),
			    &allocation);

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_editor->toolbar->zoom));

  if((x0 * AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->control_width) < zoom_factor * gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->hscrollbar)) ||
     (x0 * AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->control_width) + ((x1 - x0) * AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->control_width) > zoom_factor * gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->hscrollbar)) + (zoom_factor * allocation.width)){
    gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->hscrollbar),
			     (x0 * AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->control_width) / zoom_factor);
  }
  
  if((AGS_NAVIGATION_MAX_POSITION_TICS * AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->control_width) > (zoom_factor * allocation.width)){
    viewport_x = zoom_factor * gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->hscrollbar));
  }else{
    viewport_x = 0.0;
  }

  if((y * AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->control_height) < gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->vscrollbar)) ||
      (y * AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->control_height) + AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->control_height > gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->vscrollbar)) + allocation.height){
    gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->vscrollbar),
			     (y * AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->control_height));
  }

  if((channel_count * AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->control_height) > allocation.height){
    viewport_y = gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->vscrollbar));
  }else{
    viewport_y = 0.0;
  }
  
  event_x = (x0 * AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->control_width);
  event_y = (y * AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->control_height);

  g_signal_emit_by_name(AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->gesture_controller,
			"pressed",
			1,
			(event_x - viewport_x) / zoom_factor,
			event_y - viewport_y);

  event_x = (x1 * AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->control_width);

  g_signal_emit_by_name(AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->gesture_controller,
			"released",
			1,
			(event_x - viewport_x) / zoom_factor,
			event_y - viewport_y);
}

void
ags_functional_test_util_notation_edit_add_point(guint x0, guint x1,
						 guint y)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_notation_edit_add_point_driver_program;
  
  driver_program->n_params = 3;

  /* param string vector */
  driver_program->param_strv = g_malloc(4 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("x0");
  driver_program->param_strv[1] = g_strdup("x1");
  driver_program->param_strv[2] = g_strdup("y");
  driver_program->param_strv[3] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 3);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   x0);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   x1);

  g_value_init(driver_program->param + 2,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 2,
		   y);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_notation_edit_select_region(guint x0, guint x1,
						     guint y0, guint y1)
{
  //TODO:JK: 
}

void
ags_functional_test_util_automation_edit_delete_point(guint nth_index,
						      guint x,
						      guint y)
{
  //TODO:JK: 
}

void
ags_functional_test_util_automation_edit_add_point(guint nth_index,
						   guint x,
						   guint y)
{
  //TODO:JK: 
}

void
ags_functional_test_util_automation_edit_select_region(guint nth_index,
						       guint x0, guint x1,
						       guint y0, guint y1)
{
  //TODO:JK: 
}

void
ags_functional_test_util_preferences_click_tab_driver_program(guint n_params,
							      gchar **param_strv,
							      GValue *param)
{
  AgsPreferences *preferences;
  
  AgsApplicationContext *application_context;

  guint nth_tab;

  nth_tab = g_value_get_uint(param);
  
  application_context = ags_application_context_get_instance();

  preferences = ags_ui_provider_get_preferences(AGS_UI_PROVIDER(application_context));

  /* click tab */
  gtk_notebook_set_current_page(preferences->notebook,
				nth_tab);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_preferences_click_tab(guint nth_tab)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_preferences_click_tab_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_tab");
  driver_program->param_strv[2] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_tab);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_audio_preferences_buffer_size_driver_program(guint n_params,
								      gchar **param_strv,
								      GValue *param)
{
  AgsPreferences *preferences;
  AgsSoundcardEditor *soundcard_editor;
  
  AgsApplicationContext *application_context;

  GList *start_list;

  guint nth_backend;
  guint buffer_size;

  nth_backend = g_value_get_uint(param);

  buffer_size = g_value_get_uint(param + 1);
  
  application_context = ags_application_context_get_instance();

  preferences = ags_ui_provider_get_preferences(AGS_UI_PROVIDER(application_context));

  /* click tab */
  gtk_notebook_set_current_page(preferences->notebook,
				1);

  ags_functional_test_util_reaction_time_long();

  start_list = ags_audio_preferences_get_soundcard_editor(preferences->audio_preferences);
  soundcard_editor = g_list_nth_data(start_list,
				     nth_backend);
  
  g_list_free(start_list);

  gtk_spin_button_set_value(soundcard_editor->buffer_size,
			    (gdouble) buffer_size);
    
  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_audio_preferences_buffer_size(guint nth_backend,
						       guint buffer_size)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_audio_preferences_buffer_size_driver_program;
  
  driver_program->n_params = 2;

  /* param string vector */
  driver_program->param_strv = g_malloc(3 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_backend");
  driver_program->param_strv[1] = g_strdup("buffer_size");
  driver_program->param_strv[2] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 2);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_backend);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   buffer_size);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_audio_preferences_samplerate_driver_program(guint n_params,
								     gchar **param_strv,
								     GValue *param)
{
  AgsPreferences *preferences;
  AgsSoundcardEditor *soundcard_editor;
  
  AgsApplicationContext *application_context;

  GList *start_list;

  guint nth_backend;
  guint samplerate;

  nth_backend = g_value_get_uint(param);

  samplerate = g_value_get_uint(param + 1);
  
  application_context = ags_application_context_get_instance();

  preferences = ags_ui_provider_get_preferences(AGS_UI_PROVIDER(application_context));

  /* click tab */
  gtk_notebook_set_current_page(preferences->notebook,
				1);

  ags_functional_test_util_reaction_time_long();

  start_list = ags_audio_preferences_get_soundcard_editor(preferences->audio_preferences);
  soundcard_editor = g_list_nth_data(start_list,
				     nth_backend);
  
  g_list_free(start_list);

  gtk_spin_button_set_value(soundcard_editor->samplerate,
			    (gdouble) samplerate);
    
  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_audio_preferences_samplerate(guint nth_backend,
						      guint samplerate)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_audio_preferences_samplerate_driver_program;
  
  driver_program->n_params = 2;

  /* param string vector */
  driver_program->param_strv = g_malloc(3 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_backend");
  driver_program->param_strv[1] = g_strdup("samplerate");
  driver_program->param_strv[2] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 2);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_backend);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   samplerate);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_machine_menu_button_click_driver_program(guint n_params,
								  gchar **param_strv,
								  GValue *param)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  GtkMenuButton *menu_button;

  AgsApplicationContext *application_context;

  GList *start_list;

  gchar **item_path;
  gchar *action;

  guint nth_machine;

  nth_machine = g_value_get_uint(param);
  
  menu_button = g_value_get_object(param + 1);

  item_path = g_value_get_pointer(param + 2);
  action = g_value_get_string(param + 3);
  
  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  /* retrieve machine */
  start_list = window->machine;
  machine = g_list_nth_data(start_list,
			    nth_machine);
  
  if(!g_strcmp0(action, "machine.move_up")){
    gtk_widget_activate_action_variant(machine,
				       "machine.move_up",
				       NULL);
  }else if(!g_strcmp0(action, "machine.move_down")){
    gtk_widget_activate_action_variant(machine,
				       "machine.move_down",
				       NULL);
  }else if(!g_strcmp0(action, "machine.hide")){
    gtk_widget_activate_action_variant(machine,
				       "machine.hide",
				       NULL);
  }else if(!g_strcmp0(action, "machine.show")){
    gtk_widget_activate_action_variant(machine,
				       "machine.show",
				       NULL);
  }else if(!g_strcmp0(action, "machine.destroy")){
    gtk_widget_activate_action_variant(machine,
				       "machine.destroy",
				       NULL);
  }else if(!g_strcmp0(action, "machine.rename")){
    gtk_widget_activate_action_variant(machine,
				       "machine.rename",
				       NULL);
  }else if(!g_strcmp0(action, "machine.rename_audio")){
    gtk_widget_activate_action_variant(machine,
				       "machine.rename_audio",
				       NULL);
  }else if(!g_strcmp0(action, "machine.reposition_audio")){
    gtk_widget_activate_action_variant(machine,
				       "machine.reposition_audio",
				       NULL);
  }else if(!g_strcmp0(action, "machine.properties")){
    gtk_widget_activate_action_variant(machine,
				       "machine.properties",
				       NULL);
  }else if(!g_strcmp0(action, "machine.sticky_controls")){
    gtk_widget_activate_action_variant(machine,
				       "machine.sticky_controls",
				       NULL);
  }else if(!g_strcmp0(action, "machine.copy_pattern")){
    gtk_widget_activate_action_variant(machine,
				       "machine.copy_pattern",
				       NULL);
  }else if(!g_strcmp0(action, "machine.paste_pattern")){
    gtk_widget_activate_action_variant(machine,
				       "machine.paste_pattern",
				       NULL);
  }else if(!g_strcmp0(action, "machine.envelope")){
    gtk_widget_activate_action_variant(machine,
				       "machine.envelope",
				       NULL);
  }else if(!g_strcmp0(action, "machine.audio_connection")){
    gtk_widget_activate_action_variant(machine,
				       "machine.audio_connection",
				       NULL);
  }else if(!g_strcmp0(action, "machine.midi_connection")){
    gtk_widget_activate_action_variant(machine,
				       "machine.midi_connection",
				       NULL);
  }else if(!g_strcmp0(action, "machine.audio_export")){
    gtk_widget_activate_action_variant(machine,
				       "machine.audio_export",
				       NULL);
  }else if(!g_strcmp0(action, "machine.midi_export")){
    gtk_widget_activate_action_variant(machine,
				       "machine.midi_export",
				       NULL);
  }else if(!g_strcmp0(action, "machine.audio_import")){
    gtk_widget_activate_action_variant(machine,
				       "machine.audio_import",
				       NULL);
  }else if(!g_strcmp0(action, "machine.midi_import")){
    gtk_widget_activate_action_variant(machine,
				       "machine.midi_import",
				       NULL);
  }  
}

void
ags_functional_test_util_machine_menu_button_click(guint nth_machine,
						   GtkMenuButton *menu_button,
						   gchar **item_path,
						   gchar *action)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_menu_button_click;
  
  driver_program->n_params = 4;

  /* param string vector */
  driver_program->param_strv = g_malloc(5 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = g_strdup("menu_button");
  driver_program->param_strv[2] = g_strdup("item_path");
  driver_program->param_strv[3] = g_strdup("action");
  driver_program->param_strv[4] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 4);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  g_value_init(driver_program->param + 1,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param + 1,
		     menu_button);

  g_value_init(driver_program->param + 2,
	       G_TYPE_POINTER);
  g_value_set_pointer(driver_program->param + 2,
		      item_path);
  
  g_value_init(driver_program->param + 3,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param + 3,
		     action);
  
  ags_functional_test_util_add_driver_program(driver_program);    
}

void
ags_functional_test_util_machine_move_up_driver_program(guint n_params,
							gchar **param_strv,
							GValue *param)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsApplicationContext *application_context;

  GList *start_list;

  guint nth_machine;
  
  static gchar* path_strv[] = {
    "move up",
    NULL
  };

  nth_machine = g_value_get_uint(param);

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  /* retrieve machine */
  start_list = ags_window_get_machine(window);
  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);

  gtk_widget_activate_action_variant(machine,
				     "machine.move_up",
				     NULL);
}

void
ags_functional_test_util_machine_move_up(guint nth_machine)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_move_up_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  ags_functional_test_util_add_driver_program(driver_program);    
}

void
ags_functional_test_util_machine_move_down_driver_program(guint n_params,
							  gchar **param_strv,
							  GValue *param)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsApplicationContext *application_context;

  GList *start_list;

  guint nth_machine;
  
  static gchar* path_strv[] = {
    "move down",
    NULL
  };

  nth_machine = g_value_get_uint(param);

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  /* retrieve machine */
  start_list = ags_window_get_machine(window);
  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);

  /* activate move down */  
  gtk_widget_activate_action_variant(machine,
				     "machine.move_down",
				     NULL);
}

void
ags_functional_test_util_machine_move_down(guint nth_machine)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_move_down_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  ags_functional_test_util_add_driver_program(driver_program);    
}

void
ags_functional_test_util_machine_hide_driver_program(guint n_params,
						     gchar **param_strv,
						     GValue *param)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsApplicationContext *application_context;

  GList *start_list;

  guint nth_machine;
  
  static gchar* path_strv[] = {
    "hide",
    NULL
  };

  nth_machine = g_value_get_uint(param);

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  /* retrieve machine */
  start_list = ags_window_get_machine(window);
  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);
  
  /* activate hide */
  gtk_widget_activate_action_variant(machine,
				     "machine.hide",
				     NULL);
}

void
ags_functional_test_util_machine_hide(guint nth_machine)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_hide_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  ags_functional_test_util_add_driver_program(driver_program);    
}

void
ags_functional_test_util_machine_show_driver_program(guint n_params,
						     gchar **param_strv,
						     GValue *param)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsApplicationContext *application_context;

  GList *start_list;

  guint nth_machine;
  
  static gchar* path_strv[] = {
    "show",
    NULL
  };

  nth_machine = g_value_get_uint(param);

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  /* retrieve machine */
  start_list = ags_window_get_machine(window);
  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);
  
  /* activate show */  
  gtk_widget_activate_action_variant(machine,
				     "machine.show",
				     NULL);
}

void
ags_functional_test_util_machine_show(guint nth_machine)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_show_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  ags_functional_test_util_add_driver_program(driver_program);    
}

void
ags_functional_test_util_machine_destroy_driver_program(guint n_params,
							gchar **param_strv,
							GValue *param)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsApplicationContext *application_context;

  GList *start_list;

  guint nth_machine;
  
  static gchar* path_strv[] = {
    "destroy",
    NULL
  };

  nth_machine = g_value_get_uint(param);
  
  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  /* retrieve machine */
  start_list = ags_window_get_machine(window);
  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);
  
  /* activate hide */
#if 0
  ags_functional_test_util_machine_menu_button_click(nth_machine,
						     machine->context_menu_button,
						     path_strv,
						     "machine.destroy");
#else
  gtk_widget_activate_action_variant(machine,
				     "machine.destroy",
				     NULL);
#endif
}

void
ags_functional_test_util_machine_destroy(guint nth_machine)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_destroy_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  ags_functional_test_util_add_driver_program(driver_program);    
}

void
ags_functional_test_util_machine_rename_open_driver_program(guint n_params,
							    gchar **param_strv,
							    GValue *param)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsApplicationContext *application_context;

  GList *start_list;

  guint nth_machine;
  
  static gchar* path_strv[] = {
    "rename",
    NULL
  };

  nth_machine = g_value_get_uint(param);

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  /* retrieve machine */
  start_list = ags_window_get_machine(window);
  machine = g_list_nth_data(start_list,
			    nth_machine);
  
  /* activate rename */  
  gtk_widget_activate_action_variant(machine,
				     "machine.rename",
				     NULL);
}

void
ags_functional_test_util_machine_rename_open(guint nth_machine)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_rename_open_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  ags_functional_test_util_add_driver_program(driver_program);    
}

void
ags_functional_test_util_machine_rename_close(guint nth_machine)
{
  //TODO:JK: implement me
}

void
ags_functional_test_util_machine_rename_set_name(guint nth_machine,
						 gchar *name)
{
  //TODO:JK: implement me 
}

GtkWidget*
ags_functional_test_util_get_machine_editor_dialog_line(GtkWidget *machine_editor_dialog,
							guint nth_pad, guint nth_audio_channel,
							gboolean is_output)
{
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  
  GList *start_list, *list;

  if(machine_editor_dialog == NULL){
    return(NULL);
  }

  if(is_output){
    machine_editor_listing = AGS_MACHINE_EDITOR_DIALOG(machine_editor_dialog)->machine_editor->output_editor_listing;
  }else{
    machine_editor_listing = AGS_MACHINE_EDITOR_DIALOG(machine_editor_dialog)->machine_editor->input_editor_listing;
  }

  start_list = ags_machine_editor_listing_get_pad(machine_editor_listing);
  
  machine_editor_pad = g_list_nth_data(start_list,
				       nth_pad);

  if(!AGS_IS_MACHINE_EDITOR_PAD(machine_editor_pad)){
    return(NULL);
  }
  
  /* line editor */
  start_list = ags_machine_editor_pad_get_line(machine_editor_pad);

  machine_editor_line = g_list_nth_data(start_list,
					nth_audio_channel);

  if(!AGS_IS_MACHINE_EDITOR_LINE(machine_editor_line)){
    return(NULL);
  }
  
  return(machine_editor_line);
}

void
ags_functional_test_util_machine_editor_dialog_open_driver_program(guint n_params,
								   gchar **param_strv,
								   GValue *param)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsApplicationContext *application_context;

  GList *start_list;
  
  guint nth_machine;
  
  static gchar* path_strv[] = {
    "properties",
    NULL
  };

  nth_machine = g_value_get_uint(param);

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  /* retrieve machine */
  start_list = ags_window_get_machine(window);
  
  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);
  
  /* activate properties */
  gtk_widget_activate_action_variant(machine,
				     "machine.properties",
				     NULL);
}

void
ags_functional_test_util_machine_editor_dialog_open(guint nth_machine)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_open_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  ags_functional_test_util_add_driver_program(driver_program);    
}

void
ags_functional_test_util_machine_editor_dialog_click_tab_driver_program(guint n_params,
									gchar **param_strv,
									GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  
  GList *start_list, *list;
  
  guint nth_machine;
  guint nth_tab;

  nth_machine = g_value_get_uint(param);
  
  nth_tab = g_value_get_uint(param + 1);
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);

  /* click tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  gtk_notebook_set_current_page(machine_editor->notebook,
				nth_tab);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_machine_editor_dialog_click_tab(guint nth_machine,
							 guint nth_tab)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_click_tab_driver_program;
  
  driver_program->n_params = 2;

  /* param string vector */
  driver_program->param_strv = g_malloc(3 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = g_strdup("nth_tab");
  driver_program->param_strv[2] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 2);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   nth_tab);
  
  ags_functional_test_util_add_driver_program(driver_program);    
}

void
ags_functional_test_util_machine_editor_dialog_click_enable_driver_program(guint n_params,
									   gchar **param_strv,
									   GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  GtkButton *enable_button;
  
  GList *start_list, *list;

  guint nth_machine;
  guint nth_tab;

  nth_machine = g_value_get_uint(param);
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */
  enable_button = NULL;
  
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);

  /* click tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      enable_button = machine_editor->output_editor_listing->enabled;
    }
    break;
  case 1:
    {
      enable_button = machine_editor->input_editor_listing->enabled;
    }
    break;
  case 2:
    {
      enable_button = machine_editor->output_editor_collection->enabled;
    }
    break;
  case 3:
    {
      enable_button = machine_editor->input_editor_collection->enabled;
    }
    break;
  case 4:
    {
      enable_button = machine_editor->resize_editor->enabled;
    }
    break;
  }
  
  ags_functional_test_util_fake_check_button_click(enable_button);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_machine_editor_dialog_click_enable(guint nth_machine)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_click_enable_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);
  
  ags_functional_test_util_add_driver_program(driver_program);    
}

void
ags_functional_test_util_machine_editor_dialog_link_set_driver_program(guint n_params,
								       gchar **param_strv,
								       GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  AgsLinkEditor *link_editor;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *start_list, *list;

  gchar *link_name;
  gchar *value;

  guint nth_machine;
  guint nth_pad, nth_audio_channel;
  guint link_nth_line;
  guint nth_tab;
  gboolean success;

  nth_machine = g_value_get_uint(param);

  nth_pad = g_value_get_uint(param + 1);
  nth_audio_channel = g_value_get_uint(param + 2);

  link_name = g_value_get_string(param + 3);

  link_nth_line = g_value_get_uint(param + 4);

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine= g_list_nth_data(start_list,
			   nth_machine);

  g_list_free(start_list);

  /* set link */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(machine_editor->output_editor_listing);
    }
    break;
  case 1:
    {
      machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(machine_editor->input_editor_listing);
    }
    break;
  default:
    return;
  }

  start_list = ags_machine_editor_listing_get_pad(machine_editor_listing);
  
  machine_editor_pad = g_list_nth_data(start_list,
				       nth_pad);
  
  /* expander */
  gtk_expander_set_expanded(machine_editor_pad->expander,
			    TRUE);

  /* line editor */
  start_list = ags_machine_editor_pad_get_line(machine_editor_pad);
  machine_editor_line = g_list_nth_data(start_list,
					nth_audio_channel);
  
  /* link editor */
  link_editor = machine_editor_line->link_editor;

  /* set link */
  model = gtk_combo_box_get_model(GTK_COMBO_BOX(link_editor->combo));
  success = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);
      
      if(!g_strcmp0(link_name,
		    value)){
	gtk_combo_box_set_active_iter((GtkComboBox *) link_editor->combo,
				      &iter);
	success = TRUE;
	
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }      
    
  /* set link line */
  gtk_spin_button_set_value(link_editor->spin_button,
			    link_nth_line);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_machine_editor_dialog_link_set(guint nth_machine,
							guint nth_pad, guint nth_audio_channel,
							gchar *link_name, guint link_nth_line)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_link_set_driver_program;
  
  driver_program->n_params = 5;

  /* param string vector */
  driver_program->param_strv = g_malloc(6 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = g_strdup("nth_pad");
  driver_program->param_strv[2] = g_strdup("nth_audio_channel");
  driver_program->param_strv[3] = g_strdup("link_name");
  driver_program->param_strv[4] = g_strdup("link_nth_line");
  driver_program->param_strv[5] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 5);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   nth_pad);

  g_value_init(driver_program->param + 2,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 2,
		   nth_audio_channel);

  g_value_init(driver_program->param + 3,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param + 3,
		     link_name);

  g_value_init(driver_program->param + 4,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 4,
		   link_nth_line);
  
  ags_functional_test_util_add_driver_program(driver_program);    
}

void
ags_functional_test_util_machine_editor_dialog_link_open(guint nth_machine,
							 guint pad, guint audio_channel)
{
  //TODO:JK: implement me
}

void
ags_functional_test_util_machine_editor_dialog_effect_add_driver_program(guint n_params,
									 gchar **param_strv,
									 GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  AgsLineMemberEditor *member_editor;
  GtkButton *add_button;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *start_list, *list;

  gchar *value;

  guint nth_machine;
  guint nth_tab;
  guint nth_pad;
  guint nth_audio_channel;

  nth_machine = g_value_get_uint(param);

  nth_pad = g_value_get_uint(param + 1);
  nth_audio_channel = g_value_get_uint(param + 2);
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */
  machine = NULL;
  
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);

  /* get tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      machine_editor_listing = machine_editor->output_editor_listing;
    }
    break;
  case 1:
    {
      machine_editor_listing = machine_editor->input_editor_listing;
    }
    break;
  default:
    return;
  }

  start_list = ags_machine_editor_listing_get_pad(machine_editor_listing);
  
  machine_editor_pad = g_list_nth_data(start_list,
				       nth_pad);

  /* expander */
  gtk_expander_set_expanded(machine_editor_pad->expander,
			    TRUE);

  /* line editor */
  start_list = ags_machine_editor_pad_get_line(machine_editor_pad);
  machine_editor_line = g_list_nth_data(start_list,
					nth_audio_channel);
  
  /* line member editor */
  member_editor = machine_editor_line->line_member_editor;

  /* add button */
  add_button = member_editor->add;

  ags_functional_test_util_fake_button_click(add_button);
  
  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_machine_editor_dialog_effect_add(guint nth_machine,
							  guint nth_pad, guint nth_audio_channel)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_effect_add_driver_program;
  
  driver_program->n_params = 3;

  /* param string vector */
  driver_program->param_strv = g_malloc(4 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = g_strdup("nth_pad");
  driver_program->param_strv[2] = g_strdup("nth_audio_channel");
  driver_program->param_strv[3] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 3);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   nth_pad);

  g_value_init(driver_program->param + 2,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 2,
		   nth_audio_channel);
  
  ags_functional_test_util_add_driver_program(driver_program);      
}

void
ags_functional_test_util_machine_editor_dialog_effect_remove_driver_program(guint n_params,
									    gchar **param_strv,
									    GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  AgsLineMemberEditor *member_editor;
  AgsLineMemberEditorEntry *member_editor_entry;
  GtkDialog **effect_dialog;
  GtkButton *check_button;
  GtkButton *remove_button;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *start_list, *list;

  gchar *value;

  guint nth_machine;
  guint nth_tab;
  guint nth_pad;
  guint nth_audio_channel;
  guint nth_effect;

  nth_machine = g_value_get_uint(param);

  nth_pad = g_value_get_uint(param + 1);
  nth_audio_channel = g_value_get_uint(param + 2);
  
  nth_effect = g_value_get_uint(param + 3);

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);

  /* get tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(machine_editor->output_editor_listing);
    }
    break;
  case 1:
    {
      machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(machine_editor->input_editor_listing);
    }
    break;
  default:
    return;
  }

  start_list = ags_machine_editor_listing_get_pad(machine_editor_listing);
  
  machine_editor_pad = g_list_nth_data(start_list,
				       nth_pad);

  /* expander */
  gtk_expander_set_expanded(machine_editor_pad->expander,
			    TRUE);

  /* line editor */
  start_list = ags_machine_editor_pad_get_line(machine_editor_pad);

  machine_editor_line = g_list_nth_data(start_list,
					nth_audio_channel);

  /* line member editor */
  member_editor = machine_editor_line->line_member_editor;
  
  /* effect dialog and remove button */
  effect_dialog = &(member_editor->plugin_browser);
  remove_button = member_editor->remove;

  /* check button */
  start_list = ags_line_member_editor_get_entry(member_editor);

  member_editor_entry = g_list_nth_data(start_list,
					nth_effect);
  
  /* click check button */
  ags_functional_test_util_fake_check_button_click(member_editor_entry->check_button);

  /* click remove */
  ags_functional_test_util_fake_button_click(remove_button);
  
  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_machine_editor_dialog_effect_remove(guint nth_machine,
							     guint nth_pad, guint nth_audio_channel,
							     guint nth_effect)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_effect_remove_driver_program;
  
  driver_program->n_params = 4;

  /* param string vector */
  driver_program->param_strv = g_malloc(5 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = g_strdup("nth_pad");
  driver_program->param_strv[2] = g_strdup("nth_audio_channel");
  driver_program->param_strv[3] = g_strdup("nth_effect");
  driver_program->param_strv[4] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 4);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   nth_pad);

  g_value_init(driver_program->param + 2,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 2,
		   nth_audio_channel);
  
  g_value_init(driver_program->param + 3,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 3,
		   nth_effect);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_machine_editor_dialog_effect_plugin_type_driver_program(guint n_params,
										 gchar **param_strv,
										 GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  AgsLineMemberEditor *member_editor;
  AgsLineMemberEditorEntry *member_editor_entry;
  AgsPluginBrowser *plugin_browser;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *start_list, *list;

  gchar *plugin_type;
  gchar *value;

  guint nth_machine;
  guint nth_tab;
  guint nth_pad;
  guint nth_audio_channel;

  nth_machine = g_value_get_uint(param);

  nth_pad = g_value_get_uint(param + 1);
  nth_audio_channel = g_value_get_uint(param + 2);
  
  plugin_type = g_value_get_string(param + 3);
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(start_list,
			    nth_machine);
  
  g_list_free(start_list);

  /* get tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(machine_editor->output_editor_listing);
    }
    break;
  case 1:
    {
      machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(machine_editor->input_editor_listing);
    }
    break;
  default:
    return;
  }

  /* pad editor */
  start_list = ags_machine_editor_listing_get_pad(machine_editor_listing);

  machine_editor_pad = g_list_nth_data(start_list,
				       nth_pad);
    
  /* line editor */
  start_list = ags_machine_editor_pad_get_line(machine_editor_pad);

  machine_editor_line = g_list_nth_data(start_list,
					nth_audio_channel);

  /* line member editor */
  member_editor = machine_editor_line->line_member_editor;

  plugin_browser = member_editor->plugin_browser;
  
  /* set plugin type */
  if(!g_ascii_strncasecmp(plugin_type,
			  "ladspa",
			  7)){
    gtk_combo_box_set_active((GtkComboBox *) plugin_browser->plugin_type,
			     1);
  }else if(!g_ascii_strncasecmp(plugin_type,
				"lv2",
				4)){
    gtk_combo_box_set_active((GtkComboBox *) plugin_browser->plugin_type,
			     0);
  }else if(!g_ascii_strncasecmp(plugin_type,
				"vst3",
				5)){
    gtk_combo_box_set_active((GtkComboBox *) plugin_browser->plugin_type,
			     2);
  }
  
  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_machine_editor_dialog_effect_plugin_type(guint nth_machine,
								  guint nth_pad, guint nth_audio_channel,
								  gchar *plugin_type)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_effect_plugin_type_driver_program;
  
  driver_program->n_params = 4;

  /* param string vector */
  driver_program->param_strv = g_malloc(5 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = g_strdup("nth_pad");
  driver_program->param_strv[2] = g_strdup("nth_audio_channel");
  driver_program->param_strv[3] = g_strdup("plugin_type");
  driver_program->param_strv[4] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 4);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   nth_pad);

  g_value_init(driver_program->param + 2,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 2,
		   nth_audio_channel);
  
  g_value_init(driver_program->param + 3,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param + 3,
		     plugin_type);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_machine_editor_dialog_ladspa_filename_driver_program(guint n_params,
									      gchar **param_strv,
									      GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  AgsLineMemberEditor *member_editor;
  AgsPluginBrowser *plugin_browser;
  AgsLadspaBrowser *ladspa_browser;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *start_list, *list;

  gchar *filename;
  gchar *value;
  
  guint nth_machine;
  guint nth_tab;
  guint nth_pad;
  guint nth_audio_channel;  
  gboolean success;
  
  nth_machine = g_value_get_uint(param);

  nth_pad = g_value_get_uint(param + 1);
  nth_audio_channel = g_value_get_uint(param + 2);
  
  filename = g_value_get_string(param + 3);

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(start_list,
			    nth_machine);
  
  g_list_free(start_list);

  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(machine_editor->output_editor_listing);
    }
    break;
  case 1:
    {
      machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(machine_editor->input_editor_listing);
    }
    break;
  default:
    return;
  }

  /* pad editor */
  start_list = ags_machine_editor_listing_get_pad(machine_editor_listing);

  machine_editor_pad = g_list_nth_data(start_list,
				       nth_pad);
    
  /* line editor */
  start_list = ags_machine_editor_pad_get_line(machine_editor_pad);

  machine_editor_line = g_list_nth_data(start_list,
					nth_audio_channel);

  /* line member editor */
  member_editor = machine_editor_line->line_member_editor;

  plugin_browser = member_editor->plugin_browser;

  ladspa_browser = plugin_browser->ladspa_browser;
  
  /* set plugin filename */
  model = gtk_tree_view_get_model(ladspa_browser->filename_tree_view);
  success = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      value = NULL;
      
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);
      
      if(g_str_has_suffix(value,
			  filename)){
	gtk_tree_view_set_cursor(ladspa_browser->filename_tree_view,
				 gtk_tree_model_get_path(model,
							 &iter),
				 NULL,
				 FALSE);
	gtk_tree_view_row_activated(ladspa_browser->filename_tree_view,
				    gtk_tree_model_get_path(model,
							    &iter),
				    NULL);
	
	success = TRUE;
	
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }      
  
  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_machine_editor_dialog_ladspa_filename(guint nth_machine,
							       guint nth_pad, guint nth_audio_channel,
							       gchar *filename)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_ladspa_filename_driver_program;
  
  driver_program->n_params = 4;

  /* param string vector */
  driver_program->param_strv = g_malloc(5 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = g_strdup("nth_pad");
  driver_program->param_strv[2] = g_strdup("nth_audio_channel");
  driver_program->param_strv[3] = g_strdup("filename");
  driver_program->param_strv[4] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 4);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   nth_pad);

  g_value_init(driver_program->param + 2,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 2,
		   nth_audio_channel);
  
  g_value_init(driver_program->param + 3,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param + 3,
		     filename);
  
  ags_functional_test_util_add_driver_program(driver_program);
}

void
ags_functional_test_util_machine_editor_dialog_ladspa_effect_driver_program(guint n_params,
									    gchar **param_strv,
									    GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  AgsLineMemberEditor *member_editor;
  AgsPluginBrowser *plugin_browser;
  AgsLadspaBrowser *ladspa_browser;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *start_list, *list;

  gchar *effect;
  gchar *value;
  
  guint nth_machine;
  guint nth_tab;
  guint nth_pad;
  guint nth_audio_channel;  
  gboolean success;
  
  nth_machine = g_value_get_uint(param);

  nth_pad = g_value_get_uint(param + 1);
  nth_audio_channel = g_value_get_uint(param + 2);
  
  effect = g_value_get_string(param + 3);

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);

  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(machine_editor->output_editor_listing);
    }
    break;
  case 1:
    {
      machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(machine_editor->input_editor_listing);
    }
    break;
  default:
    return;
  }

  /* pad editor */
  start_list = ags_machine_editor_listing_get_pad(machine_editor_listing);

  machine_editor_pad = g_list_nth_data(start_list,
				       nth_pad);
    
  /* line editor */
  start_list = ags_machine_editor_pad_get_line(machine_editor_pad);

  machine_editor_line = g_list_nth_data(start_list,
					nth_audio_channel);

  /* line member editor */
  member_editor = machine_editor_line->line_member_editor;

  plugin_browser = member_editor->plugin_browser;

  ladspa_browser = plugin_browser->ladspa_browser;
  
  /* set plugin effect */
  model = gtk_tree_view_get_model(ladspa_browser->effect_tree_view);
  success = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      value = NULL;
      
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);
      
      if(!g_strcmp0(effect,
		    value)){
	gtk_tree_view_set_cursor(ladspa_browser->effect_tree_view,
				 gtk_tree_model_get_path(model,
							 &iter),
				 NULL,
				 FALSE);	
	gtk_tree_view_row_activated(ladspa_browser->effect_tree_view,
				    gtk_tree_model_get_path(model,
							    &iter),
				    NULL);

	success = TRUE;
	
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }        
  
  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_machine_editor_dialog_ladspa_effect(guint nth_machine,
							     guint nth_pad, guint nth_audio_channel,
							     gchar *effect)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_ladspa_effect_driver_program;
  
  driver_program->n_params = 4;

  /* param string vector */
  driver_program->param_strv = g_malloc(5 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = g_strdup("nth_pad");
  driver_program->param_strv[2] = g_strdup("nth_audio_channel");
  driver_program->param_strv[3] = g_strdup("effect");
  driver_program->param_strv[4] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 4);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   nth_pad);

  g_value_init(driver_program->param + 2,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 2,
		   nth_audio_channel);
  
  g_value_init(driver_program->param + 3,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param + 3,
		     effect);
  
  ags_functional_test_util_add_driver_program(driver_program);
}

void
ags_functional_test_util_machine_editor_dialog_lv2_filename_driver_program(guint n_params,
									   gchar **param_strv,
									   GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  AgsLineMemberEditor *member_editor;
  AgsPluginBrowser *plugin_browser;
  AgsLv2Browser *lv2_browser;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *start_list, *list;

  gchar *filename;
  gchar *value;
  
  guint nth_machine;
  guint nth_tab;
  guint nth_pad;
  guint nth_audio_channel;  
  gboolean success;
  
  nth_machine = g_value_get_uint(param);

  nth_pad = g_value_get_uint(param + 1);
  nth_audio_channel = g_value_get_uint(param + 2);
  
  filename = g_value_get_string(param + 3);

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(start_list,
			    nth_machine);
  
  g_list_free(start_list);

  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(machine_editor->output_editor_listing);
    }
    break;
  case 1:
    {
      machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(machine_editor->input_editor_listing);
    }
    break;
  default:
    return;
  }

  /* pad editor */
  start_list = ags_machine_editor_listing_get_pad(machine_editor_listing);

  machine_editor_pad = g_list_nth_data(start_list,
				       nth_pad);
    
  /* line editor */
  start_list = ags_machine_editor_pad_get_line(machine_editor_pad);

  machine_editor_line = g_list_nth_data(start_list,
					nth_audio_channel);

  /* line member editor */
  member_editor = machine_editor_line->line_member_editor;

  plugin_browser = member_editor->plugin_browser;

  lv2_browser = plugin_browser->lv2_browser;
  
  /* set plugin filename */
  model = gtk_tree_view_get_model(lv2_browser->filename_tree_view);
  success = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      value = NULL;
      
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);
      
      if(g_str_has_suffix(value,
			  filename)){
	gtk_tree_view_set_cursor(lv2_browser->filename_tree_view,
				 gtk_tree_model_get_path(model,
							 &iter),
				 NULL,
				 FALSE);
	gtk_tree_view_row_activated(lv2_browser->filename_tree_view,
				    gtk_tree_model_get_path(model,
							    &iter),
				    NULL);
	
	success = TRUE;
	
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }      
  
  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_machine_editor_dialog_lv2_filename(guint nth_machine,
							    guint nth_pad, guint nth_audio_channel,
							    gchar *filename)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_lv2_filename_driver_program;
  
  driver_program->n_params = 4;

  /* param string vector */
  driver_program->param_strv = g_malloc(5 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = g_strdup("nth_pad");
  driver_program->param_strv[2] = g_strdup("nth_audio_channel");
  driver_program->param_strv[3] = g_strdup("filename");
  driver_program->param_strv[4] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 4);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   nth_pad);

  g_value_init(driver_program->param + 2,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 2,
		   nth_audio_channel);
  
  g_value_init(driver_program->param + 3,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param + 3,
		     filename);
  
  ags_functional_test_util_add_driver_program(driver_program);
}

void
ags_functional_test_util_machine_editor_dialog_lv2_effect_driver_program(guint n_params,
									 gchar **param_strv,
									 GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  AgsLineMemberEditor *member_editor;
  AgsPluginBrowser *plugin_browser;
  AgsLv2Browser *lv2_browser;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *start_list, *list;

  gchar *effect;
  gchar *value;
  
  guint nth_machine;
  guint nth_tab;
  guint nth_pad;
  guint nth_audio_channel;
  gboolean success;
  
  nth_machine = g_value_get_uint(param);

  nth_pad = g_value_get_uint(param + 1);
  nth_audio_channel = g_value_get_uint(param + 2);
  
  effect = g_value_get_string(param + 3);

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);

  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(machine_editor->output_editor_listing);
    }
    break;
  case 1:
    {
      machine_editor_listing = AGS_MACHINE_EDITOR_LISTING(machine_editor->input_editor_listing);
    }
    break;
  default:
    return;
  }

  /* pad editor */
  start_list = ags_machine_editor_listing_get_pad(machine_editor_listing);

  machine_editor_pad = g_list_nth_data(start_list,
				       nth_pad);

  g_list_free(start_list);
    
  /* line editor */
  start_list = ags_machine_editor_pad_get_line(machine_editor_pad);

  machine_editor_line = g_list_nth_data(start_list,
					nth_audio_channel);

  g_list_free(start_list);

  /* line member editor */
  member_editor = machine_editor_line->line_member_editor;

  plugin_browser = member_editor->plugin_browser;

  lv2_browser = plugin_browser->lv2_browser;
  
  /* set plugin effect */
  model = gtk_tree_view_get_model(lv2_browser->effect_tree_view);
  success = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      value = NULL;
      
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);
      
      if(!g_strcmp0(effect,
		    value)){
	gtk_tree_view_set_cursor(lv2_browser->effect_tree_view,
				 gtk_tree_model_get_path(model,
							 &iter),
				 NULL,
				 FALSE);
	gtk_tree_view_row_activated(lv2_browser->effect_tree_view,
				    gtk_tree_model_get_path(model,
							    &iter),
				    NULL);
	
	success = TRUE;
	
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }        
  
  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_machine_editor_dialog_lv2_effect(guint nth_machine,
							  guint nth_pad, guint nth_audio_channel,
							  gchar *effect)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_lv2_effect_driver_program;
  
  driver_program->n_params = 4;

  /* param string vector */
  driver_program->param_strv = g_malloc(5 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = g_strdup("nth_pad");
  driver_program->param_strv[2] = g_strdup("nth_audio_channel");
  driver_program->param_strv[3] = g_strdup("effect");
  driver_program->param_strv[4] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 4);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   nth_pad);

  g_value_init(driver_program->param + 2,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 2,
		   nth_audio_channel);
  
  g_value_init(driver_program->param + 3,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param + 3,
		     effect);
  
  ags_functional_test_util_add_driver_program(driver_program);
}

void
ags_functional_test_util_machine_editor_dialog_bulk_add_driver_program(guint n_params,
								       gchar **param_strv,
								       GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorCollection *machine_editor_collection;
  GtkButton *add_collection;

  GList *start_list, *list;

  guint nth_machine;
  guint nth_tab;

  nth_machine = g_value_get_uint(param);
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);

  /* get tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_OUTPUT_TAB:
    {
      machine_editor_collection = machine_editor->output_editor_collection;
    }
    break;
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_INPUT_TAB:
    {
      machine_editor_collection = machine_editor->input_editor_collection;
    }
    break;
  default:
    return;
  }

  add_collection = machine_editor_collection->add_bulk;
  
  /* add collection */
  ags_functional_test_util_fake_button_click(add_collection);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_machine_editor_dialog_bulk_add(guint nth_machine)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_bulk_add_driver_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);
  
  ags_functional_test_util_add_driver_program(driver_program);    
}

void
ags_functional_test_util_machine_editor_dialog_bulk_remove(guint nth_machine,
							   guint nth_bulk)
{
  //TODO:JK: 
}

void
ags_functional_test_util_machine_editor_dialog_bulk_link_driver_program(guint n_params,
									gchar **param_strv,
									GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorCollection *machine_editor_collection;
  AgsMachineEditorBulk *machine_editor_bulk;

  GList *start_list, *list;

  GtkTreeModel *model;
  
  GtkTreeIter iter;

  gchar *link;
  gchar *value;
  
  guint nth_machine;
  guint nth_bulk;
  guint nth_tab;
  gboolean success;

  nth_machine = g_value_get_uint(param);

  nth_bulk = g_value_get_uint(param + 1);

  link = g_value_get_string(param + 2);
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);
  
  /* get notebook tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_OUTPUT_TAB:
    {
      machine_editor_collection = machine_editor->output_editor_collection;
    }
    break;
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_INPUT_TAB:
    {
      machine_editor_collection = machine_editor->input_editor_collection;
    }
    break;
  default:
    return;
  }

  /* get bulk editor */
  start_list = ags_machine_editor_collection_get_bulk(machine_editor_collection);

  machine_editor_bulk = g_list_nth_data(start_list,
					nth_bulk);

  g_list_free(start_list);
  
  /* set link */
  model = gtk_combo_box_get_model(machine_editor_bulk->link);

  success = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);
      
      if(!g_strcmp0(link,
		    value)){
	gtk_combo_box_set_active_iter(machine_editor_bulk->link,
				      &iter);
	
	success = TRUE;
	
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }      
  
  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_machine_editor_dialog_bulk_link(guint nth_machine,
							 guint nth_bulk,
							 gchar *link)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_bulk_link_driver_program;
  
  driver_program->n_params = 3;

  /* param string vector */
  driver_program->param_strv = g_malloc(4 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = g_strdup("nth_bulk");
  driver_program->param_strv[2] = g_strdup("link");
  driver_program->param_strv[3] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 3);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   nth_bulk);

  g_value_init(driver_program->param + 2,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param + 2,
		     link);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_machine_editor_dialog_bulk_first_line_driver_program(guint n_params,
									      gchar **param_strv,
									      GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorCollection *machine_editor_collection;
  AgsMachineEditorBulk *machine_editor_bulk;

  GList *start_list, *list;

  GtkTreeModel *model;
  
  GtkTreeIter iter;

  gchar *value;

  guint nth_machine;
  guint nth_tab;
  guint nth_bulk;
  guint first_line;

  nth_machine = g_value_get_uint(param);

  nth_bulk = g_value_get_uint(param + 1);

  first_line = g_value_get_uint(param + 2);
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);
  
  /* get notebook tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_OUTPUT_TAB:
    {
      machine_editor_collection = machine_editor->output_editor_collection;
    }
    break;
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_INPUT_TAB:
    {
      machine_editor_collection = machine_editor->input_editor_collection;
    }
    break;
  default:
    return;
  }

  /* get bulk editor */
  start_list = ags_machine_editor_collection_get_bulk(machine_editor_collection);

  machine_editor_bulk = g_list_nth_data(start_list,
					nth_bulk);

  g_list_free(start_list);

  gtk_spin_button_set_value(machine_editor_bulk->first_line,
			    first_line);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_machine_editor_dialog_bulk_first_line(guint nth_machine,
							       guint nth_bulk,
							       guint first_line)
{  
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_bulk_first_line_driver_program;
  
  driver_program->n_params = 3;

  /* param string vector */
  driver_program->param_strv = g_malloc(4 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = g_strdup("nth_bulk");
  driver_program->param_strv[2] = g_strdup("first_line");
  driver_program->param_strv[3] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 3);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   nth_bulk);

  g_value_init(driver_program->param + 2,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 2,
		   first_line);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_machine_editor_dialog_bulk_link_line_driver_program(guint n_params,
									     gchar **param_strv,
									     GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorCollection *machine_editor_collection;
  AgsMachineEditorBulk *machine_editor_bulk;

  GList *start_list, *list;

  GtkTreeModel *model;
  
  GtkTreeIter iter;

  gchar *value;

  guint nth_machine;
  guint nth_bulk;
  guint first_link_line;
  guint nth_tab;

  nth_machine = g_value_get_uint(param);

  nth_bulk = g_value_get_uint(param + 1);

  first_link_line = g_value_get_uint(param + 2);
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);

  /* get notebook tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_OUTPUT_TAB:
    {
      machine_editor_collection = machine_editor->output_editor_collection;
    }
    break;
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_INPUT_TAB:
    {
      machine_editor_collection = machine_editor->input_editor_collection;
    }
    break;
  default:
    return;
  }

  /* get bulk editor */
  start_list = ags_machine_editor_collection_get_bulk(machine_editor_collection);

  machine_editor_bulk = g_list_nth_data(start_list,
					nth_bulk);

  g_list_free(start_list);

  gtk_spin_button_set_value(machine_editor_bulk->first_link_line,
			    first_link_line);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_machine_editor_dialog_bulk_link_line(guint nth_machine,
							      guint nth_bulk,
							      guint first_link_line)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_bulk_link_line_driver_program;
  
  driver_program->n_params = 3;

  /* param string vector */
  driver_program->param_strv = g_malloc(4 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = g_strdup("nth_bulk");
  driver_program->param_strv[2] = g_strdup("first_link_line");
  driver_program->param_strv[3] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 3);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   nth_bulk);

  g_value_init(driver_program->param + 2,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 2,
		   first_link_line);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_machine_editor_dialog_bulk_count_driver_program(guint n_params,
									 gchar **param_strv,
									 GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorCollection *machine_editor_collection;
  AgsMachineEditorBulk *machine_editor_bulk;

  GList *start_list, *list;

  GtkTreeModel *model;
  
  GtkTreeIter iter;

  gchar *value;

  guint nth_machine;
  guint nth_bulk;
  guint count;
  guint nth_tab;
  
  nth_machine = g_value_get_uint(param);

  nth_bulk = g_value_get_uint(param + 1);

  count = g_value_get_uint(param + 2);

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);

  /* get notebook tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_OUTPUT_TAB:
    {
      machine_editor_collection = machine_editor->output_editor_collection;
    }
    break;
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_INPUT_TAB:
    {
      machine_editor_collection = machine_editor->input_editor_collection;
    }
    break;
  default:
    return;
  }

  /* get bulk editor */
  start_list = ags_machine_editor_collection_get_bulk(machine_editor_collection);

  machine_editor_bulk = g_list_nth_data(start_list,
					nth_bulk);

  g_list_free(start_list);

  gtk_spin_button_set_value(machine_editor_bulk->count,
			    count);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_machine_editor_dialog_bulk_count(guint nth_machine,
							  guint nth_bulk,
							  guint count)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_bulk_count_driver_program;
  
  driver_program->n_params = 3;

  /* param string vector */
  driver_program->param_strv = g_malloc(4 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = g_strdup("nth_bulk");
  driver_program->param_strv[2] = g_strdup("count");
  driver_program->param_strv[3] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 3);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   nth_bulk);

  g_value_init(driver_program->param + 2,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 2,
		   count);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_test_util_machine_editor_dialog_resize_audio_channels_driver_program(guint n_params,
										    gchar **param_strv,
										    GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  
  GList *start_list, *list;

  guint nth_machine;
  guint audio_channels;

  nth_machine = g_value_get_uint(param);
  
  audio_channels = g_value_get_uint(param + 1);

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);

  /* resize audio channels */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  gtk_spin_button_set_value(machine_editor->resize_editor->audio_channels,
			    audio_channels);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_machine_editor_dialog_resize_audio_channels(guint nth_machine,
								     guint audio_channels)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_resize_audio_channels_driver_program;
  
  driver_program->n_params = 2;

  /* param string vector */
  driver_program->param_strv = g_malloc(3 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = g_strdup("audio_channels");
  driver_program->param_strv[2] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 2);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   audio_channels);
  
  ags_functional_test_util_add_driver_program(driver_program);    
}

void
ags_functional_test_util_machine_editor_dialog_resize_inputs_driver_program(guint n_params,
									    gchar **param_strv,
									    GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  
  GList *start_list, *list;

  guint nth_machine;
  guint inputs;

  nth_machine = g_value_get_uint(param);
  
  inputs = g_value_get_uint(param + 1);

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);
  
  /* resize input pads */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  gtk_spin_button_set_value(machine_editor->resize_editor->input_pads,
			    inputs);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_machine_editor_dialog_resize_inputs(guint nth_machine,
							     guint inputs)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_resize_inputs_driver_program;
  
  driver_program->n_params = 2;

  /* param string vector */
  driver_program->param_strv = g_malloc(3 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = g_strdup("inputs");
  driver_program->param_strv[2] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 2);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   inputs);
  
  ags_functional_test_util_add_driver_program(driver_program);    
}

void
ags_functional_test_util_machine_editor_dialog_resize_outputs_driver_program(guint n_params,
									     gchar **param_strv,
									     GValue *param)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  
  GList *start_list, *list;

  guint nth_machine;
  guint outputs;
  
  nth_machine = g_value_get_uint(param);
  
  outputs = g_value_get_uint(param + 1);

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(start_list,
			    nth_machine);

  g_list_free(start_list);
  
  /* resize output pads */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  gtk_spin_button_set_value(machine_editor->resize_editor->output_pads,
			    outputs);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_machine_editor_dialog_resize_outputs(guint nth_machine,
							      guint outputs)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_machine_editor_dialog_resize_outputs_driver_program;
  
  driver_program->n_params = 2;

  /* param string vector */
  driver_program->param_strv = g_malloc(3 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("nth_machine");
  driver_program->param_strv[1] = g_strdup("outputs");
  driver_program->param_strv[2] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 2);

  g_value_init(driver_program->param,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param,
		   nth_machine);

  g_value_init(driver_program->param + 1,
	       G_TYPE_UINT);
  g_value_set_uint(driver_program->param + 1,
		   outputs);
  
  ags_functional_test_util_add_driver_program(driver_program);    
}

void
ags_functional_test_util_machine_edit_copy(guint nth_machine)
{
  //TODO:JK: 
}

void
ags_functional_test_util_line_member_set_toggled(gchar *specifier,
						 gboolean is_toggled)
{
  //TODO:JK: 
}

void
ags_functional_test_util_line_member_set_value(gchar *specifier,
					       gdouble value)
{
  //TODO:JK: 
}

void
ags_functional_test_util_machine_audio_connection_click_tab(guint nth_machine,
							    guint nth_tab)
{
  //TODO:JK: 
}

void
ags_functional_test_util_machine_audio_connection_click_enable(guint nth_machine)
{
  //TODO:JK: 
}

void
ags_functional_test_util_machine_audio_connection_set_device(guint nth_machine,
							     guint pad, guint audio_channel,
							     gchar *device)
{
  //TODO:JK: 
}

void
ags_functional_test_util_machine_audio_connection_set_line(guint nth_machine,
							   uint pad, guint audio_channel,
							   guint line)
{
  //TODO:JK: 
}

void
ags_functional_test_util_machine_audio_connection_bulk_add(guint nth_machine)
{
  //TODO:JK: 
}

void
ags_functional_test_util_machine_audio_connection_bulk_remove(guint nth_machine,
							      guint nth_bulk)
{
  //TODO:JK: 
}

void
ags_functional_test_util_machine_audio_connection_bulk_first_line(guint nth_machine,
								  guint nth_bulk,
								  guint first_line)
{
  //TODO:JK: 
}

void
ags_functional_test_util_machine_audio_connection_bulk_count(guint nth_machine,
							     guint nth_bulk,
							     guint count)
{
  //TODO:JK: 
}

void
ags_functional_test_util_machine_audio_connection_bulk_device(guint nth_machine,
							      guint nth_bulk,
							      gchar *device)
{
  //TODO:JK: 
}

void
ags_functional_test_util_machine_midi_connection_midi_channel(guint nth_machine,
							      guint midi_channel)
{
  //TODO:JK: 
}

void
ags_functional_test_util_machine_midi_connection_audio_start_mapping(guint nth_machine,
								     guint audio_start_mapping)
{
  //TODO:JK: 
}

void
ags_functional_test_util_machine_midi_connection_audio_end_mapping(guint nth_machine,
								   guint audio_end_mapping)
{
  //TODO:JK: 
}

void
ags_functional_test_util_machine_midi_connection_midi_start_mapping(guint nth_machine,
								    guint midi_start_mapping)
{
  //TODO:JK: 
}

void
ags_functional_test_util_machine_midi_connection_midi_end_mapping(guint nth_machine,
								  guint midi_end_mapping)
{
  //TODO:JK: 
}

void
ags_functional_test_util_machine_midi_connection_midi_device(guint nth_machine,
							     gchar *device)
{
  //TODO:JK: 
}

void
ags_functional_test_util_pad_group(guint nth_machine,
				   guint nth_pad)
{
  //TODO:JK: 
}

void
ags_functional_test_util_pad_mute(guint nth_machine,
				  guint nth_pad)
{
  //TODO:JK: 
}

void
ags_functional_test_util_pad_solo(guint nth_machine,
				  guint nth_pad)
{
  //TODO:JK: 
}

void
ags_functional_test_util_pad_play(guint nth_machine,
				  guint nth_pad)
{
  //TODO:JK: 
}

void
ags_functional_test_util_expander(guint nth_machine,
				  guint nth_pad, guint nth_audio_channel)
{
  //TODO:JK: 
}

void
ags_functional_test_util_effect_bulk_add(guint nth_machine)
{
  //TODO:JK: 
}

void
ags_functional_test_util_effect_bulk_remove(guint nth_machine,
					    guint nth_bulk_effect)
{
  //TODO:JK: 
}

void
ags_functional_test_util_effect_bulk_ladspa_filename(guint nth_machine,
						     gchar *filename)
{
  //TODO:JK: 
}

void
ags_functional_test_util_effect_bulk_ladspa_effect(guint nth_machine,
						   gchar *effect)
{
  //TODO:JK: 
}

void
ags_functional_test_util_effect_bulk_lv2_filename(guint nth_machine,
						  gchar *filename)
{
  //TODO:JK: 
}

void
ags_functional_test_util_effect_bulk_lv2_effect(guint nth_machine,
						gchar *effect)
{
  //TODO:JK: 
}

void
ags_functional_test_util_panel_mute_line(guint nth_machine,
					 guint nth_pad, guint nth_audio_channel)
{
  //TODO:JK: 
}

void
ags_functional_test_util_mixer_input_line_volume(guint nth_machine,
						 guint nth_pad, guint nth_audio_channel,
						 gdouble volume)
{
  //TODO:JK: 
}

void
ags_functional_test_util_drum_open(guint nth_machine)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsDrum *drum;
  GtkButton *open_button;
  GtkWidget **open_dialog;
  
  GList *start_list, *list;

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve drum */
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  drum = g_list_nth_data(start_list,
			 nth_machine);

  g_list_free(start_list);
    
  /* open dialog */
  open_button = drum->open;
  open_dialog = &(drum->open_dialog);
  
  /* click open */
  ags_functional_test_util_button_click(open_button);

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_test_util_default_timeout,
						      open_dialog);
}

void
ags_functional_test_util_drum_run(guint nth_machine)
{
  //TODO:JK: 
}

void
ags_functional_test_util_drum_loop(guint nth_machine)
{
  //TODO:JK: 
}

void
ags_functional_test_util_drum_bank_0(guint nth_machine,
				     guint bank_0)
{
  //TODO:JK: 
}

void
ags_functional_test_util_drum_bank_1(guint nth_machine,
				     guint bank_1)
{
  //TODO:JK: 
}

void
ags_functional_test_util_drum_length(guint nth_machine,
				     guint length)
{
  //TODO:JK: 
}

void
ags_functional_test_util_drum_index(guint nth_machine,
				    guint nth_radio)
{
  //TODO:JK: 
}

void
ags_functional_test_util_drum_pad(guint nth_machine,
				  guint nth_pad)
{
  //TODO:JK: 
}

void
ags_functional_test_util_drum_pad_open(guint nth_machine,
				       guint nth_pad)
{
  //TODO:JK: 
}

void
ags_functional_test_util_drum_pad_edit(guint nth_machine,
				       guint nth_pad)
{
  //TODO:JK: 
}

void
ags_functional_test_util_drum_input_line_volume(guint nth_machine,
						guint nth_pad, guint nth_audio_channel,
						gdouble volume)
{
  //TODO:JK: 
}

void
ags_functional_test_util_matrix_run(guint nth_machine)
{
  //TODO:JK: 
}

void
ags_functional_test_util_matrix_loop(guint nth_machine)
{
  //TODO:JK: 
}

void
ags_functional_test_util_matrix_bank_1(guint nth_machine,
				       guint bank_1)
{
  //TODO:JK: 
}

void
ags_functional_test_util_matrix_length(guint nth_machine,
				       guint length)
{
  //TODO:JK: 
}

void
ags_functional_test_util_matrix_gutter(guint nth_machine,
				       guint nth_gutter)
{
  //TODO:JK: 
}

void
ags_functional_test_util_drum_cell(guint nth_machine,
				   guint x, guint y)
{
  //TODO:JK: 
}

void
ags_functional_test_util_synth_auto_update(guint nth_machine)
{
  //TODO:JK: 
}

void
ags_functional_test_util_synth_update(guint nth_machine)
{
  //TODO:JK: 
}

void ags_functional_test_util_oscillator_wave(guint nth_machine,
					      guint nth_oscillator,
					      gchar *wave)
{
  //TODO:JK: 
}

void
ags_functional_test_util_oscillator_phase(guint nth_machine,
					  guint nth_oscillator,
					  gdouble phase)
{
  //TODO:JK: 
}

void
ags_functional_test_util_oscillator_attack(guint nth_machine,
					   guint nth_oscillator,
					   gdouble attack)
{
  //TODO:JK: 
}

void
ags_functional_test_util_oscillator_frequency(guint nth_machine,
					      guint nth_oscillator,
					      gdouble frequency)
{
  //TODO:JK: 
}

void
ags_functional_test_util_oscillator_length(guint nth_machine,
					   guint nth_oscillator,
					   gdouble length)
{
  //TODO:JK: 
}

void
ags_functional_test_util_oscillator_volume(guint nth_machine,
					   guint nth_oscillator,
					   gdouble volume)
{
  //TODO:JK: 
}

void
ags_functional_test_util_ffplayer_open(guint nth_machine)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsFFPlayer *ffplayer;
  GtkButton *open_button;
  GtkWidget **open_dialog;
  
  GList *start_list, *list;

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve ffplayer */
  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  ffplayer = g_list_nth_data(start_list,
			     nth_machine);

  g_list_free(start_list);
  
  /* open dialog */
  open_button = ffplayer->open;
  open_dialog = &(ffplayer->open_dialog);

  /* click open */
  ags_functional_test_util_button_click(open_button);

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_test_util_default_timeout,
						      open_dialog);
}

void
ags_functional_test_util_ffplayer_preset(guint nth_machine,
					 gchar *preset)
{
  //TODO:JK: 
}

void
ags_functional_test_util_ffplayer_instrument(guint nth_machine,
					     gchar *instrument)
{
  //TODO:JK: 
}
