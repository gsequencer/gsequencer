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

void ags_functional_automation_editor_workflow_test_add_test();

int ags_functional_automation_editor_workflow_test_init_suite();
int ags_functional_automation_editor_workflow_test_clean_suite();

void ags_functional_automation_editor_workflow_test_panel();
void ags_functional_automation_editor_workflow_test_mixer();
void ags_functional_automation_editor_workflow_test_equalizer10();
void ags_functional_automation_editor_workflow_test_drum();
void ags_functional_automation_editor_workflow_test_matrix();
#ifdef AGS_WITH_LIBINSTPATCH
void ags_functional_automation_editor_workflow_test_ffplayer();
#endif
void ags_functional_automation_editor_workflow_test_syncsynth();

#define AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME (3.0 * G_USEC_PER_SEC)

#define AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_CONFIG "[generic]\n" \
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

extern AgsApplicationContext *ags_application_context;

extern struct timespec ags_functional_automation_editor_workflow_test_default_timeout = {
  300,
  0,
};

void ags_functional_automation_editor_workflow_test_add_test()
{
  /* add the tests to the suite */
  if((CU_add_test(pSuite, "functional test of GSequencer automation editor workflow AgsPanel", ags_functional_automation_editor_workflow_test_panel) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer automation editor workflow AgsMixer", ags_functional_automation_editor_workflow_test_mixer) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer automation editor workflow AgsEqualizer10", ags_functional_automation_editor_workflow_test_equalizer10) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer automation editor workflow AgsDrum", ags_functional_automation_editor_workflow_test_drum) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer automation editor workflow AgsMatrix", ags_functional_automation_editor_workflow_test_matrix) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer automation editor workflow AgsSyncsynth", ags_functional_automation_editor_workflow_test_syncsynth) == NULL)
#ifdef AGS_WITH_LIBINSTPATCH
     || (CU_add_test(pSuite, "functional test of GSequencer automation editor workflow AgsFFPlayer", ags_functional_automation_editor_workflow_test_ffplayer) == NULL)
#endif
     ){
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
ags_functional_automation_editor_workflow_test_init_suite()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = ags_application_context;

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_realized),
						      &ags_functional_automation_editor_workflow_test_default_timeout,
						      &(gsequencer_application_context->window));

  ags_functional_test_util_sync();

  /* window size */
  ags_functional_test_util_file_default_window_resize();

  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_automation_editor_workflow_test_clean_suite()
{  
  return(0);
}

void
ags_functional_automation_editor_workflow_test_panel()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;
  AgsMachineSelector *machine_selector;
  
  AgsFunctionalTestUtilListLengthCondition condition;
  
  GList *start_list, *list;

  gchar *machine_str;
  
  guint nth_machine;
  gboolean success;

  gsequencer_application_context = ags_application_context;

  nth_machine = 0;

  /* add panel */
  ags_functional_test_util_add_machine(NULL,
				       "Panel");
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* retrieve panel */
  nth_machine = 0;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_PANEL);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);

  /*
   *  add machine to automation editor
   */

  window = AGS_WINDOW(gsequencer_application_context->window);

  composite_editor = window->composite_editor;

  ags_functional_test_util_machine_selector_add(nth_machine);  
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
  
  /* select machine */
  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_functional_test_util_machine_selector_select(machine_str);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* automation edit */
  ags_functional_test_util_header_bar_menu_button_click(window->edit_button,
							NULL,
							"app.edit_automation");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
}

void
ags_functional_automation_editor_workflow_test_mixer()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;
  AgsMachineSelector *machine_selector;

  GList *start_list, *list;

  AgsFunctionalTestUtilListLengthCondition condition;

  gchar *machine_str;
  
  guint nth_machine;
  gboolean success;

  gsequencer_application_context = ags_application_context;

  /* add mixer */
  ags_functional_test_util_add_machine(NULL,
				       "Mixer");
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 2;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_automation_editor_workflow_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve mixer */
  nth_machine = 1;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_MIXER);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);

  /*
   *  add machine to automation editor
   */

  window = AGS_WINDOW(gsequencer_application_context->window);

  composite_editor = window->composite_editor;

  ags_functional_test_util_machine_selector_add(nth_machine);  
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
  
  /* select machine */
  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_functional_test_util_machine_selector_select(machine_str);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* automation edit */
  ags_functional_test_util_header_bar_menu_button_click(window->edit_button,
							NULL,
							"app.edit_automation");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
}

void
ags_functional_automation_editor_workflow_test_equalizer10()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;
  AgsMachineSelector *machine_selector;

  GList *start_list, *list;

  AgsFunctionalTestUtilListLengthCondition condition;

  gchar *machine_str;
  
  guint nth_machine;
  gboolean success;

  gsequencer_application_context = ags_application_context;

  /* add mixer */
  ags_functional_test_util_add_machine(NULL,
				       "Equalizer");
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 3;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_automation_editor_workflow_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve equalizer */
  nth_machine = 2;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_EQUALIZER10);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);

  /*
   *  add machine to automation editor
   */

  window = AGS_WINDOW(gsequencer_application_context->window);

  composite_editor = window->composite_editor;

  ags_functional_test_util_machine_selector_add(nth_machine);  
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
  
  /* select machine */
  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_functional_test_util_machine_selector_select(machine_str);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* automation edit */
  ags_functional_test_util_header_bar_menu_button_click(window->edit_button,
							NULL,
							"app.edit_automation");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
}

void
ags_functional_automation_editor_workflow_test_drum()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;
  AgsMachineSelector *machine_selector;

  GList *start_list, *list;

  AgsFunctionalTestUtilListLengthCondition condition;

  gchar *machine_str;
  
  guint nth_machine;
  gboolean success;

  gsequencer_application_context = ags_application_context;

  /* add mixer */
  ags_functional_test_util_add_machine(NULL,
				       "Drum");
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 4;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_automation_editor_workflow_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve drum */
  nth_machine = 3;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_DRUM);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);

  /*
   *  add machine to automation editor
   */

  window = AGS_WINDOW(gsequencer_application_context->window);

  composite_editor = window->composite_editor;

  ags_functional_test_util_machine_selector_add(nth_machine);  
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
  
  /* select machine */
  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_functional_test_util_machine_selector_select(machine_str);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* automation edit */
  ags_functional_test_util_header_bar_menu_button_click(window->edit_button,
							NULL,
							"app.edit_automation");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
}

void
ags_functional_automation_editor_workflow_test_matrix()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;
  AgsMachineSelector *machine_selector;

  GList *start_list, *list;

  AgsFunctionalTestUtilListLengthCondition condition;

  gchar *machine_str;
  
  guint nth_machine;
  gboolean success;

  gsequencer_application_context = ags_application_context;

  /* add mixer */
  ags_functional_test_util_add_machine(NULL,
				       "Matrix");
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 5;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_automation_editor_workflow_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve matrix */
  nth_machine = 4;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_MATRIX);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);

  /*
   *  add machine to automation editor
   */

  window = AGS_WINDOW(gsequencer_application_context->window);

  composite_editor = window->composite_editor;

  ags_functional_test_util_machine_selector_add(nth_machine);  
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
  
  /* select machine */
  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_functional_test_util_machine_selector_select(machine_str);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* automation edit */
  ags_functional_test_util_header_bar_menu_button_click(window->edit_button,
							NULL,
							"app.edit_automation");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
}

void
ags_functional_automation_editor_workflow_test_syncsynth()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;
  AgsMachineSelector *machine_selector;

  GList *start_list, *list;

  AgsFunctionalTestUtilListLengthCondition condition;

  gchar *machine_str;
  
  guint nth_machine;
  gboolean success;

  gsequencer_application_context = ags_application_context;

  /* add mixer */
  ags_functional_test_util_add_machine(NULL,
				       "Syncsynth");
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 6;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_automation_editor_workflow_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve syncsynth */
  nth_machine = 5;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_SYNCSYNTH);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);

  /*
   *  add machine to automation editor
   */

  window = AGS_WINDOW(gsequencer_application_context->window);

  composite_editor = window->composite_editor;

  ags_functional_test_util_machine_selector_add(nth_machine);  
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
  
  /* select machine */
  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_functional_test_util_machine_selector_select(machine_str);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* automation edit */
  ags_functional_test_util_header_bar_menu_button_click(window->edit_button,
							NULL,
							"app.edit_automation");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
}

#if defined(AGS_WITH_LIBINSTPATCH)
void
ags_functional_automation_editor_workflow_test_ffplayer()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;
  AgsMachineSelector *machine_selector;

  GList *start_list, *list;

  AgsFunctionalTestUtilListLengthCondition condition;

  gchar *machine_str;
  
  guint nth_machine;
  gboolean success;

  gsequencer_application_context = ags_application_context;

  /* add mixer */
  ags_functional_test_util_add_machine(NULL,
				       "FPlayer");
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 7;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_automation_editor_workflow_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve ffplayer */
  nth_machine = 6;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_FFPLAYER);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);

  /*
   *  add machine to automation editor
   */

  window = AGS_WINDOW(gsequencer_application_context->window);

  composite_editor = window->composite_editor;

  ags_functional_test_util_machine_selector_add(nth_machine);  
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
  
  /* select machine */
  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_functional_test_util_machine_selector_select(machine_str);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* automation edit */
  ags_functional_test_util_header_bar_menu_button_click(window->edit_button,
							NULL,
							"app.edit_automation");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
}
#endif

int
main(int argc, char **argv)
{
  gchar *str;
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFunctionalAutomationEditorWorkflowTest", ags_functional_automation_editor_workflow_test_init_suite, ags_functional_automation_editor_workflow_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  g_atomic_int_set(&is_available,
		   FALSE);
  g_atomic_int_set(&is_terminated,
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
				  AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_CONFIG);
  }
#endif
  
  ags_functional_test_util_do_run(argc, argv,
				  ags_functional_automation_editor_workflow_test_add_test, &is_available);

  g_atomic_int_set(&is_terminated,
		   TRUE);

  g_thread_join(ags_functional_test_util_test_runner_thread());

  return(-1);
}
