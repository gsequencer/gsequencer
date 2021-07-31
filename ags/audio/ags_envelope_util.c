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

#include <ags/audio/ags_envelope_util.h>

#include <ags/audio/ags_audio_buffer_util.h>

/**
 * SECTION:ags_envelope_util
 * @short_description: Boxed type of envelope util
 * @title: AgsEnvelopeUtil
 * @section_id:
 * @include: ags/audio/ags_envelope_util.h
 *
 * Boxed type of envelope util data type.
 */

GType
ags_envelope_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_envelope_util = 0;

    ags_type_envelope_util =
      g_boxed_type_register_static("AgsEnvelopeUtil",
				   (GBoxedCopyFunc) ags_envelope_util_copy,
				   (GBoxedFreeFunc) ags_envelope_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_envelope_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_envelope_util_alloc:
 *
 * Allocate #AgsEnvelopeUtil-struct
 *
 * Returns: a new #AgsEnvelopeUtil-struct
 *
 * Since: 3.9.2
 */
AgsEnvelopeUtil*
ags_envelope_util_alloc()
{
  AgsEnvelopeUtil *ptr;

  ptr = (AgsEnvelopeUtil *) g_new(AgsEnvelopeUtil,
				  1);

  ptr->destination = NULL;
  ptr->destination_stride = 1;

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->buffer_length = 0;
  ptr->format = AGS_ENVELOPE_UTIL_DEFAULT_FORMAT;

  ptr->audio_buffer_util_format = AGS_ENVELOPE_UTIL_DEFAULT_AUDIO_BUFFER_UTIL_FORMAT;
  
  ptr->volume = 1.0;
  ptr->amount = 0.0;

  return(ptr);
}

/**
 * ags_envelope_util_copy:
 * @ptr: the original #AgsEnvelopeUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsEnvelopeUtil-struct
 *
 * Since: 3.9.2
 */
gpointer
ags_envelope_util_copy(AgsEnvelopeUtil *ptr)
{
  AgsEnvelopeUtil *new_ptr;
  
  new_ptr = (AgsEnvelopeUtil *) g_new(AgsEnvelopeUtil,
				      1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;

  new_ptr->audio_buffer_util_format = ptr->audio_buffer_util_format;

  new_ptr->volume = ptr->volume;
  new_ptr->amount = ptr->amount;

  return(new_ptr);
}

/**
 * ags_envelope_util_free:
 * @ptr: the #AgsEnvelopeUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_free(AgsEnvelopeUtil *ptr)
{
  g_free(ptr->destination);

  if(ptr->destination != ptr->source){
    g_free(ptr->source);
  }

  g_free(ptr);
}

/**
 * ags_envelope_util_get_destination:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get destination buffer of @envelope_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 3.9.2
 */
gpointer
ags_envelope_util_get_destination(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(NULL);
  }

  return(envelope_util->destination);
}

/**
 * ags_envelope_util_set_destination:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @destination: the destination buffer
 *
 * Set @destination buffer of @envelope_util.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_set_destination(AgsEnvelopeUtil *envelope_util,
				  gpointer destination)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->destination = destination;
}

/**
 * ags_envelope_util_get_destination_stride:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get destination stride of @envelope_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 3.9.2
 */
guint
ags_envelope_util_get_destination_stride(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(0);
  }

  return(envelope_util->destination_stride);
}

/**
 * ags_envelope_util_set_destination_stride:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @envelope_util.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_set_destination_stride(AgsEnvelopeUtil *envelope_util,
					 guint destination_stride)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->destination_stride = destination_stride;
}

/**
 * ags_envelope_util_get_source:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get source buffer of @envelope_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.9.2
 */
gpointer
ags_envelope_util_get_source(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(NULL);
  }

  return(envelope_util->source);
}

/**
 * ags_envelope_util_set_source:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @envelope_util.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_set_source(AgsEnvelopeUtil *envelope_util,
			     gpointer source)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->source = source;
}

/**
 * ags_envelope_util_get_source_stride:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get source stride of @envelope_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.9.2
 */
guint
ags_envelope_util_get_source_stride(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(0);
  }

  return(envelope_util->source_stride);
}

/**
 * ags_envelope_util_set_source_stride:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @envelope_util.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_set_source_stride(AgsEnvelopeUtil *envelope_util,
				    guint source_stride)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->source_stride = source_stride;
}

/**
 * ags_envelope_util_get_buffer_length:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get buffer length of @envelope_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.9.2
 */
guint
ags_envelope_util_get_buffer_length(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(0);
  }

  return(envelope_util->buffer_length);
}

/**
 * ags_envelope_util_set_buffer_length:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @envelope_util.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_set_buffer_length(AgsEnvelopeUtil *envelope_util,
				    guint buffer_length)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->buffer_length = buffer_length;
}

/**
 * ags_envelope_util_get_format:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get format of @envelope_util.
 * 
 * Returns: the format
 * 
 * Since: 3.9.6
 */
guint
ags_envelope_util_get_format(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(0);
  }

  return(envelope_util->format);
}

/**
 * ags_envelope_util_set_format:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @format: the format
 *
 * Set @format of @envelope_util.
 *
 * Since: 3.9.6
 */
void
ags_envelope_util_set_format(AgsEnvelopeUtil *envelope_util,
			     guint format)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->format = format;

  envelope_util->audio_buffer_util_format = ags_audio_buffer_util_format_from_soundcard(format);
}

/**
 * ags_envelope_util_get_audio_buffer_util_format:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get audio buffer util format of @envelope_util.
 * 
 * Returns: the audio buffer util format
 * 
 * Since: 3.9.2
 */
guint
ags_envelope_util_get_audio_buffer_util_format(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(0);
  }

  return(envelope_util->audio_buffer_util_format);
}

/**
 * ags_envelope_util_set_audio_buffer_util_format:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @audio_buffer_util_format: the audio buffer util format
 *
 * Set @audio_buffer_util_format of @envelope_util.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_set_audio_buffer_util_format(AgsEnvelopeUtil *envelope_util,
					       guint audio_buffer_util_format)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->audio_buffer_util_format = audio_buffer_util_format;
}

/**
 * ags_envelope_util_get_volume:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get volume of @envelope_util.
 * 
 * Returns: the volume
 * 
 * Since: 3.9.2
 */
gdouble
ags_envelope_util_get_volume(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(1.0);
  }

  return(envelope_util->volume);
}

/**
 * ags_envelope_util_set_volume:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @volume: the volume
 *
 * Set @volume of @envelope_util.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_set_volume(AgsEnvelopeUtil *envelope_util,
			     gdouble volume)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->volume = volume;
}

/**
 * ags_envelope_util_get_amount:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Get amount of @envelope_util.
 * 
 * Returns: the amount
 * 
 * Since: 3.9.2
 */
gdouble
ags_envelope_util_get_amount(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL){
    return(0.0);
  }

  return(envelope_util->amount);
}

/**
 * ags_envelope_util_set_amount:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * @amount: the amount
 *
 * Set @amount of @envelope_util.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_set_amount(AgsEnvelopeUtil *envelope_util,
			     gdouble amount)
{
  if(envelope_util == NULL){
    return;
  }

  envelope_util->amount = amount;
}

/**
 * ags_envelope_util_compute_s8:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Compute envelope of signed 8 bit data.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_compute_s8(AgsEnvelopeUtil *envelope_util)
{
  gint8 *destination;
  gint8 *source;
  gdouble start_volume;
  guint i, i_stop;

  if(envelope_util == NULL ||
     envelope_util->destination == NULL ||
     envelope_util->source == NULL){
    return;
  }

  destination = (gint8 *) envelope_util->destination;
  source = (gint8 *) envelope_util->source;
  
  start_volume = envelope_util->volume;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride)
    };

    source += envelope_util->source_stride;

    v_volume = (ags_v8double) {
      (gdouble) start_volume + i * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount
    };

    i++;
    
    v_buffer *= v_volume;

    *(destination) = (gint8) v_buffer[0];
    *(destination += envelope_util->destination_stride) = (gint8) v_buffer[1];
    *(destination += envelope_util->destination_stride) = (gint8) v_buffer[2];
    *(destination += envelope_util->destination_stride) = (gint8) v_buffer[3];
    *(destination += envelope_util->destination_stride) = (gint8) v_buffer[4];
    *(destination += envelope_util->destination_stride) = (gint8) v_buffer[5];
    *(destination += envelope_util->destination_stride) = (gint8) v_buffer[6];
    *(destination += envelope_util->destination_stride) = (gint8) v_buffer[7];

    destination += envelope_util->destination_stride;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {(double) buffer[0],
      (double) *(source),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride)};

    double v_volume[] = {(double) start_volume + i * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount};

    source += envelope_util->source_stride;
    i++;

    vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);

    *(destination) = (gint8) ret_v_buffer[0];
    *(destination += envelope_util->destination_stride) = (gint8) ret_v_buffer[1];
    *(destination += envelope_util->destination_stride) = (gint8) ret_v_buffer[2];
    *(destination += envelope_util->destination_stride) = (gint8) ret_v_buffer[3];
    *(destination += envelope_util->destination_stride) = (gint8) ret_v_buffer[4];
    *(destination += envelope_util->destination_stride) = (gint8) ret_v_buffer[5];
    *(destination += envelope_util->destination_stride) = (gint8) ret_v_buffer[6];
    *(destination += envelope_util->destination_stride) = (gint8) ret_v_buffer[7];

    destination += envelope_util->destination_stride;
  }
#else
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint8) ((gint16) ((source)[0] * (start_volume + i * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint8) ((gint16) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint8) ((gint16) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint8) ((gint16) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint8) ((gint16) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint8) ((gint16) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint8) ((gint16) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint8) ((gint16) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));

    destination += envelope_util->destination_stride;
    source += envelope_util->source_stride;
    i++;
  }
#endif

  /* loop tail */
  for(; i < envelope_util->buffer_length;){
    destination[0] = (gint8) ((gint16) (source[0] * (start_volume + i * envelope_util->amount)));

    destination += envelope_util->destination_stride;
    source += envelope_util->source_stride;
    i++;
  }

  envelope_util->volume = start_volume + i * envelope_util->amount;
}

/**
 * ags_envelope_util_compute_s16:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Compute envelope of signed 16 bit data.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_compute_s16(AgsEnvelopeUtil *envelope_util)
{
  gint16 *destination;
  gint16 *source;
  gdouble start_volume;
  guint i, i_stop;

  if(envelope_util == NULL ||
     envelope_util->destination == NULL ||
     envelope_util->source == NULL){
    return;
  }

  destination = (gint16 *) envelope_util->destination;
  source = (gint16 *) envelope_util->source;
  
  start_volume = envelope_util->volume;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride)
    };

    source += envelope_util->source_stride;

    v_volume = (ags_v8double) {
      (gdouble) start_volume + i * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount
    };

    i++;
    
    v_buffer *= v_volume;

    *(destination) = (gint16) v_buffer[0];
    *(destination += envelope_util->destination_stride) = (gint16) v_buffer[1];
    *(destination += envelope_util->destination_stride) = (gint16) v_buffer[2];
    *(destination += envelope_util->destination_stride) = (gint16) v_buffer[3];
    *(destination += envelope_util->destination_stride) = (gint16) v_buffer[4];
    *(destination += envelope_util->destination_stride) = (gint16) v_buffer[5];
    *(destination += envelope_util->destination_stride) = (gint16) v_buffer[6];
    *(destination += envelope_util->destination_stride) = (gint16) v_buffer[7];

    destination += envelope_util->destination_stride;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {(double) buffer[0],
      (double) *(source),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride)};

    double v_volume[] = {(double) start_volume + i * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount};

    source += envelope_util->source_stride;
    i++;

    vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);

    *(destination) = (gint16) ret_v_buffer[0];
    *(destination += envelope_util->destination_stride) = (gint16) ret_v_buffer[1];
    *(destination += envelope_util->destination_stride) = (gint16) ret_v_buffer[2];
    *(destination += envelope_util->destination_stride) = (gint16) ret_v_buffer[3];
    *(destination += envelope_util->destination_stride) = (gint16) ret_v_buffer[4];
    *(destination += envelope_util->destination_stride) = (gint16) ret_v_buffer[5];
    *(destination += envelope_util->destination_stride) = (gint16) ret_v_buffer[6];
    *(destination += envelope_util->destination_stride) = (gint16) ret_v_buffer[7];

    destination += envelope_util->destination_stride;
  }
#else
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint16) ((gint32) ((source)[0] * (start_volume + i * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint16) ((gint32) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint16) ((gint32) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint16) ((gint32) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint16) ((gint32) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint16) ((gint32) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint16) ((gint32) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint16) ((gint32) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));

    destination += envelope_util->destination_stride;
    source += envelope_util->source_stride;
    i++;
  }
#endif

  /* loop tail */
  for(; i < envelope_util->buffer_length;){
    destination[0] = (gint16) ((gint32) (source[0] * (start_volume + i * envelope_util->amount)));

    destination += envelope_util->destination_stride;
    source += envelope_util->source_stride;
    i++;
  }

  envelope_util->volume = start_volume + i * envelope_util->amount;
}

/**
 * ags_envelope_util_compute_s24:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Compute envelope of signed 24 bit data.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_compute_s24(AgsEnvelopeUtil *envelope_util)
{
  gint32 *destination;
  gint32 *source;
  gdouble start_volume;
  guint i, i_stop;

  if(envelope_util == NULL ||
     envelope_util->destination == NULL ||
     envelope_util->source == NULL){
    return;
  }

  destination = (gint32 *) envelope_util->destination;
  source = (gint32 *) envelope_util->source;
  
  start_volume = envelope_util->volume;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride)
    };

    source += envelope_util->source_stride;

    v_volume = (ags_v8double) {
      (gdouble) start_volume + i * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount
    };

    i++;
    
    v_buffer *= v_volume;

    *(destination) = (gint32) v_buffer[0];
    *(destination += envelope_util->destination_stride) = (gint32) v_buffer[1];
    *(destination += envelope_util->destination_stride) = (gint32) v_buffer[2];
    *(destination += envelope_util->destination_stride) = (gint32) v_buffer[3];
    *(destination += envelope_util->destination_stride) = (gint32) v_buffer[4];
    *(destination += envelope_util->destination_stride) = (gint32) v_buffer[5];
    *(destination += envelope_util->destination_stride) = (gint32) v_buffer[6];
    *(destination += envelope_util->destination_stride) = (gint32) v_buffer[7];

    destination += envelope_util->destination_stride;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {(double) buffer[0],
      (double) *(source),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride)};

    double v_volume[] = {(double) start_volume + i * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount};

    source += envelope_util->source_stride;
    i++;

    vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);

    *(destination) = (gint32) ret_v_buffer[0];
    *(destination += envelope_util->destination_stride) = (gint32) ret_v_buffer[1];
    *(destination += envelope_util->destination_stride) = (gint32) ret_v_buffer[2];
    *(destination += envelope_util->destination_stride) = (gint32) ret_v_buffer[3];
    *(destination += envelope_util->destination_stride) = (gint32) ret_v_buffer[4];
    *(destination += envelope_util->destination_stride) = (gint32) ret_v_buffer[5];
    *(destination += envelope_util->destination_stride) = (gint32) ret_v_buffer[6];
    *(destination += envelope_util->destination_stride) = (gint32) ret_v_buffer[7];

    destination += envelope_util->destination_stride;
  }
#else
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint32) ((gint32) ((source)[0] * (start_volume + i * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint32) ((gint32) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint32) ((gint32) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint32) ((gint32) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint32) ((gint32) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint32) ((gint32) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint32) ((gint32) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint32) ((gint32) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));

    destination += envelope_util->destination_stride;
    source += envelope_util->source_stride;
    i++;
  }
#endif

  /* loop tail */
  for(; i < envelope_util->buffer_length;){
    destination[0] = (gint32) ((gint32) (source[0] * (start_volume + i * envelope_util->amount)));

    destination += envelope_util->destination_stride;
    source += envelope_util->source_stride;
    i++;
  }

  envelope_util->volume = start_volume + i * envelope_util->amount;
}

/**
 * ags_envelope_util_compute_s32:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Compute envelope of signed 32 bit data.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_compute_s32(AgsEnvelopeUtil *envelope_util)
{
  gint32 *destination;
  gint32 *source;
  gdouble start_volume;
  guint i, i_stop;

  if(envelope_util == NULL ||
     envelope_util->destination == NULL ||
     envelope_util->source == NULL){
    return;
  }

  destination = (gint32 *) envelope_util->destination;
  source = (gint32 *) envelope_util->source;
  
  start_volume = envelope_util->volume;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride)
    };

    source += envelope_util->source_stride;

    v_volume = (ags_v8double) {
      (gdouble) start_volume + i * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount
    };

    i++;
    
    v_buffer *= v_volume;

    *(destination) = (gint32) v_buffer[0];
    *(destination += envelope_util->destination_stride) = (gint32) v_buffer[1];
    *(destination += envelope_util->destination_stride) = (gint32) v_buffer[2];
    *(destination += envelope_util->destination_stride) = (gint32) v_buffer[3];
    *(destination += envelope_util->destination_stride) = (gint32) v_buffer[4];
    *(destination += envelope_util->destination_stride) = (gint32) v_buffer[5];
    *(destination += envelope_util->destination_stride) = (gint32) v_buffer[6];
    *(destination += envelope_util->destination_stride) = (gint32) v_buffer[7];

    destination += envelope_util->destination_stride;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {(double) buffer[0],
      (double) *(source),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride)};

    double v_volume[] = {(double) start_volume + i * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount};

    source += envelope_util->source_stride;
    i++;

    vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);

    *(destination) = (gint32) ret_v_buffer[0];
    *(destination += envelope_util->destination_stride) = (gint32) ret_v_buffer[1];
    *(destination += envelope_util->destination_stride) = (gint32) ret_v_buffer[2];
    *(destination += envelope_util->destination_stride) = (gint32) ret_v_buffer[3];
    *(destination += envelope_util->destination_stride) = (gint32) ret_v_buffer[4];
    *(destination += envelope_util->destination_stride) = (gint32) ret_v_buffer[5];
    *(destination += envelope_util->destination_stride) = (gint32) ret_v_buffer[6];
    *(destination += envelope_util->destination_stride) = (gint32) ret_v_buffer[7];

    destination += envelope_util->destination_stride;
  }
#else
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint32) ((gint64) ((source)[0] * (start_volume + i * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint32) ((gint64) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint32) ((gint64) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint32) ((gint64) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint32) ((gint64) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint32) ((gint64) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint32) ((gint64) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint32) ((gint64) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));

    destination += envelope_util->destination_stride;
    source += envelope_util->source_stride;
    i++;
  }
#endif

  /* loop tail */
  for(; i < envelope_util->buffer_length;){
    destination[0] = (gint32) ((gint64) (source[0] * (start_volume + i * envelope_util->amount)));

    destination += envelope_util->destination_stride;
    source += envelope_util->source_stride;
    i++;
  }

  envelope_util->volume = start_volume + i * envelope_util->amount;
}

/**
 * ags_envelope_util_compute_s64:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Compute envelope of signed 64 bit data.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_compute_s64(AgsEnvelopeUtil *envelope_util)
{
  gint64 *destination;
  gint64 *source;
  gdouble start_volume;
  guint i, i_stop;

  if(envelope_util == NULL ||
     envelope_util->destination == NULL ||
     envelope_util->source == NULL){
    return;
  }

  destination = (gint64 *) envelope_util->destination;
  source = (gint64 *) envelope_util->source;
  
  start_volume = envelope_util->volume;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride)
    };

    source += envelope_util->source_stride;

    v_volume = (ags_v8double) {
      (gdouble) start_volume + i * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount
    };

    i++;
    
    v_buffer *= v_volume;

    *(destination) = (gint64) v_buffer[0];
    *(destination += envelope_util->destination_stride) = (gint64) v_buffer[1];
    *(destination += envelope_util->destination_stride) = (gint64) v_buffer[2];
    *(destination += envelope_util->destination_stride) = (gint64) v_buffer[3];
    *(destination += envelope_util->destination_stride) = (gint64) v_buffer[4];
    *(destination += envelope_util->destination_stride) = (gint64) v_buffer[5];
    *(destination += envelope_util->destination_stride) = (gint64) v_buffer[6];
    *(destination += envelope_util->destination_stride) = (gint64) v_buffer[7];

    destination += envelope_util->destination_stride;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {(double) buffer[0],
      (double) *(source),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride)};

    double v_volume[] = {(double) start_volume + i * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount};

    source += envelope_util->source_stride;
    i++;

    vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);

    *(destination) = (gint64) ret_v_buffer[0];
    *(destination += envelope_util->destination_stride) = (gint64) ret_v_buffer[1];
    *(destination += envelope_util->destination_stride) = (gint64) ret_v_buffer[2];
    *(destination += envelope_util->destination_stride) = (gint64) ret_v_buffer[3];
    *(destination += envelope_util->destination_stride) = (gint64) ret_v_buffer[4];
    *(destination += envelope_util->destination_stride) = (gint64) ret_v_buffer[5];
    *(destination += envelope_util->destination_stride) = (gint64) ret_v_buffer[6];
    *(destination += envelope_util->destination_stride) = (gint64) ret_v_buffer[7];

    destination += envelope_util->destination_stride;
  }
#else
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gint64) ((gint64) ((source)[0] * (start_volume + i * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint64) ((gint64) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint64) ((gint64) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint64) ((gint64) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint64) ((gint64) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint64) ((gint64) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint64) ((gint64) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gint64) ((gint64) ((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));

    destination += envelope_util->destination_stride;
    source += envelope_util->source_stride;
    i++;
  }
#endif

  /* loop tail */
  for(; i < envelope_util->buffer_length;){
    destination[0] = (gint64) ((gint64) (source[0] * (start_volume + i * envelope_util->amount)));

    destination += envelope_util->destination_stride;
    source += envelope_util->source_stride;
    i++;
  }

  envelope_util->volume = start_volume + i * envelope_util->amount;
}

/**
 * ags_envelope_util_compute_float:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Compute envelope of floating point data.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_compute_float(AgsEnvelopeUtil *envelope_util)
{
  gfloat *destination;
  gfloat *source;
  gdouble start_volume;
  guint i, i_stop;

  if(envelope_util == NULL ||
     envelope_util->destination == NULL ||
     envelope_util->source == NULL){
    return;
  }

  destination = (gfloat *) envelope_util->destination;
  source = (gfloat *) envelope_util->source;
  
  start_volume = envelope_util->volume;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride)
    };

    source += envelope_util->source_stride;

    v_volume = (ags_v8double) {
      (gdouble) start_volume + i * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount
    };

    i++;
    
    v_buffer *= v_volume;

    *(destination) = (gfloat) v_buffer[0];
    *(destination += envelope_util->destination_stride) = (gfloat) v_buffer[1];
    *(destination += envelope_util->destination_stride) = (gfloat) v_buffer[2];
    *(destination += envelope_util->destination_stride) = (gfloat) v_buffer[3];
    *(destination += envelope_util->destination_stride) = (gfloat) v_buffer[4];
    *(destination += envelope_util->destination_stride) = (gfloat) v_buffer[5];
    *(destination += envelope_util->destination_stride) = (gfloat) v_buffer[6];
    *(destination += envelope_util->destination_stride) = (gfloat) v_buffer[7];

    destination += envelope_util->destination_stride;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {(double) buffer[0],
      (double) *(source),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride)};

    double v_volume[] = {(double) start_volume + i * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount};

    source += envelope_util->source_stride;
    i++;

    vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);

    *(destination) = (gfloat) ret_v_buffer[0];
    *(destination += envelope_util->destination_stride) = (gfloat) ret_v_buffer[1];
    *(destination += envelope_util->destination_stride) = (gfloat) ret_v_buffer[2];
    *(destination += envelope_util->destination_stride) = (gfloat) ret_v_buffer[3];
    *(destination += envelope_util->destination_stride) = (gfloat) ret_v_buffer[4];
    *(destination += envelope_util->destination_stride) = (gfloat) ret_v_buffer[5];
    *(destination += envelope_util->destination_stride) = (gfloat) ret_v_buffer[6];
    *(destination += envelope_util->destination_stride) = (gfloat) ret_v_buffer[7];

    destination += envelope_util->destination_stride;
  }
#else
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gfloat) (((source)[0] * (start_volume + i * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gfloat) (((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gfloat) (((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gfloat) (((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gfloat) (((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gfloat) (((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gfloat) (((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gfloat) (((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));

    destination += envelope_util->destination_stride;
    source += envelope_util->source_stride;
    i++;
  }
#endif

  /* loop tail */
  for(; i < envelope_util->buffer_length;){
    destination[0] = (gfloat) ((source[0] * (start_volume + i * envelope_util->amount)));

    destination += envelope_util->destination_stride;
    source += envelope_util->source_stride;
    i++;
  }

  envelope_util->volume = start_volume + i * envelope_util->amount;
}

/**
 * ags_envelope_util_compute_double:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Compute envelope of double floating point data.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_compute_double(AgsEnvelopeUtil *envelope_util)
{
  gdouble *destination;
  gdouble *source;
  gdouble start_volume;
  guint i, i_stop;

  if(envelope_util == NULL ||
     envelope_util->destination == NULL ||
     envelope_util->source == NULL){
    return;
  }

  destination = (gdouble *) envelope_util->destination;
  source = (gdouble *) envelope_util->source;
  
  start_volume = envelope_util->volume;

  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_volume;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride),
      (gdouble) *(source += envelope_util->source_stride)
    };

    source += envelope_util->source_stride;

    v_volume = (ags_v8double) {
      (gdouble) start_volume + i * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount,
      (gdouble) start_volume + (i++) * envelope_util->amount
    };

    i++;
    
    v_buffer *= v_volume;

    *(destination) = (gdouble) v_buffer[0];
    *(destination += envelope_util->destination_stride) = (gdouble) v_buffer[1];
    *(destination += envelope_util->destination_stride) = (gdouble) v_buffer[2];
    *(destination += envelope_util->destination_stride) = (gdouble) v_buffer[3];
    *(destination += envelope_util->destination_stride) = (gdouble) v_buffer[4];
    *(destination += envelope_util->destination_stride) = (gdouble) v_buffer[5];
    *(destination += envelope_util->destination_stride) = (gdouble) v_buffer[6];
    *(destination += envelope_util->destination_stride) = (gdouble) v_buffer[7];

    destination += envelope_util->destination_stride;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);
  
  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {(double) buffer[0],
      (double) *(source),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride),
      (double) *(source += envelope_util->source_stride)};

    double v_volume[] = {(double) start_volume + i * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount,
      (double) start_volume + (i++) * envelope_util->amount};

    source += envelope_util->source_stride;
    i++;

    vDSP_vmulD(v_buffer, 1, v_volume, 1, ret_v_buffer, 1, 8);

    *(destination) = (gdouble) ret_v_buffer[0];
    *(destination += envelope_util->destination_stride) = (gdouble) ret_v_buffer[1];
    *(destination += envelope_util->destination_stride) = (gdouble) ret_v_buffer[2];
    *(destination += envelope_util->destination_stride) = (gdouble) ret_v_buffer[3];
    *(destination += envelope_util->destination_stride) = (gdouble) ret_v_buffer[4];
    *(destination += envelope_util->destination_stride) = (gdouble) ret_v_buffer[5];
    *(destination += envelope_util->destination_stride) = (gdouble) ret_v_buffer[6];
    *(destination += envelope_util->destination_stride) = (gdouble) ret_v_buffer[7];

    destination += envelope_util->destination_stride;
  }
#else
  i_stop = envelope_util->buffer_length - (envelope_util->buffer_length % 8);

  for(; i < i_stop;){
    *(destination) = (gdouble) (((source)[0] * (start_volume + i * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gdouble) (((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gdouble) (((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gdouble) (((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gdouble) (((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gdouble) (((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gdouble) (((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));
    *(destination += envelope_util->destination_stride) = (gdouble) (((source += envelope_util->source_stride)[0] * (start_volume + (i++) * envelope_util->amount)));

    destination += envelope_util->destination_stride;
    source += envelope_util->source_stride;
    i++;
  }
#endif

  /* loop tail */
  for(; i < envelope_util->buffer_length;){
    destination[0] = (gdouble) ((source[0] * (start_volume + i * envelope_util->amount)));

    destination += envelope_util->destination_stride;
    source += envelope_util->source_stride;
    i++;
  }

  envelope_util->volume = start_volume + i * envelope_util->amount;
}

/**
 * ags_envelope_util_compute_complex:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Compute envelope of complex floating point data.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_compute_complex(AgsEnvelopeUtil *envelope_util)
{
  AgsComplex *destination;
  AgsComplex *source;
  gdouble start_volume;
  guint i;

  if(envelope_util == NULL ||
     envelope_util->destination == NULL ||
     envelope_util->source == NULL){
    return;
  }
  
  destination = (AgsComplex *) envelope_util->destination;
  source = (AgsComplex *) envelope_util->source;
  
  start_volume = envelope_util->volume;

  i = 0;
  
  for(; i < envelope_util->buffer_length;){
    double _Complex z;

    z = ags_complex_get(source);

    ags_complex_set(destination,
		    z * (start_volume + i * envelope_util->amount));

    destination += envelope_util->destination_stride;
    source += envelope_util->source_stride;
    i++;
  }

  envelope_util->volume = start_volume + i * envelope_util->amount;
}

/**
 * ags_envelope_util_compute:
 * @envelope_util: the #AgsEnvelopeUtil-struct
 * 
 * Compute envelope.
 *
 * Since: 3.9.2
 */
void
ags_envelope_util_compute(AgsEnvelopeUtil *envelope_util)
{
  if(envelope_util == NULL ||
     envelope_util->destination == NULL ||
     envelope_util->source == NULL){
    return;
  }

  switch(envelope_util->audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_envelope_util_compute_s8(envelope_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_envelope_util_compute_s16(envelope_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_envelope_util_compute_s24(envelope_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_envelope_util_compute_s32(envelope_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_envelope_util_compute_s64(envelope_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_envelope_util_compute_float(envelope_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_envelope_util_compute_double(envelope_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_envelope_util_compute_complex(envelope_util);
  }
  break;
  }
}
