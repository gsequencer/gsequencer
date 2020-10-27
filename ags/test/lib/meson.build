# Copyright (C) 2020 Daniel Maksymow
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

static_tests = [
  'ags_buffer_util_test',
  'ags_complex_test',
  'ags_conversion_test',
  'ags_function_test',
  'ags_log_test',
  'ags_math_util_test',
  'ags_solver_matrix_test',
  'ags_solver_polynomial_test',
  'ags_solver_vector_test',
  'ags_time_test',
  'ags_turtle_manager_test',
  'ags_turtle_test',
]

static_test_dependencies = [
  common_test_dependencies,
  xml2_dependency,
  soup_dependency,
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
    c_args: [compiler_flags],
    include_directories: [includes],
    dependencies: [static_test_dependencies],
    link_with: [
      liblib,
      libobject,
    ]
  )

  test(static_test, static_test_bin)
endforeach

