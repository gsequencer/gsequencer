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

#include <ags/audio/midi/ags_midi_util.h>

#include <ags/lib/ags_time.h>

/**
 * SECTION:ags_midi_util
 * @short_description: MIDI util
 * @title: AgsMidiUtil
 * @section_id:
 * @include: ags/audio/midi/ags_midi_util.h
 *
 * Utility functions for MIDI.
 */

/**
 * ags_midi_util_is_key_on:
 * @buffer: the midi buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 1.0.0
 */
gboolean
ags_midi_util_is_key_on(unsigned char *buffer)
{
  gboolean retval;

  retval = ((0xf0 & buffer[0]) == 0x90) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_key_off:
 * @buffer: the midi buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 1.0.0
 */
gboolean
ags_midi_util_is_key_off(unsigned char *buffer)
{
  gboolean retval;

  retval = ((0xf0 & buffer[0]) == 0x80) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_key_pressure:
 * @buffer: the midi buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 1.0.0
 */
gboolean
ags_midi_util_is_key_pressure(unsigned char *buffer)
{
  gboolean retval;

  retval = ((0xf0 & buffer[0]) == 0xa0) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_change_parameter:
 * @buffer: the midi buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 1.0.0
 */
gboolean
ags_midi_util_is_change_parameter(unsigned char *buffer)
{
  gboolean retval;

  retval = ((0xf0 & buffer[0]) == 0xb0) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_pitch_bend:
 * @buffer: the midi buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 1.0.0
 */
gboolean
ags_midi_util_is_pitch_bend(unsigned char *buffer)
{
  gboolean retval;

  retval = ((0xf0 & buffer[0]) == 0xe0) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_change_program:
 * @buffer: the midi buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 1.0.0
 */
gboolean
ags_midi_util_is_change_program(unsigned char *buffer)
{
  gboolean retval;

  retval = ((0xf0 & buffer[0]) == 0xc0) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_change_pressure:
 * @buffer: the midi buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 1.0.0
 */
gboolean
ags_midi_util_is_change_pressure(unsigned char *buffer)
{
  gboolean retval;

  retval = ((0xf0 & buffer[0]) == 0xd0) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_sysex:
 * @buffer: the midi buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 1.0.0
 */
gboolean
ags_midi_util_is_sysex(unsigned char *buffer)
{
  gboolean retval;

  retval = ((0xff & buffer[0]) == 0xf0) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_quarter_frame:
 * @buffer: the midi buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 1.0.0
 */
gboolean
ags_midi_util_is_quarter_frame(unsigned char *buffer)
{
  gboolean retval;

  retval = ((0xff & buffer[0]) == 0xf1) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_song_position:
 * @buffer: the midi buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 1.0.0
 */
gboolean
ags_midi_util_is_song_position(unsigned char *buffer)
{
  gboolean retval;

  retval = ((0xff & buffer[0]) == 0xf2) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_song_select:
 * @buffer: the midi buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 1.0.0
 */
gboolean
ags_midi_util_is_song_select(unsigned char *buffer)
{
  gboolean retval;

  retval = ((0xff & buffer[0]) == 0xf3) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_tune_request:
 * @buffer: the midi buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 1.0.0
 */
gboolean
ags_midi_util_is_tune_request(unsigned char *buffer)
{
  gboolean retval;

  retval = ((0xff & buffer[0]) == 0xf6) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_util_is_meta_event:
 * @buffer: the midi buffer
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 1.0.0
 */
gboolean
ags_midi_util_is_meta_event(unsigned char *buffer)
{
  gboolean retval;

  retval = ((0xff & buffer[0]) == 0xff) ? TRUE: FALSE;
  
  return(retval);
}

/**
 * ags_midi_util_to_smf:
 * @buffer: the midi buffer
 * @buffer_length: the buffer length
 * @delta_time: the delta time
 * @smf_buffer_length: the return location of resulting length
 * 
 * Convert real-time MIDI to SMF.
 * 
 * Returns: the SMF buffer
 * 
 * Since: 1.0.0
 */
unsigned char*
ags_midi_util_to_smf(unsigned char *midi_buffer, guint buffer_length,
		     glong delta_time,
		     guint *smf_buffer_length)
{
  unsigned char *midi_iter;
  unsigned char *smf_buffer;

  
  guint ret_smf_buffer_length;
  
  auto unsigned char* ags_midi_util_to_smf_realloc(unsigned char *smf_buffer, guint smf_buffer_length);

  unsigned char* ags_midi_util_to_smf_realloc(unsigned char *smf_buffer, guint smf_buffer_length){
    if(smf_buffer == NULL){
      smf_buffer = (unsigned char *) malloc(smf_buffer_length * sizeof(unsigned char));
    }else{
      smf_buffer = (unsigned char *) realloc(smf_buffer,
					     smf_buffer_length * sizeof(unsigned char));
    }

    return(smf_buffer);
  }
  
  if(midi_buffer == NULL){
    return(NULL);
  }

  smf_buffer = NULL;
  ret_smf_buffer_length = 0;
  
  /* parse bytes */
  midi_iter = midi_buffer;
      
  while(midi_iter < midi_buffer + buffer_length){
    if(ags_midi_util_is_key_on(midi_iter)){
      /* key on */
      ret_smf_buffer_length += (ags_midi_buffer_util_get_varlength_size(delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(smf_buffer,
						ret_smf_buffer_length);
      ags_midi_buffer_util_put_key_on(smf_buffer,
				      delta_time,
				      0xf & midi_iter[0],
				      0x7f & midi_iter[1],
				      0x7f & midi_iter[2]);

      midi_iter += 3;
    }else if(ags_midi_util_is_key_off(midi_iter)){
      /* key off */
      ret_smf_buffer_length += (ags_midi_buffer_util_get_varlength_size(delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(smf_buffer,
						ret_smf_buffer_length);
      ags_midi_buffer_util_put_key_off(smf_buffer,
				       delta_time,
				       0xf & midi_iter[0],
				       0x7f & midi_iter[1],
				       0x7f & midi_iter[2]);
      
      midi_iter += 3;
    }else if(ags_midi_util_is_key_pressure(midi_iter)){
      /* key pressure */
      ret_smf_buffer_length += (ags_midi_buffer_util_get_varlength_size(delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(smf_buffer,
						ret_smf_buffer_length);
      ags_midi_buffer_util_put_key_pressure(smf_buffer,
					    delta_time,
					    0xf & midi_iter[0],
					    0x7f & midi_iter[1],
					    0x7f & midi_iter[2]);
      
      midi_iter += 3;
    }else if(ags_midi_util_is_change_parameter(midi_iter)){
      /* change parameter */
      ret_smf_buffer_length += (ags_midi_buffer_util_get_varlength_size(delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(smf_buffer,
						ret_smf_buffer_length);
      ags_midi_buffer_util_put_change_parameter(smf_buffer,
						delta_time,
						0xf & midi_iter[0],
						0x7f & midi_iter[1],
						0x7f & midi_iter[2]);
      
      midi_iter += 3;
    }else if(ags_midi_util_is_pitch_bend(midi_iter)){
      /* pitch bend */
      ret_smf_buffer_length += (ags_midi_buffer_util_get_varlength_size(delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(smf_buffer,
						ret_smf_buffer_length);
      ags_midi_buffer_util_put_pitch_bend(smf_buffer,
					  delta_time,
					  0xf & midi_iter[0],
					  0x7f & midi_iter[1],
					  0x7f & midi_iter[2]);
      
      midi_iter += 3;
    }else if(ags_midi_util_is_change_program(midi_iter)){
      /* change program */
      ret_smf_buffer_length += (ags_midi_buffer_util_get_varlength_size(delta_time) + 2);
      smf_buffer = ags_midi_util_to_smf_realloc(smf_buffer,
						ret_smf_buffer_length);
      ags_midi_buffer_util_put_change_program(smf_buffer,
					      delta_time,
					      0xf & midi_iter[0],
					      0x7f & midi_iter[1]);
      
      midi_iter += 2;
    }else if(ags_midi_util_is_change_pressure(midi_iter)){
      /* change pressure */
      ret_smf_buffer_length += (ags_midi_buffer_util_get_varlength_size(delta_time) + 2);
      smf_buffer = ags_midi_util_to_smf_realloc(smf_buffer,
						ret_smf_buffer_length);
      ags_midi_buffer_util_put_change_pressure(smf_buffer,
					       delta_time,
					       0xf & midi_iter[0],
					       0x7f & midi_iter[1]);
      
      midi_iter += 2;
    }else if(ags_midi_util_is_sysex(midi_iter)){
      guint n;
	  
      /* sysex */
      n = 1;
	  
      while(midi_iter[n] != 0xf7){
	n++;
      }

      ret_smf_buffer_length += (ags_midi_buffer_util_get_varlength_size(delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(smf_buffer,
						ret_smf_buffer_length);
      ags_midi_buffer_util_put_sysex(smf_buffer,
				     delta_time,
				     &(midi_iter[1]),
				     n);
      
      midi_iter += (n + 2);
    }else if(ags_midi_util_is_song_position(midi_iter)){
      /* song position */
      ret_smf_buffer_length += (ags_midi_buffer_util_get_varlength_size(delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(smf_buffer,
						ret_smf_buffer_length);
      ags_midi_buffer_util_put_song_position(smf_buffer,
					     delta_time,
					     ((0x7f & midi_iter[0]) | ((0x7f & midi_iter[1]) << 7)));

      midi_iter += 3;
    }else if(ags_midi_util_is_song_select(midi_iter)){
      /* song select */
      ret_smf_buffer_length += (ags_midi_buffer_util_get_varlength_size(delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(smf_buffer,
						ret_smf_buffer_length);
      ags_midi_buffer_util_put_change_program(smf_buffer,
					      delta_time,
					      0x7f & midi_iter[1]);

      midi_iter += 2;
    }else if(ags_midi_util_is_tune_request(midi_iter)){
      /* tune request */
      ret_smf_buffer_length += (ags_midi_buffer_util_get_varlength_size(delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(smf_buffer,
						ret_smf_buffer_length);
      ags_midi_buffer_util_put_tune_request(smf_buffer,
					    delta_time);
      
      midi_iter += 1;
    }else if(ags_midi_util_is_meta_event(midi_iter)){
      /* meta event */
      ret_smf_buffer_length += (ags_midi_buffer_util_get_varlength_size(delta_time) + 3);
      smf_buffer = ags_midi_util_to_smf_realloc(smf_buffer,
						ret_smf_buffer_length);

      if(midi_iter[1] == 0x01){
	ags_midi_buffer_util_put_text_event(smf_buffer,
					    delta_time,
					    midi_iter + 3,
					    0xff & midi_iter[2]);
      }else if(midi_iter[1] == 0x7f){
	if(midi_iter[2] == 3){
	  ags_midi_buffer_util_put_sequencer_meta_event(smf_buffer,
							delta_time,
							midi_iter[2],
							midi_iter[3],
							((midi_iter[4]) | ((midi_iter[5]) << 8)) | ((midi_iter[5]) << 16));
	}else if(midi_iter[2] == 2){
	  ags_midi_buffer_util_put_sequencer_meta_event(smf_buffer,
							delta_time,
							midi_iter[2],
							midi_iter[3],
							((midi_iter[4]) | ((midi_iter[5]) << 8)));
	}
      }
      
      midi_iter += (3 + midi_iter[2]);
    }else{
      g_warning("ags_midi_util.c - unexpected byte %x\0", midi_iter[0]);
	  
      midi_iter++;
    }
  }

  return(smf_buffer);
}

/**
 * ags_midi_util_envelope_to_velocity:
 * @attack: attack
 * @decay: decay
 * @sustain: sustain
 * @release: release
 * @ratio: ratio
 * @samplerate: samplerate
 * @start_frame: start frame
 * @end_frame: end frame
 *
 * Envelope to velocity.
 *
 * Returns: the velocity
 *
 * Since: 0.7.2
 */
glong
ags_midi_util_envelope_to_velocity(AgsComplex *attack,
				   AgsComplex *decay,
				   AgsComplex *sustain,
				   AgsComplex *release,
				   AgsComplex *ratio,
				   guint samplerate,
				   guint start_frame, guint end_frame)
{
  glong velocity;

  velocity = 127;

  //TODO:JK: implement me
  
  return(velocity);
}

/**
 * ags_midi_util_velocity_to_envelope:
 * @delta_time: delta time
 * @is_release: is release
 * @attack: attack
 * @decay: decay
 * @sustain: sustain
 * @release: release
 * @ratio: ratio
 * @samplerate: samplerate
 * @start_frame: start frame
 * @end_frame: end frame
 *
 * Velocity to envelope.
 *
 * Since: 0.7.2
 */
void
ags_midi_util_velocity_to_envelope(glong delta_time,
				   gboolean is_release,
				   AgsComplex **attack,
				   AgsComplex **decay,
				   AgsComplex **sustain,
				   AgsComplex **release,
				   AgsComplex **ratio,
				   guint *samplerate,
				   guint *start_frame, guint *end_frame)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_util_envelope_to_pressure:
 * @attack: attack
 * @decay: decay
 * @sustain: sustain
 * @release: release
 * @ratio: ratio
 * @samplerate: samplerate
 * @start_frame: start frame
 * @end_frame: end frame
 *
 * Envelope to pressure.
 *
 * Returns: the pressure
 *
 * Since: 0.7.2
 */
glong
ags_midi_util_envelope_to_pressure(AgsComplex *attack,
				   AgsComplex *decay,
				   AgsComplex *sustain,
				   AgsComplex *release,
				   AgsComplex *ratio,
				   guint samplerate,
				   guint start_frame, guint end_frame)
{
  glong pressure;

  pressure = 127;

  //TODO:JK: implement me
  
  return(pressure);
}

/**
 * ags_midi_util_pressure_to_envelope:
 * @delta_time: delta time
 * @is_sustain: is sustain
 * @attack: attack
 * @decay: decay
 * @sustain: sustain
 * @release: release
 * @ratio: ratio
 * @samplerate: samplerate
 * @start_frame: start frame
 * @end_frame: end frame
 *
 * Pressure to envelope.
 *
 * Since: 0.7.2
 */
void
ags_midi_util_pressure_to_envelope(glong delta_time,
				   gboolean is_sustain,
				   AgsComplex **attack,
				   AgsComplex **decay,
				   AgsComplex **sustain,
				   AgsComplex **release,
				   AgsComplex **ratio,
				   guint *samplerate,
				   guint *start_frame, guint *end_frame)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_util_delta_time_to_offset:
 * @division: division
 * @tempo: tempo
 * @bpm: bpm
 * @delta_time: delta time
 *
 * Delta time to offset
 *
 * Returns: the offset
 *
 * Since: 0.7.2
 */
guint
ags_midi_util_delta_time_to_offset(glong division,
				   glong tempo,
				   glong bpm,
				   glong delta_time)
{
  guint offset;

  if(((1 << 15) & division) == 0){
    /* ticks per quarter note */
    offset = (16.0 * bpm / 60.0) * delta_time * (tempo / division / ((gdouble) USEC_PER_SEC));
  }else{
    /* SMTPE */
    offset = (16.0 * bpm / 60.0) * delta_time / (((division * division) / 256.0) / ((gdouble) USEC_PER_SEC));
  }

  return(offset);
}

/**
 * ags_midi_util_offset_to_delta_time:
 * @division: division
 * @tempo: tempo
 * @bpm: bpm
 * @x: note offset
 *
 * Offset to delta time
 *
 * Returns: the delta time
 *
 * Since: 0.7.2
 */
glong
ags_midi_util_offset_to_delta_time(glong division,
				   glong tempo,
				   glong bpm,
				   guint x)
{
  guint delta_time;

  if(((1 << 15) & division) == 0){
    /* ticks per quarter note */
    delta_time = x / (16.0 * bpm / 60.0) / (tempo / division / ((gdouble) USEC_PER_SEC));
  }else{
    /* SMTPE */
    delta_time = x / (16.0 * bpm / 60.0) * (((division * division) / 256.0) / ((gdouble) USEC_PER_SEC));
  }

  return(delta_time);
}
