/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/lib/ags_buffer_util.h>

#include <ags/lib/ags_endian.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

gpointer ags_buffer_util_copy(gpointer ptr);
void ags_buffer_util_free(gpointer ptr);

/**
 * SECTION:ags_buffer_util
 * @short_description: buffer util
 * @title: AgsBufferUtil
 * @section_id:
 * @include: ags/lib/ags_buffer_util.h
 *
 * Common utility functions related to char buffers.
 */

GType
ags_buffer_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_buffer_util = 0;

    ags_type_buffer_util =
      g_boxed_type_register_static("AgsBufferUtil",
				   (GBoxedCopyFunc) ags_buffer_util_copy,
				   (GBoxedFreeFunc) ags_buffer_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_buffer_util);
  }

  return g_define_type_id__volatile;
}

gpointer
ags_buffer_util_copy(gpointer ptr)
{
  gpointer retval;

  retval = g_memdup(ptr, sizeof(AgsBufferUtil));
 
  return(retval);
}

void
ags_buffer_util_free(gpointer ptr)
{
  g_free(ptr);
}

/**
 * ags_buffer_util_s8_to_char_buffer:
 * @buffer: the gint8 buffer
 * @buffer_length: the buffer length
 * 
 * Pack @buffer into an guchar buffer.
 * 
 * Returns: (transfer full): the guchar buffer
 * 
 * Since: 3.0.0
 */
guchar*
ags_buffer_util_s8_to_char_buffer(gint8 *buffer,
				  guint buffer_length)
{
  guchar *cbuffer, *start_cbuffer;

  guint limit;
  guint i;

  if(buffer == NULL){
    return(NULL);
  }
  
  start_cbuffer = 
    cbuffer = (guchar *) malloc(buffer_length * sizeof(guchar));

  i = 0;
  
  limit = buffer_length - (buffer_length % 8);

  for(; i < limit; i += 8){
    cbuffer[0] = (guchar) (0xff & buffer[0]);
    cbuffer[1] = (guchar) (0xff & buffer[1]);
    cbuffer[2] = (guchar) (0xff & buffer[2]);
    cbuffer[3] = (guchar) (0xff & buffer[3]);
    cbuffer[4] = (guchar) (0xff & buffer[4]);
    cbuffer[5] = (guchar) (0xff & buffer[5]);
    cbuffer[6] = (guchar) (0xff & buffer[6]);
    cbuffer[7] = (guchar) (0xff & buffer[7]);

    buffer += 8;
    cbuffer += 8;
  }

  for(; i < buffer_length; i++){
    cbuffer[0] = (guchar) (0xff & buffer[0]);

    buffer++;
    cbuffer++;
  }

  return(start_cbuffer);
}

/**
 * ags_buffer_util_s16_to_char_buffer:
 * @buffer: the gint16 buffer
 * @buffer_length: the buffer length
 * 
 * Pack @buffer into an guchar buffer.
 * 
 * Returns: (transfer full): the guchar buffer
 * 
 * Since: 3.0.0
 */
guchar*
ags_buffer_util_s16_to_char_buffer(gint16 *buffer,
				   guint buffer_length)
{
  guchar *cbuffer, *start_cbuffer;

  guint limit;
  guint i;
  
  if(buffer == NULL){
    return(NULL);
  }
  
  start_cbuffer = 
    cbuffer = (guchar *) malloc((2 * buffer_length) * sizeof(guchar));

  i = 0;

  limit = buffer_length - (buffer_length % 8);

  for(; i < limit; i += 8){
    cbuffer[0] = (guchar) ((0xff00 & buffer[0]) >> 8);
    cbuffer[1] = (guchar) (0xff & buffer[0]);

    cbuffer[2] = (guchar) ((0xff00 & buffer[1]) >> 8);
    cbuffer[3] = (guchar) (0xff & buffer[1]);

    cbuffer[4] = (guchar) ((0xff00 & buffer[2]) >> 8);
    cbuffer[5] = (guchar) (0xff & buffer[2]);

    cbuffer[6] = (guchar) ((0xff00 & buffer[3]) >> 8);
    cbuffer[7] = (guchar) (0xff & buffer[3]);

    cbuffer[8] = (guchar) ((0xff00 & buffer[4]) >> 8);
    cbuffer[9] = (guchar) (0xff & buffer[4]);
      
    cbuffer[10] = (guchar) ((0xff00 & buffer[5]) >> 8);
    cbuffer[11] = (guchar) (0xff & buffer[5]);

    cbuffer[12] = (guchar) ((0xff00 & buffer[6]) >> 8);
    cbuffer[13] = (guchar) (0xff & buffer[6]);

    cbuffer[14] = (guchar) ((0xff00 & buffer[7]) >> 8);
    cbuffer[15] = (guchar) (0xff & buffer[7]);
      
    buffer += 8;
    cbuffer += (2 * 8);
  }
  
  for(; i < buffer_length; i++){
    cbuffer[0] = (guchar) ((0xff00 & buffer[0]) >> 8);
    cbuffer[1] = (guchar) (0xff & buffer[0]);

    buffer++;
    cbuffer += 2;
  }

  return(start_cbuffer);
}

/**
 * ags_buffer_util_s24_to_char_buffer:
 * @buffer: the gint32 buffer
 * @buffer_length: the buffer length
 * 
 * Pack @buffer into an guchar buffer.
 * 
 * Returns: (transfer full): the guchar buffer
 * 
 * Since: 3.0.0
 */
guchar*
ags_buffer_util_s24_to_char_buffer(gint32 *buffer,
				   guint buffer_length)
{
  guchar *cbuffer, *start_cbuffer;

  guint limit;
  guint i;

  if(buffer == NULL){
    return(NULL);
  }
  
  start_cbuffer = 
    cbuffer = (guchar *) malloc((4 * buffer_length) * sizeof(guchar));

  i = 0;
  
  limit = buffer_length - (buffer_length % 8);

  for(; i < limit; i += 8){
    cbuffer[0] = (guchar) ((0xff000000 & buffer[0]) >> 24);
    cbuffer[1] = (guchar) ((0xff0000 & buffer[0]) >> 16);
    cbuffer[2] = (guchar) ((0xff00 & buffer[0]) >> 8);
    cbuffer[3] = (guchar) (0xff & buffer[0]);
      
    cbuffer[4] = (guchar) ((0xff000000 & buffer[1]) >> 24);
    cbuffer[5] = (guchar) ((0xff0000 & buffer[1]) >> 16);
    cbuffer[6] = (guchar) ((0xff00 & buffer[1]) >> 8);
    cbuffer[7] = (guchar) (0xff & buffer[1]);

    cbuffer[8] = (guchar) ((0xff000000 & buffer[2]) >> 24);
    cbuffer[9] = (guchar) ((0xff0000 & buffer[2]) >> 16);
    cbuffer[10] = (guchar) ((0xff00 & buffer[2]) >> 8);
    cbuffer[11] = (guchar) (0xff & buffer[2]);

    cbuffer[12] = (guchar) ((0xff000000 & buffer[3]) >> 24);
    cbuffer[13] = (guchar) ((0xff0000 & buffer[3]) >> 16);
    cbuffer[14] = (guchar) ((0xff00 & buffer[3]) >> 8);
    cbuffer[15] = (guchar) (0xff & buffer[3]);

    cbuffer[16] = (guchar) ((0xff000000 & buffer[4]) >> 24);
    cbuffer[17] = (guchar) ((0xff0000 & buffer[4]) >> 16);
    cbuffer[18] = (guchar) ((0xff00 & buffer[4]) >> 8);
    cbuffer[19] = (guchar) (0xff & buffer[4]);

    cbuffer[20] = (guchar) ((0xff000000 & buffer[5]) >> 24);
    cbuffer[21] = (guchar) ((0xff0000 & buffer[5]) >> 16);
    cbuffer[22] = (guchar) ((0xff00 & buffer[5]) >> 8);
    cbuffer[23] = (guchar) (0xff & buffer[5]);

    cbuffer[24] = (guchar) ((0xff000000 & buffer[6]) >> 24);
    cbuffer[25] = (guchar) ((0xff0000 & buffer[6]) >> 16);
    cbuffer[26] = (guchar) ((0xff00 & buffer[6]) >> 8);
    cbuffer[27] = (guchar) (0xff & buffer[6]);

    cbuffer[28] = (guchar) ((0xff000000 & buffer[7]) >> 24);
    cbuffer[29] = (guchar) ((0xff0000 & buffer[7]) >> 16);
    cbuffer[30] = (guchar) ((0xff00 & buffer[7]) >> 8);
    cbuffer[31] = (guchar) (0xff & buffer[7]);

    buffer += 8;
    cbuffer += (4 * 8);
  }

  for(; i < buffer_length; i++){
    cbuffer[0] = (guchar) ((0xff000000 & buffer[0]) >> 24);
    cbuffer[1] = (guchar) ((0xff0000 & buffer[0]) >> 16);
    cbuffer[2] = (guchar) ((0xff00 & buffer[0]) >> 8);
    cbuffer[3] = (guchar) (0xff & buffer[0]);

    buffer++;
    cbuffer += 4;
  }

  return(start_cbuffer);
}

/**
 * ags_buffer_util_s32_to_char_buffer:
 * @buffer: the gint32 buffer
 * @buffer_length: the buffer length
 * 
 * Pack @buffer into an guchar buffer.
 * 
 * Returns: (transfer full): the guchar buffer
 * 
 * Since: 3.0.0
 */
guchar*
ags_buffer_util_s32_to_char_buffer(gint32 *buffer,
				   guint buffer_length)
{
  guchar *cbuffer, *start_cbuffer;

  guint limit;
  guint i;

  if(buffer == NULL){
    return(NULL);
  }
  
  start_cbuffer = 
    cbuffer = (guchar *) malloc((4 * buffer_length) * sizeof(guchar));

  i = 0;
  
  limit = buffer_length - (buffer_length % 8);

  for(; i < limit; i += 8){
    cbuffer[0] = (guchar) ((0xff000000 & buffer[0]) >> 24);
    cbuffer[1] = (guchar) ((0xff0000 & buffer[0]) >> 16);
    cbuffer[2] = (guchar) ((0xff00 & buffer[0]) >> 8);
    cbuffer[3] = (guchar) (0xff & buffer[0]);
      
    cbuffer[4] = (guchar) ((0xff000000 & buffer[1]) >> 24);
    cbuffer[5] = (guchar) ((0xff0000 & buffer[1]) >> 16);
    cbuffer[6] = (guchar) ((0xff00 & buffer[1]) >> 8);
    cbuffer[7] = (guchar) (0xff & buffer[1]);

    cbuffer[8] = (guchar) ((0xff000000 & buffer[2]) >> 24);
    cbuffer[9] = (guchar) ((0xff0000 & buffer[2]) >> 16);
    cbuffer[10] = (guchar) ((0xff00 & buffer[2]) >> 8);
    cbuffer[11] = (guchar) (0xff & buffer[2]);

    cbuffer[12] = (guchar) ((0xff000000 & buffer[3]) >> 24);
    cbuffer[13] = (guchar) ((0xff0000 & buffer[3]) >> 16);
    cbuffer[14] = (guchar) ((0xff00 & buffer[3]) >> 8);
    cbuffer[15] = (guchar) (0xff & buffer[3]);

    cbuffer[16] = (guchar) ((0xff000000 & buffer[4]) >> 24);
    cbuffer[17] = (guchar) ((0xff0000 & buffer[4]) >> 16);
    cbuffer[18] = (guchar) ((0xff00 & buffer[4]) >> 8);
    cbuffer[19] = (guchar) (0xff & buffer[4]);

    cbuffer[20] = (guchar) ((0xff000000 & buffer[5]) >> 24);
    cbuffer[21] = (guchar) ((0xff0000 & buffer[5]) >> 16);
    cbuffer[22] = (guchar) ((0xff00 & buffer[5]) >> 8);
    cbuffer[23] = (guchar) (0xff & buffer[5]);

    cbuffer[24] = (guchar) ((0xff000000 & buffer[6]) >> 24);
    cbuffer[25] = (guchar) ((0xff0000 & buffer[6]) >> 16);
    cbuffer[26] = (guchar) ((0xff00 & buffer[6]) >> 8);
    cbuffer[27] = (guchar) (0xff & buffer[6]);

    cbuffer[28] = (guchar) ((0xff000000 & buffer[7]) >> 24);
    cbuffer[29] = (guchar) ((0xff0000 & buffer[7]) >> 16);
    cbuffer[30] = (guchar) ((0xff00 & buffer[7]) >> 8);
    cbuffer[31] = (guchar) (0xff & buffer[7]);
      
    buffer += 8;
    cbuffer += (4 * 8);
  }

  for(; i < buffer_length; i++){
    cbuffer[0] = (guchar) ((0xff000000 & buffer[0]) >> 24);
    cbuffer[1] = (guchar) ((0xff0000 & buffer[0]) >> 16);
    cbuffer[2] = (guchar) ((0xff00 & buffer[0]) >> 8);
    cbuffer[3] = (guchar) (0xff & buffer[0]);

    buffer++;
    cbuffer += 4;
  }

  return(start_cbuffer);
}

/**
 * ags_buffer_util_s64_to_char_buffer:
 * @buffer: the gint64 buffer
 * @buffer_length: the buffer length
 * 
 * Pack @buffer into an guchar buffer.
 * 
 * Returns: (transfer full): the guchar buffer
 * 
 * Since: 3.0.0
 */
guchar*
ags_buffer_util_s64_to_char_buffer(gint64 *buffer,
				   guint buffer_length)
{
  guchar *cbuffer, *start_cbuffer;

  guint limit;
  guint i;

  if(buffer == NULL){
    return(NULL);
  }
  
  start_cbuffer = 
    cbuffer = (guchar *) malloc((8 * buffer_length) * sizeof(guchar));

  i = 0;
  
  limit = buffer_length - (buffer_length % 8);

  for(; i < limit; i += 8){
    cbuffer[0] = (guchar) ((0xff00000000000000 & buffer[0]) >> 56);
    cbuffer[1] = (guchar) ((0xff000000000000 & buffer[0]) >> 48);
    cbuffer[2] = (guchar) ((0xff0000000000 & buffer[0]) >> 40);
    cbuffer[3] = (guchar) ((0xff00000000 & buffer[0]) >> 32);
    cbuffer[4] = (guchar) ((0xff000000 & buffer[0]) >> 24);
    cbuffer[5] = (guchar) ((0xff0000 & buffer[0]) >> 16);
    cbuffer[6] = (guchar) ((0xff00 & buffer[0]) >> 8);
    cbuffer[7] = (guchar) (0xff & buffer[0]);

    cbuffer[8] = (guchar) ((0xff00000000000000 & buffer[1]) >> 56);
    cbuffer[9] = (guchar) ((0xff000000000000 & buffer[1]) >> 48);
    cbuffer[10] = (guchar) ((0xff0000000000 & buffer[1]) >> 40);
    cbuffer[11] = (guchar) ((0xff00000000 & buffer[1]) >> 32);
    cbuffer[12] = (guchar) ((0xff000000 & buffer[1]) >> 24);
    cbuffer[13] = (guchar) ((0xff0000 & buffer[1]) >> 16);
    cbuffer[14] = (guchar) ((0xff00 & buffer[1]) >> 8);
    cbuffer[15] = (guchar) (0xff & buffer[1]);

    cbuffer[16] = (guchar) ((0xff00000000000000 & buffer[2]) >> 56);
    cbuffer[17] = (guchar) ((0xff000000000000 & buffer[2]) >> 48);
    cbuffer[18] = (guchar) ((0xff0000000000 & buffer[2]) >> 40);
    cbuffer[19] = (guchar) ((0xff00000000 & buffer[2]) >> 32);
    cbuffer[20] = (guchar) ((0xff000000 & buffer[2]) >> 24);
    cbuffer[21] = (guchar) ((0xff0000 & buffer[2]) >> 16);
    cbuffer[22] = (guchar) ((0xff00 & buffer[2]) >> 8);
    cbuffer[23] = (guchar) (0xff & buffer[2]);

    cbuffer[24] = (guchar) ((0xff00000000000000 & buffer[3]) >> 56);
    cbuffer[25] = (guchar) ((0xff000000000000 & buffer[3]) >> 48);
    cbuffer[26] = (guchar) ((0xff0000000000 & buffer[3]) >> 40);
    cbuffer[27] = (guchar) ((0xff00000000 & buffer[3]) >> 32);
    cbuffer[28] = (guchar) ((0xff000000 & buffer[3]) >> 24);
    cbuffer[29] = (guchar) ((0xff0000 & buffer[3]) >> 16);
    cbuffer[30] = (guchar) ((0xff00 & buffer[3]) >> 8);
    cbuffer[31] = (guchar) (0xff & buffer[3]);

    cbuffer[32] = (guchar) ((0xff00000000000000 & buffer[4]) >> 56);
    cbuffer[33] = (guchar) ((0xff000000000000 & buffer[4]) >> 48);
    cbuffer[34] = (guchar) ((0xff0000000000 & buffer[4]) >> 40);
    cbuffer[35] = (guchar) ((0xff00000000 & buffer[4]) >> 32);
    cbuffer[36] = (guchar) ((0xff000000 & buffer[4]) >> 24);
    cbuffer[37] = (guchar) ((0xff0000 & buffer[4]) >> 16);
    cbuffer[38] = (guchar) ((0xff00 & buffer[4]) >> 8);
    cbuffer[39] = (guchar) (0xff & buffer[4]);
      
    cbuffer[40] = (guchar) ((0xff00000000000000 & buffer[5]) >> 56);
    cbuffer[41] = (guchar) ((0xff000000000000 & buffer[5]) >> 48);
    cbuffer[42] = (guchar) ((0xff0000000000 & buffer[5]) >> 40);
    cbuffer[43] = (guchar) ((0xff00000000 & buffer[5]) >> 32);
    cbuffer[44] = (guchar) ((0xff000000 & buffer[5]) >> 24);
    cbuffer[45] = (guchar) ((0xff0000 & buffer[5]) >> 16);
    cbuffer[46] = (guchar) ((0xff00 & buffer[5]) >> 8);
    cbuffer[47] = (guchar) (0xff & buffer[5]);

    cbuffer[48] = (guchar) ((0xff00000000000000 & buffer[6]) >> 56);
    cbuffer[49] = (guchar) ((0xff000000000000 & buffer[6]) >> 48);
    cbuffer[50] = (guchar) ((0xff0000000000 & buffer[6]) >> 40);
    cbuffer[51] = (guchar) ((0xff00000000 & buffer[6]) >> 32);
    cbuffer[52] = (guchar) ((0xff000000 & buffer[6]) >> 24);
    cbuffer[53] = (guchar) ((0xff0000 & buffer[6]) >> 16);
    cbuffer[54] = (guchar) ((0xff00 & buffer[6]) >> 8);
    cbuffer[55] = (guchar) (0xff & buffer[6]);

    cbuffer[56] = (guchar) ((0xff00000000000000 & buffer[7]) >> 56);
    cbuffer[57] = (guchar) ((0xff000000000000 & buffer[7]) >> 48);
    cbuffer[58] = (guchar) ((0xff0000000000 & buffer[7]) >> 40);
    cbuffer[59] = (guchar) ((0xff00000000 & buffer[7]) >> 32);
    cbuffer[60] = (guchar) ((0xff000000 & buffer[7]) >> 24);
    cbuffer[61] = (guchar) ((0xff0000 & buffer[7]) >> 16);
    cbuffer[62] = (guchar) ((0xff00 & buffer[7]) >> 8);
    cbuffer[63] = (guchar) (0xff & buffer[7]);

    buffer += 8;
    cbuffer += (8 * 8);
  }

  for(; i < buffer_length; i++){
    cbuffer[0] = (guchar) ((0xff00000000000000 & buffer[0]) >> 56);
    cbuffer[1] = (guchar) ((0xff000000000000 & buffer[0]) >> 48);
    cbuffer[2] = (guchar) ((0xff0000000000 & buffer[0]) >> 40);
    cbuffer[3] = (guchar) ((0xff00000000 & buffer[0]) >> 32);
    cbuffer[4] = (guchar) ((0xff000000 & buffer[0]) >> 24);
    cbuffer[5] = (guchar) ((0xff0000 & buffer[0]) >> 16);
    cbuffer[6] = (guchar) ((0xff00 & buffer[0]) >> 8);
    cbuffer[7] = (guchar) (0xff & buffer[0]);

    buffer++;
    cbuffer += 8;
  }

  return(start_cbuffer);
}

/**
 * ags_buffer_util_float_to_char_buffer:
 * @buffer: the gfloat buffer
 * @buffer_length: the buffer length
 * 
 * Pack @buffer into an guchar buffer.
 * 
 * Returns: (transfer full): the guchar buffer
 * 
 * Since: 3.0.0
 */
guchar*
ags_buffer_util_float_to_char_buffer(gfloat *buffer,
				     guint buffer_length)
{
  guchar *cbuffer, *start_cbuffer;

  if(buffer == NULL){
    return(NULL);
  }
  
  start_cbuffer = 
    cbuffer = (guchar *) malloc((buffer_length * sizeof(gfloat)) * sizeof(guchar));

  memcpy(cbuffer, buffer, buffer_length * sizeof(gfloat));

  return(start_cbuffer);
}

/**
 * ags_buffer_util_double_to_char_buffer:
 * @buffer: the gdouble buffer
 * @buffer_length: the buffer length
 * 
 * Pack @buffer into an guchar buffer.
 * 
 * Returns: (transfer full): the guchar buffer
 * 
 * Since: 3.0.0
 */
guchar*
ags_buffer_util_double_to_char_buffer(gdouble *buffer,
				      guint buffer_length)
{
  guchar *cbuffer, *start_cbuffer;

  if(buffer == NULL){
    return(NULL);
  }

  start_cbuffer = 
    cbuffer = (guchar *) malloc((buffer_length * sizeof(gdouble)) * sizeof(guchar));

  memcpy(cbuffer, buffer, buffer_length * sizeof(gdouble));

  return(start_cbuffer);
}

/**
 * ags_buffer_util_complex_to_char_buffer:
 * @buffer: the #AgsComplex buffer
 * @buffer_length: the buffer length
 * 
 * Pack @buffer into an guchar buffer.
 * 
 * Returns: (transfer full): the guchar buffer
 * 
 * Since: 3.0.0
 */
guchar*
ags_buffer_util_complex_to_char_buffer(AgsComplex *buffer,
				       guint buffer_length)
{
  guchar *cbuffer, *start_cbuffer;

  if(buffer == NULL){
    return(NULL);
  }

  start_cbuffer = 
    cbuffer = (guchar *) malloc((buffer_length * sizeof(AgsComplex)) * sizeof(guchar));

  memcpy(cbuffer, buffer, buffer_length * sizeof(AgsComplex));

  return(start_cbuffer);
}

/**
 * ags_buffer_util_char_buffer_to_s8:
 * @cbuffer: the guchar buffer
 * @buffer_size: the buffer size
 *
 * Unpack @cbuffer to a gint8 buffer
 *
 * Returns: (transfer full): the gint8 buffer
 * 
 * Since: 3.0.0
 */
gint8*
ags_buffer_util_char_buffer_to_s8(guchar *cbuffer,
				  guint buffer_size)
{
  gint8 *buffer, *start_buffer;

  guint limit;
  guint i;

  if(cbuffer == NULL){
    return(NULL);
  }

  start_buffer = 
    buffer = (gint8 *) malloc(buffer_size * sizeof(gint8));
  memset(buffer, 0, buffer_size * sizeof(gint8));
  
  i = 0;
  
  if(buffer_size > 8){
    limit = buffer_size - (buffer_size % 8);

    for(; i < limit; i += 8){
      buffer[0] |= (0xff & cbuffer[0]);
      buffer[1] |= (0xff & cbuffer[1]);
      buffer[2] |= (0xff & cbuffer[2]);
      buffer[3] |= (0xff & cbuffer[3]);
      buffer[4] |= (0xff & cbuffer[4]);
      buffer[5] |= (0xff & cbuffer[5]);
      buffer[6] |= (0xff & cbuffer[6]);
      buffer[7] |= (0xff & cbuffer[7]);
      
      buffer += 8;
      cbuffer += 8;
    }
  }

  for(; i < buffer_size; i++){
    buffer[0] |= (0xff & cbuffer[0]);
      
    buffer++;
    cbuffer++;
  }

  return(start_buffer);
}

/**
 * ags_buffer_util_char_buffer_to_s16:
 * @cbuffer: the guchar buffer
 * @buffer_size: the buffer size
 *
 * Unpack @cbuffer to a gint16 buffer
 *
 * Returns: (transfer full): the gint16 buffer
 * 
 * Since: 3.0.0
 */
gint16*
ags_buffer_util_char_buffer_to_s16(guchar *cbuffer,
				   guint buffer_size)
{
  gint16 *buffer, *start_buffer;

  guint limit;
  guint i;

  if(cbuffer == NULL){
    return(NULL);
  }

  start_buffer = 
    buffer = (gint16 *) malloc((buffer_size / 2) * sizeof(gint16));
  memset(buffer, 0, (buffer_size / 2) * sizeof(gint16));

  buffer_size = (guint) (2 * floor(buffer_size / 2.0));
  
  i = 0;

  limit = (buffer_size / 2) - ((buffer_size / 2) % 8);

  for(; i < limit; i += 8){
    buffer[0] |= ((gint16) (0xff & cbuffer[0]) << 8);
    buffer[0] |= ((gint16) 0xff & cbuffer[1]);
      
    buffer[1] |= ((gint16) (0xff & cbuffer[2]) << 8);
    buffer[1] |= ((gint16) 0xff & cbuffer[3]);

    buffer[2] |= ((gint16) (0xff & cbuffer[4]) << 8);
    buffer[2] |= ((gint16) 0xff & cbuffer[5]);

    buffer[3] |= ((gint16) (0xff & cbuffer[6]) << 8);
    buffer[3] |= ((gint16) 0xff & cbuffer[7]);
      
    buffer[4] |= ((gint16) (0xff & cbuffer[8]) << 8);
    buffer[4] |= ((gint16) 0xff & cbuffer[9]);
      
    buffer[5] |= ((gint16) (0xff & cbuffer[10]) << 8);
    buffer[5] |= ((gint16) 0xff & cbuffer[11]);

    buffer[6] |= ((gint16) (0xff & cbuffer[12]) << 8);
    buffer[6] |= ((gint16) 0xff & cbuffer[13]);

    buffer[7] |= ((gint16) (0xff & cbuffer[14]) << 8);
    buffer[7] |= ((gint16) 0xff & cbuffer[15]);

    buffer += 8;
    cbuffer += (2 * 8);
  }
  
  for(; i < buffer_size / 2; i++){    
    buffer[0] |= ((gint16) (0xff & cbuffer[0]) << 8);
    buffer[0] |= ((gint16) 0xff & cbuffer[1]);
      
    buffer++;
    cbuffer += 2;
  }

  return(start_buffer);
}

/**
 * ags_buffer_util_char_buffer_to_s24:
 * @cbuffer: the guchar buffer
 * @buffer_size: the buffer size
 *
 * Unpack @cbuffer to a gint32 buffer
 *
 * Returns: (transfer full): the gint32 buffer
 * 
 * Since: 3.0.0
 */
gint32*
ags_buffer_util_char_buffer_to_s24(guchar *cbuffer,
				   guint buffer_size)
{
  gint32 *buffer, *start_buffer;

  guint limit;
  guint i;

  if(cbuffer == NULL){
    return(NULL);
  }

  start_buffer = 
    buffer = (gint32 *) malloc((buffer_size / 4) * sizeof(gint32));
  memset(buffer, 0, (buffer_size / 4) * sizeof(gint32));

  buffer_size = (guint) (4 * floor(buffer_size / 4.0));
  
  i = 0;
  
  limit = (buffer_size / 4) - ((buffer_size / 4) % 8);

  for(; i < limit; i += 8){
    buffer[0] |= ((0xff & cbuffer[0]) << 24);
    buffer[0] |= ((0xff & cbuffer[1]) << 16);
    buffer[0] |= ((0xff & cbuffer[2]) << 8);
    buffer[0] |= (0xff & cbuffer[3]);
      
    buffer[1] |= ((0xff & cbuffer[4]) << 24);
    buffer[1] |= ((0xff & cbuffer[5]) << 16);
    buffer[1] |= ((0xff & cbuffer[6]) << 8);
    buffer[1] |= (0xff & cbuffer[7]);
      
    buffer[2] |= ((0xff & cbuffer[8]) << 24);
    buffer[2] |= ((0xff & cbuffer[9]) << 16);
    buffer[2] |= ((0xff & cbuffer[10]) << 8);
    buffer[2] |= (0xff & cbuffer[11]);

    buffer[3] |= ((0xff & cbuffer[12]) << 24);
    buffer[3] |= ((0xff & cbuffer[13]) << 16);
    buffer[3] |= ((0xff & cbuffer[14]) << 8);
    buffer[3] |= (0xff & cbuffer[15]);

    buffer[4] |= ((0xff & cbuffer[16]) << 24);
    buffer[4] |= ((0xff & cbuffer[17]) << 16);
    buffer[4] |= ((0xff & cbuffer[18]) << 8);
    buffer[4] |= (0xff & cbuffer[19]);

    buffer[5] |= ((0xff & cbuffer[20]) << 24);
    buffer[5] |= ((0xff & cbuffer[21]) << 16);
    buffer[5] |= ((0xff & cbuffer[22]) << 8);
    buffer[5] |= (0xff & cbuffer[23]);

    buffer[6] |= ((0xff & cbuffer[24]) << 24);
    buffer[6] |= ((0xff & cbuffer[25]) << 16);
    buffer[6] |= ((0xff & cbuffer[26]) << 8);
    buffer[6] |= (0xff & cbuffer[27]);

    buffer[7] |= ((0xff & cbuffer[28]) << 24);
    buffer[7] |= ((0xff & cbuffer[29]) << 16);
    buffer[7] |= ((0xff & cbuffer[30]) << 8);
    buffer[7] |= (0xff & cbuffer[31]);
      
    buffer += 8;
    cbuffer += (4 * 8);
  }

  for(; i < buffer_size / 4; i++){
    buffer[0] |= ((0xff & cbuffer[0]) << 24);
    buffer[0] |= ((0xff & cbuffer[1]) << 16);
    buffer[0] |= ((0xff & cbuffer[2]) << 8);
    buffer[0] |= (0xff & cbuffer[3]);
      
    buffer++;
    cbuffer += 4;
  }

  return(start_buffer);
}

/**
 * ags_buffer_util_char_buffer_to_s32:
 * @cbuffer: the guchar buffer
 * @buffer_size: the buffer size
 *
 * Unpack @cbuffer to a gint32 buffer
 *
 * Returns: (transfer full): the gint32 buffer
 * 
 * Since: 3.0.0
 */
gint32*
ags_buffer_util_char_buffer_to_s32(guchar *cbuffer,
				   guint buffer_size)
{
  gint32 *buffer, *start_buffer;

  guint limit;
  guint i;
  
  if(cbuffer == NULL){
    return(NULL);
  }

  start_buffer = 
    buffer = (gint32 *) malloc((buffer_size / 4) * sizeof(gint32));
  memset(buffer, 0, (buffer_size / 4) * sizeof(gint32));

  buffer_size = (guint) (4 * floor(buffer_size / 4.0));
  
  i = 0;
  
  limit = (buffer_size / 4) - ((buffer_size / 4) % 8);

  for(; i < limit; i += 8){
    buffer[0] |= ((0xff & cbuffer[0]) << 24);
    buffer[0] |= ((0xff & cbuffer[1]) << 16);
    buffer[0] |= ((0xff & cbuffer[2]) << 8);
    buffer[0] |= (0xff & cbuffer[3]);
      
    buffer[1] |= ((0xff & cbuffer[4]) << 24);
    buffer[1] |= ((0xff & cbuffer[5]) << 16);
    buffer[1] |= ((0xff & cbuffer[6]) << 8);
    buffer[1] |= (0xff & cbuffer[7]);
      
    buffer[2] |= ((0xff & cbuffer[8]) << 24);
    buffer[2] |= ((0xff & cbuffer[9]) << 16);
    buffer[2] |= ((0xff & cbuffer[10]) << 8);
    buffer[2] |= (0xff & cbuffer[11]);

    buffer[3] |= ((0xff & cbuffer[12]) << 24);
    buffer[3] |= ((0xff & cbuffer[13]) << 16);
    buffer[3] |= ((0xff & cbuffer[14]) << 8);
    buffer[3] |= (0xff & cbuffer[15]);

    buffer[4] |= ((0xff & cbuffer[16]) << 24);
    buffer[4] |= ((0xff & cbuffer[17]) << 16);
    buffer[4] |= ((0xff & cbuffer[18]) << 8);
    buffer[4] |= (0xff & cbuffer[19]);

    buffer[5] |= ((0xff & cbuffer[20]) << 24);
    buffer[5] |= ((0xff & cbuffer[21]) << 16);
    buffer[5] |= ((0xff & cbuffer[22]) << 8);
    buffer[5] |= (0xff & cbuffer[23]);

    buffer[6] |= ((0xff & cbuffer[24]) << 24);
    buffer[6] |= ((0xff & cbuffer[25]) << 16);
    buffer[6] |= ((0xff & cbuffer[26]) << 8);
    buffer[6] |= (0xff & cbuffer[27]);

    buffer[7] |= ((0xff & cbuffer[28]) << 24);
    buffer[7] |= ((0xff & cbuffer[29]) << 16);
    buffer[7] |= ((0xff & cbuffer[30]) << 8);
    buffer[7] |= (0xff & cbuffer[31]);

    buffer += 8;
    cbuffer += (4 * 8);
  }

  for(; i < buffer_size / 4; i++){
    buffer[0] |= ((0xff & cbuffer[0]) << 24);
    buffer[0] |= ((0xff & cbuffer[1]) << 16);
    buffer[0] |= ((0xff & cbuffer[2]) << 8);
    buffer[0] |= (0xff & cbuffer[3]);
      
    buffer++;
    cbuffer += 4;
  }

  return(start_buffer);
}

/**
 * ags_buffer_util_char_buffer_to_s64:
 * @cbuffer: the guchar buffer
 * @buffer_size: the buffer size
 *
 * Unpack @cbuffer to a gint64 buffer
 *
 * Returns: (transfer full): the gint64 buffer
 * 
 * Since: 3.0.0
 */
gint64*
ags_buffer_util_char_buffer_to_s64(guchar *cbuffer,
				   guint buffer_size)
{
  gint64 *buffer, *start_buffer;

  guint limit;
  guint i;

  if(cbuffer == NULL){
    return(NULL);
  }

  start_buffer = 
    buffer = (gint64 *) malloc((buffer_size / 8) * sizeof(gint64));
  memset(buffer, 0, (buffer_size / 8) * sizeof(gint64));

  buffer_size = (guint) (8 * floor(buffer_size / 8.0)); 
  
  i = 0;
  
  limit = (buffer_size / 8) - ((buffer_size / 8) % 8);

  for(; i < limit; i += 8){
    buffer[0] |= ((gint64) (0xff & cbuffer[0]) << 56);
    buffer[0] |= ((gint64) (0xff & cbuffer[1]) << 48);
    buffer[0] |= ((gint64) (0xff & cbuffer[2]) << 40);
    buffer[0] |= ((gint64) (0xff & cbuffer[3]) << 32);
    buffer[0] |= ((gint64) (0xff & cbuffer[4]) << 24);
    buffer[0] |= ((gint64) (0xff & cbuffer[5]) << 16);
    buffer[0] |= ((gint64) (0xff & cbuffer[6]) << 8);
    buffer[0] |= (gint64) (0xff & cbuffer[7]);
      
    buffer[1] |= ((gint64) (0xff & cbuffer[8]) << 56);
    buffer[1] |= ((gint64) (0xff & cbuffer[9]) << 48);
    buffer[1] |= ((gint64) (0xff & cbuffer[10]) << 40);
    buffer[1] |= ((gint64) (0xff & cbuffer[11]) << 32);
    buffer[1] |= ((gint64) (0xff & cbuffer[12]) << 24);
    buffer[1] |= ((gint64) (0xff & cbuffer[13]) << 16);
    buffer[1] |= ((gint64) (0xff & cbuffer[14]) << 8);
    buffer[1] |= (gint64) (0xff & cbuffer[15]);
      
    buffer[2] |= ((gint64) (0xff & cbuffer[16]) << 56);
    buffer[2] |= ((gint64) (0xff & cbuffer[17]) << 48);
    buffer[2] |= ((gint64) (0xff & cbuffer[18]) << 40);
    buffer[2] |= ((gint64) (0xff & cbuffer[19]) << 32);
    buffer[2] |= ((gint64) (0xff & cbuffer[20]) << 24);
    buffer[2] |= ((gint64) (0xff & cbuffer[21]) << 16);
    buffer[2] |= ((gint64) (0xff & cbuffer[22]) << 8);
    buffer[2] |= (gint64) (0xff & cbuffer[23]);
      
    buffer[3] |= ((gint64) (0xff & cbuffer[24]) << 56);
    buffer[3] |= ((gint64) (0xff & cbuffer[25]) << 48);
    buffer[3] |= ((gint64) (0xff & cbuffer[26]) << 40);
    buffer[3] |= ((gint64) (0xff & cbuffer[27]) << 32);
    buffer[3] |= ((gint64) (0xff & cbuffer[28]) << 24);
    buffer[3] |= ((gint64) (0xff & cbuffer[29]) << 16);
    buffer[3] |= ((gint64) (0xff & cbuffer[30]) << 8);
    buffer[3] |= (gint64) (0xff & cbuffer[31]);
      
    buffer[4] |= ((gint64) (0xff & cbuffer[32]) << 56);
    buffer[4] |= ((gint64) (0xff & cbuffer[33]) << 48);
    buffer[4] |= ((gint64) (0xff & cbuffer[34]) << 40);
    buffer[4] |= ((gint64) (0xff & cbuffer[35]) << 32);
    buffer[4] |= ((gint64) (0xff & cbuffer[36]) << 24);
    buffer[4] |= ((gint64) (0xff & cbuffer[37]) << 16);
    buffer[4] |= ((gint64) (0xff & cbuffer[38]) << 8);
    buffer[4] |= (gint64) (0xff & cbuffer[39]);
      
    buffer[5] |= ((gint64) (0xff & cbuffer[40]) << 56);
    buffer[5] |= ((gint64) (0xff & cbuffer[41]) << 48);
    buffer[5] |= ((gint64) (0xff & cbuffer[42]) << 40);
    buffer[5] |= ((gint64) (0xff & cbuffer[43]) << 32);
    buffer[5] |= ((gint64) (0xff & cbuffer[44]) << 24);
    buffer[5] |= ((gint64) (0xff & cbuffer[45]) << 16);
    buffer[5] |= ((gint64) (0xff & cbuffer[46]) << 8);
    buffer[5] |= (gint64) (0xff & cbuffer[47]);
      
    buffer[6] |= ((gint64) (0xff & cbuffer[48]) << 56);
    buffer[6] |= ((gint64) (0xff & cbuffer[49]) << 48);
    buffer[6] |= ((gint64) (0xff & cbuffer[50]) << 32);
    buffer[6] |= ((gint64) (0xff & cbuffer[52]) << 24);
    buffer[6] |= ((gint64) (0xff & cbuffer[51]) << 16);
    buffer[6] |= ((gint64) (0xff & cbuffer[52]) << 40);
    buffer[6] |= ((gint64) (0xff & cbuffer[53]) << 8);
    buffer[6] |= (gint64) (0xff & cbuffer[54]);
      
    buffer[7] |= ((gint64) (0xff & cbuffer[56]) << 56);
    buffer[7] |= ((gint64) (0xff & cbuffer[57]) << 48);
    buffer[7] |= ((gint64) (0xff & cbuffer[58]) << 40);
    buffer[7] |= ((gint64) (0xff & cbuffer[59]) << 32);
    buffer[7] |= ((gint64) (0xff & cbuffer[60]) << 24);
    buffer[7] |= ((gint64) (0xff & cbuffer[61]) << 16);
    buffer[7] |= ((gint64) (0xff & cbuffer[62]) << 8);
    buffer[7] |= (gint64) (0xff & cbuffer[63]);
      
    buffer += 8;
    cbuffer += (8 * 8);
  }

  for(; i < buffer_size / 8; i++){
    buffer[0] |= ((gint64) (0xff & cbuffer[0]) << 56);
    buffer[0] |= ((gint64) (0xff & cbuffer[1]) << 48);
    buffer[0] |= ((gint64) (0xff & cbuffer[2]) << 40);
    buffer[0] |= ((gint64) (0xff & cbuffer[3]) << 32);
    buffer[0] |= ((gint64) (0xff & cbuffer[4]) << 24);
    buffer[0] |= ((gint64) (0xff & cbuffer[5]) << 16);
    buffer[0] |= ((gint64) (0xff & cbuffer[6]) << 8);
    buffer[0] |= (gint64) (0xff & cbuffer[7]);
    
    buffer++;
    cbuffer += 8;
  }

  return(start_buffer);
}

/**
 * ags_buffer_util_char_buffer_to_float:
 * @cbuffer: the guchar buffer
 * @buffer_size: the buffer size
 *
 * Unpack @cbuffer to a gfloat buffer
 *
 * Returns: (transfer full): the gfloat buffer
 * 
 * Since: 3.0.0
 */
gfloat*
ags_buffer_util_char_buffer_to_float(guchar *cbuffer,
				     guint buffer_size)
{
  gfloat *buffer, *start_buffer;
  
  if(cbuffer == NULL){
    return(NULL);
  }

  start_buffer = 
    buffer = (gfloat *) malloc((buffer_size / sizeof(gfloat)) * sizeof(gfloat));

  buffer_size = (guint) (sizeof(gfloat) * floor(buffer_size / sizeof(gfloat))); 

  memcpy(buffer, cbuffer, buffer_size * sizeof(guchar));

  return(start_buffer);
}

/**
 * ags_buffer_util_char_buffer_to_double:
 * @cbuffer: the guchar buffer
 * @buffer_size: the buffer size
 *
 * Unpack @cbuffer to a gdouble buffer
 *
 * Returns: (transfer full): the gdouble buffer
 * 
 * Since: 3.0.0
 */
gdouble*
ags_buffer_util_char_buffer_to_double(guchar *cbuffer,
				      guint buffer_size)
{
  gdouble *buffer, *start_buffer;
  
  if(cbuffer == NULL){
    return(NULL);
  }

  start_buffer = 
    buffer = (gdouble *) malloc((buffer_size / sizeof(gdouble)) * sizeof(gdouble));

  buffer_size = (guint) (sizeof(gdouble) * floor(buffer_size / sizeof(gdouble))); 

  memcpy(buffer, cbuffer, buffer_size * sizeof(guchar));

  return(start_buffer);
}

/**
 * ags_buffer_util_char_buffer_to_complex:
 * @cbuffer: the guchar buffer
 * @buffer_size: the buffer size
 *
 * Unpack @cbuffer to a #AgsComplex buffer
 *
 * Returns: (transfer full): the #AgsComplex buffer
 * 
 * Since: 3.0.0
 */
AgsComplex*
ags_buffer_util_char_buffer_to_complex(guchar *cbuffer,
				       guint buffer_size)
{
  AgsComplex *buffer, *start_buffer;
  
  if(cbuffer == NULL){
    return(NULL);
  }

  start_buffer = 
    buffer = (AgsComplex *) malloc((buffer_size / sizeof(AgsComplex)) * sizeof(AgsComplex));

  buffer_size = (guint) (sizeof(AgsComplex) * floor(buffer_size / sizeof(AgsComplex))); 

  memcpy(buffer, cbuffer, buffer_size * sizeof(guchar));

  return(start_buffer);
}

/**
 * ags_buffer_util_char_buffer_read_s8:
 * @cbuffer: the character buffer
 * @byte_order: reverse order, ignored here
 * 
 * Read a gint8 quantity of @cbuffer.
 * 
 * Returns: the gint8 value
 * 
 * Since: 3.0.0
 */
gint8
ags_buffer_util_char_buffer_read_s8(guchar *cbuffer,
				    guint byte_order)
{
  gint8 val;

  if(cbuffer == NULL){
    return(0);
  }

  val = 0;
  val |= cbuffer[0];

  return(val);
}

/**
 * ags_buffer_util_char_buffer_read_s16:
 * @cbuffer: the character buffer
 * @byte_order: reverse order
 * 
 * Read a gint16 quantity of @cbuffer.
 * 
 * Returns: the gint16 value
 * 
 * Since: 3.0.0
 */
gint16
ags_buffer_util_char_buffer_read_s16(guchar *cbuffer,
				     guint byte_order)
{
  gint16 val;

  if(cbuffer == NULL){
    return(0);
  }

  val = 0;
  
  if(byte_order == AGS_BYTE_ORDER_BE){
    val |= (cbuffer[0]);
    val |= (cbuffer[1] << 8);
  }else{
    val |= (cbuffer[0] << 8);
    val |= (cbuffer[1]);
  }

  return(val);
}

/**
 * ags_buffer_util_char_buffer_read_s24:
 * @cbuffer: the character buffer
 * @byte_order: reverse order
 * 
 * Read a gint32 24 bit quantity of @cbuffer.
 * 
 * Returns: the gint32 value
 * 
 * Since: 3.0.0
 */
gint32
ags_buffer_util_char_buffer_read_s24(guchar *cbuffer,
				     guint byte_order)
{
  gint32 val;

  if(cbuffer == NULL){
    return(0);
  }

  val = 0;
  
  if(byte_order == AGS_BYTE_ORDER_BE){
    val |= (cbuffer[0]);
    val |= (cbuffer[1] << 8);
    val |= (cbuffer[2] << 16);
  }else{
    val |= (cbuffer[0] << 16);
    val |= (cbuffer[1] << 8);
    val |= (cbuffer[2]);
  }

  return(val);
}

/**
 * ags_buffer_util_char_buffer_read_s32:
 * @cbuffer: the character buffer
 * @byte_order: reverse order
 * 
 * Read a gint32 32 bit quantity of @cbuffer.
 * 
 * Returns: the gint32 value
 * 
 * Since: 3.0.0
 */
gint32
ags_buffer_util_char_buffer_read_s32(guchar *cbuffer,
				     guint byte_order)
{
  gint32 val;

  if(cbuffer == NULL){
    return(0);
  }

  val = 0;
  
  if(byte_order == AGS_BYTE_ORDER_BE){
    val |= (cbuffer[0]);
    val |= (cbuffer[1] << 8);
    val |= (cbuffer[2] << 16);
    val |= (cbuffer[3] << 24);
  }else{
    val |= (cbuffer[0] << 24);
    val |= (cbuffer[1] << 16);
    val |= (cbuffer[2] << 8);
    val |= (cbuffer[3]);
  }

  return(val);
}

/**
 * ags_buffer_util_char_buffer_read_s64:
 * @cbuffer: the character buffer
 * @byte_order: reverse order
 * 
 * Read a gint64 64 bit quantity of @cbuffer.
 * 
 * Returns: the gint64 value
 * 
 * Since: 3.0.0
 */
gint64
ags_buffer_util_char_buffer_read_s64(guchar *cbuffer,
				     guint byte_order)
{
  gint64 val;

  if(cbuffer == NULL){
    return(0);
  }

  val = 0;
  
  if(byte_order == AGS_BYTE_ORDER_BE){
    val |= ((gint64) cbuffer[0]);
    val |= ((gint64) cbuffer[1] << 8);
    val |= ((gint64) cbuffer[2] << 16);
    val |= ((gint64) cbuffer[3] << 24);
    val |= ((gint64) cbuffer[4] << 32);
    val |= ((gint64) cbuffer[5] << 40);
    val |= ((gint64) cbuffer[6] << 48);
    val |= ((gint64) cbuffer[7] << 56);
  }else{
    val |= ((gint64) cbuffer[0] << 56);
    val |= ((gint64) cbuffer[1] << 48);
    val |= ((gint64) cbuffer[2] << 40);
    val |= ((gint64) cbuffer[3] << 32);
    val |= ((gint64) cbuffer[4] << 24);
    val |= ((gint64) cbuffer[5] << 16);
    val |= ((gint64) cbuffer[6] << 8);
    val |= ((gint64) cbuffer[7]);
  }

  return(val);
}

/**
 * ags_buffer_util_char_buffer_read_float:
 * @cbuffer: the character buffer
 * @byte_order: reverse order
 * 
 * Read a gfloat quantity of @cbuffer.
 * 
 * Returns: the gfloat value
 * 
 * Since: 3.0.0
 */
gfloat
ags_buffer_util_char_buffer_read_float(guchar *cbuffer,
				       guint byte_order)
{
  union{
    guint32 val;
    GFloatIEEE754 ieee_float;
  }data;

  if(cbuffer == NULL){
    return(0.0);
  }
  
  data.val = 0;
  
  if(byte_order == AGS_BYTE_ORDER_BE){
    data.val |= (cbuffer[0]);
    data.val |= (cbuffer[1] << 8);
    data.val |= (cbuffer[2] << 16);
    data.val |= (cbuffer[3] << 24);
  }else{
    data.val |= (cbuffer[0] << 24);
    data.val |= (cbuffer[1] << 16);
    data.val |= (cbuffer[2] << 8);
    data.val |= (cbuffer[3]);
  }

  return(data.ieee_float.v_float);
}

/**
 * ags_buffer_util_char_buffer_read_double:
 * @cbuffer: the character buffer
 * @byte_order: reverse order
 * 
 * Read a gdouble quantity of @cbuffer.
 * 
 * Returns: the gdouble value
 * 
 * Since: 3.0.0
 */
gdouble
ags_buffer_util_char_buffer_read_double(guchar *cbuffer,
					guint byte_order)
{
  union{
    guint64 val;
    GDoubleIEEE754 ieee_double;
  }data;

  if(cbuffer == NULL){
    return(0.0);
  }

  data.val = 0;
  
  if(byte_order == AGS_BYTE_ORDER_BE){
    data.val |= ((guint64) cbuffer[0]);
    data.val |= ((guint64) cbuffer[1] << 8);
    data.val |= ((guint64) cbuffer[2] << 16);
    data.val |= ((guint64) cbuffer[3] << 24);
    data.val |= ((guint64) cbuffer[4] << 32);
    data.val |= ((guint64) cbuffer[5] << 40);
    data.val |= ((guint64) cbuffer[6] << 48);
    data.val |= ((guint64) cbuffer[7] << 56);
  }else{
    data.val |= ((guint64) cbuffer[0] << 56);
    data.val |= ((guint64) cbuffer[1] << 48);
    data.val |= ((guint64) cbuffer[2] << 40);
    data.val |= ((guint64) cbuffer[3] << 32);
    data.val |= ((guint64) cbuffer[4] << 24);
    data.val |= ((guint64) cbuffer[5] << 16);
    data.val |= ((guint64) cbuffer[6] << 8);
    data.val |= ((guint64) cbuffer[7]);
  }

  return(data.ieee_double.v_double);
}

/**
 * ags_buffer_util_char_buffer_read_complex:
 * @cbuffer: the character buffer
 * @byte_order: reverse order
 * 
 * Read a #AgsComplex quantity of @cbuffer.
 * 
 * Returns: the #AgsComplex value
 * 
 * Since: 3.0.0
 */
AgsComplex*
ags_buffer_util_char_buffer_read_complex(guchar *cbuffer,
					 guint byte_order)
{
  union{
    guchar val[sizeof(AgsComplex)];
    AgsComplex c_val;
  }data;

  guint i;

  if(cbuffer == NULL){
    return(NULL);
  }
  
  for(i = 0; i < sizeof(AgsComplex); i++){
    if(byte_order == AGS_BYTE_ORDER_BE){
      data.val[i] = cbuffer[i];
    }else{
      data.val[i] = cbuffer[sizeof(AgsComplex) - i - 1];
    }
  }
  
  return(ags_complex_copy(&(data.c_val)));
}

/**
 * ags_buffer_util_char_buffer_write_s8:
 * @cbuffer: the character buffer
 * @value: the gint8 value
 * @byte_order: reverse order
 * 
 * Write a gint8 quantity to @cbuffer.
 * 
 * Since: 3.0.0
 */
void
ags_buffer_util_char_buffer_write_s8(guchar *cbuffer,
				     gint8 value,
				     guint byte_order)
{
  if(cbuffer == NULL){
    return;
  }
  
  cbuffer[0] = value;
}

/**
 * ags_buffer_util_char_buffer_write_s16:
 * @cbuffer: the character buffer
 * @value: the gint16 value
 * @byte_order: reverse order
 * 
 * Write a gint16 quantity to @cbuffer.
 * 
 * Since: 3.0.0
 */
void
ags_buffer_util_char_buffer_write_s16(guchar *cbuffer,
				      gint16 value,
				      guint byte_order)
{
  if(cbuffer == NULL){
    return;
  }

  if(byte_order == AGS_BYTE_ORDER_BE){
    cbuffer[0] = (0xff & value);
    cbuffer[1] = (0xff00 & value) >> 8;
  }else{
    cbuffer[0] = (0xff00 & value) >> 8;
    cbuffer[1] = (0xff & value);
  }
}

/**
 * ags_buffer_util_char_buffer_write_s24:
 * @cbuffer: the character buffer
 * @value: the gint32 value
 * @byte_order: reverse order
 * 
 * Write a gint32 quantity to @cbuffer.
 * 
 * Since: 3.0.0
 */
void
ags_buffer_util_char_buffer_write_s24(guchar *cbuffer,
				      gint32 value,
				      guint byte_order)
{
  if(cbuffer == NULL){
    return;
  }

  if(byte_order == AGS_BYTE_ORDER_BE){
    cbuffer[0] = (0xff & value);
    cbuffer[1] = (0xff00 & value) >> 8;
    cbuffer[2] = (0xff0000 & value) >> 16;
  }else{
    cbuffer[0] = (0xff0000 & value) >> 16;
    cbuffer[1] = (0xff00 & value) >> 8;
    cbuffer[2] = (0xff & value);
  }
}

/**
 * ags_buffer_util_char_buffer_write_s32:
 * @cbuffer: the character buffer
 * @value: the gint32 value
 * @byte_order: reverse order
 * 
 * Write a gint32 quantity to @cbuffer.
 * 
 * Since: 3.0.0
 */
void
ags_buffer_util_char_buffer_write_s32(guchar *cbuffer,
				      gint32 value,
				      guint byte_order)
{
  if(cbuffer == NULL){
    return;
  }

  if(byte_order == AGS_BYTE_ORDER_BE){
    cbuffer[0] = (0xff & value);
    cbuffer[1] = (0xff00 & value) >> 8;
    cbuffer[2] = (0xff0000 & value) >> 16;
    cbuffer[3] = (0xff000000 & value) >> 24;
  }else{
    cbuffer[0] = (0xff000000 & value) >> 24;
    cbuffer[1] = (0xff0000 & value) >> 16;
    cbuffer[2] = (0xff00 & value) >> 8;
    cbuffer[3] = (0xff & value);
  }
}

/**
 * ags_buffer_util_char_buffer_write_s64:
 * @cbuffer: the character buffer
 * @value: the gint64 value
 * @byte_order: reverse order
 * 
 * Write a gint64 quantity to @cbuffer.
 * 
 * Since: 3.0.0
 */
void
ags_buffer_util_char_buffer_write_s64(guchar *cbuffer,
				      gint64 value,
				      guint byte_order)
{
  if(cbuffer == NULL){
    return;
  }

  if(byte_order == AGS_BYTE_ORDER_BE){
    cbuffer[0] = (0xff & value);
    cbuffer[1] = (0xff00 & value) >> 8;
    cbuffer[2] = (0xff0000 & value) >> 16;
    cbuffer[3] = (0xff000000 & value) >> 24;
    cbuffer[4] = (0xff00000000 & value) >> 32;
    cbuffer[5] = (0xff0000000000 & value) >> 40;
    cbuffer[6] = (0xff000000000000 & value) >> 48;
    cbuffer[7] = (0xff00000000000000 & value) >> 56;
  }else{
    cbuffer[0] = (0xff00000000000000 & value) >> 56;
    cbuffer[1] = (0xff000000000000 & value) >> 48;
    cbuffer[2] = (0xff0000000000 & value) >> 40;
    cbuffer[3] = (0xff00000000 & value) >> 32;
    cbuffer[4] = (0xff000000 & value) >> 24;
    cbuffer[5] = (0xff0000 & value) >> 16;
    cbuffer[6] = (0xff00 & value) >> 8;
    cbuffer[7] = (0xff & value);
  }
}

/**
 * ags_buffer_util_char_buffer_write_float:
 * @cbuffer: the character buffer
 * @value: the gfloat value
 * @byte_order: reverse order
 * 
 * Write a gfloat quantity to @cbuffer.
 * 
 * Since: 3.0.0
 */
void
ags_buffer_util_char_buffer_write_float(guchar *cbuffer,
					gfloat value,
					guint byte_order)
{
  union{
    guint32 val;
    GFloatIEEE754 ieee_float;
  }data;

  if(cbuffer == NULL){
    return;
  }

  data.ieee_float.v_float = value;
  
  if(byte_order == AGS_BYTE_ORDER_BE){
    cbuffer[0] = (0xff & data.val);
    cbuffer[1] = (0xff00 & data.val) >> 8;
    cbuffer[2] = (0xff0000 & data.val) >> 16;
    cbuffer[3] = (0xff000000 & data.val) >> 24;
  }else{
    cbuffer[0] = (0xff000000 & data.val) >> 24;
    cbuffer[1] = (0xff0000 & data.val) >> 16;
    cbuffer[2] = (0xff00 & data.val) >> 8;
    cbuffer[3] = (0xff & data.val);
  }
}

/**
 * ags_buffer_util_char_buffer_write_double:
 * @cbuffer: the character buffer
 * @value: the gdouble value
 * @byte_order: reverse order
 * 
 * Write a gdouble quantity to @cbuffer.
 * 
 * Since: 3.0.0
 */
void
ags_buffer_util_char_buffer_write_double(guchar *cbuffer,
					 gdouble value,
					 guint byte_order)
{
  union{
    guint64 val;
    GDoubleIEEE754 ieee_double;
  }data;

  if(cbuffer == NULL){
    return;
  }

  data.ieee_double.v_double = value;

  if(byte_order == AGS_BYTE_ORDER_BE){
    cbuffer[0] = (0xff & data.val);
    cbuffer[1] = (0xff00 & data.val) >> 8;
    cbuffer[2] = (0xff0000 & data.val) >> 16;
    cbuffer[3] = (0xff000000 & data.val) >> 24;
    cbuffer[4] = (0xff00000000 & data.val) >> 32;
    cbuffer[5] = (0xff0000000000 & data.val) >> 40;
    cbuffer[6] = (0xff000000000000 & data.val) >> 48;
    cbuffer[7] = (0xff00000000000000 & data.val) >> 56;
  }else{
    cbuffer[0] = (0xff00000000000000 & data.val) >> 56;
    cbuffer[1] = (0xff000000000000 & data.val) >> 48;
    cbuffer[2] = (0xff0000000000 & data.val) >> 40;
    cbuffer[3] = (0xff00000000 & data.val) >> 32;
    cbuffer[4] = (0xff000000 & data.val) >> 24;
    cbuffer[5] = (0xff0000 & data.val) >> 16;
    cbuffer[6] = (0xff00 & data.val) >> 8;
    cbuffer[7] = (0xff & data.val);
  }
}

/**
 * ags_buffer_util_char_buffer_write_complex:
 * @cbuffer: the character buffer
 * @value: the #AgsComplex value
 * @byte_order: reverse order
 * 
 * Write a #AgsComplex quantity to @cbuffer.
 * 
 * Since: 3.0.0
 */
void
ags_buffer_util_char_buffer_write_complex(guchar *cbuffer,
					  AgsComplex *value,
					  guint byte_order)
{
  union{
    guchar val[sizeof(AgsComplex)];
    AgsComplex c_val;
  }data;

  guint i;

  if(cbuffer == NULL){
    return;
  }

  ags_complex_set(&(data.c_val),
		  ags_complex_get(value));

  for(i = 0; i < sizeof(AgsComplex); i++){
    if(byte_order == AGS_BYTE_ORDER_BE){
      cbuffer[i] = data.val[i];
    }else{
      cbuffer[i] = data.val[sizeof(AgsComplex) - i - 1];
    }
  }
}


/**
 * ags_buffer_util_char_buffer_swap_bytes:
 * @cbuffer: the buffer to swap
 * @word_size: the word size
 * @buffer_size: the buffer size
 * 
 * Swap bytes in view of Little/Big-Endian.
 * 
 * Since: 3.0.0
 */
void
ags_buffer_util_char_buffer_swap_bytes(guchar *cbuffer, guint word_size,
				       guint buffer_size)
{
  guchar tmp_buffer[8];
    
  guint i, j;

  if(cbuffer == NULL){
    return;
  }

  if(word_size <= 1){
    return;
  }

  buffer_size = (guint) (word_size * floor(buffer_size / word_size));
  
  for(i = 0; i < buffer_size; i += word_size){
    for(j = 0; j < word_size; j++){
      tmp_buffer[j] = cbuffer[i + (word_size - j - 1)];
    }

    for(j = 0; j < word_size; j++){
      cbuffer[i + j] = tmp_buffer[j];
    }    
  }
}
