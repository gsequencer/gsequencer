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

#ifdef AGS_WITH_LIBINSTPATCH
#include <libinstpatch/libinstpatch.h>
#endif

#include <ags/gsequencer_main.h>

#include <ags/test/GSequencer/libgsequencer.h>

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

#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT "/usr/lib/ladspa/cmt.so"
#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_DELAY "Echo Delay Line (Maximum Delay 1s)"
#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_FREEVERB "Freeverb (Version 3)"

#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH "/usr/lib/lv2/gverb-swh.lv2/plugin-linux.so"
#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH_GVERB "GVerb"

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

struct timespec ags_functional_line_member_add_and_destroy_test_default_timeout = {
  300,
  0,
};

void
ags_functional_line_member_add_and_destroy_test_add_test()
{
  /* add the tests to the suite */
  if((CU_add_test(pSuite, "functional test of GSequencer line member add and destroy AgsPanel", ags_functional_line_member_add_and_destroy_test_panel) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer line member add and destroy AgsMixer", ags_functional_line_member_add_and_destroy_test_mixer) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer line member add and destroy AgsDrum", ags_functional_line_member_add_and_destroy_test_drum) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer line member add and destroy AgsMatrix", ags_functional_line_member_add_and_destroy_test_matrix) == NULL) ||
     (CU_add_test(pSuite, "functional test of GSequencer line member add and destroy AgsSynth", ags_functional_line_member_add_and_destroy_test_synth) == NULL)
#ifdef AGS_WITH_LIBINSTPATCH
     || (CU_add_test(pSuite, "functional test of GSequencer line member add and destroy AgsFFPlayer", ags_functional_line_member_add_and_destroy_test_ffplayer) == NULL)
#endif
     ){
    CU_cleanup_registry();
      
    exit(CU_get_error());
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  ags_test_quit();
  
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
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = ags_application_context;

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_realized),
						      &ags_functional_line_member_add_and_destroy_test_default_timeout,
						      &(gsequencer_application_context->window));

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
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsLineEditor *line_editor;
  AgsPluginBrowser *plugin_browser;

  GList *list_start, *list;

  struct _AgsFunctionalTestUtilContainerTest container_test;
  
  guint nth_machine;
  guint nth_pad, nth_audio_channel;
  gboolean success;

  gsequencer_application_context = ags_application_context;
  
  /* add panel */
  success = ags_functional_test_util_add_machine(NULL,
						 "Panel");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  nth_machine = 0;

  container_test.container = &(AGS_WINDOW(gsequencer_application_context->window)->machines);
  container_test.count = nth_machine + 1;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_container_children_count),
						      ags_functional_test_util_get_default_timeout(),
						      &container_test);
  
  ags_test_enter();

  list_start = gtk_container_get_children(AGS_WINDOW(gsequencer_application_context->window)->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();

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
									   "LADSPA");

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
  ags_test_enter();

  machine_editor = machine->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  ags_test_leave();

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
									   "LADSPA");

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
  ags_test_enter();

  machine_editor = AGS_MACHINE(machine)->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  ags_test_leave();

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* response OK */
  ags_test_enter();

  machine_editor = machine->properties;

  ags_test_leave();

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
									   "Lv2");

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
  ags_test_enter();

  machine_editor = machine->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  ags_test_leave();

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* response OK */
  ags_test_enter();

  machine_editor = machine->properties;

  ags_test_leave();

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);

  /*
   * destroy effects
   */  

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
  ags_test_enter();

  machine_editor = machine->properties;

  ags_test_leave();

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);

  /* destroy panel */
  success = ags_functional_test_util_machine_destroy(nth_machine);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_line_member_add_and_destroy_test_mixer()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsLineEditor *line_editor;
  AgsPluginBrowser *plugin_browser;
  
  GList *list_start, *list;

  struct _AgsFunctionalTestUtilContainerTest container_test;

  guint nth_machine;
  guint nth_pad, nth_audio_channel;
  gboolean success;

  gsequencer_application_context = ags_application_context;
  
  /* add mixer */
  success = ags_functional_test_util_add_machine(NULL,
						 "Mixer");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  nth_machine = 0;
  
  container_test.container = &(AGS_WINDOW(gsequencer_application_context->window)->machines);
  container_test.count = nth_machine + 1;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_container_children_count),
						      ags_functional_test_util_get_default_timeout(),
						      &container_test);

  ags_test_enter();

  list_start = gtk_container_get_children(AGS_WINDOW(gsequencer_application_context->window)->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();

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
									   "LADSPA");

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
  ags_test_enter();

  machine_editor = machine->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  ags_test_leave();

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
									   "LADSPA");

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
  ags_test_enter();

  machine_editor = AGS_MACHINE(machine)->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  ags_test_leave();

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* response OK */
  ags_test_enter();

  machine_editor = machine->properties;

  ags_test_leave();

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
									   "Lv2");

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
  ags_test_enter();

  machine_editor = machine->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  ags_test_leave();

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* response OK */
  ags_test_enter();

  machine_editor = machine->properties;

  ags_test_leave();

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);

  /*
   * destroy effects
   */  

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
  ags_test_enter();

  machine_editor = machine->properties;

  ags_test_leave();

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);
  
  /* destroy mixer */
  success = ags_functional_test_util_machine_destroy(nth_machine);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_line_member_add_and_destroy_test_drum()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsLineEditor *line_editor;
  AgsPluginBrowser *plugin_browser;
  
  GList *list_start, *list;

  struct _AgsFunctionalTestUtilContainerTest container_test;

  guint nth_machine;
  guint nth_pad, nth_audio_channel;
  gboolean success;

  gsequencer_application_context = ags_application_context;
  
  /* add drum */
  success = ags_functional_test_util_add_machine(NULL,
						 "Drum");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  nth_machine = 0;
  
  container_test.container = &(AGS_WINDOW(gsequencer_application_context->window)->machines);
  container_test.count = nth_machine + 1;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_container_children_count),
						      ags_functional_test_util_get_default_timeout(),
						      &container_test);

  ags_test_enter();

  list_start = gtk_container_get_children(AGS_WINDOW(gsequencer_application_context->window)->machines);
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
   *  add line member to input
   */


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
									   "LADSPA");

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
  ags_test_enter();

  machine_editor = machine->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  ags_test_leave();

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
									   "LADSPA");

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
  ags_test_enter();

  machine_editor = AGS_MACHINE(machine)->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  ags_test_leave();

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* response OK */
  ags_test_enter();

  machine_editor = machine->properties;

  ags_test_leave();

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
									   "Lv2");

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
  ags_test_enter();

  machine_editor = machine->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  ags_test_leave();

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* response OK */
  ags_test_enter();

  machine_editor = machine->properties;

  ags_test_leave();

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);

  /*
   * destroy effects
   */  

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
  ags_test_enter();

  machine_editor = machine->properties;

  ags_test_leave();

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);
  
  /* destroy drum */
  success = ags_functional_test_util_machine_destroy(nth_machine);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_line_member_add_and_destroy_test_matrix()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;

  GList *list_start, *list;

  struct _AgsFunctionalTestUtilContainerTest container_test;

  guint nth_machine;
  gboolean success;

  gsequencer_application_context = ags_application_context;
  
  /* add matrix */
  success = ags_functional_test_util_add_machine(NULL,
						 "Matrix");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  nth_machine = 0;
  
  container_test.container = &(AGS_WINDOW(gsequencer_application_context->window)->machines);
  container_test.count = nth_machine + 1;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_container_children_count),
						      ags_functional_test_util_get_default_timeout(),
						      &container_test);

  ags_test_enter();

  list_start = gtk_container_get_children(AGS_WINDOW(gsequencer_application_context->window)->machines);
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

  //TODO:JK: 
  
  /* destroy matrix */
  success = ags_functional_test_util_machine_destroy(nth_machine);
  
  CU_ASSERT(success == TRUE);
}

void
ags_functional_line_member_add_and_destroy_test_synth()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  
  GList *list_start, *list;

  struct _AgsFunctionalTestUtilContainerTest container_test;

  guint nth_machine;
  gboolean success;

  gsequencer_application_context = ags_application_context;
  
  /* add synth */
  success = ags_functional_test_util_add_machine(NULL,
						 "Synth");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  nth_machine = 0;
  
  container_test.container = &(AGS_WINDOW(gsequencer_application_context->window)->machines);
  container_test.count = nth_machine + 1;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_container_children_count),
						      ags_functional_test_util_get_default_timeout(),
						      &container_test);

  ags_test_enter();

  list_start = gtk_container_get_children(AGS_WINDOW(gsequencer_application_context->window)->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();

  if(list != NULL &&
     AGS_IS_SYNTH(list->data)){
    machine = list->data;
  }else{
    machine = NULL;
  }
  
  CU_ASSERT(machine != NULL);

  //TODO:JK: 
  
  /* destroy synth */
  success = ags_functional_test_util_machine_destroy(nth_machine);
  
  CU_ASSERT(success == TRUE);
}

#ifdef AGS_WITH_LIBINSTPATCH
void
ags_functional_line_member_add_and_destroy_test_ffplayer()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsLineEditor *line_editor;
  AgsPluginBrowser *plugin_browser;
  
  GList *list_start, *list;

  struct _AgsFunctionalTestUtilContainerTest container_test;

  guint nth_machine;
  guint nth_pad, nth_audio_channel;
  gboolean success;

  gsequencer_application_context = ags_application_context;
  
  /* add fplayer */
  success = ags_functional_test_util_add_machine(NULL,
						 "FPlayer");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  nth_machine = 0;
  
  container_test.container = &(AGS_WINDOW(gsequencer_application_context->window)->machines);
  container_test.count = nth_machine + 1;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_container_children_count),
						      ags_functional_test_util_get_default_timeout(),
						      &container_test);

  ags_test_enter();

  list_start = gtk_container_get_children(AGS_WINDOW(gsequencer_application_context->window)->machines);
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
   *  add line member to input
   */

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
									   "LADSPA");

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
  ags_test_enter();

  machine_editor = machine->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  ags_test_leave();

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
									   "LADSPA");

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
  ags_test_enter();

  machine_editor = AGS_MACHINE(machine)->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  ags_test_leave();

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* response OK */
  ags_test_enter();

  machine_editor = machine->properties;

  ags_test_leave();

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
									   "Lv2");

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
  ags_test_enter();

  machine_editor = machine->properties;

  line_editor = ags_functional_test_util_get_line_editor(machine_editor,
							 nth_pad, nth_audio_channel,
							 FALSE);

  plugin_browser = line_editor->member_editor->plugin_browser;
  
  ags_test_leave();

  success = ags_functional_test_util_dialog_ok(plugin_browser);
  
  CU_ASSERT(success == TRUE);

  /* response OK */
  ags_test_enter();

  machine_editor = machine->properties;

  ags_test_leave();

  success = ags_functional_test_util_dialog_ok(machine_editor);
  
  CU_ASSERT(success == TRUE);

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
  ags_test_enter();

  machine_editor = machine->properties;

  ags_test_leave();

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
  gchar *str;
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFuncitonalLineMemberAddAndDestroyTest", ags_functional_line_member_add_and_destroy_test_init_suite, ags_functional_line_member_add_and_destroy_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  g_atomic_int_set(&is_available,
		   FALSE);
  
#if defined(AGS_TEST_CONFIG)
  ags_test_init(&argc, &argv,
		AGS_TEST_CONFIG);
#else
  if((str = getenv("AGS_TEST_CONFIG")) != NULL){
    ags_test_init(&argc, &argv,
		  str);
  }else{
    ags_test_init(&argc, &argv,
		  AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_CONFIG);
  }
#endif
  
  ags_functional_test_util_do_run(argc, argv,
				  ags_functional_line_member_add_and_destroy_test_add_test, &is_available);

  g_thread_join(ags_functional_test_util_test_runner_thread());

  return(-1);
}
