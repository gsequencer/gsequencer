/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_MACHINE_CALLBACKS_H__
#define __AGS_MACHINE_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>

G_BEGIN_DECLS

void ags_machine_check_message_callback(GObject *application_context, AgsMachine *machine);

void ags_machine_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsMachine *machine);

void ags_machine_map_recall_callback(AgsMachine *machine,
				     gpointer user_data);

/* controls */
void ags_machine_move_up_callback(GAction *action, GVariant *parameter,
				  AgsMachine *machine);
void ags_machine_move_down_callback(GAction *action, GVariant *parameter,
				  AgsMachine *machine);
void ags_machine_hide_callback(GAction *action, GVariant *parameter,
			       AgsMachine *machine);
void ags_machine_show_callback(GAction *action, GVariant *parameter,
			       AgsMachine *machine);
void ags_machine_destroy_callback(GAction *action, GVariant *parameter,
				  AgsMachine *machine);
void ags_machine_rename_callback(GAction *action, GVariant *parameter,
				 AgsMachine *machine);
void ags_machine_rename_audio_callback(GAction *action, GVariant *parameter,
				       AgsMachine *machine);
void ags_machine_reposition_audio_callback(GAction *action, GVariant *parameter,
					   AgsMachine *machine);
void ags_machine_properties_callback(GAction *action, GVariant *parameter,
				     AgsMachine *machine);
void ags_machine_sticky_controls_callback(GAction *action, GVariant *parameter,
					  AgsMachine *machine);

void ags_machine_copy_pattern_callback(GAction *action, GVariant *parameter,
				       AgsMachine *machine);
void ags_machine_paste_pattern_callback(GAction *action, GVariant *parameter,
					AgsMachine *machine);
void ags_machine_envelope_callback(GAction *action, GVariant *parameter,
				   AgsMachine *machine);

void ags_machine_audio_connection_callback(GAction *action, GVariant *parameter,
					   AgsMachine *machine);
void ags_machine_midi_connection_callback(GAction *action, GVariant *parameter,
					  AgsMachine *machine);

void ags_machine_audio_export_callback(GAction *action, GVariant *parameter,
				       AgsMachine *machine);
void ags_machine_midi_export_callback(GAction *action, GVariant *parameter,
				      AgsMachine *machine);

void ags_machine_audio_import_callback(GAction *action, GVariant *parameter,
				       AgsMachine *machine);
void ags_machine_midi_import_callback(GAction *action, GVariant *parameter,
				      AgsMachine *machine);

void ags_machine_open_response_callback(GtkDialog *dialog, gint response, AgsMachine *machine);

void ags_machine_play_callback(GtkWidget *toggle_button, AgsMachine *machine);

/* forwarded callbacks */
void ags_machine_resize_audio_channels_callback(AgsMachine *machine,
						guint audio_channels, guint audio_channels_old,
						gpointer data);
void ags_machine_resize_pads_callback(AgsMachine *machine, GType channel_type,
				      guint pads, guint pads_old,
				      gpointer data);

void ags_machine_stop_callback(AgsMachine *machine,
			       GList *recall_id, gint sound_scope,
			       gpointer data);

/* library callbacks */
void ags_machine_active_playback_start_channel_launch_callback(AgsTask *task,
							       AgsPlayback *playback);

G_END_DECLS

#endif /*__AGS_MACHINE_CALLBACKS_H__*/
