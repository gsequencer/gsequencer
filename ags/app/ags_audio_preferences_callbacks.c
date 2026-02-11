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

#include <ags/app/ags_audio_preferences_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_preferences.h>
#include <ags/app/ags_soundcard_editor.h>

#include <ags/config.h>
#include <ags/i18n.h>

void
ags_audio_preferences_notify_parent_callback(GObject *gobject,
					     GParamSpec *pspec,
					     gpointer user_data)
{
  //empty
}

void
ags_audio_preferences_add_alsa_output_soundcard_callback(GAction *action, GVariant *parameter,
							 AgsAudioPreferences *audio_preferences)
{
  ags_audio_preferences_add_soundcard(audio_preferences,
				      "alsa",
				      TRUE);
}

void
ags_audio_preferences_add_alsa_input_soundcard_callback(GAction *action, GVariant *parameter,
							AgsAudioPreferences *audio_preferences)
{
  ags_audio_preferences_add_soundcard(audio_preferences,
				      "alsa",
				      FALSE);
}

void
ags_audio_preferences_add_oss_output_soundcard_callback(GAction *action, GVariant *parameter,
							AgsAudioPreferences *audio_preferences)
{
  ags_audio_preferences_add_soundcard(audio_preferences,
				      "oss",
				      TRUE);
}

void
ags_audio_preferences_add_oss_input_soundcard_callback(GAction *action, GVariant *parameter,
						       AgsAudioPreferences *audio_preferences)
{
  ags_audio_preferences_add_soundcard(audio_preferences,
				      "oss",
				      FALSE);
}

void
ags_audio_preferences_add_jack_output_soundcard_callback(GAction *action, GVariant *parameter,
							 AgsAudioPreferences *audio_preferences)
{
  ags_audio_preferences_add_soundcard(audio_preferences,
				      "jack",
				      TRUE);
}

void
ags_audio_preferences_add_jack_input_soundcard_callback(GAction *action, GVariant *parameter,
							AgsAudioPreferences *audio_preferences)
{
  ags_audio_preferences_add_soundcard(audio_preferences,
				      "jack",
				      FALSE);
}

void
ags_audio_preferences_add_pulse_output_soundcard_callback(GAction *action, GVariant *parameter,
							  AgsAudioPreferences *audio_preferences)
{
  ags_audio_preferences_add_soundcard(audio_preferences,
				      "pulse",
				      TRUE);
}

void
ags_audio_preferences_add_pulse_input_soundcard_callback(GAction *action, GVariant *parameter,
							 AgsAudioPreferences *audio_preferences)
{
  ags_audio_preferences_add_soundcard(audio_preferences,
				      "pulse",
				      FALSE);
}

void
ags_audio_preferences_add_core_audio_output_soundcard_callback(GAction *action, GVariant *parameter,
							       AgsAudioPreferences *audio_preferences)
{
  ags_audio_preferences_add_soundcard(audio_preferences,
				      "core-audio",
				      TRUE);
}

void
ags_audio_preferences_add_core_audio_input_soundcard_callback(GAction *action, GVariant *parameter,
							      AgsAudioPreferences *audio_preferences)
{
  ags_audio_preferences_add_soundcard(audio_preferences,
				      "core-audio",
				      FALSE);
}

void
ags_audio_preferences_add_wasapi_output_soundcard_callback(GAction *action, GVariant *parameter,
							   AgsAudioPreferences *audio_preferences)
{
  ags_audio_preferences_add_soundcard(audio_preferences,
				      "wasapi",
				      TRUE);
}

void
ags_audio_preferences_add_wasapi_input_soundcard_callback(GAction *action, GVariant *parameter,
							  AgsAudioPreferences *audio_preferences)
{
  ags_audio_preferences_add_soundcard(audio_preferences,
				      "wasapi",
				      FALSE);
}

void
ags_audio_preferences_remove_soundcard_editor_callback(GtkWidget *button,
						       AgsAudioPreferences *audio_preferences)
{
  AgsSoundcardEditor *soundcard_editor;

  GList *start_list;
  
  soundcard_editor = (AgsSoundcardEditor *) gtk_widget_get_ancestor(button,
								    AGS_TYPE_SOUNDCARD_EDITOR);
  
  ags_audio_preferences_remove_soundcard_editor(audio_preferences,
						soundcard_editor);
}

void
ags_audio_preferences_start_jack_callback(GtkButton *button,
					  AgsAudioPreferences *audio_preferences)
{
  //TODO:JK: implement me
}

void
ags_audio_preferences_stop_jack_callback(GtkButton *button,
					 AgsAudioPreferences *audio_preferences)
{
  //TODO:JK: implement me
}
