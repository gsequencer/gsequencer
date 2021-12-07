#Copyright (C) 2005-2021 Joel Kraehemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

AUTOMAKE_OPTIONS = foreign

SUBDIRS = m4 po lib

AM_DISTCHECK_CONFIGURE_FLAGS = --enable-introspection --enable-vst3

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

# what flags you want to pass to the C compiler & linker
# AM_CPPFLAGS += -march=native -O3 -ftree-vectorize -ftree-slp-vectorize -ffast-math -ftree-vectorizer-verbose=2
CPPFLAGS =  @CPPFLAGS@ -include errno.h -DSRCDIR=\"$(srcdir)\" -DDESTDIR=\"$(datadir)\" -DDOCDIR=\"$(docdir)\" -DPACKAGE_VERSION=\"$(PACKAGE_VERSION)\" -DAGS_REDUCE_RT_EVENTS=1

if WITH_W32API
else
if WITH_OSXAPI
else
CPPFLAGS += -D_FORTIFY_SOURCE=2 -Wall -Wformat -Werror=format-security
endif
endif

if WITH_LIBINSTPATCH
CPPFLAGS += -DAGS_WITH_LIBINSTPATCH=1
endif

CFLAGS =  @CFLAGS@
CXXFLAGS =  @CXXFLAGS@

LDFLAGS = @LDFLAGS@ $(LTLIBINTL)
LIBTOOL_DEPS = @LIBTOOL_DEPS@

if WITH_W32API
PIC_FLAGS = -shared -DDLL_EXPORT -DPIC
else
PIC_FLAGS = -shared -fPIC
endif

gsequencer_CFLAGS = @GSEQUENCER_CFLAGS@
gsequencer_LDFLAGS = @GSEQUENCER_LDFLAGS@

midi2xml_CFLAGS = @GSEQUENCER_CFLAGS@
midi2xml_LDFLAGS = @GSEQUENCER_LDFLAGS@

if WITH_W32API
LDFLAGS += -no-undefined -L$(top_builddir)/lib -lgnu
endif

# 
# TARGETS = gsequencer

localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
lib_LTLIBRARIES = libags.la libags_thread.la libags_server.la

if WITH_VST3
lib_LTLIBRARIES += libags_vst.la 
endif

lib_LTLIBRARIES += libags_audio.la libags_gui.la

# lib_LTLIBRARIES += libgsequencer.la

bin_PROGRAMS = gsequencer midi2xml

HTMLHELP_XSL = @HTMLHELP_XSL@

if AGS_WITH_PUBLIC_LIBGSEQUENCER
libgsequencerdir = $(libdir)
else
libgsequencerdir = $(libdir)/gsequencer
endif

GOBJECT_API_DOC = @GOBJECT_API_DOC@
GTK_API_DOC = @GTK_API_DOC@

LIBAGS_API_DOC = @LIBAGS_API_DOC@
LIBAGS_AUDIO_API_DOC = @LIBAGS_AUDIO_API_DOC@
LIBAGS_GUI_API_DOC = @LIBAGS_GUI_API_DOC@
LIBGSEQUENCER_API_DOC = @LIBGSEQUENCER_API_DOC@

LIBAGS_API_DIR = @LIBAGS_API_DIR@
LIBAGS_AUDIO_API_DIR = @LIBAGS_AUDIO_API_DIR@
LIBAGS_GUI_API_DIR = @LIBAGS_GUI_API_DIR@
LIBGSEQUENCER_API_DIR = @LIBGSEQUENCER_API_DIR@

libgsequencer_LTLIBRARIES = libgsequencer.la

EXTRA_DIST = config.rpath \
	COPYING.server \
	COPYING.docs \
	COPYING.stk-4.3 \
	ags/config.h.in \
	ags/ags_api_config.h.in \
	gtk-doc.make \
	gsequencer.share \
	aclocal.m4 \
	gsequencer.1 \
	README \
	ags_example.xml \
	midi2xml.1 \
	ags_check_system_unit_programs \
	ags_check_system_valgrind_unit_programs \
	ags_check_system_functional_programs \
	ags_check_system_valgrind_functional_programs \
	ags_authentication_test.xml \
	ags_business_group_test.xml \
	ags_certificate_test.xml \
	ags_password_store_test.xml \
	ags_functional_server_test_authentication.xml \
	ags_functional_server_test_password_store.xml \
	ags_functional_osc_xmlrpc_server_test_authentication.xml \
	ags_functional_osc_xmlrpc_server_test_password_store.xml \
	ags_functional_notation_edit_test.xml \
	ags_functional_audio_config_test.xml \
	pot_exclude.txt \
	m4/Makefile.am \
	m4/Makefile.in \
	patches/libags_audio_xml-with-vst.patch \
	po/POTFILES.in \
	po/POTFILES.in.in \
	po/gsequencer.pot-header \
	lib/Makefile.am \
	ags/i18n.h \
	ags/gettext.h \
	ags/lib/AUTHORS \
	ags/server/security/AUTHORS \
	ags/audio/AUTHORS \
	ags/audio/recall/AUTHORS \
	ags/test/X/examples/ags-loop-000.xml \
	ags/test/X/examples/ags-loop-001.xml \
	ags/test/X/examples/ags-loop-002.xml \
	ags/test/X/examples/ags-loop-003.xml \
	ags/test/X/examples/ags-loop-004.xml \
	ags/test/X/examples/ags-loop-005.xml \
	ags/test/X/examples/ags-loop-006.xml \
	ags/test/X/examples/ags-loop-007.xml \
	ags/test/X/examples/ags-loop-008.xml \
	ags/test/X/examples/ags-loop-010.xml \
	docs/images/README \
	gsequencer.share/icons/README \
	gsequencer.share/images/README

EXTRA_DIST += \
	license-notice-gnu-agpl-3-0+-c.txt \
	license-notice-gnu-fdl-1-3-xml.txt \
	license-notice-gnu-gpl-3-0+-c.txt \
	license-notice-gnu-gpl-3-0+-sym.txt \
	license-notice-gnu-agpl-3-0+-sym.txt \
	license-notice-permissive-xml.txt

EXTRA_DIST += \
	libags.sym.in \
	libags_thread.sym.in \
	libags_server.sym.in \
	libags_audio.sym.in \
	libags_vst.sym.in \
	libags_gui.sym.in

EXTRA_DIST += \
	unit-system-tests.mk.am \
	unit-system-tests.mk.in \
	functional-system-tests.mk.am \
	functional-system-tests.mk.in

EXTRA_DIST += \
	docs/reference/libags/version.xml.in \
	docs/reference/libags/libags-sections.txt.in \
	docs/reference/libags/libags.types.in \
	docs/reference/libags/libags.interfaces.in \
	docs/reference/libags-audio/version.xml.in \
	docs/reference/libags-audio/libags_audio-sections.txt.in \
	docs/reference/libags-audio/libags_audio.types.in \
	docs/reference/libags-audio/libags_audio.interfaces.in \
	docs/reference/libags-gui/version.xml.in \
	docs/reference/libags-gui/libags_gui-sections.txt.in \
	docs/reference/libags-gui/libags_gui.types.in \
	docs/reference/libags-gui/libags_gui.interfaces.in \
	docs/reference/libgsequencer/version.xml.in \
	docs/reference/libgsequencer/libgsequencer-sections.txt.in \
	docs/reference/libgsequencer/libgsequencer.types.in \
	docs/reference/libgsequencer/libgsequencer.interfaces.in \
	clean-gtk-doc.sh

EXTRA_DIST += \
	docs/reference/libags-gui/meson.build \
	docs/reference/libags-audio/meson.build \
	docs/reference/libgsequencer/meson.build \
	docs/reference/libags/meson.build \
	docs/reference/meson.build \
	ags/test/X/meson.build \
	ags/test/audio/meson.build \
	ags/test/plugin/meson.build \
	ags/test/thread/meson.build \
	ags/test/lib/meson.build \
	ags/test/object/meson.build \
	ags/test/meson.build \
	ags/test/server/meson.build \
	ags/test/file/meson.build \
	po/meson.build \
	ags/meson.build \
	listing_script.py \
	meson.build \
	meson_options.txt

EXTRA_DIST += \
	lv2/lv2plug.in/ns/lv2ext/lv2_programs.h

#EXTRA_DIST += \
#	lv2.lib/mda/EPiano-presets.ttl

EXTRA_DIST += \
	apple_script.sh \
	prepare-distribution.sh

man1_MANS = gsequencer.1 midi2xml.1

CLEANFILES = \
	functional-system-tests.mk \
	m4/Makefile \
	lib/Makefile \
	libags.sym \
	libags_thread.sym \
	libags_server.sym \
	libags_audio.sym \
	libags_vst.sym \
	libags_gui.sym \
	ags/object/ags_marshallers.list \
	ags/widget/ags_widget_marshallers.list \
	docs/listings/add_pattern.c \
	docs/listings/application_mutex.c \
	docs/listings/audio.c \
	docs/listings/audio_application_context.c \
	docs/listings/complete_example.c \
	docs/listings/config.c \
	docs/listings/effects_echo_audio.c \
	docs/listings/effects_echo_audio_run.c \
	docs/listings/effects_echo_channel.c \
	docs/listings/effects_echo_channel_run.c \
	docs/listings/effects_recall_container.c \
	docs/listings/file_property.c \
	docs/listings/file_read.c \
	docs/listings/file_write.c \
	docs/listings/find_port.c \
	docs/listings/fx_factory.c \
	docs/listings/linking_prerequisites.c \
	docs/listings/linking_safe.c \
	docs/listings/linking_unsafe.c \
	docs/listings/notation_clipboard.c \
	docs/listings/pcm_info.c \
	docs/listings/port.c \
	docs/listings/pull_thread.c \
	docs/listings/recycling.c \
	docs/listings/sequencer_buffer.c \
	docs/listings/start_thread.c \
	docs/listings/soundcard_buffer.c \
	docs/listings/thread_application_context.c \
	docs/listings/thread_obj_mutex.c \
	docs/listings/wave_concat.c \
	docs/reference/libags/libags-sections.txt \
	docs/reference/libags/libags.types \
	docs/reference/libags/libags.interfaces \
	docs/reference/libags-audio/libags_audio-sections.txt \
	docs/reference/libags-audio/libags_audio.types \
	docs/reference/libags-audio/libags_audio.interfaces \
	docs/reference/libags-gui/libags_gui-sections.txt \
	docs/reference/libags-gui/libags_gui.types \
	docs/reference/libags-gui/libags_gui.interfaces \
	docs/reference/libgsequencer/libgsequencer-sections.txt \
	docs/reference/libgsequencer/libgsequencer.types \
	docs/reference/libgsequencer/libgsequencer.interfaces

# gir - gobject introspection
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(top_srcdir) --add-include-path=$(top_builddir) -I$(top_srcdir) -I$(top_builddir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(top_srcdir) --includedir=$(top_builddir)

if HAVE_INTROSPECTION
introspection_sources = $(libags_la_SOURCES) $(libags_thread_la_SOURCES) $(libags_server_la_SOURCES) $(libags_audio_la_SOURCES) $(libags_gui_la_SOURCES)

Ags-3.0.gir: libags.la libags_thread.la libags_server.la
Ags_3_0_gir_INCLUDES = libxml2-2.0 GObject-2.0 Soup-2.4
Ags_3_0_gir_C_INCLUDES = ags/libags.h
Ags_3_0_gir_CFLAGS = $(INCLUDES) $(UUID_CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(GIO_CFLAGS) $(LIBSOUP_CFLAGS)
Ags_3_0_gir_LIBS = libags.la libags_thread.la libags_server.la
Ags_3_0_gir_FILES = $(libags_la_HEADERS_0) $(libags_la_SOURCES) $(libags_thread_la_HEADERS_0) $(libags_thread_la_SOURCES) $(libags_server_la_HEADERS_0) $(libags_server_la_SOURCES)
Ags_3_0_gir_NAMESPACE = Ags
Ags_3_0_gir_VERSION = 3.0
Ags_3_0_gir_SCANNERFLAGS = --add-include-path=$(top_srcdir) --add-include-path=$(top_builddir) -I$(top_srcdir) -I$(top_builddir) --warn-all --include=Soup-2.4 --include=GObject-2.0 --include=libxml2-2.0 --namespace=Ags --nsversion=3.0 --identifier-prefix=Ags --symbol-prefix=ags_ --output=$(top_builddir)/Ags-3.0.gir
INTROSPECTION_GIRS += Ags-3.0.gir

AgsAudio-3.0.gir: libags_audio.la $(top_builddir)/Ags-3.0.gir
AgsAudio_3_0_gir_INCLUDES = libxml2-2.0 GObject-2.0 Soup-2.4 Gst-1.0 GstBase-1.0 GstAudio-1.0 Ags-3.0
AgsAudio_3_0_gir_C_INCLUDES = ags/libags-audio.h
AgsAudio_3_0_gir_CFLAGS = $(INCLUDES) $(LIBASOUND2_CFLAGS) $(LIBDSSIALSACOMPAT_CFLAGS) $(LIBXML2_CFLAGS) $(SAMPLERATE_CFLAGS) $(SNDFILE_CFLAGS) $(FFTW_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(PULSE_CFLAGS) $(GSTREAMER_CFLAGS)
AgsAudio_3_0_gir_LIBS = libags_audio.la
AgsAudio_3_0_gir_FILES = $(libags_audio_la_HEADERS_0) $(libags_audio_la_SOURCES)

AgsAudio_3_0_gir_NAMESPACE = AgsAudio
AgsAudio_3_0_gir_VERSION = 3.0
AgsAudio_3_0_gir_SCANNERFLAGS = --add-include-path=$(top_srcdir) --add-include-path=$(top_builddir) -I$(top_srcdir) -I$(top_builddir) --warn-all --include=Gst-1.0 --include=GstBase-1.0 --include=GstAudio-1.0 --include=Soup-2.4 --include=GObject-2.0 --include=libxml2-2.0 --include=Ags-3.0 --namespace=AgsAudio --nsversion=3.0 --identifier-prefix=Ags --symbol-prefix=ags_ --output=$(top_builddir)/AgsAudio-3.0.gir
INTROSPECTION_GIRS += AgsAudio-3.0.gir

AgsGui-3.0.gir: libags_gui.la
AgsGui_3_0_gir_INCLUDES = GObject-2.0 Gtk-3.0
AgsGui_3_0_gir_C_INCLUDES = ags/libags-gui.h
AgsGui_3_0_gir_CFLAGS = $(INCLUDES) $(GTK_CFLAGS)
AgsGui_3_0_gir_LIBS = libags_gui.la
AgsGui_3_0_gir_FILES = $(libags_gui_la_HEADERS_0) $(libags_gui_la_SOURCES)
AgsGui_3_0_gir_NAMESPACE = AgsGui
AgsGui_3_0_gir_VERSION = 3.0
AgsGui_3_0_gir_SCANNERFLAGS = --add-include-path=$(top_srcdir) --add-include-path=$(top_builddir) -I$(top_srcdir) -I$(top_builddir) --warn-all --include=Gtk-3.0 --namespace=AgsGui --nsversion=3.0 --identifier-prefix=Ags --symbol-prefix=ags_ --output=$(top_builddir)/AgsGui-3.0.gir
INTROSPECTION_GIRS += AgsGui-3.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

# pkg-config
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libags.pc libags_audio.pc libags_gui.pc

if AGS_WITH_PUBLIC_LIBGSEQUENCER
pkgconfig_DATA += libgsequencer.pc
endif

EXTRA_DIST += libags.pc.in libags_audio.pc.in libags_gui.pc.in libgsequencer.pc.in

# desktop file
appdatadir = $(datadir)/applications
appdata_DATA = gsequencer.desktop

EXTRA_DIST += gsequencer.desktop.in

# MIME specification
EXTRA_DIST += application-x-gsequencer.xml \
	org.nongnu.gsequencer.gsequencer.appdata.xml

# clean
CLEANFILES += $(appdata_DATA) $(pkgconfig_DATA)

# extra
EXTRA_DIST += \
	libags.sym \
	libags_thread.sym \
	libags_server.sym \
	libags_audio.sym \
	libags_vst.sym \
	libags_gui.sym

EXTRA_DIST += \
	docs/reference/libags/git.mk \
	docs/reference/libags/gtk-doc.make \
	docs/reference/libags/Makefile.am \
	docs/reference/libags/Makefile.in

EXTRA_DIST += \
	docs/reference/libags/libags-overrides.txt \
	docs/reference/libags/libags-sections.txt \
	docs/reference/libags/libags.types \
	docs/reference/libags/libags.xml

EXTRA_DIST += \
	docs/reference/libags-audio/git.mk \
	docs/reference/libags-audio/gtk-doc.make \
	docs/reference/libags-audio/Makefile.am \
	docs/reference/libags-audio/Makefile.in

EXTRA_DIST += \
	docs/reference/libags-audio/libags_audio-overrides.txt \
	docs/reference/libags-audio/libags_audio-sections.txt \
	docs/reference/libags-audio/libags_audio.types \
	docs/reference/libags-audio/libags_audio.xml

EXTRA_DIST += \
	docs/reference/libags-gui/git.mk \
	docs/reference/libags-gui/gtk-doc.make \
	docs/reference/libags-gui/Makefile.am \
	docs/reference/libags-gui/Makefile.in

EXTRA_DIST += \
	docs/reference/libags-gui/libags_gui-overrides.txt \
	docs/reference/libags-gui/libags_gui-sections.txt \
	docs/reference/libags-gui/libags_gui.types \
	docs/reference/libags-gui/libags_gui.xml

EXTRA_DIST += docs/reference/libgsequencer/git.mk \
	docs/reference/libgsequencer/gtk-doc.make \
	docs/reference/libgsequencer/Makefile.am \
	docs/reference/libgsequencer/Makefile.in

EXTRA_DIST += \
	docs/reference/libgsequencer/libgsequencer-overrides.txt \
	docs/reference/libgsequencer/libgsequencer.xml

# EXTRA_DIST += docs/reference/libgsequencer

# include
otherincludedir = $(includedir)/ags
nobase_include_HEADERS = $(libags_la_HEADERS_0) $(libags_thread_la_HEADERS_0) $(libags_server_la_HEADERS_0) $(libags_audio_la_HEADERS_0) $(libags_vst_la_HEADERS_0) $(libags_gui_la_HEADERS_0)
# nobase_include_HEADERS += $(libgsequencer_la_HEADERS_0)

# doc
# docdir = $(datadir)/doc/gsequencer
doc_DATA = README

# documentation
manpages = \
	gsequencer.1.xml \
	midi2xml.1.xml

EXTRA_DIST += \
	docs/custom.css

userdoc = \
	docs/usersBook.xml \
	docs/appa.xml \
	docs/usersBook/appb.xml \
	docs/usersBook/chap1.xml \
	docs/usersBook/chap2.xml \
	docs/usersBook/chap3.xml \
	docs/usersBook/chap4.xml \
	docs/usersBook/chap5.xml \
	docs/usersBook/chap6.xml \
	docs/usersBook/preface.xml

devdoc = \
	docs/developersBook.xml \
	docs/developersBook/preface.xml \
	docs/developersBook/chap1.xml \
	docs/developersBook/chap2.xml \
	docs/developersBook/chap3.xml \
	docs/developersBook/chap4.xml \
	docs/developersBook/chap5.xml \
	docs/developersBook/chap6.xml \
	docs/developersBook/chap7.xml \
	docs/developersBook/chap8.xml \
	docs/developersBook/chap9.xml \
	docs/developersBook/chap10.xml \
	docs/developersBook/chap11.xml \
	docs/developersBook/chap12.xml \
	docs/developersBook/appb.xml

oscdoc = \
	docs/oscBook.xml \
	docs/oscBook/appb.xml \
	docs/oscBook/chap1.xml \
	docs/oscBook/chap2.xml \
	docs/oscBook/chap3.xml \
	docs/oscBook/chap4.xml \
	docs/oscBook/chap5.xml \
	docs/oscBook/chap6.xml \
	docs/oscBook/chap7.xml \
	docs/oscBook/chap8.xml \
	docs/oscBook/appb.xml

devdoclistings = \
	docs/listings/add_pattern.c.in \
	docs/listings/application_mutex.c.in \
	docs/listings/audio_application_context.c.in \
	docs/listings/audio.c.in \
	docs/listings/complete_example.c.in \
	docs/listings/config.c.in \
	docs/listings/effects_echo_audio.c.in \
	docs/listings/effects_echo_audio_run.c.in \
	docs/listings/effects_echo_channel.c.in \
	docs/listings/effects_echo_channel_run.c.in \
	docs/listings/effects_recall_container.c.in \
	docs/listings/file_property.c.in \
	docs/listings/file_read.c.in \
	docs/listings/file_write.c.in \
	docs/listings/find_port.c.in \
	docs/listings/fx_factory.c.in \
	docs/listings/linking_prerequisites.c.in \
	docs/listings/linking_safe.c.in \
	docs/listings/linking_unsafe.c.in \
	docs/listings/notation_clipboard.c.in \
	docs/listings/pcm_info.c.in \
	docs/listings/pull_thread.c.in \
	docs/listings/port.c.in \
	docs/listings/recycling.c.in \
	docs/listings/soundcard_buffer.c.in \
	docs/listings/start_thread.c.in \
	docs/listings/sequencer_buffer.c.in \
	docs/listings/thread_obj_mutex.c.in \
	docs/listings/thread_application_context.c.in \
	docs/listings/wave_concat.c.in

devdocimages = \
	docs/images/AGS_iterator.png \
	docs/images/AGS_recall_lifecycle.png \
	docs/images/AGS_super-threaded.png \
	docs/images/AGS_linking.png \
	docs/images/ags-threading.png

userdocimages = \
	docs/images/ags_audiorec.png \
	docs/images/ags-automation-toolbar.png \
	docs/images/ags_automation_window_drum.png \
	docs/images/ags_equalizer10.png \
	docs/images/ags_export_window.png \
	docs/images/ags_machine_properties-input.png \
	docs/images/ags_machine_properties-resize_channels.png \
	docs/images/ags_navigation.png \
	docs/images/ags_audio_preferences.png \
	docs/images/ags_audio_connection_output_bulk.png \
	docs/images/ags_audio_connection_output_line.png \
	docs/images/ags_audio_connection_input_bulk.png \
	docs/images/ags_audio_connection_input_line.png \
	docs/images/ags_midi_dialog.png \
	docs/images/ags_midi_export_wizard_no1.png \
	docs/images/ags_midi_export_wizard_no2.png \
	docs/images/ags_midi_import_wizard_no1.png \
	docs/images/ags_midi_import_wizard_no2.png \
	docs/images/ags_midi_preferences.png \
	docs/images/ags_ffplayer.png \
	docs/images/ags_fm_synth.png \
	docs/images/ags_fm_syncsynth.png \
	docs/images/ags_ladspa_browser.png \
	docs/images/ags_machine_properties-link_input.png \
	docs/images/ags_matrix.png \
	docs/images/ags_panel.png \
	docs/images/ags_spectrometer.png \
	docs/images/ags_synth.png \
	docs/images/ags_syncsynth.png \
	docs/images/ags-toolbar.png \
	docs/images/ags_drum.png \
	docs/images/ags_generic_preferences.png \
	docs/images/ags_machine_properties-output.png \
	docs/images/ags_mixer.png \
	docs/images/ags_pitch_sampler.png \
	docs/images/ags_performance_preferences.png \
	docs/images/ags_server_preferences.png \
	docs/images/ags_machine_envelope_editor.png \
	docs/images/ags_machine_envelope_info.png \
	docs/images/ags_machine_envelope_pattern.png \
	docs/images/ags_move_notes_dialog.png \
	docs/images/ags_crop_notes_dialog.png \
	docs/images/ags_position_notation_cursor_dialog.png \
	docs/images/ags_position_automation_cursor_dialog.png \
	docs/images/ags_position_wave_cursor_dialog.png \
	docs/images/ags_select_notes_dialog.png \
	docs/images/ags_select_accelerations_dialog.png \
	docs/images/ags_select_buffers_dialog.png \
	docs/images/ags_ramp_accelerations_dialog.png \
	docs/images/ags-wave-toolbar.png \
	docs/images/ags_wave_window_audiorec.png

userdocadwaitaimages = \
	docs/images/adwaita/accessories-text-editor.png \
	docs/images/adwaita/edit-clear-all.png \
	docs/images/adwaita/edit-copy.png \
	docs/images/adwaita/edit-cut.png \
	docs/images/adwaita/edit-paste.png \
	docs/images/adwaita/edit-select-all.png \
	docs/images/adwaita/go-jump.png \
	docs/images/adwaita/media-playback-start.png \
	docs/images/adwaita/media-playback-stop.png \
	docs/images/adwaita/media-seek-backward.png \
	docs/images/adwaita/media-seek-forward.png \
	docs/images/adwaita/media-skip-backward.png \
	docs/images/adwaita/media-skip-forward.png \
	docs/images/adwaita/object-flip-vertical.png \
	docs/images/adwaita/system-run.png

EXTRA_DIST += $(manpages) $(userdoc) $(userdocimages) $(userdocadwaitaimages) $(devdoc) $(devdocimages) $(devdoclistings) $(oscdoc)

EXTRA_DIST += \
	ags/server/security/ags_authentication.dtd \
	ags/server/security/ags_business_group.dtd \
	ags/server/security/ags_certificate.dtd \
	ags/server/security/ags_password_store.dtd \
	ags/audio/midi/ags_midi_file.dtd \
	ags/audio/osc/ags_osc_file.dtd \
	ags/file/ags_file.dtd \
	ags/X/file/ags_simple_file.dtd \
	ags.xsl \
	ags-simple.xsl \
	ags/object/ags_marshallers.list.in \
	ags/widget/ags_widget_marshallers.list.in

# conditional objects


# libraries and gsequencer
libags_la_SOURCES = $(libags_c_sources) $(libags_util_c_sources) $(libags_object_c_sources) $(libags_file_c_sources) $(libags_file_task_c_sources)
libags_la_HEADERS_0 = ags/libags.h $(libags_h_sources) $(libags_util_h_sources) $(libags_object_h_sources) $(libags_file_h_sources) $(libags_file_task_h_sources)

libags_thread_la_SOURCES = $(libags_thread_c_sources) $(libags_thread_task_c_sources)
libags_thread_la_HEADERS_0 = $(libags_thread_h_sources) $(libags_thread_task_h_sources)
libags_server_la_SOURCES = $(libags_server_c_sources) $(libags_server_security_c_sources) $(libags_server_controller_c_sources)
libags_server_la_HEADERS_0 = $(libags_server_h_sources) $(libags_server_security_h_sources) $(libags_server_controller_h_sources)

libags_audio_la_SOURCES = $(libags_audio_c_sources) $(libags_audio_thread_c_sources) $(libags_audio_file_c_sources) $(libags_audio_midi_c_sources) $(libags_audio_osc_c_sources) $(libags_audio_osc_controller_c_sources) $(libags_audio_osc_xmlrpc_c_sources) $(libags_audio_gstreamer_c_sources) $(libags_audio_jack_c_sources) $(libags_audio_pulse_c_sources) $(libags_audio_core_audio_c_sources) $(libags_audio_audio_unit_c_sources) $(libags_audio_wasapi_c_sources) $(libags_audio_task_c_sources) $(libags_audio_task_recall_c_sources) $(libags_audio_recall_c_sources) $(libags_audio_fx_c_sources) $(libags_plugin_c_sources) 
libags_audio_la_HEADERS_0 = ags/libags-audio.h $(libags_audio_h_sources) $(libags_audio_thread_h_sources) $(libags_audio_file_h_sources) $(libags_audio_midi_h_sources) $(libags_audio_osc_h_sources) $(libags_audio_osc_controller_h_sources) $(libags_audio_osc_xmlrpc_h_sources) $(libags_audio_gstreamer_h_sources) $(libags_audio_jack_h_sources) $(libags_audio_pulse_h_sources) $(libags_audio_core_audio_h_sources) $(libags_audio_audio_unit_h_sources) $(libags_audio_wasapi_h_sources) $(libags_audio_task_h_sources) $(libags_audio_task_recall_h_sources) $(libags_audio_recall_h_sources) $(libags_audio_fx_h_sources)$(libags_plugin_h_sources)

libags_gui_la_SOURCES = $(libags_gui_c_sources)
libags_gui_la_HEADERS_0 = ags/libags-gui.h $(libags_gui_h_sources)

libgsequencer_la_HEADERS_0 = $(libgsequencer_h_sources) $(libgsequencer_osc_controller_h_sources) $(libgsequencer_file_h_sources) $(libgsequencer_task_h_sources) $(libgsequencer_import_h_sources) $(libgsequencer_export_h_sources) $(libgsequencer_machine_h_sources) $(libgsequencer_editor_h_sources)
libgsequencer_la_SOURCES = $(libgsequencer_la_HEADERS_0) $(libgsequencer_c_sources) $(libgsequencer_osc_controller_c_sources) $(libgsequencer_file_c_sources) $(libgsequencer_task_c_sources) $(libgsequencer_import_c_sources) $(libgsequencer_export_c_sources) $(libgsequencer_machine_c_sources) $(libgsequencer_editor_c_sources)

libags_vst_la_SOURCES = 
libags_vst_la_HEADERS_0 =

if WITH_VST3
libags_vst_la_SOURCES += $(libags_vst_c_sources)
libags_vst_la_HEADERS_0 += ags/libags-vst.h $(libags_vst_h_sources)
endif

# library libags
libags_la_CPPFLAGS = -std=gnu99 $(CPPFLAGS) $(LIBAGS_CPPFLAGS)
libags_la_CFLAGS = $(CFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ $(UUID_CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(GIO_CFLAGS) $(W32API_CFLAGS)
libags_la_LDFLAGS = $(LDFLAGS) -version-info 3:0:0 $(PIC_FLAGS) -pthread
libags_la_LIBADD = -lm $(UUID_LIBS) $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(GIO_LIBS) $(W32API_LIBS) 

if WITH_W32API
libags_la_LDFLAGS += -Wl,--export-all-symbols,--out-implib=libags.dll.a
else
if WITH_OSXAPI
libags_la_LDFLAGS += -Wl,-headerpad_max_install_names -export-symbols libags.sym -Wl,-install_name,libags.dylib
else
libags_la_LDFLAGS += -export-symbols libags.sym -Wl,-soname,libags.so
libags_la_LIBADD += -ldl -lcrypt
endif
endif

# library libags-thread
libags_thread_la_CPPFLAGS = -std=gnu99 $(CPPFLAGS) $(LIBAGS_THREAD_CPPFLAGS)
libags_thread_la_CFLAGS = $(CFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(W32API_CFLAGS)
libags_thread_la_LDFLAGS = $(LDFLAGS) -version-info 3:0:0 $(PIC_FLAGS) -pthread
libags_thread_la_LIBADD = libags.la -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(W32API_LIBS)

if WITH_W32API
libags_thread_la_LDFLAGS += -Wl,--export-all-symbols,--out-implib=libags_thread.dll.a
else
if WITH_OSXAPI
libags_thread_la_LDFLAGS += -Wl,-headerpad_max_install_names -export-symbols libags_thread.sym -Wl,-install_name,libags_thread.dylib
else
libags_thread_la_LDFLAGS += -export-symbols libags_thread.sym -Wl,-soname,libags_thread.so
libags_thread_la_LIBADD += -ldl -lcrypt
endif
endif

# library libags-server
libags_server_la_CPPFLAGS = -std=gnu99 $(CPPFLAGS) $(LIBAGS_SERVER_CPPFLAGS)
libags_server_la_CFLAGS = $(CFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(W32API_CFLAGS)
libags_server_la_LDFLAGS = $(LDFLAGS) -version-info 3:0:0 $(PIC_FLAGS) -pthread
libags_server_la_LIBADD = libags_thread.la libags.la -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(W32API_LIBS)

if WITH_W32API
libags_server_la_LDFLAGS += -Wl,--export-all-symbols,--out-implib=libags_server.dll.a
else
if WITH_OSXAPI
libags_server_la_LDFLAGS += -Wl,-headerpad_max_install_names -export-symbols libags_server.sym -Wl,-install_name,libags_server.dylib
else
libags_server_la_LDFLAGS += -export-symbols libags_server.sym -Wl,-soname,libags_server.so
libags_server_la_LIBADD += -ldl -lcrypt
endif
endif

# library libags-audio
if WITH_OSXAPI
libags_audio_la_CPPFLAGS = -x objective-c -framework Cocoa
else
libags_audio_la_CPPFLAGS = -std=gnu99
endif

libags_audio_la_CPPFLAGS += $(CPPFLAGS) $(LIBAGS_AUDIO_CPPFLAGS)

libags_audio_la_CFLAGS = $(CFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ $(CORE_AUDIO_CFLAGS) $(WASAPI_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBDSSIALSACOMPAT_CFLAGS) $(LIBXML2_CFLAGS) $(SAMPLERATE_CFLAGS) $(SNDFILE_CFLAGS) $(FFTW_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(PULSE_CFLAGS) $(GSTREAMER_CFLAGS) $(W32API_CFLAGS)
libags_audio_la_LDFLAGS = $(LDFLAGS) -version-info 3:0:0 $(PIC_FLAGS) -pthread 
libags_audio_la_LIBADD = libags_server.la libags_thread.la libags.la

if WITH_VST3
libags_audio_la_LIBADD += libags_vst.la
endif

libags_audio_la_LIBADD += -lm $(CORE_AUDIO_LIBS) $(WASAPI_LIBS) $(LIBASOUND2_LIBS) $(LIBDSSIALSACOMPAT_LIBS) $(LIBXML2_LIBS) $(SAMPLERATE_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(FFTW_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(PULSE_LIBS) $(GSTREAMER_LIBS) $(W32API_LIBS)

if WITH_W32API
libags_audio_la_CPPFLAGS += -DAGS_VECTORIZED_BUILTIN_FUNCTIONS=1 -DAGS_W32API=1 -DAGS_LIBRARY_SUFFIX=\".dll\" -include $(top_srcdir)/ags/config.h -I$(top_srcdir) -I$(top_srcdir)/lib -I/mingw64/include -I/mingw64/include/dssi -Werror=return-type -Werror=uninitialized -Werror=maybe-uninitialized
libags_audio_la_LDFLAGS += -Wl,--export-all-symbols,--out-implib=libags_audio.dll.a
else
if WITH_OSXAPI
libags_audio_la_CPPFLAGS += -DAGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS=1 -DAGS_OSXAPI=1 -I/usr/include -I/opt/local/include -I/opt/local/include/dssi -DAGS_LIBRARY_SUFFIX=\".dylib\"
libags_audio_la_LDFLAGS += -Wl,-headerpad_max_install_names -export-symbols libags_audio.sym -Wl,-install_name,libags_audio.dylib
else
libags_audio_la_CPPFLAGS += -DAGS_VECTORIZED_BUILTIN_FUNCTIONS=1 -DAGS_LIBRARY_SUFFIX=\".so\" -Werror=return-type -Werror=uninitialized -Werror=maybe-uninitialized
libags_audio_la_LDFLAGS += -export-symbols libags_audio.sym -Wl,-soname,libags_audio.so
libags_audio_la_LIBADD += -ldl
endif
endif

# library libags-gui
libags_gui_la_CPPFLAGS = -std=gnu99 $(CPPFLAGS) $(LIBAGS_GUI_CPPFLAGS)
libags_gui_la_CFLAGS = $(CFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ $(GOBJECT_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(W32API_CFLAGS)
libags_gui_la_LDFLAGS = $(LDFLAGS) -version-info 3:0:0 $(PIC_FLAGS) -pthread
libags_gui_la_LIBADD = -lm $(GOBJECT_LIBS) $(CAIRO_LIBS) $(GTK_LIBS) $(W32API_LIBS)

if WITH_W32API
libags_gui_la_LDFLAGS += -Wl,--export-all-symbols,--out-implib=libags_gui.dll.a
else
if WITH_OSXAPI
libags_gui_la_LDFLAGS += -Wl,-headerpad_max_install_names -export-symbols libags_gui.sym -Wl,-install_name,libags_gui.dylib
else
libags_gui_la_LDFLAGS += -export-symbols libags_gui.sym -Wl,-soname,libags_gui.so
libags_gui_la_LIBADD += -ldl
endif
endif

# library libgsequencer
if WITH_OSXAPI
libgsequencer_la_CPPFLAGS = -x objective-c -framework Cocoa
else
libgsequencer_la_CPPFLAGS = -std=gnu99
endif

libgsequencer_la_CPPFLAGS += $(CPPFLAGS) $(LIBGSEQUENCER_CPPFLAGS)
libgsequencer_la_CFLAGS = $(CFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ $(CORE_AUDIO_CFLAGS) $(WASAPI_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBDSSIALSACOMPAT_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(FFTW_CFLAGS) $(JACK_CFLAGS) $(PULSE_CFLAGS) $(GSTREAMER_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
libgsequencer_la_LDFLAGS = $(LDFLAGS) -version-info 0:1:0 $(PIC_FLAGS) -pthread
libgsequencer_la_LIBADD = libags_audio.la libags_server.la libags_gui.la libags_thread.la libags.la -lm $(CORE_AUDIO_LIBS) $(WASAPI_LIBS) $(LIBASOUND2_LIBS) $(LIBDSSIALSACOMPAT_LIBS) $(LIBXML2_LIBS) $(FFTW_LIBS) $(SNDFILE_LIBS) $(JACK_LIBS) $(PULSE_LIBS) $(GSTREAMER_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(CAIRO_LIBS) $(GTK_LIBS) $(WEBKITGTK_LIBS) $(POPPLER_LIBS) $(GTK_MAC_INTEGRATION_LIBS) $(W32API_LIBS)

if WITH_W32API
libgsequencer_la_CPPFLAGS += -DAGS_LIBRARY_SUFFIX=\".dll\"
libgsequencer_la_LDFLAGS += -Wl,--export-all-symbols,--out-implib=libgsequencer.dll.a
else
if WITH_OSXAPI
libgsequencer_la_CPPFLAGS += -DAGS_LIBRARY_SUFFIX=\".dylib\"
libgsequencer_la_LDFLAGS += -Wl,-headerpad_max_install_names -Wl,-install_name,libgsequencer.dylib
else
libgsequencer_la_CPPFLAGS += -DAGS_LIBRARY_SUFFIX=\".so\"
libgsequencer_la_LIBADD += -ldl
endif
endif

# library libags-vst
if WITH_VST3
libags_vst_la_CPPFLAGS = -std=gnu++2a $(LIBAGS_VST3_RELEASE_TYPE) $(LIBAGS_VST3_CPPFLAGS) -Wall -Wformat -Werror=format-security -Werror=return-type -Werror=uninitialized -Werror=maybe-uninitialized
libags_vst_la_CXXFLAGS = $(CXXFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ $(GLIB_CFLAGS) $(LIBAGS_VST3_CXXFLAGS) $(W32API_CFLAGS)
libags_vst_la_LDFLAGS = $(LDFLAGS) $(LIBAGS_VST3_LDFLAGS) -version-info 3:0:0 $(PIC_FLAGS) -pthread 
libags_vst_la_LIBADD = -lm $(LIBAGS_VST3_LIBS) -lbase -lpluginterfaces -lsdk -lsdk_common -lsdk_hosting -lvstgui -lvstgui_standalone -lvstgui_support -lvstgui_uidescription $(GLIB_LIBS) $(W32API_LIBS)

if WITH_W32API
libags_vst_la_LDFLAGS += -Wl,--export-all-symbols,--out-implib=libags_vst.dll.a
else
if WITH_OSXAPI
libags_vst_la_LDFLAGS += -export-symbols libags_vst.sym -Wl,-install_name,libags_vst.dylib
else
libags_vst_la_LDFLAGS += -export-symbols libags_vst.sym -Wl,-soname,libags_vst.so
libags_vst_la_LIBADD += -ldl
endif
endif
endif

# gsequencer
gsequencer_SOURCES = $(gsequencer_c_sources) $(gsequencerheaders)

gsequencer_CFLAGS += $(CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBDSSIALSACOMPAT_CFLAGS) $(LIBAO_CFLAGS) $(LIBXML2_CFLAGS) $(LIBSOUP_CFLAGS) $(SNDFILE_CFLAGS) $(JACK_CFLAGS) $(PULSE_CFLAGS) $(GSTREAMER_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(WEBKITGTK_CFLAGS) $(POPPLER_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
gsequencer_LDFLAGS += -pthread
gsequencer_LDADD = libgsequencer.la libags_audio.la libags_server.la libags_gui.la libags_thread.la libags.la -lm $(RT_LIBS) $(X11_LIBS) $(CORE_AUDIO_CFLAGS) $(WASAPI_CFLAGS) $(LIBASOUND2_LIBS) $(LIBDSSIALSACOMPAT_LIBS) $(LIBAO_LIBS) $(LIBXML2_LIBS) $(LIBSOUP_LIBS) $(SNDFILE_LIBS) $(JACK_LIBS) $(PULSE_LIBS) $(GSTREAMER_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(CAIRO_LIBS) $(GTK_LIBS) $(WEBKITGTK_LIBS) $(POPPLER_LIBS) $(GTK_MAC_INTEGRATION_LIBS) $(W32API_LIBS)

if WITH_W32API
else
if WITH_OSXAPI
else
gsequencer_LDADD += -ldl
endif
endif

# unit and functional tests
check_PROGRAMS = 

include $(top_srcdir)/unit-tests.mk

if RUN_FUNCTIONAL_TESTS
include $(top_srcdir)/functional-tests.mk
endif

TESTS = $(check_PROGRAMS)

# internationalization
noinst_HEADERS = ags/i18n.h

# libags all files
deprecated_libags_h_sources =

libags_h_sources = \
	$(deprecated_libags_h_sources) \
	ags/lib/ags_buffer_util.h \
	ags/lib/ags_complex.h \
	ags/lib/ags_conversion.h \
	ags/lib/ags_endian.h \
	ags/lib/ags_function.h \
	ags/lib/ags_log.h \
	ags/lib/ags_math_util.h \
	ags/lib/ags_regex.h \
	ags/lib/ags_solver_matrix.h \
	ags/lib/ags_solver_polynomial.h \
	ags/lib/ags_solver_vector.h \
	ags/lib/ags_string_util.h \
	ags/lib/ags_time.h \
	ags/lib/ags_turtle.h \
	ags/lib/ags_turtle_manager.h \
	ags/lib/ags_uuid.h

deprecated_libags_c_sources =

libags_c_sources = \
	$(deprecated_libags_c_sources) \
	ags/lib/ags_buffer_util.c \
	ags/lib/ags_complex.c \
	ags/lib/ags_conversion.c \
	ags/lib/ags_endian.c \
	ags/lib/ags_function.c \
	ags/lib/ags_log.c \
	ags/lib/ags_math_util.c \
	ags/lib/ags_regex.c \
	ags/lib/ags_solver_matrix.c \
	ags/lib/ags_solver_polynomial.c \
	ags/lib/ags_solver_vector.c \
	ags/lib/ags_string_util.c \
	ags/lib/ags_time.c \
	ags/lib/ags_turtle.c \
	ags/lib/ags_turtle_manager.c \
	ags/lib/ags_uuid.c

deprecated_libags_util_h_sources =

# libags - util
libags_util_h_sources = \
	$(deprecated_libags_util_h_sources) \
	ags/util/ags_destroy_util.h \
	ags/util/ags_id_generator.h \
	ags/util/ags_list_util.h \
	ags/util/ags_soundcard_helper.h

deprecated_libags_util_c_sources =

libags_util_c_sources = \
	$(deprecated_libags_util_c_sources) \
	ags/util/ags_destroy_util.c \
	ags/util/ags_id_generator.c \
	ags/util/ags_list_util.c \
	ags/util/ags_soundcard_helper.c

deprecated_libags_object_h_sources =

# libags - object
libags_object_h_sources =  \
	$(deprecated_libags_object_h_sources) \
	ags/object/ags_applicable.h \
	ags/object/ags_application_context.h \
	ags/object/ags_config.h \
	ags/object/ags_connectable.h \
	ags/object/ags_countable.h \
	ags/object/ags_cursor.h \
	ags/object/ags_globals.h \
	ags/object/ags_macros.h \
	ags/object/ags_main_loop.h \
	ags/object/ags_marshal.h \
	ags/object/ags_mutable.h \
	ags/object/ags_plugin.h \
	ags/object/ags_portlet.h \
	ags/object/ags_priority.h \
	ags/object/ags_seekable.h \
	ags/object/ags_sequencer.h \
	ags/object/ags_soundcard.h \
	ags/object/ags_sound_server.h \
	ags/object/ags_tactable.h

deprecated_libags_object_c_sources =

libags_object_c_sources = \
	$(deprecated_libags_object_c_sources) \
	ags/object/ags_applicable.c \
	ags/object/ags_application_context.c \
	ags/object/ags_config.c \
	ags/object/ags_connectable.c \
	ags/object/ags_countable.c \
	ags/object/ags_cursor.c \
	ags/object/ags_globals.c \
	ags/object/ags_main_loop.c \
	ags/object/ags_marshal.c \
	ags/object/ags_mutable.c \
	ags/object/ags_plugin.c \
	ags/object/ags_portlet.c \
	ags/object/ags_priority.c \
	ags/object/ags_seekable.c \
	ags/object/ags_sequencer.c \
	ags/object/ags_soundcard.c \
	ags/object/ags_sound_server.c \
	ags/object/ags_tactable.c

deprecated_libags_file_h_sources =

# libags - file
libags_file_h_sources = \
	$(deprecated_libags_file_h_sources) \
	ags/file/ags_file.h \
	ags/file/ags_file_id_ref.h \
	ags/file/ags_file_launch.h \
	ags/file/ags_file_link.h \
	ags/file/ags_file_lookup.h

deprecated_libags_file_c_sources =

libags_file_c_sources = \
	$(deprecated_libags_file_c_sources) \
	ags/file/ags_file.c \
	ags/file/ags_file_id_ref.c \
	ags/file/ags_file_launch.c \
	ags/file/ags_file_link.c \
	ags/file/ags_file_lookup.c

# libags-thread all files
deprecated_libags_thread_h_sources =

libags_thread_h_sources = \
	$(deprecated_libags_thread_h_sources) \
	ags/thread/ags_concurrency_provider.h \
	ags/thread/ags_destroy_worker.h \
	ags/thread/ags_generic_main_loop.h \
	ags/thread/ags_message_delivery.h \
	ags/thread/ags_message_envelope.h \
	ags/thread/ags_message_queue.h \
	ags/thread/ags_returnable_thread.h \
	ags/thread/ags_task_completion.h \
	ags/thread/ags_task.h \
	ags/thread/ags_task_launcher.h \
	ags/thread/ags_thread_application_context.h \
	ags/thread/ags_thread_pool.h \
	ags/thread/ags_thread.h \
	ags/thread/ags_timestamp.h \
	ags/thread/ags_worker_thread.h

deprecated_libags_thread_c_sources =

libags_thread_c_sources = \
	$(deprecated_libags_thread_c_sources) \
	ags/thread/ags_concurrency_provider.c \
	ags/thread/ags_destroy_worker.c \
	ags/thread/ags_generic_main_loop.c \
	ags/thread/ags_message_delivery.c \
	ags/thread/ags_message_envelope.c \
	ags/thread/ags_message_queue.c \
	ags/thread/ags_returnable_thread.c \
	ags/thread/ags_task_completion.c \
	ags/thread/ags_task.c \
	ags/thread/ags_task_launcher.c \
	ags/thread/ags_thread_application_context.c \
	ags/thread/ags_thread_pool.c \
	ags/thread/ags_thread.c \
	ags/thread/ags_timestamp.c \
	ags/thread/ags_worker_thread.c

# libags-server all files
deprecated_libags_server_h_sources =

libags_server_h_sources = \
	$(deprecated_libags_server_h_sources) \
	ags/server/ags_registry.h \
	ags/server/ags_server.h \
	ags/server/ags_server_application_context.h \
	ags/server/ags_server_status.h \
	ags/server/ags_service_provider.h

deprecated_libags_server_c_sources =

libags_server_c_sources = \
	$(deprecated_libags_server_c_sources) \
	ags/server/ags_registry.c \
	ags/server/ags_server.c \
	ags/server/ags_server_application_context.c \
	ags/server/ags_server_status.c \
	ags/server/ags_service_provider.c

# libags-server - security
deprecated_libags_server_security_h_sources =

libags_server_security_h_sources = \
	$(deprecated_libags_server_security_h_sources) \
	ags/server/security/ags_auth_security_context.h \
	ags/server/security/ags_authentication.h \
	ags/server/security/ags_authentication_manager.h \
	ags/server/security/ags_business_group.h \
	ags/server/security/ags_business_group_manager.h \
	ags/server/security/ags_certificate.h \
	ags/server/security/ags_certificate_manager.h \
	ags/server/security/ags_password_store.h \
	ags/server/security/ags_password_store_manager.h \
	ags/server/security/ags_security_context.h \
	ags/server/security/ags_xml_authentication.h \
	ags/server/security/ags_xml_business_group.h \
	ags/server/security/ags_xml_certificate.h \
	ags/server/security/ags_xml_password_store.h

deprecated_libags_server_security_c_sources =

libags_server_security_c_sources = \
	$(deprecated_libags_server_security_c_sources) \
	ags/server/security/ags_auth_security_context.c \
	ags/server/security/ags_authentication.c \
	ags/server/security/ags_authentication_manager.c \
	ags/server/security/ags_business_group.c \
	ags/server/security/ags_business_group_manager.c \
	ags/server/security/ags_certificate.c \
	ags/server/security/ags_certificate_manager.c \
	ags/server/security/ags_password_store.c \
	ags/server/security/ags_password_store_manager.c \
	ags/server/security/ags_security_context.c \
	ags/server/security/ags_xml_authentication.c \
	ags/server/security/ags_xml_business_group.c \
	ags/server/security/ags_xml_certificate.c \
	ags/server/security/ags_xml_password_store.c

# libags-server - controller
deprecated_libags_server_controller_h_sources =

libags_server_controller_h_sources = \
	$(deprecated_libags_server_controller_h_sources) \
	ags/server/controller/ags_controller.h \
	ags/server/controller/ags_front_controller.h \
	ags/server/controller/ags_plugin_controller.h

deprecated_libags_server_controller_c_sources =

libags_server_controller_c_sources = \
	$(deprecated_libags_server_controller_c_sources) \
	ags/server/controller/ags_controller.c \
	ags/server/controller/ags_front_controller.c \
	ags/server/controller/ags_plugin_controller.c

# libags-server - file
deprecated_libags_server_file_h_sources =

libags_server_file_h_sources = \
	$(deprecated_libags_server_file_h_sources)

deprecated_libags_server_file_c_sources =

libags_server_file_c_sources = \
	$(deprecated_libags_server_file_c_sources)

# libags-audio all files
deprecated_libags_audio_h_sources =

libags_audio_h_sources = \
	$(deprecated_libags_audio_h_sources) \
	ags/audio/ags_sound_enums.h \
	ags/audio/ags_synth_enums.h \
	ags/audio/ags_acceleration.h \
	ags/audio/ags_audio.h \
	ags/audio/ags_audio_application_context.h \
	ags/audio/ags_audio_buffer_util.h \
	ags/audio/ags_audio_signal.h \
	ags/audio/ags_automation.h \
	ags/audio/ags_buffer.h \
	ags/audio/ags_channel.h \
	ags/audio/ags_char_buffer_util.h \
	ags/audio/ags_devout.h \
	ags/audio/ags_devin.h \
	ags/audio/ags_diatonic_scale.h \
	ags/audio/ags_envelope_util.h \
	ags/audio/ags_fast_pitch_util.h \
	ags/audio/ags_filter_util.h \
	ags/audio/ags_fifoout.h \
	ags/audio/ags_fluid_interpolate_4th_order_util.h \
	ags/audio/ags_fluid_interpolate_7th_order_util.h \
	ags/audio/ags_fluid_iir_filter_util.h \
	ags/audio/ags_fluid_interpolate_linear_util.h \
	ags/audio/ags_fluid_interpolate_none_util.h \
	ags/audio/ags_fluid_pitch_util.h \
	ags/audio/ags_fluid_util.h \
	ags/audio/ags_fm_synth_util.h \
	ags/audio/ags_fourier_transform_util.h \
	ags/audio/ags_frequency_aliase_util.h \
	ags/audio/ags_frequency_map.h \
	ags/audio/ags_frequency_map_manager.h \
	ags/audio/ags_fx_factory.h \
	ags/audio/ags_generic_pitch_util.h \
	ags/audio/ags_generic_recall_channel_run.h \
	ags/audio/ags_generic_recall_recycling.h \
	ags/audio/ags_hq_pitch_util.h \
	ags/audio/ags_input.h \
	ags/audio/ags_lfo_synth_util.h \
	ags/audio/ags_linear_interpolate_util.h \
	ags/audio/ags_midi.h \
	ags/audio/ags_midiin.h \
	ags/audio/ags_notation.h \
	ags/audio/ags_note.h \
	ags/audio/ags_output.h \
	ags/audio/ags_pattern.h \
	ags/audio/ags_peak_util.h \
	ags/audio/ags_phase_shift_util.h \
	ags/audio/ags_playback.h \
	ags/audio/ags_playback_domain.h \
	ags/audio/ags_port.h \
	ags/audio/ags_port_util.h \
	ags/audio/ags_preset.h \
	ags/audio/ags_recall_audio.h \
	ags/audio/ags_recall_audio_run.h \
	ags/audio/ags_recall_audio_signal.h \
	ags/audio/ags_recall_channel.h \
	ags/audio/ags_recall_channel_run.h \
	ags/audio/ags_recall_container.h \
	ags/audio/ags_recall_dependency.h \
	ags/audio/ags_recall_factory.h \
	ags/audio/ags_recall.h \
	ags/audio/ags_recall_dssi.h \
	ags/audio/ags_recall_dssi_run.h \
	ags/audio/ags_recall_id.h \
	ags/audio/ags_recall_ladspa.h \
	ags/audio/ags_recall_ladspa_run.h \
	ags/audio/ags_recall_lv2.h \
	ags/audio/ags_recall_lv2_run.h \
	ags/audio/ags_recall_recycling.h \
	ags/audio/ags_recycling_context.h \
	ags/audio/ags_recycling.h \
	ags/audio/ags_resample_util.h \
	ags/audio/ags_sound_provider.h \
	ags/audio/ags_sequencer_util.h \
	ags/audio/ags_soundcard_util.h \
	ags/audio/ags_sfz_synth_generator.h \
	ags/audio/ags_sfz_synth_util.h \
	ags/audio/ags_sfz_util.h \
	ags/audio/ags_synth_generator.h \
	ags/audio/ags_synth_util.h \
	ags/audio/ags_time_stretch_util.h \
	ags/audio/ags_track.h \
	ags/audio/ags_volume_util.h \
	ags/audio/ags_wave.h

if WITH_LIBINSTPATCH
libags_audio_h_sources += \
	$(deprecated_libags_audio_h_sources) \
	ags/audio/ags_sf2_synth_generator.h \
	ags/audio/ags_sf2_synth_util.h \
	ags/audio/ags_sf2_util.h
endif

deprecated_libags_audio_c_sources =

libags_audio_c_sources = \
	$(deprecated_libags_audio_c_sources) \
	ags/audio/ags_sound_enums.c \
	ags/audio/ags_synth_enums.c \
	ags/audio/ags_acceleration.c \
	ags/audio/ags_audio.c \
	ags/audio/ags_audio_application_context.c \
	ags/audio/ags_audio_buffer_util.c \
	ags/audio/ags_audio_signal.c \
	ags/audio/ags_automation.c \
	ags/audio/ags_buffer.c \
	ags/audio/ags_channel.c \
	ags/audio/ags_char_buffer_util.c \
	ags/audio/ags_devout.c \
	ags/audio/ags_devin.c \
	ags/audio/ags_diatonic_scale.c \
	ags/audio/ags_envelope_util.c \
	ags/audio/ags_fast_pitch_util.c \
	ags/audio/ags_filter_util.c \
	ags/audio/ags_fifoout.c \
	ags/audio/ags_fluid_interpolate_4th_order_util.c \
	ags/audio/ags_fluid_interpolate_7th_order_util.c \
	ags/audio/ags_fluid_iir_filter_util.c \
	ags/audio/ags_fluid_interpolate_linear_util.c \
	ags/audio/ags_fluid_interpolate_none_util.c \
	ags/audio/ags_fluid_pitch_util.c \
	ags/audio/ags_fluid_util.c \
	ags/audio/ags_fm_synth_util.c \
	ags/audio/ags_fourier_transform_util.c \
	ags/audio/ags_frequency_aliase_util.c \
	ags/audio/ags_frequency_map.c \
	ags/audio/ags_frequency_map_manager.c \
	ags/audio/ags_fx_factory.c \
	ags/audio/ags_generic_pitch_util.c \
	ags/audio/ags_generic_recall_channel_run.c \
	ags/audio/ags_generic_recall_recycling.c \
	ags/audio/ags_hq_pitch_util.c \
	ags/audio/ags_input.c \
	ags/audio/ags_lfo_synth_util.c \
	ags/audio/ags_linear_interpolate_util.c \
	ags/audio/ags_midi.c \
	ags/audio/ags_midiin.c \
	ags/audio/ags_notation.c \
	ags/audio/ags_note.c \
	ags/audio/ags_output.c \
	ags/audio/ags_pattern.c \
	ags/audio/ags_peak_util.c \
	ags/audio/ags_phase_shift_util.c \
	ags/audio/ags_playback.c \
	ags/audio/ags_playback_domain.c \
	ags/audio/ags_port.c \
	ags/audio/ags_port_util.c \
	ags/audio/ags_preset.c \
	ags/audio/ags_recall_audio.c \
	ags/audio/ags_recall_audio_run.c \
	ags/audio/ags_recall_audio_signal.c \
	ags/audio/ags_recall.c \
	ags/audio/ags_recall_channel.c \
	ags/audio/ags_recall_channel_run.c \
	ags/audio/ags_recall_container.c \
	ags/audio/ags_recall_dependency.c \
	ags/audio/ags_recall_factory.c \
	ags/audio/ags_recall_dssi.c \
	ags/audio/ags_recall_dssi_run.c \
	ags/audio/ags_recall_id.c \
	ags/audio/ags_recall_ladspa.c \
	ags/audio/ags_recall_ladspa_run.c \
	ags/audio/ags_recall_lv2.c \
	ags/audio/ags_recall_lv2_run.c \
	ags/audio/ags_recall_recycling.c \
	ags/audio/ags_recycling.c \
	ags/audio/ags_recycling_context.c \
	ags/audio/ags_resample_util.c \
	ags/audio/ags_sound_provider.c \
	ags/audio/ags_sequencer_util.c \
	ags/audio/ags_soundcard_util.c \
	ags/audio/ags_sfz_synth_generator.c \
	ags/audio/ags_sfz_synth_util.c \
	ags/audio/ags_sfz_util.c \
	ags/audio/ags_synth_generator.c \
	ags/audio/ags_synth_util.c \
	ags/audio/ags_time_stretch_util.c \
	ags/audio/ags_track.c \
	ags/audio/ags_volume_util.c \
	ags/audio/ags_wave.c

if WITH_LIBINSTPATCH
libags_audio_c_sources += \
	$(deprecated_libags_audio_c_sources) \
	ags/audio/ags_sf2_synth_generator.c \
	ags/audio/ags_sf2_synth_util.c \
	ags/audio/ags_sf2_util.c
endif

# libags-audio - alsa
deprecated_libags_audio_alsa_h_sources =

libags_audio_alsa_h_sources = \
	$(deprecated_libags_audio_alsa_h_sources) \
	ags/audio/alsa/ags_alsa_devout.h

deprecated_libags_audio_alsa_c_sources =

libags_audio_alsa_c_sources = \
	$(deprecated_libags_audio_alsa_c_sources) \
	ags/audio/alsa/ags_alsa_devout.c

# libags-audio - oss
deprecated_libags_audio_oss_h_sources =

libags_audio_oss_h_sources = \
	$(deprecated_libags_audio_oss_h_sources) \
	ags/audio/oss/ags_oss_devout.h

deprecated_libags_audio_oss_c_sources =

libags_audio_oss_c_sources = \
	$(deprecated_libags_audio_oss_c_sources) \
	ags/audio/oss/ags_oss_devout.c

# libags-audio - plugin
deprecated_libags_plugin_h_sources =

libags_plugin_h_sources = \
	$(deprecated_libags_plugin_h_sources) \
	ags/plugin/ags_base_plugin.h \
	ags/plugin/ags_dssi_manager.h \
	ags/plugin/ags_dssi_plugin.h \
	ags/plugin/ags_ladspa_conversion.h \
	ags/plugin/ags_ladspa_manager.h \
	ags/plugin/ags_ladspa_plugin.h \
	ags/plugin/ags_lv2_conversion.h \
	ags/plugin/ags_lv2_manager.h \
	ags/plugin/ags_lv2_turtle_parser.h \
	ags/plugin/ags_lv2_turtle_scanner.h \
	ags/plugin/ags_lv2_plugin.h \
	ags/plugin/ags_lv2_preset_manager.h \
	ags/plugin/ags_lv2_preset.h \
	ags/plugin/ags_lv2_event_manager.h \
	ags/plugin/ags_lv2_log_manager.h \
	ags/plugin/ags_lv2_option_manager.h \
	ags/plugin/ags_lv2_uri_map_manager.h \
	ags/plugin/ags_lv2_urid_manager.h \
	ags/plugin/ags_lv2_worker_manager.h \
	ags/plugin/ags_lv2_worker.h \
	ags/plugin/ags_lv2ui_manager.h \
	ags/plugin/ags_lv2ui_plugin.h \
	ags/plugin/ags_plugin_stock.h \
	ags/plugin/ags_plugin_port.h

if WITH_VST3
libags_plugin_h_sources += \
	ags/plugin/ags_vst3_conversion.h \
	ags/plugin/ags_vst3_manager.h \
	ags/plugin/ags_vst3_plugin.h
endif

deprecated_libags_plugin_c_sources =

libags_plugin_c_sources = \
	$(deprecated_libags_plugin_c_sources) \
	ags/plugin/ags_base_plugin.c \
	ags/plugin/ags_dssi_manager.c \
	ags/plugin/ags_dssi_plugin.c \
	ags/plugin/ags_ladspa_conversion.c \
	ags/plugin/ags_ladspa_manager.c \
	ags/plugin/ags_ladspa_plugin.c \
	ags/plugin/ags_lv2_conversion.c \
	ags/plugin/ags_lv2_manager.c \
	ags/plugin/ags_lv2_turtle_parser.c \
	ags/plugin/ags_lv2_turtle_scanner.c \
	ags/plugin/ags_lv2_plugin.c \
	ags/plugin/ags_lv2_preset_manager.c \
	ags/plugin/ags_lv2_preset.c \
	ags/plugin/ags_lv2_log_manager.c \
	ags/plugin/ags_lv2_event_manager.c \
	ags/plugin/ags_lv2_option_manager.c \
	ags/plugin/ags_lv2_uri_map_manager.c \
	ags/plugin/ags_lv2_urid_manager.c \
	ags/plugin/ags_lv2_worker_manager.c \
	ags/plugin/ags_lv2_worker.c \
	ags/plugin/ags_lv2ui_manager.c \
	ags/plugin/ags_lv2ui_plugin.c \
	ags/plugin/ags_plugin_port.c

if WITH_VST3
libags_plugin_c_sources += \
	ags/plugin/ags_vst3_conversion.c \
	ags/plugin/ags_vst3_manager.c \
	ags/plugin/ags_vst3_plugin.c
endif

# libags-audio - file
deprecated_libags_audio_file_h_sources =

libags_audio_file_h_sources = \
	$(deprecated_libags_audio_file_h_sources) \
	ags/audio/file/ags_audio_container.h \
	ags/audio/file/ags_audio_container_manager.h \
	ags/audio/file/ags_audio_file.h \
	ags/audio/file/ags_audio_file_manager.h \
	ags/audio/file/ags_audio_file_link.h \
	ags/audio/file/ags_sound_container.h \
	ags/audio/file/ags_sound_resource.h \
	ags/audio/file/ags_sndfile.h \
	ags/audio/file/ags_sfz_file.h \
	ags/audio/file/ags_sfz_group.h \
	ags/audio/file/ags_sfz_region.h \
	ags/audio/file/ags_sfz_sample.h	

if WITH_OSXAPI
libags_audio_file_h_sources += \
	ags/audio/file/ags_audio_toolbox.h
endif

if WITH_LIBINSTPATCH
libags_audio_file_h_sources += \
	ags/audio/file/ags_ipatch.h \
	ags/audio/file/ags_ipatch_sample.h \
	ags/audio/file/ags_ipatch_dls2_reader.h \
	ags/audio/file/ags_ipatch_gig_reader.h \
	ags/audio/file/ags_ipatch_sf2_reader.h
endif

if WITH_GSTREAMER
libags_audio_file_h_sources += \
	ags/audio/file/ags_gstreamer_file.h
endif

libags_audio_file_c_sources = \
	$(deprecated_libags_audio_file_c_sources) \
	ags/audio/file/ags_audio_container_manager.c \
	ags/audio/file/ags_audio_container.c \
	ags/audio/file/ags_audio_file.c \
	ags/audio/file/ags_audio_file_manager.c \
	ags/audio/file/ags_audio_file_link.c \
	ags/audio/file/ags_sound_container.c \
	ags/audio/file/ags_sound_resource.c \
	ags/audio/file/ags_sndfile.c \
	ags/audio/file/ags_sfz_file.c \
	ags/audio/file/ags_sfz_group.c \
	ags/audio/file/ags_sfz_region.c \
	ags/audio/file/ags_sfz_sample.c	

if WITH_OSXAPI
libags_audio_file_c_sources += \
	ags/audio/file/ags_audio_toolbox.c
endif

if WITH_LIBINSTPATCH
libags_audio_file_c_sources += \
	ags/audio/file/ags_ipatch.c \
	ags/audio/file/ags_ipatch_sample.c \
	ags/audio/file/ags_ipatch_dls2_reader.c \
	ags/audio/file/ags_ipatch_gig_reader.c \
	ags/audio/file/ags_ipatch_sf2_reader.c
endif

if WITH_GSTREAMER
libags_audio_file_c_sources += \
	ags/audio/file/ags_gstreamer_file.c
endif

# libags-audio - midi
deprecated_libags_audio_midi_h_sources =

libags_audio_midi_h_sources = \
	$(deprecated_libags_audio_midi_h_sources) \
	ags/audio/midi/ags_midi_buffer_util.h \
	ags/audio/midi/ags_midi_util.h \
	ags/audio/midi/ags_midi_builder.h \
	ags/audio/midi/ags_midi_file.h \
	ags/audio/midi/ags_midi_parser.h

deprecated_libags_audio_midi_c_sources =

libags_audio_midi_c_sources = \
	$(deprecated_libags_audio_midi_c_sources) \
	ags/audio/midi/ags_midi_buffer_util.c \
	ags/audio/midi/ags_midi_util.c \
	ags/audio/midi/ags_midi_builder.c \
	ags/audio/midi/ags_midi_file.c \
	ags/audio/midi/ags_midi_parser.c

# libags-audio - osc
deprecated_libags_audio_osc_h_sources =

libags_audio_osc_h_sources = \
	$(deprecated_libags_audio_osc_h_sources) \
	ags/audio/osc/ags_osc_buffer_util.h \
	ags/audio/osc/ags_osc_builder.h \
	ags/audio/osc/ags_osc_client.h \
	ags/audio/osc/ags_osc_connection.h \
	ags/audio/osc/ags_osc_message.h \
	ags/audio/osc/ags_osc_parser.h \
	ags/audio/osc/ags_osc_response.h \
	ags/audio/osc/ags_osc_server.h \
	ags/audio/osc/ags_osc_util.h \
	ags/audio/osc/ags_osc_websocket_connection.h \
	ags/audio/osc/ags_osc_xmlrpc_message.h \
	ags/audio/osc/ags_osc_xmlrpc_server.h

deprecated_libags_audio_osc_c_sources =

libags_audio_osc_c_sources = \
	$(deprecated_libags_audio_osc_c_sources) \
	ags/audio/osc/ags_osc_buffer_util.c \
	ags/audio/osc/ags_osc_builder.c \
	ags/audio/osc/ags_osc_client.c \
	ags/audio/osc/ags_osc_connection.c \
	ags/audio/osc/ags_osc_message.c \
	ags/audio/osc/ags_osc_parser.c \
	ags/audio/osc/ags_osc_response.c \
	ags/audio/osc/ags_osc_server.c \
	ags/audio/osc/ags_osc_util.c \
	ags/audio/osc/ags_osc_websocket_connection.c \
	ags/audio/osc/ags_osc_xmlrpc_message.c \
	ags/audio/osc/ags_osc_xmlrpc_server.c

deprecated_libags_audio_osc_controller_h_sources =

libags_audio_osc_controller_h_sources = \
	$(deprecated_libags_audio_osc_controller_h_sources) \
	ags/audio/osc/controller/ags_osc_controller.h \
	ags/audio/osc/controller/ags_osc_action_controller.h \
	ags/audio/osc/controller/ags_osc_config_controller.h \
	ags/audio/osc/controller/ags_osc_export_controller.h \
	ags/audio/osc/controller/ags_osc_front_controller.h \
	ags/audio/osc/controller/ags_osc_info_controller.h \
	ags/audio/osc/controller/ags_osc_meter_controller.h \
	ags/audio/osc/controller/ags_osc_node_controller.h \
	ags/audio/osc/controller/ags_osc_plugin_controller.h \
	ags/audio/osc/controller/ags_osc_renew_controller.h \
	ags/audio/osc/controller/ags_osc_status_controller.h

deprecated_libags_audio_osc_controller_c_sources =

libags_audio_osc_controller_c_sources = \
	$(deprecated_libags_audio_controller_osc_c_sources) \
	ags/audio/osc/controller/ags_osc_controller.c \
	ags/audio/osc/controller/ags_osc_action_controller.c \
	ags/audio/osc/controller/ags_osc_config_controller.c \
	ags/audio/osc/controller/ags_osc_export_controller.c \
	ags/audio/osc/controller/ags_osc_front_controller.c \
	ags/audio/osc/controller/ags_osc_info_controller.c \
	ags/audio/osc/controller/ags_osc_meter_controller.c \
	ags/audio/osc/controller/ags_osc_node_controller.c \
	ags/audio/osc/controller/ags_osc_plugin_controller.c \
	ags/audio/osc/controller/ags_osc_renew_controller.c \
	ags/audio/osc/controller/ags_osc_status_controller.c

deprecated_libags_audio_osc_xmlrpc_h_sources =

libags_audio_osc_xmlrpc_h_sources = \
	$(deprecated_libags_audio_osc_xmlrpc_h_sources) \
	ags/audio/osc/xmlrpc/ags_osc_xmlrpc_controller.h

deprecated_libags_audio_osc_xmlrpc_c_sources =

libags_audio_osc_xmlrpc_c_sources = \
	$(deprecated_libags_audio_xmlrpc_osc_c_sources) \
	ags/audio/osc/xmlrpc/ags_osc_xmlrpc_controller.c

# libags-audio - jack
deprecated_libags_audio_jack_h_sources =

libags_audio_jack_h_sources = \
	$(deprecated_libags_audio_jack_h_sources) \
	ags/audio/jack/ags_jack_client.h \
	ags/audio/jack/ags_jack_midiin.h \
	ags/audio/jack/ags_jack_devout.h \
	ags/audio/jack/ags_jack_devin.h \
	ags/audio/jack/ags_jack_port.h \
	ags/audio/jack/ags_jack_server.h

deprecated_libags_audio_jack_c_sources =

libags_audio_jack_c_sources = \
	$(deprecated_libags_audio_jack_c_sources) \
	ags/audio/jack/ags_jack_client.c \
	ags/audio/jack/ags_jack_midiin.c \
	ags/audio/jack/ags_jack_devout.c \
	ags/audio/jack/ags_jack_devin.c \
	ags/audio/jack/ags_jack_port.c \
	ags/audio/jack/ags_jack_server.c

# libags-audio - pulse
deprecated_libags_audio_pulse_h_sources =

libags_audio_pulse_h_sources = \
	$(deprecated_libags_audio_pulse_h_sources) \
	ags/audio/pulse/ags_pulse_client.h \
	ags/audio/pulse/ags_pulse_devout.h \
	ags/audio/pulse/ags_pulse_devin.h \
	ags/audio/pulse/ags_pulse_port.h \
	ags/audio/pulse/ags_pulse_server.h

deprecated_libags_audio_pulse_c_sources =

libags_audio_pulse_c_sources = \
	$(deprecated_libags_audio_pulse_c_sources) \
	ags/audio/pulse/ags_pulse_client.c \
	ags/audio/pulse/ags_pulse_devout.c \
	ags/audio/pulse/ags_pulse_devin.c \
	ags/audio/pulse/ags_pulse_port.c \
	ags/audio/pulse/ags_pulse_server.c

# libags-audio - core-audio
deprecated_libags_audio_core_audio_h_sources =

libags_audio_core_audio_h_sources = \
	$(deprecated_libags_audio_core_audio_h_sources) \
	ags/audio/core-audio/ags_core_audio_client.h \
	ags/audio/core-audio/ags_core_audio_midiin.h \
	ags/audio/core-audio/ags_core_audio_devout.h \
	ags/audio/core-audio/ags_core_audio_devin.h \
	ags/audio/core-audio/ags_core_audio_port.h \
	ags/audio/core-audio/ags_core_audio_server.h

deprecated_libags_audio_core_audio_c_sources =

libags_audio_core_audio_c_sources = \
	$(deprecated_libags_audio_core_audio_c_sources) \
	ags/audio/core-audio/ags_core_audio_client.c \
	ags/audio/core-audio/ags_core_audio_midiin.c \
	ags/audio/core-audio/ags_core_audio_devout.c \
	ags/audio/core-audio/ags_core_audio_devin.c \
	ags/audio/core-audio/ags_core_audio_port.c \
	ags/audio/core-audio/ags_core_audio_server.c

# libags-audio - wasapi
deprecated_libags_audio_wasapi_h_sources =

libags_audio_wasapi_h_sources = \
	$(deprecated_libags_audio_wasapi_h_sources) \
	ags/audio/wasapi/ags_wasapi_devout.h \
	ags/audio/wasapi/ags_wasapi_devin.h

deprecated_libags_audio_wasapi_c_sources =

libags_audio_wasapi_c_sources = \
	$(deprecated_libags_audio_wasapi_c_sources) \
	ags/audio/wasapi/ags_wasapi_devout.c \
	ags/audio/wasapi/ags_wasapi_devin.c

# libags-audio - audio-unit
deprecated_libags_audio_audio_unit_h_sources =

libags_audio_audio_unit_h_sources = \
	$(deprecated_libags_audio_audio_unit_h_sources) \
	ags/audio/audio-unit/ags_audio_unit_client.h \
	ags/audio/audio-unit/ags_audio_unit_devin.h \
	ags/audio/audio-unit/ags_audio_unit_devout.h \
	ags/audio/audio-unit/ags_audio_unit_port.h \
	ags/audio/audio-unit/ags_audio_unit_server.h

deprecated_libags_audio_audio_unit_c_sources =

libags_audio_audio_unit_c_sources = \
	$(deprecated_libags_audio_audio_unit_c_sources) \
	ags/audio/audio-unit/ags_audio_unit_client.c \
	ags/audio/audio-unit/ags_audio_unit_devin.c \
	ags/audio/audio-unit/ags_audio_unit_devout.c \
	ags/audio/audio-unit/ags_audio_unit_port.c \
	ags/audio/audio-unit/ags_audio_unit_server.c

# libags-audio - gstreamer
deprecated_libags_audio_gstreamer_h_sources =

libags_audio_gstreamer_h_sources = \
	$(deprecated_libags_audio_gstreamer_h_sources) \
	ags/audio/gstreamer/ags_gstreamer_client.h \
	ags/audio/gstreamer/ags_gstreamer_devout.h \
	ags/audio/gstreamer/ags_gstreamer_devin.h \
	ags/audio/gstreamer/ags_gstreamer_port.h \
	ags/audio/gstreamer/ags_gstreamer_server.h

deprecated_libags_audio_gstreamer_c_sources =

libags_audio_gstreamer_c_sources = \
	$(deprecated_libags_audio_gstreamer_c_sources) \
	ags/audio/gstreamer/ags_gstreamer_client.c \
	ags/audio/gstreamer/ags_gstreamer_devout.c \
	ags/audio/gstreamer/ags_gstreamer_devin.c \
	ags/audio/gstreamer/ags_gstreamer_port.c \
	ags/audio/gstreamer/ags_gstreamer_server.c

# libags-audio - recall
deprecated_libags_audio_recall_h_sources =

libags_audio_recall_h_sources = \
	$(deprecated_libags_audio_recall_h_sources) \
	ags/audio/recall/ags_analyse_audio_signal.h \
	ags/audio/recall/ags_analyse_channel.h \
	ags/audio/recall/ags_analyse_channel_run.h \
	ags/audio/recall/ags_analyse_recycling.h \
	ags/audio/recall/ags_buffer_audio_signal.h \
	ags/audio/recall/ags_buffer_channel.h \
	ags/audio/recall/ags_buffer_channel_run.h \
	ags/audio/recall/ags_buffer_recycling.h \
	ags/audio/recall/ags_capture_wave_audio.h \
	ags/audio/recall/ags_capture_wave_audio_run.h \
	ags/audio/recall/ags_capture_wave_channel.h \
	ags/audio/recall/ags_capture_wave_channel_run.h \
	ags/audio/recall/ags_copy_audio_signal.h \
	ags/audio/recall/ags_copy_channel.h \
	ags/audio/recall/ags_copy_channel_run.h \
	ags/audio/recall/ags_copy_pattern_audio.h \
	ags/audio/recall/ags_copy_pattern_audio_run.h \
	ags/audio/recall/ags_copy_pattern_channel.h \
	ags/audio/recall/ags_copy_pattern_channel_run.h \
	ags/audio/recall/ags_copy_recycling.h \
	ags/audio/recall/ags_count_beats_audio.h \
	ags/audio/recall/ags_count_beats_audio_run.h \
	ags/audio/recall/ags_delay_audio.h \
	ags/audio/recall/ags_delay_audio_run.h \
	ags/audio/recall/ags_envelope_audio_signal.h \
	ags/audio/recall/ags_envelope_channel.h \
	ags/audio/recall/ags_envelope_channel_run.h \
	ags/audio/recall/ags_envelope_recycling.h \
	ags/audio/recall/ags_eq10_audio_signal.h \
	ags/audio/recall/ags_eq10_channel.h \
	ags/audio/recall/ags_eq10_channel_run.h \
	ags/audio/recall/ags_eq10_recycling.h \
	ags/audio/recall/ags_feed_audio_signal.h \
	ags/audio/recall/ags_feed_channel.h \
	ags/audio/recall/ags_feed_channel_run.h \
	ags/audio/recall/ags_feed_recycling.h \
	ags/audio/recall/ags_lfo_audio_signal.h \
	ags/audio/recall/ags_lfo_channel.h \
	ags/audio/recall/ags_lfo_channel_run.h \
	ags/audio/recall/ags_lfo_recycling.h \
	ags/audio/recall/ags_loop_channel.h \
	ags/audio/recall/ags_loop_channel_run.h \
	ags/audio/recall/ags_mute_audio.h \
	ags/audio/recall/ags_mute_audio_run.h \
	ags/audio/recall/ags_mute_audio_signal.h \
	ags/audio/recall/ags_mute_channel.h \
	ags/audio/recall/ags_mute_channel_run.h \
	ags/audio/recall/ags_mute_recycling.h \
	ags/audio/recall/ags_peak_audio_signal.h \
	ags/audio/recall/ags_peak_channel.h \
	ags/audio/recall/ags_peak_channel_run.h \
	ags/audio/recall/ags_peak_recycling.h \
	ags/audio/recall/ags_play_audio.h \
	ags/audio/recall/ags_play_audio_signal.h \
	ags/audio/recall/ags_play_channel.h \
	ags/audio/recall/ags_play_channel_run.h \
	ags/audio/recall/ags_play_channel_run_master.h \
	ags/audio/recall/ags_play_dssi_audio.h \
	ags/audio/recall/ags_play_dssi_audio_run.h \
	ags/audio/recall/ags_play_lv2_audio.h \
	ags/audio/recall/ags_play_lv2_audio_run.h \
	ags/audio/recall/ags_play_notation_audio.h \
	ags/audio/recall/ags_play_notation_audio_run.h \
	ags/audio/recall/ags_play_recycling.h \
	ags/audio/recall/ags_play_wave_audio.h \
	ags/audio/recall/ags_play_wave_audio_run.h \
	ags/audio/recall/ags_play_wave_channel.h \
	ags/audio/recall/ags_play_wave_channel_run.h \
	ags/audio/recall/ags_prepare_audio_signal.h \
	ags/audio/recall/ags_prepare_channel.h \
	ags/audio/recall/ags_prepare_channel_run.h \
	ags/audio/recall/ags_prepare_recycling.h \
	ags/audio/recall/ags_record_midi_audio.h \
	ags/audio/recall/ags_record_midi_audio_run.h \
	ags/audio/recall/ags_route_dssi_audio.h \
	ags/audio/recall/ags_route_dssi_audio_run.h \
	ags/audio/recall/ags_route_lv2_audio.h \
	ags/audio/recall/ags_route_lv2_audio_run.h \
	ags/audio/recall/ags_rt_stream_audio_signal.h \
	ags/audio/recall/ags_rt_stream_channel.h \
	ags/audio/recall/ags_rt_stream_channel_run.h \
	ags/audio/recall/ags_rt_stream_recycling.h \
	ags/audio/recall/ags_stream_audio_signal.h \
	ags/audio/recall/ags_stream_channel.h \
	ags/audio/recall/ags_stream_channel_run.h \
	ags/audio/recall/ags_stream_recycling.h \
	ags/audio/recall/ags_volume_audio_signal.h \
	ags/audio/recall/ags_volume_channel.h \
	ags/audio/recall/ags_volume_channel_run.h \
	ags/audio/recall/ags_volume_recycling.h

deprecated_libags_audio_recall_c_sources =

libags_audio_recall_c_sources = \
	$(deprecated_libags_audio_recall_c_sources) \
	ags/audio/recall/ags_analyse_audio_signal.c \
	ags/audio/recall/ags_analyse_channel.c \
	ags/audio/recall/ags_analyse_channel_run.c \
	ags/audio/recall/ags_analyse_recycling.c \
	ags/audio/recall/ags_buffer_audio_signal.c \
	ags/audio/recall/ags_buffer_channel.c \
	ags/audio/recall/ags_buffer_channel_run.c \
	ags/audio/recall/ags_buffer_recycling.c \
	ags/audio/recall/ags_capture_wave_audio.c \
	ags/audio/recall/ags_capture_wave_audio_run.c \
	ags/audio/recall/ags_capture_wave_channel.c \
	ags/audio/recall/ags_capture_wave_channel_run.c \
	ags/audio/recall/ags_copy_audio_signal.c \
	ags/audio/recall/ags_copy_channel.c \
	ags/audio/recall/ags_copy_channel_run.c \
	ags/audio/recall/ags_copy_pattern_audio.c \
	ags/audio/recall/ags_copy_pattern_audio_run.c \
	ags/audio/recall/ags_copy_pattern_channel.c \
	ags/audio/recall/ags_copy_pattern_channel_run.c \
	ags/audio/recall/ags_copy_recycling.c \
	ags/audio/recall/ags_count_beats_audio.c \
	ags/audio/recall/ags_count_beats_audio_run.c \
	ags/audio/recall/ags_delay_audio.c \
	ags/audio/recall/ags_delay_audio_run.c \
	ags/audio/recall/ags_envelope_audio_signal.c \
	ags/audio/recall/ags_envelope_channel.c \
	ags/audio/recall/ags_envelope_channel_run.c \
	ags/audio/recall/ags_envelope_recycling.c \
	ags/audio/recall/ags_eq10_audio_signal.c \
	ags/audio/recall/ags_eq10_channel.c \
	ags/audio/recall/ags_eq10_channel_run.c \
	ags/audio/recall/ags_eq10_recycling.c \
	ags/audio/recall/ags_feed_audio_signal.c \
	ags/audio/recall/ags_feed_channel.c \
	ags/audio/recall/ags_feed_channel_run.c \
	ags/audio/recall/ags_feed_recycling.c \
	ags/audio/recall/ags_lfo_audio_signal.c \
	ags/audio/recall/ags_lfo_channel.c \
	ags/audio/recall/ags_lfo_channel_run.c \
	ags/audio/recall/ags_lfo_recycling.c \
	ags/audio/recall/ags_loop_channel.c \
	ags/audio/recall/ags_loop_channel_run.c \
	ags/audio/recall/ags_mute_audio.c \
	ags/audio/recall/ags_mute_audio_run.c \
	ags/audio/recall/ags_mute_audio_signal.c \
	ags/audio/recall/ags_mute_channel.c \
	ags/audio/recall/ags_mute_channel_run.c \
	ags/audio/recall/ags_mute_recycling.c \
	ags/audio/recall/ags_peak_audio_signal.c \
	ags/audio/recall/ags_peak_channel.c \
	ags/audio/recall/ags_peak_channel_run.c \
	ags/audio/recall/ags_peak_recycling.c \
	ags/audio/recall/ags_play_audio.c \
	ags/audio/recall/ags_play_audio_signal.c \
	ags/audio/recall/ags_play_channel.c \
	ags/audio/recall/ags_play_channel_run.c \
	ags/audio/recall/ags_play_channel_run_master.c \
	ags/audio/recall/ags_play_dssi_audio.c \
	ags/audio/recall/ags_play_dssi_audio_run.c \
	ags/audio/recall/ags_play_lv2_audio.c \
	ags/audio/recall/ags_play_lv2_audio_run.c \
	ags/audio/recall/ags_play_notation_audio.c \
	ags/audio/recall/ags_play_notation_audio_run.c \
	ags/audio/recall/ags_play_recycling.c \
	ags/audio/recall/ags_play_wave_audio.c \
	ags/audio/recall/ags_play_wave_audio_run.c \
	ags/audio/recall/ags_play_wave_channel.c \
	ags/audio/recall/ags_play_wave_channel_run.c \
	ags/audio/recall/ags_prepare_audio_signal.c \
	ags/audio/recall/ags_prepare_channel.c \
	ags/audio/recall/ags_prepare_channel_run.c \
	ags/audio/recall/ags_prepare_recycling.c \
	ags/audio/recall/ags_record_midi_audio.c \
	ags/audio/recall/ags_record_midi_audio_run.c \
	ags/audio/recall/ags_route_dssi_audio.c \
	ags/audio/recall/ags_route_dssi_audio_run.c \
	ags/audio/recall/ags_route_lv2_audio.c \
	ags/audio/recall/ags_route_lv2_audio_run.c \
	ags/audio/recall/ags_rt_stream_audio_signal.c \
	ags/audio/recall/ags_rt_stream_channel.c \
	ags/audio/recall/ags_rt_stream_channel_run.c \
	ags/audio/recall/ags_rt_stream_recycling.c \
	ags/audio/recall/ags_stream_audio_signal.c \
	ags/audio/recall/ags_stream_channel.c \
	ags/audio/recall/ags_stream_channel_run.c \
	ags/audio/recall/ags_stream_recycling.c \
	ags/audio/recall/ags_volume_audio_signal.c \
	ags/audio/recall/ags_volume_channel.c \
	ags/audio/recall/ags_volume_channel_run.c \
	ags/audio/recall/ags_volume_recycling.c

# libags-audio - fx
deprecated_libags_audio_fx_h_sources =

libags_audio_fx_h_sources = \
	$(deprecated_libags_audio_fx_h_sources) \
	ags/audio/fx/ags_fx_playback_audio.h \
	ags/audio/fx/ags_fx_playback_audio_processor.h \
	ags/audio/fx/ags_fx_playback_channel.h \
	ags/audio/fx/ags_fx_playback_channel_processor.h \
	ags/audio/fx/ags_fx_playback_recycling.h \
	ags/audio/fx/ags_fx_playback_audio_signal.h \
	ags/audio/fx/ags_fx_buffer_audio.h \
	ags/audio/fx/ags_fx_buffer_audio_processor.h \
	ags/audio/fx/ags_fx_buffer_channel.h \
	ags/audio/fx/ags_fx_buffer_channel_processor.h \
	ags/audio/fx/ags_fx_buffer_recycling.h \
	ags/audio/fx/ags_fx_buffer_audio_signal.h \
	ags/audio/fx/ags_fx_volume_audio.h \
	ags/audio/fx/ags_fx_volume_audio_processor.h \
	ags/audio/fx/ags_fx_volume_channel.h \
	ags/audio/fx/ags_fx_volume_channel_processor.h \
	ags/audio/fx/ags_fx_volume_recycling.h \
	ags/audio/fx/ags_fx_volume_audio_signal.h \
	ags/audio/fx/ags_fx_lfo_audio.h \
	ags/audio/fx/ags_fx_lfo_audio_processor.h \
	ags/audio/fx/ags_fx_lfo_channel.h \
	ags/audio/fx/ags_fx_lfo_channel_processor.h \
	ags/audio/fx/ags_fx_lfo_recycling.h \
	ags/audio/fx/ags_fx_lfo_audio_signal.h \
	ags/audio/fx/ags_fx_low_pass_audio.h \
	ags/audio/fx/ags_fx_low_pass_audio_processor.h \
	ags/audio/fx/ags_fx_low_pass_channel.h \
	ags/audio/fx/ags_fx_low_pass_channel_processor.h \
	ags/audio/fx/ags_fx_low_pass_recycling.h \
	ags/audio/fx/ags_fx_low_pass_audio_signal.h \
	ags/audio/fx/ags_fx_high_pass_audio.h \
	ags/audio/fx/ags_fx_high_pass_audio_processor.h \
	ags/audio/fx/ags_fx_high_pass_channel.h \
	ags/audio/fx/ags_fx_high_pass_channel_processor.h \
	ags/audio/fx/ags_fx_high_pass_recycling.h \
	ags/audio/fx/ags_fx_high_pass_audio_signal.h \
	ags/audio/fx/ags_fx_eq10_audio.h \
	ags/audio/fx/ags_fx_eq10_audio_processor.h \
	ags/audio/fx/ags_fx_eq10_channel.h \
	ags/audio/fx/ags_fx_eq10_channel_processor.h \
	ags/audio/fx/ags_fx_eq10_recycling.h \
	ags/audio/fx/ags_fx_eq10_audio_signal.h \
	ags/audio/fx/ags_fx_peak_audio.h \
	ags/audio/fx/ags_fx_peak_audio_processor.h \
	ags/audio/fx/ags_fx_peak_channel.h \
	ags/audio/fx/ags_fx_peak_channel_processor.h \
	ags/audio/fx/ags_fx_peak_recycling.h \
	ags/audio/fx/ags_fx_peak_audio_signal.h \
	ags/audio/fx/ags_fx_envelope_audio.h \
	ags/audio/fx/ags_fx_envelope_audio_processor.h \
	ags/audio/fx/ags_fx_envelope_channel.h \
	ags/audio/fx/ags_fx_envelope_channel_processor.h \
	ags/audio/fx/ags_fx_envelope_recycling.h \
	ags/audio/fx/ags_fx_envelope_audio_signal.h \
	ags/audio/fx/ags_fx_analyse_audio.h \
	ags/audio/fx/ags_fx_analyse_audio_processor.h \
	ags/audio/fx/ags_fx_analyse_channel.h \
	ags/audio/fx/ags_fx_analyse_channel_processor.h \
	ags/audio/fx/ags_fx_analyse_recycling.h \
	ags/audio/fx/ags_fx_analyse_audio_signal.h \
	ags/audio/fx/ags_fx_two_pass_aliase_audio.h \
	ags/audio/fx/ags_fx_two_pass_aliase_audio_processor.h \
	ags/audio/fx/ags_fx_two_pass_aliase_channel.h \
	ags/audio/fx/ags_fx_two_pass_aliase_channel_processor.h \
	ags/audio/fx/ags_fx_two_pass_aliase_recycling.h \
	ags/audio/fx/ags_fx_two_pass_aliase_audio_signal.h \
	ags/audio/fx/ags_fx_pattern_audio.h \
	ags/audio/fx/ags_fx_pattern_audio_processor.h \
	ags/audio/fx/ags_fx_pattern_channel.h \
	ags/audio/fx/ags_fx_pattern_channel_processor.h \
	ags/audio/fx/ags_fx_pattern_recycling.h \
	ags/audio/fx/ags_fx_pattern_audio_signal.h \
	ags/audio/fx/ags_fx_notation_audio.h \
	ags/audio/fx/ags_fx_notation_audio_processor.h \
	ags/audio/fx/ags_fx_notation_channel.h \
	ags/audio/fx/ags_fx_notation_channel_processor.h \
	ags/audio/fx/ags_fx_notation_recycling.h \
	ags/audio/fx/ags_fx_notation_audio_signal.h \
	ags/audio/fx/ags_fx_ladspa_audio.h \
	ags/audio/fx/ags_fx_ladspa_audio_processor.h \
	ags/audio/fx/ags_fx_ladspa_channel.h \
	ags/audio/fx/ags_fx_ladspa_channel_processor.h \
	ags/audio/fx/ags_fx_ladspa_recycling.h \
	ags/audio/fx/ags_fx_ladspa_audio_signal.h \
	ags/audio/fx/ags_fx_dssi_audio.h \
	ags/audio/fx/ags_fx_dssi_audio_processor.h \
	ags/audio/fx/ags_fx_dssi_channel.h \
	ags/audio/fx/ags_fx_dssi_channel_processor.h \
	ags/audio/fx/ags_fx_dssi_recycling.h \
	ags/audio/fx/ags_fx_dssi_audio_signal.h \
	ags/audio/fx/ags_fx_lv2_audio.h \
	ags/audio/fx/ags_fx_lv2_audio_processor.h \
	ags/audio/fx/ags_fx_lv2_channel.h \
	ags/audio/fx/ags_fx_lv2_channel_processor.h \
	ags/audio/fx/ags_fx_lv2_recycling.h \
	ags/audio/fx/ags_fx_lv2_audio_signal.h

if WITH_VST3
libags_audio_fx_h_sources += \
	ags/audio/fx/ags_fx_vst3_audio.h \
	ags/audio/fx/ags_fx_vst3_audio_processor.h \
	ags/audio/fx/ags_fx_vst3_channel.h \
	ags/audio/fx/ags_fx_vst3_channel_processor.h \
	ags/audio/fx/ags_fx_vst3_recycling.h \
	ags/audio/fx/ags_fx_vst3_audio_signal.h
endif

deprecated_libags_audio_fx_c_sources =

libags_audio_fx_c_sources = \
	$(deprecated_libags_audio_fx_c_sources) \
	ags/audio/fx/ags_fx_playback_audio.c \
	ags/audio/fx/ags_fx_playback_audio_processor.c \
	ags/audio/fx/ags_fx_playback_channel.c \
	ags/audio/fx/ags_fx_playback_channel_processor.c \
	ags/audio/fx/ags_fx_playback_recycling.c \
	ags/audio/fx/ags_fx_playback_audio_signal.c \
	ags/audio/fx/ags_fx_buffer_audio.c \
	ags/audio/fx/ags_fx_buffer_audio_processor.c \
	ags/audio/fx/ags_fx_buffer_channel.c \
	ags/audio/fx/ags_fx_buffer_channel_processor.c \
	ags/audio/fx/ags_fx_buffer_recycling.c \
	ags/audio/fx/ags_fx_buffer_audio_signal.c \
	ags/audio/fx/ags_fx_volume_audio.c \
	ags/audio/fx/ags_fx_volume_audio_processor.c \
	ags/audio/fx/ags_fx_volume_channel.c \
	ags/audio/fx/ags_fx_volume_channel_processor.c \
	ags/audio/fx/ags_fx_volume_recycling.c \
	ags/audio/fx/ags_fx_volume_audio_signal.c \
	ags/audio/fx/ags_fx_low_pass_audio.c \
	ags/audio/fx/ags_fx_low_pass_audio_processor.c \
	ags/audio/fx/ags_fx_low_pass_channel.c \
	ags/audio/fx/ags_fx_low_pass_channel_processor.c \
	ags/audio/fx/ags_fx_low_pass_recycling.c \
	ags/audio/fx/ags_fx_low_pass_audio_signal.c \
	ags/audio/fx/ags_fx_high_pass_audio.c \
	ags/audio/fx/ags_fx_high_pass_audio_processor.c \
	ags/audio/fx/ags_fx_high_pass_channel.c \
	ags/audio/fx/ags_fx_high_pass_channel_processor.c \
	ags/audio/fx/ags_fx_high_pass_recycling.c \
	ags/audio/fx/ags_fx_high_pass_audio_signal.c \
	ags/audio/fx/ags_fx_lfo_audio.c \
	ags/audio/fx/ags_fx_lfo_audio_processor.c \
	ags/audio/fx/ags_fx_lfo_channel.c \
	ags/audio/fx/ags_fx_lfo_channel_processor.c \
	ags/audio/fx/ags_fx_lfo_recycling.c \
	ags/audio/fx/ags_fx_lfo_audio_signal.c \
	ags/audio/fx/ags_fx_eq10_audio.c \
	ags/audio/fx/ags_fx_eq10_audio_processor.c \
	ags/audio/fx/ags_fx_eq10_channel.c \
	ags/audio/fx/ags_fx_eq10_channel_processor.c \
	ags/audio/fx/ags_fx_eq10_recycling.c \
	ags/audio/fx/ags_fx_eq10_audio_signal.c \
	ags/audio/fx/ags_fx_peak_audio.c \
	ags/audio/fx/ags_fx_peak_audio_processor.c \
	ags/audio/fx/ags_fx_peak_channel.c \
	ags/audio/fx/ags_fx_peak_channel_processor.c \
	ags/audio/fx/ags_fx_peak_recycling.c \
	ags/audio/fx/ags_fx_peak_audio_signal.c \
	ags/audio/fx/ags_fx_envelope_audio.c \
	ags/audio/fx/ags_fx_envelope_audio_processor.c \
	ags/audio/fx/ags_fx_envelope_channel.c \
	ags/audio/fx/ags_fx_envelope_channel_processor.c \
	ags/audio/fx/ags_fx_envelope_recycling.c \
	ags/audio/fx/ags_fx_envelope_audio_signal.c \
	ags/audio/fx/ags_fx_analyse_audio.c \
	ags/audio/fx/ags_fx_analyse_audio_processor.c \
	ags/audio/fx/ags_fx_analyse_channel.c \
	ags/audio/fx/ags_fx_analyse_channel_processor.c \
	ags/audio/fx/ags_fx_analyse_recycling.c \
	ags/audio/fx/ags_fx_analyse_audio_signal.c \
	ags/audio/fx/ags_fx_two_pass_aliase_audio.c \
	ags/audio/fx/ags_fx_two_pass_aliase_audio_processor.c \
	ags/audio/fx/ags_fx_two_pass_aliase_channel.c \
	ags/audio/fx/ags_fx_two_pass_aliase_channel_processor.c \
	ags/audio/fx/ags_fx_two_pass_aliase_recycling.c \
	ags/audio/fx/ags_fx_two_pass_aliase_audio_signal.c \
	ags/audio/fx/ags_fx_pattern_audio.c \
	ags/audio/fx/ags_fx_pattern_audio_processor.c \
	ags/audio/fx/ags_fx_pattern_channel.c \
	ags/audio/fx/ags_fx_pattern_channel_processor.c \
	ags/audio/fx/ags_fx_pattern_recycling.c \
	ags/audio/fx/ags_fx_pattern_audio_signal.c \
	ags/audio/fx/ags_fx_notation_audio.c \
	ags/audio/fx/ags_fx_notation_audio_processor.c \
	ags/audio/fx/ags_fx_notation_channel.c \
	ags/audio/fx/ags_fx_notation_channel_processor.c \
	ags/audio/fx/ags_fx_notation_recycling.c \
	ags/audio/fx/ags_fx_notation_audio_signal.c \
	ags/audio/fx/ags_fx_ladspa_audio.c \
	ags/audio/fx/ags_fx_ladspa_audio_processor.c \
	ags/audio/fx/ags_fx_ladspa_channel.c \
	ags/audio/fx/ags_fx_ladspa_channel_processor.c \
	ags/audio/fx/ags_fx_ladspa_recycling.c \
	ags/audio/fx/ags_fx_ladspa_audio_signal.c \
	ags/audio/fx/ags_fx_dssi_audio.c \
	ags/audio/fx/ags_fx_dssi_audio_processor.c \
	ags/audio/fx/ags_fx_dssi_channel.c \
	ags/audio/fx/ags_fx_dssi_channel_processor.c \
	ags/audio/fx/ags_fx_dssi_recycling.c \
	ags/audio/fx/ags_fx_dssi_audio_signal.c \
	ags/audio/fx/ags_fx_lv2_audio.c \
	ags/audio/fx/ags_fx_lv2_audio_processor.c \
	ags/audio/fx/ags_fx_lv2_channel.c \
	ags/audio/fx/ags_fx_lv2_channel_processor.c \
	ags/audio/fx/ags_fx_lv2_recycling.c \
	ags/audio/fx/ags_fx_lv2_audio_signal.c

if WITH_VST3
libags_audio_fx_c_sources += \
	ags/audio/fx/ags_fx_vst3_audio.c \
	ags/audio/fx/ags_fx_vst3_audio_processor.c \
	ags/audio/fx/ags_fx_vst3_channel.c \
	ags/audio/fx/ags_fx_vst3_channel_processor.c \
	ags/audio/fx/ags_fx_vst3_recycling.c \
	ags/audio/fx/ags_fx_vst3_audio_signal.c
endif

# libags-audio - task
deprecated_libags_audio_task_h_sources =

libags_audio_task_h_sources = \
	$(deprecated_libags_audio_task_h_sources) \
	ags/audio/task/ags_add_audio.h \
	ags/audio/task/ags_add_audio_signal.h \
	ags/audio/task/ags_add_effect.h \
	ags/audio/task/ags_add_note.h \
	ags/audio/task/ags_add_soundcard.h \
	ags/audio/task/ags_apply_presets.h \
	ags/audio/task/ags_apply_sound_config.h \
	ags/audio/task/ags_apply_synth.h \
	ags/audio/task/ags_apply_sfz_synth.h \
	ags/audio/task/ags_cancel_audio.h \
	ags/audio/task/ags_cancel_channel.h \
	ags/audio/task/ags_clear_audio_signal.h \
	ags/audio/task/ags_clear_buffer.h \
	ags/audio/task/ags_crop_note.h \
	ags/audio/task/ags_export_output.h \
	ags/audio/task/ags_free_selection.h \
	ags/audio/task/ags_link_channel.h \
	ags/audio/task/ags_move_note.h \
	ags/audio/task/ags_open_file.h \
	ags/audio/task/ags_open_single_file.h \
	ags/audio/task/ags_open_sfz_file.h \
	ags/audio/task/ags_open_wave.h \
	ags/audio/task/ags_remove_audio.h \
	ags/audio/task/ags_remove_audio_signal.h \
	ags/audio/task/ags_remove_note.h \
	ags/audio/task/ags_remove_soundcard.h \
	ags/audio/task/ags_resize_audio.h \
	ags/audio/task/ags_seek_soundcard.h \
	ags/audio/task/ags_set_audio_channels.h \
	ags/audio/task/ags_set_buffer_size.h \
	ags/audio/task/ags_set_device.h \
	ags/audio/task/ags_set_format.h \
	ags/audio/task/ags_set_samplerate.h \
	ags/audio/task/ags_start_audio.h \
	ags/audio/task/ags_start_channel.h \
	ags/audio/task/ags_start_sequencer.h \
	ags/audio/task/ags_start_soundcard.h \
	ags/audio/task/ags_stop_sequencer.h \
	ags/audio/task/ags_stop_soundcard.h \
	ags/audio/task/ags_switch_buffer_flag.h \
	ags/audio/task/ags_tic_device.h \
	ags/audio/task/ags_toggle_pattern_bit.h \
	ags/audio/task/ags_apply_bpm.h \
	ags/audio/task/ags_apply_sequencer_length.h \
	ags/audio/task/ags_apply_tact.h \
	ags/audio/task/ags_reset_amplitude.h \
	ags/audio/task/ags_reset_note.h \
	ags/audio/task/ags_reset_peak.h \
	ags/audio/task/ags_reset_fx_analyse.h \
	ags/audio/task/ags_reset_fx_peak.h \
	ags/audio/task/ags_set_muted.h


if WITH_LIBINSTPATCH
libags_audio_task_h_sources += \
	ags/audio/task/ags_apply_sf2_synth.h \
	ags/audio/task/ags_open_sf2_instrument.h \
	ags/audio/task/ags_open_sf2_sample.h
endif

if WITH_VST3
libags_audio_task_h_sources += \
	ags/audio/task/ags_instantiate_vst3_plugin.h \
	ags/audio/task/ags_write_vst3_port.h
endif

deprecated_libags_audio_task_c_sources =

libags_audio_task_c_sources = \
	$(deprecated_libags_audio_task_c_sources) \
	ags/audio/task/ags_add_audio.c \
	ags/audio/task/ags_add_audio_signal.c \
	ags/audio/task/ags_add_effect.c \
	ags/audio/task/ags_add_note.c \
	ags/audio/task/ags_add_soundcard.c \
	ags/audio/task/ags_apply_presets.c \
	ags/audio/task/ags_apply_sound_config.c \
	ags/audio/task/ags_apply_synth.c \
	ags/audio/task/ags_apply_sfz_synth.c \
	ags/audio/task/ags_cancel_audio.c \
	ags/audio/task/ags_cancel_channel.c \
	ags/audio/task/ags_clear_audio_signal.c \
	ags/audio/task/ags_clear_buffer.c \
	ags/audio/task/ags_crop_note.c \
	ags/audio/task/ags_export_output.c \
	ags/audio/task/ags_free_selection.c \
	ags/audio/task/ags_link_channel.c \
	ags/audio/task/ags_move_note.c \
	ags/audio/task/ags_open_file.c \
	ags/audio/task/ags_open_single_file.c \
	ags/audio/task/ags_open_sfz_file.c \
	ags/audio/task/ags_open_wave.c \
	ags/audio/task/ags_remove_audio.c \
	ags/audio/task/ags_remove_audio_signal.c \
	ags/audio/task/ags_remove_note.c \
	ags/audio/task/ags_remove_soundcard.c \
	ags/audio/task/ags_resize_audio.c \
	ags/audio/task/ags_seek_soundcard.c \
	ags/audio/task/ags_set_audio_channels.c \
	ags/audio/task/ags_set_buffer_size.c \
	ags/audio/task/ags_set_device.c \
	ags/audio/task/ags_set_format.c \
	ags/audio/task/ags_set_samplerate.c \
	ags/audio/task/ags_start_audio.c \
	ags/audio/task/ags_start_channel.c \
	ags/audio/task/ags_start_sequencer.c \
	ags/audio/task/ags_start_soundcard.c \
	ags/audio/task/ags_stop_sequencer.c \
	ags/audio/task/ags_stop_soundcard.c \
	ags/audio/task/ags_switch_buffer_flag.c \
	ags/audio/task/ags_tic_device.c \
	ags/audio/task/ags_toggle_pattern_bit.c \
	ags/audio/task/ags_apply_bpm.c \
	ags/audio/task/ags_apply_sequencer_length.c \
	ags/audio/task/ags_apply_tact.c \
	ags/audio/task/ags_reset_amplitude.c \
	ags/audio/task/ags_reset_note.c \
	ags/audio/task/ags_reset_peak.c \
	ags/audio/task/ags_reset_fx_analyse.c \
	ags/audio/task/ags_reset_fx_peak.c \
	ags/audio/task/ags_set_muted.c

if WITH_LIBINSTPATCH
libags_audio_task_c_sources += \
	ags/audio/task/ags_apply_sf2_synth.c \
	ags/audio/task/ags_open_sf2_instrument.c \
	ags/audio/task/ags_open_sf2_sample.c
endif

if WITH_VST3
libags_audio_task_c_sources += \
	ags/audio/task/ags_instantiate_vst3_plugin.c \
	ags/audio/task/ags_write_vst3_port.c
endif

# libags-gui all files
deprecated_libags_gui_h_sources =

libags_gui_h_sources = \
	$(deprecated_libags_gui_h_sources) \
	ags/widget/ags_cartesian.h \
	ags/widget/ags_dial.h \
	ags/widget/ags_expander.h \
	ags/widget/ags_expander_set.h \
	ags/widget/ags_hindicator.h \
	ags/widget/ags_hled_array.h \
	ags/widget/ags_indicator.h \
	ags/widget/ags_led.h \
	ags/widget/ags_led_array.h \
	ags/widget/ags_level.h \
	ags/widget/ags_level_box.h \
	ags/widget/ags_hlevel_box.h \
	ags/widget/ags_vlevel_box.h \
	ags/widget/ags_notebook.h \
	ags/widget/ags_piano_keys.h \
	ags/widget/ags_piano.h \
	ags/widget/ags_scrolled_piano.h \
	ags/widget/ags_scale.h \
	ags/widget/ags_scale_box.h \
	ags/widget/ags_vscale_box.h \
	ags/widget/ags_hscale_box.h \
	ags/widget/ags_scrolled_level_box.h \
	ags/widget/ags_scrolled_scale_box.h \
	ags/widget/ags_ruler.h \
	ags/widget/ags_vindicator.h \
	ags/widget/ags_vled_array.h \
	ags/widget/ags_viewport.h \
	ags/widget/ags_widget_marshal.h \
	ags/widget/ags_container.h

deprecated_libags_gui_c_sources =

libags_gui_c_sources = \
	$(deprecated_libags_gui_c_sources) \
	ags/widget/ags_cartesian.c \
	ags/widget/ags_dial.c \
	ags/widget/ags_expander.c \
	ags/widget/ags_expander_set.c \
	ags/widget/ags_hindicator.c \
	ags/widget/ags_hled_array.c \
	ags/widget/ags_indicator.c \
	ags/widget/ags_led.c \
	ags/widget/ags_led_array.c \
	ags/widget/ags_level.c \
	ags/widget/ags_level_box.c \
	ags/widget/ags_hlevel_box.c \
	ags/widget/ags_vlevel_box.c \
	ags/widget/ags_notebook.c \
	ags/widget/ags_piano.c \
	ags/widget/ags_scrolled_piano.c \
	ags/widget/ags_scale.c \
	ags/widget/ags_scale_box.c \
	ags/widget/ags_vscale_box.c \
	ags/widget/ags_hscale_box.c \
	ags/widget/ags_scrolled_scale_box.c \
	ags/widget/ags_scrolled_level_box.c \
	ags/widget/ags_ruler.c \
	ags/widget/ags_vindicator.c \
	ags/widget/ags_vled_array.c \
	ags/widget/ags_viewport.c \
	ags/widget/ags_widget_marshal.c \
	ags/widget/ags_container.c

# libgsequencer all files
deprecated_libgsequencer_h_sources =

libgsequencer_h_sources = \
	$(deprecated_libgsequencer_h_sources) \
	ags/X/ags_animation_window.h \
	ags/X/ags_automation_editor_callbacks.h \
	ags/X/ags_automation_editor.h \
	ags/X/ags_automation_window_callbacks.h \
	ags/X/ags_automation_window.h \
	ags/X/ags_audio_preferences_callbacks.h \
	ags/X/ags_audio_preferences.h \
	ags/X/ags_bulk_member_callbacks.h \
	ags/X/ags_bulk_member.h \
	ags/X/ags_composite_editor.h \
	ags/X/ags_composite_editor_callbacks.h \
	ags/X/ags_connection_editor_callbacks.h \
	ags/X/ags_connection_editor.h \
	ags/X/ags_context_menu.h \
	ags/X/ags_dssi_browser_callbacks.h \
	ags/X/ags_dssi_browser.h \
	ags/X/ags_effect_bridge_callbacks.h \
	ags/X/ags_effect_bridge.h \
	ags/X/ags_effect_bulk_callbacks.h \
	ags/X/ags_effect_bulk.h \
	ags/X/ags_effect_line_callbacks.h \
	ags/X/ags_effect_line.h \
	ags/X/ags_effect_pad_callbacks.h \
	ags/X/ags_effect_pad.h \
	ags/X/ags_effect_separator.h \
	ags/X/ags_export_soundcard_callbacks.h \
	ags/X/ags_export_soundcard.h \
	ags/X/ags_export_window_callbacks.h \
	ags/X/ags_export_window.h \
	ags/X/ags_generic_preferences_callbacks.h \
	ags/X/ags_generic_preferences.h \
	ags/X/ags_input_collection_editor_callbacks.h \
	ags/X/ags_input_collection_editor.h \
	ags/X/ags_input_editor_callbacks.h \
	ags/X/ags_input_editor.h \
	ags/X/ags_input_listing_editor_callbacks.h \
	ags/X/ags_input_listing_editor.h \
	ags/X/ags_ladspa_browser_callbacks.h \
	ags/X/ags_ladspa_browser.h \
	ags/X/ags_lv2_browser_callbacks.h \
	ags/X/ags_lv2_browser.h \
	ags/X/ags_line_callbacks.h \
	ags/X/ags_line_editor_callbacks.h \
	ags/X/ags_line_editor.h \
	ags/X/ags_line.h \
	ags/X/ags_line_member_callbacks.h \
	ags/X/ags_line_member_editor_callbacks.h \
	ags/X/ags_line_member_editor.h \
	ags/X/ags_line_member.h \
	ags/X/ags_link_collection_editor_callbacks.h \
	ags/X/ags_link_collection_editor.h \
	ags/X/ags_link_editor_callbacks.h \
	ags/X/ags_link_editor.h \
	ags/X/ags_listing_editor_callbacks.h \
	ags/X/ags_listing_editor.h \
	ags/X/ags_machine_callbacks.h \
	ags/X/ags_machine_editor_callbacks.h \
	ags/X/ags_machine_editor.h \
	ags/X/ags_machine.h \
	ags/X/ags_machine_util.h \
	ags/X/ags_menu_action_callbacks.h \
	ags/X/ags_menu_bar.h \
	ags/X/ags_midi_dialog_callbacks.h \
	ags/X/ags_midi_dialog.h \
	ags/X/ags_midi_preferences.h \
	ags/X/ags_midi_preferences_callbacks.h \
	ags/X/ags_navigation_callbacks.h \
	ags/X/ags_navigation.h \
	ags/X/ags_notation_editor_callbacks.h \
	ags/X/ags_notation_editor.h \
	ags/X/ags_online_help_window.h \
	ags/X/ags_online_help_window_callbacks.h \
	ags/X/ags_osc_server_preferences_callbacks.h \
	ags/X/ags_osc_server_preferences.h \
	ags/X/ags_output_collection_editor_callbacks.h \
	ags/X/ags_output_collection_editor.h \
	ags/X/ags_output_editor_callbacks.h \
	ags/X/ags_output_editor.h \
	ags/X/ags_output_listing_editor_callbacks.h \
	ags/X/ags_output_listing_editor.h \
	ags/X/ags_pad_callbacks.h \
	ags/X/ags_pad_editor_callbacks.h \
	ags/X/ags_pad_editor.h \
	ags/X/ags_pad.h \
	ags/X/ags_performance_preferences_callbacks.h \
	ags/X/ags_performance_preferences.h \
	ags/X/ags_playback_window_callbacks.h \
	ags/X/ags_playback_window.h \
	ags/X/ags_plugin_browser_callbacks.h \
	ags/X/ags_plugin_browser.h \
	ags/X/ags_plugin_preferences_callbacks.h \
	ags/X/ags_plugin_preferences.h \
	ags/X/ags_preferences_callbacks.h \
	ags/X/ags_preferences.h \
	ags/X/ags_property_collection_editor_callbacks.h \
	ags/X/ags_property_collection_editor.h \
	ags/X/ags_property_editor_callbacks.h \
	ags/X/ags_property_editor.h \
	ags/X/ags_property_listing_editor.h \
	ags/X/ags_quit_dialog_callbacks.h \
	ags/X/ags_quit_dialog.h \
	ags/X/ags_resize_editor.h \
	ags/X/ags_sequencer_editor_callbacks.h \
	ags/X/ags_sequencer_editor.h \
	ags/X/ags_server_preferences_callbacks.h \
	ags/X/ags_server_preferences.h \
	ags/X/ags_sheet_window.h \
	ags/X/ags_sheet_window_callbacks.h \
	ags/X/ags_sheet_editor.h \
	ags/X/ags_sheet_editor_callbacks.h \
	ags/X/ags_soundcard_editor_callbacks.h \
	ags/X/ags_soundcard_editor.h \
	ags/X/ags_ui_provider.h \
	ags/X/ags_wave_window_callbacks.h \
	ags/X/ags_wave_window.h \
	ags/X/ags_wave_editor_callbacks.h \
	ags/X/ags_wave_editor.h \
	ags/X/ags_window.h \
	ags/X/ags_window_callbacks.h \
	ags/X/ags_xorg_application_context.h

if WITH_VST3
libgsequencer_h_sources += \
	ags/X/ags_vst3_browser_callbacks.h \
	ags/X/ags_vst3_browser.h
endif

deprecated_libgsequencer_c_sources =

libgsequencer_c_sources = \
	$(deprecated_libgsequencer_c_sources) \
	ags/X/ags_animation_window.c \
	ags/X/ags_automation_editor.c \
	ags/X/ags_automation_editor_callbacks.c \
	ags/X/ags_automation_window.c \
	ags/X/ags_automation_window_callbacks.c \
	ags/X/ags_audio_preferences.c \
	ags/X/ags_audio_preferences_callbacks.c \
	ags/X/ags_bulk_member.c \
	ags/X/ags_bulk_member_callbacks.c \
	ags/X/ags_composite_editor.c \
	ags/X/ags_composite_editor_callbacks.c \
	ags/X/ags_connection_editor_callbacks.c \
	ags/X/ags_connection_editor.c \
	ags/X/ags_context_menu.c \
	ags/X/ags_dssi_browser_callbacks.c \
	ags/X/ags_dssi_browser.c \
	ags/X/ags_effect_bridge.c \
	ags/X/ags_effect_bridge_callbacks.c \
	ags/X/ags_effect_bulk.c \
	ags/X/ags_effect_bulk_callbacks.c \
	ags/X/ags_effect_line.c \
	ags/X/ags_effect_line_callbacks.c \
	ags/X/ags_effect_pad.c \
	ags/X/ags_effect_pad_callbacks.c \
	ags/X/ags_effect_separator.c \
	ags/X/ags_export_soundcard.c \
	ags/X/ags_export_soundcard_callbacks.c \
	ags/X/ags_export_window.c \
	ags/X/ags_export_window_callbacks.c \
	ags/X/ags_generic_preferences.c \
	ags/X/ags_generic_preferences_callbacks.c \
	ags/X/ags_ladspa_browser.c \
	ags/X/ags_ladspa_browser_callbacks.c \
	ags/X/ags_lv2_browser.c \
	ags/X/ags_lv2_browser_callbacks.c \
	ags/X/ags_input_collection_editor_callbacks.c \
	ags/X/ags_input_collection_editor.c \
	ags/X/ags_input_editor_callbacks.c \
	ags/X/ags_input_editor.c \
	ags/X/ags_input_listing_editor_callbacks.c \
	ags/X/ags_input_listing_editor.c \
	ags/X/ags_line.c \
	ags/X/ags_line_callbacks.c \
	ags/X/ags_line_editor.c \
	ags/X/ags_line_editor_callbacks.c \
	ags/X/ags_line_member.c \
	ags/X/ags_line_member_callbacks.c \
	ags/X/ags_line_member_editor.c \
	ags/X/ags_line_member_editor_callbacks.c \
	ags/X/ags_link_collection_editor.c \
	ags/X/ags_link_collection_editor_callbacks.c \
	ags/X/ags_link_editor.c \
	ags/X/ags_link_editor_callbacks.c \
	ags/X/ags_listing_editor.c \
	ags/X/ags_listing_editor_callbacks.c \
	ags/X/ags_machine.c \
	ags/X/ags_machine_util.c \
	ags/X/ags_machine_callbacks.c \
	ags/X/ags_machine_editor.c \
	ags/X/ags_machine_editor_callbacks.c \
	ags/X/ags_menu_action_callbacks.c \
	ags/X/ags_menu_bar.c \
	ags/X/ags_midi_dialog.c \
	ags/X/ags_midi_dialog_callbacks.c \
	ags/X/ags_midi_preferences.c \
	ags/X/ags_midi_preferences_callbacks.c \
	ags/X/ags_navigation.c \
	ags/X/ags_navigation_callbacks.c \
	ags/X/ags_notation_editor.c \
	ags/X/ags_notation_editor_callbacks.c \
	ags/X/ags_online_help_window.c \
	ags/X/ags_online_help_window_callbacks.c \
	ags/X/ags_osc_server_preferences_callbacks.c \
	ags/X/ags_osc_server_preferences.c \
	ags/X/ags_output_collection_editor_callbacks.c \
	ags/X/ags_output_collection_editor.c \
	ags/X/ags_output_editor_callbacks.c \
	ags/X/ags_output_editor.c \
	ags/X/ags_output_listing_editor_callbacks.c \
	ags/X/ags_output_listing_editor.c \
	ags/X/ags_pad.c \
	ags/X/ags_pad_callbacks.c \
	ags/X/ags_pad_editor.c \
	ags/X/ags_pad_editor_callbacks.c \
	ags/X/ags_performance_preferences.c \
	ags/X/ags_performance_preferences_callbacks.c \
	ags/X/ags_playback_window.c \
	ags/X/ags_playback_window_callbacks.c \
	ags/X/ags_plugin_browser.c \
	ags/X/ags_plugin_browser_callbacks.c \
	ags/X/ags_plugin_preferences.c \
	ags/X/ags_plugin_preferences_callbacks.c \
	ags/X/ags_preferences.c \
	ags/X/ags_preferences_callbacks.c \
	ags/X/ags_property_collection_editor.c \
	ags/X/ags_property_collection_editor_callbacks.c \
	ags/X/ags_property_editor.c \
	ags/X/ags_property_editor_callbacks.c \
	ags/X/ags_property_listing_editor.c \
	ags/X/ags_quit_dialog_callbacks.c \
	ags/X/ags_quit_dialog.c \
	ags/X/ags_resize_editor.c \
	ags/X/ags_sequencer_editor_callbacks.c \
	ags/X/ags_sequencer_editor.c \
	ags/X/ags_server_preferences.c \
	ags/X/ags_server_preferences_callbacks.c \
	ags/X/ags_sheet_window.c \
	ags/X/ags_sheet_window_callbacks.c \
	ags/X/ags_sheet_editor.c \
	ags/X/ags_sheet_editor_callbacks.c \
	ags/X/ags_ui_provider.c \
	ags/X/ags_wave_window_callbacks.c \
	ags/X/ags_wave_window.c \
	ags/X/ags_wave_editor.c \
	ags/X/ags_wave_editor_callbacks.c \
	ags/X/ags_window.c \
	ags/X/ags_soundcard_editor_callbacks.c \
	ags/X/ags_soundcard_editor.c \
	ags/X/ags_window_callbacks.c \
	ags/X/ags_xorg_application_context.c

if WITH_VST3
libgsequencer_c_sources += \
	ags/X/ags_vst3_browser_callbacks.c \
	ags/X/ags_vst3_browser.c
endif

# libgsequencer - OSC conotroller
deprecated_libgsequencer_osc_controller_h_sources =

libgsequencer_osc_controller_h_sources = \
	$(deprecated_libgsequencer_osc_controller_h_sources) \
	ags/X/osc/controller/ags_ui_osc_renew_controller.h

deprecated_libgsequencer_osc_controller_c_sources =

libgsequencer_osc_controller_c_sources = \
	$(deprecated_libgsequencer_osc_controller_c_sources) \
	ags/X/osc/controller/ags_ui_osc_renew_controller.c

# libgsequencer - file
deprecated_libgsequencer_file_h_sources =

libgsequencer_file_h_sources = \
	$(deprecated_libgsequencer_file_h_sources) \
	ags/X/file/ags_simple_file.h

deprecated_libgsequencer_file_c_sources =

libgsequencer_file_c_sources = \
	$(deprecated_libgsequencer_file_c_sources) \
	ags/X/file/ags_simple_file.c

# libgsequencer - task
deprecated_libgsequencer_task_h_sources =

libgsequencer_task_h_sources = \
	$(deprecated_libgsequencer_task_h_sources) \
	ags/X/task/ags_simple_file_read.h \
	ags/X/task/ags_simple_file_write.h

deprecated_libgsequencer_task_c_sources =

libgsequencer_task_c_sources = \
	$(deprecated_libgsequencer_task_c_sources) \
	ags/X/task/ags_simple_file_read.c \
	ags/X/task/ags_simple_file_write.c

# libgsequencer - import
deprecated_libgsequencer_import_h_sources =

libgsequencer_import_h_sources = \
	$(deprecated_libgsequencer_import_h_sources) \
	ags/X/import/ags_midi_import_wizard.h \
	ags/X/import/ags_midi_import_wizard_callbacks.h \
	ags/X/import/ags_track_collection.h \
	ags/X/import/ags_track_collection_callbacks.h \
	ags/X/import/ags_track_collection_mapper.h \
	ags/X/import/ags_track_collection_mapper_callbacks.h

deprecated_libgsequencer_import_c_sources =

libgsequencer_import_c_sources = \
	$(deprecated_libgsequencer_import_c_sources) \
	ags/X/import/ags_midi_import_wizard.c \
	ags/X/import/ags_midi_import_wizard_callbacks.c \
	ags/X/import/ags_track_collection.c \
	ags/X/import/ags_track_collection_callbacks.c \
	ags/X/import/ags_track_collection_mapper.c \
	ags/X/import/ags_track_collection_mapper_callbacks.c

# libgsequencer - export
deprecated_libgsequencer_export_h_sources =

libgsequencer_export_h_sources = \
	$(deprecated_libgsequencer_export_h_sources) \
	ags/X/export/ags_midi_export_wizard.h \
	ags/X/export/ags_midi_export_wizard_callbacks.h \
	ags/X/export/ags_machine_collection.h \
	ags/X/export/ags_machine_collection_callbacks.h \
	ags/X/export/ags_machine_collection_entry.h \
	ags/X/export/ags_machine_collection_entry_callbacks.h \
	ags/X/export/ags_wave_export_dialog.h \
	ags/X/export/ags_wave_export_dialog_callbacks.h

deprecated_libgsequencer_export_c_sources =

libgsequencer_export_c_sources = \
	$(deprecated_libgsequencer_export_c_sources) \
	ags/X/export/ags_midi_export_wizard.c \
	ags/X/export/ags_midi_export_wizard_callbacks.c \
	ags/X/export/ags_machine_collection.c \
	ags/X/export/ags_machine_collection_callbacks.c \
	ags/X/export/ags_machine_collection_entry.c \
	ags/X/export/ags_machine_collection_entry_callbacks.c \
	ags/X/export/ags_wave_export_dialog.c \
	ags/X/export/ags_wave_export_dialog_callbacks.c

# libgsequencer - machine
deprecated_libgsequencer_machine_h_sources =

libgsequencer_machine_h_sources = \
	$(deprecated_libgsequencer_machine_h_sources) \
	ags/X/machine/ags_audiorec.h \
	ags/X/machine/ags_audiorec_callbacks.h \
	ags/X/machine/ags_cell_pattern_callbacks.h \
	ags/X/machine/ags_cell_pattern.h \
	ags/X/machine/ags_desk_callbacks.h \
	ags/X/machine/ags_desk.h \
	ags/X/machine/ags_desk_file_chooser.h \
	ags/X/machine/ags_desk_pad_callbacks.h \
	ags/X/machine/ags_desk_pad.h \
	ags/X/machine/ags_drum_callbacks.h \
	ags/X/machine/ags_drum.h \
	ags/X/machine/ags_drum_input_line_callbacks.h \
	ags/X/machine/ags_drum_input_line.h \
	ags/X/machine/ags_drum_input_pad_callbacks.h \
	ags/X/machine/ags_drum_input_pad.h \
	ags/X/machine/ags_drum_output_line_callbacks.h \
	ags/X/machine/ags_drum_output_line.h \
	ags/X/machine/ags_drum_output_pad_callbacks.h \
	ags/X/machine/ags_drum_output_pad.h \
	ags/X/machine/ags_dssi_bridge_callbacks.h \
	ags/X/machine/ags_dssi_bridge.h \
	ags/X/machine/ags_equalizer10.h \
	ags/X/machine/ags_equalizer10_callbacks.h \
	ags/X/machine/ags_fm_oscillator_callbacks.h \
	ags/X/machine/ags_fm_oscillator.h \
	ags/X/machine/ags_fm_syncsynth.h \
	ags/X/machine/ags_fm_syncsynth_callbacks.h \
	ags/X/machine/ags_fm_synth.h \
	ags/X/machine/ags_fm_synth_callbacks.h \
	ags/X/machine/ags_fm_synth_input_pad.h \
	ags/X/machine/ags_fm_synth_input_line.h \
	ags/X/machine/ags_fm_synth_input_line_callbacks.h \
	ags/X/machine/ags_ladspa_bridge_callbacks.h \
	ags/X/machine/ags_ladspa_bridge.h \
	ags/X/machine/ags_live_dssi_bridge_callbacks.h \
	ags/X/machine/ags_live_dssi_bridge.h \
	ags/X/machine/ags_live_lv2_bridge_callbacks.h \
	ags/X/machine/ags_live_lv2_bridge.h \
	ags/X/machine/ags_lv2_bridge_callbacks.h \
	ags/X/machine/ags_lv2_bridge.h \
	ags/X/machine/ags_matrix_callbacks.h \
	ags/X/machine/ags_matrix.h \
	ags/X/machine/ags_matrix_bridge_callbacks.h \
	ags/X/machine/ags_matrix_bridge.h \
	ags/X/machine/ags_matrix_bulk_input_callbacks.h \
	ags/X/machine/ags_matrix_bulk_input.h \
	ags/X/machine/ags_mixer_callbacks.h \
	ags/X/machine/ags_mixer.h \
	ags/X/machine/ags_mixer_input_line.h \
	ags/X/machine/ags_mixer_input_pad.h \
	ags/X/machine/ags_oscillator_callbacks.h \
	ags/X/machine/ags_oscillator.h \
	ags/X/machine/ags_panel_callbacks.h \
	ags/X/machine/ags_panel.h \
	ags/X/machine/ags_panel_input_line.h \
	ags/X/machine/ags_panel_input_line_callbacks.h \
	ags/X/machine/ags_panel_input_pad.h \
	ags/X/machine/ags_pattern_box_callbacks.h \
	ags/X/machine/ags_pattern_box.h \
	ags/X/machine/ags_pitch_sampler.h \
	ags/X/machine/ags_pitch_sampler_callbacks.h \
	ags/X/machine/ags_pitch_sampler_file.h \
	ags/X/machine/ags_pitch_sampler_file_callbacks.h \
	ags/X/machine/ags_sfz_synth.h \
	ags/X/machine/ags_sfz_synth_callbacks.h \
	ags/X/machine/ags_synth_callbacks.h \
	ags/X/machine/ags_syncsynth.h \
	ags/X/machine/ags_syncsynth_callbacks.h \
	ags/X/machine/ags_synth.h \
	ags/X/machine/ags_synth_input_line.h \
	ags/X/machine/ags_synth_input_line_callbacks.h \
	ags/X/machine/ags_synth_input_pad.h \
	ags/X/machine/ags_spectrometer.h \
	ags/X/machine/ags_spectrometer_callbacks.h

if WITH_LIBINSTPATCH
libgsequencer_machine_h_sources += \
	ags/X/machine/ags_ffplayer_callbacks.h \
	ags/X/machine/ags_ffplayer.h \
	ags/X/machine/ags_ffplayer_bridge_callbacks.h \
	ags/X/machine/ags_ffplayer_bridge.h \
	ags/X/machine/ags_ffplayer_bulk_input_callbacks.h \
	ags/X/machine/ags_ffplayer_bulk_input.h \
	ags/X/machine/ags_ffplayer_input_line_callbacks.h \
	ags/X/machine/ags_ffplayer_input_line.h \
	ags/X/machine/ags_ffplayer_input_pad_callbacks.h \
	ags/X/machine/ags_ffplayer_input_pad.h \
	ags/X/machine/ags_sf2_synth_callbacks.h \
	ags/X/machine/ags_sf2_synth.h
endif

if WITH_VST3
libgsequencer_machine_h_sources += \
	ags/X/machine/ags_live_vst3_bridge_callbacks.h \
	ags/X/machine/ags_live_vst3_bridge.h \
	ags/X/machine/ags_vst3_bridge_callbacks.h \
	ags/X/machine/ags_vst3_bridge.h
endif

deprecated_libgsequencer_machine_c_sources =

libgsequencer_machine_c_sources = \
	$(deprecated_libgsequencer_machine_c_sources) \
	ags/X/machine/ags_audiorec.c \
	ags/X/machine/ags_audiorec_callbacks.c \
	ags/X/machine/ags_cell_pattern.c \
	ags/X/machine/ags_cell_pattern_callbacks.c \
	ags/X/machine/ags_desk_callbacks.c \
	ags/X/machine/ags_desk.c \
	ags/X/machine/ags_desk_file_chooser.c \
	ags/X/machine/ags_desk_pad_callbacks.c \
	ags/X/machine/ags_desk_pad.c \
	ags/X/machine/ags_drum.c \
	ags/X/machine/ags_drum_callbacks.c \
	ags/X/machine/ags_drum_input_line.c \
	ags/X/machine/ags_drum_input_line_callbacks.c \
	ags/X/machine/ags_drum_input_pad.c \
	ags/X/machine/ags_drum_input_pad_callbacks.c \
	ags/X/machine/ags_drum_output_line.c \
	ags/X/machine/ags_drum_output_line_callbacks.c \
	ags/X/machine/ags_drum_output_pad.c \
	ags/X/machine/ags_drum_output_pad_callbacks.c \
	ags/X/machine/ags_dssi_bridge.c \
	ags/X/machine/ags_dssi_bridge_callbacks.c \
	ags/X/machine/ags_equalizer10.c \
	ags/X/machine/ags_equalizer10_callbacks.c \
	ags/X/machine/ags_fm_oscillator_callbacks.c \
	ags/X/machine/ags_fm_oscillator.c \
	ags/X/machine/ags_fm_syncsynth.c \
	ags/X/machine/ags_fm_syncsynth_callbacks.c \
	ags/X/machine/ags_fm_synth.c \
	ags/X/machine/ags_fm_synth_callbacks.c \
	ags/X/machine/ags_fm_synth_input_pad.c \
	ags/X/machine/ags_fm_synth_input_line.c \
	ags/X/machine/ags_fm_synth_input_line_callbacks.c \
	ags/X/machine/ags_ladspa_bridge.c \
	ags/X/machine/ags_ladspa_bridge_callbacks.c \
	ags/X/machine/ags_live_dssi_bridge.c \
	ags/X/machine/ags_live_dssi_bridge_callbacks.c \
	ags/X/machine/ags_live_lv2_bridge.c \
	ags/X/machine/ags_live_lv2_bridge_callbacks.c \
	ags/X/machine/ags_lv2_bridge.c \
	ags/X/machine/ags_lv2_bridge_callbacks.c \
	ags/X/machine/ags_matrix.c \
	ags/X/machine/ags_matrix_callbacks.c \
	ags/X/machine/ags_matrix_bridge.c \
	ags/X/machine/ags_matrix_bridge_callbacks.c \
	ags/X/machine/ags_matrix_bulk_input.c \
	ags/X/machine/ags_matrix_bulk_input_callbacks.c \
	ags/X/machine/ags_mixer.c \
	ags/X/machine/ags_mixer_callbacks.c \
	ags/X/machine/ags_mixer_input_line.c \
	ags/X/machine/ags_mixer_input_pad.c \
	ags/X/machine/ags_oscillator.c \
	ags/X/machine/ags_oscillator_callbacks.c \
	ags/X/machine/ags_panel.c \
	ags/X/machine/ags_panel_callbacks.c \
	ags/X/machine/ags_panel_input_line.c \
	ags/X/machine/ags_panel_input_line_callbacks.c \
	ags/X/machine/ags_panel_input_pad.c \
	ags/X/machine/ags_pattern_box_callbacks.c \
	ags/X/machine/ags_pattern_box.c \
	ags/X/machine/ags_pitch_sampler.c \
	ags/X/machine/ags_pitch_sampler_callbacks.c \
	ags/X/machine/ags_pitch_sampler_file.c \
	ags/X/machine/ags_pitch_sampler_file_callbacks.c \
	ags/X/machine/ags_sfz_synth.c \
	ags/X/machine/ags_sfz_synth_callbacks.c \
	ags/X/machine/ags_syncsynth.c \
	ags/X/machine/ags_syncsynth_callbacks.c \
	ags/X/machine/ags_synth.c \
	ags/X/machine/ags_synth_callbacks.c \
	ags/X/machine/ags_synth_input_line.c \
	ags/X/machine/ags_synth_input_line_callbacks.c \
	ags/X/machine/ags_synth_input_pad.c \
	ags/X/machine/ags_spectrometer.c \
	ags/X/machine/ags_spectrometer_callbacks.c

if WITH_LIBINSTPATCH
libgsequencer_machine_c_sources += \
	ags/X/machine/ags_ffplayer.c \
	ags/X/machine/ags_ffplayer_callbacks.c \
	ags/X/machine/ags_ffplayer_bridge.c \
	ags/X/machine/ags_ffplayer_bridge_callbacks.c \
	ags/X/machine/ags_ffplayer_bulk_input.c \
	ags/X/machine/ags_ffplayer_bulk_input_callbacks.c \
	ags/X/machine/ags_ffplayer_input_line.c \
	ags/X/machine/ags_ffplayer_input_line_callbacks.c \
	ags/X/machine/ags_ffplayer_input_pad.c \
	ags/X/machine/ags_ffplayer_input_pad_callbacks.c \
	ags/X/machine/ags_sf2_synth.c \
	ags/X/machine/ags_sf2_synth_callbacks.c
endif

if WITH_VST3
libgsequencer_machine_c_sources += \
	ags/X/machine/ags_live_vst3_bridge.c \
	ags/X/machine/ags_live_vst3_bridge_callbacks.c \
	ags/X/machine/ags_vst3_bridge.c \
	ags/X/machine/ags_vst3_bridge_callbacks.c
endif

# libgsequencer - editor
deprecated_libgsequencer_editor_h_sources =

libgsequencer_editor_h_sources = \
	$(deprecated_libgsequencer_editor_h_sources) \
	ags/X/editor/ags_automation_edit_callbacks.h \
	ags/X/editor/ags_automation_edit_box.h \
	ags/X/editor/ags_automation_edit.h \
	ags/X/editor/ags_automation_meta_callbacks.h \
	ags/X/editor/ags_automation_meta.h \
	ags/X/editor/ags_automation_toolbar_callbacks.h \
	ags/X/editor/ags_automation_toolbar.h \
	ags/X/editor/ags_composite_edit.h \
	ags/X/editor/ags_composite_edit_callbacks.h \
	ags/X/editor/ags_composite_toolbar.h \
	ags/X/editor/ags_composite_toolbar_callbacks.h \
	ags/X/editor/ags_crop_note_dialog_callbacks.h \
	ags/X/editor/ags_crop_note_dialog.h \
	ags/X/editor/ags_envelope_dialog_callbacks.h \
	ags/X/editor/ags_envelope_dialog.h \
	ags/X/editor/ags_envelope_editor_callbacks.h \
	ags/X/editor/ags_envelope_editor.h \
	ags/X/editor/ags_envelope_info_callbacks.h \
	ags/X/editor/ags_envelope_info.h \
	ags/X/editor/ags_machine_radio_button.h \
	ags/X/editor/ags_machine_radio_button_callbacks.h \
	ags/X/editor/ags_machine_selection.h \
	ags/X/editor/ags_machine_selector_callbacks.h \
	ags/X/editor/ags_machine_selector.h \
	ags/X/editor/ags_move_note_dialog_callbacks.h \
	ags/X/editor/ags_move_note_dialog.h \
	ags/X/editor/ags_notation_edit_callbacks.h \
	ags/X/editor/ags_notation_edit.h \
	ags/X/editor/ags_notation_meta_callbacks.h \
	ags/X/editor/ags_notation_meta.h \
	ags/X/editor/ags_notation_toolbar_callbacks.h \
	ags/X/editor/ags_notation_toolbar.h \
	ags/X/editor/ags_pattern_envelope_callbacks.h \
	ags/X/editor/ags_pattern_envelope.h \
	ags/X/editor/ags_position_automation_cursor_dialog.h \
	ags/X/editor/ags_position_automation_cursor_dialog_callbacks.h \
	ags/X/editor/ags_position_notation_cursor_dialog.h \
	ags/X/editor/ags_position_notation_cursor_dialog_callbacks.h \
	ags/X/editor/ags_position_wave_cursor_dialog.h \
	ags/X/editor/ags_position_wave_cursor_dialog_callbacks.h \
	ags/X/editor/ags_ramp_acceleration_dialog.h \
	ags/X/editor/ags_ramp_acceleration_dialog_callbacks.h \
	ags/X/editor/ags_scrolled_automation_edit_box.h \
	ags/X/editor/ags_scrolled_wave_edit_box.h \
	ags/X/editor/ags_select_acceleration_dialog.h \
	ags/X/editor/ags_select_acceleration_dialog_callbacks.h \
	ags/X/editor/ags_select_buffer_dialog.h \
	ags/X/editor/ags_select_buffer_dialog_callbacks.h \
	ags/X/editor/ags_select_note_dialog.h \
	ags/X/editor/ags_select_note_dialog_callbacks.h \
	ags/X/editor/ags_sheet_edit.h \
	ags/X/editor/ags_sheet_edit_callbacks.h \
	ags/X/editor/ags_sheet_toolbar.h \
	ags/X/editor/ags_sheet_toolbar_callbacks.h \
	ags/X/editor/ags_time_stretch_buffer_dialog.h \
	ags/X/editor/ags_time_stretch_buffer_dialog_callbacks.h \
	ags/X/editor/ags_vautomation_edit_box.h \
	ags/X/editor/ags_vwave_edit_box.h \
	ags/X/editor/ags_wave_edit_callbacks.h \
	ags/X/editor/ags_wave_edit.h \
	ags/X/editor/ags_wave_edit_box.h \
	ags/X/editor/ags_wave_meta_callbacks.h \
	ags/X/editor/ags_wave_meta.h \
	ags/X/editor/ags_wave_toolbar_callbacks.h \
	ags/X/editor/ags_wave_toolbar.h

deprecated_libgsequencer_editor_c_sources =

libgsequencer_editor_c_sources = \
	$(deprecated_libgsequencer_editor_c_sources) \
	ags/X/editor/ags_automation_edit.c \
	ags/X/editor/ags_automation_edit_box.c \
	ags/X/editor/ags_automation_edit_callbacks.c \
	ags/X/editor/ags_automation_meta.c \
	ags/X/editor/ags_automation_meta_callbacks.c \
	ags/X/editor/ags_automation_toolbar.c \
	ags/X/editor/ags_automation_toolbar_callbacks.c \
	ags/X/editor/ags_composite_edit.c \
	ags/X/editor/ags_composite_edit_callbacks.c \
	ags/X/editor/ags_composite_toolbar.c \
	ags/X/editor/ags_composite_toolbar_callbacks.c \
	ags/X/editor/ags_crop_note_dialog_callbacks.c \
	ags/X/editor/ags_crop_note_dialog.c \
	ags/X/editor/ags_envelope_dialog_callbacks.c \
	ags/X/editor/ags_envelope_dialog.c \
	ags/X/editor/ags_envelope_editor_callbacks.c \
	ags/X/editor/ags_envelope_editor.c \
	ags/X/editor/ags_envelope_info_callbacks.c \
	ags/X/editor/ags_envelope_info.c \
	ags/X/editor/ags_machine_radio_button.c \
	ags/X/editor/ags_machine_radio_button_callbacks.c \
	ags/X/editor/ags_machine_selection.c \
	ags/X/editor/ags_machine_selector.c \
	ags/X/editor/ags_machine_selector_callbacks.c \
	ags/X/editor/ags_move_note_dialog_callbacks.c \
	ags/X/editor/ags_move_note_dialog.c \
	ags/X/editor/ags_notation_edit.c \
	ags/X/editor/ags_notation_edit_callbacks.c \
	ags/X/editor/ags_notation_meta.c \
	ags/X/editor/ags_notation_meta_callbacks.c \
	ags/X/editor/ags_notation_toolbar.c \
	ags/X/editor/ags_notation_toolbar_callbacks.c \
	ags/X/editor/ags_pattern_envelope_callbacks.c \
	ags/X/editor/ags_pattern_envelope.c \
	ags/X/editor/ags_position_automation_cursor_dialog.c \
	ags/X/editor/ags_position_automation_cursor_dialog_callbacks.c \
	ags/X/editor/ags_position_notation_cursor_dialog.c \
	ags/X/editor/ags_position_notation_cursor_dialog_callbacks.c \
	ags/X/editor/ags_position_wave_cursor_dialog.c \
	ags/X/editor/ags_position_wave_cursor_dialog_callbacks.c \
	ags/X/editor/ags_ramp_acceleration_dialog.c \
	ags/X/editor/ags_ramp_acceleration_dialog_callbacks.c \
	ags/X/editor/ags_scrolled_automation_edit_box.c \
	ags/X/editor/ags_scrolled_wave_edit_box.c \
	ags/X/editor/ags_select_acceleration_dialog.c \
	ags/X/editor/ags_select_acceleration_dialog_callbacks.c \
	ags/X/editor/ags_select_buffer_dialog.c \
	ags/X/editor/ags_select_buffer_dialog_callbacks.c \
	ags/X/editor/ags_select_note_dialog.c \
	ags/X/editor/ags_select_note_dialog_callbacks.c \
	ags/X/editor/ags_sheet_edit.c \
	ags/X/editor/ags_sheet_edit_callbacks.c \
	ags/X/editor/ags_sheet_toolbar.c \
	ags/X/editor/ags_sheet_toolbar_callbacks.c \
	ags/X/editor/ags_time_stretch_buffer_dialog.c \
	ags/X/editor/ags_time_stretch_buffer_dialog_callbacks.c \
	ags/X/editor/ags_vautomation_edit_box.c \
	ags/X/editor/ags_vwave_edit_box.c \
	ags/X/editor/ags_wave_edit.c \
	ags/X/editor/ags_wave_edit_box.c \
	ags/X/editor/ags_wave_edit_callbacks.c \
	ags/X/editor/ags_wave_meta.c \
	ags/X/editor/ags_wave_meta_callbacks.c \
	ags/X/editor/ags_wave_toolbar.c \
	ags/X/editor/ags_wave_toolbar_callbacks.c

# libags-vst
deprecated_libags_vst_h_sources =

libags_vst_h_sources = \
	$(deprecated_libags_vst_h_sources) \
	ags/vst3-capi/ags_vst_headers.h \
	ags/vst3-capi/host/ags_vst_component_handler.h \
	ags/vst3-capi/host/ags_vst_host_context.h \
	ags/vst3-capi/pluginterfaces/gui/ags_vst_iplug_view.h \
	ags/vst3-capi/pluginterfaces/gui/ags_vst_iplug_view_content_scale_support.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_types.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iattributes.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iaudio_processor.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iautomation_state.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_icomponent.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_ichannel_context_info.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_icontext_menu.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_ievents.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iedit_controller.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iinter_app_audio.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_imessage.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_imidi_controllers.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_imidi_learn.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_inote_expression.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iparameter_changes.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iphysical_ui.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iparameter_function_name.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iplug_interface_support.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iplug_view.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iprocess_context.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iprefetchable_support.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_irepresentation.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iunits.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_ihost_application.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_preset_keys.h \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_speaker.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_icloneable.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_geo_constants.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_ierror_context.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_fvariant.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_ftypes.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_iplugin_base.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_futils.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_ibstream.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_fstrdefs.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_const_string_table.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_ipersistent.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_istring_result.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_iupdate_handler.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_fplatform.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_smart_pointer.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_ustring.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_ucolor_spec.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_keycodes.h \
	ags/vst3-capi/util/ags_vst_string_util.h \
	ags/vst3-capi/base/source/ags_vst_fbuffer.h \
	ags/vst3-capi/base/source/ags_vst_fobject.h \
	ags/vst3-capi/base/source/ags_vst_fstring.h \
	ags/vst3-capi/public.sdk/source/common/ags_vst_plugin_view.h \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_audio_effect.h \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_component.h \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_component_base.h \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_edit_controller.h \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_bus.h \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_parameters.h \
	ags/vst3-capi/public.sdk/source/vst/hosting/ags_vst_event_list.h \
	ags/vst3-capi/public.sdk/source/vst/hosting/ags_vst_module.h \
	ags/vst3-capi/public.sdk/source/vst/hosting/ags_vst_parameter_changes.h \
	ags/vst3-capi/public.sdk/source/vst/hosting/ags_vst_plug_interface_support.h \
	ags/vst3-capi/public.sdk/source/vst/utility/ags_vst_uid.h


deprecated_libags_vst_c_sources =

libags_vst_c_sources = \
	$(deprecated_libags_vst_c_sources) \
	ags/vst3-capi/host/ags_vst_component_handler.cpp \
	ags/vst3-capi/host/ags_vst_host_context.cpp \
	ags/vst3-capi/host/componenthandler.h \
	ags/vst3-capi/host/hostcontext.h \
	ags/vst3-capi/host/updatehandler.h \
	ags/vst3-capi/host/componenthandler.cpp \
	ags/vst3-capi/host/hostcontext.cpp \
	ags/vst3-capi/host/updatehandler.cpp \
	ags/vst3-capi/pluginterfaces/gui/ags_vst_iplug_view.cpp \
	ags/vst3-capi/pluginterfaces/gui/ags_vst_iplug_view_content_scale_support.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iattributes.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iaudio_processor.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iautomation_state.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_ichannel_context_info.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_icontext_menu.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_icomponent.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_ievents.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iplug_view.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iedit_controller.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iinter_app_audio.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_imessage.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_imidi_learn.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_inote_expression.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iparameter_changes.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iphysical_ui.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iparameter_function_name.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iplug_interface_support.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iprefetchable_support.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iprocess_context.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_irepresentation.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_iunits.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_ihost_application.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_speaker.cpp \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_preset_keys.cpp \
	ags/vst3-capi/pluginterfaces/base/ags_vst_icloneable.cpp \
	ags/vst3-capi/pluginterfaces/base/ags_vst_geo_constants.cpp \
	ags/vst3-capi/pluginterfaces/base/ags_vst_ierror_context.cpp \
	ags/vst3-capi/pluginterfaces/base/ags_vst_fvariant.cpp \
	ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.cpp \
	ags/vst3-capi/pluginterfaces/base/ags_vst_ftypes.cpp \
	ags/vst3-capi/pluginterfaces/base/ags_vst_iplugin_base.cpp \
	ags/vst3-capi/pluginterfaces/base/ags_vst_futils.cpp \
	ags/vst3-capi/pluginterfaces/base/ags_vst_ibstream.cpp \
	ags/vst3-capi/pluginterfaces/base/ags_vst_fstrdefs.cpp \
	ags/vst3-capi/pluginterfaces/base/ags_vst_const_string_table.cpp \
	ags/vst3-capi/pluginterfaces/base/ags_vst_ipersistent.cpp \
	ags/vst3-capi/pluginterfaces/base/ags_vst_istring_result.cpp \
	ags/vst3-capi/pluginterfaces/base/ags_vst_iupdate_handler.cpp \
	ags/vst3-capi/pluginterfaces/base/ags_vst_ustring.cpp \
	ags/vst3-capi/pluginterfaces/base/ags_vst_ucolor_spec.cpp \
	ags/vst3-capi/pluginterfaces/base/ags_vst_keycodes.cpp \
	ags/vst3-capi/util/ags_vst_string_util.cpp  \
	ags/vst3-capi/base/source/ags_vst_fbuffer.cpp \
	ags/vst3-capi/base/source/ags_vst_fobject.cpp \
	ags/vst3-capi/base/source/ags_vst_fstring.cpp \
	ags/vst3-capi/public.sdk/source/common/ags_vst_plugin_view.cpp \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_audio_effect.cpp \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_component.cpp \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_component_base.cpp \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_edit_controller.cpp \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_bus.cpp \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_parameters.cpp \
	ags/vst3-capi/public.sdk/source/vst/hosting/ags_vst_event_list.cpp \
	ags/vst3-capi/public.sdk/source/vst/hosting/ags_vst_parameter_changes.cpp \
	ags/vst3-capi/public.sdk/source/vst/hosting/ags_vst_plug_interface_support.cpp \
	ags/vst3-capi/public.sdk/source/vst/hosting/ags_vst_module.cpp

# gsequencer all files
gsequencerheaders = \
	ags/gsequencer_main.h

gsequencer_c_sources = \
	ags/gsequencer_main.c

midi2xml_CPPFLAGS = -std=gnu99 $(CPPFLAGS) 
midi2xml_CFLAGS += $(CFLAGS) -I./ $(LIBASOUND2_CFLAGS) $(LIBDSSIALSACOMPAT_CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(GIO_CFLAGS) $(GSTREAMER_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(LIBSOUP_CFLAGS) $(W32API_CFLAGS)
midi2xml_LDFLAGS += $(LIBASOUND2_LIBS) $(LIBDSSIALSACOMPAT_LIBS) $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(GSTREAMER_LIBS) $(LIBINSTPATCH_LIBS) $(W32API_LIBS)
midi2xml_LDADD = libags_audio.la libags_server.la libags_gui.la libags_thread.la libags.la

if WITH_W32API
else
if WITH_OSXAPI
midi2xml_LDADD += -ldl
midi2xml_CFLAGS += -I/usr/include -I/opt/local/include -I/opt/local/include/dssi
else
endif
endif

midi2xml_SOURCES = ags/midi2xml_main.c

ags-integration-unit-test: 
	$(MAKE) -C $(top_srcdir) -f unit-system-tests.mk ags-check-system-unit-test

ags-parallel-integration-unit-test: 
	$(MAKE) -C $(top_srcdir) -f unit-system-tests.mk ags-parallel-check-system-unit-test

ags-parallel-integration-valgrind-unit-test: 
	$(MAKE) -C $(top_srcdir) -f unit-system-tests.mk ags-parallel-check-system-valgrind-unit-test

ags-integration-functional-test: 
	$(MAKE) -C $(top_srcdir) -f functional-system-tests.mk ags-check-system-functional-test

ags-integration-valgrind-functional-test: 
	$(MAKE) -C $(top_srcdir) -f functional-system-tests.mk ags-check-system-valgrind-functional-test

ags-parallel-integration-functional-test: 
	$(MAKE) -C $(top_srcdir) -f functional-system-tests.mk ags-parallel-check-system-functional-test

ags-parallel-integration-valgrind-functional-test: 
	$(MAKE) -C $(top_srcdir) -f functional-system-tests.mk ags-parallel-check-system-valgrind-functional-test

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool

gsequencer.1: gsequencer.1.xml
	xsltproc --nonet --param make.year.ranges 1 --param make.single.year.ranges 1 --param man.charmap.use.subset 0 -o $(top_builddir) http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl $(top_srcdir)/gsequencer.1.xml

midi2xml.1: midi2xml.1.xml
	xsltproc --nonet --param make.year.ranges 1 --param make.single.year.ranges 1 --param man.charmap.use.subset 0 -o $(top_builddir)/ http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl $(top_srcdir)/midi2xml.1.xml

gsequencer.desktop: gsequencer.desktop.in
	$(AM_V_GEN)sed -e 's,\@datadir\@,$(datadir),g' $(top_srcdir)/gsequencer.desktop.in > $(top_builddir)/gsequencer.desktop

html:
	mkdir -p $(top_builddir)/html/
	mkdir -p $(top_builddir)/html/developer-docs
	mkdir -p $(top_builddir)/html/user-docs
	mkdir -p $(top_builddir)/html/osc-docs
	xsltproc --output $(top_builddir)/html/user-docs/ --xinclude --stringparam html.stylesheet "custom.css" --stringparam htmlhelp.show.menu "1" --stringparam suppress.navigation "0" $(HTMLHELP_XSL) $(top_builddir)/docs/usersBook.xml
	xsltproc --output $(top_builddir)/html/developer-docs/ --xinclude --stringparam html.stylesheet "custom.css" --stringparam htmlhelp.show.menu "1" --stringparam suppress.navigation "0" $(HTMLHELP_XSL) $(top_builddir)/docs/developersBook.xml
	xsltproc --output $(top_builddir)/html/osc-docs/ --xinclude --stringparam html.stylesheet "custom.css" --stringparam htmlhelp.show.menu "1" --stringparam suppress.navigation "0" $(HTMLHELP_XSL) $(top_builddir)/docs/oscBook.xml
	mkdir -p $(top_builddir)/docs/reference/libags/libags-html
	mkdir -p $(top_builddir)/docs/reference/libags-audio/libags-audio-html
	mkdir -p $(top_builddir)/docs/reference/libags-gui/libags-gui-html
	mkdir -p $(top_builddir)/docs/reference/libgsequencer/libgsequencer-html
	$(MAKE) -C $(top_builddir)/docs/reference/libags
	cd $(top_builddir)
	$(MAKE) -C $(top_builddir)/docs/reference/libags-audio
	cd $(top_builddir)
	$(MAKE) -C $(top_builddir)/docs/reference/libags-gui
	cd $(top_builddir)
	$(MAKE) -C $(top_builddir)/docs/reference/libgsequencer
	cd $(top_builddir)

pdf:
	mkdir -p $(top_builddir)/pdf/
	dblatex --pdf $(top_builddir)/docs/usersBook.xml -o $(top_builddir)/pdf/user-manual.pdf
	dblatex --pdf $(top_builddir)/docs/developersBook.xml -o $(top_builddir)/pdf/developer-manual.pdf
	dblatex --pdf $(top_builddir)/docs/oscBook.xml -o $(top_builddir)/pdf/osc-manual.pdf

sphinx:
	mkdir -p $(top_builddir)/xml/
	mkdir -p $(top_builddir)/sphinx/
	xmllint --xinclude $(top_builddir)/docs/usersBook.xml --output $(top_builddir)/xml/usersBook.xml
	xmllint --xinclude $(top_builddir)/docs/developersBook.xml --output $(top_builddir)/xml/developersBook.xml
	xmllint --xinclude $(top_builddir)/docs/oscBook.xml --output $(top_builddir)/xml/oscBook.xml
	pandoc -f docbook $(top_builddir)/xml/usersBook.xml -o $(top_builddir)/sphinx/usersBook.rst
	pandoc -f docbook $(top_builddir)/xml/developersBook.xml -o $(top_builddir)/sphinx/developersBook.rst
	pandoc -f docbook $(top_builddir)/xml/oscBook.xml -o $(top_builddir)/sphinx/oscBook.rst

install-changelog:
	mkdir -p $(DESTDIR)$(docdir)/
	install -c -p -m 644 $(top_srcdir)/ChangeLog $(DESTDIR)$(docdir)/changelog

install-compress-changelog:
	mkdir -p $(DESTDIR)$(docdir)/
	gzip -9 -c $(top_srcdir)/ChangeLog > $(DESTDIR)$(docdir)/changelog.gz

install-data-local:
	mkdir -p $(DESTDIR)$(includedir)/ags
	install -c -p -m 644 $(top_builddir)/ags/ags_api_config.h $(DESTDIR)$(includedir)/ags/ags_api_config.h
	mkdir -p $(DESTDIR)$(datadir)/gsequencer/icons
	mkdir -p $(DESTDIR)$(datadir)/icons/hicolor/32x32/apps
	mkdir -p $(DESTDIR)$(datadir)/icons/hicolor/48x48/apps
	mkdir -p $(DESTDIR)$(datadir)/icons/hicolor/64x64/apps
	mkdir -p $(DESTDIR)$(datadir)/icons/hicolor/128x128/apps
	mkdir -p $(DESTDIR)$(datadir)/gsequencer/images
	mkdir -p $(DESTDIR)$(datadir)/gsequencer/styles
	mkdir -p $(DESTDIR)$(datadir)/xml/gsequencer/schema/dtd/$(PACKAGE_VERSION)
	mkdir -p $(DESTDIR)$(datadir)/xml/gsequencer/stylesheet/ags-xsl/midi-xml
	mkdir -p $(DESTDIR)$(datadir)/mime/packages
	mkdir -p $(DESTDIR)$(datadir)/metainfo
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/icons/ags.xpm $(DESTDIR)$(datadir)/gsequencer/icons/ags.xpm
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/icons/jumper.png $(DESTDIR)$(datadir)/gsequencer/icons/jumper.png
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/icons/ags.png $(DESTDIR)$(datadir)/gsequencer/icons/ags.png
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/icons/gsequencer-32x32.png $(DESTDIR)$(datadir)/icons/hicolor/32x32/apps/gsequencer.png
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/icons/gsequencer-48x48.png $(DESTDIR)$(datadir)/icons/hicolor/48x48/apps/gsequencer.png
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/icons/gsequencer-64x64.png $(DESTDIR)$(datadir)/icons/hicolor/64x64/apps/gsequencer.png
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/icons/gsequencer-128x128.png $(DESTDIR)$(datadir)/icons/hicolor/128x128/apps/gsequencer.png
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/images/ags.png $(DESTDIR)$(datadir)/gsequencer/images/ags.png
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/images/gsequencer-800x450.png $(DESTDIR)$(datadir)/gsequencer/images/gsequencer-800x450.png
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/styles/ags.css $(DESTDIR)$(datadir)/gsequencer/styles/ags.css
	install -c -p -m 644 $(top_srcdir)/ags/X/file/ags_simple_file.dtd $(DESTDIR)$(datadir)/xml/gsequencer/schema/dtd/$(PACKAGE_VERSION)/
	install -c -p -m 644 $(top_srcdir)/ags/audio/midi/ags_midi_file.dtd $(DESTDIR)$(datadir)/xml/gsequencer/schema/dtd/$(PACKAGE_VERSION)/
	install -c -p -m 644 $(top_srcdir)/ags/audio/osc/ags_osc_file.dtd $(DESTDIR)$(datadir)/xml/gsequencer/schema/dtd/$(PACKAGE_VERSION)/
	install -c -p -m 644 $(top_srcdir)/ags/file/ags_file.dtd $(DESTDIR)$(datadir)/xml/gsequencer/schema/dtd/$(PACKAGE_VERSION)/
	install -c -p -m 644 $(top_srcdir)/ags.xsl $(DESTDIR)$(datadir)/xml/gsequencer/stylesheet/ags-xsl/midi-xml/
	install -c -p -m 644 $(top_srcdir)/ags-simple.xsl $(DESTDIR)$(datadir)/xml/gsequencer/stylesheet/ags-xsl/midi-xml/
	install -c -p -m 644 $(top_srcdir)/application-x-gsequencer.xml $(DESTDIR)$(datadir)/mime/packages/gsequencer.xml
	install -c -p -m 644 $(top_srcdir)/org.nongnu.gsequencer.gsequencer.appdata.xml $(DESTDIR)$(datadir)/metainfo/

fix-local-html:
	find $(top_builddir)/docs/reference/libags/libags-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../gobject@$(GOBJECT_API_DOC)@g' {} \;
	find $(top_builddir)/docs/reference/libags-audio/libags-audio-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../gobject@$(GOBJECT_API_DOC)@g' {} \;
	find $(top_builddir)/docs/reference/libags-audio/libags-audio-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../libags-html@$(LIBAGS_API_DOC)@g' {} \;
	find $(top_builddir)/docs/reference/libags-gui/libags-gui-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../gobject@$(GOBJECT_API_DOC)@g' {} \;
	find $(top_builddir)/docs/reference/libags-gui/libags-gui-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../gtk3@$(GTK_API_DOC)@g' {} \;
	find $(top_builddir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../gobject@$(GOBJECT_API_DOC)@g' {} \;
	find $(top_builddir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../gtk3@$(GTK_API_DOC)@g' {} \;
	find $(top_builddir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../libags-html@$(LIBAGS_API_DOC)@g' {} \;
	find $(top_builddir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../libags-audio-html@$(LIBAGS_AUDIO_API_DOC)@g' {} \;
	find $(top_builddir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../libags-gui-html@$(LIBAGS_GUI_API_DOC)@g' {} \;

fix-local-html-bsd:
	find $(top_builddir)/docs/reference/libags/libags-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i '' -e 's@../gobject@$(GOBJECT_API_DOC)@g' {} \;
	find $(top_builddir)/docs/reference/libags-audio/libags-audio-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i '' -e 's@../gobject@$(GOBJECT_API_DOC)@g' {} \;
	find $(top_builddir)/docs/reference/libags-audio/libags-audio-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i '' -e 's@../libags-html@$(LIBAGS_API_DOC)@g' {} \;
	find $(top_builddir)/docs/reference/libags-gui/libags-gui-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i '' -e 's@../gobject@$(GOBJECT_API_DOC)@g' {} \;
	find $(top_builddir)/docs/reference/libags-gui/libags-gui-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i '' -e 's@../gtk3@$(GTK_API_DOC)@g' {} \;
	find $(top_builddir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i '' -e 's@../gobject@$(GOBJECT_API_DOC)@g' {} \;
	find $(top_builddir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i '' -e 's@../gtk3@$(GTK_API_DOC)@g' {} \;
	find $(top_builddir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i '' -e 's@../libags-html@$(LIBAGS_API_DOC)@g' {} \;
	find $(top_builddir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i '' -e 's@../libags-audio-html@$(LIBAGS_AUDIO_API_DOC)@g' {} \;
	find $(top_builddir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i '' -e 's@../libags-gui-html@$(LIBAGS_GUI_API_DOC)@g' {} \;

fix-online-books-html:
	find $(top_builddir)/html/ -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's/\/usr\/share\/icons\/Adwaita\/32x32\/actions/..\/images/g' {} \;
	find $(top_builddir)/html/ -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's/\/usr\/share\/icons\/Adwaita\/32x32\/apps/..\/images/g' {} \;

fix-online-html:
	find $(top_builddir)/docs/reference/libags/libags-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../gobject@https://developer.gnome.org/gobject/stable@g' {} \;
	find $(top_builddir)/docs/reference/libags-audio/libags-audio-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../gobject@https://developer.gnome.org/gobject/stable@g' {} \;
	find $(top_builddir)/docs/reference/libags-audio/libags-audio-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../libags-html@../libags@g' {} \;
	find $(top_builddir)/docs/reference/libags-gui/libags-gui-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../gobject@https://developer.gnome.org/gobject/stable@g' {} \;
	find $(top_builddir)/docs/reference/libags-gui/libags-gui-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../gtk3@https://developer.gnome.org/gtk3/stable@g' {} \;
	find $(top_builddir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../gobject@https://developer.gnome.org/gobject/stable@g' {} \;
	find $(top_builddir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../gtk3@https://developer.gnome.org/gtk3/stable@g' {} \;
	find $(top_builddir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../libags-html@../libags@g' {} \;
	find $(top_builddir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../libags-audio-html@../libags-audio@g' {} \;
	find $(top_builddir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec $(AM_V_GEN)sed -i 's@../libags-gui-html@../libags-gui@g' {} \;

install-html-mkdir-links:
	echo "creating upstream API reference manual directory [$(AGS_WITH_UPSTREAM_GTK_DOC)]"
if AGS_WITH_UPSTREAM_GTK_DOC
if ENABLE_GTK_DOC
	mkdir -p $(DESTDIR)$(LIBAGS_API_DIR)
	mkdir -p $(DESTDIR)$(LIBAGS_AUDIO_API_DIR)
	mkdir -p $(DESTDIR)$(LIBAGS_GUI_API_DIR)
	mkdir -p $(DESTDIR)$(LIBGSEQUENCER_API_DIR)
endif
endif

install-html-mkdir:
if AGS_WITH_SINGLE_DOCDIR
	mkdir -p $(DESTDIR)$(docdir)/images
	mkdir -p $(DESTDIR)$(docdir)/images/adwaita
	mkdir -p $(DESTDIR)$(docdir)/user-manual
	mkdir -p $(DESTDIR)$(docdir)/developer-manual
	mkdir -p $(DESTDIR)$(docdir)/osc-manual
else
	mkdir -p $(DESTDIR)$(docdir)/images
	mkdir -p $(DESTDIR)$(docdir)/images/adwaita
	mkdir -p $(DESTDIR)$(docdir)/html
	mkdir -p $(DESTDIR)$(datadir)/doc/libags-audio-doc/images
	mkdir -p $(DESTDIR)$(datadir)/doc/libags-audio-doc/html
	mkdir -p $(DESTDIR)$(datadir)/doc/libags-audio-doc/osc-server
if ENABLE_GTK_DOC
	mkdir -p $(DESTDIR)$(datadir)/gtk-doc/html
endif
endif
if AGS_WITH_UPSTREAM_GTK_DOC
else
if ENABLE_GTK_DOC
	mkdir -p $(DESTDIR)$(LIBAGS_API_DIR)
	mkdir -p $(DESTDIR)$(LIBAGS_AUDIO_API_DIR)
	mkdir -p $(DESTDIR)$(LIBAGS_GUI_API_DIR)
	mkdir -p $(DESTDIR)$(LIBGSEQUENCER_API_DIR)
endif
endif

install-html-links: install-html-mkdir-links
	echo "creating links [$(AGS_WITH_UPSTREAM_GTK_DOC)]"
if AGS_WITH_SINGLE_DOCDIR
else
if AGS_WITH_UPSTREAM_GTK_DOC
if ENABLE_GTK_DOC
	ln -s ../../doc/libags-doc/api $(DESTDIR)$(datadir)/gtk-doc/html/libags
	ln -s ../../doc/libags-audio-doc/api $(DESTDIR)$(datadir)/gtk-doc/html/libags_audio
	ln -s ../../doc/libags-gui-doc/api $(DESTDIR)$(datadir)/gtk-doc/html/libags_gui
	ln -s ../../doc/libgsequencer-doc/api $(DESTDIR)$(datadir)/gtk-doc/html/libgsequencer
endif
endif
endif

install-html: install-html-mkdir install-html-links
if AGS_WITH_SINGLE_DOCDIR
	install -c -p -m 644 $(userdocimages) $(DESTDIR)$(docdir)/images/
	install -c -p -m 644 $(userdocadwaitaimages) $(DESTDIR)$(docdir)/images/adwaita/
	install -c -p -m 644 $(devdocimages) $(DESTDIR)$(docdir)/images/
	install -c -p -m 644 $(top_builddir)/html/user-docs/* $(DESTDIR)$(docdir)/user-manual/
	install -c -p -m 644 $(top_builddir)/html/developer-docs/* $(DESTDIR)$(docdir)/developer-manual/
	install -c -p -m 644 $(top_builddir)/html/osc-docs/* $(DESTDIR)$(docdir)/osc-manual/
	install -c -p -m 644 $(top_builddir)/docs/custom.css $(DESTDIR)$(docdir)/developer-manual/
	install -c -p -m 644 $(top_builddir)/docs/custom.css $(DESTDIR)$(docdir)/user-manual/
	install -c -p -m 644 $(top_builddir)/docs/custom.css $(DESTDIR)$(docdir)/osc-manual/
else
	install -c -p -m 644 $(userdocimages) $(DESTDIR)$(docdir)/images/
	install -c -p -m 644 $(userdocadwaitaimages) $(DESTDIR)$(docdir)/images/adwaita/
	install -c -p -m 644 $(devdocimages) $(DESTDIR)$(datadir)/doc/libags-audio-doc/images/
	install -c -p -m 644 $(top_builddir)/html/user-docs/* $(DESTDIR)$(docdir)/html/
	install -c -p -m 644 $(top_builddir)/html/developer-docs/* $(DESTDIR)$(datadir)/doc/libags-audio-doc/html/
	install -c -p -m 644 $(top_builddir)/html/osc-docs/* $(DESTDIR)$(datadir)/doc/libags-audio-doc/osc-server/
	install -c -p -m 644 $(top_builddir)/docs/custom.css $(DESTDIR)$(docdir)/html/
	install -c -p -m 644 $(top_builddir)/docs/custom.css $(DESTDIR)$(datadir)/doc/libags-audio-doc/html/
	install -c -p -m 644 $(top_builddir)/docs/custom.css $(DESTDIR)$(datadir)/doc/libags-audio-doc/osc-server/
endif
if ENABLE_GTK_DOC
	install -c -p -m 644 $(top_builddir)/docs/reference/libags/html/* $(DESTDIR)$(LIBAGS_API_DIR)
	install -c -p -m 644 $(top_builddir)/docs/reference/libags-audio/html/* $(DESTDIR)$(LIBAGS_AUDIO_API_DIR)
	install -c -p -m 644 $(top_builddir)/docs/reference/libags-gui/html/* $(DESTDIR)$(LIBAGS_GUI_API_DIR)
	install -c -p -m 644 $(top_builddir)/docs/reference/libgsequencer/html/* $(DESTDIR)$(LIBGSEQUENCER_API_DIR)
endif

install-pdf-mkdir:
if AGS_WITH_SINGLE_DOCDIR
	mkdir -p $(DESTDIR)$(docdir)/pdf
else
	mkdir -p $(DESTDIR)$(docdir)/pdf
	mkdir -p $(DESTDIR)$(datadir)/doc/libags-audio-doc/pdf
endif

install-pdf: install-pdf-mkdir
if AGS_WITH_SINGLE_DOCDIR
	install -c -p -m 644 $(top_builddir)/pdf/user-manual.pdf $(DESTDIR)$(docdir)/pdf/
	install -c -p -m 644 $(top_builddir)/pdf/developer-manual.pdf $(DESTDIR)$(docdir)/pdf/
	install -c -p -m 644 $(top_builddir)/pdf/osc-manual.pdf $(DESTDIR)$(docdir)/pdf/
else
	install -c -p -m 644 $(top_builddir)/pdf/user-manual.pdf $(DESTDIR)$(docdir)/pdf/
	install -c -p -m 644 $(top_builddir)/pdf/developer-manual.pdf $(DESTDIR)$(datadir)/doc/libags-audio-doc/pdf/
	install -c -p -m 644 $(top_builddir)/pdf/osc-manual.pdf $(DESTDIR)$(datadir)/doc/libags-audio-doc/pdf/
endif

uninstall-hook:
	rm -rf $(DESTDIR)$(includedir)/ags/ags_api_config.h
	rm -rf $(DESTDIR)$(datadir)/gsequencer
	rm -rf $(DESTDIR)$(docdir)
	rm -rf $(DESTDIR)$(datadir)/xml/gsequencer
if AGS_WITH_SINGLE_DOCDIR
else
	rm -rf $(DESTDIR)$(datadir)/doc/libags-doc
	rm -rf $(DESTDIR)$(datadir)/doc/libags-audio-doc
	rm -rf $(DESTDIR)$(datadir)/doc/libags-gui-doc
	rm -rf $(DESTDIR)$(datadir)/doc/libgsequencer-doc
	rm -rf $(DESTDIR)$(datadir)/gtk-doc/html/libags
	rm -rf $(DESTDIR)$(datadir)/gtk-doc/html/libags_audio
	rm -rf $(DESTDIR)$(datadir)/gtk-doc/html/libags_gui
	rm -rf $(DESTDIR)$(datadir)/gtk-doc/html/libgsequencer
endif
	rm -f $(DESTDIR)$(datadir)/icons/hicolor/32x32/apps/gsequencer.png
	rm -f $(DESTDIR)$(datadir)/icons/hicolor/48x48/apps/gsequencer.png
	rm -f $(DESTDIR)$(datadir)/icons/hicolor/64x64/apps/gsequencer.png
	rm -f $(DESTDIR)$(datadir)/icons/hicolor/128x128/apps/gsequencer.png
	rm -f $(DESTDIR)$(datadir)/mime/packages/gsequencer.xml
	rm -f $(DESTDIR)$(datadir)/metainfo/org.nongnu.gsequencer.gsequencer.appdata.xml
	rm -f $(DESTDIR)$(pkgconfigdir)/libags.pc
	rm -f $(DESTDIR)$(pkgconfigdir)/libags_audio.pc
	rm -f $(DESTDIR)$(pkgconfigdir)/libags_gui.pc
	rm -f $(DESTDIR)$(pkgconfigdir)/libgsequencer.pc

clean-local:
	@if test "$(top_builddir)" != "$(top_srcdir)" ; then rm -f "$(top_builddir)"/docs/usersBook.xml; fi
	@if test "$(top_builddir)" != "$(top_srcdir)" ; then rm -f "$(top_builddir)"/docs/developersBook.xml; fi
	@if test "$(top_builddir)" != "$(top_srcdir)" ; then rm -f "$(top_builddir)"/docs/oscBook.xml; fi
	@if test "$(top_builddir)" != "$(top_srcdir)" ; then rm -f "$(top_builddir)"/docs/appa.xml; fi
	@if test "$(top_builddir)" != "$(top_srcdir)" ; then rm -f "$(top_builddir)"/docs/reference/libags/libags.xml; fi
	@if test "$(top_builddir)" != "$(top_srcdir)" ; then rm -f "$(top_builddir)"/docs/reference/libags-audio/libags_audio.xml; fi
	@if test "$(top_builddir)" != "$(top_srcdir)" ; then rm -f "$(top_builddir)"/docs/reference/libags-gui/libags_gui.xml; fi
	@if test "$(top_builddir)" != "$(top_srcdir)" ; then rm -f "$(top_builddir)"/docs/reference/libgsequencer/libgsequencer.xml; fi
	@if test "$(top_builddir)" != "$(top_srcdir)" ; then rm -f "$(top_builddir)"/po/POTFILES.in; fi
	rm -rf docs/reference/libags/xml
	rm -rf docs/reference/libags/libags-html
	rm -rf docs/reference/libags/tmpl
	rm -rf docs/reference/libags-audio/xml
	rm -rf docs/reference/libags-audio/libags-audio-html
	rm -rf docs/reference/libags-audio/tmpl
	rm -rf docs/reference/libags-gui/xml
	rm -rf docs/reference/libags-gui/libags-gui-html
	rm -rf docs/reference/libags-gui/tmpl
	rm -rf docs/reference/libgsequencer/xml
	rm -rf docs/reference/libgsequencer/libgsequencer-html
	rm -rf docs/reference/libgsequencer/tmpl
	rm -rf html
	rm -rf pdf
	rm -rf xml
	rm -rf sphinx
	rm -f docs/reference/libags/Makefile
	rm -f docs/reference/libags-audio/Makefile
	rm -f docs/reference/libags-gui/Makefile
	rm -f docs/reference/libgsequencer/Makefile
	rm -f unit-system-tests.mk
	rm -f functional-system-tests.mk
	rm -f docs/reference/libags-audio/libags_audio-decl-list.txt
	rm -f docs/reference/libags-audio/libags_audio-undeclared.txt
	rm -f docs/reference/libags-audio/libags_audio-undocumented.txt
	rm -f docs/reference/libags-audio/libags_audio-unused.txt
	rm -f docs/reference/libags-audio/libags_audio.args
	rm -f docs/reference/libags-audio/libags_audio.hierarchy
	rm -f docs/reference/libags-audio/libags_audio.prerequisites
	rm -f docs/reference/libags-audio/libags_audio.signals
	rm -f docs/reference/libags-gui/libags_gui-decl-list.txt
	rm -f docs/reference/libags-gui/libags_gui-undeclared.txt
	rm -f docs/reference/libags-gui/libags_gui-undocumented.txt
	rm -f docs/reference/libags-gui/libags_gui-unused.txt
	rm -f docs/reference/libags-gui/libags_gui.args
	rm -f docs/reference/libags-gui/libags_gui.hierarchy
	rm -f docs/reference/libags-gui/libags_gui.prerequisites
	rm -f docs/reference/libags-gui/libags_gui.signals
	rm -f docs/reference/libags/libags-decl-list.txt
	rm -f docs/reference/libags/libags-undeclared.txt
	rm -f docs/reference/libags/libags-undocumented.txt
	rm -f docs/reference/libags/libags-unused.txt
	rm -f docs/reference/libags/libags.args
	rm -f docs/reference/libags/libags.hierarchy
	rm -f docs/reference/libags/libags.prerequisites
	rm -f docs/reference/libags/libags.signals
	rm -f docs/reference/libgsequencer/libgsequencer-decl-list.txt
	rm -f docs/reference/libgsequencer/libgsequencer-undeclared.txt
	rm -f docs/reference/libgsequencer/libgsequencer-undocumented.txt
	rm -f docs/reference/libgsequencer/libgsequencer-unused.txt
	rm -f docs/reference/libgsequencer/libgsequencer.args
	rm -f docs/reference/libgsequencer/libgsequencer.hierarchy
	rm -f docs/reference/libgsequencer/libgsequencer.prerequisites
	rm -f docs/reference/libgsequencer/libgsequencer.signals

gen-symbols-ags:
	rm -f libags.sym.in
	touch libags.sym.in
	cat license-notice-gnu-gpl-3-0+-sym.txt >> libags.sym.in
	echo -e -n "\n" >> libags.sym.in
	find ags/util -name "*.h" | xargs grep --no-filename -o -P "(?<=\s)(ags_[a-z0-9_]+)(?=[\s]*\()" >> libags.sym.in
	find ags/lib -name "*.h" | xargs grep --no-filename -o -P "(?<=\s)(ags_[a-z0-9_]+)(?=[\s]*\()" >> libags.sym.in
	echo "ags_application_context" >> libags.sym.in
	find ags/object -name "*.h" | xargs grep --no-filename -o -P "(?<=\s)(ags_[a-z0-9_]+)(?=[\s]*\()" >> libags.sym.in
	find ags/object -name "ags_marshal.h" | xargs grep --no-filename -o -P "(ags_cclosure_marshal_[a-zA-Z0-9_]+)" >> libags.sym.in
	find ags/file -name "*.h" | xargs grep --no-filename -o -P "(?<=\s)(ags_[a-z0-9_]+)(?=[\s]*\()" >> libags.sym.in

gen-symbols-ags-thread:
	rm -f libags_thread.sym.in
	touch libags_thread.sym.in
	cat license-notice-gnu-gpl-3-0+-sym.txt >> libags_thread.sym.in
	echo -e -n "\n" >> libags_thread.sym.in
	find ags/thread -name "*.h" | xargs grep --no-filename -o -P "(?<=\s)(ags_[a-z0-9_]+)(?=[\s]*\()" >> libags_thread.sym.in

gen-symbols-ags-server:
	rm -f libags_server.sym.in
	touch libags_server.sym.in
	cat license-notice-gnu-agpl-3-0+-sym.txt >> libags_server.sym.in
	echo -e -n "\n" >> libags_server.sym.in
	find ags/server -name "*.h" | xargs grep --no-filename -o -P "(?<=\s)(ags_[a-z0-9_]+)(?=[\s]*\()" >> libags_server.sym.in

gen-symbols-ags-audio:
	rm -f libags_audio.sym.in
	touch libags_audio.sym.in
	cat license-notice-gnu-gpl-3-0+-sym.txt >> libags_audio.sym.in
	echo -e -n "\n" >> libags_audio.sym.in
	find ags/plugin -name "*.h" | xargs grep --no-filename -o -P "(?<=\s)(ags_[a-z0-9_]+)(?=[\s]*\()" >> libags_audio.sym.in
	find ags/audio -name "*.h" | xargs grep --no-filename -o -P "(?<=\s)(ags_[a-z0-9_]+)(?=[\s]*\()" >> libags_audio.sym.in

gen-symbols-ags-vst:
	rm -f libags_vst.sym.in
	touch libags_vst.sym.in
	cat license-notice-gnu-gpl-3-0+-sym.txt >> libags_vst.sym.in
	echo -e -n "\n" >> libags_vst.sym.in
	find ags/vst3-capi -name "*.h" | xargs grep --no-filename -o -P "(?<=\s)(ags_[a-z0-9_]+)(?=[\s]*\()" >> libags_vst.sym.in

gen-symbols-ags-gui:
	rm -f libags_gui.sym.in
	touch libags_gui.sym.in
	cat license-notice-gnu-gpl-3-0+-sym.txt >> libags_gui.sym.in
	echo -e -n "\n" >> libags_gui.sym.in
	find ags/widget -name "*.h" | xargs grep --no-filename -o -P "(?<=\s)(ags_[a-z0-9_]+)(?=[\s]*\()" >> libags_gui.sym.in
	find ags/widget -name "ags_widget_marshal.h" | xargs grep --no-filename -o -P "(ags_widget_cclosure_marshal_[a-zA-Z0-9_]+)" >> libags_gui.sym.in

gen-symbols-all: gen-symbols-ags gen-symbols-ags-thread gen-symbols-ags-server gen-symbols-ags-audio gen-symbols-ags-vst gen-symbols-ags-gui
	echo "Generated all symbols"

gen-marshallers:
	cat ags_license_header > ags/object/ags_marshal.h
	glib-genmarshal --prefix=ags_cclosure_marshal --header ags/object/ags_marshallers.list >> ags/object/ags_marshal.h
	cat ags_license_header > ags/object/ags_marshal.c
	glib-genmarshal --prefix=ags_cclosure_marshal --body ags/object/ags_marshallers.list >> ags/object/ags_marshal.c
	echo "generated marshallers in ags/object/"
	cat ags_license_header > ags/widget/ags_widget_marshal.h
	glib-genmarshal --prefix=ags_widget_cclosure_marshal --header ags/widget/ags_widget_marshallers.list >> ags/widget/ags_widget_marshal.h
	cat ags_license_header > ags/widget/ags_widget_marshal.c
	glib-genmarshal --prefix=ags_widget_cclosure_marshal --body ags/widget/ags_widget_marshallers.list >> ags/widget/ags_widget_marshal.c
	echo "generated marshallers in ags/widget/"

check-gettext:
	@if test x$(USE_NLS) != "xyes" ; then echo "Missing gettext. Rerun configure and check for" \
	"'checking whether to use NLS... yes'!" ; exit 1 ; fi

update-po: check-gettext
	cd po && $(MAKE) $(AM_MAKEFLAGS) POTFILES gsequencer.pot update-po

update-gmo: check-gettext
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-gmo

force-update-gmo: check-gettext
	touch po/*.po
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-gmo

force-update-gmo-%: check-gettext
	@language=`echo $@ | sed s/force-update-gmo-//` ; \
	if test ! -f po/$$language.po ; then echo "file po/$$language.po does not exist" ; exit 1 ; fi ; \
	touch po/$$language.po ; \
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-gmo

.PHONY: check-gettext update-po update-gmo force-update-gmo
