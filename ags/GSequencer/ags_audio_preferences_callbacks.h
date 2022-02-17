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

#ifndef __AGS_AUDIO_PREFERENCES_CALLBACKS_H__
#define __AGS_AUDIO_PREFERENCES_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/GSequencer/ags_audio_preferences.h>

G_BEGIN_DECLS

void ags_audio_preferences_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsAudioPreferences *audio_preferences);

void ags_audio_preferences_add_callback(GtkWidget *widget, AgsAudioPreferences *audio_preferences);
void ags_audio_preferences_remove_soundcard_editor_callback(GtkWidget *soundcard_editor,
							    AgsAudioPreferences *audio_preferences);

void ags_audio_preferences_enable_jack_callback(GtkCheckButton *check_button,
						AgsAudioPreferences *audio_preferences);
void ags_audio_preferences_start_jack_callback(GtkButton *button,
					       AgsAudioPreferences *audio_preferences);
void ags_audio_preferences_stop_jack_callback(GtkButton *button,
					      AgsAudioPreferences *audio_preferences);

G_END_DECLS

#endif /*__AGS_AUDIO_PREFERENCES_CALLBACKS_H__*/
