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

#include "../ags_functional_test_util.h"

void ags_functional_drum_test_add_test();

int ags_functional_drum_test_init_suite();
int ags_functional_drum_test_clean_suite();

void ags_functional_drum_test_open_drum_kit();
void ags_functional_drum_test_resize_pads();
void ags_functional_drum_test_resize_audio_channels();

void ags_functional_drum_test_drum_open_drum_kit(AgsDrum *drum,
						 GSList *filename);
void ags_functional_drum_test_drum_open_drum_kit_driver_program(guint n_params,
								gchar **param_strv,
								GValue *param);

#define AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME (3.0 * G_USEC_PER_SEC)

#define AGS_FUNCTIONAL_DRUM_TEST_OPEN_DRUM_KIT_PATH "/usr/share/hydrogen/data/drumkits/HardElectro1/"

#define AGS_FUNCTIONAL_DRUM_TEST_RESIZE_OUTPUT_PADS (5)
#define AGS_FUNCTIONAL_DRUM_TEST_RESIZE_INPUT_PADS (15)

#define AGS_FUNCTIONAL_DRUM_TEST_RESIZE_AUDIO_CHANNELS (7)

#define AGS_FUNCTIONAL_DRUM_TEST_CONFIG "[generic]\n" \
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

extern AgsApplicationContext *ags_application_context;

struct timespec ags_functional_drum_test_default_timeout = {
  300,
  0,
};

void
ags_functional_drum_test_add_test()
{
  /* add the tests to the suite */
  if((CU_add_test(pSuite, "functional test of AgsDrum open drum kit", ags_functional_drum_test_open_drum_kit) == NULL) ||
     (CU_add_test(pSuite, "functional test of AgsDrum resize pads", ags_functional_drum_test_resize_pads) == NULL) ||
     (CU_add_test(pSuite, "functional test of AgsDrum resize audio channels", ags_functional_drum_test_resize_audio_channels) == NULL)){
    CU_cleanup_registry();
      
    exit(CU_get_error());
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();

  ags_functional_test_util_quit();
  
  CU_cleanup_registry();
  
  exit(CU_get_error());
}

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_drum_test_init_suite()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = ags_application_context;

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_realized),
						      &ags_functional_drum_test_default_timeout,
						      &(gsequencer_application_context->window));
  
  ags_functional_test_util_sync();

  /* window size */
  ags_functional_test_util_file_default_window_resize();

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_drum_test_clean_suite()
{  
  return(0);
}

void
ags_functional_drum_test_drum_open_drum_kit_driver_program(guint n_params,
							   gchar **param_strv,
							   GValue *param)
{
  AgsDrum *drum;

  GSList *filename;

  drum = g_value_get_object(param);
  
  filename = g_value_get_pointer(param + 1);

  ags_machine_open_files(drum,
			 filename,
			 TRUE,
			 TRUE);
}

void
ags_functional_drum_test_drum_open_drum_kit(AgsDrum *drum,
					    GSList *filename)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  if(!AGS_IS_DRUM(drum) ||
     filename == NULL){
    return;
  }

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_drum_test_drum_open_drum_kit_driver_program;
  
  driver_program->n_params = 2;

  /* param string vector */
  driver_program->param_strv = g_malloc(3 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("drum");
  driver_program->param_strv[1] = g_strdup("filename");
  driver_program->param_strv[2] = NULL;
  
  /* param value array */
  driver_program->param = g_new0(GValue,
				 2);

  g_value_init(driver_program->param,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param,
		     drum);

  g_value_init(driver_program->param + 1,
	       G_TYPE_POINTER);
  g_value_set_pointer(driver_program->param + 1,
		      filename);
  
  ags_functional_test_util_add_driver_program(driver_program);  
}

void
ags_functional_drum_test_open_drum_kit()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  AgsDrum *drum;

  AgsFunctionalTestUtilListLengthCondition condition;

  GList *start_list, *list;
  
  guint nth_machine;
  gboolean success;
  
  gsequencer_application_context = ags_application_context;

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_realized),
						      &ags_functional_drum_test_default_timeout,
						      &(gsequencer_application_context->window));

  /* add drum */
  ags_functional_test_util_add_machine(NULL,
				       "Drum");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 1;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_drum_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve drum */
  nth_machine = 0;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_DRUM);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  drum = g_list_nth_data(start_list,
			 nth_machine);

  //TODO:JK: 
#if 0
  /* open dialog */
  ags_functional_test_util_drum_open(0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* open path */
  ags_functional_test_util_file_chooser_open_path(GTK_FILE_CHOOSER(drum->open_dialog),
						  AGS_FUNCTIONAL_DRUM_TEST_OPEN_DRUM_KIT_PATH);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* select all */
  ags_functional_test_util_file_chooser_select_all(GTK_FILE_CHOOSER(drum->open_dialog));

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* response ok */
  ags_functional_test_util_dialog_ok(GTK_DIALOG(drum->open_dialog));

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
#else
  GDir *dir;

  GSList *filename;

  gchar *str;
  
  GError *error;
  
  filename = NULL;

  error = NULL;
  dir = g_dir_open(AGS_FUNCTIONAL_DRUM_TEST_OPEN_DRUM_KIT_PATH,
		   0,
		   &error);

  while((str = g_dir_read_name(dir)) != NULL){
    filename = g_slist_append(filename,
			      str);
  }

  ags_functional_drum_test_drum_open_drum_kit(drum,
					      filename);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
#endif
  
  /* destroy drum */
  ags_functional_test_util_machine_destroy(0);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
}

void
ags_functional_drum_test_resize_pads()
{
  GtkDialog *properties;

  AgsGSequencerApplicationContext *gsequencer_application_context;
  
  AgsDrum *drum;

  AgsFunctionalTestUtilListLengthCondition condition;

  GList *start_list, *list;

  guint nth_machine;
  guint resize_tab;
  gboolean success;
  
  gsequencer_application_context = ags_application_context;

  /* add drum */
  ags_functional_test_util_add_machine(NULL,
				       "Drum");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 1;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_drum_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve drum */
  nth_machine = 0;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_DRUM);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  drum = g_list_nth_data(start_list,
			 nth_machine);

  /*
   * resize output and input pads
   */
  
  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_drum_test_default_timeout,
						      &(AGS_MACHINE(drum)->machine_editor_dialog));

  ags_functional_test_util_sync();
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_drum_test_default_timeout,
						      &(AGS_MACHINE_EDITOR_DIALOG(AGS_MACHINE(drum)->machine_editor_dialog)->machine_editor));

  ags_functional_test_util_sync();

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_drum_test_default_timeout,
						      &(AGS_MACHINE_EDITOR_DIALOG(AGS_MACHINE(drum)->machine_editor_dialog)->machine_editor->resize_editor));
  
  ags_functional_test_util_sync();

  /* click tab */
  resize_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_RESIZE_TAB;
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_machine,
							   resize_tab);  
    
  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* click enable */
  ags_functional_test_util_machine_editor_dialog_click_enable(nth_machine);
    
  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* resize output */
  ags_functional_test_util_machine_editor_dialog_resize_outputs(nth_machine,
								AGS_FUNCTIONAL_DRUM_TEST_RESIZE_OUTPUT_PADS);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* resize input */
  ags_functional_test_util_machine_editor_dialog_resize_inputs(nth_machine,
							       AGS_FUNCTIONAL_DRUM_TEST_RESIZE_INPUT_PADS);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response ok */
  properties = AGS_MACHINE(drum)->machine_editor_dialog;  

  ags_functional_test_util_dialog_ok(properties);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* destroy drum */
  ags_functional_test_util_machine_destroy(0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
}

void
ags_functional_drum_test_resize_audio_channels()
{
  GtkDialog *properties;

  AgsGSequencerApplicationContext *gsequencer_application_context;
  
  AgsDrum *drum;

  AgsFunctionalTestUtilListLengthCondition condition;

  GList *start_list, *list;

  guint nth_machine;
  guint resize_tab;
  gboolean success;
  
  gsequencer_application_context = ags_application_context;

  /* add drum */
  ags_functional_test_util_add_machine(NULL,
				       "Drum");
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 1;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_drum_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve drum */
  nth_machine = 0;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_DRUM);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  drum = g_list_nth_data(start_list,
			  nth_machine);

  /*
   * resize audio channels
   */
  
  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_drum_test_default_timeout,
						      &(AGS_MACHINE(drum)->machine_editor_dialog));

  ags_functional_test_util_sync();
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_drum_test_default_timeout,
						      &(AGS_MACHINE_EDITOR_DIALOG(AGS_MACHINE(drum)->machine_editor_dialog)->machine_editor));

  ags_functional_test_util_sync();

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_drum_test_default_timeout,
						      &(AGS_MACHINE_EDITOR_DIALOG(AGS_MACHINE(drum)->machine_editor_dialog)->machine_editor->resize_editor));
  
  ags_functional_test_util_sync();

  /* click tab */
  resize_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_RESIZE_TAB;
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_machine,
							   resize_tab);  
    
  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* click enable */
  ags_functional_test_util_machine_editor_dialog_click_enable(nth_machine);
    
  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* resize audio channels */
  ags_functional_test_util_machine_editor_dialog_resize_audio_channels(nth_machine,
								       AGS_FUNCTIONAL_DRUM_TEST_RESIZE_AUDIO_CHANNELS);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response ok */
  properties = AGS_MACHINE(drum)->machine_editor_dialog;  

  ags_functional_test_util_dialog_ok(properties);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* destroy drum */
  ags_functional_test_util_machine_destroy(0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_DRUM_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
}

int
main(int argc, char **argv)
{
  gchar *str;

  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFunctionalDrumTest", ags_functional_drum_test_init_suite, ags_functional_drum_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  g_atomic_int_set(&is_available,
		   FALSE);
  
#if defined(AGS_TEST_CONFIG)
  ags_functional_test_util_init(&argc, &argv,
				AGS_TEST_CONFIG);
#else
  if((str = getenv("AGS_TEST_CONFIG")) != NULL){
    ags_functional_test_util_init(&argc, &argv,
				  str);
  }else{
    ags_functional_test_util_init(&argc, &argv,
				  AGS_FUNCTIONAL_DRUM_TEST_CONFIG);
  }
#endif
  
  ags_functional_test_util_do_run(argc, argv,
				  ags_functional_drum_test_add_test, &is_available);

  g_thread_join(ags_functional_test_util_test_runner_thread());

  return(-1);
}
