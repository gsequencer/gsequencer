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

AgsXorgApplicationContext *xorg_application_context;

AgsGuiThread *gui_thread;
AgsTaskThread *task_thread;

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

void
ags_functional_machine_link_test_add_test()
{
  /* add the tests to the suite */
  if((CU_add_test(pSuite, "functional test of GSequencer machine link master mixer\0", ags_functional_machine_link_test_master_mixer) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine link slave mixer\0", ags_functional_machine_link_test_slave_mixer) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine link drum\0", ags_functional_machine_link_test_drum) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine link matrix\0", ags_functional_machine_link_test_matrix) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine link synth\0", ags_functional_machine_link_test_synth) == NULL) ||
#ifdef AGS_WITH_LIBINSTPATCH
     (CU_add_test(pSuite, "functional test of GSequencer machine link fplayer #0\0", ags_functional_machine_link_test_ffplayer_0) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine link fplayer #1\0", ags_functional_machine_link_test_ffplayer_1) == NULL) ||
#endif
     (CU_add_test(pSuite, "functional test of GSequencer machine relink all\0", ags_functional_machine_link_test_relink_all) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine reset link all\0", ags_functional_machine_link_test_reset_link_all) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine link destroy all\0", ags_functional_machine_link_test_destroy_all) == NULL)){
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
ags_functional_machine_link_test_init_suite()
{
  GList *list_start, *list;

  guint nth_machine;

  /* get gui thread */
  gui_thread = ags_thread_find_type(ags_application_context->main_loop,
				    AGS_TYPE_GUI_THREAD);

  /* add panel */
  ags_functional_test_util_add_machine(NULL,
				       "Panel");

  /*  */
  pthread_mutex_lock(gui_thread->dispatch_mutex);
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve panel */
  nth_machine = 0;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  if(list != NULL &&
     AGS_IS_PANEL(list->data)){
    panel = list->data;
  }else{
    panel = NULL;
  }
  
  ags_functional_test_util_machine_hide(nth_machine);

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
  GtkDialog *properties;
  
  GList *list_start, *list;
  
  gchar *link_name;
  
  guint resize_tab;
  guint output_tab;
  guint nth_machine;
  gboolean success;

  /* add mixer */
  success = ags_functional_test_util_add_machine(NULL,
						 "Mixer");

  CU_ASSERT(success == TRUE);

  /*  */
  pthread_mutex_lock(gui_thread->dispatch_mutex);
  
  /* retrieve master mixer */
  nth_machine = 1;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  if(list != NULL &&
     AGS_IS_MIXER(list->data)){
    master_mixer = list->data;
  }else{
    master_mixer = NULL;
  }
  
  CU_ASSERT(master_mixer != NULL);

  /*
   * resize input pads
   */
  
  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_machine);

  CU_ASSERT(success == TRUE);

  /* click tab */
  resize_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_RESIZE_TAB;
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_machine,
								  resize_tab);

  CU_ASSERT(success == TRUE);
  
  /* click enable */
  success = ags_functional_test_util_machine_properties_click_enable(nth_machine);

  CU_ASSERT(success == TRUE);

  /* resize input */
  success = ags_functional_test_util_machine_properties_resize_inputs(nth_machine,
								      AGS_FUNCTIONAL_MACHINE_LINK_TEST_MASTER_MIXER_INPUT_PADS);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  properties = AGS_MACHINE(master_mixer)->properties;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);

  /*
   * link panel with master mixer
   */

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_machine);

  CU_ASSERT(success == TRUE);

  /* click tab */
  output_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_OUTPUT_TAB;
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_machine,
								  output_tab);

  CU_ASSERT(success == TRUE);

  /* click enable */
  success = ags_functional_test_util_machine_properties_click_enable(nth_machine);

  CU_ASSERT(success == TRUE);

  /* set link */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(panel),
			      AGS_MACHINE(panel)->machine_name);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_machine,
								 0, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_machine,
								 0, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  properties = AGS_MACHINE(master_mixer)->properties;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);

  /* hide */
  success = ags_functional_test_util_machine_hide(nth_machine);

  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_link_test_slave_mixer()
{
  GtkDialog *properties;
  
  GList *list_start, *list;
  
  gchar *link_name;
  
  guint resize_tab;
  guint output_tab;
  guint nth_machine;
  gboolean success;

  /* add mixer */
  success = ags_functional_test_util_add_machine(NULL,
						 "Mixer");

  CU_ASSERT(success == TRUE);

  /*  */
  pthread_mutex_lock(gui_thread->dispatch_mutex);
  
  /* retrieve master mixer */
  nth_machine = 2;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  if(list != NULL &&
     AGS_IS_MIXER(list->data)){
    slave_mixer = list->data;
  }else{
    slave_mixer = NULL;
  }
  
  CU_ASSERT(slave_mixer != NULL);

  /*
   * link master mixer with slave mixer
   */

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_machine);

  CU_ASSERT(success == TRUE);

  /* click tab */
  output_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_OUTPUT_TAB;
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_machine,
								  output_tab);

  CU_ASSERT(success == TRUE);

  /* click enable */
  success = ags_functional_test_util_machine_properties_click_enable(nth_machine);

  CU_ASSERT(success == TRUE);

  /* set link */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(master_mixer),
			      AGS_MACHINE(master_mixer)->machine_name);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_machine,
								 0, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_machine,
								 0, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);

  /* hide */
  success = ags_functional_test_util_machine_hide(nth_machine);

  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_link_test_drum()
{
  GtkDialog *properties;
  
  GList *list_start, *list;
  
  gchar *link_name;
  
  guint resize_tab;
  guint input_tab;
  guint nth_parent_machine;
  guint nth_machine;
  gboolean success;

  /* add drum */
  success = ags_functional_test_util_add_machine(NULL,
						 "Drum");

  CU_ASSERT(success == TRUE);

  /*  */
  pthread_mutex_lock(gui_thread->dispatch_mutex);
  
  /* retrieve drum */
  nth_parent_machine = 2;
  nth_machine = 3;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  if(list != NULL &&
     AGS_IS_DRUM(list->data)){
    drum = list->data;
  }else{
    drum = NULL;
  }
  
  CU_ASSERT(drum != NULL);

  /*
   * link slave mixer with drum
   */

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB;
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_parent_machine,
								  input_tab);

  CU_ASSERT(success == TRUE);

  /* click enable */
  success = ags_functional_test_util_machine_properties_click_enable(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* set link */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(drum),
			      AGS_MACHINE(drum)->machine_name);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 0, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 0, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);

  /* hide */
  success = ags_functional_test_util_machine_hide(nth_machine);

  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_link_test_matrix()
{
  GtkDialog *properties;
  
  GList *list_start, *list;
  
  gchar *link_name;
  
  guint resize_tab;
  guint input_tab;
  guint nth_parent_machine;
  guint nth_machine;
  gboolean success;

  /* add matrix */
  success = ags_functional_test_util_add_machine(NULL,
						 "Matrix");

  CU_ASSERT(success == TRUE);

  /*  */
  pthread_mutex_lock(gui_thread->dispatch_mutex);
  
  /* retrieve matrix */
  nth_parent_machine = 2;
  nth_machine = 4;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  if(list != NULL &&
     AGS_IS_MATRIX(list->data)){
    matrix = list->data;
  }else{
    matrix = NULL;
  }
  
  CU_ASSERT(matrix != NULL);

  /*
   * resize output pads
   */
  
  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_machine);

  CU_ASSERT(success == TRUE);

  /* click tab */
  resize_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_RESIZE_TAB;
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_machine,
								  resize_tab);

  CU_ASSERT(success == TRUE);
  
  /* click enable */
  success = ags_functional_test_util_machine_properties_click_enable(nth_machine);

  CU_ASSERT(success == TRUE);

  /* resize output */
  success = ags_functional_test_util_machine_properties_resize_outputs(nth_machine,
								       AGS_FUNCTIONAL_MACHINE_LINK_TEST_MATRIX_OUTPUT_PADS);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  properties = AGS_MACHINE(matrix)->properties;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);
  
  /*
   * link slave mixer with matrix
   */

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB;
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_parent_machine,
								  input_tab);

  CU_ASSERT(success == TRUE);

  /* click enable */
  success = ags_functional_test_util_machine_properties_click_enable(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* set link */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(matrix),
			      AGS_MACHINE(matrix)->machine_name);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 1, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 1, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);

  /* hide */
  success = ags_functional_test_util_machine_hide(nth_machine);

  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_link_test_synth()
{
  GtkDialog *properties;
  
  GList *list_start, *list;
  
  gchar *link_name;

  guint input_line_count;
  guint input_link_tab;
  guint nth_parent_machine;
  guint nth_machine;
  gboolean success;

  /* add synth */
  success = ags_functional_test_util_add_machine(NULL,
						 "Synth");

  CU_ASSERT(success == TRUE);

  /*  */
  pthread_mutex_lock(gui_thread->dispatch_mutex);
  
  /* retrieve synth */
  nth_parent_machine = 4;
  nth_machine = 5;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  if(list != NULL &&
     AGS_IS_SYNTH(list->data)){
    synth = list->data;
  }else{
    synth = NULL;
  }
  
  CU_ASSERT(synth != NULL);

  /*
   * link matrix with synth
   */

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* click tab */
  input_link_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_BULK_INPUT_TAB;
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_parent_machine,
								  input_link_tab);

  CU_ASSERT(success == TRUE);

  /* click enable */
  success = ags_functional_test_util_machine_properties_click_enable(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* set link */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(synth),
			      AGS_MACHINE(synth)->machine_name);

  input_line_count = matrix->audio->input_lines;  

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_machine_properties_bulk_add(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_bulk_link(nth_parent_machine,
								  0,
								  link_name);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_bulk_count(nth_parent_machine,
								   0,
								   input_line_count);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  properties = AGS_MACHINE(matrix)->properties;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);

  /* hide */
  success = ags_functional_test_util_machine_hide(nth_machine);

  CU_ASSERT(success == TRUE);
}

#ifdef AGS_WITH_LIBINSTPATCH
void
ags_functional_machine_link_test_ffplayer_0()
{
  GtkDialog *properties;
  
  GList *list_start, *list;
  
  gchar *link_name;
  
  guint resize_tab;
  guint input_tab;
  guint nth_parent_machine;
  guint nth_machine;
  gboolean success;

  /* add ffplayer #0 */
  success = ags_functional_test_util_add_machine(NULL,
						 "FPlayer");

  CU_ASSERT(success == TRUE);

  /*  */
  pthread_mutex_lock(gui_thread->dispatch_mutex);
  
  /* retrieve ffplayer #0 */
  nth_parent_machine = 2;
  nth_machine = 6;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  if(list != NULL &&
     AGS_IS_FFPLAYER(list->data)){
    ffplayer_0 = list->data;
  }else{
    ffplayer_0 = NULL;
  }
  
  CU_ASSERT(ffplayer_0 != NULL);

  /*
   * link slave mixer with ffplayer #0
   */

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB;
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_parent_machine,
								  input_tab);

  CU_ASSERT(success == TRUE);

  /* click enable */
  success = ags_functional_test_util_machine_properties_click_enable(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* set link */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(ffplayer_0),
			      AGS_MACHINE(ffplayer_0)->machine_name);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 2, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 2, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);

  /* hide */
  success = ags_functional_test_util_machine_hide(nth_machine);

  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_link_test_ffplayer_1()
{
  GtkDialog *properties;
  
  GList *list_start, *list;
  
  gchar *link_name;
  
  guint resize_tab;
  guint input_tab;
  guint nth_parent_machine;
  guint nth_machine;
  gboolean success;

  /* add ffplayer #1 */
  success = ags_functional_test_util_add_machine(NULL,
						 "FPlayer");

  CU_ASSERT(success == TRUE);

  /*  */
  pthread_mutex_lock(gui_thread->dispatch_mutex);
  
  /* retrieve ffplayer #1 */
  nth_parent_machine = 2;
  nth_machine = 7;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  if(list != NULL &&
     AGS_IS_FFPLAYER(list->data)){
    ffplayer_1 = list->data;
  }else{
    ffplayer_1 = NULL;
  }
  
  CU_ASSERT(ffplayer_1 != NULL);

  /*
   * link slave mixer with ffplayer #1
   */

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB;
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_parent_machine,
								  input_tab);

  CU_ASSERT(success == TRUE);

  /* click enable */
  success = ags_functional_test_util_machine_properties_click_enable(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* set link */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(ffplayer_1),
			      AGS_MACHINE(ffplayer_1)->machine_name);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 3, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 3, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);

  /* hide */
  success = ags_functional_test_util_machine_hide(nth_machine);

  CU_ASSERT(success == TRUE);
}
#endif

void
ags_functional_machine_link_test_relink_all()
{
  GtkDialog *properties;
  
  GList *list_start, *list;
  
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
  success = ags_functional_test_util_machine_properties_open(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB;
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_parent_machine,
								  input_tab);

  CU_ASSERT(success == TRUE);

  /* click enable */
  success = ags_functional_test_util_machine_properties_click_enable(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* set link */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(ffplayer_0),
			      AGS_MACHINE(ffplayer_0)->machine_name);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 7, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 7, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);
#endif
  
  /*
   * relink slave mixer with drum
   */

  /*  */
  nth_parent_machine = 2;
  nth_machine = 3;

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB;
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_parent_machine,
								  input_tab);

  CU_ASSERT(success == TRUE);

  /* click enable */
  success = ags_functional_test_util_machine_properties_click_enable(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* set link */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(drum),
			      AGS_MACHINE(drum)->machine_name);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 6, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 6, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);

  /*
   * relink slave mixer with matrix
   */
  
  /*  */
  nth_parent_machine = 2;
  nth_machine = 4;

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB;
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_parent_machine,
								  input_tab);

  CU_ASSERT(success == TRUE);

  /* click enable */
  success = ags_functional_test_util_machine_properties_click_enable(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* set link */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(matrix),
			      AGS_MACHINE(matrix)->machine_name);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 0, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 0, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_link_test_reset_link_all()
{
  GtkDialog *properties;
  
  GList *list_start, *list;
  
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
  success = ags_functional_test_util_machine_properties_open(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB;
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_parent_machine,
								  input_tab);

  CU_ASSERT(success == TRUE);

  /* click enable */
  success = ags_functional_test_util_machine_properties_click_enable(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* set link */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(ffplayer_0),
			      AGS_MACHINE(ffplayer_0)->machine_name);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 0, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 0, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);
#endif
  
  /*
   * reset link slave mixer with drum
   */

  /*  */
  nth_parent_machine = 2;
  nth_machine = 3;

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB;
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_parent_machine,
								  input_tab);

  CU_ASSERT(success == TRUE);

  /* click enable */
  success = ags_functional_test_util_machine_properties_click_enable(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* set link */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(drum),
			      AGS_MACHINE(drum)->machine_name);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 1, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 1, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);

  /*
   * reset link slave mixer with matrix
   */
  
  /*  */
  nth_parent_machine = 2;
  nth_machine = 4;

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB;
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_parent_machine,
								  input_tab);

  CU_ASSERT(success == TRUE);

  /* click enable */
  success = ags_functional_test_util_machine_properties_click_enable(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* set link */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(matrix),
			      AGS_MACHINE(matrix)->machine_name);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 1, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 1, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);

  /*
   * reset link slave mixer with drum
   */

  /*  */
  nth_parent_machine = 2;
  nth_machine = 3;

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* click tab */
  input_tab = AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB;
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_parent_machine,
								  input_tab);

  CU_ASSERT(success == TRUE);

  /* click enable */
  success = ags_functional_test_util_machine_properties_click_enable(nth_parent_machine);

  CU_ASSERT(success == TRUE);

  /* set link */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(drum),
			      AGS_MACHINE(drum)->machine_name);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 2, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 2, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);
}

void
ags_functional_machine_link_test_destroy_all()
{
  gboolean success;

  /* destroy master mixer */
  success = ags_functional_test_util_machine_destroy(1);

  CU_ASSERT(success == TRUE);

  /* destroy master panel */
  success = ags_functional_test_util_machine_destroy(0);

  CU_ASSERT(success == TRUE);

  /* destroy matrix */
  success = ags_functional_test_util_machine_destroy(2);

  CU_ASSERT(success == TRUE);

  /* destroy drum */
  success = ags_functional_test_util_machine_destroy(1);

  CU_ASSERT(success == TRUE);

  /* destroy slave mixer */
  success = ags_functional_test_util_machine_destroy(0);

  CU_ASSERT(success == TRUE);

  /* destroy synth */
  success = ags_functional_test_util_machine_destroy(0);

  CU_ASSERT(success == TRUE);

#ifdef AGS_WITH_LIBINSTPATCH
  /* destroy ffplayer #0 */
  success = ags_functional_test_util_machine_destroy(0);

  CU_ASSERT(success == TRUE);

  /* destroy ffplayer #1 */
  success = ags_functional_test_util_machine_destroy(0);

  CU_ASSERT(success == TRUE);
#endif
}

int
main(int argc, char **argv)
{
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFuncitonalMachineLinkTest\0", ags_functional_machine_link_test_init_suite, ags_functional_machine_link_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  g_atomic_int_set(&is_available,
		   FALSE);
  
  ags_test_init(&argc, &argv,
		AGS_FUNCTIONAL_MACHINE_LINK_TEST_CONFIG);
  ags_functional_test_util_do_run(argc, argv,
				  ags_functional_machine_link_test_add_test, &is_available);

  return(-1);
}
