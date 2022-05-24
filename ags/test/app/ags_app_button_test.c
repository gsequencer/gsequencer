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

#include <gtk/gtk.h>

#include <stdlib.h>

void
activate(GtkApplication *app,
	 gpointer user_data)
{
  GtkWindow *window;
  GtkGrid *grid;
  GtkMenuButton *menu_button;
  
  GtkBuilder *builder;

  GMenu *menu;
  
  window = gtk_application_window_new(app);

  grid = gtk_grid_new();
  gtk_window_set_child(window,
		       grid);

  menu_button = gtk_menu_button_new();
  g_object_set(menu_button,
	       "direction", GTK_ARROW_NONE,
	       NULL);
  gtk_grid_attach(grid,
		  menu_button,
		  0, 0,
		  1, 1);
  
  builder = gtk_builder_new_from_resource("/org/nongnu/gsequencer/ags/app/ui/ags_primary_menu.ui");

  menu = gtk_builder_get_object(builder,
				"ags-primary-menu");
  gtk_menu_button_set_menu_model(menu_button,
				 menu);

  gtk_widget_show(window);
}

int
main(int argc, char **argv)
{
  GtkApplication *app;

  int status;
  
  app = gtk_application_new("org.nongnu.gsequencer.ags_add_button_test",
			    G_APPLICATION_FLAGS_NONE);
  
  g_signal_connect(app, "activate",
		   G_CALLBACK(activate), NULL);

  status = g_application_run(G_APPLICATION(app),
			     argc,
			     argv);

  g_object_unref(app);
  
  return(status);
}
