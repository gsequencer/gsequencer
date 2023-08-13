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

void
ags_midi_ump_util_put_system_common(AgsMidiUmpUtil *midi_ump_util,
				    guchar *buffer,
				    gint group,
				    gint status,
				    gchar **extension_name, GValue *extension_value,
				    guint extension_count)
{
  //TODO:JK: implement me
}

guint
ags_midi_ump_util_get_system_common(AgsMidiUmpUtil *midi_ump_util,
				    guchar *buffer,
				    gint *group,
				    gint *status,
				    gchar ***extension_name, GValue **extension_value,
				    guint *extension_count)
{
  //TODO:JK: implement me

  return(0);
}

gboolean
ags_midi_ump_util_is_midi1_channel_voice(AgsMidiUmpUtil *midi_ump_util,
					 guchar *buffer)
{
  if((0xf0 & (buffer[0])) == 0x20){
    return(TRUE);
  }
  
  return(FALSE);
}

void
ags_midi_ump_util_put_midi1_channel_voice(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gchar **extension_name, GValue *extension_value,
					  guint extension_count)
{
  //TODO:JK: implement me
}

guint
ags_midi_ump_util_get_midi1_channel_voice(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gchar ***extension_name, GValue **extension_value,
					  guint *extension_count)
{
  //TODO:JK: implement me

  return(0);
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

void
ags_midi_ump_util_put_data_message_with_sysex(AgsMidiUmpUtil *midi_ump_util,
					      guchar *buffer,
					      gint group,
					      gint status,
					      gchar **extension_name, GValue *extension_value,
					      guint extension_count)
{
  //TODO:JK: implement me
}

guint
ags_midi_ump_util_get_data_message_with_sysex(AgsMidiUmpUtil *midi_ump_util,
					      guchar *buffer,
					      gint *group,
					      gint *status,
					      gchar ***extension_name, GValue **extension_value,
					      guint *extension_count)
{
  //TODO:JK: implement me

  return(0);
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

void
ags_midi_ump_util_put_data_message(AgsMidiUmpUtil *midi_ump_util,
				   guchar *buffer,
				   gint group,
				   gint status,
				   gchar **extension_name, GValue *extension_value,
				   guint extension_count)
{
  //TODO:JK: implement me
}

guint
ags_midi_ump_util_get_data_message(AgsMidiUmpUtil *midi_ump_util,
				   guchar *buffer,
				   gint *group,
				   gint *status,
				   gchar ***extension_name, GValue **extension_value,
				   guint *extension_count)
{
  //TODO:JK: implement me

  return(0);
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

void
ags_midi_ump_util_put_midi2_channel_voice(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gint group,
					  gint status,
					  gint index)
{
  //TODO:JK: implement me
}

guint
ags_midi_ump_util_get_midi2_channel_voice(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gint *group,
					  gint *status,
					  gint *index)
{
  //TODO:JK: implement me

  return(0);
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
 * ags_midi_ump_util_put_stream_message:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @format: the format
 * @status: the status
 * @data: the data
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 *
 * Put stream message.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_stream_message(AgsMidiUmpUtil *midi_ump_util,
				     guchar *buffer,
				     gint format,
				     gint status,
				     guchar data[10],
				     gchar **extension_name, GValue *extension_value,
				     guint extension_count)
{
  guint nth;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0) | ((0x03 & format) << 2) | ((0x300 & status) >> 8);
  nth++;

  buffer[nth] = (0xff & status);
  nth++;

  memcpy(buffer + nth, data, 10 * sizeof(guchar));  
  nth += 10;
}

guint
ags_midi_ump_util_get_stream_message(AgsMidiUmpUtil *midi_ump_util,
				     guchar *buffer,
				     gint *format,
				     gint *status,
				     guchar data[10],
				     gchar ***extension_name, GValue **extension_value,
				     guint *extension_count)
{
  g_return_val_if_fail(midi_ump_util != NULL, 0);
  g_return_val_if_fail(buffer != NULL, 0);

  //TODO:JK: implement me

  return(0);
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
     (((0x0f & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x0){
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

guint
ags_midi_ump_util_get_endpoint_discovery(AgsMidiUmpUtil *midi_ump_util,
					 guchar *buffer,
					 gint *major,
					 gint *minor,
					 gint *filter,
					 gchar ***extension_name, GValue **extension_value,
					 guint *extension_count)
{
  //TODO:JK: implement me

  return(0);
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
     (((0x03 & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x1){
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
  //TODO:JK: implement me

  return(0);
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
     (((0x03 & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x2){
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
 * @defice_family_model: the device family model
 * @software_revision: the software revision
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
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
  buffer[nth] = (0x7f & device_manufacturer);
  nth++;

  buffer[nth] = (0x7f00 & device_manufacturer) >> 8;
  nth++;

  buffer[nth] = (0x7f0000 & device_manufacturer) >> 16;
  nth++;
  
  /* device family */
  buffer[nth] = (0x7f & device_family);
  nth++;

  buffer[nth] = (0x7f00 & device_family) >> 8;
  nth++;

  /* device family model */
  buffer[nth] = (0x7f & device_family_model);
  nth++;

  buffer[nth] = (0x7f00 & device_family_model) >> 8;
  nth++;

  /* software revision */
  buffer[nth] = (0x7f & software_revision);
  nth++;

  buffer[nth] = (0x3f80 & software_revision) >> 7;
  nth++;

  buffer[nth] = (0x1fc000 & software_revision) >> 14;
  nth++;

  buffer[nth] = (0x0fe00000 & software_revision) >> 21;
  nth++;
}

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
  g_return_val_if_fail(midi_ump_util != NULL, 0);
  g_return_val_if_fail(buffer != NULL, 0);

  //TODO:JK: implement me

  return(0);
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
     (((0x03 & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x3){
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
  
  const gint status = 0x3;
  
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

guint
ags_midi_ump_util_get_endpoint_name_notification(AgsMidiUmpUtil *midi_ump_util,
						 guchar *buffer,
						 gchar **endpoint_name,
						 gchar ***extension_name, GValue **extension_value,
						 guint *extension_count)
{
  //TODO:JK: implement me

  return(0);
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
     (((0x03 & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x4){
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
  
  const gint status = 0x4;
  
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

guint
ags_midi_ump_util_get_product_instance_id_notification(AgsMidiUmpUtil *midi_ump_util,
						       guchar *buffer,
						       gchar **product_instance_id,
						       gchar ***extension_name, GValue **extension_value,
						       guint *extension_count)
{
  //TODO:JK: implement me

  return(0);
}
