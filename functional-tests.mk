# Copyright (C) 2005-2023 Joel Kraehemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

libgsequencer_test_LIBADD = @LIBGSEQUENCER_TEST_LIBADD@
gsequencer_functional_test_LDADD = @GSEQUENCER_FUNCTIONAL_TEST_LDADD@

# libadd and ldadd
libgsequencer_test_LIBADD += libgsequencer.la libags_gui.la libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS) $(FONTCONFIG_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS)  $(WEBKITGTK_LIBS) $(POPPLER_LIBS) $(GTK_MAC_INTEGRATION_LIBS) $(W32API_LIBS)

gsequencer_functional_test_LDADD += libgsequencer.la libags_gui.la libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la libgsequencer_test.la -lcunit -lrt -lm -lXtst $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS) $(FONTCONFIG_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS)  $(WEBKITGTK_LIBS) $(POPPLER_LIBS) $(GTK_MAC_INTEGRATION_LIBS) $(W32API_LIBS)

check_LTLIBRARIES = libgsequencer_test.la

# functional tests
check_PROGRAMS += \
	ags_functional_server_test \
	ags_functional_audio_test \
	ags_functional_pitch_test \
	ags_functional_fast_pitch_test \
	ags_functional_fourier_transform_test \
	ags_functional_osc_server_test \
	ags_functional_osc_xmlrpc_server_test \
	ags_functional_machine_add_and_destroy_test \
	ags_functional_machine_link_test \
	ags_functional_line_member_add_and_destroy_test \
	ags_functional_notation_editor_workflow_test \
	ags_functional_automation_editor_workflow_test \
	ags_functional_notation_edit_test \
	ags_functional_panel_test \
	ags_functional_spectrometer_test \
	ags_functional_equalizer10_test \
	ags_functional_mixer_test \
	ags_functional_drum_test \
	ags_functional_matrix_test \
	ags_functional_synth_test \
	ags_functional_fm_synth_test \
	ags_functional_syncsynth_test \
	ags_functional_fm_syncsynth_test \
	ags_functional_hybrid_synth_test \
	ags_functional_hybrid_fm_synth_test \
	ags_functional_ffplayer_test \
	ags_functional_sf2_synth_test \
	ags_functional_pitch_sampler_test \
	ags_functional_sfz_synth_test \
	ags_functional_audiorec_test \
	ags_functional_audio_config_test \
	ags_functional_loop_000_xml_test \
	ags_functional_loop_001_xml_test \
	ags_functional_loop_002_xml_test \
	ags_functional_loop_003_xml_test \
	ags_functional_loop_004_xml_test \
	ags_functional_loop_005_xml_test \
	ags_functional_loop_006_xml_test \
	ags_functional_loop_007_xml_test \
	ags_functional_loop_008_xml_test \
	ags_functional_loop_010_xml_test

# functional test util library
libgsequencer_test_la_SOURCES = ags/test/app/ags_functional_test_util.c ags/test/app/ags_functional_test_util.h ags/test/app/libgsequencer.h
libgsequencer_test_la_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
libgsequencer_test_la_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"

if FAST_TEST_MODE
libgsequencer_test_la_CPPFLAGS += -DAGS_FAST_FUNCTIONAL_TESTS=1
endif

libgsequencer_test_la_LDFLAGS = -pthread $(LDFLAGS)
libgsequencer_test_la_LIBADD = $(libgsequencer_test_LIBADD)

# functional server test
ags_functional_server_test_SOURCES = ags/test/server/ags_functional_server_test.c
ags_functional_server_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_functional_server_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_server_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# functional audio test
ags_functional_audio_test_SOURCES = ags/test/audio/ags_functional_audio_test.c
ags_functional_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_functional_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_audio_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional pitch test
ags_functional_pitch_test_SOURCES = ags/test/audio/ags_functional_pitch_test.c
ags_functional_pitch_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_functional_pitch_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_pitch_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional fast pitch test
ags_functional_fast_pitch_test_SOURCES = ags/test/audio/ags_functional_fast_pitch_test.c
ags_functional_fast_pitch_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_functional_fast_pitch_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_fast_pitch_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional fourier transform test
ags_functional_fourier_transform_test_SOURCES = ags/test/audio/ags_functional_fourier_transform_test.c
ags_functional_fourier_transform_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_functional_fourier_transform_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_fourier_transform_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional osc server test
ags_functional_osc_server_test_SOURCES = ags/test/audio/osc/ags_functional_osc_server_test.c
ags_functional_osc_server_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_functional_osc_server_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_osc_server_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional osc xmlrpc server test
ags_functional_osc_xmlrpc_server_test_SOURCES = ags/test/audio/osc/ags_functional_osc_xmlrpc_server_test.c
ags_functional_osc_xmlrpc_server_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_functional_osc_xmlrpc_server_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_osc_xmlrpc_server_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional machine add and destroy test
ags_functional_machine_add_and_destroy_test_SOURCES = ags/test/app/ags_functional_machine_add_and_destroy_test.c
ags_functional_machine_add_and_destroy_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_machine_add_and_destroy_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_machine_add_and_destroy_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_machine_add_and_destroy_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional machine link test
ags_functional_machine_link_test_SOURCES = ags/test/app/ags_functional_machine_link_test.c
ags_functional_machine_link_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_machine_link_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_machine_link_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_machine_link_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional line member add and destroy test
ags_functional_line_member_add_and_destroy_test_SOURCES = ags/test/app/ags_functional_line_member_add_and_destroy_test.c
ags_functional_line_member_add_and_destroy_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_line_member_add_and_destroy_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_line_member_add_and_destroy_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_line_member_add_and_destroy_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional notation editor workflow test
ags_functional_notation_editor_workflow_test_SOURCES = ags/test/app/ags_functional_notation_editor_workflow_test.c
ags_functional_notation_editor_workflow_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_notation_editor_workflow_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_notation_editor_workflow_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_notation_editor_workflow_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional automation editor workflow test
ags_functional_automation_editor_workflow_test_SOURCES = ags/test/app/ags_functional_automation_editor_workflow_test.c
ags_functional_automation_editor_workflow_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_automation_editor_workflow_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_automation_editor_workflow_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_automation_editor_workflow_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional panel test
ags_functional_panel_test_SOURCES = ags/test/app/machine/ags_functional_panel_test.c
ags_functional_panel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_panel_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_panel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_panel_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional spectrometer test
ags_functional_spectrometer_test_SOURCES = ags/test/app/machine/ags_functional_spectrometer_test.c
ags_functional_spectrometer_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_spectrometer_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_spectrometer_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_spectrometer_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional equalizer10 test
ags_functional_equalizer10_test_SOURCES = ags/test/app/machine/ags_functional_equalizer10_test.c
ags_functional_equalizer10_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_equalizer10_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_equalizer10_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_equalizer10_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional mixer test
ags_functional_mixer_test_SOURCES = ags/test/app/machine/ags_functional_mixer_test.c
ags_functional_mixer_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_mixer_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_mixer_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_mixer_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional drum test
ags_functional_drum_test_SOURCES = ags/test/app/machine/ags_functional_drum_test.c
ags_functional_drum_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_drum_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_drum_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_drum_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional matrix test
ags_functional_matrix_test_SOURCES = ags/test/app/machine/ags_functional_matrix_test.c
ags_functional_matrix_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_matrix_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_matrix_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_matrix_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional synth test
ags_functional_synth_test_SOURCES = ags/test/app/machine/ags_functional_synth_test.c
ags_functional_synth_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_synth_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_synth_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_synth_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional FM synth test
ags_functional_fm_synth_test_SOURCES = ags/test/app/machine/ags_functional_fm_synth_test.c
ags_functional_fm_synth_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_fm_synth_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_fm_synth_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_fm_synth_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional syncsynth test
ags_functional_syncsynth_test_SOURCES = ags/test/app/machine/ags_functional_syncsynth_test.c
ags_functional_syncsynth_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_syncsynth_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_syncsynth_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_syncsynth_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional FM syncsynth test
ags_functional_fm_syncsynth_test_SOURCES = ags/test/app/machine/ags_functional_fm_syncsynth_test.c
ags_functional_fm_syncsynth_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_fm_syncsynth_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_fm_syncsynth_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_fm_syncsynth_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional hybrid synth test
ags_functional_hybrid_synth_test_SOURCES = ags/test/app/machine/ags_functional_hybrid_synth_test.c
ags_functional_hybrid_synth_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_hybrid_synth_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_hybrid_synth_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_hybrid_synth_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional hybrid FM synth test
ags_functional_hybrid_fm_synth_test_SOURCES = ags/test/app/machine/ags_functional_hybrid_fm_synth_test.c
ags_functional_hybrid_fm_synth_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_hybrid_fm_synth_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_hybrid_fm_synth_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_hybrid_fm_synth_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional ffplayer test
ags_functional_ffplayer_test_SOURCES = ags/test/app/machine/ags_functional_ffplayer_test.c
ags_functional_ffplayer_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_ffplayer_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_ffplayer_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_ffplayer_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional SF2 synth test
ags_functional_sf2_synth_test_SOURCES = ags/test/app/machine/ags_functional_sf2_synth_test.c
ags_functional_sf2_synth_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_sf2_synth_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_sf2_synth_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_sf2_synth_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional pitch sampler test
ags_functional_pitch_sampler_test_SOURCES = ags/test/app/machine/ags_functional_pitch_sampler_test.c
ags_functional_pitch_sampler_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_pitch_sampler_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_pitch_sampler_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_pitch_sampler_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional SFZ synth test
ags_functional_sfz_synth_test_SOURCES = ags/test/app/machine/ags_functional_sfz_synth_test.c
ags_functional_sfz_synth_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_sfz_synth_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_sfz_synth_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_sfz_synth_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional audiorec test
ags_functional_audiorec_test_SOURCES = ags/test/app/machine/ags_functional_audiorec_test.c
ags_functional_audiorec_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_audiorec_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_audiorec_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_audiorec_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional notation edit test
ags_functional_notation_edit_test_SOURCES = ags/test/app/ags_functional_notation_edit_test.c
ags_functional_notation_edit_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_notation_edit_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_notation_edit_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_notation_edit_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional audio config test
ags_functional_audio_config_test_SOURCES = ags/test/app/ags_functional_audio_config_test.c
ags_functional_audio_config_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_audio_config_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_audio_config_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_audio_config_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional loop 000 XML test
ags_functional_loop_000_xml_test_SOURCES = ags/test/app/ags_functional_loop_000_xml_test.c
ags_functional_loop_000_xml_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_loop_000_xml_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_loop_000_xml_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_loop_000_xml_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional loop 001 XML test
ags_functional_loop_001_xml_test_SOURCES = ags/test/app/ags_functional_loop_001_xml_test.c
ags_functional_loop_001_xml_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_loop_001_xml_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_loop_001_xml_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_loop_001_xml_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional loop 002 XML test
ags_functional_loop_002_xml_test_SOURCES = ags/test/app/ags_functional_loop_002_xml_test.c
ags_functional_loop_002_xml_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_loop_002_xml_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_loop_002_xml_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_loop_002_xml_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional loop 003 XML test
ags_functional_loop_003_xml_test_SOURCES = ags/test/app/ags_functional_loop_003_xml_test.c
ags_functional_loop_003_xml_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_loop_003_xml_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_loop_003_xml_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_loop_003_xml_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional loop 004 XML test
ags_functional_loop_004_xml_test_SOURCES = ags/test/app/ags_functional_loop_004_xml_test.c
ags_functional_loop_004_xml_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_loop_004_xml_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_loop_004_xml_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_loop_004_xml_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional loop 005 XML test
ags_functional_loop_005_xml_test_SOURCES = ags/test/app/ags_functional_loop_005_xml_test.c
ags_functional_loop_005_xml_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_loop_005_xml_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_loop_005_xml_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_loop_005_xml_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional loop 006 XML test
ags_functional_loop_006_xml_test_SOURCES = ags/test/app/ags_functional_loop_006_xml_test.c
ags_functional_loop_006_xml_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_loop_006_xml_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_loop_006_xml_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_loop_006_xml_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional loop 007 XML test
ags_functional_loop_007_xml_test_SOURCES = ags/test/app/ags_functional_loop_007_xml_test.c
ags_functional_loop_007_xml_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_loop_007_xml_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_loop_007_xml_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_loop_007_xml_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional loop 008 XML test
ags_functional_loop_008_xml_test_SOURCES = ags/test/app/ags_functional_loop_008_xml_test.c
ags_functional_loop_008_xml_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_loop_008_xml_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_loop_008_xml_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_loop_008_xml_test_LDADD = $(gsequencer_functional_test_LDADD)

# functional loop 010 XML test
ags_functional_loop_010_xml_test_SOURCES = ags/test/app/ags_functional_loop_010_xml_test.c
ags_functional_loop_010_xml_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
ags_functional_loop_010_xml_test_CPPFLAGS = -DAGS_SRC_DIR=\"$(srcdir)\" -DAGS_DATA_DIR=\"gsequencer.share\"
ags_functional_loop_010_xml_test_LDFLAGS = -pthread $(LDFLAGS)
ags_functional_loop_010_xml_test_LDADD = $(gsequencer_functional_test_LDADD)
