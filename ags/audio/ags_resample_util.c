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

#include <ags/audio/ags_resample_util.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_samplerate_process.h>
#include <ags/audio/ags_samplerate_coeffs.h>

#include <complex.h>

/**
 * SECTION:ags_resample_util
 * @short_description: Boxed type of resample util
 * @title: AgsResampleUtil
 * @section_id:
 * @include: ags/audio/ags_resample_util.h
 *
 * Boxed type of resample util data type.
 */

GType
ags_resample_util_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_resample_util = 0;

    ags_type_resample_util =
      g_boxed_type_register_static("AgsResampleUtil",
				   (GBoxedCopyFunc) ags_resample_util_copy,
				   (GBoxedFreeFunc) ags_resample_util_free);

    g_once_init_leave(&g_define_type_id__static, ags_type_resample_util);
  }

  return(g_define_type_id__static);
}

/**
 * ags_resample_util_alloc:
 *
 * Allocate #AgsResampleUtil-struct
 *
 * Returns: a new #AgsResampleUtil-struct
 *
 * Since: 3.9.2
 */
AgsResampleUtil*
ags_resample_util_alloc()
{
  AgsResampleUtil *ptr;

  ptr = (AgsResampleUtil *) g_new(AgsResampleUtil,
				  1);

  ptr[0] = AGS_RESAMPLE_UTIL_INITIALIZER;
  
  ptr->b_current = 0;
  ptr->b_end = 0;
  ptr->b_real_end = -1;
  ptr->b_len = 3 * (int) psf_lrint ((ptr->coeff_half_len + 2.0) / ptr->index_inc * SRC_MAX_RATIO + 1);
  ptr->b_len = MAX (ptr->b_len, 4096);
  ptr->b_len += 1;

  return(ptr);
}

/**
 * ags_resample_util_init:
 * @resample_util: the original #AgsResampleUtil-struct
 *
 * Init #AgsResampleUtil-struct
 *
 * Since: 5.5.1
 */
void
ags_resample_util_init(AgsResampleUtil *resample_util)
{
  g_return_if_fail(resample_util != NULL);

  resample_util[0] = AGS_RESAMPLE_UTIL_INITIALIZER;
  
  resample_util->b_current = 0;
  resample_util->b_end = 0;
  resample_util->b_real_end = -1;
  resample_util->b_len = 3 * (int) psf_lrint ((resample_util->coeff_half_len + 2.0) / resample_util->index_inc * SRC_MAX_RATIO + 1);
  resample_util->b_len = MAX (resample_util->b_len, 4096);
  resample_util->b_len += 1;
  
  resample_util->buffer = NULL;
}

/**
 * ags_resample_util_copy:
 * @ptr: the original #AgsResampleUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsResampleUtil-struct
 *
 * Since: 3.9.2
 */
gpointer
ags_resample_util_copy(AgsResampleUtil *ptr)
{
  AgsResampleUtil *new_ptr;
  
  g_return_val_if_fail(ptr != NULL, NULL);
  
  new_ptr = (AgsResampleUtil *) g_new(AgsResampleUtil,
				      1);

  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->target_samplerate = ptr->target_samplerate;

  new_ptr->src_ratio = ptr->src_ratio;

  new_ptr->input_frames = ptr->input_frames;
  new_ptr->data_in = NULL;

  if(new_ptr->input_frames > 0){
    new_ptr->data_in = ags_stream_alloc(MAX(ptr->input_frames, 4096),
					ptr->format);
  }

  new_ptr->output_frames = ptr->output_frames;
  new_ptr->data_out = NULL;

  if(new_ptr->output_frames > 0){
    new_ptr->data_out = ags_stream_alloc(MAX(ptr->output_frames, 4096),
					 ptr->format);
  }

  new_ptr->increment = ptr->increment;

  new_ptr->bypass_cache = ptr->bypass_cache;
  
  new_ptr->in_count = ptr->in_count;
  new_ptr->in_used = ptr->in_used;
  new_ptr->out_count = ptr->out_count;
  new_ptr->out_gen = ptr->out_gen;
  
  new_ptr->coeff_half_len = ptr->coeff_half_len;
  new_ptr->index_inc = ptr->index_inc;
  
  new_ptr->input_index = ptr->input_index;
  
  new_ptr->b_current = ptr->b_current;
  new_ptr->b_end = ptr->b_end;
  new_ptr->b_real_end = ptr->b_real_end;
  new_ptr->b_len = ptr->b_len;
  
  new_ptr->coeffs = ptr->coeffs;
  
  new_ptr->left_calc = ptr->left_calc;
  new_ptr->right_calc = ptr->right_calc;

  new_ptr->coeffs = ptr->coeffs;

  new_ptr->last_ratio = ptr->last_ratio;
  new_ptr->last_position = ptr->last_position;

  if(ptr->input_frames < ptr->output_frames){
    new_ptr->buffer = ags_stream_alloc(MAX(ptr->output_frames, 4096),
				       ptr->format);
  }else{
    new_ptr->buffer = ags_stream_alloc(MAX(ptr->input_frames, 4096),
				       ptr->format);
  }
  
  new_ptr->audio_buffer_util = ags_audio_buffer_util_copy(ptr->audio_buffer_util);
  
  return(new_ptr);
}

/**
 * ags_resample_util_free:
 * @ptr: the #AgsResampleUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_free(AgsResampleUtil *ptr)
{
  g_return_if_fail(ptr != NULL);

  ags_stream_free(ptr->data_in);
  ags_stream_free(ptr->data_out);
    
  ags_stream_free(ptr->buffer);
  
  g_free(ptr);
}

/**
 * ags_resample_util_get_destination:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Get destination buffer of @resample_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 3.9.2
 */
gpointer
ags_resample_util_get_destination(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL){
    return(NULL);
  }

  return(resample_util->destination);
}

/**
 * ags_resample_util_set_destination:
 * @resample_util: the #AgsResampleUtil-struct
 * @destination: (transfer none): the destination buffer
 *
 * Set @destination buffer of @resample_util.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_set_destination(AgsResampleUtil *resample_util,
				  gpointer destination)
{
  if(resample_util == NULL){
    return;
  }

  resample_util->destination = destination;
}

/**
 * ags_resample_util_get_destination_stride:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Get destination stride of @resample_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 3.9.2
 */
guint
ags_resample_util_get_destination_stride(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL){
    return(0);
  }

  return(resample_util->destination_stride);
}

/**
 * ags_resample_util_set_destination_stride:
 * @resample_util: the #AgsResampleUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @resample_util.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_set_destination_stride(AgsResampleUtil *resample_util,
					 guint destination_stride)
{
  if(resample_util == NULL){
    return;
  }

  resample_util->destination_stride = destination_stride;
}

/**
 * ags_resample_util_get_source:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Get source buffer of @resample_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.9.2
 */
gpointer
ags_resample_util_get_source(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL){
    return(NULL);
  }

  return(resample_util->source);
}

/**
 * ags_resample_util_set_source:
 * @resample_util: the #AgsResampleUtil-struct
 * @source: (transfer none): the source buffer
 *
 * Set @source buffer of @resample_util.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_set_source(AgsResampleUtil *resample_util,
			     gpointer source)
{
  if(resample_util == NULL){
    return;
  }

  resample_util->source = source;
}

/**
 * ags_resample_util_get_source_stride:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Get source stride of @resample_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.9.2
 */
guint
ags_resample_util_get_source_stride(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL){
    return(0);
  }

  return(resample_util->source_stride);
}

/**
 * ags_resample_util_set_source_stride:
 * @resample_util: the #AgsResampleUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @resample_util.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_set_source_stride(AgsResampleUtil *resample_util,
				    guint source_stride)
{
  if(resample_util == NULL){
    return;
  }

  resample_util->source_stride = source_stride;
}

/**
 * ags_resample_util_get_buffer_length:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Get buffer length of @resample_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.9.2
 */
guint
ags_resample_util_get_buffer_length(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL){
    return(0);
  }

  return(resample_util->buffer_length);
}

/**
 * ags_resample_util_set_buffer_length:
 * @resample_util: the #AgsResampleUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @resample_util.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_set_buffer_length(AgsResampleUtil *resample_util,
				    guint buffer_length)
{
  if(resample_util == NULL ||
     resample_util->buffer_length == buffer_length){
    return;
  }

  resample_util->buffer_length = buffer_length;

  if(buffer_length > 0){
    resample_util->input_frames = buffer_length;
    resample_util->output_frames = ceil(resample_util->src_ratio * buffer_length);

    ags_stream_free(resample_util->data_in);
    ags_stream_free(resample_util->data_out);
    
    ags_stream_free(resample_util->buffer);

    resample_util->data_in = ags_stream_alloc(MAX(resample_util->input_frames, 4096),
					      resample_util->format);
    resample_util->data_out = ags_stream_alloc(MAX(resample_util->output_frames, 4096),
					       resample_util->format);

    if(resample_util->input_frames < resample_util->output_frames){
      resample_util->buffer = ags_stream_alloc(MAX(resample_util->output_frames, 4096),
					       resample_util->format);
    }else{
      resample_util->buffer = ags_stream_alloc(MAX(resample_util->input_frames, 4096),
					       resample_util->format);
    }
  }else{
    resample_util->input_frames = 0;
    resample_util->output_frames = 0;
    
    ags_stream_free(resample_util->data_out);
    ags_stream_free(resample_util->data_in);

    ags_stream_free(resample_util->buffer);
    
    resample_util->data_out = NULL;
    resample_util->data_in = NULL;

    resample_util->buffer = NULL;
  }
}

/**
 * ags_resample_util_get_format:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Get format of @resample_util.
 * 
 * Returns: the format
 * 
 * Since: 3.9.6
 */
AgsSoundcardFormat
ags_resample_util_get_format(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL){
    return(0);
  }

  return(resample_util->format);
}

/**
 * ags_resample_util_set_format:
 * @resample_util: the #AgsResampleUtil-struct
 * @format: the format
 *
 * Set @format of @resample_util.
 *
 * Since: 3.9.6
 */
void
ags_resample_util_set_format(AgsResampleUtil *resample_util,
			     AgsSoundcardFormat format)
{
  if(resample_util == NULL ||
     resample_util->format == format){
    return;
  }

  resample_util->format = format;

  if(resample_util->buffer_length > 0){
    ags_stream_free(resample_util->data_out);
    ags_stream_free(resample_util->data_in);
    
    ags_stream_free(resample_util->buffer);

    resample_util->data_out = ags_stream_alloc(MAX(resample_util->output_frames, 4096),
					       resample_util->format);
    resample_util->data_in = ags_stream_alloc(MAX(resample_util->input_frames, 4096),
					      resample_util->format);

    if(resample_util->input_frames < resample_util->output_frames){
      resample_util->buffer = ags_stream_alloc(MAX(resample_util->output_frames, 4096),
					       resample_util->format);
    }else{
      resample_util->buffer = ags_stream_alloc(MAX(resample_util->input_frames, 4096),
					       resample_util->format);
    }
  }
}

/**
 * ags_resample_util_get_samplerate:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Get samplerate of @resample_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.9.2
 */
guint
ags_resample_util_get_samplerate(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL){
    return(0);
  }
  
  return(resample_util->samplerate);
}

/**
 * ags_resample_util_set_samplerate:
 * @resample_util: the #AgsResampleUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @resample_util.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_set_samplerate(AgsResampleUtil *resample_util,
				 guint samplerate)
{
  if(resample_util == NULL ||
     resample_util->samplerate == samplerate){
    return;
  }

  resample_util->samplerate = samplerate;

  resample_util->src_ratio = resample_util->target_samplerate / samplerate;

  if(resample_util->buffer_length > 0){
    resample_util->output_frames = ceil(resample_util->src_ratio * resample_util->buffer_length);

    ags_stream_free(resample_util->data_out);
    
    ags_stream_free(resample_util->buffer);

    resample_util->data_out = ags_stream_alloc(MAX(resample_util->output_frames, 4096),
					       resample_util->format);

    if(resample_util->input_frames < resample_util->output_frames){
      resample_util->buffer = ags_stream_alloc(MAX(resample_util->output_frames, 4096),
					       resample_util->format);
    }else{
      resample_util->buffer = ags_stream_alloc(MAX(resample_util->input_frames, 4096),
					       resample_util->format);
    }
  }
}

/**
 * ags_resample_util_get_target_samplerate:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Get target samplerate of @resample_util.
 * 
 * Returns: the  samplerate
 * 
 * Since: 3.9.2
 */
guint
ags_resample_util_get_target_samplerate(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL){
    return(0);
  }

  return(resample_util->target_samplerate);
}

/**
 * ags_resample_util_set_target_samplerate:
 * @resample_util: the #AgsResampleUtil-struct
 * @target_samplerate: the target samplerate
 *
 * Set @target_samplerate of @resample_util.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_set_target_samplerate(AgsResampleUtil *resample_util,
					guint target_samplerate)
{
  if(resample_util == NULL ||
     resample_util->target_samplerate == target_samplerate){
    return;
  }

  resample_util->target_samplerate = target_samplerate;
  
  resample_util->src_ratio = target_samplerate / resample_util->samplerate;

  if(resample_util->buffer_length > 0){
    resample_util->output_frames = ceil(resample_util->src_ratio * resample_util->buffer_length);

    ags_stream_free(resample_util->data_out);

    ags_stream_free(resample_util->buffer);
    
    resample_util->data_out = ags_stream_alloc(MAX(resample_util->output_frames, 4096),
					       resample_util->format);

    if(resample_util->input_frames < resample_util->output_frames){
      resample_util->buffer = ags_stream_alloc(MAX(resample_util->output_frames, 4096),
					       resample_util->format);
    }else{
      resample_util->buffer = ags_stream_alloc(MAX(resample_util->input_frames, 4096),
					       resample_util->format);
    }
  }
}

/**
 * ags_resample_util_compute_s8:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Compute resample of signed 8 bit data.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_compute_s8(AgsResampleUtil *resample_util)
{
  gint8 *destination;
  gint8 *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  destination = (gint8 *) resample_util->destination;
  source = (gint8 *) resample_util->source;

  if(resample_util->samplerate == resample_util->target_samplerate){
    ags_audio_buffer_util_copy_s8_to_s8(resample_util->audio_buffer_util,
					destination, resample_util->destination_stride,
					source, resample_util->source_stride,
					resample_util->buffer_length);
  }else{
    ags_audio_buffer_util_clear_buffer(resample_util->audio_buffer_util,
				       resample_util->data_in, 1,
				       resample_util->input_frames, AGS_AUDIO_BUFFER_UTIL_S8);

    ags_audio_buffer_util_copy_s8_to_s8(resample_util->audio_buffer_util,
					resample_util->data_in, 1,
					source, resample_util->source_stride,
					resample_util->input_frames);
  
    ags_samplerate_process_resample_util(resample_util);

    memset(destination, 0, resample_util->output_frames * sizeof(gint8));
  
    ags_audio_buffer_util_copy_s8_to_s8(resample_util->audio_buffer_util,
					destination, resample_util->destination_stride,
					resample_util->data_out, 1,
					resample_util->output_frames);
  }
}

/**
 * ags_resample_util_compute_s16:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Compute resample of signed 16 bit data.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_compute_s16(AgsResampleUtil *resample_util)
{
  gint16 *destination;
  gint16 *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  destination = (gint16 *) resample_util->destination;
  source = (gint16 *) resample_util->source;
    
  if(resample_util->samplerate == resample_util->target_samplerate){
    ags_audio_buffer_util_copy_s16_to_s16(resample_util->audio_buffer_util,
					  destination, resample_util->destination_stride,
					  source, resample_util->source_stride,
					  resample_util->buffer_length);
  }else{
    ags_audio_buffer_util_clear_buffer(resample_util->audio_buffer_util,
				       resample_util->data_in, 1,
				       resample_util->input_frames, AGS_AUDIO_BUFFER_UTIL_S16);

    ags_audio_buffer_util_copy_s16_to_s16(resample_util->audio_buffer_util,
					  resample_util->data_in, 1,
					  source, resample_util->source_stride,
					  resample_util->input_frames);

    ags_samplerate_process_resample_util(resample_util);

    memset(destination, 0, resample_util->output_frames * sizeof(gint16));
  
    ags_audio_buffer_util_copy_s16_to_s16(resample_util->audio_buffer_util,
					  destination, resample_util->destination_stride,
					  resample_util->data_out, 1,
					  resample_util->output_frames);
  }
}

/**
 * ags_resample_util_compute_s24:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Compute resample of signed 24 bit data.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_compute_s24(AgsResampleUtil *resample_util)
{
  gint32 *destination;
  gint32 *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  destination = (gint32 *) resample_util->destination;
  source = (gint32 *) resample_util->source;
  
  if(resample_util->samplerate == resample_util->target_samplerate){
    ags_audio_buffer_util_copy_s24_to_s24(resample_util->audio_buffer_util,
					  destination, resample_util->destination_stride,
					  source, resample_util->source_stride,
					  resample_util->buffer_length);
  }else{
    ags_audio_buffer_util_clear_buffer(resample_util->audio_buffer_util,
				       resample_util->data_in, 1,
				       resample_util->input_frames, AGS_AUDIO_BUFFER_UTIL_S24);

    ags_audio_buffer_util_copy_s24_to_s24(resample_util->audio_buffer_util,
					  resample_util->data_in, 1,
					  source, resample_util->source_stride,
					  resample_util->input_frames);
  
    ags_samplerate_process_resample_util(resample_util);

    memset(destination, 0, resample_util->output_frames * sizeof(gint32));
  
    ags_audio_buffer_util_copy_s24_to_s24(resample_util->audio_buffer_util,
					  destination, resample_util->destination_stride,
					  resample_util->data_out, 1,
					  resample_util->output_frames);
  }
}

/**
 * ags_resample_util_compute_s32:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Compute resample of signed 32 bit data.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_compute_s32(AgsResampleUtil *resample_util)
{
  gint32 *destination;
  gint32 *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  destination = (gint32 *) resample_util->destination;
  source = (gint32 *) resample_util->source;
  
  if(resample_util->samplerate == resample_util->target_samplerate){
    ags_audio_buffer_util_copy_s32_to_s32(resample_util->audio_buffer_util,
					  destination, resample_util->destination_stride,
					  source, resample_util->source_stride,
					  resample_util->buffer_length);
  }else{
    ags_audio_buffer_util_clear_buffer(resample_util->audio_buffer_util,
				       resample_util->data_in, 1,
				       resample_util->input_frames, AGS_AUDIO_BUFFER_UTIL_S32);

    ags_audio_buffer_util_copy_s32_to_s32(resample_util->audio_buffer_util,
					  resample_util->data_in, 1,
					  source, resample_util->source_stride,
					  resample_util->input_frames);

    ags_samplerate_process_resample_util(resample_util);

    memset(destination, 0, resample_util->output_frames * sizeof(gint32));
  
    ags_audio_buffer_util_copy_s32_to_s32(resample_util->audio_buffer_util,
					  destination, resample_util->destination_stride,
					  resample_util->data_out, 1,
					  resample_util->output_frames);
  }
}

/**
 * ags_resample_util_compute_s64:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Compute resample of signed 64 bit data.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_compute_s64(AgsResampleUtil *resample_util)
{
  gint64 *destination;
  gint64 *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  destination = (gint64 *) resample_util->destination;
  source = (gint64 *) resample_util->source;
  
  if(resample_util->samplerate == resample_util->target_samplerate){
    ags_audio_buffer_util_copy_s64_to_s64(resample_util->audio_buffer_util,
					  destination, resample_util->destination_stride,
					  source, resample_util->source_stride,
					  resample_util->buffer_length);
  }else{
    ags_audio_buffer_util_clear_buffer(resample_util->audio_buffer_util,
				       resample_util->data_in, 1,
				       resample_util->input_frames, AGS_AUDIO_BUFFER_UTIL_S64);

    ags_audio_buffer_util_copy_s64_to_s64(resample_util->audio_buffer_util,
					  resample_util->data_in, 1,
					  source, resample_util->source_stride,
					  resample_util->input_frames);

    ags_samplerate_process_resample_util(resample_util);

    memset(destination, 0, resample_util->output_frames * sizeof(gint64));
  
    ags_audio_buffer_util_copy_s64_to_s64(resample_util->audio_buffer_util,
					  destination, resample_util->destination_stride,
					  resample_util->data_out, 1,
					  resample_util->output_frames);
  }
}

/**
 * ags_resample_util_compute_float:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Compute resample of floating point data.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_compute_float(AgsResampleUtil *resample_util)
{
  gfloat *destination;
  gfloat *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  destination = (gfloat *) resample_util->destination;
  source = (gfloat *) resample_util->source;
  
  if(resample_util->samplerate == resample_util->target_samplerate){
    ags_audio_buffer_util_copy_float_to_float(resample_util->audio_buffer_util,
					      destination, resample_util->destination_stride,
					      source, resample_util->source_stride,
					      resample_util->buffer_length);
  }else{
    ags_audio_buffer_util_clear_float(resample_util->audio_buffer_util,
				      resample_util->data_in, 1,
				      resample_util->input_frames);

    ags_audio_buffer_util_copy_float_to_float(resample_util->audio_buffer_util,
					      resample_util->data_in, 1,
					      source, resample_util->source_stride,
					      resample_util->input_frames);

    ags_samplerate_process_resample_util(resample_util);

    memset(destination, 0, resample_util->output_frames * sizeof(gfloat));
  
    ags_audio_buffer_util_copy_float_to_float(resample_util->audio_buffer_util,
					      destination, resample_util->destination_stride,
					      resample_util->data_out, 1,
					      resample_util->output_frames);
  }
}

/**
 * ags_resample_util_compute_double:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Compute resample of double floating point data.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_compute_double(AgsResampleUtil *resample_util)
{
  gdouble *destination;
  gdouble *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }
  
  destination = (gdouble *) resample_util->destination;
  source = (gdouble *) resample_util->source;
  
  if(resample_util->samplerate == resample_util->target_samplerate){
    ags_audio_buffer_util_copy_double_to_double(resample_util->audio_buffer_util,
						destination, resample_util->destination_stride,
						source, resample_util->source_stride,
						resample_util->buffer_length);
  }else{
    ags_audio_buffer_util_clear_double(resample_util->audio_buffer_util,
				       resample_util->data_in, 1,
				       resample_util->input_frames);

    ags_audio_buffer_util_copy_double_to_double(resample_util->audio_buffer_util,
						resample_util->data_in, 1,
						source, resample_util->source_stride,
						resample_util->input_frames);

    //  g_message("in %d", ags_synth_util_get_xcross_count_float(resample_util->data_in,
    //							   resample_util->input_frames));

    ags_samplerate_process_resample_util(resample_util);

    //  g_message("out %d", ags_synth_util_get_xcross_count_float(resample_util->data_out,
    //							    resample_util->output_frames));
  
    memset(destination, 0, resample_util->output_frames * sizeof(gdouble));
  
    ags_audio_buffer_util_copy_double_to_double(resample_util->audio_buffer_util,
						destination, resample_util->destination_stride,
						resample_util->data_out, 1,
						resample_util->output_frames);
  }
}

/**
 * ags_resample_util_compute_complex:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Compute resample of complex floating point data.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_compute_complex(AgsResampleUtil *resample_util)
{
  AgsComplex *destination;
  AgsComplex *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }
  
  destination = (AgsComplex *) resample_util->destination;
  source = (AgsComplex *) resample_util->source;
  
  if(resample_util->samplerate == resample_util->target_samplerate){
    ags_audio_buffer_util_copy_complex_to_complex(resample_util->audio_buffer_util,
						  destination, resample_util->destination_stride,
						  source, resample_util->source_stride,
						  resample_util->buffer_length);
  }else{
    ags_audio_buffer_util_clear_complex(resample_util->audio_buffer_util,
					resample_util->data_in, 1,
					resample_util->input_frames);

    ags_audio_buffer_util_copy_complex_to_complex(resample_util->audio_buffer_util,
						  resample_util->data_in, 1,
						  source, resample_util->source_stride,
						  resample_util->input_frames);

    ags_samplerate_process_resample_util(resample_util);

    memset(destination, 0, resample_util->output_frames * sizeof(AgsComplex));
  
    ags_audio_buffer_util_copy_complex_to_complex(resample_util->audio_buffer_util,
						  destination, resample_util->destination_stride,
						  resample_util->data_out, 1,
						  resample_util->output_frames);
  }
}

/**
 * ags_resample_util_compute:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Compute resample.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_compute(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  switch(resample_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_resample_util_compute_s8(resample_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_resample_util_compute_s16(resample_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_resample_util_compute_s24(resample_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_resample_util_compute_s32(resample_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_resample_util_compute_s64(resample_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_resample_util_compute_float(resample_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_resample_util_compute_double(resample_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_resample_util_compute_complex(resample_util);
  }
  break;
  }
}
