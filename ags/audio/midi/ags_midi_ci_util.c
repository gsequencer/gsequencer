/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/audio/midi/ags_midi_ci_util.h>

#include <glib.h>
#include <glib-object.h>

#include <json-glib/json-glib.h>

#include <string.h>

/**
 * SECTION:ags_midi_ci_util
 * @short_description: MIDI CI util
 * @title: AgsMidiCiUtil
 * @section_id:
 * @include: ags/audio/midi/ags_midi_ci_util.h
 *
 * Utility functions for MIDI CI.
 */

GType
ags_midi_ci_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_midi_ci_util = 0;

    ags_type_midi_ci_util =
      g_boxed_type_register_static("AgsMidiCIUtil",
				   (GBoxedCopyFunc) ags_midi_ci_util_copy,
				   (GBoxedFreeFunc) ags_midi_ci_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_midi_ci_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_midi_ci_util_alloc:
 *
 * Allocate MIDI CI util.
 *
 * Returns: (transfer full): the newly allocated #AgsMidiCIUtil-struct
 * 
 * Since: 5.5.0
 */
AgsMidiCIUtil*
ags_midi_ci_util_alloc()
{
  AgsMidiCIUtil *midi_ci_util;

  midi_ci_util = g_new0(AgsMidiCIUtil,
			1);

  midi_ci_util->rand = g_rand_new();

  midi_ci_util->major = 1;
  midi_ci_util->minor = 2;
  
  return(midi_ci_util);
}

/**
 * ags_midi_ci_util_free:
 * @midi_ci_util: the MIDI CI util
 *
 * Free MIDI CI util.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_free(AgsMidiCIUtil *midi_ci_util)
{
  g_return_if_fail(midi_ci_util != NULL);
  
  g_free(midi_ci_util);
}

/**
 * ags_midi_ci_util_copy:
 * @midi_ci_util: the MIDI CI util
 *
 * Copy MIDI CI util.
 *
 * Returns: (transfer full): the newly allocated #AgsMidiCIUtil-struct
 * 
 * Since: 5.5.0
 */
AgsMidiCIUtil*
ags_midi_ci_util_copy(AgsMidiCIUtil *midi_ci_util)
{
  AgsMidiCIUtil *ptr;

  g_return_val_if_fail(midi_ci_util != NULL, NULL);

  ptr = ags_midi_ci_util_alloc();

  ptr->major = midi_ci_util->major;
  ptr->minor = midi_ci_util->minor;
  
  return(ptr);
}

/**
 * ags_midi_ci_util_generate_muid:
 * @midi_ci_util: the MIDI CI util
 *
 * Generate MUID.
 *
 * Returns: the generated MUID
 * 
 * Since: 5.5.0
 */
AgsMUID
ags_midi_ci_util_generate_muid(AgsMidiCIUtil *midi_ci_util)
{
  AgsMUID muid;

  g_return_val_if_fail(midi_ci_util != NULL, 0);

  muid = g_rand_int_range(midi_ci_util->rand,
			  0,
			  exp2(28));

  return(muid);
}

/**
 * ags_midi_ci_util_put_muid:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @muid: the #AgsMUID
 *
 * Put MUID.
 * 
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_muid(AgsMidiCIUtil *midi_ci_util,
			  guchar *buffer,
			  AgsMUID muid)
{
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  buffer[0] = ((0x0f000000 & muid) >> 24) | ((0x800000 & muid) >> 17) | ((0x8000 & muid) >> 10) | ((0x80 & muid) >> 3);
  buffer[1] = (0x7f0000 & muid) >> 16;
  buffer[2] = (0x7f00 & muid) >> 8;
  buffer[3] = (0x7f & muid);
}

/**
 * ags_midi_ci_util_get_muid:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @muid: (out): the return location of #AgsMUID
 *
 * Get MUID.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_muid(AgsMidiCIUtil *midi_ci_util,
			  guchar *buffer,
			  AgsMUID *muid)
{
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer != NULL, 0);

  if(muid != NULL){
    muid[0] = ((0x0f & buffer[0]) << 24) | ((0x7f & buffer[1]) << 16) | ((0x7f & buffer[2]) << 8) | (0x7f & buffer[3]) | ((0x40 & buffer[0]) << 17) | ((0x20 & buffer[0]) << 10) | ((0x10 & buffer[0]) << 3);
  }

  return(4);
}

/**
 * ags_midi_ci_util_put_discovery:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @manufacturer_id: the manufacturer ID
 * @device_family: the device family
 * @device_family_model_number: the device family number
 * @software_revision_level: the software revision level
 * @capability: the capability
 * @max_sysex_message_size: the maximum SYSEX message size
 *
 * Put discovery message.
 * 
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_discovery(AgsMidiCIUtil *midi_ci_util,
			       guchar *buffer,
			       guchar device_id,
			       guchar version,
			       AgsMUID source,
			       guchar manufacturer_id[3],
			       guint16 device_family,
			       guint16 device_family_model_number,
			       guchar *software_revision_level,
			       guchar capability,
			       guint32 max_sysex_message_size)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = 0x7f; // device_id - ignored

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x70; // Sub-ID#2 - discovery

  nth = 0;

  /* version */
  buffer[5 + nth] = version;
  nth++;

  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;
  
  /* broadcast */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    AGS_MIDI_CI_UTIL_BROADCAST_MUID);  
  nth += 4;

  /* manufacturer */
  buffer[5 + nth] = manufacturer_id[0];
  nth++;
  
  buffer[5 + nth] = 0x0;
  nth++;
  
  buffer[5 + nth] = 0x0;
  nth++;

  /* device family */
  buffer[5 + nth] = (0xff & device_family);
  nth++;
  
  buffer[5 + nth] = (0xff00 & device_family) >> 8;
  nth++;

  /* device family model number */
  buffer[5 + nth] = (0xff & device_family_model_number);
  nth++;
  
  buffer[5 + nth] = (0xff00 & device_family_model_number) >> 8;
  nth++;

  /* software revision level */
  buffer[5 + nth] = software_revision_level[0];
  nth++;
  
  buffer[5 + nth] = software_revision_level[1];
  nth++;
  
  buffer[5 + nth] = software_revision_level[2];
  nth++;
  
  buffer[5 + nth] = software_revision_level[3];
  nth++;

  /* capability */
  buffer[5 + nth] = capability;
  nth++;

  /* maximum sysex message size */
  buffer[5 + nth] = (0xff & max_sysex_message_size);
  nth++;
  
  buffer[5 + nth] = (0xff00 & max_sysex_message_size) >> 8;
  nth++;

  buffer[5 + nth] = (0xff0000 & max_sysex_message_size) >> 16;
  nth++;

  buffer[5 + nth] = (0xff000000 & max_sysex_message_size) >> 24;
  nth++;
  
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_discovery:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the return location of device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @manufacturer_id: (out): the return location of manufacturer ID
 * @device_family: (out): the return location of device family
 * @device_family_model_number: (out): the return location of device family number
 * @software_revision_level: (out): the return location of software revision level
 * @capability: (out): the return location of capability
 * @max_sysex_message_size: (out): the return location of maximum SYSEX message size
 *
 * Get discovery message.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_discovery(AgsMidiCIUtil *midi_ci_util,
			       guchar *buffer,
			       guchar *device_id,
			       guchar *version,
			       AgsMUID *source,
			       guchar manufacturer_id[3],
			       guint16 *device_family,
			       guint16 *device_family_model_number,
			       guchar *software_revision_level,
			       guchar *capability,
			       guint32 *max_sysex_message_size)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[2] == 0x7f, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x70, 0);
  g_return_val_if_fail(buffer[9] == 0x7f || buffer[10] == 0x7f || buffer[11] == 0x7f || buffer[12] == 0x7f, 0);

  /* device id */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth+= 4;

  /* destination - broadcast */
  //NOTE:JK: validate first - see top of function
  
  nth += 4;

  /* manufacturer */
  if(manufacturer_id != NULL){
    manufacturer_id[0] = buffer[5 + nth];
    manufacturer_id[1] = buffer[5 + nth + 1];
    manufacturer_id[2] = buffer[5 + nth + 2];
  }

  nth += 3;

  /* device family */
  if(device_family != NULL){
    device_family[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }
  
  nth += 2;

  /* device family model number */
  if(device_family_model_number != NULL){
    device_family_model_number[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }
  
  nth += 2;

  /* software revision level */
  if(software_revision_level != NULL){
    software_revision_level[0] = buffer[5 + nth];
    software_revision_level[1] = buffer[5 + nth + 1];
    software_revision_level[2] = buffer[5 + nth + 2];
    software_revision_level[3] = buffer[5 + nth + 3];
  }

  nth += 4;
  
  /* capability */
  if(capability != NULL){
    capability[0] = buffer[5 + nth];
  }
  
  nth++;

  /* maximum sysex message size */
  if(max_sysex_message_size != NULL){
    max_sysex_message_size[0] = (buffer[5 + nth]) | (buffer[5 + nth + 1] << 8) | (buffer[5 + nth + 2] << 16) | (buffer[5 + nth + 3] << 24);
  }

  nth += 4;

  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_discovery_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @manufacturer_id: the manufacturer ID
 * @device_family: the device family
 * @device_family_model_number: the device family number
 * @software_revision_level: the software revision level
 * @capability: the capability
 * @max_sysex_message_size: the maximum SYSEX message size
 *
 * Put discovery message.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_discovery_reply(AgsMidiCIUtil *midi_ci_util,
				     guchar *buffer,
				     guchar device_id,
				     guchar version,
				     AgsMUID source,
				     AgsMUID destination,
				     gchar manufacturer_id[3],
				     guint16 device_family,
				     guint16 device_family_model_number,
				     gchar *software_revision_level,
				     guchar capability,
				     guint32 max_sysex_message_size)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = 0x7f; // device_id - ignored

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x71; // Sub-ID#2 - discovery reply

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;
  
  /* manufacturer */
  buffer[5 + nth] = manufacturer_id[0];
  nth++;
  
  buffer[5 + nth] = 0x0;
  nth++;
  
  buffer[5 + nth] = 0x0;
  nth++;

  /* device family */
  buffer[5 + nth] = (0xff & device_family);
  nth++;
  
  buffer[5 + nth] = (0xff00 & device_family) >> 8;
  nth++;

  /* device family model number */
  buffer[5 + nth] = (0xff & device_family_model_number);
  nth++;
  
  buffer[5 + nth] = (0xff00 & device_family_model_number) >> 8;
  nth++;

  /* software revision level */
  buffer[5 + nth] = software_revision_level[0];
  nth++;
  
  buffer[5 + nth] = software_revision_level[1];
  nth++;
  
  buffer[5 + nth] = software_revision_level[2];
  nth++;
  
  buffer[5 + nth] = software_revision_level[3];
  nth++;

  /* capability */
  buffer[5 + nth] = capability;
  nth++;

  /* maximum sysex message size */
  buffer[5 + nth] = (0xff & max_sysex_message_size);
  nth++;
  
  buffer[5 + nth] = (0xff00 & max_sysex_message_size) >> 8;
  nth++;

  buffer[5 + nth] = (0xff0000 & max_sysex_message_size) >> 16;
  nth++;

  buffer[5 + nth] = (0xff000000 & max_sysex_message_size) >> 24;
  nth++;
  
  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_discovery_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the return location of device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location destination
 * @manufacturer_id: (out): the return location of manufacturer ID
 * @device_family: (out): the return location of device family
 * @device_family_model_number: (out): the return location of device family number
 * @software_revision_level: (out): the return location of software revision level
 * @capability: (out): the return location of capability
 * @max_sysex_message_size: (out): the return location of maximum SYSEX message size
 *
 * Get discovery message.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_discovery_reply(AgsMidiCIUtil *midi_ci_util,
				     guchar *buffer,
				     guchar *device_id,
				     guchar *version,
				     AgsMUID *source,
				     AgsMUID *destination,
				     guchar manufacturer_id[3],
				     guint16 *device_family,
				     guint16 *device_family_model_number,
				     guchar *software_revision_level,
				     guchar *capability,
				     guint32 *max_sysex_message_size)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[2] == 0x7f, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x71, 0);

  /* device_id */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;
  
  /* manufacturer */
  if(manufacturer_id != NULL){
    manufacturer_id[0] = buffer[5 + nth];
    manufacturer_id[1] = buffer[5 + nth + 1];
    manufacturer_id[2] = buffer[5 + nth + 2];
  }

  nth += 3;

  /* device family */
  if(device_family != NULL){
    device_family[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }
  
  nth += 2;

  /* device family model number */
  if(device_family_model_number != NULL){
    device_family_model_number[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }
  
  nth += 2;

  /* software revision level */
  if(software_revision_level != NULL){
    software_revision_level[0] = buffer[5 + nth];
    software_revision_level[1] = buffer[5 + nth + 1];
    software_revision_level[2] = buffer[5 + nth + 2];
    software_revision_level[3] = buffer[5 + nth + 3];
  }

  nth += 4;
  
  /* capability */
  if(capability != NULL){
    capability[0] = buffer[5 + nth];
  }
  
  nth++;

  /* maximum sysex message size */
  if(max_sysex_message_size != NULL){
    max_sysex_message_size[0] = (buffer[5 + nth]) | (buffer[5 + nth + 1] << 8) | (buffer[5 + nth + 2] << 16) | (buffer[5 + nth + 3] << 24);
  }

  nth += 4;

  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_invalidate_muid:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @target_muid: the MUID
 *
 * Put invalidate MUID.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_invalidate_muid(AgsMidiCIUtil *midi_ci_util,
				     guchar *buffer,
				     guchar device_id,
				     guchar version,
				     AgsMUID source,
				     AgsMUID target_muid)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = 0x7f;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x7e; // Sub-ID#2 - invalidate MUID

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* broadcast */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    AGS_MIDI_CI_UTIL_BROADCAST_MUID);  
  nth += 4;
  
  /* target muid */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    target_muid);
  nth += 4;

  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_invalidate_muid:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the return location of device ID
 * @version: the return location of version
 * @source: the return location of source
 * @target_muid: the return location of MUID
 *
 * Get invalidate MUID.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_invalidate_muid(AgsMidiCIUtil *midi_ci_util,
				     guchar *buffer,
				     guchar *device_id,
				     guchar *version,
				     AgsMUID *source,
				     AgsMUID *target_muid)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[2] == 0x7f, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x7e, 0);
  g_return_val_if_fail(buffer[9] == 0x7f || buffer[10] == 0x7f || buffer[11] == 0x7f || buffer[12] == 0x7f, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination - broadcast */
  //NOTE:JK: validate first - see top of function

  nth += 4;

  /* target muid */
  if(target_muid != NULL){
    ags_midi_ci_util_get_muid(midi_ci_util,
			      buffer + 5 + nth,
			      target_muid);
  }

  nth += 4;

  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_ack:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @orig_transaction: the original transaction
 * @status_code: the status code
 * @status_data: the status data
 * @details: the details
 * @message_length: the message length
 * @message: the message
 *
 * Put ACK MIDI CI message.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_ack(AgsMidiCIUtil *midi_ci_util,
			 guchar *buffer,
			 guchar device_id,
			 guchar version,
			 AgsMUID source,
			 AgsMUID destination,
			 guchar orig_transaction,
			 guchar status_code,
			 guchar status_data,
			 guchar details[5],
			 guint16 message_length,
			 guchar *message)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x7d; // Sub-ID#2 - ACK

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* orig transaction */
  buffer[5 + nth] = orig_transaction;

  nth++;

  /* status code */
  buffer[5 + nth] = status_code;

  nth++;

  /* status data */
  buffer[5 + nth] = status_data;

  nth++;

  /* details */
  buffer[5 + nth] = details[0];
  nth++;

  buffer[5 + nth] = details[1];
  nth++;

  buffer[5 + nth] = details[2];
  nth++;

  buffer[5 + nth] = details[3];
  nth++;
  
  buffer[5 + nth] = details[4];
  nth++;

  /* message length */
  buffer[5 + nth] = (0xff & message_length);
  nth++;
  
  buffer[5 + nth] = (0xff00 & message_length) >> 8;
  nth++;

  /* message */
  if(message != NULL){
    memcpy(buffer + 5 + nth, message, message_length * sizeof(guchar));
  }

  nth += message_length;

  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_ack:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the return location of device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location of destination
 * @orig_transaction: the original transaction
 * @status_code: (out): the return location of status code
 * @status_data: (out): the return location of status data
 * @details: (out): the return location of details
 * @message_length: (out): the return location of message length
 * @message: (out): the return location of message
 *
 * Get ACK MIDI CI message.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_ack(AgsMidiCIUtil *midi_ci_util,
			 guchar *buffer,
			 guchar *device_id,
			 guchar *version,
			 AgsMUID *source,
			 AgsMUID *destination,
			 guchar *orig_transaction,
			 guchar *status_code,
			 guchar *status_data,
			 guchar details[5],
			 guint16 *message_length,
			 guchar **message)
{
  guint nth;
  guint i_stop;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x7d, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;

  /* original transaction */
  if(orig_transaction != NULL){
    orig_transaction[0] = buffer[5 + nth];
  }

  nth++;

  /* status code */
  if(status_code != NULL){
    status_code[0] = buffer[5 + nth];
  }

  nth++;

  /* status data */
  if(status_data != NULL){
    status_data[0] = buffer[5 + nth];
  }

  nth++;

  /* details */
  if(details != NULL){
    details[0] = buffer[5 + nth];
    details[1] = buffer[5 + nth + 1];
    details[2] = buffer[5 + nth + 2];
    details[3] = buffer[5 + nth + 3];
    details[4] = buffer[5 + nth + 4];
  }

  nth += 5;
  
  /* message length */
  if(message_length != NULL){
    message_length[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  i_stop = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));

  nth += 2;

  /* message */
  if(message != NULL){
    if(i_stop > 0){
      message[0] = g_malloc(i_stop * sizeof(guchar));
      
      memcpy(message[0], buffer + 5 + nth, i_stop * sizeof(guchar));
    }else{
      message[0] = NULL;
    }
  }

  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_nak:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 *
 * Put NAK MIDI CI message.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_nak(AgsMidiCIUtil *midi_ci_util,
			 guchar *buffer,
			 guchar device_id,
			 guchar version,
			 AgsMUID source,
			 AgsMUID destination)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = 0x7f;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x7f; // Sub-ID#2 - NAK

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_nak:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the return location of device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location of destination
 *
 * Get NAK MIDI CI message.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_nak(AgsMidiCIUtil *midi_ci_util,
			 guchar *buffer,
			 guchar *device_id,
			 guchar *version,
			 AgsMUID *source,
			 AgsMUID *destination)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[2] == 0x7f, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x7f, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;

  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_initiate_protocol_negotiation:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @authority_level: the authority level
 * @number_of_supported_protocols: the number of supported protocols
 * @preferred_protocol_type: the preferred protocol type
 *
 * Put initiate protocol negotiation message. Deprecated since MIDI CI version 1.2.
 * 
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_initiate_protocol_negotiation(AgsMidiCIUtil *midi_ci_util,
						   guchar *buffer,
						   guchar device_id,
						   guchar version,
						   AgsMUID source,
						   AgsMUID destination,
						   AgsMidiCIAuthorityLevel authority_level,
						   guchar number_of_supported_protocols,
						   guchar **preferred_protocol_type)
{
  guint nth;
  guint i;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = 0x7f;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x10; // Sub-ID#2 - initiate protocol negotiation

  nth = 0;

  /* version */
  buffer[5 + nth] = version;
  nth++;

  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* authority level */
  buffer[5 + nth] = authority_level;
  nth++;

  /* number of supported protocols */
  buffer[5 + nth] = number_of_supported_protocols;
  nth++;

  /* preferred protocol type */
  for(i = 0; i < number_of_supported_protocols; i++){
    buffer[5 + nth + (i * 5)] = preferred_protocol_type[i][0];
    buffer[5 + nth + (i * 5) + 1] = preferred_protocol_type[i][1];
    buffer[5 + nth + (i * 5) + 2] = preferred_protocol_type[i][2];
    buffer[5 + nth + (i * 5) + 3] = preferred_protocol_type[i][3];
    buffer[5 + nth + (i * 5) + 4] = preferred_protocol_type[i][4];
  }

  nth += (number_of_supported_protocols * 5);
  
  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_initiate_protocol_negotiation:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the return location of device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the destination
 * @authority_level: (out): the authority level
 * @number_of_supported_protocols: (out): the number of supported protocols
 * @preferred_protocol_type: (out): the preferred protocol type
 *
 * Get initiate protocol negotiation message. Deprecated since MIDI CI version 1.2.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_initiate_protocol_negotiation(AgsMidiCIUtil *midi_ci_util,
						   guchar *buffer,
						   guchar *device_id,
						   guchar *version,
						   AgsMUID *source,
						   AgsMUID *destination,
						   AgsMidiCIAuthorityLevel *authority_level,
						   guchar *number_of_supported_protocols,
						   guchar **preferred_protocol_type)
{
  guint nth;
  guint i, i_stop;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[2] == 0x7f, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x10, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;
  
  /* authority level */
  if(authority_level != NULL){
    authority_level[0] = buffer[5 + nth];
  }

  nth++;

  /* authority level */
  if(number_of_supported_protocols != NULL){
    number_of_supported_protocols[0] = buffer[5 + nth];
  }

  i_stop = buffer[5 + nth];
  
  nth++;

  /* preferred protocol type */
  if(preferred_protocol_type != NULL){
    for(i = 0; i < i_stop; i++){
      if(preferred_protocol_type[i] != NULL){
	preferred_protocol_type[i][0] = buffer[5 + nth + (i * 5)];
	preferred_protocol_type[i][1] = buffer[5 + nth + (i * 5) + 1];
	preferred_protocol_type[i][2] = buffer[5 + nth + (i * 5) + 2];
	preferred_protocol_type[i][3] = buffer[5 + nth + (i * 5) + 3];
	preferred_protocol_type[i][4] = buffer[5 + nth + (i * 5) + 4];
      }
    }
  }
  
  nth += (i_stop * 5);

  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_initiate_protocol_negotiation_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @authority_level: the authority level
 * @number_of_supported_protocols: the number of supported protocols
 * @preferred_protocol_type: the preferred protocol type
 *
 * Put initiate protocol negotiation reply message. Deprecated since MIDI CI version 1.2.
 * 
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_initiate_protocol_negotiation_reply(AgsMidiCIUtil *midi_ci_util,
							 guchar *buffer,
							 guchar device_id,
							 guchar version,
							 AgsMUID source,
							 AgsMUID destination,
							 AgsMidiCIAuthorityLevel authority_level,
							 guchar number_of_supported_protocols,
							 guchar **preferred_protocol_type)
{
  guint nth;
  guint i;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = 0x7f;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x11; // Sub-ID#2 - initiate protocol negotiation reply

  nth = 0;

  /* version */
  buffer[5 + nth] = version;
  nth++;

  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* authority level */
  buffer[5 + nth] = authority_level;
  nth++;

  /* number of supported protocols */
  buffer[5 + nth] = number_of_supported_protocols;
  nth++;

  /* preferred protocol type */
  for(i = 0; i < number_of_supported_protocols; i++){
    buffer[5 + nth + (i * 5)] = preferred_protocol_type[i][0];
    buffer[5 + nth + (i * 5) + 1] = preferred_protocol_type[i][1];
    buffer[5 + nth + (i * 5) + 2] = preferred_protocol_type[i][2];
    buffer[5 + nth + (i * 5) + 3] = preferred_protocol_type[i][3];
    buffer[5 + nth + (i * 5) + 4] = preferred_protocol_type[i][4];
  }

  nth += (number_of_supported_protocols * 5);
  
  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_initiate_protocol_negotiation_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the return location of device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the destination
 * @authority_level: (out): the authority level
 * @number_of_supported_protocols: (out): the number of supported protocols
 * @preferred_protocol_type: (out): the preferred protocol type
 *
 * Get initiate protocol negotiation reply message. Deprecated since MIDI CI version 1.2.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_initiate_protocol_negotiation_reply(AgsMidiCIUtil *midi_ci_util,
							 guchar *buffer,
							 guchar *device_id,
							 guchar *version,
							 AgsMUID *source,
							 AgsMUID *destination,
							 AgsMidiCIAuthorityLevel *authority_level,
							 guchar *number_of_supported_protocols,
							 guchar **preferred_protocol_type)
{
  guint nth;
  guint i, i_stop;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[2] == 0x7f, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x11, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;
  
  /* authority level */
  if(authority_level != NULL){
    authority_level[0] = buffer[5 + nth];
  }

  nth++;

  /* number of supported protocols */
  if(number_of_supported_protocols != NULL){
    number_of_supported_protocols[0] = buffer[5 + nth];
  }

  i_stop = buffer[5 + nth];
  
  nth++;

  /* preferred protocol type */
  if(preferred_protocol_type != NULL){
    for(i = 0; i < i_stop; i++){
      if(preferred_protocol_type[i] != NULL){
	preferred_protocol_type[i][0] = buffer[5 + nth + (i * 5)];
	preferred_protocol_type[i][1] = buffer[5 + nth + (i * 5) + 1];
	preferred_protocol_type[i][2] = buffer[5 + nth + (i * 5) + 2];
	preferred_protocol_type[i][3] = buffer[5 + nth + (i * 5) + 3];
	preferred_protocol_type[i][4] = buffer[5 + nth + (i * 5) + 4];
      }
    }
  }
  
  nth += (i_stop * 5);

  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_set_protocol_type:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @authority_level: the authority level
 * @protocol_type: the new protocol type
 *
 * Put set protocol type message. Deprecated since MIDI CI version 1.2.
 * 
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_set_protocol_type(AgsMidiCIUtil *midi_ci_util,
				       guchar *buffer,
				       guchar device_id,
				       guchar version,
				       AgsMUID source,
				       AgsMUID destination,
				       AgsMidiCIAuthorityLevel authority_level,
				       guchar *protocol_type)
{
  guint nth;
  guint i;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = 0x7f;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x12; // Sub-ID#2 - set protocol type

  nth = 0;

  /* version */
  buffer[5 + nth] = version;
  nth++;

  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* authority level */
  buffer[5 + nth] = authority_level;
  nth++;

  /* preferred protocol type */
  buffer[5 + nth] = protocol_type[0];
  buffer[5 + nth + 1] = protocol_type[1];
  buffer[5 + nth + 2] = protocol_type[2];
  buffer[5 + nth + 3] = protocol_type[3];
  buffer[5 + nth + 4] = protocol_type[4];

  nth += 5;
  
  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_set_protocol_type:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the return location of device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the destination
 * @authority_level: (out): the authority level
 * @protocol_type: (out): the protocol type
 *
 * Get set protocol type message. Deprecated since MIDI CI version 1.2.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_set_protocol_type(AgsMidiCIUtil *midi_ci_util,
				       guchar *buffer,
				       guchar *device_id,
				       guchar *version,
				       AgsMUID *source,
				       AgsMUID *destination,
				       AgsMidiCIAuthorityLevel *authority_level,
				       guchar *protocol_type)
{
  guint nth;
  guint i;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[2] == 0x7f, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x13, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }
 
  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;
  
  /* authority level */
  if(authority_level != NULL){
    authority_level[0] = buffer[5 + nth];
  }

  nth++;

  /* preferred protocol type */
  if(protocol_type != NULL){
    protocol_type[0] = buffer[5 + nth];
    protocol_type[1] = buffer[5 + nth + 1];
    protocol_type[2] = buffer[5 + nth + 2];
    protocol_type[3] = buffer[5 + nth + 3];
    protocol_type[4] = buffer[5 + nth + 4];
  }
  
  nth += 5;

  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_confirm_protocol_type:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @authority_level: the authority level
 *
 * Put confirm protocol type message. Deprecated since MIDI CI version 1.2.
 * 
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_confirm_protocol_type(AgsMidiCIUtil *midi_ci_util,
					   guchar *buffer,
					   guchar device_id,
					   guchar version,
					   AgsMUID source,
					   AgsMUID destination,
					   AgsMidiCIAuthorityLevel authority_level)
{
  guint nth;
  guint i;

  static GMutex mutex = {0,};

  static guchar *test_data;

  static gboolean init_test_data = FALSE;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  if(!init_test_data){
    for(i = 0; i < 48; i++){
      test_data[i] = (guchar) i;
    }
    
    init_test_data = TRUE;
  }

  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = 0x7f;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x13; // Sub-ID#2 - confirm new protocol type

  nth = 0;

  /* version */
  buffer[5 + nth] = version;
  nth++;

  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* authority level */
  buffer[5 + nth] = authority_level;
  nth++;

  /* test data */
  memcpy(buffer + 5 + nth, test_data, 48 * sizeof(guchar));
  
  nth += 48;
  
  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_confirm_protocol_type:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the return location of device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the destination
 * @authority_level: (out): the authority level
 *
 * Get confirm protocol type message. Deprecated since MIDI CI version 1.2.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_confirm_protocol_type(AgsMidiCIUtil *midi_ci_util,
					   guchar *buffer,
					   guchar *device_id,
					   guchar *version,
					   AgsMUID *source,
					   AgsMUID *destination,
					   AgsMidiCIAuthorityLevel *authority_level)
{
  guint nth;
  guint i, i_stop;

  static GMutex mutex = {0,};

  static guchar *test_data;

  static gboolean init_test_data = FALSE;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[2] == 0x7f, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x13, 0);

  if(!init_test_data){
    for(i = 0; i < 48; i++){
      test_data[i] = (guchar) i;
    }
    
    init_test_data = TRUE;
  }

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;
  
  /* authority level */
  if(authority_level != NULL){
    authority_level[0] = buffer[5 + nth];
  }

  nth++;

  /* check test data */
  if(!g_ascii_strncasecmp(buffer + 5 + nth, test_data, 48)){
    nth += 48;

    /* sysex end */
    if(buffer[5 + nth] == 0xf7){
      nth++;

      return(5 + nth);
    }
  }  

  return(0);
}

/**
 * ags_midi_ci_util_put_confirm_protocol_type_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @authority_level: the authority level
 *
 * Put confirm protocol type reply message. Deprecated since MIDI CI version 1.2.
 * 
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_confirm_protocol_type_reply(AgsMidiCIUtil *midi_ci_util,
						 guchar *buffer,
						 guchar device_id,
						 guchar version,
						 AgsMUID source,
						 AgsMUID destination,
						 AgsMidiCIAuthorityLevel authority_level)
{
  guint nth;
  guint i;

  static GMutex mutex = {0,};

  static guchar *test_data;

  static gboolean init_test_data = FALSE;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  if(!init_test_data){
    for(i = 0; i < 48; i++){
      test_data[i] = (guchar) i;
    }
    
    init_test_data = TRUE;
  }

  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = 0x7f;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x14; // Sub-ID#2 - confirm protocol type reply

  nth = 0;

  /* version */
  buffer[5 + nth] = version;
  nth++;

  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* authority level */
  buffer[5 + nth] = authority_level;
  nth++;

  /* test data */
  memcpy(buffer + 5 + nth, test_data, 48 * sizeof(guchar));
  
  nth += 48;
  
  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_confirm_protocol_type_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the return location of device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the destination
 * @authority_level: (out): the authority level
 *
 * Get confirm protocol type reply message. Deprecated since MIDI CI version 1.2.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_confirm_protocol_type_reply(AgsMidiCIUtil *midi_ci_util,
						 guchar *buffer,
						 guchar *device_id,
						 guchar *version,
						 AgsMUID *source,
						 AgsMUID *destination,
						 AgsMidiCIAuthorityLevel *authority_level)
{
  guint nth;
  guint i;

  static GMutex mutex = {0,};

  static guchar *test_data;

  static gboolean init_test_data = FALSE;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[2] == 0x7f, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x14, 0);

  if(!init_test_data){
    for(i = 0; i < 48; i++){
      test_data[i] = (guchar) i;
    }
    
    init_test_data = TRUE;
  }

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }
 
  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;
  
  /* authority level */
  if(authority_level != NULL){
    authority_level[0] = buffer[5 + nth];
  }

  nth++;

  /* check test data */
  if(!g_ascii_strncasecmp(buffer + 5 + nth, test_data, 48)){
    nth += 48;

    /* sysex end */
    if(buffer[5 + nth] == 0xf7){
      nth++;

      return(5 + nth);
    }
  }  

  return(0);
}

/**
 * ags_midi_ci_util_put_confirm_protocol_type_established:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @authority_level: the authority level
 *
 * Put confirm protocol type established message. Deprecated since MIDI CI version 1.2.
 * 
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_confirm_protocol_type_established(AgsMidiCIUtil *midi_ci_util,
						       guchar *buffer,
						       guchar device_id,
						       guchar version,
						       AgsMUID source,
						       AgsMUID destination,
						       AgsMidiCIAuthorityLevel authority_level)
{
  guint nth;
  guint i;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = 0x7f;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x15; // Sub-ID#2 - confirm protocol type established

  nth = 0;

  /* version */
  buffer[5 + nth] = version;
  nth++;

  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* authority level */
  buffer[5 + nth] = authority_level;
  nth++;
  
  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_confirm_protocol_type_established:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the return location of device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the destination
 * @authority_level: (out): the authority level
 *
 * Get confirm protocol type established message. Deprecated since MIDI CI version 1.2.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_confirm_protocol_type_established(AgsMidiCIUtil *midi_ci_util,
						       guchar *buffer,
						       guchar *device_id,
						       guchar *version,
						       AgsMUID *source,
						       AgsMUID *destination,
						       AgsMidiCIAuthorityLevel *authority_level)
{
  guint nth;
  guint i;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[2] == 0x7f, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x15, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }
 
  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;
  
  /* authority level */
  if(authority_level != NULL){
    authority_level[0] = buffer[5 + nth];
  }

  nth++;

  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_profile:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 *
 * Put profile message.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_profile(AgsMidiCIUtil *midi_ci_util,
			     guchar *buffer,
			     guchar device_id,
			     guchar version,
			     AgsMUID source,
			     AgsMUID destination)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x20; // Sub-ID#2 - profile message

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_profile:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the return location of device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location of destination
 *
 * Profile message.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_profile(AgsMidiCIUtil *midi_ci_util,
			     guchar *buffer,
			     guchar *device_id,
			     guchar *version,
			     AgsMUID *source,
			     AgsMUID *destination)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x20, 0);

  /* device_id */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;

  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_profile_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @enabled_profile_count: enabled profile count
 * @enabled_profile: enabled profile
 * @disabled_profile_count: disabled profile count
 * @disabled_profile: disabled profile
 *
 * Put profile message.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_profile_reply(AgsMidiCIUtil *midi_ci_util,
				   guchar *buffer,
				   guchar device_id,
				   guchar version,
				   AgsMUID source,
				   AgsMUID destination,
				   guint16 enabled_profile_count,
				   guchar* enabled_profile[5],
				   guint16 disabled_profile_count,
				   guchar* disabled_profile[5])
{
  guint nth;
  guint i, i_stop;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x21; // Sub-ID#2 - profile message reply

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* enabled profile count */
  buffer[5 + nth] = (0xff & enabled_profile_count);
  nth++;
  
  buffer[5 + nth] = (0xff00 & enabled_profile_count) >> 8;
  nth++;

  i_stop = enabled_profile_count;
  
  /* enabled profile */
  for(i = 0; i < i_stop; i++){
    buffer[5 + nth + (i * 5)] = enabled_profile[i][0];
    buffer[5 + nth + (i * 5) + 1] = enabled_profile[i][1];
    buffer[5 + nth + (i * 5) + 2] = enabled_profile[i][2];
    buffer[5 + nth + (i * 5) + 3] = enabled_profile[i][3];
    buffer[5 + nth + (i * 5) + 4] = enabled_profile[i][4];
  }

  nth += (i_stop * 5);

  /* disabled profile count */
  buffer[5 + nth] = (0xff & disabled_profile_count);
  nth++;
  
  buffer[5 + nth] = (0xff00 & disabled_profile_count) >> 8;
  nth++;

  i_stop = disabled_profile_count;
  
  /* disabled profile */
  for(i = 0; i < i_stop; i++){
    buffer[5 + nth + (i * 5)] = disabled_profile[i][0];
    buffer[5 + nth + (i * 5) + 1] = disabled_profile[i][1];
    buffer[5 + nth + (i * 5) + 2] = disabled_profile[i][2];
    buffer[5 + nth + (i * 5) + 3] = disabled_profile[i][3];
    buffer[5 + nth + (i * 5) + 4] = disabled_profile[i][4];
  }

  nth += (i_stop * 5);
  
  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_profile_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the return location of device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location of destination
 * @enabled_profile_count: (out): enabled profile count
 * @enabled_profile: (out): enabled profile
 * @disabled_profile_count: (out): disabled profile count
 * @disabled_profile: (out): disabled profile
 *
 * Profile message.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_profile_reply(AgsMidiCIUtil *midi_ci_util,
				   guchar *buffer,
				   guchar *device_id,
				   guchar *version,
				   AgsMUID *source,
				   AgsMUID *destination,
				   guint16 *enabled_profile_count,
				   guchar ***enabled_profile,
				   guint16 *disabled_profile_count,
				   guchar ***disabled_profile)
{
  guint nth;
  guint i, i_stop;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x21, 0);

  /* device_id */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;

  /* enabled profile count */
  if(enabled_profile_count != NULL){
    enabled_profile_count[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  i_stop = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  nth += 2;

  /* enabled profile */
  if(i_stop > 0){
    enabled_profile[0] = (guchar **) g_malloc(i_stop * sizeof(guchar *));

    for(i = 0; i < i_stop; i++){
      enabled_profile[0][i] = (guchar *) g_malloc(5 * sizeof(guchar));
    
      enabled_profile[0][i][0] = buffer[5 + nth + (i * 5)];
      enabled_profile[0][i][1] = buffer[5 + nth + (i * 5) + 1];
      enabled_profile[0][i][2] = buffer[5 + nth + (i * 5) + 2];
      enabled_profile[0][i][3] = buffer[5 + nth + (i * 5) + 3];
      enabled_profile[0][i][4] = buffer[5 + nth + (i * 5) + 4];
    }
  }else{
    enabled_profile[0] = NULL;
  }
  
  nth += (i_stop * 5);

  /* disabled profile count */
  if(disabled_profile_count != NULL){
    disabled_profile_count[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  i_stop = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  nth += 2;

  /* disabled profile */
  if(i_stop > 0){
    disabled_profile[0] = (guchar **) g_malloc(i_stop * sizeof(guchar *));

    for(i = 0; i < i_stop; i++){
      disabled_profile[0][i] = (guchar *) g_malloc(5 * sizeof(guchar));
    
      disabled_profile[0][i][0] = buffer[5 + nth + (i * 5)];
      disabled_profile[0][i][1] = buffer[5 + nth + (i * 5) + 1];
      disabled_profile[0][i][2] = buffer[5 + nth + (i * 5) + 2];
      disabled_profile[0][i][3] = buffer[5 + nth + (i * 5) + 3];
      disabled_profile[0][i][4] = buffer[5 + nth + (i * 5) + 4];
    }
  }else{
    disabled_profile[0] = NULL;
  }
  
  nth += (i_stop * 5);

  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_profile_enabled_report:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @enabled_profile: the enabled profile
 * @enabled_channel_count: the enabled channel count
 *
 * Put profile enabled report message.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_profile_enabled_report(AgsMidiCIUtil *midi_ci_util,
					    guchar *buffer,
					    guchar device_id,
					    guchar version,
					    AgsMUID source,
					    guchar enabled_profile[5],
					    guint16 enabled_channel_count)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x24; // Sub-ID#2 - profile enabled report

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* broadcast */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    AGS_MIDI_CI_UTIL_BROADCAST_MUID);  
  nth += 4;

  /* enabled profile */
  buffer[5 + nth] = enabled_profile[0];
  nth++;

  buffer[5 + nth] = enabled_profile[1];
  nth++;

  buffer[5 + nth] = enabled_profile[2];
  nth++;

  buffer[5 + nth] = enabled_profile[3];
  nth++;

  buffer[5 + nth] = enabled_profile[4];
  nth++;  

  /* enabled channel count */
  buffer[5 + nth] = (0xff & enabled_channel_count);
  nth++;
  
  buffer[5 + nth] = (0xff00 & enabled_channel_count) >> 8;
  nth++;
  
  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_profile_enabled_report:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the return location of device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @enabled_profile: (out): the return location of enabled profile
 * @enabled_channel_count: (out): the return location of enabled channel count
 *
 * Get profile enabled report message.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_profile_enabled_report(AgsMidiCIUtil *midi_ci_util,
					    guchar *buffer,
					    guchar *device_id,
					    guchar *version,
					    AgsMUID *source,
					    guchar enabled_profile[5],
					    guint16 *enabled_channel_count)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x24, 0);
  g_return_val_if_fail(buffer[9] == 0x7f || buffer[10] == 0x7f || buffer[11] == 0x7f || buffer[12] == 0x7f, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }
  
  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination - broadcast */
  //NOTE:JK: validate first - see top of function
  
  nth += 4;

  /* enabled profile */
  enabled_profile[0] = buffer[5 + nth];
  enabled_profile[1] = buffer[5 + nth + 1];
  enabled_profile[2] = buffer[5 + nth + 2];
  enabled_profile[3] = buffer[5 + nth + 3];
  enabled_profile[4] = buffer[5 + nth + 4];

  nth += 5;

  /* enabled channel count */
  if(enabled_channel_count != NULL){
    enabled_channel_count[0] = (buffer[5 + nth]) | (buffer[5 + nth + 1] << 8);
    nth += 2;
  }
  
  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_profile_disabled_report:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @disabled_profile: the disabled profile
 * @disabled_channel_count: the disabled channel count
 *
 * Put profile disabled report message.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_profile_disabled_report(AgsMidiCIUtil *midi_ci_util,
					     guchar *buffer,
					     guchar device_id,
					     guchar version,
					     AgsMUID source,
					     guchar disabled_profile[5],
					     guint16 disabled_channel_count)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x25; // Sub-ID#2 - profile disabled report

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* broadcast */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    AGS_MIDI_CI_UTIL_BROADCAST_MUID);  
  nth += 4;

  /* disabled profile */
  buffer[5 + nth] = disabled_profile[0];
  nth++;

  buffer[5 + nth] = disabled_profile[1];
  nth++;

  buffer[5 + nth] = disabled_profile[2];
  nth++;

  buffer[5 + nth] = disabled_profile[3];
  nth++;

  buffer[5 + nth] = disabled_profile[4];
  nth++;  

  /* disabled channel count */
  buffer[5 + nth] = (0xff & disabled_channel_count);
  nth++;
  
  buffer[5 + nth] = (0xff00 & disabled_channel_count) >> 8;
  nth++;
  
  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_profile_disabled_report:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the return location of device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @disabled_profile: (out): the return location of disabled profile
 * @disabled_channel_count: (out): the return location of disabled channel count
 *
 * Get profile disabled report message.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_profile_disabled_report(AgsMidiCIUtil *midi_ci_util,
					     guchar *buffer,
					     guchar *device_id,
					     guchar *version,
					     AgsMUID *source,
					     guchar disabled_profile[5],
					     guint16 *disabled_channel_count)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x25, 0);
  g_return_val_if_fail(buffer[9] == 0x7f || buffer[10] == 0x7f || buffer[11] == 0x7f || buffer[12] == 0x7f, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination - broadcast */
  //NOTE:JK: validate first - see top of function
  
  nth += 4;

  /* disabled profile */
  disabled_profile[0] = buffer[5 + nth];
  disabled_profile[1] = buffer[5 + nth + 1];
  disabled_profile[2] = buffer[5 + nth + 2];
  disabled_profile[3] = buffer[5 + nth + 3];
  disabled_profile[4] = buffer[5 + nth + 4];

  nth += 5;

  /* disabled channel count */
  if(disabled_channel_count != NULL){
    disabled_channel_count[0] = (buffer[5 + nth]) | (buffer[5 + nth + 1] << 8);
    nth += 2;
  }
  
  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_profile_added:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @add_profile: the profile to add
 *
 * Put profile added message.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_profile_added(AgsMidiCIUtil *midi_ci_util,
				   guchar *buffer,
				   guchar device_id,
				   guchar version,
				   AgsMUID source,
				   guchar add_profile[5])
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x26; // Sub-ID#2 - profile added

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* broadcast */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    AGS_MIDI_CI_UTIL_BROADCAST_MUID);  
  nth += 4;

  /* add profile */
  buffer[5 + nth] = add_profile[0];
  nth++;

  buffer[5 + nth] = add_profile[1];
  nth++;

  buffer[5 + nth] = add_profile[2];
  nth++;

  buffer[5 + nth] = add_profile[3];
  nth++;

  buffer[5 + nth] = add_profile[4];
  nth++;  
  
  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_profile_added:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @add_profile: (out): the return location of profile to add
 *
 * Get profile added message.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_profile_added(AgsMidiCIUtil *midi_ci_util,
				   guchar *buffer,
				   guchar *device_id,
				   guchar *version,
				   AgsMUID *source,
				   guchar add_profile[5])
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x26, 0);
  g_return_val_if_fail(buffer[9] == 0x7f || buffer[10] == 0x7f || buffer[11] == 0x7f || buffer[12] == 0x7f, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination - broadcast */
  //NOTE:JK: validate first - see top of function
  
  nth += 4;

  /* add profile */
  add_profile[0] = buffer[5 + nth];
  add_profile[1] = buffer[5 + nth + 1];
  add_profile[2] = buffer[5 + nth + 2];
  add_profile[3] = buffer[5 + nth + 3];
  add_profile[4] = buffer[5 + nth + 4];

  nth += 5;
  
  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_profile_removed:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @remove_profile: the profile to remove
 *
 * Put profile remove message.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_profile_removed(AgsMidiCIUtil *midi_ci_util,
				     guchar *buffer,
				     guchar device_id,
				     guchar version,
				     AgsMUID source,
				     guchar remove_profile[5])
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x27; // Sub-ID#2 - profile removed

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* broadcast */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    AGS_MIDI_CI_UTIL_BROADCAST_MUID);  
  nth += 4;

  /* remove profile */
  buffer[5 + nth] = remove_profile[0];
  nth++;

  buffer[5 + nth] = remove_profile[1];
  nth++;

  buffer[5 + nth] = remove_profile[2];
  nth++;

  buffer[5 + nth] = remove_profile[3];
  nth++;

  buffer[5 + nth] = remove_profile[4];
  nth++;  
  
  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_profile_removed:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @remove_profile: (out): the profile to remove
 *
 * Get profile remove message.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_profile_removed(AgsMidiCIUtil *midi_ci_util,
				     guchar *buffer,
				     guchar *device_id,
				     guchar *version,
				     AgsMUID *source,
				     guchar remove_profile[5])
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x27, 0);
  g_return_val_if_fail(buffer[9] == 0x7f || buffer[10] == 0x7f || buffer[11] == 0x7f || buffer[12] == 0x7f, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination - broadcast */
  //NOTE:JK: validate first - see top of function
  
  nth += 4;

  /* remove profile */
  remove_profile[0] = buffer[5 + nth];
  remove_profile[1] = buffer[5 + nth + 1];
  remove_profile[2] = buffer[5 + nth + 2];
  remove_profile[3] = buffer[5 + nth + 3];
  remove_profile[4] = buffer[5 + nth + 4];

  nth += 5;
  
  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_profile_specific_data:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @profile_id: the profile ID
 * @profile_specific_data_length: profile specific data length
 * @profile_specific_data: profile specific data
 *
 * Put profile specific data message.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_profile_specific_data(AgsMidiCIUtil *midi_ci_util,
					   guchar *buffer,
					   guchar device_id,
					   guchar version,
					   AgsMUID source,
					   AgsMUID destination,
					   guchar profile_id[5],
					   guint32 profile_specific_data_length,
					   guchar *profile_specific_data)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x2f; // Sub-ID#2 - profile specific data

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* profile ID */
  buffer[5 + nth] = profile_id[0];
  nth++;
  
  buffer[5 + nth] = profile_id[1];
  nth++;

  buffer[5 + nth] = profile_id[2];
  nth++;

  buffer[5 + nth] = profile_id[3];
  nth++;

  buffer[5 + nth] = profile_id[4];
  nth++;

  /* profile specific data length */
  buffer[5 + nth] = (0xff & profile_specific_data_length);
  nth++;
  
  buffer[5 + nth] = (0xff00 & profile_specific_data_length) >> 8;
  nth++;

  buffer[5 + nth] = (0xff0000 & profile_specific_data_length) >> 16;
  nth++;

  buffer[5 + nth] = (0xff000000 & profile_specific_data_length) >> 24;
  nth++;

  /* profile specific data */
  if(profile_specific_data != NULL){
    memcpy(buffer + 5 + nth, profile_specific_data, profile_specific_data_length * sizeof(guchar));
  }

  nth += profile_specific_data_length;
  
  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_profile_specific_data:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the return location of device id
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location of destination
 * @profile_id: (out): the profile ID
 * @profile_specific_data_length: (out): the return location of profile specific data length
 * @profile_specific_data: (out): the return location of profile specific data
 *
 * Get profile specific data message.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_profile_specific_data(AgsMidiCIUtil *midi_ci_util,
					   guchar *buffer,
					   guchar *device_id,
					   guchar *version,
					   AgsMUID *source,
					   AgsMUID *destination,
					   guchar profile_id[5],
					   guint32 *profile_specific_data_length,
					   guchar **profile_specific_data)
{
  guint nth;
  guint i_stop;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x2f, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;

  /* profile ID */
  if(profile_id != NULL){
    profile_id[0] = buffer[5 + nth];
    profile_id[1] = buffer[5 + nth + 1];
    profile_id[2] = buffer[5 + nth + 2];
    profile_id[3] = buffer[5 + nth + 3];
    profile_id[4] = buffer[5 + nth + 4];
  }

  nth += 5;
  
  /* profile specific data length */
  if(profile_specific_data_length != NULL){
    profile_specific_data_length[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8) | (buffer[5 + nth + 2] << 16) | (buffer[5 + nth + 3] << 24));
  }

  i_stop = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8) | (buffer[5 + nth + 2] << 16) | (buffer[5 + nth + 3] << 24));

  nth += 4;
  
  /* profile specific data */
  if(profile_specific_data != NULL){
    if(i_stop > 0){
      profile_specific_data[0] = g_malloc(i_stop * sizeof(guchar));
      
      memcpy(profile_specific_data[0], buffer + 5 + nth, i_stop * sizeof(guchar));
    }else{
      profile_specific_data[0] = NULL;
    }
  }

  nth += i_stop;
  
  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_property_exchange_capabilities:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @supported_property_exchange_count: the supported property exchange count
 * @property_exchange_major: the major version
 * @property_exchange_minor: the minor version
 * 
 * Put number of supported property exchange message.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_property_exchange_capabilities(AgsMidiCIUtil *midi_ci_util,
						    guchar *buffer,
						    guchar device_id,
						    guchar version,
						    AgsMUID source,
						    AgsMUID destination,
						    guchar supported_property_exchange_count,
						    guchar property_exchange_major,
						    guchar property_exchange_minor)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x30; // Sub-ID#2 - property data exchange capabilities

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* supported property exchange count */
  buffer[5 + nth] = supported_property_exchange_count;
  nth++;

  /* major */
  buffer[5 + nth] = property_exchange_major;
  nth++;

  /* minor */
  buffer[5 + nth] = property_exchange_minor;
  nth++;

  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_property_exchange_capabilities:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the return location of device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location of destination
 * @supported_property_exchange_count: (out): the supported property exchange count
 * @property_exchange_major: (out): the return location of major version
 * @property_exchange_minor: (out): the return location of minor version
 *
 * Get number of supported property exchange count data message.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_property_exchange_capabilities(AgsMidiCIUtil *midi_ci_util,
						    guchar *buffer,
						    guchar *device_id,
						    guchar *version,
						    AgsMUID *source,
						    AgsMUID *destination,
						    guchar *supported_property_exchange_count,
						    guchar *property_exchange_major,
						    guchar *property_exchange_minor)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x30, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;

  /* supported property exchange count */
  supported_property_exchange_count[0] = buffer[5 + nth];
  nth++;

  /* major */
  property_exchange_major[0] = buffer[5 + nth];
  nth++;

  /* minor */
  property_exchange_minor[0] = buffer[5 + nth];
  nth++;

  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_property_exchange_capabilities_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @supported_property_exchange_count: the supported property exchange count
 * @property_exchange_major: the major version
 * @property_exchange_minor: the minor version
 *
 * Put number of supported property exchange message.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_property_exchange_capabilities_reply(AgsMidiCIUtil *midi_ci_util,
							  guchar *buffer,
							  guchar device_id,
							  guchar version,
							  AgsMUID source,
							  AgsMUID destination,
							  guchar supported_property_exchange_count,
							  guchar property_exchange_major,
							  guchar property_exchange_minor)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x31; // Sub-ID#2 - property data exchange capabilities reply

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* supported property exchange count */
  buffer[5 + nth] = supported_property_exchange_count;
  nth++;

  /* major */
  buffer[5 + nth] = property_exchange_major;
  nth++;

  /* minor */
  buffer[5 + nth] = property_exchange_minor;
  nth++;

  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_property_exchange_capabilities_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the return location of device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location of destination
 * @supported_property_exchange_count: (out): the supported property exchange count
 * @property_exchange_major: (out): the return location of major version
 * @property_exchange_minor: (out): the return location of minor version
 *
 * Get number of supported property exchange count data message.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_property_exchange_capabilities_reply(AgsMidiCIUtil *midi_ci_util,
							  guchar *buffer,
							  guchar *device_id,
							  guchar *version,
							  AgsMUID *source,
							  AgsMUID *destination,
							  guchar *supported_property_exchange_count,
							  guchar *property_exchange_major,
							  guchar *property_exchange_minor)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x31, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;

  /* supported property exchange count */
  supported_property_exchange_count[0] = buffer[5 + nth];
  nth++;

  /* major */
  property_exchange_major[0] = buffer[5 + nth];
  nth++;

  /* minor */
  property_exchange_minor[0] = buffer[5 + nth];
  nth++;

  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_get_property_data:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @request_id: the request ID
 * @header_data_length: the header data length
 * @header_data: the header data
 * @chunk_count: the chunk count
 * @nth_chunk: the nth chunk
 * @property_data_length: the property data length
 * @property_data: the property data
 *
 * Put get property data.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_get_property_data(AgsMidiCIUtil *midi_ci_util,
				       guchar *buffer,
				       guchar device_id,
				       guchar version,
				       AgsMUID source,
				       AgsMUID destination,
				       guchar request_id,
				       guint16 header_data_length,
				       guchar *header_data,
				       guint16 chunk_count,
				       guint16 nth_chunk,
				       guint16 property_data_length,
				       guchar *property_data)
{
  guint nth;

  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x34; // Sub-ID#2 - get property data

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* request ID */
  buffer[5 + nth] = request_id;
  nth++;

  /* header data length */
  buffer[5 + nth] = (0xff & header_data_length);
  nth++;
  
  buffer[5 + nth] = (0xff00 & header_data_length) >> 8;
  nth++;

  /* header data */
  if(header_data != NULL &&
     header_data_length > 0){
    memcpy(buffer + 5 + nth, header_data, header_data_length * sizeof(guchar));
    nth += header_data_length;
  }
  
  /* chunk count */
  buffer[5 + nth] = (0xff & chunk_count);
  nth++;
  
  buffer[5 + nth] = (0xff00 & chunk_count) >> 8;
  nth++;

  /* nth chunk */
  buffer[5 + nth] = (0xff & nth_chunk);
  nth++;
  
  buffer[5 + nth] = (0xff00 & nth_chunk) >> 8;
  nth++;

  /* property data length */
  buffer[5 + nth] = (0xff & property_data_length);
  nth++;
  
  buffer[5 + nth] = (0xff00 & property_data_length) >> 8;
  nth++;

  /* property data */
  if(property_data != NULL &&
     property_data_length > 0){
    memcpy(buffer + 5 + nth, property_data, property_data_length * sizeof(guchar));
    nth += property_data_length;
  }
  
  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_get_property_data:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location of destination
 * @request_id: (out): the request ID
 * @header_data_length: (out): the header data length
 * @header_data: (out): the header data
 * @chunk_count: (out): the chunk count
 * @nth_chunk: (out): the nth chunk
 * @property_data_length: (out): the property data length
 * @property_data: (out): the property data
 *
 * Get get property data.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_get_property_data(AgsMidiCIUtil *midi_ci_util,
				       guchar *buffer,
				       guchar *device_id,
				       guchar *version,
				       AgsMUID *source,
				       AgsMUID *destination,
				       guchar *request_id,
				       guint16 *header_data_length,
				       guchar **header_data,
				       guint16 *chunk_count,
				       guint16 *nth_chunk,
				       guint16 *property_data_length,
				       guchar **property_data)
{
  guint nth;
  guint i_stop;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x34, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;

  /* request ID */
  if(request_id != NULL){
    request_id[0] = buffer[5 + nth];
  }

  nth++;

  /* header data length */
  if(header_data_length != NULL){
    header_data_length[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  i_stop = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));

  nth += 2;

  /* header data */
  if(header_data != NULL){
    if(i_stop > 0){
      memcpy(header_data[0], buffer + 5 + nth, i_stop * sizeof(guchar));
    }else{
      header_data[0] = NULL;
    }
  }

  nth += i_stop;

  /* chunk count */
  if(chunk_count != NULL){
    chunk_count[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  nth += 2;

  /* nth chunk */
  if(nth_chunk != NULL){
    nth_chunk[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  nth += 2;

  /* property data length */
  if(property_data_length != NULL){
    property_data_length[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  i_stop = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));

  nth += 2;

  /* property data */
  if(property_data != NULL){
    if(i_stop > 0){
      memcpy(property_data[0], buffer + 5 + nth, i_stop * sizeof(guchar));
    }else{
      property_data[0] = NULL;
    }
  }

  nth += i_stop;

  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_get_property_data_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @request_id: the request ID
 * @header_data_length: the header data length
 * @header_data: the header data
 * @chunk_count: the chunk count
 * @nth_chunk: the nth chunk
 * @property_data_length: the property data length
 * @property_data: the property data
 *
 * Put get property data.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_get_property_data_reply(AgsMidiCIUtil *midi_ci_util,
					     guchar *buffer,
					     guchar device_id,
					     guchar version,
					     AgsMUID source,
					     AgsMUID destination,
					     guchar request_id,
					     guint16 header_data_length,
					     guchar *header_data,
					     guint16 chunk_count,
					     guint16 nth_chunk,
					     guint16 property_data_length,
					     guchar *property_data)
{
  guint nth;

  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x35; // Sub-ID#2 - get property data reply

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* request ID */
  buffer[5 + nth] = request_id;
  nth++;

  /* header data length */
  buffer[5 + nth] = (0xff & header_data_length);
  nth++;
  
  buffer[5 + nth] = (0xff00 & header_data_length) >> 8;
  nth++;

  /* header data */
  memcpy(buffer + 5 + nth, header_data, header_data_length * sizeof(guchar));
  nth += header_data_length;

  /* chunk count */
  buffer[5 + nth] = (0xff & chunk_count);
  nth++;
  
  buffer[5 + nth] = (0xff00 & chunk_count) >> 8;
  nth++;

  /* nth chunk */
  buffer[5 + nth] = (0xff & nth_chunk);
  nth++;
  
  buffer[5 + nth] = (0xff00 & nth_chunk) >> 8;
  nth++;

  /* property data length */
  buffer[5 + nth] = (0xff & property_data_length);
  nth++;
  
  buffer[5 + nth] = (0xff00 & property_data_length) >> 8;
  nth++;

  /* property data */
  memcpy(buffer + 5 + nth, property_data, property_data_length * sizeof(guchar));
  nth += property_data_length;
  
  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_get_property_data_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location of destination
 * @request_id: (out): the return location of request ID
 * @header_data_length: (out): the return location of header data length
 * @header_data: (out): the return location of header data
 * @chunk_count: (out): the return location of chunk count
 * @nth_chunk: (out): the return location of nth chunk
 * @property_data_length: (out): the return location of property data length
 * @property_data: (out): the return location of property data
 *
 * Get get property data.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_get_property_data_reply(AgsMidiCIUtil *midi_ci_util,
					     guchar *buffer,
					     guchar *device_id,
					     guchar *version,
					     AgsMUID *source,
					     AgsMUID *destination,
					     guchar *request_id,
					     guint16 *header_data_length,
					     guchar **header_data,
					     guint16 *chunk_count,
					     guint16 *nth_chunk,
					     guint16 *property_data_length,
					     guchar **property_data)
{
  guint nth;
  guint i_stop;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x35, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;

  /* request ID */
  if(request_id != NULL){
    request_id[0] = buffer[5 + nth];
  }

  nth++;

  /* header data length */
  if(header_data_length != NULL){
    header_data_length[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  i_stop = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));

  nth += 2;

  /* header data */
  if(header_data != NULL){
    if(i_stop > 0){
      memcpy(header_data[0], buffer + 5 + nth, i_stop * sizeof(guchar));
    }else{
      header_data[0] = NULL;
    }
  }

  nth += i_stop;

  /* chunk count */
  if(chunk_count != NULL){
    chunk_count[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  nth += 2;

  /* nth chunk */
  if(nth_chunk != NULL){
    nth_chunk[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  nth += 2;

  /* property data length */
  if(property_data_length != NULL){
    property_data_length[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  i_stop = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));

  nth += 2;

  /* property data */
  if(property_data != NULL){
    if(i_stop > 0){
      memcpy(property_data[0], buffer + 5 + nth, i_stop * sizeof(guchar));
    }else{
      property_data[0] = NULL;
    }
  }

  nth += i_stop;

  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_set_property_data:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @request_id: the request ID
 * @header_data_length: the header data length
 * @header_data: the header data
 * @chunk_count: the chunk count
 * @nth_chunk: the nth chunk
 * @property_data_length: the property data length
 * @property_data: the property data
 *
 * Put set property data.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_set_property_data(AgsMidiCIUtil *midi_ci_util,
				       guchar *buffer,
				       guchar device_id,
				       guchar version,
				       AgsMUID source,
				       AgsMUID destination,
				       guchar request_id,
				       guint16 header_data_length,
				       guchar *header_data,
				       guint16 chunk_count,
				       guint16 nth_chunk,
				       guint16 property_data_length,
				       guchar *property_data)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x36; // Sub-ID#2 - set property data

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* request ID */
  buffer[5 + nth] = request_id;
  nth++;

  /* header data length */
  buffer[5 + nth] = (0xff & header_data_length);
  nth++;
  
  buffer[5 + nth] = (0xff00 & header_data_length) >> 8;
  nth++;

  /* header data */
  memcpy(buffer + 5 + nth, header_data, header_data_length * sizeof(guchar));
  nth += header_data_length;

  /* chunk count */
  buffer[5 + nth] = (0xff & chunk_count);
  nth++;
  
  buffer[5 + nth] = (0xff00 & chunk_count) >> 8;
  nth++;

  /* nth chunk */
  buffer[5 + nth] = (0xff & nth_chunk);
  nth++;
  
  buffer[5 + nth] = (0xff00 & nth_chunk) >> 8;
  nth++;

  /* property data length */
  buffer[5 + nth] = (0xff & property_data_length);
  nth++;
  
  buffer[5 + nth] = (0xff00 & property_data_length) >> 8;
  nth++;

  /* property data */
  memcpy(buffer + 5 + nth, property_data, property_data_length * sizeof(guchar));
  nth += property_data_length;

  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_set_property_data:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location of destination
 * @request_id: (out): the request ID
 * @header_data_length: (out): the header data length
 * @header_data: (out): the header data
 * @chunk_count: (out): the chunk count
 * @nth_chunk: (out): the nth chunk
 * @property_data_length: (out): the property data length
 * @property_data: (out): the property data
 *
 * Get set property data.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_set_property_data(AgsMidiCIUtil *midi_ci_util,
				       guchar *buffer,
				       guchar *device_id,
				       guchar *version,
				       AgsMUID *source,
				       AgsMUID *destination,
				       guchar *request_id,
				       guint16 *header_data_length,
				       guchar **header_data,
				       guint16 *chunk_count,
				       guint16 *nth_chunk,
				       guint16 *property_data_length,
				       guchar **property_data)
{
  guint nth;
  guint i_stop;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x36, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;

  /* request ID */
  if(request_id != NULL){
    request_id[0] = buffer[5 + nth];
  }

  nth++;

  /* header data length */
  if(header_data_length != NULL){
    header_data_length[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  i_stop = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));

  nth += 2;

  /* header data */
  if(header_data != NULL){
    if(i_stop > 0){
      memcpy(header_data, buffer + 5 + nth, i_stop * sizeof(guchar));
    }else{
      header_data[0] = NULL;
    }
  }

  nth += i_stop;

  /* chunk count */
  if(chunk_count != NULL){
    chunk_count[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  nth += 2;

  /* nth chunk */
  if(nth_chunk != NULL){
    nth_chunk[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  nth += 2;

  /* property data length */
  if(property_data_length != NULL){
    property_data_length[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  i_stop = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));

  nth += 2;

  /* property data */
  if(property_data != NULL){
    if(i_stop > 0){
      memcpy(property_data, buffer + 5 + nth, i_stop * sizeof(guchar));
    }else{
      property_data[0] = NULL;
    }
  }

  nth += i_stop;
  
  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_set_property_data_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @request_id: the request ID
 * @header_data_length: the header data length
 * @header_data: the header data
 * @chunk_count: the chunk count
 * @nth_chunk: the nth chunk
 * @property_data_length: the property data length
 * @property_data: the property data
 *
 * Put set property data reply.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_set_property_data_reply(AgsMidiCIUtil *midi_ci_util,
					     guchar *buffer,
					     guchar device_id,
					     guchar version,
					     AgsMUID source,
					     AgsMUID destination,
					     guchar request_id,
					     guint16 header_data_length,
					     guchar *header_data,
					     guint16 chunk_count,
					     guint16 nth_chunk,
					     guint16 property_data_length,
					     guchar *property_data)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x37; // Sub-ID#2 - set property data reply

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* request ID */
  buffer[5 + nth] = request_id;
  nth++;

  /* header data length */
  buffer[5 + nth] = (0xff & header_data_length);
  nth++;
  
  buffer[5 + nth] = (0xff00 & header_data_length) >> 8;
  nth++;

  /* header data */
  memcpy(buffer + 5 + nth, header_data, header_data_length * sizeof(guchar));
  nth += header_data_length;

  /* chunk count */
  buffer[5 + nth] = (0xff & chunk_count);
  nth++;
  
  buffer[5 + nth] = (0xff00 & chunk_count) >> 8;
  nth++;

  /* nth chunk */
  buffer[5 + nth] = (0xff & nth_chunk);
  nth++;
  
  buffer[5 + nth] = (0xff00 & nth_chunk) >> 8;
  nth++;

  /* property data length */
  buffer[5 + nth] = (0xff & property_data_length);
  nth++;
  
  buffer[5 + nth] = (0xff00 & property_data_length) >> 8;
  nth++;

  /* property data */
  memcpy(buffer + 5 + nth, property_data, property_data_length * sizeof(guchar));
  nth += property_data_length;

  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_set_property_data_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location of destination
 * @request_id: (out): the request ID
 * @header_data_length: (out): the header data length
 * @header_data: (out): the header data
 * @chunk_count: (out): the chunk count
 * @nth_chunk: (out): the nth chunk
 * @property_data_length: (out): the property data length
 * @property_data: (out): the property data
 *
 * Get set property data reply.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_set_property_data_reply(AgsMidiCIUtil *midi_ci_util,
					     guchar *buffer,
					     guchar *device_id,
					     guchar *version,
					     AgsMUID *source,
					     AgsMUID *destination,
					     guchar *request_id,
					     guint16 *header_data_length,
					     guchar **header_data,
					     guint16 *chunk_count,
					     guint16 *nth_chunk,
					     guint16 *property_data_length,
					     guchar **property_data)
{
  guint nth;
  guint i_stop;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x37, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;

  /* request ID */
  if(request_id != NULL){
    request_id[0] = buffer[5 + nth];
  }

  nth++;

  /* header data length */
  if(header_data_length != NULL){
    header_data_length[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  i_stop = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));

  nth += 2;

  /* header data */
  if(header_data != NULL){
    if(i_stop > 0){
      memcpy(header_data, buffer + 5 + nth, i_stop * sizeof(guchar));
    }else{
      header_data[0] = NULL;
    }      
  }

  nth += i_stop;

  /* chunk count */
  if(chunk_count != NULL){
    chunk_count[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  nth += 2;

  /* nth chunk */
  if(nth_chunk != NULL){
    nth_chunk[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  nth += 2;

  /* property data length */
  if(property_data_length != NULL){
    property_data_length[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  i_stop = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));

  nth += 2;

  /* property data */
  if(property_data != NULL){
    if(i_stop > 0){
      memcpy(property_data, buffer + 5 + nth, i_stop * sizeof(guchar));
    }else{
      property_data[0] = NULL;
    }
  }

  nth += i_stop;
  
  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_subscription:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @request_id: the request ID
 * @header_data_length: the header data length
 * @header_data: the header data
 * @chunk_count: the chunk count
 * @nth_chunk: the nth chunk
 * @property_data_length: the property data length
 * @property_data: the property data
 *
 * Put subscription.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_subscription(AgsMidiCIUtil *midi_ci_util,
				  guchar *buffer,
				  guchar device_id,
				  guchar version,
				  AgsMUID source,
				  AgsMUID destination,
				  guchar request_id,
				  guint16 header_data_length,
				  guchar *header_data,
				  guint16 chunk_count,
				  guint16 nth_chunk,
				  guint16 property_data_length,
				  guchar *property_data)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x38; // Sub-ID#2 - subscription

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* request ID */
  buffer[5 + nth] = request_id;
  nth++;

  /* header data length */
  buffer[5 + nth] = (0xff & header_data_length);
  nth++;
  
  buffer[5 + nth] = (0xff00 & header_data_length) >> 8;
  nth++;

  /* header data */
  memcpy(buffer + 5 + nth, header_data, header_data_length * sizeof(guchar));
  nth += header_data_length;

  /* chunk count */
  buffer[5 + nth] = (0xff & chunk_count);
  nth++;
  
  buffer[5 + nth] = (0xff00 & chunk_count) >> 8;
  nth++;

  /* nth chunk */
  buffer[5 + nth] = (0xff & nth_chunk);
  nth++;
  
  buffer[5 + nth] = (0xff00 & nth_chunk) >> 8;
  nth++;

  /* property data length */
  buffer[5 + nth] = (0xff & property_data_length);
  nth++;
  
  buffer[5 + nth] = (0xff00 & property_data_length) >> 8;
  nth++;

  /* property data */
  memcpy(buffer + 5 + nth, property_data, property_data_length * sizeof(guchar));
  nth += property_data_length;

  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_subscription:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location of destination
 * @request_id: (out): the request ID
 * @header_data_length: (out): the header data length
 * @header_data: (out): the header data
 * @chunk_count: (out): the chunk count
 * @nth_chunk: (out): the nth chunk
 * @property_data_length: (out): the property data length
 * @property_data: (out): the property data
 *
 * Get subscription.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_subscription(AgsMidiCIUtil *midi_ci_util,
				  guchar *buffer,
				  guchar *device_id,
				  guchar *version,
				  AgsMUID *source,
				  AgsMUID *destination,
				  guchar *request_id,
				  guint16 *header_data_length,
				  guchar **header_data,
				  guint16 *chunk_count,
				  guint16 *nth_chunk,
				  guint16 *property_data_length,
				  guchar **property_data)
{
  guint nth;
  guint i_stop;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x38, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;

  /* request ID */
  if(request_id != NULL){
    request_id[0] = buffer[5 + nth];
  }

  nth++;

  /* header data length */
  if(header_data_length != NULL){
    header_data_length[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  i_stop = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));

  nth += 2;

  /* header data */
  if(header_data != NULL){
    if(i_stop > 0){
      memcpy(header_data[0], buffer + 5 + nth, i_stop * sizeof(guchar));
    }else{
      header_data[0] = NULL;
    }
  }

  nth += i_stop;

  /* chunk count */
  if(chunk_count != NULL){
    chunk_count[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  nth += 2;

  /* nth chunk */
  if(nth_chunk != NULL){
    nth_chunk[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  nth += 2;

  /* property data length */
  if(property_data_length != NULL){
    property_data_length[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  i_stop = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));

  nth += 2;

  /* property data */
  if(property_data != NULL){
    if(i_stop > 0){
      memcpy(property_data[0], buffer + 5 + nth, i_stop * sizeof(guchar));
    }else{
      property_data[0] = NULL;
    }
  }

  nth += i_stop;
  
  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_subscription_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @request_id: the request ID
 * @header_data_length: the header data length
 * @header_data: the header data
 * @chunk_count: the chunk count
 * @nth_chunk: the nth chunk
 * @property_data_length: the property data length
 * @property_data: the property data
 *
 * Put subscription reply.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_subscription_reply(AgsMidiCIUtil *midi_ci_util,
					guchar *buffer,
					guchar device_id,
					guchar version,
					AgsMUID source,
					AgsMUID destination,
					guchar request_id,
					guint16 header_data_length,
					guchar *header_data,
					guint16 chunk_count,
					guint16 nth_chunk,
					guint16 property_data_length,
					guchar *property_data)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x39; // Sub-ID#2 - subscription reply

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* request ID */
  buffer[5 + nth] = request_id;
  nth++;

  /* header data length */
  buffer[5 + nth] = (0xff & header_data_length);
  nth++;
  
  buffer[5 + nth] = (0xff00 & header_data_length) >> 8;
  nth++;

  /* header data */
  memcpy(buffer + 5 + nth, header_data, header_data_length * sizeof(guchar));
  nth += header_data_length;

  /* chunk count */
  buffer[5 + nth] = (0xff & chunk_count);
  nth++;
  
  buffer[5 + nth] = (0xff00 & chunk_count) >> 8;
  nth++;

  /* nth chunk */
  buffer[5 + nth] = (0xff & nth_chunk);
  nth++;
  
  buffer[5 + nth] = (0xff00 & nth_chunk) >> 8;
  nth++;

  /* property data length */
  buffer[5 + nth] = (0xff & property_data_length);
  nth++;
  
  buffer[5 + nth] = (0xff00 & property_data_length) >> 8;
  nth++;

  /* property data */
  memcpy(buffer + 5 + nth, property_data, property_data_length * sizeof(guchar));
  nth += property_data_length;

  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_subscription_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location of destination
 * @request_id: (out): the request ID
 * @header_data_length: (out): the header data length
 * @header_data: (out): the header data
 * @chunk_count: (out): the chunk count
 * @nth_chunk: (out): the nth chunk
 * @property_data_length: (out): the property data length
 * @property_data: (out): the property data
 *
 * Get subscription reply.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_subscription_reply(AgsMidiCIUtil *midi_ci_util,
					guchar *buffer,
					guchar *device_id,
					guchar *version,
					AgsMUID *source,
					AgsMUID *destination,
					guchar *request_id,
					guint16 *header_data_length,
					guchar **header_data,
					guint16 *chunk_count,
					guint16 *nth_chunk,
					guint16 *property_data_length,
					guchar **property_data)
{
  guint nth;
  guint i_stop;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x39, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;

  /* request ID */
  if(request_id != NULL){
    request_id[0] = buffer[5 + nth];
  }

  nth++;

  /* header data length */
  if(header_data_length != NULL){
    header_data_length[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  i_stop = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));

  nth += 2;

  /* header data */
  if(header_data != NULL){
    if(i_stop > 0){
      memcpy(header_data, buffer + 5 + nth, i_stop * sizeof(guchar));
    }else{
      header_data[0] = NULL;
    }
  }

  nth += i_stop;

  /* chunk count */
  if(chunk_count != NULL){
    chunk_count[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  nth += 2;

  /* nth chunk */
  if(nth_chunk != NULL){
    nth_chunk[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  nth += 2;

  /* property data length */
  if(property_data_length != NULL){
    property_data_length[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));
  }

  i_stop = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8));

  nth += 2;

  /* property data */
  if(property_data != NULL){
    if(i_stop > 0){
      memcpy(property_data[0], buffer + 5 + nth, i_stop * sizeof(guchar));
    }else{
      property_data[0] = NULL;
    }      
  }

  nth += i_stop;
  
  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_process_capabilities:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 *
 * Put process capabilities.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_process_capabilities(AgsMidiCIUtil *midi_ci_util,
					  guchar *buffer,
					  guchar device_id,
					  guchar version,
					  AgsMUID source,
					  AgsMUID destination)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x40; // Sub-ID#2 - process capabilities

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_process_capabilities:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location of destination
 *
 * Get process capabilities.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_process_capabilities(AgsMidiCIUtil *midi_ci_util,
					  guchar *buffer,
					  guchar *device_id,
					  guchar *version,
					  AgsMUID *source,
					  AgsMUID *destination)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x40, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;
  
  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_process_capabilities_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @supported_features: the supported features
 *
 * Put process capabilities reply.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_process_capabilities_reply(AgsMidiCIUtil *midi_ci_util,
						guchar *buffer,
						guchar device_id,
						guchar version,
						AgsMUID source,
						AgsMUID destination,
						guchar supported_features)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x41; // Sub-ID#2 - process capabilities reply

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* supported features */
  buffer[5 + nth] = supported_features;

  nth++;

  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_process_capabilities_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location of destination
 * @supported_features: (out): the return location of supported features
 *
 * Get process capabilities reply.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_process_capabilities_reply(AgsMidiCIUtil *midi_ci_util,
						guchar *buffer,
						guchar *device_id,
						guchar *version,
						AgsMUID *source,
						AgsMUID *destination,
						guchar *supported_features)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x41, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;

  /* supported features */
  if(supported_features != NULL){
    supported_features[0] = buffer[5 + nth];
  }

  nth++;
  
  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_message_report:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @data_control: the data control
 * @system_messages: the system messages
 * @other_messages: the other messages
 * @channel_controller_messages: the channel controller messages
 * @note_data_messages: the note data messages
 *
 * Put message report.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_message_report(AgsMidiCIUtil *midi_ci_util,
				    guchar *buffer,
				    guchar device_id,
				    guchar version,
				    AgsMUID source,
				    AgsMUID destination,
				    guchar data_control,
				    guchar system_messages,
				    guchar other_messages,
				    guchar channel_controller_messages,
				    guchar note_data_messages)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x42; // Sub-ID#2 - message report

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* data control */
  buffer[5 + nth] = data_control;

  nth++;

  /* system messages */
  buffer[5 + nth] = system_messages;

  nth++;

  /* other messages */
  buffer[5 + nth] = other_messages;

  nth++;

  /* channel controller messages */
  buffer[5 + nth] = channel_controller_messages;

  nth++;

  /* note data messages */
  buffer[5 + nth] = note_data_messages;

  nth++;

  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_message_report:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location of destination
 * @data_control: (out): the return location of data control
 * @system_messages: (out): the return location of system messages
 * @other_messages: (out): the return location of other system messages
 * @channel_controller_messages: (out): the return location of channel controller messages
 * @note_data_messages: (out): the return location of note data messages
 *
 * Get message report.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_message_report(AgsMidiCIUtil *midi_ci_util,
				    guchar *buffer,
				    guchar *device_id,
				    guchar *version,
				    AgsMUID *source,
				    AgsMUID *destination,
				    guchar *data_control,
				    guchar *system_messages,
				    guchar *other_messages,
				    guchar *channel_controller_messages,
				    guchar *note_data_messages)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x42, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;

  /* data control */
  if(data_control != NULL){
    data_control[0] = buffer[5 + nth];
  }

  nth++;

  /* system messages */
  if(system_messages != NULL){
    system_messages[0] = buffer[5 + nth];
  }

  nth++;

  /* other essages */
  if(other_messages != NULL){
    other_messages[0] = buffer[5 + nth];
  }

  nth++;

  /* channel controller messages */
  if(channel_controller_messages != NULL){
    channel_controller_messages[0] = buffer[5 + nth];
  }

  nth++;

  /* note data messages */
  if(note_data_messages != NULL){
    note_data_messages[0] = buffer[5 + nth];
  }

  nth++;
  
  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_message_report_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 * @system_messages: the system messages
 * @other_messages: the other messages
 * @channel_controller_messages: the channel controller messages
 * @note_data_messages: the note data messages
 *
 * Put message report reply.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_message_report_reply(AgsMidiCIUtil *midi_ci_util,
					  guchar *buffer,
					  guchar device_id,
					  guchar version,
					  AgsMUID source,
					  AgsMUID destination,
					  guchar system_messages,
					  guchar other_messages,
					  guchar channel_controller_messages,
					  guchar note_data_messages)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x43; // Sub-ID#2 - message report

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* system messages */
  buffer[5 + nth] = system_messages;

  nth++;

  /* other messages */
  buffer[5 + nth] = other_messages;

  nth++;

  /* channel controller messages */
  buffer[5 + nth] = channel_controller_messages;

  nth++;

  /* note data messages */
  buffer[5 + nth] = note_data_messages;

  nth++;

  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_message_report_reply:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location of destination
 * @system_messages: (out): the return location of system messages
 * @other_messages: (out): the return location of other system messages
 * @channel_controller_messages: (out): the return location of channel controller messages
 * @note_data_messages: (out): the return location of note data messages
 *
 * Get message report reply.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_message_report_reply(AgsMidiCIUtil *midi_ci_util,
					  guchar *buffer,
					  guchar *device_id,
					  guchar *version,
					  AgsMUID *source,
					  AgsMUID *destination,
					  guchar *system_messages,
					  guchar *other_messages,
					  guchar *channel_controller_messages,
					  guchar *note_data_messages)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x43, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;

  /* system messages */
  if(system_messages != NULL){
    system_messages[0] = buffer[5 + nth];
  }

  nth++;

  /* other essages */
  if(other_messages != NULL){
    other_messages[0] = buffer[5 + nth];
  }

  nth++;

  /* channel controller messages */
  if(channel_controller_messages != NULL){
    channel_controller_messages[0] = buffer[5 + nth];
  }

  nth++;

  /* note data messages */
  if(note_data_messages != NULL){
    note_data_messages[0] = buffer[5 + nth];
  }

  nth++;

  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}

/**
 * ags_midi_ci_util_put_end_of_message_report:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: the device ID
 * @version: the version
 * @source: the source
 * @destination: the destination
 *
 * Put message report.
 *
 * Since: 5.5.0
 */
void
ags_midi_ci_util_put_end_of_message_report(AgsMidiCIUtil *midi_ci_util,
					   guchar *buffer,
					   guchar device_id,
					   guchar version,
					   AgsMUID source,
					   AgsMUID destination)
{
  guint nth;
  
  g_return_if_fail(midi_ci_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = device_id;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x44; // Sub-ID#2 - message report

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  nth++;
  
  /* source */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);
  nth += 4;

  /* destination */
  ags_midi_ci_util_put_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);
  nth += 4;

  /* sysex end */
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_util_get_end_of_message_report:
 * @midi_ci_util: the MIDI CI util
 * @buffer: the buffer
 * @device_id: (out): the device ID
 * @version: (out): the return location of version
 * @source: (out): the return location of source
 * @destination: (out): the return location of destination
 *
 * Get message report.
 *
 * Returns: the number of bytes read
 * 
 * Since: 5.5.0
 */
guint
ags_midi_ci_util_get_end_of_message_report(AgsMidiCIUtil *midi_ci_util,
					   guchar *buffer,
					   guchar *device_id,
					   guchar *version,
					   AgsMUID *source,
					   AgsMUID *destination)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_util != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail(buffer[1] == 0x7e, 0);
  g_return_val_if_fail(buffer[3] == 0x0d, 0);
  g_return_val_if_fail(buffer[4] == 0x44, 0);

  /* device ID */
  if(device_id != NULL){
    device_id[0] = buffer[2];
  }

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    source);

  nth += 4;

  /* destination */
  ags_midi_ci_util_get_muid(midi_ci_util,
			    buffer + 5 + nth,
			    destination);

  nth += 4;

  /* sysex end */
  if(buffer[5 + nth] == 0xf7){
    nth++;

    return(5 + nth);
  }

  return(0);
}
