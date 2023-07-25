# Copyright (C) 2020 Daniel Maksymow
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

# TODO: fix multiple definition in tests
static_tests = [
  'ags_base_plugin_test',
#  'ags_dssi_manager_test',
  'ags_dssi_plugin_test',
  'ags_ladspa_conversion_test',
#  'ags_ladspa_manager_test',
  'ags_lv2_conversion_test',
#  'ags_lv2_manager_test',
  'ags_lv2_option_manager_test',
  'ags_lv2_plugin_test',
  'ags_lv2_preset_test',
#  'ags_lv2ui_manager_test',
  'ags_lv2ui_plugin_test',
  'ags_lv2_urid_manager_test',
  'ags_lv2_uri_map_manager_test',
  'ags_lv2_worker_manager_test',
  'ags_plugin_port_test',
]

static_test_dependencies = [
  common_test_dependencies,
  xml2_dependency,
  soup_dependency,
  instpatch_dependency,
  json_glib_dependency,
  dl_dependency,
  m_dependency,
]

if get_option('gstreamer')
  static_test_dependencies += gstreamer_dependency
  static_test_dependencies += gstreamer_audio_dependency
  static_test_dependencies += gstreamer_video_dependency
  static_test_dependencies += gstreamer_app_dependency
  static_test_dependencies += gstreamer_pbutils_dependency
endif

foreach static_test : static_tests
  source = ['@0@.c'.format(static_test)]
  if static_test.contains('/')
    tmp = static_test.split('/')
    static_test = '_'.join(tmp)
  endif

  static_test_bin = executable(
    static_test,
    source,
    c_args: [compiler_flags, srcdir_def],
    include_directories: [includes],
    dependencies: [static_test_dependencies],
    link_with: [
      libags,
      libags_thread,
      libags_server,
      libags_audio,
    ]
  )

  test(static_test, static_test_bin)
endforeach

