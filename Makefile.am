AUTOMAKE_OPTIONS = foreign

#
SUBDIRS = po \
	. \
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

#
libags_a_SOURCES = $(ags_util_c_sources) $(ags_lib_c_sources) $(ags_object_c_sources) $(ags_file_c_sources) $(ags_file_task_c_sources)
libags_thread_a_SOURCES = $(ags_thread_c_sources) $(ags_thread_file_c_sources) $(ags_thread_task_c_sources)
libags_log_a_SOURCES = $(ags_log_c_sources) $(ags_log_task_c_sources)
libags_audio_a_SOURCES = $(ags_audio_c_sources) $(ags_audio_thread_c_sources) $(ags_audio_file_c_sources) $(ags_audio_midi_c_sources) $(ags_audio_task_c_sources) $(ags_audio_task_recall_c_sources) $(ags_audio_recall_c_sources)  $(ags_plugin_c_sources)
libags_gui_a_SOURCES = $(ags_widget_c_sources)
gsequencer_SOURCES = $(gsequencer_main_c_sources) $(gsequencer_server_c_sources) $(gsequencer_xorg_c_sources) $(gsequencer_xorg_import_c_sources) $(gsequencer_xorg_thread_c_sources) $(gsequencer_xorg_task_c_sources) $(gsequencer_machine_c_sources) $(gsequencer_editor_c_sources) $(ags_gsequencer_task_c_sources) $(gsequencer_file_c_sources)

#
TARGETS = gsequencer

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
noinst_LIBRARIES = libags.a libags_thread.a libags_log.a libags_audio.a libags_gui.a
bin_PROGRAMS = gsequencer

# application
COMPILER_FLAGS = -g -std=gnu11
WARN_FLAGS = -Wuninitialized -Wreturn-type -Wunused-variable -Wunused-but-set-variable -Wmaybe-uninitialized -Wimplicit-function-declaration -Wparentheses -Wpointer-sign -Wdeprecated-declarations -Wcomment

libags_a_CFLAGS = @CFLAGS@ $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./src -I/usr/include $(UUID_CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
libags_a_LDFLAGS = @LDFLAGS@ -L/lib -L/usr/lib -L/usr/X11/lib -ldl -lm -pthread -lrt -lgmp  $(UUID_LIBS) $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS)

libags_thread_a_CFLAGS = @CFLAGS@ $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./src -I/usr/include $(UUID_CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
libags_thread_a_LDFLAGS = @LDFLAGS@ -L/lib -L/usr/lib -L/usr/X11/lib -ldl -lm -pthread -lrt -lgmp $(UUID_LIBS) $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS)
libags_thread_a_LIBADD = libags.a

libags_log_a_CFLAGS = @CFLAGS@ $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./src -I/usr/include $(UUID_CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
libags_log_a_LDFLAGS = @LDFLAGS@ -L/lib -L/usr/lib -L/usr/X11/lib -ldl -lm -pthread -lrt -lgmp $(UUID_LIBS) $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS)
libags_log_a_LIBADD = libags_thread.a libags.a

libags_audio_a_CFLAGS = @CFLAGS@ $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./src -I/usr/include $(UUID_CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
libags_audio_a_LDFLAGS = @LDFLAGS@ -L/lib -L/usr/lib -L/usr/X11/lib -ldl -lm -pthread -lrt -lgmp $(UUID_LIBS) $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS)
libags_audio_a_LIBADD = libags_log.a libags_thread.a libags.a

libags_gui_a_CFLAGS = @CFLAGS@ $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./src -I/usr/include $(UUID_CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
libags_gui_a_LDFLAGS = @LDFLAGS@ -L/lib -L/usr/lib -L/usr/X11/lib -ldl -lm -pthread -lrt -lgmp $(UUID_LIBS) $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS)

gsequencer_CFLAGS = @CFLAGS@ $(COMPILER_FLAGS) $(WARN_FLAGS) -O -I./src -I/usr/include $(UUID_CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
gsequencer_LDFLAGS = @LDFLAGS@ -L/lib -L/usr/lib -L/usr/X11/lib -ldl -lm -pthread -lrt -lgmp $(UUID_LIBS) $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS)
gsequencer_LDADD = libags_audio.a libags_gui.a libags_log.a libags_thread.a libags.a

ACLOCAL_AMFLAGS = -I m4

# libags all files
deprecated_ags_lib_h_sources =

ags_lib_h_sources = \
	$(deprecated_ags_lib_h_sources) \
	src/ags/lib/ags_combo_box_text.h		\
	src/ags/lib/ags_container.h			\
	src/ags/lib/ags_list.h				\
	src/ags/lib/ags_parameter.h			\
	src/ags/lib/ags_turtle.h

deprecated_ags_lib_c_sources = 

ags_lib_c_sources = \
	$(deprecated_ags_lib_c_sources) \
	src/ags/lib/ags_combo_box_text.c		\
	src/ags/lib/ags_container.c			\
	src/ags/lib/ags_list.c				\
	src/ags/lib/ags_parameter.c			\
	src/ags/lib/ags_turtle.c

deprecated_ags_util_h_sources =

ags_util_h_sources = \
	$(deprecated_ags_util_h_sources) \
	src/ags/util/ags_id_generator.h

deprecated_ags_util_c_sources = 

ags_util_c_sources = \
	$(deprecated_ags_util_c_sources) \
	src/ags/util/ags_id_generator.c

deprecated_ags_log_h_sources =

ags_log_h_sources = \
	$(deprecated_ags_log_h_sources) \
	src/ags/log/ags_log.h

deprecated_ags_log_c_sources = 

ags_log_c_sources = \
	$(deprecated_ags_log_c_sources) \
	src/ags/log/ags_log.c


deprecated_ags_log_task_h_sources =

ags_log_task_h_sources = \
	$(deprecated_ags_log_task_h_sources) \
	src/ags/log/task/ags_log_task.h

deprecated_ags_log_task_c_sources = 

ags_log_task_c_sources = \
	$(deprecated_ags_log_task_c_sources) \
	src/ags/log/task/ags_log_task.c

deprecated_ags_object_h_sources =

ags_object_h_sources = \
	$(deprecated_ags_object_h_sources) \
	src/ags/object/ags_init.h			\
	src/ags/object/ags_application_context.h	\
	src/ags/object/ags_config.h			\
	src/ags/object/ags_applicable.h			\
	src/ags/object/ags_countable.h			\
	src/ags/object/ags_connectable.h		\
	src/ags/object/ags_dynamic_connectable.h	\
	src/ags/object/ags_effect.h			\
	src/ags/object/ags_main_loop.h			\
	src/ags/object/ags_marshal.h			\
	src/ags/object/ags_mutable.h			\
	src/ags/object/ags_packable.h			\
	src/ags/object/ags_persistable.h		\
	src/ags/object/ags_plugin.h			\
	src/ags/object/ags_portlet.h			\
	src/ags/object/ags_runnable.h			\
	src/ags/object/ags_seekable.h			\
	src/ags/object/ags_soundcard.h			\
	src/ags/object/ags_stackable.h			\
	src/ags/object/ags_tactable.h			\
	src/ags/object/ags_tree_iterator.h

deprecated_ags_object_c_sources = 

ags_object_c_sources = \
	$(deprecated_ags_object_c_sources) \
	src/ags/object/ags_init.c			\
	src/ags/object/ags_application_context.c	\
	src/ags/object/ags_config.c			\
	src/ags/object/ags_applicable.c			\
	src/ags/object/ags_countable.c			\
	src/ags/object/ags_connectable.c		\
	src/ags/object/ags_dynamic_connectable.c	\
	src/ags/object/ags_effect.c			\
	src/ags/object/ags_main_loop.c			\
	src/ags/object/ags_marshal.c			\
	src/ags/object/ags_mutable.c			\
	src/ags/object/ags_packable.c			\
	src/ags/object/ags_persistable.c		\
	src/ags/object/ags_plugin.c			\
	src/ags/object/ags_portlet.c			\
	src/ags/object/ags_runnable.c			\
	src/ags/object/ags_seekable.c			\
	src/ags/object/ags_soundcard.c			\
	src/ags/object/ags_stackable.c			\
	src/ags/object/ags_tactable.c			\
	src/ags/object/ags_tree_iterator.c

deprecated_ags_file_h_sources = \
	src/ags/file/ags_file_link.h			\
	src/ags/file/ags_file_read.h			\
	src/ags/file/ags_file_value_factory.h		\
	src/ags/file/ags_file_write.h 

ags_file_h_sources = \
	$(deprecated_ags_file_h_sources) \
	src/ags/file/ags_file_stock.h		\
	src/ags/file/ags_file.h			\
	src/ags/file/ags_file_id_ref.h		\
	src/ags/file/ags_file_launch.h		\
	src/ags/file/ags_file_lookup.h		\
	src/ags/file/ags_file_util.h

deprecated_ags_file_c_sources = 

ags_file_c_sources = \
	$(deprecated_ags_file_c_sources) \
	src/ags/file/ags_file.c			\
	src/ags/file/ags_file_id_ref.c		\
	src/ags/file/ags_file_launch.c		\
	src/ags/file/ags_file_link.c		\
	src/ags/file/ags_file_lookup.c		\
	src/ags/file/ags_file_util.c

deprecated_ags_file_task_h_sources = 

ags_file_task_h_sources = \
	$(deprecated_ags_file_task_h_sources) \
	src/ags/file/task/ags_save_file.h

deprecated_ags_file_task_c_sources = 

ags_file_task_c_sources = \
	$(deprecated_ags_file_task_c_sources) \
	src/ags/file/task/ags_save_file.c

deprecated_ags_plugin_h_sources =

ags_plugin_h_sources = \
	$(deprecated_ags_plugin_h_sources) \
	src/ags/plugin/ags_ladspa_manager.h		\
	src/ags/plugin/ags_lv2_manager.h		\
	src/ags/plugin/ags_lv2ui_manager.h		\
	src/ags/plugin/ags_plugin_factory.h		\
	src/ags/plugin/ags_plugin_stock.h

deprecated_ags_plugin_c_sources = 

ags_plugin_c_sources = \
	$(deprecated_ags_plugin_c_sources) \
	src/ags/plugin/ags_ladspa_manager.c		\
	src/ags/plugin/ags_lv2_manager.c		\
	src/ags/plugin/ags_lv2ui_manager.c		\
	src/ags/plugin/ags_plugin_factory.c

# libags_thread all files
deprecated_ags_thread_h_sources = \
	src/ags/thread/ags_history.h

ags_thread_h_sources = \
	$(deprecated_ags_thread_h_sources) \
	src/ags/thread/ags_mutex_manager.h		\
	src/ags/thread/ags_thread_init.h		\
	src/ags/thread/ags_thread_application_context.h	\
	src/ags/thread/ags_concurrency_provider.h	\
	src/ags/thread/ags_autosave_thread.h		\
	src/ags/thread/ags_returnable_thread.h		\
	src/ags/thread/ags_single_thread.h		\
	src/ags/thread/ags_task_thread.h		\
	src/ags/thread/ags_task.h			\
	src/ags/thread/ags_thread_pool.h		\
	src/ags/thread/ags_thread-posix.h		\
	src/ags/thread/ags_timestamp_thread.h

deprecated_ags_thread_c_sources = 

ags_thread_c_sources = \
	$(deprecated_ags_thread_c_sources) \
	src/ags/thread/ags_mutex_manager.c		\
	src/ags/thread/ags_thread_init.c		\
	src/ags/thread/ags_thread_application_context.c	\
	src/ags/thread/ags_concurrency_provider.c	\
	src/ags/thread/ags_autosave_thread.c		\
	src/ags/thread/ags_returnable_thread.c		\
	src/ags/thread/ags_single_thread.c		\
	src/ags/thread/ags_task_thread.c		\
	src/ags/thread/ags_task.c			\
	src/ags/thread/ags_thread_pool.c		\
	src/ags/thread/ags_thread-posix.c		\
	src/ags/thread/ags_timestamp_thread.c

deprecated_ags_thread_file_h_sources =

ags_thread_file_h_sources = \
	$(deprecated_ags_thread_file_h_sources) \
	src/ags/thread/file/ags_thread_file_xml.h

deprecated_ags_thread_file_c_sources =

ags_thread_file_c_sources = \
	$(deprecated_ags_thread_file_c_sources) \
	src/ags/thread/file/ags_thread_file_xml.c

deprecated_ags_thread_task_h_sources =

ags_thread_task_h_sources = \
	$(deprecated_ags_thread_task_h_sources) \
	src/ags/thread/task/ags_start_thread.h

deprecated_ags_thread_task_c_sources =

ags_thread_task_c_sources = \
	$(deprecated_ags_thread_task_c_sources) \
	src/ags/thread/task/ags_start_thread.c

# libags_gui all files
odeprecated_ags_widget_h_sources = 

ags_widget_h_sources = \
	$(deprecated_ags_widget_h_sources) \
	src/ags/widget/ags_gui_init.h			\
	src/ags/widget/ags_cartesian.h			\
	src/ags/widget/ags_dial.h 			\
	src/ags/widget/ags_expander.h			\
	src/ags/widget/ags_expander_set.h		\
	src/ags/widget/ags_hindicator.h		\
	src/ags/widget/ags_htimebar.h			\
	src/ags/widget/ags_indicator.h			\
	src/ags/widget/ags_led.h			\
	src/ags/widget/ags_menu.h			\
	src/ags/widget/ags_menu_item.h			\
	src/ags/widget/ags_option_menu.h		\
	src/ags/widget/ags_ruler.h			\
	src/ags/widget/ags_table.h			\
	src/ags/widget/ags_timebar.h			\
	src/ags/widget/ags_vindicator.h		\
	src/ags/widget/ags_waveform.h

deprecated_ags_widget_c_sources = 

ags_widget_c_sources = \
	$(deprecated_ags_widget_c_sources) \
	src/ags/widget/ags_gui_init.c			\
	src/ags/widget/ags_cartesian.c			\
	src/ags/widget/ags_dial.c 			\
	src/ags/widget/ags_expander.c			\
	src/ags/widget/ags_expander_set.c		\
	src/ags/widget/ags_hindicator.c		\
	src/ags/widget/ags_htimebar.c			\
	src/ags/widget/ags_indicator.c			\
	src/ags/widget/ags_led.c			\
	src/ags/widget/ags_menu.c			\
	src/ags/widget/ags_menu_item.c			\
	src/ags/widget/ags_option_menu.c		\
	src/ags/widget/ags_ruler.c			\
	src/ags/widget/ags_table.c			\
	src/ags/widget/ags_timebar.c			\
	src/ags/widget/ags_vindicator.c		\
	src/ags/widget/ags_waveform.c

# libags_audio all files
deprecated_ags_audio_h_sources = \
	src/ags/audio/ags_effect.h		\
	src/ags/audio/ags_libao.h		\
	src/ags/audio/ags_timestamp.h		\
	src/ags/audio/ags_timestamp_factory.h	

ags_audio_h_sources = 		\
	$(deprecated_ags_audio_h_sources) \
	src/ags/audio/ags_audio_init.h		\
	src/ags/audio/ags_audio_application_context.h	\
	src/ags/audio/ags_sound_provider.h	\
	src/ags/audio/ags_acceleration.h	\
	src/ags/audio/ags_audio.h		\
	src/ags/audio/ags_audio_signal.h	\
	src/ags/audio/ags_automation.h		\
	src/ags/audio/ags_channel.h		\
	src/ags/audio/ags_channel_iter.h	\
	src/ags/audio/ags_playback_domain.h	\
	src/ags/audio/ags_playback.h		\
	src/ags/audio/ags_devout.h		\
	src/ags/audio/ags_input.h		\
	src/ags/audio/ags_notation.h		\
	src/ags/audio/ags_note.h		\
	src/ags/audio/ags_output.h		\
	src/ags/audio/ags_pattern.h		\
	src/ags/audio/ags_port.h		\
	src/ags/audio/ags_recall_audio.h	\
	src/ags/audio/ags_recall_audio_run.h	\
	src/ags/audio/ags_recall_audio_signal.h \
	src/ags/audio/ags_recall_channel.h	\
	src/ags/audio/ags_recall_channel_run_dummy.h \
	src/ags/audio/ags_recall_channel_run.h \
	src/ags/audio/ags_recall_container.h	\
	src/ags/audio/ags_recall_dependency.h	\
	src/ags/audio/ags_recall_factory.h	\
	src/ags/audio/ags_recall.h		\
	src/ags/audio/ags_recall_id.h		\
	src/ags/audio/ags_recall_ladspa.h	\
	src/ags/audio/ags_recall_ladspa_run.h	\
	src/ags/audio/ags_recall_lv2.h	\
	src/ags/audio/ags_recall_lv2_run.h	\
	src/ags/audio/ags_recall_recycling_dummy.h \
	src/ags/audio/ags_recall_recycling.h	\
	src/ags/audio/ags_recycling_context.h	\
	src/ags/audio/ags_recycling.h		\
	src/ags/audio/ags_synths.h		\
	src/ags/audio/ags_timestamp_factory.h	\
	src/ags/audio/ags_timestamp.h

deprecated_ags_audio_c_sources =  \
	src/ags/audio/ags_libao.c		\
	src/ags/audio/ags_timestamp.c		\
	src/ags/audio/ags_timestamp_factory.c	

ags_audio_c_sources =  		\
	$(deprecated_ags_audio_c_sources) \
	src/ags/audio/ags_audio_init.c		\
	src/ags/audio/ags_audio_application_context.c	\
	src/ags/audio/ags_sound_provider.c	\
	src/ags/audio/ags_acceleration.c	\
	src/ags/audio/ags_audio.c		\
	src/ags/audio/ags_audio_signal.c	\
	src/ags/audio/ags_automation.c		\
	src/ags/audio/ags_channel.c		\
	src/ags/audio/ags_channel_iter.c	\
	src/ags/audio/ags_playback_domain.c	\
	src/ags/audio/ags_playback.c		\
	src/ags/audio/ags_devout.c		\
	src/ags/audio/ags_input.c		\
	src/ags/audio/ags_notation.c		\
	src/ags/audio/ags_note.c		\
	src/ags/audio/ags_output.c		\
	src/ags/audio/ags_pattern.c		\
	src/ags/audio/ags_port.c		\
	src/ags/audio/ags_recall_audio.c	\
	src/ags/audio/ags_recall_audio_run.c	\
	src/ags/audio/ags_recall_audio_signal.c \
	src/ags/audio/ags_recall_channel.c	\
	src/ags/audio/ags_recall_channel_run_dummy.c \
	src/ags/audio/ags_recall_channel_run.c \
	src/ags/audio/ags_recall_container.c	\
	src/ags/audio/ags_recall_dependency.c	\
	src/ags/audio/ags_recall_factory.c	\
	src/ags/audio/ags_recall.c		\
	src/ags/audio/ags_recall_id.c		\
	src/ags/audio/ags_recall_ladspa.c	\
	src/ags/audio/ags_recall_ladspa_run.c	\
	src/ags/audio/ags_recall_lv2.c	\
	src/ags/audio/ags_recall_lv2_run.c	\
	src/ags/audio/ags_recall_recycling_dummy.c \
	src/ags/audio/ags_recall_recycling.c	\
	src/ags/audio/ags_recycling_context.c	\
	src/ags/audio/ags_recycling.c		\
	src/ags/audio/ags_synths.c		\
	src/ags/audio/ags_timestamp_factory.c	\
	src/ags/audio/ags_timestamp.c


deprecated_ags_audio_thread_h_sources =

ags_audio_thread_h_sources = \
	$(deprecated_ags_audio_thread_h_sources) \
	src/ags/audio/thread/ags_audio_loop.h			\
	src/ags/audio/thread/ags_soundcard_thread.h		\
	src/ags/audio/thread/ags_export_thread.h		\
	src/ags/audio/thread/ags_iterator_thread.h		\
	src/ags/audio/thread/ags_record_thread.h		\
	src/ags/audio/thread/ags_recycling_thread_callbacks.h \
	src/ags/audio/thread/ags_recycling_thread.h

deprecated_ags_audio_thread_c_sources =

ags_audio_thread_c_sources = \
	$(deprecated_ags_audio_thread_c_sources) \
	src/ags/audio/thread/ags_audio_loop.c			\
	src/ags/audio/thread/ags_soundcard_thread.c		\
	src/ags/audio/thread/ags_export_thread.c		\
	src/ags/audio/thread/ags_iterator_thread.c		\
	src/ags/audio/thread/ags_record_thread.c		\
	src/ags/audio/thread/ags_recycling_thread_callbacks.c \
	src/ags/audio/thread/ags_recycling_thread.c

deprecated_ags_audio_file_h_sources =

ags_audio_file_h_sources = \
	$(deprecated_ags_audio_file_h_sources) \
	src/ags/audio/file/ags_audio_file_xml.h		\
	src/ags/audio/file/ags_playable.h			\
	src/ags/audio/file/ags_audio_file.h	\
	src/ags/audio/file/ags_ipatch.h		\
	src/ags/audio/file/ags_ipatch_sf2_reader.h	\
	src/ags/audio/file/ags_sndfile.h

deprecated_ags_audio_file_c_sources = \
	src/ags/audio/file/ags_audio_file_xml.c

ags_audio_file_c_sources = \
	$(deprecated_ags_audio_file_c_sources) \
	src/ags/audio/file/ags_audio_file_xml.c		\
	src/ags/audio/file/ags_playable.c		\
	src/ags/audio/file/ags_audio_file.c	\
	src/ags/audio/file/ags_ipatch.c		\
	src/ags/audio/file/ags_ipatch_sf2_reader.c	\
	src/ags/audio/file/ags_sndfile.c

deprecated_ags_audio_midi_h_sources = 

ags_audio_midi_h_sources = \
	$(deprecated_ags_audio_midi_h_sources) \
	src/ags/audio/midi/ags_midi_parser.h

deprecated_ags_audio_midi_c_sources = 

ags_audio_midi_c_sources = \
	$(deprecated_ags_audio_midi_c_sources) \
	src/ags/audio/midi/ags_midi_parser.c

deprecated_ags_audio_recall_h_sources = \
	src/ags/audio/recall/ags_clone_channel.h		\
	src/ags/audio/recall/ags_clone_channel_run.h		\
	src/ags/audio/recall/ags_loop_channel.h		\
	src/ags/audio/recall/ags_loop_channel_run.h		\
	src/ags/audio/recall/ags_play_note.h

ags_audio_recall_h_sources = \
	$(deprecated_ags_audio_recall_h_sources) \
	src/ags/audio/recall/ags_buffer_audio_signal.h	\
	src/ags/audio/recall/ags_buffer_channel.h		\
	src/ags/audio/recall/ags_buffer_channel_run.h	\
	src/ags/audio/recall/ags_buffer_recycling.h		\
	src/ags/audio/recall/ags_copy_audio_signal.h		\
	src/ags/audio/recall/ags_copy_channel.h		\
	src/ags/audio/recall/ags_copy_channel_run.h		\
	src/ags/audio/recall/ags_copy_notation_audio.h	\
	src/ags/audio/recall/ags_copy_notation_audio_run.h	\
	src/ags/audio/recall/ags_copy_pattern_audio.h	\
	src/ags/audio/recall/ags_copy_pattern_audio_run.h	\
	src/ags/audio/recall/ags_copy_pattern_channel.h	\
	src/ags/audio/recall/ags_copy_pattern_channel_run.h	\
	src/ags/audio/recall/ags_copy_recycling.h		\
	src/ags/audio/recall/ags_count_beats_audio.h		\
	src/ags/audio/recall/ags_count_beats_audio_run.h	\
	src/ags/audio/recall/ags_delay_audio.h 		\
	src/ags/audio/recall/ags_delay_audio_run.h		\
	src/ags/audio/recall/ags_mute_audio.h		\
	src/ags/audio/recall/ags_mute_audio_signal.h		\
	src/ags/audio/recall/ags_mute_channel.h		\
	src/ags/audio/recall/ags_mute_channel_run.h		\
	src/ags/audio/recall/ags_mute_recycling.h		\
	src/ags/audio/recall/ags_peak_channel.h		\
	src/ags/audio/recall/ags_peak_channel_run.h		\
	src/ags/audio/recall/ags_play_audio_file.h		\
	src/ags/audio/recall/ags_play_audio.h		\
	src/ags/audio/recall/ags_play_audio_signal.h		\
	src/ags/audio/recall/ags_play_channel.h		\
	src/ags/audio/recall/ags_play_channel_run.h		\
	src/ags/audio/recall/ags_play_channel_run_master.h	\
	src/ags/audio/recall/ags_play_notation_audio.h	\
	src/ags/audio/recall/ags_play_notation_audio_run.h	\
	src/ags/audio/recall/ags_play_pattern.h		\
	src/ags/audio/recall/ags_play_recycling.h		\
	src/ags/audio/recall/ags_stream_audio_signal.h	\
	src/ags/audio/recall/ags_stream_channel.h		\
	src/ags/audio/recall/ags_stream_channel_run.h	\
	src/ags/audio/recall/ags_stream_recycling.h		\
	src/ags/audio/recall/ags_volume_audio_signal.h	\
	src/ags/audio/recall/ags_volume_channel.h		\
	src/ags/audio/recall/ags_volume_channel_run.h	\
	src/ags/audio/recall/ags_volume_recycling.h

deprecated_ags_audio_recall_c_sources =  \
	src/ags/audio/recall/ags_clone_channel.c		\
	src/ags/audio/recall/ags_clone_channel_run.c		\
	src/ags/audio/recall/ags_loop_channel.c		\
	src/ags/audio/recall/ags_loop_channel_run.c		\
	src/ags/audio/recall/ags_play_note.c

ags_audio_recall_c_sources = \
	$(deprecated_ags_audio_recall_c_sources) \
	src/ags/audio/recall/ags_buffer_audio_signal.c	\
	src/ags/audio/recall/ags_buffer_channel.c		\
	src/ags/audio/recall/ags_buffer_channel_run.c	\
	src/ags/audio/recall/ags_buffer_recycling.c		\
	src/ags/audio/recall/ags_copy_audio_signal.c		\
	src/ags/audio/recall/ags_copy_channel.c		\
	src/ags/audio/recall/ags_copy_channel_run.c		\
	src/ags/audio/recall/ags_copy_notation_audio.c	\
	src/ags/audio/recall/ags_copy_notation_audio_run.c	\
	src/ags/audio/recall/ags_copy_pattern_audio.c	\
	src/ags/audio/recall/ags_copy_pattern_audio_run.c	\
	src/ags/audio/recall/ags_copy_pattern_channel.c	\
	src/ags/audio/recall/ags_copy_pattern_channel_run.c	\
	src/ags/audio/recall/ags_copy_recycling.c		\
	src/ags/audio/recall/ags_count_beats_audio.c		\
	src/ags/audio/recall/ags_count_beats_audio_run.c	\
	src/ags/audio/recall/ags_delay_audio.c 		\
	src/ags/audio/recall/ags_delay_audio_run.c		\
	src/ags/audio/recall/ags_mute_audio.c		\
	src/ags/audio/recall/ags_mute_audio_signal.c		\
	src/ags/audio/recall/ags_mute_channel.c		\
	src/ags/audio/recall/ags_mute_channel_run.c		\
	src/ags/audio/recall/ags_mute_recycling.c		\
	src/ags/audio/recall/ags_peak_channel.c		\
	src/ags/audio/recall/ags_peak_channel_run.c		\
	src/ags/audio/recall/ags_play_audio_file.c		\
	src/ags/audio/recall/ags_play_audio.c		\
	src/ags/audio/recall/ags_play_audio_signal.c		\
	src/ags/audio/recall/ags_play_channel.c		\
	src/ags/audio/recall/ags_play_channel_run.c		\
	src/ags/audio/recall/ags_play_channel_run_master.c	\
	src/ags/audio/recall/ags_play_notation_audio.c	\
	src/ags/audio/recall/ags_play_notation_audio_run.c	\
	src/ags/audio/recall/ags_play_pattern.c		\
	src/ags/audio/recall/ags_play_recycling.c		\
	src/ags/audio/recall/ags_stream_audio_signal.c	\
	src/ags/audio/recall/ags_stream_channel.c		\
	src/ags/audio/recall/ags_stream_channel_run.c	\
	src/ags/audio/recall/ags_stream_recycling.c		\
	src/ags/audio/recall/ags_volume_audio_signal.c	\
	src/ags/audio/recall/ags_volume_channel.c		\
	src/ags/audio/recall/ags_volume_channel_run.c	\
	src/ags/audio/recall/ags_volume_recycling.c

deprecated_ags_task_h_sources = \
	src/ags/audio/task/ags_apply_wavetable.h

ags_audio_task_h_sources = \
	$(deprecated_ags_audio_task_h_sources) \
	src/ags/audio/task/ags_add_audio.h				\
	src/ags/audio/task/ags_add_audio_signal.h			\
	src/ags/audio/task/ags_add_note.h				\
	src/ags/audio/task/ags_add_point_to_selection.h			\
	src/ags/audio/task/ags_add_effect.h				\
	src/ags/audio/task/ags_remove_point_from_selection.h 		\
	src/ags/audio/task/ags_add_region_to_selection.h		\
	src/ags/audio/task/ags_remove_region_from_selection.h 		\
	src/ags/audio/task/ags_free_selection.h				\
	src/ags/audio/task/ags_add_recall_container.h 			\
	src/ags/audio/task/ags_add_recall.h				\
	src/ags/audio/task/ags_append_audio.h				\
	src/ags/audio/task/ags_append_audio_threaded.h 			\
	src/ags/audio/task/ags_append_channel.h				\
	src/ags/audio/task/ags_append_recall.h				\
	src/ags/audio/task/ags_apply_synth.h				\
	src/ags/audio/task/ags_audio_set_recycling.h 			\
	src/ags/audio/task/ags_cancel_audio.h				\
	src/ags/audio/task/ags_cancel_channel.h				\
	src/ags/audio/task/ags_cancel_recall.h				\
	src/ags/audio/task/ags_channel_set_recycling.h 			\
	src/ags/audio/task/ags_export_output.h				\
	src/ags/audio/task/ags_init_audio.h				\
	src/ags/audio/task/ags_init_channel.h				\
	src/ags/audio/task/ags_link_channel.h				\
	src/ags/audio/task/ags_open_file.h				\
	src/ags/audio/task/ags_open_single_file.h			\
	src/ags/audio/task/ags_remove_audio.h				\
	src/ags/audio/task/ags_remove_audio_signal.h 			\
	src/ags/audio/task/ags_remove_note.h				\
	src/ags/audio/task/ags_remove_recall_container.h 		\
	src/ags/audio/task/ags_remove_recall.h				\
	src/ags/audio/task/ags_resize_audio.h				\
	src/ags/audio/task/ags_set_audio_channels.h 			\
	src/ags/audio/task/ags_set_buffer_size.h			\
	src/ags/audio/task/ags_set_playback_flags.h 			\
	src/ags/audio/task/ags_set_output_device.h			\
	src/ags/audio/task/ags_set_samplerate.h				\
	src/ags/audio/task/ags_start_devout.h				\
	src/ags/audio/task/ags_unref_audio_signal.h			\
	src/ags/audio/task/ags_toggle_pattern_bit.h

deprecated_ags_task_c_sources = \
	src/ags/audio/task/ags_apply_wavetable.c

ags_audio_task_c_sources = \
	$(deprecated_ags_audio_task_c_sources) \
	src/ags/audio/task/ags_add_audio.c				\
	src/ags/audio/task/ags_add_audio_signal.c			\
	src/ags/audio/task/ags_add_note.c				\
	src/ags/audio/task/ags_add_point_to_selection.c			\
	src/ags/audio/task/ags_add_effect.c				\
	src/ags/audio/task/ags_remove_point_from_selection.c 		\
	src/ags/audio/task/ags_add_region_to_selection.c		\
	src/ags/audio/task/ags_remove_region_from_selection.c 		\
	src/ags/audio/task/ags_free_selection.c				\
	src/ags/audio/task/ags_add_recall_container.c 			\
	src/ags/audio/task/ags_add_recall.c				\
	src/ags/audio/task/ags_append_audio.c				\
	src/ags/audio/task/ags_append_audio_threaded.c 			\
	src/ags/audio/task/ags_append_channel.c				\
	src/ags/audio/task/ags_append_recall.c				\
	src/ags/audio/task/ags_apply_synth.c				\
	src/ags/audio/task/ags_audio_set_recycling.c 			\
	src/ags/audio/task/ags_cancel_audio.c				\
	src/ags/audio/task/ags_cancel_channel.c				\
	src/ags/audio/task/ags_cancel_recall.c				\
	src/ags/audio/task/ags_channel_set_recycling.c 			\
	src/ags/audio/task/ags_export_output.c				\
	src/ags/audio/task/ags_init_audio.c				\
	src/ags/audio/task/ags_init_channel.c				\
	src/ags/audio/task/ags_link_channel.c				\
	src/ags/audio/task/ags_open_file.c				\
	src/ags/audio/task/ags_open_single_file.c			\
	src/ags/audio/task/ags_remove_audio.c				\
	src/ags/audio/task/ags_remove_audio_signal.c 			\
	src/ags/audio/task/ags_remove_note.c				\
	src/ags/audio/task/ags_remove_recall_container.c 		\
	src/ags/audio/task/ags_remove_recall.c				\
	src/ags/audio/task/ags_resize_audio.c				\
	src/ags/audio/task/ags_set_audio_channels.c 			\
	src/ags/audio/task/ags_set_buffer_size.c			\
	src/ags/audio/task/ags_set_playback_flags.c 			\
	src/ags/audio/task/ags_set_output_device.c			\
	src/ags/audio/task/ags_set_samplerate.c				\
	src/ags/audio/task/ags_start_soundcard.c			\
	src/ags/audio/task/ags_unref_audio_signal.c			\
	src/ags/audio/task/ags_toggle_pattern_bit.c

deprecated_ags_audio_task_recall_h_sources =

ags_audio_task_recall_h_sources = \
	$(deprecated_ags_audio_task_recall_h_sources) \
	src/ags/audio/task/recall/ags_apply_bpm.h		\
	src/ags/audio/task/recall/ags_apply_sequencer_length.h \
	src/ags/audio/task/recall/ags_apply_tact.h	\
	src/ags/audio/task/recall/ags_set_muted.h

deprecated_ags_audio_task_recall_c_sources =

ags_audio_task_recall_c_sources = \
	$(deprecated_ags_audio_task_recall_c_sources) \
	src/ags/audio/task/recall/ags_apply_bpm.c		\
	src/ags/audio/task/recall/ags_apply_sequencer_length.c \
	src/ags/audio/task/recall/ags_apply_tact.c	\
	src/ags/audio/task/recall/ags_set_muted.c

# gsequencer all files
gsequencer_main_h_sources = \
	src/ags/main.h

gsequencer_main_c_sources = \
	src/ags/main.c

deprecated_gsequencer_server_h_sources = 

gsequencer_server_h_sources = \
	$(deprecated_gsequencer_server_h_sources) \
	src/ags/server/ags_server_init.h		\
	src/ags/server/ags_server_application_context.h	\
	src/ags/server/ags_service_provider.h 		\
	src/ags/server/ags_registry.h			\
	src/ags/server/ags_remote_task.h		\
	src/ags/server/ags_server.h

deprecated_gsequencer_server_c_sources = 

gsequencer_server_c_sources = \
	$(deprecated_gsequencer_server_c_sources) \
	src/ags/server/ags_server_init.c		\
	src/ags/server/ags_server_application_context.c	\
	src/ags/server/ags_service_provider.c 		\
	src/ags/server/ags_registry.c			\
	src/ags/server/ags_remote_task.c		\
	src/ags/server/ags_server.c


deprecated_gsequencer_xorg_h_sources =

gsequencer_xorg_h_sources = \
	$(deprecated_gsequencer_xorg_h_sources)	\
	src/ags/X/ags_xorg_init.h			\
	src/ags/X/ags_xorg_application_context.h	\
	src/ags/X/ags_audio_preferences_callbacks.h	\
	src/ags/X/ags_audio_preferences.h		\
	src/ags/X/ags_automation_editor_callbacks.h	\
	src/ags/X/ags_automation_editor.h		\
	src/ags/X/ags_automation_window.h		\
	src/ags/X/ags_automation_window_callbacks.h	\
	src/ags/X/ags_editor_callbacks.h		\
	src/ags/X/ags_editor.h				\
	src/ags/X/ags_effect_bridge_callbacks.h		\
	src/ags/X/ags_effect_bridge.h			\
	src/ags/X/ags_effect_bulk_callbacks.h		\
	src/ags/X/ags_effect_bulk.h			\
	src/ags/X/ags_bulk_member_callbacks.h		\
	src/ags/X/ags_bulk_member.h			\
	src/ags/X/ags_effect_line.h			\
	src/ags/X/ags_effect_line_callbacks.h		\
	src/ags/X/ags_effect_pad_callbacks.h		\
	src/ags/X/ags_effect_pad.h			\
	src/ags/X/ags_export_window_callbacks.h		\
	src/ags/X/ags_export_window.h			\
	src/ags/X/ags_generic_preferences_callbacks.h	\
	src/ags/X/ags_generic_preferences.h		\
	src/ags/X/ags_plugin_browser_callbacks.h	\
	src/ags/X/ags_plugin_browser.h			\
	src/ags/X/ags_ladspa_browser_callbacks.h	\
	src/ags/X/ags_ladspa_browser.h			\
	src/ags/X/ags_lv2_browser_callbacks.h		\
	src/ags/X/ags_lv2_browser.h			\
	src/ags/X/ags_line_callbacks.h 			\
	src/ags/X/ags_line_editor_callbacks.h		\
	src/ags/X/ags_line_editor.h			\
	src/ags/X/ags_line.h				\
	src/ags/X/ags_line_member_callbacks.h		\
	src/ags/X/ags_line_member_editor_callbacks.h	\
	src/ags/X/ags_line_member_editor.h		\
	src/ags/X/ags_line_member.h			\
	src/ags/X/ags_link_collection_editor_callbacks.h	\
	src/ags/X/ags_link_collection_editor.h		\
	src/ags/X/ags_link_editor_callbacks.h		\
	src/ags/X/ags_link_editor.h			\
	src/ags/X/ags_listing_editor_callbacks.h	\
	src/ags/X/ags_listing_editor.h			\
	src/ags/X/ags_machine_callbacks.h		\
	src/ags/X/ags_machine_editor_callbacks.h	\
	src/ags/X/ags_machine_editor.h			\
	src/ags/X/ags_machine.h				\
	src/ags/X/ags_menu_bar_callbacks.h		\
	src/ags/X/ags_menu_bar.h 			\
	src/ags/X/ags_navigation_callbacks.h		\
	src/ags/X/ags_navigation.h			\
	src/ags/X/ags_pad_callbacks.h			\
	src/ags/X/ags_pad_editor_callbacks.h 		\
	src/ags/X/ags_pad_editor.h			\
	src/ags/X/ags_pad.h				\
	src/ags/X/ags_performance_preferences_callbacks.h	\
	src/ags/X/ags_performance_preferences.h		\
	src/ags/X/ags_plugin_preferences_callbacks.h	\
	src/ags/X/ags_plugin_preferences.h		\
	src/ags/X/ags_preferences_callbacks.h		\
	src/ags/X/ags_preferences.h			\
	src/ags/X/ags_property_collection_editor_callbacks.h	\
	src/ags/X/ags_property_collection_editor.h	\
	src/ags/X/ags_property_editor_callbacks.h	\
	src/ags/X/ags_property_editor.h			\
	src/ags/X/ags_property_listing_editor.h		\
	src/ags/X/ags_resize_editor.h			\
	src/ags/X/ags_server_preferences_callbacks.h	\
	src/ags/X/ags_server_preferences.h		\
	src/ags/X/ags_window_callbacks.h		\
	src/ags/X/ags_window.h

deprecated_gsequencer_xorg_c_sources = 

gsequencer_xorg_c_sources = \
	$(deprecated_gsequencer_xorg_c_sources) 	\
	src/ags/X/ags_xorg_init.c			\
	src/ags/X/ags_xorg_application_context.c	\
	src/ags/X/ags_audio_preferences_callbacks.c	\
	src/ags/X/ags_audio_preferences.c		\
	src/ags/X/ags_automation_editor_callbacks.c	\
	src/ags/X/ags_automation_editor.c		\
	src/ags/X/ags_automation_window.c		\
	src/ags/X/ags_automation_window_callbacks.c	\
	src/ags/X/ags_editor_callbacks.c		\
	src/ags/X/ags_editor.c				\
	src/ags/X/ags_effect_bridge_callbacks.c		\
	src/ags/X/ags_effect_bridge.c			\
	src/ags/X/ags_effect_bulk_callbacks.c		\
	src/ags/X/ags_effect_bulk.c			\
	src/ags/X/ags_bulk_member.c			\
	src/ags/X/ags_bulk_member_callbacks.c		\
	src/ags/X/ags_effect_line.c			\
	src/ags/X/ags_effect_line_callbacks.c		\
	src/ags/X/ags_effect_pad_callbacks.c		\
	src/ags/X/ags_effect_pad.c			\
	src/ags/X/ags_export_window_callbacks.c		\
	src/ags/X/ags_export_window.c			\
	src/ags/X/ags_generic_preferences_callbacks.c	\
	src/ags/X/ags_generic_preferences.c		\
	src/ags/X/ags_plugin_browser_callbacks.c	\
	src/ags/X/ags_plugin_browser.c			\
	src/ags/X/ags_ladspa_browser_callbacks.c	\
	src/ags/X/ags_ladspa_browser.c			\
	src/ags/X/ags_lv2_browser_callbacks.c		\
	src/ags/X/ags_lv2_browser.c			\
	src/ags/X/ags_line_callbacks.c 			\
	src/ags/X/ags_line_editor_callbacks.c		\
	src/ags/X/ags_line_editor.c			\
	src/ags/X/ags_line.c				\
	src/ags/X/ags_line_member_callbacks.c		\
	src/ags/X/ags_line_member_editor_callbacks.c	\
	src/ags/X/ags_line_member_editor.c		\
	src/ags/X/ags_line_member.c			\
	src/ags/X/ags_link_collection_editor_callbacks.c	\
	src/ags/X/ags_link_collection_editor.c		\
	src/ags/X/ags_link_editor_callbacks.c		\
	src/ags/X/ags_link_editor.c			\
	src/ags/X/ags_listing_editor_callbacks.c	\
	src/ags/X/ags_listing_editor.c			\
	src/ags/X/ags_machine_callbacks.c		\
	src/ags/X/ags_machine_editor_callbacks.c	\
	src/ags/X/ags_machine_editor.c			\
	src/ags/X/ags_machine.c				\
	src/ags/X/ags_menu_bar_callbacks.c		\
	src/ags/X/ags_menu_bar.c 			\
	src/ags/X/ags_navigation_callbacks.c		\
	src/ags/X/ags_navigation.c			\
	src/ags/X/ags_pad_callbacks.c			\
	src/ags/X/ags_pad_editor_callbacks.c 		\
	src/ags/X/ags_pad_editor.c			\
	src/ags/X/ags_pad.c				\
	src/ags/X/ags_performance_preferences_callbacks.c	\
	src/ags/X/ags_performance_preferences.c		\
	src/ags/X/ags_plugin_preferences_callbacks.c	\
	src/ags/X/ags_plugin_preferences.c		\
	src/ags/X/ags_preferences_callbacks.c		\
	src/ags/X/ags_preferences.c			\
	src/ags/X/ags_property_collection_editor_callbacks.c \
	src/ags/X/ags_property_collection_editor.c	\
	src/ags/X/ags_property_editor_callbacks.c	\
	src/ags/X/ags_property_editor.c			\
	src/ags/X/ags_property_listing_editor.c		\
	src/ags/X/ags_resize_editor.c			\
	src/ags/X/ags_server_preferences_callbacks.c	\
	src/ags/X/ags_server_preferences.c		\
	src/ags/X/ags_window_callbacks.c		\
	src/ags/X/ags_window.c


deprecated_gsequencer_xorg_import_h_sources = 

gsequencer_xorg_import_h_sources = \
	$(deprecated_gsequencer_xorg_import_h_sources) 	\
	src/ags/X/import/ags_midi_import_wizard_callbacks.h		\
	src/ags/X/import/ags_midi_import_wizard.h			\
	src/ags/X/import/ags_track_collection_callbacks.h		\
	src/ags/X/import/ags_track_collection.h				\
	src/ags/X/import/ags_track_collection_mapper_callbacks.h	\
	src/ags/X/import/ags_track_collection_mapper.h

deprecated_gsequencer_xorg_import_c_sources = 

gsequencer_xorg_import_c_sources = \
	$(deprecated_gsequencer_xorg_import_c_sources) 	\
	src/ags/X/import/ags_midi_import_wizard_callbacks.c		\
	src/ags/X/import/ags_midi_import_wizard.c			\
	src/ags/X/import/ags_track_collection_callbacks.c		\
	src/ags/X/import/ags_track_collection.c				\
	src/ags/X/import/ags_track_collection_mapper_callbacks.c	\
	src/ags/X/import/ags_track_collection_mapper.c

deprecated_gsequencer_xorg_thread_h_sources = 

gsequencer_xorg_thread_h_sources = \
	$(deprecated_gsequencer_xorg_thread_h_sources) 	\
	src/ags/X/thread/ags_gui_thread.h

deprecated_gsequencer_xorg_thread_c_sources = 

gsequencer_xorg_thread_c_sources = \
	$(deprecated_gsequencer_xorg_thread_c_sources) 	\
	src/ags/X/thread/ags_gui_thread.c

deprecated_gsequencer_xorg_task_h_sources = 

gsequencer_xorg_task_h_sources = \
	$(deprecated_gsequencer_xorg_task_h_sources) 		\
	src/ags/X/task/ags_add_line_member.h			\
	src/ags/X/task/ags_add_bulk_member.h			\
	src/ags/X/task/ags_update_bulk_member.h			\
	src/ags/X/task/ags_change_indicator.h			\
	src/ags/X/task/ags_change_tact.h			\
	src/ags/X/task/ags_display_tact.h			\
	src/ags/X/task/ags_scroll_on_play.h			\
	src/ags/X/task/ags_toggle_led.h

deprecated_gsequencer_xorg_task_c_sources = 

gsequencer_xorg_task_c_sources = \
	$(deprecated_gsequencer_xorg_task_c_sources) 	\
	src/ags/X/task/ags_add_line_member.c			\
	src/ags/X/task/ags_add_bulk_member.c			\
	src/ags/X/task/ags_update_bulk_member.c			\
	src/ags/X/task/ags_change_indicator.c			\
	src/ags/X/task/ags_change_tact.c			\
	src/ags/X/task/ags_display_tact.c			\
	src/ags/X/task/ags_scroll_on_play.c			\
	src/ags/X/task/ags_toggle_led.c

deprecated_gsequencer_machine_h_sources =

gsequencer_machine_h_sources = \
	$(deprecated_gsequencer_machine_h_sources) \
	src/ags/X/machine/ags_drum_callbacks.h			\
	src/ags/X/machine/ags_drum.h				\
	src/ags/X/machine/ags_drum_input_line_callbacks.h	\
	src/ags/X/machine/ags_drum_input_line.h			\
	src/ags/X/machine/ags_drum_input_pad_callbacks.h	\
	src/ags/X/machine/ags_drum_input_pad.h			\
	src/ags/X/machine/ags_drum_output_line_callbacks.h	\
	src/ags/X/machine/ags_drum_output_line.h		\
	src/ags/X/machine/ags_drum_output_pad_callbacks.h	\
	src/ags/X/machine/ags_drum_output_pad.h			\
	src/ags/X/machine/ags_ffplayer_bridge_callbacks.h	\
	src/ags/X/machine/ags_ffplayer_bridge.h			\
	src/ags/X/machine/ags_ffplayer_bulk_input_callbacks.h	\
	src/ags/X/machine/ags_ffplayer_bulk_input.h		\
	src/ags/X/machine/ags_ffplayer_callbacks.h		\
	src/ags/X/machine/ags_ffplayer.h			\
	src/ags/X/machine/ags_ffplayer_input_line_callbacks.h	\
	src/ags/X/machine/ags_ffplayer_input_line.h		\
	src/ags/X/machine/ags_ffplayer_input_pad_callbacks.h	\
	src/ags/X/machine/ags_ffplayer_input_pad.h		\
	src/ags/X/machine/ags_ladspa_bridge_callbacks.h		\
	src/ags/X/machine/ags_ladspa_bridge.h			\
	src/ags/X/machine/ags_lv2_bridge_callbacks.h		\
	src/ags/X/machine/ags_lv2_bridge.h			\
	src/ags/X/machine/ags_replicator_bridge_callbacks.h	\
	src/ags/X/machine/ags_replicator_bridge.h		\
	src/ags/X/machine/ags_matrix_bridge_callbacks.h		\
	src/ags/X/machine/ags_matrix_bridge.h			\
	src/ags/X/machine/ags_matrix_bulk_input_callbacks.h	\
	src/ags/X/machine/ags_matrix_bulk_input.h		\
	src/ags/X/machine/ags_matrix_callbacks.h		\
	src/ags/X/machine/ags_matrix.h				\
	src/ags/X/machine/ags_mixer_callbacks.h			\
	src/ags/X/machine/ags_mixer.h				\
	src/ags/X/machine/ags_mixer_input_line.h		\
	src/ags/X/machine/ags_mixer_input_pad.h			\
	src/ags/X/machine/ags_oscillator_callbacks.h		\
	src/ags/X/machine/ags_oscillator.h			\
	src/ags/X/machine/ags_panel_callbacks.h			\
	src/ags/X/machine/ags_panel.h				\
	src/ags/X/machine/ags_panel_input_line.h		\
	src/ags/X/machine/ags_panel_input_pad.h			\
	src/ags/X/machine/ags_synth_bridge_callbacks.h		\
	src/ags/X/machine/ags_synth_bridge.h			\
	src/ags/X/machine/ags_synth_bulk_output_callbacks.h	\
	src/ags/X/machine/ags_synth_bulk_output.h		\
	src/ags/X/machine/ags_synth_callbacks.h			\
	src/ags/X/machine/ags_synth.h				\
	src/ags/X/machine/ags_synth_input_line.h		\
	src/ags/X/machine/ags_synth_input_pad.h			\
	src/ags/X/machine/ags_synth_output_line_callbacks.h	\
	src/ags/X/machine/ags_synth_output_line.h		\
	src/ags/X/machine/ags_synth_output_pad_callbacks.h	\
	src/ags/X/machine/ags_synth_output_pad.h

deprecated_gsequencer_machine_c_sources = 

gsequencer_machine_c_sources = \
	$(deprecated_gsequencer_machine_c_sources) \
	src/ags/X/machine/ags_drum_callbacks.c			\
	src/ags/X/machine/ags_drum.c				\
	src/ags/X/machine/ags_drum_input_line_callbacks.c	\
	src/ags/X/machine/ags_drum_input_line.c			\
	src/ags/X/machine/ags_drum_input_pad_callbacks.c	\
	src/ags/X/machine/ags_drum_input_pad.c			\
	src/ags/X/machine/ags_drum_output_line_callbacks.c	\
	src/ags/X/machine/ags_drum_output_line.c		\
	src/ags/X/machine/ags_drum_output_pad_callbacks.c	\
	src/ags/X/machine/ags_drum_output_pad.c			\
	src/ags/X/machine/ags_ffplayer_bridge_callbacks.c	\
	src/ags/X/machine/ags_ffplayer_bridge.c			\
	src/ags/X/machine/ags_ffplayer_bulk_input_callbacks.c	\
	src/ags/X/machine/ags_ffplayer_bulk_input.c		\
	src/ags/X/machine/ags_ffplayer_callbacks.c		\
	src/ags/X/machine/ags_ffplayer.c			\
	src/ags/X/machine/ags_ffplayer_input_line_callbacks.c	\
	src/ags/X/machine/ags_ffplayer_input_line.c		\
	src/ags/X/machine/ags_ffplayer_input_pad_callbacks.c	\
	src/ags/X/machine/ags_ffplayer_input_pad.c		\
	src/ags/X/machine/ags_ladspa_bridge_callbacks.c		\
	src/ags/X/machine/ags_ladspa_bridge.c			\
	src/ags/X/machine/ags_lv2_bridge_callbacks.c		\
	src/ags/X/machine/ags_lv2_bridge.c			\
	src/ags/X/machine/ags_replicator_bridge_callbacks.c	\
	src/ags/X/machine/ags_replicator_bridge.c		\
	src/ags/X/machine/ags_matrix_bridge_callbacks.c		\
	src/ags/X/machine/ags_matrix_bridge.c			\
	src/ags/X/machine/ags_matrix_bulk_input_callbacks.c	\
	src/ags/X/machine/ags_matrix_bulk_input.c		\
	src/ags/X/machine/ags_matrix_callbacks.c		\
	src/ags/X/machine/ags_matrix.c				\
	src/ags/X/machine/ags_mixer_callbacks.c			\
	src/ags/X/machine/ags_mixer.c				\
	src/ags/X/machine/ags_mixer_input_line.c		\
	src/ags/X/machine/ags_mixer_input_pad.c			\
	src/ags/X/machine/ags_oscillator_callbacks.c		\
	src/ags/X/machine/ags_oscillator.c			\
	src/ags/X/machine/ags_panel_callbacks.c			\
	src/ags/X/machine/ags_panel.c				\
	src/ags/X/machine/ags_panel_input_line.c		\
	src/ags/X/machine/ags_panel_input_pad.c			\
	src/ags/X/machine/ags_synth_bridge_callbacks.c		\
	src/ags/X/machine/ags_synth_bridge.c			\
	src/ags/X/machine/ags_synth_bulk_output_callbacks.c	\
	src/ags/X/machine/ags_synth_bulk_output.c		\
	src/ags/X/machine/ags_synth_callbacks.c			\
	src/ags/X/machine/ags_synth.c				\
	src/ags/X/machine/ags_synth_input_line.c		\
	src/ags/X/machine/ags_synth_input_pad.c			\
	src/ags/X/machine/ags_synth_output_line_callbacks.c	\
	src/ags/X/machine/ags_synth_output_line.c		\
	src/ags/X/machine/ags_synth_output_pad_callbacks.c	\
	src/ags/X/machine/ags_synth_output_pad.c

deprecated_gsequencer_editor_h_sources = 

gsequencer_editor_h_sources = \
	$(deprecated_gsequencer_editor_h_sources) \
	src/ags/X/editor/ags_automation_area_callbacks.h	\
	src/ags/X/editor/ags_automation_area.h			\
	src/ags/X/editor/ags_automation_edit_callbacks.h	\
	src/ags/X/editor/ags_automation_edit.h			\
	src/ags/X/editor/ags_automation_toolbar_callbacks.h	\
	src/ags/X/editor/ags_automation_toolbar.h		\
	src/ags/X/editor/ags_file_selection_callbacks.h		\
	src/ags/X/editor/ags_file_selection.h			\
	src/ags/X/editor/ags_inline_player_callbacks.h		\
	src/ags/X/editor/ags_inline_player.h			\
	src/ags/X/editor/ags_machine_radio_button.h		\
	src/ags/X/editor/ags_machine_selection.h		\
	src/ags/X/editor/ags_machine_selector_callbacks.h	\
	src/ags/X/editor/ags_machine_selector.h			\
	src/ags/X/editor/ags_notebook_callbacks.h		\
	src/ags/X/editor/ags_notebook.h				\
	src/ags/X/editor/ags_note_edit_callbacks.h		\
	src/ags/X/editor/ags_note_edit.h			\
	src/ags/X/editor/ags_pattern_edit_callbacks.h		\
	src/ags/X/editor/ags_pattern_edit.h			\
	src/ags/X/editor/ags_piano_callbacks.h			\
	src/ags/X/editor/ags_piano.h				\
	src/ags/X/editor/ags_port_selection_callbacks.h		\
	src/ags/X/editor/ags_port_selection.h			\
	src/ags/X/editor/ags_scale_area_callbacks.h		\
	src/ags/X/editor/ags_scale_area.h			\
	src/ags/X/editor/ags_scale_callbacks.h			\
	src/ags/X/editor/ags_scale.h				\
	src/ags/X/editor/ags_sf2_chooser_callbacks.h		\
	src/ags/X/editor/ags_sf2_chooser.h			\
	src/ags/X/editor/ags_soundset_callbacks.h		\
	src/ags/X/editor/ags_soundset.h				\
	src/ags/X/editor/ags_toolbar_callbacks.h		\
	src/ags/X/editor/ags_toolbar.h				\
	src/ags/X/editor/ags_toolbar_mode_stock.h

deprecated_gsequencer_editor_c_sources = 

gsequencer_editor_c_sources = \
	$(deprecated_gsequencer_editor_c_sources) \
	src/ags/X/editor/ags_automation_area_callbacks.c	\
	src/ags/X/editor/ags_automation_area.c			\
	src/ags/X/editor/ags_automation_edit_callbacks.c	\
	src/ags/X/editor/ags_automation_edit.c			\
	src/ags/X/editor/ags_automation_toolbar_callbacks.c	\
	src/ags/X/editor/ags_automation_toolbar.c		\
	src/ags/X/editor/ags_file_selection_callbacks.c		\
	src/ags/X/editor/ags_file_selection.c			\
	src/ags/X/editor/ags_inline_player_callbacks.c		\
	src/ags/X/editor/ags_inline_player.c			\
	src/ags/X/editor/ags_machine_radio_button.c		\
	src/ags/X/editor/ags_machine_selection.c		\
	src/ags/X/editor/ags_machine_selector_callbacks.c	\
	src/ags/X/editor/ags_machine_selector.c			\
	src/ags/X/editor/ags_notebook_callbacks.c		\
	src/ags/X/editor/ags_notebook.c				\
	src/ags/X/editor/ags_note_edit_callbacks.c		\
	src/ags/X/editor/ags_note_edit.c			\
	src/ags/X/editor/ags_pattern_edit_callbacks.c		\
	src/ags/X/editor/ags_pattern_edit.c			\
	src/ags/X/editor/ags_piano_callbacks.c			\
	src/ags/X/editor/ags_piano.c				\
	src/ags/X/editor/ags_port_selection_callbacks.c		\
	src/ags/X/editor/ags_port_selection.c			\
	src/ags/X/editor/ags_scale_area_callbacks.c		\
	src/ags/X/editor/ags_scale_area.c			\
	src/ags/X/editor/ags_scale_callbacks.c			\
	src/ags/X/editor/ags_scale.c				\
	src/ags/X/editor/ags_sf2_chooser_callbacks.c		\
	src/ags/X/editor/ags_sf2_chooser.c			\
	src/ags/X/editor/ags_soundset_callbacks.c		\
	src/ags/X/editor/ags_soundset.c				\
	src/ags/X/editor/ags_toolbar_callbacks.c		\
	src/ags/X/editor/ags_toolbar.c

deprecated_gsequencer_file_h_sources =

gsequencer_file_h_sources = \
	$(deprecated_gsequencer_file_h_sources) \
	src/ags/X/file/ags_gsequencer_file_xml.h

deprecated_gsequencer_file_c_sources = 

gsequencer_file_c_sources = \
	$(deprecated_gsequencer_file_c_sources) \
	src/ags/X/file/ags_gsequencer_file_xml.c
