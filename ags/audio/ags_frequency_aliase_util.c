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
