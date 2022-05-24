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
  GtkScrolledWindow *scrolled_window;
  GtkGrid *block;

  guint i;
  guint j;
  
  window = gtk_application_window_new(app);

  grid = gtk_grid_new();
  gtk_window_set_child(window,
		       grid);

  scrolled_window = gtk_scrolled_window_new();  
  gtk_grid_attach(grid,
		  scrolled_window,
		  0, 0,
		  1, 1);

  gtk_scrolled_window_set_policy(scrolled_window,
  				 GTK_POLICY_EXTERNAL,
  				 GTK_POLICY_EXTERNAL);
  
  gtk_widget_set_vexpand(scrolled_window,
			 TRUE);

  gtk_widget_set_margin_top(scrolled_window,
			    20);

  gtk_widget_set_size_request(scrolled_window,
			      60, -1);

  block = gtk_grid_new();
  gtk_scrolled_window_set_child(scrolled_window,
				block);

  for(i = 0; i < 10; i++){
    for(j = 0; j < 10; j++){
      GtkButton *button;

      gchar *str;

      str = g_strdup_printf("%d", i * 10 + j);
      
      button = gtk_button_new_with_label(str);

      gtk_grid_attach(block,
		      button,
		      i, j,
		      1, 1);

      g_free(str);
    }
  }
  
  gtk_widget_show(window);
  gtk_widget_show(block);
}

int
main(int argc, char **argv)
{
  GtkApplication *app;

  int status;
  
  app = gtk_application_new("org.nongnu.gsequencer.ags_viewport_test",
			    G_APPLICATION_FLAGS_NONE);
  
  g_signal_connect(app, "activate",
		   G_CALLBACK(activate), NULL);

  status = g_application_run(G_APPLICATION(app),
			     argc,
			     argv);

  g_object_unref(app);
  
  return(status);
}
