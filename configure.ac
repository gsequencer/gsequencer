#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ags],[0.4.2-69],[jkraehemann@gmail.com])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lX11':
AC_CHECK_LIB([X11], [main])
# FIXME: Replace `main' with a function in `-ldl':
AC_CHECK_LIB([dl], [main])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
# FIXME: Replace `main' with a function in `-lrt':
AC_CHECK_LIB([rt], [main])

# Checks for header files.
AC_PATH_X
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([argz.h fcntl.h float.h inttypes.h langinfo.h libintl.h limits.h locale.h malloc.h netinet/in.h stddef.h stdint.h stdio_ext.h stdlib.h string.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h unistd.h])

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

PKG_CHECK_MODULES([LIBAO], [ao >= 1.1.0])
AC_SUBST(LIBAO_CFLAGS)
AC_SUBST(LIBAO_LIBS)

PKG_CHECK_MODULES([LIBASOUND2], [alsa >= 1.0.25])
AC_SUBST(LIBASOUND2_CFLAGS)
AC_SUBST(LIBASOUND2_LIBS)

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
# AM_GNU_GETTEXT
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([__argz_count __argz_next __argz_stringify atexit clock_gettime floor getcwd localeconv mblen mbrlen memchr mempcpy memset munmap nl_langinfo setlocale socket sqrt stpcpy strcasecmp strchr strcspn strdup strstr strtol strtoul])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
