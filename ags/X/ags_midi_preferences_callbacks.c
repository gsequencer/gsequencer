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

#include <ags/X/ags_midi_preferences_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_devout.h>

#include <ags/audio/thread/ags_soundcard_thread.h>

#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>
#include <ags/X/ags_soundcard_editor.h>

#include <ags/config.h>

int
ags_midi_preferences_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMidiPreferences *midi_preferences)
{  
  AgsPreferences *preferences;

  if(old_parent != NULL){
    return(0);
  }

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(midi_preferences),
							   AGS_TYPE_PREFERENCES);

  midi_preferences->add = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_end((GtkBox *) GTK_DIALOG(preferences)->action_area,
		   (GtkWidget *) midi_preferences->add,
		   TRUE, FALSE,
		   0);  

  return(0);
}

void
ags_midi_preferences_add_callback(GtkWidget *widget, AgsMidiPreferences *midi_preferences)
{
  //TODO:JK: implement me
}

void
ags_midi_preferences_remove_soundcard_editor_callback(GtkWidget *button,
						      AgsMidiPreferences *midi_preferences)
{
  //TODO:JK: implement me
}
