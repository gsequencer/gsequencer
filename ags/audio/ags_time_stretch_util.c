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

#include <ags/audio/ags_time_stretch_util.h>

/**
 * SECTION:ags_time_stretch_util
 * @short_description: time stretch util
 * @title: AgsTimeStretchUtil
 * @section_id:
 * @include: ags/audio/ags_time_stretch_util.h
 *
 * Utility functions to stretch time.
 */

GType
ags_time_stretch_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_time_stretch_util = 0;

    ags_type_time_stretch_util =
      g_boxed_type_register_static("AgsTimeStretchUtil",
				   (GBoxedCopyFunc) ags_time_stretch_util_copy,
				   (GBoxedFreeFunc) ags_time_stretch_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_time_stretch_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_time_stretch_util_alloc:
 * 
 * Allocate #AgsTimeStretchUtil-struct.
 * 
 * Returns: the newly allocated #AgsTimeStretchUtil-struct
 * 
 * Since: 3.13.0
 */
AgsTimeStretchUtil*
ags_time_stretch_util_alloc()
{
  AgsTimeStretchUtil *ptr;
  
  ptr = (AgsTimeStretchUtil *) g_new(AgsTimeStretchUtil,
				     1);

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->destination = NULL;
  ptr->destination_stride = 1;
  
  ptr->buffer_length = 0;
  ptr->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  ptr->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  ptr->frequency = AGS_TIME_STRETCH_UTIL_DEFAULT_FREQUENCY;
  
  ptr->orig_bpm = 120.0;
  ptr->new_bpm = 120.0;

  return(ptr);
}

/**
 * ags_time_stretch_util_copy:
 * @ptr: the original #AgsTimeStretchUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsTimeStretchUtil-struct
 *
 * Since: 3.13.0
 */
gpointer
ags_time_stretch_util_copy(AgsTimeStretchUtil *ptr)
{
  AgsTimeStretchUtil *new_ptr;
  
  new_ptr = (AgsTimeStretchUtil *) g_new(AgsTimeStretchUtil,
					 1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;
  
  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->frequency = ptr->frequency;

  new_ptr->orig_bpm = ptr->orig_bpm;
  new_ptr->new_bpm = ptr->new_bpm;

  return(new_ptr);
}

/**
 * ags_time_stretch_util_free:
 * @ptr: the #AgsTimeStretchUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.13.0
 */
void
ags_time_stretch_util_free(AgsTimeStretchUtil *ptr)
{
  g_free(ptr->destination);

  if(ptr->destination != ptr->source){
    g_free(ptr->source);
  }
  
  g_free(ptr);
}

/**
 * ags_time_stretch_util_get_source:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Get source buffer of @time_stretch_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.13.0
 */
gpointer
ags_time_stretch_util_get_source(AgsTimeStretchUtil *time_stretch_util)
{
  if(time_stretch_util == NULL){
    return(NULL);
  }

  return(time_stretch_util->source);
}

/**
 * ags_time_stretch_util_set_source:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @time_stretch_util.
 *
 * Since: 3.13.0
 */
void
ags_time_stretch_util_set_source(AgsTimeStretchUtil *time_stretch_util,
				 gpointer source)
{
  if(time_stretch_util == NULL){
    return;
  }

  time_stretch_util->source = source;
}

/**
 * ags_time_stretch_util_get_source_stride:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Get source stride of @time_stretch_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.13.0
 */
guint
ags_time_stretch_util_get_source_stride(AgsTimeStretchUtil *time_stretch_util)
{
  if(time_stretch_util == NULL){
    return(0);
  }

  return(time_stretch_util->source_stride);
}

/**
 * ags_time_stretch_util_set_source_stride:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @time_stretch_util.
 *
 * Since: 3.13.0
 */
void
ags_time_stretch_util_set_source_stride(AgsTimeStretchUtil *time_stretch_util,
					guint source_stride)
{
  if(time_stretch_util == NULL){
    return;
  }

  time_stretch_util->source_stride = source_stride;
}

/**
 * ags_time_stretch_util_get_source_buffer_length:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Get source buffer length of @time_stretch_util.
 * 
 * Returns: the source buffer length
 * 
 * Since: 3.13.0
 */
guint
ags_time_stretch_util_get_source_buffer_length(AgsTimeStretchUtil *time_stretch_util)
{
  if(time_stretch_util == NULL){
    return(0);
  }

  return(time_stretch_util->source_buffer_length);
}

/**
 * ags_time_stretch_util_set_source_buffer_length:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * @source_buffer_length: the buffer length
 *
 * Set @source_buffer_length of @time_stretch_util.
 *
 * Since: 3.13.0
 */
void
ags_time_stretch_util_set_source_buffer_length(AgsTimeStretchUtil *time_stretch_util,
					       guint source_buffer_length)
{
  if(time_stretch_util == NULL){
    return;
  }

  time_stretch_util->source_buffer_length = source_buffer_length;
}

/**
 * ags_time_stretch_util_get_destination:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Get destination buffer of @time_stretch_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 3.13.0
 */
gpointer
ags_time_stretch_util_get_destination(AgsTimeStretchUtil *time_stretch_util)
{
  if(time_stretch_util == NULL){
    return(NULL);
  }

  return(time_stretch_util->destination);
}

/**
 * ags_time_stretch_util_set_destination:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * @destination: the destination buffer
 *
 * Set @destination buffer of @time_stretch_util.
 *
 * Since: 3.13.0
 */
void
ags_time_stretch_util_set_destination(AgsTimeStretchUtil *time_stretch_util,
				      gpointer destination)
{
  if(time_stretch_util == NULL){
    return;
  }

  time_stretch_util->destination = destination;
}

/**
 * ags_time_stretch_util_get_destination_stride:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Get destination stride of @time_stretch_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 3.13.0
 */
guint
ags_time_stretch_util_get_destination_stride(AgsTimeStretchUtil *time_stretch_util)
{
  if(time_stretch_util == NULL){
    return(0);
  }

  return(time_stretch_util->destination_stride);
}

/**
 * ags_time_stretch_util_set_destination_stride:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @time_stretch_util.
 *
 * Since: 3.13.0
 */
void
ags_time_stretch_util_set_destination_stride(AgsTimeStretchUtil *time_stretch_util,
					     guint destination_stride)
{
  if(time_stretch_util == NULL){
    return;
  }

  time_stretch_util->destination_stride = destination_stride;
}

/**
 * ags_time_stretch_util_get_destination_buffer_length:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Get destination buffer length of @time_stretch_util.
 * 
 * Returns: the destination buffer length
 * 
 * Since: 3.13.0
 */
guint
ags_time_stretch_util_get_destination_buffer_length(AgsTimeStretchUtil *time_stretch_util)
{
  if(time_stretch_util == NULL){
    return(0);
  }

  return(time_stretch_util->destination_buffer_length);
}

/**
 * ags_time_stretch_util_set_destination_buffer_length:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * @destination_buffer_length: the buffer length
 *
 * Set @destination_buffer_length of @time_stretch_util.
 *
 * Since: 3.13.0
 */
void
ags_time_stretch_util_set_destination_buffer_length(AgsTimeStretchUtil *time_stretch_util,
						    guint destination_buffer_length)
{
  if(time_stretch_util == NULL){
    return;
  }

  time_stretch_util->destination_buffer_length = destination_buffer_length;
}

/**
 * ags_time_stretch_util_get_buffer_size:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Get buffer size of @time_stretch_util.
 * 
 * Returns: the buffer size
 * 
 * Since: 3.13.0
 */
guint
ags_time_stretch_util_get_buffer_size(AgsTimeStretchUtil *time_stretch_util)
{
  if(time_stretch_util == NULL){
    return(0);
  }

  return(time_stretch_util->buffer_size);
}

/**
 * ags_time_stretch_util_set_buffer_size:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * @buffer_size: the buffer size
 *
 * Set @buffer_size of @time_stretch_util.
 *
 * Since: 3.13.0
 */
void
ags_time_stretch_util_set_buffer_size(AgsTimeStretchUtil *time_stretch_util,
				      guint buffer_size)
{
  if(time_stretch_util == NULL){
    return;
  }

  time_stretch_util->buffer_size = buffer_size;
}

/**
 * ags_time_stretch_util_get_format:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Get format of @time_stretch_util.
 * 
 * Returns: the format
 * 
 * Since: 3.13.0
 */
guint
ags_time_stretch_util_get_format(AgsTimeStretchUtil *time_stretch_util)
{
  if(time_stretch_util == NULL){
    return(0);
  }

  return(time_stretch_util->format);
}

/**
 * ags_time_stretch_util_set_format:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * @format: the format
 *
 * Set @format of @time_stretch_util.
 *
 * Since: 3.13.0
 */
void
ags_time_stretch_util_set_format(AgsTimeStretchUtil *time_stretch_util,
				 guint format)
{
  if(time_stretch_util == NULL){
    return;
  }

  time_stretch_util->format = format;
}

/**
 * ags_time_stretch_util_get_samplerate:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Get samplerate of @time_stretch_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.13.0
 */
guint
ags_time_stretch_util_get_samplerate(AgsTimeStretchUtil *time_stretch_util)
{
  if(time_stretch_util == NULL){
    return(0);
  }

  return(time_stretch_util->samplerate);
}

/**
 * ags_time_stretch_util_set_samplerate:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @time_stretch_util.
 *
 * Since: 3.13.0
 */
void
ags_time_stretch_util_set_samplerate(AgsTimeStretchUtil *time_stretch_util,
				     guint samplerate)
{
  if(time_stretch_util == NULL){
    return;
  }

  time_stretch_util->samplerate = samplerate;
}

/**
 * ags_time_stretch_util_get_frequency:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Get frequency of @time_stretch_util.
 * 
 * Returns: the frequency
 * 
 * Since: 3.13.0
 */
gdouble
ags_time_stretch_util_get_frequency(AgsTimeStretchUtil *time_stretch_util)
{
  if(time_stretch_util == NULL){
    return(0.0);
  }

  return(time_stretch_util->frequency);
}

/**
 * ags_time_stretch_util_set_frequency:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * @frequency: the frequency
 *
 * Set @frequency of @time_stretch_util.
 *
 * Since: 3.13.0
 */
void
ags_time_stretch_util_set_frequency(AgsTimeStretchUtil *time_stretch_util,
				    gdouble frequency)
{
  if(time_stretch_util == NULL){
    return;
  }

  time_stretch_util->frequency = frequency;
}

/**
 * ags_time_stretch_util_get_orig_bpm:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Get original BPM of @time_stretch_util.
 * 
 * Returns: the original BPM
 * 
 * Since: 3.13.0
 */
gdouble
ags_time_stretch_util_get_orig_bpm(AgsTimeStretchUtil *time_stretch_util)
{
  if(time_stretch_util == NULL){
    return(0);
  }

  return(time_stretch_util->orig_bpm);
}

/**
 * ags_time_stretch_util_set_orig_bpm:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * @orig_bpm: the original BPM
 *
 * Set @orig_bpm of @time_stretch_util.
 *
 * Since: 3.13.0
 */
void
ags_time_stretch_util_set_orig_bpm(AgsTimeStretchUtil *time_stretch_util,
				   gdouble orig_bpm)
{
  if(time_stretch_util == NULL){
    return;
  }

  time_stretch_util->orig_bpm = orig_bpm;
}

/**
 * ags_time_stretch_util_get_new_bpm:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Get new BPM of @time_stretch_util.
 * 
 * Returns: the new BPM
 * 
 * Since: 3.13.0
 */
gdouble
ags_time_stretch_util_get_new_bpm(AgsTimeStretchUtil *time_stretch_util)
{
  if(time_stretch_util == NULL){
    return(0);
  }

  return(time_stretch_util->new_bpm);
}

/**
 * ags_time_stretch_util_set_new_bpm:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * @new_bpm: the new_bpm
 *
 * Set @new_bpm of @time_stretch_util.
 *
 * Since: 3.13.0
 */
void
ags_time_stretch_util_set_new_bpm(AgsTimeStretchUtil *time_stretch_util,
				  gdouble new_bpm)
{
  if(time_stretch_util == NULL){
    return;
  }

  time_stretch_util->new_bpm = new_bpm;
}

/**
 * ags_time_stretch_util_stretch_s8:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Stretch @time_stretch_util of signed 8 bit data.
 * 
 * Since: 3.13.0
 */
void
ags_time_stretch_util_stretch_s8(AgsTimeStretchUtil *time_stretch_util)
{
  gint8 *destination, *source;

  guint destination_stride, source_stride;
  guint destinatio_buffer_length, nsource_buffer_length;
  guint buffer_size;
  guint samplerate;
  gdouble frequency;
  gdouble frequency_period;
  gdouble orig_delay;
  gdouble factor;

  guint i;
  
  if(time_stretch_util == NULL ||
     time_stretch_util->destination == NULL ||
     time_stretch_util->source == NULL){
    return;
  }

  destination = time_stretch_util->destination;
  destination_stride = time_stretch_util->destination_stride;
  destination_buffer_length = time_stretch_util->destination_buffer_length;

  source = time_stretch_util->source;
  source_stride = time_stretch_util->source_stride;
  source_buffer_length = time_stretch_util->source_buffer_length;

  buffer_size = time_stretch_util->buffer_size;
  samplerate = time_stretch_util->samplerate;

  frequency = time_stretch_util->frequency;

  frequency_period = 2.0 * M_PI * samplerate / frequency;

  orig_delay = samplerate / buffer_size / time_stretch_util->orig_bpm * ((1.0 / 16.0) * (1.0 / (1.0 / 4.0)));
  
  factor = time_stretch_util->new_bpm / time_stretch_util->orig_bpm;

  for(i = 0; i < destination_buffer_length; i++){
    gint8 new_z;
    guint buffer_offset;
    guint beat_offset;
    gdouble phase;
    guint start_x;
    guint j;

    beat_offset = (guint) floor((factor * i) / (orig_delay * buffer_size));

    if(floor(orig_delay * buffer_size) != 0.0){
      start_x = (orig_delay * buffer_size) * floor((double) (factor * i) / (orig_delay * buffer_size));
    }else{
      start_x = 0;
    }
    
    phase = fmod((factor * i) - (beat_offset * orig_delay * buffer_size), frequency_period);
    
    j = start_x + phase;

    if(j < source_buffer_length){
      if(factor > 1.0){
	new_z = source[j * source_stride];
      }else{
	gdouble k;
	gdouble t;

	k = fmod(phase, frequency_period);
	
	t = (gdouble) k / ((gdouble) (k + 1));
	
	new_z = ((1.0 - t) * source[j * source_stride]) + (t * (-1.0 * source[j * source_stride]));
      }
    }else{
      new_z = 0;
    }
    
    destination[i * destination_stride] = new_z;
  }
}

/**
 * ags_time_stretch_util_stretch_s16:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Stretch @time_stretch_util of signed 16 bit data.
 * 
 * Since: 3.13.0
 */
void
ags_time_stretch_util_stretch_s16(AgsTimeStretchUtil *time_stretch_util)
{
  gint16 *destination, *source;

  guint destination_stride, source_stride;
  guint destinatio_buffer_length, nsource_buffer_length;
  guint buffer_size;
  guint samplerate;
  gdouble frequency;
  gdouble frequency_period;
  gdouble orig_delay;
  gdouble factor;

  guint i;
  
  if(time_stretch_util == NULL ||
     time_stretch_util->destination == NULL ||
     time_stretch_util->source == NULL){
    return;
  }

  destination = time_stretch_util->destination;
  destination_stride = time_stretch_util->destination_stride;
  destination_buffer_length = time_stretch_util->destination_buffer_length;

  source = time_stretch_util->source;
  source_stride = time_stretch_util->source_stride;
  source_buffer_length = time_stretch_util->source_buffer_length;

  buffer_size = time_stretch_util->buffer_size;
  samplerate = time_stretch_util->samplerate;

  frequency = time_stretch_util->frequency;

  frequency_period = 2.0 * M_PI * samplerate / frequency;

  orig_delay = samplerate / buffer_size / time_stretch_util->orig_bpm * ((1.0 / 16.0) * (1.0 / (1.0 / 4.0)));
  
  factor = time_stretch_util->new_bpm / time_stretch_util->orig_bpm;

  for(i = 0; i < destination_buffer_length; i++){
    gint16 new_z;
    guint buffer_offset;
    guint beat_offset;
    gdouble phase;
    guint start_x;
    guint j;

    beat_offset = (guint) floor((factor * i) / (orig_delay * buffer_size));

    if(floor(orig_delay * buffer_size) != 0.0){
      start_x = (orig_delay * buffer_size) * floor((double) (factor * i) / (orig_delay * buffer_size));
    }else{
      start_x = 0;
    }
    
    phase = fmod((factor * i) - (beat_offset * orig_delay * buffer_size), frequency_period);
    
    j = start_x + phase;

    if(j < source_buffer_length){
      if(factor > 1.0){
	new_z = source[j * source_stride];
      }else{
	gdouble k;
	gdouble t;

	k = fmod(phase, frequency_period);
	
	t = (gdouble) k / ((gdouble) (k + 1));
	
	new_z = ((1.0 - t) * source[j * source_stride]) + (t * (-1.0 * source[j * source_stride]));
      }
    }else{
      new_z = 0;
    }
    
    destination[i * destination_stride] = new_z;
  }
}

/**
 * ags_time_stretch_util_stretch_s24:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Stretch @time_stretch_util of signed 24 bit data.
 * 
 * Since: 3.13.0
 */
void
ags_time_stretch_util_stretch_s24(AgsTimeStretchUtil *time_stretch_util)
{
  gint32 *destination, *source;

  guint destination_stride, source_stride;
  guint destinatio_buffer_length, nsource_buffer_length;
  guint buffer_size;
  guint samplerate;
  gdouble frequency;
  gdouble frequency_period;
  gdouble orig_delay;
  gdouble factor;

  guint i;
  
  if(time_stretch_util == NULL ||
     time_stretch_util->destination == NULL ||
     time_stretch_util->source == NULL){
    return;
  }

  destination = time_stretch_util->destination;
  destination_stride = time_stretch_util->destination_stride;
  destination_buffer_length = time_stretch_util->destination_buffer_length;

  source = time_stretch_util->source;
  source_stride = time_stretch_util->source_stride;
  source_buffer_length = time_stretch_util->source_buffer_length;

  buffer_size = time_stretch_util->buffer_size;
  samplerate = time_stretch_util->samplerate;

  frequency = time_stretch_util->frequency;

  frequency_period = 2.0 * M_PI * samplerate / frequency;

  orig_delay = samplerate / buffer_size / time_stretch_util->orig_bpm * ((1.0 / 16.0) * (1.0 / (1.0 / 4.0)));
  
  factor = time_stretch_util->new_bpm / time_stretch_util->orig_bpm;

  for(i = 0; i < destination_buffer_length; i++){
    gint32 new_z;
    guint buffer_offset;
    guint beat_offset;
    gdouble phase;
    guint start_x;
    guint j;

    beat_offset = (guint) floor((factor * i) / (orig_delay * buffer_size));

    if(floor(orig_delay * buffer_size) != 0.0){
      start_x = (orig_delay * buffer_size) * floor((double) (factor * i) / (orig_delay * buffer_size));
    }else{
      start_x = 0;
    }
    
    phase = fmod((factor * i) - (beat_offset * orig_delay * buffer_size), frequency_period);
    
    j = start_x + phase;

    if(j < source_buffer_length){
      if(factor > 1.0){
	new_z = source[j * source_stride];
      }else{
	gdouble k;
	gdouble t;

	k = fmod(phase, frequency_period);
	
	t = (gdouble) k / ((gdouble) (k + 1));
	
	new_z = ((1.0 - t) * source[j * source_stride]) + (t * (-1.0 * source[j * source_stride]));
      }
    }else{
      new_z = 0;
    }
    
    destination[i * destination_stride] = new_z;
  }
}

/**
 * ags_time_stretch_util_stretch_s32:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Stretch @time_stretch_util of signed 32 bit data.
 * 
 * Since: 3.13.0
 */
void
ags_time_stretch_util_stretch_s32(AgsTimeStretchUtil *time_stretch_util)
{
  gint32 *destination, *source;

  guint destination_stride, source_stride;
  guint destinatio_buffer_length, nsource_buffer_length;
  guint buffer_size;
  guint samplerate;
  gdouble frequency;
  gdouble frequency_period;
  gdouble orig_delay;
  gdouble factor;

  guint i;
  
  if(time_stretch_util == NULL ||
     time_stretch_util->destination == NULL ||
     time_stretch_util->source == NULL){
    return;
  }

  destination = time_stretch_util->destination;
  destination_stride = time_stretch_util->destination_stride;
  destination_buffer_length = time_stretch_util->destination_buffer_length;

  source = time_stretch_util->source;
  source_stride = time_stretch_util->source_stride;
  source_buffer_length = time_stretch_util->source_buffer_length;

  buffer_size = time_stretch_util->buffer_size;
  samplerate = time_stretch_util->samplerate;

  frequency = time_stretch_util->frequency;

  frequency_period = 2.0 * M_PI * samplerate / frequency;

  orig_delay = samplerate / buffer_size / time_stretch_util->orig_bpm * ((1.0 / 16.0) * (1.0 / (1.0 / 4.0)));
  
  factor = time_stretch_util->new_bpm / time_stretch_util->orig_bpm;

  for(i = 0; i < destination_buffer_length; i++){
    gint32 new_z;
    guint buffer_offset;
    guint beat_offset;
    gdouble phase;
    guint start_x;
    guint j;

    beat_offset = (guint) floor((factor * i) / (orig_delay * buffer_size));

    if(floor(orig_delay * buffer_size) != 0.0){
      start_x = (orig_delay * buffer_size) * floor((double) (factor * i) / (orig_delay * buffer_size));
    }else{
      start_x = 0;
    }
    
    phase = fmod((factor * i) - (beat_offset * orig_delay * buffer_size), frequency_period);
    
    j = start_x + phase;

    if(j < source_buffer_length){
      if(factor > 1.0){
	new_z = source[j * source_stride];
      }else{
	gdouble k;
	gdouble t;

	k = fmod(phase, frequency_period);
	
	t = (gdouble) k / ((gdouble) (k + 1));
	
	new_z = ((1.0 - t) * source[j * source_stride]) + (t * (-1.0 * source[j * source_stride]));
      }
    }else{
      new_z = 0;
    }
    
    destination[i * destination_stride] = new_z;
  }
}

/**
 * ags_time_stretch_util_stretch_s64:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Stretch @time_stretch_util of signed 64 bit data.
 * 
 * Since: 3.13.0
 */
void
ags_time_stretch_util_stretch_s64(AgsTimeStretchUtil *time_stretch_util)
{
  gint64 *destination, *source;

  guint destination_stride, source_stride;
  guint destinatio_buffer_length, nsource_buffer_length;
  guint buffer_size;
  guint samplerate;
  gdouble frequency;
  gdouble frequency_period;
  gdouble orig_delay;
  gdouble factor;

  guint i;
  
  if(time_stretch_util == NULL ||
     time_stretch_util->destination == NULL ||
     time_stretch_util->source == NULL){
    return;
  }

  destination = time_stretch_util->destination;
  destination_stride = time_stretch_util->destination_stride;
  destination_buffer_length = time_stretch_util->destination_buffer_length;

  source = time_stretch_util->source;
  source_stride = time_stretch_util->source_stride;
  source_buffer_length = time_stretch_util->source_buffer_length;

  buffer_size = time_stretch_util->buffer_size;
  samplerate = time_stretch_util->samplerate;

  frequency = time_stretch_util->frequency;

  frequency_period = 2.0 * M_PI * samplerate / frequency;

  orig_delay = samplerate / buffer_size / time_stretch_util->orig_bpm * ((1.0 / 16.0) * (1.0 / (1.0 / 4.0)));
  
  factor = time_stretch_util->new_bpm / time_stretch_util->orig_bpm;

  for(i = 0; i < destination_buffer_length; i++){
    gint64 new_z;
    guint buffer_offset;
    guint beat_offset;
    gdouble phase;
    guint start_x;
    guint j;

    beat_offset = (guint) floor((factor * i) / (orig_delay * buffer_size));

    if(floor(orig_delay * buffer_size) != 0.0){
      start_x = (orig_delay * buffer_size) * floor((double) (factor * i) / (orig_delay * buffer_size));
    }else{
      start_x = 0;
    }
    
    phase = fmod((factor * i) - (beat_offset * orig_delay * buffer_size), frequency_period);
    
    j = start_x + phase;

    if(j < source_buffer_length){
      if(factor > 1.0){
	new_z = source[j * source_stride];
      }else{
	gdouble k;
	gdouble t;

	k = fmod(phase, frequency_period);
	
	t = (gdouble) k / ((gdouble) (k + 1));
	
	new_z = ((1.0 - t) * source[j * source_stride]) + (t * (-1.0 * source[j * source_stride]));
      }
    }else{
      new_z = 0;
    }
    
    destination[i * destination_stride] = new_z;
  }
}

/**
 * ags_time_stretch_util_stretch_float:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Stretch @time_stretch_util of floating point data.
 * 
 * Since: 3.13.0
 */
void
ags_time_stretch_util_stretch_float(AgsTimeStretchUtil *time_stretch_util)
{
  gfloat *destination, *source;

  guint destination_stride, source_stride;
  guint destinatio_buffer_length, nsource_buffer_length;
  guint buffer_size;
  guint samplerate;
  gdouble frequency;
  gdouble frequency_period;
  gdouble orig_delay;
  gdouble factor;

  guint i;
  
  if(time_stretch_util == NULL ||
     time_stretch_util->destination == NULL ||
     time_stretch_util->source == NULL){
    return;
  }

  destination = time_stretch_util->destination;
  destination_stride = time_stretch_util->destination_stride;
  destination_buffer_length = time_stretch_util->destination_buffer_length;

  source = time_stretch_util->source;
  source_stride = time_stretch_util->source_stride;
  source_buffer_length = time_stretch_util->source_buffer_length;

  buffer_size = time_stretch_util->buffer_size;
  samplerate = time_stretch_util->samplerate;

  frequency = time_stretch_util->frequency;

  frequency_period = 2.0 * M_PI * samplerate / frequency;

  orig_delay = samplerate / buffer_size / time_stretch_util->orig_bpm * ((1.0 / 16.0) * (1.0 / (1.0 / 4.0)));
  
  factor = time_stretch_util->new_bpm / time_stretch_util->orig_bpm;

  for(i = 0; i < destination_buffer_length; i++){
    gfloat new_z;
    guint buffer_offset;
    guint beat_offset;
    gdouble phase;
    guint start_x;
    guint j;

    beat_offset = (guint) floor((factor * i) / (orig_delay * buffer_size));

    if(floor(orig_delay * buffer_size) != 0.0){
      start_x = (orig_delay * buffer_size) * floor((double) (factor * i) / (orig_delay * buffer_size));
    }else{
      start_x = 0;
    }
    
    phase = fmod((factor * i) - (beat_offset * orig_delay * buffer_size), frequency_period);
    
    j = start_x + phase;

    if(j < source_buffer_length){
      if(factor > 1.0){
	new_z = source[j * source_stride];
      }else{
	gdouble k;
	gdouble t;

	k = fmod(phase, frequency_period);
	
	t = (gdouble) k / ((gdouble) (k + 1));
	
	new_z = ((1.0 - t) * source[j * source_stride]) + (t * (-1.0 * source[j * source_stride]));
      }
    }else{
      new_z = 0;
    }
    
    destination[i * destination_stride] = new_z;
  }
  //TODO:JK: implement me
}

/**
 * ags_time_stretch_util_stretch_double:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Stretch @time_stretch_util of double precision floating point data.
 * 
 * Since: 3.13.0
 */
void
ags_time_stretch_util_stretch_double(AgsTimeStretchUtil *time_stretch_util)
{
  gdouble *destination, *source;

  guint destination_stride, source_stride;
  guint destinatio_buffer_length, nsource_buffer_length;
  guint buffer_size;
  guint samplerate;
  gdouble frequency;
  gdouble frequency_period;
  gdouble orig_delay;
  gdouble factor;

  guint i;
  
  if(time_stretch_util == NULL ||
     time_stretch_util->destination == NULL ||
     time_stretch_util->source == NULL){
    return;
  }

  destination = time_stretch_util->destination;
  destination_stride = time_stretch_util->destination_stride;
  destination_buffer_length = time_stretch_util->destination_buffer_length;

  source = time_stretch_util->source;
  source_stride = time_stretch_util->source_stride;
  source_buffer_length = time_stretch_util->source_buffer_length;

  buffer_size = time_stretch_util->buffer_size;
  samplerate = time_stretch_util->samplerate;

  frequency = time_stretch_util->frequency;

  frequency_period = 2.0 * M_PI * samplerate / frequency;

  orig_delay = samplerate / buffer_size / time_stretch_util->orig_bpm * ((1.0 / 16.0) * (1.0 / (1.0 / 4.0)));
  
  factor = time_stretch_util->new_bpm / time_stretch_util->orig_bpm;

  for(i = 0; i < destination_buffer_length; i++){
    gdouble new_z;
    guint buffer_offset;
    guint beat_offset;
    gdouble phase;
    guint start_x;
    guint j;

    beat_offset = (guint) floor((factor * i) / (orig_delay * buffer_size));

    if(floor(orig_delay * buffer_size) != 0.0){
      start_x = (orig_delay * buffer_size) * floor((double) (factor * i) / (orig_delay * buffer_size));
    }else{
      start_x = 0;
    }
    
    phase = fmod((factor * i) - (beat_offset * orig_delay * buffer_size), frequency_period);
    
    j = start_x + phase;

    if(j < source_buffer_length){
      if(factor > 1.0){
	new_z = source[j * source_stride];
      }else{
	gdouble k;
	gdouble t;

	k = fmod(phase, frequency_period);
	
	t = (gdouble) k / ((gdouble) (k + 1));
	
	new_z = ((1.0 - t) * source[j * source_stride]) + (t * (-1.0 * source[j * source_stride]));
      }
    }else{
      new_z = 0;
    }
    
    destination[i * destination_stride] = new_z;
  }
  //TODO:JK: implement me
}

/**
 * ags_time_stretch_util_stretch_complex:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Stretch @time_stretch_util of complex data.
 * 
 * Since: 3.13.0
 */
void
ags_time_stretch_util_stretch_complex(AgsTimeStretchUtil *time_stretch_util)
{
  AgsComplex *destination, *source;

  guint destination_stride, source_stride;
  guint destinatio_buffer_length, nsource_buffer_length;
  guint buffer_size;
  guint samplerate;
  gdouble frequency;
  gdouble frequency_period;
  gdouble orig_delay;
  gdouble factor;

  guint i;
  
  if(time_stretch_util == NULL ||
     time_stretch_util->destination == NULL ||
     time_stretch_util->source == NULL){
    return;
  }

  destination = time_stretch_util->destination;
  destination_stride = time_stretch_util->destination_stride;
  destination_buffer_length = time_stretch_util->destination_buffer_length;

  source = time_stretch_util->source;
  source_stride = time_stretch_util->source_stride;
  source_buffer_length = time_stretch_util->source_buffer_length;

  buffer_size = time_stretch_util->buffer_size;
  samplerate = time_stretch_util->samplerate;

  frequency = time_stretch_util->frequency;

  frequency_period = 2.0 * M_PI * samplerate / frequency;

  orig_delay = samplerate / buffer_size / time_stretch_util->orig_bpm * ((1.0 / 16.0) * (1.0 / (1.0 / 4.0)));
  
  factor = time_stretch_util->new_bpm / time_stretch_util->orig_bpm;

  for(i = 0; i < destination_buffer_length; i++){
    double _Complex new_z;
    guint buffer_offset;
    guint beat_offset;
    gdouble phase;
    guint start_x;
    guint j;

    beat_offset = (guint) floor((factor * i) / (orig_delay * buffer_size));

    if(floor(orig_delay * buffer_size) != 0.0){
      start_x = (orig_delay * buffer_size) * floor((double) (factor * i) / (orig_delay * buffer_size));
    }else{
      start_x = 0;
    }
    
    phase = fmod((factor * i) - (beat_offset * orig_delay * buffer_size), frequency_period);
    
    j = start_x + phase;

    if(j < source_buffer_length){
      if(factor > 1.0){
	new_z = ags_complex_get(source + (j * source_stride));
      }else{
	gdouble k;
	gdouble t;

	k = fmod(phase, frequency_period);
	
	t = (gdouble) k / ((gdouble) (k + 1));
	
	new_z = ((1.0 - t) * ags_complex_get(source + (j * source_stride))) + (t * (-1.0 * ags_complex_get(source + (j * source_stride))));
      }
    }else{
      new_z = 0;
    }

    ags_complex_set(destination + (i * destination_stride),
		    new_z);
  }
}

/**
 * ags_time_stretch_util_stretch:
 * @time_stretch_util: the #AgsTimeStretchUtil-struct
 * 
 * Stretch @time_stretch_util.
 * 
 * Since: 3.13.0
 */
void
ags_time_stretch_util_stretch(AgsTimeStretchUtil *time_stretch_util)
{
  if(time_stretch_util == NULL ||
     time_stretch_util->destination == NULL ||
     time_stretch_util->source == NULL){
    return;
  }

  switch(time_stretch_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_time_stretch_util_pitch_s8(time_stretch_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_time_stretch_util_pitch_s16(time_stretch_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_time_stretch_util_pitch_s24(time_stretch_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_time_stretch_util_pitch_s32(time_stretch_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_time_stretch_util_pitch_s64(time_stretch_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_time_stretch_util_pitch_float(time_stretch_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_time_stretch_util_pitch_double(time_stretch_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_time_stretch_util_pitch_complex(time_stretch_util);
  }
  break;
  default:
    g_warning("unknown format");
  }
}
