configure_file(output: 'config.h', configuration: ags_config)

file_sources = files(
  'ags_file.c',
  'ags_file_id_ref.c',
  'ags_file_launch.c',
  'ags_file_link.c',
  'ags_file_lookup.c',
)

file_dependencies = [
  common_dependencies,
  dependency('libxml-2.0'),
]

libfile = static_library('file',
                        file_sources,
                        c_args: compiler_flags,
                        include_directories: [includes],
                        dependencies: file_dependencies,
                       )
