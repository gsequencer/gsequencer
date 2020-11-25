# Copyright (C) 2020 Daniel Maksymow
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

configure_file(output: 'config.h', configuration: ags_config)

interactive_test_dependencies = [
  common_test_dependencies,
  dependency('gtk+-3.0'),
  xml2_dependency,
  soup_dependency,
  instpatch_dependency,
  dependency('cairo-xlib'),
  dependency('xtst'),
  compiler.find_library('m', required: true),
]

if get_option('gstreamer')
  interactive_test_dependencies += gstreamer_dependency + gstreamer_audio_dependency + gstreamer_video_dependency + gstreamer_app_dependency + gstreamer_pbutils_dependency
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
  'ags_functional_audio_config_test',
  'ags_functional_automation_editor_workflow_test',
  'ags_functional_line_member_add_and_destroy_test',
  'ags_functional_machine_add_and_destroy_test',
  'ags_functional_machine_link_test',
  'ags_functional_notation_editor_workflow_test',
  'ags_functional_notation_edit_test',
  'ags_xorg_application_context_test',
  'machine/ags_functional_drum_test',
  'machine/ags_functional_ffplayer_test',
  'machine/ags_functional_matrix_test',
  'machine/ags_functional_mixer_test',
  'machine/ags_functional_panel_test',
  'machine/ags_functional_synth_test',
]

foreach interactive_test : interactive_tests
  source = ['@0@.c'.format(interactive_test)]
  if interactive_test.contains('/')
    tmp = interactive_test.split('/')
    interactive_test = '_'.join(tmp)
  endif

  executable(
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
endforeach
