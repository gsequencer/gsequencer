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

#include <ags/audio/ags_fluid_interpolate_4th_order_util.h>

#include <ags/audio/ags_fluid_util.h>

/* 4th Order interpolation table (2 coefficients centered on 1st) */
gboolean interp_coeff_4th_order_initialized = FALSE;

gdouble interp_coeff_4th_order[AGS_FLUID_INTERP_MAX][4];

GMutex interp_coeff_4th_order_mutex;

/**
 * SECTION:ags_fluid_interpolate_4th_order_util
 * @short_description: util functions to fluid interpolate 4th order
 * @title: AgsFluidInterpolate4thOrderUtil
 * @section_id:
 * @include: ags/audio/ags_fluid_interpolate_4th_order_util.h
 *
 * These utility functions allow you to fill fluid interpolated
 * 4th order data.
 */

GType
ags_fluid_interpolate_4th_order_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fluid_interpolate_4th_order_util = 0;

    ags_type_fluid_interpolate_4th_order_util =
      g_boxed_type_register_static("AgsFluidInterpolate4thOrderUtil",
				   (GBoxedCopyFunc) ags_fluid_interpolate_4th_order_util_copy,
				   (GBoxedFreeFunc) ags_fluid_interpolate_4th_order_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fluid_interpolate_4th_order_util);
  }

  return g_define_type_id__volatile;
}

/* Initializes interpolation tables */
void
ags_fluid_interpolate_4th_order_util_config()
{
  gdouble x;
  gint i;
  
  g_mutex_lock(&interp_coeff_4th_order_mutex);
  
  if(interp_coeff_4th_order_initialized){
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    return;
  }

  for(i = 0; i < AGS_FLUID_INTERP_MAX; i++){
    x = (double) i / (double) AGS_FLUID_INTERP_MAX;

    interp_coeff_4th_order[i][0] = (x * (-0.5 + x * (1 - 0.5 * x)));
    interp_coeff_4th_order[i][1] = (1.0 + x * x * (1.5 * x - 2.5));
    interp_coeff_4th_order[i][2] = (x * (0.5 + x * (2.0 - 1.5 * x)));
    interp_coeff_4th_order[i][3] = (0.5 * x * x * (x - 1.0));
  }
  
  interp_coeff_4th_order_initialized = TRUE;
  
  g_mutex_unlock(&interp_coeff_4th_order_mutex);
}

/**
 * ags_fluid_interpolate_4th_order_util_alloc:
 * 
 * Allocate #AgsFluidInterpolate4thOrderUtil-struct.
 * 
 * Returns: the newly allocated #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Since: 3.9.6
 */
AgsFluidInterpolate4thOrderUtil*
ags_fluid_interpolate_4th_order_util_alloc()
{
  AgsFluidInterpolate4thOrderUtil *ptr;
  
  ptr = (AgsFluidInterpolate4thOrderUtil *) g_new(AgsFluidInterpolate4thOrderUtil,
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
 * ags_fluid_interpolate_4th_order_util_copy:
 * @ptr: the original #AgsFluidInterpolate4thOrderUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsFluidInterpolate4thOrderUtil-struct
 *
 * Since: 3.9.6
 */
gpointer
ags_fluid_interpolate_4th_order_util_copy(AgsFluidInterpolate4thOrderUtil *ptr)
{
  AgsFluidInterpolate4thOrderUtil *new_ptr;
  
  new_ptr = (AgsFluidInterpolate4thOrderUtil *) g_new(AgsFluidInterpolate4thOrderUtil,
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
 * ags_fluid_interpolate_4th_order_util_free:
 * @ptr: the #AgsFluidInterpolate4thOrderUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_4th_order_util_free(AgsFluidInterpolate4thOrderUtil *ptr)
{
  g_free(ptr->destination);

  if(ptr->destination != ptr->source){
    g_free(ptr->source);
  }
  
  g_free(ptr);
}

/**
 * ags_fluid_interpolate_4th_order_util_get_destination:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Get destination buffer of @fluid_interpolate_4th_order_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 3.9.6
 */
gpointer
ags_fluid_interpolate_4th_order_util_get_destination(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return(NULL);
  }

  return(fluid_interpolate_4th_order_util->destination);
}

/**
 * ags_fluid_interpolate_4th_order_util_set_destination:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * @destination: the destination buffer
 *
 * Set @destination buffer of @fluid_interpolate_4th_order_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_4th_order_util_set_destination(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
						     gpointer destination)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return;
  }

  fluid_interpolate_4th_order_util->destination = destination;
}

/**
 * ags_fluid_interpolate_4th_order_util_get_destination_stride:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Get destination stride of @fluid_interpolate_4th_order_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 3.9.6
 */
guint
ags_fluid_interpolate_4th_order_util_get_destination_stride(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return(0);
  }

  return(fluid_interpolate_4th_order_util->destination_stride);
}

/**
 * ags_fluid_interpolate_4th_order_util_set_destination_stride:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @fluid_interpolate_4th_order_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_4th_order_util_set_destination_stride(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							    guint destination_stride)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return;
  }

  fluid_interpolate_4th_order_util->destination_stride = destination_stride;
}

/**
 * ags_fluid_interpolate_4th_order_util_get_source:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Get source buffer of @fluid_interpolate_4th_order_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.9.6
 */
gpointer
ags_fluid_interpolate_4th_order_util_get_source(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return(NULL);
  }

  return(fluid_interpolate_4th_order_util->source);
}

/**
 * ags_fluid_interpolate_4th_order_util_set_source:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @fluid_interpolate_4th_order_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_4th_order_util_set_source(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
						gpointer source)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return;
  }

  fluid_interpolate_4th_order_util->source = source;
}

/**
 * ags_fluid_interpolate_4th_order_util_get_source_stride:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Get source stride of @fluid_interpolate_4th_order_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.9.6
 */
guint
ags_fluid_interpolate_4th_order_util_get_source_stride(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return(0);
  }

  return(fluid_interpolate_4th_order_util->source_stride);
}

/**
 * ags_fluid_interpolate_4th_order_util_set_source_stride:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @fluid_interpolate_4th_order_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_4th_order_util_set_source_stride(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
						       guint source_stride)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return;
  }

  fluid_interpolate_4th_order_util->source_stride = source_stride;
}

/**
 * ags_fluid_interpolate_4th_order_util_get_buffer_length:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Get buffer length of @fluid_interpolate_4th_order_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.9.6
 */
guint
ags_fluid_interpolate_4th_order_util_get_buffer_length(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return(0);
  }

  return(fluid_interpolate_4th_order_util->buffer_length);
}

/**
 * ags_fluid_interpolate_4th_order_util_set_buffer_length:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @fluid_interpolate_4th_order_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_4th_order_util_set_buffer_length(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
						       guint buffer_length)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return;
  }

  fluid_interpolate_4th_order_util->buffer_length = buffer_length;
}

/**
 * ags_fluid_interpolate_4th_order_util_get_format:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Get format of @fluid_interpolate_4th_order_util.
 * 
 * Returns: the format
 * 
 * Since: 3.9.6
 */
guint
ags_fluid_interpolate_4th_order_util_get_format(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return(0);
  }

  return(fluid_interpolate_4th_order_util->format);
}

/**
 * ags_fluid_interpolate_4th_order_util_set_format:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * @format: the format
 *
 * Set @format of @fluid_interpolate_4th_order_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_4th_order_util_set_format(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
						guint format)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return;
  }

  fluid_interpolate_4th_order_util->format = format;
}

/**
 * ags_fluid_interpolate_4th_order_util_get_samplerate:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Get samplerate of @fluid_interpolate_4th_order_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.9.6
 */
guint
ags_fluid_interpolate_4th_order_util_get_samplerate(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return(0);
  }

  return(fluid_interpolate_4th_order_util->samplerate);
}

/**
 * ags_fluid_interpolate_4th_order_util_set_samplerate:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @fluid_interpolate_4th_order_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_4th_order_util_set_samplerate(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
						    guint samplerate)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return;
  }

  fluid_interpolate_4th_order_util->samplerate = samplerate;
}

/**
 * ags_fluid_interpolate_4th_order_util_get_fluid_interpolate_4th_order:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Get phase_increment of @fluid_interpolate_4th_order_util.
 * 
 * Returns: the phase_increment
 * 
 * Since: 3.9.6
 */
gdouble
ags_fluid_interpolate_4th_order_util_get_phase_increment(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return(1.0);
  }

  return(fluid_interpolate_4th_order_util->phase_increment);
}

/**
 * ags_fluid_interpolate_4th_order_util_set_phase_increment:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * @phase_increment: the phase increment
 *
 * Set @phase_increment of @fluid_interpolate_4th_order_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_4th_order_util_set_phase_increment(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							 gdouble phase_increment)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return;
  }

  fluid_interpolate_4th_order_util->phase_increment = phase_increment;
}

/**
 * ags_fluid_interpolate_4th_order_util_pitch_s8:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Pitch @fluid_interpolate_4th_order_util of signed 8 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_4th_order_util_pitch_s8(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_interpolate_4th_order_util_pitch_s16:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Pitch @fluid_interpolate_4th_order_util of signed 16 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_4th_order_util_pitch_s16(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_interpolate_4th_order_util_pitch_s24:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Pitch @fluid_interpolate_4th_order_util of signed 24 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_4th_order_util_pitch_s24(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_interpolate_4th_order_util_pitch_s32:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Pitch @fluid_interpolate_4th_order_util of signed 32 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_4th_order_util_pitch_s32(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_interpolate_4th_order_util_pitch_s64:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Pitch @fluid_interpolate_4th_order_util of signed 64 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_4th_order_util_pitch_s64(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_interpolate_4th_order_util_pitch_float:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Pitch @fluid_interpolate_4th_order_util of floating point data.
 * 
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_4th_order_util_pitch_float(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_interpolate_4th_order_util_pitch_double:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Pitch @fluid_interpolate_4th_order_util of double precision floating point data.
 * 
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_4th_order_util_pitch_double(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_interpolate_4th_order_util_pitch_complex:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Pitch @fluid_interpolate_4th_order_util of complex data.
 * 
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_4th_order_util_pitch_complex(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_interpolate_4th_order_util_pitch:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Pitch @fluid_interpolate_4th_order_util.
 * 
 * Since: 3.9.6
 */
void
ags_fluid_interpolate_4th_order_util_pitch(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  //TODO:JK: implement me
}

/**
 * ags_fluid_interpolate_4th_order_util_fill_s8:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate 4th order on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_4th_order_util_fill_s8(gint8 *destination,
					     gint8 *source,
					     guint buffer_length,
					     gdouble phase_incr)
{
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }
  
  ags_fluid_interpolate_4th_order_util_config();
  
  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gint row;
        
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    destination[dsp_i] = (coeffs_0 * start_point
			  + coeffs_1 * source[dsp_phase_index]
			  + coeffs_2 * source[dsp_phase_index + 1]
			  + coeffs_3 * source[dsp_phase_index + 2]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    destination[dsp_i] = (coeffs_0 * source[dsp_phase_index - 1]
			  + coeffs_1 * source[dsp_phase_index]
			  + coeffs_2 * source[dsp_phase_index + 1]
			  + coeffs_3 * source[dsp_phase_index + 2]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_4th_order_util_fill_s16:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate 4th order on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_4th_order_util_fill_s16(gint16 *destination,
					      gint16 *source,
					      guint buffer_length,
					      gdouble phase_incr)
{
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }
  
  ags_fluid_interpolate_4th_order_util_config();

  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gint row;

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    destination[dsp_i] = (coeffs_0 * start_point
			  + coeffs_1 * source[dsp_phase_index]
			  + coeffs_2 * source[dsp_phase_index + 1]
			  + coeffs_3 * source[dsp_phase_index + 2]);
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    destination[dsp_i] = (coeffs_0 * source[dsp_phase_index - 1]
			  + coeffs_1 * source[dsp_phase_index]
			  + coeffs_2 * source[dsp_phase_index + 1]
			  + coeffs_3 * source[dsp_phase_index + 2]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_4th_order_util_fill_s24:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate 4th order on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_4th_order_util_fill_s24(gint32 *destination,
					      gint32 *source,
					      guint buffer_length,
					      gdouble phase_incr)
{
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }
  
  ags_fluid_interpolate_4th_order_util_config();
  
  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    destination[dsp_i] = (coeffs_0 * start_point
			  + coeffs_1 * source[dsp_phase_index]
			  + coeffs_2 * source[dsp_phase_index + 1]
			  + coeffs_3 * source[dsp_phase_index + 2]);
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    destination[dsp_i] = (coeffs_0 * source[dsp_phase_index - 1]
			  + coeffs_1 * source[dsp_phase_index]
			  + coeffs_2 * source[dsp_phase_index + 1]
			  + coeffs_3 * source[dsp_phase_index + 2]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_4th_order_util_fill_s32:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate 4th order on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_4th_order_util_fill_s32(gint32 *destination,
					      gint32 *source,
					      guint buffer_length,
					      gdouble phase_incr)
{
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }
  
  ags_fluid_interpolate_4th_order_util_config();
  
  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    destination[dsp_i] = (coeffs_0 * start_point
			  + coeffs_1 * source[dsp_phase_index]
			  + coeffs_2 * source[dsp_phase_index + 1]
			  + coeffs_3 * source[dsp_phase_index + 2]);
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    destination[dsp_i] = (coeffs_0 * source[dsp_phase_index - 1]
			  + coeffs_1 * source[dsp_phase_index]
			  + coeffs_2 * source[dsp_phase_index + 1]
			  + coeffs_3 * source[dsp_phase_index + 2]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_4th_order_util_fill_s64:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate 4th order on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_4th_order_util_fill_s64(gint64 *destination,
					      gint64 *source,
					      guint buffer_length,
					      gdouble phase_incr)
{
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }
  
  ags_fluid_interpolate_4th_order_util_config();
  
  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    destination[dsp_i] = (coeffs_0 * start_point
			  + coeffs_1 * source[dsp_phase_index]
			  + coeffs_2 * source[dsp_phase_index + 1]
			  + coeffs_3 * source[dsp_phase_index + 2]);
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    destination[dsp_i] = (coeffs_0 * source[dsp_phase_index - 1]
			  + coeffs_1 * source[dsp_phase_index]
			  + coeffs_2 * source[dsp_phase_index + 1]
			  + coeffs_3 * source[dsp_phase_index + 2]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_4th_order_util_fill_float:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate 4th order on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_4th_order_util_fill_float(gfloat *destination,
						gfloat *source,
						guint buffer_length,
						gdouble phase_incr)
{
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }
  
  ags_fluid_interpolate_4th_order_util_config();
  
  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    destination[dsp_i] = (coeffs_0 * start_point
			  + coeffs_1 * source[dsp_phase_index]
			  + coeffs_2 * source[dsp_phase_index + 1]
			  + coeffs_3 * source[dsp_phase_index + 2]);
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    destination[dsp_i] = (coeffs_0 * source[dsp_phase_index - 1]
			  + coeffs_1 * source[dsp_phase_index]
			  + coeffs_2 * source[dsp_phase_index + 1]
			  + coeffs_3 * source[dsp_phase_index + 2]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_4th_order_util_fill_double:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate 4th order on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_4th_order_util_fill_double(gdouble *destination,
						 gdouble *source,
						 guint buffer_length,
						 gdouble phase_incr)
{
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }
  
  ags_fluid_interpolate_4th_order_util_config();
  
  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    destination[dsp_i] = (coeffs_0 * start_point
			  + coeffs_1 * source[dsp_phase_index]
			  + coeffs_2 * source[dsp_phase_index + 1]
			  + coeffs_3 * source[dsp_phase_index + 2]);
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    destination[dsp_i] = (coeffs_0 * source[dsp_phase_index - 1]
			  + coeffs_1 * source[dsp_phase_index]
			  + coeffs_2 * source[dsp_phase_index + 1]
			  + coeffs_3 * source[dsp_phase_index + 2]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_4th_order_util_fill_complex:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate 4th order on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_4th_order_util_fill_complex(AgsComplex *destination,
						  AgsComplex *source,
						  guint buffer_length,
						  gdouble phase_incr)
{
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  double _Complex start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }
  
  ags_fluid_interpolate_4th_order_util_config();
  
  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = ags_complex_get(source);

  end_point1 = ags_complex_get(source + end_index);
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    ags_complex_set(destination + dsp_i, (coeffs_0 * start_point
					  + coeffs_1 * ags_complex_get(source + dsp_phase_index)
					  + coeffs_2 * ags_complex_get(source + dsp_phase_index + 1),
					  + coeffs_3 * ags_complex_get(source + dsp_phase_index + 2)));
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    ags_complex_set(destination + dsp_i, (coeffs_0 * ags_complex_get(source + dsp_phase_index - 1)
					  + coeffs_1 * ags_complex_get(source + dsp_phase_index)
					  + coeffs_2 * ags_complex_get(source + dsp_phase_index + 1)
					  + coeffs_3 * ags_complex_get(source + dsp_phase_index + 2)));
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);
  }
}
