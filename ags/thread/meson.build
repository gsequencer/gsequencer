configure_file(output: 'config.h', configuration: ags_config)

thread_sources = files(
  'ags_concurrency_provider.c',
  'ags_destroy_worker.c',
  'ags_generic_main_loop.c',
  'ags_message_delivery.c',
  'ags_message_envelope.c',
  'ags_message_queue.c',
  'ags_returnable_thread.c',
  'ags_task.c',
  'ags_task_completion.c',
  'ags_task_launcher.c',
  'ags_thread_application_context.c',
  'ags_thread.c',
  'ags_thread_pool.c',
  'ags_timestamp.c',
  'ags_worker_thread.c',
)

thread_dependencies = [
  common_dependencies,
  dependency('libxml-2.0'),
]

libthread = static_library('thread',
                        thread_sources,
                        c_args: compiler_flags,
                        include_directories: [includes],
                        dependencies: thread_dependencies,
                       )

