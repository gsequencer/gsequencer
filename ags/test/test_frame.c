/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2022 Joël Krähemann
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


static void
activate(GtkApplication* app,
	 gpointer user_data)
{
  GtkWindow *window;
  GtkFrame *frame;

  window = gtk_application_window_new(app);

  frame = gtk_frame_new("test");
  gtk_window_set_child(window,
		       frame);

  gtk_frame_set_child(frame,
		      gtk_label_new("label"));

  gtk_frame_set_child(frame,
		      gtk_spin_button_new_with_range(0.0, 1.0, 0.1));
  
  gtk_widget_show(window);
}

int
main(int argc, char **argv)
{
  GtkApplication *app;
  gint status;
  
  gtk_init();

  app = gtk_application_new("org.gsequencer.test_frame",
			    G_APPLICATION_FLAGS_NONE);

  g_signal_connect(app, "activate",
		   G_CALLBACK(activate), NULL);

  status = g_application_run(G_APPLICATION(app),
			     0, NULL);
  
  return(status);
}
