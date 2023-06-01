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

#include <ags/audio/ags_tremolo_util.h>

#include <ags/audio/ags_audio_buffer_util.h>

#if defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#include <Accelerate/Accelerate.h>
#endif

/**
 * SECTION:ags_tremolo_util
 * @short_description: Boxed type of tremolo util
 * @title: AgsTremoloUtil
 * @section_id:
 * @include: ags/audio/ags_tremolo_util.h
 *
 * Boxed type of tremolo util data type.
 */

GType
ags_tremolo_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_tremolo_util = 0;

    ags_type_tremolo_util =
      g_boxed_type_register_static("AgsTremoloUtil",
				   (GBoxedCopyFunc) ags_tremolo_util_copy,
				   (GBoxedFreeFunc) ags_tremolo_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_tremolo_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_tremolo_util_alloc:
 *
 * Allocate #AgsTremoloUtil-struct
 *
 * Returns: a new #AgsTremoloUtil-struct
 *
 * Since: 5.2.0
 */
AgsTremoloUtil*
ags_tremolo_util_alloc()
{
  AgsTremoloUtil *ptr;

  ptr = (AgsTremoloUtil *) g_new(AgsTremoloUtil,
				 1);

  ptr->destination = NULL;
  ptr->destination_stride = 1;

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->buffer_length = 0;
  ptr->format = AGS_TREMOLO_UTIL_DEFAULT_FORMAT;
  ptr->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  ptr->tremolo_gain = 1.0;
  ptr->tremolo_lfo_depth = 0.0;
  ptr->tremolo_lfo_freq = 6.0;
  ptr->tremolo_tuning = 0.0;

  ptr->tremolo_lfo_frame_count = (guint) (ptr->samplerate / ptr->tremolo_lfo_freq);
  ptr->tremolo_lfo_offset = 0;

  return(ptr);
}

/**
 * ags_tremolo_util_copy:
 * @ptr: the original #AgsTremoloUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsTremoloUtil-struct
 *
 * Since: 5.2.0
 */
gpointer
ags_tremolo_util_copy(AgsTremoloUtil *ptr)
{
  AgsTremoloUtil *new_ptr;
  
  new_ptr = (AgsTremoloUtil *) g_new(AgsTremoloUtil,
				     1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->tremolo_gain = ptr->tremolo_gain;
  new_ptr->tremolo_lfo_depth = ptr->tremolo_lfo_depth;
  new_ptr->tremolo_lfo_freq = ptr->tremolo_lfo_freq;
  new_ptr->tremolo_tuning = ptr->tremolo_tuning;

  new_ptr->tremolo_lfo_frame_count = ptr->tremolo_lfo_frame_count;
  new_ptr->tremolo_lfo_offset = ptr->tremolo_lfo_offset;

  return(new_ptr);
}

/**
 * ags_tremolo_util_free:
 * @ptr: the #AgsTremoloUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_free(AgsTremoloUtil *ptr)
{
  g_free(ptr->destination);

  if(ptr->destination != ptr->source){
    g_free(ptr->source);
  }
  
  g_free(ptr);
}

/**
 * ags_tremolo_util_get_destination:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Get destination buffer of @tremolo_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 5.2.0
 */
gpointer
ags_tremolo_util_get_destination(AgsTremoloUtil *tremolo_util)
{
  if(tremolo_util == NULL){
    return(NULL);
  }

  return(tremolo_util->destination);
}

/**
 * ags_tremolo_util_set_destination:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * @destination: the destination buffer
 *
 * Set @destination buffer of @tremolo_util.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_set_destination(AgsTremoloUtil *tremolo_util,
				 gpointer destination)
{
  if(tremolo_util == NULL){
    return;
  }

  tremolo_util->destination = destination;
}

/**
 * ags_tremolo_util_get_destination_stride:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Get destination stride of @tremolo_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 5.2.0
 */
guint
ags_tremolo_util_get_destination_stride(AgsTremoloUtil *tremolo_util)
{
  if(tremolo_util == NULL){
    return(0);
  }

  return(tremolo_util->destination_stride);
}

/**
 * ags_tremolo_util_set_destination_stride:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @tremolo_util.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_set_destination_stride(AgsTremoloUtil *tremolo_util,
					guint destination_stride)
{
  if(tremolo_util == NULL){
    return;
  }

  tremolo_util->destination_stride = destination_stride;
}

/**
 * ags_tremolo_util_get_source:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Get source buffer of @tremolo_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 5.2.0
 */
gpointer
ags_tremolo_util_get_source(AgsTremoloUtil *tremolo_util)
{
  if(tremolo_util == NULL){
    return(NULL);
  }

  return(tremolo_util->source);
}

/**
 * ags_tremolo_util_set_source:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @tremolo_util.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_set_source(AgsTremoloUtil *tremolo_util,
			    gpointer source)
{
  if(tremolo_util == NULL){
    return;
  }

  tremolo_util->source = source;
}

/**
 * ags_tremolo_util_get_source_stride:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Get source stride of @tremolo_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 5.2.0
 */
guint
ags_tremolo_util_get_source_stride(AgsTremoloUtil *tremolo_util)
{
  if(tremolo_util == NULL){
    return(0);
  }

  return(tremolo_util->source_stride);
}

/**
 * ags_tremolo_util_set_source_stride:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @tremolo_util.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_set_source_stride(AgsTremoloUtil *tremolo_util,
				   guint source_stride)
{
  if(tremolo_util == NULL){
    return;
  }

  tremolo_util->source_stride = source_stride;
}

/**
 * ags_tremolo_util_get_buffer_length:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Get buffer length of @tremolo_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 5.2.0
 */
guint
ags_tremolo_util_get_buffer_length(AgsTremoloUtil *tremolo_util)
{
  if(tremolo_util == NULL){
    return(0);
  }

  return(tremolo_util->buffer_length);
}

/**
 * ags_tremolo_util_set_buffer_length:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @tremolo_util.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_set_buffer_length(AgsTremoloUtil *tremolo_util,
				   guint buffer_length)
{
  if(tremolo_util == NULL){
    return;
  }

  tremolo_util->buffer_length = buffer_length;
}

/**
 * ags_tremolo_util_get_format:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Get format of @tremolo_util.
 * 
 * Returns: the format
 * 
 * Since: 5.2.0
 */
AgsSoundcardFormat
ags_tremolo_util_get_format(AgsTremoloUtil *tremolo_util)
{
  if(tremolo_util == NULL){
    return(0);
  }

  return(tremolo_util->format);
}

/**
 * ags_tremolo_util_set_format:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * @format: the format
 *
 * Set @format of @tremolo_util.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_set_format(AgsTremoloUtil *tremolo_util,
			    AgsSoundcardFormat format)
{
  if(tremolo_util == NULL){
    return;
  }

  tremolo_util->format = format;
}

/**
 * ags_tremolo_util_get_samplerate:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Get buffer length of @tremolo_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 5.2.0
 */
guint
ags_tremolo_util_get_samplerate(AgsTremoloUtil *tremolo_util)
{
  if(tremolo_util == NULL){
    return(0);
  }

  return(tremolo_util->samplerate);
}

/**
 * ags_tremolo_util_set_samplerate:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * @samplerate: the buffer length
 *
 * Set @samplerate of @tremolo_util.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_set_samplerate(AgsTremoloUtil *tremolo_util,
				guint samplerate)
{
  if(tremolo_util == NULL){
    return;
  }

  tremolo_util->samplerate = samplerate;
}

/**
 * ags_tremolo_util_get_tremolo_gain:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Get tremolo gain of @tremolo_util.
 * 
 * Returns: the tremolo gain
 * 
 * Since: 5.2.0
 */
gdouble
ags_tremolo_util_get_tremolo_gain(AgsTremoloUtil *tremolo_util)
{
  if(tremolo_util == NULL){
    return(1.0);
  }

  return(tremolo_util->tremolo_gain);
}

/**
 * ags_tremolo_util_set_tremolo_gain:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * @tremolo_gain: the tremolo gain
 *
 * Set @tremolo_gain of @tremolo_util.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_set_tremolo_gain(AgsTremoloUtil *tremolo_util,
				  gdouble tremolo_gain)
{
  if(tremolo_util == NULL){
    return;
  }

  tremolo_util->tremolo_gain = tremolo_gain;
}

/**
 * ags_tremolo_util_get_tremolo_lfo_depth:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Get tremolo LFO depth of @tremolo_util.
 * 
 * Returns: the tremolo LFO depth
 * 
 * Since: 5.2.0
 */
gdouble
ags_tremolo_util_get_tremolo_lfo_depth(AgsTremoloUtil *tremolo_util)
{
  if(tremolo_util == NULL){
    return(1.0);
  }

  return(tremolo_util->tremolo_lfo_depth);
}

/**
 * ags_tremolo_util_set_tremolo_lfo_depth:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * @tremolo_lfo_depth: the tremolo LFO depth
 *
 * Set @tremolo_lfo_depth of @tremolo_util.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_set_tremolo_lfo_depth(AgsTremoloUtil *tremolo_util,
				       gdouble tremolo_lfo_depth)
{
  if(tremolo_util == NULL){
    return;
  }

  tremolo_util->tremolo_lfo_depth = tremolo_lfo_depth;
}

/**
 * ags_tremolo_util_get_tremolo_lfo_freq:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Get tremolo LFO freq of @tremolo_util.
 * 
 * Returns: the tremolo LFO freq
 * 
 * Since: 5.2.0
 */
gdouble
ags_tremolo_util_get_tremolo_lfo_freq(AgsTremoloUtil *tremolo_util)
{
  if(tremolo_util == NULL){
    return(1.0);
  }

  return(tremolo_util->tremolo_lfo_freq);
}

/**
 * ags_tremolo_util_set_tremolo_lfo_freq:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * @tremolo_lfo_freq: the tremolo LFO freq
 *
 * Set @tremolo_lfo_freq of @tremolo_util.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_set_tremolo_lfo_freq(AgsTremoloUtil *tremolo_util,
				      gdouble tremolo_lfo_freq)
{
  if(tremolo_util == NULL){
    return;
  }

  tremolo_util->tremolo_lfo_freq = tremolo_lfo_freq;
}

/**
 * ags_tremolo_util_get_tremolo_tuning:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Get tremolo tuning of @tremolo_util.
 * 
 * Returns: the tremolo tuning
 * 
 * Since: 5.2.0
 */
gdouble
ags_tremolo_util_get_tremolo_tuning(AgsTremoloUtil *tremolo_util)
{
  if(tremolo_util == NULL){
    return(1.0);
  }

  return(tremolo_util->tremolo_tuning);
}

/**
 * ags_tremolo_util_set_tremolo_tuning:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * @tremolo_tuning: the tremolo tuning
 *
 * Set @tremolo_tuning of @tremolo_util.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_set_tremolo_tuning(AgsTremoloUtil *tremolo_util,
				      gdouble tremolo_tuning)
{
  if(tremolo_util == NULL){
    return;
  }

  tremolo_util->tremolo_tuning = tremolo_tuning;
}

/**
 * ags_tremolo_util_get_tremolo_lfo_offset:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Get tremolo LFO offset of @tremolo_util.
 * 
 * Returns: the tremolo LFO offset
 * 
 * Since: 5.2.0
 */
guint
ags_tremolo_util_get_tremolo_lfo_offset(AgsTremoloUtil *tremolo_util)
{
  if(tremolo_util == NULL){
    return(0);
  }

  return(tremolo_util->tremolo_lfo_offset);
}

/**
 * ags_tremolo_util_set_tremolo_lfo_offset:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * @tremolo_lfo_offset: the tremolo LFO offset
 *
 * Set @tremolo_lfo_offset of @tremolo_util.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_set_tremolo_lfo_offset(AgsTremoloUtil *tremolo_util,
					guint tremolo_lfo_offset)
{
  if(tremolo_util == NULL){
    return;
  }

  tremolo_util->tremolo_lfo_offset = tremolo_lfo_offset;
}

/**
 * ags_tremolo_util_compute_s8:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Compute tremolo of signed 8 bit data.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_compute_s8(AgsTremoloUtil *tremolo_util)
{
  gint8 *destination;
  gint8 *source;

  gint source_stride, destination_stride;
  guint samplerate;
  gdouble tremolo_gain;
  gdouble tremolo_lfo_freq;
  gdouble tremolo_lfo_depth;
  gdouble tremolo_tuning;
  gdouble tremolo_lfo_offset;
  guint i, i_stop;

  if(tremolo_util == NULL ||
     tremolo_util->destination == NULL ||
     tremolo_util->source == NULL){
    return;
  }

  destination = (gint8 *) tremolo_util->destination;
  destination_stride = tremolo_util->destination_stride;
  
  source = (gint8 *) tremolo_util->source;
  source_stride = tremolo_util->source_stride;

  samplerate = tremolo_util->samplerate;

  tremolo_gain = tremolo_util->tremolo_gain;

  tremolo_lfo_freq = tremolo_util->tremolo_lfo_freq;
  tremolo_lfo_depth = tremolo_util->tremolo_lfo_depth;
  tremolo_tuning = tremolo_util->tremolo_tuning;

  tremolo_lfo_offset = tremolo_util->tremolo_lfo_offset;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_tremolo;

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

    v_tremolo = (ags_v8double) {
      (gdouble) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
    };
    		 
    source += source_stride;

    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }
    
    v_buffer *= v_tremolo;

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
    tremolo_util->tremolo_lfo_offset += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);
  
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

    double v_tremolo[] = {
      (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth))};

    source += source_stride;

    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }

    vDSP_vmulD(v_buffer, 1, v_tremolo, 1, ret_v_buffer, 1, 8);

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
    tremolo_util->tremolo_lfo_offset += 8;
  }
#else
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);

  for(; i < i_stop;){
    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }
    
    *(destination) = (gint8) ((gint16) ((source)[0] * (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin(tremolo_lfo_offset * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 1) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 2) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 3) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 4) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 5) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 6) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint8) ((gint16) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 7) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));

    destination += destination_stride;
    source += source_stride;
    i += 8;
    tremolo_util->tremolo_lfo_offset += 8;
  }
#endif

  /* loop tail */
  for(; i < tremolo_util->buffer_length;){
    *(destination) = (gint8) ((gint16) ((source)[0] * (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin(tremolo_lfo_offset * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));

    destination += destination_stride;
    source += source_stride;
    i++;
    tremolo_util->tremolo_lfo_offset += 1;
  }
}

/**
 * ags_tremolo_util_compute_s16:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Compute tremolo of signed 16 bit data.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_compute_s16(AgsTremoloUtil *tremolo_util)
{
  gint16 *destination;
  gint16 *source;

  gint source_stride, destination_stride;
  guint samplerate;
  gdouble tremolo_gain;
  gdouble tremolo_lfo_freq;
  gdouble tremolo_lfo_depth;
  gdouble tremolo_tuning;
  gdouble tremolo_lfo_offset;
  guint i, i_stop;

  if(tremolo_util == NULL ||
     tremolo_util->destination == NULL ||
     tremolo_util->source == NULL){
    return;
  }

  destination = (gint16 *) tremolo_util->destination;
  destination_stride = tremolo_util->destination_stride;

  source = (gint16 *) tremolo_util->source;
  source_stride = tremolo_util->source_stride;

  samplerate = tremolo_util->samplerate;

  tremolo_gain = tremolo_util->tremolo_gain;

  tremolo_lfo_freq = tremolo_util->tremolo_lfo_freq;
  tremolo_lfo_depth = tremolo_util->tremolo_lfo_depth;
  tremolo_tuning = tremolo_util->tremolo_tuning;

  tremolo_lfo_offset = tremolo_util->tremolo_lfo_offset;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_tremolo;

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
    
    v_tremolo = (ags_v8double) {
      (gdouble) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
    };
    
    source += source_stride;

    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }
    
    v_buffer *= v_tremolo;

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
    tremolo_util->tremolo_lfo_offset += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);
  
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

    double v_tremolo[] = {
      (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth))};

    source += source_stride;

    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }

    vDSP_vmulD(v_buffer, 1, v_tremolo, 1, ret_v_buffer, 1, 8);

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
    tremolo_util->tremolo_lfo_offset += 8;
  }
#else
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);

  for(; i < i_stop;){
    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }

    *(destination) = (gint16) ((gint32) ((source)[0] * (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin(tremolo_lfo_offset * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 1) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 2) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 3) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 4) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 5) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 6) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint16) ((gint32) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 7) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));

    destination += destination_stride;
    source += source_stride;
    i += 8;
    tremolo_util->tremolo_lfo_offset += 8;
  }
#endif

  /* loop tail */
  for(; i < tremolo_util->buffer_length;){
    *(destination) = (gint16) ((gint32) ((source)[0] * (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin(tremolo_lfo_offset * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));

    destination += destination_stride;
    source += source_stride;
    i++;
    tremolo_util->tremolo_lfo_offset += 1;
  }
}

/**
 * ags_tremolo_util_compute_s24:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Compute tremolo of signed 24 bit data.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_compute_s24(AgsTremoloUtil *tremolo_util)
{
  gint32 *destination;
  gint32 *source;

  gint source_stride, destination_stride;
  guint samplerate;
  gdouble tremolo_gain;
  gdouble tremolo_lfo_freq;
  gdouble tremolo_lfo_depth;
  gdouble tremolo_tuning;
  gdouble tremolo_lfo_offset;
  guint i, i_stop;

  if(tremolo_util == NULL ||
     tremolo_util->destination == NULL ||
     tremolo_util->source == NULL){
    return;
  }

  destination = (gint32 *) tremolo_util->destination;
  destination_stride = tremolo_util->destination_stride;

  source = (gint32 *) tremolo_util->source;
  source_stride = tremolo_util->source_stride;

  samplerate = tremolo_util->samplerate;

  tremolo_gain = tremolo_util->tremolo_gain;

  tremolo_lfo_freq = tremolo_util->tremolo_lfo_freq;
  tremolo_lfo_depth = tremolo_util->tremolo_lfo_depth;
  tremolo_tuning = tremolo_util->tremolo_tuning;

  tremolo_lfo_offset = tremolo_util->tremolo_lfo_offset;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_tremolo;

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
    
    v_tremolo = (ags_v8double) {
      (gdouble) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
    };
    
    source += source_stride;

    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }
    
    v_buffer *= v_tremolo;

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
    tremolo_util->tremolo_lfo_offset += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);
  
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

    double v_tremolo[] = {
      (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth))};

    source += source_stride;

    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }

    vDSP_vmulD(v_buffer, 1, v_tremolo, 1, ret_v_buffer, 1, 8);

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
    tremolo_util->tremolo_lfo_offset += 8;
  }
#else
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);

  for(; i < i_stop;){
    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }

    *(destination) = (gint32) ((gint32) ((source)[0] * (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin(tremolo_lfo_offset * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 1) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 2) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 3) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 4) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 5) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 6) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint32) ((gint32) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 7) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));

    destination += destination_stride;
    source += source_stride;
    i += 8;
    tremolo_util->tremolo_lfo_offset += 8;
  }
#endif

  /* loop tail */
  for(; i < tremolo_util->buffer_length;){
    *(destination) = (gint32) ((gint32) ((source)[0] * (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin(tremolo_lfo_offset * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));

    destination += destination_stride;
    source += source_stride;
    i++;
    tremolo_util->tremolo_lfo_offset += 1;
  }
}

/**
 * ags_tremolo_util_compute_s32:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Compute tremolo of signed 32 bit data.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_compute_s32(AgsTremoloUtil *tremolo_util)
{
  gint32 *destination;
  gint32 *source;

  gint source_stride, destination_stride;
  guint samplerate;
  gdouble tremolo_gain;
  gdouble tremolo_lfo_freq;
  gdouble tremolo_lfo_depth;
  gdouble tremolo_tuning;
  gdouble tremolo_lfo_offset;
  guint i, i_stop;

  if(tremolo_util == NULL ||
     tremolo_util->destination == NULL ||
     tremolo_util->source == NULL){
    return;
  }

  destination = (gint32 *) tremolo_util->destination;
  destination_stride = tremolo_util->destination_stride;

  source = (gint32 *) tremolo_util->source;
  source_stride = tremolo_util->source_stride;

  samplerate = tremolo_util->samplerate;

  tremolo_gain = tremolo_util->tremolo_gain;

  tremolo_lfo_freq = tremolo_util->tremolo_lfo_freq;
  tremolo_lfo_depth = tremolo_util->tremolo_lfo_depth;
  tremolo_tuning = tremolo_util->tremolo_tuning;

  tremolo_lfo_offset = tremolo_util->tremolo_lfo_offset;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_tremolo;

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
    
    v_tremolo = (ags_v8double) {
      (gdouble) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
    };
    
    source += source_stride;

    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }
    
    v_buffer *= v_tremolo;

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
    tremolo_util->tremolo_lfo_offset += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);
  
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

    double v_tremolo[] = {
      (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth))};

    source += source_stride;

    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }

    vDSP_vmulD(v_buffer, 1, v_tremolo, 1, ret_v_buffer, 1, 8);

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
    tremolo_util->tremolo_lfo_offset += 8;
  }
#else
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);

  for(; i < i_stop;){
    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }

    *(destination) = (gint32) ((gint64) ((source)[0] * (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin(tremolo_lfo_offset * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 1) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 2) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 3) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 4) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 5) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 6) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint32) ((gint64) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 7) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));

    destination += destination_stride;
    source += source_stride;
    i += 8;
    tremolo_util->tremolo_lfo_offset += 8;
  }
#endif

  /* loop tail */
  for(; i < tremolo_util->buffer_length;){
    *(destination) = (gint32) ((gint64) ((source)[0] * (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin(tremolo_lfo_offset * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));

    destination += destination_stride;
    source += source_stride;
    i++;
    tremolo_util->tremolo_lfo_offset += 1;
  }
}

/**
 * ags_tremolo_util_compute_s64:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Compute tremolo of signed 64 bit data.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_compute_s64(AgsTremoloUtil *tremolo_util)
{
  gint64 *destination;
  gint64 *source;

  gint source_stride, destination_stride;
  guint samplerate;
  gdouble tremolo_gain;
  gdouble tremolo_lfo_freq;
  gdouble tremolo_lfo_depth;
  gdouble tremolo_tuning;
  gdouble tremolo_lfo_offset;
  guint i, i_stop;

  if(tremolo_util == NULL ||
     tremolo_util->destination == NULL ||
     tremolo_util->source == NULL){
    return;
  }

  destination = (gint64 *) tremolo_util->destination;
  destination_stride = tremolo_util->destination_stride;

  source = (gint64 *) tremolo_util->source;
  source_stride = tremolo_util->source_stride;

  samplerate = tremolo_util->samplerate;

  tremolo_gain = tremolo_util->tremolo_gain;

  tremolo_lfo_freq = tremolo_util->tremolo_lfo_freq;
  tremolo_lfo_depth = tremolo_util->tremolo_lfo_depth;
  tremolo_tuning = tremolo_util->tremolo_tuning;

  tremolo_lfo_offset = tremolo_util->tremolo_lfo_offset;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_tremolo;

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
    
    v_tremolo = (ags_v8double) {
      (gdouble) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
    };
    
    source += source_stride;

    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }
    
    v_buffer *= v_tremolo;

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
    tremolo_util->tremolo_lfo_offset += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);
  
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

    double v_tremolo[] = {
      (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth))};

    source += source_stride;

    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }

    vDSP_vmulD(v_buffer, 1, v_tremolo, 1, ret_v_buffer, 1, 8);

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
    tremolo_util->tremolo_lfo_offset += 8;
  }
#else
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);

  for(; i < i_stop;){
    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }

    *(destination) = (gint64) ((gint64) ((source)[0] * (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin(tremolo_lfo_offset * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 1) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 2) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 3) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 4) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 5) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 6) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));
    *(destination += destination_stride) = (gint64) ((gint64) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 7) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));

    destination += destination_stride;
    source += source_stride;
    i += 8;
    tremolo_util->tremolo_lfo_offset += 8;
  }
#endif

  /* loop tail */
  for(; i < tremolo_util->buffer_length;){
    *(destination) = (gint64) ((gint64) ((source)[0] * (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin(tremolo_lfo_offset * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate)))));

    destination += destination_stride;
    source += source_stride;
    i++;
    tremolo_util->tremolo_lfo_offset += 1;
  }
}

/**
 * ags_tremolo_util_compute_float:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Compute tremolo of floating point data.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_compute_float(AgsTremoloUtil *tremolo_util)
{
  gfloat *destination;
  gfloat *source;

  gint source_stride, destination_stride;
  guint samplerate;
  gdouble tremolo_gain;
  gdouble tremolo_lfo_freq;
  gdouble tremolo_lfo_depth;
  gdouble tremolo_tuning;
  gdouble tremolo_lfo_offset;
  guint i, i_stop;

  if(tremolo_util == NULL ||
     tremolo_util->destination == NULL ||
     tremolo_util->source == NULL){
    return;
  }

  destination = (gfloat *) tremolo_util->destination;
  destination_stride = tremolo_util->destination_stride;

  source = (gfloat *) tremolo_util->source;
  source_stride = tremolo_util->source_stride;

  samplerate = tremolo_util->samplerate;

  tremolo_gain = tremolo_util->tremolo_gain;

  tremolo_lfo_freq = tremolo_util->tremolo_lfo_freq;
  tremolo_lfo_depth = tremolo_util->tremolo_lfo_depth;
  tremolo_tuning = tremolo_util->tremolo_tuning;

  tremolo_lfo_offset = tremolo_util->tremolo_lfo_offset;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_tremolo;

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
    
    v_tremolo = (ags_v8double) {
      (gdouble) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
    };
    
    source += source_stride;

    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }
    
    v_buffer *= v_tremolo;

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
    tremolo_util->tremolo_lfo_offset += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);
  
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

    double v_tremolo[] = {
      (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth))};

    source += source_stride;

    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }

    vDSP_vmulD(v_buffer, 1, v_tremolo, 1, ret_v_buffer, 1, 8);

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
    tremolo_util->tremolo_lfo_offset += 8;
  }
#else
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);

  for(; i < i_stop;){
    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }

    *(destination) = (gfloat) ((source)[0] * (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin(tremolo_lfo_offset * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate))));
    *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 1) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate))));
    *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 2) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate))));
    *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 3) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate))));
    *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 4) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate))));
    *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 5) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate))));
    *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 6) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate))));
    *(destination += destination_stride) = (gfloat) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 7) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate))));

    destination += destination_stride;
    source += source_stride;
    i += 8;
    tremolo_util->tremolo_lfo_offset += 8;
  }
#endif

  /* loop tail */
  for(; i < tremolo_util->buffer_length;){
    *(destination) = (gfloat) ((source)[0] * (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin(tremolo_lfo_offset * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate))));

    destination += destination_stride;
    source += source_stride;
    i++;
    tremolo_util->tremolo_lfo_offset += 1;
  }
}

/**
 * ags_tremolo_util_compute_double:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Compute tremolo of double floating point data.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_compute_double(AgsTremoloUtil *tremolo_util)
{
  gdouble *destination;
  gdouble *source;

  gint source_stride, destination_stride;
  guint samplerate;
  gdouble tremolo_gain;
  gdouble tremolo_lfo_freq;
  gdouble tremolo_lfo_depth;
  gdouble tremolo_tuning;
  gdouble tremolo_lfo_offset;
  guint i, i_stop;

  if(tremolo_util == NULL ||
     tremolo_util->destination == NULL ||
     tremolo_util->source == NULL){
    return;
  }

  destination = (gdouble *) tremolo_util->destination;
  destination_stride = tremolo_util->destination_stride;

  source = (gdouble *) tremolo_util->source;
  source_stride = tremolo_util->source_stride;

  samplerate = tremolo_util->samplerate;

  tremolo_gain = tremolo_util->tremolo_gain;

  tremolo_lfo_freq = tremolo_util->tremolo_lfo_freq;
  tremolo_lfo_depth = tremolo_util->tremolo_lfo_depth;
  tremolo_tuning = tremolo_util->tremolo_tuning;

  tremolo_lfo_offset = tremolo_util->tremolo_lfo_offset;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_tremolo;

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

    v_tremolo = (ags_v8double) {
      (gdouble) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (gdouble) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
    };
    
    source += source_stride;

    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }
    
    v_buffer *= v_tremolo;

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
    tremolo_util->tremolo_lfo_offset += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);
  
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

    double v_tremolo[] = {
      (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)),
      (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth))};

    source += source_stride;

    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }

    vDSP_vmulD(v_buffer, 1, v_tremolo, 1, ret_v_buffer, 1, 8);

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
    tremolo_util->tremolo_lfo_offset += 8;
  }
#else
  i_stop = tremolo_util->buffer_length - (tremolo_util->buffer_length % 8);

  for(; i < i_stop;){
    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }

    *(destination) = (gdouble) ((source)[0] * (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin(tremolo_lfo_offset * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate))));
    *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 1) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 1) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate))));
    *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 2) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 2) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate))));
    *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 3) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 3) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate))));
    *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 4) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 4) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate))));
    *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 5) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 5) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate))));
    *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 6) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 6) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate))));
    *(destination += destination_stride) = (gdouble) ((source += source_stride)[0] * (double) (tremolo_gain * sin((gdouble) ((tremolo_lfo_offset + 7) * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin((tremolo_lfo_offset + 7) * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate))));

    destination += destination_stride;
    source += source_stride;
    i += 8;
    tremolo_util->tremolo_lfo_offset += 8;
  }
#endif

  /* loop tail */
  for(; i < tremolo_util->buffer_length;){
    *(destination) = (gdouble) ((source)[0] * (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0 + sin(tremolo_lfo_offset * 2.0 * M_PI * tremolo_lfo_freq / samplerate) * tremolo_lfo_depth)) / (gdouble) samplerate))));
    
    destination += destination_stride;
    source += source_stride;
    i++;
    tremolo_util->tremolo_lfo_offset += 1;
  }
}

/**
 * ags_tremolo_util_compute_complex:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Compute tremolo of complex floating point data.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_compute_complex(AgsTremoloUtil *tremolo_util)
{
  AgsComplex *destination;
  AgsComplex *source;

  gint source_stride, destination_stride;
  guint samplerate;
  gdouble tremolo_gain;
  gdouble tremolo_lfo_freq;
  gdouble tremolo_lfo_depth;
  gdouble tremolo_tuning;
  gdouble tremolo_lfo_offset;
  guint i;

  if(tremolo_util == NULL ||
     tremolo_util->destination == NULL ||
     tremolo_util->source == NULL){
    return;
  }
  
  destination = (AgsComplex *) tremolo_util->destination;
  destination_stride = tremolo_util->destination_stride;

  source = (AgsComplex *) tremolo_util->source;
  source_stride = tremolo_util->source_stride;

  samplerate = tremolo_util->samplerate;

  tremolo_gain = tremolo_util->tremolo_gain;

  tremolo_lfo_freq = tremolo_util->tremolo_lfo_freq;
  tremolo_lfo_depth = tremolo_util->tremolo_lfo_depth;
  tremolo_tuning = tremolo_util->tremolo_tuning;

  tremolo_lfo_offset = tremolo_util->tremolo_lfo_offset;
  
  i = 0;
  
  for(; i < tremolo_util->buffer_length;){
    double _Complex z;

    if(tremolo_util->tremolo_lfo_offset > tremolo_util->tremolo_lfo_frame_count){
      tremolo_util->tremolo_lfo_offset = 0;
    }

    z = ags_complex_get(source);

    ags_complex_set(destination,
		    z * (double) (tremolo_gain * sin((gdouble) (tremolo_lfo_offset * 2.0 * M_PI * (tremolo_lfo_freq * exp2(tremolo_tuning / 1200.0)) / samplerate) * tremolo_lfo_depth)));

    destination += destination_stride;
    source += source_stride;
    i++;
    tremolo_util->tremolo_lfo_offset += 1;
  }
}

/**
 * ags_tremolo_util_compute:
 * @tremolo_util: the #AgsTremoloUtil-struct
 * 
 * Compute tremolo.
 *
 * Since: 5.2.0
 */
void
ags_tremolo_util_compute(AgsTremoloUtil *tremolo_util)
{
  if(tremolo_util == NULL ||
     tremolo_util->destination == NULL ||
     tremolo_util->source == NULL){
    return;
  }

  switch(tremolo_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      ags_tremolo_util_compute_s8(tremolo_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      ags_tremolo_util_compute_s16(tremolo_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      ags_tremolo_util_compute_s24(tremolo_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      ags_tremolo_util_compute_s32(tremolo_util);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      ags_tremolo_util_compute_s64(tremolo_util);
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      ags_tremolo_util_compute_float(tremolo_util);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      ags_tremolo_util_compute_double(tremolo_util);
    }
    break;
  case AGS_SOUNDCARD_COMPLEX:
    {
      ags_tremolo_util_compute_complex(tremolo_util);
    }
    break;
  }
}
