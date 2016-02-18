/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/midi/ags_midi_buffer_util.h>

/**
 * ags_midi_buffer_util_get_varlength_size:
 * @varlength: the variable length
 *
 * Retrieve the size needed to store the variable length.
 *
 * Returns: the size needed to be allocated
 *
 * Since: 0.7.2
 */
guint
ags_midi_buffer_util_get_varlength_size(long varlength)
{
  unsigned char c;
  guint i;
  long mask;
  
  /* retrieve new size */
  i = 0;
  mask = 0xff;

  do{
    c = ((mask << (i * 8)) & varlength) >> (i * 8);
    i++;
  }while(0x80 & c);

  return(i);
}

/**
 * ags_midi_buffer_util_put_varlength:
 * @buffer: the character buffer
 * @varlength: the value to put
 *
 * Put the variable lenght value to @buffer.
 *
 * Since: 0.7.2
 */
void
ags_midi_buffer_util_put_varlength(unsigned char *buffer,
				   long varlength)
{
  guint i, j;
  long mask;

  i = ags_midi_buffer_util_get_varlength_size(varlength);
  mask = 0xff;
  
  /* write to internal buffer */
  for(j = 0; j < i; i++){
    buffer[j] = ((mask << (j * 8)) & varlength) >> (j * 8);
  }
}

/**
 * ags_midi_buffer_util_get_varlength:
 * @buffer: the character buffer
 * @varlength: the return location
 *
 * Get the variable lenght value from @buffer.
 *
 * Returns: the number of bytes read.
 *
 * Since: 0.7.4
 */
guint
ags_midi_buffer_util_get_varlength(unsigned char *buffer,
				   long *varlength)
{
  long value;
  guint i;
  char c;
  
  c = buffer[0];
  value = c;
  i = 1;
  
  if(c & 0x80){
    value &= 0x7F;
   
    do{
      value = (value << 7) + ((c = buffer[i]) & 0x7F);
      i++;
    }while(c & 0x80);
  }

  if(varlength != NULL){
    *varlength = value;
  }

  return(i);
}

/**
 * ags_midi_buffer_util_put_key_on:
 * @buffer: the character buffer
 * @delta_time: timing information
 * @channel: valid channels from 0-15
 * @key: valid keys to play 0-128
 * @velocity: the key dynamics
 * 
 * Puts the given values to @buffer with appropriate channel message.
 *
 * Since: 0.7.2
 */
void
ags_midi_buffer_util_put_key_on(unsigned char *buffer,
				long delta_time,
				long channel,
				long key,
				long velocity)
{
  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  ags_midi_buffer_util_put_varlength(buffer,
				     delta_time);

  /* key-on channel message */
  buffer[delta_time_size + 1] = 0x90;
  buffer[delta_time_size + 1] |= (0xf & channel);

  /* key */
  buffer[delta_time_size + 2] = 0x7f & key;

  /* velocity */
  buffer[delta_time_size + 3] = 0x7f & velocity;
}

/**
 * ags_midi_buffer_util_get_key_on:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * @channel: the return location of channel
 * @key: the return location of key
 * @velocity: the return location of velocity
 *
 * Get the key-on message from @buffer.
 *
 * Returns: the number of bytes read.
 *
 * Since: 0.7.4
 */
guint
ags_midi_buffer_util_get_key_on(unsigned char *buffer,
				long *delta_time,
				long *channel,
				long *key,
				long *velocity)
{
  long val;
  guint delta_time_size;
  
  if(buffer == NULL){
    return(0);
  }

  delta_time_size = ags_midi_buffer_util_get_varlength(buffer,
						       &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  if(channel != NULL){
    *channel = (0xf & buffer[delta_time_size]);
  }

  if(key != NULL){
    *key = (0x7f & buffer[delta_time_size + 1]);
  }

  if(velocity != NULL){
    *velocity = (0x7f & buffer[delta_time_size + 2]);
  }
  
  return(delta_time_size + 3);
}

/**
 * ags_midi_buffer_util_put_key_off:
 * @buffer: the character buffer
 * @delta_time: timing information
 * @channel: valid channels from 0-15
 * @key: valid keys to play 0-128
 * @velocity: the key dynamics
 * 
 * Puts the given values to @buffer with appropriate channel message.
 *
 * Since: 0.7.2
 */
void
ags_midi_buffer_util_put_key_off(unsigned char *buffer,
				 long delta_time,
				 long channel,
				 long key,
				 long velocity)
{
  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  ags_midi_buffer_util_put_varlength(buffer,
				     delta_time);

  /* key-off channel message */
  buffer[delta_time_size] = 0x80;
  buffer[delta_time_size] |= (0xf & channel);

  /* key */
  buffer[delta_time_size + 1] = 0x7f & key;

  /* velocity */
  buffer[delta_time_size + 2] = 0x7f & velocity;
}

/**
 * ags_midi_buffer_util_get_key_off:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * @channel: the return location of channel
 * @key: the return location of key
 * @velocity: the return location of velocity
 *
 * Get the key-off message from @buffer.
 *
 * Returns: the number of bytes read.
 *
 * Since: 0.7.4
 */
guint
ags_midi_buffer_util_get_key_off(unsigned char *buffer,
				 long *delta_time,
				 long *channel,
				 long *key,
				 long *velocity)
{
  long val;
  guint delta_time_size;
  
  if(buffer == NULL){
    return(0);
  }

  delta_time_size = ags_midi_buffer_util_get_varlength(buffer,
						       &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  if(channel != NULL){
    *channel = (0xf & buffer[delta_time_size]);
  }

  if(key != NULL){
    *key = (0x7f & buffer[delta_time_size + 1]);
  }

  if(velocity != NULL){
    *velocity = (0x7f & buffer[delta_time_size + 2]);
  }
  
  return(delta_time_size + 3);
}

/**
 * ags_midi_buffer_util_put_key_pressure:
 * @buffer: the character buffer
 * @delta_time: timing information
 * @channel: valid channels from 0-15
 * @key: valid keys to play 0-128
 * @pressure: the key dynamics
 * 
 * Puts the given values to @buffer with appropriate channel message.
 *
 * Since: 0.7.2
 */
void
ags_midi_buffer_util_put_key_pressure(unsigned char *buffer,
				      long delta_time,
				      long channel,
				      long key,
				      long pressure)
{
  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  ags_midi_buffer_util_put_varlength(buffer,
				     delta_time);

  /* key-pressure channel message */
  buffer[delta_time_size + 1] = 0xa0;
  buffer[delta_time_size + 1] |= (0xf & channel);

  /* key */
  buffer[delta_time_size + 2] = 0x7f & key;

  /* velocity */
  buffer[delta_time_size + 3] = 0x7f & pressure;
}

/**
 * ags_midi_buffer_util_get_key_pressure:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * @channel: the return location of channel
 * @key: the return location of key
 * @pressure: the return location of pressure
 *
 * Get the key-pressure message from @buffer.
 *
 * Returns: the number of bytes read.
 *
 * Since: 0.7.4
 */
guint
ags_midi_buffer_util_get_key_pressure(unsigned char *buffer,
				      long *delta_time,
				      long *channel,
				      long *key,
				      long *pressure)
{
  long val;
  guint delta_time_size;
  
  if(buffer == NULL){
    return(0);
  }

  delta_time_size = ags_midi_buffer_util_get_varlength(buffer,
						       &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  if(channel != NULL){
    *channel = (0xf & buffer[delta_time_size]);
  }

  if(key != NULL){
    *key = (0x7f & buffer[delta_time_size + 1]);
  }

  if(pressure != NULL){
    *pressure = (0x7f & buffer[delta_time_size + 2]);
  }
  
  return(delta_time_size + 3);
}
