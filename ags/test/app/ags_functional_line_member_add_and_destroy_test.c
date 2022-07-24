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

#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME (3.0 * G_USEC_PER_SEC)

#define AGS_LIB64_LIBDIR (1)

#if defined(AGS_LIB_LIBDIR)
#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT "/usr/lib/ladspa/cmt.so"
#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_DELAY "Echo Delay Line (Maximum Delay 1s)"
#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_FREEVERB "Freeverb (Version 3)"

#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH "/usr/lib/lv2/gverb-swh.lv2/plugin-linux.so"
#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH_GVERB "GVerb"
#elif defined(AGS_LIB64_LIBDIR)
#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT "/usr/lib64/ladspa/cmt.so"
#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_DELAY "Echo Delay Line (Maximum Delay 1s)"
#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_FREEVERB "Freeverb (Version 3)"

#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH "/usr/lib64/lv2/gverb-swh.lv2/plugin-Linux.so"
#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH_GVERB "GVerb"
#endif

#define AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_CONFIG "[generic]\n" \
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

  ags_functional_test_util_quit();
  
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
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  AgsLineMemberEditor *line_member_editor;
  AgsPluginBrowser *plugin_browser;
  GtkWidget **widget;
  
  GList *start_list, *list;

  AgsFunctionalTestUtilListLengthCondition condition;
  
  guint nth_machine;
  guint nth_pad, nth_audio_channel;
  gboolean success;

  gsequencer_application_context = ags_application_context;
  
  /* add panel */
  ags_functional_test_util_add_machine(NULL,
				       "Panel");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 1;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_line_member_add_and_destroy_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve panel */
  nth_machine = 0;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_PANEL);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);
  
  /*
   *  add line member to input
   */

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* add ladspa effect */
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_machine,
							   AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  nth_pad = 0;
  nth_audio_channel = 0;
  
  ags_functional_test_util_machine_editor_dialog_effect_add(nth_machine,
							    nth_pad, nth_audio_channel);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  machine_editor_listing = NULL;
  
  switch(AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB){
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
  }

  start_list = ags_machine_editor_listing_get_pad(machine_editor_listing);

  machine_editor_pad = g_list_nth_data(start_list,
				       nth_pad);
    
  start_list = ags_machine_editor_pad_get_line(machine_editor_pad);

  machine_editor_line = g_list_nth_data(start_list,
					nth_audio_channel);

  line_member_editor = machine_editor_line->line_member_editor;

  widget = &(line_member_editor->plugin_browser);
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_line_member_add_and_destroy_test_default_timeout,
						      widget);

  ags_functional_test_util_sync();

  /* ladspa */
  ags_functional_test_util_machine_editor_dialog_effect_plugin_type(nth_machine,
								    nth_pad, nth_audio_channel,
								    "LADSPA");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* cmt and delay */  
  ags_functional_test_util_machine_editor_dialog_ladspa_filename(nth_machine,
								 nth_pad, nth_audio_channel,
								 AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_ladspa_effect(nth_machine,
							       nth_pad, nth_audio_channel,
							       AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_DELAY);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  machine_editor_line = ags_functional_test_util_get_machine_editor_dialog_line(machine->machine_editor_dialog,
										nth_pad, nth_audio_channel,
										FALSE);

  plugin_browser = machine_editor_line->line_member_editor->plugin_browser;
  
  ags_functional_test_util_dialog_ok(plugin_browser);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* add ladspa effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  ags_functional_test_util_machine_editor_dialog_effect_add(nth_machine,
							    nth_pad, nth_audio_channel);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* ladspa */
  ags_functional_test_util_machine_editor_dialog_effect_plugin_type(nth_machine,
								    nth_pad, nth_audio_channel,
								    "LADSPA");
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* cmt and freeverb */  
  ags_functional_test_util_machine_editor_dialog_ladspa_filename(nth_machine,
								 nth_pad, nth_audio_channel,
								 AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_ladspa_effect(nth_machine,
							       nth_pad, nth_audio_channel,
							       AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_FREEVERB);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* response OK */
  machine_editor_line = ags_functional_test_util_get_machine_editor_dialog_line(machine->machine_editor_dialog,
										nth_pad, nth_audio_channel,
										FALSE);

  plugin_browser = machine_editor_line->line_member_editor->plugin_browser;

  ags_functional_test_util_dialog_ok(plugin_browser);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  ags_functional_test_util_dialog_ok(machine->machine_editor_dialog);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* add lv2 effect */
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_machine,
							   AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  nth_pad = 0;
  nth_audio_channel = 0;

  ags_functional_test_util_machine_editor_dialog_effect_add(nth_machine,
							    nth_pad, nth_audio_channel);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  machine_editor_listing = NULL;
  
  switch(AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB){
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
  }

  start_list = ags_machine_editor_listing_get_pad(machine_editor_listing);

  machine_editor_pad = g_list_nth_data(start_list,
				       nth_pad);
    
  start_list = ags_machine_editor_pad_get_line(machine_editor_pad);

  machine_editor_line = g_list_nth_data(start_list,
					nth_audio_channel);

  line_member_editor = machine_editor_line->line_member_editor;

  widget = &(line_member_editor->plugin_browser);
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_line_member_add_and_destroy_test_default_timeout,
						      widget);

  ags_functional_test_util_sync();

  /* lv2 */
  ags_functional_test_util_machine_editor_dialog_effect_plugin_type(nth_machine,
								    nth_pad, nth_audio_channel,
								    "Lv2");
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* lv2 and gverb */  
  ags_functional_test_util_machine_editor_dialog_lv2_filename(nth_machine,
							      nth_pad, nth_audio_channel,
							      AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_lv2_effect(nth_machine,
							    nth_pad, nth_audio_channel,
							    AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH_GVERB);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  machine_editor_line = ags_functional_test_util_get_machine_editor_dialog_line(machine->machine_editor_dialog,
										nth_pad, nth_audio_channel,
										FALSE);

  plugin_browser = machine_editor_line->line_member_editor->plugin_browser;

  ags_functional_test_util_dialog_ok(plugin_browser);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  ags_functional_test_util_dialog_ok(machine->machine_editor_dialog);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*
   * destroy effects
   */  

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* remove effect */
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_machine,
							   AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  nth_pad = 0;
  nth_audio_channel = 0;
  
  ags_functional_test_util_machine_editor_dialog_effect_remove(nth_machine,
							       nth_pad, nth_audio_channel,
							       2);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_effect_remove(nth_machine,
							       nth_pad, nth_audio_channel,
							       0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  ags_functional_test_util_machine_editor_dialog_effect_remove(nth_machine,
							       nth_pad, nth_audio_channel,
							       0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  ags_functional_test_util_dialog_ok(machine->machine_editor_dialog);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* destroy panel */
  ags_functional_test_util_machine_destroy(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
}

void
ags_functional_line_member_add_and_destroy_test_mixer()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditorLine *machine_editor_line;  
  AgsPluginBrowser *plugin_browser;
  
  GList *start_list, *list;

  AgsFunctionalTestUtilListLengthCondition condition;

  guint nth_machine;
  guint nth_pad, nth_audio_channel;
  gboolean success;

  gsequencer_application_context = ags_application_context;
  
  /* add mixer */
  ags_functional_test_util_add_machine(NULL,
				       "Mixer");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 1;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_line_member_add_and_destroy_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve mixer */
  nth_machine = 0;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_MIXER);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);
  
  /*
   *  add line member to input
   */

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* add ladspa effect */
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_machine,
							   AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  nth_pad = 0;
  nth_audio_channel = 0;
  
  ags_functional_test_util_machine_editor_dialog_effect_add(nth_machine,
							    nth_pad, nth_audio_channel);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* ladspa */
  ags_functional_test_util_machine_editor_dialog_effect_plugin_type(nth_machine,
								    nth_pad, nth_audio_channel,
								    "LADSPA");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* cmt and delay */  
  ags_functional_test_util_machine_editor_dialog_ladspa_filename(nth_machine,
								 nth_pad, nth_audio_channel,
								 AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_ladspa_effect(nth_machine,
							       nth_pad, nth_audio_channel,
							       AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_DELAY);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  machine_editor_line = ags_functional_test_util_get_machine_editor_dialog_line(machine->machine_editor_dialog,
										nth_pad, nth_audio_channel,
										FALSE);

  plugin_browser = machine_editor_line->line_member_editor->plugin_browser;

  ags_functional_test_util_dialog_ok(plugin_browser);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* add ladspa effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  ags_functional_test_util_machine_editor_dialog_effect_add(nth_machine,
							    nth_pad, nth_audio_channel);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* ladspa */
  ags_functional_test_util_machine_editor_dialog_effect_plugin_type(nth_machine,
								    nth_pad, nth_audio_channel,
								    "LADSPA");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* cmt and freeverb */  
  ags_functional_test_util_machine_editor_dialog_ladspa_filename(nth_machine,
								 nth_pad, nth_audio_channel,
								 AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_ladspa_effect(nth_machine,
							       nth_pad, nth_audio_channel,
							       AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_FREEVERB);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* response OK */
  machine_editor_line = ags_functional_test_util_get_machine_editor_dialog_line(machine->machine_editor_dialog,
										nth_pad, nth_audio_channel,
										FALSE);

  plugin_browser = machine_editor_line->line_member_editor->plugin_browser;

  ags_functional_test_util_dialog_ok(plugin_browser);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  ags_functional_test_util_dialog_ok(machine->machine_editor_dialog);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* add lv2 effect */
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_machine,
							   AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  nth_pad = 0;
  nth_audio_channel = 0;
  
  ags_functional_test_util_machine_editor_dialog_effect_add(nth_machine,
							    nth_pad, nth_audio_channel);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* lv2 */
  ags_functional_test_util_machine_editor_dialog_effect_plugin_type(nth_machine,
								    nth_pad, nth_audio_channel,
								    "Lv2");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* lv2 and gverb */  
  ags_functional_test_util_machine_editor_dialog_lv2_filename(nth_machine,
							      nth_pad, nth_audio_channel,
							      AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_lv2_effect(nth_machine,
							    nth_pad, nth_audio_channel,
							    AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH_GVERB);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  machine_editor_line = ags_functional_test_util_get_machine_editor_dialog_line(machine->machine_editor_dialog,
										nth_pad, nth_audio_channel,
										FALSE);

  plugin_browser = machine_editor_line->line_member_editor->plugin_browser;
  
  ags_functional_test_util_dialog_ok(plugin_browser);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  ags_functional_test_util_dialog_ok(machine->machine_editor_dialog);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*
   * destroy effects
   */  

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* remove effect */
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_machine,
							   AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  nth_pad = 0;
  nth_audio_channel = 0;
  
  ags_functional_test_util_machine_editor_dialog_effect_remove(nth_machine,
							       nth_pad, nth_audio_channel,
							       2);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_effect_remove(nth_machine,
							       nth_pad, nth_audio_channel,
							       0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  ags_functional_test_util_machine_editor_dialog_effect_remove(nth_machine,
							       nth_pad, nth_audio_channel,
							       0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  ags_functional_test_util_dialog_ok(machine->machine_editor_dialog);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* destroy mixer */
  ags_functional_test_util_machine_destroy(nth_machine);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
}

void
ags_functional_line_member_add_and_destroy_test_drum()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditorLine *machine_editor_line;  
  AgsPluginBrowser *plugin_browser;
  
  GList *start_list, *list;

  AgsFunctionalTestUtilListLengthCondition condition;

  guint nth_machine;
  guint nth_pad, nth_audio_channel;
  gboolean success;

  gsequencer_application_context = ags_application_context;
  
  /* add drum */
  ags_functional_test_util_add_machine(NULL,
				       "Drum");
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* get machine */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 1;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_line_member_add_and_destroy_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve drum */
  nth_machine = 0;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_DRUM);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);
  
  /*
   *  add line member to input
   */

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* add ladspa effect */
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_machine,
							   AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  nth_pad = 0;
  nth_audio_channel = 0;
  
  ags_functional_test_util_machine_editor_dialog_effect_add(nth_machine,
							    nth_pad, nth_audio_channel);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* ladspa */
  ags_functional_test_util_machine_editor_dialog_effect_plugin_type(nth_machine,
								    nth_pad, nth_audio_channel,
								    "LADSPA");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* cmt and delay */  
  ags_functional_test_util_machine_editor_dialog_ladspa_filename(nth_machine,
								 nth_pad, nth_audio_channel,
								 AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_ladspa_effect(nth_machine,
							       nth_pad, nth_audio_channel,
							       AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_DELAY);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  machine_editor_line = ags_functional_test_util_get_machine_editor_dialog_line(machine->machine_editor_dialog,
										nth_pad, nth_audio_channel,
										FALSE);

  plugin_browser = machine_editor_line->line_member_editor->plugin_browser;  

  ags_functional_test_util_dialog_ok(plugin_browser);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* add ladspa effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  ags_functional_test_util_machine_editor_dialog_effect_add(nth_machine,
							    nth_pad, nth_audio_channel);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* ladspa */
  ags_functional_test_util_machine_editor_dialog_effect_plugin_type(nth_machine,
								    nth_pad, nth_audio_channel,
								    "LADSPA");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* cmt and freeverb */  
  ags_functional_test_util_machine_editor_dialog_ladspa_filename(nth_machine,
								 nth_pad, nth_audio_channel,
								 AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_ladspa_effect(nth_machine,
							       nth_pad, nth_audio_channel,
							       AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_FREEVERB);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  machine_editor_line = ags_functional_test_util_get_machine_editor_dialog_line(machine->machine_editor_dialog,
										nth_pad, nth_audio_channel,
										FALSE);

  plugin_browser = machine_editor_line->line_member_editor->plugin_browser;
  
  ags_functional_test_util_dialog_ok(plugin_browser);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  ags_functional_test_util_dialog_ok(machine->machine_editor_dialog);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* add lv2 effect */
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_machine,
							   AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  nth_pad = 0;
  nth_audio_channel = 0;
  
  ags_functional_test_util_machine_editor_dialog_effect_add(nth_machine,
							    nth_pad, nth_audio_channel);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* lv2 */
  ags_functional_test_util_machine_editor_dialog_effect_plugin_type(nth_machine,
								    nth_pad, nth_audio_channel,
								    "Lv2");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* lv2 and gverb */  
  ags_functional_test_util_machine_editor_dialog_lv2_filename(nth_machine,
							      nth_pad, nth_audio_channel,
							      AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_lv2_effect(nth_machine,
							    nth_pad, nth_audio_channel,
							    AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH_GVERB);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  machine_editor_line = ags_functional_test_util_get_machine_editor_dialog_line(machine->machine_editor_dialog,
										nth_pad, nth_audio_channel,
										FALSE);

  plugin_browser = machine_editor_line->line_member_editor->plugin_browser;
  
  ags_functional_test_util_dialog_ok(plugin_browser);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  ags_functional_test_util_dialog_ok(machine->machine_editor_dialog);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*
   * destroy effects
   */  

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* remove effect */  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_machine,
							   AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  nth_pad = 0;
  nth_audio_channel = 0;
  
  ags_functional_test_util_machine_editor_dialog_effect_remove(nth_machine,
							       nth_pad, nth_audio_channel,
							       2);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_effect_remove(nth_machine,
							       nth_pad, nth_audio_channel,
							       0);
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  
  ags_functional_test_util_machine_editor_dialog_effect_remove(nth_machine,
							       nth_pad, nth_audio_channel,
							       0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  ags_functional_test_util_dialog_ok(machine->machine_editor_dialog);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* destroy drum */
  ags_functional_test_util_machine_destroy(nth_machine);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
}

void
ags_functional_line_member_add_and_destroy_test_matrix()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;

  GList *start_list, *list;

  AgsFunctionalTestUtilListLengthCondition condition;

  guint nth_machine;
  gboolean success;

  gsequencer_application_context = ags_application_context;
  
  /* add matrix */
  ags_functional_test_util_add_machine(NULL,
				       "Matrix");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 1;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_line_member_add_and_destroy_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve matrix */
  nth_machine = 0;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_MATRIX);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);
  
  //TODO:JK: 
  
  /* destroy matrix */
  ags_functional_test_util_machine_destroy(nth_machine);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
}

void
ags_functional_line_member_add_and_destroy_test_synth()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  
  GList *start_list, *list;

  AgsFunctionalTestUtilListLengthCondition condition;

  guint nth_machine;
  gboolean success;

  gsequencer_application_context = ags_application_context;
  
  /* add synth */
  ags_functional_test_util_add_machine(NULL,
				       "Synth");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 1;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_line_member_add_and_destroy_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve synth */
  nth_machine = 0;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_SYNTH);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);
  
  //TODO:JK: 
  
  /* destroy synth */
  ags_functional_test_util_machine_destroy(nth_machine);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
}

#ifdef AGS_WITH_LIBINSTPATCH
void
ags_functional_line_member_add_and_destroy_test_ffplayer()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditorLine *machine_editor_line;  
  AgsPluginBrowser *plugin_browser;
  
  GList *start_list, *list;

  AgsFunctionalTestUtilListLengthCondition condition;

  guint nth_machine;
  guint nth_pad, nth_audio_channel;
  gboolean success;

  gsequencer_application_context = ags_application_context;
  
  /* add fplayer */
  ags_functional_test_util_add_machine(NULL,
				       "FPlayer");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /*  */
  condition.start_list = &(AGS_WINDOW(gsequencer_application_context->window)->machine);

  condition.length = 1;
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_line_member_add_and_destroy_test_default_timeout,
						      &condition);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);
  
  ags_functional_test_util_sync();

  /* retrieve ffplayer */
  nth_machine = 0;
  
  AGS_FUNCTIONAL_TEST_UTIL_ASSERT_STACK_OBJECT_IS_A_TYPE(0, AGS_TYPE_FFPLAYER);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_hide(nth_machine);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  start_list = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(start_list,
			    nth_machine);
  
  /*
   *  add line member to input
   */

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* add ladspa effect */  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_machine,
							   AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  nth_pad = 0;
  nth_audio_channel = 0;
  
  ags_functional_test_util_machine_editor_dialog_effect_add(nth_machine,
							    nth_pad, nth_audio_channel);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* ladspa */
  ags_functional_test_util_machine_editor_dialog_effect_plugin_type(nth_machine,
								    nth_pad, nth_audio_channel,
								    "LADSPA");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* cmt and delay */  
  ags_functional_test_util_machine_editor_dialog_ladspa_filename(nth_machine,
								 nth_pad, nth_audio_channel,
								 AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_ladspa_effect(nth_machine,
							       nth_pad, nth_audio_channel,
							       AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_DELAY);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  machine_editor_line = ags_functional_test_util_get_machine_editor_dialog_line(machine->machine_editor_dialog,
										nth_pad, nth_audio_channel,
										FALSE);

  plugin_browser = machine_editor_line->line_member_editor->plugin_browser;
  

  ags_functional_test_util_dialog_ok(plugin_browser);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* add ladspa effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  ags_functional_test_util_machine_editor_dialog_effect_add(nth_machine,
							    nth_pad, nth_audio_channel);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* ladspa */
  ags_functional_test_util_machine_editor_dialog_effect_plugin_type(nth_machine,
								    nth_pad, nth_audio_channel,
								    "LADSPA");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* cmt and freeverb */  
  ags_functional_test_util_machine_editor_dialog_ladspa_filename(nth_machine,
								 nth_pad, nth_audio_channel,
								 AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_ladspa_effect(nth_machine,
							       nth_pad, nth_audio_channel,
							       AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LADSPA_CMT_FREEVERB);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* response OK */
  machine_editor_line = ags_functional_test_util_get_machine_editor_dialog_line(machine->machine_editor_dialog,
										nth_pad, nth_audio_channel,
										FALSE);

  plugin_browser = machine_editor_line->line_member_editor->plugin_browser;
  
  ags_functional_test_util_dialog_ok(plugin_browser);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  ags_functional_test_util_dialog_ok(machine->machine_editor_dialog);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* add lv2 effect */
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_machine,
							   AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  nth_pad = 0;
  nth_audio_channel = 0;
  
  ags_functional_test_util_machine_editor_dialog_effect_add(nth_machine,
							    nth_pad, nth_audio_channel);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* lv2 */
  ags_functional_test_util_machine_editor_dialog_effect_plugin_type(nth_machine,
								    nth_pad, nth_audio_channel,
								    "Lv2");

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* lv2 and gverb */  
  ags_functional_test_util_machine_editor_dialog_lv2_filename(nth_machine,
							      nth_pad, nth_audio_channel,
							      AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_lv2_effect(nth_machine,
							    nth_pad, nth_audio_channel,
							    AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_LV2_SWH_GVERB);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  machine_editor_line = ags_functional_test_util_get_machine_editor_dialog_line(machine->machine_editor_dialog,
										nth_pad, nth_audio_channel,
										FALSE);

  plugin_browser = machine_editor_line->line_member_editor->plugin_browser;
  
  ags_functional_test_util_dialog_ok(plugin_browser);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  ags_functional_test_util_dialog_ok(machine->machine_editor_dialog);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* open properties */
  ags_functional_test_util_machine_editor_dialog_open(nth_machine);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  ags_functional_test_util_machine_editor_dialog_click_tab(nth_machine,
							   AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* remove effect */
  nth_pad = 0;
  nth_audio_channel = 0;
  
  ags_functional_test_util_machine_editor_dialog_effect_remove(nth_machine,
							       nth_pad, nth_audio_channel,
							       2);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  ags_functional_test_util_machine_editor_dialog_effect_remove(nth_machine,
							       nth_pad, nth_audio_channel,
							       0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  ags_functional_test_util_machine_editor_dialog_effect_remove(nth_machine,
							       nth_pad, nth_audio_channel,
							       0);

  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();

  /* response OK */
  ags_functional_test_util_dialog_ok(machine->machine_editor_dialog);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

  ags_functional_test_util_sync();
  
  /* destroy fplayer */
  ags_functional_test_util_machine_destroy(nth_machine);
  
  ags_functional_test_util_idle(AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_DEFAULT_IDLE_TIME);

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
  pSuite = CU_add_suite("AgsFuncitonalLineMemberAddAndDestroyTest", ags_functional_line_member_add_and_destroy_test_init_suite, ags_functional_line_member_add_and_destroy_test_clean_suite);
  
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
				  AGS_FUNCTIONAL_LINE_MEMBER_ADD_AND_DESTROY_TEST_CONFIG);
  }
#endif
  
  ags_functional_test_util_do_run(argc, argv,
				  ags_functional_line_member_add_and_destroy_test_add_test, &is_available);

  g_thread_join(ags_functional_test_util_test_runner_thread());

  return(-1);
}
