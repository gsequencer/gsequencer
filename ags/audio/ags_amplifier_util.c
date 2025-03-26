/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/audio/ags_amplifier_util.h>

#include <ags/audio/ags_audio_buffer_util.h>

/**
 * SECTION:ags_amplifier_util
 * @short_description: Boxed type of amplifier util
 * @title: AgsAmplifierUtil
 * @section_id:
 * @include: ags/audio/ags_amplifier_util.h
 *
 * Boxed type of amplifier util data type.
 */

GType
ags_amplifier_util_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_amplifier_util = 0;

    ags_type_amplifier_util =
      g_boxed_type_register_static("AgsAmplifierUtil",
				   (GBoxedCopyFunc) ags_amplifier_util_copy,
				   (GBoxedFreeFunc) ags_amplifier_util_free);

    g_once_init_leave(&g_define_type_id__static, ags_type_amplifier_util);
  }

  return(g_define_type_id__static);
}

/**
 * ags_amplifier_util_alloc:
 *
 * Allocate #AgsAmplifierUtil-struct
 *
 * Returns: a new #AgsAmplifierUtil-struct
 *
 * Since: 7.7.0
 */
AgsAmplifierUtil*
ags_amplifier_util_alloc()
{
  AgsAmplifierUtil *ptr;

  ptr = (AgsAmplifierUtil *) g_new(AgsAmplifierUtil,
				   1);

  ptr[0] = AGS_AMPLIFIER_UTIL_INITIALIZER;

  return(ptr);
}

/**
 * ags_amplifier_util_copy:
 * @ptr: the original #AgsAmplifierUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsAmplifierUtil-struct
 *
 * Since: 7.7.0
 */
gpointer
ags_amplifier_util_copy(AgsAmplifierUtil *ptr)
{
  AgsAmplifierUtil *new_ptr;

  g_return_val_if_fail(ptr != NULL, NULL);
  
  new_ptr = (AgsAmplifierUtil *) g_new(AgsAmplifierUtil,
				       1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->amp_0_frequency = ptr->amp_0_frequency;
  new_ptr->amp_0_bandwidth = ptr->amp_0_bandwidth;
  new_ptr->amp_0_gain = ptr->amp_0_gain;

  new_ptr->amp_1_frequency = ptr->amp_1_frequency;
  new_ptr->amp_1_bandwidth = ptr->amp_1_bandwidth;
  new_ptr->amp_1_gain = ptr->amp_1_gain;

  new_ptr->amp_2_frequency = ptr->amp_2_frequency;
  new_ptr->amp_2_bandwidth = ptr->amp_2_bandwidth;
  new_ptr->amp_2_gain = ptr->amp_2_gain;

  new_ptr->amp_3_frequency = ptr->amp_3_frequency;
  new_ptr->amp_3_bandwidth = ptr->amp_3_bandwidth;
  new_ptr->amp_3_gain = ptr->amp_3_gain;

  new_ptr->filter_gain = ptr->filter_gain;

  return(new_ptr);
}

/**
 * ags_amplifier_util_free:
 * @ptr: the #AgsAmplifierUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_free(AgsAmplifierUtil *ptr)
{
  g_return_if_fail(ptr != NULL);

  //  g_free(ptr->destination);

  //  if(ptr->destination != ptr->source){
  //    g_free(ptr->source);
  //  }
  
  g_free(ptr);
}

/**
 * ags_amplifier_util_get_destination:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get destination buffer of @amplifier_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 7.7.0
 */
gpointer
ags_amplifier_util_get_destination(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(NULL);
  }

  return(amplifier_util->destination);
}

/**
 * ags_amplifier_util_set_destination:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @destination: (transfer none) : the destination buffer
 *
 * Set @destination buffer of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_destination(AgsAmplifierUtil *amplifier_util,
				   gpointer destination)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->destination = destination;
}

/**
 * ags_amplifier_util_get_destination_stride:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get destination stride of @amplifier_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 7.7.0
 */
guint
ags_amplifier_util_get_destination_stride(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0);
  }

  return(amplifier_util->destination_stride);
}

/**
 * ags_amplifier_util_set_destination_stride:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_destination_stride(AgsAmplifierUtil *amplifier_util,
					  guint destination_stride)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->destination_stride = destination_stride;
}

/**
 * ags_amplifier_util_get_source:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get source buffer of @amplifier_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 7.7.0
 */
gpointer
ags_amplifier_util_get_source(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(NULL);
  }

  return(amplifier_util->source);
}

/**
 * ags_amplifier_util_set_source:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @source: (transfer none): the source buffer
 *
 * Set @source buffer of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_source(AgsAmplifierUtil *amplifier_util,
			      gpointer source)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->source = source;
}

/**
 * ags_amplifier_util_get_source_stride:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get source stride of @amplifier_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 7.7.0
 */
guint
ags_amplifier_util_get_source_stride(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0);
  }

  return(amplifier_util->source_stride);
}

/**
 * ags_amplifier_util_set_source_stride:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_source_stride(AgsAmplifierUtil *amplifier_util,
				     guint source_stride)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->source_stride = source_stride;
}

/**
 * ags_amplifier_util_get_buffer_length:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get buffer length of @amplifier_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 7.7.0
 */
guint
ags_amplifier_util_get_buffer_length(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0);
  }

  return(amplifier_util->buffer_length);
}

/**
 * ags_amplifier_util_set_buffer_length:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_buffer_length(AgsAmplifierUtil *amplifier_util,
				     guint buffer_length)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->buffer_length = buffer_length;
}

/**
 * ags_amplifier_util_get_format:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get format of @amplifier_util.
 * 
 * Returns: the format
 * 
 * Since: 7.7.0
 */
AgsSoundcardFormat
ags_amplifier_util_get_format(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0);
  }

  return(amplifier_util->format);
}

/**
 * ags_amplifier_util_set_format:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @format: the format
 *
 * Set @format of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_format(AgsAmplifierUtil *amplifier_util,
			      AgsSoundcardFormat format)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->format = format;
}

/**
 * ags_amplifier_util_get_samplerate:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get samplerate of @amplifier_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 7.7.0
 */
guint
ags_amplifier_util_get_samplerate(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0);
  }

  return(amplifier_util->samplerate);
}

/**
 * ags_amplifier_util_set_samplerate:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_samplerate(AgsAmplifierUtil *amplifier_util,
				  guint samplerate)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->samplerate = samplerate;
}

/**
 * ags_amplifier_util_get_amp_0_frequency:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 0 frequency of @amplifier_util.
 * 
 * Returns: the frequency of amp 0
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_0_frequency(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_0_frequency);
}

/**
 * ags_amplifier_util_set_amp_0_frequency:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_0_frequency: the amp 0 frequency
 *
 * Set @amp_0_frequency of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_0_frequency(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_0_frequency)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_0_frequency = amp_0_frequency;
}

/**
 * ags_amplifier_util_get_amp_0_bandwidth:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 0 bandwidth of @amplifier_util.
 * 
 * Returns: the bandwidth of amp 0
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_0_bandwidth(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_0_bandwidth);
}

/**
 * ags_amplifier_util_set_amp_0_bandwidth:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_0_bandwidth: the amp 0 bandwidth
 *
 * Set @amp_0_bandwidth of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_0_bandwidth(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_0_bandwidth)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_0_bandwidth = amp_0_bandwidth;
}

/**
 * ags_amplifier_util_get_amp_0_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 0 gain of @amplifier_util.
 * 
 * Returns: the gain of amp 0
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_0_gain(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_0_gain);
}

/**
 * ags_amplifier_util_set_amp_0_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_0_gain: the amp 0 gain
 *
 * Set @amp_0_gain of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_0_gain(AgsAmplifierUtil *amplifier_util,
				  gdouble amp_0_gain)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_0_gain = amp_0_gain;
}

/**
 * ags_amplifier_util_get_amp_1_frequency:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 1 frequency of @amplifier_util.
 * 
 * Returns: the frequency of amp 1
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_1_frequency(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_1_frequency);
}

/**
 * ags_amplifier_util_set_amp_1_frequency:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_1_frequency: the amp 1 frequency
 *
 * Set @amp_1_frequency of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_1_frequency(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_1_frequency)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_1_frequency = amp_1_frequency;
}

/**
 * ags_amplifier_util_get_amp_1_bandwidth:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 1 bandwidth of @amplifier_util.
 * 
 * Returns: the bandwidth of amp 1
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_1_bandwidth(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_1_bandwidth);
}

/**
 * ags_amplifier_util_set_amp_1_bandwidth:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_1_bandwidth: the amp 1 bandwidth
 *
 * Set @amp_1_bandwidth of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_1_bandwidth(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_1_bandwidth)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_1_bandwidth = amp_1_bandwidth;
}

/**
 * ags_amplifier_util_get_amp_1_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 1 gain of @amplifier_util.
 * 
 * Returns: the gain of amp 1
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_1_gain(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_1_gain);
}

/**
 * ags_amplifier_util_set_amp_1_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_1_gain: the amp 1 gain
 *
 * Set @amp_1_gain of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_1_gain(AgsAmplifierUtil *amplifier_util,
				  gdouble amp_1_gain)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_1_gain = amp_1_gain;
}

/**
 * ags_amplifier_util_get_amp_2_frequency:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 2 frequency of @amplifier_util.
 * 
 * Returns: the frequency of amp 2
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_2_frequency(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_2_frequency);
}

/**
 * ags_amplifier_util_set_amp_2_frequency:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_2_frequency: the amp 2 frequency
 *
 * Set @amp_2_frequency of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_2_frequency(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_2_frequency)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_2_frequency = amp_2_frequency;
}

/**
 * ags_amplifier_util_get_amp_2_bandwidth:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 2 bandwidth of @amplifier_util.
 * 
 * Returns: the bandwidth of amp 2
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_2_bandwidth(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_2_bandwidth);
}

/**
 * ags_amplifier_util_set_amp_2_bandwidth:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_2_bandwidth: the amp 2 bandwidth
 *
 * Set @amp_2_bandwidth of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_2_bandwidth(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_2_bandwidth)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_2_bandwidth = amp_2_bandwidth;
}

/**
 * ags_amplifier_util_get_amp_2_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 2 gain of @amplifier_util.
 * 
 * Returns: the gain of amp 2
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_2_gain(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_2_gain);
}

/**
 * ags_amplifier_util_set_amp_2_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_2_gain: the amp 2 gain
 *
 * Set @amp_2_gain of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_2_gain(AgsAmplifierUtil *amplifier_util,
				  gdouble amp_2_gain)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_2_gain = amp_2_gain;
}

/**
 * ags_amplifier_util_get_amp_3_frequency:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 3 frequency of @amplifier_util.
 * 
 * Returns: the frequency of amp 3
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_3_frequency(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_3_frequency);
}

/**
 * ags_amplifier_util_set_amp_3_frequency:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_3_frequency: the amp 3 frequency
 *
 * Set @amp_3_frequency of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_3_frequency(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_3_frequency)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_3_frequency = amp_3_frequency;
}

/**
 * ags_amplifier_util_get_amp_3_bandwidth:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 3 bandwidth of @amplifier_util.
 * 
 * Returns: the bandwidth of amp 3
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_3_bandwidth(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_3_bandwidth);
}

/**
 * ags_amplifier_util_set_amp_3_bandwidth:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_3_bandwidth: the amp 3 bandwidth
 *
 * Set @amp_3_bandwidth of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_3_bandwidth(AgsAmplifierUtil *amplifier_util,
				       gdouble amp_3_bandwidth)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_3_bandwidth = amp_3_bandwidth;
}

/**
 * ags_amplifier_util_get_amp_3_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get amp 3 gain of @amplifier_util.
 * 
 * Returns: the gain of amp 3
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_amp_3_gain(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->amp_3_gain);
}

/**
 * ags_amplifier_util_set_amp_3_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @amp_3_gain: the amp 3 gain
 *
 * Set @amp_3_gain of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_amp_3_gain(AgsAmplifierUtil *amplifier_util,
				  gdouble amp_3_gain)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->amp_3_gain = amp_3_gain;
}

/**
 * ags_amplifier_util_get_filter_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Get filter gain of @amplifier_util.
 * 
 * Returns: the gain of filter
 * 
 * Since: 7.7.0
 */
gdouble
ags_amplifier_util_get_filter_gain(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL){
    return(0.0);
  }

  return(amplifier_util->filter_gain);
}

/**
 * ags_amplifier_util_set_filter_gain:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * @filter_gain: the filter gain
 *
 * Set @filter_gain of @amplifier_util.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_set_filter_gain(AgsAmplifierUtil *amplifier_util,
				   gdouble filter_gain)
{
  if(amplifier_util == NULL){
    return;
  }

  amplifier_util->filter_gain = filter_gain;
}

/**
 * ags_amplifier_util_process_s8:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Process amplifier of signed 8 bit data.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_process_s8(AgsAmplifierUtil *amplifier_util)
{
  //TODO:JK: implement me
}

/**
 * ags_amplifier_util_process_s16:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Process amplifier of signed 16 bit data.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_process_s16(AgsAmplifierUtil *amplifier_util)
{
  //TODO:JK: implement me
}

/**
 * ags_amplifier_util_process_s24:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Process amplifier of signed 24 bit data.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_process_s24(AgsAmplifierUtil *amplifier_util)
{
  //TODO:JK: implement me
}

/**
 * ags_amplifier_util_process_s32:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Process amplifier of signed 32 bit data.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_process_s32(AgsAmplifierUtil *amplifier_util)
{
  //TODO:JK: implement me
}

/**
 * ags_amplifier_util_process_s64:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Process amplifier of signed 64 bit data.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_process_s64(AgsAmplifierUtil *amplifier_util)
{
  //TODO:JK: implement me
}

/**
 * ags_amplifier_util_process_float:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Process amplifier of floating point data.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_process_float(AgsAmplifierUtil *amplifier_util)
{
  //TODO:JK: implement me
}

/**
 * ags_amplifier_util_process_double:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Process amplifier of double precision floating point data.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_process_double(AgsAmplifierUtil *amplifier_util)
{
  //TODO:JK: implement me
}

/**
 * ags_amplifier_util_process_complex:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Process amplifier of complex floating point data.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_process_complex(AgsAmplifierUtil *amplifier_util)
{
  //TODO:JK: implement me
}

/**
 * ags_amplifier_util_process:
 * @amplifier_util: the #AgsAmplifierUtil-struct
 * 
 * Process amplifier.
 *
 * Since: 7.7.0
 */
void
ags_amplifier_util_process(AgsAmplifierUtil *amplifier_util)
{
  if(amplifier_util == NULL ||
     amplifier_util->destination == NULL ||
     amplifier_util->source == NULL){
    return;
  }

  switch(amplifier_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      ags_amplifier_util_process_s8(amplifier_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      ags_amplifier_util_process_s16(amplifier_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      ags_amplifier_util_process_s24(amplifier_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      ags_amplifier_util_process_s32(amplifier_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      ags_amplifier_util_process_s64(amplifier_util);
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      ags_amplifier_util_process_float(amplifier_util);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      ags_amplifier_util_process_double(amplifier_util);
    }
    break;
  case AGS_SOUNDCARD_COMPLEX:
    {
      ags_amplifier_util_process_complex(amplifier_util);
    }
    break;
  }
}
