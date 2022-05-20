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
 * ags_frequency_aliase_util_compute_s8:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @phase_shifted_source: the phase shifted source audio buffer
 * @buffer_length: the audio buffer's length
 * 
 * Compute aliased audio buffer.
 * 
 * Since: 3.8.0
 */
void
ags_frequency_aliase_util_compute_s8(gint8 *destination,
				     gint8 *source,
				     gint8 *phase_shifted_source,
				     guint buffer_length)
{
  guint i;
  gboolean is_signed;
  
  if(destination == NULL ||
     source == NULL ||
     phase_shifted_source == NULL ||
     buffer_length == 0){
    return;
  }

  for(i = 0; i < buffer_length; i++){
    is_signed = FALSE;

    if((source[i] < 0 &&
	phase_shifted_source[i] < 0) ||
       (source[i] < 0 &&
	-1 * source[i] > phase_shifted_source[i]) ||
       (phase_shifted_source[i] < 0 &&
	-1 * phase_shifted_source[i] > source[i])){
      is_signed = TRUE;
    }
    
    destination[i] = (is_signed ? -1: 1) * (gint8) sqrt(pow((double) source[i], 2.0) + pow((double) phase_shifted_source[i], 2.0));
  }
}

/**
 * ags_frequency_aliase_util_compute_s16:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @phase_shifted_source: the phase shifted source audio buffer
 * @buffer_length: the audio buffer's length
 * 
 * Compute aliased audio buffer.
 * 
 * Since: 3.8.0
 */
void
ags_frequency_aliase_util_compute_s16(gint16 *destination,
				      gint16 *source,
				      gint16 *phase_shifted_source,
				      guint buffer_length)
{
  guint i;
  gboolean is_signed;
  
  if(destination == NULL ||
     source == NULL ||
     phase_shifted_source == NULL ||
     buffer_length == 0){
    return;
  }

  for(i = 0; i < buffer_length; i++){
    is_signed = FALSE;

    if((source[i] < 0 &&
	phase_shifted_source[i] < 0) ||
       (source[i] < 0 &&
	-1 * source[i] > phase_shifted_source[i]) ||
       (phase_shifted_source[i] < 0 &&
	-1 * phase_shifted_source[i] > source[i])){
      is_signed = TRUE;
    }
    
    destination[i] = (is_signed ? -1: 1) * (gint16) sqrt(pow((double) source[i], 2.0) + pow((double) phase_shifted_source[i], 2.0));
  }
}

/**
 * ags_frequency_aliase_util_compute_s24:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @phase_shifted_source: the phase shifted source audio buffer
 * @buffer_length: the audio buffer's length
 * 
 * Compute aliased audio buffer.
 * 
 * Since: 3.8.0
 */
void
ags_frequency_aliase_util_compute_s24(gint32 *destination,
				      gint32 *source,
				      gint32 *phase_shifted_source,
				      guint buffer_length)
{
  guint i;
  gboolean is_signed;
  
  if(destination == NULL ||
     source == NULL ||
     phase_shifted_source == NULL ||
     buffer_length == 0){
    return;
  }

  for(i = 0; i < buffer_length; i++){
    is_signed = FALSE;

    if((source[i] < 0 &&
	phase_shifted_source[i] < 0) ||
       (source[i] < 0 &&
	-1 * source[i] > phase_shifted_source[i]) ||
       (phase_shifted_source[i] < 0 &&
	-1 * phase_shifted_source[i] > source[i])){
      is_signed = TRUE;
    }
    
    destination[i] = (is_signed ? -1: 1) * (gint32) sqrt(pow((double) source[i], 2.0) + pow((double) phase_shifted_source[i], 2.0));
  }
}

/**
 * ags_frequency_aliase_util_compute_s32:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @phase_shifted_source: the phase shifted source audio buffer
 * @buffer_length: the audio buffer's length
 * 
 * Compute aliased audio buffer.
 * 
 * Since: 3.8.0
 */
void
ags_frequency_aliase_util_compute_s32(gint32 *destination,
				      gint32 *source,
				      gint32 *phase_shifted_source,
				      guint buffer_length)
{
  guint i;
  gboolean is_signed;
  
  if(destination == NULL ||
     source == NULL ||
     phase_shifted_source == NULL ||
     buffer_length == 0){
    return;
  }

  for(i = 0; i < buffer_length; i++){
    is_signed = FALSE;

    if((source[i] < 0 &&
	phase_shifted_source[i] < 0) ||
       (source[i] < 0 &&
	-1 * source[i] > phase_shifted_source[i]) ||
       (phase_shifted_source[i] < 0 &&
	-1 * phase_shifted_source[i] > source[i])){
      is_signed = TRUE;
    }
    
    destination[i] = (is_signed ? -1: 1) * (gint32) sqrt(pow((double) source[i], 2.0) + pow((double) phase_shifted_source[i], 2.0));
  }
}

/**
 * ags_frequency_aliase_util_compute_s64:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @phase_shifted_source: the phase shifted source audio buffer
 * @buffer_length: the audio buffer's length
 * 
 * Compute aliased audio buffer.
 * 
 * Since: 3.8.0
 */
void
ags_frequency_aliase_util_compute_s64(gint64 *destination,
				      gint64 *source,
				      gint64 *phase_shifted_source,
				      guint buffer_length)
{
  guint i;
  gboolean is_signed;
  
  if(destination == NULL ||
     source == NULL ||
     phase_shifted_source == NULL ||
     buffer_length == 0){
    return;
  }

  for(i = 0; i < buffer_length; i++){
    is_signed = FALSE;

    if((source[i] < 0 &&
	phase_shifted_source[i] < 0) ||
       (source[i] < 0 &&
	-1 * source[i] > phase_shifted_source[i]) ||
       (phase_shifted_source[i] < 0 &&
	-1 * phase_shifted_source[i] > source[i])){
      is_signed = TRUE;
    }
    
    destination[i] = (is_signed ? -1: 1) * (gint64) sqrt(pow((double) source[i], 2.0) + pow((double) phase_shifted_source[i], 2.0));
  }
}

/**
 * ags_frequency_aliase_util_compute_float:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @phase_shifted_source: the phase shifted source audio buffer
 * @buffer_length: the audio buffer's length
 * 
 * Compute aliased audio buffer.
 * 
 * Since: 3.8.0
 */
void
ags_frequency_aliase_util_compute_float(gfloat *destination,
					gfloat *source,
					gfloat *phase_shifted_source,
					guint buffer_length)
{
  guint i;
  gboolean is_signed;
  
  if(destination == NULL ||
     source == NULL ||
     phase_shifted_source == NULL ||
     buffer_length == 0){
    return;
  }

  for(i = 0; i < buffer_length; i++){
    is_signed = FALSE;

    if((source[i] < 0.0 &&
	phase_shifted_source[i] < 0.0) ||
       (source[i] < 0.0 &&
	-1.0 * source[i] > phase_shifted_source[i]) ||
       (phase_shifted_source[i] < 0.0 &&
	-1.0 * phase_shifted_source[i] > source[i])){
      is_signed = TRUE;
    }
    
    destination[i] = (is_signed ? -1.0: 1.0) * (gfloat) sqrt(pow((double) source[i], 2.0) + pow((double) phase_shifted_source[i], 2.0));
  }
}

/**
 * ags_frequency_aliase_util_compute_double:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @phase_shifted_source: the phase shifted source audio buffer
 * @buffer_length: the audio buffer's length
 * 
 * Compute aliased audio buffer.
 * 
 * Since: 3.8.0
 */
void
ags_frequency_aliase_util_compute_double(gdouble *destination,
					 gdouble *source,
					 gdouble *phase_shifted_source,
					 guint buffer_length)
{
  guint i;
  gboolean is_signed;
  
  if(destination == NULL ||
     source == NULL ||
     phase_shifted_source == NULL ||
     buffer_length == 0){
    return;
  }

  for(i = 0; i < buffer_length; i++){
    is_signed = FALSE;

    if((source[i] < 0.0 &&
	phase_shifted_source[i] < 0.0) ||
       (source[i] < 0.0 &&
	-1.0 * source[i] > phase_shifted_source[i]) ||
       (phase_shifted_source[i] < 0.0 &&
	-1.0 * phase_shifted_source[i] > source[i])){
      is_signed = TRUE;
    }
    
    destination[i] = (is_signed ? -1.0: 1.0) * (gdouble) sqrt(pow((double) source[i], 2.0) + pow((double) phase_shifted_source[i], 2.0));
  }
}

/**
 * ags_frequency_aliase_util_compute_complex:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @phase_shifted_source: the phase shifted source audio buffer
 * @buffer_length: the audio buffer's length
 * 
 * Compute aliased audio buffer.
 * 
 * Since: 3.8.0
 */
void
ags_frequency_aliase_util_compute_complex(AgsComplex *destination,
					  AgsComplex *source,
					  AgsComplex *phase_shifted_source,
					  guint buffer_length)
{
  guint i;
  gboolean is_signed;
  
  if(destination == NULL ||
     source == NULL ||
     phase_shifted_source == NULL ||
     buffer_length == 0){
    return;
  }

  for(i = 0; i < buffer_length; i++){
    is_signed = FALSE;

    //FIXME:JK: improve me
    if(((gdouble) ags_complex_get(source + i) < 0.0 &&
	(gdouble) ags_complex_get(phase_shifted_source + i) < 0.0) ||
       ((gdouble) ags_complex_get(source + i) < 0.0 &&
	-1.0 * (gdouble) ags_complex_get(source + i) > (gdouble) ags_complex_get(phase_shifted_source + i)) ||
       ((gdouble) ags_complex_get(phase_shifted_source + i) < 0.0 &&
	-1.0 * (gdouble) ags_complex_get(phase_shifted_source + i) > (gdouble) ags_complex_get(source + i))){
      is_signed = TRUE;
    }
    
    ags_complex_set(destination + i,
		    (is_signed ? -1.0: 1.0) * (double _Complex) sqrt(pow((double) ags_complex_get(source + i), 2.0) + pow((double) ags_complex_get(phase_shifted_source + i), 2.0)));
  }
}
