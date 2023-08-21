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

#include <ags/audio/midi/ags_midi_smf_util.h>

#include <alsa/seq_midi_event.h>
#include <alsa/seq_event.h>

#include <math.h>
#include <string.h>
#include <stdlib.h>

/**
 * SECTION:ags_midi_smf_util
 * @short_description: MIDI SMF buffer util
 * @title: AgsMidiSmfUtil
 * @section_id:
 * @include: ags/audio/midi/ags_midi_smf_util.h
 *
 * Utility functions for MIDI SMF buffers.
 */

GType
ags_midi_smf_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_midi_smf_util = 0;

    ags_type_midi_smf_util =
      g_boxed_type_register_static("AgsMidiSmfUtil",
				   (GBoxedCopyFunc) ags_midi_smf_util_copy,
				   (GBoxedFreeFunc) ags_midi_smf_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_midi_smf_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_midi_smf_util_alloc:
 *
 * Allocate MIDI util.
 *
 * Returns: (transfer full): the newly allocated #AgsMidiSmfUtil-struct
 * 
 * Since: 6.0.0
 */
AgsMidiSmfUtil*
ags_midi_smf_util_alloc()
{
  AgsMidiSmfUtil *midi_smf_util;

  midi_smf_util = g_new0(AgsMidiSmfUtil,
			 1);

  midi_smf_util->major = 1;
  midi_smf_util->minor = 0;
  
  return(midi_smf_util);
}

/**
 * ags_midi_smf_util_free:
 * @midi_smf_util: the MIDI util
 *
 * Free MIDI util.
 *
 * Since: 6.0.0
 */
void
ags_midi_smf_util_free(AgsMidiSmfUtil *midi_smf_util)
{
  g_return_if_fail(midi_smf_util != NULL);
  
  g_free(midi_smf_util);
}

/**
 * ags_midi_smf_util_copy:
 * @midi_smf_util: the MIDI util
 *
 * Copy MIDI util.
 *
 * Returns: (transfer full): the newly allocated #AgsMidiSmfUtil-struct
 * 
 * Since: 6.0.0
 */
AgsMidiSmfUtil*
ags_midi_smf_util_copy(AgsMidiSmfUtil *midi_smf_util)
{
  AgsMidiSmfUtil *ptr;

  g_return_val_if_fail(midi_smf_util != NULL, NULL);

  ptr = ags_midi_smf_util_alloc();

  ptr->major = midi_smf_util->major;
  ptr->minor = midi_smf_util->minor;
  
  return(ptr);
}

/**
 * ags_midi_smf_util_get_varlength_size:
 * @midi_smf_util: the MIDI util
 * @varlength: the variable length
 *
 * Retrieve the size needed to store the variable length.
 *
 * Returns: the size needed to be allocated
 *
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_varlength_size(AgsMidiSmfUtil *midi_smf_util,
				     gint varlength)
{
  guint current;
  guchar c;
  guint i;
  gint mask;
  
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
 * ags_midi_smf_util_put_varlength:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @varlength: the value to put
 *
 * Put the variable lenght value to @buffer.
 *
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_varlength(AgsMidiSmfUtil *midi_smf_util,
				guchar *buffer,
				gint varlength)
{
  guint varlength_size;
  guint i, j;
  gint mask;

  if(buffer == NULL){
    return;
  }

  varlength_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, varlength);

  /* write to internal buffer */
  mask = 0x7f;
  j = 0;
  i = 7 * varlength_size;

  for(; j < varlength_size; ){
    buffer[j] = ((mask << (i - 7)) & varlength) >> (i - 7);

    if(j + 1 < varlength_size){
      buffer[j] |= 0x80;
    }
    
    i -= 7;
    j++;
  }
}

/**
 * ags_midi_smf_util_get_varlength:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @varlength: the return location
 *
 * Get the variable lenght value from @buffer.
 *
 * Returns: the number of bytes read.
 *
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_varlength(AgsMidiSmfUtil *midi_smf_util,
				guchar *buffer,
				gint *varlength)
{
  gint value;
  guint i;
  char c;

  if(buffer == NULL){
    if(varlength != NULL){
      *varlength = 0;
    }
    
    return(0);
  }

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
 * ags_midi_smf_util_put_int16:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @val: the integer
 * 
 * Put signed 16 bit integer.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_int16(AgsMidiSmfUtil *midi_smf_util,
			    guchar *buffer,
			    gint val)
{
  if(buffer == NULL){
    return;
  }

  buffer[0] = (val & (0xff << 8)) >> 8;
  buffer[1] = val & 0xff;
}

/**
 * ags_midi_smf_util_get_int16:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @val: return location of the integer
 *
 * Get signed 32 bit integer.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_get_int16(AgsMidiSmfUtil *midi_smf_util,
			    guchar *buffer,
			    gint *val)
{
  gint tmp;

  if(buffer == NULL){
    if(val != NULL){
      *val = 0;
    }
    
    return;
  }

  tmp = (buffer[0] & 0xff);
  tmp = (tmp << 8) + (buffer[1] & 0xff);

  if(val != NULL){
    *val = tmp;
  }
}

/**
 * ags_midi_smf_util_put_int24:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @val: the integer
 * 
 * Put signed 24 bit integer.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_int24(AgsMidiSmfUtil *midi_smf_util,
			    guchar *buffer,
			    gint val)
{
  if(buffer == NULL){
    return;
  }
  
  buffer[0] = (val & (0xff << 16)) >> 16;
  buffer[1] = (val & (0xff << 8)) >> 8;
  buffer[2] = val & 0xff;
}

/**
 * ags_midi_smf_util_get_int24:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @val: return location of the integer
 *
 * Get signed 24 bit integer.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_get_int24(AgsMidiSmfUtil *midi_smf_util,
			    guchar *buffer,
			    gint *val)
{
  gint tmp;

  if(buffer == NULL){
    if(val != NULL){
      *val = 0;
    }

    return;
  }
  
  tmp = (buffer[0] & 0xff);
  tmp = (tmp << 8) + (buffer[1] & 0xff);
  tmp = (tmp << 8) + (buffer[2] & 0xff);

  if(val != NULL){
    *val = tmp;
  }
}

/**
 * ags_midi_smf_util_put_int32:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @val: the integer
 * 
 * Put signed 32 bit integer.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_int32(AgsMidiSmfUtil *midi_smf_util,
			    guchar *buffer,
			    gint val)
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
 * ags_midi_smf_util_get_int32:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @val: return location of the integer
 *
 * Get signed 32 bit integer.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_get_int32(AgsMidiSmfUtil *midi_smf_util,
			    guchar *buffer,
			    gint *val)
{
  gint tmp;

  if(buffer == NULL){
    if(val != NULL){
      *val = 0;
    }

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
 * ags_midi_smf_util_put_header:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @offset: start delta-time
 * @format: either 0, 1 or 2.
 * @track_count: the number of tracks
 * @division: timing division
 *
 * Puts the midi header.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_header(AgsMidiSmfUtil *midi_smf_util,
			     guchar *buffer,
			     gint offset, gint format,
			     gint track_count, gint division)
{
  static gchar header[] = "MThd";

  if(buffer == NULL){
    return;
  }
  
  /* put MThd */
  memcpy(buffer, header, 4 * sizeof(guchar));

  /* chunk length */
  ags_midi_smf_util_put_int32(midi_smf_util, buffer + 4,
			      offset);

  /* format */
  ags_midi_smf_util_put_int16(midi_smf_util, buffer + 8,
			      format);

  /* track count */
  ags_midi_smf_util_put_int16(midi_smf_util, buffer + 10,
			      track_count);

  /* division */
  ags_midi_smf_util_put_int16(midi_smf_util, buffer + 12,
			      division);  
}

/**
 * ags_midi_smf_util_get_header:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @offset: start delta-time
 * @format: either 0, 1 or 2.
 * @track_count: the number of tracks
 * @division: timing division
 * 
 * Gets the midi header
 * 
 * Returns: the number of bytes read.
 *
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_header(AgsMidiSmfUtil *midi_smf_util,
			     guchar *buffer,
			     gint *offset, gint *format,
			     gint *track_count, gint *division)
{
  static gchar header[] = "MThd";

  if(buffer == NULL ||
     (!g_ascii_strncasecmp(buffer,
			   header,
			   4)) == FALSE){
    if(offset != NULL){
      *offset = 0;
    }
    
    if(format != NULL){
      *format = 0;
    }

    if(track_count != NULL){
      *track_count = 0;
    }

    if(division != NULL){
      *division = 0;
    }
    
    return(0);
  }
  
  /* offset */
  ags_midi_smf_util_get_int32(midi_smf_util, buffer + 4,
			      offset);

  /* format */
  ags_midi_smf_util_get_int16(midi_smf_util, buffer + 8,
			      format);

  /* track count */
  ags_midi_smf_util_get_int16(midi_smf_util, buffer + 10,
			      track_count);

  /* division */
  ags_midi_smf_util_get_int16(midi_smf_util, buffer + 12,
			      division);

  return(14);
}

/**
 * ags_midi_smf_util_put_track:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @offset: start delta-time
 * 
 * Put track.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_track(AgsMidiSmfUtil *midi_smf_util,
			    guchar *buffer,
			    gint offset)
{
  static gchar track[] = "MTrk";

  if(buffer == NULL){
    return;
  }
  
  /* put MTrk */
  memcpy(buffer, track, 4 * sizeof(guchar));

  /* offset */
  ags_midi_smf_util_put_int32(midi_smf_util, buffer + 4,
			      offset);  
}

/**
 * ags_midi_smf_util_get_track:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @offset: start delta-time
 * 
 * Get track
 * 
 * Returns: the number of bytes read.
 *
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_track(AgsMidiSmfUtil *midi_smf_util,
			    guchar *buffer,
			    gint *offset)
{
  static gchar track[] = "MTrk";

  if(buffer == NULL ||
     (!g_ascii_strncasecmp(buffer,
			   track,
			   4)) == FALSE){
    if(offset != NULL){
      *offset = 0;
    }

    return(0);
  }

  /* offset */
  ags_midi_smf_util_get_int32(midi_smf_util, buffer + 4,
			      offset);

  return(8);
}

/**
 * ags_midi_smf_util_put_key_on:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * @channel: valid channels from 0-15
 * @key: valid keys to play 0-128
 * @velocity: the key dynamics
 * 
 * Puts the given values to @buffer with appropriate channel message.
 *
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_key_on(AgsMidiSmfUtil *midi_smf_util,
			     guchar *buffer,
			     gint delta_time,
			     gint channel,
			     gint key,
			     gint velocity)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
				  delta_time);

  /* key-on channel message */
  buffer[delta_time_size] = 0x90 | (channel & 0xf);

  /* key */
  buffer[delta_time_size + 1] = key & 0x7f;

  /* velocity */
  buffer[delta_time_size + 2] = velocity & 0x7f;
}

/**
 * ags_midi_smf_util_get_key_on:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * @channel: the return location of channel
 * @key: the return location of key
 * @velocity: the return location of velocity
 *
 * Get the key-on message from @buffer.
 *
 * Returns: the number of bytes read.
 *
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_key_on(AgsMidiSmfUtil *midi_smf_util,
			     guchar *buffer,
			     gint *delta_time,
			     gint *channel,
			     gint *key,
			     gint *velocity)
{
  gint val;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    if(channel != NULL){
      *channel = 0;
    }

    if(key != NULL){
      *key = 0;
    }

    if(velocity != NULL){
      *velocity = 0;
    }
    
    return(0);
  }

  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
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
 * ags_midi_smf_util_put_key_off:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * @channel: valid channels from 0-15
 * @key: valid keys to play 0-128
 * @velocity: the key dynamics
 * 
 * Puts the given values to @buffer with appropriate channel message.
 *
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_key_off(AgsMidiSmfUtil *midi_smf_util,
			      guchar *buffer,
			      gint delta_time,
			      gint channel,
			      gint key,
			      gint velocity)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
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
 * ags_midi_smf_util_get_key_off:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * @channel: the return location of channel
 * @key: the return location of key
 * @velocity: the return location of velocity
 *
 * Get the key-off message from @buffer.
 *
 * Returns: the number of bytes read.
 *
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_key_off(AgsMidiSmfUtil *midi_smf_util,
			      guchar *buffer,
			      gint *delta_time,
			      gint *channel,
			      gint *key,
			      gint *velocity)
{
  gint val;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    if(channel != NULL){
      *channel = 0;
    }

    if(key != NULL){
      *key = 0;
    }

    if(velocity != NULL){
      *velocity = 0;
    }

    return(0);
  }

  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
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
 * ags_midi_smf_util_put_key_pressure:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * @channel: valid channels from 0-15
 * @key: valid keys to play 0-128
 * @pressure: the key dynamics
 * 
 * Puts the given values to @buffer with appropriate channel message.
 *
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_key_pressure(AgsMidiSmfUtil *midi_smf_util,
				   guchar *buffer,
				   gint delta_time,
				   gint channel,
				   gint key,
				   gint pressure)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
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
 * ags_midi_smf_util_get_key_pressure:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * @channel: the return location of channel
 * @key: the return location of key
 * @pressure: the return location of pressure
 *
 * Get the key-pressure message from @buffer.
 *
 * Returns: the number of bytes read.
 *
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_key_pressure(AgsMidiSmfUtil *midi_smf_util,
				   guchar *buffer,
				   gint *delta_time,
				   gint *channel,
				   gint *key,
				   gint *pressure)
{
  gint val;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    if(channel != NULL){
      *channel = 0;
    }

    if(key != NULL){
      *key = 0;
    }

    if(pressure != NULL){
      *pressure = 0;
    }

    return(0);
  }

  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
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
 * ags_midi_smf_util_put_change_parameter:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * @channel: channel
 * @control: the control
 * @value: the value
 * 
 * Put change parameter.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_change_parameter(AgsMidiSmfUtil *midi_smf_util,
				       guchar *buffer,
				       gint delta_time,
				       gint channel,
				       gint control,
				       gint value)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
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
 * ags_midi_smf_util_get_change_parameter:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * @channel: the return location of channel
 * @control: the return location of the control
 * @value: the return location the value
 *
 * Get change parameter.
 * 
 * Returns: the number of bytes read.
 *
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_change_parameter(AgsMidiSmfUtil *midi_smf_util,
				       guchar *buffer,
				       gint *delta_time,
				       gint *channel,
				       gint *control,
				       gint *value)
{
  gint val;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    if(channel != NULL){
      *channel = 0;
    }

    if(control != NULL){
      *control = 0;
    }

    if(value != NULL){
      *value = 0;
    }

    return(0);
  }

  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
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
 * ags_midi_smf_util_put_pitch_bend:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * @channel: channel
 * @pitch: the pitch
 * @transmitter: the transmitter
 * 
 * Put pitch bend.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_pitch_bend(AgsMidiSmfUtil *midi_smf_util,
				 guchar *buffer,
				 gint delta_time,
				 gint channel,
				 gint pitch,
				 gint transmitter)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
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
 * ags_midi_smf_util_get_pitch_bend:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * @channel: the return location of channel
 * @pitch: the return location of the pitch
 * @transmitter: the return location the transmitter
 * 
 * Get pitch bend.
 * 
 * Returns: the number of bytes read.
 *
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_pitch_bend(AgsMidiSmfUtil *midi_smf_util,
				 guchar *buffer,
				 gint *delta_time,
				 gint *channel,
				 gint *pitch,
				 gint *transmitter)
{
  gint val;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    if(channel != NULL){
      *channel = 0;
    }

    if(pitch != NULL){
      *pitch = 0;
    }

    if(transmitter != NULL){
      *transmitter = 0;
    }

    return(0);
  }

  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
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
 * ags_midi_smf_util_put_change_program:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * @channel: channel
 * @program: the program
 * 
 * Put change program.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_change_program(AgsMidiSmfUtil *midi_smf_util,
				     guchar *buffer,
				     gint delta_time,
				     gint channel,
				     gint program)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
				  delta_time);

  /* change-parameter channel message */
  buffer[delta_time_size] = 0xc0;
  buffer[delta_time_size] |= (channel & 0xf);

  /* program */
  buffer[delta_time_size + 1] = 0x7f & program;
}

/**
 * ags_midi_smf_util_get_change_program:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * @channel: the return location of channel
 * @program: the return location of the program
 * 
 * Get change program.
 * 
 * Returns: the number of bytes read.
 *
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_change_program(AgsMidiSmfUtil *midi_smf_util,
				     guchar *buffer,
				     gint *delta_time,
				     gint *channel,
				     gint *program)
{
  gint val;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    if(channel != NULL){
      *channel = 0;
    }

    if(program != NULL){
      *program = 0;
    }

    return(0);
  }

  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
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
 * ags_midi_smf_util_put_change_pressure:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * @channel: channel
 * @pressure: the pressure
 *
 * Put change pressure.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_change_pressure(AgsMidiSmfUtil *midi_smf_util,
				      guchar *buffer,
				      gint delta_time,
				      gint channel,
				      gint pressure)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
				  delta_time);

  /* change-parameter channel message */
  buffer[delta_time_size] = 0xd0;
  buffer[delta_time_size] |= (channel & 0xf);

  /* pressure */
  buffer[delta_time_size + 1] = 0x7f & pressure;
}

/**
 * ags_midi_smf_util_get_change_pressure:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * @channel: the return location of channel
 * @pressure: the return location of the pressure
 * 
 * Get change pressure.
 * 
 * Returns: the number of bytes read.
 *
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_change_pressure(AgsMidiSmfUtil *midi_smf_util,
				      guchar *buffer,
				      gint *delta_time,
				      gint *channel,
				      gint *pressure)
{
  gint val;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    if(channel != NULL){
      *channel = 0;
    }

    if(pressure != NULL){
      *pressure = 0;
    }

    return(0);
  }

  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
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
 * ags_midi_smf_util_put_sysex:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * @data: the data
 * @length: the data's length
 * 
 * Put sysex.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_sysex(AgsMidiSmfUtil *midi_smf_util,
			    guchar *buffer,
			    gint delta_time,
			    guchar *data, gint length)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
				  delta_time);

  /* status byte */
  buffer[delta_time_size] = 0xf0;

  /* put data */  
  memcpy(buffer + delta_time_size + 1, data, length * sizeof(guchar));

  /* EOX end of sysex */
  buffer[delta_time_size + length + 1] = 0xf7;
}

/**
 * ags_midi_smf_util_get_sysex:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * @data: the return location of data
 * @length: the return location of length
 * 
 * Get sysex.
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_sysex(AgsMidiSmfUtil *midi_smf_util,
			    guchar *buffer,
			    gint *delta_time,
			    guchar **data, gint *length)
{
  guchar *tmp_data;

  gint val;
  guint delta_time_size;
  guint i;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    if(data != NULL){
      *data = NULL;
    }

    if(length != NULL){
      *length = 0;
    }

    return(0);
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
						    &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  for(i = 0; buffer[delta_time_size + i + 1] != 0xf7; i++);

  if(data != NULL){
    if(i > 0){
      tmp_data = (guchar *) malloc(i * sizeof(guchar));
    }else{
      tmp_data = NULL;
    }

    memcpy(tmp_data, buffer + delta_time_size + 1, i * sizeof(guchar));

    *data = tmp_data;
  }

  if(length != NULL){
    *length = i;
  }

  return(delta_time_size + i + 2);
}

/**
 * ags_midi_smf_util_put_quarter_frame:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * @message_type: the message type
 * @values: the values
 * 
 * Put quarter frame.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_quarter_frame(AgsMidiSmfUtil *midi_smf_util,
				    guchar *buffer,
				    gint delta_time,
				    gint message_type,
				    gint values)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }

  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
				  delta_time);

  /* status byte */
  buffer[delta_time_size] = 0xf1;

  /* message type */
  buffer[delta_time_size + 1] = 0x70 & message_type;

  /* values */
  buffer[delta_time_size + 1] |= 0x0f & values;
}

/**
 * ags_midi_smf_util_get_quarter_frame:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * @message_type: the return location of the message type
 * @values: the return location of the values
 * 
 * Get quarter frame.
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_quarter_frame(AgsMidiSmfUtil *midi_smf_util,
				    guchar *buffer,
				    gint *delta_time,
				    gint *message_type, gint *values)
{
  gint val;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    if(message_type != NULL){
      *message_type = 0;
    }

    if(values != NULL){
      *values = 0;
    }

    return(0);
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
						    &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  if(message_type != NULL){
    *message_type = 0x70 & buffer[delta_time_size + 1];
  }

  if(values != NULL){
    *values = 0x0f & buffer[delta_time_size + 1];
  }

  return(delta_time_size + 2);
}

/**
 * ags_midi_smf_util_put_song_position:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * @song_position: the song position
 * 
 * Put song position.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_song_position(AgsMidiSmfUtil *midi_smf_util,
				    guchar *buffer,
				    gint delta_time,
				    gint song_position)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
				  delta_time);

  /* status byte */
  buffer[delta_time_size] = 0xf2;

  /* song position */
  buffer[delta_time_size + 1] = 0x7f & song_position;
  buffer[delta_time_size + 2] = 0x7f & (song_position >> 7);
}

/**
 * ags_midi_smf_util_get_song_position:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * @song_position: the return location of the song position
 * 
 * Get song position.
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_song_position(AgsMidiSmfUtil *midi_smf_util,
				    guchar *buffer,
				    gint *delta_time,
				    gint *song_position)
{
  gint val;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    if(song_position != NULL){
      *song_position = 0;
    }

    return(0);
  }

  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
						    &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  /* song position */
  if(song_position != NULL){
    *song_position = 0x7f & buffer[delta_time_size + 1];
    *song_position |= ((0x7f & buffer[delta_time_size + 2]) << 7);
  }
  
  return(delta_time_size + 3);
}

/**
 * ags_midi_smf_util_put_song_select:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * @song_select: the song select
 * 
 * Put song select.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_song_select(AgsMidiSmfUtil *midi_smf_util,
				  guchar *buffer,
				  gint delta_time,
				  gint song_select)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
				  delta_time);

  /* status byte */
  buffer[delta_time_size] = 0xf3;

  /* song select */
  buffer[delta_time_size + 1] = 0x7f & song_select;
}

/**
 * ags_midi_smf_util_get_song_select:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * @song_select: the return location of the song select
 * 
 * Get song select.
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_song_select(AgsMidiSmfUtil *midi_smf_util,
				  guchar *buffer,
				  gint *delta_time,
				  gint *song_select)
{
  gint val;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    if(song_select != NULL){
      *song_select = 0;
    }

    return(0);
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
						    &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  if(song_select != NULL){
    *song_select = 0x7f & buffer[delta_time_size + 1];
  }

  return(delta_time_size + 2);
}

/**
 * ags_midi_smf_util_put_tune_request:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * 
 * Put tune request
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_tune_request(AgsMidiSmfUtil *midi_smf_util,
				   guchar *buffer,
				   gint delta_time)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
				  delta_time);
  
  /* status byte */
  buffer[delta_time_size] = 0xf6;
}

/**
 * ags_midi_smf_util_get_tune_request:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * 
 * Get tune request.
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_tune_request(AgsMidiSmfUtil *midi_smf_util,
				   guchar *buffer,
				   gint *delta_time)
{
  gint val;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    return(0);
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
						    &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  return(delta_time_size + 1);
}

/**
 * ags_midi_smf_util_put_sequence_number:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * @sequence: the sequence
 * 
 * Put sequence number.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_sequence_number(AgsMidiSmfUtil *midi_smf_util,
				      guchar *buffer,
				      gint delta_time,
				      gint sequence)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
				  delta_time);

  /* status byte */
  buffer[delta_time_size] = 0xff;

  /* type */
  buffer[delta_time_size + 1] = 0x00;

  /* length */
  buffer[delta_time_size + 2] = 2;

  /* data */
  buffer[delta_time_size + 3] = ((0xff00 & sequence) >> 8);
  buffer[delta_time_size + 4] = 0xff & sequence;
}

/**
 * ags_midi_smf_util_get_sequence_number:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * @sequence: the return location of the sequence
 * 
 * Get sequence number.
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_sequence_number(AgsMidiSmfUtil *midi_smf_util,
				      guchar *buffer,
				      gint *delta_time,
				      gint *sequence)
{
  gint val;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    if(sequence != NULL){
      *sequence = 0;
    }

    return(0);
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
						    &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  /* sequence */
  if(sequence != NULL){
    *sequence = buffer[delta_time_size + 3] << 8;
    *sequence |= buffer[delta_time_size + 4];
  }
  
  return(delta_time_size + 5);
}

/**
 * ags_midi_smf_util_put_smtpe:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * @rr: frame rate
 * @hr: hour
 * @mn: minute
 * @se: second
 * @fr: frame number
 * 
 * Put smtpe timestamp.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_smtpe(AgsMidiSmfUtil *midi_smf_util,
			    guchar *buffer,
			    gint delta_time,
			    gint rr, gint hr, gint mn, gint se, gint fr)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
				  delta_time);

  /* status byte */
  buffer[delta_time_size] = 0xff;

  /* type */
  buffer[delta_time_size + 1] = 0x54;

  /* length */
  buffer[delta_time_size + 2] = 5;

  /* rr */
  buffer[delta_time_size + 3] = rr;
  
  /* hr */
  buffer[delta_time_size + 3] |= hr;

  /* mn */
  buffer[delta_time_size + 4] = mn;
  
  /* se */
  buffer[delta_time_size + 5] = se;

  /* fr */
  buffer[delta_time_size + 6] = fr;
}

/**
 * ags_midi_smf_util_get_smtpe:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * @rr: the return location of frame rate
 * @hr: the return location of hour
 * @mn: the return location of minute
 * @se: the return location of second
 * @fr: the return location of frame number
 * 
 * Get smtpe timestamp.
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_smtpe(AgsMidiSmfUtil *midi_smf_util,
			    guchar *buffer,
			    gint *delta_time,
			    gint *rr, gint *hr, gint *mn, gint *se, gint *fr)
{
  gint val;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    if(rr != NULL){
      *rr = 0;
    }

    if(hr != NULL){
      *hr = 0;
    }

    if(mn != NULL){
      *mn = 0;
    }

    if(se != NULL){
      *se = 0;
    }

    if(fr != NULL){
      *fr = 0;
    }

    return(0);
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
						    &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  /* rr */
  if(rr != NULL){
    *rr = 0xc0 & buffer[delta_time_size + 3];
  }

  /* hr */
  if(hr != NULL){
    *hr = 0x3f & buffer[delta_time_size + 3];
  }
  
  /* mn */
  if(mn != NULL){
    *mn = buffer[delta_time_size + 4];
  }
  
  /* se */
  if(se != NULL){
    *se = buffer[delta_time_size + 5];
  }
  
  /* fr */
  if(fr != NULL){
    *fr = buffer[delta_time_size + 6];
  }
  
  return(delta_time_size + 7);
}

/**
 * ags_midi_smf_util_put_tempo:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * @tempo: the tempo
 * 
 * Put tempo.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_tempo(AgsMidiSmfUtil *midi_smf_util,
			    guchar *buffer,
			    gint delta_time,
			    gint tempo)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
				  delta_time);

  /* status byte */
  buffer[delta_time_size] = 0xff;

  /* type */
  buffer[delta_time_size + 1] = 0x51;

  /* length */
  buffer[delta_time_size + 2] = 0x03;
  
  /* tempo */
  ags_midi_smf_util_put_int24(midi_smf_util, buffer + delta_time_size + 3,
			      tempo);
}

/**
 * ags_midi_smf_util_get_tempo:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * @tempo: the tempo
 * 
 * Get tempo.
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_tempo(AgsMidiSmfUtil *midi_smf_util,
			    guchar *buffer,
			    gint *delta_time,
			    gint *tempo)
{
  gint val;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    if(tempo != NULL){
      *tempo = 0;
    }

    return(0);
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
						    &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  /* tempo */
  ags_midi_smf_util_get_int24(midi_smf_util, buffer + delta_time_size + 3,
			      tempo);

  return(delta_time_size + 6);
}

/**
 * ags_midi_smf_util_put_time_signature:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * @nn: numerator
 * @dd: denominator
 * @cc: clocks
 * @bb: beats
 * 
 * Put time signature
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_time_signature(AgsMidiSmfUtil *midi_smf_util,
				     guchar *buffer,
				     gint delta_time,
				     gint nn, gint dd, gint cc, gint bb)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
				  delta_time);

  /* status byte */
  buffer[delta_time_size] = 0xff;

  /* type */
  buffer[delta_time_size + 1] = 0x58;

  /* length */
  buffer[delta_time_size + 2] = 0x4;

  /* nn */
  buffer[delta_time_size + 3] = nn;
  
  /* dd */
  buffer[delta_time_size + 4] = dd;

  /* cc */
  buffer[delta_time_size + 5] = cc;

  /* bb */
  buffer[delta_time_size + 6] = bb;
}

/**
 * ags_midi_smf_util_get_time_signature:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * @nn: the return location of numerator
 * @dd: the return location of denominator
 * @cc: the return location of clocks
 * @bb: the return location of beats
 * 
 * Get time signature.
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_time_signature(AgsMidiSmfUtil *midi_smf_util,
				     guchar *buffer,
				     gint *delta_time,
				     gint *nn, gint *dd, gint *cc, gint *bb)
{
  gint val;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    if(nn != NULL){
      *nn = 0;
    }

    if(dd != NULL){
      *dd = 0;
    }

    if(cc != NULL){
      *cc = 0;
    }

    if(bb != NULL){
      *bb = 0;
    }

    return(0);
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
						    &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  /* nn */
  if(nn != NULL){
    *nn = buffer[delta_time_size + 3];
  }
  
  /* dd */
  if(dd != NULL){
    *dd = buffer[delta_time_size + 4];
  }

  /* cc */
  if(cc != NULL){
    *cc = buffer[delta_time_size + 5];
  }

  /* bb */
  if(bb != NULL){
    *bb = buffer[delta_time_size + 6];
  }
  
  return(delta_time_size + 7);
}

/**
 * ags_midi_smf_util_put_key_signature:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * @sf: flats or sharps
 * @mi: 1 equals minor or 0 means major
 * 
 * Put key signature.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_key_signature(AgsMidiSmfUtil *midi_smf_util,
				    guchar *buffer,
				    gint delta_time,
				    gint sf, gint mi)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
				  delta_time);
  
  /* status byte */
  buffer[delta_time_size] = 0xff;

  /* type */
  buffer[delta_time_size + 1] = 0x59;

  /* length */
  buffer[delta_time_size + 2] = 2;

  /* sf */
  buffer[delta_time_size + 3] = sf;
  
  /* mi */
  buffer[delta_time_size + 4] = mi;
}

/**
 * ags_midi_smf_util_get_key_signature:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * @sf: the return location of flats or sharps
 * @mi: the return location of minor or major
 * 
 * Get key signature.
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_key_signature(AgsMidiSmfUtil *midi_smf_util,
				    guchar *buffer,
				    gint *delta_time,
				    gint *sf, gint *mi)
{
  gint val;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    if(sf != NULL){
      *sf = 0;
    }

    if(mi != NULL){
      *mi = 0;
    }

    return(0);
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
						    &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  /* sf */
  if(sf != NULL){
    *sf = buffer[delta_time_size + 3];
  }

  /* mi */
  if(mi != NULL){
    *mi = buffer[delta_time_size + 4];
  }

  return(delta_time_size + 5);
}

/**
 * ags_midi_smf_util_put_sequencer_meta_event:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * @len: the length of data
 * @id: the manufacturer id
 * @data: the data
 * 
 * Put sequencer meta event.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_sequencer_meta_event(AgsMidiSmfUtil *midi_smf_util,
					   guchar *buffer,
					   gint delta_time,
					   gint len, gint id, gint data)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
				  delta_time);

  /* status byte */
  buffer[delta_time_size] = 0xff;

  /* type */
  buffer[delta_time_size + 1] = 0x7f;

  /* length */
  buffer[delta_time_size + 2] = 0xff & len;

  /* id */
  buffer[delta_time_size + 3] = 0xff & id;
  
  /* data */
  switch(len){
  case 3:
    {
      buffer[delta_time_size + 6] = ((0xff << 16) & data) >> 16;
    }
  case 2:
    {
      buffer[delta_time_size + 4] = 0xff & data;
      buffer[delta_time_size + 5] = ((0xff << 8) & data) >> 8;
    }
  }
}

/**
 * ags_midi_smf_util_get_sequencer_meta_event:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * @len: the return location of the length of data
 * @id: the return location of the manufacturer id
 * @data: the return location of the data
 * 
 * Get sequencer meta event.
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_sequencer_meta_event(AgsMidiSmfUtil *midi_smf_util,
					   guchar *buffer,
					   gint *delta_time,
					   gint *len, gint *id, gint *data)
{
  gint val;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    if(len != NULL){
      *len = 0;
    }

    if(id != NULL){
      *id = 0;
    }

    if(data != NULL){
      *data = 0;
    }

    return(0);
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
						    &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  if(len != NULL){
    len[0] = buffer[delta_time_size + 2];
  }

  if(id != NULL){
    *id = buffer[delta_time_size + 3];
  }

  if(data != NULL){
    *data = 0;
    
    switch(buffer[delta_time_size + 2]){
    case 3:
      {
	*data |= (buffer[delta_time_size + 6] << 16);
      }
    case 2:
      {
	*data |= buffer[delta_time_size + 4];
	*data |= (buffer[delta_time_size + 5] << 8);
      }
    }
  }

  return(delta_time_size + buffer[delta_time_size + 2] + 3);
}

/**
 * ags_midi_smf_util_put_text_event:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * @text: the text
 * @length: the length
 * 
 * Put text event.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_text_event(AgsMidiSmfUtil *midi_smf_util,
				 guchar *buffer,
				 gint delta_time,
				 gchar *text, gint length)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
				  delta_time);
  
  /* status byte */
  buffer[delta_time_size] = 0xff;

  /* type */
  buffer[delta_time_size + 1] = 0x01;

  /* length */
  buffer[delta_time_size + 2] = 0xff & length;

  /* text */
  memcpy(buffer + delta_time_size + 3, text, length * sizeof(guchar));
}

/**
 * ags_midi_smf_util_get_text_event:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * @text: the return location of the text
 * @length: the return location of the length
 * 
 * Get text event.
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_text_event(AgsMidiSmfUtil *midi_smf_util,
				 guchar *buffer,
				 gint *delta_time,
				 gchar **text, gint *length)
{
  gint val;
  guint text_size;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    if(text != NULL){
      *text = NULL;
    }

    if(length != NULL){
      *length = 0;
    }
    
    return(0);
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
						    &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  /* length */
  text_size = 0x7f & buffer[delta_time_size + 2];
  
  if(length != NULL){
    *length = text_size;
  }
  
  /* text */
  if(text != NULL){
    text[0] = (gchar *) malloc(text_size * sizeof(gchar));
    memcpy(text[0], buffer + delta_time_size + 3, text_size * sizeof(gchar));
  }

  return(delta_time_size + text_size + 3);
}

/**
 * ags_midi_smf_util_put_end_of_track:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: timing information
 * 
 * Put end of track.
 * 
 * Since: 6.0.0
 */
void
ags_midi_smf_util_put_end_of_track(AgsMidiSmfUtil *midi_smf_util,
				   guchar *buffer,
				   gint delta_time)
{
  guint delta_time_size;

  if(buffer == NULL){
    return;
  }
  
  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength_size(midi_smf_util, delta_time);
  ags_midi_smf_util_put_varlength(midi_smf_util, buffer,
				  delta_time);

  /* status byte */
  buffer[delta_time_size] = 0xff;

  /* type */
  buffer[delta_time_size + 1] = 0x2f;

  /* length */
  buffer[delta_time_size + 2] = 0;
}

/**
 * ags_midi_smf_util_get_end_of_track:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @delta_time: the return location of timing information
 * 
 * Get end of track. 
 * 
 * Returns: the number of bytes read.
 * 
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_get_end_of_track(AgsMidiSmfUtil *midi_smf_util,
				   guchar *buffer,
				   gint *delta_time)
{
  gint val;
  guint delta_time_size;
  
  if(buffer == NULL){
    if(delta_time != NULL){
      *delta_time = 0;
    }

    return(0);
  }

  /* delta time */
  delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, buffer,
						    &val);
  
  if(delta_time != NULL){
    *delta_time = val;
  }

  return(delta_time_size + 3);
}

/**
 * ags_midi_smf_util_seek_message:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the buffer to seek
 * @message_count: seek count messages
 * @delta_time: the return location of current delta time
 * 
 * Seek MIDI messages from @buffer
 * 
 * Returns: the buffer at offset at @message_count ahead
 * 
 * Since: 6.0.0
 */
guchar*
ags_midi_smf_util_seek_message(AgsMidiSmfUtil *midi_smf_util,
			       guchar *buffer,
			       guint message_count,
			       gint *delta_time)
{
  static const gchar header[] = "MThd";
  static const gchar track[] = "MTrk";

  guchar *offset;
  
  gint current_delta_time, tmp_delta_time;
  gint next_delta_time;
  guint delta_time_size;
  guchar status, prev_status;
  guchar meta_type;
  guint n;
  guint i;
  gboolean initial_run;
  
  if(buffer == NULL){
    return(NULL);
  }

  offset = buffer;
  
  /* check for header */
  if(!g_ascii_strncasecmp(offset,
			  header,
			  4)){
    offset += 14;
  }

  /* seek message count */
  current_delta_time = 0;
  initial_run = TRUE;

  prev_status = 0;
  
  for(i = 0; i < message_count; i++){
    /* check for track */
    if(initial_run){
      if(!g_ascii_strncasecmp(offset,
			      track,
			      4)){
	offset += 8;
      }

      initial_run = FALSE;
    }

    /* read delta time */
    tmp_delta_time = 0;
    
    delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, offset,
						      &tmp_delta_time);

    if(delta_time_size > 0){
      current_delta_time = tmp_delta_time;
    }
    
    /* read status byte */
    status = offset[delta_time_size];

    n = 1;
  ags_midi_smf_util_seek_message_REPEAT_STATUS:
    
    if((0xf0 & status) != 0xf0){
      switch(status & 0xf0){
      case 0x80:
	{
	  n += delta_time_size + 2;

	  prev_status = status;
	}
	break;
      case 0x90:
	{
	  n += delta_time_size + 2;

	  prev_status = status;
	}
	break;
      case 0xa0:
	{
	  n += delta_time_size + 2;

	  prev_status = status;
	}
	break;
      case 0xb0:
	{
	  n += delta_time_size + 2;

	  prev_status = status;
	}
	break;
      case 0xc0:
	{
	  n += delta_time_size + 1;

	  prev_status = status;
	}
	break;
      case 0xd0:
	{
	  n += delta_time_size + 1;

	  prev_status = status;
	}
	break;
      case 0xe0:
	{
	  n += delta_time_size + 2;

	  prev_status = status;
	}
	break;
      default:
	{
#ifdef AGS_DEBUG
	  g_message("repeat status=0x%x", prev_status);
#endif
	  status = prev_status;
	  
	  goto ags_midi_smf_util_seek_message_REPEAT_STATUS;
	}
      }

      offset += n;

      /* check if status is omitted */
      next_delta_time = 0;
      
      delta_time_size = ags_midi_smf_util_get_varlength(midi_smf_util, offset,
							&next_delta_time);
      
      if((0xf0 & offset[delta_time_size]) != 0xf0){
	switch(status & 0xf0){
	case 0x80:
	case 0x90:
	case 0xa0:
	case 0xb0:
	case 0xc0:
	case 0xd0:
	case 0xe0:
	  break;
	default:
	  {
	    n = 0;
	    current_delta_time = next_delta_time;
	    
	    goto ags_midi_smf_util_seek_message_REPEAT_STATUS;
	  }
	}
      }      
    }else{      
      status = offset[delta_time_size];

      switch(status){
      case 0xf0:
	{
	  /* start of system exclusive */
	  n = ags_midi_smf_util_get_sysex(NULL,
					  offset,
					  NULL,
					  NULL, NULL);

	  offset += n;
	}
	break;
      case 0xf1:
	{
	  /* quarter frame */
	  n = ags_midi_smf_util_get_quarter_frame(NULL,
						  offset,
						  NULL,
						  NULL, NULL);

	  offset += n;
	}
	break;
      case 0xf2:
	{
	  /* song position */
	  n = ags_midi_smf_util_get_song_position(NULL,
						  offset,
						  NULL,
						  NULL);
	  
	  offset += n;
	}
	break;
      case 0xf3:
	{
	  /* song select */
	  n = ags_midi_smf_util_get_song_select(NULL,
						offset,
						NULL,
						NULL);
	  
	  offset += n;
	}
	break;
      case 0xf4:
      case 0xf5:
	{
#ifdef AGS_DEBUG
	  g_message("undefined");
#endif
	  
	  offset += delta_time_size;
	}
	break;
      case 0xf6:
	{
	  /* tune request */
	  n = ags_midi_smf_util_get_tune_request(NULL,
						 offset,
						 NULL);
	  
	  offset += n;
	}
	break;
      case 0xf7:
	{
	  /* sysex continuation or arbitrary stuff */
#ifdef AGS_DEBUG
	  g_message("sysex end");
#endif
	}
	break;
      case 0xff:
	{
	  guint meta_type;

	  meta_type = offset[delta_time_size + 1];


      
	  switch(meta_type){
	  case 0x00:
	    {
	      int c;

	      //	  c = offset[delta_time_size + 1];

	      //	  if(c == 0x02){
	      n = ags_midi_smf_util_get_sequence_number(NULL,
							offset,
							NULL,
							NULL);
	  
	      offset += n;
	      //	  }
	    }
	    break;
	  case 0x01:      /* Text event */
	  case 0x02:      /* Copyright notice */
	  case 0x03:      /* Sequence/Track name */
	  case 0x04:      /* Instrument name */
	  case 0x05:      /* Lyric */
	  case 0x06:      /* Marker */
	  case 0x07:      /* Cue point */
	  case 0x08:
	  case 0x09:
	  case 0x0a:
	  case 0x0b:
	  case 0x0c:
	  case 0x0d:
	  case 0x0e:
	  case 0x0f:
	    {
	      /* These are all text events */
	      n = ags_midi_smf_util_get_text_event(NULL,
						   offset,
						   NULL,
						   NULL, NULL);

	      offset += n;
	    }
	    break;
	  case 0x2f:
	    {
	      int c;

	      //	      c = buffer[delta_time_size];

	      //	      if(c == 0x0){
	      /* End of Track */
	      n = ags_midi_smf_util_get_end_of_track(NULL,
						     offset,
						     NULL);

	      offset = NULL;
	    }
	    break;
	  case 0x51:
	    {
	      int c;

	      //	      c = offset[delta_time_size];

	      //	      if(c == 0x03){
	      /* Set tempo */
	      n = ags_midi_smf_util_get_tempo(NULL,
					      offset,
					      NULL,
					      NULL);
		
	      offset += n;
	      //	      }
	    }
	    break;
	  case 0x54:
	    {
	      int c;

	      c = offset[delta_time_size];

	      //	      if(c == 0x05){
	      n = ags_midi_smf_util_get_smtpe(NULL,
					      offset,
					      NULL,
					      NULL, NULL, NULL, NULL, NULL);

	      offset += n;
	      //	      }
	    }
	    break;
	  case 0x58:
	    {
	      int c;

	      //      c = ags_midi_parser_midi_getc(midi_parser);
      
	      n = ags_midi_smf_util_get_time_signature(NULL,
						       offset,
						       NULL,
						       NULL, NULL, NULL, NULL);

	      offset += n;
      
	      //      if(c == 0x04){
	      //      }
	    }
	    break;
	  case 0x59:
	    {
	      int c;

	      //	      c = offset[delta_time_size];

	      //	      if(c == 0x02){
	      n = ags_midi_smf_util_get_key_signature(NULL,
						      offset,
						      NULL,
						      NULL, NULL);

	      offset += n;
	      //	      }
	    }
	    break;
	  case 0x7f:
	    {
	      n = ags_midi_smf_util_get_sequencer_meta_event(NULL,
							     NULL,
							     NULL,
							     NULL, NULL, NULL);
	      
	      offset += n;
	    }
	    break;
	  default:
	    offset += 5;
	  }
	}    
	break;
      default:
	g_warning("bad byte");
      }
    }
  }

  if(delta_time != NULL){
    *delta_time = current_delta_time;
  }
  
  return(offset);
}

/**
 * ags_midi_smf_util_decode:
 * @midi_smf_util: the #AgsMidiSmfUtil-struct
 * @buffer: the MIDI buffer
 * @event: the ALSA sequencer event
 *
 * Decode @event to @buffer
 *
 * Returns: the bytes written
 * 
 * Since: 6.0.0
 */
guint
ags_midi_smf_util_decode(AgsMidiSmfUtil *midi_smf_util,
			 guchar *buffer,
			 snd_seq_event_t *event)
{
  guint count;

  if(buffer == NULL ||
     event == NULL){
    return(0);
  }
  
  count = 0;
  
  switch(event->type){
  case SND_SEQ_EVENT_NOTEON:
    {    
      guchar tmp[8];
      
      ags_midi_smf_util_put_key_on(midi_smf_util,
				   tmp,
				   0,
				   event->data.note.channel,
				   event->data.note.note,
				   event->data.note.velocity);

      count = ags_midi_smf_util_get_varlength_size(midi_smf_util, 0);      
      memcpy(buffer, tmp + count, 3 * sizeof(guchar));
      
      count = 3;
    }
    break;
  case SND_SEQ_EVENT_NOTEOFF:
    {
      guchar tmp[8];

      ags_midi_smf_util_put_key_off(midi_smf_util,
				    tmp,
				    0,
				    event->data.note.channel,
				    event->data.note.note,
				    event->data.note.velocity);

      count = ags_midi_smf_util_get_varlength_size(midi_smf_util, 0);
      memcpy(buffer, tmp + count, 3 * sizeof(guchar));
      
      count = 3;      
    }
    break;
  default:
    g_warning("ags_midi_smf_util_decode() - unsupported MIDI event");
    break;
  }
  
  return(count);
}
