# Makefile for PO directory in any package using GNU gettext.
# Copyright (C) 1995-1997, 2000-2007, 2009-2010 by Ulrich Drepper <drepper@gnu.ai.mit.edu>
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.
#
# Origin: gettext-0.19.8
GETTEXT_MACRO_VERSION = 0.19

PACKAGE = gsequencer
VERSION = 3.18.2
PACKAGE_BUGREPORT = jkraehemann@gmail.com

SED = /usr/bin/sed
SHELL = /bin/sh


srcdir = .
top_srcdir = ..


prefix = /usr/local
exec_prefix = ${prefix}
datarootdir = ${prefix}/share
datadir = ${datarootdir}
localedir = ${datarootdir}/locale
gettextsrcdir = $(datadir)/gettext/po

INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644

# We use $(mkdir_p).
# In automake <= 1.9.x, $(mkdir_p) is defined either as "mkdir -p --" or as
# "$(mkinstalldirs)" or as "$(install_sh) -d". For these automake versions,
# ${SHELL} /home/joelkraehemann/savannah/gsequencer/install-sh does not start with $(SHELL), so we add it.
# In automake >= 1.10, $(MKDIR_P) is derived from ${MKDIR_P}, which is defined
# either as "/path/to/mkdir -p" or ".../install-sh -c -d". For these automake
# versions, $(mkinstalldirs) and $(install_sh) are unused.
mkinstalldirs = $(SHELL) ${SHELL} /home/joelkraehemann/savannah/gsequencer/install-sh -d
install_sh = $(SHELL) ${SHELL} /home/joelkraehemann/savannah/gsequencer/install-sh
MKDIR_P = /usr/bin/mkdir -p
mkdir_p = $(MKDIR_P)

# When building gettext-tools, we prefer to use the built programs
# rather than installed programs.  However, we can't do that when we
# are cross compiling.
CROSS_COMPILING = @CROSS_COMPILING@

GMSGFMT_ = /usr/bin/msgfmt
GMSGFMT_no = /usr/bin/msgfmt
GMSGFMT_yes = /usr/bin/msgfmt
GMSGFMT = $(GMSGFMT_$(USE_MSGCTXT))
MSGFMT_ = /usr/bin/msgfmt
MSGFMT_no = /usr/bin/msgfmt
MSGFMT_yes = /usr/bin/msgfmt
MSGFMT = $(MSGFMT_$(USE_MSGCTXT))
XGETTEXT_ = /usr/bin/xgettext
XGETTEXT_no = /usr/bin/xgettext
XGETTEXT_yes = /usr/bin/xgettext
XGETTEXT = $(XGETTEXT_$(USE_MSGCTXT))
MSGMERGE = msgmerge
MSGMERGE_UPDATE = /usr/bin/msgmerge --update
MSGINIT = msginit
MSGCONV = msgconv
MSGFILTER = msgfilter

POFILES =  de.po fr.po it.po pl.po
GMOFILES =  de.gmo fr.gmo it.gmo pl.gmo
UPDATEPOFILES =  de.po-update fr.po-update it.po-update pl.po-update
DUMMYPOFILES =  de.nop fr.nop it.nop pl.nop
DISTFILES.common = Makefile.in.in remove-potcdate.sin \
$(DISTFILES.common.extra1) $(DISTFILES.common.extra2) $(DISTFILES.common.extra3)
DISTFILES = $(DISTFILES.common) Makevars POTFILES.in \
$(POFILES) $(GMOFILES) \
$(DISTFILES.extra1) $(DISTFILES.extra2) $(DISTFILES.extra3)

POTFILES = \
     ../ags/app/ags_animation_window.c \
     ../ags/app/ags_audio_preferences.c \
     ../ags/app/ags_audio_preferences_callbacks.c \
     ../ags/app/ags_automation_editor.c \
     ../ags/app/ags_automation_editor_callbacks.c \
     ../ags/app/ags_automation_window.c \
     ../ags/app/ags_automation_window_callbacks.c \
     ../ags/app/ags_bulk_member.c \
     ../ags/app/ags_bulk_member_callbacks.c \
     ../ags/app/ags_connection_editor.c \
     ../ags/app/ags_connection_editor_callbacks.c \
     ../ags/app/ags_context_menu.c \
     ../ags/app/ags_dssi_browser.c \
     ../ags/app/ags_dssi_browser_callbacks.c \
     ../ags/app/ags_effect_bridge.c \
     ../ags/app/ags_effect_bridge_callbacks.c \
     ../ags/app/ags_effect_bulk.c \
     ../ags/app/ags_effect_bulk_callbacks.c \
     ../ags/app/ags_effect_line.c \
     ../ags/app/ags_effect_line_callbacks.c \
     ../ags/app/ags_effect_pad.c \
     ../ags/app/ags_effect_pad_callbacks.c \
     ../ags/app/ags_effect_separator.c \
     ../ags/app/ags_export_soundcard.c \
     ../ags/app/ags_export_soundcard_callbacks.c \
     ../ags/app/ags_export_window.c \
     ../ags/app/ags_export_window_callbacks.c \
     ../ags/app/ags_generic_preferences.c \
     ../ags/app/ags_generic_preferences_callbacks.c \
     ../ags/app/ags_input_collection_editor.c \
     ../ags/app/ags_input_collection_editor_callbacks.c \
     ../ags/app/ags_input_editor.c \
     ../ags/app/ags_input_editor_callbacks.c \
     ../ags/app/ags_input_listing_editor.c \
     ../ags/app/ags_input_listing_editor_callbacks.c \
     ../ags/app/ags_ladspa_browser.c \
     ../ags/app/ags_ladspa_browser_callbacks.c \
     ../ags/app/ags_line.c \
     ../ags/app/ags_line_callbacks.c \
     ../ags/app/ags_line_editor.c \
     ../ags/app/ags_line_editor_callbacks.c \
     ../ags/app/ags_line_member.c \
     ../ags/app/ags_line_member_callbacks.c \
     ../ags/app/ags_line_member_editor.c \
     ../ags/app/ags_line_member_editor_callbacks.c \
     ../ags/app/ags_link_collection_editor.c \
     ../ags/app/ags_link_collection_editor_callbacks.c \
     ../ags/app/ags_link_editor.c \
     ../ags/app/ags_link_editor_callbacks.c \
     ../ags/app/ags_listing_editor.c \
     ../ags/app/ags_listing_editor_callbacks.c \
     ../ags/app/ags_lv2_browser.c \
     ../ags/app/ags_lv2_browser_callbacks.c \
     ../ags/app/ags_machine.c \
     ../ags/app/ags_machine_callbacks.c \
     ../ags/app/ags_machine_editor.c \
     ../ags/app/ags_machine_editor_callbacks.c \
     ../ags/app/ags_machine_util.c \
     ../ags/app/ags_menu_action_callbacks.c \
     ../ags/app/ags_menu_bar.c \
     ../ags/app/ags_midi_dialog.c \
     ../ags/app/ags_midi_dialog_callbacks.c \
     ../ags/app/ags_midi_preferences.c \
     ../ags/app/ags_midi_preferences_callbacks.c \
     ../ags/app/ags_navigation.c \
     ../ags/app/ags_navigation_callbacks.c \
     ../ags/app/ags_notation_editor.c \
     ../ags/app/ags_notation_editor_callbacks.c \
     ../ags/app/ags_online_help_window.c \
     ../ags/app/ags_online_help_window_callbacks.c \
     ../ags/app/ags_osc_server_preferences.c \
     ../ags/app/ags_osc_server_preferences_callbacks.c \
     ../ags/app/ags_output_collection_editor.c \
     ../ags/app/ags_output_collection_editor_callbacks.c \
     ../ags/app/ags_output_editor.c \
     ../ags/app/ags_output_editor_callbacks.c \
     ../ags/app/ags_output_listing_editor.c \
     ../ags/app/ags_output_listing_editor_callbacks.c \
     ../ags/app/ags_pad.c \
     ../ags/app/ags_pad_callbacks.c \
     ../ags/app/ags_pad_editor.c \
     ../ags/app/ags_pad_editor_callbacks.c \
     ../ags/app/ags_performance_preferences.c \
     ../ags/app/ags_performance_preferences_callbacks.c \
     ../ags/app/ags_playback_window.c \
     ../ags/app/ags_playback_window_callbacks.c \
     ../ags/app/ags_plugin_browser.c \
     ../ags/app/ags_plugin_browser_callbacks.c \
     ../ags/app/ags_plugin_preferences.c \
     ../ags/app/ags_plugin_preferences_callbacks.c \
     ../ags/app/ags_preferences.c \
     ../ags/app/ags_preferences_callbacks.c \
     ../ags/app/ags_property_collection_editor.c \
     ../ags/app/ags_property_collection_editor_callbacks.c \
     ../ags/app/ags_property_editor.c \
     ../ags/app/ags_property_editor_callbacks.c \
     ../ags/app/ags_property_listing_editor.c \
     ../ags/app/ags_quit_dialog.c \
     ../ags/app/ags_quit_dialog_callbacks.c \
     ../ags/app/ags_resize_editor.c \
     ../ags/app/ags_sequencer_editor.c \
     ../ags/app/ags_sequencer_editor_callbacks.c \
     ../ags/app/ags_server_preferences.c \
     ../ags/app/ags_server_preferences_callbacks.c \
     ../ags/app/ags_sheet_editor.c \
     ../ags/app/ags_sheet_editor_callbacks.c \
     ../ags/app/ags_sheet_window.c \
     ../ags/app/ags_sheet_window_callbacks.c \
     ../ags/app/ags_soundcard_editor.c \
     ../ags/app/ags_soundcard_editor_callbacks.c \
     ../ags/app/ags_ui_provider.c \
     ../ags/app/ags_wave_editor.c \
     ../ags/app/ags_wave_editor_callbacks.c \
     ../ags/app/ags_wave_window.c \
     ../ags/app/ags_wave_window_callbacks.c \
     ../ags/app/ags_window.c \
     ../ags/app/ags_window_callbacks.c \
     ../ags/app/ags_gsequencer_application_context.c \
     ../ags/app/editor/ags_automation_edit.c \
     ../ags/app/editor/ags_automation_edit_box.c \
     ../ags/app/editor/ags_automation_edit_callbacks.c \
     ../ags/app/editor/ags_automation_meta.c \
     ../ags/app/editor/ags_automation_meta_callbacks.c \
     ../ags/app/editor/ags_automation_toolbar.c \
     ../ags/app/editor/ags_automation_toolbar_callbacks.c \
     ../ags/app/editor/ags_crop_note_dialog.c \
     ../ags/app/editor/ags_crop_note_dialog_callbacks.c \
     ../ags/app/editor/ags_envelope_dialog.c \
     ../ags/app/editor/ags_envelope_dialog_callbacks.c \
     ../ags/app/editor/ags_envelope_editor.c \
     ../ags/app/editor/ags_envelope_editor_callbacks.c \
     ../ags/app/editor/ags_envelope_info.c \
     ../ags/app/editor/ags_envelope_info_callbacks.c \
     ../ags/app/editor/ags_machine_radio_button.c \
     ../ags/app/editor/ags_machine_radio_button_callbacks.c \
     ../ags/app/editor/ags_machine_selection.c \
     ../ags/app/editor/ags_machine_selector.c \
     ../ags/app/editor/ags_machine_selector_callbacks.c \
     ../ags/app/editor/ags_move_note_dialog.c \
     ../ags/app/editor/ags_move_note_dialog_callbacks.c \
     ../ags/app/editor/ags_notation_edit.c \
     ../ags/app/editor/ags_notation_edit_callbacks.c \
     ../ags/app/editor/ags_notation_meta.c \
     ../ags/app/editor/ags_notation_meta_callbacks.c \
     ../ags/app/editor/ags_notation_toolbar.c \
     ../ags/app/editor/ags_notation_toolbar_callbacks.c \
     ../ags/app/editor/ags_pattern_envelope.c \
     ../ags/app/editor/ags_pattern_envelope_callbacks.c \
     ../ags/app/editor/ags_position_automation_cursor_dialog.c \
     ../ags/app/editor/ags_position_automation_cursor_dialog_callbacks.c \
     ../ags/app/editor/ags_position_notation_cursor_dialog.c \
     ../ags/app/editor/ags_position_notation_cursor_dialog_callbacks.c \
     ../ags/app/editor/ags_position_wave_cursor_dialog.c \
     ../ags/app/editor/ags_position_wave_cursor_dialog_callbacks.c \
     ../ags/app/editor/ags_ramp_acceleration_dialog.c \
     ../ags/app/editor/ags_ramp_acceleration_dialog_callbacks.c \
     ../ags/app/editor/ags_scrolled_automation_edit_box.c \
     ../ags/app/editor/ags_scrolled_wave_edit_box.c \
     ../ags/app/editor/ags_select_acceleration_dialog.c \
     ../ags/app/editor/ags_select_acceleration_dialog_callbacks.c \
     ../ags/app/editor/ags_select_buffer_dialog.c \
     ../ags/app/editor/ags_select_buffer_dialog_callbacks.c \
     ../ags/app/editor/ags_select_note_dialog.c \
     ../ags/app/editor/ags_select_note_dialog_callbacks.c \
     ../ags/app/editor/ags_sheet_edit.c \
     ../ags/app/editor/ags_sheet_edit_callbacks.c \
     ../ags/app/editor/ags_sheet_toolbar.c \
     ../ags/app/editor/ags_sheet_toolbar_callbacks.c \
     ../ags/app/editor/ags_vautomation_edit_box.c \
     ../ags/app/editor/ags_vwave_edit_box.c \
     ../ags/app/editor/ags_wave_edit.c \
     ../ags/app/editor/ags_wave_edit_box.c \
     ../ags/app/editor/ags_wave_edit_callbacks.c \
     ../ags/app/editor/ags_wave_meta.c \
     ../ags/app/editor/ags_wave_meta_callbacks.c \
     ../ags/app/editor/ags_wave_toolbar.c \
     ../ags/app/editor/ags_wave_toolbar_callbacks.c \
     ../ags/app/export/ags_machine_collection.c \
     ../ags/app/export/ags_machine_collection_callbacks.c \
     ../ags/app/export/ags_machine_collection_entry.c \
     ../ags/app/export/ags_machine_collection_entry_callbacks.c \
     ../ags/app/export/ags_midi_export_wizard.c \
     ../ags/app/export/ags_midi_export_wizard_callbacks.c \
     ../ags/app/export/ags_wave_export_dialog.c \
     ../ags/app/export/ags_wave_export_dialog_callbacks.c \
     ../ags/app/file/ags_simple_file.c \
     ../ags/app/import/ags_midi_import_wizard.c \
     ../ags/app/import/ags_midi_import_wizard_callbacks.c \
     ../ags/app/import/ags_track_collection.c \
     ../ags/app/import/ags_track_collection_callbacks.c \
     ../ags/app/import/ags_track_collection_mapper.c \
     ../ags/app/import/ags_track_collection_mapper_callbacks.c \
     ../ags/app/machine/ags_audiorec.c \
     ../ags/app/machine/ags_audiorec_callbacks.c \
     ../ags/app/machine/ags_cell_pattern.c \
     ../ags/app/machine/ags_cell_pattern_callbacks.c \
     ../ags/app/machine/ags_desk.c \
     ../ags/app/machine/ags_desk_callbacks.c \
     ../ags/app/machine/ags_desk_file_chooser.c \
     ../ags/app/machine/ags_desk_pad.c \
     ../ags/app/machine/ags_desk_pad_callbacks.c \
     ../ags/app/machine/ags_drum.c \
     ../ags/app/machine/ags_drum_callbacks.c \
     ../ags/app/machine/ags_drum_input_line.c \
     ../ags/app/machine/ags_drum_input_line_callbacks.c \
     ../ags/app/machine/ags_drum_input_pad.c \
     ../ags/app/machine/ags_drum_input_pad_callbacks.c \
     ../ags/app/machine/ags_drum_output_line.c \
     ../ags/app/machine/ags_drum_output_line_callbacks.c \
     ../ags/app/machine/ags_drum_output_pad.c \
     ../ags/app/machine/ags_drum_output_pad_callbacks.c \
     ../ags/app/machine/ags_dssi_bridge.c \
     ../ags/app/machine/ags_dssi_bridge_callbacks.c \
     ../ags/app/machine/ags_equalizer10.c \
     ../ags/app/machine/ags_equalizer10_callbacks.c \
     ../ags/app/machine/ags_ffplayer.c \
     ../ags/app/machine/ags_ffplayer_bridge.c \
     ../ags/app/machine/ags_ffplayer_bridge_callbacks.c \
     ../ags/app/machine/ags_ffplayer_bulk_input.c \
     ../ags/app/machine/ags_ffplayer_bulk_input_callbacks.c \
     ../ags/app/machine/ags_ffplayer_callbacks.c \
     ../ags/app/machine/ags_ffplayer_input_line.c \
     ../ags/app/machine/ags_ffplayer_input_line_callbacks.c \
     ../ags/app/machine/ags_ffplayer_input_pad.c \
     ../ags/app/machine/ags_ffplayer_input_pad_callbacks.c \
     ../ags/app/machine/ags_fm_oscillator.c \
     ../ags/app/machine/ags_fm_oscillator_callbacks.c \
     ../ags/app/machine/ags_fm_syncsynth.c \
     ../ags/app/machine/ags_fm_syncsynth_callbacks.c \
     ../ags/app/machine/ags_fm_synth.c \
     ../ags/app/machine/ags_fm_synth_callbacks.c \
     ../ags/app/machine/ags_fm_synth_input_line.c \
     ../ags/app/machine/ags_fm_synth_input_line_callbacks.c \
     ../ags/app/machine/ags_fm_synth_input_pad.c \
     ../ags/app/machine/ags_ladspa_bridge.c \
     ../ags/app/machine/ags_ladspa_bridge_callbacks.c \
     ../ags/app/machine/ags_live_dssi_bridge.c \
     ../ags/app/machine/ags_live_dssi_bridge_callbacks.c \
     ../ags/app/machine/ags_live_lv2_bridge.c \
     ../ags/app/machine/ags_live_lv2_bridge_callbacks.c \
     ../ags/app/machine/ags_lv2_bridge.c \
     ../ags/app/machine/ags_lv2_bridge_callbacks.c \
     ../ags/app/machine/ags_matrix.c \
     ../ags/app/machine/ags_matrix_bridge.c \
     ../ags/app/machine/ags_matrix_bridge_callbacks.c \
     ../ags/app/machine/ags_matrix_bulk_input.c \
     ../ags/app/machine/ags_matrix_bulk_input_callbacks.c \
     ../ags/app/machine/ags_matrix_callbacks.c \
     ../ags/app/machine/ags_mixer.c \
     ../ags/app/machine/ags_mixer_callbacks.c \
     ../ags/app/machine/ags_mixer_input_line.c \
     ../ags/app/machine/ags_mixer_input_pad.c \
     ../ags/app/machine/ags_oscillator.c \
     ../ags/app/machine/ags_oscillator_callbacks.c \
     ../ags/app/machine/ags_panel.c \
     ../ags/app/machine/ags_panel_callbacks.c \
     ../ags/app/machine/ags_panel_input_line.c \
     ../ags/app/machine/ags_panel_input_line_callbacks.c \
     ../ags/app/machine/ags_panel_input_pad.c \
     ../ags/app/machine/ags_pattern_box.c \
     ../ags/app/machine/ags_pattern_box_callbacks.c \
     ../ags/app/machine/ags_pitch_sampler.c \
     ../ags/app/machine/ags_pitch_sampler_callbacks.c \
     ../ags/app/machine/ags_pitch_sampler_file.c \
     ../ags/app/machine/ags_pitch_sampler_file_callbacks.c \
     ../ags/app/machine/ags_sf2_synth.c \
     ../ags/app/machine/ags_sf2_synth_callbacks.c \
     ../ags/app/machine/ags_sfz_synth.c \
     ../ags/app/machine/ags_sfz_synth_callbacks.c \
     ../ags/app/machine/ags_spectrometer.c \
     ../ags/app/machine/ags_spectrometer_callbacks.c \
     ../ags/app/machine/ags_syncsynth.c \
     ../ags/app/machine/ags_syncsynth_callbacks.c \
     ../ags/app/machine/ags_synth.c \
     ../ags/app/machine/ags_synth_callbacks.c \
     ../ags/app/machine/ags_synth_input_line.c \
     ../ags/app/machine/ags_synth_input_line_callbacks.c \
     ../ags/app/machine/ags_synth_input_pad.c \
     ../ags/app/osc/controller/ags_ui_osc_renew_controller.c \
     ../ags/app/task/ags_simple_file_read.c \
     ../ags/app/task/ags_simple_file_write.c \
     ../ags/audio/ags_acceleration.c \
     ../ags/audio/ags_audio.c \
     ../ags/audio/ags_audio_application_context.c \
     ../ags/audio/ags_audio_buffer_util.c \
     ../ags/audio/ags_audio_signal.c \
     ../ags/audio/ags_automation.c \
     ../ags/audio/ags_buffer.c \
     ../ags/audio/ags_channel.c \
     ../ags/audio/ags_char_buffer_util.c \
     ../ags/audio/ags_devin.c \
     ../ags/audio/ags_devout.c \
     ../ags/audio/ags_diatonic_scale.c \
     ../ags/audio/ags_fast_pitch_util.c \
     ../ags/audio/ags_fifoout.c \
     ../ags/audio/ags_filter_util.c \
     ../ags/audio/ags_fm_synth_util.c \
     ../ags/audio/ags_fourier_transform_util.c \
     ../ags/audio/ags_frequency_map.c \
     ../ags/audio/ags_frequency_map_manager.c \
     ../ags/audio/ags_fx_factory.c \
     ../ags/audio/ags_generic_recall_channel_run.c \
     ../ags/audio/ags_generic_recall_recycling.c \
     ../ags/audio/ags_input.c \
     ../ags/audio/ags_lfo_synth_util.c \
     ../ags/audio/ags_midi.c \
     ../ags/audio/ags_midiin.c \
     ../ags/audio/ags_notation.c \
     ../ags/audio/ags_note.c \
     ../ags/audio/ags_output.c \
     ../ags/audio/ags_pattern.c \
     ../ags/audio/ags_playback.c \
     ../ags/audio/ags_playback_domain.c \
     ../ags/audio/ags_port.c \
     ../ags/audio/ags_port_util.c \
     ../ags/audio/ags_preset.c \
     ../ags/audio/ags_recall.c \
     ../ags/audio/ags_recall_audio.c \
     ../ags/audio/ags_recall_audio_run.c \
     ../ags/audio/ags_recall_audio_signal.c \
     ../ags/audio/ags_recall_channel.c \
     ../ags/audio/ags_recall_channel_run.c \
     ../ags/audio/ags_recall_container.c \
     ../ags/audio/ags_recall_dependency.c \
     ../ags/audio/ags_recall_dssi.c \
     ../ags/audio/ags_recall_dssi_run.c \
     ../ags/audio/ags_recall_factory.c \
     ../ags/audio/ags_recall_id.c \
     ../ags/audio/ags_recall_ladspa.c \
     ../ags/audio/ags_recall_ladspa_run.c \
     ../ags/audio/ags_recall_lv2.c \
     ../ags/audio/ags_recall_lv2_run.c \
     ../ags/audio/ags_recall_recycling.c \
     ../ags/audio/ags_recycling.c \
     ../ags/audio/ags_recycling_context.c \
     ../ags/audio/ags_sequencer_util.c \
     ../ags/audio/ags_sf2_synth_generator.c \
     ../ags/audio/ags_sf2_synth_util.c \
     ../ags/audio/ags_sfz_synth_generator.c \
     ../ags/audio/ags_sfz_synth_util.c \
     ../ags/audio/ags_sound_enums.c \
     ../ags/audio/ags_sound_provider.c \
     ../ags/audio/ags_soundcard_util.c \
     ../ags/audio/ags_synth_enums.c \
     ../ags/audio/ags_synth_generator.c \
     ../ags/audio/ags_synth_util.c \
     ../ags/audio/ags_track.c \
     ../ags/audio/ags_wave.c \
     ../ags/audio/core-audio/ags_core_audio_client.c \
     ../ags/audio/core-audio/ags_core_audio_devin.c \
     ../ags/audio/core-audio/ags_core_audio_devout.c \
     ../ags/audio/core-audio/ags_core_audio_midiin.c \
     ../ags/audio/core-audio/ags_core_audio_port.c \
     ../ags/audio/core-audio/ags_core_audio_server.c \
     ../ags/audio/file/ags_audio_container.c \
     ../ags/audio/file/ags_audio_container_manager.c \
     ../ags/audio/file/ags_audio_file.c \
     ../ags/audio/file/ags_audio_file_link.c \
     ../ags/audio/file/ags_audio_file_manager.c \
     ../ags/audio/file/ags_gstreamer_file.c \
     ../ags/audio/file/ags_ipatch.c \
     ../ags/audio/file/ags_ipatch_dls2_reader.c \
     ../ags/audio/file/ags_ipatch_gig_reader.c \
     ../ags/audio/file/ags_ipatch_sample.c \
     ../ags/audio/file/ags_ipatch_sf2_reader.c \
     ../ags/audio/file/ags_sfz_file.c \
     ../ags/audio/file/ags_sfz_group.c \
     ../ags/audio/file/ags_sfz_region.c \
     ../ags/audio/file/ags_sfz_sample.c \
     ../ags/audio/file/ags_sndfile.c \
     ../ags/audio/file/ags_sound_container.c \
     ../ags/audio/file/ags_sound_resource.c \
     ../ags/audio/fx/ags_fx_analyse_audio.c \
     ../ags/audio/fx/ags_fx_analyse_audio_processor.c \
     ../ags/audio/fx/ags_fx_analyse_audio_signal.c \
     ../ags/audio/fx/ags_fx_analyse_channel.c \
     ../ags/audio/fx/ags_fx_analyse_channel_processor.c \
     ../ags/audio/fx/ags_fx_analyse_recycling.c \
     ../ags/audio/fx/ags_fx_buffer_audio.c \
     ../ags/audio/fx/ags_fx_buffer_audio_processor.c \
     ../ags/audio/fx/ags_fx_buffer_audio_signal.c \
     ../ags/audio/fx/ags_fx_buffer_channel.c \
     ../ags/audio/fx/ags_fx_buffer_channel_processor.c \
     ../ags/audio/fx/ags_fx_buffer_recycling.c \
     ../ags/audio/fx/ags_fx_dssi_audio.c \
     ../ags/audio/fx/ags_fx_dssi_audio_processor.c \
     ../ags/audio/fx/ags_fx_dssi_audio_signal.c \
     ../ags/audio/fx/ags_fx_dssi_channel.c \
     ../ags/audio/fx/ags_fx_dssi_channel_processor.c \
     ../ags/audio/fx/ags_fx_dssi_recycling.c \
     ../ags/audio/fx/ags_fx_envelope_audio.c \
     ../ags/audio/fx/ags_fx_envelope_audio_processor.c \
     ../ags/audio/fx/ags_fx_envelope_audio_signal.c \
     ../ags/audio/fx/ags_fx_envelope_channel.c \
     ../ags/audio/fx/ags_fx_envelope_channel_processor.c \
     ../ags/audio/fx/ags_fx_envelope_recycling.c \
     ../ags/audio/fx/ags_fx_eq10_audio.c \
     ../ags/audio/fx/ags_fx_eq10_audio_processor.c \
     ../ags/audio/fx/ags_fx_eq10_audio_signal.c \
     ../ags/audio/fx/ags_fx_eq10_channel.c \
     ../ags/audio/fx/ags_fx_eq10_channel_processor.c \
     ../ags/audio/fx/ags_fx_eq10_recycling.c \
     ../ags/audio/fx/ags_fx_ladspa_audio.c \
     ../ags/audio/fx/ags_fx_ladspa_audio_processor.c \
     ../ags/audio/fx/ags_fx_ladspa_audio_signal.c \
     ../ags/audio/fx/ags_fx_ladspa_channel.c \
     ../ags/audio/fx/ags_fx_ladspa_channel_processor.c \
     ../ags/audio/fx/ags_fx_ladspa_recycling.c \
     ../ags/audio/fx/ags_fx_lv2_audio.c \
     ../ags/audio/fx/ags_fx_lv2_audio_processor.c \
     ../ags/audio/fx/ags_fx_lv2_audio_signal.c \
     ../ags/audio/fx/ags_fx_lv2_channel.c \
     ../ags/audio/fx/ags_fx_lv2_channel_processor.c \
     ../ags/audio/fx/ags_fx_lv2_recycling.c \
     ../ags/audio/fx/ags_fx_notation_audio.c \
     ../ags/audio/fx/ags_fx_notation_audio_processor.c \
     ../ags/audio/fx/ags_fx_notation_audio_signal.c \
     ../ags/audio/fx/ags_fx_notation_channel.c \
     ../ags/audio/fx/ags_fx_notation_channel_processor.c \
     ../ags/audio/fx/ags_fx_notation_recycling.c \
     ../ags/audio/fx/ags_fx_pattern_audio.c \
     ../ags/audio/fx/ags_fx_pattern_audio_processor.c \
     ../ags/audio/fx/ags_fx_pattern_audio_signal.c \
     ../ags/audio/fx/ags_fx_pattern_channel.c \
     ../ags/audio/fx/ags_fx_pattern_channel_processor.c \
     ../ags/audio/fx/ags_fx_pattern_recycling.c \
     ../ags/audio/fx/ags_fx_peak_audio.c \
     ../ags/audio/fx/ags_fx_peak_audio_processor.c \
     ../ags/audio/fx/ags_fx_peak_audio_signal.c \
     ../ags/audio/fx/ags_fx_peak_channel.c \
     ../ags/audio/fx/ags_fx_peak_channel_processor.c \
     ../ags/audio/fx/ags_fx_peak_recycling.c \
     ../ags/audio/fx/ags_fx_playback_audio.c \
     ../ags/audio/fx/ags_fx_playback_audio_processor.c \
     ../ags/audio/fx/ags_fx_playback_audio_signal.c \
     ../ags/audio/fx/ags_fx_playback_channel.c \
     ../ags/audio/fx/ags_fx_playback_channel_processor.c \
     ../ags/audio/fx/ags_fx_playback_recycling.c \
     ../ags/audio/fx/ags_fx_volume_audio.c \
     ../ags/audio/fx/ags_fx_volume_audio_processor.c \
     ../ags/audio/fx/ags_fx_volume_audio_signal.c \
     ../ags/audio/fx/ags_fx_volume_channel.c \
     ../ags/audio/fx/ags_fx_volume_channel_processor.c \
     ../ags/audio/fx/ags_fx_volume_recycling.c \
     ../ags/audio/gstreamer/ags_gstreamer_client.c \
     ../ags/audio/gstreamer/ags_gstreamer_devin.c \
     ../ags/audio/gstreamer/ags_gstreamer_devout.c \
     ../ags/audio/gstreamer/ags_gstreamer_port.c \
     ../ags/audio/gstreamer/ags_gstreamer_server.c \
     ../ags/audio/jack/ags_jack_client.c \
     ../ags/audio/jack/ags_jack_devin.c \
     ../ags/audio/jack/ags_jack_devout.c \
     ../ags/audio/jack/ags_jack_midiin.c \
     ../ags/audio/jack/ags_jack_port.c \
     ../ags/audio/jack/ags_jack_server.c \
     ../ags/audio/midi/ags_midi_buffer_util.c \
     ../ags/audio/midi/ags_midi_builder.c \
     ../ags/audio/midi/ags_midi_file.c \
     ../ags/audio/midi/ags_midi_parser.c \
     ../ags/audio/midi/ags_midi_util.c \
     ../ags/audio/osc/ags_osc_buffer_util.c \
     ../ags/audio/osc/ags_osc_builder.c \
     ../ags/audio/osc/ags_osc_client.c \
     ../ags/audio/osc/ags_osc_connection.c \
     ../ags/audio/osc/ags_osc_message.c \
     ../ags/audio/osc/ags_osc_parser.c \
     ../ags/audio/osc/ags_osc_response.c \
     ../ags/audio/osc/ags_osc_server.c \
     ../ags/audio/osc/ags_osc_util.c \
     ../ags/audio/osc/ags_osc_websocket_connection.c \
     ../ags/audio/osc/ags_osc_xmlrpc_message.c \
     ../ags/audio/osc/ags_osc_xmlrpc_server.c \
     ../ags/audio/osc/controller/ags_osc_action_controller.c \
     ../ags/audio/osc/controller/ags_osc_config_controller.c \
     ../ags/audio/osc/controller/ags_osc_controller.c \
     ../ags/audio/osc/controller/ags_osc_export_controller.c \
     ../ags/audio/osc/controller/ags_osc_front_controller.c \
     ../ags/audio/osc/controller/ags_osc_info_controller.c \
     ../ags/audio/osc/controller/ags_osc_meter_controller.c \
     ../ags/audio/osc/controller/ags_osc_node_controller.c \
     ../ags/audio/osc/controller/ags_osc_plugin_controller.c \
     ../ags/audio/osc/controller/ags_osc_renew_controller.c \
     ../ags/audio/osc/controller/ags_osc_status_controller.c \
     ../ags/audio/osc/xmlrpc/ags_osc_xmlrpc_controller.c \
     ../ags/audio/pulse/ags_pulse_client.c \
     ../ags/audio/pulse/ags_pulse_devin.c \
     ../ags/audio/pulse/ags_pulse_devout.c \
     ../ags/audio/pulse/ags_pulse_port.c \
     ../ags/audio/pulse/ags_pulse_server.c \
     ../ags/audio/recall/ags_analyse_audio_signal.c \
     ../ags/audio/recall/ags_analyse_channel.c \
     ../ags/audio/recall/ags_analyse_channel_run.c \
     ../ags/audio/recall/ags_analyse_recycling.c \
     ../ags/audio/recall/ags_buffer_audio_signal.c \
     ../ags/audio/recall/ags_buffer_channel.c \
     ../ags/audio/recall/ags_buffer_channel_run.c \
     ../ags/audio/recall/ags_buffer_recycling.c \
     ../ags/audio/recall/ags_capture_wave_audio.c \
     ../ags/audio/recall/ags_capture_wave_audio_run.c \
     ../ags/audio/recall/ags_capture_wave_channel.c \
     ../ags/audio/recall/ags_capture_wave_channel_run.c \
     ../ags/audio/recall/ags_copy_audio_signal.c \
     ../ags/audio/recall/ags_copy_channel.c \
     ../ags/audio/recall/ags_copy_channel_run.c \
     ../ags/audio/recall/ags_copy_pattern_audio.c \
     ../ags/audio/recall/ags_copy_pattern_audio_run.c \
     ../ags/audio/recall/ags_copy_pattern_channel.c \
     ../ags/audio/recall/ags_copy_pattern_channel_run.c \
     ../ags/audio/recall/ags_copy_recycling.c \
     ../ags/audio/recall/ags_count_beats_audio.c \
     ../ags/audio/recall/ags_count_beats_audio_run.c \
     ../ags/audio/recall/ags_delay_audio.c \
     ../ags/audio/recall/ags_delay_audio_run.c \
     ../ags/audio/recall/ags_envelope_audio_signal.c \
     ../ags/audio/recall/ags_envelope_channel.c \
     ../ags/audio/recall/ags_envelope_channel_run.c \
     ../ags/audio/recall/ags_envelope_recycling.c \
     ../ags/audio/recall/ags_eq10_audio_signal.c \
     ../ags/audio/recall/ags_eq10_channel.c \
     ../ags/audio/recall/ags_eq10_channel_run.c \
     ../ags/audio/recall/ags_eq10_recycling.c \
     ../ags/audio/recall/ags_feed_audio_signal.c \
     ../ags/audio/recall/ags_feed_channel.c \
     ../ags/audio/recall/ags_feed_channel_run.c \
     ../ags/audio/recall/ags_feed_recycling.c \
     ../ags/audio/recall/ags_lfo_audio_signal.c \
     ../ags/audio/recall/ags_lfo_channel.c \
     ../ags/audio/recall/ags_lfo_channel_run.c \
     ../ags/audio/recall/ags_lfo_recycling.c \
     ../ags/audio/recall/ags_loop_channel.c \
     ../ags/audio/recall/ags_loop_channel_run.c \
     ../ags/audio/recall/ags_mute_audio.c \
     ../ags/audio/recall/ags_mute_audio_run.c \
     ../ags/audio/recall/ags_mute_audio_signal.c \
     ../ags/audio/recall/ags_mute_channel.c \
     ../ags/audio/recall/ags_mute_channel_run.c \
     ../ags/audio/recall/ags_mute_recycling.c \
     ../ags/audio/recall/ags_peak_audio_signal.c \
     ../ags/audio/recall/ags_peak_channel.c \
     ../ags/audio/recall/ags_peak_channel_run.c \
     ../ags/audio/recall/ags_peak_recycling.c \
     ../ags/audio/recall/ags_play_audio.c \
     ../ags/audio/recall/ags_play_audio_signal.c \
     ../ags/audio/recall/ags_play_channel.c \
     ../ags/audio/recall/ags_play_channel_run.c \
     ../ags/audio/recall/ags_play_channel_run_master.c \
     ../ags/audio/recall/ags_play_dssi_audio.c \
     ../ags/audio/recall/ags_play_dssi_audio_run.c \
     ../ags/audio/recall/ags_play_lv2_audio.c \
     ../ags/audio/recall/ags_play_lv2_audio_run.c \
     ../ags/audio/recall/ags_play_notation_audio.c \
     ../ags/audio/recall/ags_play_notation_audio_run.c \
     ../ags/audio/recall/ags_play_recycling.c \
     ../ags/audio/recall/ags_play_wave_audio.c \
     ../ags/audio/recall/ags_play_wave_audio_run.c \
     ../ags/audio/recall/ags_play_wave_channel.c \
     ../ags/audio/recall/ags_play_wave_channel_run.c \
     ../ags/audio/recall/ags_prepare_audio_signal.c \
     ../ags/audio/recall/ags_prepare_channel.c \
     ../ags/audio/recall/ags_prepare_channel_run.c \
     ../ags/audio/recall/ags_prepare_recycling.c \
     ../ags/audio/recall/ags_record_midi_audio.c \
     ../ags/audio/recall/ags_record_midi_audio_run.c \
     ../ags/audio/recall/ags_route_dssi_audio.c \
     ../ags/audio/recall/ags_route_dssi_audio_run.c \
     ../ags/audio/recall/ags_route_lv2_audio.c \
     ../ags/audio/recall/ags_route_lv2_audio_run.c \
     ../ags/audio/recall/ags_rt_stream_audio_signal.c \
     ../ags/audio/recall/ags_rt_stream_channel.c \
     ../ags/audio/recall/ags_rt_stream_channel_run.c \
     ../ags/audio/recall/ags_rt_stream_recycling.c \
     ../ags/audio/recall/ags_stream_audio_signal.c \
     ../ags/audio/recall/ags_stream_channel.c \
     ../ags/audio/recall/ags_stream_channel_run.c \
     ../ags/audio/recall/ags_stream_recycling.c \
     ../ags/audio/recall/ags_volume_audio_signal.c \
     ../ags/audio/recall/ags_volume_channel.c \
     ../ags/audio/recall/ags_volume_channel_run.c \
     ../ags/audio/recall/ags_volume_recycling.c \
     ../ags/audio/task/ags_add_audio.c \
     ../ags/audio/task/ags_add_audio_signal.c \
     ../ags/audio/task/ags_add_effect.c \
     ../ags/audio/task/ags_add_note.c \
     ../ags/audio/task/ags_add_soundcard.c \
     ../ags/audio/task/ags_apply_bpm.c \
     ../ags/audio/task/ags_apply_presets.c \
     ../ags/audio/task/ags_apply_sequencer_length.c \
     ../ags/audio/task/ags_apply_sf2_synth.c \
     ../ags/audio/task/ags_apply_sfz_synth.c \
     ../ags/audio/task/ags_apply_sound_config.c \
     ../ags/audio/task/ags_apply_synth.c \
     ../ags/audio/task/ags_apply_tact.c \
     ../ags/audio/task/ags_cancel_audio.c \
     ../ags/audio/task/ags_cancel_channel.c \
     ../ags/audio/task/ags_clear_audio_signal.c \
     ../ags/audio/task/ags_clear_buffer.c \
     ../ags/audio/task/ags_crop_note.c \
     ../ags/audio/task/ags_export_output.c \
     ../ags/audio/task/ags_free_selection.c \
     ../ags/audio/task/ags_link_channel.c \
     ../ags/audio/task/ags_move_note.c \
     ../ags/audio/task/ags_open_file.c \
     ../ags/audio/task/ags_open_sf2_instrument.c \
     ../ags/audio/task/ags_open_sf2_sample.c \
     ../ags/audio/task/ags_open_sfz_file.c \
     ../ags/audio/task/ags_open_single_file.c \
     ../ags/audio/task/ags_open_wave.c \
     ../ags/audio/task/ags_remove_audio.c \
     ../ags/audio/task/ags_remove_audio_signal.c \
     ../ags/audio/task/ags_remove_note.c \
     ../ags/audio/task/ags_remove_soundcard.c \
     ../ags/audio/task/ags_reset_amplitude.c \
     ../ags/audio/task/ags_reset_fx_analyse.c \
     ../ags/audio/task/ags_reset_fx_peak.c \
     ../ags/audio/task/ags_reset_note.c \
     ../ags/audio/task/ags_reset_peak.c \
     ../ags/audio/task/ags_resize_audio.c \
     ../ags/audio/task/ags_seek_soundcard.c \
     ../ags/audio/task/ags_set_audio_channels.c \
     ../ags/audio/task/ags_set_buffer_size.c \
     ../ags/audio/task/ags_set_device.c \
     ../ags/audio/task/ags_set_format.c \
     ../ags/audio/task/ags_set_muted.c \
     ../ags/audio/task/ags_set_samplerate.c \
     ../ags/audio/task/ags_start_audio.c \
     ../ags/audio/task/ags_start_channel.c \
     ../ags/audio/task/ags_start_sequencer.c \
     ../ags/audio/task/ags_start_soundcard.c \
     ../ags/audio/task/ags_stop_sequencer.c \
     ../ags/audio/task/ags_stop_soundcard.c \
     ../ags/audio/task/ags_switch_buffer_flag.c \
     ../ags/audio/task/ags_tic_device.c \
     ../ags/audio/task/ags_toggle_pattern_bit.c \
     ../ags/audio/thread/ags_audio_loop.c \
     ../ags/audio/thread/ags_audio_thread.c \
     ../ags/audio/thread/ags_channel_thread.c \
     ../ags/audio/thread/ags_export_thread.c \
     ../ags/audio/thread/ags_sequencer_thread.c \
     ../ags/audio/thread/ags_sf2_loader.c \
     ../ags/audio/thread/ags_sfz_loader.c \
     ../ags/audio/thread/ags_soundcard_thread.c \
     ../ags/audio/thread/ags_wave_loader.c \
     ../ags/audio/wasapi/ags_wasapi_devin.c \
     ../ags/audio/wasapi/ags_wasapi_devout.c \
     ../ags/file/ags_file.c \
     ../ags/file/ags_file_id_ref.c \
     ../ags/file/ags_file_launch.c \
     ../ags/file/ags_file_link.c \
     ../ags/file/ags_file_lookup.c \
     ../ags/lib/ags_buffer_util.c \
     ../ags/lib/ags_complex.c \
     ../ags/lib/ags_conversion.c \
     ../ags/lib/ags_endian.c \
     ../ags/lib/ags_function.c \
     ../ags/lib/ags_log.c \
     ../ags/lib/ags_math_util.c \
     ../ags/lib/ags_regex.c \
     ../ags/lib/ags_solver_matrix.c \
     ../ags/lib/ags_solver_polynomial.c \
     ../ags/lib/ags_solver_vector.c \
     ../ags/lib/ags_string_util.c \
     ../ags/lib/ags_time.c \
     ../ags/lib/ags_turtle.c \
     ../ags/lib/ags_turtle_manager.c \
     ../ags/lib/ags_uuid.c \
     ../ags/object/ags_applicable.c \
     ../ags/object/ags_application_context.c \
     ../ags/object/ags_config.c \
     ../ags/object/ags_connectable.c \
     ../ags/object/ags_countable.c \
     ../ags/object/ags_cursor.c \
     ../ags/object/ags_globals.c \
     ../ags/object/ags_main_loop.c \
     ../ags/object/ags_marshal.c \
     ../ags/object/ags_mutable.c \
     ../ags/object/ags_plugin.c \
     ../ags/object/ags_portlet.c \
     ../ags/object/ags_priority.c \
     ../ags/object/ags_seekable.c \
     ../ags/object/ags_sequencer.c \
     ../ags/object/ags_sound_server.c \
     ../ags/object/ags_soundcard.c \
     ../ags/object/ags_tactable.c \
     ../ags/plugin/ags_base_plugin.c \
     ../ags/plugin/ags_dssi_manager.c \
     ../ags/plugin/ags_dssi_plugin.c \
     ../ags/plugin/ags_ladspa_conversion.c \
     ../ags/plugin/ags_ladspa_manager.c \
     ../ags/plugin/ags_ladspa_plugin.c \
     ../ags/plugin/ags_lv2_conversion.c \
     ../ags/plugin/ags_lv2_event_manager.c \
     ../ags/plugin/ags_lv2_log_manager.c \
     ../ags/plugin/ags_lv2_manager.c \
     ../ags/plugin/ags_lv2_option_manager.c \
     ../ags/plugin/ags_lv2_plugin.c \
     ../ags/plugin/ags_lv2_preset.c \
     ../ags/plugin/ags_lv2_preset_manager.c \
     ../ags/plugin/ags_lv2_turtle_parser.c \
     ../ags/plugin/ags_lv2_turtle_scanner.c \
     ../ags/plugin/ags_lv2_uri_map_manager.c \
     ../ags/plugin/ags_lv2_urid_manager.c \
     ../ags/plugin/ags_lv2_worker.c \
     ../ags/plugin/ags_lv2_worker_manager.c \
     ../ags/plugin/ags_lv2ui_manager.c \
     ../ags/plugin/ags_lv2ui_plugin.c \
     ../ags/plugin/ags_plugin_port.c \
     ../ags/server/ags_registry.c \
     ../ags/server/ags_server.c \
     ../ags/server/ags_server_application_context.c \
     ../ags/server/ags_server_status.c \
     ../ags/server/ags_service_provider.c \
     ../ags/server/controller/ags_controller.c \
     ../ags/server/controller/ags_front_controller.c \
     ../ags/server/controller/ags_plugin_controller.c \
     ../ags/server/security/ags_auth_security_context.c \
     ../ags/server/security/ags_authentication.c \
     ../ags/server/security/ags_authentication_manager.c \
     ../ags/server/security/ags_business_group.c \
     ../ags/server/security/ags_business_group_manager.c \
     ../ags/server/security/ags_certificate.c \
     ../ags/server/security/ags_certificate_manager.c \
     ../ags/server/security/ags_password_store.c \
     ../ags/server/security/ags_password_store_manager.c \
     ../ags/server/security/ags_security_context.c \
     ../ags/server/security/ags_xml_authentication.c \
     ../ags/server/security/ags_xml_business_group.c \
     ../ags/server/security/ags_xml_certificate.c \
     ../ags/server/security/ags_xml_password_store.c \
     ../ags/thread/ags_concurrency_provider.c \
     ../ags/thread/ags_destroy_worker.c \
     ../ags/thread/ags_generic_main_loop.c \
     ../ags/thread/ags_message_delivery.c \
     ../ags/thread/ags_message_envelope.c \
     ../ags/thread/ags_message_queue.c \
     ../ags/thread/ags_returnable_thread.c \
     ../ags/thread/ags_task.c \
     ../ags/thread/ags_task_completion.c \
     ../ags/thread/ags_task_launcher.c \
     ../ags/thread/ags_thread.c \
     ../ags/thread/ags_thread_application_context.c \
     ../ags/thread/ags_thread_pool.c \
     ../ags/thread/ags_timestamp.c \
     ../ags/thread/ags_worker_thread.c \
     ../ags/util/ags_destroy_util.c \
     ../ags/util/ags_id_generator.c \
     ../ags/util/ags_list_util.c \
     ../ags/util/ags_soundcard_helper.c \
     ../ags/widget/ags_cartesian.c \
     ../ags/widget/ags_container.c \
     ../ags/widget/ags_dial.c \
     ../ags/widget/ags_expander.c \
     ../ags/widget/ags_expander_set.c \
     ../ags/widget/ags_hindicator.c \
     ../ags/widget/ags_hled_array.c \
     ../ags/widget/ags_hlevel_box.c \
     ../ags/widget/ags_hscale_box.c \
     ../ags/widget/ags_indicator.c \
     ../ags/widget/ags_led.c \
     ../ags/widget/ags_led_array.c \
     ../ags/widget/ags_level.c \
     ../ags/widget/ags_level_box.c \
     ../ags/widget/ags_notebook.c \
     ../ags/widget/ags_piano.c \
     ../ags/widget/ags_ruler.c \
     ../ags/widget/ags_scale.c \
     ../ags/widget/ags_scale_box.c \
     ../ags/widget/ags_scrolled_level_box.c \
     ../ags/widget/ags_scrolled_piano.c \
     ../ags/widget/ags_scrolled_scale_box.c \
     ../ags/widget/ags_vindicator.c \
     ../ags/widget/ags_vled_array.c \
     ../ags/widget/ags_vlevel_box.c \
     ../ags/widget/ags_vscale_box.c \
     ../ags/widget/ags_widget_marshal.c

CATALOGS =  de.gmo fr.gmo it.gmo pl.gmo

POFILESDEPS_ = $(srcdir)/$(DOMAIN).pot
POFILESDEPS_yes = $(POFILESDEPS_)
POFILESDEPS_no =
POFILESDEPS = $(POFILESDEPS_$(PO_DEPENDS_ON_POT))

DISTFILESDEPS_ = update-po
DISTFILESDEPS_yes = $(DISTFILESDEPS_)
DISTFILESDEPS_no =
DISTFILESDEPS = $(DISTFILESDEPS_$(DIST_DEPENDS_ON_UPDATE_PO))

# Makevars gets inserted here. (Don't remove this line!)
# Makefile variables for PO directory in any package using GNU gettext.
#
# Copyright (C) 2003-2017 Free Software Foundation, Inc.
# This file, Makevars, is free software; the Free Software Foundation
# gives unlimited permission to use, copy, distribute, and modify it.

# Usually the message domain is the same as the package name.
DOMAIN = $(PACKAGE)

# These two variables depend on the location of this directory.
subdir = po
top_builddir = ..

# These options get passed to xgettext.
XGETTEXT_OPTIONS = --keyword=i18n --keyword=i18nM --keyword=i18nP:1,2

# This is the copyright holder that gets inserted into the header of the
# $(DOMAIN).pot file.  Set this to the copyright holder of the surrounding
# package.  (Note that the msgstr strings, extracted from the package's
# sources, belong to the copyright holder of the package.)  Translators are
# expected to transfer the copyright for their translations to this person
# or entity, or to disclaim their copyright.  The empty string stands for
# the public domain; in this case the translators are expected to disclaim
# their copyright.
COPYRIGHT_HOLDER = Joël Krähemann

# This tells whether or not to prepend "GNU " prefix to the package
# name that gets inserted into the header of the $(DOMAIN).pot file.
# Possible values are "yes", "no", or empty.  If it is empty, try to
# detect it automatically by scanning the files in $(top_srcdir) for
# "GNU packagename" string.
PACKAGE_GNU =

# This is the email address or URL to which the translators shall report
# bugs in the untranslated strings:
# - Strings which are not entire sentences, see the maintainer guidelines
#   in the GNU gettext documentation, section 'Preparing Strings'.
# - Strings which use unclear terms or require additional context to be
#   understood.
# - Strings which make invalid assumptions about notation of date, time or
#   money.
# - Pluralisation problems.
# - Incorrect English spelling.
# - Incorrect formatting.
# It can be your email address, or a mailing list address where translators
# can write to without being subscribed, or the URL of a web page through
# which the translators can contact you.
MSGID_BUGS_ADDRESS = jkraehemann@gmail.com

# This is the list of locale categories, beyond LC_MESSAGES, for which the
# message catalogs shall be used.  It is usually empty.
EXTRA_LOCALE_CATEGORIES =

# This tells whether the $(DOMAIN).pot file contains messages with an 'msgctxt'
# context.  Possible values are "yes" and "no".  Set this to yes if the
# package uses functions taking also a message context, like pgettext(), or
# if in $(XGETTEXT_OPTIONS) you define keywords with a context argument.
USE_MSGCTXT = no

# These options get passed to msgmerge.
# Useful options are in particular:
#   --previous            to keep previous msgids of translated messages,
#   --quiet               to reduce the verbosity.
MSGMERGE_OPTIONS =

# These options get passed to msginit.
# If you want to disable line wrapping when writing PO files, add
# --no-wrap to MSGMERGE_OPTIONS, XGETTEXT_OPTIONS, and
# MSGINIT_OPTIONS.
MSGINIT_OPTIONS =

# This tells whether or not to regenerate a PO file when $(DOMAIN).pot
# has changed.  Possible values are "yes" and "no".  Set this to no if
# the POT file is checked in the repository and the version control
# program ignores timestamps.
PO_DEPENDS_ON_POT = yes

# This tells whether or not to forcibly update $(DOMAIN).pot and
# regenerate PO files on "make dist".  Possible values are "yes" and
# "no".  Set this to no if the POT file and PO files are maintained
# externally.
DIST_DEPENDS_ON_UPDATE_PO = yes

.SUFFIXES:
.SUFFIXES: .po .gmo .mo .sed .sin .nop .po-create .po-update

.po.mo:
	@echo "$(MSGFMT) -c -o $@ $<"; \
	$(MSGFMT) -c -o t-$@ $< && mv t-$@ $@

.po.gmo:
	@lang=`echo $* | sed -e 's,.*/,,'`; \
	test "$(srcdir)" = . && cdcmd="" || cdcmd="cd $(srcdir) && "; \
	echo "$${cdcmd}rm -f $${lang}.gmo && $(GMSGFMT) -c --statistics --verbose -o $${lang}.gmo $${lang}.po"; \
	cd $(srcdir) && rm -f $${lang}.gmo && $(GMSGFMT) -c --statistics --verbose -o t-$${lang}.gmo $${lang}.po && mv t-$${lang}.gmo $${lang}.gmo

.sin.sed:
	sed -e '/^#/d' $< > t-$@
	mv t-$@ $@


all: all-yes

all-yes: stamp-po
all-no:

# Ensure that the gettext macros and this Makefile.in.in are in sync.
CHECK_MACRO_VERSION = \
	test "$(GETTEXT_MACRO_VERSION)" = "0.19" \
	  || { echo "*** error: gettext infrastructure mismatch: using a Makefile.in.in from gettext version $(GETTEXT_MACRO_VERSION) but the autoconf macros are from gettext version 0.19" 1>&2; \
	       exit 1; \
	     }

# $(srcdir)/$(DOMAIN).pot is only created when needed. When xgettext finds no
# internationalized messages, no $(srcdir)/$(DOMAIN).pot is created (because
# we don't want to bother translators with empty POT files). We assume that
# LINGUAS is empty in this case, i.e. $(POFILES) and $(GMOFILES) are empty.
# In this case, stamp-po is a nop (i.e. a phony target).

# stamp-po is a timestamp denoting the last time at which the CATALOGS have
# been loosely updated. Its purpose is that when a developer or translator
# checks out the package via CVS, and the $(DOMAIN).pot file is not in CVS,
# "make" will update the $(DOMAIN).pot and the $(CATALOGS), but subsequent
# invocations of "make" will do nothing. This timestamp would not be necessary
# if updating the $(CATALOGS) would always touch them; however, the rule for
# $(POFILES) has been designed to not touch files that don't need to be
# changed.
stamp-po: $(srcdir)/$(DOMAIN).pot
	@$(CHECK_MACRO_VERSION)
	test ! -f $(srcdir)/$(DOMAIN).pot || \
	  test -z "$(GMOFILES)" || $(MAKE) $(GMOFILES)
	@test ! -f $(srcdir)/$(DOMAIN).pot || { \
	  echo "touch stamp-po" && \
	  echo timestamp > stamp-poT && \
	  mv stamp-poT stamp-po; \
	}

# Note: Target 'all' must not depend on target '$(DOMAIN).pot-update',
# otherwise packages like GCC can not be built if only parts of the source
# have been downloaded.

# This target rebuilds $(DOMAIN).pot; it is an expensive operation.
# Note that $(DOMAIN).pot is not touched if it doesn't need to be changed.
# The determination of whether the package xyz is a GNU one is based on the
# heuristic whether some file in the top level directory mentions "GNU xyz".
# If GNU 'find' is available, we avoid grepping through monster files.
$(DOMAIN).pot-update: $(POTFILES) $(srcdir)/POTFILES.in remove-potcdate.sed
	package_gnu="$(PACKAGE_GNU)"; \
	test -n "$$package_gnu" || { \
	  if { if (LC_ALL=C find --version) 2>/dev/null | grep GNU >/dev/null; then \
		 LC_ALL=C find -L $(top_srcdir) -maxdepth 1 -type f \
			       -size -10000000c -exec grep 'GNU gsequencer' \
			       /dev/null '{}' ';' 2>/dev/null; \
	       else \
		 LC_ALL=C grep 'GNU gsequencer' $(top_srcdir)/* 2>/dev/null; \
	       fi; \
	     } | grep -v 'libtool:' >/dev/null; then \
	     package_gnu=yes; \
	   else \
	     package_gnu=no; \
	   fi; \
	}; \
	if test "$$package_gnu" = "yes"; then \
	  package_prefix='GNU '; \
	else \
	  package_prefix=''; \
	fi; \
	if test -n '$(MSGID_BUGS_ADDRESS)' || test '$(PACKAGE_BUGREPORT)' = '@'PACKAGE_BUGREPORT'@'; then \
	  msgid_bugs_address='$(MSGID_BUGS_ADDRESS)'; \
	else \
	  msgid_bugs_address='$(PACKAGE_BUGREPORT)'; \
	fi; \
	case `$(XGETTEXT) --version | sed 1q | sed -e 's,^[^0-9]*,,'` in \
	  '' | 0.[0-9] | 0.[0-9].* | 0.1[0-5] | 0.1[0-5].* | 0.16 | 0.16.[0-1]*) \
	    $(XGETTEXT) --default-domain=$(DOMAIN) --directory=$(top_srcdir) \
	      --add-comments=TRANSLATORS: $(XGETTEXT_OPTIONS)  \
	      --files-from=$(srcdir)/POTFILES.in \
	      --copyright-holder='$(COPYRIGHT_HOLDER)' \
	      --msgid-bugs-address="$$msgid_bugs_address" \
	    ;; \
	  *) \
	    $(XGETTEXT) --default-domain=$(DOMAIN) --directory=$(top_srcdir) \
	      --add-comments=TRANSLATORS: $(XGETTEXT_OPTIONS)  \
	      --files-from=$(srcdir)/POTFILES.in \
	      --copyright-holder='$(COPYRIGHT_HOLDER)' \
	      --package-name="$${package_prefix}gsequencer" \
	      --package-version='3.18.2' \
	      --msgid-bugs-address="$$msgid_bugs_address" \
	    ;; \
	esac
	test ! -f $(DOMAIN).po || { \
	  if test -f $(srcdir)/$(DOMAIN).pot-header; then \
	    sed -e '1,/^#$$/d' < $(DOMAIN).po > $(DOMAIN).1po && \
	    cat $(srcdir)/$(DOMAIN).pot-header $(DOMAIN).1po > $(DOMAIN).po; \
	    rm -f $(DOMAIN).1po; \
	  fi; \
	  if test -f $(srcdir)/$(DOMAIN).pot; then \
	    sed -f remove-potcdate.sed < $(srcdir)/$(DOMAIN).pot > $(DOMAIN).1po && \
	    sed -f remove-potcdate.sed < $(DOMAIN).po > $(DOMAIN).2po && \
	    if cmp $(DOMAIN).1po $(DOMAIN).2po >/dev/null 2>&1; then \
	      rm -f $(DOMAIN).1po $(DOMAIN).2po $(DOMAIN).po; \
	    else \
	      rm -f $(DOMAIN).1po $(DOMAIN).2po $(srcdir)/$(DOMAIN).pot && \
	      mv $(DOMAIN).po $(srcdir)/$(DOMAIN).pot; \
	    fi; \
	  else \
	    mv $(DOMAIN).po $(srcdir)/$(DOMAIN).pot; \
	  fi; \
	}

# This rule has no dependencies: we don't need to update $(DOMAIN).pot at
# every "make" invocation, only create it when it is missing.
# Only "make $(DOMAIN).pot-update" or "make dist" will force an update.
$(srcdir)/$(DOMAIN).pot:
	$(MAKE) $(DOMAIN).pot-update

# This target rebuilds a PO file if $(DOMAIN).pot has changed.
# Note that a PO file is not touched if it doesn't need to be changed.
$(POFILES): $(POFILESDEPS)
	@lang=`echo $@ | sed -e 's,.*/,,' -e 's/\.po$$//'`; \
	if test -f "$(srcdir)/$${lang}.po"; then \
	  test -f $(srcdir)/$(DOMAIN).pot || $(MAKE) $(srcdir)/$(DOMAIN).pot; \
	  test "$(srcdir)" = . && cdcmd="" || cdcmd="cd $(srcdir) && "; \
	  echo "$${cdcmd}$(MSGMERGE_UPDATE) $(MSGMERGE_OPTIONS) --lang=$${lang} $${lang}.po $(DOMAIN).pot"; \
	  cd $(srcdir) \
	    && { case `$(MSGMERGE) --version | sed 1q | sed -e 's,^[^0-9]*,,'` in \
	           '' | 0.[0-9] | 0.[0-9].* | 0.1[0-7] | 0.1[0-7].*) \
	             $(MSGMERGE_UPDATE) $(MSGMERGE_OPTIONS) $${lang}.po $(DOMAIN).pot;; \
	           *) \
	             $(MSGMERGE_UPDATE) $(MSGMERGE_OPTIONS) --lang=$${lang} $${lang}.po $(DOMAIN).pot;; \
	         esac; \
	       }; \
	else \
	  $(MAKE) $${lang}.po-create; \
	fi


install: install-exec install-data
install-exec:
install-data: install-data-yes
	if test "$(PACKAGE)" = "gettext-tools"; then \
	  $(mkdir_p) $(DESTDIR)$(gettextsrcdir); \
	  for file in $(DISTFILES.common) Makevars.template; do \
	    $(INSTALL_DATA) $(srcdir)/$$file \
			    $(DESTDIR)$(gettextsrcdir)/$$file; \
	  done; \
	  for file in Makevars; do \
	    rm -f $(DESTDIR)$(gettextsrcdir)/$$file; \
	  done; \
	else \
	  : ; \
	fi
install-data-no: all
install-data-yes: all
	@catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed -e 's/\.gmo$$//'`; \
	  dir=$(localedir)/$$lang/LC_MESSAGES; \
	  $(mkdir_p) $(DESTDIR)$$dir; \
	  if test -r $$cat; then realcat=$$cat; else realcat=$(srcdir)/$$cat; fi; \
	  $(INSTALL_DATA) $$realcat $(DESTDIR)$$dir/$(DOMAIN).mo; \
	  echo "installing $$realcat as $(DESTDIR)$$dir/$(DOMAIN).mo"; \
	  for lc in '' $(EXTRA_LOCALE_CATEGORIES); do \
	    if test -n "$$lc"; then \
	      if (cd $(DESTDIR)$(localedir)/$$lang && LC_ALL=C ls -l -d $$lc 2>/dev/null) | grep ' -> ' >/dev/null; then \
	        link=`cd $(DESTDIR)$(localedir)/$$lang && LC_ALL=C ls -l -d $$lc | sed -e 's/^.* -> //'`; \
	        mv $(DESTDIR)$(localedir)/$$lang/$$lc $(DESTDIR)$(localedir)/$$lang/$$lc.old; \
	        mkdir $(DESTDIR)$(localedir)/$$lang/$$lc; \
	        (cd $(DESTDIR)$(localedir)/$$lang/$$lc.old && \
	         for file in *; do \
	           if test -f $$file; then \
	             ln -s ../$$link/$$file $(DESTDIR)$(localedir)/$$lang/$$lc/$$file; \
	           fi; \
	         done); \
	        rm -f $(DESTDIR)$(localedir)/$$lang/$$lc.old; \
	      else \
	        if test -d $(DESTDIR)$(localedir)/$$lang/$$lc; then \
	          :; \
	        else \
	          rm -f $(DESTDIR)$(localedir)/$$lang/$$lc; \
	          mkdir $(DESTDIR)$(localedir)/$$lang/$$lc; \
	        fi; \
	      fi; \
	      rm -f $(DESTDIR)$(localedir)/$$lang/$$lc/$(DOMAIN).mo; \
	      ln -s ../LC_MESSAGES/$(DOMAIN).mo $(DESTDIR)$(localedir)/$$lang/$$lc/$(DOMAIN).mo 2>/dev/null || \
	      ln $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(DOMAIN).mo $(DESTDIR)$(localedir)/$$lang/$$lc/$(DOMAIN).mo 2>/dev/null || \
	      cp -p $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(DOMAIN).mo $(DESTDIR)$(localedir)/$$lang/$$lc/$(DOMAIN).mo; \
	      echo "installing $$realcat link as $(DESTDIR)$(localedir)/$$lang/$$lc/$(DOMAIN).mo"; \
	    fi; \
	  done; \
	done

install-strip: install

installdirs: installdirs-exec installdirs-data
installdirs-exec:
installdirs-data: installdirs-data-yes
	if test "$(PACKAGE)" = "gettext-tools"; then \
	  $(mkdir_p) $(DESTDIR)$(gettextsrcdir); \
	else \
	  : ; \
	fi
installdirs-data-no:
installdirs-data-yes:
	@catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed -e 's/\.gmo$$//'`; \
	  dir=$(localedir)/$$lang/LC_MESSAGES; \
	  $(mkdir_p) $(DESTDIR)$$dir; \
	  for lc in '' $(EXTRA_LOCALE_CATEGORIES); do \
	    if test -n "$$lc"; then \
	      if (cd $(DESTDIR)$(localedir)/$$lang && LC_ALL=C ls -l -d $$lc 2>/dev/null) | grep ' -> ' >/dev/null; then \
	        link=`cd $(DESTDIR)$(localedir)/$$lang && LC_ALL=C ls -l -d $$lc | sed -e 's/^.* -> //'`; \
	        mv $(DESTDIR)$(localedir)/$$lang/$$lc $(DESTDIR)$(localedir)/$$lang/$$lc.old; \
	        mkdir $(DESTDIR)$(localedir)/$$lang/$$lc; \
	        (cd $(DESTDIR)$(localedir)/$$lang/$$lc.old && \
	         for file in *; do \
	           if test -f $$file; then \
	             ln -s ../$$link/$$file $(DESTDIR)$(localedir)/$$lang/$$lc/$$file; \
	           fi; \
	         done); \
	        rm -f $(DESTDIR)$(localedir)/$$lang/$$lc.old; \
	      else \
	        if test -d $(DESTDIR)$(localedir)/$$lang/$$lc; then \
	          :; \
	        else \
	          rm -f $(DESTDIR)$(localedir)/$$lang/$$lc; \
	          mkdir $(DESTDIR)$(localedir)/$$lang/$$lc; \
	        fi; \
	      fi; \
	    fi; \
	  done; \
	done

# Define this as empty until I found a useful application.
installcheck:

uninstall: uninstall-exec uninstall-data
uninstall-exec:
uninstall-data: uninstall-data-yes
	if test "$(PACKAGE)" = "gettext-tools"; then \
	  for file in $(DISTFILES.common) Makevars.template; do \
	    rm -f $(DESTDIR)$(gettextsrcdir)/$$file; \
	  done; \
	else \
	  : ; \
	fi
uninstall-data-no:
uninstall-data-yes:
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed -e 's/\.gmo$$//'`; \
	  for lc in LC_MESSAGES $(EXTRA_LOCALE_CATEGORIES); do \
	    rm -f $(DESTDIR)$(localedir)/$$lang/$$lc/$(DOMAIN).mo; \
	  done; \
	done

check: all

info dvi ps pdf html tags TAGS ctags CTAGS ID:

mostlyclean:
	rm -f remove-potcdate.sed
	rm -f stamp-poT
	rm -f core core.* $(DOMAIN).po $(DOMAIN).1po $(DOMAIN).2po *.new.po
	rm -fr *.o

clean: mostlyclean

distclean: clean
	rm -f Makefile Makefile.in POTFILES *.mo

maintainer-clean: distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."
	rm -f stamp-po $(GMOFILES)

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)
dist distdir:
	test -z "$(DISTFILESDEPS)" || $(MAKE) $(DISTFILESDEPS)
	@$(MAKE) dist2
# This is a separate target because 'update-po' must be executed before.
dist2: stamp-po $(DISTFILES)
	dists="$(DISTFILES)"; \
	if test "$(PACKAGE)" = "gettext-tools"; then \
	  dists="$$dists Makevars.template"; \
	fi; \
	if test -f $(srcdir)/$(DOMAIN).pot; then \
	  dists="$$dists $(DOMAIN).pot stamp-po"; \
	fi; \
	if test -f $(srcdir)/ChangeLog; then \
	  dists="$$dists ChangeLog"; \
	fi; \
	for i in 0 1 2 3 4 5 6 7 8 9; do \
	  if test -f $(srcdir)/ChangeLog.$$i; then \
	    dists="$$dists ChangeLog.$$i"; \
	  fi; \
	done; \
	if test -f $(srcdir)/LINGUAS; then dists="$$dists LINGUAS"; fi; \
	for file in $$dists; do \
	  if test -f $$file; then \
	    cp -p $$file $(distdir) || exit 1; \
	  else \
	    cp -p $(srcdir)/$$file $(distdir) || exit 1; \
	  fi; \
	done

update-po: Makefile
	$(MAKE) $(DOMAIN).pot-update
	test -z "$(UPDATEPOFILES)" || $(MAKE) $(UPDATEPOFILES)
	$(MAKE) update-gmo

# General rule for creating PO files.

.nop.po-create:
	@lang=`echo $@ | sed -e 's/\.po-create$$//'`; \
	echo "File $$lang.po does not exist. If you are a translator, you can create it through 'msginit'." 1>&2; \
	exit 1

# General rule for updating PO files.

.nop.po-update:
	@lang=`echo $@ | sed -e 's/\.po-update$$//'`; \
	if test "$(PACKAGE)" = "gettext-tools" && test "$(CROSS_COMPILING)" != "yes"; then PATH=`pwd`/../src:$$PATH; fi; \
	tmpdir=`pwd`; \
	echo "$$lang:"; \
	test "$(srcdir)" = . && cdcmd="" || cdcmd="cd $(srcdir) && "; \
	echo "$${cdcmd}$(MSGMERGE) $(MSGMERGE_OPTIONS) --lang=$$lang $$lang.po $(DOMAIN).pot -o $$lang.new.po"; \
	cd $(srcdir); \
	if { case `$(MSGMERGE) --version | sed 1q | sed -e 's,^[^0-9]*,,'` in \
	       '' | 0.[0-9] | 0.[0-9].* | 0.1[0-7] | 0.1[0-7].*) \
	         $(MSGMERGE) $(MSGMERGE_OPTIONS) -o $$tmpdir/$$lang.new.po $$lang.po $(DOMAIN).pot;; \
	       *) \
	         $(MSGMERGE) $(MSGMERGE_OPTIONS) --lang=$$lang -o $$tmpdir/$$lang.new.po $$lang.po $(DOMAIN).pot;; \
	     esac; \
	   }; then \
	  if cmp $$lang.po $$tmpdir/$$lang.new.po >/dev/null 2>&1; then \
	    rm -f $$tmpdir/$$lang.new.po; \
	  else \
	    if mv -f $$tmpdir/$$lang.new.po $$lang.po; then \
	      :; \
	    else \
	      echo "msgmerge for $$lang.po failed: cannot move $$tmpdir/$$lang.new.po to $$lang.po" 1>&2; \
	      exit 1; \
	    fi; \
	  fi; \
	else \
	  echo "msgmerge for $$lang.po failed!" 1>&2; \
	  rm -f $$tmpdir/$$lang.new.po; \
	fi

$(DUMMYPOFILES):

update-gmo: Makefile $(GMOFILES)
	@:

# Recreate Makefile by invoking config.status. Explicitly invoke the shell,
# because execution permission bits may not work on the current file system.
# Use /bin/sh, which is the shell determined by autoconf for the use by its
# scripts, not $(SHELL) which is hardwired to /bin/sh and may be deficient.
Makefile: Makefile.in.in Makevars $(top_builddir)/config.status POTFILES.in LINGUAS
	cd $(top_builddir) \
	  && /bin/sh ./config.status $(subdir)/$@.in po-directories

force:

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
# This file, Rules-quot, can be copied and used freely without restrictions.
# Special Makefile rules for English message catalogs with quotation marks.

DISTFILES.common.extra1 = quot.sed boldquot.sed en@quot.header en@boldquot.header insert-header.sin Rules-quot

.SUFFIXES: .insert-header .po-update-en

en@quot.po-create:
	$(MAKE) en@quot.po-update
en@boldquot.po-create:
	$(MAKE) en@boldquot.po-update

en@quot.po-update: en@quot.po-update-en
en@boldquot.po-update: en@boldquot.po-update-en

.insert-header.po-update-en:
	@lang=`echo $@ | sed -e 's/\.po-update-en$$//'`; \
	if test "$(PACKAGE)" = "gettext-tools" && test "$(CROSS_COMPILING)" != "yes"; then PATH=`pwd`/../src:$$PATH; GETTEXTLIBDIR=`cd $(top_srcdir)/src && pwd`; export GETTEXTLIBDIR; fi; \
	tmpdir=`pwd`; \
	echo "$$lang:"; \
	ll=`echo $$lang | sed -e 's/@.*//'`; \
	LC_ALL=C; export LC_ALL; \
	cd $(srcdir); \
	if $(MSGINIT) $(MSGINIT_OPTIONS) -i $(DOMAIN).pot --no-translator -l $$lang -o - 2>/dev/null \
	   | $(SED) -f $$tmpdir/$$lang.insert-header | $(MSGCONV) -t UTF-8 | \
	   { case `$(MSGFILTER) --version | sed 1q | sed -e 's,^[^0-9]*,,'` in \
	     '' | 0.[0-9] | 0.[0-9].* | 0.1[0-8] | 0.1[0-8].*) \
	       $(MSGFILTER) $(SED) -f `echo $$lang | sed -e 's/.*@//'`.sed \
	       ;; \
	     *) \
	       $(MSGFILTER) `echo $$lang | sed -e 's/.*@//'` \
	       ;; \
	     esac } 2>/dev/null > $$tmpdir/$$lang.new.po \
	     ; then \
	  if cmp $$lang.po $$tmpdir/$$lang.new.po >/dev/null 2>&1; then \
	    rm -f $$tmpdir/$$lang.new.po; \
	  else \
	    if mv -f $$tmpdir/$$lang.new.po $$lang.po; then \
	      :; \
	    else \
	      echo "creation of $$lang.po failed: cannot move $$tmpdir/$$lang.new.po to $$lang.po" 1>&2; \
	      exit 1; \
	    fi; \
	  fi; \
	else \
	  echo "creation of $$lang.po failed!" 1>&2; \
	  rm -f $$tmpdir/$$lang.new.po; \
	fi

en@quot.insert-header: insert-header.sin
	sed -e '/^#/d' -e 's/HEADER/en@quot.header/g' $(srcdir)/insert-header.sin > en@quot.insert-header

en@boldquot.insert-header: insert-header.sin
	sed -e '/^#/d' -e 's/HEADER/en@boldquot.header/g' $(srcdir)/insert-header.sin > en@boldquot.insert-header

mostlyclean: mostlyclean-quot
mostlyclean-quot:
	rm -f *.insert-header
