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

#ifndef __AGS_MACHINE_EDITOR_CALLBACKS_H__
#define __AGS_MACHINE_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_machine_editor.h>

int ags_machine_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMachineEditor *machine_editor);
int ags_machine_editor_destroy_callback(GtkObject *object, AgsMachineEditor *machine_editor);
int ags_machine_editor_show_callback(GtkWidget *widget, AgsMachineEditor *machine_editor);

int ags_machine_editor_switch_page_callback(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, AgsMachineEditor *machine_editor);

int ags_machine_editor_add_callback(GtkWidget *widget, AgsMachineEditor *machine_editor);
int ags_machine_editor_remove_callback(GtkWidget *widget, AgsMachineEditor *machine_editor);
int ags_machine_editor_back_callback(GtkWidget *button, AgsMachineEditor *machine_editor);

int ags_machine_editor_apply_callback(GtkWidget *widget, AgsMachineEditor *machine_editor);
int ags_machine_editor_ok_callback(GtkWidget *widget, AgsMachineEditor *machine_editor);
int ags_machine_editor_cancel_callback(GtkWidget *widget, AgsMachineEditor *machine_editor);

#endif /*__AGS_MACHINE_EDITOR_CALLBACKS_H__*/
