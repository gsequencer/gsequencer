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

void ags_functional_line_member_add_and_destroy_test_add_test();

int ags_functional_line_member_add_and_destroy_test_init_suite();
int ags_functional_line_member_add_and_destroy_test_clean_suite();

void ags_functional_line_member_add_and_destroy_test_panel();
void ags_functional_line_member_add_and_destroy_test_mixer();
void ags_functional_line_member_add_and_destroy_test_drum();
void ags_functional_line_member_add_and_destroy_test_matrix();
void ags_functional_line_member_add_and_destroy_test_synth();
#ifdef AGS_WITH_LIBINSTPATCH
void ags_functional_line_member_add_and_destroy_test_ffplayer();
#endif

#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT "/usr/lib/ladspa/cmt.so\0"
#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_DELAY "Echo Delay Line (Maximum Delay 1s)\0"
#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_FREEVERB "Freeverb (Version 3)\0"

#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH "/usr/lib/lv2/gverb-swh.lv2/plugin-linux.so\0"
#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH_GVERB "GVerb\0"

#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_CONFIG "[generic]\n" \
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
ags_functional_line_member_add_and_destroy_test_add_test()
{

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "functional test of GSequencer line member add and destroy AgsPanel\0", ags_functional_line_member_add_and_destroy_test_panel) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer line member add and destroy AgsMixer\0", ags_functional_line_member_add_and_destroy_test_mixer) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer line member add and destroy AgsDrum\0", ags_functional_line_member_add_and_destroy_test_drum) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer line member add and destroy AgsMatrix\0", ags_functional_line_member_add_and_destroy_test_matrix) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer line member add and destroy AgsSynth\0", ags_functional_line_member_add_and_destroy_test_synth) == NULL)
#ifdef AGS_WITH_LIBINSTPATCH
     || (CU_add_test(pSuite, "functional test of GSequencer line member add and destroy AgsFFPlayer\0", ags_functional_line_member_add_and_destroy_test_ffplayer) == NULL)
#endif
     ){
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
ags_functional_line_member_add_and_destroy_test_init_suite()
{
  /* get gui thread */
  gui_thread = ags_thread_find_type(ags_application_context->main_loop,
				    AGS_TYPE_GUI_THREAD);

  /* get task thread */
  task_thread = ags_thread_find_type(ags_application_context->main_loop,
				     AGS_TYPE_TASK_THREAD);

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_line_member_add_and_destroy_test_clean_suite()
{  
  return(0);
}

void
ags_functional_line_member_add_and_destroy_test_panel()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsLineEditor *line_editor;
  AgsPluginBrowser *plugin_browser;

  GList *list_start, *list;
  
  guint nth_machine;
  guint nth_pad, nth_audio_channel;
  gboolean success;

  xorg_application_context = ags_application_context;
  
  /* add panel */
  success = ags_functional_test_util_add_machine(NULL,
						 "Panel");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  if(list != NULL &&
     AGS_IS_PANEL(list->data)){
    machine = list->data;
  }else{
    machine = NULL;
  }
  
  CU_ASSERT(machine != NULL);
  
  /*
   *  add line member to input
   */

  nth_machine = 0;

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_machine);

  CU_ASSERT(success == TRUE);
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_machine,
								  AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB);

  /* add ladspa effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  success = ags_functional_test_util_machine_properties_effect_add(nth_machine,
								   nth_pad, nth_audio_channel);

  CU_ASSERT(success == TRUE);

  /* ladspa */
  success = ags_functional_test_util_machine_properties_effect_plugin_type(nth_machine,
									   nth_pad, nth_audio_channel,
									   "LADSPA\0");

  CU_ASSERT(success == TRUE);

  /* cmt and delay */  
  success = ags_functional_test_util_machine_properties_ladspa_filename(nth_machine,
									nth_pad, nth_audio_channel,
									AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_ladspa_effect(nth_machine,
								      nth_pad, nth_audio_channel,
								      AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_DELAY);

  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* add ladspa effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  success = ags_functional_test_util_machine_properties_effect_add(nth_machine,
								   nth_pad, nth_audio_channel);

  CU_ASSERT(success == TRUE);

  /* ladspa */
  success = ags_functional_test_util_machine_properties_effect_plugin_type(nth_machine,
									   nth_pad, nth_audio_channel,
									   "LADSPA\0");

  CU_ASSERT(success == TRUE);

  /* cmt and freeverb */  
  success = ags_functional_test_util_machine_properties_ladspa_filename(nth_machine,
									nth_pad, nth_audio_channel,
									AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_ladspa_effect(nth_machine,
								      nth_pad, nth_audio_channel,
								      AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_FREEVERB);

  CU_ASSERT(success == TRUE);
  
  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = AGS_MACHINE(machine)->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);
  
  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_machine);

  CU_ASSERT(success == TRUE);
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_machine,
								  AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB);

  /* add lv2 effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  success = ags_functional_test_util_machine_properties_effect_add(nth_machine,
								   nth_pad, nth_audio_channel);

  CU_ASSERT(success == TRUE);

  /* lv2 */
  success = ags_functional_test_util_machine_properties_effect_plugin_type(nth_machine,
									   nth_pad, nth_audio_channel,
									   "Lv2\0");

  CU_ASSERT(success == TRUE);

  /* lv2 and gverb */  
  success = ags_functional_test_util_machine_properties_lv2_filename(nth_machine,
								     nth_pad, nth_audio_channel,
								     AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_lv2_effect(nth_machine,
								   nth_pad, nth_audio_channel,
								   AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH_GVERB);

  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);

  /*
   * destroy effects
   */  
  nth_machine = 0;

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_machine);

  CU_ASSERT(success == TRUE);
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_machine,
								  AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB);

  /* remove effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  success = ags_functional_test_util_machine_properties_effect_remove(nth_machine,
								      nth_pad, nth_audio_channel,
								      2);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_effect_remove(nth_machine,
								      nth_pad, nth_audio_channel,
								      0);

  CU_ASSERT(success == TRUE);
  
  success = ags_functional_test_util_machine_properties_effect_remove(nth_machine,
								      nth_pad, nth_audio_channel,
								      0);

  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);

  /* destroy panel */
  success = ags_functional_test_util_machine_destroy(nth_machine);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_line_member_add_and_destroy_test_mixer()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsLineEditor *line_editor;
  AgsPluginBrowser *plugin_browser;
  
  GList *list_start, *list;

  guint nth_machine;
  guint nth_pad, nth_audio_channel;
  gboolean success;

  xorg_application_context = ags_application_context;
  
  /* add mixer */
  success = ags_functional_test_util_add_machine(NULL,
						 "Mixer");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  if(list != NULL &&
     AGS_IS_MIXER(list->data)){
    machine = list->data;
  }else{
    machine = NULL;
  }
  
  CU_ASSERT(machine != NULL);
  
  /*
   *  add line member to input
   */

  nth_machine = 0;

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_machine);

  CU_ASSERT(success == TRUE);
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_machine,
								  AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB);

  /* add ladspa effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  success = ags_functional_test_util_machine_properties_effect_add(nth_machine,
								   nth_pad, nth_audio_channel);

  CU_ASSERT(success == TRUE);

  /* ladspa */
  success = ags_functional_test_util_machine_properties_effect_plugin_type(nth_machine,
									   nth_pad, nth_audio_channel,
									   "LADSPA\0");

  CU_ASSERT(success == TRUE);

  /* cmt and delay */  
  success = ags_functional_test_util_machine_properties_ladspa_filename(nth_machine,
									nth_pad, nth_audio_channel,
									AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_ladspa_effect(nth_machine,
								      nth_pad, nth_audio_channel,
								      AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_DELAY);

  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* add ladspa effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  success = ags_functional_test_util_machine_properties_effect_add(nth_machine,
								   nth_pad, nth_audio_channel);

  CU_ASSERT(success == TRUE);

  /* ladspa */
  success = ags_functional_test_util_machine_properties_effect_plugin_type(nth_machine,
									   nth_pad, nth_audio_channel,
									   "LADSPA\0");

  CU_ASSERT(success == TRUE);

  /* cmt and freeverb */  
  success = ags_functional_test_util_machine_properties_ladspa_filename(nth_machine,
									nth_pad, nth_audio_channel,
									AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_ladspa_effect(nth_machine,
								      nth_pad, nth_audio_channel,
								      AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_FREEVERB);

  CU_ASSERT(success == TRUE);
  
  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = AGS_MACHINE(machine)->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);
  
  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_machine);

  CU_ASSERT(success == TRUE);
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_machine,
								  AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB);

  /* add lv2 effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  success = ags_functional_test_util_machine_properties_effect_add(nth_machine,
								   nth_pad, nth_audio_channel);

  CU_ASSERT(success == TRUE);

  /* lv2 */
  success = ags_functional_test_util_machine_properties_effect_plugin_type(nth_machine,
									   nth_pad, nth_audio_channel,
									   "Lv2\0");

  CU_ASSERT(success == TRUE);

  /* lv2 and gverb */  
  success = ags_functional_test_util_machine_properties_lv2_filename(nth_machine,
								     nth_pad, nth_audio_channel,
								     AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_lv2_effect(nth_machine,
								   nth_pad, nth_audio_channel,
								   AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH_GVERB);

  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);

  /*
   * destroy effects
   */  
  nth_machine = 0;

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_machine);

  CU_ASSERT(success == TRUE);
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_machine,
								  AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB);

  /* remove effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  success = ags_functional_test_util_machine_properties_effect_remove(nth_machine,
								      nth_pad, nth_audio_channel,
								      2);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_effect_remove(nth_machine,
								      nth_pad, nth_audio_channel,
								      0);

  CU_ASSERT(success == TRUE);
  
  success = ags_functional_test_util_machine_properties_effect_remove(nth_machine,
								      nth_pad, nth_audio_channel,
								      0);

  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);
  
  /* destroy mixer */
  success = ags_functional_test_util_machine_destroy(nth_machine);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_line_member_add_and_destroy_test_drum()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsLineEditor *line_editor;
  AgsPluginBrowser *plugin_browser;
  
  GList *list_start, *list;

  guint nth_machine;
  guint nth_pad, nth_audio_channel;
  gboolean success;

  xorg_application_context = ags_application_context;
  
  /* add drum */
  success = ags_functional_test_util_add_machine(NULL,
						 "Drum");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  if(list != NULL &&
     AGS_IS_DRUM(list->data)){
    machine = list->data;
  }else{
    machine = NULL;
  }
  
  CU_ASSERT(machine != NULL);

  /*
   *  add line member to input
   */

  nth_machine = 0;

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_machine);

  CU_ASSERT(success == TRUE);
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_machine,
								  AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB);

  /* add ladspa effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  success = ags_functional_test_util_machine_properties_effect_add(nth_machine,
								   nth_pad, nth_audio_channel);

  CU_ASSERT(success == TRUE);

  /* ladspa */
  success = ags_functional_test_util_machine_properties_effect_plugin_type(nth_machine,
									   nth_pad, nth_audio_channel,
									   "LADSPA\0");

  CU_ASSERT(success == TRUE);

  /* cmt and delay */  
  success = ags_functional_test_util_machine_properties_ladspa_filename(nth_machine,
									nth_pad, nth_audio_channel,
									AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_ladspa_effect(nth_machine,
								      nth_pad, nth_audio_channel,
								      AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_DELAY);

  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* add ladspa effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  success = ags_functional_test_util_machine_properties_effect_add(nth_machine,
								   nth_pad, nth_audio_channel);

  CU_ASSERT(success == TRUE);

  /* ladspa */
  success = ags_functional_test_util_machine_properties_effect_plugin_type(nth_machine,
									   nth_pad, nth_audio_channel,
									   "LADSPA\0");

  CU_ASSERT(success == TRUE);

  /* cmt and freeverb */  
  success = ags_functional_test_util_machine_properties_ladspa_filename(nth_machine,
									nth_pad, nth_audio_channel,
									AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_ladspa_effect(nth_machine,
								      nth_pad, nth_audio_channel,
								      AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_FREEVERB);

  CU_ASSERT(success == TRUE);
  
  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = AGS_MACHINE(machine)->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);
  
  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_machine);

  CU_ASSERT(success == TRUE);
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_machine,
								  AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB);

  /* add lv2 effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  success = ags_functional_test_util_machine_properties_effect_add(nth_machine,
								   nth_pad, nth_audio_channel);

  CU_ASSERT(success == TRUE);

  /* lv2 */
  success = ags_functional_test_util_machine_properties_effect_plugin_type(nth_machine,
									   nth_pad, nth_audio_channel,
									   "Lv2\0");

  CU_ASSERT(success == TRUE);

  /* lv2 and gverb */  
  success = ags_functional_test_util_machine_properties_lv2_filename(nth_machine,
								     nth_pad, nth_audio_channel,
								     AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_lv2_effect(nth_machine,
								   nth_pad, nth_audio_channel,
								   AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH_GVERB);

  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);

  /*
   * destroy effects
   */  
  nth_machine = 0;

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_machine);

  CU_ASSERT(success == TRUE);
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_machine,
								  AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB);

  /* remove effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  success = ags_functional_test_util_machine_properties_effect_remove(nth_machine,
								      nth_pad, nth_audio_channel,
								      2);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_effect_remove(nth_machine,
								      nth_pad, nth_audio_channel,
								      0);

  CU_ASSERT(success == TRUE);
  
  success = ags_functional_test_util_machine_properties_effect_remove(nth_machine,
								      nth_pad, nth_audio_channel,
								      0);

  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);
  
  /* destroy drum */
  success = ags_functional_test_util_machine_destroy(nth_machine);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_line_member_add_and_destroy_test_matrix()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;

  GList *list_start, *list;

  guint nth_machine;
  gboolean success;

  xorg_application_context = ags_application_context;
  
  /* add matrix */
  success = ags_functional_test_util_add_machine(NULL,
						 "Matrix");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  if(list != NULL &&
     AGS_IS_MATRIX(list->data)){
    machine = list->data;
  }else{
    machine = NULL;
  }
  
  CU_ASSERT(machine != NULL);

  //TODO:JK: 
  nth_machine = 0;
  
  /* destroy matrix */
  success = ags_functional_test_util_machine_destroy(nth_machine);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_line_member_add_and_destroy_test_synth()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  
  GList *list_start, *list;

  guint nth_machine;
  gboolean success;

  xorg_application_context = ags_application_context;
  
  /* add synth */
  success = ags_functional_test_util_add_machine(NULL,
						 "Synth");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  if(list != NULL &&
     AGS_IS_SYNTH(list->data)){
    machine = list->data;
  }else{
    machine = NULL;
  }
  
  CU_ASSERT(machine != NULL);

  //TODO:JK: 
  nth_machine = 0;
  
  /* destroy synth */
  success = ags_functional_test_util_machine_destroy(nth_machine);
  
  CU_ASSERT(success == TRUE);
}

#ifdef AGS_WITH_LIBINSTPATCH
void
ags_functional_line_member_add_and_destroy_test_ffplayer()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsLineEditor *line_editor;
  AgsPluginBrowser *plugin_browser;
  
  GList *list_start, *list;

  guint nth_machine;
  guint nth_pad, nth_audio_channel;
  gboolean success;

  xorg_application_context = ags_application_context;
  
  /* add fplayer */
  success = ags_functional_test_util_add_machine(NULL,
						 "FPlayer");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  if(list != NULL &&
     AGS_IS_FFPLAYER(list->data)){
    machine = list->data;
  }else{
    machine = NULL;
  }
  
  CU_ASSERT(machine != NULL);

  /*
   *  add line member to input
   */

  nth_machine = 0;

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_machine);

  CU_ASSERT(success == TRUE);
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_machine,
								  AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB);

  /* add ladspa effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  success = ags_functional_test_util_machine_properties_effect_add(nth_machine,
								   nth_pad, nth_audio_channel);

  CU_ASSERT(success == TRUE);

  /* ladspa */
  success = ags_functional_test_util_machine_properties_effect_plugin_type(nth_machine,
									   nth_pad, nth_audio_channel,
									   "LADSPA\0");

  CU_ASSERT(success == TRUE);

  /* cmt and delay */  
  success = ags_functional_test_util_machine_properties_ladspa_filename(nth_machine,
									nth_pad, nth_audio_channel,
									AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_ladspa_effect(nth_machine,
								      nth_pad, nth_audio_channel,
								      AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_DELAY);

  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* add ladspa effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  success = ags_functional_test_util_machine_properties_effect_add(nth_machine,
								   nth_pad, nth_audio_channel);

  CU_ASSERT(success == TRUE);

  /* ladspa */
  success = ags_functional_test_util_machine_properties_effect_plugin_type(nth_machine,
									   nth_pad, nth_audio_channel,
									   "LADSPA\0");

  CU_ASSERT(success == TRUE);

  /* cmt and freeverb */  
  success = ags_functional_test_util_machine_properties_ladspa_filename(nth_machine,
									nth_pad, nth_audio_channel,
									AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_ladspa_effect(nth_machine,
								      nth_pad, nth_audio_channel,
								      AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_FREEVERB);

  CU_ASSERT(success == TRUE);
  
  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = AGS_MACHINE(machine)->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);
  
  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_machine);

  CU_ASSERT(success == TRUE);
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_machine,
								  AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB);

  /* add lv2 effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  success = ags_functional_test_util_machine_properties_effect_add(nth_machine,
								   nth_pad, nth_audio_channel);

  CU_ASSERT(success == TRUE);

  /* lv2 */
  success = ags_functional_test_util_machine_properties_effect_plugin_type(nth_machine,
									   nth_pad, nth_audio_channel,
									   "Lv2\0");

  CU_ASSERT(success == TRUE);

  /* lv2 and gverb */  
  success = ags_functional_test_util_machine_properties_lv2_filename(nth_machine,
								     nth_pad, nth_audio_channel,
								     AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_lv2_effect(nth_machine,
								   nth_pad, nth_audio_channel,
								   AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH_GVERB);

  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);

  /*
   * destroy effects
   */  
  nth_machine = 0;

  /* open properties */
  success = ags_functional_test_util_machine_properties_open(nth_machine);

  CU_ASSERT(success == TRUE);
  
  success = ags_functional_test_util_machine_properties_click_tab(nth_machine,
								  AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB);

  /* remove effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  success = ags_functional_test_util_machine_properties_effect_remove(nth_machine,
								      nth_pad, nth_audio_channel,
								      2);

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_properties_effect_remove(nth_machine,
								      nth_pad, nth_audio_channel,
								      0);

  CU_ASSERT(success == TRUE);
  
  success = ags_functional_test_util_machine_properties_effect_remove(nth_machine,
								      nth_pad, nth_audio_channel,
								      0);

  CU_ASSERT(success == TRUE);

  /* response OK */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  machine_editor = machine->properties;

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);
  
  /* destroy fplayer */
  success = ags_functional_test_util_machine_destroy(nth_machine);
  
  CU_ASSERT(success == TRUE);
}
#endif

int
main(int argc, char **argv)
{
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFuncitonalLineMemberAddAndDestroyTest\0", ags_functional_line_member_add_and_destroy_test_init_suite, ags_functional_line_member_add_and_destroy_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  g_atomic_int_set(&is_available,
		   FALSE);
  
  ags_test_init(&argc, &argv,
		AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_CONFIG);
  ags_functional_test_util_do_run(argc, argv,
				  ags_functional_line_member_add_and_destroy_test_add_test, &is_available);

  return(-1);
}

