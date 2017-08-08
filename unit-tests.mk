# unit tests
check_PROGRAMS += \
	ags_application_context_test \
	ags_config_test \
	ags_connection_manager_test \
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
	ags_file_lookup_test \
	ags_complex_test \
	ags_log_test \
	ags_turtle_test \
	ags_turtle_manager_test \
	ags_base_plugin_test \
	ags_dssi_manager_test \
	ags_dssi_plugin_test \
	ags_ladspa_manager_test \
	ags_lv2_manager_test \
	ags_lv2_option_manager_test \
	ags_audio_application_context_test \
	ags_devout_test \
	ags_audio_test \
	ags_channel_test \
	ags_recycling_test \
	ags_audio_signal_test \
	ags_recall_test \
	ags_port_test \
	ags_pattern_test \
	ags_notation_test \
	ags_note_test \
	ags_automation_test \
	ags_acceleration_test \
	ags_midi_buffer_util_test \
	ags_midi_builder_test \
	ags_xorg_application_context_test

# application context unit test
ags_application_context_test_SOURCES = ags/test/object/ags_application_context_test.c
ags_application_context_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_application_context_test_LDFLAGS = -pthread $(LDFLAGS)
ags_application_context_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# config unit test
ags_config_test_SOURCES = ags/test/object/ags_config_test.c
ags_config_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_config_test_LDFLAGS = -pthread $(LDFLAGS)
ags_config_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# connection manager unit test
ags_connection_manager_test_SOURCES = ags/test/object/ags_connection_manager_test.c
ags_connection_manager_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_connection_manager_test_LDFLAGS = -pthread $(LDFLAGS)
ags_connection_manager_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# condition manager unit test
ags_condition_manager_test_SOURCES = ags/test/thread/ags_condition_manager_test.c
ags_condition_manager_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_condition_manager_test_LDFLAGS = -pthread $(LDFLAGS)
ags_condition_manager_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# destroy worker unit test
ags_destroy_worker_test_SOURCES = ags/test/thread/ags_destroy_worker_test.c
ags_destroy_worker_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_destroy_worker_test_LDFLAGS = -pthread $(LDFLAGS)
ags_destroy_worker_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# mutex manager unit test
ags_mutex_manager_test_SOURCES = ags/test/thread/ags_mutex_manager_test.c
ags_mutex_manager_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_mutex_manager_test_LDFLAGS = -pthread $(LDFLAGS)
ags_mutex_manager_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# poll fd unit test
ags_poll_fd_test_SOURCES = ags/test/thread/ags_poll_fd_test.c
ags_poll_fd_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_poll_fd_test_LDFLAGS = -pthread $(LDFLAGS)
ags_poll_fd_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# polling thread unit test
ags_polling_thread_test_SOURCES = ags/test/thread/ags_polling_thread_test.c
ags_polling_thread_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_polling_thread_test_LDFLAGS = -pthread $(LDFLAGS)
ags_polling_thread_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# returnable thread unit test
ags_returnable_thread_test_SOURCES = ags/test/thread/ags_returnable_thread_test.c
ags_returnable_thread_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_returnable_thread_test_LDFLAGS = -pthread $(LDFLAGS)
ags_returnable_thread_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# task unit test
ags_task_test_SOURCES = ags/test/thread/ags_task_test.c
ags_task_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_task_test_LDFLAGS = -lcunit -lm -pthread -lrt $(LDFLAGS) $(LIBXML2_LIBS) $(GOBJECT_LIBS)
ags_task_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# task thread unit test
ags_task_thread_test_SOURCES = ags/test/thread/ags_task_thread_test.c
ags_task_thread_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_task_thread_test_LDFLAGS = -pthread $(LDFLAGS)
ags_task_thread_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# thread unit test
ags_thread_test_SOURCES = ags/test/thread/ags_thread_test.c
ags_thread_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_thread_test_LDFLAGS = -lcunit -lm -pthread -lrt $(LDFLAGS) $(LIBXML2_LIBS) $(GOBJECT_LIBS)
ags_thread_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# thread pool unit test
ags_thread_pool_test_SOURCES = ags/test/thread/ags_thread_pool_test.c
ags_thread_pool_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_thread_pool_test_LDFLAGS = -pthread $(LDFLAGS)
ags_thread_pool_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# worker thread unit test
ags_worker_thread_test_SOURCES = ags/test/thread/ags_worker_thread_test.c
ags_worker_thread_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_worker_thread_test_LDFLAGS = -pthread $(LDFLAGS)
ags_worker_thread_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# thread file xml unit test
ags_thread_file_xml_test_SOURCES = ags/test/thread/file/ags_thread_file_xml_test.c
ags_thread_file_xml_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_thread_file_xml_test_LDFLAGS = -pthread $(LDFLAGS)
ags_thread_file_xml_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# file unit test
ags_file_test_SOURCES = ags/test/file/ags_file_test.c
ags_file_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_file_test_LDFLAGS = -pthread $(LDFLAGS)
ags_file_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# file id ref unit test
ags_file_id_ref_test_SOURCES = ags/test/file/ags_file_id_ref_test.c
ags_file_id_ref_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_file_id_ref_test_LDFLAGS = -pthread $(LDFLAGS)
ags_file_id_ref_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# file launch unit test
ags_file_launch_test_SOURCES = ags/test/file/ags_file_launch_test.c
ags_file_launch_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_file_launch_test_LDFLAGS = -pthread $(LDFLAGS)
ags_file_launch_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# file lookup unit test
ags_file_lookup_test_SOURCES = ags/test/file/ags_file_lookup_test.c
ags_file_lookup_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_file_lookup_test_LDFLAGS = -pthread $(LDFLAGS)
ags_file_lookup_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# complex unit test
ags_complex_test_SOURCES = ags/test/lib/ags_complex_test.c
ags_complex_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_complex_test_LDFLAGS = -pthread $(LDFLAGS)
ags_complex_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# log unit test
ags_log_test_SOURCES = ags/test/lib/ags_log_test.c
ags_log_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_log_test_LDFLAGS = -pthread $(LDFLAGS)
ags_log_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# turtle unit test
ags_turtle_test_SOURCES = ags/test/lib/ags_turtle_test.c
ags_turtle_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_turtle_test_LDFLAGS = -pthread $(LDFLAGS)
ags_turtle_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# turtle manager unit test
ags_turtle_manager_test_SOURCES = ags/test/lib/ags_turtle_manager_test.c
ags_turtle_manager_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS)
ags_turtle_manager_test_LDFLAGS = -pthread $(LDFLAGS)
ags_turtle_manager_test_LDADD = libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS)

# base plugin unit test
ags_base_plugin_test_SOURCES = ags/test/plugin/ags_base_plugin_test.c
ags_base_plugin_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_base_plugin_test_LDFLAGS = $(LDFLAGS) -pthread
ags_base_plugin_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# dssi manager unit test
ags_dssi_manager_test_SOURCES = ags/test/plugin/ags_dssi_manager_test.c
ags_dssi_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_dssi_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_dssi_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# dssi plugin unit test
ags_dssi_plugin_test_SOURCES = ags/test/plugin/ags_dssi_plugin_test.c
ags_dssi_plugin_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_dssi_plugin_test_LDFLAGS = $(LDFLAGS) -pthread
ags_dssi_plugin_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# ladspa manager unit test
ags_ladspa_manager_test_SOURCES = ags/test/plugin/ags_ladspa_manager_test.c
ags_ladspa_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_ladspa_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_ladspa_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# lv2 manager unit test
ags_lv2_manager_test_SOURCES = ags/test/plugin/ags_lv2_manager_test.c
ags_lv2_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_lv2_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# lv2 option manager unit test
ags_lv2_option_manager_test_SOURCES = ags/test/plugin/ags_lv2_option_manager_test.c
ags_lv2_option_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_lv2_option_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2_option_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# audio application context unit test
ags_audio_application_context_test_SOURCES = ags/test/audio/ags_audio_application_context_test.c
ags_audio_application_context_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_audio_application_context_test_LDFLAGS = $(LDFLAGS) -pthread
ags_audio_application_context_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# devout unit test
ags_devout_test_SOURCES = ags/test/audio/ags_devout_test.c
ags_devout_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_devout_test_LDFLAGS = -pthread $(LDFLAGS)
ags_devout_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# audio unit test
ags_audio_test_SOURCES = ags/test/audio/ags_audio_test.c
ags_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# channel unit test
ags_channel_test_SOURCES = ags/test/audio/ags_channel_test.c
ags_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# recycling unit test
ags_recycling_test_SOURCES = ags/test/audio/ags_recycling_test.c
ags_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# audio signal unit test
ags_audio_signal_test_SOURCES = ags/test/audio/ags_audio_signal_test.c
ags_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# recall unit test
ags_recall_test_SOURCES = ags/test/audio/ags_recall_test.c
ags_recall_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_recall_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recall_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# port unit test
ags_port_test_SOURCES = ags/test/audio/ags_port_test.c
ags_port_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_port_test_LDFLAGS = -pthread $(LDFLAGS)
ags_port_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# pattern unit test
ags_pattern_test_SOURCES = ags/test/audio/ags_pattern_test.c
ags_pattern_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_pattern_test_LDFLAGS = -pthread $(LDFLAGS)
ags_pattern_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lm -lrt $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# notation unit test
ags_notation_test_SOURCES = ags/test/audio/ags_notation_test.c
ags_notation_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_notation_test_LDFLAGS = -pthread $(LDFLAGS)
ags_notation_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lm -lrt $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# note unit test
ags_note_test_SOURCES = ags/test/audio/ags_note_test.c
ags_note_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_note_test_LDFLAGS = -pthread $(LDFLAGS)
ags_note_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# automation unit test
ags_automation_test_SOURCES = ags/test/audio/ags_automation_test.c
ags_automation_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_automation_test_LDFLAGS = -pthread $(LDFLAGS)
ags_automation_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lm -lrt $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# acceleration unit test
ags_acceleration_test_SOURCES = ags/test/audio/ags_acceleration_test.c
ags_acceleration_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_acceleration_test_LDFLAGS = -pthread $(LDFLAGS)
ags_acceleration_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# midi buffer util unit test
ags_midi_buffer_util_test_SOURCES = ags/test/audio/midi/ags_midi_buffer_util_test.c
ags_midi_buffer_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_midi_buffer_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_midi_buffer_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# midi builder unit test
ags_midi_builder_test_SOURCES = ags/test/audio/midi/ags_midi_builder_test.c
ags_midi_builder_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS)
ags_midi_builder_test_LDFLAGS = -pthread $(LDFLAGS)
ags_midi_builder_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS)

# xorg application context unit test
ags_xorg_application_context_test_SOURCES = ags/test/X/ags_xorg_application_context_test.c
ags_xorg_application_context_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(JACK_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
ags_xorg_application_context_test_LDFLAGS = -pthread $(LDFLAGS)
ags_xorg_application_context_test_LDADD = libgsequencer.la libags_audio.la libags_server.la libags_gui.la libags_thread.la libags.la -lcunit -lm -lrt $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(JACK_LIBS) $(FONTCONFIG_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS)
