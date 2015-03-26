AUTOMAKE_OPTIONS = foreign

TARGETS = gsequencer

# what flags you want to pass to the C compiler & linker
CFLAGS = -g --pedantic -Wall -O -I./src -I/usr/include
LDFLAGS = -L/lib -L/usr/lib -L/usr/X11/lib -lm -pthread -lrt -lgmp

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
noinst_LIBRARIES = libags.a libags_thread.a libags_audio.a libags_gui.a
bin_PROGRAMS = gsequencer

# application
gsequencer_CFLAGS = $(UUID_CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
gsequencer_LDFLAGS = $(UUID_LIBS) $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS)
gsequencer_LDADD = libags.a libags_thread.a libags_audio.a libags_gui.a

#
libags_a_SOURCES = 
libags_thread_a_SOURCES = 
libags_audio_a_SOURCES = 
libags_gui_a_SOURCES = 

gsequencer_SOURCES = 

include po/Makefile.am \
	src/ags/util/Makefile.am \
	src/ags/lib/Makefile.am \
	src/ags/object/Makefile.am \
	src/ags/widget/Makefile.am \
	src/ags/audio/task/Makefile.am \
	src/ags/audio/task/recall/Makefile.am \
	src/ags/audio/file/Makefile.am \
	src/ags/audio/recall/Makefile.am \
	src/ags/audio/Makefile.am \
	src/ags/file/Makefile.am \
	src/ags/server/Makefile.am \
	src/ags/X/editor/Makefile.am \
	src/ags/X/machine/Makefile.am \
	src/ags/X/Makefile.am \
	src/ags/Makefile.am \
	doc/Makefile.am

SUBDIRS = po \
	src/ags/util \
	src/ags/lib \
	src/ags/object \
	src/ags/widget \
	src/ags/audio/task \
	src/ags/audio/task/recall \
	src/ags/audio/file \
	src/ags/audio/recall \
	src/ags/audio \
	src/ags/file \
	src/ags/server \
	src/ags/X/editor \
	src/ags/X/machine \
	src/ags/X \
	src/ags \
	doc

ACLOCAL_AMFLAGS = -I m4
