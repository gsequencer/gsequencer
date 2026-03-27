/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2026 Joël Krähemann
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

GtkWindow *window;
GtkGrid *grid;
GtkButton *button;
GtkPopover *popover;
GtkGrid *popover_grid;
GtkSpinButton *spin_button;

void
button_clicked(GtkButton *button, gpointer data)
{
  gtk_popover_popup(popover);
}

void
activate(GtkApplication *app,
	 gpointer user_data)
{
  window = gtk_application_window_new(app);

  grid = (GtkGrid *) gtk_grid_new();
  gtk_window_set_child(window,
		       (GtkWidget *) grid);

  button = (GtkButton *) gtk_button_new_with_label("input");
  gtk_grid_attach(grid,
		  (GtkWidget *) button,
		  0, 0,
		  1, 1);
  
  popover = (GtkPopover *) gtk_popover_new();
  gtk_grid_attach(grid,
		  (GtkWidget *) popover,
		  1, 0,
		  1, 1);

  popover_grid = (GtkGrid *) gtk_grid_new();
  gtk_popover_set_child(popover,
			(GtkWidget *) popover_grid);

  spin_button = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100.0, 1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) spin_button,
		  0, 0,
		  1, 1);

  g_signal_connect(button, "clicked",
		   G_CALLBACK(button_clicked), NULL);
  
  gtk_widget_show(window);
}

int
main(int argc, char **argv)
{
  GtkApplication *app;

  int status;
  
  app = gtk_application_new("org.nongnu.gsequencer.gtk_popover_and_spin_button_test",
			    G_APPLICATION_FLAGS_NONE);
  
  g_signal_connect(app, "activate",
		   G_CALLBACK(activate), NULL);

  status = g_application_run(G_APPLICATION(app),
			     argc,
			     argv);

  g_object_unref(app);
  
  return(status);
}
