#                                               -*- Autoconf -*-
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([gsequencer],[0.9.9],[jkraehemann-guest@users.alioth.debian.org])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR([ags/config.h.in])
AC_CONFIG_HEADERS([ags/config.h])
AC_CONFIG_MACRO_DIR([m4])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.8])

# options
AC_ARG_ENABLE(alsa, [AS_HELP_STRING([--enable-alsa],
				    [enable alsa (default is yes)])],
	      [],
	      [enable_alsa=yes])

AC_ARG_ENABLE(oss, [AS_HELP_STRING([--enable-oss],
				   [enable oss (default is yes)])],
	      [],
	      [enable_oss=yes])

AC_ARG_ENABLE(libinstpatch, [AS_HELP_STRING([--enable-libinstpatch],
					    [enable libinstpatch (default is yes)])],
	      [],
	      [enable_libinstpatch=yes])

AC_ARG_ENABLE(xmlrpc, [AS_HELP_STRING([--enable-xmlrpc],
				      [enable xmlrpc (default is no)])],
	      [],
	      [enable_xmlrpc=no])

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL

# libtool
LT_PREREQ([2.4.2])
LT_INIT
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
AC_CHECK_LIB([X11], [main], , AC_MSG_ERROR("No X11 library present"))
# FIXME: Replace `main' with a function in `-ldl':
AC_CHECK_LIB([dl], [main], , AC_MSG_ERROR("No dl library present"))
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main], , AC_MSG_ERROR("No math library present"))
# FIXME: Replace `main' with a function in `-lrt':
AC_CHECK_LIB([rt], [main], , AC_MSG_ERROR("No rt library present"))

# Checks for header files.
AC_PATH_X
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([argz.h fcntl.h float.h inttypes.h langinfo.h libintl.h limits.h locale.h malloc.h netinet/in.h stddef.h stdint.h stdio_ext.h stdlib.h string.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h unistd.h ladspa.h dssi.h lv2.h], , AC_MSG_ERROR("missing headers"))

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

PKG_CHECK_MODULES([UUID], [uuid >= 1.0.2])
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

PKG_CHECK_MODULES([JACK], [jack])
AC_SUBST(JACK_CFLAGS)
AC_SUBST(JACK_LIBS)

AC_CHECK_FUNC([jack_port_uuid],
	      [AC_DEFINE([HAVE_JACK_PORT_UUID], [1], [Jack port uuid available])])

PKG_CHECK_MODULES([PULSE], [libpulse])
AC_SUBST(PULSE_CFLAGS)
AC_SUBST(PULSE_LIBS)

PKG_CHECK_MODULES([LIBXML2], [libxml-2.0 >= 2.8.0])
AC_SUBST(LIBXML2_CFLAGS)
AC_SUBST(LIBXML2_LIBS)

PKG_CHECK_MODULES([SAMPLERATE], [samplerate >= 0.1.8])
AC_SUBST(SAMPLERATE_CFLAGS)
AC_SUBST(SAMPLERATE_LIBS)

PKG_CHECK_MODULES([SNDFILE], [sndfile >= 1.0.25])
AC_SUBST(SNDFILE_CFLAGS)
AC_SUBST(SNDFILE_LIBS)

PKG_CHECK_MODULES([GOBJECT], [gobject-2.0 >= 2.32.3 glib-2.0 >= 2.32.3 gmodule-2.0 >= 2.32.3 gthread-2.0 >= 2.32.3])
AC_SUBST(GOBJECT_CFLAGS)
AC_SUBST(GOBJECT_LIBS)

AC_CHECK_LIB(glib-2.0, g_strv_length,
	     AC_DEFINE([HAVE_GLIB_2_6], [1], [GLib 2.6 available]))
AC_CHECK_LIB(glib-2.0, g_strv_contains,
	     AC_DEFINE([HAVE_GLIB_2_44], [1], [GLib 2.44 available]))

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

PKG_CHECK_MODULES([FONTCONFIG], [fontconfig >= 2.9.0])
AC_SUBST(FONTCONFIG_CFLAGS)
AC_SUBST(FONTCONFIG_LIBS)

PKG_CHECK_MODULES([GDKPIXBUF], [gdk-pixbuf-2.0 >= 2.26.1])
AC_SUBST(GDKPIXBUF_CFLAGS)
AC_SUBST(GDKPIXBUF_LIBS)

PKG_CHECK_MODULES([CAIRO], [cairo >= 1.12.0])
AC_SUBST(CAIRO_CFLAGS)
AC_SUBST(CAIRO_LIBS)

PKG_CHECK_MODULES([GTK], [gtk+-2.0 >= 2.24.10])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

AS_IF([test "x$enable_xmlrpc" != xno],
      [PKG_CHECK_MODULES([XMLRPC_UTIL], [xmlrpc_util >= 1.16.33])
      AC_SUBST(XMLRPC_UTIL_CFLAGS)
      AC_SUBST(XMLRPC_UTIL_LIBS)
      PKG_CHECK_MODULES([XMLRPC_SERVER], [xmlrpc_client >= 1.16.33])
      AC_SUBST(XMLRPC_SERVER_CFLAGS)
      AC_SUBST(XMLRPC_SERVER_LIBS)
      PKG_CHECK_MODULES([XMLRPC_SERVER_CGI], [xmlrpc_server_cgi >= 1.16.33])
      AC_SUBST(XMLRPC_SERVER_CGI_CFLAGS)
      AC_SUBST(XMLRPC_SERVER_CGI_LIBS)
      PKG_CHECK_MODULES([XMLRPC_SERVER_ABYSS], [xmlrpc_server_abyss >= 1.16.33])
      AC_SUBST(XMLRPC_SERVER_ABYSS_CFLAGS)
      AC_SUBST(XMLRPC_SERVER_ABYSS_LIBS)
      AC_DEFINE([AGS_WITH_XMLRPC_C], [1], [Disable if you don't want xmlrpc-c])],
      AC_MSG_NOTICE([xmlrpc-c disabled]))

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

AM_EXTRA_RECURSIVE_TARGETS([ags-docs])
AC_CONFIG_FILES([
Makefile
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

AC_OUTPUT
