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

#include <ags/audio/midi/ags_midi_ump_util.h>

#include <glib.h>
#include <glib-object.h>

#include <json-glib/json-glib.h>

#include <string.h>

void ags_midi_ump_util_put_flex_set_text(AgsMidiUmpUtil *midi_ump_util,
					 guchar *buffer,
					 gint group,
					 gint channel,
					 gint status_bank,
					 gint status,
					 gchar *text,
					 gchar **extension_name, GValue *extension_value,
					 guint extension_count);
guint ags_midi_ump_util_get_flex_set_text(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gint *group,
					  gint *channel,
					  gint *status_bank,
					  gint *status,
					  gchar **text,
					  gchar ***extension_name, GValue **extension_value,
					  guint *extension_count);

/**
 * SECTION:ags_midi_ump_util
 * @short_description: MIDI UMP util
 * @title: AgsMidiUtil
 * @section_id:
 * @include: ags/audio/midi/ags_midi_ump_util.h
 *
 * Utility functions for MIDI UMP messages.
 */

GType
ags_midi_ump_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_midi_ump_util = 0;

    ags_type_midi_ump_util =
      g_boxed_type_register_static("AgsMidiUmpUtil",
				   (GBoxedCopyFunc) ags_midi_ump_util_copy,
				   (GBoxedFreeFunc) ags_midi_ump_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_midi_ump_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_midi_ump_util_alloc:
 *
 * Allocate MIDI UMP util.
 *
 * Returns: (transfer full): the newly allocated #AgsMidiUmpUtil-struct
 * 
 * Since: 5.5.4
 */
AgsMidiUmpUtil*
ags_midi_ump_util_alloc()
{
  AgsMidiUmpUtil *midi_ump_util;

  midi_ump_util = g_new0(AgsMidiUmpUtil,
			 1);

  midi_ump_util->major = 2;
  midi_ump_util->minor = 0;
  
  return(midi_ump_util);
}

/**
 * ags_midi_ump_util_free:
 * @midi_ump_util: the MIDI UMP util
 *
 * Free MIDI UMP util.
 *
 * Since: 5.5.4
 */
void
ags_midi_ump_util_free(AgsMidiUmpUtil *midi_ump_util)
{
  g_return_if_fail(midi_ump_util != NULL);
  
  g_free(midi_ump_util);
}

/**
 * ags_midi_ump_util_copy:
 * @midi_ump_util: the MIDI UMP util
 *
 * Copy MIDI UMP util.
 *
 * Returns: (transfer full): the newly allocated #AgsMidiUmpUtil-struct
 * 
 * Since: 5.5.4
 */
AgsMidiUmpUtil*
ags_midi_ump_util_copy(AgsMidiUmpUtil *midi_ump_util)
{
  AgsMidiUmpUtil *ptr;

  g_return_val_if_fail(midi_ump_util != NULL, NULL);

  ptr = ags_midi_ump_util_alloc();

  ptr->major = midi_ump_util->major;
  ptr->minor = midi_ump_util->minor;
  
  return(ptr);
}

/**
 * ags_midi_ump_util_is_system_common:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is system common.
 * 
 * Returns: %TRUE if is system common, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_system_common(AgsMidiUmpUtil *midi_ump_util,
				   guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x10){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_is_data_message_with_sysex:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is data message with SYSEX.
 * 
 * Returns: %TRUE if is data message with SYSEX, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_data_message_with_sysex(AgsMidiUmpUtil *midi_ump_util,
					     guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x30){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_is_data_message:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is data message.
 * 
 * Returns: %TRUE if is data message, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_data_message(AgsMidiUmpUtil *midi_ump_util,
				  guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x50){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_is_stream_message:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is stream message.
 * 
 * Returns: %TRUE if is stream message, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_stream_message(AgsMidiUmpUtil *midi_ump_util,
				    guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xf0){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_is_endpoint_discovery:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is endpoint discovery.
 * 
 * Returns: %TRUE if is endpoint discovery, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_endpoint_discovery(AgsMidiUmpUtil *midi_ump_util,
					guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xf0 &&
     ((0x300 & (buffer[0] << 8)) | (0xff & (buffer[1]))) == 0x00){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_endpoint_discovery:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @major: the major
 * @minor: the minor
 * @filter: the filter
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put endpoint discovery.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_endpoint_discovery(AgsMidiUmpUtil *midi_ump_util,
					 guchar *buffer,
					 gint major,
					 gint minor,
					 gint filter,
					 gchar **extension_name, GValue *extension_value,
					 guint extension_count)
{  
  guint nth;
  const gint status = 0x0;
  const gint format = 0x0;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0) | ((0x03 & format) << 2) | ((0x300 & status) >> 8);
  nth++;

  buffer[nth] = (0xff & status);
  nth++;

  buffer[nth] = (0xff & major);
  nth++;

  buffer[nth] = (0xff & minor);
  nth++;

  /* reserved */
  memset(buffer + nth, 0, 3 * sizeof(guchar));
  nth += 3;

  /* filter bitmap */
  buffer[nth] = (0xff & filter);
  nth++;

  /* reserved */
  memset(buffer + nth, 0, 8 * sizeof(guchar));
  nth += 8;
}

/**
 * ags_midi_ump_util_get_endpoint_discovery:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @major: (out): the return location of major
 * @minor: (out): the return location of minor
 * @filter: (out): the return location of filter
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get endpoint discovery.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4
 */
guint
ags_midi_ump_util_get_endpoint_discovery(AgsMidiUmpUtil *midi_ump_util,
					 guchar *buffer,
					 gint *major,
					 gint *minor,
					 gint *filter,
					 gchar ***extension_name, GValue **extension_value,
					 guint *extension_count)
{
  guint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0xf0, 0);
  g_return_val_if_fail(((0x300 & (buffer[0] << 8)) | (0xff & (buffer[1]))) == 0x0, 0);

  nth = 2;

  /* major */
  if(major != NULL){
    major[0] = buffer[nth];
  }

  nth++;

  /* minor */
  if(minor != NULL){
    minor[0] = buffer[nth];
  }

  nth++;

  /* reserved */
  nth += 3;

  /* filter bitmap */
  if(filter != NULL){
    filter[0] = buffer[nth];
  }

  nth++;

  /* reserved */
  nth += 8;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_endpoint_info_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is endpoint info notification.
 * 
 * Returns: %TRUE if is endpoint info notification, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_endpoint_info_notification(AgsMidiUmpUtil *midi_ump_util,
						guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xf0 &&
     (0x0c & (buffer[0])) == 0x0 &&
     ((0x300 & (buffer[0] << 8)) | (0xff & (buffer[1]))) == 0x01){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_endpoint_info_notifcation:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @major: the major
 * @minor: the minor
 * @static_function_blocks: the static function blocks
 * @function_block_count: the function block count
 * @midi_v2_0_support: has MIDI v2.0 support
 * @midi_v1_0_support: has MIDI v1.0 support
 * @rx_jitter_reduction: the receiving jitter reduction
 * @tx_jitter_reduction: the transfering jitter reduction
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put endpoint info notification.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_endpoint_info_notification(AgsMidiUmpUtil *midi_ump_util,
						 guchar *buffer,
						 gint major,
						 gint minor,
						 gboolean static_function_blocks,
						 gint function_block_count,
						 gboolean midi_v2_0_support,
						 gboolean midi_v1_0_support,
						 gboolean rx_jitter_reduction,
						 gboolean tx_jitter_reduction,
						 gchar **extension_name, GValue *extension_value,
						 guint extension_count)
{  
  guint nth;
  const gint status = 0x1;
  const gint format = 0x0;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0) | ((0x03 & format) << 2) | ((0x300 & status) >> 8);
  nth++;

  buffer[nth] = (0xff & status);
  nth++;

  buffer[nth] = (0xff & major);
  nth++;

  buffer[nth] = (0xff & minor);
  nth++;

  /* function block */
  buffer[nth] = 0x00;
  
  if(static_function_blocks){
    buffer[nth] = 0x80;
  }

  buffer[nth] |= (0x7f & (function_block_count));
  nth++;

  /* reserved */
  buffer[nth] = 0x00;
  nth++;
  
  /* MIDI support */
  buffer[nth] = 0x00;

  if(midi_v2_0_support){
    buffer[nth] |= 0x02;
  }

  if(midi_v1_0_support){
    buffer[nth] |= 0x01;
  }

  nth++;

  /* jitter reduction  */
  buffer[nth] = 0x00;

  if(rx_jitter_reduction){
    buffer[nth] |= 0x02;
  }

  if(tx_jitter_reduction){
    buffer[nth] |= 0x01;
  }

  nth++;
  
  /* reserved */
  memset(buffer + nth, 0, 8 * sizeof(guchar));
  nth += 8;
}

/**
 * ags_midi_ump_util_get_endpoint_info_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @major: (out): the return location of major
 * @minor: (out): the return location of minor
 * @static_function_blocks: (out): the return location of static function blocks
 * @function_block_count: (out): the return location of function block count
 * @midi_v2_0_support: (out): the return location of MIDI v2.0 support
 * @midi_v1_0_support: (out): the return location of MIDI v1.0 support
 * @rx_jitter_reduction: (out): the return location of RX jitter reduction
 * @tx_jitter_reduction: (out): the return location of TX jitter reduction
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get endpoint discovery.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_endpoint_info_notification(AgsMidiUmpUtil *midi_ump_util,
						 guchar *buffer,
						 gint *major,
						 gint *minor,
						 gboolean *static_function_blocks,
						 gint *function_block_count,
						 gboolean *midi_v2_0_support,
						 gboolean *midi_v1_0_support,
						 gboolean *rx_jitter_reduction,
						 gboolean *tx_jitter_reduction,
						 gchar ***extension_name, GValue **extension_value,
						 guint *extension_count)
{
  guint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0xf0, 0);
  g_return_val_if_fail(((0x300 & (buffer[0] << 8)) | (0xff & (buffer[1]))) == 0x01, 0);

  nth = 2;

  /* major */
  if(major != NULL){
    major[0] = buffer[nth];
  }

  nth++;

  /* minor */
  if(minor != NULL){
    minor[0] = buffer[nth];
  }

  nth++;

  /* static function blocks and function block count */
  if(static_function_blocks != NULL){
    static_function_blocks[0] = ((0x80 & buffer[nth]) != 0) ? TRUE: FALSE;
  }
  
  if(function_block_count != NULL){
    function_block_count[0] = (0x7f & buffer[nth]);
  }

  nth++;

  /* reserved */
  nth++;

  /* MIDI v1.0 and v2.0 support */
  if(midi_v2_0_support != NULL){
    midi_v2_0_support[0] = ((0x2 & buffer[nth]) != 0) ? TRUE: FALSE;
  }

  if(midi_v1_0_support != NULL){
    midi_v1_0_support[0] = ((0x1 & buffer[nth]) != 0) ? TRUE: FALSE;
  }

  nth++;

  /* RX/TX jitter reduction */
  if(rx_jitter_reduction != NULL){
    rx_jitter_reduction[0] = ((0x2 & buffer[0]) != 0) ? TRUE: FALSE;
  }
  
  if(tx_jitter_reduction != NULL){
    tx_jitter_reduction[0] = ((0x2 & buffer[0]) != 0) ? TRUE: FALSE;
  }

  nth++;
  
  /* reserved */
  nth += 8;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_device_identity_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is device identity notification.
 * 
 * Returns: %TRUE if is device identity notification, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_device_identity_notification(AgsMidiUmpUtil *midi_ump_util,
						  guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xf0 &&
     (0x0c & (buffer[0])) == 0x0 &&
     ((0x300 & (buffer[0] << 8)) | (0xff & (buffer[1]))) == 0x02){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_device_identity_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @device_manufacturer: the device manufacturer
 * @device_family: the device family
 * @device_family_model: the device family model
 * @software_revision: the software revision
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put device identity notification.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_device_identity_notification(AgsMidiUmpUtil *midi_ump_util,
						   guchar *buffer,
						   gint device_manufacturer,
						   gint device_family,
						   gint device_family_model,
						   gint software_revision,
						   gchar **extension_name, GValue *extension_value,
						   guint extension_count)
{
  guint nth;
  const gint status = 0x2;
  const gint format = 0x0;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0) | ((0x03 & format) << 2) | ((0x300 & status) >> 8);
  nth++;

  buffer[nth] = (0xff & status);
  nth++;

  /* reserved */
  memset(buffer + nth, 0, 3 * sizeof(guchar));
  nth += 3;

  /* device manufacturer */
  buffer[nth] = 0x7f & (device_manufacturer >> 14);
  nth++;

  buffer[nth] = 0x7f & (device_manufacturer >> 7);
  nth++;

  buffer[nth] = (0x7f & device_manufacturer);
  nth++;
  
  /* device family */
  buffer[nth] = (0x7f & device_family);
  nth++;

  buffer[nth] = 0x7f & (device_family >> 7);
  nth++;

  /* device family model */
  buffer[nth] = (0x7f & device_family_model);
  nth++;

  buffer[nth] = 0x7f & (device_family_model >> 7);
  nth++;

  /* software revision */
  buffer[nth] = (0x7f & software_revision);
  nth++;

  buffer[nth] = 0x7f & (software_revision >> 7);
  nth++;

  buffer[nth] = 0x7f & (software_revision >> 14);
  nth++;

  buffer[nth] = 0x7f & (software_revision >> 21);
  nth++;
}

/**
 * ags_midi_ump_util_get_device_identity_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @device_manufacturer: (out): the return location of device manufacturer
 * @device_family: (out): the return location of device family
 * @device_family_model: (out): the return location of device family model
 * @software_revision: (out): the return location of software revision
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get device identity notification.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_device_identity_notification(AgsMidiUmpUtil *midi_ump_util,
						   guchar *buffer,
						   gint *device_manufacturer,
						   gint *device_family,
						   gint *device_family_model,
						   gint *software_revision,
						   gchar ***extension_name, GValue **extension_value,
						   guint *extension_count)
{
  guint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0xf0, 0);
  g_return_val_if_fail((0x0c & (buffer[0])) == 0x0, 0);
  g_return_val_if_fail(((0x300 & (buffer[0] << 8)) | (0xff & (buffer[1]))) == 0x02, 0);

  nth = 2;

  /* reserved */
  nth++;

  /* reserved */
  nth++;

  /* reserved */
  nth++;

  /* device manufacturer */
  if(device_manufacturer != NULL){
    device_manufacturer[0] = (0x1fc000 & (buffer[nth] << 14)) | (0x03f80 & (buffer[nth + 1] << 7)) | (0x7f & buffer[nth + 2]);
  }

  nth += 3;

  /* device family */
  if(device_family != NULL){
    device_family[0] = (0x03f80 & (buffer[nth + 1] << 7)) | (0x7f & buffer[nth]);
  }

  nth += 2;

  /* device family model */
  if(device_family_model != NULL){
    device_family_model[0] = (0x03f80 & (buffer[nth + 1] << 7)) | (0x7f & buffer[nth]);
  }

  nth += 2;

  /* software revision */
  if(software_revision != NULL){
    software_revision[0] = (0x0fe00000 & (buffer[nth + 3]) << 21) | (0x1fc000 & (buffer[nth + 2]) << 14) | (0x03f80 & (buffer[nth + 1]) << 7) | (0x7f & buffer[nth]);
  }

  nth += 4;

  return(nth);
}

/**
 * ags_midi_ump_util_is_endpoint_name_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is endpoint name notification.
 * 
 * Returns: %TRUE if is endpoint name notification, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_endpoint_name_notification(AgsMidiUmpUtil *midi_ump_util,
						guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xf0 &&
     ((0x300 & (buffer[0] << 8)) | (0xff & (buffer[1]))) == 0x03){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_endpoint_name_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @endpoint_name: the endpoint name
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put endpoint name notification.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_endpoint_name_notification(AgsMidiUmpUtil *midi_ump_util,
						 guchar *buffer,
						 gchar *endpoint_name,
						 gchar **extension_name, GValue *extension_value,
						 guint extension_count)
{
  guint nth;
  gint endpoint_name_length;
  guint i;
  guint j, j_stop;
  gboolean is_complete, is_end;
  gint format;
  
  const gint status = 0x03;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  endpoint_name_length = strlen(endpoint_name);
  
  g_return_if_fail(endpoint_name_length <= 98);

  /* put endpoint name - may be in multiple chunks  */
  format = 0x0;
  is_complete = TRUE;

  if(endpoint_name_length >= 15){
    format = 0x1;
    is_complete = FALSE;
  }
  
  is_end = FALSE;
  
  for(i = 0, j = 0; i < 128 && !is_end;){
    nth = 0;
  
    buffer[nth] = (0xf0) | ((0x03 & format) << 2) | ((0x300 & status) >> 8);
    nth++;
    i++;

    buffer[nth] = (0xff & status);
    nth++;
    i++;

    /* fill in endpoint name */
    if(j < endpoint_name_length){
      if(j + 14 < endpoint_name_length){
	j_stop = 14;
      }else{
	j_stop = (endpoint_name_length - j) % 14;
      }
      
      memcpy(buffer + nth, endpoint_name + j, j_stop * sizeof(gchar));

      if(j_stop < 14){
	memset(buffer + nth + j_stop, 0, (14 - j_stop) * sizeof(gchar));
      }
    }else{
      g_warning("malformed MIDI UMP data");

      j_stop = 0;
    }
    
    nth += 14;
    j += j_stop;
    
    /* break condition */
    if(is_complete){
      is_end = TRUE;
    }

    if(format == 0x3){
      is_end = TRUE;
    }

    /* iterate */
    buffer += nth;

    if(format == 0x2){
      if(j + 14 < endpoint_name_length){
	format = 0x2;
      }else{
	format = 0x3;	
      }
    }

    if(format == 0x1){
      if(j + 14 < endpoint_name_length){
	format = 0x2;
      }else{
	format = 0x3;	
      }
    }
  }  
}

/**
 * ags_midi_ump_util_get_endpoint_name_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @endpoint_name: (out): the return location of endpoint name
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get endpoint name notification.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_endpoint_name_notification(AgsMidiUmpUtil *midi_ump_util,
						 guchar *buffer,
						 gchar **endpoint_name,
						 gchar ***extension_name, GValue **extension_value,
						 guint *extension_count)
{
  gchar *str;
  
  guint nth;
  gboolean is_complete, is_end;
  gint format;
  guint i;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0xf0, 0);
  g_return_val_if_fail(((0x0300 & (buffer[0] << 8)) | (0xff & (buffer[1]))) == 0x3, 0);

  str = (gchar *) g_malloc(99 * sizeof(gchar));
  memset(str, 0, 99 * sizeof(gchar));

  format = ((0x0c & (buffer[0])) >> 2);

  is_complete = TRUE;

  if(format != 0x0){
    is_complete = FALSE;
  }

  is_end = FALSE;

  nth = 0;

  for(i = 0; i < 99 && !is_end;){
    nth += 2;    
    
    memcpy(str + i, buffer + nth, 14 * sizeof(gchar));
    nth += 14;
    i += 14;
    
    format = ((0x0c & (buffer[nth])) >> 2);

    /* break condition */
    if(is_complete){
      is_end = TRUE;
    }

    if(format == 0x03){
      is_end = TRUE;
    }
  }

  if(endpoint_name != NULL){
    endpoint_name[0] = str;
  }else{
    g_free(str);
  }
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_product_id_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is product ID notification.
 * 
 * Returns: %TRUE if is product ID notification, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_product_instance_id_notification(AgsMidiUmpUtil *midi_ump_util,
						      guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xf0 &&
     ((0x0300 & (buffer[0] << 8)) | (0xff & (buffer[1]))) == 0x04){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_product_instance_id_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @product_instance_id: the product instance ID
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put product instance ID notification.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_product_instance_id_notification(AgsMidiUmpUtil *midi_ump_util,
						       guchar *buffer,
						       gchar *product_instance_id,
						       gchar **extension_name, GValue *extension_value,
						       guint extension_count)
{
  guint nth;
  gint product_instance_id_length;
  guint i;
  guint j, j_stop;
  gboolean is_complete, is_end;
  gint format;
  
  const gint status = 0x04;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  product_instance_id_length = strlen(product_instance_id);
  
  g_return_if_fail(product_instance_id_length <= 42);

  /* put endpoint name - may be in multiple chunks  */
  format = 0x0;
  is_complete = TRUE;

  if(product_instance_id_length >= 15){
    format = 0x1;
    is_complete = FALSE;
  }
  
  is_end = FALSE;
  
  for(i = 0, j = 0; i < 48 && !is_end;){
    nth = 0;
  
    buffer[nth] = (0xf0) | ((0x03 & format) << 2) | ((0x300 & status) >> 8);
    nth++;
    i++;

    buffer[nth] = (0xff & status);
    nth++;
    i++;

    /* fill in endpoint name */
    if(j < product_instance_id_length){
      if(j + 14 < product_instance_id_length){
	j_stop = 14;
      }else{
	j_stop = (product_instance_id_length - j) % 14;
      }
      
      memcpy(buffer + nth, product_instance_id + j, j_stop * sizeof(gchar));

      if(j_stop < 14){
	memset(buffer + nth + j_stop, 0, (14 - j_stop) * sizeof(gchar));
      }
    }else{
      g_warning("malformed MIDI UMP data");

      j_stop = 0;
    }
    
    nth += 14;
    j += j_stop;
    
    /* break condition */
    if(is_complete){
      is_end = TRUE;
    }

    if(format == 0x3){
      is_end = TRUE;
    }

    /* iterate */
    buffer += nth;
    
    if(format == 0x2){
      if(j + 14 < product_instance_id_length){
	format = 0x2;
      }else{
	format = 0x3;	
      }
    }

    if(format == 0x1){
      if(j + 14 < product_instance_id_length){
	format = 0x2;
      }else{
	format = 0x3;	
      }
    }
  }  
}

/**
 * ags_midi_ump_util_get_endpoint_info_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @product_instance_id_notification: (out): the return location of product instance ID notification
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get product instance ID notification.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_product_instance_id_notification(AgsMidiUmpUtil *midi_ump_util,
						       guchar *buffer,
						       gchar **product_instance_id,
						       gchar ***extension_name, GValue **extension_value,
						       guint *extension_count)
{
  gchar *str;
  
  guint nth;
  gboolean is_complete, is_end;
  gint format;
  guint i;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0xf0, 0);
  g_return_val_if_fail(((0x0300 & (buffer[0] << 8)) | (0xff & (buffer[1]))) == 0x04, 0);

  nth = 0;

  str = (gchar *) g_malloc(43 * sizeof(gchar));
  memset(str, 0, 43 * sizeof(gchar));

  format = ((0x0c & (buffer[0])) >> 2);

  is_complete = TRUE;

  if(format != 0x0){
    is_complete = FALSE;
  }

  is_end = FALSE;

  for(i = 0; i < 43 && !is_end;){
    nth += 2;    
    
    memcpy(str + i, buffer + nth, 14 * sizeof(gchar));
    nth += 14;
    i += 14;
    
    format = ((0x0c & (buffer[nth])) >> 2);

    /* break condition */
    if(is_complete){
      is_end = TRUE;
    }

    if(format == 0x03){
      is_end = TRUE;
    }
  }

  if(product_instance_id != NULL){
    product_instance_id[0] = str;
  }else{
    g_free(str);
  }
  
  return(0);
}

/**
 * ags_midi_ump_util_is_stream_configuration_request:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is stream configuration request.
 * 
 * Returns: %TRUE if is stream configuration request, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_stream_configuration_request(AgsMidiUmpUtil *midi_ump_util,
						  guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xf0 &&
     (((0x3 & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x05){
    return(TRUE);
  }

  return(FALSE);
}

/**
 * ags_midi_ump_util_put_stream_configuration_request:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @protocol: the protocol
 * @rx_jitter_reduction: %TRUE if receiving jitter reduction, otherwise %FALSE
 * @tx_jitter_reduction: %TRUE if transfer jitter reduction, otherwise %FALSE
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put product instance ID request.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_stream_configuration_request(AgsMidiUmpUtil *midi_ump_util,
						   guchar *buffer,
						   gint protocol,
						   gboolean rx_jitter_reduction, gboolean tx_jitter_reduction,
						   gchar **extension_name, GValue *extension_value,
						   guint extension_count)
{
  guint nth;
  gint format;
  
  const gint status = 0x05;

  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);
  
  format = 0x00;

  nth = 0;
  
  buffer[nth] = (0xf0) | ((0x03 & format) << 2) | ((0x300 & status) >> 8);
  nth++;

  buffer[nth] = (0xff & status);
  nth++;
  
  buffer[nth] = (0xff) & (protocol);
  nth++;

  buffer[nth] = (0xff) & ((rx_jitter_reduction ? (0x01 << 1): 0x0) | (rx_jitter_reduction ? (0x01): 0x0));
  nth++;

  memset(buffer + nth, 0, 12 * sizeof(guchar));
  nth += 12;
}

/**
 * ags_midi_ump_util_get_stream_configuration_request:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @protocol: (out): the return location of protocol
 * @rx_jitter_reduction: (out): the return location of RX jitter reduction
 * @tx_jitter_reduction: (out): the return location of TX jitter reduction
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get stream configuration request.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_stream_configuration_request(AgsMidiUmpUtil *midi_ump_util,
						   guchar *buffer,
						   gint *protocol,
						   gboolean *rx_jitter_reduction, gboolean *tx_jitter_reduction,
						   gchar ***extension_name, GValue **extension_value,
						   guint *extension_count)
{  
  guint nth;

  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail((((0x0f & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x5, 0);

  nth = 2;

  /* protocol */
  if(protocol != NULL){
    protocol[0] = 0xff & buffer[nth];
  }

  nth++;

  /* RX/TX jitter reduction */
  if(rx_jitter_reduction != NULL){
    rx_jitter_reduction[0] = ((0x2 & buffer[nth]) != 0) ? TRUE: FALSE;
  }

  if(tx_jitter_reduction != NULL){
    tx_jitter_reduction[0] = ((0x1 & buffer[nth]) != 0) ? TRUE: FALSE;
  }

  nth += 12;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_stream_configuration_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is stream configuration notification.
 * 
 * Returns: %TRUE if is stream configuration notification, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_stream_configuration_notification(AgsMidiUmpUtil *midi_ump_util,
						       guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xf0 &&
     (((0x3 & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x06){
    return(TRUE);
  }

  return(FALSE);
}

/**
 * ags_midi_ump_util_put_product_instance_id_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @protocol: the protocol
 * @rx_jitter_reduction: %TRUE if receiving jitter reduction, otherwise %FALSE
 * @tx_jitter_reduction: %TRUE if transfer jitter reduction, otherwise %FALSE
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put product instance ID notification.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_stream_configuration_notification(AgsMidiUmpUtil *midi_ump_util,
							guchar *buffer,
							gint protocol,
							gboolean rx_jitter_reduction, gboolean tx_jitter_reduction,
							gchar **extension_name, GValue *extension_value,
							guint extension_count)
{
  guint nth;
  gint format;
  
  const gint status = 0x06;

  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);
  
  format = 0x00;

  nth = 0;
  
  buffer[nth] = (0xf0) | ((0x03 & format) << 2) | ((0x300 & status) >> 8);
  nth++;

  buffer[nth] = (0xff) & (protocol);
  nth++;

  buffer[nth] = (0xff) & ((rx_jitter_reduction ? (0x01 << 1): 0x0) | (rx_jitter_reduction ? (0x01): 0x0));
  nth++;

  memset(buffer, 0, 12 * sizeof(guchar));
  nth += 12;
}

/**
 * ags_midi_ump_util_get_stream_configuration_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @protocol: (out): the return location of protocol
 * @rx_jitter_reduction: (out): the return location of RX jitter reduction
 * @tx_jitter_reduction: (out): the return location of TX jitter reduction
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get stream configuration notification.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_stream_configuration_notification(AgsMidiUmpUtil *midi_ump_util,
							guchar *buffer,
							gint *protocol,
							gboolean *rx_jitter_reduction, gboolean *tx_jitter_reduction,
							gchar ***extension_name, GValue **extension_value,
							guint *extension_count)
{  
  guint nth;

  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail((((0x0f & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x6, 0);

  nth = 2;

  /* protocol */
  if(protocol != NULL){
    protocol[0] = 0xff & buffer[nth];
  }

  nth++;

  /* RX/TX jitter reduction */
  if(rx_jitter_reduction != NULL){
    rx_jitter_reduction[0] = ((0x2 & buffer[nth]) != 0) ? TRUE: FALSE;
  }

  if(tx_jitter_reduction != NULL){
    tx_jitter_reduction[0] = ((0x1 & buffer[nth]) != 0) ? TRUE: FALSE;
  }

  nth += 12;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_function_block_discovery:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is function block discovery.
 * 
 * Returns: %TRUE if is function block discovery, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_function_block_discovery(AgsMidiUmpUtil *midi_ump_util,
					      guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xf0 &&
     (((0x03 & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x10){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_function_block_discovery:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @function_block: the function block through 0x00 - 0x1f or 0xff to request all
 * @filter: the filter
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put function block discovery.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_function_block_discovery(AgsMidiUmpUtil *midi_ump_util,
					       guchar *buffer,
					       gint function_block,
					       gint filter,
					       gchar **extension_name, GValue *extension_value,
					       guint extension_count)
{
  guint nth;
  gint format;
  
  const gint status = 0x10;

  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);
  
  format = 0x00;

  nth = 0;
  
  buffer[nth] = (0xf0) | ((0x03 & format) << 2) | ((0x300 & status) >> 8);
  nth++;

  buffer[nth] = (0xff) & (function_block);
  nth++;

  buffer[nth] = (0xff) & (filter);
  nth++;

  memset(buffer, 0, 12 * sizeof(guchar));
  nth += 12;
}

/**
 * ags_midi_ump_util_get_function_block_discovery:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @function_block: (out): the return location of function block
 * @filter: (out): the return location of filter
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get function block discovery.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_function_block_discovery(AgsMidiUmpUtil *midi_ump_util,
					       guchar *buffer,
					       gint *function_block,
					       gint *filter,
					       gchar ***extension_name, GValue **extension_value,
					       guint *extension_count)
{
  guint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail((((0x0f & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x10, 0);

  nth = 2;

  /* function block */
  if(function_block != NULL){
    function_block[0] = buffer[nth];
  }

  nth++;

  /* filter */
  if(filter != NULL){
    filter[0] = buffer[nth];
  }

  nth++;
    
  /* reserved */
  nth += 12;
  
  //TODO:JK: implement me

  return(nth);
}

/**
 * ags_midi_ump_util_is_function_block_info_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is function block info notification.
 * 
 * Returns: %TRUE if is function block info notification, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_function_block_info_notification(AgsMidiUmpUtil *midi_ump_util,
						      guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xf0 &&
     (((0x03 & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x11){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_function_block_info_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @function_block_active: the function block active
 * @function_block: the function block through 0x00 - 0x1f or 0xff to request all
 * @direction: the direction
 * @midi1_port: the MIDI v1.0 port
 * @ui_hint: the UI hint
 * @first_group: the first group
 * @group_count: the group count
 * @message_version: the message version
 * @max_sysex8_stream_count: the max SYSEX8 stream count
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put function block info notification.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_function_block_info_notification(AgsMidiUmpUtil *midi_ump_util,
						       guchar *buffer,
						       gboolean function_block_active,
						       gint function_block,
						       gint direction,
						       gint midi1_port,
						       gint ui_hint,
						       gint first_group,
						       gint group_count,
						       gint message_version,
						       gint max_sysex8_stream_count,
						       gchar **extension_name, GValue *extension_value,
						       guint extension_count)
{
  guint nth;
  gint format;
  
  const gint status = 0x11;

  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);
  
  format = 0x00;

  nth = 0;
  
  buffer[nth] = (0xf0) | ((0x03 & format) << 2) | ((0x300 & status) >> 8);
  nth++;

  buffer[nth] = (0xff) & ((function_block_active ? (0x80): 0x00) | (function_block));
  nth++;

  buffer[nth] = (0xff) & ((direction) | (midi1_port << 2) | (ui_hint << 4));
  nth++;

  buffer[nth] = (0xff) & (first_group);
  nth++;
  
  buffer[nth] = (0xff) & (group_count);
  nth++;

  buffer[nth] = (0xff) & (message_version);
  nth++;

  buffer[nth] = (0xff) & (max_sysex8_stream_count);
  nth++;
  
  memset(buffer, 0, 8 * sizeof(guchar));
  nth += 12;
}

guint
ags_midi_ump_util_get_function_block_info_notification(AgsMidiUmpUtil *midi_ump_util,
						       guchar *buffer,
						       gboolean *function_block_active,
						       gint *function_block,
						       gint *direction,
						       gint *midi1_port,
						       gint *ui_hint,
						       gint *first_group,
						       gint *group_count,
						       gint *message_version,
						       gint *max_sysex8_stream_count,
						       gint *filter,
						       gchar ***extension_name, GValue **extension_value,
						       guint *extension_count)
{
  guint nth;

  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail((((0x0f & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x11, 0);

  nth = 2;

  /* function block active and function block */
  if(function_block_active != NULL){
    function_block_active[0] = ((0x01 & buffer[nth]) != 0) ? TRUE: FALSE;
  }

  if(function_block != NULL){
    function_block[0] = 0x7f & buffer[nth];
  }

  nth++;

  /* direction, MIDI v1.0 port and UI hint */
  if(direction != NULL){
    direction = 0x3 & buffer[nth];
  }

  if(midi1_port != NULL){
    midi1_port[0] = (0x0c & buffer[nth]) >> 2;
  }

  if(ui_hint != NULL){
    ui_hint = (0x30 & buffer[nth]) >> 4;
  }
  
  nth++;

  /* first group */
  if(first_group != NULL){
    first_group[0] = buffer[nth];
  }
  
  nth++;

  /* group count */
  if(group_count != NULL){
    group_count[0] = buffer[nth];
  }
  
  nth++;

  /* message version */
  if(message_version != NULL){
    message_version[0] = buffer[nth];    
  }
  
  nth++;

  /* max SYSEX8 stream count */
  if(max_sysex8_stream_count != NULL){
    max_sysex8_stream_count[0] = buffer[nth];
  }
  
  nth++;

  /* reserved */
  nth += 8;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_function_block_name_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is function block name notification.
 * 
 * Returns: %TRUE if is function block name notification, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_function_block_name_notification(AgsMidiUmpUtil *midi_ump_util,
						      guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xf0 &&
     (((0x03 & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x12){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_function_block_name_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @function_block: the function block
 * @function_block_name: the function block name
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put function block name notification.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_function_block_name_notification(AgsMidiUmpUtil *midi_ump_util,
						       guchar *buffer,
						       gint function_block,
						       gchar *function_block_name,
						       gchar **extension_name, GValue *extension_value,
						       guint extension_count)
{
  guint nth;
  gint function_block_name_length;
  guint i;
  guint j, j_stop;
  gboolean is_complete, is_end;
  gint format;
  
  const gint status = 0x12;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  function_block_name_length = strlen(function_block_name);
  
  g_return_if_fail(function_block_name_length <= 98);

  /* put function_block name - may be in multiple chunks  */
  format = 0x0;
  is_complete = TRUE;

  if(function_block_name_length >= 15){
    format = 0x1;
    is_complete = FALSE;
  }
  
  is_end = FALSE;

  for(i = 0, j = 0; i < 128 && !is_end;){
    nth = 0;
  
    buffer[nth] = (0xf0) | ((0x03 & format) << 2) | ((0x300 & status) >> 8);
    nth++;
    i++;

    buffer[nth] = (0xff & status);
    nth++;
    i++;

    /* function block */    
    buffer[nth] = (0xff & function_block);
    nth++;
    i++;

    /* fill in function block name */
    if(j < function_block_name_length){
      if(j + 14 < function_block_name_length){
	j_stop = 14;
      }else{
	j_stop = (function_block_name_length - j) % 14;
      }
      
      memcpy(buffer + nth, function_block_name + j, j_stop * sizeof(gchar));

      if(j_stop < 14){
	memset(buffer + nth + j_stop, 0, (14 - j_stop) * sizeof(gchar));
      }
    }else{
      g_warning("malformed MIDI UMP data");

      j_stop = 0;
    }
    
    nth += 14;
    j += j_stop;
    
    /* break condition */
    if(is_complete){
      is_end = TRUE;
    }

    if(format == 0x3){
      is_end = TRUE;
    }

    /* iterate */
    buffer += nth;

    if(format == 0x2){
      if(j + 14 < function_block_name_length){
	format = 0x2;
      }else{
	format = 0x3;	
      }
    }

    if(format == 0x1){
      if(j + 14 < function_block_name_length){
	format = 0x2;
      }else{
	format = 0x3;	
      }
    }
  }  
}

/**
 * ags_midi_ump_util_get_function_block_name_notification:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @function_block: (out): the return location of function block
 * @function_block_name: (out): the return location of function block name
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get function block name notification.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_function_block_name_notification(AgsMidiUmpUtil *midi_ump_util,
						       guchar *buffer,
						       gint *function_block,
						       gchar **function_block_name,
						       gchar ***extension_name, GValue **extension_value,
						       guint *extension_count)
{
  gchar *str;
  
  guint nth;
  gboolean is_complete, is_end;
  gint format;
  guint i;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail((((0x0f & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x12, 0);

  str = (gchar *) g_malloc(99 * sizeof(gchar));
  memset(str, 0, 99 * sizeof(gchar));

  format = ((0x0c & (buffer[0])) >> 2);

  is_complete = TRUE;

  if(format != 0x0){
    is_complete = FALSE;
  }

  is_end = FALSE;

  if(function_block != NULL){
    function_block[0] =  0xff & buffer[2];
  }
  
  nth = 3;

  for(i = 0; i < 99 && !is_end;){
    nth += 2;    
    
    memcpy(str + i, buffer + nth, 14 * sizeof(gchar));
    nth += 14;
    i += 14;
    
    format = ((0x0c & (buffer[nth])) >> 2);

    /* break condition */
    if(is_complete){
      is_end = TRUE;
    }

    if(format == 0x03){
      is_end = TRUE;
    }
  }
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_start_of_clip:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is start of clip.
 * 
 * Returns: %TRUE if is start of clip, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_start_of_clip(AgsMidiUmpUtil *midi_ump_util,
				   guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xf0 &&
     (((0x0f & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x20){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_start_of_clip:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put start of clip.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_start_of_clip(AgsMidiUmpUtil *midi_ump_util,
				    guchar *buffer,
				    gchar **extension_name, GValue *extension_value,
				    guint extension_count)
{
  guint nth;
  const gint status = 0x20;
  const gint format = 0x0;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0) | ((0x03 & format) << 2) | ((0x300 & status) >> 8);
  nth++;

  buffer[nth] = (0xff & status);
  nth++;

  /* reserved */
  memset(buffer + nth, 0, 14 * sizeof(guchar));
  nth += 14;
}

guint
ags_midi_ump_util_get_start_of_clip(AgsMidiUmpUtil *midi_ump_util,
				    guchar *buffer,
				    gchar ***extension_name, GValue **extension_value,
				    guint *extension_count)
{
  guint nth;

  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail((((0x0f & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x20, 0);

  nth = 2;

  /* reserved */
  nth += 14;

  return(nth);
}


/**
 * ags_midi_ump_util_is_end_of_clip:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is end of clip.
 * 
 * Returns: %TRUE if is end of clip, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_end_of_clip(AgsMidiUmpUtil *midi_ump_util,
				 guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xf0 &&
     (((0x0f & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x21){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_end_of_clip:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put end of clip.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_end_of_clip(AgsMidiUmpUtil *midi_ump_util,
				  guchar *buffer,
				  gchar **extension_name, GValue *extension_value,
				  guint extension_count)
{
  guint nth;
  const gint status = 0x21;
  const gint format = 0x0;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0) | ((0x03 & format) << 2) | ((0x300 & status) >> 8);
  nth++;

  buffer[nth] = (0xff & status);
  nth++;

  /* reserved */
  memset(buffer + nth, 0, 14 * sizeof(guchar));
  nth += 14;
}

guint
ags_midi_ump_util_get_end_of_clip(AgsMidiUmpUtil *midi_ump_util,
				  guchar *buffer,
				  gchar ***extension_name, GValue **extension_value,
				  guint *extension_count)
{
  guint nth;

  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail((((0x0f & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x21, 0);

  nth = 2;

  /* reserved */
  nth += 14;

  return(nth);
}

/**
 * ags_midi_ump_util_is_noop:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is NOOP.
 * 
 * Returns: %TRUE if is NOOP, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_noop(AgsMidiUmpUtil *midi_ump_util,
			  guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x00 &&
     (0x0f & (buffer[1])) == 0x00){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_noop:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put NOOP.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_noop(AgsMidiUmpUtil *midi_ump_util,
			   guchar *buffer,
			   gchar **extension_name, GValue *extension_value,
			   guint extension_count)
{
  guint nth;
  const gint status = 0x00;
  const gint mt = 0x00;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0) | ((0x07 & mt) << 4);
  nth++;

  buffer[nth] = (0xff) & (status << 4);
  nth++;
  
  /* NOOP */
  buffer[nth] = 0x00;
  nth++;

  buffer[nth] = 0x00;
  nth++;
}

guint
ags_midi_ump_util_get_noop(AgsMidiUmpUtil *midi_ump_util,
			   guchar *buffer,
			   gchar ***extension_name, GValue **extension_value,
			   guint *extension_count)
{
  guint nth;

  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail((0xf0 & (buffer[0])) == 0x00 && (0x0f & (buffer[1])) == 0x00, 0);

  nth = 2;

  /* 0000 */
  nth += 2;

  return(nth);
}

/**
 * ags_midi_ump_util_is_jr_clock:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is JR clock.
 * 
 * Returns: %TRUE if is JR clock, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_jr_clock(AgsMidiUmpUtil *midi_ump_util,
			      guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x00 &&
     (0x0f & (buffer[1])) == 0x01){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_jr_clock:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @sender_clock_time: sender clock time as a unsigned 16 bit integer
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put JR clock.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_jr_clock(AgsMidiUmpUtil *midi_ump_util,
			       guchar *buffer,
			       guint16 sender_clock_time,
			       gchar **extension_name, GValue *extension_value,
			       guint extension_count)
{
  guint nth;
  const gint status = 0x01;
  const gint mt = 0x00;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0) | ((0x07 & mt) << 4);
  nth++;

  buffer[nth] = (0xff) & (status << 4);
  nth++;
  
  /* JR clock */
  buffer[nth] = (0xff & (sender_clock_time >> 8));
  nth++;
  
  buffer[nth] = (0xff & (sender_clock_time));
  nth++;
}

/**
 * ags_midi_ump_util_get_jr_clock:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @sender_clock_time: (out): the return location of sender clock time
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get jitter reduction clock.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_jr_clock(AgsMidiUmpUtil *midi_ump_util,
			       guchar *buffer,
			       guint16 *sender_clock_time,
			       gchar ***extension_name, GValue **extension_value,
			       guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail((0xf0 & (buffer[0])) == 0x00 && (0x0f & (buffer[1])) == 0x01, 0);

  nth = 2;

  /* sender clock time */
  if(sender_clock_time != NULL){
    sender_clock_time[0] = (0xff00 | (buffer[nth] << 8)) | (0xff & buffer[nth + 1]);
  }

  nth += 2;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_jr_timestamp:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is JR timestamp.
 * 
 * Returns: %TRUE if is JR timestamp, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_jr_timestamp(AgsMidiUmpUtil *midi_ump_util,
				  guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x00 &&
     (0x0f & (buffer[1])) == 0x02){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_jr_timestamp:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @sender_clock_timestamp: sender clock timestamp as a unsigned 16 bit integer
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put JR timestamp.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_jr_timestamp(AgsMidiUmpUtil *midi_ump_util,
				   guchar *buffer,
				   guint16 sender_clock_timestamp,
				   gchar **extension_name, GValue *extension_value,
				   guint extension_count)
{
  guint nth;
  const gint status = 0x02;
  const gint mt = 0x00;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0) | ((0x07 & mt) << 4);
  nth++;

  buffer[nth] = (0xff) & (status << 4);
  nth++;
  
  /* JR timestamp */
  buffer[nth] = (0xff & (sender_clock_timestamp >> 8));
  nth++;
  
  buffer[nth] = (0xff & (sender_clock_timestamp));
  nth++;
}

/**
 * ags_midi_ump_util_get_jr_timestamp:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @sender_clock_timestamp: (out): the return location of sender clock timestamp
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get jitter reduction timestamp.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_jr_timestamp(AgsMidiUmpUtil *midi_ump_util,
				   guchar *buffer,
				   guint16 *sender_clock_timestamp,
				   gchar ***extension_name, GValue **extension_value,
				   guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail((0xf0 & (buffer[0])) == 0x00 && (0x0f & (buffer[1])) == 0x02, 0);

  nth = 2;

  /* sender timestamp time */
  if(sender_clock_timestamp != NULL){
    sender_clock_timestamp[0] = (0xff00 | (buffer[nth] << 8)) | (0xff & buffer[nth + 1]);
  }

  nth += 2;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_delta_clock_ticks_per_quarter_note:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is delta clock ticks per quarter note.
 * 
 * Returns: %TRUE if is delta clock ticks per quarter note, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_delta_clock_ticks_per_quarter_note(AgsMidiUmpUtil *midi_ump_util,
							guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x00 &&
     (0x0f & (buffer[1])) == 0x03){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_delta_clock_ticks_per_quarter_note:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @ticks_per_quarter_note_count: ticks per quarter note count as a unsigned 16 bit integer
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put delta clock ticks per quarter note.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_delta_clock_ticks_per_quarter_note(AgsMidiUmpUtil *midi_ump_util,
							 guchar *buffer,
							 guint16 ticks_per_quarter_note_count,
							 gchar **extension_name, GValue *extension_value,
							 guint extension_count)
{
  guint nth;
  const gint status = 0x03;
  const gint mt = 0x00;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0) | ((0x07 & mt) << 4);
  nth++;

  buffer[nth] = (0xff) & (status << 4);
  nth++;
  
  /* delta clock ticks per quarter note count */
  buffer[nth] = (0xff & (ticks_per_quarter_note_count >> 8));
  nth++;
  
  buffer[nth] = (0xff & (ticks_per_quarter_note_count));
  nth++;
}

/**
 * ags_midi_ump_util_get_delta_clock_ticks_per_quarter_note:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @ticks_per_quarter_note_count: (out): the return location of ticks per quarter note count as a unsigned 16 bit integer
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get product instance ID notification.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_delta_clock_ticks_per_quarter_note(AgsMidiUmpUtil *midi_ump_util,
							 guchar *buffer,
							 guint16 *ticks_per_quarter_note_count,
							 gchar ***extension_name, GValue **extension_value,
							 guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail((0xf0 & (buffer[0])) == 0x00 && (0x0f & (buffer[1])) == 0x03, 0);

  nth = 2;

  /* ticks per quarter note count */
  if(ticks_per_quarter_note_count != NULL){
    ticks_per_quarter_note_count[0] = (0xff00 | (buffer[nth] << 8)) | (0xff & buffer[nth + 1]);
  }

  nth += 2;

  return(nth);
}

/**
 * ags_midi_ump_util_is_delta_clock_ticks_since_last_event:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is delta clock ticks since last event.
 * 
 * Returns: %TRUE if is delta clock ticks since last event, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_delta_clock_ticks_since_last_event(AgsMidiUmpUtil *midi_ump_util,
							guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x00 &&
     (0x0f & (buffer[1])) == 0x04){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_delta_clock_ticks_since_last_event:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @ticks_since_last_event_count: ticks since last event count as a unsigned 16 bit integer
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put delta clock ticks per quarter note.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_delta_clock_ticks_since_last_event(AgsMidiUmpUtil *midi_ump_util,
							 guchar *buffer,
							 guint16 ticks_since_last_event_count,
							 gchar **extension_name, GValue *extension_value,
							 guint extension_count)
{
  guint nth;
  const gint status = 0x04;
  const gint mt = 0x00;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0) | ((0x07 & mt) << 4);
  nth++;

  buffer[nth] = (0xff) & (status << 4);
  nth++;
  
  /* delta clock ticks per quarter note count */
  buffer[nth] = (0xff & (ticks_since_last_event_count));
  nth++;
  
  buffer[nth] = (0xff & (ticks_since_last_event_count >> 8));
  nth++;
}

/**
 * ags_midi_ump_util_get_delta_clock_ticks_since_last_event:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @ticks_since_last_event_count: (out): the return location of ticks since last event count as a unsigned 16 bit integer
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get product instance ID notification.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_delta_clock_ticks_since_last_event(AgsMidiUmpUtil *midi_ump_util,
							 guchar *buffer,
							 guint16 *ticks_since_last_event_count,
							 gchar ***extension_name, GValue **extension_value,
							 guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail(buffer[0] == 0xf0, 0);
  g_return_val_if_fail((0xf0 & (buffer[0])) == 0x00 && (0x0f & (buffer[1])) == 0x04, 0);

  nth = 2;

  /* ticks per quarter note count */
  if(ticks_since_last_event_count != NULL){
    ticks_since_last_event_count[0] = (0xff00 | (buffer[nth] << 8)) | (0xff & buffer[nth + 1]);
  }

  nth += 2;

  return(nth);
}

/**
 * ags_midi_ump_util_is_midi1_channel_voice:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 1.0 channel voice.
 * 
 * Returns: %TRUE if is delta clock ticks since last event, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi1_channel_voice(AgsMidiUmpUtil *midi_ump_util,
					 guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x20){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_is_midi1_note_off:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 1.0 note off.
 * 
 * Returns: %TRUE if is MIDI version 1.0 note off, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi1_note_off(AgsMidiUmpUtil *midi_ump_util,
				    guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x20 &&
     (0xf0 & (buffer[1])) == 0x80){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi1_note_off:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @key: the index key
 * @velocity: the velocity
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 1.0 note off.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi1_note_off(AgsMidiUmpUtil *midi_ump_util,
				     guchar *buffer,
				     gint group,
				     gint channel,
				     gint key,
				     gint velocity,
				     gchar **extension_name, GValue *extension_value,
				     guint extension_count)
{
  guint nth;
  const gint opcode = 0x08;
  const gint mt = 0x02;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* index key and velocity */
  buffer[nth] = (0xff & (key));
  nth++;
  
  buffer[nth] = (0xff & (velocity));
  nth++;
}

/**
 * ags_midi_ump_util_get_midi1_note_off:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @key: (out): the return location of index key
 * @velocity: (out): the return location of velocity
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 1.0 note off.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi1_note_off(AgsMidiUmpUtil *midi_ump_util,
				     guchar *buffer,
				     gint *group,
				     gint *channel,
				     gint *key,
				     gint *velocity,
				     gchar ***extension_name, GValue **extension_value,
				     guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x20, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0x80, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
		       
  nth = 2;

  /* key */
  if(key != NULL){
    key[0] = 0xff & buffer[nth];
  }

  nth++;

  /* velocity */
  if(velocity != NULL){
    velocity[0] = 0xff & buffer[nth];
  }

  nth++;

  return(nth);
}

/**
 * ags_midi_ump_util_is_midi1_note_on:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 1.0 note on.
 * 
 * Returns: %TRUE if is MIDI version 1.0 note on, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi1_note_on(AgsMidiUmpUtil *midi_ump_util,
				   guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x20 &&
     (0xf0 & (buffer[1])) == 0x90){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi1_note_on:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @key: the index key
 * @velocity: the velocity
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 1.0 note on.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi1_note_on(AgsMidiUmpUtil *midi_ump_util,
				    guchar *buffer,
				    gint group,
				    gint channel,
				    gint key,
				    gint velocity,
				    gchar **extension_name, GValue *extension_value,
				    guint extension_count)
{
  guint nth;
  const gint opcode = 0x09;
  const gint mt = 0x02;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* index key and velocity */
  buffer[nth] = (0xff & (key));
  nth++;
  
  buffer[nth] = (0xff & (velocity));
  nth++;
}

/**
 * ags_midi_ump_util_get_midi1_note_on:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @key: (out): the return location of index key
 * @velocity: (out): the return location of velocity
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 1.0 note on.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi1_note_on(AgsMidiUmpUtil *midi_ump_util,
				    guchar *buffer,
				    gint *group,
				    gint *channel,
				    gint *key,
				    gint *velocity,
				    gchar ***extension_name, GValue **extension_value,
				    guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x20, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0x90, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
		       
  nth = 2;

  /* key */
  if(key != NULL){
    key[0] = 0xff & buffer[nth];
  }

  nth++;

  /* velocity */
  if(velocity != NULL){
    velocity[0] = 0xff & buffer[nth];
  }

  nth++;

  return(nth);
}

/**
 * ags_midi_ump_util_is_midi1_polyphonic_aftertouch:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 1.0 polyphonic aftertouch.
 * 
 * Returns: %TRUE if is MIDI version 1.0 polyphonic aftertouch, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi1_polyphonic_aftertouch(AgsMidiUmpUtil *midi_ump_util,
						 guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x20 &&
     (0xf0 & (buffer[1])) == 0xa0){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi1_polyphonic_aftertouch:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @key: the index key
 * @data: the data
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 1.0 polyphonic aftertouch.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi1_polyphonic_aftertouch(AgsMidiUmpUtil *midi_ump_util,
						  guchar *buffer,
						  gint group,
						  gint channel,
						  gint key,
						  gint data,
						  gchar **extension_name, GValue *extension_value,
						  guint extension_count)
{
  guint nth;
  const gint opcode = 0x0a;
  const gint mt = 0x02;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* index key and data */
  buffer[nth] = (0xff & (key));
  nth++;
  
  buffer[nth] = (0xff & (data));
  nth++;
}

/**
 * ags_midi_ump_util_get_midi1_polyphonic_aftertouch:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @key: (out): the return location of index key
 * @data: (out): the return location of data
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 1.0 polyphonic aftertouch.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi1_polyphonic_aftertouch(AgsMidiUmpUtil *midi_ump_util,
						  guchar *buffer,
						  gint *group,
						  gint *channel,
						  gint *key,
						  gint *data,
						  gchar ***extension_name, GValue **extension_value,
						  guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x20, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0xa0, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
		       
  nth = 2;

  /* key */
  if(key != NULL){
    key[0] = 0xff & buffer[nth];
  }

  nth++;

  /* data */
  if(data != NULL){
    data[0] = 0xff & buffer[nth];
  }

  nth++;

  return(nth);
}

/**
 * ags_midi_ump_util_is_midi1_control_change:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 1.0 control change.
 * 
 * Returns: %TRUE if is MIDI version 1.0 control change, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi1_control_change(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x20 &&
     (0xf0 & (buffer[1])) == 0xb0){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi1_control_change:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @index_key: the index key
 * @data: the data
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 1.0 control change.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi1_control_change(AgsMidiUmpUtil *midi_ump_util,
					   guchar *buffer,
					   gint group,
					   gint channel,
					   gint index_key,
					   gint data,
					   gchar **extension_name, GValue *extension_value,
					   guint extension_count)
{
  guint nth;
  const gint opcode = 0x0b;
  const gint mt = 0x02;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* index key and data */
  buffer[nth] = (0xff & (index_key));
  nth++;
  
  buffer[nth] = (0xff & (data));
  nth++;
}

/**
 * ags_midi_ump_util_get_midi1_control_change:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @index_key: (out): the return location of index key
 * @data: (out): the return location of data
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 1.0 control change.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi1_control_change(AgsMidiUmpUtil *midi_ump_util,
					   guchar *buffer,
					   gint *group,
					   gint *channel,
					   gint *index_key,
					   gint *data,
					   gchar ***extension_name, GValue **extension_value,
					   guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x20, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0xb0, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
		       
  nth = 2;

  /* index key */
  if(index_key != NULL){
    index_key[0] = 0xff & buffer[nth];
  }

  nth++;

  /* data */
  if(data != NULL){
    data[0] = 0xff & buffer[nth];
  }

  nth++;

  return(nth);
}

/**
 * ags_midi_ump_util_is_midi1_program_change:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 1.0 program change.
 * 
 * Returns: %TRUE if is MIDI version 1.0 program change, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi1_program_change(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x20 &&
     (0xf0 & (buffer[1])) == 0xc0){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi1_program_change:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @program: the program
 * @data: the data
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 1.0 program change.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi1_program_change(AgsMidiUmpUtil *midi_ump_util,
					   guchar *buffer,
					   gint group,
					   gint channel,
					   gint program,
					   gint data,
					   gchar **extension_name, GValue *extension_value,
					   guint extension_count)
{
  guint nth;
  const gint opcode = 0x0c;
  const gint mt = 0x02;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* index program and data */
  buffer[nth] = (0xff & (program));
  nth++;
  
  buffer[nth] = (0xff & (data));
  nth++;
}

/**
 * ags_midi_ump_util_get_midi1_program_change:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @program: (out): the return location of program
 * @data: (out): the return location of data
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 1.0 program change.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi1_program_change(AgsMidiUmpUtil *midi_ump_util,
					   guchar *buffer,
					   gint *group,
					   gint *channel,
					   gint *program,
					   gint *data,
					   gchar ***extension_name, GValue **extension_value,
					   guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x20, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0xc0, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
		       
  nth = 2;

  /* program */
  if(program != NULL){
    program[0] = 0xff & buffer[nth];
  }

  nth++;

  /* data */
  if(data != NULL){
    data[0] = 0xff & buffer[nth];
  }

  nth++;

  return(nth);
}

/**
 * ags_midi_ump_util_is_midi1_channel_pressure:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 1.0 channel pressure.
 * 
 * Returns: %TRUE if is MIDI version 1.0 channel pressure, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi1_channel_pressure(AgsMidiUmpUtil *midi_ump_util,
					    guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x20 &&
     (0xf0 & (buffer[1])) == 0xd0){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi1_channel_pressure:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @pressure: the pressure
 * @data: the data
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 1.0 channel pressure.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi1_channel_pressure(AgsMidiUmpUtil *midi_ump_util,
					     guchar *buffer,
					     gint group,
					     gint channel,
					     gint pressure,
					     gint data,
					     gchar **extension_name, GValue *extension_value,
					     guint extension_count)
{
  guint nth;
  const gint opcode = 0x0d;
  const gint mt = 0x02;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* index pressure and data */
  buffer[nth] = (0xff & (pressure));
  nth++;
  
  buffer[nth] = (0xff & (data));
  nth++;
}

/**
 * ags_midi_ump_util_get_midi1_channel_pressure:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @pressure: (out): the return location of pressure
 * @data: (out): the return location of data
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 1.0 channel pressure.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi1_channel_pressure(AgsMidiUmpUtil *midi_ump_util,
					     guchar *buffer,
					     gint *group,
					     gint *channel,
					     gint *pressure,
					     gint *data,
					     gchar ***extension_name, GValue **extension_value,
					     guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x20, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0xd0, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
		       
  nth = 2;

  /* pressure */
  if(pressure != NULL){
    pressure[0] = 0xff & buffer[nth];
  }

  nth++;

  /* data */
  if(data != NULL){
    data[0] = 0xff & buffer[nth];
  }

  nth++;

  return(nth);
}

/**
 * ags_midi_ump_util_is_midi1_pitch_bend:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 1.0 pitch bend.
 * 
 * Returns: %TRUE if is MIDI version 1.0 pitch bend, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi1_pitch_bend(AgsMidiUmpUtil *midi_ump_util,
				      guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x20 &&
     (0xf0 & (buffer[1])) == 0xe0){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi1_pitch_bend:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @pitch: the pitch
 * @data: the data
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 1.0 pitch bend.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi1_pitch_bend(AgsMidiUmpUtil *midi_ump_util,
				       guchar *buffer,
				       gint group,
				       gint channel,
				       gint pitch,
				       gint data,
				       gchar **extension_name, GValue *extension_value,
				       guint extension_count)
{
  guint nth;
  const gint opcode = 0x0e;
  const gint mt = 0x02;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* index pitch and data */
  buffer[nth] = (0xff & (pitch));
  nth++;
  
  buffer[nth] = (0xff & (data));
  buffer[nth] = (0xff00 & (data)) >> 8;  
  nth += 2;
}

/**
 * ags_midi_ump_util_get_midi1_pitch_bend:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @pitch: (out): the return location of pitch
 * @data: (out): the return location of data
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 1.0 channel pitch.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi1_pitch_bend(AgsMidiUmpUtil *midi_ump_util,
				       guchar *buffer,
				       gint *group,
				       gint *channel,
				       gint *pitch,
				       gint *data,
				       gchar ***extension_name, GValue **extension_value,
				       guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x20, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0xe0, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
		       
  nth = 2;

  /* pitch */
  if(pitch != NULL){
    pitch[0] = 0xff & buffer[nth];
  }

  nth++;

  /* data */
  if(data != NULL){
    data[0] = (0xff00 & (buffer[nth + 1] << 8)) | (0xff & buffer[nth]);
  }

  nth += 2;

  return(nth);
}

/**
 * ags_midi_ump_util_is_midi2_channel_voice:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 channel voice.
 * 
 * Returns: %TRUE if is MIDI version 2.0 channel voice, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi2_channel_voice(AgsMidiUmpUtil *midi_ump_util,
					 guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x40){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_is_midi2_note_off:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 note off.
 * 
 * Returns: %TRUE if is MIDI version 2.0 note off, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi2_note_off(AgsMidiUmpUtil *midi_ump_util,
				    guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x40 &&
     (0xf0 & (buffer[1])) == 0x80){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi2_note_off:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @key: the index key
 * @attribute_type: the attribute type
 * @velocity: the velocity
 * @attribute: the attribute
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 note off.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi2_note_off(AgsMidiUmpUtil *midi_ump_util,
				     guchar *buffer,
				     gint group,
				     gint channel,
				     gint key,
				     gint attribute_type,
				     gint velocity,
				     gint attribute,
				     gchar **extension_name, GValue *extension_value,
				     guint extension_count)
{
  guint nth;
  const gint opcode = 0x08;
  const gint mt = 0x04;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* key */
  buffer[nth] = (0xff & (key));
  nth++;

  /* attribute type */
  buffer[nth] = (0xff & (attribute_type));
  nth++;

  /* velocity */
  buffer[nth] = (0xff & (velocity >> 8));
  nth++;

  buffer[nth] = (0xff & (velocity));
  nth++;

  /* attribute */
  buffer[nth] = (0xff & (attribute >> 8));
  nth++;

  buffer[nth] = (0xff & (attribute));
  nth++;
}

/**
 * ags_midi_ump_util_get_midi2_note_off:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @key: (out): the return location of key
 * @attribute_type: (out): the return location of attribute type
 * @velocity: (out): the return location of velocity
 * @attribute: (out): the return location of attribute
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 2.0 note off.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi2_note_off(AgsMidiUmpUtil *midi_ump_util,
				     guchar *buffer,
				     gint *group,
				     gint *channel,
				     gint *key,
				     gint *attribute_type,
				     gint *velocity,
				     gint *attribute,
				     gchar ***extension_name, GValue **extension_value,
				     guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x40, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0x80, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 2;
  
  /* key */
  if(key != NULL){
    key[0] = 0xff & buffer[nth];
  }

  nth++;

  /* attribute type */
  if(attribute_type != NULL){
    attribute_type[0] = 0xff & buffer[nth];
  }

  nth++;

  /* velocity */
  if(velocity != NULL){
    velocity[0] = 0xff & buffer[nth];
  }

  nth++;

  /* attribute */
  if(attribute != NULL){
    attribute[0] = 0xff & buffer[nth];
  }

  nth++;

  return(nth);
}

/**
 * ags_midi_ump_util_is_midi2_note_on:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 note on.
 * 
 * Returns: %TRUE if is MIDI version 2.0 note on, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi2_note_on(AgsMidiUmpUtil *midi_ump_util,
				   guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x40 &&
     (0xf0 & (buffer[1])) == 0x90){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi2_note_on:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @key: the index key
 * @attribute_type: the attribute type
 * @velocity: the velocity
 * @attribute: the attribute
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 note on.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi2_note_on(AgsMidiUmpUtil *midi_ump_util,
				    guchar *buffer,
				    gint group,
				    gint channel,
				    gint key,
				    gint attribute_type,
				    gint velocity,
				    gint attribute,
				    gchar **extension_name, GValue *extension_value,
				    guint extension_count)
{
  guint nth;
  const gint opcode = 0x09;
  const gint mt = 0x04;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* key */
  buffer[nth] = (0xff & (key));
  nth++;

  /* attribute type */
  buffer[nth] = (0xff & (attribute_type));
  nth++;

  /* velocity */
  buffer[nth] = (0xff & (velocity >> 8));
  nth++;

  buffer[nth] = (0xff & (velocity));
  nth++;

  /* attribute */
  buffer[nth] = (0xff & (attribute >> 8));
  nth++;

  buffer[nth] = (0xff & (attribute));
  nth++;
}

/**
 * ags_midi_ump_util_get_midi2_note_on:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @key: (out): the return location of key
 * @attribute_type: (out): the return location of attribute type
 * @velocity: (out): the return location of velocity
 * @attribute: (out): the return location of attribute
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 2.0 note on.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi2_note_on(AgsMidiUmpUtil *midi_ump_util,
				    guchar *buffer,
				    gint *group,
				    gint *channel,
				    gint *key,
				    gint *attribute_type,
				    gint *velocity,
				    gint *attribute,
				    gchar ***extension_name, GValue **extension_value,
				    guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x40, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0x90, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 2;
  
  /* key */
  if(key != NULL){
    key[0] = 0xff & buffer[nth];
  }

  nth++;

  /* attribute type */
  if(attribute_type != NULL){
    attribute_type[0] = 0xff & buffer[nth];
  }

  nth++;

  /* velocity */
  if(velocity != NULL){
    velocity[0] = 0xff & buffer[nth];
  }

  nth++;

  /* attribute */
  if(attribute != NULL){
    attribute[0] = 0xff & buffer[nth];
  }

  nth++;

  return(nth);
}

/**
 * ags_midi_ump_util_is_midi2_polyphonic_aftertouch:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 polyphonic aftertouch.
 * 
 * Returns: %TRUE if is MIDI version 2.0 polyphonic aftertouch, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi2_polyphonic_aftertouch(AgsMidiUmpUtil *midi_ump_util,
						 guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x40 &&
     (0xf0 & (buffer[1])) == 0xa0){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi2_polyphonic_aftertouch:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @key: the index key
 * @data: the data
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 polyphonic aftertouch.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi2_polyphonic_aftertouch(AgsMidiUmpUtil *midi_ump_util,
						  guchar *buffer,
						  gint group,
						  gint channel,
						  gint key,
						  gint data,
						  gchar **extension_name, GValue *extension_value,
						  guint extension_count)
{
  guint nth;
  const gint opcode = 0x0a;
  const gint mt = 0x04;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* index key */
  buffer[nth] = (0xff & (key));
  nth++;

  /* reserved */
  nth++;
  
  /* data */
  buffer[nth] = (0xff & (data >> 24));
  nth++;

  buffer[nth] = (0xff & (data >> 16));
  nth++;

  buffer[nth] = (0xff & (data >> 8));
  nth++;

  buffer[nth] = (0xff & (data));
  nth++;
}

/**
 * ags_midi_ump_util_get_midi2_polyphonic_aftertouch:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @key: (out): the return location of key
 * @data: (out): the return location of data
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 2.0 polyphonic aftertouch.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi2_polyphonic_aftertouch(AgsMidiUmpUtil *midi_ump_util,
						  guchar *buffer,
						  gint *group,
						  gint *channel,
						  gint *key,
						  gint *data,
						  gchar ***extension_name, GValue **extension_value,
						  guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x40, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0xa0, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 2;
  
  /* key */
  if(key != NULL){
    key[0] = 0xff & buffer[nth];
  }

  nth++;

  /* reserved */
  nth++;

  /* data */
  if(data != NULL){
    data[0] = (0xff000000 & (buffer[nth] << 24)) | (0xff0000 & (buffer[nth + 1] << 16)) | (0xff00 & (buffer[nth + 2] << 8)) | (0xff & buffer[nth + 3]);
  }

  nth += 4;

  return(nth);
}

/**
 * ags_midi_ump_util_is_midi2_registered_per_note_controller:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 registered per note controller.
 * 
 * Returns: %TRUE if is MIDI version 2.0 registered per note controller, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi2_registered_per_note_controller(AgsMidiUmpUtil *midi_ump_util,
							  guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x40 &&
     (0xf0 & (buffer[1])) == 0x00){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi2_registered_per_note_controller:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @key: the index key
 * @data_index: the data index
 * @data: the data
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 registered per note controller.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi2_registered_per_note_controller(AgsMidiUmpUtil *midi_ump_util,
							   guchar *buffer,
							   gint group,
							   gint channel,
							   gint key,
							   gint data_index,
							   gint data,
							   gchar **extension_name, GValue *extension_value,
							   guint extension_count)
{
  guint nth;
  const gint opcode = 0x00;
  const gint mt = 0x04;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* key */
  buffer[nth] = (0xff & (key));
  nth++;

  /* data index */
  buffer[nth] = (0xff & (data_index));
  nth++;

  /* data */
  buffer[nth] = (0xff & (data >> 24));
  nth++;

  buffer[nth] = (0xff & (data >> 16));
  nth++;

  buffer[nth] = (0xff & (data >> 8));
  nth++;

  buffer[nth] = (0xff & (data));
  nth++;
}

/**
 * ags_midi_ump_util_get_midi2_registered_per_note_controller:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @key: (out): the return location of key
 * @data_index: (out): the return location of data index
 * @data: (out): the return location of data
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 2.0 registered per note controller.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi2_registered_per_note_controller(AgsMidiUmpUtil *midi_ump_util,
							   guchar *buffer,
							   gint *group,
							   gint *channel,
							   gint *key,
							   gint *data_index,
							   gint *data,
							   gchar ***extension_name, GValue **extension_value,
							   guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x40, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0x00, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 2;
  
  /* key */
  if(key != NULL){
    key[0] = 0xff & buffer[nth];
  }

  nth++;

  /* data index */
  if(data_index != NULL){
    data_index[0] = 0xff & buffer[nth];
  }

  nth++;

  /* data */
  if(data != NULL){
    data[0] = (0xff000000 & (buffer[nth] << 24)) | (0xff0000 & (buffer[nth + 1] << 16)) | (0xff00 & (buffer[nth + 2] << 8)) | (0xff & buffer[nth + 3]);
  }

  nth += 4;

  return(nth);
}

/**
 * ags_midi_ump_util_is_midi2_assignable_per_note_controller:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 assignable per note controller.
 * 
 * Returns: %TRUE if is MIDI version 2.0 assignable per note controller, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi2_assignable_per_note_controller(AgsMidiUmpUtil *midi_ump_util,
							  guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x40 &&
     (0xf0 & (buffer[1])) == 0x10){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi2_assignable_per_note_controller:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @key: the index key
 * @data_index: the data index
 * @data: the data
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 assignable per note controller.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi2_assignable_per_note_controller(AgsMidiUmpUtil *midi_ump_util,
							   guchar *buffer,
							   gint group,
							   gint channel,
							   gint key,
							   gint data_index,
							   gint data,
							   gchar **extension_name, GValue *extension_value,
							   guint extension_count)
{
  guint nth;
  const gint opcode = 0x01;
  const gint mt = 0x04;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* key */
  buffer[nth] = (0xff & (key));
  nth++;

  /* data index */
  buffer[nth] = (0xff & (data_index));
  nth++;

  /* data */
  buffer[nth] = (0xff & (data >> 24));
  nth++;

  buffer[nth] = (0xff & (data >> 16));
  nth++;

  buffer[nth] = (0xff & (data >> 8));
  nth++;

  buffer[nth] = (0xff & (data));
  nth++;
}

/**
 * ags_midi_ump_util_get_midi2_assignable_per_note_controller:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @key: (out): the return location of key
 * @attribute_type: (out): the return location of attribute type
 * @velocity: (out): the return location of velocity
 * @attribute: (out): the return location of attribute
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 2.0 assignable per note controller.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi2_assignable_per_note_controller(AgsMidiUmpUtil *midi_ump_util,
							   guchar *buffer,
							   gint *group,
							   gint *channel,
							   gint *key,
							   gint *data_index,
							   gint *data,
							   gchar ***extension_name, GValue **extension_value,
							   guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x40, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0x10, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 2;
  
  /* key */
  if(key != NULL){
    key[0] = 0xff & buffer[nth];
  }

  nth++;

  /* data index */
  if(data_index != NULL){
    data_index[0] = 0xff & buffer[nth];
  }

  nth++;

  /* data */
  if(data != NULL){
    data[0] = (0xff000000 & (buffer[nth] << 24)) | (0xff0000 & (buffer[nth + 1] << 16)) | (0xff00 & (buffer[nth + 2] << 8)) | (0xff & buffer[nth + 3]);
  }

  nth += 4;

  return(nth);
}

/**
 * ags_midi_ump_util_is_midi2_per_note_management:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 per note management.
 * 
 * Returns: %TRUE if is MIDI version 2.0 per note management, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi2_per_note_management(AgsMidiUmpUtil *midi_ump_util,
					       guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x40 &&
     (0xf0 & (buffer[1])) == 0xf0){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi2_per_note_management:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @key: the index key
 * @options_flags: the options flags
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 registered per note management.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi2_per_note_management(AgsMidiUmpUtil *midi_ump_util,
						guchar *buffer,
						gint group,
						gint channel,
						gint key,
						gint options_flags,
						gchar **extension_name, GValue *extension_value,
						guint extension_count)
{
  guint nth;
  const gint opcode = 0x0f;
  const gint mt = 0x04;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* key */
  buffer[nth] = (0xff & (key));
  nth++;

  /* options flags */
  buffer[nth] = (0xff & (options_flags));
  nth++;

  /* reserved */
  nth += 4;
}

/**
 * ags_midi_ump_util_get_midi2_per_note_management:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @key: (out): the return location of key
 * @options_flags: (out): the return location of options flags
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 2.0 assignable per note controller.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi2_per_note_management(AgsMidiUmpUtil *midi_ump_util,
						guchar *buffer,
						gint *group,
						gint *channel,
						gint *key,
						gint *options_flags,
						gchar ***extension_name, GValue **extension_value,
						guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x40, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0xf0, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 2;
  
  /* key */
  if(key != NULL){
    key[0] = 0xff & buffer[nth];
  }

  nth++;

  /* options flags */
  if(options_flags != NULL){
    options_flags[0] = 0xff & buffer[nth];
  }

  nth++;

  /* reserved */
  nth += 4;

  return(nth);
}

/**
 * ags_midi_ump_util_is_midi2_control_change:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 control change.
 * 
 * Returns: %TRUE if is MIDI version 2.0 control change, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi2_control_change(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x40 &&
     (0xf0 & (buffer[1])) == 0xb0){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi2_control_change:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @index_key: the index key
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 control change.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi2_control_change(AgsMidiUmpUtil *midi_ump_util,
					   guchar *buffer,
					   gint group,
					   gint channel,
					   gint index_key,
					   gchar **extension_name, GValue *extension_value,
					   guint extension_count)
{
  guint nth;
  gint position;
  
  const gint opcode = 0x0b;
  const gint mt = 0x04;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* index key */
  buffer[nth] = (0xff & (index_key));
  nth++;

  /* reserved */
  buffer[nth] = 0x0;  
  nth++;

  /* data */
  buffer[nth] = 0x0;  
  buffer[nth + 1] = 0x0;  
  buffer[nth + 2] = 0x0;  
  buffer[nth + 3] = 0x0;  

  if((position = ags_strv_index(extension_name, "portamento")) >= 0){
    gint source_note_number;

    source_note_number = g_value_get_int(extension_value + position);

    buffer[nth] = 0xfe & (source_note_number << 1);    
  }

  if((position = ags_strv_index(extension_name, "omni-off")) >= 0){
    gint channels_count;

    channels_count = g_value_get_int(extension_value + position);

    buffer[nth] = 0xfe & (channels_count << 1);
  }

  nth += 4;
}

/**
 * ags_midi_ump_util_get_midi2_control_change:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @index_key: (out): the return location of index key
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 2.0 control change.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi2_control_change(AgsMidiUmpUtil *midi_ump_util,
					   guchar *buffer,
					   gint *group,
					   gint *channel,
					   gint *index_key,
					   gchar ***extension_name, GValue **extension_value,
					   guint *extension_count)
{
  gint nth;
  gint position;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x40, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0xb0, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 2;
  
  /* index key */
  if(index_key != NULL){
    index_key[0] = 0xff & buffer[nth];
  }

  nth++;

  /* reserved */
  nth++;

  /* data */
  if((position = ags_strv_index(extension_name, "portamento")) >= 0){
    gpointer source_note_number;

    source_note_number = g_value_get_pointer(extension_value + position);

    if(source_note_number != NULL){
      ((gint *) source_note_number)[0] = (0xfe & buffer[nth]) >> 1;
    }
  }

  if((position = ags_strv_index(extension_name, "omni-off")) >= 0){
    gpointer channels_count;

    channels_count = g_value_get_pointer(extension_value + position);

    if(channels_count != NULL){
      ((gint *) channels_count)[0] = (0xfe & buffer[nth]) >> 1;
    }
  }

  nth += 4;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_midi2_rpn_pitch_bend_range:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 RPN pitch bend range.
 * 
 * Returns: %TRUE if is MIDI version 2.0 RPN pitch bend range, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi2_rpn_pitch_bend_range(AgsMidiUmpUtil *midi_ump_util,
						guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x40 &&
     (0xf0 & (buffer[1])) == 0x20 &&
     (0xff & (buffer[2])) == 0x00 &&
     (0xff & (buffer[3])) == 0x00){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi2_rpn_pitch_bend_range:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @semitones: the semi tones
 * @cents: the cents
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 RPN pitch bend range.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi2_rpn_pitch_bend_range(AgsMidiUmpUtil *midi_ump_util,
						 guchar *buffer,
						 gint group,
						 gint channel,
						 gint semitones,
						 gint cents,
						 gchar **extension_name, GValue *extension_value,
						 guint extension_count)
{
  guint nth;
  gint position;

  const gint bank = 0x0;
  const gint index_key = 0x0;
  const gint opcode = 0x02;
  const gint mt = 0x04;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* bank */
  buffer[nth] = (0xff & (bank));
  nth++;

  /* index key */
  buffer[nth] = (0xff & (index_key));
  nth++;

  /* semitones */
  buffer[nth] = 0xfe & ((0x7f & (semitones)) << 1) | ((0x40 & (cents)) >> 6);
  nth++;

  /* cents */
  buffer[nth] = 0xfc & ((0x7f & (cents)) << 2);
  nth++;

  /* undefined */
  buffer[nth] = 0x0;  
  buffer[nth + 1] = 0x0;  

  nth += 2;
}

/**
 * ags_midi_ump_util_get_midi2_rpn_pitch_bend_range:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @semitones: (out): the return location of semitones
 * @cents: (out): the return location of cents
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 2.0 RPN pitch bend range.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi2_rpn_pitch_bend_range(AgsMidiUmpUtil *midi_ump_util,
						 guchar *buffer,
						 gint *group,
						 gint *channel,
						 gint *semitones,
						 gint *cents,
						 gchar ***extension_name, GValue **extension_value,
						 guint *extension_count)
{
  gint nth;
  gint position;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x40, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0x20, 0);
  g_return_val_if_fail((0xff & buffer[2]) == 0x00, 0);
  g_return_val_if_fail((0xff & buffer[3]) == 0x00, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 4;
  
  /* semitones and cents */
  if(semitones != NULL){
    semitones[0] = (0xfe & buffer[nth]) >> 1;
  }
  
  if(cents != NULL){
    cents[0] = (0x01 & buffer[nth] >> 1) | ((0xfc & buffer[nth + 1]) >> 2);
  }

  nth += 2;

  /* undefined */
  nth += 2;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_midi2_rpn_coarse_tuning:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 RPN coarse tuning.
 * 
 * Returns: %TRUE if is MIDI version 2.0 RPN coarse tuning, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi2_rpn_coarse_tuning(AgsMidiUmpUtil *midi_ump_util,
					     guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x40 &&
     (0xf0 & (buffer[1])) == 0x20 &&
     (0xff & (buffer[2])) == 0x00 &&
     (0xff & (buffer[3])) == 0x02){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi2_rpn_coarse_tuning:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @coarse_tuning: the coarse tuning
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 RPN coarse tuning.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi2_rpn_coarse_tuning(AgsMidiUmpUtil *midi_ump_util,
					      guchar *buffer,
					      gint group,
					      gint channel,
					      gint coarse_tuning,
					      gchar **extension_name, GValue *extension_value,
					      guint extension_count)
{
  guint nth;
  gint position;

  const gint bank = 0x0;
  const gint index_key = 0x02;
  const gint opcode = 0x02;
  const gint mt = 0x04;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* bank */
  buffer[nth] = (0xff & (bank));
  nth++;

  /* index key */
  buffer[nth] = (0xff & (index_key));
  nth++;

  /* coarse tuning */
  buffer[nth] = 0xfe & ((0x7f & (coarse_tuning)) << 1);
  nth++;

  /* undefined */
  buffer[nth] = 0x0;  
  buffer[nth + 1] = 0x0;  
  buffer[nth + 2] = 0x0;  

  nth += 3;
}

/**
 * ags_midi_ump_util_get_midi2_rpn_coarse_tuning:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @coarse_tuning: (out): the return location of coarse tuning
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 2.0 RPN coarse tuning.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi2_rpn_coarse_tuning(AgsMidiUmpUtil *midi_ump_util,
					      guchar *buffer,
					      gint *group,
					      gint *channel,
					      gint *coarse_tuning,
					      gchar ***extension_name, GValue **extension_value,
					      guint *extension_count)
{
  gint nth;
  gint position;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x40, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0x20, 0);
  g_return_val_if_fail((0xff & buffer[2]) == 0x00, 0);
  g_return_val_if_fail((0xff & buffer[3]) == 0x02, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 4;
  
  /* coarse tuning */
  if(coarse_tuning != NULL){
    coarse_tuning[0] = ((0xfe & buffer[nth]) >> 1);
  }

  nth += 1;

  /* undefined */
  nth += 3;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_midi2_rpn_tuning_program_change:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 RPN tuning program change.
 * 
 * Returns: %TRUE if is MIDI version 2.0 RPN tuning program change, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi2_rpn_tuning_program_change(AgsMidiUmpUtil *midi_ump_util,
						     guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x40 &&
     (0xf0 & (buffer[1])) == 0x20 &&
     (0xff & (buffer[2])) == 0x00 &&
     (0xff & (buffer[3])) == 0x03){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi2_rpn_tuning_program_change:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @tuning_program_number: the tuning program number
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 RPN tuning program change.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi2_rpn_tuning_program_change(AgsMidiUmpUtil *midi_ump_util,
						      guchar *buffer,
						      gint group,
						      gint channel,
						      gint tuning_program_number,
						      gchar **extension_name, GValue *extension_value,
						      guint extension_count)
{
  guint nth;
  gint position;

  const gint bank = 0x0;
  const gint index_key = 0x03;
  const gint opcode = 0x02;
  const gint mt = 0x04;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* bank */
  buffer[nth] = (0xff & (bank));
  nth++;

  /* index key */
  buffer[nth] = (0xff & (index_key));
  nth++;

  /* tuning program number */
  buffer[nth] = 0xfc & ((0x7f & (tuning_program_number)) << 1);
  nth++;

  /* undefined */
  buffer[nth] = 0x0;  
  buffer[nth + 1] = 0x0;  

  nth += 2;
}

/**
 * ags_midi_ump_util_get_midi2_rpn_tuning_program_change:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @tuning_program_number: (out): the return location of tuning program number
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 2.0 RPN tuning program change.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi2_rpn_tuning_program_change(AgsMidiUmpUtil *midi_ump_util,
						      guchar *buffer,
						      gint *group,
						      gint *channel,
						      gint *tuning_program_number,
						      gchar ***extension_name, GValue **extension_value,
						      guint *extension_count)
{
  gint nth;
  gint position;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x40, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0x20, 0);
  g_return_val_if_fail((0xff & buffer[2]) == 0x00, 0);
  g_return_val_if_fail((0xff & buffer[3]) == 0x03, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 4;
  
  /* tuning program number */
  if(tuning_program_number != NULL){
    tuning_program_number[0] = ((0xfe & buffer[nth]) >> 1);
  }

  nth += 1;

  /* undefined */
  nth += 3;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_midi2_rpn_tuning_bank_select:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 RPN tuning bank select.
 * 
 * Returns: %TRUE if is MIDI version 2.0 RPN tuning bank select, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi2_rpn_tuning_bank_select(AgsMidiUmpUtil *midi_ump_util,
						  guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x40 &&
     (0xf0 & (buffer[1])) == 0x20 &&
     (0xff & (buffer[2])) == 0x00 &&
     (0xff & (buffer[3])) == 0x04){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi2_rpn_tuning_bank_select:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @tuning_bank_number: the tuning bank number
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 RPN tuning bank select.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi2_rpn_tuning_bank_select(AgsMidiUmpUtil *midi_ump_util,
						   guchar *buffer,
						   gint group,
						   gint channel,
						   gint tuning_bank_number,
						   gchar **extension_name, GValue *extension_value,
						   guint extension_count)
{
  guint nth;
  gint position;

  const gint bank = 0x0;
  const gint index_key = 0x04;
  const gint opcode = 0x02;
  const gint mt = 0x04;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* bank */
  buffer[nth] = (0xff & (bank));
  nth++;

  /* index key */
  buffer[nth] = (0xff & (index_key));
  nth++;

  /* tuning bank number */
  buffer[nth] = 0xfc & ((0x7f & (tuning_bank_number)) << 1);
  nth++;

  /* undefined */
  buffer[nth] = 0x0;  
  buffer[nth + 1] = 0x0;  

  nth += 2;
}

/**
 * ags_midi_ump_util_get_midi2_rpn_tuning_bank_select:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @tuning_bank_number: (out): the return location of tuning bank number
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 2.0 RPN tuning bank select.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi2_rpn_tuning_bank_select(AgsMidiUmpUtil *midi_ump_util,
						   guchar *buffer,
						   gint *group,
						   gint *channel,
						   gint *tuning_bank_number,
						   gchar ***extension_name, GValue **extension_value,
						   guint *extension_count)
{
  gint nth;
  gint position;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x40, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0x20, 0);
  g_return_val_if_fail((0xff & buffer[2]) == 0x00, 0);
  g_return_val_if_fail((0xff & buffer[3]) == 0x04, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 4;
  
  /* tuning bank number */
  if(tuning_bank_number != NULL){
    tuning_bank_number[0] = ((0xfe & buffer[nth]) >> 1);
  }

  nth += 1;

  /* undefined */
  nth += 3;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_midi2_rpn_mpe_mcm:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 RPN MPE MCM.
 * 
 * Returns: %TRUE if is MIDI version 2.0 RPN MPE MCM, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi2_rpn_mpe_mcm(AgsMidiUmpUtil *midi_ump_util,
				       guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x40 &&
     (0xf0 & (buffer[1])) == 0x20 &&
     (0xff & (buffer[2])) == 0x00 &&
     (0xff & (buffer[3])) == 0x06){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi2_rpn_mpe_mcm:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @channel_count: the tuning bank number
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 RPN MPE MCM.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi2_rpn_mpe_mcm(AgsMidiUmpUtil *midi_ump_util,
					guchar *buffer,
					gint group,
					gint channel,
					gint channel_count,
					gchar **extension_name, GValue *extension_value,
					guint extension_count)
{
  guint nth;
  gint position;

  const gint bank = 0x0;
  const gint index_key = 0x06;
  const gint opcode = 0x02;
  const gint mt = 0x04;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* bank */
  buffer[nth] = (0xff & (bank));
  nth++;

  /* index key */
  buffer[nth] = (0xff & (index_key));
  nth++;

  /* channel count */
  buffer[nth] = 0xfc & ((0x7f & (channel_count)) << 1);
  nth++;

  /* undefined */
  buffer[nth] = 0x0;  
  buffer[nth + 1] = 0x0;  

  nth += 2;
}

/**
 * ags_midi_ump_util_get_midi2_rpn_mpe_mcm:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @channel_count: (out): the return location of channel count
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 2.0 RPN MPE MCM.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi2_rpn_mpe_mcm(AgsMidiUmpUtil *midi_ump_util,
					guchar *buffer,
					gint *group,
					gint *channel,
					gint *channel_count,
					gchar ***extension_name, GValue **extension_value,
					guint *extension_count)
{
  gint nth;
  gint position;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x40, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0x20, 0);
  g_return_val_if_fail((0xff & buffer[2]) == 0x00, 0);
  g_return_val_if_fail((0xff & buffer[3]) == 0x06, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 4;
  
  /* channel count */
  if(channel_count != NULL){
    channel_count[0] = ((0xfe & buffer[nth]) >> 1);
  }

  nth += 1;

  /* undefined */
  nth += 3;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_midi2_program_change:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 program change.
 * 
 * Returns: %TRUE if is MIDI version 2.0 program change, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi2_program_change(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x40 &&
     (0xf0 & (buffer[1])) == 0xc0){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi2_program_change:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @option_flags: the option flags
 * @program: the program
 * @bank: the bank
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 program change.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi2_program_change(AgsMidiUmpUtil *midi_ump_util,
					   guchar *buffer,
					   gint group,
					   gint channel,
					   gint option_flags,
					   gint program,
					   gint bank,
					   gchar **extension_name, GValue *extension_value,
					   guint extension_count)
{
  guint nth;
  gint position;

  const gint opcode = 0x0c;
  const gint mt = 0x04;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;

  /* reserved */
  buffer[nth] = 0x0;
  nth++;
  
  /* option flags */
  buffer[nth] = (0xff & (option_flags));
  nth++;

  /* program */
  buffer[nth] = (0xff & (program));
  nth++;

  /* reserved */
  buffer[nth] = 0x0;
  nth++;

  /* bank MSB */
  buffer[nth] = (0xff00 & (bank)) >> 8;
  nth++;

  /* bank LSB */
  buffer[nth] = (0xff & (bank));
  nth++;
}

/**
 * ags_midi_ump_util_get_midi2_program_change:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @option_flags: (out): the return location of option flags
 * @program: (out): the return location of program
 * @bank: (out): the return location of bank
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 2.0 program change.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi2_program_change(AgsMidiUmpUtil *midi_ump_util,
					   guchar *buffer,
					   gint *group,
					   gint *channel,
					   gint *option_flags,
					   gint *program,
					   gint *bank,
					   gchar ***extension_name, GValue **extension_value,
					   guint *extension_count)
{
  gint nth;
  gint position;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x40, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0xc0, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 2;

  /* reserved */
  nth++;
  
  /* option flags */
  if(option_flags != NULL){
    option_flags[0] = 0xff & buffer[nth];
  }

  nth++;

  /* program */
  if(program != NULL){
    program[0] = 0xff & buffer[nth];
  }

  nth++;

  /* reserved */
  nth++;
  
  /* bank */
  if(bank != NULL){
    bank[0] = (0xff00 & (buffer[nth] << 8)) | (0xff & (buffer[nth + 1]));
  }

  nth += 2;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_midi2_channel_pressure:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 channel pressure.
 * 
 * Returns: %TRUE if is MIDI version 2.0 channel pressure, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi2_channel_pressure(AgsMidiUmpUtil *midi_ump_util,
					    guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x40 &&
     (0xf0 & (buffer[1])) == 0xd0){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi2_channel_pressure:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @data: the data
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 channel pressure.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi2_channel_pressure(AgsMidiUmpUtil *midi_ump_util,
					     guchar *buffer,
					     gint group,
					     gint channel,
					     gint data,
					     gchar **extension_name, GValue *extension_value,
					     guint extension_count)
{
  guint nth;
  gint position;

  const gint opcode = 0x0d;
  const gint mt = 0x04;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;

  /* reserved */
  buffer[nth] = 0x0;
  nth++;
  
  /* reserved */
  buffer[nth] = 0x0;
  nth++;

  /* data */
  buffer[nth] = (0xff & (data >> 24));
  nth++;

  buffer[nth] = (0xff & (data >> 16));
  nth++;

  buffer[nth] = (0xff & (data >> 8));
  nth++;

  buffer[nth] = (0xff & (data));
  nth++;
}

/**
 * ags_midi_ump_util_get_midi2_channel_pressure:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @data: (out): the return location of data
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 2.0 channel pressure.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi2_channel_pressure(AgsMidiUmpUtil *midi_ump_util,
					     guchar *buffer,
					     gint *group,
					     gint *channel,
					     gint *data,
					     gchar ***extension_name, GValue **extension_value,
					     guint *extension_count)
{
  gint nth;
  gint position;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x40, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0xd0, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 2;

  /* reserved */
  nth += 2;
    
  /* data */
  if(data != NULL){
    data[0] = (0xff000000 & (buffer[nth] << 24)) | (0xff0000 & (buffer[nth + 1] << 16)) | (0xff00 & (buffer[nth + 2] << 8)) | (0xff & (buffer[nth + 3]));
  }

  nth += 4;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_midi2_pitch_bend:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 pitch bend.
 * 
 * Returns: %TRUE if is MIDI version 2.0 pitch bend, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi2_pitch_bend(AgsMidiUmpUtil *midi_ump_util,
				      guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x40 &&
     (0xf0 & (buffer[1])) == 0xe0){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi2_pitch_bend:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @data: the data
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 pitch bend.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi2_pitch_bend(AgsMidiUmpUtil *midi_ump_util,
				       guchar *buffer,
				       gint group,
				       gint channel,
				       gint data,
				       gchar **extension_name, GValue *extension_value,
				       guint extension_count)
{
  guint nth;
  gint position;

  const gint opcode = 0x0e;
  const gint mt = 0x04;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;

  /* reserved */
  buffer[nth] = 0x0;
  nth++;
  
  /* reserved */
  buffer[nth] = 0x0;
  nth++;

  /* data */
  buffer[nth] = (0xff & (data >> 24));
  nth++;

  buffer[nth] = (0xff & (data >> 16));
  nth++;

  buffer[nth] = (0xff & (data >> 8));
  nth++;

  buffer[nth] = (0xff & (data));
  nth++;
}

/**
 * ags_midi_ump_util_get_midi2_pitch_bend:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @data: (out): the return location of data
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 2.0 pitch bend.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi2_pitch_bend(AgsMidiUmpUtil *midi_ump_util,
				       guchar *buffer,
				       gint *group,
				       gint *channel,
				       gint *data,
				       gchar ***extension_name, GValue **extension_value,
				       guint *extension_count)
{
  gint nth;
  gint position;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x40, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0xe0, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 2;

  /* reserved */
  nth += 2;
    
  /* data */
  if(data != NULL){
    data[0] = (0xff000000 & (buffer[nth] << 24)) | (0xff0000 & (buffer[nth + 1] << 16)) | (0xff00 & (buffer[nth + 2] << 8)) | (0xff & (buffer[nth + 3]));
  }

  nth += 4;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_midi2_per_note_pitch_bend:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 pitch bend.
 * 
 * Returns: %TRUE if is MIDI version 2.0 pitch bend, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_midi2_per_note_pitch_bend(AgsMidiUmpUtil *midi_ump_util,
					       guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x40 &&
     (0xf0 & (buffer[1])) == 0x60){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_midi2_per_note_pitch_bend:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @key: the key
 * @data: the data
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 pitch bend.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi2_per_note_pitch_bend(AgsMidiUmpUtil *midi_ump_util,
						guchar *buffer,
						gint group,
						gint channel,
						gint key,
						gint data,
						gchar **extension_name, GValue *extension_value,
						guint extension_count)
{
  guint nth;
  gint position;

  const gint opcode = 0x06;
  const gint mt = 0x04;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;

  /* key */
  buffer[nth] = 0xff & (key);
  nth++;
  
  /* reserved */
  buffer[nth] = 0x0;
  nth++;

  /* data */
  buffer[nth] = (0xff & (data >> 24));
  nth++;

  buffer[nth] = (0xff & (data >> 16));
  nth++;

  buffer[nth] = (0xff & (data >> 8));
  nth++;

  buffer[nth] = (0xff & (data));
  nth++;
}

/**
 * ags_midi_ump_util_get_midi2_per_note_pitch_bend:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @key: (out): the return location of key
 * @data: (out): the return location of data
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get MIDI version 2.0 per note pitch bend.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_midi2_per_note_pitch_bend(AgsMidiUmpUtil *midi_ump_util,
						guchar *buffer,
						gint *group,
						gint *channel,
						gint *key,
						gint *data,
						gchar ***extension_name, GValue **extension_value,
						guint *extension_count)
{
  gint nth;
  gint position;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0x40, 0);
  g_return_val_if_fail((0xf0 & buffer[1]) == 0x60, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 2;

  /* key */
  if(key != NULL){
    key[0] = 0xff & (buffer[nth]);
  }

  nth++;
  
  /* reserved */
  nth++;
    
  /* data */
  if(data != NULL){
    data[0] = (0xff000000 & (buffer[nth] << 24)) | (0xff0000 & (buffer[nth + 1] << 16)) | (0xff00 & (buffer[nth + 2] << 8)) | (0xff & (buffer[nth + 3]));
  }

  nth += 4;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_flex_set_tempo:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 flex set tempo.
 * 
 * Returns: %TRUE if is MIDI version 2.0 flex set tempo, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_flex_set_tempo(AgsMidiUmpUtil *midi_ump_util,
				    guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xd0 &&
     (0x30 & (buffer[1])) == 0x10 &&
     (0x30 & (buffer[2])) == 0x00 &&
     (0x30 & (buffer[3])) == 0x00){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_flex_set_tempo:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @ten_ns_per_quarter_note: the 10 ns per quarter note
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 flex set tempo.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_flex_set_tempo(AgsMidiUmpUtil *midi_ump_util,
				     guchar *buffer,
				     gint group,
				     gint channel,
				     gint ten_ns_per_quarter_note,
				     gchar **extension_name, GValue *extension_value,
				     guint extension_count)
{
  guint nth;
  gint position;

  const gint status_bank = 0;
  const gint status = 0;
  const gint form = 0x00;
  const gint addr = 0x01;
  const gint mt = 0x0d;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xc0 & (form << 6)) | (0x30 & (addr << 4)) | (0x0f & (channel));
  nth++;

  /* status bank */
  buffer[nth] = 0xff & (status_bank);
  nth++;

  /* status */
  buffer[nth] = 0xff & (status);
  nth++;

  /* ten ns per quarter note, */
  buffer[nth] = (0xff & (ten_ns_per_quarter_note >> 24));
  nth++;

  buffer[nth] = (0xff & (ten_ns_per_quarter_note >> 16));
  nth++;

  buffer[nth] = (0xff & (ten_ns_per_quarter_note >> 8));
  nth++;

  buffer[nth] = (0xff & (ten_ns_per_quarter_note));
  nth++;

  /* reserved */
  memset(buffer + nth, 0, 8 * sizeof(guchar));
  nth += 8;
}

/**
 * ags_midi_ump_util_get_flex_set_tempo:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @ten_ns_per_quarter_note: (out): the return location of 10 ns per quarter note
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get flex set tempo.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_flex_set_tempo(AgsMidiUmpUtil *midi_ump_util,
				     guchar *buffer,
				     gint *group,
				     gint *channel,
				     gint *ten_ns_per_quarter_note,
				     gchar ***extension_name, GValue **extension_value,
				     guint *extension_count)
{
  gint nth;
  gint position;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0xd0, 0);
  g_return_val_if_fail((0x30 & buffer[1]) == 0x10, 0);
  g_return_val_if_fail((0x30 & buffer[2]) == 0x00, 0);
  g_return_val_if_fail((0x30 & buffer[3]) == 0x00, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 4;
    
  /* 10 ns per quarter note */
  if(ten_ns_per_quarter_note != NULL){
    ten_ns_per_quarter_note[0] = (0xff000000 & (buffer[nth] << 24)) | (0xff0000 & (buffer[nth + 1] << 16)) | (0xff00 & (buffer[nth + 2] << 8)) | (0xff & (buffer[nth + 3]));
  }

  nth += 4;

  /* reserved */
  nth += 8;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_flex_set_time_signature:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 flex set time_signature.
 * 
 * Returns: %TRUE if is MIDI version 2.0 flex set time_signature, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_flex_set_time_signature(AgsMidiUmpUtil *midi_ump_util,
					     guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xd0 &&
     (0x30 & (buffer[1])) == 0x10 &&
     (0x30 & (buffer[2])) == 0x00 &&
     (0x30 & (buffer[3])) == 0x01){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_flex_set_time_signature:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @numerator: the numerator
 * @denominator: the denominator
 * @thirty_two_ticks: the 1/32 ticks
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 flex set time_signature.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_flex_set_time_signature(AgsMidiUmpUtil *midi_ump_util,
					      guchar *buffer,
					      gint group,
					      gint channel,
					      gint numerator,
					      gint denominator,
					      gint thirty_two_ticks,
					      gchar **extension_name, GValue *extension_value,
					      guint extension_count)
{
  guint nth;
  gint position;

  const gint status_bank = 0;
  const gint status = 1;
  const gint form = 0x00;
  const gint addr = 0x01;
  const gint mt = 0x0d;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xc0 & (form << 6)) | (0x30 & (addr << 4)) | (0x0f & (channel));
  nth++;

  /* status bank */
  buffer[nth] = 0xff & (status_bank);
  nth++;

  /* status */
  buffer[nth] = 0xff & (status);
  nth++;

  /* numerator */
  buffer[nth] = 0xff & (numerator);
  nth++;

  /* denominator */
  buffer[nth] = 0xff & (denominator);
  nth++;

  /* 1/32 ticks */
  buffer[nth] = 0xff & (thirty_two_ticks);
  nth++;

  /* reserved */
  memset(buffer + nth, 0, 9 * sizeof(guchar));
  nth += 9;
}

/**
 * ags_midi_ump_util_get_flex_set_time_signature:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @numerator: (out): the return location of numerator
 * @denominator: (out): the return location of denominator
 * @thirty_two_ticks: (out): the return location of 1/32 ticks
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get flex set time signature.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_flex_set_time_signature(AgsMidiUmpUtil *midi_ump_util,
					      guchar *buffer,
					      gint *group,
					      gint *channel,
					      gint *numerator,
					      gint *denominator,
					      gint *thirty_two_ticks,
					      gchar ***extension_name, GValue **extension_value,
					      guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0xd0, 0);
  g_return_val_if_fail((0x30 & buffer[1]) == 0x10, 0);
  g_return_val_if_fail((0x30 & buffer[2]) == 0x00, 0);
  g_return_val_if_fail((0x30 & buffer[3]) == 0x01, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 4;

  /* numerator */
  if(numerator != NULL){
    numerator[0] = 0xff & buffer[nth];
  }

  nth++;

  /* denominator */
  if(denominator != NULL){
    denominator[0] = 0xff & buffer[nth];
  }

  nth++;

  /* 1/32 ticks */
  if(thirty_two_ticks != NULL){
    thirty_two_ticks[0] = 0xff & buffer[nth];
  }

  nth++;
  
  /* reserved */
  nth += 9;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_flex_set_metronome:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 flex set metronome.
 * 
 * Returns: %TRUE if is MIDI version 2.0 flex set metronome, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_flex_set_metronome(AgsMidiUmpUtil *midi_ump_util,
					guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xd0 &&
     (0x30 & (buffer[1])) == 0x10 &&
     (0x30 & (buffer[2])) == 0x00 &&
     (0x30 & (buffer[3])) == 0x02){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_flex_set_metronome:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @clocks_per_primary_click: the clocks per primary click
 * @bar_accent_part_1: the bar accent part 1
 * @bar_accent_part_2: the bar accent part 2
 * @bar_accent_part_3: the bar accent part 3
 * @subdivision_clicks_1: the subdivision clicks 1
 * @subdivision_clicks_2: the subdivision clicks 2
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 flex set metronome.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_flex_set_metronome(AgsMidiUmpUtil *midi_ump_util,
					 guchar *buffer,
					 gint group,
					 gint channel,
					 gint clocks_per_primary_click,
					 gint bar_accent_part_1,
					 gint bar_accent_part_2,
					 gint bar_accent_part_3,
					 gint subdivision_clicks_1,
					 gint subdivision_clicks_2,
					 gchar **extension_name, GValue *extension_value,
					 guint extension_count)
{
  guint nth;
  gint position;

  const gint status_bank = 0x0;
  const gint status = 0x2;
  const gint form = 0x00;
  const gint addr = 0x01;
  const gint mt = 0x0d;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xc0 & (form << 6)) | (0x30 & (addr << 4)) | (0x0f & (channel));
  nth++;

  /* status bank */
  buffer[nth] = 0xff & (status_bank);
  nth++;

  /* status */
  buffer[nth] = 0xff & (status);
  nth++;

  /* clocks per primary click */
  buffer[nth] = 0xff & (clocks_per_primary_click);
  nth++;

  /* bar accent part 1 */
  buffer[nth] = 0xff & (bar_accent_part_1);
  nth++;

  /* bar accent part 2 */
  buffer[nth] = 0xff & (bar_accent_part_2);
  nth++;

  /* bar accent part 3 */
  buffer[nth] = 0xff & (bar_accent_part_3);
  nth++;

  /* subdivision clicks 1 */
  buffer[nth] = 0xff & (subdivision_clicks_1);
  nth++;

  /* subdivision clicks 2 */
  buffer[nth] = 0xff & (subdivision_clicks_2);
  nth++;

  /* reserved */
  memset(buffer + nth, 0, 6 * sizeof(guchar));
  nth += 6;
}

/**
 * ags_midi_ump_util_get_flex_set_metronome:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @clocks_per_primary_click: (out): the return location of clocks per primary click
 * @bar_accent_part_1: (out): the return location of bar accent part 1
 * @bar_accent_part_2: (out): the return location of bar accent part 2
 * @bar_accent_part_3: (out): the return location of bar accent part 3
 * @subdivision_clicks_1: (out): the return location of subdivision clicks 1
 * @subdivision_clicks_2: (out): the return location of subdivision clicks 2
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get flex set metronome.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_flex_set_metronome(AgsMidiUmpUtil *midi_ump_util,
					 guchar *buffer,
					 gint *group,
					 gint *channel,
					 gint *clocks_per_primary_click,
					 gint *bar_accent_part_1,
					 gint *bar_accent_part_2,
					 gint *bar_accent_part_3,
					 gint *subdivision_clicks_1,
					 gint *subdivision_clicks_2,
					 gchar ***extension_name, GValue **extension_value,
					 guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0xd0, 0);
  g_return_val_if_fail((0x30 & buffer[1]) == 0x10, 0);
  g_return_val_if_fail((0x30 & buffer[2]) == 0x00, 0);
  g_return_val_if_fail((0x30 & buffer[3]) == 0x02, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 4;

  /* clocks per primary click */
  if(clocks_per_primary_click != NULL){
    clocks_per_primary_click[0] = 0xff & buffer[nth];
  }

  nth++;
  
  /* bar accent part 1 */
  if(bar_accent_part_1 != NULL){
    bar_accent_part_1[0] = 0xff & buffer[nth];
  }

  nth++;
  
  /* bar accent part 2 */
  if(bar_accent_part_2 != NULL){
    bar_accent_part_2[0] = 0xff & buffer[nth];
  }

  nth++;
  
  /* bar accent part 3 */
  if(bar_accent_part_3 != NULL){
    bar_accent_part_3[0] = 0xff & buffer[nth];
  }

  nth++;
  
  /* subdivision clicks 1 */
  if(subdivision_clicks_1 != NULL){
    subdivision_clicks_1[0] = 0xff & buffer[nth];
  }

  nth++;
  
  /* subdivision clicks 2 */
  if(subdivision_clicks_2 != NULL){
    subdivision_clicks_2[0] = 0xff & buffer[nth];
  }

  nth++;

  /* reserved */
  nth += 6;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_flex_set_key_signature:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 flex set key signature.
 * 
 * Returns: %TRUE if is MIDI version 2.0 flex set key signature, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_flex_set_key_signature(AgsMidiUmpUtil *midi_ump_util,
					    guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xd0 &&
     (0x30 & (buffer[1])) == 0x10 &&
     (0x30 & (buffer[2])) == 0x00 &&
     (0x30 & (buffer[3])) == 0x05){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_flex_set_key_signature:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @sharp_flats: the sharp flats count
 * @tonic_note: the tonic note
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 flex set key signature.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_flex_set_key_signature(AgsMidiUmpUtil *midi_ump_util,
					     guchar *buffer,
					     gint group,
					     gint channel,
					     gint sharp_flats,
					     gint tonic_note,
					     gchar **extension_name, GValue *extension_value,
					     guint extension_count)
{
  guint nth;
  gint position;

  const gint status_bank = 0x0;
  const gint status = 0x05;
  const gint form = 0x00;
  const gint addr = 0x01;
  const gint mt = 0x0d;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xc0 & (form << 6)) | (0x30 & (addr << 4)) | (0x0f & (channel));
  nth++;

  /* status bank */
  buffer[nth] = 0xff & (status_bank);
  nth++;

  /* status */
  buffer[nth] = 0xff & (status);
  nth++;

  /* sharp flats and tonic note */
  buffer[nth] = (0xf0 & (sharp_flats << 4)) | (0x0f & (tonic_note));
  nth++;

  /* reserved */
  memset(buffer + nth, 0, 11 * sizeof(guchar));
  nth += 11;
}

/**
 * ags_midi_ump_util_get_flex_set_key_signature:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @sharp_flats: (out): the return location of sharp flats
 * @tonic_note: (out): the return location of tonic note
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get flex set time signature.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_flex_set_key_signature(AgsMidiUmpUtil *midi_ump_util,
					     guchar *buffer,
					     gint *group,
					     gint *channel,
					     gint *sharp_flats,
					     gint *tonic_note,
					     gchar ***extension_name, GValue **extension_value,
					     guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0xd0, 0);
  g_return_val_if_fail((0x30 & buffer[1]) == 0x10, 0);
  g_return_val_if_fail((0x30 & buffer[2]) == 0x00, 0);
  g_return_val_if_fail((0x30 & buffer[3]) == 0x05, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 4;

  /* sharp flats and tonic note */
  if(sharp_flats != NULL){
    sharp_flats[0] = 0x0f & (buffer[nth] >> 4);
  }

  if(tonic_note != NULL){
    tonic_note[0] = 0x0f & buffer[nth];
  }

  nth++;

  /* reserved */
  nth += 13;
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_flex_set_chord_name:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 flex set chord name.
 * 
 * Returns: %TRUE if is MIDI version 2.0 flex set chord name, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_flex_set_chord_name(AgsMidiUmpUtil *midi_ump_util,
					    guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xd0 &&
     (0x30 & (buffer[1])) == 0x10 &&
     (0x30 & (buffer[2])) == 0x00 &&
     (0x30 & (buffer[3])) == 0x06){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_flex_set_chord_name:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @t_sharp_flats: the t sharp flats count
 * @chord_tonic: the chord tonic
 * @chord_type: the chord type
 * @alter_1_type: the alter 1 type
 * @alter_1_degree: the alter 1 degree
 * @alter_2_type: the alter 2 type
 * @alter_2_degree: the alter 2 degree
 * @alter_3_type: the alter 3 type
 * @alter_3_degree: the alter 3 degree
 * @alter_4_type: the alter 4 type
 * @alter_4_degree: the alter 4 degree
 * @b_sharp_flats: the b sharp flats count
 * @bass_note: the bass note
 * @bass_chord_type: the bass chord type
 * @b_alter_1_type: the alter 1 type
 * @b_alter_1_degree: the alter 1 degree
 * @b_alter_2_type: the alter 2 type
 * @b_alter_2_degree: the alter 2 degree
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 flex set chord name.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_flex_set_chord_name(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gint group,
					  gint channel,
					  gint t_sharp_flats,
					  gint chord_tonic,
					  gint chord_type,
					  gint alter_1_type,
					  gint alter_1_degree,
					  gint alter_2_type,
					  gint alter_2_degree,
					  gint alter_3_type,
					  gint alter_3_degree,
					  gint alter_4_type,
					  gint alter_4_degree,
					  gint b_sharp_flats,
					  gint bass_note,
					  gint bass_chord_type,
					  gint b_alter_1_type,
					  gint b_alter_1_degree,
					  gint b_alter_2_type,
					  gint b_alter_2_degree,
					  gchar **extension_name, GValue *extension_value,
					  guint extension_count)
{
  guint nth;
  gint position;

  const gint status_bank = 0x0;
  const gint status = 0x06;
  const gint form = 0x00;
  const gint addr = 0x01;
  const gint mt = 0x0d;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xc0 & (form << 6)) | (0x30 & (addr << 4)) | (0x0f & (channel));
  nth++;

  /* status bank */
  buffer[nth] = 0xff & (status_bank);
  nth++;

  /* status */
  buffer[nth] = 0xff & (status);
  nth++;

  /* sharp flats and chord tonic */
  buffer[nth] = (0xf0 & (t_sharp_flats << 4)) | (0x0f & (chord_tonic));
  nth++;

  /* chord type */
  buffer[nth] = 0xff & (chord_type);
  nth++;

  /* alter 1 type and degree */
  buffer[nth] = (0xf0 & (alter_1_type << 4)) | (0x0f & (alter_1_degree));
  nth++;

  /* alter 2 type and degree */
  buffer[nth] = (0xf0 & (alter_2_type << 4)) | (0x0f & (alter_2_degree));
  nth++;

  /* alter 3 type and degree */
  buffer[nth] = (0xf0 & (alter_3_type << 4)) | (0x0f & (alter_3_degree));
  nth++;

  /* alter 4 type and degree */
  buffer[nth] = (0xf0 & (alter_4_type << 4)) | (0x0f & (alter_4_degree));
  nth++;

  /* reserved */
  buffer[nth] = 0x0;
  nth++;

  buffer[nth] = 0x0;
  nth++;

  /* b sharp flats and bass note */
  buffer[nth] = (0xf0 & (b_sharp_flats << 4)) | (0x0f & (bass_note));
  nth++;

  /* bass chord type */
  buffer[nth] = 0xff & (bass_chord_type);
  nth++;

  /* alter 1 type and degree */
  buffer[nth] = (0xf0 & (b_alter_1_type << 4)) | (0x0f & (b_alter_1_degree));
  nth++;

  /* alter 2 type and degree */
  buffer[nth] = (0xf0 & (b_alter_2_type << 4)) | (0x0f & (b_alter_2_degree));
  nth++;
}

/**
 * ags_midi_ump_util_get_flex_set_chord_name:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: (out): the return location of group
 * @channel: (out): the return location of channel number
 * @t_sharp_flats: (out): the return location of t sharp flats
 * @chord_tonic: (out): the return location of chord tonic
 * @chord_type: (out): the return location of chord type
 * @alter_1_type: (out): the return location of alter 1 type
 * @alter_1_degree: (out): the return location of alter 1 degree
 * @alter_2_type: (out): the return location of alter 2 type
 * @alter_2_degree: (out): the return location of alter 2 degree
 * @alter_3_type: (out): the return location of alter 3 type
 * @alter_3_degree: (out): the return location of alter 3 degree
 * @alter_4_type: (out): the return location of alter 4 type
 * @alter_4_degree: (out): the return location of alter 4 degree
 * @b_sharp_flats: (out): the return location of b sharp flats
 * @bass_note: (out): the return location of bass note
 * @bass_chord_type: (out): the return location of bass chord type
 * @b_alter_1_type: (out): the return location of b alter 1 type
 * @b_alter_1_degree: (out): the return location of b alter 1 degree
 * @b_alter_2_type: (out): the return location of b alter 2 type
 * @b_alter_2_degree: (out): the return location of b alter 2 degree
 * @extension_name: (out): the return location of extension name string vector
 * @extension_value: (out): the return location of extension value array
 * @extension_count: (out): the return location of extension count
 *
 * Get flex set chord name.
 * 
 * Returns: the number of bytes read
 * 
 * Since: 5.5.4 
 */
guint
ags_midi_ump_util_get_flex_set_chord_name(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gint *group,
					  gint *channel,
					  gint *t_sharp_flats,
					  gint *chord_tonic,
					  gint *chord_type,
					  gint *alter_1_type,
					  gint *alter_1_degree,
					  gint *alter_2_type,
					  gint *alter_2_degree,
					  gint *alter_3_type,
					  gint *alter_3_degree,
					  gint *alter_4_type,
					  gint *alter_4_degree,
					  gint *b_sharp_flats,
					  gint *bass_note,
					  gint *bass_chord_type,
					  gint *b_alter_1_type,
					  gint *b_alter_1_degree,
					  gint *b_alter_2_type,
					  gint *b_alter_2_degree,
					  gchar ***extension_name, GValue **extension_value,
					  guint *extension_count)
{
  gint nth;
  
  g_return_val_if_fail(midi_ump_util != NULL, 0);     
  g_return_val_if_fail(buffer != NULL, 0);
  g_return_val_if_fail((0xf0 & buffer[0]) == 0xd0, 0);
  g_return_val_if_fail((0x30 & buffer[1]) == 0x10, 0);
  g_return_val_if_fail((0x30 & buffer[2]) == 0x00, 0);
  g_return_val_if_fail((0x30 & buffer[3]) == 0x06, 0);

  if(group != NULL){
    group[0] = 0x0f & buffer[0];
  }

  if(channel != NULL){
    channel[0] = 0x0f & buffer[1];
  }
  
  nth = 4;

  /* sharp flats and tonic note */
  if(t_sharp_flats != NULL){
    t_sharp_flats[0] = 0x0f & (buffer[nth] >> 4);
  }

  if(chord_tonic != NULL){
    chord_tonic[0] = 0x0f & buffer[nth];
  }

  nth++;

  /* chord type */
  if(chord_type != NULL){
    chord_type[0] = 0xff & buffer[nth];
  }

  nth++;

  /* alter 1 type and degree */
  if(alter_1_type != NULL){
    alter_1_type[0] = 0x0f & (buffer[nth] >> 4);
  }

  if(alter_1_degree != NULL){
    alter_1_degree[0] = 0x0f & (buffer[nth]);
  }

  nth++;

  /* alter 2 type and degree */
  if(alter_2_type != NULL){
    alter_2_type[0] = 0x0f & (buffer[nth] >> 4);
  }

  if(alter_2_degree != NULL){
    alter_2_degree[0] = 0x0f & (buffer[nth]);
  }

  nth++;

  /* alter 3 type and degree */
  if(alter_3_type != NULL){
    alter_3_type[0] = 0x0f & (buffer[nth] >> 4);
  }

  if(alter_3_degree != NULL){
    alter_3_degree[0] = 0x0f & (buffer[nth]);
  }

  nth++;

  /* alter 4 type and degree */
  if(alter_4_type != NULL){
    alter_4_type[0] = 0x0f & (buffer[nth] >> 4);
  }

  if(alter_4_degree != NULL){
    alter_4_degree[0] = 0x0f & (buffer[nth]);
  }

  nth++;

  /* reserved */
  nth += 2;

  /* b sharp flats and bass note */
  if(b_sharp_flats != NULL){
    b_sharp_flats[0] = 0x0f & (buffer[nth] >> 4);
  }

  if(bass_note != NULL){
    bass_note[0] = 0x0f & buffer[nth];
  }

  nth++;

  /* bass chord type */
  if(bass_chord_type != NULL){
    bass_chord_type[0] = 0xff & buffer[nth];
  }

  nth++;

  /* b alter 1 type and degree */
  if(b_alter_1_type != NULL){
    b_alter_1_type[0] = 0x0f & (buffer[nth] >> 4);
  }

  if(b_alter_1_degree != NULL){
    b_alter_1_degree[0] = 0x0f & (buffer[nth]);
  }

  nth++;

  /* b alter 2 type and degree */
  if(b_alter_2_type != NULL){
    b_alter_2_type[0] = 0x0f & (buffer[nth] >> 4);
  }

  if(b_alter_2_degree != NULL){
    b_alter_2_degree[0] = 0x0f & (buffer[nth]);
  }

  nth++;  
  
  return(nth);
}

/**
 * ags_midi_ump_util_is_flex_set_text:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 *
 * Test if is MIDI version 2.0 flex set text.
 * 
 * Returns: %TRUE if is MIDI version 2.0 flex set text, otherwise %FALSE
 * 
 * Since: 5.5.4
 */
gboolean
ags_midi_ump_util_is_flex_set_text(AgsMidiUmpUtil *midi_ump_util,
				   guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0xd0 &&
     (0x30 & (buffer[1])) == 0x10){
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_midi_ump_util_put_flex_set_text:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @channel: the channel number
 * @status_bank: the status bank
 * @status: the status
 * @text: the text
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 flex set text.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_flex_set_text(AgsMidiUmpUtil *midi_ump_util,
				    guchar *buffer,
				    gint group,
				    gint channel,
				    gint status_bank,
				    gint status,
				    gchar *text,
				    gchar **extension_name, GValue *extension_value,
				    guint extension_count)
{
  guint nth;
  gint position;

  const gint form = 0x00;
  const gint addr = 0x01;
  const gint mt = 0x0d;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xc0 & (form << 6)) | (0x30 & (addr << 4)) | (0x0f & (channel));
  nth++;

  /* status bank */
  buffer[nth] = 0xff & (status_bank);
  nth++;

  /* status */
  buffer[nth] = 0xff & (status);
  nth++;

  //TODO:JK: implement me
}

guint
ags_midi_ump_util_get_flex_set_text(AgsMidiUmpUtil *midi_ump_util,
				    guchar *buffer,
				    gint *group,
				    gint *channel,
				    gint *status_bank,
				    gint *status,
				    gchar **text,
				    gchar ***extension_name, GValue **extension_value,
				    guint *extension_count)
{
  //TODO:JK: implement me

  return(0);
}
