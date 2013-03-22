#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([ags], [0.3.16-SNAPSHOT], [joel@weedlight.ch])
AM_INIT_AUTOMAKE([ags], [0.3.15-SNAPSHOT])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-ldl':
AC_CHECK_LIB([dl], [main])
# FIXME: Replace `main' with a function in `-lfontconfig':
AC_CHECK_LIB([fontconfig], [main])
# FIXME: Replace `main' with a function in `-lfreetype':
AC_CHECK_LIB([freetype], [main])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
# FIXME: Replace `main' with a function in `-lrt':
AC_CHECK_LIB([rt], [main])


PKG_CHECK_MODULES(LIBASOUND2, alsa >= 1.0.25)
AC_SUBST(LIBASOUND2_CFLAGS)
AC_SUBST(LIBASOUND2_LIBS)

PKG_CHECK_MODULES(LIBAO, ao >= 1.1.0)
AC_SUBST(LIBAO_CFLAGS)
AC_SUBST(LIBAO_LIBS)

PKG_CHECK_MODULES(LIBXML2, libxml-2.0 >= 2.8.0)
AC_SUBST(LIBXML2_CFLAGS)
AC_SUBST(LIBXML2_LIBS)

PKG_CHECK_MODULES(SNDFILE, sndfile >= 1.0.25)
AC_SUBST(SNDFILE_CFLAGS)
AC_SUBST(SNDFILE_LIBS)

PKG_CHECK_MODULES(LIBINSTPATCH, libinstpatch-1.0 >= 1.0)
AC_SUBST(LIBINSTPATCH_CFLAGS)
AC_SUBST(LIBINSTPATCH_LIBS)

PKG_CHECK_MODULES(GOBJECT, gobject-2.0 >= 2.32.3 glib-2.0 >= 2.32.3 gmodule-2.0 >= 2.32.3 gthread-2.0 >= 2.32.3)
AC_SUBST(GOBJECT_CFLAGS)
AC_SUBST(GOBJECT_LIBS)

PKG_CHECK_MODULES(GDKPIXBUF, gdk-pixbuf-xlib-2.0 >= 2.26.1)
AC_SUBST(GDKPIXBUF_CFLAGS)
AC_SUBST(GDKPIXBUF_LIBS)

PKG_CHECK_MODULES(CAIRO, cairo >= 1.12.0)
AC_SUBST(CAIRO_CFLAGS)
AC_SUBST(CAIRO_LIBS)

PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.24.10)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/ioctl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([clock_gettime floor memset strtol strtoul])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
