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

#include <ags/audio/ags_synth_util.h>

#include <ags/audio/ags_fourier_transform_util.h>

#include <math.h>
#include <complex.h>

/**
 * SECTION:ags_synth_util
 * @short_description: synth util
 * @title: AgsSynthUtil
 * @section_id:
 * @include: ags/audio/ags_synth_util.h
 *
 * Utility functions to compute synths.
 */

GType
ags_synth_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_synth_util = 0;

    ags_type_synth_util =
      g_boxed_type_register_static("AgsSynthUtil",
				   (GBoxedCopyFunc) ags_synth_util_copy,
				   (GBoxedFreeFunc) ags_synth_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_synth_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_synth_util_alloc:
 *
 * Allocate #AgsSynthUtil-struct
 *
 * Returns: a new #AgsSynthUtil-struct
 *
 * Since: 3.9.3
 */
AgsSynthUtil*
ags_synth_util_alloc()
{
  AgsSynthUtil *ptr;

  ptr = (AgsSynthUtil *) g_new(AgsSynthUtil,
			       1);

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->buffer_length = 0;
  ptr->audio_buffer_util_format = AGS_SYNTH_UTIL_DEFAULT_AUDIO_BUFFER_UTIL_FORMAT;
  ptr->samplerate = 0;
  
  ptr->synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SIN;

  ptr->frequency = AGS_SYNTH_UTIL_DEFAULT_FREQUENCY;
  ptr->phase = 0.0;
  ptr->volume = 1.0;

  ptr->frame_count = 0;
  ptr->offset = 0;

  return(ptr);
}

/**
 * ags_synth_util_copy:
 * @ptr: the original #AgsSynthUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsSynthUtil-struct
 *
 * Since: 3.9.3
 */
gpointer
ags_synth_util_copy(AgsSynthUtil *ptr)
{
  AgsSynthUtil *new_ptr;
  
  new_ptr = (AgsSynthUtil *) g_new(AgsSynthUtil,
				   1);
  
  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->audio_buffer_util_format = ptr->audio_buffer_util_format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->synth_oscillator_mode = ptr->synth_oscillator_mode;

  new_ptr->frequency = ptr->frequency;
  new_ptr->phase = ptr->phase;
  new_ptr->volume = ptr->volume;

  new_ptr->frame_count = ptr->frame_count;
  new_ptr->offset = ptr->offset;
  
  return(new_ptr);
}

/**
 * ags_synth_util_free:
 * @ptr: the #AgsSynthUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_free(AgsSynthUtil *ptr)
{
  g_free(ptr->source);
  
  g_free(ptr);
}

/**
 * ags_synth_util_get_source:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Get source buffer of @synth_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.9.3
 */
gpointer
ags_synth_util_get_source(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL){
    return(NULL);
  }

  return(synth_util->source);
}

/**
 * ags_synth_util_set_source:
 * @synth_util: the #AgsSynthUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @synth_util.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_set_source(AgsSynthUtil *synth_util,
			  gpointer source)
{
  if(synth_util == NULL){
    return;
  }

  synth_util->source = source;
}

/**
 * ags_synth_util_get_source_stride:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Get source stride of @synth_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.9.3
 */
guint
ags_synth_util_get_source_stride(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL){
    return(0);
  }

  return(synth_util->source_stride);
}

/**
 * ags_synth_util_set_source_stride:
 * @synth_util: the #AgsSynthUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @synth_util.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_set_source_stride(AgsSynthUtil *synth_util,
				 guint source_stride)
{
  if(synth_util == NULL){
    return;
  }

  synth_util->source_stride = source_stride;
}

/**
 * ags_synth_util_get_buffer_length:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Get buffer length of @synth_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.9.3
 */
guint
ags_synth_util_get_buffer_length(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL){
    return(0);
  }

  return(synth_util->buffer_length);
}

/**
 * ags_synth_util_set_buffer_length:
 * @synth_util: the #AgsSynthUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @synth_util.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_set_buffer_length(AgsSynthUtil *synth_util,
				 guint buffer_length)
{
  if(synth_util == NULL){
    return;
  }

  synth_util->buffer_length = buffer_length;
}

/**
 * ags_synth_util_get_audio_buffer_util_format:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Get audio buffer util format of @synth_util.
 * 
 * Returns: the audio buffer util format
 * 
 * Since: 3.9.3
 */
guint
ags_synth_util_get_audio_buffer_util_format(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL){
    return(0);
  }

  return(synth_util->audio_buffer_util_format);
}

/**
 * ags_synth_util_set_audio_buffer_util_format:
 * @synth_util: the #AgsSynthUtil-struct
 * @audio_buffer_util_format: the audio buffer util format
 *
 * Set @audio_buffer_util_format of @synth_util.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_set_audio_buffer_util_format(AgsSynthUtil *synth_util,
					    guint audio_buffer_util_format)
{
  if(synth_util == NULL){
    return;
  }

  synth_util->audio_buffer_util_format = audio_buffer_util_format;
}

/**
 * ags_synth_util_get_samplerate:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Get samplerate of @synth_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.9.3
 */
guint
ags_synth_util_get_samplerate(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL){
    return(0);
  }

  return(synth_util->samplerate);
}

/**
 * ags_synth_util_set_samplerate:
 * @synth_util: the #AgsSynthUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @synth_util.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_set_samplerate(AgsSynthUtil *synth_util,
			      guint samplerate)
{
  if(synth_util == NULL){
    return;
  }

  synth_util->samplerate = samplerate;
}

/**
 * ags_synth_util_get_synth_oscillator_mode:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Get synth oscillator mode of @synth_util.
 * 
 * Returns: the synth oscillator mode
 * 
 * Since: 3.9.3
 */
guint
ags_synth_util_get_synth_oscillator_mode(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL){
    return(0);
  }

  return(synth_util->synth_oscillator_mode);
}

/**
 * ags_synth_util_set_synth_oscillator_mode:
 * @synth_util: the #AgsSynthUtil-struct
 * @synth_oscillator_mode: the synth oscillator mode
 *
 * Set @synth_oscillator_mode of @synth_util.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_set_synth_oscillator_mode(AgsSynthUtil *synth_util,
					 guint synth_oscillator_mode)
{
  if(synth_util == NULL){
    return;
  }

  synth_util->synth_oscillator_mode = synth_oscillator_mode;
}

/**
 * ags_synth_util_get_frequency:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Get frequency of @synth_util.
 * 
 * Returns: the frequency
 * 
 * Since: 3.9.3
 */
gdouble
ags_synth_util_get_frequency(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL){
    return(1.0);
  }

  return(synth_util->frequency);
}

/**
 * ags_synth_util_set_frequency:
 * @synth_util: the #AgsSynthUtil-struct
 * @frequency: the frequency
 *
 * Set @frequency of @synth_util.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_set_frequency(AgsSynthUtil *synth_util,
			     gdouble frequency)
{
  if(synth_util == NULL){
    return;
  }

  synth_util->frequency = frequency;
}

/**
 * ags_synth_util_get_phase:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Get phase of @synth_util.
 * 
 * Returns: the phase
 * 
 * Since: 3.9.3
 */
gdouble
ags_synth_util_get_phase(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL){
    return(1.0);
  }

  return(synth_util->phase);
}

/**
 * ags_synth_util_set_phase:
 * @synth_util: the #AgsSynthUtil-struct
 * @phase: the phase
 *
 * Set @phase of @synth_util.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_set_phase(AgsSynthUtil *synth_util,
			 gdouble phase)
{
  if(synth_util == NULL){
    return;
  }

  synth_util->phase = phase;
}

/**
 * ags_synth_util_get_volume:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Get volume of @synth_util.
 * 
 * Returns: the volume
 * 
 * Since: 3.9.3
 */
gdouble
ags_synth_util_get_volume(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL){
    return(1.0);
  }

  return(synth_util->volume);
}

/**
 * ags_synth_util_set_volume:
 * @synth_util: the #AgsSynthUtil-struct
 * @volume: the volume
 *
 * Set @volume of @synth_util.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_set_volume(AgsSynthUtil *synth_util,
			  gdouble volume)
{
  if(synth_util == NULL){
    return;
  }

  synth_util->volume = volume;
}

/**
 * ags_synth_util_get_frame_count:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Get frame count of @synth_util.
 * 
 * Returns: the frame count
 * 
 * Since: 3.9.3
 */
guint
ags_synth_util_get_frame_count(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL){
    return(0);
  }

  return(synth_util->frame_count);
}

/**
 * ags_synth_util_set_frame_count:
 * @synth_util: the #AgsSynthUtil-struct
 * @frame_count: the frame count
 *
 * Set @frame_count of @synth_util.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_set_frame_count(AgsSynthUtil *synth_util,
			       guint frame_count)
{
  if(synth_util == NULL){
    return;
  }

  synth_util->frame_count = frame_count;
}

/**
 * ags_synth_util_get_offset:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Get offset of @synth_util.
 * 
 * Returns: the offset
 * 
 * Since: 3.9.3
 */
guint
ags_synth_util_get_offset(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL){
    return(0);
  }

  return(synth_util->offset);
}

/**
 * ags_synth_util_set_offset:
 * @synth_util: the #AgsSynthUtil-struct
 * @offset: the offset
 *
 * Set @offset of @synth_util.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_set_offset(AgsSynthUtil *synth_util,
			  guint offset)
{
  if(synth_util == NULL){
    return;
  }

  synth_util->offset = offset;
}

/**
 * ags_synth_util_get_xcross_count_s8:
 * @buffer: the buffer containing audio data
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 3.0.0
 */
guint
ags_synth_util_get_xcross_count_s8(gint8 *buffer,
				   guint buffer_size)
{
  guint count;
  guint i;
  gboolean negative;

  if(buffer == NULL){
    return(0);
  }
  
  count = 0;

  if(buffer[0] > 0){
    negative = FALSE;
  }else{
    negative = TRUE;
  }

  for(i = 1; i < buffer_size; i++){
    if(negative &&
       buffer[i] > 0){
      count++;

      negative = FALSE;
    }else if(!negative &&
	     buffer[i] < 0){
      count++;

      negative = TRUE;
    }
  }
  
  return(count);
}

/**
 * ags_synth_util_get_xcross_count_s16:
 * @buffer: the buffer containing audio data
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 3.0.0
 */
guint
ags_synth_util_get_xcross_count_s16(gint16 *buffer,
				    guint buffer_size)
{
  guint count;
  guint i;
  gboolean negative;

  if(buffer == NULL){
    return(0);
  }
  
  count = 0;

  if(buffer[0] > 0){
    negative = FALSE;
  }else{
    negative = TRUE;
  }

  for(i = 1; i < buffer_size; i++){
    if(negative &&
       buffer[i] > 0){
      count++;

      negative = FALSE;
    }else if(!negative &&
	     buffer[i] < 0){
      count++;

      negative = TRUE;
    }
  }
  
  return(count);
}

/**
 * ags_synth_util_get_xcross_count_s24:
 * @buffer: the buffer containing audio data
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 3.0.0
 */
guint
ags_synth_util_get_xcross_count_s24(gint32 *buffer,
				    guint buffer_size)
{
  guint count;
  guint i;
  gboolean negative;

  if(buffer == NULL){
    return(0);
  }
  
  count = 0;

  if(buffer[0] > 0){
    negative = FALSE;
  }else{
    negative = TRUE;
  }

  for(i = 1; i < buffer_size; i++){
    if(negative &&
       buffer[i] > 0){
      count++;

      negative = FALSE;
    }else if(!negative &&
	     buffer[i] < 0){
      count++;

      negative = TRUE;
    }
  }
  
  return(count);
}

/**
 * ags_synth_util_get_xcross_count_s32:
 * @buffer: the buffer containing audio data
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 3.0.0
 */
guint
ags_synth_util_get_xcross_count_s32(gint32 *buffer,
				    guint buffer_size)
{
  guint count;
  guint i;
  gboolean negative;

  if(buffer == NULL){
    return(0);
  }
  
  count = 0;

  if(buffer[0] > 0){
    negative = FALSE;
  }else{
    negative = TRUE;
  }

  for(i = 1; i < buffer_size; i++){
    if(negative &&
       buffer[i] > 0){
      count++;

      negative = FALSE;
    }else if(!negative &&
	     buffer[i] < 0){
      count++;

      negative = TRUE;
    }
  }
  
  return(count);
}

/**
 * ags_synth_util_get_xcross_count_s64:
 * @buffer: the buffer containing audio data
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 3.0.0
 */
guint
ags_synth_util_get_xcross_count_s64(gint64 *buffer,
				    guint buffer_size)
{
  guint count;
  guint i;
  gboolean negative;

  if(buffer == NULL){
    return(0);
  }
  
  count = 0;

  if(buffer[0] > 0){
    negative = FALSE;
  }else{
    negative = TRUE;
  }

  for(i = 1; i < buffer_size; i++){
    if(negative &&
       buffer[i] > 0){
      count++;

      negative = FALSE;
    }else if(!negative &&
	     buffer[i] < 0){
      count++;

      negative = TRUE;
    }
  }
  
  return(count);
}

/**
 * ags_synth_util_get_xcross_count_float:
 * @buffer: the buffer containing audio data
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 3.0.0
 */
guint
ags_synth_util_get_xcross_count_float(gfloat *buffer,
				      guint buffer_size)
{
  guint count;
  guint i;
  gboolean negative;

  if(buffer == NULL){
    return(0);
  }
  
  count = 0;

  if(buffer[0] > 0.0){
    negative = FALSE;
  }else{
    negative = TRUE;
  }

  for(i = 1; i < buffer_size; i++){
    if(negative &&
       buffer[i] > 0.0){
      count++;

      negative = FALSE;
    }else if(!negative &&
	     buffer[i] < 0.0){
      count++;

      negative = TRUE;
    }
  }
  
  return(count);
}

/**
 * ags_synth_util_get_xcross_count_double:
 * @buffer: the buffer containing audio data
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 3.0.0
 */
guint
ags_synth_util_get_xcross_count_double(gdouble *buffer,
				       guint buffer_size)
{
  guint count;

  guint i;
  gboolean negative;

  if(buffer == NULL){
    return(0);
  }
  
  count = 0;

  if(buffer[0] > 0){
    negative = FALSE;
  }else{
    negative = TRUE;
  }

  for(i = 1; i < buffer_size; i++){
    if(negative &&
       buffer[i] > 0.0){
      count++;

      negative = FALSE;
    }else if(!negative &&
	     buffer[i] < 0.0){
      count++;

      negative = TRUE;
    }
  }
  
  return(count);
}

/**
 * ags_synth_util_get_xcross_count_complex:
 * @buffer: the buffer containing audio data
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 3.0.0
 */
guint
ags_synth_util_get_xcross_count_complex(AgsComplex *buffer,
					guint buffer_size)
{
  gfloat **ptr_ptr;
  gfloat *ptr;

  gfloat value;
  guint count;
  complex z;
  
  guint i;
  gboolean negative;

  if(buffer == NULL){
    return(0);
  }
  
  count = 0;

  ptr = &value;    
  ptr_ptr = &ptr;

  AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_FLOAT_FRAME(buffer, 1,
						      0, buffer_size,
						      ptr_ptr);

  if(value > 0.0){
    negative = FALSE;
  }else{
    negative = TRUE;
  }

  for(i = 1; i < buffer_size; i++){
    AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_FLOAT_FRAME(buffer + i, 1,
							i, buffer_size,
							ptr_ptr);

    if(negative &&
       value > 0.0){
      count++;

      negative = FALSE;
    }else if(!negative &&
	     value < 0.0){
      count++;

      negative = TRUE;
    }
  }
  
  return(count);
}

/**
 * ags_synth_util_get_xcross_count:
 * @buffer: the buffer containing audio data
 * @audio_buffer_util_format: the audio buffer util format
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 3.0.0
 */
guint
ags_synth_util_get_xcross_count(void *buffer,
				guint audio_buffer_util_format,
				guint buffer_size)
{
  guint count;

  if(buffer == NULL){
    return(0);
  }

  count = 0;

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    count = ags_synth_util_get_xcross_count_s8((gint8 *) buffer,
					       buffer_size);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    count = ags_synth_util_get_xcross_count_s16((gint16 *) buffer,
						buffer_size);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    count = ags_synth_util_get_xcross_count_s24((gint32 *) buffer,
						buffer_size);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    count = ags_synth_util_get_xcross_count_s32((gint32 *) buffer,
						buffer_size);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    count = ags_synth_util_get_xcross_count_s64((gint64 *) buffer,
						buffer_size);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    count = ags_synth_util_get_xcross_count_float((gfloat *) buffer,
						  buffer_size);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    count = ags_synth_util_get_xcross_count_double((gdouble *) buffer,
						   buffer_size);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    count = ags_synth_util_get_xcross_count_complex((AgsComplex *) buffer,
						    buffer_size);
  }
  break;
  default:
  {
    g_warning("ags_synth_util_get_xcross_count() - unsupported format");
  }
  }
  
  return(count);
}

/**
 * ags_synth_util_compute_sin_s8:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sine synth of signed 8 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sin_s8(AgsSynthUtil *synth_util)
{
  gint8 *source, *tmp_source;
  
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 127.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_sine;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_sine = (ags_v8double) {
      sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate)
    };

    i++;

    v_sine *= volume;

    v_buffer += v_sine;

    *(source) = (gint8) v_buffer[0];
    *(source += synth_util->source_stride) = (gint8) v_buffer[1];
    *(source += synth_util->source_stride) = (gint8) v_buffer[2];
    *(source += synth_util->source_stride) = (gint8) v_buffer[3];
    *(source += synth_util->source_stride) = (gint8) v_buffer[4];
    *(source += synth_util->source_stride) = (gint8) v_buffer[5];
    *(source += synth_util->source_stride) = (gint8) v_buffer[6];
    *(source += synth_util->source_stride) = (gint8) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_sine[] = {
      sin((double) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate)};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_sine, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint8) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint8) ((gint16) (tmp_source)[0] + (gint16) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint8) ((gint16) source[0] + (gint16) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_sin_s16:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sine synth of signed 16 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sin_s16(AgsSynthUtil *synth_util)
{
  gint16 *source, *tmp_source;
  
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 32767.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_sine;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_sine = (ags_v8double) {
      sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate)
    };

    i++;

    v_sine *= volume;

    v_buffer += v_sine;

    *(source) = (gint16) v_buffer[0];
    *(source += synth_util->source_stride) = (gint16) v_buffer[1];
    *(source += synth_util->source_stride) = (gint16) v_buffer[2];
    *(source += synth_util->source_stride) = (gint16) v_buffer[3];
    *(source += synth_util->source_stride) = (gint16) v_buffer[4];
    *(source += synth_util->source_stride) = (gint16) v_buffer[5];
    *(source += synth_util->source_stride) = (gint16) v_buffer[6];
    *(source += synth_util->source_stride) = (gint16) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_sine[] = {
      sin((double) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate)};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_sine, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint16) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint16) ((gint32) (tmp_source)[0] + (gint32) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint16) ((gint32) source[0] + (gint32) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_sin_s24:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sine synth of signed 24 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sin_s24(AgsSynthUtil *synth_util)
{
  gint32 *source, *tmp_source;
  
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 8388607.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_sine;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_sine = (ags_v8double) {
      sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate)
    };

    i++;

    v_sine *= volume;

    v_buffer += v_sine;

    *(source) = (gint32) v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_sine[] = {
      sin((double) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate)};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_sine, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint32) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint32) ((gint32) (tmp_source)[0] + (gint32) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint32) ((gint32) source[0] + (gint32) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_sin_s32:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sine synth of signed 32 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sin_s32(AgsSynthUtil *synth_util)
{
  gint32 *source, *tmp_source;
  
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 214748363.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_sine;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_sine = (ags_v8double) {
      sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate)
    };

    i++;

    v_sine *= volume;

    v_buffer += v_sine;

    *(source) = (gint32) v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_sine[] = {
      sin((double) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate)};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_sine, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint32) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint32) ((gint64) (tmp_source)[0] + (gint64) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint32) ((gint64) source[0] + (gint64) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_sin_s64:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sine synth of signed 64 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sin_s64(AgsSynthUtil *synth_util)
{
  gint64 *source, *tmp_source;
  
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 9223372036854775807.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_sine;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_sine = (ags_v8double) {
      sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate)
    };

    i++;

    v_sine *= volume;

    v_buffer += v_sine;

    *(source) = (gint64) v_buffer[0];
    *(source += synth_util->source_stride) = (gint64) v_buffer[1];
    *(source += synth_util->source_stride) = (gint64) v_buffer[2];
    *(source += synth_util->source_stride) = (gint64) v_buffer[3];
    *(source += synth_util->source_stride) = (gint64) v_buffer[4];
    *(source += synth_util->source_stride) = (gint64) v_buffer[5];
    *(source += synth_util->source_stride) = (gint64) v_buffer[6];
    *(source += synth_util->source_stride) = (gint64) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_sine[] = {
      sin((double) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate)};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_sine, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint64) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint64) ((gint64) (tmp_source)[0] + (gint64) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint64) ((gint64) source[0] + (gint64) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_sin_float:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sine synth of float data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sin_float(AgsSynthUtil *synth_util)
{
  gfloat *source, *tmp_source;
  
  gdouble volume;
  guint i, i_stop;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_sine;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_sine = (ags_v8double) {
      sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate)
    };

    i++;

    v_sine *= volume;

    v_buffer += v_sine;

    *(source) = (gfloat) v_buffer[0];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[1];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[2];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[3];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[4];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[5];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[6];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_sine[] = {
      sin((double) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate)};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_sine, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gfloat) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gfloat) ((tmp_source)[0] + (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gfloat) (source[0] + (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_sin_double:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sine synth of double data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sin_double(AgsSynthUtil *synth_util)
{
  gdouble *source, *tmp_source;
  
  gdouble volume;
  guint i, i_stop;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_sine;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_sine = (ags_v8double) {
      sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate)
    };

    i++;

    v_sine *= volume;

    v_buffer += v_sine;

    *(source) = (gdouble) v_buffer[0];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[1];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[2];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[3];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[4];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[5];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[6];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_sine[] = {
      sin((double) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate)};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_sine, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gdouble) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gdouble) ((tmp_source)[0] + (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gdouble) (source[0] + (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_sin_complex:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sine synth of complex data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sin_complex(AgsSynthUtil *synth_util)
{
  AgsComplex *source;
  
  gdouble volume;
  guint i;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = synth_util->volume;
  
  i = 0;
  
  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    ags_complex_set(source,
		    (ags_complex_get(source) + (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume)));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_sin:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sine synth.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sin(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  switch(synth_util->audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_synth_util_compute_sin_s8(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_synth_util_compute_sin_s16(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_synth_util_compute_sin_s24(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_synth_util_compute_sin_s32(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_synth_util_compute_sin_s64(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_synth_util_compute_sin_float(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_synth_util_compute_sin_double(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_synth_util_compute_sin_complex(synth_util);
  }
  break;
  }
}

/**
 * ags_synth_util_sin_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sinus wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_sin_s8(gint8 *buffer,
		      gdouble freq, gdouble phase, gdouble volume,
		      guint samplerate,
		      guint offset, guint n_frames)
{
  static const gdouble scale = 127.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * scale * volume)));
  }
}

/**
 * ags_synth_util_sin_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sinus wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_sin_s16(gint16 *buffer,
		       gdouble freq, gdouble phase, gdouble volume,
		       guint samplerate,
		       guint offset, guint n_frames)
{
  static const gdouble scale = 32767.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * scale * volume)));
  }
}

/**
 * ags_synth_util_sin_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sinus wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_sin_s24(gint32 *buffer,
		       gdouble freq, gdouble phase, gdouble volume,
		       guint samplerate,
		       guint offset, guint n_frames)
{
  static const gdouble scale = 8388607.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * scale * volume)));
  }
}

/**
 * ags_synth_util_sin_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sinus wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_sin_s32(gint32 *buffer,
		       gdouble freq, gdouble phase, gdouble volume,
		       guint samplerate,
		       guint offset, guint n_frames)
{
  static const gdouble scale = 214748363.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * scale * volume)));
  }
}

/**
 * ags_synth_util_sin_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sinus wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_sin_s64(gint64 *buffer,
		       gdouble freq, gdouble phase, gdouble volume,
		       guint samplerate,
		       guint offset, guint n_frames)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint64) (0xffffffffffff & ((gint64) buffer[i] + (gint64) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * scale * volume)));
  }
}

/**
 * ags_synth_util_sin_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sinus wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_sin_float(gfloat *buffer,
			 gdouble freq, gdouble phase, gdouble volume,
			 guint samplerate,
			 guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (float) ((gdouble) buffer[i] + (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * volume));
  }
}

/**
 * ags_synth_util_sin_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sinus wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_sin_double(gdouble *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (double) ((gdouble) buffer[i] + (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * volume));
  }
}

/**
 * ags_synth_util_sin_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sinus wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_sin_complex(AgsComplex *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  double y;

  guint i_stop;
  guint i;

  if(buffer == NULL){
    return;
  }

  c_ptr = buffer;
  c_ptr_ptr = &c_ptr;
  
  i_stop = offset + n_frames;
  
  for(i = offset; i < i_stop; i++, c_ptr++){
    y = (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * volume);

    AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
  }
}

/**
 * ags_synth_util_sin:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sin wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_sin(void *buffer,
		   gdouble freq, gdouble phase, gdouble volume,
		   guint samplerate, guint audio_buffer_util_format,
		   guint offset, guint n_frames)
{
  if(buffer == NULL){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_synth_util_sin_s8((gint8 *) buffer,
			  freq, phase, volume,
			  samplerate,
			  offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_synth_util_sin_s16((gint16 *) buffer,
			   freq, phase, volume,
			   samplerate,
			   offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_synth_util_sin_s24((gint32 *) buffer,
			   freq, phase, volume,
			   samplerate,
			   offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_synth_util_sin_s32((gint32 *) buffer,
			   freq, phase, volume,
			   samplerate,
			   offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_synth_util_sin_s64((gint64 *) buffer,
			   freq, phase, volume,
			   samplerate,
			   offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_synth_util_sin_float((gfloat *) buffer,
			     freq, phase, volume,
			     samplerate,
			     offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_synth_util_sin_double((gdouble *) buffer,
			      freq, phase, volume,
			      samplerate,
			      offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_synth_util_sin_complex((AgsComplex *) buffer,
			       freq, phase, volume,
			       samplerate,
			       offset, n_frames);
  }
  break;
  default:
  {
    g_warning("ags_synth_util_sin() - unsupported format");
  }
  }
}

/**
 * ags_synth_util_compute_sawtooth_s8:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sawtooth synth of signed 8 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sawtooth_s8(AgsSynthUtil *synth_util)
{
  gint8 *source, *tmp_source;
  
  gdouble ratio;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 127.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  ratio = synth_util->frequency / synth_util->samplerate;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_sawtooth;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_sawtooth = (ags_v8double) {
      ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
    };

    i++;

    v_sawtooth *= volume;

    v_buffer += v_sawtooth;

    *(source) = (gint8) v_buffer[0];
    *(source += synth_util->source_stride) = (gint8) v_buffer[1];
    *(source += synth_util->source_stride) = (gint8) v_buffer[2];
    *(source += synth_util->source_stride) = (gint8) v_buffer[3];
    *(source += synth_util->source_stride) = (gint8) v_buffer[4];
    *(source += synth_util->source_stride) = (gint8) v_buffer[5];
    *(source += synth_util->source_stride) = (gint8) v_buffer[6];
    *(source += synth_util->source_stride) = (gint8) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_sawtooth[] = {
      ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0)};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_sawtooth, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint8) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint8) ((gint16) (tmp_source)[0] + (gint16) ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint8) ((gint16) source[0] + (gint16) ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_sawtooth_s16:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sawtooth synth of signed 16 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sawtooth_s16(AgsSynthUtil *synth_util)
{
  gint16 *source, *tmp_source;
  
  gdouble ratio;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 32767.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  ratio = synth_util->frequency / synth_util->samplerate;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_sawtooth;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_sawtooth = (ags_v8double) {
      ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
    };

    i++;

    v_sawtooth *= volume;

    v_buffer += v_sawtooth;

    *(source) = (gint16) v_buffer[0];
    *(source += synth_util->source_stride) = (gint16) v_buffer[1];
    *(source += synth_util->source_stride) = (gint16) v_buffer[2];
    *(source += synth_util->source_stride) = (gint16) v_buffer[3];
    *(source += synth_util->source_stride) = (gint16) v_buffer[4];
    *(source += synth_util->source_stride) = (gint16) v_buffer[5];
    *(source += synth_util->source_stride) = (gint16) v_buffer[6];
    *(source += synth_util->source_stride) = (gint16) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_sawtooth[] = {
      ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0)};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_sawtooth, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint16) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint16) ((gint32) (tmp_source)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint16) ((gint32) source[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_sawtooth_s24:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sawtooth synth of signed 24 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sawtooth_s24(AgsSynthUtil *synth_util)
{
  gint32 *source, *tmp_source;
  
  gdouble ratio;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 8388607.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  ratio = synth_util->frequency / synth_util->samplerate;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_sawtooth;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_sawtooth = (ags_v8double) {
      ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
    };

    i++;

    v_sawtooth *= volume;

    v_buffer += v_sawtooth;

    *(source) = (gint32) v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_sawtooth[] = {
      ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0)};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_sawtooth, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint32) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint32) ((gint32) (tmp_source)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint32) ((gint32) source[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_sawtooth_s32:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sawtooth synth of signed 32 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sawtooth_s32(AgsSynthUtil *synth_util)
{
  gint32 *source, *tmp_source;
  
  gdouble ratio;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 214748363.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  ratio = synth_util->frequency / synth_util->samplerate;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_sawtooth;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_sawtooth = (ags_v8double) {
      ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
    };

    i++;

    v_sawtooth *= volume;

    v_buffer += v_sawtooth;

    *(source) = (gint32) v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_sawtooth[] = {
      ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0)};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_sawtooth, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint32) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint32) ((gint64) (tmp_source)[0] + (gint64) ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint32) ((gint64) source[0] + (gint64) ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_sawtooth_s64:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sawtooth synth of signed 64 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sawtooth_s64(AgsSynthUtil *synth_util)
{
  gint64 *source, *tmp_source;
  
  gdouble ratio;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 9223372036854775807.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  ratio = synth_util->frequency / synth_util->samplerate;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_sawtooth;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_sawtooth = (ags_v8double) {
      ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
    };

    i++;

    v_sawtooth *= volume;

    v_buffer += v_sawtooth;

    *(source) = (gint64) v_buffer[0];
    *(source += synth_util->source_stride) = (gint64) v_buffer[1];
    *(source += synth_util->source_stride) = (gint64) v_buffer[2];
    *(source += synth_util->source_stride) = (gint64) v_buffer[3];
    *(source += synth_util->source_stride) = (gint64) v_buffer[4];
    *(source += synth_util->source_stride) = (gint64) v_buffer[5];
    *(source += synth_util->source_stride) = (gint64) v_buffer[6];
    *(source += synth_util->source_stride) = (gint64) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_sawtooth[] = {
      ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0)};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_sawtooth, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint64) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint64) ((gint64) (tmp_source)[0] + (gint64) ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint64) ((gint64) source[0] + (gint64) ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_sawtooth_float:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sawtooth synth of float data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sawtooth_float(AgsSynthUtil *synth_util)
{
  gfloat *source, *tmp_source;
  
  gdouble ratio;
  gdouble volume;
  guint i, i_stop;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  ratio = synth_util->frequency / synth_util->samplerate;

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_sawtooth;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_sawtooth = (ags_v8double) {
      ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
    };

    i++;

    v_sawtooth *= volume;

    v_buffer += v_sawtooth;

    *(source) = (gfloat) v_buffer[0];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[1];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[2];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[3];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[4];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[5];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[6];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_sawtooth[] = {
      ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0)};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_sawtooth, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gfloat) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gfloat) ((tmp_source)[0] + ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gfloat) (source[0] + ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_sawtooth_double:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sawtooth synth of double data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sawtooth_double(AgsSynthUtil *synth_util)
{
  gdouble *source, *tmp_source;
  
  gdouble ratio;
  gdouble volume;
  guint i, i_stop;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  ratio = synth_util->frequency / synth_util->samplerate;

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_sawtooth;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_sawtooth = (ags_v8double) {
      ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
    };

    i++;

    v_sawtooth *= volume;

    v_buffer += v_sawtooth;

    *(source) = (gdouble) v_buffer[0];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[1];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[2];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[3];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[4];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[5];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[6];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_sawtooth[] = {
      ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0)};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_sawtooth, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gdouble) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gdouble) ((tmp_source)[0] + ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gdouble) (source[0] + ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_sawtooth_complex:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sawtooth synth of complex data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sawtooth_complex(AgsSynthUtil *synth_util)
{
  AgsComplex *source;

  gdouble ratio;
  gdouble volume;
  guint i;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  ratio = synth_util->frequency / synth_util->samplerate;

  volume = synth_util->volume;
  
  i = 0;
  
  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    ags_complex_set(source,
		    (ags_complex_get(source) + ((fmod(((gdouble) (synth_util->offset + i) + synth_util->phase), ratio) * 2.0 * ratio) - 1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_sawtooth:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sawtooth synth.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sawtooth(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  switch(synth_util->audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_synth_util_compute_sawtooth_s8(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_synth_util_compute_sawtooth_s16(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_synth_util_compute_sawtooth_s24(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_synth_util_compute_sawtooth_s32(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_synth_util_compute_sawtooth_s64(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_synth_util_compute_sawtooth_float(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_synth_util_compute_sawtooth_double(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_synth_util_compute_sawtooth_complex(synth_util);
  }
  break;
  }
}

/**
 * ags_synth_util_sawtooth_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @volume: the volume of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_sawtooth_s8(gint8 *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  static const gdouble scale = 127.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_sawtooth_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @volume: the volume of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_sawtooth_s16(gint16 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 32767.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_sawtooth_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @volume: the volume of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_sawtooth_s24(gint32 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 8388607.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_sawtooth_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @volume: the volume of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_sawtooth_s32(gint32 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 214748363.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_sawtooth_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @volume: the volume of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_sawtooth_s64(gint64 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_sawtooth_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @volume: the volume of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_sawtooth_float(gfloat *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = ((double) buffer[i] + (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * volume));
  }
}

/**
 * ags_synth_util_sawtooth_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @volume: the volume of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_sawtooth_double(gdouble *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = ((double) buffer[i] + (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * volume));
  }
}

/**
 * ags_synth_util_sawtooth_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @volume: the volume of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_sawtooth_complex(AgsComplex *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames)
{
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  double y;

  guint i_stop;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  c_ptr = buffer;
  c_ptr_ptr = &c_ptr;

  i_stop = offset + n_frames;

  for(i = offset; i < i_stop; i++, c_ptr++){
    y = (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * volume);

    AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
  }
}

/**
 * ags_synth_util_sawtooth:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @volume: the volume of the sawtooth wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_sawtooth(void *buffer,
			gdouble freq, gdouble phase, gdouble volume,
			guint samplerate, guint audio_buffer_util_format,
			guint offset, guint n_frames)
{
  if(buffer == NULL){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_synth_util_sawtooth_s8((gint8 *) buffer,
			       freq, phase, volume,
			       samplerate,
			       offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_synth_util_sawtooth_s16((gint16 *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_synth_util_sawtooth_s24((gint32 *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_synth_util_sawtooth_s32((gint32 *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_synth_util_sawtooth_s64((gint64 *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_synth_util_sawtooth_float((gfloat *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_synth_util_sawtooth_double((gdouble *) buffer,
				   freq, phase, volume,
				   samplerate,
				   offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_synth_util_sawtooth_complex((AgsComplex *) buffer,
				    freq, phase, volume,
				    samplerate,
				    offset, n_frames);
  }
  break;
  default:
  {
    g_warning("ags_synth_util_sawtooth() - unsupported format");
  }
  }
}

/**
 * ags_synth_util_compute_triangle_s8:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute triangle synth of signed 8 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_triangle_s8(AgsSynthUtil *synth_util)
{
  gint8 *source, *tmp_source;

  gdouble ratio;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 127.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  ratio = synth_util->frequency / synth_util->samplerate;
  volume = scale * synth_util->volume;
  
  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_triangle;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_triangle = (ags_v8double) {
      (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 1) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 2) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 3) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 4) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 5) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 6) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 7) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
    };

    v_triangle *= volume;

    v_buffer += v_triangle;

    *(source) = (gint8) v_buffer[0];
    *(source += synth_util->source_stride) = (gint8) v_buffer[1];
    *(source += synth_util->source_stride) = (gint8) v_buffer[2];
    *(source += synth_util->source_stride) = (gint8) v_buffer[3];
    *(source += synth_util->source_stride) = (gint8) v_buffer[4];
    *(source += synth_util->source_stride) = (gint8) v_buffer[5];
    *(source += synth_util->source_stride) = (gint8) v_buffer[6];
    *(source += synth_util->source_stride) = (gint8) v_buffer[7];

    source += synth_util->source_stride;
    
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_triangle[] = {
      (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 1) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 2) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 3) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 4) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 5) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 6) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 7) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume)};
    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_triangle, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint8) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[7];

    source += synth_util->source_stride;

    i += 8;    
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    source[0] = (gint8) ((gint16) source[0] + (gint16) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint8) ((gint16) source[0] + (gint16) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint8) ((gint16) source[0] + (gint16) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint8) ((gint16) source[0] + (gint16) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint8) ((gint16) source[0] + (gint16) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint8) ((gint16) source[0] + (gint16) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint8) ((gint16) source[0] + (gint16) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint8) ((gint16) source[0] + (gint16) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;    
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint8) ((gint16) source[0] + (gint16) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_triangle_s16:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute triangle synth of signed 16 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_triangle_s16(AgsSynthUtil *synth_util)
{
  gint16 *source, *tmp_source;

  gdouble ratio;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 32767.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  ratio = synth_util->frequency / synth_util->samplerate;
  volume = scale * synth_util->volume;
  
  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_triangle;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_triangle = (ags_v8double) {
      (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 1) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 2) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 3) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 4) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 5) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 6) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 7) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
    };

    i++;

    v_triangle *= volume;

    v_buffer += v_triangle;

    *(source) = (gint16) v_buffer[0];
    *(source += synth_util->source_stride) = (gint16) v_buffer[1];
    *(source += synth_util->source_stride) = (gint16) v_buffer[2];
    *(source += synth_util->source_stride) = (gint16) v_buffer[3];
    *(source += synth_util->source_stride) = (gint16) v_buffer[4];
    *(source += synth_util->source_stride) = (gint16) v_buffer[5];
    *(source += synth_util->source_stride) = (gint16) v_buffer[6];
    *(source += synth_util->source_stride) = (gint16) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_triangle[] = {
      (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 1) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 2) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 3) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 4) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 5) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 6) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 7) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume)};
    double v_volume[] = {(double) volume};
    
    vDSP_vmulD(v_triangle, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint16) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[7];

    source += synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    source[0] = (gint16) ((gint32) source[0] + (gint32) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint16) ((gint32) source[0] + (gint32) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint16) ((gint32) source[0] + (gint32) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint16) ((gint32) source[0] + (gint32) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint16) ((gint32) source[0] + (gint32) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint16) ((gint32) source[0] + (gint32) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint16) ((gint32) source[0] + (gint32) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint16) ((gint32) source[0] + (gint32) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;    
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint16) ((gint32) source[0] + (gint32) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_triangle_s24:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute triangle synth of signed 24 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_triangle_s24(AgsSynthUtil *synth_util)
{
  gint32 *source, *tmp_source;

  gdouble ratio;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 8388607.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  ratio = synth_util->frequency / synth_util->samplerate;
  volume = scale * synth_util->volume;
  
  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_triangle;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_triangle = (ags_v8double) {
      (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 1) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 2) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 3) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 4) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 5) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 6) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 7) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
    };

    v_triangle *= volume;

    v_buffer += v_triangle;

    *(source) = (gint32) v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) v_buffer[7];

    source += synth_util->source_stride;    

    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_triangle[] = {
      (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 1) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 2) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 3) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 4) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 5) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 6) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + i + 7) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume)};
    double v_volume[] = {(double) volume};
    
    vDSP_vmulD(v_triangle, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint32) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    source[0] = (gint32) ((gint32) source[0] + (gint32) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint32) source[0] + (gint32) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint32) source[0] + (gint32) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint32) source[0] + (gint32) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint32) source[0] + (gint32) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint32) source[0] + (gint32) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint32) source[0] + (gint32) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint32) source[0] + (gint32) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;    
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint32) ((gint32) source[0] + (gint32) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_triangle_s32:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute triangle synth of signed 32 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_triangle_s32(AgsSynthUtil *synth_util)
{
  gint32 *source, *tmp_source;

  gdouble ratio;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 214748363.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  ratio = synth_util->frequency / synth_util->samplerate;
  volume = scale * synth_util->volume;
  
  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_triangle;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_triangle = (ags_v8double) {
      (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
    };

    i++;

    v_triangle *= volume;

    v_buffer += v_triangle;

    *(source) = (gint32) v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_triangle[] = {
      (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume)};
    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_triangle, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint32) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    source[0] = (gint32) ((gint64) source[0] + (gint64) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint64) source[0] + (gint64) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint64) source[0] + (gint64) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint64) source[0] + (gint64) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint64) source[0] + (gint64) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint64) source[0] + (gint64) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint64) source[0] + (gint64) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint64) source[0] + (gint64) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;    
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint32) ((gint64) source[0] + (gint64) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_triangle_s64:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute triangle synth of signed 64 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_triangle_s64(AgsSynthUtil *synth_util)
{
  gint64 *source, *tmp_source;

  gdouble ratio;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 9223372036854775807.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  ratio = synth_util->frequency / synth_util->samplerate;
  volume = scale * synth_util->volume;
  
  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_triangle;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_triangle = (ags_v8double) {
      (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
    };

    i++;

    v_triangle *= volume;

    v_buffer += v_triangle;

    *(source) = (gint64) v_buffer[0];
    *(source += synth_util->source_stride) = (gint64) v_buffer[1];
    *(source += synth_util->source_stride) = (gint64) v_buffer[2];
    *(source += synth_util->source_stride) = (gint64) v_buffer[3];
    *(source += synth_util->source_stride) = (gint64) v_buffer[4];
    *(source += synth_util->source_stride) = (gint64) v_buffer[5];
    *(source += synth_util->source_stride) = (gint64) v_buffer[6];
    *(source += synth_util->source_stride) = (gint64) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_triangle[] = {
      (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume)};
    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_triangle, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint64) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    source[0] = (gint64) ((gint64) source[0] + (gint64) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint64) ((gint64) source[0] + (gint64) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint64) ((gint64) source[0] + (gint64) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint64) ((gint64) source[0] + (gint64) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint64) ((gint64) source[0] + (gint64) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint64) ((gint64) source[0] + (gint64) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint64) ((gint64) source[0] + (gint64) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint64) ((gint64) source[0] + (gint64) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;    
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint64) ((gint64) source[0] + (gint64) (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_triangle_float:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute triangle synth of float data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_triangle_float(AgsSynthUtil *synth_util)
{
  gfloat *source, *tmp_source;

  gdouble ratio;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 9223372036854775807.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  ratio = synth_util->frequency / synth_util->samplerate;
  volume = scale * synth_util->volume;
  
  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_triangle;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_triangle = (ags_v8double) {
      (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
    };

    i++;

    v_triangle *= volume;

    v_buffer += v_triangle;

    *(source) = (gfloat) v_buffer[0];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[1];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[2];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[3];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[4];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[5];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[6];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_triangle[] = {
      (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume)};
    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_triangle, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gfloat) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    source[0] = (gfloat) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gfloat) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gfloat) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gfloat) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gfloat) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gfloat) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gfloat) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gfloat) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;    
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gfloat) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_triangle_double:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute triangle synth of double data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_triangle_double(AgsSynthUtil *synth_util)
{
  gdouble *source, *tmp_source;

  gdouble ratio;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 9223372036854775807.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  ratio = synth_util->frequency / synth_util->samplerate;
  volume = scale * synth_util->volume;
  
  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_triangle;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_triangle = (ags_v8double) {
      (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
    };

    i++;

    v_triangle *= volume;

    v_buffer += v_triangle;

    *(source) = (gdouble) v_buffer[0];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[1];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[2];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[3];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[4];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[5];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[6];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_triangle[] = {
      (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume),
      (((((synth_util->offset + (i++)) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume)};
    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_triangle, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gdouble) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    source[0] = (gdouble) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gdouble) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gdouble) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gdouble) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gdouble) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gdouble) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gdouble) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gdouble) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;    
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gdouble) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_triangle_complex:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute triangle synth of complex data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_triangle_complex(AgsSynthUtil *synth_util)
{
  AgsComplex *source;

  gdouble ratio;
  gdouble volume;
  guint i;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  ratio = synth_util->frequency / synth_util->samplerate;

  volume = synth_util->volume;
  
  i = 0;
  
  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    ags_complex_set(source,
		    (ags_complex_get(source) + (((((synth_util->offset + i) + synth_util->phase) * ratio * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * ratio)) / 2.0) * 2.0) - 1.0) * volume)));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_triangle:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute triangle synth.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_triangle(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  switch(synth_util->audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_synth_util_compute_triangle_s8(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_synth_util_compute_triangle_s16(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_synth_util_compute_triangle_s24(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_synth_util_compute_triangle_s32(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_synth_util_compute_triangle_s64(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_synth_util_compute_triangle_float(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_synth_util_compute_triangle_double(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_synth_util_compute_triangle_complex(synth_util);
  }
  break;
  }
}

/**
 * ags_synth_util_triangle_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @volume: the volume of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_triangle_s8(gint8 *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  static const gdouble scale = 127.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_triangle_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @volume: the volume of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_triangle_s16(gint16 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 32767.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_triangle_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @volume: the volume of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_triangle_s24(gint32 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 8388607.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_triangle_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @volume: the volume of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_triangle_s32(gint32 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 214748363.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_triangle_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @volume: the volume of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_triangle_s64(gint64 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_triangle_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @volume: the volume of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_triangle_float(gfloat *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = ((double) buffer[i] + (double) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * volume));
  }
}

/**
 * ags_synth_util_triangle_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @volume: the volume of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_triangle_double(gdouble *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = ((double) buffer[i] + (double) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * volume));
  }
}

/**
 * ags_synth_util_triangle_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @volume: the volume of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_triangle_complex(AgsComplex *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames)
{
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  double y;

  guint i_stop;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  c_ptr = buffer;
  c_ptr_ptr = &c_ptr;
  
  i_stop = offset + n_frames;

  for(i = offset; i < i_stop; i++, c_ptr++){
    y = (double) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * volume);

    AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
  }
}

/**
 * ags_synth_util_triangle:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @volume: the volume of the triangle wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_triangle(void *buffer,
			gdouble freq, gdouble phase, gdouble volume,
			guint samplerate, guint audio_buffer_util_format,
			guint offset, guint n_frames)
{
  if(buffer == NULL){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_synth_util_triangle_s8((gint8 *) buffer,
			       freq, phase, volume,
			       samplerate,
			       offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_synth_util_triangle_s16((gint16 *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_synth_util_triangle_s24((gint32 *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_synth_util_triangle_s32((gint32 *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_synth_util_triangle_s64((gint64 *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_synth_util_triangle_float((gfloat *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_synth_util_triangle_double((gdouble *) buffer,
				   freq, phase, volume,
				   samplerate,
				   offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_synth_util_triangle_complex((AgsComplex *) buffer,
				    freq, phase, volume,
				    samplerate,
				    offset, n_frames);
  }
  break;
  default:
  {
    g_warning("ags_synth_util_triangle() - unsupported format");
  }
  }
}

/**
 * ags_synth_util_compute_square_s8:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute square synth of signed 8 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_square_s8(AgsSynthUtil *synth_util)
{
  gint8 *source, *tmp_source;

  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 127.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_square;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_square = (ags_v8double) {
      ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
    };

    i++;

    v_square *= volume;

    v_buffer += v_square;

    *(source) = (gint8) v_buffer[0];
    *(source += synth_util->source_stride) = (gint8) v_buffer[1];
    *(source += synth_util->source_stride) = (gint8) v_buffer[2];
    *(source += synth_util->source_stride) = (gint8) v_buffer[3];
    *(source += synth_util->source_stride) = (gint8) v_buffer[4];
    *(source += synth_util->source_stride) = (gint8) v_buffer[5];
    *(source += synth_util->source_stride) = (gint8) v_buffer[6];
    *(source += synth_util->source_stride) = (gint8) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_square[] = {
      ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_square, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint8) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint8) ((gint16) (tmp_source)[0] + (gint16) ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint8) ((gint16) source[0] + (gint16) ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_square_s16:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute square synth of signed 16 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_square_s16(AgsSynthUtil *synth_util)
{
  gint16 *source, *tmp_source;

  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 32767.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_square;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_square = (ags_v8double) {
      ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
    };

    i++;

    v_square *= volume;

    v_buffer += v_square;

    *(source) = (gint16) v_buffer[0];
    *(source += synth_util->source_stride) = (gint16) v_buffer[1];
    *(source += synth_util->source_stride) = (gint16) v_buffer[2];
    *(source += synth_util->source_stride) = (gint16) v_buffer[3];
    *(source += synth_util->source_stride) = (gint16) v_buffer[4];
    *(source += synth_util->source_stride) = (gint16) v_buffer[5];
    *(source += synth_util->source_stride) = (gint16) v_buffer[6];
    *(source += synth_util->source_stride) = (gint16) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_square[] = {
      ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_square, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint16) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint16) ((gint32) (tmp_source)[0] + (gint32) ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint16) ((gint32) source[0] + (gint32) ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_square_s24:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute square synth of signed 24 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_square_s24(AgsSynthUtil *synth_util)
{
  gint32 *source, *tmp_source;

  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 8388607.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_square;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_square = (ags_v8double) {
      ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
    };

    i++;

    v_square *= volume;

    v_buffer += v_square;

    *(source) = (gint32) v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_square[] = {
      ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_square, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint32) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint32) ((gint32) (tmp_source)[0] + (gint32) ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint32) ((gint32) source[0] + (gint32) ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_square_s32:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute square synth of signed 32 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_square_s32(AgsSynthUtil *synth_util)
{
  gint32 *source, *tmp_source;

  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 214748363.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_square;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_square = (ags_v8double) {
      ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
    };

    i++;

    v_square *= volume;

    v_buffer += v_square;

    *(source) = (gint32) v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_square[] = {
      ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_square, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint32) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint32) ((gint64) (tmp_source)[0] + (gint64) ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint32) ((gint64) source[0] + (gint64) ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_square_s64:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute square synth of signed 64 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_square_s64(AgsSynthUtil *synth_util)
{
  gint64 *source, *tmp_source;

  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 9223372036854775807.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_square;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_square = (ags_v8double) {
      ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
    };

    i++;

    v_square *= volume;

    v_buffer += v_square;

    *(source) = (gint64) v_buffer[0];
    *(source += synth_util->source_stride) = (gint64) v_buffer[1];
    *(source += synth_util->source_stride) = (gint64) v_buffer[2];
    *(source += synth_util->source_stride) = (gint64) v_buffer[3];
    *(source += synth_util->source_stride) = (gint64) v_buffer[4];
    *(source += synth_util->source_stride) = (gint64) v_buffer[5];
    *(source += synth_util->source_stride) = (gint64) v_buffer[6];
    *(source += synth_util->source_stride) = (gint64) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_square[] = {
      ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_square, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint64) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint64) ((gint64) (tmp_source)[0] + (gint64) ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint64) ((gint64) source[0] + (gint64) ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_square_float:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute square synth of float data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_square_float(AgsSynthUtil *synth_util)
{
  gfloat *source, *tmp_source;

  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 127.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_square;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_square = (ags_v8double) {
      ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
    };

    i++;

    v_square *= volume;

    v_buffer += v_square;

    *(source) = (gfloat) v_buffer[0];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[1];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[2];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[3];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[4];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[5];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[6];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_square[] = {
      ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_square, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gfloat) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gfloat) ((tmp_source)[0] + ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gfloat) (source[0] + ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_square_double:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute square synth of double data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_square_double(AgsSynthUtil *synth_util)
{
  gdouble *source, *tmp_source;

  gdouble volume;
  guint i, i_stop;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_square;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_square = (ags_v8double) {
      ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
    };

    i++;

    v_square *= volume;

    v_buffer += v_square;

    *(source) = (gdouble) v_buffer[0];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[1];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[2];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[3];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[4];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[5];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[6];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_square[] = {
      ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_square, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gdouble) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gdouble) ((tmp_source)[0] + ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gdouble) (source[0] + ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_square_complex:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute square synth of complex data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_square_complex(AgsSynthUtil *synth_util)
{
  AgsComplex *source;

  gdouble volume;
  guint i;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = synth_util->volume;

  i = 0;
  
  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    ags_complex_set(source,
		    (gdouble) (ags_complex_get(source) + ((sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume)));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_square:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute square synth.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_square(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  switch(synth_util->audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_synth_util_compute_square_s8(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_synth_util_compute_square_s16(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_synth_util_compute_square_s24(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_synth_util_compute_square_s32(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_synth_util_compute_square_s64(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_synth_util_compute_square_float(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_synth_util_compute_square_double(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_synth_util_compute_square_complex(synth_util);
  }
  break;
  }
}

/**
 * ags_synth_util_square_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @volume: the volume of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate square wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_square_s8(gint8 *buffer,
			 gdouble freq, gdouble phase, gdouble volume,
			 guint samplerate,
			 guint offset, guint n_frames)
{
  static const gdouble scale = 127.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_square_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @volume: the volume of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate square wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_square_s16(gint16 *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames)
{
  static const gdouble scale = 32767.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_square_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @volume: the volume of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate square wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_square_s24(gint32 *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames)
{
  static const gdouble scale = 8388607.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_square_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @volume: the volume of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate square wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_square_s32(gint32 *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames)
{
  static const gdouble scale = 214748363.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_square_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @volume: the volume of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate square wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_square_s64(gint64 *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_square_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @volume: the volume of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate square wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_square_float(gfloat *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (buffer[i] + (1.0 * volume));
    }else{
      buffer[i] = (buffer[i] + (-1.0 * volume));
    }
  }
}

/**
 * ags_synth_util_square_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @volume: the volume of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate square wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_square_double(gdouble *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate,
			     guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (buffer[i] + (1.0 * volume));
    }else{
      buffer[i] = (buffer[i] + (-1.0 * volume));
    }
  }
}

/**
 * ags_synth_util_square_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @volume: the volume of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate square wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_square_complex(AgsComplex *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames)
{
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  double y;

  guint i_stop;
  guint i;

  if(buffer == NULL){
    return;
  }

  c_ptr = buffer;
  c_ptr_ptr = &c_ptr;

  i_stop = offset + n_frames;

  for(i = offset; i < i_stop; i++, c_ptr++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      y = (1.0 * volume);
    }else{
      y = (-1.0 * volume);
    }    

    AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
  }
}

/**
 * ags_synth_util_square:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @volume: the volume of the square wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate square wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_square(void *buffer,
		      gdouble freq, gdouble phase, gdouble volume,
		      guint samplerate, guint audio_buffer_util_format,
		      guint offset, guint n_frames)
{
  if(buffer == NULL){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_synth_util_square_s8((gint8 *) buffer,
			     freq, phase, volume,
			     samplerate,
			     offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_synth_util_square_s16((gint16 *) buffer,
			      freq, phase, volume,
			      samplerate,
			      offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_synth_util_square_s24((gint32 *) buffer,
			      freq, phase, volume,
			      samplerate,
			      offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_synth_util_square_s32((gint32 *) buffer,
			      freq, phase, volume,
			      samplerate,
			      offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_synth_util_square_s64((gint64 *) buffer,
			      freq, phase, volume,
			      samplerate,
			      offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_synth_util_square_float((gfloat *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_synth_util_square_double((gdouble *) buffer,
				 freq, phase, volume,
				 samplerate,
				 offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_synth_util_square_complex((AgsComplex *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames);
  }
  break;
  default:
  {
    g_warning("ags_synth_util_square() - unsupported format");
  }
  }
}

/**
 * ags_synth_util_compute_impulse_s8:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute impulse synth of signed 8 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_impulse_s8(AgsSynthUtil *synth_util)
{
  gint8 *source, *tmp_source;

  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 127.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_impulse;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_impulse = (ags_v8double) {
      (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume
    };

    i++;

    v_impulse *= volume;

    v_buffer += v_impulse;

    *(source) = (gint8) v_buffer[0];
    *(source += synth_util->source_stride) = (gint8) v_buffer[1];
    *(source += synth_util->source_stride) = (gint8) v_buffer[2];
    *(source += synth_util->source_stride) = (gint8) v_buffer[3];
    *(source += synth_util->source_stride) = (gint8) v_buffer[4];
    *(source += synth_util->source_stride) = (gint8) v_buffer[5];
    *(source += synth_util->source_stride) = (gint8) v_buffer[6];
    *(source += synth_util->source_stride) = (gint8) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_impulse[] = {
      (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_impulse, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint8) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint8) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint8) ((gint16) (tmp_source)[0] + (gint16) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint8) ((gint16) source[0] + (gint16) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_impulse_s16:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute impulse synth of signed 16 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_impulse_s16(AgsSynthUtil *synth_util)
{
  gint16 *source, *tmp_source;

  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 32767.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_impulse;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_impulse = (ags_v8double) {
      (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume
    };

    i++;

    v_impulse *= volume;

    v_buffer += v_impulse;

    *(source) = (gint16) v_buffer[0];
    *(source += synth_util->source_stride) = (gint16) v_buffer[1];
    *(source += synth_util->source_stride) = (gint16) v_buffer[2];
    *(source += synth_util->source_stride) = (gint16) v_buffer[3];
    *(source += synth_util->source_stride) = (gint16) v_buffer[4];
    *(source += synth_util->source_stride) = (gint16) v_buffer[5];
    *(source += synth_util->source_stride) = (gint16) v_buffer[6];
    *(source += synth_util->source_stride) = (gint16) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_impulse[] = {
      (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_impulse, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint16) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint16) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint16) ((gint32) (tmp_source)[0] + (gint32) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint16) ((gint32) source[0] + (gint32) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_impulse_s24:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute impulse synth of signed 24 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_impulse_s24(AgsSynthUtil *synth_util)
{
  gint32 *source, *tmp_source;

  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 8388607.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_impulse;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_impulse = (ags_v8double) {
      (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume
    };

    i++;

    v_impulse *= volume;

    v_buffer += v_impulse;

    *(source) = (gint32) v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_impulse[] = {
      (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_impulse, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint32) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint32) ((gint32) (tmp_source)[0] + (gint32) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint32) ((gint32) source[0] + (gint32) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_impulse_s32:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute impulse synth of signed 32 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_impulse_s32(AgsSynthUtil *synth_util)
{
  gint32 *source, *tmp_source;

  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 214748363.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_impulse;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_impulse = (ags_v8double) {
      (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume
    };

    i++;

    v_impulse *= volume;

    v_buffer += v_impulse;

    *(source) = (gint32) v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_impulse[] = {
      (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_impulse, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint32) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint32) ((gint64) (tmp_source)[0] + (gint64) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint32) ((gint64) source[0] + (gint64) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_impulse_s64:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute impulse synth of signed 64 bit data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_impulse_s64(AgsSynthUtil *synth_util)
{
  gint64 *source, *tmp_source;

  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 9223372036854775807.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = scale * synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_impulse;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_impulse = (ags_v8double) {
      (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume
    };

    i++;

    v_impulse *= volume;

    v_buffer += v_impulse;

    *(source) = (gint64) v_buffer[0];
    *(source += synth_util->source_stride) = (gint64) v_buffer[1];
    *(source += synth_util->source_stride) = (gint64) v_buffer[2];
    *(source += synth_util->source_stride) = (gint64) v_buffer[3];
    *(source += synth_util->source_stride) = (gint64) v_buffer[4];
    *(source += synth_util->source_stride) = (gint64) v_buffer[5];
    *(source += synth_util->source_stride) = (gint64) v_buffer[6];
    *(source += synth_util->source_stride) = (gint64) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_impulse[] = {
      (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_impulse, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint64) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint64) ((gint64) (tmp_source)[0] + (gint64) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gint64) ((gint64) source[0] + (gint64) (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_impulse_float:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute impulse synth of float data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_impulse_float(AgsSynthUtil *synth_util)
{
  gfloat *source, *tmp_source;

  gdouble volume;
  guint i, i_stop;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_impulse;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_impulse = (ags_v8double) {
      (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume
    };

    i++;

    v_impulse *= volume;

    v_buffer += v_impulse;

    *(source) = (gfloat) v_buffer[0];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[1];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[2];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[3];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[4];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[5];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[6];
    *(source += synth_util->source_stride) = (gfloat) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_impulse[] = {
      (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_impulse, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gfloat) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gfloat) ((tmp_source)[0] + (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gfloat) (source[0] + (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_impulse_double:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute impulse synth of double data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_impulse_double(AgsSynthUtil *synth_util)
{
  gdouble *source, *tmp_source;

  gdouble volume;
  guint i, i_stop;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    ags_v8double v_buffer, v_impulse;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride),
      (gdouble) *(tmp_source += synth_util->source_stride)
    };
    
    v_impulse = (ags_v8double) {
      (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume
    };

    i++;

    v_impulse *= volume;

    v_buffer += v_impulse;

    *(source) = (gdouble) v_buffer[0];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[1];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[2];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[3];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[4];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[5];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[6];
    *(source += synth_util->source_stride) = (gdouble) v_buffer[7];

    source += synth_util->source_stride;    
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp_ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride),
      (double) *(tmp_source += synth_util->source_stride)};
    double v_impulse[] = {
      (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};

    i++;
    
    vDSP_vmulD(v_impulse, 1, v_volume, 0, tmp_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gdouble) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[7];

    source += synth_util->source_stride;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);

  if(synth_util->offset + i_stop > synth_util->frame_count){
    i_stop = (synth_util->frame_count - synth_util->offset) - ((synth_util->frame_count - synth_util->offset) % 8);
  }
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gdouble) ((tmp_source)[0] + (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
#endif

  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    source[0] = (gdouble) (source[0] + (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_impulse_complex:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute impulse synth of complex data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_impulse_complex(AgsSynthUtil *synth_util)
{
  AgsComplex *source;

  gdouble volume;
  guint i;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  volume = synth_util->volume;

  i = 0;
  
  for(; i < synth_util->buffer_length && synth_util->offset + i < synth_util->frame_count;){
    ags_complex_set(source,
		    (gdouble) (ags_complex_get(source) + (sin((gdouble) ((synth_util->offset + i) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_impulse:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute impulse synth.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_impulse(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  switch(synth_util->audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_synth_util_compute_impulse_s8(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_synth_util_compute_impulse_s16(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_synth_util_compute_impulse_s24(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_synth_util_compute_impulse_s32(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_synth_util_compute_impulse_s64(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_synth_util_compute_impulse_float(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_synth_util_compute_impulse_double(synth_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_synth_util_compute_impulse_complex(synth_util);
  }
  break;
  }
}

/**
 * ags_synth_util_impulse_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @volume: the volume of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_impulse_s8(gint8 *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames)
{
  static const gdouble scale = 127.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_impulse_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @volume: the volume of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_impulse_s16(gint16 *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  static const gdouble scale = 32767.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_impulse_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @volume: the volume of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_impulse_s24(gint32 *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  static const gdouble scale = 8388607.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_impulse_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @volume: the volume of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_impulse_s32(gint32 *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  static const gdouble scale = 214748363.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_impulse_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @volume: the volume of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_impulse_s64(gint64 *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_impulse_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @volume: the volume of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_impulse_float(gfloat *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate,
			     guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (buffer[i] + (1.0 * volume));
    }else{
      buffer[i] = (buffer[i] + (-1.0 * volume));
    }
  }
}

/**
 * ags_synth_util_impulse_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @volume: the volume of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_impulse_double(gdouble *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (buffer[i] + (1.0 * volume));
    }else{
      buffer[i] = (buffer[i] + (-1.0 * volume));
    }
  }
}

/**
 * ags_synth_util_impulse_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @volume: the volume of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_impulse_complex(AgsComplex *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames)
{
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  double y;

  guint i_stop;
  guint i;

  if(buffer == NULL){
    return;
  }

  c_ptr = buffer;
  c_ptr_ptr = &c_ptr;

  i_stop = offset + n_frames;

  for(i = offset; i < i_stop; i++, c_ptr++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      y = (1.0 * volume);
    }else{
      y = (-1.0 * volume);
    }

    AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
  }
}

/**
 * ags_synth_util_impulse:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @volume: the volume of the impulse wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_synth_util_impulse(void *buffer,
		       gdouble freq, gdouble phase, gdouble volume,
		       guint samplerate, guint audio_buffer_util_format,
		       guint offset, guint n_frames)
{
  if(buffer == NULL){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_synth_util_impulse_s8((gint8 *) buffer,
			      freq, phase, volume,
			      samplerate,
			      offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_synth_util_impulse_s16((gint16 *) buffer,
			       freq, phase, volume,
			       samplerate,
			       offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_synth_util_impulse_s24((gint32 *) buffer,
			       freq, phase, volume,
			       samplerate,
			       offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_synth_util_impulse_s32((gint32 *) buffer,
			       freq, phase, volume,
			       samplerate,
			       offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_synth_util_impulse_s64((gint64 *) buffer,
			       freq, phase, volume,
			       samplerate,
			       offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_synth_util_impulse_float((gfloat *) buffer,
				 freq, phase, volume,
				 samplerate,
				 offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_synth_util_impulse_double((gdouble *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_synth_util_impulse_complex((AgsComplex *) buffer,
				   freq, phase, volume,
				   samplerate,
				   offset, n_frames);
  }
  break;
  default:
  {
    g_warning("ags_synth_util_impulse() - unsupported format");
  }
  }
}
