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

#include <ags/widget/ags_led_array.h>

#include <stdlib.h>

#define AGS_LED_ARRAY_TEST_LED_COUNT (16)

gboolean timeout(AgsLedArray *led_array);

gboolean
timeout(AgsLedArray *led_array)
{
  static gint i = 0;

  ags_led_array_unset_all(led_array);
  ags_led_array_set_nth(led_array,
			i);  

  i++;
  i %= AGS_LED_ARRAY_TEST_LED_COUNT;
  
  return(G_SOURCE_CONTINUE);
}

void
activate(GtkApplication *app,
	 gpointer user_data)
{
  GtkWindow *window;
  GtkGrid *grid;
  AgsLedArray *led_array;

  window = gtk_application_window_new(app);

  grid = gtk_grid_new();
  gtk_window_set_child(window,
		       grid);

  led_array = ags_led_array_new(GTK_ORIENTATION_VERTICAL,
				16,
				7,
				AGS_LED_ARRAY_TEST_LED_COUNT);
  gtk_grid_attach(grid,
		  led_array,
		  0, 0,
		  1, 1);
  
  gtk_widget_show(window);

  g_timeout_add(100,
		timeout,
		led_array);
}

int
main(int argc, char **argv)
{
  GtkApplication *app;

  int status;
  
  app = gtk_application_new("org.nongnu.gsequencer.ags_led_array_test",
			    G_APPLICATION_FLAGS_NONE);
  
  g_signal_connect(app, "activate",
		   G_CALLBACK(activate), NULL);

  status = g_application_run(G_APPLICATION(app),
			     argc,
			     argv);

  g_object_unref(app);
  
  return(status);
}
