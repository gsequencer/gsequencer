/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_LIVE_LV2_BRIDGE_CALLBACKS_H__
#define __AGS_LIVE_LV2_BRIDGE_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/machine/ags_live_lv2_bridge.h>

#include <lv2.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

void ags_live_lv2_bridge_show_gui_callback(GtkMenuItem *item, AgsLiveLv2Bridge *live_lv2_bridge);
gboolean ags_live_lv2_bridge_delete_event_callback(GtkWidget *widget, GdkEvent *event, AgsLiveLv2Bridge *live_lv2_bridge);

void ags_live_lv2_bridge_lv2ui_write_function(LV2UI_Controller controller, uint32_t port_index, uint32_t buffer_size, uint32_t port_protocol, const void *buffer);

void ags_live_lv2_bridge_program_changed_callback(GtkComboBox *combo_box, AgsLiveLv2Bridge *live_lv2_bridge);
void ags_live_lv2_bridge_preset_changed_callback(GtkComboBox *combo_box, AgsLiveLv2Bridge *live_lv2_bridge);

#endif /*__AGS_LIVE_LV2_BRIDGE_CALLBACKS_H__*/
