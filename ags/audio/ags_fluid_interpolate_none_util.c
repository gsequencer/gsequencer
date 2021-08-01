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

GType
ags_fluid_interpolate_none_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fluid_interpolate_none_util = 0;

    ags_type_fluid_interpolate_none_util =
      g_boxed_type_register_static("AgsFluidInterpolateNoneUtil",
				   (GBoxedCopyFunc) ags_fluid_interpolate_none_util_copy,
				   (GBoxedFreeFunc) ags_fluid_interpolate_none_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fluid_interpolate_none_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_fluid_interpolate_none_util_alloc:
 * 
 * Allocate #AgsFluidInterpolateNoneUtil-struct.
 * 
 * Returns: the newly allocated #AgsFluidInterpolateNoneUtil-struct
 * 
 * Since: 3.9.6
 */
AgsFluidInterpolateNoneUtil*
ags_fluid_interpolate_none_util_alloc()
{
  AgsFluidInterpolateNoneUtil *ptr;
  
  ptr = (AgsFluidInterpolateNoneUtil *) g_new(AgsFluidInterpolateNoneUtil,
					      1);

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->destination = NULL;
  ptr->destination_stride = 1;

  ptr->buffer_length = 0;
  ptr->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  ptr->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  ptr->phase_increment = 0.0;

  return(ptr);
}

/**
 * ags_fluid_interpolate_none_util_copy:
 * @ptr: the original #AgsFluidInterpolateNoneUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsFluidInterpolateNoneUtil-struct
 *
 * Since: 3.9.6
 */
gpointer
ags_fluid_interpolate_none_util_copy(AgsFluidInterpolateNoneUtil *ptr)
{
  AgsFluidInterpolateNoneUtil *new_ptr;
  
  new_ptr = (AgsFluidInterpolateNoneUtil *) g_new(AgsFluidInterpolateNoneUtil,
						  1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->phase_increment = ptr->phase_increment;

  return(new_ptr);
}

/**
 * ags_fluid_interpolate_none_util_free:
 * @ptr: the #AgsFluidInterpolateNoneUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_none_util_free(AgsFluidInterpolateNoneUtil *ptr)
{
  g_free(ptr->destination);

  if(ptr->destination != ptr->source){
    g_free(ptr->source);
  }
  
  g_free(ptr);
}

/**
 * ags_fluid_interpolate_none_util_get_destination:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * 
 * Get destination buffer of @fluid_interpolate_none_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 3.9.6
 */
gpointer
ags_fluid_interpolate_none_util_get_destination(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util)
{
  if(fluid_interpolate_none_util == NULL){
    return(NULL);
  }

  return(fluid_interpolate_none_util->destination);
}

/**
 * ags_fluid_interpolate_none_util_set_destination:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * @destination: the destination buffer
 *
 * Set @destination buffer of @fluid_interpolate_none_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_none_util_set_destination(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						gpointer destination)
{
  if(fluid_interpolate_none_util == NULL){
    return;
  }

  fluid_interpolate_none_util->destination = destination;
}

/**
 * ags_fluid_interpolate_none_util_get_destination_stride:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * 
 * Get destination stride of @fluid_interpolate_none_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 3.9.6
 */
guint
ags_fluid_interpolate_none_util_get_destination_stride(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util)
{
  if(fluid_interpolate_none_util == NULL){
    return(0);
  }

  return(fluid_interpolate_none_util->destination_stride);
}

/**
 * ags_fluid_interpolate_none_util_set_destination_stride:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @fluid_interpolate_none_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_none_util_set_destination_stride(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						       guint destination_stride)
{
  if(fluid_interpolate_none_util == NULL){
    return;
  }

  fluid_interpolate_none_util->destination_stride = destination_stride;
}

/**
 * ags_fluid_interpolate_none_util_get_source:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * 
 * Get source buffer of @fluid_interpolate_none_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.9.6
 */
gpointer
ags_fluid_interpolate_none_util_get_source(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util)
{
  if(fluid_interpolate_none_util == NULL){
    return(NULL);
  }

  return(fluid_interpolate_none_util->source);
}

/**
 * ags_fluid_interpolate_none_util_set_source:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @fluid_interpolate_none_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_none_util_set_source(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
					   gpointer source)
{
  if(fluid_interpolate_none_util == NULL){
    return;
  }

  fluid_interpolate_none_util->source = source;
}

/**
 * ags_fluid_interpolate_none_util_get_source_stride:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * 
 * Get source stride of @fluid_interpolate_none_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.9.6
 */
guint
ags_fluid_interpolate_none_util_get_source_stride(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util)
{
  if(fluid_interpolate_none_util == NULL){
    return(0);
  }

  return(fluid_interpolate_none_util->source_stride);
}

/**
 * ags_fluid_interpolate_none_util_set_source_stride:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @fluid_interpolate_none_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_none_util_set_source_stride(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						  guint source_stride)
{
  if(fluid_interpolate_none_util == NULL){
    return;
  }

  fluid_interpolate_none_util->source_stride = source_stride;
}

/**
 * ags_fluid_interpolate_none_util_get_buffer_length:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * 
 * Get buffer length of @fluid_interpolate_none_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.9.6
 */
guint
ags_fluid_interpolate_none_util_get_buffer_length(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util)
{
  if(fluid_interpolate_none_util == NULL){
    return(0);
  }

  return(fluid_interpolate_none_util->buffer_length);
}

/**
 * ags_fluid_interpolate_none_util_set_buffer_length:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @fluid_interpolate_none_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_none_util_set_buffer_length(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						  guint buffer_length)
{
  if(fluid_interpolate_none_util == NULL){
    return;
  }

  fluid_interpolate_none_util->buffer_length = buffer_length;
}

/**
 * ags_fluid_interpolate_none_util_get_format:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * 
 * Get format of @fluid_interpolate_none_util.
 * 
 * Returns: the format
 * 
 * Since: 3.9.6
 */
guint
ags_fluid_interpolate_none_util_get_format(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util)
{
  if(fluid_interpolate_none_util == NULL){
    return(0);
  }

  return(fluid_interpolate_none_util->format);
}

/**
 * ags_fluid_interpolate_none_util_set_format:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * @format: the format
 *
 * Set @format of @fluid_interpolate_none_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_none_util_set_format(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
					   guint format)
{
  if(fluid_interpolate_none_util == NULL){
    return;
  }

  fluid_interpolate_none_util->format = format;
}

/**
 * ags_fluid_interpolate_none_util_get_samplerate:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * 
 * Get samplerate of @fluid_interpolate_none_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.9.6
 */
guint
ags_fluid_interpolate_none_util_get_samplerate(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util)
{
  if(fluid_interpolate_none_util == NULL){
    return(0);
  }

  return(fluid_interpolate_none_util->samplerate);
}

/**
 * ags_fluid_interpolate_none_util_set_samplerate:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @fluid_interpolate_none_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_none_util_set_samplerate(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
					       guint samplerate)
{
  if(fluid_interpolate_none_util == NULL){
    return;
  }

  fluid_interpolate_none_util->samplerate = samplerate;
}

/**
 * ags_fluid_interpolate_none_util_get_fluid_interpolate_none:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * 
 * Get phase_increment of @fluid_interpolate_none_util.
 * 
 * Returns: the phase_increment
 * 
 * Since: 3.9.6
 */
gdouble
ags_fluid_interpolate_none_util_get_phase_increment(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util)
{
  if(fluid_interpolate_none_util == NULL){
    return(1.0);
  }

  return(fluid_interpolate_none_util->phase_increment);
}

/**
 * ags_fluid_interpolate_none_util_set_phase_increment:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * @phase_increment: the phase increment
 *
 * Set @phase_increment of @fluid_interpolate_none_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_none_util_set_phase_increment(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						    gdouble phase_increment)
{
  if(fluid_interpolate_none_util == NULL){
    return;
  }

  fluid_interpolate_none_util->phase_increment = phase_increment;
}

/**
 * ags_fluid_interpolate_none_util_pitch_s8:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * 
 * Pitch @fluid_interpolate_none_util of signed 8 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_none_util_pitch_s8(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_interpolate_none_util_pitch_s16:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * 
 * Pitch @fluid_interpolate_none_util of signed 16 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_none_util_pitch_s16(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_interpolate_none_util_pitch_s24:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * 
 * Pitch @fluid_interpolate_none_util of signed 24 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_none_util_pitch_s24(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_interpolate_none_util_pitch_s32:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * 
 * Pitch @fluid_interpolate_none_util of signed 32 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_none_util_pitch_s32(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_interpolate_none_util_pitch_s64:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * 
 * Pitch @fluid_interpolate_none_util of signed 64 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_none_util_pitch_s64(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_interpolate_none_util_pitch_float:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * 
 * Pitch @fluid_interpolate_none_util of floating point data.
 * 
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_none_util_pitch_float(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_interpolate_none_util_pitch_double:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * 
 * Pitch @fluid_interpolate_none_util of double precision floating point data.
 * 
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_none_util_pitch_double(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_interpolate_none_util_pitch_complex:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * 
 * Pitch @fluid_interpolate_none_util of complex data.
 * 
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_none_util_pitch_complex(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_interpolate_none_util_pitch:
 * @fluid_interpolate_none_util: the #AgsFluidInterpolateNoneUtil-struct
 * 
 * Pitch @fluid_interpolate_none_util.
 * 
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_none_util_pitch(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util)
{
  //TODO:JK: implement me
}

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
  dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

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
  dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

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
  dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

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
  dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

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
  dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

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
  dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

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
  dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

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
  dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

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
