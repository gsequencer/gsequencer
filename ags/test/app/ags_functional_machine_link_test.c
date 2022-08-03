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

void ags_functional_machine_link_test_add_test();

int ags_functional_machine_link_test_init_suite();
int ags_functional_machine_link_test_clean_suite();

void ags_functional_machine_link_test_master_mixer();
void ags_functional_machine_link_test_slave_mixer();
void ags_functional_machine_link_test_drum();
void ags_functional_machine_link_test_matrix();
void ags_functional_machine_link_test_synth();
#ifdef AGS_WITH_LIBINSTPATCH
void ags_functional_machine_link_test_ffplayer_0();
void ags_functional_machine_link_test_ffplayer_1();
#endif

void ags_functional_machine_link_test_relink_all();
void ags_functional_machine_link_test_reset_link_all();

void ags_functional_machine_link_test_destroy_all();

#define AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME (3.0 * G_USEC_PER_SEC)

#define AGS_FUNCTIONAL_MACHINE_LINK_TEST_MASTER_MIXER_INPUT_PADS (1)

#define AGS_FUNCTIONAL_MACHINE_LINK_TEST_MATRIX_OUTPUT_PADS (2)

#define AGS_FUNCTIONAL_MACHINE_LINK_TEST_CONFIG "[generic]\n"	\
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
  "device=default\n"						\
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
volatile gboolean is_terminated;

extern AgsApplicationContext *ags_application_context;

AgsGSequencerApplicationContext *gsequencer_application_context;

AgsMachine *panel;
AgsMachine *master_mixer;
AgsMachine *slave_mixer;
AgsMachine *drum;
AgsMachine *matrix;
AgsMachine *synth;

#ifdef AGS_WITH_LIBINSTPATCH
AgsMachine *ffplayer_0;
AgsMachine *ffplayer_1;
#endif

struct timespec ags_functional_machine_link_test_default_timeout = {
  300,
  0,
};

void
ags_functional_machine_link_test_add_test()
{
  /* add the tests to the suite */
  if((CU_add_test(pSuite, "functional test of GSequencer machine link master mixer", ags_functional_machine_link_test_master_mixer) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine link slave mixer", ags_functional_machine_link_test_slave_mixer) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine link drum", ags_functional_machine_link_test_drum) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine link matrix", ags_functional_machine_link_test_matrix) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine link synth", ags_functional_machine_link_test_synth) == NULL) ||
#ifdef AGS_WITH_LIBINSTPATCH
     (CU_add_test(pSuite, "functional test of GSequencer machine link fplayer #0", ags_functional_machine_link_test_ffplayer_0) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine link fplayer #1", ags_functional_machine_link_test_ffplayer_1) == NULL) ||
#endif
     (CU_add_test(pSuite, "functional test of GSequencer machine relink all", ags_functional_machine_link_test_relink_all) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine reset link all", ags_functional_machine_link_test_reset_link_all) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine link destroy all", ags_functional_machine_link_test_destroy_all) == NULL)){
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
ags_functional_machine_link_test_init_suite()
{
  gsequencer_application_context = ags_application_context;

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_realized),
						      &ags_functional_machine_link_test_default_timeout,
						      &(gsequencer_application_context->window));

  ags_functional_test_util_sync();

  /* window size */
  ags_functional_test_util_file_default_window_resize();

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_machine_link_test_clean_suite()
{
  return(0);
}

void
ags_functional_machine_link_test_master_mixer()
{
  AgsMachine *machine;
  GtkDialog *properties;
  GtkWidget **widget;
  
  AgsFunctionalTestUtilListLengthCondition condition;
  
  GList *start_list, *list;
  
  gchar *link_name;
  
  guint resize_tab;
  guint output_tab;
  guint nth_machine;
  gboolean success;

  /* add panel */
  ags_functional_test_util_add_machine(NULL,
				       "Panel");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 1;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_machine_link_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve panel */
  nth_machine = 0;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_PANEL);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_hide(nth_machine);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine =
    panel = g_list_nth_data(start_list,
			    nth_machine);

  /* add mixer */
  ags_functional_test_util_add_machine(NULL,
				       "Mixer");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 2;

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_machine_link_test_default_timeout,
						      &condition);

  ags_functional_test_util_sync();

  /* test master mixer */
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_MIXER);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /*
   * resize input pads
   */
  
  /* open properties */
  nth_machine = 1;

  ags_functional_test_util_machine_editor_dialog_open(nth_machine);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine =
    master_mixer = g_list_nth_data(start_list,
				   nth_machine);

  widget = &(machine->machine_editor_dialog);
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_machine_link_test_default_timeout,
						      widget);

  ags_functional_test_util_sync();
  
  widget = &(AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor);

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_machine_link_test_default_timeout,
						      widget);

  ags_functional_test_util_sync();

  widget = &(AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor->resize_editor);

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_machine_link_test_default_timeout,
						      widget);
  
  ags_functional_test_util_sync();

  /* click tab */
  resize_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_RESIZE_TAB;
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_machine,
							   resize_tab);  
    
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* click enable */
  ags_functional_test_util_machine_editor_dialog_click_enable(nth_machine);
    
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* resize input */
  ags_functional_test_util_machine_editor_dialog_resize_inputs(nth_machine,
							       AGS_FUNCTIONAL_MACHINE_LINK_TEST_MASTER_MIXER_INPUT_PADS);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response ok */
  properties = AGS_MACHINE(master_mixer)->machine_editor_dialog;  

  ags_functional_test_util_dialog_ok(properties);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*
   * link panel with master mixer
   */

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_machine);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* click tab */
  output_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_OUTPUT_TAB;
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_machine,
							   output_tab);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* click enable */
  ags_functional_test_util_machine_editor_dialog_click_enable(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* set link */
  link_name = g_strdup_printf("%s: %s",
			      G_OBJECT_TYPE_NAME(panel),
			      AGS_MACHINE(panel)->machine_name);

  ags_functional_test_util_machine_editor_dialog_link_set(nth_machine,
							  0, 0,
							  link_name, 0);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_link_set(nth_machine,
							  0, 1,
							  link_name, 1);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response ok */
  properties = AGS_MACHINE(master_mixer)->machine_editor_dialog;
  
  ags_functional_test_util_dialog_ok(properties);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* hide */
  ags_functional_test_util_machine_hide(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
}

void
ags_functional_machine_link_test_slave_mixer()
{
  AgsMachine *machine;
  GtkDialog *properties;
  
  AgsFunctionalTestUtilListLengthCondition condition;
  
  GList *start_list, *list;
  
  gchar *link_name;
  
  guint resize_tab;
  guint output_tab;
  guint nth_machine;
  gboolean success;

  /* add mixer */
  ags_functional_test_util_add_machine(NULL,
				       "Mixer");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 3;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_machine_link_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
  
  /* test slave mixer */
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_MIXER);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  nth_machine = 2;

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine =
    slave_mixer = g_list_nth_data(start_list,
				  nth_machine);

  /*
   * link master mixer with slave mixer
   */

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_machine);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* click tab */
  output_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_OUTPUT_TAB;
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_machine,
							   output_tab);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* click enable */
  ags_functional_test_util_machine_editor_dialog_click_enable(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* set link */
  link_name = g_strdup_printf("%s: %s",
			      G_OBJECT_TYPE_NAME(master_mixer),
			      AGS_MACHINE(master_mixer)->machine_name);

  ags_functional_test_util_machine_editor_dialog_link_set(nth_machine,
								    0, 0,
								    link_name, 0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_link_set(nth_machine,
							  0, 1,
							  link_name, 1);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response ok */
  properties = AGS_MACHINE(slave_mixer)->machine_editor_dialog;  

  ags_functional_test_util_dialog_ok(properties);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* hide */
  ags_functional_test_util_machine_hide(nth_machine);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
}

void
ags_functional_machine_link_test_drum()
{
  GtkDialog *properties;
  
  AgsFunctionalTestUtilListLengthCondition condition;
  
  GList *start_list, *list;
  
  gchar *link_name;
  
  guint resize_tab;
  guint input_tab;
  guint nth_parent_machine;
  guint nth_machine;
  gboolean success;

  /* add drum */
  ags_functional_test_util_add_machine(NULL,
				       "Drum");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 4;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_machine_link_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
  
  /* test drum */
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_DRUM);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  nth_parent_machine = 2;
  nth_machine = 3;

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  drum = g_list_nth_data(start_list,
			 nth_machine);

  /*
   * link slave mixer with drum
   */

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB;
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_parent_machine,
							   input_tab);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* click enable */
  ags_functional_test_util_machine_editor_dialog_click_enable(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* set link */
  link_name = g_strdup_printf("%s: %s",
			      G_OBJECT_TYPE_NAME(drum),
			      AGS_MACHINE(drum)->machine_name);

  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
							  0, 0,
							  link_name, 0);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
							  0, 1,
							  link_name, 1);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response ok */
  properties = AGS_MACHINE(slave_mixer)->machine_editor_dialog;  

  ags_functional_test_util_dialog_ok(properties);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* hide */
  ags_functional_test_util_machine_hide(nth_machine);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
}

void
ags_functional_machine_link_test_matrix()
{
  GtkDialog *properties;
  
  AgsFunctionalTestUtilListLengthCondition condition;
  
  GList *start_list, *list;
  
  gchar *link_name;
  
  guint resize_tab;
  guint input_tab;
  guint nth_parent_machine;
  guint nth_machine;
  gboolean success;

  /* add matrix */
  ags_functional_test_util_add_machine(NULL,
				       "Matrix");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 5;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_machine_link_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
  
  /* test matrix */
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_MATRIX);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  nth_parent_machine = 2;
  nth_machine = 4;

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  matrix = g_list_nth_data(start_list,
			 nth_machine);

  /*
   * resize output pads
   */
  
  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* click tab */
  resize_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_RESIZE_TAB;
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_machine,
							   resize_tab);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* click enable */
  ags_functional_test_util_machine_editor_dialog_click_enable(nth_machine);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* resize output */
  ags_functional_test_util_machine_editor_dialog_resize_outputs(nth_machine,
								AGS_FUNCTIONAL_MACHINE_LINK_TEST_MATRIX_OUTPUT_PADS);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response ok */
  properties = AGS_MACHINE(matrix)->machine_editor_dialog;  

  ags_functional_test_util_dialog_ok(properties);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*
   * link slave mixer with matrix
   */

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB;
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_parent_machine,
								     input_tab);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* click enable */
  ags_functional_test_util_machine_editor_dialog_click_enable(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* set link */
  link_name = g_strdup_printf("%s: %s",
			      G_OBJECT_TYPE_NAME(matrix),
			      AGS_MACHINE(matrix)->machine_name);

  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
								    1, 0,
								    link_name, 0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
								    1, 1,
								    link_name, 1);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response ok */
  properties = AGS_MACHINE(slave_mixer)->machine_editor_dialog;  

  ags_functional_test_util_dialog_ok(properties);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* hide */
  ags_functional_test_util_machine_hide(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
}

void
ags_functional_machine_link_test_synth()
{
  GtkDialog *properties;
  
  AgsFunctionalTestUtilListLengthCondition condition;
  
  GList *start_list, *list;
  
  gchar *link_name;

  guint input_line_count;
  guint input_link_tab;
  guint nth_parent_machine;
  guint nth_machine;
  gboolean success;

  /* add synth */
  ags_functional_test_util_add_machine(NULL,
				       "Synth");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 6;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_machine_link_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* test synth */
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_SYNTH);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  nth_parent_machine = 4;
  nth_machine = 5;

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  synth = g_list_nth_data(start_list,
			  nth_machine);

  /*
   * link matrix with synth
   */

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* click tab */
  input_link_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_INPUT_TAB;
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_parent_machine,
								     input_link_tab);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* click enable */
  ags_functional_test_util_machine_editor_dialog_click_enable(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* set link */
  link_name = g_strdup_printf("%s: %s",
			      G_OBJECT_TYPE_NAME(synth),
			      AGS_MACHINE(synth)->machine_name);

  input_line_count = matrix->audio->input_lines;  

  ags_functional_test_util_machine_editor_dialog_bulk_add(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_bulk_link(nth_parent_machine,
							   0,
							   link_name);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_bulk_count(nth_parent_machine,
							    0,
							    input_line_count);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response ok */
  properties = AGS_MACHINE(matrix)->machine_editor_dialog;  

  ags_functional_test_util_dialog_ok(properties);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* hide */
  ags_functional_test_util_machine_hide(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
}

#ifdef AGS_WITH_LIBINSTPATCH
void
ags_functional_machine_link_test_ffplayer_0()
{
  GtkDialog *properties;
  
  AgsFunctionalTestUtilListLengthCondition condition;
  
  GList *start_list, *list;
  
  gchar *link_name;
  
  guint resize_tab;
  guint input_tab;
  guint nth_parent_machine;
  guint nth_machine;
  gboolean success;

  /* add ffplayer #0 */
  ags_functional_test_util_add_machine(NULL,
				       "FPlayer");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 7;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_machine_link_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* test ffplayer */
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_FFPLAYER);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  nth_parent_machine = 2;
  nth_machine = 6;

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  ffplayer_0 = g_list_nth_data(start_list,
			       nth_machine);

  /*
   * link slave mixer with ffplayer #0
   */

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB;
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_parent_machine,
							   input_tab);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* click enable */
  ags_functional_test_util_machine_editor_dialog_click_enable(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* set link */
  link_name = g_strdup_printf("%s: %s",
			      G_OBJECT_TYPE_NAME(ffplayer_0),
			      AGS_MACHINE(ffplayer_0)->machine_name);


  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
							  2, 0,
							  link_name, 0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
								    2, 1,
								    link_name, 1);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response ok */
  properties = AGS_MACHINE(slave_mixer)->machine_editor_dialog;  

  ags_functional_test_util_dialog_ok(properties);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* hide */
  ags_functional_test_util_machine_hide(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
}

void
ags_functional_machine_link_test_ffplayer_1()
{
  GtkDialog *properties;
  
  AgsFunctionalTestUtilListLengthCondition condition;
  
  GList *start_list, *list;
  
  gchar *link_name;
  
  guint resize_tab;
  guint input_tab;
  guint nth_parent_machine;
  guint nth_machine;
  gboolean success;

  /* add ffplayer #1 */
  ags_functional_test_util_add_machine(NULL,
				       "FPlayer");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 8;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_machine_link_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
  
  /* test ffplayer */
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_FFPLAYER);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  nth_parent_machine = 2;
  nth_machine = 7;

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  ffplayer_1 = g_list_nth_data(start_list,
			       nth_machine);

  /*
   * link slave mixer with ffplayer #1
   */

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB;
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_parent_machine,
								     input_tab);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* click enable */
  ags_functional_test_util_machine_editor_dialog_click_enable(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* set link */
  link_name = g_strdup_printf("%s: %s",
			      G_OBJECT_TYPE_NAME(ffplayer_1),
			      AGS_MACHINE(ffplayer_1)->machine_name);

  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
								    3, 0,
								    link_name, 0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
								    3, 1,
								    link_name, 1);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* response ok */
  properties = AGS_MACHINE(slave_mixer)->machine_editor_dialog;  

  ags_functional_test_util_dialog_ok(properties);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* hide */
  ags_functional_test_util_machine_hide(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
}
#endif

void
ags_functional_machine_link_test_relink_all()
{
  GtkDialog *properties;
  
  GList *start_list, *list;
  
  gchar *link_name;
  
  guint resize_tab;
  guint input_tab;
  guint nth_parent_machine;
  guint nth_machine;
  gboolean success;  

#ifdef AGS_WITH_LIBINSTPATCH
  /*
   * relink slave mixer with ffplayer #0
   */

  /*  */
  nth_parent_machine = 2;
  nth_machine = 6;

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB;
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_parent_machine,
							   input_tab);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* click enable */
  ags_functional_test_util_machine_editor_dialog_click_enable(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* set link */
  link_name = g_strdup_printf("%s: %s",
			      G_OBJECT_TYPE_NAME(ffplayer_0),
			      AGS_MACHINE(ffplayer_0)->machine_name);

  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
							  7, 0,
							  link_name, 0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
							  7, 1,
							  link_name, 1);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* response ok */
  properties = AGS_MACHINE(slave_mixer)->machine_editor_dialog;  

  ags_functional_test_util_dialog_ok(properties);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
#endif
  
  /*
   * relink slave mixer with drum
   */

  /*  */
  nth_parent_machine = 2;
  nth_machine = 3;

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB;
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_parent_machine,
							   input_tab);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* click enable */
  ags_functional_test_util_machine_editor_dialog_click_enable(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* set link */
  link_name = g_strdup_printf("%s: %s",
			      G_OBJECT_TYPE_NAME(drum),
			      AGS_MACHINE(drum)->machine_name);


  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
							  6, 0,
							  link_name, 0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
							  6, 1,
							  link_name, 1);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
  
  /* response ok */
  properties = AGS_MACHINE(slave_mixer)->machine_editor_dialog;
  
  ags_functional_test_util_dialog_ok(properties);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /*
   * relink slave mixer with matrix
   */
  
  /*  */
  nth_parent_machine = 2;
  nth_machine = 4;

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB;
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_parent_machine,
							   input_tab);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* click enable */
  ags_functional_test_util_machine_editor_dialog_click_enable(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* set link */
  link_name = g_strdup_printf("%s: %s",
			      G_OBJECT_TYPE_NAME(matrix),
			      AGS_MACHINE(matrix)->machine_name);


  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
							  0, 0,
							  link_name, 0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
							  0, 1,
							  link_name, 1);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* response ok */
  properties = AGS_MACHINE(slave_mixer)->machine_editor_dialog;  

  ags_functional_test_util_dialog_ok(properties);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
}

void
ags_functional_machine_link_test_reset_link_all()
{
  GtkDialog *properties;
  
  GList *start_list, *list;
  
  gchar *link_name;
  
  guint resize_tab;
  guint input_tab;
  guint nth_parent_machine;
  guint nth_machine;
  gboolean success;  

#ifdef AGS_WITH_LIBINSTPATCH
  /*
   * reset link slave mixer with ffplayer #0
   */

  /*  */
  nth_parent_machine = 2;
  nth_machine = 6;

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB;
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_parent_machine,
							   input_tab);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* click enable */
  ags_functional_test_util_machine_editor_dialog_click_enable(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* set link */
  link_name = g_strdup_printf("%s: %s",
			      G_OBJECT_TYPE_NAME(ffplayer_0),
			      AGS_MACHINE(ffplayer_0)->machine_name);

  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
							  0, 0,
							  link_name, 0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
							  0, 1,
							  link_name, 1);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* response ok */
  properties = AGS_MACHINE(slave_mixer)->machine_editor_dialog;  

  ags_functional_test_util_dialog_ok(properties);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
#endif
  
  /*
   * reset link slave mixer with drum
   */

  /*  */
  nth_parent_machine = 2;
  nth_machine = 3;

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB;
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_parent_machine,
							   input_tab);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* click enable */
  ags_functional_test_util_machine_editor_dialog_click_enable(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* set link */
  link_name = g_strdup_printf("%s: %s",
			      G_OBJECT_TYPE_NAME(drum),
			      AGS_MACHINE(drum)->machine_name);

  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
							  1, 0,
							  link_name, 0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
							  1, 1,
							  link_name, 1);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* response ok */
  properties = AGS_MACHINE(slave_mixer)->machine_editor_dialog;  

  ags_functional_test_util_dialog_ok(properties);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /*
   * reset link slave mixer with matrix
   */
  
  /*  */
  nth_parent_machine = 2;
  nth_machine = 4;

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB;
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_parent_machine,
							   input_tab);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* click enable */
  ags_functional_test_util_machine_editor_dialog_click_enable(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
  
  /* set link */
  link_name = g_strdup_printf("%s: %s",
			      G_OBJECT_TYPE_NAME(matrix),
			      AGS_MACHINE(matrix)->machine_name);
  
  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
							  1, 0,
							  link_name, 0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
							  1, 1,
							  link_name, 1);
 
  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* response ok */
  properties = AGS_MACHINE(slave_mixer)->machine_editor_dialog;
  
  ags_functional_test_util_dialog_ok(properties);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /*
   * reset link slave mixer with drum
   */

  /*  */
  nth_parent_machine = 2;
  nth_machine = 3;

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB;
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_parent_machine,
							   input_tab);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* click enable */
  ags_functional_test_util_machine_editor_dialog_click_enable(nth_parent_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* set link */
  link_name = g_strdup_printf("%s: %s",
			      G_OBJECT_TYPE_NAME(drum),
			      AGS_MACHINE(drum)->machine_name);

  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
							  2, 0,
							  link_name, 0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_link_set(nth_parent_machine,
							  2, 1,
							  link_name, 1);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* response ok */
  properties = AGS_MACHINE(slave_mixer)->machine_editor_dialog;  

  ags_functional_test_util_dialog_ok(properties);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
}

void
ags_functional_machine_link_test_destroy_all()
{
  gboolean success;

  /* destroy master mixer */
  ags_functional_test_util_machine_destroy(1);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* destroy master panel */
  ags_functional_test_util_machine_destroy(0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* destroy matrix */
  ags_functional_test_util_machine_destroy(2);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
  
  /* destroy drum */
  ags_functional_test_util_machine_destroy(1);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
  
  /* destroy slave mixer */
  ags_functional_test_util_machine_destroy(0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* destroy synth */
  ags_functional_test_util_machine_destroy(0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
  
#ifdef AGS_WITH_LIBINSTPATCH
  /* destroy ffplayer #0 */
  ags_functional_test_util_machine_destroy(0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
  
  /* destroy ffplayer #1 */
  ags_functional_test_util_machine_destroy(0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_MACHINE_LINK_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();
#endif
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
  pSuite = CU_add_suite("AgsFunctionalMachineLinkTest", ags_functional_machine_link_test_init_suite, ags_functional_machine_link_test_clean_suite);
  
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
				  AGS_FUNCTIONAL_MACHINE_LINK_TEST_CONFIG);
  }
#endif
  
  ags_functional_test_util_do_run(argc, argv,
				  ags_functional_machine_link_test_add_test, &is_available);

  g_atomic_int_set(&is_terminated,
		   TRUE);

  g_thread_join(ags_functional_test_util_test_runner_thread());
  
  return(-1);
}
