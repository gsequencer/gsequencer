# Makefile.am - template makefile for gtk-doc module
# Copyright (C) 2007-2017  Stefan Sauer
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# As a special exception, the above copyright owner gives unlimited
# permission to copy, distribute and modify this Makefile.am template.
# You need not follow the terms of the GNU General Public License when
# using or distributing such Makefile.am files, even though portions of
# the text of the Makefile.am appear in them. The GNU General Public
# License (GPL) does govern all other use of the material that constitutes
# the Makefile.am template.

# This is a blank Makefile.am for using gtk-doc.
# Copy this to your project's API docs directory and modify the variables to
# suit your project. See the GTK+ Makefiles in gtk+/docs/reference for examples
# of using the various options.

# The name of the module, e.g. 'glib'.
DOC_MODULE=libags_gui

# Uncomment for versioned docs and specify the version of the module, e.g. '2'.
#DOC_MODULE_VERSION=2

# The top-level XML file.
DOC_MAIN_SGML_FILE=$(DOC_MODULE).xml

HTML_DIR=libags_gui

# Directories containing the source code.
# gtk-doc will search all .c and .h files beneath these paths
# for inline comments documenting functions and macros.
# e.g. DOC_SOURCE_DIR=$(top_srcdir)/gtk $(top_srcdir)/gdk
DOC_SOURCE_DIR=$(top_srcdir)/ags/widget

# Extra options to pass to gtkdoc-scangobj. Normally not needed.
SCANGOBJ_OPTIONS=--cflags="$(CFLAGS) $(GOBJECT_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)" --ldflags="-static -ldl -lm -lrt -pthread $(LDFLAGS) $(FONTCONFIG_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS) -L$(top_builddir) $(top_builddir)/libags_gui.la"

# Extra options to supply to gtkdoc-scan.
# e.g. SCAN_OPTIONS=--deprecated-guards="GTK_DISABLE_DEPRECATED"
# SCAN_OPTIONS=--rebuild-types --rebuild-sections
SCAN_OPTIONS=--rebuild-sections --module=libags_gui --deprecated-guards="G_DISABLE_DEPRECATED"

# Extra options to supply to gtkdoc-mkdb
# e.g. MKDB_OPTIONS=--xml-mode --output-format=xml
MKDB_OPTIONS=--xml-mode --output-format=xml --name-space=ags --module=libags_gui --default-stability=Stable

# Extra options to supply to gtkdoc-mkhtml
MKHTML_OPTIONS=

# Extra options to supply to gtkdoc-fixref. Normally not needed.
# e.g. FIXXREF_OPTIONS=--extra-dir=../gdk-pixbuf/html --extra-dir=../gdk/html
FIXXREF_OPTIONS=--module=libags_gui --module-dir=./libags-gui-html/ --html-dir=$(DESTDIR)/$(datadir)/doc/libags-gui-doc/api

# Used for dependencies. The docs will be rebuilt if any of these change.
# e.g. HFILE_GLOB=$(top_srcdir)/gtk/*.h
# e.g. CFILE_GLOB=$(top_srcdir)/gtk/*.c
HFILE_GLOB=
CFILE_GLOB=

# Extra header to include when scanning, which are not under DOC_SOURCE_DIR
# e.g. EXTRA_HFILES=$(top_srcdir}/contrib/extra.h
EXTRA_HFILES=

# Header files or dirs to ignore when scanning. Use base file/dir names
# e.g. IGNORE_HFILES=gtkdebug.h gtkintl.h private_code
IGNORE_HFILES=

# Images to copy into HTML directory.
# e.g. HTML_IMAGES=$(top_srcdir)/gtk/stock-icons/stock_about_24.png
HTML_IMAGES=

# Extra files that are included by $(DOC_MAIN_SGML_FILE).
# e.g. content_files=running.xml building.xml changes-2.0.xml
content_files=

# Files where gtk-doc abbrevations (#GtkWidget) are expanded
# e.g. expand_content_files=running.xml
expand_content_files=

AM_CPPFLAGS = \
	-I$(top_srcdir)	\
	-I$(top_builddir)

# CFLAGS and LDFLAGS for compiling gtkdoc-scangobj with your library.
# Only needed if you are using gtkdoc-scangobj to dynamically query widget
# signals and properties.
# e.g. GTKDOC_CFLAGS=-I$(top_srcdir) -I$(top_builddir) $(GTK_DEBUG_FLAGS)
# e.g. GTKDOC_LIBS=$(top_builddir)/gtk/$(gtktargetlib)
GTKDOC_CFLAGS=$(gsequencer_CFLAGS) -DGTK_IS_WIDGET_CLASS=1
GTKDOC_LIBS = \
	$(top_builddir)/libags_gui.la

# This includes the standard gtk-doc make rules, copied by gtkdocize.
include ./gtk-doc.make

# Comment this out if you want 'make check' to test you doc status
# and run some sanity checks
if ENABLE_GTK_DOC
TESTS_ENVIRONMENT = \
  DOC_MODULE=$(DOC_MODULE) DOC_MAIN_SGML_FILE=$(DOC_MAIN_SGML_FILE) \
  SRCDIR=$(abs_srcdir) BUILDDIR=$(abs_builddir)
TESTS = $(GTKDOC_CHECK)
endif

-include $(top_srcdir)/git.mk
