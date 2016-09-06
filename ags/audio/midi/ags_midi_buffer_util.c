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

#include <alsa/seq_midi_event.h>
#include <alsa/seq_event.h>

/**
 * SECTION:ags_midi_buffer_util
 * @short_description: MIDI buffer util
 * @title: AgsMidiBufferUtil
 * @section_id:
 * @include: ags/audio/midi/ags_midi_buffer_util.h
 *
 * Utility functions for MIDI buffers.
 */

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
ags_midi_buffer_util_get_varlength_size(glong varlength)
{
  guint current;
  unsigned char c;
  guint i;
  glong mask;
  
  /* retrieve new size */
  mask = 0x7f;
  current = 8 * 4 - 4;
  i = current;

  for(; current >= 8; ){
    if(((mask << (i - 7)) & varlength) != 0){
      break;
    }

    i -= 7;
    current -= 8;
  }

  
  return(floor(current / 8.0) + 1);
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
				   glong varlength)
{
  guint i, j;
  glong mask;

  i = ags_midi_buffer_util_get_varlength_size(varlength);
  mask = 0xff;
  
  /* write to internal buffer */
  mask = 0x7f;
  j = 8 * i;
  i = 8 * i - 1;

  for(; i > 0; ){
    buffer[j] = ((mask << (i - 7)) & varlength) >> (i * 7);
    
    i -= 7;
    j -= 8;
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
				   glong *varlength)
{
  glong value;
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
				glong delta_time,
				glong channel,
				glong key,
				glong velocity)
{
  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  ags_midi_buffer_util_put_varlength(buffer,
				     delta_time);

  /* key-on channel message */
  buffer[delta_time_size] = 0x90 | (channel & 0xf);

  /* key */
  buffer[delta_time_size + 1] = key & 0x7f;

  /* velocity */
  buffer[delta_time_size + 2] = velocity & 0x7f;
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
				glong *delta_time,
				glong *channel,
				glong *key,
				glong *velocity)
{
  glong val;
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
				 glong delta_time,
				 glong channel,
				 glong key,
				 glong velocity)
{
  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  ags_midi_buffer_util_put_varlength(buffer,
				     delta_time);

  /* key-off channel message */
  buffer[delta_time_size] = 0x80;
  buffer[delta_time_size] |= (channel & 0xf);

  /* key */
  buffer[delta_time_size + 1] = key & 0x7f;

  /* velocity */
  buffer[delta_time_size + 2] = velocity & 0x7f;
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
				 glong *delta_time,
				 glong *channel,
				 glong *key,
				 glong *velocity)
{
  glong val;
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
				      glong delta_time,
				      glong channel,
				      glong key,
				      glong pressure)
{
  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  ags_midi_buffer_util_put_varlength(buffer,
				     delta_time);

  /* key-pressure channel message */
  buffer[delta_time_size + 1] = 0xa0;
  buffer[delta_time_size + 1] |= (channel & 0xf);

  /* key */
  buffer[delta_time_size + 2] = key & 0x7f;

  /* velocity */
  buffer[delta_time_size + 3] = pressure & 0x7f;
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
				      glong *delta_time,
				      glong *channel,
				      glong *key,
				      glong *pressure)
{
  glong val;
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

/**
 * ags_midi_buffer_util_decode:
 * @buffer: the midi buffer
 * @event: the ALSA sequencer event
 *
 * Decode @event to @buffer
 *
 * Returns: the bytes written
 * 
 * Since: 0.7.64
 */
guint
ags_midi_buffer_util_decode(unsigned char *buffer,
			    snd_seq_event_t *event)
{
  guint count;

  count = 0;
  
  switch(event->type){
  case SND_SEQ_EVENT_NOTEON:
    {    
      unsigned char tmp[8];
      
      ags_midi_buffer_util_put_key_on(tmp,
				      0,
				      event->data.note.channel,
				      event->data.note.note,
				      event->data.note.velocity);

      count = ags_midi_buffer_util_get_varlength_size(0);      
      memcpy(buffer, tmp + count, 3 * sizeof(unsigned char));
      
      count = 3;
    }
    break;
  case SND_SEQ_EVENT_NOTEOFF:
    {
      unsigned char tmp[8];

      ags_midi_buffer_util_put_key_off(tmp,
				       0,
				       event->data.note.channel,
				       event->data.note.note,
				       event->data.note.velocity);

      count = ags_midi_buffer_util_get_varlength_size(0);
      memcpy(buffer, tmp + count, 3 * sizeof(unsigned char));
      
      count = 3;      
    }
    break;
  default:
    g_warning("ags_midi_buffer_util_decode() - unsupported MIDI event\0");
    break;
  }
  
  return(count);
}

