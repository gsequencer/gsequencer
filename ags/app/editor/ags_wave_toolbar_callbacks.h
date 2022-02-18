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

#ifndef __AGS_WAVE_TOOLBAR_CALLBACKS_H__
#define __AGS_WAVE_TOOLBAR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/editor/ags_wave_toolbar.h>

G_BEGIN_DECLS

void ags_wave_toolbar_position_callback(GtkToggleToolButton *toggle_tool_button, AgsWaveToolbar *wave_toolbar);
void ags_wave_toolbar_select_callback(GtkToggleToolButton *toggle_tool_button, AgsWaveToolbar *wave_toolbar);

void ags_wave_toolbar_copy_or_cut_callback(GtkWidget *widget, AgsWaveToolbar *wave_toolbar);
void ags_wave_toolbar_paste_callback(GtkWidget *widget, AgsWaveToolbar *wave_toolbar);

void ags_wave_toolbar_match_line_callback(GtkWidget *widget, AgsWaveToolbar *wave_toolbar);

void ags_wave_toolbar_tool_popup_select_buffer_callback(GtkWidget *item, AgsWaveToolbar *wave_toolbar);
void ags_wave_toolbar_tool_popup_position_cursor_callback(GtkWidget *item, AgsWaveToolbar *wave_toolbar);

void ags_wave_toolbar_tool_popup_enable_all_lines_callback(GtkWidget *item, AgsWaveToolbar *wave_toolbar);
void ags_wave_toolbar_tool_popup_disable_all_lines_callback(GtkWidget *item, AgsWaveToolbar *wave_toolbar);

void ags_wave_toolbar_zoom_callback(GtkComboBox *combo_box, AgsWaveToolbar *wave_toolbar);

void ags_wave_toolbar_opacity_callback(GtkSpinButton *spin_button, AgsWaveToolbar *wave_toolbar);

G_END_DECLS

#endif /*__AGS_WAVE_TOOLBAR_CALLBACKS_H__*/
