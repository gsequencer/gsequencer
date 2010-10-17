CC = gcc

GTK_DIR = /usr/include/gtk/
AGS_DIR = ./


AGS_AUDIO_OBJECTS = $(AGS_DIR)audio/ags_devout.o $(AGS_DIR)audio/ags_task.o $(AGS_DIR)audio/ags_audio.o $(AGS_DIR)audio/ags_channel.o $(AGS_DIR)audio/ags_input.o $(AGS_DIR)audio/ags_output.o $(AGS_DIR)audio/ags_recycling.o $(AGS_DIR)audio/ags_audio_signal.o $(AGS_DIR)audio/ags_pattern.o $(AGS_DIR)audio/ags_notation.o $(AGS_DIR)audio/ags_recall.o $(AGS_DIR)audio/ags_recall_id.o $(AGS_DIR)audio/ags_recall_audio.o $(AGS_DIR)audio/ags_recall_audio_run.o $(AGS_DIR)audio/ags_recall_channel.o $(AGS_DIR)audio/ags_recall_channel_run.o $(AGS_DIR)audio/ags_garbage_collector.o $(AGS_DIR)audio/ags_synths.o

AGS_TASK_OBJECTS = $(AGS_DIR)audio/task/ags_open_file.o $(AGS_DIR)audio/task/ags_link_channel.o $(AGS_DIR)audio/task/ags_change_bpm.o

AGS_RECALL_OBJECTS = $(AGS_DIR)audio/recall/ags_loop_channel.o $(AGS_DIR)audio/recall/ags_stream_channel.o $(AGS_DIR)audio/recall/ags_stream_recycling.o $(AGS_DIR)audio/recall/ags_stream_audio_signal.o $(AGS_DIR)audio/recall/ags_stream.o $(AGS_DIR)audio/recall/ags_copy_pattern_channel_run.o $(AGS_DIR)audio/recall/ags_copy_pattern_audio.o $(AGS_DIR)audio/recall/ags_copy_pattern_audio_run.o $(AGS_DIR)audio/recall/ags_copy_pattern_channel.o $(AGS_DIR)audio/recall/ags_copy_channel.o $(AGS_DIR)audio/recall/ags_copy_recycling.o $(AGS_DIR)audio/recall/ags_copy_audio_signal.o $(AGS_DIR)audio/recall/ags_play_audio_file.o $(AGS_DIR)audio/recall/ags_play_notation.o $(AGS_DIR)audio/recall/ags_play_note.o $(AGS_DIR)audio/recall/ags_play_pattern.o $(AGS_DIR)audio/recall/ags_play_channel.o $(AGS_DIR)audio/recall/ags_play_recycling.o $(AGS_DIR)audio/recall/ags_play_audio_signal.o $(AGS_DIR)audio/recall/ags_volume_channel.o $(AGS_DIR)audio/recall/ags_volume_recycling.o $(AGS_DIR)audio/recall/ags_volume_audio_signal.o $(AGS_DIR)audio/recall/ags_delay_audio_run.o $(AGS_DIR)audio/recall/ags_delay_audio.o $(AGS_DIR)audio/recall/ags_amplifier.o $(AGS_DIR)audio/recall/ags_echo.o

AGS_AUDIO_FILE_OBJECTS = $(AGS_DIR)audio/file/ags_audio_file.o $(AGS_DIR)audio/file/ags_audio_file_wav.o $(AGS_DIR)audio/file/ags_audio_file_xml.o $(AGS_DIR)audio/file/ags_audio_file_raw.o $(AGS_DIR)audio/file/ags_audio_file_ogg.o $(AGS_DIR)audio/file/ags_audio_file_mp3.o


AGS_FILE_OBJECTS = $(AGS_DIR)file/ags_file.o

AGS_WIDGET_OBJECTS = $(AGS_DIR)widget/ags_timebar.o $(AGS_DIR)widget/ags_htimebar.o $(AGS_DIR)widget/ags_table.o $(AGS_DIR)widget/ags_menu.o $(AGS_DIR)widget/ags_option_menu.o

AGS_X_OBJECTS = $(AGS_DIR)X/ags_window.o $(AGS_DIR)X/ags_menu_bar.o $(AGS_DIR)X/ags_machine.o $(AGS_DIR)X/ags_pad.o $(AGS_DIR)X/ags_line.o $(AGS_DIR)X/ags_editor.o $(AGS_DIR)X/ags_navigation.o $(AGS_DIR)X/ags_machine_editor.o $(AGS_DIR)X/ags_property_editor.o $(AGS_DIR)X/ags_property_listing_editor.o $(AGS_DIR)X/ags_property_collection_editor.o $(AGS_DIR)X/ags_channel_listing_editor.o $(AGS_DIR)X/ags_channel_link_collection_editor.o $(AGS_DIR)X/ags_channel_resize_editor.o $(AGS_DIR)X/ags_pad_editor.o $(AGS_DIR)X/ags_line_editor.o $(AGS_DIR)X/ags_link_editor.o $(AGS_DIR)X/ags_line_member_editor.o 

AGS_X_CALLBACK_OBJECTS = $(AGS_DIR)X/ags_window_callbacks.o $(AGS_DIR)X/ags_menu_bar_callbacks.o $(AGS_DIR)X/ags_machine_callbacks.o $(AGS_DIR)X/ags_pad_callbacks.o $(AGS_DIR)X/ags_line_callbacks.o $(AGS_DIR)X/ags_editor_callbacks.o $(AGS_DIR)X/ags_navigation_callbacks.o $(AGS_DIR)X/ags_machine_editor_callbacks.o $(AGS_DIR)X/ags_property_editor_callbacks.o $(AGS_DIR)X/ags_property_collection_editor_callbacks.o $(AGS_DIR)X/ags_channel_listing_editor_callbacks.o $(AGS_DIR)X/ags_channel_link_collection_editor_callbacks.o $(AGS_DIR)X/ags_pad_editor_callbacks.o $(AGS_DIR)X/ags_line_editor_callbacks.o $(AGS_DIR)X/ags_link_editor_callbacks.o $(AGS_DIR)X/ags_line_member_editor_callbacks.o

AGS_EDITOR_OBJECTS = $(AGS_DIR)X/editor/ags_toolbar.o $(AGS_DIR)X/editor/ags_notebook.o $(AGS_DIR)X/editor/ags_meter.o $(AGS_DIR)X/editor/ags_ruler.o
AGS_EDITOR_CALLBACK_OBJECTS = $(AGS_DIR)X/editor/ags_toolbar_callbacks.o $(AGS_DIR)X/editor/ags_notebook_callbacks.o $(AGS_DIR)X/editor/ags_meter_callbacks.o $(AGS_DIR)X/editor/ags_ruler_callbacks.o

AGS_MACHINE_OBJECTS = $(AGS_DIR)X/machine/ags_panel.o $(AGS_DIR)X/machine/ags_mixer.o $(AGS_DIR)X/machine/ags_mixer_input_pad.o $(AGS_DIR)X/machine/ags_mixer_input_line.o $(AGS_DIR)X/machine/ags_drum.o $(AGS_DIR)X/machine/ags_drum_output_pad.o $(AGS_DIR)X/machine/ags_drum_input_pad.o $(AGS_DIR)X/machine/ags_drum_output_line.o $(AGS_DIR)X/machine/ags_drum_input_line.o $(AGS_DIR)X/machine/ags_matrix.o $(AGS_DIR)X/machine/ags_synth.o $(AGS_DIR)X/machine/ags_oscillator.o $(AGS_DIR)X/machine/ags_ffplayer.o

AGS_MACHINE_CALLBACK_OBJECTS = $(AGS_DIR)X/machine/ags_panel_callbacks.o $(AGS_DIR)X/machine/ags_mixer_callbacks.o $(AGS_DIR)X/machine/ags_drum_callbacks.o $(AGS_DIR)X/machine/ags_drum_input_pad_callbacks.o $(AGS_DIR)X/machine/ags_drum_input_line_callbacks.o $(AGS_DIR)X/machine/ags_drum_output_pad_callbacks.o $(AGS_DIR)X/machine/ags_drum_output_line_callbacks.o $(AGS_DIR)X/machine/ags_matrix_callbacks.o $(AGS_DIR)X/machine/ags_synth_callbacks.o $(AGS_DIR)X/machine/ags_oscillator_callbacks.o $(AGS_DIR)X/machine/ags_ffplayer_callbacks.o

AGS_OBJECT_OBJECTS = $(AGS_DIR)object/ags_connectable.o $(AGS_DIR)object/ags_run_connectable.o $(AGS_DIR)object/ags_runnable.o $(AGS_DIR)object/ags_applicable.o $(AGS_DIR)object/ags_marshal.o

AGS_OBJECTS = $(AGS_DIR)main.o $(AGS_OBJECT_OBJECTS) $(AGS_AUDIO_OBJECTS) $(AGS_TASK_OBJECTS) $(AGS_RECALL_OBJECTS) $(AGS_AUDIO_FILE_OBJECTS) $(AGS_FILE_OBJECTS) $(AGS_WIDGET_OBJECTS) $(AGS_X_OBJECTS) $(AGS_X_CALLBACK_OBJECTS) $(AGS_EDITOR_OBJECTS) $(AGS_EDITOR_CALLBACK_OBJECTS) $(AGS_MACHINE_OBJECTS) $(AGS_MACHINE_CALLBACK_OBJECTS)


AGS_OBJECT_SOURCES = $(wildcard $(AGS_DIR)object/*.[ch])

AGS_AUDIO_SOURCES = $(wildcard $(AGS_DIR)audio/*.[ch])
AGS_RECALL_SOURCES = $(wildcard $(AGS_DIR)audio/recall/*.[ch])
AGS_AUDIO_FILE_SOURCES = $(wildcard $(AGS_DIR)audio/audio_file/*.[ch])

AGS_WIDGET_SOURCES = $(wildcard $(AGS_DIR)widget/*.[ch])

AGS_X_SOURCES = $(wildcard $(AGS_DIR)X/*.[ch])
AGS_EDITOR_SOURCES = $(wildcard $(AGS_DIR)X/editor/*.[ch])
AGS_MACHINE_SOURCES = $(wildcard $(AGS_DIR)X/machine/*.[ch])

AGS_FILE_SOURCES = $(wildcard $(AGS_DIR)file/*.[ch])

AGS_SOURCES = $(AGS_DIR)main.[ch] $(AGS_OBJECT_SOURCES) $(AGS_AUDIO_SOURCES) $(AGS_RECALL_SOURCES) $(AGS_AUDIO_FILE_SOURCES) $(AGS_WIDGET_SOURCES) $(AGS_X_SOURCES) $(AGS_EDITOR_SOURCES) $(AGS_MACHINE_SOURCES) $(AGS_FILE_SOURCES)


LIBS = `pkg-config --libs alsa gthread-2.0 glib-2.0 gobject-2.0 gdk-2.0 gdk-pixbuf-xlib-2.0 gmodule-2.0 gtk+-2.0 libxml-2.0 sndfile`

CFLAGS = `pkg-config --cflags alsa gthread-2.0 glib-2.0 gobject-2.0 gdk-2.0 gdk-pixbuf-xlib-2.0 gmodule-2.0 gtk+-2.0 libxml-2.0 sndfile`


$(AGS_DIR)ags: $(AGS_OBJECTS)
	$(CC) -g -o $(AGS_DIR)ags -ldl -lm -lao $(AGS_OBJECTS) $(LIBS)


$(AGS_DIR).c.o: $(AGS_X_SOURCES) $(AGS_EDITOR_SOURCES) $(AGS_MACHINE_SOURCES)
	$(CC) -g -c $< -o $@ $(CFLAGS)


$(AGS_DIR)object/.c.o: $(AGS_OBJECT_SOURCES)
	$(CC) -g -c $< -o $@ $(CFLAGS)


$(AGS_DIR)audio/.c.o: $(AGS_AUDIO_SOURCES) $(AGS_RECALL_SOURCES) $(AGS_AUDIO_FILE_SOURCES) $(AGS_OBJECT_SOURCES)
	$(CC) -g -c $(AGS_DIR)audio/$< -o $(AGS_DIR)audio/$@ $(CFLAGS)

$(AGS_DIR)audio/recall/.c.o: $(AGS_AUDIO_SOURCES) $(AGS_RECALL_SOURCES) $(AGS_AUDIO_FILE_SOURCES) $(AGS_OBJECT_SOURCES)
	$(CC) -g -c $(AGS_DIR)audio/recall/$< -o $(AGS_DIR)audio/recall/$@ $(CFLAGS)

$(AGS_DIR)audio/audio_file/.c.o: $(AGS_AUDIO_SOURCES) $(AGS_RECALL_SOURCES) $(AGS_AUDIO_FILE_SOURCES) $(AGS_OBJECT_SOURCES)
	$(CC) -g -c $(AGS_DIR)audio/audio_file/$< -o $(AGS_DIR)audio/audio_file/$@ $(CFLAGS)


$(AGS_DIR)widget/.c.o: $(AGS_WIDGET_SOURCES)
	$(CC) -g -c $(AGS_DIR)X/$< -o $(AGS_DIR)X/$@ $(CFLAGS)


$(AGS_DIR)X/.c.o: $(AGS_SOURCES)
	$(CC) -g -c $(AGS_DIR)X/$< -o $(AGS_DIR)X/$@ $(CFLAGS)

$(AGS_DIR)X/editor/.c.o: $(AGS_SOURCES)
	$(CC) -g -c $(AGS_DIR)X/editor/$< -o $(AGS_DIR)X/editor/$@ $(CFLAGS)

$(AGS_DIR)X/machine/.c.o: $(AGS_SOURCES)
	$(CC) -g -c $(AGS_DIR)X/machine/$< -o $(AGS_DIR)X/machine/$@ $(CFLAGS)


$(AGS_DIR)file/.c.o: $(AGS_SOURCES)
	$(CC) -g -c $(AGS_DIR)file/$< -o $(AGS_DIR)file/$@ $(CFLAGS)


release:
	./archivate.sh

clean:
	rm $(wildcard $(AGS_DIR)*.o) $(wildcard $(AGS_DIR)object/*.o) $(wildcard $(AGS_DIR)audio/*.o) $(wildcard $(AGS_DIR)audio/recall/*.o) $(wildcard $(AGS_DIR)audio/file/*.o) $(wildcard $(AGS_DIR)widget/*.o) $(wildcard $(AGS_DIR)X/*.o) $(wildcard $(AGS_DIR)X/editor/*.o) $(wildcard $(AGS_DIR)X/machine/*.o) $(wildcard $(AGS_DIR)file/*.o)
