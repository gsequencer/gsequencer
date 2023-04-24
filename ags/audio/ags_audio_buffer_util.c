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
 */

#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/libags.h>

#include <samplerate.h>

#if defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#include <Accelerate/Accelerate.h>
#endif

gpointer ags_audio_buffer_util_copy(gpointer ptr);
void ags_audio_buffer_util_free(gpointer ptr);

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <complex.h>

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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audio_buffer_util = 0;

    ags_type_audio_buffer_util =
      g_boxed_type_register_static("AgsAudioBufferUtil",
				   (GBoxedCopyFunc) ags_audio_buffer_util_copy,
				   (GBoxedFreeFunc) ags_audio_buffer_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audio_buffer_util);
  }

  return g_define_type_id__volatile;
}

gpointer
ags_audio_buffer_util_copy(gpointer ptr)
{
  gpointer retval;

  retval = g_memdup(ptr, sizeof(AgsAudioBufferUtil));
 
  return(retval);
}

void
ags_audio_buffer_util_free(gpointer ptr)
{
  g_free(ptr);
}

/**
 * ags_audio_buffer_util_format_from_soundcard:
 * @soundcard_format: the soundcard bit mode
 *
 * Translate soundcard format to audio buffer util format.
 *
 * Returns: the converted format
 *
 * Since: 3.0.0
 */
AgsAudioBufferUtilFormat
ags_audio_buffer_util_format_from_soundcard(AgsSoundcardFormat soundcard_format)
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
 * ags_audio_buffer_util_get_copy_mode:
 * @destination_format: the destination buffer format
 * @source_format: the source buffer format
 *
 * Get copy mode for given destination and source format.
 *
 * Returns: the desired copy mode
 *
 * Since: 3.0.0
 */
guint
ags_audio_buffer_util_get_copy_mode(AgsAudioBufferUtilFormat destination_format,
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
 * @buffer: the buffer to clear
 * @channels: number of channels
 * @count: count frames
 * 
 * Clears a buffer.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_clear_float(gfloat *buffer, guint channels,
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
  
    for(; i < limit; i += 8){
      buffer[0] = 0.0;
      buffer[channels] = 0.0;
      buffer[2 * channels] = 0.0;
      buffer[3 * channels] = 0.0;
      buffer[4 * channels] = 0.0;
      buffer[5 * channels] = 0.0;
      buffer[6 * channels] = 0.0;
      buffer[7 * channels] = 0.0;

      buffer += (8 * channels);
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
 * @buffer: the buffer to clear
 * @channels: number of channels
 * @count: count frames
 * 
 * Clears a buffer.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_clear_float32(Float32 *buffer, guint channels,
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
  
    for(; i < limit; i += 8){
      buffer[0] = 0.0;
      buffer[channels] = 0.0;
      buffer[2 * channels] = (Float32) 0.0;
      buffer[3 * channels] = (Float32) 0.0;
      buffer[4 * channels] = (Float32) 0.0;
      buffer[5 * channels] = (Float32) 0.0;
      buffer[6 * channels] = (Float32) 0.0;
      buffer[7 * channels] = (Float32) 0.0;

      buffer += (8 * channels);
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
 * @buffer: the buffer to clear
 * @channels: number of channels
 * @count: count frames
 * 
 * Clears a buffer.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_clear_double(gdouble *buffer, guint channels,
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
  
    for(; i < limit; i += 8){
      buffer[0] = 0.0;
      buffer[channels] = 0.0;
      buffer[2 * channels] = 0.0;
      buffer[3 * channels] = 0.0;
      buffer[4 * channels] = 0.0;
      buffer[5 * channels] = 0.0;
      buffer[6 * channels] = 0.0;
      buffer[7 * channels] = 0.0;

      buffer += (8 * channels);
    }
  }

  for(; i < count; i++){
    *buffer = 0.0;

    buffer += channels;
  }
}

/**
 * ags_audio_buffer_util_clear_complex:
 * @buffer: the buffer to clear
 * @channels: number of channels
 * @count: count frames
 * 
 * Clears a buffer.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_clear_complex(AgsComplex *buffer, guint channels,
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
  
    for(; i < limit; i += 8){
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
ags_audio_buffer_util_clear_buffer(void *buffer, guint channels,
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
      ags_audio_buffer_util_clear_float(buffer, channels,
					count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
    {
      ags_audio_buffer_util_clear_double(buffer, channels,
					 count);
    }
    break;
#ifdef __APPLE__
  case AGS_AUDIO_BUFFER_UTIL_FLOAT32:
    {
      ags_audio_buffer_util_clear_float32(buffer, channels,
					  count);
    }
    break;
#endif
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
    {
      ags_audio_buffer_util_clear_complex(buffer, channels,
					  count);
    }
    break;
  default:
    g_critical("unsupported audio buffer format");
  }
}

/**
 * ags_audio_buffer_util_pong_s8:
 * @destination: the destination
 * @dchannels: the destination channels
 * @source: the source
 * @schannels: the source channels
 * @count: the count of frames
 * 
 * Pingpong copy @source to @destination.
 * 
 * Since: 3.4.0
 */
void
ags_audio_buffer_util_pong_s8(gint8 *destination, guint dchannels,
			      gint8 *source, guint schannels,
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
				  destination[(current_dchannel = dchannels)],
				  destination[(current_dchannel += dchannels)],
				  destination[(current_dchannel += dchannels)],
				  destination[(current_dchannel += dchannels)],
				  destination[(current_dchannel += dchannels)],
				  destination[(current_dchannel += dchannels)],
				  destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8s8) {source[0],
			     source[(current_schannel -= schannels)],
			     source[(current_schannel -= schannels)],
			     source[(current_schannel -= schannels)],
			     source[(current_schannel -= schannels)],
			     source[(current_schannel -= schannels)],
			     source[(current_schannel -= schannels)],
			     source[(current_schannel -= schannels)]};

      /* no scale, just copy */
      v_destination += v_source;
      
      current_dchannel = 0;

      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
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
			     (int) destination[(current_dchannel = dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint8) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint8) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
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
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel = dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
    }
  }
#endif

  for(; i < count; i++){
    destination[0] = 0xff & ((gint16) (destination[0] + source[0]));

    destination += dchannels;
    source -= schannels;
  }
}

/**
 * ags_audio_buffer_util_pong_s16:
 * @destination: the destination
 * @dchannels: the destination channels
 * @source: the source
 * @schannels: the source channels
 * @count: the count of frames
 * 
 * Pingpong copy @source to @destination.
 * 
 * Since: 3.4.0
 */
void
ags_audio_buffer_util_pong_s16(gint16 *destination, guint dchannels,
			       gint16 *source, guint schannels,
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
				   destination[(current_dchannel = dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)]};
      v_source = (ags_v8s16) {source[0],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)]};

      /* no scale, just copy */
      v_destination += v_source;

      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
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
			     (int) destination[(current_dchannel = dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint16) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint16) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
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
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel = dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
    }
  }
#endif

  for(; i < count; i++){
    destination[0] = (gint16) 0xffff & ((gint32) (destination[0] + source[0]));

    destination += dchannels;
    source -= schannels;
  }
}

/**
 * ags_audio_buffer_util_pong_s24:
 * @destination: the destination
 * @dchannels: the destination channels
 * @source: the source
 * @schannels: the source channels
 * @count: the count of frames
 * 
 * Pingpong copy @source to @destination.
 * 
 * Since: 3.4.0
 */
void
ags_audio_buffer_util_pong_s24(gint32 *destination, guint dchannels,
			       gint32 *source, guint schannels,
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
				   destination[(current_dchannel = dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)]};

      v_source = (ags_v8s32) {source[0],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)]};
      
      /* no scale, just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
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
			     (int) destination[(current_dchannel = dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
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
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel = dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint32) (destination[0] + source[0]));

    destination += dchannels;
    source -= schannels;
  }
}

/**
 * ags_audio_buffer_util_pong_s32:
 * @destination: the destination
 * @dchannels: the destination channels
 * @source: the source
 * @schannels: the source channels
 * @count: the count of frames
 * 
 * Pingpong copy @source to @destination.
 * 
 * Since: 3.4.0
 */
void
ags_audio_buffer_util_pong_s32(gint32 *destination, guint dchannels,
			       gint32 *source, guint schannels,
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
				   destination[(current_dchannel = dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)]};

      v_source = (ags_v8s32) {source[0],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)]};

      /* no scale, just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
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
			     (int) destination[(current_dchannel = dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = 0xffffffff & ((gint64) (destination[0] + source[0]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel = dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint64) (destination[0] + source[0]));

    destination += dchannels;
    source -= schannels;
  }
}

/**
 * ags_audio_buffer_util_pong_s64:
 * @destination: the destination
 * @dchannels: the destination channels
 * @source: the source
 * @schannels: the source channels
 * @count: the count of frames
 * 
 * Pingpong copy @source to @destination.
 * 
 * Since: 3.4.0
 */
void
ags_audio_buffer_util_pong_s64(gint64 *destination, guint dchannels,
			       gint64 *source, guint schannels,
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
				   destination[(current_dchannel = dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8s64) {source[0],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)],
			      source[(current_schannel -= schannels)]};

      /* no scale, just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
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
			     (int) destination[(current_dchannel = dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)],
			(int) source[(current_schannel -= schannels)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint64) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint64) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
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
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel = dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel -= schannels)]));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + source[0]));

    destination += dchannels;
    source -= schannels;
  }
}

/**
 * ags_audio_buffer_util_pong_float:
 * @destination: the destination
 * @dchannels: the destination channels
 * @source: the source
 * @schannels: the source channels
 * @count: the count of frames
 * 
 * Pingpong copy @source to @destination.
 * 
 * Since: 3.4.0
 */
void
ags_audio_buffer_util_pong_float(gfloat *destination, guint dchannels,
				 gfloat *source, guint schannels,
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
				     destination[(current_dchannel = dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {source[0],
				source[(current_schannel -= schannels)],
				source[(current_schannel -= schannels)],
				source[(current_schannel -= schannels)],
				source[(current_schannel -= schannels)],
				source[(current_schannel -= schannels)],
				source[(current_schannel -= schannels)],
				source[(current_schannel -= schannels)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel -= schannels)],
			   (double) source[(current_schannel -= schannels)],
			   (double) source[(current_schannel -= schannels)],
			   (double) source[(current_schannel -= schannels)],
			   (double) source[(current_schannel -= schannels)],
			   (double) source[(current_schannel -= schannels)],
			   (double) source[(current_schannel -= schannels)]};

      /* no scale, just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gfloat) v_result[0];
      destination[(current_dchannel = dchannels)] = (gfloat) v_result[1];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[2];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[3];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[4];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[5];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[6];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (source[(current_schannel -= schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel -= schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel -= schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel -= schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel -= schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel -= schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel -= schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0])));

    destination += dchannels;
    source -= schannels;
  }
}

/**
 * ags_audio_buffer_util_pong_double:
 * @destination: the destination
 * @dchannels: the destination channels
 * @source: the source
 * @schannels: the source channels
 * @count: the count of frames
 * 
 * Pingpong copy @source to @destination.
 * 
 * Since: 3.4.0
 */
void
ags_audio_buffer_util_pong_double(gdouble *destination, guint dchannels,
				  gdouble *source, guint schannels,
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
				      destination[(current_dchannel = dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {source[0],
				 source[(current_schannel -= schannels)],
				 source[(current_schannel -= schannels)],
				 source[(current_schannel -= schannels)],
				 source[(current_schannel -= schannels)],
				 source[(current_schannel -= schannels)],
				 source[(current_schannel -= schannels)],
				 source[(current_schannel -= schannels)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel -= schannels)],
			   (double) source[(current_schannel -= schannels)],
			   (double) source[(current_schannel -= schannels)],
			   (double) source[(current_schannel -= schannels)],
			   (double) source[(current_schannel -= schannels)],
			   (double) source[(current_schannel -= schannels)],
			   (double) source[(current_schannel -= schannels)]};

      /* no scale, just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gdouble) v_result[0];
      destination[(current_dchannel = dchannels)] = (gdouble) v_result[1];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[2];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[3];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[4];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[5];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[6];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
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
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (source[(current_schannel -= schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel -= schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel -= schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel -= schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel -= schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel -= schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel -= schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel - schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = ((gdouble) ((destination[0]) + (gdouble) (source[0])));

    destination += dchannels;
    source -= schannels;
  }
}

/**
 * ags_audio_buffer_util_pong_complex:
 * @destination: the destination
 * @dchannels: the destination channels
 * @source: the source
 * @schannels: the source channels
 * @count: the count of frames
 * 
 * Pingpong copy @source to @destination.
 * 
 * Since: 3.4.0
 */
void
ags_audio_buffer_util_pong_complex(AgsComplex *destination, guint dchannels,
				   AgsComplex *source, guint schannels,
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
    
    destination += dchannels;
    source -= schannels;
  }
}

/**
 * ags_audio_buffer_util_pong:
 * @destination: the destination
 * @dchannels: the destination channels
 * @source: the source
 * @schannels: the source channels
 * @count: the count of frames
 * @format: the format
 * 
 * Pingpong copy @source to @destination.
 * 
 * Since: 3.4.0
 */
void
ags_audio_buffer_util_pong(void *destination, guint dchannels,
			   void *source, guint schannels,
			   guint count, guint format)
{
  if(destination == NULL ||
     source == NULL){
    return;
  }

  switch(format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
    {
      ags_audio_buffer_util_pong_s8((gint8 *) destination, dchannels,
				    (gint8 *) source, schannels,
				    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
    {
      ags_audio_buffer_util_pong_s16((gint16 *) destination, dchannels,
				     (gint16 *) source, schannels,
				     count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
    {
      ags_audio_buffer_util_pong_s24((gint32 *) destination, dchannels,
				     (gint32 *) source, schannels,
				     count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
    {
      ags_audio_buffer_util_pong_s32((gint32 *) destination, dchannels,
				     (gint32 *) source, schannels,
				     count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
    {
      ags_audio_buffer_util_pong_s64((gint64 *) destination, dchannels,
				     (gint64 *) source, schannels,
				     count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
    {
      ags_audio_buffer_util_pong_float((gfloat *) destination, dchannels,
				       (gfloat *) source, schannels,
				       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
    {
      ags_audio_buffer_util_pong_double((gdouble *) destination, dchannels,
					(gdouble *) source, schannels,
					count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
    {
      ags_audio_buffer_util_pong_complex((AgsComplex *) destination, dchannels,
					 (AgsComplex *) source, schannels,
					 count);
    }
    break;
  default:
    g_warning("ags_audio_buffer_util_pong() - unknown format");
  }
}

/**
 * ags_audio_buffer_util_copy_s8_to_s8:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s8_to_s8(gint8 *destination, guint dchannels,
				    gint8 *source, guint schannels,
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
      ags_v8s8 v_destination;
      ags_v8s8 v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8s8) {destination[0],
				  destination[(current_dchannel = dchannels)],
				  destination[(current_dchannel += dchannels)],
				  destination[(current_dchannel += dchannels)],
				  destination[(current_dchannel += dchannels)],
				  destination[(current_dchannel += dchannels)],
				  destination[(current_dchannel += dchannels)],
				  destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8s8) {source[0],
			     source[(current_schannel = schannels)],
			     source[(current_schannel += schannels)],
			     source[(current_schannel += schannels)],
			     source[(current_schannel += schannels)],
			     source[(current_schannel += schannels)],
			     source[(current_schannel += schannels)],
			     source[(current_schannel += schannels)]};

      /* no scale, just copy */
      v_destination += v_source;
      
      current_dchannel = 0;

      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
			     (int) destination[(current_dchannel = dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel = schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint8) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint8) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel = dchannels)] + source[(current_schannel = schannels)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xff & ((gint16) (destination[0] + source[0]));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s8_to_s16:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s8_to_s16(gint16 *destination, guint dchannels,
				     gint8 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;

      current_dchannel = 0;
      
      destination[0] = (gint16) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint16) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint16) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint16) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffff & ((gint32) (destination[0] + (gint32) (scale * source[0])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel = dchannels)] + (gint32) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffff & ((gint32) (destination[0] + (gint32) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s8_to_s24:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s8_to_s24(gint32 *destination, guint dchannels,
				     gint8 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;

      current_dchannel = 0;
      
      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
    
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & ((gint32) (destination[0] + (gint32) (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel = dchannels)] + (gint32) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint32) (destination[0] + (gint32) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s8_to_s32:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s8_to_s32(gint32 *destination, guint dchannels,
				     gint8 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;

      current_dchannel = 0;
      
      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & ((gint64) (destination[0] + (gint32) (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel = dchannels)] + (gint32) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint64) (destination[0] + (gint32) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s8_to_s64:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s8_to_s64(gint64 *destination, guint dchannels,
				     gint8 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint64) v_destination[0];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint64) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint64) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
    
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + (gint64) (scale * source[0])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel = dchannels)] + (gint64) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint64) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint64) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint64) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint64) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint64) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint64) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + (gint64) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s8_to_float:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s8_to_float(gfloat *destination, guint dchannels,
				       gint8 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gfloat) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gfloat) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gfloat) v_result[0];
      destination[(current_dchannel = dchannels)] = (gfloat) v_result[1];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[2];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[3];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[4];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[5];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[6];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (source[(current_schannel = schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s8_to_double:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s8_to_double(gdouble *destination, guint dchannels,
					gint8 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* normalize and copy */
      v_source /= normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gdouble) v_result[0];
      destination[(current_dchannel = dchannels)] = (gdouble) v_result[1];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[2];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[3];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[4];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[5];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[6];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (source[(current_schannel = schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s16_to_s8:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s16_to_s8(gint8 *destination, guint dchannels,
				     gint16 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;

      current_dchannel = 0;
      
      destination[0] = (gint8) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint8) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint8) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint8) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
    
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xff & ((gint16) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel = dchannels)] + (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xff & ((gint16) (destination[0] + (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s16_to_s16:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s16_to_s16(gint16 *destination, guint dchannels,
				      gint16 *source, guint schannels,
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
      ags_v8s16 v_destination;
      ags_v8s16 v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8s16) {destination[0],
				   destination[(current_dchannel = dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)]};
      v_source = (ags_v8s16) {source[0],
			      source[(current_schannel = schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)]};

      /* no scale, just copy */
      v_destination += v_source;

      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
			     (int) destination[(current_dchannel = dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel = schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint16) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint16) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel = dchannels)] + source[(current_schannel = schannels)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = (gint16) 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif

  for(; i < count; i++){
    destination[0] = (gint16) 0xffff & ((gint32) (destination[0] + source[0]));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s16_to_s24:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s16_to_s24(gint32 *destination, guint dchannels,
				      gint16 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;

      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & ((gint32) (destination[0] + (gint32) (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel = dchannels)] + (gint32) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint32) (destination[0] + (gint32) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s16_to_s32:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s16_to_s32(gint32 *destination, guint dchannels,
				      gint16 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;

      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & ((gint64) (destination[0] + (gint32) (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel = dchannels)] + (gint32) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint64) (destination[0] + (gint32) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s16_to_s64:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s16_to_s64(gint64 *destination, guint dchannels,
				      gint16 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint64) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint64) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint64) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint64) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + (gint64) (scale * source[0])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel = dchannels)] + (gint64) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint64) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint64) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint64) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint64) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint64) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (gint64) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + (gint64) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s16_to_float:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s16_to_float(gfloat *destination, guint dchannels,
					gint16 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gfloat) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gfloat) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gfloat) v_result[0];
      destination[(current_dchannel = dchannels)] = (gfloat) v_result[1];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[2];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[3];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[4];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[5];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[6];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (source[(current_schannel = schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s16_to_double:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s16_to_double(gdouble *destination, guint dchannels,
					 gint16 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* normalize and copy */
      v_source /= normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gdouble) v_result[0];
      destination[(current_dchannel = dchannels)] = (gdouble) v_result[1];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[2];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[3];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[4];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[5];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[6];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (source[(current_schannel = schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s24_to_s8:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s24_to_s8(gint8 *destination, guint dchannels,
				     gint32 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint8) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint8) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint8) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint8) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
      
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xff & ((gint16) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel = dchannels)] + (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xff & ((gint16) (destination[0] + (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s24_to_s16:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s24_to_s16(gint16 *destination, guint dchannels,
				      gint32 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint16) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint16) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint16) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint16) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffff & ((gint32) (destination[0] + (gint32) (scale * source[0])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel = dchannels)] + (gint32) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (gint32) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffff & ((gint32) (destination[0] + (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s24_to_s24:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s24_to_s24(gint32 *destination, guint dchannels,
				      gint32 *source, guint schannels,
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
      ags_v8s32 v_destination;
      ags_v8s32 v_source;

      current_dchannel = 0;
      current_schannel = 0;
      
      v_destination = (ags_v8s32) {destination[0],
				   destination[(current_dchannel = dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)]};

      v_source = (ags_v8s32) {source[0],
			      source[(current_schannel = schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)]};
      
      /* no scale, just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
			     (int) destination[(current_dchannel = dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel = schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel = dchannels)] + source[(current_schannel = schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint32) (destination[0] + source[0]));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s24_to_s32:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s24_to_s32(gint32 *destination, guint dchannels,
				      gint32 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;

      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
    
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & ((gint64) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel = dchannels)] + (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint64) (destination[0] + (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s24_to_s64:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s24_to_s64(gint64 *destination, guint dchannels,
				      gint32 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint64) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint64) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint64) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint64) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
    
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel = dchannels)] + (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
 
      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif

  for(; i < count; i++){
    destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s24_to_float:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s24_to_float(gfloat *destination, guint dchannels,
					gint32 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gfloat) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gfloat) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gfloat) v_result[0];
      destination[(current_dchannel = dchannels)] = (gfloat) v_result[1];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[2];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[3];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[4];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[5];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[6];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (source[(current_schannel = schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s24_to_double:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s24_to_double(gdouble *destination, guint dchannels,
					 gint32 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* normalize and copy */
      v_source /= normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gdouble) v_result[0];
      destination[(current_dchannel = dchannels)] = (gdouble) v_result[1];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[2];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[3];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[4];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[5];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[6];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (source[(current_schannel = schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s32_to_s8:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s32_to_s8(gint8 *destination, guint dchannels,
				     gint32 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;

      current_dchannel = 0;
      
      destination[0] = (gint8) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint8) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint8) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint8) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xff & ((gint16) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel = dchannels)] + (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xff & ((gint16) (destination[0] + (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s32_to_s16:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s32_to_s16(gint16 *destination, guint dchannels,
				      gint32 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint16) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint16) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint16) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint16) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffff & ((gint32) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel = dchannels)] + (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gint16) ((gint32) (destination[0] + (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s32_to_s24:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s32_to_s24(gint32 *destination, guint dchannels,
				      gint32 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;

      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & ((gint32) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel = dchannels)] + (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint32) (destination[0] + (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s32_to_s32:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s32_to_s32(gint32 *destination, guint dchannels,
				      gint32 *source, guint schannels,
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
      ags_v8s32 v_destination;
      ags_v8s32 v_source;

      current_dchannel = 0;
      current_schannel = 0;
      
      v_destination = (ags_v8s32) {destination[0],
				   destination[(current_dchannel = dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)]};

      v_source = (ags_v8s32) {source[0],
			      source[(current_schannel = schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)]};

      /* no scale, just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
			     (int) destination[(current_dchannel = dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel = schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = 0xffffffff & ((gint64) (destination[0] + source[0]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel = dchannels)] + source[(current_schannel = schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint64) (destination[0] + source[0]));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s32_to_s64:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s32_to_s64(gint64 *destination, guint dchannels,
				      gint32 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint64) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint64) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint64) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint64) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
      
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel = dchannels)] + (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s32_to_float:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s32_to_float(gfloat *destination, guint dchannels,
					gint32 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gfloat) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gfloat) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gfloat) v_result[0];
      destination[(current_dchannel = dchannels)] = (gfloat) v_result[1];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[2];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[3];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[4];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[5];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[6];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (source[(current_schannel = schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s32_to_double:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s32_to_double(gdouble *destination, guint dchannels,
					 gint32 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* normalize and copy */
      v_source /= normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gdouble) v_result[0];
      destination[(current_dchannel = dchannels)] = (gdouble) v_result[1];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[2];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[3];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[4];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[5];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[6];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (source[(current_schannel = schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_s8:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s64_to_s8(gint8 *destination, guint dchannels,
				     gint64 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;

      current_dchannel = 0;
      
      destination[0] = (gint8) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint8) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint8) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint8) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
      
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xff & ((gint16) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel = dchannels)] + (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & ((gint16) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xff & ((gint16) (destination[0] + (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_s16:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s64_to_s16(gint16 *destination, guint dchannels,
				      gint64 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint16) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint16) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint16) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint16) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffff & ((gint32) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel = dchannels)] + (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffff & ((gint32) (destination[0] + (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_s24:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s64_to_s24(gint32 *destination, guint dchannels,
				      gint64 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;

      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & ((gint32) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel = dchannels)] + (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint32) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint32) (destination[0] + (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_s32:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s64_to_s32(gint32 *destination, guint dchannels,
				      gint64 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;

      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = 0xffffffff & ((gint64) (destination[0] + (scale * source[0])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel = dchannels)] + (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & ((gint64) (destination[0] + (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_s64:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s64_to_s64(gint64 *destination, guint dchannels,
				      gint64 *source, guint schannels,
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
      ags_v8s64 v_destination;
      ags_v8s64 v_source;

      current_dchannel = 0;
      current_schannel = 0;      

      v_destination = (ags_v8s64) {destination[0],
				   destination[(current_dchannel = dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)],
				   destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8s64) {source[0],
			      source[(current_schannel = schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)],
			      source[(current_schannel += schannels)]};

      /* no scale, just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
			     (int) destination[(current_dchannel = dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)],
			     (int) destination[(current_dchannel += dchannels)]};
      
      int v_source[] = {(int) source[0],
			(int) source[(current_schannel = schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)],
			(int) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vaddi(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint64) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint64) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel = dchannels)] + source[(current_schannel = schannels)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));
      destination[current_dchannel] = 0xffffffffffffffff & ((gint64) (destination[(current_dchannel += dchannels)] + source[(current_schannel += schannels)]));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffffffffffff & ((gint64) (destination[0] + source[0]));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_float:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s64_to_float(gfloat *destination, guint dchannels,
					gint64 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8float v_destination;
      ags_v8float v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8float) {(gfloat) destination[0],
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gfloat) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gfloat) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gfloat) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gfloat) v_result[0];
      destination[(current_dchannel = dchannels)] = (gfloat) v_result[1];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[2];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[3];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[4];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[5];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[6];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (source[(current_schannel = schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_double:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s64_to_double(gdouble *destination, guint dchannels,
					 gint64 *source, guint schannels,
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
  
    for(; i < limit; i += 8){
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* normalize and copy */
      v_source /= normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gdouble) v_result[0];
      destination[(current_dchannel = dchannels)] = (gdouble) v_result[1];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[2];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[3];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[4];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[5];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[6];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (source[(current_schannel = schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0] / normalize_divisor)));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_float_to_s8:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float_to_s8(gint8 *destination, guint dchannels,
				       gfloat *source, guint schannels,
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
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {source[0],
				source[(current_schannel = schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint8) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint8) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint8) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint8) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xff & (gint8) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_float_to_s16:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float_to_s16(gint16 *destination, guint dchannels,
					gfloat *source, guint schannels,
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
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {source[0],
				source[(current_schannel = schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint16) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint16) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint16) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint16) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffff & (gint16) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_float_to_s24:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float_to_s24(gint32 *destination, guint dchannels,
					gfloat *source, guint schannels,
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
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {source[0],
				source[(current_schannel = schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & (gint32) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_float_to_s32:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float_to_s32(gint32 *destination, guint dchannels,
					gfloat *source, guint schannels,
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
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {source[0],
				source[(current_schannel = schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & (gint32) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_float_to_s64:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float_to_s64(gint64 *destination, guint dchannels,
					gfloat *source, guint schannels,
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
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {source[0],
				source[(current_schannel = schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint64) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint64) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint64) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint64) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_float_to_float:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float_to_float(gfloat *destination, guint dchannels,
					  gfloat *source, guint schannels,
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
				     destination[(current_dchannel = dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {source[0],
				source[(current_schannel = schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gfloat) v_result[0];
      destination[(current_dchannel = dchannels)] = (gfloat) v_result[1];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[2];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[3];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[4];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[5];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[6];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (source[(current_schannel = schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_float_to_double:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float_to_double(gdouble *destination, guint dchannels,
					   gfloat *source, guint schannels,
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
				      destination[(current_dchannel = dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gdouble) v_result[0];
      destination[(current_dchannel = dchannels)] = (gdouble) v_result[1];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[2];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[3];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[4];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[5];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[6];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (source[(current_schannel = schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_double_to_s8:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_double_to_s8(gint8 *destination, guint dchannels,
					gdouble *source, guint schannels,
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
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {source[0],
				 source[(current_schannel = schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint8) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint8) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint8) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint8) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xff & (gint8) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_double_to_s16:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_double_to_s16(gint16 *destination, guint dchannels,
					 gdouble *source, guint schannels,
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
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {source[0],
				 source[(current_schannel = schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint16) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint16) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint16) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint16) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffff & (gint16) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_double_to_s24:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_double_to_s24(gint32 *destination, guint dchannels,
					 gdouble *source, guint schannels,
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
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {source[0],
				 source[(current_schannel = schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & (gint32) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_double_to_s32:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_double_to_s32(gint32 *destination, guint dchannels,
					 gdouble *source, guint schannels,
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
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {source[0],
				 source[(current_schannel = schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & (gint32) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_double_to_s64:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_double_to_s64(gint64 *destination, guint dchannels,
					 gdouble *source, guint schannels,
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
      ags_v8double v_destination;
      ags_v8double v_source;

      current_dchannel = 0;
      current_schannel = 0;

      v_destination = (ags_v8double) {(gdouble) destination[0],
				      (gdouble) destination[(current_dchannel = dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)],
				      (gdouble) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {source[0],
				 source[(current_schannel = schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint64) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint64) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint64) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint64) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_double_to_float:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_double_to_float(gfloat *destination, guint dchannels,
					   gdouble *source, guint schannels,
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
				     destination[(current_dchannel = dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      v_destination += v_source;

      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* no scale, just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gfloat) v_result[0];
      destination[(current_dchannel = dchannels)] = (gfloat) v_result[1];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[2];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[3];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[4];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[5];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[6];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      current_dchannel = 0;
      current_schannel = 0;

      destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (source[(current_schannel = schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_double_to_double:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_double_to_double(gdouble *destination, guint dchannels,
					    gdouble *source, guint schannels,
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
				      destination[(current_dchannel = dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {source[0],
				 source[(current_schannel = schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)],
				 source[(current_schannel += schannels)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gdouble) v_result[0];
      destination[(current_dchannel = dchannels)] = (gdouble) v_result[1];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[2];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[3];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[4];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[5];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[6];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (source[(current_schannel = schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = ((gdouble) ((destination[0]) + (gdouble) (source[0])));

    destination += dchannels;
    source += schannels;
  }
}

#ifdef __APPLE__
/**
 * ags_audio_buffer_util_copy_s8_to_float32:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s8_to_float32(Float32 *destination, guint dchannels,
					 gint8 *source, guint schannels,
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
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (Float32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (Float32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (Float32) v_result[0];
      destination[(current_dchannel = dchannels)] = (Float32) v_result[1];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[2];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[3];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[4];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[5];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[6];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel = dchannels)] + (Float64) (source[(current_schannel = schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0] / normalize_divisor)));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s16_to_float32:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s16_to_float32(Float32 *destination, guint dchannels,
					  gint16 *source, guint schannels,
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
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (Float32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (Float32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (Float32) v_result[0];
      destination[(current_dchannel = dchannels)] = (Float32) v_result[1];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[2];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[3];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[4];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[5];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[6];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel = dchannels)] + (Float64) (source[(current_schannel = schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0] / normalize_divisor)));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s24_to_float32:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s24_to_float32(Float32 *destination, guint dchannels,
					  gint32 *source, guint schannels,
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
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (Float32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (Float32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (Float32) v_result[0];
      destination[(current_dchannel = dchannels)] = (Float32) v_result[1];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[2];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[3];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[4];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[5];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[6];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel = dchannels)] + (Float64) (source[(current_schannel = schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0] / normalize_divisor)));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s32_to_float32:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s32_to_float32(Float32 *destination, guint dchannels,
					  gint32 *source, guint schannels,
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
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (Float32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (Float32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (Float32) v_result[0];
      destination[(current_dchannel = dchannels)] = (Float32) v_result[1];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[2];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[3];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[4];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[5];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[6];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel = dchannels)] + (Float64) (source[(current_schannel = schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0] / normalize_divisor)));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_float32:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s64_to_float32(Float32 *destination, guint dchannels,
					  gint64 *source, guint schannels,
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
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* normalize and copy */
      v_source /= (gfloat) normalize_divisor;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (Float32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (Float32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vdivD(v_normalize_divisor, 0, v_source, 1, v_normalized, 1, 8);
      vDSP_vaddD(v_destination, 1, v_normalized, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (Float32) v_result[0];
      destination[(current_dchannel = dchannels)] = (Float32) v_result[1];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[2];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[3];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[4];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[5];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[6];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel = dchannels)] + (Float64) (source[(current_schannel = schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)] / normalize_divisor)));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0] / normalize_divisor)));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_float_to_float32:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float_to_float32(Float32 *destination, guint dchannels,
					    float *source, guint schannels,
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
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {source[0],
				source[(current_schannel = schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)],
				source[(current_schannel += schannels)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (Float32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (Float32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (Float32) v_result[0];
      destination[(current_dchannel = dchannels)] = (Float32) v_result[1];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[2];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[3];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[4];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[5];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[6];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel = dchannels)] + (Float64) (source[(current_schannel = schannels)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_double_to_float32:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_double_to_float32(Float32 *destination, guint dchannels,
					     gdouble *source, guint schannels,
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
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (Float32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (Float32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (Float32) v_result[0];
      destination[(current_dchannel = dchannels)] = (Float32) v_result[1];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[2];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[3];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[4];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[5];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[6];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel = dchannels)] + (Float64) (source[(current_schannel = schannels)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_float32_to_float32:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float32_to_float32(Float32 *destination, guint dchannels,
					      Float32 *source, guint schannels,
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
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = (Float32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (Float32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (Float32) v_result[0];
      destination[(current_dchannel = dchannels)] = (Float32) v_result[1];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[2];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[3];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[4];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[5];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[6];
      destination[(current_dchannel += dchannels)] = (Float32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel = dchannels)] + (Float64) (source[(current_schannel = schannels)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (Float32) ((Float64) (destination[(current_dchannel += dchannels)] + (Float64) (source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (Float32) ((Float64) (destination[0] + (Float64) (source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_float32_to_s8:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float32_to_s8(gint8 *destination, guint dchannels,
					 Float32 *source, guint schannels,
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
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint8) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint8) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint8) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint8) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint8) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint8) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xff & (gint8) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xff & (gint8) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_float32_to_s16:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float32_to_s16(gint16 *destination, guint dchannels,
					  Float32 *source, guint schannels,
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
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint16) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint16) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint16) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint16) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint16) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint16) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffff & (gint16) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffff & (gint16) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_float32_to_s24:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float32_to_s24(gint32 *destination, guint dchannels,
					  Float32 *source, guint schannels,
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
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & (gint32) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_float32_to_s32:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float32_to_s32(gint32 *destination, guint dchannels,
					  Float32 *source, guint schannels,
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
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint32) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint32) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint32) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint32) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffff & (gint32) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffff & (gint32) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_float32_to_s64:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float32_to_s64(gint64 *destination, guint dchannels,
					  Float32 *source, guint schannels,
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
				     (gfloat) destination[(current_dchannel = dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)],
				     (gfloat) destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* scale and copy */
      v_source *= (gfloat) scale;
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = (gint64) v_destination[0];
      destination[(current_dchannel = dchannels)] = (gint64) v_destination[1];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[2];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[3];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[4];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[5];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[6];
      destination[(current_dchannel += dchannels)] = (gint64) v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* scale and copy */
      vDSP_vmulD(v_source, 1, v_scale, 0, v_scaled, 1, 8);
      vDSP_vaddD(v_destination, 1, v_scaled, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gint64) v_result[0];
      destination[(current_dchannel = dchannels)] = (gint64) v_result[1];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[2];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[3];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[4];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[5];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[6];
      destination[(current_dchannel += dchannels)] = (gint64) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (scale * source[(current_schannel = schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));
      destination[current_dchannel] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (scale * source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = 0xffffffffffffffff & (gint64) ((gdouble) (destination[0] + (gdouble) (scale * source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_float32_to_float:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float32_to_float(gfloat *destination, guint dchannels,
					    Float32 *source, guint schannels,
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
				     destination[(current_dchannel = dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)],
				     destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8float) {(gfloat) source[0],
				(gfloat) source[(current_schannel = schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)],
				(gfloat) source[(current_schannel += schannels)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gfloat) v_result[0];
      destination[(current_dchannel = dchannels)] = (gfloat) v_result[1];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[2];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[3];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[4];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[5];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[6];
      destination[(current_dchannel += dchannels)] = (gfloat) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (source[(current_schannel = schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = (gfloat) ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = (gfloat) ((gdouble) (destination[0] + (gdouble) (source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_float32_to_double:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 * 
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float32_to_double(gdouble *destination, guint dchannels,
					     Float32 *source, guint schannels,
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
				      destination[(current_dchannel = dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)],
				      destination[(current_dchannel += dchannels)]};
      
      v_source = (ags_v8double) {(gdouble) source[0],
				 (gdouble) source[(current_schannel = schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)],
				 (gdouble) source[(current_schannel += schannels)]};

      /* just copy */
      v_destination += v_source;
      
      current_dchannel = 0;
      
      destination[0] = v_destination[0];
      destination[(current_dchannel = dchannels)] = v_destination[1];
      destination[(current_dchannel += dchannels)] = v_destination[2];
      destination[(current_dchannel += dchannels)] = v_destination[3];
      destination[(current_dchannel += dchannels)] = v_destination[4];
      destination[(current_dchannel += dchannels)] = v_destination[5];
      destination[(current_dchannel += dchannels)] = v_destination[6];
      destination[(current_dchannel += dchannels)] = v_destination[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
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
				(double) destination[(current_dchannel = dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)],
				(double) destination[(current_dchannel += dchannels)]};
      
      double v_source[] = {(double) source[0],
			   (double) source[(current_schannel = schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)],
			   (double) source[(current_schannel += schannels)]};

      /* just copy */
      vDSP_vaddD(v_destination, 1, v_source, 1, v_result, 1, 8);
      
      current_dchannel = 0;

      destination[0] = (gdouble) v_result[0];
      destination[(current_dchannel = dchannels)] = (gdouble) v_result[1];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[2];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[3];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[4];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[5];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[6];
      destination[(current_dchannel += dchannels)] = (gdouble) v_result[7];

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#else  
  /* unrolled function */
  if(count > 8){
    limit = count - (count % 8);
  
    for(; i < limit; i += 8){
      destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel = dchannels)] + (gdouble) (source[(current_schannel = schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));
      destination[current_dchannel] = ((gdouble) (destination[(current_dchannel += dchannels)] + (gdouble) (source[(current_schannel += schannels)])));

      /* iterate destination */
      destination += (current_dchannel + dchannels);

      /* iterate source */
      source += (current_schannel + schannels);
    }
  }
#endif
  
  for(; i < count; i++){
    destination[0] = ((gdouble) (destination[0] + (gdouble) (source[0])));

    destination += dchannels;
    source += schannels;
  }
}
#endif

/**
 * ags_audio_buffer_util_copy_s8_to_complex:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s8_to_complex(AgsComplex *destination, guint dchannels,
					 gint8 *source, guint schannels,
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
    
    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s16_to_complex:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s16_to_complex(AgsComplex *destination, guint dchannels,
					  gint16 *source, guint schannels,
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
    
    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s24_to_complex:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s24_to_complex(AgsComplex *destination, guint dchannels,
					  gint32 *source, guint schannels,
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
    
    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s32_to_complex:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s32_to_complex(AgsComplex *destination, guint dchannels,
					  gint32 *source, guint schannels,
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
    
    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_complex:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_s64_to_complex(AgsComplex *destination, guint dchannels,
					  gint64 *source, guint schannels,
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
    
    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_float_to_complex:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float_to_complex(AgsComplex *destination, guint dchannels,
					    gfloat *source, guint schannels,
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
    
    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_double_to_complex:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_double_to_complex(AgsComplex *destination, guint dchannels,
					     gdouble *source, guint schannels,
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
    
    destination += dchannels;
    source += schannels;
  }
}

#ifdef __APPLE__
/**
 * ags_audio_buffer_util_copy_float32_to_complex:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_float32_to_complex(AgsComplex *destination, guint dchannels,
					      Float32 *source, guint schannels,
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
    
    destination += dchannels;
    source += schannels;
  }
}
#endif

/**
 * ags_audio_buffer_util_copy_complex_to_complex:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_complex_to_complex(AgsComplex *destination, guint dchannels,
					      AgsComplex *source, guint schannels,
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
    
    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_complex_to_s8:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_complex_to_s8(gint8 *destination, guint dchannels,
					 AgsComplex *source, guint schannels,
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
    
    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_complex_to_s16:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_complex_to_s16(gint16 *destination, guint dchannels,
					  AgsComplex *source, guint schannels,
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
    
    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_complex_to_s24:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_complex_to_s24(gint32 *destination, guint dchannels,
					  AgsComplex *source, guint schannels,
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
    
    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_complex_to_s32:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_complex_to_s32(gint32 *destination, guint dchannels,
					  AgsComplex *source, guint schannels,
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
    
    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_complex_to_s64:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_complex_to_s64(gint64 *destination, guint dchannels,
					  AgsComplex *source, guint schannels,
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
    
    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_complex_to_float:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_complex_to_float(gfloat *destination, guint dchannels,
					    AgsComplex *source, guint schannels,
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
    
    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_complex_to_double:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_complex_to_double(gdouble *destination, guint dchannels,
					     AgsComplex *source, guint schannels,
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
    
    destination += dchannels;
    source += schannels;
  }
}

#ifdef __APPLE__
/**
 * ags_audio_buffer_util_copy_complex_to_float32:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_complex_to_float32(Float32 *destination, guint dchannels,
					      AgsComplex *source, guint schannels,
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
    
    destination += dchannels;
    source += schannels;
  }
}
#endif

/**
 * ags_audio_buffer_util_copy_buffer_to_buffer:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @doffset: start frame of destination
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @soffset: start frame of source
 * @count: number of frames to copy
 * @mode: specified type conversion as described
 * 
 * Wrapper function to copy functions. Doing type conversion.
 *
 * Since: 3.0.0
 */
void
ags_audio_buffer_util_copy_buffer_to_buffer(void *destination, guint dchannels, guint doffset,
					    void *source, guint schannels, guint soffset,
					    guint count, AgsAudioBufferUtilCopyMode mode)
{
  if(destination == NULL ||
     source == NULL){
    return;
  }

  switch(mode){
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S8:
    {
      ags_audio_buffer_util_copy_s8_to_s8((gint8 *) destination + doffset, dchannels,
					  (gint8 *) source + soffset, schannels,
					  count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S16:
    {
      ags_audio_buffer_util_copy_s8_to_s16((gint16 *) destination + doffset, dchannels,
					   (gint8 *) source + soffset, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S24:
    {
      ags_audio_buffer_util_copy_s8_to_s24((gint32 *) destination + doffset, dchannels,
					   (gint8 *) source + soffset, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S32:
    {
      ags_audio_buffer_util_copy_s8_to_s32((gint32 *) destination + doffset, dchannels,
					   (gint8 *) source + soffset, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S64:
    {
      ags_audio_buffer_util_copy_s8_to_s64((gint64 *) destination + doffset, dchannels,
					   (gint8 *) source + soffset, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s8_to_float((gfloat *) destination + doffset, dchannels,
					     (gint8 *) source + soffset, schannels,
					     count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s8_to_double((gdouble *) destination + doffset, dchannels,
					      (gint8 *) source + soffset, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_COMPLEX:
    {
      ags_audio_buffer_util_copy_s8_to_complex((AgsComplex *) destination + doffset, dchannels,
					       (gint8 *) source + soffset, schannels,
					       count);
    }
    break;

  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S8:
    {
      ags_audio_buffer_util_copy_s16_to_s8((gint8 *) destination + doffset, dchannels,
					   (gint16 *) source + soffset, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16:
    {
      ags_audio_buffer_util_copy_s16_to_s16(((gint16 *) destination) + doffset, dchannels,
					    ((gint16 *) source) + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S24:
    {
      ags_audio_buffer_util_copy_s16_to_s24((gint32 *) destination + doffset, dchannels,
					    (gint16 *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S32:
    {
      ags_audio_buffer_util_copy_s16_to_s32((gint32 *) destination + doffset, dchannels,
					    (gint16 *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S64:
    {
      ags_audio_buffer_util_copy_s16_to_s64((gint64 *) destination + doffset, dchannels,
					    (gint16 *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s16_to_float((gfloat *) destination + doffset, dchannels,
					      (gint16 *) source + soffset, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s16_to_double((gdouble *) destination + doffset, dchannels,
					       (gint16 *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_COMPLEX:
    {
      ags_audio_buffer_util_copy_s16_to_complex((AgsComplex *) destination + doffset, dchannels,
						(gint16 *) source + soffset, schannels,
						count);
    }
    break;

  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S8:
    {
      ags_audio_buffer_util_copy_s24_to_s8((gint8 *) destination + doffset, dchannels,
					   (gint32 *) source + soffset, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S16:
    {
      ags_audio_buffer_util_copy_s24_to_s16((gint16 *) destination + doffset, dchannels,
					    (gint32 *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S24:
    {
      ags_audio_buffer_util_copy_s24_to_s24((gint32 *) destination + doffset, dchannels,
					    (gint32 *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S32:
    {
      ags_audio_buffer_util_copy_s24_to_s32((gint32 *) destination + doffset, dchannels,
					    (gint32 *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S64:
    {
      ags_audio_buffer_util_copy_s24_to_s64((gint64 *) destination + doffset, dchannels,
					    (gint32 *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s24_to_float((gfloat *) destination + doffset, dchannels,
					      (gint32 *) source + soffset, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s24_to_double((gdouble *) destination + doffset, dchannels,
					       (gint32 *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_COMPLEX:
    {
      ags_audio_buffer_util_copy_s24_to_complex((AgsComplex *) destination + doffset, dchannels,
						(gint32 *) source + soffset, schannels,
						count);
    }
    break;

  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S8:
    {
      ags_audio_buffer_util_copy_s32_to_s8((gint8 *) destination + doffset, dchannels,
					   (gint32 *) source + soffset, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S16:
    {
      ags_audio_buffer_util_copy_s32_to_s16((gint16 *) destination + doffset, dchannels,
					    (gint32 *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S24:
    {
      ags_audio_buffer_util_copy_s32_to_s24((gint32 *) destination + doffset, dchannels,
					    (gint32 *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32:
    {
      ags_audio_buffer_util_copy_s32_to_s32((gint32 *) destination + doffset, dchannels,
					    (gint32 *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S64:
    {
      ags_audio_buffer_util_copy_s32_to_s64((gint64 *) destination + doffset, dchannels,
					    (gint32 *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s32_to_s8((gint8 *) destination + doffset, dchannels,
					   (gint32 *) source + soffset, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s32_to_double((gdouble *) destination + doffset, dchannels,
					       (gint32 *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_COMPLEX:
    {
      ags_audio_buffer_util_copy_s32_to_complex((AgsComplex *) destination + doffset, dchannels,
						(gint32 *) source + soffset, schannels,
						count);
    }
    break;

  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S8:
    {
      ags_audio_buffer_util_copy_s64_to_s8((gint8 *) destination + doffset, dchannels,
					   (gint64 *) source + soffset, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S16:
    {
      ags_audio_buffer_util_copy_s64_to_s16((gint16 *) destination + doffset, dchannels,
					    (gint64 *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S24:
    {
      ags_audio_buffer_util_copy_s64_to_s24((gint32 *) destination + doffset, dchannels,
					    (gint64 *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S32:
    {
      ags_audio_buffer_util_copy_s64_to_s32((gint32 *) destination + doffset, dchannels,
					    (gint64 *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S64:
    {
      ags_audio_buffer_util_copy_s64_to_s64((gint64 *) destination + doffset, dchannels,
					    (gint64 *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s64_to_float((gfloat *) destination + doffset, dchannels,
					      (gint64 *) source + soffset, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s64_to_double((gdouble *) destination + doffset, dchannels,
					       (gint64 *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_COMPLEX:
    {
      ags_audio_buffer_util_copy_s64_to_complex((AgsComplex *) destination + doffset, dchannels,
						(gint64 *) source + soffset, schannels,
						count);
    }
    break;

  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S8:
    {
      ags_audio_buffer_util_copy_float_to_s8((gint8 *) destination + doffset, dchannels,
					     (gfloat *) source + soffset, schannels,
					     count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S16:
    {
      ags_audio_buffer_util_copy_float_to_s16((gint16 *) destination + doffset, dchannels,
					      (gfloat *) source + soffset, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S24:
    {
      ags_audio_buffer_util_copy_float_to_s24((gint32 *) destination + doffset, dchannels,
					      (gfloat *) source + soffset, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S32:
    {
      ags_audio_buffer_util_copy_float_to_s32((gint32 *) destination + doffset, dchannels,
					      (gfloat *) source + soffset, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S64:
    {
      ags_audio_buffer_util_copy_float_to_s64((gint64 *) destination + doffset, dchannels,
					      (gfloat *) source + soffset, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_float_to_float((gfloat *) destination + doffset, dchannels,
						(gfloat *) source + soffset, schannels,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_float_to_double((gdouble *) destination + doffset, dchannels,
						 (gfloat *) source + soffset, schannels,
						 count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_COMPLEX:
    {
      ags_audio_buffer_util_copy_float_to_complex((AgsComplex *) destination + doffset, dchannels,
						  (gfloat *) source + soffset, schannels,
						  count);
    }
    break;

  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S8:
    {
      ags_audio_buffer_util_copy_double_to_s8((gint8 *) destination + doffset, dchannels,
					      (gdouble *) source + soffset, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S16:
    {
      ags_audio_buffer_util_copy_double_to_s16((gint16 *) destination + doffset, dchannels,
					       (gdouble *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S24:
    {
      ags_audio_buffer_util_copy_double_to_s24((gint32 *) destination + doffset, dchannels,
					       (gdouble *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S32:
    {
      ags_audio_buffer_util_copy_double_to_s32((gint32 *) destination + doffset, dchannels,
					       (gdouble *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S64:
    {
      ags_audio_buffer_util_copy_double_to_s64((gint64 *) destination + doffset, dchannels,
					       (gdouble *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_double_to_float((gfloat *) destination + doffset, dchannels,
						 (gdouble *) source + soffset, schannels,
						 count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_double_to_double((gdouble *) destination + doffset, dchannels,
						  (gdouble *) source + soffset, schannels,
						  count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_COMPLEX:
    {
      ags_audio_buffer_util_copy_double_to_complex((AgsComplex *) destination + doffset, dchannels,
						   (gdouble *) source + soffset, schannels,
						   count);
    }
    break;

  case AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S8:
    {
      ags_audio_buffer_util_copy_complex_to_s8((gint8 *) destination + doffset, dchannels,
					       (AgsComplex *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S16:
    {
      ags_audio_buffer_util_copy_complex_to_s16((gint16 *) destination + doffset, dchannels,
						(AgsComplex *) source + soffset, schannels,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S24:
    {
      ags_audio_buffer_util_copy_complex_to_s24((gint32 *) destination + doffset, dchannels,
						(AgsComplex *) source + soffset, schannels,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S32:
    {
      ags_audio_buffer_util_copy_complex_to_s32((gint32 *) destination + doffset, dchannels,
						(AgsComplex *) source + soffset, schannels,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S64:
    {
      ags_audio_buffer_util_copy_complex_to_s64((gint64 *) destination + doffset, dchannels,
						(AgsComplex *) source + soffset, schannels,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_complex_to_float((gfloat *) destination + doffset, dchannels,
						  (AgsComplex *) source + soffset, schannels,
						  count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_complex_to_double((gdouble *) destination + doffset, dchannels,
						   (AgsComplex *) source + soffset, schannels,
						   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_COMPLEX:
    {
      ags_audio_buffer_util_copy_complex_to_complex((AgsComplex *) destination + doffset, dchannels,
						    (AgsComplex *) source + soffset, schannels,
						    count);
    }
    break;
  
#ifdef __APPLE__
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_s8_to_float32((Float32 *) destination + doffset, dchannels,
					       (gint8 *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_s16_to_float32((Float32 *) destination + doffset, dchannels,
						((gint16 *) source) + soffset, schannels,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_s24_to_float32((Float32 *) destination + doffset, dchannels,
						(gint32 *) source + soffset, schannels,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_s32_to_float32((Float32 *) destination + doffset, dchannels,
						(gint32 *) source + soffset, schannels,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_s64_to_float32((Float32 *) destination + doffset, dchannels,
						(gint64 *) source + soffset, schannels,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_float_to_float32((Float32 *) destination + doffset, dchannels,
						  (gfloat *) source + soffset, schannels,
						  count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_double_to_float32((Float32 *) destination + doffset, dchannels,
						   (gdouble *) source + soffset, schannels,
						   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_float32_to_float32((Float32 *) destination + doffset, dchannels,
						    (Float32 *) source + soffset, schannels,
						    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_complex_to_float32((Float32 *) destination + doffset, dchannels,
						    (AgsComplex *) source + soffset, schannels,
						    count);
    }
    break;
    
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S8:
    {
      ags_audio_buffer_util_copy_float32_to_s8((gint8 *) destination + doffset, dchannels,
					       (Float32 *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S16:
    {
      ags_audio_buffer_util_copy_float32_to_s16((gint16 *) destination + doffset, dchannels,
						(Float32 *) source + soffset, schannels,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S24:
    {
      ags_audio_buffer_util_copy_float32_to_s24((gint32 *) destination + doffset, dchannels,
						(Float32 *) source + soffset, schannels,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S32:
    {
      ags_audio_buffer_util_copy_float32_to_s32((gint32 *) destination + doffset, dchannels,
						(Float32 *) source + soffset, schannels,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S64:
    {
      ags_audio_buffer_util_copy_float32_to_s64((gint64 *) destination + doffset, dchannels,
						(Float32 *) source + soffset, schannels,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_float32_to_float((gfloat *) destination + doffset, dchannels,
						  (Float32 *) source + soffset, schannels,
						  count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_COMPLEX:
    {
      ags_audio_buffer_util_copy_float32_to_complex((AgsComplex *) destination + doffset, dchannels,
						    (Float32 *) source + soffset, schannels,
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
