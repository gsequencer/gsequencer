# Copyright (C) 2005-2022 Joel Kraehemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

libgsequencer_test_LIBADD = @LIBGSEQUENCER_TEST_LIBADD@
gsequencer_functional_test_LDADD = @GSEQUENCER_FUNCTIONAL_TEST_LDADD@

# libadd and ldadd
libgsequencer_test_LIBADD += libgsequencer.la libags_gui.la libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS) $(FONTCONFIG_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS)  $(WEBKITGTK_LIBS) $(POPPLER_LIBS) $(GTK_MAC_INTEGRATION_LIBS) $(W32API_LIBS)

gsequencer_functional_test_LDADD += libgsequencer.la libags_gui.la libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la libgsequencer_test.la -lcunit -lrt -lm -lXtst $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS) $(FONTCONFIG_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS)  $(WEBKITGTK_LIBS) $(POPPLER_LIBS) $(GTK_MAC_INTEGRATION_LIBS) $(W32API_LIBS)

check_LTLIBRARIES = libgsequencer_test.la

#TODO:JK: fix tests after refactoring
# functional tests
check_PROGRAMS += \
	ags_functional_server_test \
	ags_functional_audio_test \
	ags_functional_pitch_test \
	ags_functional_fast_pitch_test \
	ags_functional_fourier_transform_test \
	ags_functional_osc_server_test \
	ags_functional_osc_xmlrpc_server_test \
	ags_functional_machine_add_and_destroy_test

# functional test util library
libgsequencer_test_la_SOURCES = ags/test/app/ags_functional_test_util.c ags/test/app/ags_functional_test_util.h ags/test/app/libgsequencer.h
libgsequencer_test_la_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
libgsequencer_test_la_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"

if FAST_TEST_MODE
libgsequencer_test_la_CPPFLAGS += -DAGS_FAST_FUNCTIONAL_TESTS=1
endif

libgsequencer_test_la_LDFLAGS = -pthread $(LDFLAGS)
libgsequencer_test_la_LIBADD = $(libgsequencer_test_LIBADD)

# functional server test
ags_functional_server_test_SOURCES = ags/test/server/ags_functional_server_test.c
ags_functional_server_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_functional_server_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_server_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# functional audio test
ags_functional_audio_test_SOURCES = ags/test/audio/ags_functional_audio_test.c
ags_functional_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_functional_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_audio_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional pitch test
ags_functional_pitch_test_SOURCES = ags/test/audio/ags_functional_pitch_test.c
ags_functional_pitch_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_functional_pitch_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_pitch_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional fast pitch test
ags_functional_fast_pitch_test_SOURCES = ags/test/audio/ags_functional_fast_pitch_test.c
ags_functional_fast_pitch_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_functional_fast_pitch_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_fast_pitch_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional fourier transform test
ags_functional_fourier_transform_test_SOURCES = ags/test/audio/ags_functional_fourier_transform_test.c
ags_functional_fourier_transform_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_functional_fourier_transform_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_fourier_transform_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional osc server test
ags_functional_osc_server_test_SOURCES = ags/test/audio/osc/ags_functional_osc_server_test.c
ags_functional_osc_server_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_functional_osc_server_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_osc_server_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional osc xmlrpc server test
ags_functional_osc_xmlrpc_server_test_SOURCES = ags/test/audio/osc/ags_functional_osc_xmlrpc_server_test.c
ags_functional_osc_xmlrpc_server_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_functional_osc_xmlrpc_server_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_osc_xmlrpc_server_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional machine add and destroy test
ags_functional_machine_add_and_destroy_test_SOURCES = ags/test/app/ags_functional_machine_add_and_destroy_test.c
ags_functional_machine_add_and_destroy_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_machine_add_and_destroy_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_machine_add_and_destroy_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_machine_add_and_destroy_test_LDADD = $(gsequencer_functional_test_LDADD)
