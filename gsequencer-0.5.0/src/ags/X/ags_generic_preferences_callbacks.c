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

#include <ags/X/ags_generic_preferences_callbacks.h>

#include <ags/main.h>

#include <ags/thread/ags_thread-posix.h>
#include <ags/thread/ags_autosave_thread.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences_callbacks.h>

void
ags_generic_preferences_autosave_thread_clicked_callback(GtkWidget *check_button,
							 AgsGenericPreferences *generic_preferences)
{
  AgsMain *ags_main;
  AgsAutosaveThread *autosave_thread;
  AgsPreferences *preferences;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(generic_preferences),
							   AGS_TYPE_PREFERENCES);
  ags_main = (AgsMain *) AGS_WINDOW(preferences->window)->ags_main;
  autosave_thread = (AgsAutosaveThread *) ags_main->autosave_thread;

  if(gtk_toggle_button_get_active(check_button)){
    ags_thread_start((AgsThread *) autosave_thread);
  }else{
    ags_thread_stop((AgsThread *) autosave_thread);
  }
}
