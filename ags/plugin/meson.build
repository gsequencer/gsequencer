# Copyright (C) 2020 Daniel Maksymow
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

configure_file(output: 'config.h', configuration: ags_config)

plugin_sources = files(
  'ags_base_plugin.c',
  'ags_dssi_manager.c',
  'ags_dssi_plugin.c',
  'ags_ladspa_conversion.c',
  'ags_ladspa_manager.c',
  'ags_ladspa_plugin.c',
  'ags_lv2_conversion.c',
  'ags_lv2_event_manager.c',
  'ags_lv2_log_manager.c',
  'ags_lv2_manager.c',
  'ags_lv2_option_manager.c',
  'ags_lv2_plugin.c',
  'ags_lv2_preset.c',
  'ags_lv2_preset_manager.c',
  'ags_lv2_turtle_parser.c',
  'ags_lv2_turtle_scanner.c',
  'ags_lv2ui_manager.c',
  'ags_lv2ui_plugin.c',
  'ags_lv2_urid_manager.c',
  'ags_lv2_uri_map_manager.c',
  'ags_lv2_worker.c',
  'ags_lv2_worker_manager.c',
  'ags_plugin_port.c',
)

plugin_dependencies = [
  common_dependencies,
  dependency('libxml-2.0'),
  dependency('libsoup-2.4'),
]

libplugin = static_library('plugin',
                        plugin_sources,
                        c_args: compiler_flags,
                        include_directories: [includes],
                        dependencies: plugin_dependencies,
                       )
