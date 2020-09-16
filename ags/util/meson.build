util_sources = files(
  'ags_destroy_util.c',
  'ags_id_generator.c',
  'ags_list_util.c',
  'ags_soundcard_helper.c',
)

util_dependencies = [
  common_dependencies,
]

libutil = static_library('util',
                        util_sources,
                        c_args: compiler_flags,
                        include_directories: [includes],
                        dependencies: util_dependencies,
                       )

