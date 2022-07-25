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

void ags_functional_notation_editor_workflow_test_add_test();

int ags_functional_notation_editor_workflow_test_init_suite();
int ags_functional_notation_editor_workflow_test_clean_suite();

void ags_functional_notation_editor_workflow_test_drum();
void ags_functional_notation_editor_workflow_test_matrix();
#ifdef AGS_WITH_LIBINSTPATCH
void ags_functional_notation_editor_workflow_test_ffplayer();
#endif
void ags_functional_notation_editor_workflow_test_edit_all();
void ags_functional_notation_editor_workflow_test_fill_all();

#define AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME (3.0 * G_USEC_PER_SEC)

#define AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_CONFIG "[generic]\n" \
  "autosave-thread=false\n"						\
  "simple-file=true\n"							\
  "disable-feature=experimental\n"					\
  "segmentation=4/4\n"							\
  "\n"									\
  "[thread]\n"								\
  "model=super-threaded\n"						\
  "super-threaded-scope=channel\n"					\
  "lock-global=ags-thread\n"						\
  "lock-parent=ags-recycling-thread\n"					\
  "\n"									\
  "[soundcard-0]\n"							\
  "backend=alsa\n"							\
  "device=default\n"							\
  "samplerate=44100\n"							\
  "buffer-size=1024\n"							\
  "pcm-channels=2\n"							\
  "dsp-channels=2\n"							\
  "format=16\n"								\
  "\n"									\
  "[recall]\n"								\
  "auto-sense=true\n"							\
  "\n"

CU_pSuite pSuite = NULL;
volatile gboolean is_available;

extern AgsApplicationContext *ags_application_context;

struct timespec ags_functional_notation_editor_workflow_test_default_timeout = {
  300,
  0,
};

void ags_functional_notation_editor_workflow_test_add_test()
{
  /* add the tests to the suite */
  if((CU_add_test(pSuite, "functional test of GSequencer editor workflow AgsDrum", ags_functional_notation_editor_workflow_test_drum) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer editor workflow AgsMatrix", ags_functional_notation_editor_workflow_test_matrix) == NULL) ||
#ifdef AGS_WITH_LIBINSTPATCH
     (CU_add_test(pSuite, "functional test of GSequencer editor workflow AgsFFPlayer", ags_functional_notation_editor_workflow_test_ffplayer) == NULL) ||
#endif
     (CU_add_test(pSuite, "functional test of GSequencer editor workflow edit all", ags_functional_notation_editor_workflow_test_edit_all) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer editor workflow fill all", ags_functional_notation_editor_workflow_test_fill_all) == NULL)){
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
ags_functional_notation_editor_workflow_test_init_suite()
{ 
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = ags_application_context;

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_realized),
						      &ags_functional_notation_editor_workflow_test_default_timeout,
						      &(gsequencer_application_context->window));

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_notation_editor_workflow_test_clean_suite()
{  
  return(0);
}

void
ags_functional_notation_editor_workflow_test_drum()
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

  /* window and editor size */
  ags_functional_test_util_file_default_window_resize();

  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* add drum */
  ags_functional_test_util_add_machine(NULL,
				       "Drum");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 1;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_notation_editor_workflow_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve drum */
  nth_machine = 0;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_DRUM);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);

  /*
   *  add machine to editor
   */

  window = AGS_WINDOW(gsequencer_application_context->window);

  composite_editor = window->composite_editor;

  ags_functional_test_util_machine_selector_add(nth_machine);  
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* select machine */
  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_functional_test_util_machine_selector_select(machine_str);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
}

void
ags_functional_notation_editor_workflow_test_matrix()
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

  /* add matrix */
  ags_functional_test_util_add_machine(NULL,
				       "Matrix");
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 2;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_notation_editor_workflow_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve matrix */
  nth_machine = 1;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_MATRIX);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);

  /*
   *  add machine to editor
   */
  window = AGS_WINDOW(gsequencer_application_context->window);

  composite_editor = window->composite_editor;

  ags_functional_test_util_machine_selector_add(nth_machine);  
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* select machine */
  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_functional_test_util_machine_selector_select(machine_str);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
}

#ifdef AGS_WITH_LIBINSTPATCH
void
ags_functional_notation_editor_workflow_test_ffplayer()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;
  AgsMachineSelector *machine_selector;
  AgsMachineSelection *machine_selection;

  AgsFunctionalTestUtilListLengthCondition condition;
    
  GList *start_list, *list;

  gchar *machine_str;

  guint nth_machine;
  gboolean success;

  gsequencer_application_context = ags_application_context;

  /* add ffplayer */
  ags_functional_test_util_add_machine(NULL,
				       "FPlayer");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 3;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_notation_editor_workflow_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve drum */
  nth_machine = 2;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_FFPLAYER);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);

  /*
   *  add machine to editor
   */

  window = AGS_WINDOW(gsequencer_application_context->window);

  composite_editor = window->composite_editor;

  ags_functional_test_util_machine_selector_add(nth_machine);  
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* select machine */
  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_functional_test_util_machine_selector_select(machine_str);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
}
#endif

void
ags_functional_notation_editor_workflow_test_edit_all()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *current_machine;
  
  GList *start_list;
  
  gchar *machine_str;
  
  guint nth_machine;
  guint i, j;
  gboolean success;

  gsequencer_application_context = ags_application_context;

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  
  /*
   * edit drum
   */

  nth_machine = 0;

  current_machine = g_list_nth_data(start_list,
				    nth_machine);

  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(current_machine),
				current_machine->machine_name);
  
  ags_functional_test_util_machine_selector_select(machine_str);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* set zoom */
  ags_functional_test_util_composite_toolbar_zoom(AGS_FUNCTIONAL_TEST_UTIL_TOOLBAR_ZOOM_1_TO_4);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* edit tool */
  ags_functional_test_util_composite_toolbar_edit_click();

  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* add drum kick pattern 2/4 */
  success = TRUE;
  
  for(i = 0; i < 64 && success;){
    ags_functional_test_util_notation_edit_add_point(i, i + 1,
						     0);

    ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
    ags_functional_test_util_sync();

    i += 8;
  }

  for(i = 4; i < 64 && success;){
    ags_functional_test_util_notation_edit_add_point(i, i + 1,
						     1);

    ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
    ags_functional_test_util_sync();

    i += 8;
  }

  /*
   * edit matrix
   */

  nth_machine = 1;

  current_machine = g_list_nth_data(start_list,
				    nth_machine);

  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(current_machine),
				current_machine->machine_name);
  
  ags_functional_test_util_machine_selector_select(machine_str);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();  

  /* set zoom */
  ags_functional_test_util_composite_toolbar_zoom(AGS_FUNCTIONAL_TEST_UTIL_TOOLBAR_ZOOM_1_TO_8);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* edit tool */
  ags_functional_test_util_composite_toolbar_edit_click();

  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* add matrix baseline pattern 8/8 */
  success = TRUE;
  
  for(i = 0; i < 32 && success;){
    for(j = 0; j < 56 && success;){
      ags_functional_test_util_notation_edit_add_point(i * 8 + j, i * 8 + j,
						       15);

      ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
      ags_functional_test_util_sync();

      j += 8;
    }

    if(!success){
      break;
    }
    
    ags_functional_test_util_notation_edit_add_point(i * 8 + 56, i * 8 + 56,
						     14);

    ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
    ags_functional_test_util_sync();
    
    i += 8;
  }

#ifdef AGS_WITH_LIBINSTPATCH
  /*
   * edit ffplayer
   */

  nth_machine = 2;

  current_machine = g_list_nth_data(start_list,
				    nth_machine);

  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(current_machine),
				current_machine->machine_name);
  
  ags_functional_test_util_machine_selector_select(machine_str);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();  

  /* set zoom */
  ags_functional_test_util_composite_toolbar_zoom(AGS_FUNCTIONAL_TEST_UTIL_TOOLBAR_ZOOM_1_TO_4);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* edit tool */
  ags_functional_test_util_composite_toolbar_edit_click();

  ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* add ffplayer note 4/4 */
  success = TRUE;
  
  for(i = 0; i < 64 && success;){
    ags_functional_test_util_notation_edit_add_point(i, i + 1,
						     rand() % 10);

    ags_functional_test_util_idle(AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_DEFAULT_IDLE_TIME);
  
    ags_functional_test_util_sync();
    
    i += 8;
  }
#endif
}

void
ags_functional_notation_editor_workflow_test_fill_all()
{
  guint nth_machine;

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
  pSuite = CU_add_suite("AgsFunctionalNotationEditorWorkflowTest", ags_functional_notation_editor_workflow_test_init_suite, ags_functional_notation_editor_workflow_test_clean_suite);
  
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
				  AGS_FUNCTIONAL_NOTATION_EDITOR_WORKFLOW_TEST_CONFIG);
  }
#endif
  
  ags_functional_test_util_do_run(argc, argv,
				  ags_functional_notation_editor_workflow_test_add_test, &is_available);

  g_thread_join(ags_functional_test_util_test_runner_thread());

  return(-1);
}
