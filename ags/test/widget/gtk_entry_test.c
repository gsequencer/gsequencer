/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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
clicked_callback(GtkButton *button, GtkEntry *entry)
{
  GtkEntryBuffer *buffer;
  
  gchar *str;

  buffer = gtk_entry_get_buffer(entry);
  
  str = gtk_editable_get_text(GTK_EDITABLE(entry));
  
  g_message("editable %s", str);

  str = gtk_entry_buffer_get_text(buffer);
  
  g_message("buffer %s", str);
}

void
activate(GtkApplication *app,
	 gpointer user_data)
{
  GtkWindow *window;
  GtkGrid *grid;
  GtkEntry *entry;
  GtkButton *activate;

  window = (GtkWindow *) gtk_application_window_new(app);

  grid = (GtkGrid *) gtk_grid_new();
  gtk_window_set_child(window,
		       (GtkWidget *) grid);

  entry = (GtkEntry *) gtk_entry_new();
  gtk_grid_attach(grid,
		  (GtkWidget *) entry,
		  0, 0,
		  1, 1);

  activate = (GtkButton *) gtk_button_new_with_label("ok");
  gtk_grid_attach(grid,
		  (GtkWidget *) activate,
		  1, 0,
		  1, 1);

  g_signal_connect(activate, "clicked",
		   G_CALLBACK(clicked_callback), entry);
  
  gtk_widget_show((GtkWidget *) window);
}

int
main(int argc, char **argv)
{
  GtkApplication *app;

  int status;
  
  app = gtk_application_new("org.nongnu.gsequencer.gtk_entry_test",
			    G_APPLICATION_FLAGS_NONE);
  
  g_signal_connect(app, "activate",
		   G_CALLBACK(activate), NULL);

  status = g_application_run(G_APPLICATION(app),
			     argc,
			     argv);

  g_object_unref(app);
  
  return(status);
}
