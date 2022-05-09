# Copyright (C) 2020 Daniel Maksymow
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

configure_file(output: 'config.h', configuration: ags_config)

interactive_test_dependencies = [
  common_test_dependencies,
  dependency('gtk4'),
  xml2_dependency,
  soup_dependency,
  instpatch_dependency,
  dependency('cairo-xlib'),
  dependency('xtst'),
  compiler.find_library('m', required: true),
]

if get_option('gstreamer')
  interactive_test_dependencies += gstreamer_dependency
  interactive_test_dependencies += gstreamer_audio_dependency
  interactive_test_dependencies += gstreamer_video_dependency
  interactive_test_dependencies += gstreamer_app_dependency
  interactive_test_dependencies += gstreamer_pbutils_dependency
endif

libgsequencer_test_sources = [
  'ags_functional_test_util.c',
  'gsequencer_setup_util.c',
]

libgsequencer_test = static_library(
  'libgsequencer_test',
  libgsequencer_test_sources,
  c_args: [compiler_flags, srcdir_def],
  include_directories: [includes],
  dependencies: [interactive_test_dependencies],
)

interactive_tests = [
]

foreach interactive_test : interactive_tests
  source = ['@0@.c'.format(interactive_test)]
  if interactive_test.contains('/')
    tmp = interactive_test.split('/')
    interactive_test = '_'.join(tmp)
  endif

  interactive_test_bin = executable(
    interactive_test,
    source,
    c_args: [compiler_flags, srcdir_def],
    include_directories: [includes],
    dependencies: [interactive_test_dependencies],
    link_with: [
      libags,
      libags_thread,
      libags_server,
      libags_audio,
      libgsequencer,
      libgsequencer_test,
      ]
    )

  test(interactive_test, interactive_test_bin, is_parallel : false)
endforeach
