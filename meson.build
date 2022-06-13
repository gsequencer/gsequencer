# Copyright (C) 2005-2022 Daniel Maksymow, Joël Krähemann
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

project('gsequencer', 'c', 'cpp',
        license: 'GPL3',
        default_options: [ 'buildtype=release', 'c_std=gnu99'],
        meson_version: '>=0.53.0',
        version: '4.2.4')

#note:jk: specify --buildtype=debug if needed
gsequencer_debug = get_option('buildtype').contains('debug')

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
# REMINDER: all dependencies have to be defined here globally
# Just to prevent double check
common_dependencies = []
gtk_dependency = dependency('gtk4')
glib_dependency = dependency('glib-2.0', version: '>= 2.56.0')
gobject_dependency = dependency('gobject-2.0', version: '>= 2.56.0')
gthread_dependency = dependency('gthread-2.0', version: '>= 2.56.0')
gio_dependency = dependency('gio-2.0', version: '>= 2.56.0')
xml2_dependency = dependency('libxml-2.0')
soup_dependency = dependency('libsoup-3.0')
instpatch_dependency = dependency('libinstpatch-1.0')
samplerate_dependency = dependency('samplerate')
fftw3_dependency = dependency('fftw3')
if get_option('alsa')
  alsa_dependency = dependency('alsa')
  compiler.check_header('ladspa.h', required: true)
  dssi_dependency = dependency('dssi')
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
if get_option('gtk_unix_print')
  poppler_dependency = dependency('gtk4-unix-print')
endif
if get_option('gstreamer')
  gstreamer_dependency = dependency('gstreamer-1.0')
  gstreamer_audio_dependency = dependency('gstreamer-audio-1.0')
  gstreamer_video_dependency = dependency('gstreamer-video-1.0')
  gstreamer_app_dependency = dependency('gstreamer-app-1.0')
  gstreamer_pbutils_dependency = dependency('gstreamer-pbutils-1.0')
endif
if gsequencer_debug
  cunit_dependency = dependency('cunit')
endif
m_dependency = compiler.find_library('m', required: true)
dl_dependency = compiler.find_library('dl', required: true)
crypt_dependency = compiler.find_library('crypt', required: true)
uuid_dependency = compiler.find_library('uuid', required: true)
lv2_dependency = dependency('lv2')

includes = include_directories(
  '.'
  )

subdir('po')
subdir('ags')
subdir('docs/reference/')

install_data(join_paths(meson.source_root(), 'COPYING'),
             install_dir: join_paths(get_option('datadir'),
                                     meson.project_name(), 'license'),
	     rename: 'GPL-3')

install_data(join_paths(meson.source_root(), 'gsequencer.share', 'icons', 'ags.xpm'),
             install_dir: join_paths(get_option('datadir'),
                                     meson.project_name(), 'icons'))
install_data(join_paths(meson.source_root(), 'gsequencer.share', 'icons', 'ags.png'),
             install_dir: join_paths(get_option('datadir'),
                                     meson.project_name(), 'icons'))
install_data(join_paths(meson.source_root(), 'gsequencer.share', 'icons', 'jumper.png'),
             install_dir: join_paths(get_option('datadir'),
                                     meson.project_name(), 'icons'))

install_data(join_paths(meson.source_root(), 'gsequencer.share', 'icons', 'gsequencer-32x32.png'),
             install_dir: join_paths(get_option('datadir'),
                                     'icons', 'hicolor', '32x32', 'apps'),
	     rename: 'gsequencer.png')
install_data(join_paths(meson.source_root(), 'gsequencer.share', 'icons', 'gsequencer-48x48.png'),
             install_dir: join_paths(get_option('datadir'),
                                     'icons', 'hicolor', '48x48', 'apps'),
	     rename: 'gsequencer.png')
install_data(join_paths(meson.source_root(), 'gsequencer.share', 'icons', 'gsequencer-64x64.png'),
             install_dir: join_paths(get_option('datadir'),
                                     'icons', 'hicolor', '64x64', 'apps'),
	     rename: 'gsequencer.png')
install_data(join_paths(meson.source_root(), 'gsequencer.share', 'icons', 'gsequencer-128x128.png'),
             install_dir: join_paths(get_option('datadir'),
                                     'icons', 'hicolor', '128x128', 'apps'),
	     rename: 'gsequencer.png')

install_data(join_paths(meson.source_root(), 'gsequencer.share', 'icons', 'ags-light.svg'),
             install_dir: join_paths(get_option('datadir'),
                                     'icons', 'hicolor', 'scalable', 'apps'),
	     rename: 'gsequencer.svg')

install_data(join_paths(meson.source_root(), 'org.nongnu.gsequencer.gsequencer.appdata.xml'),
             install_dir: join_paths(get_option('datadir'),
                                     'metainfo'))

install_data(join_paths(meson.source_root(), 'application-x-gsequencer.xml'),
             install_dir: join_paths(get_option('datadir'),
                                     'mime', 'packages'),
             rename: 'gsequencer.xml')

install_data(join_paths(meson.source_root(), 'ags', 'app', 'file', 'ags_simple_file.dtd'),
             install_dir: join_paths(get_option('datadir'),
                                     'xml', 'gsequencer', 'schema', 'dtd', meson.project_version()))	     
install_data(join_paths(meson.source_root(), 'ags', 'audio', 'midi', 'ags_midi_file.dtd'),
             install_dir: join_paths(get_option('datadir'),
                                     'xml', 'gsequencer', 'schema', 'dtd', meson.project_version()))
install_data(join_paths(meson.source_root(), 'ags', 'audio', 'osc', 'ags_osc_file.dtd'),
             install_dir: join_paths(get_option('datadir'),
                                     'xml', 'gsequencer', 'schema', 'dtd', meson.project_version()))

install_data(join_paths(meson.source_root(), 'ags.xsl'),
             install_dir: join_paths(get_option('datadir'),
                                     'xml', 'gsequencer', 'stylesheet', 'ags-xsl', 'midi-xml'))
install_data(join_paths(meson.source_root(), 'ags-simple.xsl'),
             install_dir: join_paths(get_option('datadir'),
                                     'xml', 'gsequencer', 'stylesheet', 'ags-xsl', 'midi-xml'))

pkgconfig_conf_data = configuration_data()
pkgconfig_conf_data.set('prefix', gsequencer_prefix)
pkgconfig_conf_data.set('exec_prefix', '${prefix}')
pkgconfig_conf_data.set('libdir', '${prefix}/lib')
pkgconfig_conf_data.set('includedir', '${prefix}/include')
pkgconfig_conf_data.set('PACKAGE_VERSION', meson.project_version())

configure_file(input: 'libags.pc.in', output: 'libags.pc',
  configuration: pkgconfig_conf_data,
  install: true,
  install_dir: gsequencer_libdir / 'pkgconfig')

configure_file(input: 'libags_audio.pc.in', output: 'libags_audio.pc',
  configuration: pkgconfig_conf_data,
  install: true,
  install_dir: gsequencer_libdir / 'pkgconfig')

configure_file(input: 'libags_gui.pc.in', output: 'libags_gui.pc',
  configuration: pkgconfig_conf_data,
  install: true,
  install_dir: gsequencer_libdir / 'pkgconfig')

configure_file(input: 'libgsequencer.pc.in', output: 'libgsequencer.pc',
  configuration: pkgconfig_conf_data,
  install: true,
  install_dir: gsequencer_libdir / 'pkgconfig')

desktop_conf_data = configuration_data()
desktop_conf_data.set('datadir', gsequencer_datadir)

configure_file(input: 'gsequencer.desktop.in', output: 'gsequencer.desktop',
  configuration: desktop_conf_data,
  install: true,
  install_dir: gsequencer_datadir / 'applications')

xsltproc = find_program('xsltproc', required: false)
python_exe = find_program('python3', 'python', required: false)

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
else
install_man(join_paths(meson.source_root(), 'gsequencer.1'))
install_man(join_paths(meson.source_root(), 'midi2xml.1'))
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

if get_option('online_help') and xsltproc.found() and python_exe.found()
  env = environment()

  user_manual_dir = join_paths(meson.project_build_root(), 'html/user-manual/')
  developer_manual_dir = join_paths(meson.project_build_root(), 'html/developer-manual/')
  osc_manual_dir = join_paths(meson.project_build_root(), 'html/osc-manual/')

  listing_files = [
    'add_pattern.c',
    'application_mutex.c',
    'audio.c',
    'audio_application_context.c',
    'complete_example.c',
    'config.c',
    'effects_echo_audio.c',
    'effects_echo_audio_run.c',
    'effects_echo_channel.c',
    'effects_echo_channel_run.c',
    'effects_recall_container.c',
    'file_property.c',
    'file_read.c',
    'file_write.c',
    'find_port.c',
    'fx_factory.c',
    'linking_prerequisites.c',
    'linking_safe.c',
    'linking_unsafe.c',
    'notation_clipboard.c',
    'pcm_info.c',
    'port.c',
    'pull_thread.c',
    'recycling.c',
    'start_thread.c',
    'thread_application_context.c',
    'thread_obj_mutex.c',
  ]

  listing_script = join_paths(meson.source_root(), 'listing_script.py')

  foreach listing: listing_files
    source = '@0@/docs/listings/@1@.in'.format(meson.source_root(), listing)
    destination = '@0@/docs/listings/@1@'.format(meson.source_root(), listing)

    run_command(python_exe, listing_script, source, destination)
  endforeach

  run_target('ags-user-docs', command: [ xsltproc, '--nonet', '--xinclude', '--stringparam', 'html.stylesheet', '"custom.css"', '--stringparam', 'htmlhelp.show.menu', '"1"', '--stringparam', 'suppress.navigation', '"0"', '-o', user_manual_dir,  'http://docbook.sourceforge.net/release/xsl/current/htmlhelp/htmlhelp.xsl', 'docs/usersBook.xml' ])
  run_target('ags-developer-docs', command: [ xsltproc, '--nonet', '--xinclude', '--stringparam', 'html.stylesheet', '"custom.css"', '--stringparam', 'htmlhelp.show.menu', '"1"', '--stringparam', 'suppress.navigation', '"0"', '-o', developer_manual_dir, 'http://docbook.sourceforge.net/release/xsl/current/htmlhelp/htmlhelp.xsl', 'docs/developersBook.xml' ])
  run_target('ags-osc-docs', command: [ xsltproc, '--nonet', '--xinclude', '--stringparam', 'html.stylesheet', '"custom.css"', '--stringparam', 'htmlhelp.show.menu', '"1"', '--stringparam', 'suppress.navigation', '"0"', '-o', osc_manual_dir, 'http://docbook.sourceforge.net/release/xsl/current/htmlhelp/htmlhelp.xsl', 'docs/oscBook.xml' ])

  install_subdir(meson.project_build_root() / 'html/user-manual', install_dir: gsequencer_docdir)
  install_subdir(meson.project_build_root() / 'html/developer-manual', install_dir: gsequencer_docdir)
  install_subdir(meson.project_build_root() / 'html/osc-manual', install_dir: gsequencer_docdir)

  install_data(['docs/custom.css'], install_dir: gsequencer_docdir / 'user-manual')
  install_data(['docs/custom.css'], install_dir: gsequencer_docdir / 'developer-manual')
  install_data(['docs/custom.css'], install_dir: gsequencer_docdir / 'osc-manual')

  install_data(user_doc_images, install_dir: gsequencer_docdir / 'images')
  install_data(user_doc_images_adwaita, install_dir: gsequencer_docdir / 'images/adwaita')
  install_data(developer_doc_images, install_dir: gsequencer_docdir / 'images')
endif
