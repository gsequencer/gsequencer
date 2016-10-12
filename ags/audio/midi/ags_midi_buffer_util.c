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

#include <math.h>
#include <string.h>

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
  guint varlength_size;
  guint i, j;
  glong mask;

  varlength_size = ags_midi_buffer_util_get_varlength_size(varlength);

  /* write to internal buffer */
  mask = 0x7f;
  j = 8 * varlength_size;
  i = 8 * varlength_size - 1;

  for(; i > 0; ){
    buffer[j] = ((mask << (i - 7)) & varlength) >> (i * 7);

    if(j < varlength_size){
      buffer[j] |= 0x80;
    }
    
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
 * ags_midi_buffer_util_put_int16:
 * @buffer: the character buffer
 * @val: the integer
 * 
 * Put signed 16 bit integer.
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_int16(unsigned char *buffer,
			       glong val)
{
  buffer[0] = val & (0xff << 8);
  buffer[1] = val & 0xff;
}

/**
 * ags_midi_buffer_util_get_int16:
 * @buffer: the character buffer
 * @val: return location of the integer
 *
 * Get signed 32 bit integer.
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_get_int16(unsigned char *buffer,
			       glong *val)
{
  glong tmp;
  
  tmp = (buffer[0] & 0xff);
  tmp = (tmp << 8) + (buffer[1] & 0xff);

  if(val != NULL){
    *val = tmp;
  }
}

/**
 * ags_midi_buffer_util_put_int32:
 * @buffer: the character buffer
 * @val: the integer
 * 
 * Put signed 32 bit integer.
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_int32(unsigned char *buffer,
			       glong val)
{
  buffer[0] = val & (0xff << 24);
  buffer[1] = val & (0xff << 16);
  buffer[2] = val & (0xff << 8);
  buffer[3] = val & 0xff;
}

/**
 * ags_midi_buffer_util_get_int32:
 * @buffer: the character buffer
 * @val: return location of the integer
 *
 * Get signed 32 bit integer.
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_get_int32(unsigned char *buffer,
			       glong *val)
{
  glong tmp;
  
  tmp = (buffer[0] & 0xff);
  tmp = (tmp << 8) + (buffer[1] & 0xff);
  tmp = (tmp << 8) + (buffer[2] & 0xff);
  tmp = (tmp << 8) + (buffer[3] & 0xff);

  if(val != NULL){
    *val = tmp;
  }
}

/**
 * ags_midi_buffer_util_put_header:
 * @buffer: the character buffer
 * @offset: start delta-time
 * @format: either 0, 1 or 2.
 * @track_count: the number of tracks
 * @division: timing division
 *
 * Puts the midi header.
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_header(unsigned char *buffer,
				glong offset, glong format,
				glong track_count, glong division)
{
  static gchar header[] = "MThd";

  /* put MThd */
  memcpy(buffer, header, 4 * sizeof(unsigned char));

  /* offset */
  ags_midi_buffer_util_put_int32(buffer + 4,
				 offset);

  /* format */
  ags_midi_buffer_util_put_int16(buffer + 8,
				 format);

  /* track count */
  ags_midi_buffer_util_put_int16(buffer + 10,
				 track_count);

  /* division */
  ags_midi_buffer_util_put_int16(buffer + 12,
				 division);  
}

/**
 * ags_midi_buffer_util_get_header:
 * @buffer: the character buffer
 * @offset: start delta-time
 * @format: either 0, 1 or 2.
 * @track_count: the number of tracks
 * @division: timing division
 * 
 * Gets the midi header
 * 
 * Returns: the number of bytes read.
 *
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_header(unsigned char *buffer,
				glong *offset, glong *format,
				glong *track_count, glong *division)
{
  static gchar header[] = "MThd";

  if(g_ascii_strncasecmp(buffer,
			 header,
			 4)){
    return(0);
  }
  
  /* offset */
  ags_midi_buffer_util_get_int32(buffer + 4,
				 offset);

  /* format */
  ags_midi_buffer_util_get_int16(buffer + 8,
				 format);

  /* track count */
  ags_midi_buffer_util_get_int16(buffer + 10,
				 track_count);

  /* division */
  ags_midi_buffer_util_get_int16(buffer + 12,
				 division);

  return(14);
}

/**
 * ags_midi_buffer_util_put_track:
 * @offset: start delta-time
 * 
 * Put track.
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_track(unsigned char *buffer,
			       glong offset)
{
  static gchar track[] = "MTrk";

  /* put MTrk */
  memcpy(buffer, track, 4 * sizeof(unsigned char));

  /* offset */
  ags_midi_buffer_util_put_int32(buffer + 4,
				 offset);  
}

/**
 * ags_midi_buffer_util_get_track:
 * @offset: start delta-time
 * 
 * Get track
 * 
 * Returns: the number of bytes read.
 *
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_track(unsigned char *buffer,
			       glong *offset)
{
  static gchar track[] = "MTrk";

  if(g_ascii_strncasecmp(buffer,
			 track,
			 4)){
    return(0);
  }

  /* offset */
  ags_midi_buffer_util_get_int32(buffer + 4,
				 offset);

  return(8);
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

  /* delta time */
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

  /* delta time */
  delta_time_size = ags_midi_buffer_util_get_varlength(buffer,
						       &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  /* channel */
  if(channel != NULL){
    *channel = (0xf & buffer[delta_time_size]);
  }

  /* key */
  if(key != NULL){
    *key = (0x7f & buffer[delta_time_size + 1]);
  }

  /* velocity */
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

  /* delta time */
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

  /* delta time */
  delta_time_size = ags_midi_buffer_util_get_varlength(buffer,
						       &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  /* channel */
  if(channel != NULL){
    *channel = (0xf & buffer[delta_time_size]);
  }

  /* key */
  if(key != NULL){
    *key = (0x7f & buffer[delta_time_size + 1]);
  }

  /* velocity */
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

  /* delta time */
  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  ags_midi_buffer_util_put_varlength(buffer,
				     delta_time);

  /* key-pressure channel message */
  buffer[delta_time_size] = 0xa0;
  buffer[delta_time_size] |= (channel & 0xf);

  /* key */
  buffer[delta_time_size + 1] = key & 0x7f;

  /* pressure */
  buffer[delta_time_size + 2] = pressure & 0x7f;
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
 * Since: 1.0.0
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

  /* delta time */
  delta_time_size = ags_midi_buffer_util_get_varlength(buffer,
						       &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  /* channel */
  if(channel != NULL){
    *channel = (0xf & buffer[delta_time_size]);
  }

  /* key */
  if(key != NULL){
    *key = (0x7f & buffer[delta_time_size + 1]);
  }

  /* pressure */
  if(pressure != NULL){
    *pressure = (0x7f & buffer[delta_time_size + 2]);
  }
  
  return(delta_time_size + 3);
}

/**
 * ags_midi_buffer_util_put_change_parameter:
 * @buffer: the character buffer
 * @delta_time: timing information
 * @channel: channel
 * @control: the control
 * @value: the value
 * 
 * Put change parameter.
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_change_parameter(unsigned char *buffer,
					  glong delta_time,
					  glong channel,
					  glong control,
					  glong value)
{
  guint delta_time_size;

  /* delta time */
  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  ags_midi_buffer_util_put_varlength(buffer,
				     delta_time);

  /* change-parameter channel message */
  buffer[delta_time_size] = 0xb0;
  buffer[delta_time_size] |= (channel & 0xf);

  /* control */
  buffer[delta_time_size + 1] = 0x7f & control;
  
  /* value */
  buffer[delta_time_size + 2] = 0x7f & value;
}

/**
 * ags_midi_buffer_util_get_change_parameter:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * @channel: the return location of channel
 * @control: the return location of the control
 * @value: the return location the value
 *
 * Get change parameter.
 * 
 * Returns: the number of bytes read.
 *
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_change_parameter(unsigned char *buffer,
					  glong *delta_time,
					  glong *channel,
					  glong *control,
					  glong *value)
{
  glong val;
  guint delta_time_size;
  
  if(buffer == NULL){
    return(0);
  }

  /* delta time */
  delta_time_size = ags_midi_buffer_util_get_varlength(buffer,
						       &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  /* channel */
  if(channel != NULL){
    *channel = (0xf & buffer[delta_time_size]);
  }

  /* control */
  if(control != NULL){
    *control = 0x7f & buffer[delta_time_size + 1];
  }

  /* value */
  if(value != NULL){
    *value = 0x7f & buffer[delta_time_size + 2];
  }
  
  return(delta_time_size + 3);
}

/**
 * ags_midi_buffer_util_put_pitch_bend:
 * @buffer: the character buffer
 * @delta_time: timing information
 * @channel: channel
 * @pitch: the pitch
 * @transmitter: the transmitter
 * 
 * Put pitch bend.
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_pitch_bend(unsigned char *buffer,
				    glong delta_time,
				    glong channel,
				    glong pitch,
				    glong transmitter)
{
  guint delta_time_size;

  /* delta time */
  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  ags_midi_buffer_util_put_varlength(buffer,
				     delta_time);

  /* pitch-bend channel message */
  buffer[delta_time_size] = 0xe0;
  buffer[delta_time_size] |= (channel & 0xf);

  /* pitch */
  buffer[delta_time_size + 1] = 0x7f & pitch;
  
  /* transmitter */
  buffer[delta_time_size + 2] = 0x7f & transmitter;
}

/**
 * ags_midi_buffer_util_get_pitch_bend:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * @channel: the return location of channel
 * @pitch: the return location of the pitch
 * @transmitter: the return location the transmitter
 * 
 * Get pitch bend.
 * 
 * Returns: the number of bytes read.
 *
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_pitch_bend(unsigned char *buffer,
				    glong *delta_time,
				    glong *channel,
				    glong *pitch,
				    glong *transmitter)
{
  glong val;
  guint delta_time_size;
  
  if(buffer == NULL){
    return(0);
  }

  /* delta time */
  delta_time_size = ags_midi_buffer_util_get_varlength(buffer,
						       &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  /* channel */
  if(channel != NULL){
    *channel = (0xf & buffer[delta_time_size]);
  }

  /* pitch */
  if(pitch != NULL){
    *pitch = 0x7f & buffer[delta_time_size + 1];
  }
  
  /* transmitter */
  if(transmitter != NULL){
    *transmitter = 0x7f & buffer[delta_time_size + 2];
  }
  
  return(delta_time_size + 3);
}

/**
 * ags_midi_buffer_util_put_change_program:
 * @buffer: the character buffer
 * @delta_time: timing information
 * @channel: channel
 * @program: the program
 * 
 * Put change program.
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_change_program(unsigned char *buffer,
					glong delta_time,
					glong channel,
					glong program)
{
  guint delta_time_size;

  /* delta time */
  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  ags_midi_buffer_util_put_varlength(buffer,
				     delta_time);

  /* change-parameter channel message */
  buffer[delta_time_size] = 0xc0;
  buffer[delta_time_size] |= (channel & 0xf);

  /* program */
  buffer[delta_time_size + 1] = 0x7f & program;

  return(delta_time_size + 2);
}

/**
 * ags_midi_buffer_util_get_change_program:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * @channel: the return location of channel
 * @program: the return location of the program
 * 
 * Get change program.
 * 
 * Returns: the number of bytes read.
 *
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_change_program(unsigned char *buffer,
					glong *delta_time,
					glong *channel,
					glong *program)
{
  glong val;
  guint delta_time_size;
  
  if(buffer == NULL){
    return(0);
  }

  /* delta time */
  delta_time_size = ags_midi_buffer_util_get_varlength(buffer,
						       &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  /* channel */
  if(channel != NULL){
    *channel = (0xf & buffer[delta_time_size]);
  }

  /* program */
  if(program != NULL){
    *program = 0x7f & buffer[delta_time_size + 1];
  }
  
  return(delta_time_size + 2);
}

/**
 * ags_midi_buffer_util_put_change_pressure:
 * @buffer: the character buffer
 * @delta_time: timing information
 * @channel: channel
 * @pressure: the pressure
 *
 * Put change pressure.
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_change_pressure(unsigned char *buffer,
					 glong delta_time,
					 glong channel,
					 glong pressure)
{
  guint delta_time_size;

  /* delta time */
  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  ags_midi_buffer_util_put_varlength(buffer,
				     delta_time);

  /* change-parameter channel message */
  buffer[delta_time_size] = 0xd0;
  buffer[delta_time_size] |= (channel & 0xf);

  /* pressure */
  buffer[delta_time_size + 1] = 0x7f & pressure;
}

/**
 * ags_midi_buffer_util_get_change_pressure:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * @channel: the return location of channel
 * @pressure: the return location of the pressure
 * 
 * Get change pressure.
 * 
 * Returns: the number of bytes read.
 *
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_change_pressure(unsigned char *buffer,
					 glong *delta_time,
					 glong *channel,
					 glong *pressure)
{
  glong val;
  guint delta_time_size;
  
  if(buffer == NULL){
    return(0);
  }

  /* delta time */
  delta_time_size = ags_midi_buffer_util_get_varlength(buffer,
						       &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  /* channel */
  if(channel != NULL){
    *channel = (0xf & buffer[delta_time_size]);
  }

  /* pressure */
  if(pressure != NULL){
    *pressure = 0x7f & buffer[delta_time_size + 1];
  }

  return(delta_time_size + 2);
}

/**
 * ags_midi_buffer_util_put_sysex:
 * @buffer: the character buffer
 * @delta_time: timing information
 * @data: the data
 * @length: the data's length
 * 
 * Put sysex.
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_sysex(unsigned char *buffer,
			       glong delta_time,
			       unsigned char *data, glong length)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_get_sysex:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * @data: the return location of data
 * @length: the return location of length
 * 
 * Get sysex.
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_sysex(unsigned char *buffer,
			       glong *delta_time,
			       unsigned char **data, glong *length)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_put_quarter_frame:
 * @buffer: the character buffer
 * @delta_time: timing information
 * @message_type: the message type
 * @values: the values
 * 
 * Put quarter frame.
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_quarter_frame(unsigned char *buffer,
				       glong delta_time,
				       glong message_type,
				       glong values)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_get_quarter_frame:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * @message_type: the return location of the message type
 * @values: the return location of the values
 * 
 * Get quarter frame.
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_quarter_frame(unsigned char *buffer,
				       glong *delta_time,
				       glong *message_type, glong *values)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_put_song_position:
 * @buffer: the character buffer
 * @delta_time: timing information
 * @song_position: the song position
 * 
 * Put song position.
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_song_position(unsigned char *buffer,
				       glong delta_time,
				       glong song_position)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_get_song_position:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * @song_position: the return location of the song position
 * 
 * Get song position.
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_song_position(unsigned char *buffer,
				       glong *delta_time,
				       glong *song_position)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_put_song_select:
 * @buffer: the character buffer
 * @delta_time: timing information
 * @song_select: the song select
 * 
 * Put song select.
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_song_select(unsigned char *buffer,
				     glong delta_time,
				     glong song_select)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_get_song_select:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * @song_select: the return location of the song select
 * 
 * Get song select.
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_song_select(unsigned char *buffer,
				     glong *delta_time,
				     glong *song_select)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_put_tune_request:
 * @buffer: the character buffer
 * @delta_time: timing information
 * 
 * Put tune request
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_tune_request(unsigned char *buffer,
				      glong delta_time)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_get_tune_request:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * 
 * Get tune request.
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_tune_request(unsigned char *buffer,
				      glong *delta_time)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_put_sequence_number:
 * @buffer: the character buffer
 * @delta_time: timing information
 * @sequence: the sequence
 * 
 * Put sequence number.
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_sequence_number(unsigned char *buffer,
					 glong delta_time,
					 glong sequence)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_get_sequence_number:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * @sequence: the return location of the sequence
 * 
 * Get sequence number.
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_sequence_number(unsigned char *buffer,
					 glong *delta_time,
					 glong *sequence)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_put_smtpe:
 * @buffer: the character buffer
 * @delta_time: timing information
 * @hr: hour
 * @mn: minute
 * @se: second
 * @fr: frame
 * @ff: 
 * 
 * Put smtpe timestamp.
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_smtpe(unsigned char *buffer,
			       glong delta_time,
			       glong hr, glong mn, glong se, glong fr, glong ff)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_get_smtpe:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * 
 * 
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_smtpe(unsigned char *buffer,
			       glong *delta_time,
			       glong *hr, glong *mn, glong *se, glong *fr, glong *ff)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_put_tempo:
 * @buffer: the character buffer
 * @delta_time: timing information
 * 
 * 
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_tempo(unsigned char *buffer,
			       glong delta_time,
			       glong tempo)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_get_tempo:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * 
 * 
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_tempo(unsigned char *buffer,
			       glong *delta_time,
			       glong tempo)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_put_time_signature:
 * @buffer: the character buffer
 * @delta_time: timing information
 * 
 * 
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_time_signature(unsigned char *buffer,
					glong delta_time,
					glong nn, glong dd, glong cc, glong bb)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_get_time_signature:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * 
 * 
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_time_signature(unsigned char *buffer,
					glong *delta_time,
					glong *nn, glong *dd, glong *cc, glong *bb)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_put_key_signature:
 * @buffer: the character buffer
 * @delta_time: timing information
 * 
 * 
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_key_signature(unsigned char *buffer,
				       glong delta_time,
				       glong sf, glong mi)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_get_key_signature:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * 
 * 
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_key_signature(unsigned char *buffer,
				       glong *delta_time,
				       glong *sf, glong *mi)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_put_sequencer_meta_event:
 * @buffer: the character buffer
 * @delta_time: timing information
 * 
 * 
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_sequencer_meta_event(unsigned char *buffer,
					      glong delta_time,
					      glong len, glong id, glong data)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_get_sequencer_meta_event:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * 
 * 
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_sequencer_meta_event(unsigned char *buffer,
					      glong *delta_time,
					      glong *len, glong *id, glong *data)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_put_text_event:
 * @buffer: the character buffer
 * @delta_time: timing information
 * 
 * 
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_text_event(unsigned char *buffer,
				    glong delta_time,
				    gchar *text, glong length)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_get_text_event:
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * 
 * 
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_text_event(unsigned char *buffer,
				    glong *delta_time,
				    gchar **text, glong *length)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_buffer_util_put_end_of_track:
 * @buffer: the character buffer
 * @delta_time: timing information
 * 
 * 
 * 
 * Since: 1.0.0
 */
void
ags_midi_buffer_util_put_end_of_track(unsigned char *buffer,
				      glong delta_time)
{
  //TODO:JK: implement me
}

/**
 * 
 * @buffer: the character buffer
 * @delta_time: the return location of timing information
 * 
 * 
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 1.0.0
 */
guint
ags_midi_buffer_util_get_end_of_track(unsigned char *buffer,
				      glong *delta_time)
{
  //TODO:JK: implement me
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
