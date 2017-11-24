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

#include <ags/test/X/libgsequencer.h>

#include "gsequencer_setup_util.h"
#include "ags_functional_test_util.h"

void ags_functional_editor_workflow_test_add_test();

int ags_functional_editor_workflow_test_init_suite();
int ags_functional_editor_workflow_test_clean_suite();

void ags_functional_editor_workflow_test_drum();
void ags_functional_editor_workflow_test_matrix();
#ifdef AGS_WITH_LIBINSTPATCH
void ags_functional_editor_workflow_test_ffplayer();
#endif
void ags_functional_editor_workflow_test_edit_all();
void ags_functional_editor_workflow_test_fill_all();

#define AGS_FUNCTIONAL_EDITOR_WORKFLOW_TEST_CONFIG "[generic]\n" \
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

AgsGuiThread *gui_thread;
AgsTaskThread *task_thread;

void ags_functional_editor_workflow_test_add_test()
{
  /* add the tests to the suite */
  if((CU_add_test(pSuite, "functional test of GSequencer editor workflow AgsDrum\0", ags_functional_editor_workflow_test_drum) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer editor workflow AgsMatrix\0", ags_functional_editor_workflow_test_matrix) == NULL) ||
#ifdef AGS_WITH_LIBINSTPATCH
     (CU_add_test(pSuite, "functional test of GSequencer editor workflow AgsFFPlayer\0", ags_functional_editor_workflow_test_ffplayer) == NULL) ||
#endif
     (CU_add_test(pSuite, "functional test of GSequencer editor workflow edit all\0", ags_functional_editor_workflow_test_edit_all) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer editor workflow fill all\0", ags_functional_editor_workflow_test_fill_all) == NULL)){
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
ags_functional_editor_workflow_test_init_suite()
{
  /* get gui thread */
  gui_thread = ags_thread_find_type(ags_application_context->main_loop,
				    AGS_TYPE_GUI_THREAD);

  /* get task thread */
  task_thread = ags_thread_find_type(ags_application_context->main_loop,
				     AGS_TYPE_TASK_THREAD);

  /* window and editor size */
  ags_functional_test_util_file_default_window_resize();

  ags_functional_test_util_file_default_editor_resize();
 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_editor_workflow_test_clean_suite()
{  
  return(0);
}

void
ags_functional_editor_workflow_test_drum()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsEditor *editor;
  AgsMachine *machine;
  AgsMachineSelector *machine_selector;
  AgsMachineSelection *machine_selection;
  
  GList *list_start, *list;

  gchar *machine_str;
  
  guint nth_machine;
  gboolean success;

  xorg_application_context = ags_application_context;

  nth_machine = 0;

  /* add drum */
  success = ags_functional_test_util_add_machine(NULL,
						 "Drum");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  ags_test_enter();

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();

  if(list != NULL &&
     AGS_IS_DRUM(list->data)){
    machine = list->data;
  }else{
    machine = NULL;
  }
  
  CU_ASSERT(machine != NULL);

  /*
   *  add machine to editor
   */

  ags_test_enter();

  window = xorg_application_context->window;
  editor = window->editor;
  
  ags_test_leave();

  /* add index and link */
  success = ags_functional_test_util_machine_selection_add_index();

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selector_select(nth_machine);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selection_link_index();

  CU_ASSERT(success == TRUE);

  /* select machine */
  ags_test_enter();

  machine_selector = editor->machine_selector;
  machine_selection = machine_selector->machine_selection;

  machine_str = g_strdup_printf("%s: %s\0",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_test_leave();

  success = ags_functional_test_util_machine_selection_select(machine_str);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_dialog_ok(machine_selection);

  CU_ASSERT(success == TRUE);
}

void
ags_functional_editor_workflow_test_matrix()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsEditor *editor;
  AgsMachine *machine;
  AgsMachineSelector *machine_selector;
  AgsMachineSelection *machine_selection;
  
  GList *list_start, *list;

  gchar *machine_str;

  guint nth_machine;
  gboolean success;

  xorg_application_context = ags_application_context;

  nth_machine = 1;
  
  /* add matrix */
  success = ags_functional_test_util_add_machine(NULL,
						 "Matrix");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  ags_test_enter();

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();

  if(list != NULL &&
     AGS_IS_MATRIX(list->data)){
    machine = list->data;
  }else{
    machine = NULL;
  }
  
  CU_ASSERT(machine != NULL);

  /*
   *  add machine to editor
   */

  ags_test_enter();

  window = xorg_application_context->window;
  editor = window->editor;
  
  ags_test_leave();

  /* add index set link */
  success = ags_functional_test_util_machine_selection_add_index();

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selector_select(nth_machine);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selection_link_index();

  CU_ASSERT(success == TRUE);

  /* select machine */
  ags_test_enter();

  machine_selector = editor->machine_selector;
  machine_selection = machine_selector->machine_selection;

  machine_str = g_strdup_printf("%s: %s\0",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_test_leave();
  
  success = ags_functional_test_util_machine_selection_select(machine_str);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_dialog_ok(machine_selection);

  CU_ASSERT(success == TRUE);
}

#ifdef AGS_WITH_LIBINSTPATCH
void
ags_functional_editor_workflow_test_ffplayer()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsEditor *editor;
  AgsMachine *machine;
  AgsMachineSelector *machine_selector;
  AgsMachineSelection *machine_selection;
    
  GList *list_start, *list;

  gchar *machine_str;

  guint nth_machine;
  gboolean success;

  xorg_application_context = ags_application_context;

  nth_machine = 2;
  
  /* add fplayer */
  success = ags_functional_test_util_add_machine(NULL,
						 "FPlayer");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  ags_test_enter();

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();

  if(list != NULL &&
     AGS_IS_FFPLAYER(list->data)){
    machine = list->data;
  }else{
    machine = NULL;
  }
  
  CU_ASSERT(machine != NULL);

  /*
   *  add machine to editor
   */

  ags_test_enter();

  window = xorg_application_context->window;
  editor = window->editor;
  
  ags_test_leave();

  /* add index and link */
  success = ags_functional_test_util_machine_selection_add_index();

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selector_select(nth_machine);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selection_link_index();

  CU_ASSERT(success == TRUE);

  /* select machine */
  ags_test_enter();

  machine_selector = editor->machine_selector;
  machine_selection = machine_selector->machine_selection;

  machine_str = g_strdup_printf("%s: %s\0",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_test_leave();

  success = ags_functional_test_util_machine_selection_select(machine_str);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_dialog_ok(machine_selection);

  CU_ASSERT(success == TRUE);
}
#endif

void
ags_functional_editor_workflow_test_edit_all()
{
  guint nth_machine;
  guint i, j;
  gboolean success;

  /*
   * edit drum
   */

  nth_machine = 0;
  
  /* set zoom */
  success = ags_functional_test_util_toolbar_zoom(AGS_FUNCTIONAL_TEST_UTIL_TOOLBAR_ZOOM_1_TO_4);

  CU_ASSERT(success == TRUE);

  /* edit tool */
  success = ags_functional_test_util_toolbar_edit_click();

  CU_ASSERT(success == TRUE);

  /* select index */
  success = ags_functional_test_util_machine_selector_select(nth_machine);
  
  CU_ASSERT(success == TRUE);

  /* add drum kick pattern 2/4 */
  success = TRUE;
  
  for(i = 0; i < 64 && success;){
    success = ags_functional_test_util_pattern_edit_add_point(i, 0);

    i += 8;
  }

  for(i = 4; i < 64 && success;){
    success = ags_functional_test_util_pattern_edit_add_point(i, 1);

    i += 8;
  }

  CU_ASSERT(success == TRUE);

  /*
   * edit matrix
   */

  nth_machine = 1;

  /* select index */
  success = ags_functional_test_util_machine_selector_select(nth_machine);
  
  CU_ASSERT(success == TRUE);

  /* set zoom */
  success = ags_functional_test_util_toolbar_zoom(AGS_FUNCTIONAL_TEST_UTIL_TOOLBAR_ZOOM_1_TO_8);

  CU_ASSERT(success == TRUE);

  /* add matrix baseline pattern 8/8 */
  success = TRUE;
  
  for(i = 0; i < 32 && success;){
    for(j = 0; j < 56 && success;){
      success = ags_functional_test_util_pattern_edit_add_point(i * 8 + j, 15);

      j += 8;
    }

    if(!success){
      break;
    }
    
    success = ags_functional_test_util_pattern_edit_add_point(i * 8 + 56, 14);
    
    i += 8;
  }

  CU_ASSERT(success == TRUE);

#ifdef AGS_WITH_LIBINSTPATCH
  /*
   * edit ffplayer
   */

  nth_machine = 2;

  /* select index */
  success = ags_functional_test_util_machine_selector_select(nth_machine);
  
  CU_ASSERT(success == TRUE);

  /* set zoom */
  success = ags_functional_test_util_toolbar_zoom(AGS_FUNCTIONAL_TEST_UTIL_TOOLBAR_ZOOM_1_TO_4);

  CU_ASSERT(success == TRUE);

  /* add ffplayer note 4/4 */
  success = TRUE;
  
  for(i = 0; i < 64 && success;){
    success = ags_functional_test_util_note_edit_add_point(i, i + 1,
							   rand() % 10);
    
    i += 8;
  }

  CU_ASSERT(success == TRUE);
#endif
}

void
ags_functional_editor_workflow_test_fill_all()
{
  guint nth_machine;

}

int
main(int argc, char **argv)
{  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFuncitonalEditorWorkflowTest\0", ags_functional_editor_workflow_test_init_suite, ags_functional_editor_workflow_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  g_atomic_int_set(&is_available,
		   FALSE);
  
  ags_test_init(&argc, &argv,
		AGS_FUNCTIONAL_EDITOR_WORKFLOW_TEST_CONFIG);
  ags_functional_test_util_do_run(argc, argv,
				  ags_functional_editor_workflow_test_add_test, &is_available);

  return(-1);
}
