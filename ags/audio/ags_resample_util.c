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

#include <ags/audio/ags_resample_util.h>

#include <ags/audio/ags_audio_buffer_util.h>

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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_resample_util = 0;

    ags_type_resample_util =
      g_boxed_type_register_static("AgsResampleUtil",
				   (GBoxedCopyFunc) ags_resample_util_copy,
				   (GBoxedFreeFunc) ags_resample_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_resample_util);
  }

  return g_define_type_id__volatile;
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

  ptr->secret_rabbit.src_ratio = 1.0;

  ptr->secret_rabbit.input_frames = 0;
  ptr->secret_rabbit.data_in = NULL;

  ptr->secret_rabbit.output_frames = 0;
  ptr->secret_rabbit.data_out = NULL;
  
  ptr->destination = NULL;
  ptr->destination_stride = 1;

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->buffer_length = 0;
  ptr->audio_buffer_util_format = AGS_RESAMPLE_UTIL_DEFAULT_AUDIO_BUFFER_UTIL_FORMAT;
  ptr->samplerate = AGS_RESAMPLE_UTIL_DEFAULT_SAMPLERATE;

  ptr->target_samplerate = AGS_RESAMPLE_UTIL_DEFAULT_TARGET_SAMPLERATE;

  return(ptr);
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
  
  new_ptr = (AgsResampleUtil *) g_new(AgsResampleUtil,
				      1);
  
  new_ptr->secret_rabbit.src_ratio = 1.0;

  new_ptr->secret_rabbit.input_frames = ptr->secret_rabbit.input_frames;
  new_ptr->secret_rabbit.data_in = NULL;

  if(new_ptr->secret_rabbit.input_frames > 0){
    new_ptr->secret_rabbit.data_in = (gfloat *) g_malloc(new_ptr->secret_rabbit.input_frames * sizeof(gfloat));
  }

  new_ptr->secret_rabbit.output_frames = ptr->secret_rabbit.output_frames;
  new_ptr->secret_rabbit.data_out = NULL;

  if(new_ptr->secret_rabbit.output_frames > 0){
    new_ptr->secret_rabbit.data_out = (gfloat *) g_malloc(new_ptr->secret_rabbit.output_frames * sizeof(gfloat));
  }

  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->audio_buffer_util_format = ptr->audio_buffer_util_format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->target_samplerate = ptr->target_samplerate;

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
  g_free(ptr->secret_rabbit.data_in);
  g_free(ptr->secret_rabbit.data_out);
  
  g_free(ptr->destination);

  if(ptr->destination != ptr->source){
    g_free(ptr->source);
  }
  
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
 * @destination: the destination buffer
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
 * @source: the source buffer
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
    resample_util->secret_rabbit.input_frames = buffer_length;
    resample_util->secret_rabbit.output_frames = ceil(resample_util->secret_rabbit.src_ratio * buffer_length);

    g_free(resample_util->secret_rabbit.data_in);
    g_free(resample_util->secret_rabbit.data_out);
    
    resample_util->secret_rabbit.data_in = (gfloat *) g_malloc(buffer_length * sizeof(gfloat));
    resample_util->secret_rabbit.data_out = (gfloat *) g_malloc(resample_util->secret_rabbit.output_frames * sizeof(gfloat));
  }else{
    resample_util->secret_rabbit.input_frames = 0;
    resample_util->secret_rabbit.output_frames = 0;
    
    g_free(resample_util->secret_rabbit.data_out);
    g_free(resample_util->secret_rabbit.data_in);

    resample_util->secret_rabbit.data_out = NULL;
    resample_util->secret_rabbit.data_in = NULL;
  }
}

/**
 * ags_resample_util_get_audio_buffer_util_format:
 * @resample_util: the #AgsResampleUtil-struct
 * 
 * Get audio buffer util format of @resample_util.
 * 
 * Returns: the audio buffer util format
 * 
 * Since: 3.9.2
 */
guint
ags_resample_util_get_audio_buffer_util_format(AgsResampleUtil *resample_util)
{
  if(resample_util == NULL){
    return(0);
  }

  return(resample_util->audio_buffer_util_format);
}

/**
 * ags_resample_util_set_audio_buffer_util_format:
 * @resample_util: the #AgsResampleUtil-struct
 * @audio_buffer_util_format: the audio buffer util format
 *
 * Set @audio_buffer_util_format of @resample_util.
 *
 * Since: 3.9.2
 */
void
ags_resample_util_set_audio_buffer_util_format(AgsResampleUtil *resample_util,
					       guint audio_buffer_util_format)
{
  if(resample_util == NULL){
    return;
  }

  resample_util->audio_buffer_util_format = audio_buffer_util_format;
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
    return(AGS_RESAMPLE_UTIL_DEFAULT_SAMPLERATE);
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

  resample_util->secret_rabbit.src_ratio = resample_util->target_samplerate / samplerate;

  if(resample_util->buffer_length > 0){
    resample_util->secret_rabbit.output_frames = ceil(resample_util->secret_rabbit.src_ratio * resample_util->buffer_length);

    g_free(resample_util->secret_rabbit.data_out);
    
    resample_util->secret_rabbit.data_out = (gfloat *) g_malloc(resample_util->secret_rabbit.output_frames * sizeof(gfloat));
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
    return(AGS_RESAMPLE_UTIL_DEFAULT_TARGET_SAMPLERATE);
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
  
  resample_util->secret_rabbit.src_ratio = target_samplerate / resample_util->samplerate;

  if(resample_util->buffer_length > 0){
    resample_util->secret_rabbit.output_frames = ceil(resample_util->secret_rabbit.src_ratio * resample_util->buffer_length);

    g_free(resample_util->secret_rabbit.data_out);
    
    resample_util->secret_rabbit.data_out = (gfloat *) g_malloc(resample_util->secret_rabbit.output_frames * sizeof(gfloat));
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
  SRC_DATA *secret_rabbit;

  gint8 *destination;
  gint8 *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  secret_rabbit = &(resample_util->secret_rabbit);
  
  destination = (gint8 *) resample_util->destination;
  source = (gint8 *) resample_util->source;
    
  ags_audio_buffer_util_clear_float(secret_rabbit->data_in, 1,
				    secret_rabbit->input_frames);

  ags_audio_buffer_util_copy_s8_to_float(secret_rabbit->data_in, 1,
					 source, resample_util->source_stride,
					 resample_util->buffer_length);

  src_simple(secret_rabbit,
	     SRC_SINC_BEST_QUALITY,
	     1);

  memset(destination, 0, resample_util->buffer_length * sizeof(gint8));
  
  ags_audio_buffer_util_copy_float_to_s8(destination, resample_util->destination_stride,
					 secret_rabbit->data_out, 1,
					 resample_util->buffer_length);
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
  SRC_DATA *secret_rabbit;

  gint16 *destination;
  gint16 *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  secret_rabbit = &(resample_util->secret_rabbit);
  
  destination = (gint16 *) resample_util->destination;
  source = (gint16 *) resample_util->source;
    
  ags_audio_buffer_util_clear_float(secret_rabbit->data_in, 1,
				    secret_rabbit->input_frames);

  ags_audio_buffer_util_copy_s16_to_float(secret_rabbit->data_in, 1,
					  source, resample_util->source_stride,
					  resample_util->buffer_length);

  src_simple(secret_rabbit,
	     SRC_SINC_BEST_QUALITY,
	     1);

  memset(destination, 0, resample_util->buffer_length * sizeof(gint8));
  
  ags_audio_buffer_util_copy_float_to_s16(destination, resample_util->destination_stride,
					  secret_rabbit->data_out, 1,
					  resample_util->buffer_length);
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
  SRC_DATA *secret_rabbit;

  gint32 *destination;
  gint32 *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  secret_rabbit = &(resample_util->secret_rabbit);
  
  destination = (gint32 *) resample_util->destination;
  source = (gint32 *) resample_util->source;
  
  ags_audio_buffer_util_clear_float(secret_rabbit->data_in, 1,
				    secret_rabbit->input_frames);

  ags_audio_buffer_util_copy_s24_to_float(secret_rabbit->data_in, 1,
					  source, resample_util->source_stride,
					  resample_util->buffer_length);

  src_simple(secret_rabbit,
	     SRC_SINC_BEST_QUALITY,
	     1);

  memset(destination, 0, resample_util->buffer_length * sizeof(gint8));
  
  ags_audio_buffer_util_copy_float_to_s24(destination, resample_util->destination_stride,
					  secret_rabbit->data_out, 1,
					  resample_util->buffer_length);
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
  SRC_DATA *secret_rabbit;

  gint32 *destination;
  gint32 *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  secret_rabbit = &(resample_util->secret_rabbit);
  
  destination = (gint32 *) resample_util->destination;
  source = (gint32 *) resample_util->source;
  
  ags_audio_buffer_util_clear_float(secret_rabbit->data_in, 1,
				    secret_rabbit->input_frames);

  ags_audio_buffer_util_copy_s32_to_float(secret_rabbit->data_in, 1,
					  source, resample_util->source_stride,
					  resample_util->buffer_length);

  src_simple(secret_rabbit,
	     SRC_SINC_BEST_QUALITY,
	     1);

  memset(destination, 0, resample_util->buffer_length * sizeof(gint8));
  
  ags_audio_buffer_util_copy_float_to_s32(destination, resample_util->destination_stride,
					  secret_rabbit->data_out, 1,
					  resample_util->buffer_length);
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
  SRC_DATA *secret_rabbit;

  gint64 *destination;
  gint64 *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  secret_rabbit = &(resample_util->secret_rabbit);
  
  destination = (gint64 *) resample_util->destination;
  source = (gint64 *) resample_util->source;
  
  ags_audio_buffer_util_clear_float(secret_rabbit->data_in, 1,
				    secret_rabbit->input_frames);

  ags_audio_buffer_util_copy_s64_to_float(secret_rabbit->data_in, 1,
					  source, resample_util->source_stride,
					  resample_util->buffer_length);

  src_simple(secret_rabbit,
	     SRC_SINC_BEST_QUALITY,
	     1);

  memset(destination, 0, resample_util->buffer_length * sizeof(gint8));
  
  ags_audio_buffer_util_copy_float_to_s64(destination, resample_util->destination_stride,
					  secret_rabbit->data_out, 1,
					  resample_util->buffer_length);
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
  SRC_DATA *secret_rabbit;

  gfloat *destination;
  gfloat *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  secret_rabbit = &(resample_util->secret_rabbit);
  
  destination = (gfloat *) resample_util->destination;
  source = (gfloat *) resample_util->source;
  
  ags_audio_buffer_util_clear_float(secret_rabbit->data_in, 1,
				    secret_rabbit->input_frames);

  ags_audio_buffer_util_copy_float_to_float(secret_rabbit->data_in, 1,
					    source, resample_util->source_stride,
					    resample_util->buffer_length);

  src_simple(secret_rabbit,
	     SRC_SINC_BEST_QUALITY,
	     1);

  memset(destination, 0, resample_util->buffer_length * sizeof(gint8));
  
  ags_audio_buffer_util_copy_float_to_float(destination, resample_util->destination_stride,
					    secret_rabbit->data_out, 1,
					    resample_util->buffer_length);
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
  SRC_DATA *secret_rabbit;

  gdouble *destination;
  gdouble *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }

  secret_rabbit = &(resample_util->secret_rabbit);
  
  destination = (gdouble *) resample_util->destination;
  source = (gdouble *) resample_util->source;
  
  ags_audio_buffer_util_clear_float(secret_rabbit->data_in, 1,
				    secret_rabbit->input_frames);

  ags_audio_buffer_util_copy_double_to_float(secret_rabbit->data_in, 1,
					     source, resample_util->source_stride,
					     resample_util->buffer_length);

  src_simple(secret_rabbit,
	     SRC_SINC_BEST_QUALITY,
	     1);

  memset(destination, 0, resample_util->buffer_length * sizeof(gint8));
  
  ags_audio_buffer_util_copy_float_to_double(destination, resample_util->destination_stride,
					     secret_rabbit->data_out, 1,
					     resample_util->buffer_length);
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
  SRC_DATA *secret_rabbit;

  AgsComplex *destination;
  AgsComplex *source;

  if(resample_util == NULL ||
     resample_util->destination == NULL ||
     resample_util->source == NULL){
    return;
  }
  
  secret_rabbit = &(resample_util->secret_rabbit);
  
  destination = (AgsComplex *) resample_util->destination;
  source = (AgsComplex *) resample_util->source;
  
  ags_audio_buffer_util_clear_float(secret_rabbit->data_in, 1,
				    secret_rabbit->input_frames);

  ags_audio_buffer_util_copy_complex_to_float(secret_rabbit->data_in, 1,
					      source, resample_util->source_stride,
					      resample_util->buffer_length);

  src_simple(secret_rabbit,
	     SRC_SINC_BEST_QUALITY,
	     1);

  memset(destination, 0, resample_util->buffer_length * sizeof(gint8));
  
  ags_audio_buffer_util_copy_float_to_complex(destination, resample_util->destination_stride,
					      secret_rabbit->data_out, 1,
					      resample_util->buffer_length);
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

  switch(resample_util->audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_resample_util_compute_s8(resample_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_resample_util_compute_s16(resample_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_resample_util_compute_s24(resample_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_resample_util_compute_s32(resample_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_resample_util_compute_s64(resample_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_resample_util_compute_float(resample_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_resample_util_compute_double(resample_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_resample_util_compute_complex(resample_util);
  }
  break;
  }
}
