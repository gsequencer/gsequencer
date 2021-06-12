#                                               -*- Autoconf -*-
# Copyright (C) 2005-2021 Joel Kraehemann
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([gsequencer],[3.8.13],[jkraehemann@gmail.com])
AM_INIT_AUTOMAKE([subdir-objects -Wno-portability])
AC_CONFIG_SRCDIR([ags/config.h.in])
AC_CONFIG_HEADERS([ags/config.h])
AC_CONFIG_HEADERS([ags/ags_api_config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_USE_SYSTEM_EXTENSIONS([_GNU_SOURCE])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.8])

# options
AC_ARG_VAR(HTMLHELP_XSL, path to Docbook XML htmlhelp XSL file)

AC_ARG_VAR(GOBJECT_API_DOC, path for use with sed to GObject API Reference Manual)
AC_ARG_VAR(GTK_API_DOC, path for use with sed to Gtk-3 API Reference Manual)
AC_ARG_VAR(LIBAGS_API_DOC, path for use with sed to Libags API Reference Manual)
AC_ARG_VAR(LIBAGS_AUDIO_API_DOC, path for use with sed to Libags-audio API Reference Manual)
AC_ARG_VAR(LIBAGS_GUI_API_DOC, path for use with sed to Libags-gui API Reference Manual)

AC_ARG_VAR(LIBAGS_API_DIR, install path of Libags API Reference Manual)
AC_ARG_VAR(LIBAGS_AUDIO_API_DIR, install path of Libags-audio API Reference Manual)
AC_ARG_VAR(LIBAGS_GUI_API_DIR, install path of Libags-gui API Reference Manual)
AC_ARG_VAR(LIBGSEQUENCER_API_DIR, install path of Libgsequencer API Reference Manual)

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

AC_ARG_ENABLE(osxapi, [AS_HELP_STRING([--enable-osxapi],
		      		      [enable OS X API (default is no)])],
	      [],
	      [enable_osxapi=no])

AC_ARG_ENABLE(alsa, [AS_HELP_STRING([--enable-alsa],
				    [enable alsa (default is yes)])],
	      [],
	      [enable_alsa=yes])

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

AC_ARG_WITH([webkit],
	    [AS_HELP_STRING([--without-webkit],
			    [disable support for webkit])],
	    [],
	    [with_webkit=yes])

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
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL

#AS_IF([test "x$enable_w32api" != xno],
#      [gl_EARLY])

# libtool
LT_PREREQ([2.4.2])
LT_INIT([win32-dll])
AC_SUBST([LIBTOOL_DEPS])
AC_ENABLE_SHARED

AC_SUBST([GSEQUENCER_CFLAGS])
AC_SUBST([GSEQUENCER_LDFLAGS])

AC_SUBST([MIDI2XML_CFLAGS])
AC_SUBST([MIDI2XML_LDFLAGS])

AC_SUBST([LIBGSEQUENCER_TEST_LIBADD])

AC_SUBST([GSEQUENCER_FUNCTIONAL_TEST_LDADD])

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lX11':
AS_IF([test "x$enable_x11" != xno],
      [AC_DEFINE([AGS_WITH_X11], [1], [X11 enabled])
       AC_CHECK_LIB([X11], [main], , AC_MSG_ERROR("No X11 library present"))
       X11_LIBS="-lX11"],
      [AC_MSG_NOTICE([X11 disabled])])

AS_IF([test "x$enable_quartz" != xno],
      [AC_DEFINE([AGS_WITH_QUARTZ], [1], [quartz enabled])
       PKG_CHECK_MODULES([GTK_MAC_INTEGRATION], [gtk-mac-integration-gtk3 >= 2.0.7])
       AC_SUBST(GTK_MAC_INTEGRATION_CFLAGS)
       AC_SUBST(GTK_MAC_INTEGRATION_LIBS)
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
AC_CHECK_HEADERS([stdlib.h string.h unistd.h ladspa.h dssi.h lv2.h], , AC_MSG_ERROR("missing headers"))

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
      W32API_CFLAGS="-nostdinc -I./lib -I/usr/lib/gcc/x86_64-w64-mingw32/7.4.0/include -I/usr/x86_64-w64-mingw32/sys-root/mingw/include -I/usr/x86_64-w64-mingw32/sys-root/mingw/include/dssi -include ags/config.h "
      W32API_LIBS="-L./lib -lgnu"
      AC_MSG_NOTICE([w32api enabled])],
      [AC_MSG_NOTICE([w32api disabled])])
AM_CONDITIONAL([WITH_W32API], [test "x$enable_w32api" == xyes])

AS_IF([test "x$enable_osxapi" != xno],
      [AC_DEFINE([AGS_OSXAPI], [1], [OS X api enabled])
      OSXAPI_CFLAGS=""
      OSXAPI_LIBS=""
      AC_MSG_NOTICE([OS X api enabled])],
      [AC_MSG_NOTICE([OS X api disabled])])
AM_CONDITIONAL([WITH_OSXAPI], [test "x$enable_osxapi" == xyes])

PKG_CHECK_MODULES([UUID], [uuid >= 1.0.1])
AC_SUBST(UUID_CFLAGS)
AC_SUBST(UUID_LIBS)

AS_IF([test "x$enable_alsa" != xno],
      [PKG_CHECK_MODULES([LIBASOUND2], [alsa >= 1.0.25])
       AC_SUBST(LIBASOUND2_CFLAGS)
       AC_SUBST(LIBASOUND2_LIBS)
       AC_DEFINE([AGS_WITH_ALSA], [1], [ALSA enabled])
       AC_MSG_NOTICE([ALSA enabled])],
      [AC_MSG_NOTICE([ALSA disabled])
       AC_CHECK_HEADERS([alsa/seq_event.h],
			[AC_DEFINE([HAVE_ALSA_SEQ_EVENT_H], [1], [ALSA sequencer event available])]
			[PKG_CHECK_MODULES([LIBASOUND2], [alsa >= 1.0.5])
			AC_SUBST(LIBASOUND2_CFLAGS)
			AC_SUBST(LIBASOUND2_LIBS)],
		  	[AC_MSG_NOTICE([ALSA sequencer event not available])],[
#if HAVE_ALSA_SEQ_EVENT_H
#  include <alsa/seq_event.h>
#  include <alsa/seq_midi_event.h>
#endif
			])])

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

GOBJECT_INTROSPECTION_CHECK([1.30.0])

PKG_CHECK_MODULES([GIO], [gio-2.0 >= 2.56.0 glib-2.0 >= 2.56.0 gmodule-2.0 >= 2.56.0 gthread-2.0 >= 2.56.0])
AC_SUBST(GIO_CFLAGS)
AC_SUBST(GIO_LIBS)

PKG_CHECK_MODULES([LIBSOUP], [libsoup-2.4 >= 2.52.0])
AC_SUBST(LIBSOUP_CFLAGS)
AC_SUBST(LIBSOUP_LIBS)

AC_DEFINE([HAVE_ATK_2_12], [1], [Atk 2.12 available])

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
      AC_DEFINE([AGS_WITH_LIBINSTPATCH], [1], [Disable if you don't want libinstpatch])],
      [AC_MSG_NOTICE([libinstpatch disabled])])
AM_CONDITIONAL([WITH_LIBINSTPATCH], [test "x$enable_libinstpatch" == xyes])

AS_IF([test "x$enable_vst3" != xno],
      [AC_DEFINE([AGS_WITH_VST3], [1], [VST3 enabled])

      AC_SUBST(VST3_CXXFLAGS)
      AC_SUBST(VST3_LIBS)
      
      AC_MSG_NOTICE([VST3 enabled])],
      [AC_MSG_NOTICE([VST3 disabled])])

PKG_CHECK_MODULES([CAIRO], [cairo >= 1.12.0])
AC_SUBST(CAIRO_CFLAGS)
AC_SUBST(CAIRO_LIBS)

PKG_CHECK_MODULES([GTK], [gtk+-3.0 >= 3.18.0])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

AS_IF([test "x$with_webkit" != xno],
      [PKG_CHECK_MODULES([WEBKITGTK], [webkit2gtk-4.0 >= 2.10.0])
       AC_SUBST(WEBKITGTK_CFLAGS)
       AC_SUBST(WEBKITGTK_LIBS)
       AC_DEFINE([AGS_WITH_WEBKIT], [1],
       		 [Define if you have webkit])
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
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([__argz_count __argz_next __argz_stringify atexit clock_gettime floor memmove memset pow putenv realpath regcomp setlocale sqrt strchr strdup strerror strrchr strstr strtoul])

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
LIBAGS_GUI_SYMBOLS([])

LIBAGS_MARSHALLERS([])
LIBAGS_GUI_MARSHALLERS([])

AGS_DEVELOPER_BOOK_LISTINGS([])

AGS_DOCS_BOOKS([])
AGS_DOCS_API([])

AS_IF([test "x$HTMLHELP_XSL" != x],
      [AC_SUBST(HTMLHELP_XSL, $HTMLHELP_XSL)
       AC_MSG_NOTICE([HTMLHELP_XSL = $HTMLHELP_XSL])],
      [AC_SUBST(HTMLHELP_XSL, /usr/share/xml/docbook/stylesheet/docbook-xsl/htmlhelp/htmlhelp.xsl)
       AC_MSG_NOTICE([HTMLHELP_XSL = $HTMLHELP_XSL])])

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

#AS_IF([test "x$enable_w32api" != xno],
#       [gl_INIT])

AM_EXTRA_RECURSIVE_TARGETS([ags-docs])
AC_CONFIG_FILES([
Makefile
unit-system-tests.mk
functional-system-tests.mk
m4/Makefile
po/Makefile.in
po/Makefile
docs/reference/libags/Makefile
docs/reference/libags-audio/Makefile
docs/reference/libags-gui/Makefile
docs/reference/libgsequencer/Makefile
libags.pc
libags_audio.pc
libags_gui.pc
libgsequencer.pc])

#AM_COND_IF([WITH_W32API],
#           [AC_CONFIG_FILES([lib/Makefile])])

AC_OUTPUT
