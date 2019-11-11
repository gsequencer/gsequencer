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

#ifndef __AGS_AUTOMATION_EDITOR_CALLBACKS_H__
#define __AGS_AUTOMATION_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_machine.h>

#include <ags/X/editor/ags_machine_selector.h>

G_BEGIN_DECLS

gboolean ags_automation_editor_audio_edit_expose_event(GtkWidget *widget, GdkEventExpose *event, AgsAutomationEditor *automation_editor);
gboolean ags_automation_editor_audio_edit_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsAutomationEditor *automation_editor);

gboolean ags_automation_editor_output_edit_expose_event(GtkWidget *widget, GdkEventExpose *event, AgsAutomationEditor *automation_editor);
gboolean ags_automation_editor_output_edit_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsAutomationEditor *automation_editor);

gboolean ags_automation_editor_input_edit_expose_event(GtkWidget *widget, GdkEventExpose *event, AgsAutomationEditor *automation_editor);
gboolean ags_automation_editor_input_edit_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsAutomationEditor *automation_editor);

void ags_automation_editor_audio_vscrollbar_value_changed(GtkRange *range, AgsAutomationEditor *automation_editor);
void ags_automation_editor_audio_hscrollbar_value_changed(GtkRange *range, AgsAutomationEditor *automation_editor);

void ags_automation_editor_output_vscrollbar_value_changed(GtkRange *range, AgsAutomationEditor *automation_editor);
void ags_automation_editor_output_hscrollbar_value_changed(GtkRange *range, AgsAutomationEditor *automation_editor);

void ags_automation_editor_input_vscrollbar_value_changed(GtkRange *range, AgsAutomationEditor *automation_editor);
void ags_automation_editor_input_hscrollbar_value_changed(GtkRange *range, AgsAutomationEditor *automation_editor);

void ags_automation_editor_audio_automation_edit_hscrollbar_value_changed(GtkRange *range, AgsAutomationEditor *automation_editor);
void ags_automation_editor_output_automation_edit_hscrollbar_value_changed(GtkRange *range, AgsAutomationEditor *automation_editor);
void ags_automation_editor_input_automation_edit_hscrollbar_value_changed(GtkRange *range, AgsAutomationEditor *automation_editor);

void ags_automation_editor_machine_changed_callback(AgsMachineSelector *machine_selector, AgsMachine *machine,
						    AgsAutomationEditor *automation_editor);

void ags_automation_editor_resize_audio_channels_callback(AgsMachine *machine,
							  guint audio_channels, guint audio_channels_old,
							  AgsAutomationEditor *automation_editor);
void ags_automation_editor_resize_pads_callback(AgsMachine *machine, GType channel_type,
						guint pads, guint pads_old,
						AgsAutomationEditor *automation_editor);

G_END_DECLS

#endif /*__AGS_AUTOMATION_EDITOR_CALLBACKS_H__*/
