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

#include <ags/audio/ags_wah_wah_util.h>

#include <ags/audio/ags_audio_buffer_util.h>

#if defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#include <Accelerate/Accelerate.h>
#endif

/**
 * SECTION:ags_wah_wah_util
 * @short_description: Boxed type of wah_wah util
 * @title: AgsWahWahUtil
 * @section_id:
 * @include: ags/audio/ags_wah_wah_util.h
 *
 * Boxed type of wah_wah util data type.
 */

GType
ags_wah_wah_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_wah_wah_util = 0;

    ags_type_wah_wah_util =
      g_boxed_type_register_static("AgsWahWahUtil",
				   (GBoxedCopyFunc) ags_wah_wah_util_copy,
				   (GBoxedFreeFunc) ags_wah_wah_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_wah_wah_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_wah_wah_util_alloc:
 *
 * Allocate #AgsWahWahUtil-struct
 *
 * Returns: a new #AgsWahWahUtil-struct
 *
 * Since: 3.9.2
 */
AgsWahWahUtil*
ags_wah_wah_util_alloc()
{
  AgsWahWahUtil *ptr;

  ptr = (AgsWahWahUtil *) g_new(AgsWahWahUtil,
				1);

  ptr->destination = NULL;
  ptr->destination_stride = 1;

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->buffer_length = 0;
  ptr->format = AGS_WAH_WAH_UTIL_DEFAULT_FORMAT;
  
  ptr->volume = 1.0;
  ptr->amount = 0.0;

  ptr->wah_wah_enabled = FALSE;

  ptr->wah_wah_delay = 0.0;

  ptr->wah_wah_lfo_depth = 1.0;
  ptr->wah_wah_lfo_freq = 6.0;
  ptr->wah_wah_tuning = 0.0;

  ptr->wah_wah_lfo_frame_count = (guint) (ptr->samplerate / ptr->wah_wah_lfo_freq);
  ptr->wah_wah_lfo_offset = 0;

  return(ptr);
}

/**
 * ags_wah_wah_util_copy:
 * @ptr: the original #AgsWahWahUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsWahWahUtil-struct
 *
 * Since: 3.9.2
 */
gpointer
ags_wah_wah_util_copy(AgsWahWahUtil *ptr)
{
  AgsWahWahUtil *new_ptr;
  
  new_ptr = (AgsWahWahUtil *) g_new(AgsWahWahUtil,
				    1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;

  new_ptr->volume = ptr->volume;
  new_ptr->amount = ptr->amount;

  new_ptr->wah_wah_enabled = ptr->wah_wah_enabled;
  
  new_ptr->wah_wah_lfo_depth = ptr->wah_wah_lfo_depth;
  new_ptr->wah_wah_lfo_freq = ptr->wah_wah_lfo_freq;
  new_ptr->wah_wah_tuning = ptr->wah_wah_tuning;

  new_ptr->wah_wah_lfo_frame_count = ptr->wah_wah_lfo_frame_count;
  new_ptr->wah_wah_lfo_offset = ptr->wah_wah_lfo_offset;

  return(new_ptr);
}

/**
 * ags_wah_wah_util_free:
 * @ptr: the #AgsWahWahUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.9.2
 */
void
ags_wah_wah_util_free(AgsWahWahUtil *ptr)
{
  g_free(ptr->destination);

  if(ptr->destination != ptr->source){
    g_free(ptr->source);
  }

  g_free(ptr);
}

/**
 * ags_wah_wah_util_get_destination:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Get destination buffer of @wah_wah_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 3.9.2
 */
gpointer
ags_wah_wah_util_get_destination(AgsWahWahUtil *wah_wah_util)
{
  if(wah_wah_util == NULL){
    return(NULL);
  }

  return(wah_wah_util->destination);
}

/**
 * ags_wah_wah_util_set_destination:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * @destination: the destination buffer
 *
 * Set @destination buffer of @wah_wah_util.
 *
 * Since: 3.9.2
 */
void
ags_wah_wah_util_set_destination(AgsWahWahUtil *wah_wah_util,
				 gpointer destination)
{
  if(wah_wah_util == NULL){
    return;
  }

  wah_wah_util->destination = destination;
}

/**
 * ags_wah_wah_util_get_destination_stride:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Get destination stride of @wah_wah_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 3.9.2
 */
guint
ags_wah_wah_util_get_destination_stride(AgsWahWahUtil *wah_wah_util)
{
  if(wah_wah_util == NULL){
    return(0);
  }

  return(wah_wah_util->destination_stride);
}

/**
 * ags_wah_wah_util_set_destination_stride:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @wah_wah_util.
 *
 * Since: 3.9.2
 */
void
ags_wah_wah_util_set_destination_stride(AgsWahWahUtil *wah_wah_util,
					guint destination_stride)
{
  if(wah_wah_util == NULL){
    return;
  }

  wah_wah_util->destination_stride = destination_stride;
}

/**
 * ags_wah_wah_util_get_source:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Get source buffer of @wah_wah_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.9.2
 */
gpointer
ags_wah_wah_util_get_source(AgsWahWahUtil *wah_wah_util)
{
  if(wah_wah_util == NULL){
    return(NULL);
  }

  return(wah_wah_util->source);
}

/**
 * ags_wah_wah_util_set_source:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @wah_wah_util.
 *
 * Since: 3.9.2
 */
void
ags_wah_wah_util_set_source(AgsWahWahUtil *wah_wah_util,
			    gpointer source)
{
  if(wah_wah_util == NULL){
    return;
  }

  wah_wah_util->source = source;
}

/**
 * ags_wah_wah_util_get_source_stride:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Get source stride of @wah_wah_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.9.2
 */
guint
ags_wah_wah_util_get_source_stride(AgsWahWahUtil *wah_wah_util)
{
  if(wah_wah_util == NULL){
    return(0);
  }

  return(wah_wah_util->source_stride);
}

/**
 * ags_wah_wah_util_set_source_stride:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @wah_wah_util.
 *
 * Since: 3.9.2
 */
void
ags_wah_wah_util_set_source_stride(AgsWahWahUtil *wah_wah_util,
				   guint source_stride)
{
  if(wah_wah_util == NULL){
    return;
  }

  wah_wah_util->source_stride = source_stride;
}

/**
 * ags_wah_wah_util_get_buffer_length:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Get buffer length of @wah_wah_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.9.2
 */
guint
ags_wah_wah_util_get_buffer_length(AgsWahWahUtil *wah_wah_util)
{
  if(wah_wah_util == NULL){
    return(0);
  }

  return(wah_wah_util->buffer_length);
}

/**
 * ags_wah_wah_util_set_buffer_length:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @wah_wah_util.
 *
 * Since: 3.9.2
 */
void
ags_wah_wah_util_set_buffer_length(AgsWahWahUtil *wah_wah_util,
				   guint buffer_length)
{
  if(wah_wah_util == NULL){
    return;
  }

  wah_wah_util->buffer_length = buffer_length;
}

/**
 * ags_wah_wah_util_get_format:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Get format of @wah_wah_util.
 * 
 * Returns: the format
 * 
 * Since: 3.9.6
 */
guint
ags_wah_wah_util_get_format(AgsWahWahUtil *wah_wah_util)
{
  if(wah_wah_util == NULL){
    return(0);
  }

  return(wah_wah_util->format);
}

/**
 * ags_wah_wah_util_set_format:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * @format: the format
 *
 * Set @format of @wah_wah_util.
 *
 * Since: 3.9.6
 */
void
ags_wah_wah_util_set_format(AgsWahWahUtil *wah_wah_util,
			    guint format)
{
  if(wah_wah_util == NULL){
    return;
  }

  wah_wah_util->format = format;
}

/**
 * ags_wah_wah_util_get_samplerate:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Get samplerate of @wah_wah_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 5.3.2
 */
guint
ags_wah_wah_util_get_samplerate(AgsWahWahUtil *wah_wah_util)
{
  if(wah_wah_util == NULL){
    return(0);
  }

  return(wah_wah_util->samplerate);
}

/**
 * ags_wah_wah_util_set_samplerate:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @wah_wah_util.
 *
 * Since: 5.3.2
 */
void
ags_wah_wah_util_set_samplerate(AgsWahWahUtil *wah_wah_util,
				guint samplerate)
{
  if(wah_wah_util == NULL){
    return;
  }

  wah_wah_util->samplerate = samplerate;
}

/**
 * ags_wah_wah_util_get_volume:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Get volume of @wah_wah_util.
 * 
 * Returns: the volume
 * 
 * Since: 3.9.2
 */
gdouble
ags_wah_wah_util_get_volume(AgsWahWahUtil *wah_wah_util)
{
  if(wah_wah_util == NULL){
    return(1.0);
  }

  return(wah_wah_util->volume);
}

/**
 * ags_wah_wah_util_set_volume:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * @volume: the volume
 *
 * Set @volume of @wah_wah_util.
 *
 * Since: 3.9.2
 */
void
ags_wah_wah_util_set_volume(AgsWahWahUtil *wah_wah_util,
			    gdouble volume)
{
  if(wah_wah_util == NULL){
    return;
  }

  wah_wah_util->volume = volume;
}

/**
 * ags_wah_wah_util_get_amount:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Get amount of @wah_wah_util.
 * 
 * Returns: the amount
 * 
 * Since: 3.9.2
 */
gdouble
ags_wah_wah_util_get_amount(AgsWahWahUtil *wah_wah_util)
{
  if(wah_wah_util == NULL){
    return(0.0);
  }

  return(wah_wah_util->amount);
}

/**
 * ags_wah_wah_util_set_amount:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * @amount: the amount
 *
 * Set @amount of @wah_wah_util.
 *
 * Since: 3.9.2
 */
void
ags_wah_wah_util_set_amount(AgsWahWahUtil *wah_wah_util,
			    gdouble amount)
{
  if(wah_wah_util == NULL){
    return;
  }

  wah_wah_util->amount = amount;
}

/**
 * ags_wah_wah_util_get_wah_wah_enabled:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Get wah-wah enabled of @wah_wah_util.
 * 
 * Returns: the wah-wah enabled
 * 
 * Since: 5.3.2
 */
gboolean 
ags_wah_wah_util_get_wah_wah_enabled(AgsWahWahUtil *wah_wah_util)
{
  if(wah_wah_util == NULL){
    return(FALSE);
  }

  return(wah_wah_util->wah_wah_enabled);
}

/**
 * ags_wah_wah_util_set_wah_wah_enabled:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * @wah_wah_enabled: the wah-wah enabled
 *
 * Set @wah_wah_enabled of @wah_wah_util.
 *
 * Since: 5.3.2
 */
void
ags_wah_wah_util_set_wah_wah_enabled(AgsWahWahUtil *wah_wah_util,
				     gboolean wah_wah_enabled)
{
  if(wah_wah_util == NULL){
    return;
  }

  wah_wah_util->wah_wah_enabled = wah_wah_enabled;
}

/**
 * ags_wah_wah_util_get_wah_wah_lfo_depth:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Get wah-wah LFO depth of @wah_wah_util.
 * 
 * Returns: the wah-wah LFO depth
 * 
 * Since: 5.3.2
 */
gdouble
ags_wah_wah_util_get_wah_wah_lfo_depth(AgsWahWahUtil *wah_wah_util)
{
  if(wah_wah_util == NULL){
    return(1.0);
  }

  return(wah_wah_util->wah_wah_lfo_depth);
}

/**
 * ags_wah_wah_util_set_wah_wah_lfo_depth:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * @wah_wah_lfo_depth: the wah-wah LFO depth
 *
 * Set @wah_wah_lfo_depth of @wah_wah_util.
 *
 * Since: 5.3.2
 */
void
ags_wah_wah_util_set_wah_wah_lfo_depth(AgsWahWahUtil *wah_wah_util,
				       gdouble wah_wah_lfo_depth)
{
  if(wah_wah_util == NULL){
    return;
  }

  wah_wah_util->wah_wah_lfo_depth = wah_wah_lfo_depth;
}

/**
 * ags_wah_wah_util_get_wah_wah_lfo_freq:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Get wah-wah LFO freq of @wah_wah_util.
 * 
 * Returns: the wah-wah LFO freq
 * 
 * Since: 5.3.2
 */
gdouble
ags_wah_wah_util_get_wah_wah_lfo_freq(AgsWahWahUtil *wah_wah_util)
{
  if(wah_wah_util == NULL){
    return(1.0);
  }

  return(wah_wah_util->wah_wah_lfo_freq);
}

/**
 * ags_wah_wah_util_set_wah_wah_lfo_freq:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * @wah_wah_lfo_freq: the wah-wah LFO freq
 *
 * Set @wah_wah_lfo_freq of @wah_wah_util.
 *
 * Since: 5.3.2
 */
void
ags_wah_wah_util_set_wah_wah_lfo_freq(AgsWahWahUtil *wah_wah_util,
				      gdouble wah_wah_lfo_freq)
{
  if(wah_wah_util == NULL){
    return;
  }

  wah_wah_util->wah_wah_lfo_freq = wah_wah_lfo_freq;
}

/**
 * ags_wah_wah_util_get_wah_wah_tuning:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Get wah-wah tuning of @wah_wah_util.
 * 
 * Returns: the wah-wah tuning
 * 
 * Since: 5.3.2
 */
gdouble
ags_wah_wah_util_get_wah_wah_tuning(AgsWahWahUtil *wah_wah_util)
{
  if(wah_wah_util == NULL){
    return(1.0);
  }

  return(wah_wah_util->wah_wah_tuning);
}

/**
 * ags_wah_wah_util_set_wah_wah_tuning:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * @wah_wah_tuning: the wah-wah tuning
 *
 * Set @wah_wah_tuning of @wah_wah_util.
 *
 * Since: 5.3.2
 */
void
ags_wah_wah_util_set_wah_wah_tuning(AgsWahWahUtil *wah_wah_util,
				    gdouble wah_wah_tuning)
{
  if(wah_wah_util == NULL){
    return;
  }

  wah_wah_util->wah_wah_tuning = wah_wah_tuning;
}

/**
 * ags_wah_wah_util_get_wah_wah_lfo_offset:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Get wah-wah LFO offset of @wah_wah_util.
 * 
 * Returns: the wah-wah LFO offset
 * 
 * Since: 5.2.4
 */
guint
ags_wah_wah_util_get_wah_wah_lfo_offset(AgsWahWahUtil *wah_wah_util)
{
  if(wah_wah_util == NULL){
    return(0);
  }

  return(wah_wah_util->wah_wah_lfo_offset);
}

/**
 * ags_wah_wah_util_set_wah_wah_lfo_offset:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * @wah_wah_lfo_offset: the wah-wah LFO offset
 *
 * Set @wah_wah_lfo_offset of @wah_wah_util.
 *
 * Since: 5.2.4
 */
void
ags_wah_wah_util_set_wah_wah_lfo_offset(AgsWahWahUtil *wah_wah_util,
					guint wah_wah_lfo_offset)
{
  if(wah_wah_util == NULL){
    return;
  }

  wah_wah_util->wah_wah_lfo_offset = wah_wah_lfo_offset;
}

/**
 * ags_wah_wah_util_compute_s8:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Compute wah_wah of signed 8 bit data.
 *
 * Since: 3.9.2
 */
void
ags_wah_wah_util_compute_s8(AgsWahWahUtil *wah_wah_util)
{
  gint8 *destination;
  gint8 *source;

  guint source_stride, destination_stride;
  guint samplerate;
  gdouble start_volume;
  gdouble amount;
  gdouble wah_wah_delay;
  gdouble wah_wah_lfo_freq;
  gdouble wah_wah_lfo_depth;
  gdouble wah_wah_tuning;
  gint64 wah_wah_lfo_offset;
  guint i, i_stop;

  if(wah_wah_util == NULL ||
     wah_wah_util->destination == NULL ||
     wah_wah_util->source == NULL){
    return;
  }

  destination = (gint8 *) wah_wah_util->destination;
  source = (gint8 *) wah_wah_util->source;
  
  source_stride = wah_wah_util->source_stride;
  destination_stride = wah_wah_util->destination_stride;

  samplerate = wah_wah_util->samplerate;

  start_volume = wah_wah_util->volume;
  amount = wah_wah_util->amount;

  wah_wah_delay = wah_wah_util->wah_wah_delay;

  wah_wah_lfo_freq = wah_wah_util->wah_wah_lfo_freq;
  wah_wah_lfo_depth = wah_wah_util->wah_wah_lfo_depth;
  wah_wah_tuning = wah_wah_util->wah_wah_tuning;
  wah_wah_lfo_offset = wah_wah_util->wah_wah_lfo_offset;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;

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

    v_volume = (ags_v8double) {
			       (gdouble) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
    };

    v_buffer *= v_volume;

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
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);
  
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

    double v_volume[] = {(double) start_volume + i * amount,
			 (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))};

    source += source_stride;

    vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);

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
#else
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint8) ((gint16) ((source)[0] * (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));

    destination += destination_stride;
    source += source_stride;

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < wah_wah_util->buffer_length;){
    *(destination) = (gint8) ((gint16) ((source)[0] * (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));

    destination += destination_stride;
    source += source_stride;
    i++;
  }

  wah_wah_util->volume = start_volume + i * amount;
}

/**
 * ags_wah_wah_util_compute_s16:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Compute wah_wah of signed 16 bit data.
 *
 * Since: 3.9.2
 */
void
ags_wah_wah_util_compute_s16(AgsWahWahUtil *wah_wah_util)
{
  gint16 *destination;
  gint16 *source;

  guint source_stride, destination_stride;
  guint samplerate;
  gdouble start_volume;
  gdouble amount;
  gdouble wah_wah_delay;
  gdouble wah_wah_lfo_freq;
  gdouble wah_wah_lfo_depth;
  gdouble wah_wah_tuning;
  gint64 wah_wah_lfo_offset;
  guint i, i_stop;

  if(wah_wah_util == NULL ||
     wah_wah_util->destination == NULL ||
     wah_wah_util->source == NULL){
    return;
  }

  destination = (gint16 *) wah_wah_util->destination;
  source = (gint16 *) wah_wah_util->source;
  
  source_stride = wah_wah_util->source_stride;
  destination_stride = wah_wah_util->destination_stride;

  samplerate = wah_wah_util->samplerate;

  start_volume = wah_wah_util->volume;
  amount = wah_wah_util->amount;

  wah_wah_delay = wah_wah_util->wah_wah_delay;

  wah_wah_lfo_freq = wah_wah_util->wah_wah_lfo_freq;
  wah_wah_lfo_depth = wah_wah_util->wah_wah_lfo_depth;
  wah_wah_tuning = wah_wah_util->wah_wah_tuning;
  wah_wah_lfo_offset = wah_wah_util->wah_wah_lfo_offset;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;

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

    v_volume = (ags_v8double) {
			       (gdouble) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))
    };

    v_buffer *= v_volume;

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
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);
  
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

    double v_volume[] = {(double) start_volume + i * amount,
			 (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))};

    source += source_stride;

    vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);

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
#else
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint16) ((gint32) ((source)[0] * (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));

    destination += destination_stride;
    source += source_stride;

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < wah_wah_util->buffer_length;){
    *(destination) = (gint16) ((gint32) ((source)[0] * (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));

    destination += destination_stride;
    source += source_stride;
    i++;
  }

  wah_wah_util->volume = start_volume + i * amount;
}

/**
 * ags_wah_wah_util_compute_s24:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Compute wah_wah of signed 24 bit data.
 *
 * Since: 3.9.2
 */
void
ags_wah_wah_util_compute_s24(AgsWahWahUtil *wah_wah_util)
{
  gint32 *destination;
  gint32 *source;

  guint source_stride, destination_stride;
  guint samplerate;
  gdouble start_volume;
  gdouble amount;
  gdouble wah_wah_delay;
  gdouble wah_wah_lfo_freq;
  gdouble wah_wah_lfo_depth;
  gdouble wah_wah_tuning;
  gint64 wah_wah_lfo_offset;
  guint i, i_stop;

  if(wah_wah_util == NULL ||
     wah_wah_util->destination == NULL ||
     wah_wah_util->source == NULL){
    return;
  }

  destination = (gint32 *) wah_wah_util->destination;
  source = (gint32 *) wah_wah_util->source;
  
  source_stride = wah_wah_util->source_stride;
  destination_stride = wah_wah_util->destination_stride;

  samplerate = wah_wah_util->samplerate;

  start_volume = wah_wah_util->volume;
  amount = wah_wah_util->amount;

  wah_wah_delay = wah_wah_util->wah_wah_delay;

  wah_wah_lfo_freq = wah_wah_util->wah_wah_lfo_freq;
  wah_wah_lfo_depth = wah_wah_util->wah_wah_lfo_depth;
  wah_wah_tuning = wah_wah_util->wah_wah_tuning;
  wah_wah_lfo_offset = wah_wah_util->wah_wah_lfo_offset;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;

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

    v_volume = (ags_v8double) {
			       (gdouble) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))
    };

    v_buffer *= v_volume;

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
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);
  
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

    double v_volume[] = {(double) start_volume + i * amount,
			 (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))};

    source += source_stride;

    vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);

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
#else
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint32) ((gint32) ((source)[0] * (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));

    destination += destination_stride;
    source += source_stride;

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < wah_wah_util->buffer_length;){
    destination[0] = (gint32) ((gint32) (source[0] * (start_volume + i * amount)));

    destination += destination_stride;
    source += source_stride;
    i++;
  }

  wah_wah_util->volume = start_volume + i * amount;
}

/**
 * ags_wah_wah_util_compute_s32:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Compute wah_wah of signed 32 bit data.
 *
 * Since: 3.9.2
 */
void
ags_wah_wah_util_compute_s32(AgsWahWahUtil *wah_wah_util)
{
  gint32 *destination;
  gint32 *source;

  guint source_stride, destination_stride;
  guint samplerate;
  gdouble start_volume;
  gdouble amount;
  gdouble wah_wah_delay;
  gdouble wah_wah_lfo_freq;
  gdouble wah_wah_lfo_depth;
  gdouble wah_wah_tuning;
  gint64 wah_wah_lfo_offset;
  guint i, i_stop;

  if(wah_wah_util == NULL ||
     wah_wah_util->destination == NULL ||
     wah_wah_util->source == NULL){
    return;
  }

  destination = (gint32 *) wah_wah_util->destination;
  source = (gint32 *) wah_wah_util->source;
  
  source_stride = wah_wah_util->source_stride;
  destination_stride = wah_wah_util->destination_stride;

  samplerate = wah_wah_util->samplerate;

  start_volume = wah_wah_util->volume;
  amount = wah_wah_util->amount;

  wah_wah_delay = wah_wah_util->wah_wah_delay;

  wah_wah_lfo_freq = wah_wah_util->wah_wah_lfo_freq;
  wah_wah_lfo_depth = wah_wah_util->wah_wah_lfo_depth;
  wah_wah_tuning = wah_wah_util->wah_wah_tuning;
  wah_wah_lfo_offset = wah_wah_util->wah_wah_lfo_offset;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;

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

    v_volume = (ags_v8double) {
			       (gdouble) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))
    };

    v_buffer *= v_volume;

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
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);
  
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

    double v_volume[] = {(double) start_volume + i * amount,
			 (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))};

    source += source_stride;

    vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);

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
#else
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint32) ((gint64) ((source)[0] * (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));

    destination += destination_stride;
    source += source_stride;

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < wah_wah_util->buffer_length;){
    *(destination) = (gint32) ((gint64) ((source)[0] * (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));

    destination += destination_stride;
    source += source_stride;
    i++;
  }

  wah_wah_util->volume = start_volume + i * amount;
}

/**
 * ags_wah_wah_util_compute_s64:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Compute wah_wah of signed 64 bit data.
 *
 * Since: 3.9.2
 */
void
ags_wah_wah_util_compute_s64(AgsWahWahUtil *wah_wah_util)
{
  gint64 *destination;
  gint64 *source;

  guint source_stride, destination_stride;
  guint samplerate;
  gdouble start_volume;
  gdouble amount;
  gdouble wah_wah_delay;
  gdouble wah_wah_lfo_freq;
  gdouble wah_wah_lfo_depth;
  gdouble wah_wah_tuning;
  gint64 wah_wah_lfo_offset;
  guint i, i_stop;

  if(wah_wah_util == NULL ||
     wah_wah_util->destination == NULL ||
     wah_wah_util->source == NULL){
    return;
  }

  destination = (gint64 *) wah_wah_util->destination;
  source = (gint64 *) wah_wah_util->source;
  
  source_stride = wah_wah_util->source_stride;
  destination_stride = wah_wah_util->destination_stride;

  samplerate = wah_wah_util->samplerate;

  start_volume = wah_wah_util->volume;
  amount = wah_wah_util->amount;

  wah_wah_delay = wah_wah_util->wah_wah_delay;

  wah_wah_lfo_freq = wah_wah_util->wah_wah_lfo_freq;
  wah_wah_lfo_depth = wah_wah_util->wah_wah_lfo_depth;
  wah_wah_tuning = wah_wah_util->wah_wah_tuning;
  wah_wah_lfo_offset = wah_wah_util->wah_wah_lfo_offset;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;

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

    v_volume = (ags_v8double) {
			       (gdouble) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))
    };

    v_buffer *= v_volume;

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
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);
  
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

    double v_volume[] = {(double) start_volume + i * amount,
			 (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))};

    source += source_stride;

    vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);

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
#else
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint64) ((gint64) ((source)[0] * (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));

    destination += destination_stride;
    source += source_stride;

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < wah_wah_util->buffer_length;){
    *(destination) = (gint64) ((gint64) ((source)[0] * (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))));

    destination += destination_stride;
    source += source_stride;
    i++;
  }

  wah_wah_util->volume = start_volume + i * amount;
}

/**
 * ags_wah_wah_util_compute_float:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Compute wah_wah of floating point data.
 *
 * Since: 3.9.2
 */
void
ags_wah_wah_util_compute_float(AgsWahWahUtil *wah_wah_util)
{
  gfloat *destination;
  gfloat *source;

  guint source_stride, destination_stride;
  guint samplerate;
  gdouble start_volume;
  gdouble amount;
  gdouble wah_wah_delay;
  gdouble wah_wah_lfo_freq;
  gdouble wah_wah_lfo_depth;
  gdouble wah_wah_tuning;
  gint64 wah_wah_lfo_offset;
  guint i, i_stop;

  if(wah_wah_util == NULL ||
     wah_wah_util->destination == NULL ||
     wah_wah_util->source == NULL){
    return;
  }

  destination = (gfloat *) wah_wah_util->destination;
  source = (gfloat *) wah_wah_util->source;
  
  source_stride = wah_wah_util->source_stride;
  destination_stride = wah_wah_util->destination_stride;

  samplerate = wah_wah_util->samplerate;

  start_volume = wah_wah_util->volume;
  amount = wah_wah_util->amount;

  wah_wah_delay = wah_wah_util->wah_wah_delay;

  wah_wah_lfo_freq = wah_wah_util->wah_wah_lfo_freq;
  wah_wah_lfo_depth = wah_wah_util->wah_wah_lfo_depth;
  wah_wah_tuning = wah_wah_util->wah_wah_tuning;
  wah_wah_lfo_offset = wah_wah_util->wah_wah_lfo_offset;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;

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

    v_volume = (ags_v8double) {
			       (gdouble) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))
    };

    v_buffer *= v_volume;

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
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);
  
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

    double v_volume[] = {(double) start_volume + i * amount,
			 (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))};

    source += source_stride;

    vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);

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
#else
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gfloat) ((source)[0] * (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));
    *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));
    *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));
    *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));
    *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));
    *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));
    *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));
    *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));

    destination += destination_stride;
    source += source_stride;

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < wah_wah_util->buffer_length;){
    *(destination) = (gfloat) ((source)[0] * (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));

    destination += destination_stride;
    source += source_stride;
    i++;
  }

  wah_wah_util->volume = start_volume + i * amount;
}

/**
 * ags_wah_wah_util_compute_double:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Compute wah_wah of double floating point data.
 *
 * Since: 3.9.2
 */
void
ags_wah_wah_util_compute_double(AgsWahWahUtil *wah_wah_util)
{
  gdouble *destination;
  gdouble *source;

  guint source_stride, destination_stride;
  guint samplerate;
  gdouble start_volume;
  gdouble amount;
  gdouble wah_wah_delay;
  gdouble wah_wah_lfo_freq;
  gdouble wah_wah_lfo_depth;
  gdouble wah_wah_tuning;
  gint64 wah_wah_lfo_offset;
  guint i, i_stop;

  if(wah_wah_util == NULL ||
     wah_wah_util->destination == NULL ||
     wah_wah_util->source == NULL){
    return;
  }

  destination = (gdouble *) wah_wah_util->destination;
  source = (gdouble *) wah_wah_util->source;
  
  source_stride = wah_wah_util->source_stride;
  destination_stride = wah_wah_util->destination_stride;

  samplerate = wah_wah_util->samplerate;

  start_volume = wah_wah_util->volume;
  amount = wah_wah_util->amount;

  wah_wah_delay = wah_wah_util->wah_wah_delay;

  wah_wah_lfo_freq = wah_wah_util->wah_wah_lfo_freq;
  wah_wah_lfo_depth = wah_wah_util->wah_wah_lfo_depth;
  wah_wah_tuning = wah_wah_util->wah_wah_tuning;
  wah_wah_lfo_offset = wah_wah_util->wah_wah_lfo_offset;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;

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

    v_volume = (ags_v8double) {
			       (gdouble) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			       (gdouble) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))
    };

    v_buffer *= v_volume;

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
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);
  
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

    double v_volume[] = {(double) start_volume + i * amount,
			 (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)),
			 (double) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth))};

    source += source_stride;

    vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);

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
#else
  i_stop = wah_wah_util->buffer_length - (wah_wah_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gdouble) ((source)[0] * (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));
    *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (start_volume + (i + 1) * sin((gdouble) ((wah_wah_lfo_offset + i + 1) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));
    *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (start_volume + (i + 2) * sin((gdouble) ((wah_wah_lfo_offset + i + 2) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));
    *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (start_volume + (i + 3) * sin((gdouble) ((wah_wah_lfo_offset + i + 3) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));
    *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (start_volume + (i + 4) * sin((gdouble) ((wah_wah_lfo_offset + i + 4) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));
    *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (start_volume + (i + 5) * sin((gdouble) ((wah_wah_lfo_offset + i + 5) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));
    *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (start_volume + (i + 6) * sin((gdouble) ((wah_wah_lfo_offset + i + 6) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));
    *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (start_volume + (i + 7) * sin((gdouble) ((wah_wah_lfo_offset + i + 7) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));

    destination += destination_stride;
    source += source_stride;

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < wah_wah_util->buffer_length;){
    *(destination) = (gdouble) ((source)[0] * (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));

    destination += destination_stride;
    source += source_stride;
    i++;
  }

  wah_wah_util->volume = start_volume + i * amount;
}

/**
 * ags_wah_wah_util_compute_complex:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Compute wah_wah of complex floating point data.
 *
 * Since: 3.9.2
 */
void
ags_wah_wah_util_compute_complex(AgsWahWahUtil *wah_wah_util)
{
  AgsComplex *destination;
  AgsComplex *source;

  guint source_stride, destination_stride;
  guint samplerate;
  gdouble start_volume;
  gdouble amount;
  gdouble wah_wah_delay;
  gdouble wah_wah_lfo_freq;
  gdouble wah_wah_lfo_depth;
  gdouble wah_wah_tuning;
  gint64 wah_wah_lfo_offset;
  guint i;

  if(wah_wah_util == NULL ||
     wah_wah_util->destination == NULL ||
     wah_wah_util->source == NULL){
    return;
  }
  
  destination = (AgsComplex *) wah_wah_util->destination;
  source = (AgsComplex *) wah_wah_util->source;
  
  source_stride = wah_wah_util->source_stride;
  destination_stride = wah_wah_util->destination_stride;

  samplerate = wah_wah_util->samplerate;

  start_volume = wah_wah_util->volume;
  amount = wah_wah_util->amount;

  wah_wah_delay = wah_wah_util->wah_wah_delay;

  wah_wah_lfo_freq = wah_wah_util->wah_wah_lfo_freq;
  wah_wah_lfo_depth = wah_wah_util->wah_wah_lfo_depth;
  wah_wah_tuning = wah_wah_util->wah_wah_tuning;
  wah_wah_lfo_offset = wah_wah_util->wah_wah_lfo_offset;

  i = 0;
  
  for(; i < wah_wah_util->buffer_length;){
    double _Complex z;

    z = ags_complex_get(source);

    ags_complex_set(destination,
		    z * (double) (start_volume + (i) * sin((gdouble) ((wah_wah_lfo_offset + i) * amount * 2.0 * M_PI * (wah_wah_lfo_freq * exp2(wah_wah_tuning / 1200.0)) / samplerate) * wah_wah_lfo_depth)));

    destination += destination_stride;
    source += source_stride;
    i++;
  }

  wah_wah_util->volume = start_volume + i * amount;
}

/**
 * ags_wah_wah_util_compute:
 * @wah_wah_util: the #AgsWahWahUtil-struct
 * 
 * Compute wah_wah.
 *
 * Since: 3.9.2
 */
void
ags_wah_wah_util_compute(AgsWahWahUtil *wah_wah_util)
{
  if(wah_wah_util == NULL ||
     wah_wah_util->destination == NULL ||
     wah_wah_util->source == NULL){
    return;
  }

  switch(wah_wah_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      ags_wah_wah_util_compute_s8(wah_wah_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      ags_wah_wah_util_compute_s16(wah_wah_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      ags_wah_wah_util_compute_s24(wah_wah_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      ags_wah_wah_util_compute_s32(wah_wah_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      ags_wah_wah_util_compute_s64(wah_wah_util);
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      ags_wah_wah_util_compute_float(wah_wah_util);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      ags_wah_wah_util_compute_double(wah_wah_util);
    }
    break;
  case AGS_SOUNDCARD_COMPLEX:
    {
      ags_wah_wah_util_compute_complex(wah_wah_util);
    }
    break;
  }
}
