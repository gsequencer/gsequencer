/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/audio/ags_envelope_util.h>

#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_vector_256_manager.h>

#if defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#include <Accelerate/Accelerate.h>

#define AGS_VECTORIZED_BUILTIN_FUNCTIONS 1
#define AGS_VECTOR_256_FUNCTIONS 1
#endif

/**
 * SECTION:ags_envelope_util
 * @short_description: Boxed type of envelope util
 * @title: AgsEnvelopeUtil
 * @section_id:
 * @include: ags/audio/ags_envelope_util.h
 *
 * Boxed type of envelope util data type.
 */

GType
ags_envelope_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_envelope_util = 0;

    ags_type_envelope_util =
      g_boxed_type_register_static("AgsEnvelopeUtil",
				   (GBoxedCopyFunc) ags_envelope_util_copy,
				   (GBoxedFreeFunc) ags_envelope_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_envelope_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_envelope_util_alloc:
 *
 * Allocate #AgsEnvelopeUtil-struct
 *
 * Returns: a new #AgsEnvelopeUtil-struct
 *
 * Since: 3.9.2
 */
AgsEnvelopeUtil*
ags_envelope_util_alloc()
{
  AgsEnvelopeUtil *ptr;

  ptr = (AgsEnvelopeUtil *) g_new(AgsEnvelopeUtil,
				  1);

  ptr[0] = AGS_ENVELOPE_UTIL_INITIALIZER;

  return(ptr);
}

/**
 * ags_envelope_util_copy:
 * @ptr: the original #AgsEnvelopeUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsEnvelopeUtil-struct
 *
 * Since: 3.9.2
 */
gpointer
ags_envelope_util_copy(AgsEnvelopeUtil *ptr)
{
  AgsEnvelopeUtil *new_ptr;

  g_return_val_if_fail(ptr != NULL, NULL);
  
  new_ptr = (AgsEnvelopeUtil *) g_new(AgsEnvelopeUtil,
				      1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->volume = ptr->volume;
  new_ptr->amount = ptr->amount;

  new_ptr->lfo_enabled = ptr->lfo_enabled;
  new_ptr->lfo_freq = ptr->lfo_freq;

  new_ptr->frame_count = ptr->frame_count;
  new_ptr->offset = ptr->offset;

  return(new_ptr);
}

/**
 * ags_envelope_util_free:
 * @ptr: the #AgsEnvelopeUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_free(AgsEnvelopeUtil *ptr)
{
  g_return_if_fail(ptr != NULL);

  g_free(ptr->destination);

  if(ptr->destination != ptr->source){
    g_free(ptr->source);
  }

  g_free(ptr);
}

/**
 * ags_envelope_util_get_destination:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get destination buffer of @envelope_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 3.9.2
 */
gpointer
ags_envelope_util_get_destination(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(NULL);
  }

  return(envelope_util->destination);
}

/**
 * ags_envelope_util_set_destination:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @destination: the destination buffer
 *
 * Set @destination buffer of @envelope_util.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_set_destination(AgsEnvelopeUtil *envelope_util,
				  gpointer destination)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->destination = destination;
}

/**
 * ags_envelope_util_get_destination_stride:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get destination stride of @envelope_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 3.9.2
 */
guint
ags_envelope_util_get_destination_stride(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(0);
  }

  return(envelope_util->destination_stride);
}

/**
 * ags_envelope_util_set_destination_stride:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @envelope_util.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_set_destination_stride(AgsEnvelopeUtil *envelope_util,
					 guint destination_stride)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->destination_stride = destination_stride;
}

/**
 * ags_envelope_util_get_source:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get source buffer of @envelope_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.9.2
 */
gpointer
ags_envelope_util_get_source(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(NULL);
  }

  return(envelope_util->source);
}

/**
 * ags_envelope_util_set_source:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @envelope_util.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_set_source(AgsEnvelopeUtil *envelope_util,
			     gpointer source)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->source = source;
}

/**
 * ags_envelope_util_get_source_stride:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get source stride of @envelope_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.9.2
 */
guint
ags_envelope_util_get_source_stride(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(0);
  }

  return(envelope_util->source_stride);
}

/**
 * ags_envelope_util_set_source_stride:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @envelope_util.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_set_source_stride(AgsEnvelopeUtil *envelope_util,
				    guint source_stride)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->source_stride = source_stride;
}

/**
 * ags_envelope_util_get_buffer_length:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get buffer length of @envelope_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.9.2
 */
guint
ags_envelope_util_get_buffer_length(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(0);
  }

  return(envelope_util->buffer_length);
}

/**
 * ags_envelope_util_set_buffer_length:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @envelope_util.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_set_buffer_length(AgsEnvelopeUtil *envelope_util,
				    guint buffer_length)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->buffer_length = buffer_length;
}

/**
 * ags_envelope_util_get_format:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get format of @envelope_util.
 * 
 * Returns: the format
 * 
 * Since: 3.9.6
 */
guint
ags_envelope_util_get_format(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(0);
  }

  return(envelope_util->format);
}

/**
 * ags_envelope_util_set_format:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @format: the format
 *
 * Set @format of @envelope_util.
 *
 * Since: 3.9.6
 */
void
ags_envelope_util_set_format(AgsEnvelopeUtil *envelope_util,
			     guint format)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->format = format;
}

/**
 * ags_envelope_util_get_samplerate:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get samplerate of @envelope_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 5.2.0
 */
guint
ags_envelope_util_get_samplerate(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(0);
  }

  return(envelope_util->samplerate);
}

/**
 * ags_envelope_util_set_samplerate:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @envelope_util.
 *
 * Since: 5.2.0
 */
void
ags_envelope_util_set_samplerate(AgsEnvelopeUtil *envelope_util,
				 guint samplerate)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->samplerate = samplerate;
}

/**
 * ags_envelope_util_get_volume:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get volume of @envelope_util.
 * 
 * Returns: the volume
 * 
 * Since: 3.9.2
 */
gdouble
ags_envelope_util_get_volume(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(1.0);
  }

  return(envelope_util->volume);
}

/**
 * ags_envelope_util_set_volume:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @volume: the volume
 *
 * Set @volume of @envelope_util.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_set_volume(AgsEnvelopeUtil *envelope_util,
			     gdouble volume)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->volume = volume;
}

/**
 * ags_envelope_util_get_amount:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get amount of @envelope_util.
 * 
 * Returns: the amount
 * 
 * Since: 3.9.2
 */
gdouble
ags_envelope_util_get_amount(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(0.0);
  }

  return(envelope_util->amount);
}

/**
 * ags_envelope_util_set_amount:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @amount: the amount
 *
 * Set @amount of @envelope_util.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_set_amount(AgsEnvelopeUtil *envelope_util,
			     gdouble amount)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->amount = amount;
}

/**
 * ags_envelope_util_get_lfo_enabled:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get LFO enabled of @envelope_util.
 * 
 * Returns: the LFO enabled
 * 
 * Since: 7.0.0
 */
gboolean 
ags_envelope_util_get_lfo_enabled(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(FALSE);
  }

  return(envelope_util->lfo_enabled);
}

/**
 * ags_envelope_util_set_lfo_enabled:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @lfo_enabled: the LFO enabled
 *
 * Set @lfo_enabled of @envelope_util.
 *
 * Since: 7.0.0
 */
void
ags_envelope_util_set_lfo_enabled(AgsEnvelopeUtil *envelope_util,
				  gboolean lfo_enabled)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->lfo_enabled = lfo_enabled;
}

/**
 * ags_envelope_util_get_lfo_freq:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get LFO frequency of @envelope_util.
 * 
 * Returns: the LFO frequency
 * 
 * Since: 7.0.0
 */
gdouble
ags_envelope_util_get_lfo_freq(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(0.0);
  }

  return(envelope_util->lfo_freq);
}

/**
 * ags_envelope_util_set_lfo_freq:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @lfo_freq: the LFO frequency
 *
 * Set @lfo_freq of @envelope_util.
 *
 * Since: 7.0.0
 */
void
ags_envelope_util_set_lfo_freq(AgsEnvelopeUtil *envelope_util,
			       gdouble lfo_freq)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->lfo_freq = lfo_freq;
}

/**
 * ags_envelope_util_get_frame_count:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get frame count of @envelope_util.
 * 
 * Returns: the frame count
 * 
 * Since: 7.0.0
 */
guint
ags_envelope_util_get_frame_count(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(0);
  }

  return(envelope_util->frame_count);
}

/**
 * ags_envelope_util_set_frame_count:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @frame_count: the frame count
 *
 * Set @frame_count of @envelope_util.
 *
 * Since: 7.0.0
 */
void
ags_envelope_util_set_frame_count(AgsEnvelopeUtil *envelope_util,
				  guint frame_count)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->frame_count = frame_count;
}

/**
 * ags_envelope_util_get_offset:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get offset of @envelope_util.
 * 
 * Returns: the offset
 * 
 * Since: 7.0.0
 */
guint
ags_envelope_util_get_offset(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(0);
  }

  return(envelope_util->offset);
}

/**
 * ags_envelope_util_set_offset:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @offset: the offset
 *
 * Set @offset of @envelope_util.
 *
 * Since: 7.0.0
 */
void
ags_envelope_util_set_offset(AgsEnvelopeUtil *envelope_util,
			     guint offset)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->offset = offset;
}

/**
 * ags_envelope_util_compute_s8:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Compute envelope of signed 8 bit data.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_compute_s8(AgsEnvelopeUtil *envelope_util)
{
  gint8 *destination;
  gint8 *source;

  guint source_stride, destination_stride;
  guint samplerate;
  gdouble start_volume;
  gdouble amount;
  guint offset;
  gdouble lfo_freq;
  gboolean lfo_enabled;
  guint i, i_stop;

  destination = (gint8 *) envelope_util->destination;
  source = (gint8 *) envelope_util->source;
  
  source_stride = envelope_util->source_stride;
  destination_stride = envelope_util->destination_stride;

  samplerate = envelope_util->samplerate;

  start_volume = envelope_util->volume;
  amount = envelope_util->amount;

  lfo_enabled = envelope_util->lfo_enabled;
  lfo_freq = envelope_util->lfo_freq;

  offset = envelope_util->offset;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

#if defined(AGS_VECTOR_256_FUNCTIONS)
  AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
  while(i + 256 <= i_stop){
    AgsVectorArr *buffer_arr, *sine_arr, *volume_arr;

    guint j;
    
    while(!ags_vector_256_manager_try_acquire_triple(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &buffer_arr, &sine_arr, &volume_arr)){
      g_thread_yield();
    }

    ags_audio_buffer_util_fill_v8double_from_s8(NULL,
						buffer_arr->vector.vec_double->mem_double, 1,
						source, source_stride,
						32);
    
    if(lfo_enabled){
      for(j = 0; j < 32; j++){
	sine_arr->vector.vec_double->mem_double[j][0] = sin((gdouble) (offset + i + (j * 8)) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][1] = sin((gdouble) (offset + i + (j * 8) + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][2] = sin((gdouble) (offset + i + (j * 8) + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][3] = sin((gdouble) (offset + i + (j * 8) + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][4] = sin((gdouble) (offset + i + (j * 8) + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][5] = sin((gdouble) (offset + i + (j * 8) + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][6] = sin((gdouble) (offset + i + (j * 8) + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][7] = sin((gdouble) (offset + i + (j * 8) + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
      }
      
      for(j = 0; j < 32; j++){	
	buffer_arr->vector.vec_double->mem_double[j] *= sine_arr->vector.vec_double->mem_double[j];
      }
    }

    for(j = 0; j < 32; j++){
      volume_arr->vector.vec_double->mem_double[j][0] = (start_volume + (i + (j * 8)) * amount);
      volume_arr->vector.vec_double->mem_double[j][1] = (start_volume + (i + (j * 8) + 1) * amount);
      volume_arr->vector.vec_double->mem_double[j][2] = (start_volume + (i + (j * 8) + 2) * amount);
      volume_arr->vector.vec_double->mem_double[j][3] = (start_volume + (i + (j * 8) + 3) * amount);
      volume_arr->vector.vec_double->mem_double[j][4] = (start_volume + (i + (j * 8) + 4) * amount);
      volume_arr->vector.vec_double->mem_double[j][5] = (start_volume + (i + (j * 8) + 5) * amount);
      volume_arr->vector.vec_double->mem_double[j][6] = (start_volume + (i + (j * 8) + 6) * amount);
      volume_arr->vector.vec_double->mem_double[j][7] = (start_volume + (i + (j * 8) + 7) * amount);
    }
    
    for(j = 0; j < 32; j++){
      buffer_arr->vector.vec_double->mem_double[j] *= volume_arr->vector.vec_double->mem_double[j];
    }
    
    ags_audio_buffer_util_fetch_v8double_as_s8(NULL,
					       source, source_stride,
					       buffer_arr->vector.vec_double->mem_double, 1,
					       32);

    ags_vector_256_manager_release(vector_256_manager,
				   buffer_arr);
    ags_vector_256_manager_release(vector_256_manager,
				   sine_arr);
    ags_vector_256_manager_release(vector_256_manager,
				   volume_arr);

    source += (256 * source_stride);
    i += 256;
  }
#endif

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;
    ags_v8double v_sine;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride)
    };

    source += source_stride;

    v_volume = (ags_v8double) {
      (gdouble) (start_volume + (i) * amount),
      (gdouble) (start_volume + (i + 1) * amount),
      (gdouble) (start_volume + (i + 2) * amount),
      (gdouble) (start_volume + (i + 3) * amount),
      (gdouble) (start_volume + (i + 4) * amount),
      (gdouble) (start_volume + (i + 5) * amount),
      (gdouble) (start_volume + (i + 6) * amount),
      (gdouble) (start_volume + (i + 7) * amount),
    };

    if(lfo_enabled){
      v_sine = (ags_v8double) {
	sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),	
      };
    
      v_buffer *= v_sine;
    }
    
    v_buffer *= v_volume;

    *(destination) = (gint8) v_buffer[0];
    *(destination += destination_stride) = (gint8) v_buffer[1];
    *(destination += destination_stride) = (gint8) v_buffer[2];
    *(destination += destination_stride) = (gint8) v_buffer[3];
    *(destination += destination_stride) = (gint8) v_buffer[4];
    *(destination += destination_stride) = (gint8) v_buffer[5];
    *(destination += destination_stride) = (gint8) v_buffer[6];
    *(destination += destination_stride) = (gint8) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride)};

    double v_volume[] = {(double) start_volume + i * amount,
      (double) (start_volume + (i) * amount),
      (double) (start_volume + (i + 1) * amount),
      (double) (start_volume + (i + 2) * amount),
      (double) (start_volume + (i + 3) * amount),
      (double) (start_volume + (i + 4) * amount),
      (double) (start_volume + (i + 5) * amount),
      (double) (start_volume + (i + 6) * amount),
      (double) (start_volume + (i + 7) * amount)};
    
    source += source_stride;

    if(lfo_enabled){
      double ret_sine_buffer[8];
      
      double v_sine[] = {sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate)};
    
      vDSP_vmulD(v_buffer, 1, v_sine, 1, ret_sine_buffer, 1, 8);

      vDSP_vmulD(ret_sine_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);
    }else{
      vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);
    }
    
    *(destination) = (gint8) ret_v_buffer[0];
    *(destination += destination_stride) = (gint8) ret_v_buffer[1];
    *(destination += destination_stride) = (gint8) ret_v_buffer[2];
    *(destination += destination_stride) = (gint8) ret_v_buffer[3];
    *(destination += destination_stride) = (gint8) ret_v_buffer[4];
    *(destination += destination_stride) = (gint8) ret_v_buffer[5];
    *(destination += destination_stride) = (gint8) ret_v_buffer[6];
    *(destination += destination_stride) = (gint8) ret_v_buffer[7];

    destination += destination_stride;
    
    i += 8;
  }
#else
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    if(lfo_enabled){
      *(destination) = (gint8) ((gint16) ((source)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i) * amount)));
      *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 1) * amount)));
      *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 2) * amount)));
      *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 3) * amount)));
      *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 4) * amount)));
      *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 5) * amount)));
      *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 6) * amount)));
      *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 7) * amount)));
    }else{
      *(destination) = (gint8) ((gint16) ((source)[0] * (double) (start_volume + (i) * amount)));
      *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (start_volume + (i + 1) * amount)));
      *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (start_volume + (i + 2) * amount)));
      *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (start_volume + (i + 3) * amount)));
      *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (start_volume + (i + 4) * amount)));
      *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (start_volume + (i + 5) * amount)));
      *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (start_volume + (i + 6) * amount)));
      *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (start_volume + (i + 7) * amount)));
    }
    
    destination += destination_stride;
    source += source_stride;

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < envelope_util->buffer_length;){
    if(lfo_enabled){
      *(destination) = (gint8) ((gint16) ((source)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i) * amount)));
    }else{
      *(destination) = (gint8) ((gint16) ((source)[0] * (double) (start_volume + (i) * amount)));
    }
    
    destination += destination_stride;
    source += source_stride;
    i++;
  }

  envelope_util->volume = start_volume + i * amount;
}

/**
 * ags_envelope_util_compute_s16:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Compute envelope of signed 16 bit data.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_compute_s16(AgsEnvelopeUtil *envelope_util)
{
  gint16 *destination;
  gint16 *source;

  guint source_stride, destination_stride;
  guint samplerate;
  gdouble start_volume;
  gdouble amount;
  guint offset;
  gdouble lfo_freq;
  gboolean lfo_enabled;
  guint i, i_stop;

  destination = (gint16 *) envelope_util->destination;
  source = (gint16 *) envelope_util->source;
  
  source_stride = envelope_util->source_stride;
  destination_stride = envelope_util->destination_stride;

  samplerate = envelope_util->samplerate;

  start_volume = envelope_util->volume;
  amount = envelope_util->amount;

  lfo_enabled = envelope_util->lfo_enabled;
  lfo_freq = envelope_util->lfo_freq;

  offset = envelope_util->offset;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

#if defined(AGS_VECTOR_256_FUNCTIONS)
  AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
  while(i + 256 <= i_stop){
    AgsVectorArr *buffer_arr, *sine_arr, *volume_arr;

    guint j;
    
    while(!ags_vector_256_manager_try_acquire_triple(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &buffer_arr, &sine_arr, &volume_arr)){
      g_thread_yield();
    }

    ags_audio_buffer_util_fill_v8double_from_s16(NULL,
						 buffer_arr->vector.vec_double->mem_double, 1,
						 source, source_stride,
						 32);
    
    if(lfo_enabled){
      for(j = 0; j < 32; j++){
	sine_arr->vector.vec_double->mem_double[j][0] = sin((gdouble) (offset + i + (j * 8)) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][1] = sin((gdouble) (offset + i + (j * 8) + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][2] = sin((gdouble) (offset + i + (j * 8) + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][3] = sin((gdouble) (offset + i + (j * 8) + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][4] = sin((gdouble) (offset + i + (j * 8) + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][5] = sin((gdouble) (offset + i + (j * 8) + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][6] = sin((gdouble) (offset + i + (j * 8) + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][7] = sin((gdouble) (offset + i + (j * 8) + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
      }
      
      for(j = 0; j < 32; j++){	
	buffer_arr->vector.vec_double->mem_double[j] *= sine_arr->vector.vec_double->mem_double[j];
      }
    }

    for(j = 0; j < 32; j++){
      volume_arr->vector.vec_double->mem_double[j][0] = (start_volume + (i + (j * 8)) * amount);
      volume_arr->vector.vec_double->mem_double[j][1] = (start_volume + (i + (j * 8) + 1) * amount);
      volume_arr->vector.vec_double->mem_double[j][2] = (start_volume + (i + (j * 8) + 2) * amount);
      volume_arr->vector.vec_double->mem_double[j][3] = (start_volume + (i + (j * 8) + 3) * amount);
      volume_arr->vector.vec_double->mem_double[j][4] = (start_volume + (i + (j * 8) + 4) * amount);
      volume_arr->vector.vec_double->mem_double[j][5] = (start_volume + (i + (j * 8) + 5) * amount);
      volume_arr->vector.vec_double->mem_double[j][6] = (start_volume + (i + (j * 8) + 6) * amount);
      volume_arr->vector.vec_double->mem_double[j][7] = (start_volume + (i + (j * 8) + 7) * amount);
    }
    
    for(j = 0; j < 32; j++){
      buffer_arr->vector.vec_double->mem_double[j] *= volume_arr->vector.vec_double->mem_double[j];
    }
    
    ags_audio_buffer_util_fetch_v8double_as_s16(NULL,
						source, source_stride,
						buffer_arr->vector.vec_double->mem_double, 1,
						32);

    ags_vector_256_manager_release(vector_256_manager,
				   buffer_arr);
    ags_vector_256_manager_release(vector_256_manager,
				   sine_arr);
    ags_vector_256_manager_release(vector_256_manager,
				   volume_arr);

    source += (256 * source_stride);
    i += 256;
  }
#endif

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;
    ags_v8double v_sine;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride)
    };

    source += source_stride;

    v_volume = (ags_v8double) {
      (gdouble) (start_volume + (i) * amount),
      (gdouble) (start_volume + (i + 1) * amount),
      (gdouble) (start_volume + (i + 2) * amount),
      (gdouble) (start_volume + (i + 3) * amount),
      (gdouble) (start_volume + (i + 4) * amount),
      (gdouble) (start_volume + (i + 5) * amount),
      (gdouble) (start_volume + (i + 6) * amount),
      (gdouble) (start_volume + (i + 7) * amount)
    };

    if(lfo_enabled){
      v_sine = (ags_v8double) {
	sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),	
      };
    
      v_buffer *= v_sine;
    }

    v_buffer *= v_volume;

    *(destination) = (gint16) v_buffer[0];
    *(destination += destination_stride) = (gint16) v_buffer[1];
    *(destination += destination_stride) = (gint16) v_buffer[2];
    *(destination += destination_stride) = (gint16) v_buffer[3];
    *(destination += destination_stride) = (gint16) v_buffer[4];
    *(destination += destination_stride) = (gint16) v_buffer[5];
    *(destination += destination_stride) = (gint16) v_buffer[6];
    *(destination += destination_stride) = (gint16) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride)};

    double v_volume[] = {(double) start_volume + i * amount,
      (double) (start_volume + (i) * amount),
      (double) (start_volume + (i + 1) * amount),
      (double) (start_volume + (i + 2) * amount),
      (double) (start_volume + (i + 3) * amount),
      (double) (start_volume + (i + 4) * amount),
      (double) (start_volume + (i + 5) * amount),
      (double) (start_volume + (i + 6) * amount),
      (double) (start_volume + (i + 7) * amount)};

    source += source_stride;

    if(lfo_enabled){
      double ret_sine_buffer[8];
      
      double v_sine[] = {sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate)};
    
      vDSP_vmulD(v_buffer, 1, v_sine, 1, ret_sine_buffer, 1, 8);

      vDSP_vmulD(ret_sine_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);
    }else{
      vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);
    }

    *(destination) = (gint16) ret_v_buffer[0];
    *(destination += destination_stride) = (gint16) ret_v_buffer[1];
    *(destination += destination_stride) = (gint16) ret_v_buffer[2];
    *(destination += destination_stride) = (gint16) ret_v_buffer[3];
    *(destination += destination_stride) = (gint16) ret_v_buffer[4];
    *(destination += destination_stride) = (gint16) ret_v_buffer[5];
    *(destination += destination_stride) = (gint16) ret_v_buffer[6];
    *(destination += destination_stride) = (gint16) ret_v_buffer[7];

    destination += destination_stride;
    
    i += 8;
  }
#else
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    if(lfo_enabled){
      *(destination) = (gint16) ((gint32) ((source)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i) * amount)));
      *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 1) * amount)));
      *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 2) * amount)));
      *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 3) * amount)));
      *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 4) * amount)));
      *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 5) * amount)));
      *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 6) * amount)));
      *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 7) * amount)));
    }else{
      *(destination) = (gint16) ((gint32) ((source)[0] * (double) (start_volume + (i) * amount)));
      *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 1) * amount)));
      *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 2) * amount)));
      *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 3) * amount)));
      *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 4) * amount)));
      *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 5) * amount)));
      *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 6) * amount)));
      *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 7) * amount)));
    }

    destination += destination_stride;
    source += source_stride;

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < envelope_util->buffer_length;){
    if(lfo_enabled){
      *(destination) = (gint16) ((gint32) ((source)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i) * amount)));
    }else{
      *(destination) = (gint16) ((gint32) ((source)[0] * (double) (start_volume + (i) * amount)));
    }
    
    destination += destination_stride;
    source += source_stride;
    i++;
  }

  envelope_util->volume = start_volume + i * amount;
}

/**
 * ags_envelope_util_compute_s24:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Compute envelope of signed 24 bit data.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_compute_s24(AgsEnvelopeUtil *envelope_util)
{
  gint32 *destination;
  gint32 *source;

  guint source_stride, destination_stride;
  guint samplerate;
  gdouble start_volume;
  gdouble amount;
  guint offset;
  gdouble lfo_freq;
  gboolean lfo_enabled;
  guint i, i_stop;

  destination = (gint32 *) envelope_util->destination;
  source = (gint32 *) envelope_util->source;
  
  source_stride = envelope_util->source_stride;
  destination_stride = envelope_util->destination_stride;

  samplerate = envelope_util->samplerate;

  start_volume = envelope_util->volume;
  amount = envelope_util->amount;

  lfo_enabled = envelope_util->lfo_enabled;
  lfo_freq = envelope_util->lfo_freq;

  offset = envelope_util->offset;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

#if defined(AGS_VECTOR_256_FUNCTIONS)
  AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
  while(i + 256 <= i_stop){
    AgsVectorArr *buffer_arr, *sine_arr, *volume_arr;

    guint j;
    
    while(!ags_vector_256_manager_try_acquire_triple(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &buffer_arr, &sine_arr, &volume_arr)){
      g_thread_yield();
    }

    ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						 buffer_arr->vector.vec_double->mem_double, 1,
						 source, source_stride,
						 32);
    
    if(lfo_enabled){
      for(j = 0; j < 32; j++){
	sine_arr->vector.vec_double->mem_double[j][0] = sin((gdouble) (offset + i + (j * 8)) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][1] = sin((gdouble) (offset + i + (j * 8) + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][2] = sin((gdouble) (offset + i + (j * 8) + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][3] = sin((gdouble) (offset + i + (j * 8) + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][4] = sin((gdouble) (offset + i + (j * 8) + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][5] = sin((gdouble) (offset + i + (j * 8) + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][6] = sin((gdouble) (offset + i + (j * 8) + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][7] = sin((gdouble) (offset + i + (j * 8) + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
      }
      
      for(j = 0; j < 32; j++){	
	buffer_arr->vector.vec_double->mem_double[j] *= sine_arr->vector.vec_double->mem_double[j];
      }
    }

    for(j = 0; j < 32; j++){
      volume_arr->vector.vec_double->mem_double[j][0] = (start_volume + (i + (j * 8)) * amount);
      volume_arr->vector.vec_double->mem_double[j][1] = (start_volume + (i + (j * 8) + 1) * amount);
      volume_arr->vector.vec_double->mem_double[j][2] = (start_volume + (i + (j * 8) + 2) * amount);
      volume_arr->vector.vec_double->mem_double[j][3] = (start_volume + (i + (j * 8) + 3) * amount);
      volume_arr->vector.vec_double->mem_double[j][4] = (start_volume + (i + (j * 8) + 4) * amount);
      volume_arr->vector.vec_double->mem_double[j][5] = (start_volume + (i + (j * 8) + 5) * amount);
      volume_arr->vector.vec_double->mem_double[j][6] = (start_volume + (i + (j * 8) + 6) * amount);
      volume_arr->vector.vec_double->mem_double[j][7] = (start_volume + (i + (j * 8) + 7) * amount);
    }
    
    for(j = 0; j < 32; j++){
      buffer_arr->vector.vec_double->mem_double[j] *= volume_arr->vector.vec_double->mem_double[j];
    }
    
    ags_audio_buffer_util_fetch_v8double_as_s32(NULL,
						source, source_stride,
						buffer_arr->vector.vec_double->mem_double, 1,
						32);

    ags_vector_256_manager_release(vector_256_manager,
				   buffer_arr);
    ags_vector_256_manager_release(vector_256_manager,
				   sine_arr);
    ags_vector_256_manager_release(vector_256_manager,
				   volume_arr);

    source += (256 * source_stride);
    i += 256;
  }
#endif

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;
    ags_v8double v_sine;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride)
    };

    source += source_stride;

    v_volume = (ags_v8double) {
      (gdouble) (start_volume + (i) * amount),
      (gdouble) (start_volume + (i + 1) * amount),
      (gdouble) (start_volume + (i + 2) * amount),
      (gdouble) (start_volume + (i + 3) * amount),
      (gdouble) (start_volume + (i + 4) * amount),
      (gdouble) (start_volume + (i + 5) * amount),
      (gdouble) (start_volume + (i + 6) * amount),
      (gdouble) (start_volume + (i + 7) * amount)
    };

    if(lfo_enabled){
      v_sine = (ags_v8double) {
	sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),	
      };
    
      v_buffer *= v_sine;
    }

    v_buffer *= v_volume;

    *(destination) = (gint32) v_buffer[0];
    *(destination += destination_stride) = (gint32) v_buffer[1];
    *(destination += destination_stride) = (gint32) v_buffer[2];
    *(destination += destination_stride) = (gint32) v_buffer[3];
    *(destination += destination_stride) = (gint32) v_buffer[4];
    *(destination += destination_stride) = (gint32) v_buffer[5];
    *(destination += destination_stride) = (gint32) v_buffer[6];
    *(destination += destination_stride) = (gint32) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride)};

    double v_volume[] = {(double) start_volume + i * amount,
      (double) (start_volume + (i) * amount),
      (double) (start_volume + (i + 1) * amount),
      (double) (start_volume + (i + 2) * amount),
      (double) (start_volume + (i + 3) * amount),
      (double) (start_volume + (i + 4) * amount),
      (double) (start_volume + (i + 5) * amount),
      (double) (start_volume + (i + 6) * amount),
      (double) (start_volume + (i + 7) * amount)};

    source += source_stride;

    if(lfo_enabled){
      double ret_sine_buffer[8];
      
      double v_sine[] = {sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate)};
    
      vDSP_vmulD(v_buffer, 1, v_sine, 1, ret_sine_buffer, 1, 8);

      vDSP_vmulD(ret_sine_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);
    }else{
      vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);
    }

    *(destination) = (gint32) ret_v_buffer[0];
    *(destination += destination_stride) = (gint32) ret_v_buffer[1];
    *(destination += destination_stride) = (gint32) ret_v_buffer[2];
    *(destination += destination_stride) = (gint32) ret_v_buffer[3];
    *(destination += destination_stride) = (gint32) ret_v_buffer[4];
    *(destination += destination_stride) = (gint32) ret_v_buffer[5];
    *(destination += destination_stride) = (gint32) ret_v_buffer[6];
    *(destination += destination_stride) = (gint32) ret_v_buffer[7];

    destination += destination_stride;

    i += 8;
  }
#else
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    if(lfo_enabled){
      *(destination) = (gint32) ((gint32) ((source)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i) * amount)));
      *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 1) * amount)));
      *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 2) * amount)));
      *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 3) * amount)));
      *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 4) * amount)));
      *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 5) * amount)));
      *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 6) * amount)));
      *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 7) * amount)));
    }else{
      *(destination) = (gint32) ((gint32) ((source)[0] * (double) (start_volume + (i) * amount)));
      *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 1) * amount)));
      *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 2) * amount)));
      *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 3) * amount)));
      *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 4) * amount)));
      *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 5) * amount)));
      *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 6) * amount)));
      *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 7) * amount)));
    }

    destination += destination_stride;
    source += source_stride;

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < envelope_util->buffer_length;){
    if(lfo_enabled){
      destination[0] = (gint32) ((gint32) (source[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (start_volume + i * amount)));
    }else{
      destination[0] = (gint32) ((gint32) (source[0] * (start_volume + i * amount)));
    }
    
    destination += destination_stride;
    source += source_stride;
    i++;
  }

  envelope_util->volume = start_volume + i * amount;
}

/**
 * ags_envelope_util_compute_s32:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Compute envelope of signed 32 bit data.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_compute_s32(AgsEnvelopeUtil *envelope_util)
{
  gint32 *destination;
  gint32 *source;

  guint source_stride, destination_stride;
  guint samplerate;
  gdouble start_volume;
  gdouble amount;
  guint offset;
  gdouble lfo_freq;
  gboolean lfo_enabled;
  guint i, i_stop;

  destination = (gint32 *) envelope_util->destination;
  source = (gint32 *) envelope_util->source;
  
  source_stride = envelope_util->source_stride;
  destination_stride = envelope_util->destination_stride;

  samplerate = envelope_util->samplerate;

  start_volume = envelope_util->volume;
  amount = envelope_util->amount;

  lfo_enabled = envelope_util->lfo_enabled;
  lfo_freq = envelope_util->lfo_freq;

  offset = envelope_util->offset;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

#if defined(AGS_VECTOR_256_FUNCTIONS)
  AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
  while(i + 256 <= i_stop){
    AgsVectorArr *buffer_arr, *sine_arr, *volume_arr;

    guint j;
    
    while(!ags_vector_256_manager_try_acquire_triple(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &buffer_arr, &sine_arr, &volume_arr)){
      g_thread_yield();
    }

    ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						 buffer_arr->vector.vec_double->mem_double, 1,
						 source, source_stride,
						 32);
    
    if(lfo_enabled){
      for(j = 0; j < 32; j++){
	sine_arr->vector.vec_double->mem_double[j][0] = sin((gdouble) (offset + i + (j * 8)) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][1] = sin((gdouble) (offset + i + (j * 8) + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][2] = sin((gdouble) (offset + i + (j * 8) + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][3] = sin((gdouble) (offset + i + (j * 8) + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][4] = sin((gdouble) (offset + i + (j * 8) + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][5] = sin((gdouble) (offset + i + (j * 8) + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][6] = sin((gdouble) (offset + i + (j * 8) + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][7] = sin((gdouble) (offset + i + (j * 8) + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
      }
      
      for(j = 0; j < 32; j++){	
	buffer_arr->vector.vec_double->mem_double[j] *= sine_arr->vector.vec_double->mem_double[j];
      }
    }

    for(j = 0; j < 32; j++){
      volume_arr->vector.vec_double->mem_double[j][0] = (start_volume + (i + (j * 8)) * amount);
      volume_arr->vector.vec_double->mem_double[j][1] = (start_volume + (i + (j * 8) + 1) * amount);
      volume_arr->vector.vec_double->mem_double[j][2] = (start_volume + (i + (j * 8) + 2) * amount);
      volume_arr->vector.vec_double->mem_double[j][3] = (start_volume + (i + (j * 8) + 3) * amount);
      volume_arr->vector.vec_double->mem_double[j][4] = (start_volume + (i + (j * 8) + 4) * amount);
      volume_arr->vector.vec_double->mem_double[j][5] = (start_volume + (i + (j * 8) + 5) * amount);
      volume_arr->vector.vec_double->mem_double[j][6] = (start_volume + (i + (j * 8) + 6) * amount);
      volume_arr->vector.vec_double->mem_double[j][7] = (start_volume + (i + (j * 8) + 7) * amount);
    }
    
    for(j = 0; j < 32; j++){
      buffer_arr->vector.vec_double->mem_double[j] *= volume_arr->vector.vec_double->mem_double[j];
    }
    
    ags_audio_buffer_util_fetch_v8double_as_s32(NULL,
						source, source_stride,
						buffer_arr->vector.vec_double->mem_double, 1,
						32);

    ags_vector_256_manager_release(vector_256_manager,
				   buffer_arr);
    ags_vector_256_manager_release(vector_256_manager,
				   sine_arr);
    ags_vector_256_manager_release(vector_256_manager,
				   volume_arr);

    source += (256 * source_stride);
    i += 256;
  }
#endif

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;
    ags_v8double v_sine;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride)
    };

    source += source_stride;

    v_volume = (ags_v8double) {
      (gdouble) (start_volume + (i) * amount),
      (gdouble) (start_volume + (i + 1) * amount),
      (gdouble) (start_volume + (i + 2) * amount),
      (gdouble) (start_volume + (i + 3) * amount),
      (gdouble) (start_volume + (i + 4) * amount),
      (gdouble) (start_volume + (i + 5) * amount),
      (gdouble) (start_volume + (i + 6) * amount),
      (gdouble) (start_volume + (i + 7) * amount)
    };

    if(lfo_enabled){
      v_sine = (ags_v8double) {
	sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),	
      };
    
      v_buffer *= v_sine;
    }

    v_buffer *= v_volume;

    *(destination) = (gint32) v_buffer[0];
    *(destination += destination_stride) = (gint32) v_buffer[1];
    *(destination += destination_stride) = (gint32) v_buffer[2];
    *(destination += destination_stride) = (gint32) v_buffer[3];
    *(destination += destination_stride) = (gint32) v_buffer[4];
    *(destination += destination_stride) = (gint32) v_buffer[5];
    *(destination += destination_stride) = (gint32) v_buffer[6];
    *(destination += destination_stride) = (gint32) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride)};

    double v_volume[] = {(double) start_volume + i * amount,
      (double) (start_volume + (i) * amount),
      (double) (start_volume + (i + 1) * amount),
      (double) (start_volume + (i + 2) * amount),
      (double) (start_volume + (i + 3) * amount),
      (double) (start_volume + (i + 4) * amount),
      (double) (start_volume + (i + 5) * amount),
      (double) (start_volume + (i + 6) * amount),
      (double) (start_volume + (i + 7) * amount)};

    source += source_stride;

    if(lfo_enabled){
      double ret_sine_buffer[8];
      
      double v_sine[] = {sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate)};
    
      vDSP_vmulD(v_buffer, 1, v_sine, 1, ret_sine_buffer, 1, 8);

      vDSP_vmulD(ret_sine_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);
    }else{
      vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);
    }

    *(destination) = (gint32) ret_v_buffer[0];
    *(destination += destination_stride) = (gint32) ret_v_buffer[1];
    *(destination += destination_stride) = (gint32) ret_v_buffer[2];
    *(destination += destination_stride) = (gint32) ret_v_buffer[3];
    *(destination += destination_stride) = (gint32) ret_v_buffer[4];
    *(destination += destination_stride) = (gint32) ret_v_buffer[5];
    *(destination += destination_stride) = (gint32) ret_v_buffer[6];
    *(destination += destination_stride) = (gint32) ret_v_buffer[7];

    destination += destination_stride;
    
    i += 8;
  }
#else
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    if(lfo_enabled){
      *(destination) = (gint32) ((gint64) ((source)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i) * amount)));
      *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 1) * amount)));
      *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 2) * amount)));
      *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 3) * amount)));
      *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 4) * amount)));
      *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 5) * amount)));
      *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 6) * amount)));
      *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 7) * amount)));
    }else{
      *(destination) = (gint32) ((gint64) ((source)[0] * (double) (start_volume + (i) * amount)));
      *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 1) * amount)));
      *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 2) * amount)));
      *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 3) * amount)));
      *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 4) * amount)));
      *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 5) * amount)));
      *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 6) * amount)));
      *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 7) * amount)));
    }
    
    destination += destination_stride;
    source += source_stride;

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < envelope_util->buffer_length;){
    if(lfo_enabled){
      *(destination) = (gint32) ((gint64) ((source)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i) * amount)));
    }else{
      *(destination) = (gint32) ((gint64) ((source)[0] * (double) (start_volume + (i) * amount)));
    }
    
    destination += destination_stride;
    source += source_stride;
    i++;
  }

  envelope_util->volume = start_volume + i * amount;
}

/**
 * ags_envelope_util_compute_s64:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Compute envelope of signed 64 bit data.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_compute_s64(AgsEnvelopeUtil *envelope_util)
{
  gint64 *destination;
  gint64 *source;

  guint source_stride, destination_stride;
  guint samplerate;
  gdouble start_volume;
  gdouble amount;
  guint offset;
  gdouble lfo_freq;
  gboolean lfo_enabled;
  guint i, i_stop;

  destination = (gint64 *) envelope_util->destination;
  source = (gint64 *) envelope_util->source;
  
  source_stride = envelope_util->source_stride;
  destination_stride = envelope_util->destination_stride;

  samplerate = envelope_util->samplerate;

  start_volume = envelope_util->volume;
  amount = envelope_util->amount;

  lfo_enabled = envelope_util->lfo_enabled;
  lfo_freq = envelope_util->lfo_freq;

  offset = envelope_util->offset;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

#if defined(AGS_VECTOR_256_FUNCTIONS)
  AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
  while(i + 256 <= i_stop){
    AgsVectorArr *buffer_arr, *sine_arr, *volume_arr;

    guint j;
    
    while(!ags_vector_256_manager_try_acquire_triple(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &buffer_arr, &sine_arr, &volume_arr)){
      g_thread_yield();
    }

    ags_audio_buffer_util_fill_v8double_from_s64(NULL,
						 buffer_arr->vector.vec_double->mem_double, 1,
						 source, source_stride,
						 32);
    
    if(lfo_enabled){
      for(j = 0; j < 32; j++){
	sine_arr->vector.vec_double->mem_double[j][0] = sin((gdouble) (offset + i + (j * 8)) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][1] = sin((gdouble) (offset + i + (j * 8) + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][2] = sin((gdouble) (offset + i + (j * 8) + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][3] = sin((gdouble) (offset + i + (j * 8) + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][4] = sin((gdouble) (offset + i + (j * 8) + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][5] = sin((gdouble) (offset + i + (j * 8) + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][6] = sin((gdouble) (offset + i + (j * 8) + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][7] = sin((gdouble) (offset + i + (j * 8) + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
      }
      
      for(j = 0; j < 32; j++){	
	buffer_arr->vector.vec_double->mem_double[j] *= sine_arr->vector.vec_double->mem_double[j];
      }
    }

    for(j = 0; j < 32; j++){
      volume_arr->vector.vec_double->mem_double[j][0] = (start_volume + (i + (j * 8)) * amount);
      volume_arr->vector.vec_double->mem_double[j][1] = (start_volume + (i + (j * 8) + 1) * amount);
      volume_arr->vector.vec_double->mem_double[j][2] = (start_volume + (i + (j * 8) + 2) * amount);
      volume_arr->vector.vec_double->mem_double[j][3] = (start_volume + (i + (j * 8) + 3) * amount);
      volume_arr->vector.vec_double->mem_double[j][4] = (start_volume + (i + (j * 8) + 4) * amount);
      volume_arr->vector.vec_double->mem_double[j][5] = (start_volume + (i + (j * 8) + 5) * amount);
      volume_arr->vector.vec_double->mem_double[j][6] = (start_volume + (i + (j * 8) + 6) * amount);
      volume_arr->vector.vec_double->mem_double[j][7] = (start_volume + (i + (j * 8) + 7) * amount);
    }
    
    for(j = 0; j < 32; j++){
      buffer_arr->vector.vec_double->mem_double[j] *= volume_arr->vector.vec_double->mem_double[j];
    }
    
    ags_audio_buffer_util_fetch_v8double_as_s64(NULL,
						source, source_stride,
						buffer_arr->vector.vec_double->mem_double, 1,
						32);

    ags_vector_256_manager_release(vector_256_manager,
				   buffer_arr);
    ags_vector_256_manager_release(vector_256_manager,
				   sine_arr);
    ags_vector_256_manager_release(vector_256_manager,
				   volume_arr);

    source += (256 * source_stride);
    i += 256;
  }
#endif

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;
    ags_v8double v_sine;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride)
    };

    source += source_stride;

    v_volume = (ags_v8double) {
      (gdouble) (start_volume + (i) * amount),
      (gdouble) (start_volume + (i + 1) * amount),
      (gdouble) (start_volume + (i + 2) * amount),
      (gdouble) (start_volume + (i + 3) * amount),
      (gdouble) (start_volume + (i + 4) * amount),
      (gdouble) (start_volume + (i + 5) * amount),
      (gdouble) (start_volume + (i + 6) * amount),
      (gdouble) (start_volume + (i + 7) * amount)
    };

    if(lfo_enabled){
      v_sine = (ags_v8double) {
	sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),	
      };
    
      v_buffer *= v_sine;
    }

    v_buffer *= v_volume;

    *(destination) = (gint64) v_buffer[0];
    *(destination += destination_stride) = (gint64) v_buffer[1];
    *(destination += destination_stride) = (gint64) v_buffer[2];
    *(destination += destination_stride) = (gint64) v_buffer[3];
    *(destination += destination_stride) = (gint64) v_buffer[4];
    *(destination += destination_stride) = (gint64) v_buffer[5];
    *(destination += destination_stride) = (gint64) v_buffer[6];
    *(destination += destination_stride) = (gint64) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride)};

    double v_volume[] = {(double) start_volume + i * amount,
      (double) (start_volume + (i) * amount),
      (double) (start_volume + (i + 1) * amount),
      (double) (start_volume + (i + 2) * amount),
      (double) (start_volume + (i + 3) * amount),
      (double) (start_volume + (i + 4) * amount),
      (double) (start_volume + (i + 5) * amount),
      (double) (start_volume + (i + 6) * amount),
      (double) (start_volume + (i + 7) * amount)};

    source += source_stride;

    if(lfo_enabled){
      double ret_sine_buffer[8];
      
      double v_sine[] = {sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate)};
    
      vDSP_vmulD(v_buffer, 1, v_sine, 1, ret_sine_buffer, 1, 8);

      vDSP_vmulD(ret_sine_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);
    }else{
      vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);
    }

    *(destination) = (gint64) ret_v_buffer[0];
    *(destination += destination_stride) = (gint64) ret_v_buffer[1];
    *(destination += destination_stride) = (gint64) ret_v_buffer[2];
    *(destination += destination_stride) = (gint64) ret_v_buffer[3];
    *(destination += destination_stride) = (gint64) ret_v_buffer[4];
    *(destination += destination_stride) = (gint64) ret_v_buffer[5];
    *(destination += destination_stride) = (gint64) ret_v_buffer[6];
    *(destination += destination_stride) = (gint64) ret_v_buffer[7];

    destination += destination_stride;
    
    i += 8;
  }
#else
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    if(lfo_enabled){
      *(destination) = (gint64) ((gint64) ((source)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i) * amount)));
      *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 1) * amount)));
      *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 2) * amount)));
      *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 3) * amount)));
      *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 4) * amount)));
      *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 5) * amount)));
      *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 6) * amount)));
      *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 7) * amount)));
    }else{
      *(destination) = (gint64) ((gint64) ((source)[0] * (double) (start_volume + (i) * amount)));
      *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 1) * amount)));
      *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 2) * amount)));
      *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 3) * amount)));
      *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 4) * amount)));
      *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 5) * amount)));
      *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 6) * amount)));
      *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 7) * amount)));
    }
    
    destination += destination_stride;
    source += source_stride;

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < envelope_util->buffer_length;){
    if(lfo_enabled){
      *(destination) = (gint64) ((gint64) ((source)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i) * amount)));
    }else{
      *(destination) = (gint64) ((gint64) ((source)[0] * (double) (start_volume + (i) * amount)));
    }

    destination += destination_stride;
    source += source_stride;
    i++;
  }

  envelope_util->volume = start_volume + i * amount;
}

/**
 * ags_envelope_util_compute_float:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Compute envelope of floating point data.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_compute_float(AgsEnvelopeUtil *envelope_util)
{
  gfloat *destination;
  gfloat *source;

  guint source_stride, destination_stride;
  guint samplerate;
  gdouble start_volume;
  gdouble amount;
  guint offset;
  gdouble lfo_freq;
  gboolean lfo_enabled;
  guint i, i_stop;

  destination = (gfloat *) envelope_util->destination;
  source = (gfloat *) envelope_util->source;
  
  source_stride = envelope_util->source_stride;
  destination_stride = envelope_util->destination_stride;

  samplerate = envelope_util->samplerate;

  start_volume = envelope_util->volume;
  amount = envelope_util->amount;

  lfo_enabled = envelope_util->lfo_enabled;
  lfo_freq = envelope_util->lfo_freq;

  offset = envelope_util->offset;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;
    ags_v8double v_sine;

#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= i_stop){
      AgsVectorArr *buffer_arr, *sine_arr, *volume_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_triple(vector_256_manager,
						       AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						       &buffer_arr, &sine_arr, &volume_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_float(NULL,
						     buffer_arr->vector.vec_double->mem_double, 1,
						     source, source_stride,
						     32);
    
      if(lfo_enabled){
	for(j = 0; j < 32; j++){
	  sine_arr->vector.vec_double->mem_double[j][0] = sin((gdouble) (offset + i + (j * 8)) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	  sine_arr->vector.vec_double->mem_double[j][1] = sin((gdouble) (offset + i + (j * 8) + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	  sine_arr->vector.vec_double->mem_double[j][2] = sin((gdouble) (offset + i + (j * 8) + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	  sine_arr->vector.vec_double->mem_double[j][3] = sin((gdouble) (offset + i + (j * 8) + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	  sine_arr->vector.vec_double->mem_double[j][4] = sin((gdouble) (offset + i + (j * 8) + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	  sine_arr->vector.vec_double->mem_double[j][5] = sin((gdouble) (offset + i + (j * 8) + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	  sine_arr->vector.vec_double->mem_double[j][6] = sin((gdouble) (offset + i + (j * 8) + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	  sine_arr->vector.vec_double->mem_double[j][7] = sin((gdouble) (offset + i + (j * 8) + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	}
      
	for(j = 0; j < 32; j++){	
	  buffer_arr->vector.vec_double->mem_double[j] *= sine_arr->vector.vec_double->mem_double[j];
	}
      }

      for(j = 0; j < 32; j++){
	volume_arr->vector.vec_double->mem_double[j][0] = (start_volume + (i + (j * 8)) * amount);
	volume_arr->vector.vec_double->mem_double[j][1] = (start_volume + (i + (j * 8) + 1) * amount);
	volume_arr->vector.vec_double->mem_double[j][2] = (start_volume + (i + (j * 8) + 2) * amount);
	volume_arr->vector.vec_double->mem_double[j][3] = (start_volume + (i + (j * 8) + 3) * amount);
	volume_arr->vector.vec_double->mem_double[j][4] = (start_volume + (i + (j * 8) + 4) * amount);
	volume_arr->vector.vec_double->mem_double[j][5] = (start_volume + (i + (j * 8) + 5) * amount);
	volume_arr->vector.vec_double->mem_double[j][6] = (start_volume + (i + (j * 8) + 6) * amount);
	volume_arr->vector.vec_double->mem_double[j][7] = (start_volume + (i + (j * 8) + 7) * amount);
      }
    
      for(j = 0; j < 32; j++){
	buffer_arr->vector.vec_double->mem_double[j] *= volume_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_float(NULL,
						    source, source_stride,
						    buffer_arr->vector.vec_double->mem_double, 1,
						    32);

      ags_vector_256_manager_release(vector_256_manager,
				     buffer_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     sine_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     volume_arr);

      source += (256 * source_stride);
      i += 256;
    }
#endif

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride)
    };

    source += source_stride;

    v_volume = (ags_v8double) {
      (gdouble) (start_volume + (i) * amount),
      (gdouble) (start_volume + (i + 1) * amount),
      (gdouble) (start_volume + (i + 2) * amount),
      (gdouble) (start_volume + (i + 3) * amount),
      (gdouble) (start_volume + (i + 4) * amount),
      (gdouble) (start_volume + (i + 5) * amount),
      (gdouble) (start_volume + (i + 6) * amount),
      (gdouble) (start_volume + (i + 7) * amount)
    };

    if(lfo_enabled){
      v_sine = (ags_v8double) {
	sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),	
      };
    
      v_buffer *= v_sine;
    }

    v_buffer *= v_volume;

    *(destination) = (gfloat) v_buffer[0];
    *(destination += destination_stride) = (gfloat) v_buffer[1];
    *(destination += destination_stride) = (gfloat) v_buffer[2];
    *(destination += destination_stride) = (gfloat) v_buffer[3];
    *(destination += destination_stride) = (gfloat) v_buffer[4];
    *(destination += destination_stride) = (gfloat) v_buffer[5];
    *(destination += destination_stride) = (gfloat) v_buffer[6];
    *(destination += destination_stride) = (gfloat) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride)};

    double v_volume[] = {(double) start_volume + i * amount,
			 (double) (start_volume + (i) * amount),
			 (double) (start_volume + (i + 1) * amount),
			 (double) (start_volume + (i + 2) * amount),
			 (double) (start_volume + (i + 3) * amount),
			 (double) (start_volume + (i + 4) * amount),
			 (double) (start_volume + (i + 5) * amount),
			 (double) (start_volume + (i + 6) * amount),
			 (double) (start_volume + (i + 7) * amount)};

    source += source_stride;

    if(lfo_enabled){
      double ret_sine_buffer[8];
      
      double v_sine[] = {sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
			 sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
			 sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
			 sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
			 sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
			 sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
			 sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
			 sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate)};
    
      vDSP_vmulD(v_buffer, 1, v_sine, 1, ret_sine_buffer, 1, 8);

      vDSP_vmulD(ret_sine_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);
    }else{
      vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);
    }

    *(destination) = (gfloat) ret_v_buffer[0];
    *(destination += destination_stride) = (gfloat) ret_v_buffer[1];
    *(destination += destination_stride) = (gfloat) ret_v_buffer[2];
    *(destination += destination_stride) = (gfloat) ret_v_buffer[3];
    *(destination += destination_stride) = (gfloat) ret_v_buffer[4];
    *(destination += destination_stride) = (gfloat) ret_v_buffer[5];
    *(destination += destination_stride) = (gfloat) ret_v_buffer[6];
    *(destination += destination_stride) = (gfloat) ret_v_buffer[7];

    destination += destination_stride;
    
    i += 8;
  }
#else
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    if(lfo_enabled){
      *(destination) = (gfloat) ((source)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i) * amount));
      *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 1) * amount));
      *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 2) * amount));
      *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 3) * amount));
      *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 4) * amount));
      *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 5) * amount));
      *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 6) * amount));
      *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 7) * amount));
    }else{
      *(destination) = (gfloat) ((source)[0] * (double) (start_volume + (i) * amount));
      *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (start_volume + (i + 1) * amount));
      *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (start_volume + (i + 2) * amount));
      *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (start_volume + (i + 3) * amount));
      *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (start_volume + (i + 4) * amount));
      *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (start_volume + (i + 5) * amount));
      *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (start_volume + (i + 6) * amount));
      *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (start_volume + (i + 7) * amount));
    }
    
    destination += destination_stride;
    source += source_stride;

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < envelope_util->buffer_length;){
    if(lfo_enabled){
      *(destination) = (gfloat) ((source)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i) * amount));
    }else{
      *(destination) = (gfloat) ((source)[0] * (double) (start_volume + (i) * amount));
    }

    destination += destination_stride;
    source += source_stride;
    i++;
  }

  envelope_util->volume = start_volume + i * amount;
}

/**
 * ags_envelope_util_compute_double:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Compute envelope of double floating point data.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_compute_double(AgsEnvelopeUtil *envelope_util)
{
  gdouble *destination;
  gdouble *source;

  guint source_stride, destination_stride;
  guint samplerate;
  gdouble start_volume;
  gdouble amount;
  guint offset;
  gdouble lfo_freq;
  gboolean lfo_enabled;
  guint i, i_stop;

  destination = (gdouble *) envelope_util->destination;
  source = (gdouble *) envelope_util->source;
  
  source_stride = envelope_util->source_stride;
  destination_stride = envelope_util->destination_stride;

  samplerate = envelope_util->samplerate;

  start_volume = envelope_util->volume;
  amount = envelope_util->amount;

  lfo_enabled = envelope_util->lfo_enabled;
  lfo_freq = envelope_util->lfo_freq;

  offset = envelope_util->offset;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

#if defined(AGS_VECTOR_256_FUNCTIONS)
  AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
  while(i + 256 <= i_stop){
    AgsVectorArr *buffer_arr, *sine_arr, *volume_arr;

    guint j;
    
    while(!ags_vector_256_manager_try_acquire_triple(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &buffer_arr, &sine_arr, &volume_arr)){
      g_thread_yield();
    }

    ags_audio_buffer_util_fill_v8double(NULL,
					buffer_arr->vector.vec_double->mem_double, 1,
					source, source_stride,
					32);
    
    if(lfo_enabled){
      for(j = 0; j < 32; j++){
	sine_arr->vector.vec_double->mem_double[j][0] = sin((gdouble) (offset + i + (j * 8)) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][1] = sin((gdouble) (offset + i + (j * 8) + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][2] = sin((gdouble) (offset + i + (j * 8) + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][3] = sin((gdouble) (offset + i + (j * 8) + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][4] = sin((gdouble) (offset + i + (j * 8) + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][5] = sin((gdouble) (offset + i + (j * 8) + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][6] = sin((gdouble) (offset + i + (j * 8) + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
	sine_arr->vector.vec_double->mem_double[j][7] = sin((gdouble) (offset + i + (j * 8) + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate);
      }
      
      for(j = 0; j < 32; j++){	
	buffer_arr->vector.vec_double->mem_double[j] *= sine_arr->vector.vec_double->mem_double[j];
      }
    }

    for(j = 0; j < 32; j++){
      volume_arr->vector.vec_double->mem_double[j][0] = (start_volume + (i + (j * 8)) * amount);
      volume_arr->vector.vec_double->mem_double[j][1] = (start_volume + (i + (j * 8) + 1) * amount);
      volume_arr->vector.vec_double->mem_double[j][2] = (start_volume + (i + (j * 8) + 2) * amount);
      volume_arr->vector.vec_double->mem_double[j][3] = (start_volume + (i + (j * 8) + 3) * amount);
      volume_arr->vector.vec_double->mem_double[j][4] = (start_volume + (i + (j * 8) + 4) * amount);
      volume_arr->vector.vec_double->mem_double[j][5] = (start_volume + (i + (j * 8) + 5) * amount);
      volume_arr->vector.vec_double->mem_double[j][6] = (start_volume + (i + (j * 8) + 6) * amount);
      volume_arr->vector.vec_double->mem_double[j][7] = (start_volume + (i + (j * 8) + 7) * amount);
    }
    
    for(j = 0; j < 32; j++){
      buffer_arr->vector.vec_double->mem_double[j] *= volume_arr->vector.vec_double->mem_double[j];
    }
    
    ags_audio_buffer_util_fetch_v8double(NULL,
					 source, source_stride,
					 buffer_arr->vector.vec_double->mem_double, 1,
					 32);
    
    ags_vector_256_manager_release(vector_256_manager,
				   buffer_arr);
    ags_vector_256_manager_release(vector_256_manager,
				   sine_arr);
    ags_vector_256_manager_release(vector_256_manager,
				   volume_arr);

    source += (256 * source_stride);
    i += 256;
  }
#endif

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;
    ags_v8double v_sine;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride),
      (gdouble) *(source += source_stride)
    };

    source += source_stride;

    v_volume = (ags_v8double) {
      (gdouble) (start_volume + (i) * amount),
      (gdouble) (start_volume + (i + 1) * amount),
      (gdouble) (start_volume + (i + 2) * amount),
      (gdouble) (start_volume + (i + 3) * amount),
      (gdouble) (start_volume + (i + 4) * amount),
      (gdouble) (start_volume + (i + 5) * amount),
      (gdouble) (start_volume + (i + 6) * amount),
      (gdouble) (start_volume + (i + 7) * amount)
    };

    if(lfo_enabled){
      v_sine = (ags_v8double) {
	sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
	sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),	
      };
    
      v_buffer *= v_sine;
    }

    v_buffer *= v_volume;

    *(destination) = (gdouble) v_buffer[0];
    *(destination += destination_stride) = (gdouble) v_buffer[1];
    *(destination += destination_stride) = (gdouble) v_buffer[2];
    *(destination += destination_stride) = (gdouble) v_buffer[3];
    *(destination += destination_stride) = (gdouble) v_buffer[4];
    *(destination += destination_stride) = (gdouble) v_buffer[5];
    *(destination += destination_stride) = (gdouble) v_buffer[6];
    *(destination += destination_stride) = (gdouble) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride),
      (double) *(source += source_stride)};

    double v_volume[] = {(double) start_volume + i * amount,
			 (double) (start_volume + (i) * amount),
			 (double) (start_volume + (i + 1) * amount),
			 (double) (start_volume + (i + 2) * amount),
			 (double) (start_volume + (i + 3) * amount),
			 (double) (start_volume + (i + 4) * amount),
			 (double) (start_volume + (i + 5) * amount),
			 (double) (start_volume + (i + 6) * amount),
			 (double) (start_volume + (i + 7) * amount)};

    source += source_stride;

    if(lfo_enabled){
      double ret_sine_buffer[8];
      
      double v_sine[] = {sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
			 sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
			 sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
			 sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
			 sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
			 sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
			 sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate),
			 sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate)};
    
      vDSP_vmulD(v_buffer, 1, v_sine, 1, ret_sine_buffer, 1, 8);

      vDSP_vmulD(ret_sine_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);
    }else{
      vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);
    }

    *(destination) = (gdouble) ret_v_buffer[0];
    *(destination += destination_stride) = (gdouble) ret_v_buffer[1];
    *(destination += destination_stride) = (gdouble) ret_v_buffer[2];
    *(destination += destination_stride) = (gdouble) ret_v_buffer[3];
    *(destination += destination_stride) = (gdouble) ret_v_buffer[4];
    *(destination += destination_stride) = (gdouble) ret_v_buffer[5];
    *(destination += destination_stride) = (gdouble) ret_v_buffer[6];
    *(destination += destination_stride) = (gdouble) ret_v_buffer[7];

    destination += destination_stride;
    
    i += 8;
  }
#else
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    if(lfo_enabled){
      *(destination) = (gdouble) ((source)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i) * amount));
      *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * sin((gdouble) (offset + i + 1) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 1) * amount));
      *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * sin((gdouble) (offset + i + 2) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 2) * amount));
      *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * sin((gdouble) (offset + i + 3) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 3) * amount));
      *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * sin((gdouble) (offset + i + 4) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 4) * amount));
      *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * sin((gdouble) (offset + i + 5) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 5) * amount));
      *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * sin((gdouble) (offset + i + 6) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 6) * amount));
      *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * sin((gdouble) (offset + i + 7) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i + 7) * amount));
    }else{
      *(destination) = (gdouble) ((source)[0] * (double) (start_volume + (i) * amount));
      *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (start_volume + (i + 1) * amount));
      *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (start_volume + (i + 2) * amount));
      *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (start_volume + (i + 3) * amount));
      *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (start_volume + (i + 4) * amount));
      *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (start_volume + (i + 5) * amount));
      *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (start_volume + (i + 6) * amount));
      *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (start_volume + (i + 7) * amount));
    }
    
    destination += destination_stride;
    source += source_stride;

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < envelope_util->buffer_length;){
    if(lfo_enabled){
      *(destination) = (gdouble) ((source)[0] * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i) * amount));
    }else{
      *(destination) = (gdouble) ((source)[0] * (double) (start_volume + (i) * amount));
    }
    
    destination += destination_stride;
    source += source_stride;
    i++;
  }

  envelope_util->volume = start_volume + i * amount;
}

/**
 * ags_envelope_util_compute_complex:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Compute envelope of complex floating point data.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_compute_complex(AgsEnvelopeUtil *envelope_util)
{
  AgsComplex *destination;
  AgsComplex *source;

  guint source_stride, destination_stride;
  guint samplerate;
  gdouble start_volume;
  gdouble amount;
  guint offset;
  gdouble lfo_freq;
  gboolean lfo_enabled;
  guint i;

  destination = (AgsComplex *) envelope_util->destination;
  source = (AgsComplex *) envelope_util->source;
  
  source_stride = envelope_util->source_stride;
  destination_stride = envelope_util->destination_stride;

  samplerate = envelope_util->samplerate;

  start_volume = envelope_util->volume;
  amount = envelope_util->amount;

  lfo_enabled = envelope_util->lfo_enabled;
  lfo_freq = envelope_util->lfo_freq;

  offset = envelope_util->offset;

  i = 0;
  
  for(; i < envelope_util->buffer_length;){
    double _Complex z;

    z = ags_complex_get(source);

    if(lfo_enabled){
      ags_complex_set(destination,
		      z * sin((gdouble) (offset + i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) * (double) (start_volume + (i) * amount));
    }else{
      ags_complex_set(destination,
		      z * (double) (start_volume + (i) * amount));
    }
    
    destination += destination_stride;
    source += source_stride;
    i++;
  }

  envelope_util->volume = start_volume + i * amount;
}

/**
 * ags_envelope_util_compute:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Compute envelope.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_compute(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL ||
     envelope_util->destination == NULL ||
     envelope_util->source == NULL){
    return;
  }

  switch(envelope_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_envelope_util_compute_s8(envelope_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_envelope_util_compute_s16(envelope_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_envelope_util_compute_s24(envelope_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_envelope_util_compute_s32(envelope_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_envelope_util_compute_s64(envelope_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_envelope_util_compute_float(envelope_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_envelope_util_compute_double(envelope_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_envelope_util_compute_complex(envelope_util);
  }
  break;
  default:
    {
      g_warning("envelope util - unsupported soundcard format");
    }
  }
}
