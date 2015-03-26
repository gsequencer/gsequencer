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
libags_a_SOURCES = $(ags_util_c_sources) $(ags_lib_c_sources)
libags_thread_a_SOURCES = 
libags_audio_a_SOURCES = $(ags_audio_c_sources)
libags_gui_a_SOURCES = 
gsequencer_SOURCES = $(gsequencer_main_c_sources) $(gsequencer_xorg_c_sources)

#
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

ACLOCAL_AMFLAGS = -I m4

# libags all files
deprecated_ags_lib_h_sources =

ags_lib_h_sources = \
	$(deprecated_ags_lib_h_sources) \
	src/ags/lib/ags_combo_box_text.h		\
	src/ags/lib/container.h			\
	src/ags/lib/list.h			\
	src/ags/lib/log.h			\
	src/ags/lib/parameter.h

deprecated_ags_lib_c_sources = 

ags_lib_c_sources = \
	$(deprecated_ags_lib_c_sources) \
	src/ags/lib/combo_box_text.c		\
	src/ags/lib/container.c			\
	src/ags/lib/list.c			\
	src/ags/lib/log.c			\
	src/ags/lib/parameter.c

deprecated_ags_util_h_sources =

ags_util_h_sources = \
	$(deprecated_ags_util_h_sources) \
	src/ags/util/ags_id_generator.h

deprecated_ags_util_c_sources = 

ags_util_c_sources = \
	$(deprecated_ags_util_c_sources) \
	src/ags/util/ags_id_generator.c


# libags_audio all files
deprecated_ags_audio_h_sources = \
	src/ags/audio/garbage_collector.h	\
	src/ags/audio/effect.h		\
	src/ags/audio/libao.h		\
	src/ags/audio/timestamp.h		\
	src/ags/audio/timestamp_factory.h	

ags_audio_h_sources = 		\
	$(deprecated_ags_audio_h_sources) \
	src/ags/audio/acceleration.h	\
	src/ags/audio/audio.h		\
	src/ags/audio/audio_signal.h	\
	src/ags/audio/automation.h	\
	src/ags/audio/channel.h		\
	src/ags/audio/channel_iter.h	\
	src/ags/audio/config.h		\
	src/ags/audio/devout.h		\
	src/ags/audio/input.h		\
	src/ags/audio/notation.h		\
	src/ags/audio/note.h		\
	src/ags/audio/output.h		\
	src/ags/audio/pattern.h		\
	src/ags/audio/port.h		\
	src/ags/audio/recall_audio.h	\
	src/ags/audio/recall_audio_run.h	\
	src/ags/audio/recall_audio_signal.h \
	src/ags/audio/recall_channel.h	\
	src/ags/audio/recall_channel_run_dummy.h \
	src/ags/audio/recall_channel_run.h \
	src/ags/audio/recall_container.h	\
	src/ags/audio/recall_dependency.h	\
	src/ags/audio/recall.h		\
	src/ags/audio/recall_id.h		\
	src/ags/audio/recall_ladspa.h	\
	src/ags/audio/recall_ladspa_run.h	\
	src/ags/audio/recall_recycling_dummy.h \
	src/ags/audio/recall_recycling.h	\
	src/ags/audio/recall_recycling_container.h \
	src/ags/audio/recycling.h		\
	src/ags/audio/synths.h		\
	src/ags/audio/task.h		\
	src/ags/audio/timestamp_factory.h	\
	src/ags/audio/timestamp.h

deprecated_ags_audio_c_sources =  \
	src/ags/audio/garbage_collector.c	\
	src/ags/audio/effect.c		\
	src/ags/audio/libao.c		\
	src/ags/audio/timestamp.c		\
	src/ags/audio/timestamp_factory.c	

ags_audio_c_sources =  		\
	$(deprecated_ags_audio_c_sources) \
	src/ags/audio/acceleration.c	\
	src/ags/audio/audio.c		\
	src/ags/audio/audio_signal.c	\
	src/ags/audio/automation.c	\
	src/ags/audio/channel.c		\
	src/ags/audio/channel_iter.c	\
	src/ags/audio/config.c		\
	src/ags/audio/devout.c		\
	src/ags/audio/input.c		\
	src/ags/audio/notation.c		\
	src/ags/audio/note.c		\
	src/ags/audio/output.c		\
	src/ags/audio/pattern.c		\
	src/ags/audio/port.c		\
	src/ags/audio/recall_audio.c	\
	src/ags/audio/recall_audio_run.c	\
	src/ags/audio/recall_audio_signal.c \
	src/ags/audio/recall_channel.c	\
	src/ags/audio/recall_channel_run_dummy.c \
	src/ags/audio/recall_channel_run.c \
	src/ags/audio/recall_container.c	\
	src/ags/audio/recall_dependency.c	\
	src/ags/audio/recall.c		\
	src/ags/audio/recall_id.c		\
	src/ags/audio/recall_ladspa.c	\
	src/ags/audio/recall_ladspa_run.c	\
	src/ags/audio/recall_recycling_dummy.c \
	src/ags/audio/recall_recycling.c	\
	src/ags/audio/recall_recycling_container.c \
	src/ags/audio/recycling.c		\
	src/ags/audio/synths.c		\
	src/ags/audio/task.c		\
	src/ags/audio/timestamp_factory.c	\
	src/ags/audio/timestamp.c

# gsequencer all files
gsequencer_main_h_sources = \
	src/ags/main.h

gsequencer_main_c_sources = \
	src/ags/main.c

deprecated_gsequencer_xorg_h_sources =

gsequencer_xorg_h_sources = \
	$(deprecated_gsequencer_xorg_h_sources)	\
	src/ags/X/ags_audio_preferences_callbacks.h	\
	src/ags/X/ags_audio_preferences.h		\
	src/ags/X/ags_automation_editor_callbacks.h	\
	src/ags/X/ags_automation_editor.h		\
	src/ags/X/ags_editor_callbacks.h		\
	src/ags/X/ags_editor.h				\
	src/ags/X/ags_effect_bridge_callbacks.h		\
	src/ags/X/ags_effect_bridge.h			\
	src/ags/X/effect_bulk_callbacks.h		\
	src/ags/X/effect_bulk.h			\
	src/ags/X/effect_line_callbacks.h		\
	src/ags/X/effect_line.h			\
	src/ags/X/effect_pad_callbacks.h		\
	src/ags/X/effect_pad.h			\
	src/ags/X/export_window_callbacks.h		\
	src/ags/X/export_window.h			\
	src/ags/X/generic_preferences_callbacks.h	\
	src/ags/X/generic_preferences.h		\
	src/ags/X/ladspa_browser_callbacks.h		\
	src/ags/X/ladspa_browser.h			\
	src/ags/X/line_callbacks.h 			\
	src/ags/X/line_editor_callbacks.h		\
	src/ags/X/line_editor.h			\
	src/ags/X/line.h				\
	src/ags/X/line_member_callbacks.h		\
	src/ags/X/line_member_editor_callbacks.h	\
	src/ags/X/line_member_editor.h		\
	src/ags/X/line_member.h			\
	src/ags/X/link_collection_editor_callbacks.h	\
	src/ags/X/link_collection_editor.h		\
	src/ags/X/link_editor_callbacks.h		\
	src/ags/X/link_editor.h			\
	src/ags/X/listing_editor_callbacks.h		\
	src/ags/X/listing_editor.h			\
	src/ags/X/machine_callbacks.h			\
	src/ags/X/machine_editor_callbacks.h		\
	src/ags/X/machine_editor.h			\
	src/ags/X/machine.h				\
	src/ags/X/menu_bar_callbacks.h		\
	src/ags/X/menu_bar.h 				\
	src/ags/X/navigation_callbacks.h		\
	src/ags/X/navigation.h			\
	src/ags/X/pad_callbacks.h			\
	src/ags/X/pad_editor_callbacks.h 		\
	src/ags/X/pad_editor.h			\
	src/ags/X/pad.h				\
	src/ags/X/performance_preferences_callbacks.h	\
	src/ags/X/performance_preferences.h		\
	src/ags/X/plugin_preferences_callbacks.h	\
	src/ags/X/plugin_preferences.h		\
	src/ags/X/preferences_callbacks.h		\
	src/ags/X/preferences.h			\
	src/ags/X/property_collection_editor_callbacks.h \
	src/ags/X/property_collection_editor.h	\
	src/ags/X/property_editor_callbacks.h		\
	src/ags/X/property_editor.h			\
	src/ags/X/property_listing_editor.h		\
	src/ags/X/resize_editor.h			\
	src/ags/X/server_preferences_callbacks.h	\
	src/ags/X/server_preferences.h		\
	src/ags/X/window_callbacks.h			\
	src/ags/X/window.h

deprecated_gsequencer_xorg_c_sources = 

gsequencer_xorg_c_sources = \
	$(deprecated_gsequencer_xorg_c_sources) 	\
	src/ags/X/audio_preferences_callbacks.c		\
	src/ags/X/audio_preferences.c			\
	src/ags/X/automation_editor_callbacks.c		\
	src/ags/X/automation_editor.c			\
	src/ags/X/editor_callbacks.c			\
	src/ags/X/editor.c				\
	src/ags/X/effect_bridge_callbacks.c		\
	src/ags/X/effect_bridge.c			\
	src/ags/X/effect_bulk_callbacks.c		\
	src/ags/X/effect_bulk.c				\
	src/ags/X/effect_line_callbacks.c		\
	src/ags/X/effect_line.c				\
	src/ags/X/effect_pad_callbacks.c		\
	src/ags/X/effect_pad.c				\
	src/ags/X/export_window_callbacks.c		\
	src/ags/X/export_window.c			\
	src/ags/X/generic_preferences_callbacks.c	\
	src/ags/X/generic_preferences.c			\
	src/ags/X/ladspa_browser_callbacks.c		\
	src/ags/X/ladspa_browser.c			\
	src/ags/X/line_callbacks.c 			\
	src/ags/X/line_editor_callbacks.c		\
	src/ags/X/line_editor.c				\
	src/ags/X/line.c				\
	src/ags/X/line_member_callbacks.c		\
	src/ags/X/line_member_editor_callbacks.c	\
	src/ags/X/line_member_editor.c			\
	src/ags/X/line_member.c				\
	src/ags/X/link_collection_editor_callbacks.c	\
	src/ags/X/link_collection_editor.c		\
	src/ags/X/link_editor_callbacks.c		\
	src/ags/X/link_editor.c				\
	src/ags/X/listing_editor_callbacks.c		\
	src/ags/X/listing_editor.c			\
	src/ags/X/machine_callbacks.c			\
	src/ags/X/machine_editor_callbacks.c		\
	src/ags/X/machine_editor.c			\
	src/ags/X/machine.c				\
	src/ags/X/menu_bar_callbacks.c			\
	src/ags/X/menu_bar.c 				\
	src/ags/X/navigation_callbacks.c		\
	src/ags/X/navigation.c				\
	src/ags/X/pad_callbacks.c			\
	src/ags/X/pad_editor_callbacks.c 		\
	src/ags/X/pad_editor.c				\
	src/ags/X/pad.c					\
	src/ags/X/performance_preferences_callbacks.c	\
	src/ags/X/performance_preferences.c		\
	src/ags/X/plugin_preferences_callbacks.c	\
	src/ags/X/plugin_preferences.c			\
	src/ags/X/preferences_callbacks.c		\
	src/ags/X/preferences.c				\
	src/ags/X/property_collection_editor_callbacks.c \
	src/ags/X/property_collection_editor.c		\
	src/ags/X/property_editor_callbacks.c		\
	src/ags/X/property_editor.c			\
	src/ags/X/property_listing_editor.c		\
	src/ags/X/resize_editor.c			\
	src/ags/X/server_preferences_callbacks.c	\
	src/ags/X/server_preferences.c			\
	src/ags/X/window_callbacks.c			\
	src/ags/X/window.c
