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

  ptr->base_key = 0.0;
  ptr->tuning = 0.0;

  ptr->phase_increment = 0.0;

  ptr->vibrato_enabled = TRUE;

  ptr->vibrato_gain = 1.0;
  ptr->vibrato_lfo_depth = 1.0;
  ptr->vibrato_lfo_freq = 8.172;
  ptr->vibrato_tuning = 0.0;

  ptr->vibrato_lfo_frame_count = ptr->samplerate / ptr->vibrato_lfo_freq;
  ptr->vibrato_lfo_offset = 0;

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

  new_ptr->base_key = ptr->base_key;
  new_ptr->tuning = ptr->tuning;

  new_ptr->phase_increment = ptr->phase_increment;

  new_ptr->vibrato_enabled = ptr->vibrato_enabled;

  new_ptr->vibrato_gain = ptr->vibrato_gain;
  new_ptr->vibrato_lfo_depth = ptr->vibrato_lfo_depth;
  new_ptr->vibrato_lfo_freq = ptr->vibrato_lfo_freq;
  new_ptr->vibrato_tuning = ptr->vibrato_tuning;

  new_ptr->vibrato_lfo_frame_count = ptr->vibrato_lfo_frame_count;
  new_ptr->vibrato_lfo_offset = ptr->vibrato_lfo_offset;
  
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
AgsSoundcardFormat
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
						AgsSoundcardFormat format)
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

  fluid_interpolate_4th_order_util->vibrato_lfo_frame_count = fluid_interpolate_4th_order_util->samplerate / fluid_interpolate_4th_order_util->vibrato_lfo_freq;
}

/**
 * ags_fluid_interpolate_4th_order_util_get_base_key:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Get base key of @fluid_interpolate_4th_order_util.
 * 
 * Returns: the base key
 * 
 * Since: 4.0.0
 */
gdouble
ags_fluid_interpolate_4th_order_util_get_base_key(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return(0.0);
  }

  return(fluid_interpolate_4th_order_util->base_key);
}

/**
 * ags_fluid_interpolate_4th_order_util_set_base_key:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * @base_key: the base key
 *
 * Set @base_key of @fluid_interpolate_4th_order_util.
 *
 * Since: 4.0.0
 */
void
ags_fluid_interpolate_4th_order_util_set_base_key(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
						  gdouble base_key)
{
  gdouble root_pitch_hz;
  gdouble phase_incr;

  if(fluid_interpolate_4th_order_util == NULL){
    return;
  }

  fluid_interpolate_4th_order_util->base_key = base_key;

  root_pitch_hz = exp2(((double) base_key - 48.0) / 12.0) * 440.0;

  phase_incr = (exp2((((double) base_key - 48.0 + (fluid_interpolate_4th_order_util->tuning / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
  if(phase_incr == 0.0){
    phase_incr = 1.0;
  }

  fluid_interpolate_4th_order_util->phase_increment = phase_incr;
}

/**
 * ags_fluid_interpolate_4th_order_util_get_tuning:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Get tuning of @fluid_interpolate_4th_order_util.
 * 
 * Returns: the tuning
 * 
 * Since: 4.0.0
 */
gdouble
ags_fluid_interpolate_4th_order_util_get_tuning(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return(0.0);
  }

  return(fluid_interpolate_4th_order_util->tuning);
}

/**
 * ags_fluid_interpolate_4th_order_util_set_tuning:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * @tuning: the tuning
 *
 * Set @tuning of @fluid_interpolate_4th_order_util.
 *
 * Since: 4.0.0
 */
void
ags_fluid_interpolate_4th_order_util_set_tuning(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
						gdouble tuning)
{
  gdouble root_pitch_hz;
  gdouble phase_incr;

  if(fluid_interpolate_4th_order_util == NULL){
    return;
  }

  fluid_interpolate_4th_order_util->tuning = tuning;

  root_pitch_hz = exp2(((double) fluid_interpolate_4th_order_util->base_key - 48.0) / 12.0) * 440.0;
  
  phase_incr = (exp2((((double) fluid_interpolate_4th_order_util->base_key - 48.0 + (tuning / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
  if(phase_incr == 0.0){
    phase_incr = 1.0;
  }

  fluid_interpolate_4th_order_util->phase_increment = phase_incr;
}

/**
 * ags_fluid_interpolate_4th_order_util_get_phase_increment:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Get phase increment of @fluid_interpolate_4th_order_util.
 * 
 * Returns: the phase increment
 * 
 * Since: 3.9.6
 */
gdouble
ags_fluid_interpolate_4th_order_util_get_phase_increment(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return(0.0);
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
 * ags_fluid_interpolate_4th_order_util_get_vibrato_enabled:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Get vibrato enabled of @fluid_interpolate_4th_order_util.
 * 
 * Returns: %TRUE if the vibrato is enabled, otherwise %FALSE
 * 
 * Since: 5.2.0
 */
gboolean
ags_fluid_interpolate_4th_order_util_get_vibrato_enabled(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return(FALSE);
  }

  return(fluid_interpolate_4th_order_util->vibrato_enabled);
}

/**
 * ags_fluid_interpolate_4th_order_util_set_vibrato_enabled:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * @vibrato_enabled: the vibrato enabled
 *
 * Set @vibrato_enabled of @fluid_interpolate_4th_order_util.
 *
 * Since: 5.2.0
 */
void
ags_fluid_interpolate_4th_order_util_set_vibrato_enabled(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							 gboolean vibrato_enabled)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return;
  }

  fluid_interpolate_4th_order_util->vibrato_enabled = vibrato_enabled;
}

/**
 * ags_fluid_interpolate_4th_order_util_get_vibrato_gain:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Get vibrato gain of @fluid_interpolate_4th_order_util.
 * 
 * Returns: the vibrato gain
 * 
 * Since: 5.2.0
 */
gdouble
ags_fluid_interpolate_4th_order_util_get_vibrato_gain(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return(0.0);
  }

  return(fluid_interpolate_4th_order_util->vibrato_gain);
}

/**
 * ags_fluid_interpolate_4th_order_util_set_vibrato_gain:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * @vibrato_gain: the vibrato gain
 *
 * Set @vibrato_gain of @fluid_interpolate_4th_order_util.
 *
 * Since: 5.2.0
 */
void
ags_fluid_interpolate_4th_order_util_set_vibrato_gain(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
						      gdouble vibrato_gain)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return;
  }

  fluid_interpolate_4th_order_util->vibrato_gain = vibrato_gain;
}

/**
 * ags_fluid_interpolate_4th_order_util_get_vibrato_lfo_depth:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Get vibrato LFO depth of @fluid_interpolate_4th_order_util.
 * 
 * Returns: the vibrato LFO depth
 * 
 * Since: 5.2.0
 */
gdouble
ags_fluid_interpolate_4th_order_util_get_vibrato_lfo_depth(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return(0.0);
  }

  return(fluid_interpolate_4th_order_util->vibrato_lfo_depth);
}

/**
 * ags_fluid_interpolate_4th_order_util_set_vibrato_lfo_depth:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * @vibrato_lfo_depth: the vibrato LFO depth
 *
 * Set @vibrato_lfo_depth of @fluid_interpolate_4th_order_util.
 *
 * Since: 5.2.0
 */
void
ags_fluid_interpolate_4th_order_util_set_vibrato_lfo_depth(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							   gdouble vibrato_lfo_depth)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return;
  }

  fluid_interpolate_4th_order_util->vibrato_lfo_depth = vibrato_lfo_depth;
}

/**
 * ags_fluid_interpolate_4th_order_util_get_vibrato_lfo_freq:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Get vibrato LFO frequency of @fluid_interpolate_4th_order_util.
 * 
 * Returns: the vibrato LFO frequency
 * 
 * Since: 5.2.0
 */
gdouble
ags_fluid_interpolate_4th_order_util_get_vibrato_lfo_freq(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return(0.0);
  }

  return(fluid_interpolate_4th_order_util->vibrato_lfo_freq);
}

/**
 * ags_fluid_interpolate_4th_order_util_set_vibrato_lfo_freq:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * @vibrato_lfo_freq: the vibrato LFO freq
 *
 * Set @vibrato_lfo_freq of @fluid_interpolate_4th_order_util.
 *
 * Since: 5.2.0
 */
void
ags_fluid_interpolate_4th_order_util_set_vibrato_lfo_freq(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							  gdouble vibrato_lfo_freq)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return;
  }

  fluid_interpolate_4th_order_util->vibrato_lfo_freq = vibrato_lfo_freq;
}

/**
 * ags_fluid_interpolate_4th_order_util_get_vibrato_tuning:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Get vibrato tuning of @fluid_interpolate_4th_order_util.
 * 
 * Returns: the vibrato tuning
 * 
 * Since: 5.2.0
 */
gdouble
ags_fluid_interpolate_4th_order_util_get_vibrato_tuning(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return(0.0);
  }

  return(fluid_interpolate_4th_order_util->vibrato_tuning);
}

/**
 * ags_fluid_interpolate_4th_order_util_set_vibrato_tuning:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * @vibrato_tuning: the vibrato tuning
 *
 * Set @vibrato_tuning of @fluid_interpolate_4th_order_util.
 *
 * Since: 5.2.0
 */
void
ags_fluid_interpolate_4th_order_util_set_vibrato_tuning(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							gdouble vibrato_tuning)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return;
  }

  fluid_interpolate_4th_order_util->vibrato_tuning = vibrato_tuning;
}

/**
 * ags_fluid_interpolate_4th_order_util_get_vibrato_lfo_offset:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * 
 * Get vibrato lfo_offset of @fluid_interpolate_4th_order_util.
 * 
 * Returns: the vibrato lfo_offset
 * 
 * Since: 5.2.4
 */
guint
ags_fluid_interpolate_4th_order_util_get_vibrato_lfo_offset(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return(0);
  }

  return(fluid_interpolate_4th_order_util->vibrato_lfo_offset);
}

/**
 * ags_fluid_interpolate_4th_order_util_set_vibrato_lfo_offset:
 * @fluid_interpolate_4th_order_util: the #AgsFluidInterpolate4thOrderUtil-struct
 * @vibrato_lfo_offset: the vibrato lfo_offset
 *
 * Set @vibrato_lfo_offset of @fluid_interpolate_4th_order_util.
 *
 * Since: 5.2.4
 */
void
ags_fluid_interpolate_4th_order_util_set_vibrato_lfo_offset(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							    guint vibrato_lfo_offset)
{
  if(fluid_interpolate_4th_order_util == NULL){
    return;
  }

  fluid_interpolate_4th_order_util->vibrato_lfo_offset = vibrato_lfo_offset;
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
  gint8 *destination, *source;
  
  guint samplerate;
  guint destination_stride, source_stride;
  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint vibrato_lfo_offset;
  guint buffer_length;
  gdouble phase_incr;
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(fluid_interpolate_4th_order_util == NULL ||
     fluid_interpolate_4th_order_util->destination == NULL ||
     fluid_interpolate_4th_order_util->source == NULL){
    return;
  }

  destination = fluid_interpolate_4th_order_util->destination;
  destination_stride = fluid_interpolate_4th_order_util->destination_stride;

  source = fluid_interpolate_4th_order_util->source;
  source_stride = fluid_interpolate_4th_order_util->source_stride;

  buffer_length = fluid_interpolate_4th_order_util->buffer_length;

  phase_incr = fluid_interpolate_4th_order_util->phase_increment;

  ags_fluid_interpolate_4th_order_util_config();
  
  samplerate = fluid_interpolate_4th_order_util->samplerate;

  base_key = fluid_interpolate_4th_order_util->base_key;
  tuning = fluid_interpolate_4th_order_util->tuning;

  vibrato_gain = fluid_interpolate_4th_order_util->vibrato_gain;
  vibrato_lfo_depth = fluid_interpolate_4th_order_util->vibrato_lfo_depth;
  vibrato_lfo_freq = fluid_interpolate_4th_order_util->vibrato_lfo_freq;
  vibrato_tuning = fluid_interpolate_4th_order_util->vibrato_tuning;
  vibrato_lfo_offset = fluid_interpolate_4th_order_util->vibrato_lfo_offset;

  if(fluid_interpolate_4th_order_util->vibrato_enabled == FALSE){
    vibrato_gain = 0.0;
  }

  dsp_phase = 0;
  
  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index * source_stride];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  root_pitch_hz = exp2(((double) fluid_interpolate_4th_order_util->base_key - 48.0) / 12.0) * 440.0;

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gdouble phase_incr;

    gint row;
    
    phase_incr = (exp2((((double) base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((vibrato_lfo_offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    destination[dsp_i * destination_stride] = (coeffs_0 * start_point
					       + coeffs_1 * source[dsp_phase_index * source_stride]
					       + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
					       + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    vibrato_lfo_offset += 1;
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gdouble phase_incr;

    gint row;
    
    phase_incr = (exp2((((double) base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((vibrato_lfo_offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    destination[dsp_i * destination_stride] = (coeffs_0 * source[(dsp_phase_index - 1) * source_stride]
					       + coeffs_1 * source[dsp_phase_index * source_stride]
					       + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
					       + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    vibrato_lfo_offset += 1;
  }

  fluid_interpolate_4th_order_util->vibrato_lfo_offset = vibrato_lfo_offset;
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
  gint16 *destination, *source;
  
  guint samplerate;
  guint destination_stride, source_stride;
  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint vibrato_lfo_offset;
  guint buffer_length;
  gdouble phase_incr;
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(fluid_interpolate_4th_order_util == NULL ||
     fluid_interpolate_4th_order_util->destination == NULL ||
     fluid_interpolate_4th_order_util->source == NULL){
    return;
  }

  destination = fluid_interpolate_4th_order_util->destination;
  destination_stride = fluid_interpolate_4th_order_util->destination_stride;

  source = fluid_interpolate_4th_order_util->source;
  source_stride = fluid_interpolate_4th_order_util->source_stride;

  buffer_length = fluid_interpolate_4th_order_util->buffer_length;

  phase_incr = fluid_interpolate_4th_order_util->phase_increment;

  ags_fluid_interpolate_4th_order_util_config();
  
  samplerate = fluid_interpolate_4th_order_util->samplerate;

  base_key = fluid_interpolate_4th_order_util->base_key;
  tuning = fluid_interpolate_4th_order_util->tuning;

  vibrato_gain = fluid_interpolate_4th_order_util->vibrato_gain;
  vibrato_lfo_depth = fluid_interpolate_4th_order_util->vibrato_lfo_depth;
  vibrato_lfo_freq = fluid_interpolate_4th_order_util->vibrato_lfo_freq;
  vibrato_tuning = fluid_interpolate_4th_order_util->vibrato_tuning;
  vibrato_lfo_offset = fluid_interpolate_4th_order_util->vibrato_lfo_offset;

  if(fluid_interpolate_4th_order_util->vibrato_enabled == FALSE){
    vibrato_gain = 0.0;
  }

  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index * source_stride];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  root_pitch_hz = exp2(((double) fluid_interpolate_4th_order_util->base_key - 48.0) / 12.0) * 440.0;

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gdouble phase_incr;

    gint row;
        
    phase_incr = (exp2((((double) base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((vibrato_lfo_offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    destination[dsp_i * destination_stride] = (coeffs_0 * start_point
					       + coeffs_1 * source[dsp_phase_index * source_stride]
					       + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
					       + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    vibrato_lfo_offset += 1;
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gdouble phase_incr;

    gint row;
    
    phase_incr = (exp2((((double) base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((vibrato_lfo_offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    destination[dsp_i * destination_stride] = (coeffs_0 * source[(dsp_phase_index - 1) * source_stride]
					       + coeffs_1 * source[dsp_phase_index * source_stride]
					       + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
					       + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    vibrato_lfo_offset += 1;
  }

  fluid_interpolate_4th_order_util->vibrato_lfo_offset = vibrato_lfo_offset;
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
  gint32 *destination, *source;
  
  guint samplerate;
  guint destination_stride, source_stride;
  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint vibrato_lfo_offset;
  guint buffer_length;
  gdouble phase_incr;
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(fluid_interpolate_4th_order_util == NULL ||
     fluid_interpolate_4th_order_util->destination == NULL ||
     fluid_interpolate_4th_order_util->source == NULL){
    return;
  }

  destination = fluid_interpolate_4th_order_util->destination;
  destination_stride = fluid_interpolate_4th_order_util->destination_stride;

  source = fluid_interpolate_4th_order_util->source;
  source_stride = fluid_interpolate_4th_order_util->source_stride;

  buffer_length = fluid_interpolate_4th_order_util->buffer_length;

  phase_incr = fluid_interpolate_4th_order_util->phase_increment;

  ags_fluid_interpolate_4th_order_util_config();
  
  samplerate = fluid_interpolate_4th_order_util->samplerate;

  base_key = fluid_interpolate_4th_order_util->base_key;
  tuning = fluid_interpolate_4th_order_util->tuning;

  vibrato_gain = fluid_interpolate_4th_order_util->vibrato_gain;
  vibrato_lfo_depth = fluid_interpolate_4th_order_util->vibrato_lfo_depth;
  vibrato_lfo_freq = fluid_interpolate_4th_order_util->vibrato_lfo_freq;
  vibrato_tuning = fluid_interpolate_4th_order_util->vibrato_tuning;
  vibrato_lfo_offset = fluid_interpolate_4th_order_util->vibrato_lfo_offset;

  if(fluid_interpolate_4th_order_util->vibrato_enabled == FALSE){
    vibrato_gain = 0.0;
  }

  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index * source_stride];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  root_pitch_hz = exp2(((double) fluid_interpolate_4th_order_util->base_key - 48.0) / 12.0) * 440.0;

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gdouble phase_incr;

    gint row;
        
    phase_incr = (exp2((((double) base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((vibrato_lfo_offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    destination[dsp_i * destination_stride] = (coeffs_0 * start_point
					       + coeffs_1 * source[dsp_phase_index * source_stride]
					       + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
					       + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    vibrato_lfo_offset += 1;
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gdouble phase_incr;

    gint row;
    
    phase_incr = (exp2((((double) base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((vibrato_lfo_offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    destination[dsp_i * destination_stride] = (coeffs_0 * source[(dsp_phase_index - 1) * source_stride]
					       + coeffs_1 * source[dsp_phase_index * source_stride]
					       + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
					       + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    vibrato_lfo_offset += 1;
  }

  fluid_interpolate_4th_order_util->vibrato_lfo_offset = vibrato_lfo_offset;
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
  gint32 *destination, *source;
  
  guint samplerate;
  guint destination_stride, source_stride;
  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint vibrato_lfo_offset;
  guint buffer_length;
  gdouble phase_incr;
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(fluid_interpolate_4th_order_util == NULL ||
     fluid_interpolate_4th_order_util->destination == NULL ||
     fluid_interpolate_4th_order_util->source == NULL){
    return;
  }

  destination = fluid_interpolate_4th_order_util->destination;
  destination_stride = fluid_interpolate_4th_order_util->destination_stride;

  source = fluid_interpolate_4th_order_util->source;
  source_stride = fluid_interpolate_4th_order_util->source_stride;

  buffer_length = fluid_interpolate_4th_order_util->buffer_length;

  phase_incr = fluid_interpolate_4th_order_util->phase_increment;

  ags_fluid_interpolate_4th_order_util_config();
  
  samplerate = fluid_interpolate_4th_order_util->samplerate;

  base_key = fluid_interpolate_4th_order_util->base_key;
  tuning = fluid_interpolate_4th_order_util->tuning;

  vibrato_gain = fluid_interpolate_4th_order_util->vibrato_gain;
  vibrato_lfo_depth = fluid_interpolate_4th_order_util->vibrato_lfo_depth;
  vibrato_lfo_freq = fluid_interpolate_4th_order_util->vibrato_lfo_freq;
  vibrato_tuning = fluid_interpolate_4th_order_util->vibrato_tuning;
  vibrato_lfo_offset = fluid_interpolate_4th_order_util->vibrato_lfo_offset;

  if(fluid_interpolate_4th_order_util->vibrato_enabled == FALSE){
    vibrato_gain = 0.0;
  }

  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index * source_stride];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  root_pitch_hz = exp2(((double) fluid_interpolate_4th_order_util->base_key - 48.0) / 12.0) * 440.0;

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gdouble phase_incr;

    gint row;
        
    phase_incr = (exp2((((double) base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((vibrato_lfo_offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    destination[dsp_i * destination_stride] = (coeffs_0 * start_point
					       + coeffs_1 * source[dsp_phase_index * source_stride]
					       + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
					       + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    vibrato_lfo_offset += 1;
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gdouble phase_incr;

    gint row;
    
    phase_incr = (exp2((((double) base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((vibrato_lfo_offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    destination[dsp_i * destination_stride] = (coeffs_0 * source[(dsp_phase_index - 1) * source_stride]
					       + coeffs_1 * source[dsp_phase_index * source_stride]
					       + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
					       + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    vibrato_lfo_offset += 1;
  }

  fluid_interpolate_4th_order_util->vibrato_lfo_offset = vibrato_lfo_offset;
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
  gint64 *destination, *source;
  
  guint samplerate;
  guint destination_stride, source_stride;
  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint vibrato_lfo_offset;
  guint buffer_length;
  gdouble phase_incr;
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(fluid_interpolate_4th_order_util == NULL ||
     fluid_interpolate_4th_order_util->destination == NULL ||
     fluid_interpolate_4th_order_util->source == NULL){
    return;
  }

  destination = fluid_interpolate_4th_order_util->destination;
  destination_stride = fluid_interpolate_4th_order_util->destination_stride;

  source = fluid_interpolate_4th_order_util->source;
  source_stride = fluid_interpolate_4th_order_util->source_stride;

  buffer_length = fluid_interpolate_4th_order_util->buffer_length;

  phase_incr = fluid_interpolate_4th_order_util->phase_increment;

  ags_fluid_interpolate_4th_order_util_config();
  
  samplerate = fluid_interpolate_4th_order_util->samplerate;

  base_key = fluid_interpolate_4th_order_util->base_key;
  tuning = fluid_interpolate_4th_order_util->tuning;

  vibrato_gain = fluid_interpolate_4th_order_util->vibrato_gain;
  vibrato_lfo_depth = fluid_interpolate_4th_order_util->vibrato_lfo_depth;
  vibrato_lfo_freq = fluid_interpolate_4th_order_util->vibrato_lfo_freq;
  vibrato_tuning = fluid_interpolate_4th_order_util->vibrato_tuning;
  vibrato_lfo_offset = fluid_interpolate_4th_order_util->vibrato_lfo_offset;

  if(fluid_interpolate_4th_order_util->vibrato_enabled == FALSE){
    vibrato_gain = 0.0;
  }

  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index * source_stride];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  root_pitch_hz = exp2(((double) fluid_interpolate_4th_order_util->base_key - 48.0) / 12.0) * 440.0;

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gdouble phase_incr;

    gint row;
        
    phase_incr = (exp2((((double) base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((vibrato_lfo_offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    destination[dsp_i * destination_stride] = (coeffs_0 * start_point
					       + coeffs_1 * source[dsp_phase_index * source_stride]
					       + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
					       + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    vibrato_lfo_offset += 1;
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gdouble phase_incr;

    gint row;
    
    phase_incr = (exp2((((double) base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((vibrato_lfo_offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    destination[dsp_i * destination_stride] = (coeffs_0 * source[(dsp_phase_index - 1) * source_stride]
					       + coeffs_1 * source[dsp_phase_index * source_stride]
					       + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
					       + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    vibrato_lfo_offset += 1;
  }

  fluid_interpolate_4th_order_util->vibrato_lfo_offset = vibrato_lfo_offset;
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
  gfloat *destination, *source;
  
  guint samplerate;
  guint destination_stride, source_stride;
  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint vibrato_lfo_offset;
  guint buffer_length;
  gdouble phase_incr;
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(fluid_interpolate_4th_order_util == NULL ||
     fluid_interpolate_4th_order_util->destination == NULL ||
     fluid_interpolate_4th_order_util->source == NULL){
    return;
  }

  destination = fluid_interpolate_4th_order_util->destination;
  destination_stride = fluid_interpolate_4th_order_util->destination_stride;

  source = fluid_interpolate_4th_order_util->source;
  source_stride = fluid_interpolate_4th_order_util->source_stride;

  buffer_length = fluid_interpolate_4th_order_util->buffer_length;

  phase_incr = fluid_interpolate_4th_order_util->phase_increment;

  ags_fluid_interpolate_4th_order_util_config();
  
  samplerate = fluid_interpolate_4th_order_util->samplerate;

  base_key = fluid_interpolate_4th_order_util->base_key;
  tuning = fluid_interpolate_4th_order_util->tuning;

  vibrato_gain = fluid_interpolate_4th_order_util->vibrato_gain;
  vibrato_lfo_depth = fluid_interpolate_4th_order_util->vibrato_lfo_depth;
  vibrato_lfo_freq = fluid_interpolate_4th_order_util->vibrato_lfo_freq;
  vibrato_tuning = fluid_interpolate_4th_order_util->vibrato_tuning;
  vibrato_lfo_offset = fluid_interpolate_4th_order_util->vibrato_lfo_offset;

  if(fluid_interpolate_4th_order_util->vibrato_enabled == FALSE){
    vibrato_gain = 0.0;
  }

  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index * source_stride];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  root_pitch_hz = exp2(((double) fluid_interpolate_4th_order_util->base_key - 48.0) / 12.0) * 440.0;

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gdouble phase_incr;

    gint row;
        
    phase_incr = (exp2((((double) base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((vibrato_lfo_offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    destination[dsp_i * destination_stride] = (coeffs_0 * start_point
					       + coeffs_1 * source[dsp_phase_index * source_stride]
					       + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
					       + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    vibrato_lfo_offset += 1;
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gdouble phase_incr;

    gint row;
    
    phase_incr = (exp2((((double) base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((vibrato_lfo_offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    destination[dsp_i * destination_stride] = (coeffs_0 * source[(dsp_phase_index - 1) * source_stride]
					       + coeffs_1 * source[dsp_phase_index * source_stride]
					       + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
					       + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    vibrato_lfo_offset += 1;
  }

  fluid_interpolate_4th_order_util->vibrato_lfo_offset = vibrato_lfo_offset;
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
  gdouble *destination, *source;
  
  guint samplerate;
  guint destination_stride, source_stride;
  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint vibrato_lfo_offset;
  guint buffer_length;
  gdouble phase_incr;
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(fluid_interpolate_4th_order_util == NULL ||
     fluid_interpolate_4th_order_util->destination == NULL ||
     fluid_interpolate_4th_order_util->source == NULL){
    return;
  }

  destination = fluid_interpolate_4th_order_util->destination;
  destination_stride = fluid_interpolate_4th_order_util->destination_stride;

  source = fluid_interpolate_4th_order_util->source;
  source_stride = fluid_interpolate_4th_order_util->source_stride;

  buffer_length = fluid_interpolate_4th_order_util->buffer_length;

  phase_incr = fluid_interpolate_4th_order_util->phase_increment;

  ags_fluid_interpolate_4th_order_util_config();
  
  samplerate = fluid_interpolate_4th_order_util->samplerate;

  base_key = fluid_interpolate_4th_order_util->base_key;
  tuning = fluid_interpolate_4th_order_util->tuning;

  vibrato_gain = fluid_interpolate_4th_order_util->vibrato_gain;
  vibrato_lfo_depth = fluid_interpolate_4th_order_util->vibrato_lfo_depth;
  vibrato_lfo_freq = fluid_interpolate_4th_order_util->vibrato_lfo_freq;
  vibrato_tuning = fluid_interpolate_4th_order_util->vibrato_tuning;
  vibrato_lfo_offset = fluid_interpolate_4th_order_util->vibrato_lfo_offset;

  if(fluid_interpolate_4th_order_util->vibrato_enabled == FALSE){
    vibrato_gain = 0.0;
  }

  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index * source_stride];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  root_pitch_hz = exp2(((double) fluid_interpolate_4th_order_util->base_key - 48.0) / 12.0) * 440.0;

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gdouble phase_incr;

    gint row;
        
    phase_incr = (exp2((((double) base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((vibrato_lfo_offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    destination[dsp_i * destination_stride] = (coeffs_0 * start_point
					       + coeffs_1 * source[dsp_phase_index * source_stride]
					       + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
					       + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    vibrato_lfo_offset += 1;
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gdouble phase_incr;

    gint row;
    
    phase_incr = (exp2((((double) base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((vibrato_lfo_offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    destination[dsp_i * destination_stride] = (coeffs_0 * source[(dsp_phase_index - 1) * source_stride]
					       + coeffs_1 * source[dsp_phase_index * source_stride]
					       + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
					       + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    vibrato_lfo_offset += 1;
  }

  fluid_interpolate_4th_order_util->vibrato_lfo_offset = vibrato_lfo_offset;
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
  AgsComplex *destination, *source;
  
  guint samplerate;
  guint destination_stride, source_stride;
  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint vibrato_lfo_offset;
  guint buffer_length;
  gdouble phase_incr;
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  double _Complex start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  if(fluid_interpolate_4th_order_util == NULL ||
     fluid_interpolate_4th_order_util->destination == NULL ||
     fluid_interpolate_4th_order_util->source == NULL){
    return;
  }

  destination = fluid_interpolate_4th_order_util->destination;
  destination_stride = fluid_interpolate_4th_order_util->destination_stride;

  source = fluid_interpolate_4th_order_util->source;
  source_stride = fluid_interpolate_4th_order_util->source_stride;

  buffer_length = fluid_interpolate_4th_order_util->buffer_length;

  phase_incr = fluid_interpolate_4th_order_util->phase_increment;

  ags_fluid_interpolate_4th_order_util_config();
  
  samplerate = fluid_interpolate_4th_order_util->samplerate;

  base_key = fluid_interpolate_4th_order_util->base_key;
  tuning = fluid_interpolate_4th_order_util->tuning;

  vibrato_gain = fluid_interpolate_4th_order_util->vibrato_gain;
  vibrato_lfo_depth = fluid_interpolate_4th_order_util->vibrato_lfo_depth;
  vibrato_lfo_freq = fluid_interpolate_4th_order_util->vibrato_lfo_freq;
  vibrato_tuning = fluid_interpolate_4th_order_util->vibrato_tuning;
  vibrato_lfo_offset = fluid_interpolate_4th_order_util->vibrato_lfo_offset;

  if(fluid_interpolate_4th_order_util->vibrato_enabled == FALSE){
    vibrato_gain = 0.0;
  }

  dsp_phase = 0;

  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = ags_complex_get(source);

  end_point1 = ags_complex_get(source + (end_index * source_stride));
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  root_pitch_hz = exp2(((double) fluid_interpolate_4th_order_util->base_key - 48.0) / 12.0) * 440.0;

  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gdouble phase_incr;

    gint row;
        
    phase_incr = (exp2((((double) base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((vibrato_lfo_offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    ags_complex_set(destination + (dsp_i * destination_stride),
		    (coeffs_0 * start_point
		     + coeffs_1 * ags_complex_get(source + (dsp_phase_index * source_stride))
		     + coeffs_2 * ags_complex_get(source + ((dsp_phase_index + 1) * source_stride))
		     + coeffs_3 * ags_complex_get(source + ((dsp_phase_index + 2) * source_stride))));

    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    vibrato_lfo_offset += 1;
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gdouble phase_incr;

    gint row;
    
    phase_incr = (exp2((((double) base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((vibrato_lfo_offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    ags_complex_set(destination + (dsp_i * destination_stride),
		    (coeffs_0 * ags_complex_get(source + ((dsp_phase_index - 1) * source_stride))
		     + coeffs_1 * ags_complex_get(source + (dsp_phase_index * source_stride))
		     + coeffs_2 * ags_complex_get(source + ((dsp_phase_index + 1) * source_stride))
		     + coeffs_3 * ags_complex_get(source + ((dsp_phase_index + 2) * source_stride))));
		    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    vibrato_lfo_offset += 1;
  }

  fluid_interpolate_4th_order_util->vibrato_lfo_offset = vibrato_lfo_offset;
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
  if(fluid_interpolate_4th_order_util == NULL ||
     fluid_interpolate_4th_order_util->destination == NULL ||
     fluid_interpolate_4th_order_util->source == NULL){
    return;
  }

  switch(fluid_interpolate_4th_order_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_fluid_interpolate_4th_order_util_pitch_s8(fluid_interpolate_4th_order_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_fluid_interpolate_4th_order_util_pitch_s16(fluid_interpolate_4th_order_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_fluid_interpolate_4th_order_util_pitch_s24(fluid_interpolate_4th_order_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_fluid_interpolate_4th_order_util_pitch_s32(fluid_interpolate_4th_order_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_fluid_interpolate_4th_order_util_pitch_s64(fluid_interpolate_4th_order_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_fluid_interpolate_4th_order_util_pitch_float(fluid_interpolate_4th_order_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_fluid_interpolate_4th_order_util_pitch_double(fluid_interpolate_4th_order_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_fluid_interpolate_4th_order_util_pitch_complex(fluid_interpolate_4th_order_util);
  }
  break;
  default:
    g_warning("unknown format");
  }
}
