/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/audio/ags_fluid_iir_filter_util.h>

#include <ags/audio/ags_fluid_util.h>

#include <math.h>

/**
 * SECTION:ags_fluid_iir_filter_util
 * @short_description: util functions to fluid IIR filter
 * @title: AgsFluidIIRFilterUtil
 * @section_id:
 * @include: ags/audio/ags_fluid_iir_filter_util.h
 *
 * These utility functions allow you to calc and apply fluid
 * IIR filter data.
 */

GType
ags_fluid_iir_filter_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fluid_iir_filter_util = 0;

    ags_type_fluid_iir_filter_util =
      g_boxed_type_register_static("AgsFluidIIRFilterUtil",
				   (GBoxedCopyFunc) ags_fluid_iir_filter_util_copy,
				   (GBoxedFreeFunc) ags_fluid_iir_filter_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fluid_iir_filter_util);
  }

  return g_define_type_id__volatile;
}

GType
ags_fluid_iir_filter_type_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_FLUID_IIR_DISABLED, "AGS_FLUID_IIR_DISABLED", "fluid-iir-disabled" },
      { AGS_FLUID_IIR_LOWPASS, "AGS_FLUID_IIR_LOWPASS", "fluid-iir-lowpass" },
      { AGS_FLUID_IIR_HIGHPASS, "AGS_FLUID_IIR_HIGHPASS", "fluid-iir-highpass" },
      { AGS_FLUID_IIR_LAST, "AGS_FLUID_IIR_LAST", "fluid-iir-last" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsFluidIIRFilterType"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

/**
 * ags_fluid_iir_filter_util_alloc:
 * 
 * Allocate #AgsFluidIIRFilterUtil-struct.
 * 
 * Returns: the newly allocated #AgsFluidIIRFilterUtil-struct
 * 
 * Since: 3.9.6
 */
AgsFluidIIRFilterUtil*
ags_fluid_iir_filter_util_alloc()
{
  AgsFluidIIRFilterUtil *ptr;
  
  ptr = (AgsFluidIIRFilterUtil *) g_new(AgsFluidIIRFilterUtil,
					1);

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->destination = NULL;
  ptr->destination_stride = 1;

  ptr->buffer_length = 0;
  ptr->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  ptr->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  ptr->filter_type = 0;

  ptr->flags = 0;
  
  ptr->b02 = 0.0;
  ptr->b1 = 0.0;
  ptr->a1 = 0.0;
  ptr->a2 = 0.0;
  ptr->b02_incr = 0.0;
  ptr->b1_incr = 0.0;
  ptr->a1_incr = 0.0;
  ptr->a2_incr = 0.0;

  ptr->filter_coeff_incr_count = 0;
  ptr->compensate_incr = 0;

  ptr->hist1 = 0.0;
  ptr->hist2 = 0.0;

  ptr->filter_startup = TRUE;

  ptr->fres = 0.0;
  ptr->last_fres = 0.0;
  
  ptr->q_lin = 0.0;
  ptr->filter_gain = 1.0;
  
  return(ptr);
}

/**
 * ags_fluid_iir_filter_util_copy:
 * @ptr: the original #AgsFluidIIRFilterUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsFluidIIRFilterUtil-struct
 *
 * Since: 3.9.6
 */
gpointer
ags_fluid_iir_filter_util_copy(AgsFluidIIRFilterUtil *ptr)
{
  AgsFluidIIRFilterUtil *new_ptr;
  
  new_ptr = (AgsFluidIIRFilterUtil *) g_new(AgsFluidIIRFilterUtil,
					    1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->filter_type = ptr->filter_type;

  new_ptr->flags = ptr->flags;
  
  new_ptr->b02 = ptr->b02;
  new_ptr->b1 = ptr->b1;
  new_ptr->a1 = ptr->a1;
  new_ptr->a2 = ptr->a2;
  new_ptr->b02_incr = ptr->b02_incr;
  new_ptr->b1_incr = ptr->b1_incr;
  new_ptr->a1_incr = ptr->a1_incr;
  new_ptr->a2_incr = ptr->a2_incr;

  new_ptr->filter_coeff_incr_count = ptr->filter_coeff_incr_count;
  new_ptr->compensate_incr = ptr->compensate_incr;

  new_ptr->hist1 = ptr->hist1;
  new_ptr->hist2 = ptr->hist2;

  new_ptr->filter_startup = ptr->filter_startup;

  new_ptr->fres = ptr->fres;
  new_ptr->last_fres = ptr->last_fres;
  
  new_ptr->q_lin = ptr->q_lin;
  new_ptr->filter_gain = ptr->filter_gain;
  
  return(new_ptr);
}

/**
 * ags_fluid_iir_filter_util_free:
 * @ptr: the #AgsFluidIIRFilterUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.9.6
 */
void
ags_fluid_iir_filter_util_free(AgsFluidIIRFilterUtil *ptr)
{
  g_free(ptr->destination);

  if(ptr->destination != ptr->source){
    g_free(ptr->source);
  }
  
  g_free(ptr);
}

/**
 * ags_fluid_iir_filter_util_get_destination:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Get destination buffer of @fluid_iir_filter_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 3.9.6
 */
gpointer
ags_fluid_iir_filter_util_get_destination(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  if(fluid_iir_filter_util == NULL){
    return(NULL);
  }

  return(fluid_iir_filter_util->destination);
}

/**
 * ags_fluid_iir_filter_util_set_destination:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * @destination: the destination buffer
 *
 * Set @destination buffer of @fluid_iir_filter_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_iir_filter_util_set_destination(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
					  gpointer destination)
{
  if(fluid_iir_filter_util == NULL){
    return;
  }

  fluid_iir_filter_util->destination = destination;
}

/**
 * ags_fluid_iir_filter_util_get_destination_stride:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Get destination stride of @fluid_iir_filter_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 3.9.6
 */
guint
ags_fluid_iir_filter_util_get_destination_stride(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  if(fluid_iir_filter_util == NULL){
    return(0);
  }

  return(fluid_iir_filter_util->destination_stride);
}

/**
 * ags_fluid_iir_filter_util_set_destination_stride:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @fluid_iir_filter_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_iir_filter_util_set_destination_stride(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
						 guint destination_stride)
{
  if(fluid_iir_filter_util == NULL){
    return;
  }

  fluid_iir_filter_util->destination_stride = destination_stride;
}

/**
 * ags_fluid_iir_filter_util_get_source:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Get source buffer of @fluid_iir_filter_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.9.6
 */
gpointer
ags_fluid_iir_filter_util_get_source(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  if(fluid_iir_filter_util == NULL){
    return(NULL);
  }

  return(fluid_iir_filter_util->source);
}

/**
 * ags_fluid_iir_filter_util_set_source:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @fluid_iir_filter_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_iir_filter_util_set_source(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
				     gpointer source)
{
  if(fluid_iir_filter_util == NULL){
    return;
  }

  fluid_iir_filter_util->source = source;
}

/**
 * ags_fluid_iir_filter_util_get_source_stride:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Get source stride of @fluid_iir_filter_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.9.6
 */
guint
ags_fluid_iir_filter_util_get_source_stride(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  if(fluid_iir_filter_util == NULL){
    return(0);
  }

  return(fluid_iir_filter_util->source_stride);
}

/**
 * ags_fluid_iir_filter_util_set_source_stride:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @fluid_iir_filter_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_iir_filter_util_set_source_stride(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
					    guint source_stride)
{
  if(fluid_iir_filter_util == NULL){
    return;
  }

  fluid_iir_filter_util->source_stride = source_stride;
}

/**
 * ags_fluid_iir_filter_util_get_buffer_length:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Get buffer length of @fluid_iir_filter_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.9.6
 */
guint
ags_fluid_iir_filter_util_get_buffer_length(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  if(fluid_iir_filter_util == NULL){
    return(0);
  }

  return(fluid_iir_filter_util->buffer_length);
}

/**
 * ags_fluid_iir_filter_util_set_buffer_length:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @fluid_iir_filter_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_iir_filter_util_set_buffer_length(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
					    guint buffer_length)
{
  if(fluid_iir_filter_util == NULL){
    return;
  }

  fluid_iir_filter_util->buffer_length = buffer_length;
}

/**
 * ags_fluid_iir_filter_util_get_format:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Get format of @fluid_iir_filter_util.
 * 
 * Returns: the format
 * 
 * Since: 3.9.6
 */
AgsSoundcardFormat
ags_fluid_iir_filter_util_get_format(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  if(fluid_iir_filter_util == NULL){
    return(0);
  }

  return(fluid_iir_filter_util->format);
}

/**
 * ags_fluid_iir_filter_util_set_format:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * @format: the format
 *
 * Set @format of @fluid_iir_filter_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_iir_filter_util_set_format(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
				     AgsSoundcardFormat format)
{
  if(fluid_iir_filter_util == NULL){
    return;
  }

  fluid_iir_filter_util->format = format;
}

/**
 * ags_fluid_iir_filter_util_get_samplerate:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Get samplerate of @fluid_iir_filter_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.9.6
 */
guint
ags_fluid_iir_filter_util_get_samplerate(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  if(fluid_iir_filter_util == NULL){
    return(0);
  }

  return(fluid_iir_filter_util->samplerate);
}

/**
 * ags_fluid_iir_filter_util_set_samplerate:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @fluid_iir_filter_util.
 *
 * Since: 3.9.6
 */
void
ags_fluid_iir_filter_util_set_samplerate(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
					 guint samplerate)
{
  if(fluid_iir_filter_util == NULL){
    return;
  }

  fluid_iir_filter_util->samplerate = samplerate;
}

/**
 * ags_fluid_iir_filter_util_get_filter_type:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Get filter type of @fluid_iir_filter_util.
 * 
 * Returns: the filter type
 * 
 * Since: 3.9.7
 */
AgsFluidIIRFilterType
ags_fluid_iir_filter_util_get_filter_type(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  if(fluid_iir_filter_util == NULL){
    return(0);
  }

  return(fluid_iir_filter_util->filter_type);
}

/**
 * ags_fluid_iir_filter_util_set_filter_type:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * @filter_type: the filter type
 *
 * Set @filter_type of @fluid_iir_filter_util.
 *
 * Since: 3.9.7
 */
void
ags_fluid_iir_filter_util_set_filter_type(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
					  AgsFluidIIRFilterType filter_type)
{
  if(fluid_iir_filter_util == NULL){
    return;
  }

  fluid_iir_filter_util->filter_type = filter_type;
}

/**
 * ags_fluid_iir_filter_util_get_flags:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Get flags of @fluid_iir_filter_util.
 * 
 * Returns: the flags
 * 
 * Since: 3.9.7
 */
guint
ags_fluid_iir_filter_util_get_flags(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  if(fluid_iir_filter_util == NULL){
    return(0);
  }

  return(fluid_iir_filter_util->flags);
}

/**
 * ags_fluid_iir_filter_util_set_flags:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * @flags: the flags
 *
 * Set @flags of @fluid_iir_filter_util.
 *
 * Since: 3.9.7
 */
void
ags_fluid_iir_filter_util_set_flags(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
				    guint flags)
{
  if(fluid_iir_filter_util == NULL){
    return;
  }

  fluid_iir_filter_util->flags = flags;
}

/**
 * ags_fluid_iir_filter_util_get_filter_startup:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Get filter startup of @fluid_iir_filter_util.
 * 
 * Returns: %TRUE if the filter does startup, otherwise %FALSE
 * 
 * Since: 3.9.7
 */
gboolean
ags_fluid_iir_filter_util_get_filter_startup(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  if(fluid_iir_filter_util == NULL){
    return(FALSE);
  }

  return(fluid_iir_filter_util->filter_startup);
}

/**
 * ags_fluid_iir_filter_util_set_filter_startup:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * @filter_startup: the filter startup
 *
 * Set @filter_startup of @fluid_iir_filter_util.
 *
 * Since: 3.9.7
 */
void
ags_fluid_iir_filter_util_set_filter_startup(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
					     gboolean filter_startup)
{
  if(fluid_iir_filter_util == NULL){
    return;
  }

  fluid_iir_filter_util->filter_startup = filter_startup;
}

/**
 * ags_fluid_iir_filter_util_get_q_lin:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Get q-lin of @fluid_iir_filter_util.
 * 
 * Returns: the q-lin
 * 
 * Since: 3.9.7
 */
gdouble
ags_fluid_iir_filter_util_get_q_lin(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  if(fluid_iir_filter_util == NULL){
    return(0.0);
  }

  return(fluid_iir_filter_util->q_lin);
}

/**
 * ags_fluid_iir_filter_util_set_q_lin:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * @q_lin: the q-lin
 *
 * Set @q_lin of @fluid_iir_filter_util.
 *
 * Since: 3.9.7
 */
void
ags_fluid_iir_filter_util_set_q_lin(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
				    gdouble q_lin)
{
  if(fluid_iir_filter_util == NULL){
    return;
  }

  fluid_iir_filter_util->q_lin = q_lin;
}

/**
 * ags_fluid_iir_filter_util_get_filter_gain:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Get filter gain of @fluid_iir_filter_util.
 * 
 * Returns: the filter gain
 * 
 * Since: 3.9.7
 */
gdouble
ags_fluid_iir_filter_util_get_filter_gain(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  if(fluid_iir_filter_util == NULL){
    return(1.0);
  }

  return(fluid_iir_filter_util->filter_gain);
}

/**
 * ags_fluid_iir_filter_util_set_filter_gain:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * @filter_gain: the filter gain
 *
 * Set @filter_gain of @fluid_iir_filter_util.
 *
 * Since: 3.9.7
 */
void
ags_fluid_iir_filter_util_set_filter_gain(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
					  gdouble filter_gain)
{
  if(fluid_iir_filter_util == NULL){
    return;
  }

  fluid_iir_filter_util->filter_gain = filter_gain;
}

/**
 * ags_fluid_iir_filter_util_process_s8:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Process @fluid_iir_filter_util of signed 8 bit data.
 * 
 * Since: 3.9.7
 */
void
ags_fluid_iir_filter_util_process_s8(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  gint8 *destination, *source;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  
  if(fluid_iir_filter_util == NULL ||
     fluid_iir_filter_util->destination == NULL ||
     fluid_iir_filter_util->source == NULL){
    return;
  }

  destination = fluid_iir_filter_util->destination;
  destination_stride = fluid_iir_filter_util->destination_stride;

  source = fluid_iir_filter_util->source;
  source_stride = fluid_iir_filter_util->source_stride;

  buffer_length = fluid_iir_filter_util->buffer_length;

  if(fluid_iir_filter_util->filter_type == AGS_FLUID_IIR_DISABLED || fluid_iir_filter_util->q_lin == 0.0){
    return;
  }else{
    /* IIR filter sample history */
    gdouble dsp_hist1;
    gdouble dsp_hist2;
    
    /* IIR filter coefficients */
    gdouble dsp_a1;
    gdouble dsp_a2;
    gdouble dsp_b02;
    gdouble dsp_b1;
    gint dsp_filter_coeff_incr_count;
      
    gdouble dsp_centernode;
    gint dsp_i;
      
    /* filter (implement the voice filter according to SoundFont standard) */
    dsp_hist1 = fluid_iir_filter_util->hist1;
    dsp_hist2 = fluid_iir_filter_util->hist2;
    
    dsp_a1 = fluid_iir_filter_util->a1;
    dsp_a2 = fluid_iir_filter_util->a2;
    dsp_b02 = fluid_iir_filter_util->b02;
    dsp_b1 = fluid_iir_filter_util->b1;

    dsp_filter_coeff_incr_count = fluid_iir_filter_util->filter_coeff_incr_count;
    
    /* Two versions of the filter loop. One, while the filter is
     * changing towards its new setting. The other, if the filter
     * doesn't change.
     */

    if(dsp_filter_coeff_incr_count > 0){
      gdouble dsp_a1_incr;
      gdouble dsp_a2_incr;
      gdouble dsp_b02_incr;
      gdouble dsp_b1_incr;
      
      dsp_a1_incr = fluid_iir_filter_util->a1_incr;
      dsp_a2_incr = fluid_iir_filter_util->a2_incr;
      dsp_b02_incr = fluid_iir_filter_util->b02_incr;
      dsp_b1_incr = fluid_iir_filter_util->b1_incr;

      /* Increment is added to each filter coefficient filter_coeff_incr_count times. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i * source_stride] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;

	destination[dsp_i * destination_stride] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;

	if(dsp_filter_coeff_incr_count-- > 0){
	  gdouble old_b02;

	  old_b02 = dsp_b02;

	  dsp_a1 += dsp_a1_incr;
	  dsp_a2 += dsp_a2_incr;
	  dsp_b02 += dsp_b02_incr;
	  dsp_b1 += dsp_b1_incr;

	  /* Compensate history to avoid the filter going havoc with large frequency changes */
	  if(fluid_iir_filter_util->compensate_incr && fabs(dsp_b02) > 0.001f){
	    gdouble compensate;

	    compensate = old_b02 / dsp_b02;

	    dsp_hist1 *= compensate;
	    dsp_hist2 *= compensate;
	  }
	}
      } /* for dsp_i */
    }else{ /* The filter parameters are constant.  This is duplicated to save time. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i * source_stride] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;
	
	destination[dsp_i * destination_stride] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;
      }
    }
    
    fluid_iir_filter_util->hist1 = dsp_hist1;
    fluid_iir_filter_util->hist2 = dsp_hist2;
    
    fluid_iir_filter_util->a1 = dsp_a1;
    fluid_iir_filter_util->a2 = dsp_a2;
    fluid_iir_filter_util->b02 = dsp_b02;
    fluid_iir_filter_util->b1 = dsp_b1;
    
    fluid_iir_filter_util->filter_coeff_incr_count = dsp_filter_coeff_incr_count;
  }
}

/**
 * ags_fluid_iir_filter_util_process_s16:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Process @fluid_iir_filter_util of signed 16 bit data.
 * 
 * Since: 3.9.7
 */
void
ags_fluid_iir_filter_util_process_s16(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  gint16 *destination, *source;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  
  if(fluid_iir_filter_util == NULL ||
     fluid_iir_filter_util->destination == NULL ||
     fluid_iir_filter_util->source == NULL){
    return;
  }

  destination = fluid_iir_filter_util->destination;
  destination_stride = fluid_iir_filter_util->destination_stride;

  source = fluid_iir_filter_util->source;
  source_stride = fluid_iir_filter_util->source_stride;

  buffer_length = fluid_iir_filter_util->buffer_length;

  if(fluid_iir_filter_util->filter_type == AGS_FLUID_IIR_DISABLED || fluid_iir_filter_util->q_lin == 0.0){
    return;
  }else{
    /* IIR filter sample history */
    gdouble dsp_hist1;
    gdouble dsp_hist2;
    
    /* IIR filter coefficients */
    gdouble dsp_a1;
    gdouble dsp_a2;
    gdouble dsp_b02;
    gdouble dsp_b1;
    gint dsp_filter_coeff_incr_count;
      
    gdouble dsp_centernode;
    gint dsp_i;
      
    /* filter (implement the voice filter according to SoundFont standard) */
    dsp_hist1 = fluid_iir_filter_util->hist1;
    dsp_hist2 = fluid_iir_filter_util->hist2;
    
    dsp_a1 = fluid_iir_filter_util->a1;
    dsp_a2 = fluid_iir_filter_util->a2;
    dsp_b02 = fluid_iir_filter_util->b02;
    dsp_b1 = fluid_iir_filter_util->b1;

    dsp_filter_coeff_incr_count = fluid_iir_filter_util->filter_coeff_incr_count;
    
    /* Two versions of the filter loop. One, while the filter is
     * changing towards its new setting. The other, if the filter
     * doesn't change.
     */

    if(dsp_filter_coeff_incr_count > 0){
      gdouble dsp_a1_incr;
      gdouble dsp_a2_incr;
      gdouble dsp_b02_incr;
      gdouble dsp_b1_incr;
      
      dsp_a1_incr = fluid_iir_filter_util->a1_incr;
      dsp_a2_incr = fluid_iir_filter_util->a2_incr;
      dsp_b02_incr = fluid_iir_filter_util->b02_incr;
      dsp_b1_incr = fluid_iir_filter_util->b1_incr;

      /* Increment is added to each filter coefficient filter_coeff_incr_count times. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i * source_stride] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;

	destination[dsp_i * destination_stride] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;

	if(dsp_filter_coeff_incr_count-- > 0){
	  gdouble old_b02;

	  old_b02 = dsp_b02;

	  dsp_a1 += dsp_a1_incr;
	  dsp_a2 += dsp_a2_incr;
	  dsp_b02 += dsp_b02_incr;
	  dsp_b1 += dsp_b1_incr;

	  /* Compensate history to avoid the filter going havoc with large frequency changes */
	  if(fluid_iir_filter_util->compensate_incr && fabs(dsp_b02) > 0.001f){
	    gdouble compensate;

	    compensate = old_b02 / dsp_b02;

	    dsp_hist1 *= compensate;
	    dsp_hist2 *= compensate;
	  }
	}
      } /* for dsp_i */
    }else{ /* The filter parameters are constant.  This is duplicated to save time. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i * source_stride] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;
	
	destination[dsp_i * destination_stride] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;
      }
    }
    
    fluid_iir_filter_util->hist1 = dsp_hist1;
    fluid_iir_filter_util->hist2 = dsp_hist2;
    
    fluid_iir_filter_util->a1 = dsp_a1;
    fluid_iir_filter_util->a2 = dsp_a2;
    fluid_iir_filter_util->b02 = dsp_b02;
    fluid_iir_filter_util->b1 = dsp_b1;
    
    fluid_iir_filter_util->filter_coeff_incr_count = dsp_filter_coeff_incr_count;
  }
}

/**
 * ags_fluid_iir_filter_util_process_s24:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Process @fluid_iir_filter_util of signed 24 bit data.
 * 
 * Since: 3.9.7
 */
void
ags_fluid_iir_filter_util_process_s24(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  gint32 *destination, *source;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  
  if(fluid_iir_filter_util == NULL ||
     fluid_iir_filter_util->destination == NULL ||
     fluid_iir_filter_util->source == NULL){
    return;
  }

  destination = fluid_iir_filter_util->destination;
  destination_stride = fluid_iir_filter_util->destination_stride;

  source = fluid_iir_filter_util->source;
  source_stride = fluid_iir_filter_util->source_stride;

  buffer_length = fluid_iir_filter_util->buffer_length;

  if(fluid_iir_filter_util->filter_type == AGS_FLUID_IIR_DISABLED || fluid_iir_filter_util->q_lin == 0.0){
    return;
  }else{
    /* IIR filter sample history */
    gdouble dsp_hist1;
    gdouble dsp_hist2;
    
    /* IIR filter coefficients */
    gdouble dsp_a1;
    gdouble dsp_a2;
    gdouble dsp_b02;
    gdouble dsp_b1;
    gint dsp_filter_coeff_incr_count;
      
    gdouble dsp_centernode;
    gint dsp_i;
      
    /* filter (implement the voice filter according to SoundFont standard) */
    dsp_hist1 = fluid_iir_filter_util->hist1;
    dsp_hist2 = fluid_iir_filter_util->hist2;
    
    dsp_a1 = fluid_iir_filter_util->a1;
    dsp_a2 = fluid_iir_filter_util->a2;
    dsp_b02 = fluid_iir_filter_util->b02;
    dsp_b1 = fluid_iir_filter_util->b1;

    dsp_filter_coeff_incr_count = fluid_iir_filter_util->filter_coeff_incr_count;
    
    /* Two versions of the filter loop. One, while the filter is
     * changing towards its new setting. The other, if the filter
     * doesn't change.
     */

    if(dsp_filter_coeff_incr_count > 0){
      gdouble dsp_a1_incr;
      gdouble dsp_a2_incr;
      gdouble dsp_b02_incr;
      gdouble dsp_b1_incr;
      
      dsp_a1_incr = fluid_iir_filter_util->a1_incr;
      dsp_a2_incr = fluid_iir_filter_util->a2_incr;
      dsp_b02_incr = fluid_iir_filter_util->b02_incr;
      dsp_b1_incr = fluid_iir_filter_util->b1_incr;

      /* Increment is added to each filter coefficient filter_coeff_incr_count times. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i * source_stride] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;

	destination[dsp_i * destination_stride] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;

	if(dsp_filter_coeff_incr_count-- > 0){
	  gdouble old_b02;

	  old_b02 = dsp_b02;

	  dsp_a1 += dsp_a1_incr;
	  dsp_a2 += dsp_a2_incr;
	  dsp_b02 += dsp_b02_incr;
	  dsp_b1 += dsp_b1_incr;

	  /* Compensate history to avoid the filter going havoc with large frequency changes */
	  if(fluid_iir_filter_util->compensate_incr && fabs(dsp_b02) > 0.001f){
	    gdouble compensate;

	    compensate = old_b02 / dsp_b02;

	    dsp_hist1 *= compensate;
	    dsp_hist2 *= compensate;
	  }
	}
      } /* for dsp_i */
    }else{ /* The filter parameters are constant.  This is duplicated to save time. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i * source_stride] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;
	
	destination[dsp_i * destination_stride] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;
      }
    }
    
    fluid_iir_filter_util->hist1 = dsp_hist1;
    fluid_iir_filter_util->hist2 = dsp_hist2;
    
    fluid_iir_filter_util->a1 = dsp_a1;
    fluid_iir_filter_util->a2 = dsp_a2;
    fluid_iir_filter_util->b02 = dsp_b02;
    fluid_iir_filter_util->b1 = dsp_b1;
    
    fluid_iir_filter_util->filter_coeff_incr_count = dsp_filter_coeff_incr_count;
  }
}

/**
 * ags_fluid_iir_filter_util_process_s32:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Process @fluid_iir_filter_util of signed 32 bit data.
 * 
 * Since: 3.9.7
 */
void
ags_fluid_iir_filter_util_process_s32(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  gint32 *destination, *source;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  
  if(fluid_iir_filter_util == NULL ||
     fluid_iir_filter_util->destination == NULL ||
     fluid_iir_filter_util->source == NULL){
    return;
  }

  destination = fluid_iir_filter_util->destination;
  destination_stride = fluid_iir_filter_util->destination_stride;

  source = fluid_iir_filter_util->source;
  source_stride = fluid_iir_filter_util->source_stride;

  buffer_length = fluid_iir_filter_util->buffer_length;

  if(fluid_iir_filter_util->filter_type == AGS_FLUID_IIR_DISABLED || fluid_iir_filter_util->q_lin == 0.0){
    return;
  }else{
    /* IIR filter sample history */
    gdouble dsp_hist1;
    gdouble dsp_hist2;
    
    /* IIR filter coefficients */
    gdouble dsp_a1;
    gdouble dsp_a2;
    gdouble dsp_b02;
    gdouble dsp_b1;
    gint dsp_filter_coeff_incr_count;
      
    gdouble dsp_centernode;
    gint dsp_i;
      
    /* filter (implement the voice filter according to SoundFont standard) */
    dsp_hist1 = fluid_iir_filter_util->hist1;
    dsp_hist2 = fluid_iir_filter_util->hist2;
    
    dsp_a1 = fluid_iir_filter_util->a1;
    dsp_a2 = fluid_iir_filter_util->a2;
    dsp_b02 = fluid_iir_filter_util->b02;
    dsp_b1 = fluid_iir_filter_util->b1;

    dsp_filter_coeff_incr_count = fluid_iir_filter_util->filter_coeff_incr_count;
    
    /* Two versions of the filter loop. One, while the filter is
     * changing towards its new setting. The other, if the filter
     * doesn't change.
     */

    if(dsp_filter_coeff_incr_count > 0){
      gdouble dsp_a1_incr;
      gdouble dsp_a2_incr;
      gdouble dsp_b02_incr;
      gdouble dsp_b1_incr;
      
      dsp_a1_incr = fluid_iir_filter_util->a1_incr;
      dsp_a2_incr = fluid_iir_filter_util->a2_incr;
      dsp_b02_incr = fluid_iir_filter_util->b02_incr;
      dsp_b1_incr = fluid_iir_filter_util->b1_incr;

      /* Increment is added to each filter coefficient filter_coeff_incr_count times. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i * source_stride] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;

	destination[dsp_i * destination_stride] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;

	if(dsp_filter_coeff_incr_count-- > 0){
	  gdouble old_b02;

	  old_b02 = dsp_b02;

	  dsp_a1 += dsp_a1_incr;
	  dsp_a2 += dsp_a2_incr;
	  dsp_b02 += dsp_b02_incr;
	  dsp_b1 += dsp_b1_incr;

	  /* Compensate history to avoid the filter going havoc with large frequency changes */
	  if(fluid_iir_filter_util->compensate_incr && fabs(dsp_b02) > 0.001f){
	    gdouble compensate;

	    compensate = old_b02 / dsp_b02;

	    dsp_hist1 *= compensate;
	    dsp_hist2 *= compensate;
	  }
	}
      } /* for dsp_i */
    }else{ /* The filter parameters are constant.  This is duplicated to save time. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i * source_stride] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;
	
	destination[dsp_i * destination_stride] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;
      }
    }
    
    fluid_iir_filter_util->hist1 = dsp_hist1;
    fluid_iir_filter_util->hist2 = dsp_hist2;
    
    fluid_iir_filter_util->a1 = dsp_a1;
    fluid_iir_filter_util->a2 = dsp_a2;
    fluid_iir_filter_util->b02 = dsp_b02;
    fluid_iir_filter_util->b1 = dsp_b1;
    
    fluid_iir_filter_util->filter_coeff_incr_count = dsp_filter_coeff_incr_count;
  }
}

/**
 * ags_fluid_iir_filter_util_process_s64:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Process @fluid_iir_filter_util of signed 64 bit data.
 * 
 * Since: 3.9.7
 */
void
ags_fluid_iir_filter_util_process_s64(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  gint64 *destination, *source;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  
  if(fluid_iir_filter_util == NULL ||
     fluid_iir_filter_util->destination == NULL ||
     fluid_iir_filter_util->source == NULL){
    return;
  }

  destination = fluid_iir_filter_util->destination;
  destination_stride = fluid_iir_filter_util->destination_stride;

  source = fluid_iir_filter_util->source;
  source_stride = fluid_iir_filter_util->source_stride;

  buffer_length = fluid_iir_filter_util->buffer_length;

  if(fluid_iir_filter_util->filter_type == AGS_FLUID_IIR_DISABLED || fluid_iir_filter_util->q_lin == 0.0){
    return;
  }else{
    /* IIR filter sample history */
    gdouble dsp_hist1;
    gdouble dsp_hist2;
    
    /* IIR filter coefficients */
    gdouble dsp_a1;
    gdouble dsp_a2;
    gdouble dsp_b02;
    gdouble dsp_b1;
    gint dsp_filter_coeff_incr_count;
      
    gdouble dsp_centernode;
    gint dsp_i;
      
    /* filter (implement the voice filter according to SoundFont standard) */
    dsp_hist1 = fluid_iir_filter_util->hist1;
    dsp_hist2 = fluid_iir_filter_util->hist2;
    
    dsp_a1 = fluid_iir_filter_util->a1;
    dsp_a2 = fluid_iir_filter_util->a2;
    dsp_b02 = fluid_iir_filter_util->b02;
    dsp_b1 = fluid_iir_filter_util->b1;

    dsp_filter_coeff_incr_count = fluid_iir_filter_util->filter_coeff_incr_count;
    
    /* Two versions of the filter loop. One, while the filter is
     * changing towards its new setting. The other, if the filter
     * doesn't change.
     */

    if(dsp_filter_coeff_incr_count > 0){
      gdouble dsp_a1_incr;
      gdouble dsp_a2_incr;
      gdouble dsp_b02_incr;
      gdouble dsp_b1_incr;
      
      dsp_a1_incr = fluid_iir_filter_util->a1_incr;
      dsp_a2_incr = fluid_iir_filter_util->a2_incr;
      dsp_b02_incr = fluid_iir_filter_util->b02_incr;
      dsp_b1_incr = fluid_iir_filter_util->b1_incr;

      /* Increment is added to each filter coefficient filter_coeff_incr_count times. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i * source_stride] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;

	destination[dsp_i * destination_stride] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;

	if(dsp_filter_coeff_incr_count-- > 0){
	  gdouble old_b02;

	  old_b02 = dsp_b02;

	  dsp_a1 += dsp_a1_incr;
	  dsp_a2 += dsp_a2_incr;
	  dsp_b02 += dsp_b02_incr;
	  dsp_b1 += dsp_b1_incr;

	  /* Compensate history to avoid the filter going havoc with large frequency changes */
	  if(fluid_iir_filter_util->compensate_incr && fabs(dsp_b02) > 0.001f){
	    gdouble compensate;

	    compensate = old_b02 / dsp_b02;

	    dsp_hist1 *= compensate;
	    dsp_hist2 *= compensate;
	  }
	}
      } /* for dsp_i */
    }else{ /* The filter parameters are constant.  This is duplicated to save time. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i * source_stride] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;
	
	destination[dsp_i * destination_stride] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;
      }
    }
    
    fluid_iir_filter_util->hist1 = dsp_hist1;
    fluid_iir_filter_util->hist2 = dsp_hist2;
    
    fluid_iir_filter_util->a1 = dsp_a1;
    fluid_iir_filter_util->a2 = dsp_a2;
    fluid_iir_filter_util->b02 = dsp_b02;
    fluid_iir_filter_util->b1 = dsp_b1;
    
    fluid_iir_filter_util->filter_coeff_incr_count = dsp_filter_coeff_incr_count;
  }
}

/**
 * ags_fluid_iir_filter_util_process_float:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Process @fluid_iir_filter_util of floating point data.
 * 
 * Since: 3.9.7
 */
void
ags_fluid_iir_filter_util_process_float(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  gfloat *destination, *source;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  
  if(fluid_iir_filter_util == NULL ||
     fluid_iir_filter_util->destination == NULL ||
     fluid_iir_filter_util->source == NULL){
    return;
  }

  destination = fluid_iir_filter_util->destination;
  destination_stride = fluid_iir_filter_util->destination_stride;

  source = fluid_iir_filter_util->source;
  source_stride = fluid_iir_filter_util->source_stride;

  buffer_length = fluid_iir_filter_util->buffer_length;

  if(fluid_iir_filter_util->filter_type == AGS_FLUID_IIR_DISABLED || fluid_iir_filter_util->q_lin == 0.0){
    return;
  }else{
    /* IIR filter sample history */
    gdouble dsp_hist1;
    gdouble dsp_hist2;
    
    /* IIR filter coefficients */
    gdouble dsp_a1;
    gdouble dsp_a2;
    gdouble dsp_b02;
    gdouble dsp_b1;
    gint dsp_filter_coeff_incr_count;
      
    gdouble dsp_centernode;
    gint dsp_i;
      
    /* filter (implement the voice filter according to SoundFont standard) */
    dsp_hist1 = fluid_iir_filter_util->hist1;
    dsp_hist2 = fluid_iir_filter_util->hist2;
    
    dsp_a1 = fluid_iir_filter_util->a1;
    dsp_a2 = fluid_iir_filter_util->a2;
    dsp_b02 = fluid_iir_filter_util->b02;
    dsp_b1 = fluid_iir_filter_util->b1;

    dsp_filter_coeff_incr_count = fluid_iir_filter_util->filter_coeff_incr_count;
    
    /* Two versions of the filter loop. One, while the filter is
     * changing towards its new setting. The other, if the filter
     * doesn't change.
     */

    if(dsp_filter_coeff_incr_count > 0){
      gdouble dsp_a1_incr;
      gdouble dsp_a2_incr;
      gdouble dsp_b02_incr;
      gdouble dsp_b1_incr;
      
      dsp_a1_incr = fluid_iir_filter_util->a1_incr;
      dsp_a2_incr = fluid_iir_filter_util->a2_incr;
      dsp_b02_incr = fluid_iir_filter_util->b02_incr;
      dsp_b1_incr = fluid_iir_filter_util->b1_incr;

      /* Increment is added to each filter coefficient filter_coeff_incr_count times. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i * source_stride] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;

	destination[dsp_i * destination_stride] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;

	if(dsp_filter_coeff_incr_count-- > 0){
	  gdouble old_b02;

	  old_b02 = dsp_b02;

	  dsp_a1 += dsp_a1_incr;
	  dsp_a2 += dsp_a2_incr;
	  dsp_b02 += dsp_b02_incr;
	  dsp_b1 += dsp_b1_incr;

	  /* Compensate history to avoid the filter going havoc with large frequency changes */
	  if(fluid_iir_filter_util->compensate_incr && fabs(dsp_b02) > 0.001f){
	    gdouble compensate;

	    compensate = old_b02 / dsp_b02;

	    dsp_hist1 *= compensate;
	    dsp_hist2 *= compensate;
	  }
	}
      } /* for dsp_i */
    }else{ /* The filter parameters are constant.  This is duplicated to save time. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i * source_stride] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;
	
	destination[dsp_i * destination_stride] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;
      }
    }
    
    fluid_iir_filter_util->hist1 = dsp_hist1;
    fluid_iir_filter_util->hist2 = dsp_hist2;
    
    fluid_iir_filter_util->a1 = dsp_a1;
    fluid_iir_filter_util->a2 = dsp_a2;
    fluid_iir_filter_util->b02 = dsp_b02;
    fluid_iir_filter_util->b1 = dsp_b1;
    
    fluid_iir_filter_util->filter_coeff_incr_count = dsp_filter_coeff_incr_count;
  }
}

/**
 * ags_fluid_iir_filter_util_process_double:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Process @fluid_iir_filter_util of double precision floating point data.
 * 
 * Since: 3.9.7
 */
void
ags_fluid_iir_filter_util_process_double(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  gdouble *destination, *source;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  
  if(fluid_iir_filter_util == NULL ||
     fluid_iir_filter_util->destination == NULL ||
     fluid_iir_filter_util->source == NULL){
    return;
  }

  destination = fluid_iir_filter_util->destination;
  destination_stride = fluid_iir_filter_util->destination_stride;

  source = fluid_iir_filter_util->source;
  source_stride = fluid_iir_filter_util->source_stride;

  buffer_length = fluid_iir_filter_util->buffer_length;

  if(fluid_iir_filter_util->filter_type == AGS_FLUID_IIR_DISABLED || fluid_iir_filter_util->q_lin == 0.0){
    return;
  }else{
    /* IIR filter sample history */
    gdouble dsp_hist1;
    gdouble dsp_hist2;
    
    /* IIR filter coefficients */
    gdouble dsp_a1;
    gdouble dsp_a2;
    gdouble dsp_b02;
    gdouble dsp_b1;
    gint dsp_filter_coeff_incr_count;
      
    gdouble dsp_centernode;
    gint dsp_i;
      
    /* filter (implement the voice filter according to SoundFont standard) */
    dsp_hist1 = fluid_iir_filter_util->hist1;
    dsp_hist2 = fluid_iir_filter_util->hist2;
    
    dsp_a1 = fluid_iir_filter_util->a1;
    dsp_a2 = fluid_iir_filter_util->a2;
    dsp_b02 = fluid_iir_filter_util->b02;
    dsp_b1 = fluid_iir_filter_util->b1;

    dsp_filter_coeff_incr_count = fluid_iir_filter_util->filter_coeff_incr_count;
    
    /* Two versions of the filter loop. One, while the filter is
     * changing towards its new setting. The other, if the filter
     * doesn't change.
     */

    if(dsp_filter_coeff_incr_count > 0){
      gdouble dsp_a1_incr;
      gdouble dsp_a2_incr;
      gdouble dsp_b02_incr;
      gdouble dsp_b1_incr;
      
      dsp_a1_incr = fluid_iir_filter_util->a1_incr;
      dsp_a2_incr = fluid_iir_filter_util->a2_incr;
      dsp_b02_incr = fluid_iir_filter_util->b02_incr;
      dsp_b1_incr = fluid_iir_filter_util->b1_incr;

      /* Increment is added to each filter coefficient filter_coeff_incr_count times. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i * source_stride] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;

	destination[dsp_i * destination_stride] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;

	if(dsp_filter_coeff_incr_count-- > 0){
	  gdouble old_b02;

	  old_b02 = dsp_b02;

	  dsp_a1 += dsp_a1_incr;
	  dsp_a2 += dsp_a2_incr;
	  dsp_b02 += dsp_b02_incr;
	  dsp_b1 += dsp_b1_incr;

	  /* Compensate history to avoid the filter going havoc with large frequency changes */
	  if(fluid_iir_filter_util->compensate_incr && fabs(dsp_b02) > 0.001f){
	    gdouble compensate;

	    compensate = old_b02 / dsp_b02;

	    dsp_hist1 *= compensate;
	    dsp_hist2 *= compensate;
	  }
	}
      } /* for dsp_i */
    }else{ /* The filter parameters are constant.  This is duplicated to save time. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i * source_stride] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;
	
	destination[dsp_i * destination_stride] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;
      }
    }
    
    fluid_iir_filter_util->hist1 = dsp_hist1;
    fluid_iir_filter_util->hist2 = dsp_hist2;
    
    fluid_iir_filter_util->a1 = dsp_a1;
    fluid_iir_filter_util->a2 = dsp_a2;
    fluid_iir_filter_util->b02 = dsp_b02;
    fluid_iir_filter_util->b1 = dsp_b1;
    
    fluid_iir_filter_util->filter_coeff_incr_count = dsp_filter_coeff_incr_count;
  }
}

/**
 * ags_fluid_iir_filter_util_process_complex:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Process @fluid_iir_filter_util of complex data.
 * 
 * Since: 3.9.7
 */
void
ags_fluid_iir_filter_util_process_complex(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  AgsComplex *destination, *source;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  
  if(fluid_iir_filter_util == NULL ||
     fluid_iir_filter_util->destination == NULL ||
     fluid_iir_filter_util->source == NULL){
    return;
  }

  destination = fluid_iir_filter_util->destination;
  destination_stride = fluid_iir_filter_util->destination_stride;

  source = fluid_iir_filter_util->source;
  source_stride = fluid_iir_filter_util->source_stride;

  buffer_length = fluid_iir_filter_util->buffer_length;

  if(fluid_iir_filter_util->filter_type == AGS_FLUID_IIR_DISABLED || fluid_iir_filter_util->q_lin == 0.0){
    return;
  }else{
    /* IIR filter sample history */
    gdouble dsp_hist1;
    gdouble dsp_hist2;
    
    /* IIR filter coefficients */
    gdouble dsp_a1;
    gdouble dsp_a2;
    gdouble dsp_b02;
    gdouble dsp_b1;
    gint dsp_filter_coeff_incr_count;
      
    gdouble dsp_centernode;
    gint dsp_i;
      
    /* filter (implement the voice filter according to SoundFont standard) */
    dsp_hist1 = fluid_iir_filter_util->hist1;
    dsp_hist2 = fluid_iir_filter_util->hist2;
    
    dsp_a1 = fluid_iir_filter_util->a1;
    dsp_a2 = fluid_iir_filter_util->a2;
    dsp_b02 = fluid_iir_filter_util->b02;
    dsp_b1 = fluid_iir_filter_util->b1;

    dsp_filter_coeff_incr_count = fluid_iir_filter_util->filter_coeff_incr_count;
    
    /* Two versions of the filter loop. One, while the filter is
     * changing towards its new setting. The other, if the filter
     * doesn't change.
     */

    if(dsp_filter_coeff_incr_count > 0){
      gdouble dsp_a1_incr;
      gdouble dsp_a2_incr;
      gdouble dsp_b02_incr;
      gdouble dsp_b1_incr;
      
      dsp_a1_incr = fluid_iir_filter_util->a1_incr;
      dsp_a2_incr = fluid_iir_filter_util->a2_incr;
      dsp_b02_incr = fluid_iir_filter_util->b02_incr;
      dsp_b1_incr = fluid_iir_filter_util->b1_incr;

      /* Increment is added to each filter coefficient filter_coeff_incr_count times. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = ags_complex_get(source + (dsp_i * source_stride)) - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;

	ags_complex_set(destination + (dsp_i * destination_stride),
			(dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1));

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;

	if(dsp_filter_coeff_incr_count-- > 0){
	  gdouble old_b02;

	  old_b02 = dsp_b02;

	  dsp_a1 += dsp_a1_incr;
	  dsp_a2 += dsp_a2_incr;
	  dsp_b02 += dsp_b02_incr;
	  dsp_b1 += dsp_b1_incr;

	  /* Compensate history to avoid the filter going havoc with large frequency changes */
	  if(fluid_iir_filter_util->compensate_incr && fabs(dsp_b02) > 0.001f){
	    gdouble compensate;

	    compensate = old_b02 / dsp_b02;

	    dsp_hist1 *= compensate;
	    dsp_hist2 *= compensate;
	  }
	}
      } /* for dsp_i */
    }else{ /* The filter parameters are constant.  This is duplicated to save time. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = ags_complex_get(source + (dsp_i * source_stride)) - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;
	
	ags_complex_set(destination + (dsp_i * destination_stride),
			dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1);

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;
      }
    }
    
    fluid_iir_filter_util->hist1 = dsp_hist1;
    fluid_iir_filter_util->hist2 = dsp_hist2;
    
    fluid_iir_filter_util->a1 = dsp_a1;
    fluid_iir_filter_util->a2 = dsp_a2;
    fluid_iir_filter_util->b02 = dsp_b02;
    fluid_iir_filter_util->b1 = dsp_b1;
    
    fluid_iir_filter_util->filter_coeff_incr_count = dsp_filter_coeff_incr_count;
  }
}

/**
 * ags_fluid_iir_filter_util_process:
 * @fluid_iir_filter_util: the #AgsFluidIIRFilterUtil-struct
 * 
 * Process @fluid_iir_filter_util.
 * 
 * Since: 3.9.7
 */
void
ags_fluid_iir_filter_util_process(AgsFluidIIRFilterUtil *fluid_iir_filter_util)
{
  if(fluid_iir_filter_util == NULL ||
     fluid_iir_filter_util->destination == NULL ||
     fluid_iir_filter_util->source == NULL){
    return;
  }

  switch(fluid_iir_filter_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_fluid_iir_filter_util_process_s8(fluid_iir_filter_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_fluid_iir_filter_util_process_s16(fluid_iir_filter_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_fluid_iir_filter_util_process_s24(fluid_iir_filter_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_fluid_iir_filter_util_process_s32(fluid_iir_filter_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_fluid_iir_filter_util_process_s64(fluid_iir_filter_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_fluid_iir_filter_util_process_float(fluid_iir_filter_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_fluid_iir_filter_util_process_double(fluid_iir_filter_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_fluid_iir_filter_util_process_complex(fluid_iir_filter_util);
  }
  break;
  default:
    g_warning("unknown format");
  }
}
