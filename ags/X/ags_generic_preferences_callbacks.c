/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences_callbacks.h>

void
ags_generic_preferences_autosave_thread_clicked_callback(GtkWidget *check_button,
							 AgsGenericPreferences *generic_preferences)
{
  AgsApplicationContext *application_context;
  AgsPreferences *preferences;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(generic_preferences),
							   AGS_TYPE_PREFERENCES);

  application_context = ags_application_context_get_instance();

  //FIXME:JK: defunct
#if 0
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button))){
    ags_thread_start((AgsThread *) autosave_thread);
  }else{
    ags_thread_stop((AgsThread *) autosave_thread);
  }
#endif
}

void
ags_generic_preferences_rt_safe_callback(GtkWidget *check_button,
					 AgsGenericPreferences *generic_preferences)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button))){
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(generic_preferences->engine_mode),
				 1);

    gtk_widget_set_sensitive(GTK_WIDGET(generic_preferences->engine_mode),
			     FALSE);
  }else{
    gtk_widget_set_sensitive(GTK_WIDGET(generic_preferences->engine_mode),
			     TRUE);
  }
}

