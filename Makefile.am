AUTOMAKE_OPTIONS = foreign

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
libags_h_sources = $(libags_util_h_sources) $(libags_object_h_sources)
libags_c_sources = $(libags_util_c_sources) $(libags_object_c_sources)

libags_thread_h_sources =
libags_thread_c_sources =

libags_gui_h_sources =
libags_gui_c_sources =

gsequencer_h_sources = $(gsequencer_main_h_sources)
gsequencer_c_sources = $(gsequencer_main_c_sources)

libags_a_SOURCES = $(libags_c_sources)
libags_thread_a_SOURCES = 
libags_audio_a_SOURCES = 
libags_gui_a_SOURCES = 

gsequencer_SOURCES = $(gsequencer_c_sources)
