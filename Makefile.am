# AUTOMAKE_OPTIONS = foreign

top_srcdir = $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/src/ags

SUBDIRS = po \
	src/ags \
	src/ags/util \
	src/ags/lib \
	src/ags/object \
	src/ags/widget \
	src/ags/audio \
	src/ags/audio/task \
	src/ags/audio/task/recall \
	src/ags/audio/file \
	src/ags/audio/recall \
	src/ags/file \
	src/ags/X \
	src/ags/file \
	src/ags/server \
	src/ags/X/editor \
	src/ags/X/machine \
	doc


TARGETS = gsequencer$(EXEEXT)

# what flags you want to pass to the C compiler & linker
CFLAGS = -g --pedantic -Wall -O -I./src -I/usr/include
LDFLAGS = -L/lib -L/usr/lib -L/usr/X11/lib -lm -pthread -lrt -lgmp

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
noinst_LIBRARIES = libags.a libags-thread.a libags-audio.a libags-gui.a
bin_PROGRAMS = gsequencer

# application
gsequencer_CFLAGS =
gsequencer_LDFLAGS =
gsequencer_LDADD = libags.a libags-thread.a libags-audio.a libags-gui.a

#
libags_audio_h_sources =
libags_audio_c_sources =

libags_a_SOURCES = 
libags_thread_a_SOURCES = 
libags_audio_a_SOURCES = 
libags_gui_a_SOURCES = 

gsequencer_SOURCES = 


ACLOCAL_AMFLAGS = -I m4
