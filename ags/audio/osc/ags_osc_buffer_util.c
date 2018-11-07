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

#include <ags/audio/osc/ags_osc_buffer_util.h>

#include <ags/libags.h>

#include <ags/audio/osc/ags_osc_util.h>

#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <stdarg.h>

/**
 * ags_osc_buffer_util_put_int32:
 * @buffer: the unsigned char buffer
 * @val: the 32 bit integer value
 * 
 * Put @val in @buffer.
 * 
 * Since: 2.1.0
 */
void
ags_osc_buffer_util_put_int32(unsigned char *buffer,
			      gint32 val)
{  
  if(buffer == NULL){
    return;
  }

  buffer[0] = (val & (0xff << 24)) >> 24;
  buffer[1] = (val & (0xff << 16)) >> 16;
  buffer[2] = (val & (0xff << 8)) >> 8;
  buffer[3] = val & 0xff;
}

/**
 * ags_osc_buffer_util_get_int32:
 * @buffer: the unsigned char buffer
 * @val: return location of the 32 bit integer value
 * 
 * Get @val from @buffer.
 * 
 * Since: 2.1.0
 */
void
ags_osc_buffer_util_get_int32(unsigned char *buffer,
			      gint32 *val)
{
  gint32 tmp;
  
  if(buffer == NULL){
    return;
  }

  tmp = (buffer[0] & 0xff);
  tmp = (tmp << 8) + (buffer[1] & 0xff);
  tmp = (tmp << 8) + (buffer[2] & 0xff);
  tmp = (tmp << 8) + (buffer[3] & 0xff);
  
  if(val != NULL){
    *val = tmp;
  }
}

/**
 * ags_osc_buffer_util_put_timetag:
 * @buffer: the unsigned char buffer
 * @tv_secs: number of seconds since midnight on January 1, 1900
 * @tv_fraction: fraction of seconds to a precision of about 200 picoseconds
 * @immediately: if %TRUE apply immediately, otherwise %FALSE not immediately
 * 
 * Put timetag specified by @tv_secs, @tv_fraction and @immediately to @buffer.
 * 
 * Since: 2.1.0
 */
void
ags_osc_buffer_util_put_timetag(unsigned char *buffer,
				gint32 tv_secs, gint32 tv_fraction, gboolean immediately)
{
  if(buffer == NULL){
    return;
  }

  buffer[0] = (tv_secs & (0xff << 24)) >> 24;
  buffer[1] = (tv_secs & (0xff << 16)) >> 16;
  buffer[2] = (tv_secs & (0xff << 8)) >> 8;
  buffer[3] = tv_secs & 0xff;

  buffer[4] = (tv_fraction & (0xff << 24)) >> 24;
  buffer[5] = (tv_fraction & (0xff << 16)) >> 16;
  buffer[6] = (tv_fraction & (0xff << 8)) >> 8;
  buffer[7] = tv_fraction & 0xfe;

  if(immediately){
    buffer[7] |= 0x1;
  }
}

/**
 * ags_osc_buffer_util_get_timetag:
 * @buffer: the unsigned char buffer
 * @tv_secs: return location of number of seconds since midnight on January 1, 1900
 * @tv_fraction: return location of fraction of seconds to a precision of about 200 picoseconds
 * @immediately: return location of if %TRUE apply immediately, otherwise %FALSE not immediately
 *
 * Get timetag specified by @tv_secs, @tv_fraction and @immediately from @buffer.
 * 
 * Since: 2.1.0
 */
void
ags_osc_buffer_util_get_timetag(unsigned char *buffer,
				gint32 *tv_secs, gint32 *tv_fraction, gboolean *immediately)
{
  gint32 tmp;
  
  if(buffer == NULL){
    return;
  }

  tmp = (buffer[0] & 0xff);
  tmp = (tmp << 8) + (buffer[1] & 0xff);
  tmp = (tmp << 8) + (buffer[2] & 0xff);
  tmp = (tmp << 8) + (buffer[3] & 0xff);
  
  if(tv_secs != NULL){
    *tv_secs = tmp;
  }

  tmp = (buffer[4] & 0xff);
  tmp = (tmp << 8) + (buffer[5] & 0xff);
  tmp = (tmp << 8) + (buffer[6] & 0xff);
  tmp = (tmp << 8) + (buffer[7] & 0xff);
  
  if(tv_fraction != NULL){
    *tv_fraction = tmp;
  }

  if(immediately != NULL){
    if((0x1 & buffer[7]) != 0){
      *immediately = TRUE;
    }else{
      *immediately = FALSE;
    }
  }
}

/**
 * ags_osc_buffer_util_put_float:
 * @buffer: the unsigned char buffer
 * @val: the float value
 * 
 * Put @val to @buffer.
 * 
 * Since: 2.1.0
 */
void
ags_osc_buffer_util_put_float(unsigned char *buffer,
			      gfloat val)
{
  union{
    guint32 val;
    GFloatIEEE754 ieee_float;
  }data;

  if(buffer == NULL){
    return;
  }

  data.ieee_float.v_float = val;

  buffer[0] = (data.val & (0xff << 24)) >> 24;
  buffer[1] = (data.val & (0xff << 16)) >> 16;
  buffer[2] = (data.val & (0xff << 8)) >> 8;
  buffer[3] = data.val & 0xff;
}

/**
 * ags_osc_buffer_util_get_float:
 * @buffer: the unsigned char buffer
 * @val: the float value
 * 
 * Get @val from @buffer.
 * 
 * Since: 2.1.0
 */
void
ags_osc_buffer_util_get_float(unsigned char *buffer,
			      gfloat *val)
{
  union{
    guint32 val;
    GFloatIEEE754 ieee_float;
  }data;

  if(buffer == NULL){
    return;
  }

  data.val = (buffer[0] & 0xff);
  data.val = (data.val << 8) + (buffer[1] & 0xff);
  data.val = (data.val << 8) + (buffer[2] & 0xff);
  data.val = (data.val << 8) + (buffer[3] & 0xff);

  if(val != NULL){
    *val = data.ieee_float.v_float;
  }
}

/**
 * ags_osc_buffer_util_put_string:
 * @buffer: the unsigned char buffer
 * @str: the ASCII string
 * @length: the length to write or -1 to write until null byte
 * 
 * Put @str to @buffer.
 * 
 * Since: 2.1.0
 */
void
ags_osc_buffer_util_put_string(unsigned char *buffer,
			       gchar *str, gsize length)
{
  if(buffer == NULL ||
     length == 0){
    return;
  }

  if(length == -1){
    length = strlen(str);
  }
  
  memcpy(buffer,
	 str,
	 length * sizeof(unsigned char));
  buffer[length] = '\0';
}

/**
 * ags_osc_buffer_util_get_string:
 * @buffer: the unsigned char buffer
 * @str: the return location of ASCII string
 * @length: the return location of bytes read
 * 
 * Get @str from @buffer.
 * 
 * Since: 2.1.0
 */
void
ags_osc_buffer_util_get_string(unsigned char *buffer,
			       gchar **str, gsize *length)
{
  gchar *tmp;
  unsigned char *offset;

  gint64 count;
  
  if(buffer == NULL){
    return;
  }

  offset = index(buffer, '\0');
  count = offset - buffer;
  
  if(str != NULL){
    if(count > 0){
      tmp = (gchar *) malloc((count + 1) * sizeof(gchar));
      memcpy(tmp, buffer, count + 1);
    }else{
      tmp = NULL;
    }
    
    *str = tmp;
  }
  
  if(length != NULL){
    *length = count;
  }
}

/**
 * ags_osc_buffer_util_put_blob:
 * @buffer: the unsigned char buffer
 * @data_size: the blob's data size
 * @data: the blob's data
 * 
 * Put @data to @buffer.
 * 
 * Since: 2.1.0
 */
void
ags_osc_buffer_util_put_blob(unsigned char *buffer,
			     gint32 data_size, unsigned char *data)
{  
  guint padding;
  guint i;
  
  if(buffer == NULL ||
     data_size < 0){
    return;
  }

  ags_osc_buffer_util_put_int32(buffer,
				data_size);

  memcpy(buffer + 4,
	 data,
	 data_size);

  if(data_size % 4 != 0){
    padding = 4 - (data_size % 4);
  }else{
    padding = 0;
  }

  for(i = 0; i < padding; i++){
    buffer[4 + data_size + i] = 0x0;
  }
}

/**
 * ags_osc_buffer_util_get_blob:
 * @buffer: the unsigned char buffer
 * @data_size: the return location of blob's data size
 * @data: the return location of blob's data
 * 
 * Get @data from @buffer.
 * 
 * Since: 2.1.0
 */
void
ags_osc_buffer_util_get_blob(unsigned char *buffer,
			     gint32 *data_size, unsigned char **data)
{
  unsigned char *blob;
  
  gint32 tmp;
  
  if(buffer == NULL){
    return;
  }

  ags_osc_buffer_util_get_int32(buffer,
				&tmp);
  
  if(data_size != NULL){
    *data_size = tmp;
  }

  if(data != NULL){
    if(tmp > 0){
      blob = (unsigned char *) malloc(tmp * sizeof(unsigned char));
      memcpy(blob,
	     buffer + 4,
	     tmp);
    
      *data = blob;
    }else{
      *data = NULL;
    }
  }
}

/**
 * ags_osc_buffer_util_put_packet:
 * @buffer: the unsigned char buffer
 * @packet_size: the packet's size
 * @packet: the packet
 * 
 * Put @packet to @buffer.
 * 
 * Since: 2.1.0
 */
void
ags_osc_buffer_util_put_packet(unsigned char *buffer,
			       gint32 packet_size, unsigned char *packet)
{
  guint padding;
  guint i;
  
  if(buffer == NULL ||
     packet_size < 0){
    return;
  }

  if(packet_size % 4 != 0){
    padding = 4 - (packet_size % 4);
  }else{
    padding = 0;
  }

  ags_osc_buffer_util_put_int32(buffer,
				packet_size + padding);
  
  memcpy(buffer + 4,
	 packet,
	 packet_size);

  for(i = 0; i < padding; i++){
    buffer[4 + packet_size + i] = 0x0;
  }
}

/**
 * ags_osc_buffer_util_get_packet:
 * @buffer: the unsigned char buffer
 * @packet_size: the return location of packet's size
 * @packet: the return location of packet
 * 
 * Get @packet from @buffer.
 * 
 * Since: 2.1.0
 */
void
ags_osc_buffer_util_get_packet(unsigned char *buffer,
			       gint32 *packet_size, unsigned char **packet)
{
  unsigned char *data;
  
  gint32 tmp;

  if(buffer == NULL){
    return;
  }

  if(packet_size != NULL){
    *packet_size = tmp;
  }

  if(packet != NULL){
    if(tmp > 0){
      data = (unsigned char *) malloc(tmp * sizeof(unsigned char));
      memcpy(data,
	     buffer + 4,
	     tmp);
    
      *packet = data;
    }else{
      *packet = NULL;
    }
  }
}

/**
 * ags_osc_buffer_util_put_packets:
 * @buffer: the unsigned char buffer
 * @packet_size: the packet size or -1 if no packet
 * @...: the packets followed by packet size until it is -1
 * 
 * Put packets to @buffer.
 * 
 * Since: 2.1.0
 */
void
ags_osc_buffer_util_put_packets(unsigned char *buffer,
				gint32 packet_size, ...)
{
  va_list var_args;
  
  unsigned char *packet;
  
  if(buffer == NULL ||
     packet_size < 0){
    return;
  }

  va_start(var_args, packet_size);

  while(packet_size >= 0){
    packet = va_arg(var_args, unsigned char *);

    ags_osc_buffer_util_put_packet(buffer,
				   packet_size, packet);

    /* iterate */
    buffer += (4 + packet_size);

    packet_size = va_arg(var_args, gint32);
  }

  va_end(var_args);
}

/**
 * ags_osc_buffer_util_put_message:
 * @buffer: the unsigned char buffer
 * @address_pattern: the address pattern
 * @type_tag: the type tag
 * 
 * Put message heading to @buffer. Note you have to put following arguments
 * yourself.
 * 
 * Since: 2.1.0
 */
void
ags_osc_buffer_util_put_message(unsigned char *buffer,
				gchar *address_pattern, gchar *type_tag)
{
  gsize address_pattern_length;
  gsize type_tag_length;
  
  if(buffer == NULL ||
     address_pattern[0] != '/' ||
     type_tag[0] != ','){
    return;
  }

  address_pattern_length = strlen(address_pattern);
  ags_osc_buffer_util_put_string(buffer,
				 address_pattern, address_pattern_length);

  buffer += (address_pattern_length + 1);
  
  type_tag_length = strlen(type_tag);
  ags_osc_buffer_util_put_string(buffer,
				 type_tag, type_tag_length);

  buffer += (type_tag_length + 1);
}

/**
 * ags_osc_buffer_util_get_message:
 * @buffer: the unsigned char buffer
 * @address_pattern: return location of the address pattern
 * @type_tag: return location of the type tag
 * 
 * Get message heading from @buffer. Note you have to get following arguments
 * yourself.
 * 
 * Since: 2.1.0
 */
void
ags_osc_buffer_util_get_message(unsigned char *buffer,
				gchar **address_pattern, gchar **type_tag)
{
  gchar *str;

  gsize length;
  
  if(buffer == NULL){
    return;
  }

  if(buffer[0] == '/'){
    ags_osc_buffer_util_get_string(buffer,
				   &str, &length);
  }else{
    if(address_pattern != NULL){
      *address_pattern = NULL;
    }

    if(type_tag != NULL){
      *type_tag = NULL;
    }
    
    return;
  }
  
  if(address_pattern != NULL){
    *address_pattern = str;
  }else{
    g_free(str);
  }

  buffer += (length + 1);

  if(buffer[0] == ','){
    ags_osc_buffer_util_get_string(buffer,
				   &str, &length);
  }else{
    str = NULL;
  }

  if(type_tag != NULL){
    *type_tag = str;
  }else{
    g_free(str);
  }
}

/**
 * ags_osc_buffer_util_put_bundle:
 * @buffer: the unsigned char buffer
 * @tv_secs: number of seconds since midnight on January 1, 1900
 * @tv_fraction: fraction of seconds to a precision of about 200 picoseconds
 * @immediately: if %TRUE apply immediately, otherwise %FALSE not immediately
 * 
 * Put bundle to @buffer.
 * 
 * Since: 2.1.0
 */
void
ags_osc_buffer_util_put_bundle(unsigned char *buffer,
			       gint32 tv_secs, gint32 tv_fraction, gboolean immediately)
{
  if(buffer == NULL){
    return;
  }

  memcpy(buffer,
	 "#bundle",
	 7 * sizeof(unsigned char));  

  buffer += 7;
  
  ags_osc_buffer_util_put_timetag(buffer,
				  tv_secs, tv_fraction, immediately);
}

/**
 * ags_osc_buffer_util_get_bundle:
 * @buffer: the unsigned char buffer
 * @tv_secs: return location of number of seconds since midnight on January 1, 1900
 * @tv_fraction: return location of fraction of seconds to a precision of about 200 picoseconds
 * @immediately: return location of if %TRUE apply immediately, otherwise %FALSE not immediately
 *
 * Get bundle from @buffer.
 * 
 * Since: 2.1.0
 */
void
ags_osc_buffer_util_get_bundle(unsigned char *buffer,
			       gint32 *tv_secs, gint32 *tv_fraction, gboolean *immediately)
{
  gboolean success;
  
  if(buffer == NULL){
    return;
  }
  
  success = (!strncmp(buffer, "#bundle", 7)) ? TRUE: FALSE;
  
  if(!success){
    if(tv_secs != NULL){
      *tv_secs = -1;
    }

    if(tv_fraction != NULL){
      *tv_fraction = -1;
    }

    if(immediately != NULL){
      *immediately = FALSE;
    }
  }

  buffer += 7;

  ags_osc_buffer_util_get_timetag(buffer,
				  tv_secs, tv_fraction, immediately);
}
