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

#ifndef __AGS_SOUNCARD_EDITOR_CALLBACKS_H__
#define __AGS_SOUNCARD_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/ags_soundcard_editor.h>

void ags_soundcard_editor_backend_changed_callback(GtkComboBox *combo,
						   AgsSoundcardEditor *soundcard_editor);

void ags_soundcard_editor_card_changed_callback(GtkComboBox *combo,
						AgsSoundcardEditor *soundcard_editor);
void ags_soundcard_editor_add_jack_callback(GtkWidget *button,
					    AgsSoundcardEditor *soundcard_editor);
void ags_soundcard_editor_remove_jack_callback(GtkWidget *button,
					       AgsSoundcardEditor *soundcard_editor);

void ags_soundcard_editor_audio_channels_changed_callback(GtkSpinButton *spin_button,
							  AgsSoundcardEditor *soundcard_editor);
void ags_soundcard_editor_samplerate_changed_callback(GtkSpinButton *spin_button,
						      AgsSoundcardEditor *soundcard_editor);
void ags_soundcard_editor_buffer_size_changed_callback(GtkSpinButton *spin_button,
						       AgsSoundcardEditor *soundcard_editor);
void ags_soundcard_editor_format_changed_callback(GtkComboBox *combo_box,
						  AgsSoundcardEditor *soundcard_editor);

#endif /*__AGS_SOUNDCARD_EDITOR_CALLBACKS_H__*/
