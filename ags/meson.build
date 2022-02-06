# Copyright (C) 2020-2021 Daniel Maksymow, Joël Krähemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

sources = []

dynamic_library_suffix = 'unknown'
if host_machine.system() == 'linux'
  dynamic_library_suffix = '.so'
elif host_machine.system() == 'freebsd'
  dynamic_library_suffix = '.so'
elif host_machine.system() == 'dragonfly'
  dynamic_library_suffix = '.so'
elif host_machine.system() == 'netbsd'
  dynamic_library_suffix = '.so'
elif host_machine.system() == 'openbsd'
  dynamic_library_suffix = '.so'
elif host_machine.system() == 'darwin'
  dynamic_library_suffix = '.dylib'
elif host_machine.system() == 'windows'
  dynamic_library_suffix = '.dll'
else
  error('add support to other os systems')
endif
define_int_template = '-D@0@=@1@'
define_str_template = '-D@0@="@1@"'
# TODO: this is needed only by plugin

add_project_arguments(define_str_template.format('AGS_DATA_DIR', gsequencer_datadir),
                      language:'c')
add_project_arguments(define_str_template.format('AGS_DOC_DIR', gsequencer_docdir),
                      language:'c')
add_project_arguments(define_str_template.format('PACKAGE',
                                             'gsequencer'),
                      language: 'c')
add_project_arguments(define_str_template.format('PACKAGE_VERSION',
                                             meson.project_version()),
                      language: 'c')
add_project_arguments(define_str_template.format('PACKAGE_BUGREPORT',
                                             'jkraehemann@gmail.com'),
                      language: 'c')
#add_project_arguments(define_str_template.format('PACKAGE_NAME',
#                                             'gsequencer'),
#                      language: 'c')
add_project_arguments(define_str_template.format('AGS_LIBRARY_SUFFIX',
                                             dynamic_library_suffix),
                      language: 'c')
add_project_arguments(define_int_template.format('AGS_VECTORIZED_BUILTIN_FUNCTIONS',
                                             1),
                      language: 'c')
add_project_arguments(define_int_template.format('PACKAGE_NAME',
                                             'gsequencer'),
                      language: 'c')
add_project_arguments(define_int_template.format('HAVE_ATK_2_12',
                                             1),
                      language: 'c')
add_project_arguments(define_int_template.format('HAVE_GLIB_2_44',
                                             1),
                      language: 'c')
add_project_arguments(define_int_template.format('HAVE_GLIB_2_52',
                                             1),
                      language: 'c')
add_project_arguments(define_int_template.format('HAVE_GLIB_2_56',
                                             1),
                      language: 'c')
add_project_arguments(define_int_template.format('HAVE_GLIB_2_6',
                                             1),
                      language: 'c')
add_project_arguments(define_int_template.format('HAVE_GLIB_2_62',
                                             1),
                      language: 'c')

ags_config = configuration_data({
  'AGS_WITH_LIBINSTPATCH' : true,
  'AGS_STRIP_RT_EVENTS' : true,
  'AGS_WITH_PUBLIC_LIBGSEQUENCER' : true,
  'AGS_WITH_RT' : true,
  'AGS_WITH_X11' : true,
                                 })

if get_option('w32api')
ags_config.set('AGS_WITH_W32API', true)
endif

if get_option('osxapi')
ags_config.set('AGS_WITH_OSXAPI', true)
endif

if get_option('alsa')
ags_config.set('AGS_WITH_ALSA', true)
endif

if get_option('oss')
ags_config.set('AGS_WITH_OSS', true)
endif

if get_option('core_audio')
ags_config.set('AGS_WITH_CORE_AUDIO', true)
endif

if get_option('jack')
ags_config.set('AGS_WITH_JACK', true)
endif

if get_option('pulse')
ags_config.set('AGS_WITH_PULSE', true)
endif

if get_option('wasapi')
ags_config.set('AGS_WITH_WASAPI', true)
endif

if get_option('quartz')
ags_config.set('AGS_WITH_QUARTZ', true)
endif

if get_option('gstreamer')
ags_config.set('AGS_WITH_GSTREAMER', true)
endif

if get_option('webkit')
ags_config.set('AGS_WITH_WEBKIT', true)
endif

if get_option('poppler')
ags_config.set('AGS_WITH_POPPLER', true)
endif

# TODO: it looks like this must be mandatory
ags_config.set('AGS_WITH_LIBINSTPATCH', true)

configure_file(output: 'config.h', configuration: ags_config)
configure_file(output: 'ags_api_config.h', configuration: ags_config,
  install: true,
  install_dir: gsequencer_includedir / 'ags')

common_dependencies = [
  m_dependency,
  glib_dependency,
  gobject_dependency,
  gthread_dependency,
  gio_dependency,
  xml2_dependency,
  uuid_dependency,
]

compiler_flags = []
# TODO: watch out!
fix_me_and_disable = true
if fix_me_and_disable
  compiler_flags += '-w'
endif

cxx_compiler_flags = [
  '-std=gnu++2a',
  '-DRELEASE',
  '-I/usr/include/vst3'
]

libags_sources = files(
  'util/ags_destroy_util.c',
  'util/ags_id_generator.c',
  'util/ags_list_util.c',
  'util/ags_soundcard_helper.c',
  'lib/ags_buffer_util.c',
  'lib/ags_complex.c',
  'lib/ags_conversion.c',
  'lib/ags_endian.c',
  'lib/ags_function.c',
  'lib/ags_log.c',
  'lib/ags_math_util.c',
  'lib/ags_regex.c',
  'lib/ags_solver_matrix.c',
  'lib/ags_solver_polynomial.c',
  'lib/ags_solver_vector.c',
  'lib/ags_string_util.c',
  'lib/ags_time.c',
  'lib/ags_turtle.c',
  'lib/ags_turtle_manager.c',
  'lib/ags_uuid.c',
  'object/ags_applicable.c',
  'object/ags_application_context.c',
  'object/ags_config.c',
  'object/ags_connectable.c',
  'object/ags_countable.c',
  'object/ags_cursor.c',
  'object/ags_globals.c',
  'object/ags_main_loop.c',
  'object/ags_marshal.c',
  'object/ags_mutable.c',
  'object/ags_plugin.c',
  'object/ags_portlet.c',
  'object/ags_priority.c',
  'object/ags_seekable.c',
  'object/ags_sequencer.c',
  'object/ags_soundcard.c',
  'object/ags_sound_server.c',
  'object/ags_tactable.c',
  'file/ags_file.c',
  'file/ags_file_id_ref.c',
  'file/ags_file_launch.c',
  'file/ags_file_link.c',
  'file/ags_file_lookup.c',
)

libags_dependencies = [
  common_dependencies,
  crypt_dependency,
]

libags = library('ags',
  libags_sources,
  c_args: compiler_flags,
  include_directories: [includes],
  dependencies: libags_dependencies,
  version: '3.0.0',
  install: true,
)

libags_thread_sources = files(
  'thread/ags_concurrency_provider.c',
  'thread/ags_destroy_worker.c',
  'thread/ags_generic_main_loop.c',
  'thread/ags_message_delivery.c',
  'thread/ags_message_envelope.c',
  'thread/ags_message_queue.c',
  'thread/ags_returnable_thread.c',
  'thread/ags_task.c',
  'thread/ags_task_completion.c',
  'thread/ags_task_launcher.c',
  'thread/ags_thread_application_context.c',
  'thread/ags_thread.c',
  'thread/ags_thread_pool.c',
  'thread/ags_timestamp.c',
  'thread/ags_worker_thread.c',
)

libags_thread_dependencies = [
  common_dependencies,
]

libags_thread = library('ags_thread',
  libags_thread_sources,
  c_args: compiler_flags,
  include_directories: [includes],
  dependencies: libags_thread_dependencies,
  link_with: [
    libags,
  ],
  version: '3.0.0',
  install: true,
)

libags_server_sources = files(
  'server/ags_registry.c',
  'server/ags_server_application_context.c',
  'server/ags_server.c',
  'server/ags_server_status.c',
  'server/ags_service_provider.c',
  'server/controller/ags_controller.c',
  'server/controller/ags_front_controller.c',
  'server/controller/ags_plugin_controller.c',
  'server/security/ags_authentication.c',
  'server/security/ags_authentication_manager.c',
  'server/security/ags_auth_security_context.c',
  'server/security/ags_business_group.c',
  'server/security/ags_business_group_manager.c',
  'server/security/ags_certificate.c',
  'server/security/ags_certificate_manager.c',
  'server/security/ags_password_store.c',
  'server/security/ags_password_store_manager.c',
  'server/security/ags_security_context.c',
  'server/security/ags_xml_authentication.c',
  'server/security/ags_xml_business_group.c',
  'server/security/ags_xml_certificate.c',
  'server/security/ags_xml_password_store.c',
)

libags_server_dependencies = [
  common_dependencies,
  dependency('libsoup-2.4'),
  crypt_dependency,
]

libags_server = library('libags_server',
  libags_server_sources,
  c_args: compiler_flags,
  include_directories: [includes],
  dependencies: libags_server_dependencies,
  link_with: [
    libags,
    libags_thread,
  ],
  version: '3.0.0',
  install: true,
)

libags_audio_sources = files(
  'plugin/ags_base_plugin.c',
  'plugin/ags_dssi_manager.c',
  'plugin/ags_dssi_plugin.c',
  'plugin/ags_ladspa_conversion.c',
  'plugin/ags_ladspa_manager.c',
  'plugin/ags_ladspa_plugin.c',
  'plugin/ags_lv2_conversion.c',
  'plugin/ags_lv2_event_manager.c',
  'plugin/ags_lv2_log_manager.c',
  'plugin/ags_lv2_manager.c',
  'plugin/ags_lv2_option_manager.c',
  'plugin/ags_lv2_plugin.c',
  'plugin/ags_lv2_preset.c',
  'plugin/ags_lv2_preset_manager.c',
  'plugin/ags_lv2_turtle_parser.c',
  'plugin/ags_lv2_turtle_scanner.c',
  'plugin/ags_lv2ui_manager.c',
  'plugin/ags_lv2ui_plugin.c',
  'plugin/ags_lv2_urid_manager.c',
  'plugin/ags_lv2_uri_map_manager.c',
  'plugin/ags_lv2_worker.c',
  'plugin/ags_lv2_worker_manager.c',
  'plugin/ags_plugin_port.c',
  'audio/ags_acceleration.c',
  'audio/ags_audio_application_context.c',
  'audio/ags_audio_buffer_util.c',
  'audio/ags_audio.c',
  'audio/ags_audio_signal.c',
  'audio/ags_automation.c',
  'audio/ags_buffer.c',
  'audio/ags_channel.c',
  'audio/ags_char_buffer_util.c',
  'audio/ags_chorus_util.c',
  'audio/ags_devin.c',
  'audio/ags_devout.c',
  'audio/ags_diatonic_scale.c',
  'audio/ags_envelope_util.c',
  'audio/ags_fast_pitch_util.c',
  'audio/ags_fifoout.c',
  'audio/ags_filter_util.c',
  'audio/ags_fm_synth_util.c',
  'audio/ags_fluid_iir_filter_util.c',
  'audio/ags_fluid_interpolate_4th_order_util.c',
  'audio/ags_fluid_interpolate_7th_order_util.c',
  'audio/ags_fluid_interpolate_linear_util.c',
  'audio/ags_fluid_interpolate_none_util.c',
  'audio/ags_fluid_pitch_util.c',
  'audio/ags_fluid_util.c',
  'audio/ags_fourier_transform_util.c',
  'audio/ags_frequency_aliase_util.c',
  'audio/ags_frequency_map.c',
  'audio/ags_frequency_map_manager.c',
  'audio/ags_fx_factory.c',
  'audio/ags_generic_pitch_util.c',
  'audio/ags_generic_recall_channel_run.c',
  'audio/ags_generic_recall_recycling.c',
  'audio/ags_hq_pitch_util.c',
  'audio/ags_synth_enums.c',
  'audio/ags_input.c',
  'audio/ags_lfo_synth_util.c',
  'audio/ags_linear_interpolate_util.c',
  'audio/ags_midi.c',
  'audio/ags_midiin.c',
  'audio/ags_noise_util.c',
  'audio/ags_notation.c',
  'audio/ags_note.c',
  'audio/ags_output.c',
  'audio/ags_pattern.c',
  'audio/ags_peak_util.c',
  'audio/ags_phase_shift_util.c',
  'audio/ags_playback.c',
  'audio/ags_playback_domain.c',
  'audio/ags_port.c',
  'audio/ags_port_util.c',
  'audio/ags_preset.c',
  'audio/ags_recall_audio.c',
  'audio/ags_recall_audio_run.c',
  'audio/ags_recall_audio_signal.c',
  'audio/ags_recall.c',
  'audio/ags_recall_channel.c',
  'audio/ags_recall_channel_run.c',
  'audio/ags_recall_container.c',
  'audio/ags_recall_dependency.c',
  'audio/ags_recall_dssi.c',
  'audio/ags_recall_dssi_run.c',
  'audio/ags_recall_factory.c',
  'audio/ags_recall_id.c',
  'audio/ags_recall_ladspa.c',
  'audio/ags_recall_ladspa_run.c',
  'audio/ags_recall_lv2.c',
  'audio/ags_recall_lv2_run.c',
  'audio/ags_recall_recycling.c',
  'audio/ags_recycling.c',
  'audio/ags_recycling_context.c',
  'audio/ags_resample_util.c',
  'audio/ags_sequencer_util.c',
  'audio/ags_sf2_synth_generator.c',
  'audio/ags_sf2_synth_util.c',
  'audio/ags_sf2_util.c',
  'audio/ags_sfz_synth_generator.c',
  'audio/ags_sfz_synth_util.c',
  'audio/ags_sfz_util.c',
  'audio/ags_sound_enums.c',
  'audio/ags_soundcard_util.c',
  'audio/ags_sound_provider.c',
  'audio/ags_synth_generator.c',
  'audio/ags_synth_util.c',
  'audio/ags_time_stretch_util.c',
  'audio/ags_track.c',
  'audio/ags_volume_util.c',
  'audio/ags_wave.c',
  'audio/alsa/ags_alsa_devin.c',
  'audio/alsa/ags_alsa_devout.c',
  'audio/alsa/ags_alsa_midiin.c',
  'audio/audio-unit/ags_audio_unit_client.c',
  'audio/audio-unit/ags_audio_unit_devin.c',
  'audio/audio-unit/ags_audio_unit_devout.c',
  'audio/audio-unit/ags_audio_unit_port.c',
  'audio/audio-unit/ags_audio_unit_server.c',
  'audio/core-audio/ags_core_audio_client.c',
  'audio/core-audio/ags_core_audio_devin.c',
  'audio/core-audio/ags_core_audio_devout.c',
  'audio/core-audio/ags_core_audio_midiin.c',
  'audio/core-audio/ags_core_audio_port.c',
  'audio/core-audio/ags_core_audio_server.c',
  'audio/file/ags_audio_container.c',
  'audio/file/ags_audio_container_manager.c',
  'audio/file/ags_audio_file.c',
  'audio/file/ags_audio_file_link.c',
  'audio/file/ags_audio_file_manager.c',
  'audio/file/ags_ipatch.c',
  'audio/file/ags_ipatch_dls2_reader.c',
  'audio/file/ags_ipatch_gig_reader.c',
  'audio/file/ags_ipatch_sample.c',
  'audio/file/ags_ipatch_sf2_reader.c',
  'audio/file/ags_sfz_file.c',
  'audio/file/ags_sfz_group.c',
  'audio/file/ags_sfz_region.c',
  'audio/file/ags_sfz_sample.c',
  'audio/file/ags_sndfile.c',
  'audio/file/ags_sound_container.c',
  'audio/file/ags_sound_resource.c',
  'audio/fx/ags_fx_analyse_audio.c',
  'audio/fx/ags_fx_analyse_audio_processor.c',
  'audio/fx/ags_fx_analyse_audio_signal.c',
  'audio/fx/ags_fx_analyse_channel.c',
  'audio/fx/ags_fx_analyse_channel_processor.c',
  'audio/fx/ags_fx_analyse_recycling.c',
  'audio/fx/ags_fx_buffer_audio.c',
  'audio/fx/ags_fx_buffer_audio_processor.c',
  'audio/fx/ags_fx_buffer_audio_signal.c',
  'audio/fx/ags_fx_buffer_channel.c',
  'audio/fx/ags_fx_buffer_channel_processor.c',
  'audio/fx/ags_fx_buffer_recycling.c',
  'audio/fx/ags_fx_chorus_audio.c',
  'audio/fx/ags_fx_chorus_audio_processor.c',
  'audio/fx/ags_fx_chorus_audio_signal.c',
  'audio/fx/ags_fx_chorus_channel.c',
  'audio/fx/ags_fx_chorus_channel_processor.c',
  'audio/fx/ags_fx_chorus_recycling.c',
  'audio/fx/ags_fx_dssi_audio.c',
  'audio/fx/ags_fx_dssi_audio_processor.c',
  'audio/fx/ags_fx_dssi_audio_signal.c',
  'audio/fx/ags_fx_dssi_channel.c',
  'audio/fx/ags_fx_dssi_channel_processor.c',
  'audio/fx/ags_fx_dssi_recycling.c',
  'audio/fx/ags_fx_envelope_audio.c',
  'audio/fx/ags_fx_envelope_audio_processor.c',
  'audio/fx/ags_fx_envelope_audio_signal.c',
  'audio/fx/ags_fx_envelope_channel.c',
  'audio/fx/ags_fx_envelope_channel_processor.c',
  'audio/fx/ags_fx_envelope_recycling.c',
  'audio/fx/ags_fx_eq10_audio.c',
  'audio/fx/ags_fx_eq10_audio_processor.c',
  'audio/fx/ags_fx_eq10_audio_signal.c',
  'audio/fx/ags_fx_eq10_channel.c',
  'audio/fx/ags_fx_eq10_channel_processor.c',
  'audio/fx/ags_fx_eq10_recycling.c',
  'audio/fx/ags_fx_fm_synth_audio.c',
  'audio/fx/ags_fx_fm_synth_audio_processor.c',
  'audio/fx/ags_fx_fm_synth_audio_signal.c',
  'audio/fx/ags_fx_fm_synth_channel.c',
  'audio/fx/ags_fx_fm_synth_channel_processor.c',
  'audio/fx/ags_fx_fm_synth_recycling.c',
  'audio/fx/ags_fx_high_pass_audio.c',
  'audio/fx/ags_fx_high_pass_audio_processor.c',
  'audio/fx/ags_fx_high_pass_audio_signal.c',
  'audio/fx/ags_fx_high_pass_channel.c',
  'audio/fx/ags_fx_high_pass_channel_processor.c',
  'audio/fx/ags_fx_high_pass_recycling.c',
  'audio/fx/ags_fx_ladspa_audio.c',
  'audio/fx/ags_fx_ladspa_audio_processor.c',
  'audio/fx/ags_fx_ladspa_audio_signal.c',
  'audio/fx/ags_fx_ladspa_channel.c',
  'audio/fx/ags_fx_ladspa_channel_processor.c',
  'audio/fx/ags_fx_ladspa_recycling.c',
  'audio/fx/ags_fx_lfo_audio.c',
  'audio/fx/ags_fx_lfo_audio_processor.c',
  'audio/fx/ags_fx_lfo_audio_signal.c',
  'audio/fx/ags_fx_lfo_channel.c',
  'audio/fx/ags_fx_lfo_channel_processor.c',
  'audio/fx/ags_fx_lfo_recycling.c',
  'audio/fx/ags_fx_low_pass_audio.c',
  'audio/fx/ags_fx_low_pass_audio_processor.c',
  'audio/fx/ags_fx_low_pass_audio_signal.c',
  'audio/fx/ags_fx_low_pass_channel.c',
  'audio/fx/ags_fx_low_pass_channel_processor.c',
  'audio/fx/ags_fx_low_pass_recycling.c',
  'audio/fx/ags_fx_lv2_audio.c',
  'audio/fx/ags_fx_lv2_audio_processor.c',
  'audio/fx/ags_fx_lv2_audio_signal.c',
  'audio/fx/ags_fx_lv2_channel.c',
  'audio/fx/ags_fx_lv2_channel_processor.c',
  'audio/fx/ags_fx_lv2_recycling.c',
  'audio/fx/ags_fx_notation_audio.c',
  'audio/fx/ags_fx_notation_audio_processor.c',
  'audio/fx/ags_fx_notation_audio_signal.c',
  'audio/fx/ags_fx_notation_channel.c',
  'audio/fx/ags_fx_notation_channel_processor.c',
  'audio/fx/ags_fx_notation_recycling.c',
  'audio/fx/ags_fx_pattern_audio.c',
  'audio/fx/ags_fx_pattern_audio_processor.c',
  'audio/fx/ags_fx_pattern_audio_signal.c',
  'audio/fx/ags_fx_pattern_channel.c',
  'audio/fx/ags_fx_pattern_channel_processor.c',
  'audio/fx/ags_fx_pattern_recycling.c',
  'audio/fx/ags_fx_peak_audio.c',
  'audio/fx/ags_fx_peak_audio_processor.c',
  'audio/fx/ags_fx_peak_audio_signal.c',
  'audio/fx/ags_fx_peak_channel.c',
  'audio/fx/ags_fx_peak_channel_processor.c',
  'audio/fx/ags_fx_peak_recycling.c',
  'audio/fx/ags_fx_playback_audio.c',
  'audio/fx/ags_fx_playback_audio_processor.c',
  'audio/fx/ags_fx_playback_audio_signal.c',
  'audio/fx/ags_fx_playback_channel.c',
  'audio/fx/ags_fx_playback_channel_processor.c',
  'audio/fx/ags_fx_playback_recycling.c',
  'audio/fx/ags_fx_sf2_synth_audio.c',
  'audio/fx/ags_fx_sf2_synth_audio_processor.c',
  'audio/fx/ags_fx_sf2_synth_audio_signal.c',
  'audio/fx/ags_fx_sf2_synth_channel.c',
  'audio/fx/ags_fx_sf2_synth_channel_processor.c',
  'audio/fx/ags_fx_sf2_synth_recycling.c',
  'audio/fx/ags_fx_sfz_synth_audio.c',
  'audio/fx/ags_fx_sfz_synth_audio_processor.c',
  'audio/fx/ags_fx_sfz_synth_audio_signal.c',
  'audio/fx/ags_fx_sfz_synth_channel.c',
  'audio/fx/ags_fx_sfz_synth_channel_processor.c',
  'audio/fx/ags_fx_sfz_synth_recycling.c',
  'audio/fx/ags_fx_synth_audio.c',
  'audio/fx/ags_fx_synth_audio_processor.c',
  'audio/fx/ags_fx_synth_audio_signal.c',
  'audio/fx/ags_fx_synth_channel.c',
  'audio/fx/ags_fx_synth_channel_processor.c',
  'audio/fx/ags_fx_synth_recycling.c',
  'audio/fx/ags_fx_two_pass_aliase_audio.c',
  'audio/fx/ags_fx_two_pass_aliase_audio_processor.c',
  'audio/fx/ags_fx_two_pass_aliase_audio_signal.c',
  'audio/fx/ags_fx_two_pass_aliase_channel.c',
  'audio/fx/ags_fx_two_pass_aliase_channel_processor.c',
  'audio/fx/ags_fx_two_pass_aliase_recycling.c',
  'audio/fx/ags_fx_volume_audio.c',
  'audio/fx/ags_fx_volume_audio_processor.c',
  'audio/fx/ags_fx_volume_audio_signal.c',
  'audio/fx/ags_fx_volume_channel.c',
  'audio/fx/ags_fx_volume_channel_processor.c',
  'audio/fx/ags_fx_volume_recycling.c',
  'audio/jack/ags_jack_client.c',
  'audio/jack/ags_jack_devin.c',
  'audio/jack/ags_jack_devout.c',
  'audio/jack/ags_jack_midiin.c',
  'audio/jack/ags_jack_port.c',
  'audio/jack/ags_jack_server.c',
  'audio/midi/ags_midi_buffer_util.c',
  'audio/midi/ags_midi_builder.c',
  'audio/midi/ags_midi_file.c',
  'audio/midi/ags_midi_parser.c',
  'audio/midi/ags_midi_util.c',
  'audio/osc/ags_osc_buffer_util.c',
  'audio/osc/ags_osc_builder.c',
  'audio/osc/ags_osc_client.c',
  'audio/osc/ags_osc_connection.c',
  'audio/osc/ags_osc_message.c',
  'audio/osc/ags_osc_parser.c',
  'audio/osc/ags_osc_response.c',
  'audio/osc/ags_osc_server.c',
  'audio/osc/ags_osc_util.c',
  'audio/osc/ags_osc_websocket_connection.c',
  'audio/osc/ags_osc_xmlrpc_message.c',
  'audio/osc/ags_osc_xmlrpc_server.c',
  'audio/osc/controller/ags_osc_action_controller.c',
  'audio/osc/controller/ags_osc_config_controller.c',
  'audio/osc/controller/ags_osc_controller.c',
  'audio/osc/controller/ags_osc_export_controller.c',
  'audio/osc/controller/ags_osc_front_controller.c',
  'audio/osc/controller/ags_osc_info_controller.c',
  'audio/osc/controller/ags_osc_meter_controller.c',
  'audio/osc/controller/ags_osc_node_controller.c',
  'audio/osc/controller/ags_osc_plugin_controller.c',
  'audio/osc/controller/ags_osc_renew_controller.c',
  'audio/osc/controller/ags_osc_status_controller.c',
  'audio/osc/xmlrpc/ags_osc_xmlrpc_controller.c',
  'audio/oss/ags_oss_devin.c',
  'audio/oss/ags_oss_devout.c',
  'audio/oss/ags_oss_midiin.c',
  'audio/pulse/ags_pulse_client.c',
  'audio/pulse/ags_pulse_devin.c',
  'audio/pulse/ags_pulse_devout.c',
  'audio/pulse/ags_pulse_port.c',
  'audio/pulse/ags_pulse_server.c',
  'audio/recall/ags_analyse_audio_signal.c',
  'audio/recall/ags_analyse_channel.c',
  'audio/recall/ags_analyse_channel_run.c',
  'audio/recall/ags_analyse_recycling.c',
  'audio/recall/ags_buffer_audio_signal.c',
  'audio/recall/ags_buffer_channel.c',
  'audio/recall/ags_buffer_channel_run.c',
  'audio/recall/ags_buffer_recycling.c',
  'audio/recall/ags_capture_wave_audio.c',
  'audio/recall/ags_capture_wave_audio_run.c',
  'audio/recall/ags_capture_wave_channel.c',
  'audio/recall/ags_capture_wave_channel_run.c',
  'audio/recall/ags_copy_audio_signal.c',
  'audio/recall/ags_copy_channel.c',
  'audio/recall/ags_copy_channel_run.c',
  'audio/recall/ags_copy_pattern_audio.c',
  'audio/recall/ags_copy_pattern_audio_run.c',
  'audio/recall/ags_copy_pattern_channel.c',
  'audio/recall/ags_copy_pattern_channel_run.c',
  'audio/recall/ags_copy_recycling.c',
  'audio/recall/ags_count_beats_audio.c',
  'audio/recall/ags_count_beats_audio_run.c',
  'audio/recall/ags_delay_audio.c',
  'audio/recall/ags_delay_audio_run.c',
  'audio/recall/ags_envelope_audio_signal.c',
  'audio/recall/ags_envelope_channel.c',
  'audio/recall/ags_envelope_channel_run.c',
  'audio/recall/ags_envelope_recycling.c',
  'audio/recall/ags_eq10_audio_signal.c',
  'audio/recall/ags_eq10_channel.c',
  'audio/recall/ags_eq10_channel_run.c',
  'audio/recall/ags_eq10_recycling.c',
  'audio/recall/ags_feed_audio_signal.c',
  'audio/recall/ags_feed_channel.c',
  'audio/recall/ags_feed_channel_run.c',
  'audio/recall/ags_feed_recycling.c',
  'audio/recall/ags_lfo_audio_signal.c',
  'audio/recall/ags_lfo_channel.c',
  'audio/recall/ags_lfo_channel_run.c',
  'audio/recall/ags_lfo_recycling.c',
  'audio/recall/ags_loop_channel.c',
  'audio/recall/ags_loop_channel_run.c',
  'audio/recall/ags_mute_audio.c',
  'audio/recall/ags_mute_audio_run.c',
  'audio/recall/ags_mute_audio_signal.c',
  'audio/recall/ags_mute_channel.c',
  'audio/recall/ags_mute_channel_run.c',
  'audio/recall/ags_mute_recycling.c',
  'audio/recall/ags_peak_audio_signal.c',
  'audio/recall/ags_peak_channel.c',
  'audio/recall/ags_peak_channel_run.c',
  'audio/recall/ags_peak_recycling.c',
  'audio/recall/ags_play_audio.c',
  'audio/recall/ags_play_audio_signal.c',
  'audio/recall/ags_play_channel.c',
  'audio/recall/ags_play_channel_run.c',
  'audio/recall/ags_play_channel_run_master.c',
  'audio/recall/ags_play_dssi_audio.c',
  'audio/recall/ags_play_dssi_audio_run.c',
  'audio/recall/ags_play_lv2_audio.c',
  'audio/recall/ags_play_lv2_audio_run.c',
  'audio/recall/ags_play_notation_audio.c',
  'audio/recall/ags_play_notation_audio_run.c',
  'audio/recall/ags_play_recycling.c',
  'audio/recall/ags_play_wave_audio.c',
  'audio/recall/ags_play_wave_audio_run.c',
  'audio/recall/ags_play_wave_channel.c',
  'audio/recall/ags_play_wave_channel_run.c',
  'audio/recall/ags_prepare_audio_signal.c',
  'audio/recall/ags_prepare_channel.c',
  'audio/recall/ags_prepare_channel_run.c',
  'audio/recall/ags_prepare_recycling.c',
  'audio/recall/ags_record_midi_audio.c',
  'audio/recall/ags_record_midi_audio_run.c',
  'audio/recall/ags_route_dssi_audio.c',
  'audio/recall/ags_route_dssi_audio_run.c',
  'audio/recall/ags_route_lv2_audio.c',
  'audio/recall/ags_route_lv2_audio_run.c',
  'audio/recall/ags_rt_stream_audio_signal.c',
  'audio/recall/ags_rt_stream_channel.c',
  'audio/recall/ags_rt_stream_channel_run.c',
  'audio/recall/ags_rt_stream_recycling.c',
  'audio/recall/ags_stream_audio_signal.c',
  'audio/recall/ags_stream_channel.c',
  'audio/recall/ags_stream_channel_run.c',
  'audio/recall/ags_stream_recycling.c',
  'audio/recall/ags_volume_audio_signal.c',
  'audio/recall/ags_volume_channel.c',
  'audio/recall/ags_volume_channel_run.c',
  'audio/recall/ags_volume_recycling.c',
  'audio/task/ags_add_audio.c',
  'audio/task/ags_add_audio_signal.c',
  'audio/task/ags_add_effect.c',
  'audio/task/ags_add_note.c',
  'audio/task/ags_add_soundcard.c',
  'audio/task/ags_apply_bpm.c',
  'audio/task/ags_apply_presets.c',
  'audio/task/ags_apply_sequencer_length.c',
  'audio/task/ags_apply_sf2_midi_locale.c',
  'audio/task/ags_apply_sf2_synth.c',
  'audio/task/ags_apply_sfz_instrument.c',
  'audio/task/ags_apply_sfz_synth.c',
  'audio/task/ags_apply_sound_config.c',
  'audio/task/ags_apply_synth.c',
  'audio/task/ags_apply_tact.c',
  'audio/task/ags_cancel_audio.c',
  'audio/task/ags_cancel_channel.c',
  'audio/task/ags_clear_audio_signal.c',
  'audio/task/ags_clear_buffer.c',
  'audio/task/ags_crop_note.c',
  'audio/task/ags_export_output.c',
  'audio/task/ags_free_selection.c',
  'audio/task/ags_link_channel.c',
  'audio/task/ags_move_note.c',
  'audio/task/ags_open_file.c',
  'audio/task/ags_open_sf2_instrument.c',
  'audio/task/ags_open_sf2_sample.c',
  'audio/task/ags_open_sfz_file.c',
  'audio/task/ags_open_single_file.c',
  'audio/task/ags_open_wave.c',
  'audio/task/ags_remove_audio.c',
  'audio/task/ags_remove_audio_signal.c',
  'audio/task/ags_remove_note.c',
  'audio/task/ags_remove_soundcard.c',
  'audio/task/ags_reset_amplitude.c',
  'audio/task/ags_reset_fx_analyse.c',
  'audio/task/ags_reset_fx_peak.c',
  'audio/task/ags_reset_note.c',
  'audio/task/ags_reset_peak.c',
  'audio/task/ags_resize_audio.c',
  'audio/task/ags_seek_soundcard.c',
  'audio/task/ags_set_audio_channels.c',
  'audio/task/ags_set_buffer_size.c',
  'audio/task/ags_set_device.c',
  'audio/task/ags_set_format.c',
  'audio/task/ags_set_muted.c',
  'audio/task/ags_set_samplerate.c',
  'audio/task/ags_start_audio.c',
  'audio/task/ags_start_channel.c',
  'audio/task/ags_start_sequencer.c',
  'audio/task/ags_start_soundcard.c',
  'audio/task/ags_stop_sequencer.c',
  'audio/task/ags_stop_soundcard.c',
  'audio/task/ags_stop_thread.c',
  'audio/task/ags_switch_buffer_flag.c',
  'audio/task/ags_tic_device.c',
  'audio/task/ags_toggle_pattern_bit.c',
  'audio/thread/ags_audio_loop.c',
  'audio/thread/ags_audio_thread.c',
  'audio/thread/ags_channel_thread.c',
  'audio/thread/ags_export_thread.c',
  'audio/thread/ags_sequencer_thread.c',
  'audio/thread/ags_sf2_midi_locale_loader.c',
  'audio/thread/ags_sfz_instrument_loader.c',
  'audio/thread/ags_sf2_loader.c',
  'audio/thread/ags_sfz_loader.c',
  'audio/thread/ags_soundcard_thread.c',
  'audio/thread/ags_wave_loader.c',
  'audio/wasapi/ags_wasapi_devin.c',
  'audio/wasapi/ags_wasapi_devout.c',
)

if get_option('vst3')
libags_audio_sources += files(
    'plugin/ags_vst3_conversion.c',
    'plugin/ags_vst3_manager.c',
    'plugin/ags_vst3_plugin.c',
    'audio/fx/ags_fx_vst3_audio.c',
    'audio/fx/ags_fx_vst3_audio_processor.c',
    'audio/fx/ags_fx_vst3_audio_signal.c',
    'audio/fx/ags_fx_vst3_channel.c',
    'audio/fx/ags_fx_vst3_channel_processor.c',
    'audio/fx/ags_fx_vst3_recycling.c',
    'audio/task/ags_write_vst3_port.c',
    'audio/task/ags_instantiate_vst3_plugin.c',
  )
endif

if get_option('gstreamer')
libags_audio_sources += files(
    'audio/gstreamer/ags_gstreamer_client.c',
    'audio/gstreamer/ags_gstreamer_server.c',
    'audio/gstreamer/ags_gstreamer_devout.c',
    'audio/gstreamer/ags_gstreamer_port.c',
    'audio/gstreamer/ags_gstreamer_devin.c',
    'audio/file/ags_gstreamer_file.c',
  )
endif

libags_audio_dependencies = [
  common_dependencies,
  dl_dependency,
  soup_dependency,
  instpatch_dependency,
  samplerate_dependency,
  fftw3_dependency,
  lv2_dependency,
]

if get_option('alsa')
  libags_audio_dependencies += alsa_dependency
endif

if get_option('jack')
  libags_audio_dependencies += jack_dependency
endif

if get_option('pulse')
  libags_audio_dependencies += dependency('libpulse')
endif

if get_option('gstreamer')
  libags_audio_dependencies += gstreamer_dependency
  libags_audio_dependencies += gstreamer_audio_dependency
  libags_audio_dependencies += gstreamer_video_dependency
  libags_audio_dependencies += gstreamer_app_dependency
  libags_audio_dependencies += gstreamer_pbutils_dependency
endif

libags_audio = library('ags_audio',
  libags_audio_sources,
  c_args: compiler_flags,
  include_directories: [includes],
  dependencies: libags_audio_dependencies,
  link_with: [
    libags,
    libags_thread,
    libags_server,
  ],
  version: '3.0.0',
  install: true,
)

libags_vst_sources = files()

if get_option('vst3')

  libags_vst_sources += files(
    'vst3-capi/host/ags_vst_host_context.cpp',
    'vst3-capi/host/hostcontext.h',
    'vst3-capi/host/updatehandler.h',
    'vst3-capi/host/hostcontext.cpp',
    'vst3-capi/host/updatehandler.cpp',
    'vst3-capi/host/ags_vst_host_context.cpp',
    'vst3-capi/pluginterfaces/gui/ags_vst_iplug_view_content_scale_support.cpp',
    'vst3-capi/pluginterfaces/gui/ags_vst_iplug_view.cpp',
    'vst3-capi/pluginterfaces/base/ags_vst_geo_constants.cpp',
    'vst3-capi/pluginterfaces/base/ags_vst_keycodes.cpp',
    'vst3-capi/pluginterfaces/base/ags_vst_const_string_table.cpp',
    'vst3-capi/pluginterfaces/base/ags_vst_futils.cpp',
    'vst3-capi/pluginterfaces/base/ags_vst_ucolor_spec.cpp',
    'vst3-capi/pluginterfaces/base/ags_vst_ustring.cpp',
    'vst3-capi/pluginterfaces/base/ags_vst_fstrdefs.cpp',
    'vst3-capi/pluginterfaces/base/ags_vst_ibstream.cpp',
    'vst3-capi/pluginterfaces/base/ags_vst_funknown.cpp',
    'vst3-capi/pluginterfaces/base/ags_vst_istring_result.cpp',
    'vst3-capi/pluginterfaces/base/ags_vst_ftypes.cpp',
    'vst3-capi/pluginterfaces/base/ags_vst_icloneable.cpp',
    'vst3-capi/pluginterfaces/base/ags_vst_iplugin_base.cpp',
    'vst3-capi/pluginterfaces/base/ags_vst_iupdate_handler.cpp',
    'vst3-capi/pluginterfaces/base/ags_vst_ipersistent.cpp',
    'vst3-capi/pluginterfaces/base/ags_vst_ierror_context.cpp',
    'vst3-capi/pluginterfaces/base/ags_vst_fvariant.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_iaudio_processor.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_iautomation_state.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_iattributes.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_ichannel_context_info.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_icomponent.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_icontext_menu.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_iedit_controller.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_ievents.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_ihost_application.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_iinter_app_audio.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_inote_expression.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_imessage.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_imidi_learn.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_iplug_view.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_iparameter_changes.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_iphysical_ui.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_iparameter_function_name.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_iplug_interface_support.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_iprefetchable_support.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_iprocess_context.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_irepresentation.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_iunits.cpp',
    'vst3-capi/pluginterfaces/vst/ags_vst_speaker.cpp',
    'vst3-capi/public.sdk/source/common/ags_vst_plugin_view.cpp',
    'vst3-capi/public.sdk/source/vst/ags_vst_edit_controller.cpp',
    'vst3-capi/public.sdk/source/vst/ags_vst_component_base.cpp',
    'vst3-capi/public.sdk/source/vst/ags_vst_component.cpp',
    'vst3-capi/public.sdk/source/vst/ags_vst_parameters.cpp',
    'vst3-capi/public.sdk/source/vst/ags_vst_audio_effect.cpp',
    'vst3-capi/public.sdk/source/vst/ags_vst_bus.cpp',
    'vst3-capi/public.sdk/source/vst/hosting/ags_vst_event_list.cpp',
    'vst3-capi/base/source/ags_vst_fbuffer.cpp',
    'vst3-capi/base/source/ags_vst_fobject.cpp',
    'vst3-capi/base/source/ags_vst_fstring.cpp',
    'vst3-capi/util/ags_vst_string_util.cpp'
   )

  libags_vst_dependencies = [
    glib_dependency,
 ]

  libags_vst = library('ags_vst',
    libags_vst_sources,
    cpp_args: cxx_compiler_flags,
    dependencies: libags_vst_dependencies,
    include_directories: [includes],
    version: '3.0.0',
    install: true,
  )

endif

libags_gui_sources = files(
  'widget/ags_cartesian.c',
  'widget/ags_container.c',
  'widget/ags_dial.c',
  'widget/ags_expander.c',
  'widget/ags_expander_set.c',
  'widget/ags_hindicator.c',
  'widget/ags_hled_array.c',
  'widget/ags_hlevel_box.c',
  'widget/ags_hscale_box.c',
  'widget/ags_indicator.c',
  'widget/ags_led_array.c',
  'widget/ags_led.c',
  'widget/ags_level_box.c',
  'widget/ags_level.c',
  'widget/ags_notebook.c',
  'widget/ags_piano.c',
  'widget/ags_ruler.c',
  'widget/ags_scale_box.c',
  'widget/ags_scale.c',
  'widget/ags_scrolled_level_box.c',
  'widget/ags_scrolled_piano.c',
  'widget/ags_scrolled_scale_box.c',
  'widget/ags_vindicator.c',
  'widget/ags_vled_array.c',
  'widget/ags_vlevel_box.c',
  'widget/ags_vscale_box.c',
  'widget/ags_viewport.c',
  'widget/ags_widget_marshal.c',
)

libags_gui_dependencies = [
  common_dependencies,
  dependency('gtk+-3.0'),
]

libags_gui = library('ags_gui',
  libags_gui_sources,
  c_args: compiler_flags,
  include_directories: [includes],
  dependencies: libags_gui_dependencies,
  version: '3.0.0',
  install: true,
)

libgsequencer_sources = files(
  'X/ags_animation_window.c',
  'X/ags_audio_preferences.c',
  'X/ags_audio_preferences_callbacks.c',
  'X/ags_automation_editor.c',
  'X/ags_automation_editor_callbacks.c',
  'X/ags_automation_window.c',
  'X/ags_automation_window_callbacks.c',
  'X/ags_bulk_member.c',
  'X/ags_bulk_member_callbacks.c',
  'X/ags_composite_editor.c',
  'X/ags_composite_editor_callbacks.c',
  'X/ags_connection_editor.c',
  'X/ags_connection_editor_callbacks.c',
  'X/ags_context_menu.c',
  'X/ags_dssi_browser.c',
  'X/ags_dssi_browser_callbacks.c',
  'X/ags_effect_bridge.c',
  'X/ags_effect_bridge_callbacks.c',
  'X/ags_effect_bulk.c',
  'X/ags_effect_bulk_callbacks.c',
  'X/ags_effect_line.c',
  'X/ags_effect_line_callbacks.c',
  'X/ags_effect_pad.c',
  'X/ags_effect_pad_callbacks.c',
  'X/ags_effect_separator.c',
  'X/ags_export_soundcard.c',
  'X/ags_export_soundcard_callbacks.c',
  'X/ags_export_window.c',
  'X/ags_export_window_callbacks.c',
  'X/ags_generic_preferences.c',
  'X/ags_generic_preferences_callbacks.c',
  'X/ags_input_collection_editor.c',
  'X/ags_input_collection_editor_callbacks.c',
  'X/ags_input_editor.c',
  'X/ags_input_editor_callbacks.c',
  'X/ags_input_listing_editor.c',
  'X/ags_input_listing_editor_callbacks.c',
  'X/ags_ladspa_browser.c',
  'X/ags_ladspa_browser_callbacks.c',
  'X/ags_line.c',
  'X/ags_line_callbacks.c',
  'X/ags_line_editor.c',
  'X/ags_line_editor_callbacks.c',
  'X/ags_line_member.c',
  'X/ags_line_member_callbacks.c',
  'X/ags_line_member_editor.c',
  'X/ags_line_member_editor_callbacks.c',
  'X/ags_link_collection_editor.c',
  'X/ags_link_collection_editor_callbacks.c',
  'X/ags_link_editor.c',
  'X/ags_link_editor_callbacks.c',
  'X/ags_listing_editor.c',
  'X/ags_listing_editor_callbacks.c',
  'X/ags_lv2_browser.c',
  'X/ags_lv2_browser_callbacks.c',
  'X/ags_machine.c',
  'X/ags_machine_callbacks.c',
  'X/ags_machine_editor.c',
  'X/ags_machine_editor_callbacks.c',
  'X/ags_machine_util.c',
  'X/ags_menu_action_callbacks.c',
  'X/ags_menu_bar.c',
  'X/ags_midi_dialog.c',
  'X/ags_midi_dialog_callbacks.c',
  'X/ags_midi_preferences.c',
  'X/ags_midi_preferences_callbacks.c',
  'X/ags_navigation.c',
  'X/ags_navigation_callbacks.c',
  'X/ags_notation_editor.c',
  'X/ags_notation_editor_callbacks.c',
  'X/ags_online_help_window.c',
  'X/ags_online_help_window_callbacks.c',
  'X/ags_osc_server_preferences.c',
  'X/ags_osc_server_preferences_callbacks.c',
  'X/ags_output_collection_editor.c',
  'X/ags_output_collection_editor_callbacks.c',
  'X/ags_output_editor.c',
  'X/ags_output_editor_callbacks.c',
  'X/ags_output_listing_editor.c',
  'X/ags_output_listing_editor_callbacks.c',
  'X/ags_pad.c',
  'X/ags_pad_callbacks.c',
  'X/ags_pad_editor.c',
  'X/ags_pad_editor_callbacks.c',
  'X/ags_performance_preferences.c',
  'X/ags_performance_preferences_callbacks.c',
  'X/ags_playback_window.c',
  'X/ags_playback_window_callbacks.c',
  'X/ags_plugin_browser.c',
  'X/ags_plugin_browser_callbacks.c',
  'X/ags_plugin_preferences.c',
  'X/ags_plugin_preferences_callbacks.c',
  'X/ags_preferences.c',
  'X/ags_preferences_callbacks.c',
  'X/ags_property_collection_editor.c',
  'X/ags_property_collection_editor_callbacks.c',
  'X/ags_property_editor.c',
  'X/ags_property_editor_callbacks.c',
  'X/ags_property_listing_editor.c',
  'X/ags_quit_dialog.c',
  'X/ags_quit_dialog_callbacks.c',
  'X/ags_resize_editor.c',
  'X/ags_sequencer_editor.c',
  'X/ags_sequencer_editor_callbacks.c',
  'X/ags_server_preferences.c',
  'X/ags_server_preferences_callbacks.c',
  'X/ags_sheet_editor.c',
  'X/ags_sheet_editor_callbacks.c',
  'X/ags_sheet_window.c',
  'X/ags_sheet_window_callbacks.c',
  'X/ags_soundcard_editor.c',
  'X/ags_soundcard_editor_callbacks.c',
  'X/ags_ui_provider.c',
  'X/ags_wave_editor.c',
  'X/ags_wave_editor_callbacks.c',
  'X/ags_wave_window.c',
  'X/ags_wave_window_callbacks.c',
  'X/ags_window.c',
  'X/ags_window_callbacks.c',
  'X/ags_xorg_application_context.c',
  'X/editor/ags_automation_edit_box.c',
  'X/editor/ags_automation_edit.c',
  'X/editor/ags_automation_edit_callbacks.c',
  'X/editor/ags_automation_meta.c',
  'X/editor/ags_automation_meta_callbacks.c',
  'X/editor/ags_automation_toolbar.c',
  'X/editor/ags_automation_toolbar_callbacks.c',
  'X/editor/ags_composite_edit.c',
  'X/editor/ags_composite_edit_callbacks.c',
  'X/editor/ags_composite_toolbar.c',
  'X/editor/ags_composite_toolbar_callbacks.c',
  'X/editor/ags_crop_note_dialog.c',
  'X/editor/ags_crop_note_dialog_callbacks.c',
  'X/editor/ags_envelope_dialog.c',
  'X/editor/ags_envelope_dialog_callbacks.c',
  'X/editor/ags_envelope_editor.c',
  'X/editor/ags_envelope_editor_callbacks.c',
  'X/editor/ags_envelope_info.c',
  'X/editor/ags_envelope_info_callbacks.c',
  'X/editor/ags_machine_radio_button.c',
  'X/editor/ags_machine_radio_button_callbacks.c',
  'X/editor/ags_machine_selection.c',
  'X/editor/ags_machine_selector.c',
  'X/editor/ags_machine_selector_callbacks.c',
  'X/editor/ags_move_note_dialog.c',
  'X/editor/ags_move_note_dialog_callbacks.c',
  'X/editor/ags_notation_edit.c',
  'X/editor/ags_notation_edit_callbacks.c',
  'X/editor/ags_notation_meta.c',
  'X/editor/ags_notation_meta_callbacks.c',
  'X/editor/ags_notation_toolbar.c',
  'X/editor/ags_notation_toolbar_callbacks.c',
  'X/editor/ags_pattern_envelope.c',
  'X/editor/ags_pattern_envelope_callbacks.c',
  'X/editor/ags_position_automation_cursor_dialog.c',
  'X/editor/ags_position_automation_cursor_dialog_callbacks.c',
  'X/editor/ags_position_notation_cursor_dialog.c',
  'X/editor/ags_position_notation_cursor_dialog_callbacks.c',
  'X/editor/ags_position_wave_cursor_dialog.c',
  'X/editor/ags_position_wave_cursor_dialog_callbacks.c',
  'X/editor/ags_ramp_acceleration_dialog.c',
  'X/editor/ags_ramp_acceleration_dialog_callbacks.c',
  'X/editor/ags_scrolled_automation_edit_box.c',
  'X/editor/ags_scrolled_wave_edit_box.c',
  'X/editor/ags_select_acceleration_dialog.c',
  'X/editor/ags_select_acceleration_dialog_callbacks.c',
  'X/editor/ags_select_buffer_dialog.c',
  'X/editor/ags_select_buffer_dialog_callbacks.c',
  'X/editor/ags_select_note_dialog.c',
  'X/editor/ags_select_note_dialog_callbacks.c',
  'X/editor/ags_sheet_edit.c',
  'X/editor/ags_sheet_edit_callbacks.c',
  'X/editor/ags_sheet_toolbar.c',
  'X/editor/ags_sheet_toolbar_callbacks.c',
  'X/editor/ags_time_stretch_buffer_dialog.c',
  'X/editor/ags_time_stretch_buffer_dialog_callbacks.c',
  'X/editor/ags_vautomation_edit_box.c',
  'X/editor/ags_vwave_edit_box.c',
  'X/editor/ags_wave_edit_box.c',
  'X/editor/ags_wave_edit.c',
  'X/editor/ags_wave_edit_callbacks.c',
  'X/editor/ags_wave_meta.c',
  'X/editor/ags_wave_meta_callbacks.c',
  'X/editor/ags_wave_toolbar.c',
  'X/editor/ags_wave_toolbar_callbacks.c',
  'X/export/ags_machine_collection.c',
  'X/export/ags_machine_collection_callbacks.c',
  'X/export/ags_machine_collection_entry.c',
  'X/export/ags_machine_collection_entry_callbacks.c',
  'X/export/ags_midi_export_wizard.c',
  'X/export/ags_midi_export_wizard_callbacks.c',
  'X/export/ags_wave_export_dialog.c',
  'X/export/ags_wave_export_dialog_callbacks.c',
  'X/file/ags_simple_file.c',
  'X/import/ags_midi_import_wizard.c',
  'X/import/ags_midi_import_wizard_callbacks.c',
  'X/import/ags_track_collection.c',
  'X/import/ags_track_collection_callbacks.c',
  'X/import/ags_track_collection_mapper.c',
  'X/import/ags_track_collection_mapper_callbacks.c',
  'X/machine/ags_audiorec.c',
  'X/machine/ags_audiorec_callbacks.c',
  'X/machine/ags_cell_pattern.c',
  'X/machine/ags_cell_pattern_callbacks.c',
  'X/machine/ags_desk.c',
  'X/machine/ags_desk_callbacks.c',
  'X/machine/ags_desk_file_chooser.c',
  'X/machine/ags_desk_pad.c',
  'X/machine/ags_desk_pad_callbacks.c',
  'X/machine/ags_drum.c',
  'X/machine/ags_drum_callbacks.c',
  'X/machine/ags_drum_input_line.c',
  'X/machine/ags_drum_input_line_callbacks.c',
  'X/machine/ags_drum_input_pad.c',
  'X/machine/ags_drum_input_pad_callbacks.c',
  'X/machine/ags_drum_output_line.c',
  'X/machine/ags_drum_output_line_callbacks.c',
  'X/machine/ags_drum_output_pad.c',
  'X/machine/ags_drum_output_pad_callbacks.c',
  'X/machine/ags_dssi_bridge.c',
  'X/machine/ags_dssi_bridge_callbacks.c',
  'X/machine/ags_equalizer10.c',
  'X/machine/ags_equalizer10_callbacks.c',
  'X/machine/ags_ffplayer_bridge.c',
  'X/machine/ags_ffplayer_bridge_callbacks.c',
  'X/machine/ags_ffplayer_bulk_input.c',
  'X/machine/ags_ffplayer_bulk_input_callbacks.c',
  'X/machine/ags_ffplayer.c',
  'X/machine/ags_ffplayer_callbacks.c',
  'X/machine/ags_ffplayer_input_line.c',
  'X/machine/ags_ffplayer_input_line_callbacks.c',
  'X/machine/ags_ffplayer_input_pad.c',
  'X/machine/ags_ffplayer_input_pad_callbacks.c',
  'X/machine/ags_fm_oscillator.c',
  'X/machine/ags_fm_oscillator_callbacks.c',
  'X/machine/ags_fm_syncsynth.c',
  'X/machine/ags_fm_syncsynth_callbacks.c',
  'X/machine/ags_fm_synth.c',
  'X/machine/ags_fm_synth_callbacks.c',
  'X/machine/ags_fm_synth_input_line.c',
  'X/machine/ags_fm_synth_input_line_callbacks.c',
  'X/machine/ags_fm_synth_input_pad.c',
  'X/machine/ags_hybrid_fm_synth.c',
  'X/machine/ags_hybrid_fm_synth_callbacks.c',
  'X/machine/ags_hybrid_synth.c',
  'X/machine/ags_hybrid_synth_callbacks.c',
  'X/machine/ags_ladspa_bridge.c',
  'X/machine/ags_ladspa_bridge_callbacks.c',
  'X/machine/ags_live_dssi_bridge.c',
  'X/machine/ags_live_dssi_bridge_callbacks.c',
  'X/machine/ags_live_lv2_bridge.c',
  'X/machine/ags_live_lv2_bridge_callbacks.c',
  'X/machine/ags_lv2_bridge.c',
  'X/machine/ags_lv2_bridge_callbacks.c',
  'X/machine/ags_matrix_bridge.c',
  'X/machine/ags_matrix_bridge_callbacks.c',
  'X/machine/ags_matrix_bulk_input.c',
  'X/machine/ags_matrix_bulk_input_callbacks.c',
  'X/machine/ags_matrix.c',
  'X/machine/ags_matrix_callbacks.c',
  'X/machine/ags_mixer.c',
  'X/machine/ags_mixer_callbacks.c',
  'X/machine/ags_mixer_input_line.c',
  'X/machine/ags_mixer_input_pad.c',
  'X/machine/ags_oscillator.c',
  'X/machine/ags_oscillator_callbacks.c',
  'X/machine/ags_panel.c',
  'X/machine/ags_panel_callbacks.c',
  'X/machine/ags_panel_input_line.c',
  'X/machine/ags_panel_input_line_callbacks.c',
  'X/machine/ags_panel_input_pad.c',
  'X/machine/ags_pattern_box.c',
  'X/machine/ags_pattern_box_callbacks.c',
  'X/machine/ags_pitch_sampler.c',
  'X/machine/ags_pitch_sampler_callbacks.c',
  'X/machine/ags_pitch_sampler_file.c',
  'X/machine/ags_pitch_sampler_file_callbacks.c',
  'X/machine/ags_sf2_synth.c',
  'X/machine/ags_sf2_synth_callbacks.c',
  'X/machine/ags_sfz_synth.c',
  'X/machine/ags_sfz_synth_callbacks.c',
  'X/machine/ags_spectrometer.c',
  'X/machine/ags_spectrometer_callbacks.c',
  'X/machine/ags_syncsynth.c',
  'X/machine/ags_syncsynth_callbacks.c',
  'X/machine/ags_synth.c',
  'X/machine/ags_synth_callbacks.c',
  'X/machine/ags_synth_input_line.c',
  'X/machine/ags_synth_input_line_callbacks.c',
  'X/machine/ags_synth_input_pad.c',
  'X/osc/controller/ags_ui_osc_renew_controller.c',
  'X/task/ags_simple_file_read.c',
  'X/task/ags_simple_file_write.c',
)

if get_option('vst3')
  libgsequencer_sources += files(
    'X/machine/ags_live_vst3_bridge.c',
    'X/machine/ags_live_vst3_bridge_callbacks.c',
    'X/machine/ags_vst3_bridge.c',
    'X/machine/ags_vst3_bridge_callbacks.c',
    'X/ags_vst3_browser.c',
    'X/ags_vst3_browser_callbacks.c',
  )
endif

libgsequencer_dependencies = [
  common_dependencies,
  gtk_dependency,
  soup_dependency,
  xml2_dependency,
#TODO: i think that shouldn't be dependency for X
  instpatch_dependency,
  dl_dependency,
]

if get_option('jack')
  libgsequencer_dependencies += jack_dependency
endif

if get_option('pulse')
  libgsequencer_dependencies += pulse_dependency
endif

libgsequencer_compiler_flags = compiler_flags
if get_option('poppler')
  libgsequencer_dependencies += poppler_dependency
endif

if get_option('webkit')
  libgsequencer_dependencies += webkit_dependency
endif

if get_option('poppler')
  libgsequencer_dependencies += poppler_dependency
endif

if get_option('gstreamer')
  libgsequencer_dependencies += gstreamer_dependency
  libgsequencer_dependencies += gstreamer_audio_dependency
  libgsequencer_dependencies += gstreamer_video_dependency
  libgsequencer_dependencies += gstreamer_app_dependency
  libgsequencer_dependencies += gstreamer_pbutils_dependency
endif

libgsequencer = library('gsequencer',
  libgsequencer_sources,
  c_args: libgsequencer_compiler_flags,
  include_directories: [includes],
  dependencies: libgsequencer_dependencies,
  link_with: [
    libags,
    libags_thread,
    libags_server,
    libags_audio,
    libags_gui,
  ],
  install: true,
)

gsequencer_dependencies = [
  common_dependencies,
  dependency('gtk+-3.0'),
  dependency('libsoup-2.4'),
  dependency('libinstpatch-1.0'),
#  compiler.find_library('crypt', required: true),
  ]

midi2xml_dependencies = [
  common_dependencies,
  dependency('libsoup-2.4'),
  dependency('libinstpatch-1.0'),
#  compiler.find_library('crypt', required: true),
  ]


if get_option('gstreamer')
  gsequencer_dependencies += gstreamer_dependency
  gsequencer_dependencies += gstreamer_audio_dependency
  gsequencer_dependencies += gstreamer_video_dependency
  gsequencer_dependencies += gstreamer_app_dependency
  gsequencer_dependencies += gstreamer_pbutils_dependency
  midi2xml_dependencies += gstreamer_dependency
  midi2xml_dependencies += gstreamer_audio_dependency
  midi2xml_dependencies += gstreamer_video_dependency
  midi2xml_dependencies += gstreamer_app_dependency
  midi2xml_dependencies += gstreamer_pbutils_dependency
endif

if get_option('jack')
  gsequencer_dependencies += jack_dependency
endif

if get_option('pulse')
  gsequencer_dependencies += pulse_dependency
endif

gsequencer_sources = files(
  'gsequencer_main.c'
)

gsequencer = executable(
  'gsequencer',
  gsequencer_sources,
  c_args: compiler_flags,
  include_directories: [includes],
  dependencies: [gsequencer_dependencies],
  link_with: [
    libags,
    libags_thread,
    libags_server,
    libags_audio,
    libags_gui,
    libgsequencer,
  ],
  install: true,
)

midi2xml_sources = files(
  'midi2xml_main.c'
)

midi2xml = executable(
  'midi2xml',
  midi2xml_sources,
  c_args: compiler_flags,
  include_directories: [includes],
  dependencies: [midi2xml_dependencies],
  link_with: [
    libags,
    libags_audio,
  ],
  install: true,
)

install_data(join_paths(meson.source_root(), 'gsequencer.share/styles/ags.css'),
             install_dir: join_paths(get_option('datadir'),
                                     meson.project_name(), 'styles'))

install_data(join_paths(meson.source_root(), 'gsequencer.share/images/ags.png'),
             install_dir: join_paths(get_option('datadir'),
                                     meson.project_name(), 'images'))
install_data(join_paths(meson.source_root(), 'gsequencer.share/images/gsequencer-800x450.png'),
             install_dir: join_paths(get_option('datadir'),
                                     meson.project_name(), 'images'))

libags_util_headers = [
  'util/ags_id_generator.h',
  'util/ags_destroy_util.h',
  'util/ags_soundcard_helper.h',
  'util/ags_list_util.h',
]

libags_lib_headers = [
  'lib/ags_function.h',
  'lib/ags_complex.h',
  'lib/ags_solver_vector.h',
  'lib/ags_math_util.h',
  'lib/ags_string_util.h',
  'lib/ags_endian.h',
  'lib/ags_log.h',
  'lib/ags_turtle_manager.h',
  'lib/ags_buffer_util.h',
  'lib/ags_time.h',
  'lib/ags_turtle.h',
  'lib/ags_uuid.h',
  'lib/ags_solver_polynomial.h',
  'lib/ags_regex.h',
  'lib/ags_conversion.h',
  'lib/ags_solver_matrix.h',
]

libags_object_headers = [
  'object/ags_connectable.h',
  'object/ags_soundcard.h',
  'object/ags_applicable.h',
  'object/ags_marshal.h',
  'object/ags_sequencer.h',
  'object/ags_tactable.h',
  'object/ags_main_loop.h',
  'object/ags_seekable.h',
  'object/ags_cursor.h',
  'object/ags_sound_server.h',
  'object/ags_countable.h',
  'object/ags_portlet.h',
  'object/ags_globals.h',
  'object/ags_application_context.h',
  'object/ags_priority.h',
  'object/ags_macros.h',
  'object/ags_mutable.h',
  'object/ags_plugin.h',
  'object/ags_config.h',
]

libags_file_headers = [
  'file/ags_file.h',
  'file/ags_file_launch.h',
  'file/ags_file_lookup.h',
  'file/ags_file_id_ref.h',
  'file/ags_file_link.h',
]

libags_headers = libags_util_headers + libags_lib_headers + libags_object_headers + libags_file_headers

libags_thread_thread_headers = [
  'thread/ags_concurrency_provider.h',
  'thread/ags_returnable_thread.h',
  'thread/ags_message_envelope.h',
  'thread/ags_generic_main_loop.h',
  'thread/ags_timestamp.h',
  'thread/ags_task.h',
  'thread/ags_message_delivery.h',
  'thread/ags_thread_pool.h',
  'thread/ags_thread_application_context.h',
  'thread/ags_thread.h',
  'thread/ags_worker_thread.h',
  'thread/ags_task_launcher.h',
  'thread/ags_destroy_worker.h',
  'thread/ags_message_queue.h',
  'thread/ags_task_completion.h',
]

libags_thread_headers = libags_thread_thread_headers

libags_server_server_headers = [
  'server/ags_registry.h',
  'server/ags_server.h',
  'server/ags_server_application_context.h',
  'server/ags_server_status.h',
  'server/ags_service_provider.h',
]

libags_server_server_security_headers = [
  'server/security/ags_auth_security_context.h',
  'server/security/ags_authentication.h',
  'server/security/ags_authentication_manager.h',
  'server/security/ags_business_group.h',
  'server/security/ags_business_group_manager.h',
  'server/security/ags_certificate.h',
  'server/security/ags_certificate_manager.h',
  'server/security/ags_password_store.h',
  'server/security/ags_password_store_manager.h',
  'server/security/ags_security_context.h',
  'server/security/ags_xml_authentication.h',
  'server/security/ags_xml_business_group.h',
  'server/security/ags_xml_certificate.h',
  'server/security/ags_xml_password_store.h',
]

libags_server_server_controller_headers = [
  'server/controller/ags_controller.h',
  'server/controller/ags_front_controller.h',
  'server/controller/ags_plugin_controller.h',
]

libags_server_headers = libags_server_server_headers + libags_server_server_security_headers + libags_server_server_controller_headers

libags_audio_plugin_headers = [
  'plugin/ags_base_plugin.h',
  'plugin/ags_dssi_manager.h',
  'plugin/ags_dssi_plugin.h',
  'plugin/ags_ladspa_conversion.h',
  'plugin/ags_ladspa_manager.h',
  'plugin/ags_ladspa_plugin.h',
  'plugin/ags_lv2_conversion.h',
  'plugin/ags_lv2_event_manager.h',
  'plugin/ags_lv2_log_manager.h',
  'plugin/ags_lv2_manager.h',
  'plugin/ags_lv2_option_manager.h',
  'plugin/ags_lv2_plugin.h',
  'plugin/ags_lv2_preset.h',
  'plugin/ags_lv2_preset_manager.h',
  'plugin/ags_lv2_turtle_parser.h',
  'plugin/ags_lv2_turtle_scanner.h',
  'plugin/ags_lv2_uri_map_manager.h',
  'plugin/ags_lv2_urid_manager.h',
  'plugin/ags_lv2_worker.h',
  'plugin/ags_lv2_worker_manager.h',
  'plugin/ags_lv2ui_manager.h',
  'plugin/ags_lv2ui_plugin.h',
  'plugin/ags_plugin_port.h',
  'plugin/ags_plugin_stock.h',
]

if get_option('vst3')
libags_audio_plugin_headers += [
    'plugin/ags_vst3_conversion.h',
    'plugin/ags_vst3_manager.h',
    'plugin/ags_vst3_plugin.h',
  ]
endif

libags_audio_audio_headers = [
  'audio/ags_acceleration.h',
  'audio/ags_audio.h',
  'audio/ags_audio_application_context.h',
  'audio/ags_audio_buffer_util.h',
  'audio/ags_audio_signal.h',
  'audio/ags_automation.h',
  'audio/ags_buffer.h',
  'audio/ags_channel.h',
  'audio/ags_char_buffer_util.h',
  'audio/ags_chorus_util.h',
  'audio/ags_devin.h',
  'audio/ags_devout.h',
  'audio/ags_diatonic_scale.h',
  'audio/ags_envelope_util.h',
  'audio/ags_fast_pitch_util.h',
  'audio/ags_fifoout.h',
  'audio/ags_filter_util.h',
  'audio/ags_fm_synth_util.h',
  'audio/ags_fluid_iir_filter_util.h',
  'audio/ags_fluid_interpolate_4th_order_util.h',
  'audio/ags_fluid_interpolate_7th_order_util.h',
  'audio/ags_fluid_interpolate_linear_util.h',
  'audio/ags_fluid_interpolate_none_util.h',
  'audio/ags_fluid_pitch_util.h',
  'audio/ags_fluid_util.h',
  'audio/ags_fourier_transform_util.h',
  'audio/ags_frequency_aliase_util.h',
  'audio/ags_frequency_map.h',
  'audio/ags_frequency_map_manager.h',
  'audio/ags_fx_factory.h',
  'audio/ags_generic_pitch_util.h',
  'audio/ags_generic_recall_channel_run.h',
  'audio/ags_generic_recall_recycling.h',
  'audio/ags_hq_pitch_util.h',
  'audio/ags_input.h',
  'audio/ags_linear_interpolate_util.h',
  'audio/ags_lfo_synth_util.h',
  'audio/ags_midi.h',
  'audio/ags_midiin.h',
  'audio/ags_noise_util.h',
  'audio/ags_notation.h',
  'audio/ags_note.h',
  'audio/ags_output.h',
  'audio/ags_pattern.h',
  'audio/ags_peak_util.h',
  'audio/ags_phase_shift_util.h',
  'audio/ags_playback.h',
  'audio/ags_playback_domain.h',
  'audio/ags_port.h',
  'audio/ags_port_util.h',
  'audio/ags_preset.h',
  'audio/ags_recall.h',
  'audio/ags_recall_audio.h',
  'audio/ags_recall_audio_run.h',
  'audio/ags_recall_audio_signal.h',
  'audio/ags_recall_channel.h',
  'audio/ags_recall_channel_run.h',
  'audio/ags_recall_container.h',
  'audio/ags_recall_dependency.h',
  'audio/ags_recall_dssi.h',
  'audio/ags_recall_dssi_run.h',
  'audio/ags_recall_factory.h',
  'audio/ags_recall_id.h',
  'audio/ags_recall_ladspa.h',
  'audio/ags_recall_ladspa_run.h',
  'audio/ags_recall_lv2.h',
  'audio/ags_recall_lv2_run.h',
  'audio/ags_recall_recycling.h',
  'audio/ags_recycling.h',
  'audio/ags_recycling_context.h',
  'audio/ags_resample_util.h',
  'audio/ags_sequencer_util.h',
  'audio/ags_sf2_synth_generator.h',
  'audio/ags_sf2_synth_util.h',
  'audio/ags_sf2_util.h',
  'audio/ags_sfz_synth_generator.h',
  'audio/ags_sfz_synth_util.h',
  'audio/ags_sfz_util.h',
  'audio/ags_sound_enums.h',
  'audio/ags_sound_provider.h',
  'audio/ags_soundcard_util.h',
  'audio/ags_synth_enums.h',
  'audio/ags_synth_generator.h',
  'audio/ags_synth_util.h',
  'audio/ags_time_stretch_util.h',
  'audio/ags_track.h',
  'audio/ags_volume_util.h',
  'audio/ags_wave.h',
]

libags_audio_audio_audio_unit_headers = [
  'audio/audio-unit/ags_audio_unit_client.h',
  'audio/audio-unit/ags_audio_unit_devin.h',
  'audio/audio-unit/ags_audio_unit_devout.h',
  'audio/audio-unit/ags_audio_unit_port.h',
  'audio/audio-unit/ags_audio_unit_server.h',
]

libags_audio_audio_core_audio_headers = [
  'audio/core-audio/ags_core_audio_client.h',
  'audio/core-audio/ags_core_audio_devin.h',
  'audio/core-audio/ags_core_audio_devout.h',
  'audio/core-audio/ags_core_audio_midiin.h',
  'audio/core-audio/ags_core_audio_port.h',
  'audio/core-audio/ags_core_audio_server.h',
]

libags_audio_audio_file_headers = [
  'audio/file/ags_audio_container.h',
  'audio/file/ags_audio_container_manager.h',
  'audio/file/ags_audio_file.h',
  'audio/file/ags_audio_file_link.h',
  'audio/file/ags_audio_file_manager.h',
  'audio/file/ags_ipatch.h',
  'audio/file/ags_ipatch_dls2_reader.h',
  'audio/file/ags_ipatch_gig_reader.h',
  'audio/file/ags_ipatch_sample.h',
  'audio/file/ags_ipatch_sf2_reader.h',
  'audio/file/ags_sfz_file.h',
  'audio/file/ags_sfz_group.h',
  'audio/file/ags_sfz_region.h',
  'audio/file/ags_sfz_sample.h',
  'audio/file/ags_sndfile.h',
  'audio/file/ags_sound_container.h',
  'audio/file/ags_sound_resource.h',
]

if get_option('gstreamer')
libags_audio_audio_file_gstreamer_headers = [
  'audio/file/ags_gstreamer_file.h',
]
endif

libags_audio_audio_fx_headers = [
  'audio/fx/ags_fx_analyse_audio.h',
  'audio/fx/ags_fx_analyse_audio_processor.h',
  'audio/fx/ags_fx_analyse_audio_signal.h',
  'audio/fx/ags_fx_analyse_channel.h',
  'audio/fx/ags_fx_analyse_channel_processor.h',
  'audio/fx/ags_fx_analyse_recycling.h',
  'audio/fx/ags_fx_buffer_audio.h',
  'audio/fx/ags_fx_buffer_audio_processor.h',
  'audio/fx/ags_fx_buffer_audio_signal.h',
  'audio/fx/ags_fx_buffer_channel.h',
  'audio/fx/ags_fx_buffer_channel_processor.h',
  'audio/fx/ags_fx_buffer_recycling.h',
  'audio/fx/ags_fx_chorus_audio.h',
  'audio/fx/ags_fx_chorus_audio_processor.h',
  'audio/fx/ags_fx_chorus_audio_signal.h',
  'audio/fx/ags_fx_chorus_channel.h',
  'audio/fx/ags_fx_chorus_channel_processor.h',
  'audio/fx/ags_fx_chorus_recycling.h',
  'audio/fx/ags_fx_dssi_audio.h',
  'audio/fx/ags_fx_dssi_audio_processor.h',
  'audio/fx/ags_fx_dssi_audio_signal.h',
  'audio/fx/ags_fx_dssi_channel.h',
  'audio/fx/ags_fx_dssi_channel_processor.h',
  'audio/fx/ags_fx_dssi_recycling.h',
  'audio/fx/ags_fx_envelope_audio.h',
  'audio/fx/ags_fx_envelope_audio_processor.h',
  'audio/fx/ags_fx_envelope_audio_signal.h',
  'audio/fx/ags_fx_envelope_channel.h',
  'audio/fx/ags_fx_envelope_channel_processor.h',
  'audio/fx/ags_fx_envelope_recycling.h',
  'audio/fx/ags_fx_eq10_audio.h',
  'audio/fx/ags_fx_eq10_audio_processor.h',
  'audio/fx/ags_fx_eq10_audio_signal.h',
  'audio/fx/ags_fx_eq10_channel.h',
  'audio/fx/ags_fx_eq10_channel_processor.h',
  'audio/fx/ags_fx_eq10_recycling.h',
  'audio/fx/ags_fx_fm_synth_audio.h',
  'audio/fx/ags_fx_fm_synth_audio_processor.h',
  'audio/fx/ags_fx_fm_synth_audio_signal.h',
  'audio/fx/ags_fx_fm_synth_channel.h',
  'audio/fx/ags_fx_fm_synth_channel_processor.h',
  'audio/fx/ags_fx_fm_synth_recycling.h',
  'audio/fx/ags_fx_high_pass_audio.h',
  'audio/fx/ags_fx_high_pass_audio_processor.h',
  'audio/fx/ags_fx_high_pass_audio_signal.h',
  'audio/fx/ags_fx_high_pass_channel.h',
  'audio/fx/ags_fx_high_pass_channel_processor.h',
  'audio/fx/ags_fx_high_pass_recycling.h',
  'audio/fx/ags_fx_ladspa_audio.h',
  'audio/fx/ags_fx_ladspa_audio_processor.h',
  'audio/fx/ags_fx_ladspa_audio_signal.h',
  'audio/fx/ags_fx_ladspa_channel.h',
  'audio/fx/ags_fx_ladspa_channel_processor.h',
  'audio/fx/ags_fx_ladspa_recycling.h',
  'audio/fx/ags_fx_lfo_audio.h',
  'audio/fx/ags_fx_lfo_audio_processor.h',
  'audio/fx/ags_fx_lfo_audio_signal.h',
  'audio/fx/ags_fx_lfo_channel.h',
  'audio/fx/ags_fx_lfo_channel_processor.h',
  'audio/fx/ags_fx_lfo_recycling.h',
  'audio/fx/ags_fx_low_pass_audio.h',
  'audio/fx/ags_fx_low_pass_audio_processor.h',
  'audio/fx/ags_fx_low_pass_audio_signal.h',
  'audio/fx/ags_fx_low_pass_channel.h',
  'audio/fx/ags_fx_low_pass_channel_processor.h',
  'audio/fx/ags_fx_low_pass_recycling.h',
  'audio/fx/ags_fx_lv2_audio.h',
  'audio/fx/ags_fx_lv2_audio_processor.h',
  'audio/fx/ags_fx_lv2_audio_signal.h',
  'audio/fx/ags_fx_lv2_channel.h',
  'audio/fx/ags_fx_lv2_channel_processor.h',
  'audio/fx/ags_fx_lv2_recycling.h',
  'audio/fx/ags_fx_notation_audio.h',
  'audio/fx/ags_fx_notation_audio_processor.h',
  'audio/fx/ags_fx_notation_audio_signal.h',
  'audio/fx/ags_fx_notation_channel.h',
  'audio/fx/ags_fx_notation_channel_processor.h',
  'audio/fx/ags_fx_notation_recycling.h',
  'audio/fx/ags_fx_pattern_audio.h',
  'audio/fx/ags_fx_pattern_audio_processor.h',
  'audio/fx/ags_fx_pattern_audio_signal.h',
  'audio/fx/ags_fx_pattern_channel.h',
  'audio/fx/ags_fx_pattern_channel_processor.h',
  'audio/fx/ags_fx_pattern_recycling.h',
  'audio/fx/ags_fx_peak_audio.h',
  'audio/fx/ags_fx_peak_audio_processor.h',
  'audio/fx/ags_fx_peak_audio_signal.h',
  'audio/fx/ags_fx_peak_channel.h',
  'audio/fx/ags_fx_peak_channel_processor.h',
  'audio/fx/ags_fx_peak_recycling.h',
  'audio/fx/ags_fx_playback_audio.h',
  'audio/fx/ags_fx_playback_audio_processor.h',
  'audio/fx/ags_fx_playback_audio_signal.h',
  'audio/fx/ags_fx_playback_channel.h',
  'audio/fx/ags_fx_playback_channel_processor.h',
  'audio/fx/ags_fx_playback_recycling.h',
  'audio/fx/ags_fx_sf2_synth_audio.h',
  'audio/fx/ags_fx_sf2_synth_audio_processor.h',
  'audio/fx/ags_fx_sf2_synth_audio_signal.h',
  'audio/fx/ags_fx_sf2_synth_channel.h',
  'audio/fx/ags_fx_sf2_synth_channel_processor.h',
  'audio/fx/ags_fx_sf2_synth_recycling.h',
  'audio/fx/ags_fx_sfz_synth_audio.h',
  'audio/fx/ags_fx_sfz_synth_audio_processor.h',
  'audio/fx/ags_fx_sfz_synth_audio_signal.h',
  'audio/fx/ags_fx_sfz_synth_channel.h',
  'audio/fx/ags_fx_sfz_synth_channel_processor.h',
  'audio/fx/ags_fx_sfz_synth_recycling.h',
  'audio/fx/ags_fx_synth_audio.h',
  'audio/fx/ags_fx_synth_audio_processor.h',
  'audio/fx/ags_fx_synth_audio_signal.h',
  'audio/fx/ags_fx_synth_channel.h',
  'audio/fx/ags_fx_synth_channel_processor.h',
  'audio/fx/ags_fx_synth_recycling.h',
  'audio/fx/ags_fx_two_pass_aliase_audio.h',
  'audio/fx/ags_fx_two_pass_aliase_audio_processor.h',
  'audio/fx/ags_fx_two_pass_aliase_audio_signal.h',
  'audio/fx/ags_fx_two_pass_aliase_channel.h',
  'audio/fx/ags_fx_two_pass_aliase_channel_processor.h',
  'audio/fx/ags_fx_two_pass_aliase_recycling.h',
  'audio/fx/ags_fx_volume_audio.h',
  'audio/fx/ags_fx_volume_audio_processor.h',
  'audio/fx/ags_fx_volume_audio_signal.h',
  'audio/fx/ags_fx_volume_channel.h',
  'audio/fx/ags_fx_volume_channel_processor.h',
  'audio/fx/ags_fx_volume_recycling.h',
]

if get_option('vst3')
libags_audio_audio_fx_headers += [
    'audio/fx/ags_fx_vst3_audio.h',
    'audio/fx/ags_fx_vst3_audio_processor.h',
    'audio/fx/ags_fx_vst3_audio_signal.h',
    'audio/fx/ags_fx_vst3_channel.h',
    'audio/fx/ags_fx_vst3_channel_processor.h',
    'audio/fx/ags_fx_vst3_recycling.h',
  ]
endif

libags_audio_audio_gstreamer_headers = [ ]

if get_option('gstreamer')
libags_audio_audio_gstreamer_headers = [
  'audio/gstreamer/ags_gstreamer_client.h',
  'audio/gstreamer/ags_gstreamer_devin.h',
  'audio/gstreamer/ags_gstreamer_devout.h',
  'audio/gstreamer/ags_gstreamer_port.h',
  'audio/gstreamer/ags_gstreamer_server.h',
]
endif

libags_audio_audio_alsa_headers = [
  'audio/alsa/ags_alsa_devin.h',
  'audio/alsa/ags_alsa_devout.h',
  'audio/alsa/ags_alsa_midiin.h',
]

libags_audio_audio_oss_headers = [
  'audio/oss/ags_oss_devin.h',
  'audio/oss/ags_oss_devout.h',
  'audio/oss/ags_oss_midiin.h',
]

libags_audio_audio_jack_headers = [
  'audio/jack/ags_jack_client.h',
  'audio/jack/ags_jack_devin.h',
  'audio/jack/ags_jack_devout.h',
  'audio/jack/ags_jack_midiin.h',
  'audio/jack/ags_jack_port.h',
  'audio/jack/ags_jack_server.h',
]

libags_audio_audio_midi_headers = [
  'audio/midi/ags_midi_buffer_util.h',
  'audio/midi/ags_midi_builder.h',
  'audio/midi/ags_midi_file.h',
  'audio/midi/ags_midi_parser.h',
  'audio/midi/ags_midi_util.h',
]

libags_audio_audio_osc_headers = [
  'audio/osc/ags_osc_buffer_util.h',
  'audio/osc/ags_osc_builder.h',
  'audio/osc/ags_osc_client.h',
  'audio/osc/ags_osc_connection.h',
  'audio/osc/ags_osc_message.h',
  'audio/osc/ags_osc_parser.h',
  'audio/osc/ags_osc_response.h',
  'audio/osc/ags_osc_server.h',
  'audio/osc/ags_osc_util.h',
  'audio/osc/ags_osc_websocket_connection.h',
  'audio/osc/ags_osc_xmlrpc_message.h',
  'audio/osc/ags_osc_xmlrpc_server.h',
]

libags_audio_audio_osc_controller_headers = [
  'audio/osc/controller/ags_osc_action_controller.h',
  'audio/osc/controller/ags_osc_config_controller.h',
  'audio/osc/controller/ags_osc_controller.h',
  'audio/osc/controller/ags_osc_export_controller.h',
  'audio/osc/controller/ags_osc_front_controller.h',
  'audio/osc/controller/ags_osc_info_controller.h',
  'audio/osc/controller/ags_osc_meter_controller.h',
  'audio/osc/controller/ags_osc_node_controller.h',
  'audio/osc/controller/ags_osc_plugin_controller.h',
  'audio/osc/controller/ags_osc_renew_controller.h',
  'audio/osc/controller/ags_osc_status_controller.h',
]

libags_audio_audio_osc_xmlrpc_headers = [
  'audio/osc/xmlrpc/ags_osc_xmlrpc_controller.h',
]

libags_audio_audio_pulse_headers = [
  'audio/pulse/ags_pulse_client.h',
  'audio/pulse/ags_pulse_devin.h',
  'audio/pulse/ags_pulse_devout.h',
  'audio/pulse/ags_pulse_port.h',
  'audio/pulse/ags_pulse_server.h',
]

libags_audio_audio_recall_headers = [
  'audio/recall/ags_analyse_audio_signal.h',
  'audio/recall/ags_analyse_channel.h',
  'audio/recall/ags_analyse_channel_run.h',
  'audio/recall/ags_analyse_recycling.h',
  'audio/recall/ags_buffer_audio_signal.h',
  'audio/recall/ags_buffer_channel.h',
  'audio/recall/ags_buffer_channel_run.h',
  'audio/recall/ags_buffer_recycling.h',
  'audio/recall/ags_capture_wave_audio.h',
  'audio/recall/ags_capture_wave_audio_run.h',
  'audio/recall/ags_capture_wave_channel.h',
  'audio/recall/ags_capture_wave_channel_run.h',
  'audio/recall/ags_copy_audio_signal.h',
  'audio/recall/ags_copy_channel.h',
  'audio/recall/ags_copy_channel_run.h',
  'audio/recall/ags_copy_pattern_audio.h',
  'audio/recall/ags_copy_pattern_audio_run.h',
  'audio/recall/ags_copy_pattern_channel.h',
  'audio/recall/ags_copy_pattern_channel_run.h',
  'audio/recall/ags_copy_recycling.h',
  'audio/recall/ags_count_beats_audio.h',
  'audio/recall/ags_count_beats_audio_run.h',
  'audio/recall/ags_delay_audio.h',
  'audio/recall/ags_delay_audio_run.h',
  'audio/recall/ags_envelope_audio_signal.h',
  'audio/recall/ags_envelope_channel.h',
  'audio/recall/ags_envelope_channel_run.h',
  'audio/recall/ags_envelope_recycling.h',
  'audio/recall/ags_eq10_audio_signal.h',
  'audio/recall/ags_eq10_channel.h',
  'audio/recall/ags_eq10_channel_run.h',
  'audio/recall/ags_eq10_recycling.h',
  'audio/recall/ags_feed_audio_signal.h',
  'audio/recall/ags_feed_channel.h',
  'audio/recall/ags_feed_channel_run.h',
  'audio/recall/ags_feed_recycling.h',
  'audio/recall/ags_lfo_audio_signal.h',
  'audio/recall/ags_lfo_channel.h',
  'audio/recall/ags_lfo_channel_run.h',
  'audio/recall/ags_lfo_recycling.h',
  'audio/recall/ags_loop_channel.h',
  'audio/recall/ags_loop_channel_run.h',
  'audio/recall/ags_mute_audio.h',
  'audio/recall/ags_mute_audio_run.h',
  'audio/recall/ags_mute_audio_signal.h',
  'audio/recall/ags_mute_channel.h',
  'audio/recall/ags_mute_channel_run.h',
  'audio/recall/ags_mute_recycling.h',
  'audio/recall/ags_peak_audio_signal.h',
  'audio/recall/ags_peak_channel.h',
  'audio/recall/ags_peak_channel_run.h',
  'audio/recall/ags_peak_recycling.h',
  'audio/recall/ags_play_audio.h',
  'audio/recall/ags_play_audio_signal.h',
  'audio/recall/ags_play_channel.h',
  'audio/recall/ags_play_channel_run.h',
  'audio/recall/ags_play_channel_run_master.h',
  'audio/recall/ags_play_dssi_audio.h',
  'audio/recall/ags_play_dssi_audio_run.h',
  'audio/recall/ags_play_lv2_audio.h',
  'audio/recall/ags_play_lv2_audio_run.h',
  'audio/recall/ags_play_notation_audio.h',
  'audio/recall/ags_play_notation_audio_run.h',
  'audio/recall/ags_play_recycling.h',
  'audio/recall/ags_play_wave_audio.h',
  'audio/recall/ags_play_wave_audio_run.h',
  'audio/recall/ags_play_wave_channel.h',
  'audio/recall/ags_play_wave_channel_run.h',
  'audio/recall/ags_prepare_audio_signal.h',
  'audio/recall/ags_prepare_channel.h',
  'audio/recall/ags_prepare_channel_run.h',
  'audio/recall/ags_prepare_recycling.h',
  'audio/recall/ags_record_midi_audio.h',
  'audio/recall/ags_record_midi_audio_run.h',
  'audio/recall/ags_route_dssi_audio.h',
  'audio/recall/ags_route_dssi_audio_run.h',
  'audio/recall/ags_route_lv2_audio.h',
  'audio/recall/ags_route_lv2_audio_run.h',
  'audio/recall/ags_rt_stream_audio_signal.h',
  'audio/recall/ags_rt_stream_channel.h',
  'audio/recall/ags_rt_stream_channel_run.h',
  'audio/recall/ags_rt_stream_recycling.h',
  'audio/recall/ags_stream_audio_signal.h',
  'audio/recall/ags_stream_channel.h',
  'audio/recall/ags_stream_channel_run.h',
  'audio/recall/ags_stream_recycling.h',
  'audio/recall/ags_volume_audio_signal.h',
  'audio/recall/ags_volume_channel.h',
  'audio/recall/ags_volume_channel_run.h',
  'audio/recall/ags_volume_recycling.h',
]

libags_audio_audio_task_headers = [
  'audio/task/ags_add_audio.h',
  'audio/task/ags_add_audio_signal.h',
  'audio/task/ags_add_effect.h',
  'audio/task/ags_add_note.h',
  'audio/task/ags_add_soundcard.h',
  'audio/task/ags_apply_bpm.h',
  'audio/task/ags_apply_presets.h',
  'audio/task/ags_apply_sequencer_length.h',
  'audio/task/ags_apply_sf2_midi_locale.h',
  'audio/task/ags_apply_sf2_synth.h',
  'audio/task/ags_apply_sfz_instrument.h',
  'audio/task/ags_apply_sfz_synth.h',
  'audio/task/ags_apply_sound_config.h',
  'audio/task/ags_apply_synth.h',
  'audio/task/ags_apply_tact.h',
  'audio/task/ags_cancel_audio.h',
  'audio/task/ags_cancel_channel.h',
  'audio/task/ags_clear_audio_signal.h',
  'audio/task/ags_clear_buffer.h',
  'audio/task/ags_crop_note.h',
  'audio/task/ags_export_output.h',
  'audio/task/ags_free_selection.h',
  'audio/task/ags_link_channel.h',
  'audio/task/ags_move_note.h',
  'audio/task/ags_open_file.h',
  'audio/task/ags_open_sf2_instrument.h',
  'audio/task/ags_open_sf2_sample.h',
  'audio/task/ags_open_sfz_file.h',
  'audio/task/ags_open_single_file.h',
  'audio/task/ags_open_wave.h',
  'audio/task/ags_remove_audio.h',
  'audio/task/ags_remove_audio_signal.h',
  'audio/task/ags_remove_note.h',
  'audio/task/ags_remove_soundcard.h',
  'audio/task/ags_reset_amplitude.h',
  'audio/task/ags_reset_fx_analyse.h',
  'audio/task/ags_reset_fx_peak.h',
  'audio/task/ags_reset_note.h',
  'audio/task/ags_reset_peak.h',
  'audio/task/ags_resize_audio.h',
  'audio/task/ags_seek_soundcard.h',
  'audio/task/ags_set_audio_channels.h',
  'audio/task/ags_set_buffer_size.h',
  'audio/task/ags_set_device.h',
  'audio/task/ags_set_format.h',
  'audio/task/ags_set_muted.h',
  'audio/task/ags_set_samplerate.h',
  'audio/task/ags_start_audio.h',
  'audio/task/ags_start_channel.h',
  'audio/task/ags_start_sequencer.h',
  'audio/task/ags_start_soundcard.h',
  'audio/task/ags_stop_sequencer.h',
  'audio/task/ags_stop_soundcard.h',
  'audio/task/ags_stop_thread.h',
  'audio/task/ags_switch_buffer_flag.h',
  'audio/task/ags_tic_device.h',
  'audio/task/ags_toggle_pattern_bit.h',
]

if get_option('vst3')
  libags_audio_audio_task_headers += [
    'audio/task/ags_write_vst3_port.h',
    'audio/task/ags_instantiate_vst3_plugin.h',
  ]
endif

libags_audio_audio_thread_headers = [
  'audio/thread/ags_audio_loop.h',
  'audio/thread/ags_audio_thread.h',
  'audio/thread/ags_channel_thread.h',
  'audio/thread/ags_export_thread.h',
  'audio/thread/ags_sequencer_thread.h',
  'audio/thread/ags_sf2_midi_locale_loader.h',
  'audio/thread/ags_sf2_loader.h',
  'audio/thread/ags_sfz_instrument_loader.h',
  'audio/thread/ags_sfz_loader.h',
  'audio/thread/ags_soundcard_thread.h',
  'audio/thread/ags_wave_loader.h',
]

libags_audio_audio_wasapi_headers = [
  'audio/wasapi/ags_wasapi_devin.h',
  'audio/wasapi/ags_wasapi_devout.h',
]

libags_audio_headers = libags_audio_plugin_headers + libags_audio_audio_headers

libags_audio_headers += libags_audio_audio_audio_unit_headers + libags_audio_audio_core_audio_headers + libags_audio_audio_alsa_headers + libags_audio_audio_oss_headers + libags_audio_audio_gstreamer_headers + libags_audio_audio_jack_headers + libags_audio_audio_pulse_headers

libags_audio_headers += libags_audio_audio_file_headers

libags_audio_headers += libags_audio_audio_midi_headers

libags_audio_headers += libags_audio_audio_osc_headers

libags_audio_headers += libags_audio_audio_fx_headers + libags_audio_audio_recall_headers

libags_audio_headers += libags_audio_audio_task_headers

if get_option('vst3')
  libags_vst_host_headers = [
    'vst3-capi/host/ags_vst_host_context.h'
  ]

  libags_vst_pluginterfaces_gui_headers = [
    'vst3-capi/pluginterfaces/gui/ags_vst_iplug_view_content_scale_support.h',
    'vst3-capi/pluginterfaces/gui/ags_vst_iplug_view.h'
  ]

  libags_vst_pluginterfaces_base_headers = [
    'vst3-capi/pluginterfaces/base/ags_vst_ucolor_spec.h',
    'vst3-capi/pluginterfaces/base/ags_vst_fstrdefs.h',
    'vst3-capi/pluginterfaces/base/ags_vst_ibstream.h',
    'vst3-capi/pluginterfaces/base/ags_vst_iupdate_handler.h',
    'vst3-capi/pluginterfaces/base/ags_vst_ftypes.h',
    'vst3-capi/pluginterfaces/base/ags_vst_ustring.h',
    'vst3-capi/pluginterfaces/base/ags_vst_fvariant.h',
    'vst3-capi/pluginterfaces/base/ags_vst_futils.h',
    'vst3-capi/pluginterfaces/base/ags_vst_geo_constants.h',
    'vst3-capi/pluginterfaces/base/ags_vst_iplugin_base.h',
    'vst3-capi/pluginterfaces/base/ags_vst_keycodes.h',
    'vst3-capi/pluginterfaces/base/ags_vst_icloneable.h',
    'vst3-capi/pluginterfaces/base/ags_vst_fplatform.h',
    'vst3-capi/pluginterfaces/base/ags_vst_const_string_table.h',
    'vst3-capi/pluginterfaces/base/ags_vst_ipersistent.h',
    'vst3-capi/pluginterfaces/base/ags_vst_funknown.h',
    'vst3-capi/pluginterfaces/base/ags_vst_smart_pointer.h',
    'vst3-capi/pluginterfaces/base/ags_vst_ierror_context.h',
    'vst3-capi/pluginterfaces/base/ags_vst_istring_result.h'
  ]

  libags_vst_pluginterfaces_vst_headers = [
    'vst3-capi/pluginterfaces/vst/ags_vst_iattributes.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_iaudio_processor.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_iautomation_state.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_ichannel_context_info.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_icomponent.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_icontext_menu.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_ievents.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_iedit_controller.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_ihost_application.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_iinter_app_audio.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_imessage.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_imidi_controllers.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_imidi_learn.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_inote_expression.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_iparameter_function_name.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_iparameter_changes.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_iphysical_ui.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_iplug_view.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_iprocess_context.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_iplug_interface_support.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_iprefetchable_support.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_irepresentation.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_iunits.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_speaker.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_types.h',
    'vst3-capi/pluginterfaces/vst/ags_vst_preset_keys.h'
  ]

  libags_vst_public_sdk_source_common_headers = [
    'vst3-capi/public.sdk/source/common/ags_vst_plugin_view.h',
  ]

  libags_vst_public_sdk_source_vst_headers = [
    'vst3-capi/public.sdk/source/vst/ags_vst_bus.h',
    'vst3-capi/public.sdk/source/vst/ags_vst_parameters.h',
    'vst3-capi/public.sdk/source/vst/ags_vst_edit_controller.h',
    'vst3-capi/public.sdk/source/vst/ags_vst_component_base.h',
    'vst3-capi/public.sdk/source/vst/ags_vst_component.h',
    'vst3-capi/public.sdk/source/vst/ags_vst_audio_effect.h',
  ]

  libags_vst_public_sdk_source_vst_hosting_headers = [
    'vst3-capi/public.sdk/source/vst/hosting/ags_vst_event_list.h',
  ]
  
  libags_vst_base_source_headers = [
    'vst3-capi/base/source/ags_vst_fobject.h',
    'vst3-capi/base/source/ags_vst_fbuffer.h',
    'vst3-capi/base/source/ags_vst_fstring.h'
  ]

  libags_vst_util_headers = [
    'vst3-capi/util/ags_vst_string_util.h'
  ]

  libags_vst_headers = libags_vst_host_headers + libags_vst_pluginterfaces_gui_headers + libags_vst_pluginterfaces_base_headers + libags_vst_pluginterfaces_vst_headers + libags_vst_public_sdk_source_common_headers + libags_vst_public_sdk_source_vst_headers + libags_vst_public_sdk_source_vst_hosting_headers + libags_vst_base_source_headers + libags_vst_util_headers

  install_headers(libags_vst_pluginterfaces_gui_headers, install_dir: gsequencer_includedir / 'ags/vst3-capi/pluginterfaces/gui/')
  install_headers(libags_vst_pluginterfaces_base_headers, install_dir: gsequencer_includedir / 'ags/vst3-capi/pluginterfaces/base/')
  install_headers(libags_vst_pluginterfaces_vst_headers, install_dir: gsequencer_includedir / 'ags/vst3-capi/pluginterfaces/vst/')
  install_headers(libags_vst_public_sdk_source_common_headers, install_dir: gsequencer_includedir / 'ags/vst3-capi/public.sdk/source/common/')
  install_headers(libags_vst_public_sdk_source_vst_headers, install_dir: gsequencer_includedir / 'ags/vst3-capi/public.sdk/source/vst/')
  install_headers(libags_vst_base_source_headers, install_dir: gsequencer_includedir / 'ags/vst3-capi/base/source/vst/')
  install_headers(libags_vst_util_headers, install_dir: gsequencer_includedir / 'ags/vst3-capi/util/')

endif

libags_gui_widget_headers = [
  'widget/ags_dial.h',
  'widget/ags_vlevel_box.h',
  'widget/ags_level_box.h',
  'widget/ags_hlevel_box.h',
  'widget/ags_scale_box.h',
  'widget/ags_indicator.h',
  'widget/ags_vled_array.h',
  'widget/ags_hindicator.h',
  'widget/ags_level.h',
  'widget/ags_ruler.h',
  'widget/ags_hscale_box.h',
  'widget/ags_vscale_box.h',
  'widget/ags_scrolled_level_box.h',
  'widget/ags_widget_marshal.h',
  'widget/ags_piano_keys.h',
  'widget/ags_scale.h',
  'widget/ags_scrolled_scale_box.h',
  'widget/ags_expander_set.h',
  'widget/ags_cartesian.h',
  'widget/ags_container.h',
  'widget/ags_expander.h',
  'widget/ags_vindicator.h',
  'widget/ags_led_array.h',
  'widget/ags_led.h',
  'widget/ags_scrolled_piano.h',
  'widget/ags_hled_array.h',
  'widget/ags_notebook.h',
  'widget/ags_piano.h',
  'widget/ags_viewport.h',
]

libags_gui_headers = libags_gui_widget_headers

install_headers(libags_util_headers, install_dir: gsequencer_includedir / 'ags/util')
install_headers(libags_lib_headers, install_dir: gsequencer_includedir / 'ags/lib')
install_headers(libags_object_headers, install_dir: gsequencer_includedir / 'ags/object')
install_headers(libags_file_headers, install_dir: gsequencer_includedir / 'ags/file')
install_headers(libags_thread_thread_headers, install_dir: gsequencer_includedir / 'ags/thread')
install_headers(libags_server_server_headers, install_dir: gsequencer_includedir / 'ags/server')
install_headers(libags_server_server_controller_headers, install_dir: gsequencer_includedir / 'ags/server/controller')
install_headers(libags_server_server_security_headers, install_dir: gsequencer_includedir / 'ags/server/security')

install_headers(libags_audio_plugin_headers, install_dir: gsequencer_includedir / 'ags/plugin')
install_headers(libags_audio_audio_headers, install_dir: gsequencer_includedir / 'ags/audio')
install_headers(libags_audio_audio_audio_unit_headers, install_dir: gsequencer_includedir / 'ags/audio/audio-unit')
install_headers(libags_audio_audio_core_audio_headers, install_dir: gsequencer_includedir / 'ags/audio/core-audio')
install_headers(libags_audio_audio_file_headers, install_dir: gsequencer_includedir / 'ags/audio/file')

if get_option('gstreamer')
install_headers(libags_audio_audio_file_gstreamer_headers, install_dir: gsequencer_includedir / 'ags/audio/file')
endif

install_headers(libags_audio_audio_fx_headers, install_dir: gsequencer_includedir / 'ags/audio/fx')

if get_option('gstreamer')
  install_headers(libags_audio_audio_gstreamer_headers, install_dir: gsequencer_includedir / 'ags/audio/gstreamer')
endif

install_headers(libags_audio_audio_alsa_headers, install_dir: gsequencer_includedir / 'ags/audio/alsa')
install_headers(libags_audio_audio_oss_headers, install_dir: gsequencer_includedir / 'ags/audio/oss')
install_headers(libags_audio_audio_jack_headers, install_dir: gsequencer_includedir / 'ags/audio/jack')
install_headers(libags_audio_audio_midi_headers, install_dir: gsequencer_includedir / 'ags/audio/midi')
install_headers(libags_audio_audio_osc_headers, install_dir: gsequencer_includedir / 'ags/audio/osc')
install_headers(libags_audio_audio_osc_controller_headers, install_dir: gsequencer_includedir / 'ags/audio/osc/controller')
install_headers(libags_audio_audio_osc_xmlrpc_headers, install_dir: gsequencer_includedir / 'ags/audio/osc/xmlrpc')
install_headers(libags_audio_audio_pulse_headers, install_dir: gsequencer_includedir / 'ags/audio/pulse')
install_headers(libags_audio_audio_recall_headers, install_dir: gsequencer_includedir / 'ags/audio/recall')
install_headers(libags_audio_audio_task_headers, install_dir: gsequencer_includedir / 'ags/audio/task')
install_headers(libags_audio_audio_thread_headers, install_dir: gsequencer_includedir / 'ags/audio/thread')
install_headers(libags_audio_audio_wasapi_headers, install_dir: gsequencer_includedir / 'ags/audio/wasapi')

if get_option('vst3')
  install_headers(libags_audio_audio_gstreamer_headers, install_dir: gsequencer_includedir / 'ags/audio/gstreamer')
endif

install_headers(libags_gui_widget_headers, install_dir: gsequencer_includedir / 'ags/widget')

install_headers('libags.h', 'libags-audio.h', 'libags-gui.h', install_dir: gsequencer_includedir / 'ags')

libags_dep_sources = []
libags_audio_dep_sources = []
libags_gui_dep_sources = []

gir = find_program('g-ir-scanner', required : get_option('introspection'))
build_gir = gir.found() and (not meson.is_cross_build() or get_option('introspection').enabled())

if build_gir
  gir_args = [
    '--quiet',
  
  ]
  libags_gir = gnome.generate_gir(
    [ libags, libags_thread, libags_server ],
    sources: libags_sources + libags_headers + libags_thread_sources + libags_thread_headers + libags_server_sources + libags_server_headers,
    namespace: 'Ags',
    nsversion: '3.0',
    identifier_prefix: 'Ags',
    symbol_prefix: 'ags_',
    export_packages: 'Ags',
    includes: [ 'Soup-2.4', 'GObject-2.0', 'libxml2-2.0', ],
    header: 'libags.h',
    install: true,
    extra_args: gir_args,
  )
  libags_gir_dep = declare_dependency(sources: libags_gir)
  libags_dep_sources += libags_gir

  libags_audio_gir = gnome.generate_gir(
    libags_audio,
    sources: libags_audio_sources + libags_audio_headers,
    namespace: 'AgsAudio',
    nsversion: '3.0',
    identifier_prefix: 'Ags',
    symbol_prefix: 'ags_',
    export_packages: 'AgsAudio',
    includes: [ 'Gst-1.0', 'GstBase-1.0', 'GstAudio-1.0', 'Soup-2.4', 'GObject-2.0', 'libxml2-2.0', ],
    header: 'libags-audio.h',
    install: true,
    dependencies: libags_gir_dep,
    extra_args: gir_args,
  )
  libags_audio_gir_dep = declare_dependency(sources: libags_audio_gir)
  libags_audio_dep_sources += libags_audio_gir

  libags_gui_gir = gnome.generate_gir(
    libags_gui,
    sources: libags_gui_sources + libags_gui_headers,
    namespace: 'AgsGui',
    nsversion: '3.0',
    identifier_prefix: 'Ags',
    symbol_prefix: 'ags_',
    export_packages: 'AgsGui',
    includes: [ 'Gtk-3.0', ],
    header: 'libags-gui.h',
    install: true,
    extra_args: gir_args,
  )
  libags_gui_gir_dep = declare_dependency(sources: libags_gui_gir)
  libags_gui_dep_sources += libags_gui_gir
endif

if gsequencer_debug
  subdir('test')
endif
