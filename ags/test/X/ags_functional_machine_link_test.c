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

#include <ags/test/X/libgsequencer.h>

#include "gsequencer_setup_util.h"
#include "ags_functional_test_util.h"

int ags_functional_machine_link_test_init_suite();
int ags_functional_machine_link_test_clean_suite();

void ags_functional_machine_link_test_master_mixer();
void ags_functional_machine_link_test_slave_mixer();
void ags_functional_machine_link_test_drum();
void ags_functional_machine_link_test_matrix();
void ags_functional_machine_link_test_synth();
void ags_functional_machine_link_test_ffplayer_0();
void ags_functional_machine_link_test_ffplayer_1();

void ags_functional_machine_link_test_relink_all();

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

extern AgsApplicationContext *ags_application_context;

extern struct sigaction ags_test_sigact;

extern volatile gboolean ags_show_start_animation;

AgsXorgApplicationContext *xorg_application_context;

AgsGuiThread *gui_thread;
AgsTaskThread *task_thread;

AgsMachine *panel;
AgsMachine *master_mixer;
AgsMachine *slave_mixer;
AgsMachine *drum;
AgsMachine *matrix;
AgsMachine *synth;
AgsMachine *ffplayer_0;
AgsMachine *ffplayer_1;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_machine_link_test_init_suite()
{
  AgsConfig *config;

  GList *list_start, *list;

  guint nth_machine;

  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_FUNCTIONAL_MACHINE_LINK_TEST_CONFIG,
			    strlen(AGS_FUNCTIONAL_MACHINE_LINK_TEST_CONFIG));

  ags_functional_test_util_setup_and_launch();

  /* get gui thread */
  gui_thread = ags_thread_find_type(ags_application_context->main_loop,
				    AGS_TYPE_GUI_THREAD);

  /* add panel */
  ags_functional_test_util_add_machine(NULL,
				       "Panel");

  /*  */
  gdk_threads_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve panel */
  nth_machine = 0;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  gdk_threads_leave();

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
  ags_thread_stop(gui_thread);  

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
  gdk_threads_enter();
  
  /* retrieve master mixer */
  nth_machine = 1;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  gdk_threads_leave();

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
  pthread_mutex_lock(task_thread->launch_mutex);

  properties = AGS_MACHINE(master_mixer)->properties;
  
  pthread_mutex_unlock(task_thread->launch_mutex);

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
  pthread_mutex_lock(task_thread->launch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(panel),
			      AGS_MACHINE(panel)->name);

  pthread_mutex_unlock(task_thread->launch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_machine,
								 0, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_machine,
								 0, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(task_thread->launch_mutex);

  properties = AGS_MACHINE(master_mixer)->properties;
  
  pthread_mutex_unlock(task_thread->launch_mutex);

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
  gdk_threads_enter();
  
  /* retrieve master mixer */
  nth_machine = 2;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  gdk_threads_leave();

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
  pthread_mutex_lock(task_thread->launch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(master_mixer),
			      AGS_MACHINE(master_mixer)->name);

  pthread_mutex_unlock(task_thread->launch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_machine,
								 0, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_machine,
								 0, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(task_thread->launch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(task_thread->launch_mutex);

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
  gdk_threads_enter();
  
  /* retrieve drum */
  nth_parent_machine = 2;
  nth_machine = 3;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  gdk_threads_leave();

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
  pthread_mutex_lock(task_thread->launch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(drum),
			      AGS_MACHINE(drum)->name);

  pthread_mutex_unlock(task_thread->launch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 0, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 0, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(task_thread->launch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(task_thread->launch_mutex);

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
  gdk_threads_enter();
  
  /* retrieve matrix */
  nth_parent_machine = 2;
  nth_machine = 4;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  gdk_threads_leave();

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
  pthread_mutex_lock(task_thread->launch_mutex);

  properties = AGS_MACHINE(matrix)->properties;
  
  pthread_mutex_unlock(task_thread->launch_mutex);

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
  pthread_mutex_lock(task_thread->launch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(matrix),
			      AGS_MACHINE(matrix)->name);

  pthread_mutex_unlock(task_thread->launch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 1, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 1, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(task_thread->launch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(task_thread->launch_mutex);

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
  gdk_threads_enter();
  
  /* retrieve synth */
  nth_parent_machine = 4;
  nth_machine = 5;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  gdk_threads_leave();

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
  pthread_mutex_lock(task_thread->launch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(synth),
			      AGS_MACHINE(synth)->name);

  input_line_count = matrix->audio->input_lines;  

  pthread_mutex_unlock(task_thread->launch_mutex);

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
  pthread_mutex_lock(task_thread->launch_mutex);

  properties = AGS_MACHINE(matrix)->properties;
  
  pthread_mutex_unlock(task_thread->launch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);

  /* hide */
  success = ags_functional_test_util_machine_hide(nth_machine);

  CU_ASSERT(success == TRUE);
}

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
  gdk_threads_enter();
  
  /* retrieve ffplayer #0 */
  nth_parent_machine = 2;
  nth_machine = 6;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  gdk_threads_leave();

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
  pthread_mutex_lock(task_thread->launch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(ffplayer_0),
			      AGS_MACHINE(ffplayer_0)->name);

  pthread_mutex_unlock(task_thread->launch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 2, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 2, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(task_thread->launch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(task_thread->launch_mutex);

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
  gdk_threads_enter();
  
  /* retrieve ffplayer #1 */
  nth_parent_machine = 2;
  nth_machine = 7;

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  gdk_threads_leave();

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
  pthread_mutex_lock(task_thread->launch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(ffplayer_1),
			      AGS_MACHINE(ffplayer_1)->name);

  pthread_mutex_unlock(task_thread->launch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 3, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 3, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(task_thread->launch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(task_thread->launch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);

  /* hide */
  success = ags_functional_test_util_machine_hide(nth_machine);

  CU_ASSERT(success == TRUE);
}

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
  pthread_mutex_lock(task_thread->launch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(ffplayer_0),
			      AGS_MACHINE(ffplayer_0)->name);

  pthread_mutex_unlock(task_thread->launch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 7, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 7, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(task_thread->launch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(task_thread->launch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);

  /* hide */
  success = ags_functional_test_util_machine_hide(nth_machine);

  CU_ASSERT(success == TRUE);

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
  pthread_mutex_lock(task_thread->launch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(drum),
			      AGS_MACHINE(drum)->name);

  pthread_mutex_unlock(task_thread->launch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 6, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 6, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(task_thread->launch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(task_thread->launch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);

  /* hide */
  success = ags_functional_test_util_machine_hide(nth_machine);

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
  pthread_mutex_lock(task_thread->launch_mutex);

  link_name = g_strdup_printf("%s: %s\0",
			      G_OBJECT_TYPE_NAME(matrix),
			      AGS_MACHINE(matrix)->name);

  pthread_mutex_unlock(task_thread->launch_mutex);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 0, 0,
								 link_name, 0);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_link_set(nth_parent_machine,
								 0, 1,
								 link_name, 1);

  CU_ASSERT(success == TRUE);

  /* response ok */
  pthread_mutex_lock(task_thread->launch_mutex);

  properties = AGS_MACHINE(slave_mixer)->properties;
  
  pthread_mutex_unlock(task_thread->launch_mutex);

  success = ags_functional_test_util_dialog_ok(properties);

  CU_ASSERT(success == TRUE);

  /* hide */
  success = ags_functional_test_util_machine_hide(nth_machine);

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

  /* destroy ffplayer #0 */
  success = ags_functional_test_util_machine_destroy(0);

  CU_ASSERT(success == TRUE);

  /* destroy ffplayer #1 */
  success = ags_functional_test_util_machine_destroy(0);

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

  gchar *rc_filename;
  
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
      
  /* parse rc file */
  rc_filename = g_strdup_printf("%s/%s\0",
				SRCDIR,
				"gsequencer.share/styles/ags.rc\0");
  
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
  pSuite = CU_add_suite("AgsFuncitonalMachineLinkTest\0", ags_functional_machine_link_test_init_suite, ags_functional_machine_link_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  gtk_init(NULL,
	   NULL);
  //  g_log_set_fatal_mask(G_LOG_DOMAIN, // , // "Gtk\0" G_LOG_DOMAIN,"GLib-GObject\0",
  //		       G_LOG_LEVEL_CRITICAL);

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "functional test of GSequencer machine link master mixer\0", ags_functional_machine_link_test_master_mixer) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine link slave mixer\0", ags_functional_machine_link_test_slave_mixer) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine link drum\0", ags_functional_machine_link_test_drum) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine link matrix\0", ags_functional_machine_link_test_matrix) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine link synth\0", ags_functional_machine_link_test_synth) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine link fplayer #0\0", ags_functional_machine_link_test_ffplayer_0) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine link fplayer #1\0", ags_functional_machine_link_test_ffplayer_1) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer machine relink all\0", ags_functional_machine_link_test_relink_all) == NULL) ||
    (CU_add_test(pSuite, "functional test of GSequencer machine link destroy all\0", ags_functional_machine_link_test_destroy_all) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
