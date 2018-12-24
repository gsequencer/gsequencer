/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/ags_performance_preferences_callbacks.h>

#include <ags/X/ags_preferences.h>

void
ags_performance_preferences_super_threaded_channel_callback(GtkWidget *button,
							    AgsPerformancePreferences *performance_preferences)
{
  if(gtk_toggle_button_get_active((GtkToggleButton *) button)){
    gtk_toggle_button_set_active((GtkToggleButton *) performance_preferences->super_threaded_audio,
				 TRUE);
    gtk_widget_set_sensitive((GtkWidget *) performance_preferences->super_threaded_audio,
			     FALSE);
  }else{
    gtk_widget_set_sensitive((GtkWidget *) performance_preferences->super_threaded_audio,
			     TRUE);
  }
}

