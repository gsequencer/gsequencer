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

  gboolean retval;
  
  const guchar buffer[512] = "\xf0\x2\x0\x0\x0\x7e\x15\x3\x2d\x3d\x6f\x7d\x0\x2\x0";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_device_identity_notification(midi_ump_util,
							     buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_device_identity_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\xf0\x2\x0\x0\x0\x3\x15\x7e\x2d\x3d\x6f\x7d\x0\x2\x0";
  
  gint device_manufacturer = 0xcafe;
  gint device_family = 0xdead;
  gint device_family_model = 0xbeef;
  gint software_revision = 0x0100;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_device_identity_notification(midi_ump_util,
						     buffer,
						     device_manufacturer,
						     device_family,
						     device_family_model,
						     software_revision,
						     NULL, NULL,
						     0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_device_identity_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\xf0\x2\x0\x0\x0\x3\x15\x7e\x2d\x3d\x6f\x7d\x0\x2\x0";

  gint device_manufacturer;
  gint device_family;
  gint device_family_model;
  gint software_revision;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_device_identity_notification(midi_ump_util,
						     buffer,
						     &device_manufacturer,
						     &device_family,
						     &device_family_model,
						     &software_revision,
						     NULL, NULL,
						     0);

  CU_ASSERT(device_manufacturer == 0xcafe);
  CU_ASSERT(device_family == 0x1ead);
  CU_ASSERT(device_family_model == 0x3eef);
  CU_ASSERT(software_revision == 0x0100);
}

void
ags_midi_ump_util_test_is_endpoint_name_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\xf0\x03\x6c\x69\x67\x68\x74\x79\x65\x61\x72\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_endpoint_name_notification(midi_ump_util,
							   buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_endpoint_name_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\xf0\x03\x6c\x69\x67\x68\x74\x79\x65\x61\x72\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  
  gchar *endpoint_name = "lightyear";
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_endpoint_name_notification(midi_ump_util,
						   buffer,
						   endpoint_name,
						   NULL, NULL,
						   0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_endpoint_name_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\xf0\x03\x6c\x69\x67\x68\x74\x79\x65\x61\x72\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gchar *endpoint_name;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_endpoint_name_notification(midi_ump_util,
						   buffer,
						   &endpoint_name,
						   NULL, NULL,
						   0);

  CU_ASSERT(!strncmp(endpoint_name, "lightyear", 9));
}

void
ags_midi_ump_util_test_is_product_instance_id_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\xf0\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_product_instance_id_notification(midi_ump_util,
								 buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_product_instance_id_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\xf0\x04\x62\x75\x7a\x7a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  
  gchar *product_instance_id = "buzz";
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_product_instance_id_notification(midi_ump_util,
							 buffer,
							 product_instance_id,
							 NULL, NULL,
							 0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_product_instance_id_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\xf0\x04\x62\x75\x7a\x7a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gchar *product_instance_id;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_product_instance_id_notification(midi_ump_util,
							 buffer,
							 &product_instance_id,
							 NULL, NULL,
							 0);

  CU_ASSERT(!strncmp(product_instance_id, "buzz", 4));
}

void
ags_midi_ump_util_test_is_stream_configuration_request()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\xf0\x05\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_stream_configuration_request(midi_ump_util,
							     buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_stream_configuration_request()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\xf0\x05\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  
  gint protocol = 0x2;
  gboolean rx_jitter_reduction = FALSE;
  gboolean tx_jitter_reduction = FALSE;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_stream_configuration_request(midi_ump_util,
						     buffer,
						     protocol,
						     rx_jitter_reduction, tx_jitter_reduction,
						     NULL, NULL,
						     0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_stream_configuration_request()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\xf0\x05\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint protocol;
  gboolean rx_jitter_reduction;
  gboolean tx_jitter_reduction;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_stream_configuration_request(midi_ump_util,
						     buffer,
						     &protocol,
						     &rx_jitter_reduction, &tx_jitter_reduction,
						     NULL, NULL,
						     0);

  CU_ASSERT(protocol == 0x2);
  CU_ASSERT(rx_jitter_reduction == FALSE);
  CU_ASSERT(tx_jitter_reduction == FALSE);
}

void
ags_midi_ump_util_test_is_stream_configuration_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\xf0\x06\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_stream_configuration_notification(midi_ump_util,
								  buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_stream_configuration_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\xf0\x06\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  
  gint protocol = 0x2;
  gboolean rx_jitter_reduction = FALSE;
  gboolean tx_jitter_reduction = FALSE;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_stream_configuration_notification(midi_ump_util,
							  buffer,
							  protocol,
							  rx_jitter_reduction, tx_jitter_reduction,
							  NULL, NULL,
							  0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_stream_configuration_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\xf0\x06\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint protocol;
  gboolean rx_jitter_reduction;
  gboolean tx_jitter_reduction;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_stream_configuration_notification(midi_ump_util,
							  buffer,
							  &protocol,
							  &rx_jitter_reduction, &tx_jitter_reduction,
							  NULL, NULL,
							  0);

  CU_ASSERT(protocol == 0x2);
  CU_ASSERT(rx_jitter_reduction == FALSE);
  CU_ASSERT(tx_jitter_reduction == FALSE);
}

void
ags_midi_ump_util_test_is_function_block_discovery()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\xf0\x10\x01f\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_function_block_discovery(midi_ump_util,
							 buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_function_block_discovery()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\xf0\x10\x01f\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint function_block = 0x1f;
  gint filter = 0x2;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_function_block_discovery(midi_ump_util,
						 buffer,
						 function_block,
						 filter,
						 NULL, NULL,
						 0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_function_block_discovery()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\xf0\x10\x1f\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint function_block;
  gint filter;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_function_block_discovery(midi_ump_util,
						 buffer,
						 &function_block,
						 &filter,
						 NULL, NULL,
						 0);

  CU_ASSERT(function_block == 0x1f);
  CU_ASSERT(filter == 0x02);
}

void
ags_midi_ump_util_test_is_function_block_info_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\xf0\x11\x9f\x00\x00\x00\x00\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_function_block_info_notification(midi_ump_util,
								 buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_function_block_info_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\xf0\x11\x9f\x02\x00\x00\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gboolean function_block_active = TRUE;
  gint function_block = 0x1f;
  gint direction = 0x2;
  gint midi1 = 0x0;
  gint ui_hint = 0x0;
  gint first_group = 0x0;
  gint groups_spanned = 0x0;
  gint midi_ci_version = 0x1;
  gint max_sysex_8_streams = 0x1;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_function_block_info_notification(midi_ump_util,
							 buffer,
							 function_block_active,
							 function_block,
							 direction,
							 midi1,
							 ui_hint,
							 first_group,
							 groups_spanned,
							 midi_ci_version,
							 max_sysex_8_streams,
							 NULL, NULL,
							 0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_function_block_info_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\xf0\x11\x9f\x02\x00\x00\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gboolean function_block_active = TRUE;
  gint function_block = 0x1f;
  gint direction = 0x2;
  gint midi1 = 0x0;
  gint ui_hint = 0x0;
  gint first_group = 0x0;
  gint groups_spanned = 0x0;
  gint midi_ci_version = 0x1;
  gint max_sysex_8_streams = 0x1;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_function_block_info_notification(midi_ump_util,
							 buffer,
							 &function_block_active,
							 &function_block,
							 &direction,
							 &midi1,
							 &ui_hint,
							 &first_group,
							 &groups_spanned,
							 &midi_ci_version,
							 &max_sysex_8_streams,
							 NULL, NULL,
							 0);

  CU_ASSERT(function_block_active == TRUE);
  CU_ASSERT(function_block == 0x1f);
  CU_ASSERT(direction == 0x2);
  CU_ASSERT(midi1 == 0x0);
  CU_ASSERT(ui_hint == 0x0);
  CU_ASSERT(first_group == 0x0);
  CU_ASSERT(groups_spanned == 0x0);
  CU_ASSERT(midi_ci_version == 0x1);
  CU_ASSERT(max_sysex_8_streams == 0x1);
}

void
ags_midi_ump_util_test_is_function_block_name_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\xf0\x12\x1f\x6c\x69\x67\x68\x74\x79\x65\x61\x72\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_function_block_name_notification(midi_ump_util,
								 buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_function_block_name_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\xf0\x12\x1f\x6c\x69\x67\x68\x74\x79\x65\x61\x72\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  
  gchar *function_block_name = "lightyear";

  gint function_block = 0x1f;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_function_block_name_notification(midi_ump_util,
							 buffer,
							 function_block,
							 function_block_name,
							 NULL, NULL,
							 0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_function_block_name_notification()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\xf0\x12\x1f\x6c\x69\x67\x68\x74\x79\x65\x61\x72\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gchar *function_block_name;

  gint function_block;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_function_block_name_notification(midi_ump_util,
							 buffer,
							 &function_block,
							 &function_block_name,
							 NULL, NULL,
							 0);

  CU_ASSERT(function_block == 0x1f);
  CU_ASSERT(!strncmp(function_block_name, "lightyear", 9));
}

void
ags_midi_ump_util_test_is_start_of_clip()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\xf0\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_start_of_clip(midi_ump_util,
					      buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_start_of_clip()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\xf0\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_start_of_clip(midi_ump_util,
				      buffer,
				      NULL, NULL,
				      0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_start_of_clip()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\xf0\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  
  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_start_of_clip(midi_ump_util,
				      buffer,
				      NULL, NULL,
				      0);
}

void
ags_midi_ump_util_test_is_end_of_clip()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\xf0\x21\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_end_of_clip(midi_ump_util,
					    buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_end_of_clip()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\xf0\x21\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_end_of_clip(midi_ump_util,
				    buffer,
				    NULL, NULL,
				    0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_end_of_clip()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\xf0\x21\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  
  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_end_of_clip(midi_ump_util,
				    buffer,
				    NULL, NULL,
				    0);
}

void
ags_midi_ump_util_test_is_noop()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_noop(midi_ump_util,
				     buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_noop()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_noop(midi_ump_util,
			     buffer,
			     NULL, NULL,
			     0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_noop()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  
  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_noop(midi_ump_util,
			     buffer,
			     NULL, NULL,
			     0);
}

void
ags_midi_ump_util_test_is_jr_clock()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_jr_clock(midi_ump_util,
					 buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_jr_clock()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  guint16 sender_clock_time = 0;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_jr_clock(midi_ump_util,
				 buffer,
				 sender_clock_time,
				 NULL, NULL,
				 0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_jr_clock()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  
  guint16 sender_clock_time;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_jr_clock(midi_ump_util,
				 buffer,
				 &sender_clock_time,
				 NULL, NULL,
				 0);

  CU_ASSERT(sender_clock_time == 0);
}

void
ags_midi_ump_util_test_is_jr_timestamp()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x00\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_jr_timestamp(midi_ump_util,
					     buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_jr_timestamp()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x00\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  guint16 sender_clock_time = 0;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_jr_timestamp(midi_ump_util,
				     buffer,
				     sender_clock_time,
				     NULL, NULL,
				     0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_jr_timestamp()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x00\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  
  guint16 sender_clock_time;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_jr_timestamp(midi_ump_util,
				     buffer,
				     &sender_clock_time,
				     NULL, NULL,
				     0);

  CU_ASSERT(sender_clock_time == 0);
}

void
ags_midi_ump_util_test_is_delta_clock_ticks_per_quarter_note()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x00\x30\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_delta_clock_ticks_per_quarter_note(midi_ump_util,
								   buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_delta_clock_ticks_per_quarter_note()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x00\x30\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  guint16 ticks_per_quarter_note = 1;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_delta_clock_ticks_per_quarter_note(midi_ump_util,
							   buffer,
							   ticks_per_quarter_note,
							   NULL, NULL,
							   0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_delta_clock_ticks_per_quarter_note()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x00\x30\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  
  guint16 ticks_per_quarter_note;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_delta_clock_ticks_per_quarter_note(midi_ump_util,
							   buffer,
							   &ticks_per_quarter_note,
							   NULL, NULL,
							   0);

  CU_ASSERT(ticks_per_quarter_note == 1);
}

void
ags_midi_ump_util_test_is_delta_clock_ticks_since_last_event()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x00\x40\x01\x90\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_delta_clock_ticks_since_last_event(midi_ump_util,
								   buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_delta_clock_ticks_since_last_event()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x00\x40\x01\x90\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  guint16 ticks_since_last_event = 400;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_delta_clock_ticks_since_last_event(midi_ump_util,
							   buffer,
							   ticks_since_last_event,
							   NULL, NULL,
							   0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_delta_clock_ticks_since_last_event()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x00\x40\x01\x90\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  
  guint16 ticks_since_last_event;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_delta_clock_ticks_since_last_event(midi_ump_util,
							   buffer,
							   &ticks_since_last_event,
							   NULL, NULL,
							   0);

  CU_ASSERT(ticks_since_last_event == 400);
}

void
ags_midi_ump_util_test_is_midi1_channel_voice()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x20\x80\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi1_channel_voice(midi_ump_util,
						    buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_is_midi1_note_off()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x20\x80\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi1_note_off(midi_ump_util,
					       buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_midi1_note_off()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x20\x80\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint key = 48;
  gint velocity = 127;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_midi1_note_off(midi_ump_util,
				       buffer,
				       0,
				       0,
				       key,
				       velocity,
				       NULL, NULL,
				       0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_midi1_note_off()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x20\x80\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint group;
  gint channel;
  gint key;
  gint velocity;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_midi1_note_off(midi_ump_util,
				       buffer,
				       &group,
				       &channel,
				       &key,
				       &velocity,
				       NULL, NULL,
				       0);
  
  CU_ASSERT(group == 0);
  CU_ASSERT(channel == 0);
  CU_ASSERT(key == 48);
  CU_ASSERT(velocity == 127);
}

void
ags_midi_ump_util_test_is_midi1_note_on()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x20\x90\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi1_note_on(midi_ump_util,
					      buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_midi1_note_on()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x20\x90\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint key = 48;
  gint velocity = 127;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_midi1_note_on(midi_ump_util,
				      buffer,
				      0,
				      0,
				      key,
				      velocity,
				      NULL, NULL,
				      0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_midi1_note_on()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x20\x90\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint group;
  gint channel;
  gint key;
  gint velocity;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_midi1_note_on(midi_ump_util,
				      buffer,
				      &group,
				      &channel,
				      &key,
				      &velocity,
				      NULL, NULL,
				      0);
  
  CU_ASSERT(group == 0);
  CU_ASSERT(channel == 0);
  CU_ASSERT(key == 48);
  CU_ASSERT(velocity == 127);
}

void
ags_midi_ump_util_test_is_midi1_polyphonic_aftertouch()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x20\xa0\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi1_polyphonic_aftertouch(midi_ump_util,
							    buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_midi1_polyphonic_aftertouch()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x20\xa0\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint key = 48;
  gint data = 127;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_midi1_polyphonic_aftertouch(midi_ump_util,
						    buffer,
						    0,
						    0,
						    key,
						    data,
						    NULL, NULL,
						    0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_midi1_polyphonic_aftertouch()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x20\xa0\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint group;
  gint channel;
  gint key;
  gint data;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_midi1_polyphonic_aftertouch(midi_ump_util,
						    buffer,
						    &group,
						    &channel,
						    &key,
						    &data,
						    NULL, NULL,
						    0);
  
  CU_ASSERT(group == 0);
  CU_ASSERT(channel == 0);
  CU_ASSERT(key == 48);
  CU_ASSERT(data == 127);
}

void
ags_midi_ump_util_test_is_midi1_control_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x20\xb0\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi1_control_change(midi_ump_util,
						     buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_midi1_control_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x20\xb0\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint index_key = 48;
  gint data = 127;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_midi1_control_change(midi_ump_util,
					     buffer,
					     0,
					     0,
					     index_key,
					     data,
					     NULL, NULL,
					     0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_midi1_control_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x20\xb0\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint group;
  gint channel;
  gint index_key;
  gint data;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_midi1_control_change(midi_ump_util,
					     buffer,
					     &group,
					     &channel,
					     &index_key,
					     &data,
					     NULL, NULL,
					     0);
  
  CU_ASSERT(group == 0);
  CU_ASSERT(channel == 0);
  CU_ASSERT(index_key == 48);
  CU_ASSERT(data == 127);
}

void
ags_midi_ump_util_test_is_midi1_program_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x20\xc0\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi1_program_change(midi_ump_util,
						     buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_midi1_program_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x20\xc0\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint program = 48;
  gint data = 127;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_midi1_program_change(midi_ump_util,
					     buffer,
					     0,
					     0,
					     program,
					     data,
					     NULL, NULL,
					     0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_midi1_program_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x20\xc0\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint group;
  gint channel;
  gint program;
  gint data;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_midi1_program_change(midi_ump_util,
					     buffer,
					     &group,
					     &channel,
					     &program,
					     &data,
					     NULL, NULL,
					     0);
  
  CU_ASSERT(group == 0);
  CU_ASSERT(channel == 0);
  CU_ASSERT(program == 48);
  CU_ASSERT(data == 127);
}

void
ags_midi_ump_util_test_is_midi1_channel_pressure()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x20\xd0\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi1_channel_pressure(midi_ump_util,
						       buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_midi1_channel_pressure()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x20\xd0\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint pressure = 48;
  gint data = 127;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_midi1_channel_pressure(midi_ump_util,
					       buffer,
					       0,
					       0,
					       pressure,
					       data,
					       NULL, NULL,
					       0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_midi1_channel_pressure()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x20\xd0\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint group;
  gint channel;
  gint pressure;
  gint data;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_midi1_channel_pressure(midi_ump_util,
					       buffer,
					       &group,
					       &channel,
					       &pressure,
					       &data,
					       NULL, NULL,
					       0);
  
  CU_ASSERT(group == 0);
  CU_ASSERT(channel == 0);
  CU_ASSERT(pressure == 48);
  CU_ASSERT(data == 127);
}

void
ags_midi_ump_util_test_is_midi1_pitch_bend()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x20\xe0\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi1_pitch_bend(midi_ump_util,
						 buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_midi1_pitch_bend()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x20\xe0\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint pitch = 48;
  gint data = 127;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_midi1_pitch_bend(midi_ump_util,
					 buffer,
					 0,
					 0,
					 pitch,
					 data,
					 NULL, NULL,
					 0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_midi1_pitch_bend()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x20\xe0\x30\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint group;
  gint channel;
  gint pitch;
  gint data;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_midi1_pitch_bend(midi_ump_util,
					 buffer,
					 &group,
					 &channel,
					 &pitch,
					 &data,
					 NULL, NULL,
					 0);
  
  CU_ASSERT(group == 0);
  CU_ASSERT(channel == 0);
  CU_ASSERT(pitch == 48);
  CU_ASSERT(data == 127);
}

void
ags_midi_ump_util_test_is_midi2_channel_voice()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x40\x80\x30\x00\x7f\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi2_channel_voice(midi_ump_util,
						    buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_is_midi2_note_off()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x40\x80\x30\x00\x7f\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi2_note_off(midi_ump_util,
					       buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_midi2_note_off()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x40\x80\x30\x00\x7f\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint key = 48;
  gint velocity = 0x7fff;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_midi2_note_off(midi_ump_util,
				       buffer,
				       0,
				       0,
				       key,
				       0,
				       velocity,
				       0,
				       NULL, NULL,
				       0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_midi2_note_off()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x40\x80\x30\x00\x7f\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint group;
  gint channel;
  gint key;
  gint attribute_id;
  gint velocity;
  gint attribute_value;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_midi2_note_off(midi_ump_util,
				       buffer,
				       &group,
				       &channel,
				       &key,
				       &attribute_id,
				       &velocity,
				       &attribute_value,
				       NULL, NULL,
				       0);
  
  CU_ASSERT(group == 0);
  CU_ASSERT(channel == 0);
  CU_ASSERT(key == 48);
  CU_ASSERT(attribute_id == 0);
  CU_ASSERT(velocity == 0x7fff);
  CU_ASSERT(attribute_value == 0);
}

void
ags_midi_ump_util_test_is_midi2_note_on()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x40\x90\x30\x00\x7f\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi2_note_on(midi_ump_util,
					      buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_midi2_note_on()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x40\x90\x30\x00\x7f\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint key = 48;
  gint velocity = 0x7fff;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_midi2_note_on(midi_ump_util,
				      buffer,
				      0,
				      0,
				      key,
				      0,
				      velocity,
				      0,
				      NULL, NULL,
				      0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_midi2_note_on()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x40\x90\x30\x00\x7f\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint group;
  gint channel;
  gint key;
  gint attribute_id;
  gint data;
  gint attribute_value;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_midi2_note_on(midi_ump_util,
				      buffer,
				      &group,
				      &channel,
				      &key,
				      &attribute_id,
				      &data,
				      &attribute_value,
				      NULL, NULL,
				      0);
  
  CU_ASSERT(group == 0);
  CU_ASSERT(channel == 0);
  CU_ASSERT(key == 48);
  CU_ASSERT(attribute_id == 0);
  CU_ASSERT(data == 0x7fff);
  CU_ASSERT(attribute_value == 0);
}

void
ags_midi_ump_util_test_is_midi2_polyphonic_aftertouch()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x40\xa0\x30\x00\x00\x00\x7f\xff\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi2_polyphonic_aftertouch(midi_ump_util,
							    buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_midi2_polyphonic_aftertouch()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x40\xa0\x30\x00\x00\x00\x7f\xff\x00\x00\x00\x00\x00\x00\x00\x00";

  gint key = 48;
  gint data = 0x7fff;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_midi2_polyphonic_aftertouch(midi_ump_util,
						    buffer,
						    0,
						    0,
						    key,
						    data,
						    NULL, NULL,
						    0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_midi2_polyphonic_aftertouch()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x40\xa0\x30\x00\x00\x00\x7f\xff\x00\x00\x00\x00\x00\x00\x00\x00";

  gint group;
  gint channel;
  gint key;
  gint data;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_midi2_polyphonic_aftertouch(midi_ump_util,
						    buffer,
						    &group,
						    &channel,
						    &key,
						    &data,
						    NULL, NULL,
						    0);
  
  CU_ASSERT(group == 0);
  CU_ASSERT(channel == 0);
  CU_ASSERT(key == 48);
  CU_ASSERT(data == 0x7fff);
}

void
ags_midi_ump_util_test_is_midi2_registered_per_note_controller()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x40\x00\x30\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi2_registered_per_note_controller(midi_ump_util,
								     buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_midi2_registered_per_note_controller()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x40\x00\x30\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint key = 48;
  gint data_index = 0x0;
  gint data = 0x0;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_midi2_registered_per_note_controller(midi_ump_util,
							     buffer,
							     0,
							     0,
							     key,
							     data_index,
							     data,
							     NULL, NULL,
							     0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_midi2_registered_per_note_controller()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x40\x00\x30\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint group;
  gint channel;
  gint key;
  gint data;
  gint data_index;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_midi2_registered_per_note_controller(midi_ump_util,
							     buffer,
							     &group,
							     &channel,
							     &key,
							     &data_index,
							     &data,
							     NULL, NULL,
							     0);
  
  CU_ASSERT(group == 0);
  CU_ASSERT(channel == 0);
  CU_ASSERT(key == 48);
  CU_ASSERT(data == 0x0);
  CU_ASSERT(data_index == 0x0);
}

void
ags_midi_ump_util_test_is_midi2_assignable_per_note_controller()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x40\x10\x30\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi2_assignable_per_note_controller(midi_ump_util,
								     buffer);

  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_midi2_assignable_per_note_controller()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x40\x10\x30\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint key = 48;
  gint data_index = 0x0;
  gint data = 0x0;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_midi2_assignable_per_note_controller(midi_ump_util,
							     buffer,
							     0,
							     0,
							     key,
							     data_index,
							     data,
							     NULL, NULL,
							     0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_midi2_assignable_per_note_controller()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x40\x10\x30\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint group;
  gint channel;
  gint key;
  gint data;
  gint data_index;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_midi2_assignable_per_note_controller(midi_ump_util,
							     buffer,
							     &group,
							     &channel,
							     &key,
							     &data_index,
							     &data,
							     NULL, NULL,
							     0);
  
  CU_ASSERT(group == 0);
  CU_ASSERT(channel == 0);
  CU_ASSERT(key == 48);
  CU_ASSERT(data == 0x0);
  CU_ASSERT(data_index == 0x0);
}

void
ags_midi_ump_util_test_is_midi2_per_note_management()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x40\xf0\x30\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi2_per_note_management(midi_ump_util,
							  buffer);
  
  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_midi2_per_note_management()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x40\xf0\x30\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint key = 48;
  gint options_flags = 0x0;
  
  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_midi2_per_note_management(midi_ump_util,
						  buffer,
						  0,
						  0,
						  key,
						  options_flags,
						  NULL, NULL,
						  0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_midi2_per_note_management()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x40\xf0\x30\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint group;
  gint channel;
  gint key;
  gint options_flags;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_midi2_per_note_management(midi_ump_util,
						  buffer,
						  &group,
						  &channel,
						  &key,
						  &options_flags,
						  NULL, NULL,
						  0);
  
  CU_ASSERT(group == 0);
  CU_ASSERT(channel == 0);
  CU_ASSERT(key == 48);
  CU_ASSERT(options_flags == 0x0);
}

void
ags_midi_ump_util_test_is_midi2_control_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x40\xb0\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi2_control_change(midi_ump_util,
							  buffer);
  
  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_midi2_control_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x40\xb0\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint index_key = 0x7f;

  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_midi2_control_change(midi_ump_util,
						  buffer,
						  0,
						  0,
						  index_key,
						  NULL, NULL,
						  0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_midi2_control_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x40\xb0\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint group;
  gint channel;
  gint index_key;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_midi2_control_change(midi_ump_util,
					     buffer,
					     &group,
					     &channel,
					     &index_key,
					     NULL, NULL,
					     0);
  
  CU_ASSERT(group == 0);
  CU_ASSERT(channel == 0);
  CU_ASSERT(index_key == 127);
}

void
ags_midi_ump_util_test_is_midi2_rpn_pitch_bend_range()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x40\x20\x00\x00\x1f\xc0\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi2_rpn_pitch_bend_range(midi_ump_util,
							   buffer);
  
  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_midi2_rpn_pitch_bend_range()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x40\x20\x00\x00\x11\xfc\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint semitones = 8;
  gint cents = 127;

  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_midi2_rpn_pitch_bend_range(midi_ump_util,
						   buffer,
						   0,
						   0,
						   semitones,
						   cents,
						   NULL, NULL,
						   0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_midi2_rpn_pitch_bend_range()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x40\x20\x00\x00\x11\xfc\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint group;
  gint channel;
  gint semitones;
  gint cents;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_midi2_rpn_pitch_bend_range(midi_ump_util,
						   buffer,
						   &group,
						   &channel,
						   &semitones,
						   &cents,
						   NULL, NULL,
						   0);
  
  CU_ASSERT(group == 0);
  CU_ASSERT(channel == 0);
  CU_ASSERT(semitones == 8);
  CU_ASSERT(cents == 127);
}

void
ags_midi_ump_util_test_is_midi2_rpn_coarse_tuning()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x40\x20\x00\x02\xfe\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi2_rpn_coarse_tuning(midi_ump_util,
							buffer);
  
  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_midi2_rpn_coarse_tuning()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x40\x20\x00\x02\xfe\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint coarse_tuning = 127;

  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_midi2_rpn_coarse_tuning(midi_ump_util,
						buffer,
						0,
						0,
						coarse_tuning,
						NULL, NULL,
						0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_midi2_rpn_coarse_tuning()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x40\x20\x00\x02\xfe\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint group;
  gint channel;
  gint coarse_tuning;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_midi2_rpn_coarse_tuning(midi_ump_util,
						buffer,
						&group,
						&channel,
						&coarse_tuning,
						NULL, NULL,
						0);
  
  CU_ASSERT(group == 0);
  CU_ASSERT(channel == 0);
  CU_ASSERT(coarse_tuning == 127);
}

void
ags_midi_ump_util_test_is_midi2_rpn_tuning_program_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  gboolean retval;
  
  const guchar buffer[512] = "\x40\x20\x00\x03\xfe\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  midi_ump_util = ags_midi_ump_util_alloc();

  retval = ags_midi_ump_util_is_midi2_rpn_tuning_program_change(midi_ump_util,
								buffer);
  
  CU_ASSERT(retval == TRUE);
}

void
ags_midi_ump_util_test_put_midi2_rpn_tuning_program_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  guchar buffer[512];
  const guchar filled_buffer[512] = "\x40\x20\x00\x03\xfe\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint tuning_program_number = 127;

  midi_ump_util = ags_midi_ump_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ump_util_put_midi2_rpn_tuning_program_change(midi_ump_util,
							buffer,
							0,
							0,
							tuning_program_number,
							NULL, NULL,
							0);
  
  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ump_util_test_get_midi2_rpn_tuning_program_change()
{
  AgsMidiUmpUtil *midi_ump_util;

  const guchar buffer[512] = "\x40\x20\x00\x03\xfe\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  gint group;
  gint channel;
  gint tuning_program_number;

  midi_ump_util = ags_midi_ump_util_alloc();

  ags_midi_ump_util_get_midi2_rpn_tuning_program_change(midi_ump_util,
							buffer,
							&group,
							&channel,
							&tuning_program_number,
							NULL, NULL,
							0);
  
  CU_ASSERT(group == 0);
  CU_ASSERT(channel == 0);
  CU_ASSERT(tuning_program_number == 127);
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
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is device identity notification", ags_midi_ump_util_test_is_device_identity_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put device identity notification", ags_midi_ump_util_test_put_device_identity_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get device identity notification", ags_midi_ump_util_test_get_device_identity_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c is endpoint name notification", ags_midi_ump_util_test_is_endpoint_name_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c put endpoint name notification", ags_midi_ump_util_test_put_endpoint_name_notification) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ump_util.c get endpoint name notification", ags_midi_ump_util_test_get_endpoint_name_notification) == NULL) ||
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
