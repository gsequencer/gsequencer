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

#ifndef __LIBGSEQUENCER_H__
#define __LIBGSEQUENCER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/ags_api_config.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_animation_window.h>
#include <ags/app/ags_audio_preferences.h>
#include <ags/app/ags_audio_preferences_callbacks.h>
#include <ags/app/ags_automation_editor.h>
#include <ags/app/ags_automation_editor_callbacks.h>
#include <ags/app/ags_automation_window.h>
#include <ags/app/ags_automation_window_callbacks.h>
#include <ags/app/ags_bulk_member.h>
#include <ags/app/ags_bulk_member_callbacks.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_composite_editor_callbacks.h>
#include <ags/app/ags_connection_editor.h>
#include <ags/app/ags_connection_editor_callbacks.h>
#include <ags/app/ags_context_menu.h>
#include <ags/app/ags_dssi_browser.h>
#include <ags/app/ags_dssi_browser_callbacks.h>
#include <ags/app/ags_effect_bridge.h>
#include <ags/app/ags_effect_bridge_callbacks.h>
#include <ags/app/ags_effect_bulk.h>
#include <ags/app/ags_effect_bulk_callbacks.h>
#include <ags/app/ags_effect_line.h>
#include <ags/app/ags_effect_line_callbacks.h>
#include <ags/app/ags_effect_pad.h>
#include <ags/app/ags_effect_pad_callbacks.h>
#include <ags/app/ags_effect_separator.h>
#include <ags/app/ags_export_soundcard.h>
#include <ags/app/ags_export_soundcard_callbacks.h>
#include <ags/app/ags_export_window.h>
#include <ags/app/ags_export_window_callbacks.h>
#include <ags/app/ags_generic_preferences.h>
#include <ags/app/ags_generic_preferences_callbacks.h>
#include <ags/app/ags_input_collection_editor.h>
#include <ags/app/ags_input_collection_editor_callbacks.h>
#include <ags/app/ags_input_editor.h>
#include <ags/app/ags_input_editor_callbacks.h>
#include <ags/app/ags_input_listing_editor.h>
#include <ags/app/ags_input_listing_editor_callbacks.h>
#include <ags/app/ags_ladspa_browser.h>
#include <ags/app/ags_ladspa_browser_callbacks.h>
#include <ags/app/ags_line.h>
#include <ags/app/ags_line_callbacks.h>
#include <ags/app/ags_line_editor.h>
#include <ags/app/ags_line_editor_callbacks.h>
#include <ags/app/ags_line_member.h>
#include <ags/app/ags_line_member_callbacks.h>
#include <ags/app/ags_line_member_editor.h>
#include <ags/app/ags_line_member_editor_callbacks.h>
#include <ags/app/ags_link_collection_editor.h>
#include <ags/app/ags_link_collection_editor_callbacks.h>
#include <ags/app/ags_link_editor.h>
#include <ags/app/ags_link_editor_callbacks.h>
#include <ags/app/ags_listing_editor.h>
#include <ags/app/ags_listing_editor_callbacks.h>
#include <ags/app/ags_lv2_browser.h>
#include <ags/app/ags_lv2_browser_callbacks.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_machine_callbacks.h>
#include <ags/app/ags_machine_editor.h>
#include <ags/app/ags_machine_editor_callbacks.h>
#include <ags/app/ags_machine_util.h>
#include <ags/app/ags_menu_action_callbacks.h>
#include <ags/app/ags_menu_bar.h>
#include <ags/app/ags_midi_dialog.h>
#include <ags/app/ags_midi_dialog_callbacks.h>
#include <ags/app/ags_midi_preferences.h>
#include <ags/app/ags_midi_preferences_callbacks.h>
#include <ags/app/ags_navigation.h>
#include <ags/app/ags_navigation_callbacks.h>
#include <ags/app/ags_notation_editor.h>
#include <ags/app/ags_notation_editor_callbacks.h>
#include <ags/app/ags_online_help_window.h>
#include <ags/app/ags_online_help_window_callbacks.h>
#include <ags/app/ags_osc_server_preferences.h>
#include <ags/app/ags_osc_server_preferences_callbacks.h>
#include <ags/app/ags_output_collection_editor.h>
#include <ags/app/ags_output_collection_editor_callbacks.h>
#include <ags/app/ags_output_editor.h>
#include <ags/app/ags_output_editor_callbacks.h>
#include <ags/app/ags_output_listing_editor.h>
#include <ags/app/ags_output_listing_editor_callbacks.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_pad_callbacks.h>
#include <ags/app/ags_pad_editor.h>
#include <ags/app/ags_pad_editor_callbacks.h>
#include <ags/app/ags_performance_preferences.h>
#include <ags/app/ags_performance_preferences_callbacks.h>
#include <ags/app/ags_playback_window.h>
#include <ags/app/ags_playback_window_callbacks.h>
#include <ags/app/ags_plugin_browser.h>
#include <ags/app/ags_plugin_browser_callbacks.h>
#include <ags/app/ags_plugin_preferences.h>
#include <ags/app/ags_plugin_preferences_callbacks.h>
#include <ags/app/ags_preferences.h>
#include <ags/app/ags_preferences_callbacks.h>
#include <ags/app/ags_property_collection_editor.h>
#include <ags/app/ags_property_collection_editor_callbacks.h>
#include <ags/app/ags_property_editor.h>
#include <ags/app/ags_property_editor_callbacks.h>
#include <ags/app/ags_property_listing_editor.h>
#include <ags/app/ags_quit_dialog.h>
#include <ags/app/ags_quit_dialog_callbacks.h>
#include <ags/app/ags_resize_editor.h>
#include <ags/app/ags_sequencer_editor.h>
#include <ags/app/ags_sequencer_editor_callbacks.h>
#include <ags/app/ags_server_preferences.h>
#include <ags/app/ags_server_preferences_callbacks.h>
#include <ags/app/ags_sheet_editor.h>
#include <ags/app/ags_sheet_editor_callbacks.h>
#include <ags/app/ags_sheet_window.h>
#include <ags/app/ags_sheet_window_callbacks.h>
#include <ags/app/ags_soundcard_editor.h>
#include <ags/app/ags_soundcard_editor_callbacks.h>
#include <ags/app/ags_ui_provider.h>

#if defined(AGS_WITH_VST3)
#include <ags/app/ags_vst3_browser.h>
#include <ags/app/ags_vst3_browser_callbacks.h>
#endif

#include <ags/app/ags_wave_editor.h>
#include <ags/app/ags_wave_editor_callbacks.h>
#include <ags/app/ags_wave_window.h>
#include <ags/app/ags_wave_window_callbacks.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_window_callbacks.h>
#include <ags/app/ags_gsequencer_application_context.h>

#include <ags/app/editor/ags_automation_edit.h>
#include <ags/app/editor/ags_automation_edit_box.h>
#include <ags/app/editor/ags_automation_edit_callbacks.h>
#include <ags/app/editor/ags_automation_meta.h>
#include <ags/app/editor/ags_automation_meta_callbacks.h>
#include <ags/app/editor/ags_automation_toolbar.h>
#include <ags/app/editor/ags_automation_toolbar_callbacks.h>
#include <ags/app/editor/ags_composite_edit.h>
#include <ags/app/editor/ags_composite_edit_callbacks.h>
#include <ags/app/editor/ags_composite_toolbar.h>
#include <ags/app/editor/ags_composite_toolbar_callbacks.h>
#include <ags/app/editor/ags_crop_note_dialog.h>
#include <ags/app/editor/ags_crop_note_dialog_callbacks.h>
#include <ags/app/editor/ags_envelope_dialog.h>
#include <ags/app/editor/ags_envelope_dialog_callbacks.h>
#include <ags/app/editor/ags_envelope_editor.h>
#include <ags/app/editor/ags_envelope_editor_callbacks.h>
#include <ags/app/editor/ags_envelope_info.h>
#include <ags/app/editor/ags_envelope_info_callbacks.h>
#include <ags/app/editor/ags_machine_radio_button.h>
#include <ags/app/editor/ags_machine_radio_button_callbacks.h>
#include <ags/app/editor/ags_machine_selection.h>
#include <ags/app/editor/ags_machine_selector.h>
#include <ags/app/editor/ags_machine_selector_callbacks.h>
#include <ags/app/editor/ags_move_note_dialog.h>
#include <ags/app/editor/ags_move_note_dialog_callbacks.h>
#include <ags/app/editor/ags_notation_edit.h>
#include <ags/app/editor/ags_notation_edit_callbacks.h>
#include <ags/app/editor/ags_notation_meta.h>
#include <ags/app/editor/ags_notation_meta_callbacks.h>
#include <ags/app/editor/ags_notation_toolbar.h>
#include <ags/app/editor/ags_notation_toolbar_callbacks.h>
#include <ags/app/editor/ags_pattern_envelope.h>
#include <ags/app/editor/ags_pattern_envelope_callbacks.h>
#include <ags/app/editor/ags_position_automation_cursor_dialog.h>
#include <ags/app/editor/ags_position_automation_cursor_dialog_callbacks.h>
#include <ags/app/editor/ags_position_notation_cursor_dialog.h>
#include <ags/app/editor/ags_position_notation_cursor_dialog_callbacks.h>
#include <ags/app/editor/ags_position_wave_cursor_dialog.h>
#include <ags/app/editor/ags_position_wave_cursor_dialog_callbacks.h>
#include <ags/app/editor/ags_ramp_acceleration_dialog.h>
#include <ags/app/editor/ags_ramp_acceleration_dialog_callbacks.h>
#include <ags/app/editor/ags_scrolled_automation_edit_box.h>
#include <ags/app/editor/ags_scrolled_wave_edit_box.h>
#include <ags/app/editor/ags_select_acceleration_dialog.h>
#include <ags/app/editor/ags_select_acceleration_dialog_callbacks.h>
#include <ags/app/editor/ags_select_buffer_dialog.h>
#include <ags/app/editor/ags_select_buffer_dialog_callbacks.h>
#include <ags/app/editor/ags_select_note_dialog.h>
#include <ags/app/editor/ags_select_note_dialog_callbacks.h>
#include <ags/app/editor/ags_sheet_edit.h>
#include <ags/app/editor/ags_sheet_edit_callbacks.h>
#include <ags/app/editor/ags_sheet_toolbar.h>
#include <ags/app/editor/ags_sheet_toolbar_callbacks.h>
#include <ags/app/editor/ags_time_stretch_buffer_dialog.h>
#include <ags/app/editor/ags_time_stretch_buffer_dialog_callbacks.h>
#include <ags/app/editor/ags_vautomation_edit_box.h>
#include <ags/app/editor/ags_vwave_edit_box.h>
#include <ags/app/editor/ags_wave_edit.h>
#include <ags/app/editor/ags_wave_edit_box.h>
#include <ags/app/editor/ags_wave_edit_callbacks.h>
#include <ags/app/editor/ags_wave_meta.h>
#include <ags/app/editor/ags_wave_meta_callbacks.h>
#include <ags/app/editor/ags_wave_toolbar.h>
#include <ags/app/editor/ags_wave_toolbar_callbacks.h>

#include <ags/app/export/ags_machine_collection.h>
#include <ags/app/export/ags_machine_collection_callbacks.h>
#include <ags/app/export/ags_machine_collection_entry.h>
#include <ags/app/export/ags_machine_collection_entry_callbacks.h>
#include <ags/app/export/ags_midi_export_wizard.h>
#include <ags/app/export/ags_midi_export_wizard_callbacks.h>
#include <ags/app/export/ags_wave_export_dialog.h>
#include <ags/app/export/ags_wave_export_dialog_callbacks.h>

#include <ags/app/file/ags_simple_file.h>

#include <ags/app/import/ags_midi_import_wizard.h>
#include <ags/app/import/ags_midi_import_wizard_callbacks.h>
#include <ags/app/import/ags_track_collection.h>
#include <ags/app/import/ags_track_collection_callbacks.h>
#include <ags/app/import/ags_track_collection_mapper.h>
#include <ags/app/import/ags_track_collection_mapper_callbacks.h>

#include <ags/app/machine/ags_audiorec.h>
#include <ags/app/machine/ags_audiorec_callbacks.h>
#include <ags/app/machine/ags_cell_pattern.h>
#include <ags/app/machine/ags_cell_pattern_callbacks.h>
#include <ags/app/machine/ags_desk.h>
#include <ags/app/machine/ags_desk_callbacks.h>
#include <ags/app/machine/ags_desk_file_chooser.h>
#include <ags/app/machine/ags_desk_pad.h>
#include <ags/app/machine/ags_desk_pad_callbacks.h>
#include <ags/app/machine/ags_drum.h>
#include <ags/app/machine/ags_drum_callbacks.h>
#include <ags/app/machine/ags_drum_input_line.h>
#include <ags/app/machine/ags_drum_input_line_callbacks.h>
#include <ags/app/machine/ags_drum_input_pad.h>
#include <ags/app/machine/ags_drum_input_pad_callbacks.h>
#include <ags/app/machine/ags_drum_output_line.h>
#include <ags/app/machine/ags_drum_output_line_callbacks.h>
#include <ags/app/machine/ags_drum_output_pad.h>
#include <ags/app/machine/ags_drum_output_pad_callbacks.h>
#include <ags/app/machine/ags_dssi_bridge.h>
#include <ags/app/machine/ags_dssi_bridge_callbacks.h>
#include <ags/app/machine/ags_equalizer10.h>
#include <ags/app/machine/ags_equalizer10_callbacks.h>

#if defined(AGS_WITH_LIBINSTPATCH)
#include <ags/app/machine/ags_ffplayer.h>
#include <ags/app/machine/ags_ffplayer_bridge.h>
#include <ags/app/machine/ags_ffplayer_bridge_callbacks.h>
#include <ags/app/machine/ags_ffplayer_bulk_input.h>
#include <ags/app/machine/ags_ffplayer_bulk_input_callbacks.h>
#include <ags/app/machine/ags_ffplayer_callbacks.h>
#include <ags/app/machine/ags_ffplayer_input_line.h>
#include <ags/app/machine/ags_ffplayer_input_line_callbacks.h>
#include <ags/app/machine/ags_ffplayer_input_pad.h>
#include <ags/app/machine/ags_ffplayer_input_pad_callbacks.h>
#endif

#include <ags/app/machine/ags_fm_oscillator.h>
#include <ags/app/machine/ags_fm_oscillator_callbacks.h>
#include <ags/app/machine/ags_fm_syncsynth.h>
#include <ags/app/machine/ags_fm_syncsynth_callbacks.h>
#include <ags/app/machine/ags_fm_synth.h>
#include <ags/app/machine/ags_fm_synth_callbacks.h>
#include <ags/app/machine/ags_fm_synth_input_line.h>
#include <ags/app/machine/ags_fm_synth_input_line_callbacks.h>
#include <ags/app/machine/ags_fm_synth_input_pad.h>
#include <ags/app/machine/ags_hybrid_fm_synth.h>
#include <ags/app/machine/ags_hybrid_fm_synth_callbacks.h>
#include <ags/app/machine/ags_hybrid_synth.h>
#include <ags/app/machine/ags_hybrid_synth_callbacks.h>
#include <ags/app/machine/ags_ladspa_bridge.h>
#include <ags/app/machine/ags_ladspa_bridge_callbacks.h>
#include <ags/app/machine/ags_live_dssi_bridge.h>
#include <ags/app/machine/ags_live_dssi_bridge_callbacks.h>
#include <ags/app/machine/ags_live_lv2_bridge.h>
#include <ags/app/machine/ags_live_lv2_bridge_callbacks.h>

#if defined(AGS_WITH_VST3)
#include <ags/app/machine/ags_live_vst3_bridge.h>
#include <ags/app/machine/ags_live_vst3_bridge_callbacks.h>
#endif

#include <ags/app/machine/ags_lv2_bridge.h>
#include <ags/app/machine/ags_lv2_bridge_callbacks.h>
#include <ags/app/machine/ags_matrix.h>
#include <ags/app/machine/ags_matrix_bridge.h>
#include <ags/app/machine/ags_matrix_bridge_callbacks.h>
#include <ags/app/machine/ags_matrix_bulk_input.h>
#include <ags/app/machine/ags_matrix_bulk_input_callbacks.h>
#include <ags/app/machine/ags_matrix_callbacks.h>
#include <ags/app/machine/ags_mixer.h>
#include <ags/app/machine/ags_mixer_callbacks.h>
#include <ags/app/machine/ags_mixer_input_line.h>
#include <ags/app/machine/ags_mixer_input_pad.h>
#include <ags/app/machine/ags_oscillator.h>
#include <ags/app/machine/ags_oscillator_callbacks.h>
#include <ags/app/machine/ags_panel.h>
#include <ags/app/machine/ags_panel_callbacks.h>
#include <ags/app/machine/ags_panel_input_line.h>
#include <ags/app/machine/ags_panel_input_line_callbacks.h>
#include <ags/app/machine/ags_panel_input_pad.h>
#include <ags/app/machine/ags_pattern_box.h>
#include <ags/app/machine/ags_pattern_box_callbacks.h>
#include <ags/app/machine/ags_pitch_sampler.h>
#include <ags/app/machine/ags_pitch_sampler_callbacks.h>
#include <ags/app/machine/ags_pitch_sampler_file.h>
#include <ags/app/machine/ags_pitch_sampler_file_callbacks.h>

#if defined(AGS_WITH_LIBINSTPATCH)
#include <ags/app/machine/ags_sf2_synth.h>
#include <ags/app/machine/ags_sf2_synth_callbacks.h>
#endif

#include <ags/app/machine/ags_sfz_synth.h>
#include <ags/app/machine/ags_sfz_synth_callbacks.h>
#include <ags/app/machine/ags_spectrometer.h>
#include <ags/app/machine/ags_spectrometer_callbacks.h>
#include <ags/app/machine/ags_syncsynth.h>
#include <ags/app/machine/ags_syncsynth_callbacks.h>
#include <ags/app/machine/ags_synth.h>
#include <ags/app/machine/ags_synth_callbacks.h>
#include <ags/app/machine/ags_synth_input_line.h>
#include <ags/app/machine/ags_synth_input_line_callbacks.h>
#include <ags/app/machine/ags_synth_input_pad.h>

#if defined(AGS_WITH_VST3)
#include <ags/app/machine/ags_vst3_bridge.h>
#include <ags/app/machine/ags_vst3_bridge_callbacks.h>
#endif

#include <ags/app/osc/controller/ags_ui_osc_renew_controller.h>

#include <ags/app/task/ags_simple_file_read.h>
#include <ags/app/task/ags_simple_file_write.h>

#endif /*__LIBGSEQUENCER_H__*/
