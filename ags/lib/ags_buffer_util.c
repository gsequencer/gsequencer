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

#include <stdlib.h>

/**
 * ags_buffer_util_s8_to_char_buffer:
 * @buffer: the signed char buffer
 * @buffer_length: the buffer length
 * 
 * Pack @buffer into an unsigned char buffer.
 * 
 * Returns: the unsigned char buffer
 * 
 * Since: 1.4.0
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
 * Since: 1.4.0
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
      cbuffer[0] = (unsigned char) (0xff & buffer[0]);
      cbuffer[1] = (unsigned char) ((0xff00 & buffer[0]) >> 8);

      cbuffer[2] = (unsigned char) (0xff & buffer[1]);
      cbuffer[3] = (unsigned char) ((0xff00 & buffer[1]) >> 8);

      cbuffer[4] = (unsigned char) (0xff & buffer[2]);
      cbuffer[5] = (unsigned char) ((0xff00 & buffer[2]) >> 8);

      cbuffer[6] = (unsigned char) (0xff & buffer[3]);
      cbuffer[7] = (unsigned char) ((0xff00 & buffer[3]) >> 8);

      cbuffer[8] = (unsigned char) (0xff & buffer[4]);
      cbuffer[9] = (unsigned char) ((0xff00 & buffer[4]) >> 8);
      
      cbuffer[10] = (unsigned char) (0xff & buffer[5]);
      cbuffer[11] = (unsigned char) ((0xff00 & buffer[5]) >> 8);

      cbuffer[12 = (unsigned char) (0xff & buffer[6]);
      cbuffer[13] = (unsigned char) ((0xff00 & buffer[6]) >> 8);

      cbuffer[14] = (unsigned char) (0xff & buffer[7]);
      cbuffer[15] = (unsigned char) ((0xff00 & buffer[7]) >> 8);
      
      buffer += 8;
      cbuffer += (2 * 8);
    }
  }

  for(; i < buffer_length; i++){
    cbuffer[0] = (unsigned char) (0xff & buffer[0]);
    cbuffer[1] = (unsigned char) ((0xff00 & buffer[0]) >> 8);

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
 * Since: 1.4.0
 */
unsigned char*
ags_buffer_util_s24_to_char_buffer(signed long *buffer,
				   guint buffer_length)
{
  unsigned char *cbuffer;

  guint limit;
  guint i;
  
  cbuffer = (unsigned char *) malloc((3 * buffer_length) * sizeof(unsigned char));

  i = 0;
  
  if(buffer_length > 8){
    limit = buffer_length - 8;

    for(; i < limit; i += 8){
      cbuffer[0] = (unsigned char) (0xff & buffer[0]);
      cbuffer[1] = (unsigned char) ((0xff00 & buffer[0]) >> 8);
      cbuffer[2] = (unsigned char) ((0xff0000 & buffer[0]) >> 16);

      cbuffer[3] = (unsigned char) (0xff & buffer[1]);
      cbuffer[4] = (unsigned char) ((0xff00 & buffer[1]) >> 8);
      cbuffer[5] = (unsigned char) ((0xff0000 & buffer[1]) >> 16);

      cbuffer[6] = (unsigned char) (0xff & buffer[2]);
      cbuffer[7] = (unsigned char) ((0xff00 & buffer[2]) >> 8);
      cbuffer[8] = (unsigned char) ((0xff0000 & buffer[2]) >> 16);

      cbuffer[9] = (unsigned char) (0xff & buffer[3]);
      cbuffer[10] = (unsigned char) ((0xff00 & buffer[3]) >> 8);
      cbuffer[11] = (unsigned char) ((0xff0000 & buffer[3]) >> 16);

      cbuffer[12] = (unsigned char) (0xff & buffer[4]);
      cbuffer[13] = (unsigned char) ((0xff00 & buffer[4]) >> 8);
      cbuffer[14] = (unsigned char) ((0xff0000 & buffer[4]) >> 16);
      
      cbuffer[15] = (unsigned char) (0xff & buffer[5]);
      cbuffer[16] = (unsigned char) ((0xff00 & buffer[5]) >> 8);
      cbuffer[17] = (unsigned char) ((0xff0000 & buffer[5]) >> 16);

      cbuffer[18] = (unsigned char) (0xff & buffer[6]);
      cbuffer[19] = (unsigned char) ((0xff00 & buffer[6]) >> 8);
      cbuffer[20] = (unsigned char) ((0xff0000 & buffer[6]) >> 16);

      cbuffer[21] = (unsigned char) (0xff & buffer[7]);
      cbuffer[22] = (unsigned char) ((0xff00 & buffer[7]) >> 8);
      cbuffer[23] = (unsigned char) ((0xff0000 & buffer[7]) >> 16);
      
      buffer += 8;
      cbuffer += (3 * 8);
    }
  }

  for(; i < buffer_length; i++){
    cbuffer[0] = (unsigned char) (0xff & buffer[0]);
    cbuffer[1] = (unsigned char) ((0xff00 & buffer[0]) >> 8);
    cbuffer[2] = (unsigned char) ((0xff0000 & buffer[0]) >> 16);

    buffer++;
    cbuffer += 3;
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
 * Since: 1.4.0
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
      cbuffer[0] = (unsigned char) (0xff & buffer[0]);
      cbuffer[1] = (unsigned char) ((0xff00 & buffer[0]) >> 8);
      cbuffer[2] = (unsigned char) ((0xff0000 & buffer[0]) >> 16);
      cbuffer[3] = (unsigned char) ((0xff000000 & buffer[0]) >> 24);
      
      cbuffer[4] = (unsigned char) (0xff & buffer[1]);
      cbuffer[5] = (unsigned char) ((0xff00 & buffer[1]) >> 8);
      cbuffer[6] = (unsigned char) ((0xff0000 & buffer[1]) >> 16);
      cbuffer[7] = (unsigned char) ((0xff000000 & buffer[1]) >> 24);

      cbuffer[8] = (unsigned char) (0xff & buffer[2]);
      cbuffer[9] = (unsigned char) ((0xff00 & buffer[2]) >> 8);
      cbuffer[10] = (unsigned char) ((0xff0000 & buffer[2]) >> 16);
      cbuffer[11] = (unsigned char) ((0xff000000 & buffer[2]) >> 24);

      cbuffer[12] = (unsigned char) (0xff & buffer[3]);
      cbuffer[13] = (unsigned char) ((0xff00 & buffer[3]) >> 8);
      cbuffer[14] = (unsigned char) ((0xff0000 & buffer[3]) >> 16);
      cbuffer[15] = (unsigned char) ((0xff000000 & buffer[3]) >> 24);

      cbuffer[16] = (unsigned char) (0xff & buffer[4]);
      cbuffer[17] = (unsigned char) ((0xff00 & buffer[4]) >> 8);
      cbuffer[18] = (unsigned char) ((0xff0000 & buffer[4]) >> 16);
      cbuffer[19] = (unsigned char) ((0xff000000 & buffer[4]) >> 24);

      cbuffer[20] = (unsigned char) (0xff & buffer[5]);
      cbuffer[21] = (unsigned char) ((0xff00 & buffer[5]) >> 8);
      cbuffer[22] = (unsigned char) ((0xff0000 & buffer[5]) >> 16);
      cbuffer[23] = (unsigned char) ((0xff000000 & buffer[5]) >> 24);

      cbuffer[24] = (unsigned char) (0xff & buffer[6]);
      cbuffer[25] = (unsigned char) ((0xff00 & buffer[6]) >> 8);
      cbuffer[26] = (unsigned char) ((0xff0000 & buffer[6]) >> 16);
      cbuffer[27] = (unsigned char) ((0xff000000 & buffer[6]) >> 24);

      cbuffer[28] = (unsigned char) (0xff & buffer[7]);
      cbuffer[29] = (unsigned char) ((0xff00 & buffer[7]) >> 8);
      cbuffer[30] = (unsigned char) ((0xff0000 & buffer[7]) >> 16);
      cbuffer[31] = (unsigned char) ((0xff000000 & buffer[7]) >> 24);
      
      buffer += 8;
      cbuffer += (4 * 8);
    }
  }

  for(; i < buffer_length; i++){
    cbuffer[0] = (unsigned char) (0xff & buffer[0]);
    cbuffer[1] = (unsigned char) ((0xff00 & buffer[0]) >> 8);
    cbuffer[2] = (unsigned char) ((0xff0000 & buffer[0]) >> 16);
    cbuffer[3] = (unsigned char) ((0xff000000 & buffer[0]) >> 24);

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
 * Since: 1.4.0
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
      cbuffer[0] = (unsigned char) (0xff & buffer[0]);
      cbuffer[1] = (unsigned char) ((0xff00 & buffer[0]) >> 8);
      cbuffer[2] = (unsigned char) ((0xff0000 & buffer[0]) >> 16);
      cbuffer[3] = (unsigned char) ((0xff000000 & buffer[0]) >> 24);
      cbuffer[4] = (unsigned char) ((0xff00000000 & buffer[0]) >> 32);
      cbuffer[5] = (unsigned char) ((0xff0000000000 & buffer[0]) >> 40);
      cbuffer[6] = (unsigned char) ((0xff000000000000 & buffer[0]) >> 48);
      cbuffer[7] = (unsigned char) ((0xff00000000000000 & buffer[0]) >> 56);

      cbuffer[8] = (unsigned char) (0xff & buffer[1]);
      cbuffer[9] = (unsigned char) ((0xff00 & buffer[1]) >> 8);
      cbuffer[10] = (unsigned char) ((0xff0000 & buffer[1]) >> 16);
      cbuffer[11] = (unsigned char) ((0xff000000 & buffer[1]) >> 24);
      cbuffer[12] = (unsigned char) ((0xff00000000 & buffer[1]) >> 32);
      cbuffer[13] = (unsigned char) ((0xff0000000000 & buffer[1]) >> 40);
      cbuffer[14] = (unsigned char) ((0xff000000000000 & buffer[1]) >> 48);
      cbuffer[15] = (unsigned char) ((0xff00000000000000 & buffer[1]) >> 56);

      cbuffer[16] = (unsigned char) (0xff & buffer[2]);
      cbuffer[17] = (unsigned char) ((0xff00 & buffer[2]) >> 8);
      cbuffer[18] = (unsigned char) ((0xff0000 & buffer[2]) >> 16);
      cbuffer[19] = (unsigned char) ((0xff000000 & buffer[2]) >> 24);
      cbuffer[20] = (unsigned char) ((0xff00000000 & buffer[2]) >> 32);
      cbuffer[21] = (unsigned char) ((0xff0000000000 & buffer[2]) >> 40);
      cbuffer[22] = (unsigned char) ((0xff000000000000 & buffer[2]) >> 48);
      cbuffer[23] = (unsigned char) ((0xff00000000000000 & buffer[2]) >> 56);

      cbuffer[24] = (unsigned char) (0xff & buffer[3]);
      cbuffer[25] = (unsigned char) ((0xff00 & buffer[3]) >> 8);
      cbuffer[26] = (unsigned char) ((0xff0000 & buffer[3]) >> 16);
      cbuffer[27] = (unsigned char) ((0xff000000 & buffer[3]) >> 24);
      cbuffer[28] = (unsigned char) ((0xff00000000 & buffer[3]) >> 32);
      cbuffer[29] = (unsigned char) ((0xff0000000000 & buffer[3]) >> 40);
      cbuffer[30] = (unsigned char) ((0xff000000000000 & buffer[3]) >> 48);
      cbuffer[31] = (unsigned char) ((0xff00000000000000 & buffer[3]) >> 56);

      cbuffer[32] = (unsigned char) (0xff & buffer[4]);
      cbuffer[33] = (unsigned char) ((0xff00 & buffer[4]) >> 8);
      cbuffer[34] = (unsigned char) ((0xff0000 & buffer[4]) >> 16);
      cbuffer[35] = (unsigned char) ((0xff000000 & buffer[4]) >> 24);
      cbuffer[36] = (unsigned char) ((0xff00000000 & buffer[4]) >> 32);
      cbuffer[37] = (unsigned char) ((0xff0000000000 & buffer[4]) >> 40);
      cbuffer[38] = (unsigned char) ((0xff000000000000 & buffer[4]) >> 48);
      cbuffer[39] = (unsigned char) ((0xff00000000000000 & buffer[4]) >> 56);
      
      cbuffer[40] = (unsigned char) (0xff & buffer[5]);
      cbuffer[41] = (unsigned char) ((0xff00 & buffer[5]) >> 8);
      cbuffer[42] = (unsigned char) ((0xff0000 & buffer[5]) >> 16);
      cbuffer[43] = (unsigned char) ((0xff000000 & buffer[5]) >> 24);
      cbuffer[44] = (unsigned char) ((0xff00000000 & buffer[5]) >> 32);
      cbuffer[45] = (unsigned char) ((0xff0000000000 & buffer[5]) >> 40);
      cbuffer[46] = (unsigned char) ((0xff000000000000 & buffer[5]) >> 48);
      cbuffer[47] = (unsigned char) ((0xff00000000000000 & buffer[5]) >> 56);

      cbuffer[48] = (unsigned char) (0xff & buffer[6]);
      cbuffer[49] = (unsigned char) ((0xff00 & buffer[6]) >> 8);
      cbuffer[50] = (unsigned char) ((0xff0000 & buffer[6]) >> 16);
      cbuffer[51] = (unsigned char) ((0xff000000 & buffer[6]) >> 24);
      cbuffer[52] = (unsigned char) ((0xff00000000 & buffer[6]) >> 32);
      cbuffer[53] = (unsigned char) ((0xff0000000000 & buffer[6]) >> 40);
      cbuffer[54] = (unsigned char) ((0xff000000000000 & buffer[6]) >> 48);
      cbuffer[55] = (unsigned char) ((0xff00000000000000 & buffer[6]) >> 56);

      cbuffer[56] = (unsigned char) (0xff & buffer[7]);
      cbuffer[57] = (unsigned char) ((0xff00 & buffer[7]) >> 8);
      cbuffer[58] = (unsigned char) ((0xff0000 & buffer[7]) >> 16);
      cbuffer[59] = (unsigned char) ((0xff000000 & buffer[7]) >> 24);
      cbuffer[60] = (unsigned char) ((0xff00000000 & buffer[7]) >> 32);
      cbuffer[61] = (unsigned char) ((0xff0000000000 & buffer[7]) >> 40);
      cbuffer[62] = (unsigned char) ((0xff000000000000 & buffer[7]) >> 48);
      cbuffer[63] = (unsigned char) ((0xff00000000000000 & buffer[7]) >> 56);

      buffer += 8;
      cbuffer += (8 * 8);
    }
  }

  for(; i < buffer_length; i++){
    cbuffer[0] = (unsigned char) (0xff & buffer[0]);
    cbuffer[1] = (unsigned char) ((0xff00 & buffer[0]) >> 8);
    cbuffer[2] = (unsigned char) ((0xff0000 & buffer[0]) >> 16);
    cbuffer[3] = (unsigned char) ((0xff000000 & buffer[0]) >> 24);
    cbuffer[4] = (unsigned char) ((0xff00000000 & buffer[0]) >> 32);
    cbuffer[5] = (unsigned char) ((0xff0000000000 & buffer[0]) >> 40);
    cbuffer[6] = (unsigned char) ((0xff000000000000 & buffer[0]) >> 48);
    cbuffer[7] = (unsigned char) ((0xff00000000000000 & buffer[0]) >> 56);

    buffer++;
    cbuffer += 8;
  }

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
 * Since: 1.4.0
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
  
  if(buffer_length > 8){
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

  for(; i < buffer_length; i++){
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
 * Since: 1.4.0
 */
signed short*
ags_buffer_util_char_buffer_to_s16(unsigned char *cbuffer,
				   guint buffer_size)
{
  signed short *buffer;

  guint limit;
  guint i;

  buffer = (signed short *) malloc(buffer_size * sizeof(signed short));
  memset(buffer, 0, (buffer_size / 2) * sizeof(signed short));
  
  i = 0;
  
  if(buffer_length > 8){
    limit = (buffer_size / 2) - 8;

    for(; i < limit; i += 8){
      buffer[0] |= (0xff & cbuffer[0]);
      buffer[0] |= ((0xff & cbuffer[1]) << 8);
      
      buffer[1] |= (0xff & cbuffer[2]);
      buffer[1] |= ((0xff & cbuffer[3]) << 8);

      buffer[2] |= (0xff & cbuffer[4]);
      buffer[2] |= ((0xff & cbuffer[5]) << 8);

      buffer[3] |= (0xff & cbuffer[6]);
      buffer[3] |= ((0xff & cbuffer[7]) << 8);
      
      buffer[4] |= (0xff & cbuffer[8]);
      buffer[4] |= ((0xff & cbuffer[9]) << 8);
      
      buffer[5] |= (0xff & cbuffer[10]);
      buffer[5] |= ((0xff & cbuffer[11]) << 8);

      buffer[6] |= (0xff & cbuffer[12]);
      buffer[6] |= ((0xff & cbuffer[13]) << 8);

      buffer[7] |= (0xff & cbuffer[14]);
      buffer[7] |= ((0xff & cbuffer[15]) << 8);

      buffer += 8;
      cbuffer += (2 * 8);
    }
  }

  for(; i < buffer_size; i += 2){
    buffer[0] |= (0xff & cbuffer[0]);
    buffer[0] |= ((0xff & cbuffer[1]) << 8);
      
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
 * Since: 1.4.0
 */
signed long*
ags_buffer_util_char_buffer_to_s24(unsigned char *cbuffer,
				   guint buffer_size)
{
  signed long *buffer;

  guint limit;
  guint i;

  buffer = (signed long *) malloc(buffer_size * sizeof(signed long));
  memset(buffer, 0, (buffer_size / 3) * sizeof(signed long));
  
  i = 0;
  
  if(buffer_length > 8){
    limit = (buffer_size / 3) - 8;

    for(; i < limit; i += 8){
      buffer[0] |= (0xff & cbuffer[0]);
      buffer[0] |= ((0xff & cbuffer[1]) << 8);
      buffer[0] |= ((0xff & cbuffer[2]) << 16);
      
      buffer[1] |= (0xff & cbuffer[3]);
      buffer[1] |= ((0xff & cbuffer[4]) << 8);
      buffer[1] |= ((0xff & cbuffer[5]) << 16);

      buffer[2] |= (0xff & cbuffer[6]);
      buffer[2] |= ((0xff & cbuffer[7]) << 8);
      buffer[2] |= ((0xff & cbuffer[8]) << 16);
      
      buffer[3] |= (0xff & cbuffer[9]);
      buffer[3] |= ((0xff & cbuffer[10]) << 8);
      buffer[3] |= ((0xff & cbuffer[11]) << 16);
      
      buffer[4] |= (0xff & cbuffer[12]);
      buffer[4] |= ((0xff & cbuffer[13]) << 8);
      buffer[4] |= ((0xff & cbuffer[14]) << 16);
      
      buffer[5] |= (0xff & cbuffer[15]);
      buffer[5] |= ((0xff & cbuffer[16]) << 8);
      buffer[5] |= ((0xff & cbuffer[17]) << 16);
      
      buffer[6] |= (0xff & cbuffer[18]);
      buffer[6] |= ((0xff & cbuffer[19]) << 8);
      buffer[6] |= ((0xff & cbuffer[20]) << 16);
      
      buffer[7] |= (0xff & cbuffer[21]);
      buffer[7] |= ((0xff & cbuffer[22]) << 8);
      buffer[7] |= ((0xff & cbuffer[23]) << 16);
      
      buffer += 8;
      cbuffer += (3 * 8);
    }
  }

  for(; i < buffer_size; i += 3){
    buffer[0] |= (0xff & cbuffer[0]);
    buffer[0] |= ((0xff & cbuffer[1]) << 8);
    buffer[0] |= ((0xff & cbuffer[2]) << 16);
      
    buffer++;
    cbuffer += 3;
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
 * Since: 1.4.0
 */
signed long*
ags_buffer_util_char_buffer_to_s32(unsigned char *cbuffer,
				   guint buffer_size)
{
  signed long *buffer;

  guint limit;
  guint i;

  buffer = (signed long *) malloc(buffer_size * sizeof(signed long));
  memset(buffer, 0, (buffer_size / 4) * sizeof(signed long));
  
  i = 0;
  
  if(buffer_length > 8){
    limit = (buffer_size / 4) - 8;

    for(; i < limit; i += 8){
      buffer[0] |= (0xff & cbuffer[0]);
      buffer[0] |= ((0xff & cbuffer[1]) << 8);
      buffer[0] |= ((0xff & cbuffer[2]) << 16);
      buffer[0] |= ((0xff & cbuffer[3]) << 24);
      
      buffer[1] |= (0xff & cbuffer[4]);
      buffer[1] |= ((0xff & cbuffer[5]) << 8);
      buffer[1] |= ((0xff & cbuffer[6]) << 16);
      buffer[1] |= ((0xff & cbuffer[7]) << 24);
      
      buffer[2] |= (0xff & cbuffer[8]);
      buffer[2] |= ((0xff & cbuffer[9]) << 8);
      buffer[2] |= ((0xff & cbuffer[10]) << 16);
      buffer[2] |= ((0xff & cbuffer[11]) << 24);

      buffer[3] |= (0xff & cbuffer[12]);
      buffer[3] |= ((0xff & cbuffer[13]) << 8);
      buffer[3] |= ((0xff & cbuffer[14]) << 16);
      buffer[3] |= ((0xff & cbuffer[15]) << 24);

      buffer[4] |= (0xff & cbuffer[16]);
      buffer[4] |= ((0xff & cbuffer[17]) << 8);
      buffer[4] |= ((0xff & cbuffer[18]) << 16);
      buffer[4] |= ((0xff & cbuffer[19]) << 24);

      buffer[5] |= (0xff & cbuffer[20]);
      buffer[5] |= ((0xff & cbuffer[21]) << 8);
      buffer[5] |= ((0xff & cbuffer[22]) << 16);
      buffer[5] |= ((0xff & cbuffer[23]) << 24);

      buffer[6] |= (0xff & cbuffer[24]);
      buffer[6] |= ((0xff & cbuffer[25]) << 8);
      buffer[6] |= ((0xff & cbuffer[26]) << 16);
      buffer[6] |= ((0xff & cbuffer[27]) << 24);

      buffer[7] |= (0xff & cbuffer[28]);
      buffer[7] |= ((0xff & cbuffer[29]) << 8);
      buffer[7] |= ((0xff & cbuffer[30]) << 16);
      buffer[7] |= ((0xff & cbuffer[31]) << 24);

      buffer += 8;
      cbuffer += (4 * 8);
    }
  }

  for(; i < buffer_size; i += 4){
    buffer[0] |= (0xff & cbuffer[0]);
    buffer[0] |= ((0xff & cbuffer[1]) << 8);
    buffer[0] |= ((0xff & cbuffer[2]) << 16);
    buffer[0] |= ((0xff & cbuffer[3]) << 24);
      
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
 * Since: 1.4.0
 */
signed long long*
ags_buffer_util_char_buffer_to_s64(unsigned char *cbuffer,
				   guint buffer_size)
{
  signed long long *buffer;

  guint limit;
  guint i;

  buffer = (signed long long *) malloc(buffer_size * sizeof(signed long long));
  memset(buffer, 0, (buffer_size / 8) * sizeof(signed long long));
  
  i = 0;
  
  if(buffer_length > 8){
    limit = (buffer_size / 8) - 8;

    for(; i < limit; i += 8){
      buffer[0] |= (0xff & cbuffer[0]);
      buffer[0] |= ((0xff & cbuffer[1]) << 8);
      buffer[0] |= ((0xff & cbuffer[2]) << 16);
      buffer[0] |= ((0xff & cbuffer[3]) << 24);
      buffer[0] |= ((0xff & cbuffer[4]) << 32);
      buffer[0] |= ((0xff & cbuffer[5]) << 40);
      buffer[0] |= ((0xff & cbuffer[6]) << 48);
      buffer[0] |= ((0xff & cbuffer[7]) << 56);
      
      buffer[1] |= (0xff & cbuffer[8]);
      buffer[1] |= ((0xff & cbuffer[9]) << 8);
      buffer[1] |= ((0xff & cbuffer[10]) << 16);
      buffer[1] |= ((0xff & cbuffer[11]) << 24);
      buffer[1] |= ((0xff & cbuffer[12]) << 32);
      buffer[1] |= ((0xff & cbuffer[13) << 40);
      buffer[1] |= ((0xff & cbuffer[14]) << 48);
      buffer[1] |= ((0xff & cbuffer[15]) << 56);
      
      buffer[2] |= (0xff & cbuffer[16]);
      buffer[2] |= ((0xff & cbuffer[17]) << 8);
      buffer[2] |= ((0xff & cbuffer[18]) << 16);
      buffer[2] |= ((0xff & cbuffer[19]) << 24);
      buffer[2] |= ((0xff & cbuffer[20]) << 32);
      buffer[2] |= ((0xff & cbuffer[21]) << 40);
      buffer[2] |= ((0xff & cbuffer[22]) << 48);
      buffer[2] |= ((0xff & cbuffer[23]) << 56);
      
      buffer[3] |= (0xff & cbuffer[24]);
      buffer[3] |= ((0xff & cbuffer[25]) << 8);
      buffer[3] |= ((0xff & cbuffer[26]) << 16);
      buffer[3] |= ((0xff & cbuffer[27]) << 24);
      buffer[3] |= ((0xff & cbuffer[28]) << 32);
      buffer[3] |= ((0xff & cbuffer[29]) << 40);
      buffer[3] |= ((0xff & cbuffer[30]) << 48);
      buffer[3] |= ((0xff & cbuffer[31]) << 56);
      
      buffer[4] |= (0xff & cbuffer[32]);
      buffer[4] |= ((0xff & cbuffer[33]) << 8);
      buffer[4] |= ((0xff & cbuffer[34]) << 16);
      buffer[4] |= ((0xff & cbuffer[35]) << 24);
      buffer[4] |= ((0xff & cbuffer[36]) << 32);
      buffer[4] |= ((0xff & cbuffer[37]) << 40);
      buffer[4] |= ((0xff & cbuffer[38]) << 48);
      buffer[4] |= ((0xff & cbuffer[39]) << 56);
      
      buffer[5] |= (0xff & cbuffer[40]);
      buffer[5] |= ((0xff & cbuffer[41]) << 8);
      buffer[5] |= ((0xff & cbuffer[42]) << 16);
      buffer[5] |= ((0xff & cbuffer[43]) << 24);
      buffer[5] |= ((0xff & cbuffer[44]) << 32);
      buffer[5] |= ((0xff & cbuffer[45]) << 40);
      buffer[5] |= ((0xff & cbuffer[46]) << 48);
      buffer[5] |= ((0xff & cbuffer[47]) << 56);
      
      buffer[6] |= (0xff & cbuffer[48]);
      buffer[6] |= ((0xff & cbuffer[49]) << 8);
      buffer[6] |= ((0xff & cbuffer[50]) << 16);
      buffer[6] |= ((0xff & cbuffer[51]) << 24);
      buffer[6] |= ((0xff & cbuffer[52]) << 32);
      buffer[6] |= ((0xff & cbuffer[53]) << 40);
      buffer[6] |= ((0xff & cbuffer[54]) << 48);
      buffer[6] |= ((0xff & cbuffer[55]) << 56);
      
      buffer[7] |= (0xff & cbuffer[56]);
      buffer[7] |= ((0xff & cbuffer[57]) << 8);
      buffer[7] |= ((0xff & cbuffer[58]) << 16);
      buffer[7] |= ((0xff & cbuffer[59]) << 24);
      buffer[7] |= ((0xff & cbuffer[60]) << 32);
      buffer[7] |= ((0xff & cbuffer[61]) << 40);
      buffer[7] |= ((0xff & cbuffer[62]) << 48);
      buffer[7] |= ((0xff & cbuffer[63]) << 56);
      
      buffer += 8;
      cbuffer += (8 * 8);
    }
  }

  for(; i < buffer_size; i += 8){
    buffer[0] |= (0xff & cbuffer[0]);
    buffer[0] |= ((0xff & cbuffer[1]) << 8);
    buffer[0] |= ((0xff & cbuffer[2]) << 16);
    buffer[0] |= ((0xff & cbuffer[3]) << 24);
    buffer[0] |= ((0xff & cbuffer[4]) << 32);
    buffer[0] |= ((0xff & cbuffer[5]) << 40);
    buffer[0] |= ((0xff & cbuffer[6]) << 48);
    buffer[0] |= ((0xff & cbuffer[7]) << 56);
      
    buffer++;
    cbuffer += 8;
  }

  return(buffer);
}
