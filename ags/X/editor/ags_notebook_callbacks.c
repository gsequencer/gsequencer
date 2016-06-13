/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/editor/ags_notebook_callbacks.h>

void
ags_notebook_scroll_prev_callback(GtkWidget *button,
				  AgsNotebook *notebook)
{
  GtkAdjustment *adjustment;
  
  gint length;
  
  adjustment = gtk_viewport_get_hadjustment(notebook->viewport);

  length = g_list_length(notebook->tabs);

  if(adjustment->value - adjustment->step_increment > 0){
    gtk_adjustment_set_value(adjustment,
			     adjustment->value - adjustment->step_increment);
  }else{
    gtk_adjustment_set_value(adjustment,
			     0.0);
  }

  gdk_window_invalidate_rect(gtk_viewport_get_view_window(notebook->viewport),
			     &(GTK_WIDGET(notebook->hbox)->allocation),
			     TRUE);
}

void
ags_notebook_scroll_next_callback(GtkWidget *button,
				  AgsNotebook *notebook)
{
  GtkAdjustment *adjustment;

  gint length;
  
  adjustment = gtk_viewport_get_hadjustment(notebook->viewport);

  length = g_list_length(notebook->tabs);
  
  if(adjustment->value + adjustment->step_increment < adjustment->upper - adjustment->page_size){
    gtk_adjustment_set_value(adjustment,
			     adjustment->value + adjustment->step_increment);
  }else{
    gtk_adjustment_set_value(adjustment,
			     adjustment->upper - adjustment->page_size);
  }

  gdk_window_invalidate_rect(gtk_viewport_get_view_window(notebook->viewport),
			     &(GTK_WIDGET(notebook->hbox)->allocation),
			     TRUE);
}

