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

#ifndef __AGS_LV2_BRIDGE_CALLBACKS_H__
#define __AGS_LV2_BRIDGE_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/machine/ags_lv2_bridge.h>

#include <lv2.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

G_BEGIN_DECLS

void ags_lv2_bridge_lv2ui_cleanup_function(LV2UI_Handle handle);
void ags_lv2_bridge_lv2ui_write_function(LV2UI_Controller controller, uint32_t port_index, uint32_t buffer_size, uint32_t port_protocol, const void *buffer);

void ags_lv2_bridge_program_changed_callback(GtkComboBox *combo_box, AgsLv2Bridge *lv2_bridge);
void ags_lv2_bridge_preset_changed_callback(GtkComboBox *combo_box, AgsLv2Bridge *lv2_bridge);

void ags_lv2_bridge_dial_changed_callback(GtkWidget *dial, AgsLv2Bridge *lv2_bridge);
void ags_lv2_bridge_scale_changed_callback(GtkWidget *scale, AgsLv2Bridge *lv2_bridge);
void ags_lv2_bridge_spin_button_changed_callback(GtkWidget *spin_button, AgsLv2Bridge *lv2_bridge);
void ags_lv2_bridge_check_button_toggled_callback(GtkWidget *check_button, AgsLv2Bridge *lv2_bridge);
void ags_lv2_bridge_toggle_button_toggled_callback(GtkWidget *toggle_button, AgsLv2Bridge *lv2_bridge);
void ags_lv2_bridge_button_clicked_callback(GtkWidget *button, AgsLv2Bridge *lv2_bridge);

G_END_DECLS

#endif /*__AGS_LV2_BRIDGE_CALLBACKS_H__*/
