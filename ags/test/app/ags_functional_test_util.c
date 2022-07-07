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

#include <ags/test/app/ags_functional_test_util.h>

//FIXME:JK: conflict of KeyCode
#define __AGS_VST_KEYCODES_H__

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/gsequencer_main.h>

#include <ags/test/app/libgsequencer.h>

#include <gdk/gdk.h>
#include <gdk/gdkevents.h>

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

gboolean ags_functional_test_util_driver_timeout(gpointer data);

void ags_functional_test_util_idle_condition_and_timeout_driver_program(guint n_params,
									gchar **param_strv,
									GValue *param);
void ags_functional_test_util_leave_program(guint n_params,
					    gchar **param_strv,
					    GValue *param);

void ags_functional_test_util_header_bar_menu_button_click_driver_program(guint n_params,
									  gchar **param_strv,
									  GValue *param);

#ifdef AGS_FAST_FUNCTIONAL_TESTS
#define AGS_FUNCTIONAL_TEST_UTIL_REACTION_TIME (125000)
#define AGS_FUNCTIONAL_TEST_UTIL_REACTION_TIME_LONG (750000)
#else
#define AGS_FUNCTIONAL_TEST_UTIL_REACTION_TIME (1000000)
#define AGS_FUNCTIONAL_TEST_UTIL_REACTION_TIME_LONG (2000000)
#endif

extern AgsApplicationContext *ags_application_context;

extern AgsLadspaManager *ags_ladspa_manager;
extern AgsDssiManager *ags_dssi_manager;
extern AgsLv2Manager *ags_lv2_manager;
extern AgsLv2uiManager *ags_lv2ui_manager;

AgsTaskLauncher *task_launcher;

struct timespec ags_functional_test_util_default_timeout = {
  59,
  0,
};

gint64 driver_stamp = 0;
gint64 reaction_time_stamp = -1;
gint64 reaction_idle_time = 0;

gboolean test_driver_yield = FALSE;

GList *test_driver_program = NULL;

gboolean test_driver_program_glue = FALSE;

struct timespec idle_condition_start_time;

gboolean
ags_functional_test_util_driver_timeout(gpointer data)
{
  gint64 current_stamp;
  static const gint64 time_limit = 400;
  static const gint64 action_time = (gint64) (G_USEC_PER_SEC / 30.0);
  
  current_stamp = g_get_monotonic_time();

  do{
    if(reaction_time_stamp != -1){
      if(reaction_time_stamp + reaction_idle_time >= current_stamp){
	reaction_time_stamp = -1;

	reaction_idle_time = 0;
      }
    }
    
    if(reaction_time_stamp == -1){
      if(test_driver_program != NULL){
	AgsFunctionalTestUtilDriverProgram *driver_program;

	GList *list;

	gboolean success;

	list = g_list_last(test_driver_program);
	
	driver_program = list->data;

	driver_program->driver_program_func(driver_program->n_params,
					    driver_program->param_strv,
					    driver_program->param);
	
	if(!test_driver_program_glue){
	  test_driver_program = g_list_remove(test_driver_program,
					      driver_program);
	}else{
	  test_driver_program_glue = FALSE;
	}
      }
    }
  }while(!test_driver_yield && current_stamp + time_limit < driver_stamp + action_time && test_driver_program != NULL);

  if(current_stamp >= driver_stamp + action_time){
    driver_stamp = g_get_monotonic_time();
  }
  
  return(G_SOURCE_CONTINUE);
}

void
ags_functional_test_util_add_driver(gdouble timeout_s)
{
  g_timeout_add((guint) (1000.0 * timeout_s),
		(GSourceFunc) ags_functional_test_util_driver_timeout,
		NULL);
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
ags_functional_test_util_reaction_time()
{
  reaction_time_stamp = g_get_monotonic_time();

  reaction_idle_time = AGS_FUNCTIONAL_TEST_UTIL_REACTION_TIME;
}

void
ags_functional_test_util_reaction_time_long()
{
  reaction_time_stamp = g_get_monotonic_time();

  reaction_idle_time = AGS_FUNCTIONAL_TEST_UTIL_REACTION_TIME_LONG;
}

void
ags_functional_test_util_idle_driver_program(guint n_params,
					     gchar **param_strv,
					     GValue *param)
{
  gint64 time_stamp;
  gint64 idle_time;
  gint64 current_stamp;
  
  current_stamp = g_get_monotonic_time();

  time_stamp = g_value_get_int64(param);
  
  idle_time = g_value_get_int64(param + 1);

  if(time_stamp + idle_time > current_stamp){
    test_driver_program_glue = TRUE;

    test_driver_yield = TRUE;
  }else{
    g_usleep(4);
  }
}

void
ags_functional_test_util_idle(gint64 idle_time)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  gint64 time_stamp;
  
  time_stamp = g_get_monotonic_time();

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_idle_driver_program;
  
  driver_program->n_params = 2;

  /* param string vector */
  driver_program->param_strv = g_malloc(3 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("time_stamp");
  driver_program->param_strv[1] = g_strdup("idle_time");
  driver_program->param_strv[2] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 2);

  g_value_init(driver_program->param,
	       G_TYPE_INT64);
  g_value_set_int64(driver_program->param,
		    time_stamp);

  g_value_init(driver_program->param + 1,
	       G_TYPE_INT64);
  g_value_set_int64(driver_program->param + 1,
		    idle_time);
  
  test_driver_program = g_list_prepend(test_driver_program,
				       driver_program);
}

void
ags_functional_test_util_idle_condition_and_timeout_driver_program(guint n_params,
								   gchar **param_strv,
								   GValue *param)
{
  AgsFunctionalTestUtilIdleCondition idle_condition;

  struct timespec current_time;
  struct timespec *timeout;
  
  gpointer data;
  
  idle_condition = g_value_get_pointer(param);
  
  timeout = g_value_get_pointer(param + 1);
  
  data = g_value_get_pointer(param + 2);

  clock_gettime(CLOCK_MONOTONIC,
                &current_time);

  if(idle_condition(data)){
    g_usleep(4);    
  }else{
    if(idle_condition_start_time.tv_sec + timeout->tv_sec < current_time.tv_sec){
      test_driver_program_glue = TRUE;

      test_driver_yield = TRUE;
    }else{
      g_warning("test condition timeout");
    }
  }
}

void
ags_functional_test_util_idle_condition_and_timeout(AgsFunctionalTestUtilIdleCondition idle_condition,
						    struct timespec *timeout,
						    gpointer data)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_idle_condition_and_timeout_driver_program;
  
  driver_program->n_params = 3;

  /* param string vector */
  driver_program->param_strv = g_malloc(4 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("idle_condition");
  driver_program->param_strv[1] = g_strdup("timeout");
  driver_program->param_strv[2] = g_strdup("data");  
  driver_program->param_strv[3] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 3);

  g_value_init(driver_program->param,
	       G_TYPE_POINTER);
  g_value_set_pointer(driver_program->param,
		      idle_condition);
  
  g_value_init(driver_program->param + 1,
	       G_TYPE_POINTER);
  g_value_set_pointer(driver_program->param + 1,
		      timeout);

  g_value_init(driver_program->param + 2,
	       G_TYPE_POINTER);
  g_value_set_pointer(driver_program->param + 2,
		      data);

  clock_gettime(CLOCK_MONOTONIC,
		&idle_condition_start_time);

  test_driver_program = g_list_prepend(test_driver_program,
				       driver_program);
}

gboolean
ags_functional_test_util_idle_test_widget_visible(GtkWidget **widget)
{
  gboolean do_idle;

  do_idle = TRUE;
  
  if(*widget != NULL &&
     GTK_IS_WIDGET(*widget) &&
     gtk_widget_is_visible(*widget)){
    do_idle = FALSE;
  }
  
  return(do_idle);
}

gboolean
ags_functional_test_util_idle_test_widget_hidden(GtkWidget **widget)
{
  gboolean do_idle;

  do_idle = TRUE;
  
  if(*widget != NULL &&
     GTK_IS_WIDGET(*widget) &&
     !gtk_widget_is_visible(*widget)){
    do_idle = FALSE;
  }
  
  return(do_idle);
}

gboolean
ags_functional_test_util_idle_test_widget_realized(GtkWidget **widget)
{
  gboolean do_idle;

  do_idle = TRUE;
  
  if(*widget != NULL &&
     GTK_IS_WIDGET(*widget) &&
     gtk_widget_get_realized(*widget)){
    do_idle = FALSE;
  }
  
  return(do_idle);
}

gboolean
ags_functional_test_util_idle_test_null(GtkWidget **widget)
{
  gboolean do_idle;

  do_idle = TRUE;
  
  if(*widget == NULL){
    do_idle = FALSE;
  }
  
  return(do_idle);
}

gboolean
ags_functional_test_util_idle_test_list_length(AgsFunctionalTestUtilListLengthCondition *condition)
{
  gboolean do_idle;

  do_idle = TRUE;
  
  if(g_list_length(condition->start_list[0]) == condition->length){
    do_idle = FALSE;
  }
  
  return(do_idle);
}

void
ags_functional_test_util_leave_program(guint n_params,
				       gchar **param_strv,
				       GValue *param)
{
  GtkWidget *window;

  window = g_value_get_object(param);

  gtk_window_destroy(window);

  ags_functional_test_util_reaction_time_long();
}

void
ags_functional_test_util_leave(GtkWidget *window)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  if(!GTK_IS_WINDOW(window)){
    return;
  }

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_leave_program;
  
  driver_program->n_params = 1;

  /* param string vector */
  driver_program->param_strv = g_malloc(2 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("window");
  driver_program->param_strv[1] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 1);

  g_value_init(driver_program->param,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param,
		     window);

  test_driver_program = g_list_prepend(test_driver_program,
				       driver_program);
}

void
ags_functional_test_util_fake_mouse_warp(gpointer display, guint screen, guint x, guint y)
{
  static const gulong delay = 1;

  XTestFakeMotionEvent((Display *) display, screen, x, y, delay);
  XSync((Display *) display, 0);
}

void
ags_functional_test_util_fake_mouse_button_press(gpointer display, guint button)
{
  static const gulong delay = 1;
  
  XTestFakeButtonEvent((Display *) display, button, 1, delay);
  XFlush((Display *) display);
}

void
ags_functional_test_util_fake_mouse_button_release(gpointer display, guint button)
{
  static const gulong delay = 1;
  
  XTestFakeButtonEvent((Display *) display, button, 0, delay);
  XFlush((Display *) display);
}

void
ags_functional_test_util_fake_mouse_button_click(gpointer display, guint button)
{
  static const gulong delay = 1;
  
  XTestFakeButtonEvent((Display *) display, button, 1, delay);
  XTestFakeButtonEvent((Display *) display, button, 0, delay);
  XFlush((Display *) display);
}

void
ags_functional_test_util_header_bar_menu_button_click_driver_program(guint n_params,
								     gchar **param_strv,
								     GValue *param)
{
  GtkMenuButton *menu_button;
  
  gchar **item_path;
  gchar *action;

  gboolean success;

  menu_button = g_value_get_object(param);
  
  item_path = g_value_get_pointer(param + 1);
  
  action = g_value_get_string(param + 2);

  success = TRUE;
  
  if(!g_ascii_strncasecmp(action, "app.open", 9)){
    ags_app_action_util_open();
  }else if(!g_ascii_strncasecmp(action, "app.save", 9)){
    ags_app_action_util_save();
  }else if(!g_ascii_strncasecmp(action, "app.save_as", 12)){
    ags_app_action_util_save_as();
  }else if(!g_ascii_strncasecmp(action, "app.export", 11)){
    ags_app_action_util_export();
  }else if(!g_ascii_strncasecmp(action, "app.meta_data", 14)){
    ags_app_action_util_meta_data();
  }else if(!g_ascii_strncasecmp(action, "app.smf_import", 15)){
    ags_app_action_util_smf_import();
  }else if(!g_ascii_strncasecmp(action, "app.smf_export", 15)){
    ags_app_action_util_smf_export();
  }else if(!g_ascii_strncasecmp(action, "app.preferences", 16)){
    ags_app_action_util_preferences();
  }else if(!g_ascii_strncasecmp(action, "app.about", 10)){
    ags_app_action_util_about();
  }else if(!g_ascii_strncasecmp(action, "app.help", 9)){
    ags_app_action_util_help();
  }else if(!g_ascii_strncasecmp(action, "app.quit", 9)){
    ags_app_action_util_quit();
  }else if(!g_ascii_strncasecmp(action, "app.add_panel", 14)){
    ags_app_action_util_add_panel();
  }else if(!g_ascii_strncasecmp(action, "app.add_spectrometer", 20)){
    ags_app_action_util_add_spectrometer();
  }else if(!g_ascii_strncasecmp(action, "app.add_equalizer", 18)){
    ags_app_action_util_add_equalizer();
  }else if(!g_ascii_strncasecmp(action, "app.add_mixer", 13)){
    ags_app_action_util_add_mixer();
  }else if(!g_ascii_strncasecmp(action, "app.add_drum", 12)){
    ags_app_action_util_add_drum();
  }else if(!g_ascii_strncasecmp(action, "app.add_matrix", 14)){
    ags_app_action_util_add_matrix();
  }else if(!g_ascii_strncasecmp(action, "app.add_synth", 13)){
    ags_app_action_util_add_synth();
  }else if(!g_ascii_strncasecmp(action, "app.add_fm_synth", 16)){
    ags_app_action_util_add_fm_synth();
  }else if(!g_ascii_strncasecmp(action, "app.add_syncsynth", 17)){
    ags_app_action_util_add_syncsynth();
  }else if(!g_ascii_strncasecmp(action, "app.add_fm_syncsynth", 20)){
    ags_app_action_util_add_fm_syncsynth();
  }else if(!g_ascii_strncasecmp(action, "app.add_hybrid_synth", 20)){
    ags_app_action_util_add_hybrid_synth();
  }else if(!g_ascii_strncasecmp(action, "app.add_hybrid_fm_synth", 23)){
    ags_app_action_util_add_hybrid_fm_synth();
  }else if(!g_ascii_strncasecmp(action, "app.add_ffplayer", 16)){
    ags_app_action_util_add_ffplayer();
  }else if(!g_ascii_strncasecmp(action, "app.add_sf2_synth", 17)){
    ags_app_action_util_add_sf2_synth();
  }else if(!g_ascii_strncasecmp(action, "app.add_pitch_sampler", 21)){
    ags_app_action_util_add_pitch_sampler();
  }else if(!g_ascii_strncasecmp(action, "app.add_sfz_synth", 17)){
    ags_app_action_util_add_sfz_synth();
  }else if(!g_ascii_strncasecmp(action, "app.add_audiorec", 16)){
    ags_app_action_util_add_audiorec();
  }else{
    success = FALSE;
  }

  if(!success){
    g_warning("unknown action");
  }
}

void
ags_functional_test_util_header_bar_menu_button_click(GtkMenuButton *menu_button,
						      gchar **item_path,
						      gchar *action)
{
  AgsFunctionalTestUtilDriverProgram *driver_program;

  if(!GTK_IS_MENU_BUTTON(menu_button)){
    return;
  }

  driver_program = g_new0(AgsFunctionalTestUtilDriverProgram,
			  1);

  driver_program->driver_program_func = ags_functional_test_util_leave_program;
  
  driver_program->n_params = 3;

  /* param string vector */
  driver_program->param_strv = g_malloc(4 * sizeof(gchar *));

  driver_program->param_strv[0] = g_strdup("window");
  driver_program->param_strv[1] = g_strdup("item_path");
  driver_program->param_strv[2] = g_strdup("action");
  driver_program->param_strv[3] = NULL;

  /* param value array */
  driver_program->param = g_new0(GValue,
				 3);

  g_value_init(driver_program->param,
	       G_TYPE_OBJECT);
  g_value_set_object(driver_program->param,
		     menu_button);

  g_value_init(driver_program->param,
	       G_TYPE_POINTER);
  g_value_set_pointer(driver_program->param,
		      item_path);

  g_value_init(driver_program->param,
	       G_TYPE_STRING);
  g_value_set_string(driver_program->param,
		     action);
  
  test_driver_program = g_list_prepend(test_driver_program,
				       driver_program);
}

gboolean
ags_functional_test_util_combo_box_click(GtkComboBox *combo_box,
					 guint nth)
{
  GtkWidget *widget;

  GtkAllocation allocation;
  
  gint x, y;
  gint origin_x, origin_y;	
  gint position_x, position_y;
  
  if(combo_box == NULL ||
     !GTK_IS_COMBO_BOX(combo_box)){
    return(FALSE);
  }

  widget = combo_box;

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_realized),
						      &ags_functional_test_util_default_timeout,
						      &widget);
  
  /*  */
  gtk_widget_get_allocation(widget,
			    &allocation);
  
  x = allocation.x;
  y = allocation.y;

  position_x = allocation.width / 2.0;
  position_y = allocation.height / 2.0;

  ags_functional_test_util_reaction_time();
  
  /*  */

  gtk_combo_box_set_active(combo_box,
			   nth);
	
  ags_functional_test_util_reaction_time_long();
  
  return(TRUE);
}

gboolean
ags_functional_test_util_button_click(GtkButton *button)
{
  GtkWidget *widget;

  GtkAllocation allocation;
  
  gint x, y;
  gint origin_x, origin_y;
  gint position_x, position_y;
  gboolean is_realized;
  
  if(button == NULL ||
     !GTK_IS_BUTTON(button)){
    return(FALSE);
  }
  
  widget = button;

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_realized),
						      &ags_functional_test_util_default_timeout,
						      &widget);
  
  /*  */
  gtk_widget_get_allocation(widget,
			    &allocation);
  
  x = allocation.x;
  y = allocation.y;

  position_x = allocation.width / 2.0;
  position_y = allocation.height / 2.0;

#if 0
  //TODO:JK: implement me
#else
  /* emit signal */
  g_signal_emit_by_name(widget,
			"clicked");
  #endif
  
  ags_functional_test_util_reaction_time_long();
  
  return(TRUE);
}

gboolean
ags_functional_test_util_toggle_button_click(GtkToggleButton *button)
{
  GtkWidget *widget;

  GtkAllocation allocation;
  
  gint x, y;
  gint origin_x, origin_y;
  gint position_x, position_y;
  gboolean is_realized;
  
  if(button == NULL ||
     !GTK_IS_BUTTON(button)){
    return(FALSE);
  }
  
  widget = button;

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_realized),
						      &ags_functional_test_util_default_timeout,
						      &widget);
  
  /*  */
  gtk_widget_get_allocation(widget,
			    &allocation);
  
  x = allocation.x;
  y = allocation.y;

  position_x = allocation.width / 2.0;
  position_y = allocation.height / 2.0;

#if 0
  //TODO:JK: implement me
#else
  /* emit signal */
  g_signal_emit_by_name(widget,
			"toggled");
  #endif
  
  ags_functional_test_util_reaction_time_long();
  
  return(TRUE);
}

gboolean
ags_functional_test_util_check_button_click(GtkCheckButton *button)
{
  GtkWidget *widget;

  GtkAllocation allocation;
  
  gint x, y;
  gint origin_x, origin_y;
  gint position_x, position_y;
  gboolean is_realized;
  
  if(button == NULL ||
     !GTK_IS_BUTTON(button)){
    return(FALSE);
  }
  
  widget = button;

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_realized),
						      &ags_functional_test_util_default_timeout,
						      &widget);
  
  /*  */
  gtk_widget_get_allocation(widget,
			    &allocation);
  
  x = allocation.x;
  y = allocation.y;

  position_x = allocation.width / 2.0;
  position_y = allocation.height / 2.0;

#if 0
  //TODO:JK: implement me
#else
  /* emit signal */
  g_signal_emit_by_name(widget,
			"toggled");
  #endif
  
  ags_functional_test_util_reaction_time_long();
  
  return(TRUE);
}

gboolean
ags_functional_test_util_menu_button_click(GtkMenuButton *button)
{
  GtkWidget *widget;

  GtkAllocation allocation;
  
  gint x, y;
  gint origin_x, origin_y;
  gint position_x, position_y;
	
  if(button == NULL ||
     !GTK_IS_MENU_BUTTON(button)){
    return(FALSE);
  }
  
  widget = button;

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_realized),
						      &ags_functional_test_util_default_timeout,
						      &widget);

  gtk_widget_get_allocation(widget,
			    &allocation);  

  x = allocation.x;
  y = allocation.y;

  position_x = allocation.width / 2.0;
  position_y = allocation.height / 2.0;

#if 0
  /* warp and click */
  //TODO:JK: implement me
#else
  /* emit signal */  
  ags_functional_test_util_reaction_time_long();

  /*  */
  g_signal_emit_by_name(widget,
			"activate");
  #endif

  ags_functional_test_util_reaction_time_long();
  
  return(TRUE);
}

gboolean
ags_functional_test_util_dialog_apply(GtkDialog *dialog)
{
  if(!GTK_IS_DIALOG(dialog)){
    return(FALSE);
  }

  gtk_dialog_response(dialog,
		      GTK_RESPONSE_APPLY);

  ags_functional_test_util_reaction_time_long();
  
  return(TRUE);
}

gboolean
ags_functional_test_util_dialog_ok(GtkDialog *dialog)
{
  GtkButton *ok_button;

  gboolean success;
  
  if(!GTK_IS_DIALOG(dialog)){
    return(FALSE);
  }

  if(ok_button == NULL){
    return(FALSE);
  }

  ok_button = gtk_dialog_get_widget_for_response(dialog,
						 GTK_RESPONSE_OK);
  
  success = ags_functional_test_util_button_click(ok_button);

  ags_functional_test_util_reaction_time_long();
    
  return(success);
}

gboolean
ags_functional_test_util_dialog_cancel(GtkDialog *dialog)
{
  GtkButton *cancel_button;

  gboolean success;
  
  if(!GTK_IS_DIALOG(dialog)){
    return(FALSE);
  }

  cancel_button = gtk_dialog_get_widget_for_response(dialog,
						     GTK_RESPONSE_CANCEL);
  
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

  gtk_file_chooser_set_current_folder(file_chooser,
				      g_file_new_for_path(path),
				      NULL);

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

  gtk_file_chooser_set_file(file_chooser,
			    g_file_new_for_path(filename),
			    NULL);

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

  while(filename != NULL){
    gtk_file_chooser_set_file(file_chooser,
			      g_file_new_for_path(filename->data),
			      NULL);

    filename = filename->next;
  }
  
  ags_functional_test_util_reaction_time_long();
  
  return(TRUE); 
}

gboolean
ags_functional_test_util_file_default_window_resize()
{
  AgsWindow *window;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  gtk_window_set_default_size(window,
			      1920 - 128, 1080 - 64);
    
  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_open()
{
  AgsWindow *window;
  
  AgsApplicationContext *application_context;

  gboolean success;

  static gchar* path_strv[] = {
    "_Open",
    NULL
  };

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  success = TRUE;

  ags_functional_test_util_header_bar_menu_button_click(window->app_button,
							path_strv,
							"app.open");

  ags_functional_test_util_reaction_time_long();
  
  return(success); 
}

gboolean
ags_functional_test_util_save()
{
  AgsWindow *window;
  
  AgsApplicationContext *application_context;

  gboolean success;

  static gchar* path_strv[] = {
    "_Save",
    NULL
  };

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  success = TRUE;

  ags_functional_test_util_header_bar_menu_button_click(window->app_button,
							path_strv,
							"app.save");

  ags_functional_test_util_reaction_time_long();
  
  return(success); 
}

gboolean
ags_functional_test_util_save_as()
{
  AgsWindow *window;
  
  AgsApplicationContext *application_context;

  gboolean success;

  static gchar* path_strv[] = {
    "Save as",
    NULL
  };

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  success = TRUE;

  ags_functional_test_util_header_bar_menu_button_click(window->app_button,
							path_strv,
							"app.save_as");

  ags_functional_test_util_reaction_time_long();
  
  return(success); 
}

gboolean
ags_functional_test_util_export_open()
{
  AgsWindow *window;
  
  AgsApplicationContext *application_context;

  gboolean success;

  static gchar* path_strv[] = {
    "_Export",
    NULL
  };

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  success = TRUE;

  ags_functional_test_util_header_bar_menu_button_click(window->app_button,
							path_strv,
							"app.export");

  ags_functional_test_util_reaction_time_long();
  
  return(success); 
}

gboolean
ags_functional_test_util_export_close()
{
  AgsExportWindow *export_window;
  
  AgsApplicationContext *application_context;

  gboolean success;

  static gchar* path_strv[] = {
    "_Export",
    NULL
  };

  application_context = ags_application_context_get_instance();

  export_window = ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context));

  success = ags_functional_test_util_dialog_close(export_window);
  
  ags_functional_test_util_reaction_time_long();
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_hidden),
						      &ags_functional_test_util_default_timeout,
						      &export_window);
  
  return(success); 
}

gboolean
ags_functional_test_util_export_add()
{
  AgsExportWindow *export_window;
  GtkButton *add_button;

  AgsApplicationContext *application_context;

  AgsFunctionalTestUtilListLengthCondition condition;

  gboolean success;

  application_context = ags_application_context_get_instance();

  export_window = ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context));

  add_button = export_window->add;

  condition.start_list = &(export_window->export_soundcard);

  condition.length = g_list_length(export_window->export_soundcard) + 1;
    
  success = ags_functional_test_util_button_click(add_button);

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
						      &ags_functional_test_util_default_timeout,
						      &condition);
  
  return(success);
}

gboolean
ags_functional_test_util_export_tact(gdouble tact)
{
  AgsExportWindow *export_window;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  export_window = ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context));

  gtk_spin_button_set_value(export_window->tact,
			    tact);

  ags_functional_test_util_reaction_time_long();
  
  return(TRUE);
}

gboolean
ags_functional_test_util_export_remove(guint nth)
{
  AgsExportWindow *export_window;
  GtkButton *remove_button;

  AgsApplicationContext *application_context;

  AgsFunctionalTestUtilListLengthCondition condition;

  GList *list;

  guint i;
  gboolean success;

  application_context = ags_application_context_get_instance();

  export_window = ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context));

  condition.start_list = &(export_window->export_soundcard);

  condition.length = g_list_length(export_window->export_soundcard) - 1;

  success = FALSE;

  if(export_window->export_soundcard != NULL){
    remove_button = NULL;
  
    list = export_window->export_soundcard;

    for(i = 0; list != NULL; i++){
      if(i == nth){
	remove_button = AGS_EXPORT_SOUNDCARD(list->data)->remove_button;

	success = TRUE;
      
	break;
      }

      list = list->next;
    }
  
    g_list_free(list);
  
    if(!success){
      return(FALSE);
    }
    
    success = ags_functional_test_util_button_click(remove_button);
    ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
							&ags_functional_test_util_default_timeout,
							&condition);
  }
  
  return(success);
}

gboolean
ags_functional_test_util_export_set_backend(guint nth,
					    gchar *backend)
{
  AgsExportWindow *export_window;

  AgsApplicationContext *application_context;

  GList *list_start, *list;

  guint i;
  gboolean success;

  application_context = ags_application_context_get_instance();

  export_window = ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context));

  list = export_window->export_soundcard;
  
  success = FALSE;

  for(i = 0; list != NULL; i++){
    if(i == nth){
      AgsExportSoundcard *export_soundcard;
      GtkTreeModel *model;

      GtkTreeIter iter;

      gchar *value;

      guint active;
      
      export_soundcard = AGS_EXPORT_SOUNDCARD(list->data);

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
  
  ags_functional_test_util_reaction_time_long();
  
  return(success);
}

gboolean
ags_functional_test_util_export_set_device(guint nth,
					   gchar *device)
{
  AgsExportWindow *export_window;

  AgsApplicationContext *application_context;

  GList *list;

  guint i;
  gboolean success;

  application_context = ags_application_context_get_instance();

  export_window = ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context));

  list = export_window->export_soundcard;
  
  success = FALSE;

  for(i = 0; list != NULL; i++){
    if(i == nth){
      AgsExportSoundcard *export_soundcard;
      GtkTreeModel *model;

      GtkTreeIter iter;

      gchar *value;

      guint active;

      export_soundcard = AGS_EXPORT_SOUNDCARD(list->data);

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
  
  ags_functional_test_util_reaction_time_long();
  
  return(success);
}

gboolean
ags_functional_test_util_export_set_filename(guint nth,
					     gchar *filename)
{
  AgsExportWindow *export_window;

  AgsApplicationContext *application_context;

  GList *list;

  guint i;
  gboolean success;

  application_context = ags_application_context_get_instance();

  export_window = ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context));

  list = export_window->export_soundcard;
  success = FALSE;

  for(i = 0; list != NULL; i++){
    if(i == nth){
      AgsExportSoundcard *export_soundcard;
      
      export_soundcard = AGS_EXPORT_SOUNDCARD(list->data);

      gtk_editable_set_text(GTK_EDITABLE(export_soundcard->filename),
			    filename);
      
      success = TRUE;
      
      break;
    }
    
    list = list->next;
  }
  
  ags_functional_test_util_reaction_time_long();
  
  return(success);
}

gboolean
ags_functional_test_util_export_nth(guint nth)
{
  //TODO:JK:

  return(TRUE);
}

gboolean
ags_functional_test_util_export_set_format(guint nth,
					   gchar *format)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_export_do_export(guint nth,
					  gchar *format)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_quit()
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_add_machine(gchar *submenu,
				     gchar *machine_name)
{
  AgsWindow *window;
  
  AgsApplicationContext *application_context;

  gboolean success;

  gchar **path_strv;
  gchar *action;

  path_strv = NULL;
  action = NULL;
  
  if(submenu == NULL){
    if(!g_ascii_strncasecmp(machine_name,
			    "Panel",
			    6)){
      static gchar* panel_path[] = {
	"Panel",
	NULL
      };
      
      path_strv = panel_path;
      
      action = "app.add_panel";
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Spectrometer",
				  13)){
      static gchar* spectrometer_path[] = {
	"Spectrometer",
	NULL
      };
      
      path_strv = spectrometer_path;
      
      action = "app.add_spectrometer";
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Equalizer",
				  10)){
      static gchar* equalizer_path[] = {
	"Equalizer",
	NULL
      };
      
      path_strv = equalizer_path;
      
      action = "app.add_equalizer";
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Mixer",
				  6)){
      static gchar* mixer_path[] = {
	"Mixer",
	NULL
      };
      
      path_strv = mixer_path;
      
      action = "app.add_mixer";
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Drum",
				  5)){
      static gchar* drum_path[] = {
	"Drum",
	NULL
      };
      
      path_strv = drum_path;
      
      action = "app.add_drum";
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Matrix",
				  13)){
      static gchar* matrix_path[] = {
	"Matrix",
	NULL
      };
      
      path_strv = matrix_path;
      
      action = "app.add_matrix";
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Synth",
				  6)){
      static gchar* synth_path[] = {
	"Synth",
	NULL
      };
      
      path_strv = synth_path;
      
      action = "app.add_synth";
    }else if(!g_ascii_strncasecmp(machine_name,
				  "FM Synth",
				  9)){
      static gchar* fm_synth_path[] = {
	"FM Synth",
	NULL
      };
      
      path_strv = fm_synth_path;
      
      action = "app.add_fm_synth";
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Syncsynth",
				  10)){
      static gchar* syncsynth_path[] = {
	"Syncsynth",
	NULL
      };
      
      path_strv = syncsynth_path;
      
      action = "app.add_syncsynth";
    }else if(!g_ascii_strncasecmp(machine_name,
				  "FM Syncsynth",
				  13)){
      static gchar* fm_syncsynth_path[] = {
	"FM Syncsynth",
	NULL
      };
      
      path_strv = fm_syncsynth_path;
      
      action = "app.add_fm_syncsynth";
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Hybrid Synth",
				  13)){
      static gchar* hybrid_synth_path[] = {
	"Hybrid Synth",
	NULL
      };
      
      path_strv = hybrid_synth_path;
      
      action = "app.add_hybrid_synth";
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Hybrid FM Synth",
				  16)){
      static gchar* hybrid_fm_synth_path[] = {
	"Hybrid FM Synth",
	NULL
      };
      
      path_strv = hybrid_fm_synth_path;
      
      action = "app.add_hybrid_fm_synth";
    }else if(!g_ascii_strncasecmp(machine_name,
				  "FPlayer",
				  8)){
      static gchar* ffplayer_path[] = {
	"FPlayer",
	NULL
      };
      
      path_strv = ffplayer_path;
      
      action = "app.add_ffplayer";
    }else if(!g_ascii_strncasecmp(machine_name,
				  "SF2 Synth",
				  10)){
      static gchar* sf2_synth_path[] = {
	"SF2 Synth",
	NULL
      };
      
      path_strv = sf2_synth_path;
      
      action = "app.add_sf2_synth";
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Pitch Sampler",
				  8)){
      static gchar* pitch_sampler_path[] = {
	"FPlayer",
	NULL
      };
      
      path_strv = pitch_sampler_path;
      
      action = "app.add_pitch_sampler";
    }else if(!g_ascii_strncasecmp(machine_name,
				  "SFZ Synth",
				  10)){
      static gchar* sfz_synth_path[] = {
	"SFZ Synth",
	NULL
      };
      
      path_strv = sfz_synth_path;
      
      action = "app.add_sfz_synth";
    }else if(!g_ascii_strncasecmp(machine_name,
				  "Audiorec",
				  9)){
      static gchar* audiorec_path[] = {
	"SF2 Synth",
	NULL
      };
      
      path_strv = audiorec_path;
      
      action = "app.add_audiorec";
    }
  }
    
  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  success = TRUE;

  ags_functional_test_util_header_bar_menu_button_click(window->add_button,
							path_strv,
							action);
  
  ags_functional_test_util_reaction_time_long();
  
  return(success);
}

gboolean
ags_functional_test_util_automation_open()
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_automation_close()
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_preferences_open()
{
  AgsWindow *window;
  
  AgsApplicationContext *application_context;

  gboolean success;

  static gchar* path_strv[] = {
    "_Preferences",
    NULL
  };

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  success = TRUE;

  ags_functional_test_util_header_bar_menu_button_click(window->app_button,
							path_strv,
							"app.preferences");

  ags_functional_test_util_reaction_time_long();
  
  return(success); 
}

gboolean
ags_functional_test_util_preferences_close()
{
  AgsPreferences *preferences;
  
  AgsApplicationContext *application_context;

  gboolean success;

  application_context = ags_application_context_get_instance();

  preferences = ags_ui_provider_get_preferences(AGS_UI_PROVIDER(application_context));

  success = ags_functional_test_util_dialog_close(preferences);
  
  ags_functional_test_util_reaction_time_long();
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_null),
						      &ags_functional_test_util_default_timeout,
						      &preferences);
  
  return(success); 
}

gboolean
ags_functional_test_util_about_open()
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_about_close()
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_navigation_expand()
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_navigation_bpm(gdouble bpm)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_navigation_rewind()
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_navigation_prev()
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_navigation_play()
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_navigation_stop()
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_navigation_next()
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_navigation_forward()
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_navigation_loop()
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_navigation_position(gdouble position)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_navigation_loop_left(gdouble loop_left)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_navigation_loop_right(gdouble loop_right)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_navigation_exclude_sequencers()
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_composite_toolbar_cursor_click()
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;  
  GtkToggleButton *position;

  AgsApplicationContext *application_context;

  gboolean success;

  application_context = ags_application_context_get_instance();
  
  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  position = composite_toolbar->position;
  
  success = ags_functional_test_util_toggle_button_click(position);

  return(success);
}

gboolean
ags_functional_test_util_composite_toolbar_edit_click()
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;  
  GtkToggleButton *edit;

  AgsApplicationContext *application_context;

  gboolean success;

  application_context = ags_application_context_get_instance();
  
  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  edit = composite_toolbar->edit;
  
  success = ags_functional_test_util_toggle_button_click(edit);

  return(success);
}

gboolean
ags_functional_test_util_composite_toolbar_delete_click()
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;  
  GtkToggleButton *clear;

  AgsApplicationContext *application_context;

  gboolean success;

  application_context = ags_application_context_get_instance();
  
  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  clear = composite_toolbar->clear;
  
  success = ags_functional_test_util_toggle_button_click(clear);

  return(success);
}

gboolean
ags_functional_test_util_composite_toolbar_select_click()
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;  
  GtkToggleButton *select;

  AgsApplicationContext *application_context;

  gboolean success;

  application_context = ags_application_context_get_instance();
  
  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  select = composite_toolbar->select;
  
  success = ags_functional_test_util_toggle_button_click(select);

  return(success);
}

gboolean
ags_functional_test_util_composite_toolbar_invert_click()
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;  
  GtkButton *invert;

  AgsApplicationContext *application_context;

  gboolean success;

  application_context = ags_application_context_get_instance();
  
  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  invert = composite_toolbar->invert;
  
  success = ags_functional_test_util_button_click(invert);

  return(success);
}

gboolean
ags_functional_test_util_composite_toolbar_paste_click()
{
  //TODO:JK: implement me

  return(TRUE);
}

gboolean
ags_functional_test_util_composite_toolbar_copy_click()
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;  
  GtkButton *copy;

  AgsApplicationContext *application_context;

  gboolean success;

  application_context = ags_application_context_get_instance();
  
  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  copy = composite_toolbar->copy;
  
  success = ags_functional_test_util_button_click(copy);

  return(success);
}

gboolean
ags_functional_test_util_composite_toolbar_cut_click()
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;  
  GtkButton *cut;

  AgsApplicationContext *application_context;

  gboolean success;

  application_context = ags_application_context_get_instance();
  
  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  cut = composite_toolbar->cut;
  
  success = ags_functional_test_util_button_click(cut);

  return(success);
}

gboolean
ags_functional_test_util_composite_toolbar_zoom(guint nth_zoom)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;  
  GtkComboBox *zoom;

  AgsApplicationContext *application_context;

  gboolean success;
  
  application_context = ags_application_context_get_instance();
  
  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  zoom = composite_toolbar->zoom;
  
  success = ags_functional_test_util_combo_box_click(zoom,
						     nth_zoom);

  return(success);
}

gboolean
ags_functional_test_util_machine_selector_select(gchar *machine)
{
  gboolean success;

  success = FALSE;

  //TODO:JK: implement me
  
  return(success);
}

gboolean
ags_functional_test_util_machine_selector_reverse_mapping()
{
  gboolean success;

  success = FALSE;

  //TODO:JK: implement me

  return(success);
}

gboolean
ags_functional_test_util_machine_selector_shift_piano(guint nth_shift)
{
  gboolean success;

  success = FALSE;

  //TODO:JK: implement me

  return(success);
}

gboolean
ags_functional_test_util_notation_edit_delete_point(guint x,
						    guint y)
{
  gboolean success;

  success = FALSE;

  //TODO:JK: implement me

  return(success);
}

gboolean
ags_functional_test_util_notation_edit_add_point(guint x0, guint x1,
						 guint y)
{
  gboolean success;

  success = FALSE;

  //TODO:JK: implement me

  return(success);
}

gboolean
ags_functional_test_util_notation_edit_select_region(guint x0, guint x1,
						     guint y0, guint y1)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_automation_edit_delete_point(guint nth_index,
						      guint x,
						      guint y)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_automation_edit_add_point(guint nth_index,
						   guint x,
						   guint y)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_automation_edit_select_region(guint nth_index,
						       guint x0, guint x1,
						       guint y0, guint y1)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_preferences_click_tab(guint nth_tab)
{
  AgsPreferences *preferences;
  
  AgsApplicationContext *application_context;

  gboolean success;

  application_context = ags_application_context_get_instance();

  preferences = ags_ui_provider_get_preferences(AGS_UI_PROVIDER(application_context));

  if(preferences == NULL){
    return(FALSE);
  }

  /* click tab */
  gtk_notebook_set_current_page(preferences->notebook,
				nth_tab);

  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_audio_preferences_buffer_size(guint nth_backend,
						       guint buffer_size)
{
  AgsPreferences *preferences;
  AgsSoundcardEditor *soundcard_editor;
  
  AgsApplicationContext *application_context;

  GList *start_list;
  
  gboolean success;

  application_context = ags_application_context_get_instance();

  preferences = ags_ui_provider_get_preferences(AGS_UI_PROVIDER(application_context));
  
  if(preferences == NULL){
    return(FALSE);
  }

  success = ags_functional_test_util_preferences_click_tab(1);

  if(!success){
    return(FALSE);
  }

  start_list = preferences->audio_preferences->soundcard_editor;
  soundcard_editor = g_list_nth_data(start_list,
				     nth_backend);
  
  g_list_free(start_list);

  if(soundcard_editor == NULL){
    return(FALSE);
  }

  gtk_spin_button_set_value(soundcard_editor->buffer_size,
			    (gdouble) buffer_size);
    
  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_audio_preferences_samplerate(guint nth_backend,
						      guint samplerate)
{
  AgsPreferences *preferences;
  AgsSoundcardEditor *soundcard_editor;
  
  AgsApplicationContext *application_context;

  GList *start_list;
  
  gboolean success;

  application_context = ags_application_context_get_instance();

  preferences = ags_ui_provider_get_preferences(AGS_UI_PROVIDER(application_context));

  if(preferences == NULL){
    return(FALSE);
  }

  success = ags_functional_test_util_preferences_click_tab(1);

  if(!success){
    return(FALSE);
  }

  start_list = preferences->audio_preferences->soundcard_editor;
  soundcard_editor = g_list_nth_data(start_list,
				     nth_backend);
  
  g_list_free(start_list);

  if(soundcard_editor == NULL){
    return(FALSE);
  }

  gtk_spin_button_set_value(soundcard_editor->samplerate,
			    (gdouble) samplerate);
    
  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_menu_button_click(guint nth_machine,
						   GtkMenuButton *menu_button,
						   gchar **item_path,
						   gchar *action)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsApplicationContext *application_context;

  GList *start_list;
    
  gboolean success;
  
  success = TRUE;

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  /* retrieve machine */
  start_list = window->machine;
  machine = g_list_nth_data(start_list,
			    nth_machine);
  
  if(!AGS_IS_MACHINE(machine)){
    return(FALSE);
  }
  
  if(!g_strcmp0(action, "machine.move_up")){
    //TODO:JK: implement me
  }else if(!g_strcmp0(action, "machine.move_down")){
    //TODO:JK: implement me
  }else if(!g_strcmp0(action, "machine.hide")){
    //TODO:JK: implement me
  }else if(!g_strcmp0(action, "machine.show")){
    //TODO:JK: implement me
  }else if(!g_strcmp0(action, "machine.destroy")){
      ags_machine_destroy_callback(NULL, NULL,
				 machine);
  }else if(!g_strcmp0(action, "machine.rename")){
    //TODO:JK: implement me
  }else if(!g_strcmp0(action, "machine.rename_audio")){
    //TODO:JK: implement me
  }else if(!g_strcmp0(action, "machine.reposition_audio")){
    //TODO:JK: implement me
  }else if(!g_strcmp0(action, "machine.properties")){
    GtkWidget **widget;
    
      ags_machine_properties_callback(NULL, NULL,
				    machine);

    widget = &(machine->machine_editor_dialog);

    ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_realized),
							&ags_functional_test_util_default_timeout,
							widget);

    ags_functional_test_util_reaction_time_long();
  }else if(!g_strcmp0(action, "machine.sticky_controls")){
    //TODO:JK: implement me
  }else if(!g_strcmp0(action, "machine.copy_pattern")){
    //TODO:JK: implement me
  }else if(!g_strcmp0(action, "machine.paste_pattern")){
    //TODO:JK: implement me
  }else if(!g_strcmp0(action, "machine.envelope")){
    //TODO:JK: implement me
  }else if(!g_strcmp0(action, "machine.audio_connection")){
    //TODO:JK: implement me
  }else if(!g_strcmp0(action, "machine.midi_connection")){
    //TODO:JK: implement me
  }else if(!g_strcmp0(action, "machine.audio_export")){
    //TODO:JK: implement me
  }else if(!g_strcmp0(action, "machine.midi_export")){
    //TODO:JK: implement me
  }else if(!g_strcmp0(action, "machine.audio_import")){
    //TODO:JK: implement me
  }else if(!g_strcmp0(action, "machine.midi_import")){
    //TODO:JK: implement me
  }else{
    success = FALSE;
  }
    
  return(success);
}

gboolean
ags_functional_test_util_machine_move_up(guint nth_machine)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsApplicationContext *application_context;

  GList *start_list;
  
  gboolean success;

  static gchar* path_strv[] = {
    "move up",
    NULL
  };

  success = TRUE;

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  /* retrieve machine */
  start_list = window->machine;
  machine = g_list_nth_data(start_list,
			    nth_machine);
  
  if(!AGS_IS_MACHINE(machine)){
    return(FALSE);
  }

  /* activate hide */
  success = TRUE;
  
  ags_functional_test_util_machine_menu_button_click(nth_machine,machine->context_menu_button,
						     path_strv,
						     "machine.move_up");

  return(success);
}

gboolean
ags_functional_test_util_machine_move_down(guint nth_machine)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsApplicationContext *application_context;

  GList *start_list;
  
  gboolean success;

  static gchar* path_strv[] = {
    "move down",
    NULL
  };

  success = TRUE;

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  /* retrieve machine */
  start_list = window->machine;
  machine = g_list_nth_data(start_list,
			    nth_machine);
  
  if(!AGS_IS_MACHINE(machine)){
    return(FALSE);
  }

  /* activate hide */
  success = TRUE;
  
  ags_functional_test_util_machine_menu_button_click(nth_machine,
						     machine->context_menu_button,
						     path_strv,
						     "machine.move_down");

  return(success);
}

gboolean
ags_functional_test_util_machine_hide(guint nth_machine)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsApplicationContext *application_context;

  GList *start_list;
  
  gboolean success;

  static gchar* path_strv[] = {
    "hide",
    NULL
  };

  success = TRUE;

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  /* retrieve machine */
  start_list = window->machine;
  machine = g_list_nth_data(start_list,
			    nth_machine);
  
  if(!AGS_IS_MACHINE(machine)){
    return(FALSE);
  }

  /* activate hide */
  success = TRUE;
  
  ags_functional_test_util_machine_menu_button_click(nth_machine,
						     machine->context_menu_button,
						     path_strv,
						     "machine.hide");

  return(success);
}

gboolean
ags_functional_test_util_machine_show(guint nth_machine)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsApplicationContext *application_context;

  GList *start_list;
  
  gboolean success;

  static gchar* path_strv[] = {
    "show",
    NULL
  };

  success = TRUE;

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  /* retrieve machine */
  start_list = window->machine;
  machine = g_list_nth_data(start_list,
			    nth_machine);
  
  if(!AGS_IS_MACHINE(machine)){
    return(FALSE);
  }

  /* activate hide */
  success = TRUE;
  
  ags_functional_test_util_machine_menu_button_click(nth_machine,
						     machine->context_menu_button,
						     path_strv,
						     "machine.show");

  return(success);
}

gboolean
ags_functional_test_util_machine_destroy(guint nth_machine)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsApplicationContext *application_context;

  GList *start_list;
  
  gboolean success;

  static gchar* path_strv[] = {
    "destroy",
    NULL
  };

  success = TRUE;

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  /* retrieve machine */
  start_list = window->machine;
  machine = g_list_nth_data(start_list,
			    nth_machine);
  
  if(!AGS_IS_MACHINE(machine)){
    return(FALSE);
  }

  /* activate hide */
  success = TRUE;
  
  ags_functional_test_util_machine_menu_button_click(nth_machine,
						     machine->context_menu_button,
						     path_strv,
						     "machine.destroy");

  return(success);
}

gboolean
ags_functional_test_util_machine_rename_open(guint nth_machine)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsApplicationContext *application_context;

  GList *start_list;
  
  gboolean success;

  static gchar* path_strv[] = {
    "rename",
    NULL
  };

  success = TRUE;

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  /* retrieve machine */
  start_list = window->machine;
  machine = g_list_nth_data(start_list,
			    nth_machine);
  
  if(!AGS_IS_MACHINE(machine)){
    return(FALSE);
  }

  /* activate hide */
  success = TRUE;
  
  ags_functional_test_util_machine_menu_button_click(nth_machine,
						     machine->context_menu_button,
						     path_strv,
						     "machine.rename");

  return(success);
}

gboolean
ags_functional_test_util_machine_rename_close(guint nth_machine)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_rename_set_name(guint nth_machine,
						 gchar *name)
{
  //TODO:JK: 

  return(TRUE);
}

GtkWidget*
ags_functional_test_util_get_machine_editor_dialog_line(GtkWidget *machine_editor_dialog,
							guint nth_pad, guint nth_audio_channel,
							gboolean is_output)
{
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  
  GList *list;

  if(machine_editor_dialog == NULL){
    return(NULL);
  }

  if(is_output){
    machine_editor_listing = AGS_MACHINE_EDITOR_DIALOG(machine_editor_dialog)->machine_editor->output_editor_listing;
  }else{
    machine_editor_listing = AGS_MACHINE_EDITOR_DIALOG(machine_editor_dialog)->machine_editor->input_editor_listing;
  }

  machine_editor_pad = g_list_nth_data(machine_editor_listing->pad,
				       nth_pad);

  if(!AGS_IS_MACHINE_EDITOR_PAD(machine_editor_pad)){
    return(NULL);
  }
  
  /* line editor */
  machine_editor_line = g_list_nth_data(machine_editor_pad->line,
					nth_audio_channel);

  if(!AGS_IS_MACHINE_EDITOR_LINE(machine_editor_line)){
    return(NULL);
  }
  
  return(machine_editor_line);
}

gboolean
ags_functional_test_util_machine_editor_dialog_open(guint nth_machine)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsApplicationContext *application_context;

  gboolean success;

  static gchar* path_strv[] = {
    "properties",
    NULL
  };

  success = TRUE;

  application_context = ags_application_context_get_instance();

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  /* retrieve machine */
  machine = g_list_nth_data(window->machine,
			    nth_machine);
  
  if(!AGS_IS_MACHINE(machine)){
    return(FALSE);
  }

  /* activate hide */
  success = ags_functional_test_util_machine_menu_button_click(nth_machine,
							       machine->context_menu_button,
							       path_strv,
							       "machine.properties");

  return(success);
}

gboolean
ags_functional_test_util_machine_editor_dialog_click_tab(guint nth_machine,
							 guint nth_tab)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  GtkWidget **widget;
  
  GList *list_start, *list;
  
  gboolean success;

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(list_start,
			 nth_machine);
  
  if(machine == NULL){
    return(FALSE);
  }

  g_list_free(list_start);

  /* click tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  widget = &(machine_editor->notebook);
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_test_util_default_timeout,
						      widget);

  ags_functional_test_util_reaction_time_long();

  gtk_notebook_set_current_page(machine_editor->notebook,
				nth_tab);

  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_editor_dialog_click_enable(guint nth_machine)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  GtkButton *enable_button;
  
  GList *list_start, *list;

  guint nth_tab;
  gboolean success;

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */
  enable_button = NULL;
  
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  list = g_list_nth(list_start,
		    nth_machine);
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* click tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      enable_button = AGS_MACHINE_EDITOR_LISTING(machine_editor->output_editor_listing)->enabled;
    }
    break;
  case 1:
    {
      enable_button = AGS_MACHINE_EDITOR_LISTING(machine_editor->input_editor_listing)->enabled;
    }
    break;
  case 2:
    {
      enable_button = AGS_MACHINE_EDITOR_COLLECTION(machine_editor->output_editor_collection)->enabled;
    }
    break;
  case 3:
    {
      enable_button = AGS_MACHINE_EDITOR_COLLECTION(machine_editor->input_editor_collection)->enabled;
    }
    break;
  case 4:
    {
      enable_button = AGS_RESIZE_EDITOR(machine_editor->resize_editor)->enabled;
    }
    break;
  }
  
  success = ags_functional_test_util_check_button_click(enable_button);

  ags_functional_test_util_reaction_time_long();

  return(success);
}

gboolean
ags_functional_test_util_machine_editor_dialog_link_set(guint nth_machine,
							guint nth_pad, guint nth_audio_channel,
							gchar *link_name, guint link_nth_line)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  AgsLinkEditor *link_editor;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *list_start, *list;

  gchar *value;

  guint nth_tab;
  gboolean success;
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */
  machine = NULL;
  
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  list = g_list_nth(list_start,
		    nth_machine);
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* set link */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
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
  default:
    return(FALSE);
  }

  list_start = ags_machine_editor_listing_get_pad(machine_editor_listing);
  
  machine_editor_pad = g_list_nth_data(list_start,
				       nth_pad);

  if(!AGS_IS_MACHINE_EDITOR_PAD(machine_editor_pad)){
    return(FALSE);
  }
  
  /* expander */
  gtk_expander_set_expanded(machine_editor_pad->expander,
			    TRUE);

  /* line editor */
  list_start = ags_machine_editor_pad_get_line(machine_editor_pad);
  machine_editor_line = g_list_nth_data(list_start,
					nth_audio_channel);

  if(!AGS_IS_MACHINE_EDITOR_LINE(machine_editor_line)){
    return(FALSE);
  }
  
  ags_functional_test_util_reaction_time_long();
  
  /* link editor */
  link_editor = machine_editor_line->link_editor;

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
  
  ags_functional_test_util_reaction_time_long();
  
  if(!success){
    return(FALSE);
  }
  
  /* set link line */
  success = TRUE;
  gtk_spin_button_set_value(link_editor->spin_button,
			    link_nth_line);

  ags_functional_test_util_reaction_time_long();
 
  return(success);
}

gboolean
ags_functional_test_util_machine_editor_dialog_link_open(guint nth_machine,
							 guint pad, guint audio_channel)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_editor_dialog_effect_add(guint nth_machine,
							  guint nth_pad, guint nth_audio_channel)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  AgsLineMemberEditor *member_editor;
  GtkDialog **effect_dialog;
  GtkButton *add_button;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *list_start, *list;

  gchar *value;

  guint nth_tab;
  gboolean success;
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */
  machine = NULL;
  
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  list = g_list_nth(list_start,
		    nth_machine);
  
  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }

  g_list_free(list_start);

  /* get tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case 0:
    {
      machine_editor_listing = machine_editor->output_editor_listing;
    }
    break;
  case 1:
    {
      machine_editor_listing = machine_editor->output_editor_listing;
    }
    break;
  default:
    return(FALSE);
  }

  list_start = ags_machine_editor_listing_get_pad(machine_editor_listing);
  
  machine_editor_pad = g_list_nth_data(list_start,
				       nth_pad);

  if(!AGS_IS_MACHINE_EDITOR_PAD(machine_editor_pad)){
    return(FALSE);
  }
  
  /* expander */
  gtk_expander_set_expanded(machine_editor_pad->expander,
			    TRUE);

  /* line editor */
  list_start = ags_machine_editor_pad_get_line(machine_editor_pad);
  machine_editor_line = g_list_nth_data(list_start,
					nth_audio_channel);

  if(!AGS_IS_MACHINE_EDITOR_LINE(machine_editor_line)){
    return(FALSE);
  }
  
  ags_functional_test_util_reaction_time_long();
  
  /* line member editor */
  member_editor = machine_editor_line->line_member_editor;
  
  /* effect dialog and add button */
  effect_dialog = &(member_editor->plugin_browser);
  add_button = member_editor->add;
  
  success = ags_functional_test_util_button_click(add_button);
  
  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_test_util_default_timeout,
						      effect_dialog);
  
  ags_functional_test_util_reaction_time_long();
 
  return(success);
}

gboolean
ags_functional_test_util_machine_editor_dialog_effect_remove(guint nth_machine,
							     guint nth_pad, guint nth_audio_channel,
							     guint nth_effect)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  AgsLineMemberEditor *member_editor;
  AgsLineMemberEditorEntry *member_editor_entry;
  GtkDialog **effect_dialog;
  GtkButton *check_button;
  GtkButton *remove_button;
  
  GtkTreeModel *model;

  AgsFunctionalTestUtilListLengthCondition condition;
  
  GtkTreeIter iter;
  GList *list_start, *list;

  gchar *value;

  guint nth_tab;
  gboolean success;
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));
  machine = g_list_nth_data(list_start,
			    nth_machine);
  
  if(machine == NULL){
    return(FALSE);
  }

  g_list_free(list_start);

  /* get tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
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
  default:
    return(FALSE);
  }

  list_start = ags_machine_editor_listing_get_pad(machine_editor_listing);
  
  machine_editor_pad = g_list_nth_data(list_start,
				       nth_pad);

  if(!AGS_IS_MACHINE_EDITOR_PAD(machine_editor_pad)){
    return(FALSE);
  }

  /* expander */
  gtk_expander_set_expanded(machine_editor_pad->expander,
			    TRUE);

  /* line editor */
  list_start = ags_machine_editor_pad_get_line(machine_editor_pad);

  machine_editor_line = g_list_nth_data(list_start,
					nth_audio_channel);

  if(!AGS_IS_MACHINE_EDITOR_LINE(machine_editor_line)){
    return(FALSE);
  }

  ags_functional_test_util_reaction_time_long();

  /* line member editor */
  member_editor = machine_editor_line->line_member_editor;
  
  /* effect dialog and remove button */
  effect_dialog = &(member_editor->plugin_browser);
  remove_button = member_editor->remove;

  /* check button */
  check_button = NULL;

  condition.start_list = &(member_editor->entry);

  list_start = ags_line_member_editor_get_entry(member_editor);

  condition.length = g_list_length(list_start) - 1;

  member_editor_entry = g_list_nth_data(list_start,
					nth_effect);

  if(member_editor_entry == NULL){
    return(FALSE);
  }
  
  /* click check button */
  success = ags_functional_test_util_check_button_click(member_editor_entry->check_button);

  /* click remove */
  if(success){
    ags_functional_test_util_reaction_time_long();
  
    success = ags_functional_test_util_button_click(remove_button);
  
    ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_list_length),
							&ags_functional_test_util_default_timeout,
							&condition);
  
    ags_functional_test_util_reaction_time_long();
  }
  
  return(success);
}

gboolean
ags_functional_test_util_machine_editor_dialog_effect_plugin_type(guint nth_machine,
								  guint nth_pad, guint nth_audio_channel,
								  gchar *plugin_type)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  AgsLineMemberEditor *member_editor;
  AgsLineMemberEditorEntry *member_editor_entry;
  AgsPluginBrowser *plugin_browser;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *list_start, *list;

  gchar *value;

  guint nth_tab;
  gboolean success;
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(list_start,
			    nth_machine);
  
  if(machine == NULL){
    return(FALSE);
  }
  
  g_list_free(list_start);

  /* get tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
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
  default:
    return(FALSE);
  }

  /* pad editor */
  list_start = ags_machine_editor_listing_get_pad(machine_editor_listing);

  machine_editor_pad = g_list_nth_data(list_start,
				       nth_pad);

  if(!AGS_IS_MACHINE_EDITOR_PAD(machine_editor_pad)){
    return(FALSE);
  }
    
  /* line editor */
  list_start = ags_machine_editor_pad_get_line(machine_editor_pad);

  machine_editor_line = g_list_nth_data(list_start,
					nth_audio_channel);

  if(!AGS_IS_MACHINE_EDITOR_LINE(machine_editor_line)){
    return(FALSE);
  }

  /* line member editor */
  member_editor = machine_editor_line->line_member_editor;

  plugin_browser = member_editor->plugin_browser;
  
  /* set plugin type */
  if(!g_ascii_strncasecmp(plugin_type,
			  "ladspa",
			  7)){
    gtk_combo_box_set_active((GtkComboBox *) plugin_browser->plugin_type,
			     1);

    success = TRUE;
  }else if(!g_ascii_strncasecmp(plugin_type,
				"lv2",
				4)){
    gtk_combo_box_set_active((GtkComboBox *) plugin_browser->plugin_type,
			     0);

    success = TRUE;
  }else if(!g_ascii_strncasecmp(plugin_type,
				"vst3",
				5)){
    gtk_combo_box_set_active((GtkComboBox *) plugin_browser->plugin_type,
			     2);

    success = TRUE;
  }
  
  ags_functional_test_util_reaction_time_long();
 
  return(success);
}

gboolean
ags_functional_test_util_machine_editor_dialog_ladspa_filename(guint nth_machine,
							       guint nth_pad, guint nth_audio_channel,
							       gchar *filename)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  AgsLineMemberEditor *member_editor;
  AgsPluginBrowser *plugin_browser;
  AgsLadspaBrowser *ladspa_browser;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *list_start, *list;

  gchar *value;

  guint nth_tab;
  gboolean success;
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(list_start,
			    nth_machine);
  
  if(machine == NULL){
    return(FALSE);
  }

  g_list_free(list_start);

  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
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
  default:
    return(FALSE);
  }

  /* pad editor */
  list_start = ags_machine_editor_listing_get_pad(machine_editor_listing);

  machine_editor_pad = g_list_nth_data(list_start,
				       nth_pad);

  if(!AGS_IS_MACHINE_EDITOR_PAD(machine_editor_pad)){
    return(FALSE);
  }
    
  /* line editor */
  list_start = ags_machine_editor_pad_get_line(machine_editor_pad);

  machine_editor_line = g_list_nth_data(list_start,
					nth_audio_channel);

  if(!AGS_IS_MACHINE_EDITOR_LINE(machine_editor_line)){
    return(FALSE);
  }

  /* line member editor */
  member_editor = machine_editor_line->line_member_editor;

  plugin_browser = member_editor->plugin_browser;

  ladspa_browser = plugin_browser->ladspa_browser;
  
  /* set plugin filename */
  model = gtk_tree_view_get_model(ladspa_browser->filename_tree_view);
  success = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      value = NULL;
      
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);
      
      if(!g_strcmp0(filename,
		    value)){
	gtk_tree_view_set_cursor(ladspa_browser->filename_tree_view,
				 gtk_tree_model_get_path(model,
							 &iter),
				 NULL,
				 FALSE);
	
	success = TRUE;
	
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }      
  
  ags_functional_test_util_reaction_time_long();
 
  return(success);
}

gboolean
ags_functional_test_util_machine_editor_dialog_ladspa_effect(guint nth_machine,
							     guint nth_pad, guint nth_audio_channel,
							     gchar *effect)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  AgsLineMemberEditor *member_editor;
  AgsPluginBrowser *plugin_browser;
  AgsLadspaBrowser *ladspa_browser;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *list_start, *list;

  gchar *value;

  guint nth_tab;
  gboolean success;
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(list_start,
			    nth_machine);
  
  if(machine == NULL){
    return(FALSE);
  }

  g_list_free(list_start);

  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
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
  default:
    return(FALSE);
  }

  /* pad editor */
  list_start = ags_machine_editor_listing_get_pad(machine_editor_listing);

  machine_editor_pad = g_list_nth_data(list_start,
				       nth_pad);

  if(!AGS_IS_MACHINE_EDITOR_PAD(machine_editor_pad)){
    return(FALSE);
  }
    
  /* line editor */
  list_start = ags_machine_editor_pad_get_line(machine_editor_pad);

  machine_editor_line = g_list_nth_data(list_start,
					nth_audio_channel);

  if(!AGS_IS_MACHINE_EDITOR_LINE(machine_editor_line)){
    return(FALSE);
  }

  /* line member editor */
  member_editor = machine_editor_line->line_member_editor;

  plugin_browser = member_editor->plugin_browser;

  ladspa_browser = plugin_browser->ladspa_browser;
  
  /* set plugin effect */
  model = gtk_tree_view_get_model(ladspa_browser->effect_tree_view);
  success = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      value = NULL;
      
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);
      
      if(!g_strcmp0(effect,
		    value)){
	gtk_tree_view_set_cursor(ladspa_browser->effect_tree_view,
				 gtk_tree_model_get_path(model,
							 &iter),
				 NULL,
				 FALSE);
	
	success = TRUE;
	
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }        
  
  ags_functional_test_util_reaction_time_long();
 
  return(success);
}

gboolean
ags_functional_test_util_machine_editor_dialog_lv2_filename(guint nth_machine,
							    guint nth_pad, guint nth_audio_channel,
							    gchar *filename)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  AgsLineMemberEditor *member_editor;
  AgsPluginBrowser *plugin_browser;
  AgsLv2Browser *lv2_browser;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *list_start, *list;

  gchar *value;

  guint nth_tab;
  gboolean success;
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(list_start,
			    nth_machine);
  
  if(machine == NULL){
    return(FALSE);
  }

  g_list_free(list_start);

  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
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
  default:
    return(FALSE);
  }

  /* pad editor */
  list_start = ags_machine_editor_listing_get_pad(machine_editor_listing);

  machine_editor_pad = g_list_nth_data(list_start,
				       nth_pad);

  if(!AGS_IS_MACHINE_EDITOR_PAD(machine_editor_pad)){
    return(FALSE);
  }
    
  /* line editor */
  list_start = ags_machine_editor_pad_get_line(machine_editor_pad);

  machine_editor_line = g_list_nth_data(list_start,
					nth_audio_channel);

  if(!AGS_IS_MACHINE_EDITOR_LINE(machine_editor_line)){
    return(FALSE);
  }

  /* line member editor */
  member_editor = machine_editor_line->line_member_editor;

  plugin_browser = member_editor->plugin_browser;

  lv2_browser = plugin_browser->lv2_browser;
  
  /* set plugin filename */
  model = gtk_tree_view_get_model(lv2_browser->filename_tree_view);
  success = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      value = NULL;
      
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);
      
      if(!g_strcmp0(filename,
		    value)){
	gtk_tree_view_set_cursor(lv2_browser->filename_tree_view,
				 gtk_tree_model_get_path(model,
							 &iter),
				 NULL,
				 FALSE);
	
	success = TRUE;
	
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }      
  
  ags_functional_test_util_reaction_time_long();
 
  return(success);
}

gboolean
ags_functional_test_util_machine_editor_dialog_lv2_effect(guint nth_machine,
							  guint nth_pad, guint nth_audio_channel,
							  gchar *effect)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorListing *machine_editor_listing;
  AgsMachineEditorPad *machine_editor_pad;
  AgsMachineEditorLine *machine_editor_line;
  AgsLineMemberEditor *member_editor;
  AgsPluginBrowser *plugin_browser;
  AgsLv2Browser *lv2_browser;
  
  GtkTreeModel *model;
  
  GtkTreeIter iter;
  GList *list_start, *list;

  gchar *value;

  guint nth_tab;
  gboolean success;
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(list_start,
			    nth_machine);

  g_list_free(list_start);
  
  if(machine == NULL){
    return(FALSE);
  }

  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
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
  default:
    return(FALSE);
  }

  /* pad editor */
  list_start = ags_machine_editor_listing_get_pad(machine_editor_listing);

  machine_editor_pad = g_list_nth_data(list_start,
				       nth_pad);

  g_list_free(list_start);

  if(!AGS_IS_MACHINE_EDITOR_PAD(machine_editor_pad)){
    return(FALSE);
  }
    
  /* line editor */
  list_start = ags_machine_editor_pad_get_line(machine_editor_pad);

  machine_editor_line = g_list_nth_data(list_start,
					nth_audio_channel);

  g_list_free(list_start);

  if(!AGS_IS_MACHINE_EDITOR_LINE(machine_editor_line)){
    return(FALSE);
  }

  /* line member editor */
  member_editor = machine_editor_line->line_member_editor;

  plugin_browser = member_editor->plugin_browser;

  lv2_browser = plugin_browser->lv2_browser;
  
  /* set plugin effect */
  model = gtk_tree_view_get_model(lv2_browser->effect_tree_view);
  success = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      value = NULL;
      
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);
      
      if(!g_strcmp0(effect,
		    value)){
	gtk_tree_view_set_cursor(lv2_browser->effect_tree_view,
				 gtk_tree_model_get_path(model,
							 &iter),
				 NULL,
				 FALSE);
	
	success = TRUE;
	
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }        
  
  ags_functional_test_util_reaction_time_long();
 
  return(success);
}

gboolean
ags_functional_test_util_machine_editor_dialog_bulk_add(guint nth_machine)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorCollection *machine_editor_collection;
  GtkButton *add_collection;

  GList *list_start, *list;

  guint nth_tab;
  gboolean success;
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(list_start,
			    nth_machine);

  g_list_free(list_start);
  
  if(machine == NULL){
    return(FALSE);
  }

  /* get tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_OUTPUT_TAB:
    {
      machine_editor_collection = machine_editor->output_editor_collection;
    }
    break;
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_INPUT_TAB:
    {
      machine_editor_collection = machine_editor->input_editor_collection;
    }
    break;
  default:
    return(FALSE);
  }

  add_collection = machine_editor_collection->add_bulk;
  
  /* add collection */
  success = ags_functional_test_util_button_click(add_collection);

  ags_functional_test_util_reaction_time_long();

  return(success);
}

gboolean
ags_functional_test_util_machine_editor_dialog_bulk_remove(guint nth_machine,
							   guint nth_bulk)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_editor_dialog_bulk_link(guint nth_machine,
							 guint nth_bulk,
							 gchar *link)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorCollection *machine_editor_collection;
  AgsMachineEditorBulk *machine_editor_bulk;

  GList *list_start, *list;

  GtkTreeModel *model;
  
  GtkTreeIter iter;

  gchar *value;

  guint nth_tab;
  gboolean success;
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(list_start,
			    nth_machine);

  g_list_free(list_start);
  
  if(machine == NULL){
    return(FALSE);
  }

  /* get notebook tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_OUTPUT_TAB:
    {
      machine_editor_collection = machine_editor->output_editor_collection;
    }
    break;
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_INPUT_TAB:
    {
      machine_editor_collection = machine_editor->input_editor_collection;
    }
    break;
  default:
    return(FALSE);
  }

  /* get bulk editor */
  list_start = ags_machine_editor_collection_get_bulk(machine_editor_collection);

  machine_editor_bulk = g_list_nth_data(list_start,
					nth_bulk);

  g_list_free(list_start);

  if(machine_editor_bulk == NULL){
    return(FALSE);
  }
  
  /* set link */
  model = gtk_combo_box_get_model(machine_editor_bulk->link);

  success = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);
      
      if(!g_strcmp0(link,
		    value)){
	gtk_combo_box_set_active_iter(machine_editor_bulk->link,
				      &iter);
	
	success = TRUE;
	
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }      
  
  ags_functional_test_util_reaction_time_long();

  return(success);
}

gboolean
ags_functional_test_util_machine_editor_dialog_bulk_first_line(guint nth_machine,
							       guint nth_bulk,
							       guint first_line)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorCollection *machine_editor_collection;
  AgsMachineEditorBulk *machine_editor_bulk;

  GList *list_start, *list;

  GtkTreeModel *model;
  
  GtkTreeIter iter;

  gchar *value;

  guint nth_tab;
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(list_start,
			    nth_machine);

  g_list_free(list_start);
  
  if(machine == NULL){
    return(FALSE);
  }

  /* get notebook tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_OUTPUT_TAB:
    {
      machine_editor_collection = machine_editor->output_editor_collection;
    }
    break;
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_INPUT_TAB:
    {
      machine_editor_collection = machine_editor->input_editor_collection;
    }
    break;
  default:
    return(FALSE);
  }

  /* get bulk editor */
  list_start = ags_machine_editor_collection_get_bulk(machine_editor_collection);

  machine_editor_bulk = g_list_nth_data(list_start,
					nth_bulk);

  g_list_free(list_start);

  if(machine_editor_bulk == NULL){
    return(FALSE);
  }

  gtk_spin_button_set_value(machine_editor_bulk->first_line,
			    first_line);

  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_editor_dialog_bulk_link_line(guint nth_machine,
							      guint nth_bulk,
							      guint first_link_line)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorCollection *machine_editor_collection;
  AgsMachineEditorBulk *machine_editor_bulk;

  GList *list_start, *list;

  GtkTreeModel *model;
  
  GtkTreeIter iter;

  gchar *value;

  guint nth_tab;
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(list_start,
			    nth_machine);

  g_list_free(list_start);
  
  if(machine == NULL){
    return(FALSE);
  }

  /* get notebook tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_OUTPUT_TAB:
    {
      machine_editor_collection = machine_editor->output_editor_collection;
    }
    break;
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_INPUT_TAB:
    {
      machine_editor_collection = machine_editor->input_editor_collection;
    }
    break;
  default:
    return(FALSE);
  }

  /* get bulk editor */
  list_start = ags_machine_editor_collection_get_bulk(machine_editor_collection);

  machine_editor_bulk = g_list_nth_data(list_start,
					nth_bulk);

  g_list_free(list_start);

  if(machine_editor_bulk == NULL){
    return(FALSE);
  }

  gtk_spin_button_set_value(machine_editor_bulk->first_link_line,
			    first_link_line);

  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_editor_dialog_bulk_count(guint nth_machine,
							  guint nth_bulk,
							  guint count)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorCollection *machine_editor_collection;
  AgsMachineEditorBulk *machine_editor_bulk;

  GList *list_start, *list;

  GtkTreeModel *model;
  
  GtkTreeIter iter;

  gchar *value;

  guint nth_tab;
  
  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(list_start,
			    nth_machine);

  g_list_free(list_start);
  
  if(machine == NULL){
    return(FALSE);
  }

  /* get notebook tab */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  nth_tab = gtk_notebook_get_current_page(machine_editor->notebook);

  switch(nth_tab){
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_OUTPUT_TAB:
    {
      machine_editor_collection = machine_editor->output_editor_collection;
    }
    break;
  case AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_INPUT_TAB:
    {
      machine_editor_collection = machine_editor->input_editor_collection;
    }
    break;
  default:
    return(FALSE);
  }

  /* get bulk editor */
  list_start = ags_machine_editor_collection_get_bulk(machine_editor_collection);

  machine_editor_bulk = g_list_nth_data(list_start,
					nth_bulk);

  g_list_free(list_start);

  if(machine_editor_bulk == NULL){
    return(FALSE);
  }

  gtk_spin_button_set_value(machine_editor_bulk->count,
			    count);

  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_editor_dialog_resize_audio_channels(guint nth_machine,
								     guint audio_channels)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  
  GList *list_start, *list;

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(list_start,
			    nth_machine);

  g_list_free(list_start);
  
  if(machine == NULL){
    return(FALSE);
  }

  /* resize audio channels */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  gtk_spin_button_set_value(machine_editor->resize_editor->audio_channels,
			    audio_channels);

  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_editor_dialog_resize_inputs(guint nth_machine,
							     guint inputs)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  
  GList *list_start, *list;

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */  
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(list_start,
			    nth_machine);

  g_list_free(list_start);
  
  if(machine == NULL){
    return(FALSE);
  }

  /* resize input pads */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  gtk_spin_button_set_value(machine_editor->resize_editor->input_pads,
			    inputs);

  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_editor_dialog_resize_outputs(guint nth_machine,
							      guint outputs)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  
  GList *list_start, *list;

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve machine */
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  machine = g_list_nth_data(list_start,
			    nth_machine);

  g_list_free(list_start);
  
  if(machine == NULL){
    return(FALSE);
  }

  /* resize output pads */
  machine_editor = AGS_MACHINE_EDITOR_DIALOG(machine->machine_editor_dialog)->machine_editor;

  gtk_spin_button_set_value(machine_editor->resize_editor->output_pads,
			    outputs);

  ags_functional_test_util_reaction_time_long();

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_edit_copy(guint nth_machine)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_line_member_set_toggled(gchar *specifier,
						 gboolean is_toggled)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_line_member_set_value(gchar *specifier,
					       gdouble value)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_audio_connection_click_tab(guint nth_machine,
							    guint nth_tab)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_audio_connection_click_enable(guint nth_machine)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_audio_connection_set_device(guint nth_machine,
							     guint pad, guint audio_channel,
							     gchar *device)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_audio_connection_set_line(guint nth_machine,
							   uint pad, guint audio_channel,
							   guint line)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_audio_connection_bulk_add(guint nth_machine)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_audio_connection_bulk_remove(guint nth_machine,
							      guint nth_bulk)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_audio_connection_bulk_first_line(guint nth_machine,
								  guint nth_bulk,
								  guint first_line)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_audio_connection_bulk_count(guint nth_machine,
							     guint nth_bulk,
							     guint count)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_audio_connection_bulk_device(guint nth_machine,
							      guint nth_bulk,
							      gchar *device)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_midi_connection_midi_channel(guint nth_machine,
							      guint midi_channel)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_midi_connection_audio_start_mapping(guint nth_machine,
								     guint audio_start_mapping)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_midi_connection_audio_end_mapping(guint nth_machine,
								   guint audio_end_mapping)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_midi_connection_midi_start_mapping(guint nth_machine,
								    guint midi_start_mapping)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_midi_connection_midi_end_mapping(guint nth_machine,
								  guint midi_end_mapping)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_machine_midi_connection_midi_device(guint nth_machine,
							     gchar *device)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_pad_group(guint nth_machine,
				   guint nth_pad)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_pad_mute(guint nth_machine,
				  guint nth_pad)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_pad_solo(guint nth_machine,
				  guint nth_pad)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_pad_play(guint nth_machine,
				  guint nth_pad)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_expander(guint nth_machine,
				  guint nth_pad, guint nth_audio_channel)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_effect_bulk_add(guint nth_machine)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_effect_bulk_remove(guint nth_machine,
					    guint nth_bulk_effect)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_effect_bulk_ladspa_filename(guint nth_machine,
						     gchar *filename)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_effect_bulk_ladspa_effect(guint nth_machine,
						   gchar *effect)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_effect_bulk_lv2_filename(guint nth_machine,
						  gchar *filename)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_effect_bulk_lv2_effect(guint nth_machine,
						gchar *effect)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_panel_mute_line(guint nth_machine,
					 guint nth_pad, guint nth_audio_channel)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_mixer_input_line_volume(guint nth_machine,
						 guint nth_pad, guint nth_audio_channel,
						 gdouble volume)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_drum_open(guint nth_machine)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsDrum *drum;
  GtkButton *open_button;
  GtkWidget **open_dialog;
  
  GList *list_start, *list;
  
  gboolean success;

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve drum */
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  drum = g_list_nth_data(list_start,
			 nth_machine);

  g_list_free(list_start);
  
  if(drum == NULL ||
     !AGS_IS_DRUM(drum)){
    return(FALSE);
  }
  
  /* open dialog */
  open_button = drum->open;
  open_dialog = &(drum->open_dialog);
  
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

  return(TRUE);
}

gboolean
ags_functional_test_util_drum_loop(guint nth_machine)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_drum_bank_0(guint nth_machine,
				     guint bank_0)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_drum_bank_1(guint nth_machine,
				     guint bank_1)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_drum_length(guint nth_machine,
				     guint length)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_drum_index(guint nth_machine,
				    guint nth_radio)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_drum_pad(guint nth_machine,
				  guint nth_pad)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_drum_pad_open(guint nth_machine,
				       guint nth_pad)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_drum_pad_edit(guint nth_machine,
				       guint nth_pad)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_drum_input_line_volume(guint nth_machine,
						guint nth_pad, guint nth_audio_channel,
						gdouble volume)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_matrix_run(guint nth_machine)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_matrix_loop(guint nth_machine)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_matrix_bank_1(guint nth_machine,
				       guint bank_1)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_matrix_length(guint nth_machine,
				       guint length)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_matrix_gutter(guint nth_machine,
				       guint nth_gutter)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_drum_cell(guint nth_machine,
				   guint x, guint y)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_synth_auto_update(guint nth_machine)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_synth_update(guint nth_machine)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean ags_functional_test_util_oscillator_wave(guint nth_machine,
						  guint nth_oscillator,
						  gchar *wave)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_oscillator_phase(guint nth_machine,
					  guint nth_oscillator,
					  gdouble phase)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_oscillator_attack(guint nth_machine,
					   guint nth_oscillator,
					   gdouble attack)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_oscillator_frequency(guint nth_machine,
					      guint nth_oscillator,
					      gdouble frequency)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_oscillator_length(guint nth_machine,
					   guint nth_oscillator,
					   gdouble length)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_oscillator_volume(guint nth_machine,
					   guint nth_oscillator,
					   gdouble volume)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_ffplayer_open(guint nth_machine)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  AgsFFPlayer *ffplayer;
  GtkButton *open_button;
  GtkWidget **open_dialog;
  
  GList *list_start, *list;
  
  gboolean success;

  gsequencer_application_context = ags_application_context_get_instance();

  /* retrieve ffplayer */
  list_start = ags_window_get_machine(AGS_WINDOW(gsequencer_application_context->window));

  ffplayer = g_list_nth_data(list_start,
			     nth_machine);

  g_list_free(list_start);
  
  if(ffplayer == NULL ||
     !AGS_IS_FFPLAYER(ffplayer)){
    return(FALSE);
  }

  /* open dialog */
  open_button = ffplayer->open;
  open_dialog = &(ffplayer->open_dialog);

  /* click open */
  success = ags_functional_test_util_button_click(open_button);

  ags_functional_test_util_idle_condition_and_timeout(AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(ags_functional_test_util_idle_test_widget_visible),
						      &ags_functional_test_util_default_timeout,
						      open_dialog);
  
  return(success);
}

gboolean
ags_functional_test_util_ffplayer_preset(guint nth_machine,
					 gchar *preset)
{
  //TODO:JK: 

  return(TRUE);
}

gboolean
ags_functional_test_util_ffplayer_instrument(guint nth_machine,
					     gchar *instrument)
{
  //TODO:JK: 

  return(TRUE);
}
