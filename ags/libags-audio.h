/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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
#include <ags/plugin/ags_plugin_factory.h>
#include <ags/plugin/ags_lv2_conversion.h>
#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_lv2_preset.h>
#include <ags/plugin/ags_lv2_event_manager.h>
#include <ags/plugin/ags_lv2_log_manager.h>
#include <ags/plugin/ags_lv2_option_manager.h>
#include <ags/plugin/ags_lv2_uri_map_manager.h>
#include <ags/plugin/ags_lv2_urid_manager.h>
#include <ags/plugin/ags_lv2_worker_manager.h>
#include <ags/plugin/ags_lv2_worker.h>
#include <ags/plugin/ags_lv2ui_manager.h>
#include <ags/plugin/ags_lv2ui_plugin.h>
#include <ags/plugin/ags_plugin_stock.h>

/* audio */
#include <ags/audio/ags_acceleration.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_audio_application_context.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_audio_connection.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_automation.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_channel_iter.h>
#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_effect.h>
#include <ags/audio/ags_fifoout.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_message.h>
#include <ags/audio/ags_midiin.h>
#include <ags/audio/ags_notation.h>
#include <ags/audio/ags_note.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_playable.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_preset.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_audio_signal.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run_dummy.h>
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
#include <ags/audio/ags_recall_recycling_dummy.h>
#include <ags/audio/ags_recall_recycling.h>
#include <ags/audio/ags_recycling_context.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_synth_generator.h>
#include <ags/audio/ags_synth_util.h>

/* audio thread */
#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>
#include <ags/audio/thread/ags_sequencer_thread.h>
#include <ags/audio/thread/ags_soundcard_thread.h>
#include <ags/audio/thread/ags_export_thread.h>
#include <ags/audio/thread/ags_iterator_thread.h>
#include <ags/audio/thread/ags_record_thread.h>
#include <ags/audio/thread/ags_recycling_thread_callbacks.h>
#include <ags/audio/thread/ags_recycling_thread.h>

/* audio file */
#include <ags/audio/file/ags_audio_file.h>
#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_audio_file_xml.h>
#include <ags/audio/file/ags_ipatch.h>
#include <ags/audio/file/ags_ipatch_dls2_reader.h>
#include <ags/audio/file/ags_ipatch_sf2_reader.h>
#include <ags/audio/file/ags_sndfile.h>

/* audio midi */
#include <ags/audio/midi/ags_midi_buffer_util.h>
#include <ags/audio/midi/ags_midi_builder.h>
#include <ags/audio/midi/ags_midi_file.h>
#include <ags/audio/midi/ags_midi_parser.h>
#include <ags/audio/midi/ags_midi_util.h>

/* audio core_audio */
#include <ags/audio/core-audio/ags_core_audio_client.h>
#include <ags/audio/core-audio/ags_core_audio_devout.h>
#include <ags/audio/core-audio/ags_core_audio_port.h>
#include <ags/audio/core-audio/ags_core_audio_server.h>

/* audio pulse */
#include <ags/audio/pulse/ags_pulse_client.h>
#include <ags/audio/pulse/ags_pulse_devout.h>
#include <ags/audio/pulse/ags_pulse_port.h>
#include <ags/audio/pulse/ags_pulse_server.h>

/* audio jack */
#include <ags/audio/jack/ags_jack_client.h>
#include <ags/audio/jack/ags_jack_midiin.h>
#include <ags/audio/jack/ags_jack_devout.h>
#include <ags/audio/jack/ags_jack_port.h>
#include <ags/audio/jack/ags_jack_server.h>

/* audio recall */
#include <ags/audio/recall/ags_buffer_audio_signal.h>
#include <ags/audio/recall/ags_buffer_channel.h>
#include <ags/audio/recall/ags_buffer_channel_run.h>
#include <ags/audio/recall/ags_buffer_recycling.h>
#include <ags/audio/recall/ags_copy_audio_signal.h>
#include <ags/audio/recall/ags_copy_channel.h>
#include <ags/audio/recall/ags_copy_channel_run.h>
#include <ags/audio/recall/ags_copy_notation_audio.h>
#include <ags/audio/recall/ags_copy_notation_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>
#include <ags/audio/recall/ags_copy_recycling.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_envelope_audio_signal.h>
#include <ags/audio/recall/ags_envelope_channel.h>
#include <ags/audio/recall/ags_envelope_channel_run.h>
#include <ags/audio/recall/ags_envelope_recycling.h>
#include <ags/audio/recall/ags_feed_audio_signal.h>
#include <ags/audio/recall/ags_feed_channel.h>
#include <ags/audio/recall/ags_feed_channel_run.h>
#include <ags/audio/recall/ags_feed_recycling.h>
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
#include <ags/audio/recall/ags_play_audio_file.h>
#include <ags/audio/recall/ags_play_audio.h>
#include <ags/audio/recall/ags_play_audio_signal.h>
#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_channel_run.h>
#include <ags/audio/recall/ags_play_channel_run_master.h>
#include <ags/audio/recall/ags_play_notation_audio.h>
#include <ags/audio/recall/ags_play_notation_audio_run.h>
#include <ags/audio/recall/ags_play_note.h>
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

/* audio task */
#include <ags/audio/task/ags_add_audio.h>
#include <ags/audio/task/ags_add_audio_signal.h>
#include <ags/audio/task/ags_add_effect.h>
#include <ags/audio/task/ags_add_note.h>
#include <ags/audio/task/ags_add_point_to_selection.h>
#include <ags/audio/task/ags_add_recall_container.h>
#include <ags/audio/task/ags_add_recall.h>
#include <ags/audio/task/ags_add_region_to_selection.h>
#include <ags/audio/task/ags_add_soundcard.h>
#include <ags/audio/task/ags_append_audio.h>
#include <ags/audio/task/ags_append_audio_threaded.h>
#include <ags/audio/task/ags_append_channel.h>
#include <ags/audio/task/ags_append_recall.h>
#include <ags/audio/task/ags_apply_presets.h>
#include <ags/audio/task/ags_apply_synth.h>
#include <ags/audio/task/ags_cancel_audio.h>
#include <ags/audio/task/ags_cancel_channel.h>
#include <ags/audio/task/ags_cancel_recall.h>
#include <ags/audio/task/ags_change_soundcard.h>
#include <ags/audio/task/ags_export_output.h>
#include <ags/audio/task/ags_free_selection.h>
#include <ags/audio/task/ags_init_audio.h>
#include <ags/audio/task/ags_init_channel.h>
#include <ags/audio/task/ags_link_channel.h>
#include <ags/audio/task/ags_notify_soundcard.h>
#include <ags/audio/task/ags_open_file.h>
#include <ags/audio/task/ags_open_sf2_sample.h>
#include <ags/audio/task/ags_open_single_file.h>
#include <ags/audio/task/ags_remove_audio.h>
#include <ags/audio/task/ags_remove_audio_signal.h>
#include <ags/audio/task/ags_remove_note.h>
#include <ags/audio/task/ags_remove_point_from_selection.h>
#include <ags/audio/task/ags_remove_recall_container.h>
#include <ags/audio/task/ags_remove_recall.h>
#include <ags/audio/task/ags_remove_region_from_selection.h>
#include <ags/audio/task/ags_remove_soundcard.h>
#include <ags/audio/task/ags_reset_audio_connection.h>
#include <ags/audio/task/ags_resize_audio.h>
#include <ags/audio/task/ags_save_file.h>
#include <ags/audio/task/ags_seek_soundcard.h>
#include <ags/audio/task/ags_set_audio_channels.h>
#include <ags/audio/task/ags_set_buffer_size.h>
#include <ags/audio/task/ags_set_format.h>
#include <ags/audio/task/ags_set_input_device.h>
#include <ags/audio/task/ags_set_output_device.h>
#include <ags/audio/task/ags_set_samplerate.h>
#include <ags/audio/task/ags_start_sequencer.h>
#include <ags/audio/task/ags_start_soundcard.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>
#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_toggle_pattern_bit.h>
#include <ags/audio/task/ags_unref_audio_signal.h>

/* audio recall task */
#include <ags/audio/task/recall/ags_apply_bpm.h>
#include <ags/audio/task/recall/ags_apply_sequencer_length.h>
#include <ags/audio/task/recall/ags_apply_tact.h>
#include <ags/audio/task/recall/ags_set_muted.h>

#endif /*__LIBAGS_AUDIO_H__*/
