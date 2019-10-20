#                                               -*- Autoconf -*-
# Copyright (C) 2005-2018 Jo\u00EBl Kr\u00E4hemann
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([gsequencer],[2.3.15],[jkraehemann@gmail.com])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR([ags/config.h.in])
AC_CONFIG_HEADERS([ags/config.h])
AC_CONFIG_HEADERS([ags/ags_config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_USE_SYSTEM_EXTENSIONS([_GNU_SOURCE])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.8])

# options
AC_ARG_ENABLE(gnulib, [AS_HELP_STRING([--enable-gnulib],
				      [gnulib builtin functions (default is no)])],
	      [],			       
	      [enable_gnulib=no])

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

AC_ARG_ENABLE(xmlrpc, [AS_HELP_STRING([--enable-xmlrpc],
				      [enable xmlrpc (default is no)])],
	      [],
	      [enable_xmlrpc=no])

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

# Checks for programs.
AC_PROG_CC
# gl_EARLY
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL

# libtool
LT_PREREQ([2.4.2])
LT_INIT
AC_SUBST([LIBTOOL_DEPS])
AC_ENABLE_SHARED

AM_CONDITIONAL([WITH_GNULIB], [test "x$enable_gnulib" == xyes])

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
       PKG_CHECK_MODULES([GTK_MAC_INTEGRATION], [gtk-mac-integration-gtk2 >= 2.0.7])
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
      [AC_MSG_NOTICE([striping rt-events disabled])
       ])

# FIXME: Replace `main' with a function in `-lrt':
AS_IF([test "x$enable_rt" != xno],
      [AC_DEFINE([AGS_WITH_RT], [1], [rt enabled])
      RT_LIBS="-lrt"
      AC_CHECK_LIB([rt], [main], , AC_MSG_ERROR("No rt library present"))
      AC_MSG_NOTICE([rt enabled])],
      [AC_MSG_NOTICE([rt disabled])
       ])

# Checks for header files.
AC_PATH_X
AC_FUNC_ALLOCA
# AC_CHECK_HEADERS([fcntl.h float.h inttypes.h langinfo.h libintl.h limits.h locale.h netinet/in.h stddef.h stdint.h stdlib.h string.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h unistd.h ladspa.h lv2.h], , AC_MSG_ERROR("missing headers"))

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

AS_IF([test "x$enable_w32api" != xno],
      [AC_DEFINE([AGS_W32API], [1], [w32api enabled])
      W32API_CFLAGS="-nostdinc -I./lib -I/usr/lib/gcc/x86_64-w64-mingw32/7.4.0/include -I/usr/x86_64-w64-mingw32/sys-root/mingw/include -I/usr/x86_64-w64-mingw32/sys-root/mingw/include/dssi -include ags/config.h "
      W32API_LIBS="-L./lib -lgnu"
      AC_MSG_NOTICE([w32api enabled])],
      AC_MSG_NOTICE([w32api disabled]))
AM_CONDITIONAL([WITH_W32API], [test "x$enable_w32api" == xyes])

AS_IF([test "x$enable_osxapi" != xno],
      [AC_DEFINE([AGS_OSXAPI], [1], [OS X api enabled])
      OSXAPI_CFLAGS=""
      OSXAPI_LIBS=""
      AC_MSG_NOTICE([OS X api enabled])],
      AC_MSG_NOTICE([OS X api disabled]))
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
      AC_MSG_NOTICE([OSS disabled]))

AS_IF([test "x$enable_core_audio" != xno],
      [AC_DEFINE([AGS_WITH_CORE_AUDIO], [1], [core-audio enabled])

      CORE_AUDIO_CFLAGS= 
      CORE_AUDIO_LIBS="-framework CoreAudio -framework AudioToolbox -framework CoreMIDI"
      AC_SUBST(CORE_AUDIO_CFLAGS)
      AC_SUBST(CORE_AUDIO_LIBS)

      AC_MSG_NOTICE([core audio enabled])],
      AC_MSG_NOTICE([core audio disabled]))

AS_IF([test "x$enable_audio_unit" != xno],
      [AC_DEFINE([AGS_WITH_AUDIO_UNIT], [1], [core-audio enabled])

      AUDIO_UNIT_CFLAGS= 
      AUDIO_UNIT_LIBS="-framework AudioUnit -framework AudioToolbox"
      AC_SUBST(AUDIO_UNIT_CFLAGS)
      AC_SUBST(AUDIO_UNIT_LIBS)

      AC_MSG_NOTICE([core audio enabled])],
      AC_MSG_NOTICE([core audio disabled]))

AS_IF([test "x$enable_wasapi" != xno],
      [AC_DEFINE([AGS_WITH_WASAPI], [1], [wasapi enabled])
      WASAPI_CFLAGS=""
      WASAPI_LIBS="-lwinmm -ldsound -lsetupapi -lole32"
      AC_SUBST(WASAPI_CFLAGS)
      AC_SUBST(WASAPI_LIBS)

      AC_MSG_NOTICE([wasapi enabled])],
      AC_MSG_NOTICE([wasapi disabled]))

AS_IF([test "x$enable_jack" != xno],
      [AC_DEFINE([AGS_WITH_JACK], [1], [JACK enabled])

      PKG_CHECK_MODULES([JACK], [jack])
      AC_SUBST(JACK_CFLAGS)
      AC_SUBST(JACK_LIBS)

      AC_CHECK_FUNC([jack_port_uuid],
		    [AC_DEFINE([HAVE_JACK_PORT_UUID], [1], [Jack port uuid available])])
      AC_MSG_NOTICE([JACK enabled])],
      AC_MSG_NOTICE([JACK disabled]))

AS_IF([test "x$enable_pulse" != xno],
      [AC_DEFINE([AGS_WITH_PULSE], [1], [PULSE enabled])

      PKG_CHECK_MODULES([PULSE], [libpulse-mainloop-glib])
      AC_SUBST(PULSE_CFLAGS)
      AC_SUBST(PULSE_LIBS)

      AC_MSG_NOTICE([PULSE enabled])],
      AC_MSG_NOTICE([PULSE disabled]))

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

PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.32.3])
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES([GOBJECT], [gobject-2.0 >= 2.32.3 glib-2.0 >= 2.32.3 gmodule-2.0 >= 2.32.3 gthread-2.0 >= 2.32.3])
AC_SUBST(GOBJECT_CFLAGS)
AC_SUBST(GOBJECT_LIBS)

AC_CHECK_LIB(glib-2.0, g_strv_length,
	     AC_DEFINE([HAVE_GLIB_2_6], [1], [GLib 2.6 available]))
AC_CHECK_LIB(glib-2.0, g_strv_contains,
	     AC_DEFINE([HAVE_GLIB_2_44], [1], [GLib 2.44 available]))
AC_CHECK_LIB(glib-2.0, g_object_setv,
	     AC_DEFINE([HAVE_GLIB_2_54], [1], [GLib 2.54 available]))


PKG_CHECK_MODULES([GIO], [gio-2.0 >= 2.32.3 glib-2.0 >= 2.32.3 gmodule-2.0 >= 2.32.3 gthread-2.0 >= 2.32.3])
AC_SUBST(GIO_CFLAGS)
AC_SUBST(GIO_LIBS)

AC_CHECK_LIB(atk-1.0, atk_value_get_value_and_text,
             [AC_DEFINE([HAVE_ATK_2_12], [1], [Atk 2.12 available])])

AS_IF([test "x$enable_libinstpatch" != xno],
      [PKG_CHECK_MODULES([LIBINSTPATCH], [libinstpatch-1.0 >= 1.0])
      AC_SUBST(LIBINSTPATCH_CFLAGS)
      AC_SUBST(LIBINSTPATCH_LIBS)
      AC_DEFINE([AGS_WITH_LIBINSTPATCH], [1], [Disable if you don't want libinstpatch])],
      AC_MSG_NOTICE([libinstpatch disabled]))
AM_CONDITIONAL([WITH_LIBINSTPATCH], [test "x$enable_libinstpatch" == xyes])

AS_IF([test "x$enable_vst3" != xno],
      [AC_DEFINE([AGS_WITH_VST3], [1], [VST3 enabled])

      AC_SUBST(VST3_CXXFLAGS)
      AC_SUBST(VST3_LIBS)
      
      AC_MSG_NOTICE([VST3 enabled])],
      AC_MSG_NOTICE([VST3 disabled]))

PKG_CHECK_MODULES([FONTCONFIG], [fontconfig >= 2.9.0])
AC_SUBST(FONTCONFIG_CFLAGS)
AC_SUBST(FONTCONFIG_LIBS)

PKG_CHECK_MODULES([GDKPIXBUF], [gdk-pixbuf-2.0 >= 2.32.2])
AC_SUBST(GDKPIXBUF_CFLAGS)
AC_SUBST(GDKPIXBUF_LIBS)

PKG_CHECK_MODULES([CAIRO], [cairo >= 1.12.0])
AC_SUBST(CAIRO_CFLAGS)
AC_SUBST(CAIRO_LIBS)

PKG_CHECK_MODULES([GTK], [gtk+-2.0 >= 2.24.10])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

AS_IF([test "x$enable_xmlrpc" != xno],
      [AC_CHECK_PROG([XMLRPC_C_CONFIG], [xmlrpc-c-config], [1], [0], [$PATH], [1])
      AC_SUBST(XMLRPC_UTIL_CFLAGS, $(xmlrpc-c-config server-util --cflags))
      AC_SUBST(XMLRPC_UTIL_LIBS, $(xmlrpc-c-config server-util --libs))
      AC_SUBST(XMLRPC_CLIENT_CFLAGS, $(xmlrpc-c-config client --cflags))
      AC_SUBST(XMLRPC_CLIENT_LIBS, $(xmlrpc-c-config client --libs))
#      AC_SUBST(XMLRPC_SERVER_CFLAGS, $(xmlrpc-c-config abyss --cflags))
#      AC_SUBST(XMLRPC_SERVER_LIBS, $(xmlrpc-c-config abyss --libs))
      AC_SUBST(XMLRPC_SERVER_CGI_CFLAGS, $(xmlrpc-c-config cgi-server --cflags))
      AC_SUBST(XMLRPC_SERVER_CGI_LIBS, $(xmlrpc-c-config cgi-server --libs))
      AC_SUBST(XMLRPC_SERVER_ABYSS_CFLAGS, $(xmlrpc-c-config abyss-server --cflags))
      AC_SUBST(XMLRPC_SERVER_ABYSS_LIBS, $(xmlrpc-c-config abyss-server --libs))
      AC_MSG_NOTICE([xmlrpc-c enabled])
      AC_DEFINE([AGS_WITH_XMLRPC_C], [1], [Disable if you don't want xmlrpc-c])],
      AC_MSG_NOTICE([xmlrpc-c disabled]))

AS_IF([test "x$enable_run_functional_tests" == xyes],
      [AC_MSG_NOTICE([run functional tests enabled])],
      AC_MSG_NOTICE([run functional tests disabled]))
AM_CONDITIONAL([RUN_FUNCTIONAL_TESTS], [test "x$enable_run_functional_tests" == xyes])

AS_IF([test "x$enable_fast_test_mode" == xyes],
      [AC_MSG_NOTICE([fast test mode enabled])],
      AC_MSG_NOTICE([fast test mode disabled]))
AM_CONDITIONAL([FAST_TEST_MODE], [test "x$enable_fast_test_mode" == xyes])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([__argz_count __argz_next __argz_stringify atexit clock_gettime floor getcwd localeconv mblen mbrlen memchr mempcpy memset munmap nl_langinfo setlocale socket sqrt stpcpy strcasecmp strchr strcspn strdup strstr strtol strtoul])

# check for gtk-doc
m4_ifdef([GTK_DOC_CHECK], [
GTK_DOC_CHECK([1.14],[--flavour no-tmpl])
],[
AM_CONDITIONAL([ENABLE_GTK_DOC], false)
])

AM_CONDITIONAL([WITH_SYSTEM_TESTS], [test "x$enable_run_system_tests" == xyes])
AM_CONDITIONAL([WITH_VST3], [test "x$enable_vst3" == xyes])
AM_CONDITIONAL([WITH_EXPERIMENTAL], [test "x$enable_experimental" == xyes])

LIBAGS_SYMBOLS([])
LIBAGS_THREAD_SYMBOLS([])
LIBAGS_SERVER_SYMBOLS([])
LIBAGS_AUDIO_SYMBOLS([])
LIBAGS_GUI_SYMBOLS([])

LIBAGS_MARSHALLERS([])
LIBAGS_GUI_MARSHALLERS([])

AGS_DEVELOPER_BOOK_LISTINGS([])

AGS_DOCS_API([])

# gl_INIT

AM_EXTRA_RECURSIVE_TARGETS([ags-docs])
AC_CONFIG_FILES([
Makefile
unit-system-tests.mk
functional-system-tests.mk
docs/reference/libags/Makefile
docs/reference/libags-audio/Makefile
docs/reference/libags-gui/Makefile
docs/reference/libgsequencer/Makefile
m4/Makefile
po/Makefile.in
libags.pc
libags_audio.pc
libags_gui.pc
libgsequencer.pc])

# lib/Makefile

AC_OUTPUT
