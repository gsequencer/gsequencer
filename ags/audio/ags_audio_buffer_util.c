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

#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_vector_256_manager.h>

#include <samplerate.h>

#if defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#include <Accelerate/Accelerate.h>

#define AGS_VECTORIZED_BUILTIN_FUNCTIONS 1
#define AGS_VECTOR_256_FUNCTIONS 1

// #define LARGE_VECTOR 1
// #define AGS_VDSP_256_FUNCTIONS
#endif

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <complex.h>

//FIXME:JK: disabled because of silent output
#undef AGS_VECTOR_256_FUNCTIONS

/**
 * SECTION:ags_audio_buffer_util
 * @short_description: audio buffer util
 * @title: AgsAudioBufferUtil
 * @section_id:
 * @include: ags/audio/ags_audio_buffer_util.h
 *
 * Utility functions to process audio buffer.
 */

GType
ags_audio_buffer_util_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_audio_buffer_util = 0;

    ags_type_audio_buffer_util =
      g_boxed_type_register_static("AgsAudioBufferUtil",
				   (GBoxedCopyFunc) ags_audio_buffer_util_copy,
				   (GBoxedFreeFunc) ags_audio_buffer_util_free);

    g_once_init_leave(&g_define_type_id__static, ags_type_audio_buffer_util);
  }

  return(g_define_type_id__static);
}

/**
 * ags_audio_buffer_util_alloc:
 *
 * Allocate #AgsAudioBufferUtil-struct
 *
 * Returns: a new #AgsAudioBufferUtil-struct
 *
 * Since: 7.0.0
 */
AgsAudioBufferUtil*
ags_audio_buffer_util_alloc()
{
  AgsAudioBufferUtil *ptr;

  ptr = (AgsAudioBufferUtil *) g_new(AgsAudioBufferUtil,
				     1);

  ptr->destination = NULL;
  ptr->destination_format = AGS_AUDIO_BUFFER_UTIL_DEFAULT_FORMAT;
  ptr->destination_stride = 1;

  ptr->source = NULL;
  ptr->source_format = AGS_AUDIO_BUFFER_UTIL_DEFAULT_FORMAT;
  ptr->source_stride = 1;

  ptr->copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16;

  ptr->converted_source = NULL;

  ptr->buffer_length = 0;

  return(ptr);
}

/**
 * ags_audio_buffer_util_copy:
 * @ptr: the original #AgsAudioBufferUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsAudioBufferUtil-struct
 *
 * Since: 7.0.0
 */
gpointer
ags_audio_buffer_util_copy(AgsAudioBufferUtil *ptr)
{
  AgsAudioBufferUtil *new_ptr;

  if(ptr == NULL){
    return(NULL);
  }
  
  new_ptr = (AgsAudioBufferUtil *) g_new(AgsAudioBufferUtil,
					 1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_format = ptr->destination_format;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_format = ptr->source_format;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->copy_mode = ptr->copy_mode;

  new_ptr->converted_source = ptr->converted_source;

  new_ptr->buffer_length = ptr->buffer_length;

  return(new_ptr);
}

/**
 * ags_audio_buffer_util_free:
 * @ptr: the #AgsAudioBufferUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_free(AgsAudioBufferUtil *ptr)
{
  g_free(ptr);
}

/**
 * ags_audio_buffer_util_get_destination:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * 
 * Get destination buffer of @audio_buffer_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 7.0.0
 */
gpointer
ags_audio_buffer_util_get_destination(AgsAudioBufferUtil *audio_buffer_util)
{
  if(audio_buffer_util == NULL){
    return(NULL);
  }

  return(audio_buffer_util->destination);
}

/**
 * ags_audio_buffer_util_set_destination:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: (transfer none): the destination buffer
 *
 * Set @destination buffer of @audio_buffer_util.
 *
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_set_destination(AgsAudioBufferUtil *audio_buffer_util,
				      gpointer destination)
{
  if(audio_buffer_util == NULL){
    return;
  }

  audio_buffer_util->destination = destination;
}

/**
 * ags_audio_buffer_util_get_destination_format:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * 
 * Get destination format of @audio_buffer_util.
 * 
 * Returns: the destination format
 * 
 * Since: 7.0.0
 */
AgsSoundcardFormat
ags_audio_buffer_util_get_destination_format(AgsAudioBufferUtil *audio_buffer_util)
{
  if(audio_buffer_util == NULL){
    return(0);
  }

  return(audio_buffer_util->destination_format);
}

/**
 * ags_audio_buffer_util_set_destination_format:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination_format: the destination format
 *
 * Set @destination_format of @audio_buffer_util.
 *
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_set_destination_format(AgsAudioBufferUtil *audio_buffer_util,
					     AgsSoundcardFormat destination_format)
{
  if(audio_buffer_util == NULL){
    return;
  }

  if(audio_buffer_util->destination_format != destination_format){
    audio_buffer_util->destination_format = destination_format;

    ags_stream_free(audio_buffer_util->converted_source);

    audio_buffer_util->converted_source = ags_stream_alloc(audio_buffer_util->buffer_length,
							   destination_format);
  }
}

/**
 * ags_audio_buffer_util_get_destination_stride:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * 
 * Get destination stride of @audio_buffer_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 7.0.0
 */
guint
ags_audio_buffer_util_get_destination_stride(AgsAudioBufferUtil *audio_buffer_util)
{
  if(audio_buffer_util == NULL){
    return(0);
  }

  return(audio_buffer_util->destination_stride);
}

/**
 * ags_audio_buffer_util_set_destination_stride:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @audio_buffer_util.
 *
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_set_destination_stride(AgsAudioBufferUtil *audio_buffer_util,
					     guint destination_stride)
{
  if(audio_buffer_util == NULL){
    return;
  }

  audio_buffer_util->destination_stride = destination_stride;
}

/**
 * ags_audio_buffer_util_get_source:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * 
 * Get source buffer of @audio_buffer_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 7.0.0
 */
gpointer
ags_audio_buffer_util_get_source(AgsAudioBufferUtil *audio_buffer_util)
{
  if(audio_buffer_util == NULL){
    return(NULL);
  }

  return(audio_buffer_util->source);
}

/**
 * ags_audio_buffer_util_set_source:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @source: (transfer none): the source buffer
 *
 * Set @source buffer of @audio_buffer_util.
 *
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_set_source(AgsAudioBufferUtil *audio_buffer_util,
				 gpointer source)
{
  if(audio_buffer_util == NULL){
    return;
  }

  audio_buffer_util->source = source;
}

/**
 * ags_audio_buffer_util_get_source_format:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * 
 * Get source format of @audio_buffer_util.
 * 
 * Returns: the source format
 * 
 * Since: 7.0.0
 */
AgsSoundcardFormat
ags_audio_buffer_util_get_source_format(AgsAudioBufferUtil *audio_buffer_util)
{
  if(audio_buffer_util == NULL){
    return(0);
  }

  return(audio_buffer_util->source_format);
}

/**
 * ags_audio_buffer_util_set_source_format:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @source_format: the source format
 *
 * Set @source_format of @audio_buffer_util.
 *
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_set_source_format(AgsAudioBufferUtil *audio_buffer_util,
					AgsSoundcardFormat source_format)
{
  if(audio_buffer_util == NULL){
    return;
  }

  audio_buffer_util->source_format = source_format;
}

/**
 * ags_audio_buffer_util_get_source_stride:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * 
 * Get source stride of @audio_buffer_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 7.0.0
 */
guint
ags_audio_buffer_util_get_source_stride(AgsAudioBufferUtil *audio_buffer_util)
{
  if(audio_buffer_util == NULL){
    return(0);
  }

  return(audio_buffer_util->source_stride);
}

/**
 * ags_audio_buffer_util_set_source_stride:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @audio_buffer_util.
 *
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_set_source_stride(AgsAudioBufferUtil *audio_buffer_util,
					guint source_stride)
{
  if(audio_buffer_util == NULL){
    return;
  }

  audio_buffer_util->source_stride = source_stride;
}

/**
 * ags_audio_buffer_util_get_buffer_length:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * 
 * Get buffer length of @audio_buffer_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 7.0.0
 */
guint
ags_audio_buffer_util_get_buffer_length(AgsAudioBufferUtil *audio_buffer_util)
{
  if(audio_buffer_util == NULL){
    return(0);
  }

  return(audio_buffer_util->buffer_length);
}

/**
 * ags_audio_buffer_util_set_buffer_length:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @audio_buffer_util.
 *
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_set_buffer_length(AgsAudioBufferUtil *audio_buffer_util,
					guint buffer_length)
{
  if(audio_buffer_util == NULL){
    return;
  }

  if(audio_buffer_util->buffer_length != buffer_length){
    audio_buffer_util->buffer_length = buffer_length;
    
    ags_stream_free(audio_buffer_util->converted_source);

    audio_buffer_util->converted_source = ags_stream_alloc(buffer_length,
							   audio_buffer_util->destination_format);
  }
}

/**
 * ags_audio_buffer_util_get_copy_mode:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * 
 * Get copy_mode of @audio_buffer_util.
 * 
 * Returns: the copy_mode
 * 
 * Since: 7.0.0
 */
AgsAudioBufferUtilCopyMode
ags_audio_buffer_util_get_copy_mode(AgsAudioBufferUtil *audio_buffer_util)
{
  if(audio_buffer_util == NULL){
    return(0);
  }

  return(audio_buffer_util->copy_mode);
}

/**
 * ags_audio_buffer_util_set_copy_mode:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @copy_mode: the copy_mode
 *
 * Set @copy_mode of @audio_buffer_util.
 *
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_set_copy_mode(AgsAudioBufferUtil *audio_buffer_util,
				    AgsAudioBufferUtilCopyMode copy_mode)
{
  if(audio_buffer_util == NULL){
    return;
  }

  audio_buffer_util->copy_mode = copy_mode;
}

/**
 * ags_audio_buffer_util_format_from_soundcard:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @soundcard_format: the soundcard bit mode
 *
 * Translate soundcard format to audio buffer util format.
 *
 * Returns: the converted format
 *
 * Since: 3.0.0
 */
AgsAudioBufferUtilFormat
ags_audio_buffer_util_format_from_soundcard(AgsAudioBufferUtil *audio_buffer_util,
					    AgsSoundcardFormat soundcard_format)
{
  switch(soundcard_format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    return(AGS_AUDIO_BUFFER_UTIL_S8);
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    return(AGS_AUDIO_BUFFER_UTIL_S16);
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    return(AGS_AUDIO_BUFFER_UTIL_S24);
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    return(AGS_AUDIO_BUFFER_UTIL_S32);
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    return(AGS_AUDIO_BUFFER_UTIL_S64);
  case AGS_SOUNDCARD_FLOAT:
    return(AGS_AUDIO_BUFFER_UTIL_FLOAT);
  case AGS_SOUNDCARD_DOUBLE:
    return(AGS_AUDIO_BUFFER_UTIL_DOUBLE);
  case AGS_SOUNDCARD_COMPLEX:
    return(AGS_AUDIO_BUFFER_UTIL_COMPLEX);
  default:
    {
      g_warning("ags_audio_buffer_util_format_from_soundcard() - unsupported soundcard format");
      
      return(0);
    }
  }
}

/**
 * ags_audio_buffer_util_get_copy_mode_from_format:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination_format: the destination buffer format
 * @source_format: the source buffer format
 *
 * Get copy mode for given destination and source format.
 *
 * Returns: the desired copy mode
 *
 * Since: 3.0.0
 */
AgsAudioBufferUtilCopyMode
ags_audio_buffer_util_get_copy_mode_from_format(AgsAudioBufferUtil *audio_buffer_util,
						AgsAudioBufferUtilFormat destination_format,
						AgsAudioBufferUtilFormat source_format)
{
  guint copy_mode;

  copy_mode = 0;

  if(source_format == AGS_AUDIO_BUFFER_UTIL_S8){

    /* signed 8 bit source */
    switch(destination_format){
    case AGS_AUDIO_BUFFER_UTIL_S8:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S8;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S16:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S16;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S24:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S24;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S32:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S32;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S64:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S64;
      break;
    case AGS_AUDIO_BUFFER_UTIL_FLOAT:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_FLOAT;
      break;
    case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_DOUBLE;
      break;
#ifdef __APPLE__
    case AGS_AUDIO_BUFFER_UTIL_FLOAT32:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_FLOAT32;
      break;
#endif
    case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_COMPLEX;
      break;
    default:
      {
	g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format");
      }
    }

  }else if(source_format == AGS_AUDIO_BUFFER_UTIL_S16){    

    /* signed 16 bit source */  
    switch(destination_format){
    case AGS_AUDIO_BUFFER_UTIL_S8:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S8;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S16:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S24:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S24;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S32:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S32;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S64:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S64;
      break;
    case AGS_AUDIO_BUFFER_UTIL_FLOAT:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_FLOAT;
      break;
    case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_DOUBLE;
      break;
#ifdef __APPLE__
    case AGS_AUDIO_BUFFER_UTIL_FLOAT32:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_FLOAT32;
      break;
#endif
    case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_COMPLEX;
      break;
    default:
      {
	g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format");
      }
    }

  }else if(source_format == AGS_AUDIO_BUFFER_UTIL_S24){    

    /* signed 24 bit source */
    switch(destination_format){
    case AGS_AUDIO_BUFFER_UTIL_S8:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S8;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S16:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S16;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S24:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S24;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S32:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S32;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S64:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S64;
      break;
    case AGS_AUDIO_BUFFER_UTIL_FLOAT:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_FLOAT;
      break;
    case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_DOUBLE;
      break;
#ifdef __APPLE__
    case AGS_AUDIO_BUFFER_UTIL_FLOAT32:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_FLOAT32;
      break;
#endif
    case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_COMPLEX;
      break;
    default:
      {
	g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format");
      }
    }

  }else if(source_format == AGS_AUDIO_BUFFER_UTIL_S32){    

    /* signed 32 bit source */
    switch(destination_format){
    case AGS_AUDIO_BUFFER_UTIL_S8:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S8;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S16:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S16;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S24:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S24;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S32:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S64:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S64;
      break;
    case AGS_AUDIO_BUFFER_UTIL_FLOAT:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_FLOAT;
      break;
    case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_DOUBLE;
      break;
#ifdef __APPLE__
    case AGS_AUDIO_BUFFER_UTIL_FLOAT32:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_FLOAT32;
      break;
#endif
    case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_COMPLEX;
      break;
    default:
      {
	g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format");
      }
    }

  }else if(source_format == AGS_AUDIO_BUFFER_UTIL_S64){    

    /* signed 64 bit source */
    switch(destination_format){
    case AGS_AUDIO_BUFFER_UTIL_S8:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S8;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S16:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S16;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S24:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S24;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S32:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S32;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S64:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S64;
      break;
    case AGS_AUDIO_BUFFER_UTIL_FLOAT:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_FLOAT;
      break;
    case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_DOUBLE;
      break;
#ifdef __APPLE__
    case AGS_AUDIO_BUFFER_UTIL_FLOAT32:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_FLOAT32;
      break;
#endif
    case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_COMPLEX;
      break;
    default:
      {
	g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format");
      }
    }

  }else if(source_format == AGS_AUDIO_BUFFER_UTIL_FLOAT){    

    /* float source */
    switch(destination_format){
    case AGS_AUDIO_BUFFER_UTIL_S8:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S8;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S16:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S16;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S24:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S24;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S32:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S32;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S64:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S64;
      break;
    case AGS_AUDIO_BUFFER_UTIL_FLOAT:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT;
      break;
    case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_DOUBLE;
      break;
#ifdef __APPLE__
    case AGS_AUDIO_BUFFER_UTIL_FLOAT32:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT32;
      break;
#endif
    case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_COMPLEX;
      break;
    default:
      {
	g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format");
      }
    }

  }else if(source_format == AGS_AUDIO_BUFFER_UTIL_DOUBLE){    

    /* double source */
    switch(destination_format){
    case AGS_AUDIO_BUFFER_UTIL_S8:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S8;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S16:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S16;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S24:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S24;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S32:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S32;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S64:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S64;
      break;
    case AGS_AUDIO_BUFFER_UTIL_FLOAT:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_FLOAT;
      break;
    case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_DOUBLE;
      break;
#ifdef __APPLE__
    case AGS_AUDIO_BUFFER_UTIL_FLOAT32:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_FLOAT32;
      break;
#endif
    case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_COMPLEX;
      break;
    default:
      {
	g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format");
      }
    }

#ifdef __APPLE__
  }else if(source_format == AGS_AUDIO_BUFFER_UTIL_FLOAT32){

    /* double source */
    switch(destination_format){
    case AGS_AUDIO_BUFFER_UTIL_S8:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S8;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S16:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S16;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S24:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S24;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S32:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S32;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S64:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S64;
      break;
    case AGS_AUDIO_BUFFER_UTIL_FLOAT:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_FLOAT;
      break;
    case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_DOUBLE;
      break;
    case AGS_AUDIO_BUFFER_UTIL_FLOAT32:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_FLOAT32;
      break;
    case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_COMPLEX;
      break;
    default:
      {
	g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format");
      }
    }
#endif

  }else if(source_format == AGS_AUDIO_BUFFER_UTIL_COMPLEX){

    /* complex source */
    switch(destination_format){
    case AGS_AUDIO_BUFFER_UTIL_S8:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S8;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S16:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S16;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S24:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S24;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S32:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S32;
      break;
    case AGS_AUDIO_BUFFER_UTIL_S64:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S64;
      break;
    case AGS_AUDIO_BUFFER_UTIL_FLOAT:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_FLOAT;
      break;
    case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_DOUBLE;
      break;
#ifdef __APPLE__
    case AGS_AUDIO_BUFFER_UTIL_FLOAT32:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_FLOAT32;
      break;
#endif
    case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_COMPLEX;
      break;
    default:
      {
	g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format");
      }
    }    
  }else{
    g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported source buffer format");
  }

  return(copy_mode);
}

/**
 * ags_audio_buffer_util_clear_float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @buffer: the buffer to clear
 * @channels: number of channels
 * @count: count frames
 * 
 * Clears a buffer.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_clear_float(AgsAudioBufferUtil *audio_buffer_util,
				  gfloat *buffer, guint channels,
				  guint count)
{
  guint limit;
  guint current_channel;
  guint i;

  if(buffer == NULL){
    return;
  }
  
  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit;){
      buffer[0] = 0.0;
      buffer[channels] = 0.0;
      buffer[2 * channels] = 0.0;
      buffer[3 * channels] = 0.0;
      buffer[4 * channels] = 0.0;
      buffer[5 * channels] = 0.0;
      buffer[6 * channels] = 0.0;
      buffer[7 * channels] = 0.0;

      buffer += (8 * channels);
      i += 8;
    }
  }

  for(; i < count; i++){
    *buffer = 0.0;

    buffer += channels;
  }
}

#ifdef __APPLE__
/**
 * ags_audio_buffer_util_clear_float32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @buffer: the buffer to clear
 * @channels: number of channels
 * @count: count frames
 * 
 * Clears a buffer.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_clear_float32(AgsAudioBufferUtil *audio_buffer_util,
				    Float32 *buffer, guint channels,
				    guint count)
{
  guint limit;
  guint current_channel;
  guint i;

  if(buffer == NULL){
    return;
  }

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit;){
      buffer[0] = 0.0;
      buffer[channels] = 0.0;
      buffer[2 * channels] = (Float32) 0.0;
      buffer[3 * channels] = (Float32) 0.0;
      buffer[4 * channels] = (Float32) 0.0;
      buffer[5 * channels] = (Float32) 0.0;
      buffer[6 * channels] = (Float32) 0.0;
      buffer[7 * channels] = (Float32) 0.0;

      buffer += (8 * channels);
      i += 8;
    }
  }

  for(; i < count; i++){
    *buffer = (Float32) 0.0;

    buffer += channels;
  }
}
#endif

/**
 * ags_audio_buffer_util_clear_double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @buffer: the buffer to clear
 * @channels: number of channels
 * @count: count frames
 * 
 * Clears a buffer.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_clear_double(AgsAudioBufferUtil *audio_buffer_util,
				   gdouble *buffer, guint channels,
				   guint count)
{
  guint limit;
  guint current_channel;
  guint i;

  if(buffer == NULL){
    return;
  }

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit;){
      buffer[0] = 0.0;
      buffer[channels] = 0.0;
      buffer[2 * channels] = 0.0;
      buffer[3 * channels] = 0.0;
      buffer[4 * channels] = 0.0;
      buffer[5 * channels] = 0.0;
      buffer[6 * channels] = 0.0;
      buffer[7 * channels] = 0.0;

      buffer += (8 * channels);
      i += 8;
    }
  }

  for(; i < count; i++){
    *buffer = 0.0;

    buffer += channels;
  }
}

/**
 * ags_audio_buffer_util_clear_complex:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @buffer: the buffer to clear
 * @channels: number of channels
 * @count: count frames
 * 
 * Clears a buffer.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_clear_complex(AgsAudioBufferUtil *audio_buffer_util,
				    AgsComplex *buffer, guint channels,
				    guint count)
{
  guint limit;
  guint current_channel;
  guint i;

  if(buffer == NULL){
    return;
  }

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit;){
      current_channel = 0;

      buffer[0].real = 0.0;
      buffer[0].imag = 0.0;

      buffer[(current_channel = channels)].real = 0.0;
      buffer[(current_channel)].imag = 0.0;

      buffer[(current_channel += channels)].real = 0.0;
      buffer[(current_channel)].imag = 0.0;

      buffer[(current_channel += channels)].real = 0.0;
      buffer[(current_channel)].imag = 0.0;

      buffer[(current_channel += channels)].real = 0.0;
      buffer[(current_channel)].imag = 0.0;

      buffer[(current_channel += channels)].real = 0.0;
      buffer[(current_channel)].imag = 0.0;

      buffer[(current_channel += channels)].real = 0.0;
      buffer[(current_channel)].imag = 0.0;

      buffer[(current_channel += channels)].real = 0.0;
      buffer[(current_channel)].imag = 0.0;

      buffer += (current_channel + channels);
      i += 8;
    }
  }

  for(; i < count; i++){
    buffer[0].real = 0.0;
    buffer[0].imag = 0.0;

    buffer += channels;
  }
}

/**
 * ags_audio_buffer_util_clear_buffer:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @buffer: the audio data
 * @channels: the number of channels
 * @count: number of frames
 * @format: the format
 *
 * Clears the audio data.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_clear_buffer(AgsAudioBufferUtil *audio_buffer_util,
				   void *buffer, guint channels,
				   guint count, guint format)
{
  switch(format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
    {
      memset((gint8 *) buffer, 0, channels * count * sizeof(gint8));
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
    {
      memset((gint16 *) buffer, 0, channels * count * sizeof(gint16));
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
    {
      memset((gint32 *) buffer, 0, channels * count * sizeof(gint32));
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
    {
      memset((gint32 *) buffer, 0, channels * count * sizeof(gint32));
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
    {
      memset((gint64 *) buffer, 0, channels * count * sizeof(gint64));
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
    {
      ags_audio_buffer_util_clear_float(audio_buffer_util,
					buffer, channels,
					count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
    {
      ags_audio_buffer_util_clear_double(audio_buffer_util,
					 buffer, channels,
					 count);
    }
    break;
#ifdef __APPLE__
  case AGS_AUDIO_BUFFER_UTIL_FLOAT32:
    {
      ags_audio_buffer_util_clear_float32(audio_buffer_util,
					  buffer, channels,
					  count);
    }
    break;
#endif
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
    {
      ags_audio_buffer_util_clear_complex(audio_buffer_util,
					  buffer, channels,
					  count);
    }
    break;
  default:
    g_critical("unsupported audio buffer format");
  }
}

/**
 * ags_audio_buffer_util_pong_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: the destination
 * @destination_stride: the destination channels
 * @source: the source
 * @source_stride: the source channels
 * @count: the count of frames
 * 
 * Pingpong copy @source to @destination.
 * 
 * Since: 3.4.0
 */
void
ags_audio_buffer_util_pong_s8(AgsAudioBufferUtil *audio_buffer_util,
			      gint8 *destination, guint destination_stride,
			      gint8 *source, guint source_stride,
			      guint count)
{
  guint limit;
  gint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8s8 v_destination;
      ags_v8s8 v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8s8) {destination[0],
				  destination[(current_dchannel = destination_stride)],
				  destination[(current_dchannel += destination_stride)],
				  destination[(current_dchannel += destination_stride)],
				  destination[(current_dchannel += destination_stride)],
				  destination[(current_dchannel += destination_stride)],
				  destination[(current_dchannel += destination_stride)],
				  destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8s8) {source[0],
			     source[(current_schannel -= source_stride)],
			     source[(current_schannel -= source_stride)],
			     source[(current_schannel -= source_stride)],
			     source[(current_schannel -= source_stride)],
			     source[(current_schannel -= source_stride)],
			     source[(current_schannel -= source_stride)],
			     source[(current_schannel -= source_stride)]};

      /* no scale, just copy */
      v_destination += v_source;
      
      current_dchannel = 0;

      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      int v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      int v_destination[] = {(int) destination[0],
			     (int) destination[(current_dchannel = destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint8) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint8) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;
      
      destination[0] = 0xff & ((gint16) (destination[0] + source[0]));      
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel = destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#endif

  for(; i < count; i++){
    destination[0] = 0xff & ((gint16) (destination[0] + source[0]));

    destination += destination_stride;
    source -= source_stride;
  }
}

/**
 * ags_audio_buffer_util_pong_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: the destination
 * @destination_stride: the destination channels
 * @source: the source
 * @source_stride: the source channels
 * @count: the count of frames
 * 
 * Pingpong copy @source to @destination.
 * 
 * Since: 3.4.0
 */
void
ags_audio_buffer_util_pong_s16(AgsAudioBufferUtil *audio_buffer_util,
			       gint16 *destination, guint destination_stride,
			       gint16 *source, guint source_stride,
			       guint count)
{
  guint limit;
  gint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8s16 v_destination;
      ags_v8s16 v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8s16) {destination[0],
				   destination[(current_dchannel = destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)]};
      v_source = (ags_v8s16) {source[0],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)]};

      /* no scale, just copy */
      v_destination += v_source;

      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      int v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      int v_destination[] = {(int) destination[0],
			     (int) destination[(current_dchannel = destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint16) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint16) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){      
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (gint16) 0xffff & ((gint32) (destination[0] + source[0]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel = destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#endif

  for(; i < count; i++){
    destination[0] = (gint16) 0xffff & ((gint32) (destination[0] + source[0]));

    destination += destination_stride;
    source -= source_stride;
  }
}

/**
 * ags_audio_buffer_util_pong_s24:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: the destination
 * @destination_stride: the destination channels
 * @source: the source
 * @source_stride: the source channels
 * @count: the count of frames
 * 
 * Pingpong copy @source to @destination.
 * 
 * Since: 3.4.0
 */
void
ags_audio_buffer_util_pong_s24(AgsAudioBufferUtil *audio_buffer_util,
			       gint32 *destination, guint destination_stride,
			       gint32 *source, guint source_stride,
			       guint count)
{
  guint limit;
  gint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8s32 v_destination;
      ags_v8s32 v_source;

      current_dchannel = 0;
      current_schannel = 0;
      
      v_destination = (ags_v8s32) {destination[0],
				   destination[(current_dchannel = destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)]};

      v_source = (ags_v8s32) {source[0],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)]};
      
      /* no scale, just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      int v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      int v_destination[] = {(int) destination[0],
			     (int) destination[(current_dchannel = destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & ((gint32) (destination[0] + source[0]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel = destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint32) (destination[0] + source[0]));

    destination += destination_stride;
    source -= source_stride;
  }
}

/**
 * ags_audio_buffer_util_pong_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: the destination
 * @destination_stride: the destination channels
 * @source: the source
 * @source_stride: the source channels
 * @count: the count of frames
 * 
 * Pingpong copy @source to @destination.
 * 
 * Since: 3.4.0
 */
void
ags_audio_buffer_util_pong_s32(AgsAudioBufferUtil *audio_buffer_util,
			       gint32 *destination, guint destination_stride,
			       gint32 *source, guint source_stride,
			       guint count)
{
  guint limit;
  gint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8s32 v_destination;
      ags_v8s32 v_source;

      current_dchannel = 0;
      current_schannel = 0;
      
      v_destination = (ags_v8s32) {destination[0],
				   destination[(current_dchannel = destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)]};

      v_source = (ags_v8s32) {source[0],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)]};

      /* no scale, just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      int v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      int v_destination[] = {(int) destination[0],
			     (int) destination[(current_dchannel = destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = 0xffffffff & ((gint64) (destination[0] + source[0]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel = destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint64) (destination[0] + source[0]));

    destination += destination_stride;
    source -= source_stride;
  }
}

/**
 * ags_audio_buffer_util_pong_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: the destination
 * @destination_stride: the destination channels
 * @source: the source
 * @source_stride: the source channels
 * @count: the count of frames
 * 
 * Pingpong copy @source to @destination.
 * 
 * Since: 3.4.0
 */
void
ags_audio_buffer_util_pong_s64(AgsAudioBufferUtil *audio_buffer_util,
			       gint64 *destination, guint destination_stride,
			       gint64 *source, guint source_stride,
			       guint count)
{
  guint limit;
  gint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;  

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8s64 v_destination;
      ags_v8s64 v_source;

      current_dchannel = 0;
      current_schannel = 0;      

      v_destination = (ags_v8s64) {destination[0],
				   destination[(current_dchannel = destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8s64) {source[0],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)],
			      source[(current_schannel -= source_stride)]};

      /* no scale, just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      int v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      int v_destination[] = {(int) destination[0],
			     (int) destination[(current_dchannel = destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)],
			(int) source[(current_schannel -= source_stride)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint64) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint64) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + source[0]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel = destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel -= source_stride)]));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + source[0]));

    destination += destination_stride;
    source -= source_stride;
  }
}

/**
 * ags_audio_buffer_util_pong_float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: the destination
 * @destination_stride: the destination channels
 * @source: the source
 * @source_stride: the source channels
 * @count: the count of frames
 * 
 * Pingpong copy @source to @destination.
 * 
 * Since: 3.4.0
 */
void
ags_audio_buffer_util_pong_float(AgsAudioBufferUtil *audio_buffer_util,
				 gfloat *destination, guint destination_stride,
				 gfloat *source, guint source_stride,
				 guint count)
{
  guint limit;
  gint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {destination[0],
				     destination[(current_dchannel = destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {source[0],
				source[(current_schannel -= source_stride)],
				source[(current_schannel -= source_stride)],
				source[(current_schannel -= source_stride)],
				source[(current_schannel -= source_stride)],
				source[(current_schannel -= source_stride)],
				source[(current_schannel -= source_stride)],
				source[(current_schannel -= source_stride)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel -= source_stride)],
			   (double) source[(current_schannel -= source_stride)],
			   (double) source[(current_schannel -= source_stride)],
			   (double) source[(current_schannel -= source_stride)],
			   (double) source[(current_schannel -= source_stride)],
			   (double) source[(current_schannel -= source_stride)],
			   (double) source[(current_schannel -= source_stride)]};

      /* no scale, just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gfloat) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gfloat) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (source[(current_schannel -= source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel -= source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel -= source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel -= source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel -= source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel -= source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel -= source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0])));

    destination += destination_stride;
    source -= source_stride;
  }
}

/**
 * ags_audio_buffer_util_pong_double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: the destination
 * @destination_stride: the destination channels
 * @source: the source
 * @source_stride: the source channels
 * @count: the count of frames
 * 
 * Pingpong copy @source to @destination.
 * 
 * Since: 3.4.0
 */
void
ags_audio_buffer_util_pong_double(AgsAudioBufferUtil *audio_buffer_util,
				  gdouble *destination, guint destination_stride,
				  gdouble *source, guint source_stride,
				  guint count)
{
  guint limit;
  gint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {destination[0],
				      destination[(current_dchannel = destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {source[0],
				 source[(current_schannel -= source_stride)],
				 source[(current_schannel -= source_stride)],
				 source[(current_schannel -= source_stride)],
				 source[(current_schannel -= source_stride)],
				 source[(current_schannel -= source_stride)],
				 source[(current_schannel -= source_stride)],
				 source[(current_schannel -= source_stride)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel -= source_stride)],
			   (double) source[(current_schannel -= source_stride)],
			   (double) source[(current_schannel -= source_stride)],
			   (double) source[(current_schannel -= source_stride)],
			   (double) source[(current_schannel -= source_stride)],
			   (double) source[(current_schannel -= source_stride)],
			   (double) source[(current_schannel -= source_stride)]};

      /* no scale, just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gdouble) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gdouble) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (source[(current_schannel -= source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel -= source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel -= source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel -= source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel -= source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel -= source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel -= source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel - source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = ((gdouble) ((destination[0]) + (gdouble) (source[0])));

    destination += destination_stride;
    source -= source_stride;
  }
}

/**
 * ags_audio_buffer_util_pong_complex:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: the destination
 * @destination_stride: the destination channels
 * @source: the source
 * @source_stride: the source channels
 * @count: the count of frames
 * 
 * Pingpong copy @source to @destination.
 * 
 * Since: 3.4.0
 */
void
ags_audio_buffer_util_pong_complex(AgsAudioBufferUtil *audio_buffer_util,
				   AgsComplex *destination, guint destination_stride,
				   AgsComplex *source, guint source_stride,
				   guint count)
{
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;

  for(; i < count; i++){
    double _Complex z0, z1;

    z0 = ags_complex_get(destination);
    z1 = ags_complex_get(source);

    ags_complex_set(destination,
		    z0 + z1);
    
    destination += destination_stride;
    source -= source_stride;
  }
}

/**
 * ags_audio_buffer_util_pong:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: the destination
 * @destination_stride: the destination channels
 * @source: the source
 * @source_stride: the source channels
 * @count: the count of frames
 * @format: the format
 * 
 * Pingpong copy @source to @destination.
 * 
 * Since: 3.4.0
 */
void
ags_audio_buffer_util_pong(AgsAudioBufferUtil *audio_buffer_util,
			   void *destination, guint destination_stride,
			   void *source, guint source_stride,
			   guint count, guint format)
{
  if(destination == NULL ||
     source == NULL){
    return;
  }

  switch(format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
    {
      ags_audio_buffer_util_pong_s8(audio_buffer_util,
				    (gint8 *) destination, destination_stride,
				    (gint8 *) source, source_stride,
				    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
    {
      ags_audio_buffer_util_pong_s16(audio_buffer_util,
				     (gint16 *) destination, destination_stride,
				     (gint16 *) source, source_stride,
				     count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
    {
      ags_audio_buffer_util_pong_s24(audio_buffer_util,
				     (gint32 *) destination, destination_stride,
				     (gint32 *) source, source_stride,
				     count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
    {
      ags_audio_buffer_util_pong_s32(audio_buffer_util,
				     (gint32 *) destination, destination_stride,
				     (gint32 *) source, source_stride,
				     count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
    {
      ags_audio_buffer_util_pong_s64(audio_buffer_util,
				     (gint64 *) destination, destination_stride,
				     (gint64 *) source, source_stride,
				     count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
    {
      ags_audio_buffer_util_pong_float(audio_buffer_util,
				       (gfloat *) destination, destination_stride,
				       (gfloat *) source, source_stride,
				       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
    {
      ags_audio_buffer_util_pong_double(audio_buffer_util,
					(gdouble *) destination, destination_stride,
					(gdouble *) source, source_stride,
					count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
    {
      ags_audio_buffer_util_pong_complex(audio_buffer_util,
					 (AgsComplex *) destination, destination_stride,
					 (AgsComplex *) source, source_stride,
					 count);
    }
    break;
  default:
    g_warning("ags_audio_buffer_util_pong() - unknown format");
  }
}

/**
 * ags_audio_buffer_util_copy_s8_to_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s8_to_s8(AgsAudioBufferUtil *audio_buffer_util,
				    gint8 *destination, guint destination_stride,
				    gint8 *source, guint source_stride,
				    guint count)
{
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_SIGNED_8_BIT, AGS_VECTOR_256_SIGNED_8_BIT,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8s8(NULL,
				       source_arr->vector.vec_s8->mem_s8, 1,
				       source, source_stride,
				       256);

      ags_audio_buffer_util_fill_v8s8(NULL,
				       destination_arr->vector.vec_s8->mem_s8, 1,
				       destination, destination_stride,
				       256);

      for(j = 0; j < 32; j++){
	destination_arr->vector.vec_s8->mem_s8[j] += source_arr->vector.vec_s8->mem_s8[j];
      }
    
      ags_audio_buffer_util_fetch_v8s8(NULL,
					destination, destination_stride,
					destination_arr->vector.vec_s8->mem_s8, 1,
					256);

      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8s8 v_destination;
      ags_v8s8 v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8s8) {destination[0],
				  destination[(current_dchannel = destination_stride)],
				  destination[(current_dchannel += destination_stride)],
				  destination[(current_dchannel += destination_stride)],
				  destination[(current_dchannel += destination_stride)],
				  destination[(current_dchannel += destination_stride)],
				  destination[(current_dchannel += destination_stride)],
				  destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8s8) {source[0],
			     source[(current_schannel = source_stride)],
			     source[(current_schannel += source_stride)],
			     source[(current_schannel += source_stride)],
			     source[(current_schannel += source_stride)],
			     source[(current_schannel += source_stride)],
			     source[(current_schannel += source_stride)],
			     source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      v_destination += v_source;
      
      current_dchannel = 0;

      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  int v_destination[256];
  int v_source[256];
  int v_result[256];

  gint j;

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = destination[destination_stride * (i + j)];
      v_source[j] = source[source_stride * (i + j)];

      j++;
    }

    vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint8) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#elif defined(AGS_VDSP_256_FUNCTIONS)
  AgsVDSP256Manager *vdsp_256_manager = ags_vdsp_256_manager_get_instance();
  
  while(i + 256 <= limit){
    AgsVDSPArr *source_arr, *destination_arr, *result_arr;

    guint j;
    
    while(!ags_vdsp_256_manager_try_acquire_triple(vdsp_256_manager,
						   AGS_VDSP_256_INT, AGS_VDSP_256_INT, AGS_VDSP_256_INT,
						   &source_arr, &destination_arr, &result_arr)){
      g_thread_yield();
    }

    ags_audio_buffer_util_fill_int_from_s8(NULL,
					   source_arr->vdsp.vec_int->mem_int, 1,
					   source, source_stride,
					   256);

    ags_audio_buffer_util_fill_int_from_s8(NULL,
					   destination_arr->vdsp.vec_int->mem_int, 1,
					   destination, destination_stride,
					   256);
    
    vDSP_vaddi(destination_arr->vdsp.vec_int->mem_int, 1, source_arr->vdsp.vec_int->mem_int, 1, result_arr->vdsp.vec_int->mem_int, 1, 256);
    
    ags_audio_buffer_util_get_int_as_s8(NULL,
					destination, destination_stride,
					result_arr->vdsp.vec_int->mem_int, 1,
					256);

    ags_vdsp_256_manager_release(vdsp_256_manager,
				 source_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 destination_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 result_arr);
    
    destination += (256 * destination_stride);
    source += (256 * source_stride);
    i += 256;
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      int v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      int v_destination[] = {(int) destination[0],
			     (int) destination[(current_dchannel = destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel = source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint8) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint8) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;
      
      destination[0] = 0xff & ((gint16) (destination[0] + source[0]));      
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel = destination_stride)] + source[(current_schannel = source_stride)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xff & ((gint16) (destination[0] + source[0]));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s8_to_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s8_to_s16(AgsAudioBufferUtil *audio_buffer_util,
				     gint16 *destination, guint destination_stride,
				     gint8 *source, guint source_stride,
				     guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 258.00787401574803149606;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s8(NULL,
						  source_arr->vector.vec_double->mem_double, 1,
						  source, source_stride,
						  256);

      ags_audio_buffer_util_fill_v8double_from_s16(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s16(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;

      current_dchannel = 0;
      
      destination[0] = (gint16) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint16) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint16) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#elif defined(AGS_VDSP_256_FUNCTIONS)
  AgsVDSP256Manager *vdsp_256_manager = ags_vdsp_256_manager_get_instance();
  
  while(i + 256 <= limit){
    AgsVDSPArr *source_arr, *destination_arr, *scaled_arr, *result_arr;

    guint j;
    
    while(!ags_vdsp_256_manager_try_acquire_quad(vdsp_256_manager,
						 AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE,
						 &source_arr, &destination_arr, &scaled_arr, &result_arr)){
      g_thread_yield();
    }

    ags_audio_buffer_util_fill_double_from_s8(NULL,
					      source_arr->vdsp.vec_double->mem_double, 1,
					      source, source_stride,
					      256);

    ags_audio_buffer_util_fill_double_from_s16(NULL,
					       destination_arr->vdsp.vec_double->mem_double, 1,
					       destination, destination_stride,
					       256);
    
    vDSP_vsmulD(source_arr->vdsp.vec_double->mem_double, 1, &scale, scaled_arr->vdsp.vec_double->mem_double, 1, 256);
    vDSP_vaddD(destination_arr->vdsp.vec_double->mem_double, 1, scaled_arr->vdsp.vec_double->mem_double, 1, result_arr->vdsp.vec_double->mem_double, 1, 256);
    
    ags_audio_buffer_util_get_double_as_s16(NULL,
					    destination, destination_stride,
					    result_arr->vdsp.vec_double->mem_double, 1,
					    256);
    
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 source_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 destination_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 scaled_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 result_arr);
    
    destination += (256 * destination_stride);
    source += (256 * source_stride);
    i += 256;
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint16) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint16) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffff & ((gint32) (destination[0] + (gint32) (scale * source[0])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel = destination_stride)] + (gint32) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffff & ((gint32) (destination[0] + (gint32) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s8_to_s24:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s8_to_s24(AgsAudioBufferUtil *audio_buffer_util,
				     gint32 *destination, guint destination_stride,
				     gint8 *source, guint source_stride,
				     guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 66052.03149606299212598425;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s8(NULL,
						  source_arr->vector.vec_double->mem_double, 1,
						  source, source_stride,
						  256);

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s32(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;

      current_dchannel = 0;
      
      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint32) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#elif defined(AGS_VDSP_256_FUNCTIONS)
  AgsVDSP256Manager *vdsp_256_manager = ags_vdsp_256_manager_get_instance();
  
  while(i + 256 <= limit){
    AgsVDSPArr *source_arr, *destination_arr, *scaled_arr, *result_arr;

    guint j;
    
    while(!ags_vdsp_256_manager_try_acquire_quad(vdsp_256_manager,
						 AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE,
						 &source_arr, &destination_arr, &scaled_arr, &result_arr)){
      g_thread_yield();
    }

    ags_audio_buffer_util_fill_double_from_s8(NULL,
					      source_arr->vdsp.vec_double->mem_double, 1,
					      source, source_stride,
					      256);

    ags_audio_buffer_util_fill_double_from_s32(NULL,
					       destination_arr->vdsp.vec_double->mem_double, 1,
					       destination, destination_stride,
					       256);
    
    vDSP_vsmulD(source_arr->vdsp.vec_double->mem_double, 1, &scale, scaled_arr->vdsp.vec_double->mem_double, 1, 256);
    vDSP_vaddD(destination_arr->vdsp.vec_double->mem_double, 1, scaled_arr->vdsp.vec_double->mem_double, 1, result_arr->vdsp.vec_double->mem_double, 1, 256);
    
    ags_audio_buffer_util_get_double_as_s32(NULL,
					    destination, destination_stride,
					    result_arr->vdsp.vec_double->mem_double, 1,
					    256);
    
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 source_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 destination_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 scaled_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 result_arr);
    
    destination += (256 * destination_stride);
    source += (256 * source_stride);
    i += 256;
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
    
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & ((gint32) (destination[0] + (gint32) (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel = destination_stride)] + (gint32) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint32) (destination[0] + (gint32) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s8_to_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s8_to_s32(AgsAudioBufferUtil *audio_buffer_util,
				     gint32 *destination, guint destination_stride,
				     gint8 *source, guint source_stride,
				     guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 1690931.99212598425196850393;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s8(NULL,
						  source_arr->vector.vec_double->mem_double, 1,
						  source, source_stride,
						  256);

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s32(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;

      current_dchannel = 0;
      
      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint32) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#elif defined(AGS_VDSP_256_FUNCTIONS)
  AgsVDSP256Manager *vdsp_256_manager = ags_vdsp_256_manager_get_instance();
  
  while(i + 256 <= limit){
    AgsVDSPArr *source_arr, *destination_arr, *scaled_arr, *result_arr;

    guint j;
    
    while(!ags_vdsp_256_manager_try_acquire_quad(vdsp_256_manager,
						 AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE,
						 &source_arr, &destination_arr, &scaled_arr, &result_arr)){
      g_thread_yield();
    }

    ags_audio_buffer_util_fill_double_from_s8(NULL,
					      source_arr->vdsp.vec_double->mem_double, 1,
					      source, source_stride,
					      256);

    ags_audio_buffer_util_fill_double_from_s32(NULL,
					       destination_arr->vdsp.vec_double->mem_double, 1,
					       destination, destination_stride,
					       256);
    
    vDSP_vsmulD(source_arr->vdsp.vec_double->mem_double, 1, &scale, scaled_arr->vdsp.vec_double->mem_double, 1, 256);
    vDSP_vaddD(destination_arr->vdsp.vec_double->mem_double, 1, scaled_arr->vdsp.vec_double->mem_double, 1, result_arr->vdsp.vec_double->mem_double, 1, 256);
    
    ags_audio_buffer_util_get_double_as_s32(NULL,
					    destination, destination_stride,
					    result_arr->vdsp.vec_double->mem_double, 1,
					    256);
    
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 source_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 destination_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 scaled_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 result_arr);
    
    destination += (256 * destination_stride);
    source += (256 * source_stride);
    i += 256;
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & ((gint64) (destination[0] + (gint32) (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel = destination_stride)] + (gint32) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint64) (destination[0] + (gint32) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s8_to_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s8_to_s64(AgsAudioBufferUtil *audio_buffer_util,
				     gint64 *destination, guint destination_stride,
				     gint8 *source, guint source_stride,
				     guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 72624976668147841.00000000000000000000;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s8(NULL,
						  source_arr->vector.vec_double->mem_double, 1,
						  source, source_stride,
						  256);

      ags_audio_buffer_util_fill_v8double_from_s64(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s64(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint64) v_destination[0];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint64) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#elif defined(AGS_VDSP_256_FUNCTIONS)
  AgsVDSP256Manager *vdsp_256_manager = ags_vdsp_256_manager_get_instance();
  
  while(i + 256 <= limit){
    AgsVDSPArr *source_arr, *destination_arr, *scaled_arr, *result_arr;

    guint j;
    
    while(!ags_vdsp_256_manager_try_acquire_quad(vdsp_256_manager,
						 AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE,
						 &source_arr, &destination_arr, &scaled_arr, &result_arr)){
      g_thread_yield();
    }

    ags_audio_buffer_util_fill_double_from_s8(NULL,
					      source_arr->vdsp.vec_double->mem_double, 1,
					      source, source_stride,
					      256);

    ags_audio_buffer_util_fill_double_from_s64(NULL,
					       destination_arr->vdsp.vec_double->mem_double, 1,
					       destination, destination_stride,
					       256);
    
    vDSP_vsmulD(source_arr->vdsp.vec_double->mem_double, 1, &scale, scaled_arr->vdsp.vec_double->mem_double, 1, 256);
    vDSP_vaddD(destination_arr->vdsp.vec_double->mem_double, 1, scaled_arr->vdsp.vec_double->mem_double, 1, result_arr->vdsp.vec_double->mem_double, 1, 256);
    
    ags_audio_buffer_util_get_double_as_s64(NULL,
					    destination, destination_stride,
					    result_arr->vdsp.vec_double->mem_double, 1,
					    256);
    
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 source_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 destination_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 scaled_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 result_arr);
    
    destination += (256 * destination_stride);
    source += (256 * source_stride);
    i += 256;
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint64) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint64) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
    
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + (gint64) (scale * source[0])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel = destination_stride)] + (gint64) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint64) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint64) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint64) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint64) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint64) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint64) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + (gint64) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s8_to_float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s8_to_float(AgsAudioBufferUtil *audio_buffer_util,
				       gfloat *destination, guint destination_stride,
				       gint8 *source, guint source_stride,
				       guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  //  static const gdouble scale = 0.00787401574803149606;
  static const gdouble normalize_divisor = 127.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_FLOAT, AGS_VECTOR_256_FLOAT,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8float_from_s8(NULL,
						 source_arr->vector.vec_float->mem_float, 1,
						 source, source_stride,
						 256);

      ags_audio_buffer_util_fill_v8float(NULL,
					 destination_arr->vector.vec_float->mem_float, 1,
					 destination, destination_stride,
					 256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_float->mem_float[j] /= (gfloat) normalize_divisor;
	
	destination_arr->vector.vec_float->mem_float[j] += source_arr->vector.vec_float->mem_float[j];
      }
    
      ags_audio_buffer_util_fetch_v8float(NULL,
					  destination, destination_stride,
					  destination_arr->vector.vec_float->mem_float, 1,
					  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gfloat) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gfloat) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_normalized[256];
  double v_result[256];

  gint j;

  static const double v_normalize_divisor[1] = { (double) normalize_divisor };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsdivD(v_source, 1, v_normalize_divisor, v_normalized, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gfloat) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#elif defined(AGS_VDSP_256_FUNCTIONS)
  AgsVDSP256Manager *vdsp_256_manager = ags_vdsp_256_manager_get_instance();
  
  while(i + 256 <= limit){
    AgsVDSPArr *source_arr, *destination_arr, *normalized_arr, *result_arr;

    guint j;
    
    while(!ags_vdsp_256_manager_try_acquire_quad(vdsp_256_manager,
						 AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE,
						 &source_arr, &destination_arr, &normalized_arr, &result_arr)){
      g_thread_yield();
    }

    ags_audio_buffer_util_fill_double_from_s8(NULL,
					      source_arr->vdsp.vec_double->mem_double, 1,
					      source, source_stride,
					      256);

    ags_audio_buffer_util_fill_double_from_float(NULL,
						 destination_arr->vdsp.vec_double->mem_double, 1,
						 destination, destination_stride,
						 256);
    
    vDSP_vsdivD(source_arr->vdsp.vec_double->mem_double, 1, &normalize_divisor, normalized_arr->vdsp.vec_double->mem_double, 1, 256);
    vDSP_vaddD(destination_arr->vdsp.vec_double->mem_double, 1, normalized_arr->vdsp.vec_double->mem_double, 1, result_arr->vdsp.vec_double->mem_double, 1, 256);
    
    ags_audio_buffer_util_get_double_as_float(NULL,
					      destination, destination_stride,
					      result_arr->vdsp.vec_double->mem_double, 1,
					      256);

    ags_vdsp_256_manager_release(vdsp_256_manager,
				 source_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 destination_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 result_arr);
    
    destination += (256 * destination_stride);
    source += (256 * source_stride);
    i += 256;
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_normalized[8];
      double v_result[8];
      static const double v_normalize_divisor[1] = { (double) normalize_divisor };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gfloat) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gfloat) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (source[(current_schannel = source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s8_to_double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s8_to_double(AgsAudioBufferUtil *audio_buffer_util,
					gdouble *destination, guint destination_stride,
					gint8 *source, guint source_stride,
					guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  //  static const gdouble scale = 0.00787401574803149606;
  static const gdouble normalize_divisor = 127.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s8(NULL,
						  source_arr->vector.vec_double->mem_double, 1,
						  source, source_stride,
						  256);

      ags_audio_buffer_util_fill_v8double(NULL,
					 destination_arr->vector.vec_double->mem_double, 1,
					 destination, destination_stride,
					 256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] /= normalize_divisor;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double(NULL,
					  destination, destination_stride,
					  destination_arr->vector.vec_double->mem_double, 1,
					  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* normalize and copy */
      v_source /= normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_normalized[256];
  double v_result[256];

  gint j;

  static const double v_normalize_divisor[1] = { (double) normalize_divisor };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsdivD(v_source, 1, v_normalize_divisor, v_normalized, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gdouble) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#elif defined(AGS_VDSP_256_FUNCTIONS)
  AgsVDSP256Manager *vdsp_256_manager = ags_vdsp_256_manager_get_instance();
  
  while(i + 256 <= limit){
    AgsVDSPArr *source_arr, *destination_arr, *normalized_arr, *result_arr;

    guint j;
    
    while(!ags_vdsp_256_manager_try_acquire_quad(vdsp_256_manager,
						 AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE,
						 &source_arr, &destination_arr, &normalized_arr, &result_arr)){
      g_thread_yield();
    }

    ags_audio_buffer_util_fill_double_from_s8(NULL,
					      source_arr->vdsp.vec_double->mem_double, 1,
					      source, source_stride,
					      256);

    ags_audio_buffer_util_fill_double(NULL,
				      destination_arr->vdsp.vec_double->mem_double, 1,
				      destination, destination_stride,
				      256);
    
    vDSP_vsdivD(source_arr->vdsp.vec_double->mem_double, 1, &normalize_divisor, normalized_arr->vdsp.vec_double->mem_double, 1, 256);
    vDSP_vaddD(destination_arr->vdsp.vec_double->mem_double, 1, normalized_arr->vdsp.vec_double->mem_double, 1, result_arr->vdsp.vec_double->mem_double, 1, 256);
    
    ags_audio_buffer_util_get_double(NULL,
				     destination, destination_stride,
				     result_arr->vdsp.vec_double->mem_double, 1,
				     256);

    ags_vdsp_256_manager_release(vdsp_256_manager,
				 source_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 destination_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 result_arr);
    
    destination += (256 * destination_stride);
    source += (256 * source_stride);
    i += 256;
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_normalized[8];
      double v_result[8];
      static const double v_normalize_divisor[1] = { (double) normalize_divisor };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gdouble) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gdouble) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (source[(current_schannel = source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s16_to_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s16_to_s8(AgsAudioBufferUtil *audio_buffer_util,
				     gint8 *destination, guint destination_stride,
				     gint16 *source, guint source_stride,
				     guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00387585070345164342;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s16(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double_from_s8(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s8(NULL,
						 destination, destination_stride,
						 destination_arr->vector.vec_double->mem_double, 1,
						 256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;

      current_dchannel = 0;
      
      destination[0] = (gint8) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint8) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint8) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#elif defined(AGS_VDSP_256_FUNCTIONS)
  AgsVDSP256Manager *vdsp_256_manager = ags_vdsp_256_manager_get_instance();
  
  while(i + 256 <= limit){
    AgsVDSPArr *source_arr, *destination_arr, *scaled_arr, *result_arr;

    guint j;
    
    while(!ags_vdsp_256_manager_try_acquire_quad(vdsp_256_manager,
						 AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE, AGS_VDSP_256_DOUBLE,
						 &source_arr, &destination_arr, &scaled_arr, &result_arr)){
      g_thread_yield();
    }

    ags_audio_buffer_util_fill_double_from_s16(NULL,
					      source_arr->vdsp.vec_double->mem_double, 1,
					      source, source_stride,
					      256);

    ags_audio_buffer_util_fill_double_from_s8(NULL,
					      destination_arr->vdsp.vec_double->mem_double, 1,
					      destination, destination_stride,
					      256);
    
    vDSP_vsmulD(source_arr->vdsp.vec_double->mem_double, 1, &scale, scaled_arr->vdsp.vec_double->mem_double, 1, 256);
    vDSP_vaddD(destination_arr->vdsp.vec_double->mem_double, 1, scaled_arr->vdsp.vec_double->mem_double, 1, result_arr->vdsp.vec_double->mem_double, 1, 256);
    
    ags_audio_buffer_util_get_double_as_s8(NULL,
					   destination, destination_stride,
					   result_arr->vdsp.vec_double->mem_double, 1,
					   256);
    
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 source_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 destination_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 scaled_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 result_arr);
    
    destination += (256 * destination_stride);
    source += (256 * source_stride);
    i += 256;
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint8) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint8) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
    
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xff & ((gint16) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel = destination_stride)] + (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xff & ((gint16) (destination[0] + (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s16_to_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s16_to_s16(AgsAudioBufferUtil *audio_buffer_util,
				      gint16 *destination, guint destination_stride,
				      gint16 *source, guint source_stride,
				      guint count)
{
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_SIGNED_16_BIT, AGS_VECTOR_256_SIGNED_16_BIT,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8s16(NULL,
				       source_arr->vector.vec_s16->mem_s16, 1,
				       source, source_stride,
				       256);

      ags_audio_buffer_util_fill_v8s16(NULL,
				       destination_arr->vector.vec_s16->mem_s16, 1,
				       destination, destination_stride,
				       256);

      for(j = 0; j < 32; j++){
	destination_arr->vector.vec_s16->mem_s16[j] = (ags_v8s16) destination_arr->vector.vec_s16->mem_s16[j] + (ags_v8s16) source_arr->vector.vec_s16->mem_s16[j];
      }
    
      ags_audio_buffer_util_fetch_v8s16(NULL,
					destination, destination_stride,
					destination_arr->vector.vec_s16->mem_s16, 1,
					256);

      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif
  
    for(; i < limit; i += 8){
      ags_v8s16 v_destination;
      ags_v8s16 v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8s16) {destination[0],
				   destination[(current_dchannel = destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)]};
      v_source = (ags_v8s16) {source[0],
			      source[(current_schannel = source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      v_destination += v_source;

      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  int v_destination[256];
  int v_source[256];
  int v_result[256];

  gint j;

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = destination[destination_stride * (i + j)];
      v_source[j] = source[source_stride * (i + j)];

      j++;
    }

    vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint16) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#elif defined(AGS_VDSP_256_FUNCTIONS)
  AgsVDSP256Manager *vdsp_256_manager = ags_vdsp_256_manager_get_instance();
  
  while(i + 256 <= limit){
    AgsVDSPArr *source_arr, *destination_arr, *result_arr;

    guint j;
    
    while(!ags_vdsp_256_manager_try_acquire_triple(vdsp_256_manager,
						   AGS_VDSP_256_INT, AGS_VDSP_256_INT, AGS_VDSP_256_INT,
						   &source_arr, &destination_arr, &result_arr)){
      g_thread_yield();
    }

    ags_audio_buffer_util_fill_int_from_s16(NULL,
					    source_arr->vdsp.vec_int->mem_int, 1,
					    source, source_stride,
					    256);

    ags_audio_buffer_util_fill_int_from_s16(NULL,
					    destination_arr->vdsp.vec_int->mem_int, 1,
					    destination, destination_stride,
					    256);
    
    vDSP_vaddi(destination_arr->vdsp.vec_int->mem_int, 1, source_arr->vdsp.vec_int->mem_int, 1, result_arr->vdsp.vec_int->mem_int, 1, 256);
    
    ags_audio_buffer_util_get_int_as_s16(NULL,
					 destination, destination_stride,
					 result_arr->vdsp.vec_int->mem_int, 1,
					 256);
    
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 source_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 destination_arr);
    ags_vdsp_256_manager_release(vdsp_256_manager,
				 result_arr);
    
    destination += (256 * destination_stride);
    source += (256 * source_stride);
    i += 256;
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      int v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      int v_destination[] = {(int) destination[0],
			     (int) destination[(current_dchannel = destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel = source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint16) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint16) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){      
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (gint16) 0xffff & ((gint32) (destination[0] + source[0]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel = destination_stride)] + source[(current_schannel = source_stride)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif

  for(; i < count; i++){
    destination[0] = (gint16) 0xffff & ((gint32) (destination[0] + source[0]));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s16_to_s24:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s16_to_s24(AgsAudioBufferUtil *audio_buffer_util,
				      gint32 *destination, guint destination_stride,
				      gint16 *source, guint source_stride,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 256.00778221991637928403;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s16(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s32(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;

      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint32) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & ((gint32) (destination[0] + (gint32) (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel = destination_stride)] + (gint32) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint32) (destination[0] + (gint32) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s16_to_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s16_to_s32(AgsAudioBufferUtil *audio_buffer_util,
				      gint32 *destination, guint destination_stride,
				      gint16 *source, guint source_stride,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 6553.79995117038483840449;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s16(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s32(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;

      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint32) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & ((gint64) (destination[0] + (gint32) (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel = destination_stride)] + (gint32) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint64) (destination[0] + (gint32) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s16_to_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s16_to_s64(AgsAudioBufferUtil *audio_buffer_util,
				      gint64 *destination, guint destination_stride,
				      gint16 *source, guint source_stride,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 281483566907400.00021362956633198034;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s16(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double_from_s64(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s64(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint64) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint64) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint64) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint64) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint64) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + (gint64) (scale * source[0])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel = destination_stride)] + (gint64) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint64) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint64) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint64) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint64) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint64) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (gint64) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + (gint64) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s16_to_float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s16_to_float(AgsAudioBufferUtil *audio_buffer_util,
					gfloat *destination, guint destination_stride,
					gint16 *source, guint source_stride,
					guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  //  static const gdouble scale = 0.00003051850947599719;
  static const gdouble normalize_divisor = 32767.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_FLOAT, AGS_VECTOR_256_FLOAT,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8float_from_s16(NULL,
						  source_arr->vector.vec_float->mem_float, 1,
						  source, source_stride,
						  256);

      ags_audio_buffer_util_fill_v8float(NULL,
					 destination_arr->vector.vec_float->mem_float, 1,
					 destination, destination_stride,
					 256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_float->mem_float[j] /= (gfloat) normalize_divisor;
	
	destination_arr->vector.vec_float->mem_float[j] = (ags_v8float) destination_arr->vector.vec_float->mem_float[j] + (ags_v8float) source_arr->vector.vec_float->mem_float[j];
      }
    
      ags_audio_buffer_util_fetch_v8float(NULL,
					  destination, destination_stride,
					  destination_arr->vector.vec_float->mem_float, 1,
					  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gfloat) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gfloat) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_normalized[256];
  double v_result[256];

  gint j;

  static const double v_normalize_divisor[1] = { (double) normalize_divisor };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsdivD(v_source, 1, v_normalize_divisor, v_normalized, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gfloat) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_normalized[8];
      double v_result[8];
      static const double v_normalize_divisor[1] = { (double) normalize_divisor };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gfloat) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gfloat) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (source[(current_schannel = source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s16_to_double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s16_to_double(AgsAudioBufferUtil *audio_buffer_util,
					 gdouble *destination, guint destination_stride,
					 gint16 *source, guint source_stride,
					 guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  //  static const gdouble scale = 0.00003051850947599719;
  static const gdouble normalize_divisor = 32767.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s16(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double(NULL,
					 destination_arr->vector.vec_double->mem_double, 1,
					 destination, destination_stride,
					 256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] /= normalize_divisor;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double(NULL,
					  destination, destination_stride,
					  destination_arr->vector.vec_double->mem_double, 1,
					  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* normalize and copy */
      v_source /= normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_normalized[256];
  double v_result[256];

  gint j;

  static const double v_normalize_divisor[1] = { (double) normalize_divisor };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsdivD(v_source, 1, v_normalize_divisor, v_normalized, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gdouble) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_normalized[8];
      double v_result[8];
      static const double v_normalize_divisor[1] = { (double) normalize_divisor };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gdouble) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gdouble) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (source[(current_schannel = source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s24_to_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s24_to_s8(AgsAudioBufferUtil *audio_buffer_util,
				     gint8 *destination, guint destination_stride,
				     gint32 *source, guint source_stride,
				     guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00001513958157772798;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double_from_s8(NULL,
						  destination_arr->vector.vec_double->mem_double, 1,
						  destination, destination_stride,
						  256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s8(NULL,
						 destination, destination_stride,
						 destination_arr->vector.vec_double->mem_double, 1,
						 256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint8) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint8) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint8) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint8) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint8) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
      
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xff & ((gint16) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel = destination_stride)] + (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xff & ((gint16) (destination[0] + (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s24_to_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s24_to_s16(AgsAudioBufferUtil *audio_buffer_util,
				      gint16 *destination, guint destination_stride,
				      gint32 *source, guint source_stride,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00390613125635758118;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double_from_s16(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s16(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint16) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint16) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint16) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint16) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint16) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffff & ((gint32) (destination[0] + (gint32) (scale * source[0])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel = destination_stride)] + (gint32) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (gint32) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffff & ((gint32) (destination[0] + (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s24_to_s24:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s24_to_s24(AgsAudioBufferUtil *audio_buffer_util,
				      gint32 *destination, guint destination_stride,
				      gint32 *source, guint source_stride,
				      guint count)
{
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_SIGNED_32_BIT, AGS_VECTOR_256_SIGNED_32_BIT,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8s32(NULL,
				       source_arr->vector.vec_s32->mem_s32, 1,
				       source, source_stride,
				       256);

      ags_audio_buffer_util_fill_v8s32(NULL,
				       destination_arr->vector.vec_s32->mem_s32, 1,
				       destination, destination_stride,
				       256);

      for(j = 0; j < 32; j++){
	destination_arr->vector.vec_s32->mem_s32[j] += source_arr->vector.vec_s32->mem_s32[j];
      }
    
      ags_audio_buffer_util_fetch_v8s32(NULL,
					destination, destination_stride,
					destination_arr->vector.vec_s32->mem_s32, 1,
					256);

      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8s32 v_destination;
      ags_v8s32 v_source;

      current_dchannel = 0;
      current_schannel = 0;
      
      v_destination = (ags_v8s32) {destination[0],
				   destination[(current_dchannel = destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)]};

      v_source = (ags_v8s32) {source[0],
			      source[(current_schannel = source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)]};
      
      /* no scale, just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  int v_destination[256];
  int v_source[256];
  int v_result[256];

  gint j;

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = destination[destination_stride * (i + j)];
      v_source[j] = source[source_stride * (i + j)];

      j++;
    }

    vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint32) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      int v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      int v_destination[] = {(int) destination[0],
			     (int) destination[(current_dchannel = destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel = source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & ((gint32) (destination[0] + source[0]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel = destination_stride)] + source[(current_schannel = source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint32) (destination[0] + source[0]));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s24_to_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s24_to_s32(AgsAudioBufferUtil *audio_buffer_util,
				      gint32 *destination, guint destination_stride,
				      gint32 *source, guint source_stride,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 25.60000283718142952697;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s32(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;

      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint32) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
    
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & ((gint64) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel = destination_stride)] + (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint64) (destination[0] + (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s24_to_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s24_to_s64(AgsAudioBufferUtil *audio_buffer_util,
				      gint64 *destination, guint destination_stride,
				      gint32 *source, guint source_stride,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 1099511758848.01562488265334160963;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double_from_s64(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s64(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint64) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint64) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint64) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint64) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint64) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
    
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel = destination_stride)] + (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
 
      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif

  for(; i < count; i++){
    destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s24_to_float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s24_to_float(AgsAudioBufferUtil *audio_buffer_util,
					gfloat *destination, guint destination_stride,
					gint32 *source, guint source_stride,
					guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  //  static const gdouble scale = 0.00000011920930376163;
  static const gdouble normalize_divisor = 8388607.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_FLOAT, AGS_VECTOR_256_FLOAT,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8float_from_s32(NULL,
						  source_arr->vector.vec_float->mem_float, 1,
						  source, source_stride,
						  256);

      ags_audio_buffer_util_fill_v8float(NULL,
					 destination_arr->vector.vec_float->mem_float, 1,
					 destination, destination_stride,
					 256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_float->mem_float[j] /= (gfloat) normalize_divisor;
	
	destination_arr->vector.vec_float->mem_float[j] += source_arr->vector.vec_float->mem_float[j];
      }
    
      ags_audio_buffer_util_fetch_v8float(NULL,
					  destination, destination_stride,
					  destination_arr->vector.vec_float->mem_float, 1,
					  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gfloat) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gfloat) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_normalized[256];
  double v_result[256];

  gint j;

  static const double v_normalize_divisor[1] = { (double) normalize_divisor };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsdivD(v_source, 1, v_normalize_divisor, v_normalized, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gfloat) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_normalized[8];
      double v_result[8];
      static const double v_normalize_divisor[1] = { (double) normalize_divisor };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gfloat) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gfloat) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (source[(current_schannel = source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s24_to_double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s24_to_double(AgsAudioBufferUtil *audio_buffer_util,
					 gdouble *destination, guint destination_stride,
					 gint32 *source, guint source_stride,
					 guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  //  static const gdouble scale = 0.00000011920930376163;
  static const gdouble normalize_divisor = 8388607.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double(NULL,
					 destination_arr->vector.vec_double->mem_double, 1,
					 destination, destination_stride,
					 256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] /= normalize_divisor;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double(NULL,
					  destination, destination_stride,
					  destination_arr->vector.vec_double->mem_double, 1,
					  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* normalize and copy */
      v_source /= normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_normalized[256];
  double v_result[256];

  gint j;

  static const double v_normalize_divisor[1] = { (double) normalize_divisor };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsdivD(v_source, 1, v_normalize_divisor, v_normalized, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gdouble) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_normalized[8];
      double v_result[8];
      static const double v_normalize_divisor[1] = { (double) normalize_divisor };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gdouble) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gdouble) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (source[(current_schannel = source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s32_to_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s32_to_s8(AgsAudioBufferUtil *audio_buffer_util,
				     gint8 *destination, guint destination_stride,
				     gint32 *source, guint source_stride,
				     guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00000059138983983780;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double_from_s8(NULL,
						  destination_arr->vector.vec_double->mem_double, 1,
						  destination, destination_stride,
						  256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s8(NULL,
						 destination, destination_stride,
						 destination_arr->vector.vec_double->mem_double, 1,
						 256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;

      current_dchannel = 0;
      
      destination[0] = (gint8) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint8) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint8) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint8) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint8) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xff & ((gint16) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel = destination_stride)] + (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xff & ((gint16) (destination[0] + (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s32_to_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s32_to_s16(AgsAudioBufferUtil *audio_buffer_util,
				      gint16 *destination, guint destination_stride,
				      gint32 *source, guint source_stride,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00015258323529106482;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double_from_s16(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s16(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint16) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint16) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint16) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint16) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint16) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffff & ((gint32) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel = destination_stride)] + (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gint16) ((gint32) (destination[0] + (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s32_to_s24:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s32_to_s24(AgsAudioBufferUtil *audio_buffer_util,
				      gint32 *destination, guint destination_stride,
				      gint32 *source, guint source_stride,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.03906249548890626240;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s32(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;

      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint32) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & ((gint32) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel = destination_stride)] + (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint32) (destination[0] + (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s32_to_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s32_to_s32(AgsAudioBufferUtil *audio_buffer_util,
				      gint32 *destination, guint destination_stride,
				      gint32 *source, guint source_stride,
				      guint count)
{
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_SIGNED_32_BIT, AGS_VECTOR_256_SIGNED_32_BIT,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8s32(NULL,
				       source_arr->vector.vec_s32->mem_s32, 1,
				       source, source_stride,
				       256);

      ags_audio_buffer_util_fill_v8s32(NULL,
				       destination_arr->vector.vec_s32->mem_s32, 1,
				       destination, destination_stride,
				       256);

      for(j = 0; j < 32; j++){
	destination_arr->vector.vec_s32->mem_s32[j] += source_arr->vector.vec_s32->mem_s32[j];
      }
    
      ags_audio_buffer_util_fetch_v8s32(NULL,
					destination, destination_stride,
					destination_arr->vector.vec_s32->mem_s32, 1,
					256);

      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8s32 v_destination;
      ags_v8s32 v_source;

      current_dchannel = 0;
      current_schannel = 0;
      
      v_destination = (ags_v8s32) {destination[0],
				   destination[(current_dchannel = destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)]};

      v_source = (ags_v8s32) {source[0],
			      source[(current_schannel = source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  int v_destination[256];
  int v_source[256];
  int v_result[256];

  gint j;

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = destination[destination_stride * (i + j)];
      v_source[j] = source[source_stride * (i + j)];

      j++;
    }

    vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint32) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      int v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      int v_destination[] = {(int) destination[0],
			     (int) destination[(current_dchannel = destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel = source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = 0xffffffff & ((gint64) (destination[0] + source[0]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel = destination_stride)] + source[(current_schannel = source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint64) (destination[0] + source[0]));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s32_to_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s32_to_s64(AgsAudioBufferUtil *audio_buffer_util,
				      gint64 *destination, guint destination_stride,
				      gint32 *source, guint source_stride,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00000000002328306417;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double_from_s64(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s64(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint64) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint64) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint64) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint64) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint64) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
      
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel = destination_stride)] + (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s32_to_float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s32_to_float(AgsAudioBufferUtil *audio_buffer_util,
					gfloat *destination, guint destination_stride,
					gint32 *source, guint source_stride,
					guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  //  static const gdouble scale = 0.00000000465661291210;
  static const gdouble normalize_divisor = 214748363.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_FLOAT, AGS_VECTOR_256_FLOAT,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8float_from_s32(NULL,
						  source_arr->vector.vec_float->mem_float, 1,
						  source, source_stride,
						  256);

      ags_audio_buffer_util_fill_v8float(NULL,
					 destination_arr->vector.vec_float->mem_float, 1,
					 destination, destination_stride,
					 256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_float->mem_float[j] /= (gfloat) normalize_divisor;
	
	destination_arr->vector.vec_float->mem_float[j] += source_arr->vector.vec_float->mem_float[j];
      }
    
      ags_audio_buffer_util_fetch_v8float(NULL,
					  destination, destination_stride,
					  destination_arr->vector.vec_float->mem_float, 1,
					  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gfloat) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gfloat) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_normalized[256];
  double v_result[256];

  gint j;

  static const double v_normalize_divisor[1] = { (double) normalize_divisor };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsdivD(v_source, 1, v_normalize_divisor, v_normalized, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gfloat) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_normalized[8];
      double v_result[8];
      static const double v_normalize_divisor[1] = { (double) normalize_divisor };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gfloat) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gfloat) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (source[(current_schannel = source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s32_to_double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s32_to_double(AgsAudioBufferUtil *audio_buffer_util,
					 gdouble *destination, guint destination_stride,
					 gint32 *source, guint source_stride,
					 guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  //  static const gdouble scale = 0.00000000465661291210;
  static const gdouble normalize_divisor = 214748363.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double(NULL,
					 destination_arr->vector.vec_double->mem_double, 1,
					 destination, destination_stride,
					 256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] /= normalize_divisor;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double(NULL,
					  destination, destination_stride,
					  destination_arr->vector.vec_double->mem_double, 1,
					  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* normalize and copy */
      v_source /= normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_normalized[256];
  double v_result[256];

  gint j;

  static const double v_normalize_divisor[1] = { (double) normalize_divisor };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsdivD(v_source, 1, v_normalize_divisor, v_normalized, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gdouble) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_normalized[8];
      double v_result[8];
      static const double v_normalize_divisor[1] = { (double) normalize_divisor };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gdouble) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gdouble) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (source[(current_schannel = source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s64_to_s8(AgsAudioBufferUtil *audio_buffer_util,
				     gint8 *destination, guint destination_stride,
				     gint64 *source, guint source_stride,
				     guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00000000000000001376;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s64(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double_from_s8(NULL,
						  destination_arr->vector.vec_double->mem_double, 1,
						  destination, destination_stride,
						  256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s8(NULL,
						 destination, destination_stride,
						 destination_arr->vector.vec_double->mem_double, 1,
						 256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;

      current_dchannel = 0;
      
      destination[0] = (gint8) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint8) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint8) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint8) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint8) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
      
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xff & ((gint16) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel = destination_stride)] + (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xff & ((gint16) (destination[0] + (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s64_to_s16(AgsAudioBufferUtil *audio_buffer_util,
				      gint16 *destination, guint destination_stride,
				      gint64 *source, guint source_stride,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00000000000000355260;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s64(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double_from_s16(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s16(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint16) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint16) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint16) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint16) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint16) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffff & ((gint32) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel = destination_stride)] + (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffff & ((gint32) (destination[0] + (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_s24:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s64_to_s24(AgsAudioBufferUtil *audio_buffer_util,
				      gint32 *destination, guint destination_stride,
				      gint64 *source, guint source_stride,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00000000000090949459;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s64(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s32(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;

      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint32) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & ((gint32) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel = destination_stride)] + (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint32) (destination[0] + (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s64_to_s32(AgsAudioBufferUtil *audio_buffer_util,
				      gint32 *destination, guint destination_stride,
				      gint64 *source, guint source_stride,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00000000002328306417;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s64(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s32(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;

      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint32) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & ((gint64) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel = destination_stride)] + (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint64) (destination[0] + (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s64_to_s64(AgsAudioBufferUtil *audio_buffer_util,
				      gint64 *destination, guint destination_stride,
				      gint64 *source, guint source_stride,
				      guint count)
{
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;  

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  int v_destination[256];
  int v_source[256];
  int v_result[256];

  gint j;

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = destination[destination_stride * (i + j)];
      v_source[j] = source[source_stride * (i + j)];

      j++;
    }

    vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint64) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_SIGNED_64_BIT, AGS_VECTOR_256_SIGNED_64_BIT,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8s64(NULL,
				       source_arr->vector.vec_s64->mem_s64, 1,
				       source, source_stride,
				       256);

      ags_audio_buffer_util_fill_v8s64(NULL,
				       destination_arr->vector.vec_s64->mem_s64, 1,
				       destination, destination_stride,
				       256);

      for(j = 0; j < 32; j++){
	destination_arr->vector.vec_s64->mem_s64[j] += source_arr->vector.vec_s64->mem_s64[j];
      }
    
      ags_audio_buffer_util_fetch_v8s64(NULL,
					destination, destination_stride,
					destination_arr->vector.vec_s64->mem_s64, 1,
					256);

      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8s64 v_destination;
      ags_v8s64 v_source;

      current_dchannel = 0;
      current_schannel = 0;      

      v_destination = (ags_v8s64) {destination[0],
				   destination[(current_dchannel = destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)],
				   destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8s64) {source[0],
			      source[(current_schannel = source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)],
			      source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      int v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      int v_destination[] = {(int) destination[0],
			     (int) destination[(current_dchannel = destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)],
			     (int) destination[(current_dchannel += destination_stride)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel = source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)],
			(int) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint64) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint64) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + source[0]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel = destination_stride)] + source[(current_schannel = source_stride)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += destination_stride)] + source[(current_schannel += source_stride)]));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + source[0]));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s64_to_float(AgsAudioBufferUtil *audio_buffer_util,
					gfloat *destination, guint destination_stride,
					gint64 *source, guint source_stride,
					guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  //  static const gdouble scale = 0.00000000000000000010;
  static const gdouble normalize_divisor = 9223372036854775807.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_FLOAT, AGS_VECTOR_256_FLOAT,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8float_from_s64(NULL,
						  source_arr->vector.vec_float->mem_float, 1,
						  source, source_stride,
						  256);

      ags_audio_buffer_util_fill_v8float(NULL,
					 destination_arr->vector.vec_float->mem_float, 1,
					 destination, destination_stride,
					 256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_float->mem_float[j] /= (gfloat) normalize_divisor;
	
	destination_arr->vector.vec_float->mem_float[j] += source_arr->vector.vec_float->mem_float[j];
      }
    
      ags_audio_buffer_util_fetch_v8float(NULL,
					  destination, destination_stride,
					  destination_arr->vector.vec_float->mem_float, 1,
					  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gfloat) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gfloat) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_normalized[256];
  double v_result[256];

  gint j;

  static const double v_normalize_divisor[1] = { (double) normalize_divisor };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsdivD(v_source, 1, v_normalize_divisor, v_normalized, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gfloat) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_normalized[8];
      double v_result[8];
      static const double v_normalize_divisor[1] = { (double) normalize_divisor };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gfloat) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gfloat) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (source[(current_schannel = source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s64_to_double(AgsAudioBufferUtil *audio_buffer_util,
					 gdouble *destination, guint destination_stride,
					 gint64 *source, guint source_stride,
					 guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  //  static const gdouble scale = 0.00000000000000000010;
  static const gdouble normalize_divisor = 9223372036854775807.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_s64(NULL,
						   source_arr->vector.vec_double->mem_double, 1,
						   source, source_stride,
						   256);

      ags_audio_buffer_util_fill_v8double(NULL,
					 destination_arr->vector.vec_double->mem_double, 1,
					 destination, destination_stride,
					 256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] /= normalize_divisor;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double(NULL,
					  destination, destination_stride,
					  destination_arr->vector.vec_double->mem_double, 1,
					  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* normalize and copy */
      v_source /= normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_normalized[256];
  double v_result[256];

  gint j;

  static const double v_normalize_divisor[1] = { (double) normalize_divisor };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsdivD(v_source, 1, v_normalize_divisor, v_normalized, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gdouble) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_normalized[8];
      double v_result[8];
      static const double v_normalize_divisor[1] = { (double) normalize_divisor };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gdouble) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gdouble) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);
      
      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (source[(current_schannel = source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_float_to_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float_to_s8(AgsAudioBufferUtil *audio_buffer_util,
				       gint8 *destination, guint destination_stride,
				       gfloat *source, guint source_stride,
				       guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 127.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_FLOAT, AGS_VECTOR_256_FLOAT,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8float(NULL,
					 source_arr->vector.vec_float->mem_float, 1,
					 source, source_stride,
					 256);

      ags_audio_buffer_util_fill_v8float_from_s8(NULL,
						 destination_arr->vector.vec_float->mem_float, 1,
						 destination, destination_stride,
						 256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_float->mem_float[j] *= (gfloat) scale;
	
	destination_arr->vector.vec_float->mem_float[j] += source_arr->vector.vec_float->mem_float[j];
      }
    
      ags_audio_buffer_util_fetch_v8float_as_s8(NULL,
						destination, destination_stride,
						destination_arr->vector.vec_float->mem_float, 1,
						256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {source[0],
				source[(current_schannel = source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint8) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint8) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint8) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint8) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint8) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xff & (gint8) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xff & (gint8) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_float_to_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float_to_s16(AgsAudioBufferUtil *audio_buffer_util,
					gint16 *destination, guint destination_stride,
					gfloat *source, guint source_stride,
					guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 32767.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_FLOAT, AGS_VECTOR_256_FLOAT,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8float(NULL,
					 source_arr->vector.vec_float->mem_float, 1,
					 source, source_stride,
					 256);

      ags_audio_buffer_util_fill_v8float_from_s16(NULL,
						  destination_arr->vector.vec_float->mem_float, 1,
						  destination, destination_stride,
						  256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_float->mem_float[j] *= (gfloat) scale;
	
	destination_arr->vector.vec_float->mem_float[j] = (ags_v8float) destination_arr->vector.vec_float->mem_float[j] + (ags_v8float) source_arr->vector.vec_float->mem_float[j];
      }
    
      ags_audio_buffer_util_fetch_v8float_as_s16(NULL,
						 destination, destination_stride,
						 destination_arr->vector.vec_float->mem_float, 1,
						 256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {source[0],
				source[(current_schannel = source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint16) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint16) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint16) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint16) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint16) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffff & (gint16) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffff & (gint16) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_float_to_s24:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float_to_s24(AgsAudioBufferUtil *audio_buffer_util,
					gint32 *destination, guint destination_stride,
					gfloat *source, guint source_stride,
					guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 8388607.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_FLOAT, AGS_VECTOR_256_FLOAT,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8float(NULL,
					 source_arr->vector.vec_float->mem_float, 1,
					 source, source_stride,
					 256);

      ags_audio_buffer_util_fill_v8float_from_s32(NULL,
						  destination_arr->vector.vec_float->mem_float, 1,
						  destination, destination_stride,
						  256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_float->mem_float[j] *= (gfloat) scale;
	
	destination_arr->vector.vec_float->mem_float[j] += source_arr->vector.vec_float->mem_float[j];
      }
    
      ags_audio_buffer_util_fetch_v8float_as_s32(NULL,
						 destination, destination_stride,
						 destination_arr->vector.vec_float->mem_float, 1,
						 256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {source[0],
				source[(current_schannel = source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint16) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & (gint32) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & (gint32) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_float_to_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float_to_s32(AgsAudioBufferUtil *audio_buffer_util,
					gint32 *destination, guint destination_stride,
					gfloat *source, guint source_stride,
					guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 214748363.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_FLOAT, AGS_VECTOR_256_FLOAT,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8float(NULL,
					 source_arr->vector.vec_float->mem_float, 1,
					 source, source_stride,
					 256);

      ags_audio_buffer_util_fill_v8float_from_s32(NULL,
						  destination_arr->vector.vec_float->mem_float, 1,
						  destination, destination_stride,
						  256);
      
      for(j = 0; j < 32; j++){
	source_arr->vector.vec_float->mem_float[j] *= (gfloat) scale;
	
	destination_arr->vector.vec_float->mem_float[j] += source_arr->vector.vec_float->mem_float[j];
      }
    
      ags_audio_buffer_util_fetch_v8float_as_s32(NULL,
						 destination, destination_stride,
						 destination_arr->vector.vec_float->mem_float, 1,
						 256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {source[0],
				source[(current_schannel = source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint32) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
    
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & (gint32) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & (gint32) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_float_to_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float_to_s64(AgsAudioBufferUtil *audio_buffer_util,
					gint64 *destination, guint destination_stride,
					gfloat *source, guint source_stride,
					guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 9223372036854775807.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_FLOAT, AGS_VECTOR_256_FLOAT,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8float(NULL,
					 source_arr->vector.vec_float->mem_float, 1,
					 source, source_stride,
					 256);

      ags_audio_buffer_util_fill_v8float_from_s64(NULL,
						  destination_arr->vector.vec_float->mem_float, 1,
						  destination, destination_stride,
						  256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_float->mem_float[j] *= (gfloat) scale;
	
	destination_arr->vector.vec_float->mem_float[j] += source_arr->vector.vec_float->mem_float[j];
      }
    
      ags_audio_buffer_util_fetch_v8float_as_s64(NULL,
						 destination, destination_stride,
						 destination_arr->vector.vec_float->mem_float, 1,
						 256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {source[0],
				source[(current_schannel = source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint64) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint64) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint64) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint64) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint64) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_float_to_float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float_to_float(AgsAudioBufferUtil *audio_buffer_util,
					  gfloat *destination, guint destination_stride,
					  gfloat *source, guint source_stride,
					  guint count)
{
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_FLOAT, AGS_VECTOR_256_FLOAT,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8float(NULL,
					 source_arr->vector.vec_float->mem_float, 1,
					 source, source_stride,
					 256);

      ags_audio_buffer_util_fill_v8float(NULL,
					 destination_arr->vector.vec_float->mem_float, 1,
					 destination, destination_stride,
					 256);

      for(j = 0; j < 32; j++){
	destination_arr->vector.vec_float->mem_float[j] += source_arr->vector.vec_float->mem_float[j];
      }
    
      ags_audio_buffer_util_fetch_v8float(NULL,
					  destination, destination_stride,
					  destination_arr->vector.vec_float->mem_float, 1,
					  256);

      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {destination[0],
				     destination[(current_dchannel = destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {source[0],
				source[(current_schannel = source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_result[256];

  gint j;

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = destination[destination_stride * (i + j)];
      v_source[j] = source[source_stride * (i + j)];

      j++;
    }

    vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gfloat) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gfloat) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gfloat) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (source[(current_schannel = source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_float_to_double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float_to_double(AgsAudioBufferUtil *audio_buffer_util,
					   gdouble *destination, guint destination_stride,
					   gfloat *source, guint source_stride,
					   guint count)
{
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double_from_float(NULL,
						     source_arr->vector.vec_double->mem_double, 1,
						     source, source_stride,
						     256);

      ags_audio_buffer_util_fill_v8double(NULL,
					  destination_arr->vector.vec_double->mem_double, 1,
					  destination, destination_stride,
					  256);

      for(j = 0; j < 32; j++){
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double(NULL,
					   destination, destination_stride,
					   destination_arr->vector.vec_double->mem_double, 1,
					   256);

      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {destination[0],
				      destination[(current_dchannel = destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_result[256];

  gint j;

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = destination[destination_stride * (i + j)];
      v_source[j] = source[source_stride * (i + j)];

      j++;
    }

    vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gdouble) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gdouble) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gdouble) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (source[(current_schannel = source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_double_to_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_double_to_s8(AgsAudioBufferUtil *audio_buffer_util,
					gint8 *destination, guint destination_stride,
					gdouble *source, guint source_stride,
					guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 127.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double(NULL,
					  source_arr->vector.vec_double->mem_double, 1,
					  source, source_stride,
					  256);

      ags_audio_buffer_util_fill_v8double_from_s8(NULL,
						  destination_arr->vector.vec_double->mem_double, 1,
						  destination, destination_stride,
						  256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s8(NULL,
						 destination, destination_stride,
						 destination_arr->vector.vec_double->mem_double, 1,
						 256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {source[0],
				 source[(current_schannel = source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint8) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint8) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint8) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint8) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint8) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xff & (gint8) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xff & (gint8) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_double_to_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_double_to_s16(AgsAudioBufferUtil *audio_buffer_util,
					 gint16 *destination, guint destination_stride,
					 gdouble *source, guint source_stride,
					 guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 32767.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double(NULL,
					 source_arr->vector.vec_double->mem_double, 1,
					 source, source_stride,
					 256);

      ags_audio_buffer_util_fill_v8double_from_s16(NULL,
						  destination_arr->vector.vec_double->mem_double, 1,
						  destination, destination_stride,
						  256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s16(NULL,
						 destination, destination_stride,
						 destination_arr->vector.vec_double->mem_double, 1,
						 256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {source[0],
				 source[(current_schannel = source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint16) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint16) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint16) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint16) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint16) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffff & (gint16) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffff & (gint16) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_double_to_s24:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_double_to_s24(AgsAudioBufferUtil *audio_buffer_util,
					 gint32 *destination, guint destination_stride,
					 gdouble *source, guint source_stride,
					 guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 8388607.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double(NULL,
					  source_arr->vector.vec_double->mem_double, 1,
					  source, source_stride,
					  256);

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s32(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {source[0],
				 source[(current_schannel = source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint32) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & (gint32) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & (gint32) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_double_to_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_double_to_s32(AgsAudioBufferUtil *audio_buffer_util,
					 gint32 *destination, guint destination_stride,
					 gdouble *source, guint source_stride,
					 guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 214748363.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double(NULL,
					  source_arr->vector.vec_double->mem_double, 1,
					  source, source_stride,
					  256);

      ags_audio_buffer_util_fill_v8double_from_s32(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s32(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {source[0],
				 source[(current_schannel = source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint32) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & (gint32) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & (gint32) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_double_to_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_double_to_s64(AgsAudioBufferUtil *audio_buffer_util,
					 gint64 *destination, guint destination_stride,
					 gdouble *source, guint source_stride,
					 guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 9223372036854775807.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double(NULL,
					  source_arr->vector.vec_double->mem_double, 1,
					  source, source_stride,
					  256);

      ags_audio_buffer_util_fill_v8double_from_s64(NULL,
						   destination_arr->vector.vec_double->mem_double, 1,
						   destination, destination_stride,
						   256);

      for(j = 0; j < 32; j++){
	source_arr->vector.vec_double->mem_double[j] *= scale;
	
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double_as_s64(NULL,
						  destination, destination_stride,
						  destination_arr->vector.vec_double->mem_double, 1,
						  256);
      
      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)],
				      (gdouble) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {source[0],
				 source[(current_schannel = source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint64) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint64) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_scaled[256];
  double v_result[256];

  gint j;

  static const double v_scale[1] = { (double) scale };

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = (double) destination[destination_stride * (i + j)];
      v_source[j] = (double) source[source_stride * (i + j)];

      j++;
    }

    vDSP_vsmulD(v_source, 1, v_scale, v_scaled, 1, ((count < 256) ? count: 256));
    vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gint64) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint64) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint64) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_double_to_float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_double_to_float(AgsAudioBufferUtil *audio_buffer_util,
					   gfloat *destination, guint destination_stride,
					   gdouble *source, guint source_stride,
					   guint count)
{
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_FLOAT, AGS_VECTOR_256_FLOAT,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8float_from_double(NULL,
						     source_arr->vector.vec_float->mem_float, 1,
						     source, source_stride,
						     256);

      ags_audio_buffer_util_fill_v8float(NULL,
					 destination_arr->vector.vec_float->mem_float, 1,
					 destination, destination_stride,
					 256);

      for(j = 0; j < 32; j++){
	destination_arr->vector.vec_float->mem_float[j] += source_arr->vector.vec_float->mem_float[j];
      }
    
      ags_audio_buffer_util_fetch_v8float(NULL,
					  destination, destination_stride,
					  destination_arr->vector.vec_float->mem_float, 1,
					  256);

      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {destination[0],
				     destination[(current_dchannel = destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      v_destination += v_source;

      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_result[256];

  gint j;

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = destination[destination_stride * (i + j)];
      v_source[j] = source[source_stride * (i + j)];

      j++;
    }

    vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gfloat) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* no scale, just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gfloat) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gfloat) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (source[(current_schannel = source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_double_to_double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_double_to_double(AgsAudioBufferUtil *audio_buffer_util,
					    gdouble *destination, guint destination_stride,
					    gdouble *source, guint source_stride,
					    guint count)
{
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
#if defined(AGS_VECTOR_256_FUNCTIONS)
    AgsVector256Manager *vector_256_manager = ags_vector_256_manager_get_instance();
  
    while(i + 256 <= limit){
      AgsVectorArr *source_arr, *destination_arr;

      guint j;
    
      while(!ags_vector_256_manager_try_acquire_dual(vector_256_manager,
						     AGS_VECTOR_256_DOUBLE, AGS_VECTOR_256_DOUBLE,
						     &source_arr, &destination_arr)){
	g_thread_yield();
      }

      ags_audio_buffer_util_fill_v8double(NULL,
					  source_arr->vector.vec_double->mem_double, 1,
					  source, source_stride,
					  256);

      ags_audio_buffer_util_fill_v8double(NULL,
					  destination_arr->vector.vec_double->mem_double, 1,
					  destination, destination_stride,
					  256);

      for(j = 0; j < 32; j++){
	destination_arr->vector.vec_double->mem_double[j] += source_arr->vector.vec_double->mem_double[j];
      }
    
      ags_audio_buffer_util_fetch_v8double(NULL,
					   destination, destination_stride,
					   destination_arr->vector.vec_double->mem_double, 1,
					   256);

      ags_vector_256_manager_release(vector_256_manager,
				     source_arr);
      ags_vector_256_manager_release(vector_256_manager,
				     destination_arr);

      destination += (256 * destination_stride);
      source += (256 * source_stride);
      i += 256;
    }
#endif

    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {destination[0],
				      destination[(current_dchannel = destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {source[0],
				 source[(current_schannel = source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)],
				 source[(current_schannel += source_stride)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
#if defined(LARGE_VECTOR)
  double v_destination[256];
  double v_source[256];
  double v_result[256];

  gint j;

  limit = count - (count % 256);
  
  while(i < limit){
    j = 0;

    while(i + j < count && j < 256){
      v_destination[j] = destination[destination_stride * (i + j)];
      v_source[j] = source[source_stride * (i + j)];

      j++;
    }

    vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, ((count < 256) ? count: 256));

    j = 0;

    while(i + j < count && j < 256){
      destination[destination_stride * (i + j)] = (gdouble) v_result[j];

      j++;
    }
    
    i += ((count < 256) ? count: 256);
  }
#else
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gdouble) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gdouble) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (source[(current_schannel = source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = ((gdouble) ((destination[0]) + (gdouble) (source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

#ifdef __APPLE__
/**
 * ags_audio_buffer_util_copy_s8_to_float32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s8_to_float32(AgsAudioBufferUtil *audio_buffer_util,
					 Float32 *destination, guint destination_stride,
					 gint8 *source, guint source_stride,
					 guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  //  static const Float64 scale = 0.0078740157480314960629854564334861866115034L;
  static const gdouble normalize_divisor = 128.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (Float32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (Float32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_normalized[8];
      double v_result[8];
      static const double v_normalize_divisor[1] = { (double) normalize_divisor };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (Float32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (Float32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel = destination_stride)] + (Float64) (source[(current_schannel = source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0] / normalize_divisor)));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s16_to_float32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s16_to_float32(AgsAudioBufferUtil *audio_buffer_util,
					  Float32 *destination, guint destination_stride,
					  gint16 *source, guint source_stride,
					  guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  //  static const Float64 scale = 0.0000305185094759971922971274004400890622613L;
  static const gdouble normalize_divisor = 32767.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (Float32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (Float32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_normalized[8];
      double v_result[8];
      static const double v_normalize_divisor[1] = { (double) normalize_divisor };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (Float32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (Float32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel = destination_stride)] + (Float64) (source[(current_schannel = source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0] / normalize_divisor)));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s24_to_float32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s24_to_float32(AgsAudioBufferUtil *audio_buffer_util,
					  Float32 *destination, guint destination_stride,
					  gint32 *source, guint source_stride,
					  guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  //  static const Float64 scale = 0.0000001192093037616376592678982260231634882L;
  static const gdouble normalize_divisor = 8388607.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (Float32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (Float32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_normalized[8];
      double v_result[8];
      static const double v_normalize_divisor[1] = { (double) normalize_divisor };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (Float32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (Float32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
    
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel = destination_stride)] + (Float64) (source[(current_schannel = source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0] / normalize_divisor)));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s32_to_float32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s32_to_float32(AgsAudioBufferUtil *audio_buffer_util,
					  Float32 *destination, guint destination_stride,
					  gint32 *source, guint source_stride,
					  guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  //  static const Float64 scale = 0.0000000004656612875245796924105750826697801L;
  static const gdouble normalize_divisor = 214748363.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (Float32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (Float32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_normalized[8];
      double v_result[8];
      static const double v_normalize_divisor[1] = { (double) normalize_divisor };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (Float32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (Float32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel = destination_stride)] + (Float64) (source[(current_schannel = source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0] / normalize_divisor)));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_float32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s64_to_float32(AgsAudioBufferUtil *audio_buffer_util,
					  Float32 *destination, guint destination_stride,
					  gint64 *source, guint source_stride,
					  guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  //  static const Float64 scale = 0.0000000000000000000722801448323667726912712L;
  static const gdouble normalize_divisor = 9223372036854775807.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (Float32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (Float32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_normalized[8];
      double v_result[8];
      static const double v_normalize_divisor[1] = { (double) normalize_divisor };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (Float32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (Float32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel = destination_stride)] + (Float64) (source[(current_schannel = source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0] / normalize_divisor)));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_float_to_float32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float_to_float32(AgsAudioBufferUtil *audio_buffer_util,
					    Float32 *destination, guint destination_stride,
					    float *source, guint source_stride,
					    guint count)
{
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {source[0],
				source[(current_schannel = source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)],
				source[(current_schannel += source_stride)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (Float32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (Float32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (Float32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (Float32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel = destination_stride)] + (Float64) (source[(current_schannel = source_stride)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_double_to_float32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_double_to_float32(AgsAudioBufferUtil *audio_buffer_util,
					     Float32 *destination, guint destination_stride,
					     gdouble *source, guint source_stride,
					     guint count)
{
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (Float32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (Float32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (Float32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (Float32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel = destination_stride)] + (Float64) (source[(current_schannel = source_stride)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_float32_to_float32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float32_to_float32(AgsAudioBufferUtil *audio_buffer_util,
					      Float32 *destination, guint destination_stride,
					      Float32 *source, guint source_stride,
					      guint count)
{
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = (Float32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (Float32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (Float32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (Float32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (Float32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel = destination_stride)] + (Float64) (source[(current_schannel = source_stride)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += destination_stride)] + (Float64) (source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_float32_to_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float32_to_s8(AgsAudioBufferUtil *audio_buffer_util,
					 gint8 *destination, guint destination_stride,
					 Float32 *source, guint source_stride,
					 guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 127.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint8) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint8) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint8) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint8) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint8) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint8) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xff & (gint8) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xff & (gint8) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_float32_to_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float32_to_s16(AgsAudioBufferUtil *audio_buffer_util,
					  gint16 *destination, guint destination_stride,
					  Float32 *source, guint source_stride,
					  guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 32767.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint16) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint16) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint16) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint16) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint16) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint16) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffff & (gint16) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffff & (gint16) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_float32_to_s24:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float32_to_s24(AgsAudioBufferUtil *audio_buffer_util,
					  gint32 *destination, guint destination_stride,
					  Float32 *source, guint source_stride,
					  guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 8388607.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & (gint32) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & (gint32) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_float32_to_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float32_to_s32(AgsAudioBufferUtil *audio_buffer_util,
					  gint32 *destination, guint destination_stride,
					  Float32 *source, guint source_stride,
					  guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 214748363.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint32) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
    
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & (gint32) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & (gint32) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_float32_to_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float32_to_s64(AgsAudioBufferUtil *audio_buffer_util,
					  gint64 *destination, guint destination_stride,
					  Float32 *source, guint source_stride,
					  guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 9223372036854775807.0;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)],
				     (gfloat) destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint64) v_destination[0];
      destination[(current_dchannel = destination_stride)] = (gint64) v_destination[1];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[2];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[3];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[4];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[5];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[6];
      destination[(current_dchannel += destination_stride)] = (gint64) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_scaled[8];
      double v_result[8];
      static const double v_scale[1] = { (double) scale };

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint64) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gint64) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gint64) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (scale * source[(current_schannel = source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (scale * source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_float32_to_float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float32_to_float(AgsAudioBufferUtil *audio_buffer_util,
					    gfloat *destination, guint destination_stride,
					    Float32 *source, guint source_stride,
					    guint count)
{
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {destination[0],
				     destination[(current_dchannel = destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)],
				     destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)],
				(gfloat) source[(current_schannel += source_stride)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gfloat) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gfloat) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gfloat) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (source[(current_schannel = source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_float32_to_double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float32_to_double(AgsAudioBufferUtil *audio_buffer_util,
					     gdouble *destination, guint destination_stride,
					     Float32 *source, guint source_stride,
					     guint count)
{
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {destination[0],
				      destination[(current_dchannel = destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)],
				      destination[(current_dchannel += destination_stride)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)],
				 (gdouble) source[(current_schannel += source_stride)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = destination_stride)] = v_destination[1];
      destination[(current_dchannel += destination_stride)] = v_destination[2];
      destination[(current_dchannel += destination_stride)] = v_destination[3];
      destination[(current_dchannel += destination_stride)] = v_destination[4];
      destination[(current_dchannel += destination_stride)] = v_destination[5];
      destination[(current_dchannel += destination_stride)] = v_destination[6];
      destination[(current_dchannel += destination_stride)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  /* vectorized function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      double v_result[8];

      current_dchannel = 0;
      current_schannel = 0;

      double v_destination[] = {(double) destination[0],
				(double) destination[(current_dchannel = destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)],
				(double) destination[(current_dchannel += destination_stride)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)],
			   (double) source[(current_schannel += source_stride)]};

      /* just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gdouble) v_result[0];
      destination[(current_dchannel = destination_stride)] = (gdouble) v_result[1];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[2];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[3];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[4];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[5];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[6];
      destination[(current_dchannel += destination_stride)] = (gdouble) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel = destination_stride)] + (gdouble) (source[(current_schannel = source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += destination_stride)] + (gdouble) (source[(current_schannel += source_stride)])));

      /* iterate destination */
      destination += (current_dchannel + destination_stride);

      /* iterate source */
      source += (current_schannel + source_stride);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0])));

    destination += destination_stride;
    source += source_stride;
  }
}
#endif

/**
 * ags_audio_buffer_util_copy_s8_to_complex:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s8_to_complex(AgsAudioBufferUtil *audio_buffer_util,
					 AgsComplex *destination, guint destination_stride,
					 gint8 *source, guint source_stride,
					 guint count)
{
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;
  AgsComplex c_value;

  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  c_ptr = &c_value;
  c_ptr_ptr = &c_ptr;
  
  i = 0;

  for(; i < count; i++){
    double _Complex z0, z1;

    AGS_AUDIO_BUFFER_UTIL_S8_TO_COMPLEX(source[0], c_ptr_ptr);

    z0 = ags_complex_get(destination);
    z1 = ags_complex_get(c_ptr);

    ags_complex_set(destination,
		    z0 + z1);
    
    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s16_to_complex:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s16_to_complex(AgsAudioBufferUtil *audio_buffer_util,
					  AgsComplex *destination, guint destination_stride,
					  gint16 *source, guint source_stride,
					  guint count)
{
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;
  AgsComplex c_value;

  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  c_ptr = &c_value;
  c_ptr_ptr = &c_ptr;
  
  i = 0;

  for(; i < count; i++){
    double _Complex z0, z1;
    
    AGS_AUDIO_BUFFER_UTIL_S16_TO_COMPLEX(source[0], c_ptr_ptr);

    z0 = ags_complex_get(destination);
    z1 = ags_complex_get(c_ptr);

    ags_complex_set(destination,
		    z0 + z1);
    
    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s24_to_complex:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s24_to_complex(AgsAudioBufferUtil *audio_buffer_util,
					  AgsComplex *destination, guint destination_stride,
					  gint32 *source, guint source_stride,
					  guint count)
{
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;
  AgsComplex c_value;

  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  c_ptr = &c_value;
  c_ptr_ptr = &c_ptr;
  
  i = 0;

  for(; i < count; i++){
    double _Complex z0, z1;
    
    AGS_AUDIO_BUFFER_UTIL_S24_TO_COMPLEX(source[0], c_ptr_ptr);

    z0 = ags_complex_get(destination);
    z1 = ags_complex_get(c_ptr);

    ags_complex_set(destination,
		    z0 + z1);
    
    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s32_to_complex:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s32_to_complex(AgsAudioBufferUtil *audio_buffer_util,
					  AgsComplex *destination, guint destination_stride,
					  gint32 *source, guint source_stride,
					  guint count)
{
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;
  AgsComplex c_value;

  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  c_ptr = &c_value;
  c_ptr_ptr = &c_ptr;
  
  i = 0;

  for(; i < count; i++){
    double _Complex z0, z1;
    
    AGS_AUDIO_BUFFER_UTIL_S32_TO_COMPLEX(source[0], c_ptr_ptr);

    z0 = ags_complex_get(destination);
    z1 = ags_complex_get(c_ptr);

    ags_complex_set(destination,
		    z0 + z1);
    
    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_complex:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s64_to_complex(AgsAudioBufferUtil *audio_buffer_util,
					  AgsComplex *destination, guint destination_stride,
					  gint64 *source, guint source_stride,
					  guint count)
{
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;
  AgsComplex c_value;

  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  c_ptr = &c_value;
  c_ptr_ptr = &c_ptr;
  
  i = 0;

  for(; i < count; i++){
    double _Complex z0, z1;
    
    AGS_AUDIO_BUFFER_UTIL_S64_TO_COMPLEX(source[0], c_ptr_ptr);

    z0 = ags_complex_get(destination);
    z1 = ags_complex_get(c_ptr);

    ags_complex_set(destination,
		    z0 + z1);
    
    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_float_to_complex:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float_to_complex(AgsAudioBufferUtil *audio_buffer_util,
					    AgsComplex *destination, guint destination_stride,
					    gfloat *source, guint source_stride,
					    guint count)
{
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;
  AgsComplex c_value;

  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  c_ptr = &c_value;
  c_ptr_ptr = &c_ptr;
  
  i = 0;

  for(; i < count; i++){
    double _Complex z0, z1;
    
    AGS_AUDIO_BUFFER_UTIL_FLOAT_TO_COMPLEX(source[0], c_ptr_ptr);

    z0 = ags_complex_get(destination);
    z1 = ags_complex_get(c_ptr);

    ags_complex_set(destination,
		    z0 + z1);
    
    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_double_to_complex:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_double_to_complex(AgsAudioBufferUtil *audio_buffer_util,
					     AgsComplex *destination, guint destination_stride,
					     gdouble *source, guint source_stride,
					     guint count)
{
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;
  AgsComplex c_value;

  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  c_ptr = &c_value;
  c_ptr_ptr = &c_ptr;
  
  i = 0;

  for(; i < count; i++){
    double _Complex z0, z1;
    
    AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(source[0], c_ptr_ptr);

    z0 = ags_complex_get(destination);
    z1 = ags_complex_get(c_ptr);

    ags_complex_set(destination,
		    z0 + z1);
    
    destination += destination_stride;
    source += source_stride;
  }
}

#ifdef __APPLE__
/**
 * ags_audio_buffer_util_copy_float32_to_complex:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float32_to_complex(AgsAudioBufferUtil *audio_buffer_util,
					      AgsComplex *destination, guint destination_stride,
					      Float32 *source, guint source_stride,
					      guint count)
{
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;
  AgsComplex c_value;

  gfloat *f_ptr;
  gfloat f_value;
  
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  c_ptr = &c_value;
  c_ptr_ptr = &c_ptr;
  
  f_ptr = &f_value;
  
  i = 0;

  for(; i < count; i++){
    double _Complex z0, z1;

    f_value = source[0];
    
    AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(f_value, c_ptr_ptr);

    z0 = ags_complex_get(destination);
    z1 = ags_complex_get(c_ptr);

    ags_complex_set(destination,
		    z0 + z1);
    
    destination += destination_stride;
    source += source_stride;
  }
}
#endif

/**
 * ags_audio_buffer_util_copy_complex_to_complex:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_complex_to_complex(AgsAudioBufferUtil *audio_buffer_util,
					      AgsComplex *destination, guint destination_stride,
					      AgsComplex *source, guint source_stride,
					      guint count)
{  
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;

  for(; i < count; i++){
    double _Complex z0, z1;

    z0 = ags_complex_get(destination);
    z1 = ags_complex_get(source);

    ags_complex_set(destination,
		    z0 + z1);
    
    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_complex_to_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_complex_to_s8(AgsAudioBufferUtil *audio_buffer_util,
					 gint8 *destination, guint destination_stride,
					 AgsComplex *source, guint source_stride,
					 guint count)
{
  gint8 *ptr;

  gint8 value;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;
  
  ptr = &value;

  for(; i < count; i++){
    AGS_AUDIO_BUFFER_UTIL_COMPLEX_TO_S8(source, ptr);

    destination[0] = 0xff & ((gint16) (destination[0] + value));      
    
    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_complex_to_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_complex_to_s16(AgsAudioBufferUtil *audio_buffer_util,
					  gint16 *destination, guint destination_stride,
					  AgsComplex *source, guint source_stride,
					  guint count)
{
  gint16 *ptr;

  gint16 value;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;

  ptr = &value;    

  for(; i < count; i++){
    AGS_AUDIO_BUFFER_UTIL_COMPLEX_TO_S16(source, ptr);

    destination[0] = (gint16) 0xffff & ((gint32) (destination[0] + value));
    
    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_complex_to_s24:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_complex_to_s24(AgsAudioBufferUtil *audio_buffer_util,
					  gint32 *destination, guint destination_stride,
					  AgsComplex *source, guint source_stride,
					  guint count)
{
  gint32 *ptr;

  gint32 value;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;

  ptr = &value;    

  for(; i < count; i++){
    AGS_AUDIO_BUFFER_UTIL_COMPLEX_TO_S24(source, ptr);

    destination[0] = 0xffffffff & ((gint32) (destination[0] + value));
    
    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_complex_to_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_complex_to_s32(AgsAudioBufferUtil *audio_buffer_util,
					  gint32 *destination, guint destination_stride,
					  AgsComplex *source, guint source_stride,
					  guint count)
{
  gint32 *ptr;

  gint32 value;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;

  ptr = &value;    

  for(; i < count; i++){
    AGS_AUDIO_BUFFER_UTIL_COMPLEX_TO_S32(source, ptr);

    destination[0] = 0xffffffff & ((gint64) (destination[0] + value));
    
    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_complex_to_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_complex_to_s64(AgsAudioBufferUtil *audio_buffer_util,
					  gint64 *destination, guint destination_stride,
					  AgsComplex *source, guint source_stride,
					  guint count)
{
  gint64 *ptr;
  
  gint64 value;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;

  ptr = &value;    

  for(; i < count; i++){
    AGS_AUDIO_BUFFER_UTIL_COMPLEX_TO_S64(source, ptr);

    destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + value));
    
    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_complex_to_float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_complex_to_float(AgsAudioBufferUtil *audio_buffer_util,
					    gfloat *destination, guint destination_stride,
					    AgsComplex *source, guint source_stride,
					    guint count)
{
  gfloat *ptr;
  
  gfloat value;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;

  ptr = &value;    

  for(; i < count; i++){
    AGS_AUDIO_BUFFER_UTIL_COMPLEX_TO_FLOAT(source, ptr);

    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (value)));
    
    destination += destination_stride;
    source += source_stride;
  }
}

/**
 * ags_audio_buffer_util_copy_complex_to_double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_complex_to_double(AgsAudioBufferUtil *audio_buffer_util,
					     gdouble *destination, guint destination_stride,
					     AgsComplex *source, guint source_stride,
					     guint count)
{
  gdouble *ptr;
  
  gdouble value;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;

  ptr = &value;    

  for(; i < count; i++){
    AGS_AUDIO_BUFFER_UTIL_COMPLEX_TO_DOUBLE(source, ptr);

    destination[0] = ((gdouble) ((destination[0]) + (value)));
    
    destination += destination_stride;
    source += source_stride;
  }
}

#ifdef __APPLE__
/**
 * ags_audio_buffer_util_copy_complex_to_float32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_complex_to_float32(AgsAudioBufferUtil *audio_buffer_util,
					      Float32 *destination, guint destination_stride,
					      AgsComplex *source, guint source_stride,
					      guint count)
{
  gdouble *ptr;
  
  gdouble value;
  guint limit;
  guint current_dchannel, current_schannel;
  guint i;

  if(destination == NULL ||
     source == NULL){
    return;
  }

  i = 0;

  ptr = &value;    

  for(; i < count; i++){
    AGS_AUDIO_BUFFER_UTIL_COMPLEX_TO_DOUBLE(source, ptr);

    destination[0] = ((Float32) ((gdouble) (destination[0]) + (value)));
    
    destination += destination_stride;
    source += source_stride;
  }
}
#endif

/**
 * ags_audio_buffer_util_put_int:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with integer data.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_int(AgsAudioBufferUtil *audio_buffer_util,
			      gint *destination, guint destination_stride,
			      gint *source, guint source_stride,
			      guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_put_float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with single precision floating point data.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_float(AgsAudioBufferUtil *audio_buffer_util,
				gfloat *destination, guint destination_stride,
				gfloat *source, guint source_stride,
				guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_put_double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with double precision floating point data.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_double(AgsAudioBufferUtil *audio_buffer_util,
				 gdouble *destination, guint destination_stride,
				 gdouble *source, guint source_stride,
				 guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_put_int_from_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with single precision floating point data from signed 8 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_int_from_s8(AgsAudioBufferUtil *audio_buffer_util,
				      gint *destination, guint destination_stride,
				      gint8 *source, guint source_stride,
				      guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gint) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_put_int_from_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with single precision floating point data from signed 16 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_int_from_s16(AgsAudioBufferUtil *audio_buffer_util,
				       gint *destination, guint destination_stride,
				       gint16 *source, guint source_stride,
				       guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gint) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_put_int_from_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with single precision floating point data from signed 32 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_int_from_s32(AgsAudioBufferUtil *audio_buffer_util,
				       gint *destination, guint destination_stride,
				       gint32 *source, guint source_stride,
				       guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gint) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_put_int_from_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with single precision floating point data from signed 64 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_int_from_s64(AgsAudioBufferUtil *audio_buffer_util,
				       gint *destination, guint destination_stride,
				       gint64 *source, guint source_stride,
				       guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gint) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_put_int_from_float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with single precision floating point data from float.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_int_from_float(AgsAudioBufferUtil *audio_buffer_util,
					 gint *destination, guint destination_stride,
					 gfloat *source, guint source_stride,
					 guint count)

{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gint) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_put_int_from_double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with single precision floating point data from double.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_int_from_double(AgsAudioBufferUtil *audio_buffer_util,
					  gint *destination, guint destination_stride,
					  gdouble *source, guint source_stride,
					  guint count)

{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gint) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_get_int_as_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Read vector with integer as signed 8 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_get_int_as_s8(AgsAudioBufferUtil *audio_buffer_util,
				    gint8 *destination, guint destination_stride,
				    gint *source, guint source_stride,
				    guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gint8) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_get_int_as_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Read vector with integer as signed 16 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_get_int_as_s16(AgsAudioBufferUtil *audio_buffer_util,
				     gint16 *destination, guint destination_stride,
				     gint *source, guint source_stride,
				     guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gint16) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_get_int_as_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Read vector with integer as signed 32 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_get_int_as_s32(AgsAudioBufferUtil *audio_buffer_util,
				     gint32 *destination, guint destination_stride,
				     gint *source, guint source_stride,
				     guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gint32) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_get_int_as_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Read vector with integer as signed 64 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_get_int_as_s64(AgsAudioBufferUtil *audio_buffer_util,
				     gint64 *destination, guint destination_stride,
				     gint *source, guint source_stride,
				     guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gint64) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_get_int_as_float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Read vector with integer as float.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_get_int_as_float(AgsAudioBufferUtil *audio_buffer_util,
				       gfloat *destination, guint destination_stride,
				       gint *source, guint source_stride,
				       guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gfloat) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_get_int_as_double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Read vector with integer as double.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_get_int_as_double(AgsAudioBufferUtil *audio_buffer_util,
					gdouble *destination, guint destination_stride,
					gint *source, guint source_stride,
					guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gdouble) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_put_float_from_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with single precision floating point data from signed 8 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_float_from_s8(AgsAudioBufferUtil *audio_buffer_util,
					gfloat *destination, guint destination_stride,
					gint8 *source, guint source_stride,
					guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gfloat) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_put_float_from_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with single precision floating point data from signed 16 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_float_from_s16(AgsAudioBufferUtil *audio_buffer_util,
					 gfloat *destination, guint destination_stride,
					 gint16 *source, guint source_stride,
					 guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gfloat) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_put_float_from_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with single precision floating point data from signed 32 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_float_from_s32(AgsAudioBufferUtil *audio_buffer_util,
					 gfloat *destination, guint destination_stride,
					 gint32 *source, guint source_stride,
					 guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gfloat) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_put_float_from_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with single precision floating point data from signed 64 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_float_from_s64(AgsAudioBufferUtil *audio_buffer_util,
					 gfloat *destination, guint destination_stride,
					 gint64 *source, guint source_stride,
					 guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gfloat) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_put_float_from_double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with single precision floating point data from double.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_float_from_double(AgsAudioBufferUtil *audio_buffer_util,
					    gfloat *destination, guint destination_stride,
					    gdouble *source, guint source_stride,
					    guint count)

{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gfloat) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_get_float_as_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Read vector with single precision floating point data as signed 8 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_get_float_as_s8(AgsAudioBufferUtil *audio_buffer_util,
				      gint8 *destination, guint destination_stride,
				      gfloat *source, guint source_stride,
				      guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gint8) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_get_float_as_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Read vector with single precision floating point data as signed 16 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_get_float_as_s16(AgsAudioBufferUtil *audio_buffer_util,
				       gint16 *destination, guint destination_stride,
				       gfloat *source, guint source_stride,
				       guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gint16) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_get_float_as_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Read vector with single precision floating point data as signed 32 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_get_float_as_s32(AgsAudioBufferUtil *audio_buffer_util,
				       gint32 *destination, guint destination_stride,
				       gfloat *source, guint source_stride,
				       guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gint32) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_get_float_as_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Read vector with single precision floating point data as signed 64 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_get_float_as_s64(AgsAudioBufferUtil *audio_buffer_util,
				       gint64 *destination, guint destination_stride,
				       gfloat *source, guint source_stride,
				       guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gint64) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_get_float_as_double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Read vector with single precision floating point data as double.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_get_float_as_double(AgsAudioBufferUtil *audio_buffer_util,
					  gdouble *destination, guint destination_stride,
					  gfloat *source, guint source_stride,
					  guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gdouble) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_put_double_from_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with double precision floating point data from signed 8 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_double_from_s8(AgsAudioBufferUtil *audio_buffer_util,
					 gdouble *destination, guint destination_stride,
					 gint8 *source, guint source_stride,
					 guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gdouble) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_put_double_from_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with double precision floating point data from signed 16 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_double_from_s16(AgsAudioBufferUtil *audio_buffer_util,
					  gdouble *destination, guint destination_stride,
					  gint16 *source, guint source_stride,
					  guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gdouble) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_put_double_from_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with double precision floating point data from signed 32 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_double_from_s32(AgsAudioBufferUtil *audio_buffer_util,
					  gdouble *destination, guint destination_stride,
					  gint32 *source, guint source_stride,
					  guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gdouble) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_put_double_from_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with double precision floating point data from signed 64 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_double_from_s64(AgsAudioBufferUtil *audio_buffer_util,
					  gdouble *destination, guint destination_stride,
					  gint64 *source, guint source_stride,
					  guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gdouble) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_put_double_from_float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with double precision floating point data from float.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_put_double_from_float(AgsAudioBufferUtil *audio_buffer_util,
					    gdouble *destination, guint destination_stride,
					    gfloat *source, guint source_stride,
					    guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gdouble) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_get_double_as_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Read vector with double precision floating point data as signed 8 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_get_double_as_s8(AgsAudioBufferUtil *audio_buffer_util,
				       gint8 *destination, guint destination_stride,
				       gdouble *source, guint source_stride,
				       guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gint8) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_get_double_as_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Read vector with double precision floating point data as signed 16 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_get_double_as_s16(AgsAudioBufferUtil *audio_buffer_util,
					gint16 *destination, guint destination_stride,
					gdouble *source, guint source_stride,
					guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gint16) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_get_double_as_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Read vector with double precision floating point data as signed 32 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_get_double_as_s32(AgsAudioBufferUtil *audio_buffer_util,
					gint32 *destination, guint destination_stride,
					gdouble *source, guint source_stride,
					guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gint32) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_get_double_as_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Read vector with double precision floating point data as signed 64 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_get_double_as_s64(AgsAudioBufferUtil *audio_buffer_util,
					gint64 *destination, guint destination_stride,
					gdouble *source, guint source_stride,
					guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gint64) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_get_double_as_float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Read vector with double precision floating point data as float.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_get_double_as_float(AgsAudioBufferUtil *audio_buffer_util,
					  gfloat *destination, guint destination_stride,
					  gdouble *source, guint source_stride,
					  guint count)
{
  guint i;

  for(i = 0; i < count; i++){
    destination[i * destination_stride] = (gfloat) source[i * source_stride];
  }
}

/**
 * ags_audio_buffer_util_fill_v8s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with signed 8 bit data.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fill_v8s8(AgsAudioBufferUtil *audio_buffer_util,
				ags_v8s8 *destination, guint destination_stride,
				gint8 *source, guint source_stride,
				guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * destination_stride] = (ags_v8s8) {source[i * 8 * source_stride],
						      source[(i * 8 + 1) * source_stride],
						      source[(i * 8 + 2) * source_stride],
						      source[(i * 8 + 3) * source_stride],
						      source[(i * 8 + 4) * source_stride],
						      source[(i * 8 + 5) * source_stride],
						      source[(i * 8 + 6) * source_stride],
						      source[(i * 8 + 7) * source_stride]};
  }
}

/**
 * ags_audio_buffer_util_fill_v8s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with signed 16 bit data.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fill_v8s16(AgsAudioBufferUtil *audio_buffer_util,
				 ags_v8s16 *destination, guint destination_stride,
				 gint16 *source, guint source_stride,
				 guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * destination_stride] = (ags_v8s16) {source[i * 8 * source_stride],
						       source[(i * 8 + 1) * source_stride],
						       source[(i * 8 + 2) * source_stride],
						       source[(i * 8 + 3) * source_stride],
						       source[(i * 8 + 4) * source_stride],
						       source[(i * 8 + 5) * source_stride],
						       source[(i * 8 + 6) * source_stride],
						       source[(i * 8 + 7) * source_stride]};
  }
}

/**
 * ags_audio_buffer_util_fill_v8s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with signed 32 bit data.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fill_v8s32(AgsAudioBufferUtil *audio_buffer_util,
				 ags_v8s32 *destination, guint destination_stride,
				 gint32 *source, guint source_stride,
				 guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * destination_stride] = (ags_v8s32) {source[i * 8 * source_stride],
						       source[(i * 8 + 1) * source_stride],
						       source[(i * 8 + 2) * source_stride],
						       source[(i * 8 + 3) * source_stride],
						       source[(i * 8 + 4) * source_stride],
						       source[(i * 8 + 5) * source_stride],
						       source[(i * 8 + 6) * source_stride],
						       source[(i * 8 + 7) * source_stride]};
  }
}

/**
 * ags_audio_buffer_util_fill_v8s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with signed 64 bit data.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fill_v8s64(AgsAudioBufferUtil *audio_buffer_util,
				 ags_v8s64 *destination, guint destination_stride,
				 gint64 *source, guint source_stride,
				 guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * destination_stride] = (ags_v8s64) {source[i * 8 * source_stride],
						       source[(i * 8 + 1) * source_stride],
						       source[(i * 8 + 2) * source_stride],
						       source[(i * 8 + 3) * source_stride],
						       source[(i * 8 + 4) * source_stride],
						       source[(i * 8 + 5) * source_stride],
						       source[(i * 8 + 6) * source_stride],
						       source[(i * 8 + 7) * source_stride]};
  }
}

/**
 * ags_audio_buffer_util_fill_v8float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with single precision floating point data.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fill_v8float(AgsAudioBufferUtil *audio_buffer_util,
				   ags_v8float *destination, guint destination_stride,
				   gfloat *source, guint source_stride,
				   guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * destination_stride] = (ags_v8float) {source[i * 8 * source_stride],
							 source[(i * 8 + 1) * source_stride],
							 source[(i * 8 + 2) * source_stride],
							 source[(i * 8 + 3) * source_stride],
							 source[(i * 8 + 4) * source_stride],
							 source[(i * 8 + 5) * source_stride],
							 source[(i * 8 + 6) * source_stride],
							 source[(i * 8 + 7) * source_stride]};
  }
}

/**
 * ags_audio_buffer_util_fill_v8double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with double precision floating point data.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fill_v8double(AgsAudioBufferUtil *audio_buffer_util,
				    ags_v8double *destination, guint destination_stride,
				    gdouble *source, guint source_stride,
				    guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * destination_stride] = (ags_v8double) {source[i * 8 * source_stride],
							  source[(i * 8 + 1) * source_stride],
							  source[(i * 8 + 2) * source_stride],
							  source[(i * 8 + 3) * source_stride],
							  source[(i * 8 + 4) * source_stride],
							  source[(i * 8 + 5) * source_stride],
							  source[(i * 8 + 6) * source_stride],
							  source[(i * 8 + 7) * source_stride]};
  }
}

/**
 * ags_audio_buffer_util_fetch_v8s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source vector
 * @source_stride: source vector's count of channels
 * @count: number of frames to copy
 *
 * Fetch signed 8 bit data from vector.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fetch_v8s8(AgsAudioBufferUtil *audio_buffer_util,
				 gint8 *destination, guint destination_stride,
				 ags_v8s8 *source, guint source_stride,
				 guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * 8 * destination_stride] = source[i * source_stride][0];
    destination[(i * 8 + 1) * destination_stride] = source[i * source_stride][1];
    destination[(i * 8 + 2) * destination_stride] = source[i * source_stride][2];
    destination[(i * 8 + 3) * destination_stride] = source[i * source_stride][3];
    destination[(i * 8 + 4) * destination_stride] = source[i * source_stride][4];
    destination[(i * 8 + 5) * destination_stride] = source[i * source_stride][5];
    destination[(i * 8 + 6) * destination_stride] = source[i * source_stride][6];
    destination[(i * 8 + 7) * destination_stride] = source[i * source_stride][7];
  }
}

/**
 * ags_audio_buffer_util_fetch_v8s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source vector
 * @source_stride: source vector's count of channels
 * @count: number of frames to copy
 *
 * Fetch signed 16 bit data from vector.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fetch_v8s16(AgsAudioBufferUtil *audio_buffer_util,
				  gint16 *destination, guint destination_stride,
				  ags_v8s16 *source, guint source_stride,
				  guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * 8 * destination_stride] = source[i * source_stride][0];
    destination[(i * 8 + 1) * destination_stride] = source[i * source_stride][1];
    destination[(i * 8 + 2) * destination_stride] = source[i * source_stride][2];
    destination[(i * 8 + 3) * destination_stride] = source[i * source_stride][3];
    destination[(i * 8 + 4) * destination_stride] = source[i * source_stride][4];
    destination[(i * 8 + 5) * destination_stride] = source[i * source_stride][5];
    destination[(i * 8 + 6) * destination_stride] = source[i * source_stride][6];
    destination[(i * 8 + 7) * destination_stride] = source[i * source_stride][7];
  }
}

/**
 * ags_audio_buffer_util_fetch_v8s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source vector
 * @source_stride: source vector's count of channels
 * @count: number of frames to copy
 *
 * Fetch signed 32 bit data from vector.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fetch_v8s32(AgsAudioBufferUtil *audio_buffer_util,
				  gint32 *destination, guint destination_stride,
				  ags_v8s32 *source, guint source_stride,
				  guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * 8 * destination_stride] = source[i * source_stride][0];
    destination[(i * 8 + 1) * destination_stride] = source[i * source_stride][1];
    destination[(i * 8 + 2) * destination_stride] = source[i * source_stride][2];
    destination[(i * 8 + 3) * destination_stride] = source[i * source_stride][3];
    destination[(i * 8 + 4) * destination_stride] = source[i * source_stride][4];
    destination[(i * 8 + 5) * destination_stride] = source[i * source_stride][5];
    destination[(i * 8 + 6) * destination_stride] = source[i * source_stride][6];
    destination[(i * 8 + 7) * destination_stride] = source[i * source_stride][7];
  }
}

/**
 * ags_audio_buffer_util_fetch_v8s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source vector
 * @source_stride: source vector's count of channels
 * @count: number of frames to copy
 *
 * Fetch signed 64 bit data from vector.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fetch_v8s64(AgsAudioBufferUtil *audio_buffer_util,
				  gint64 *destination, guint destination_stride,
				  ags_v8s64 *source, guint source_stride,
				  guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * 8 * destination_stride] = source[i * source_stride][0];
    destination[(i * 8 + 1) * destination_stride] = source[i * source_stride][1];
    destination[(i * 8 + 2) * destination_stride] = source[i * source_stride][2];
    destination[(i * 8 + 3) * destination_stride] = source[i * source_stride][3];
    destination[(i * 8 + 4) * destination_stride] = source[i * source_stride][4];
    destination[(i * 8 + 5) * destination_stride] = source[i * source_stride][5];
    destination[(i * 8 + 6) * destination_stride] = source[i * source_stride][6];
    destination[(i * 8 + 7) * destination_stride] = source[i * source_stride][7];
  }
}

/**
 * ags_audio_buffer_util_fetch_v8float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source vector
 * @source_stride: source vector's count of channels
 * @count: number of frames to copy
 *
 * Fetch single precision floating point data from vector.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fetch_v8float(AgsAudioBufferUtil *audio_buffer_util,
				    gfloat *destination, guint destination_stride,
				    ags_v8float *source, guint source_stride,
				    guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * 8 * destination_stride] = source[i * source_stride][0];
    destination[(i * 8 + 1) * destination_stride] = source[i * source_stride][1];
    destination[(i * 8 + 2) * destination_stride] = source[i * source_stride][2];
    destination[(i * 8 + 3) * destination_stride] = source[i * source_stride][3];
    destination[(i * 8 + 4) * destination_stride] = source[i * source_stride][4];
    destination[(i * 8 + 5) * destination_stride] = source[i * source_stride][5];
    destination[(i * 8 + 6) * destination_stride] = source[i * source_stride][6];
    destination[(i * 8 + 7) * destination_stride] = source[i * source_stride][7];
  }
}

/**
 * ags_audio_buffer_util_fetch_v8double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source vector
 * @source_stride: source vector's count of channels
 * @count: number of frames to copy
 *
 * Fetch double precision floating point data from vector.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fetch_v8double(AgsAudioBufferUtil *audio_buffer_util,
				     gdouble *destination, guint destination_stride,
				     ags_v8double *source, guint source_stride,
				     guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * 8 * destination_stride] = source[i * source_stride][0];
    destination[(i * 8 + 1) * destination_stride] = source[i * source_stride][1];
    destination[(i * 8 + 2) * destination_stride] = source[i * source_stride][2];
    destination[(i * 8 + 3) * destination_stride] = source[i * source_stride][3];
    destination[(i * 8 + 4) * destination_stride] = source[i * source_stride][4];
    destination[(i * 8 + 5) * destination_stride] = source[i * source_stride][5];
    destination[(i * 8 + 6) * destination_stride] = source[i * source_stride][6];
    destination[(i * 8 + 7) * destination_stride] = source[i * source_stride][7];
  }
}

/**
 * ags_audio_buffer_util_fill_v8float_from_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with float precision floating point data from signed 8 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fill_v8float_from_s8(AgsAudioBufferUtil *audio_buffer_util,
					   ags_v8float *destination, guint destination_stride,
					   gint8 *source, guint source_stride,
					   guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * destination_stride] = (ags_v8float) {source[i * 8 * source_stride],
							 source[(i * 8 + 1) * source_stride],
							 source[(i * 8 + 2) * source_stride],
							 source[(i * 8 + 3) * source_stride],
							 source[(i * 8 + 4) * source_stride],
							 source[(i * 8 + 5) * source_stride],
							 source[(i * 8 + 6) * source_stride],
							 source[(i * 8 + 7) * source_stride]};
  }
}

/**
 * ags_audio_buffer_util_fill_v8float_from_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with float precision floating point data from signed 16 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fill_v8float_from_s16(AgsAudioBufferUtil *audio_buffer_util,
					    ags_v8float *destination, guint destination_stride,
					    gint16 *source, guint source_stride,
					    guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * destination_stride] = (ags_v8float) {source[i * 8 * source_stride],
							 source[(i * 8 + 1) * source_stride],
							 source[(i * 8 + 2) * source_stride],
							 source[(i * 8 + 3) * source_stride],
							 source[(i * 8 + 4) * source_stride],
							 source[(i * 8 + 5) * source_stride],
							 source[(i * 8 + 6) * source_stride],
							 source[(i * 8 + 7) * source_stride]};
  }
}

/**
 * ags_audio_buffer_util_fill_v8float_from_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with float precision floating point data from signed 32 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fill_v8float_from_s32(AgsAudioBufferUtil *audio_buffer_util,
					    ags_v8float *destination, guint destination_stride,
					    gint32 *source, guint source_stride,
					    guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * destination_stride] = (ags_v8float) {source[i * 8 * source_stride],
							 source[(i * 8 + 1) * source_stride],
							 source[(i * 8 + 2) * source_stride],
							 source[(i * 8 + 3) * source_stride],
							 source[(i * 8 + 4) * source_stride],
							 source[(i * 8 + 5) * source_stride],
							 source[(i * 8 + 6) * source_stride],
							 source[(i * 8 + 7) * source_stride]};
  }
}

/**
 * ags_audio_buffer_util_fill_v8float_from_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with float precision floating point data from signed 64 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fill_v8float_from_s64(AgsAudioBufferUtil *audio_buffer_util,
					    ags_v8float *destination, guint destination_stride,
					    gint64 *source, guint source_stride,
					    guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * destination_stride] = (ags_v8float) {source[i * 8 * source_stride],
							 source[(i * 8 + 1) * source_stride],
							 source[(i * 8 + 2) * source_stride],
							 source[(i * 8 + 3) * source_stride],
							 source[(i * 8 + 4) * source_stride],
							 source[(i * 8 + 5) * source_stride],
							 source[(i * 8 + 6) * source_stride],
							 source[(i * 8 + 7) * source_stride]};
  }
}

/**
 * ags_audio_buffer_util_fill_v8float_from_double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with float precision floating point data from float.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fill_v8float_from_double(AgsAudioBufferUtil *audio_buffer_util,
					       ags_v8float *destination, guint destination_stride,
					       gdouble *source, guint source_stride,
					       guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * destination_stride] = (ags_v8float) {source[i * 8 * source_stride],
							 source[(i * 8 + 1) * source_stride],
							 source[(i * 8 + 2) * source_stride],
							 source[(i * 8 + 3) * source_stride],
							 source[(i * 8 + 4) * source_stride],
							 source[(i * 8 + 5) * source_stride],
							 source[(i * 8 + 6) * source_stride],
							 source[(i * 8 + 7) * source_stride]};
  }
}

/**
 * ags_audio_buffer_util_fetch_v8float_as_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source vector
 * @source_stride: source vector's count of channels
 * @count: number of frames to copy
 *
 * Fetch signed 8 bit data from float vector.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fetch_v8float_as_s8(AgsAudioBufferUtil *audio_buffer_util,
					  gint8 *destination, guint destination_stride,
					  ags_v8float *source, guint source_stride,
					  guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * 8 * destination_stride] = (gint8) source[i * source_stride][0];
    destination[(i * 8 + 1) * destination_stride] = (gint8) source[i * source_stride][1];
    destination[(i * 8 + 2) * destination_stride] = (gint8) source[i * source_stride][2];
    destination[(i * 8 + 3) * destination_stride] = (gint8) source[i * source_stride][3];
    destination[(i * 8 + 4) * destination_stride] = (gint8) source[i * source_stride][4];
    destination[(i * 8 + 5) * destination_stride] = (gint8) source[i * source_stride][5];
    destination[(i * 8 + 6) * destination_stride] = (gint8) source[i * source_stride][6];
    destination[(i * 8 + 7) * destination_stride] = (gint8) source[i * source_stride][7];
  }
}

/**
 * ags_audio_buffer_util_fetch_v8float_as_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source vector
 * @source_stride: source vector's count of channels
 * @count: number of frames to copy
 *
 * Fetch signed 16 bit data from float vector.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fetch_v8float_as_s16(AgsAudioBufferUtil *audio_buffer_util,
					   gint16 *destination, guint destination_stride,
					   ags_v8float *source, guint source_stride,
					   guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * 8 * destination_stride] = (gint16) source[i * source_stride][0];
    destination[(i * 8 + 1) * destination_stride] = (gint16) source[i * source_stride][1];
    destination[(i * 8 + 2) * destination_stride] = (gint16) source[i * source_stride][2];
    destination[(i * 8 + 3) * destination_stride] = (gint16) source[i * source_stride][3];
    destination[(i * 8 + 4) * destination_stride] = (gint16) source[i * source_stride][4];
    destination[(i * 8 + 5) * destination_stride] = (gint16) source[i * source_stride][5];
    destination[(i * 8 + 6) * destination_stride] = (gint16) source[i * source_stride][6];
    destination[(i * 8 + 7) * destination_stride] = (gint16) source[i * source_stride][7];
  }
}

/**
 * ags_audio_buffer_util_fetch_v8float_as_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source vector
 * @source_stride: source vector's count of channels
 * @count: number of frames to copy
 *
 * Fetch signed 32 bit data from float vector.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fetch_v8float_as_s32(AgsAudioBufferUtil *audio_buffer_util,
					   gint32 *destination, guint destination_stride,
					   ags_v8float *source, guint source_stride,
					   guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * 8 * destination_stride] = (gint32) source[i * source_stride][0];
    destination[(i * 8 + 1) * destination_stride] = (gint32) source[i * source_stride][1];
    destination[(i * 8 + 2) * destination_stride] = (gint32) source[i * source_stride][2];
    destination[(i * 8 + 3) * destination_stride] = (gint32) source[i * source_stride][3];
    destination[(i * 8 + 4) * destination_stride] = (gint32) source[i * source_stride][4];
    destination[(i * 8 + 5) * destination_stride] = (gint32) source[i * source_stride][5];
    destination[(i * 8 + 6) * destination_stride] = (gint32) source[i * source_stride][6];
    destination[(i * 8 + 7) * destination_stride] = (gint32) source[i * source_stride][7];
  }
}

/**
 * ags_audio_buffer_util_fetch_v8float_as_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source vector
 * @source_stride: source vector's count of channels
 * @count: number of frames to copy
 *
 * Fetch signed 64 bit data from float vector.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fetch_v8float_as_s64(AgsAudioBufferUtil *audio_buffer_util,
					   gint64 *destination, guint destination_stride,
					   ags_v8float *source, guint source_stride,
					   guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * 8 * destination_stride] = (gint64) source[i * source_stride][0];
    destination[(i * 8 + 1) * destination_stride] = (gint64) source[i * source_stride][1];
    destination[(i * 8 + 2) * destination_stride] = (gint64) source[i * source_stride][2];
    destination[(i * 8 + 3) * destination_stride] = (gint64) source[i * source_stride][3];
    destination[(i * 8 + 4) * destination_stride] = (gint64) source[i * source_stride][4];
    destination[(i * 8 + 5) * destination_stride] = (gint64) source[i * source_stride][5];
    destination[(i * 8 + 6) * destination_stride] = (gint64) source[i * source_stride][6];
    destination[(i * 8 + 7) * destination_stride] = (gint64) source[i * source_stride][7];
  }
}

/**
 * ags_audio_buffer_util_fetch_v8float_as_double:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source vector
 * @source_stride: source vector's count of channels
 * @count: number of frames to copy
 *
 * Fetch signed 16 bit data from float vector.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fetch_v8float_as_float(AgsAudioBufferUtil *audio_buffer_util,
					     gdouble *destination, guint destination_stride,
					     ags_v8float *source, guint source_stride,
					     guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * 8 * destination_stride] = (gfloat) source[i * source_stride][0];
    destination[(i * 8 + 1) * destination_stride] = (gfloat) source[i * source_stride][1];
    destination[(i * 8 + 2) * destination_stride] = (gfloat) source[i * source_stride][2];
    destination[(i * 8 + 3) * destination_stride] = (gfloat) source[i * source_stride][3];
    destination[(i * 8 + 4) * destination_stride] = (gfloat) source[i * source_stride][4];
    destination[(i * 8 + 5) * destination_stride] = (gfloat) source[i * source_stride][5];
    destination[(i * 8 + 6) * destination_stride] = (gfloat) source[i * source_stride][6];
    destination[(i * 8 + 7) * destination_stride] = (gfloat) source[i * source_stride][7];
  }
}

/**
 * ags_audio_buffer_util_fill_v8double_from_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with double precision floating point data from signed 8 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fill_v8double_from_s8(AgsAudioBufferUtil *audio_buffer_util,
					    ags_v8double *destination, guint destination_stride,
					    gint8 *source, guint source_stride,
					    guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * destination_stride] = (ags_v8double) {source[i * 8 * source_stride],
							  source[(i * 8 + 1) * source_stride],
							  source[(i * 8 + 2) * source_stride],
							  source[(i * 8 + 3) * source_stride],
							  source[(i * 8 + 4) * source_stride],
							  source[(i * 8 + 5) * source_stride],
							  source[(i * 8 + 6) * source_stride],
							  source[(i * 8 + 7) * source_stride]};
  }
}

/**
 * ags_audio_buffer_util_fill_v8double_from_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with double precision floating point data from signed 16 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fill_v8double_from_s16(AgsAudioBufferUtil *audio_buffer_util,
					     ags_v8double *destination, guint destination_stride,
					     gint16 *source, guint source_stride,
					     guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * destination_stride] = (ags_v8double) {source[i * 8 * source_stride],
							  source[(i * 8 + 1) * source_stride],
							  source[(i * 8 + 2) * source_stride],
							  source[(i * 8 + 3) * source_stride],
							  source[(i * 8 + 4) * source_stride],
							  source[(i * 8 + 5) * source_stride],
							  source[(i * 8 + 6) * source_stride],
							  source[(i * 8 + 7) * source_stride]};
  }
}

/**
 * ags_audio_buffer_util_fill_v8double_from_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with double precision floating point data from signed 32 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fill_v8double_from_s32(AgsAudioBufferUtil *audio_buffer_util,
					     ags_v8double *destination, guint destination_stride,
					     gint32 *source, guint source_stride,
					     guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * destination_stride] = (ags_v8double) {source[i * 8 * source_stride],
							  source[(i * 8 + 1) * source_stride],
							  source[(i * 8 + 2) * source_stride],
							  source[(i * 8 + 3) * source_stride],
							  source[(i * 8 + 4) * source_stride],
							  source[(i * 8 + 5) * source_stride],
							  source[(i * 8 + 6) * source_stride],
							  source[(i * 8 + 7) * source_stride]};
  }
}

/**
 * ags_audio_buffer_util_fill_v8double_from_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with double precision floating point data from signed 64 bit.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fill_v8double_from_s64(AgsAudioBufferUtil *audio_buffer_util,
					     ags_v8double *destination, guint destination_stride,
					     gint64 *source, guint source_stride,
					     guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * destination_stride] = (ags_v8double) {source[i * 8 * source_stride],
							  source[(i * 8 + 1) * source_stride],
							  source[(i * 8 + 2) * source_stride],
							  source[(i * 8 + 3) * source_stride],
							  source[(i * 8 + 4) * source_stride],
							  source[(i * 8 + 5) * source_stride],
							  source[(i * 8 + 6) * source_stride],
							  source[(i * 8 + 7) * source_stride]};
  }
}

/**
 * ags_audio_buffer_util_fill_v8double_from_float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination vector
 * @destination_stride: destination vector's count of channels
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Fill vector with double precision floating point data from float.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fill_v8double_from_float(AgsAudioBufferUtil *audio_buffer_util,
					       ags_v8double *destination, guint destination_stride,
					       gfloat *source, guint source_stride,
					       guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * destination_stride] = (ags_v8double) {source[i * 8 * source_stride],
							  source[(i * 8 + 1) * source_stride],
							  source[(i * 8 + 2) * source_stride],
							  source[(i * 8 + 3) * source_stride],
							  source[(i * 8 + 4) * source_stride],
							  source[(i * 8 + 5) * source_stride],
							  source[(i * 8 + 6) * source_stride],
							  source[(i * 8 + 7) * source_stride]};
  }
}

/**
 * ags_audio_buffer_util_fetch_v8double_as_s8:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source vector
 * @source_stride: source vector's count of channels
 * @count: number of frames to copy
 *
 * Fetch signed 8 bit data from double vector.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fetch_v8double_as_s8(AgsAudioBufferUtil *audio_buffer_util,
					   gint8 *destination, guint destination_stride,
					   ags_v8double *source, guint source_stride,
					   guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * 8 * destination_stride] = (gint8) source[i * source_stride][0];
    destination[(i * 8 + 1) * destination_stride] = (gint8) source[i * source_stride][1];
    destination[(i * 8 + 2) * destination_stride] = (gint8) source[i * source_stride][2];
    destination[(i * 8 + 3) * destination_stride] = (gint8) source[i * source_stride][3];
    destination[(i * 8 + 4) * destination_stride] = (gint8) source[i * source_stride][4];
    destination[(i * 8 + 5) * destination_stride] = (gint8) source[i * source_stride][5];
    destination[(i * 8 + 6) * destination_stride] = (gint8) source[i * source_stride][6];
    destination[(i * 8 + 7) * destination_stride] = (gint8) source[i * source_stride][7];
  }
}

/**
 * ags_audio_buffer_util_fetch_v8double_as_s16:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source vector
 * @source_stride: source vector's count of channels
 * @count: number of frames to copy
 *
 * Fetch signed 16 bit data from double vector.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fetch_v8double_as_s16(AgsAudioBufferUtil *audio_buffer_util,
					    gint16 *destination, guint destination_stride,
					    ags_v8double *source, guint source_stride,
					    guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * 8 * destination_stride] = (gint16) source[i * source_stride][0];
    destination[(i * 8 + 1) * destination_stride] = (gint16) source[i * source_stride][1];
    destination[(i * 8 + 2) * destination_stride] = (gint16) source[i * source_stride][2];
    destination[(i * 8 + 3) * destination_stride] = (gint16) source[i * source_stride][3];
    destination[(i * 8 + 4) * destination_stride] = (gint16) source[i * source_stride][4];
    destination[(i * 8 + 5) * destination_stride] = (gint16) source[i * source_stride][5];
    destination[(i * 8 + 6) * destination_stride] = (gint16) source[i * source_stride][6];
    destination[(i * 8 + 7) * destination_stride] = (gint16) source[i * source_stride][7];
  }
}

/**
 * ags_audio_buffer_util_fetch_v8double_as_s32:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source vector
 * @source_stride: source vector's count of channels
 * @count: number of frames to copy
 *
 * Fetch signed 32 bit data from double vector.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fetch_v8double_as_s32(AgsAudioBufferUtil *audio_buffer_util,
					    gint32 *destination, guint destination_stride,
					    ags_v8double *source, guint source_stride,
					    guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * 8 * destination_stride] = (gint32) source[i * source_stride][0];
    destination[(i * 8 + 1) * destination_stride] = (gint32) source[i * source_stride][1];
    destination[(i * 8 + 2) * destination_stride] = (gint32) source[i * source_stride][2];
    destination[(i * 8 + 3) * destination_stride] = (gint32) source[i * source_stride][3];
    destination[(i * 8 + 4) * destination_stride] = (gint32) source[i * source_stride][4];
    destination[(i * 8 + 5) * destination_stride] = (gint32) source[i * source_stride][5];
    destination[(i * 8 + 6) * destination_stride] = (gint32) source[i * source_stride][6];
    destination[(i * 8 + 7) * destination_stride] = (gint32) source[i * source_stride][7];
  }
}

/**
 * ags_audio_buffer_util_fetch_v8double_as_s64:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source vector
 * @source_stride: source vector's count of channels
 * @count: number of frames to copy
 *
 * Fetch signed 64 bit data from double vector.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fetch_v8double_as_s64(AgsAudioBufferUtil *audio_buffer_util,
					    gint64 *destination, guint destination_stride,
					    ags_v8double *source, guint source_stride,
					    guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * 8 * destination_stride] = (gint64) source[i * source_stride][0];
    destination[(i * 8 + 1) * destination_stride] = (gint64) source[i * source_stride][1];
    destination[(i * 8 + 2) * destination_stride] = (gint64) source[i * source_stride][2];
    destination[(i * 8 + 3) * destination_stride] = (gint64) source[i * source_stride][3];
    destination[(i * 8 + 4) * destination_stride] = (gint64) source[i * source_stride][4];
    destination[(i * 8 + 5) * destination_stride] = (gint64) source[i * source_stride][5];
    destination[(i * 8 + 6) * destination_stride] = (gint64) source[i * source_stride][6];
    destination[(i * 8 + 7) * destination_stride] = (gint64) source[i * source_stride][7];
  }
}

/**
 * ags_audio_buffer_util_fetch_v8double_as_float:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @source: source vector
 * @source_stride: source vector's count of channels
 * @count: number of frames to copy
 *
 * Fetch signed 16 bit data from double vector.
 * 
 * Since: 7.0.0
 */
void
ags_audio_buffer_util_fetch_v8double_as_float(AgsAudioBufferUtil *audio_buffer_util,
					      gfloat *destination, guint destination_stride,
					      ags_v8double *source, guint source_stride,
					      guint count)
{
  guint i;

  for(i = 0; i < count / 8; i++){
    destination[i * 8 * destination_stride] = (gfloat) source[i * source_stride][0];
    destination[(i * 8 + 1) * destination_stride] = (gfloat) source[i * source_stride][1];
    destination[(i * 8 + 2) * destination_stride] = (gfloat) source[i * source_stride][2];
    destination[(i * 8 + 3) * destination_stride] = (gfloat) source[i * source_stride][3];
    destination[(i * 8 + 4) * destination_stride] = (gfloat) source[i * source_stride][4];
    destination[(i * 8 + 5) * destination_stride] = (gfloat) source[i * source_stride][5];
    destination[(i * 8 + 6) * destination_stride] = (gfloat) source[i * source_stride][6];
    destination[(i * 8 + 7) * destination_stride] = (gfloat) source[i * source_stride][7];
  }
}

/**
 * ags_audio_buffer_util_copy_buffer_to_buffer:
 * @audio_buffer_util: the #AgsAudioBufferUtil-struct
 * @destination: destination buffer
 * @destination_stride: destination buffer's count of channels
 * @doffset: start frame of destination
 * @source: source buffer
 * @source_stride: source buffer's count of channels
 * @soffset: start frame of source
 * @count: number of frames to copy
 * @mode: specified type conversion as described
 * 
 * Wrapper function to copy functions. Doing type conversion.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_buffer_to_buffer(AgsAudioBufferUtil *audio_buffer_util,
					    void *destination, guint destination_stride, guint doffset,
					    void *source, guint source_stride, guint soffset,
					    guint count, AgsAudioBufferUtilCopyMode mode)
{
  if(destination == NULL ||
     source == NULL){
    return;
  }

  switch(mode){
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S8:
    {
      ags_audio_buffer_util_copy_s8_to_s8(audio_buffer_util,
					  ((gint8 *) destination) + doffset, destination_stride,
					  ((gint8 *) source) + soffset, source_stride,
					  count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S16:
    {
      ags_audio_buffer_util_copy_s8_to_s16(audio_buffer_util,
					   ((gint16 *) destination) + doffset, destination_stride,
					   ((gint8 *) source) + soffset, source_stride,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S24:
    {
      ags_audio_buffer_util_copy_s8_to_s24(audio_buffer_util,
					   ((gint32 *) destination) + doffset, destination_stride,
					   ((gint8 *) source) + soffset, source_stride,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S32:
    {
      ags_audio_buffer_util_copy_s8_to_s32(audio_buffer_util,
					   ((gint32 *) destination) + doffset, destination_stride,
					   ((gint8 *) source) + soffset, source_stride,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S64:
    {
      ags_audio_buffer_util_copy_s8_to_s64(audio_buffer_util,
					   ((gint64 *) destination) + doffset, destination_stride,
					   ((gint8 *) source) + soffset, source_stride,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s8_to_float(audio_buffer_util,
					     ((gfloat *) destination) + doffset, destination_stride,
					     ((gint8 *) source) + soffset, source_stride,
					     count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s8_to_double(audio_buffer_util,
					      ((gdouble *) destination) + doffset, destination_stride,
					      ((gint8 *) source) + soffset, source_stride,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_COMPLEX:
    {
      ags_audio_buffer_util_copy_s8_to_complex(audio_buffer_util,
					       ((AgsComplex *) destination) + doffset, destination_stride,
					       ((gint8 *) source) + soffset, source_stride,
					       count);
    }
    break;

  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S8:
    {
      ags_audio_buffer_util_copy_s16_to_s8(audio_buffer_util,
					   ((gint8 *) destination) + doffset, destination_stride,
					   ((gint16 *) source) + soffset, source_stride,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16:
    {
      ags_audio_buffer_util_copy_s16_to_s16(audio_buffer_util,
					    ((gint16 *) destination) + doffset, destination_stride,
					    ((gint16 *) source) + soffset, source_stride,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S24:
    {
      ags_audio_buffer_util_copy_s16_to_s24(audio_buffer_util,
					    ((gint32 *) destination) + doffset, destination_stride,
					    ((gint16 *) source) + soffset, source_stride,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S32:
    {
      ags_audio_buffer_util_copy_s16_to_s32(audio_buffer_util,
					    ((gint32 *) destination) + doffset, destination_stride,
					    ((gint16 *) source) + soffset, source_stride,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S64:
    {
      ags_audio_buffer_util_copy_s16_to_s64(audio_buffer_util,
					    ((gint64 *) destination) + doffset, destination_stride,
					    ((gint16 *) source) + soffset, source_stride,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s16_to_float(audio_buffer_util,
					      ((gfloat *) destination) + doffset, destination_stride,
					      ((gint16 *) source) + soffset, source_stride,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s16_to_double(audio_buffer_util,
					       ((gdouble *) destination) + doffset, destination_stride,
					       ((gint16 *) source) + soffset, source_stride,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_COMPLEX:
    {
      ags_audio_buffer_util_copy_s16_to_complex(audio_buffer_util,
						((AgsComplex *) destination) + doffset, destination_stride,
						((gint16 *) source) + soffset, source_stride,
						count);
    }
    break;

  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S8:
    {
      ags_audio_buffer_util_copy_s24_to_s8(audio_buffer_util,
					   ((gint8 *) destination) + doffset, destination_stride,
					   ((gint32 *) source) + soffset, source_stride,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S16:
    {
      ags_audio_buffer_util_copy_s24_to_s16(audio_buffer_util,
					    ((gint16 *) destination) + doffset, destination_stride,
					    ((gint32 *) source) + soffset, source_stride,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S24:
    {
      ags_audio_buffer_util_copy_s24_to_s24(audio_buffer_util,
					    ((gint32 *) destination) + doffset, destination_stride,
					    ((gint32 *) source) + soffset, source_stride,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S32:
    {
      ags_audio_buffer_util_copy_s24_to_s32(audio_buffer_util,
					    ((gint32 *) destination) + doffset, destination_stride,
					    ((gint32 *) source) + soffset, source_stride,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S64:
    {
      ags_audio_buffer_util_copy_s24_to_s64(audio_buffer_util,
					    ((gint64 *) destination) + doffset, destination_stride,
					    ((gint32 *) source) + soffset, source_stride,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s24_to_float(audio_buffer_util,
					      ((gfloat *) destination) + doffset, destination_stride,
					      ((gint32 *) source) + soffset, source_stride,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s24_to_double(audio_buffer_util,
					       ((gdouble *) destination) + doffset, destination_stride,
					       ((gint32 *) source) + soffset, source_stride,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_COMPLEX:
    {
      ags_audio_buffer_util_copy_s24_to_complex(audio_buffer_util,
						((AgsComplex *) destination) + doffset, destination_stride,
						((gint32 *) source) + soffset, source_stride,
						count);
    }
    break;

  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S8:
    {
      ags_audio_buffer_util_copy_s32_to_s8(audio_buffer_util,
					   ((gint8 *) destination) + doffset, destination_stride,
					   ((gint32 *) source) + soffset, source_stride,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S16:
    {
      ags_audio_buffer_util_copy_s32_to_s16(audio_buffer_util,
					    ((gint16 *) destination) + doffset, destination_stride,
					    ((gint32 *) source) + soffset, source_stride,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S24:
    {
      ags_audio_buffer_util_copy_s32_to_s24(audio_buffer_util,
					    ((gint32 *) destination) + doffset, destination_stride,
					    ((gint32 *) source) + soffset, source_stride,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32:
    {
      ags_audio_buffer_util_copy_s32_to_s32(audio_buffer_util,
					    ((gint32 *) destination) + doffset, destination_stride,
					    ((gint32 *) source) + soffset, source_stride,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S64:
    {
      ags_audio_buffer_util_copy_s32_to_s64(audio_buffer_util,
					    ((gint64 *) destination) + doffset, destination_stride,
					    ((gint32 *) source) + soffset, source_stride,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s32_to_s8(audio_buffer_util,
					   ((gint8 *) destination) + doffset, destination_stride,
					   ((gint32 *) source) + soffset, source_stride,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s32_to_double(audio_buffer_util,
					       ((gdouble *) destination) + doffset, destination_stride,
					       ((gint32 *) source) + soffset, source_stride,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_COMPLEX:
    {
      ags_audio_buffer_util_copy_s32_to_complex(audio_buffer_util,
						((AgsComplex *) destination) + doffset, destination_stride,
						((gint32 *) source) + soffset, source_stride,
						count);
    }
    break;

  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S8:
    {
      ags_audio_buffer_util_copy_s64_to_s8(audio_buffer_util,
					   ((gint8 *) destination) + doffset, destination_stride,
					   ((gint64 *) source) + soffset, source_stride,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S16:
    {
      ags_audio_buffer_util_copy_s64_to_s16(audio_buffer_util,
					    ((gint16 *) destination) + doffset, destination_stride,
					    ((gint64 *) source) + soffset, source_stride,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S24:
    {
      ags_audio_buffer_util_copy_s64_to_s24(audio_buffer_util,
					    ((gint32 *) destination) + doffset, destination_stride,
					    ((gint64 *) source) + soffset, source_stride,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S32:
    {
      ags_audio_buffer_util_copy_s64_to_s32(audio_buffer_util,
					    ((gint32 *) destination) + doffset, destination_stride,
					    ((gint64 *) source) + soffset, source_stride,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S64:
    {
      ags_audio_buffer_util_copy_s64_to_s64(audio_buffer_util,
					    ((gint64 *) destination) + doffset, destination_stride,
					    ((gint64 *) source) + soffset, source_stride,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s64_to_float(audio_buffer_util,
					      ((gfloat *) destination) + doffset, destination_stride,
					      ((gint64 *) source) + soffset, source_stride,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s64_to_double(audio_buffer_util,
					       ((gdouble *) destination) + doffset, destination_stride,
					       ((gint64 *) source) + soffset, source_stride,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_COMPLEX:
    {
      ags_audio_buffer_util_copy_s64_to_complex(audio_buffer_util,
						((AgsComplex *) destination) + doffset, destination_stride,
						((gint64 *) source) + soffset, source_stride,
						count);
    }
    break;

  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S8:
    {
      ags_audio_buffer_util_copy_float_to_s8(audio_buffer_util,
					     ((gint8 *) destination) + doffset, destination_stride,
					     ((gfloat *) source) + soffset, source_stride,
					     count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S16:
    {
      ags_audio_buffer_util_copy_float_to_s16(audio_buffer_util,
					      ((gint16 *) destination) + doffset, destination_stride,
					      ((gfloat *) source) + soffset, source_stride,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S24:
    {
      ags_audio_buffer_util_copy_float_to_s24(audio_buffer_util,
					      ((gint32 *) destination) + doffset, destination_stride,
					      ((gfloat *) source) + soffset, source_stride,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S32:
    {
      ags_audio_buffer_util_copy_float_to_s32(audio_buffer_util,
					      ((gint32 *) destination) + doffset, destination_stride,
					      ((gfloat *) source) + soffset, source_stride,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S64:
    {
      ags_audio_buffer_util_copy_float_to_s64(audio_buffer_util,
					      ((gint64 *) destination) + doffset, destination_stride,
					      ((gfloat *) source) + soffset, source_stride,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_float_to_float(audio_buffer_util,
						((gfloat *) destination) + doffset, destination_stride,
						((gfloat *) source) + soffset, source_stride,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_float_to_double(audio_buffer_util,
						 ((gdouble *) destination) + doffset, destination_stride,
						 ((gfloat *) source) + soffset, source_stride,
						 count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_COMPLEX:
    {
      ags_audio_buffer_util_copy_float_to_complex(audio_buffer_util,
						  ((AgsComplex *) destination) + doffset, destination_stride,
						  ((gfloat *) source) + soffset, source_stride,
						  count);
    }
    break;

  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S8:
    {
      ags_audio_buffer_util_copy_double_to_s8(audio_buffer_util,
					      ((gint8 *) destination) + doffset, destination_stride,
					      ((gdouble *) source) + soffset, source_stride,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S16:
    {
      ags_audio_buffer_util_copy_double_to_s16(audio_buffer_util,
					       ((gint16 *) destination) + doffset, destination_stride,
					       ((gdouble *) source) + soffset, source_stride,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S24:
    {
      ags_audio_buffer_util_copy_double_to_s24(audio_buffer_util,
					       ((gint32 *) destination) + doffset, destination_stride,
					       ((gdouble *) source) + soffset, source_stride,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S32:
    {
      ags_audio_buffer_util_copy_double_to_s32(audio_buffer_util,
					       ((gint32 *) destination) + doffset, destination_stride,
					       ((gdouble *) source) + soffset, source_stride,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S64:
    {
      ags_audio_buffer_util_copy_double_to_s64(audio_buffer_util,
					       ((gint64 *) destination) + doffset, destination_stride,
					       ((gdouble *) source) + soffset, source_stride,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_double_to_float(audio_buffer_util,
						 ((gfloat *) destination) + doffset, destination_stride,
						 ((gdouble *) source) + soffset, source_stride,
						 count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_double_to_double(audio_buffer_util,
						  ((gdouble *) destination) + doffset, destination_stride,
						  ((gdouble *) source) + soffset, source_stride,
						  count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_COMPLEX:
    {
      ags_audio_buffer_util_copy_double_to_complex(audio_buffer_util,
						   ((AgsComplex *) destination) + doffset, destination_stride,
						   ((gdouble *) source) + soffset, source_stride,
						   count);
    }
    break;

  case AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S8:
    {
      ags_audio_buffer_util_copy_complex_to_s8(audio_buffer_util,
					       ((gint8 *) destination) + doffset, destination_stride,
					       ((AgsComplex *) source) + soffset, source_stride,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S16:
    {
      ags_audio_buffer_util_copy_complex_to_s16(audio_buffer_util,
						((gint16 *) destination) + doffset, destination_stride,
						((AgsComplex *) source) + soffset, source_stride,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S24:
    {
      ags_audio_buffer_util_copy_complex_to_s24(audio_buffer_util,
						((gint32 *) destination) + doffset, destination_stride,
						((AgsComplex *) source) + soffset, source_stride,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S32:
    {
      ags_audio_buffer_util_copy_complex_to_s32(audio_buffer_util,
						((gint32 *) destination) + doffset, destination_stride,
						((AgsComplex *) source) + soffset, source_stride,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S64:
    {
      ags_audio_buffer_util_copy_complex_to_s64(audio_buffer_util,
						((gint64 *) destination) + doffset, destination_stride,
						((AgsComplex *) source) + soffset, source_stride,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_complex_to_float(audio_buffer_util,
						  ((gfloat *) destination) + doffset, destination_stride,
						  ((AgsComplex *) source) + soffset, source_stride,
						  count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_complex_to_double(audio_buffer_util,
						   ((gdouble *) destination) + doffset, destination_stride,
						   ((AgsComplex *) source) + soffset, source_stride,
						   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_COMPLEX:
    {
      ags_audio_buffer_util_copy_complex_to_complex(audio_buffer_util,
						    ((AgsComplex *) destination) + doffset, destination_stride,
						    ((AgsComplex *) source) + soffset, source_stride,
						    count);
    }
    break;
  
#ifdef __APPLE__
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_s8_to_float32(audio_buffer_util,
					       ((Float32 *) destination) + doffset, destination_stride,
					       ((gint8 *) source) + soffset, source_stride,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_s16_to_float32(audio_buffer_util,
						((Float32 *) destination) + doffset, destination_stride,
						((gint16 *) source) + soffset, source_stride,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_s24_to_float32(audio_buffer_util,
						((Float32 *) destination) + doffset, destination_stride,
						((gint32 *) source) + soffset, source_stride,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_s32_to_float32(audio_buffer_util,
						((Float32 *) destination) + doffset, destination_stride,
						((gint32 *) source) + soffset, source_stride,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_s64_to_float32(audio_buffer_util,
						((Float32 *) destination) + doffset, destination_stride,
						((gint64 *) source) + soffset, source_stride,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_float_to_float32(audio_buffer_util,
						  ((Float32 *) destination) + doffset, destination_stride,
						  ((gfloat *) source) + soffset, source_stride,
						  count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_double_to_float32(audio_buffer_util,
						   ((Float32 *) destination) + doffset, destination_stride,
						   ((gdouble *) source) + soffset, source_stride,
						   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_float32_to_float32(audio_buffer_util,
						    ((Float32 *) destination) + doffset, destination_stride,
						    ((Float32 *) source) + soffset, source_stride,
						    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_complex_to_float32(audio_buffer_util,
						    ((Float32 *) destination) + doffset, destination_stride,
						    ((AgsComplex *) source) + soffset, source_stride,
						    count);
    }
    break;
    
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S8:
    {
      ags_audio_buffer_util_copy_float32_to_s8(audio_buffer_util,
					       ((gint8 *) destination) + doffset, destination_stride,
					       ((Float32 *) source) + soffset, source_stride,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S16:
    {
      ags_audio_buffer_util_copy_float32_to_s16(audio_buffer_util,
						((gint16 *) destination) + doffset, destination_stride,
						((Float32 *) source) + soffset, source_stride,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S24:
    {
      ags_audio_buffer_util_copy_float32_to_s24(audio_buffer_util,
						((gint32 *) destination) + doffset, destination_stride,
						((Float32 *) source) + soffset, source_stride,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S32:
    {
      ags_audio_buffer_util_copy_float32_to_s32(audio_buffer_util,
						((gint32 *) destination) + doffset, destination_stride,
						((Float32 *) source) + soffset, source_stride,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S64:
    {
      ags_audio_buffer_util_copy_float32_to_s64(audio_buffer_util,
						((gint64 *) destination) + doffset, destination_stride,
						((Float32 *) source) + soffset, source_stride,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_float32_to_float(audio_buffer_util,
						  ((gfloat *) destination) + doffset, destination_stride,
						  ((Float32 *) source) + soffset, source_stride,
						  count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_COMPLEX:
    {
      ags_audio_buffer_util_copy_float32_to_complex(audio_buffer_util,
						    ((AgsComplex *) destination) + doffset, destination_stride,
						    ((Float32 *) source) + soffset, source_stride,
						    count);
    }
    break;
#endif    
  default:
    {
      g_warning("ags_audio_buffer_util.c - unknown copy mode");
    }
  }
}
