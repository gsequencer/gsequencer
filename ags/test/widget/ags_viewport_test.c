/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/libags-gui.h>

int
main(int argc, char **argv)
{
  GtkWindow *window;
  GtkBox *vbox;
  AgsViewport *viewport;
  GtkGrid *block;

  guint i, j;
  
  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,
		     0);
  
  gtk_widget_set_halign(vbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign(vbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_vexpand(vbox,
			 TRUE);
  gtk_widget_set_hexpand(vbox,
			 TRUE);
  
  gtk_container_add(window,
		    vbox);

  gtk_widget_set_size_request(window,
			      50, 50);

  viewport = ags_viewport_new();
  
  gtk_widget_set_size_request(viewport,
			      100, 100);

  gtk_widget_set_halign(viewport,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign(viewport,
			GTK_ALIGN_FILL);

#if 0
  gtk_widget_set_vexpand(viewport,
			 TRUE);
  gtk_widget_set_hexpand(viewport,
			 TRUE);
#endif
  
  gtk_box_pack_start(vbox,
		     viewport,
		     FALSE, FALSE,
		     0);

  block = gtk_grid_new();
  gtk_container_add(viewport,
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
  
  gtk_widget_show_all(window);
  
  gtk_main();
  
  return(0);
}
