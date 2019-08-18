# This is a blank Makefile.am for using gtk-doc.
# Copy this to your project's API docs directory and modify the variables to
# suit your project. See the GTK+ Makefiles in gtk+/docs/reference for examples
# of using the various options.

# The name of the module, e.g. 'glib'.
DOC_MODULE=libags

# Uncomment for versioned docs and specify the version of the module, e.g. '2'.
#DOC_MODULE_VERSION=2

# The top-level XML file.
DOC_MAIN_SGML_FILE=$(DOC_MODULE).xml

# Directories containing the source code.
# gtk-doc will search all .c and .h files beneath these paths
# for inline comments documenting functions and macros.
# e.g. DOC_SOURCE_DIR=$(top_srcdir)/gtk $(top_srcdir)/gdk
DOC_SOURCE_DIR=$(top_srcdir)/ags/util $(top_srcdir)/ags/lib $(top_srcdir)/ags/object $(top_srcdir)/ags/file $(top_srcdir)/ags/thread $(top_srcdir)/ags/server

# Extra options to pass to gtkdoc-scangobj. Normally not needed.
SCANGOBJ_OPTIONS=--cflags="$(CFLAGS) $(UUID_CFLAGS) $(LIBXML2_CFLAGS) $(OPENSSL_CFLAGS) $(XMLRPC_CFLAGS) $(XMLRPC_UTIL_CFLAGS) $(XMLRPC_SERVER_ABYSS_CFLAGS) $(XMLRPC_SERVER_CGI_CFLAGS) $(XMLRPC_SERVER_CFLAGS) $(GOBJECT_CFLAGS)" --ldflags="-static -ldl -lm -lrt -pthread $(LDFLAGS) $(UUID_LIBS) $(LIBXML2_LIBS) $(OPENSSL_LIBS) $(XMLRPC_LIBS) $(XMLRPC_UTIL_LIBS) $(XMLRPC_SERVER_ABYSS_LIBS) $(XMLRPC_SERVER_CGI_LIBS) $(XMLRPC_SERVER_LIBS) $(GOBJECT_LIBS) -L$(top_builddir) $(top_builddir)/libags.la $(top_builddir)/libags_thread.la $(top_builddir)/libags_server.la"

# Extra options to supply to gtkdoc-scan.
# e.g. SCAN_OPTIONS=--deprecated-guards="GTK_DISABLE_DEPRECATED"
# SCAN_OPTIONS=--rebuild-types --rebuild-sections

# Extra options to supply to gtkdoc-mkdb
# e.g. MKDB_OPTIONS=--xml-mode --output-format=xml
MKDB_OPTIONS=--xml-mode --output-format=xml --name-space=libags --module=libags

# Extra options to supply to gtkdoc-mkhtml
MKHTML_OPTIONS=

# Extra options to supply to gtkdoc-fixref. Normally not needed.
# e.g. FIXXREF_OPTIONS=--extra-dir=../gdk-pixbuf/html --extra-dir=../gdk/html
FIXXREF_OPTIONS=--module=libags --module-dir=./libags-html/ --html-dir=$(DESTDIR)/$(datadir)/doc/libags-doc/api

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
	$(top_builddir)/libags.la \
	$(top_builddir)/libags_thread.la \
	$(top_builddir)/libags_server.la

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
