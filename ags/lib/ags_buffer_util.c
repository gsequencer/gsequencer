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

    for(; i < limit;){
      cbuffer[0] = (unsigned char *) (0xff & buffer[0]);
      cbuffer[1] = (unsigned char *) (0xff & buffer[1]);
      cbuffer[2] = (unsigned char *) (0xff & buffer[2]);
      cbuffer[3] = (unsigned char *) (0xff & buffer[3]);
      cbuffer[4] = (unsigned char *) (0xff & buffer[4]);
      cbuffer[5] = (unsigned char *) (0xff & buffer[5]);
      cbuffer[6] = (unsigned char *) (0xff & buffer[6]);
      cbuffer[7] = (unsigned char *) (0xff & buffer[7]);

      buffer += 8;
      cbuffer += 8;
    }
  }

  for(; i < buffer_length; i++){
    cbuffer[0] = (unsigned char *) (0xff & buffer[0]);

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

    for(; i < limit;){
      cbuffer[0] = (unsigned char *) (0xff & buffer[0]);
      cbuffer[1] = (unsigned char *) ((0xff00 & buffer[0]) >> 8);

      cbuffer[2] = (unsigned char *) (0xff & buffer[1]);
      cbuffer[3] = (unsigned char *) ((0xff00 & buffer[1]) >> 8);

      cbuffer[4] = (unsigned char *) (0xff & buffer[2]);
      cbuffer[5] = (unsigned char *) ((0xff00 & buffer[2]) >> 8);

      cbuffer[6] = (unsigned char *) (0xff & buffer[3]);
      cbuffer[7] = (unsigned char *) ((0xff00 & buffer[3]) >> 8);

      cbuffer[8] = (unsigned char *) (0xff & buffer[4]);
      cbuffer[9] = (unsigned char *) ((0xff00 & buffer[4]) >> 8);
      
      cbuffer[10] = (unsigned char *) (0xff & buffer[5]);
      cbuffer[11] = (unsigned char *) ((0xff00 & buffer[5]) >> 8);

      cbuffer[12 = (unsigned char *) (0xff & buffer[6]);
      cbuffer[13] = (unsigned char *) ((0xff00 & buffer[6]) >> 8);

      cbuffer[14] = (unsigned char *) (0xff & buffer[7]);
      cbuffer[15] = (unsigned char *) ((0xff00 & buffer[7]) >> 8);
      
      buffer += 8;
      cbuffer += (2 * 8);
    }
  }

  for(; i < buffer_length; i++){
    cbuffer[0] = (unsigned char *) (0xff & buffer[0]);
    cbuffer[1] = (unsigned char *) ((0xff00 & buffer[0]) >> 8);

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

    for(; i < limit;){
      cbuffer[0] = (unsigned char *) (0xff & buffer[0]);
      cbuffer[1] = (unsigned char *) ((0xff00 & buffer[0]) >> 8);
      cbuffer[2] = (unsigned char *) ((0xff0000 & buffer[0]) >> 16);

      cbuffer[3] = (unsigned char *) (0xff & buffer[1]);
      cbuffer[4] = (unsigned char *) ((0xff00 & buffer[1]) >> 8);
      cbuffer[5] = (unsigned char *) ((0xff0000 & buffer[1]) >> 16);

      cbuffer[6] = (unsigned char *) (0xff & buffer[2]);
      cbuffer[7] = (unsigned char *) ((0xff00 & buffer[2]) >> 8);
      cbuffer[8] = (unsigned char *) ((0xff0000 & buffer[2]) >> 16);

      cbuffer[9] = (unsigned char *) (0xff & buffer[3]);
      cbuffer[10] = (unsigned char *) ((0xff00 & buffer[3]) >> 8);
      cbuffer[11] = (unsigned char *) ((0xff0000 & buffer[3]) >> 16);

      cbuffer[12] = (unsigned char *) (0xff & buffer[4]);
      cbuffer[13] = (unsigned char *) ((0xff00 & buffer[4]) >> 8);
      cbuffer[14] = (unsigned char *) ((0xff0000 & buffer[4]) >> 16);
      
      cbuffer[15] = (unsigned char *) (0xff & buffer[5]);
      cbuffer[16] = (unsigned char *) ((0xff00 & buffer[5]) >> 8);
      cbuffer[17] = (unsigned char *) ((0xff0000 & buffer[5]) >> 16);

      cbuffer[18] = (unsigned char *) (0xff & buffer[6]);
      cbuffer[19] = (unsigned char *) ((0xff00 & buffer[6]) >> 8);
      cbuffer[20] = (unsigned char *) ((0xff0000 & buffer[6]) >> 16);

      cbuffer[21] = (unsigned char *) (0xff & buffer[7]);
      cbuffer[22] = (unsigned char *) ((0xff00 & buffer[7]) >> 8);
      cbuffer[23] = (unsigned char *) ((0xff0000 & buffer[7]) >> 16);
      
      buffer += 8;
      cbuffer += (3 * 8);
    }
  }

  for(; i < buffer_length; i++){
    cbuffer[0] = (unsigned char *) (0xff & buffer[0]);
    cbuffer[1] = (unsigned char *) ((0xff00 & buffer[0]) >> 8);
    cbuffer[2] = (unsigned char *) ((0xff0000 & buffer[0]) >> 16);

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

    for(; i < limit;){
      cbuffer[0] = (unsigned char *) (0xff & buffer[0]);
      cbuffer[1] = (unsigned char *) ((0xff00 & buffer[0]) >> 8);
      cbuffer[2] = (unsigned char *) ((0xff0000 & buffer[0]) >> 16);
      cbuffer[3] = (unsigned char *) ((0xff000000 & buffer[0]) >> 24);
      
      cbuffer[4] = (unsigned char *) (0xff & buffer[1]);
      cbuffer[5] = (unsigned char *) ((0xff00 & buffer[1]) >> 8);
      cbuffer[6] = (unsigned char *) ((0xff0000 & buffer[1]) >> 16);
      cbuffer[7] = (unsigned char *) ((0xff000000 & buffer[1]) >> 24);

      cbuffer[8] = (unsigned char *) (0xff & buffer[2]);
      cbuffer[9] = (unsigned char *) ((0xff00 & buffer[2]) >> 8);
      cbuffer[10] = (unsigned char *) ((0xff0000 & buffer[2]) >> 16);
      cbuffer[11] = (unsigned char *) ((0xff000000 & buffer[2]) >> 24);

      cbuffer[12] = (unsigned char *) (0xff & buffer[3]);
      cbuffer[13] = (unsigned char *) ((0xff00 & buffer[3]) >> 8);
      cbuffer[14] = (unsigned char *) ((0xff0000 & buffer[3]) >> 16);
      cbuffer[15] = (unsigned char *) ((0xff000000 & buffer[3]) >> 24);

      cbuffer[16] = (unsigned char *) (0xff & buffer[4]);
      cbuffer[17] = (unsigned char *) ((0xff00 & buffer[4]) >> 8);
      cbuffer[18] = (unsigned char *) ((0xff0000 & buffer[4]) >> 16);
      cbuffer[19] = (unsigned char *) ((0xff000000 & buffer[4]) >> 24);

      cbuffer[20] = (unsigned char *) (0xff & buffer[5]);
      cbuffer[21] = (unsigned char *) ((0xff00 & buffer[5]) >> 8);
      cbuffer[22] = (unsigned char *) ((0xff0000 & buffer[5]) >> 16);
      cbuffer[23] = (unsigned char *) ((0xff000000 & buffer[5]) >> 24);

      cbuffer[24] = (unsigned char *) (0xff & buffer[6]);
      cbuffer[25] = (unsigned char *) ((0xff00 & buffer[6]) >> 8);
      cbuffer[26] = (unsigned char *) ((0xff0000 & buffer[6]) >> 16);
      cbuffer[27] = (unsigned char *) ((0xff000000 & buffer[6]) >> 24);

      cbuffer[28] = (unsigned char *) (0xff & buffer[7]);
      cbuffer[29] = (unsigned char *) ((0xff00 & buffer[7]) >> 8);
      cbuffer[30] = (unsigned char *) ((0xff0000 & buffer[7]) >> 16);
      cbuffer[31] = (unsigned char *) ((0xff000000 & buffer[7]) >> 24);
      
      buffer += 8;
      cbuffer += (4 * 8);
    }
  }

  for(; i < buffer_length; i++){
    cbuffer[0] = (unsigned char *) (0xff & buffer[0]);
    cbuffer[1] = (unsigned char *) ((0xff00 & buffer[0]) >> 8);
    cbuffer[2] = (unsigned char *) ((0xff0000 & buffer[0]) >> 16);
    cbuffer[3] = (unsigned char *) ((0xff000000 & buffer[0]) >> 24);

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
  //TODO:JK: implement me
}

/**
 * ags_buffer_util_char_buffer_to_s8:
 * @buffer: the unsigned char buffer
 * @buffer_size: the buffer size
 *
 * Unpack @buffer to a signed char buffer
 *
 * Returns: the signed char buffer
 * 
 * Since: 1.4.0
 */
signed char*
ags_buffer_util_char_buffer_to_s8(unsigned char *cbuffer,
				  guint buffer_size)
{
  //TODO:JK: implement me
}

/**
 * ags_buffer_util_char_buffer_to_s16:
 * @buffer: the unsigned char buffer
 * @buffer_size: the buffer size
 *
 * Unpack @buffer to a signed short buffer
 *
 * Returns: the signed short buffer
 * 
 * Since: 1.4.0
 */
signed short*
ags_buffer_util_char_buffer_to_s16(unsigned char *cbuffer,
				   guint buffer_size)
{
  //TODO:JK: implement me
}

/**
 * ags_buffer_util_char_buffer_to_s24:
 * @buffer: the unsigned char buffer
 * @buffer_size: the buffer size
 *
 * Unpack @buffer to a signed long buffer
 *
 * Returns: the signed long buffer
 * 
 * Since: 1.4.0
 */
signed long*
ags_buffer_util_char_buffer_to_s24(unsigned char *cbuffer,
				   guint buffer_size)
{
  //TODO:JK: implement me
}

/**
 * ags_buffer_util_char_buffer_to_s32:
 * @buffer: the unsigned char buffer
 * @buffer_size: the buffer size
 *
 * Unpack @buffer to a signed long buffer
 *
 * Returns: the signed long buffer
 * 
 * Since: 1.4.0
 */
signed long*
ags_buffer_util_char_buffer_to_s32(unsigned char *cbuffer,
				   guint buffer_size)
{
  //TODO:JK: implement me
}

/**
 * ags_buffer_util_char_buffer_to_s64:
 * @buffer: the unsigned char buffer
 * @buffer_size: the buffer size
 *
 * Unpack @buffer to a signed long long buffer
 *
 * Returns: the signed long long buffer
 * 
 * Since: 1.4.0
 */
signed long long*
ags_buffer_util_char_buffer_to_s64(unsigned char *cbuffer,
				   guint buffer_size)
{
  //TODO:JK: implement me
}
