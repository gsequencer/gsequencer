/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/ags_preferences_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_gsequencer_application.h>

gboolean
ags_preferences_close_request_callback(GtkWindow *window, gpointer user_data)
{
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  ags_gsequencer_application_refresh_window_menu((AgsGSequencerApplication *) ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context)));

  return(TRUE);
}

void
ags_preferences_notebook_switch_page_callback(GtkNotebook *notebook,
					      gpointer page,
					      guint page_n,
					      AgsPreferences *preferences)
{
  if((AGS_PREFERENCES_SHUTDOWN & (preferences->flags)) != 0){
    return;
  }
  
  if(page_n == 0){
    gtk_widget_hide((GtkWidget *) preferences->midi_preferences->add);

    gtk_widget_show((GtkWidget *) preferences->audio_preferences->add);
  }else if(page_n == 1){
    gtk_widget_hide((GtkWidget *) preferences->audio_preferences->add);

    gtk_widget_show((GtkWidget *) preferences->midi_preferences->add);
  }else{
    gtk_widget_hide((GtkWidget *) preferences->audio_preferences->add);

    gtk_widget_hide((GtkWidget *) preferences->midi_preferences->add);
  }
}
