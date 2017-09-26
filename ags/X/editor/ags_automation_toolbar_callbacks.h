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

#ifndef __AGS_AUTOMATION_TOOLBAR_CALLBACKS_H__
#define __AGS_AUTOMATION_TOOLBAR_CALLBACKS_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_automation_editor.h>

#include <ags/X/editor/ags_automation_toolbar.h>

void ags_automation_toolbar_machine_changed_callback(AgsAutomationEditor *automation_editor,
						     AgsMachine *machine,
						     AgsAutomationToolbar *toolbar);

void ags_automation_toolbar_position_callback(GtkToggleButton *toggle_button, AgsAutomationToolbar *automation_toolbar);
void ags_automation_toolbar_edit_callback(GtkToggleButton *toggle_button, AgsAutomationToolbar *automation_toolbar);
void ags_automation_toolbar_clear_callback(GtkToggleButton *toggle_button, AgsAutomationToolbar *automation_toolbar);
void ags_automation_toolbar_select_callback(GtkToggleButton *toggle_button, AgsAutomationToolbar *automation_toolbar);

void ags_automation_toolbar_copy_or_cut_callback(GtkWidget *widget, AgsAutomationToolbar *automation_toolbar);
void ags_automation_toolbar_paste_callback(GtkWidget *widget, AgsAutomationToolbar *automation_toolbar);

void ags_automation_toolbar_tool_popup_select_acceleration_callback(GtkWidget *item, AgsToolbar *automation_toolbar);
void ags_automation_toolbar_tool_popup_ramp_acceleration_callback(GtkWidget *item, AgsToolbar *automation_toolbar);

void ags_automation_toolbar_zoom_callback(GtkComboBox *combo_box, AgsAutomationToolbar *automation_toolbar);

void ags_automation_toolbar_port_changed_callback(GtkComboBox *combo_box,
						  AgsAutomationToolbar *automation_toolbar);

#endif /*__AGS_AUTOMATION_TOOLBAR_CALLBACKS_H__*/
