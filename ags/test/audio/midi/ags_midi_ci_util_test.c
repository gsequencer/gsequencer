/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

int ags_midi_ci_util_test_init_suite();
int ags_midi_ci_util_test_clean_suite();

void ags_midi_ci_util_test_alloc();
void ags_midi_ci_util_test_free();
void ags_midi_ci_util_test_copy();
void ags_midi_ci_util_test_generate_muid();
void ags_midi_ci_util_test_put_discovery();
void ags_midi_ci_util_test_get_discovery();
void ags_midi_ci_util_test_put_discovery_reply();
void ags_midi_ci_util_test_get_discovery_reply();
void ags_midi_ci_util_test_put_invalidate_muid();
void ags_midi_ci_util_test_get_invalidate_muid();
void ags_midi_ci_util_test_put_ack();
void ags_midi_ci_util_test_get_ack();
void ags_midi_ci_util_test_put_nak();
void ags_midi_ci_util_test_get_nak();
void ags_midi_ci_util_test_put_initiate_protocol_negotiation();
void ags_midi_ci_util_test_get_initiate_protocol_negotiation();
void ags_midi_ci_util_test_put_initiate_protocol_negotiation_reply();
void ags_midi_ci_util_test_get_initiate_protocol_negotiation_reply();
void ags_midi_ci_util_test_put_set_protocol_type();
void ags_midi_ci_util_test_get_set_protocol_type();
void ags_midi_ci_util_test_put_confirm_protocol_type();
void ags_midi_ci_util_test_get_confirm_protocol_type();
void ags_midi_ci_util_test_put_confirm_protocol_type_reply();
void ags_midi_ci_util_test_get_confirm_protocol_type_reply();
void ags_midi_ci_util_test_put_confirm_protocol_type_established();
void ags_midi_ci_util_test_get_confirm_protocol_type_established();
void ags_midi_ci_util_test_put_profile();
void ags_midi_ci_util_test_get_profile();
void ags_midi_ci_util_test_put_profile_reply();
void ags_midi_ci_util_test_get_profile_reply();
void ags_midi_ci_util_test_put_profile_enabled_report();
void ags_midi_ci_util_test_get_profile_enabled_report();
void ags_midi_ci_util_test_put_profile_disabled_report();
void ags_midi_ci_util_test_get_profile_disabled_report();
void ags_midi_ci_util_test_put_profile_added();
void ags_midi_ci_util_test_get_profile_added();
void ags_midi_ci_util_test_put_profile_removed();
void ags_midi_ci_util_test_get_profile_removed();
void ags_midi_ci_util_test_put_profile_specific_data();
void ags_midi_ci_util_test_get_profile_specific_data();
void ags_midi_ci_util_test_put_property_exchange_capabilities();
void ags_midi_ci_util_test_get_property_exchange_capabilities();
void ags_midi_ci_util_test_put_property_exchange_capabilities_reply();
void ags_midi_ci_util_test_get_property_exchange_capabilities_reply();
void ags_midi_ci_util_test_put_get_property_data();
void ags_midi_ci_util_test_get_get_property_data();
void ags_midi_ci_util_test_put_get_property_data_reply();
void ags_midi_ci_util_test_get_get_property_data_reply();
void ags_midi_ci_util_test_put_set_property_data();
void ags_midi_ci_util_test_get_set_property_data();
void ags_midi_ci_util_test_put_set_property_data_reply();
void ags_midi_ci_util_test_get_set_property_data_reply();
void ags_midi_ci_util_test_put_subscription();
void ags_midi_ci_util_test_get_subscription();
void ags_midi_ci_util_test_put_subscription_reply();
void ags_midi_ci_util_test_get_subscription_reply();
void ags_midi_ci_util_test_put_process_capabilities();
void ags_midi_ci_util_test_get_process_capabilities();
void ags_midi_ci_util_test_put_process_capabilities_reply();
void ags_midi_ci_util_test_get_process_capabilities_reply();
void ags_midi_ci_util_test_put_message_report();
void ags_midi_ci_util_test_get_message_report();
void ags_midi_ci_util_test_put_message_report_reply();
void ags_midi_ci_util_test_get_message_report_reply();
void ags_midi_ci_util_test_put_end_of_message_report();
void ags_midi_ci_util_test_get_end_of_message_report();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_midi_ci_util_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_midi_ci_util_test_clean_suite()
{
  return(0);
}

void
ags_midi_ci_util_test_alloc()
{
  AgsMidiCIUtil *midi_ci_util;

  midi_ci_util = ags_midi_ci_util_alloc();

  CU_ASSERT(midi_ci_util != NULL);
}

void
ags_midi_ci_util_test_free()
{
  AgsMidiCIUtil *midi_ci_util;

  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_free(midi_ci_util);
}

void
ags_midi_ci_util_test_copy()
{
  AgsMidiCIUtil *a, *b;

  a = ags_midi_ci_util_alloc();

  b = ags_midi_ci_util_copy(a);
  
  CU_ASSERT(b != NULL);
}

void
ags_midi_ci_util_test_generate_muid()
{
  AgsMidiCIUtil *midi_ci_util;

  AgsMUID muid;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  muid = ags_midi_ci_util_generate_muid(midi_ci_util);
  
  CU_ASSERT(muid != AGS_MIDI_CI_UTIL_BROADCAST_MUID);
}

void
ags_midi_ci_util_test_put_discovery()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2d\x7e\x01\x70\x7f\x7f\x6f\x3e\x00\x08\x7f\x7f\x52\xaf\xfe\x00\x00\x05\x00\x0a\x02\x00\x05\x05\x00\xf7\x00\x00";

  AgsMUID source = 0x0eadbeef;
  
  guchar device_id = 0x7f;
  guchar version = '\x01';
  guchar manufacturer_id[3] = "\x08\x00\x00";
  guint16 device_family = 0xaffe;
  guint16 device_family_model_number = 0x0a52;  
  guchar software_revision_level[4] = "\x00\x05\x00\x05";
  guchar capability = '\x05';
  guint32 max_sysex_message_size = 512;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_put_discovery(midi_ci_util,
				 buffer,
				 device_id,
				 version,
				 source,
				 manufacturer_id,
				 device_family,
				 device_family_model_number,
				 software_revision_level,
				 capability,
				 max_sysex_message_size);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 32 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_discovery()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2d\x7e\x01\x70\x7f\x7f\x6f\x3e\x00\x08\x7f\x7f\x52\xaf\xfe\x00\x00\x05\x00\x0a\x02\x00\x05\x05\x00\xf7\x00\x00";

  AgsMUID source;

  guchar device_id;
  guchar version;
  guchar manufacturer_id[3];
  guint16 device_family;
  guint16 device_family_model_number;
  guchar software_revision_level[4];
  guchar capability;
  guint32 max_sysex_message_size;

  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_get_discovery(midi_ci_util,
				 buffer,
				 &device_id,
				 &version,
				 &source,
				 manufacturer_id,
				 &device_family,
				 &device_family_model_number,
				 software_revision_level,
				 &capability,
				 &max_sysex_message_size);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0eadbeef);
  CU_ASSERT(!memcmp(manufacturer_id, "\x08\x00\x00", 3 * sizeof(guchar)));
  CU_ASSERT(device_family == 0xaffe);
  CU_ASSERT(device_family_model_number == 0x0a52);
  CU_ASSERT(!memcmp(software_revision_level, "\x00\x05\x00\x05", 4 * sizeof(guchar)));
  CU_ASSERT(capability == 0x05);
  CU_ASSERT(max_sysex_message_size == 512);
}

void
ags_midi_ci_util_test_put_discovery_reply()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x71\x2d\x7e\x10\x60\x00\x08\x6f\x3e\x52\xaf\xfe\x00\x00\x05\x00\x0a\x02\x00\x05\x05\x00\xf7\x00\x00";

  AgsMUID source = 0x0cafe010;
  AgsMUID destination = 0x0eadbeef;
  
  guchar device_id = 0x7f;
  guchar version = '\x01';
  guchar manufacturer_id[] = "\x08\x00\x00";
  guint16 device_family = 0xaffe;
  guint16 device_family_model_number = 0x0a52;  
  guchar software_revision_level[] = "\x00\x05\x00\x05";
  guchar capability = '\x05';
  guint32 max_sysex_message_size = 512;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));

  ags_midi_ci_util_put_discovery_reply(midi_ci_util,
				       buffer,
				       device_id,
				       version,
				       source,
				       destination,
				       manufacturer_id,
				       device_family,
				       device_family_model_number,
				       software_revision_level,
				       capability,
				       max_sysex_message_size);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 32 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_discovery_reply()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x71\x2d\x7e\x10\x60\x00\x08\x6f\x3e\x52\xaf\xfe\x00\x00\x05\x00\x0a\x02\x00\x05\x05\x00\xf7\x00\x00";

  AgsMUID source;
  AgsMUID destination;

  guchar device_id;
  guchar version;
  guchar manufacturer_id[3];
  guint16 device_family;
  guint16 device_family_model_number;
  guchar software_revision_level[4];
  guchar capability;
  guint32 max_sysex_message_size;

  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_get_discovery_reply(midi_ci_util,
				       buffer,
				       &device_id,
				       &version,
				       &source,
				       &destination,
				       manufacturer_id,
				       &device_family,
				       &device_family_model_number,
				       software_revision_level,
				       &capability,
				       &max_sysex_message_size);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(destination == 0x0eadbeef);
  CU_ASSERT(!memcmp(manufacturer_id, "\x08\x00\x00", 3 * sizeof(guchar)));
  CU_ASSERT(device_family == 0xaffe);
  CU_ASSERT(device_family_model_number == 0x0a52);
  CU_ASSERT(!memcmp(software_revision_level, "\x00\x05\x00\x05", 4 * sizeof(guchar)));
  CU_ASSERT(capability == 0x05);
  CU_ASSERT(max_sysex_message_size == 512);
}

void
ags_midi_ci_util_test_put_invalidate_muid()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x7e\x7f\x7f\x10\x60\x2d\x7e\x7f\x7f\x00\xf7\x6f\x3e\x00\x00\x00\x00";

  AgsMUID source = 0x0cafe010;
  AgsMUID target_muid = 0x0eadbeef;
  
  guchar device_id = '\x7f';
  guchar version = '\x01';
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));

  ags_midi_ci_util_put_invalidate_muid(midi_ci_util,
				       buffer,
				       device_id,
				       version,
				       source,
				       target_muid);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 24 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_invalidate_muid()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x7e\x7f\x7f\x10\x60\x2d\x7e\x7f\x7f\x00\xf7\x6f\x3e\x00\x00\x00\x00";

  AgsMUID source;
  AgsMUID target_muid;
  
  guchar device_id;
  guchar version;

  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_get_invalidate_muid(midi_ci_util,
				       buffer,
				       &device_id,
				       &version,
				       &source,
				       &target_muid);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(target_muid == 0x0eadbeef);
}

void
ags_midi_ci_util_test_put_ack()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x7d\x2d\x7e\x10\x60\x00\x34\x6f\x3e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xf7\x00\x00\x00\x00";

  AgsMUID source = 0x0cafe010;
  AgsMUID destination = 0x0eadbeef;
  
  guchar device_id = 0x7f;
  guchar version = '\x01';
  guchar orig_transaction = '\x34';
  guchar status_code = '\x00';
  guchar status_data = '\x00';
  guchar details[5] = "\x00\x00\x00\x00\x00";
  guint16 message_length = 0;
  guchar *message = NULL;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));

  ags_midi_ci_util_put_ack(midi_ci_util,
			   buffer,
			   device_id,
			   version,
			   source,
			   destination,
			   orig_transaction,
			   status_code,
			   status_data,
			   details,
			   message_length,
			   message);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 32 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_ack()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x7d\x2d\x7e\x10\x60\x00\x34\x6f\x3e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xf7\x00\x00\x00\x00";

  AgsMUID source;
  AgsMUID destination;
  
  guchar device_id;
  guchar version;
  guchar orig_transaction;
  guchar status_code;
  guchar status_data;
  guchar details[5];
  guint16 message_length;
  guchar *message = NULL;

  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_get_ack(midi_ci_util,
			   buffer,
			   &device_id,
			   &version,
			   &source,
			   &destination,
			   &orig_transaction,
			   &status_code,
			   &status_data,
			   details,
			   &message_length,
			   &message);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(destination == 0x0eadbeef);
  CU_ASSERT(orig_transaction == 0x34);
  CU_ASSERT(status_code == 0x00);
  CU_ASSERT(status_data == 0x00);
  CU_ASSERT(details[0] == 0x00 && details[1] == 0x00 && details[2] == 0x00 && details[3] == 0x00 && details[4] == 0x00);
  CU_ASSERT(message_length == 0x00);
  CU_ASSERT(message == NULL);
}

void
ags_midi_ci_util_test_put_nak()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x7f\x2d\x7e\x10\x60\x00\xf7\x6f\x3e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  AgsMUID source = 0x0cafe010;
  AgsMUID destination = 0x0eadbeef;

  guchar device_id = '\x7f';
  guchar version = '\x01';
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));

  ags_midi_ci_util_put_nak(midi_ci_util,
			   buffer,
			   device_id,
			   version,
			   source,
			   destination);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_nak()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x7f\x2d\x7e\x10\x60\x00\xf7\x6f\x3e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  AgsMUID source;
  AgsMUID destination;
  
  guchar device_id;
  guchar version;

  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_get_nak(midi_ci_util,
			   buffer,
			   &device_id,
			   &version,
			   &source,
			   &destination);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(destination == 0x0eadbeef);
}

void
ags_midi_ci_util_test_put_initiate_protocol_negotiation()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_initiate_protocol_negotiation()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_initiate_protocol_negotiation_reply()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_initiate_protocol_negotiation_reply()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_set_protocol_type()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_set_protocol_type()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_confirm_protocol_type()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_confirm_protocol_type()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_confirm_protocol_type_reply()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_confirm_protocol_type_reply()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_confirm_protocol_type_established()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_confirm_protocol_type_established()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_profile()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x20\x2d\x7e\x10\x60\x00\xf7\x6f\x3e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  AgsMUID source = 0x0cafe010;
  AgsMUID destination = 0x0eadbeef;
  
  guchar device_id = 0x7f;
  guchar version = '\x01';
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));

  ags_midi_ci_util_put_profile(midi_ci_util,
			       buffer,
			       device_id,
			       version,
			       source,
			       destination);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_profile()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x20\x2d\x7e\x10\x60\x00\xf7\x6f\x3e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  AgsMUID source;
  AgsMUID destination;

  guchar device_id;
  guchar version;

  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_get_profile(midi_ci_util,
			       buffer,
			       &device_id,
			       &version,
			       &source,
			       &destination);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(destination == 0x0eadbeef);
}

void
ags_midi_ci_util_test_put_profile_reply()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x21\x2d\x7e\x10\x60\x00\x03\x6f\x3e\x01\x00\x00\x7e\x00\x00\x7e\x02\x00\x7e\x03\x01\x01\x09\x01\x00\x00\x00\x7e\x00\x00\xf7\x01\x01";

  static const guchar* const enabled_profile[5] = {
    "\x7e\x00\x00\x01\x02",
    "\x7e\x00\x00\x01\x03",
    "\x7e\x00\x00\x01\x09",
  };
  static const guchar* const disabled_profile[5] = {
    "\x7e\x00\x00\x01\x01",
  };
  
  AgsMUID source = 0x0cafe010;
  AgsMUID destination = 0x0eadbeef;

  guint16 enabled_profile_count = 3;
  guint16 disabled_profile_count = 1;
  guchar device_id = 0x7f;
  guchar version = '\x01';
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));

  ags_midi_ci_util_put_profile_reply(midi_ci_util,
				     buffer,
				     device_id,
				     version,
				     source,
				     destination,
				     enabled_profile_count,
				     enabled_profile,
				     disabled_profile_count,
				     disabled_profile);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 40 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_profile_reply()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x21\x2d\x7e\x10\x60\x00\x03\x6f\x3e\x01\x00\x00\x7e\x00\x00\x7e\x02\x00\x7e\x03\x01\x01\x09\x01\x00\x00\x00\x7e\x00\x00\xf7\x01\x01";
  
  guchar **enabled_profile;
  guchar **disabled_profile;

  static const guchar* const filled_enabled_profile[5] = {
    "\x7e\x00\x00\x01\x02",
    "\x7e\x00\x00\x01\x03",
    "\x7e\x00\x00\x01\x09",
  };
  static const guchar* const filled_disabled_profile[5] = {
    "\x7e\x00\x00\x01\x01",
  };

  AgsMUID source;
  AgsMUID destination;
  
  guchar device_id;
  guchar version;
  guint16 enabled_profile_count;
  guint16 disabled_profile_count;
  guint i;
  gboolean success;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  enabled_profile = NULL;
  disabled_profile = NULL;

  ags_midi_ci_util_get_profile_reply(midi_ci_util,
				     buffer,
				     &device_id,
				     &version,
				     &source,
				     &destination,
				     &enabled_profile_count,
				     &enabled_profile,
				     &disabled_profile_count,
				     &disabled_profile);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(destination == 0x0eadbeef);
  CU_ASSERT(enabled_profile_count == 3);
  CU_ASSERT(disabled_profile_count == 1);

  success = TRUE;

  for(i = 0; i < 3; i++){
    if((!memcmp(enabled_profile[i], filled_enabled_profile[i], 5 * sizeof(guchar))) == FALSE){
      success = FALSE;

      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
  
  success = TRUE;

  for(i = 0; i < 1; i++){
    if((!memcmp(disabled_profile[i], filled_disabled_profile[i], 5 * sizeof(guchar))) == FALSE){
      success = FALSE;

      break;
    }
  }
  
  CU_ASSERT(success == TRUE);
}

void
ags_midi_ci_util_test_put_profile_enabled_report()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x24\x7f\x7f\x10\x60\x00\x7e\x7f\x7f\x00\x01\x01\x00\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  const guchar add_profile[] = "\x7e\x00\x00\x01\x01";
  
  AgsMUID source = 0x0cafe010;

  guint16 enabled_channel_count = 0;
  guchar device_id = 0x7f;
  guchar version = '\x01';

  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));

  ags_midi_ci_util_put_profile_enabled_report(midi_ci_util,
					      buffer,
					      device_id,
					      version,
					      source,
					      add_profile,
					      enabled_channel_count);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 32 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_profile_enabled_report()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x24\x7f\x7f\x10\x60\x00\x7e\x7f\x7f\x00\x01\x01\x00\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  guchar enabled_profile[5];

  const guchar filled_enabled_profile[5] = "\x7e\x00\x00\x01\x01";

  AgsMUID source;
  
  guchar device_id;
  guchar version;
  guint16 enabled_channel_count;
  gboolean success;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(enabled_profile, 0, 5 * sizeof(guchar));

  ags_midi_ci_util_get_profile_enabled_report(midi_ci_util,
					      buffer,
					      &device_id,
					      &version,
					      &source,
					      enabled_profile,
					      &enabled_channel_count);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(enabled_channel_count == 0);

  success = TRUE;

  if((!memcmp(enabled_profile, filled_enabled_profile, 5 * sizeof(guchar))) == FALSE){
    success = FALSE;
  }
    
  CU_ASSERT(success == TRUE);
}

void
ags_midi_ci_util_test_put_profile_disabled_report()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x25\x7f\x7f\x10\x60\x00\x7e\x7f\x7f\x00\x01\x01\x00\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  const guchar enabled_profile[] = "\x7e\x00\x00\x01\x01";
  
  AgsMUID source = 0x0cafe010;

  guint16 enabled_channel_count = 0;
  guchar device_id = 0x7f;
  guchar version = '\x01';

  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));

  ags_midi_ci_util_put_profile_disabled_report(midi_ci_util,
					       buffer,
					       device_id,
					       version,
					       source,
					       enabled_profile,
					       enabled_channel_count);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 32 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_profile_disabled_report()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x25\x7f\x7f\x10\x60\x00\x7e\x7f\x7f\x00\x01\x01\x00\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  guchar enabled_profile[5];

  const guchar filled_enabled_profile[5] = "\x7e\x00\x00\x01\x01";

  AgsMUID source;
  
  guchar device_id;
  guchar version;
  guint16 enabled_channel_count;
  gboolean success;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(enabled_profile, 0, 5 * sizeof(guchar));

  ags_midi_ci_util_get_profile_disabled_report(midi_ci_util,
					       buffer,
					       &device_id,
					       &version,
					       &source,
					       enabled_profile,
					       &enabled_channel_count);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(enabled_channel_count == 0);

  success = TRUE;

  if((!memcmp(enabled_profile, filled_enabled_profile, 5 * sizeof(guchar))) == FALSE){
    success = FALSE;
  }
    
  CU_ASSERT(success == TRUE);
}

void
ags_midi_ci_util_test_put_profile_added()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x26\x7f\x7f\x10\x60\x00\x7e\x7f\x7f\xf7\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  const guchar add_profile[] = "\x7e\x00\x00\x01\x01";
  
  AgsMUID source = 0x0cafe010;

  guchar device_id = 0x7f;
  guchar version = '\x01';

  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));

  ags_midi_ci_util_put_profile_added(midi_ci_util,
				     buffer,
				     device_id,
				     version,
				     source,
				     add_profile);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 32 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_profile_added()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x26\x7f\x7f\x10\x60\x00\x7e\x7f\x7f\xf7\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  guchar add_profile[5];

  const guchar filled_add_profile[5] = "\x7e\x00\x00\x01\x01";

  AgsMUID source;
  
  guchar device_id;
  guchar version;
  gboolean success;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(add_profile, 0, 5 * sizeof(guchar));

  ags_midi_ci_util_get_profile_added(midi_ci_util,
				     buffer,
				     &device_id,
				     &version,
				     &source,
				     add_profile);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);

  success = TRUE;

  if((!memcmp(add_profile, filled_add_profile, 5 * sizeof(guchar))) == FALSE){
    success = FALSE;
  }
    
  CU_ASSERT(success == TRUE);
}

void
ags_midi_ci_util_test_put_profile_removed()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x27\x7f\x7f\x10\x60\x00\x7e\x7f\x7f\xf7\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  const guchar remove_profile[] = "\x7e\x00\x00\x01\x01";
  
  AgsMUID source = 0x0cafe010;

  guchar device_id = 0x7f;
  guchar version = '\x01';

  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));

  ags_midi_ci_util_put_profile_removed(midi_ci_util,
				       buffer,
				       device_id,
				       version,
				       source,
				       remove_profile);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 20 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_profile_removed()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x27\x7f\x7f\x10\x60\x00\x7e\x7f\x7f\xf7\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  guchar remove_profile[5];

  const guchar filled_remove_profile[5] = "\x7e\x00\x00\x01\x01";

  AgsMUID source;
  
  guchar device_id;
  guchar version;
  gboolean success;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(remove_profile, 0, 5 * sizeof(guchar));

  ags_midi_ci_util_get_profile_removed(midi_ci_util,
				       buffer,
				       &device_id,
				       &version,
				       &source,
				       remove_profile);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);

  success = TRUE;

  if((!memcmp(remove_profile, filled_remove_profile, 5 * sizeof(guchar))) == FALSE){
    success = FALSE;
  }
    
  CU_ASSERT(success == TRUE);
}

void
ags_midi_ci_util_test_put_profile_specific_data()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x2f\x7f\x7f\x10\x60\x00\x7e\x7f\x7f\x00\x01\x01\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  guchar *profile_specific_data;
  
  const guchar profile_id[] = "\x7e\x00\x00\x01\x01";
  
  AgsMUID source = 0x0cafe010;
  AgsMUID destination = AGS_MIDI_CI_UTIL_BROADCAST_MUID;

  guchar device_id = 0x7f;
  guchar version = '\x01';
  guint32 profile_specific_data_length;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));

  profile_specific_data_length = 0;
  profile_specific_data = NULL;
  
  ags_midi_ci_util_put_profile_specific_data(midi_ci_util,
					     buffer,
					     device_id,
					     version,
					     source,
					     destination,
					     profile_id,
					     profile_specific_data_length,
					     profile_specific_data);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 24 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_profile_specific_data()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x2f\x7f\x7f\x10\x60\x00\x7e\x7f\x7f\x00\x01\x01\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  guchar profile_id[5];
  guchar *profile_specific_data;

  const guchar filled_profile_id[5] = "\x7e\x00\x00\x01\x01";

  AgsMUID source;
  AgsMUID destination;
  
  guchar device_id;
  guchar version;
  guint32 profile_specific_data_length;
  gboolean success;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(profile_id, 0, 5 * sizeof(guchar));

  ags_midi_ci_util_get_profile_specific_data(midi_ci_util,
					     buffer,
					     &device_id,
					     &version,
					     &source,
					     &destination,
					     profile_id,
					     &profile_specific_data_length,
					     &profile_specific_data);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(destination == AGS_MIDI_CI_UTIL_BROADCAST_MUID);
  CU_ASSERT(profile_specific_data_length == 0);
  CU_ASSERT(profile_specific_data == NULL);

  success = TRUE;

  if((!memcmp(profile_id, filled_profile_id, 5 * sizeof(guchar))) == FALSE){
    success = FALSE;
  }
    
  CU_ASSERT(success == TRUE);
}

void
ags_midi_ci_util_test_put_property_exchange_capabilities()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x30\x7f\x7f\x10\x60\x01\x01\x7f\x7f\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  
  AgsMUID source = 0x0cafe010;
  AgsMUID destination = AGS_MIDI_CI_UTIL_BROADCAST_MUID;

  guchar device_id = 0x7f;
  guchar supported_property_exchange_count = '\x01';
  guchar major = '\x01';
  guchar minor = '\x00';
  guchar version = '\x01';
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ci_util_put_property_exchange_capabilities(midi_ci_util,
						      buffer,
						      device_id,
						      version,
						      source,
						      destination,
						      supported_property_exchange_count,
						      major,
						      minor);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 24 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_property_exchange_capabilities()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x30\x7f\x7f\x10\x60\x01\x01\x7f\x7f\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  AgsMUID source;
  AgsMUID destination;
  
  guchar device_id;
  guchar version;
  guchar supported_property_exchange_count;
  guchar major;
  guchar minor;
  gboolean success;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_get_property_exchange_capabilities(midi_ci_util,
						      buffer,
						      &device_id,
						      &version,
						      &source,
						      &destination,
						      &supported_property_exchange_count,
						      &major,
						      &minor);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(destination == AGS_MIDI_CI_UTIL_BROADCAST_MUID);
  CU_ASSERT(supported_property_exchange_count == 0x01);
  CU_ASSERT(major == 0x01);
  CU_ASSERT(minor == 0x00);
}

void
ags_midi_ci_util_test_put_property_exchange_capabilities_reply()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x31\x7f\x7f\x10\x60\x01\x01\x7f\x7f\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  
  AgsMUID source = 0x0cafe010;
  AgsMUID destination = AGS_MIDI_CI_UTIL_BROADCAST_MUID;

  guchar device_id = 0x7f;
  guchar supported_property_exchange_count = '\x01';
  guchar major = '\x01';
  guchar minor = '\x00';
  guchar version = '\x01';
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ci_util_put_property_exchange_capabilities_reply(midi_ci_util,
							    buffer,
							    device_id,
							    version,
							    source,
							    destination,
							    supported_property_exchange_count,
							    major,
							    minor);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 24 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_property_exchange_capabilities_reply()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x31\x7f\x7f\x10\x60\x01\x01\x7f\x7f\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  AgsMUID source;
  AgsMUID destination;
  
  guchar device_id;
  guchar version;
  guchar supported_property_exchange_count;
  guchar major;
  guchar minor;
  gboolean success;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_get_property_exchange_capabilities_reply(midi_ci_util,
							    buffer,
							    &device_id,
							    &version,
							    &source,
							    &destination,
							    &supported_property_exchange_count,
							    &major,
							    &minor);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(destination == AGS_MIDI_CI_UTIL_BROADCAST_MUID);
  CU_ASSERT(supported_property_exchange_count == 0x01);
  CU_ASSERT(major == 0x01);
  CU_ASSERT(minor == 0x00);
}

void
ags_midi_ci_util_test_put_get_property_data()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x34\x2d\x7e\x10\x60\x00\x01\x6f\x3e\x01\x00\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  guchar *header_data = NULL;
  guchar *property_data = NULL;
  
  AgsMUID source = 0x0cafe010;
  AgsMUID destination = 0x0eadbeef;

  guchar device_id = '\x7f';
  guchar version = '\x01';
  guchar request_id = '\x01';
  guint16 header_data_length = 0;
  guint16 property_data_length = 0;  
  guint16 chunk_count = 0;
  guint16 nth_chunk = 1;  
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ci_util_put_get_property_data(midi_ci_util,
					 buffer,
					 device_id,
					 version,
					 source,
					 destination,
					 request_id,
					 header_data_length,
					 header_data,
					 chunk_count,
					 nth_chunk,
					 property_data_length,
					 property_data);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 24 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_get_property_data()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x34\x2d\x7e\x10\x60\x00\x01\x6f\x3e\x01\x00\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  guint16 header_data_length;  
  guchar *header_data;
  guint16 property_data_length;  
  guchar *property_data;

  AgsMUID source;
  AgsMUID destination;
  
  guchar device_id;
  guchar version;
  guchar request_id;
  guint16 chunk_count;
  guint16 nth_chunk;  
  gboolean success;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_get_get_property_data(midi_ci_util,
					 buffer,
					 &device_id,
					 &version,
					 &source,
					 &destination,
					 &request_id,
					 &header_data_length,
					 &header_data,
					 &chunk_count,
					 &nth_chunk,
					 &property_data_length,
					 &property_data);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(destination == 0x0eadbeef);
  CU_ASSERT(request_id == 0x01);
  CU_ASSERT(header_data_length == 0x00);
  CU_ASSERT(header_data == NULL);
  CU_ASSERT(chunk_count == 0x00);
  CU_ASSERT(nth_chunk == 0x01);
  CU_ASSERT(property_data_length == 0x00);
  CU_ASSERT(property_data == NULL);  
}

void
ags_midi_ci_util_test_put_get_property_data_reply()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x35\x2d\x7e\x10\x60\x00\x01\x6f\x3e\x01\x00\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  guchar *header_data = NULL;
  guchar *property_data = NULL;
  
  AgsMUID source = 0x0cafe010;
  AgsMUID destination = 0x0eadbeef;

  guchar device_id = '\x7f';
  guchar version = '\x01';
  guchar request_id = '\x01';
  guint16 header_data_length = 0;
  guint16 property_data_length = 0;  
  guint16 chunk_count = 0;
  guint16 nth_chunk = 1;  
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ci_util_put_get_property_data_reply(midi_ci_util,
					       buffer,
					       device_id,
					       version,
					       source,
					       destination,
					       request_id,
					       header_data_length,
					       header_data,
					       chunk_count,
					       nth_chunk,
					       property_data_length,
					       property_data);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 24 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_get_property_data_reply()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x35\x2d\x7e\x10\x60\x00\x01\x6f\x3e\x01\x00\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  guint16 header_data_length;  
  guchar *header_data;
  guint16 property_data_length;  
  guchar *property_data;

  AgsMUID source;
  AgsMUID destination;
  
  guchar device_id;
  guchar version;
  guchar request_id;
  guint16 chunk_count;
  guint16 nth_chunk;  
  gboolean success;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_get_get_property_data_reply(midi_ci_util,
					       buffer,
					       &device_id,
					       &version,
					       &source,
					       &destination,
					       &request_id,
					       &header_data_length,
					       &header_data,
					       &chunk_count,
					       &nth_chunk,
					       &property_data_length,
					       &property_data);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(destination == 0x0eadbeef);
  CU_ASSERT(request_id == 0x01);
  CU_ASSERT(header_data_length == 0x00);
  CU_ASSERT(header_data == NULL);
  CU_ASSERT(chunk_count == 0x00);
  CU_ASSERT(nth_chunk == 0x01);
  CU_ASSERT(property_data_length == 0x00);
  CU_ASSERT(property_data == NULL);  
}

void
ags_midi_ci_util_test_put_set_property_data()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x36\x2d\x7e\x10\x60\x00\x01\x6f\x3e\x01\x00\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  guchar *header_data = NULL;
  guchar *property_data = NULL;
  
  AgsMUID source = 0x0cafe010;
  AgsMUID destination = 0x0eadbeef;

  guchar device_id = '\x7f';
  guchar version = '\x01';
  guchar request_id = '\x01';
  guint16 header_data_length = 0;
  guint16 property_data_length = 0;  
  guint16 chunk_count = 0;
  guint16 nth_chunk = 1;  
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ci_util_put_set_property_data(midi_ci_util,
					 buffer,
					 device_id,
					 version,
					 source,
					 destination,
					 request_id,
					 header_data_length,
					 header_data,
					 chunk_count,
					 nth_chunk,
					 property_data_length,
					 property_data);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 24 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_set_property_data()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x36\x2d\x7e\x10\x60\x00\x01\x6f\x3e\x01\x00\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  guint16 header_data_length;  
  guchar *header_data;
  guint16 property_data_length;  
  guchar *property_data;

  AgsMUID source;
  AgsMUID destination;
  
  guchar device_id;
  guchar version;
  guchar request_id;
  guint16 chunk_count;
  guint16 nth_chunk;  
  gboolean success;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_get_set_property_data(midi_ci_util,
					 buffer,
					 &device_id,
					 &version,
					 &source,
					 &destination,
					 &request_id,
					 &header_data_length,
					 &header_data,
					 &chunk_count,
					 &nth_chunk,
					 &property_data_length,
					 &property_data);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(destination == 0x0eadbeef);
  CU_ASSERT(request_id == 0x01);
  CU_ASSERT(header_data_length == 0x00);
  CU_ASSERT(header_data == NULL);
  CU_ASSERT(chunk_count == 0x00);
  CU_ASSERT(nth_chunk == 0x01);
  CU_ASSERT(property_data_length == 0x00);
  CU_ASSERT(property_data == NULL);  
}

void
ags_midi_ci_util_test_put_set_property_data_reply()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x37\x2d\x7e\x10\x60\x00\x01\x6f\x3e\x01\x00\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  guchar *header_data = NULL;
  guchar *property_data = NULL;
  
  AgsMUID source = 0x0cafe010;
  AgsMUID destination = 0x0eadbeef;

  guchar device_id = '\x7f';
  guchar version = '\x01';
  guchar request_id = '\x01';
  guint16 header_data_length = 0;
  guint16 property_data_length = 0;  
  guint16 chunk_count = 0;
  guint16 nth_chunk = 1;  
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ci_util_put_set_property_data_reply(midi_ci_util,
					       buffer,
					       device_id,
					       version,
					       source,
					       destination,
					       request_id,
					       header_data_length,
					       header_data,
					       chunk_count,
					       nth_chunk,
					       property_data_length,
					       property_data);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 24 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_set_property_data_reply()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x37\x2d\x7e\x10\x60\x00\x01\x6f\x3e\x01\x00\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  guint16 header_data_length;  
  guchar *header_data;
  guint16 property_data_length;  
  guchar *property_data;

  AgsMUID source;
  AgsMUID destination;
  
  guchar device_id;
  guchar version;
  guchar request_id;
  guint16 chunk_count;
  guint16 nth_chunk;  
  gboolean success;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_get_set_property_data_reply(midi_ci_util,
					       buffer,
					       &device_id,
					       &version,
					       &source,
					       &destination,
					       &request_id,
					       &header_data_length,
					       &header_data,
					       &chunk_count,
					       &nth_chunk,
					       &property_data_length,
					       &property_data);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(destination == 0x0eadbeef);
  CU_ASSERT(request_id == 0x01);
  CU_ASSERT(header_data_length == 0x00);
  CU_ASSERT(header_data == NULL);
  CU_ASSERT(chunk_count == 0x00);
  CU_ASSERT(nth_chunk == 0x01);
  CU_ASSERT(property_data_length == 0x00);
  CU_ASSERT(property_data == NULL);  
}

void
ags_midi_ci_util_test_put_subscription()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x38\x2d\x7e\x10\x60\x00\x01\x6f\x3e\x01\x00\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  guchar *header_data = NULL;
  guchar *property_data = NULL;
  
  AgsMUID source = 0x0cafe010;
  AgsMUID destination = 0x0eadbeef;

  guchar device_id = '\x7f';
  guchar version = '\x01';
  guchar request_id = '\x01';
  guint16 header_data_length = 0;
  guint16 property_data_length = 0;  
  guint16 chunk_count = 0;
  guint16 nth_chunk = 1;  
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ci_util_put_subscription(midi_ci_util,
				    buffer,
				    device_id,
				    version,
				    source,
				    destination,
				    request_id,
				    header_data_length,
				    header_data,
				    chunk_count,
				    nth_chunk,
				    property_data_length,
				    property_data);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 24 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_subscription()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x38\x2d\x7e\x10\x60\x00\x01\x6f\x3e\x01\x00\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  guint16 header_data_length;  
  guchar *header_data;
  guint16 property_data_length;  
  guchar *property_data;

  AgsMUID source;
  AgsMUID destination;
  
  guchar device_id;
  guchar version;
  guchar request_id;
  guint16 chunk_count;
  guint16 nth_chunk;  
  gboolean success;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_get_subscription(midi_ci_util,
				    buffer,
				    &device_id,
				    &version,
				    &source,
				    &destination,
				    &request_id,
				    &header_data_length,
				    &header_data,
				    &chunk_count,
				    &nth_chunk,
				    &property_data_length,
				    &property_data);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(destination == 0x0eadbeef);
  CU_ASSERT(request_id == 0x01);
  CU_ASSERT(header_data_length == 0x00);
  CU_ASSERT(header_data == NULL);
  CU_ASSERT(chunk_count == 0x00);
  CU_ASSERT(nth_chunk == 0x01);
  CU_ASSERT(property_data_length == 0x00);
  CU_ASSERT(property_data == NULL);  
}

void
ags_midi_ci_util_test_put_subscription_reply()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x39\x2d\x7e\x10\x60\x00\x01\x6f\x3e\x01\x00\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  guchar *header_data = NULL;
  guchar *property_data = NULL;
  
  AgsMUID source = 0x0cafe010;
  AgsMUID destination = 0x0eadbeef;

  guchar device_id = '\x7f';
  guchar version = '\x01';
  guchar request_id = '\x01';
  guint16 header_data_length = 0;
  guint16 property_data_length = 0;  
  guint16 chunk_count = 0;
  guint16 nth_chunk = 0x1;  
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));
  
  ags_midi_ci_util_put_subscription_reply(midi_ci_util,
					  buffer,
					  device_id,
					  version,
					  source,
					  destination,
					  request_id,
					  header_data_length,
					  header_data,
					  chunk_count,
					  nth_chunk,
					  property_data_length,
					  property_data);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 24 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_subscription_reply()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x39\x2d\x7e\x10\x60\x00\x01\x6f\x3e\x01\x00\x00\x00\xf7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
  guint16 header_data_length;  
  guchar *header_data;
  guint16 property_data_length;  
  guchar *property_data;

  AgsMUID source;
  AgsMUID destination;
  
  guchar device_id;
  guchar version;
  guchar request_id;
  guint16 chunk_count;
  guint16 nth_chunk;  
  gboolean success;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_get_subscription_reply(midi_ci_util,
					  buffer,
					  &device_id,
					  &version,
					  &source,
					  &destination,
					  &request_id,
					  &header_data_length,
					  &header_data,
					  &chunk_count,
					  &nth_chunk,
					  &property_data_length,
					  &property_data);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(destination == 0x0eadbeef);
  CU_ASSERT(request_id == 0x01);
  CU_ASSERT(header_data_length == 0x00);
  CU_ASSERT(header_data == NULL);
  CU_ASSERT(chunk_count == 0x00);
  CU_ASSERT(nth_chunk == 0x01);
  CU_ASSERT(property_data_length == 0x00);
  CU_ASSERT(property_data == NULL);  
}

void
ags_midi_ci_util_test_put_process_capabilities()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x40\x2d\x7e\x10\x60\x00\xf7\x6f\x3e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  AgsMUID source = 0x0cafe010;
  AgsMUID destination = 0x0eadbeef;

  guchar device_id = '\x7f';
  guchar version = '\x01';
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));

  ags_midi_ci_util_put_process_capabilities(midi_ci_util,
					    buffer,
					    device_id,
					    version,
					    source,
					    destination);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 16 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_process_capabilities()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x40\x2d\x7e\x10\x60\x00\xf7\x6f\x3e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  AgsMUID source;
  AgsMUID destination;
  
  guchar device_id;
  guchar version;

  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_get_process_capabilities(midi_ci_util,
					    buffer,
					    &device_id,
					    &version,
					    &source,
					    &destination);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(destination == 0x0eadbeef);
}

void
ags_midi_ci_util_test_put_process_capabilities_reply()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  static const guchar filled_buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x41\x2d\x7e\x10\x60\x00\x00\x6f\x3e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  AgsMUID source = 0x0cafe010;
  AgsMUID destination = 0x0eadbeef;

  guchar device_id = '\x7f';
  guchar version = '\x01';
  guchar supported_features = '\x00';
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));

  ags_midi_ci_util_put_process_capabilities_reply(midi_ci_util,
						  buffer,
						  device_id,
						  version,
						  source,
						  destination,
						  supported_features);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 15 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_process_capabilities_reply()
{
  AgsMidiCIUtil *midi_ci_util;

  static const guchar buffer[] = "\x0d\x7f\x7e\xf0\x2f\x6c\x01\x41\x2d\x7e\x10\x60\x00\x00\x6f\x3e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  AgsMUID source;
  AgsMUID destination;
  
  guchar device_id;
  guchar version;
  guchar supported_features;

  midi_ci_util = ags_midi_ci_util_alloc();
  
  ags_midi_ci_util_get_process_capabilities_reply(midi_ci_util,
						  buffer,
						  &device_id,
						  &version,
						  &source,
						  &destination,
						  &supported_features);

  CU_ASSERT(device_id == 0x7f);
  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(destination == 0x0eadbeef);
  CU_ASSERT(supported_features == 0x00);
}

void
ags_midi_ci_util_test_put_message_report()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_message_report()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_message_report_reply()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_message_report_reply()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_end_of_message_report()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_end_of_message_report()
{
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
  pSuite = CU_add_suite("AgsMidiCIUtilTest", ags_midi_ci_util_test_init_suite, ags_midi_ci_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_midi_ci_util.c alloc", ags_midi_ci_util_test_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c free", ags_midi_ci_util_test_free) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c copy", ags_midi_ci_util_test_copy) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c generate MUID", ags_midi_ci_util_test_generate_muid) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put discovery", ags_midi_ci_util_test_put_discovery) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get discovery", ags_midi_ci_util_test_get_discovery) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put discovery reply", ags_midi_ci_util_test_put_discovery_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get discovery reply", ags_midi_ci_util_test_get_discovery_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put invalidate MUID", ags_midi_ci_util_test_put_invalidate_muid) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get invalidate MUID", ags_midi_ci_util_test_get_invalidate_muid) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put ACK", ags_midi_ci_util_test_put_ack) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get ACK", ags_midi_ci_util_test_get_ack) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put NAK", ags_midi_ci_util_test_put_nak) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get NAK", ags_midi_ci_util_test_get_nak) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put initiate protocol negotiation", ags_midi_ci_util_test_put_initiate_protocol_negotiation) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get initiate protocol negotiation", ags_midi_ci_util_test_get_initiate_protocol_negotiation) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put initiate protocol negotiation reply", ags_midi_ci_util_test_put_initiate_protocol_negotiation_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get initiate protocol negotiation reply", ags_midi_ci_util_test_get_initiate_protocol_negotiation_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put set protocol type", ags_midi_ci_util_test_put_set_protocol_type) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get set protocol type", ags_midi_ci_util_test_get_set_protocol_type) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put confirm protocol type", ags_midi_ci_util_test_put_confirm_protocol_type) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get confirm protocol type", ags_midi_ci_util_test_get_confirm_protocol_type) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put confirm protocol type reply", ags_midi_ci_util_test_put_confirm_protocol_type_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get confirm protocol type reply", ags_midi_ci_util_test_get_confirm_protocol_type_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put profile inquiry", ags_midi_ci_util_test_put_profile) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get profile inquiry", ags_midi_ci_util_test_get_profile) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put profile inquiry reply", ags_midi_ci_util_test_put_profile_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get profile inquiry reply", ags_midi_ci_util_test_get_profile_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put profile enabled report", ags_midi_ci_util_test_put_profile_enabled_report) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get profile enabled report", ags_midi_ci_util_test_get_profile_enabled_report) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put profile disabled report", ags_midi_ci_util_test_put_profile_disabled_report) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get profile disabled report", ags_midi_ci_util_test_get_profile_disabled_report) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put profile added", ags_midi_ci_util_test_put_profile_added) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get profile added", ags_midi_ci_util_test_get_profile_added) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put profile removed", ags_midi_ci_util_test_put_profile_removed) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get profile removed", ags_midi_ci_util_test_get_profile_removed) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put profile specific data", ags_midi_ci_util_test_put_profile_specific_data) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get profile specific data", ags_midi_ci_util_test_get_profile_specific_data) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put property exchange capabilities", ags_midi_ci_util_test_put_property_exchange_capabilities) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get property exchange capabilities", ags_midi_ci_util_test_get_property_exchange_capabilities) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put property exchange capabilities reply", ags_midi_ci_util_test_put_property_exchange_capabilities_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get property exchange capabilities reply", ags_midi_ci_util_test_get_property_exchange_capabilities_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put get property data", ags_midi_ci_util_test_put_get_property_data) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get get property data", ags_midi_ci_util_test_get_get_property_data) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put get property data reply", ags_midi_ci_util_test_put_get_property_data_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get get property data reply", ags_midi_ci_util_test_get_get_property_data_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put set property data", ags_midi_ci_util_test_put_set_property_data) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get set property data", ags_midi_ci_util_test_get_set_property_data) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put set property data reply", ags_midi_ci_util_test_put_set_property_data_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get set property data reply", ags_midi_ci_util_test_get_set_property_data_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put subscription", ags_midi_ci_util_test_put_subscription) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get subscription", ags_midi_ci_util_test_get_subscription) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put subscription reply", ags_midi_ci_util_test_put_subscription_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get subscription reply", ags_midi_ci_util_test_get_subscription_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put process capabilities", ags_midi_ci_util_test_put_process_capabilities) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get process capabilities", ags_midi_ci_util_test_get_process_capabilities) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put process capabilities reply", ags_midi_ci_util_test_put_process_capabilities_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get process capabilities reply", ags_midi_ci_util_test_get_process_capabilities_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put message report", ags_midi_ci_util_test_put_message_report) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get message report", ags_midi_ci_util_test_get_message_report) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put message report reply", ags_midi_ci_util_test_put_message_report_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get message report reply", ags_midi_ci_util_test_get_message_report_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put end of message report", ags_midi_ci_util_test_put_end_of_message_report) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get end of message report", ags_midi_ci_util_test_get_end_of_message_report) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
