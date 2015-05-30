/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_AUTOMATION_EDITOR_CALLBACKS_H__
#define __AGS_AUTOMATION_EDITOR_CALLBACKS_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_audio.h>

#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_navigation.h>
#include <ags/X/ags_machine.h>

#include <ags/X/editor/ags_machine_selector.h>

void ags_automation_editor_set_audio_channels_callback(AgsAudio *audio,
						       guint audio_channels, guint audio_channels_old,
						       AgsAutomationEditor *automation_editor);
void ags_automation_editor_set_pads_callback(AgsAudio *audio,
					     GType channel_type,
					     guint pads, guint pads_old,
					     AgsAutomationEditor *automation_editor);

void ags_automation_editor_machine_changed_callback(AgsMachineSelector *machine_selector, AgsMachine *machine,
						    AgsAutomationEditor *automation_editor);

void ags_automation_editor_change_position_callback(AgsNavigation *navigation, gdouble tact,
						    AgsAutomationEditor *automation_editor);

void ags_automation_editor_edit_vscrollbar_value_changed_callback(GtkWidget *note_edit,
								  AgsAutomationEditor *automation_editor);

#endif /*__AGS_AUTOMATION_EDITOR_CALLBACKS_H__*/
