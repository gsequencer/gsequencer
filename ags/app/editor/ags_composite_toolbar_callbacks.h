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

#ifndef __AGS_COMPOSITE_TOOLBAR_CALLBACKS_H__
#define __AGS_COMPOSITE_TOOLBAR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/editor/ags_composite_toolbar.h>

G_BEGIN_DECLS

void ags_composite_toolbar_position_callback(GtkToggleButton *button, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_edit_callback(GtkToggleButton *button, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_clear_callback(GtkToggleButton *button, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_select_callback(GtkToggleButton *button, AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_invert_callback(GtkButton *button, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_copy_callback(GtkButton *button, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_cut_callback(GtkButton *button, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_paste_callback(GtkButton *button, AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_paste_match_audio_channel_callback(GAction *action, GVariant *parameter,
							      AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_paste_match_line_callback(GAction *action, GVariant *parameter,
						     AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_paste_no_duplicates_callback(GAction *action, GVariant *parameter,
							AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_menu_tool_popup_notation_move_note_callback(GAction *action, GVariant *parameter,
								       AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_notation_crop_note_callback(GAction *action, GVariant *parameter,
								       AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_notation_select_note_callback(GAction *action, GVariant *parameter,
									 AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_notation_position_cursor_callback(GAction *action, GVariant *parameter,
									     AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_menu_tool_popup_sheet_position_cursor_callback(GAction *action, GVariant *parameter,
									  AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_sheet_add_page_callback(GAction *action, GVariant *parameter,
								   AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_sheet_remove_page_callback(GAction *action, GVariant *parameter,
								      AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_menu_tool_popup_automation_select_acceleration_callback(GAction *action, GVariant *parameter,
										   AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_automation_ramp_acceleration_callback(GAction *action, GVariant *parameter,
										 AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_automation_position_cursor_callback(GAction *action, GVariant *parameter,
									       AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_menu_tool_popup_wave_select_buffer_callback(GAction *action, GVariant *parameter,
								       AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_wave_position_cursor_callback(GAction *action, GVariant *parameter,
									 AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_wave_time_stretch_buffer_callback(GAction *action, GVariant *parameter,
									     AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_menu_tool_popup_enable_all_audio_channels_callback(GAction *action, GVariant *parameter,
									      AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_disable_all_audio_channels_callback(GAction *action, GVariant *parameter,
									       AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_enable_all_lines_callback(GAction *action, GVariant *parameter,
								     AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_disable_all_lines_callback(GAction *action, GVariant *parameter,
								      AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_port_callback(GtkComboBox *combo_box, AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_zoom_callback(GtkComboBox *combo_box, AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_opacity_callback(GtkSpinButton *spin_button, AgsCompositeToolbar *composite_toolbar);

G_END_DECLS

#endif /*__AGS_COMPOSITE_TOOLBAR_CALLBACKS_H__*/
