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
 * ags_midi_buffer_util_put_key_on:
 * @delta_time:
 * @channel:
 * @key:
 * @velocity:
 * 
 *
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
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_put_key_off:
 * @delta_time:
 * @channel:
 * @key:
 * @velocity:
 * 
 *
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
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_put_key_pressure:
 * @delta_time:
 * @channel:
 * @key:
 * @pressure:
 * 
 *
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
  //TODO:JK: implement me
}
