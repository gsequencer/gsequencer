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

#include <ags/audio/ags_fast_pitch_util.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_fourier_transform_util.h>

/**
 * SECTION:ags_fast_pitch_util
 * @short_description: fast pitch util
 * @title: AgsFastPitchUtil
 * @section_id:
 * @include: ags/audio/ags_fast_pitch_util.h
 *
 * Utility functions to pitch.
 */

GType
ags_fast_pitch_util_get_type(void)
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fast_pitch_util = 0;

    ags_type_fast_pitch_util =
      g_boxed_type_register_static("AgsFastPitchUtil",
				   (GBoxedCopyFunc) ags_fast_pitch_util_copy,
				   (GBoxedFreeFunc) ags_fast_pitch_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fast_pitch_util);
  }

  return g_define_type_id__volatile;
}


/**
 * ags_fast_pitch_util_alloc:
 * 
 * Allocate #AgsFastPitchUtil-struct.
 * 
 * Returns: the newly allocated #AgsFastPitchUtil-struct
 * 
 * Since: 3.9.6
 */
AgsFastPitchUtil*
ags_fast_pitch_util_alloc()
{
  AgsFastPitchUtil *ptr;
  
  ptr = (AgsFastPitchUtil *) g_new(AgsFastPitchUtil,
				   1);

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->destination = NULL;
  ptr->destination_stride = 1;

  ptr->mix_buffer = ags_stream_alloc(AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE,
				     AGS_SOUNDCARD_DEFAULT_FORMAT);

  ptr->mix_buffer_max_buffer_length = AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE;
  
  ptr->im_mix_buffer = ags_stream_alloc(AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE,
					AGS_SOUNDCARD_DEFAULT_FORMAT);  

  ptr->im_mix_buffer_max_buffer_length = AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE;

  ptr->low_mix_buffer = ags_stream_alloc(AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE,
					 AGS_SOUNDCARD_DEFAULT_FORMAT);

  ptr->low_mix_buffer_max_buffer_length = AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE;

  ptr->new_mix_buffer = ags_stream_alloc(AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE,
					 AGS_SOUNDCARD_DEFAULT_FORMAT);

  ptr->new_mix_buffer_max_buffer_length = AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE;

  
  ptr->buffer_length = 0;
  ptr->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  ptr->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  ptr->base_key = 0.0;
  ptr->tuning = 0.0;

  ptr->vibrato_enabled = TRUE;

  ptr->vibrato_gain = 1.0;
  ptr->vibrato_lfo_depth = 0.0;
  ptr->vibrato_lfo_freq = 8.172;
  ptr->vibrato_tuning = 0.0;

  ptr->frame_count = ptr->samplerate / ptr->vibrato_lfo_freq;
  ptr->offset = 0;

  ptr->note_256th_mode = TRUE;

  ptr->offset_256th = 0;
  
  return(ptr);
}

/**
 * ags_fast_pitch_util_copy:
 * @ptr: the original #AgsFastPitchUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsFastPitchUtil-struct
 *
 * Since: 3.9.6
 */
gpointer
ags_fast_pitch_util_copy(AgsFastPitchUtil *ptr)
{
  AgsFastPitchUtil *new_ptr;

  g_return_val_if_fail(ptr != NULL, NULL);
  
  new_ptr = (AgsFastPitchUtil *) g_new(AgsFastPitchUtil,
				       1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->mix_buffer = ags_stream_alloc(AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE,
					 ptr->format);

  new_ptr->mix_buffer_max_buffer_length = AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE;
  
  new_ptr->im_mix_buffer = ags_stream_alloc(AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE,
					    ptr->format);  

  new_ptr->im_mix_buffer_max_buffer_length = AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE;

  new_ptr->low_mix_buffer = ags_stream_alloc(AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE,
					     ptr->format);

  new_ptr->low_mix_buffer_max_buffer_length = AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE;

  new_ptr->new_mix_buffer = ags_stream_alloc(AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE,
					     ptr->format);

  new_ptr->new_mix_buffer_max_buffer_length = AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->base_key = ptr->base_key;
  new_ptr->tuning = ptr->tuning;

  new_ptr->vibrato_enabled = ptr->vibrato_enabled;

  new_ptr->vibrato_gain = ptr->vibrato_gain;
  new_ptr->vibrato_lfo_depth = ptr->vibrato_lfo_depth;
  new_ptr->vibrato_lfo_freq = ptr->vibrato_lfo_freq;
  new_ptr->vibrato_tuning = ptr->vibrato_tuning;

  new_ptr->frame_count = ptr->frame_count;
  new_ptr->offset = ptr->offset;

  new_ptr->note_256th_mode = ptr->note_256th_mode;

  new_ptr->offset_256th = ptr->offset_256th;
  
  return(new_ptr);
}

/**
 * ags_fast_pitch_util_free:
 * @ptr: the #AgsFastPitchUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_free(AgsFastPitchUtil *ptr)
{
  g_return_if_fail(ptr != NULL);

  ags_stream_free(ptr->mix_buffer);
  ags_stream_free(ptr->im_mix_buffer);
  ags_stream_free(ptr->low_mix_buffer);
  ags_stream_free(ptr->new_mix_buffer);
  
  g_free(ptr);
}

/**
 * ags_fast_pitch_util_get_destination:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Get destination buffer of @fast_pitch_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 3.9.6
 */
gpointer
ags_fast_pitch_util_get_destination(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL){
    return(NULL);
  }

  return(fast_pitch_util->destination);
}

/**
 * ags_fast_pitch_util_set_destination:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * @destination: (transfer none): the destination buffer
 *
 * Set @destination buffer of @fast_pitch_util.
 *
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_set_destination(AgsFastPitchUtil *fast_pitch_util,
				    gpointer destination)
{
  if(fast_pitch_util == NULL){
    return;
  }

  fast_pitch_util->destination = destination;
}

/**
 * ags_fast_pitch_util_get_destination_stride:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Get destination stride of @fast_pitch_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 3.9.6
 */
guint
ags_fast_pitch_util_get_destination_stride(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL){
    return(0);
  }

  return(fast_pitch_util->destination_stride);
}

/**
 * ags_fast_pitch_util_set_destination_stride:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @fast_pitch_util.
 *
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_set_destination_stride(AgsFastPitchUtil *fast_pitch_util,
					   guint destination_stride)
{
  if(fast_pitch_util == NULL){
    return;
  }

  fast_pitch_util->destination_stride = destination_stride;
}

/**
 * ags_fast_pitch_util_get_source:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Get source buffer of @fast_pitch_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.9.6
 */
gpointer
ags_fast_pitch_util_get_source(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL){
    return(NULL);
  }

  return(fast_pitch_util->source);
}

/**
 * ags_fast_pitch_util_set_source:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * @source: (transfer none): the source buffer
 *
 * Set @source buffer of @fast_pitch_util.
 *
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_set_source(AgsFastPitchUtil *fast_pitch_util,
			       gpointer source)
{
  if(fast_pitch_util == NULL){
    return;
  }

  fast_pitch_util->source = source;
}

/**
 * ags_fast_pitch_util_get_source_stride:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Get source stride of @fast_pitch_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.9.6
 */
guint
ags_fast_pitch_util_get_source_stride(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL){
    return(0);
  }

  return(fast_pitch_util->source_stride);
}

/**
 * ags_fast_pitch_util_set_source_stride:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @fast_pitch_util.
 *
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_set_source_stride(AgsFastPitchUtil *fast_pitch_util,
				      guint source_stride)
{
  if(fast_pitch_util == NULL){
    return;
  }

  fast_pitch_util->source_stride = source_stride;
}

/**
 * ags_fast_pitch_util_get_buffer_length:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Get buffer length of @fast_pitch_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.9.6
 */
guint
ags_fast_pitch_util_get_buffer_length(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL){
    return(0);
  }

  return(fast_pitch_util->buffer_length);
}

/**
 * ags_fast_pitch_util_set_buffer_length:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @fast_pitch_util.
 *
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_set_buffer_length(AgsFastPitchUtil *fast_pitch_util,
				      guint buffer_length)
{
  if(fast_pitch_util == NULL ||
     fast_pitch_util->buffer_length == buffer_length){
    return;
  }

  fast_pitch_util->buffer_length = buffer_length;
}

/**
 * ags_fast_pitch_util_get_format:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Get format of @fast_pitch_util.
 * 
 * Returns: the format
 * 
 * Since: 3.9.6
 */
guint
ags_fast_pitch_util_get_format(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL){
    return(0);
  }

  return(fast_pitch_util->format);
}

/**
 * ags_fast_pitch_util_set_format:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * @format: the format
 *
 * Set @format of @fast_pitch_util.
 *
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_set_format(AgsFastPitchUtil *fast_pitch_util,
			       guint format)
{
  if(fast_pitch_util == NULL ||
     fast_pitch_util->format == format){
    return;
  }

  fast_pitch_util->format = format;

  ags_stream_free(fast_pitch_util->mix_buffer);
  ags_stream_free(fast_pitch_util->im_mix_buffer);
  ags_stream_free(fast_pitch_util->low_mix_buffer);
  ags_stream_free(fast_pitch_util->new_mix_buffer);

  fast_pitch_util->mix_buffer = ags_stream_alloc(AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE,
						 format);
    
  fast_pitch_util->im_mix_buffer = ags_stream_alloc(AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE,
						    format);
    
  fast_pitch_util->low_mix_buffer = ags_stream_alloc(AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE,
						     format);
    
  fast_pitch_util->new_mix_buffer = ags_stream_alloc(AGS_FAST_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE,
						     format);
}

/**
 * ags_fast_pitch_util_get_samplerate:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Get samplerate of @fast_pitch_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.9.6
 */
guint
ags_fast_pitch_util_get_samplerate(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL){
    return(0);
  }

  return(fast_pitch_util->samplerate);
}

/**
 * ags_fast_pitch_util_set_samplerate:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @fast_pitch_util.
 *
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_set_samplerate(AgsFastPitchUtil *fast_pitch_util,
				   guint samplerate)
{
  if(fast_pitch_util == NULL ||
     fast_pitch_util->samplerate == samplerate){
    return;
  }

  fast_pitch_util->samplerate = samplerate;
}

/**
 * ags_fast_pitch_util_get_base_key:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Get base key of @fast_pitch_util.
 * 
 * Returns: the base key
 * 
 * Since: 3.9.6
 */
gdouble
ags_fast_pitch_util_get_base_key(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL){
    return(0.0);
  }

  return(fast_pitch_util->base_key);
}

/**
 * ags_fast_pitch_util_set_base_key:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * @base_key: the base key
 *
 * Set @base_key of @fast_pitch_util.
 *
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_set_base_key(AgsFastPitchUtil *fast_pitch_util,
				 gdouble base_key)
{
  if(fast_pitch_util == NULL){
    return;
  }

  fast_pitch_util->base_key = base_key;
}

/**
 * ags_fast_pitch_util_get_tuning:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Get tuning of @fast_pitch_util.
 * 
 * Returns: the tuning
 * 
 * Since: 3.9.6
 */
gdouble
ags_fast_pitch_util_get_tuning(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL){
    return(0.0);
  }

  return(fast_pitch_util->tuning);
}

/**
 * ags_fast_pitch_util_set_tuning:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * @tuning: the tuning
 *
 * Set @tuning of @fast_pitch_util.
 *
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_set_tuning(AgsFastPitchUtil *fast_pitch_util,
			       gdouble tuning)
{
  if(fast_pitch_util == NULL){
    return;
  }

  fast_pitch_util->tuning = tuning;
}


/**
 * ags_fast_pitch_util_get_vibrato_enabled:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Get vibrato enabled of @fast_pitch_util.
 * 
 * Returns: %TRUE if the vibrato is enabled, otherwise %FALSE
 * 
 * Since: 5.2.0
 */
gboolean
ags_fast_pitch_util_get_vibrato_enabled(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL){
    return(FALSE);
  }

  return(fast_pitch_util->vibrato_enabled);
}

/**
 * ags_fast_pitch_util_set_vibrato_enabled:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * @vibrato_enabled: the vibrato enabled
 *
 * Set @vibrato_enabled of @fast_pitch_util.
 *
 * Since: 5.2.0
 */
void
ags_fast_pitch_util_set_vibrato_enabled(AgsFastPitchUtil *fast_pitch_util,
					gboolean vibrato_enabled)
{
  if(fast_pitch_util == NULL){
    return;
  }

  fast_pitch_util->vibrato_enabled = vibrato_enabled;
}

/**
 * ags_fast_pitch_util_get_vibrato_gain:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Get vibrato gain of @fast_pitch_util.
 * 
 * Returns: the vibrato gain
 * 
 * Since: 5.2.0
 */
gdouble
ags_fast_pitch_util_get_vibrato_gain(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL){
    return(0.0);
  }

  return(fast_pitch_util->vibrato_gain);
}

/**
 * ags_fast_pitch_util_set_vibrato_gain:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * @vibrato_gain: the vibrato gain
 *
 * Set @vibrato_gain of @fast_pitch_util.
 *
 * Since: 5.2.0
 */
void
ags_fast_pitch_util_set_vibrato_gain(AgsFastPitchUtil *fast_pitch_util,
				     gdouble vibrato_gain)
{
  if(fast_pitch_util == NULL){
    return;
  }

  fast_pitch_util->vibrato_gain = vibrato_gain;
}

/**
 * ags_fast_pitch_util_get_vibrato_lfo_depth:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Get vibrato lfo_depth of @fast_pitch_util.
 * 
 * Returns: the vibrato lfo_depth
 * 
 * Since: 5.2.0
 */
gdouble
ags_fast_pitch_util_get_vibrato_lfo_depth(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL){
    return(0.0);
  }

  return(fast_pitch_util->vibrato_lfo_depth);
}

/**
 * ags_fast_pitch_util_set_vibrato_lfo_depth:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * @vibrato_lfo_depth: the vibrato lfo_depth
 *
 * Set @vibrato_lfo_depth of @fast_pitch_util.
 *
 * Since: 5.2.0
 */
void
ags_fast_pitch_util_set_vibrato_lfo_depth(AgsFastPitchUtil *fast_pitch_util,
					  gdouble vibrato_lfo_depth)
{
  if(fast_pitch_util == NULL){
    return;
  }

  fast_pitch_util->vibrato_lfo_depth = vibrato_lfo_depth;
}

/**
 * ags_fast_pitch_util_get_vibrato_lfo_freq:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Get vibrato lfo_freq of @fast_pitch_util.
 * 
 * Returns: the vibrato lfo_freq
 * 
 * Since: 5.2.0
 */
gdouble
ags_fast_pitch_util_get_vibrato_lfo_freq(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL){
    return(0.0);
  }

  return(fast_pitch_util->vibrato_lfo_freq);
}

/**
 * ags_fast_pitch_util_set_vibrato_lfo_freq:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * @vibrato_lfo_freq: the vibrato lfo_freq
 *
 * Set @vibrato_lfo_freq of @fast_pitch_util.
 *
 * Since: 5.2.0
 */
void
ags_fast_pitch_util_set_vibrato_lfo_freq(AgsFastPitchUtil *fast_pitch_util,
					 gdouble vibrato_lfo_freq)
{
  if(fast_pitch_util == NULL){
    return;
  }

  fast_pitch_util->vibrato_lfo_freq = vibrato_lfo_freq;
}

/**
 * ags_fast_pitch_util_get_vibrato_tuning:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Get vibrato tuning of @fast_pitch_util.
 * 
 * Returns: the vibrato tuning
 * 
 * Since: 5.2.0
 */
gdouble
ags_fast_pitch_util_get_vibrato_tuning(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL){
    return(0.0);
  }

  return(fast_pitch_util->vibrato_tuning);
}

/**
 * ags_fast_pitch_util_set_vibrato_tuning:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * @vibrato_tuning: the vibrato tuning
 *
 * Set @vibrato_tuning of @fast_pitch_util.
 *
 * Since: 5.2.0
 */
void
ags_fast_pitch_util_set_vibrato_tuning(AgsFastPitchUtil *fast_pitch_util,
				       gdouble vibrato_tuning)
{
  if(fast_pitch_util == NULL){
    return;
  }

  fast_pitch_util->vibrato_tuning = vibrato_tuning;
}

/**
 * ags_fast_pitch_util_get_frame_count:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Get frame count of @fast_pitch_util.
 * 
 * Returns: the frame count
 * 
 * Since: 6.1.0
 */
guint
ags_fast_pitch_util_get_frame_count(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL){
    return(0);
  }

  return(fast_pitch_util->frame_count);
}

/**
 * ags_fast_pitch_util_set_frame_count:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * @frame_count: the frame count
 *
 * Set @frame_count of @fast_pitch_util.
 *
 * Since: 6.1.0
 */
void
ags_fast_pitch_util_set_frame_count(AgsFastPitchUtil *fast_pitch_util,
				    guint frame_count)
{
  if(fast_pitch_util == NULL){
    return;
  }

  fast_pitch_util->frame_count = frame_count;
}

/**
 * ags_fast_pitch_util_get_offset:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Get offset of @fast_pitch_util.
 * 
 * Returns: the offset
 * 
 * Since: 6.1.0
 */
guint
ags_fast_pitch_util_get_offset(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL){
    return(0);
  }

  return(fast_pitch_util->offset);
}

/**
 * ags_fast_pitch_util_set_offset:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * @offset: the offset
 *
 * Set @offset of @fast_pitch_util.
 *
 * Since: 6.1.0
 */
void
ags_fast_pitch_util_set_offset(AgsFastPitchUtil *fast_pitch_util,
			       guint offset)
{
  if(fast_pitch_util == NULL){
    return;
  }

  fast_pitch_util->offset = offset;
}

/**
 * ags_fast_pitch_util_get_note_256th_mode:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Get note 256th mode of @fast_pitch_util.
 * 
 * Returns: %TRUE if note 256th mode, otherwise %FALSE
 * 
 * Since: 6.1.0
 */
gboolean
ags_fast_pitch_util_get_note_256th_mode(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL){
    return(0);
  }

  return(fast_pitch_util->note_256th_mode);
}

/**
 * ags_fast_pitch_util_set_note_256th_mode:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * @note_256th_mode: the note 256th mode
 *
 * Set @note_256th_mode of @fast_pitch_util.
 *
 * Since: 6.1.0
 */
void
ags_fast_pitch_util_set_note_256th_mode(AgsFastPitchUtil *fast_pitch_util,
					gboolean note_256th_mode)
{
  if(fast_pitch_util == NULL){
    return;
  }

  fast_pitch_util->note_256th_mode = note_256th_mode;
}

/**
 * ags_fast_pitch_util_get_offset_256th:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Get offset as note 256th of @fast_pitch_util.
 * 
 * Returns: the offset as note 256th
 * 
 * Since: 6.1.0
 */
guint
ags_fast_pitch_util_get_offset_256th(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL){
    return(0);
  }

  return(fast_pitch_util->offset_256th);
}

/**
 * ags_fast_pitch_util_set_offset_256th:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * @offset_256th: the offset as note 256th
 *
 * Set @offset_256th of @fast_pitch_util.
 *
 * Since: 6.1.0
 */
void
ags_fast_pitch_util_set_offset_256th(AgsFastPitchUtil *fast_pitch_util,
				     guint offset_256th)
{
  if(fast_pitch_util == NULL){
    return;
  }
  
  fast_pitch_util->offset_256th = offset_256th;
}

/**
 * ags_fast_pitch_util_pitch_s8:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Pitch @fast_pitch_util of signed 8 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_pitch_s8(AgsFastPitchUtil *fast_pitch_util)
{
  gint8 *destination, *source;
  gint8 *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint8 *mix_buffer, *im_mix_buffer, *low_mix_buffer, *new_mix_buffer;
  gint8 *ptr_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble volume;
  gdouble base_freq, im_freq, low_freq, new_freq;
  gdouble offset_factor, im_offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, low_freq_period, new_freq_period;
  gdouble t;
  guint i, j;

  if(fast_pitch_util == NULL ||
     fast_pitch_util->destination == NULL ||
     fast_pitch_util->source == NULL){
    return;
  }

  destination = fast_pitch_util->destination;
  destination_stride = fast_pitch_util->destination_stride;

  source = fast_pitch_util->source;
  source_stride = fast_pitch_util->source_stride;

  buffer_length = fast_pitch_util->buffer_length;
  samplerate = fast_pitch_util->samplerate;
  
  /* frequency */
  base_freq = exp2((fast_pitch_util->base_key) / 12.0) * 440.0;

  if(fast_pitch_util->tuning <= 0.0){
    im_freq = (1.25 * ((double) samplerate / base_freq)) * (double) samplerate;
  }else{
    im_freq = (0.75 * ((double) samplerate / base_freq)) * (double) samplerate;
  }
  
  low_freq = exp2((fast_pitch_util->base_key - 12.0) / 12.0) * 440.0;

  new_freq = exp2((fast_pitch_util->base_key + (fast_pitch_util->tuning / 100.0))  / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(im_freq <= 0.0){
    g_warning("rejecting pitch intermediate freq %f <= 0.0", im_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;
  
  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* get buffer */
  mix_buffer = (gint8 *) fast_pitch_util->mix_buffer;

  im_mix_buffer = (gint8 *) fast_pitch_util->im_mix_buffer;

  low_mix_buffer = (gint8 *) fast_pitch_util->low_mix_buffer;

  new_mix_buffer = (gint8 *) fast_pitch_util->new_mix_buffer;

  /* mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;

    /* write mix buffer */
    ptr_mix_buffer[0] = source[i * source_stride];
  }

  /* im mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint8 z, mix_z, im_z;
    gdouble phase, im_phase;
    guint start_x;

    if(j >= im_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    im_phase = fmod(i, im_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_im_mix_buffer = im_mix_buffer + i;

    /* write im mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = ((j / im_offset_factor) - j) / (freq_period - im_freq_period);
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_im_mix_buffer[0] = im_z;
  }

  /* low mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint8 z, mix_z, low_z;
    gdouble phase, low_phase;
    guint start_x;

    if(j >= low_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    low_phase = fmod(i, low_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_low_mix_buffer = low_mix_buffer + i;

    /* write low mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = low_freq_period / freq_period;
    
    low_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_low_mix_buffer[0] = low_z;
  }
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint8 new_z;
    gdouble phase, im_phase, low_phase, new_phase;    
    guint start_x, im_start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(im_freq_period) != 0.0){
      im_start_x = im_freq_period * floor((double) i / im_freq_period);
    }else{
      im_start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    im_phase = fmod(i, im_freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase));
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }

    if(im_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase));
    }else{
      if((im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) < buffer_length &&
	 (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) > 0){
	ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_im_mix_buffer = im_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_im_mix_buffer = im_mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_im_mix_buffer[0] / im_freq_period) / (ptr_mix_buffer[0] / freq_period)) + (1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = destination + (i * destination_stride);

    ptr_buffer[0] = ptr_new_mix_buffer[0];
  }
}

/**
 * ags_fast_pitch_util_pitch_s16:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Pitch @fast_pitch_util of signed 16 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_pitch_s16(AgsFastPitchUtil *fast_pitch_util)
{
  gint16 *destination, *source;
  gint16 *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint16 *mix_buffer, *im_mix_buffer, *low_mix_buffer, *new_mix_buffer;
  gint16 *ptr_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble volume;
  gdouble base_freq, im_freq, low_freq, new_freq;
  gdouble offset_factor, im_offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, low_freq_period, new_freq_period;
  gdouble t;
  guint i, j;

  if(fast_pitch_util == NULL ||
     fast_pitch_util->destination == NULL ||
     fast_pitch_util->source == NULL){
    return;
  }

  destination = fast_pitch_util->destination;
  destination_stride = fast_pitch_util->destination_stride;

  source = fast_pitch_util->source;
  source_stride = fast_pitch_util->source_stride;

  buffer_length = fast_pitch_util->buffer_length;
  samplerate = fast_pitch_util->samplerate;
  
  /* frequency */
  base_freq = exp2((fast_pitch_util->base_key) / 12.0) * 440.0;

  if(fast_pitch_util->tuning <= 0.0){
    im_freq = (1.25 * ((double) samplerate / base_freq)) * (double) samplerate;
  }else{
    im_freq = (0.75 * ((double) samplerate / base_freq)) * (double) samplerate;
  }

  low_freq = exp2((fast_pitch_util->base_key - 12.0) / 12.0) * 440.0;

  new_freq = exp2((fast_pitch_util->base_key + (fast_pitch_util->tuning / 100.0))  / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(im_freq <= 0.0){
    g_warning("rejecting pitch intermediate freq %f <= 0.0", im_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;
  
  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* get buffer */
  mix_buffer = (gint16 *) fast_pitch_util->mix_buffer;

  im_mix_buffer = (gint16 *) fast_pitch_util->im_mix_buffer;

  low_mix_buffer = (gint16 *) fast_pitch_util->low_mix_buffer;

  new_mix_buffer = (gint16 *) fast_pitch_util->new_mix_buffer;

  /* mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;

    /* write mix buffer */
    ptr_mix_buffer[0] = source[i * source_stride];
  }

  /* im mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint16 z, mix_z, im_z;
    gdouble phase, im_phase;
    guint start_x;

    if(j >= im_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    im_phase = fmod(i, im_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_im_mix_buffer = im_mix_buffer + i;

    /* write im mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = ((j / im_offset_factor) - j) / (freq_period - im_freq_period);
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_im_mix_buffer[0] = im_z;
  }

  /* low mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint16 z, mix_z, low_z;
    gdouble phase, low_phase;
    guint start_x;

    if(j >= low_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    low_phase = fmod(i, low_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_low_mix_buffer = low_mix_buffer + i;

    /* write low mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = low_freq_period / freq_period;
    
    low_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_low_mix_buffer[0] = low_z;
  }
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint16 new_z;
    gdouble phase, im_phase, low_phase, new_phase;    
    guint start_x, im_start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(im_freq_period) != 0.0){
      im_start_x = im_freq_period * floor((double) i / im_freq_period);
    }else{
      im_start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    im_phase = fmod(i, im_freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase));
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }

    if(im_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase));
    }else{
      if((im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) < buffer_length &&
	 (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) > 0){
	ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_im_mix_buffer = im_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_im_mix_buffer = im_mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_im_mix_buffer[0] / im_freq_period) / (ptr_mix_buffer[0] / freq_period)) + (1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = destination + (i * destination_stride);

    ptr_buffer[0] = ptr_new_mix_buffer[0];
  }
}

/**
 * ags_fast_pitch_util_pitch_s24:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Pitch @fast_pitch_util of signed 24 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_pitch_s24(AgsFastPitchUtil *fast_pitch_util)
{
  gint32 *destination, *source;
  gint32 *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint32 *mix_buffer, *im_mix_buffer, *low_mix_buffer, *new_mix_buffer;
  gint32 *ptr_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble volume;
  gdouble base_freq, im_freq, low_freq, new_freq;
  gdouble offset_factor, im_offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, low_freq_period, new_freq_period;
  gdouble t;
  guint i, j;

  if(fast_pitch_util == NULL ||
     fast_pitch_util->destination == NULL ||
     fast_pitch_util->source == NULL){
    return;
  }

  destination = fast_pitch_util->destination;
  destination_stride = fast_pitch_util->destination_stride;

  source = fast_pitch_util->source;
  source_stride = fast_pitch_util->source_stride;

  buffer_length = fast_pitch_util->buffer_length;
  samplerate = fast_pitch_util->samplerate;
  
  /* frequency */
  base_freq = exp2((fast_pitch_util->base_key) / 12.0) * 440.0;

  if(fast_pitch_util->tuning <= 0.0){
    im_freq = (1.25 * ((double) samplerate / base_freq)) * (double) samplerate;
  }else{
    im_freq = (0.75 * ((double) samplerate / base_freq)) * (double) samplerate;
  }

  low_freq = exp2((fast_pitch_util->base_key - 12.0) / 12.0) * 440.0;

  new_freq = exp2((fast_pitch_util->base_key + (fast_pitch_util->tuning / 100.0))  / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(im_freq <= 0.0){
    g_warning("rejecting pitch intermediate freq %f <= 0.0", im_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;
  
  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* get buffer */
  mix_buffer = (gint32 *) fast_pitch_util->mix_buffer;

  im_mix_buffer = (gint32 *) fast_pitch_util->im_mix_buffer;

  low_mix_buffer = (gint32 *) fast_pitch_util->low_mix_buffer;

  new_mix_buffer = (gint32 *) fast_pitch_util->new_mix_buffer;

  /* mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;

    /* write mix buffer */
    ptr_mix_buffer[0] = source[i * source_stride];
  }

  /* im mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 z, mix_z, im_z;
    gdouble phase, im_phase;
    guint start_x;

    if(j >= im_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    im_phase = fmod(i, im_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_im_mix_buffer = im_mix_buffer + i;

    /* write im mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = ((j / im_offset_factor) - j) / (freq_period - im_freq_period);
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_im_mix_buffer[0] = im_z;
  }

  /* low mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 z, mix_z, low_z;
    gdouble phase, low_phase;
    guint start_x;

    if(j >= low_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    low_phase = fmod(i, low_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_low_mix_buffer = low_mix_buffer + i;

    /* write low mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = low_freq_period / freq_period;
    
    low_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_low_mix_buffer[0] = low_z;
  }
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 new_z;
    gdouble phase, im_phase, low_phase, new_phase;    
    guint start_x, im_start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(im_freq_period) != 0.0){
      im_start_x = im_freq_period * floor((double) i / im_freq_period);
    }else{
      im_start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    im_phase = fmod(i, im_freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase));
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }

    if(im_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase));
    }else{
      if((im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) < buffer_length &&
	 (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) > 0){
	ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_im_mix_buffer = im_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_im_mix_buffer = im_mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_im_mix_buffer[0] / im_freq_period) / (ptr_mix_buffer[0] / freq_period)) + (1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = destination + (i * destination_stride);

    ptr_buffer[0] = ptr_new_mix_buffer[0];
  }
}

/**
 * ags_fast_pitch_util_pitch_s32:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Pitch @fast_pitch_util of signed 32 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_pitch_s32(AgsFastPitchUtil *fast_pitch_util)
{
  gint32 *destination, *source;
  gint32 *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint32 *mix_buffer, *im_mix_buffer, *low_mix_buffer, *new_mix_buffer;
  gint32 *ptr_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble volume;
  gdouble base_freq, im_freq, low_freq, new_freq;
  gdouble offset_factor, im_offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, low_freq_period, new_freq_period;
  gdouble t;
  guint i, j;

  if(fast_pitch_util == NULL ||
     fast_pitch_util->destination == NULL ||
     fast_pitch_util->source == NULL){
    return;
  }

  destination = fast_pitch_util->destination;
  destination_stride = fast_pitch_util->destination_stride;

  source = fast_pitch_util->source;
  source_stride = fast_pitch_util->source_stride;

  buffer_length = fast_pitch_util->buffer_length;
  samplerate = fast_pitch_util->samplerate;
  
  /* frequency */
  base_freq = exp2((fast_pitch_util->base_key) / 12.0) * 440.0;

  if(fast_pitch_util->tuning <= 0.0){
    im_freq = (1.25 * ((double) samplerate / base_freq)) * (double) samplerate;
  }else{
    im_freq = (0.75 * ((double) samplerate / base_freq)) * (double) samplerate;
  }

  low_freq = exp2((fast_pitch_util->base_key - 12.0) / 12.0) * 440.0;

  new_freq = exp2((fast_pitch_util->base_key + (fast_pitch_util->tuning / 100.0))  / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(im_freq <= 0.0){
    g_warning("rejecting pitch intermediate freq %f <= 0.0", im_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;
  
  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* get buffer */
  mix_buffer = (gint32 *) fast_pitch_util->mix_buffer;

  im_mix_buffer = (gint32 *) fast_pitch_util->im_mix_buffer;

  low_mix_buffer = (gint32 *) fast_pitch_util->low_mix_buffer;

  new_mix_buffer = (gint32 *) fast_pitch_util->new_mix_buffer;

  /* mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;

    /* write mix buffer */
    ptr_mix_buffer[0] = source[i * source_stride];
  }

  /* im mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 z, mix_z, im_z;
    gdouble phase, im_phase;
    guint start_x;

    if(j >= im_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    im_phase = fmod(i, im_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_im_mix_buffer = im_mix_buffer + i;

    /* write im mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = ((j / im_offset_factor) - j) / (freq_period - im_freq_period);
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_im_mix_buffer[0] = im_z;
  }

  /* low mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 z, mix_z, low_z;
    gdouble phase, low_phase;
    guint start_x;

    if(j >= low_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    low_phase = fmod(i, low_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_low_mix_buffer = low_mix_buffer + i;

    /* write low mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = low_freq_period / freq_period;
    
    low_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_low_mix_buffer[0] = low_z;
  }
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 new_z;
    gdouble phase, im_phase, low_phase, new_phase;    
    guint start_x, im_start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(im_freq_period) != 0.0){
      im_start_x = im_freq_period * floor((double) i / im_freq_period);
    }else{
      im_start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    im_phase = fmod(i, im_freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase));
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }

    if(im_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase));
    }else{
      if((im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) < buffer_length &&
	 (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) > 0){
	ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_im_mix_buffer = im_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_im_mix_buffer = im_mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_im_mix_buffer[0] / im_freq_period) / (ptr_mix_buffer[0] / freq_period)) + (1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = destination + (i * destination_stride);

    ptr_buffer[0] = ptr_new_mix_buffer[0];
  }
}

/**
 * ags_fast_pitch_util_pitch_s64:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Pitch @fast_pitch_util of signed 64 bit data.
 * 
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_pitch_s64(AgsFastPitchUtil *fast_pitch_util)
{
  gint64 *destination, *source;
  gint64 *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint64 *mix_buffer, *im_mix_buffer, *low_mix_buffer, *new_mix_buffer;
  gint64 *ptr_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble volume;
  gdouble base_freq, im_freq, low_freq, new_freq;
  gdouble offset_factor, im_offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, low_freq_period, new_freq_period;
  gdouble t;
  guint i, j;

  if(fast_pitch_util == NULL ||
     fast_pitch_util->destination == NULL ||
     fast_pitch_util->source == NULL){
    return;
  }

  destination = fast_pitch_util->destination;
  destination_stride = fast_pitch_util->destination_stride;

  source = fast_pitch_util->source;
  source_stride = fast_pitch_util->source_stride;

  buffer_length = fast_pitch_util->buffer_length;
  samplerate = fast_pitch_util->samplerate;
  
  /* frequency */
  base_freq = exp2((fast_pitch_util->base_key) / 12.0) * 440.0;

  if(fast_pitch_util->tuning <= 0.0){
    im_freq = (1.25 * ((double) samplerate / base_freq)) * (double) samplerate;
  }else{
    im_freq = (0.75 * ((double) samplerate / base_freq)) * (double) samplerate;
  }

  low_freq = exp2((fast_pitch_util->base_key - 12.0) / 12.0) * 440.0;

  new_freq = exp2((fast_pitch_util->base_key + (fast_pitch_util->tuning / 100.0))  / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(im_freq <= 0.0){
    g_warning("rejecting pitch intermediate freq %f <= 0.0", im_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;
  
  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* get buffer */
  mix_buffer = (gint64 *) fast_pitch_util->mix_buffer;

  im_mix_buffer = (gint64 *) fast_pitch_util->im_mix_buffer;

  low_mix_buffer = (gint64 *) fast_pitch_util->low_mix_buffer;

  new_mix_buffer = (gint64 *) fast_pitch_util->new_mix_buffer;

  /* mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;

    /* write mix buffer */
    ptr_mix_buffer[0] = source[i * source_stride];
  }

  /* im mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint64 z, mix_z, im_z;
    gdouble phase, im_phase;
    guint start_x;

    if(j >= im_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    im_phase = fmod(i, im_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_im_mix_buffer = im_mix_buffer + i;

    /* write im mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = ((j / im_offset_factor) - j) / (freq_period - im_freq_period);
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_im_mix_buffer[0] = im_z;
  }

  /* low mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint64 z, mix_z, low_z;
    gdouble phase, low_phase;
    guint start_x;

    if(j >= low_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    low_phase = fmod(i, low_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_low_mix_buffer = low_mix_buffer + i;

    /* write low mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = low_freq_period / freq_period;
    
    low_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_low_mix_buffer[0] = low_z;
  }
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint64 new_z;
    gdouble phase, im_phase, low_phase, new_phase;    
    guint start_x, im_start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(im_freq_period) != 0.0){
      im_start_x = im_freq_period * floor((double) i / im_freq_period);
    }else{
      im_start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    im_phase = fmod(i, im_freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase));
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }

    if(im_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase));
    }else{
      if((im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) < buffer_length &&
	 (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) > 0){
	ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_im_mix_buffer = im_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_im_mix_buffer = im_mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_im_mix_buffer[0] / im_freq_period) / (ptr_mix_buffer[0] / freq_period)) + (1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = destination + (i * destination_stride);

    ptr_buffer[0] = ptr_new_mix_buffer[0];
  }
}

/**
 * ags_fast_pitch_util_pitch_float:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Pitch @fast_pitch_util of floating point data.
 * 
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_pitch_float(AgsFastPitchUtil *fast_pitch_util)
{
  gfloat *destination, *source;
  gfloat *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gfloat *mix_buffer, *im_mix_buffer, *low_mix_buffer, *new_mix_buffer;
  gfloat *ptr_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble volume;
  gdouble base_freq, im_freq, low_freq, new_freq;
  gdouble offset_factor, im_offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, low_freq_period, new_freq_period;
  gdouble t;
  guint i, j;

  if(fast_pitch_util == NULL ||
     fast_pitch_util->destination == NULL ||
     fast_pitch_util->source == NULL){
    return;
  }

  destination = fast_pitch_util->destination;
  destination_stride = fast_pitch_util->destination_stride;

  source = fast_pitch_util->source;
  source_stride = fast_pitch_util->source_stride;

  buffer_length = fast_pitch_util->buffer_length;
  samplerate = fast_pitch_util->samplerate;
  
  /* frequency */
  base_freq = exp2((fast_pitch_util->base_key) / 12.0) * 440.0;

  if(fast_pitch_util->tuning <= 0.0){
    im_freq = (1.25 * ((double) samplerate / base_freq)) * (double) samplerate;
  }else{
    im_freq = (0.75 * ((double) samplerate / base_freq)) * (double) samplerate;
  }

  low_freq = exp2((fast_pitch_util->base_key - 12.0) / 12.0) * 440.0;

  new_freq = exp2((fast_pitch_util->base_key + (fast_pitch_util->tuning / 100.0))  / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(im_freq <= 0.0){
    g_warning("rejecting pitch intermediate freq %f <= 0.0", im_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;
  
  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* get buffer */
  mix_buffer = (gfloat *) fast_pitch_util->mix_buffer;

  im_mix_buffer = (gfloat *) fast_pitch_util->im_mix_buffer;

  low_mix_buffer = (gfloat *) fast_pitch_util->low_mix_buffer;

  new_mix_buffer = (gfloat *) fast_pitch_util->new_mix_buffer;

  /* mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;

    /* write mix buffer */
    ptr_mix_buffer[0] = source[i * source_stride];
  }

  /* im mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gfloat z, mix_z, im_z;
    gdouble phase, im_phase;
    guint start_x;

    if(j >= im_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    im_phase = fmod(i, im_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_im_mix_buffer = im_mix_buffer + i;

    /* write im mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = ((j / im_offset_factor) - j) / (freq_period - im_freq_period);
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_im_mix_buffer[0] = im_z;
  }

  /* low mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gfloat z, mix_z, low_z;
    gdouble phase, low_phase;
    guint start_x;

    if(j >= low_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    low_phase = fmod(i, low_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_low_mix_buffer = low_mix_buffer + i;

    /* write low mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = low_freq_period / freq_period;
    
    low_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_low_mix_buffer[0] = low_z;
  }
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gfloat new_z;
    gdouble phase, im_phase, low_phase, new_phase;    
    guint start_x, im_start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(im_freq_period) != 0.0){
      im_start_x = im_freq_period * floor((double) i / im_freq_period);
    }else{
      im_start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    im_phase = fmod(i, im_freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase));
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }

    if(im_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase));
    }else{
      if((im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) < buffer_length &&
	 (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) > 0){
	ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_im_mix_buffer = im_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_im_mix_buffer = im_mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0.0){
      new_z = volume * ((1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_im_mix_buffer[0] / im_freq_period) / (ptr_mix_buffer[0] / freq_period)) + (1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = destination + (i * destination_stride);

    ptr_buffer[0] = ptr_new_mix_buffer[0];
  }
}

/**
 * ags_fast_pitch_util_pitch_double:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Pitch @fast_pitch_util of double precision floating point data.
 * 
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_pitch_double(AgsFastPitchUtil *fast_pitch_util)
{
  gdouble *destination, *source;
  gdouble *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gdouble *mix_buffer, *im_mix_buffer, *low_mix_buffer, *new_mix_buffer;
  gdouble *ptr_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble volume;
  gdouble base_freq, im_freq, low_freq, new_freq;
  gdouble offset_factor, im_offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, low_freq_period, new_freq_period;
  gdouble t;
  guint i, j;

  if(fast_pitch_util == NULL ||
     fast_pitch_util->destination == NULL ||
     fast_pitch_util->source == NULL){
    return;
  }

  destination = fast_pitch_util->destination;
  destination_stride = fast_pitch_util->destination_stride;

  source = fast_pitch_util->source;
  source_stride = fast_pitch_util->source_stride;

  buffer_length = fast_pitch_util->buffer_length;
  samplerate = fast_pitch_util->samplerate;
  
  /* frequency */
  base_freq = exp2((fast_pitch_util->base_key) / 12.0) * 440.0;

  if(fast_pitch_util->tuning <= 0.0){
    im_freq = (1.25 * ((double) samplerate / base_freq)) * (double) samplerate;
  }else{
    im_freq = (0.75 * ((double) samplerate / base_freq)) * (double) samplerate;
  }

  low_freq = exp2((fast_pitch_util->base_key - 12.0) / 12.0) * 440.0;

  new_freq = exp2((fast_pitch_util->base_key + (fast_pitch_util->tuning / 100.0))  / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(im_freq <= 0.0){
    g_warning("rejecting pitch intermediate freq %f <= 0.0", im_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;
  
  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* get buffer */
  mix_buffer = (gdouble *) fast_pitch_util->mix_buffer;

  im_mix_buffer = (gdouble *) fast_pitch_util->im_mix_buffer;

  low_mix_buffer = (gdouble *) fast_pitch_util->low_mix_buffer;

  new_mix_buffer = (gdouble *) fast_pitch_util->new_mix_buffer;

  /* mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;

    /* write mix buffer */
    ptr_mix_buffer[0] = source[i * source_stride];
  }

  /* im mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gdouble z, mix_z, im_z;
    gdouble phase, im_phase;
    guint start_x;

    if(j >= im_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    im_phase = fmod(i, im_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_im_mix_buffer = im_mix_buffer + i;

    /* write im mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = ((j / im_offset_factor) - j) / (freq_period - im_freq_period);
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_im_mix_buffer[0] = im_z;
  }

  /* low mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gdouble z, mix_z, low_z;
    gdouble phase, low_phase;
    guint start_x;

    if(j >= low_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    low_phase = fmod(i, low_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_low_mix_buffer = low_mix_buffer + i;

    /* write low mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = low_freq_period / freq_period;
    
    low_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_low_mix_buffer[0] = low_z;
  }
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gdouble new_z;
    gdouble phase, im_phase, low_phase, new_phase;    
    guint start_x, im_start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(im_freq_period) != 0.0){
      im_start_x = im_freq_period * floor((double) i / im_freq_period);
    }else{
      im_start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    im_phase = fmod(i, im_freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase));
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }

    if(im_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase));
    }else{
      if((im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) < buffer_length &&
	 (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) > 0){
	ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_im_mix_buffer = im_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_im_mix_buffer = im_mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0.0){
      new_z = volume * ((1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_im_mix_buffer[0] / im_freq_period) / (ptr_mix_buffer[0] / freq_period)) + (1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = destination + (i * destination_stride);

    ptr_buffer[0] = ptr_new_mix_buffer[0];
  }
}

/**
 * ags_fast_pitch_util_pitch_complex:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Pitch @fast_pitch_util of complex data.
 * 
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_pitch_complex(AgsFastPitchUtil *fast_pitch_util)
{
  AgsComplex *destination, *source;
  AgsComplex *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  AgsComplex *mix_buffer, *im_mix_buffer, *low_mix_buffer, *new_mix_buffer;
  AgsComplex *ptr_buffer;

  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble volume;
  gdouble base_freq, im_freq, low_freq, new_freq;
  gdouble offset_factor, im_offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, low_freq_period, new_freq_period;
  gdouble t;
  guint i, j;

  if(fast_pitch_util == NULL ||
     fast_pitch_util->destination == NULL ||
     fast_pitch_util->source == NULL){
    return;
  }

  destination = fast_pitch_util->destination;
  destination_stride = fast_pitch_util->destination_stride;

  source = fast_pitch_util->source;
  source_stride = fast_pitch_util->source_stride;

  buffer_length = fast_pitch_util->buffer_length;
  samplerate = fast_pitch_util->samplerate;
  
  /* frequency */
  base_freq = exp2((fast_pitch_util->base_key) / 12.0) * 440.0;

  if(fast_pitch_util->tuning <= 0.0){
    im_freq = (1.25 * ((double) samplerate / base_freq)) * (double) samplerate;
  }else{
    im_freq = (0.75 * ((double) samplerate / base_freq)) * (double) samplerate;
  }

  low_freq = exp2((fast_pitch_util->base_key - 12.0) / 12.0) * 440.0;

  new_freq = exp2((fast_pitch_util->base_key + (fast_pitch_util->tuning / 100.0))  / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(im_freq <= 0.0){
    g_warning("rejecting pitch intermediate freq %f <= 0.0", im_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;
  
  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* get buffer */
  mix_buffer = (AgsComplex *) fast_pitch_util->mix_buffer;

  im_mix_buffer = (AgsComplex *) fast_pitch_util->im_mix_buffer;

  low_mix_buffer = (AgsComplex *) fast_pitch_util->low_mix_buffer;

  new_mix_buffer = (AgsComplex *) fast_pitch_util->new_mix_buffer;

  /* mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;

    /* write mix buffer */
    ags_complex_set(ptr_mix_buffer,
		    ags_complex_get(source + (i * source_stride)));
  }

  /* im mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    double _Complex z, mix_z, im_z;
    gdouble phase, im_phase;
    guint start_x;

    if(j >= im_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    im_phase = fmod(i, im_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_im_mix_buffer = im_mix_buffer + i;

    /* write im mix buffer */
    z = ags_complex_get(mix_buffer + i);
    mix_z = ags_complex_get(ptr_mix_buffer);

    t = ((j / im_offset_factor) - j) / (freq_period - im_freq_period);
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    ags_complex_set(ptr_im_mix_buffer,
		    im_z);
  }

  /* low mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    double _Complex z, mix_z, low_z;
    gdouble phase, low_phase;
    guint start_x;

    if(j >= low_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    low_phase = fmod(i, low_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_low_mix_buffer = low_mix_buffer + i;

    /* write low mix buffer */
    z = ags_complex_get(mix_buffer + i);
    mix_z = ags_complex_get(ptr_mix_buffer);
			
    t = low_freq_period / freq_period;
    
    low_z = (1.0 - t) * z + (t * mix_z);
    
    ags_complex_set(ptr_low_mix_buffer,
		    low_z);
  }
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    double _Complex new_z;
    gdouble phase, im_phase, low_phase, new_phase;    
    guint start_x, im_start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(im_freq_period) != 0.0){
      im_start_x = im_freq_period * floor((double) i / im_freq_period);
    }else{
      im_start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    im_phase = fmod(i, im_freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase));
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }

    if(im_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase));
    }else{
      if((im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) < buffer_length &&
	 (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) > 0){
	ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_im_mix_buffer = im_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_im_mix_buffer = im_mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ags_complex_get(ptr_mix_buffer) != 0.0){
      new_z = volume * ((1.0 / 2.0) * (new_freq_period * (ags_complex_get(ptr_mix_buffer) / freq_period) * (ags_complex_get(ptr_im_mix_buffer) / im_freq_period) / (ags_complex_get(ptr_mix_buffer) / freq_period)) + (1.0 / 2.0) * (new_freq_period * (ags_complex_get(ptr_mix_buffer) / freq_period) * (ags_complex_get(ptr_low_mix_buffer) / low_freq_period) / (ags_complex_get(ptr_mix_buffer) / freq_period)));
    }else{
      new_z = 0;
    }
    
    ags_complex_set(ptr_new_mix_buffer,
		    new_z);
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = destination + (i * destination_stride);

    ags_complex_set(ptr_buffer,
		    ags_complex_get(ptr_new_mix_buffer));
  }
}

/**
 * ags_fast_pitch_util_pitch:
 * @fast_pitch_util: the #AgsFastPitchUtil-struct
 * 
 * Pitch @fast_pitch_util.
 * 
 * Since: 3.9.6
 */
void
ags_fast_pitch_util_pitch(AgsFastPitchUtil *fast_pitch_util)
{
  if(fast_pitch_util == NULL ||
     fast_pitch_util->destination == NULL ||
     fast_pitch_util->source == NULL){
    return;
  }

  switch(fast_pitch_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_fast_pitch_util_pitch_s8(fast_pitch_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_fast_pitch_util_pitch_s16(fast_pitch_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_fast_pitch_util_pitch_s24(fast_pitch_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_fast_pitch_util_pitch_s32(fast_pitch_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_fast_pitch_util_pitch_s64(fast_pitch_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_fast_pitch_util_pitch_float(fast_pitch_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_fast_pitch_util_pitch_double(fast_pitch_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_fast_pitch_util_pitch_complex(fast_pitch_util);
  }
  break;
  default:
    g_warning("unknown format");
  }
}
