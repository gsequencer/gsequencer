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
 * Allocate #AgsEnvelopeUtil
 *
 * Returns: a new #AgsEnvelopeUtil
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
  ptr->source = NULL;

  ptr->audio_channels = 0;

  ptr->buffer_length = 0;
  ptr->audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S16;
  
  ptr->volume = 1.0;
  ptr->amount = 0.0;

  return(ptr);
}

/**
 * ags_envelope_util_copy:
 * @ptr: the original #AgsEnvelopeUtil
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsEnvelopeUtil
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
  new_ptr->source = ptr->source;

  new_ptr->audio_channels = ptr->audio_channels;

  new_ptr->buffer_length = ptr->buffer_length;
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
  g_free(ptr);
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
  gint8 *destination, *source;
  gdouble start_volume, current_volume;
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
    guint j;

    j = 0;
    
    v_buffer = (ags_v8double) {
      (gdouble) source[0],
      (gdouble) source[j++],
      (gdouble) source[j++],
      (gdouble) source[j++],
      (gdouble) source[j++],
      (gdouble) source[j++],
      (gdouble) source[j++],
      (gdouble) source[j++]
    };
    
    j = 0;

    v_volume = (ags_v8double) {
      (gdouble) start_volume + i * envelope_util->amount,
      (gdouble) start_volume + (i + (j++)) * envelope_util->amount,
      (gdouble) start_volume + (i + (j++)) * envelope_util->amount,
      (gdouble) start_volume + (i + (j++)) * envelope_util->amount,
      (gdouble) start_volume + (i + (j++)) * envelope_util->amount,
      (gdouble) start_volume + (i + (j++)) * envelope_util->amount,
      (gdouble) start_volume + (i + (j++)) * envelope_util->amount,
      (gdouble) start_volume + (i + (j++)) * envelope_util->amount
    };

    v_buffer *= v_volume;

    j = 0;
    
    destination[0] = (gint8) v_buffer[0];
    destination[j++] = (gint8) v_buffer[1];
    destination[j++] = (gint8) v_buffer[2];
    destination[j++] = (gint8) v_buffer[3];
    destination[j++] = (gint8) v_buffer[4];
    destination[j++] = (gint8) v_buffer[5];
    destination[j++] = (gint8) v_buffer[6];
    destination[j++] = (gint8) v_buffer[7];
    
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  //TODO:JK: implement me
#else
  //TODO:JK: implement me
#endif

  /* loop tail */
  for(; i < envelope_util->buffer_length; i++){
    current_volume = start_volume + i * envelope_util->amount;
    
    destination[0] = (gint8) ((gint16) (source[0] * current_volume));

    destination++;
    source++;
  }
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
  if(envelope_util == NULL ||
     envelope_util->destination == NULL ||
     envelope_util->source == NULL){
    return;
  }
  
  //TODO:JK: implement me
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
  if(envelope_util == NULL ||
     envelope_util->destination == NULL ||
     envelope_util->source == NULL){
    return;
  }
  
  //TODO:JK: implement me
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
  if(envelope_util == NULL ||
     envelope_util->destination == NULL ||
     envelope_util->source == NULL){
    return;
  }
  
  //TODO:JK: implement me
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
  if(envelope_util == NULL ||
     envelope_util->destination == NULL ||
     envelope_util->source == NULL){
    return;
  }
  
  //TODO:JK: implement me
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
  if(envelope_util == NULL ||
     envelope_util->destination == NULL ||
     envelope_util->source == NULL){
    return;
  }
  
  //TODO:JK: implement me
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
  if(envelope_util == NULL ||
     envelope_util->destination == NULL ||
     envelope_util->source == NULL){
    return;
  }
  
  //TODO:JK: implement me
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
  if(envelope_util == NULL ||
     envelope_util->destination == NULL ||
     envelope_util->source == NULL){
    return;
  }
  
  //TODO:JK: implement me
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
