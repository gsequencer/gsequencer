# Copyright (C) 2020 Daniel Maksymow
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

project('gsequencer', 'c',
        license: 'GPL3',
        default_options: ['c_std=gnu99'],
        version: '3.6.23')

compiler = meson.get_compiler('c')
# TODO: all dependencies have to be defined here globally
gtk_dependency = dependency('gtk+-3.0')
glib_dependency = dependency('glib-2.0', version: '>= 2.58.0')
gobject_dependency = dependency('gobject-2.0', version: '>= 2.58.0')
gthread_dependency = dependency('gthread-2.0', version: '>= 2.58.0')
gio_dependency = dependency('gio-2.0', version: '>= 2.58.0')
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
if get_option('gstreamer')
  gstreamer_dependency = dependency('gstreamer-1.0')
  gstreamer_audio_dependency = dependency('gstreamer-audio-1.0')
  gstreamer_video_dependency = dependency('gstreamer-video-1.0')
  gstreamer_app_dependency = dependency('gstreamer-app-1.0')
  gstreamer_pbutils_dependency = dependency('gstreamer-pbutils-1.0')
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

