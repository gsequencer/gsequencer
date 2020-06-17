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

#ifndef __LIBAGS_AUDIO_H__
#define __LIBAGS_AUDIO_H__

/* plugin */
#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_dssi_plugin.h>
#include <ags/plugin/ags_ladspa_conversion.h>
#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_ladspa_plugin.h>
#include <ags/plugin/ags_lv2_conversion.h>
#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_lv2_preset.h>
#include <ags/plugin/ags_lv2_event_manager.h>
#include <ags/plugin/ags_lv2_log_manager.h>
#include <ags/plugin/ags_lv2_option_manager.h>
#include <ags/plugin/ags_lv2_turtle_parser.h>
#include <ags/plugin/ags_lv2_uri_map_manager.h>
#include <ags/plugin/ags_lv2_urid_manager.h>
#include <ags/plugin/ags_lv2_worker_manager.h>
#include <ags/plugin/ags_lv2_worker.h>
#include <ags/plugin/ags_lv2ui_manager.h>
#include <ags/plugin/ags_lv2ui_plugin.h>
#include <ags/plugin/ags_plugin_stock.h>
#include <ags/plugin/ags_plugin_port.h>

/* audio */
#include <ags/audio/ags_acceleration.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_audio_application_context.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_automation.h>
#include <ags/audio/ags_buffer.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_devin.h>
#include <ags/audio/ags_diatonic_scale.h>
#include <ags/audio/ags_fifoout.h>
#include <ags/audio/ags_filter_util.h>
#include <ags/audio/ags_fm_synth_util.h>
#include <ags/audio/ags_fourier_transform_util.h>
#include <ags/audio/ags_frequency_map_manager.h>
#include <ags/audio/ags_frequency_map.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_lfo_synth_util.h>
#include <ags/audio/ags_midi.h>
#include <ags/audio/ags_midiin.h>
#include <ags/audio/ags_notation.h>
#include <ags/audio/ags_note.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_preset.h>
#include <ags/audio/ags_fx_factory.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_audio_signal.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_generic_recall_channel_run.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_dependency.h>
#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_dssi.h>
#include <ags/audio/ags_recall_dssi_run.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_ladspa.h>
#include <ags/audio/ags_recall_ladspa_run.h>
#include <ags/audio/ags_recall_lv2.h>
#include <ags/audio/ags_recall_lv2_run.h>
#include <ags/audio/ags_generic_recall_recycling.h>
#include <ags/audio/ags_recall_recycling.h>
#include <ags/audio/ags_recycling_context.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_sequencer_util.h>
#include <ags/audio/ags_soundcard_util.h>
#include <ags/audio/ags_synth_enums.h>
#include <ags/audio/ags_synth_generator.h>
#include <ags/audio/ags_synth_util.h>
#include <ags/audio/ags_sf2_synth_generator.h>
#include <ags/audio/ags_sf2_synth_util.h>
#include <ags/audio/ags_sfz_synth_generator.h>
#include <ags/audio/ags_sfz_synth_util.h>
#include <ags/audio/ags_track.h>
#include <ags/audio/ags_wave.h>

/* audio thread */
#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>
#include <ags/audio/thread/ags_sequencer_thread.h>
#include <ags/audio/thread/ags_soundcard_thread.h>
#include <ags/audio/thread/ags_export_thread.h>
#include <ags/audio/thread/ags_sf2_loader.h>
#include <ags/audio/thread/ags_sfz_loader.h>
#include <ags/audio/thread/ags_wave_loader.h>

/* audio file */
#include <ags/audio/file/ags_audio_container.h>
#include <ags/audio/file/ags_audio_file.h>
#include <ags/audio/file/ags_audio_file_link.h>
#ifdef AGS_WITH_LIBINSTPATCH
#include <ags/audio/file/ags_ipatch.h>
#include <ags/audio/file/ags_ipatch_gig_reader.h>
#include <ags/audio/file/ags_ipatch_dls2_reader.h>
#include <ags/audio/file/ags_ipatch_sf2_reader.h>
#include <ags/audio/file/ags_ipatch_sample.h>
#endif
#include <ags/audio/file/ags_sfz_file.h>
#include <ags/audio/file/ags_sfz_group.h>
#include <ags/audio/file/ags_sfz_region.h>
#include <ags/audio/file/ags_sfz_sample.h>
#include <ags/audio/file/ags_sndfile.h>
#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>

/* audio midi */
#include <ags/audio/midi/ags_midi_buffer_util.h>
#include <ags/audio/midi/ags_midi_builder.h>
#include <ags/audio/midi/ags_midi_file.h>
#include <ags/audio/midi/ags_midi_parser.h>
#include <ags/audio/midi/ags_midi_util.h>

/* audio osc */
#include <ags/audio/osc/ags_osc_buffer_util.h>
#include <ags/audio/osc/ags_osc_builder.h>
#include <ags/audio/osc/ags_osc_client.h>
#include <ags/audio/osc/ags_osc_connection.h>
#include <ags/audio/osc/ags_osc_parser.h>
#include <ags/audio/osc/ags_osc_response.h>
#include <ags/audio/osc/ags_osc_server.h>
#include <ags/audio/osc/ags_osc_util.h>
#include <ags/audio/osc/ags_osc_websocket_connection.h>
#include <ags/audio/osc/ags_osc_xmlrpc_message.h>
#include <ags/audio/osc/ags_osc_xmlrpc_server.h>

#include <ags/audio/osc/controller/ags_osc_action_controller.h>
#include <ags/audio/osc/controller/ags_osc_config_controller.h>
#include <ags/audio/osc/controller/ags_osc_controller.h>
#include <ags/audio/osc/controller/ags_osc_front_controller.h>
#include <ags/audio/osc/controller/ags_osc_info_controller.h>
#include <ags/audio/osc/controller/ags_osc_meter_controller.h>
#include <ags/audio/osc/controller/ags_osc_node_controller.h>
#include <ags/audio/osc/controller/ags_osc_plugin_controller.h>
#include <ags/audio/osc/controller/ags_osc_renew_controller.h>
#include <ags/audio/osc/controller/ags_osc_status_controller.h>

#include <ags/audio/osc/xmlrpc/ags_osc_xmlrpc_controller.h>

/* audio wasapi */
#include <ags/audio/wasapi/ags_wasapi_devout.h>
#include <ags/audio/wasapi/ags_wasapi_devin.h>

/* audio core-audio */
#include <ags/audio/core-audio/ags_core_audio_client.h>
#include <ags/audio/core-audio/ags_core_audio_devout.h>
#include <ags/audio/core-audio/ags_core_audio_devin.h>
#include <ags/audio/core-audio/ags_core_audio_midiin.h>
#include <ags/audio/core-audio/ags_core_audio_port.h>
#include <ags/audio/core-audio/ags_core_audio_server.h>

/* audio audio-unit */
#include <ags/audio/audio-unit/ags_audio_unit_client.h>
#include <ags/audio/audio-unit/ags_audio_unit_devout.h>
#include <ags/audio/audio-unit/ags_audio_unit_devin.h>
#include <ags/audio/audio-unit/ags_audio_unit_port.h>
#include <ags/audio/audio-unit/ags_audio_unit_server.h>

/* audio pulse */
#include <ags/audio/pulse/ags_pulse_client.h>
#include <ags/audio/pulse/ags_pulse_devout.h>
#include <ags/audio/pulse/ags_pulse_devin.h>
#include <ags/audio/pulse/ags_pulse_port.h>
#include <ags/audio/pulse/ags_pulse_server.h>

/* audio jack */
#include <ags/audio/jack/ags_jack_client.h>
#include <ags/audio/jack/ags_jack_midiin.h>
#include <ags/audio/jack/ags_jack_devout.h>
#include <ags/audio/jack/ags_jack_devin.h>
#include <ags/audio/jack/ags_jack_port.h>
#include <ags/audio/jack/ags_jack_server.h>

/* audio recall */
#include <ags/audio/recall/ags_analyse_audio_signal.h>
#include <ags/audio/recall/ags_analyse_channel.h>
#include <ags/audio/recall/ags_analyse_channel_run.h>
#include <ags/audio/recall/ags_analyse_recycling.h>
#include <ags/audio/recall/ags_buffer_audio_signal.h>
#include <ags/audio/recall/ags_buffer_channel.h>
#include <ags/audio/recall/ags_buffer_channel_run.h>
#include <ags/audio/recall/ags_buffer_recycling.h>
#include <ags/audio/recall/ags_capture_wave_audio.h>
#include <ags/audio/recall/ags_capture_wave_audio_run.h>
#include <ags/audio/recall/ags_capture_wave_channel.h>
#include <ags/audio/recall/ags_capture_wave_channel_run.h>
#include <ags/audio/recall/ags_copy_audio_signal.h>
#include <ags/audio/recall/ags_copy_channel.h>
#include <ags/audio/recall/ags_copy_channel_run.h>
#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>
#include <ags/audio/recall/ags_copy_recycling.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_eq10_audio_signal.h>
#include <ags/audio/recall/ags_eq10_channel.h>
#include <ags/audio/recall/ags_eq10_channel_run.h>
#include <ags/audio/recall/ags_eq10_recycling.h>
#include <ags/audio/recall/ags_envelope_audio_signal.h>
#include <ags/audio/recall/ags_envelope_channel.h>
#include <ags/audio/recall/ags_envelope_channel_run.h>
#include <ags/audio/recall/ags_envelope_recycling.h>
#include <ags/audio/recall/ags_feed_audio_signal.h>
#include <ags/audio/recall/ags_feed_channel.h>
#include <ags/audio/recall/ags_feed_channel_run.h>
#include <ags/audio/recall/ags_feed_recycling.h>
#include <ags/audio/recall/ags_lfo_audio_signal.h>
#include <ags/audio/recall/ags_lfo_channel.h>
#include <ags/audio/recall/ags_lfo_channel_run.h>
#include <ags/audio/recall/ags_lfo_recycling.h>
#include <ags/audio/recall/ags_loop_channel.h>
#include <ags/audio/recall/ags_loop_channel_run.h>
#include <ags/audio/recall/ags_mute_audio.h>
#include <ags/audio/recall/ags_mute_audio_run.h>
#include <ags/audio/recall/ags_mute_audio_signal.h>
#include <ags/audio/recall/ags_mute_channel.h>
#include <ags/audio/recall/ags_mute_channel_run.h>
#include <ags/audio/recall/ags_mute_recycling.h>
#include <ags/audio/recall/ags_peak_audio_signal.h>
#include <ags/audio/recall/ags_peak_channel.h>
#include <ags/audio/recall/ags_peak_channel_run.h>
#include <ags/audio/recall/ags_peak_recycling.h>
#include <ags/audio/recall/ags_play_audio.h>
#include <ags/audio/recall/ags_play_audio_signal.h>
#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_channel_run.h>
#include <ags/audio/recall/ags_play_channel_run_master.h>
#include <ags/audio/recall/ags_play_notation_audio.h>
#include <ags/audio/recall/ags_play_notation_audio_run.h>
#include <ags/audio/recall/ags_play_wave_audio.h>
#include <ags/audio/recall/ags_play_wave_audio_run.h>
#include <ags/audio/recall/ags_play_wave_channel.h>
#include <ags/audio/recall/ags_play_wave_channel_run.h>
#include <ags/audio/recall/ags_play_dssi_audio.h>
#include <ags/audio/recall/ags_play_dssi_audio_run.h>
#include <ags/audio/recall/ags_play_lv2_audio.h>
#include <ags/audio/recall/ags_play_lv2_audio_run.h>
#include <ags/audio/recall/ags_play_recycling.h>
#include <ags/audio/recall/ags_prepare_audio_signal.h>
#include <ags/audio/recall/ags_prepare_channel.h>
#include <ags/audio/recall/ags_prepare_channel_run.h>
#include <ags/audio/recall/ags_prepare_recycling.h>
#include <ags/audio/recall/ags_record_midi_audio.h>
#include <ags/audio/recall/ags_record_midi_audio_run.h>
#include <ags/audio/recall/ags_route_dssi_audio.h>
#include <ags/audio/recall/ags_route_dssi_audio_run.h>
#include <ags/audio/recall/ags_route_lv2_audio.h>
#include <ags/audio/recall/ags_route_lv2_audio_run.h>
#include <ags/audio/recall/ags_stream_audio_signal.h>
#include <ags/audio/recall/ags_stream_channel.h>
#include <ags/audio/recall/ags_stream_channel_run.h>
#include <ags/audio/recall/ags_stream_recycling.h>
#include <ags/audio/recall/ags_volume_audio_signal.h>
#include <ags/audio/recall/ags_volume_channel.h>
#include <ags/audio/recall/ags_volume_channel_run.h>
#include <ags/audio/recall/ags_volume_recycling.h>

/* audio fx */
#include <ags/audio/fx/ags_fx_analyse_audio.h>
#include <ags/audio/fx/ags_fx_analyse_audio_processor.h>
#include <ags/audio/fx/ags_fx_analyse_audio_signal.h>
#include <ags/audio/fx/ags_fx_analyse_channel.h>
#include <ags/audio/fx/ags_fx_analyse_channel_processor.h>
#include <ags/audio/fx/ags_fx_analyse_recycling.h>
#include <ags/audio/fx/ags_fx_buffer_audio.h>
#include <ags/audio/fx/ags_fx_buffer_audio_processor.h>
#include <ags/audio/fx/ags_fx_buffer_audio_signal.h>
#include <ags/audio/fx/ags_fx_buffer_channel.h>
#include <ags/audio/fx/ags_fx_buffer_channel_processor.h>
#include <ags/audio/fx/ags_fx_buffer_recycling.h>
#include <ags/audio/fx/ags_fx_dssi_audio.h>
#include <ags/audio/fx/ags_fx_dssi_audio_processor.h>
#include <ags/audio/fx/ags_fx_dssi_audio_signal.h>
#include <ags/audio/fx/ags_fx_dssi_channel.h>
#include <ags/audio/fx/ags_fx_dssi_channel_processor.h>
#include <ags/audio/fx/ags_fx_dssi_recycling.h>
#include <ags/audio/fx/ags_fx_envelope_audio.h>
#include <ags/audio/fx/ags_fx_envelope_audio_processor.h>
#include <ags/audio/fx/ags_fx_envelope_audio_signal.h>
#include <ags/audio/fx/ags_fx_envelope_channel.h>
#include <ags/audio/fx/ags_fx_envelope_channel_processor.h>
#include <ags/audio/fx/ags_fx_envelope_recycling.h>
#include <ags/audio/fx/ags_fx_ladspa_audio.h>
#include <ags/audio/fx/ags_fx_ladspa_audio_processor.h>
#include <ags/audio/fx/ags_fx_ladspa_audio_signal.h>
#include <ags/audio/fx/ags_fx_ladspa_channel.h>
#include <ags/audio/fx/ags_fx_ladspa_channel_processor.h>
#include <ags/audio/fx/ags_fx_ladspa_recycling.h>
#include <ags/audio/fx/ags_fx_lv2_audio.h>
#include <ags/audio/fx/ags_fx_lv2_audio_processor.h>
#include <ags/audio/fx/ags_fx_lv2_audio_signal.h>
#include <ags/audio/fx/ags_fx_lv2_channel.h>
#include <ags/audio/fx/ags_fx_lv2_channel_processor.h>
#include <ags/audio/fx/ags_fx_lv2_recycling.h>
#include <ags/audio/fx/ags_fx_notation_audio.h>
#include <ags/audio/fx/ags_fx_notation_audio_processor.h>
#include <ags/audio/fx/ags_fx_notation_audio_signal.h>
#include <ags/audio/fx/ags_fx_notation_channel.h>
#include <ags/audio/fx/ags_fx_notation_channel_processor.h>
#include <ags/audio/fx/ags_fx_notation_recycling.h>
#include <ags/audio/fx/ags_fx_pattern_audio.h>
#include <ags/audio/fx/ags_fx_pattern_audio_processor.h>
#include <ags/audio/fx/ags_fx_pattern_audio_signal.h>
#include <ags/audio/fx/ags_fx_pattern_channel.h>
#include <ags/audio/fx/ags_fx_pattern_channel_processor.h>
#include <ags/audio/fx/ags_fx_pattern_recycling.h>
#include <ags/audio/fx/ags_fx_peak_audio.h>
#include <ags/audio/fx/ags_fx_peak_audio_processor.h>
#include <ags/audio/fx/ags_fx_peak_audio_signal.h>
#include <ags/audio/fx/ags_fx_peak_channel.h>
#include <ags/audio/fx/ags_fx_peak_channel_processor.h>
#include <ags/audio/fx/ags_fx_peak_recycling.h>
#include <ags/audio/fx/ags_fx_playback_audio.h>
#include <ags/audio/fx/ags_fx_playback_audio_processor.h>
#include <ags/audio/fx/ags_fx_playback_audio_signal.h>
#include <ags/audio/fx/ags_fx_playback_channel.h>
#include <ags/audio/fx/ags_fx_playback_channel_processor.h>
#include <ags/audio/fx/ags_fx_playback_recycling.h>
#include <ags/audio/fx/ags_fx_volume_audio.h>
#include <ags/audio/fx/ags_fx_volume_audio_processor.h>
#include <ags/audio/fx/ags_fx_volume_audio_signal.h>
#include <ags/audio/fx/ags_fx_volume_channel.h>
#include <ags/audio/fx/ags_fx_volume_channel_processor.h>
#include <ags/audio/fx/ags_fx_volume_recycling.h>
#include <ags/audio/fx/ags_fx_eq10_audio.h>
#include <ags/audio/fx/ags_fx_eq10_audio_processor.h>
#include <ags/audio/fx/ags_fx_eq10_audio_signal.h>
#include <ags/audio/fx/ags_fx_eq10_channel.h>
#include <ags/audio/fx/ags_fx_eq10_channel_processor.h>
#include <ags/audio/fx/ags_fx_eq10_recycling.h>

/* audio task */
#include <ags/audio/task/ags_add_audio.h>
#include <ags/audio/task/ags_add_audio_signal.h>
#include <ags/audio/task/ags_add_effect.h>
#include <ags/audio/task/ags_add_note.h>
#include <ags/audio/task/ags_add_soundcard.h>
#include <ags/audio/task/ags_apply_presets.h>
#include <ags/audio/task/ags_apply_sound_config.h>
#include <ags/audio/task/ags_apply_synth.h>
#include <ags/audio/task/ags_cancel_audio.h>
#include <ags/audio/task/ags_cancel_channel.h>
#include <ags/audio/task/ags_clear_audio_signal.h>
#include <ags/audio/task/ags_clear_buffer.h>
#include <ags/audio/task/ags_crop_note.h>
#include <ags/audio/task/ags_export_output.h>
#include <ags/audio/task/ags_free_selection.h>
#include <ags/audio/task/ags_link_channel.h>
#include <ags/audio/task/ags_move_note.h>
#include <ags/audio/task/ags_open_file.h>
#include <ags/audio/task/ags_open_sf2_instrument.h>
#include <ags/audio/task/ags_open_sf2_sample.h>
#include <ags/audio/task/ags_open_single_file.h>
#include <ags/audio/task/ags_open_wave.h>
#include <ags/audio/task/ags_start_audio.h>
#include <ags/audio/task/ags_start_channel.h>
#include <ags/audio/task/ags_remove_audio.h>
#include <ags/audio/task/ags_remove_audio_signal.h>
#include <ags/audio/task/ags_remove_note.h>
#include <ags/audio/task/ags_remove_soundcard.h>
#include <ags/audio/task/ags_resize_audio.h>
#include <ags/audio/task/ags_reset_amplitude.h>
#include <ags/audio/task/ags_reset_note.h>
#include <ags/audio/task/ags_reset_peak.h>
#include <ags/audio/task/ags_seek_soundcard.h>
#include <ags/audio/task/ags_set_audio_channels.h>
#include <ags/audio/task/ags_set_buffer_size.h>
#include <ags/audio/task/ags_set_device.h>
#include <ags/audio/task/ags_set_format.h>
#include <ags/audio/task/ags_set_samplerate.h>
#include <ags/audio/task/ags_start_sequencer.h>
#include <ags/audio/task/ags_start_soundcard.h>
#include <ags/audio/task/ags_stop_sequencer.h>
#include <ags/audio/task/ags_stop_soundcard.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>
#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_toggle_pattern_bit.h>

/* audio recall task */
#include <ags/audio/task/ags_apply_bpm.h>
#include <ags/audio/task/ags_apply_sequencer_length.h>
#include <ags/audio/task/ags_apply_tact.h>
#include <ags/audio/task/ags_set_muted.h>

#endif /*__LIBAGS_AUDIO_H__*/
