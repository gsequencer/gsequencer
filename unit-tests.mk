# Copyright (C) 2005-2018 Jo\u00EBl Kr\u00E4hemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

# unit tests - libags
check_PROGRAMS += \
	ags_buffer_util_test \
	ags_complex_test \
	ags_conversion_test \
	ags_log_test \
	ags_time_test \
	ags_turtle_manager_test \
	ags_turtle_test \
	ags_application_context_test \
	ags_config_test \
	ags_condition_manager_test \
	ags_destroy_worker_test \
	ags_mutex_manager_test \
	ags_poll_fd_test \
	ags_polling_thread_test \
	ags_returnable_thread_test \
	ags_task_test \
	ags_task_thread_test \
	ags_thread_test \
	ags_thread_pool_test \
	ags_worker_thread_test \
	ags_thread_file_xml_test \
	ags_file_test \
	ags_file_id_ref_test \
	ags_file_launch_test \
	ags_file_lookup_test

# unit tests - libags-audio
check_PROGRAMS += \
	ags_base_plugin_test \
	ags_dssi_manager_test \
	ags_dssi_plugin_test \
	ags_ladspa_conversion_test \
	ags_ladspa_manager_test \
	ags_lv2_conversion_test \
	ags_lv2_manager_test \
	ags_lv2_option_manager_test \
	ags_lv2_plugin_test \
	ags_lv2_preset_test \
	ags_lv2_uri_map_manager_test \
	ags_lv2_urid_manager_test \
	ags_lv2_worker_manager_test \
	ags_lv2ui_manager_test \
	ags_lv2ui_plugin_test \
	ags_plugin_port_test

check_PROGRAMS += \
	ags_audio_application_context_test \
	ags_devin_test \
	ags_devout_test \
	ags_fifoout_test \
	ags_midiin_test \
	ags_audio_test \
	ags_playback_domain_test \
	ags_playback_test \
	ags_preset_test \
	ags_channel_test \
	ags_input_test \
	ags_output_test \
	ags_recycling_test \
	ags_audio_signal_test \
	ags_audio_buffer_util_test \
	ags_char_buffer_util_test \
	ags_recall_test \
	ags_recall_channel_test \
	ags_recall_channel_run_test \
	ags_recall_container_test \
	ags_recall_dependency_test \
	ags_recall_id_test \
	ags_recall_recycling_test \
	ags_recycling_context_test \
	ags_synth_generator_test \
	ags_port_test \
	ags_pattern_test \
	ags_notation_test \
	ags_note_test \
	ags_automation_test \
	ags_acceleration_test \
	ags_wave_test \
	ags_buffer_test \
	ags_midi_test \
	ags_track_test \
	ags_midi_buffer_util_test \
	ags_midi_builder_test

check_PROGRAMS += \
	ags_analyse_audio_signal_test \
	ags_analyse_channel_test \
	ags_buffer_audio_signal_test \
	ags_buffer_channel_test \
	ags_capture_wave_audio_test \
	ags_capture_wave_channel_test \
	ags_copy_audio_signal_test \
	ags_copy_channel_test \
	ags_copy_pattern_audio_test \
	ags_copy_pattern_channel_test \
	ags_count_beats_audio_test \
	ags_delay_audio_test \
	ags_envelope_audio_signal_test \
	ags_envelope_channel_test \
	ags_eq10_audio_signal_test \
	ags_eq10_channel_test \
	ags_feed_audio_signal_test \
	ags_mute_audio_test \
	ags_mute_audio_signal_test \
	ags_mute_channel_test \
	ags_peak_audio_signal_test \
	ags_peak_channel_test \
	ags_play_audio_test \
	ags_play_audio_signal_test \
	ags_play_channel_test \
	ags_play_wave_audio_test \
	ags_play_wave_channel_test \
	ags_prepare_audio_signal_test \
	ags_stream_audio_signal_test \
	ags_volume_audio_signal_test

# unit tests - libgsequencer
check_PROGRAMS += \
	ags_xorg_application_context_test

# buffer util unit test
ags_buffer_util_test_SOURCES = ags/test/lib/ags_buffer_util_test.c
ags_buffer_util_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_buffer_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_buffer_util_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# complex unit test
ags_complex_test_SOURCES = ags/test/lib/ags_complex_test.c
ags_complex_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_complex_test_LDFLAGS = -pthread $(LDFLAGS)
ags_complex_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# conversion unit test
ags_conversion_test_SOURCES = ags/test/lib/ags_conversion_test.c
ags_conversion_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_conversion_test_LDFLAGS = -pthread $(LDFLAGS)
ags_conversion_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# log unit test
ags_log_test_SOURCES = ags/test/lib/ags_log_test.c
ags_log_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_log_test_LDFLAGS = -pthread $(LDFLAGS)
ags_log_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# time unit test
ags_time_test_SOURCES = ags/test/lib/ags_time_test.c
ags_time_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_time_test_LDFLAGS = -pthread $(LDFLAGS)
ags_time_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# turtle_manager unit test
ags_turtle_manager_test_SOURCES = ags/test/lib/ags_turtle_manager_test.c
ags_turtle_manager_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_turtle_manager_test_LDFLAGS = -pthread $(LDFLAGS)
ags_turtle_manager_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# turtle unit test
ags_turtle_test_SOURCES = ags/test/lib/ags_turtle_test.c
ags_turtle_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_turtle_test_LDFLAGS = -pthread $(LDFLAGS)
ags_turtle_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# application context unit test
ags_application_context_test_SOURCES = ags/test/object/ags_application_context_test.c
ags_application_context_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_application_context_test_LDFLAGS = -pthread $(LDFLAGS)
ags_application_context_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# config unit test
ags_config_test_SOURCES = ags/test/object/ags_config_test.c
ags_config_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_config_test_LDFLAGS = -pthread $(LDFLAGS)
ags_config_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# condition manager unit test
ags_condition_manager_test_SOURCES = ags/test/thread/ags_condition_manager_test.c
ags_condition_manager_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_condition_manager_test_LDFLAGS = -pthread $(LDFLAGS)
ags_condition_manager_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# destroy worker unit test
ags_destroy_worker_test_SOURCES = ags/test/thread/ags_destroy_worker_test.c
ags_destroy_worker_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_destroy_worker_test_LDFLAGS = -pthread $(LDFLAGS)
ags_destroy_worker_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# mutex manager unit test
ags_mutex_manager_test_SOURCES = ags/test/thread/ags_mutex_manager_test.c
ags_mutex_manager_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_mutex_manager_test_LDFLAGS = -pthread $(LDFLAGS)
ags_mutex_manager_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# poll fd unit test
ags_poll_fd_test_SOURCES = ags/test/thread/ags_poll_fd_test.c
ags_poll_fd_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_poll_fd_test_LDFLAGS = -pthread $(LDFLAGS)
ags_poll_fd_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# polling thread unit test
ags_polling_thread_test_SOURCES = ags/test/thread/ags_polling_thread_test.c
ags_polling_thread_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_polling_thread_test_LDFLAGS = -pthread $(LDFLAGS)
ags_polling_thread_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# returnable thread unit test
ags_returnable_thread_test_SOURCES = ags/test/thread/ags_returnable_thread_test.c
ags_returnable_thread_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_returnable_thread_test_LDFLAGS = -pthread $(LDFLAGS)
ags_returnable_thread_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# task unit test
ags_task_test_SOURCES = ags/test/thread/ags_task_test.c
ags_task_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_task_test_LDFLAGS = -lcunit -lm -pthread -lrt $(LDFLAGS) $(LIBXML2_LIBS) $(GOBJECT_LIBS)
ags_task_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# task thread unit test
ags_task_thread_test_SOURCES = ags/test/thread/ags_task_thread_test.c
ags_task_thread_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_task_thread_test_LDFLAGS = -pthread $(LDFLAGS)
ags_task_thread_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# thread unit test
ags_thread_test_SOURCES = ags/test/thread/ags_thread_test.c
ags_thread_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_thread_test_LDFLAGS = -lcunit -lm -pthread -lrt $(LDFLAGS) $(LIBXML2_LIBS) $(GOBJECT_LIBS)
ags_thread_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# thread pool unit test
ags_thread_pool_test_SOURCES = ags/test/thread/ags_thread_pool_test.c
ags_thread_pool_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_thread_pool_test_LDFLAGS = -pthread $(LDFLAGS)
ags_thread_pool_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# worker thread unit test
ags_worker_thread_test_SOURCES = ags/test/thread/ags_worker_thread_test.c
ags_worker_thread_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_worker_thread_test_LDFLAGS = -pthread $(LDFLAGS)
ags_worker_thread_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# thread file xml unit test
ags_thread_file_xml_test_SOURCES = ags/test/thread/file/ags_thread_file_xml_test.c
ags_thread_file_xml_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_thread_file_xml_test_LDFLAGS = -pthread $(LDFLAGS)
ags_thread_file_xml_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# file unit test
ags_file_test_SOURCES = ags/test/file/ags_file_test.c
ags_file_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_file_test_LDFLAGS = -pthread $(LDFLAGS)
ags_file_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# file id ref unit test
ags_file_id_ref_test_SOURCES = ags/test/file/ags_file_id_ref_test.c
ags_file_id_ref_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_file_id_ref_test_LDFLAGS = -pthread $(LDFLAGS)
ags_file_id_ref_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# file launch unit test
ags_file_launch_test_SOURCES = ags/test/file/ags_file_launch_test.c
ags_file_launch_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_file_launch_test_LDFLAGS = -pthread $(LDFLAGS)
ags_file_launch_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# file lookup unit test
ags_file_lookup_test_SOURCES = ags/test/file/ags_file_lookup_test.c
ags_file_lookup_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_file_lookup_test_LDFLAGS = -pthread $(LDFLAGS)
ags_file_lookup_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# base plugin unit test
ags_base_plugin_test_SOURCES = ags/test/plugin/ags_base_plugin_test.c
ags_base_plugin_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_base_plugin_test_LDFLAGS = $(LDFLAGS) -pthread
ags_base_plugin_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# dssi manager unit test
ags_dssi_manager_test_SOURCES = ags/test/plugin/ags_dssi_manager_test.c
ags_dssi_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_dssi_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_dssi_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# dssi plugin unit test
ags_dssi_plugin_test_SOURCES = ags/test/plugin/ags_dssi_plugin_test.c
ags_dssi_plugin_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_dssi_plugin_test_LDFLAGS = $(LDFLAGS) -pthread
ags_dssi_plugin_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# ladspa conversion unit test
ags_ladspa_conversion_test_SOURCES = ags/test/plugin/ags_ladspa_conversion_test.c
ags_ladspa_conversion_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_ladspa_conversion_test_LDFLAGS = $(LDFLAGS) -pthread
ags_ladspa_conversion_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# ladspa manager unit test
ags_ladspa_manager_test_SOURCES = ags/test/plugin/ags_ladspa_manager_test.c
ags_ladspa_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_ladspa_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_ladspa_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# lv2 conversion unit test
ags_lv2_conversion_test_SOURCES = ags/test/plugin/ags_lv2_conversion_test.c
ags_lv2_conversion_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_lv2_conversion_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2_conversion_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# lv2 manager unit test
ags_lv2_manager_test_SOURCES = ags/test/plugin/ags_lv2_manager_test.c
ags_lv2_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_lv2_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# lv2 option manager unit test
ags_lv2_option_manager_test_SOURCES = ags/test/plugin/ags_lv2_option_manager_test.c
ags_lv2_option_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_lv2_option_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2_option_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# lv2 plugin unit test
ags_lv2_plugin_test_SOURCES = ags/test/plugin/ags_lv2_plugin_test.c
ags_lv2_plugin_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_lv2_plugin_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2_plugin_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# lv2 preset unit test
ags_lv2_preset_test_SOURCES = ags/test/plugin/ags_lv2_preset_test.c
ags_lv2_preset_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_lv2_preset_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2_preset_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# lv2 uri map manager unit test
ags_lv2_uri_map_manager_test_SOURCES = ags/test/plugin/ags_lv2_uri_map_manager_test.c
ags_lv2_uri_map_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_lv2_uri_map_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2_uri_map_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# lv2 urid manager unit test
ags_lv2_urid_manager_test_SOURCES = ags/test/plugin/ags_lv2_urid_manager_test.c
ags_lv2_urid_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_lv2_urid_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2_urid_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# lv2 worker manager unit test
ags_lv2_worker_manager_test_SOURCES = ags/test/plugin/ags_lv2_worker_manager_test.c
ags_lv2_worker_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_lv2_worker_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2_worker_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# lv2ui manager unit test
ags_lv2ui_manager_test_SOURCES = ags/test/plugin/ags_lv2ui_manager_test.c
ags_lv2ui_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_lv2ui_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2ui_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# lv2ui plugin unit test
ags_lv2ui_plugin_test_SOURCES = ags/test/plugin/ags_lv2ui_plugin_test.c
ags_lv2ui_plugin_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_lv2ui_plugin_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2ui_plugin_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# plugin port unit test
ags_plugin_port_test_SOURCES = ags/test/plugin/ags_plugin_port_test.c
ags_plugin_port_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_plugin_port_test_LDFLAGS = $(LDFLAGS) -pthread
ags_plugin_port_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# audio application context unit test
ags_audio_application_context_test_SOURCES = ags/test/audio/ags_audio_application_context_test.c
ags_audio_application_context_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_audio_application_context_test_LDFLAGS = $(LDFLAGS) -pthread
ags_audio_application_context_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# devin unit test
ags_devin_test_SOURCES = ags/test/audio/ags_devin_test.c
ags_devin_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_devin_test_LDFLAGS = -pthread $(LDFLAGS)
ags_devin_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# devout unit test
ags_devout_test_SOURCES = ags/test/audio/ags_devout_test.c
ags_devout_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_devout_test_LDFLAGS = -pthread $(LDFLAGS)
ags_devout_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# fifoout unit test
ags_fifoout_test_SOURCES = ags/test/audio/ags_fifoout_test.c
ags_fifoout_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_fifoout_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fifoout_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# midiin unit test
ags_midiin_test_SOURCES = ags/test/audio/ags_midiin_test.c
ags_midiin_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_midiin_test_LDFLAGS = -pthread $(LDFLAGS)
ags_midiin_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# audio unit test
ags_audio_test_SOURCES = ags/test/audio/ags_audio_test.c
ags_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# playback domain unit test
ags_playback_domain_test_SOURCES = ags/test/audio/ags_playback_domain_test.c
ags_playback_domain_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_playback_domain_test_LDFLAGS = -pthread $(LDFLAGS)
ags_playback_domain_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# playback unit test
ags_playback_test_SOURCES = ags/test/audio/ags_playback_test.c
ags_playback_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_playback_test_LDFLAGS = -pthread $(LDFLAGS)
ags_playback_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# preset unit test
ags_preset_test_SOURCES = ags/test/audio/ags_preset_test.c
ags_preset_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_preset_test_LDFLAGS = -pthread $(LDFLAGS)
ags_preset_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# channel unit test
ags_channel_test_SOURCES = ags/test/audio/ags_channel_test.c
ags_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# input unit test
ags_input_test_SOURCES = ags/test/audio/ags_input_test.c
ags_input_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_input_test_LDFLAGS = -pthread $(LDFLAGS)
ags_input_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# output unit test
ags_output_test_SOURCES = ags/test/audio/ags_output_test.c
ags_output_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_output_test_LDFLAGS = -pthread $(LDFLAGS)
ags_output_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# recycling unit test
ags_recycling_test_SOURCES = ags/test/audio/ags_recycling_test.c
ags_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# audio signal unit test
ags_audio_signal_test_SOURCES = ags/test/audio/ags_audio_signal_test.c
ags_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# audio buffer util unit test
ags_audio_buffer_util_test_SOURCES = ags/test/audio/ags_audio_buffer_util_test.c
ags_audio_buffer_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_audio_buffer_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_audio_buffer_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# char buffer util unit test
ags_char_buffer_util_test_SOURCES = ags/test/audio/ags_char_buffer_util_test.c
ags_char_buffer_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_char_buffer_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_char_buffer_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# recall unit test
ags_recall_test_SOURCES = ags/test/audio/ags_recall_test.c
ags_recall_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_recall_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recall_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# recall channel unit test
ags_recall_channel_test_SOURCES = ags/test/audio/ags_recall_channel_test.c
ags_recall_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_recall_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recall_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# recall channel run unit test
ags_recall_channel_run_test_SOURCES = ags/test/audio/ags_recall_channel_run_test.c
ags_recall_channel_run_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_recall_channel_run_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recall_channel_run_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# recall container unit test
ags_recall_container_test_SOURCES = ags/test/audio/ags_recall_container_test.c
ags_recall_container_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_recall_container_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recall_container_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# recall dependency unit test
ags_recall_dependency_test_SOURCES = ags/test/audio/ags_recall_dependency_test.c
ags_recall_dependency_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_recall_dependency_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recall_dependency_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# recall id unit test
ags_recall_id_test_SOURCES = ags/test/audio/ags_recall_id_test.c
ags_recall_id_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_recall_id_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recall_id_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# recall recycling unit test
ags_recall_recycling_test_SOURCES = ags/test/audio/ags_recall_recycling_test.c
ags_recall_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_recall_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recall_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# recycling context unit test
ags_recycling_context_test_SOURCES = ags/test/audio/ags_recycling_context_test.c
ags_recycling_context_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_recycling_context_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recycling_context_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# synth generator unit test
ags_synth_generator_test_SOURCES = ags/test/audio/ags_synth_generator_test.c
ags_synth_generator_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_synth_generator_test_LDFLAGS = -pthread $(LDFLAGS)
ags_synth_generator_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# port unit test
ags_port_test_SOURCES = ags/test/audio/ags_port_test.c
ags_port_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_port_test_LDFLAGS = -pthread $(LDFLAGS)
ags_port_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# pattern unit test
ags_pattern_test_SOURCES = ags/test/audio/ags_pattern_test.c
ags_pattern_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_pattern_test_LDFLAGS = -pthread $(LDFLAGS)
ags_pattern_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# notation unit test
ags_notation_test_SOURCES = ags/test/audio/ags_notation_test.c
ags_notation_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_notation_test_LDFLAGS = -pthread $(LDFLAGS)
ags_notation_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# note unit test
ags_note_test_SOURCES = ags/test/audio/ags_note_test.c
ags_note_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_note_test_LDFLAGS = -pthread $(LDFLAGS)
ags_note_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# automation unit test
ags_automation_test_SOURCES = ags/test/audio/ags_automation_test.c
ags_automation_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_automation_test_LDFLAGS = -pthread $(LDFLAGS)
ags_automation_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# acceleration unit test
ags_acceleration_test_SOURCES = ags/test/audio/ags_acceleration_test.c
ags_acceleration_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_acceleration_test_LDFLAGS = -pthread $(LDFLAGS)
ags_acceleration_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# wave unit test
ags_wave_test_SOURCES = ags/test/audio/ags_wave_test.c
ags_wave_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_wave_test_LDFLAGS = -pthread $(LDFLAGS)
ags_wave_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# buffer unit test
ags_buffer_test_SOURCES = ags/test/audio/ags_buffer_test.c
ags_buffer_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_buffer_test_LDFLAGS = -pthread $(LDFLAGS)
ags_buffer_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# midi unit test
ags_midi_test_SOURCES = ags/test/audio/ags_midi_test.c
ags_midi_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_midi_test_LDFLAGS = -pthread $(LDFLAGS)
ags_midi_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# track unit test
ags_track_test_SOURCES = ags/test/audio/ags_track_test.c
ags_track_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_track_test_LDFLAGS = -pthread $(LDFLAGS)
ags_track_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# midi buffer util unit test
ags_midi_buffer_util_test_SOURCES = ags/test/audio/midi/ags_midi_buffer_util_test.c
ags_midi_buffer_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_midi_buffer_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_midi_buffer_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# midi builder unit test
ags_midi_builder_test_SOURCES = ags/test/audio/midi/ags_midi_builder_test.c
ags_midi_builder_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_midi_builder_test_LDFLAGS = -pthread $(LDFLAGS)
ags_midi_builder_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# analyse audio signal unit test
ags_analyse_audio_signal_test_SOURCES = ags/test/audio/recall/ags_analyse_audio_signal_test.c
ags_analyse_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_analyse_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_analyse_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# analyse channel unit test
ags_analyse_channel_test_SOURCES = ags/test/audio/recall/ags_analyse_channel_test.c
ags_analyse_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_analyse_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_analyse_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# buffer audio signal unit test
ags_buffer_audio_signal_test_SOURCES = ags/test/audio/recall/ags_buffer_audio_signal_test.c
ags_buffer_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_buffer_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_buffer_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# buffer channel unit test
ags_buffer_channel_test_SOURCES = ags/test/audio/recall/ags_buffer_channel_test.c
ags_buffer_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_buffer_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_buffer_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# capture wave audio unit test
ags_capture_wave_audio_test_SOURCES = ags/test/audio/recall/ags_capture_wave_audio_test.c
ags_capture_wave_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_capture_wave_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_capture_wave_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# capture wave channel unit test
ags_capture_wave_channel_test_SOURCES = ags/test/audio/recall/ags_capture_wave_channel_test.c
ags_capture_wave_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_capture_wave_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_capture_wave_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# copy audio signal unit test
ags_copy_audio_signal_test_SOURCES = ags/test/audio/recall/ags_copy_audio_signal_test.c
ags_copy_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_copy_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_copy_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# copy channel unit test
ags_copy_channel_test_SOURCES = ags/test/audio/recall/ags_copy_channel_test.c
ags_copy_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_copy_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_copy_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# copy audio pattern unit test
ags_copy_pattern_audio_test_SOURCES = ags/test/audio/recall/ags_copy_pattern_audio_test.c
ags_copy_pattern_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_copy_pattern_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_copy_pattern_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# copy channel pattern unit test
ags_copy_pattern_channel_test_SOURCES = ags/test/audio/recall/ags_copy_pattern_channel_test.c
ags_copy_pattern_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_copy_pattern_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_copy_pattern_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# count beats audio unit test
ags_count_beats_audio_test_SOURCES = ags/test/audio/recall/ags_count_beats_audio_test.c
ags_count_beats_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_count_beats_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_count_beats_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# delay audio unit test
ags_delay_audio_test_SOURCES = ags/test/audio/recall/ags_delay_audio_test.c
ags_delay_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_delay_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_delay_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# envelope audio signal unit test
ags_envelope_audio_signal_test_SOURCES = ags/test/audio/recall/ags_envelope_audio_signal_test.c
ags_envelope_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_envelope_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_envelope_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# envelope channel unit test
ags_envelope_channel_test_SOURCES = ags/test/audio/recall/ags_envelope_channel_test.c
ags_envelope_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_envelope_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_envelope_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# eq10 audio signal unit test
ags_eq10_audio_signal_test_SOURCES = ags/test/audio/recall/ags_eq10_audio_signal_test.c
ags_eq10_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_eq10_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_eq10_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# eq10 channel unit test
ags_eq10_channel_test_SOURCES = ags/test/audio/recall/ags_eq10_channel_test.c
ags_eq10_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_eq10_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_eq10_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# feed audio signal unit test
ags_feed_audio_signal_test_SOURCES = ags/test/audio/recall/ags_feed_audio_signal_test.c
ags_feed_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_feed_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_feed_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# mute audio unit test
ags_mute_audio_test_SOURCES = ags/test/audio/recall/ags_mute_audio_test.c
ags_mute_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_mute_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_mute_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# mute audio signal unit test
ags_mute_audio_signal_test_SOURCES = ags/test/audio/recall/ags_mute_audio_signal_test.c
ags_mute_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_mute_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_mute_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# mute channel unit test
ags_mute_channel_test_SOURCES = ags/test/audio/recall/ags_mute_channel_test.c
ags_mute_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_mute_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_mute_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# peak audio signal unit test
ags_peak_audio_signal_test_SOURCES = ags/test/audio/recall/ags_peak_audio_signal_test.c
ags_peak_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_peak_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_peak_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# peak channel unit test
ags_peak_channel_test_SOURCES = ags/test/audio/recall/ags_peak_channel_test.c
ags_peak_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_peak_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_peak_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# play audio unit test
ags_play_audio_test_SOURCES = ags/test/audio/recall/ags_play_audio_test.c
ags_play_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_play_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_play_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# play audio signal unit test
ags_play_audio_signal_test_SOURCES = ags/test/audio/recall/ags_play_audio_signal_test.c
ags_play_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_play_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_play_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# play channel unit test
ags_play_channel_test_SOURCES = ags/test/audio/recall/ags_play_channel_test.c
ags_play_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_play_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_play_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# play wave audio unit test
ags_play_wave_audio_test_SOURCES = ags/test/audio/recall/ags_play_wave_audio_test.c
ags_play_wave_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_play_wave_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_play_wave_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# play wave channel unit test
ags_play_wave_channel_test_SOURCES = ags/test/audio/recall/ags_play_wave_channel_test.c
ags_play_wave_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_play_wave_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_play_wave_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# prepare audio signal unit test
ags_prepare_audio_signal_test_SOURCES = ags/test/audio/recall/ags_prepare_audio_signal_test.c
ags_prepare_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_prepare_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_prepare_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# stream audio signal unit test
ags_stream_audio_signal_test_SOURCES = ags/test/audio/recall/ags_stream_audio_signal_test.c
ags_stream_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_stream_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_stream_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# volume audio signal unit test
ags_volume_audio_signal_test_SOURCES = ags/test/audio/recall/ags_volume_audio_signal_test.c
ags_volume_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_volume_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_volume_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# xorg application context unit test
ags_xorg_application_context_test_SOURCES = ags/test/X/ags_xorg_application_context_test.c
ags_xorg_application_context_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
ags_xorg_application_context_test_LDFLAGS = -pthread $(LDFLAGS)
ags_xorg_application_context_test_LDADD = libgsequencer.la libags_gui.la libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS) $(FONTCONFIG_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS)
