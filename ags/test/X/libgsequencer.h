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

#ifndef __LIBGSEQUENCER_H__
#define __LIBGSEQUENCER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/ags_midi_preferences_callbacks.h>
#include <ags/X/ags_automation_editor_callbacks.h>
#include <ags/X/ags_performance_preferences_callbacks.h>
#include <ags/X/ags_soundcard_editor_callbacks.h>
#include <ags/X/ags_line_editor_callbacks.h>
#include <ags/X/ags_output_listing_editor_callbacks.h>
#include <ags/X/ags_menu_bar.h>
#include <ags/X/ags_editor_callbacks.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad_callbacks.h>
#include <ags/X/ags_effect_bulk_callbacks.h>
#include <ags/X/ags_export_soundcard.h>
#include <ags/X/ags_lv2_browser_callbacks.h>
#include <ags/X/ags_dssi_browser.h>
#include <ags/X/ags_line.h>
#include <ags/X/ags_ladspa_browser_callbacks.h>
#include <ags/X/ags_pad_editor_callbacks.h>
#include <ags/X/file/ags_simple_file.h>
#include <ags/X/file/ags_gui_file_xml.h>
#include <ags/X/ags_line_member_editor_callbacks.h>
#include <ags/X/ags_editor.h>
#include <ags/X/ags_plugin_preferences_callbacks.h>
#include <ags/X/editor/ags_sf2_chooser.h>
#include <ags/X/editor/ags_machine_selector_callbacks.h>
#include <ags/X/editor/ags_automation_edit_callbacks.h>
#include <ags/X/editor/ags_meter.h>
#include <ags/X/editor/ags_automation_area.h>
#include <ags/X/editor/ags_scale_area_callbacks.h>
#include <ags/X/editor/ags_machine_radio_button.h>
#include <ags/X/editor/ags_pattern_edit_callbacks.h>
#include <ags/X/editor/ags_automation_edit.h>
#include <ags/X/editor/ags_toolbar_callbacks.h>
#include <ags/X/editor/ags_inline_player.h>
#include <ags/X/editor/ags_sf2_chooser_callbacks.h>
#include <ags/X/editor/ags_note_edit.h>
#include <ags/X/editor/ags_pattern_edit.h>
#include <ags/X/editor/ags_file_selection.h>
#include <ags/X/editor/ags_scale_area.h>
#include <ags/X/editor/ags_automation_area_callbacks.h>
#include <ags/X/editor/ags_file_selection_callbacks.h>
#include <ags/X/editor/ags_automation_toolbar_callbacks.h>
#include <ags/X/editor/ags_toolbar.h>
#include <ags/X/editor/ags_note_edit_callbacks.h>
#include <ags/X/editor/ags_inline_player_callbacks.h>
#include <ags/X/editor/ags_toolbar_mode_stock.h>
#include <ags/X/editor/ags_notebook_callbacks.h>
#include <ags/X/editor/ags_scale_callbacks.h>
#include <ags/X/editor/ags_machine_selector.h>
#include <ags/X/editor/ags_machine_selection.h>
#include <ags/X/editor/ags_scale.h>
#include <ags/X/editor/ags_notebook.h>
#include <ags/X/editor/ags_meter_callbacks.h>
#include <ags/X/editor/ags_automation_toolbar.h>
#include <ags/X/ags_link_editor.h>
#include <ags/X/import/ags_track_collection.h>
#include <ags/X/import/ags_track_collection_mapper.h>
#include <ags/X/import/ags_midi_import_wizard.h>
#include <ags/X/import/ags_midi_import_wizard_callbacks.h>
#include <ags/X/import/ags_track_collection_mapper_callbacks.h>
#include <ags/X/import/ags_track_collection_callbacks.h>
#include <ags/X/ags_midi_dialog_callbacks.h>
#include <ags/X/ags_export_soundcard_callbacks.h>
#include <ags/X/ags_navigation_callbacks.h>
#include <ags/X/ags_plugin_preferences.h>
#include <ags/X/machine/ags_synth.h>
#include <ags/X/machine/ags_matrix_bridge.h>
#include <ags/X/machine/ags_drum_output_line.h>
#include <ags/X/machine/ags_pattern_box_callbacks.h>
#include <ags/X/machine/ags_ffplayer_callbacks.h>
#include <ags/X/machine/ags_synth_input_line.h>
#include <ags/X/machine/ags_ffplayer_bridge.h>
#include <ags/X/machine/ags_ffplayer_input_line.h>
#include <ags/X/machine/ags_mixer_callbacks.h>
#include <ags/X/machine/ags_lv2_bridge_callbacks.h>
#include <ags/X/machine/ags_matrix_bridge_callbacks.h>
#include <ags/X/machine/ags_mixer_input_line.h>
#include <ags/X/machine/ags_ffplayer_bulk_input_callbacks.h>
#include <ags/X/machine/ags_ffplayer_input_line_callbacks.h>
#include <ags/X/machine/ags_oscillator.h>
#include <ags/X/machine/ags_pattern_box.h>
#include <ags/X/machine/ags_dssi_bridge.h>
#include <ags/X/machine/ags_matrix_bulk_input_callbacks.h>
#include <ags/X/machine/ags_mixer.h>
#include <ags/X/machine/ags_panel_input_line_callbacks.h>
#include <ags/X/machine/ags_ffplayer.h>
#include <ags/X/machine/ags_cell_pattern.h>
#include <ags/X/machine/ags_mixer_input_pad.h>
#include <ags/X/machine/ags_dssi_bridge_callbacks.h>
#include <ags/X/machine/ags_drum_input_pad.h>
#include <ags/X/machine/ags_oscillator_callbacks.h>
#include <ags/X/machine/ags_matrix_bulk_input.h>
#include <ags/X/machine/ags_synth_input_pad.h>
#include <ags/X/machine/ags_matrix_callbacks.h>
#include <ags/X/machine/ags_ffplayer_bridge_callbacks.h>
#include <ags/X/machine/ags_ladspa_bridge_callbacks.h>
#include <ags/X/machine/ags_drum_output_pad_callbacks.h>
#include <ags/X/machine/ags_ffplayer_bulk_input.h>
#include <ags/X/machine/ags_cell_pattern_callbacks.h>
#include <ags/X/machine/ags_drum_input_line_callbacks.h>
#include <ags/X/machine/ags_panel.h>
#include <ags/X/machine/ags_lv2_bridge.h>
#include <ags/X/machine/ags_panel_input_line.h>
#include <ags/X/machine/ags_ladspa_bridge.h>
#include <ags/X/machine/ags_panel_input_pad.h>
#include <ags/X/machine/ags_synth_callbacks.h>
#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_drum.h>
#include <ags/X/machine/ags_drum_callbacks.h>
#include <ags/X/machine/ags_drum_output_line_callbacks.h>
#include <ags/X/machine/ags_panel_callbacks.h>
#include <ags/X/machine/ags_drum_input_line.h>
#include <ags/X/machine/ags_ffplayer_input_pad.h>
#include <ags/X/machine/ags_drum_output_pad.h>
#include <ags/X/machine/ags_ffplayer_input_pad_callbacks.h>
#include <ags/X/machine/ags_drum_input_pad_callbacks.h>
#include <ags/X/ags_lv2_browser.h>
#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_line_editor.h>
#include <ags/X/ags_sequencer_editor.h>
#include <ags/X/ags_effect_line_callbacks.h>
#include <ags/X/ags_listing_editor_callbacks.h>
#include <ags/X/ags_soundcard_editor.h>
#include <ags/X/ags_machine_callbacks.h>
#include <ags/X/ags_dssi_browser_callbacks.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line_member.h>
#include <ags/X/ags_sequencer_editor_callbacks.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_machine_editor_callbacks.h>
#include <ags/X/ags_effect_bridge_callbacks.h>
#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_output_collection_editor_callbacks.h>
#include <ags/X/ags_plugin_browser.h>
#include <ags/X/ags_property_editor.h>
#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_server_preferences.h>
#include <ags/X/ags_connection_editor.h>
#include <ags/X/ags_export_window.h>
#include <ags/X/ags_export_window_callbacks.h>
#include <ags/X/ags_link_collection_editor_callbacks.h>
#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_output_editor.h>
#include <ags/X/ags_listing_editor.h>
#include <ags/X/ags_preferences.h>
#include <ags/X/ags_automation_window_callbacks.h>
#include <ags/X/ags_menu_bar_callbacks.h>
#include <ags/X/ags_plugin_browser_callbacks.h>
#include <ags/X/ags_generic_preferences_callbacks.h>
#include <ags/X/ags_property_collection_editor.h>
#include <ags/X/ags_automation_window.h>
#include <ags/X/ags_connection_editor_callbacks.h>
#include <ags/X/ags_property_collection_editor_callbacks.h>
#include <ags/X/ags_server_preferences_callbacks.h>
#include <ags/X/ags_link_collection_editor.h>
#include <ags/X/ags_output_editor_callbacks.h>
#include <ags/X/ags_effect_pad.h>
#include <ags/X/ags_output_collection_editor.h>
#include <ags/X/task/ags_blink_cell_pattern_cursor.h>
#include <ags/X/task/ags_toggle_led.h>
#include <ags/X/task/ags_add_soundcard_editor_sink.h>
#include <ags/X/task/ags_remove_soundcard_editor_sink.h>
#include <ags/X/task/ags_display_tact.h>
#include <ags/X/task/ags_scroll_on_play.h>
#include <ags/X/task/ags_change_tact.h>
#include <ags/X/task/ags_remove_sequencer_editor_jack.h>
#include <ags/X/task/ags_add_line_member.h>
#include <ags/X/task/ags_simple_file_write.h>
#include <ags/X/task/ags_add_sequencer_editor_jack.h>
#include <ags/X/task/ags_update_bulk_member.h>
#include <ags/X/task/ags_change_indicator.h>
#include <ags/X/task/ags_simple_file_read.h>
#include <ags/X/task/ags_add_bulk_member.h>
#include <ags/X/ags_link_editor_callbacks.h>
#include <ags/X/ags_effect_pad_callbacks.h>
#include <ags/X/ags_window_callbacks.h>
#include <ags/X/ags_bulk_member.h>
#include <ags/X/thread/ags_gui_thread.h>
#include <ags/X/thread/ags_simple_autosave_thread.h>
#include <ags/X/ags_effect_line.h>
#include <ags/X/ags_line_member_callbacks.h>
#include <ags/X/ags_resize_editor.h>
#include <ags/X/ags_midi_preferences.h>
#include <ags/X/ags_history_browser_callbacks.h>
#include <ags/X/ags_ladspa_browser.h>
#include <ags/X/ags_history_browser.h>
#include <ags/X/ags_property_listing_editor.h>
#include <ags/X/ags_performance_preferences.h>
#include <ags/X/ags_audio_preferences.h>
#include <ags/X/ags_generic_preferences.h>
#include <ags/X/ags_bulk_member_callbacks.h>
#include <ags/X/ags_property_editor_callbacks.h>
#include <ags/X/ags_line_member_editor.h>
#include <ags/X/ags_preferences_callbacks.h>
#include <ags/X/ags_output_listing_editor.h>
#include <ags/X/ags_audio_preferences_callbacks.h>
#include <ags/X/ags_navigation.h>
#include <ags/X/ags_line_callbacks.h>
#include <ags/X/ags_pad_editor.h>
#include <ags/X/ags_midi_dialog.h>
#include <ags/X/ags_effect_bulk.h>

#endif /*__LIBGSEQUENCER_H__*/
