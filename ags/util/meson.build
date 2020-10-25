# Copyright (C) 2020 Daniel Maksymow
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

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

