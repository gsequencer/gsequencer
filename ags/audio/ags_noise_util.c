/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

/* noise.cpp
 *
 * Computer Music Toolkit - a library of LADSPA plugins. Copyright (C)
 * 2000-2002 Richard W.E. Furse.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence as
 * published by the Free Software Foundation; either version 2 of the
 * Licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA
*/

/* pink_sh.cpp
 *
 * A sample-and-hold pink noise generator.
 *
 * (c) 2002 Nathaniel Virgo
 *
 * Part of the Computer Music Toolkit - a library of LADSPA plugins. 
 * The Computer Music Toolkit is Copyright (C) 2000-2002 
 * Richard W.E. Furse.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence as
 * published by the Free Software Foundation; either version 2 of the
 * Licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <ags/audio/ags_noise_util.h>

#include <ags/audio/ags_audio_buffer_util.h>

#if defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#include <Accelerate/Accelerate.h>
#endif

#include <stdlib.h>
#include <math.h>

/**
 * SECTION:ags_noise_util
 * @short_description: noise util
 * @title: AgsNoiseUtil
 * @section_id:
 * @include: ags/audio/ags_noise_util.h
 *
 * Utility functions to compute noise.
 */

GType
ags_noise_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_noise_util = 0;

    ags_type_noise_util =
      g_boxed_type_register_static("AgsNoiseUtil",
				   (GBoxedCopyFunc) ags_noise_util_copy,
				   (GBoxedFreeFunc) ags_noise_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_noise_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_noise_util_alloc:
 * 
 * Allocate #AgsNoiseUtil-struct.
 * 
 * Returns: the newly allocated #AgsNoiseUtil-struct
 * 
 * Since: 3.15.0
 */
AgsNoiseUtil*
ags_noise_util_alloc()
{
  AgsNoiseUtil *ptr;
  
  ptr = (AgsNoiseUtil *) g_new(AgsNoiseUtil,
			       1);

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->destination = NULL;
  ptr->destination_stride = 1;

  ptr->noise = NULL;
  
  ptr->buffer_length = 0;
  ptr->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  ptr->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  ptr->mode = AGS_NOISE_UTIL_WHITE_NOISE;

  ptr->volume = 0.0;

  ptr->frequency = AGS_NOISE_UTIL_DEFAULT_FREQUENCY;

  ptr->frame_count = 0;
  ptr->offset = 0;

  return(ptr);
}

/**
 * ags_noise_util_copy:
 * @ptr: the original #AgsNoiseUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsNoiseUtil-struct
 *
 * Since: 3.15.0
 */
gpointer
ags_noise_util_copy(AgsNoiseUtil *ptr)
{
  AgsNoiseUtil *new_ptr;
  
  new_ptr = (AgsNoiseUtil *) g_new(AgsNoiseUtil,
				   1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;
  
  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->mode = ptr->mode;

  new_ptr->volume = ptr->volume;

  new_ptr->frequency = ptr->frequency;

  new_ptr->frame_count = ptr->frame_count;
  new_ptr->offset = ptr->offset;
  
  return(new_ptr);
}

/**
 * ags_noise_util_free:
 * @ptr: the #AgsNoiseUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_free(AgsNoiseUtil *ptr)
{
  g_free(ptr->destination);

  if(ptr->destination != ptr->source){
    g_free(ptr->source);
  }
  
  g_free(ptr);
}

/**
 * ags_noise_util_get_destination:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get destination buffer of @noise_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 3.15.0
 */
gpointer
ags_noise_util_get_destination(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(NULL);
  }

  return(noise_util->destination);
}

/**
 * ags_noise_util_set_destination:
 * @noise_util: the #AgsNoiseUtil-struct
 * @destination: the destination buffer
 *
 * Set @destination buffer of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_destination(AgsNoiseUtil *noise_util,
			       gpointer destination)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->destination = destination;
}

/**
 * ags_noise_util_get_destination_stride:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get destination stride of @noise_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 3.15.0
 */
guint
ags_noise_util_get_destination_stride(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(0);
  }

  return(noise_util->destination_stride);
}

/**
 * ags_noise_util_set_destination_stride:
 * @noise_util: the #AgsNoiseUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_destination_stride(AgsNoiseUtil *noise_util,
				      guint destination_stride)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->destination_stride = destination_stride;
}

/**
 * ags_noise_util_get_source:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get source buffer of @noise_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.15.0
 */
gpointer
ags_noise_util_get_source(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(NULL);
  }

  return(noise_util->source);
}

/**
 * ags_noise_util_set_source:
 * @noise_util: the #AgsNoiseUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_source(AgsNoiseUtil *noise_util,
			  gpointer source)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->source = source;
}

/**
 * ags_noise_util_get_source_stride:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get source stride of @noise_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.15.0
 */
guint
ags_noise_util_get_source_stride(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(0);
  }

  return(noise_util->source_stride);
}

/**
 * ags_noise_util_set_source_stride:
 * @noise_util: the #AgsNoiseUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_source_stride(AgsNoiseUtil *noise_util,
				 guint source_stride)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->source_stride = source_stride;
}

/**
 * ags_noise_util_get_buffer_length:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get buffer length of @noise_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.15.0
 */
guint
ags_noise_util_get_buffer_length(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(0);
  }

  return(noise_util->buffer_length);
}

/**
 * ags_noise_util_set_buffer_length:
 * @noise_util: the #AgsNoiseUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_buffer_length(AgsNoiseUtil *noise_util,
				 guint buffer_length)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->buffer_length = buffer_length;
}

/**
 * ags_noise_util_get_format:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get format of @noise_util.
 * 
 * Returns: the format
 * 
 * Since: 3.15.0
 */
guint
ags_noise_util_get_format(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(0);
  }

  return(noise_util->format);
}

/**
 * ags_noise_util_set_format:
 * @noise_util: the #AgsNoiseUtil-struct
 * @format: the format
 *
 * Set @format of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_format(AgsNoiseUtil *noise_util,
			  guint format)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->format = format;
}

/**
 * ags_noise_util_get_samplerate:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get samplerate of @noise_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.15.0
 */
guint
ags_noise_util_get_samplerate(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(0);
  }

  return(noise_util->samplerate);
}

/**
 * ags_noise_util_set_samplerate:
 * @noise_util: the #AgsNoiseUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_samplerate(AgsNoiseUtil *noise_util,
			      guint samplerate)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->samplerate = samplerate;
}

/**
 * ags_noise_util_get_mode:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get mode of @noise_util.
 * 
 * Returns: the mode
 * 
 * Since: 3.15.0
 */
guint
ags_noise_util_get_mode(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(0);
  }

  return(noise_util->mode);
}

/**
 * ags_noise_util_set_mode:
 * @noise_util: the #AgsNoiseUtil-struct
 * @mode: the mode
 *
 * Set @mode of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_mode(AgsNoiseUtil *noise_util,
			guint mode)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->mode = mode;
}

/**
 * ags_noise_util_get_volume:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get volume of @noise_util.
 * 
 * Returns: the volume
 * 
 * Since: 3.15.0
 */
gdouble
ags_noise_util_get_volume(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(0.0);
  }

  return(noise_util->volume);
}

/**
 * ags_noise_util_set_volume:
 * @noise_util: the #AgsNoiseUtil-struct
 * @volume: the volume
 *
 * Set @volume of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_volume(AgsNoiseUtil *noise_util,
			  gdouble volume)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->volume = volume;
}

/**
 * ags_noise_util_get_frequency:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get frequency of @noise_util.
 * 
 * Returns: the frequency
 * 
 * Since: 3.15.0
 */
gdouble
ags_noise_util_get_frequency(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(0.0);
  }

  return(noise_util->frequency);
}

/**
 * ags_noise_util_set_frequency:
 * @noise_util: the #AgsNoiseUtil-struct
 * @frequency: the frequency
 *
 * Set @frequency of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_frequency(AgsNoiseUtil *noise_util,
			     gdouble frequency)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->frequency = frequency;
}

/**
 * ags_noise_util_get_frame_count:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get frame count of @noise_util.
 * 
 * Returns: the frame count
 * 
 * Since: 3.15.0
 */
guint
ags_noise_util_get_frame_count(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(0);
  }

  return(noise_util->frame_count);
}

/**
 * ags_noise_util_set_frame_count:
 * @noise_util: the #AgsNoiseUtil-struct
 * @frame_count: the frame count
 *
 * Set @frame_count of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_frame_count(AgsNoiseUtil *noise_util,
			       guint frame_count)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->frame_count = frame_count;
}

/**
 * ags_noise_util_get_offset:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Get offset of @noise_util.
 * 
 * Returns: the offset
 * 
 * Since: 3.15.0
 */
guint
ags_noise_util_get_offset(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL){
    return(0);
  }

  return(noise_util->offset);
}

/**
 * ags_noise_util_set_offset:
 * @noise_util: the #AgsNoiseUtil-struct
 * @offset: the offset
 *
 * Set @offset of @noise_util.
 *
 * Since: 3.15.0
 */
void
ags_noise_util_set_offset(AgsNoiseUtil *noise_util,
			  guint offset)
{
  if(noise_util == NULL){
    return;
  }

  noise_util->offset = offset;
}

/**
 * ags_noise_util_compute_s8:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Compute noise of @noise_util of signed 8 bit data.
 *  
 * Since: 3.15.0
 */
void
ags_noise_util_compute_s8(AgsNoiseUtil *noise_util)
{
  gint8 *destination, *source;
  gint8 *noise;
  gdouble current_noise[8];
  
  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  guint mode;
  gdouble volume;
  gdouble frequency;    
  guint i, i_stop;
  gdouble last_value;
  guint counter;
  guint remain;
  guint jump_samples;
  guint j;
  guint k;
    
  const int n_generators = 8 * sizeof(guint);
  const gdouble scale = 127.0;
  
  if(noise_util == NULL ||
     noise_util->destination == NULL ||
     noise_util->source == NULL){
    return;
  }

  destination = noise_util->destination;
  destination_stride = noise_util->destination_stride;

  source = noise_util->source;
  source_stride = noise_util->source_stride;

  noise = noise_util->noise;

  buffer_length = noise_util->buffer_length;
  samplerate = noise_util->samplerate;

  mode = noise_util->mode;

  volume = noise_util->volume;

  frequency = noise_util->frequency;

  for(i = 0; i < 8; i++){
    current_noise[i] = 0.0;
  }

  i = 0;
  i_stop = buffer_length - (buffer_length % 8);
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      ags_v8double v_buffer;
      ags_v8double v_rand;

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

      v_rand = (ags_v8double) {      
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand()
      };

      v_rand /= (gdouble) RAND_MAX;
      v_rand *= 2.0;
      v_rand *= volume;
      v_rand -= volume;
      
      v_rand *= scale;
      
      v_buffer += v_rand;
      
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
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
    for(; i < i_stop;){
      ags_v8double v_buffer;
      ags_v8double v_noise;

      gint position;

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

      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = scale * (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = scale * (last_value / n_generators);
	}
      }
      
      v_noise = (ags_v8double) {
	(gdouble) *(current_noise),
	(gdouble) *(current_noise + 1),
	(gdouble) *(current_noise + 2),
	(gdouble) *(current_noise + 3),
	(gdouble) *(current_noise + 4),
	(gdouble) *(current_noise + 5),
	(gdouble) *(current_noise + 6),
	(gdouble) *(current_noise + 7)
      };
      
      v_buffer += v_noise;
      
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
  }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble noise[8 * sizeof(guint)];
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      double ret_v_buffer[8];
      double ret_v_rand[8];
      double tmp0_v_rand[8];
      
      double v_buffer[] = {
	(double) *(source),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride)};      
      
      double v_rand[] = {
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand()};

      double v_factor[] = { volume * 2.0 / (double) RAND_MAX};
      double v_volume[] = {(double) volume};

      source += source_stride;

      vDSP_vmulD(v_factor, 0, v_rand, 1, tmp0_v_rand, 1, 8);
      vDSP_vsubD(v_volume, 0, tmp0_v_rand, 1, ret_v_rand, 1, 8);

      vDSP_vaddD(v_buffer, 1, ret_v_rand, 1, ret_v_buffer, 1, 8);

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
  }    
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    double current_noise[8];
    
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
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

      source += source_stride;

      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = scale * (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = scale * (last_value / n_generators);
	}
      }

      vDSP_vaddD(v_buffer, 1, current_noise, 1, ret_v_buffer, 1, 8);
      
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
  }
  }
#else
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      *(destination) = ((gdouble) rand() * factor - volume) + source[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];

      destination += destination_stride;
      source += source_stride;
      i += 8;
    }
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
    for(; i < i_stop;){
      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = scale * (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = scale * (last_value / n_generators);
	}
      }

      *(destination) = current_noise[0] + source[0];
      *(destination += destination_stride) = current_noise[1] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[2] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[3] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[4] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[5] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[6] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[7] + (source += source_stride)[0];

      destination += destination_stride;
      source += source_stride;
      i += 8;
    }
  }
  }
#endif
  
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < buffer_length; i++){
      destination[0] = ((gdouble) rand() * factor - volume) + source[0];
      
      destination += destination_stride;
      source += source_stride;
    }
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    for(; i < buffer_length; i++){
      /* pink noise */
      if(frequency > 0.0){
	if(remain != 0){
	  jump_samples = (remain < counter) ? remain : counter;
	  
	  if(j < jump_samples){
	    current_noise[i % 8] = scale * (last_value / n_generators);
	    j++;
	  }

	  if(j == jump_samples){
	    counter -= jump_samples;
	    remain -= jump_samples;

	    if(counter == 0){
	      AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
	      counter = (guint) floor(samplerate / frequency);
	    }
	  }                  
	}
      }else{
	current_noise[i % 8] = scale * (last_value / n_generators);
      }

      *(destination) = current_noise[0] + source[0];
      
      destination += destination_stride;
      source += source_stride;
    }
  }
  }
}

/**
 * ags_noise_util_compute_s16:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Compute noise of @noise_util of signed 16 bit data.
 *  
 * Since: 3.15.0
 */
void
ags_noise_util_compute_s16(AgsNoiseUtil *noise_util)
{
  gint16 *destination, *source;
  gint16 *noise;
  gdouble current_noise[8];
  
  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  guint mode;
  gdouble volume;
  gdouble frequency;    
  guint i, i_stop;
  gdouble last_value;
  guint counter;
  guint remain;
  guint jump_samples;
  guint j;
  guint k;
    
  const int n_generators = 8 * sizeof(guint);
  const gdouble scale = 32767.0;
  
  if(noise_util == NULL ||
     noise_util->destination == NULL ||
     noise_util->source == NULL){
    return;
  }

  destination = noise_util->destination;
  destination_stride = noise_util->destination_stride;

  source = noise_util->source;
  source_stride = noise_util->source_stride;

  noise = noise_util->noise;

  buffer_length = noise_util->buffer_length;
  samplerate = noise_util->samplerate;

  mode = noise_util->mode;

  volume = noise_util->volume;

  frequency = noise_util->frequency;

  for(i = 0; i < 8; i++){
    current_noise[i] = 0.0;
  }

  i = 0;
  i_stop = buffer_length - (buffer_length % 8);
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      ags_v8double v_buffer;
      ags_v8double v_rand;

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

      v_rand = (ags_v8double) {      
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand()
      };

      v_rand /= (gdouble) RAND_MAX;
      v_rand *= 2.0;
      v_rand *= volume;
      v_rand -= volume;
      
      v_rand *= scale;
      
      v_buffer += v_rand;
      
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
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
    for(; i < i_stop;){
      ags_v8double v_buffer;
      ags_v8double v_noise;

      gint position;

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

      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = scale * (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = scale * (last_value / n_generators);
	}
      }
      
      v_noise = (ags_v8double) {
	(gdouble) *(current_noise),
	(gdouble) *(current_noise + 1),
	(gdouble) *(current_noise + 2),
	(gdouble) *(current_noise + 3),
	(gdouble) *(current_noise + 4),
	(gdouble) *(current_noise + 5),
	(gdouble) *(current_noise + 6),
	(gdouble) *(current_noise + 7)
      };
      
      v_buffer += v_noise;
      
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
  }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble noise[8 * sizeof(guint)];
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      double ret_v_buffer[8];
      double ret_v_rand[8];
      double tmp0_v_rand[8];
      
      double v_buffer[] = {
	(double) *(source),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride)};      
      
      double v_rand[] = {
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand()};

      double v_factor[] = { volume * 2.0 / (double) RAND_MAX};
      double v_volume[] = {(double) volume};

      source += source_stride;

      vDSP_vmulD(v_factor, 0, v_rand, 1, tmp0_v_rand, 1, 8);
      vDSP_vsubD(v_volume, 0, tmp0_v_rand, 1, ret_v_rand, 1, 8);

      vDSP_vaddD(v_buffer, 1, ret_v_rand, 1, ret_v_buffer, 1, 8);

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
  }    
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    double current_noise[8];
    
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
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

      source += source_stride;

      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = scale * (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = scale * (last_value / n_generators);
	}
      }

      vDSP_vaddD(v_buffer, 1, current_noise, 1, ret_v_buffer, 1, 8);
      
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
  }
  }
#else
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      *(destination) = ((gdouble) rand() * factor - volume) + source[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];

      destination += destination_stride;
      source += source_stride;
      i += 8;
    }
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
    for(; i < i_stop;){
      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = scale * (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = scale * (last_value / n_generators);
	}
      }

      *(destination) = current_noise[0] + source[0];
      *(destination += destination_stride) = current_noise[1] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[2] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[3] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[4] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[5] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[6] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[7] + (source += source_stride)[0];

      destination += destination_stride;
      source += source_stride;
      i += 8;
    }
  }
  }
#endif
  
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < buffer_length; i++){
      destination[0] = ((gdouble) rand() * factor - volume) + source[0];
      
      destination += destination_stride;
      source += source_stride;
    }
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    for(; i < buffer_length; i++){
      /* pink noise */
      if(frequency > 0.0){
	if(remain != 0){
	  jump_samples = (remain < counter) ? remain : counter;
	  
	  if(j < jump_samples){
	    current_noise[i % 8] = scale * (last_value / n_generators);
	    j++;
	  }

	  if(j == jump_samples){
	    counter -= jump_samples;
	    remain -= jump_samples;

	    if(counter == 0){
	      AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
	      counter = (guint) floor(samplerate / frequency);
	    }
	  }                  
	}
      }else{
	current_noise[i % 8] = scale * (last_value / n_generators);
      }

      *(destination) = current_noise[0] + source[0];
      
      destination += destination_stride;
      source += source_stride;
    }
  }
  }
}

/**
 * ags_noise_util_compute_s24:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Compute noise of @noise_util of signed 24 bit data.
 *  
 * Since: 3.15.0
 */
void
ags_noise_util_compute_s24(AgsNoiseUtil *noise_util)
{
  gint32 *destination, *source;
  gint32 *noise;
  gdouble current_noise[8];
  
  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  guint mode;
  gdouble volume;
  gdouble frequency;    
  guint i, i_stop;
  gdouble last_value;
  guint counter;
  guint remain;
  guint jump_samples;
  guint j;
  guint k;
    
  const int n_generators = 8 * sizeof(guint);
  const gdouble scale = 8388607.0;
  
  if(noise_util == NULL ||
     noise_util->destination == NULL ||
     noise_util->source == NULL){
    return;
  }

  destination = noise_util->destination;
  destination_stride = noise_util->destination_stride;

  source = noise_util->source;
  source_stride = noise_util->source_stride;

  noise = noise_util->noise;

  buffer_length = noise_util->buffer_length;
  samplerate = noise_util->samplerate;

  mode = noise_util->mode;

  volume = noise_util->volume;

  frequency = noise_util->frequency;

  for(i = 0; i < 8; i++){
    current_noise[i] = 0.0;
  }

  i = 0;
  i_stop = buffer_length - (buffer_length % 8);
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      ags_v8double v_buffer;
      ags_v8double v_rand;

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

      v_rand = (ags_v8double) {      
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand()
      };

      v_rand /= (gdouble) RAND_MAX;
      v_rand *= 2.0;
      v_rand *= volume;
      v_rand -= volume;
      
      v_rand *= scale;
      
      v_buffer += v_rand;
      
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
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
    for(; i < i_stop;){
      ags_v8double v_buffer;
      ags_v8double v_noise;

      gint position;

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

      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = scale * (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = scale * (last_value / n_generators);
	}
      }
      
      v_noise = (ags_v8double) {
	(gdouble) *(current_noise),
	(gdouble) *(current_noise + 1),
	(gdouble) *(current_noise + 2),
	(gdouble) *(current_noise + 3),
	(gdouble) *(current_noise + 4),
	(gdouble) *(current_noise + 5),
	(gdouble) *(current_noise + 6),
	(gdouble) *(current_noise + 7)
      };
      
      v_buffer += v_noise;
      
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
  }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble noise[8 * sizeof(guint)];
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      double ret_v_buffer[8];
      double ret_v_rand[8];
      double tmp0_v_rand[8];
      
      double v_buffer[] = {
	(double) *(source),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride)};      
      
      double v_rand[] = {
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand()};

      double v_factor[] = { volume * 2.0 / (double) RAND_MAX};
      double v_volume[] = {(double) volume};

      source += source_stride;

      vDSP_vmulD(v_factor, 0, v_rand, 1, tmp0_v_rand, 1, 8);
      vDSP_vsubD(v_volume, 0, tmp0_v_rand, 1, ret_v_rand, 1, 8);

      vDSP_vaddD(v_buffer, 1, ret_v_rand, 1, ret_v_buffer, 1, 8);

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
  }    
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    double current_noise[8];
    
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
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

      source += source_stride;

      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = scale * (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = scale * (last_value / n_generators);
	}
      }

      vDSP_vaddD(v_buffer, 1, current_noise, 1, ret_v_buffer, 1, 8);
      
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
  }
  }
#else
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      *(destination) = ((gdouble) rand() * factor - volume) + source[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];

      destination += destination_stride;
      source += source_stride;
      i += 8;
    }
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
    for(; i < i_stop;){
      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = scale * (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = scale * (last_value / n_generators);
	}
      }

      *(destination) = current_noise[0] + source[0];
      *(destination += destination_stride) = current_noise[1] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[2] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[3] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[4] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[5] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[6] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[7] + (source += source_stride)[0];

      destination += destination_stride;
      source += source_stride;
      i += 8;
    }
  }
  }
#endif
  
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < buffer_length; i++){
      destination[0] = ((gdouble) rand() * factor - volume) + source[0];
      
      destination += destination_stride;
      source += source_stride;
    }
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    for(; i < buffer_length; i++){
      /* pink noise */
      if(frequency > 0.0){
	if(remain != 0){
	  jump_samples = (remain < counter) ? remain : counter;
	  
	  if(j < jump_samples){
	    current_noise[i % 8] = scale * (last_value / n_generators);
	    j++;
	  }

	  if(j == jump_samples){
	    counter -= jump_samples;
	    remain -= jump_samples;

	    if(counter == 0){
	      AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
	      counter = (guint) floor(samplerate / frequency);
	    }
	  }                  
	}
      }else{
	current_noise[i % 8] = scale * (last_value / n_generators);
      }

      *(destination) = current_noise[0] + source[0];
      
      destination += destination_stride;
      source += source_stride;
    }
  }
  }
}

/**
 * ags_noise_util_compute_s32:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Compute noise of @noise_util of signed 32 bit data.
 *  
 * Since: 3.15.0
 */
void
ags_noise_util_compute_s32(AgsNoiseUtil *noise_util)
{
  gint32 *destination, *source;
  gint32 *noise;
  gdouble current_noise[8];
  
  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  guint mode;
  gdouble volume;
  gdouble frequency;    
  guint i, i_stop;
  gdouble last_value;
  guint counter;
  guint remain;
  guint jump_samples;
  guint j;
  guint k;
    
  const int n_generators = 8 * sizeof(guint);
  const gdouble scale = 214748363.0;
  
  if(noise_util == NULL ||
     noise_util->destination == NULL ||
     noise_util->source == NULL){
    return;
  }

  destination = noise_util->destination;
  destination_stride = noise_util->destination_stride;

  source = noise_util->source;
  source_stride = noise_util->source_stride;

  noise = noise_util->noise;

  buffer_length = noise_util->buffer_length;
  samplerate = noise_util->samplerate;

  mode = noise_util->mode;

  volume = noise_util->volume;

  frequency = noise_util->frequency;

  for(i = 0; i < 8; i++){
    current_noise[i] = 0.0;
  }

  i = 0;
  i_stop = buffer_length - (buffer_length % 8);
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      ags_v8double v_buffer;
      ags_v8double v_rand;

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

      v_rand = (ags_v8double) {      
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand()
      };

      v_rand /= (gdouble) RAND_MAX;
      v_rand *= 2.0;
      v_rand *= volume;
      v_rand -= volume;
      
      v_rand *= scale;
      
      v_buffer += v_rand;
      
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
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
    for(; i < i_stop;){
      ags_v8double v_buffer;
      ags_v8double v_noise;

      gint position;

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

      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = scale * (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = scale * (last_value / n_generators);
	}
      }
      
      v_noise = (ags_v8double) {
	(gdouble) *(current_noise),
	(gdouble) *(current_noise + 1),
	(gdouble) *(current_noise + 2),
	(gdouble) *(current_noise + 3),
	(gdouble) *(current_noise + 4),
	(gdouble) *(current_noise + 5),
	(gdouble) *(current_noise + 6),
	(gdouble) *(current_noise + 7)
      };
      
      v_buffer += v_noise;
      
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
  }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble noise[8 * sizeof(guint)];
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      double ret_v_buffer[8];
      double ret_v_rand[8];
      double tmp0_v_rand[8];
      
      double v_buffer[] = {
	(double) *(source),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride)};      
      
      double v_rand[] = {
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand()};

      double v_factor[] = { volume * 2.0 / (double) RAND_MAX};
      double v_volume[] = {(double) volume};

      source += source_stride;

      vDSP_vmulD(v_factor, 0, v_rand, 1, tmp0_v_rand, 1, 8);
      vDSP_vsubD(v_volume, 0, tmp0_v_rand, 1, ret_v_rand, 1, 8);

      vDSP_vaddD(v_buffer, 1, ret_v_rand, 1, ret_v_buffer, 1, 8);

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
  }    
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    double current_noise[8];
    
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
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

      source += source_stride;

      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = scale * (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = scale * (last_value / n_generators);
	}
      }

      vDSP_vaddD(v_buffer, 1, current_noise, 1, ret_v_buffer, 1, 8);
      
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
  }
  }
#else
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      *(destination) = ((gdouble) rand() * factor - volume) + source[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];

      destination += destination_stride;
      source += source_stride;
      i += 8;
    }
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
    for(; i < i_stop;){
      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = scale * (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = scale * (last_value / n_generators);
	}
      }

      *(destination) = current_noise[0] + source[0];
      *(destination += destination_stride) = current_noise[1] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[2] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[3] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[4] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[5] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[6] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[7] + (source += source_stride)[0];

      destination += destination_stride;
      source += source_stride;
      i += 8;
    }
  }
  }
#endif
  
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < buffer_length; i++){
      destination[0] = ((gdouble) rand() * factor - volume) + source[0];
      
      destination += destination_stride;
      source += source_stride;
    }
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    for(; i < buffer_length; i++){
      /* pink noise */
      if(frequency > 0.0){
	if(remain != 0){
	  jump_samples = (remain < counter) ? remain : counter;
	  
	  if(j < jump_samples){
	    current_noise[i % 8] = scale * (last_value / n_generators);
	    j++;
	  }

	  if(j == jump_samples){
	    counter -= jump_samples;
	    remain -= jump_samples;

	    if(counter == 0){
	      AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
	      counter = (guint) floor(samplerate / frequency);
	    }
	  }                  
	}
      }else{
	current_noise[i % 8] = scale * (last_value / n_generators);
      }

      *(destination) = current_noise[0] + source[0];
      
      destination += destination_stride;
      source += source_stride;
    }
  }
  }
}

/**
 * ags_noise_util_compute_s64:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Compute noise of @noise_util of signed 64 bit data.
 *  
 * Since: 3.15.0
 */
void
ags_noise_util_compute_s64(AgsNoiseUtil *noise_util)
{
  gint64 *destination, *source;
  gint64 *noise;
  gdouble current_noise[8];
  
  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  guint mode;
  gdouble volume;
  gdouble frequency;    
  guint i, i_stop;
  gdouble last_value;
  guint counter;
  guint remain;
  guint jump_samples;
  guint j;
  guint k;
    
  const int n_generators = 8 * sizeof(guint);
  const gdouble scale = 9223372036854775807.0;
  
  if(noise_util == NULL ||
     noise_util->destination == NULL ||
     noise_util->source == NULL){
    return;
  }

  destination = noise_util->destination;
  destination_stride = noise_util->destination_stride;

  source = noise_util->source;
  source_stride = noise_util->source_stride;

  noise = noise_util->noise;

  buffer_length = noise_util->buffer_length;
  samplerate = noise_util->samplerate;

  mode = noise_util->mode;

  volume = noise_util->volume;

  frequency = noise_util->frequency;

  for(i = 0; i < 8; i++){
    current_noise[i] = 0.0;
  }

  i = 0;
  i_stop = buffer_length - (buffer_length % 8);
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      ags_v8double v_buffer;
      ags_v8double v_rand;

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

      v_rand = (ags_v8double) {      
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand()
      };

      v_rand /= (gdouble) RAND_MAX;
      v_rand *= 2.0;
      v_rand *= volume;
      v_rand -= volume;
      
      v_rand *= scale;
      
      v_buffer += v_rand;
      
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
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
    for(; i < i_stop;){
      ags_v8double v_buffer;
      ags_v8double v_noise;

      gint position;

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

      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = scale * (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = scale * (last_value / n_generators);
	}
      }
      
      v_noise = (ags_v8double) {
	(gdouble) *(current_noise),
	(gdouble) *(current_noise + 1),
	(gdouble) *(current_noise + 2),
	(gdouble) *(current_noise + 3),
	(gdouble) *(current_noise + 4),
	(gdouble) *(current_noise + 5),
	(gdouble) *(current_noise + 6),
	(gdouble) *(current_noise + 7)
      };
      
      v_buffer += v_noise;
      
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
  }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble noise[8 * sizeof(guint)];
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      double ret_v_buffer[8];
      double ret_v_rand[8];
      double tmp0_v_rand[8];
      
      double v_buffer[] = {
	(double) *(source),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride)};      
      
      double v_rand[] = {
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand()};

      double v_factor[] = { volume * 2.0 / (double) RAND_MAX};
      double v_volume[] = {(double) volume};

      source += source_stride;

      vDSP_vmulD(v_factor, 0, v_rand, 1, tmp0_v_rand, 1, 8);
      vDSP_vsubD(v_volume, 0, tmp0_v_rand, 1, ret_v_rand, 1, 8);

      vDSP_vaddD(v_buffer, 1, ret_v_rand, 1, ret_v_buffer, 1, 8);

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
  }    
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    double current_noise[8];
    
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
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

      source += source_stride;

      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = scale * (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = scale * (last_value / n_generators);
	}
      }

      vDSP_vaddD(v_buffer, 1, current_noise, 1, ret_v_buffer, 1, 8);
      
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
  }
  }
#else
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      *(destination) = ((gdouble) rand() * factor - volume) + source[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];

      destination += destination_stride;
      source += source_stride;
      i += 8;
    }
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
    for(; i < i_stop;){
      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = scale * (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = scale * (last_value / n_generators);
	}
      }

      *(destination) = current_noise[0] + source[0];
      *(destination += destination_stride) = current_noise[1] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[2] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[3] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[4] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[5] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[6] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[7] + (source += source_stride)[0];

      destination += destination_stride;
      source += source_stride;
      i += 8;
    }
  }
  }
#endif
  
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < buffer_length; i++){
      destination[0] = ((gdouble) rand() * factor - volume) + source[0];
      
      destination += destination_stride;
      source += source_stride;
    }
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    for(; i < buffer_length; i++){
      /* pink noise */
      if(frequency > 0.0){
	if(remain != 0){
	  jump_samples = (remain < counter) ? remain : counter;
	  
	  if(j < jump_samples){
	    current_noise[i % 8] = scale * (last_value / n_generators);
	    j++;
	  }

	  if(j == jump_samples){
	    counter -= jump_samples;
	    remain -= jump_samples;

	    if(counter == 0){
	      AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
	      counter = (guint) floor(samplerate / frequency);
	    }
	  }                  
	}
      }else{
	current_noise[i % 8] = scale * (last_value / n_generators);
      }

      *(destination) = current_noise[0] + source[0];
      
      destination += destination_stride;
      source += source_stride;
    }
  }
  }
}

/**
 * ags_noise_util_compute_float:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Compute noise of @noise_util of floating point data.
 *  
 * Since: 3.15.0
 */
void
ags_noise_util_compute_float(AgsNoiseUtil *noise_util)
{
  gfloat *destination, *source;
  gfloat *noise;
  gdouble current_noise[8];
  
  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  guint mode;
  gdouble volume;
  gdouble frequency;    
  guint i, i_stop;
  gdouble last_value;
  guint counter;
  guint remain;
  guint jump_samples;
  guint j;
  guint k;
    
  const int n_generators = 8 * sizeof(guint);
  
  if(noise_util == NULL ||
     noise_util->destination == NULL ||
     noise_util->source == NULL){
    return;
  }

  destination = noise_util->destination;
  destination_stride = noise_util->destination_stride;

  source = noise_util->source;
  source_stride = noise_util->source_stride;

  noise = noise_util->noise;

  buffer_length = noise_util->buffer_length;
  samplerate = noise_util->samplerate;

  mode = noise_util->mode;

  volume = noise_util->volume;

  frequency = noise_util->frequency;

  for(i = 0; i < 8; i++){
    current_noise[i] = 0.0;
  }

  i = 0;
  i_stop = buffer_length - (buffer_length % 8);
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      ags_v8double v_buffer;
      ags_v8double v_rand;

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

      v_rand = (ags_v8double) {      
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand()
      };

      v_rand /= (gdouble) RAND_MAX;
      v_rand *= 2.0;
      v_rand *= volume;
      v_rand -= volume;
      
      v_buffer += v_rand;
      
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
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
    for(; i < i_stop;){
      ags_v8double v_buffer;
      ags_v8double v_noise;

      gint position;

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

      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = (last_value / n_generators);
	}
      }
      
      v_noise = (ags_v8double) {
	(gdouble) *(current_noise),
	(gdouble) *(current_noise + 1),
	(gdouble) *(current_noise + 2),
	(gdouble) *(current_noise + 3),
	(gdouble) *(current_noise + 4),
	(gdouble) *(current_noise + 5),
	(gdouble) *(current_noise + 6),
	(gdouble) *(current_noise + 7)
      };
      
      v_buffer += v_noise;
      
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
  }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble noise[8 * sizeof(guint)];
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      double ret_v_buffer[8];
      double ret_v_rand[8];
      double tmp0_v_rand[8];
      
      double v_buffer[] = {
	(double) *(source),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride)};      
      
      double v_rand[] = {
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand()};

      double v_factor[] = { volume * 2.0 / (double) RAND_MAX};
      double v_volume[] = {(double) volume};

      source += source_stride;

      vDSP_vmulD(v_factor, 0, v_rand, 1, tmp0_v_rand, 1, 8);
      vDSP_vsubD(v_volume, 0, tmp0_v_rand, 1, ret_v_rand, 1, 8);

      vDSP_vaddD(v_buffer, 1, ret_v_rand, 1, ret_v_buffer, 1, 8);

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
  }    
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    double current_noise[8];
    
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
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

      source += source_stride;

      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = (last_value / n_generators);
	}
      }

      vDSP_vaddD(v_buffer, 1, current_noise, 1, ret_v_buffer, 1, 8);
      
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
  }
  }
#else
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      *(destination) = ((gdouble) rand() * factor - volume) + source[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];

      destination += destination_stride;
      source += source_stride;
      i += 8;
    }
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
    for(; i < i_stop;){
      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = (last_value / n_generators);
	}
      }

      *(destination) = current_noise[0] + source[0];
      *(destination += destination_stride) = current_noise[1] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[2] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[3] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[4] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[5] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[6] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[7] + (source += source_stride)[0];

      destination += destination_stride;
      source += source_stride;
      i += 8;
    }
  }
  }
#endif
  
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < buffer_length; i++){
      destination[0] = ((gdouble) rand() * factor - volume) + source[0];
      
      destination += destination_stride;
      source += source_stride;
    }
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    for(; i < buffer_length; i++){
      /* pink noise */
      if(frequency > 0.0){
	if(remain != 0){
	  jump_samples = (remain < counter) ? remain : counter;
	  
	  if(j < jump_samples){
	    current_noise[i % 8] = (last_value / n_generators);
	    j++;
	  }

	  if(j == jump_samples){
	    counter -= jump_samples;
	    remain -= jump_samples;

	    if(counter == 0){
	      AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
	      counter = (guint) floor(samplerate / frequency);
	    }
	  }                  
	}
      }else{
	current_noise[i % 8] = (last_value / n_generators);
      }

      *(destination) = current_noise[0] + source[0];
      
      destination += destination_stride;
      source += source_stride;
    }
  }
  }
}

/**
 * ags_noise_util_compute_double:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Compute noise of @noise_util of double precision floating point data.
 *  
 * Since: 3.15.0
 */
void
ags_noise_util_compute_double(AgsNoiseUtil *noise_util)
{
  gdouble *destination, *source;
  gdouble *noise;
  gdouble current_noise[8];
  
  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  guint mode;
  gdouble volume;
  gdouble frequency;    
  guint i, i_stop;
  gdouble last_value;
  guint counter;
  guint remain;
  guint jump_samples;
  guint j;
  guint k;
    
  const int n_generators = 8 * sizeof(guint);
  
  if(noise_util == NULL ||
     noise_util->destination == NULL ||
     noise_util->source == NULL){
    return;
  }

  destination = noise_util->destination;
  destination_stride = noise_util->destination_stride;

  source = noise_util->source;
  source_stride = noise_util->source_stride;

  noise = noise_util->noise;

  buffer_length = noise_util->buffer_length;
  samplerate = noise_util->samplerate;

  mode = noise_util->mode;

  volume = noise_util->volume;

  frequency = noise_util->frequency;

  for(i = 0; i < 8; i++){
    current_noise[i] = 0.0;
  }

  i = 0;
  i_stop = buffer_length - (buffer_length % 8);
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      ags_v8double v_buffer;
      ags_v8double v_rand;

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

      v_rand = (ags_v8double) {      
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand(),
	(gdouble) rand()
      };

      v_rand /= (gdouble) RAND_MAX;
      v_rand *= 2.0;
      v_rand *= volume;
      v_rand -= volume;
      
      v_buffer += v_rand;
      
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
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
    for(; i < i_stop;){
      ags_v8double v_buffer;
      ags_v8double v_noise;

      gint position;

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

      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = (last_value / n_generators);
	}
      }
      
      v_noise = (ags_v8double) {
	(gdouble) *(current_noise),
	(gdouble) *(current_noise + 1),
	(gdouble) *(current_noise + 2),
	(gdouble) *(current_noise + 3),
	(gdouble) *(current_noise + 4),
	(gdouble) *(current_noise + 5),
	(gdouble) *(current_noise + 6),
	(gdouble) *(current_noise + 7)
      };
      
      v_buffer += v_noise;
      
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
  }
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble noise[8 * sizeof(guint)];
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      double ret_v_buffer[8];
      double ret_v_rand[8];
      double tmp0_v_rand[8];
      
      double v_buffer[] = {
	(double) *(source),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride),
	(double) *(source += source_stride)};      
      
      double v_rand[] = {
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand(),
	(double) rand()};

      double v_factor[] = { volume * 2.0 / (double) RAND_MAX};
      double v_volume[] = {(double) volume};

      source += source_stride;

      vDSP_vmulD(v_factor, 0, v_rand, 1, tmp0_v_rand, 1, 8);
      vDSP_vsubD(v_volume, 0, tmp0_v_rand, 1, ret_v_rand, 1, 8);

      vDSP_vaddD(v_buffer, 1, ret_v_rand, 1, ret_v_buffer, 1, 8);

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
  }    
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    double current_noise[8];
    
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
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

      source += source_stride;

      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = (last_value / n_generators);
	}
      }

      vDSP_vaddD(v_buffer, 1, current_noise, 1, ret_v_buffer, 1, 8);
      
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
  }
  }
#else
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < i_stop;){
      *(destination) = ((gdouble) rand() * factor - volume) + source[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];
      *(destination += destination_stride) = ((gdouble) rand() * factor - volume) + (source += source_stride)[0];

      destination += destination_stride;
      source += source_stride;
      i += 8;
    }
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
    for(; i < i_stop;){
      /* pink noise */
      for(k = 0; k < 8; k++){
	if(frequency > 0.0){
	  if(remain != 0){
	    jump_samples = (remain < counter) ? remain : counter;
	  
	    if(j < jump_samples){
	      current_noise[k] = (last_value / n_generators);
	      j++;
	    }

	    if(j == jump_samples){
	      counter -= jump_samples;
	      remain -= jump_samples;

	      if(counter == 0){
		AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
		counter = (guint) floor(samplerate / frequency);
	      }
	    }                  
	  }
	}else{
	  current_noise[k] = (last_value / n_generators);
	}
      }

      *(destination) = current_noise[0] + source[0];
      *(destination += destination_stride) = current_noise[1] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[2] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[3] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[4] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[5] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[6] + (source += source_stride)[0];
      *(destination += destination_stride) = current_noise[7] + (source += source_stride)[0];

      destination += destination_stride;
      source += source_stride;
      i += 8;
    }
  }
  }
#endif
  
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < buffer_length; i++){
      destination[0] = ((gdouble) rand() * factor - volume) + source[0];
      
      destination += destination_stride;
      source += source_stride;
    }
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    for(; i < buffer_length; i++){
      /* pink noise */
      if(frequency > 0.0){
	if(remain != 0){
	  jump_samples = (remain < counter) ? remain : counter;
	  
	  if(j < jump_samples){
	    current_noise[i % 8] = (last_value / n_generators);
	    j++;
	  }

	  if(j == jump_samples){
	    counter -= jump_samples;
	    remain -= jump_samples;

	    if(counter == 0){
	      AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
	      counter = (guint) floor(samplerate / frequency);
	    }
	  }                  
	}
      }else{
	current_noise[i % 8] = (last_value / n_generators);
      }

      *(destination) = current_noise[0] + source[0];
      
      destination += destination_stride;
      source += source_stride;
    }
  }
  }
}

/**
 * ags_noise_util_compute_complex:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Compute noise of @noise_util of complex data.
 *  
 * Since: 3.15.0
 */
void
ags_noise_util_compute_complex(AgsNoiseUtil *noise_util)
{
  AgsComplex *destination, *source;
  AgsComplex *noise;
  gdouble current_noise[8];
  
  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  guint mode;
  gdouble volume;
  gdouble frequency;    
  guint i, i_stop;
  gdouble last_value;
  guint counter;
  guint remain;
  guint jump_samples;
  guint j;
  guint k;
    
  const int n_generators = 8 * sizeof(guint);
  
  if(noise_util == NULL ||
     noise_util->destination == NULL ||
     noise_util->source == NULL){
    return;
  }

  destination = noise_util->destination;
  destination_stride = noise_util->destination_stride;

  source = noise_util->source;
  source_stride = noise_util->source_stride;

  noise = noise_util->noise;

  buffer_length = noise_util->buffer_length;
  samplerate = noise_util->samplerate;

  mode = noise_util->mode;

  volume = noise_util->volume;

  frequency = noise_util->frequency;

  for(i = 0; i < 8; i++){
    current_noise[i] = 0.0;
  }
  
  i = 0;
  i_stop = buffer_length - (buffer_length % 8);
  
  switch(mode){
  case AGS_NOISE_UTIL_WHITE_NOISE:
  {
    gdouble factor;

    factor = volume * 2.0 / (gdouble) RAND_MAX;
    
    for(; i < buffer_length; i++){
      ags_complex_set(destination, ((gdouble) rand() * factor - volume) + ags_complex_get(source));
      
      destination += destination_stride;
      source += source_stride;
    }
  }
  break;
  case AGS_NOISE_UTIL_PINK_NOISE:
  {
    frequency = (frequency <= (gdouble) samplerate) ? frequency : (gdouble) samplerate;
  
    /* compute noise */
    AGS_NOISE_UTIL_RESET(current_noise, (&last_value));

    j = 0;
    
    counter = 0;
    remain = buffer_length;
    
    for(; i < buffer_length; i++){
      /* pink noise */
      if(frequency > 0.0){
	if(remain != 0){
	  jump_samples = (remain < counter) ? remain : counter;
	  
	  if(j < jump_samples){
	    current_noise[i % 8] = (last_value / n_generators);
	    j++;
	  }

	  if(j == jump_samples){
	    counter -= jump_samples;
	    remain -= jump_samples;

	    if(counter == 0){
	      AGS_NOISE_UTIL_GET_VALUE((&last_value), current_noise, (&counter), (&last_value));
	      
	      counter = (guint) floor(samplerate / frequency);
	    }
	  }                  
	}
      }else{
	current_noise[i % 8] = (last_value / n_generators);
      }

      ags_complex_set(destination, current_noise[i % 8] + ags_complex_get(source));
      
      destination += destination_stride;
      source += source_stride;
    }
  }
  }
}

/**
 * ags_noise_util_compute:
 * @noise_util: the #AgsNoiseUtil-struct
 * 
 * Compute noise of @noise_util.
 *  
 * Since: 3.15.0
 */
void
ags_noise_util_compute(AgsNoiseUtil *noise_util)
{
  if(noise_util == NULL ||
     noise_util->destination == NULL ||
     noise_util->source == NULL){
    return;
  }

  switch(noise_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_noise_util_compute_s8(noise_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_noise_util_compute_s16(noise_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_noise_util_compute_s24(noise_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_noise_util_compute_s32(noise_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_noise_util_compute_s64(noise_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_noise_util_compute_float(noise_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_noise_util_compute_double(noise_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_noise_util_compute_complex(noise_util);
  }
  break;
  }
}
