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

#include <ags/audio/ags_pitch_2x_alias_util.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

/**
 * SECTION:ags_pitch_2x_alias_util
 * @short_description: 2 times aliased pitch util
 * @title: AgsPitch2xAliasUtil
 * @section_id:
 * @include: ags/audio/ags_pitch_2x_alias_util.h
 *
 * Utility functions to pitch.
 */

GType
ags_pitch_2x_alias_util_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_pitch_2x_alias_util = 0;

    ags_type_pitch_2x_alias_util =
      g_boxed_type_register_static("AgsPitch2xAliasUtil",
				   (GBoxedCopyFunc) ags_pitch_2x_alias_util_copy,
				   (GBoxedFreeFunc) ags_pitch_2x_alias_util_free);

    g_once_init_leave(&g_define_type_id__static, ags_type_pitch_2x_alias_util);
  }

  return(g_define_type_id__static);
}


/**
 * ags_pitch_2x_alias_util_alloc:
 * 
 * Allocate #AgsPitch2xAliasUtil-struct.
 * 
 * Returns: the newly allocated #AgsPitch2xAliasUtil-struct
 * 
 * Since: 6.15.0
 */
AgsPitch2xAliasUtil*
ags_pitch_2x_alias_util_alloc()
{
  AgsPitch2xAliasUtil *ptr;
  
  ptr = (AgsPitch2xAliasUtil *) g_new(AgsPitch2xAliasUtil,
				       1);

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->destination = NULL;
  ptr->destination_stride = 1;

  ptr->alias_source_buffer = ags_stream_alloc(AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE,
					      AGS_SOUNDCARD_DEFAULT_FORMAT);

  ptr->alias_source_max_buffer_length = AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE;

  ptr->alias_new_source_buffer = ags_stream_alloc(AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE,
						  AGS_SOUNDCARD_DEFAULT_FORMAT);

  ptr->alias_new_source_max_buffer_length = AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE;
  
  ptr->buffer_length = 0;
  ptr->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  ptr->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  ptr->base_key = 0.0;
  ptr->tuning = 0.0;

  ptr->vibrato_enabled = TRUE;

  ptr->vibrato_gain = 1.0;
  ptr->vibrato_lfo_depth = 1.0;
  ptr->vibrato_lfo_freq = 8.172;
  ptr->vibrato_tuning = 0.0;

  ptr->frame_count = 0;
  ptr->offset = 0;

  ptr->note_256th_mode = TRUE;

  ptr->offset_256th = 0;
  
  return(ptr);
}

/**
 * ags_pitch_2x_alias_util_copy:
 * @ptr: the original #AgsPitch2xAliasUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsPitch2xAliasUtil-struct
 *
 * Since: 6.15.0
 */
gpointer
ags_pitch_2x_alias_util_copy(AgsPitch2xAliasUtil *ptr)
{
  AgsPitch2xAliasUtil *new_ptr;
  
  new_ptr = (AgsPitch2xAliasUtil *) g_new(AgsPitch2xAliasUtil,
					   1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->alias_source_buffer = ags_stream_alloc(AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE,
						  ptr->format);

  new_ptr->alias_source_max_buffer_length = AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE;

  new_ptr->alias_new_source_buffer = ags_stream_alloc(AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE,
						      ptr->format);

  new_ptr->alias_new_source_max_buffer_length = AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE;
  

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
 * ags_pitch_2x_alias_util_free:
 * @ptr: the #AgsPitch2xAliasUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_free(AgsPitch2xAliasUtil *ptr)
{
  if(ptr == NULL){
    return;
  }
  
  ags_stream_free(ptr->alias_source_buffer);
  ags_stream_free(ptr->alias_new_source_buffer);
  
  g_free(ptr);
}

/**
 * ags_pitch_2x_alias_util_get_destination:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Get destination buffer of @pitch_2x_alias_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 6.15.0
 */
gpointer
ags_pitch_2x_alias_util_get_destination(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL){
    return(NULL);
  }

  return(pitch_2x_alias_util->destination);
}

/**
 * ags_pitch_2x_alias_util_set_destination:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * @destination: (transfer none): the destination buffer
 *
 * Set @destination buffer of @pitch_2x_alias_util.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_set_destination(AgsPitch2xAliasUtil *pitch_2x_alias_util,
					 gpointer destination)
{
  if(pitch_2x_alias_util == NULL){
    return;
  }

  pitch_2x_alias_util->destination = destination;
}

/**
 * ags_pitch_2x_alias_util_get_destination_stride:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Get destination stride of @pitch_2x_alias_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 6.15.0
 */
guint
ags_pitch_2x_alias_util_get_destination_stride(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL){
    return(0);
  }

  return(pitch_2x_alias_util->destination_stride);
}

/**
 * ags_pitch_2x_alias_util_set_destination_stride:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @pitch_2x_alias_util.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_set_destination_stride(AgsPitch2xAliasUtil *pitch_2x_alias_util,
						guint destination_stride)
{
  if(pitch_2x_alias_util == NULL){
    return;
  }

  pitch_2x_alias_util->destination_stride = destination_stride;
}

/**
 * ags_pitch_2x_alias_util_get_source:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Get source buffer of @pitch_2x_alias_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 6.15.0
 */
gpointer
ags_pitch_2x_alias_util_get_source(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL){
    return(NULL);
  }

  return(pitch_2x_alias_util->source);
}

/**
 * ags_pitch_2x_alias_util_set_source:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * @source: (transfer none): the source buffer
 *
 * Set @source buffer of @pitch_2x_alias_util.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_set_source(AgsPitch2xAliasUtil *pitch_2x_alias_util,
				    gpointer source)
{
  if(pitch_2x_alias_util == NULL){
    return;
  }

  pitch_2x_alias_util->source = source;
}

/**
 * ags_pitch_2x_alias_util_get_source_stride:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Get source stride of @pitch_2x_alias_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 6.15.0
 */
guint
ags_pitch_2x_alias_util_get_source_stride(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL){
    return(0);
  }

  return(pitch_2x_alias_util->source_stride);
}

/**
 * ags_pitch_2x_alias_util_set_source_stride:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @pitch_2x_alias_util.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_set_source_stride(AgsPitch2xAliasUtil *pitch_2x_alias_util,
					   guint source_stride)
{
  if(pitch_2x_alias_util == NULL){
    return;
  }

  pitch_2x_alias_util->source_stride = source_stride;
}

/**
 * ags_pitch_2x_alias_util_get_buffer_length:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Get buffer length of @pitch_2x_alias_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 6.15.0
 */
guint
ags_pitch_2x_alias_util_get_buffer_length(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL){
    return(0);
  }

  return(pitch_2x_alias_util->buffer_length);
}

/**
 * ags_pitch_2x_alias_util_set_buffer_length:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @pitch_2x_alias_util.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_set_buffer_length(AgsPitch2xAliasUtil *pitch_2x_alias_util,
					   guint buffer_length)
{
  if(pitch_2x_alias_util == NULL ||
     pitch_2x_alias_util->buffer_length == buffer_length){
    return;
  }

  pitch_2x_alias_util->buffer_length = buffer_length;
}

/**
 * ags_pitch_2x_alias_util_get_format:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Get format of @pitch_2x_alias_util.
 * 
 * Returns: the format
 * 
 * Since: 6.15.0
 */
guint
ags_pitch_2x_alias_util_get_format(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL){
    return(0);
  }

  return(pitch_2x_alias_util->format);
}

/**
 * ags_pitch_2x_alias_util_set_format:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * @format: the format
 *
 * Set @format of @pitch_2x_alias_util.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_set_format(AgsPitch2xAliasUtil *pitch_2x_alias_util,
				    guint format)
{
  if(pitch_2x_alias_util == NULL ||
     pitch_2x_alias_util->format == format){
    return;
  }

  pitch_2x_alias_util->format = format;

  ags_stream_free(pitch_2x_alias_util->alias_source_buffer);
  ags_stream_free(pitch_2x_alias_util->alias_new_source_buffer);

  pitch_2x_alias_util->alias_source_buffer = ags_stream_alloc(AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE,
							      format);
  
  pitch_2x_alias_util->alias_new_source_buffer = ags_stream_alloc(AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE,
								  format);
}

/**
 * ags_pitch_2x_alias_util_get_samplerate:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Get samplerate of @pitch_2x_alias_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 6.15.0
 */
guint
ags_pitch_2x_alias_util_get_samplerate(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL){
    return(0);
  }

  return(pitch_2x_alias_util->samplerate);
}

/**
 * ags_pitch_2x_alias_util_set_samplerate:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @pitch_2x_alias_util.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_set_samplerate(AgsPitch2xAliasUtil *pitch_2x_alias_util,
					guint samplerate)
{
  if(pitch_2x_alias_util == NULL ||
     pitch_2x_alias_util->samplerate == samplerate){
    return;
  }

  pitch_2x_alias_util->samplerate = samplerate;
}

/**
 * ags_pitch_2x_alias_util_get_base_key:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Get base key of @pitch_2x_alias_util.
 * 
 * Returns: the base key
 * 
 * Since: 6.15.0
 */
gdouble
ags_pitch_2x_alias_util_get_base_key(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL){
    return(0.0);
  }

  return(pitch_2x_alias_util->base_key);
}

/**
 * ags_pitch_2x_alias_util_set_base_key:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * @base_key: the base key
 *
 * Set @base_key of @pitch_2x_alias_util.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_set_base_key(AgsPitch2xAliasUtil *pitch_2x_alias_util,
				      gdouble base_key)
{
  if(pitch_2x_alias_util == NULL){
    return;
  }

  pitch_2x_alias_util->base_key = base_key;
}

/**
 * ags_pitch_2x_alias_util_get_tuning:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Get tuning of @pitch_2x_alias_util.
 * 
 * Returns: the tuning
 * 
 * Since: 6.15.0
 */
gdouble
ags_pitch_2x_alias_util_get_tuning(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL){
    return(0.0);
  }

  return(pitch_2x_alias_util->tuning);
}

/**
 * ags_pitch_2x_alias_util_set_tuning:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * @tuning: the tuning
 *
 * Set @tuning of @pitch_2x_alias_util.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_set_tuning(AgsPitch2xAliasUtil *pitch_2x_alias_util,
				    gdouble tuning)
{
  if(pitch_2x_alias_util == NULL){
    return;
  }

  pitch_2x_alias_util->tuning = tuning;
}


/**
 * ags_pitch_2x_alias_util_get_vibrato_enabled:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Get vibrato enabled of @pitch_2x_alias_util.
 * 
 * Returns: %TRUE if the vibrato is enabled, otherwise %FALSE
 * 
 * Since: 6.15.0
 */
gboolean
ags_pitch_2x_alias_util_get_vibrato_enabled(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL){
    return(FALSE);
  }

  return(pitch_2x_alias_util->vibrato_enabled);
}

/**
 * ags_pitch_2x_alias_util_set_vibrato_enabled:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * @vibrato_enabled: the vibrato enabled
 *
 * Set @vibrato_enabled of @pitch_2x_alias_util.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_set_vibrato_enabled(AgsPitch2xAliasUtil *pitch_2x_alias_util,
					     gboolean vibrato_enabled)
{
  if(pitch_2x_alias_util == NULL){
    return;
  }

  pitch_2x_alias_util->vibrato_enabled = vibrato_enabled;
}

/**
 * ags_pitch_2x_alias_util_get_vibrato_gain:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Get vibrato gain of @pitch_2x_alias_util.
 * 
 * Returns: the vibrato gain
 * 
 * Since: 6.15.0
 */
gdouble
ags_pitch_2x_alias_util_get_vibrato_gain(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL){
    return(0.0);
  }

  return(pitch_2x_alias_util->vibrato_gain);
}

/**
 * ags_pitch_2x_alias_util_set_vibrato_gain:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * @vibrato_gain: the vibrato gain
 *
 * Set @vibrato_gain of @pitch_2x_alias_util.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_set_vibrato_gain(AgsPitch2xAliasUtil *pitch_2x_alias_util,
					  gdouble vibrato_gain)
{
  if(pitch_2x_alias_util == NULL){
    return;
  }

  pitch_2x_alias_util->vibrato_gain = vibrato_gain;
}

/**
 * ags_pitch_2x_alias_util_get_vibrato_lfo_depth:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Get vibrato lfo_depth of @pitch_2x_alias_util.
 * 
 * Returns: the vibrato lfo_depth
 * 
 * Since: 6.15.0
 */
gdouble
ags_pitch_2x_alias_util_get_vibrato_lfo_depth(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL){
    return(0.0);
  }

  return(pitch_2x_alias_util->vibrato_lfo_depth);
}

/**
 * ags_pitch_2x_alias_util_set_vibrato_lfo_depth:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * @vibrato_lfo_depth: the vibrato lfo_depth
 *
 * Set @vibrato_lfo_depth of @pitch_2x_alias_util.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_set_vibrato_lfo_depth(AgsPitch2xAliasUtil *pitch_2x_alias_util,
					       gdouble vibrato_lfo_depth)
{
  if(pitch_2x_alias_util == NULL){
    return;
  }

  pitch_2x_alias_util->vibrato_lfo_depth = vibrato_lfo_depth;
}

/**
 * ags_pitch_2x_alias_util_get_vibrato_lfo_freq:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Get vibrato lfo_freq of @pitch_2x_alias_util.
 * 
 * Returns: the vibrato lfo_freq
 * 
 * Since: 6.15.0
 */
gdouble
ags_pitch_2x_alias_util_get_vibrato_lfo_freq(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL){
    return(0.0);
  }

  return(pitch_2x_alias_util->vibrato_lfo_freq);
}

/**
 * ags_pitch_2x_alias_util_set_vibrato_lfo_freq:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * @vibrato_lfo_freq: the vibrato lfo_freq
 *
 * Set @vibrato_lfo_freq of @pitch_2x_alias_util.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_set_vibrato_lfo_freq(AgsPitch2xAliasUtil *pitch_2x_alias_util,
					      gdouble vibrato_lfo_freq)
{
  if(pitch_2x_alias_util == NULL){
    return;
  }

  pitch_2x_alias_util->vibrato_lfo_freq = vibrato_lfo_freq;
}

/**
 * ags_pitch_2x_alias_util_get_vibrato_tuning:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Get vibrato tuning of @pitch_2x_alias_util.
 * 
 * Returns: the vibrato tuning
 * 
 * Since: 6.15.0
 */
gdouble
ags_pitch_2x_alias_util_get_vibrato_tuning(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL){
    return(0.0);
  }

  return(pitch_2x_alias_util->vibrato_tuning);
}

/**
 * ags_pitch_2x_alias_util_set_vibrato_tuning:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * @vibrato_tuning: the vibrato tuning
 *
 * Set @vibrato_tuning of @pitch_2x_alias_util.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_set_vibrato_tuning(AgsPitch2xAliasUtil *pitch_2x_alias_util,
					    gdouble vibrato_tuning)
{
  if(pitch_2x_alias_util == NULL){
    return;
  }

  pitch_2x_alias_util->vibrato_tuning = vibrato_tuning;
}

/**
 * ags_pitch_2x_alias_util_get_frame_count:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Get frame count of @pitch_2x_alias_util.
 * 
 * Returns: the frame count
 * 
 * Since: 6.15.0
 */
guint
ags_pitch_2x_alias_util_get_frame_count(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL){
    return(0);
  }

  return(pitch_2x_alias_util->frame_count);
}

/**
 * ags_pitch_2x_alias_util_set_frame_count:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * @frame_count: the frame count
 *
 * Set @frame_count of @pitch_2x_alias_util.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_set_frame_count(AgsPitch2xAliasUtil *pitch_2x_alias_util,
					 guint frame_count)
{
  if(pitch_2x_alias_util == NULL){
    return;
  }

  pitch_2x_alias_util->frame_count = frame_count;
}

/**
 * ags_pitch_2x_alias_util_get_offset:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Get offset of @pitch_2x_alias_util.
 * 
 * Returns: the offset
 * 
 * Since: 6.15.0
 */
guint
ags_pitch_2x_alias_util_get_offset(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL){
    return(0);
  }

  return(pitch_2x_alias_util->offset);
}

/**
 * ags_pitch_2x_alias_util_set_offset:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * @offset: the offset
 *
 * Set @offset of @pitch_2x_alias_util.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_set_offset(AgsPitch2xAliasUtil *pitch_2x_alias_util,
				    guint offset)
{
  if(pitch_2x_alias_util == NULL){
    return;
  }

  pitch_2x_alias_util->offset = offset;
}

/**
 * ags_pitch_2x_alias_util_get_note_256th_mode:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Get note 256th mode of @pitch_2x_alias_util.
 * 
 * Returns: %TRUE if note 256th mode, otherwise %FALSE
 * 
 * Since: 6.15.0
 */
gboolean
ags_pitch_2x_alias_util_get_note_256th_mode(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL){
    return(0);
  }

  return(pitch_2x_alias_util->note_256th_mode);
}

/**
 * ags_pitch_2x_alias_util_set_note_256th_mode:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * @note_256th_mode: the note 256th mode
 *
 * Set @note_256th_mode of @pitch_2x_alias_util.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_set_note_256th_mode(AgsPitch2xAliasUtil *pitch_2x_alias_util,
					     gboolean note_256th_mode)
{
  if(pitch_2x_alias_util == NULL){
    return;
  }

  pitch_2x_alias_util->note_256th_mode = note_256th_mode;
}

/**
 * ags_pitch_2x_alias_util_get_offset_256th:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Get offset as note 256th of @pitch_2x_alias_util.
 * 
 * Returns: the offset as note 256th
 * 
 * Since: 6.15.0
 */
guint
ags_pitch_2x_alias_util_get_offset_256th(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL){
    return(0);
  }

  return(pitch_2x_alias_util->offset_256th);
}

/**
 * ags_pitch_2x_alias_util_set_offset_256th:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * @offset_256th: the offset as note 256th
 *
 * Set @offset_256th of @pitch_2x_alias_util.
 *
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_set_offset_256th(AgsPitch2xAliasUtil *pitch_2x_alias_util,
					  guint offset_256th)
{
  if(pitch_2x_alias_util == NULL){
    return;
  }
  
  pitch_2x_alias_util->offset_256th = offset_256th;
}

/**
 * ags_pitch_2x_alias_util_pitch_s8:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Pitch @pitch_2x_alias_util of signed 8 bit data.
 * 
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_pitch_s8(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  gint8 *destination, *source;
  gint8 *alias_source_buffer;
  gint8 *alias_new_source_buffer;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;
  gdouble new_pitch_hz;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint offset;
  gdouble source_freq_period;
  gdouble new_source_freq_period;
  gdouble t;
  gint reset_i;
  guint i;

  if(pitch_2x_alias_util == NULL ||
     pitch_2x_alias_util->destination == NULL ||
     pitch_2x_alias_util->source == NULL){
    return;
  }

  destination = pitch_2x_alias_util->destination;
  destination_stride = pitch_2x_alias_util->destination_stride;

  source = pitch_2x_alias_util->source;
  source_stride = pitch_2x_alias_util->source_stride;

  alias_source_buffer = pitch_2x_alias_util->alias_source_buffer;
  
  alias_new_source_buffer = pitch_2x_alias_util->alias_new_source_buffer;

  buffer_length = pitch_2x_alias_util->buffer_length;
  samplerate = pitch_2x_alias_util->samplerate;

  base_key = pitch_2x_alias_util->base_key;
  tuning = pitch_2x_alias_util->tuning;

  vibrato_gain = pitch_2x_alias_util->vibrato_gain;
  vibrato_lfo_depth = pitch_2x_alias_util->vibrato_lfo_depth;
  vibrato_lfo_freq = pitch_2x_alias_util->vibrato_lfo_freq;
  vibrato_tuning = pitch_2x_alias_util->vibrato_tuning;
  offset = pitch_2x_alias_util->offset;

  if(pitch_2x_alias_util->vibrato_enabled == FALSE){
    vibrato_gain = 0.0;
  }
  
  root_pitch_hz = exp2(((double) base_key - 48.0) / 12.0) * 440.0;
  new_pitch_hz = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  for(i = 0; i < 2 * buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    t = (double) (i % 2) / 2.0;

    if((i / 2) + 1 < buffer_length){
      alias_source_buffer[i] = ((1.0 - t) * (source[(i / 2) * source_stride] + (t * source[((i / 2) + 1) * source_stride])));
    }
  }

  source_freq_period = (2 * samplerate) / root_pitch_hz;

  new_source_freq_period = (2 * samplerate) / new_pitch_hz;

  reset_i = -1;

  for(i = 0; i < 2 * buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    new_pitch_hz = exp2((base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0))  / 12.0) * 440.0;
      
    new_source_freq_period = (2 * samplerate) / new_pitch_hz;
    
    if(source_freq_period < new_source_freq_period){
      t = (i % (guint) source_freq_period) / new_source_freq_period;

      if((guint) floor(i * (source_freq_period / new_source_freq_period)) < 2 * buffer_length){
	alias_new_source_buffer[i] = ((1.0 - t) * alias_source_buffer[i]) + (t * alias_source_buffer[(guint) floor(i * (source_freq_period / new_source_freq_period))]);
      }
    }else{
      t = 1.0 / source_freq_period * new_source_freq_period;

      if(i * source_freq_period / new_source_freq_period < 2 * buffer_length){
	alias_new_source_buffer[i] = alias_source_buffer[(guint) floor(i * source_freq_period / new_source_freq_period)];
      }else{
	if(reset_i == -1){
	  reset_i = i;
	}

	if(reset_i != -1 && i - reset_i >= reset_i){
	  reset_i = i;
	}
	
	alias_new_source_buffer[i] = alias_source_buffer[(guint) floor((i - reset_i) * source_freq_period / new_source_freq_period)];
      }
    }
  }

  for(i = 0; i < buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    destination[i * destination_stride] = alias_new_source_buffer[2 * i];
  }
}

/**
 * ags_pitch_2x_alias_util_pitch_s16:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Pitch @pitch_2x_alias_util of signed 16 bit data.
 * 
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_pitch_s16(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  gint16 *destination, *source;
  gint16 *alias_source_buffer;
  gint16 *alias_new_source_buffer;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;
  gdouble new_pitch_hz;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint offset;
  gdouble source_freq_period;
  gdouble new_source_freq_period;
  gdouble t;
  gint reset_i;
  guint i;

  if(pitch_2x_alias_util == NULL ||
     pitch_2x_alias_util->destination == NULL ||
     pitch_2x_alias_util->source == NULL){
    return;
  }

  destination = pitch_2x_alias_util->destination;
  destination_stride = pitch_2x_alias_util->destination_stride;

  source = pitch_2x_alias_util->source;
  source_stride = pitch_2x_alias_util->source_stride;

  alias_source_buffer = pitch_2x_alias_util->alias_source_buffer;
  
  alias_new_source_buffer = pitch_2x_alias_util->alias_new_source_buffer;

  buffer_length = pitch_2x_alias_util->buffer_length;
  samplerate = pitch_2x_alias_util->samplerate;

  base_key = pitch_2x_alias_util->base_key;
  tuning = pitch_2x_alias_util->tuning;

  vibrato_gain = pitch_2x_alias_util->vibrato_gain;
  vibrato_lfo_depth = pitch_2x_alias_util->vibrato_lfo_depth;
  vibrato_lfo_freq = pitch_2x_alias_util->vibrato_lfo_freq;
  vibrato_tuning = pitch_2x_alias_util->vibrato_tuning;
  offset = pitch_2x_alias_util->offset;

  if(pitch_2x_alias_util->vibrato_enabled == FALSE){
    vibrato_gain = 0.0;
  }
  
  root_pitch_hz = exp2(((double) base_key - 48.0) / 12.0) * 440.0;
  new_pitch_hz = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  for(i = 0; i < 2 * buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    t = (double) (i % 2) / 2.0;

    if((i / 2) + 1 < buffer_length){
      alias_source_buffer[i] = ((1.0 - t) * (source[(i / 2) * source_stride] + (t * source[((i / 2) + 1) * source_stride])));
    }
  }

  source_freq_period = (2 * samplerate) / root_pitch_hz;

  new_source_freq_period = (2 * samplerate) / new_pitch_hz;

  reset_i = -1;

  for(i = 0; i < 2 * buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    new_pitch_hz = exp2((base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0))  / 12.0) * 440.0;
      
    new_source_freq_period = (2 * samplerate) / new_pitch_hz;
    
    if(source_freq_period < new_source_freq_period){
      t = (i % (guint) source_freq_period) / new_source_freq_period;

      if((guint) floor(i * (source_freq_period / new_source_freq_period)) < 2 * buffer_length){
	alias_new_source_buffer[i] = ((1.0 - t) * alias_source_buffer[i]) + (t * alias_source_buffer[(guint) floor(i * (source_freq_period / new_source_freq_period))]);
      }
    }else{
      t = 1.0 / source_freq_period * new_source_freq_period;

      if(i * source_freq_period / new_source_freq_period < 2 * buffer_length){
	alias_new_source_buffer[i] = alias_source_buffer[(guint) floor(i * source_freq_period / new_source_freq_period)];
      }else{
	if(reset_i == -1){
	  reset_i = i;
	}

	if(reset_i != -1 && i - reset_i >= reset_i){
	  reset_i = i;
	}
	
	alias_new_source_buffer[i] = alias_source_buffer[(guint) floor((i - reset_i) * source_freq_period / new_source_freq_period)];
      }
    }
  }

  for(i = 0; i < buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    destination[i * destination_stride] = alias_new_source_buffer[2 * i];
  }
}

/**
 * ags_pitch_2x_alias_util_pitch_s24:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Pitch @pitch_2x_alias_util of signed 24 bit data.
 * 
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_pitch_s24(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  gint32 *destination, *source;
  gint32 *alias_source_buffer;
  gint32 *alias_new_source_buffer;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;
  gdouble new_pitch_hz;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint offset;
  gdouble source_freq_period;
  gdouble new_source_freq_period;
  gdouble t;
  gint reset_i;
  guint i;

  if(pitch_2x_alias_util == NULL ||
     pitch_2x_alias_util->destination == NULL ||
     pitch_2x_alias_util->source == NULL){
    return;
  }

  destination = pitch_2x_alias_util->destination;
  destination_stride = pitch_2x_alias_util->destination_stride;

  source = pitch_2x_alias_util->source;
  source_stride = pitch_2x_alias_util->source_stride;

  alias_source_buffer = pitch_2x_alias_util->alias_source_buffer;
  
  alias_new_source_buffer = pitch_2x_alias_util->alias_new_source_buffer;

  buffer_length = pitch_2x_alias_util->buffer_length;
  samplerate = pitch_2x_alias_util->samplerate;

  base_key = pitch_2x_alias_util->base_key;
  tuning = pitch_2x_alias_util->tuning;

  vibrato_gain = pitch_2x_alias_util->vibrato_gain;
  vibrato_lfo_depth = pitch_2x_alias_util->vibrato_lfo_depth;
  vibrato_lfo_freq = pitch_2x_alias_util->vibrato_lfo_freq;
  vibrato_tuning = pitch_2x_alias_util->vibrato_tuning;
  offset = pitch_2x_alias_util->offset;

  if(pitch_2x_alias_util->vibrato_enabled == FALSE){
    vibrato_gain = 0.0;
  }
  
  root_pitch_hz = exp2(((double) base_key - 48.0) / 12.0) * 440.0;
  new_pitch_hz = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  for(i = 0; i < 2 * buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    t = (double) (i % 2) / 2.0;

    if((i / 2) + 1 < buffer_length){
      alias_source_buffer[i] = ((1.0 - t) * (source[(i / 2) * source_stride] + (t * source[((i / 2) + 1) * source_stride])));
    }
  }

  source_freq_period = (2 * samplerate) / root_pitch_hz;

  new_source_freq_period = (2 * samplerate) / new_pitch_hz;

  reset_i = -1;

  for(i = 0; i < 2 * buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    new_pitch_hz = exp2((base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0))  / 12.0) * 440.0;
      
    new_source_freq_period = (2 * samplerate) / new_pitch_hz;
    
    if(source_freq_period < new_source_freq_period){
      t = (i % (guint) source_freq_period) / new_source_freq_period;

      if((guint) floor(i * (source_freq_period / new_source_freq_period)) < 2 * buffer_length){
	alias_new_source_buffer[i] = ((1.0 - t) * alias_source_buffer[i]) + (t * alias_source_buffer[(guint) floor(i * (source_freq_period / new_source_freq_period))]);
      }
    }else{
      t = 1.0 / source_freq_period * new_source_freq_period;

      if(i * source_freq_period / new_source_freq_period < 2 * buffer_length){
	alias_new_source_buffer[i] = alias_source_buffer[(guint) floor(i * source_freq_period / new_source_freq_period)];
      }else{
	if(reset_i == -1){
	  reset_i = i;
	}

	if(reset_i != -1 && i - reset_i >= reset_i){
	  reset_i = i;
	}
	
	alias_new_source_buffer[i] = alias_source_buffer[(guint) floor((i - reset_i) * source_freq_period / new_source_freq_period)];
      }
    }
  }

  for(i = 0; i < buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    destination[i * destination_stride] = alias_new_source_buffer[2 * i];
  }
}

/**
 * ags_pitch_2x_alias_util_pitch_s32:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Pitch @pitch_2x_alias_util of signed 32 bit data.
 * 
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_pitch_s32(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  gint32 *destination, *source;
  gint32 *alias_source_buffer;
  gint32 *alias_new_source_buffer;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;
  gdouble new_pitch_hz;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint offset;
  gdouble source_freq_period;
  gdouble new_source_freq_period;
  gdouble t;
  gint reset_i;
  guint i;

  if(pitch_2x_alias_util == NULL ||
     pitch_2x_alias_util->destination == NULL ||
     pitch_2x_alias_util->source == NULL){
    return;
  }

  destination = pitch_2x_alias_util->destination;
  destination_stride = pitch_2x_alias_util->destination_stride;

  source = pitch_2x_alias_util->source;
  source_stride = pitch_2x_alias_util->source_stride;

  alias_source_buffer = pitch_2x_alias_util->alias_source_buffer;
  
  alias_new_source_buffer = pitch_2x_alias_util->alias_new_source_buffer;

  buffer_length = pitch_2x_alias_util->buffer_length;
  samplerate = pitch_2x_alias_util->samplerate;

  base_key = pitch_2x_alias_util->base_key;
  tuning = pitch_2x_alias_util->tuning;

  vibrato_gain = pitch_2x_alias_util->vibrato_gain;
  vibrato_lfo_depth = pitch_2x_alias_util->vibrato_lfo_depth;
  vibrato_lfo_freq = pitch_2x_alias_util->vibrato_lfo_freq;
  vibrato_tuning = pitch_2x_alias_util->vibrato_tuning;
  offset = pitch_2x_alias_util->offset;

  if(pitch_2x_alias_util->vibrato_enabled == FALSE){
    vibrato_gain = 0.0;
  }
  
  root_pitch_hz = exp2(((double) base_key - 48.0) / 12.0) * 440.0;
  new_pitch_hz = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  for(i = 0; i < 2 * buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    t = (double) (i % 2) / 2.0;

    if((i / 2) + 1 < buffer_length){
      alias_source_buffer[i] = ((1.0 - t) * (source[(i / 2) * source_stride] + (t * source[((i / 2) + 1) * source_stride])));
    }
  }

  source_freq_period = (2 * samplerate) / root_pitch_hz;

  new_source_freq_period = (2 * samplerate) / new_pitch_hz;

  reset_i = -1;

  for(i = 0; i < 2 * buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    new_pitch_hz = exp2((base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0))  / 12.0) * 440.0;
      
    new_source_freq_period = (2 * samplerate) / new_pitch_hz;
    
    if(source_freq_period < new_source_freq_period){
      t = (i % (guint) source_freq_period) / new_source_freq_period;

      if((guint) floor(i * (source_freq_period / new_source_freq_period)) < 2 * buffer_length){
	alias_new_source_buffer[i] = ((1.0 - t) * alias_source_buffer[i]) + (t * alias_source_buffer[(guint) floor(i * (source_freq_period / new_source_freq_period))]);
      }
    }else{
      t = 1.0 / source_freq_period * new_source_freq_period;

      if(i * source_freq_period / new_source_freq_period < 2 * buffer_length){
	alias_new_source_buffer[i] = alias_source_buffer[(guint) floor(i * source_freq_period / new_source_freq_period)];
      }else{
	if(reset_i == -1){
	  reset_i = i;
	}

	if(reset_i != -1 && i - reset_i >= reset_i){
	  reset_i = i;
	}
	
	alias_new_source_buffer[i] = alias_source_buffer[(guint) floor((i - reset_i) * source_freq_period / new_source_freq_period)];
      }
    }
  }

  for(i = 0; i < buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    destination[i * destination_stride] = alias_new_source_buffer[2 * i];
  }
}

/**
 * ags_pitch_2x_alias_util_pitch_s64:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Pitch @pitch_2x_alias_util of signed 64 bit data.
 * 
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_pitch_s64(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  gint64 *destination, *source;
  gint64 *alias_source_buffer;
  gint64 *alias_new_source_buffer;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;
  gdouble new_pitch_hz;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint offset;
  gdouble source_freq_period;
  gdouble new_source_freq_period;
  gdouble t;
  gint reset_i;
  guint i;

  if(pitch_2x_alias_util == NULL ||
     pitch_2x_alias_util->destination == NULL ||
     pitch_2x_alias_util->source == NULL){
    return;
  }

  destination = pitch_2x_alias_util->destination;
  destination_stride = pitch_2x_alias_util->destination_stride;

  source = pitch_2x_alias_util->source;
  source_stride = pitch_2x_alias_util->source_stride;

  alias_source_buffer = pitch_2x_alias_util->alias_source_buffer;
  
  alias_new_source_buffer = pitch_2x_alias_util->alias_new_source_buffer;

  buffer_length = pitch_2x_alias_util->buffer_length;
  samplerate = pitch_2x_alias_util->samplerate;

  base_key = pitch_2x_alias_util->base_key;
  tuning = pitch_2x_alias_util->tuning;

  vibrato_gain = pitch_2x_alias_util->vibrato_gain;
  vibrato_lfo_depth = pitch_2x_alias_util->vibrato_lfo_depth;
  vibrato_lfo_freq = pitch_2x_alias_util->vibrato_lfo_freq;
  vibrato_tuning = pitch_2x_alias_util->vibrato_tuning;
  offset = pitch_2x_alias_util->offset;

  if(pitch_2x_alias_util->vibrato_enabled == FALSE){
    vibrato_gain = 0.0;
  }
  
  root_pitch_hz = exp2(((double) base_key - 48.0) / 12.0) * 440.0;
  new_pitch_hz = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  for(i = 0; i < 2 * buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    t = (double) (i % 2) / 2.0;

    if((i / 2) + 1 < buffer_length){
      alias_source_buffer[i] = ((1.0 - t) * (source[(i / 2) * source_stride] + (t * source[((i / 2) + 1) * source_stride])));
    }
  }

  source_freq_period = (2 * samplerate) / root_pitch_hz;

  new_source_freq_period = (2 * samplerate) / new_pitch_hz;

  reset_i = -1;

  for(i = 0; i < 2 * buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    new_pitch_hz = exp2((base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0))  / 12.0) * 440.0;
      
    new_source_freq_period = (2 * samplerate) / new_pitch_hz;
    
    if(source_freq_period < new_source_freq_period){
      t = (i % (guint) source_freq_period) / new_source_freq_period;

      if((guint) floor(i * (source_freq_period / new_source_freq_period)) < 2 * buffer_length){
	alias_new_source_buffer[i] = ((1.0 - t) * alias_source_buffer[i]) + (t * alias_source_buffer[(guint) floor(i * (source_freq_period / new_source_freq_period))]);
      }
    }else{
      t = 1.0 / source_freq_period * new_source_freq_period;

      if(i * source_freq_period / new_source_freq_period < 2 * buffer_length){
	alias_new_source_buffer[i] = alias_source_buffer[(guint) floor(i * source_freq_period / new_source_freq_period)];
      }else{
	if(reset_i == -1){
	  reset_i = i;
	}

	if(reset_i != -1 && i - reset_i >= reset_i){
	  reset_i = i;
	}
	
	alias_new_source_buffer[i] = alias_source_buffer[(guint) floor((i - reset_i) * source_freq_period / new_source_freq_period)];
      }
    }
  }

  for(i = 0; i < buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    destination[i * destination_stride] = alias_new_source_buffer[2 * i];
  }
}

/**
 * ags_pitch_2x_alias_util_pitch_float:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Pitch @pitch_2x_alias_util of single precision floating point numbers.
 * 
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_pitch_float(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  gfloat *destination, *source;
  gfloat *alias_source_buffer;
  gfloat *alias_new_source_buffer;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;
  gdouble new_pitch_hz;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint offset;
  gdouble source_freq_period;
  gdouble new_source_freq_period;
  gdouble t;
  gint reset_i;
  guint i;

  if(pitch_2x_alias_util == NULL ||
     pitch_2x_alias_util->destination == NULL ||
     pitch_2x_alias_util->source == NULL){
    return;
  }

  destination = pitch_2x_alias_util->destination;
  destination_stride = pitch_2x_alias_util->destination_stride;

  source = pitch_2x_alias_util->source;
  source_stride = pitch_2x_alias_util->source_stride;

  alias_source_buffer = pitch_2x_alias_util->alias_source_buffer;
  
  alias_new_source_buffer = pitch_2x_alias_util->alias_new_source_buffer;

  buffer_length = pitch_2x_alias_util->buffer_length;
  samplerate = pitch_2x_alias_util->samplerate;

  base_key = pitch_2x_alias_util->base_key;
  tuning = pitch_2x_alias_util->tuning;

  vibrato_gain = pitch_2x_alias_util->vibrato_gain;
  vibrato_lfo_depth = pitch_2x_alias_util->vibrato_lfo_depth;
  vibrato_lfo_freq = pitch_2x_alias_util->vibrato_lfo_freq;
  vibrato_tuning = pitch_2x_alias_util->vibrato_tuning;
  offset = pitch_2x_alias_util->offset;

  if(pitch_2x_alias_util->vibrato_enabled == FALSE){
    vibrato_gain = 0.0;
  }
  
  root_pitch_hz = exp2(((double) base_key - 48.0) / 12.0) * 440.0;
  new_pitch_hz = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  for(i = 0; i < 2 * buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    t = (double) (i % 2) / 2.0;

    if((i / 2) + 1 < buffer_length){
      alias_source_buffer[i] = ((1.0 - t) * (source[(i / 2) * source_stride] + (t * source[((i / 2) + 1) * source_stride])));
    }
  }

  source_freq_period = (2 * samplerate) / root_pitch_hz;

  new_source_freq_period = (2 * samplerate) / new_pitch_hz;

  reset_i = -1;

  for(i = 0; i < 2 * buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    new_pitch_hz = exp2((base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0))  / 12.0) * 440.0;
      
    new_source_freq_period = (2 * samplerate) / new_pitch_hz;
    
    if(source_freq_period < new_source_freq_period){
      t = (i % (guint) source_freq_period) / new_source_freq_period;

      if((guint) floor(i * (source_freq_period / new_source_freq_period)) < 2 * buffer_length){
	alias_new_source_buffer[i] = ((1.0 - t) * alias_source_buffer[i]) + (t * alias_source_buffer[(guint) floor(i * (source_freq_period / new_source_freq_period))]);
      }
    }else{
      t = 1.0 / source_freq_period * new_source_freq_period;

      if(i * source_freq_period / new_source_freq_period < 2 * buffer_length){
	alias_new_source_buffer[i] = alias_source_buffer[(guint) floor(i * source_freq_period / new_source_freq_period)];
      }else{
	if(reset_i == -1){
	  reset_i = i;
	}

	if(reset_i != -1 && i - reset_i >= reset_i){
	  reset_i = i;
	}
	
	alias_new_source_buffer[i] = alias_source_buffer[(guint) floor((i - reset_i) * source_freq_period / new_source_freq_period)];
      }
    }
  }

  for(i = 0; i < buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    destination[i * destination_stride] = alias_new_source_buffer[2 * i];
  }
}

/**
 * ags_pitch_2x_alias_util_pitch_double:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Pitch @pitch_2x_alias_util of double precision floating point numbers.
 * 
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_pitch_double(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  gdouble *destination, *source;
  gdouble *alias_source_buffer;
  gdouble *alias_new_source_buffer;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;
  gdouble new_pitch_hz;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint offset;
  gdouble source_freq_period;
  gdouble new_source_freq_period;
  gdouble t;
  gint reset_i;
  guint i;

  if(pitch_2x_alias_util == NULL ||
     pitch_2x_alias_util->destination == NULL ||
     pitch_2x_alias_util->source == NULL){
    return;
  }

  destination = pitch_2x_alias_util->destination;
  destination_stride = pitch_2x_alias_util->destination_stride;

  source = pitch_2x_alias_util->source;
  source_stride = pitch_2x_alias_util->source_stride;

  alias_source_buffer = pitch_2x_alias_util->alias_source_buffer;
  
  alias_new_source_buffer = pitch_2x_alias_util->alias_new_source_buffer;

  buffer_length = pitch_2x_alias_util->buffer_length;
  samplerate = pitch_2x_alias_util->samplerate;

  base_key = pitch_2x_alias_util->base_key;
  tuning = pitch_2x_alias_util->tuning;

  vibrato_gain = pitch_2x_alias_util->vibrato_gain;
  vibrato_lfo_depth = pitch_2x_alias_util->vibrato_lfo_depth;
  vibrato_lfo_freq = pitch_2x_alias_util->vibrato_lfo_freq;
  vibrato_tuning = pitch_2x_alias_util->vibrato_tuning;
  offset = pitch_2x_alias_util->offset;

  if(pitch_2x_alias_util->vibrato_enabled == FALSE){
    vibrato_gain = 0.0;
  }
  
  root_pitch_hz = exp2(((double) base_key - 48.0) / 12.0) * 440.0;
  new_pitch_hz = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  for(i = 0; i < 2 * buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    t = (double) (i % 2) / 2.0;

    if((i / 2) + 1 < buffer_length){
      alias_source_buffer[i] = ((1.0 - t) * (source[(i / 2) * source_stride] + (t * source[((i / 2) + 1) * source_stride])));
    }
  }

  source_freq_period = (2 * samplerate) / root_pitch_hz;

  new_source_freq_period = (2 * samplerate) / new_pitch_hz;

  reset_i = -1;

  for(i = 0; i < 2 * buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    new_pitch_hz = exp2((base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0))  / 12.0) * 440.0;
      
    new_source_freq_period = (2 * samplerate) / new_pitch_hz;
    
    if(source_freq_period < new_source_freq_period){
      t = (i % (guint) source_freq_period) / new_source_freq_period;

      if((guint) floor(i * (source_freq_period / new_source_freq_period)) < 2 * buffer_length){
	alias_new_source_buffer[i] = ((1.0 - t) * alias_source_buffer[i]) + (t * alias_source_buffer[(guint) floor(i * (source_freq_period / new_source_freq_period))]);
      }
    }else{
      t = 1.0 / source_freq_period * new_source_freq_period;

      if(i * source_freq_period / new_source_freq_period < 2 * buffer_length){
	alias_new_source_buffer[i] = alias_source_buffer[(guint) floor(i * source_freq_period / new_source_freq_period)];
      }else{
	if(reset_i == -1){
	  reset_i = i;
	}

	if(reset_i != -1 && i - reset_i >= reset_i){
	  reset_i = i;
	}
	
	alias_new_source_buffer[i] = alias_source_buffer[(guint) floor((i - reset_i) * source_freq_period / new_source_freq_period)];
      }
    }
  }

  for(i = 0; i < buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    destination[i * destination_stride] = alias_new_source_buffer[2 * i];
  }
}

/**
 * ags_pitch_2x_alias_util_pitch_complex:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Pitch @pitch_2x_alias_util of complex data type.
 * 
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_pitch_complex(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  AgsComplex *destination, *source;
  AgsComplex *alias_source_buffer;
  AgsComplex *alias_new_source_buffer;
  
  guint destination_stride, source_stride;
  guint buffer_length;
  guint samplerate;
  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;
  gdouble new_pitch_hz;
  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  guint offset;
  gdouble source_freq_period;
  gdouble new_source_freq_period;
  gdouble t;
  gint reset_i;
  guint i;

  if(pitch_2x_alias_util == NULL ||
     pitch_2x_alias_util->destination == NULL ||
     pitch_2x_alias_util->source == NULL){
    return;
  }

  destination = pitch_2x_alias_util->destination;
  destination_stride = pitch_2x_alias_util->destination_stride;

  source = pitch_2x_alias_util->source;
  source_stride = pitch_2x_alias_util->source_stride;

  alias_source_buffer = pitch_2x_alias_util->alias_source_buffer;
  
  alias_new_source_buffer = pitch_2x_alias_util->alias_new_source_buffer;

  buffer_length = pitch_2x_alias_util->buffer_length;
  samplerate = pitch_2x_alias_util->samplerate;

  base_key = pitch_2x_alias_util->base_key;
  tuning = pitch_2x_alias_util->tuning;

  vibrato_gain = pitch_2x_alias_util->vibrato_gain;
  vibrato_lfo_depth = pitch_2x_alias_util->vibrato_lfo_depth;
  vibrato_lfo_freq = pitch_2x_alias_util->vibrato_lfo_freq;
  vibrato_tuning = pitch_2x_alias_util->vibrato_tuning;
  offset = pitch_2x_alias_util->offset;

  if(pitch_2x_alias_util->vibrato_enabled == FALSE){
    vibrato_gain = 0.0;
  }
  
  root_pitch_hz = exp2(((double) base_key - 48.0) / 12.0) * 440.0;
  new_pitch_hz = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  for(i = 0; i < 2 * buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    t = (double) (i % 2) / 2.0;

    if((i / 2) + 1 < buffer_length){
      ags_complex_set(alias_source_buffer + i,
		      ((1.0 - t) * (ags_complex_get(source + ((i / 2) * source_stride)) + (t * ags_complex_get(source + (((i / 2) + 1) * source_stride))))));
    }
  }

  source_freq_period = (2 * samplerate) / root_pitch_hz;

  new_source_freq_period = (2 * samplerate) / new_pitch_hz;

  reset_i = -1;

  for(i = 0; i < 2 * buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    new_pitch_hz = exp2((base_key - 48.0 + ((tuning + 100.0 * (vibrato_gain * sin((offset) * 2.0 * M_PI * (vibrato_lfo_freq * (exp2(vibrato_tuning / 1200.0))) / samplerate) * vibrato_lfo_depth)) / 100.0))  / 12.0) * 440.0;
      
    new_source_freq_period = (2 * samplerate) / new_pitch_hz;
    
    if(source_freq_period < new_source_freq_period){
      t = (i % (guint) source_freq_period) / new_source_freq_period;

      if((guint) floor(i * (source_freq_period / new_source_freq_period)) < 2 * buffer_length){
	ags_complex_set(alias_new_source_buffer + i,
			((1.0 - t) * ags_complex_get(alias_source_buffer + i)) + (t * ags_complex_get(alias_source_buffer + ((guint) floor(i * (source_freq_period / new_source_freq_period))))));
      }
    }else{
      t = 1.0 / source_freq_period * new_source_freq_period;

      if(i * source_freq_period / new_source_freq_period < 2 * buffer_length){
	ags_complex_set(alias_new_source_buffer + i,
			ags_complex_get(alias_source_buffer + ((guint) floor(i * source_freq_period / new_source_freq_period))));
      }else{
	if(reset_i == -1){
	  reset_i = i;
	}

	if(reset_i != -1 && i - reset_i >= reset_i){
	  reset_i = i;
	}
	
	ags_complex_set(alias_new_source_buffer + i,
			ags_complex_get(alias_source_buffer + ((guint) floor((i - reset_i) * source_freq_period / new_source_freq_period))));
      }
    }
  }

  for(i = 0; i < buffer_length && i < AGS_PITCH_2X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE; i++){
    ags_complex_set(destination + (i * destination_stride),
		    ags_complex_get(alias_new_source_buffer + (2 * i)));
  }
}

/**
 * ags_pitch_2x_alias_util_pitch:
 * @pitch_2x_alias_util: the #AgsPitch2xAliasUtil-struct
 * 
 * Pitch @pitch_2x_alias_util.
 * 
 * Since: 6.15.0
 */
void
ags_pitch_2x_alias_util_pitch(AgsPitch2xAliasUtil *pitch_2x_alias_util)
{
  if(pitch_2x_alias_util == NULL ||
     pitch_2x_alias_util->destination == NULL ||
     pitch_2x_alias_util->source == NULL){
    return;
  }

  switch(pitch_2x_alias_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_pitch_2x_alias_util_pitch_s8(pitch_2x_alias_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_pitch_2x_alias_util_pitch_s16(pitch_2x_alias_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_pitch_2x_alias_util_pitch_s24(pitch_2x_alias_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_pitch_2x_alias_util_pitch_s32(pitch_2x_alias_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_pitch_2x_alias_util_pitch_s64(pitch_2x_alias_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_pitch_2x_alias_util_pitch_float(pitch_2x_alias_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_pitch_2x_alias_util_pitch_double(pitch_2x_alias_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_pitch_2x_alias_util_pitch_complex(pitch_2x_alias_util);
  }
  break;
  default:
    g_warning("unknown format");
  }
}
