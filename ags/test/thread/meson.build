# Copyright (C) 2020 Daniel Maksymow
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

static_tests = [
  'ags_concurrency_provider_test',
  'ags_destroy_worker_test',
#  'ags_functional_thread_test', TODO: missing header?
  'ags_generic_main_loop_test',
  'ags_message_delivery_test',
  'ags_message_envelope_test',
  'ags_message_queue_test',
  'ags_returnable_thread_test',
  'ags_task_test',
  'ags_task_launcher_test',
  'ags_thread_application_context_test',
  'ags_thread_pool_test',
  'ags_thread_test',
  'ags_timestamp_test',
  'ags_worker_thread_test',
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
      libags,
      libags_thread,
    ]
  )

  test(static_test, static_test_bin)
endforeach

