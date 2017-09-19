/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2016 Joël Krähemann
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

#include <ags/object/ags_soundcard.h>

#include <samplerate.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

/**
 * ags_audio_buffer_util_format_from_soundcard:
 * @soundcard_format: the soundcard bit mode
 *
 * Translate soundcard format to audio buffer util format.
 *
 * Returns: the converted format
 *
 * Since: 0.7.45
 */
guint
ags_audio_buffer_util_format_from_soundcard(guint soundcard_format)
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
 * Since: 0.7.45
 */
guint
ags_audio_buffer_util_get_copy_mode(guint destination_format,
				    guint source_format)
{
  switch(source_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
    {
      /* signed 8 bit source*/
      switch(destination_format){
      case AGS_AUDIO_BUFFER_UTIL_S8:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S8);
      case AGS_AUDIO_BUFFER_UTIL_S16:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S16);
      case AGS_AUDIO_BUFFER_UTIL_S24:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S24);
      case AGS_AUDIO_BUFFER_UTIL_S32:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S32);
      case AGS_AUDIO_BUFFER_UTIL_S64:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S64);
      case AGS_AUDIO_BUFFER_UTIL_FLOAT:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_FLOAT);
      case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_DOUBLE);
#ifdef __APPLE__
      case AGS_AUDIO_BUFFER_UTIL_FLOAT32:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_FLOAT32);
#endif
      default:
	{
	  g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format");
	  
	  return(0);
	}
      }
    }
  case AGS_AUDIO_BUFFER_UTIL_S16:
    {    
      /* signed 16 bit source */  
      switch(destination_format){
      case AGS_AUDIO_BUFFER_UTIL_S8:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S8);
      case AGS_AUDIO_BUFFER_UTIL_S16:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16);
      case AGS_AUDIO_BUFFER_UTIL_S24:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S24);
      case AGS_AUDIO_BUFFER_UTIL_S32:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S32);
      case AGS_AUDIO_BUFFER_UTIL_S64:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S64);
      case AGS_AUDIO_BUFFER_UTIL_FLOAT:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_FLOAT);
      case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_DOUBLE);
#ifdef __APPLE__
      case AGS_AUDIO_BUFFER_UTIL_FLOAT32:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_FLOAT32);
#endif
      default:
	{
	  g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format");
	  
	  return(0);
	}
      }
    }
  case AGS_AUDIO_BUFFER_UTIL_S24:
    {    
      
      /* signed 24 bit source */
      switch(destination_format){
      case AGS_AUDIO_BUFFER_UTIL_S8:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S8);
      case AGS_AUDIO_BUFFER_UTIL_S16:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S16);
      case AGS_AUDIO_BUFFER_UTIL_S24:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S24);
      case AGS_AUDIO_BUFFER_UTIL_S32:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S32);
      case AGS_AUDIO_BUFFER_UTIL_S64:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S64);
      case AGS_AUDIO_BUFFER_UTIL_FLOAT:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_FLOAT);
      case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_DOUBLE);
#ifdef __APPLE__
      case AGS_AUDIO_BUFFER_UTIL_FLOAT32:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_FLOAT32);
#endif
      default:
	{
	  g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format");
	  
	  return(0);
	}
      }
    }
  case AGS_AUDIO_BUFFER_UTIL_S32:
    {    
      /* signed 32 bit source */
      switch(destination_format){
      case AGS_AUDIO_BUFFER_UTIL_S8:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S8);
      case AGS_AUDIO_BUFFER_UTIL_S16:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S16);
      case AGS_AUDIO_BUFFER_UTIL_S24:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S24);
      case AGS_AUDIO_BUFFER_UTIL_S32:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32);
      case AGS_AUDIO_BUFFER_UTIL_S64:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S64);
      case AGS_AUDIO_BUFFER_UTIL_FLOAT:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_FLOAT);
      case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_DOUBLE);
#ifdef __APPLE__
      case AGS_AUDIO_BUFFER_UTIL_FLOAT32:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_FLOAT32);
#endif
      default:
	{
	  g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format");
	  
	  return(0);
	}
      }
    }
  case AGS_AUDIO_BUFFER_UTIL_S64:
    {    
      /* signed 64 bit source */
      switch(destination_format){
      case AGS_AUDIO_BUFFER_UTIL_S8:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S8);
      case AGS_AUDIO_BUFFER_UTIL_S16:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S16);
      case AGS_AUDIO_BUFFER_UTIL_S24:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S24);
      case AGS_AUDIO_BUFFER_UTIL_S32:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S32);
      case AGS_AUDIO_BUFFER_UTIL_S64:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S64);
      case AGS_AUDIO_BUFFER_UTIL_FLOAT:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_FLOAT);
      case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_DOUBLE);
#ifdef __APPLE__
      case AGS_AUDIO_BUFFER_UTIL_FLOAT32:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_FLOAT32);
#endif
      default:
	{
	  g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format");
	  
	  return(0);
	}
      }
    }
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
    {    
      /* float source */
      switch(destination_format){
      case AGS_AUDIO_BUFFER_UTIL_S8:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S8);
      case AGS_AUDIO_BUFFER_UTIL_S16:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S16);
      case AGS_AUDIO_BUFFER_UTIL_S24:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S24);
      case AGS_AUDIO_BUFFER_UTIL_S32:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S32);
      case AGS_AUDIO_BUFFER_UTIL_S64:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S64);
      case AGS_AUDIO_BUFFER_UTIL_FLOAT:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT);
      case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_DOUBLE);
      default:
	{
	  g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format");
	  
	  return(0);
	}
      }
    }
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
    {    
      /* double source */
      switch(destination_format){
      case AGS_AUDIO_BUFFER_UTIL_S8:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S8);
      case AGS_AUDIO_BUFFER_UTIL_S16:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S16);
      case AGS_AUDIO_BUFFER_UTIL_S24:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S24);
      case AGS_AUDIO_BUFFER_UTIL_S32:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S32);
      case AGS_AUDIO_BUFFER_UTIL_S64:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S64);
      case AGS_AUDIO_BUFFER_UTIL_FLOAT:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_FLOAT);
      case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
	return(AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_DOUBLE);
      default:
	{
	  g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format");
	  
	  return(0);
	}
      }
    }
  default:
    {
      g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported source buffer format");
      
      return(0);
    }
  }
}

/**
 * ags_audio_buffer_util_clear_float:
 * @buffer: the buffer to clear
 * @channels: number of channels
 * @count: count frames
 * 
 * Clears a buffer.
 *
 * Since: 0.7.65
 */
void
ags_audio_buffer_util_clear_float(float *buffer, guint channels,
				  guint count)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *buffer = 0.0;
      buffer[1 * channels] = 0.0;
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
 * Since: 0.7.65
 */
void
ags_audio_buffer_util_clear_float32(Float32 *buffer, guint channels,
				    guint count)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *buffer = 0.0;
      buffer[1 * channels] = (Float32) 0.0;
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
 * Since: 0.7.65
 */
void
ags_audio_buffer_util_clear_double(double *buffer, guint channels,
				   guint count)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *buffer = 0.0;
      buffer[1 * channels] = 0.0;
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
 * ags_audio_buffer_util_clear_buffer:
 * @buffer: the audio data
 * @channels: the number of channels
 * @count: number of frames
 * @format: the format
 *
 * Clears the audio data.
 * 
 * Since: 0.7.81
 */
void
ags_audio_buffer_util_clear_buffer(void *buffer, guint channels,
				   guint count, guint format)
{
  switch(format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
    {
      memset((signed char *) buffer, 0, channels * count * sizeof(signed char));
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
    {
      memset((signed short *) buffer, 0, channels * count * sizeof(signed short));
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
    {
      memset((signed long *) buffer, 0, channels * count * sizeof(signed long));
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
    {
      memset((signed long *) buffer, 0, channels * count * sizeof(signed long));
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
    {
      memset((signed long long *) buffer, 0, channels * count * sizeof(signed long long));
    }
  default:
    g_critical("unsupported audio buffer format");
  }
}

/**
 * ags_audio_buffer_util_envelope_s8:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @current_volume: current volume
 * @ratio: the amount to increment
 * 
 * Envelope buffer at @ratio.
 * 
 * Returns: the last volume used
 * 
 * Since: 0.7.111
 */
gdouble
ags_audio_buffer_util_envelope_s8(signed char *buffer, guint channels,
				  guint buffer_length,
				  gdouble current_volume,
				  gdouble ratio)
{
  gdouble start_volume;
  guint limit;
  guint i;

  start_volume = current_volume;
  i = 0;
  
  /* unrolled function */
  if(buffer_length > 8){
    limit = buffer_length - 8;
  
    for(; i < limit; i += 8){
      *buffer = 0xff & ((signed short) ((*buffer) * current_volume));
      buffer[1 * channels] = 0xff & ((signed short) (buffer[1 * channels] * current_volume));
      buffer[2 * channels] = 0xff & ((signed short) (buffer[2 * channels] * current_volume));
      buffer[3 * channels] = 0xff & ((signed short) (buffer[3 * channels] * current_volume));
      buffer[4 * channels] = 0xff & ((signed short) (buffer[4 * channels] * current_volume));
      buffer[5 * channels] = 0xff & ((signed short) (buffer[5 * channels] * current_volume));
      buffer[6 * channels] = 0xff & ((signed short) (buffer[6 * channels] * current_volume));
      buffer[7 * channels] = 0xff & ((signed short) (buffer[7 * channels] * current_volume));
      
      buffer += (8 * channels);
      
      current_volume = start_volume + ((double) i / (double) buffer_length) * ratio;
    }
  }

  for(; i < buffer_length; i++){
    *buffer = 0xff & ((signed short) ((*buffer) * current_volume));

    buffer += channels;

    current_volume = start_volume + ((double) i / (double) buffer_length) * ratio;
  }

  return(current_volume);
}

/**
 * ags_audio_buffer_util_envelope_s16:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @current_volume: current volume
 * @ratio: the amount to increment
 * 
 * Envelope buffer at @ratio.
 * 
 * Returns: the last volume used
 * 
 * Since: 0.7.111
 */
gdouble
ags_audio_buffer_util_envelope_s16(signed short *buffer, guint channels,
				   guint buffer_length,
				   gdouble current_volume,
				   gdouble ratio)
{
  gdouble start_volume;
  guint limit;
  guint i;

  start_volume = current_volume;
  i = 0;
  
  /* unrolled function */
  if(buffer_length > 8){
    limit = buffer_length - 8;
  
    for(; i < limit; i += 8){
      *buffer = (signed short) 0xffff & ((signed long) ((*buffer) * current_volume));
      buffer[1 * channels] = (signed short) 0xffff & ((signed long) (buffer[1 * channels] * current_volume));
      buffer[2 * channels] = (signed short) 0xffff & ((signed long) (buffer[2 * channels] * current_volume));
      buffer[3 * channels] = (signed short) 0xffff & ((signed long) (buffer[3 * channels] * current_volume));
      buffer[4 * channels] = (signed short) 0xffff & ((signed long) (buffer[4 * channels] * current_volume));
      buffer[5 * channels] = (signed short) 0xffff & ((signed long) (buffer[5 * channels] * current_volume));
      buffer[6 * channels] = (signed short) 0xffff & ((signed long) (buffer[6 * channels] * current_volume));
      buffer[7 * channels] = (signed short) 0xffff & ((signed long) (buffer[7 * channels] * current_volume));

      buffer += (8 * channels);

      current_volume = start_volume + ((double) i / (double) buffer_length) * ratio;
    }
  }

  for(; i < buffer_length; i++){
    *buffer = (signed short) 0xffff & ((signed long) ((*buffer) * current_volume));

    buffer += channels;

    current_volume = start_volume + ((double) i / (double) buffer_length) * ratio;
  }

  return(current_volume);
}

/**
 * ags_audio_buffer_util_envelope_s24:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @current_volume: current volume
 * @ratio: the amount to increment
 * 
 * Envelope buffer at @ratio.
 * 
 * Returns: the last volume used
 * 
 * Since: 0.7.111
 */
gdouble
ags_audio_buffer_util_envelope_s24(signed long *buffer, guint channels,
				   guint buffer_length,
				   gdouble current_volume,
				   gdouble ratio)
{
  gdouble start_volume;
  guint limit;
  guint i;

  start_volume = current_volume;
  i = 0;
  
  /* unrolled function */
  if(buffer_length > 8){
    limit = buffer_length - 8;
  
    for(; i < limit; i += 8){
      *buffer = (signed long) 0xffffff & ((signed long) ((*buffer) * current_volume));
      buffer[1 * channels] = (signed long) 0xffffff & ((signed long) (buffer[1 * channels] * current_volume));
      buffer[2 * channels] = (signed long) 0xffffff & ((signed long) (buffer[2 * channels] * current_volume));
      buffer[3 * channels] = (signed long) 0xffffff & ((signed long) (buffer[3 * channels] * current_volume));
      buffer[4 * channels] = (signed long) 0xffffff & ((signed long) (buffer[4 * channels] * current_volume));
      buffer[5 * channels] = (signed long) 0xffffff & ((signed long) (buffer[5 * channels] * current_volume));
      buffer[6 * channels] = (signed long) 0xffffff & ((signed long) (buffer[6 * channels] * current_volume));
      buffer[7 * channels] = (signed long) 0xffffff & ((signed long) (buffer[7 * channels] * current_volume));

      buffer += (8 * channels);

      current_volume = start_volume + ((double) i / (double) buffer_length) * ratio;
    }
  }

  for(; i < buffer_length; i++){
    *buffer = (signed long) 0xffffff & ((signed long) ((*buffer) * current_volume));

    buffer += channels;

    current_volume = start_volume + ((double) i / (double) buffer_length) * ratio;
  }

  return(current_volume);
}

/**
 * ags_audio_buffer_util_envelope_s32:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @current_volume: current volume
 * @ratio: the amount to increment
 * 
 * Envelope buffer at @ratio.
 * 
 * Returns: the last volume used
 * 
 * Since: 0.7.111
 */
gdouble
ags_audio_buffer_util_envelope_s32(signed long *buffer, guint channels,
				   guint buffer_length,
				   gdouble current_volume,
				   gdouble ratio)
{
  gdouble start_volume;
  guint limit;
  guint i;

  start_volume = current_volume;
  i = 0;
  
  /* unrolled function */
  if(buffer_length > 8){
    limit = buffer_length - 8;
  
    for(; i < limit; i += 8){
      *buffer = (signed long) 0xffffffff & ((signed long long) ((*buffer) * current_volume));
      buffer[1 * channels] = (signed long) 0xffffffff & ((signed long long) (buffer[1 * channels] * current_volume));
      buffer[2 * channels] = (signed long) 0xffffffff & ((signed long long) (buffer[2 * channels] * current_volume));
      buffer[3 * channels] = (signed long) 0xffffffff & ((signed long long) (buffer[3 * channels] * current_volume));
      buffer[4 * channels] = (signed long) 0xffffffff & ((signed long long) (buffer[4 * channels] * current_volume));
      buffer[5 * channels] = (signed long) 0xffffffff & ((signed long long) (buffer[5 * channels] * current_volume));
      buffer[6 * channels] = (signed long) 0xffffffff & ((signed long long) (buffer[6 * channels] * current_volume));
      buffer[7 * channels] = (signed long) 0xffffffff & ((signed long long) (buffer[7 * channels] * current_volume));

      buffer += (8 * channels);

      current_volume = start_volume + ((double) i / (double) buffer_length) * ratio;
    }
  }

  for(; i < buffer_length; i++){
    *buffer = (signed long) 0xffffffff & ((signed long long) ((*buffer) * current_volume));

    buffer += channels;

    current_volume = start_volume + ((double) i / (double) buffer_length) * ratio;
  }

  return(current_volume);
}

/**
 * ags_audio_buffer_util_envelope_s64:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @current_volume: current volume
 * @ratio: the amount to increment
 * 
 * Envelope buffer at @ratio.
 * 
 * Returns: the last volume used
 * 
 * Since: 0.7.111
 */
gdouble
ags_audio_buffer_util_envelope_s64(signed long long *buffer, guint channels,
				   guint buffer_length,
				   gdouble current_volume,
				   gdouble ratio)
{
  gdouble start_volume;
  guint limit;
  guint i;

  start_volume = current_volume;
  i = 0;
  
  /* unrolled function */
  if(buffer_length > 8){
    limit = buffer_length - 8;
  
    for(; i < limit; i += 8){
      *buffer = (signed long long) 0xffffffffffffffff & ((signed long long) ((*buffer) * current_volume));
      buffer[1 * channels] = (signed long long) 0xffffffffffffffff & ((signed long long) (buffer[1 * channels] * current_volume));
      buffer[2 * channels] = (signed long long) 0xffffffffffffffff & ((signed long long) (buffer[2 * channels] * current_volume));
      buffer[3 * channels] = (signed long long) 0xffffffffffffffff & ((signed long long) (buffer[3 * channels] * current_volume));
      buffer[4 * channels] = (signed long long) 0xffffffffffffffff & ((signed long long) (buffer[4 * channels] * current_volume));
      buffer[5 * channels] = (signed long long) 0xffffffffffffffff & ((signed long long) (buffer[5 * channels] * current_volume));
      buffer[6 * channels] = (signed long long) 0xffffffffffffffff & ((signed long long) (buffer[6 * channels] * current_volume));
      buffer[7 * channels] = (signed long long) 0xffffffffffffffff & ((signed long long) (buffer[7 * channels] * current_volume));

      buffer += (8 * channels);

      current_volume = start_volume + ((double) i / (double) buffer_length) * ratio;
    }
  }

  for(; i < buffer_length; i++){
    *buffer = (signed long long) 0xffffffffffffffff & ((signed long long) ((*buffer) * current_volume));

    buffer += channels;

    current_volume = start_volume + ((double) i / (double) buffer_length) * ratio;
  }

  return(current_volume);
}

/**
 * ags_audio_buffer_util_envelope_float:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @current_volume: current volume
 * @ratio: the amount to increment
 * 
 * Envelope buffer at @ratio.
 * 
 * Returns: the last volume used
 * 
 * Since: 0.7.111
 */
gdouble
ags_audio_buffer_util_envelope_float(float *buffer, guint channels,
				     guint buffer_length,
				     gdouble current_volume,
				     gdouble ratio)
{
  gdouble start_volume;
  guint limit;
  guint i;

  start_volume = current_volume;
  i = 0;
  
  /* unrolled function */
  if(buffer_length > 8){
    limit = buffer_length - 8;
  
    for(; i < limit; i += 8){
      *buffer = ((double) ((*buffer) * current_volume));
      buffer[1 * channels] = ((double) (buffer[1 * channels] * current_volume));
      buffer[2 * channels] = ((double) (buffer[2 * channels] * current_volume));
      buffer[3 * channels] = ((double) (buffer[3 * channels] * current_volume));
      buffer[4 * channels] = ((double) (buffer[4 * channels] * current_volume));
      buffer[5 * channels] = ((double) (buffer[5 * channels] * current_volume));
      buffer[6 * channels] = ((double) (buffer[6 * channels] * current_volume));
      buffer[7 * channels] = ((double) (buffer[7 * channels] * current_volume));

      buffer += (8 * channels);

      current_volume = start_volume + ((double) i / (double) buffer_length) * ratio;
    }
  }

  for(; i < buffer_length; i++){
    *buffer = ((double) ((*buffer) * current_volume));

    buffer += channels;

    current_volume = start_volume + ((double) i / (double) buffer_length) * ratio;
  }

  return(current_volume);
}

/**
 * ags_audio_buffer_util_envelope_double:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @current_volume: current volume
 * @ratio: the amount to increment
 * 
 * Envelope buffer at @ratio.
 * 
 * Returns: the last volume used
 * 
 * Since: 0.7.111
 */
gdouble
ags_audio_buffer_util_envelope_double(double *buffer, guint channels,
				      guint buffer_length,
				      gdouble current_volume,
				      gdouble ratio)
{
  gdouble start_volume;
  guint limit;
  guint i;

  start_volume = current_volume;
  i = 0;
  
  /* unrolled function */
  if(buffer_length > 8){
    limit = buffer_length - 8;
  
    for(; i < limit; i += 8){
      *buffer = ((double) ((*buffer) * current_volume));
      buffer[1 * channels] = ((double) (buffer[1 * channels] * current_volume));
      buffer[2 * channels] = ((double) (buffer[2 * channels] * current_volume));
      buffer[3 * channels] = ((double) (buffer[3 * channels] * current_volume));
      buffer[4 * channels] = ((double) (buffer[4 * channels] * current_volume));
      buffer[5 * channels] = ((double) (buffer[5 * channels] * current_volume));
      buffer[6 * channels] = ((double) (buffer[6 * channels] * current_volume));
      buffer[7 * channels] = ((double) (buffer[7 * channels] * current_volume));

      buffer += (8 * channels);

      current_volume = start_volume + ((double) i / (double) buffer_length) * ratio;
    }
  }

  for(; i < buffer_length; i++){
    *buffer = ((double) ((*buffer) * current_volume));

    buffer += channels;

    current_volume = start_volume + ((double) i / (double) buffer_length) * ratio;
  }

  return(current_volume);
}

/**
 * ags_audio_buffer_util_envelope:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @format: the format to use
 * @buffer_length: the buffer's length
 * @current_volume: current volume
 * @ratio: the amount to increment
 * 
 * Envelope buffer at @ratio.
 * 
 * Returns: the last volume used
 * 
 * Since: 0.7.111
 */
gdouble
ags_audio_buffer_util_envelope(void *buffer, guint channels,
			       guint format,
			       guint buffer_length,
			       gdouble current_volume,
			       gdouble ratio)
{
  gdouble retval;

  switch(format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
    {
      retval = ags_audio_buffer_util_envelope_s8((signed char *) buffer, channels,
						 buffer_length,
						 current_volume,
						 ratio);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
    {
      retval = ags_audio_buffer_util_envelope_s16((signed short *) buffer, channels,
						  buffer_length,
						  current_volume,
						  ratio);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
    {
      retval = ags_audio_buffer_util_envelope_s24((signed long *) buffer, channels,
						  buffer_length,
						  current_volume,
						  ratio);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
    {
      retval = ags_audio_buffer_util_envelope_s32((signed long *) buffer, channels,
						  buffer_length,
						  current_volume,
						  ratio);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
    {
      retval = ags_audio_buffer_util_envelope_s64((signed long long *) buffer, channels,
						  buffer_length,
						  current_volume,
						  ratio);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
    {
      retval = ags_audio_buffer_util_envelope_float((float *) buffer, channels,
						    buffer_length,
						    current_volume,
						    ratio);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
    {
      retval = ags_audio_buffer_util_envelope_double((double *) buffer, channels,
						     buffer_length,
						     current_volume,
						     ratio);
    }
    break;
  default:
    g_warning("ags_audio_buffer_util_envelope() - unknown format");
  }

  return(retval);
}

/**
 * ags_audio_buffer_util_volume_s8:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @volume: volume
 * 
 * Adjust volume of buffer.
 * 
 * Since: 0.7.111
 */
void
ags_audio_buffer_util_volume_s8(signed char *buffer, guint channels,
				guint buffer_length,
				gdouble volume)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(buffer_length > 8){
    limit = buffer_length - 8;
  
    for(; i < limit; i += 8){
      *buffer = 0xff & ((signed short) ((*buffer) * volume));
      buffer[1 * channels] = 0xff & ((signed short) (buffer[1 * channels] * volume));
      buffer[2 * channels] = 0xff & ((signed short) (buffer[2 * channels] * volume));
      buffer[3 * channels] = 0xff & ((signed short) (buffer[3 * channels] * volume));
      buffer[4 * channels] = 0xff & ((signed short) (buffer[4 * channels] * volume));
      buffer[5 * channels] = 0xff & ((signed short) (buffer[5 * channels] * volume));
      buffer[6 * channels] = 0xff & ((signed short) (buffer[6 * channels] * volume));
      buffer[7 * channels] = 0xff & ((signed short) (buffer[7 * channels] * volume));

      buffer += (8 * channels);
    }
  }

  for(; i < buffer_length; i++){
    *buffer = 0xff & ((signed short) ((*buffer) * volume));

    buffer += channels;
  }
}

/**
 * ags_audio_buffer_util_volume_s16:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @volume: volume
 * 
 * Adjust volume of buffer.
 * 
 * Since: 0.7.111
 */
void
ags_audio_buffer_util_volume_s16(signed short *buffer, guint channels,
				 guint buffer_length,
				 gdouble volume)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(buffer_length > 8){
    limit = buffer_length - 8;
  
    for(; i < limit; i += 8){
      *buffer = (signed short) 0xffff & ((signed long) ((*buffer) * volume));
      buffer[1 * channels] = (signed short) 0xffff & ((signed long) (buffer[1 * channels] * volume));
      buffer[2 * channels] = (signed short) 0xffff & ((signed long) (buffer[2 * channels] * volume));
      buffer[3 * channels] = (signed short) 0xffff & ((signed long) (buffer[3 * channels] * volume));
      buffer[4 * channels] = (signed short) 0xffff & ((signed long) (buffer[4 * channels] * volume));
      buffer[5 * channels] = (signed short) 0xffff & ((signed long) (buffer[5 * channels] * volume));
      buffer[6 * channels] = (signed short) 0xffff & ((signed long) (buffer[6 * channels] * volume));
      buffer[7 * channels] = (signed short) 0xffff & ((signed long) (buffer[7 * channels] * volume));

      buffer += (8 * channels);
    }
  }

  for(; i < buffer_length; i++){
    *buffer = (signed short) 0xffff & ((signed long) ((*buffer) * volume));

    buffer += channels;
  }
}

/**
 * ags_audio_buffer_util_volume_s24:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @volume: volume
 * 
 * Adjust volume of buffer.
 * 
 * Since: 0.7.111
 */
void
ags_audio_buffer_util_volume_s24(signed long *buffer, guint channels,
				 guint buffer_length,
				 gdouble volume)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(buffer_length > 8){
    limit = buffer_length - 8;
  
    for(; i < limit; i += 8){
      *buffer = 0xffffff & ((signed long) ((*buffer) * volume));
      buffer[1 * channels] = 0xffffff & ((signed long) (buffer[1 * channels] * volume));
      buffer[2 * channels] = 0xffffff & ((signed long) (buffer[2 * channels] * volume));
      buffer[3 * channels] = 0xffffff & ((signed long) (buffer[3 * channels] * volume));
      buffer[4 * channels] = 0xffffff & ((signed long) (buffer[4 * channels] * volume));
      buffer[5 * channels] = 0xffffff & ((signed long) (buffer[5 * channels] * volume));
      buffer[6 * channels] = 0xffffff & ((signed long) (buffer[6 * channels] * volume));
      buffer[7 * channels] = 0xffffff & ((signed long) (buffer[7 * channels] * volume));

      buffer += (8 * channels);
    }
  }

  for(; i < buffer_length; i++){
    *buffer = 0xffffff & ((signed long) ((*buffer) * volume));

    buffer += channels;
  }
}

/**
 * ags_audio_buffer_util_volume_s32:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @volume: volume
 * 
 * Adjust volume of buffer.
 * 
 * Since: 0.7.111
 */
void
ags_audio_buffer_util_volume_s32(signed long *buffer, guint channels,
				 guint buffer_length,
				 gdouble volume)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(buffer_length > 8){
    limit = buffer_length - 8;
  
    for(; i < limit; i += 8){
      *buffer = 0xffffffff & ((signed long long) ((*buffer) * volume));
      buffer[1 * channels] = 0xffffffff & ((signed long long) (buffer[1 * channels] * volume));
      buffer[2 * channels] = 0xffffffff & ((signed long long) (buffer[2 * channels] * volume));
      buffer[3 * channels] = 0xffffffff & ((signed long long) (buffer[3 * channels] * volume));
      buffer[4 * channels] = 0xffffffff & ((signed long long) (buffer[4 * channels] * volume));
      buffer[5 * channels] = 0xffffffff & ((signed long long) (buffer[5 * channels] * volume));
      buffer[6 * channels] = 0xffffffff & ((signed long long) (buffer[6 * channels] * volume));
      buffer[7 * channels] = 0xffffffff & ((signed long long) (buffer[7 * channels] * volume));

      buffer += (8 * channels);
    }
  }

  for(; i < buffer_length; i++){
    *buffer = 0xffffffff & ((signed long long) ((*buffer) * volume));

    buffer += channels;
  }
}

/**
 * ags_audio_buffer_util_volume_s64:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @volume: volume
 * 
 * Adjust volume of buffer.
 * 
 * Since: 0.7.111
 */
void
ags_audio_buffer_util_volume_s64(signed long long *buffer, guint channels,
				 guint buffer_length,
				 gdouble volume)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(buffer_length > 8){
    limit = buffer_length - 8;
  
    for(; i < limit; i += 8){
      *buffer = 0xffffffffffffffff & ((signed long long) ((*buffer) * volume));
      buffer[1 * channels] = 0xffffffffffffffff & ((signed long long) (buffer[1 * channels] * volume));
      buffer[2 * channels] = 0xffffffffffffffff & ((signed long long) (buffer[2 * channels] * volume));
      buffer[3 * channels] = 0xffffffffffffffff & ((signed long long) (buffer[3 * channels] * volume));
      buffer[4 * channels] = 0xffffffffffffffff & ((signed long long) (buffer[4 * channels] * volume));
      buffer[5 * channels] = 0xffffffffffffffff & ((signed long long) (buffer[5 * channels] * volume));
      buffer[6 * channels] = 0xffffffffffffffff & ((signed long long) (buffer[6 * channels] * volume));
      buffer[7 * channels] = 0xffffffffffffffff & ((signed long long) (buffer[7 * channels] * volume));

      buffer += (8 * channels);
    }
  }

  for(; i < buffer_length; i++){
    *buffer = 0xffffffffffffffff & ((signed long long) ((*buffer) * volume));

    buffer += channels;
  }
}

/**
 * ags_audio_buffer_util_volume_float:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @volume: volume
 * 
 * Adjust volume of buffer.
 * 
 * Since: 0.7.111
 */
void
ags_audio_buffer_util_volume_float(float *buffer, guint channels,
				   guint buffer_length,
				   gdouble volume)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(buffer_length > 8){
    limit = buffer_length - 8;
  
    for(; i < limit; i += 8){
      *buffer = ((double) ((*buffer) * volume));
      buffer[1 * channels] = ((double) (buffer[1 * channels] * volume));
      buffer[2 * channels] = ((double) (buffer[2 * channels] * volume));
      buffer[3 * channels] = ((double) (buffer[3 * channels] * volume));
      buffer[4 * channels] = ((double) (buffer[4 * channels] * volume));
      buffer[5 * channels] = ((double) (buffer[5 * channels] * volume));
      buffer[6 * channels] = ((double) (buffer[6 * channels] * volume));
      buffer[7 * channels] = ((double) (buffer[7 * channels] * volume));

      buffer += (8 * channels);
    }
  }

  for(; i < buffer_length; i++){
    *buffer = ((double) ((*buffer) * volume));

    buffer += channels;
  }
}

/**
 * ags_audio_buffer_util_volume_double:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @volume: volume
 * 
 * Adjust volume of buffer.
 * 
 * Since: 0.7.111
 */
void
ags_audio_buffer_util_volume_double(double *buffer, guint channels,
				    guint buffer_length,
				    gdouble volume)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(buffer_length > 8){
    limit = buffer_length - 8;
  
    for(; i < limit; i += 8){
      *buffer = ((double) ((*buffer) * volume));
      buffer[1 * channels] = ((double) (buffer[1 * channels] * volume));
      buffer[2 * channels] = ((double) (buffer[2 * channels] * volume));
      buffer[3 * channels] = ((double) (buffer[3 * channels] * volume));
      buffer[4 * channels] = ((double) (buffer[4 * channels] * volume));
      buffer[5 * channels] = ((double) (buffer[5 * channels] * volume));
      buffer[6 * channels] = ((double) (buffer[6 * channels] * volume));
      buffer[7 * channels] = ((double) (buffer[7 * channels] * volume));

      buffer += (8 * channels);
    }
  }

  for(; i < buffer_length; i++){
    *buffer = ((double) ((*buffer) * volume));

    buffer += channels;
  }
}

/**
 * ags_audio_buffer_util_volume:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @format: the format to use
 * @buffer_length: the buffer's length
 * @volume: volume
 * 
 * Adjust volume of buffer.
 * 
 * Since: 0.7.111
 */
void
ags_audio_buffer_util_volume(void *buffer, guint channels,
			     guint format,
			     guint buffer_length,
			     gdouble volume)
{
  switch(format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
    {
      ags_audio_buffer_util_volume_s8((signed char *) buffer, channels,
				      buffer_length,
				      volume);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
    {
      ags_audio_buffer_util_volume_s16((signed short *) buffer, channels,
				       buffer_length,
				       volume);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
    {
      ags_audio_buffer_util_volume_s24((signed long *) buffer, channels,
				       buffer_length,
				       volume);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
    {
      ags_audio_buffer_util_volume_s32((signed long *) buffer, channels,
				       buffer_length,
				       volume);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
    {
      ags_audio_buffer_util_volume_s64((signed long long *) buffer, channels,
				       buffer_length,
				       volume);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
    {
      ags_audio_buffer_util_volume_float((float *) buffer, channels,
					 buffer_length,
					 volume);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
    {
      ags_audio_buffer_util_volume_double((double *) buffer, channels,
					  buffer_length,
					  volume);
    }
    break;
  default:
    g_warning("ags_audio_buffer_util_volume() - unknown format");
  }
}

/**
 * ags_audio_buffer_util_peak_s8:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer length
 * @harmonic_rate: the harmonic rate
 * @max_rate: the max rate
 * @pressure_factor: the pressure factor
 * 
 * Retrive peak of buffer.
 * 
 * Returns: the peak as gdouble
 * 
 * Since: 0.9.6
 */
gdouble
ags_audio_buffer_util_peak_s8(signed char *buffer, guint channels,
			      guint buffer_length,
			      gdouble harmonic_rate,
			      gdouble max_rate,
			      gdouble pressure_factor)
{
  double current_value;
  guint limit;
  guint i;

  /* calculate average value */
  current_value = 0.0;

  i = 0;
  
  /* unrolled function */
  if(buffer_length > 0){
    limit = buffer_length - 8;

    for(; i < limit; i += 8){
      if(buffer[i] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT8 * pressure_factor) * buffer[i]));
      }

      if(buffer[i + 1] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT8 * pressure_factor) * buffer[i + 1]));
      }

      if(buffer[i + 2] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT8 * pressure_factor) * buffer[i + 2]));
      }

      if(buffer[i + 3] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT8 * pressure_factor) * buffer[i + 3]));
      }
    
      if(buffer[i + 4] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT8 * pressure_factor) * buffer[i + 4]));
      }

      if(buffer[i + 5] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT8 * pressure_factor) * buffer[i + 5]));
      }
    
      if(buffer[i + 6] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT8 * pressure_factor) * buffer[i + 6]));
      }

      if(buffer[i + 7] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT8 * pressure_factor) * buffer[i + 7]));
      }
    }
  }
  
  for(; i < buffer_length; i++){
    if(buffer[i] != 0){
      current_value += (1.0 / ((1.0 / (double) G_MAXUINT8 * pressure_factor) * buffer[i]));
    }
  }

  if(current_value != 0.0){
    current_value = (atan(1.0 / harmonic_rate) / sin(current_value / max_rate));
  }

  return(current_value);
}

/**
 * ags_audio_buffer_util_peak_s16:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer length
 * @harmonic_rate: the harmonic rate
 * @max_rate: the max rate
 * @pressure_factor: the pressure factor
 * 
 * Retrive peak of buffer.
 * 
 * Returns: the peak as gdouble
 * 
 * Since: 0.9.6
 */
gdouble
ags_audio_buffer_util_peak_s16(signed short *buffer, guint channels,
			       guint buffer_length,
			       gdouble harmonic_rate,
			       gdouble max_rate,
			       gdouble pressure_factor)
{
  double current_value;
  guint limit;
  guint i;

  /* calculate average value */
  current_value = 0.0;

  i = 0;
  
  /* unrolled function */
  if(buffer_length > 0){
    limit = buffer_length - 8;

    for(; i < limit; i += 8){
      if(buffer[i] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT16 * pressure_factor) * buffer[i]));
      }

      if(buffer[i + 1] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT16 * pressure_factor) * buffer[i + 1]));
      }

      if(buffer[i + 2] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT16 * pressure_factor) * buffer[i + 2]));
      }

      if(buffer[i + 3] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT16 * pressure_factor) * buffer[i + 3]));
      }
    
      if(buffer[i + 4] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT16 * pressure_factor) * buffer[i + 4]));
      }

      if(buffer[i + 5] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT16 * pressure_factor) * buffer[i + 5]));
      }
    
      if(buffer[i + 6] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT16 * pressure_factor) * buffer[i + 6]));
      }

      if(buffer[i + 7] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT16 * pressure_factor) * buffer[i + 7]));
      }
    }
  }
  
  for(; i < buffer_length; i++){
    if(buffer[i] != 0){
      current_value += (1.0 / ((1.0 / (double) G_MAXUINT16 * pressure_factor) * buffer[i]));
    }
  }

  if(current_value != 0.0){
    current_value = (atan(1.0 / harmonic_rate) / sin(current_value / max_rate));
  }

  return(current_value);
}

/**
 * ags_audio_buffer_util_peak_s24:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer length
 * @harmonic_rate: the harmonic rate
 * @max_rate: the max rate
 * @pressure_factor: the pressure factor
 * 
 * Retrive peak of buffer.
 * 
 * Returns: the peak as gdouble
 * 
 * Since: 0.9.6
 */
gdouble
ags_audio_buffer_util_peak_s24(signed long *buffer, guint channels,
			       guint buffer_length,
			       gdouble harmonic_rate,
			       gdouble max_rate,
			       gdouble pressure_factor)
{
  double current_value;
  guint limit;
  guint i;

  /* calculate average value */
  current_value = 0.0;

  i = 0;
  
  /* unrolled function */
  if(buffer_length > 0){
    limit = buffer_length - 8;

    for(; i < limit; i += 8){
      if(buffer[i] != 0){
	current_value += (1.0 / ((1.0 / (double) 0xffffff * pressure_factor) * buffer[i]));
      }

      if(buffer[i + 1] != 0){
	current_value += (1.0 / ((1.0 / (double) 0xffffff * pressure_factor) * buffer[i + 1]));
      }

      if(buffer[i + 2] != 0){
	current_value += (1.0 / ((1.0 / (double) 0xffffff * pressure_factor) * buffer[i + 2]));
      }

      if(buffer[i + 3] != 0){
	current_value += (1.0 / ((1.0 / (double) 0xffffff * pressure_factor) * buffer[i + 3]));
      }
    
      if(buffer[i + 4] != 0){
	current_value += (1.0 / ((1.0 / (double) 0xffffff * pressure_factor) * buffer[i + 4]));
      }

      if(buffer[i + 5] != 0){
	current_value += (1.0 / ((1.0 / (double) 0xffffff * pressure_factor) * buffer[i + 5]));
      }
    
      if(buffer[i + 6] != 0){
	current_value += (1.0 / ((1.0 / (double) 0xffffff * pressure_factor) * buffer[i + 6]));
      }

      if(buffer[i + 7] != 0){
	current_value += (1.0 / ((1.0 / (double) 0xffffff * pressure_factor) * buffer[i + 7]));
      }
    }
  }
  
  for(; i < buffer_length; i++){
    if(buffer[i] != 0){
      current_value += (1.0 / ((1.0 / (double) 0xffffff * pressure_factor) * buffer[i]));
    }
  }

  if(current_value != 0.0){
    current_value = (atan(1.0 / harmonic_rate) / sin(current_value / max_rate));
  }

  return(current_value);
}

/**
 * ags_audio_buffer_util_peak_32:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer length
 * @harmonic_rate: the harmonic rate
 * @max_rate: the max rate
 * @pressure_factor: the pressure factor
 * 
 * Retrive peak of buffer.
 * 
 * Returns: the peak as gdouble
 * 
 * Since: 0.9.6
 */
gdouble
ags_audio_buffer_util_peak_s32(signed long *buffer, guint channels,
			       guint buffer_length,
			       gdouble harmonic_rate,
			       gdouble max_rate,
			       gdouble pressure_factor)
{
  double current_value;
  guint limit;
  guint i;

  /* calculate average value */
  current_value = 0.0;

  i = 0;
  
  /* unrolled function */
  if(buffer_length > 0){
    limit = buffer_length - 8;

    for(; i < limit; i += 8){
      if(buffer[i] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT32 * pressure_factor) * buffer[i]));
      }

      if(buffer[i + 1] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT32 * pressure_factor) * buffer[i + 1]));
      }

      if(buffer[i + 2] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT32 * pressure_factor) * buffer[i + 2]));
      }

      if(buffer[i + 3] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT32 * pressure_factor) * buffer[i + 3]));
      }
    
      if(buffer[i + 4] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT32 * pressure_factor) * buffer[i + 4]));
      }

      if(buffer[i + 5] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT32 * pressure_factor) * buffer[i + 5]));
      }
    
      if(buffer[i + 6] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT32 * pressure_factor) * buffer[i + 6]));
      }

      if(buffer[i + 7] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT32 * pressure_factor) * buffer[i + 7]));
      }
    }
  }
  
  for(; i < buffer_length; i++){
    if(buffer[i] != 0){
      current_value += (1.0 / ((1.0 / (double) G_MAXUINT32 * pressure_factor) * buffer[i]));
    }
  }

  if(current_value != 0.0){
    current_value = (atan(1.0 / harmonic_rate) / sin(current_value / max_rate));
  }

  return(current_value);
}

/**
 * ags_audio_buffer_util_peak_64:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer length
 * @harmonic_rate: the harmonic rate
 * @max_rate: the max rate
 * @pressure_factor: the pressure factor
 * 
 * Retrive peak of buffer.
 * 
 * Returns: the peak as gdouble
 * 
 * Since: 0.9.6
 */
gdouble
ags_audio_buffer_util_peak_s64(signed long long *buffer, guint channels,
			       guint buffer_length,
			       gdouble harmonic_rate,
			       gdouble max_rate,
			       gdouble pressure_factor)
{
  double current_value;
  guint limit;
  guint i;

  /* calculate average value */
  current_value = 0.0;

  i = 0;
  
  /* unrolled function */
  if(buffer_length > 0){
    limit = buffer_length - 8;

    for(; i < limit; i += 8){
      if(buffer[i] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT64 * pressure_factor) * buffer[i]));
      }

      if(buffer[i + 1] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT64 * pressure_factor) * buffer[i + 1]));
      }

      if(buffer[i + 2] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT64 * pressure_factor) * buffer[i + 2]));
      }

      if(buffer[i + 3] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT64 * pressure_factor) * buffer[i + 3]));
      }
    
      if(buffer[i + 4] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT64 * pressure_factor) * buffer[i + 4]));
      }

      if(buffer[i + 5] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT64 * pressure_factor) * buffer[i + 5]));
      }
    
      if(buffer[i + 6] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT64 * pressure_factor) * buffer[i + 6]));
      }

      if(buffer[i + 7] != 0){
	current_value += (1.0 / ((1.0 / (double) G_MAXUINT64 * pressure_factor) * buffer[i + 7]));
      }
    }
  }
  
  for(; i < buffer_length; i++){
    if(buffer[i] != 0){
      current_value += (1.0 / ((1.0 / (double) G_MAXUINT64 * pressure_factor) * buffer[i]));
    }
  }

  if(current_value != 0.0){
    current_value = (atan(1.0 / harmonic_rate) / sin(current_value / max_rate));
  }

  return(current_value);
}

/**
 * ags_audio_buffer_util_peak_float:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer length
 * @harmonic_rate: the harmonic rate
 * @max_rate: the max rate
 * @pressure_factor: the pressure factor
 * 
 * Retrive peak of buffer.
 * 
 * Returns: the peak as gdouble
 * 
 * Since: 0.9.6
 */
gdouble
ags_audio_buffer_util_peak_float(float *buffer, guint channels,
				 guint buffer_length,
				 gdouble harmonic_rate,
				 gdouble max_rate,
				 gdouble pressure_factor)
{
  double current_value;
  guint limit;
  guint i;

  /* calculate average value */
  current_value = 0.0;

  i = 0;
  
  /* unrolled function */
  if(buffer_length > 0){
    limit = buffer_length - 8;

    for(; i < limit; i += 8){
      if(buffer[i] != 0){
	current_value += (1.0 / ((0.5 * pressure_factor) * buffer[i]));
      }

      if(buffer[i + 1] != 0){
	current_value += (1.0 / ((0.5 * pressure_factor) * buffer[i + 1]));
      }

      if(buffer[i + 2] != 0){
	current_value += (1.0 / ((0.5 * pressure_factor) * buffer[i + 2]));
      }

      if(buffer[i + 3] != 0){
	current_value += (1.0 / ((0.5 * pressure_factor) * buffer[i + 3]));
      }
    
      if(buffer[i + 4] != 0){
	current_value += (1.0 / ((0.5 * pressure_factor) * buffer[i + 4]));
      }

      if(buffer[i + 5] != 0){
	current_value += (1.0 / ((0.5 * pressure_factor) * buffer[i + 5]));
      }
    
      if(buffer[i + 6] != 0){
	current_value += (1.0 / ((0.5 * pressure_factor) * buffer[i + 6]));
      }

      if(buffer[i + 7] != 0){
	current_value += (1.0 / ((0.5 * pressure_factor) * buffer[i + 7]));
      }
    }
  }
  
  for(; i < buffer_length; i++){
    if(buffer[i] != 0){
      current_value += (1.0 / ((0.5 * pressure_factor) * buffer[i]));
    }
  }
  
  if(current_value != 0.0){
    current_value = (atan(1.0 / harmonic_rate) / sin(current_value / max_rate));
  }

  return(current_value);
}

/**
 * ags_audio_buffer_util_peak_double:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer length
 * @harmonic_rate: the harmonic rate
 * @max_rate: the max rate
 * @pressure_factor: the pressure factor
 * 
 * Retrive peak of buffer.
 * 
 * Returns: the peak as gdouble
 * 
 * Since: 0.9.6
 */
gdouble
ags_audio_buffer_util_peak_double(double *buffer, guint channels,
				  guint buffer_length,
				  gdouble harmonic_rate,
				  gdouble max_rate,
				  gdouble pressure_factor)
{
  double current_value;
  guint limit;
  guint i;

  /* calculate average value */
  current_value = 0.0;

  i = 0;
  
  /* unrolled function */
  if(buffer_length > 0){
    limit = buffer_length - 8;

    for(; i < limit; i += 8){
      if(buffer[i] != 0){
	current_value += (1.0 / ((0.5 * pressure_factor) * buffer[i]));
      }

      if(buffer[i + 1] != 0){
	current_value += (1.0 / ((0.5 * pressure_factor) * buffer[i + 1]));
      }

      if(buffer[i + 2] != 0){
	current_value += (1.0 / ((0.5 * pressure_factor) * buffer[i + 2]));
      }

      if(buffer[i + 3] != 0){
	current_value += (1.0 / ((0.5 * pressure_factor) * buffer[i + 3]));
      }
    
      if(buffer[i + 4] != 0){
	current_value += (1.0 / ((0.5 * pressure_factor) * buffer[i + 4]));
      }

      if(buffer[i + 5] != 0){
	current_value += (1.0 / ((0.5 * pressure_factor) * buffer[i + 5]));
      }
    
      if(buffer[i + 6] != 0){
	current_value += (1.0 / ((0.5 * pressure_factor) * buffer[i + 6]));
      }

      if(buffer[i + 7] != 0){
	current_value += (1.0 / ((0.5 * pressure_factor) * buffer[i + 7]));
      }
    }
  }
  
  for(; i < buffer_length; i++){
    if(buffer[i] != 0){
      current_value += (1.0 / ((0.5 * pressure_factor) * buffer[i]));
    }
  }

  if(current_value != 0.0){
    current_value = (atan(1.0 / harmonic_rate) / sin(current_value / max_rate));
  }

  return(current_value);
}

/**
 * ags_audio_buffer_util_peak:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @format: the format to use
 * @buffer_length: the buffer length
 * @harmonic_rate: the harmonic rate
 * @max_rate: the max rate
 * @pressure_factor: the pressure factor
 * 
 * Retrive peak of buffer.
 * 
 * Returns: the peak as gdouble
 * 
 * Since: 0.9.6
 */
gdouble
ags_audio_buffer_util_peak(void *buffer, guint channels,
			   guint format,
			   guint buffer_length,
			   gdouble harmonic_rate,
			   gdouble max_rate,
			   gdouble pressure_factor)
{
  gdouble current_value;
  
  switch(format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
    {
      current_value = ags_audio_buffer_util_peak_s8((signed char *) buffer, channels,
						    buffer_length,
						    harmonic_rate,
						    max_rate,
						    pressure_factor);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
    {
      current_value = ags_audio_buffer_util_peak_s16((signed short *) buffer, channels,
						     buffer_length,
						     harmonic_rate,
						     max_rate,
						     pressure_factor);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
    {
      current_value = ags_audio_buffer_util_peak_s24((signed long *) buffer, channels,
						     buffer_length,
						     harmonic_rate,
						     max_rate,
						     pressure_factor);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
    {
      current_value = ags_audio_buffer_util_peak_s32((signed long *) buffer, channels,
						     buffer_length,
						     harmonic_rate,
						     max_rate,
						     pressure_factor);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
    {
      current_value = ags_audio_buffer_util_peak_s64((signed long long *) buffer, channels,
						     buffer_length,
						     harmonic_rate,
						     max_rate,
						     pressure_factor);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
    {
      current_value = ags_audio_buffer_util_peak_float((float *) buffer, channels,
						       buffer_length,
						       harmonic_rate,
						       max_rate,
						       pressure_factor);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
    {
      current_value = ags_audio_buffer_util_peak_double((double *) buffer, channels,
							buffer_length,
							harmonic_rate,
							max_rate,
							pressure_factor);
    }
    break;
  default:
    g_warning("ags_audio_buffer_util_peak() - unknown format");
  }

  return(current_value);
}

/**
 * ags_audio_buffer_util_resample_s8:
 * @buffer: the audio buffer
 * @samplerate: the current samplerate
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @target_samplerate: the samplerate to use
 * 
 * Resamples @buffer from @samplerate to @target_samplerate.
 * 
 * Returns: the resampled audio buffer
 * 
 * Since: 0.7.65
 */
signed char*
ags_audio_buffer_util_resample_s8(signed char *buffer, guint channels,
				  guint samplerate,
				  guint buffer_length,
				  guint target_samplerate)
{
  SRC_DATA secret_rabbit;

  signed char *ret_buffer;

  secret_rabbit.src_ratio = target_samplerate / samplerate;

  secret_rabbit.input_frames = buffer_length;
  secret_rabbit.data_in = (float *) malloc(channels * buffer_length * sizeof(float));
  ags_audio_buffer_util_clear_float(secret_rabbit.data_in, channels,
				    buffer_length);
  ags_audio_buffer_util_copy_s8_to_float(secret_rabbit.data_in, channels,
					 buffer, channels,
					 buffer_length);

  secret_rabbit.output_frames = ceil(secret_rabbit.src_ratio * buffer_length);
  secret_rabbit.data_out = (float *) malloc(channels * secret_rabbit.output_frames * sizeof(float));
  
  src_simple(&secret_rabbit,
	     SRC_SINC_BEST_QUALITY,
	     channels);

  ret_buffer = (signed char *) malloc(channels * secret_rabbit.output_frames * sizeof(signed char));
  memset(ret_buffer, 0, channels * secret_rabbit.output_frames * sizeof(signed char));
  ags_audio_buffer_util_copy_float_to_s8(ret_buffer, channels,
					 secret_rabbit.data_out, channels,
					 secret_rabbit.output_frames);

  return(ret_buffer);
}

/**
 * ags_audio_buffer_util_resample_s16:
 * @buffer: the audio buffer
 * @samplerate: the current samplerate
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @target_samplerate: the samplerate to use
 * 
 * Resamples @buffer from @samplerate to @target_samplerate.
 * 
 * Returns: the resampled audio buffer
 * 
 * Since: 0.7.65
 */
signed short*
ags_audio_buffer_util_resample_s16(signed short *buffer, guint channels,
				   guint samplerate,
				   guint buffer_length,
				   guint target_samplerate)
{
  SRC_DATA secret_rabbit;

  signed short *ret_buffer;

  secret_rabbit.src_ratio = target_samplerate / samplerate;

  secret_rabbit.input_frames = buffer_length;
  secret_rabbit.data_in = (float *) malloc(channels * buffer_length * sizeof(float));
  ags_audio_buffer_util_clear_float(secret_rabbit.data_in, channels,
				    buffer_length);
  ags_audio_buffer_util_copy_s16_to_float(secret_rabbit.data_in, channels,
					  buffer, channels,
					  buffer_length);

  secret_rabbit.output_frames = ceil(secret_rabbit.src_ratio * buffer_length);
  secret_rabbit.data_out = (float *) malloc(channels * secret_rabbit.output_frames * sizeof(float));
  
  src_simple(&secret_rabbit,
	     SRC_SINC_BEST_QUALITY,
	     channels);

  ret_buffer = (signed short *) malloc(channels * secret_rabbit.output_frames * sizeof(signed short));
  memset(ret_buffer, 0, channels * secret_rabbit.output_frames * sizeof(signed short));
  ags_audio_buffer_util_copy_float_to_s16(ret_buffer, channels,
					  secret_rabbit.data_out, channels,
					  secret_rabbit.output_frames);

  return(ret_buffer);
}

/**
 * ags_audio_buffer_util_resample_s24:
 * @buffer: the audio buffer
 * @samplerate: the current samplerate
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @target_samplerate: the samplerate to use
 * 
 * Resamples @buffer from @samplerate to @target_samplerate.
 * 
 * Returns: the resampled audio buffer
 * 
 * Since: 0.7.65
 */
signed long*
ags_audio_buffer_util_resample_s24(signed long *buffer, guint channels,
				   guint samplerate,
				   guint buffer_length,
				   guint target_samplerate)
{
  SRC_DATA secret_rabbit;

  signed long *ret_buffer;

  secret_rabbit.src_ratio = target_samplerate / samplerate;

  secret_rabbit.input_frames = buffer_length;
  secret_rabbit.data_in = (float *) malloc(channels * buffer_length * sizeof(float));
  ags_audio_buffer_util_clear_float(secret_rabbit.data_in, channels,
				    buffer_length);
  ags_audio_buffer_util_copy_s24_to_float(secret_rabbit.data_in, channels,
					  buffer, channels,
					  buffer_length);

  secret_rabbit.output_frames = ceil(secret_rabbit.src_ratio * buffer_length);
  secret_rabbit.data_out = (float *) malloc(channels * secret_rabbit.output_frames * sizeof(float));
  
  src_simple(&secret_rabbit,
	     SRC_SINC_BEST_QUALITY,
	     channels);

  ret_buffer = (signed long *) malloc(channels * secret_rabbit.output_frames * sizeof(signed long));
  memset(ret_buffer, 0, channels * secret_rabbit.output_frames * sizeof(signed long));
  ags_audio_buffer_util_copy_float_to_s24(ret_buffer, channels,
					  secret_rabbit.data_out, channels,
					  secret_rabbit.output_frames);

  return(ret_buffer);
}

/**
 * ags_audio_buffer_util_resample_s32:
 * @buffer: the audio buffer
 * @samplerate: the current samplerate
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @target_samplerate: the samplerate to use
 * 
 * Resamples @buffer from @samplerate to @target_samplerate.
 * 
 * Returns: the resampled audio buffer
 * 
 * Since: 0.7.65
 */
signed long*
ags_audio_buffer_util_resample_s32(signed long *buffer, guint channels,
				   guint samplerate,
				   guint buffer_length,
				   guint target_samplerate)
{
  SRC_DATA secret_rabbit;

  signed long *ret_buffer;

  secret_rabbit.src_ratio = target_samplerate / samplerate;

  secret_rabbit.input_frames = buffer_length;
  secret_rabbit.data_in = (float *) malloc(channels * buffer_length * sizeof(float));
  ags_audio_buffer_util_clear_float(secret_rabbit.data_in, channels,
				    buffer_length);
  ags_audio_buffer_util_copy_s32_to_float(secret_rabbit.data_in, channels,
					  buffer, channels,
					  buffer_length);

  secret_rabbit.output_frames = ceil(secret_rabbit.src_ratio * buffer_length);
  secret_rabbit.data_out = (float *) malloc(channels * secret_rabbit.output_frames * sizeof(float));
  
  src_simple(&secret_rabbit,
	     SRC_SINC_BEST_QUALITY,
	     channels);

  ret_buffer = (signed long *) malloc(channels * secret_rabbit.output_frames * sizeof(signed long));
  memset(ret_buffer, 0, channels * secret_rabbit.output_frames * sizeof(signed long));
  ags_audio_buffer_util_copy_float_to_s32(ret_buffer, channels,
					  secret_rabbit.data_out, channels,
					  secret_rabbit.output_frames);

  return(ret_buffer);
}

/**
 * ags_audio_buffer_util_resample_s64:
 * @buffer: the audio buffer
 * @samplerate: the current samplerate
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @target_samplerate: the samplerate to use
 * 
 * Resamples @buffer from @samplerate to @target_samplerate.
 * 
 * Returns: the resampled audio buffer
 * 
 * Since: 0.7.65
 */
signed long long*
ags_audio_buffer_util_resample_s64(signed long long *buffer, guint channels,
				   guint samplerate,
				   guint buffer_length,
				   guint target_samplerate)
{
  SRC_DATA secret_rabbit;

  signed long long *ret_buffer;

  secret_rabbit.src_ratio = target_samplerate / samplerate;

  secret_rabbit.input_frames = buffer_length;
  secret_rabbit.data_in = (float *) malloc(channels * buffer_length * sizeof(float));
  ags_audio_buffer_util_clear_float(secret_rabbit.data_in, channels,
				    buffer_length);
  ags_audio_buffer_util_copy_s64_to_float(secret_rabbit.data_in, channels,
					  buffer, channels,
					  buffer_length);

  secret_rabbit.output_frames = ceil(secret_rabbit.src_ratio * buffer_length);
  secret_rabbit.data_out = (float *) malloc(channels * secret_rabbit.output_frames * sizeof(float));
  
  src_simple(&secret_rabbit,
	     SRC_SINC_BEST_QUALITY,
	     channels);

  ret_buffer = (signed long long *) malloc(channels * secret_rabbit.output_frames * sizeof(signed long long));
  memset(ret_buffer, 0, channels * secret_rabbit.output_frames * sizeof(signed long long));
  ags_audio_buffer_util_copy_float_to_s64(ret_buffer, channels,
					  secret_rabbit.data_out, channels,
					  secret_rabbit.output_frames);

  return(ret_buffer);
}

/**
 * ags_audio_buffer_util_resample_float:
 * @buffer: the audio buffer
 * @samplerate: the current samplerate
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @target_samplerate: the samplerate to use
 * 
 * Resamples @buffer from @samplerate to @target_samplerate.
 * 
 * Returns: the resampled audio buffer
 * 
 * Since: 0.7.65
 */
float*
ags_audio_buffer_util_resample_float(float *buffer, guint channels,
				     guint samplerate,
				     guint buffer_length,
				     guint target_samplerate)
{
  SRC_DATA secret_rabbit;

  secret_rabbit.src_ratio = target_samplerate / samplerate;

  secret_rabbit.input_frames = buffer_length;
  secret_rabbit.data_in = buffer;

  secret_rabbit.output_frames = ceil(secret_rabbit.src_ratio * buffer_length);
  secret_rabbit.data_out = (float *) malloc(channels * secret_rabbit.output_frames * sizeof(float));

  src_simple(&secret_rabbit,
	     SRC_SINC_BEST_QUALITY,
	     channels);

  return(secret_rabbit.data_out);
}

/**
 * ags_audio_buffer_util_resample_double:
 * @buffer: the audio buffer
 * @samplerate: the current samplerate
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @target_samplerate: the samplerate to use
 * 
 * Resamples @buffer from @samplerate to @target_samplerate.
 * 
 * Returns: the resampled audio buffer
 * 
 * Since: 0.7.65
 */
double*
ags_audio_buffer_util_resample_double(double *buffer, guint channels,
				      guint samplerate,
				      guint buffer_length,
				      guint target_samplerate)
{
  SRC_DATA secret_rabbit;

  double *ret_buffer;

  //FIXME:JK: lost precision

  secret_rabbit.src_ratio = target_samplerate / samplerate;

  secret_rabbit.input_frames = buffer_length;
  secret_rabbit.data_in = (float *) malloc(channels * buffer_length * sizeof(float));
  ags_audio_buffer_util_clear_float(secret_rabbit.data_in, channels,
				    buffer_length);
  ags_audio_buffer_util_copy_double_to_float(secret_rabbit.data_in, channels,
					     buffer, channels,
					     buffer_length);

  secret_rabbit.output_frames = ceil(secret_rabbit.src_ratio * buffer_length);
  secret_rabbit.data_out = (float *) malloc(channels * secret_rabbit.output_frames * sizeof(float));

  src_simple(&secret_rabbit,
	     SRC_SINC_BEST_QUALITY,
	     channels);

  ret_buffer = (double *) malloc(channels * secret_rabbit.output_frames * sizeof(double));
  ags_audio_buffer_util_clear_double(ret_buffer, channels,
				     channels * secret_rabbit.output_frames);
  ags_audio_buffer_util_copy_float_to_double(ret_buffer, channels,
					     secret_rabbit.data_out, channels,
					     secret_rabbit.output_frames);

  return(ret_buffer);
}

void*
ags_audio_buffer_util_resample(void *buffer, guint channels,
			       guint format,  guint samplerate,
			       guint buffer_length,
			       guint target_samplerate)
{
  void *retval;

  switch(format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
    {
      retval = ags_audio_buffer_util_resample_s8((signed char *) buffer, channels,
						 samplerate,
						 buffer_length,
						 target_samplerate);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
    {
      retval = ags_audio_buffer_util_resample_s16((signed short *) buffer, channels,
						  samplerate,
						  buffer_length,
						  target_samplerate);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
    {
      retval = ags_audio_buffer_util_resample_s24((signed long *) buffer, channels,
						  samplerate,
						  buffer_length,
						  target_samplerate);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
    {
      retval = ags_audio_buffer_util_resample_s32((signed long *) buffer, channels,
						  samplerate,
						  buffer_length,
						  target_samplerate);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
    {
      retval = ags_audio_buffer_util_resample_s64((signed long long *) buffer, channels,
						  samplerate,
						  buffer_length,
						  target_samplerate);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
    {
      retval = ags_audio_buffer_util_resample_float((float *) buffer, channels,
						    samplerate,
						    buffer_length,
						    target_samplerate);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
    {
      retval = ags_audio_buffer_util_resample_double((double *) buffer, channels,
						     samplerate,
						     buffer_length,
						     target_samplerate);
    }
    break;
  default:
    g_warning("ags_audio_buffer_util_resample() - unknown format");
  }

  return(retval);
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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s8_to_s8(signed char *destination, guint dchannels,
				    signed char *source, guint schannels,
				    guint count)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xff & ((signed short) ((*destination) + (*source)));
      destination[1 * dchannels] = 0xff & ((signed short) (destination[1 * dchannels] + source[1 * schannels]));
      destination[2 * dchannels] = 0xff & ((signed short) (destination[2 * dchannels] + source[2 * schannels]));
      destination[3 * dchannels] = 0xff & ((signed short) (destination[3 * dchannels] + source[3 * schannels]));
      destination[4 * dchannels] = 0xff & ((signed short) (destination[4 * dchannels] + source[4 * schannels]));
      destination[5 * dchannels] = 0xff & ((signed short) (destination[5 * dchannels] + source[5 * schannels]));
      destination[6 * dchannels] = 0xff & ((signed short) (destination[6 * dchannels] + source[6 * schannels]));
      destination[7 * dchannels] = 0xff & ((signed short) (destination[7 * dchannels] + source[7 * schannels]));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xff & ((signed short) ((*destination) + (*source)));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s8_to_s16(signed short *destination, guint dchannels,
				     signed char *source, guint schannels,
				     guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 258.00787401574803149606;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffff & ((signed long) ((*destination) + (signed long) (scale * source[0])));
      destination[1 * dchannels] = 0xffff & ((signed long) (destination[1 * dchannels] + (signed long) (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffff & ((signed long) (destination[2 * dchannels] + (signed long) (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffff & ((signed long) (destination[3 * dchannels] + (signed long) (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffff & ((signed long) (destination[4 * dchannels] + (signed long) (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffff & ((signed long) (destination[5 * dchannels] + (signed long) (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffff & ((signed long) (destination[6 * dchannels] + (signed long) (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffff & ((signed long) (destination[7 * dchannels] + (signed long) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffff & ((signed long) ((*destination) + (signed long) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s8_to_s24(signed long *destination, guint dchannels,
				     signed char *source, guint schannels,
				     guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 66052.03149606299212598425;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffff & ((signed long) ((*destination) + (signed long) (scale * source[0])));
      destination[1 * dchannels] = 0xffffff & ((signed long) (destination[1 * dchannels] + (signed long) (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffffff & ((signed long) (destination[2 * dchannels] + (signed long) (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffffff & ((signed long) (destination[3 * dchannels] + (signed long) (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffffff & ((signed long) (destination[4 * dchannels] + (signed long) (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffffff & ((signed long) (destination[5 * dchannels] + (signed long) (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffffff & ((signed long) (destination[6 * dchannels] + (signed long) (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffffff & ((signed long) (destination[7 * dchannels] + (signed long) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffff & ((signed long) ((*destination) + (signed long) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s8_to_s32(signed long *destination, guint dchannels,
				     signed char *source, guint schannels,
				     guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 1690931.99212598425196850393;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffffff & ((signed long long) ((*destination) + (signed long) (scale * source[0])));
      destination[1 * dchannels] = 0xffffffff & ((signed long long) (destination[1 * dchannels] + (signed long) (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffffffff & ((signed long long) (destination[2 * dchannels] + (signed long) (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffffffff & ((signed long long) (destination[3 * dchannels] + (signed long) (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffffffff & ((signed long long) (destination[4 * dchannels] + (signed long) (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffffffff & ((signed long long) (destination[5 * dchannels] + (signed long) (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffffffff & ((signed long long) (destination[6 * dchannels] + (signed long) (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffffffff & ((signed long long) (destination[7 * dchannels] + (signed long) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffffff & ((signed long long) ((*destination) + (signed long) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s8_to_s64(signed long long *destination, guint dchannels,
				     signed char *source, guint schannels,
				     guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 72624976668147841.00000000000000000000;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffffffffffffff & ((signed long long) ((*destination) + (signed long long) (scale * source[0])));
      destination[1 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[1 * dchannels] + (signed long long) (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[2 * dchannels] + (signed long long) (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[3 * dchannels] + (signed long long) (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[4 * dchannels] + (signed long long) (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[5 * dchannels] + (signed long long) (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[6 * dchannels] + (signed long long) (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[7 * dchannels] + (signed long long) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffffffffffffff & ((signed long long) ((*destination) + (signed long long) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s8_to_float(float *destination, guint dchannels,
				       signed char *source, guint schannels,
				       guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00787401574803149606;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s8_to_double(double *destination, guint dchannels,
					signed char *source, guint schannels,
					guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00787401574803149606;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s16_to_s8(signed char *destination, guint dchannels,
				     signed short *source, guint schannels,
				     guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00387585070345164342;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xff & ((signed short) ((*destination) + (scale * source[0])));
      destination[1 * dchannels] = 0xff & ((signed short) (destination[1 * dchannels] + (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xff & ((signed short) (destination[2 * dchannels] + (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xff & ((signed short) (destination[3 * dchannels] + (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xff & ((signed short) (destination[4 * dchannels] + (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xff & ((signed short) (destination[5 * dchannels] + (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xff & ((signed short) (destination[6 * dchannels] + (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xff & ((signed short) (destination[7 * dchannels] + (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xff & ((signed short) ((*destination) + (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s16_to_s16(signed short *destination, guint dchannels,
				      signed short *source, guint schannels,
				      guint count)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = (signed short) 0xffff & ((signed long) ((*destination) + (*source)));
      destination[1 * dchannels] = (signed short) 0xffff & ((signed long) (destination[1 * dchannels] + source[1 * schannels]));
      destination[2 * dchannels] = (signed short) 0xffff & ((signed long) (destination[2 * dchannels] + source[2 * schannels]));
      destination[3 * dchannels] = (signed short) 0xffff & ((signed long) (destination[3 * dchannels] + source[3 * schannels]));
      destination[4 * dchannels] = (signed short) 0xffff & ((signed long) (destination[4 * dchannels] + source[4 * schannels]));
      destination[5 * dchannels] = (signed short) 0xffff & ((signed long) (destination[5 * dchannels] + source[5 * schannels]));
      destination[6 * dchannels] = (signed short) 0xffff & ((signed long) (destination[6 * dchannels] + source[6 * schannels]));
      destination[7 * dchannels] = (signed short) 0xffff & ((signed long) (destination[7 * dchannels] + source[7 * schannels]));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = (signed short) 0xffff & ((signed long) ((*destination) + (*source)));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s16_to_s24(signed long *destination, guint dchannels,
				      signed short *source, guint schannels,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 256.00778221991637928403;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffff & ((signed long) ((*destination) + (signed long) (scale * source[0])));
      destination[1 * dchannels] = 0xffffff & ((signed long) (destination[1 * dchannels] + (signed long) (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffffff & ((signed long) (destination[2 * dchannels] + (signed long) (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffffff & ((signed long) (destination[3 * dchannels] + (signed long) (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffffff & ((signed long) (destination[4 * dchannels] + (signed long) (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffffff & ((signed long) (destination[5 * dchannels] + (signed long) (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffffff & ((signed long) (destination[6 * dchannels] + (signed long) (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffffff & ((signed long) (destination[7 * dchannels] + (signed long) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffff & ((signed long) ((*destination) + (signed long) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s16_to_s32(signed long *destination, guint dchannels,
				      signed short *source, guint schannels,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 6553.79995117038483840449;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffffff & ((signed long long) ((*destination) + (signed long) (scale * source[0])));
      destination[1 * dchannels] = 0xffffffff & ((signed long long) (destination[1 * dchannels] + (signed long) (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffffffff & ((signed long long) (destination[2 * dchannels] + (signed long) (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffffffff & ((signed long long) (destination[3 * dchannels] + (signed long) (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffffffff & ((signed long long) (destination[4 * dchannels] + (signed long) (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffffffff & ((signed long long) (destination[5 * dchannels] + (signed long) (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffffffff & ((signed long long) (destination[6 * dchannels] + (signed long) (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffffffff & ((signed long long) (destination[7 * dchannels] + (signed long) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffffff & ((signed long long) ((*destination) + (signed long) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s16_to_s64(signed long long *destination, guint dchannels,
				      signed short *source, guint schannels,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 281483566907400.00021362956633198034;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffffffffffffff & ((signed long long) ((*destination) + (signed long long) (scale * source[0])));
      destination[1 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[1 * dchannels] + (signed long long) (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[2 * dchannels] + (signed long long) (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[3 * dchannels] + (signed long long) (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[4 * dchannels] + (signed long long) (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[5 * dchannels] + (signed long long) (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[6 * dchannels] + (signed long long) (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[7 * dchannels] + (signed long long) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffffffffffffff & ((signed long long) ((*destination) + (signed long long) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s16_to_float(float *destination, guint dchannels,
					signed short *source, guint schannels,
					guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00003051850947599719;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = (float) ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = (float) ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = (float) ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = (float) ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = (float) ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = (float) ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = (float) ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = (float) ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = (float) ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s16_to_double(double *destination, guint dchannels,
					 signed short *source, guint schannels,
					 guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00003051850947599719;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s24_to_s8(signed char *destination, guint dchannels,
				     signed long *source, guint schannels,
				     guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00001513958157772798;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xff & ((signed short) ((*destination) + (scale * source[0])));
      destination[1 * dchannels] = 0xff & ((signed short) (destination[1 * dchannels] + (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xff & ((signed short) (destination[2 * dchannels] + (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xff & ((signed short) (destination[3 * dchannels] + (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xff & ((signed short) (destination[4 * dchannels] + (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xff & ((signed short) (destination[5 * dchannels] + (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xff & ((signed short) (destination[6 * dchannels] + (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xff & ((signed short) (destination[7 * dchannels] + (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xff & ((signed short) ((*destination) + (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s24_to_s16(signed short *destination, guint dchannels,
				      signed long *source, guint schannels,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00390613125635758118;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffff & ((signed long) ((*destination) + (scale * source[0])));
      destination[1 * dchannels] = 0xffff & ((signed long) (destination[1 * dchannels] + (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffff & ((signed long) (destination[2 * dchannels] + (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffff & ((signed long) (destination[3 * dchannels] + (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffff & ((signed long) (destination[4 * dchannels] + (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffff & ((signed long) (destination[5 * dchannels] + (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffff & ((signed long) (destination[6 * dchannels] + (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffff & ((signed long) (destination[7 * dchannels] + (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffff & ((signed long) ((*destination) + (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s24_to_s24(signed long *destination, guint dchannels,
				      signed long *source, guint schannels,
				      guint count)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffff & ((signed long) ((*destination) + (*source)));
      destination[1 * dchannels] = 0xffffff & ((signed long) (destination[1 * dchannels] + source[1 * schannels]));
      destination[2 * dchannels] = 0xffffff & ((signed long) (destination[2 * dchannels] + source[2 * schannels]));
      destination[3 * dchannels] = 0xffffff & ((signed long) (destination[3 * dchannels] + source[3 * schannels]));
      destination[4 * dchannels] = 0xffffff & ((signed long) (destination[4 * dchannels] + source[4 * schannels]));
      destination[5 * dchannels] = 0xffffff & ((signed long) (destination[5 * dchannels] + source[5 * schannels]));
      destination[6 * dchannels] = 0xffffff & ((signed long) (destination[6 * dchannels] + source[6 * schannels]));
      destination[7 * dchannels] = 0xffffff & ((signed long) (destination[7 * dchannels] + source[7 * schannels]));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffff & ((signed long) ((*destination) + (*source)));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s24_to_s32(signed long *destination, guint dchannels,
				      signed long *source, guint schannels,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 25.60000283718142952697;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffffff & ((signed long long) ((*destination) + (scale * source[0])));
      destination[1 * dchannels] = 0xffffffff & ((signed long long) (destination[1 * dchannels] + (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffffffff & ((signed long long) (destination[2 * dchannels] + (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffffffff & ((signed long long) (destination[3 * dchannels] + (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffffffff & ((signed long long) (destination[4 * dchannels] + (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffffffff & ((signed long long) (destination[5 * dchannels] + (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffffffff & ((signed long long) (destination[6 * dchannels] + (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffffffff & ((signed long long) (destination[7 * dchannels] + (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffffff & ((signed long long) ((*destination) + (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s24_to_s64(signed long long *destination, guint dchannels,
				      signed long *source, guint schannels,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 1099511758848.01562488265334160963;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffffffffffffff & ((signed long long) ((*destination) + (scale * source[0])));
      destination[1 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[1 * dchannels] + (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[2 * dchannels] + (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[3 * dchannels] + (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[4 * dchannels] + (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[5 * dchannels] + (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[6 * dchannels] + (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[7 * dchannels] + (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffffffffffffff & ((signed long long) ((*destination) + (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s24_to_float(float *destination, guint dchannels,
					signed long *source, guint schannels,
					guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00000011920930376163;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s24_to_double(double *destination, guint dchannels,
					 signed long *source, guint schannels,
					 guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00000011920930376163;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s32_to_s8(signed char *destination, guint dchannels,
				     signed long *source, guint schannels,
				     guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00000059138983983780;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xff & ((signed short) ((*destination) + (scale * source[0])));
      destination[1 * dchannels] = 0xff & ((signed short) (destination[1 * dchannels] + (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xff & ((signed short) (destination[2 * dchannels] + (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xff & ((signed short) (destination[3 * dchannels] + (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xff & ((signed short) (destination[4 * dchannels] + (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xff & ((signed short) (destination[5 * dchannels] + (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xff & ((signed short) (destination[6 * dchannels] + (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xff & ((signed short) (destination[7 * dchannels] + (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xff & ((signed short) ((*destination) + (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s32_to_s16(signed short *destination, guint dchannels,
				      signed long *source, guint schannels,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00015258323529106482;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffff & ((signed long) ((*destination) + (scale * source[0])));
      destination[1 * dchannels] = 0xffff & ((signed long) (destination[1 * dchannels] + (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffff & ((signed long) (destination[2 * dchannels] + (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffff & ((signed long) (destination[3 * dchannels] + (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffff & ((signed long) (destination[4 * dchannels] + (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffff & ((signed long) (destination[5 * dchannels] + (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffff & ((signed long) (destination[6 * dchannels] + (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffff & ((signed long) (destination[7 * dchannels] + (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffff & ((signed long) ((*destination) + (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s32_to_s24(signed long *destination, guint dchannels,
				      signed long *source, guint schannels,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.03906249548890626240;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffff & ((signed long) ((*destination) + (scale * source[0])));
      destination[1 * dchannels] = 0xffffff & ((signed long) (destination[1 * dchannels] + (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffffff & ((signed long) (destination[2 * dchannels] + (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffffff & ((signed long) (destination[3 * dchannels] + (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffffff & ((signed long) (destination[4 * dchannels] + (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffffff & ((signed long) (destination[5 * dchannels] + (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffffff & ((signed long) (destination[6 * dchannels] + (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffffff & ((signed long) (destination[7 * dchannels] + (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffff & ((signed long) ((*destination) + (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s32_to_s32(signed long *destination, guint dchannels,
				      signed long *source, guint schannels,
				      guint count)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffffff & ((signed long long) ((*destination) + (*source)));
      destination[1 * dchannels] = 0xffffffff & ((signed long long) (destination[1 * dchannels] + source[1 * schannels]));
      destination[2 * dchannels] = 0xffffffff & ((signed long long) (destination[2 * dchannels] + source[2 * schannels]));
      destination[3 * dchannels] = 0xffffffff & ((signed long long) (destination[3 * dchannels] + source[3 * schannels]));
      destination[4 * dchannels] = 0xffffffff & ((signed long long) (destination[4 * dchannels] + source[4 * schannels]));
      destination[5 * dchannels] = 0xffffffff & ((signed long long) (destination[5 * dchannels] + source[5 * schannels]));
      destination[6 * dchannels] = 0xffffffff & ((signed long long) (destination[6 * dchannels] + source[6 * schannels]));
      destination[7 * dchannels] = 0xffffffff & ((signed long long) (destination[7 * dchannels] + source[7 * schannels]));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffffff & ((signed long long) ((*destination) + (*source)));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s32_to_s64(signed long long *destination, guint dchannels,
				      signed long *source, guint schannels,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00000000002328306417;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffffffffffffff & ((signed long long) ((*destination) + (scale * source[0])));
      destination[1 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[1 * dchannels] + (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[2 * dchannels] + (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[3 * dchannels] + (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[4 * dchannels] + (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[5 * dchannels] + (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[6 * dchannels] + (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[7 * dchannels] + (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffffffffffffff & ((signed long long) ((*destination) + (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s32_to_float(float *destination, guint dchannels,
					signed long *source, guint schannels,
					guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00000000465661291210;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s32_to_double(double *destination, guint dchannels,
					 signed long *source, guint schannels,
					 guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00000000465661291210;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s64_to_s8(signed char *destination, guint dchannels,
				     signed long long *source, guint schannels,
				     guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00000000000000001376;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xff & ((signed short) ((*destination) + (scale * source[0])));
      destination[1 * dchannels] = 0xff & ((signed short) (destination[1 * dchannels] + (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xff & ((signed short) (destination[2 * dchannels] + (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xff & ((signed short) (destination[3 * dchannels] + (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xff & ((signed short) (destination[4 * dchannels] + (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xff & ((signed short) (destination[5 * dchannels] + (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xff & ((signed short) (destination[6 * dchannels] + (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xff & ((signed short) (destination[7 * dchannels] + (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xff & ((signed short) ((*destination) + (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s64_to_s16(signed short *destination, guint dchannels,
				      signed long long *source, guint schannels,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00000000000000355260;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffff & ((signed long) ((*destination) + (scale * source[0])));
      destination[1 * dchannels] = 0xffff & ((signed long) (destination[1 * dchannels] + (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffff & ((signed long) (destination[2 * dchannels] + (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffff & ((signed long) (destination[3 * dchannels] + (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffff & ((signed long) (destination[4 * dchannels] + (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffff & ((signed long) (destination[5 * dchannels] + (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffff & ((signed long) (destination[6 * dchannels] + (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffff & ((signed long) (destination[7 * dchannels] + (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffff & ((signed long) ((*destination) + (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s64_to_s24(signed long *destination, guint dchannels,
				      signed long long *source, guint schannels,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00000000000090949459;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffff & ((signed long) ((*destination) + (scale * source[0])));
      destination[1 * dchannels] = 0xffffff & ((signed long) (destination[1 * dchannels] + (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffffff & ((signed long) (destination[2 * dchannels] + (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffffff & ((signed long) (destination[3 * dchannels] + (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffffff & ((signed long) (destination[4 * dchannels] + (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffffff & ((signed long) (destination[5 * dchannels] + (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffffff & ((signed long) (destination[6 * dchannels] + (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffffff & ((signed long) (destination[7 * dchannels] + (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffff & ((signed long) ((*destination) + (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s64_to_s32(signed long *destination, guint dchannels,
				      signed long long *source, guint schannels,
				      guint count)
{
  //NOTE:JK: scale = (2^bits_destination / 2.0 - 1.0) / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00000000002328306417;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffff & ((signed long long) ((*destination) + (scale * source[0])));
      destination[1 * dchannels] = 0xffffff & ((signed long long) (destination[1 * dchannels] + (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffffff & ((signed long long) (destination[2 * dchannels] + (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffffff & ((signed long long) (destination[3 * dchannels] + (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffffff & ((signed long long) (destination[4 * dchannels] + (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffffff & ((signed long long) (destination[5 * dchannels] + (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffffff & ((signed long long) (destination[6 * dchannels] + (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffffff & ((signed long long) (destination[7 * dchannels] + (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffff & ((signed long long) ((*destination) + (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s64_to_s64(signed long long *destination, guint dchannels,
				      signed long long *source, guint schannels,
				      guint count)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffffffffffffff & ((signed long long) ((*destination) + (*source)));
      destination[1 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[1 * dchannels] + source[1 * schannels]));
      destination[2 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[2 * dchannels] + source[2 * schannels]));
      destination[3 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[3 * dchannels] + source[3 * schannels]));
      destination[4 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[4 * dchannels] + source[4 * schannels]));
      destination[5 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[5 * dchannels] + source[5 * schannels]));
      destination[6 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[6 * dchannels] + source[6 * schannels]));
      destination[7 * dchannels] = 0xffffffffffffffff & ((signed long long) (destination[7 * dchannels] + source[7 * schannels]));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffffffffffffff & ((signed long long) ((*destination) + (*source)));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s64_to_float(float *destination, guint dchannels,
					signed long long *source, guint schannels,
					guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00000000000000000010;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_s64_to_double(double *destination, guint dchannels,
					 signed long long *source, guint schannels,
					 guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 0.00000000000000000010;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_float_to_s8(signed char *destination, guint dchannels,
				       float *source, guint schannels,
				       guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 127.0;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xff & (signed char) ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = 0xff & (signed char) ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xff & (signed char) ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xff & (signed char) ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xff & (signed char) ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xff & (signed char) ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xff & (signed char) ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xff & (signed char) ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xff & (signed char) ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_float_to_s16(signed short *destination, guint dchannels,
					float *source, guint schannels,
					guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 32767.0;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffff & (signed short) ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = 0xffff & (signed short) ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffff & (signed short) ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffff & (signed short) ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffff & (signed short) ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffff & (signed short) ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffff & (signed short) ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffff & (signed short) ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffff & (signed short) ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_float_to_s24(signed long *destination, guint dchannels,
					float *source, guint schannels,
					guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 8388607.0;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffff & (signed long) ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = 0xffffff & (signed long) ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffffff & (signed long) ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffffff & (signed long) ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffffff & (signed long) ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffffff & (signed long) ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffffff & (signed long) ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffffff & (signed long) ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffff & (signed long) ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_float_to_s32(signed long *destination, guint dchannels,
					float *source, guint schannels,
					guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 214748363.0;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffffff & (signed long) ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = 0xffffffff & (signed long) ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffffffff & (signed long) ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffffffff & (signed long) ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffffffff & (signed long) ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffffffff & (signed long) ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffffffff & (signed long) ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffffffff & (signed long) ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffffff & (signed long) ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_float_to_s64(signed long long *destination, guint dchannels,
					float *source, guint schannels,
					guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 9223372036854775807.0;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffffffffffffff & (signed long long) ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = 0xffffffffffffffff & (signed long long) ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffffffffffffffff & (signed long long) ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffffffffffffffff & (signed long long) ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffffffffffffffff & (signed long long) ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffffffffffffffff & (signed long long) ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffffffffffffffff & (signed long long) ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffffffffffffffff & (signed long long) ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffffffffffffff & (signed long long) ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_float_to_float(float *destination, guint dchannels,
					  float *source, guint schannels,
					  guint count)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = (float) ((double) ((*destination) + (double) (source[0])));
      destination[1 * dchannels] = (float) ((double) (destination[1 * dchannels] + (double) (source[1 * schannels])));
      destination[2 * dchannels] = (float) ((double) (destination[2 * dchannels] + (double) (source[2 * schannels])));
      destination[3 * dchannels] = (float) ((double) (destination[3 * dchannels] + (double) (source[3 * schannels])));
      destination[4 * dchannels] = (float) ((double) (destination[4 * dchannels] + (double) (source[4 * schannels])));
      destination[5 * dchannels] = (float) ((double) (destination[5 * dchannels] + (double) (source[5 * schannels])));
      destination[6 * dchannels] = (float) ((double) (destination[6 * dchannels] + (double) (source[6 * schannels])));
      destination[7 * dchannels] = (float) ((double) (destination[7 * dchannels] + (double) (source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = (float) ((double) ((*destination) + (double) (source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_float_to_double(double *destination, guint dchannels,
					   float *source, guint schannels,
					   guint count)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = ((double) ((*destination) + (double) (source[0])));
      destination[1 * dchannels] = ((double) (destination[1 * dchannels] + (double) (source[1 * schannels])));
      destination[2 * dchannels] = ((double) (destination[2 * dchannels] + (double) (source[2 * schannels])));
      destination[3 * dchannels] = ((double) (destination[3 * dchannels] + (double) (source[3 * schannels])));
      destination[4 * dchannels] = ((double) (destination[4 * dchannels] + (double) (source[4 * schannels])));
      destination[5 * dchannels] = ((double) (destination[5 * dchannels] + (double) (source[5 * schannels])));
      destination[6 * dchannels] = ((double) (destination[6 * dchannels] + (double) (source[6 * schannels])));
      destination[7 * dchannels] = ((double) (destination[7 * dchannels] + (double) (source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = ((double) ((*destination) + (double) (source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_double_to_s8(signed char *destination, guint dchannels,
					double *source, guint schannels,
					guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 127.0;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xff & (signed char) ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = 0xff & (signed char) ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xff & (signed char) ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xff & (signed char) ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xff & (signed char) ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xff & (signed char) ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xff & (signed char) ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xff & (signed char) ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xff & (signed char) ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_double_to_s16(signed short *destination, guint dchannels,
					 double *source, guint schannels,
					 guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 32767.0;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffff & (signed short) ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = 0xffff & (signed short) ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffff & (signed short) ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffff & (signed short) ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffff & (signed short) ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffff & (signed short) ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffff & (signed short) ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffff & (signed short) ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffff & (signed short) ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_double_to_s24(signed long *destination, guint dchannels,
					 double *source, guint schannels,
					 guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 8388607.0;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffff & (signed long) ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = 0xffffff & (signed long) ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffffff & (signed long) ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffffff & (signed long) ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffffff & (signed long) ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffffff & (signed long) ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffffff & (signed long) ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffffff & (signed long) ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffff & (signed long) ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_double_to_s32(signed long *destination, guint dchannels,
					 double *source, guint schannels,
					 guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 214748363.0;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffffff & (signed long) (((double) destination[0] + (double) (scale * source[0])));
      destination[1 * dchannels] = 0xffffffff & (signed long) (((double) destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffffffff & (signed long) (((double) destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffffffff & (signed long) (((double) destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffffffff & (signed long) (((double) destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffffffff & (signed long) (((double) destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffffffff & (signed long) (((double) destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffffffff & (signed long) (((double) destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffffff & (signed long) (((double) destination[0] + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_double_to_s64(signed long long *destination, guint dchannels,
					 double *source, guint schannels,
					 guint count)
{
  //NOTE:JK: scale = (2^bits_source / 2.0 - 1.0)
  static const gdouble scale = 9223372036854775807.0;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = 0xffffffffffffffff & (signed long long) ((double) ((*destination) + (double) (scale * source[0])));
      destination[1 * dchannels] = 0xffffffffffffffff & (signed long long) ((double) (destination[1 * dchannels] + (double) (scale * source[1 * schannels])));
      destination[2 * dchannels] = 0xffffffffffffffff & (signed long long) ((double) (destination[2 * dchannels] + (double) (scale * source[2 * schannels])));
      destination[3 * dchannels] = 0xffffffffffffffff & (signed long long) ((double) (destination[3 * dchannels] + (double) (scale * source[3 * schannels])));
      destination[4 * dchannels] = 0xffffffffffffffff & (signed long long) ((double) (destination[4 * dchannels] + (double) (scale * source[4 * schannels])));
      destination[5 * dchannels] = 0xffffffffffffffff & (signed long long) ((double) (destination[5 * dchannels] + (double) (scale * source[5 * schannels])));
      destination[6 * dchannels] = 0xffffffffffffffff & (signed long long) ((double) (destination[6 * dchannels] + (double) (scale * source[6 * schannels])));
      destination[7 * dchannels] = 0xffffffffffffffff & (signed long long) ((double) (destination[7 * dchannels] + (double) (scale * source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffffffffffffffff & (signed long long) ((double) ((*destination) + (double) (scale * source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_double_to_float(float *destination, guint dchannels,
					   double *source, guint schannels,
					   guint count)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = (float) ((double) ((*destination) + (double) (source[0])));
      destination[1 * dchannels] = (float) ((double) (destination[1 * dchannels] + (double) (source[1 * schannels])));
      destination[2 * dchannels] = (float) ((double) (destination[2 * dchannels] + (double) (source[2 * schannels])));
      destination[3 * dchannels] = (float) ((double) (destination[3 * dchannels] + (double) (source[3 * schannels])));
      destination[4 * dchannels] = (float) ((double) (destination[4 * dchannels] + (double) (source[4 * schannels])));
      destination[5 * dchannels] = (float) ((double) (destination[5 * dchannels] + (double) (source[5 * schannels])));
      destination[6 * dchannels] = (float) ((double) (destination[6 * dchannels] + (double) (source[6 * schannels])));
      destination[7 * dchannels] = (float) ((double) (destination[7 * dchannels] + (double) (source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = (float) ((double) ((*destination) + (double) (source[0])));

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
 * Since: 0.7.45
 */
void
ags_audio_buffer_util_copy_double_to_double(double *destination, guint dchannels,
					    double *source, guint schannels,
					    guint count)
{
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = ((double) ((*destination) + (double) (source[0])));
      destination[1 * dchannels] = ((double) (destination[1 * dchannels] + (double) (source[1 * schannels])));
      destination[2 * dchannels] = ((double) (destination[2 * dchannels] + (double) (source[2 * schannels])));
      destination[3 * dchannels] = ((double) (destination[3 * dchannels] + (double) (source[3 * schannels])));
      destination[4 * dchannels] = ((double) (destination[4 * dchannels] + (double) (source[4 * schannels])));
      destination[5 * dchannels] = ((double) (destination[5 * dchannels] + (double) (source[5 * schannels])));
      destination[6 * dchannels] = ((double) (destination[6 * dchannels] + (double) (source[6 * schannels])));
      destination[7 * dchannels] = ((double) (destination[7 * dchannels] + (double) (source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = ((double) ((*destination) + (double) (source[0])));

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
 * Since: 0.9.24
 */
void
ags_audio_buffer_util_copy_s8_to_float32(Float32 *destination, guint dchannels,
					 signed char *source, guint schannels,
					 guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  static const Float64 scale = 0.0078740157480314960629854564334861866115034L;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = ((Float64) ((*destination) + (Float64) (scale * (Float64) source[0])));
      destination[1 * dchannels] = ((Float64) (destination[1 * dchannels] + (Float64) (scale * (Float64) source[1 * schannels])));
      destination[2 * dchannels] = ((Float64) (destination[2 * dchannels] + (Float64) (scale * (Float64) source[2 * schannels])));
      destination[3 * dchannels] = ((Float64) (destination[3 * dchannels] + (Float64) (scale * (Float64) source[3 * schannels])));
      destination[4 * dchannels] = ((Float64) (destination[4 * dchannels] + (Float64) (scale * (Float64) source[4 * schannels])));
      destination[5 * dchannels] = ((Float64) (destination[5 * dchannels] + (Float64) (scale * (Float64) source[5 * schannels])));
      destination[6 * dchannels] = ((Float64) (destination[6 * dchannels] + (Float64) (scale * (Float64) source[6 * schannels])));
      destination[7 * dchannels] = ((Float64) (destination[7 * dchannels] + (Float64) (scale * (Float64) source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = ((Float64) ((*destination) + (Float64) (scale * (Float64) source[0])));

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
 * Since: 0.9.24
 */
void
ags_audio_buffer_util_copy_s16_to_float32(Float32 *destination, guint dchannels,
					  signed short *source, guint schannels,
					  guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  static const Float64 scale = 0.0000305185094759971922971274004400890622613L;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = (Float32) (((*destination) + (Float64) (scale * (Float64) source[0])));
      destination[1 * dchannels] = (Float32) ((destination[1 * dchannels] + (Float64) (scale * (Float64) source[1 * schannels])));
      destination[2 * dchannels] = (Float32) ((destination[2 * dchannels] + (Float64) (scale * (Float64) source[2 * schannels])));
      destination[3 * dchannels] = (Float32) ((destination[3 * dchannels] + (Float64) (scale * (Float64) source[3 * schannels])));
      destination[4 * dchannels] = (Float32) ((destination[4 * dchannels] + (Float64) (scale * (Float64) source[4 * schannels])));
      destination[5 * dchannels] = (Float32) ((destination[5 * dchannels] + (Float64) (scale * (Float64) source[5 * schannels])));
      destination[6 * dchannels] = (Float32) ((destination[6 * dchannels] + (Float64) (scale * (Float64) source[6 * schannels])));
      destination[7 * dchannels] = (Float32) ((destination[7 * dchannels] + (Float64) (scale * (Float64) source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = (Float32) (((*destination) + (Float64) (scale * (Float64) source[0])));

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
 * Since: 0.9.24
 */
void
ags_audio_buffer_util_copy_s24_to_float32(Float32 *destination, guint dchannels,
					  signed long *source, guint schannels,
					  guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  static const Float64 scale = 0.0000001192093037616376592678982260231634882L;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = ((Float64) ((*destination) + (Float64) (scale * (Float64) source[0])));
      destination[1 * dchannels] = ((Float64) (destination[1 * dchannels] + (Float64) (scale * (Float64) source[1 * schannels])));
      destination[2 * dchannels] = ((Float64) (destination[2 * dchannels] + (Float64) (scale * (Float64) source[2 * schannels])));
      destination[3 * dchannels] = ((Float64) (destination[3 * dchannels] + (Float64) (scale * (Float64) source[3 * schannels])));
      destination[4 * dchannels] = ((Float64) (destination[4 * dchannels] + (Float64) (scale * (Float64) source[4 * schannels])));
      destination[5 * dchannels] = ((Float64) (destination[5 * dchannels] + (Float64) (scale * (Float64) source[5 * schannels])));
      destination[6 * dchannels] = ((Float64) (destination[6 * dchannels] + (Float64) (scale * (Float64) source[6 * schannels])));
      destination[7 * dchannels] = ((Float64) (destination[7 * dchannels] + (Float64) (scale * (Float64) source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = ((Float64) ((*destination) + (Float64) (scale * (Float64) source[0])));

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
 * Since: 0.9.24
 */
void
ags_audio_buffer_util_copy_s32_to_float32(Float32 *destination, guint dchannels,
					  signed long *source, guint schannels,
					  guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  static const Float64 scale = 0.0000000004656612875245796924105750826697801L;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = ((Float64) ((*destination) + (Float64) (scale * (Float64) source[0])));
      destination[1 * dchannels] = ((Float64) (destination[1 * dchannels] + (Float64) (scale * (Float64) source[1 * schannels])));
      destination[2 * dchannels] = ((Float64) (destination[2 * dchannels] + (Float64) (scale * (Float64) source[2 * schannels])));
      destination[3 * dchannels] = ((Float64) (destination[3 * dchannels] + (Float64) (scale * (Float64) source[3 * schannels])));
      destination[4 * dchannels] = ((Float64) (destination[4 * dchannels] + (Float64) (scale * (Float64) source[4 * schannels])));
      destination[5 * dchannels] = ((Float64) (destination[5 * dchannels] + (Float64) (scale * (Float64) source[5 * schannels])));
      destination[6 * dchannels] = ((Float64) (destination[6 * dchannels] + (Float64) (scale * (Float64) source[6 * schannels])));
      destination[7 * dchannels] = ((Float64) (destination[7 * dchannels] + (Float64) (scale * (Float64) source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = ((Float64) ((*destination) + (Float64) (scale * (Float64) source[0])));

    destination += dchannels;
    source += schannels;
  }
}

/**
 * ags_audio_buffer_util_copy_s64_to_float32o:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 *
 * Copy audio data using additive strategy.
 *
 * Since: 0.9.24
 */
void
ags_audio_buffer_util_copy_s64_to_float32(Float32 *destination, guint dchannels,
					  signed long long *source, guint schannels,
					  guint count)
{
  //NOTE:JK: scale = 1.0 / (2^bits_source / 2.0 - 1.0)
  static const Float64 scale = 0.0000000000000000000722801448323667726912712L;
  guint limit;
  guint i;

  i = 0;
  
  /* unrolled function */
  if(count > 8){
    limit = count - 8;
  
    for(; i < limit; i += 8){
      *destination = ((Float64) ((*destination) + (Float64) (scale * (Float64) source[0])));
      destination[1 * dchannels] = ((Float64) (destination[1 * dchannels] + (Float64) (scale * (Float64) source[1 * schannels])));
      destination[2 * dchannels] = ((Float64) (destination[2 * dchannels] + (Float64) (scale * (Float64) source[2 * schannels])));
      destination[3 * dchannels] = ((Float64) (destination[3 * dchannels] + (Float64) (scale * (Float64) source[3 * schannels])));
      destination[4 * dchannels] = ((Float64) (destination[4 * dchannels] + (Float64) (scale * (Float64) source[4 * schannels])));
      destination[5 * dchannels] = ((Float64) (destination[5 * dchannels] + (Float64) (scale * (Float64) source[5 * schannels])));
      destination[6 * dchannels] = ((Float64) (destination[6 * dchannels] + (Float64) (scale * (Float64) source[6 * schannels])));
      destination[7 * dchannels] = ((Float64) (destination[7 * dchannels] + (Float64) (scale * (Float64) source[7 * schannels])));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = ((Float64) ((*destination) + (Float64) (scale * (Float64) source[0])));

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
 * Since: 0.7.0
 */
void
ags_audio_buffer_util_copy_buffer_to_buffer(void *destination, guint dchannels, guint doffset,
					    void *source, guint schannels, guint soffset,
					    guint count, guint mode)
{
  switch(mode){
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S8:
    {
      ags_audio_buffer_util_copy_s8_to_s8((signed char *) destination + doffset, dchannels,
					  (signed char *) source + soffset, schannels,
					  count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S16:
    {
      ags_audio_buffer_util_copy_s8_to_s16((signed short *) destination + doffset, dchannels,
					   (signed char *) source + soffset, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S24:
    {
      ags_audio_buffer_util_copy_s8_to_s24((signed long *) destination + doffset, dchannels,
					   (signed char *) source + soffset, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S32:
    {
      ags_audio_buffer_util_copy_s8_to_s32((signed long *) destination + doffset, dchannels,
					   (signed char *) source + soffset, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S64:
    {
      ags_audio_buffer_util_copy_s8_to_s64((signed long long *) destination + doffset, dchannels,
					   (signed char *) source + soffset, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s8_to_float((float *) destination + doffset, dchannels,
					     (signed char *) source + soffset, schannels,
					     count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s8_to_double((double *) destination + doffset, dchannels,
					      (signed char *) source + soffset, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S8:
    {
      ags_audio_buffer_util_copy_s16_to_s8((signed char *) destination + doffset, dchannels,
					   (signed short *) source + soffset, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16:
    {
      ags_audio_buffer_util_copy_s16_to_s16(((signed short *) destination) + doffset, dchannels,
					     ((signed short *) source) + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S24:
    {
      ags_audio_buffer_util_copy_s16_to_s24((signed long *) destination + doffset, dchannels,
					    (signed short *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S32:
    {
      ags_audio_buffer_util_copy_s16_to_s32((signed long *) destination + doffset, dchannels,
					    (signed short *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S64:
    {
      ags_audio_buffer_util_copy_s16_to_s64((signed long long *) destination + doffset, dchannels,
					    (signed short *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s16_to_float((float *) destination + doffset, dchannels,
					      (signed short *) source + soffset, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s16_to_double((double *) destination + doffset, dchannels,
					       (signed short *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S8:
    {
      ags_audio_buffer_util_copy_s24_to_s8((signed char *) destination + doffset, dchannels,
					   (signed long *) source + soffset, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S16:
    {
      ags_audio_buffer_util_copy_s24_to_s16((signed short *) destination + doffset, dchannels,
					    (signed long *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S24:
    {
      ags_audio_buffer_util_copy_s24_to_s24((signed long *) destination + doffset, dchannels,
					    (signed long *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S32:
    {
      ags_audio_buffer_util_copy_s24_to_s32((signed long *) destination + doffset, dchannels,
					    (signed long *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S64:
    {
      ags_audio_buffer_util_copy_s24_to_s64((signed long long *) destination + doffset, dchannels,
					    (signed long *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s24_to_float((float *) destination + doffset, dchannels,
					      (signed long *) source + soffset, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s24_to_double((double *) destination + doffset, dchannels,
					       (signed long *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S8:
    {
      ags_audio_buffer_util_copy_s32_to_s8((signed char *) destination + doffset, dchannels,
					   (signed long *) source + soffset, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S16:
    {
      ags_audio_buffer_util_copy_s32_to_s16((signed short *) destination + doffset, dchannels,
					    (signed long *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S24:
    {
      ags_audio_buffer_util_copy_s32_to_s24((signed long *) destination + doffset, dchannels,
					    (signed long *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32:
    {
      ags_audio_buffer_util_copy_s32_to_s32((signed long *) destination + doffset, dchannels,
					    (signed long *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S64:
    {
      ags_audio_buffer_util_copy_s32_to_s64((signed long long *) destination + doffset, dchannels,
					    (signed long *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s32_to_s8((signed char *) destination + doffset, dchannels,
					   (signed long *) source + soffset, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s32_to_double((double *) destination + doffset, dchannels,
					       (signed long *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S8:
    {
      ags_audio_buffer_util_copy_s64_to_s8((signed char *) destination + doffset, dchannels,
					   (signed long long *) source + soffset, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S16:
    {
      ags_audio_buffer_util_copy_s64_to_s16((signed short *) destination + doffset, dchannels,
					    (signed long long *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S24:
    {
      ags_audio_buffer_util_copy_s64_to_s24((signed long *) destination + doffset, dchannels,
					    (signed long long *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S32:
    {
      ags_audio_buffer_util_copy_s64_to_s32((signed long *) destination + doffset, dchannels,
					    (signed long long *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S64:
    {
      ags_audio_buffer_util_copy_s64_to_s64((signed long long *) destination + doffset, dchannels,
					    (signed long long *) source + soffset, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s64_to_float((float *) destination + doffset, dchannels,
					      (signed long long *) source + soffset, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s64_to_double((double *) destination + doffset, dchannels,
					       (signed long long *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S8:
    {
      ags_audio_buffer_util_copy_float_to_s8((signed char *) destination + doffset, dchannels,
					     (float *) source + soffset, schannels,
					     count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S16:
    {
      ags_audio_buffer_util_copy_float_to_s16((signed short *) destination + doffset, dchannels,
					      (float *) source + soffset, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S24:
    {
      ags_audio_buffer_util_copy_float_to_s24((signed long *) destination + doffset, dchannels,
					      (float *) source + soffset, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S32:
    {
      ags_audio_buffer_util_copy_float_to_s32((signed long *) destination + doffset, dchannels,
					      (float *) source + soffset, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S64:
    {
      ags_audio_buffer_util_copy_float_to_s64((signed long long *) destination + doffset, dchannels,
					      (float *) source + soffset, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_float_to_float((float *) destination + doffset, dchannels,
						(float *) source + soffset, schannels,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_float_to_double((double *) destination + doffset, dchannels,
						 (float *) source + soffset, schannels,
						 count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S8:
    {
      ags_audio_buffer_util_copy_double_to_s8((signed char *) destination + doffset, dchannels,
					      (double *) source + soffset, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S16:
    {
      ags_audio_buffer_util_copy_double_to_s16((signed short *) destination + doffset, dchannels,
					       (double *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S24:
    {
      ags_audio_buffer_util_copy_double_to_s24((signed long *) destination + doffset, dchannels,
					       (double *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S32:
    {
      ags_audio_buffer_util_copy_double_to_s32((signed long *) destination + doffset, dchannels,
					       (double *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S64:
    {
      ags_audio_buffer_util_copy_double_to_s64((signed long long *) destination + doffset, dchannels,
					       (double *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_double_to_float((float *) destination + doffset, dchannels,
						 (double *) source + soffset, schannels,
						 count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_double_to_double((double *) destination + doffset, dchannels,
						  (double *) source + soffset, schannels,
						  count);
    }
    break;
#ifdef __APPLE__
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_s8_to_float32((Float32 *) destination + doffset, dchannels,
					       (signed char *) source + soffset, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_s16_to_float32((Float32 *) destination + doffset, dchannels,
						((signed short *) source) + soffset, schannels,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_s24_to_float32((Float32 *) destination + doffset, dchannels,
						(signed long *) source + soffset, schannels,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_s32_to_float32((Float32 *) destination + doffset, dchannels,
						(signed long *) source + soffset, schannels,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_FLOAT32:
    {
      ags_audio_buffer_util_copy_s64_to_float32((Float32 *) destination + doffset, dchannels,
						(signed long long *) source + soffset, schannels,
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
