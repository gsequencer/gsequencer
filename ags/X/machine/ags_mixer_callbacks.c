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

#include <ags/X/machine/ags_mixer_callbacks.h>

#include <ags/X/ags_window.h>

void
ags_mixer_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMixer *mixer)
{
  AgsWindow *window;

  if(old_parent != NULL)
    return;

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);

  AGS_MACHINE(mixer)->name = g_strdup_printf("Default %d\0",
					     ags_window_find_machine_counter(window, AGS_TYPE_MIXER)->counter);
  ags_window_increment_machine_counter(window,
				       AGS_TYPE_MIXER);
}
