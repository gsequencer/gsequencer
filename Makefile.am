AUTOMAKE_OPTIONS = foreign

top_srcdir = $(shell pwd)/src/ags

SUBDIRS = src/ags \
	doc

include ./src/ags/Makefile.am \
	$(top_srcdir)/util/Makefile.am \
	$(top_srcdir)/lib/Makefile.am \
	$(top_srcdir)/object/Makefile.am \
	$(top_srcdir)/widget/Makefile.am \
	$(top_srcdir)/audio/Makefile.am \
	$(top_srcdir)/audio/task/Makefile.am \
	$(top_srcdir)/audio/file/Makefile.am \
	$(top_srcdir)/audio/recall/Makefile.am \
	$(top_srcdir)/audio/task/recall/Makefile.am \
	$(top_srcdir)/file/Makefile.am \
	$(top_srcdir)/X/Makefile.am \
	$(top_srcdir)/X/editor/Makefile.am \
	$(top_srcdir)/X/machine/Makefile.am \
	$(top_srcdir)/file/Makefile.am \
	$(top_srcdir)/server/Makefile.am \
	./doc/Makefile.am

CC = gcc

# what flags you want to pass to the C compiler & linker
CFLAGS = -g --pedantic -Wall -O -I./src -I/usr/include
LDFLAGS = -L/lib -L/usr/lib -L/usr/X11/lib -lm -pthread -lrt -lgmp

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
noinst_LIBRARIES = libags.a libags-thread.a libags-audio.a
bin_PROGRAMS = gsequencer

# library
libags_a_CFLAGS=
libags_a_LDFLAGS =

# application
gsequencer_CFLAGS=
gsequencer_LDFLAGS=
gsequencer_LDADD=libags.a libags-thread.a libags-audio.a

# 
libags_audio_h_sources = $(ags_recall_audio_h_sources) $(ags_audio_task_h_sources) $(ags_xorg_task_h_sources) $(ags_audio_file_h_sources) $(ags_audio_recall_task_h_sources) 
libags_audio_c_sources = $(ags_recall_audio_c_sources) $(ags_audio_task_c_sources) $(ags_xorg_task_c_sources) $(ags_audio_file_c_sources) $(ags_audio_recall_task_c_sources) 

gsequencer_h_sources = $(ags_xorg_h_sources) $(ags_machine_h_sources) $(ags_editor_h_sources)
gsequencer_c_sources = $(ags_xorg_c_sources) $(ags_machine_c_sources) $(ags_editor_c_sources)

libags_a_SOURCES = $(ags_lib_c_sources) $(ags_object_c_sources) $(ags_util_c_sources)
libags_thread_a_SOURCES = $(ags_thread_c_sources)
libags_audio_a_SOURCES = $(ags_audio_c_sources)
libags_gui_a_SOURCES = $(ags_widget_c_sources)

gsequencer_SOURCES = $(gsequencer_c_sources) $(ags_server_c_sources) $(ags_file_c_sources)

