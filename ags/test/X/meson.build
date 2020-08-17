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

sources = [
  'ags_functional_test_util.c',
  'gsequencer_setup_util.c',
]

libtest = static_library(
  'testutils',
  sources,
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
      libtest,
      libobject,
      libthread,
      libx,
      libaudio,
      liblib,
      libutil,
      libplugin,
      libserver,
      libfile,
      libwidget,
      ]
    )
endforeach
