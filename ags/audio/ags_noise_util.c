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
 */

#include <ags/audio/ags_noise_util.h>
/**
 * SECTION:ags_noise_util
 * @short_description: noise util
 * @title: AgsNoiseUtil
 * @section_id:
 * @include: ags/audio/ags_noise_util.h
 *
 * Utility functions to compute noise.
 */

GType
ags_noise_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_noise_util = 0;

    ags_type_noise_util =
      g_boxed_type_register_static("AgsNoiseUtil",
				   (GBoxedCopyFunc) ags_noise_util_copy,
				   (GBoxedFreeFunc) ags_noise_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_noise_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_noise_util_alloc:
 * 
 * Allocate #AgsNoiseUtil-struct.
 * 
 * Returns: the newly allocated #AgsNoiseUtil-struct
 * 
 * Since: 3.15.0
 */
AgsNoiseUtil*
ags_noise_util_alloc()
{
  AgsNoiseUtil *ptr;
  
  ptr = (AgsNoiseUtil *) g_new(AgsNoiseUtil,
			       1);

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->destination = NULL;
  ptr->destination_stride = 1;
  
  ptr->buffer_length = 0;
  ptr->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  ptr->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  ptr->mode = AGS_NOISE_UTIL_WHITE_NOISE;

  ptr->volume = 0.0;

  ptr->frequency = AGS_NOISE_UTIL_DEFAULT_FREQUENCY;

  return(ptr);
}

/**
 * ags_noise_util_copy:
 * @ptr: the original #AgsNoiseUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsNoiseUtil-struct
 *
 * Since: 3.15.0
 */
gpointer
ags_noise_util_copy(AgsNoiseUtil *ptr)
{
  AgsNoiseUtil *new_ptr;
  
  new_ptr = (AgsNoiseUtil *) g_new(AgsNoiseUtil,
				   1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;
  
  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->mode = ptr->mode;

  new_ptr->volume = ptr->volume;

  new_ptr->frequency = ptr->frequency;

  return(new_ptr);
}

/**
 * ags_noise_util_free:
 * @ptr: the #AgsNoiseUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_free(AgsNoiseUtil *ptr)
{
  g_free(ptr->destination);

  if(ptr->destination != ptr->source){
    g_free(ptr->source);
  }

  g_free(ptr->low_mix_buffer);
  g_free(ptr->new_mix_buffer);
  
  g_free(ptr);
}

/**
 * ags_noise_util_get_destination:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get destination buffer of @noise_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 3.15.0
 */
gpointer
ags_noise_util_get_destination(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(NULL);
  }

  return(noise_util->destination);
}

/**
 * ags_noise_util_set_destination:
 * @noise_util: the #AgsNoiseUtil-struct
 * @destination: the destination buffer
 *
 * Set @destination buffer of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_destination(AgsNoiseUtil *noise_util,
			       gpointer destination)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->destination = destination;
}

/**
 * ags_noise_util_get_destination_stride:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get destination stride of @noise_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 3.15.0
 */
guint
ags_noise_util_get_destination_stride(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(0);
  }

  return(noise_util->destination_stride);
}

/**
 * ags_noise_util_set_destination_stride:
 * @noise_util: the #AgsNoiseUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_destination_stride(AgsNoiseUtil *noise_util,
				      guint destination_stride)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->destination_stride = destination_stride;
}

/**
 * ags_noise_util_get_source:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get source buffer of @noise_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.15.0
 */
gpointer
ags_noise_util_get_source(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(NULL);
  }

  return(noise_util->source);
}

/**
 * ags_noise_util_set_source:
 * @noise_util: the #AgsNoiseUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_source(AgsNoiseUtil *noise_util,
			  gpointer source)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->source = source;
}

/**
 * ags_noise_util_get_source_stride:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get source stride of @noise_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.15.0
 */
guint
ags_noise_util_get_source_stride(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(0);
  }

  return(noise_util->source_stride);
}

/**
 * ags_noise_util_set_source_stride:
 * @noise_util: the #AgsNoiseUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_source_stride(AgsNoiseUtil *noise_util,
				 guint source_stride)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->source_stride = source_stride;
}

/**
 * ags_noise_util_get_buffer_length:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get buffer length of @noise_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.15.0
 */
guint
ags_noise_util_get_buffer_length(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(0);
  }

  return(noise_util->buffer_length);
}

/**
 * ags_noise_util_set_buffer_length:
 * @noise_util: the #AgsNoiseUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_buffer_length(AgsNoiseUtil *noise_util,
				 guint buffer_length)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->buffer_length = buffer_length;
}

/**
 * ags_noise_util_get_format:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get format of @noise_util.
 * 
 * Returns: the format
 * 
 * Since: 3.15.0
 */
guint
ags_noise_util_get_format(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(0);
  }

  return(noise_util->format);
}

/**
 * ags_noise_util_set_format:
 * @noise_util: the #AgsNoiseUtil-struct
 * @format: the format
 *
 * Set @format of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_format(AgsNoiseUtil *noise_util,
			  guint format)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->format = format;
}

/**
 * ags_noise_util_get_samplerate:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get samplerate of @noise_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.15.0
 */
guint
ags_noise_util_get_samplerate(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(0);
  }

  return(noise_util->samplerate);
}

/**
 * ags_noise_util_set_samplerate:
 * @noise_util: the #AgsNoiseUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_samplerate(AgsNoiseUtil *noise_util,
			      guint samplerate)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->samplerate = samplerate;
}

/**
 * ags_noise_util_get_mode:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get mode of @noise_util.
 * 
 * Returns: the mode
 * 
 * Since: 3.15.0
 */
guint
ags_noise_util_get_mode(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(0);
  }

  return(noise_util->mode);
}

/**
 * ags_noise_util_set_mode:
 * @noise_util: the #AgsNoiseUtil-struct
 * @mode: the mode
 *
 * Set @mode of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_mode(AgsNoiseUtil *noise_util,
			guint mode)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->mode = mode;
}

/**
 * ags_noise_util_get_volume:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get volume of @noise_util.
 * 
 * Returns: the volume
 * 
 * Since: 3.15.0
 */
gdouble
ags_noise_util_get_volume(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(0.0);
  }

  return(noise_util->volume);
}

/**
 * ags_noise_util_set_volume:
 * @noise_util: the #AgsNoiseUtil-struct
 * @volume: the volume
 *
 * Set @volume of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_volume(AgsNoiseUtil *noise_util,
			  gdouble volume)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->volume = volume;
}

/**
 * ags_noise_util_get_frequency:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get frequency of @noise_util.
 * 
 * Returns: the frequency
 * 
 * Since: 3.15.0
 */
gdouble
ags_noise_util_get_frequency(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(0.0);
  }

  return(noise_util->frequency);
}

/**
 * ags_noise_util_set_frequency:
 * @noise_util: the #AgsNoiseUtil-struct
 * @frequency: the frequency
 *
 * Set @frequency of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_frequency(AgsNoiseUtil *noise_util,
			     gdouble frequency)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->frequency = frequency;
}

/**
 * ags_noise_util_compute_s8:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Compute noise of @noise_util of signed 8 bit data.
 *  
 * Since: 3.15.0
 */
void
ags_noise_util_compute_s8(AgsNoiseUtil *noise_util)
{
  //TODO:JK: implement me
}

/**
 * ags_noise_util_compute_s16:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Compute noise of @noise_util of signed 16 bit data.
 *  
 * Since: 3.15.0
 */
void
ags_noise_util_compute_s16(AgsNoiseUtil *noise_util)
{
  //TODO:JK: implement me
}

/**
 * ags_noise_util_compute_s24:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Compute noise of @noise_util of signed 24 bit data.
 *  
 * Since: 3.15.0
 */
void
ags_noise_util_compute_s24(AgsNoiseUtil *noise_util)
{
  //TODO:JK: implement me
}

/**
 * ags_noise_util_compute_s32:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Compute noise of @noise_util of signed 32 bit data.
 *  
 * Since: 3.15.0
 */
void
ags_noise_util_compute_s32(AgsNoiseUtil *noise_util)
{
  //TODO:JK: implement me
}

/**
 * ags_noise_util_compute_s64:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Compute noise of @noise_util of signed 64 bit data.
 *  
 * Since: 3.15.0
 */
void
ags_noise_util_compute_s64(AgsNoiseUtil *noise_util)
{
  //TODO:JK: implement me
}

/**
 * ags_noise_util_compute_float:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Compute noise of @noise_util of floating point data.
 *  
 * Since: 3.15.0
 */
void
ags_noise_util_compute_float(AgsNoiseUtil *noise_util)
{
  //TODO:JK: implement me
}

/**
 * ags_noise_util_compute_double:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Compute noise of @noise_util of double precision floating point data.
 *  
 * Since: 3.15.0
 */
void
ags_noise_util_compute_double(AgsNoiseUtil *noise_util)
{
  //TODO:JK: implement me
}

/**
 * ags_noise_util_compute_complex:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Compute noise of @noise_util of complex data.
 *  
 * Since: 3.15.0
 */
void
ags_noise_util_compute_complex(AgsNoiseUtil *noise_util)
{
  //TODO:JK: implement me
}

/**
 * ags_noise_util_compute:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Compute noise of @noise_util.
 *  
 * Since: 3.15.0
 */
void
ags_noise_util_compute(AgsNoiseUtil *noise_util)
{
  //TODO:JK: implement me
}

