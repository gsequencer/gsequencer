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
 * @extension_count: the extension count
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
     (((0x03 & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x03){
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
     (((0x03 & (buffer[0])) << 8) | (0xff & (buffer[1]))) == 0x04){
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

guint
ags_midi_ump_util_get_stream_configuration_notification(AgsMidiUmpUtil *midi_ump_util,
							guchar *buffer,
							gint *protocol,
							gboolean *rx_jitter_reduction, gboolean *tx_jitter_reduction,
							gchar ***extension_name, GValue **extension_value,
							guint *extension_count)
{
  //TODO:JK: implement me

  return(0);
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

guint
ags_midi_ump_util_get_function_block_discovery(AgsMidiUmpUtil *midi_ump_util,
					       guchar *buffer,
					       gint *function_block,
					       gint *filter,
					       gchar ***extension_name, GValue **extension_value,
					       guint *extension_count)
{
  //TODO:JK: implement me

  return(0);
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
 * @function_block: the function block through 0x00 - 0x1f or 0xff to request all
 * @filter: the filter
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
  //TODO:JK: implement me

  return(0);
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
						       gboolean function_block_active,
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

    /* fill in function_block name */
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

guint
ags_midi_ump_util_get_function_block_name_notification(AgsMidiUmpUtil *midi_ump_util,
						       guchar *buffer,
						       gboolean *function_block_active,
						       gint *function_block,
						       gchar **function_block_name,
						       gchar ***extension_name, GValue **extension_value,
						       guint *extension_count)
{
  //TODO:JK: implement me

  return(0);
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
  //TODO:JK: implement me

  return(0);
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
  //TODO:JK: implement me

  return(0);
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
  //TODO:JK: implement me

  return(0);
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
  buffer[nth] = (0xff & (sender_clock_time));
  nth++;
  
  buffer[nth] = (0xff & (sender_clock_time >> 8));
  nth++;
}

guint
ags_midi_ump_util_get_jr_clock(AgsMidiUmpUtil *midi_ump_util,
			       guchar *buffer,
			       guint16 *sender_clock_time,
			       gchar ***extension_name, GValue **extension_value,
			       guint *extension_count)
{
  //TODO:JK: implement me

  return(0);
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
 * ags_midi_ump_util_put_jr_clock:
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
  buffer[nth] = (0xff & (ticks_per_quarter_note_count));
  nth++;
  
  buffer[nth] = (0xff & (ticks_per_quarter_note_count >> 8));
  nth++;
}

guint
ags_midi_ump_util_get_delta_clock_ticks_per_quarter_note(AgsMidiUmpUtil *midi_ump_util,
							 guchar *buffer,
							 guint16 *ticks_per_quarter_note_count,
							 gchar ***extension_name, GValue **extension_value,
							 guint *extension_count)
{
  //TODO:JK: implement me

  return(0);
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

guint
ags_midi_ump_util_get_delta_clock_ticks_since_last_event(AgsMidiUmpUtil *midi_ump_util,
							 guchar *buffer,
							 guint16 *ticks_since_last_event_count,
							 gchar ***extension_name, GValue **extension_value,
							 guint *extension_count)
{
  //TODO:JK: implement me

  return(0);
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
 * ags_midi_ump_util_put_midi1_channel_voice:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @opcode: the opcode
 * @channel: the channel number
 * @index_key: the index key
 * @data: the data
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 1.0 channel voice message.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi1_channel_voice(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gint group,
					  gint opcode,
					  gint channel,
					  gint index_key,
					  gint data,
					  gchar **extension_name, GValue *extension_value,
					  guint extension_count)
{
  guint nth;
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

guint
ags_midi_ump_util_get_midi1_channel_voice(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gint *group,
					  gint *opcode,
					  gint *channel,
					  gint *index_key,
					  gint *data,
					  gchar ***extension_name, GValue **extension_value,
					  guint *extension_count)
{
  //TODO:JK: implement me

  return(0);
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
  const gint opcode = 0x80;
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
  //TODO:JK: implement me

  return(0);
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
  const gint opcode = 0x90;
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
  //TODO:JK: implement me

  return(0);
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
  const gint opcode = 0xa0;
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
  //TODO:JK: implement me

  return(0);
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
  const gint opcode = 0xb0;
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
  //TODO:JK: implement me

  return(0);
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
  const gint opcode = 0xc0;
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
  //TODO:JK: implement me

  return(0);
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
     (0xf0 & (buffer[1])) == 0xc0){
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
  const gint opcode = 0xc0;
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
  //TODO:JK: implement me

  return(0);
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
     (0xf0 & (buffer[1])) == 0xc0){
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
  const gint opcode = 0xc0;
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
  nth++;
}

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

/**
 * ags_midi_ump_util_put_midi2_channel_voice:
 * @midi_ump_util: the MIDI UMP util
 * @buffer: the buffer
 * @group: the group
 * @opcode: the opcode
 * @channel: the channel number
 * @index_key: the index key
 * @data: the data
 * @extension_name: the extension name string vector
 * @extension_value: the extension value array
 * @extension_count: the extension count
 *
 * Put MIDI version 2.0 channel voice message.
 * 
 * Since: 5.5.4
 */
void
ags_midi_ump_util_put_midi2_channel_voice(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gint group,
					  gint opcode,
					  gint channel,
					  gint index_key,
					  gint data,
					  gchar **extension_name, GValue *extension_value,
					  guint extension_count)
{
  guint nth;
  const gint mt = 0x04;
  
  g_return_if_fail(midi_ump_util != NULL);
  g_return_if_fail(buffer != NULL);

  nth = 0;
  
  buffer[nth] = (0xf0 & (mt << 4)) | (0x0f & (group));
  nth++;

  buffer[nth] = (0xf0 & (opcode << 4)) | (0x0f & (channel));
  nth++;
  
  /* index key */
  buffer[nth] = (0xff & (index_key >> 8));
  nth++;

  buffer[nth] = (0xff & (index_key));
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

guint
ags_midi_ump_util_get_midi2_channel_voice(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gint *group,
					  gint *opcode,
					  gint *channel,
					  gint *index_key,
					  gint *data,
					  gchar ***extension_name, GValue **extension_value,
					  guint *extension_count)
{
  //TODO:JK: implement me

  return(0);
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
  const gint opcode = 0x80;
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
  //TODO:JK: implement me

  return(0);
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
  const gint opcode = 0x90;
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
  //TODO:JK: implement me

  return(0);
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
  const gint opcode = 0xa0;
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
  //TODO:JK: implement me

  return(0);
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
  //TODO:JK: implement me

  return(0);
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
     (0xf0 & (buffer[1])) == 0x01){
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
  //TODO:JK: implement me

  return(0);
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
  //TODO:JK: implement me

  return(0);
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

guint
ags_midi_ump_util_get_midi2_control_change(AgsMidiUmpUtil *midi_ump_util,
					   guchar *buffer,
					   gint *group,
					   gint *channel,
					   gint *index_key,
					   gchar ***extension_name, GValue **extension_value,
					   guint *extension_count)
{
  //TODO:JK: implement me

  return(0);
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
     (0xf0 & (buffer[1])) == 0x20){
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
 * @index_key: the index key
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
						 gint bank,
						 gint index_key,
						 gint semitones,
						 gint cents,
						 gchar **extension_name, GValue *extension_value,
						 guint extension_count)
{
  guint nth;
  gint position;
  
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

guint
ags_midi_ump_util_get_midi2_rpn_pitch_bend_range(AgsMidiUmpUtil *midi_ump_util,
						 guchar *buffer,
						 gint *group,
						 gint *channel,
						 gint *index_key,
						 gchar ***extension_name, GValue **extension_value,
						 guint *extension_count)
{
  //TODO:JK: implement me

  return(0);
}
