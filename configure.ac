#                                               -*- Autoconf -*-
# Copyright (C) 2005-2023 Joel Kraehemann
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([gsequencer],[5.3.11],[jkraehemann@gmail.com])
AM_INIT_AUTOMAKE([subdir-objects -Wno-portability])
AC_CONFIG_SRCDIR([ags/config.h.in])
AC_CONFIG_HEADERS([ags/config.h])
AC_CONFIG_HEADERS([ags/ags_api_config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_USE_SYSTEM_EXTENSIONS([_GNU_SOURCE])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.8])

# options
AC_ARG_VAR(INTROSPECTION_SCANNER, introspection scanner)
AC_ARG_VAR(INTROSPECTION_COMPILER, introspection compiler)

AC_ARG_VAR(HTMLHELP_XSL, path to Docbook XML htmlhelp XSL file)
AC_ARG_VAR(FO_XSL, path to Docbook XML fo XSL file)

AC_ARG_VAR(GOBJECT_API_DOC, path for use with sed to GObject API Reference Manual)
AC_ARG_VAR(GTK_API_DOC, path for use with sed to Gtk-4 API Reference Manual)
AC_ARG_VAR(LIBAGS_API_DOC, path for use with sed to Libags API Reference Manual)
AC_ARG_VAR(LIBAGS_AUDIO_API_DOC, path for use with sed to Libags-audio API Reference Manual)
AC_ARG_VAR(LIBAGS_GUI_API_DOC, path for use with sed to Libags-gui API Reference Manual)

AC_ARG_VAR(LIBAGS_API_DIR, install path of Libags API Reference Manual)
AC_ARG_VAR(LIBAGS_AUDIO_API_DIR, install path of Libags-audio API Reference Manual)
AC_ARG_VAR(LIBAGS_GUI_API_DIR, install path of Libags-gui API Reference Manual)
AC_ARG_VAR(LIBGSEQUENCER_API_DIR, install path of Libgsequencer API Reference Manual)

AC_ARG_ENABLE(debug, [AS_HELP_STRING([--enable-debug],
                                     [enable debug symbols (default is no)])],
              [],
              [enable_debug=no])

AC_ARG_ENABLE(devel, [AS_HELP_STRING([--enable-devel],
                                     [enable devel mode (default is no)])],
              [],
              [enable_devel=no])

AC_ARG_ENABLE(introspection, [AS_HELP_STRING([--enable-introspection],
                                             [enable introspection symbols (default is no)])],
              [],
              [enable_introspection=no])

AC_ARG_ENABLE(public-libgsequencer, [AS_HELP_STRING([--enable-public-libgsequencer],
                                                    [provide public libgsequencer (default is yes)])],
              [],
              [enable_public_libgsequencer=yes])

AC_ARG_ENABLE(upstream-gtk-doc, [AS_HELP_STRING([--enable-upstream-gtk-doc],
                                                [put API reference manual in upstream dedicated directory (default is yes)])],
              [],
              [enable_upstream_gtk_doc=yes])

AC_ARG_ENABLE(single-docdir, [AS_HELP_STRING([--enable-single-docdir],
                                             [put API reference manual in upstream dedicated directory (default is no)])],
              [],
              [enable_single_docdir=no])

AC_ARG_ENABLE(strip-rt-events, [AS_HELP_STRING([--enable-strip-rt-events],
                                               [strip rt events (default is yes)])],
              [],
              [enable_strip_rt_events=yes])

AC_ARG_ENABLE(rt, [AS_HELP_STRING([--enable-rt],
                                  [enable rt (default is yes)])],
              [],
              [enable_rt=yes])

AC_ARG_ENABLE(w32api, [AS_HELP_STRING([--enable-w32api],
                                      [enable w32 API (default is no)])],
              [],
              [enable_w32api=no])

AC_ARG_ENABLE(w32-exe-env, [AS_HELP_STRING([--enable-w32-exe-env],
                     	                    [enable w32 exe environment (default is no)])],
              [],
              [enable_w32_exe_env=no])

AC_ARG_ENABLE(osxapi, [AS_HELP_STRING([--enable-osxapi],
                                      [enable OS X API (default is no)])],
              [],
              [enable_osxapi=no])

AC_ARG_ENABLE(osx-dmg-env, [AS_HELP_STRING([--enable-osx-dmg-env],
                                      [enable OS X DMG environment (default is no)])],
              [],
              [enable_osx_dmg_env=no])

AC_ARG_ENABLE(alsa, [AS_HELP_STRING([--enable-alsa],
                                    [enable alsa (default is yes)])],
              [],
              [enable_alsa=yes])

AC_ARG_ENABLE(alsa-compat, [AS_HELP_STRING([--enable-alsa-compat],
                                    [enable alsa compatibility library (default is no)])],
              [],
              [enable_alsa_compat=no])

AC_ARG_ENABLE(oss, [AS_HELP_STRING([--enable-oss],
                                   [enable oss (default is no)])],
              [],
              [enable_oss=no])

AC_ARG_ENABLE(jack, [AS_HELP_STRING([--enable-jack],
                                    [enable jack (default is yes)])],
              [],
              [enable_jack=yes])

AC_ARG_ENABLE(pulse, [AS_HELP_STRING([--enable-pulse],
                                     [enable pulse (default is yes)])],
              [],
              [enable_pulse=yes])

AC_ARG_ENABLE(core-audio, [AS_HELP_STRING([--enable-core-audio],
                                          [enable core audio (default is no)])],
              [],
              [enable_core_audio=no])

AC_ARG_ENABLE(audio-unit, [AS_HELP_STRING([--enable-audio-unit],
                                          [enable audio unit (default is no)])],
              [],
              [enable_audio_unit=no])

AC_ARG_ENABLE(wasapi, [AS_HELP_STRING([--enable-wasapi],
                                      [enable WASAPI (default is no)])],
              [],
              [enable_wasapi=no])

AC_ARG_ENABLE(libinstpatch, [AS_HELP_STRING([--enable-libinstpatch],
                                            [enable libinstpatch (default is yes)])],
              [],
              [enable_libinstpatch=yes])

AC_ARG_ENABLE(vst3, [AS_HELP_STRING([--enable-vst3],
                                    [enable vst3 (default is no)])],
              [],
              [enable_vst3=no])

AC_ARG_ENABLE(midi2, [AS_HELP_STRING([--enable-midi2],
                                     [enable MIDI 2.0 (default is yes)])],
              [],
              [enable_midi2=no])

AC_ARG_ENABLE(pulse, [AS_HELP_STRING([--enable-pulse],
                                     [enable pulse (default is yes)])],
              [],
              [enable_pulse=yes])

AC_ARG_ENABLE(x11, [AS_HELP_STRING([--enable-x11],
                                   [enable X11 (default is yes)])],
              [],
              [enable_x11=yes])

AC_ARG_ENABLE(quartz, [AS_HELP_STRING([--enable-quartz],
                                      [enable quartz (default is no)])],
              [],
              [enable_quartz=no])

AC_ARG_ENABLE(run-functional-tests, [AS_HELP_STRING([--enable-run-functional-tests],
                                    [run functional tests (default is no)])],
              [],
              [enable_run_functional_tests=no])

AC_ARG_ENABLE(fast-test-mode, [AS_HELP_STRING([--enable-fast-test-mode],
                              [fast test mode (default is no)])],
              [],
              [enable_fast_test_mode=no])

AC_ARG_ENABLE(run-system-tests, [AS_HELP_STRING([--enable-run-system-tests],
                                [run system tests (default is no)])],
              [],
              [enable_run_system_tests=no])

AC_ARG_ENABLE(experimental, [AS_HELP_STRING([--enable-experimental],
                                [experimental (default is no)])],
              [],
              [enable_experimental=no])

AC_ARG_WITH([gtk_unix_print],
            [AS_HELP_STRING([--with-gtk-unix-print],
                            [enable support for Gtk4 Unix print])],
            [],
            [with_gtk_unix_print=no])

AC_ARG_WITH([poppler],
            [AS_HELP_STRING([--with-poppler],
                            [enable support for poppler])],
            [],
            [with_poppler=no])

AC_ARG_WITH([gstreamer],
            [AS_HELP_STRING([--without-gstreamer],
                            [disable support for gstreamer])],
            [],
            [with_gstreamer=yes])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
# AC_PROG_RANLIB

PKG_PROG_PKG_CONFIG([0.19])

AC_PATH_PROG([XSLTPROC], [xsltproc])
if test -z "$XSLTPROC"; then
   AC_MSG_WARN([xsltproc not found])
else
   AC_MSG_NOTICE([xsltproc found])
fi

AC_PATH_PROG([FOP], [fop])
if test -z "$FOP"; then
   AC_MSG_WARN([fop not found])
else
   AC_MSG_NOTICE([fop found])
fi


gl_EARLY

# libtool
LT_PREREQ([2.4.2])
LT_INIT([win32-dll])
AC_SUBST([LIBTOOL_DEPS])
AC_ENABLE_SHARED

AC_SUBST([GSEQUENCER_CFLAGS])
AC_SUBST([GSEQUENCER_CXXFLAGS])
AC_SUBST([GSEQUENCER_LDFLAGS])

AC_SUBST([MIDI2XML_CFLAGS])
AC_SUBST([MIDI2XML_CXXFLAGS])
AC_SUBST([MIDI2XML_LDFLAGS])

AC_SUBST([LIBAGS_CPPFLAGS])
AC_SUBST([LIBAGS_THREAD_CPPFLAGS])
AC_SUBST([LIBAGS_SERVER_CPPFLAGS])
AC_SUBST([LIBAGS_AUDIO_CPPFLAGS])
AC_SUBST([LIBAGS_GUI_CPPFLAGS])
AC_SUBST([LIBGSEQUENCER_CPPFLAGS])

AC_SUBST([LIBGSEQUENCER_TEST_LIBADD])

AC_SUBST([GSEQUENCER_FUNCTIONAL_TEST_LDADD])

# Checks for libraries.
# FIXME: Replace 'main' with a function in '-lcrypt':
AC_CHECK_LIB([crypt], [main])
# FIXME: Replace 'main' with a function in '-ldl':
AC_CHECK_LIB([dl], [main])
# FIXME: Replace 'main' with a function in '-lgnu':
AC_CHECK_LIB([gnu], [main])
# FIXME: Replace 'main' with a function in '-lm':
AC_CHECK_LIB([m], [main])
# FIXME: Replace 'main' with a function in '-lrt':
AC_CHECK_LIB([rt], [main])
# FIXME: Replace 'main' with a function in '-luuid':
AC_CHECK_LIB([uuid], [main])

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lX11':
AS_IF([test "x$enable_x11" != xno],
      [AC_DEFINE([AGS_WITH_X11], [1], [X11 enabled])
       AC_CHECK_LIB([X11], [main], , AC_MSG_ERROR("No X11 library present"))
       X11_LIBS="-lX11"],
      [AC_MSG_NOTICE([X11 disabled])])

AS_IF([test "x$enable_quartz" != xno],
      [AC_DEFINE([AGS_WITH_QUARTZ], [1], [quartz enabled])
       AC_MSG_NOTICE([quartz enabled])],
      [AC_MSG_NOTICE([quartz disabled])])

# FIXME: Replace `main' with a function in `-ldl':
# AC_CHECK_LIB([dl], [main], , AC_MSG_ERROR("No dl library present"))

# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main], , AC_MSG_ERROR("No math library present"))

AS_IF([test "x$enable_strip_rt_events" != xno],
      [AC_DEFINE([AGS_STRIP_RT_EVENTS], [1], [striping rt-events enabled])
      AC_MSG_NOTICE([striping rt-events enabled])],
      [AC_MSG_NOTICE([striping rt-events disabled])])

# FIXME: Replace `main' with a function in `-lrt':
AS_IF([test "x$enable_rt" != xno],
      [AC_DEFINE([AGS_WITH_RT], [1], [rt enabled])
      RT_LIBS="-lrt"
      AC_CHECK_LIB([rt], [main], , AC_MSG_ERROR("No rt library present"))
      AC_MSG_NOTICE([rt enabled])],
      [AC_MSG_NOTICE([rt disabled])])

# Checks for header files.
AC_PATH_X
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([arpa/inet.h fcntl.h libintl.h mach/mach.h netinet/in.h stdint.h strings.h sys/ioctl.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UID_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

WITH_W32API=0

AS_IF([test "x$enable_w32api" != xno],
      [AC_DEFINE([AGS_W32API], [1], [w32api enabled])
#      W32API_CFLAGS="-nostdinc -I./lib -I/usr/lib/gcc/x86_64-w64-mingw32/7.4.0/include -I/usr/x86_64-w64-mingw32/sys-root/mingw/include -I/usr/x86_64-w64-mingw32/sys-root/mingw/include/dssi -include ags/config.h "
#      W32API_LIBS="-L./lib -lgnu"
      AC_SUBST(W32API_CFLAGS)
      AC_SUBST(W32API_LIBS)
      AC_MSG_NOTICE([w32api enabled])],
      [AC_MSG_NOTICE([w32api disabled])])
AM_CONDITIONAL([WITH_W32API], [test "x$enable_w32api" == xyes])

AS_IF([test "x$enable_w32_exe_env" != xno],
      [AC_DEFINE([AGS_W32_EXE_ENV], [1], [w32 exe environment enabled])
      AC_MSG_NOTICE([w32 exe environment enabled])],
      [AC_MSG_NOTICE([w32 exe environment disabled])])
AM_CONDITIONAL([WITH_W32_EXE_ENV], [test "x$enable_w32_exe_env" == xyes])

AS_IF([test "x$enable_osxapi" != xno],
      [AC_DEFINE([AGS_OSXAPI], [1], [OS X api enabled])
      AC_SUBST(OSXAPI_CFLAGS)
      AC_SUBST(OSXAPI_LIBS)
      AC_MSG_NOTICE([OS X api enabled])],
      [AC_MSG_NOTICE([OS X api disabled])])
AM_CONDITIONAL([WITH_OSXAPI], [test "x$enable_osxapi" == xyes])

AS_IF([test "x$enable_osx_dmg_env" != xno],
      [AC_DEFINE([AGS_OSX_DMG_ENV], [1], [OS X DMG environment enabled])
      AC_MSG_NOTICE([OS X DMG environment enabled])],
      [AC_MSG_NOTICE([OS X DMG environment disabled])])
AM_CONDITIONAL([WITH_OSX_DMG_ENV], [test "x$enable_osx_dmg_env" == xyes])

PKG_CHECK_MODULES([UUID], [uuid >= 1.0.1])
AC_SUBST(UUID_CFLAGS)
AC_SUBST(UUID_LIBS)

AS_IF([test "x$enable_alsa" != xno],
      [PKG_CHECK_MODULES([LIBASOUND2], [alsa >= 1.0.25])
       AC_SUBST(LIBASOUND2_CFLAGS)
       AC_SUBST(LIBASOUND2_LIBS)
       AC_DEFINE([AGS_WITH_ALSA], [1], [ALSA enabled])
       AC_MSG_NOTICE([ALSA enabled])],
      [AC_MSG_NOTICE([ALSA disabled])])

AS_IF([test "x$enable_alsa_compat" != xno],
      [PKG_CHECK_MODULES([LIBDSSIALSACOMPAT], [libdssialsacompat >= 1.0.0])
       AC_SUBST(LIBDSSIALSACOMPAT_CFLAGS)
       AC_SUBST(LIBDSSIALSACOMPAT_LIBS)
       AC_MSG_NOTICE([ALSA compatibility library enabled])],
      [AC_MSG_NOTICE([ALSA compatibility library disabled])])

AS_IF([test "x$enable_oss" != xno],
      [AC_DEFINE([AGS_WITH_OSS], [1], [OSS enabled])
       AC_MSG_NOTICE([OSS enabled])],
      [AC_MSG_NOTICE([OSS disabled])])

AS_IF([test "x$enable_core_audio" != xno],
      [AC_DEFINE([AGS_WITH_CORE_AUDIO], [1], [core-audio enabled])

      CORE_AUDIO_CFLAGS= 
      CORE_AUDIO_LIBS="-framework CoreAudio -framework AudioToolbox -framework CoreMIDI"
      AC_SUBST(CORE_AUDIO_CFLAGS)
      AC_SUBST(CORE_AUDIO_LIBS)

      AC_MSG_NOTICE([core audio enabled])],
      [AC_MSG_NOTICE([core audio disabled])])

AS_IF([test "x$enable_audio_unit" != xno],
      [AC_DEFINE([AGS_WITH_AUDIO_UNIT], [1], [audio-unit enabled])

      AUDIO_UNIT_CFLAGS= 
      AUDIO_UNIT_LIBS="-framework AudioUnit -framework AudioToolbox"
      AC_SUBST(AUDIO_UNIT_CFLAGS)
      AC_SUBST(AUDIO_UNIT_LIBS)

      AC_MSG_NOTICE([audio-unit enabled])],
      [AC_MSG_NOTICE([audio-unit disabled])])

AS_IF([test "x$enable_wasapi" != xno],
      [AC_DEFINE([AGS_WITH_WASAPI], [1], [wasapi enabled])
      WASAPI_CFLAGS=""
      WASAPI_LIBS="-lwinmm -ldsound -lsetupapi -lole32"
      AC_SUBST(WASAPI_CFLAGS)
      AC_SUBST(WASAPI_LIBS)

      AC_MSG_NOTICE([wasapi enabled])],
      [AC_MSG_NOTICE([wasapi disabled])])

AS_IF([test "x$enable_jack" != xno],
      [AC_DEFINE([AGS_WITH_JACK], [1], [JACK enabled])

      PKG_CHECK_MODULES([JACK], [jack])
      AC_SUBST(JACK_CFLAGS)
      AC_SUBST(JACK_LIBS)

      AC_CHECK_FUNC([jack_port_uuid],
                    [AC_DEFINE([HAVE_JACK_PORT_UUID], [1], [Jack port uuid available])])
      AC_MSG_NOTICE([JACK enabled])],
      [AC_MSG_NOTICE([JACK disabled])])

AS_IF([test "x$enable_pulse" != xno],
      [AC_DEFINE([AGS_WITH_PULSE], [1], [PULSE enabled])

      PKG_CHECK_MODULES([PULSE], [libpulse-mainloop-glib])
      AC_SUBST(PULSE_CFLAGS)
      AC_SUBST(PULSE_LIBS)

      AC_MSG_NOTICE([PULSE enabled])],
      [AC_MSG_NOTICE([PULSE disabled])])

PKG_CHECK_MODULES([LIBXML2], [libxml-2.0 >= 2.8.0])
AC_SUBST(LIBXML2_CFLAGS)
AC_SUBST(LIBXML2_LIBS)

PKG_CHECK_MODULES([SAMPLERATE], [samplerate >= 0.1.8])
AC_SUBST(SAMPLERATE_CFLAGS)
AC_SUBST(SAMPLERATE_LIBS)

PKG_CHECK_MODULES([SNDFILE], [sndfile >= 1.0.25])
AC_SUBST(SNDFILE_CFLAGS)
AC_SUBST(SNDFILE_LIBS)

PKG_CHECK_MODULES([FFTW], [fftw3 >= 3.3.4])
AC_SUBST(FFTW_CFLAGS)
AC_SUBST(FFTW_LIBS)

PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.56.0])
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES([GOBJECT], [gobject-2.0 >= 2.56.0 glib-2.0 >= 2.56.0 gmodule-2.0 >= 2.56.0 gthread-2.0 >= 2.56.0])
AC_SUBST(GOBJECT_CFLAGS)
AC_SUBST(GOBJECT_LIBS)

AC_DEFINE([HAVE_GLIB_2_6], [1], [GLib 2.6 available])
AC_DEFINE([HAVE_GLIB_2_44], [1], [GLib 2.44 available])
AC_DEFINE([HAVE_GLIB_2_52], [1], [GLib 2.52 available])
AC_DEFINE([HAVE_GLIB_2_54], [1], [GLib 2.54 available])
AC_DEFINE([HAVE_GLIB_2_56], [1], [GLib 2.56 available])
AC_DEFINE([HAVE_GLIB_2_62], [0], [GLib 2.62 available])
AC_DEFINE([HAVE_GLIB_2_68], [0], [GLib 2.68 available])

AS_IF([test "x$enable_introspection" != xno],
      [AC_MSG_NOTICE([introspection enabled])],
      [AC_MSG_NOTICE([introspection disabled])])
AM_CONDITIONAL([HAVE_INTROSPECTION], [test "x$enable_introspection" == xyes])
# GOBJECT_INTROSPECTION_CHECK([1.30.0])

PKG_CHECK_MODULES([GIO], [gio-2.0 >= 2.56.0])
AC_SUBST(GIO_CFLAGS)
AC_SUBST(GIO_LIBS)

PKG_CHECK_MODULES([JSON], [json-glib-1.0 >= 1.6.0])
AC_SUBST(JSON_GLIB_CFLAGS)
AC_SUBST(JSON_GLIB_LIBS)

PKG_CHECK_MODULES([LIBSOUP], [libsoup-3.0 >= 3.0.0])
AC_SUBST(LIBSOUP_CFLAGS)
AC_SUBST(LIBSOUP_LIBS)

AC_DEFINE([HAVE_ATK_2_12], [1], [Atk 2.12 available])

AS_IF([test "x$enable_debug" != xno],
      [AC_DEFINE([AGS_WITH_DEBUG], [1], [debug symbols enabled])
      
      AC_MSG_NOTICE([debug enabled])],
      [AC_MSG_NOTICE([debug disabled])])
AM_CONDITIONAL([AGS_WITH_DEBUG], [test "x$enable_debug" == xyes])

AS_IF([test "x$enable_devel" != xno],
      [AC_DEFINE([AGS_WITH_DEVEL], [1], [devel mode enabled])
      
      AC_MSG_NOTICE([devel enabled])],
      [AC_MSG_NOTICE([devel disabled])])
AM_CONDITIONAL([AGS_WITH_DEVEL], [test "x$enable_devel" == xyes])

AS_IF([test "x$enable_public_libgsequencer" != xno],
      [AC_DEFINE([AGS_WITH_PUBLIC_LIBGSEQUENCER], [1], [public libgsequencer enabled])
      
      AC_MSG_NOTICE([public libgsequencer enabled])],
      [AC_MSG_NOTICE([public libgsequencer disabled])])
AM_CONDITIONAL([AGS_WITH_PUBLIC_LIBGSEQUENCER], [test "x$enable_public_libgsequencer" == xyes])

AS_IF([test "x$enable_upstream_gtk_doc" != xno],
      [AC_DEFINE([AGS_WITH_UPSTREAM_GTK_DOC], [1], [upstream API reference directory enabled])
      
      AC_MSG_NOTICE([upstream API reference directory enabled])],
      [AC_MSG_NOTICE([upstream API reference directory disabled])])
AM_CONDITIONAL([AGS_WITH_UPSTREAM_GTK_DOC], [test "x$enable_upstream_gtk_doc" == xyes])

AS_IF([test "x$enable_single_docdir" != xno],
      [AC_DEFINE([AGS_WITH_SINGLE_DOCDIR], [1], [single documentation directory enabled])
      
      AC_MSG_NOTICE([single documentation directory enabled])],
      [AC_MSG_NOTICE([single documentation directory disabled])])
AM_CONDITIONAL([AGS_WITH_SINGLE_DOCDIR], [test "x$enable_single_docdir" == xyes])

AS_IF([test "x$enable_libinstpatch" != xno],
      [PKG_CHECK_MODULES([LIBINSTPATCH], [libinstpatch-1.0 >= 1.0])
      AC_SUBST(LIBINSTPATCH_CFLAGS)
      AC_SUBST(LIBINSTPATCH_LIBS)

LIBAGS_AUDIO_LIBINSTPATCH_FILE_XINCLUDE="      <xi:include href=\"xml/ags_ipatch.xml\"/>\\
      <xi:include href=\"xml/ags_ipatch_gig_reader.xml\"/>\\
      <xi:include href=\"xml/ags_ipatch_sf2_reader.xml\"/>\\
      <xi:include href=\"xml/ags_ipatch_dls2_reader.xml\"/>\\
      <xi:include href=\"xml/ags_ipatch_sample.xml\"/>"

LIBAGS_AUDIO_LIBINSTPATCH_SF2_XINCLUDE="      <xi:include href=\"xml/ags_sf2_synth_generator.xml\"/>\\
      <xi:include href=\"xml/ags_sf2_synth_util.xml\"/>"
      
LIBAGS_AUDIO_LIBINSTPATCH_THREAD_XINCLUDE="      <xi:include href=\"xml/ags_sf2_loader.xml\"/>\\
      <xi:include href=\"xml/ags_sf2_midi_locale_loader.xml\"/>"

LIBAGS_AUDIO_LIBINSTPATCH_FX_XINCLUDE="      <xi:include href=\"xml/ags_fx_sf2_synth_audio.xml\"/>\\
      <xi:include href=\"xml/ags_fx_sf2_synth_audio_processor.xml\"/>\\
      <xi:include href=\"xml/ags_fx_sf2_synth_audio_signal.xml\"/>\\
      <xi:include href=\"xml/ags_fx_sf2_synth_channel.xml\"/>\\
      <xi:include href=\"xml/ags_fx_sf2_synth_channel_processor.xml\"/>\\
      <xi:include href=\"xml/ags_fx_sf2_synth_recycling.xml\"/>"

LIBAGS_AUDIO_LIBINSTPATCH_TASK_XINCLUDE="      <xi:include href=\"xml/ags_apply_sf2_synth.xml\"/>\\
      <xi:include href=\"xml/ags_apply_sf2_midi_locale.xml\"/>\\
      <xi:include href=\"xml/ags_open_sf2_instrument.xml\"/>\\
      <xi:include href=\"xml/ags_open_sf2_sample.xml\"/>"

LIBGSEQUENCER_LIBINSTPATCH_MACHINE_XINCLUDE="      <xi:include href=\"xml/ags_ffplayer.xml\"/>\\
      <xi:include href=\"xml/ags_ffplayer_bridge.xml\"/>\\
      <xi:include href=\"xml/ags_ffplayer_bulk_input.xml\"/>\\
      <xi:include href=\"xml/ags_ffplayer_callbacks.xml\"/>\\
      <xi:include href=\"xml/ags_ffplayer_input_line.xml\"/>\\
      <xi:include href=\"xml/ags_ffplayer_input_line_callbacks.xml\"/>\\
      <xi:include href=\"xml/ags_ffplayer_input_pad.xml\"/>\\
      <xi:include href=\"xml/ags_sf2_synth.xml\"/>\\
      <xi:include href=\"xml/ags_sf2_synth_callbacks.xml\"/>"

      AC_SUBST(LIBAGS_AUDIO_LIBINSTPATCH_FILE_XINCLUDE)
      AC_SUBST(LIBAGS_AUDIO_LIBINSTPATCH_SF2_XINCLUDE)
      AC_SUBST(LIBAGS_AUDIO_LIBINSTPATCH_TRHEAD_XINCLUDE)
      AC_SUBST(LIBAGS_AUDIO_LIBINSTPATCH_FX_XINCLUDE)
      AC_SUBST(LIBAGS_AUDIO_LIBINSTPATCH_TASK_XINCLUDE)
      AC_SUBST(LIBGSEQUENCER_LIBINSTPATCH_MACHINE_XINCLUDE)
      AC_DEFINE([AGS_WITH_LIBINSTPATCH], [1], [Disable if you don't want libinstpatch])],
      [AC_MSG_NOTICE([libinstpatch disabled])])
AM_CONDITIONAL([WITH_LIBINSTPATCH], [test "x$enable_libinstpatch" == xyes])

WITH_VST3=0

if test "${LIBAGS_VST3_LIBDIR+set}" = set; then
echo "variable \$LIBAGS_VST3_LIBDIR is set to: $LIBAGS_VST3_LIBDIR"
else
LIBAGS_VST3_LIBDIR="-L$libexecdir/lib/vst3"
echo "variable \$LIBAGS_VST3_LIBDIR is set to: $LIBAGS_VST3_LIBDIR"
fi

if test "${LIBAGS_VST3_LIBS+set}" = set; then
echo "variable \$LIBAGS_VST3_LIBS is set to: $LIBAGS_VST3_LIBS"
else
# -lvstgui_standalone
LIBAGS_VST3_LIBS="-lbase -lpluginterfaces -lsdk -lsdk_common -lsdk_hosting -lvstgui -lvstgui_support -lvstgui_uidescription"
echo "variable \$LIBAGS_VST3_LIBS is set to: $LIBAGS_VST3_LIBS"
fi

AS_IF([test "x$enable_vst3" != xno],
      [AC_DEFINE([AGS_WITH_VST3], [1], [VST3 enabled])
LIBAGS_AUDIO_VST3_PLUGIN_XINCLUDE="    <chapter id=\"plugin-vst3\">\\
      <title>VST3 plugins</title>\\
      <para>The VST3 plugin objects.</para>\\
      <xi:include href=\"xml/ags_vst3_conversion.xml\"/>\\
      <xi:include href=\"xml/ags_vst3_plugin.xml\"/>\\
      <xi:include href=\"xml/ags_vst3_manager.xml\"/>\\
    </chapter>"

LIBAGS_AUDIO_VST3_FX_XINCLUDE="      <xi:include href=\"xml/ags_fx_vst3_audio.xml\"/>\\
      <xi:include href=\"xml/ags_fx_vst3_audio_processor.xml\"/>\\
      <xi:include href=\"xml/ags_fx_vst3_audio_signal.xml\"/>\\
      <xi:include href=\"xml/ags_fx_vst3_channel.xml\"/>\\
      <xi:include href=\"xml/ags_fx_vst3_channel_processor.xml\"/>\\
      <xi:include href=\"xml/ags_fx_vst3_recycling.xml\"/>"

LIBAGS_AUDIO_VST3_TASK_XINCLUDE="      <xi:include href=\"xml/ags_instantiate_vst3_plugin.xml\"/>\\
      <xi:include href=\"xml/ags_write_vst3_port.xml\"/>"

LIBGSEQUENCER_VST3_BROWSER_XINCLUDE="      <xi:include href=\"xml/ags_vst3_browser.xml\"/>\\
      <xi:include href=\"xml/ags_vst3_browser_callbacks.xml\"/>"

LIBGSEQUENCER_VST3_MACHINE_XINCLUDE="      <xi:include href=\"xml/ags_live_vst3_bridge.xml\"/>\\
      <xi:include href=\"xml/ags_live_vst3_bridge_callbacks.xml\"/>\\
      <xi:include href=\"xml/ags_vst3_bridge.xml\"/>\\
      <xi:include href=\"xml/ags_vst3_bridge_callbacks.xml\"/>"

      AC_SUBST(LIBAGS_AUDIO_VST3_PLUGIN_XINCLUDE)
      AC_SUBST(LIBAGS_AUDIO_VST3_FX_XINCLUDE)
      AC_SUBST(LIBAGS_AUDIO_VST3_TASK_XINCLUDE)
      AC_SUBST(LIBGSEQUENCER_VST3_BROWSER_XINCLUDE)
      AC_SUBST(LIBGSEQUENCER_VST3_MACHINE_XINCLUDE)
      AC_SUBST(LIBAGS_VST3_RELEASE_TYPE)
      AC_SUBST(LIBAGS_VST3_CPPFLAGS)
      AC_SUBST(LIBAGS_VST3_CXXFLAGS)
      AC_SUBST(LIBAGS_VST3_LDFLAGS)
      AC_SUBST(LIBAGS_VST3_LIBDIR)
      AC_SUBST(LIBAGS_VST3_LIBS)
      AC_MSG_NOTICE([VST3 enabled])],
      [AC_MSG_NOTICE([VST3 disabled])])
AM_CONDITIONAL([WITH_VST3], [test "x$enable_vst3" == xyes])

AM_CONDITIONAL([WITH_MIDI2], [test "x$enable_midi2" == xyes])

PKG_CHECK_MODULES([CAIRO], [cairo >= 1.12.0])
AC_SUBST(CAIRO_CFLAGS)
AC_SUBST(CAIRO_LIBS)

PKG_CHECK_MODULES([GTK], [gtk4 >= 4.0.0])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

AS_IF([test "x$with_gtk_unix_print" != xno],
      [PKG_CHECK_MODULES([GTK_UNIX_PRINT], [gtk4-unix-print >= 4.0.0])
       AC_SUBST(GTK_UNIX_PRINT_CFLAGS)
       AC_SUBST(GTK_UNIX_PRINT_LIBS)
       AC_DEFINE([AGS_WITH_GTK_UNIX_PRINT], [1],
                 [Define if you have Gtk4 Unix print])
      ])

AS_IF([test "x$with_poppler" != xno],
      [PKG_CHECK_MODULES([POPPLER], [poppler-glib >= 0.71.0])
       AC_SUBST(POPPLER_CFLAGS)
       AC_SUBST(POPPLER_LIBS)
       AC_DEFINE([AGS_WITH_POPPLER], [1],
                 [Define if you have poppler])
      ])

AS_IF([test "x$with_gstreamer" != xno],
      [PKG_CHECK_MODULES([GSTREAMER], [gstreamer-1.0 >= 1.6.0 gstreamer-audio-1.0 >= 1.6.0 gstreamer-video-1.0 >= 1.6.0 gstreamer-app-1.0 >= 1.6.0 gstreamer-pbutils-1.0 >= 1.6.0])
       AC_SUBST(GSTREAMER_CFLAGS)
       AC_SUBST(GSTREAMER_LIBS)
       AC_DEFINE([AGS_WITH_GSTREAMER], [1],
                 [Define if you have gstreamer])
      ])
AM_CONDITIONAL([WITH_GSTREAMER], [test "x$with_gstreamer" == xyes])

AS_IF([test "x$enable_run_functional_tests" == xyes],
      [AC_MSG_NOTICE([run functional tests enabled])],
      [AC_MSG_NOTICE([run functional tests disabled])])
AM_CONDITIONAL([RUN_FUNCTIONAL_TESTS], [test "x$enable_run_functional_tests" == xyes])

AS_IF([test "x$enable_fast_test_mode" == xyes],
      [AC_MSG_NOTICE([fast test mode enabled])],
      [AC_MSG_NOTICE([fast test mode disabled])])
AM_CONDITIONAL([FAST_TEST_MODE], [test "x$enable_fast_test_mode" == xyes])

# Checks for library functions.
# AM_GNU_GETTEXT
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([atexit clock_gettime floor memmove memset pow putenv realpath regcomp setlocale sqrt strchr strdup strerror strrchr strstr strtoul uname])

AC_CANONICAL_HOST

build_gnu=no
build_linux=no
build_windows=no
build_mac=no
build_freebsd=no

# Detect the target system
case "${host_os}" in
    gnu*)
        build_gnu=yes
        ;;
    linux*)
        build_linux=yes
        ;;
    freebsd*)
        build_freebsd=yes
        ;;
    cygwin*|mingw*)
        build_windows=yes
        ;;
    darwin*)
        build_mac=yes
        ;;
    *)
        AC_MSG_NOTICE(["OS $host_os is not supported"])
        ;;
esac

# Pass the conditionals to automake
AM_CONDITIONAL([GNU], [test "$build_gnu" = "yes"])
AM_CONDITIONAL([LINUX], [test "$build_linux" = "yes"])
AM_CONDITIONAL([FREEBSD], [test "$build_freebsd" = "yes"])
AM_CONDITIONAL([WINDOWS], [test "$build_windows" = "yes"])
AM_CONDITIONAL([OSX], [test "$build_mac" = "yes"])


# check for gtk-doc
m4_ifdef([GTK_DOC_CHECK], [
GTK_DOC_CHECK([1.14],[--flavour no-tmpl])
],[
AM_CONDITIONAL([ENABLE_GTK_DOC], false)
])

AM_CONDITIONAL([WITH_SYSTEM_TESTS], [test "x$enable_run_system_tests" == xyes])
AM_CONDITIONAL([WITH_VST3], [test "x$enable_vst3" == xyes])
AM_CONDITIONAL([WITH_EXPERIMENTAL], [test "x$enable_experimental" == xyes])

AGS_POTFILES([])

LIBAGS_SYMBOLS([])
LIBAGS_THREAD_SYMBOLS([])
LIBAGS_SERVER_SYMBOLS([])
LIBAGS_AUDIO_SYMBOLS([])
LIBAGS_VST_SYMBOLS([])
LIBAGS_GUI_SYMBOLS([])

LIBAGS_MARSHALLERS([])
LIBAGS_GUI_MARSHALLERS([])

AGS_DEVELOPER_BOOK_LISTINGS([])

AGS_DOCS_BOOKS([])
AGS_DOCS_API([])


AS_IF([test "x$INTROSPECTION_SCANNER" != x],
      [AC_SUBST(INTROSPECTION_SCANNER, $INTROSPECTION_SCANNER)
       AC_MSG_NOTICE([INTROSPECTION_SCANNER = $INTROSPECTION_SCANNER])],
      [AC_SUBST(INTROSPECTION_SCANNER, /usr/bin/g-ir-scanner)
       AC_MSG_NOTICE([INTROSPECTION_SCANNER = $INTROSPECTION_SCANNER])])

AS_IF([test "x$INTROSPECTION_COMPILER" != x],
      [AC_SUBST(INTROSPECTION_COMPILER, $INTROSPECTION_COMPILER)
       AC_MSG_NOTICE([INTROSPECTION_COMPILER = $INTROSPECTION_COMPILER])],
      [AC_SUBST(INTROSPECTION_COMPILER, /usr/bin/g-ir-compiler)
       AC_MSG_NOTICE([INTROSPECTION_COMPILER = $INTROSPECTION_COMPILER])])

AS_IF([test "x$HTMLHELP_XSL" != x],
      [AC_SUBST(HTMLHELP_XSL, $HTMLHELP_XSL)
       AC_MSG_NOTICE([HTMLHELP_XSL = $HTMLHELP_XSL])],
      [AC_SUBST(HTMLHELP_XSL, /usr/share/xml/docbook/stylesheet/docbook-xsl/htmlhelp/htmlhelp.xsl)
       AC_MSG_NOTICE([HTMLHELP_XSL = $HTMLHELP_XSL])])

AS_IF([test "x$FO_XSL" != x],
      [AC_SUBST(FO_XSL, $FO_XSL)
       AC_MSG_NOTICE([FO_XSL = $FO_XSL])],
      [AC_SUBST(FO_XSL, /usr/share/xml/docbook/stylesheet/docbook-xsl/fo/docbook.xsl)
       AC_MSG_NOTICE([FO_XSL = $FO_XSL])])

AS_IF([test "x$enable_upstream_gtk_doc" != xno],
[AS_IF([test "x$GOBJECT_API_DOC" != x],
      [AC_SUBST(GOBJECT_API_DOC, $GOBJECT_API_DOC)
       AC_MSG_NOTICE([GOBJECT_API_DOC = $GOBJECT_API_DOC])],
      [AC_SUBST(GOBJECT_API_DOC, '/usr/share/doc/libglib2.0-doc/gobject')
       AC_MSG_NOTICE([GOBJECT_API_DOC = $GOBJECT_API_DOC])])

AS_IF([test "x$GTK_API_DOC" != x],
      [AC_SUBST(GTK_API_DOC, $GTK_API_DOC)
       AC_MSG_NOTICE([GTK_API_DOC = $GTK_API_DOC])],
      [AC_SUBST(GTK_API_DOC, '/usr/share/doc/libgtk-3-doc/gtk3')
       AC_MSG_NOTICE([GTK_API_DOC = $GTK_API_DOC])])

AS_IF([test "x$LIBAGS_API_DOC" != x],
      [AC_SUBST(LIBAGS_API_DOC, $LIBAGS_API_DOC)
       AC_MSG_NOTICE([LIBAGS_API_DOC = $LIBAGS_API_DOC])],
      [AC_SUBST(LIBAGS_API_DOC, '/usr/share/doc/libags-doc/api')
       AC_MSG_NOTICE([LIBAGS_API_DOC = $LIBAGS_API_DOC])])

AS_IF([test "x$LIBAGS_AUDIO_API_DOC" != x],
      [AC_SUBST(LIBAGS_AUDIO_API_DOC, $LIBAGS_AUDIO_API_DOC)
       AC_MSG_NOTICE([LIBAGS_AUDIO_API_DOC = $LIBAGS_AUDIO_API_DOC])],
      [AC_SUBST(LIBAGS_AUDIO_API_DOC, '/usr/share/doc/libags-audio-doc/api')
       AC_MSG_NOTICE([LIBAGS_AUDIO_API_DOC = $LIBAGS_AUDIO_API_DOC])])

AS_IF([test "x$LIBAGS_GUI_API_DOC" != x],
      [AC_SUBST(LIBAGS_GUI_API_DOC, $LIBAGS_GUI_API_DOC)
       AC_MSG_NOTICE([LIBAGS_GUI_API_DOC = $LIBAGS_GUI_API_DOC])],
      [AC_SUBST(LIBAGS_GUI_API_DOC, '/usr/share/doc/libags-gui-doc/api')
       AC_MSG_NOTICE([LIBAGS_GUI_API_DOC = $LIBAGS_GUI_API_DOC])])

AS_IF([test "x$LIBGSEQUENCER_API_DOC" != x],
      [AC_SUBST(LIBGSEQUENCER_API_DOC, $LIBGSEQUENCER_API_DOC)
       AC_MSG_NOTICE([LIBGSEQUENCER_API_DOC = $LIBGSEQUENCER_API_DOC])],
      [AC_SUBST(LIBGSEQUENCER_API_DOC, '/usr/share/doc/libgsequencer-doc/api')
       AC_MSG_NOTICE([LIBGSEQUENCER_API_DOC = $LIBGSEQUENCER_API_DOC])])

AS_IF([test "x$LIBAGS_API_DIR" != x],
      [AC_SUBST(LIBAGS_API_DIR, $LIBAGS_API_DIR)
       AC_MSG_NOTICE([LIBAGS_API_DIR = $LIBAGS_API_DIR])],
      [AC_SUBST(LIBAGS_API_DIR, $datadir/doc/libags-doc/api)
       AC_MSG_NOTICE([LIBAGS_API_DIR = $LIBAGS_API_DIR])])

AS_IF([test "x$LIBAGS_AUDIO_API_DIR" != x],
      [AC_SUBST(LIBAGS_AUDIO_API_DIR, $LIBAGS_AUDIO_API_DIR)
       AC_MSG_NOTICE([LIBAGS_AUDIO_API_DIR = $LIBAGS_AUDIO_API_DIR])],
      [AC_SUBST(LIBAGS_AUDIO_API_DIR, $datadir/doc/libags-audio-doc/api)
       AC_MSG_NOTICE([LIBAGS_AUDIO_API_DIR = $LIBAGS_AUDIO_API_DIR])])

AS_IF([test "x$LIBAGS_GUI_API_DIR" != x],
      [AC_SUBST(LIBAGS_GUI_API_DIR, $LIBAGS_GUI_API_DIR)
       AC_MSG_NOTICE([LIBAGS_GUI_API_DIR = $LIBAGS_GUI_API_DIR])],
      [AC_SUBST(LIBAGS_GUI_API_DIR, $datadir/doc/libags-gui-doc/api)
       AC_MSG_NOTICE([LIBAGS_GUI_API_DIR = $LIBAGS_GUI_API_DIR])])

AS_IF([test "x$LIBGSEQUENCER_API_DIR" != x],
      [AC_SUBST(LIBGSEQUENCER_API_DIR, $LIBGSEQUENCER_API_DIR)
       AC_MSG_NOTICE([LIBGSEQUENCER_API_DIR = $LIBGSEQUENCER_API_DIR])],
      [AC_SUBST(LIBGSEQUENCER_API_DIR, $datadir/doc/libgsequencer-doc/api)
       AC_MSG_NOTICE([LIBGSEQUENCER_API_DIR = $LIBGSEQUENCER_API_DIR])])
],
[
AS_IF([test "x$GOBJECT_API_DOC" != x],
      [AC_SUBST(GOBJECT_API_DOC, $GOBJECT_API_DOC)
       AC_MSG_NOTICE([GOBJECT_API_DOC = $GOBJECT_API_DOC])],
      [AC_SUBST(GOBJECT_API_DOC, '../gobject')
       AC_MSG_NOTICE([GOBJECT_API_DOC = $GOBJECT_API_DOC])])

AS_IF([test "x$GTK_API_DOC" != x],
      [AC_SUBST(GTK_API_DOC, $GTK_API_DOC)
       AC_MSG_NOTICE([GTK_API_DOC = $GTK_API_DOC])],
      [AC_SUBST(GTK_API_DOC, '../gtk3')
       AC_MSG_NOTICE([GTK_API_DOC = $GTK_API_DOC])])

AS_IF([test "x$LIBAGS_API_DOC" != x],
      [AC_SUBST(LIBAGS_API_DOC, $LIBAGS_API_DOC)
       AC_MSG_NOTICE([LIBAGS_API_DOC = $LIBAGS_API_DOC])],
      [AC_SUBST(LIBAGS_API_DOC, '../libags')
       AC_MSG_NOTICE([LIBAGS_API_DOC = $LIBAGS_API_DOC])])

AS_IF([test "x$LIBAGS_AUDIO_API_DOC" != x],
      [AC_SUBST(LIBAGS_AUDIO_API_DOC, $LIBAGS_AUDIO_API_DOC)
       AC_MSG_NOTICE([LIBAGS_AUDIO_API_DOC = $LIBAGS_AUDIO_API_DOC])],
      [AC_SUBST(LIBAGS_AUDIO_API_DOC, '../libags_audio')
       AC_MSG_NOTICE([LIBAGS_AUDIO_API_DOC = $LIBAGS_AUDIO_API_DOC])])

AS_IF([test "x$LIBAGS_GUI_API_DOC" != x],
      [AC_SUBST(LIBAGS_GUI_API_DOC, $LIBAGS_GUI_API_DOC)
       AC_MSG_NOTICE([LIBAGS_GUI_API_DOC = $LIBAGS_GUI_API_DOC])],
      [AC_SUBST(LIBAGS_GUI_API_DOC, '../libags_gui')
       AC_MSG_NOTICE([LIBAGS_GUI_API_DOC = $LIBAGS_GUI_API_DOC])])

AS_IF([test "x$LIBGSEQUENCER_API_DOC" != x],
      [AC_SUBST(LIBGSEQUENCER_API_DOC, $LIBGSEQUENCER_API_DOC)
       AC_MSG_NOTICE([LIBGSEQUENCER_API_DOC = $LIBGSEQUENCER_API_DOC])],
      [AC_SUBST(LIBGSEQUENCER_API_DOC, '../libgsequencer')
       AC_MSG_NOTICE([LIBGSEQUENCER_API_DOC = $LIBGSEQUENCER_API_DOC])])

AS_IF([test "x$LIBAGS_API_DIR" != x],
      [AC_SUBST(LIBAGS_API_DIR, $LIBAGS_API_DIR)
       AC_MSG_NOTICE([LIBAGS_API_DIR = $LIBAGS_API_DIR])],
      [AC_SUBST(LIBAGS_API_DIR, $datadir/gtk-doc/html/libags)
       AC_MSG_NOTICE([LIBAGS_API_DIR = $LIBAGS_API_DIR])])

AS_IF([test "x$LIBAGS_AUDIO_API_DIR" != x],
      [AC_SUBST(LIBAGS_AUDIO_API_DIR, $LIBAGS_AUDIO_API_DIR)
       AC_MSG_NOTICE([LIBAGS_AUDIO_API_DIR = $LIBAGS_AUDIO_API_DIR])],
      [AC_SUBST(LIBAGS_AUDIO_API_DIR, $datadir/gtk-doc/html/libags_audio)
       AC_MSG_NOTICE([LIBAGS_AUDIO_API_DIR = $LIBAGS_AUDIO_API_DIR])])

AS_IF([test "x$LIBAGS_GUI_API_DIR" != x],
      [AC_SUBST(LIBAGS_GUI_API_DIR, $LIBAGS_GUI_API_DIR)
       AC_MSG_NOTICE([LIBAGS_GUI_API_DIR = $LIBAGS_GUI_API_DIR])],
      [AC_SUBST(LIBAGS_GUI_API_DIR, $datadir/gtk-doc/html/libags_gui)
       AC_MSG_NOTICE([LIBAGS_GUI_API_DIR = $LIBAGS_GUI_API_DIR])])

AS_IF([test "x$LIBGSEQUENCER_API_DIR" != x],
      [AC_SUBST(LIBGSEQUENCER_API_DIR, $LIBGSEQUENCER_API_DIR)
       AC_MSG_NOTICE([LIBGSEQUENCER_API_DIR = $LIBGSEQUENCER_API_DIR])],
      [AC_SUBST(LIBGSEQUENCER_API_DIR, $datadir/gtk-doc/html/libgsequencer)
       AC_MSG_NOTICE([LIBGSEQUENCER_API_DIR = $LIBGSEQUENCER_API_DIR])])
])

gl_INIT

AM_EXTRA_RECURSIVE_TARGETS([ags-docs])
AC_CONFIG_FILES([
lib/Makefile
Makefile
Doxyfile
m4/Makefile
po/Makefile.in
po/Makefile
unit-system-tests.mk
functional-system-tests.mk
docs/reference/libags/Makefile
docs/reference/libags/Ags.conf
docs/reference/libags/version.xml
docs/reference/libags/libags.xml
docs/reference/libags-audio/Makefile
docs/reference/libags-audio/AgsAudio.conf
docs/reference/libags-audio/version.xml
docs/reference/libags-audio/libags_audio.xml
docs/reference/libags-gui/Makefile
docs/reference/libags-gui/AgsGui.conf
docs/reference/libags-gui/version.xml
docs/reference/libags-gui/libags_gui.xml
docs/reference/libgsequencer/Makefile
docs/reference/libgsequencer/version.xml
docs/reference/libgsequencer/libgsequencer.xml
libags.pc
libags_audio.pc
libags_gui.pc
libgsequencer.pc])

AC_OUTPUT
