#Copyright (C) 2005-2019 Jo\u00EBl Kr\u00E4hemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

AUTOMAKE_OPTIONS = foreign
SUBDIRS = po

# if WITH_GNULIB
# SUBDIRS += lib
# endif

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -std=gnu99 -include errno.h

if WITH_GNULIB
AM_CPPFLAGS += -I$(top_srcdir)/lib
endif

if WITH_W32API
AM_CPPFLAGS += -DAGS_W32API=1 -DHAVE_GLIB_2_6=1 -DHAVE_GLIB_2_44=1 -DHAVE_GLIB_2_54=1 -DAGS_LIBRARY_SUFFIX=\".dll\" -DAGS_VECTORIZED_BUILTIN_FUNCTIONS=1 -include $(top_srcdir)/ags/config.h -I$(top_srcdir) -I/usr/lib/gcc/x86_64-w64-mingw32/8.3-win32/include-fixed -I/usr/lib/gcc/x86_64-w64-mingw32/8.3-win32/include  -I/usr/x86_64-w64-mingw32/include -I/home/joelkraehemann/mingw64/target/include -I/home/joelkraehemann/mingw64/target/include/dssi -Werror=return-type -Werror=uninitialized -Werror=maybe-uninitialized
AM_LDFLAGS = -no-undefined
else
if WITH_OSXAPI
AM_CPPFLAGS += -fnested-functions -DAGS_OSXAPI=1 -I/usr/include -I/opt/local/include -I/opt/local/include/dssi -DAGS_LIBRARY_SUFFIX=\".dylib\"
else
AM_CPPFLAGS += -DAGS_VECTORIZED_BUILTIN_FUNCTIONS=1 -I/usr/include -DAGS_LIBRARY_SUFFIX=\".so\" -Werror=return-type -Werror=uninitialized -Werror=maybe-uninitialized
endif
endif


AM_CPPFLAGS += -DSRCDIR=\"$(srcdir)\" -DDESTDIR=\"$(DESTDIR)$(datadir)\" -DPACKAGE_VERSION=\"$(PACKAGE_VERSION)\" -DAGS_REDUCE_RT_EVENTS=1 -D_FORTIFY_SOURCE=2 -Wall -Wformat -Werror=format-security

if WITH_LIBINSTPATCH
AM_CPPFLAGS += -DAGS_WITH_LIBINSTPATCH=1
endif

# AM_CPPFLAGS += -march=native -O3 -ftree-vectorize -ftree-slp-vectorize -ffast-math -ftree-vectorizer-verbose=2

# what flags you want to pass to the C compiler & linker
CFLAGS =  @CFLAGS@
LDFLAGS = @LDFLAGS@ $(LTLIBINTL)
LIBTOOL_DEPS = @LIBTOOL_DEPS@

gsequencer_CFLAGS = @GSEQUENCER_CFLAGS@
gsequencer_LDFLAGS = @GSEQUENCER_LDFLAGS@

midi2xml_CFLAGS = @GSEQUENCER_CFLAGS@
midi2xml_LDFLAGS = @GSEQUENCER_LDFLAGS@

if WITH_W32API
LDFLAGS += -no-undefined
endif

if WITH_GNULIB
LDFLAGS += -L./lib -lgnu
endif

# 
# TARGETS = gsequencer

localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
lib_LTLIBRARIES = libags.la libags_thread.la libags_server.la libags_audio.la libags_gui.la

if WITH_VST3
lib_LTLIBRARIES += libags_vst.la 
endif

# lib_LTLIBRARIES += libgsequencer.la

bin_PROGRAMS = gsequencer midi2xml

libgsequencerdir = $(libdir)/gsequencer
libgsequencer_LTLIBRARIES = libgsequencer.la

EXTRA_DIST = config.rpath \
	COPYING.server \
	COPYING.docs \
	COPYING.stk-4.3 \
	ags/config.h.in \
	ags/ags_config.h.in \
	gtk-doc.make \
	gsequencer.share \
	acsite.m4 \
	aclocal.m4 \
	m4/gnulib-cache.m4 \
	gsequencer.1 \
	README \
	ags_example.xml \
	midi2xml.1 \
	ags_functional_notation_edit_test.xml \
	ags_functional_audio_config_test.xml \
	mkinstalldirs \
	m4/Makefile.am \
	m4/Makefile.in \
	ags/i18n.h \
	ags/gettext.h \
	ags/lib/AUTHORS \
	ags/object/README \
	ags/server/security/AUTHORS \
	ags/audio/AUTHORS \
	ags/audio/recall/AUTHORS \
	ags/widget/README \
	docs/images/README \
	docs/listings/README \
	docs/reference/libags-audio/README \
	docs/reference/libags-gui/README \
	docs/reference/libags/README \
	docs/reference/libgsequencer/README \
	gsequencer.share/icons/README \
	gsequencer.share/images/README \
	gsequencer.share/styles/README \
	m4/README \
	po/README

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
	libags_gui.sym.in

EXTRA_DIST += \
	unit-system-tests.mk.am \
	unit-system-tests.mk.in \
	functional-system-tests.mk.am \
	functional-system-tests.mk.in

EXTRA_DIST += \
	docs/reference/libags/libags-sections.txt.in \
	docs/reference/libags/libags.types.in \
	docs/reference/libags/libags.interfaces.in \
	docs/reference/libags-audio/libags_audio-sections.txt.in \
	docs/reference/libags-audio/libags_audio.types.in \
	docs/reference/libags-audio/libags_audio.interfaces.in \
	docs/reference/libags-gui/libags_gui-sections.txt.in \
	docs/reference/libags-gui/libags_gui.types.in \
	docs/reference/libags-gui/libags_gui.interfaces.in \
	docs/reference/libgsequencer/libgsequencer-sections.txt.in \
	docs/reference/libgsequencer/libgsequencer.types.in \
	docs/reference/libgsequencer/libgsequencer.interfaces.in \
	clean-gtk-doc.sh

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
	libags.sym \
	libags_thread.sym \
	libags_server.sym \
	libags_audio.sym \
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
	docs/listings/linking_prerequisites.c \
	docs/listings/linking_safe.c \
	docs/listings/linking_unsafe.c \
	docs/listings/notation_clipboard.c \
	docs/listings/pcm_info.c \
	docs/listings/port.c \
	docs/listings/pull_thread.c \
	docs/listings/recycling.c \
	docs/listings/start_thread.c \
	docs/listings/thread_application_context.c \
	docs/listings/thread_obj_mutex.c \
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

# pkg-config
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libags.pc libags_audio.pc libags_gui.pc
# pkgconfig_DATA += libgsequencer.pc

EXTRA_DIST += libags.pc.in libags_audio.pc.in libags_gui.pc.in libgsequencer.pc.in
# EXTRA_DIST += libgsequencer.pc.in

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
	libags_gui.sym

EXTRA_DIST += docs/reference/libags/git.mk docs/reference/libags/gtk-doc.make docs/reference/libags/Makefile.am docs/reference/libags/Makefile.in
EXTRA_DIST += docs/reference/libags/libags-overrides.txt docs/reference/libags/libags-sections.txt docs/reference/libags/libags.types docs/reference/libags/libags.xml

EXTRA_DIST += docs/reference/libags-audio/git.mk docs/reference/libags-audio/gtk-doc.make docs/reference/libags-audio/Makefile.am docs/reference/libags-audio/Makefile.in
EXTRA_DIST += docs/reference/libags-audio/libags_audio-overrides.txt  docs/reference/libags-audio/libags_audio-sections.txt docs/reference/libags-audio/libags_audio.types docs/reference/libags-audio/libags_audio.xml

EXTRA_DIST += docs/reference/libags-gui/git.mk docs/reference/libags-gui/gtk-doc.make docs/reference/libags-gui/Makefile.am docs/reference/libags-gui/Makefile.in
EXTRA_DIST += docs/reference/libags-gui/libags_gui-overrides.txt docs/reference/libags-gui/libags_gui-sections.txt docs/reference/libags-gui/libags_gui.types docs/reference/libags-gui/libags_gui.xml

EXTRA_DIST += docs/reference/libgsequencer/git.mk docs/reference/libgsequencer/gtk-doc.make docs/reference/libgsequencer/Makefile.am docs/reference/libgsequencer/Makefile.in
EXTRA_DIST += docs/reference/libgsequencer/libgsequencer-overrides.txt docs/reference/libgsequencer/libgsequencer.xml

# EXTRA_DIST += docs/reference/libgsequencer

# include
otherincludedir = $(includedir)/ags
nobase_include_HEADERS = $(libags_la_HEADERS_0) $(libags_thread_la_HEADERS_0) $(libags_server_la_HEADERS_0) $(libags_audio_la_HEADERS_0) $(libags_gui_la_HEADERS_0)
# nobase_include_HEADERS += $(libgsequencer_la_HEADERS_0)

# doc
# docdir = $(datadir)/doc/gsequencer
doc_DATA = README ags_example.xml

# documentation
manpages = \
	gsequencer.1.xml \
	midi2xml.1.xml

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
	docs/listings/linking_prerequisites.c.in \
	docs/listings/linking_safe.c.in \
	docs/listings/linking_unsafe.c.in \
	docs/listings/notation_clipboard.c.in \
	docs/listings/pcm_info.c.in \
	docs/listings/pull_thread.c.in \
	docs/listings/port.c.in \
	docs/listings/recycling.c.in \
	docs/listings/start_thread.c.in \
	docs/listings/thread_obj_mutex.c.in \
	docs/listings/thread_application_context.c.in

devdocimages = \
	$(top_srcdir)/docs/images/AGS_iterator.png \
	$(top_srcdir)/docs/images/AGS_recall_lifecycle.png \
	$(top_srcdir)/docs/images/AGS_super-threaded.png \
	$(top_srcdir)/docs/images/AGS_linking.png

userdocimages = \
	$(top_srcdir)/docs/images/ags_audiorec.png \
	$(top_srcdir)/docs/images/ags-automation-toolbar.png \
	$(top_srcdir)/docs/images/ags_automation_window_drum.png \
	$(top_srcdir)/docs/images/ags_equalizer10.png \
	$(top_srcdir)/docs/images/ags_export_window.png \
	$(top_srcdir)/docs/images/ags_machine_properties-input.png \
	$(top_srcdir)/docs/images/ags_machine_properties-resize_channels.png \
	$(top_srcdir)/docs/images/ags_navigation.png \
	$(top_srcdir)/docs/images/ags_audio_preferences.png \
	$(top_srcdir)/docs/images/ags_audio_connection_output_bulk.png \
	$(top_srcdir)/docs/images/ags_audio_connection_output_line.png \
	$(top_srcdir)/docs/images/ags_audio_connection_input_bulk.png \
	$(top_srcdir)/docs/images/ags_audio_connection_input_line.png \
	$(top_srcdir)/docs/images/ags_midi_dialog.png \
	$(top_srcdir)/docs/images/ags_midi_export_wizard_no1.png \
	$(top_srcdir)/docs/images/ags_midi_export_wizard_no2.png \
	$(top_srcdir)/docs/images/ags_midi_import_wizard_no1.png \
	$(top_srcdir)/docs/images/ags_midi_import_wizard_no2.png \
	$(top_srcdir)/docs/images/ags_midi_preferences.png \
	$(top_srcdir)/docs/images/ags_ffplayer.png \
	$(top_srcdir)/docs/images/ags_ladspa_browser.png \
	$(top_srcdir)/docs/images/ags_machine_properties-link_input.png \
	$(top_srcdir)/docs/images/ags_matrix.png \
	$(top_srcdir)/docs/images/ags_panel.png \
	$(top_srcdir)/docs/images/ags_spectrometer.png \
	$(top_srcdir)/docs/images/ags_synth.png \
	$(top_srcdir)/docs/images/ags_syncsynth.png \
	$(top_srcdir)/docs/images/ags-toolbar.png \
	$(top_srcdir)/docs/images/ags_drum.png \
	$(top_srcdir)/docs/images/ags_generic_preferences.png \
	$(top_srcdir)/docs/images/ags_machine_properties-output.png \
	$(top_srcdir)/docs/images/ags_mixer.png \
	$(top_srcdir)/docs/images/ags_performance_preferences.png \
	$(top_srcdir)/docs/images/ags_server_preferences.png \
	$(top_srcdir)/docs/images/ags_machine_envelope_editor.png \
	$(top_srcdir)/docs/images/ags_machine_envelope_info.png \
	$(top_srcdir)/docs/images/ags_machine_envelope_pattern.png \
	$(top_srcdir)/docs/images/ags_move_notes_dialog.png \
	$(top_srcdir)/docs/images/ags_crop_notes_dialog.png \
	$(top_srcdir)/docs/images/ags_position_notation_cursor_dialog.png \
	$(top_srcdir)/docs/images/ags_position_automation_cursor_dialog.png \
	$(top_srcdir)/docs/images/ags_position_wave_cursor_dialog.png \
	$(top_srcdir)/docs/images/ags_select_notes_dialog.png \
	$(top_srcdir)/docs/images/ags_select_accelerations_dialog.png \
	$(top_srcdir)/docs/images/ags_select_buffers_dialog.png \
	$(top_srcdir)/docs/images/ags_ramp_accelerations_dialog.png \
	$(top_srcdir)/docs/images/ags-wave-toolbar.png \
	$(top_srcdir)/docs/images/ags_wave_window_audiorec.png


EXTRA_DIST += $(manpages) $(userdoc) $(userdocimages) $(devdoc) $(devdocimages) $(devdoclistings) $(oscdoc)

EXTRA_DIST += \
	$(top_srcdir)/ags/server/security/ags_authentication.dtd \
	$(top_srcdir)/ags/server/security/ags_certificate.dtd \
	$(top_srcdir)/ags/server/security/ags_password_store.dtd \
	$(top_srcdir)/ags/audio/midi/ags_midi_file.dtd \
	$(top_srcdir)/ags/audio/osc/ags_osc_file.dtd \
	$(top_srcdir)/ags/file/ags_file.dtd \
	$(top_srcdir)/ags/X/file/ags_simple_file.dtd \
	$(top_srcdir)/ags.xsl \
	$(top_srcdir)/ags-simple.xsl \
	$(top_srcdir)/ags/object/ags_marshallers.list.in \
	$(top_srcdir)/ags/widget/ags_widget_marshallers.list.in

# conditional objects


# libraries and gsequencer
libags_la_SOURCES = $(libags_c_sources) $(libags_util_c_sources) $(libags_object_c_sources) $(libags_file_c_sources) $(libags_file_task_c_sources)
libags_la_HEADERS_0 = ags/libags.h $(libags_h_sources) $(libags_util_h_sources) $(libags_object_h_sources) $(libags_file_h_sources) $(libags_file_task_h_sources)

libags_thread_la_SOURCES = $(libags_thread_c_sources) $(libags_thread_file_c_sources) $(libags_thread_task_c_sources)
libags_thread_la_HEADERS_0 = $(libags_thread_h_sources) $(libags_thread_file_h_sources) $(libags_thread_task_h_sources)
libags_server_la_SOURCES = $(libags_server_c_sources) $(libags_server_security_c_sources) $(libags_server_controller_c_sources) $(libags_server_thread_c_sources)
libags_server_la_HEADERS_0 = $(libags_server_h_sources) $(libags_server_security_h_sources) $(libags_server_controller_h_sources) $(libags_server_thread_h_sources)

libags_audio_la_SOURCES = $(libags_audio_c_sources) $(libags_audio_thread_c_sources) $(libags_audio_file_c_sources) $(libags_audio_midi_c_sources) $(libags_audio_osc_c_sources) $(libags_audio_osc_controller_c_sources) $(libags_audio_jack_c_sources) $(libags_audio_pulse_c_sources) $(libags_audio_core_audio_c_sources) $(libags_audio_audio_unit_c_sources) $(libags_audio_wasapi_c_sources) $(libags_audio_task_c_sources) $(libags_audio_task_recall_c_sources) $(libags_audio_recall_c_sources)  $(libags_plugin_c_sources)
libags_audio_la_HEADERS_0 = ags/libags-audio.h $(libags_audio_h_sources) $(libags_audio_thread_h_sources) $(libags_audio_file_h_sources) $(libags_audio_midi_h_sources) $(libags_audio_osc_h_sources) $(libags_audio_osc_controller_h_sources) $(libags_audio_jack_h_sources) $(libags_audio_pulse_h_sources) $(libags_audio_core_audio_h_sources) $(libags_audio_audio_unit_h_sources) $(libags_audio_wasapi_h_sources) $(libags_audio_task_h_sources) $(libags_audio_task_recall_h_sources) $(libags_audio_recall_h_sources)  $(libags_plugin_h_sources)

libags_gui_la_SOURCES = $(libags_gui_c_sources)
libags_gui_la_HEADERS_0 = ags/libags-gui.h $(libags_gui_h_sources)

libgsequencer_la_HEADERS_0 = $(libgsequencer_h_sources) $(libgsequencer_file_h_sources) $(libgsequencer_thread_h_sources) $(libgsequencer_task_h_sources) $(libgsequencer_import_h_sources) $(libgsequencer_export_h_sources) $(libgsequencer_machine_h_sources) $(libgsequencer_editor_h_sources)
libgsequencer_la_SOURCES = $(libgsequencer_la_HEADERS_0) $(libgsequencer_c_sources) $(libgsequencer_file_c_sources) $(libgsequencer_thread_c_sources) $(libgsequencer_task_c_sources) $(libgsequencer_import_c_sources) $(libgsequencer_export_c_sources) $(libgsequencer_machine_c_sources) $(libgsequencer_editor_c_sources)

if WITH_VST3
libags_vst_la_SOURCES = $(libags_vst_c_sources)
libags_vst_la_HEADERS_0 = ags/libags-vst.h $(libags_vst_h_sources)
endif

# library libags
libags_la_CFLAGS = $(CFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ $(UUID_CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(GIO_CFLAGS) $(W32API_CFLAGS)
libags_la_LDFLAGS = $(LDFLAGS) -export-symbols libags.sym -version-info 2:0:0 -shared -fPIC -pthread
libags_la_LIBADD = -lm $(UUID_LIBS) $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(GIO_LIBS) $(W32API_LIBS) 

if WITH_W32API
else
libags_la_LDFLAGS += -Wl,-soname,libags.so
libags_la_LIBADD += -ldl -lcrypt
endif

# library libags-thread
libags_thread_la_CFLAGS = $(CFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(W32API_CFLAGS)
libags_thread_la_LDFLAGS = $(LDFLAGS) -export-symbols libags_thread.sym -version-info 2:0:0 -shared -fPIC -pthread
libags_thread_la_LIBADD = libags.la -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(W32API_LIBS)

if WITH_W32API
else
libags_thread_la_LDFLAGS += -Wl,-soname,libags_thread.so
libags_thread_la_LIBADD += -ldl -lcrypt
endif

# library libags-server
libags_server_la_CFLAGS = $(CFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(W32API_CFLAGS)
libags_server_la_LDFLAGS = $(LDFLAGS) -export-symbols libags_server.sym -version-info 2:0:0 -shared -fPIC -pthread
libags_server_la_LIBADD = libags_thread.la libags.la -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(W32API_LIBS)

if WITH_W32API
else
libags_server_la_LDFLAGS += -Wl,-soname,libags_server.so
libags_server_la_LIBADD += -ldl -lcrypt
endif

# library libags-audio
libags_audio_la_CFLAGS = $(CFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ $(CORE_AUDIO_CFLAGS) $(WASAPI_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SAMPLERATE_CFLAGS) $(SNDFILE_CFLAGS) $(FFTW_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS) $(PULSE_CFLAGS) $(W32API_CFLAGS)
libags_audio_la_LDFLAGS = $(LDFLAGS) -export-symbols libags_audio.sym -version-info 2:0:0 -shared -fPIC -pthread 
libags_audio_la_LIBADD = libags_server.la libags_thread.la libags.la -lm $(CORE_AUDIO_LIBS) $(WASAPI_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SAMPLERATE_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(FFTW_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS) $(PULSE_LIBS) $(W32API_LIBS)

if WITH_W32API
else
libags_audio_la_LDFLAGS += -Wl,-soname,libags_audio.so
libags_audio_la_LIBADD += -ldl
endif

# library libags-gui
libags_gui_la_CFLAGS = $(CFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ $(GOBJECT_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(W32API_CFLAGS)
libags_gui_la_LDFLAGS = $(LDFLAGS) -export-symbols libags_gui.sym -version-info 2:0:0 -shared -fPIC -pthread
libags_gui_la_LIBADD = -lm $(GOBJECT_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS) $(W32API_LIBS)

if WITH_W32API
else
libags_gui_la_LDFLAGS += -Wl,-soname,libags_gui.so
libags_gui_la_LIBADD += -ldl
endif

# library libgsequencer
libgsequencer_la_CFLAGS = $(CFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ $(CORE_AUDIO_CFLAGS) $(WASAPI_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(FFTW_CFLAGS) $(JACK_CFLAGS) $(PULSE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
libgsequencer_la_LDFLAGS = $(LDFLAGS) -version-info 0:1:0 -shared -fPIC -pthread
libgsequencer_la_LIBADD = libags_audio.la libags_server.la libags_gui.la libags_thread.la libags.la -lm $(CORE_AUDIO_LIBS) $(WASAPI_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(FFTW_LIBS) $(SNDFILE_LIBS) $(JACK_LIBS) $(PULSE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS) $(GTK_MAC_INTEGRATION_LIBS) $(W32API_LIBS)

if WITH_W32API
else
libgsequencer_la_LIBADD += -ldl
endif

# library libags-vst
if WITH_VST3
libags_vst_la_CXXFLAGS = $(CXXFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ $(GLIB_CFLAGS) $(VST3_CXXFLAGS) $(W32API_CFLAGS)
libags_vst_la_LDFLAGS = $(LDFLAGS) -export-symbols libags_vst.sym -version-info 2:0:0 -shared -fPIC -pthread 
libags_vst_la_LIBADD = -lm $(GLIB_LIBS) $(VST3_LIBS) $(W32API_LIBS)

if WITH_W32API
else
libags_vst_la_LDFLAGS += -Wl,-soname,libags_vst.so
libags_vst_la_LIBADD += -ldl
endif
endif

# gsequencer
gsequencer_SOURCES = $(gsequencer_c_sources) $(gsequencerheaders)

gsequencer_CFLAGS += $(LIBASOUND2_CFLAGS) $(LIBAO_CFLAGS) $(LIBXML2_CFLAGS) $(OPENSSL_CFLAGS) $(XMLRPC_CFLAGS) $(XMLRPC_UTIL_CFLAGS) $(XMLRPC_SERVER_ABYSS_CFLAGS) $(XMLRPC_SERVER_CGI_CFLAGS) $(XMLRPC_SERVER_CFLAGS) $(SNDFILE_CFLAGS) $(JACK_CFLAGS) $(PULSE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(GTK_MAC_INTEGRATION_CFLAGS) $(W32API_CFLAGS)
gsequencer_LDFLAGS += -pthread
gsequencer_LDADD = libgsequencer.la libags_audio.la libags_server.la libags_gui.la libags_thread.la libags.la -lm $(RT_LIBS) $(X11_LIBS) $(CORE_AUDIO_CFLAGS) $(WASAPI_CFLAGS) $(LIBASOUND2_LIBS) $(LIBAO_LIBS) $(LIBXML2_LIBS) $(OPENSSL_LIBS) $(XMLRPC_LIBS) $(XMLRPC_UTIL_LIBS) $(XMLRPC_SERVER_ABYSS_LIBS) $(XMLRPC_SERVER_CGI_LIBS) $(XMLRPC_SERVER_LIBS) $(SNDFILE_LIBS) $(JACK_LIBS) $(PULSE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(FONTCONFIG_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS) $(GTK_MAC_INTEGRATION_LIBS) $(W32API_LIBS)

if WITH_W32API
else
gsequencer_LDADD += -ldl
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
	ags/lib/ags_parameter.h \
	ags/lib/ags_regex.h \
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
	ags/lib/ags_parameter.c \
	ags/lib/ags_regex.c \
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
	ags/object/ags_async_queue.h \
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
	ags/object/ags_async_queue.c \
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
	ags/file/ags_file_lookup.h \
	ags/file/ags_file_stock.h \
	ags/file/ags_file_util.h

deprecated_libags_file_c_sources =

libags_file_c_sources = \
	$(deprecated_libags_file_c_sources) \
	ags/file/ags_file.c \
	ags/file/ags_file_id_ref.c \
	ags/file/ags_file_launch.c \
	ags/file/ags_file_link.c \
	ags/file/ags_file_lookup.c \
	ags/file/ags_file_util.c

# libags-thread all files
deprecated_libags_thread_h_sources =

libags_thread_h_sources = \
	$(deprecated_libags_thread_h_sources) \
	ags/thread/ags_autosave_thread.h \
	ags/thread/ags_concurrency_provider.h \
	ags/thread/ags_condition_manager.h \
	ags/thread/ags_destroy_worker.h \
	ags/thread/ags_generic_main_loop.h \
	ags/thread/ags_history.h \
	ags/thread/ags_message_delivery.h \
	ags/thread/ags_message_queue.h \
	ags/thread/ags_mutex_manager.h \
	ags/thread/ags_poll_fd.h \
	ags/thread/ags_polling_thread.h \
	ags/thread/ags_returnable_thread.h \
	ags/thread/ags_single_thread.h \
	ags/thread/ags_task_completion.h \
	ags/thread/ags_task.h \
	ags/thread/ags_task_thread.h \
	ags/thread/ags_thread_application_context.h \
	ags/thread/ags_thread_pool.h \
	ags/thread/ags_thread-posix.h \
	ags/thread/ags_timestamp.h \
	ags/thread/ags_worker_thread.h

deprecated_libags_thread_c_sources =

libags_thread_c_sources = \
	$(deprecated_libags_thread_c_sources) \
	ags/thread/ags_autosave_thread.c \
	ags/thread/ags_concurrency_provider.c \
	ags/thread/ags_condition_manager.c \
	ags/thread/ags_destroy_worker.c \
	ags/thread/ags_generic_main_loop.c \
	ags/thread/ags_history.c \
	ags/thread/ags_message_delivery.c \
	ags/thread/ags_message_queue.c \
	ags/thread/ags_mutex_manager.c \
	ags/thread/ags_poll_fd.c \
	ags/thread/ags_polling_thread.c \
	ags/thread/ags_returnable_thread.c \
	ags/thread/ags_single_thread.c \
	ags/thread/ags_task_completion.c \
	ags/thread/ags_task.c \
	ags/thread/ags_task_thread.c \
	ags/thread/ags_thread_application_context.c \
	ags/thread/ags_thread_pool.c \
	ags/thread/ags_thread-posix.c \
	ags/thread/ags_timestamp.c \
	ags/thread/ags_worker_thread.c

# libags-thread - file
deprecated_libags_thread_file_h_sources =

libags_thread_file_h_sources = \
	$(deprecated_libags_thread_file_h_sources) \
	ags/thread/file/ags_thread_file_xml.h

deprecated_libags_thread_file_c_sources =

libags_thread_file_c_sources = \
	$(deprecated_libags_thread_file_c_sources) \
	ags/thread/file/ags_thread_file_xml.c

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
	ags/server/security/ags_authentication.h \
	ags/server/security/ags_authentication_manager.h \
	ags/server/security/ags_business_group.h \
	ags/server/security/ags_certificate.h \
	ags/server/security/ags_certificate_manager.h \
	ags/server/security/ags_password_store.h \
	ags/server/security/ags_password_store_manager.h \
	ags/server/security/ags_security_context.h \
	ags/server/security/ags_xml_authentication.h \
	ags/server/security/ags_xml_certificate.h \
	ags/server/security/ags_xml_password_store.h

deprecated_libags_server_security_c_sources =

libags_server_security_c_sources = \
	$(deprecated_libags_server_security_c_sources) \
	ags/server/security/ags_authentication.c \
	ags/server/security/ags_authentication_manager.c \
	ags/server/security/ags_business_group.c \
	ags/server/security/ags_certificate.c \
	ags/server/security/ags_certificate_manager.c \
	ags/server/security/ags_password_store.c \
	ags/server/security/ags_password_store_manager.c \
	ags/server/security/ags_security_context.c \
	ags/server/security/ags_xml_authentication.c \
	ags/server/security/ags_xml_certificate.c \
	ags/server/security/ags_xml_password_store.c

# libags-server - controller
deprecated_libags_server_controller_h_sources =

libags_server_controller_h_sources = \
	$(deprecated_libags_server_controller_h_sources) \
	ags/server/controller/ags_controller.h \
	ags/server/controller/ags_front_controller.h \
	ags/server/controller/ags_local_factory_controller.h \
	ags/server/controller/ags_local_registry_controller.h \
	ags/server/controller/ags_local_serialization_controller.h \
	ags/server/controller/ags_local_task_controller.h

deprecated_libags_server_controller_c_sources =

libags_server_controller_c_sources = \
	$(deprecated_libags_server_controller_c_sources) \
	ags/server/controller/ags_controller.c \
	ags/server/controller/ags_front_controller.c \
	ags/server/controller/ags_local_factory_controller.c \
	ags/server/controller/ags_local_registry_controller.c \
	ags/server/controller/ags_local_serialization_controller.c \
	ags/server/controller/ags_local_task_controller.c

# libags-server - file
deprecated_libags_server_file_h_sources =

libags_server_file_h_sources = \
	$(deprecated_libags_server_file_h_sources)

deprecated_libags_server_file_c_sources =

libags_server_file_c_sources = \
	$(deprecated_libags_server_file_c_sources)

# libags-server - thread
deprecated_libags_server_thread_h_sources =

libags_server_thread_h_sources = \
	$(deprecated_libags_server_thread_h_sources) \
	ags/server/thread/ags_remote_task.h \
	ags/server/thread/ags_remote_task_thread.h

deprecated_libags_server_thread_c_sources =

libags_server_thread_c_sources = \
	$(deprecated_libags_server_thread_c_sources) \
	ags/server/thread/ags_remote_task.c \
	ags/server/thread/ags_remote_task_thread.c

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
	ags/audio/ags_filter_util.h \
	ags/audio/ags_fifoout.h \
	ags/audio/ags_fm_synth_util.h \
	ags/audio/ags_fourier_transform_util.h \
	ags/audio/ags_frequency_map.h \
	ags/audio/ags_frequency_map_manager.h \
	ags/audio/ags_generic_recall_channel_run.h \
	ags/audio/ags_generic_recall_recycling.h \
	ags/audio/ags_input.h \
	ags/audio/ags_lfo_synth_util.h \
	ags/audio/ags_midi.h \
	ags/audio/ags_midiin.h \
	ags/audio/ags_notation.h \
	ags/audio/ags_note.h \
	ags/audio/ags_output.h \
	ags/audio/ags_pattern.h \
	ags/audio/ags_playback.h \
	ags/audio/ags_playback_domain.h \
	ags/audio/ags_port.h \
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
	ags/audio/ags_sound_provider.h \
	ags/audio/ags_sequencer_util.h \
	ags/audio/ags_soundcard_util.h \
	ags/audio/ags_synth_generator.h \
	ags/audio/ags_synth_util.h \
	ags/audio/ags_track.h \
	ags/audio/ags_wave.h

deprecated_libags_audio_c_sources =

libags_audio_c_sources = \
	$(deprecated_libags_audio_c_sources) \
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
	ags/audio/ags_input.c \
	ags/audio/ags_filter_util.c \
	ags/audio/ags_fifoout.c \
	ags/audio/ags_fm_synth_util.c \
	ags/audio/ags_fourier_transform_util.c \
	ags/audio/ags_frequency_map.c \
	ags/audio/ags_frequency_map_manager.c \
	ags/audio/ags_generic_recall_channel_run.c \
	ags/audio/ags_generic_recall_recycling.c \
	ags/audio/ags_lfo_synth_util.c \
	ags/audio/ags_midi.c \
	ags/audio/ags_midiin.c \
	ags/audio/ags_notation.c \
	ags/audio/ags_note.c \
	ags/audio/ags_output.c \
	ags/audio/ags_pattern.c \
	ags/audio/ags_playback.c \
	ags/audio/ags_playback_domain.c \
	ags/audio/ags_port.c \
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
	ags/audio/ags_sound_provider.c \
	ags/audio/ags_sequencer_util.c \
	ags/audio/ags_soundcard_util.c \
	ags/audio/ags_synth_generator.c \
	ags/audio/ags_synth_util.c \
	ags/audio/ags_track.c \
	ags/audio/ags_wave.c

# libags-audio - thread
deprecated_libags_audio_thread_h_sources =

libags_audio_thread_h_sources = \
	$(deprecated_libags_audio_thread_h_sources) \
	ags/audio/thread/ags_audio_loop.h \
	ags/audio/thread/ags_audio_thread.h \
	ags/audio/thread/ags_channel_thread.h \
	ags/audio/thread/ags_sequencer_thread.h \
	ags/audio/thread/ags_soundcard_thread.h \
	ags/audio/thread/ags_export_thread.h \
	ags/audio/thread/ags_sfz_loader.h \
	ags/audio/thread/ags_wave_loader.h

deprecated_libags_audio_thread_c_sources =

libags_audio_thread_c_sources = \
	$(deprecated_libags_audio_thread_c_sources) \
	ags/audio/thread/ags_audio_loop.c \
	ags/audio/thread/ags_audio_thread.c \
	ags/audio/thread/ags_channel_thread.c \
	ags/audio/thread/ags_sequencer_thread.c \
	ags/audio/thread/ags_soundcard_thread.c \
	ags/audio/thread/ags_export_thread.c \
	ags/audio/thread/ags_sfz_loader.c \
	ags/audio/thread/ags_wave_loader.c

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

# libags-audio - file
deprecated_libags_audio_file_h_sources =

libags_audio_file_h_sources = \
	$(deprecated_libags_audio_file_h_sources) \
	ags/audio/file/ags_audio_container.h \
	ags/audio/file/ags_audio_file.h \
	ags/audio/file/ags_audio_file_link.h \
	ags/audio/file/ags_audio_file_xml.h \
	ags/audio/file/ags_sound_container.h \
	ags/audio/file/ags_sound_resource.h \
	ags/audio/file/ags_sndfile.h \
	ags/audio/file/ags_sfz_file.h \
	ags/audio/file/ags_sfz_group.h \
	ags/audio/file/ags_sfz_region.h \
	ags/audio/file/ags_sfz_sample.h	

if WITH_LIBINSTPATCH
libags_audio_file_h_sources += \
	ags/audio/file/ags_ipatch.h \
	ags/audio/file/ags_ipatch_sample.h \
	ags/audio/file/ags_ipatch_dls2_reader.h \
	ags/audio/file/ags_ipatch_gig_reader.h \
	ags/audio/file/ags_ipatch_sf2_reader.h
endif

deprecated_libags_audio_file_c_sources =

libags_audio_file_c_sources = \
	$(deprecated_libags_audio_file_c_sources) \
	ags/audio/file/ags_audio_container.c \
	ags/audio/file/ags_audio_file.c \
	ags/audio/file/ags_audio_file_link.c \
	ags/audio/file/ags_audio_file_xml.c \
	ags/audio/file/ags_sound_container.c \
	ags/audio/file/ags_sound_resource.c \
	ags/audio/file/ags_sndfile.c \
	ags/audio/file/ags_sfz_file.c \
	ags/audio/file/ags_sfz_group.c \
	ags/audio/file/ags_sfz_region.c \
	ags/audio/file/ags_sfz_sample.c	

if WITH_LIBINSTPATCH
libags_audio_file_c_sources += \
	ags/audio/file/ags_ipatch.c \
	ags/audio/file/ags_ipatch_sample.c \
	ags/audio/file/ags_ipatch_dls2_reader.c \
	ags/audio/file/ags_ipatch_gig_reader.c \
	ags/audio/file/ags_ipatch_sf2_reader.c
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
	ags/audio/osc/ags_osc_parser.h \
	ags/audio/osc/ags_osc_response.h \
	ags/audio/osc/ags_osc_server.h \
	ags/audio/osc/ags_osc_util.h

deprecated_libags_audio_osc_c_sources =

libags_audio_osc_c_sources = \
	$(deprecated_libags_audio_osc_c_sources) \
	ags/audio/osc/ags_osc_buffer_util.c \
	ags/audio/osc/ags_osc_builder.c \
	ags/audio/osc/ags_osc_client.c \
	ags/audio/osc/ags_osc_connection.c \
	ags/audio/osc/ags_osc_parser.c \
	ags/audio/osc/ags_osc_response.c \
	ags/audio/osc/ags_osc_server.c \
	ags/audio/osc/ags_osc_util.c

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
	ags/audio/task/ags_cancel_audio.h \
	ags/audio/task/ags_cancel_channel.h \
	ags/audio/task/ags_clear_audio_signal.h \
	ags/audio/task/ags_clear_buffer.h \
	ags/audio/task/ags_crop_note.h \
	ags/audio/task/ags_export_output.h \
	ags/audio/task/ags_free_selection.h \
	ags/audio/task/ags_link_channel.h \
	ags/audio/task/ags_move_note.h \
	ags/audio/task/ags_notify_soundcard.h \
	ags/audio/task/ags_open_file.h \
	ags/audio/task/ags_open_single_file.h \
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
	ags/audio/task/ags_reset_peak.h \
	ags/audio/task/ags_set_muted.h

if WITH_LIBINSTPATCH
libags_audio_task_h_sources += \
	ags/audio/task/ags_open_sf2_instrument.h \
	ags/audio/task/ags_open_sf2_sample.h
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
	ags/audio/task/ags_cancel_audio.c \
	ags/audio/task/ags_cancel_channel.c \
	ags/audio/task/ags_clear_audio_signal.c \
	ags/audio/task/ags_clear_buffer.c \
	ags/audio/task/ags_crop_note.c \
	ags/audio/task/ags_export_output.c \
	ags/audio/task/ags_free_selection.c \
	ags/audio/task/ags_link_channel.c \
	ags/audio/task/ags_move_note.c \
	ags/audio/task/ags_notify_soundcard.c \
	ags/audio/task/ags_open_file.c \
	ags/audio/task/ags_open_single_file.c \
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
	ags/audio/task/ags_reset_peak.c \
	ags/audio/task/ags_set_muted.c

if WITH_LIBINSTPATCH
libags_audio_task_c_sources += \
	ags/audio/task/ags_open_sf2_instrument.c \
	ags/audio/task/ags_open_sf2_sample.c
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
	ags/X/ags_history_browser_callbacks.h \
	ags/X/ags_history_browser.h \
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
	ags/X/ags_history_browser.c \
	ags/X/ags_history_browser_callbacks.c \
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

# libgsequencer - file
deprecated_libgsequencer_file_h_sources =

libgsequencer_file_h_sources = \
	$(deprecated_libgsequencer_file_h_sources) \
	ags/X/file/ags_gui_file_xml.h \
	ags/X/file/ags_simple_file.h

deprecated_libgsequencer_file_c_sources =

libgsequencer_file_c_sources = \
	$(deprecated_libgsequencer_file_c_sources) \
	ags/X/file/ags_gui_file_xml.c \
	ags/X/file/ags_simple_file.c

# libgsequencer-thread - thread
deprecated_libgsequencer_thread_h_sources =

libgsequencer_thread_h_sources = \
	$(deprecated_libgsequencer_thread_h_sources) \
	ags/X/thread/ags_gui_thread.h \
	ags/X/thread/ags_simple_autosave_thread.h

deprecated_libgsequencer_thread_c_sources =

libgsequencer_thread_c_sources = \
	$(deprecated_libgsequencer_thread_c_sources) \
	ags/X/thread/ags_gui_thread.c \
	ags/X/thread/ags_simple_autosave_thread.c

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
	ags/X/export/ags_machine_collection_entry_callbacks.h

deprecated_libgsequencer_export_c_sources =

libgsequencer_export_c_sources = \
	$(deprecated_libgsequencer_export_c_sources) \
	ags/X/export/ags_midi_export_wizard.c \
	ags/X/export/ags_midi_export_wizard_callbacks.c \
	ags/X/export/ags_machine_collection.c \
	ags/X/export/ags_machine_collection_callbacks.c \
	ags/X/export/ags_machine_collection_entry.c \
	ags/X/export/ags_machine_collection_entry_callbacks.c

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
	ags/X/machine/ags_desk_input_pad_callbacks.h \
	ags/X/machine/ags_desk_input_pad.h \
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
	ags/X/machine/ags_ffplayer_input_pad.h
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
	ags/X/machine/ags_desk_input_pad_callbacks.c \
	ags/X/machine/ags_desk_input_pad.c \
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
	ags/X/machine/ags_ffplayer_input_pad_callbacks.c
endif

# libgsequencer - editor
deprecated_libgsequencer_editor_h_sources =

libgsequencer_editor_h_sources = \
	$(deprecated_libgsequencer_editor_h_sources) \
	ags/X/editor/ags_automation_edit_callbacks.h \
	ags/X/editor/ags_automation_edit_box.h \
	ags/X/editor/ags_automation_edit.h \
	ags/X/editor/ags_automation_toolbar_callbacks.h \
	ags/X/editor/ags_automation_toolbar.h \
	ags/X/editor/ags_crop_note_dialog_callbacks.h \
	ags/X/editor/ags_crop_note_dialog.h \
	ags/X/editor/ags_envelope_dialog_callbacks.h \
	ags/X/editor/ags_envelope_dialog.h \
	ags/X/editor/ags_envelope_editor_callbacks.h \
	ags/X/editor/ags_envelope_editor.h \
	ags/X/editor/ags_envelope_info_callbacks.h \
	ags/X/editor/ags_envelope_info.h \
	ags/X/editor/ags_file_selection_callbacks.h \
	ags/X/editor/ags_file_selection.h \
	ags/X/editor/ags_inline_player_callbacks.h \
	ags/X/editor/ags_inline_player.h \
	ags/X/editor/ags_machine_radio_button.h \
	ags/X/editor/ags_machine_radio_button_callbacks.h \
	ags/X/editor/ags_machine_selection.h \
	ags/X/editor/ags_machine_selector_callbacks.h \
	ags/X/editor/ags_machine_selector.h \
	ags/X/editor/ags_move_note_dialog_callbacks.h \
	ags/X/editor/ags_move_note_dialog.h \
	ags/X/editor/ags_notation_edit_callbacks.h \
	ags/X/editor/ags_notation_edit.h \
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
	ags/X/editor/ags_sf2_chooser_callbacks.h \
	ags/X/editor/ags_sf2_chooser.h \
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
	ags/X/editor/ags_vautomation_edit_box.h \
	ags/X/editor/ags_vwave_edit_box.h \
	ags/X/editor/ags_wave_edit_callbacks.h \
	ags/X/editor/ags_wave_edit.h \
	ags/X/editor/ags_wave_edit_box.h \
	ags/X/editor/ags_wave_toolbar_callbacks.h \
	ags/X/editor/ags_wave_toolbar.h

deprecated_libgsequencer_editor_c_sources =

libgsequencer_editor_c_sources = \
	$(deprecated_libgsequencer_editor_c_sources) \
	ags/X/editor/ags_automation_edit.c \
	ags/X/editor/ags_automation_edit_box.c \
	ags/X/editor/ags_automation_edit_callbacks.c \
	ags/X/editor/ags_automation_toolbar.c \
	ags/X/editor/ags_automation_toolbar_callbacks.c \
	ags/X/editor/ags_crop_note_dialog_callbacks.c \
	ags/X/editor/ags_crop_note_dialog.c \
	ags/X/editor/ags_envelope_dialog_callbacks.c \
	ags/X/editor/ags_envelope_dialog.c \
	ags/X/editor/ags_envelope_editor_callbacks.c \
	ags/X/editor/ags_envelope_editor.c \
	ags/X/editor/ags_envelope_info_callbacks.c \
	ags/X/editor/ags_envelope_info.c \
	ags/X/editor/ags_file_selection_callbacks.c \
	ags/X/editor/ags_file_selection.c \
	ags/X/editor/ags_inline_player.c \
	ags/X/editor/ags_inline_player_callbacks.c \
	ags/X/editor/ags_machine_radio_button.c \
	ags/X/editor/ags_machine_radio_button_callbacks.c \
	ags/X/editor/ags_machine_selection.c \
	ags/X/editor/ags_machine_selector.c \
	ags/X/editor/ags_machine_selector_callbacks.c \
	ags/X/editor/ags_move_note_dialog_callbacks.c \
	ags/X/editor/ags_move_note_dialog.c \
	ags/X/editor/ags_notation_edit.c \
	ags/X/editor/ags_notation_edit_callbacks.c \
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
	ags/X/editor/ags_sf2_chooser.c \
	ags/X/editor/ags_sf2_chooser_callbacks.c \
	ags/X/editor/ags_sheet_edit.c \
	ags/X/editor/ags_sheet_edit_callbacks.c \
	ags/X/editor/ags_sheet_toolbar.c \
	ags/X/editor/ags_sheet_toolbar_callbacks.c \
	ags/X/editor/ags_vautomation_edit_box.c \
	ags/X/editor/ags_vwave_edit_box.c \
	ags/X/editor/ags_wave_edit.c \
	ags/X/editor/ags_wave_edit_box.c \
	ags/X/editor/ags_wave_edit_callbacks.c \
	ags/X/editor/ags_wave_toolbar.c \
	ags/X/editor/ags_wave_toolbar_callbacks.c

# libags-vst
deprecated_libags_vst_h_sources =

libags_vst_h_sources = \
	$(deprecated_libags_vst_h_sources) \
	ags/vst3-capi/pluginterfaces/vst/ags_vst_types.h \
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
	ags/vst3-capi/pluginterfaces/base/ags_vst_fplatform.h \
	ags/vst3-capi/pluginterfaces/base/ags_vst_smart_pointer.h \
	ags/vst3-capi/util/ags_vst_string_util.h \
	ags/vst3-capi/base/source/ags_vst_fbuffer.h \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_audio_effect.h \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_component.h \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_component_base.h \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_edit_controller.h \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_bus.h \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_parameters.h

deprecated_libags_vst_c_sources =

libags_vst_c_sources = \
	$(deprecated_libags_vst_c_sources) \
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
	ags/vst3-capi/util/ags_vst_string_util.cpp \
	ags/vst3-capi/base/source/ags_vst_fbuffer.cpp \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_audio_effect.cpp \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_component.cpp \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_component_base.cpp \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_edit_controller.cpp \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_bus.cpp \
	ags/vst3-capi/public.sdk/source/vst/ags_vst_parameters.cpp

# gsequencer all files
gsequencerheaders = \
	ags/gsequencer_main.h

gsequencer_c_sources = \
	ags/gsequencer_main.c

midi2xml_CFLAGS += -I./ $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(GIO_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(XMLRPC_CFLAGS) $(XMLRPC_UTIL_CFLAGS) $(XMLRPC_SERVER_ABYSS_CFLAGS) $(XMLRPC_SERVER_CGI_CFLAGS) $(XMLRPC_SERVER_CFLAGS) $(W32API_CFLAGS)
midi2xml_LDFLAGS += $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(XMLRPC_LIBS) $(XMLRPC_UTIL_LIBS) $(LIBINSTPATCH_LIBS) $(XMLRPC_SERVER_ABYSS_LIBS) $(XMLRPC_SERVER_CGI_LIBS) $(XMLRPC_SERVER_LIBS) $(W32API_LIBS)
midi2xml_LDADD = libags_audio.la libags_server.la libags_gui.la libags_thread.la libags.la

if WITH_W32API
else
midi2xml_LDADD += -ldl
endif

midi2xml_SOURCES = ags/midi2xml_main.c

ags-integration-unit-test: 
	$(MAKE) -C $(top_srcdir) -f unit-system-tests.mk ags-check-system-unit-tests

ags-integration-functional-test: 
	$(MAKE) -C $(top_srcdir) -f functional-system-tests.mk ags-check-system-functional-test

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool

gsequencer.1:
	xsltproc --nonet --param make.year.ranges 1 --param make.single.year.ranges 1 --param man.charmap.use.subset 0 -o $(top_srcdir) http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl gsequencer.1.xml

midi2xml.1:
	xsltproc --nonet --param make.year.ranges 1 --param make.single.year.ranges 1 --param man.charmap.use.subset 0 -o $(top_srcdir)/ http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl midi2xml.1.xml

gsequencer.desktop: gsequencer.desktop.in
	$(AM_V_GEN)sed -e 's,\@datadir\@,$(datadir),g' < "$<" > $@

html:
	mkdir -p $(top_builddir)/html/
	mkdir -p $(top_builddir)/html/developer-docs
	mkdir -p $(top_builddir)/html/user-docs
	mkdir -p $(top_builddir)/html/osc-docs
	xsltproc --output $(top_builddir)/html/user-docs/ --xinclude /usr/share/xml/docbook/stylesheet/docbook-xsl/htmlhelp/htmlhelp.xsl $(top_srcdir)/docs/usersBook.xml
	xsltproc --output $(top_builddir)/html/developer-docs/ --xinclude /usr/share/xml/docbook/stylesheet/docbook-xsl/htmlhelp/htmlhelp.xsl $(top_srcdir)/docs/developersBook.xml
	xsltproc --output $(top_builddir)/html/osc-docs/ --xinclude /usr/share/xml/docbook/stylesheet/docbook-xsl/htmlhelp/htmlhelp.xsl $(top_srcdir)/docs/oscBook.xml
	$(MAKE) -C $(top_srcdir)/docs/reference/libags
	cd $(top_srcdir)
	$(MAKE) -C $(top_srcdir)/docs/reference/libags-audio
	cd $(top_srcdir)
	$(MAKE) -C $(top_srcdir)/docs/reference/libags-gui
	cd $(top_srcdir)
	$(MAKE) -C $(top_srcdir)/docs/reference/libgsequencer
	cd $(top_srcdir)

install-data-local:
	mkdir -p $(DESTDIR)/$(includedir)/ags
	install -c -p -m 644 $(top_builddir)/ags/ags_config.h $(DESTDIR)/$(includedir)/ags/config.h
	mkdir -p $(DESTDIR)/$(datadir)/doc-base
	mkdir -p $(DESTDIR)/$(datadir)/gsequencer/icons
	mkdir -p $(DESTDIR)/$(datadir)/icons/hicolor/32x32/apps
	mkdir -p $(DESTDIR)/$(datadir)/icons/hicolor/48x48/apps
	mkdir -p $(DESTDIR)/$(datadir)/icons/hicolor/64x64/apps
	mkdir -p $(DESTDIR)/$(datadir)/icons/hicolor/128x128/apps
	mkdir -p $(DESTDIR)/$(datadir)/gsequencer/images
	mkdir -p $(DESTDIR)/$(datadir)/gsequencer/styles
	mkdir -p $(DESTDIR)/$(datadir)/xml/gsequencer/schema/dtd/$(PACKAGE_VERSION)
	mkdir -p $(DESTDIR)/$(datadir)/xml/gsequencer/stylesheet/ags-xsl/midi-xml
	mkdir -p $(DESTDIR)/$(datadir)/mime/packages
	mkdir -p $(DESTDIR)/$(datadir)/metainfo
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/icons/ags.xpm $(DESTDIR)/$(datadir)/gsequencer/icons/ags.xpm
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/icons/jumper.png $(DESTDIR)/$(datadir)/gsequencer/icons/jumper.png
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/icons/ags.png $(DESTDIR)/$(datadir)/gsequencer/icons/ags.png
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/icons/gsequencer-32x32.png $(DESTDIR)/$(datadir)/icons/hicolor/32x32/apps/gsequencer.png
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/icons/gsequencer-48x48.png $(DESTDIR)/$(datadir)/icons/hicolor/48x48/apps/gsequencer.png
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/icons/gsequencer-64x64.png $(DESTDIR)/$(datadir)/icons/hicolor/64x64/apps/gsequencer.png
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/icons/gsequencer-128x128.png $(DESTDIR)/$(datadir)/icons/hicolor/128x128/apps/gsequencer.png
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/images/ags.png $(DESTDIR)/$(datadir)/gsequencer/images/ags.png
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/images/ags_supermoon-800x450.png $(DESTDIR)/$(datadir)/gsequencer/images/ags_supermoon-800x450.png
	install -c -p -m 644 $(top_srcdir)/gsequencer.share/styles/ags.rc $(DESTDIR)/$(datadir)/gsequencer/styles/ags.rc
	install -c -p -m 644 $(top_srcdir)/ags/X/file/ags_simple_file.dtd $(DESTDIR)/$(datadir)/xml/gsequencer/schema/dtd/$(PACKAGE_VERSION)/
	install -c -p -m 644 $(top_srcdir)/ags/audio/midi/ags_midi_file.dtd $(DESTDIR)/$(datadir)/xml/gsequencer/schema/dtd/$(PACKAGE_VERSION)/
	install -c -p -m 644 $(top_srcdir)/ags/audio/osc/ags_osc_file.dtd $(DESTDIR)/$(datadir)/xml/gsequencer/schema/dtd/$(PACKAGE_VERSION)/
	install -c -p -m 644 $(top_srcdir)/ags/file/ags_file.dtd $(DESTDIR)/$(datadir)/xml/gsequencer/schema/dtd/$(PACKAGE_VERSION)/
	install -c -p -m 644 $(top_srcdir)/ags.xsl $(DESTDIR)/$(datadir)/xml/gsequencer/stylesheet/ags-xsl/midi-xml/
	install -c -p -m 644 $(top_srcdir)/ags-simple.xsl $(DESTDIR)/$(datadir)/xml/gsequencer/stylesheet/ags-xsl/midi-xml/
	install -c -p -m 644 $(top_srcdir)/application-x-gsequencer.xml $(DESTDIR)/$(datadir)/mime/packages/gsequencer.xml
	install -c -p -m 644 $(top_srcdir)/org.nongnu.gsequencer.gsequencer.appdata.xml $(DESTDIR)/$(datadir)/metainfo/
	mkdir -p $(DESTDIR)/$(docdir)/
	gzip -9 -c $(top_srcdir)/ChangeLog > $(DESTDIR)/$(docdir)/changelog.gz

fix-local-html: html
	find $(top_srcdir)/docs/reference/libags/libags-html -name "*.html" -type f -exec sed -i 's/\.\.\/gobject/\/usr\/share\/doc\/libglib2.0-doc\/gobject/g' {} \;
	find $(top_srcdir)/docs/reference/libags-audio/libags-audio-html -name "*.html" -type f -exec sed -i 's/\.\.\/gobject/\/usr\/share\/doc\/libglib2.0-doc\/gobject/g' {} \;
	find $(top_srcdir)/docs/reference/libags-audio/libags-audio-html -name "*.html" -type f -exec sed -i 's/\.\.\/libags-html/\/usr\/share\/doc\/libags-doc\/api/g' {} \;
	find $(top_srcdir)/docs/reference/libags-gui/libags-gui-html -name "*.html" -type f -exec sed -i 's/\.\.\/gobject/\/usr\/share\/doc\/libglib2.0-doc\/gobject/g' {} \;
	find $(top_srcdir)/docs/reference/libags-gui/libags-gui-html -name "*.html" -type f -exec sed -i 's/\.\.\/gtk2/\/usr\/share\/doc\/libgtk2.0-doc\/gtk2/g' {} \;
	find $(top_srcdir)/docs/reference/libags-gui/libags-gui-html -name "*.html" -type f -exec sed -i 's/\.\.\/gtk3/\/usr\/share\/doc\/libgtk2.0-doc\/gtk2/g' {} \;
	find $(top_srcdir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i 's/\.\.\/gobject/\/usr\/share\/doc\/libglib2.0-doc\/gobject/g' {} \;
	find $(top_srcdir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i 's/\.\.\/gtk2/\/usr\/share\/doc\/libgtk2.0-doc\/gtk2/g' {} \;
	find $(top_srcdir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i 's/\.\.\/gtk3/\/usr\/share\/doc\/libgtk2.0-doc\/gtk2/g' {} \;
	find $(top_srcdir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i 's/\.\.\/libags-html/\/usr\/share\/doc\/libags-doc\/api/g' {} \;
	find $(top_srcdir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i 's/\.\.\/libags-audio-html/\/usr\/share\/doc\/libags-audio-doc\/api/g' {} \;
	find $(top_srcdir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i 's/\.\.\/libags-gui-html/\/usr\/share\/doc\/libags-gui-doc\/api/g' {} \;

fix-online-books-html: html
	find $(top_srcdir)/html/ -name "*.html" -type f -exec sed -i 's/\/usr\/share\/icons\/Adwaita\/32x32\/actions/..\/images/g' {} \;
	find $(top_srcdir)/html/ -name "*.html" -type f -exec sed -i 's/\/usr\/share\/icons\/Adwaita\/32x32\/apps/..\/images/g' {} \;

fix-online-html: html
	find $(top_srcdir)/docs/reference/libags/libags-html -name "*.html" -type f -exec sed -i 's/\.\.\/gobject/https\:\/\/developer\.gnome\.org\/gobject\/stable/g' {} \;
	find $(top_srcdir)/docs/reference/libags-audio/libags-audio-html -name "*.html" -type f -exec sed -i 's/\.\.\/gobject/https\:\/\/developer\.gnome\.org\/gobject\/stable/g' {} \;
	find $(top_srcdir)/docs/reference/libags-audio/libags-audio-html -name "*.html" -type f -exec sed -i 's/\.\.\/libags-html/\.\.\/libags/g' {} \;
	find $(top_srcdir)/docs/reference/libags-gui/libags-gui-html -name "*.html" -type f -exec sed -i 's/\.\.\/gobject/https\:\/\/developer\.gnome\.org\/gobject\/stable/g' {} \;
	find $(top_srcdir)/docs/reference/libags-gui/libags-gui-html -name "*.html" -type f -exec sed -i 's/\.\.\/gtk2/https\:\/\/developer\.gnome\.org\/gtk2\/stable/g' {} \;
	find $(top_srcdir)/docs/reference/libags-gui/libags-gui-html -name "*.html" -type f -exec sed -i 's/\.\.\/gtk3/https\:\/\/developer\.gnome\.org\/gtk2\/stable/g' {} \;
	find $(top_srcdir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i 's/\.\.\/gobject/https\:\/\/developer\.gnome\.org\/gobject\/stable/g' {} \;
	find $(top_srcdir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i 's/\.\.\/gtk2/https\:\/\/developer\.gnome\.org\/gtk2\/stable/g' {} \;
	find $(top_srcdir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i 's/\.\.\/gtk3/https\:\/\/developer\.gnome\.org\/gtk2\/stable/g' {} \;
	find $(top_srcdir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i 's/\.\.\/libags-html/\.\.\/libags/g' {} \;
	find $(top_srcdir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i 's/\.\.\/libags-audio-html/\.\.\/libags-audio/g' {} \;
	find $(top_srcdir)/docs/reference/libgsequencer/libgsequencer-html -name "*.html" -type f -exec sed -i 's/\.\.\/libags-gui-html/\.\.\/libags-gui/g' {} \;

install-html: fix-local-html 
	mkdir -p $(DESTDIR)/$(docdir)/images
	mkdir -p $(DESTDIR)/$(docdir)/html
	mkdir -p $(DESTDIR)/$(datadir)/doc/libags-audio-doc/images
	mkdir -p $(DESTDIR)/$(datadir)/doc/libags-audio-doc/html
	mkdir -p $(DESTDIR)/$(datadir)/doc/libags-audio-doc/osc-server
	install -c -p -m 644 $(userdocimages) $(DESTDIR)/$(docdir)/images/
	install -c -p -m 644 $(devdocimages) $(DESTDIR)/$(datadir)/doc/libags-audio-doc/images/
	install -c -p -m 644 $(top_builddir)/html/user-docs/* $(DESTDIR)/$(docdir)/html/
	install -c -p -m 644 $(top_builddir)/html/developer-docs/* $(DESTDIR)/$(datadir)/doc/libags-audio-doc/html/
	install -c -p -m 644 $(top_builddir)/html/osc-docs/* $(DESTDIR)/$(datadir)/doc/libags-audio-doc/osc-server/
	mkdir -p $(DESTDIR)/$(datadir)/gtk-doc/html
	mkdir -p $(DESTDIR)/$(datadir)/doc/libags-doc/api
	mkdir -p $(DESTDIR)/$(datadir)/doc/libags-audio-doc/api
	mkdir -p $(DESTDIR)/$(datadir)/doc/libags-gui-doc/api
	mkdir -p $(DESTDIR)/$(datadir)/doc/libgsequencer-doc/api
	ln -s ../../../gtk-doc/html/libags $(DESTDIR)/$(datadir)/doc/libags-doc/api
	ln -s ../../../gtk-doc/html/libags_audio $(DESTDIR)/$(datadir)/doc/libags-audio-doc/api
	ln -s ../../../gtk-doc/html/libags_gui $(DESTDIR)/$(datadir)/doc/libags-gui-doc/api
	ln -s ../../../gtk-doc/html/libgsequencer $(DESTDIR)/$(datadir)/doc/libgsequencer-doc/api
	install -c -p -m 644 $(top_srcdir)/docs/reference/libags/libags-html/* $(DESTDIR)/$(datadir)/doc/libags-doc/api
	install -c -p -m 644 $(top_srcdir)/docs/reference/libags-audio/libags-audio-html/* $(DESTDIR)/$(datadir)/doc/libags-audio-doc/api
	install -c -p -m 644 $(top_srcdir)/docs/reference/libags-gui/libags-gui-html/* $(DESTDIR)/$(datadir)/doc/libags-gui-doc/api
	install -c -p -m 644 $(top_srcdir)/docs/reference/libgsequencer/libgsequencer-html/* $(DESTDIR)/$(datadir)/doc/libgsequencer-doc/api

uninstall-hook:
	rm -rf $(DESTDIR)/$(includedir)/ags/config.h
	rm -rf $(DESTDIR)/$(datadir)/gsequencer
	rm -rf $(DESTDIR)/$(docdir)
	rm -rf $(DESTDIR)/$(datadir)/xml/gsequencer
	rm -rf $(DESTDIR)/$(datadir)/doc/libags-doc
	rm -rf $(DESTDIR)/$(datadir)/doc/libags-audio-doc
	rm -rf $(DESTDIR)/$(datadir)/doc/libags-gui-doc
	rm -rf $(DESTDIR)/$(datadir)/doc/libgsequencer-doc
	rm -rf $(DESTDIR)/$(datadir)/gtk-doc/html/libags
	rm -rf $(DESTDIR)/$(datadir)/gtk-doc/html/libags_audio
	rm -rf $(DESTDIR)/$(datadir)/gtk-doc/html/libags_gui
	rm -rf $(DESTDIR)/$(datadir)/gtk-doc/html/libgsequencer
	rm -f $(DESTDIR)/$(datadir)/icons/hicolor/32x32/apps/gsequencer.png
	rm -f $(DESTDIR)/$(datadir)/icons/hicolor/48x48/apps/gsequencer.png
	rm -f $(DESTDIR)/$(datadir)/icons/hicolor/64x64/apps/gsequencer.png
	rm -f $(DESTDIR)/$(datadir)/icons/hicolor/128x128/apps/gsequencer.png
	rm -f $(DESTDIR)/$(datadir)/mime/packages/gsequencer.xml
	rm -f $(DESTDIR)/$(datadir)/metainfo/org.nongnu.gsequencer.gsequencer.appdata.xml
	rm -f $(DESTDIR)/$(pkgconfigdir)/libags.pc
	rm -f $(DESTDIR)/$(pkgconfigdir)/libags_audio.pc
	rm -f $(DESTDIR)/$(pkgconfigdir)/libags_gui.pc
	rm -f $(DESTDIR)/$(pkgconfigdir)/libgsequencer.pc

clean-local:
	rm -rf $(top_srcdir)/html
	rm -rf $(top_srcdir)/docs/reference/libags/xml
	rm -rf $(top_srcdir)/docs/reference/libags/libags-html
	rm -rf $(top_srcdir)/docs/reference/libags/tmpl
	rm -rf $(top_srcdir)/docs/reference/libags-audio/xml
	rm -rf $(top_srcdir)/docs/reference/libags-audio/libags-audio-html
	rm -rf $(top_srcdir)/docs/reference/libags-audio/tmpl
	rm -rf $(top_srcdir)/docs/reference/libags-gui/xml
	rm -rf $(top_srcdir)/docs/reference/libags-gui/libags-gui-html
	rm -rf $(top_srcdir)/docs/reference/libags-gui/tmpl
	rm -rf $(top_srcdir)/docs/reference/libgsequencer/xml
	rm -rf $(top_srcdir)/docs/reference/libgsequencer/libgsequencer-html
	rm -rf $(top_srcdir)/docs/reference/libgsequencer/tmpl
	rm -f $(top_builddir)/docs/reference/libags/Makefile
	rm -f $(top_builddir)/docs/reference/libags-audio/Makefile
	rm -f $(top_builddir)/docs/reference/libags-gui/Makefile
	rm -f $(top_builddir)/docs/reference/libgsequencer/Makefile
	rm -f $(top_builddir)/unit-system-tests.mk
	rm -f $(top_builddir)/functional-system-tests.mk
	rm -f docs/reference/libags-audio/libags_audio-decl-list.txt
	rm -f docs/reference/libags-audio/libags_audio-decl.txt
	rm -f docs/reference/libags-audio/libags_audio-undeclared.txt
	rm -f docs/reference/libags-audio/libags_audio-undocumented.txt
	rm -f docs/reference/libags-audio/libags_audio-unused.txt
	rm -f docs/reference/libags-audio/libags_audio.args
	rm -f docs/reference/libags-audio/libags_audio.hierarchy
	rm -f docs/reference/libags-audio/libags_audio.prerequisites
	rm -f docs/reference/libags-audio/libags_audio.signals
	rm -f docs/reference/libags-gui/libags_gui-decl-list.txt
	rm -f docs/reference/libags-gui/libags_gui-decl.txt
	rm -f docs/reference/libags-gui/libags_gui-undeclared.txt
	rm -f docs/reference/libags-gui/libags_gui-undocumented.txt
	rm -f docs/reference/libags-gui/libags_gui-unused.txt
	rm -f docs/reference/libags-gui/libags_gui.args
	rm -f docs/reference/libags-gui/libags_gui.hierarchy
	rm -f docs/reference/libags-gui/libags_gui.prerequisites
	rm -f docs/reference/libags-gui/libags_gui.signals
	rm -f docs/reference/libags/libags-decl-list.txt
	rm -f docs/reference/libags/libags-decl.txt
	rm -f docs/reference/libags/libags-undeclared.txt
	rm -f docs/reference/libags/libags-undocumented.txt
	rm -f docs/reference/libags/libags-unused.txt
	rm -f docs/reference/libags/libags.args
	rm -f docs/reference/libags/libags.hierarchy
	rm -f docs/reference/libags/libags.prerequisites
	rm -f docs/reference/libags/libags.signals
	rm -f docs/reference/libgsequencer/libgsequencer-decl-list.txt
	rm -f docs/reference/libgsequencer/libgsequencer-decl.txt
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

gen-symbols-ags-gui:
	rm -f libags_gui.sym.in
	touch libags_gui.sym.in
	cat license-notice-gnu-gpl-3-0+-sym.txt >> libags_gui.sym.in
	echo -e -n "\n" >> libags_gui.sym.in
	find ags/widget -name "*.h" | xargs grep --no-filename -o -P "(?<=\s)(ags_[a-z0-9_]+)(?=[\s]*\()" >> libags_gui.sym.in
	find ags/widget -name "ags_widget_marshal.h" | xargs grep --no-filename -o -P "(ags_widget_cclosure_marshal_[a-zA-Z0-9_]+)" >> libags_gui.sym.in

gen-symbols-all: gen-symbols-ags gen-symbols-ags-thread gen-symbols-ags-server gen-symbols-ags-audio gen-symbols-ags-gui
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
	@find $(srcdir)/ags/{util,lib,object,thread,file,server,plugin,audio,widget,X}/ -path $(srcdir)/ags/audio/audio-unit -prune -o -name "*.c" -print | sort > $(srcdir)/po/POTFILES.in.2 ; \
	if diff $(srcdir)/po/POTFILES.in $(srcdir)/po/POTFILES.in.2 >/dev/null 2>&1 ; then \
		rm -f $(srcdir)/po/POTFILES.in.2 ; \
	else \
		mv $(srcdir)/po/POTFILES.in.2 $(srcdir)/po/POTFILES.in ; \
	fi
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-po

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
