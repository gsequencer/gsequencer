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

#include <ags/audio/midi/ags_midi_ci_1_1_util.h>

#include <glib.h>
#include <glib-object.h>

#include <json-glib/json-glib.h>

/**
 * SECTION:ags_midi_ci_1_1_util
 * @short_description: MIDI CI version 1.1 util
 * @title: AgsMidiUtil
 * @section_id:
 * @include: ags/audio/midi/ags_midi_ci_1_1_util.h
 *
 * Utility functions for MIDI CI version 1.1.
 */

GType
ags_midi_ci_1_1_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_midi_ci_1_1_util = 0;

    ags_type_midi_ci_1_1_util =
      g_boxed_type_register_static("AgsMidiCI_1_1_Util",
				   (GBoxedCopyFunc) ags_midi_ci_1_1_util_copy,
				   (GBoxedFreeFunc) ags_midi_ci_1_1_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_midi_ci_1_1_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_midi_ci_1_1_util_alloc:
 *
 * Allocate MIDI CI version 1.1 util.
 *
 * Returns: (transfer full): the newly allocated #AgsMidiCI_1_1_Util-struct
 * 
 * Since: 5.4.4
 */
AgsMidiCI_1_1_Util*
ags_midi_ci_1_1_util_alloc()
{
  AgsMidiCI_1_1_Util *midi_ci_1_1_util;

  midi_ci_1_1_util = g_new0(AgsMidiCI_1_1_Util,
			    1);

  midi_ci_1_1_util->rand = g_rand_new();
  
  return(midi_ci_1_1_util);
}

/**
 * ags_midi_ci_1_1_util_free:
 * @midi_ci_1_1_util: the MIDI CI util
 *
 * Free MIDI CI version 1.1 util.
 *
 * Since: 5.4.4
 */
void
ags_midi_ci_1_1_util_free(AgsMidiCI_1_1_Util *midi_ci_1_1_util)
{
  g_return_if_fail(midi_ci_1_1_util != NULL);
  
  g_free(midi_ci_1_1_util);
}

/**
 * ags_midi_ci_1_1_util_copy:
 * @midi_ci_1_1_util: the MIDI CI util
 *
 * Copy MIDI CI version 1.1 util.
 *
 * Returns: (transfer full): the newly allocated #AgsMidiCI_1_1_Util-struct
 * 
 * Since: 5.4.4
 */
AgsMidiCI_1_1_Util*
ags_midi_ci_1_1_util_copy(AgsMidiCI_1_1_Util *midi_ci_1_1_util)
{
  AgsMidiCI_1_1_Util *retval;

  g_return_val_if_fail(midi_ci_1_1_util != NULL, NULL);

  retval = ags_midi_ci_1_1_util_alloc();
  
  return(retval);
}

/**
 * ags_midi_ci_1_1_util_generate_muid:
 * @midi_ci_1_1_util: the MIDI CI util
 *
 * Generate MUID.
 *
 * Returns: the generated MUID
 * 
 * Since: 5.4.4
 */
AgsMUID
ags_midi_ci_1_1_util_generate_muid(AgsMidiCI_1_1_Util *midi_ci_1_1_util)
{
  AgsMUID muid;

  g_return_val_if_fail(midi_ci_1_1_util != NULL, 0);

  muid = g_rand_int_range(midi_ci_1_1_util->rand,
			  0,
			  exp2(28));

  return(muid);
}

/**
 * ags_midi_ci_1_1_util_put_discovery:
 * @midi_ci_1_1_util: the MIDI CI util
 * @buffer: the buffer
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
 * Since: 5.4.4
 */
void
ags_midi_ci_1_1_util_put_discovery(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
				   guchar *buffer,
				   guchar version,
				   AgsMUID source,
				   guint32 manufacturer_id,
				   guint32 device_family,
				   guint32 device_family_model_number,
				   guint32 software_revision_level,
				   guchar capability,
				   guint32 max_sysex_message_size)
{
  guint nth;
  
  g_return_if_fail(midi_ci_1_1_util != NULL);
  g_return_if_fail(buffer != NULL);

  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = 0x7f;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x70; // Sub-ID#2 - discovery

  nth = 0;

  /* version */
  buffer[5 + nth] = version;
  nth++;

  /* source */
  buffer[5 + nth] = (0xff & source);
  nth++;
  
  buffer[5 + nth] = (0xff00 & source) >> 8;
  nth++;
  
  buffer[5 + nth] = (0xff0000 & source) >> 16;
  nth++;
  
  buffer[5 + nth] = (0xff000000 & source) >> 24;
  nth++;

  /* broadcast */
  buffer[5 + nth] = 0x0f;
  nth++;

  buffer[5 + nth] = 0xff;
  nth++;

  buffer[5 + nth] = 0xff;
  nth++;

  buffer[5 + nth] = 0xff;
  nth++;

  /* manufacturer */
  buffer[5 + nth] = (0xff & manufacturer_id);
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
  buffer[5 + nth] = (0xff & software_revision_level);
  nth++;
  
  buffer[5 + nth] = (0xff00 & device_family_model_number) >> 8;
  nth++;
  
  buffer[5 + nth] = (0xff00 & device_family_model_number) >> 16;
  nth++;
  
  buffer[5 + nth] = (0xff00 & device_family_model_number) >> 24;
  nth++;

  /* capability */
  buffer[5 + nth] = capability;
  nth++;

  /* maximum sysex message size */
  buffer[5 + nth] = (0xff & max_sysex_message_size);
  nth++;
  
  buffer[5 + nth] = (0xff & max_sysex_message_size) >> 8;
  nth++;

  buffer[5 + nth] = (0xff & max_sysex_message_size) >> 16;
  nth++;

  buffer[5 + nth] = (0xff & max_sysex_message_size) >> 24;
  nth++;
  
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_1_1_util_get_discovery:
 * @midi_ci_1_1_util: the MIDI CI util
 * @buffer: the buffer
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
 * @Returns: the number of bytes read
 * 
 * Since: 5.4.4
 */
guint
ags_midi_ci_1_1_util_get_discovery(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
				   guchar *buffer,
				   guchar *version,
				   AgsMUID *source,
				   guint32 *manufacturer_id,
				   guint32 *device_family,
				   guint32 *device_family_model_number,
				   guint32 *software_revision_level,
				   guchar *capability,
				   guint32 *max_sysex_message_size)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_1_1_util != NULL, 0);
  g_return_val_if_fail(buffer[0] != 0xf0, 0);
  g_return_val_if_fail(buffer[1] != 0x7e, 0);
  g_return_val_if_fail(buffer[2] != 0x7f, 0);
  g_return_val_if_fail(buffer[3] != 0x0d, 0);
  g_return_val_if_fail(buffer[4] != 0x70, 0);
  g_return_val_if_fail(buffer[9] != 0x0f || buffer[10] != 0xff || buffer[11] != 0xff || buffer[12] != 0xff, 0);

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  if(source != NULL){
    source[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8) | (buffer[5 + nth + 2] << 16) | (buffer[5 + nth + 3] << 24));
  }

  nth += 4;

  /* destination - broadcast */
  //NOTE:JK: validate first - see top of function
  
  /* manufacturer */
  if(manufacturer_id != NULL){
    manufacturer_id[0] = buffer[5 + nth];
  }

  nth++;

  /* device family */
  if(device_family != NULL){
    device_family[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] >> 8));
  }
  
  nth += 2;

  /* device family model number */
  if(device_family != NULL){
    device_family_model_number[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] >> 8));
  }
  
  nth += 2;

  /* software revision level */
  if(software_revision_level != NULL){
    device_family_model_number[0] = (buffer[5 + nth]) | (buffer[5 + nth + 1]) | (buffer[5 + nth + 2]) | (buffer[5 + nth + 3]);
  }

  nth += 4;
  
  /* capability */
  buffer[5 + nth] = capability;
  nth++;

  /* maximum sysex message size */
  if(max_sysex_message_size != NULL){
    max_sysex_message_size[0] = (buffer[5 + nth]) | (buffer[5 + nth + 1]) | (buffer[5 + nth + 2]) | (buffer[5 + nth + 3]);
  }

  nth += 4;
  
  buffer[5 + nth] = 0xf7;
  nth++;
    
  return(5 + nth);
}

/**
 * ags_midi_ci_1_1_util_put_discovery_reply:
 * @midi_ci_1_1_util: the MIDI CI util
 * @buffer: the buffer
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
 * Since: 5.4.4
 */
void
ags_midi_ci_1_1_util_put_discovery_reply(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
					 guchar *buffer,
					 guchar version,
					 AgsMUID source,
					 AgsMUID destination,
					 guint32 manufacturer_id,
					 guint32 device_family,
					 guint32 device_family_model_number,
					 guint32 software_revision_level,
					 guchar capability,
					 guint32 max_sysex_message_size)
{
  guint nth;
  
  g_return_if_fail(midi_ci_1_1_util != NULL);
  g_return_if_fail(buffer != NULL);

  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = 0x7f;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x71; // Sub-ID#2 - discovery reply

  nth = 0;

  /* version */
  buffer[5 + nth] = version;

  /* source */
  buffer[5 + nth] = (0xff & source);
  nth++;
  
  buffer[5 + nth] = (0xff00 & source) >> 8;
  nth++;
  
  buffer[5 + nth] = (0xff0000 & source) >> 16;
  nth++;
  
  buffer[5 + nth] = (0xff000000 & source) >> 24;
  nth++;

  /* destination */
  buffer[5 + nth] = (0xff & destination);
  nth++;
  
  buffer[5 + nth] = (0xff00 & destination) >> 8;
  nth++;
  
  buffer[5 + nth] = (0xff0000 & destination) >> 16;
  nth++;
  
  buffer[5 + nth] = (0xff000000 & destination) >> 24;
  nth++;
  
  /* manufacturer */
  buffer[5 + nth] = (0xff & manufacturer_id);
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
  buffer[5 + nth] = (0xff & software_revision_level);
  nth++;
  
  buffer[5 + nth] = (0xff00 & device_family_model_number) >> 8;
  nth++;
  
  buffer[5 + nth] = (0xff00 & device_family_model_number) >> 16;
  nth++;
  
  buffer[5 + nth] = (0xff00 & device_family_model_number) >> 24;
  nth++;

  /* capability */
  buffer[5 + nth] = capability;
  nth++;

  /* maximum sysex message size */
  buffer[5 + nth] = (0xff & max_sysex_message_size);
  nth++;
  
  buffer[5 + nth] = (0xff & max_sysex_message_size) >> 8;
  nth++;

  buffer[5 + nth] = (0xff & max_sysex_message_size) >> 16;
  nth++;

  buffer[5 + nth] = (0xff & max_sysex_message_size) >> 24;
  nth++;
  
  buffer[5 + nth] = 0xf7;
  nth++;
}

/**
 * ags_midi_ci_1_1_util_get_discovery_reply:
 * @midi_ci_1_1_util: the MIDI CI util
 * @buffer: the buffer
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
 * @Returns: the number of bytes read
 * 
 * Since: 5.4.4
 */
guint
ags_midi_ci_1_1_util_get_discovery_reply(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
					 guchar *buffer,
					 guchar *version,
					 AgsMUID *source,
					 AgsMUID *destination,
					 guint32 *manufacturer_id,
					 guint32 *device_family,
					 guint32 *device_family_model_number,
					 guint32 *software_revision_level,
					 guchar *capability,
					 guint32 *max_sysex_message_size)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_1_1_util != NULL, 0);
  g_return_val_if_fail(buffer[0] != 0xf0, 0);
  g_return_val_if_fail(buffer[1] != 0x7e, 0);
  g_return_val_if_fail(buffer[2] != 0x7f, 0);
  g_return_val_if_fail(buffer[3] != 0x0d, 0);
  g_return_val_if_fail(buffer[4] != 0x71, 0);

  nth = 0;

  /* version */
  if(version != NULL){
    version[0] = buffer[5 + nth];
  }

  nth++;
  
  /* source */
  if(source != NULL){
    source[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8) | (buffer[5 + nth + 2] << 16) | (buffer[5 + nth + 3] << 24));
  }

  nth += 4;

  /* destination */
  if(destination != NULL){
    destination[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] << 8) | (buffer[5 + nth + 2] << 16) | (buffer[5 + nth + 3] << 24));
  }

  nth += 4;
  
  /* manufacturer */
  if(manufacturer_id != NULL){
    manufacturer_id[0] = buffer[5 + nth];
  }

  nth++;

  /* device family */
  if(device_family != NULL){
    device_family[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] >> 8));
  }
  
  nth += 2;

  /* device family model number */
  if(device_family != NULL){
    device_family_model_number[0] = ((buffer[5 + nth]) | (buffer[5 + nth + 1] >> 8));
  }
  
  nth += 2;

  /* software revision level */
  if(software_revision_level != NULL){
    device_family_model_number[0] = (buffer[5 + nth]) | (buffer[5 + nth + 1]) | (buffer[5 + nth + 2]) | (buffer[5 + nth + 3]);
  }

  nth += 4;
  
  /* capability */
  buffer[5 + nth] = capability;
  nth++;

  /* maximum sysex message size */
  if(max_sysex_message_size != NULL){
    max_sysex_message_size[0] = (buffer[5 + nth]) | (buffer[5 + nth + 1]) | (buffer[5 + nth + 2]) | (buffer[5 + nth + 3]);
  }

  nth += 4;
  
  buffer[5 + nth] = 0xf7;
  nth++;
    
  return(5 + nth);
}

/**
 * ags_midi_ci_1_1_util_put_invalidate_muid:
 * @midi_ci_1_1_util: the MIDI CI util
 * @buffer: the buffer
 * @source: the source
 * @destination: the destination
 * @muid: the MUID
 *
 * Put invalidate MUID.
 *
 * Since: 5.4.4
 */
void
ags_midi_ci_1_1_util_put_invalidate_muid(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
					 guchar *buffer,
					 AgsMUID source,
					 AgsMUID destination,
					 AgsMUID muid)
{
  guint nth;
  
  g_return_if_fail(midi_ci_1_1_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = 0x7f;

  buffer[3] = 0x0d; // Sub-ID#1 - MIDI-CI

  buffer[4] = 0x7e; // Sub-ID#2 - invalidate MUID

  //TODO:JK: implement me
}

/**
 * ags_midi_ci_1_1_util_get_invalidate_muid:
 * @midi_ci_1_1_util: the MIDI CI util
 * @buffer: the buffer
 * @source: the return location source
 * @destination: the return location destination
 * @muid: the return location of MUID
 *
 * Get invalidate MUID.
 *
 * @Returns: the number of bytes read
 * 
 * Since: 5.4.4
 */
guint
ags_midi_ci_1_1_util_get_invalidate_muid(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
					 guchar *buffer,
					 AgsMUID *source,
					 AgsMUID *destination,
					 AgsMUID *muid)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_1_1_util != NULL, 0);
  g_return_val_if_fail(buffer[0] != 0xf0, 0);
  g_return_val_if_fail(buffer[1] != 0x7e, 0);
  g_return_val_if_fail(buffer[2] != 0x7f, 0);
  g_return_val_if_fail(buffer[3] != 0x0d, 0);
  g_return_val_if_fail(buffer[4] != 0x7e, 0);

  nth = 0;

  //TODO:JK: implement me
    
  return(5 + nth);
}
