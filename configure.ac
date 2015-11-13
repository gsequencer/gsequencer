#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([gsequencer],[0.7.1],[jkraehemann-guest@users.alioth.debian.org])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR(m4)

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AM_PROG_CC_STDC
AC_C_CONST
AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL

# check for gtk-doc
m4_ifdef([GTK_DOC_CHECK], [
GTK_DOC_CHECK([1.14],[--flavour no-tmpl])
],[
AM_CONDITIONAL([ENABLE_GTK_DOC], false)
])

# libtool
LT_PREREQ([2.4.2])
LT_INIT
AC_SUBST([LIBTOOL_DEPS])
AC_ENABLE_SHARED

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lgmp':
AC_CHECK_LIB([gmp], [main])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
# FIXME: Replace `main' with a function in `-lrt':
AC_CHECK_LIB([rt], [main])

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([fcntl.h libintl.h netinet/in.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UID_T

PKG_CHECK_MODULES([UUID], [uuid >= 1.0.2])
AC_SUBST(UUID_CFLAGS)
AC_SUBST(UUID_LIBS)

PKG_CHECK_MODULES([LIBAO], [ao >= 1.1.0])
AC_SUBST(LIBAO_CFLAGS)
AC_SUBST(LIBAO_LIBS)

PKG_CHECK_MODULES([LIBASOUND2], [alsa >= 1.0.25])
AC_SUBST(LIBASOUND2_CFLAGS)
AC_SUBST(LIBASOUND2_LIBS)

PKG_CHECK_MODULES([JACK], [jack >= 0.124.2])
AC_SUBST(JACK_CFLAGS)
AC_SUBST(JACK_LIBS)

PKG_CHECK_MODULES([LIBXML2], [libxml-2.0 >= 2.8.0])
AC_SUBST(LIBXML2_CFLAGS)
AC_SUBST(LIBXML2_LIBS)

PKG_CHECK_MODULES([SNDFILE], [sndfile >= 1.0.25])
AC_SUBST(SNDFILE_CFLAGS)
AC_SUBST(SNDFILE_LIBS)

PKG_CHECK_MODULES([LIBINSTPATCH], [libinstpatch-1.0 >= 1.0])
AC_SUBST(LIBINSTPATCH_CFLAGS)
AC_SUBST(LIBINSTPATCH_LIBS)

PKG_CHECK_MODULES([GOBJECT], [gobject-2.0 >= 2.32.3 glib-2.0 >= 2.32.3 gmodule-2.0 >= 2.32.3 gthread-2.0 >= 2.32.3])
AC_SUBST(GOBJECT_CFLAGS)
AC_SUBST(GOBJECT_LIBS)

PKG_CHECK_MODULES([FONTCONFIG], [fontconfig >= 2.11.0])
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

#PKG_CHECK_MODULES([XMLRPC_UTIL], [xmlrpc_util >= 1.16.33])
#AC_SUBST(XMLRPC_UTIL_CFLAGS)
#AC_SUBST(XMLRPC_UTIL_LIBS)

#PKG_CHECK_MODULES([XMLRPC_SERVER], [xmlrpc_client >= 1.#16.33])
#AC_SUBST(XMLRPC_SERVER_CFLAGS)
#AC_SUBST(XMLRPC_SERVER_LIBS)

#PKG_CHECK_MODULES([XMLRPC_SERVER_CGI], [xmlrpc_server_cgi >= 1.16.33])
#AC_SUBST(XMLRPC_SERVER_CGI_CFLAGS)
#AC_SUBST(XMLRPC_SERVER_CGI_LIBS)

#PKG_CHECK_MODULES([XMLRPC_SERVER_ABYSS], [xmlrpc_server_abyss >= 1.16.33])
#AC_SUBST(XMLRPC_SERVER_ABYSS_CFLAGS)
#AC_SUBST(XMLRPC_SERVER_ABYSS_LIBS)$

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([atexit clock_gettime floor memset socket sqrt strchr strtol strtoul])

AC_OUTPUT

