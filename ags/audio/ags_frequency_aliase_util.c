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

#include <ags/audio/ags_frequency_aliase_util.h>

#include <math.h>

/**
 * SECTION:ags_frequency_aliase_util
 * @short_description: frequency aliase util
 * @title: AgsFrequencyAliaseUtil
 * @section_id:
 * @include: ags/audio/ags_frequency_aliase_util.h
 *
 * Utility functions to compute frequency aliase.
 */

GType
ags_frequency_aliase_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_frequency_aliase_util = 0;

    ags_type_frequency_aliase_util =
      g_boxed_type_register_static("AgsFrequencyAliaseUtil",
				   (GBoxedCopyFunc) ags_frequency_aliase_util_copy,
				   (GBoxedFreeFunc) ags_frequency_aliase_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_frequency_aliase_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_frequency_aliase_util_alloc:
 *
 * Allocate #AgsFrequencyAliaseUtil-struct
 *
 * Returns: a new #AgsFrequencyAliaseUtil-struct
 *
 * Since: 4.0.0
 */
AgsFrequencyAliaseUtil*
ags_frequency_aliase_util_alloc()
{
  AgsFrequencyAliaseUtil *ptr;

  ptr = (AgsFrequencyAliaseUtil *) g_new(AgsFrequencyAliaseUtil,
					 1);

  ptr->destination = NULL;
  ptr->destination_stride = 1;

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->phase_shifted_source = NULL;
  ptr->phase_shifted_source_stride = 1;

  ptr->buffer_length = 0;
  ptr->format = AGS_FREQUENCY_ALIASE_UTIL_DEFAULT_FORMAT;

  return(ptr);
}

/**
 * ags_frequency_aliase_util_copy:
 * @ptr: the original #AgsFrequencyAliaseUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsFrequencyAliaseUtil-struct
 *
 * Since: 4.0.0
 */
gpointer
ags_frequency_aliase_util_copy(AgsFrequencyAliaseUtil *ptr)
{
  AgsFrequencyAliaseUtil *new_ptr;
  
  new_ptr = (AgsFrequencyAliaseUtil *) g_new(AgsFrequencyAliaseUtil,
					     1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->phase_shifted_source = ptr->phase_shifted_source;
  new_ptr->phase_shifted_source_stride = ptr->phase_shifted_source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;

  return(new_ptr);
}

/**
 * ags_frequency_aliase_util_free:
 * @ptr: the #AgsFrequencyAliaseUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 4.0.0
 */
void
ags_frequency_aliase_util_free(AgsFrequencyAliaseUtil *ptr)
{
  g_free(ptr->destination);

  if(ptr->destination != ptr->source){
    g_free(ptr->source);
  }
  
  g_free(ptr);
}

/**
 * ags_frequency_aliase_util_get_destination:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * 
 * Get destination buffer of @frequency_aliase_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 4.0.0
 */
gpointer
ags_frequency_aliase_util_get_destination(AgsFrequencyAliaseUtil *frequency_aliase_util)
{
  if(frequency_aliase_util == NULL){
    return(NULL);
  }

  return(frequency_aliase_util->destination);
}

/**
 * ags_frequency_aliase_util_set_destination:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * @destination: the destination buffer
 *
 * Set @destination buffer of @frequency_aliase_util.
 *
 * Since: 4.0.0
 */
void
ags_frequency_aliase_util_set_destination(AgsFrequencyAliaseUtil *frequency_aliase_util,
					  gpointer destination)
{
  if(frequency_aliase_util == NULL){
    return;
  }

  frequency_aliase_util->destination = destination;
}

/**
 * ags_frequency_aliase_util_get_destination_stride:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * 
 * Get destination stride of @frequency_aliase_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 4.0.0
 */
guint
ags_frequency_aliase_util_get_destination_stride(AgsFrequencyAliaseUtil *frequency_aliase_util)
{
  if(frequency_aliase_util == NULL){
    return(0);
  }

  return(frequency_aliase_util->destination_stride);
}

/**
 * ags_frequency_aliase_util_set_destination_stride:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @frequency_aliase_util.
 *
 * Since: 4.0.0
 */
void
ags_frequency_aliase_util_set_destination_stride(AgsFrequencyAliaseUtil *frequency_aliase_util,
						 guint destination_stride)
{
  if(frequency_aliase_util == NULL){
    return;
  }

  frequency_aliase_util->destination_stride = destination_stride;
}

/**
 * ags_frequency_aliase_util_get_source:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * 
 * Get source buffer of @frequency_aliase_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 4.0.0
 */
gpointer
ags_frequency_aliase_util_get_source(AgsFrequencyAliaseUtil *frequency_aliase_util)
{
  if(frequency_aliase_util == NULL){
    return(NULL);
  }

  return(frequency_aliase_util->source);
}

/**
 * ags_frequency_aliase_util_set_source:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @frequency_aliase_util.
 *
 * Since: 4.0.0
 */
void
ags_frequency_aliase_util_set_source(AgsFrequencyAliaseUtil *frequency_aliase_util,
				     gpointer source)
{
  if(frequency_aliase_util == NULL){
    return;
  }

  frequency_aliase_util->source = source;
}

/**
 * ags_frequency_aliase_util_get_phase_shifted_source:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * 
 * Get phase shifted source buffer of @frequency_aliase_util.
 * 
 * Returns: the phase shifted source buffer
 * 
 * Since: 4.0.0
 */
gpointer
ags_frequency_aliase_util_get_phase_shifted_source(AgsFrequencyAliaseUtil *frequency_aliase_util)
{
  if(frequency_aliase_util == NULL){
    return(NULL);
  }

  return(frequency_aliase_util->phase_shifted_source);
}

/**
 * ags_frequency_aliase_util_set_phase_shifted_source:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * @phase_shifted_source: the phase shifted source buffer
 *
 * Set @phase_shifted_source buffer of @frequency_aliase_util.
 *
 * Since: 4.0.0
 */
void
ags_frequency_aliase_util_set_phase_shifted_source(AgsFrequencyAliaseUtil *frequency_aliase_util,
						   gpointer phase_shifted_source)
{
  if(frequency_aliase_util == NULL){
    return;
  }

  frequency_aliase_util->phase_shifted_source = phase_shifted_source;
}

/**
 * ags_frequency_aliase_util_get_source_stride:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * 
 * Get source stride of @frequency_aliase_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 4.0.0
 */
guint
ags_frequency_aliase_util_get_source_stride(AgsFrequencyAliaseUtil *frequency_aliase_util)
{
  if(frequency_aliase_util == NULL){
    return(0);
  }

  return(frequency_aliase_util->source_stride);
}

/**
 * ags_frequency_aliase_util_set_source_stride:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @frequency_aliase_util.
 *
 * Since: 4.0.0
 */
void
ags_frequency_aliase_util_set_source_stride(AgsFrequencyAliaseUtil *frequency_aliase_util,
					    guint source_stride)
{
  if(frequency_aliase_util == NULL){
    return;
  }

  frequency_aliase_util->source_stride = source_stride;
}

/**
 * ags_frequency_aliase_util_get_buffer_length:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * 
 * Get buffer length of @frequency_aliase_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 4.0.0
 */
guint
ags_frequency_aliase_util_get_buffer_length(AgsFrequencyAliaseUtil *frequency_aliase_util)
{
  if(frequency_aliase_util == NULL){
    return(0);
  }

  return(frequency_aliase_util->buffer_length);
}

/**
 * ags_frequency_aliase_util_set_buffer_length:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @frequency_aliase_util.
 *
 * Since: 4.0.0
 */
void
ags_frequency_aliase_util_set_buffer_length(AgsFrequencyAliaseUtil *frequency_aliase_util,
					    guint buffer_length)
{
  if(frequency_aliase_util == NULL){
    return;
  }

  frequency_aliase_util->buffer_length = buffer_length;
}

/**
 * ags_frequency_aliase_util_get_format:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * 
 * Get format of @frequency_aliase_util.
 * 
 * Returns: the format
 * 
 * Since: 4.0.0
 */
guint
ags_frequency_aliase_util_get_format(AgsFrequencyAliaseUtil *frequency_aliase_util)
{
  if(frequency_aliase_util == NULL){
    return(0);
  }

  return(frequency_aliase_util->format);
}

/**
 * ags_frequency_aliase_util_set_format:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * @format: the format
 *
 * Set @format of @frequency_aliase_util.
 *
 * Since: 4.0.0
 */
void
ags_frequency_aliase_util_set_format(AgsFrequencyAliaseUtil *frequency_aliase_util,
				     guint format)
{
  if(frequency_aliase_util == NULL){
    return;
  }

  frequency_aliase_util->format = format;
}

/**
 * ags_frequency_aliase_util_process_s8:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * 
 * Process phase shift of @frequency_aliase_util of signed 8 bit data.
 * 
 * Since: 4.0.0
 */
void
ags_frequency_aliase_util_process_s8(AgsFrequencyAliaseUtil *frequency_aliase_util)
{
  gint8 *destination;
  gint8 *source;
  gint8 *phase_shifted_source;

  guint buffer_length;
  guint destination_stride;
  guint source_stride;
  guint phase_shifted_source_stride;
  guint i;
  gboolean is_signed;

  if(frequency_aliase_util == NULL ||
     frequency_aliase_util->destination == NULL ||
     frequency_aliase_util->source == NULL){
    return;
  }

  destination = frequency_aliase_util->destination;
  destination_stride = frequency_aliase_util->destination_stride;

  source = frequency_aliase_util->source;
  source_stride = frequency_aliase_util->source_stride;

  phase_shifted_source = frequency_aliase_util->phase_shifted_source;
  phase_shifted_source_stride = frequency_aliase_util->phase_shifted_source_stride;

  buffer_length = frequency_aliase_util->buffer_length;
  
  for(i = 0; i < buffer_length; i++){
    is_signed = FALSE;

    if((source[i * source_stride] < 0 &&
	phase_shifted_source[i * phase_shifted_source_stride] < 0) ||
       (source[i * source_stride] < 0 &&
	-1 * source[i * source_stride] > phase_shifted_source[i * phase_shifted_source_stride]) ||
       (phase_shifted_source[i] < 0 &&
	-1 * phase_shifted_source[i * phase_shifted_source_stride] > source[i * source_stride])){
      is_signed = TRUE;
    }
    
    destination[i * destination_stride] = (is_signed ? -1: 1) * (gint8) sqrt(pow((double) source[i * source_stride], 2.0) + pow((double) phase_shifted_source[i * phase_shifted_source_stride], 2.0));
  }
}

/**
 * ags_frequency_aliase_util_process_s16:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * 
 * Process phase shift of @frequency_aliase_util of signed 16 bit data.
 * 
 * Since: 4.0.0
 */
void
ags_frequency_aliase_util_process_s16(AgsFrequencyAliaseUtil *frequency_aliase_util)
{
  gint16 *destination;
  gint16 *source;
  gint16 *phase_shifted_source;

  guint buffer_length;
  guint destination_stride;
  guint source_stride;
  guint phase_shifted_source_stride;
  guint i;
  gboolean is_signed;

  if(frequency_aliase_util == NULL ||
     frequency_aliase_util->destination == NULL ||
     frequency_aliase_util->source == NULL){
    return;
  }

  destination = frequency_aliase_util->destination;
  destination_stride = frequency_aliase_util->destination_stride;

  source = frequency_aliase_util->source;
  source_stride = frequency_aliase_util->source_stride;

  phase_shifted_source = frequency_aliase_util->phase_shifted_source;
  phase_shifted_source_stride = frequency_aliase_util->phase_shifted_source_stride;

  buffer_length = frequency_aliase_util->buffer_length;

  for(i = 0; i < buffer_length; i++){
    is_signed = FALSE;

    if((source[i * source_stride] < 0 &&
	phase_shifted_source[i * phase_shifted_source_stride] < 0) ||
       (source[i * source_stride] < 0 &&
	-1 * source[i * source_stride] > phase_shifted_source[i * phase_shifted_source_stride]) ||
       (phase_shifted_source[i] < 0 &&
	-1 * phase_shifted_source[i * phase_shifted_source_stride] > source[i * source_stride])){
      is_signed = TRUE;
    }
    
    destination[i * destination_stride] = (is_signed ? -1: 1) * (gint16) sqrt(pow((double) source[i * source_stride], 2.0) + pow((double) phase_shifted_source[i * phase_shifted_source_stride], 2.0));
  }
}

/**
 * ags_frequency_aliase_util_process_s24:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * 
 * Process phase shift of @frequency_aliase_util of signed 24 bit data.
 * 
 * Since: 4.0.0
 */
void
ags_frequency_aliase_util_process_s24(AgsFrequencyAliaseUtil *frequency_aliase_util)
{
  gint32 *destination;
  gint32 *source;
  gint32 *phase_shifted_source;

  guint buffer_length;
  guint destination_stride;
  guint source_stride;
  guint phase_shifted_source_stride;
  guint i;
  gboolean is_signed;

  if(frequency_aliase_util == NULL ||
     frequency_aliase_util->destination == NULL ||
     frequency_aliase_util->source == NULL){
    return;
  }

  destination = frequency_aliase_util->destination;
  destination_stride = frequency_aliase_util->destination_stride;

  source = frequency_aliase_util->source;
  source_stride = frequency_aliase_util->source_stride;

  phase_shifted_source = frequency_aliase_util->phase_shifted_source;
  phase_shifted_source_stride = frequency_aliase_util->phase_shifted_source_stride;

  buffer_length = frequency_aliase_util->buffer_length;

  for(i = 0; i < buffer_length; i++){
    is_signed = FALSE;

    if((source[i * source_stride] < 0 &&
	phase_shifted_source[i * phase_shifted_source_stride] < 0) ||
       (source[i * source_stride] < 0 &&
	-1 * source[i * source_stride] > phase_shifted_source[i * phase_shifted_source_stride]) ||
       (phase_shifted_source[i] < 0 &&
	-1 * phase_shifted_source[i * phase_shifted_source_stride] > source[i * source_stride])){
      is_signed = TRUE;
    }
    
    destination[i * destination_stride] = (is_signed ? -1: 1) * (gint32) sqrt(pow((double) source[i * source_stride], 2.0) + pow((double) phase_shifted_source[i * phase_shifted_source_stride], 2.0));
  }
}

/**
 * ags_frequency_aliase_util_process_s32:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * 
 * Process phase shift of @frequency_aliase_util of signed 32 bit data.
 * 
 * Since: 4.0.0
 */
void
ags_frequency_aliase_util_process_s32(AgsFrequencyAliaseUtil *frequency_aliase_util)
{
  gint32 *destination;
  gint32 *source;
  gint32 *phase_shifted_source;

  guint buffer_length;
  guint destination_stride;
  guint source_stride;
  guint phase_shifted_source_stride;
  guint i;
  gboolean is_signed;

  if(frequency_aliase_util == NULL ||
     frequency_aliase_util->destination == NULL ||
     frequency_aliase_util->source == NULL){
    return;
  }

  destination = frequency_aliase_util->destination;
  destination_stride = frequency_aliase_util->destination_stride;

  source = frequency_aliase_util->source;
  source_stride = frequency_aliase_util->source_stride;

  phase_shifted_source = frequency_aliase_util->phase_shifted_source;
  phase_shifted_source_stride = frequency_aliase_util->phase_shifted_source_stride;

  buffer_length = frequency_aliase_util->buffer_length;

  for(i = 0; i < buffer_length; i++){
    is_signed = FALSE;

    if((source[i * source_stride] < 0 &&
	phase_shifted_source[i * phase_shifted_source_stride] < 0) ||
       (source[i * source_stride] < 0 &&
	-1 * source[i * source_stride] > phase_shifted_source[i * phase_shifted_source_stride]) ||
       (phase_shifted_source[i] < 0 &&
	-1 * phase_shifted_source[i * phase_shifted_source_stride] > source[i * source_stride])){
      is_signed = TRUE;
    }
    
    destination[i * destination_stride] = (is_signed ? -1: 1) * (gint32) sqrt(pow((double) source[i * source_stride], 2.0) + pow((double) phase_shifted_source[i * phase_shifted_source_stride], 2.0));
  }
}

/**
 * ags_frequency_aliase_util_process_s64:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * 
 * Process phase shift of @frequency_aliase_util of signed 64 bit data.
 * 
 * Since: 4.0.0
 */
void
ags_frequency_aliase_util_process_s64(AgsFrequencyAliaseUtil *frequency_aliase_util)
{
  gint64 *destination;
  gint64 *source;
  gint64 *phase_shifted_source;

  guint buffer_length;
  guint destination_stride;
  guint source_stride;
  guint phase_shifted_source_stride;
  guint i;
  gboolean is_signed;

  if(frequency_aliase_util == NULL ||
     frequency_aliase_util->destination == NULL ||
     frequency_aliase_util->source == NULL){
    return;
  }

  destination = frequency_aliase_util->destination;
  destination_stride = frequency_aliase_util->destination_stride;

  source = frequency_aliase_util->source;
  source_stride = frequency_aliase_util->source_stride;

  phase_shifted_source = frequency_aliase_util->phase_shifted_source;
  phase_shifted_source_stride = frequency_aliase_util->phase_shifted_source_stride;

  buffer_length = frequency_aliase_util->buffer_length;

  for(i = 0; i < buffer_length; i++){
    is_signed = FALSE;

    if((source[i * source_stride] < 0 &&
	phase_shifted_source[i * phase_shifted_source_stride] < 0) ||
       (source[i * source_stride] < 0 &&
	-1 * source[i * source_stride] > phase_shifted_source[i * phase_shifted_source_stride]) ||
       (phase_shifted_source[i] < 0 &&
	-1 * phase_shifted_source[i * phase_shifted_source_stride] > source[i * source_stride])){
      is_signed = TRUE;
    }
    
    destination[i * destination_stride] = (is_signed ? -1: 1) * (gint64) sqrt(pow((double) source[i * source_stride], 2.0) + pow((double) phase_shifted_source[i * phase_shifted_source_stride], 2.0));
  }
}

/**
 * ags_frequency_aliase_util_process_float:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * 
 * Process phase shift of @frequency_aliase_util of single precision floating point data.
 * 
 * Since: 4.0.0
 */
void
ags_frequency_aliase_util_process_float(AgsFrequencyAliaseUtil *frequency_aliase_util)
{
  gfloat *destination;
  gfloat *source;
  gfloat *phase_shifted_source;

  guint buffer_length;
  guint destination_stride;
  guint source_stride;
  guint phase_shifted_source_stride;
  guint i;
  gboolean is_signed;

  if(frequency_aliase_util == NULL ||
     frequency_aliase_util->destination == NULL ||
     frequency_aliase_util->source == NULL){
    return;
  }

  destination = frequency_aliase_util->destination;
  destination_stride = frequency_aliase_util->destination_stride;

  source = frequency_aliase_util->source;
  source_stride = frequency_aliase_util->source_stride;

  phase_shifted_source = frequency_aliase_util->phase_shifted_source;
  phase_shifted_source_stride = frequency_aliase_util->phase_shifted_source_stride;

  buffer_length = frequency_aliase_util->buffer_length;

  for(i = 0; i < buffer_length; i++){
    is_signed = FALSE;

    if((source[i * source_stride] < 0.0 &&
	phase_shifted_source[i * phase_shifted_source_stride] < 0.0) ||
       (source[i * source_stride] < 0.0 &&
	-1.0 * source[i * source_stride] > phase_shifted_source[i * phase_shifted_source_stride]) ||
       (phase_shifted_source[i] < 0.0 &&
	-1.0 * phase_shifted_source[i * phase_shifted_source_stride] > source[i * source_stride])){
      is_signed = TRUE;
    }
    
    destination[i * destination_stride] = (is_signed ? -1.0: 1.0) * (gfloat) sqrt(pow((double) source[i * source_stride], 2.0) + pow((double) phase_shifted_source[i * phase_shifted_source_stride], 2.0));
  }
}

/**
 * ags_frequency_aliase_util_process_double:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * 
 * Process phase shift of @frequency_aliase_util of double precision floating point data.
 * 
 * Since: 4.0.0
 */
void
ags_frequency_aliase_util_process_double(AgsFrequencyAliaseUtil *frequency_aliase_util)
{
  gdouble *destination;
  gdouble *source;
  gdouble *phase_shifted_source;

  guint buffer_length;
  guint destination_stride;
  guint source_stride;
  guint phase_shifted_source_stride;
  guint i;
  gboolean is_signed;

  if(frequency_aliase_util == NULL ||
     frequency_aliase_util->destination == NULL ||
     frequency_aliase_util->source == NULL){
    return;
  }

  destination = frequency_aliase_util->destination;
  destination_stride = frequency_aliase_util->destination_stride;

  source = frequency_aliase_util->source;
  source_stride = frequency_aliase_util->source_stride;

  phase_shifted_source = frequency_aliase_util->phase_shifted_source;
  phase_shifted_source_stride = frequency_aliase_util->phase_shifted_source_stride;

  buffer_length = frequency_aliase_util->buffer_length;

  for(i = 0; i < buffer_length; i++){
    is_signed = FALSE;

    if((source[i * source_stride] < 0.0 &&
	phase_shifted_source[i * phase_shifted_source_stride] < 0.0) ||
       (source[i * source_stride] < 0.0 &&
	-1.0 * source[i * source_stride] > phase_shifted_source[i * phase_shifted_source_stride]) ||
       (phase_shifted_source[i] < 0.0 &&
	-1.0 * phase_shifted_source[i * phase_shifted_source_stride] > source[i * source_stride])){
      is_signed = TRUE;
    }
    
    destination[i * destination_stride] = (is_signed ? -1.0: 1.0) * (gdouble) sqrt(pow((double) source[i * source_stride], 2.0) + pow((double) phase_shifted_source[i * phase_shifted_source_stride], 2.0));
  }
}

/**
 * ags_frequency_aliase_util_process_complex:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * 
 * Process phase shift of @frequency_aliase_util of complex floating point data.
 * 
 * Since: 4.0.0
 */
void
ags_frequency_aliase_util_process_complex(AgsFrequencyAliaseUtil *frequency_aliase_util)
{
  AgsComplex *destination;
  AgsComplex *source;
  AgsComplex *phase_shifted_source;

  guint buffer_length;
  guint destination_stride;
  guint source_stride;
  guint phase_shifted_source_stride;
  guint i;
  gboolean is_signed;

  if(frequency_aliase_util == NULL ||
     frequency_aliase_util->destination == NULL ||
     frequency_aliase_util->source == NULL){
    return;
  }

  destination = frequency_aliase_util->destination;
  destination_stride = frequency_aliase_util->destination_stride;

  source = frequency_aliase_util->source;
  source_stride = frequency_aliase_util->source_stride;

  phase_shifted_source = frequency_aliase_util->phase_shifted_source;
  phase_shifted_source_stride = frequency_aliase_util->phase_shifted_source_stride;

  buffer_length = frequency_aliase_util->buffer_length;

  for(i = 0; i < buffer_length; i++){
    is_signed = FALSE;

    if(((double) ags_complex_get(source + (i * source_stride)) < 0.0 &&
	(double) ags_complex_get(phase_shifted_source + (i * phase_shifted_source_stride)) < 0.0) ||
       ((double) ags_complex_get(source + (i * source_stride)) < 0.0 &&
	-1.0 * (double) ags_complex_get(source + (i * source_stride)) > (double) ags_complex_get(phase_shifted_source + (i * phase_shifted_source_stride))) ||
       ((double) ags_complex_get(phase_shifted_source + (i * phase_shifted_source_stride)) < 0.0 &&
	-1.0 * (double) ags_complex_get(phase_shifted_source + (i * phase_shifted_source_stride)) > (double) ags_complex_get(source + (i * source_stride)))){
      is_signed = TRUE;
    }
    
    ags_complex_set(destination + (i * destination_stride),
		    (is_signed ? -1.0: 1.0) * sqrt(pow((double) ags_complex_get(source + (i * source_stride)), 2.0) + pow((double) ags_complex_get(phase_shifted_source + (i * phase_shifted_source_stride)), 2.0)));
  }
}

/**
 * ags_frequency_aliase_util_process:
 * @frequency_aliase_util: the #AgsFrequencyAliaseUtil-struct
 * 
 * Process phase shift of @frequency_aliase_util.
 * 
 * Since: 4.0.0
 */
void
ags_frequency_aliase_util_process(AgsFrequencyAliaseUtil *frequency_aliase_util)
{
  if(frequency_aliase_util == NULL ||
     frequency_aliase_util->destination == NULL ||
     frequency_aliase_util->source == NULL ||
     frequency_aliase_util->phase_shifted_source == NULL){
    return;
  }

  switch(frequency_aliase_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_frequency_aliase_util_process_s8(frequency_aliase_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_frequency_aliase_util_process_s16(frequency_aliase_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_frequency_aliase_util_process_s24(frequency_aliase_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_frequency_aliase_util_process_s32(frequency_aliase_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_frequency_aliase_util_process_s64(frequency_aliase_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_frequency_aliase_util_process_float(frequency_aliase_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_frequency_aliase_util_process_double(frequency_aliase_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_frequency_aliase_util_process_complex(frequency_aliase_util);
  }
  break;
  }
}
