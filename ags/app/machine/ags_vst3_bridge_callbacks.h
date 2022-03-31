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

#ifndef __AGS_VST3_BRIDGE_CALLBACKS_H__
#define __AGS_VST3_BRIDGE_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/machine/ags_vst3_bridge.h>

G_BEGIN_DECLS

void ags_vst3_bridge_show_gui_callback(GtkMenuItem *item, AgsVst3Bridge *vst3_bridge);

void ags_vst3_bridge_program_changed_callback(GtkComboBox *combo_box, AgsVst3Bridge *vst3_bridge);

AgsVstTResult ags_vst3_bridge_perform_edit_callback(AgsVstIComponentHandler *icomponent_handler, AgsVstParamID id, AgsVstParamValue value_normalized, AgsVst3Bridge *vst3_bridge);
AgsVstTResult ags_vst3_bridge_restart_component_callback(AgsVstIComponentHandler *icomponent_handler, guint32 flags, AgsVst3Bridge *vst3_bridge);

void ags_vst3_bridge_dial_changed_callback(GtkWidget *dial, AgsVst3Bridge *vst3_bridge);
void ags_vst3_bridge_scale_changed_callback(GtkWidget *scale, AgsVst3Bridge *vst3_bridge);
void ags_vst3_bridge_spin_button_changed_callback(GtkWidget *spin_button, AgsVst3Bridge *vst3_bridge);
void ags_vst3_bridge_check_button_clicked_callback(GtkWidget *check_button, AgsVst3Bridge *vst3_bridge);
void ags_vst3_bridge_toggle_button_clicked_callback(GtkWidget *toggle_button, AgsVst3Bridge *vst3_bridge);
void ags_vst3_bridge_button_clicked_callback(GtkWidget *button, AgsVst3Bridge *vst3_bridge);

G_END_DECLS

#endif /*__AGS_VST3_BRIDGE_CALLBACKS_H__*/
