# Copyright (C) 2005-2023 Joel Kraehemann
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
	ags_function_test \
	ags_log_test \
	ags_math_util_test \
	ags_solver_matrix_test \
	ags_solver_vector_test \
	ags_solver_polynomial_test \
	ags_time_test \
	ags_turtle_manager_test \
	ags_turtle_test \
	ags_application_context_test \
	ags_config_test \
	ags_connectable_test \
	ags_soundcard_test \
	ags_concurrency_provider_test \
	ags_destroy_worker_test \
	ags_generic_main_loop_test \
	ags_message_delivery_test \
	ags_message_envelope_test \
	ags_message_queue_test \
	ags_returnable_thread_test \
	ags_task_test \
	ags_task_launcher_test \
	ags_thread_test \
	ags_thread_application_context_test \
	ags_thread_pool_test \
	ags_timestamp_test \
	ags_worker_thread_test \
	ags_file_test \
	ags_file_id_ref_test \
	ags_file_launch_test \
	ags_file_link_test \
	ags_file_lookup_test \
	ags_server_application_context_test \
	ags_server_test \
	ags_controller_test \
	ags_front_controller_test \
	ags_authentication_manager_test \
	ags_business_group_manager_test \
	ags_certificate_manager_test \
	ags_password_store_manager_test \
	ags_xml_authentication_test \
	ags_xml_business_group_test \
	ags_xml_certificate_test \
	ags_xml_password_store_test \
	ags_security_context_test

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
	ags_fifoout_test \
	ags_audio_test \
	ags_audio_ref_count_test \
	ags_fx_ref_count_test \
	ags_audio_application_context_ref_count_test \
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
	ags_envelope_util_test \
	ags_synth_util_test \
	ags_volume_util_test \
	ags_peak_util_test \
	ags_resample_util_test \
	ags_fm_synth_util_test \
	ags_sf2_synth_util_test \
	ags_sfz_synth_util_test \
	ags_fourier_transform_util_test \
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
	ags_midi_smf_util_test \
	ags_midi_builder_test \
	ags_midi_ci_util_test \
	ags_midi_ump_util_test

check_PROGRAMS += \
	ags_alsa_devout_test \
	ags_alsa_devin_test

check_PROGRAMS += \
	ags_osc_buffer_util_test \
	ags_osc_client_test \
	ags_osc_connection_test \
	ags_osc_message_test \
	ags_osc_server_test \
	ags_osc_websocket_connection_test \
	ags_osc_xmlrpc_message_test \
	ags_osc_xmlrpc_server_test \
	ags_osc_action_controller_test \
	ags_osc_config_controller_test \
	ags_osc_front_controller_test \
	ags_osc_renew_controller_test \
	ags_osc_info_controller_test \
	ags_osc_meter_controller_test \
	ags_osc_node_controller_test \
	ags_osc_status_controller_test \
	ags_osc_xmlrpc_controller_test

check_PROGRAMS += \
	ags_fx_analyse_audio_test \
	ags_fx_analyse_audio_processor_test \
	ags_fx_analyse_audio_signal_test \
	ags_fx_analyse_channel_test \
	ags_fx_analyse_channel_processor_test \
	ags_fx_analyse_recycling_test \
	ags_fx_buffer_audio_test \
	ags_fx_buffer_audio_processor_test \
	ags_fx_buffer_audio_signal_test \
	ags_fx_buffer_channel_test \
	ags_fx_buffer_channel_processor_test \
	ags_fx_buffer_recycling_test \
	ags_fx_dssi_audio_test \
	ags_fx_dssi_audio_processor_test \
	ags_fx_dssi_audio_signal_test \
	ags_fx_dssi_channel_test \
	ags_fx_dssi_channel_processor_test \
	ags_fx_dssi_recycling_test \
	ags_fx_envelope_audio_test \
	ags_fx_envelope_audio_processor_test \
	ags_fx_envelope_audio_signal_test \
	ags_fx_envelope_channel_test \
	ags_fx_envelope_channel_processor_test \
	ags_fx_envelope_recycling_test \
	ags_fx_fm_synth_audio_test \
	ags_fx_fm_synth_audio_processor_test \
	ags_fx_fm_synth_audio_signal_test \
	ags_fx_fm_synth_channel_test \
	ags_fx_fm_synth_channel_processor_test \
	ags_fx_fm_synth_recycling_test \
	ags_fx_sf2_synth_audio_test \
	ags_fx_sf2_synth_audio_processor_test \
	ags_fx_sf2_synth_audio_signal_test \
	ags_fx_sf2_synth_channel_test \
	ags_fx_sf2_synth_channel_processor_test \
	ags_fx_sf2_synth_recycling_test \
	ags_fx_sfz_synth_audio_test \
	ags_fx_sfz_synth_audio_processor_test \
	ags_fx_sfz_synth_audio_signal_test \
	ags_fx_sfz_synth_channel_test \
	ags_fx_sfz_synth_channel_processor_test \
	ags_fx_sfz_synth_recycling_test \
	ags_fx_ladspa_audio_test \
	ags_fx_ladspa_audio_processor_test \
	ags_fx_ladspa_audio_signal_test \
	ags_fx_ladspa_channel_test \
	ags_fx_ladspa_channel_processor_test \
	ags_fx_ladspa_recycling_test \
	ags_fx_lv2_audio_test \
	ags_fx_lv2_audio_processor_test \
	ags_fx_lv2_audio_signal_test \
	ags_fx_lv2_channel_test \
	ags_fx_lv2_channel_processor_test \
	ags_fx_lv2_recycling_test \
	ags_fx_notation_audio_test \
	ags_fx_notation_audio_processor_test \
	ags_fx_notation_audio_signal_test \
	ags_fx_notation_channel_test \
	ags_fx_notation_channel_processor_test \
	ags_fx_notation_recycling_test \
	ags_fx_pattern_audio_test \
	ags_fx_pattern_audio_processor_test \
	ags_fx_pattern_audio_signal_test \
	ags_fx_pattern_channel_test \
	ags_fx_pattern_channel_processor_test \
	ags_fx_pattern_recycling_test \
	ags_fx_peak_audio_test \
	ags_fx_peak_audio_processor_test \
	ags_fx_peak_audio_signal_test \
	ags_fx_peak_channel_test \
	ags_fx_peak_channel_processor_test \
	ags_fx_peak_recycling_test \
	ags_fx_playback_audio_test \
	ags_fx_playback_audio_processor_test \
	ags_fx_playback_audio_signal_test \
	ags_fx_playback_channel_test \
	ags_fx_playback_channel_processor_test \
	ags_fx_playback_recycling_test \
	ags_fx_synth_audio_test \
	ags_fx_synth_audio_processor_test \
	ags_fx_synth_audio_signal_test \
	ags_fx_synth_channel_test \
	ags_fx_synth_channel_processor_test \
	ags_fx_synth_recycling_test \
	ags_fx_volume_audio_test \
	ags_fx_volume_audio_processor_test \
	ags_fx_volume_audio_signal_test \
	ags_fx_volume_channel_test \
	ags_fx_volume_channel_processor_test \
	ags_fx_volume_recycling_test \
	ags_fx_eq10_audio_test \
	ags_fx_eq10_audio_processor_test \
	ags_fx_eq10_audio_signal_test \
	ags_fx_eq10_channel_test \
	ags_fx_eq10_channel_processor_test \
	ags_fx_eq10_recycling_test

check_PROGRAMS += \
	ags_add_audio_test \
	ags_add_audio_signal_test \
	ags_add_note_test \
	ags_add_soundcard_test \
	ags_apply_bpm_test \
	ags_apply_presets_test \
	ags_apply_sequencer_length_test \
	ags_apply_synth_test \
	ags_apply_tact_test \
	ags_cancel_audio_test \
	ags_cancel_channel_test \
	ags_clear_audio_signal_test \
	ags_clear_buffer_test \
	ags_crop_note_test \
	ags_export_output_test \
	ags_free_selection_test \
	ags_link_channel_test \
	ags_move_note_test \
	ags_remove_audio_test \
	ags_remove_audio_signal_test \
	ags_remove_note_test \
	ags_remove_soundcard_test \
	ags_resize_audio_test \
	ags_seek_soundcard_test \
	ags_set_audio_channels_test \
	ags_set_buffer_size_test \
	ags_set_device_test \
	ags_set_format_test \
	ags_set_muted_test \
	ags_set_samplerate_test \
	ags_start_audio_test \
	ags_start_channel_test

# unit tests - libgsequencer
check_PROGRAMS += \
	ags_xorg_application_context_test

# buffer util unit test
ags_buffer_util_test_SOURCES = ags/test/lib/ags_buffer_util_test.c
ags_buffer_util_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_buffer_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_buffer_util_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# complex unit test
ags_complex_test_SOURCES = ags/test/lib/ags_complex_test.c
ags_complex_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_complex_test_LDFLAGS = -pthread $(LDFLAGS)
ags_complex_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# conversion unit test
ags_conversion_test_SOURCES = ags/test/lib/ags_conversion_test.c
ags_conversion_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_conversion_test_LDFLAGS = -pthread $(LDFLAGS)
ags_conversion_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# function unit test
ags_function_test_SOURCES = ags/test/lib/ags_function_test.c
ags_function_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_function_test_LDFLAGS = -pthread $(LDFLAGS)
ags_function_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# log unit test
ags_log_test_SOURCES = ags/test/lib/ags_log_test.c
ags_log_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_log_test_LDFLAGS = -pthread $(LDFLAGS)
ags_log_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# math util unit test
ags_math_util_test_SOURCES = ags/test/lib/ags_math_util_test.c
ags_math_util_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_math_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_math_util_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# solver matrix unit test
ags_solver_matrix_test_SOURCES = ags/test/lib/ags_solver_matrix_test.c
ags_solver_matrix_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_solver_matrix_test_LDFLAGS = -pthread $(LDFLAGS)
ags_solver_matrix_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# solver vector unit test
ags_solver_vector_test_SOURCES = ags/test/lib/ags_solver_vector_test.c
ags_solver_vector_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_solver_vector_test_LDFLAGS = -pthread $(LDFLAGS)
ags_solver_vector_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# solver polynomial unit test
ags_solver_polynomial_test_SOURCES = ags/test/lib/ags_solver_polynomial_test.c
ags_solver_polynomial_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_solver_polynomial_test_LDFLAGS = -pthread $(LDFLAGS)
ags_solver_polynomial_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# time unit test
ags_time_test_SOURCES = ags/test/lib/ags_time_test.c
ags_time_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_time_test_LDFLAGS = -pthread $(LDFLAGS)
ags_time_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# turtle_manager unit test
ags_turtle_manager_test_SOURCES = ags/test/lib/ags_turtle_manager_test.c
ags_turtle_manager_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_turtle_manager_test_LDFLAGS = -pthread $(LDFLAGS)
ags_turtle_manager_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# turtle unit test
ags_turtle_test_SOURCES = ags/test/lib/ags_turtle_test.c
ags_turtle_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_turtle_test_LDFLAGS = -pthread $(LDFLAGS)
ags_turtle_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# application context unit test
ags_application_context_test_SOURCES = ags/test/object/ags_application_context_test.c
ags_application_context_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_application_context_test_LDFLAGS = -pthread $(LDFLAGS)
ags_application_context_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# config unit test
ags_config_test_SOURCES = ags/test/object/ags_config_test.c
ags_config_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_config_test_LDFLAGS = -pthread $(LDFLAGS)
ags_config_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# connectable unit test
ags_connectable_test_SOURCES = ags/test/object/ags_connectable_test.c
ags_connectable_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_connectable_test_LDFLAGS = -pthread $(LDFLAGS)
ags_connectable_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# soundcard unit test
ags_soundcard_test_SOURCES = ags/test/object/ags_soundcard_test.c
ags_soundcard_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_soundcard_test_LDFLAGS = -pthread $(LDFLAGS)
ags_soundcard_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# concurrency provider unit test
ags_concurrency_provider_test_SOURCES = ags/test/thread/ags_concurrency_provider_test.c
ags_concurrency_provider_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_concurrency_provider_test_LDFLAGS = -pthread $(LDFLAGS)
ags_concurrency_provider_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# destroy worker unit test
ags_destroy_worker_test_SOURCES = ags/test/thread/ags_destroy_worker_test.c
ags_destroy_worker_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_destroy_worker_test_LDFLAGS = -pthread $(LDFLAGS)
ags_destroy_worker_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# generic main loop unit test
ags_generic_main_loop_test_SOURCES = ags/test/thread/ags_generic_main_loop_test.c
ags_generic_main_loop_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_generic_main_loop_test_LDFLAGS = -pthread $(LDFLAGS)
ags_generic_main_loop_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# message delivery unit test
ags_message_delivery_test_SOURCES = ags/test/thread/ags_message_delivery_test.c
ags_message_delivery_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_message_delivery_test_LDFLAGS = -pthread $(LDFLAGS)
ags_message_delivery_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# message envelope unit test
ags_message_envelope_test_SOURCES = ags/test/thread/ags_message_envelope_test.c
ags_message_envelope_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_message_envelope_test_LDFLAGS = -pthread $(LDFLAGS)
ags_message_envelope_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# message queue unit test
ags_message_queue_test_SOURCES = ags/test/thread/ags_message_queue_test.c
ags_message_queue_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_message_queue_test_LDFLAGS = -pthread $(LDFLAGS)
ags_message_queue_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# returnable thread unit test
ags_returnable_thread_test_SOURCES = ags/test/thread/ags_returnable_thread_test.c
ags_returnable_thread_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_returnable_thread_test_LDFLAGS = -pthread $(LDFLAGS)
ags_returnable_thread_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# task unit test
ags_task_test_SOURCES = ags/test/thread/ags_task_test.c
ags_task_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_task_test_LDFLAGS = -lcunit -lm -pthread -lrt $(LDFLAGS) $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)
ags_task_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# task launcher unit test
ags_task_launcher_test_SOURCES = ags/test/thread/ags_task_launcher_test.c
ags_task_launcher_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_task_launcher_test_LDFLAGS = -lcunit -lm -pthread -lrt $(LDFLAGS) $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)
ags_task_launcher_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# thread unit test
ags_thread_test_SOURCES = ags/test/thread/ags_thread_test.c
ags_thread_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_thread_test_LDFLAGS = -lcunit -lm -pthread -lrt $(LDFLAGS) $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)
ags_thread_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# thread application context unit test
ags_thread_application_context_test_SOURCES = ags/test/thread/ags_thread_application_context_test.c
ags_thread_application_context_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_thread_application_context_test_LDFLAGS = -pthread $(LDFLAGS)
ags_thread_application_context_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# thread pool unit test
ags_thread_pool_test_SOURCES = ags/test/thread/ags_thread_pool_test.c
ags_thread_pool_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_thread_pool_test_LDFLAGS = -pthread $(LDFLAGS)
ags_thread_pool_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# timestamp unit test
ags_timestamp_test_SOURCES = ags/test/thread/ags_timestamp_test.c
ags_timestamp_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_timestamp_test_LDFLAGS = -pthread $(LDFLAGS)
ags_timestamp_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# worker thread unit test
ags_worker_thread_test_SOURCES = ags/test/thread/ags_worker_thread_test.c
ags_worker_thread_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_worker_thread_test_LDFLAGS = -pthread $(LDFLAGS)
ags_worker_thread_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# file unit test
ags_file_test_SOURCES = ags/test/file/ags_file_test.c
ags_file_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_file_test_LDFLAGS = -pthread $(LDFLAGS)
ags_file_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# file id ref unit test
ags_file_id_ref_test_SOURCES = ags/test/file/ags_file_id_ref_test.c
ags_file_id_ref_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_file_id_ref_test_LDFLAGS = -pthread $(LDFLAGS)
ags_file_id_ref_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# file launch unit test
ags_file_launch_test_SOURCES = ags/test/file/ags_file_launch_test.c
ags_file_launch_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_file_launch_test_LDFLAGS = -pthread $(LDFLAGS)
ags_file_launch_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# file link unit test
ags_file_link_test_SOURCES = ags/test/file/ags_file_link_test.c
ags_file_link_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_file_link_test_LDFLAGS = -pthread $(LDFLAGS)
ags_file_link_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# file lookup unit test
ags_file_lookup_test_SOURCES = ags/test/file/ags_file_lookup_test.c
ags_file_lookup_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_file_lookup_test_LDFLAGS = -pthread $(LDFLAGS)
ags_file_lookup_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# server application context unit test
ags_server_application_context_test_SOURCES = ags/test/server/ags_server_application_context_test.c
ags_server_application_context_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_server_application_context_test_LDFLAGS = -pthread $(LDFLAGS)
ags_server_application_context_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# server unit test
ags_server_test_SOURCES = ags/test/server/ags_server_test.c
ags_server_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_server_test_LDFLAGS = -pthread $(LDFLAGS)
ags_server_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# controller unit test
ags_controller_test_SOURCES = ags/test/server/controller/ags_controller_test.c
ags_controller_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_controller_test_LDFLAGS = -pthread $(LDFLAGS)
ags_controller_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# front controller unit test
ags_front_controller_test_SOURCES = ags/test/server/controller/ags_front_controller_test.c
ags_front_controller_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_front_controller_test_LDFLAGS = -pthread $(LDFLAGS)
ags_front_controller_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# authentication manager unit test
ags_authentication_manager_test_SOURCES = ags/test/server/security/ags_authentication_manager_test.c
ags_authentication_manager_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_authentication_manager_test_LDFLAGS = -pthread $(LDFLAGS)
ags_authentication_manager_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# business group manager unit test
ags_business_group_manager_test_SOURCES = ags/test/server/security/ags_business_group_manager_test.c
ags_business_group_manager_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_business_group_manager_test_LDFLAGS = -pthread $(LDFLAGS)
ags_business_group_manager_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# certificate manager unit test
ags_certificate_manager_test_SOURCES = ags/test/server/security/ags_certificate_manager_test.c
ags_certificate_manager_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_certificate_manager_test_LDFLAGS = -pthread $(LDFLAGS)
ags_certificate_manager_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# password store manager unit test
ags_password_store_manager_test_SOURCES = ags/test/server/security/ags_password_store_manager_test.c
ags_password_store_manager_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_password_store_manager_test_LDFLAGS = -pthread $(LDFLAGS)
ags_password_store_manager_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# XML authentication unit test
ags_xml_authentication_test_SOURCES = ags/test/server/security/ags_xml_authentication_test.c
ags_xml_authentication_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_xml_authentication_test_LDFLAGS = -pthread $(LDFLAGS)
ags_xml_authentication_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# XML business group unit test
ags_xml_business_group_test_SOURCES = ags/test/server/security/ags_xml_business_group_test.c
ags_xml_business_group_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_xml_business_group_test_LDFLAGS = -pthread $(LDFLAGS)
ags_xml_business_group_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# XML certificate unit test
ags_xml_certificate_test_SOURCES = ags/test/server/security/ags_xml_certificate_test.c
ags_xml_certificate_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_xml_certificate_test_LDFLAGS = -pthread $(LDFLAGS)
ags_xml_certificate_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# XML password store unit test
ags_xml_password_store_test_SOURCES = ags/test/server/security/ags_xml_password_store_test.c
ags_xml_password_store_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_xml_password_store_test_LDFLAGS = -pthread $(LDFLAGS)
ags_xml_password_store_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# security context unit test
ags_security_context_test_SOURCES = ags/test/server/security/ags_security_context_test.c
ags_security_context_test_CFLAGS = $(CFLAGS) $(LIBXML2_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS)
ags_security_context_test_LDFLAGS = -pthread $(LDFLAGS)
ags_security_context_test_LDADD = libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBXML2_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS)

# base plugin unit test
ags_base_plugin_test_SOURCES = ags/test/plugin/ags_base_plugin_test.c
ags_base_plugin_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_base_plugin_test_LDFLAGS = $(LDFLAGS) -pthread
ags_base_plugin_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# dssi manager unit test
ags_dssi_manager_test_SOURCES = ags/test/plugin/ags_dssi_manager_test.c
ags_dssi_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_dssi_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_dssi_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# dssi plugin unit test
ags_dssi_plugin_test_SOURCES = ags/test/plugin/ags_dssi_plugin_test.c
ags_dssi_plugin_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_dssi_plugin_test_LDFLAGS = $(LDFLAGS) -pthread
ags_dssi_plugin_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# ladspa conversion unit test
ags_ladspa_conversion_test_SOURCES = ags/test/plugin/ags_ladspa_conversion_test.c
ags_ladspa_conversion_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_ladspa_conversion_test_LDFLAGS = $(LDFLAGS) -pthread
ags_ladspa_conversion_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# ladspa manager unit test
ags_ladspa_manager_test_SOURCES = ags/test/plugin/ags_ladspa_manager_test.c
ags_ladspa_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_ladspa_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_ladspa_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# lv2 conversion unit test
ags_lv2_conversion_test_SOURCES = ags/test/plugin/ags_lv2_conversion_test.c
ags_lv2_conversion_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_lv2_conversion_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2_conversion_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# lv2 manager unit test
ags_lv2_manager_test_SOURCES = ags/test/plugin/ags_lv2_manager_test.c
ags_lv2_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_lv2_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# lv2 option manager unit test
ags_lv2_option_manager_test_SOURCES = ags/test/plugin/ags_lv2_option_manager_test.c
ags_lv2_option_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_lv2_option_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2_option_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# lv2 plugin unit test
ags_lv2_plugin_test_SOURCES = ags/test/plugin/ags_lv2_plugin_test.c
ags_lv2_plugin_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_lv2_plugin_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2_plugin_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# lv2 preset unit test
ags_lv2_preset_test_SOURCES = ags/test/plugin/ags_lv2_preset_test.c
ags_lv2_preset_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_lv2_preset_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2_preset_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# lv2 uri map manager unit test
ags_lv2_uri_map_manager_test_SOURCES = ags/test/plugin/ags_lv2_uri_map_manager_test.c
ags_lv2_uri_map_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_lv2_uri_map_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2_uri_map_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# lv2 urid manager unit test
ags_lv2_urid_manager_test_SOURCES = ags/test/plugin/ags_lv2_urid_manager_test.c
ags_lv2_urid_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_lv2_urid_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2_urid_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# lv2 worker manager unit test
ags_lv2_worker_manager_test_SOURCES = ags/test/plugin/ags_lv2_worker_manager_test.c
ags_lv2_worker_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_lv2_worker_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2_worker_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# lv2ui manager unit test
ags_lv2ui_manager_test_SOURCES = ags/test/plugin/ags_lv2ui_manager_test.c
ags_lv2ui_manager_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_lv2ui_manager_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2ui_manager_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# lv2ui plugin unit test
ags_lv2ui_plugin_test_SOURCES = ags/test/plugin/ags_lv2ui_plugin_test.c
ags_lv2ui_plugin_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_lv2ui_plugin_test_LDFLAGS = $(LDFLAGS) -pthread
ags_lv2ui_plugin_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# plugin port unit test
ags_plugin_port_test_SOURCES = ags/test/plugin/ags_plugin_port_test.c
ags_plugin_port_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_plugin_port_test_LDFLAGS = $(LDFLAGS) -pthread
ags_plugin_port_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# audio application context unit test
ags_audio_application_context_test_SOURCES = ags/test/audio/ags_audio_application_context_test.c
ags_audio_application_context_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_audio_application_context_test_LDFLAGS = $(LDFLAGS) -pthread
ags_audio_application_context_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fifoout unit test
ags_fifoout_test_SOURCES = ags/test/audio/ags_fifoout_test.c
ags_fifoout_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fifoout_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fifoout_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# audio unit test
ags_audio_test_SOURCES = ags/test/audio/ags_audio_test.c
ags_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# audio ref count unit test
ags_audio_ref_count_test_SOURCES = ags/test/audio/ags_audio_ref_count_test.c
ags_audio_ref_count_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_audio_ref_count_test_LDFLAGS = -pthread $(LDFLAGS)
ags_audio_ref_count_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx ref count unit test
ags_fx_ref_count_test_SOURCES = ags/test/audio/ags_fx_ref_count_test.c
ags_fx_ref_count_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_ref_count_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_ref_count_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# audio application context ref count unit test
ags_audio_application_context_ref_count_test_SOURCES = ags/test/audio/ags_audio_application_context_ref_count_test.c
ags_audio_application_context_ref_count_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_audio_application_context_ref_count_test_LDFLAGS = -pthread $(LDFLAGS)
ags_audio_application_context_ref_count_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# playback domain unit test
ags_playback_domain_test_SOURCES = ags/test/audio/ags_playback_domain_test.c
ags_playback_domain_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_playback_domain_test_LDFLAGS = -pthread $(LDFLAGS)
ags_playback_domain_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# playback unit test
ags_playback_test_SOURCES = ags/test/audio/ags_playback_test.c
ags_playback_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_playback_test_LDFLAGS = -pthread $(LDFLAGS)
ags_playback_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# preset unit test
ags_preset_test_SOURCES = ags/test/audio/ags_preset_test.c
ags_preset_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_preset_test_LDFLAGS = -pthread $(LDFLAGS)
ags_preset_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# channel unit test
ags_channel_test_SOURCES = ags/test/audio/ags_channel_test.c
ags_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# input unit test
ags_input_test_SOURCES = ags/test/audio/ags_input_test.c
ags_input_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_input_test_LDFLAGS = -pthread $(LDFLAGS)
ags_input_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# output unit test
ags_output_test_SOURCES = ags/test/audio/ags_output_test.c
ags_output_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_output_test_LDFLAGS = -pthread $(LDFLAGS)
ags_output_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# recycling unit test
ags_recycling_test_SOURCES = ags/test/audio/ags_recycling_test.c
ags_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# audio signal unit test
ags_audio_signal_test_SOURCES = ags/test/audio/ags_audio_signal_test.c
ags_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# audio buffer util unit test
ags_audio_buffer_util_test_SOURCES = ags/test/audio/ags_audio_buffer_util_test.c
ags_audio_buffer_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_audio_buffer_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_audio_buffer_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# char buffer util unit test
ags_char_buffer_util_test_SOURCES = ags/test/audio/ags_char_buffer_util_test.c
ags_char_buffer_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_char_buffer_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_char_buffer_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# envelope util unit test
ags_envelope_util_test_SOURCES = ags/test/audio/ags_envelope_util_test.c
ags_envelope_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_envelope_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_envelope_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# synth util unit test
ags_synth_util_test_SOURCES = ags/test/audio/ags_synth_util_test.c
ags_synth_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_synth_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_synth_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# volume util unit test
ags_volume_util_test_SOURCES = ags/test/audio/ags_volume_util_test.c
ags_volume_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_volume_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_volume_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# peak util unit test
ags_peak_util_test_SOURCES = ags/test/audio/ags_peak_util_test.c
ags_peak_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_peak_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_peak_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# resample util unit test
ags_resample_util_test_SOURCES = ags/test/audio/ags_resample_util_test.c
ags_resample_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_resample_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_resample_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# FM synth util unit test
ags_fm_synth_util_test_SOURCES = ags/test/audio/ags_fm_synth_util_test.c
ags_fm_synth_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fm_synth_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fm_synth_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# SF2 synth util unit test
ags_sf2_synth_util_test_SOURCES = ags/test/audio/ags_sf2_synth_util_test.c
ags_sf2_synth_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_sf2_synth_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_sf2_synth_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# SFZ synth util unit test
ags_sfz_synth_util_test_SOURCES = ags/test/audio/ags_sfz_synth_util_test.c
ags_sfz_synth_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_sfz_synth_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_sfz_synth_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fourier transform util unit test
ags_fourier_transform_util_test_SOURCES = ags/test/audio/ags_fourier_transform_util_test.c
ags_fourier_transform_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fourier_transform_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fourier_transform_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# recall unit test
ags_recall_test_SOURCES = ags/test/audio/ags_recall_test.c
ags_recall_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_recall_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recall_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# recall channel unit test
ags_recall_channel_test_SOURCES = ags/test/audio/ags_recall_channel_test.c
ags_recall_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_recall_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recall_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# recall channel run unit test
ags_recall_channel_run_test_SOURCES = ags/test/audio/ags_recall_channel_run_test.c
ags_recall_channel_run_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_recall_channel_run_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recall_channel_run_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# recall container unit test
ags_recall_container_test_SOURCES = ags/test/audio/ags_recall_container_test.c
ags_recall_container_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_recall_container_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recall_container_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# recall dependency unit test
ags_recall_dependency_test_SOURCES = ags/test/audio/ags_recall_dependency_test.c
ags_recall_dependency_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_recall_dependency_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recall_dependency_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# recall id unit test
ags_recall_id_test_SOURCES = ags/test/audio/ags_recall_id_test.c
ags_recall_id_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_recall_id_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recall_id_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# recall recycling unit test
ags_recall_recycling_test_SOURCES = ags/test/audio/ags_recall_recycling_test.c
ags_recall_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_recall_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recall_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# recycling context unit test
ags_recycling_context_test_SOURCES = ags/test/audio/ags_recycling_context_test.c
ags_recycling_context_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_recycling_context_test_LDFLAGS = -pthread $(LDFLAGS)
ags_recycling_context_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# synth generator unit test
ags_synth_generator_test_SOURCES = ags/test/audio/ags_synth_generator_test.c
ags_synth_generator_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_synth_generator_test_LDFLAGS = -pthread $(LDFLAGS)
ags_synth_generator_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# port unit test
ags_port_test_SOURCES = ags/test/audio/ags_port_test.c
ags_port_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_port_test_LDFLAGS = -pthread $(LDFLAGS)
ags_port_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# pattern unit test
ags_pattern_test_SOURCES = ags/test/audio/ags_pattern_test.c
ags_pattern_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_pattern_test_LDFLAGS = -pthread $(LDFLAGS)
ags_pattern_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# notation unit test
ags_notation_test_SOURCES = ags/test/audio/ags_notation_test.c
ags_notation_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_notation_test_LDFLAGS = -pthread $(LDFLAGS)
ags_notation_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# note unit test
ags_note_test_SOURCES = ags/test/audio/ags_note_test.c
ags_note_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_note_test_LDFLAGS = -pthread $(LDFLAGS)
ags_note_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# automation unit test
ags_automation_test_SOURCES = ags/test/audio/ags_automation_test.c
ags_automation_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_automation_test_LDFLAGS = -pthread $(LDFLAGS)
ags_automation_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# acceleration unit test
ags_acceleration_test_SOURCES = ags/test/audio/ags_acceleration_test.c
ags_acceleration_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_acceleration_test_LDFLAGS = -pthread $(LDFLAGS)
ags_acceleration_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# wave unit test
ags_wave_test_SOURCES = ags/test/audio/ags_wave_test.c
ags_wave_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_wave_test_LDFLAGS = -pthread $(LDFLAGS)
ags_wave_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# buffer unit test
ags_buffer_test_SOURCES = ags/test/audio/ags_buffer_test.c
ags_buffer_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_buffer_test_LDFLAGS = -pthread $(LDFLAGS)
ags_buffer_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# midi unit test
ags_midi_test_SOURCES = ags/test/audio/ags_midi_test.c
ags_midi_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_midi_test_LDFLAGS = -pthread $(LDFLAGS)
ags_midi_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# track unit test
ags_track_test_SOURCES = ags/test/audio/ags_track_test.c
ags_track_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_track_test_LDFLAGS = -pthread $(LDFLAGS)
ags_track_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# alsa devout unit test
ags_alsa_devout_test_SOURCES = ags/test/audio/alsa/ags_alsa_devout_test.c
ags_alsa_devout_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_alsa_devout_test_LDFLAGS = -pthread $(LDFLAGS)
ags_alsa_devout_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# alsa devin unit test
ags_alsa_devin_test_SOURCES = ags/test/audio/alsa/ags_alsa_devin_test.c
ags_alsa_devin_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_alsa_devin_test_LDFLAGS = -pthread $(LDFLAGS)
ags_alsa_devin_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# midi smf util unit test
ags_midi_smf_util_test_SOURCES = ags/test/audio/midi/ags_midi_smf_util_test.c
ags_midi_smf_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_midi_smf_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_midi_smf_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# midi builder unit test
ags_midi_builder_test_SOURCES = ags/test/audio/midi/ags_midi_builder_test.c
ags_midi_builder_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_midi_builder_test_LDFLAGS = -pthread $(LDFLAGS)
ags_midi_builder_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# MIDI CI util unit test
ags_midi_ci_util_test_SOURCES = ags/test/audio/midi/ags_midi_ci_util_test.c
ags_midi_ci_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_midi_ci_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_midi_ci_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# MIDI UMP util unit test
ags_midi_ump_util_test_SOURCES = ags/test/audio/midi/ags_midi_ump_util_test.c
ags_midi_ump_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_midi_ump_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_midi_ump_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# osc buffer util unit test
ags_osc_buffer_util_test_SOURCES = ags/test/audio/osc/ags_osc_buffer_util_test.c
ags_osc_buffer_util_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_osc_buffer_util_test_LDFLAGS = -pthread $(LDFLAGS)
ags_osc_buffer_util_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# osc client unit test
ags_osc_client_test_SOURCES = ags/test/audio/osc/ags_osc_client_test.c
ags_osc_client_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_osc_client_test_LDFLAGS = -pthread $(LDFLAGS)
ags_osc_client_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# osc connection unit test
ags_osc_connection_test_SOURCES = ags/test/audio/osc/ags_osc_connection_test.c
ags_osc_connection_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_osc_connection_test_LDFLAGS = -pthread $(LDFLAGS)
ags_osc_connection_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# osc message unit test
ags_osc_message_test_SOURCES = ags/test/audio/osc/ags_osc_message_test.c
ags_osc_message_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_osc_message_test_LDFLAGS = -pthread $(LDFLAGS)
ags_osc_message_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# osc server unit test
ags_osc_server_test_SOURCES = ags/test/audio/osc/ags_osc_server_test.c
ags_osc_server_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_osc_server_test_LDFLAGS = -pthread $(LDFLAGS)
ags_osc_server_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# osc websocket connection unit test
ags_osc_websocket_connection_test_SOURCES = ags/test/audio/osc/ags_osc_websocket_connection_test.c
ags_osc_websocket_connection_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_osc_websocket_connection_test_LDFLAGS = -pthread $(LDFLAGS)
ags_osc_websocket_connection_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# osc XMLRPC message unit test
ags_osc_xmlrpc_message_test_SOURCES = ags/test/audio/osc/ags_osc_xmlrpc_message_test.c
ags_osc_xmlrpc_message_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_osc_xmlrpc_message_test_LDFLAGS = -pthread $(LDFLAGS)
ags_osc_xmlrpc_message_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# osc XMLRPC server unit test
ags_osc_xmlrpc_server_test_SOURCES = ags/test/audio/osc/ags_osc_xmlrpc_server_test.c
ags_osc_xmlrpc_server_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_osc_xmlrpc_server_test_LDFLAGS = -pthread $(LDFLAGS)
ags_osc_xmlrpc_server_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# osc action controller unit test
ags_osc_action_controller_test_SOURCES = ags/test/audio/osc/controller/ags_osc_action_controller_test.c
ags_osc_action_controller_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_osc_action_controller_test_LDFLAGS = -pthread $(LDFLAGS)
ags_osc_action_controller_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# osc config controller unit test
ags_osc_config_controller_test_SOURCES = ags/test/audio/osc/controller/ags_osc_config_controller_test.c
ags_osc_config_controller_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_osc_config_controller_test_LDFLAGS = -pthread $(LDFLAGS)
ags_osc_config_controller_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# osc front controller unit test
ags_osc_front_controller_test_SOURCES = ags/test/audio/osc/controller/ags_osc_front_controller_test.c
ags_osc_front_controller_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_osc_front_controller_test_LDFLAGS = -pthread $(LDFLAGS)
ags_osc_front_controller_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# osc renew controller unit test
ags_osc_renew_controller_test_SOURCES = ags/test/audio/osc/controller/ags_osc_renew_controller_test.c
ags_osc_renew_controller_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_osc_renew_controller_test_LDFLAGS = -pthread $(LDFLAGS)
ags_osc_renew_controller_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# osc info controller unit test
ags_osc_info_controller_test_SOURCES = ags/test/audio/osc/controller/ags_osc_info_controller_test.c
ags_osc_info_controller_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_osc_info_controller_test_LDFLAGS = -pthread $(LDFLAGS)
ags_osc_info_controller_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# osc meter controller unit test
ags_osc_meter_controller_test_SOURCES = ags/test/audio/osc/controller/ags_osc_meter_controller_test.c
ags_osc_meter_controller_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_osc_meter_controller_test_LDFLAGS = -pthread $(LDFLAGS)
ags_osc_meter_controller_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# osc node controller unit test
ags_osc_node_controller_test_SOURCES = ags/test/audio/osc/controller/ags_osc_node_controller_test.c
ags_osc_node_controller_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_osc_node_controller_test_LDFLAGS = -pthread $(LDFLAGS)
ags_osc_node_controller_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# osc status controller unit test
ags_osc_status_controller_test_SOURCES = ags/test/audio/osc/controller/ags_osc_status_controller_test.c
ags_osc_status_controller_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_osc_status_controller_test_LDFLAGS = -pthread $(LDFLAGS)
ags_osc_status_controller_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# osc xmlrpc controller unit test
ags_osc_xmlrpc_controller_test_SOURCES = ags/test/audio/osc/xmlrpc/ags_osc_xmlrpc_controller_test.c
ags_osc_xmlrpc_controller_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_osc_xmlrpc_controller_test_LDFLAGS = -pthread $(LDFLAGS)
ags_osc_xmlrpc_controller_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt  $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx analyse audio unit test
ags_fx_analyse_audio_test_SOURCES = ags/test/audio/fx/ags_fx_analyse_audio_test.c
ags_fx_analyse_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_analyse_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_analyse_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx analyse audio processor unit test
ags_fx_analyse_audio_processor_test_SOURCES = ags/test/audio/fx/ags_fx_analyse_audio_processor_test.c
ags_fx_analyse_audio_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_analyse_audio_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_analyse_audio_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx analyse channel unit test
ags_fx_analyse_channel_test_SOURCES = ags/test/audio/fx/ags_fx_analyse_channel_test.c
ags_fx_analyse_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_analyse_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_analyse_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx analyse channel processor unit test
ags_fx_analyse_channel_processor_test_SOURCES = ags/test/audio/fx/ags_fx_analyse_channel_processor_test.c
ags_fx_analyse_channel_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_analyse_channel_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_analyse_channel_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx analyse recycling unit test
ags_fx_analyse_recycling_test_SOURCES = ags/test/audio/fx/ags_fx_analyse_recycling_test.c
ags_fx_analyse_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_analyse_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_analyse_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx analyse audio signal unit test
ags_fx_analyse_audio_signal_test_SOURCES = ags/test/audio/fx/ags_fx_analyse_audio_signal_test.c
ags_fx_analyse_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_analyse_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_analyse_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx buffer audio unit test
ags_fx_buffer_audio_test_SOURCES = ags/test/audio/fx/ags_fx_buffer_audio_test.c
ags_fx_buffer_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_buffer_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_buffer_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx buffer audio processor unit test
ags_fx_buffer_audio_processor_test_SOURCES = ags/test/audio/fx/ags_fx_buffer_audio_processor_test.c
ags_fx_buffer_audio_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_buffer_audio_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_buffer_audio_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx buffer channel unit test
ags_fx_buffer_channel_test_SOURCES = ags/test/audio/fx/ags_fx_buffer_channel_test.c
ags_fx_buffer_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_buffer_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_buffer_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx buffer channel processor unit test
ags_fx_buffer_channel_processor_test_SOURCES = ags/test/audio/fx/ags_fx_buffer_channel_processor_test.c
ags_fx_buffer_channel_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_buffer_channel_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_buffer_channel_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx buffer recycling unit test
ags_fx_buffer_recycling_test_SOURCES = ags/test/audio/fx/ags_fx_buffer_recycling_test.c
ags_fx_buffer_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_buffer_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_buffer_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx buffer audio signal unit test
ags_fx_buffer_audio_signal_test_SOURCES = ags/test/audio/fx/ags_fx_buffer_audio_signal_test.c
ags_fx_buffer_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_buffer_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_buffer_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx dssi audio unit test
ags_fx_dssi_audio_test_SOURCES = ags/test/audio/fx/ags_fx_dssi_audio_test.c
ags_fx_dssi_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_dssi_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_dssi_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx dssi audio processor unit test
ags_fx_dssi_audio_processor_test_SOURCES = ags/test/audio/fx/ags_fx_dssi_audio_processor_test.c
ags_fx_dssi_audio_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_dssi_audio_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_dssi_audio_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx dssi channel unit test
ags_fx_dssi_channel_test_SOURCES = ags/test/audio/fx/ags_fx_dssi_channel_test.c
ags_fx_dssi_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_dssi_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_dssi_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx dssi channel processor unit test
ags_fx_dssi_channel_processor_test_SOURCES = ags/test/audio/fx/ags_fx_dssi_channel_processor_test.c
ags_fx_dssi_channel_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_dssi_channel_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_dssi_channel_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx dssi recycling unit test
ags_fx_dssi_recycling_test_SOURCES = ags/test/audio/fx/ags_fx_dssi_recycling_test.c
ags_fx_dssi_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_dssi_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_dssi_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx dssi audio signal unit test
ags_fx_dssi_audio_signal_test_SOURCES = ags/test/audio/fx/ags_fx_dssi_audio_signal_test.c
ags_fx_dssi_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_dssi_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_dssi_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx envelope audio unit test
ags_fx_envelope_audio_test_SOURCES = ags/test/audio/fx/ags_fx_envelope_audio_test.c
ags_fx_envelope_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_envelope_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_envelope_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx envelope audio processor unit test
ags_fx_envelope_audio_processor_test_SOURCES = ags/test/audio/fx/ags_fx_envelope_audio_processor_test.c
ags_fx_envelope_audio_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_envelope_audio_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_envelope_audio_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx envelope channel unit test
ags_fx_envelope_channel_test_SOURCES = ags/test/audio/fx/ags_fx_envelope_channel_test.c
ags_fx_envelope_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_envelope_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_envelope_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx envelope channel processor unit test
ags_fx_envelope_channel_processor_test_SOURCES = ags/test/audio/fx/ags_fx_envelope_channel_processor_test.c
ags_fx_envelope_channel_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_envelope_channel_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_envelope_channel_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx envelope recycling unit test
ags_fx_envelope_recycling_test_SOURCES = ags/test/audio/fx/ags_fx_envelope_recycling_test.c
ags_fx_envelope_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_envelope_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_envelope_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx envelope audio signal unit test
ags_fx_envelope_audio_signal_test_SOURCES = ags/test/audio/fx/ags_fx_envelope_audio_signal_test.c
ags_fx_envelope_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_envelope_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_envelope_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx FM synth audio unit test
ags_fx_fm_synth_audio_test_SOURCES = ags/test/audio/fx/ags_fx_fm_synth_audio_test.c
ags_fx_fm_synth_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_fm_synth_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_fm_synth_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx FM synth audio processor unit test
ags_fx_fm_synth_audio_processor_test_SOURCES = ags/test/audio/fx/ags_fx_fm_synth_audio_processor_test.c
ags_fx_fm_synth_audio_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_fm_synth_audio_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_fm_synth_audio_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx FM synth channel unit test
ags_fx_fm_synth_channel_test_SOURCES = ags/test/audio/fx/ags_fx_fm_synth_channel_test.c
ags_fx_fm_synth_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_fm_synth_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_fm_synth_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx FM synth channel processor unit test
ags_fx_fm_synth_channel_processor_test_SOURCES = ags/test/audio/fx/ags_fx_fm_synth_channel_processor_test.c
ags_fx_fm_synth_channel_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_fm_synth_channel_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_fm_synth_channel_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx FM synth recycling unit test
ags_fx_fm_synth_recycling_test_SOURCES = ags/test/audio/fx/ags_fx_fm_synth_recycling_test.c
ags_fx_fm_synth_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_fm_synth_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_fm_synth_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx FM synth audio signal unit test
ags_fx_fm_synth_audio_signal_test_SOURCES = ags/test/audio/fx/ags_fx_fm_synth_audio_signal_test.c
ags_fx_fm_synth_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_fm_synth_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_fm_synth_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx SF2 synth audio unit test
ags_fx_sf2_synth_audio_test_SOURCES = ags/test/audio/fx/ags_fx_sf2_synth_audio_test.c
ags_fx_sf2_synth_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_sf2_synth_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_sf2_synth_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx SF2 synth audio processor unit test
ags_fx_sf2_synth_audio_processor_test_SOURCES = ags/test/audio/fx/ags_fx_sf2_synth_audio_processor_test.c
ags_fx_sf2_synth_audio_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_sf2_synth_audio_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_sf2_synth_audio_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx SF2 synth channel unit test
ags_fx_sf2_synth_channel_test_SOURCES = ags/test/audio/fx/ags_fx_sf2_synth_channel_test.c
ags_fx_sf2_synth_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_sf2_synth_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_sf2_synth_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx SF2 synth channel processor unit test
ags_fx_sf2_synth_channel_processor_test_SOURCES = ags/test/audio/fx/ags_fx_sf2_synth_channel_processor_test.c
ags_fx_sf2_synth_channel_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_sf2_synth_channel_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_sf2_synth_channel_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx SF2 synth recycling unit test
ags_fx_sf2_synth_recycling_test_SOURCES = ags/test/audio/fx/ags_fx_sf2_synth_recycling_test.c
ags_fx_sf2_synth_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_sf2_synth_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_sf2_synth_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx SF2 synth audio signal unit test
ags_fx_sf2_synth_audio_signal_test_SOURCES = ags/test/audio/fx/ags_fx_sf2_synth_audio_signal_test.c
ags_fx_sf2_synth_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_sf2_synth_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_sf2_synth_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx SFZ synth audio unit test
ags_fx_sfz_synth_audio_test_SOURCES = ags/test/audio/fx/ags_fx_sfz_synth_audio_test.c
ags_fx_sfz_synth_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_sfz_synth_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_sfz_synth_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx SFZ synth audio processor unit test
ags_fx_sfz_synth_audio_processor_test_SOURCES = ags/test/audio/fx/ags_fx_sfz_synth_audio_processor_test.c
ags_fx_sfz_synth_audio_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_sfz_synth_audio_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_sfz_synth_audio_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx SFZ synth channel unit test
ags_fx_sfz_synth_channel_test_SOURCES = ags/test/audio/fx/ags_fx_sfz_synth_channel_test.c
ags_fx_sfz_synth_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_sfz_synth_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_sfz_synth_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx SFZ synth channel processor unit test
ags_fx_sfz_synth_channel_processor_test_SOURCES = ags/test/audio/fx/ags_fx_sfz_synth_channel_processor_test.c
ags_fx_sfz_synth_channel_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_sfz_synth_channel_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_sfz_synth_channel_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx SFZ synth recycling unit test
ags_fx_sfz_synth_recycling_test_SOURCES = ags/test/audio/fx/ags_fx_sfz_synth_recycling_test.c
ags_fx_sfz_synth_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_sfz_synth_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_sfz_synth_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx SFZ synth audio signal unit test
ags_fx_sfz_synth_audio_signal_test_SOURCES = ags/test/audio/fx/ags_fx_sfz_synth_audio_signal_test.c
ags_fx_sfz_synth_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_sfz_synth_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_sfz_synth_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx ladspa audio unit test
ags_fx_ladspa_audio_test_SOURCES = ags/test/audio/fx/ags_fx_ladspa_audio_test.c
ags_fx_ladspa_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_ladspa_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_ladspa_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx ladspa audio processor unit test
ags_fx_ladspa_audio_processor_test_SOURCES = ags/test/audio/fx/ags_fx_ladspa_audio_processor_test.c
ags_fx_ladspa_audio_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_ladspa_audio_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_ladspa_audio_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx ladspa channel unit test
ags_fx_ladspa_channel_test_SOURCES = ags/test/audio/fx/ags_fx_ladspa_channel_test.c
ags_fx_ladspa_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_ladspa_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_ladspa_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx ladspa channel processor unit test
ags_fx_ladspa_channel_processor_test_SOURCES = ags/test/audio/fx/ags_fx_ladspa_channel_processor_test.c
ags_fx_ladspa_channel_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_ladspa_channel_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_ladspa_channel_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx ladspa recycling unit test
ags_fx_ladspa_recycling_test_SOURCES = ags/test/audio/fx/ags_fx_ladspa_recycling_test.c
ags_fx_ladspa_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_ladspa_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_ladspa_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx ladspa audio signal unit test
ags_fx_ladspa_audio_signal_test_SOURCES = ags/test/audio/fx/ags_fx_ladspa_audio_signal_test.c
ags_fx_ladspa_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_ladspa_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_ladspa_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx lv2 audio unit test
ags_fx_lv2_audio_test_SOURCES = ags/test/audio/fx/ags_fx_lv2_audio_test.c
ags_fx_lv2_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_lv2_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_lv2_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx lv2 audio processor unit test
ags_fx_lv2_audio_processor_test_SOURCES = ags/test/audio/fx/ags_fx_lv2_audio_processor_test.c
ags_fx_lv2_audio_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_lv2_audio_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_lv2_audio_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx lv2 channel unit test
ags_fx_lv2_channel_test_SOURCES = ags/test/audio/fx/ags_fx_lv2_channel_test.c
ags_fx_lv2_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_lv2_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_lv2_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx lv2 channel processor unit test
ags_fx_lv2_channel_processor_test_SOURCES = ags/test/audio/fx/ags_fx_lv2_channel_processor_test.c
ags_fx_lv2_channel_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_lv2_channel_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_lv2_channel_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx lv2 recycling unit test
ags_fx_lv2_recycling_test_SOURCES = ags/test/audio/fx/ags_fx_lv2_recycling_test.c
ags_fx_lv2_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_lv2_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_lv2_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx lv2 audio signal unit test
ags_fx_lv2_audio_signal_test_SOURCES = ags/test/audio/fx/ags_fx_lv2_audio_signal_test.c
ags_fx_lv2_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_lv2_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_lv2_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx notation audio unit test
ags_fx_notation_audio_test_SOURCES = ags/test/audio/fx/ags_fx_notation_audio_test.c
ags_fx_notation_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_notation_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_notation_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx notation audio processor unit test
ags_fx_notation_audio_processor_test_SOURCES = ags/test/audio/fx/ags_fx_notation_audio_processor_test.c
ags_fx_notation_audio_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_notation_audio_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_notation_audio_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx notation channel unit test
ags_fx_notation_channel_test_SOURCES = ags/test/audio/fx/ags_fx_notation_channel_test.c
ags_fx_notation_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_notation_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_notation_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx notation channel processor unit test
ags_fx_notation_channel_processor_test_SOURCES = ags/test/audio/fx/ags_fx_notation_channel_processor_test.c
ags_fx_notation_channel_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_notation_channel_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_notation_channel_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx notation recycling unit test
ags_fx_notation_recycling_test_SOURCES = ags/test/audio/fx/ags_fx_notation_recycling_test.c
ags_fx_notation_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_notation_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_notation_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx notation audio signal unit test
ags_fx_notation_audio_signal_test_SOURCES = ags/test/audio/fx/ags_fx_notation_audio_signal_test.c
ags_fx_notation_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_notation_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_notation_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx pattern audio unit test
ags_fx_pattern_audio_test_SOURCES = ags/test/audio/fx/ags_fx_pattern_audio_test.c
ags_fx_pattern_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_pattern_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_pattern_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx pattern audio processor unit test
ags_fx_pattern_audio_processor_test_SOURCES = ags/test/audio/fx/ags_fx_pattern_audio_processor_test.c
ags_fx_pattern_audio_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_pattern_audio_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_pattern_audio_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx pattern channel unit test
ags_fx_pattern_channel_test_SOURCES = ags/test/audio/fx/ags_fx_pattern_channel_test.c
ags_fx_pattern_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_pattern_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_pattern_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx pattern channel processor unit test
ags_fx_pattern_channel_processor_test_SOURCES = ags/test/audio/fx/ags_fx_pattern_channel_processor_test.c
ags_fx_pattern_channel_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_pattern_channel_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_pattern_channel_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx pattern recycling unit test
ags_fx_pattern_recycling_test_SOURCES = ags/test/audio/fx/ags_fx_pattern_recycling_test.c
ags_fx_pattern_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_pattern_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_pattern_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx pattern audio signal unit test
ags_fx_pattern_audio_signal_test_SOURCES = ags/test/audio/fx/ags_fx_pattern_audio_signal_test.c
ags_fx_pattern_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_pattern_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_pattern_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx peak audio unit test
ags_fx_peak_audio_test_SOURCES = ags/test/audio/fx/ags_fx_peak_audio_test.c
ags_fx_peak_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_peak_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_peak_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx peak audio processor unit test
ags_fx_peak_audio_processor_test_SOURCES = ags/test/audio/fx/ags_fx_peak_audio_processor_test.c
ags_fx_peak_audio_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_peak_audio_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_peak_audio_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx peak channel unit test
ags_fx_peak_channel_test_SOURCES = ags/test/audio/fx/ags_fx_peak_channel_test.c
ags_fx_peak_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_peak_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_peak_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx peak channel processor unit test
ags_fx_peak_channel_processor_test_SOURCES = ags/test/audio/fx/ags_fx_peak_channel_processor_test.c
ags_fx_peak_channel_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_peak_channel_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_peak_channel_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx peak recycling unit test
ags_fx_peak_recycling_test_SOURCES = ags/test/audio/fx/ags_fx_peak_recycling_test.c
ags_fx_peak_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_peak_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_peak_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx peak audio signal unit test
ags_fx_peak_audio_signal_test_SOURCES = ags/test/audio/fx/ags_fx_peak_audio_signal_test.c
ags_fx_peak_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_peak_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_peak_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx playback audio unit test
ags_fx_playback_audio_test_SOURCES = ags/test/audio/fx/ags_fx_playback_audio_test.c
ags_fx_playback_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_playback_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_playback_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx playback audio processor unit test
ags_fx_playback_audio_processor_test_SOURCES = ags/test/audio/fx/ags_fx_playback_audio_processor_test.c
ags_fx_playback_audio_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_playback_audio_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_playback_audio_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx playback channel unit test
ags_fx_playback_channel_test_SOURCES = ags/test/audio/fx/ags_fx_playback_channel_test.c
ags_fx_playback_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_playback_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_playback_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx playback channel processor unit test
ags_fx_playback_channel_processor_test_SOURCES = ags/test/audio/fx/ags_fx_playback_channel_processor_test.c
ags_fx_playback_channel_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_playback_channel_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_playback_channel_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx playback recycling unit test
ags_fx_playback_recycling_test_SOURCES = ags/test/audio/fx/ags_fx_playback_recycling_test.c
ags_fx_playback_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_playback_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_playback_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx playback audio signal unit test
ags_fx_playback_audio_signal_test_SOURCES = ags/test/audio/fx/ags_fx_playback_audio_signal_test.c
ags_fx_playback_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_playback_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_playback_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx synth audio unit test
ags_fx_synth_audio_test_SOURCES = ags/test/audio/fx/ags_fx_synth_audio_test.c
ags_fx_synth_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_synth_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_synth_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx synth audio processor unit test
ags_fx_synth_audio_processor_test_SOURCES = ags/test/audio/fx/ags_fx_synth_audio_processor_test.c
ags_fx_synth_audio_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_synth_audio_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_synth_audio_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx synth channel unit test
ags_fx_synth_channel_test_SOURCES = ags/test/audio/fx/ags_fx_synth_channel_test.c
ags_fx_synth_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_synth_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_synth_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx synth channel processor unit test
ags_fx_synth_channel_processor_test_SOURCES = ags/test/audio/fx/ags_fx_synth_channel_processor_test.c
ags_fx_synth_channel_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_synth_channel_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_synth_channel_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx synth recycling unit test
ags_fx_synth_recycling_test_SOURCES = ags/test/audio/fx/ags_fx_synth_recycling_test.c
ags_fx_synth_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_synth_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_synth_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx synth audio signal unit test
ags_fx_synth_audio_signal_test_SOURCES = ags/test/audio/fx/ags_fx_synth_audio_signal_test.c
ags_fx_synth_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_synth_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_synth_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx volume audio unit test
ags_fx_volume_audio_test_SOURCES = ags/test/audio/fx/ags_fx_volume_audio_test.c
ags_fx_volume_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_volume_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_volume_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx volume audio processor unit test
ags_fx_volume_audio_processor_test_SOURCES = ags/test/audio/fx/ags_fx_volume_audio_processor_test.c
ags_fx_volume_audio_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_volume_audio_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_volume_audio_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx volume channel unit test
ags_fx_volume_channel_test_SOURCES = ags/test/audio/fx/ags_fx_volume_channel_test.c
ags_fx_volume_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_volume_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_volume_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx volume channel processor unit test
ags_fx_volume_channel_processor_test_SOURCES = ags/test/audio/fx/ags_fx_volume_channel_processor_test.c
ags_fx_volume_channel_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_volume_channel_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_volume_channel_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx volume recycling unit test
ags_fx_volume_recycling_test_SOURCES = ags/test/audio/fx/ags_fx_volume_recycling_test.c
ags_fx_volume_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_volume_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_volume_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx volume audio signal unit test
ags_fx_volume_audio_signal_test_SOURCES = ags/test/audio/fx/ags_fx_volume_audio_signal_test.c
ags_fx_volume_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_volume_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_volume_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx eq10 audio unit test
ags_fx_eq10_audio_test_SOURCES = ags/test/audio/fx/ags_fx_eq10_audio_test.c
ags_fx_eq10_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_eq10_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_eq10_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx eq10 audio processor unit test
ags_fx_eq10_audio_processor_test_SOURCES = ags/test/audio/fx/ags_fx_eq10_audio_processor_test.c
ags_fx_eq10_audio_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_eq10_audio_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_eq10_audio_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx eq10 channel unit test
ags_fx_eq10_channel_test_SOURCES = ags/test/audio/fx/ags_fx_eq10_channel_test.c
ags_fx_eq10_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_eq10_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_eq10_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx eq10 channel processor unit test
ags_fx_eq10_channel_processor_test_SOURCES = ags/test/audio/fx/ags_fx_eq10_channel_processor_test.c
ags_fx_eq10_channel_processor_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_eq10_channel_processor_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_eq10_channel_processor_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx eq10 recycling unit test
ags_fx_eq10_recycling_test_SOURCES = ags/test/audio/fx/ags_fx_eq10_recycling_test.c
ags_fx_eq10_recycling_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_eq10_recycling_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_eq10_recycling_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# fx eq10 audio signal unit test
ags_fx_eq10_audio_signal_test_SOURCES = ags/test/audio/fx/ags_fx_eq10_audio_signal_test.c
ags_fx_eq10_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_fx_eq10_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_fx_eq10_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# add audio unit test
ags_add_audio_test_SOURCES = ags/test/audio/task/ags_add_audio_test.c
ags_add_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_add_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_add_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# add audio signal unit test
ags_add_audio_signal_test_SOURCES = ags/test/audio/task/ags_add_audio_signal_test.c
ags_add_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_add_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_add_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# add note unit test
ags_add_note_test_SOURCES = ags/test/audio/task/ags_add_note_test.c
ags_add_note_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_add_note_test_LDFLAGS = -pthread $(LDFLAGS)
ags_add_note_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# add soundcard unit test
ags_add_soundcard_test_SOURCES = ags/test/audio/task/ags_add_soundcard_test.c
ags_add_soundcard_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_add_soundcard_test_LDFLAGS = -pthread $(LDFLAGS)
ags_add_soundcard_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# apply bpm unit test
ags_apply_bpm_test_SOURCES = ags/test/audio/task/ags_apply_bpm_test.c
ags_apply_bpm_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_apply_bpm_test_LDFLAGS = -pthread $(LDFLAGS)
ags_apply_bpm_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# apply presets unit test
ags_apply_presets_test_SOURCES = ags/test/audio/task/ags_apply_presets_test.c
ags_apply_presets_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_apply_presets_test_LDFLAGS = -pthread $(LDFLAGS)
ags_apply_presets_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# apply sequencer length unit test
ags_apply_sequencer_length_test_SOURCES = ags/test/audio/task/ags_apply_sequencer_length_test.c
ags_apply_sequencer_length_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_apply_sequencer_length_test_LDFLAGS = -pthread $(LDFLAGS)
ags_apply_sequencer_length_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# apply synth unit test
ags_apply_synth_test_SOURCES = ags/test/audio/task/ags_apply_synth_test.c
ags_apply_synth_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_apply_synth_test_LDFLAGS = -pthread $(LDFLAGS)
ags_apply_synth_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# apply tact unit test
ags_apply_tact_test_SOURCES = ags/test/audio/task/ags_apply_tact_test.c
ags_apply_tact_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_apply_tact_test_LDFLAGS = -pthread $(LDFLAGS)
ags_apply_tact_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# cancel audio unit test
ags_cancel_audio_test_SOURCES = ags/test/audio/task/ags_cancel_audio_test.c
ags_cancel_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_cancel_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_cancel_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# cancel channel unit test
ags_cancel_channel_test_SOURCES = ags/test/audio/task/ags_cancel_channel_test.c
ags_cancel_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_cancel_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_cancel_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# clear audio signal unit test
ags_clear_audio_signal_test_SOURCES = ags/test/audio/task/ags_clear_audio_signal_test.c
ags_clear_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_clear_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_clear_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# clear buffer unit test
ags_clear_buffer_test_SOURCES = ags/test/audio/task/ags_clear_buffer_test.c
ags_clear_buffer_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_clear_buffer_test_LDFLAGS = -pthread $(LDFLAGS)
ags_clear_buffer_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# crop note unit test
ags_crop_note_test_SOURCES = ags/test/audio/task/ags_crop_note_test.c
ags_crop_note_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_crop_note_test_LDFLAGS = -pthread $(LDFLAGS)
ags_crop_note_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# export output unit test
ags_export_output_test_SOURCES = ags/test/audio/task/ags_export_output_test.c
ags_export_output_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_export_output_test_LDFLAGS = -pthread $(LDFLAGS)
ags_export_output_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# free selection unit test
ags_free_selection_test_SOURCES = ags/test/audio/task/ags_free_selection_test.c
ags_free_selection_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_free_selection_test_LDFLAGS = -pthread $(LDFLAGS)
ags_free_selection_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# link channel unit test
ags_link_channel_test_SOURCES = ags/test/audio/task/ags_link_channel_test.c
ags_link_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_link_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_link_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# move note unit test
ags_move_note_test_SOURCES = ags/test/audio/task/ags_move_note_test.c
ags_move_note_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_move_note_test_LDFLAGS = -pthread $(LDFLAGS)
ags_move_note_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# remove audio unit test
ags_remove_audio_test_SOURCES = ags/test/audio/task/ags_remove_audio_test.c
ags_remove_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_remove_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_remove_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# remove audio signal unit test
ags_remove_audio_signal_test_SOURCES = ags/test/audio/task/ags_remove_audio_signal_test.c
ags_remove_audio_signal_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_remove_audio_signal_test_LDFLAGS = -pthread $(LDFLAGS)
ags_remove_audio_signal_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# remove note unit test
ags_remove_note_test_SOURCES = ags/test/audio/task/ags_remove_note_test.c
ags_remove_note_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_remove_note_test_LDFLAGS = -pthread $(LDFLAGS)
ags_remove_note_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# remove soundcard unit test
ags_remove_soundcard_test_SOURCES = ags/test/audio/task/ags_remove_soundcard_test.c
ags_remove_soundcard_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_remove_soundcard_test_LDFLAGS = -pthread $(LDFLAGS)
ags_remove_soundcard_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# resize audio unit test
ags_resize_audio_test_SOURCES = ags/test/audio/task/ags_resize_audio_test.c
ags_resize_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_resize_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_resize_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# seek soundcard unit test
ags_seek_soundcard_test_SOURCES = ags/test/audio/task/ags_seek_soundcard_test.c
ags_seek_soundcard_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_seek_soundcard_test_LDFLAGS = -pthread $(LDFLAGS)
ags_seek_soundcard_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# set audio channels unit test
ags_set_audio_channels_test_SOURCES = ags/test/audio/task/ags_set_audio_channels_test.c
ags_set_audio_channels_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_set_audio_channels_test_LDFLAGS = -pthread $(LDFLAGS)
ags_set_audio_channels_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# set buffer size unit test
ags_set_buffer_size_test_SOURCES = ags/test/audio/task/ags_set_buffer_size_test.c
ags_set_buffer_size_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_set_buffer_size_test_LDFLAGS = -pthread $(LDFLAGS)
ags_set_buffer_size_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# set device unit test
ags_set_device_test_SOURCES = ags/test/audio/task/ags_set_device_test.c
ags_set_device_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_set_device_test_LDFLAGS = -pthread $(LDFLAGS)
ags_set_device_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# set format unit test
ags_set_format_test_SOURCES = ags/test/audio/task/ags_set_format_test.c
ags_set_format_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_set_format_test_LDFLAGS = -pthread $(LDFLAGS)
ags_set_format_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# set muted unit test
ags_set_muted_test_SOURCES = ags/test/audio/task/ags_set_muted_test.c
ags_set_muted_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_set_muted_test_LDFLAGS = -pthread $(LDFLAGS)
ags_set_muted_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# set samplerate unit test
ags_set_samplerate_test_SOURCES = ags/test/audio/task/ags_set_samplerate_test.c
ags_set_samplerate_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_set_samplerate_test_LDFLAGS = -pthread $(LDFLAGS)
ags_set_samplerate_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# start audio unit test
ags_start_audio_test_SOURCES = ags/test/audio/task/ags_start_audio_test.c
ags_start_audio_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_start_audio_test_LDFLAGS = -pthread $(LDFLAGS)
ags_start_audio_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# start channel unit test
ags_start_channel_test_SOURCES = ags/test/audio/task/ags_start_channel_test.c
ags_start_channel_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS)
ags_start_channel_test_LDFLAGS = -pthread $(LDFLAGS)
ags_start_channel_test_LDADD = libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lrt -lm $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS)

# xorg application context unit test
ags_xorg_application_context_test_SOURCES = ags/test/app/ags_xorg_application_context_test.c
ags_xorg_application_context_test_CFLAGS = $(CFLAGS) $(LIBAO_CFLAGS) $(LIBASOUND2_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(JSON_GLIB_CFLAGS) $(GOBJECT_CFLAGS) $(LIBSOUP_CFLAGS) $(JACK_CFLAGS) $(GSTREAMER_CFLAGS) $(FONTCONFIG_CFLAGS) $(GDKPIXBUF_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS)
ags_xorg_application_context_test_LDFLAGS = -pthread $(LDFLAGS)
ags_xorg_application_context_test_LDADD = libgsequencer.la libags_gui.la libags_audio.la libags_server.la libags_thread.la libags.la libags_thread.la -lcunit -lm -lrt $(LIBAO_LIBS) $(LIBASOUND2_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(JSON_GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSOUP_LIBS) $(JACK_LIBS) $(GSTREAMER_LIBS) $(FONTCONFIG_LIBS) $(GDKPIXBUF_LIBS) $(CAIRO_LIBS) $(GTK_LIBS)
