/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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
  AgsVScaleBox *vscale_box;
  AgsScale *scale;

  gtk_init(&argc, &argv);
  
  g_log_set_fatal_mask("GLib-GObject", // "Gtk" , // 
  		       G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL); // G_LOG_LEVEL_WARNING

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  vscale_box = ags_vscale_box_new();
  gtk_container_add(window,
		    vscale_box);

  scale = ags_scale_new();
  g_object_set(scale,
	       "control-name", "test 0",
	       NULL);
  gtk_box_pack_start(vscale_box,
		     scale,
		     FALSE, FALSE,
		     0);

  scale = ags_scale_new();
  g_object_set(scale,
	       "control-name", "test 1",
	       NULL);
  gtk_box_pack_start(vscale_box,
		     scale,
		     FALSE, FALSE,
		     0);

  gtk_widget_show_all(window);

  gtk_main();
  
  return(0);
}
