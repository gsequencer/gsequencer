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

#ifndef __AGS_MACHINE_SELECTOR_CALLBACKS_H__
#define __AGS_MACHINE_SELECTOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/editor/ags_machine_selector.h>

G_BEGIN_DECLS

void ags_machine_selector_add_index_callback(GAction *action, GVariant *parameter,
					     AgsMachineSelector *machine_selector);
void ags_machine_selector_remove_index_callback(GAction *action, GVariant *parameter,
						AgsMachineSelector *machine_selector);
void ags_machine_selector_link_index_callback(GAction *action, GVariant *parameter,
					      AgsMachineSelector *machine_selector);

void ags_machine_selector_radio_changed(GtkWidget *radio_button, AgsMachineSelector *machine_selector);

void ags_machine_selector_reverse_mapping_callback(GAction *action, GVariant *parameter,
						   AgsMachineSelector *machine_selector);

void ags_machine_selector_shift_a_callback(GAction *action, GVariant *parameter,
					   AgsMachineSelector *machine_selector);
void ags_machine_selector_shift_a_sharp_callback(GAction *action, GVariant *parameter,
						 AgsMachineSelector *machine_selector);
void ags_machine_selector_shift_h_callback(GAction *action, GVariant *parameter,
					   AgsMachineSelector *machine_selector);
void ags_machine_selector_shift_c_callback(GAction *action, GVariant *parameter,
					   AgsMachineSelector *machine_selector);
void ags_machine_selector_shift_c_sharp_callback(GAction *action, GVariant *parameter,
						 AgsMachineSelector *machine_selector);
void ags_machine_selector_shift_d_callback(GAction *action, GVariant *parameter,
					   AgsMachineSelector *machine_selector);
void ags_machine_selector_shift_d_sharp_callback(GAction *action, GVariant *parameter,
						 AgsMachineSelector *machine_selector);
void ags_machine_selector_shift_e_callback(GAction *action, GVariant *parameter,
					   AgsMachineSelector *machine_selector);
void ags_machine_selector_shift_f_callback(GAction *action, GVariant *parameter,
					   AgsMachineSelector *machine_selector);
void ags_machine_selector_shift_f_sharp_callback(GAction *action, GVariant *parameter,
						 AgsMachineSelector *machine_selector);
void ags_machine_selector_shift_g_callback(GAction *action, GVariant *parameter,
					   AgsMachineSelector *machine_selector);
void ags_machine_selector_shift_g_sharp_callback(GAction *action, GVariant *parameter,
						 AgsMachineSelector *machine_selector);

G_END_DECLS

#endif /*__AGS_MACHINE_SELECTOR_CALLBACKS_H__*/
