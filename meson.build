project('gsequencer', 'c',
        license: 'GPL3',
        default_options: ['c_std=gnu99'],
        version: '3.5.8')

compiler = meson.get_compiler('c')
# TODO: all dependencies have to be defined here globally
gtk_dependency = dependency('gtk+-3.0')
glib_dependency = dependency('glib-2.0', version: '>= 2.58.0')
cunit_dependency = dependency('cunit')
xml2_dependency = dependency('libxml-2.0')
soup_dependency = dependency('libsoup-2.4')
instpatch_dependency = dependency('libinstpatch-1.0')
samplerate_dependency = dependency('samplerate')
fftw3_dependency = dependency('fftw3')
if get_option('alsa')
  alsa_dependency = dependency('alsa')
endif
if get_option('jack')
  jack_dependency = dependency('jack')
endif
if get_option('poppler')
  poppler_dependency = dependency('poppler-glib')
endif
if get_option('webkit')
  webkit_dependency = dependency('webkit2gtk-4.0')
endif
m_dependency = compiler.find_library('m', required: true)
dl_dependency = compiler.find_library('dl', required: true)
crypt_dependency =   compiler.find_library('crypt', required: true)

#destdir = get_option('DESTDIR')
add_project_arguments('-DDESTDIR="@0@"'.format(get_option('prefix')),
                      language:'c')

includes = include_directories(
  '.'
  )

subdir('po')
subdir('ags')
