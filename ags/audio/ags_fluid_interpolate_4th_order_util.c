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

/* 4th Order interpolation table (2 coefficients centered on 1st) */
gboolean interp_coeff_4th_order_initialized = FALSE;

gdouble interp_coeff_4th_order[AGS_FLUID_INTERP_MAX][4];

GMutex interp_coeff_4th_order_mutex;

/**
 * SECTION:ags_fluid_interpolate_4th_order_util
 * @short_description: util functions to fluid interpolate 4th_order
 * @title: AgsFluidInterpolate4th_OrderUtil
 * @section_id:
 * @include: ags/audio/ags_fluid_interpolate_4th_order_util.h
 *
 * These utility functions allow you to fill fluid interpolated
 * 4th order data.
 */

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

  for(i = 0; i < FLUID_INTERP_MAX; i++){
    x = (double) i / (double) FLUID_INTERP_MAX;

    interp_coeff_4th_order[i][0] = (x * (-0.5 + x * (1 - 0.5 * x)));
    interp_coeff_4th_order[i][1] = (1.0 + x * x * (1.5 * x - 2.5));
    interp_coeff_4th_order[i][2] = (x * (0.5 + x * (2.0 - 1.5 * x)));
    interp_coeff_4th_order[i][3] = (0.5 * x * x * (x - 1.0));
  }
  
  interp_coeff_4th_order_initialized = TRUE;
  
  g_mutex_unlock(&interp_coeff_4th_order_mutex);
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
  guint end_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }
  
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
    
    dsp_buf[dsp_i] = (coeffs_0 * start_point
		      + coeffs_1 * source[dsp_phase_index]
		      + coeffs_2 * source[dsp_phase_index + 1]
		      + coeffs_3 * source[dsp_phase_index + 2]);
    
    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < FLUID_BUFSIZE && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    dsp_buf[dsp_i] = (coeffs_0 * source[dsp_phase_index - 1]
		      + coeffs_1 * source[dsp_phase_index]
		      + coeffs_2 * source[dsp_phase_index + 1]
		      + coeffs_3 * source[dsp_phase_index + 2]);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
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
  guint end_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }
  
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
    
    dsp_buf[dsp_i] = (coeffs_0 * start_point
		      + coeffs_1 * source[dsp_phase_index]
		      + coeffs_2 * source[dsp_phase_index + 1]
		      + coeffs_3 * source[dsp_phase_index + 2]);
    
    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < FLUID_BUFSIZE && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    dsp_buf[dsp_i] = (coeffs_0 * source[dsp_phase_index - 1]
		      + coeffs_1 * source[dsp_phase_index]
		      + coeffs_2 * source[dsp_phase_index + 1]
		      + coeffs_3 * source[dsp_phase_index + 2]);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
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
  guint end_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }
  
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
    
    dsp_buf[dsp_i] = (coeffs_0 * start_point
		      + coeffs_1 * source[dsp_phase_index]
		      + coeffs_2 * source[dsp_phase_index + 1]
		      + coeffs_3 * source[dsp_phase_index + 2]);
    
    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < FLUID_BUFSIZE && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    dsp_buf[dsp_i] = (coeffs_0 * source[dsp_phase_index - 1]
		      + coeffs_1 * source[dsp_phase_index]
		      + coeffs_2 * source[dsp_phase_index + 1]
		      + coeffs_3 * source[dsp_phase_index + 2]);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
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
  guint end_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }
  
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
    
    dsp_buf[dsp_i] = (coeffs_0 * start_point
		      + coeffs_1 * source[dsp_phase_index]
		      + coeffs_2 * source[dsp_phase_index + 1]
		      + coeffs_3 * source[dsp_phase_index + 2]);
    
    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < FLUID_BUFSIZE && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    dsp_buf[dsp_i] = (coeffs_0 * source[dsp_phase_index - 1]
		      + coeffs_1 * source[dsp_phase_index]
		      + coeffs_2 * source[dsp_phase_index + 1]
		      + coeffs_3 * source[dsp_phase_index + 2]);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
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
  guint end_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }
  
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
    
    dsp_buf[dsp_i] = (coeffs_0 * start_point
		      + coeffs_1 * source[dsp_phase_index]
		      + coeffs_2 * source[dsp_phase_index + 1]
		      + coeffs_3 * source[dsp_phase_index + 2]);
    
    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < FLUID_BUFSIZE && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    dsp_buf[dsp_i] = (coeffs_0 * source[dsp_phase_index - 1]
		      + coeffs_1 * source[dsp_phase_index]
		      + coeffs_2 * source[dsp_phase_index + 1]
		      + coeffs_3 * source[dsp_phase_index + 2]);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
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
  guint end_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }
  
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
    
    dsp_buf[dsp_i] = (coeffs_0 * start_point
		      + coeffs_1 * source[dsp_phase_index]
		      + coeffs_2 * source[dsp_phase_index + 1]
		      + coeffs_3 * source[dsp_phase_index + 2]);
    
    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < FLUID_BUFSIZE && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    dsp_buf[dsp_i] = (coeffs_0 * source[dsp_phase_index - 1]
		      + coeffs_1 * source[dsp_phase_index]
		      + coeffs_2 * source[dsp_phase_index + 1]
		      + coeffs_3 * source[dsp_phase_index + 2]);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
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
  guint end_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }
  
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
    
    dsp_buf[dsp_i] = (coeffs_0 * start_point
		      + coeffs_1 * source[dsp_phase_index]
		      + coeffs_2 * source[dsp_phase_index + 1]
		      + coeffs_3 * source[dsp_phase_index + 2]);
    
    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < FLUID_BUFSIZE && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    dsp_buf[dsp_i] = (coeffs_0 * source[dsp_phase_index - 1]
		      + coeffs_1 * source[dsp_phase_index]
		      + coeffs_2 * source[dsp_phase_index + 1]
		      + coeffs_3 * source[dsp_phase_index + 2]);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
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
  guint end_index;
  guint start_index, end_index;
  double _Complex start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }
  
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
    
    dsp_buf[dsp_i] = (coeffs_0 * start_point
		      + coeffs_1 * ags_complex_get(source + dsp_phase_index)
		      + coeffs_2 * ags_complex_get(source + dsp_phase_index + 1),
		      + coeffs_3 * ags_complex_get(source + dsp_phase_index + 2));
    
    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < FLUID_BUFSIZE && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    dsp_buf[dsp_i] = (coeffs_0 * ags_complex_get(source + dsp_phase_index - 1)
		      + coeffs_1 * ags_complex_get(source + dsp_phase_index)
		      + coeffs_2 * ags_complex_get(source + dsp_phase_index + 1)
		      + coeffs_3 * ags_complex_get(source + dsp_phase_index + 2));
    
    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }
}
