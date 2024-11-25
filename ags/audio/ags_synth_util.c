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

#include <ags/audio/ags_synth_util.h>

#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_fourier_transform_util.h>

#if defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#include <Accelerate/Accelerate.h>
#endif

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
  static gsize g_define_type_id__volatile = 0;

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

  ptr[0] = AGS_SYNTH_UTIL_INITIALIZER;

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

  g_return_val_if_fail(ptr != NULL, NULL);
  
  new_ptr = (AgsSynthUtil *) g_new(AgsSynthUtil,
				   1);
  
  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->synth_oscillator_mode = ptr->synth_oscillator_mode;

  new_ptr->frequency = ptr->frequency;
  new_ptr->phase = ptr->phase;
  new_ptr->volume = ptr->volume;

  new_ptr->frame_count = ptr->frame_count;
  new_ptr->offset = ptr->offset;

  new_ptr->note_256th_mode = ptr->note_256th_mode;

  new_ptr->offset_256th = ptr->offset_256th;
  
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
  g_return_if_fail(ptr != NULL);
  
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
 * @source: (transfer none): the source buffer
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
 * ags_synth_util_get_format:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Get audio buffer util format of @synth_util.
 * 
 * Returns: the audio buffer util format
 * 
 * Since: 3.14.0
 */
AgsSoundcardFormat
ags_synth_util_get_format(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL){
    return(0);
  }

  return(synth_util->format);
}

/**
 * ags_synth_util_set_format:
 * @synth_util: the #AgsSynthUtil-struct
 * @format: the audio buffer util format
 *
 * Set @format of @synth_util.
 *
 * Since: 3.14.0
 */
void
ags_synth_util_set_format(AgsSynthUtil *synth_util,
			  AgsSoundcardFormat format)
{
  if(synth_util == NULL){
    return;
  }

  synth_util->format = format;
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
 * ags_synth_util_get_note_256th_mode:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Get note 256th mode of @synth_util.
 * 
 * Returns: %TRUE if note 256th mode, otherwise %FALSE
 * 
 * Since: 6.1.0
 */
gboolean
ags_synth_util_get_note_256th_mode(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL){
    return(0);
  }

  return(synth_util->note_256th_mode);
}

/**
 * ags_synth_util_set_note_256th_mode:
 * @synth_util: the #AgsSynthUtil-struct
 * @note_256th_mode: the note 256th mode
 *
 * Set @note_256th_mode of @synth_util.
 *
 * Since: 6.1.0
 */
void
ags_synth_util_set_note_256th_mode(AgsSynthUtil *synth_util,
				   gboolean note_256th_mode)
{
  if(synth_util == NULL){
    return;
  }

  synth_util->note_256th_mode = note_256th_mode;
}

/**
 * ags_synth_util_get_offset_256th:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Get offset as note 256th of @synth_util.
 * 
 * Returns: the offset as note 256th
 * 
 * Since: 6.1.0
 */
guint
ags_synth_util_get_offset_256th(AgsSynthUtil *synth_util)
{
  if(synth_util == NULL){
    return(0);
  }

  return(synth_util->offset_256th);
}

/**
 * ags_synth_util_set_offset_256th:
 * @synth_util: the #AgsSynthUtil-struct
 * @offset_256th: the offset as note 256th
 *
 * Set @offset_256th of @synth_util.
 *
 * Since: 6.1.0
 */
void
ags_synth_util_set_offset_256th(AgsSynthUtil *synth_util,
				guint offset_256th)
{
  if(synth_util == NULL){
    return;
  }
  
  synth_util->offset_256th = offset_256th;
}

/**
 * ags_synth_util_get_xcross_count_s8:
 * @synth_util: the #AgsSynthUtil-struct
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
ags_synth_util_get_xcross_count_s8(AgsSynthUtil *synth_util,
				   gint8 *buffer,
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
 * @synth_util: the #AgsSynthUtil-struct
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
ags_synth_util_get_xcross_count_s16(AgsSynthUtil *synth_util,
				    gint16 *buffer,
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
 * @synth_util: the #AgsSynthUtil-struct
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
ags_synth_util_get_xcross_count_s24(AgsSynthUtil *synth_util,
				    gint32 *buffer,
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
 * @synth_util: the #AgsSynthUtil-struct
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
ags_synth_util_get_xcross_count_s32(AgsSynthUtil *synth_util,
				    gint32 *buffer,
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
 * @synth_util: the #AgsSynthUtil-struct
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
ags_synth_util_get_xcross_count_s64(AgsSynthUtil *synth_util,
				    gint64 *buffer,
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
 * @synth_util: the #AgsSynthUtil-struct
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
ags_synth_util_get_xcross_count_float(AgsSynthUtil *synth_util,
				      gfloat *buffer,
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
 * @synth_util: the #AgsSynthUtil-struct
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
ags_synth_util_get_xcross_count_double(AgsSynthUtil *synth_util,
				       gdouble *buffer,
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
 * @synth_util: the #AgsSynthUtil-struct
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
ags_synth_util_get_xcross_count_complex(AgsSynthUtil *synth_util,
					AgsComplex *buffer,
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
 * @synth_util: the #AgsSynthUtil-struct
 * @buffer: the buffer containing audio data
 * @format: the format
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 3.0.0
 */
guint
ags_synth_util_get_xcross_count(AgsSynthUtil *synth_util,
				void *buffer,
				guint format,
				guint buffer_size)
{
  guint count;

  if(buffer == NULL){
    return(0);
  }

  count = 0;

  switch(format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    count = ags_synth_util_get_xcross_count_s8(synth_util,
					       (gint8 *) buffer,
					       buffer_size);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    count = ags_synth_util_get_xcross_count_s16(synth_util,
						(gint16 *) buffer,
						buffer_size);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    count = ags_synth_util_get_xcross_count_s24(synth_util,
						(gint32 *) buffer,
						buffer_size);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    count = ags_synth_util_get_xcross_count_s32(synth_util,
						(gint32 *) buffer,
						buffer_size);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    count = ags_synth_util_get_xcross_count_s64(synth_util,
						(gint64 *) buffer,
						buffer_size);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    count = ags_synth_util_get_xcross_count_float(synth_util,
						  (gfloat *) buffer,
						  buffer_size);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    count = ags_synth_util_get_xcross_count_double(synth_util,
						   (gdouble *) buffer,
						   buffer_size);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    count = ags_synth_util_get_xcross_count_complex(synth_util,
						    (AgsComplex *) buffer,
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

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate)
    };
    
    v_sine *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate)};

    double v_scale[] = {(double) scale};    
    double v_volume[] = {(double) volume};
    
    vDSP_vmulD(v_sine, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint8) ((gint16) (tmp_source)[0] + (gint16) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint8) ((gint16) source[0] + (gint16) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
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

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate)
    };

    v_sine *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate)};

    double v_scale[] = {(double) scale};    
    double v_volume[] = {(double) volume};
    
    vDSP_vmulD(v_sine, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint16) ((gint32) (tmp_source)[0] + (gint32) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint16) ((gint32) source[0] + (gint32) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
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

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate)
    };

    v_sine *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate)};

    double v_scale[] = {(double) scale};    
    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_sine, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint32) ((gint32) (tmp_source)[0] + (gint32) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint32) ((gint32) source[0] + (gint32) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
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

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate)
    };

    v_sine *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate)};

    double v_scale[] = {(double) scale};    
    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_sine, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint32) ((gint64) (tmp_source)[0] + (gint64) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint32) ((gint64) source[0] + (gint64) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
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

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate)
    };

    v_sine *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate)};

    double v_scale[] = {(double) scale};    
    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_sine, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint64) ((gint64) (tmp_source)[0] + (gint64) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint64) ((gint64) source[0] + (gint64) (scale * sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
  }
}

/**
 * ags_synth_util_compute_sin_float:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sine synth of floating point data.
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
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate)
    };


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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate)};

    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_sine, 1, v_volume, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp0_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gfloat) ((tmp_source)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gfloat) (source[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
  }
}

/**
 * ags_synth_util_compute_sin_double:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sine synth of double precision floating point data.
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
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate),
      sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate)
    };


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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate),
      sin((double) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (double) synth_util->samplerate)};

    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_sine, 1, v_volume, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp0_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gdouble) ((tmp_source)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gdouble) (source[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume));

    source += synth_util->source_stride;
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
  
  for(; i < synth_util->buffer_length;){
    ags_complex_set(source,
		    (ags_complex_get(source) + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) * volume)));

    source += synth_util->source_stride;
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

  switch(synth_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_synth_util_compute_sin_s8(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_synth_util_compute_sin_s16(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_synth_util_compute_sin_s24(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_synth_util_compute_sin_s32(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_synth_util_compute_sin_s64(synth_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_synth_util_compute_sin_float(synth_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_synth_util_compute_sin_double(synth_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_synth_util_compute_sin_complex(synth_util);
  }
  break;
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
  
  gdouble freq;
  gdouble samplerate;
  gdouble freq_period;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 127.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  freq = synth_util->frequency;
  samplerate = (gdouble) synth_util->samplerate;
  freq_period = synth_util->samplerate / synth_util->frequency;

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
    };

    v_sawtooth *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0)};

    double v_scale[] = {(double) scale};    
    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_sawtooth, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint8) ((gint16) (tmp_source)[0] + (gint16) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint8) ((gint16) source[0] + (gint16) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);

    source += synth_util->source_stride;
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
  
  gdouble freq;
  gdouble samplerate;
  gdouble freq_period;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 32767.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  freq = synth_util->frequency;
  samplerate = (gdouble) synth_util->samplerate;
  freq_period = synth_util->samplerate / synth_util->frequency;

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
    };


    v_sawtooth *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0)};

    double v_scale[] = {(double) scale};    
    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_sawtooth, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint16) ((gint32) (tmp_source)[0] + (gint32) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint16) ((gint32) scale * (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint16) ((gint32) scale * (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint16) ((gint32) scale * (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint16) ((gint32) scale * (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint16) ((gint32) scale * (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint16) ((gint32) scale * (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint16) ((gint32) scale * (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint16) ((gint32) source[0] + (gint32) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);

    source += synth_util->source_stride;
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
  
  gdouble freq;
  gdouble samplerate;
  gdouble freq_period;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 8388607.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  freq = synth_util->frequency;
  samplerate = (gdouble) synth_util->samplerate;
  freq_period = synth_util->samplerate / synth_util->frequency;

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
    };


    v_sawtooth *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0)};

    double v_scale[] = {(double) scale};    
    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_sawtooth, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint32) ((gint32) (tmp_source)[0] + (gint32) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint32) scale * (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint32) scale * (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint32) scale * (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint32) scale * (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint32) scale * (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint32) scale * (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint32) scale * (tmp_source += synth_util->source_stride)[0] + (gint32) ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint32) ((gint32) source[0] + (gint32) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);

    source += synth_util->source_stride;
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
  
  gdouble freq;
  gdouble samplerate;
  gdouble freq_period;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 214748363.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  freq = synth_util->frequency;
  samplerate = (gdouble) synth_util->samplerate;
  freq_period = synth_util->samplerate / synth_util->frequency;

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
    };


    v_sawtooth *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0)};

    double v_scale[] = {(double) scale};    
    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_sawtooth, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint32) ((gint64) (tmp_source)[0] + (gint64) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint32) ((gint64) source[0] + (gint64) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);

    source += synth_util->source_stride;
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
  
  gdouble freq;
  gdouble samplerate;
  gdouble freq_period;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 9223372036854775807.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  freq = synth_util->frequency;
  samplerate = (gdouble) synth_util->samplerate;
  freq_period = synth_util->samplerate / synth_util->frequency;

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
    };


    v_sawtooth *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0)};

    double v_scale[] = {(double) scale};    
    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_sawtooth, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint64) ((gint64) (tmp_source)[0] + (gint64) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint64) ((gint64) source[0] + (gint64) scale * ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);

    source += synth_util->source_stride;
  }
}

/**
 * ags_synth_util_compute_sawtooth_float:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sawtooth synth of floating point data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sawtooth_float(AgsSynthUtil *synth_util)
{
  gfloat *source, *tmp_source;
  
  gdouble freq;
  gdouble samplerate;
  gdouble freq_period;
  gdouble volume;
  guint i, i_stop;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  freq = synth_util->frequency;
  samplerate = (gdouble) synth_util->samplerate;
  freq_period = synth_util->samplerate / synth_util->frequency;

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
    };


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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0)};

    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_sawtooth, 1, v_volume, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp0_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gfloat) ((tmp_source)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gfloat) (source[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);

    source += synth_util->source_stride;
  }
}

/**
 * ags_synth_util_compute_sawtooth_double:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute sawtooth synth of double precision floating point data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_sawtooth_double(AgsSynthUtil *synth_util)
{
  gdouble *source, *tmp_source;
  
  gdouble freq;
  gdouble samplerate;
  gdouble freq_period;
  gdouble volume;
  guint i, i_stop;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  freq = synth_util->frequency;
  samplerate = (gdouble) synth_util->samplerate;
  freq_period = synth_util->samplerate / synth_util->frequency;

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
    };


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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0),
      ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0)};

    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_sawtooth, 1, v_volume, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp0_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gdouble) ((tmp_source)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gdouble) (source[0] + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume);

    source += synth_util->source_stride;
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

  gdouble freq;
  gdouble samplerate;
  gdouble freq_period;
  gdouble volume;
  guint i;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  freq = synth_util->frequency;
  samplerate = (gdouble) synth_util->samplerate;
  freq_period = synth_util->samplerate / synth_util->frequency;

  volume = synth_util->volume;
  
  i = 0;
  
  for(; i < synth_util->buffer_length;){
    ags_complex_set(source,
		    (ags_complex_get(source) + ((fmod(((gdouble) (synth_util->offset + (i++)) + synth_util->phase), freq_period) * 2.0 * freq / samplerate) - 1.0) * volume));

    source += synth_util->source_stride;
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

  switch(synth_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_synth_util_compute_sawtooth_s8(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_synth_util_compute_sawtooth_s16(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_synth_util_compute_sawtooth_s24(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_synth_util_compute_sawtooth_s32(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_synth_util_compute_sawtooth_s64(synth_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_synth_util_compute_sawtooth_float(synth_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_synth_util_compute_sawtooth_double(synth_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_synth_util_compute_sawtooth_complex(synth_util);
  }
  break;
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

  gdouble freq;
  gdouble samplerate;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 127.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  freq = synth_util->frequency;
  samplerate = (gdouble) synth_util->samplerate;
  volume = synth_util->volume;
  
  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
    };

    v_triangle *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0))};
    
    double v_scale[] = {(double) scale};
    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_triangle, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    source[0] = (gint8) ((gint16) source[0] + (gint16) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint8) ((gint16) source[0] + (gint16) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint8) ((gint16) source[0] + (gint16) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint8) ((gint16) source[0] + (gint16) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint8) ((gint16) source[0] + (gint16) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint8) ((gint16) source[0] + (gint16) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint8) ((gint16) source[0] + (gint16) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint8) ((gint16) source[0] + (gint16) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;    
    i++;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint8) ((gint16) source[0] + (gint16) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));

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

  gdouble freq;
  gdouble samplerate;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 32767.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  freq = synth_util->frequency;
  samplerate = (gdouble) synth_util->samplerate;
  volume = synth_util->volume;
  
  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
    };
    v_triangle *= scale;
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

    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0))};
    
    double v_scale[] = {(double) scale};
    double v_volume[] = {(double) volume};
    
    vDSP_vmulD(v_triangle, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    source[0] = (gint16) ((gint32) source[0] + (gint32) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint16) ((gint32) source[0] + (gint32) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint16) ((gint32) source[0] + (gint32) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint16) ((gint32) source[0] + (gint32) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint16) ((gint32) source[0] + (gint32) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint16) ((gint32) source[0] + (gint32) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint16) ((gint32) source[0] + (gint32) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint16) ((gint32) source[0] + (gint32) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;    
    i++;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint16) ((gint32) source[0] + (gint32) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));

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

  gdouble freq;
  gdouble samplerate;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 8388607.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  freq = synth_util->frequency;
  samplerate = (gdouble) synth_util->samplerate;
  volume = synth_util->volume;
  
  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
    };
    v_triangle *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0))};
    
    double v_scale[] = {(double) scale};
    double v_volume[] = {(double) volume};
    
    vDSP_vmulD(v_triangle, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    source[0] = (gint32) ((gint32) source[0] + (gint32) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint32) source[0] + (gint32) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint32) source[0] + (gint32) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint32) source[0] + (gint32) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint32) source[0] + (gint32) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint32) source[0] + (gint32) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint32) source[0] + (gint32) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint32) source[0] + (gint32) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;    
    i++;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint32) ((gint32) source[0] + (gint32) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));

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

  gdouble freq;
  gdouble samplerate;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 214748363.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  freq = synth_util->frequency;
  samplerate = (gdouble) synth_util->samplerate;
  volume = synth_util->volume;
  
  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
    };
    v_triangle *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0))};
    
    double v_scale[] = {(double) scale};
    double v_volume[] = {(double) volume};
    
    vDSP_vmulD(v_triangle, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    source[0] = (gint32) ((gint64) source[0] + (gint64) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint64) source[0] + (gint64) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint64) source[0] + (gint64) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint64) source[0] + (gint64) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint64) source[0] + (gint64) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint64) source[0] + (gint64) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint64) source[0] + (gint64) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint32) ((gint64) source[0] + (gint64) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;    
    i++;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint32) ((gint64) source[0] + (gint64) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));

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

  gdouble freq;
  gdouble samplerate;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 9223372036854775807.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  freq = synth_util->frequency;
  samplerate = (gdouble) synth_util->samplerate;
  volume = synth_util->volume;
  
  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
    };
    v_triangle *= scale;
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

    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0))};
    
    double v_scale[] = {(double) scale};
    double v_volume[] = {(double) volume};
    
    vDSP_vmulD(v_triangle, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gint64) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gint64) ret_v_buffer[7];

    source += synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
  for(; i < i_stop;){
    source[0] = (gint64) ((gint64) source[0] + (gint64) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint64) ((gint64) source[0] + (gint64) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint64) ((gint64) source[0] + (gint64) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint64) ((gint64) source[0] + (gint64) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint64) ((gint64) source[0] + (gint64) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint64) ((gint64) source[0] + (gint64) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint64) ((gint64) source[0] + (gint64) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gint64) ((gint64) source[0] + (gint64) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;    
    i++;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint64) ((gint64) source[0] + (gint64) scale * (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_triangle_float:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute triangle synth of floating point data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_triangle_float(AgsSynthUtil *synth_util)
{
  gfloat *source, *tmp_source;

  gdouble freq;
  gdouble samplerate;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 9223372036854775807.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  freq = synth_util->frequency;
  samplerate = (gdouble) synth_util->samplerate;
  volume = synth_util->volume;
  
  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
    };

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

    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0))};
    
    double v_volume[] = {(double) volume};
    
    vDSP_vmulD(v_triangle, 1, v_volume, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp0_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gfloat) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gfloat) ret_v_buffer[7];

    source += synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
  for(; i < i_stop;){
    source[0] = (gfloat) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gfloat) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gfloat) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gfloat) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gfloat) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gfloat) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gfloat) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gfloat) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;    
    i++;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gfloat) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));

    source += synth_util->source_stride;
    i++;
  }
}

/**
 * ags_synth_util_compute_triangle_double:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute triangle synth of double precision floating point data.
 *
 * Since: 3.9.3
 */
void
ags_synth_util_compute_triangle_double(AgsSynthUtil *synth_util)
{
  gdouble *source, *tmp_source;

  gdouble freq;
  gdouble samplerate;
  gdouble volume;
  guint i, i_stop;

  static const gdouble scale = 9223372036854775807.0;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  freq = synth_util->frequency;
  samplerate = (gdouble) synth_util->samplerate;
  volume = synth_util->volume;
  
  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
    };

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

    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 1) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 2) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 3) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 4) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 5) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 6) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0)),
      (((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i + 7) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0))};
    
    double v_volume[] = {(double) volume};
    
    vDSP_vmulD(v_triangle, 1, v_volume, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp0_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
    *(source) = (gdouble) ret_v_buffer[0];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[1];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[2];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[3];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[4];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[5];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[6];
    *(source += synth_util->source_stride) = (gdouble) ret_v_buffer[7];

    source += synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
  for(; i < i_stop;){
    source[0] = (gdouble) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gdouble) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gdouble) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gdouble) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gdouble) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gdouble) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gdouble) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;
    i++;
    source[0] = (gdouble) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));
    source += synth_util->source_stride;    
    i++;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gdouble) (source[0] + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume));

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

  gdouble freq;
  gdouble samplerate;
  gdouble volume;
  guint i;

  if(synth_util == NULL ||
     synth_util->source == NULL){
    return;
  }

  source = synth_util->source;

  freq = synth_util->frequency;
  samplerate = (gdouble) synth_util->samplerate;

  volume = synth_util->volume;
  
  i = 0;
  
  for(; i < synth_util->buffer_length;){
    ags_complex_set(source,
		    (ags_complex_get(source) + (((((synth_util->offset + i) + synth_util->phase) * freq / samplerate * 2.0) - (((double) ((((synth_util->offset + i) + synth_util->phase) * freq / samplerate)) / 2.0) * 2.0) - 1.0) * volume)));

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

  switch(synth_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_synth_util_compute_triangle_s8(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_synth_util_compute_triangle_s16(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_synth_util_compute_triangle_s24(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_synth_util_compute_triangle_s32(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_synth_util_compute_triangle_s64(synth_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_synth_util_compute_triangle_float(synth_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_synth_util_compute_triangle_double(synth_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_synth_util_compute_triangle_complex(synth_util);
  }
  break;
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

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
    };


    v_square *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume};

    double v_scale[] = {(double) scale};
    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_square, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint8) ((gint16) (tmp_source)[0] + (gint16) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (gint16) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint8) ((gint16) source[0] + (gint16) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
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

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
    };

    v_square *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};
    double v_scale[] = {(double) scale};

    vDSP_vmulD(v_square, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint16) ((gint32) (tmp_source)[0] + (gint32) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint16) ((gint32) source[0] + (gint32) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
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

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
    };


    v_square *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};
    double v_scale[] = {(double) scale};

    vDSP_vmulD(v_square, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint32) ((gint32) (tmp_source)[0] + (gint32) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (gint32) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint32) ((gint32) source[0] + (gint32) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
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

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
    };


    v_square *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};
    double v_scale[] = {(double) scale};

    vDSP_vmulD(v_square, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint32) ((gint64) (tmp_source)[0] + (gint64) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint32) ((gint64) source[0] + (gint64) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
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

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
    };


    v_square *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};
    double v_scale[] = {(double) scale};

    vDSP_vmulD(v_square, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint64) ((gint64) (tmp_source)[0] + (gint64) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (gint64) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint64) ((gint64) source[0] + (gint64) scale * ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
  }
}

/**
 * ags_synth_util_compute_square_float:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute square synth of floating point data.
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
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
    };


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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_square, 1, v_volume, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp0_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gfloat) ((tmp_source)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gfloat) (source[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
  }
}

/**
 * ags_synth_util_compute_square_double:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute square synth of double precision floating point data.
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
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
    };


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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume,
      ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_square, 1, v_volume, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp0_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gdouble) ((tmp_source)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));
    *(source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gdouble) (source[0] + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
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
  
  for(; i < synth_util->buffer_length;){
    ags_complex_set(source,
		    (gdouble) (ags_complex_get(source) + ((sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= 0.0 ? 1.0: -1.0) * volume)));

    source += synth_util->source_stride;
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

  switch(synth_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_synth_util_compute_square_s8(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_synth_util_compute_square_s16(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_synth_util_compute_square_s24(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_synth_util_compute_square_s32(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_synth_util_compute_square_s64(synth_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_synth_util_compute_square_float(synth_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_synth_util_compute_square_double(synth_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_synth_util_compute_square_complex(synth_util);
  }
  break;
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

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume
    };


    v_impulse *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];
    
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
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume};

    double v_scale[] = {(double) scale};    
    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_impulse, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint8) ((gint16) (tmp_source)[0] + (gint16) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint8) ((gint16) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint8) ((gint16) source[0] + (gint16) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
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

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume
    };


    v_impulse *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume};

    double v_scale[] = {(double) scale};
    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_impulse, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint16) ((gint32) (tmp_source)[0] + (gint32) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint16) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint16) ((gint32) source[0] + (gint32) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
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

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume
    };


    v_impulse *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume};

    double v_scale[] = {(double) scale};
    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_impulse, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint32) ((gint32) (tmp_source)[0] + (gint32) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint32) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint32) ((gint32) source[0] + (gint32) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
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

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume
    };


    v_impulse *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume};

    double v_scale[] = {(double) scale};
    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_impulse, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint32) ((gint64) (tmp_source)[0] + (gint64) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint32) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint32) ((gint64) source[0] + (gint64) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
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

  volume = synth_util->volume;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = synth_util->buffer_length - (synth_util->buffer_length % 8);
  
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
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume
    };


    v_impulse *= scale;
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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume};

    double v_scale[] = {(double) scale};
    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_impulse, 1, v_scale, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vmulD(tmp0_ret_v_buffer, 1, v_volume, 0, tmp1_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp1_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gint64) ((gint64) (tmp_source)[0] + (gint64) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gint64) ((gint64) (tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gint64) ((gint64) source[0] + (gint64) (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
  }
}

/**
 * ags_synth_util_compute_impulse_float:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute impulse synth of floating point data.
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
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume
    };


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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_impulse, 1, v_volume, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp0_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gfloat) ((tmp_source)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gfloat) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gfloat) (source[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
  }
}

/**
 * ags_synth_util_compute_impulse_double:
 * @synth_util: the #AgsSynthUtil-struct
 * 
 * Compute impulse synth of double precision floating point data.
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
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume
    };


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
  
  for(; i < i_stop;){
    double ret_v_buffer[8], tmp0_ret_v_buffer[8], tmp1_ret_v_buffer[8];

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
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume,
      (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume};

    double v_volume[] = {(double) volume};

    vDSP_vmulD(v_impulse, 1, v_volume, 0, tmp0_ret_v_buffer, 1, 8);
    vDSP_vaddD(v_buffer, 1, tmp0_ret_v_buffer, 1, ret_v_buffer, 1, 8);
    
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
  
  for(; i < i_stop;){
    tmp_source = source;

    (*source) = (gdouble) ((tmp_source)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);
    (*source += synth_util->source_stride) = (gdouble) ((tmp_source += synth_util->source_stride)[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
  }
#endif

  for(; i < synth_util->buffer_length;){
    source[0] = (gdouble) (source[0] + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume);

    source += synth_util->source_stride;
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
  
  for(; i < synth_util->buffer_length;){
    ags_complex_set(source,
		    (gdouble) (ags_complex_get(source) + (sin((gdouble) ((synth_util->offset + (i++)) + synth_util->phase) * 2.0 * M_PI * synth_util->frequency / (gdouble) synth_util->samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * volume));

    source += synth_util->source_stride;
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

  switch(synth_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_synth_util_compute_impulse_s8(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_synth_util_compute_impulse_s16(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_synth_util_compute_impulse_s24(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_synth_util_compute_impulse_s32(synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_synth_util_compute_impulse_s64(synth_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_synth_util_compute_impulse_float(synth_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_synth_util_compute_impulse_double(synth_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_synth_util_compute_impulse_complex(synth_util);
  }
  break;
  }
}
