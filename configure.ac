#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ags], [0.3.15-SNAPSHOT], [joel@weedlight.ch])
AM_INIT_AUTOMAKE([ags], [0.3.15-SNAPSHOT])
AC_CONFIG_SRCDIR([src/ags/main.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lao':
AC_CHECK_LIB([ao], [main])
# FIXME: Replace `main' with a function in `-lasound':
AC_CHECK_LIB([asound], [main])
# FIXME: Replace `main' with a function in `-lcairo':
AC_CHECK_LIB([cairo], [main])
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
# FIXME: Replace `main' with a function in `-lsndfile':
AC_CHECK_LIB([sndfile], [main])
# FIXME: Replace `main' with a function in `-lxml2':
AC_CHECK_LIB([xml2], [main])

PKG_CHECK_MODULES(LIBINSTPATCH, libinstpatch-1.0 >= 1.0)
AC_SUBST(LIBINSTPATCH_CFLAGS)
AC_SUBST(LIBINSTPATCH_LIBS)

PKG_CHECK_MODULES(GOBJECT, gobject-2.0 >= 2.12 glib-2.0 >= 2.12 gmodule-2.0 >= 2.12 gthread-2.0 >= 2.12)
AC_SUBST(GOBJECT_CFLAGS)
AC_SUBST(GOBJECT_LIBS)

PKG_CHECK_MODULES(GDKPIXBUF, gdk-pixbuf-xlib-2.0 >= 2.24.0)
AC_SUBST(GDKPIXBUF_CFLAGS)
AC_SUBST(GDKPIXBUF_LIBS)

PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.24.0)
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
