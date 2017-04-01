libgsequencer_test_LIBADD = @LIBGSEQUENCER_TEST_LIBADD@
gsequencer_functional_test_LDADD = @GSEQUENCER_FUNCTIONAL_TEST_LDADD@

# libadd and ldadd
libgsequencer_test_LIBADD += libgsequencer.la libags_audio.la libags_server.la libags_gui.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS) $(FONTCONFIG_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS)

gsequencer_functional_test_LDADD += libgsequencer.la libags_audio.la libags_server.la libags_gui.la libags_thread.la libags.la libgsequencer_test.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS) $(FONTCONFIG_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS)

check_LTLIBRARIES = libgsequencer_test.la

# functional tests
check_PROGRAMS += \
	ags_functional_audio_test \
	ags_functional_machine_add_and_destroy_test \
	ags_functional_panel_test \
	ags_functional_mixer_test \
	ags_functional_drum_test \
	ags_functional_matrix_test \
	ags_functional_synth_test \
	ags_functional_ffplayer_test \
	ags_functional_note_edit_test

# functional test util library
libgsequencer_test_la_SOURCES = ags/test/X/gsequencer_setup_util.c ags/test/X/gsequencer_setup_util.h ags/test/X/ags_functional_test_util.c ags/test/X/ags_functional_test_util.h ags/test/X/libgsequencer.h
libgsequencer_test_la_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
libgsequencer_test_la_CPPFLAGS = -DSRCDIR=\"$(top_srcdir)\"
libgsequencer_test_la_LIBADD = $(libgsequencer_test_LIBADD)

# functional audio test
ags_functional_audio_test_SOURCES = ags/test/audio/ags_functional_audio_test.c
ags_functional_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_functional_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_audio_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional machine add and destroy test
ags_functional_machine_add_and_destroy_test_SOURCES = ags/test/X/ags_functional_machine_add_and_destroy_test.c
ags_functional_machine_add_and_destroy_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
ags_functional_machine_add_and_destroy_test_CPPFLAGS = -DSRCDIR=\"$(top_srcdir)\"
ags_functional_machine_add_and_destroy_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_machine_add_and_destroy_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional panel test
ags_functional_panel_test_SOURCES = ags/test/X/machine/ags_functional_panel_test.c
ags_functional_panel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
ags_functional_panel_test_CPPFLAGS = -DSRCDIR=\"$(top_srcdir)\"
ags_functional_panel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_panel_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional mixer test
ags_functional_mixer_test_SOURCES = ags/test/X/machine/ags_functional_mixer_test.c
ags_functional_mixer_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
ags_functional_mixer_test_CPPFLAGS = -DSRCDIR=\"$(top_srcdir)\"
ags_functional_mixer_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_mixer_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional drum test
ags_functional_drum_test_SOURCES = ags/test/X/machine/ags_functional_drum_test.c
ags_functional_drum_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
ags_functional_drum_test_CPPFLAGS = -DSRCDIR=\"$(top_srcdir)\"
ags_functional_drum_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_drum_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional matrix test
ags_functional_matrix_test_SOURCES = ags/test/X/machine/ags_functional_matrix_test.c
ags_functional_matrix_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
ags_functional_matrix_test_CPPFLAGS = -DSRCDIR=\"$(top_srcdir)\"
ags_functional_matrix_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_matrix_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional synth test
ags_functional_synth_test_SOURCES = ags/test/X/machine/ags_functional_synth_test.c
ags_functional_synth_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
ags_functional_synth_test_CPPFLAGS = -DSRCDIR=\"$(top_srcdir)\"
ags_functional_synth_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_synth_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional ffplayer test
ags_functional_ffplayer_test_SOURCES = ags/test/X/machine/ags_functional_ffplayer_test.c
ags_functional_ffplayer_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
ags_functional_ffplayer_test_CPPFLAGS = -DSRCDIR=\"$(top_srcdir)\"
ags_functional_ffplayer_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_ffplayer_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional note edit test
ags_functional_note_edit_test_SOURCES = ags/test/X/ags_functional_note_edit_test.c
ags_functional_note_edit_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
ags_functional_note_edit_test_CPPFLAGS = -DSRCDIR=\"$(top_srcdir)\"
ags_functional_note_edit_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_note_edit_test_LDADD = $(gsequencer_functional_test_LDADD)
