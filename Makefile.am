AUTOMAKE_OPTIONS = foreign
SUBDIRS = docs/reference/ags

AM_CPPFLAGS = -include /usr/include/errno.h -I$(top_srcdir) -DDESTDIR=\"$(DESTDIR)$(datadir)\" -DPACKAGE_VERSION=\"$(PACKAGE_VERSION)\" -D_FORTIFY_SOURCE=2

# what flags you want to pass to the C compiler & linker
CFLAGS = @CFLAGS@
LDFLAGS = @LDFLAGS@
LIBTOOL_DEPS = @LIBTOOL_DEPS@

# 
# TARGETS = gsequencer

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
lib_LTLIBRARIES = libags.la libags_thread.la libags_server.la libags_audio.la libags_gui.la libgsequencer.la

bin_PROGRAMS = gsequencer gsequencer_static midi2xml

EXTRA_DIST = gsequencer.share acsite.m4 gsequencer.1 midi2xml.1 README gsequencer-documentation

CLEANFILES =

man1_MANS = gsequencer.1 midi2xml.1

libags_ladir = $(datadir)
libags_thread_ladir = $(datadir)
libags_server_ladir = $(datadir)
libags_audio_ladir = $(datadir)
libags_gui_ladir = $(datadir)
libgsequencer_ladir = $(datadir)

appdatadir = $(datadir)/applications
appdata_DATA = gsequencer.desktop

EXTRA_DIST += gsequencer.desktop.in
CLEANFILES += $(appdata_DATA)

otherincludedir = $(includedir)/ags
nobase_include_HEADERS = $(libags_la_HEADERS) $(libags_thread_la_HEADERS) $(libags_server_la_HEADERS) $(libags_audio_la_HEADERS) $(libags_gui_la_HEADERS)

docdir = $(datadir)/docs/gsequencer
doc_DATA = README

# documentation
userdoc = \
	docs/usersBook.xml \
	docs/appa.xml \
	docs/usersBook/appb.xml \
	docs/usersBook/chap1.xml \
	docs/usersBook/chap2.xml \
	docs/usersBook/chap3.xml \
	docs/usersBook/chap4.xml \
	docs/usersBook/preface.xml

userdocimages = \
	$(top_srcdir)/docs/images/ags_export_window.png \
	$(top_srcdir)/docs/images/ags_machine_properties-input.png \
	$(top_srcdir)/docs/images/ags_machine_properties-resize_channels.png \
	$(top_srcdir)/docs/images/ags_navigation.png \
	$(top_srcdir)/docs/images/ags_synth.png \
	$(top_srcdir)/docs/images/ags_audio_preferences.png \
	$(top_srcdir)/docs/images/ags_ffplayer.png \
	$(top_srcdir)/docs/images/ags_ladspa_browser.png \
	$(top_srcdir)/docs/images/ags_machine_properties-link_input.png \
	$(top_srcdir)/docs/images/ags_matrix.png \
	$(top_srcdir)/docs/images/ags_panel.png \
	$(top_srcdir)/docs/images/ags-toolbar.png \
	$(top_srcdir)/docs/images/ags_drum.png \
	$(top_srcdir)/docs/images/ags_generic_preferences.png \
	$(top_srcdir)/docs/images/ags_machine_properties-output.png \
	$(top_srcdir)/docs/images/ags_mixer.png \
	$(top_srcdir)/docs/images/ags_performance_preferences.png \
	$(top_srcdir)/docs/images/ags_server_preferences.png

EXTRA_DIST += $(userdoc) $(userdocimages)

EXTRA_DIST += $(top_srcdir)/ags/file/ags_file.dtd $(top_srcdir)/ags.xsl

# libraries and gsequencer
libags_la_SOURCES = $(libags_c_sources) $(libags_util_c_sources) $(libags_object_c_sources) $(libags_file_c_sources) $(libags_file_task_c_sources)
libags_la_HEADERS = $(libags_h_sources) $(libags_util_h_sources) $(libags_object_h_sources) $(libags_file_h_sources) $(libags_file_task_h_sources)
libags_thread_la_SOURCES = $(libags_thread_c_sources) $(libags_thread_file_c_sources) $(libags_thread_task_c_sources)
libags_thread_la_HEADERS = $(libags_thread_h_sources) $(libags_thread_file_h_sources) $(libags_thread_task_h_sources)
libags_server_la_SOURCES = $(libags_server_c_sources)
libags_server_la_HEADERS = $(libags_server_h_sources)
libags_audio_la_SOURCES = $(libags_audio_c_sources) $(libags_audio_thread_c_sources) $(libags_audio_file_c_sources) $(libags_audio_midi_c_sources) $(libags_audio_jack_c_sources) $(libags_audio_task_c_sources) $(libags_audio_task_recall_c_sources) $(libags_audio_recall_c_sources)  $(libags_plugin_c_sources)
libags_audio_la_HEADERS = $(libags_audio_h_sources) $(libags_audio_thread_h_sources) $(libags_audio_file_h_sources) $(libags_audio_midi_h_sources) $(libags_audio_jack_h_sources) $(libags_audio_task_h_sources) $(libags_audio_task_recall_h_sources) $(libags_audio_recall_h_sources)  $(libags_plugin_h_sources)
libags_gui_la_SOURCES = $(libags_gui_c_sources)
libags_gui_la_HEADERS = $(libags_gui_h_sources)
libgsequencer_la_SOURCES = $(libgsequencer_c_sources) $(libgsequencer_file_c_sources) $(libgsequencer_thread_c_sources) $(libgsequencer_task_c_sources) $(libgsequencer_import_c_sources) $(libgsequencer_machine_c_sources) $(libgsequencer_editor_c_sources)
libgsequencer_la_HEADERS = $(libgsequencer_h_sources) $(libgsequencer_file_h_sources) $(libgsequencer_thread_h_sources) $(libgsequencer_task_h_sources) $(libgsequencer_import_h_sources) $(libgsequencer_machine_h_sources) $(libgsequencer_editor_h_sources)
gsequencer_SOURCES = $(gsequencer_c_sources) $(gsequencerheaders) $(libags_la_HEADERS) $(libags_thread_la_HEADERS) $(libags_server_la_HEADERS) $(libags_audio_la_HEADERS) $(libags_gui_la_HEADERS) $(libgsequencer_la_HEADERS)

# testing
libgsequencer_test_a_SOURCES = $(gsequencer_functional_test_runtime_sources) $(gsequencer_functional_test_x_util_sources)

# library libags
libags_la_CFLAGS = $(CFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ -I/usr/include $(UUID_CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS)
libags_la_LDFLAGS = $(LDFLAGS) -L/lib -L/usr/lib -L/usr/X11/lib -ldl -lm -pthread -lrt -lgmp  $(UUID_LIBS) $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS)

# library libags-thread
libags_thread_la_CFLAGS = $(CFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ -I/usr/include $(UUID_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(GOBJECT_CFLAGS)
libags_thread_la_LDFLAGS = $(LDFLAGS) -L/lib -L/usr/lib -L/usr/X11/lib -ldl -lm -pthread -lrt -lgmp $(UUID_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(GOBJECT_LIBS)
libags_thread_la_LIBADD = libags.la

# library libags-server
libags_server_la_CFLAGS = $(CFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ -I/usr/include $(UUID_CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
libags_server_la_LDFLAGS = $(LDFLAGS) -L/lib -L/usr/lib -L/usr/X11/lib -ldl -lm -pthread -lrt -lgmp $(UUID_LIBS) $(LIBXML2_LIBS) $(GOBJECT_LIBS)
libags_server_la_LIBADD = libags_thread.la libags.la

# library libags-audio
libags_audio_la_CFLAGS = $(CFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ -I/usr/include $(UUID_CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
libags_audio_la_LDFLAGS = $(LDFLAGS) -L/lib -L/usr/lib -L/usr/X11/lib -L/usr/lib/x86_64-linux-gnu -ldl -lm -pthread -lrt -lgmp $(UUID_LIBS) $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)
libags_audio_la_LIBADD = libags_server.la libags_thread.la libags.la

# library libags-gui
libags_gui_la_CFLAGS = $(CFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ -I/usr/include $(UUID_CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
libags_gui_la_LDFLAGS = $(LDFLAGS) -L/lib -L/usr/lib -L/usr/X11/lib -L/usr/lib/x86_64-linux-gnu -ldl -lm -pthread -lrt -lgmp $(UUID_LIBS) $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS)

# library libgsequencer
libgsequencer_la_CFLAGS = $(CFLAGS) $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./ -I/usr/include $(UUID_CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(JACK_CFLAGS)
libgsequencer_la_LDFLAGS = $(LDFLAGS) -L/lib -L/usr/lib -L/usr/X11/lib -L/usr/lib/x86_64-linux-gnu -ldl -lm -pthread -lrt -lgmp $(UUID_LIBS) $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS) $(JACK_LIBS)
libgsequencer_la_LIBADD = libags_audio.la libags_server.la libags_gui.la libags_thread.la libags.la

# application gsequencer
gsequencer_CFLAGS= $(CFLAGS) $(UUID_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBAO_CFLAGS) $(LIBXML2_CFLAGS) $(OPENSSL_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(XMLRPC_CFLAGS) $(XMLRPC_UTIL_CFLAGS) $(XMLRPC_SERVER_ABYSS_CFLAGS) $(XMLRPC_SERVER_CGI_CFLAGS) $(XMLRPC_SERVER_CFLAGS) $(PTH_FLAGS) $(JACK_CFLAGS)
gsequencer_LDFLAGS= -ldl -lm -lrt -pthread $(LDFLAGS) $(UUID_LIBS) $(LIBASOUND2_LIBS) $(LIBAO_LIBS) $(LIBXML2_LIBS) $(OPENSSL_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(FONTCONFIG_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS) $(XMLRPC_LIBS) $(XMLRPC_UTIL_LIBS) $(XMLRPC_SERVER_ABYSS_LIBS) $(XMLRPC_SERVER_CGI_LIBS) $(XMLRPC_SERVER_LIBS) $(JACK_LIBS)
gsequencer_LDADD = libgsequencer.la libags_audio.la libags_server.la libags_gui.la libags_thread.la libags.la

# static gsequencer - for testing
gsequencer_static_SOURCES = $(gsequencer_c_sources)

gsequencer_static_CFLAGS= $(CFLAGS) $(UUID_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBAO_CFLAGS) $(LIBXML2_CFLAGS) $(OPENSSL_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(XMLRPC_CFLAGS) $(XMLRPC_UTIL_CFLAGS) $(XMLRPC_SERVER_ABYSS_CFLAGS) $(XMLRPC_SERVER_CGI_CFLAGS) $(XMLRPC_SERVER_CFLAGS) $(PTH_FLAGS) $(JACK_CFLAGS)
gsequencer_static_LDFLAGS= -ldl -lm -lrt -pthread $(LDFLAGS) $(UUID_LIBS) $(LIBASOUND2_LIBS) $(LIBAO_LIBS) $(LIBXML2_LIBS) $(OPENSSL_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(FONTCONFIG_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS) $(XMLRPC_LIBS) $(XMLRPC_UTIL_LIBS) $(XMLRPC_SERVER_ABYSS_LIBS) $(XMLRPC_SERVER_CGI_LIBS) $(XMLRPC_SERVER_LIBS) $(JACK_LIBS)
gsequencer_static_LDADD = libgsequencer.la libags_audio.la libags_server.la libags_gui.la libags_thread.la libags.la
gsequencer_static_LDFLAGS = -static -ldl

# libags all files
deprecated_libags_h_sources =

libags_h_sources = \
	$(deprecated_libags_h_sources) \
	ags/lib/ags_complex.h \
	ags/lib/ags_function.h \
	ags/lib/ags_parameter.h \
	ags/lib/ags_turtle.h

deprecated_libags_c_sources =

libags_c_sources = \
	$(deprecated_libags_c_sources) \
	ags/lib/ags_complex.c \
	ags/lib/ags_function.c \
	ags/lib/ags_parameter.c \
	ags/lib/ags_turtle.c

deprecated_libags_util_h_sources =

# libags - util
libags_util_h_sources = \
	$(deprecated_libags_util_h_sources) \
	ags/util/ags_id_generator.h

deprecated_libags_util_c_sources =

libags_util_c_sources = \
	$(deprecated_libags_util_c_sources) \
	ags/util/ags_id_generator.c

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
	ags/object/ags_concurrent_tree.h \
	ags/object/ags_distributed_manager.h \
	ags/object/ags_dynamic_connectable.h \
	ags/object/ags_main_loop.h \
	ags/object/ags_marshal.h \
	ags/object/ags_mutable.h \
	ags/object/ags_packable.h \
	ags/object/ags_plugin.h \
	ags/object/ags_portlet.h \
	ags/object/ags_seekable.h \
	ags/object/ags_sequencer.h \
	ags/object/ags_soundcard.h \
	ags/object/ags_tactable.h \
	ags/object/ags_tree_iterator.h

deprecated_libags_object_c_sources =

libags_object_c_sources = \
	$(deprecated_libags_object_c_sources) \
	ags/object/ags_applicable.c \
	ags/object/ags_application_context.c \
	ags/object/ags_async_queue.c \
	ags/object/ags_config.c \
	ags/object/ags_connectable.c \
	ags/object/ags_countable.c \
	ags/object/ags_concurrent_tree.c \
	ags/object/ags_distributed_manager.c \
	ags/object/ags_dynamic_connectable.c \
	ags/object/ags_main_loop.c \
	ags/object/ags_marshal.c \
	ags/object/ags_mutable.c \
	ags/object/ags_packable.c \
	ags/object/ags_plugin.c \
	ags/object/ags_portlet.c \
	ags/object/ags_seekable.c \
	ags/object/ags_sequencer.c \
	ags/object/ags_soundcard.c \
	ags/object/ags_tactable.c \
	ags/object/ags_tree_iterator.c

deprecated_libags_file_h_sources =

# libags - file
libags_file_h_sources = \
	$(deprecated_libags_file_h_sources) \
	ags/file/ags_file.h \
	ags/file/ags_file_id_ref.h \
	ags/file/ags_file_launch.h \
	ags/file/ags_file_link.h \
	ags/file/ags_file_lookup.h \
	ags/file/ags_file_read.h \
	ags/file/ags_file_stock.h \
	ags/file/ags_file_util.h \
	ags/file/ags_file_write.h

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
	ags/thread/ags_history.h \
	ags/thread/ags_mutex_manager.h \
	ags/thread/ags_returnable_thread.h \
	ags/thread/ags_single_thread.h \
	ags/thread/ags_task_completion.h \
	ags/thread/ags_task.h \
	ags/thread/ags_task_thread.h \
	ags/thread/ags_thread_application_context.h \
	ags/thread/ags_thread_pool.h \
	ags/thread/ags_thread-posix.h \
	ags/thread/ags_timestamp_factory.h \
	ags/thread/ags_timestamp_thread.h \
	ags/thread/ags_timestamp.h

deprecated_libags_thread_c_sources =

libags_thread_c_sources = \
	$(deprecated_libags_thread_c_sources) \
	ags/thread/ags_autosave_thread.c \
	ags/thread/ags_concurrency_provider.c \
	ags/thread/ags_history.c \
	ags/thread/ags_mutex_manager.c \
	ags/thread/ags_returnable_thread.c \
	ags/thread/ags_single_thread.c \
	ags/thread/ags_task_completion.c \
	ags/thread/ags_task.c \
	ags/thread/ags_task_thread.c \
	ags/thread/ags_thread_application_context.c \
	ags/thread/ags_thread_pool.c \
	ags/thread/ags_thread-posix.c \
	ags/thread/ags_timestamp_factory.c \
	ags/thread/ags_timestamp_thread.c \
	ags/thread/ags_timestamp.c

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
	ags/server/ags_remote_task.h \
	ags/server/ags_server.h \
	ags/server/ags_server_application_context.h \
	ags/server/ags_service_provider.h

deprecated_libags_server_c_sources =

libags_server_c_sources = \
	$(deprecated_libags_server_c_sources) \
	ags/server/ags_registry.c \
	ags/server/ags_remote_task.c \
	ags/server/ags_server.c \
	ags/server/ags_server_application_context.c \
	ags/server/ags_service_provider.c

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
	ags/audio/ags_acceleration.h \
	ags/audio/ags_audio.h \
	ags/audio/ags_audio_application_context.h \
	ags/audio/ags_audio_signal.h \
	ags/audio/ags_automation.h \
	ags/audio/ags_channel.h \
	ags/audio/ags_channel_iter.h \
	ags/audio/ags_devout.h \
	ags/audio/ags_effect.h \
	ags/audio/ags_input.h \
	ags/audio/ags_libao.h \
	ags/audio/ags_message.h \
	ags/audio/ags_midiin.h \
	ags/audio/ags_notation.h \
	ags/audio/ags_note.h \
	ags/audio/ags_output.h \
	ags/audio/ags_pattern.h \
	ags/audio/ags_playable.h \
	ags/audio/ags_playback.h \
	ags/audio/ags_playback_domain.h \
	ags/audio/ags_port.h \
	ags/audio/ags_recall_audio.h \
	ags/audio/ags_recall_audio_run.h \
	ags/audio/ags_recall_audio_signal.h \
	ags/audio/ags_recall_channel.h \
	ags/audio/ags_recall_channel_run_dummy.h \
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
	ags/audio/ags_recall_recycling_dummy.h \
	ags/audio/ags_recall_recycling.h \
	ags/audio/ags_recycling_context.h \
	ags/audio/ags_recycling.h \
	ags/audio/ags_sound_provider.h \
	ags/audio/ags_synths.h

deprecated_libags_audio_c_sources =

libags_audio_c_sources = \
	$(deprecated_libags_audio_c_sources) \
	ags/audio/ags_acceleration.c \
	ags/audio/ags_audio.c \
	ags/audio/ags_audio_application_context.c \
	ags/audio/ags_audio_signal.c \
	ags/audio/ags_automation.c \
	ags/audio/ags_channel.c \
	ags/audio/ags_channel_iter.c \
	ags/audio/ags_devout.c \
	ags/audio/ags_input.c \
	ags/audio/ags_libao.c \
	ags/audio/ags_message.c \
	ags/audio/ags_midiin.c \
	ags/audio/ags_notation.c \
	ags/audio/ags_note.c \
	ags/audio/ags_output.c \
	ags/audio/ags_pattern.c \
	ags/audio/ags_playable.c \
	ags/audio/ags_playback.c \
	ags/audio/ags_playback_domain.c \
	ags/audio/ags_port.c \
	ags/audio/ags_recall_audio.c \
	ags/audio/ags_recall_audio_run.c \
	ags/audio/ags_recall_audio_signal.c \
	ags/audio/ags_recall.c \
	ags/audio/ags_recall_channel.c \
	ags/audio/ags_recall_channel_run.c \
	ags/audio/ags_recall_channel_run_dummy.c \
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
	ags/audio/ags_recall_recycling_dummy.c \
	ags/audio/ags_recycling.c \
	ags/audio/ags_recycling_context.c \
	ags/audio/ags_sound_provider.c \
	ags/audio/ags_synths.c

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
	ags/audio/thread/ags_iterator_thread.h \
	ags/audio/thread/ags_record_thread.h \
	ags/audio/thread/ags_recycling_thread_callbacks.h \
	ags/audio/thread/ags_recycling_thread.h

deprecated_libags_audio_thread_c_sources =

libags_audio_thread_c_sources = \
	$(deprecated_libags_audio_thread_c_sources) \
	ags/audio/thread/ags_audio_loop.c \
	ags/audio/thread/ags_audio_thread.c \
	ags/audio/thread/ags_channel_thread.c \
	ags/audio/thread/ags_sequencer_thread.c \
	ags/audio/thread/ags_soundcard_thread.c \
	ags/audio/thread/ags_export_thread.c \
	ags/audio/thread/ags_iterator_thread.c \
	ags/audio/thread/ags_record_thread.c \
	ags/audio/thread/ags_recycling_thread.c \
	ags/audio/thread/ags_recycling_thread_callbacks.c

# libags-audio - plugin
deprecated_libags_plugin_h_sources =

libags_plugin_h_sources = \
	$(deprecated_libags_plugin_h_sources) \
	ags/plugin/ags_dssi_manager.h \
	ags/plugin/ags_ladspa_manager.h \
	ags/plugin/ags_plugin_factory.h \
	ags/plugin/ags_lv2_manager.h \
	ags/plugin/ags_lv2ui_manager.h \
	ags/plugin/ags_plugin_stock.h

deprecated_libags_plugin_c_sources =

libags_plugin_c_sources = \
	$(deprecated_libags_plugin_c_sources) \
	ags/plugin/ags_dssi_manager.c \
	ags/plugin/ags_ladspa_manager.c \
	ags/plugin/ags_lv2_manager.c \
	ags/plugin/ags_lv2ui_manager.c \
	ags/plugin/ags_plugin_factory.c

# libags-audio - file
deprecated_libags_audio_file_h_sources =

libags_audio_file_h_sources = \
	$(deprecated_libags_audio_file_h_sources) \
	ags/audio/file/ags_audio_file.h \
	ags/audio/file/ags_audio_file_link.h \
	ags/audio/file/ags_audio_file_xml.h \
	ags/audio/file/ags_ipatch.h \
	ags/audio/file/ags_ipatch_dls2_reader.h \
	ags/audio/file/ags_ipatch_sf2_reader.h \
	ags/audio/file/ags_sndfile.h

deprecated_libags_audio_file_c_sources =

libags_audio_file_c_sources = \
	$(deprecated_libags_audio_file_c_sources) \
	ags/audio/file/ags_audio_file.c \
	ags/audio/file/ags_audio_file_link.c \
	ags/audio/file/ags_audio_file_xml.c \
	ags/audio/file/ags_ipatch.c \
	ags/audio/file/ags_ipatch_dls2_reader.c \
	ags/audio/file/ags_ipatch_sf2_reader.c \
	ags/audio/file/ags_sndfile.c

# libags-audio - midi
deprecated_libags_audio_midi_h_sources =

libags_audio_midi_h_sources = \
	$(deprecated_libags_audio_midi_h_sources) \
	ags/audio/midi/ags_midi_buffer_util.h \
	ags/audio/midi/ags_midi_util.h \
	ags/audio/midi/ags_midi_builder.h \
	ags/audio/midi/ags_midi_file.h \
	ags/audio/midi/ags_midi_parser.h \
	ags/audio/midi/ags_midi_file_reader.h \
	ags/audio/midi/ags_midi_file_writer.h

deprecated_libags_audio_midi_c_sources =

libags_audio_midi_c_sources = \
	$(deprecated_libags_audio_midi_c_sources) \
	ags/audio/midi/ags_midi_buffer_util.c \
	ags/audio/midi/ags_midi_util.c \
	ags/audio/midi/ags_midi_builder.c \
	ags/audio/midi/ags_midi_file.c \
	ags/audio/midi/ags_midi_parser.c \
	ags/audio/midi/ags_midi_file_reader.c \
	ags/audio/midi/ags_midi_file_writer.c

# libags-audio - jack
deprecated_libags_audio_jack_h_sources =

libags_audio_jack_h_sources = \
	$(deprecated_libags_audio_jack_h_sources) \
	ags/audio/jack/ags_jack_client.h \
	ags/audio/jack/ags_jack_midiin.h \
	ags/audio/jack/ags_jack_devout.h \
	ags/audio/jack/ags_jack_port.h \
	ags/audio/jack/ags_jack_server.h

deprecated_libags_audio_jack_c_sources =

libags_audio_jack_c_sources = \
	$(deprecated_libags_audio_jack_c_sources) \
	ags/audio/jack/ags_jack_client.c \
	ags/audio/jack/ags_jack_midiin.c \
	ags/audio/jack/ags_jack_devout.c \
	ags/audio/jack/ags_jack_port.c \
	ags/audio/jack/ags_jack_server.c

# libags-audio - recall
deprecated_libags_audio_recall_h_sources =

libags_audio_recall_h_sources = \
	$(deprecated_libags_audio_recall_h_sources) \
	ags/audio/recall/ags_buffer_audio_signal.h \
	ags/audio/recall/ags_buffer_channel.h \
	ags/audio/recall/ags_buffer_channel_run.h \
	ags/audio/recall/ags_buffer_recycling.h \
	ags/audio/recall/ags_copy_audio_signal.h \
	ags/audio/recall/ags_copy_channel.h \
	ags/audio/recall/ags_copy_channel_run.h \
	ags/audio/recall/ags_copy_notation_audio.h \
	ags/audio/recall/ags_copy_notation_audio_run.h \
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
	ags/audio/recall/ags_perform_automation_audio.h \
	ags/audio/recall/ags_perform_automation_audio_run.h \
	ags/audio/recall/ags_play_audio_file.h \
	ags/audio/recall/ags_play_audio.h \
	ags/audio/recall/ags_play_audio_signal.h \
	ags/audio/recall/ags_play_channel.h \
	ags/audio/recall/ags_play_channel_run.h \
	ags/audio/recall/ags_play_channel_run_master.h \
	ags/audio/recall/ags_play_notation_audio.h \
	ags/audio/recall/ags_play_notation_audio_run.h \
	ags/audio/recall/ags_play_note.h \
	ags/audio/recall/ags_play_recycling.h \
	ags/audio/recall/ags_record_midi_audio.h \
	ags/audio/recall/ags_record_midi_audio_run.h \
	ags/audio/recall/ags_route_dssi_audio.h \
	ags/audio/recall/ags_route_dssi_audio_run.h \
	ags/audio/recall/ags_route_lv2_audio.h \
	ags/audio/recall/ags_route_lv2_audio_run.h \
	ags/audio/recall/ags_stream_audio_signal.h \
	ags/audio/recall/ags_stream_channel.h \
	ags/audio/recall/ags_stream_channel_run.h \
	ags/audio/recall/ags_stream.h \
	ags/audio/recall/ags_stream_recycling.h \
	ags/audio/recall/ags_volume_audio_signal.h \
	ags/audio/recall/ags_volume_channel.h \
	ags/audio/recall/ags_volume_channel_run.h \
	ags/audio/recall/ags_volume_recycling.h

deprecated_libags_audio_recall_c_sources =

libags_audio_recall_c_sources = \
	$(deprecated_libags_audio_recall_c_sources) \
	ags/audio/recall/ags_buffer_audio_signal.c \
	ags/audio/recall/ags_buffer_channel.c \
	ags/audio/recall/ags_buffer_channel_run.c \
	ags/audio/recall/ags_buffer_recycling.c \
	ags/audio/recall/ags_copy_audio_signal.c \
	ags/audio/recall/ags_copy_channel.c \
	ags/audio/recall/ags_copy_channel_run.c \
	ags/audio/recall/ags_copy_notation_audio.c \
	ags/audio/recall/ags_copy_notation_audio_run.c \
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
	ags/audio/recall/ags_perform_automation_audio.c \
	ags/audio/recall/ags_perform_automation_audio_run.c \
	ags/audio/recall/ags_play_audio.c \
	ags/audio/recall/ags_play_audio_file.c \
	ags/audio/recall/ags_play_audio_signal.c \
	ags/audio/recall/ags_play_channel.c \
	ags/audio/recall/ags_play_channel_run.c \
	ags/audio/recall/ags_play_channel_run_master.c \
	ags/audio/recall/ags_play_notation_audio.c \
	ags/audio/recall/ags_play_notation_audio_run.c \
	ags/audio/recall/ags_play_note.c \
	ags/audio/recall/ags_play_recycling.c \
	ags/audio/recall/ags_record_midi_audio.c \
	ags/audio/recall/ags_record_midi_audio_run.c \
	ags/audio/recall/ags_route_dssi_audio.c \
	ags/audio/recall/ags_route_dssi_audio_run.c \
	ags/audio/recall/ags_route_lv2_audio.c \
	ags/audio/recall/ags_route_lv2_audio_run.c \
	ags/audio/recall/ags_stream_audio_signal.c \
	ags/audio/recall/ags_stream.c \
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
	ags/audio/task/ags_add_point_to_selection.h \
	ags/audio/task/ags_add_recall_container.h \
	ags/audio/task/ags_add_recall.h \
	ags/audio/task/ags_add_region_to_selection.h \
	ags/audio/task/ags_append_audio.h \
	ags/audio/task/ags_append_audio_threaded.h \
	ags/audio/task/ags_append_channel.h \
	ags/audio/task/ags_append_recall.h \
	ags/audio/task/ags_apply_synth.h \
	ags/audio/task/ags_apply_wavetable.h \
	ags/audio/task/ags_audio_set_recycling.h \
	ags/audio/task/ags_cancel_audio.h \
	ags/audio/task/ags_cancel_channel.h \
	ags/audio/task/ags_cancel_recall.h \
	ags/audio/task/ags_channel_set_recycling.h \
	ags/audio/task/ags_export_output.h \
	ags/audio/task/ags_free_selection.h \
	ags/audio/task/ags_init_audio.h \
	ags/audio/task/ags_init_channel.h \
	ags/audio/task/ags_link_channel.h \
	ags/audio/task/ags_open_file.h \
	ags/audio/task/ags_open_single_file.h \
	ags/audio/task/ags_remove_audio.h \
	ags/audio/task/ags_remove_audio_signal.h \
	ags/audio/task/ags_remove_note.h \
	ags/audio/task/ags_remove_point_from_selection.h \
	ags/audio/task/ags_remove_recall_container.h \
	ags/audio/task/ags_remove_recall.h \
	ags/audio/task/ags_remove_region_from_selection.h \
	ags/audio/task/ags_resize_audio.h \
	ags/audio/task/ags_save_file.h \
	ags/audio/task/ags_seek_soundcard.h \
	ags/audio/task/ags_set_audio_channels.h \
	ags/audio/task/ags_set_buffer_size.h \
	ags/audio/task/ags_set_output_device.h \
	ags/audio/task/ags_set_samplerate.h \
	ags/audio/task/ags_start_soundcard.h \
	ags/audio/task/ags_toggle_pattern_bit.h \
	ags/audio/task/ags_unref_audio_signal.h \
	ags/audio/task/recall/ags_apply_bpm.h \
	ags/audio/task/recall/ags_apply_sequencer_length.h \
	ags/audio/task/recall/ags_apply_tact.h \
	ags/audio/task/recall/ags_set_muted.h

deprecated_libags_audio_task_c_sources =

libags_audio_task_c_sources = \
	$(deprecated_libags_audio_task_c_sources) \
	ags/audio/task/ags_add_audio.c \
	ags/audio/task/ags_add_audio_signal.c \
	ags/audio/task/ags_add_effect.c \
	ags/audio/task/ags_add_note.c \
	ags/audio/task/ags_add_point_to_selection.c \
	ags/audio/task/ags_add_recall.c \
	ags/audio/task/ags_add_recall_container.c \
	ags/audio/task/ags_add_region_to_selection.c \
	ags/audio/task/ags_append_audio.c \
	ags/audio/task/ags_append_audio_threaded.c \
	ags/audio/task/ags_append_channel.c \
	ags/audio/task/ags_append_recall.c \
	ags/audio/task/ags_apply_synth.c \
	ags/audio/task/ags_apply_wavetable.c \
	ags/audio/task/ags_audio_set_recycling.c \
	ags/audio/task/ags_cancel_audio.c \
	ags/audio/task/ags_cancel_channel.c \
	ags/audio/task/ags_cancel_recall.c \
	ags/audio/task/ags_channel_set_recycling.c \
	ags/audio/task/ags_export_output.c \
	ags/audio/task/ags_free_selection.c \
	ags/audio/task/ags_init_audio.c \
	ags/audio/task/ags_init_channel.c \
	ags/audio/task/ags_link_channel.c \
	ags/audio/task/ags_open_file.c \
	ags/audio/task/ags_open_single_file.c \
	ags/audio/task/ags_remove_audio.c \
	ags/audio/task/ags_remove_audio_signal.c \
	ags/audio/task/ags_remove_note.c \
	ags/audio/task/ags_remove_point_from_selection.c \
	ags/audio/task/ags_remove_recall.c \
	ags/audio/task/ags_remove_recall_container.c \
	ags/audio/task/ags_remove_region_from_selection.c \
	ags/audio/task/ags_resize_audio.c \
	ags/audio/task/ags_save_file.c \
	ags/audio/task/ags_seek_soundcard.c \
	ags/audio/task/ags_set_audio_channels.c \
	ags/audio/task/ags_set_buffer_size.c \
	ags/audio/task/ags_set_output_device.c \
	ags/audio/task/ags_set_samplerate.c \
	ags/audio/task/ags_start_soundcard.c \
	ags/audio/task/ags_toggle_pattern_bit.c \
	ags/audio/task/ags_unref_audio_signal.c \
	ags/audio/task/recall/ags_apply_bpm.c \
	ags/audio/task/recall/ags_apply_sequencer_length.c \
	ags/audio/task/recall/ags_apply_tact.c \
	ags/audio/task/recall/ags_set_muted.c

# libags-gui all files
deprecated_libags_gui_h_sources =

libags_gui_h_sources = \
	$(deprecated_libags_gui_h_sources) \
	ags/widget/ags_cartesian.h \
	ags/widget/ags_dial.h \
	ags/widget/ags_expander.h \
	ags/widget/ags_expander_set.h \
	ags/widget/ags_hindicator.h \
	ags/widget/ags_htimebar.h \
	ags/widget/ags_indicator.h \
	ags/widget/ags_led.h \
	ags/widget/ags_menu.h \
	ags/widget/ags_menu_item.h \
	ags/widget/ags_option_menu.h \
	ags/widget/ags_ruler.h \
	ags/widget/ags_segment.h \
	ags/widget/ags_table.h \
	ags/widget/ags_timebar.h \
	ags/widget/ags_vindicator.h \
	ags/widget/ags_waveform.h \
	ags/widget/ags_container.h

deprecated_libags_gui_c_sources =

libags_gui_c_sources = \
	$(deprecated_libags_gui_c_sources) \
	ags/widget/ags_cartesian.c \
	ags/widget/ags_dial.c \
	ags/widget/ags_expander.c \
	ags/widget/ags_expander_set.c \
	ags/widget/ags_hindicator.c \
	ags/widget/ags_htimebar.c \
	ags/widget/ags_indicator.c \
	ags/widget/ags_led.c \
	ags/widget/ags_menu.c \
	ags/widget/ags_menu_item.c \
	ags/widget/ags_option_menu.c \
	ags/widget/ags_ruler.c \
	ags/widget/ags_segment.c \
	ags/widget/ags_table.c \
	ags/widget/ags_timebar.c \
	ags/widget/ags_vindicator.c \
	ags/widget/ags_waveform.c \
	ags/widget/ags_container.c

# libgsequencer all files
deprecated_libgsequencer_h_sources =

libgsequencer_h_sources = \
	$(deprecated_libgsequencer_h_sources) \
	ags/X/ags_automation_editor_callbacks.h \
	ags/X/ags_automation_editor.h \
	ags/X/ags_automation_window_callbacks.h \
	ags/X/ags_automation_window.h \
	ags/X/ags_audio_preferences_callbacks.h \
	ags/X/ags_audio_preferences.h \
	ags/X/ags_bulk_member_callbacks.h \
	ags/X/ags_bulk_member.h \
	ags/X/ags_dssi_browser_callbacks.h \
	ags/X/ags_dssi_browser.h \
	ags/X/ags_editor_callbacks.h \
	ags/X/ags_editor.h \
	ags/X/ags_effect_bridge_callbacks.h \
	ags/X/ags_effect_bridge.h \
	ags/X/ags_effect_bulk_callbacks.h \
	ags/X/ags_effect_bulk.h \
	ags/X/ags_effect_line_callbacks.h \
	ags/X/ags_effect_line.h \
	ags/X/ags_effect_pad_callbacks.h \
	ags/X/ags_effect_pad.h \
	ags/X/ags_export_window_callbacks.h \
	ags/X/ags_export_window.h \
	ags/X/ags_generic_preferences_callbacks.h \
	ags/X/ags_generic_preferences.h \
	ags/X/ags_history_browser_callbacks.h \
	ags/X/ags_history_browser.h \
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
	ags/X/ags_menu_bar_callbacks.h \
	ags/X/ags_menu_bar.h \
	ags/X/ags_midi_dialog_callbacks.h \
	ags/X/ags_midi_dialog.h \
	ags/X/ags_navigation_callbacks.h \
	ags/X/ags_navigation.h \
	ags/X/ags_pad_callbacks.h \
	ags/X/ags_pad_editor_callbacks.h \
	ags/X/ags_pad_editor.h \
	ags/X/ags_pad.h \
	ags/X/ags_performance_preferences_callbacks.h \
	ags/X/ags_performance_preferences.h \
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
	ags/X/ags_server_preferences_callbacks.h \
	ags/X/ags_server_preferences.h \
	ags/X/ags_window.h \
	ags/X/ags_window_callbacks.h \
	ags/X/ags_xorg_application_context.h

deprecated_libgsequencer_c_sources =

libgsequencer_c_sources = \
	$(deprecated_libgsequencer_c_sources) \
	ags/X/ags_automation_editor.c \
	ags/X/ags_automation_editor_callbacks.c \
	ags/X/ags_automation_window.c \
	ags/X/ags_automation_window_callbacks.c \
	ags/X/ags_audio_preferences.c \
	ags/X/ags_audio_preferences_callbacks.c \
	ags/X/ags_bulk_member.c \
	ags/X/ags_bulk_member_callbacks.c \
	ags/X/ags_dssi_browser_callbacks.c \
	ags/X/ags_dssi_browser.c \
	ags/X/ags_editor.c \
	ags/X/ags_editor_callbacks.c \
	ags/X/ags_effect_bridge.c \
	ags/X/ags_effect_bridge_callbacks.c \
	ags/X/ags_effect_bulk.c \
	ags/X/ags_effect_bulk_callbacks.c \
	ags/X/ags_effect_line.c \
	ags/X/ags_effect_line_callbacks.c \
	ags/X/ags_effect_pad.c \
	ags/X/ags_effect_pad_callbacks.c \
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
	ags/X/ags_menu_bar.c \
	ags/X/ags_menu_bar_callbacks.c \
	ags/X/ags_midi_dialog.c \
	ags/X/ags_midi_dialog_callbacks.c \
	ags/X/ags_navigation.c \
	ags/X/ags_navigation_callbacks.c \
	ags/X/ags_pad.c \
	ags/X/ags_pad_callbacks.c \
	ags/X/ags_pad_editor.c \
	ags/X/ags_pad_editor_callbacks.c \
	ags/X/ags_performance_preferences.c \
	ags/X/ags_performance_preferences_callbacks.c \
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
	ags/X/ags_server_preferences.c \
	ags/X/ags_server_preferences_callbacks.c \
	ags/X/ags_window.c \
	ags/X/ags_window_callbacks.c \
	ags/X/ags_xorg_application_context.c

# libgsequencer - file
deprecated_libgsequencer_file_h_sources =

libgsequencer_file_h_sources = \
	$(deprecated_libgsequencer_file_h_sources) \
	ags/X/file/ags_gui_file_xml.h

deprecated_libgsequencer_file_c_sources =

libgsequencer_file_c_sources = \
	$(deprecated_libgsequencer_file_c_sources) \
	ags/X/file/ags_gui_file_xml.c

# libgsequencer-thread - thread
deprecated_libgsequencer_thread_h_sources =

libgsequencer_thread_h_sources = \
	$(deprecated_libgsequencer_thread_h_sources) \
	ags/X/thread/ags_gui_thread.h

deprecated_libgsequencer_thread_c_sources =

libgsequencer_thread_c_sources = \
	$(deprecated_libgsequencer_thread_c_sources) \
	ags/X/thread/ags_gui_thread.c

# libgsequencer - task
deprecated_libgsequencer_task_h_sources =

libgsequencer_task_h_sources = \
	$(deprecated_libgsequencer_task_h_sources) \
	ags/X/task/ags_add_bulk_member.h \
	ags/X/task/ags_add_line_member.h \
	ags/X/task/ags_blink_cell_pattern_cursor.h \
	ags/X/task/ags_change_indicator.h \
	ags/X/task/ags_change_tact.h \
	ags/X/task/ags_display_tact.h \
	ags/X/task/ags_scroll_on_play.h \
	ags/X/task/ags_toggle_led.h \
	ags/X/task/ags_update_bulk_member.h

deprecated_libgsequencer_task_c_sources =

libgsequencer_task_c_sources = \
	$(deprecated_libgsequencer_task_c_sources) \
	ags/X/task/ags_add_bulk_member.c \
	ags/X/task/ags_add_line_member.c \
	ags/X/task/ags_blink_cell_pattern_cursor.c \
	ags/X/task/ags_change_indicator.c \
	ags/X/task/ags_change_tact.c \
	ags/X/task/ags_display_tact.c \
	ags/X/task/ags_scroll_on_play.c \
	ags/X/task/ags_toggle_led.c \
	ags/X/task/ags_update_bulk_member.c

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

# libgsequencer - machine
deprecated_libgsequencer_machine_h_sources =

libgsequencer_machine_h_sources = \
	$(deprecated_libgsequencer_machine_h_sources) \
	ags/X/machine/ags_cell_pattern_callbacks.h \
	ags/X/machine/ags_cell_pattern.h \
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
	ags/X/machine/ags_ladspa_bridge_callbacks.h \
	ags/X/machine/ags_ladspa_bridge.h \
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
	ags/X/machine/ags_panel_input_pad.h \
	ags/X/machine/ags_pattern_box_callbacks.h \
	ags/X/machine/ags_pattern_box.h \
	ags/X/machine/ags_synth_callbacks.h \
	ags/X/machine/ags_synth.h \
	ags/X/machine/ags_synth_output_line_callbacks.h \
	ags/X/machine/ags_synth_output_line.h \
	ags/X/machine/ags_synth_output_pad.h \
	ags/X/machine/ags_synth_output_pad_callbacks.h \
	ags/X/machine/ags_synth_input_line.h \
	ags/X/machine/ags_synth_input_pad.h

deprecated_libgsequencer_machine_c_sources =

libgsequencer_machine_c_sources = \
	$(deprecated_libgsequencer_machine_c_sources) \
	ags/X/machine/ags_cell_pattern.c \
	ags/X/machine/ags_cell_pattern_callbacks.c \
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
	ags/X/machine/ags_ladspa_bridge.c \
	ags/X/machine/ags_ladspa_bridge_callbacks.c \
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
	ags/X/machine/ags_panel_input_pad.c \
	ags/X/machine/ags_pattern_box_callbacks.c \
	ags/X/machine/ags_pattern_box.c \
	ags/X/machine/ags_synth.c \
	ags/X/machine/ags_synth_callbacks.c \
	ags/X/machine/ags_synth_input_line.c \
	ags/X/machine/ags_synth_input_pad.c

# libgsequencer - editor
deprecated_libgsequencer_editor_h_sources =

libgsequencer_editor_h_sources = \
	$(deprecated_libgsequencer_editor_h_sources) \
	ags/X/editor/ags_automation_edit_callbacks.h \
	ags/X/editor/ags_automation_edit.h \
	ags/X/editor/ags_automation_toolbar_callbacks.h \
	ags/X/editor/ags_automation_toolbar.h \
	ags/X/editor/ags_automation_area_callbacks.h \
	ags/X/editor/ags_automation_area.h \
	ags/X/editor/ags_file_selection_callbacks.h \
	ags/X/editor/ags_file_selection.h \
	ags/X/editor/ags_inline_player_callbacks.h \
	ags/X/editor/ags_inline_player.h \
	ags/X/editor/ags_machine_radio_button.h \
	ags/X/editor/ags_machine_selection.h \
	ags/X/editor/ags_machine_selector_callbacks.h \
	ags/X/editor/ags_machine_selector.h \
	ags/X/editor/ags_meter_callbacks.h \
	ags/X/editor/ags_meter.h \
	ags/X/editor/ags_notebook_callbacks.h \
	ags/X/editor/ags_notebook.h \
	ags/X/editor/ags_note_edit_callbacks.h \
	ags/X/editor/ags_note_edit.h \
	ags/X/editor/ags_pattern_edit_callbacks.h \
	ags/X/editor/ags_pattern_edit.h \
	ags/X/editor/ags_scale_area_callbacks.h \
	ags/X/editor/ags_scale_area.h \
	ags/X/editor/ags_scale_callbacks.h \
	ags/X/editor/ags_scale.h \
	ags/X/editor/ags_sf2_chooser_callbacks.h \
	ags/X/editor/ags_sf2_chooser.h \
	ags/X/editor/ags_toolbar_callbacks.h \
	ags/X/editor/ags_toolbar.h \
	ags/X/editor/ags_toolbar_mode_stock.h

deprecated_libgsequencer_editor_c_sources =

libgsequencer_editor_c_sources = \
	$(deprecated_libgsequencer_editor_c_sources) \
	ags/X/editor/ags_automation_edit.c \
	ags/X/editor/ags_automation_edit_callbacks.c \
	ags/X/editor/ags_automation_toolbar.c \
	ags/X/editor/ags_automation_toolbar_callbacks.c \
	ags/X/editor/ags_automation_area.c \
	ags/X/editor/ags_automation_area_callbacks.c \
	ags/X/editor/ags_file_selection.c \
	ags/X/editor/ags_file_selection_callbacks.c \
	ags/X/editor/ags_inline_player.c \
	ags/X/editor/ags_inline_player_callbacks.c \
	ags/X/editor/ags_machine_radio_button.c \
	ags/X/editor/ags_machine_selection.c \
	ags/X/editor/ags_machine_selector.c \
	ags/X/editor/ags_machine_selector_callbacks.c \
	ags/X/editor/ags_meter.c \
	ags/X/editor/ags_meter_callbacks.c \
	ags/X/editor/ags_notebook.c \
	ags/X/editor/ags_notebook_callbacks.c \
	ags/X/editor/ags_note_edit.c \
	ags/X/editor/ags_note_edit_callbacks.c \
	ags/X/editor/ags_pattern_edit.c \
	ags/X/editor/ags_pattern_edit_callbacks.c \
	ags/X/editor/ags_scale_area.c \
	ags/X/editor/ags_scale_area_callbacks.c \
	ags/X/editor/ags_scale.c \
	ags/X/editor/ags_scale_callbacks.c \
	ags/X/editor/ags_sf2_chooser.c \
	ags/X/editor/ags_sf2_chooser_callbacks.c \
	ags/X/editor/ags_toolbar.c \
	ags/X/editor/ags_toolbar_callbacks.c

# libgsequencer all files
gsequencerheaders = \
	ags/gsequencer_main.h

gsequencer_c_sources = \
	ags/gsequencer_main.c

midi2xml_CFLAGS= $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
midi2xml_LDFLAGS= -ldl $(LDFLAGS) $(LIBXML2_LIBS) $(GOBJECT_LIBS)

midi2xml_SOURCES = ags/midi2xml_main.c ags/audio/midi/ags_midi_parser.c ags/audio/midi/ags_midi_parser.h ags/object/ags_marshal.c ags/object/ags_marshal.h

ags-docs-local:
	cd docs/reference/ags
	make
	gtkdoc-fixxref
	cd $(top_srcdir)

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool

gsequencer.1:
	xsltproc --nonet --param make.year.ranges 1 --param make.single.year.ranges 1 --param man.charmap.use.subset 0 -o ./ http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl gsequencer.1.xml

midi2xml.1:
	xsltproc --nonet --param make.year.ranges 1 --param make.single.year.ranges 1 --param man.charmap.use.subset 0 -o ./ http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl midi2xml.1.xml

gsequencer.desktop: gsequencer.desktop.in
	$(AM_V_GEN)sed -e 's,\@datadir\@,$(datadir),g' < "$<" > $@

html:
	mkdir -p html/
	xsltproc --output html/ --xinclude /usr/share/xml/docbook/stylesheet/docbook-xsl/htmlhelp/htmlhelp.xsl $(top_srcdir)/doc/usersBook.xml

install-data-local:
	mkdir -p $(DESTDIR)/$(datadir)/doc-base
	mkdir -p $(DESTDIR)/$(datadir)/gsequencer/icons
	mkdir -p $(DESTDIR)/$(datadir)/gsequencer/images
	mkdir -p $(DESTDIR)/$(datadir)/xml/gsequencer/{schema/dtd/0.4.2/,stylesheet/ags-xsl/midi-xml/}
	cp $(top_srcdir)/gsequencer.share/icons/ags.xpm $(DESTDIR)/$(datadir)/gsequencer/icons/ags.xpm
	cp $(top_srcdir)/gsequencer.share/images/ags.png $(DESTDIR)/$(datadir)/gsequencer/images/ags.png
	cp $(top_srcdir)/ags/file/ags_file.dtd $(DESTDIR)/$(datadir)/xml/gsequencer/schema/dtd/0.4.2/
	cp $(top_srcdir)/ags.xsl $(DESTDIR)/$(datadir)/xml/gsequencer/stylesheet/ags-xsl/midi-xml/
	cp $(top_srcdir)/gsequencer-documentation $(DESTDIR)/$(datadir)/doc-base/
	mkdir -p $(DESTDIR)/$(datadir)/doc/gsequencer/
	gzip -9 -c $(top_srcdir)/ChangeLog > $(DESTDIR)/$(datadir)/doc/gsequencer/changelog.gz

install-html: html
	mkdir -p $(DESTDIR)/$(datadir)/doc/gsequencer/{images,html}/
	cp $(userdocimages) $(DESTDIR)/$(datadir)/doc/gsequencer/images/
	cp html/* $(DESTDIR)/$(datadir)/doc/gsequencer/html/

uninstall-hook:
	rm -rf $(DESTDIR)/$(datadir)/gsequencer
	rm -rf $(DESTDIR)/$(datadir)/doc/gsequencer
	rm -rf $(DESTDIR)/$(datadir)/xml/gsequencer
	rm -f $(DESTDIR)/$(datadir)/doc-base/gsequencer-documentation

clean-local:
	rm -rf html
