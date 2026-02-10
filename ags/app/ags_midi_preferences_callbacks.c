/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#include <ags/app/ags_midi_preferences_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_preferences.h>
#include <ags/app/ags_sequencer_editor.h>

#include <ags/i18n.h>

#include <ags/config.h>

void
ags_midi_preferences_notify_parent_callback(GObject *gobject,
					    GParamSpec *pspec,
					    gpointer user_data)
{
  //empty
}

void
ags_midi_preferences_add_alsa_output_sequencer_callback(GAction *action, GVariant *parameter,
							AgsMidiPreferences *midi_preferences)
{
  ags_midi_preferences_add_sequencer(midi_preferences,
				     "alsa",
				     TRUE);
}

void
ags_midi_preferences_add_alsa_input_sequencer_callback(GAction *action, GVariant *parameter,
						       AgsMidiPreferences *midi_preferences)
{
  ags_midi_preferences_add_sequencer(midi_preferences,
				     "alsa",
				     FALSE);
}

void
ags_midi_preferences_add_oss_output_sequencer_callback(GAction *action, GVariant *parameter,
						       AgsMidiPreferences *midi_preferences)
{
  ags_midi_preferences_add_sequencer(midi_preferences,
				     "oss",
				     TRUE);
}

void
ags_midi_preferences_add_oss_input_sequencer_callback(GAction *action, GVariant *parameter,
						      AgsMidiPreferences *midi_preferences)
{
  ags_midi_preferences_add_sequencer(midi_preferences,
				     "oss",
				     FALSE);
}

void
ags_midi_preferences_add_jack_output_sequencer_callback(GAction *action, GVariant *parameter,
							AgsMidiPreferences *midi_preferences)
{
  ags_midi_preferences_add_sequencer(midi_preferences,
				     "jack",
				     TRUE);
}

void
ags_midi_preferences_add_jack_input_sequencer_callback(GAction *action, GVariant *parameter,
						       AgsMidiPreferences *midi_preferences)
{
  ags_midi_preferences_add_sequencer(midi_preferences,
				     "jack",
				     FALSE);
}

void
ags_midi_preferences_add_pulse_output_sequencer_callback(GAction *action, GVariant *parameter,
							 AgsMidiPreferences *midi_preferences)
{
  ags_midi_preferences_add_sequencer(midi_preferences,
				     "pulse",
				     TRUE);
}

void
ags_midi_preferences_add_pulse_input_sequencer_callback(GAction *action, GVariant *parameter,
							AgsMidiPreferences *midi_preferences)
{
  ags_midi_preferences_add_sequencer(midi_preferences,
				     "pulse",
				     FALSE);
}

void
ags_midi_preferences_add_core_midi_output_sequencer_callback(GAction *action, GVariant *parameter,
							     AgsMidiPreferences *midi_preferences)
{
  ags_midi_preferences_add_sequencer(midi_preferences,
				     "core-midi",
				     TRUE);
}

void
ags_midi_preferences_add_core_midi_input_sequencer_callback(GAction *action, GVariant *parameter,
							    AgsMidiPreferences *midi_preferences)
{
  ags_midi_preferences_add_sequencer(midi_preferences,
				     "core-midi",
				     FALSE);
}

void
ags_midi_preferences_add_wasapi_output_sequencer_callback(GAction *action, GVariant *parameter,
							  AgsMidiPreferences *midi_preferences)
{
  ags_midi_preferences_add_sequencer(midi_preferences,
				     "wasapi",
				     TRUE);
}

void
ags_midi_preferences_add_wasapi_input_sequencer_callback(GAction *action, GVariant *parameter,
							 AgsMidiPreferences *midi_preferences)
{
  ags_midi_preferences_add_sequencer(midi_preferences,
				     "wasapi",
				     FALSE);
}

void
ags_midi_preferences_remove_sequencer_editor_callback(GtkWidget *button,
						      AgsMidiPreferences *midi_preferences)
{
  AgsSequencerEditor *sequencer_editor;
  
  sequencer_editor = (AgsSequencerEditor *) gtk_widget_get_ancestor(button,
								    AGS_TYPE_SEQUENCER_EDITOR);

  ags_midi_preferences_remove_sequencer_editor(midi_preferences,
					       sequencer_editor);
}
