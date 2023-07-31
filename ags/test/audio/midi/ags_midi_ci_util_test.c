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
  guchar filled_buffer[] = "\xf0\x7e\x7f\x0d\x70\x01\x7e\x2d\x3e\x6f\x7f\x7f\x7f\x7f\x08\x00\x00\xfe\xaf\x52\x0a\x00\x05\x00\x05\x05\x00\x02\x00\x00\xf7";

  AgsMUID source = 0x0eadbeef;
  
  guchar version = '\x01';
  guchar manufacturer_id[] = "\x08\x00\x00";
  guint16 device_family = 0xaffe;
  guint16 device_family_model_number = 0x0a52;  
  guchar software_revision_level[] = "\x00\x05\x00\x05";
  guchar capability = '\x05';
  guint32 max_sysex_message_size = 512;
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));

  ags_midi_ci_util_put_discovery(midi_ci_util,
				 buffer,
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

  guchar buffer[] = "\xf0\x7e\x7f\x0d\x70\x01\x7e\x2d\x3e\x6f\x7f\x7f\x7f\x7f\x08\x00\x00\xfe\xaf\x52\x0a\x00\x05\x00\x05\x05\x00\x02\x00\x00\xf7";

  AgsMUID source;
  
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
				 &version,
				 &source,
				 &manufacturer_id,
				 &device_family,
				 &device_family_model_number,
				 software_revision_level,
				 &capability,
				 &max_sysex_message_size);

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
  guchar filled_buffer[] = "\xf0\x7e\x7f\x0d\x71\x01\x6c\x2f\x60\x10\x7e\x2d\x3e\x6f\x08\x00\x00\xfe\xaf\x52\x0a\x00\x05\x00\x05\x05\x00\x02\x00\x00\xf7";

  AgsMUID source = 0x0cafe010;
  AgsMUID destination = 0x0eadbeef;
  
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

  guchar buffer[] = "\xf0\x7e\x7f\x0d\x71\x01\x6c\x2f\x60\x10\x7e\x2d\x3e\x6f\x08\x00\x00\xfe\xaf\x52\x0a\x00\x05\x00\x05\x05\x00\x02\x00\x00\xf7";

  AgsMUID source;
  AgsMUID destination;
  
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
				       &version,
				       &source,
				       &destination,
				       &manufacturer_id,
				       &device_family,
				       &device_family_model_number,
				       software_revision_level,
				       &capability,
				       &max_sysex_message_size);

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
  guchar filled_buffer[] = "\xf0\x7e\x7f\x0d\x7e\x01\x6c\x2f\x60\x10\x7f\x7f\x7f\x7f\x7e\x2d\x3e\x6f\xf7";

  AgsMUID source = 0x0cafe010;
  AgsMUID target_muid = 0x0eadbeef;
  
  guchar version = '\x01';
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));

  ags_midi_ci_util_put_invalidate_muid(midi_ci_util,
				       buffer,
				       version,
				       source,
				       target_muid);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 19 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_invalidate_muid()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[] = "\xf0\x7e\x7f\x0d\x7e\x01\x6c\x2f\x60\x10\x7f\x7f\x7f\x7f\x7e\x2d\x3e\x6f\xf7";

  AgsMUID source;
  AgsMUID target_muid;
  
  guchar version;

  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_get_invalidate_muid(midi_ci_util,
				       buffer,
				       &version,
				       &source,
				       &target_muid);

  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(target_muid == 0x0eadbeef);
}

void
ags_midi_ci_util_test_put_nak()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  guchar filled_buffer[] = "\xf0\x7e\x7f\x0d\x7f\x01\x6c\x2f\x60\x10\x7e\x2d\x3e\x6f\xf7";

  AgsMUID source = 0x0cafe010;
  AgsMUID destination = 0x0eadbeef;
  
  guchar version = '\x01';
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));

  ags_midi_ci_util_put_nak(midi_ci_util,
			   buffer,
			   version,
			   source,
			   destination);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 15 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_nak()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[] = "\xf0\x7e\x7f\x0d\x7f\x01\x6c\x2f\x60\x10\x7e\x2d\x3e\x6f\xf7";

  AgsMUID source;
  AgsMUID destination;
  
  guchar version;

  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_get_nak(midi_ci_util,
			   buffer,
			   &version,
			   &source,
			   &destination);

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
  guchar filled_buffer[] = "\xf0\x7e\x7f\x0d\x20\x01\x6c\x2f\x60\x10\x7e\x2d\x3e\x6f\xf7";

  AgsMUID source = 0x0cafe010;
  AgsMUID destination = 0x0eadbeef;
  
  guchar version = '\x01';
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));

  ags_midi_ci_util_put_profile(midi_ci_util,
			       buffer,
			       version,
			       source,
			       destination);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 15 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_profile()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[] = "\xf0\x7e\x7f\x0d\x20\x01\x6c\x2f\x60\x10\x7e\x2d\x3e\x6f\xf7";

  AgsMUID source;
  AgsMUID destination;
  
  guchar version;

  midi_ci_util = ags_midi_ci_util_alloc();

  ags_midi_ci_util_get_profile(midi_ci_util,
			       buffer,
			       &version,
			       &source,
			       &destination);

  CU_ASSERT(version == 0x01);
  CU_ASSERT(source == 0x0cafe010);
  CU_ASSERT(destination == 0x0eadbeef);
}

void
ags_midi_ci_util_test_put_profile_reply()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[512];
  guchar filled_buffer[] = "\xf0\x7e\x7f\x0d\x21\x01\x6c\x2f\x60\x10\x7e\x2d\x3e\x6f\x03\x00\x7e\x00\x00\x01\x02\x7e\x00\x00\x01\x03\x7e\x00\x00\x01\x09\x01\x00\x7e\x00\x00\x01\x01\xf7";

  const guchar* enabled_profile[5] = {
    "\x7e\x00\x00\x01\x02",
    "\x7e\x00\x00\x01\x03",
    "\x7e\x00\x00\x01\x09",
  };
  const guchar* disabled_profile[5] = {
    "\x7e\x00\x00\x01\x01",
  };
  
  AgsMUID source = 0x0cafe010;
  AgsMUID destination = 0x0eadbeef;

  guint16 enabled_profile_count = 3;
  guint16 disabled_profile_count = 1;
  guchar version = '\x01';
  
  midi_ci_util = ags_midi_ci_util_alloc();

  memset(buffer, 0, 512 * sizeof(guchar));

  ags_midi_ci_util_put_profile_reply(midi_ci_util,
				     buffer,
				     version,
				     source,
				     destination,
				     enabled_profile_count,
				     enabled_profile,
				     disabled_profile_count,
				     disabled_profile);

  CU_ASSERT(!memcmp(buffer, filled_buffer, 39 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_profile_reply()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[] = "\xf0\x7e\x7f\x0d\x21\x01\x6c\x2f\x60\x10\x7e\x2d\x3e\x6f\x03\x00\x7e\x00\x00\x01\x02\x7e\x00\x00\x01\x03\x7e\x00\x00\x01\x09\x01\x00\x7e\x00\x00\x01\x01\xf7";
  guchar **enabled_profile;
  guchar **disabled_profile;

  const guchar* filled_enabled_profile[5] = {
    "\x7e\x00\x00\x01\x02",
    "\x7e\x00\x00\x01\x03",
    "\x7e\x00\x00\x01\x09",
  };
  const guchar* filled_disabled_profile[5] = {
    "\x7e\x00\x00\x01\x01",
  };

  AgsMUID source;
  AgsMUID destination;
  
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
				     &version,
				     &source,
				     &destination,
				     &enabled_profile_count,
				     &enabled_profile,
				     &disabled_profile_count,
				     &disabled_profile);

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
  guchar filled_buffer[] = "\xf0\x7e\x7f\x0d\x24\x01\x6c\x2f\x60\x10\x7f\x7f\x7f\x7f\x7e\x00\x00\x01\x01\x00\x00\xf7";

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

  CU_ASSERT(!memcmp(buffer, filled_buffer, 22 * sizeof(guchar)));
}

void
ags_midi_ci_util_test_get_profile_enabled_report()
{
  AgsMidiCIUtil *midi_ci_util;

  guchar buffer[] = "\xf0\x7e\x7f\x0d\x24\x01\x6c\x2f\x60\x10\x7f\x7f\x7f\x7f\x7e\x00\x00\x01\x01\x00\x00\xf7";
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
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_profile_disabled_report()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_profile_added()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_profile_added()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_profile_removed()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_profile_removed()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_profile_specific_data()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_profile_specific_data()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_property_exchange_capabilities()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_property_exchange_capabilities()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_property_exchange_capabilities_reply()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_property_exchange_capabilities_reply()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_get_property_data()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_get_property_data()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_get_property_data_reply()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_get_property_data_reply()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_set_property_data()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_set_property_data()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_set_property_data_reply()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_set_property_data_reply()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_subscription()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_subscription()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_put_subscription_reply()
{
  //TODO:JK: implement me
}

void
ags_midi_ci_util_test_get_subscription_reply()
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
  pSuite = CU_add_suite("AgsMidiCI11UtilTest", ags_midi_ci_util_test_init_suite, ags_midi_ci_util_test_clean_suite);
  
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
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put set property data", ags_midi_ci_util_test_put_set_property_data) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get set property data", ags_midi_ci_util_test_get_set_property_data) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put subscription", ags_midi_ci_util_test_put_subscription) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get subscription", ags_midi_ci_util_test_get_subscription) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c put subscription reply", ags_midi_ci_util_test_put_subscription_reply) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_util.c get subscription reply", ags_midi_ci_util_test_get_subscription_reply) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
