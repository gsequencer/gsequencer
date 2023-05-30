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

#include <ags/audio/ags_hq_pitch_util.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

/**
 * SECTION:ags_hq_pitch_util
 * @short_description: hq pitch util
 * @title: AgsHQPitchUtil
 * @section_id:
 * @include: ags/audio/ags_hq_pitch_util.h
 *
 * Utility functions to pitch.
 */

GType
ags_hq_pitch_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_hq_pitch_util = 0;

    ags_type_hq_pitch_util =
      g_boxed_type_register_static("AgsHQPitchUtil",
				   (GBoxedCopyFunc) ags_hq_pitch_util_copy,
				   (GBoxedFreeFunc) ags_hq_pitch_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_hq_pitch_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_hq_pitch_util_alloc:
 * 
 * Allocate #AgsHQPitchUtil-struct.
 * 
 * Returns: the newly allocated #AgsHQPitchUtil-struct
 * 
 * Since: 3.9.6
 */
AgsHQPitchUtil*
ags_hq_pitch_util_alloc()
{
  AgsHQPitchUtil *ptr;
  
  ptr = (AgsHQPitchUtil *) g_new(AgsHQPitchUtil,
				 1);

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->destination = NULL;
  ptr->destination_stride = 1;

  ptr->low_mix_buffer = ags_stream_alloc(AGS_HQ_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE,
					 AGS_SOUNDCARD_DEFAULT_FORMAT);

  ptr->low_mix_buffer_max_buffer_length = AGS_HQ_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE;
  
  ptr->new_mix_buffer = ags_stream_alloc(AGS_HQ_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE,
					 AGS_SOUNDCARD_DEFAULT_FORMAT);

  ptr->new_mix_buffer_max_buffer_length = AGS_HQ_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE;
  
  ptr->buffer_length = 0;
  ptr->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  ptr->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  ptr->base_key = 0.0;
  ptr->tuning = 0.0;

  ptr->linear_interpolate_util = ags_linear_interpolate_util_alloc();

  ptr->vibrato_gain = 1.0;
  ptr->vibrato_lfo_depth = 0.0;
  ptr->vibrato_lfo_freq = 8.172;
  ptr->vibrato_tuning = 0.0;

  ptr->vibrato_lfo_frame_count = ptr->samplerate / ptr->vibrato_lfo_freq;
  ptr->vibrato_lfo_offset = 0;

  return(ptr);
}

/**
 * ags_hq_pitch_util_copy:
 * @ptr: the original #AgsHQPitchUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsHQPitchUtil-struct
 *
 * Since: 3.9.6
 */
gpointer
ags_hq_pitch_util_copy(AgsHQPitchUtil *ptr)
{
  AgsHQPitchUtil *new_ptr;
  
  new_ptr = (AgsHQPitchUtil *) g_new(AgsHQPitchUtil,
				     1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->low_mix_buffer = ags_stream_alloc(AGS_HQ_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE,
					     ptr->format);

  new_ptr->low_mix_buffer_max_buffer_length = AGS_HQ_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE;
  
  new_ptr->new_mix_buffer = ags_stream_alloc(AGS_HQ_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE,
					     ptr->format);
  
  new_ptr->new_mix_buffer_max_buffer_length = AGS_HQ_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE;
  
  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->base_key = ptr->base_key;
  new_ptr->tuning = ptr->tuning;

  new_ptr->vibrato_gain = ptr->vibrato_gain;
  new_ptr->vibrato_lfo_depth = ptr->vibrato_lfo_depth;
  new_ptr->vibrato_lfo_freq = ptr->vibrato_lfo_freq;
  new_ptr->vibrato_tuning = ptr->vibrato_tuning;

  new_ptr->vibrato_lfo_frame_count = ptr->vibrato_lfo_frame_count;
  new_ptr->vibrato_lfo_offset = ptr->vibrato_lfo_offset;

  return(new_ptr);
}

/**
 * ags_hq_pitch_util_free:
 * @ptr: the #AgsHQPitchUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_free(AgsHQPitchUtil *ptr)
{
  if(ptr == NULL){
    return;
  }
  
  g_free(ptr->destination);

  if(ptr->destination != ptr->source){
    g_free(ptr->source);
  }

  ags_stream_free(ptr->low_mix_buffer);
  ags_stream_free(ptr->new_mix_buffer);
  
  g_free(ptr);
}

/**
 * ags_hq_pitch_util_get_destination:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Get destination buffer of @hq_pitch_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 3.9.6
 */
gpointer
ags_hq_pitch_util_get_destination(AgsHQPitchUtil *hq_pitch_util)
{
  if(hq_pitch_util == NULL){
    return(NULL);
  }

  return(hq_pitch_util->destination);
}

/**
 * ags_hq_pitch_util_set_destination:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * @destination: the destination buffer
 *
 * Set @destination buffer of @hq_pitch_util.
 *
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_set_destination(AgsHQPitchUtil *hq_pitch_util,
				  gpointer destination)
{
  if(hq_pitch_util == NULL){
    return;
  }

  hq_pitch_util->destination = destination;
}

/**
 * ags_hq_pitch_util_get_destination_stride:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Get destination stride of @hq_pitch_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 3.9.6
 */
guint
ags_hq_pitch_util_get_destination_stride(AgsHQPitchUtil *hq_pitch_util)
{
  if(hq_pitch_util == NULL){
    return(0);
  }

  return(hq_pitch_util->destination_stride);
}

/**
 * ags_hq_pitch_util_set_destination_stride:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @hq_pitch_util.
 *
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_set_destination_stride(AgsHQPitchUtil *hq_pitch_util,
					 guint destination_stride)
{
  if(hq_pitch_util == NULL){
    return;
  }

  hq_pitch_util->destination_stride = destination_stride;
}

/**
 * ags_hq_pitch_util_get_source:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Get source buffer of @hq_pitch_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.9.6
 */
gpointer
ags_hq_pitch_util_get_source(AgsHQPitchUtil *hq_pitch_util)
{
  if(hq_pitch_util == NULL){
    return(NULL);
  }

  return(hq_pitch_util->source);
}

/**
 * ags_hq_pitch_util_set_source:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @hq_pitch_util.
 *
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_set_source(AgsHQPitchUtil *hq_pitch_util,
			     gpointer source)
{
  if(hq_pitch_util == NULL){
    return;
  }

  hq_pitch_util->source = source;

  ags_linear_interpolate_util_set_source(hq_pitch_util->linear_interpolate_util,
					 source);
}

/**
 * ags_hq_pitch_util_get_source_stride:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Get source stride of @hq_pitch_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.9.6
 */
guint
ags_hq_pitch_util_get_source_stride(AgsHQPitchUtil *hq_pitch_util)
{
  if(hq_pitch_util == NULL){
    return(0);
  }

  return(hq_pitch_util->source_stride);
}

/**
 * ags_hq_pitch_util_set_source_stride:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @hq_pitch_util.
 *
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_set_source_stride(AgsHQPitchUtil *hq_pitch_util,
				    guint source_stride)
{
  if(hq_pitch_util == NULL){
    return;
  }

  hq_pitch_util->source_stride = source_stride;
}

/**
 * ags_hq_pitch_util_get_buffer_length:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Get buffer length of @hq_pitch_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.9.6
 */
guint
ags_hq_pitch_util_get_buffer_length(AgsHQPitchUtil *hq_pitch_util)
{
  if(hq_pitch_util == NULL){
    return(0);
  }

  return(hq_pitch_util->buffer_length);
}

/**
 * ags_hq_pitch_util_set_buffer_length:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @hq_pitch_util.
 *
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_set_buffer_length(AgsHQPitchUtil *hq_pitch_util,
				    guint buffer_length)
{
  if(hq_pitch_util == NULL ||
     hq_pitch_util->buffer_length == buffer_length){
    return;
  }

  hq_pitch_util->buffer_length = buffer_length;
  
  ags_linear_interpolate_util_set_buffer_length(hq_pitch_util->linear_interpolate_util,
						buffer_length);
}

/**
 * ags_hq_pitch_util_get_format:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Get format of @hq_pitch_util.
 * 
 * Returns: the format
 * 
 * Since: 3.9.6
 */
AgsSoundcardFormat
ags_hq_pitch_util_get_format(AgsHQPitchUtil *hq_pitch_util)
{
  if(hq_pitch_util == NULL){
    return(0);
  }

  return(hq_pitch_util->format);
}

/**
 * ags_hq_pitch_util_set_format:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * @format: the format
 *
 * Set @format of @hq_pitch_util.
 *
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_set_format(AgsHQPitchUtil *hq_pitch_util,
			     AgsSoundcardFormat format)
{
  if(hq_pitch_util == NULL ||
     hq_pitch_util->format == format){
    return;
  }

  hq_pitch_util->format = format;

  ags_stream_free(hq_pitch_util->low_mix_buffer);
  ags_stream_free(hq_pitch_util->new_mix_buffer);

  hq_pitch_util->low_mix_buffer = ags_stream_alloc(AGS_HQ_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE,
						   hq_pitch_util->format);
  
  hq_pitch_util->new_mix_buffer = ags_stream_alloc(AGS_HQ_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE,
						   hq_pitch_util->format);
  
  ags_linear_interpolate_util_set_format(hq_pitch_util->linear_interpolate_util,
					 format);
}

/**
 * ags_hq_pitch_util_get_samplerate:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Get samplerate of @hq_pitch_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.9.6
 */
guint
ags_hq_pitch_util_get_samplerate(AgsHQPitchUtil *hq_pitch_util)
{
  if(hq_pitch_util == NULL){
    return(0);
  }

  return(hq_pitch_util->samplerate);
}

/**
 * ags_hq_pitch_util_set_samplerate:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @hq_pitch_util.
 *
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_set_samplerate(AgsHQPitchUtil *hq_pitch_util,
				 guint samplerate)
{
  if(hq_pitch_util == NULL ||
     hq_pitch_util->samplerate == samplerate){
    return;
  }

  hq_pitch_util->samplerate = samplerate;

  ags_linear_interpolate_util_set_samplerate(hq_pitch_util->linear_interpolate_util,
					     samplerate);
}

/**
 * ags_hq_pitch_util_get_base_key:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Get base key of @hq_pitch_util.
 * 
 * Returns: the base key
 * 
 * Since: 3.9.6
 */
gdouble
ags_hq_pitch_util_get_base_key(AgsHQPitchUtil *hq_pitch_util)
{
  if(hq_pitch_util == NULL){
    return(0.0);
  }

  return(hq_pitch_util->base_key);
}

/**
 * ags_hq_pitch_util_set_base_key:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * @base_key: the base key
 *
 * Set @base_key of @hq_pitch_util.
 *
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_set_base_key(AgsHQPitchUtil *hq_pitch_util,
			       gdouble base_key)
{
  if(hq_pitch_util == NULL){
    return;
  }

  hq_pitch_util->base_key = base_key;
}

/**
 * ags_hq_pitch_util_get_tuning:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Get tuning of @hq_pitch_util.
 * 
 * Returns: the tuning
 * 
 * Since: 3.9.6
 */
gdouble
ags_hq_pitch_util_get_tuning(AgsHQPitchUtil *hq_pitch_util)
{
  if(hq_pitch_util == NULL){
    return(0.0);
  }

  return(hq_pitch_util->tuning);
}

/**
 * ags_hq_pitch_util_set_tuning:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * @tuning: the tuning
 *
 * Set @tuning of @hq_pitch_util.
 *
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_set_tuning(AgsHQPitchUtil *hq_pitch_util,
			     gdouble tuning)
{
  if(hq_pitch_util == NULL){
    return;
  }

  hq_pitch_util->tuning = tuning;
}

/**
 * ags_hq_pitch_util_get_vibrato_gain:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Get vibrato gain of @hq_pitch_util.
 * 
 * Returns: the vibrato gain
 * 
 * Since: 5.2.0
 */
gdouble
ags_hq_pitch_util_get_vibrato_gain(AgsHQPitchUtil *hq_pitch_util)
{
  if(hq_pitch_util == NULL){
    return(0.0);
  }

  return(hq_pitch_util->vibrato_gain);
}

/**
 * ags_hq_pitch_util_set_vibrato_gain:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * @vibrato_gain: the vibrato gain
 *
 * Set @vibrato_gain of @hq_pitch_util.
 *
 * Since: 5.2.0
 */
void
ags_hq_pitch_util_set_vibrato_gain(AgsHQPitchUtil *hq_pitch_util,
				   gdouble vibrato_gain)
{
  if(hq_pitch_util == NULL){
    return;
  }

  hq_pitch_util->vibrato_gain = vibrato_gain;
}

/**
 * ags_hq_pitch_util_get_vibrato_lfo_depth:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Get vibrato lfo_depth of @hq_pitch_util.
 * 
 * Returns: the vibrato lfo_depth
 * 
 * Since: 5.2.0
 */
gdouble
ags_hq_pitch_util_get_vibrato_lfo_depth(AgsHQPitchUtil *hq_pitch_util)
{
  if(hq_pitch_util == NULL){
    return(0.0);
  }

  return(hq_pitch_util->vibrato_lfo_depth);
}

/**
 * ags_hq_pitch_util_set_vibrato_lfo_depth:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * @vibrato_lfo_depth: the vibrato lfo_depth
 *
 * Set @vibrato_lfo_depth of @hq_pitch_util.
 *
 * Since: 5.2.0
 */
void
ags_hq_pitch_util_set_vibrato_lfo_depth(AgsHQPitchUtil *hq_pitch_util,
					gdouble vibrato_lfo_depth)
{
  if(hq_pitch_util == NULL){
    return;
  }

  hq_pitch_util->vibrato_lfo_depth = vibrato_lfo_depth;
}

/**
 * ags_hq_pitch_util_get_vibrato_lfo_freq:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Get vibrato lfo_freq of @hq_pitch_util.
 * 
 * Returns: the vibrato lfo_freq
 * 
 * Since: 5.2.0
 */
gdouble
ags_hq_pitch_util_get_vibrato_lfo_freq(AgsHQPitchUtil *hq_pitch_util)
{
  if(hq_pitch_util == NULL){
    return(0.0);
  }

  return(hq_pitch_util->vibrato_lfo_freq);
}

/**
 * ags_hq_pitch_util_set_vibrato_lfo_freq:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * @vibrato_lfo_freq: the vibrato lfo_freq
 *
 * Set @vibrato_lfo_freq of @hq_pitch_util.
 *
 * Since: 5.2.0
 */
void
ags_hq_pitch_util_set_vibrato_lfo_freq(AgsHQPitchUtil *hq_pitch_util,
				       gdouble vibrato_lfo_freq)
{
  if(hq_pitch_util == NULL){
    return;
  }

  hq_pitch_util->vibrato_lfo_freq = vibrato_lfo_freq;
}

/**
 * ags_hq_pitch_util_get_vibrato_tuning:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Get vibrato tuning of @hq_pitch_util.
 * 
 * Returns: the vibrato tuning
 * 
 * Since: 5.2.0
 */
gdouble
ags_hq_pitch_util_get_vibrato_tuning(AgsHQPitchUtil *hq_pitch_util)
{
  if(hq_pitch_util == NULL){
    return(0.0);
  }

  return(hq_pitch_util->vibrato_tuning);
}

/**
 * ags_hq_pitch_util_set_vibrato_tuning:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * @vibrato_tuning: the vibrato tuning
 *
 * Set @vibrato_tuning of @hq_pitch_util.
 *
 * Since: 5.2.0
 */
void
ags_hq_pitch_util_set_vibrato_tuning(AgsHQPitchUtil *hq_pitch_util,
				     gdouble vibrato_tuning)
{
  if(hq_pitch_util == NULL){
    return;
  }

  hq_pitch_util->vibrato_tuning = vibrato_tuning;
}

/**
 * ags_hq_pitch_util_pitch_s8:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Pitch @hq_pitch_util of signed 8 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_pitch_s8(AgsHQPitchUtil *hq_pitch_util)
{
  AgsLinearInterpolateUtil *linear_interpolate_util;
  
  gint8 *destination, *source;
  gint8 *ptr_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint8 *mix_buffer, *new_mix_buffer, *low_mix_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble volume;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint vibrato_lfo_offset;

  gdouble base_freq, low_freq, new_freq;
  gdouble offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, low_freq_period, new_freq_period;
  guint low_mix_buffer_length;  
  guint i, j;

  if(hq_pitch_util == NULL ||
     hq_pitch_util->destination == NULL ||
     hq_pitch_util->source == NULL ||
     hq_pitch_util->tuning == 0.0){
    return;
  }

  linear_interpolate_util = hq_pitch_util->linear_interpolate_util;
  
  destination = hq_pitch_util->destination;
  destination_stride = hq_pitch_util->destination_stride;

  source = hq_pitch_util->source;
  source_stride = hq_pitch_util->source_stride;

  buffer_length = hq_pitch_util->buffer_length;
  samplerate = hq_pitch_util->samplerate;

  /* frequency */
  base_freq = exp2((hq_pitch_util->base_key) / 12.0) * 440.0;
  new_freq = exp2((hq_pitch_util->base_key + (hq_pitch_util->tuning / 100.0))  / 12.0) * 440.0;
  
  low_freq = exp2((hq_pitch_util->base_key - 0.25) / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(low_freq <= 0.0){
    g_warning("rejecting pitch low freq %f <= 0.0", low_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }
  
  volume = 1.0 / base_freq * new_freq;

  vibrato_gain = hq_pitch_util->vibrato_gain;
  vibrato_lfo_depth = hq_pitch_util->vibrato_lfo_depth;
  vibrato_lfo_freq = hq_pitch_util->vibrato_lfo_freq;
  vibrato_tuning = hq_pitch_util->vibrato_tuning;
  vibrato_lfo_offset = hq_pitch_util->vibrato_lfo_offset;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* get buffer */
  low_mix_buffer_length = (freq_period / low_freq_period) * buffer_length;

  mix_buffer = source;

  low_mix_buffer = (gint8 *) hq_pitch_util->low_mix_buffer;

  new_mix_buffer = (gint8 *) hq_pitch_util->new_mix_buffer;

  ags_linear_interpolate_util_set_destination(linear_interpolate_util,
					      low_mix_buffer);
  
  ags_linear_interpolate_util_set_source(linear_interpolate_util,
					 source);

  ags_linear_interpolate_util_set_buffer_length(linear_interpolate_util,
						buffer_length);

  ags_linear_interpolate_util_set_format(linear_interpolate_util,
					 AGS_SOUNDCARD_SIGNED_8_BIT);
  
  ags_linear_interpolate_util_set_factor(linear_interpolate_util,
					 freq_period / low_freq_period);
  
  ags_linear_interpolate_util_pitch(linear_interpolate_util);

  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint8 new_z;
    gdouble phase, low_phase, new_phase;    
    guint start_x, low_start_x;

    new_freq = (exp2((hq_pitch_util->base_key + (hq_pitch_util->tuning / 100.0)) / 12.0) * 440.0) * sin((vibrato_lfo_offset + i) * 2.0 * M_PI * vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0 + sin((vibrato_lfo_offset + i) * 2.0 * M_PI * vibrato_lfo_freq / samplerate) * vibrato_lfo_depth)) / samplerate);

    new_freq_period = samplerate / new_freq;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + ((start_x + (guint) floor(new_phase)) * source_stride);
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period)) * source_stride);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + ((guint) floor(new_phase) * source_stride);
	}else{
	  ptr_mix_buffer = mix_buffer + ((buffer_length - 1) * source_stride);
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < low_mix_buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < low_mix_buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < low_mix_buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + low_mix_buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;

    vibrato_lfo_offset += 1;
  }

  hq_pitch_util->vibrato_lfo_offset = vibrato_lfo_offset;
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    destination[i * destination_stride] = new_mix_buffer[i];
  }
}

/**
 * ags_hq_pitch_util_pitch_s16:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Pitch @hq_pitch_util of signed 16 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_pitch_s16(AgsHQPitchUtil *hq_pitch_util)
{
  AgsLinearInterpolateUtil *linear_interpolate_util;
  
  gint16 *destination, *source;
  gint16 *ptr_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint16 *mix_buffer, *new_mix_buffer, *low_mix_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble volume;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint vibrato_lfo_offset;

  gdouble base_freq, low_freq, new_freq;
  gdouble offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, low_freq_period, new_freq_period;
  guint low_mix_buffer_length;  
  guint i, j;

  if(hq_pitch_util == NULL ||
     hq_pitch_util->destination == NULL ||
     hq_pitch_util->source == NULL ||
     hq_pitch_util->tuning == 0.0){
    return;
  }

  linear_interpolate_util = hq_pitch_util->linear_interpolate_util;
  
  destination = hq_pitch_util->destination;
  destination_stride = hq_pitch_util->destination_stride;

  source = hq_pitch_util->source;
  source_stride = hq_pitch_util->source_stride;

  buffer_length = hq_pitch_util->buffer_length;
  samplerate = hq_pitch_util->samplerate;

  /* frequency */
  base_freq = exp2((hq_pitch_util->base_key) / 12.0) * 440.0;
  new_freq = exp2((hq_pitch_util->base_key + (hq_pitch_util->tuning / 100.0))  / 12.0) * 440.0;

  low_freq = exp2((hq_pitch_util->base_key - 0.25) / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(low_freq <= 0.0){
    g_warning("rejecting pitch low freq %f <= 0.0", low_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }
  
  volume = 1.0 / base_freq * new_freq;

  vibrato_gain = hq_pitch_util->vibrato_gain;
  vibrato_lfo_depth = hq_pitch_util->vibrato_lfo_depth;
  vibrato_lfo_freq = hq_pitch_util->vibrato_lfo_freq;
  vibrato_tuning = hq_pitch_util->vibrato_tuning;
  vibrato_lfo_offset = hq_pitch_util->vibrato_lfo_offset;

  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* get buffer */
  low_mix_buffer_length = (freq_period / low_freq_period) * buffer_length;

  mix_buffer = source;

  low_mix_buffer = (gint16 *) hq_pitch_util->low_mix_buffer;

  new_mix_buffer = (gint16 *) hq_pitch_util->new_mix_buffer;

  ags_linear_interpolate_util_set_destination(linear_interpolate_util,
					      low_mix_buffer);
  
  ags_linear_interpolate_util_set_source(linear_interpolate_util,
					 source);

  ags_linear_interpolate_util_set_buffer_length(linear_interpolate_util,
						buffer_length);

  ags_linear_interpolate_util_set_format(linear_interpolate_util,
					 AGS_SOUNDCARD_SIGNED_16_BIT);
  
  ags_linear_interpolate_util_set_factor(linear_interpolate_util,
					 freq_period / low_freq_period);
  
  ags_linear_interpolate_util_pitch(linear_interpolate_util);

  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint16 new_z;
    gdouble phase, low_phase, new_phase;    
    guint start_x, low_start_x;

    new_freq = (exp2((hq_pitch_util->base_key + (hq_pitch_util->tuning / 100.0)) / 12.0) * 440.0) * sin((vibrato_lfo_offset + i) * 2.0 * M_PI * vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0 + sin((vibrato_lfo_offset + i) * 2.0 * M_PI * vibrato_lfo_freq / samplerate) * vibrato_lfo_depth)) / samplerate);

    new_freq_period = samplerate / new_freq;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + ((start_x + (guint) floor(new_phase)) * source_stride);
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period)) * source_stride);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + ((guint) floor(new_phase) * source_stride);
	}else{
	  ptr_mix_buffer = mix_buffer + ((buffer_length - 1) * source_stride);
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < low_mix_buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < low_mix_buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < low_mix_buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + low_mix_buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;

    vibrato_lfo_offset += 1;
  }

  hq_pitch_util->vibrato_lfo_offset = vibrato_lfo_offset;
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    destination[i * destination_stride] = new_mix_buffer[i];
  }
}

/**
 * ags_hq_pitch_util_pitch_s24:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Pitch @hq_pitch_util of signed 24 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_pitch_s24(AgsHQPitchUtil *hq_pitch_util)
{
  AgsLinearInterpolateUtil *linear_interpolate_util;
  
  gint32 *destination, *source;
  gint32 *ptr_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint32 *mix_buffer, *new_mix_buffer, *low_mix_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble volume;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint vibrato_lfo_offset;

  gdouble base_freq, low_freq, new_freq;
  gdouble offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, low_freq_period, new_freq_period;
  guint low_mix_buffer_length;  
  guint i, j;

  if(hq_pitch_util == NULL ||
     hq_pitch_util->destination == NULL ||
     hq_pitch_util->source == NULL ||
     hq_pitch_util->tuning == 0.0){
    return;
  }

  linear_interpolate_util = hq_pitch_util->linear_interpolate_util;
  
  destination = hq_pitch_util->destination;
  destination_stride = hq_pitch_util->destination_stride;

  source = hq_pitch_util->source;
  source_stride = hq_pitch_util->source_stride;

  buffer_length = hq_pitch_util->buffer_length;
  samplerate = hq_pitch_util->samplerate;

  /* frequency */
  base_freq = exp2((hq_pitch_util->base_key) / 12.0) * 440.0;
  new_freq = exp2((hq_pitch_util->base_key + (hq_pitch_util->tuning / 100.0))  / 12.0) * 440.0;

  low_freq = exp2((hq_pitch_util->base_key - 0.25) / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(low_freq <= 0.0){
    g_warning("rejecting pitch low freq %f <= 0.0", low_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }
  
  volume = 1.0 / base_freq * new_freq;

  vibrato_gain = hq_pitch_util->vibrato_gain;
  vibrato_lfo_depth = hq_pitch_util->vibrato_lfo_depth;
  vibrato_lfo_freq = hq_pitch_util->vibrato_lfo_freq;
  vibrato_tuning = hq_pitch_util->vibrato_tuning;
  vibrato_lfo_offset = hq_pitch_util->vibrato_lfo_offset;

  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* get buffer */
  low_mix_buffer_length = (freq_period / low_freq_period) * buffer_length;

  mix_buffer = source;

  low_mix_buffer = (gint32 *) hq_pitch_util->low_mix_buffer;

  new_mix_buffer = (gint32 *) hq_pitch_util->new_mix_buffer;

  ags_linear_interpolate_util_set_destination(linear_interpolate_util,
					      low_mix_buffer);
  
  ags_linear_interpolate_util_set_source(linear_interpolate_util,
					 source);

  ags_linear_interpolate_util_set_buffer_length(linear_interpolate_util,
						buffer_length);

  ags_linear_interpolate_util_set_format(linear_interpolate_util,
					 AGS_SOUNDCARD_SIGNED_24_BIT);
  
  ags_linear_interpolate_util_set_factor(linear_interpolate_util,
					 freq_period / low_freq_period);
  
  ags_linear_interpolate_util_pitch(linear_interpolate_util);

  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 new_z;
    gdouble phase, low_phase, new_phase;    
    guint start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + ((start_x + (guint) floor(new_phase)) * source_stride);
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period)) * source_stride);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + ((guint) floor(new_phase) * source_stride);
	}else{
	  ptr_mix_buffer = mix_buffer + ((buffer_length - 1) * source_stride);
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < low_mix_buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < low_mix_buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < low_mix_buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + low_mix_buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;

    vibrato_lfo_offset += 1;
  }

  hq_pitch_util->vibrato_lfo_offset = vibrato_lfo_offset;
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    destination[i * destination_stride] = new_mix_buffer[i];
  }
}

/**
 * ags_hq_pitch_util_pitch_s32:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Pitch @hq_pitch_util of signed 32 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_pitch_s32(AgsHQPitchUtil *hq_pitch_util)
{
  AgsLinearInterpolateUtil *linear_interpolate_util;
  
  gint32 *destination, *source;
  gint32 *ptr_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint32 *mix_buffer, *new_mix_buffer, *low_mix_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble volume;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint vibrato_lfo_offset;

  gdouble base_freq, low_freq, new_freq;
  gdouble offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, low_freq_period, new_freq_period;
  guint low_mix_buffer_length;  
  guint i, j;

  if(hq_pitch_util == NULL ||
     hq_pitch_util->destination == NULL ||
     hq_pitch_util->source == NULL ||
     hq_pitch_util->tuning == 0.0){
    return;
  }

  linear_interpolate_util = hq_pitch_util->linear_interpolate_util;
  
  destination = hq_pitch_util->destination;
  destination_stride = hq_pitch_util->destination_stride;

  source = hq_pitch_util->source;
  source_stride = hq_pitch_util->source_stride;

  buffer_length = hq_pitch_util->buffer_length;
  samplerate = hq_pitch_util->samplerate;

  /* frequency */
  base_freq = exp2((hq_pitch_util->base_key) / 12.0) * 440.0;
  new_freq = exp2((hq_pitch_util->base_key + (hq_pitch_util->tuning / 100.0))  / 12.0) * 440.0;

  low_freq = exp2((hq_pitch_util->base_key - 0.25) / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(low_freq <= 0.0){
    g_warning("rejecting pitch low freq %f <= 0.0", low_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }
  
  volume = 1.0 / base_freq * new_freq;

  vibrato_gain = hq_pitch_util->vibrato_gain;
  vibrato_lfo_depth = hq_pitch_util->vibrato_lfo_depth;
  vibrato_lfo_freq = hq_pitch_util->vibrato_lfo_freq;
  vibrato_tuning = hq_pitch_util->vibrato_tuning;
  vibrato_lfo_offset = hq_pitch_util->vibrato_lfo_offset;

  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* get buffer */
  low_mix_buffer_length = (freq_period / low_freq_period) * buffer_length;

  mix_buffer = source;

  low_mix_buffer = (gint32 *) hq_pitch_util->low_mix_buffer;

  new_mix_buffer = (gint32 *) hq_pitch_util->new_mix_buffer;

  ags_linear_interpolate_util_set_destination(linear_interpolate_util,
					      low_mix_buffer);
  
  ags_linear_interpolate_util_set_source(linear_interpolate_util,
					 source);

  ags_linear_interpolate_util_set_buffer_length(linear_interpolate_util,
						buffer_length);

  ags_linear_interpolate_util_set_format(linear_interpolate_util,
					 AGS_SOUNDCARD_SIGNED_32_BIT);
  
  ags_linear_interpolate_util_set_factor(linear_interpolate_util,
					 freq_period / low_freq_period);
  
  ags_linear_interpolate_util_pitch(linear_interpolate_util);

  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 new_z;
    gdouble phase, low_phase, new_phase;    
    guint start_x, low_start_x;

    new_freq = (exp2((hq_pitch_util->base_key + (hq_pitch_util->tuning / 100.0)) / 12.0) * 440.0) * sin((vibrato_lfo_offset + i) * 2.0 * M_PI * vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0 + sin((vibrato_lfo_offset + i) * 2.0 * M_PI * vibrato_lfo_freq / samplerate) * vibrato_lfo_depth)) / samplerate);

    new_freq_period = samplerate / new_freq;


    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + ((start_x + (guint) floor(new_phase)) * source_stride);
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period)) * source_stride);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + ((guint) floor(new_phase) * source_stride);
	}else{
	  ptr_mix_buffer = mix_buffer + ((buffer_length - 1) * source_stride);
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < low_mix_buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < low_mix_buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < low_mix_buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + low_mix_buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;

    vibrato_lfo_offset += 1;
  }

  hq_pitch_util->vibrato_lfo_offset = vibrato_lfo_offset;
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    destination[i * destination_stride] = new_mix_buffer[i];
  }
}

/**
 * ags_hq_pitch_util_pitch_s64:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Pitch @hq_pitch_util of signed 64 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_pitch_s64(AgsHQPitchUtil *hq_pitch_util)
{
  AgsLinearInterpolateUtil *linear_interpolate_util;
  
  gint64 *destination, *source;
  gint64 *ptr_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint64 *mix_buffer, *new_mix_buffer, *low_mix_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble volume;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint vibrato_lfo_offset;

  gdouble base_freq, low_freq, new_freq;
  gdouble offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, low_freq_period, new_freq_period;
  guint low_mix_buffer_length;  
  guint i, j;

  if(hq_pitch_util == NULL ||
     hq_pitch_util->destination == NULL ||
     hq_pitch_util->source == NULL ||
     hq_pitch_util->tuning == 0.0){
    return;
  }

  linear_interpolate_util = hq_pitch_util->linear_interpolate_util;
  
  destination = hq_pitch_util->destination;
  destination_stride = hq_pitch_util->destination_stride;

  source = hq_pitch_util->source;
  source_stride = hq_pitch_util->source_stride;

  buffer_length = hq_pitch_util->buffer_length;
  samplerate = hq_pitch_util->samplerate;

  /* frequency */
  base_freq = exp2((hq_pitch_util->base_key) / 12.0) * 440.0;
  new_freq = exp2((hq_pitch_util->base_key + (hq_pitch_util->tuning / 100.0))  / 12.0) * 440.0;

  low_freq = exp2((hq_pitch_util->base_key - 0.25) / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(low_freq <= 0.0){
    g_warning("rejecting pitch low freq %f <= 0.0", low_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }
  
  volume = 1.0 / base_freq * new_freq;

  vibrato_gain = hq_pitch_util->vibrato_gain;
  vibrato_lfo_depth = hq_pitch_util->vibrato_lfo_depth;
  vibrato_lfo_freq = hq_pitch_util->vibrato_lfo_freq;
  vibrato_tuning = hq_pitch_util->vibrato_tuning;
  vibrato_lfo_offset = hq_pitch_util->vibrato_lfo_offset;

  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* get buffer */
  low_mix_buffer_length = (freq_period / low_freq_period) * buffer_length;

  mix_buffer = source;

  low_mix_buffer = (gint64 *) hq_pitch_util->low_mix_buffer;

  new_mix_buffer = (gint64 *) hq_pitch_util->new_mix_buffer;

  ags_linear_interpolate_util_set_destination(linear_interpolate_util,
					      low_mix_buffer);
  
  ags_linear_interpolate_util_set_source(linear_interpolate_util,
					 source);

  ags_linear_interpolate_util_set_buffer_length(linear_interpolate_util,
						buffer_length);

  ags_linear_interpolate_util_set_format(linear_interpolate_util,
					 AGS_SOUNDCARD_SIGNED_64_BIT);
  
  ags_linear_interpolate_util_set_factor(linear_interpolate_util,
					 freq_period / low_freq_period);
  
  ags_linear_interpolate_util_pitch(linear_interpolate_util);

  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint64 new_z;
    gdouble phase, low_phase, new_phase;    
    guint start_x, low_start_x;

    new_freq = (exp2((hq_pitch_util->base_key + (hq_pitch_util->tuning / 100.0)) / 12.0) * 440.0) * sin((vibrato_lfo_offset + i) * 2.0 * M_PI * vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0 + sin((vibrato_lfo_offset + i) * 2.0 * M_PI * vibrato_lfo_freq / samplerate) * vibrato_lfo_depth)) / samplerate);

    new_freq_period = samplerate / new_freq;


    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + ((start_x + (guint) floor(new_phase)) * source_stride);
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period)) * source_stride);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + ((guint) floor(new_phase) * source_stride);
	}else{
	  ptr_mix_buffer = mix_buffer + ((buffer_length - 1) * source_stride);
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < low_mix_buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < low_mix_buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < low_mix_buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + low_mix_buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;

    vibrato_lfo_offset += 1;
  }

  hq_pitch_util->vibrato_lfo_offset = vibrato_lfo_offset;
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    destination[i * destination_stride] = new_mix_buffer[i];
  }
}

/**
 * ags_hq_pitch_util_pitch_float:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Pitch @hq_pitch_util of floating point data.
 * 
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_pitch_float(AgsHQPitchUtil *hq_pitch_util)
{
  AgsLinearInterpolateUtil *linear_interpolate_util;
  
  gfloat *destination, *source;
  gfloat *ptr_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gfloat *mix_buffer, *new_mix_buffer, *low_mix_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble volume;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint vibrato_lfo_offset;

  gdouble base_freq, low_freq, new_freq;
  gdouble offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, low_freq_period, new_freq_period;
  guint low_mix_buffer_length;  
  guint i, j;

  if(hq_pitch_util == NULL ||
     hq_pitch_util->destination == NULL ||
     hq_pitch_util->source == NULL ||
     hq_pitch_util->tuning == 0.0){
    return;
  }

  linear_interpolate_util = hq_pitch_util->linear_interpolate_util;
  
  destination = hq_pitch_util->destination;
  destination_stride = hq_pitch_util->destination_stride;

  source = hq_pitch_util->source;
  source_stride = hq_pitch_util->source_stride;

  buffer_length = hq_pitch_util->buffer_length;
  samplerate = hq_pitch_util->samplerate;

  /* frequency */
  base_freq = exp2((hq_pitch_util->base_key) / 12.0) * 440.0;
  new_freq = exp2((hq_pitch_util->base_key + (hq_pitch_util->tuning / 100.0))  / 12.0) * 440.0;

  low_freq = exp2((hq_pitch_util->base_key - 0.25) / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(low_freq <= 0.0){
    g_warning("rejecting pitch low freq %f <= 0.0", low_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }
  
  volume = 1.0 / base_freq * new_freq;

  vibrato_gain = hq_pitch_util->vibrato_gain;
  vibrato_lfo_depth = hq_pitch_util->vibrato_lfo_depth;
  vibrato_lfo_freq = hq_pitch_util->vibrato_lfo_freq;
  vibrato_tuning = hq_pitch_util->vibrato_tuning;
  vibrato_lfo_offset = hq_pitch_util->vibrato_lfo_offset;

  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* get buffer */
  low_mix_buffer_length = (freq_period / low_freq_period) * buffer_length;

  mix_buffer = source;

  low_mix_buffer = (gfloat *) hq_pitch_util->low_mix_buffer;

  new_mix_buffer = (gfloat *) hq_pitch_util->new_mix_buffer;

  ags_linear_interpolate_util_set_destination(linear_interpolate_util,
					      low_mix_buffer);
  
  ags_linear_interpolate_util_set_source(linear_interpolate_util,
					 source);

  ags_linear_interpolate_util_set_buffer_length(linear_interpolate_util,
						buffer_length);

  ags_linear_interpolate_util_set_format(linear_interpolate_util,
					 AGS_SOUNDCARD_FLOAT);
  
  ags_linear_interpolate_util_set_factor(linear_interpolate_util,
					 freq_period / low_freq_period);
  
  ags_linear_interpolate_util_pitch(linear_interpolate_util);

  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gfloat new_z;
    gdouble phase, low_phase, new_phase;    
    guint start_x, low_start_x;

    new_freq = (exp2((hq_pitch_util->base_key + (hq_pitch_util->tuning / 100.0)) / 12.0) * 440.0) * sin((vibrato_lfo_offset + i) * 2.0 * M_PI * vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0 + sin((vibrato_lfo_offset + i) * 2.0 * M_PI * vibrato_lfo_freq / samplerate) * vibrato_lfo_depth)) / samplerate);

    new_freq_period = samplerate / new_freq;


    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + ((start_x + (guint) floor(new_phase)) * source_stride);
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period)) * source_stride);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + ((guint) floor(new_phase) * source_stride);
	}else{
	  ptr_mix_buffer = mix_buffer + ((buffer_length - 1) * source_stride);
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < low_mix_buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < low_mix_buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < low_mix_buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + low_mix_buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;

    vibrato_lfo_offset += 1;
  }

  hq_pitch_util->vibrato_lfo_offset = vibrato_lfo_offset;
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    destination[i * destination_stride] = new_mix_buffer[i];
  }
}

/**
 * ags_hq_pitch_util_pitch_double:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Pitch @hq_pitch_util of double precision floating point data.
 * 
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_pitch_double(AgsHQPitchUtil *hq_pitch_util)
{
  AgsLinearInterpolateUtil *linear_interpolate_util;
  
  gdouble *destination, *source;
  gdouble *ptr_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gdouble *mix_buffer, *new_mix_buffer, *low_mix_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble volume;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint vibrato_lfo_offset;

  gdouble base_freq, low_freq, new_freq;
  gdouble offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, low_freq_period, new_freq_period;
  guint low_mix_buffer_length;  
  guint i, j;

  if(hq_pitch_util == NULL ||
     hq_pitch_util->destination == NULL ||
     hq_pitch_util->source == NULL ||
     hq_pitch_util->tuning == 0.0){
    return;
  }

  linear_interpolate_util = hq_pitch_util->linear_interpolate_util;
  
  destination = hq_pitch_util->destination;
  destination_stride = hq_pitch_util->destination_stride;

  source = hq_pitch_util->source;
  source_stride = hq_pitch_util->source_stride;

  buffer_length = hq_pitch_util->buffer_length;
  samplerate = hq_pitch_util->samplerate;

  /* frequency */
  base_freq = exp2((hq_pitch_util->base_key) / 12.0) * 440.0;
  new_freq = exp2((hq_pitch_util->base_key + (hq_pitch_util->tuning / 100.0))  / 12.0) * 440.0;

  low_freq = exp2((hq_pitch_util->base_key - 0.25) / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(low_freq <= 0.0){
    g_warning("rejecting pitch low freq %f <= 0.0", low_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }
  
  volume = 1.0 / base_freq * new_freq;

  vibrato_gain = hq_pitch_util->vibrato_gain;
  vibrato_lfo_depth = hq_pitch_util->vibrato_lfo_depth;
  vibrato_lfo_freq = hq_pitch_util->vibrato_lfo_freq;
  vibrato_tuning = hq_pitch_util->vibrato_tuning;
  vibrato_lfo_offset = hq_pitch_util->vibrato_lfo_offset;

  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* get buffer */
  low_mix_buffer_length = (freq_period / low_freq_period) * buffer_length;

  mix_buffer = source;

  low_mix_buffer = (gdouble *) hq_pitch_util->low_mix_buffer;

  new_mix_buffer = (gdouble *) hq_pitch_util->new_mix_buffer;

  ags_linear_interpolate_util_set_destination(linear_interpolate_util,
					      low_mix_buffer);
  
  ags_linear_interpolate_util_set_source(linear_interpolate_util,
					 source);

  ags_linear_interpolate_util_set_buffer_length(linear_interpolate_util,
						buffer_length);

  ags_linear_interpolate_util_set_format(linear_interpolate_util,
					 AGS_SOUNDCARD_DOUBLE);
  
  ags_linear_interpolate_util_set_factor(linear_interpolate_util,
					 freq_period / low_freq_period);
  
  ags_linear_interpolate_util_pitch(linear_interpolate_util);

  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gdouble new_z;
    gdouble phase, low_phase, new_phase;    
    guint start_x, low_start_x;

    new_freq = (exp2((hq_pitch_util->base_key + (hq_pitch_util->tuning / 100.0)) / 12.0) * 440.0) * sin((vibrato_lfo_offset + i) * 2.0 * M_PI * vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0 + sin((vibrato_lfo_offset + i) * 2.0 * M_PI * vibrato_lfo_freq / samplerate) * vibrato_lfo_depth)) / samplerate);

    new_freq_period = samplerate / new_freq;


    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + ((start_x + (guint) floor(new_phase)) * source_stride);
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period)) * source_stride);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + ((guint) floor(new_phase) * source_stride);
	}else{
	  ptr_mix_buffer = mix_buffer + ((buffer_length - 1) * source_stride);
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < low_mix_buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < low_mix_buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < low_mix_buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + low_mix_buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;

    vibrato_lfo_offset += 1;
  }

  hq_pitch_util->vibrato_lfo_offset = vibrato_lfo_offset;
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    destination[i * destination_stride] = new_mix_buffer[i];
  }
}

/**
 * ags_hq_pitch_util_pitch_complex:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Pitch @hq_pitch_util of complex data.
 * 
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_pitch_complex(AgsHQPitchUtil *hq_pitch_util)
{
  AgsLinearInterpolateUtil *linear_interpolate_util;
  
  AgsComplex *destination, *source;
  AgsComplex *ptr_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  AgsComplex *mix_buffer, *new_mix_buffer, *low_mix_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble volume;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint vibrato_lfo_offset;

  gdouble base_freq, low_freq, new_freq;
  gdouble offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, low_freq_period, new_freq_period;
  guint low_mix_buffer_length;  
  guint i, j;

  if(hq_pitch_util == NULL ||
     hq_pitch_util->destination == NULL ||
     hq_pitch_util->source == NULL ||
     hq_pitch_util->tuning == 0.0){
    return;
  }

  linear_interpolate_util = hq_pitch_util->linear_interpolate_util;
  
  destination = hq_pitch_util->destination;
  destination_stride = hq_pitch_util->destination_stride;

  source = hq_pitch_util->source;
  source_stride = hq_pitch_util->source_stride;

  buffer_length = hq_pitch_util->buffer_length;
  samplerate = hq_pitch_util->samplerate;

  /* frequency */
  base_freq = exp2((hq_pitch_util->base_key) / 12.0) * 440.0;
  new_freq = exp2((hq_pitch_util->base_key + (hq_pitch_util->tuning / 100.0))  / 12.0) * 440.0;

  low_freq = exp2((hq_pitch_util->base_key - 0.25) / 12.0) * 440.0;
  
  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(low_freq <= 0.0){
    g_warning("rejecting pitch low freq %f <= 0.0", low_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }
  
  volume = 1.0 / base_freq * new_freq;

  vibrato_gain = hq_pitch_util->vibrato_gain;
  vibrato_lfo_depth = hq_pitch_util->vibrato_lfo_depth;
  vibrato_lfo_freq = hq_pitch_util->vibrato_lfo_freq;
  vibrato_tuning = hq_pitch_util->vibrato_tuning;
  vibrato_lfo_offset = hq_pitch_util->vibrato_lfo_offset;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* get buffer */
  low_mix_buffer_length = (freq_period / low_freq_period) * buffer_length;

  mix_buffer = source;

  low_mix_buffer = (AgsComplex *) hq_pitch_util->low_mix_buffer;

  new_mix_buffer = (AgsComplex *) hq_pitch_util->new_mix_buffer;

  ags_linear_interpolate_util_set_destination(linear_interpolate_util,
					      low_mix_buffer);
  
  ags_linear_interpolate_util_set_source(linear_interpolate_util,
					 source);

  ags_linear_interpolate_util_set_buffer_length(linear_interpolate_util,
						buffer_length);

  ags_linear_interpolate_util_set_format(linear_interpolate_util,
					 AGS_SOUNDCARD_COMPLEX);
  
  ags_linear_interpolate_util_set_factor(linear_interpolate_util,
					 freq_period / low_freq_period);
  
  ags_linear_interpolate_util_pitch(linear_interpolate_util);

  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    double _Complex new_z;
    gdouble phase, low_phase, new_phase;    
    guint start_x, low_start_x;

    new_freq = (exp2((hq_pitch_util->base_key + (hq_pitch_util->tuning / 100.0)) / 12.0) * 440.0) * sin((vibrato_lfo_offset + i) * 2.0 * M_PI * vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0 + sin((vibrato_lfo_offset + i) * 2.0 * M_PI * vibrato_lfo_freq / samplerate) * vibrato_lfo_depth)) / samplerate);

    new_freq_period = samplerate / new_freq;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + ((start_x + (guint) floor(new_phase)) * source_stride);
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period)) * source_stride);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + ((guint) floor(new_phase) * source_stride);
	}else{
	  ptr_mix_buffer = mix_buffer + ((buffer_length - 1) * source_stride);
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < low_mix_buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < low_mix_buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < low_mix_buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + low_mix_buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0].real != 0.0){
      new_z = volume * ((new_freq_period * (ags_complex_get(ptr_mix_buffer) / freq_period) * (ags_complex_get(ptr_low_mix_buffer) / low_freq_period) / (ags_complex_get(ptr_mix_buffer) / freq_period)));
    }else{
      new_z = 0;
    }
    
    ags_complex_set(ptr_new_mix_buffer,
		    new_z);

    vibrato_lfo_offset += 1;
  }

  hq_pitch_util->vibrato_lfo_offset = vibrato_lfo_offset;
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ags_complex_set(destination + (i * destination_stride),
		    ags_complex_get(new_mix_buffer + i));
  }
}

/**
 * ags_hq_pitch_util_pitch:
 * @hq_pitch_util: the #AgsHQPitchUtil-struct
 * 
 * Pitch @hq_pitch_util.
 * 
 * Since: 3.9.6
 */
void
ags_hq_pitch_util_pitch(AgsHQPitchUtil *hq_pitch_util)
{
  if(hq_pitch_util == NULL ||
     hq_pitch_util->destination == NULL ||
     hq_pitch_util->source == NULL){
    return;
  }

  switch(hq_pitch_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_hq_pitch_util_pitch_s8(hq_pitch_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_hq_pitch_util_pitch_s16(hq_pitch_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_hq_pitch_util_pitch_s24(hq_pitch_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_hq_pitch_util_pitch_s32(hq_pitch_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_hq_pitch_util_pitch_s64(hq_pitch_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_hq_pitch_util_pitch_float(hq_pitch_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_hq_pitch_util_pitch_double(hq_pitch_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_hq_pitch_util_pitch_complex(hq_pitch_util);
  }
  break;
  default:
    g_warning("unknown format");
  }
}
