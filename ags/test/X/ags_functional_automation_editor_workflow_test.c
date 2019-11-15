/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

extern AgsApplicationContext *ags_application_context;

AgsGuiThread *gui_thread;
AgsTaskThread *task_thread;

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
  
  ags_test_quit();
  
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
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  GtkMenu *edit_menu;
  
  GdkEvent *delete_event;

  ags_test_enter();
    
  xorg_application_context = ags_application_context;
  window = xorg_application_context->window;

  edit_menu = window->menu_bar->edit;

  ags_test_leave();

  /* get gui thread */
  gui_thread = ags_thread_find_type(ags_application_context->main_loop,
				    AGS_TYPE_GUI_THREAD);

  /* get task thread */
  task_thread = ags_thread_find_type(ags_application_context->main_loop,
				     AGS_TYPE_TASK_THREAD);

  /* window and editor size */
  ags_functional_test_util_file_default_window_resize();

  ags_functional_test_util_file_default_editor_resize();

  /* open automation window */
  ags_functional_test_util_menu_bar_click(GTK_STOCK_EDIT);
  ags_functional_test_util_menu_click(edit_menu,
				      "Automation");
  
  ags_functional_test_util_file_default_automation_window_resize();
  ags_functional_test_util_file_default_automation_editor_resize();

  /* close automation window */
  ags_test_enter();

  delete_event = gdk_event_new(GDK_DELETE);
  gtk_widget_event((GtkWidget *) window->automation_window,
		   (GdkEvent *) delete_event);
  
  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
  
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
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsAutomationEditor *automation_editor;
  AgsMachine *machine;
  AgsMachineSelector *machine_selector;
  AgsMachineSelection *machine_selection;
  GtkMenu *edit_menu;
  
  GdkEvent *delete_event;
  
  GList *list_start, *list;

  gchar *machine_str;
  
  guint nth_machine;
  gboolean success;

  xorg_application_context = ags_application_context;

  nth_machine = 0;

  /* add panel */
  success = ags_functional_test_util_add_machine(NULL,
						 "Panel");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  ags_test_enter();

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
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
   *  add machine to automation editor
   */

  ags_test_enter();

  window = xorg_application_context->window;
  automation_editor = window->automation_window->automation_editor;
  edit_menu = window->menu_bar->edit;
  
  ags_test_leave();

  /* open automation window */
  ags_functional_test_util_menu_bar_click(GTK_STOCK_EDIT);
  ags_functional_test_util_menu_click(edit_menu,
				      "Automation");
  
  /* add index and link */
  success = ags_functional_test_util_machine_selection_add_index("AgsAutomationEditor");

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selector_select("AgsAutomationEditor",
							     nth_machine);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selection_link_index("AgsAutomationEditor");

  CU_ASSERT(success == TRUE);

  /* select machine */
  ags_test_enter();

  machine_selector = automation_editor->machine_selector;
  machine_selection = machine_selector->machine_selection;

  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_test_leave();

  success = ags_functional_test_util_machine_selection_select("AgsAutomationEditor",
							      machine_str);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_dialog_ok(machine_selection);

  CU_ASSERT(success == TRUE);

  /* close automation window */
  ags_test_enter();

  delete_event = gdk_event_new(GDK_DELETE);
  gtk_widget_event((GtkWidget *) window->automation_window,
		   (GdkEvent *) delete_event);
  
  ags_test_leave();
}

void
ags_functional_automation_editor_workflow_test_mixer()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsAutomationEditor *automation_editor;
  AgsMachine *machine;
  AgsMachineSelector *machine_selector;
  AgsMachineSelection *machine_selection;
  GtkMenu *edit_menu;
  
  GdkEvent *delete_event;
  
  GList *list_start, *list;

  gchar *machine_str;
  
  guint nth_machine;
  gboolean success;

  xorg_application_context = ags_application_context;

  nth_machine = 1;

  /* add mixer */
  success = ags_functional_test_util_add_machine(NULL,
						 "Mixer");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  ags_test_enter();

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
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
   *  add machine to automation editor
   */

  ags_test_enter();

  window = xorg_application_context->window;
  automation_editor = window->automation_window->automation_editor;
  edit_menu = window->menu_bar->edit;
  
  ags_test_leave();

  /* open automation window */
  ags_functional_test_util_menu_bar_click(GTK_STOCK_EDIT);
  ags_functional_test_util_menu_click(edit_menu,
				      "Automation");
  
  /* add index and link */
  success = ags_functional_test_util_machine_selection_add_index("AgsAutomationEditor");

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selector_select("AgsAutomationEditor",
							     nth_machine);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selection_link_index("AgsAutomationEditor");

  CU_ASSERT(success == TRUE);

  /* select machine */
  ags_test_enter();

  machine_selector = automation_editor->machine_selector;
  machine_selection = machine_selector->machine_selection;

  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_test_leave();

  success = ags_functional_test_util_machine_selection_select("AgsAutomationEditor",
							      machine_str);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_dialog_ok(machine_selection);

  CU_ASSERT(success == TRUE);

  /* close automation window */
  ags_test_enter();

  delete_event = gdk_event_new(GDK_DELETE);
  gtk_widget_event((GtkWidget *) window->automation_window,
		   (GdkEvent *) delete_event);
  
  ags_test_leave();
}

void
ags_functional_automation_editor_workflow_test_equalizer10()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsAutomationEditor *automation_editor;
  AgsMachine *machine;
  AgsMachineSelector *machine_selector;
  AgsMachineSelection *machine_selection;
  GtkMenu *edit_menu;
  
  GdkEvent *delete_event;
  
  GList *list_start, *list;

  gchar *machine_str;
  
  guint nth_machine;
  gboolean success;

  xorg_application_context = ags_application_context;

  nth_machine = 2;

  /* add equalizer10 */
  success = ags_functional_test_util_add_machine(NULL,
						 "Equalizer");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  ags_test_enter();

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();

  if(list != NULL &&
     AGS_IS_EQUALIZER10(list->data)){
    machine = list->data;
  }else{
    machine = NULL;
  }
  
  CU_ASSERT(machine != NULL);

  /*
   *  add machine to automation editor
   */

  ags_test_enter();

  window = xorg_application_context->window;
  automation_editor = window->automation_window->automation_editor;
  edit_menu = window->menu_bar->edit;
  
  ags_test_leave();

  /* open automation window */
  ags_functional_test_util_menu_bar_click(GTK_STOCK_EDIT);
  ags_functional_test_util_menu_click(edit_menu,
				      "Automation");
  
  /* add index and link */
  success = ags_functional_test_util_machine_selection_add_index("AgsAutomationEditor");

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selector_select("AgsAutomationEditor",
							     nth_machine);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selection_link_index("AgsAutomationEditor");

  CU_ASSERT(success == TRUE);

  /* select machine */
  ags_test_enter();

  machine_selector = automation_editor->machine_selector;
  machine_selection = machine_selector->machine_selection;

  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_test_leave();

  success = ags_functional_test_util_machine_selection_select("AgsAutomationEditor",
							      machine_str);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_dialog_ok(machine_selection);

  CU_ASSERT(success == TRUE);

  /* close automation window */
  ags_test_enter();

  delete_event = gdk_event_new(GDK_DELETE);
  gtk_widget_event((GtkWidget *) window->automation_window,
		   (GdkEvent *) delete_event);
  
  ags_test_leave();
}

void
ags_functional_automation_editor_workflow_test_drum()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsAutomationEditor *automation_editor;
  AgsMachine *machine;
  AgsMachineSelector *machine_selector;
  AgsMachineSelection *machine_selection;
  GtkMenu *edit_menu;

  GdkEvent *delete_event;
  
  GList *list_start, *list;

  gchar *machine_str;
  
  guint nth_machine;
  gboolean success;

  xorg_application_context = ags_application_context;

  nth_machine = 3;

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
   *  add machine to automation editor
   */

  ags_test_enter();

  window = xorg_application_context->window;
  automation_editor = window->automation_window->automation_editor;
  edit_menu = window->menu_bar->edit;
  
  ags_test_leave();

  /* open automation window */
  ags_functional_test_util_menu_bar_click(GTK_STOCK_EDIT);
  ags_functional_test_util_menu_click(edit_menu,
				      "Automation");
  
  /* add index and link */
  success = ags_functional_test_util_machine_selection_add_index("AgsAutomationEditor");

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selector_select("AgsAutomationEditor",
							     nth_machine);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selection_link_index("AgsAutomationEditor");

  CU_ASSERT(success == TRUE);

  /* select machine */
  ags_test_enter();

  machine_selector = automation_editor->machine_selector;
  machine_selection = machine_selector->machine_selection;

  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_test_leave();

  success = ags_functional_test_util_machine_selection_select("AgsAutomationEditor",
							      machine_str);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_dialog_ok(machine_selection);

  CU_ASSERT(success == TRUE);

  /* close automation window */
  ags_test_enter();

  delete_event = gdk_event_new(GDK_DELETE);
  gtk_widget_event((GtkWidget *) window->automation_window,
		   (GdkEvent *) delete_event);
  
  ags_test_leave();
}

void
ags_functional_automation_editor_workflow_test_matrix()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsAutomationEditor *automation_editor;
  AgsMachine *machine;
  AgsMachineSelector *machine_selector;
  AgsMachineSelection *machine_selection;
  GtkMenu *edit_menu;

  GdkEvent *delete_event;
  
  GList *list_start, *list;

  gchar *machine_str;
  
  guint nth_machine;
  gboolean success;

  xorg_application_context = ags_application_context;

  nth_machine = 4;

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
   *  add machine to automation editor
   */

  ags_test_enter();

  window = xorg_application_context->window;
  automation_editor = window->automation_window->automation_editor;
  edit_menu = window->menu_bar->edit;
  
  ags_test_leave();

  /* open automation window */
  ags_functional_test_util_menu_bar_click(GTK_STOCK_EDIT);
  ags_functional_test_util_menu_click(edit_menu,
				      "Automation");
  
  /* add index and link */
  success = ags_functional_test_util_machine_selection_add_index("AgsAutomationEditor");

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selector_select("AgsAutomationEditor",
							     nth_machine);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selection_link_index("AgsAutomationEditor");

  CU_ASSERT(success == TRUE);

  /* select machine */
  ags_test_enter();

  machine_selector = automation_editor->machine_selector;
  machine_selection = machine_selector->machine_selection;

  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_test_leave();

  success = ags_functional_test_util_machine_selection_select("AgsAutomationEditor",
							      machine_str);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_dialog_ok(machine_selection);

  CU_ASSERT(success == TRUE);

  /* close automation window */
  ags_test_enter();

  delete_event = gdk_event_new(GDK_DELETE);
  gtk_widget_event((GtkWidget *) window->automation_window,
		   (GdkEvent *) delete_event);
  
  ags_test_leave();
}

void
ags_functional_automation_editor_workflow_test_syncsynth()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsAutomationEditor *automation_editor;
  AgsMachine *machine;
  AgsMachineSelector *machine_selector;
  AgsMachineSelection *machine_selection;
  GtkMenu *edit_menu;

  GdkEvent *delete_event;
  
  GList *list_start, *list;

  gchar *machine_str;
  
  guint nth_machine;
  gboolean success;

  xorg_application_context = ags_application_context;

  nth_machine = 5;

  /* add syncsynth */
  success = ags_functional_test_util_add_machine(NULL,
						 "Syncsynth");

  CU_ASSERT(success == TRUE);

  ags_functional_test_util_idle();

  /* get machine */
  ags_test_enter();

  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();

  if(list != NULL &&
     AGS_IS_SYNCSYNTH(list->data)){
    machine = list->data;
  }else{
    machine = NULL;
  }
  
  CU_ASSERT(machine != NULL);

  /*
   *  add machine to automation editor
   */

  ags_test_enter();

  window = xorg_application_context->window;
  automation_editor = window->automation_window->automation_editor;
  edit_menu = window->menu_bar->edit;
  
  ags_test_leave();

  /* open automation window */
  ags_functional_test_util_menu_bar_click(GTK_STOCK_EDIT);
  ags_functional_test_util_menu_click(edit_menu,
				      "Automation");
  
  /* add index and link */
  success = ags_functional_test_util_machine_selection_add_index("AgsAutomationEditor");

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selector_select("AgsAutomationEditor",
							     nth_machine);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selection_link_index("AgsAutomationEditor");

  CU_ASSERT(success == TRUE);

  /* select machine */
  ags_test_enter();

  machine_selector = automation_editor->machine_selector;
  machine_selection = machine_selector->machine_selection;

  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_test_leave();

  success = ags_functional_test_util_machine_selection_select("AgsAutomationEditor",
							      machine_str);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_dialog_ok(machine_selection);

  CU_ASSERT(success == TRUE);

  /* close automation window */
  ags_test_enter();

  delete_event = gdk_event_new(GDK_DELETE);
  gtk_widget_event((GtkWidget *) window->automation_window,
		   (GdkEvent *) delete_event);
  
  ags_test_leave();
}

#ifdef AGS_WITH_LIBINSTPATCH
void
ags_functional_automation_editor_workflow_test_ffplayer()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsAutomationEditor *automation_editor;
  AgsMachine *machine;
  AgsMachineSelector *machine_selector;
  AgsMachineSelection *machine_selection;
  GtkMenu *edit_menu;

  GdkEvent *delete_event;
  
  GList *list_start, *list;

  gchar *machine_str;
  
  guint nth_machine;
  gboolean success;

  xorg_application_context = ags_application_context;

  nth_machine = 6;

  /* add ffplayer */
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
   *  add machine to automation editor
   */

  ags_test_enter();

  window = xorg_application_context->window;
  automation_editor = window->automation_window->automation_editor;
  edit_menu = window->menu_bar->edit;
  
  ags_test_leave();

  /* open automation window */
  ags_functional_test_util_menu_bar_click(GTK_STOCK_EDIT);
  ags_functional_test_util_menu_click(edit_menu,
				      "Automation");
  
  /* add index and link */
  success = ags_functional_test_util_machine_selection_add_index("AgsAutomationEditor");

  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selector_select("AgsAutomationEditor",
							     nth_machine);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_machine_selection_link_index("AgsAutomationEditor");

  CU_ASSERT(success == TRUE);

  /* select machine */
  ags_test_enter();

  machine_selector = automation_editor->machine_selector;
  machine_selection = machine_selector->machine_selection;

  machine_str = g_strdup_printf("%s: %s",
				G_OBJECT_TYPE_NAME(machine),
				machine->machine_name);
  
  ags_test_leave();

  success = ags_functional_test_util_machine_selection_select("AgsAutomationEditor",
							      machine_str);
  
  CU_ASSERT(success == TRUE);

  success = ags_functional_test_util_dialog_ok(machine_selection);

  CU_ASSERT(success == TRUE);

  /* close automation window */
  ags_test_enter();

  delete_event = gdk_event_new(GDK_DELETE);
  gtk_widget_event((GtkWidget *) window->automation_window,
		   (GdkEvent *) delete_event);
  
  ags_test_leave();
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
  pSuite = CU_add_suite("AgsFunctionalAutomationEditorWorkflowTest", ags_functional_automation_editor_workflow_test_init_suite, ags_functional_automation_editor_workflow_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  g_atomic_int_set(&is_available,
		   FALSE);
  
  ags_test_init(&argc, &argv,
		AGS_FUNCTIONAL_AUTOMATION_EDITOR_WORKFLOW_TEST_CONFIG);
  ags_functional_test_util_do_run(argc, argv,
				  ags_functional_automation_editor_workflow_test_add_test, &is_available);

  g_thread_join(ags_functional_test_util_self());
  
  return(-1);
}
