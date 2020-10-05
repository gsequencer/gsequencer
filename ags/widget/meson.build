# Copyright (C) 2020 Daniel Maksymow
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

widget_sources = files(
  'ags_cartesian.c',
  'ags_container.c',
  'ags_dial.c',
  'ags_expander.c',
  'ags_expander_set.c',
  'ags_hindicator.c',
  'ags_hled_array.c',
  'ags_hlevel_box.c',
  'ags_hscale_box.c',
  'ags_indicator.c',
  'ags_led_array.c',
  'ags_led.c',
  'ags_level_box.c',
  'ags_level.c',
  'ags_notebook.c',
  'ags_piano.c',
  'ags_ruler.c',
  'ags_scale_box.c',
  'ags_scale.c',
  'ags_scrolled_level_box.c',
  'ags_scrolled_piano.c',
  'ags_scrolled_scale_box.c',
  'ags_vindicator.c',
  'ags_vled_array.c',
  'ags_vlevel_box.c',
  'ags_vscale_box.c',
  'ags_widget_marshal.c',
)

widget_dependencies = [
  common_dependencies,
  dependency('gtk+-3.0'),
]

libwidget = static_library('widget',
                        widget_sources,
                        c_args: compiler_flags,
                        include_directories: [includes],
                        dependencies: widget_dependencies,
                       )
