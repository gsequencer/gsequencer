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

#include <ags/app/ags_window_callbacks.h>

#include "config.h"

#if defined(AGS_WITH_MAC_INTEGRATION)
#include <gtkosxapplication.h>
#endif

#include <ags/app/ags_quit_dialog.h>
#include <ags/app/ags_menu_bar.h>
#include <ags/app/ags_context_menu.h>
#include <ags/app/ags_menu_action_callbacks.h>

#include <ags/app/file/ags_simple_file.h>

#include <ags/i18n.h>

void
ags_window_setup_completed_callback(AgsApplicationContext *application_context, AgsWindow *window)
{
  AgsMenuBar *menu_bar;
  AgsContextMenu *context_menu;
    
  GtkMenuItem *item;

  GtkBuilder *builder;

#if defined(AGS_WITH_MAC_INTEGRATION)
  GtkosxApplication *app;
#endif

  GtkAccelGroup *accel_group;

  GMenu *menu;
  GClosure *closure;

  /* open */
  accel_group = gtk_accel_group_new();

  closure = g_cclosure_new((GCallback) ags_menu_action_open_callback,
			   NULL,
			   NULL);
  gtk_accel_group_connect(accel_group,
			  GDK_KEY_O,
			  GDK_CONTROL_MASK,
			  0,
			  closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

  /* save */
  accel_group = gtk_accel_group_new();

  closure = g_cclosure_new((GCallback) ags_menu_action_save_callback,
			   NULL,
			   NULL);
  gtk_accel_group_connect(accel_group,
			  GDK_KEY_S,
			  GDK_CONTROL_MASK,
			  0,
			  closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

  /* save as */
  accel_group = gtk_accel_group_new();

  closure = g_cclosure_new((GCallback) ags_menu_action_save_as_callback,
			   NULL,
			   NULL);
  gtk_accel_group_connect(accel_group,
			  GDK_KEY_S,
			  GDK_CONTROL_MASK | GDK_SHIFT_MASK,
			  0,
			  closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

  /* quit */
  accel_group = gtk_accel_group_new();

  closure = g_cclosure_new((GCallback) ags_menu_action_quit_callback,
			   NULL,
			   NULL);
  gtk_accel_group_connect(accel_group,
			  GDK_KEY_Q,
			  GDK_CONTROL_MASK,
			  0,
			  closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

  /* online help */
  accel_group = gtk_accel_group_new();

  closure = g_cclosure_new((GCallback) ags_menu_action_online_help_callback,
			   NULL,
			   NULL);
  gtk_accel_group_connect(accel_group,
			  GDK_KEY_H,
			  GDK_CONTROL_MASK,
			  0,
			  closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
    
  /* menu */
  menu_bar = window->menu_bar;
  context_menu = window->context_menu;
    
  builder = gtk_builder_new_from_resource("/org/nongnu/gsequencer/ags/app/ui/ags_add_menu.ui");

  menu = gtk_builder_get_object(builder,
				"ags-add-menu");

  /* menu - bridge */
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("LADSPA"));
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) ags_ladspa_bridge_menu_new());
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  ags_window_load_add_menu_ladspa(window,
				  menu);
  
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("DSSI"));
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) ags_dssi_bridge_menu_new());
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  ags_window_load_add_menu_dssi(window,
				menu);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Lv2"));
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) ags_lv2_bridge_menu_new());
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  ags_window_load_add_menu_lv2(window,
			       menu);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("VST3"));
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) ags_vst3_bridge_menu_new());
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  ags_window_load_add_menu_vst3(window,
				menu);

  /* menu - live */
  menu_bar->live = (GtkMenu *) gtk_menu_new();
  
  item = (GtkMenuItem *) gtk_menu_item_new_with_label("live!");
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget *) menu_bar->live);
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("DSSI"));
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) ags_live_dssi_bridge_menu_new());
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->live, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Lv2"));
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) ags_live_lv2_bridge_menu_new());
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->live, (GtkWidget*) item);  

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("VST3"));
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) ags_live_vst3_bridge_menu_new());
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->live, (GtkWidget*) item);

  /* context menu - bridge */
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("LADSPA"));
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) ags_ladspa_bridge_menu_new());
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("DSSI"));
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) ags_dssi_bridge_menu_new());
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Lv2"));
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) ags_lv2_bridge_menu_new());
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("VST3"));
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) ags_vst3_bridge_menu_new());
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

  /* context menu - live */
  context_menu->live = (GtkMenu *) gtk_menu_new();
  
  item = (GtkMenuItem *) gtk_menu_item_new_with_label("live!");
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) context_menu->live);
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("DSSI"));
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) ags_live_dssi_bridge_menu_new());
  gtk_menu_shell_append((GtkMenuShell*) context_menu->live, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Lv2"));
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) ags_live_lv2_bridge_menu_new());
  gtk_menu_shell_append((GtkMenuShell*) context_menu->live, (GtkWidget*) item);      

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("VST3"));
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) ags_live_vst3_bridge_menu_new());
  gtk_menu_shell_append((GtkMenuShell*) context_menu->live, (GtkWidget *) item);

#if defined(AGS_WITH_MAC_INTEGRATION)
  app = gtkosx_application_get();

  gtk_widget_show_all(window->menu_bar);
  gtkosx_application_sync_menubar(app);

  gtk_widget_set_no_show_all((GtkWidget *) window->menu_bar, TRUE);
  gtk_widget_hide((GtkWidget *) window->menu_bar);
#endif

  gtk_menu_button_set_menu_model(window->add_button,
				 menu);

  /* connect and show window */
  ags_connectable_connect(AGS_CONNECTABLE(window));
  gtk_widget_show_all((GtkWidget *) window);

  gtk_widget_show_all((GtkWidget *) context_menu);
}

gboolean
ags_window_delete_event_callback(GtkWidget *widget, gpointer data)
{
  AgsQuitDialog *quit_dialog;

  quit_dialog = ags_quit_dialog_new();
  gtk_widget_show_all((GtkWidget *) quit_dialog);
  
  gtk_widget_grab_focus((GtkWidget *) quit_dialog->cancel);
  
  ags_connectable_connect(AGS_CONNECTABLE(quit_dialog));

  gtk_dialog_run((GtkDialog *) quit_dialog);

  return(TRUE);
}

gboolean
ags_window_button_press_event(GtkWidget *widget, GdkEventButton *event, AgsWindow *window)
{
  gboolean success;

  success = FALSE;
  
  if(event->type == GDK_BUTTON_PRESS && event->button == 3){
    success = TRUE;
    
    gtk_menu_popup_at_pointer(GTK_MENU(window->context_menu),
			      event);
  }

  return(success);
}