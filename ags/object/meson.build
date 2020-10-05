# Copyright (C) 2020 Daniel Maksymow
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

configure_file(output: 'config.h', configuration: ags_config)

object_sources = files(
  'ags_applicable.c',
  'ags_application_context.c',
  'ags_config.c',
  'ags_connectable.c',
  'ags_countable.c',
  'ags_cursor.c',
  'ags_globals.c',
  'ags_main_loop.c',
  'ags_marshal.c',
  'ags_mutable.c',
  'ags_plugin.c',
  'ags_portlet.c',
  'ags_priority.c',
  'ags_seekable.c',
  'ags_sequencer.c',
  'ags_soundcard.c',
  'ags_sound_server.c',
  'ags_tactable.c',
)

object_dependencies = [
  common_dependencies,
  dependency('libxml-2.0'),
]

libobject = static_library('object',
                        object_sources,
                        c_args: compiler_flags,
                        include_directories: [includes],
                        dependencies: object_dependencies,
                       )

