/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2023 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <stdlib.h>

int ags_midi_ump_util_test_init_suite();
int ags_midi_ump_util_test_clean_suite();

void ags_midi_ump_util_test_alloc();
void ags_midi_ump_util_test_free();
void ags_midi_ump_util_test_copy();
void ags_midi_ump_util_test_is_system_common();
void ags_midi_ump_util_test_is_data_message_with_sysex();
void ags_midi_ump_util_test_is_data_message();
void ags_midi_ump_util_test_is_stream_message();
void ags_midi_ump_util_test_is_endpoint_discovery();
void ags_midi_ump_util_test_put_endpoint_discovery();
void ags_midi_ump_util_test_get_endpoint_discovery();
void ags_midi_ump_util_test_is_endpoint_info_notification();
void ags_midi_ump_util_test_put_endpoint_info_notification();
void ags_midi_ump_util_test_get_endpoint_info_notification();
void ags_midi_ump_util_test_is_device_identity_notification();
void ags_midi_ump_util_test_put_device_identity_notification();
void ags_midi_ump_util_test_get_device_identity_notification();
void ags_midi_ump_util_test_is_endpoint_name_notification();
void ags_midi_ump_util_test_put_endpoint_name_notification();
void ags_midi_ump_util_test_get_endpoint_name_notification();
void ags_midi_ump_util_test_is_product_instance_id_notification();
void ags_midi_ump_util_test_put_product_instance_id_notification();
void ags_midi_ump_util_test_get_product_instance_id_notification();
void ags_midi_ump_util_test_is_stream_configuration_request();
void ags_midi_ump_util_test_put_stream_configuration_request();
void ags_midi_ump_util_test_get_stream_configuration_request();
void ags_midi_ump_util_test_is_stream_configuration_notification();
void ags_midi_ump_util_test_put_stream_configuration_notification();
void ags_midi_ump_util_test_get_stream_configuration_notification();
void ags_midi_ump_util_test_is_function_block_discovery();
void ags_midi_ump_util_test_put_function_block_discovery();
void ags_midi_ump_util_test_get_function_block_discovery();
void ags_midi_ump_util_test_is_function_block_info_notification();
void ags_midi_ump_util_test_put_function_block_info_notification();
void ags_midi_ump_util_test_get_function_block_info_notification();
void ags_midi_ump_util_test_is_function_block_name_notification();
void ags_midi_ump_util_test_put_function_block_name_notification();
void ags_midi_ump_util_test_get_function_block_name_notification();
void ags_midi_ump_util_test_is_start_of_clip();
void ags_midi_ump_util_test_put_start_of_clip();
void ags_midi_ump_util_test_get_start_of_clip();
void ags_midi_ump_util_test_is_end_of_clip();
void ags_midi_ump_util_test_put_end_of_clip();
void ags_midi_ump_util_test_get_end_of_clip();
void ags_midi_ump_util_test_is_noop();
void ags_midi_ump_util_test_put_noop();
void ags_midi_ump_util_test_get_noop();
void ags_midi_ump_util_test_is_jr_clock();
void ags_midi_ump_util_test_put_jr_clock();
void ags_midi_ump_util_test_get_jr_clock();
void ags_midi_ump_util_test_is_jr_timestamp();
void ags_midi_ump_util_test_put_jr_timestamp();
void ags_midi_ump_util_test_get_jr_timestamp();
void ags_midi_ump_util_test_is_delta_clock_ticks_per_quarter_note();
void ags_midi_ump_util_test_put_delta_clock_ticks_per_quarter_note();
void ags_midi_ump_util_test_get_delta_clock_ticks_per_quarter_note();
void ags_midi_ump_util_test_is_delta_clock_ticks_since_last_event();
void ags_midi_ump_util_test_put_delta_clock_ticks_since_last_event();
void ags_midi_ump_util_test_get_delta_clock_ticks_since_last_event();
void ags_midi_ump_util_test_is_midi1_channel_voice();
void ags_midi_ump_util_test_is_midi1_note_off();
void ags_midi_ump_util_test_put_midi1_note_off();
void ags_midi_ump_util_test_get_midi1_note_off();
void ags_midi_ump_util_test_is_midi1_note_on();
void ags_midi_ump_util_test_put_midi1_note_on();
void ags_midi_ump_util_test_get_midi1_note_on();
void ags_midi_ump_util_test_is_midi1_polyphonic_aftertouch();
void ags_midi_ump_util_test_put_midi1_polyphonic_aftertouch();
void ags_midi_ump_util_test_get_midi1_polyphonic_aftertouch();
void ags_midi_ump_util_test_is_midi1_control_change();
void ags_midi_ump_util_test_put_midi1_control_change();
void ags_midi_ump_util_test_get_midi1_control_change();
void ags_midi_ump_util_test_is_midi1_program_change();
void ags_midi_ump_util_test_put_midi1_program_change();
void ags_midi_ump_util_test_get_midi1_program_change();
void ags_midi_ump_util_test_is_midi1_channel_pressure();
void ags_midi_ump_util_test_put_midi1_channel_pressure();
void ags_midi_ump_util_test_get_midi1_channel_pressure();
void ags_midi_ump_util_test_is_midi1_pitch_bend();
void ags_midi_ump_util_test_put_midi1_pitch_bend();
void ags_midi_ump_util_test_get_midi1_pitch_bend();
void ags_midi_ump_util_test_is_midi2_channel_voice();
void ags_midi_ump_util_test_is_midi2_note_off();
void ags_midi_ump_util_test_put_midi2_note_off();
void ags_midi_ump_util_test_get_midi2_note_off();
void ags_midi_ump_util_test_is_midi2_note_on();
void ags_midi_ump_util_test_put_midi2_note_on();
void ags_midi_ump_util_test_get_midi2_note_on();
void ags_midi_ump_util_test_is_midi2_polyphonic_aftertouch();
void ags_midi_ump_util_test_put_midi2_polyphonic_aftertouch();
void ags_midi_ump_util_test_get_midi2_polyphonic_aftertouch();
void ags_midi_ump_util_test_is_midi2_registered_per_note_controller();
void ags_midi_ump_util_test_put_midi2_registered_per_note_controller();
void ags_midi_ump_util_test_get_midi2_registered_per_note_controller();
void ags_midi_ump_util_test_is_midi2_assignable_per_note_controller();
void ags_midi_ump_util_test_put_midi2_assignable_per_note_controller();
void ags_midi_ump_util_test_get_midi2_assignable_per_note_controller();
void ags_midi_ump_util_test_is_midi2_per_note_management();
void ags_midi_ump_util_test_put_midi2_per_note_management();
void ags_midi_ump_util_test_get_midi2_per_note_management();
void ags_midi_ump_util_test_is_midi2_control_change();
void ags_midi_ump_util_test_put_midi2_control_change();
void ags_midi_ump_util_test_get_midi2_control_change();
void ags_midi_ump_util_test_is_midi2_rpn_pitch_bend_range();
void ags_midi_ump_util_test_put_midi2_rpn_pitch_bend_range();
void ags_midi_ump_util_test_get_midi2_rpn_pitch_bend_range();
void ags_midi_ump_util_test_is_midi2_rpn_coarse_tuning();
void ags_midi_ump_util_test_put_midi2_rpn_coarse_tuning();
void ags_midi_ump_util_test_get_midi2_rpn_coarse_tuning();
void ags_midi_ump_util_test_is_midi2_rpn_tuning_program_change();
void ags_midi_ump_util_test_put_midi2_rpn_tuning_program_change();
void ags_midi_ump_util_test_get_midi2_rpn_tuning_program_change();
void ags_midi_ump_util_test_is_midi2_rpn_tuning_bank_select();
void ags_midi_ump_util_test_put_midi2_rpn_tuning_bank_select();
void ags_midi_ump_util_test_get_midi2_rpn_tuning_bank_select();
void ags_midi_ump_util_test_is_midi2_rpn_mpe_mcm();
void ags_midi_ump_util_test_put_midi2_rpn_mpe_mcm();
void ags_midi_ump_util_test_get_midi2_rpn_mpe_mcm();
void ags_midi_ump_util_test_is_midi2_program_change();
void ags_midi_ump_util_test_put_midi2_program_change();
void ags_midi_ump_util_test_get_midi2_program_change();
void ags_midi_ump_util_test_is_midi2_channel_pressure();
void ags_midi_ump_util_test_put_midi2_channel_pressure();
void ags_midi_ump_util_test_get_midi2_channel_pressure();
void ags_midi_ump_util_test_is_midi2_pitch_bend();
void ags_midi_ump_util_test_put_midi2_pitch_bend();
void ags_midi_ump_util_test_get_midi2_pitch_bend();
void ags_midi_ump_util_test_is_midi2_per_note_pitch_bend();
void ags_midi_ump_util_test_put_midi2_per_note_pitch_bend();
void ags_midi_ump_util_test_get_midi2_per_note_pitch_bend();
void ags_midi_ump_util_test_is_flex_set_tempo();
void ags_midi_ump_util_test_put_flex_set_tempo();
void ags_midi_ump_util_test_get_flex_set_tempo();
void ags_midi_ump_util_test_is_flex_set_time_signature();
void ags_midi_ump_util_test_put_flex_set_time_signature();
void ags_midi_ump_util_test_get_flex_set_time_signature();
void ags_midi_ump_util_test_is_flex_set_metronome();
void ags_midi_ump_util_test_put_flex_set_metronome();
void ags_midi_ump_util_test_get_flex_set_metronome();
void ags_midi_ump_util_test_is_flex_set_key_signature();
void ags_midi_ump_util_test_put_flex_set_key_signature();
void ags_midi_ump_util_test_get_flex_set_key_signature();
void ags_midi_ump_util_test_is_flex_set_chord_name();
void ags_midi_ump_util_test_put_flex_set_chord_name();
void ags_midi_ump_util_test_get_flex_set_chord_name();
void ags_midi_ump_util_test_is_flex_set_text();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_midi_ump_util_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_midi_ump_util_test_clean_suite()
{
  return(0);
}


void
ags_midi_ump_util_test_alloc()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  CU_ASSERT(midi_ump_util != NULL);
}

void
ags_midi_ump_util_test_free()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_free(midi_ump_util);
}

void
ags_midi_ump_util_test_copy()
{
  AgsMidiUmpUtil *a, *b;

  a = ags_midi_ump_util_alloc();

  b = ags_midi_ump_util_copy(a);
  
  CU_ASSERT(b != NULL);
}

void
ags_midi_ump_util_test_is_system_common()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_data_message_with_sysex()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_data_message()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_stream_message()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_endpoint_discovery()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\xf0\x00\x01\x01\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_endpoint_discovery(midi_ump_util,
						   buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_endpoint_discovery()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\xf0\x00\x01\x01\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00";
  
  gint major = 1;
  gint minor = 1;
  gint filter = 0x04;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_endpoint_discovery(midi_ump_util,
					   buffer,
					   major,
					   minor,
					   filter,
					   NULL, NULL,
					   0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_endpoint_discovery()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\xf0\x00\x01\x01\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00";

  gint major;
  gint minor;
  gint filter;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_endpoint_discovery(midi_ump_util,
					   buffer,
					   &major,
					   &minor,
					   &filter,
					   NULL, NULL,
					   0);

  CU_ASSERT(major == 1);
  CU_ASSERT(minor == 1);
  CU_ASSERT(filter == 0x04);
}

void
ags_midi_ump_util_test_is_endpoint_info_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\xf0\x01\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_endpoint_info_notification(midi_ump_util,
							   buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_endpoint_info_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\xf0\x01\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  
  gint major = 1;
  gint minor = 1;
  gboolean static_function_blocks = FALSE;
  gint function_block_count = 0;
  gboolean midi_v2_0_support = FALSE;
  gboolean midi_v1_0_support = FALSE;
  gboolean rx_jitter_reduction = FALSE;
  gboolean tx_jitter_reduction = FALSE;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_endpoint_info_notification(midi_ump_util,
						   buffer,
						   major,
						   minor,
						   static_function_blocks,
						   function_block_count,
						   midi_v2_0_support,
						   midi_v1_0_support,
						   rx_jitter_reduction,
						   tx_jitter_reduction,
						   NULL, NULL,
						   0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_endpoint_info_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\xf0\x01\x01\x01\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00";

  gint major;
  gint minor;
  gboolean static_function_blocks;
  gint function_block_count;
  gboolean midi_v2_0_support;
  gboolean midi_v1_0_support;
  gboolean rx_jitter_reduction;
  gboolean tx_jitter_reduction;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_endpoint_info_notification(midi_ump_util,
						   buffer,
						   &major,
						   &minor,
						   &static_function_blocks,
						   &function_block_count,
						   &midi_v2_0_support,
						   &midi_v1_0_support,
						   &rx_jitter_reduction,
						   &tx_jitter_reduction,
						   NULL, NULL,
						   0);

  CU_ASSERT(major == 1);
  CU_ASSERT(minor == 1);
  CU_ASSERT(static_function_blocks == FALSE);
  CU_ASSERT(function_block_count == 0);
  CU_ASSERT(midi_v2_0_support == FALSE);
  CU_ASSERT(midi_v1_0_support == FALSE);
  CU_ASSERT(rx_jitter_reduction == FALSE);
  CU_ASSERT(tx_jitter_reduction == FALSE);
}

void
ags_midi_ump_util_test_is_device_identity_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_device_identity_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_device_identity_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_endpoint_name_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_endpoint_name_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_endpoint_name_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_product_instance_id_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_product_instance_id_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_product_instance_id_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_stream_configuration_request()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_stream_configuration_request()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_stream_configuration_request()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_stream_configuration_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_stream_configuration_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_stream_configuration_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_function_block_discovery()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_function_block_discovery()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_function_block_discovery()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_function_block_info_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_function_block_info_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_function_block_info_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_function_block_name_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_function_block_name_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_function_block_name_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_start_of_clip()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_start_of_clip()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_start_of_clip()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_end_of_clip()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_end_of_clip()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_end_of_clip()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_noop()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_noop()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_noop()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_jr_clock()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_jr_clock()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_jr_clock()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_jr_timestamp()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_jr_timestamp()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_jr_timestamp()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_delta_clock_ticks_per_quarter_note()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_delta_clock_ticks_per_quarter_note()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_delta_clock_ticks_per_quarter_note()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_delta_clock_ticks_since_last_event()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_delta_clock_ticks_since_last_event()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_delta_clock_ticks_since_last_event()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi1_channel_voice()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi1_note_off()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi1_note_off()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi1_note_off()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi1_note_on()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi1_note_on()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi1_note_on()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi1_polyphonic_aftertouch()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi1_polyphonic_aftertouch()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi1_polyphonic_aftertouch()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi1_control_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi1_control_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi1_control_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi1_program_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi1_program_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi1_program_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi1_channel_pressure()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi1_channel_pressure()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi1_channel_pressure()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi1_pitch_bend()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi1_pitch_bend()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi1_pitch_bend()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi2_channel_voice()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi2_note_off()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi2_note_off()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi2_note_off()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi2_note_on()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi2_note_on()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi2_note_on()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi2_polyphonic_aftertouch()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi2_polyphonic_aftertouch()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi2_polyphonic_aftertouch()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi2_registered_per_note_controller()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi2_registered_per_note_controller()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi2_registered_per_note_controller()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi2_assignable_per_note_controller()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi2_assignable_per_note_controller()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi2_assignable_per_note_controller()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi2_per_note_management()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi2_per_note_management()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi2_per_note_management()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi2_control_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi2_control_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi2_control_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi2_rpn_pitch_bend_range()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi2_rpn_pitch_bend_range()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi2_rpn_pitch_bend_range()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi2_rpn_coarse_tuning()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi2_rpn_coarse_tuning()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi2_rpn_coarse_tuning()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi2_rpn_tuning_program_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi2_rpn_tuning_program_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi2_rpn_tuning_program_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi2_rpn_tuning_bank_select()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi2_rpn_tuning_bank_select()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi2_rpn_tuning_bank_select()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi2_rpn_mpe_mcm()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi2_rpn_mpe_mcm()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi2_rpn_mpe_mcm()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi2_program_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi2_program_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi2_program_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi2_channel_pressure()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi2_channel_pressure()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi2_channel_pressure()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi2_pitch_bend()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi2_pitch_bend()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi2_pitch_bend()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_midi2_per_note_pitch_bend()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_midi2_per_note_pitch_bend()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_midi2_per_note_pitch_bend()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_flex_set_tempo()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_flex_set_tempo()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_flex_set_tempo()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_flex_set_time_signature()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_flex_set_time_signature()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_flex_set_time_signature()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_flex_set_metronome()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_flex_set_metronome()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_flex_set_metronome()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_flex_set_key_signature()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_flex_set_key_signature()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_flex_set_key_signature()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_flex_set_chord_name()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_put_flex_set_chord_name()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_get_flex_set_chord_name()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

void
ags_midi_ump_util_test_is_flex_set_text()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = ags_midi_ump_util_alloc();

  //TODO:JK: implement me
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsMidiUmpUtilTest", ags_midi_ump_util_test_init_suite, ags_midi_ump_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_midi_ump_util.c alloc", ags_midi_ump_util_test_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c free", ags_midi_ump_util_test_free) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c copy", ags_midi_ump_util_test_copy) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is system common", ags_midi_ump_util_test_is_system_common) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is data message with sysex", ags_midi_ump_util_test_is_data_message_with_sysex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is data message", ags_midi_ump_util_test_is_data_message) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is stream message", ags_midi_ump_util_test_is_stream_message) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is endpoint discovery", ags_midi_ump_util_test_is_endpoint_discovery) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put endpoint discovery", ags_midi_ump_util_test_put_endpoint_discovery) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get endpoint discovery", ags_midi_ump_util_test_get_endpoint_discovery) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is endpoint info notification", ags_midi_ump_util_test_is_endpoint_info_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put endpoint info notification", ags_midi_ump_util_test_put_endpoint_info_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get endpoint info notification", ags_midi_ump_util_test_get_endpoint_info_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is product instance ID notification", ags_midi_ump_util_test_is_product_instance_id_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put product instance ID notification", ags_midi_ump_util_test_put_product_instance_id_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get product instance ID notification", ags_midi_ump_util_test_get_product_instance_id_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is stream configuration request", ags_midi_ump_util_test_is_stream_configuration_request) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put stream configuration request", ags_midi_ump_util_test_put_stream_configuration_request) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get stream configuration request", ags_midi_ump_util_test_get_stream_configuration_request) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is stream configuration notification", ags_midi_ump_util_test_is_stream_configuration_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put stream configuration notification", ags_midi_ump_util_test_put_stream_configuration_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get stream configuration notification", ags_midi_ump_util_test_get_stream_configuration_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is function block discovery", ags_midi_ump_util_test_is_function_block_discovery) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put function block discovery", ags_midi_ump_util_test_put_function_block_discovery) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get function block discovery", ags_midi_ump_util_test_get_function_block_discovery) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is function block info notification", ags_midi_ump_util_test_is_function_block_info_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put function block info notification", ags_midi_ump_util_test_put_function_block_info_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get function block info notification", ags_midi_ump_util_test_get_function_block_info_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is function block name notification", ags_midi_ump_util_test_is_function_block_name_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put function block name notification", ags_midi_ump_util_test_put_function_block_name_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get function block name notification", ags_midi_ump_util_test_get_function_block_name_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is start of clip", ags_midi_ump_util_test_is_start_of_clip) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put start of clip", ags_midi_ump_util_test_put_start_of_clip) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get start of clip", ags_midi_ump_util_test_get_start_of_clip) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is end of clip", ags_midi_ump_util_test_is_end_of_clip) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put end of clip", ags_midi_ump_util_test_put_end_of_clip) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get end of clip", ags_midi_ump_util_test_get_end_of_clip) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is noop", ags_midi_ump_util_test_is_noop) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put noop", ags_midi_ump_util_test_put_noop) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get noop", ags_midi_ump_util_test_get_noop) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is jitter reduction clock", ags_midi_ump_util_test_is_jr_clock) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put jitter reduction clock", ags_midi_ump_util_test_put_jr_clock) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get jitter reduction clock", ags_midi_ump_util_test_get_jr_clock) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is jitter reduction timestamp", ags_midi_ump_util_test_is_jr_timestamp) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put jitter reduction timestamp", ags_midi_ump_util_test_put_jr_timestamp) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get jitter reduction timestamp", ags_midi_ump_util_test_get_jr_timestamp) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is delta clock ticks per quarter note", ags_midi_ump_util_test_is_delta_clock_ticks_per_quarter_note) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put delta clock ticks per quarter note", ags_midi_ump_util_test_put_delta_clock_ticks_per_quarter_note) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get delta clock ticks per quarter note", ags_midi_ump_util_test_get_delta_clock_ticks_per_quarter_note) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is delta clock ticks since last event", ags_midi_ump_util_test_is_delta_clock_ticks_since_last_event) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put delta clock ticks since last event", ags_midi_ump_util_test_put_delta_clock_ticks_since_last_event) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get delta clock ticks since last event", ags_midi_ump_util_test_get_delta_clock_ticks_since_last_event) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v1.0 channel voice", ags_midi_ump_util_test_is_midi1_channel_voice) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v1.0 note off", ags_midi_ump_util_test_is_midi1_note_off) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v1.0 note off", ags_midi_ump_util_test_put_midi1_note_off) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v1.0 note off", ags_midi_ump_util_test_get_midi1_note_off) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v1.0 note on", ags_midi_ump_util_test_is_midi1_note_on) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v1.0 note on", ags_midi_ump_util_test_put_midi1_note_on) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v1.0 note on", ags_midi_ump_util_test_get_midi1_note_on) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v1.0 polyphonic aftertouch", ags_midi_ump_util_test_is_midi1_polyphonic_aftertouch) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v1.0 polyphonic aftertouch", ags_midi_ump_util_test_put_midi1_polyphonic_aftertouch) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v1.0 polyphonic aftertouch", ags_midi_ump_util_test_get_midi1_polyphonic_aftertouch) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v1.0 control change", ags_midi_ump_util_test_is_midi1_control_change) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v1.0 control change", ags_midi_ump_util_test_put_midi1_control_change) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v1.0 control change", ags_midi_ump_util_test_get_midi1_control_change) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v1.0 program change", ags_midi_ump_util_test_is_midi1_program_change) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v1.0 program change", ags_midi_ump_util_test_put_midi1_program_change) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v1.0 program change", ags_midi_ump_util_test_get_midi1_program_change) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v1.0 channel pressure", ags_midi_ump_util_test_is_midi1_channel_pressure) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v1.0 channel pressure", ags_midi_ump_util_test_put_midi1_channel_pressure) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v1.0 channel pressure", ags_midi_ump_util_test_get_midi1_channel_pressure) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v1.0 pitch bend", ags_midi_ump_util_test_is_midi1_pitch_bend) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v1.0 pitch bend", ags_midi_ump_util_test_put_midi1_pitch_bend) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v1.0 pitch bend", ags_midi_ump_util_test_get_midi1_pitch_bend) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v2.0 channel voice", ags_midi_ump_util_test_is_midi2_channel_voice) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v2.0 note off", ags_midi_ump_util_test_is_midi2_note_off) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v2.0 note off", ags_midi_ump_util_test_put_midi2_note_off) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v2.0 note off", ags_midi_ump_util_test_get_midi2_note_off) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v2.0 note on", ags_midi_ump_util_test_is_midi2_note_on) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v2.0 note on", ags_midi_ump_util_test_put_midi2_note_on) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v2.0 note on", ags_midi_ump_util_test_get_midi2_note_on) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v2.0 polyphonic aftertouch", ags_midi_ump_util_test_is_midi2_polyphonic_aftertouch) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v2.0 polyphonic aftertouch", ags_midi_ump_util_test_put_midi2_polyphonic_aftertouch) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v2.0 polyphonic aftertouch", ags_midi_ump_util_test_get_midi2_polyphonic_aftertouch) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v2.0 registered per note controller", ags_midi_ump_util_test_is_midi2_registered_per_note_controller) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v2.0 registered per note controller", ags_midi_ump_util_test_put_midi2_registered_per_note_controller) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v2.0 registered per note controller", ags_midi_ump_util_test_get_midi2_registered_per_note_controller) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v2.0 assignable per note controller", ags_midi_ump_util_test_is_midi2_assignable_per_note_controller) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v2.0 assignable per note controller", ags_midi_ump_util_test_put_midi2_assignable_per_note_controller) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v2.0 assignable per note controller", ags_midi_ump_util_test_get_midi2_assignable_per_note_controller) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v2.0 per note management", ags_midi_ump_util_test_is_midi2_per_note_management) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v2.0 per note management", ags_midi_ump_util_test_put_midi2_per_note_management) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v2.0 per note management", ags_midi_ump_util_test_get_midi2_per_note_management) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI V2.0 control change", ags_midi_ump_util_test_is_midi2_control_change) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI V2.0 control change", ags_midi_ump_util_test_put_midi2_control_change) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI V2.0 control change", ags_midi_ump_util_test_get_midi2_control_change) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v2.0 RPN pitch bend range", ags_midi_ump_util_test_is_midi2_rpn_pitch_bend_range) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v2.0 RPN pitch bend range", ags_midi_ump_util_test_put_midi2_rpn_pitch_bend_range) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v2.0 RPN pitch bend range", ags_midi_ump_util_test_get_midi2_rpn_pitch_bend_range) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v2.0 RPN coarse tuning", ags_midi_ump_util_test_is_midi2_rpn_coarse_tuning) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v2.0 RPN coarse tuning", ags_midi_ump_util_test_put_midi2_rpn_coarse_tuning) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v2.0 RPN coarse tuning", ags_midi_ump_util_test_get_midi2_rpn_coarse_tuning) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v2.0 RPN tuning program change", ags_midi_ump_util_test_is_midi2_rpn_tuning_program_change) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v2.0 RPN tuning program change", ags_midi_ump_util_test_put_midi2_rpn_tuning_program_change) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v2.0 RPN tuning program change", ags_midi_ump_util_test_get_midi2_rpn_tuning_program_change) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v2.0 RPN tuning bank select", ags_midi_ump_util_test_is_midi2_rpn_tuning_bank_select) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v2.0 RPN tuning bank select", ags_midi_ump_util_test_put_midi2_rpn_tuning_bank_select) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v2.0 RPN tuning bank select", ags_midi_ump_util_test_get_midi2_rpn_tuning_bank_select) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v2.0 RPN MPE MCM", ags_midi_ump_util_test_is_midi2_rpn_mpe_mcm) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v2.0 RPN MPE MCM", ags_midi_ump_util_test_put_midi2_rpn_mpe_mcm) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v2.0 RPN MPE MCM", ags_midi_ump_util_test_get_midi2_rpn_mpe_mcm) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v2.0 program change", ags_midi_ump_util_test_is_midi2_program_change) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v2.0 program change", ags_midi_ump_util_test_put_midi2_program_change) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v2.0 program change", ags_midi_ump_util_test_get_midi2_program_change) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v2.0 channel pressure", ags_midi_ump_util_test_is_midi2_channel_pressure) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v2.0 channel pressure", ags_midi_ump_util_test_put_midi2_channel_pressure) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v2.0 channel pressure", ags_midi_ump_util_test_get_midi2_channel_pressure) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v2.0 pitch bend", ags_midi_ump_util_test_is_midi2_pitch_bend) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v2.0 pitch bend", ags_midi_ump_util_test_put_midi2_pitch_bend) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v2.0 pitch bend", ags_midi_ump_util_test_get_midi2_pitch_bend) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is MIDI v2.0 per note pitch bend", ags_midi_ump_util_test_is_midi2_per_note_pitch_bend) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put MIDI v2.0 per note pitch bend", ags_midi_ump_util_test_put_midi2_per_note_pitch_bend) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get MIDI v2.0 per note pitch bend", ags_midi_ump_util_test_get_midi2_per_note_pitch_bend) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is flex set tempo", ags_midi_ump_util_test_is_flex_set_tempo) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put flex set tempo", ags_midi_ump_util_test_put_flex_set_tempo) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get flex set tempo", ags_midi_ump_util_test_get_flex_set_tempo) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is flex set time signature", ags_midi_ump_util_test_is_flex_set_time_signature) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put flex set time signature", ags_midi_ump_util_test_put_flex_set_time_signature) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get flex set time signature", ags_midi_ump_util_test_get_flex_set_time_signature) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is flex set metronome", ags_midi_ump_util_test_is_flex_set_metronome) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put flex set metronome", ags_midi_ump_util_test_put_flex_set_metronome) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get flex set metronome", ags_midi_ump_util_test_get_flex_set_metronome) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is flex set key signature", ags_midi_ump_util_test_is_flex_set_key_signature) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put flex set key signature", ags_midi_ump_util_test_put_flex_set_key_signature) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get flex set key signature", ags_midi_ump_util_test_get_flex_set_key_signature) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is flex set chord name", ags_midi_ump_util_test_is_flex_set_chord_name) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put flex set chord name", ags_midi_ump_util_test_put_flex_set_chord_name) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get flex set chord name", ags_midi_ump_util_test_get_flex_set_chord_name) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
