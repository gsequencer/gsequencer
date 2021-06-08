/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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
 *
 * FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#include <ags/audio/ags_fluid_interpolate_none_util.h>

#include <ags/audio/ags_fluid_util.h>

/**
 * SECTION:ags_fluid_interpolate_none_util
 * @short_description: util functions to fluid interpolate none
 * @title: AgsFluidInterpolateNoneUtil
 * @section_id:
 * @include: ags/audio/ags_fluid_interpolate_none_util.h
 *
 * These utility functions allow you to fill fluid interpolated
 * none data.
 */

/**
 * ags_fluid_interpolate_none_util_fill_s8:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate none on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_none_util_fill_s8(gint8 *destination,
					gint8 *source,
					guint buffer_length,
					gdouble phase_incr)
{
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint end_index;
  
  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }

  dsp_phase = 0;
  
  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  dsp_i = 0;

  /* round to nearest point */
  dsp_phase_index = fluid_phase_index_round(dsp_phase);

  /* interpolate sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++){
    /* write destination */
    destination[dsp_i] = source[dsp_phase_index];

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);

    /* round to nearest point */
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_none_util_fill_s16:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate none on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_none_util_fill_s16(gint16 *destination,
					 gint16 *source,
					 guint buffer_length,
					 gdouble phase_incr)
{
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint end_index;
  
  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }

  dsp_phase = 0;
  
  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  dsp_i = 0;

  /* round to nearest point */
  dsp_phase_index = fluid_phase_index_round(dsp_phase);

  /* interpolate sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++){
    /* write destination */
    destination[dsp_i] = source[dsp_phase_index];

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);

    /* round to nearest point */
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_none_util_fill_s24:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate none on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_none_util_fill_s24(gint32 *destination,
					 gint32 *source,
					 guint buffer_length,
					 gdouble phase_incr)
{
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint end_index;
  
  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }

  dsp_phase = 0;
  
  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  dsp_i = 0;

  /* round to nearest point */
  dsp_phase_index = fluid_phase_index_round(dsp_phase);

  /* interpolate sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++){
    /* write destination */
    destination[dsp_i] = source[dsp_phase_index];

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);

    /* round to nearest point */
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_none_util_fill_s32:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate none on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_none_util_fill_s32(gint32 *destination,
					 gint32 *source,
					 guint buffer_length,
					 gdouble phase_incr)
{
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint end_index;
  
  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }

  dsp_phase = 0;
  
  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  dsp_i = 0;

  /* round to nearest point */
  dsp_phase_index = fluid_phase_index_round(dsp_phase);

  /* interpolate sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++){
    /* write destination */
    destination[dsp_i] = source[dsp_phase_index];

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);

    /* round to nearest point */
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_none_util_fill_s64:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate none on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_none_util_fill_s64(gint64 *destination,
					 gint64 *source,
					 guint buffer_length,
					 gdouble phase_incr)
{
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint end_index;
  
  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }

  dsp_phase = 0;
  
  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  dsp_i = 0;

  /* round to nearest point */
  dsp_phase_index = fluid_phase_index_round(dsp_phase);

  /* interpolate sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++){
    /* write destination */
    destination[dsp_i] = source[dsp_phase_index];

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);

    /* round to nearest point */
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_none_util_fill_float:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate none on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_none_util_fill_float(gfloat *destination,
					   gfloat *source,
					   guint buffer_length,
					   gdouble phase_incr)
{
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint end_index;
  
  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }

  dsp_phase = 0;
  
  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  dsp_i = 0;

  /* round to nearest point */
  dsp_phase_index = fluid_phase_index_round(dsp_phase);

  /* interpolate sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++){
    /* write destination */
    destination[dsp_i] = source[dsp_phase_index];

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);

    /* round to nearest point */
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_none_util_fill_double:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate none on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_none_util_fill_double(gdouble *destination,
					    gdouble *source,
					    guint buffer_length,
					    gdouble phase_incr)
{
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint end_index;
  
  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }

  dsp_phase = 0;
  
  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  dsp_i = 0;

  /* round to nearest point */
  dsp_phase_index = fluid_phase_index_round(dsp_phase);

  /* interpolate sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++){
    /* write destination */
    destination[dsp_i] = source[dsp_phase_index];

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);

    /* round to nearest point */
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_none_util_fill_complex:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate none on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_none_util_fill_complex(AgsComplex *destination,
					     AgsComplex *source,
					     guint buffer_length,
					     gdouble phase_incr)
{
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint end_index;
  
  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }

  dsp_phase = 0;
  
  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  dsp_i = 0;

  /* round to nearest point */
  dsp_phase_index = fluid_phase_index_round(dsp_phase);

  /* interpolate sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++){
    /* write destination */
    ags_complex_set(destination + dsp_i,
		    ags_complex_get(source + dsp_phase_index));

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);

    /* round to nearest point */
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);
  }
}
