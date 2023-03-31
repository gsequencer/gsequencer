/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/app/ags_gsequencer_application.h>
#include <ags/app/ags_gsequencer_application_context.h>
#include <ags/app/ags_app_action_util.h>

#if defined(AGS_WITH_MAC_INTEGRATION)
#include <gtkosxapplication.h>
#endif

#include <ags/i18n.h>

void
ags_window_setup_completed_callback(AgsApplicationContext *application_context, AgsWindow *window)
{
  GtkBuilder *builder;

  AgsGSequencerApplication *gsequencer_app;

#if defined(AGS_WITH_MAC_INTEGRATION)
  GtkosxApplication *app;
#endif

  GMenu *menu;

  gsequencer_app = (AgsGSequencerApplication *) AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->app;

  /* menu */
  builder = gtk_builder_new_from_resource("/org/nongnu/gsequencer/ags/app/ui/ags_add_menu.ui");

  menu = (GMenu *) gtk_builder_get_object(builder,
					  "ags-add-menu");

#if defined(AGS_WITH_MAC_INTEGRATION)
  app = gtkosx_application_get();

  gtk_widget_show((GtkWidget *) window->menu_bar);
  gtkosx_application_sync_menubar(app);

  gtk_widget_hide((GtkWidget *) window->menu_bar);
#endif

  gtk_menu_button_set_menu_model(window->add_button,
				 G_MENU_MODEL(menu));

  ags_window_load_add_menu_ladspa(window,
				  menu);
  ags_window_load_add_menu_ladspa(window,
				  gsequencer_app->add_menu);

  ags_window_load_add_menu_dssi(window,
				menu);
  ags_window_load_add_menu_dssi(window,
				gsequencer_app->add_menu);

  ags_window_load_add_menu_lv2(window,
			       menu);
  ags_window_load_add_menu_lv2(window,
			       gsequencer_app->add_menu);

  ags_window_load_add_menu_vst3(window,
				menu);
  ags_window_load_add_menu_vst3(window,
				gsequencer_app->add_menu);

  ags_window_load_add_menu_live_dssi(window,
				     menu);
  ags_window_load_add_menu_live_dssi(window,
				     gsequencer_app->add_menu);

  ags_window_load_add_menu_live_lv2(window,
				    menu);
  ags_window_load_add_menu_live_lv2(window,
				    gsequencer_app->add_menu);

  ags_window_load_add_menu_live_vst3(window,
				     menu);
  ags_window_load_add_menu_live_vst3(window,
				     gsequencer_app->add_menu);
  
  /* connect and show window */
  gtk_widget_show((GtkWidget *) window);
}

gboolean
ags_window_close_request_callback(GtkWindow *window, gpointer user_data)
{
  ags_app_action_util_quit();
  
  return(TRUE);
}
