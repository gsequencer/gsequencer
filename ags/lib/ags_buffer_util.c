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

#include <ags/lib/ags_buffer_util.h>

#include <ags/lib/ags_endian.h>

#include <stdlib.h>
#include <string.h>

/**
 * ags_buffer_util_s8_to_char_buffer:
 * @buffer: the signed char buffer
 * @buffer_length: the buffer length
 * 
 * Pack @buffer into an unsigned char buffer.
 * 
 * Returns: the unsigned char buffer
 * 
 * Since: 2.0.0
 */
unsigned char*
ags_buffer_util_s8_to_char_buffer(signed char *buffer,
				  guint buffer_length)
{
  unsigned char *cbuffer;

  guint limit;
  guint i;
  
  cbuffer = (unsigned char *) malloc(buffer_length * sizeof(unsigned char));

  i = 0;
  
  if(buffer_length > 8){
    limit = buffer_length - 8;

    for(; i < limit; i += 8){
      cbuffer[0] = (unsigned char) (0xff & buffer[0]);
      cbuffer[1] = (unsigned char) (0xff & buffer[1]);
      cbuffer[2] = (unsigned char) (0xff & buffer[2]);
      cbuffer[3] = (unsigned char) (0xff & buffer[3]);
      cbuffer[4] = (unsigned char) (0xff & buffer[4]);
      cbuffer[5] = (unsigned char) (0xff & buffer[5]);
      cbuffer[6] = (unsigned char) (0xff & buffer[6]);
      cbuffer[7] = (unsigned char) (0xff & buffer[7]);

      buffer += 8;
      cbuffer += 8;
    }
  }

  for(; i < buffer_length; i++){
    cbuffer[0] = (unsigned char) (0xff & buffer[0]);

    buffer++;
    cbuffer++;
  }

  return(cbuffer);
}

/**
 * ags_buffer_util_s16_to_char_buffer:
 * @buffer: the signed short buffer
 * @buffer_length: the buffer length
 * 
 * Pack @buffer into an unsigned char buffer.
 * 
 * Returns: the unsigned char buffer
 * 
 * Since: 2.0.0
 */
unsigned char*
ags_buffer_util_s16_to_char_buffer(signed short *buffer,
				   guint buffer_length)
{
  unsigned char *cbuffer;

  guint limit;
  guint i;
  
  cbuffer = (unsigned char *) malloc((2 * buffer_length) * sizeof(unsigned char));

  i = 0;
  
  if(buffer_length > 8){
    limit = buffer_length - 8;

    for(; i < limit; i += 8){
      cbuffer[0] = (unsigned char) ((0xff00 & buffer[0]) >> 8);
      cbuffer[1] = (unsigned char) (0xff & buffer[0]);

      cbuffer[2] = (unsigned char) ((0xff00 & buffer[1]) >> 8);
      cbuffer[3] = (unsigned char) (0xff & buffer[1]);

      cbuffer[4] = (unsigned char) ((0xff00 & buffer[2]) >> 8);
      cbuffer[5] = (unsigned char) (0xff & buffer[2]);

      cbuffer[6] = (unsigned char) ((0xff00 & buffer[3]) >> 8);
      cbuffer[7] = (unsigned char) (0xff & buffer[3]);

      cbuffer[8] = (unsigned char) ((0xff00 & buffer[4]) >> 8);
      cbuffer[9] = (unsigned char) (0xff & buffer[4]);
      
      cbuffer[10] = (unsigned char) ((0xff00 & buffer[5]) >> 8);
      cbuffer[11] = (unsigned char) (0xff & buffer[5]);

      cbuffer[12] = (unsigned char) ((0xff00 & buffer[6]) >> 8);
      cbuffer[13] = (unsigned char) (0xff & buffer[6]);

      cbuffer[14] = (unsigned char) ((0xff00 & buffer[7]) >> 8);
      cbuffer[15] = (unsigned char) (0xff & buffer[7]);
      
      buffer += 8;
      cbuffer += (2 * 8);
    }
  }

  for(; i < buffer_length; i++){
    cbuffer[0] = (unsigned char) ((0xff00 & buffer[0]) >> 8);
    cbuffer[1] = (unsigned char) (0xff & buffer[0]);

    buffer++;
    cbuffer += 2;
  }

  return(cbuffer);
}

/**
 * ags_buffer_util_s24_to_char_buffer:
 * @buffer: the signed long buffer
 * @buffer_length: the buffer length
 * 
 * Pack @buffer into an unsigned char buffer.
 * 
 * Returns: the unsigned char buffer
 * 
 * Since: 2.0.0
 */
unsigned char*
ags_buffer_util_s24_to_char_buffer(signed long *buffer,
				   guint buffer_length)
{
  unsigned char *cbuffer;

  guint limit;
  guint i;
  
  cbuffer = (unsigned char *) malloc((4 * buffer_length) * sizeof(unsigned char));

  i = 0;
  
  if(buffer_length > 8){
    limit = buffer_length - 8;

    for(; i < limit; i += 8){
      cbuffer[0] = 0x0;
      cbuffer[1] = (unsigned char) ((0xff0000 & buffer[0]) >> 16);
      cbuffer[2] = (unsigned char) ((0xff00 & buffer[0]) >> 8);
      cbuffer[3] = (unsigned char) (0xff & buffer[0]);
      
      cbuffer[4] = 0x0;
      cbuffer[5] = (unsigned char) ((0xff0000 & buffer[1]) >> 16);
      cbuffer[6] = (unsigned char) ((0xff00 & buffer[1]) >> 8);
      cbuffer[7] = (unsigned char) (0xff & buffer[1]);

      cbuffer[8] = 0x0;
      cbuffer[9] = (unsigned char) ((0xff0000 & buffer[2]) >> 16);
      cbuffer[10] = (unsigned char) ((0xff00 & buffer[2]) >> 8);
      cbuffer[11] = (unsigned char) (0xff & buffer[2]);

      cbuffer[12] = 0x0;
      cbuffer[13] = (unsigned char) ((0xff0000 & buffer[3]) >> 16);
      cbuffer[14] = (unsigned char) ((0xff00 & buffer[3]) >> 8);
      cbuffer[15] = (unsigned char) (0xff & buffer[3]);

      cbuffer[16] = 0x0;
      cbuffer[17] = (unsigned char) ((0xff0000 & buffer[4]) >> 16);
      cbuffer[18] = (unsigned char) ((0xff00 & buffer[4]) >> 8);
      cbuffer[19] = (unsigned char) (0xff & buffer[4]);

      cbuffer[20] = 0x0;
      cbuffer[21] = (unsigned char) ((0xff0000 & buffer[5]) >> 16);
      cbuffer[22] = (unsigned char) ((0xff00 & buffer[5]) >> 8);
      cbuffer[23] = (unsigned char) (0xff & buffer[5]);

      cbuffer[24] = 0x0;
      cbuffer[25] = (unsigned char) ((0xff0000 & buffer[6]) >> 16);
      cbuffer[26] = (unsigned char) ((0xff00 & buffer[6]) >> 8);
      cbuffer[27] = (unsigned char) (0xff & buffer[6]);

      cbuffer[28] = 0x0;
      cbuffer[29] = (unsigned char) ((0xff0000 & buffer[7]) >> 16);
      cbuffer[30] = (unsigned char) ((0xff00 & buffer[7]) >> 8);
      cbuffer[31] = (unsigned char) (0xff & buffer[7]);

      buffer += 8;
      cbuffer += (4 * 8);
    }
  }

  for(; i < buffer_length; i++){
    cbuffer[0] = 0x0;
    cbuffer[1] = (unsigned char) ((0xff0000 & buffer[0]) >> 16);
    cbuffer[2] = (unsigned char) ((0xff00 & buffer[0]) >> 8);
    cbuffer[3] = (unsigned char) (0xff & buffer[0]);

    buffer++;
    cbuffer += 4;
  }

  return(cbuffer);
}

/**
 * ags_buffer_util_s32_to_char_buffer:
 * @buffer: the signed long buffer
 * @buffer_length: the buffer length
 * 
 * Pack @buffer into an unsigned char buffer.
 * 
 * Returns: the unsigned char buffer
 * 
 * Since: 2.0.0
 */
unsigned char*
ags_buffer_util_s32_to_char_buffer(signed long *buffer,
				   guint buffer_length)
{
  unsigned char *cbuffer;

  guint limit;
  guint i;
  
  cbuffer = (unsigned char *) malloc((4 * buffer_length) * sizeof(unsigned char));

  i = 0;
  
  if(buffer_length > 8){
    limit = buffer_length - 8;

    for(; i < limit; i += 8){
      cbuffer[0] = (unsigned char) ((0xff000000 & buffer[0]) >> 24);
      cbuffer[1] = (unsigned char) ((0xff0000 & buffer[0]) >> 16);
      cbuffer[2] = (unsigned char) ((0xff00 & buffer[0]) >> 8);
      cbuffer[3] = (unsigned char) (0xff & buffer[0]);
      
      cbuffer[4] = (unsigned char) ((0xff000000 & buffer[1]) >> 24);
      cbuffer[5] = (unsigned char) ((0xff0000 & buffer[1]) >> 16);
      cbuffer[6] = (unsigned char) ((0xff00 & buffer[1]) >> 8);
      cbuffer[7] = (unsigned char) (0xff & buffer[1]);

      cbuffer[8] = (unsigned char) ((0xff000000 & buffer[2]) >> 24);
      cbuffer[9] = (unsigned char) ((0xff0000 & buffer[2]) >> 16);
      cbuffer[10] = (unsigned char) ((0xff00 & buffer[2]) >> 8);
      cbuffer[11] = (unsigned char) (0xff & buffer[2]);

      cbuffer[12] = (unsigned char) ((0xff000000 & buffer[3]) >> 24);
      cbuffer[13] = (unsigned char) ((0xff0000 & buffer[3]) >> 16);
      cbuffer[14] = (unsigned char) ((0xff00 & buffer[3]) >> 8);
      cbuffer[15] = (unsigned char) (0xff & buffer[3]);

      cbuffer[16] = (unsigned char) ((0xff000000 & buffer[4]) >> 24);
      cbuffer[17] = (unsigned char) ((0xff0000 & buffer[4]) >> 16);
      cbuffer[18] = (unsigned char) ((0xff00 & buffer[4]) >> 8);
      cbuffer[19] = (unsigned char) (0xff & buffer[4]);

      cbuffer[20] = (unsigned char) ((0xff000000 & buffer[5]) >> 24);
      cbuffer[21] = (unsigned char) ((0xff0000 & buffer[5]) >> 16);
      cbuffer[22] = (unsigned char) ((0xff00 & buffer[5]) >> 8);
      cbuffer[23] = (unsigned char) (0xff & buffer[5]);

      cbuffer[24] = (unsigned char) ((0xff000000 & buffer[6]) >> 24);
      cbuffer[25] = (unsigned char) ((0xff0000 & buffer[6]) >> 16);
      cbuffer[26] = (unsigned char) ((0xff00 & buffer[6]) >> 8);
      cbuffer[27] = (unsigned char) (0xff & buffer[6]);

      cbuffer[28] = (unsigned char) ((0xff000000 & buffer[7]) >> 24);
      cbuffer[29] = (unsigned char) ((0xff0000 & buffer[7]) >> 16);
      cbuffer[30] = (unsigned char) ((0xff00 & buffer[7]) >> 8);
      cbuffer[31] = (unsigned char) (0xff & buffer[7]);
      
      buffer += 8;
      cbuffer += (4 * 8);
    }
  }

  for(; i < buffer_length; i++){
    cbuffer[0] = (unsigned char) ((0xff000000 & buffer[0]) >> 24);
    cbuffer[1] = (unsigned char) ((0xff0000 & buffer[0]) >> 16);
    cbuffer[2] = (unsigned char) ((0xff00 & buffer[0]) >> 8);
    cbuffer[3] = (unsigned char) (0xff & buffer[0]);

    buffer++;
    cbuffer += 4;
  }

  return(cbuffer);
}

/**
 * ags_buffer_util_s64_to_char_buffer:
 * @buffer: the signed long long buffer
 * @buffer_length: the buffer length
 * 
 * Pack @buffer into an unsigned char buffer.
 * 
 * Returns: the unsigned char buffer
 * 
 * Since: 2.0.0
 */
unsigned char*
ags_buffer_util_s64_to_char_buffer(signed long long *buffer,
				   guint buffer_length)
{
  unsigned char *cbuffer;

  guint limit;
  guint i;
  
  cbuffer = (unsigned char *) malloc((8 * buffer_length) * sizeof(unsigned char));

  i = 0;
  
  if(buffer_length > 8){
    limit = buffer_length - 8;

    for(; i < limit; i += 8){
      cbuffer[0] = (unsigned char) ((0xff00000000000000 & buffer[0]) >> 56);
      cbuffer[1] = (unsigned char) ((0xff000000000000 & buffer[0]) >> 48);
      cbuffer[2] = (unsigned char) ((0xff0000000000 & buffer[0]) >> 40);
      cbuffer[3] = (unsigned char) ((0xff00000000 & buffer[0]) >> 32);
      cbuffer[4] = (unsigned char) ((0xff000000 & buffer[0]) >> 24);
      cbuffer[5] = (unsigned char) ((0xff0000 & buffer[0]) >> 16);
      cbuffer[6] = (unsigned char) ((0xff00 & buffer[0]) >> 8);
      cbuffer[7] = (unsigned char) (0xff & buffer[0]);

      cbuffer[8] = (unsigned char) ((0xff00000000000000 & buffer[1]) >> 56);
      cbuffer[9] = (unsigned char) ((0xff000000000000 & buffer[1]) >> 48);
      cbuffer[10] = (unsigned char) ((0xff0000000000 & buffer[1]) >> 40);
      cbuffer[11] = (unsigned char) ((0xff00000000 & buffer[1]) >> 32);
      cbuffer[12] = (unsigned char) ((0xff000000 & buffer[1]) >> 24);
      cbuffer[13] = (unsigned char) ((0xff0000 & buffer[1]) >> 16);
      cbuffer[14] = (unsigned char) ((0xff00 & buffer[1]) >> 8);
      cbuffer[15] = (unsigned char) (0xff & buffer[1]);

      cbuffer[16] = (unsigned char) ((0xff00000000000000 & buffer[2]) >> 56);
      cbuffer[17] = (unsigned char) ((0xff000000000000 & buffer[2]) >> 48);
      cbuffer[18] = (unsigned char) ((0xff0000000000 & buffer[2]) >> 40);
      cbuffer[19] = (unsigned char) ((0xff00000000 & buffer[2]) >> 32);
      cbuffer[20] = (unsigned char) ((0xff000000 & buffer[2]) >> 24);
      cbuffer[21] = (unsigned char) ((0xff0000 & buffer[2]) >> 16);
      cbuffer[22] = (unsigned char) ((0xff00 & buffer[2]) >> 8);
      cbuffer[23] = (unsigned char) (0xff & buffer[2]);

      cbuffer[24] = (unsigned char) ((0xff00000000000000 & buffer[3]) >> 56);
      cbuffer[25] = (unsigned char) ((0xff000000000000 & buffer[3]) >> 48);
      cbuffer[26] = (unsigned char) ((0xff0000000000 & buffer[3]) >> 40);
      cbuffer[27] = (unsigned char) ((0xff00000000 & buffer[3]) >> 32);
      cbuffer[28] = (unsigned char) ((0xff000000 & buffer[3]) >> 24);
      cbuffer[29] = (unsigned char) ((0xff0000 & buffer[3]) >> 16);
      cbuffer[30] = (unsigned char) ((0xff00 & buffer[3]) >> 8);
      cbuffer[31] = (unsigned char) (0xff & buffer[3]);

      cbuffer[32] = (unsigned char) ((0xff00000000000000 & buffer[4]) >> 56);
      cbuffer[33] = (unsigned char) ((0xff000000000000 & buffer[4]) >> 48);
      cbuffer[34] = (unsigned char) ((0xff0000000000 & buffer[4]) >> 40);
      cbuffer[35] = (unsigned char) ((0xff00000000 & buffer[4]) >> 32);
      cbuffer[36] = (unsigned char) ((0xff000000 & buffer[4]) >> 24);
      cbuffer[37] = (unsigned char) ((0xff0000 & buffer[4]) >> 16);
      cbuffer[38] = (unsigned char) ((0xff00 & buffer[4]) >> 8);
      cbuffer[39] = (unsigned char) (0xff & buffer[4]);
      
      cbuffer[40] = (unsigned char) ((0xff00000000000000 & buffer[5]) >> 56);
      cbuffer[41] = (unsigned char) ((0xff000000000000 & buffer[5]) >> 48);
      cbuffer[42] = (unsigned char) ((0xff0000000000 & buffer[5]) >> 40);
      cbuffer[43] = (unsigned char) ((0xff00000000 & buffer[5]) >> 32);
      cbuffer[44] = (unsigned char) ((0xff000000 & buffer[5]) >> 24);
      cbuffer[45] = (unsigned char) ((0xff0000 & buffer[5]) >> 16);
      cbuffer[46] = (unsigned char) ((0xff00 & buffer[5]) >> 8);
      cbuffer[47] = (unsigned char) (0xff & buffer[5]);

      cbuffer[48] = (unsigned char) ((0xff00000000000000 & buffer[6]) >> 56);
      cbuffer[49] = (unsigned char) ((0xff000000000000 & buffer[6]) >> 48);
      cbuffer[50] = (unsigned char) ((0xff0000000000 & buffer[6]) >> 40);
      cbuffer[51] = (unsigned char) ((0xff00000000 & buffer[6]) >> 32);
      cbuffer[52] = (unsigned char) ((0xff000000 & buffer[6]) >> 24);
      cbuffer[53] = (unsigned char) ((0xff0000 & buffer[6]) >> 16);
      cbuffer[54] = (unsigned char) ((0xff00 & buffer[6]) >> 8);
      cbuffer[55] = (unsigned char) (0xff & buffer[6]);

      cbuffer[56] = (unsigned char) ((0xff00000000000000 & buffer[7]) >> 56);
      cbuffer[57] = (unsigned char) ((0xff000000000000 & buffer[7]) >> 48);
      cbuffer[58] = (unsigned char) ((0xff0000000000 & buffer[7]) >> 40);
      cbuffer[59] = (unsigned char) ((0xff00000000 & buffer[7]) >> 32);
      cbuffer[60] = (unsigned char) ((0xff000000 & buffer[7]) >> 24);
      cbuffer[61] = (unsigned char) ((0xff0000 & buffer[7]) >> 16);
      cbuffer[62] = (unsigned char) ((0xff00 & buffer[7]) >> 8);
      cbuffer[63] = (unsigned char) (0xff & buffer[7]);

      buffer += 8;
      cbuffer += (8 * 8);
    }
  }

  for(; i < buffer_length; i++){
    cbuffer[0] = (unsigned char) ((0xff00000000000000 & buffer[0]) >> 56);
    cbuffer[1] = (unsigned char) ((0xff000000000000 & buffer[0]) >> 48);
    cbuffer[2] = (unsigned char) ((0xff0000000000 & buffer[0]) >> 40);
    cbuffer[3] = (unsigned char) ((0xff00000000 & buffer[0]) >> 32);
    cbuffer[4] = (unsigned char) ((0xff000000 & buffer[0]) >> 24);
    cbuffer[5] = (unsigned char) ((0xff0000 & buffer[0]) >> 16);
    cbuffer[6] = (unsigned char) ((0xff00 & buffer[0]) >> 8);
    cbuffer[7] = (unsigned char) (0xff & buffer[0]);

    buffer++;
    cbuffer += 8;
  }

  return(cbuffer);
}

/**
 * ags_buffer_util_float_to_char_buffer:
 * @buffer: the float buffer
 * @buffer_length: the buffer length
 * 
 * Pack @buffer into an unsigned char buffer.
 * 
 * Returns: the unsigned char buffer
 * 
 * Since: 2.0.0
 */
unsigned char*
ags_buffer_util_float_to_char_buffer(float *buffer,
				     guint buffer_length)
{
  unsigned char *cbuffer;
  
  cbuffer = (unsigned char *) malloc((buffer_length * sizeof(float)) * sizeof(unsigned char));

  memcpy(cbuffer, buffer, buffer_length * sizeof(float));

  return(cbuffer);
}

/**
 * ags_buffer_util_double_to_char_buffer:
 * @buffer: the double buffer
 * @buffer_length: the buffer length
 * 
 * Pack @buffer into an unsigned char buffer.
 * 
 * Returns: the unsigned char buffer
 * 
 * Since: 2.0.0
 */
unsigned char*
ags_buffer_util_double_to_char_buffer(double *buffer,
				      guint buffer_length)
{
  unsigned char *cbuffer;
  
  cbuffer = (unsigned char *) malloc((buffer_length * sizeof(double)) * sizeof(unsigned char));

  memcpy(cbuffer, buffer, buffer_length * sizeof(double));

  return(cbuffer);
}

/**
 * ags_buffer_util_char_buffer_to_s8:
 * @cbuffer: the unsigned char buffer
 * @buffer_size: the buffer size
 *
 * Unpack @cbuffer to a signed char buffer
 *
 * Returns: the signed char buffer
 * 
 * Since: 2.0.0
 */
signed char*
ags_buffer_util_char_buffer_to_s8(unsigned char *cbuffer,
				  guint buffer_size)
{
  signed char *buffer;

  guint limit;
  guint i;

  buffer = (signed char *) malloc(buffer_size * sizeof(signed char));
  memset(buffer, 0, buffer_size * sizeof(signed char));
  
  i = 0;
  
  if(buffer_size > 8){
    limit = buffer_size - 8;

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

  return(buffer);
}

/**
 * ags_buffer_util_char_buffer_to_s16:
 * @cbuffer: the unsigned char buffer
 * @buffer_size: the buffer size
 *
 * Unpack @cbuffer to a signed short buffer
 *
 * Returns: the signed short buffer
 * 
 * Since: 2.0.0
 */
signed short*
ags_buffer_util_char_buffer_to_s16(unsigned char *cbuffer,
				   guint buffer_size)
{
  signed short *buffer;

  guint limit;
  guint i;

  buffer = (signed short *) malloc((buffer_size / 2) * sizeof(signed short));
  memset(buffer, 0, (buffer_size / 2) * sizeof(signed short));

  buffer_size = (guint) (2 * floor(buffer_size / 2.0));
  
  i = 0;
  
  if(buffer_size / 2 > 8){
    limit = (buffer_size / 2) - 8;

    for(; i < limit; i += 8){
      buffer[0] |= ((0xff & cbuffer[0]) << 8);
      buffer[0] |= (0xff & cbuffer[1]);
      
      buffer[1] |= ((0xff & cbuffer[2]) << 8);
      buffer[1] |= (0xff & cbuffer[3]);

      buffer[2] |= ((0xff & cbuffer[4]) << 8);
      buffer[2] |= (0xff & cbuffer[5]);

      buffer[3] |= ((0xff & cbuffer[6]) << 8);
      buffer[3] |= (0xff & cbuffer[7]);
      
      buffer[4] |= ((0xff & cbuffer[8]) << 8);
      buffer[4] |= (0xff & cbuffer[9]);
      
      buffer[5] |= ((0xff & cbuffer[10]) << 8);
      buffer[5] |= (0xff & cbuffer[11]);

      buffer[6] |= ((0xff & cbuffer[12]) << 8);
      buffer[6] |= (0xff & cbuffer[13]);

      buffer[7] |= ((0xff & cbuffer[14]) << 8);
      buffer[7] |= (0xff & cbuffer[15]);

      buffer += 8;
      cbuffer += (2 * 8);
    }
  }

  for(; i < buffer_size; i += 2){
    buffer[0] |= ((0xff & cbuffer[0]) << 8);
    buffer[0] |= (0xff & cbuffer[1]);
      
    buffer++;
    cbuffer += 2;
  }

  return(buffer);
}

/**
 * ags_buffer_util_char_buffer_to_s24:
 * @cbuffer: the unsigned char buffer
 * @buffer_size: the buffer size
 *
 * Unpack @cbuffer to a signed long buffer
 *
 * Returns: the signed long buffer
 * 
 * Since: 2.0.0
 */
signed long*
ags_buffer_util_char_buffer_to_s24(unsigned char *cbuffer,
				   guint buffer_size)
{
  signed long *buffer;

  guint limit;
  guint i;

  buffer = (signed long *) malloc((buffer_size / 4) * sizeof(signed long));
  memset(buffer, 0, (buffer_size / 4) * sizeof(signed long));

  buffer_size = (guint) (4 * floor(buffer_size / 4.0));
  
  i = 0;
  
  if(buffer_size / 4 > 8){
    limit = (buffer_size / 4) - 8;

    for(; i < limit; i += 8){
      buffer[0] |= ((0xff & cbuffer[0]) << 16);
      buffer[0] |= ((0xff & cbuffer[1]) << 8);
      buffer[0] |= (0xff & cbuffer[2]);
      
      buffer[1] |= ((0xff & cbuffer[4]) << 16);
      buffer[1] |= ((0xff & cbuffer[5]) << 8);
      buffer[1] |= (0xff & cbuffer[6]);

      buffer[2] |= ((0xff & cbuffer[8]) << 16);
      buffer[2] |= ((0xff & cbuffer[9]) << 8);
      buffer[2] |= (0xff & cbuffer[10]);
      
      buffer[3] |= ((0xff & cbuffer[12]) << 16);
      buffer[3] |= ((0xff & cbuffer[13]) << 8);
      buffer[3] |= (0xff & cbuffer[14]);
      
      buffer[4] |= ((0xff & cbuffer[16]) << 16);
      buffer[4] |= ((0xff & cbuffer[17]) << 8);
      buffer[4] |= (0xff & cbuffer[18]);
      
      buffer[5] |= ((0xff & cbuffer[20]) << 16);
      buffer[5] |= ((0xff & cbuffer[21]) << 8);
      buffer[5] |= (0xff & cbuffer[22]);
      
      buffer[6] |= ((0xff & cbuffer[24]) << 16);
      buffer[6] |= ((0xff & cbuffer[25]) << 8);
      buffer[6] |= (0xff & cbuffer[26]);
      
      buffer[7] |= ((0xff & cbuffer[28]) << 16);
      buffer[7] |= ((0xff & cbuffer[29]) << 8);
      buffer[7] |= (0xff & cbuffer[30]);
      
      buffer += 8;
      cbuffer += (4 * 8);
    }
  }

  for(; i < buffer_size; i += 4){
    buffer[0] |= ((0xff & cbuffer[0]) << 16);
    buffer[0] |= ((0xff & cbuffer[1]) << 8);
    buffer[0] |= (0xff & cbuffer[2]);
      
    buffer++;
    cbuffer += 4;
  }

  return(buffer);
}

/**
 * ags_buffer_util_char_buffer_to_s32:
 * @cbuffer: the unsigned char buffer
 * @buffer_size: the buffer size
 *
 * Unpack @cbuffer to a signed long buffer
 *
 * Returns: the signed long buffer
 * 
 * Since: 2.0.0
 */
signed long*
ags_buffer_util_char_buffer_to_s32(unsigned char *cbuffer,
				   guint buffer_size)
{
  signed long *buffer;

  guint limit;
  guint i;

  buffer = (signed long *) malloc((buffer_size / 4) * sizeof(signed long));
  memset(buffer, 0, (buffer_size / 4) * sizeof(signed long));

  buffer_size = (guint) (4 * floor(buffer_size / 4.0));
  
  i = 0;
  
  if((buffer_size / 4) > 8){
    limit = (buffer_size / 4) - 8;

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
  }

  for(; i < buffer_size; i += 4){
    buffer[0] |= ((0xff & cbuffer[0]) << 24);
    buffer[0] |= ((0xff & cbuffer[1]) << 16);
    buffer[0] |= ((0xff & cbuffer[2]) << 8);
    buffer[0] |= (0xff & cbuffer[3]);
      
    buffer++;
    cbuffer += 4;
  }

  return(buffer);
}

/**
 * ags_buffer_util_char_buffer_to_s64:
 * @cbuffer: the unsigned char buffer
 * @buffer_size: the buffer size
 *
 * Unpack @cbuffer to a signed long long buffer
 *
 * Returns: the signed long long buffer
 * 
 * Since: 2.0.0
 */
signed long long*
ags_buffer_util_char_buffer_to_s64(unsigned char *cbuffer,
				   guint buffer_size)
{
  signed long long *buffer;

  guint limit;
  guint i;

  buffer = (signed long long *) malloc((buffer_size / 8) * sizeof(signed long long));
  memset(buffer, 0, (buffer_size / 8) * sizeof(signed long long));

  buffer_size = (guint) (8 * floor(buffer_size / 8.0)); 
  
  i = 0;
  
  if(buffer_size / 8 > 8){
    limit = (buffer_size / 8) - 8;

    for(; i < limit; i += 8){
      buffer[0] |= ((0xff & cbuffer[0]) << 56);
      buffer[0] |= ((0xff & cbuffer[1]) << 48);
      buffer[0] |= ((0xff & cbuffer[2]) << 40);
      buffer[0] |= ((0xff & cbuffer[3]) << 32);
      buffer[0] |= ((0xff & cbuffer[4]) << 24);
      buffer[0] |= ((0xff & cbuffer[5]) << 16);
      buffer[0] |= ((0xff & cbuffer[6]) << 8);
      buffer[0] |= (0xff & cbuffer[7]);
      
      buffer[1] |= ((0xff & cbuffer[8]) << 56);
      buffer[1] |= ((0xff & cbuffer[9]) << 48);
      buffer[1] |= ((0xff & cbuffer[10]) << 40);
      buffer[1] |= ((0xff & cbuffer[11]) << 32);
      buffer[1] |= ((0xff & cbuffer[12]) << 24);
      buffer[1] |= ((0xff & cbuffer[13]) << 16);
      buffer[1] |= ((0xff & cbuffer[14]) << 8);
      buffer[1] |= (0xff & cbuffer[15]);
      
      buffer[2] |= ((0xff & cbuffer[16]) << 56);
      buffer[2] |= ((0xff & cbuffer[17]) << 48);
      buffer[2] |= ((0xff & cbuffer[18]) << 40);
      buffer[2] |= ((0xff & cbuffer[19]) << 32);
      buffer[2] |= ((0xff & cbuffer[20]) << 24);
      buffer[2] |= ((0xff & cbuffer[21]) << 16);
      buffer[2] |= ((0xff & cbuffer[22]) << 8);
      buffer[2] |= (0xff & cbuffer[23]);
      
      buffer[3] |= ((0xff & cbuffer[24]) << 56);
      buffer[3] |= ((0xff & cbuffer[25]) << 48);
      buffer[3] |= ((0xff & cbuffer[26]) << 40);
      buffer[3] |= ((0xff & cbuffer[27]) << 32);
      buffer[3] |= ((0xff & cbuffer[28]) << 24);
      buffer[3] |= ((0xff & cbuffer[29]) << 16);
      buffer[3] |= ((0xff & cbuffer[30]) << 8);
      buffer[3] |= (0xff & cbuffer[31]);
      
      buffer[4] |= ((0xff & cbuffer[32]) << 56);
      buffer[4] |= ((0xff & cbuffer[33]) << 48);
      buffer[4] |= ((0xff & cbuffer[34]) << 40);
      buffer[4] |= ((0xff & cbuffer[35]) << 32);
      buffer[4] |= ((0xff & cbuffer[36]) << 24);
      buffer[4] |= ((0xff & cbuffer[37]) << 16);
      buffer[4] |= ((0xff & cbuffer[38]) << 8);
      buffer[4] |= (0xff & cbuffer[39]);
      
      buffer[5] |= ((0xff & cbuffer[40]) << 56);
      buffer[5] |= ((0xff & cbuffer[41]) << 48);
      buffer[5] |= ((0xff & cbuffer[42]) << 40);
      buffer[5] |= ((0xff & cbuffer[43]) << 32);
      buffer[5] |= ((0xff & cbuffer[44]) << 24);
      buffer[5] |= ((0xff & cbuffer[45]) << 16);
      buffer[5] |= ((0xff & cbuffer[46]) << 8);
      buffer[5] |= (0xff & cbuffer[47]);
      
      buffer[6] |= ((0xff & cbuffer[48]) << 56);
      buffer[6] |= ((0xff & cbuffer[49]) << 48);
      buffer[6] |= ((0xff & cbuffer[50]) << 32);
      buffer[6] |= ((0xff & cbuffer[52]) << 24);
      buffer[6] |= ((0xff & cbuffer[51]) << 16);
      buffer[6] |= ((0xff & cbuffer[52]) << 40);
      buffer[6] |= ((0xff & cbuffer[53]) << 8);
      buffer[6] |= (0xff & cbuffer[54]);
      
      buffer[7] |= ((0xff & cbuffer[56]) << 56);
      buffer[7] |= ((0xff & cbuffer[57]) << 48);
      buffer[7] |= ((0xff & cbuffer[58]) << 40);
      buffer[7] |= ((0xff & cbuffer[59]) << 32);
      buffer[7] |= ((0xff & cbuffer[60]) << 24);
      buffer[7] |= ((0xff & cbuffer[61]) << 16);
      buffer[7] |= ((0xff & cbuffer[62]) << 8);
      buffer[7] |= (0xff & cbuffer[63]);
      
      buffer += 8;
      cbuffer += (8 * 8);
    }
  }

  for(; i < buffer_size; i += 8){
    buffer[0] |= ((0xff & cbuffer[0]) << 56);
    buffer[0] |= ((0xff & cbuffer[1]) << 48);
    buffer[0] |= ((0xff & cbuffer[2]) << 40);
    buffer[0] |= ((0xff & cbuffer[3]) << 32);
    buffer[0] |= ((0xff & cbuffer[4]) << 24);
    buffer[0] |= ((0xff & cbuffer[5]) << 16);
    buffer[0] |= ((0xff & cbuffer[6]) << 8);
    buffer[0] |= (0xff & cbuffer[7]);
    
    buffer++;
    cbuffer += 8;
  }

  return(buffer);
}

/**
 * ags_buffer_util_char_buffer_to_float:
 * @cbuffer: the unsigned char buffer
 * @buffer_size: the buffer size
 *
 * Unpack @cbuffer to a float buffer
 *
 * Returns: the float buffer
 * 
 * Since: 2.0.0
 */
float*
ags_buffer_util_char_buffer_to_float(unsigned char *cbuffer,
				     guint buffer_size)
{
  float *buffer;
  
  buffer = (signed long long *) malloc((buffer_size / sizeof(float)) * sizeof(float));

  buffer_size = (guint) (sizeof(float) * floor(buffer_size / sizeof(float))); 

  memcpy(buffer, cbuffer, buffer_size * sizeof(unsigned char));

  return(buffer);
}

/**
 * ags_buffer_util_char_buffer_to_double:
 * @cbuffer: the unsigned char buffer
 * @buffer_size: the buffer size
 *
 * Unpack @cbuffer to a double buffer
 *
 * Returns: the double buffer
 * 
 * Since: 2.0.0
 */
double*
ags_buffer_util_char_buffer_to_double(unsigned char *cbuffer,
				      guint buffer_size)
{
  double *buffer;
  
  buffer = (signed long long *) malloc((buffer_size / sizeof(double)) * sizeof(double));

  buffer_size = (guint) (sizeof(double) * floor(buffer_size / sizeof(double))); 

  memcpy(buffer, cbuffer, buffer_size * sizeof(unsigned char));

  return(buffer);
}

/**
 * ags_buffer_util_char_buffer_read_s8:
 * @cbuffer: the character buffer
 * @swap_bytes: reverse order, ignored here
 * 
 * Read a signed char quantity of @cbuffer.
 * 
 * Returns: the signed char value
 * 
 * Since: 2.0.0
 */
signed char
ags_buffer_util_char_buffer_read_s8(unsigned char *cbuffer,
				    gboolean swap_bytes)
{
  signed char val;

  val = 0;
  val |= cbuffer[0];

  return(val);
}

/**
 * ags_buffer_util_char_buffer_read_s16:
 * @cbuffer: the character buffer
 * @swap_bytes: reverse order
 * 
 * Read a signed short quantity of @cbuffer.
 * 
 * Returns: the signed short value
 * 
 * Since: 2.0.0
 */
signed short
ags_buffer_util_char_buffer_read_s16(unsigned char *cbuffer,
				     gboolean swap_bytes)
{
  signed short val;

  val = 0;
  
  if(swap_bytes){
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
 * @swap_bytes: reverse order
 * 
 * Read a signed long 24 bit quantity of @cbuffer.
 * 
 * Returns: the signed long value
 * 
 * Since: 2.0.0
 */
signed long
ags_buffer_util_char_buffer_read_s24(unsigned char *cbuffer,
				     gboolean swap_bytes)
{
  signed long val;

  val = 0;
  
  if(swap_bytes){
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
 * @swap_bytes: reverse order
 * 
 * Read a signed long 32 bit quantity of @cbuffer.
 * 
 * Returns: the signed long value
 * 
 * Since: 2.0.0
 */
signed long
ags_buffer_util_char_buffer_read_s32(unsigned char *cbuffer,
				     gboolean swap_bytes)
{
  signed long val;

  val = 0;
  
  if(swap_bytes){
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
 * @swap_bytes: reverse order
 * 
 * Read a signed long long 64 bit quantity of @cbuffer.
 * 
 * Returns: the signed long long value
 * 
 * Since: 2.0.0
 */
signed long long
ags_buffer_util_char_buffer_read_s64(unsigned char *cbuffer,
				     gboolean swap_bytes)
{
  signed long long val;

  val = 0;
  
  if(swap_bytes){
    val |= (cbuffer[0]);
    val |= (cbuffer[1] << 8);
    val |= (cbuffer[2] << 16);
    val |= (cbuffer[3] << 24);
    val |= (cbuffer[4] << 32);
    val |= (cbuffer[5] << 40);
    val |= (cbuffer[6] << 48);
    val |= (cbuffer[7] << 56);
  }else{
    val |= (cbuffer[0] << 56);
    val |= (cbuffer[1] << 48);
    val |= (cbuffer[2] << 40);
    val |= (cbuffer[3] << 32);
    val |= (cbuffer[4] << 24);
    val |= (cbuffer[5] << 16);
    val |= (cbuffer[6] << 8);
    val |= (cbuffer[7]);
  }

  return(val);
}

/**
 * ags_buffer_util_char_buffer_read_float:
 * @cbuffer: the character buffer
 * @swap_bytes: reverse order
 * 
 * Read a float quantity of @cbuffer.
 * 
 * Returns: the float value
 * 
 * Since: 2.0.0
 */
float
ags_buffer_util_char_buffer_read_float(unsigned char *cbuffer,
				       gboolean swap_bytes)
{
  float val;

  val = 0.0;
  
  if(swap_bytes){
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
 * ags_buffer_util_char_buffer_read_double:
 * @cbuffer: the character buffer
 * @swap_bytes: reverse order
 * 
 * Read a double quantity of @cbuffer.
 * 
 * Returns: the double value
 * 
 * Since: 2.0.0
 */
double
ags_buffer_util_char_buffer_read_double(unsigned char *cbuffer,
					gboolean swap_bytes)
{
  double val;

  val = 0.0;
  
  if(swap_bytes){
    val |= (cbuffer[0]);
    val |= (cbuffer[1] << 8);
    val |= (cbuffer[2] << 16);
    val |= (cbuffer[3] << 24);
    val |= (cbuffer[4] << 32);
    val |= (cbuffer[5] << 40);
    val |= (cbuffer[6] << 48);
    val |= (cbuffer[7] << 56);
  }else{
    val |= (cbuffer[0] << 56);
    val |= (cbuffer[1] << 48);
    val |= (cbuffer[2] << 40);
    val |= (cbuffer[3] << 32);
    val |= (cbuffer[4] << 24);
    val |= (cbuffer[5] << 16);
    val |= (cbuffer[6] << 8);
    val |= (cbuffer[7]);
  }

  return(val);
}

/**
 * ags_buffer_util_char_buffer_write_s8:
 * @cbuffer: the character buffer
 * @value: the signed char value
 * @swap_bytes: reverse order
 * 
 * Write a signed char quantity to @cbuffer.
 * 
 * Since: 2.0.0
 */
void
ags_buffer_util_char_buffer_write_s8(unsigned char *cbuffer,
				     signed char value,
				     gboolean swap_bytes)
{
  cbuffer[0] = value;
}

/**
 * ags_buffer_util_char_buffer_write_s16:
 * @cbuffer: the character buffer
 * @value: the signed short value
 * @swap_bytes: reverse order
 * 
 * Write a signed short quantity to @cbuffer.
 * 
 * Since: 2.0.0
 */
void
ags_buffer_util_char_buffer_write_s16(unsigned char *cbuffer,
				      signed short value,
				      gboolean swap_bytes)
{
  if(swap_bytes){
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
 * @value: the signed long value
 * @swap_bytes: reverse order
 * 
 * Write a signed long quantity to @cbuffer.
 * 
 * Since: 2.0.0
 */
void
ags_buffer_util_char_buffer_write_s24(unsigned char *cbuffer,
				      signed long value,
				      gboolean swap_bytes)
{
  if(swap_bytes){
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
 * @value: the signed long value
 * @swap_bytes: reverse order
 * 
 * Write a signed long quantity to @cbuffer.
 * 
 * Since: 2.0.0
 */
void
ags_buffer_util_char_buffer_write_s32(unsigned char *cbuffer,
				      signed long value,
				      gboolean swap_bytes)
{
  if(swap_bytes){
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
 * @value: the signed long long value
 * @swap_bytes: reverse order
 * 
 * Write a signed long long quantity to @cbuffer.
 * 
 * Since: 2.0.0
 */
void
ags_buffer_util_char_buffer_write_s64(unsigned char *cbuffer,
				      signed long long value,
				      gboolean swap_bytes)
{
  if(swap_bytes){
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
    cbuffer[3] = (0xff00000000 & value) >> 32
    cbuffer[4] = (0xff000000 & value) >> 24;
    cbuffer[5] = (0xff0000 & value) >> 16;
    cbuffer[6] = (0xff00 & value) >> 8;
    cbuffer[7] = (0xff & value);
  }
}

/**
 * ags_buffer_util_char_buffer_write_float:
 * @cbuffer: the character buffer
 * @value: the float value
 * @swap_bytes: reverse order
 * 
 * Write a float quantity to @cbuffer.
 * 
 * Since: 2.0.0
 */
void
ags_buffer_util_char_buffer_write_float(unsigned char *cbuffer,
					float value,
					gboolean swap_bytes)
{
  if(swap_bytes){
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
 * ags_buffer_util_char_buffer_write_double:
 * @cbuffer: the character buffer
 * @value: the double value
 * @swap_bytes: reverse order
 * 
 * Write a double quantity to @cbuffer.
 * 
 * Since: 2.0.0
 */
void
ags_buffer_util_char_buffer_write_double(unsigned char *cbuffer,
					 double value,
					 gboolean swap_bytes)
{
  if(swap_bytes){
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
    cbuffer[3] = (0xff00000000 & value) >> 32
    cbuffer[4] = (0xff000000 & value) >> 24;
    cbuffer[5] = (0xff0000 & value) >> 16;
    cbuffer[6] = (0xff00 & value) >> 8;
    cbuffer[7] = (0xff & value);
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
 * Since: 2.0.0
 */
void
ags_buffer_util_char_buffer_swap_bytes(unsigned char *cbuffer, guint word_size,
				       guint buffer_size)
{
  unsigned char tmp_buffer[8];
    
  guint i, j;

  if(word_size <= 1){
    return;
  }

  buffer_size = (guint) (word_size * floor(buffer_size / word_size));
  
  for(0; i < buffer_size; i += word_size){
    for(j = 0; j < word_size; j++){
      tmp_buffer[j] = cbuffer[i + (word_size - j - 1)];
    }

    for(j = 0; j < word_size; j++){
      cbuffer[i + j] = tmp_buffer[j];
    }    
  }
}
