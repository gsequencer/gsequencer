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

#include <ags/i18n.h>

void
ags_window_setup_completed_callback(AgsApplicationContext *application_context, AgsWindow *window)
{
  GtkBuilder *builder;

#if defined(AGS_WITH_MAC_INTEGRATION)
  GtkosxApplication *app;
#endif

  GMenu *menu;

  /* menu */
  builder = gtk_builder_new_from_resource("/org/nongnu/gsequencer/ags/app/ui/ags_add_menu.ui");

  menu = gtk_builder_get_object(builder,
				"ags-add-menu");

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
  gtk_widget_show((GtkWidget *) window);
}
