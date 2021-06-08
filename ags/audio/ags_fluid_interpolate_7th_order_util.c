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
 */

#include <ags/audio/ags_fluid_interpolate_7th_order_util.h>

/* 7th order interpolation (7 coefficients centered on 3rd) */
gboolean interp_coeff_7th_order_initialized = FALSE;

gdouble interp_coeff_7th_order[AGS_FLUID_INTERP_MAX][AGS_FLUID_SINC_INTERP_ORDER];

GMutex interp_coeff_7th_order_mutex;

/**
 * SECTION:ags_fluid_interpolate_7th_order_util
 * @short_description: util functions to fluid interpolate 7th_order
 * @title: AgsFluidInterpolate7th_OrderUtil
 * @section_id:
 * @include: ags/audio/ags_fluid_interpolate_7th_order_util.h
 *
 * These utility functions allow you to fill fluid interpolated
 * 7th order data.
 */

/* Initializes interpolation tables */
void
ags_fluid_interpolate_7th_order_util_config()
{
  gdouble x, v;
  gdouble i_shifted;
  int i, i2;
  
  g_mutex_lock(&interp_coeff_7th_order_mutex);
  
  if(interp_coeff_7th_order_initialized){
    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    return;
  }

  /* i: Offset in terms of whole samples */
  for(i = 0; i < AGS_FLUID_SINC_INTERP_ORDER; i++){
    /* i2: Offset in terms of fractional samples ('subsamples') */
    for(i2 = 0; i2 < AGS_FLUID_INTERP_MAX; i2++){
      /* center on middle of table */
      i_shifted = (gdouble)i - ((gdouble)AGS_FLUID_SINC_INTERP_ORDER / 2.0)
	+ (gdouble)i2 / (gdouble)AGS_FLUID_INTERP_MAX;

      /* sinc(0) cannot be calculated straightforward (limit needed for 0/0) */
      if(fabs(i_shifted) > 0.000001){
	gdouble arg = M_PI * i_shifted;
	
	v = sin(arg) / (arg);
	/* Hanning window */
	v *= 0.5 * (1.0 + cos(2.0 * arg / (gdouble)AGS_FLUID_SINC_INTERP_ORDER));
      }else{
	v = 1.0;
      }

      interp_coeff_7th_order[AGS_FLUID_INTERP_MAX - i2 - 1][i] = v;
    }
  }
  
  interp_coeff_7th_order_initialized = TRUE;
  
  g_mutex_unlock(&interp_coeff_7th_order_mutex);
}

/**
 * ags_fluid_interpolate_7th_order_util_fill_s8:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate 7th order on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_7th_order_util_fill_s8(gint8 *destination,
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
  gdouble start_points[3], end_points[3];
  gdouble *coeffs;

  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;

  start_points[0] = source[0];
  start_points[1] = start_points[0];
  start_points[2] = start_points[0];
  
  end_points[0] = source[end_index];
  end_points[1] = end_points[0];
  end_points[2] = end_points[0];

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[2]
		      + coeffs[1] * start_points[1]
		      + coeffs[2] * start_points[0]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  start_index++;

  /* interpolate 2nd to first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[1]
		      + coeffs[1] * start_points[0]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  start_index++;

  /* interpolate 3rd to first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[0]
		      + coeffs[1] * source[dsp_phase_index - 2]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  /* set back to original start index */
  start_index -= 2;

  /* interpolate the sequence of sample points */
  for(; dsp_i < FLUID_BUFSIZE && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * source[dsp_phase_index - 3]
		      + coeffs[1] * source[dsp_phase_index - 2]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_7th_order_util_fill_s16:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate 7th order on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_7th_order_util_fill_s16(gint16 *destination,
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
  gdouble start_points[3], end_points[3];
  gdouble *coeffs;

  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;

  start_points[0] = source[0];
  start_points[1] = start_points[0];
  start_points[2] = start_points[0];
  
  end_points[0] = source[end_index];
  end_points[1] = end_points[0];
  end_points[2] = end_points[0];

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[2]
		      + coeffs[1] * start_points[1]
		      + coeffs[2] * start_points[0]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  start_index++;

  /* interpolate 2nd to first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[1]
		      + coeffs[1] * start_points[0]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  start_index++;

  /* interpolate 3rd to first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[0]
		      + coeffs[1] * source[dsp_phase_index - 2]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  /* set back to original start index */
  start_index -= 2;

  /* interpolate the sequence of sample points */
  for(; dsp_i < FLUID_BUFSIZE && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * source[dsp_phase_index - 3]
		      + coeffs[1] * source[dsp_phase_index - 2]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_7th_order_util_fill_s24:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate 7th order on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_7th_order_util_fill_s24(gint32 *destination,
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
  gdouble start_points[3], end_points[3];
  gdouble *coeffs;

  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;

  start_points[0] = source[0];
  start_points[1] = start_points[0];
  start_points[2] = start_points[0];
  
  end_points[0] = source[end_index];
  end_points[1] = end_points[0];
  end_points[2] = end_points[0];

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[2]
		      + coeffs[1] * start_points[1]
		      + coeffs[2] * start_points[0]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  start_index++;

  /* interpolate 2nd to first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[1]
		      + coeffs[1] * start_points[0]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  start_index++;

  /* interpolate 3rd to first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[0]
		      + coeffs[1] * source[dsp_phase_index - 2]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  /* set back to original start index */
  start_index -= 2;

  /* interpolate the sequence of sample points */
  for(; dsp_i < FLUID_BUFSIZE && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * source[dsp_phase_index - 3]
		      + coeffs[1] * source[dsp_phase_index - 2]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_7th_order_util_fill_s32:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate 7th order on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_7th_order_util_fill_s32(gint32 *destination,
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
  gdouble start_points[3], end_points[3];
  gdouble *coeffs;

  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;

  start_points[0] = source[0];
  start_points[1] = start_points[0];
  start_points[2] = start_points[0];
  
  end_points[0] = source[end_index];
  end_points[1] = end_points[0];
  end_points[2] = end_points[0];

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[2]
		      + coeffs[1] * start_points[1]
		      + coeffs[2] * start_points[0]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  start_index++;

  /* interpolate 2nd to first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[1]
		      + coeffs[1] * start_points[0]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  start_index++;

  /* interpolate 3rd to first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[0]
		      + coeffs[1] * source[dsp_phase_index - 2]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  /* set back to original start index */
  start_index -= 2;

  /* interpolate the sequence of sample points */
  for(; dsp_i < FLUID_BUFSIZE && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * source[dsp_phase_index - 3]
		      + coeffs[1] * source[dsp_phase_index - 2]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_7th_order_util_fill_s64:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate 7th order on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_7th_order_util_fill_s64(gint64 *destination,
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
  gdouble start_points[3], end_points[3];
  gdouble *coeffs;

  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;

  start_points[0] = source[0];
  start_points[1] = start_points[0];
  start_points[2] = start_points[0];
  
  end_points[0] = source[end_index];
  end_points[1] = end_points[0];
  end_points[2] = end_points[0];

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[2]
		      + coeffs[1] * start_points[1]
		      + coeffs[2] * start_points[0]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  start_index++;

  /* interpolate 2nd to first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[1]
		      + coeffs[1] * start_points[0]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  start_index++;

  /* interpolate 3rd to first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[0]
		      + coeffs[1] * source[dsp_phase_index - 2]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  /* set back to original start index */
  start_index -= 2;

  /* interpolate the sequence of sample points */
  for(; dsp_i < FLUID_BUFSIZE && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * source[dsp_phase_index - 3]
		      + coeffs[1] * source[dsp_phase_index - 2]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_7th_order_util_fill_float:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate 7th order on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_7th_order_util_fill_float(gfloat *destination,
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
  gdouble start_points[3], end_points[3];
  gdouble *coeffs;

  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;

  start_points[0] = source[0];
  start_points[1] = start_points[0];
  start_points[2] = start_points[0];
  
  end_points[0] = source[end_index];
  end_points[1] = end_points[0];
  end_points[2] = end_points[0];

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[2]
		      + coeffs[1] * start_points[1]
		      + coeffs[2] * start_points[0]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  start_index++;

  /* interpolate 2nd to first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[1]
		      + coeffs[1] * start_points[0]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  start_index++;

  /* interpolate 3rd to first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[0]
		      + coeffs[1] * source[dsp_phase_index - 2]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  /* set back to original start index */
  start_index -= 2;

  /* interpolate the sequence of sample points */
  for(; dsp_i < FLUID_BUFSIZE && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * source[dsp_phase_index - 3]
		      + coeffs[1] * source[dsp_phase_index - 2]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_7th_order_util_fill_double:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate 7th order on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_7th_order_util_fill_double(gdouble *destination,
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
  gdouble start_points[3], end_points[3];
  gdouble *coeffs;

  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;

  start_points[0] = source[0];
  start_points[1] = start_points[0];
  start_points[2] = start_points[0];
  
  end_points[0] = source[end_index];
  end_points[1] = end_points[0];
  end_points[2] = end_points[0];

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[2]
		      + coeffs[1] * start_points[1]
		      + coeffs[2] * start_points[0]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  start_index++;

  /* interpolate 2nd to first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[1]
		      + coeffs[1] * start_points[0]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  start_index++;

  /* interpolate 3rd to first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[0]
		      + coeffs[1] * source[dsp_phase_index - 2]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  /* set back to original start index */
  start_index -= 2;

  /* interpolate the sequence of sample points */
  for(; dsp_i < FLUID_BUFSIZE && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * source[dsp_phase_index - 3]
		      + coeffs[1] * source[dsp_phase_index - 2]
		      + coeffs[2] * source[dsp_phase_index - 1]
		      + coeffs[3] * source[dsp_phase_index]
		      + coeffs[4] * source[dsp_phase_index + 1]
		      + coeffs[5] * source[dsp_phase_index + 2]
		      + coeffs[6] * source[dsp_phase_index + 3]);

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }
}

/**
 * ags_fluid_interpolate_7th_order_util_fill_complex:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * @phase_incr: the phase increment
 * 
 * Perform fluid interpolate 7th order on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_interpolate_7th_order_util_fill_complex(AgsComplex *destination,
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
  double _Complex start_points[3], end_points[3];
  gdouble *coeffs;

  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;

  start_points[0] = ags_complex_get(source);
  start_points[1] = start_points[0];
  start_points[2] = start_points[0];
  
  end_points[0] = source[end_index];
  end_points[1] = end_points[0];
  end_points[2] = end_points[0];

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[2]
		      + coeffs[1] * start_points[1]
		      + coeffs[2] * start_points[0]
		      + coeffs[3] * ags_complex_get(source + dsp_phase_index)
		      + coeffs[4] * ags_complex_get(source + dsp_phase_index + 1)
		      + coeffs[5] * ags_complex_get(source + dsp_phase_index + 2)
		      + coeffs[6] * ags_complex_get(source + dsp_phase_index + 3));

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  start_index++;

  /* interpolate 2nd to first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++){
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[1]
		      + coeffs[1] * start_points[0]
		      + coeffs[2] * ags_complex_get(source + dsp_phase_index - 1)
		      + coeffs[3] * ags_complex_get(source + dsp_phase_index)
		      + coeffs[4] * ags_complex_get(source + dsp_phase_index + 1)
		      + coeffs[5] * ags_complex_get(source + dsp_phase_index + 2)
		      + coeffs[6] * ags_complex_get(source + dsp_phase_index + 3));

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  start_index++;

  /* interpolate 3rd to first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < FLUID_BUFSIZE; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * start_points[0]
		      + coeffs[1] * ags_complex_get(source + dsp_phase_index - 2)
		      + coeffs[2] * ags_complex_get(source + dsp_phase_index - 1)
		      + coeffs[3] * ags_complex_get(source + dsp_phase_index)
		      + coeffs[4] * ags_complex_get(source + dsp_phase_index + 1)
		      + coeffs[5] * ags_complex_get(source + dsp_phase_index + 2)
		      + coeffs[6] * ags_complex_get(source + dsp_phase_index + 3));

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }

  /* set back to original start index */
  start_index -= 2;

  /* interpolate the sequence of sample points */
  for(; dsp_i < FLUID_BUFSIZE && dsp_phase_index <= end_index; dsp_i++)
  {
    gint row;
    
    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_7th_order_mutex);

    coeffs = interp_coeff_7th_order[row];

    dsp_buf[dsp_i] = (coeffs[0] * ags_complex_get(source + dsp_phase_index - 3)
		      + coeffs[1] * ags_complex_get(source + dsp_phase_index - 2)
		      + coeffs[2] * ags_complex_get(source + dsp_phase_index - 1)
		      + coeffs[3] * ags_complex_get(source + dsp_phase_index)
		      + coeffs[4] * ags_complex_get(source + dsp_phase_index + 1)
		      + coeffs[5] * ags_complex_get(source + dsp_phase_index + 2)
		      + coeffs[6] * ags_complex_get(source + dsp_phase_index + 3));

    g_mutex_unlock(&interp_coeff_7th_order_mutex);

    /* increment phase */
    fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = fluid_phase_index(dsp_phase);
  }
}
