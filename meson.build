# Copyright (C) 2020 Daniel Maksymow, Joël Krähemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

project('gsequencer', 'c',
        license: 'GPL3',
        default_options: ['c_std=gnu99'],
        version: '3.6.23')

gsequencer_prefix = get_option('prefix')
gsequencer_includedir = join_paths(gsequencer_prefix, get_option('includedir'))
gsequencer_libdir = join_paths(gsequencer_prefix, get_option('libdir'))
gsequencer_datadir = join_paths(gsequencer_prefix, get_option('datadir'))
gsequencer_localedir = join_paths(gsequencer_prefix, get_option('localedir'))
gsequencer_applicationsdir = join_paths(gsequencer_datadir, 'applications')
gsequencer_appdatadir = join_paths(gsequencer_datadir, 'metainfo')
gsequencer_docdir = join_paths(gsequencer_prefix, get_option('datadir'), 'doc', 'gsequencer')

gnome = import('gnome')

compiler = meson.get_compiler('c')
# TODO: all dependencies have to be defined here globally
gtk_dependency = dependency('gtk+-3.0')
glib_dependency = dependency('glib-2.0', version: '>= 2.58.0')
gobject_dependency = dependency('gobject-2.0', version: '>= 2.58.0')
gthread_dependency = dependency('gthread-2.0', version: '>= 2.58.0')
gio_dependency = dependency('gio-2.0', version: '>= 2.58.0')
cunit_dependency = dependency('cunit')
xml2_dependency = dependency('libxml-2.0')
soup_dependency = dependency('libsoup-2.4')
instpatch_dependency = dependency('libinstpatch-1.0')
samplerate_dependency = dependency('samplerate')
fftw3_dependency = dependency('fftw3')
if get_option('alsa')
  alsa_dependency = dependency('alsa')
endif
if get_option('jack')
  jack_dependency = dependency('jack')
endif
if get_option('pulse')
  pulse_dependency = dependency('libpulse-mainloop-glib')
endif
if get_option('poppler')
  poppler_dependency = dependency('poppler-glib')
endif
if get_option('webkit')
  webkit_dependency = dependency('webkit2gtk-4.0')
endif
if get_option('gstreamer')
  gstreamer_dependency = dependency('gstreamer-1.0')
  gstreamer_audio_dependency = dependency('gstreamer-audio-1.0')
  gstreamer_video_dependency = dependency('gstreamer-video-1.0')
  gstreamer_app_dependency = dependency('gstreamer-app-1.0')
  gstreamer_pbutils_dependency = dependency('gstreamer-pbutils-1.0')
endif
m_dependency = compiler.find_library('m', required: true)
dl_dependency = compiler.find_library('dl', required: true)
crypt_dependency =   compiler.find_library('crypt', required: true)

#destdir = get_option('DESTDIR')
add_project_arguments('-DDESTDIR="@0@"'.format(get_option('prefix')),
                      language:'c')

includes = include_directories(
  '.'
  )

libags_util_headers = [
  'ags/util/ags_id_generator.h',
  'ags/util/ags_destroy_util.h',
  'ags/util/ags_soundcard_helper.h',
  'ags/util/ags_list_util.h',
]

libags_lib_headers = [
  'ags/lib/ags_function.h',
  'ags/lib/ags_complex.h',
  'ags/lib/ags_solver_vector.h',
  'ags/lib/ags_math_util.h',
  'ags/lib/ags_string_util.h',
  'ags/lib/ags_endian.h',
  'ags/lib/ags_log.h',
  'ags/lib/ags_turtle_manager.h',
  'ags/lib/ags_buffer_util.h',
  'ags/lib/ags_time.h',
  'ags/lib/ags_turtle.h',
  'ags/lib/ags_uuid.h',
  'ags/lib/ags_solver_polynomial.h',
  'ags/lib/ags_regex.h',
  'ags/lib/ags_conversion.h',
  'ags/lib/ags_solver_matrix.h',
]

libags_object_headers = [
  'ags/object/ags_connectable.h',
  'ags/object/ags_soundcard.h',
  'ags/object/ags_applicable.h',
  'ags/object/ags_marshal.h',
  'ags/object/ags_sequencer.h',
  'ags/object/ags_tactable.h',
  'ags/object/ags_main_loop.h',
  'ags/object/ags_seekable.h',
  'ags/object/ags_cursor.h',
  'ags/object/ags_sound_server.h',
  'ags/object/ags_countable.h',
  'ags/object/ags_portlet.h',
  'ags/object/ags_globals.h',
  'ags/object/ags_application_context.h',
  'ags/object/ags_priority.h',
  'ags/object/ags_macros.h',
  'ags/object/ags_mutable.h',
  'ags/object/ags_plugin.h',
  'ags/object/ags_config.h',
]

libags_file_headers = [
  'ags/file/ags_file.h',
  'ags/file/ags_file_launch.h',
  'ags/file/ags_file_lookup.h',
  'ags/file/ags_file_id_ref.h',
  'ags/file/ags_file_link.h',
]

libagsthread_headers = [
  'ags/thread/ags_concurrency_provider.h',
  'ags/thread/ags_returnable_thread.h',
  'ags/thread/ags_message_envelope.h',
  'ags/thread/ags_generic_main_loop.h',
  'ags/thread/ags_timestamp.h',
  'ags/thread/ags_task.h',
  'ags/thread/ags_message_delivery.h',
  'ags/thread/ags_thread_pool.h',
  'ags/thread/ags_thread_application_context.h',
  'ags/thread/ags_thread.h',
  'ags/thread/ags_worker_thread.h',
  'ags/thread/ags_task_launcher.h',
  'ags/thread/ags_destroy_worker.h',
  'ags/thread/ags_message_queue.h',
  'ags/thread/ags_task_completion.h',
]

libagsserver_headers = [
  'ags/server/ags_registry.h',
  'ags/server/ags_server.h',
  'ags/server/ags_server_application_context.h',
  'ags/server/ags_server_status.h',
  'ags/server/ags_service_provider.h',
]

libagsserver_security_headers = [
  'ags/server/security/ags_auth_security_context.h',
  'ags/server/security/ags_authentication.h',
  'ags/server/security/ags_authentication_manager.h',
  'ags/server/security/ags_business_group.h',
  'ags/server/security/ags_business_group_manager.h',
  'ags/server/security/ags_certificate.h',
  'ags/server/security/ags_certificate_manager.h',
  'ags/server/security/ags_password_store.h',
  'ags/server/security/ags_password_store_manager.h',
  'ags/server/security/ags_security_context.h',
  'ags/server/security/ags_xml_authentication.h',
  'ags/server/security/ags_xml_business_group.h',
  'ags/server/security/ags_xml_certificate.h',
  'ags/server/security/ags_xml_password_store.h',
]

libagsserver_controller_headers = [
  'ags/server/controller/ags_controller.h',
  'ags/server/controller/ags_front_controller.h',
  'ags/server/controller/ags_plugin_controller.h',
]

libagsaudio_plugin_headers = [
  'ags/plugin/ags_base_plugin.h',
  'ags/plugin/ags_dssi_manager.h',
  'ags/plugin/ags_dssi_plugin.h',
  'ags/plugin/ags_ladspa_conversion.h',
  'ags/plugin/ags_ladspa_manager.h',
  'ags/plugin/ags_ladspa_plugin.h',
  'ags/plugin/ags_lv2_conversion.h',
  'ags/plugin/ags_lv2_event_manager.h',
  'ags/plugin/ags_lv2_log_manager.h',
  'ags/plugin/ags_lv2_manager.h',
  'ags/plugin/ags_lv2_option_manager.h',
  'ags/plugin/ags_lv2_plugin.h',
  'ags/plugin/ags_lv2_preset.h',
  'ags/plugin/ags_lv2_preset_manager.h',
  'ags/plugin/ags_lv2_turtle_parser.h',
  'ags/plugin/ags_lv2_turtle_scanner.h',
  'ags/plugin/ags_lv2_uri_map_manager.h',
  'ags/plugin/ags_lv2_urid_manager.h',
  'ags/plugin/ags_lv2_worker.h',
  'ags/plugin/ags_lv2_worker_manager.h',
  'ags/plugin/ags_lv2ui_manager.h',
  'ags/plugin/ags_lv2ui_plugin.h',
  'ags/plugin/ags_plugin_port.h',
  'ags/plugin/ags_plugin_stock.h',
]

libagsaudio_audio_headers = [
  'ags/audio/ags_acceleration.h',
  'ags/audio/ags_audio.h',
  'ags/audio/ags_audio_application_context.h',
  'ags/audio/ags_audio_buffer_util.h',
  'ags/audio/ags_audio_signal.h',
  'ags/audio/ags_automation.h',
  'ags/audio/ags_buffer.h',
  'ags/audio/ags_channel.h',
  'ags/audio/ags_char_buffer_util.h',
  'ags/audio/ags_devin.h',
  'ags/audio/ags_devout.h',
  'ags/audio/ags_diatonic_scale.h',
  'ags/audio/ags_fast_pitch_util.h',
  'ags/audio/ags_fifoout.h',
  'ags/audio/ags_filter_util.h',
  'ags/audio/ags_fm_synth_util.h',
  'ags/audio/ags_fourier_transform_util.h',
  'ags/audio/ags_frequency_map.h',
  'ags/audio/ags_frequency_map_manager.h',
  'ags/audio/ags_fx_factory.h',
  'ags/audio/ags_generic_recall_channel_run.h',
  'ags/audio/ags_generic_recall_recycling.h',
  'ags/audio/ags_input.h',
  'ags/audio/ags_lfo_synth_util.h',
  'ags/audio/ags_midi.h',
  'ags/audio/ags_midiin.h',
  'ags/audio/ags_notation.h',
  'ags/audio/ags_note.h',
  'ags/audio/ags_output.h',
  'ags/audio/ags_pattern.h',
  'ags/audio/ags_playback.h',
  'ags/audio/ags_playback_domain.h',
  'ags/audio/ags_port.h',
  'ags/audio/ags_port_util.h',
  'ags/audio/ags_preset.h',
  'ags/audio/ags_recall.h',
  'ags/audio/ags_recall_audio.h',
  'ags/audio/ags_recall_audio_run.h',
  'ags/audio/ags_recall_audio_signal.h',
  'ags/audio/ags_recall_channel.h',
  'ags/audio/ags_recall_channel_run.h',
  'ags/audio/ags_recall_container.h',
  'ags/audio/ags_recall_dependency.h',
  'ags/audio/ags_recall_dssi.h',
  'ags/audio/ags_recall_dssi_run.h',
  'ags/audio/ags_recall_factory.h',
  'ags/audio/ags_recall_id.h',
  'ags/audio/ags_recall_ladspa.h',
  'ags/audio/ags_recall_ladspa_run.h',
  'ags/audio/ags_recall_lv2.h',
  'ags/audio/ags_recall_lv2_run.h',
  'ags/audio/ags_recall_recycling.h',
  'ags/audio/ags_recycling.h',
  'ags/audio/ags_recycling_context.h',
  'ags/audio/ags_sequencer_util.h',
  'ags/audio/ags_sf2_synth_generator.h',
  'ags/audio/ags_sf2_synth_util.h',
  'ags/audio/ags_sfz_synth_generator.h',
  'ags/audio/ags_sfz_synth_util.h',
  'ags/audio/ags_sound_enums.h',
  'ags/audio/ags_sound_provider.h',
  'ags/audio/ags_soundcard_util.h',
  'ags/audio/ags_synth_enums.h',
  'ags/audio/ags_synth_generator.h',
  'ags/audio/ags_synth_util.h',
  'ags/audio/ags_track.h',
  'ags/audio/ags_wave.h',
]

libagsaudio_audio_audio_unit_headers = [
  'ags/audio/audio-unit/ags_audio_unit_client.h',
  'ags/audio/audio-unit/ags_audio_unit_devin.h',
  'ags/audio/audio-unit/ags_audio_unit_devout.h',
  'ags/audio/audio-unit/ags_audio_unit_port.h',
  'ags/audio/audio-unit/ags_audio_unit_server.h',
]

libagsaudio_audio_core_audio_headers = [
  'ags/audio/core-audio/ags_core_audio_client.h',
  'ags/audio/core-audio/ags_core_audio_devin.h',
  'ags/audio/core-audio/ags_core_audio_devout.h',
  'ags/audio/core-audio/ags_core_audio_midiin.h',
  'ags/audio/core-audio/ags_core_audio_port.h',
  'ags/audio/core-audio/ags_core_audio_server.h',
]

libagsaudio_audio_file_headers = [
  'ags/audio/file/ags_audio_container.h',
  'ags/audio/file/ags_audio_container_manager.h',
  'ags/audio/file/ags_audio_file.h',
  'ags/audio/file/ags_audio_file_link.h',
  'ags/audio/file/ags_audio_file_manager.h',
  'ags/audio/file/ags_ipatch.h',
  'ags/audio/file/ags_ipatch_dls2_reader.h',
  'ags/audio/file/ags_ipatch_gig_reader.h',
  'ags/audio/file/ags_ipatch_sample.h',
  'ags/audio/file/ags_ipatch_sf2_reader.h',
  'ags/audio/file/ags_sfz_file.h',
  'ags/audio/file/ags_sfz_group.h',
  'ags/audio/file/ags_sfz_region.h',
  'ags/audio/file/ags_sfz_sample.h',
  'ags/audio/file/ags_sndfile.h',
  'ags/audio/file/ags_sound_container.h',
  'ags/audio/file/ags_sound_resource.h',
]

if get_option('gstreamer')
libagsaudio_audio_file_gstreamer_headers = [
  'ags/audio/file/ags_gstreamer_file.h',
]
endif

libagsaudio_audio_fx_headers = [
  'ags/audio/fx/ags_fx_analyse_audio.h',
  'ags/audio/fx/ags_fx_analyse_audio_processor.h',
  'ags/audio/fx/ags_fx_analyse_audio_signal.h',
  'ags/audio/fx/ags_fx_analyse_channel.h',
  'ags/audio/fx/ags_fx_analyse_channel_processor.h',
  'ags/audio/fx/ags_fx_analyse_recycling.h',
  'ags/audio/fx/ags_fx_buffer_audio.h',
  'ags/audio/fx/ags_fx_buffer_audio_processor.h',
  'ags/audio/fx/ags_fx_buffer_audio_signal.h',
  'ags/audio/fx/ags_fx_buffer_channel.h',
  'ags/audio/fx/ags_fx_buffer_channel_processor.h',
  'ags/audio/fx/ags_fx_buffer_recycling.h',
  'ags/audio/fx/ags_fx_dssi_audio.h',
  'ags/audio/fx/ags_fx_dssi_audio_processor.h',
  'ags/audio/fx/ags_fx_dssi_audio_signal.h',
  'ags/audio/fx/ags_fx_dssi_channel.h',
  'ags/audio/fx/ags_fx_dssi_channel_processor.h',
  'ags/audio/fx/ags_fx_dssi_recycling.h',
  'ags/audio/fx/ags_fx_envelope_audio.h',
  'ags/audio/fx/ags_fx_envelope_audio_processor.h',
  'ags/audio/fx/ags_fx_envelope_audio_signal.h',
  'ags/audio/fx/ags_fx_envelope_channel.h',
  'ags/audio/fx/ags_fx_envelope_channel_processor.h',
  'ags/audio/fx/ags_fx_envelope_recycling.h',
  'ags/audio/fx/ags_fx_eq10_audio.h',
  'ags/audio/fx/ags_fx_eq10_audio_processor.h',
  'ags/audio/fx/ags_fx_eq10_audio_signal.h',
  'ags/audio/fx/ags_fx_eq10_channel.h',
  'ags/audio/fx/ags_fx_eq10_channel_processor.h',
  'ags/audio/fx/ags_fx_eq10_recycling.h',
  'ags/audio/fx/ags_fx_ladspa_audio.h',
  'ags/audio/fx/ags_fx_ladspa_audio_processor.h',
  'ags/audio/fx/ags_fx_ladspa_audio_signal.h',
  'ags/audio/fx/ags_fx_ladspa_channel.h',
  'ags/audio/fx/ags_fx_ladspa_channel_processor.h',
  'ags/audio/fx/ags_fx_ladspa_recycling.h',
  'ags/audio/fx/ags_fx_lv2_audio.h',
  'ags/audio/fx/ags_fx_lv2_audio_processor.h',
  'ags/audio/fx/ags_fx_lv2_audio_signal.h',
  'ags/audio/fx/ags_fx_lv2_channel.h',
  'ags/audio/fx/ags_fx_lv2_channel_processor.h',
  'ags/audio/fx/ags_fx_lv2_recycling.h',
  'ags/audio/fx/ags_fx_notation_audio.h',
  'ags/audio/fx/ags_fx_notation_audio_processor.h',
  'ags/audio/fx/ags_fx_notation_audio_signal.h',
  'ags/audio/fx/ags_fx_notation_channel.h',
  'ags/audio/fx/ags_fx_notation_channel_processor.h',
  'ags/audio/fx/ags_fx_notation_recycling.h',
  'ags/audio/fx/ags_fx_pattern_audio.h',
  'ags/audio/fx/ags_fx_pattern_audio_processor.h',
  'ags/audio/fx/ags_fx_pattern_audio_signal.h',
  'ags/audio/fx/ags_fx_pattern_channel.h',
  'ags/audio/fx/ags_fx_pattern_channel_processor.h',
  'ags/audio/fx/ags_fx_pattern_recycling.h',
  'ags/audio/fx/ags_fx_peak_audio.h',
  'ags/audio/fx/ags_fx_peak_audio_processor.h',
  'ags/audio/fx/ags_fx_peak_audio_signal.h',
  'ags/audio/fx/ags_fx_peak_channel.h',
  'ags/audio/fx/ags_fx_peak_channel_processor.h',
  'ags/audio/fx/ags_fx_peak_recycling.h',
  'ags/audio/fx/ags_fx_playback_audio.h',
  'ags/audio/fx/ags_fx_playback_audio_processor.h',
  'ags/audio/fx/ags_fx_playback_audio_signal.h',
  'ags/audio/fx/ags_fx_playback_channel.h',
  'ags/audio/fx/ags_fx_playback_channel_processor.h',
  'ags/audio/fx/ags_fx_playback_recycling.h',
  'ags/audio/fx/ags_fx_volume_audio.h',
  'ags/audio/fx/ags_fx_volume_audio_processor.h',
  'ags/audio/fx/ags_fx_volume_audio_signal.h',
  'ags/audio/fx/ags_fx_volume_channel.h',
  'ags/audio/fx/ags_fx_volume_channel_processor.h',
  'ags/audio/fx/ags_fx_volume_recycling.h',
]

if get_option('gstreamer')
libagsaudio_audio_gstreamer_headers = [
  'ags/audio/gstreamer/ags_gstreamer_client.h',
  'ags/audio/gstreamer/ags_gstreamer_devin.h',
  'ags/audio/gstreamer/ags_gstreamer_devout.h',
  'ags/audio/gstreamer/ags_gstreamer_port.h',
  'ags/audio/gstreamer/ags_gstreamer_server.h',
]
endif

libagsaudio_audio_jack_headers = [
  'ags/audio/jack/ags_jack_client.h',
  'ags/audio/jack/ags_jack_devin.h',
  'ags/audio/jack/ags_jack_devout.h',
  'ags/audio/jack/ags_jack_midiin.h',
  'ags/audio/jack/ags_jack_port.h',
  'ags/audio/jack/ags_jack_server.h',
]

libagsaudio_audio_midi_headers = [
  'ags/audio/midi/ags_midi_buffer_util.h',
  'ags/audio/midi/ags_midi_builder.h',
  'ags/audio/midi/ags_midi_file.h',
  'ags/audio/midi/ags_midi_parser.h',
  'ags/audio/midi/ags_midi_util.h',
]

libagsaudio_audio_osc_headers = [
  'ags/audio/osc/ags_osc_buffer_util.h',
  'ags/audio/osc/ags_osc_builder.h',
  'ags/audio/osc/ags_osc_client.h',
  'ags/audio/osc/ags_osc_connection.h',
  'ags/audio/osc/ags_osc_message.h',
  'ags/audio/osc/ags_osc_parser.h',
  'ags/audio/osc/ags_osc_response.h',
  'ags/audio/osc/ags_osc_server.h',
  'ags/audio/osc/ags_osc_util.h',
  'ags/audio/osc/ags_osc_websocket_connection.h',
  'ags/audio/osc/ags_osc_xmlrpc_message.h',
  'ags/audio/osc/ags_osc_xmlrpc_server.h',
]

libagsaudio_audio_osc_controller_headers = [
  'ags/audio/osc/controller/ags_osc_action_controller.h',
  'ags/audio/osc/controller/ags_osc_config_controller.h',
  'ags/audio/osc/controller/ags_osc_controller.h',
  'ags/audio/osc/controller/ags_osc_export_controller.h',
  'ags/audio/osc/controller/ags_osc_front_controller.h',
  'ags/audio/osc/controller/ags_osc_info_controller.h',
  'ags/audio/osc/controller/ags_osc_meter_controller.h',
  'ags/audio/osc/controller/ags_osc_node_controller.h',
  'ags/audio/osc/controller/ags_osc_plugin_controller.h',
  'ags/audio/osc/controller/ags_osc_renew_controller.h',
  'ags/audio/osc/controller/ags_osc_status_controller.h',
]

libagsaudio_audio_osc_xmlrpc_headers = [
  'ags/audio/osc/xmlrpc/ags_osc_xmlrpc_controller.h',
]

libagsaudio_audio_pulse_headers = [
  'ags/audio/pulse/ags_pulse_client.h',
  'ags/audio/pulse/ags_pulse_devin.h',
  'ags/audio/pulse/ags_pulse_devout.h',
  'ags/audio/pulse/ags_pulse_port.h',
  'ags/audio/pulse/ags_pulse_server.h',
]

libagsaudio_audio_recall_headers = [
  'ags/audio/recall/ags_analyse_audio_signal.h',
  'ags/audio/recall/ags_analyse_channel.h',
  'ags/audio/recall/ags_analyse_channel_run.h',
  'ags/audio/recall/ags_analyse_recycling.h',
  'ags/audio/recall/ags_buffer_audio_signal.h',
  'ags/audio/recall/ags_buffer_channel.h',
  'ags/audio/recall/ags_buffer_channel_run.h',
  'ags/audio/recall/ags_buffer_recycling.h',
  'ags/audio/recall/ags_capture_wave_audio.h',
  'ags/audio/recall/ags_capture_wave_audio_run.h',
  'ags/audio/recall/ags_capture_wave_channel.h',
  'ags/audio/recall/ags_capture_wave_channel_run.h',
  'ags/audio/recall/ags_copy_audio_signal.h',
  'ags/audio/recall/ags_copy_channel.h',
  'ags/audio/recall/ags_copy_channel_run.h',
  'ags/audio/recall/ags_copy_pattern_audio.h',
  'ags/audio/recall/ags_copy_pattern_audio_run.h',
  'ags/audio/recall/ags_copy_pattern_channel.h',
  'ags/audio/recall/ags_copy_pattern_channel_run.h',
  'ags/audio/recall/ags_copy_recycling.h',
  'ags/audio/recall/ags_count_beats_audio.h',
  'ags/audio/recall/ags_count_beats_audio_run.h',
  'ags/audio/recall/ags_delay_audio.h',
  'ags/audio/recall/ags_delay_audio_run.h',
  'ags/audio/recall/ags_envelope_audio_signal.h',
  'ags/audio/recall/ags_envelope_channel.h',
  'ags/audio/recall/ags_envelope_channel_run.h',
  'ags/audio/recall/ags_envelope_recycling.h',
  'ags/audio/recall/ags_eq10_audio_signal.h',
  'ags/audio/recall/ags_eq10_channel.h',
  'ags/audio/recall/ags_eq10_channel_run.h',
  'ags/audio/recall/ags_eq10_recycling.h',
  'ags/audio/recall/ags_feed_audio_signal.h',
  'ags/audio/recall/ags_feed_channel.h',
  'ags/audio/recall/ags_feed_channel_run.h',
  'ags/audio/recall/ags_feed_recycling.h',
  'ags/audio/recall/ags_lfo_audio_signal.h',
  'ags/audio/recall/ags_lfo_channel.h',
  'ags/audio/recall/ags_lfo_channel_run.h',
  'ags/audio/recall/ags_lfo_recycling.h',
  'ags/audio/recall/ags_loop_channel.h',
  'ags/audio/recall/ags_loop_channel_run.h',
  'ags/audio/recall/ags_mute_audio.h',
  'ags/audio/recall/ags_mute_audio_run.h',
  'ags/audio/recall/ags_mute_audio_signal.h',
  'ags/audio/recall/ags_mute_channel.h',
  'ags/audio/recall/ags_mute_channel_run.h',
  'ags/audio/recall/ags_mute_recycling.h',
  'ags/audio/recall/ags_peak_audio_signal.h',
  'ags/audio/recall/ags_peak_channel.h',
  'ags/audio/recall/ags_peak_channel_run.h',
  'ags/audio/recall/ags_peak_recycling.h',
  'ags/audio/recall/ags_play_audio.h',
  'ags/audio/recall/ags_play_audio_signal.h',
  'ags/audio/recall/ags_play_channel.h',
  'ags/audio/recall/ags_play_channel_run.h',
  'ags/audio/recall/ags_play_channel_run_master.h',
  'ags/audio/recall/ags_play_dssi_audio.h',
  'ags/audio/recall/ags_play_dssi_audio_run.h',
  'ags/audio/recall/ags_play_lv2_audio.h',
  'ags/audio/recall/ags_play_lv2_audio_run.h',
  'ags/audio/recall/ags_play_notation_audio.h',
  'ags/audio/recall/ags_play_notation_audio_run.h',
  'ags/audio/recall/ags_play_recycling.h',
  'ags/audio/recall/ags_play_wave_audio.h',
  'ags/audio/recall/ags_play_wave_audio_run.h',
  'ags/audio/recall/ags_play_wave_channel.h',
  'ags/audio/recall/ags_play_wave_channel_run.h',
  'ags/audio/recall/ags_prepare_audio_signal.h',
  'ags/audio/recall/ags_prepare_channel.h',
  'ags/audio/recall/ags_prepare_channel_run.h',
  'ags/audio/recall/ags_prepare_recycling.h',
  'ags/audio/recall/ags_record_midi_audio.h',
  'ags/audio/recall/ags_record_midi_audio_run.h',
  'ags/audio/recall/ags_route_dssi_audio.h',
  'ags/audio/recall/ags_route_dssi_audio_run.h',
  'ags/audio/recall/ags_route_lv2_audio.h',
  'ags/audio/recall/ags_route_lv2_audio_run.h',
  'ags/audio/recall/ags_rt_stream_audio_signal.h',
  'ags/audio/recall/ags_rt_stream_channel.h',
  'ags/audio/recall/ags_rt_stream_channel_run.h',
  'ags/audio/recall/ags_rt_stream_recycling.h',
  'ags/audio/recall/ags_stream_audio_signal.h',
  'ags/audio/recall/ags_stream_channel.h',
  'ags/audio/recall/ags_stream_channel_run.h',
  'ags/audio/recall/ags_stream_recycling.h',
  'ags/audio/recall/ags_volume_audio_signal.h',
  'ags/audio/recall/ags_volume_channel.h',
  'ags/audio/recall/ags_volume_channel_run.h',
  'ags/audio/recall/ags_volume_recycling.h',
]

libagsaudio_audio_task_headers = [
  'ags/audio/task/ags_add_audio.h',
  'ags/audio/task/ags_add_audio_signal.h',
  'ags/audio/task/ags_add_effect.h',
  'ags/audio/task/ags_add_note.h',
  'ags/audio/task/ags_add_soundcard.h',
  'ags/audio/task/ags_apply_bpm.h',
  'ags/audio/task/ags_apply_presets.h',
  'ags/audio/task/ags_apply_sequencer_length.h',
  'ags/audio/task/ags_apply_sf2_synth.h',
  'ags/audio/task/ags_apply_sfz_synth.h',
  'ags/audio/task/ags_apply_sound_config.h',
  'ags/audio/task/ags_apply_synth.h',
  'ags/audio/task/ags_apply_tact.h',
  'ags/audio/task/ags_cancel_audio.h',
  'ags/audio/task/ags_cancel_channel.h',
  'ags/audio/task/ags_clear_audio_signal.h',
  'ags/audio/task/ags_clear_buffer.h',
  'ags/audio/task/ags_crop_note.h',
  'ags/audio/task/ags_export_output.h',
  'ags/audio/task/ags_free_selection.h',
  'ags/audio/task/ags_link_channel.h',
  'ags/audio/task/ags_move_note.h',
  'ags/audio/task/ags_open_file.h',
  'ags/audio/task/ags_open_sf2_instrument.h',
  'ags/audio/task/ags_open_sf2_sample.h',
  'ags/audio/task/ags_open_sfz_file.h',
  'ags/audio/task/ags_open_single_file.h',
  'ags/audio/task/ags_open_wave.h',
  'ags/audio/task/ags_remove_audio.h',
  'ags/audio/task/ags_remove_audio_signal.h',
  'ags/audio/task/ags_remove_note.h',
  'ags/audio/task/ags_remove_soundcard.h',
  'ags/audio/task/ags_reset_amplitude.h',
  'ags/audio/task/ags_reset_fx_analyse.h',
  'ags/audio/task/ags_reset_fx_peak.h',
  'ags/audio/task/ags_reset_note.h',
  'ags/audio/task/ags_reset_peak.h',
  'ags/audio/task/ags_resize_audio.h',
  'ags/audio/task/ags_seek_soundcard.h',
  'ags/audio/task/ags_set_audio_channels.h',
  'ags/audio/task/ags_set_buffer_size.h',
  'ags/audio/task/ags_set_device.h',
  'ags/audio/task/ags_set_format.h',
  'ags/audio/task/ags_set_muted.h',
  'ags/audio/task/ags_set_samplerate.h',
  'ags/audio/task/ags_start_audio.h',
  'ags/audio/task/ags_start_channel.h',
  'ags/audio/task/ags_start_sequencer.h',
  'ags/audio/task/ags_start_soundcard.h',
  'ags/audio/task/ags_stop_sequencer.h',
  'ags/audio/task/ags_stop_soundcard.h',
  'ags/audio/task/ags_switch_buffer_flag.h',
  'ags/audio/task/ags_tic_device.h',
  'ags/audio/task/ags_toggle_pattern_bit.h',
]

libagsaudio_audio_thread_headers = [
  'ags/audio/thread/ags_audio_loop.h',
  'ags/audio/thread/ags_audio_thread.h',
  'ags/audio/thread/ags_channel_thread.h',
  'ags/audio/thread/ags_export_thread.h',
  'ags/audio/thread/ags_sequencer_thread.h',
  'ags/audio/thread/ags_sf2_loader.h',
  'ags/audio/thread/ags_sfz_loader.h',
  'ags/audio/thread/ags_soundcard_thread.h',
  'ags/audio/thread/ags_wave_loader.h',
]

libagsaudio_audio_wasapi_headers = [
  'ags/audio/wasapi/ags_wasapi_devin.h',
  'ags/audio/wasapi/ags_wasapi_devout.h',
]

libagsgui_headers = [
  'ags/widget/ags_dial.h',
  'ags/widget/ags_vlevel_box.h',
  'ags/widget/ags_level_box.h',
  'ags/widget/ags_hlevel_box.h',
  'ags/widget/ags_scale_box.h',
  'ags/widget/ags_indicator.h',
  'ags/widget/ags_vled_array.h',
  'ags/widget/ags_hindicator.h',
  'ags/widget/ags_level.h',
  'ags/widget/ags_ruler.h',
  'ags/widget/ags_hscale_box.h',
  'ags/widget/ags_vscale_box.h',
  'ags/widget/ags_scrolled_level_box.h',
  'ags/widget/ags_widget_marshal.h',
  'ags/widget/ags_piano_keys.h',
  'ags/widget/ags_scale.h',
  'ags/widget/ags_scrolled_scale_box.h',
  'ags/widget/ags_expander_set.h',
  'ags/widget/ags_cartesian.h',
  'ags/widget/ags_container.h',
  'ags/widget/ags_expander.h',
  'ags/widget/ags_vindicator.h',
  'ags/widget/ags_led_array.h',
  'ags/widget/ags_led.h',
  'ags/widget/ags_scrolled_piano.h',
  'ags/widget/ags_hled_array.h',
  'ags/widget/ags_notebook.h',
  'ags/widget/ags_piano.h',
]

subdir('po')
subdir('ags')
subdir('docs/reference/')

install_headers(libags_util_headers, install_dir: gsequencer_includedir / 'ags/util')
install_headers(libags_lib_headers, install_dir: gsequencer_includedir / 'ags/lib')
install_headers(libags_object_headers, install_dir: gsequencer_includedir / 'ags/object')
install_headers(libags_file_headers, install_dir: gsequencer_includedir / 'ags/file')
install_headers(libagsthread_headers, install_dir: gsequencer_includedir / 'ags/thread')
install_headers(libagsserver_headers, install_dir: gsequencer_includedir / 'ags/server')
install_headers(libagsserver_controller_headers, install_dir: gsequencer_includedir / 'ags/server/controller')
install_headers(libagsserver_security_headers, install_dir: gsequencer_includedir / 'ags/server/security')

install_headers(libagsaudio_plugin_headers, install_dir: gsequencer_includedir / 'ags/plugin')
install_headers(libagsaudio_audio_headers, install_dir: gsequencer_includedir / 'ags/audio')
install_headers(libagsaudio_audio_audio_unit_headers, install_dir: gsequencer_includedir / 'ags/audio/audio-unit')
install_headers(libagsaudio_audio_core_audio_headers, install_dir: gsequencer_includedir / 'ags/audio/core-audio')
install_headers(libagsaudio_audio_file_headers, install_dir: gsequencer_includedir / 'ags/audio/file')

if get_option('gstreamer')
install_headers(libagsaudio_audio_file_gstreamer_headers, install_dir: gsequencer_includedir / 'ags/audio/file')
endif

install_headers(libagsaudio_audio_fx_headers, install_dir: gsequencer_includedir / 'ags/audio/fx')

if get_option('gstreamer')
  install_headers(libagsaudio_audio_gstreamer_headers, install_dir: gsequencer_includedir / 'ags/audio/gstreamer')
endif

install_headers(libagsaudio_audio_jack_headers, install_dir: gsequencer_includedir / 'ags/audio/jack')
install_headers(libagsaudio_audio_midi_headers, install_dir: gsequencer_includedir / 'ags/audio/midi')
install_headers(libagsaudio_audio_osc_headers, install_dir: gsequencer_includedir / 'ags/audio/osc')
install_headers(libagsaudio_audio_osc_controller_headers, install_dir: gsequencer_includedir / 'ags/audio/osc/controller')
install_headers(libagsaudio_audio_osc_xmlrpc_headers, install_dir: gsequencer_includedir / 'ags/audio/osc/xmlrpc')
install_headers(libagsaudio_audio_pulse_headers, install_dir: gsequencer_includedir / 'ags/audio/pulse')
install_headers(libagsaudio_audio_recall_headers, install_dir: gsequencer_includedir / 'ags/audio/recall')
install_headers(libagsaudio_audio_task_headers, install_dir: gsequencer_includedir / 'ags/audio/task')
install_headers(libagsaudio_audio_thread_headers, install_dir: gsequencer_includedir / 'ags/audio/thread')
install_headers(libagsaudio_audio_wasapi_headers, install_dir: gsequencer_includedir / 'ags/audio/wasapi')

install_headers(libagsgui_headers, install_dir: gsequencer_includedir / 'ags/widget')

install_headers('ags/libags.h', 'ags/libags-audio.h', 'ags/libags-gui.h', install_dir: gsequencer_includedir / 'ags')

xsltproc = find_program('xsltproc', required: false)
if get_option('man_pages') and not xsltproc.found()
  error('No xsltproc found, but man pages were explicitly enabled')
endif

if get_option('man_pages') and xsltproc.found()
  xsltproc_flags = [
    '--nonet',
    '--stringparam', 'man.output.quietly', '1',
    '--stringparam', 'funcsynopsis.style', 'ansi',
    '--stringparam', 'man.th.extra1.suppress', '1',
    '--stringparam', 'man.authors.section.enabled', '1',
    '--stringparam', 'man.copyright.section.enabled', '1',
  ]

  man_files = [
    [ 'gsequencer', '1', ],
    [ 'midi2xml', '1', ],
  ]

  foreach man: man_files
    man_name = man.get(0)
    man_section = man.get(1, '1')
    custom_target('@0@.@1@'.format(man_name, man_section),
      input: '@0@.xml'.format(man_name),
      output: '@0@.@1@'.format(man_name, man_section),
      command: [
        xsltproc,
        xsltproc_flags,
	  '-o', '@OUTPUT@',
	  'http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl',
	  '@INPUT@',
      ],
      install: true,
      install_dir: join_paths(get_option('mandir'), 'man@0@'.format(man_section)))
  endforeach
endif

user_doc_images = [
  'docs/images/ags_audiorec.png',
  'docs/images/ags-automation-toolbar.png',
  'docs/images/ags_automation_window_drum.png',
  'docs/images/ags_equalizer10.png',
  'docs/images/ags_export_window.png',
  'docs/images/ags_machine_properties-input.png',
  'docs/images/ags_machine_properties-resize_channels.png',
  'docs/images/ags_navigation.png',
  'docs/images/ags_audio_preferences.png',
  'docs/images/ags_audio_connection_output_bulk.png',
  'docs/images/ags_audio_connection_output_line.png',
  'docs/images/ags_audio_connection_input_bulk.png',
  'docs/images/ags_audio_connection_input_line.png',
  'docs/images/ags_midi_dialog.png',
  'docs/images/ags_midi_export_wizard_no1.png',
  'docs/images/ags_midi_export_wizard_no2.png',
  'docs/images/ags_midi_import_wizard_no1.png',
  'docs/images/ags_midi_import_wizard_no2.png',
  'docs/images/ags_midi_preferences.png',
  'docs/images/ags_ffplayer.png',
  'docs/images/ags_fm_synth.png',
  'docs/images/ags_fm_syncsynth.png',
  'docs/images/ags_ladspa_browser.png',
  'docs/images/ags_machine_properties-link_input.png',
  'docs/images/ags_matrix.png',
  'docs/images/ags_panel.png',
  'docs/images/ags_spectrometer.png',
  'docs/images/ags_synth.png',
  'docs/images/ags_syncsynth.png',
  'docs/images/ags-toolbar.png',
  'docs/images/ags_drum.png',
  'docs/images/ags_generic_preferences.png',
  'docs/images/ags_machine_properties-output.png',
  'docs/images/ags_mixer.png',
  'docs/images/ags_pitch_sampler.png',
  'docs/images/ags_performance_preferences.png',
  'docs/images/ags_server_preferences.png',
  'docs/images/ags_machine_envelope_editor.png',
  'docs/images/ags_machine_envelope_info.png',
  'docs/images/ags_machine_envelope_pattern.png',
  'docs/images/ags_move_notes_dialog.png',
  'docs/images/ags_crop_notes_dialog.png',
  'docs/images/ags_position_notation_cursor_dialog.png',
  'docs/images/ags_position_automation_cursor_dialog.png',
  'docs/images/ags_position_wave_cursor_dialog.png',
  'docs/images/ags_select_notes_dialog.png',
  'docs/images/ags_select_accelerations_dialog.png',
  'docs/images/ags_select_buffers_dialog.png',
  'docs/images/ags_ramp_accelerations_dialog.png',
  'docs/images/ags-wave-toolbar.png',
  'docs/images/ags_wave_window_audiorec.png',
]

user_doc_images_adwaita = [
  'docs/images/adwaita/accessories-text-editor.png',
  'docs/images/adwaita/edit-clear-all.png',
  'docs/images/adwaita/edit-copy.png',
  'docs/images/adwaita/edit-cut.png',
  'docs/images/adwaita/edit-paste.png',
  'docs/images/adwaita/edit-select-all.png',
  'docs/images/adwaita/go-jump.png',
  'docs/images/adwaita/media-playback-start.png',
  'docs/images/adwaita/media-playback-stop.png',
  'docs/images/adwaita/media-seek-backward.png',
  'docs/images/adwaita/media-seek-forward.png',
  'docs/images/adwaita/media-skip-backward.png',
  'docs/images/adwaita/media-skip-forward.png',
  'docs/images/adwaita/object-flip-vertical.png',
  'docs/images/adwaita/system-run.png',
]

developer_doc_images = [
  'docs/images/AGS_iterator.png',
  'docs/images/AGS_recall_lifecycle.png',
  'docs/images/AGS_super-threaded.png',
  'docs/images/AGS_linking.png',
]

if get_option('online_help') and xsltproc.found()
  env = environment()

  user_manual_dir = join_paths(meson.project_build_root(), 'html/user-manual/')
  developer_manual_dir = join_paths(meson.project_build_root(), 'html/developer-manual/')
  osc_manual_dir = join_paths(meson.project_build_root(), 'html/osc-manual/')

  run_target('ags-user-docs', command: [ xsltproc, '--nonet', '--xinclude', '-o', user_manual_dir,  'http://docbook.sourceforge.net/release/xsl/current/htmlhelp/htmlhelp.xsl', 'docs/usersBook.xml' ])
  run_target('ags-developer-docs', command: [ xsltproc, '--nonet', '--xinclude', '-o', developer_manual_dir, 'http://docbook.sourceforge.net/release/xsl/current/htmlhelp/htmlhelp.xsl', 'docs/developersBook.xml' ])
  run_target('ags-osc-docs', command: [ xsltproc, '--nonet', '--xinclude', '-o', osc_manual_dir, 'http://docbook.sourceforge.net/release/xsl/current/htmlhelp/htmlhelp.xsl', 'docs/oscBook.xml' ])

  install_subdir(meson.project_build_root() / 'html/user-manual', install_dir: gsequencer_docdir)
  install_subdir(meson.project_build_root() / 'html/developer-manual', install_dir: gsequencer_docdir)
  install_subdir(meson.project_build_root() / 'html/osc-manual', install_dir: gsequencer_docdir)
  
  install_data(user_doc_images, install_dir: gsequencer_docdir / 'images')
  install_data(user_doc_images_adwaita, install_dir: gsequencer_docdir / 'images/adwaita')
  install_data(developer_doc_images, install_dir: gsequencer_docdir / 'images')
endif
