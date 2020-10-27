# Copyright (C) 2020 Daniel Maksymow
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

static_tests = [
  'ags_file_id_ref_test',
  'ags_file_launch_test',
  'ags_file_lookup_test',
  'ags_file_test',
]

static_test_dependencies = [
  common_test_dependencies,
  xml2_dependency,
  soup_dependency,
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
      libfile,
      liblib,
      libobject,
      libutil,
    ]
  )

  test(static_test, static_test_bin)
endforeach
