/* This file is part of GSequencer.
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

#ifndef __AGS_NOTE_EDIT_CALLBACKS_H__
#define __AGS_NOTE_EDIT_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/editor/ags_note_edit.h>

void ags_note_edit_set_audio_channels_callback(AgsAudio *audio,
					       guint audio_channels, guint audio_channels_old,
					       AgsNoteEdit *note_edit);
void ags_note_edit_set_pads_callback(AgsAudio *audio,
				     GType channel_type,
				     guint pads, guint pads_old,
				     AgsNoteEdit *note_edit);

gboolean ags_note_edit_drawing_area_expose_event(GtkWidget *widget, GdkEventExpose *event, AgsNoteEdit *note_edit);
gboolean ags_note_edit_drawing_area_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsNoteEdit *note_edit);
gboolean ags_note_edit_drawing_area_button_press_event(GtkWidget *widget, GdkEventButton *event, AgsNoteEdit *note_edit);
gboolean ags_note_edit_drawing_area_button_release_event(GtkWidget *widget, GdkEventButton *event, AgsNoteEdit *note_edit);
gboolean ags_note_edit_drawing_area_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, AgsNoteEdit *note_edit);
gboolean ags_note_edit_drawing_area_key_press_event(GtkWidget *widget, GdkEventKey *event, AgsNoteEdit *note_edit);
gboolean ags_note_edit_drawing_area_key_release_event(GtkWidget *widget, GdkEventKey *event, AgsNoteEdit *note_edit);

void ags_note_edit_vscrollbar_value_changed(GtkRange *range, AgsNoteEdit *note_edit);
void ags_note_edit_hscrollbar_value_changed(GtkRange *range, AgsNoteEdit *note_edit);

#endif /*__AGS_NOTE_EDIT_CALLBACKS_H__*/
