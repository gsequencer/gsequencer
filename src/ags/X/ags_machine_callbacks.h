/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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
#ifndef __AGS_MACHINE_CALLBACKS_H__
#define __AGS_MACHINE_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_machine.h>

int ags_machine_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMachine *machine);
int ags_machine_destroy_callback(GtkObject *object, AgsMachine *machine);

int ags_machine_button_press_callback(GtkWidget *handle_box, GdkEventButton *event, AgsMachine *machine);

int ags_machine_popup_move_up_activate_callback(GtkWidget *widget, AgsMachine *machine);
int ags_machine_popup_move_down_activate_callback(GtkWidget *widget, AgsMachine *machine);
int ags_machine_popup_hide_activate_callback(GtkWidget *widget, AgsMachine *machine);
int ags_machine_popup_show_activate_callback(GtkWidget *widget, AgsMachine *machine);
void ags_machine_popup_destroy_activate_callback(GtkWidget *widget, AgsMachine *machine);
int ags_machine_popup_rename_activate_callback(GtkWidget *widget, AgsMachine *machine);
int ags_machine_popup_properties_activate_callback(GtkWidget *widget, AgsMachine *machine);

void ags_machine_open_response_callback(GtkDialog *dialog, gint response, AgsMachine *machine);

void ags_machine_play_callback(GtkWidget *toggle_button, AgsMachine *machine);
void ags_machine_play_delay_done(AgsRecall *recall, AgsRecallID *recall_id, AgsMachine *machine);

#endif /*__AGS_MACHINE_CALLBACKS_H__*/
