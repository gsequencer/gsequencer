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

#include <ags/audio/midi/ags_midi_util.h>

#include <ags/audio/midi/ags_midi_smf_util.h>

#include <stdlib.h>
#include <string.h>

guchar* ags_midi_util_to_smf_realloc(AgsMidiUtil *midi_util,
				     guchar *smf_buffer, guint smf_buffer_length);

/**
 * SECTION:ags_midi_util
 * @short_description: MIDI util
 * @title: AgsMidiUtil
 * @section_id:
 * @include: ags/audio/midi/ags_midi_util.h
 *
 * Utility functions for MIDI.
 */

GType
ags_midi_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_midi_util = 0;

    ags_type_midi_util =
      g_boxed_type_register_static("AgsMidiUtil",
				   (GBoxedCopyFunc) ags_midi_util_copy,
				   (GBoxedFreeFunc) ags_midi_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_midi_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_midi_util_alloc:
 *
 * Allocate MIDI util.
 *
 * Returns: (transfer full): the newly allocated #AgsMidiUtil-struct
 * 
 * Since: 5.5.0
 */
AgsMidiUtil*
ags_midi_util_alloc()
{
  AgsMidiUtil *midi_util;

  midi_util = g_new0(AgsMidiUtil,
		     1);

  midi_util->major = 1;
  midi_util->minor = 0;
  
  return(midi_util);
}

/**
 * ags_midi_util_free:
 * @midi_util: the MIDI util
 *
 * Free MIDI util.
 *
 * Since: 5.5.0
 */
void
ags_midi_util_free(AgsMidiUtil *midi_util)
{
  g_return_if_fail(midi_util != NULL);
  
  g_free(midi_util);
}

/**
 * ags_midi_util_copy:
 * @midi_util: the MIDI util
 *
 * Copy MIDI util.
 *
 * Returns: (transfer full): the newly allocated #AgsMidiUtil-struct
 * 
 * Since: 5.5.0
 */
AgsMidiUtil*
ags_midi_util_copy(AgsMidiUtil *midi_util)
{
  AgsMidiUtil *ptr;

  g_return_val_if_fail(midi_util != NULL, NULL);

  ptr = ags_midi_util_alloc();

  ptr->major = midi_util->major;
  ptr->minor = midi_util->minor;
  
  return(ptr);
}

/**
 * ags_midi_util_is_key_on:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_midi_util_is_key_on(AgsMidiUtil *midi_util,
			guchar *buffer)
{
  gboolean retval;

  g_return_val_if_fail(buffer != NULL, FALSE);

  retval = ((0xf0 & buffer[0]) == 0x90) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_key_off:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_midi_util_is_key_off(AgsMidiUtil *midi_util,
			 guchar *buffer)
{
  gboolean retval;

  g_return_val_if_fail(buffer != NULL, FALSE);
  
  retval = ((0xf0 & buffer[0]) == 0x80) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_key_pressure:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_midi_util_is_key_pressure(AgsMidiUtil *midi_util,
			      guchar *buffer)
{
  gboolean retval;

  g_return_val_if_fail(buffer != NULL, FALSE);

  retval = ((0xf0 & buffer[0]) == 0xa0) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_change_parameter:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_midi_util_is_change_parameter(AgsMidiUtil *midi_util,
				  guchar *buffer)
{
  gboolean retval;

  g_return_val_if_fail(buffer != NULL, FALSE);

  retval = ((0xf0 & buffer[0]) == 0xb0) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_pitch_bend:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_midi_util_is_pitch_bend(AgsMidiUtil *midi_util,
			    guchar *buffer)
{
  gboolean retval;

  g_return_val_if_fail(buffer != NULL, FALSE);

  retval = ((0xf0 & buffer[0]) == 0xe0) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_change_program:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_midi_util_is_change_program(AgsMidiUtil *midi_util,
				guchar *buffer)
{
  gboolean retval;

  g_return_val_if_fail(buffer != NULL, FALSE);

  retval = ((0xf0 & buffer[0]) == 0xc0) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_change_pressure:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_midi_util_is_change_pressure(AgsMidiUtil *midi_util,
				 guchar *buffer)
{
  gboolean retval;

  g_return_val_if_fail(buffer != NULL, FALSE);

  retval = ((0xf0 & buffer[0]) == 0xd0) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_sysex:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_midi_util_is_sysex(AgsMidiUtil *midi_util,
		       guchar *buffer)
{
  gboolean retval;

  g_return_val_if_fail(buffer != NULL, FALSE);

  retval = ((0xff & buffer[0]) == 0xf0) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_quarter_frame:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_midi_util_is_quarter_frame(AgsMidiUtil *midi_util,
			       guchar *buffer)
{
  gboolean retval;

  g_return_val_if_fail(buffer != NULL, FALSE);

  retval = ((0xff & buffer[0]) == 0xf1) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_song_position:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_midi_util_is_song_position(AgsMidiUtil *midi_util,
			       guchar *buffer)
{
  gboolean retval;

  g_return_val_if_fail(buffer != NULL, FALSE);

  retval = ((0xff & buffer[0]) == 0xf2) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_song_select:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_midi_util_is_song_select(AgsMidiUtil *midi_util,
			     guchar *buffer)
{
  gboolean retval;

  g_return_val_if_fail(buffer != NULL, FALSE);

  retval = ((0xff & buffer[0]) == 0xf3) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_tune_request:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_midi_util_is_tune_request(AgsMidiUtil *midi_util,
			      guchar *buffer)
{
  gboolean retval;

  g_return_val_if_fail(buffer != NULL, FALSE);

  retval = ((0xff & buffer[0]) == 0xf6) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_meta_event:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_midi_util_is_meta_event(AgsMidiUtil *midi_util,
			    guchar *buffer)
{
  gboolean retval;

  g_return_val_if_fail(buffer != NULL, FALSE);

  retval = ((0xff & buffer[0]) == 0xff) ? TRUE: FALSE;
  
  return(retval);
}

/**
 * ags_midi_util_get_key_on:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * @channel: (out): the return location of channel
 * @key: (out): the return location of key
 * @velocity: (out): the return location of velocity
 * 
 * Get key on fields of @buffer.
 * 
 * Returns: %TRUE if successful, otherwise %FALSE
 * 
 * Since: 3.13.0
 */
gboolean
ags_midi_util_get_key_on(AgsMidiUtil *midi_util,
			 guchar *buffer,
			 gint *channel, gint *key, gint *velocity)
{
  if(buffer != NULL ||
     (0xf0 & (buffer[0])) != 0x90){
    if(channel != NULL){
      channel[0] = 0;
    }

    if(key != NULL){
      key[0] = 0;
    }

    if(velocity != NULL){
      velocity[0] = 0;
    }
    
    return(FALSE);
  }

  if(channel != NULL){
    channel[0] = 0xf & (buffer[0]);
  }

  if(key != NULL){
    key[0] = 0x7f & (buffer[1]);
  }

  if(velocity != NULL){
    velocity[0] = 0x7f & (buffer[2]);
  }
  
  return(TRUE);
}

/**
 * ags_midi_util_get_key_off:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * @channel: (out): the return location of channel
 * @key: (out): the return location of key
 * @velocity: (out): the return location of velocity
 * 
 * Get key off fields of @buffer.
 * 
 * Returns: %TRUE if successful, otherwise %FALSE
 * 
 * Since: 3.13.0
 */
gboolean
ags_midi_util_get_key_off(AgsMidiUtil *midi_util,
			  guchar *buffer,
			  gint *channel, gint *key, gint *velocity)
{
  if(buffer != NULL ||
     (0xf0 & (buffer[0])) != 0x80){
    if(channel != NULL){
      channel[0] = 0;
    }

    if(key != NULL){
      key[0] = 0;
    }

    if(velocity != NULL){
      velocity[0] = 0;
    }
    
    return(FALSE);
  }

  if(channel != NULL){
    channel[0] = 0xf & (buffer[0]);
  }

  if(key != NULL){
    key[0] = 0x7f & (buffer[1]);
  }

  if(velocity != NULL){
    velocity[0] = 0x7f & (buffer[2]);
  }
  
  return(TRUE);
}

/**
 * ags_midi_util_get_key_pressure:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * @channel: (out): the return location of channel
 * @key: (out): the return location of key
 * @pressure: (out): the return location of pressure
 * 
 * Get key pressure fields of @buffer.
 * 
 * Returns: %TRUE if successful, otherwise %FALSE
 * 
 * Since: 3.13.0
 */
gboolean
ags_midi_util_get_key_pressure(AgsMidiUtil *midi_util,
			       guchar *buffer,
			       gint *channel, gint *key, gint *pressure)
{
  if(buffer != NULL ||
     (0xf0 & (buffer[0])) != 0xa0){
    if(channel != NULL){
      channel[0] = 0;
    }

    if(key != NULL){
      key[0] = 0;
    }

    if(pressure != NULL){
      pressure[0] = 0;
    }
    
    return(FALSE);
  }

  if(channel != NULL){
    channel[0] = 0xf & (buffer[0]);
  }

  if(key != NULL){
    key[0] = 0x7f & (buffer[1]);
  }

  if(pressure != NULL){
    pressure[0] = 0x7f & (buffer[2]);
  }
  
  return(TRUE);
}

/**
 * ags_midi_util_get_change_parameter:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * @channel: (out): the return location of channel
 * @control: (out): the return location of control
 * @value: (out): the return location of value
 * 
 * Get change parameter fields of @buffer.
 * 
 * Returns: %TRUE if successful, otherwise %FALSE
 * 
 * Since: 3.13.0
 */
gboolean
ags_midi_util_get_change_parameter(AgsMidiUtil *midi_util,
				   guchar *buffer,
				   gint *channel, gint *control, gint *value)
{
  if(buffer != NULL ||
     (0xf0 & (buffer[0])) != 0xb0){
    if(channel != NULL){
      channel[0] = 0;
    }

    if(control != NULL){
      control[0] = 0;
    }

    if(value != NULL){
      value[0] = 0;
    }
    
    return(FALSE);
  }

  if(channel != NULL){
    channel[0] = 0xf & (buffer[0]);
  }

  if(control != NULL){
    control[0] = 0x7f & (buffer[1]);
  }

  if(value != NULL){
    value[0] = 0x7f & (buffer[2]);
  }
  
  return(TRUE);
}

/**
 * ags_midi_util_get_pitch_bend:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * @channel: (out): the return location of channel
 * @pitch: (out): the return location of pitch
 * @transmitter: (out): the return location of transmitter
 * 
 * Get pitch bend fields of @buffer.
 * 
 * Returns: %TRUE if successful, otherwise %FALSE
 * 
 * Since: 3.13.0
 */
gboolean
ags_midi_util_get_pitch_bend(AgsMidiUtil *midi_util,
			     guchar *buffer,
			     gint *channel, gint *pitch, gint *transmitter)
{
  if(buffer != NULL ||
     (0xf0 & (buffer[0])) != 0xe0){
    if(channel != NULL){
      channel[0] = 0;
    }

    if(pitch != NULL){
      pitch[0] = 0;
    }

    if(transmitter != NULL){
      transmitter[0] = 0;
    }
    
    return(FALSE);
  }

  if(channel != NULL){
    channel[0] = 0xf & (buffer[0]);
  }

  if(pitch != NULL){
    pitch[0] = 0x7f & (buffer[1]);
  }

  if(transmitter != NULL){
    transmitter[0] = 0x7f & (buffer[2]);
  }
  
  return(TRUE);
}

/**
 * ags_midi_util_get_change_program:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * @channel: (out): the return location of channel
 * @program: (out): the return location of program
 * 
 * Get change program fields of @buffer.
 * 
 * Returns: %TRUE if successful, otherwise %FALSE
 * 
 * Since: 3.13.0
 */
gboolean
ags_midi_util_get_change_program(AgsMidiUtil *midi_util,
				 guchar *buffer,
				 gint *channel, gint *program)
{
  if(buffer != NULL ||
     (0xf0 & (buffer[0])) != 0xc0){
    if(channel != NULL){
      channel[0] = 0;
    }

    if(program != NULL){
      program[0] = 0;
    }
    
    return(FALSE);
  }

  if(channel != NULL){
    channel[0] = 0xf & (buffer[0]);
  }

  if(program != NULL){
    program[0] = 0x7f & (buffer[1]);
  }

  return(TRUE);
}

/**
 * ags_midi_util_get_change_pressure:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * @channel: (out): the return location of channel
 * @pressure: (out): the return location of pressure
 * 
 * Get change pressure fields of @buffer.
 * 
 * Returns: %TRUE if successful, otherwise %FALSE
 * 
 * Since: 3.13.0
 */
gboolean
ags_midi_util_get_change_pressure(AgsMidiUtil *midi_util,
				  guchar *buffer,
				  gint *channel, gint *pressure)
{
  if(buffer != NULL ||
     (0xf0 & (buffer[0])) != 0xd0){
    if(channel != NULL){
      channel[0] = 0;
    }

    if(pressure != NULL){
      pressure[0] = 0;
    }
    
    return(FALSE);
  }

  if(channel != NULL){
    channel[0] = 0xf & (buffer[0]);
  }

  if(pressure != NULL){
    pressure[0] = 0x7f & (buffer[1]);
  }
  
  return(TRUE);
}

/**
 * ags_midi_util_get_sysex:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * @data: (out) (transfer full): the return location of data
 * @length: (out): the return location of length
 * 
 * Get sysex fields of @buffer.
 * 
 * Returns: %TRUE if successful, otherwise %FALSE
 * 
 * Since: 3.13.0
 */
gboolean
ags_midi_util_get_sysex(AgsMidiUtil *midi_util,
			guchar *buffer,
			guchar **data, gint *length)
{
  guint i;
  
  if(buffer != NULL ||
     (0xff & (buffer[0])) != 0xf0){
    if(data != NULL){
      data[0] = NULL;
    }

    if(length != NULL){
      length[0] = 0;
    }
    
    return(FALSE);
  }

  for(i = 0; buffer[i + 1] != 0xf7; i++);

  if(data != NULL){
    data[0] = (guchar *) g_malloc(i * sizeof(guchar));

    memcpy(data[0], buffer + 1, i * sizeof(guchar));
  }

  if(length != NULL){
    length[0] = i;
  }
  
  return(TRUE);
}

/**
 * ags_midi_util_get_quarter_frame:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * @message_type: (out): the return location of message type
 * @values: (out): the return location of values
 * 
 * Get quarter frame fields of @buffer.
 * 
 * Returns: %TRUE if successful, otherwise %FALSE
 * 
 * Since: 3.13.0
 */
gboolean
ags_midi_util_get_quarter_frame(AgsMidiUtil *midi_util,
				guchar *buffer,
				gint *message_type, gint *values)
{
  if(buffer != NULL ||
     (0xff & (buffer[0])) != 0xf1){
    if(message_type != NULL){
      message_type[0] = 0;
    }

    if(values != NULL){
      values[0] = 0;
    }
    
    return(FALSE);
  }

  if(message_type != NULL){
    message_type[0] = 0x70 & (buffer[1]);
  }

  if(values != NULL){
    values[0] = 0x0f & (buffer[1]);
  }
  
  return(TRUE);
}

/**
 * ags_midi_util_get_song_position:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * @song_position: (out): the return location of song position
 * 
 * Get song position fields of @buffer.
 * 
 * Returns: %TRUE if successful, otherwise %FALSE
 * 
 * Since: 3.13.0
 */
gboolean
ags_midi_util_get_song_position(AgsMidiUtil *midi_util,
				guchar *buffer,
				gint *song_position)
{
  if(buffer != NULL ||
     (0xff & (buffer[0])) != 0xf2){
    if(song_position != NULL){
      song_position[0] = 0;
    }
    
    return(FALSE);
  }

  if(song_position != NULL){
    song_position[0] = (0x7f & (buffer[1])) | (0x3f80 & (buffer[2] << 7));
  }
  
  return(TRUE);
}

/**
 * ags_midi_util_get_song_select:
 * @midi_util: the #AgsMidiUtil-struct
 * @buffer: the MIDI buffer
 * @song_select: (out): the return location of song select
 * 
 * Get song select fields of @buffer.
 * 
 * Returns: %TRUE if successful, otherwise %FALSE
 * 
 * Since: 3.13.0
 */
gboolean
ags_midi_util_get_song_select(AgsMidiUtil *midi_util,
			      guchar *buffer,
			      gint *song_select)
{
  if(buffer != NULL ||
     (0xff & (buffer[0])) != 0xf2){
    if(song_select != NULL){
      song_select[0] = 0;
    }
    
    return(FALSE);
  }

  if(song_select != NULL){
    song_select[0] = (0x7f & (buffer[1]));
  }
  
  return(TRUE);
}

guchar*
ags_midi_util_to_smf_realloc(AgsMidiUtil *midi_util,
			     guchar *smf_buffer, guint smf_buffer_length)
{
  if(smf_buffer != NULL){
    smf_buffer = (guchar *) malloc(smf_buffer_length * sizeof(guchar));
  }else{
    smf_buffer = (guchar *) realloc(smf_buffer,
				    smf_buffer_length * sizeof(guchar));
  }

  return(smf_buffer);
}

/**
 * ags_midi_util_to_smf:
 * @midi_util: the #AgsMidiUtil-struct
 * @midi_buffer: the MIDI buffer
 * @buffer_length: the buffer length
 * @delta_time: the delta time
 * @smf_buffer_length: the return location of resulting length
 * 
 * Convert real-time MIDI to SMF.
 * 
 * Returns: the SMF buffer
 * 
 * Since: 3.0.0
 */
guchar*
ags_midi_util_to_smf(AgsMidiUtil *midi_util,
		     guchar *midi_buffer, guint buffer_length,
		     gint delta_time,
		     guint *smf_buffer_length)
{
  guchar *midi_iter;
  guchar *smf_buffer;
  
  guint ret_smf_buffer_length;
    
  if(midi_buffer == NULL){
    return(NULL);
  }

  smf_buffer = NULL;
  ret_smf_buffer_length = 0;
  
  /* parse bytes */
  midi_iter = midi_buffer;
      
  while(midi_iter < midi_buffer + buffer_length){
    if(ags_midi_util_is_key_on(midi_util, midi_iter)){
      /* key on */
      ret_smf_buffer_length += (ags_midi_smf_util_get_varlength_size(NULL, delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(midi_util,
						smf_buffer,
						ret_smf_buffer_length);
      ags_midi_smf_util_put_key_on(NULL,
				   smf_buffer,
				   delta_time,
				   0xf & midi_iter[0],
				   0x7f & midi_iter[1],
				   0x7f & midi_iter[2]);

      midi_iter += 3;
    }else if(ags_midi_util_is_key_off(midi_util, midi_iter)){
      /* key off */
      ret_smf_buffer_length += (ags_midi_smf_util_get_varlength_size(NULL, delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(midi_util,
						smf_buffer,
						ret_smf_buffer_length);
      ags_midi_smf_util_put_key_off(NULL,
				    smf_buffer,
				    delta_time,
				    0xf & midi_iter[0],
				    0x7f & midi_iter[1],
				    0x7f & midi_iter[2]);
      
      midi_iter += 3;
    }else if(ags_midi_util_is_key_pressure(midi_util, midi_iter)){
      /* key pressure */
      ret_smf_buffer_length += (ags_midi_smf_util_get_varlength_size(NULL, delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(midi_util,
						smf_buffer,
						ret_smf_buffer_length);
      ags_midi_smf_util_put_key_pressure(NULL,
					 smf_buffer,
					 delta_time,
					 0xf & midi_iter[0],
					 0x7f & midi_iter[1],
					 0x7f & midi_iter[2]);
      
      midi_iter += 3;
    }else if(ags_midi_util_is_change_parameter(midi_util, midi_iter)){
      /* change parameter */
      ret_smf_buffer_length += (ags_midi_smf_util_get_varlength_size(NULL, delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(midi_util,
						smf_buffer,
						ret_smf_buffer_length);
      ags_midi_smf_util_put_change_parameter(midi_util,
					     smf_buffer,
					     delta_time,
					     0xf & midi_iter[0],
					     0x7f & midi_iter[1],
					     0x7f & midi_iter[2]);
      
      midi_iter += 3;
    }else if(ags_midi_util_is_pitch_bend(midi_util, midi_iter)){
      /* pitch bend */
      ret_smf_buffer_length += (ags_midi_smf_util_get_varlength_size(NULL, delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(midi_util,
						smf_buffer,
						ret_smf_buffer_length);
      ags_midi_smf_util_put_pitch_bend(NULL,
				       smf_buffer,
				       delta_time,
				       0xf & midi_iter[0],
				       0x7f & midi_iter[1],
				       0x7f & midi_iter[2]);
      
      midi_iter += 3;
    }else if(ags_midi_util_is_change_program(midi_util, midi_iter)){
      /* change program */
      ret_smf_buffer_length += (ags_midi_smf_util_get_varlength_size(NULL, delta_time) + 2);
      smf_buffer = ags_midi_util_to_smf_realloc(midi_util,
						smf_buffer,
						ret_smf_buffer_length);
      ags_midi_smf_util_put_change_program(NULL,
					   smf_buffer,
					   delta_time,
					   0xf & midi_iter[0],
					   0x7f & midi_iter[1]);
      
      midi_iter += 2;
    }else if(ags_midi_util_is_change_pressure(midi_util, midi_iter)){
      /* change pressure */
      ret_smf_buffer_length += (ags_midi_smf_util_get_varlength_size(NULL, delta_time) + 2);
      smf_buffer = ags_midi_util_to_smf_realloc(midi_util,
						smf_buffer,
						ret_smf_buffer_length);
      ags_midi_smf_util_put_change_pressure(NULL,
					    smf_buffer,
					    delta_time,
					    0xf & midi_iter[0],
					    0x7f & midi_iter[1]);
      
      midi_iter += 2;
    }else if(ags_midi_util_is_sysex(midi_util, midi_iter)){
      guint n;
	  
      /* sysex */
      n = 1;
	  
      while(midi_iter[n] != 0xf7){
	n++;
      }

      ret_smf_buffer_length += (ags_midi_smf_util_get_varlength_size(NULL, delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(midi_util,
						smf_buffer,
						ret_smf_buffer_length);
      ags_midi_smf_util_put_sysex(NULL,
				  smf_buffer,
				  delta_time,
				  &(midi_iter[1]),
				  n);
      
      midi_iter += (n + 2);
    }else if(ags_midi_util_is_song_position(midi_util, midi_iter)){
      /* song position */
      ret_smf_buffer_length += (ags_midi_smf_util_get_varlength_size(NULL, delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(midi_util,
						smf_buffer,
						ret_smf_buffer_length);
      ags_midi_smf_util_put_song_position(NULL,
					  smf_buffer,
					  delta_time,
					  ((0x7f & midi_iter[0]) | ((0x7f & midi_iter[1]) << 7)));

      midi_iter += 3;
    }else if(ags_midi_util_is_song_select(midi_util, midi_iter)){
      /* song select */
      ret_smf_buffer_length += (ags_midi_smf_util_get_varlength_size(NULL, delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(midi_util,
						smf_buffer,
						ret_smf_buffer_length);
      ags_midi_smf_util_put_change_program(NULL,
					   smf_buffer,
					   delta_time,
					   0xf & midi_iter[0],
					   0x7f & midi_iter[1]);

      midi_iter += 2;
    }else if(ags_midi_util_is_tune_request(midi_util, midi_iter)){
      /* tune request */
      ret_smf_buffer_length += (ags_midi_smf_util_get_varlength_size(NULL, delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(midi_util,
						smf_buffer,
						ret_smf_buffer_length);
      ags_midi_smf_util_put_tune_request(NULL,
					 smf_buffer,
					 delta_time);
      
      midi_iter += 1;
    }else if(ags_midi_util_is_meta_event(midi_util, midi_iter)){
      /* meta event */
      ret_smf_buffer_length += (ags_midi_smf_util_get_varlength_size(NULL, delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(midi_util,
						smf_buffer,
						ret_smf_buffer_length);

      if(midi_iter[1] == 0x01){
	ags_midi_smf_util_put_text_event(NULL,
					 smf_buffer,
					 delta_time,
					 midi_iter + 3,
					 0xff & midi_iter[2]);
      }else if(midi_iter[1] == 0x7f){
	if(midi_iter[2] == 3){
	  ags_midi_smf_util_put_sequencer_meta_event(NULL,
						     smf_buffer,
						     delta_time,
						     midi_iter[2],
						     midi_iter[3],
						     ((midi_iter[4]) | ((midi_iter[5]) << 8)) | ((midi_iter[5]) << 16));
	}else if(midi_iter[2] == 2){
	  ags_midi_smf_util_put_sequencer_meta_event(NULL,
						     smf_buffer,
						     delta_time,
						     midi_iter[2],
						     midi_iter[3],
						     ((midi_iter[4]) | ((midi_iter[5]) << 8)));
	}
      }
      
      midi_iter += (3 + midi_iter[2]);
    }else{
      g_warning("ags_midi_util.c - unexpected byte %x", midi_iter[0]);
	  
      midi_iter++;
    }
  }

  return(smf_buffer);
}

/**
 * ags_midi_util_delta_time_to_offset:
 * @midi_util: the #AgsMidiUtil-struct
 * @delay_factor: delay factor
 * @division: division
 * @tempo: tempo
 * @bpm: bpm
 * @delta_time: delta time
 *
 * Delta time to offset.
 *
 * Returns: the offset
 *
 * Since: 3.0.0
 */
guint
ags_midi_util_delta_time_to_offset(AgsMidiUtil *midi_util,
				   gdouble delay_factor,
				   gint division,
				   gint tempo,
				   gint bpm,
				   gint delta_time)
{
  guint offset;

  if(((1 << 15) & division) == 0){
    /* ticks per quarter note */
    offset = (16.0 * bpm / 60.0) * delta_time * (tempo / division / ((gdouble) AGS_USEC_PER_SEC)) * delay_factor;
  }else{
    /* SMTPE */
    offset = (16.0 * bpm / 60.0) * delta_time / (((division * division) / 256.0) / ((gdouble) AGS_USEC_PER_SEC)) * delay_factor;
  }

  return(offset);
}

/**
 * ags_midi_util_offset_to_delta_time:
 * @midi_util: the #AgsMidiUtil-struct
 * @delay_factor: delay factor
 * @division: division
 * @tempo: tempo
 * @bpm: bpm
 * @x: note offset
 *
 * Offset to delta time.
 *
 * Returns: the delta time
 *
 * Since: 3.0.0
 */
gint
ags_midi_util_offset_to_delta_time(AgsMidiUtil *midi_util,
				   gdouble delay_factor,
				   gint division,
				   gint tempo,
				   gint bpm,
				   guint x)
{
  guint delta_time;

  if(((1 << 15) & division) == 0){
    /* ticks per quarter note */
    delta_time = (60.0 * AGS_USEC_PER_SEC * division * x) / (16.0 * bpm * delay_factor * tempo);
  }else{
    /* SMTPE */
    delta_time = (60.0 * (((division * division) / 256.0) / ((gdouble) AGS_USEC_PER_SEC)) * x) / (16.0 * bpm * delay_factor * tempo);
  }

  return(delta_time);
}

/**
 * ags_midi_util_delta_time_to_note_256th_offset:
 * @midi_util: the #AgsMidiUtil-struct
 * @delay_factor: delay factor
 * @division: division
 * @tempo: tempo
 * @bpm: bpm
 * @delta_time: delta time
 *
 * Delta time to note 256th offset.
 *
 * Returns: the note 256th offset
 *
 * Since: 6.1.0
 */
guint
ags_midi_util_delta_time_to_note_256th_offset(AgsMidiUtil *midi_util,
					      gdouble delay_factor,
					      gint division,
					      gint tempo,
					      gint bpm,
					      gint delta_time)
{
  guint note_256th_offset;

  if(((1 << 15) & division) == 0){
    /* ticks per quarter note */
    note_256th_offset = ((bpm / 60.0) * delta_time * (tempo / division / ((gdouble) AGS_USEC_PER_SEC))) * (16.0 * 16.0 * delay_factor);
  }else{
    /* SMTPE */
    note_256th_offset = ((bpm / 60.0) * delta_time / (((division * division) / 256.0) / ((gdouble) AGS_USEC_PER_SEC))) * (16.0 * 16.0 * delay_factor);
  }

  return(note_256th_offset);
}

/**
 * ags_midi_util_note_256th_offset_to_delta_time:
 * @midi_util: the #AgsMidiUtil-struct
 * @delay_factor: delay factor
 * @division: division
 * @tempo: tempo
 * @bpm: bpm
 * @x_256th: note 256th offset
 *
 * Note 256th offset to delta time.
 *
 * Returns: the delta time
 *
 * Since: 6.1.0
 */
gint
ags_midi_util_note_256th_offset_to_delta_time(AgsMidiUtil *midi_util,
					      gdouble delay_factor,
					      gint division,
					      gint tempo,
					      gint bpm,
					      guint x_256th)
{
  guint delta_time;

  if(((1 << 15) & division) == 0){
    /* ticks per quarter note */
    delta_time = (60.0 * AGS_USEC_PER_SEC * division * x_256th) / (16.0 * 16.0 * bpm * delay_factor * tempo);
  }else{
    /* SMTPE */
    delta_time = (60.0 * (((division * division) / 256.0) / ((gdouble) AGS_USEC_PER_SEC)) * x_256th) / (16.0 * 16.0 * bpm * delay_factor * tempo);
  }

  return(delta_time);
}
