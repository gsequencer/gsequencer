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

/**
 * ags_audio_buffer_util_format_from_soundcard:
 * @soundcard_format: the soundcard bit mode
 *
 * Translate soundcard format to audio buffer util format.
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
      g_warning("ags_audio_buffer_util_format_from_soundcard() - unsupported soundcard format\0");
      
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
      default:
	{
	  g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format\0");
	  
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
      default:
	{
	  g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format\0");
	  
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
      default:
	{
	  g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format\0");
	  
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
      default:
	{
	  g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format\0");
	  
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
      default:
	{
	  g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format\0");
	  
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
	  g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format\0");
	  
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
	  g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported destination buffer format\0");
	  
	  return(0);
	}
      }
    }
  default:
    {
      g_warning("ags_audio_buffer_util_get_copy_mode() - unsupported source buffer format\0");
      
      return(0);
    }
  }
}

signed char*
ags_audio_buffer_util_morph_s8(signed char *buffer, guint channels,
			       guint buffer_length,
			       guint morph_lookbehind,
			       AgsComplex *morph)
{
  //TODO:JK: implement me
}

signed short*
ags_audio_buffer_util_morph_s16(signed short *buffer, guint channels,
				guint buffer_length,
				guint morph_lookbehind,
				AgsComplex *morph)
{
  //TODO:JK: implement me
}

signed long*
ags_audio_buffer_util_morph_s24(signed long *buffer, guint channels,
				guint buffer_length,
				guint morph_lookbehind,
				AgsComplex *morph)
{
  //TODO:JK: implement me
}

signed long*
ags_audio_buffer_util_morph_s32(signed long *buffer, guint channels,
				guint buffer_length,
				guint morph_lookbehind,
				AgsComplex *morph)
{
  //TODO:JK: implement me
}

signed long long*
ags_audio_buffer_util_morph_s64(signed long long *buffer, guint channels,
				guint buffer_length,
				guint morph_lookbehind,
				AgsComplex *morph)
{
  //TODO:JK: implement me
}

float*
ags_audio_buffer_util_morph_float(float *buffer, guint channels,
				  guint buffer_length,
				  guint morph_lookbehind,
				  AgsComplex *morph)
{
  //TODO:JK: implement me
}

double*
ags_audio_buffer_util_morph_double(double *buffer, guint channels,
				   guint buffer_length,
				   guint morph_lookbehind,
				   AgsComplex *morph)
{
  //TODO:JK: implement me
}

void*
ags_audio_buffer_util_morph(void *buffer, guint channels,
			    guint format,
			    guint buffer_length,
			    guint morph_lookbehind,
			    AgsComplex *morph)
{
  //TODO:JK: implement me
}

signed char*
ags_audio_buffer_util_resample_s8(signed char *buffer, guint channels,
				  guint samplerate,
				  guint buffer_length,
				  guint target_samplerate)
{
  //TODO:JK: implement me
}

signed short*
ags_audio_buffer_util_resample_s16(signed short *buffer, guint channels,
				   guint samplerate,
				   guint buffer_length,
				   guint target_samplerate)
{
  //TODO:JK: implement me
}

signed long*
ags_audio_buffer_util_resample_s24(signed long *buffer, guint channels,
				   guint samplerate,
				   guint buffer_length,
				   guint target_samplerate)
{
  //TODO:JK: implement me
}

signed long*
ags_audio_buffer_util_resample_s32(signed long *buffer, guint channels,
				   guint samplerate,
				   guint buffer_length,
				   guint target_samplerate)
{
  //TODO:JK: implement me
}

signed long long*
ags_audio_buffer_util_resample_s64(signed long long *buffer, guint channels,
				   guint samplerate,
				   guint buffer_length,
				   guint target_samplerate)
{
  //TODO:JK: implement me
}

float*
ags_audio_buffer_util_resample_float(float *buffer, guint channels,
				     guint samplerate,
				     guint buffer_length,
				     guint target_samplerate)
{
  //TODO:JK: implement me
}

double*
ags_audio_buffer_util_resample_double(double *buffer, guint channels,
				      guint samplerate,
				      guint buffer_length,
				      guint target_samplerate)
{
  //TODO:JK: implement me
}

void*
ags_audio_buffer_util_resample(void *buffer, guint channels,
			       guint format,  guint samplerate,
			       guint buffer_length,
			       guint target_samplerate)
{
  //TODO:JK: implement me
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
      *destination = 0xffff & ((signed long) ((*destination) + (*source)));
      destination[1 * dchannels] = 0xffff & ((signed long) (destination[1 * dchannels] + source[1 * schannels]));
      destination[2 * dchannels] = 0xffff & ((signed long) (destination[2 * dchannels] + source[2 * schannels]));
      destination[3 * dchannels] = 0xffff & ((signed long) (destination[3 * dchannels] + source[3 * schannels]));
      destination[4 * dchannels] = 0xffff & ((signed long) (destination[4 * dchannels] + source[4 * schannels]));
      destination[5 * dchannels] = 0xffff & ((signed long) (destination[5 * dchannels] + source[5 * schannels]));
      destination[6 * dchannels] = 0xffff & ((signed long) (destination[6 * dchannels] + source[6 * schannels]));
      destination[7 * dchannels] = 0xffff & ((signed long) (destination[7 * dchannels] + source[7 * schannels]));

      destination += (8 * dchannels);
      source += (8 * schannels);
    }
  }

  for(; i < count; i++){
    *destination = 0xffff & ((signed long) ((*destination) + (*source)));

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

/**
 * ags_audio_buffer_util_copy_buffer_to_buffer:
 * @destination: destination buffer
 * @dchannels: destination buffer's count of channels
 * @source: source buffer
 * @schannels: source buffer's count of channels
 * @count: number of frames to copy
 * @mode: specified type conversion as described
 * 
 * Wrapper function to copy functions. Doing type conversion.
 *
 * Since: 0.7.0
 */
void
ags_audio_buffer_util_copy_buffer_to_buffer(void *destination, guint dchannels,
					    void *source, guint schannels,
					    guint count, guint mode)
{
  switch(mode){
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S8:
    {
      ags_audio_buffer_util_copy_s8_to_s8((signed char *) destination, dchannels,
					  (signed char *) source, schannels,
					  count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S16:
    {
      ags_audio_buffer_util_copy_s8_to_s16((signed short *) destination, dchannels,
					   (signed char *) source, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S24:
    {
      ags_audio_buffer_util_copy_s8_to_s24((signed long *) destination, dchannels,
					   (signed char *) source, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S32:
    {
      ags_audio_buffer_util_copy_s8_to_s32((signed long *) destination, dchannels,
					   (signed char *) source, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S64:
    {
      ags_audio_buffer_util_copy_s8_to_s64((signed long long *) destination, dchannels,
					   (signed char *) source, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s8_to_float((float *) destination, dchannels,
					     (signed char *) source, schannels,
					     count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s8_to_double((double *) destination, dchannels,
					      (signed char *) source, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S8:
    {
      ags_audio_buffer_util_copy_s16_to_s8((signed char *) destination, dchannels,
					   (signed short *) source, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16:
    {
      ags_audio_buffer_util_copy_s16_to_s16((signed short *) destination, dchannels,
					    (signed short *) source, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S24:
    {
      ags_audio_buffer_util_copy_s16_to_s24((signed long *) destination, dchannels,
					    (signed short *) source, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S32:
    {
      ags_audio_buffer_util_copy_s16_to_s32((signed long *) destination, dchannels,
					    (signed short *) source, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S64:
    {
      ags_audio_buffer_util_copy_s16_to_s64((signed long long *) destination, dchannels,
					    (signed short *) source, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s16_to_float((float *) destination, dchannels,
					      (signed short *) source, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s16_to_double((double *) destination, dchannels,
					       (signed short *) source, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S8:
    {
      ags_audio_buffer_util_copy_s24_to_s8((signed char *) destination, dchannels,
					   (signed long *) source, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S16:
    {
      ags_audio_buffer_util_copy_s24_to_s16((signed short *) destination, dchannels,
					    (signed long *) source, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S24:
    {
      ags_audio_buffer_util_copy_s24_to_s24((signed long *) destination, dchannels,
					    (signed long *) source, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S32:
    {
      ags_audio_buffer_util_copy_s24_to_s32((signed long *) destination, dchannels,
					    (signed long *) source, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S64:
    {
      ags_audio_buffer_util_copy_s24_to_s64((signed long long *) destination, dchannels,
					    (signed long *) source, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s24_to_float((float *) destination, dchannels,
					      (signed long *) source, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s24_to_double((double *) destination, dchannels,
					       (signed long *) source, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S8:
    {
      ags_audio_buffer_util_copy_s32_to_s8((signed char *) destination, dchannels,
					   (signed long *) source, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S16:
    {
      ags_audio_buffer_util_copy_s32_to_s16((signed short *) destination, dchannels,
					    (signed long *) source, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S24:
    {
      ags_audio_buffer_util_copy_s32_to_s24((signed long *) destination, dchannels,
					    (signed long *) source, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32:
    {
      ags_audio_buffer_util_copy_s32_to_s32((signed long *) destination, dchannels,
					    (signed long *) source, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S64:
    {
      ags_audio_buffer_util_copy_s32_to_s64((signed long long *) destination, dchannels,
					    (signed long *) source, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s32_to_s8((float *) destination, dchannels,
					   (signed long *) source, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s32_to_double((double *) destination, dchannels,
					       (signed long *) source, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S8:
    {
      ags_audio_buffer_util_copy_s64_to_s8((signed char *) destination, dchannels,
					   (signed long long *) source, schannels,
					   count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S16:
    {
      ags_audio_buffer_util_copy_s64_to_s16((signed short *) destination, dchannels,
					    (signed long long *) source, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S24:
    {
      ags_audio_buffer_util_copy_s64_to_s24((signed long *) destination, dchannels,
					    (signed long long *) source, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S32:
    {
      ags_audio_buffer_util_copy_s64_to_s32((signed long *) destination, dchannels,
					    (signed long long *) source, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S64:
    {
      ags_audio_buffer_util_copy_s64_to_s64((signed long long *) destination, dchannels,
					    (signed long long *) source, schannels,
					    count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_s64_to_float((float *) destination, dchannels,
					      (signed long long *) source, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_s64_to_double((double *) destination, dchannels,
					       (signed long long *) source, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S8:
    {
      ags_audio_buffer_util_copy_float_to_s8((signed char *) destination, dchannels,
					     (float *) source, schannels,
					     count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S16:
    {
      ags_audio_buffer_util_copy_float_to_s16((signed short *) destination, dchannels,
					      (float *) source, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S24:
    {
      ags_audio_buffer_util_copy_float_to_s24((signed long *) destination, dchannels,
					      (float *) source, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S32:
    {
      ags_audio_buffer_util_copy_float_to_s32((signed long *) destination, dchannels,
					      (float *) source, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S64:
    {
      ags_audio_buffer_util_copy_float_to_s64((signed long long *) destination, dchannels,
					      (float *) source, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_float_to_float((float *) destination, dchannels,
						(float *) source, schannels,
						count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_float_to_double((double *) destination, dchannels,
						 (float *) source, schannels,
						 count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S8:
    {
      ags_audio_buffer_util_copy_double_to_s8((signed char *) destination, dchannels,
					      (double *) source, schannels,
					      count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S16:
    {
      ags_audio_buffer_util_copy_double_to_s16((signed short *) destination, dchannels,
					       (double *) source, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S24:
    {
      ags_audio_buffer_util_copy_double_to_s24((signed long *) destination, dchannels,
					       (double *) source, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S32:
    {
      ags_audio_buffer_util_copy_double_to_s32((signed long *) destination, dchannels,
					       (double *) source, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S64:
    {
      ags_audio_buffer_util_copy_double_to_s64((signed long long *) destination, dchannels,
					       (double *) source, schannels,
					       count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_FLOAT:
    {
      ags_audio_buffer_util_copy_double_to_float((float *) destination, dchannels,
						 (double *) source, schannels,
						 count);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_DOUBLE:
    {
      ags_audio_buffer_util_copy_double_to_double((double *) destination, dchannels,
						  (double *) source, schannels,
						  count);
    }
    break;
  default:
    {
      g_warning("ags_audio_buffer_util.c - unknown copy mode\0");
    }
  }
}
