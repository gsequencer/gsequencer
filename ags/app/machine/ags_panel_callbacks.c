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

#include <ags/app/machine/ags_panel_callbacks.h>

#include <ags/app/ags_window.h>

void
ags_panel_notify_parent_callback(GObject *gobject,
				 GParamSpec *pspec,
				 gpointer user_data)
{
  AgsWindow *window;

  gchar *str;

  window = AGS_WINDOW(gtk_widget_get_ancestor(gobject,
					      AGS_TYPE_WINDOW));

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_PANEL)->counter);

  g_object_set(AGS_MACHINE(gobject),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_PANEL);

  g_free(str);
}
