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

#include <ags/test/X/ags_functional_test_util.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/gsequencer_main.h>

#include <ags/test/X/gsequencer_setup_util.h>
#include <ags/test/X/libgsequencer.h>

#include <gdk/gdk.h>
#include <gdk/gdkevents.h>

#include <pthread.h>

#define AGS_FUNCTIONAL_TEST_UTIL_REACTION_TIME (125000)
#define AGS_FUNCTIONAL_TEST_UTIL_REACTION_TIME_LONG (500000)

gboolean ags_functional_test_util_driver_prepare(GSource *source,
						 gint *timeout_);
gboolean ags_functional_test_util_driver_check(GSource *source);
gboolean ags_functional_test_util_driver_dispatch(GSource *source,
						  GSourceFunc callback,
						  gpointer user_data);


void* ags_functional_test_util_add_test_thread(void *ptr);
void* ags_functional_test_util_do_run_thread(void *ptr);

extern AgsApplicationContext *ags_application_context;

extern AgsMutexManager *ags_mutex_manager;

extern AgsLadspaManager *ags_ladspa_manager;
extern AgsDssiManager *ags_dssi_manager;
extern AgsLv2Manager *ags_lv2_manager;
extern AgsLv2uiManager *ags_lv2ui_manager;

AgsTaskThread *task_thread;
AgsGuiThread *gui_thread;

struct _AddTest{
  AgsFunctionalTestUtilAddTest add_test;
  volatile gboolean *is_available;
};

struct timespec ags_functional_test_util_default_timeout = {
  20,
  0,
};


gboolean
ags_functional_test_util_driver_prepare(GSource *source,
					gint *timeout_)
{
  if(timeout_ != NULL){
    *timeout_ = -1;
  }
  
  return(TRUE);
}

gboolean
ags_functional_test_util_driver_check(GSource *source)
{
  return(TRUE);
}

gboolean
ags_functional_test_util_driver_dispatch(GSource *source,
					 GSourceFunc callback,
					 gpointer user_data)
{
  g_main_context_iteration(g_main_context_default(),
			   FALSE);

  pthread_mutex_unlock(ags_test_get_driver_mutex());
  
  usleep(4000);
  
  pthread_mutex_lock(ags_test_get_driver_mutex());

  g_main_context_iteration(g_main_context_default(),
			   FALSE);
  
  return(G_SOURCE_CONTINUE);
}

void*
ags_functional_test_util_add_test_thread(void *ptr)
{
  struct _AddTest *test;

  test = ptr;
  
  while(!g_atomic_int_get(test->is_available)){
    usleep(12500);
  }

  test->add_test();
  
  pthread_exit(NULL);
}

void
ags_functional_test_util_add_test(AgsFunctionalTestUtilAddTest add_test,
				  volatile gboolean *is_available)
{
  struct _AddTest *test;

  pthread_t thread;

  test = (struct _AddTest *) malloc(sizeof(struct _AddTest));

  test->add_test = add_test;
  test->is_available = is_available;

  pthread_create(&thread, NULL,
		 ags_functional_test_util_add_test_thread, test);
}

void
ags_functional_test_util_notify_add_test(volatile gboolean *is_available)
{
  g_atomic_int_set(is_available,
		   TRUE);
}

void*
ags_functional_test_util_do_run_thread(void *ptr)
{
  AgsXorgApplicationContext *xorg_application_context;
  
  volatile gboolean *is_available;
  
  xorg_application_context = (AgsXorgApplicationContext *) ags_application_context_get_instance();

  is_available = ptr;

  while(g_atomic_int_get(&(xorg_application_context->gui_ready)) == 0){
    usleep(500000);
  }
  
  ags_xorg_application_context_setup(xorg_application_context);
  
  //  g_atomic_int_set(&(xorg_application_context->gui_ready),
  //		   FALSE);

  /*  */
  while(g_atomic_int_get(&(xorg_application_context->show_animation))){
    usleep(500000);
  }

  usleep(1000000);

  /* get gui thread */
  gui_thread = ags_thread_find_type(ags_application_context->main_loop,
				    AGS_TYPE_GUI_THREAD);

  task_thread = ags_thread_find_type(ags_application_context->main_loop,
				     AGS_TYPE_TASK_THREAD);
  
  ags_functional_test_util_notify_add_test(is_available);
  
  pthread_exit(NULL);
}

void
ags_functional_test_util_do_run(int argc, char **argv,
				AgsFunctionalTestUtilAddTest add_test, volatile gboolean *is_available)
{
  AgsApplicationContext *application_context;
  AgsLog *log;

  pthread_t thread;
  pthread_mutex_t *mutex;
  
  GSource *driver_source;
  GSourceFuncs driver_funcs;

  /* application context */
  application_context = 
    ags_application_context = (AgsApplicationContext *) ags_xorg_application_context_new();
  g_object_ref(application_context);
  
  application_context->argc = argc;
  application_context->argv = argv;

  log = ags_log_get_instance();

  ags_log_add_message(log,
		      "Welcome to Advanced Gtk+ Sequencer - Test");

  ags_functional_test_util_add_test(add_test, is_available);
  
  /* application context */
  mutex = ags_test_get_driver_mutex();
  pthread_mutex_lock(mutex);

  pthread_create(&thread, NULL,
		 ags_functional_test_util_do_run_thread, is_available);

  driver_funcs.prepare = ags_functional_test_util_driver_prepare;
  driver_funcs.check = ags_functional_test_util_driver_check;
  driver_funcs.dispatch = ags_functional_test_util_driver_dispatch;
  driver_funcs.finalize = NULL;

  driver_source = g_source_new(&driver_funcs,
			       sizeof(GSource));
  g_source_attach(driver_source,
  		  g_main_context_default());
  
  ags_application_context_prepare(application_context);
}

void
ags_functional_test_util_clear_application_context()
{  
  ags_application_context = NULL;

  //  ags_mutex_manager = NULL;

  //  ags_ladspa_manager = NULL;
  //  ags_dssi_manager = NULL;
  //  ags_lv2_manager = NULL;
  //  ags_lv2ui_manager = NULL;
}

void
ags_functional_test_util_setup_and_launch()
{
  gchar *start_arg[] = {
    "./gsequencer\0"
  };

  gboolean success;

  ags_functional_test_util_clear_application_context();
  
  ags_test_setup(1, start_arg);

  /* get gui thread */
  gui_thread = ags_thread_find_type(ags_application_context->main_loop,
				    AGS_TYPE_GUI_THREAD);

  task_thread = ags_thread_find_type(ags_application_context->main_loop,
				     AGS_TYPE_TASK_THREAD);

  /* launch application */
  ags_test_launch(FALSE);

  /* do the work */
  while(g_atomic_int_get(&(AGS_XORG_APPLICATION_CONTEXT(ags_application_context)->gui_ready)) == 0){
    usleep(500000);
  }

  usleep(10000000);
}

void
ags_functional_test_util_setup_and_launch_filename(gchar *filename)
{
  gchar *start_arg[] = {
    "./gsequencer\0",
    "--filename\0",
    filename,
  };

  gboolean success;

  ags_functional_test_util_clear_application_context();
  
  ags_test_setup(3, start_arg);

  /* get gui thread */
  gui_thread = ags_thread_find_type(ags_application_context->main_loop,
				    AGS_TYPE_GUI_THREAD);

  task_thread = ags_thread_find_type(ags_application_context->main_loop,
				     AGS_TYPE_TASK_THREAD);

  /* launch application */
  ags_test_launch_filename(filename,
			   FALSE);

  /* do the work */
  while(g_atomic_int_get(&(AGS_XORG_APPLICATION_CONTEXT(ags_application_context)->gui_ready)) == 0){
    usleep(500000);
  }

  usleep(10000000);
}

void
ags_functional_test_util_reaction_time()
{
  usleep(AGS_FUNCTIONAL_TEST_UTIL_REACTION_TIME);
}

void
ags_functional_test_util_reaction_time_long()
{
  usleep(AGS_FUNCTIONAL_TEST_UTIL_REACTION_TIME_LONG);
}

void
ags_functional_test_util_idle()
{
  usleep(5000000);
}

void
ags_functional_test_util_idle_condition_and_timeout(AgsFunctionalTestUtilIdleCondition idle_condition,
						    struct timespec *timeout,
						    gpointer data)
{
  struct timespec start_time, current_time;

  clock_gettime(CLOCK_MONOTONIC,
		&start_time);
  
  while(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(idle_condition)(data)){
    ags_functional_test_util_reaction_time();

    clock_gettime(CLOCK_MONOTONIC,
		  &current_time);

    if(start_time.tv_sec + timeout->tv_sec < current_time.tv_sec){
      break;
    }
  }

  ags_functional_test_util_reaction_time();
}

gboolean
ags_functional_test_util_idle_test_widget_visible(GtkWidget **widget)
{
  gboolean do_idle;

  do_idle = TRUE;
  
  ags_test_enter();
  
  if(*widget != NULL &&
     GTK_IS_WIDGET(*widget) &&
     GTK_WIDGET_VISIBLE(*widget)){
    do_idle = FALSE;
  }

  ags_test_leave();
  
  return(do_idle);
}

gboolean
ags_functional_test_util_idle_test_widget_hidden(GtkWidget **widget)
{
  gboolean do_idle;

  do_idle = TRUE;
  
  ags_test_enter();
  
  if(*widget != NULL &&
     GTK_IS_WIDGET(*widget) &&
     !GTK_WIDGET_VISIBLE(*widget)){
    do_idle = FALSE;
  }

  ags_test_leave();
  
  return(do_idle);
}

gboolean
ags_functional_test_util_idle_test_null(GtkWidget **widget)
{
  gboolean do_idle;

  do_idle = TRUE;
  
  ags_test_enter();
  
  if(*widget == NULL){
    do_idle = FALSE;
  }

  ags_test_leave();
  
  return(do_idle);
}

gboolean
ags_functional_test_util_idle_test_container_children_count(AgsFunctionalTestUtilContainerTest *container_test)
{
  gboolean do_idle;

  do_idle = TRUE;
  
  ags_test_enter();

  if(*(container_test->container) != NULL &&
     GTK_IS_CONTAINER(*(container_test->container))){
    GList *list;

    list = gtk_container_get_children(*(container_test->container));
    
    if(g_list_length(list) == container_test->count){
      do_idle = FALSE;
    }

    g_list_free(list);
  }

  ags_test_leave();
  
  return(do_idle);
}

void
ags_functional_test_util_leave(GtkWidget *window)
{
  if(!GTK_IS_WINDOW(window)){
    return;
  }

  ags_test_enter();

  gdk_window_destroy(gtk_widget_get_window(window));

  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
}

GtkMenu*
ags_functional_test_util_submenu_find(GtkMenu *menu,
				      gchar *item_label)
{
  GtkMenu *submenu;

  GList *list_start, *list;
  
  gchar *str;
  
  if(!GTK_IS_MENU(menu) ||
     item_label == NULL){
    return(NULL);
  }

  ags_test_enter();

  list =
    list_start = gtk_container_get_children(menu);
  submenu = NULL;
  
  while(list != NULL){
    if(GTK_IS_MENU_ITEM(list->data)){
      str = NULL;
      g_object_get(list->data,
		   "label\0", &str,
		   NULL);

      if(!g_ascii_strcasecmp(str,
			     item_label)){
	g_object_get(list->data,
		     "submenu\0", &submenu,
		     NULL);

	break;
      }
    }

    list = list->next;
  }

  g_list_free(list_start);

  ags_test_leave();
  
  return(submenu);
}

gboolean
ags_functional_test_util_menu_bar_click(gchar *item_label)
{
  AgsXorgApplicationContext *xorg_application_context;
  GtkMenuBar *menu_bar;
  
  GList *list_start, *list;
  
  gchar *str;

  gboolean success;
  
  if(item_label == NULL){
    return(FALSE);
  }

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();
  menu_bar = xorg_application_context->window->menu_bar;

  list =
    list_start = gtk_container_get_children(menu_bar);
  success = FALSE;

  ags_test_leave();
  
  while(list != NULL){
    if(GTK_IS_MENU_ITEM(list->data)){
      str = NULL;
      g_object_get(list->data,
		   "label\0", &str,
		   NULL);

      if(!g_ascii_strcasecmp(str,
			     item_label)){
	GtkWidget *widget;

	GdkWindow *window;
	GdkEvent *event_motion;

	struct timespec spec;
	
	gint x, y;
	gint origin_x, origin_y;
	
	widget = GTK_WIDGET(list->data);

	if(!GTK_WIDGET_REALIZED(widget)){
	  ags_functional_test_util_reaction_time_long();
	}

	/*  */
	ags_test_enter();

	x = widget->allocation.x + widget->allocation.width / 2.0;
	y = widget->allocation.y + widget->allocation.height / 2.0;

	window = gtk_widget_get_window(widget);

	x = widget->allocation.x;
	y = widget->allocation.y;

	gdk_window_get_origin(window, &origin_x, &origin_y);

	gdk_display_warp_pointer(gtk_widget_get_display(widget),
				 gtk_widget_get_screen(widget),
				 origin_x + x + 15, origin_y + y + 5);
	
	ags_test_leave();

	/*  */
	ags_functional_test_util_reaction_time();

	gdk_test_simulate_button(window,
				 x + 5,
				 y + 5,
				 1,
				 GDK_BUTTON1_MASK,
				 GDK_BUTTON_PRESS);

	ags_functional_test_util_reaction_time();

	gdk_test_simulate_button(window,
				 x + 5,
				 y + 5,
				 1,
				 GDK_BUTTON1_MASK,
				 GDK_BUTTON_RELEASE);

	ags_functional_test_util_reaction_time();

	/*  */
	ags_test_enter();

	g_signal_emit_by_name(widget,
			      "activate-item\0");
	
	ags_test_leave();

	success = TRUE;

	break;
      }
    }

    list = list->next;
  }

  g_list_free(list_start);

  ags_functional_test_util_reaction_time_long();
  
  return(success);
}

gboolean
ags_functional_test_util_menu_click(GtkMenu *menu,
				    gchar *item_label)
{
  GList *list_start, *list;
  
  gchar *str;

  gboolean success;
  
  if(menu == NULL ||
     item_label == NULL ||
     !GTK_IS_MENU(menu)){
    return(FALSE);
  }

  ags_test_enter();

  list =
    list_start = gtk_container_get_children(menu);
  success = FALSE;

  ags_test_leave();
  
  while(list != NULL){
    if(GTK_IS_MENU_ITEM(list->data)){
      str = NULL;
      g_object_get(list->data,
		   "label\0", &str,
		   NULL);

      if(!g_ascii_strcasecmp(str,
			     item_label)){
	GtkWidget *widget;

	GdkWindow *window;
	
	gint x, y;
	gint origin_x, origin_y;
	
	widget = GTK_WIDGET(list->data);

	if(!GTK_WIDGET_REALIZED(widget)){
	  ags_functional_test_util_reaction_time_long();
	}

	/*  */
	ags_test_enter();

	window = gtk_widget_get_window(widget);

	x = widget->allocation.x + widget->allocation.width / 2.0;
	y = widget->allocation.y + widget->allocation.height / 2.0;

	gdk_window_get_origin(window, &origin_x, &origin_y);

	gdk_display_warp_pointer(gtk_widget_get_display(widget),
				 gtk_widget_get_screen(widget),
				 origin_x + x + 15, origin_y + y + 5);

	ags_test_leave();

	ags_functional_test_util_reaction_time();
	
	gdk_test_simulate_button(window,
				 x + 5,
				 y + 5,
				 1,
				 GDK_BUTTON1_MASK,
				 GDK_BUTTON_PRESS);


	ags_functional_test_util_reaction_time();

	gdk_test_simulate_button(window,
				 x + 5,
				 y + 5,
				 1,
				 GDK_BUTTON1_MASK,
				 GDK_BUTTON_RELEASE);
  	
	ags_functional_test_util_reaction_time();

	success = TRUE;

	/*  */
	ags_test_enter();

	g_signal_emit_by_name(widget,
			      "activate-item\0");
	
	ags_test_leave();

	break;
      }
    }

    list = list->next;
  }

  g_list_free(list_start);

  ags_functional_test_util_reaction_time_long();
  
  return(success);
}

gboolean
ags_functional_test_util_combo_box_click(GtkComboBox *combo_box,
					 guint nth)
{
  GtkWidget *widget;

  GdkWindow *window;
  
  gint x, y;
  gint origin_x, origin_y;	
  
  if(combo_box == NULL ||
     !GTK_IS_COMBO_BOX(combo_box)){
    return(FALSE);
  }

  widget = combo_box;

  if(!GTK_WIDGET_REALIZED(widget)){
    ags_functional_test_util_reaction_time_long();
  }
  
  /*  */
  ags_test_enter();

  window = gtk_widget_get_window(widget);

  x = widget->allocation.x + widget->allocation.width / 2.0;
  y = widget->allocation.y + widget->allocation.height / 2.0;

  gdk_window_get_origin(window, &origin_x, &origin_y);

  gdk_display_warp_pointer(gtk_widget_get_display(widget),
			   gtk_widget_get_screen(widget),
			   origin_x + x + 15, origin_y + y + 5);

  ags_test_leave();

  /*
  ags_functional_test_util_reaction_time();
	
  gdk_test_simulate_button(window,
			   x + 5,
			   y + 5,
			   1,
			   GDK_BUTTON1_MASK,
			   GDK_BUTTON_PRESS);


  ags_functional_test_util_reaction_time();

  gdk_test_simulate_button(window,
			   x + 5,
			   y + 5,
			   1,
			   GDK_BUTTON1_MASK,
			   GDK_BUTTON_RELEASE);
  	
  ags_functional_test_util_reaction_time();
  */
  
  /*  */
  ags_test_enter();

  gtk_combo_box_set_active(combo_box,
			   nth);
	
  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
  
  return(TRUE);
}

gboolean
ags_functional_test_util_button_click(GtkButton *button)
{
  GtkWidget *widget;

  GdkWindow *window;

  gint x, y;
  gint origin_x, origin_y;
	
  if(button == NULL ||
     !GTK_IS_BUTTON(button)){
    return(FALSE);
  }
  
  widget = button;

  if(!GTK_WIDGET_REALIZED(widget)){
    ags_functional_test_util_reaction_time_long();
  }
  
  /*  */
  ags_test_enter();

  window = gtk_widget_get_window(widget);

  x = widget->allocation.x + widget->allocation.width / 2.0;
  y = widget->allocation.y + widget->allocation.height / 2.0;

  gdk_window_get_origin(window, &origin_x, &origin_y);

  gdk_display_warp_pointer(gtk_widget_get_display(widget),
			   gtk_widget_get_screen(widget),
			   origin_x + x + 15, origin_y + y + 5);

  ags_test_leave();

  /*  */
  ags_functional_test_util_reaction_time();
	
  gdk_test_simulate_button(window,
			   x + 5,
			   y + 5,
			   1,
			   GDK_BUTTON1_MASK,
			   GDK_BUTTON_PRESS);


  ags_functional_test_util_reaction_time();

  gdk_test_simulate_button(window,
			   x + 5,
			   y + 5,
			   1,
			   GDK_BUTTON1_MASK,
			   GDK_BUTTON_RELEASE);
  	
  ags_functional_test_util_reaction_time();

  ags_functional_test_util_reaction_time_long();
  
  return(TRUE);
}

gboolean
ags_functional_test_util_menu_tool_button_click(GtkButton *button)
{
  GtkWidget *widget;
  GtkWidget *arrow_button = NULL;
  
  GdkWindow *window;

  gint x, y;
  gint origin_x, origin_y;
	
  if(button == NULL ||
     !GTK_IS_MENU_TOOL_BUTTON(button)){
    return(FALSE);
  }
  
  widget = button;

  if(arrow_button == NULL){
    arrow_button = gtk_container_get_children(gtk_bin_get_child(button))->next->data;
  }
  
  if(!GTK_WIDGET_REALIZED(widget)){
    ags_functional_test_util_reaction_time_long();
  }
  
  ags_test_enter();

  window = gtk_widget_get_window(widget);

  x = widget->allocation.x + widget->allocation.width - (arrow_button->allocation.width / 2.0);
  y = widget->allocation.y + widget->allocation.height - (arrow_button->allocation.height / 2.0);

  gdk_window_get_origin(window, &origin_x, &origin_y);

  gdk_display_warp_pointer(gtk_widget_get_display(widget),
			   gtk_widget_get_screen(widget),
			   origin_x + x, origin_y + y);

  ags_test_leave();

  ags_functional_test_util_reaction_time();
	
  gdk_test_simulate_button(window,
			   x + 5,
			   y + 5,
			   1,
			   GDK_BUTTON1_MASK,
			   GDK_BUTTON_PRESS);


  ags_functional_test_util_reaction_time();

  gdk_test_simulate_button(window,
			   x + 5,
			   y + 5,
			   1,
			   GDK_BUTTON1_MASK,
			   GDK_BUTTON_RELEASE);
  	
  ags_functional_test_util_reaction_time_long();

#if 0
  /*  */
  ags_test_enter();

  g_signal_emit_by_name(widget,
			"clicked\0");

  ags_test_leave();

  ags_functional_test_util_idle();

  /*  */
  ags_test_enter();

  gtk_menu_popup(gtk_menu_tool_button_get_menu(widget),
		 NULL,
		 NULL,
		 NULL,
		 widget,
		 1,
		 gtk_get_current_event_time());
  
  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
#endif
  
  return(TRUE);
}

gboolean
ags_functional_test_util_dialog_apply(GtkDialog *dialog)
{
  if(!GTK_IS_DIALOG(dialog)){
    return(FALSE);
  }

  ags_test_enter();

  gtk_dialog_response(dialog,
		      GTK_RESPONSE_APPLY);

  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
  
  return(TRUE);
}

gboolean
ags_functional_test_util_dialog_ok(GtkDialog *dialog)
{
  GtkButton *ok_button;

  GList *list_start, *list;

  gboolean success;
  
  if(!GTK_IS_DIALOG(dialog)){
    return(FALSE);
  }

  ags_test_enter();

  ok_button = NULL;
  
  list_start = 
    list = gtk_container_get_children(gtk_dialog_get_action_area(dialog));

  while(list != NULL){
    if(!g_strcmp0(GTK_STOCK_OK, gtk_button_get_label(list->data))){
      ok_button = list->data;
      
      break;
    }

    list = list->next;
  }

  ags_test_leave();

  g_list_free(list_start);

  if(ok_button == NULL){
    return(FALSE);
  }
  
  success = ags_functional_test_util_button_click(ok_button);

  ags_functional_test_util_reaction_time_long();
    
  return(success);
}

gboolean
ags_functional_test_util_dialog_cancel(GtkDialog *dialog)
{
  GtkButton *cancel_button;

  GList *list_start, *list;

  gboolean success;
  
  if(!GTK_IS_DIALOG(dialog)){
    return(FALSE);
  }

  ags_test_enter();

  cancel_button = NULL;
  
  list_start = 
    list = gtk_container_get_children(gtk_dialog_get_action_area(dialog));

  while(list != NULL){
    if(!g_strcmp0(GTK_STOCK_CANCEL, gtk_button_get_label(list->data))){
      cancel_button = list->data;
      
      break;
    }

    list = list->next;
  }

  ags_test_leave();

  g_list_free(list_start);

  if(cancel_button == NULL){
    return(FALSE);
  }
  
  success = ags_functional_test_util_button_click(cancel_button);

  ags_functional_test_util_reaction_time_long();
    
  return(success);
}

gboolean
ags_functional_test_util_file_chooser_open_path(GtkFileChooser *file_chooser,
						gchar *path)
{
  if(file_chooser == NULL ||
     path == NULL ||
     !GTK_IS_FILE_CHOOSER(file_chooser)){
    return(FALSE);
  }
  
  ags_test_enter();

  gtk_file_chooser_set_current_folder(file_chooser,
				      path);

  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
  
  return(TRUE);
}

gboolean
ags_functional_test_util_file_chooser_select_filename(GtkFileChooser *file_chooser,
						      gchar *filename)
{
  if(file_chooser == NULL ||
     filename == NULL ||
     !GTK_IS_FILE_CHOOSER(file_chooser)){
    return(FALSE);
  }
  
  ags_test_enter();

  gtk_file_chooser_select_filename(file_chooser,
				   filename);

  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
  
  return(TRUE);
}

gboolean
ags_functional_test_util_file_chooser_select_filenames(GtkFileChooser *file_chooser,
						       GSList *filename)
{
  if(file_chooser == NULL ||
     filename == NULL ||
     !GTK_IS_FILE_CHOOSER(file_chooser)){
    return(FALSE);
  }
  
  ags_test_enter();

  while(filename != NULL){
    gtk_file_chooser_select_filename(file_chooser,
				     filename->data);

    filename = filename->next;
  }
  
  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
  
  return(TRUE); 
}

gboolean
ags_functional_test_util_file_chooser_select_all(GtkFileChooser *file_chooser)
{
  if(file_chooser == NULL ||
     !GTK_IS_FILE_CHOOSER(file_chooser)){
    return(FALSE);
  }
  
  ags_test_enter();

  gtk_file_chooser_select_all(file_chooser);
  
  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
  
  return(TRUE); 
}

gboolean
ags_functional_test_util_file_default_window_resize()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;

  ags_test_enter();
    
  xorg_application_context = ags_application_context_get_instance();
  window = xorg_application_context->window;

  gdk_window_move_resize(gtk_widget_get_window(window),
			 64, 0,
			 1920 - 128, 1080 - 64);
    
  ags_test_leave();

  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_file_default_editor_resize()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsNotationEditor *notation_editor;

  GtkPaned *main_paned;
  GtkPaned *editor_paned;
  
  GdkRectangle allocation;
  
  ags_test_enter();
    
  xorg_application_context = ags_application_context_get_instance();
  window = xorg_application_context->window;
  notation_editor = window->notation_editor;

  main_paned = window->paned;
  editor_paned = notation_editor->paned;

  gtk_paned_set_position(main_paned,
			 (1080 - 64) * (2.0 / 3.0));

  gtk_paned_set_position(editor_paned,
			 (1920 - 128) / 6);

  ags_test_leave();

  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_open()
{
  AgsXorgApplicationContext *xorg_application_context;

  GtkMenu *menu;
  
  gboolean success;

  if(!ags_functional_test_util_menu_bar_click(GTK_STOCK_FILE)){
    return(FALSE);
  }

  ags_test_enter();
    
  xorg_application_context = ags_application_context_get_instance();
  menu = xorg_application_context->window->menu_bar->file;
  
  success = ags_functional_test_util_menu_click(menu,
						"open\0");

  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
  
  return(success); 
}

gboolean
ags_functional_test_util_save()
{
  AgsXorgApplicationContext *xorg_application_context;

  GtkMenu *menu;
  
  gboolean success;

  if(!ags_functional_test_util_menu_bar_click(GTK_STOCK_FILE)){
    return(FALSE);
  }
    
  ags_test_enter();

  xorg_application_context = ags_application_context_get_instance();
  menu = xorg_application_context->window->menu_bar->file;
  
  success = ags_functional_test_util_menu_click(menu,
						"save\0");

  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
  
  return(success); 
}

gboolean
ags_functional_test_util_save_as()
{
  AgsXorgApplicationContext *xorg_application_context;

  GtkMenu *menu;
  
  gboolean success;

  if(!ags_functional_test_util_menu_bar_click(GTK_STOCK_FILE)){
    return(FALSE);
  }
    
  ags_test_enter();

  xorg_application_context = ags_application_context_get_instance();
  menu = xorg_application_context->window->menu_bar->file;
  
  success = ags_functional_test_util_menu_click(menu,
						"save as\0");
  
  ags_test_leave();
  
  ags_functional_test_util_reaction_time_long();
  
  return(success); 
}

gboolean
ags_functional_test_util_export_open()
{
  AgsXorgApplicationContext *xorg_application_context;
  GtkWidget *export_window;
  
  GtkMenu *menu;
  
  gboolean success;

  if(!ags_functional_test_util_menu_bar_click(GTK_STOCK_FILE)){
    return(FALSE);
  }

  ags_test_enter();
    
  xorg_application_context = ags_application_context_get_instance();
  menu = xorg_application_context->window->menu_bar->file;

  export_window = xorg_application_context->window->export_window;

  ags_test_leave();
  
  success = ags_functional_test_util_menu_click(menu,
						"export\0");
  
  ags_functional_test_util_reaction_time_long();
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_test_util_default_timeout,
						      &export_window);
  
  return(success); 
}

gboolean
ags_functional_test_util_export_close()
{
  AgsXorgApplicationContext *xorg_application_context;
  GtkWidget *export_window;
  
  gboolean success;

  ags_test_enter();

  xorg_application_context = ags_application_context_get_instance();
  export_window = xorg_application_context->window->export_window;

  ags_test_leave();

  success = ags_functional_test_util_dialog_close(export_window);
  
  ags_functional_test_util_reaction_time_long();
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_hidden),
						      &ags_functional_test_util_default_timeout,
						      &export_window);
  
  return(success); 
}

gboolean
ags_funcitonal_test_util_export_add()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsExportWindow *export_window;
  GtkButton *add_button;

  AgsFunctionalTestUtilContainerTest container_test;

  GList *list_start;
  
  gboolean success;
  
  ags_test_enter();

  xorg_application_context = ags_application_context_get_instance();
  export_window = xorg_application_context->window->export_window;

  add_button = export_window->add;

  container_test.container = &(export_window->export_soundcard);

  list_start = gtk_container_get_children(export_window->export_soundcard);

  container_test.count = g_list_length(list_start) + 1;
  
  ags_test_leave();

  g_list_free(list_start);
  
  success = ags_functional_test_util_button_click(add_button);
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_container_children_count),
						      &ags_functional_test_util_default_timeout,
						      &container_test);
  
  return(success);
}

gboolean
ags_funcitonal_test_util_export_tact(gdouble tact)
{
  AgsXorgApplicationContext *xorg_application_context;

  AgsExportWindow *export_window;
  
  ags_test_enter();

  xorg_application_context = ags_application_context_get_instance();
  export_window = xorg_application_context->window->export_window;

  gtk_spin_button_set_value(export_window->tact,
			    tact);

  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
  
  return(TRUE);
}

gboolean
ags_funcitonal_test_util_export_remove(guint nth)
{
  AgsXorgApplicationContext *xorg_application_context;

  AgsExportWindow *export_window;

  GtkButton *remove_button;

  AgsFunctionalTestUtilContainerTest container_test;

  GList *list_start, *list;

  guint i;
  gboolean success;
  
  ags_test_enter();

  xorg_application_context = ags_application_context_get_instance();
  export_window = xorg_application_context->window->export_window;

  container_test.container = &(export_window->export_soundcard);

  list_start = gtk_container_get_children(export_window->export_soundcard);

  success = FALSE;

  if(list_start != NULL){
    remove_button = NULL;
  
    list = list_start;

    container_test.count = g_list_length(list_start) - 1;

    for(i = 0; list != NULL; i++){
      if(i == nth){
	GtkHBox *hbox;
      
	GList *tmp_start;
      
	hbox = GTK_HBOX(list->data);
	tmp_start = gtk_container_get_children(hbox);

	remove_button = gtk_bin_get_child(tmp_start->next->data);
	g_list_free(tmp_start);

	success = TRUE;
      
	break;
      }

      list = list->next;
    }

    g_list_free(list_start);
  
    ags_test_leave();

    g_list_free(list);
  
    if(!success){
      return(FALSE);
    }
    
    success = ags_functional_test_util_button_click(remove_button);
    ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_container_children_count),
							&ags_functional_test_util_default_timeout,
							&container_test);
  }  
  
  return(success);
}

gboolean
ags_funcitonal_test_util_export_set_backend(guint nth,
					    gchar *backend)
{
  AgsXorgApplicationContext *xorg_application_context;

  AgsExportWindow *export_window;

  GList *list_start, *list;

  guint i;
  gboolean success;

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();
  export_window = xorg_application_context->window->export_window;

  list_start =
    list = gtk_container_get_children(export_window->export_soundcard);
  success = FALSE;

  for(i = 0; list != NULL; i++){
    if(i == nth){
      AgsExportSoundcard *export_soundcard;
      GtkHBox *hbox;
      GtkTreeModel *model;

      GtkTreeIter iter;
      GList *tmp_start;

      gchar *value;

      guint active;
      
      hbox = GTK_HBOX(list->data);
      tmp_start = gtk_container_get_children(hbox);

      export_soundcard = AGS_EXPORT_SOUNDCARD(tmp_start->data);
      g_list_free(tmp_start);

      model = gtk_combo_box_get_model(GTK_COMBO_BOX(export_soundcard->backend));
      active = 0;

      if(gtk_tree_model_get_iter_first(model, &iter)){
	do{
	  gtk_tree_model_get(model, &iter,
			     0, &value,
			     -1);

	  if(!g_strcmp0(backend,
			value)){
	    gtk_combo_box_set_active_iter((GtkComboBox *) export_soundcard->backend,
					  &iter);
	    success = TRUE;
	    
	    break;
	  }
	}while(gtk_tree_model_iter_next(model,
					&iter));
      }      
      
      break;
    }

    list = list->next;
  }

  g_list_free(list_start);
  
  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
  
  return(success);
}

gboolean
ags_funcitonal_test_util_export_set_device(guint nth,
					   gchar *device)
{
  AgsXorgApplicationContext *xorg_application_context;

  AgsExportWindow *export_window;

  GList *list_start, *list;

  guint i;
  gboolean success;
  
  ags_test_enter();

  xorg_application_context = ags_application_context_get_instance();
  export_window = xorg_application_context->window->export_window;

  list_start =
    list = gtk_container_get_children(export_window->export_soundcard);
  success = FALSE;

  for(i = 0; list != NULL; i++){
    if(i == nth){
      AgsExportSoundcard *export_soundcard;
      GtkHBox *hbox;
      GtkTreeModel *model;

      GtkTreeIter iter;
      GList *tmp_start;

      gchar *value;

      guint active;
      
      hbox = GTK_HBOX(list->data);
      tmp_start = gtk_container_get_children(hbox);

      export_soundcard = AGS_EXPORT_SOUNDCARD(tmp_start->data);
      g_list_free(tmp_start);

      model = gtk_combo_box_get_model(GTK_COMBO_BOX(export_soundcard->card));
      active = 0;

      if(gtk_tree_model_get_iter_first(model, &iter)){
	do{
	  gtk_tree_model_get(model, &iter,
			     0, &value,
			     -1);

	  if(!g_strcmp0(device,
			value)){
	    gtk_combo_box_set_active_iter((GtkComboBox *) export_soundcard->card,
					  &iter);
	    success = TRUE;
	    
	    break;
	  }
	}while(gtk_tree_model_iter_next(model,
					&iter));
      }      
      
      break;
    }

    list = list->next;
  }

  g_list_free(list_start);

  ags_test_leave();
  
  ags_functional_test_util_reaction_time_long();
  
  return(success);
}

gboolean
ags_funcitonal_test_util_export_set_filename(guint nth,
					     gchar *filename)
{
  AgsXorgApplicationContext *xorg_application_context;

  AgsExportWindow *export_window;

  GList *list_start, *list;

  guint i;
  gboolean success;

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();
  export_window = xorg_application_context->window->export_window;

  list_start =
    list = gtk_container_get_children(export_window->export_soundcard);
  success = FALSE;

  for(i = 0; list != NULL; i++){
    if(i == nth){
      AgsExportSoundcard *export_soundcard;
      GtkHBox *hbox;

      GList *tmp_start;
      
      hbox = GTK_HBOX(list->data);
      tmp_start = gtk_container_get_children(hbox);

      export_soundcard = AGS_EXPORT_SOUNDCARD(tmp_start->data);
      g_list_free(tmp_start);

      gtk_entry_set_text(export_soundcard->filename,
			 filename);
      
      success = TRUE;
      
      break;
    }
    
    list = list->next;
  }

  g_list_free(list_start);

  ags_test_leave();
  
  ags_functional_test_util_reaction_time_long();
  
  return(success);
}

gboolean
ags_funcitonal_test_util_export_open(guint nth)
{
  //TODO:JK: 
}

gboolean
ags_funcitonal_test_util_export_set_format(guint nth,
					   gchar *format)
{
  //TODO:JK: 
}

gboolean
ags_funcitonal_test_util_export_do_export(guint nth,
					  gchar *format)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_quit()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_add_machine(gchar *submenu,
				     gchar *machine_name)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsMenuBar *menu_bar;
  AgsMachine *machine;
  
  GtkMenu *add_menu;

  GList *list_start;
  
  AgsFunctionalTestUtilContainerTest container_test;

  gboolean success;
  
  if(machine_name == NULL){
    return(FALSE);
  }

  if(!ags_functional_test_util_menu_bar_click(GTK_STOCK_EDIT)){    
    return(FALSE);
  }

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  window = xorg_application_context->window;
  menu_bar = window->menu_bar;

  container_test.container = &(window->machines);
  list_start = gtk_container_get_children(window->machines);

  ags_test_leave();

  container_test.count = g_list_length(list_start) + 1;
  
  success = FALSE;
  
  success = ags_functional_test_util_menu_click(menu_bar->edit,
						GTK_STOCK_ADD);

  if(!success){
    return(FALSE);
  }
  
  add_menu = ags_functional_test_util_submenu_find(menu_bar->edit,
						   GTK_STOCK_ADD);

  if(submenu == NULL){
    success = ags_functional_test_util_menu_click(add_menu,
						  machine_name);
  }else{
    GtkMenu *bridge_menu;
    
    bridge_menu = ags_functional_test_util_submenu_find(add_menu,
							submenu);

    success = ags_functional_test_util_menu_click(bridge_menu,
						  machine_name);    
  }

  if(success){
    ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_container_children_count),
							&ags_functional_test_util_default_timeout,
							&container_test);
    ags_functional_test_util_reaction_time_long();

    ags_test_enter();

    list_start = gtk_container_get_children(window->machines);
    machine = g_list_nth_data(list_start,
			      container_test.count - 1);

    ags_test_leave();
    
    ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
							&ags_functional_test_util_default_timeout,
							&machine);
  }

  return(success);
}

gboolean
ags_functional_test_util_automation_open()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_close()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_preferences_open()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_preferences_close()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_about_open()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_about_close()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_expand()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_bpm(gdouble bpm)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_rewind()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_prev()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_play()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_stop()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_next()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_forward()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_loop()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_position(gdouble position)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_loop_left(gdouble loop_left)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_loop_right(gdouble loop_right)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_exclude_sequencers()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_notation_toolbar_cursor_click()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  
  GtkButton *position;

  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  window = xorg_application_context->window;
  notation_editor = window->notation_editor;
  notation_toolbar = notation_editor->notation_toolbar;

  position = notation_toolbar->position;
  
  ags_test_leave();

  success = ags_functional_test_util_button_click(position);

  return(success);
}

gboolean
ags_functional_test_util_notation_toolbar_edit_click()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  
  GtkButton *edit;

  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  window = xorg_application_context->window;
  notation_editor = window->notation_editor;
  notation_toolbar = notation_editor->notation_toolbar;

  edit = notation_toolbar->edit;
  
  ags_test_leave();

  success = ags_functional_test_util_button_click(edit);

  return(success);
}

gboolean
ags_functional_test_util_notation_toolbar_delete_click()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  
  GtkButton *clear;

  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  window = xorg_application_context->window;
  notation_editor = window->notation_editor;
  notation_toolbar = notation_editor->notation_toolbar;

  clear = notation_toolbar->clear;
  
  ags_test_leave();

  success = ags_functional_test_util_button_click(clear);

  return(success);
}

gboolean
ags_functional_test_util_notation_toolbar_select_click()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  
  GtkButton *select;

  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  window = xorg_application_context->window;
  notation_editor = window->notation_editor;
  notation_toolbar = notation_editor->notation_toolbar;

  select = notation_toolbar->select;
  
  ags_test_leave();

  success = ags_functional_test_util_button_click(select);

  return(success);
}

gboolean
ags_functional_test_util_notation_toolbar_invert_click()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  
  GtkButton *invert;

  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  window = xorg_application_context->window;
  notation_editor = window->notation_editor;
  notation_toolbar = notation_editor->notation_toolbar;

  invert = notation_toolbar->invert;
  
  ags_test_leave();

  success = ags_functional_test_util_button_click(invert);

  return(success);
}

gboolean
ags_functional_test_util_notation_toolbar_paste_click()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  
  GtkButton *paste;

  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  window = xorg_application_context->window;
  notation_editor = window->notation_editor;
  notation_toolbar = notation_editor->notation_toolbar;

  paste = notation_toolbar->paste;
  
  ags_test_leave();

  success = ags_functional_test_util_button_click(paste);

  return(success);
}

gboolean
ags_functional_test_util_notation_toolbar_copy_click()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  
  GtkButton *copy;

  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  window = xorg_application_context->window;
  notation_editor = window->notation_editor;
  notation_toolbar = notation_editor->notation_toolbar;

  copy = notation_toolbar->copy;
  
  ags_test_leave();

  success = ags_functional_test_util_button_click(copy);

  return(success);
}

gboolean
ags_functional_test_util_notation_toolbar_cut_click()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  
  GtkButton *cut;

  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  window = xorg_application_context->window;
  notation_editor = window->notation_editor;
  notation_toolbar = notation_editor->notation_toolbar;

  cut = notation_toolbar->cut;
  
  ags_test_leave();

  success = ags_functional_test_util_button_click(cut);

  return(success);
}

gboolean
ags_functional_test_util_notation_toolbar_zoom(guint nth_zoom)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  
  GtkComboBox *zoom;

  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  window = xorg_application_context->window;
  notation_editor = window->notation_editor;
  notation_toolbar = notation_editor->notation_toolbar;

  zoom = notation_toolbar->zoom;
  
  ags_test_leave();

  success = ags_functional_test_util_combo_box_click(zoom,
						     nth_zoom);

  return(success);
}

gboolean
ags_functional_test_util_machine_selector_select(guint nth_index)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsNotationEditor *notation_editor;
  AgsMachineSelector *machine_selector;
  AgsMachineRadioButton *machine_radio_button;
  
  GList *list_start, *list;
  
  gboolean success;

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  window = xorg_application_context->window;
  notation_editor = window->notation_editor;
  machine_selector = notation_editor->machine_selector;
  
  list_start = gtk_container_get_children(machine_selector);

  ags_test_leave();

  list = g_list_nth(list_start,
		    nth_index + 1);
  
  if(list == NULL){
    return(FALSE);
  }

  machine_radio_button = list->data;
  g_list_free(list_start);
  
  success = ags_functional_test_util_button_click(machine_radio_button);

  ags_functional_test_util_reaction_time_long();  

  return(success);
}

gboolean
ags_functional_test_util_machine_selection_select(gchar *machine)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsNotationEditor *notation_editor;
  AgsMachineSelector *machine_selector;
  AgsMachineSelection *machine_selection;

  GtkRadioButton *radio_button;
  
  GList *list_start, *list;
  
  gboolean success;
  
  if(machine == NULL){
    return(FALSE);
  }

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  window = xorg_application_context->window;
  notation_editor = window->notation_editor;
  machine_selector = notation_editor->machine_selector;
  machine_selection = machine_selector->machine_selection;

  list = 
    list_start = gtk_container_get_children(GTK_DIALOG(machine_selection)->vbox);

  success = FALSE;

  while(list != NULL){
    if(GTK_IS_RADIO_BUTTON(list->data)){
      gchar *str;

      str = gtk_button_get_label(GTK_BUTTON(list->data));
    
      if(!g_ascii_strcasecmp(machine,
			     str)){
	radio_button = list->data;
	
	success = TRUE;

	break;
      }
    }
    
    list = list->next;
  }
  
  ags_test_leave();

  g_list_free(list_start);

  if(!success){
    return(FALSE);
  }
  
  success = ags_functional_test_util_button_click(radio_button);

  ags_functional_test_util_reaction_time_long();  

  return(success);
}

gboolean
ags_functional_test_util_machine_selection_remove_index()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsNotationEditor *notation_editor;
  AgsMachineSelector *machine_selector;
  
  GtkButton *menu_tool_button;
  GtkMenu *popup;
  
  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  window = xorg_application_context->window;
  notation_editor = window->notation_editor;
  machine_selector = notation_editor->machine_selector;
  
  menu_tool_button = machine_selector->menu_button;
  popup = machine_selector->popup;
  
  ags_test_leave();

  success = ags_functional_test_util_menu_tool_button_click(menu_tool_button);

  if(!success){
    return(FALSE);
  }
  
  ags_functional_test_util_reaction_time_long();
  
  success = ags_functional_test_util_menu_click(popup,
						"remove index\0");
  
  ags_functional_test_util_reaction_time_long();

  return(success);
}

gboolean
ags_functional_test_util_machine_selection_add_index()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsNotationEditor *notation_editor;
  AgsMachineSelector *machine_selector;
  
  GtkButton *menu_tool_button;
  GtkMenu *popup;
  
  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  window = xorg_application_context->window;
  notation_editor = window->notation_editor;
  machine_selector = notation_editor->machine_selector;
  
  menu_tool_button = machine_selector->menu_button;
  popup = machine_selector->popup;
  
  ags_test_leave();

  success = ags_functional_test_util_menu_tool_button_click(menu_tool_button);

  if(!success){
    return(FALSE);
  }
  
  ags_functional_test_util_reaction_time_long();
  
  success = ags_functional_test_util_menu_click(popup,
						"add index\0");
  
  ags_functional_test_util_reaction_time_long();

  return(success);
}

gboolean
ags_functional_test_util_machine_selection_link_index()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsNotationEditor *notation_editor;
  AgsMachineSelector *machine_selector;
  
  GtkButton *menu_tool_button;
  GtkMenu *popup;
  
  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  window = xorg_application_context->window;
  notation_editor = window->notation_editor;
  machine_selector = notation_editor->machine_selector;
  
  menu_tool_button = machine_selector->menu_button;
  popup = machine_selector->popup;
  
  ags_test_leave();

  success = ags_functional_test_util_menu_tool_button_click(menu_tool_button);

  if(!success){
    return(FALSE);
  }
  
  ags_functional_test_util_reaction_time_long();
  
  success = ags_functional_test_util_menu_click(popup,
						"link index\0");
  
  ags_functional_test_util_reaction_time_long();

  return(success);
}

gboolean
ags_functional_test_util_machine_selection_reverse_mapping()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsNotationEditor *notation_editor;
  AgsMachineSelector *machine_selector;
  
  GtkButton *menu_tool_button;
  GtkMenu *popup;
  
  gboolean success;

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  window = xorg_application_context->window;
  notation_editor = window->notation_editor;
  machine_selector = notation_editor->machine_selector;
  
  menu_tool_button = machine_selector->menu_button;
  popup = machine_selector->popup;
  
  ags_test_leave();

  success = ags_functional_test_util_menu_tool_button_click(menu_tool_button);

  if(!success){
    return(FALSE);
  }
  
  ags_functional_test_util_reaction_time_long();
  
  success = ags_functional_test_util_menu_click(popup,
						"reverse mapping\0");
  
  ags_functional_test_util_reaction_time_long();

  return(success);
}

gboolean
ags_functional_test_util_machine_selection_shift_piano(guint nth_shift)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_notation_edit_delete_point(guint x,
						    guint y)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_notation_edit_add_point(guint x0, guint x1,
						 guint y)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  AgsNotationEdit *notation_edit;
  
  GtkWidget *widget;
  GtkScrollbar *hscrollbar;
  GtkScrollbar *vscrollbar;
  GtkAdjustment *adjustment;
  
  GdkDisplay *display;
  GdkScreen *screen;
  GdkWindow *window;

  gdouble zoom;
  guint history;
  guint width, height;
  guint control_count;
  guint origin_x, origin_y;
  guint widget_x, widget_y;
  gboolean success;

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  notation_editor = xorg_application_context->window->notation_editor;
  notation_toolbar = notation_editor->notation_toolbar;

  if(notation_editor->selected_machine == NULL){
    ags_test_leave();

    return(FALSE);
  }
  
  notation_edit = notation_editor->notation_edit;
  widget = notation_edit->drawing_area;
  
  display = gtk_widget_get_display(widget);
  screen = gtk_widget_get_screen(widget);
  
  history = gtk_combo_box_get_active(GTK_COMBO_BOX(notation_toolbar->zoom));
  zoom = exp2((double) history - 2.0);
  
  ags_test_leave();

  /*  */
  ags_test_enter();

  hscrollbar = notation_edit->hscrollbar;
  vscrollbar = notation_edit->vscrollbar;
  
  window = gtk_widget_get_window(widget);

  widget_x = widget->allocation.x;
  widget_y = widget->allocation.y;

  width = widget->allocation.width;
  height = widget->allocation.height;
  
  gdk_window_get_origin(window, &origin_x, &origin_y);

  /* make visible */
  adjustment = GTK_RANGE(hscrollbar)->adjustment;
  
  if((x0 * notation_edit->control_width) > adjustment->value + adjustment->page_size){
    gtk_adjustment_set_value(adjustment,
			     x0 * notation_edit->control_width);

    ags_functional_test_util_reaction_time_long();
  }else if((x0 * notation_edit->control_width) < adjustment->value){
    gtk_adjustment_set_value(adjustment,
			     x0 * notation_edit->control_width);

    ags_functional_test_util_reaction_time_long();
  }

  x0 = (x0 * notation_edit->control_width) - (adjustment->value);
  x1 = (x1 * notation_edit->control_width) - (adjustment->value);

  adjustment = GTK_RANGE(vscrollbar)->adjustment;
  
  if((y * notation_edit->control_height) > (adjustment->value + adjustment->page_size)){
    gtk_adjustment_set_value(adjustment,
			     (y * notation_edit->control_height));

    ags_functional_test_util_reaction_time_long();
  }else if((y * notation_edit->control_height) < adjustment->value){
    gtk_adjustment_set_value(adjustment,
			     (y * notation_edit->control_height));

    ags_functional_test_util_reaction_time_long();
  }

  y = (y * notation_edit->control_height) - (adjustment->value);
  
  ags_test_leave();

  /*  */
  gdk_display_warp_pointer(display,
			   screen,
			   origin_x + x0 + 8, origin_y + y + 7);

  ags_functional_test_util_reaction_time();

  gdk_test_simulate_button(window,
			   x0 + 8,
			   y + 7,
			   1,
			   GDK_BUTTON1_MASK,
			   GDK_BUTTON_PRESS);

  ags_functional_test_util_reaction_time();

  gdk_display_warp_pointer(display,
			   screen,
			   origin_x + x1 + 8, origin_y + y + 7);

  ags_functional_test_util_reaction_time();

  gdk_test_simulate_button(window,
			   x1 + 8,
			   y + 7,
			   1,
			   GDK_BUTTON1_MASK,
			   GDK_BUTTON_RELEASE);
  
  ags_functional_test_util_reaction_time_long();
  
  return(TRUE);
}

gboolean
ags_functional_test_util_notation_edit_select_region(guint x0, guint x1,
						     guint y0, guint y1)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_toolbar_cursor_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_toolbar_edit_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_toolbar_delete_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_toolbar_select_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_toolbar_paste_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_toolbar_copy_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_toolbar_cut_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_toolbar_zoom(guint nth_zoom)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_edit_audio_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_edit_output_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_edit_input_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_edit_delete_point(guint nth_index,
						      guint x,
						      guint y)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_edit_add_point(guint nth_index,
						   guint x,
						   guint y)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_edit_select_region(guint nth_index,
						       guint x0, guint x1,
						       guint y0, guint y1)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_move_up(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_move_down(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_hide(guint nth_machine)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;

  GtkMenu *popup;
  
  GList *list_start, *list;
  
  gboolean success;

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }
  
  g_list_free(list_start);
  
  ags_test_enter();

  popup = machine->popup;
  
  ags_test_leave();

  /* activate hide */
  success = ags_functional_test_util_menu_tool_button_click(machine->menu_tool_button);

  if(!success){
    return(FALSE);
  }
  
  ags_functional_test_util_reaction_time_long();
  
  success = ags_functional_test_util_menu_click(popup,
						"hide\0");
  ags_functional_test_util_reaction_time_long();

  return(success);
}

gboolean
ags_functional_test_util_machine_show(guint nth_machine)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;

  GtkMenu *popup;

  GList *list_start, *list;
  
  gboolean success;

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }
  
  g_list_free(list_start);

  ags_test_enter();

  popup = machine->popup;
  
  ags_test_leave();

  /* activate show */
  success = ags_functional_test_util_menu_tool_button_click(machine->menu_tool_button);
  ags_functional_test_util_reaction_time_long();

  success = ags_functional_test_util_menu_click(popup,
						"show\0");
  ags_functional_test_util_reaction_time_long();

  return(success);
}

gboolean
ags_functional_test_util_machine_destroy(guint nth_machine)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsMachine *machine;

  GtkMenu *popup;

  GList *list_start, *list;

  AgsFunctionalTestUtilContainerTest container_test;
  
  gboolean success;

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  window = xorg_application_context->window;
  
  /* retrieve machine */
  container_test.container = &(window->machines);

  list_start = gtk_container_get_children(window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }
  
  container_test.count = g_list_length(list_start) - 1;

  g_list_free(list_start);

  ags_test_enter();

  popup = machine->popup;
  
  ags_test_leave();
  
  /* activate destroy */
  success = ags_functional_test_util_menu_tool_button_click(machine->menu_tool_button);

  success = ags_functional_test_util_menu_click(popup,
						"destroy\0");

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_container_children_count),
						      &ags_functional_test_util_default_timeout,
						      &container_test);

  return(success);
}

gboolean
ags_functional_test_util_machine_rename_open(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_rename_close(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_rename_set_name(guint nth_machine,
						 gchar *name)
{
  //TODO:JK: 
}

GtkWidget*
ags_functional_test_util_get_line_editor(GtkWidget *machine_editor,
					 guint nth_pad, guint nth_audio_channel,
					 gboolean is_output)
{
  AgsListingEditor *listing_editor;
  AgsPadEditor *pad_editor;
  AgsLineEditor *line_editor;  
  
  GList *list_start, *list;

  if(machine_editor == NULL){
    return(NULL);
  }
  
  ags_test_enter();

  if(is_output){
    listing_editor = AGS_LISTING_EDITOR(AGS_MACHINE_EDITOR(machine_editor)->output_editor);
  }else{
    listing_editor = AGS_LISTING_EDITOR(AGS_MACHINE_EDITOR(machine_editor)->input_editor);
  }

  list_start = gtk_container_get_children(listing_editor->child);
  list = g_list_nth(list_start,
		    nth_pad);

  if(list != NULL &&
     AGS_IS_PAD_EDITOR(list->data)){
    pad_editor = list->data;

    g_list_free(list_start);
  }else{
    g_list_free(list_start);

    return(NULL);
  }
  
  /* line editor */
  list_start = gtk_container_get_children(pad_editor->line_editor);
  list = g_list_nth(list_start,
		    nth_audio_channel);

  if(list != NULL &&
     AGS_IS_LINE_EDITOR(list->data)){
    line_editor = list->data;

    g_list_free(list_start);
  }else{
    g_list_free(list_start);

    return(NULL);
  }
  
  ags_test_leave();

  return(line_editor);
}

gboolean
ags_functional_test_util_machine_properties_open(guint nth_machine)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;
  AgsMachine *machine;
  GtkWidget **properties;
  GtkMenuToolButton *menu_tool_button;
  
  GList *list_start, *list;
  
  gboolean success;

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  window = xorg_application_context->window;
  
  /* retrieve machine */
  list_start = gtk_container_get_children(window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }
  
  g_list_free(list_start);

  /* activate destroy */
  ags_test_enter();

  properties = &(machine->properties);

  menu_tool_button = machine->menu_tool_button;

  ags_test_leave();
  
  success = ags_functional_test_util_menu_tool_button_click(menu_tool_button);

  success = ags_functional_test_util_menu_click(machine->popup,
						"properties\0");

  ags_functional_test_util_reaction_time_long();
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_test_util_default_timeout,
						      properties);

  return(success);
}

gboolean
ags_functional_test_util_machine_properties_click_tab(guint nth_machine,
						      guint nth_tab)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  
  GList *list_start, *list;
  
  gboolean success;

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* click tab */
  ags_test_enter();

  machine_editor = machine->properties;

  gtk_notebook_set_current_page(machine_editor->notebook,
				nth_tab);

  ags_test_leave();

  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_properties_click_enable(guint nth_machine)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  GtkButton *enable_button;
  
  GList *list_start, *list;

  guint nth_tab;
  gboolean success;

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */
  enable_button = NULL;
  
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* click tab */
  ags_test_enter();

  machine_editor = machine->properties;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      enable_button = AGS_PROPERTY_EDITOR(machine_editor->output_editor)->enabled;
    }
    break;
  case 1:
    {
      enable_button = AGS_PROPERTY_EDITOR(machine_editor->input_editor)->enabled;
    }
    break;
  case 2:
    {
      enable_button = AGS_PROPERTY_EDITOR(machine_editor->output_link_editor)->enabled;
    }
    break;
  case 3:
    {
      enable_button = AGS_PROPERTY_EDITOR(machine_editor->input_link_editor)->enabled;
    }
    break;
  case 4:
    {
      enable_button = AGS_PROPERTY_EDITOR(machine_editor->resize_editor)->enabled;
    }
    break;
  }
  
  ags_test_leave();

  success = ags_functional_test_util_button_click(enable_button);

  ags_functional_test_util_reaction_time_long();

  return(success);
}

gboolean
ags_functional_test_util_machine_properties_link_set(guint nth_machine,
						     guint pad, guint audio_channel,
						     gchar *link_name, guint link_nth_line)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsListingEditor *listing_editor;
  AgsPadEditor *pad_editor;
  AgsLineEditor *line_editor;
  AgsLinkEditor *link_editor;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *list_start, *list;

  gchar *value;

  guint nth_tab;
  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* set link */
  ags_test_enter();

  machine_editor = machine->properties;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      listing_editor = AGS_LISTING_EDITOR(machine_editor->output_editor);
    }
    break;
  case 1:
    {
      listing_editor = AGS_LISTING_EDITOR(machine_editor->input_editor);
    }
    break;
  default:
    ags_test_leave();

    return(FALSE);
  }

  list_start = gtk_container_get_children(listing_editor->child);
  list = g_list_nth(list_start,
		    pad);

  if(list != NULL &&
     AGS_IS_PAD_EDITOR(list->data)){
    pad_editor = list->data;

    g_list_free(list_start);
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }

  /* expander */
  gtk_expander_set_expanded(pad_editor->line_editor_expander,
			    TRUE);
  
  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
  
  /* line editor */
  ags_test_enter();

  list_start = gtk_container_get_children(pad_editor->line_editor);
  list = g_list_nth(list_start,
		    audio_channel);

  if(list != NULL &&
     AGS_IS_LINE_EDITOR(list->data)){
    line_editor = list->data;

    g_list_free(list_start);
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }

  /* link editor */
  link_editor = line_editor->link_editor;

  /* set link */
  model = gtk_combo_box_get_model(GTK_COMBO_BOX(link_editor->combo));
  success = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);
      
      if(!g_strcmp0(link_name,
		    value)){
	gtk_combo_box_set_active_iter((GtkComboBox *) link_editor->combo,
				      &iter);
	success = TRUE;
	
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }      
  
  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
  
  if(!success){
    return(FALSE);
  }
  
  /* set link line */
  ags_test_enter();

  success = TRUE;
  gtk_spin_button_set_value(link_editor->spin_button,
			    link_nth_line);

  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
 
  return(success);
}

gboolean
ags_functional_test_util_machine_properties_link_open(guint nth_machine,
						      guint pad, guint audio_channel)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_effect_add(guint nth_machine,
						       guint pad, guint audio_channel)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsListingEditor *listing_editor;
  AgsPadEditor *pad_editor;
  AgsLineEditor *line_editor;
  AgsLineMemberEditor *member_editor;
  GtkDialog **effect_dialog;
  GtkButton *add_button;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *list_start, *list;

  gchar *value;

  guint nth_tab;
  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* get tab */
  ags_test_enter();

  machine_editor = machine->properties;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      listing_editor = AGS_LISTING_EDITOR(machine_editor->output_editor);
    }
    break;
  case 1:
    {
      listing_editor = AGS_LISTING_EDITOR(machine_editor->input_editor);
    }
    break;
  default:
    ags_test_leave();

    return(FALSE);
  }

  list_start = gtk_container_get_children(listing_editor->child);
  list = g_list_nth(list_start,
		    pad);

  if(list != NULL &&
     AGS_IS_PAD_EDITOR(list->data)){
    pad_editor = list->data;

    g_list_free(list_start);
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }

  /* expander */
  gtk_expander_set_expanded(pad_editor->line_editor_expander,
			    TRUE);
  
  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
  
  /* line editor */
  ags_test_enter();

  list_start = gtk_container_get_children(pad_editor->line_editor);
  list = g_list_nth(list_start,
		    audio_channel);

  if(list != NULL &&
     AGS_IS_LINE_EDITOR(list->data)){
    line_editor = list->data;

    g_list_free(list_start);
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }

  /* line member editor */
  member_editor = line_editor->member_editor;
  
  /* effect dialog and add button */
  effect_dialog = &(member_editor->plugin_browser);
  add_button = member_editor->add;
  
  ags_test_leave();

  success = ags_functional_test_util_button_click(add_button);
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_test_util_default_timeout,
						      effect_dialog);
  
  ags_functional_test_util_reaction_time_long();
 
  return(success);
}

gboolean
ags_functional_test_util_machine_properties_effect_remove(guint nth_machine,
							  guint pad, guint audio_channel,
							  guint nth_effect)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsListingEditor *listing_editor;
  AgsPadEditor *pad_editor;
  AgsLineEditor *line_editor;
  AgsLineMemberEditor *member_editor;
  GtkDialog **effect_dialog;
  GtkButton *check_button;
  GtkButton *remove_button;
  
  GtkTreeModel *model;

  AgsFunctionalTestUtilContainerTest container_test;
  
  GtkTreeIter iter;
  GList *list_start, *list;

  gchar *value;

  guint nth_tab;
  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* get tab */
  ags_test_enter();

  machine_editor = machine->properties;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      listing_editor = AGS_LISTING_EDITOR(machine_editor->output_editor);
    }
    break;
  case 1:
    {
      listing_editor = AGS_LISTING_EDITOR(machine_editor->input_editor);
    }
    break;
  default:
    ags_test_leave();

    return(FALSE);
  }

  list_start = gtk_container_get_children(listing_editor->child);
  list = g_list_nth(list_start,
		    pad);

  if(list != NULL &&
     AGS_IS_PAD_EDITOR(list->data)){
    pad_editor = list->data;

    g_list_free(list_start);
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }

  /* expander */
  gtk_expander_set_expanded(pad_editor->line_editor_expander,
			    TRUE);
  
  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
  
  /* line editor */
  ags_test_enter();

  list_start = gtk_container_get_children(pad_editor->line_editor);
  list = g_list_nth(list_start,
		    audio_channel);

  if(list != NULL &&
     AGS_IS_LINE_EDITOR(list->data)){
    line_editor = list->data;

    g_list_free(list_start);
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }

  /* line member editor */
  member_editor = line_editor->member_editor;
  
  /* effect dialog and remove button */
  effect_dialog = &(member_editor->plugin_browser);
  remove_button = member_editor->remove;

  /* check button */
  container_test.container = &(member_editor->line_member);
  
  check_button = NULL;

  list_start = gtk_container_get_children(member_editor->line_member);

  container_test.count = g_list_length(list_start) - 1;

  list = g_list_nth(list_start,
		    nth_effect);

  if(list != NULL){
    GtkHBox *hbox;

    hbox = list->data;
    list_start = gtk_container_get_children(hbox);
    
    check_button = list_start->data;

    g_list_free(list_start);
  }
  
  ags_test_leave();

  /* click check button */
  success = ags_functional_test_util_button_click(check_button);

  /* click remove */
  if(success){
    ags_functional_test_util_reaction_time_long();
  
    success = ags_functional_test_util_button_click(remove_button);
  
    ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_container_children_count),
							&ags_functional_test_util_default_timeout,
							&container_test);
  
    ags_functional_test_util_reaction_time_long();
  }
  
  return(success);
}

gboolean
ags_functional_test_util_machine_properties_effect_plugin_type(guint nth_machine,
							       guint pad, guint audio_channel,
							       gchar *plugin_type)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsListingEditor *listing_editor;
  AgsPadEditor *pad_editor;
  AgsLineEditor *line_editor;
  AgsLineMemberEditor *member_editor;
  AgsPluginBrowser *plugin_browser;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *list_start, *list;

  gchar *value;

  guint nth_tab;
  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* get tab */
  ags_test_enter();

  machine_editor = machine->properties;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      listing_editor = AGS_LISTING_EDITOR(machine_editor->output_editor);
    }
    break;
  case 1:
    {
      listing_editor = AGS_LISTING_EDITOR(machine_editor->input_editor);
    }
    break;
  default:
    ags_test_leave();

    return(FALSE);
  }

  /* pad editor */
  list_start = gtk_container_get_children(listing_editor->child);
  list = g_list_nth(list_start,
		    pad);

  if(list != NULL &&
     AGS_IS_PAD_EDITOR(list->data)){
    pad_editor = list->data;

    g_list_free(list_start);
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }
    
  /* line editor */
  list_start = gtk_container_get_children(pad_editor->line_editor);
  list = g_list_nth(list_start,
		    audio_channel);

  if(list != NULL &&
     AGS_IS_LINE_EDITOR(list->data)){
    line_editor = list->data;

    g_list_free(list_start);
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }

  /* line member editor */
  member_editor = line_editor->member_editor;
  plugin_browser = member_editor->plugin_browser;
  
  /* set plugin type */
  model = gtk_combo_box_get_model(GTK_COMBO_BOX(plugin_browser->plugin_type));
  success = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);
      
      if(!g_strcmp0(plugin_type,
		    value)){
	gtk_combo_box_set_active_iter((GtkComboBox *) plugin_browser->plugin_type,
				      &iter);
	success = TRUE;
	
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }      
  
  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
 
  return(success);
}

gboolean
ags_functional_test_util_machine_properties_ladspa_filename(guint nth_machine,
							    guint pad, guint audio_channel,
							    gchar *filename)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsListingEditor *listing_editor;
  AgsPadEditor *pad_editor;
  AgsLineEditor *line_editor;
  AgsLineMemberEditor *member_editor;
  AgsPluginBrowser *plugin_browser;
  AgsLadspaBrowser *ladspa_browser;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *list_start, *list;

  gchar *value;

  guint nth_tab;
  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* get tab */
  ags_test_enter();

  machine_editor = machine->properties;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      listing_editor = AGS_LISTING_EDITOR(machine_editor->output_editor);
    }
    break;
  case 1:
    {
      listing_editor = AGS_LISTING_EDITOR(machine_editor->input_editor);
    }
    break;
  default:
    ags_test_leave();

    return(FALSE);
  }

  list_start = gtk_container_get_children(listing_editor->child);
  list = g_list_nth(list_start,
		    pad);

  if(list != NULL &&
     AGS_IS_PAD_EDITOR(list->data)){
    pad_editor = list->data;

    g_list_free(list_start);
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }
  
  /* line editor */
  list_start = gtk_container_get_children(pad_editor->line_editor);
  list = g_list_nth(list_start,
		    audio_channel);

  if(list != NULL &&
     AGS_IS_LINE_EDITOR(list->data)){
    line_editor = list->data;

    g_list_free(list_start);
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }

  /* line member editor */
  member_editor = line_editor->member_editor;
  plugin_browser = member_editor->plugin_browser;
  ladspa_browser = plugin_browser->ladspa_browser;
  
  /* set plugin filename */
  model = gtk_combo_box_get_model(GTK_COMBO_BOX(ladspa_browser->filename));
  success = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);
      
      if(!g_strcmp0(filename,
		    value)){
	gtk_combo_box_set_active_iter((GtkComboBox *) ladspa_browser->filename,
				      &iter);
	success = TRUE;
	
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }      
  
  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
 
  return(success);
}

gboolean
ags_functional_test_util_machine_properties_ladspa_effect(guint nth_machine,
							  guint pad, guint audio_channel,
							  gchar *effect)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsListingEditor *listing_editor;
  AgsPadEditor *pad_editor;
  AgsLineEditor *line_editor;
  AgsLineMemberEditor *member_editor;
  AgsPluginBrowser *plugin_browser;
  AgsLadspaBrowser *ladspa_browser;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *list_start, *list;

  gchar *value;

  guint nth_tab;
  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* get tab */
  ags_test_enter();

  machine_editor = machine->properties;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      listing_editor = AGS_LISTING_EDITOR(machine_editor->output_editor);
    }
    break;
  case 1:
    {
      listing_editor = AGS_LISTING_EDITOR(machine_editor->input_editor);
    }
    break;
  default:
    ags_test_leave();

    return(FALSE);
  }

  list_start = gtk_container_get_children(listing_editor->child);
  list = g_list_nth(list_start,
		    pad);

  if(list != NULL &&
     AGS_IS_PAD_EDITOR(list->data)){
    pad_editor = list->data;

    g_list_free(list_start);
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }
  
  /* line editor */
  list_start = gtk_container_get_children(pad_editor->line_editor);
  list = g_list_nth(list_start,
		    audio_channel);

  if(list != NULL &&
     AGS_IS_LINE_EDITOR(list->data)){
    line_editor = list->data;

    g_list_free(list_start);
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }

  /* line member editor */
  member_editor = line_editor->member_editor;
  plugin_browser = member_editor->plugin_browser;
  ladspa_browser = plugin_browser->ladspa_browser;
  
  /* set plugin effect */
  model = gtk_combo_box_get_model(GTK_COMBO_BOX(ladspa_browser->effect));
  success = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);
      
      if(!g_strcmp0(effect,
		    value)){
	gtk_combo_box_set_active_iter((GtkComboBox *) ladspa_browser->effect,
				      &iter);
	success = TRUE;
	
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }      
  
  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
 
  return(success);
}

gboolean
ags_functional_test_util_machine_properties_lv2_filename(guint nth_machine,
							 guint pad, guint audio_channel,
							 gchar *filename)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsListingEditor *listing_editor;
  AgsPadEditor *pad_editor;
  AgsLineEditor *line_editor;
  AgsLineMemberEditor *member_editor;
  AgsPluginBrowser *plugin_browser;
  AgsLv2Browser *lv2_browser;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *list_start, *list;

  gchar *value;

  guint nth_tab;
  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* get tab */
  ags_test_enter();

  machine_editor = machine->properties;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      listing_editor = AGS_LISTING_EDITOR(machine_editor->output_editor);
    }
    break;
  case 1:
    {
      listing_editor = AGS_LISTING_EDITOR(machine_editor->input_editor);
    }
    break;
  default:
    ags_test_leave();

    return(FALSE);
  }

  list_start = gtk_container_get_children(listing_editor->child);
  list = g_list_nth(list_start,
		    pad);

  if(list != NULL &&
     AGS_IS_PAD_EDITOR(list->data)){
    pad_editor = list->data;

    g_list_free(list_start);
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }
  
  /* line editor */
  list_start = gtk_container_get_children(pad_editor->line_editor);
  list = g_list_nth(list_start,
		    audio_channel);

  if(list != NULL &&
     AGS_IS_LINE_EDITOR(list->data)){
    line_editor = list->data;

    g_list_free(list_start);
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }

  /* line member editor */
  member_editor = line_editor->member_editor;
  plugin_browser = member_editor->plugin_browser;
  lv2_browser = plugin_browser->lv2_browser;
  
  /* set plugin filename */
  model = gtk_combo_box_get_model(GTK_COMBO_BOX(lv2_browser->filename));
  success = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);
      
      if(!g_strcmp0(filename,
		    value)){
	gtk_combo_box_set_active_iter((GtkComboBox *) lv2_browser->filename,
				      &iter);
	success = TRUE;
	
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }      
  
  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
 
  return(success);
}

gboolean
ags_functional_test_util_machine_properties_lv2_effect(guint nth_machine,
						       guint pad, guint audio_channel,
						       gchar *effect)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsListingEditor *listing_editor;
  AgsPadEditor *pad_editor;
  AgsLineEditor *line_editor;
  AgsLineMemberEditor *member_editor;
  AgsPluginBrowser *plugin_browser;
  AgsLv2Browser *lv2_browser;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *list_start, *list;

  gchar *value;

  guint nth_tab;
  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* get tab */
  ags_test_enter();

  machine_editor = machine->properties;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      listing_editor = AGS_LISTING_EDITOR(machine_editor->output_editor);
    }
    break;
  case 1:
    {
      listing_editor = AGS_LISTING_EDITOR(machine_editor->input_editor);
    }
    break;
  default:
    ags_test_leave();

    return(FALSE);
  }

  list_start = gtk_container_get_children(listing_editor->child);
  list = g_list_nth(list_start,
		    pad);

  if(list != NULL &&
     AGS_IS_PAD_EDITOR(list->data)){
    pad_editor = list->data;

    g_list_free(list_start);
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }
  
  /* line editor */
  list_start = gtk_container_get_children(pad_editor->line_editor);
  list = g_list_nth(list_start,
		    audio_channel);

  if(list != NULL &&
     AGS_IS_LINE_EDITOR(list->data)){
    line_editor = list->data;

    g_list_free(list_start);
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }

  /* line member editor */
  member_editor = line_editor->member_editor;
  plugin_browser = member_editor->plugin_browser;
  lv2_browser = plugin_browser->lv2_browser;
  
  /* set plugin effect */
  model = gtk_combo_box_get_model(GTK_COMBO_BOX(lv2_browser->effect));
  success = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);
      
      if(!g_strcmp0(effect,
		    value)){
	gtk_combo_box_set_active_iter((GtkComboBox *) lv2_browser->effect,
				      &iter);
	success = TRUE;
	
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }      
  
  ags_test_leave();

  ags_functional_test_util_reaction_time_long();
 
  return(success);
}

gboolean
ags_functional_test_util_machine_properties_bulk_add(guint nth_machine)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsPropertyCollectionEditor *property_collection_editor;
  GtkButton *add_collection;

  GList *list_start, *list;

  guint nth_tab;
  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* get tab */
  ags_test_enter();

  machine_editor = machine->properties;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_BULK_OUTPUT_TAB:
    {
      property_collection_editor = AGS_PROPERTY_COLLECTION_EDITOR(machine_editor->output_link_editor);
    }
    break;
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_BULK_INPUT_TAB:
    {
      property_collection_editor = AGS_PROPERTY_COLLECTION_EDITOR(machine_editor->input_link_editor);
    }
    break;
  default:
    ags_test_leave();

    return(FALSE);
  }

  add_collection = property_collection_editor->add_collection;
  
  ags_test_leave();

  /* add collection */
  success = ags_functional_test_util_button_click(add_collection);

  ags_functional_test_util_reaction_time_long();

  return(success);
}

gboolean
ags_functional_test_util_machine_properties_bulk_remove(guint nth_machine,
							guint nth_bulk)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_bulk_link(guint nth_machine,
						      guint nth_bulk,
						      gchar *link)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsPropertyCollectionEditor *property_collection_editor;
  AgsLinkCollectionEditor *link_collection_editor;

  GList *list_start, *list;

  GtkTreeModel *model;
  
  GtkTreeIter iter;

  gchar *value;

  guint nth_tab;
  gboolean success;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* get notebook tab */
  ags_test_enter();

  machine_editor = machine->properties;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_BULK_OUTPUT_TAB:
    {
      property_collection_editor = AGS_PROPERTY_COLLECTION_EDITOR(machine_editor->output_link_editor);
    }
    break;
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_BULK_INPUT_TAB:
    {
      property_collection_editor = AGS_PROPERTY_COLLECTION_EDITOR(machine_editor->input_link_editor);
    }
    break;
  default:
    ags_test_leave();

    return(FALSE);
  }

  /* get bulk editor */
  list_start = gtk_container_get_children(property_collection_editor->child);
  list = g_list_nth(list_start,
		    nth_bulk);

  if(list != NULL){
    GtkTable *table;
    
    table = list->data;
    g_list_free(list_start);

    link_collection_editor = g_object_get_data(table,
					       "AgsChild\0");
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }
  
  /* set link */
  model = gtk_combo_box_get_model(GTK_COMBO_BOX(link_collection_editor->link));
  success = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);
      
      if(!g_strcmp0(link,
		    value)){
	gtk_combo_box_set_active_iter((GtkComboBox *) link_collection_editor->link,
				      &iter);
	success = TRUE;
	
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }      
  
  ags_test_leave();

  ags_functional_test_util_reaction_time_long();

  return(success);
}

gboolean
ags_functional_test_util_machine_properties_bulk_first_line(guint nth_machine,
							    guint nth_bulk,
							    guint first_line)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsPropertyCollectionEditor *property_collection_editor;
  AgsLinkCollectionEditor *link_collection_editor;

  GList *list_start, *list;

  GtkTreeModel *model;
  
  GtkTreeIter iter;

  gchar *value;

  guint nth_tab;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* get notebook tab */
  ags_test_enter();

  machine_editor = machine->properties;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_BULK_OUTPUT_TAB:
    {
      property_collection_editor = AGS_PROPERTY_COLLECTION_EDITOR(machine_editor->output_link_editor);
    }
    break;
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_BULK_INPUT_TAB:
    {
      property_collection_editor = AGS_PROPERTY_COLLECTION_EDITOR(machine_editor->input_link_editor);
    }
    break;
  default:
    ags_test_leave();

    return(FALSE);
  }

  /* get bulk editor */
  list_start = gtk_container_get_children(property_collection_editor->child);
  list = g_list_nth(list_start,
		    nth_bulk);

  if(list != NULL){
    GtkTable *table;
    
    table = list->data;
    g_list_free(list_start);

    link_collection_editor = g_object_get_data(table,
					       "AgsChild\0");
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }

  gtk_spin_button_set_value(link_collection_editor->first_line,
			    first_line);

  ags_test_leave();

  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_properties_bulk_link_line(guint nth_machine,
							   guint nth_bulk,
							   guint first_link_line)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsPropertyCollectionEditor *property_collection_editor;
  AgsLinkCollectionEditor *link_collection_editor;

  GList *list_start, *list;

  GtkTreeModel *model;
  
  GtkTreeIter iter;

  gchar *value;

  guint nth_tab;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* get notebook tab */
  ags_test_enter();

  machine_editor = machine->properties;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_BULK_OUTPUT_TAB:
    {
      property_collection_editor = AGS_PROPERTY_COLLECTION_EDITOR(machine_editor->output_link_editor);
    }
    break;
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_BULK_INPUT_TAB:
    {
      property_collection_editor = AGS_PROPERTY_COLLECTION_EDITOR(machine_editor->input_link_editor);
    }
    break;
  default:
    ags_test_leave();

    return(FALSE);
  }

  /* get bulk editor */
  list_start = gtk_container_get_children(property_collection_editor->child);
  list = g_list_nth(list_start,
		    nth_bulk);

  if(list != NULL &&
     AGS_IS_LINK_COLLECTION_EDITOR(list->data)){
    GtkTable *table;
    
    table = list->data;
    g_list_free(list_start);

    link_collection_editor = g_object_get_data(table,
					       "AgsChild\0");
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }

  gtk_spin_button_set_value(link_collection_editor->first_link,
			    first_link_line);

  ags_test_leave();

  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_properties_bulk_count(guint nth_machine,
						       guint nth_bulk,
						       guint count)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsPropertyCollectionEditor *property_collection_editor;
  AgsLinkCollectionEditor *link_collection_editor;

  GList *list_start, *list;

  GtkTreeModel *model;
  
  GtkTreeIter iter;

  gchar *value;

  guint nth_tab;
  
  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* get notebook tab */
  ags_test_enter();

  machine_editor = machine->properties;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_BULK_OUTPUT_TAB:
    {
      property_collection_editor = AGS_PROPERTY_COLLECTION_EDITOR(machine_editor->output_link_editor);
    }
    break;
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_BULK_INPUT_TAB:
    {
      property_collection_editor = AGS_PROPERTY_COLLECTION_EDITOR(machine_editor->input_link_editor);
    }
    break;
  default:
    ags_test_leave();

    return(FALSE);
  }

  /* get bulk editor */
  list_start = gtk_container_get_children(property_collection_editor->child);
  list = g_list_nth(list_start,
		    nth_bulk);

  if(list != NULL){
    GtkTable *table;
    
    table = list->data;
    g_list_free(list_start);

    link_collection_editor = g_object_get_data(table,
					       "AgsChild\0");
  }else{
    g_list_free(list_start);

    ags_test_leave();

    return(FALSE);
  }

  gtk_spin_button_set_value(link_collection_editor->count,
			    count);

  ags_test_leave();

  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_properties_resize_audio_channels(guint nth_machine,
								  guint audio_channels)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  
  GList *list_start, *list;

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* resize audio channels */
  ags_test_enter();

  machine_editor = machine->properties;
  gtk_spin_button_set_value(machine_editor->resize_editor->audio_channels,
			    audio_channels);

  ags_test_leave();

  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_properties_resize_inputs(guint nth_machine,
							  guint inputs)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  
  GList *list_start, *list;

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* resize input pads */
  ags_test_enter();

  machine_editor = machine->properties;
  gtk_spin_button_set_value(machine_editor->resize_editor->input_pads,
			    inputs);

  ags_test_leave();

  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_properties_resize_outputs(guint nth_machine,
							   guint outputs)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  
  GList *list_start, *list;

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* resize output pads */
  ags_test_enter();

  machine_editor = machine->properties;
  gtk_spin_button_set_value(machine_editor->resize_editor->output_pads,
			    outputs);

  ags_test_leave();

  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_edit_copy(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_line_member_set_toggled(gchar *specifier,
						 gboolean is_toggled)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_line_member_set_value(gchar *specifier,
					       gdouble value)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_audio_connection_click_tab(guint nth_machine,
							    guint nth_tab)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_audio_connection_click_enable(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_audio_connection_set_device(guint nth_machine,
							     guint pad, guint audio_channel,
							     gchar *device)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_audio_connection_set_line(guint nth_machine,
							   uint pad, guint audio_channel,
							   guint line)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_audio_connection_bulk_add(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_audio_connection_bulk_remove(guint nth_machine,
							      guint nth_bulk)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_audio_connection_bulk_first_line(guint nth_machine,
								  guint nth_bulk,
								  guint first_line)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_audio_connection_bulk_count(guint nth_machine,
							     guint nth_bulk,
							     guint count)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_audio_connection_bulk_device(guint nth_machine,
							      guint nth_bulk,
							      gchar *device)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_midi_connection_midi_channel(guint nth_machine,
							      guint midi_channel)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_midi_connection_audio_start_mapping(guint nth_machine,
								     guint audio_start_mapping)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_midi_connection_audio_end_mapping(guint nth_machine,
								   guint audio_end_mapping)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_midi_connection_midi_start_mapping(guint nth_machine,
								    guint midi_start_mapping)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_midi_connection_midi_end_mapping(guint nth_machine,
								  guint midi_end_mapping)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_midi_connection_midi_device(guint nth_machine,
							     gchar *device)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_pad_group(guint nth_machine,
				   guint nth_pad)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_pad_mute(guint nth_machine,
				  guint nth_pad)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_pad_solo(guint nth_machine,
				  guint nth_pad)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_pad_play(guint nth_machine,
				  guint nth_pad)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_expander(guint nth_machine,
				  guint nth_pad, guint nth_audio_channel)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_effect_bulk_add(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_effect_bulk_remove(guint nth_machine,
					    guint nth_bulk_effect)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_effect_bulk_ladspa_filename(guint nth_machine,
						     gchar *filename)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_effect_bulk_ladspa_effect(guint nth_machine,
						   gchar *effect)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_effect_bulk_lv2_filename(guint nth_machine,
						  gchar *filename)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_effect_bulk_lv2_effect(guint nth_machine,
						gchar *effect)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_panel_mute_line(guint nth_machine,
					 guint nth_pad, guint nth_audio_channel)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_mixer_input_line_volume(guint nth_machine,
						 guint nth_pad, guint nth_audio_channel,
						 gdouble volume)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_open(guint nth_machine)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsDrum *drum;
  GtkButton *open_button;
  GtkWidget **open_dialog;
  
  GList *list_start, *list;
  
  gboolean success;

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve drum */
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_DRUM(list->data)){
    drum = list->data;
  }else{
    return(FALSE);
  }
  
  g_list_free(list_start);

  /* open dialog */
  ags_test_enter();
  
  open_button = drum->open;
  open_dialog = &(drum->open_dialog);
  
  ags_test_leave();

  /* click open */
  success = ags_functional_test_util_button_click(open_button);

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_test_util_default_timeout,
						      open_dialog);
  
  return(success);
}

gboolean
ags_functional_test_util_drum_run(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_loop(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_bank_0(guint nth_machine,
				     guint bank_0)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_bank_1(guint nth_machine,
				     guint bank_1)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_length(guint nth_machine,
				     guint length)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_index(guint nth_machine,
				    guint nth_radio)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_pad(guint nth_machine,
				  guint nth_pad)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_pad_open(guint nth_machine,
				       guint nth_pad)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_pad_edit(guint nth_machine,
				       guint nth_pad)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_input_line_volume(guint nth_machine,
						guint nth_pad, guint nth_audio_channel,
						gdouble volume)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_matrix_run(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_matrix_loop(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_matrix_bank_1(guint nth_machine,
				       guint bank_1)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_matrix_length(guint nth_machine,
				       guint length)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_matrix_gutter(guint nth_machine,
				       guint nth_gutter)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_cell(guint nth_machine,
				   guint x, guint y)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_synth_auto_update(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_synth_update(guint nth_machine)
{
  //TODO:JK: 
}

gboolean ags_functional_test_util_oscillator_wave(guint nth_machine,
						  guint nth_oscillator,
						  gchar *wave)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_oscillator_phase(guint nth_machine,
					  guint nth_oscillator,
					  gdouble phase)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_oscillator_attack(guint nth_machine,
					   guint nth_oscillator,
					   gdouble attack)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_oscillator_frequency(guint nth_machine,
					      guint nth_oscillator,
					      gdouble frequency)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_oscillator_length(guint nth_machine,
					   guint nth_oscillator,
					   gdouble length)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_oscillator_volume(guint nth_machine,
					   guint nth_oscillator,
					   gdouble volume)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_ffplayer_open(guint nth_machine)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsFFPlayer *ffplayer;
  GtkButton *open_button;
  GtkWidget **open_dialog;
  
  GList *list_start, *list;
  
  gboolean success;

  ags_test_enter();
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve ffplayer */
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  ags_test_leave();
  
  if(list != NULL &&
     AGS_IS_FFPLAYER(list->data)){
    ffplayer = list->data;
  }else{
    return(FALSE);
  }
  
  g_list_free(list_start);

  /* open dialog */
  ags_test_enter();
  
  open_button = ffplayer->open;
  open_dialog = &(ffplayer->open_dialog);

  ags_test_leave();

  /* click open */
  success = ags_functional_test_util_button_click(open_button);

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_test_util_default_timeout,
						      open_dialog);
  
  return(success);
}

gboolean ags_functional_test_util_ffplayer_preset(guint nth_machine,
						  gchar *preset)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_ffplayer_instrument(guint nth_machine,
					     gchar *instrument)
{
  //TODO:JK: 
}
