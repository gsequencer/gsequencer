# Copyright (C) 2020 Daniel Maksymow
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

configure_file(output: 'config.h', configuration: ags_config)

lib_sources = files(
  'ags_buffer_util.c',
  'ags_complex.c',
  'ags_conversion.c',
  'ags_endian.c',
  'ags_function.c',
  'ags_log.c',
  'ags_math_util.c',
  'ags_regex.c',
  'ags_solver_matrix.c',
  'ags_solver_polynomial.c',
  'ags_solver_vector.c',
  'ags_string_util.c',
  'ags_time.c',
  'ags_turtle.c',
  'ags_turtle_manager.c',
  'ags_uuid.c',
)

lib_dependencies = [
  common_dependencies,
  dependency('libxml-2.0'),
  compiler.find_library('uuid', required: true),
  ]

liblib = static_library('lib',
                        lib_sources,
                        c_args: compiler_flags,
                        include_directories: [includes],
                        dependencies: lib_dependencies,
                       )
