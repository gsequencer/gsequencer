# Copyright (C) 2020 Daniel Maksymow
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

sources = []

dynamic_library_suffix = 'unknown'
if host_machine.system() == 'linux'
  dynamic_library_suffix = '.so'
else
  error('add support to other os systems')
endif
define_template = '-D@0@="@1@"'
# TODO: this is needed only by plugin
add_project_arguments(define_template.format('AGS_LIBRARY_SUFFIX',
                                             dynamic_library_suffix),
                      language: 'c')

ags_config = configuration_data({
  'AGS_WITH_W32API' : get_option('w32api'),
  'AGS_WITH_OSXAPI' : get_option('osxapi'),
  'AGS_WITH_ALSA' : get_option('alsa'),
  'AGS_WITH_OSS' : get_option('oss'),
  'AGS_WITH_CORE_AUDIO' : get_option('core_audio'),
  'AGS_WITH_JACK' : get_option('jack'),
  'AGS_WITH_PULSE' : get_option('pulse'),
  'AGS_WITH_WASAPI' : get_option('wasapi'),
  'AGS_WITH_LIBINSTPATCH' : false,
  'AGS_WITH_QUARTZ' : get_option('quartz'),
  'AGS_WITH_WEBKIT' : get_option('webkit'),
  'AGS_WITH_POPPLER' : get_option('poppler'),
                                 })

# TODO: it looks like this must be mandatory
ags_config.set('AGS_WITH_LIBINSTPATCH', true)

configure_file(output: 'config.h', configuration: ags_config)

common_dependencies = [
  glib_dependency,
]

compiler_flags = []
# TODO: watch out!
fix_me_and_disable = true
if fix_me_and_disable
  compiler_flags += '-w'
endif

subdir('object')
subdir('X')
subdir('audio')
subdir('thread')
subdir('lib')
subdir('util')
subdir('plugin')
subdir('server')
subdir('file')
subdir('widget')

gsequencer_dependencies = [
  common_dependencies,
  dependency('gtk+-3.0'),
  dependency('libsoup-2.4'),
  dependency('libinstpatch-1.0'),
#  compiler.find_library('crypt', required: true),
  ]

sources = files(
  'gsequencer_main.c'
)

gsequencer = executable(
  meson.project_name(),
  sources,
  c_args: compiler_flags,
  include_directories: [includes],
  dependencies: [gsequencer_dependencies],
  link_with: [
    liblib,
    libobject,
    libx,
    libaudio,
    libthread,
    libutil,
    libplugin,
    libfile,
    libserver,
    libwidget,
  ],
  install: true,
)

install_data(join_paths(meson.source_root(), 'gsequencer.share/styles/ags.css'),
             install_dir: join_paths(get_option('datadir'),
                                     meson.project_name(), 'styles'))

subdir('test')
