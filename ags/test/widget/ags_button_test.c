/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/widget/ags_button.h>

#include <stdlib.h>

void
activate(GtkApplication *app,
	 gpointer user_data)
{
  GtkWindow *window;
  GtkGrid *grid;
  AgsButton *button;

  guint i, j;
  
  window = gtk_application_window_new(app);

  grid = gtk_grid_new();
  gtk_window_set_child(window,
		       grid);

  button = ags_button_new_with_label("hello world!");
  gtk_grid_attach(grid,
		  button,
		  0, 0,
		  3, 1);

  for(i = 0; i < 3; i++){
    for(j = 0; j < 3; j++){
      gchar *str;

      str = g_strdup_printf("%d",
			    (j * 3) + i + 1);
      
      button = ags_button_new_with_label(str);
      
      gtk_widget_set_halign((GtkWidget *) button,
			    GTK_ALIGN_START);

      ags_button_set_button_size(button,
				 AGS_BUTTON_SIZE_SMALL);
      
      gtk_grid_attach(grid,
		      (GtkWidget *) button,
		      i, j + 1,
		      1, 1);

      g_free(str);
    }
  }
  
  gtk_widget_show(window);
}

int
main(int argc, char **argv)
{
  GtkApplication *app;

  int status;
  
  app = gtk_application_new("org.nongnu.gsequencer.ags_button_test",
			    G_APPLICATION_FLAGS_NONE);
  
  g_signal_connect(app, "activate",
		   G_CALLBACK(activate), NULL);

  status = g_application_run(G_APPLICATION(app),
			     argc,
			     argv);

  g_object_unref(app);
  
  return(status);
}
