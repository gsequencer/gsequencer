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
  dl_dependency,
  m_dependency,
]

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
      liblib,
      libobject,
      libaudio,
      libutil,
      libthread,
      libplugin,
      libserver,
      libfile,
    ]
  )

  test(static_test, static_test_bin)
endforeach

