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

#include <ags/widget/ags_cartesian.h>

int
main(int argc, char **argv)
{
  GtkWindow *window; 
  AgsCartesian *cartesian;
  
  gtk_init(&argc,
	   &argv);

  window = (GtkWindow *) gtk_window_new(GTK_WINDOW_TOPLEVEL);

  cartesian = ags_cartesian_new();
  gtk_container_add(window,
		    cartesian);
  gtk_widget_show_all(window);
  
  gtk_main();
  
  return(0);
}
