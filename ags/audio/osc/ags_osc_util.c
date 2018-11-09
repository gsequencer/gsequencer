/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/osc/ags_osc_util.h>

#include <ags/libags.h>

#include <string.h>

/**
 * ags_osc_util_type_tag_string_count_type:
 * @type_tag: the type tag string
 * 
 * Count types in @type_tag.
 * 
 * Returns: the count of types specified
 * 
 * Since: 2.1.0
 */
guint
ags_osc_util_type_tag_string_count_type(gchar *type_tag)
{
  gsize count;

  if(type_tag == NULL ||
     type_tag[0] != ','){
    return(0);
  }
  
  count = strlen(type_tag);

  if(count <= 0){
    return(0);
  }

  count -= 1;
  
  return(count);
}

/**
 * ags_osc_util_meta_data:
 * @uri: the URI
 * @meta_data_length: the return location of meta data's length
 * 
 * Get meta data.
 * 
 * Returns: the byte array containing meta data
 * 
 * Since: 2.1.0
 */
unsigned char*
ags_osc_util_meta_data(gchar *uri,
		       guint *meta_data_length)
{
  unsigned char *meta_data;
  
  meta_data = g_strdup_printf("framing=slip\nversion=1.0 | 1.1\nuri=%s\ntypes=%s\n",
			      uri,
			      AGS_OSC_UTIL_TYPE_TAG_STRING_ALL);

  if(meta_data_length != NULL){
    *meta_data_length = strlen(meta_data);
  }
  
  return(meta_data);
}

/**
 * ags_osc_util_mime_header:
 * @uri: the URI
 * @mime_header_length: the return location of MIME header's length
 * 
 * Get MIME header.
 * 
 * Returns: the byte array containing MIME header
 * 
 * Since: 2.1.0
 */
unsigned char*
ags_osc_util_mime_header(gchar *uri,
			 guint *mime_header_length)
{
  unsigned char *mime_header;
  
  mime_header = g_strdup_printf("MIME-Version: 1.0\nContent-type: application/osc;\nframing=slip\nversion=1.0 | 1.1\nuri=%s\ntypes=%s\n",
				uri,
				AGS_OSC_UTIL_TYPE_TAG_STRING_ALL);

  if(mime_header_length != NULL){
    *mime_header_length = strlen(mime_header);
  }
  
  return(mime_header);
}

/**
 * ags_osc_util_slip_encode:
 * @osc_buffer: the OSC buffer
 * @buffer_length: the OSC buffer's length
 * 
 * Encode @osc_buffer to be suitable for network transmission.
 * See SLIP (RFC1055).
 * 
 * Returns: the encoded byte array
 * 
 * Since: 2.1.0
 */
unsigned char*
ags_osc_util_slip_encode(unsigned char *osc_buffer,
			 guint buffer_length)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_osc_util_slip_decode:
 * @slip_buffer: the SLIP encoded OSC buffer
 * @buffer_length: the buffer length of SLIP encoded OSC buffer
 * 
 * Decode @slip_buffer from SLIP encoded format.
 * See SLIP (RFC1055).
 * 
 * Returns: the OSC buffer as byte array
 * 
 * Since: 2.1.0
 */
unsigned char*
ags_osc_util_slip_decode(unsigned char *slip_buffer,
			 guint buffer_length)
{
  //TODO:JK: implement me

  return(NULL);
}
