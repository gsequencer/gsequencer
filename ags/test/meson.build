interactive_tests = [
  'cartesian',
  'polynomals_test',
  'scale_test',
]

interactive_test_dependencies = [
  common_dependencies,
  dependency('gtk+-3.0'),
  dependency('libxml-2.0'),
  dependency('libsoup-2.4'),
  compiler.find_library('m', required: true),
]

foreach interactive_test : interactive_tests
  source = ['@0@.c'.format(interactive_test)]
  executable(
    interactive_test,
    source,
    c_args: compiler_flags,
    include_directories: [includes],
    dependencies: [interactive_test_dependencies],
    link_with: [
      libwidget,
    ]
  )
endforeach

#TODO: move this to environment variables
srcdir_def = '-DSRCDIR="@0@"'.format(meson.source_root())

common_test_dependencies = [
  common_dependencies,
  cunit_dependency,
]

subdir('X')
subdir('audio')
subdir('file')
subdir('lib')
subdir('object')
subdir('plugin')
subdir('server')
subdir('thread')
