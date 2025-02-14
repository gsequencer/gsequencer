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

#include <ags/audio/osc/ags_osc_buffer_util.h>

#include <glib/gtypes.h>

#include <ags/audio/osc/ags_osc_util.h>

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>

#include <stdarg.h>

/**
 * SECTION:ags_osc_buffer_util
 * @short_description: OSC buffer util
 * @title: AgsOscBufferUtil
 * @section_id:
 * @include: ags/audio/osc/ags_osc_buffer_util.h
 *
 * Utility functions for OSC buffer.
 */

GType
ags_osc_buffer_util_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_osc_buffer_util = 0;

    ags_type_osc_buffer_util =
      g_boxed_type_register_static("AgsOscBufferUtil",
				   (GBoxedCopyFunc) ags_osc_buffer_util_copy,
				   (GBoxedFreeFunc) ags_osc_buffer_util_free);

    g_once_init_leave(&g_define_type_id__static, ags_type_osc_buffer_util);
  }

  return(g_define_type_id__static);
}


/**
 * ags_osc_buffer_util_alloc:
 *
 * Allocate OSC util.
 *
 * Returns: (transfer full): the newly allocated #AgsOscBufferUtil-struct
 * 
 * Since: 6.0.0
 */
AgsOscBufferUtil*
ags_osc_buffer_util_alloc()
{
  AgsOscBufferUtil *osc_buffer_util;

  osc_buffer_util = g_new0(AgsOscBufferUtil,
			   1);

  osc_buffer_util->major = 1;
  osc_buffer_util->minor = 0;
  
  return(osc_buffer_util);
}

/**
 * ags_osc_buffer_util_free:
 * @osc_buffer_util: the OSC util
 *
 * Free OSC util.
 *
 * Since: 6.0.0
 */
void
ags_osc_buffer_util_free(AgsOscBufferUtil *osc_buffer_util)
{
  g_return_if_fail(osc_buffer_util != NULL);
  
  g_free(osc_buffer_util);
}

/**
 * ags_osc_buffer_util_copy:
 * @osc_buffer_util: the OSC util
 *
 * Copy OSC util.
 *
 * Returns: (transfer full): the newly allocated #AgsOscBufferUtil-struct
 * 
 * Since: 6.0.0
 */
AgsOscBufferUtil*
ags_osc_buffer_util_copy(AgsOscBufferUtil *osc_buffer_util)
{
  AgsOscBufferUtil *ptr;

  g_return_val_if_fail(osc_buffer_util != NULL, NULL);

  ptr = ags_osc_buffer_util_alloc();

  ptr->major = osc_buffer_util->major;
  ptr->minor = osc_buffer_util->minor;
  
  return(ptr);
}

/**
 * ags_osc_buffer_util_put_int32:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @val: the 32 bit integer value
 * 
 * Put @val in @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_put_int32(AgsOscBufferUtil *osc_buffer_util,
			      guchar *buffer,
			      gint32 val)
{  
  if(buffer == NULL){
    return;
  }

  buffer[0] = (val & ((guint32) 0xff << 24)) >> 24;
  buffer[1] = (val & ((guint32) 0xff << 16)) >> 16;
  buffer[2] = (val & ((guint32) 0xff << 8)) >> 8;
  buffer[3] = val & (guint32) 0xff;
}

/**
 * ags_osc_buffer_util_get_int32:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @val: return location of the 32 bit integer value
 * 
 * Get @val from @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_get_int32(AgsOscBufferUtil *osc_buffer_util,
			      guchar *buffer,
			      gint32 *val)
{
  gint32 tmp;
  
  if(buffer == NULL){
    if(val != NULL){
      val[0] = 0;
    }
    
    return;
  }

  tmp = (guint32) (buffer[0] & 0xff);
  tmp = (guint32) (tmp << 8) + (buffer[1] & 0xff);
  tmp = (guint32) (tmp << 8) + (buffer[2] & 0xff);
  tmp = (guint32) (tmp << 8) + (buffer[3] & 0xff);
  
  if(val != NULL){
    val[0] = tmp;
  }
}

/**
 * ags_osc_buffer_util_put_timetag:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @tv_secs: number of seconds since midnight on January 1, 1900
 * @tv_fraction: fraction of seconds to a precision of about 200 picoseconds
 * @immediately: if %TRUE apply immediately, otherwise %FALSE not immediately
 * 
 * Put timetag specified by @tv_secs, @tv_fraction and @immediately to @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_put_timetag(AgsOscBufferUtil *osc_buffer_util,
				guchar *buffer,
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
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @tv_secs: return location of number of seconds since midnight on January 1, 1900
 * @tv_fraction: return location of fraction of seconds to a precision of about 200 picoseconds
 * @immediately: return location of if %TRUE apply immediately, otherwise %FALSE not immediately
 *
 * Get timetag specified by @tv_secs, @tv_fraction and @immediately from @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_get_timetag(AgsOscBufferUtil *osc_buffer_util,
				guchar *buffer,
				gint32 *tv_secs, gint32 *tv_fraction, gboolean *immediately)
{
  gint32 tmp;
  
  if(buffer == NULL){
    if(tv_secs != NULL){
      tv_secs[0] = -1;
    }

    if(tv_fraction != NULL){
      tv_fraction[0] = -1;
    }

    if(immediately != NULL){
      immediately[0] = FALSE;
    }
    
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
  tmp = (tmp << 8) + (buffer[7] & 0xfe);
  
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
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @val: the float value
 * 
 * Put @val to @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_put_float(AgsOscBufferUtil *osc_buffer_util,
			      guchar *buffer,
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

  buffer[0] = (data.val & ((guint32) 0xff << 24)) >> 24;
  buffer[1] = (data.val & ((guint32) 0xff << 16)) >> 16;
  buffer[2] = (data.val & ((guint32) 0xff << 8)) >> 8;
  buffer[3] = data.val & (guint32) 0xff;
}

/**
 * ags_osc_buffer_util_get_float:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @val: the float value
 * 
 * Get @val from @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_get_float(AgsOscBufferUtil *osc_buffer_util,
			      guchar *buffer,
			      gfloat *val)
{
  union{
    guint32 val;
    GFloatIEEE754 ieee_float;
  }data;

  if(buffer == NULL){
    if(val != NULL){
      val[0] = 0.0;
    }
    
    return;
  }

  data.val = (buffer[0] & 0xff);
  data.val = (data.val << 8) + (buffer[1] & 0xff);
  data.val = (data.val << 8) + (buffer[2] & 0xff);
  data.val = (data.val << 8) + (buffer[3] & 0xff);

  if(val != NULL){
    val[0] = data.ieee_float.v_float;
  }
}

/**
 * ags_osc_buffer_util_put_string:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @str: the ASCII string
 * @length: the length to write or -1 to write until null byte
 * 
 * Put @str to @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_put_string(AgsOscBufferUtil *osc_buffer_util,
			       guchar *buffer,
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
	 length * sizeof(guchar));
  buffer[length] = '\0';
}

/**
 * ags_osc_buffer_util_get_string:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @str: the return location of ASCII string
 * @length: the return location of bytes read
 * 
 * Get @str from @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_get_string(AgsOscBufferUtil *osc_buffer_util,
			       guchar *buffer,
			       gchar **str, gsize *length)
{
  gchar *tmp;
  guchar *offset;

  gsize count;
  
  if(buffer == NULL){
    if(str != NULL){
      str[0] = NULL;
    }
    
    if(length != NULL){
      length[0] = 0;
    }
    
    return;
  }

  offset = strchr(buffer, '\0');
  count = offset - buffer;
  
  if(str != NULL){
    if(count > 0){
      tmp = (gchar *) malloc((count + 1) * sizeof(gchar));
      memcpy(tmp, buffer, count + 1);
    }else{
      tmp = NULL;
    }
    
    str[0] = tmp;
  }
  
  if(length != NULL){
    length[0] = count;
  }
}

/**
 * ags_osc_buffer_util_put_blob:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @data_size: the blob's data size
 * @data: the blob's data
 * 
 * Put @data to @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_put_blob(AgsOscBufferUtil *osc_buffer_util,
			     guchar *buffer,
			     gint32 data_size, guchar *data)
{  
  guint padding;
  guint i;
  
  if(buffer == NULL ||
     data_size < 0){
    return;
  }

  ags_osc_buffer_util_put_int32(osc_buffer_util,
				buffer,
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
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @data_size: the return location of blob's data size
 * @data: the return location of blob's data
 * 
 * Get @data from @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_get_blob(AgsOscBufferUtil *osc_buffer_util,
			     guchar *buffer,
			     gint32 *data_size, guchar **data)
{
  guchar *blob;
  
  gint32 tmp;
  
  if(buffer == NULL){
    if(data_size != NULL){
      data_size[0] = 0;
    }
    
    if(data != NULL){
      data[0] = NULL;
    }
    
    return;
  }

  ags_osc_buffer_util_get_int32(osc_buffer_util,
				buffer,
				&tmp);
  
  if(data_size != NULL){
    *data_size = tmp;
  }

  if(data != NULL){
    if(tmp > 0){
      blob = (guchar *) malloc(tmp * sizeof(guchar));
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
 * ags_osc_buffer_util_put_int64:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @val: the 64 bit integer
 * 
 * Put @val to @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_put_int64(AgsOscBufferUtil *osc_buffer_util,
			      guchar *buffer,
			      gint64 val)
{
  if(buffer == NULL){
    return;
  }

  buffer[0] = (val & ((guint64) 0xff << 56)) >> 56;
  buffer[1] = (val & ((guint64) 0xff << 48)) >> 48;
  buffer[2] = (val & ((guint64) 0xff << 40)) >> 40;
  buffer[3] = (val & ((guint64) 0xff << 32)) >> 32;
  buffer[4] = (val & ((guint64) 0xff << 24)) >> 24;
  buffer[5] = (val & ((guint64) 0xff << 16)) >> 16;
  buffer[6] = (val & ((guint64) 0xff << 8)) >> 8;
  buffer[7] = val & (guint64) 0xff;
}

/**
 * ags_osc_buffer_util_get_int64:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @val: the return location of 64 bit integer
 * 
 * Get @val from @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_get_int64(AgsOscBufferUtil *osc_buffer_util,
			      guchar *buffer,
			      gint64 *val)
{
  gint64 tmp;
  
  if(buffer == NULL){
    if(val != NULL){
      val[0] = 0;
    }
    
    return;
  }

  tmp = (guint64) (buffer[0] & 0xff);
  tmp = (guint64) (tmp << 8) + (buffer[1] & 0xff);
  tmp = (guint64) (tmp << 8) + (buffer[2] & 0xff);
  tmp = (guint64) (tmp << 8) + (buffer[3] & 0xff);
  tmp = (guint64) (tmp << 8) + (buffer[4] & 0xff);
  tmp = (guint64) (tmp << 8) + (buffer[5] & 0xff);
  tmp = (guint64) (tmp << 8) + (buffer[6] & 0xff);
  tmp = (guint64) (tmp << 8) + (buffer[7] & 0xff);
  
  if(val != NULL){
    val[0] = tmp;
  }
}

/**
 * ags_osc_buffer_util_put_double:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @val: the double floating point value
 * 
 * Put @val to @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_put_double(AgsOscBufferUtil *osc_buffer_util,
			       guchar *buffer,
			       gdouble val)
{
  union{
    guint64 val;
    GDoubleIEEE754 ieee_double;
  }data;

  if(buffer == NULL){
    return;
  }

  data.ieee_double.v_double = val;

  buffer[0] = (data.val & ((guint64) 0xff << 56)) >> 56;
  buffer[1] = (data.val & ((guint64) 0xff << 48)) >> 48;
  buffer[2] = (data.val & ((guint64) 0xff << 40)) >> 40;
  buffer[3] = (data.val & ((guint64) 0xff << 32)) >> 32;
  buffer[4] = (data.val & ((guint64) 0xff << 24)) >> 24;
  buffer[5] = (data.val & ((guint64) 0xff << 16)) >> 16;
  buffer[6] = (data.val & ((guint64) 0xff << 8)) >> 8;
  buffer[7] = data.val & (guint64) 0xff;
}

/**
 * ags_osc_buffer_util_get_double:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @val: the return location of double floating point value
 * 
 * Get @val from @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_get_double(AgsOscBufferUtil *osc_buffer_util,
			       guchar *buffer,
			       gdouble *val)
{
  union{
    guint64 val;
    GDoubleIEEE754 ieee_double;
  }data;

  if(buffer == NULL){
    if(val != NULL){
      val[0] = 0.0;
    }

    return;
  }

  data.val = (guint64) (buffer[0] & 0xff);
  data.val = (guint64) (data.val << 8) + (buffer[1] & 0xff);
  data.val = (guint64) (data.val << 8) + (buffer[2] & 0xff);
  data.val = (guint64) (data.val << 8) + (buffer[3] & 0xff);
  data.val = (guint64) (data.val << 8) + (buffer[4] & 0xff);
  data.val = (guint64) (data.val << 8) + (buffer[5] & 0xff);
  data.val = (guint64) (data.val << 8) + (buffer[6] & 0xff);
  data.val = (guint64) (data.val << 8) + (buffer[7] & 0xff);

  if(val != NULL){
    val[0] = data.ieee_double.v_double;
  }
}

/**
 * ags_osc_buffer_util_put_char:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @val: the ASCII char
 * 
 * Put @val to @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_put_char(AgsOscBufferUtil *osc_buffer_util,
			     guchar *buffer,
			     gchar val)
{
  if(buffer == NULL){
    return;
  }
  
  buffer[0] = 0x0;
  buffer[1] = 0x0;
  buffer[2] = 0x0;
  buffer[3] = val;
}

/**
 * ags_osc_buffer_util_get_char:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @val: the return location of ASCII char
 * 
 * Get @val from @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_get_char(AgsOscBufferUtil *osc_buffer_util,
			     guchar *buffer,
			     gchar *val)
{
  gint32 tmp;
  
  if(buffer == NULL){
    if(val != NULL){
      val[0] = '\0';
    }
    
    return;
  }

  tmp = (buffer[3] & 0xff);

  if(val != NULL){
    val[0] = tmp;
  }
}

/**
 * ags_osc_buffer_util_put_rgba:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @r: the red value
 * @g: the green value
 * @b: the blue value
 * @a: the alpha value
 * 
 * Put RGBA color specified by @r, @g, @b and @a to @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_put_rgba(AgsOscBufferUtil *osc_buffer_util,
			     guchar *buffer,
			     guint8 r, guint8 g, guint8 b, guint8 a)
{
  if(buffer == NULL){
    return;
  }

  buffer[0] = r;
  buffer[1] = g;
  buffer[2] = b;
  buffer[3] = a;
}

/**
 * ags_osc_buffer_util_get_rgba:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @r: the return location of red value
 * @g: the return location of green value
 * @b: the return location of blue value
 * @a: the return location of alpha value
 * 
 * Get RGBA color specified by @r, @g, @b and @a from @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_get_rgba(AgsOscBufferUtil *osc_buffer_util,
			     guchar *buffer,
			     guint8 *r, guint8 *g, guint8 *b, guint8 *a)
{  
  if(buffer == NULL){
    if(r != NULL){
      r[0] = 0;
    }
    
    if(g != NULL){
      g[0] = 0;
    }
    
    if(b != NULL){
      b[0] = 0;
    }

    if(a != NULL){
      a[0] = 0;
    }

    return;
  }
 
  if(r != NULL){
    *r = buffer[0];
  }

  if(g != NULL){
    *g = buffer[1];
  }

  if(b != NULL){
    *b = buffer[2];
  }

  if(a != NULL){
    *a = buffer[3];
  }
}

/**
 * ags_osc_buffer_util_put_midi:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @port: the port
 * @status_byte: the status byte
 * @data0: the first piece of data
 * @data1: the second piece of data
 * 
 * Put MIDI to @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_put_midi(AgsOscBufferUtil *osc_buffer_util,
			     guchar *buffer,
			     guint8 port, guint8 status_byte, guint8 data0, guint8 data1)
{
  if(buffer == NULL){
    return;
  }

  buffer[0] = port;
  buffer[1] = status_byte;
  buffer[2] = data0;
  buffer[3] = data1;
}

/**
 * ags_osc_buffer_util_get_midi:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @port: the return location of port
 * @status_byte: the return location of status byte
 * @data0: the return location of first piece of data
 * @data1: the return location of second piece of data
 * 
 * Get MIDI from @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_get_midi(AgsOscBufferUtil *osc_buffer_util,
			     guchar *buffer,
			     guint8 *port, guint8 *status_byte, guint8 *data0, guint8 *data1)
{
  if(buffer == NULL){
    if(port != NULL){
      *port = 0x0;
    }

    if(status_byte != NULL){
      *status_byte = 0x0;
    }

    if(data0 != NULL){
      *data0 = 0x0;
    }

    if(data1 != NULL){
      *data1 = 0x0;
    }
    
    return;
  }
 
  if(port != NULL){
    *port = buffer[0];
  }

  if(status_byte != NULL){
    *status_byte = buffer[1];
  }

  if(data0 != NULL){
    *data0 = buffer[2];
  }

  if(data1 != NULL){
    *data1 = buffer[3];
  }
}

/**
 * ags_osc_buffer_util_put_packet:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @packet_size: the packet's size
 * @packet: the packet
 * 
 * Put @packet to @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_put_packet(AgsOscBufferUtil *osc_buffer_util,
			       guchar *buffer,
			       gint32 packet_size, guchar *packet)
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

  ags_osc_buffer_util_put_int32(osc_buffer_util,
				buffer,
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
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @packet_size: the return location of packet's size
 * @packet: the return location of packet
 * 
 * Get @packet from @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_get_packet(AgsOscBufferUtil *osc_buffer_util,
			       guchar *buffer,
			       gint32 *packet_size, guchar **packet)
{
  guchar *data;
  
  gint32 tmp;

  if(buffer == NULL){
    if(packet_size != NULL){
      packet_size[0] = 0;
    }

    if(packet != NULL){
      packet[0] = NULL;
    }
    
    return;
  }

  ags_osc_buffer_util_get_int32(osc_buffer_util,
				buffer,
				&tmp);

  if(packet_size != NULL){
    *packet_size = tmp;
  }

  if(packet != NULL){
    if(tmp > 0){
      data = (guchar *) malloc(tmp * sizeof(guchar));
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
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @packet_size: the packet size or -1 if no packet
 * @...: the packets followed by packet size until it is -1
 * 
 * Put packets to @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_put_packets(AgsOscBufferUtil *osc_buffer_util,
				guchar *buffer,
				gint32 packet_size, ...)
{
  va_list var_args;
  
  guchar *packet;
  
  if(buffer == NULL ||
     packet_size < 0){
    return;
  }

  va_start(var_args, packet_size);

  while(packet_size >= 0){
    packet = va_arg(var_args, guchar *);

    ags_osc_buffer_util_put_packet(osc_buffer_util,
				   buffer,
				   packet_size, packet);

    /* iterate */
    buffer += (4 + packet_size);

    packet_size = va_arg(var_args, gint32);
  }

  va_end(var_args);
}

/**
 * ags_osc_buffer_util_put_message:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @address_pattern: the address pattern
 * @type_tag: the type tag
 * 
 * Put message heading to @buffer. Note you have to put following arguments
 * yourself.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_put_message(AgsOscBufferUtil *osc_buffer_util,
				guchar *buffer,
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
  ags_osc_buffer_util_put_string(osc_buffer_util,
				 buffer,
				 address_pattern, address_pattern_length);

  buffer += (4 * (guint) ceil((double) (address_pattern_length + 1) / 4.0));
  
  type_tag_length = strlen(type_tag);
  ags_osc_buffer_util_put_string(osc_buffer_util,
				 buffer,
				 type_tag, type_tag_length);
}

/**
 * ags_osc_buffer_util_get_message:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @address_pattern: return location of the address pattern
 * @type_tag: return location of the type tag
 * 
 * Get message heading from @buffer. Note you have to get following arguments
 * yourself.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_get_message(AgsOscBufferUtil *osc_buffer_util,
				guchar *buffer,
				gchar **address_pattern, gchar **type_tag)
{
  gchar *str;

  guint offset;
  gsize length;
  
  if(buffer == NULL){
    if(address_pattern != NULL){
      address_pattern[0] = NULL;
    }

    if(type_tag != NULL){
      type_tag[0] = NULL;
    }

    return;
  }

  if(buffer[0] == '/'){
    ags_osc_buffer_util_get_string(osc_buffer_util,
				   buffer,
				   &str, &length);
  }else{
    if(address_pattern != NULL){
      address_pattern[0] = NULL;
    }

    if(type_tag != NULL){
      type_tag[0] = NULL;
    }
    
    return;
  }
  
  if(address_pattern != NULL){
    address_pattern[0] = str;
  }else{
    g_free(str);
  }

  offset = (4 * (guint) ceil((double) (length + 1) / 4.0));

  if(buffer[offset] == ','){
    ags_osc_buffer_util_get_string(osc_buffer_util,
				   buffer + offset,
				   &str, &length);
  }else{
    str = NULL;
  }

  if(type_tag != NULL){
    type_tag[0] = str;
  }else{
    g_free(str);
  }
}

/**
 * ags_osc_buffer_util_put_bundle:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @tv_secs: number of seconds since midnight on January 1, 1900
 * @tv_fraction: fraction of seconds to a precision of about 200 picoseconds
 * @immediately: if %TRUE apply immediately, otherwise %FALSE not immediately
 * 
 * Put bundle to @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_put_bundle(AgsOscBufferUtil *osc_buffer_util,
			       guchar *buffer,
			       gint32 tv_secs, gint32 tv_fraction, gboolean immediately)
{
  if(buffer == NULL){
    return;
  }

  memcpy(buffer,
	 "#bundle",
	 8 * sizeof(guchar));  

  buffer += 8;
  
  ags_osc_buffer_util_put_timetag(osc_buffer_util,
				  buffer,
				  tv_secs, tv_fraction, immediately);
}

/**
 * ags_osc_buffer_util_get_bundle:
 * @osc_buffer_util: the OSC util
 * @buffer: the guchar buffer
 * @tv_secs: return location of number of seconds since midnight on January 1, 1900
 * @tv_fraction: return location of fraction of seconds to a precision of about 200 picoseconds
 * @immediately: return location of if %TRUE apply immediately, otherwise %FALSE not immediately
 *
 * Get bundle from @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_osc_buffer_util_get_bundle(AgsOscBufferUtil *osc_buffer_util,
			       guchar *buffer,
			       gint32 *tv_secs, gint32 *tv_fraction, gboolean *immediately)
{
  gboolean success;
  
  if(buffer == NULL){
    if(tv_secs != NULL){
      *tv_secs = -1;
    }

    if(tv_fraction != NULL){
      *tv_fraction = -1;
    }

    if(immediately != NULL){
      *immediately = FALSE;
    }

    return;
  }
  
  success = (!strncmp(buffer, "#bundle", 8)) ? TRUE: FALSE;
  
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

    return;
  }

  buffer += 8;

  ags_osc_buffer_util_get_timetag(osc_buffer_util,
				  buffer,
				  tv_secs, tv_fraction, immediately);
}
