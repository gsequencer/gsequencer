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

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

/**
 * SECTION:ags_frequency_aliase_util
 * @short_description: frequency aliase util
 * @title: AgsFrequencyAliaseUtil
 * @section_id:
 * @include: ags/audio/ags_frequency_aliase_util.h
 *
 * Utility functions to frequency aliase.
 */

/**
 * ags_frequency_aliase_util_compute_s8:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * @output_buffer: (out): output buffer
 * 
 * Compute aliased audio buffer by @frequency with @amount with max radian
 * 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_frequency_aliase_util_compute_s8(gint8 *buffer,
				     guint buffer_length,
				     guint samplerate,
				     gdouble frequency,
				     gdouble amount,
				     gint8 **output_buffer)
{
  gint8 *phase_shifted_buffer;
  gint8 *out;

  guint i;
  
  if(buffer == NULL ||
     buffer_length == 0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }
    
    return;
  }

  phase_shifted_buffer = NULL;

  ags_phase_shift_util_compute_s8(buffer,
				  buffer_length,
				  samplerate,
				  frequency,
				  amount,
				  &phase_shifted_buffer);

  out = (gint8 *) g_malloc(buffer_length * sizeof(gint8));

  for(i = 0; i < buffer_length; i++){
    out[i] = sqrt(pow((double) buffer[i], 2.0) + pow((double) phase_shifted_buffer[i], 2.0));
  }

  g_free(phase_shifted_buffer);
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }
}

/**
 * ags_frequency_aliase_util_compute_s16:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * @output_buffer: (out): output buffer
 * 
 * Compute aliased audio buffer by @frequency with @amount with max radian
 * 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_frequency_aliase_util_compute_s16(gint16 *buffer,
				      guint buffer_length,
				      guint samplerate,
				      gdouble frequency,
				      gdouble amount,
				      gint16 **output_buffer)
{
  gint16 *phase_shifted_buffer;
  gint16 *out;

  guint i;
  
  if(buffer == NULL ||
     buffer_length == 0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }
    
    return;
  }

  phase_shifted_buffer = NULL;

  ags_phase_shift_util_compute_s16(buffer,
				   buffer_length,
				   samplerate,
				   frequency,
				   amount,
				   &phase_shifted_buffer);

  out = (gint16 *) g_malloc(buffer_length * sizeof(gint16));

  for(i = 0; i < buffer_length; i++){
    out[i] = sqrt(pow((double) buffer[i], 2.0) + pow((double) phase_shifted_buffer[i], 2.0));
  }

  g_free(phase_shifted_buffer);
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }
}

/**
 * ags_frequency_aliase_util_compute_s24:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * @output_buffer: (out): output buffer
 * 
 * Compute aliased audio buffer by @frequency with @amount with max radian
 * 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_frequency_aliase_util_compute_s24(gint32 *buffer,
				      guint buffer_length,
				      guint samplerate,
				      gdouble frequency,
				      gdouble amount,
				      gint32 **output_buffer)
{
  gint32 *phase_shifted_buffer;
  gint32 *out;

  guint i;
  
  if(buffer == NULL ||
     buffer_length == 0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }
    
    return;
  }

  phase_shifted_buffer = NULL;

  ags_phase_shift_util_compute_s24(buffer,
				   buffer_length,
				   samplerate,
				   frequency,
				   amount,
				   &phase_shifted_buffer);

  out = (gint32 *) g_malloc(buffer_length * sizeof(gint32));

  for(i = 0; i < buffer_length; i++){
    out[i] = sqrt(pow((double) buffer[i], 2.0) + pow((double) phase_shifted_buffer[i], 2.0));
  }

  g_free(phase_shifted_buffer);
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }
}

/**
 * ags_frequency_aliase_util_compute_s32:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * @output_buffer: (out): output buffer
 * 
 * Compute aliased audio buffer by @frequency with @amount with max radian
 * 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_frequency_aliase_util_compute_s32(gint32 *buffer,
				      guint buffer_length,
				      guint samplerate,
				      gdouble frequency,
				      gdouble amount,
				      gint32 **output_buffer)
{
  gint32 *phase_shifted_buffer;
  gint32 *out;

  guint i;
  
  if(buffer == NULL ||
     buffer_length == 0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }
    
    return;
  }

  phase_shifted_buffer = NULL;

  ags_phase_shift_util_compute_s32(buffer,
				   buffer_length,
				   samplerate,
				   frequency,
				   amount,
				   &phase_shifted_buffer);

  out = (gint32 *) g_malloc(buffer_length * sizeof(gint32));

  for(i = 0; i < buffer_length; i++){
    out[i] = sqrt(pow((double) buffer[i], 2.0) + pow((double) phase_shifted_buffer[i], 2.0));
  }

  g_free(phase_shifted_buffer);
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }
}

/**
 * ags_frequency_aliase_util_compute_s64:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * @output_buffer: (out): output buffer
 * 
 * Compute aliased audio buffer by @frequency with @amount with max radian
 * 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_frequency_aliase_util_compute_s64(gint64 *buffer,
				      guint buffer_length,
				      guint samplerate,
				      gdouble frequency,
				      gdouble amount,
				      gint64 **output_buffer)
{
  gint64 *phase_shifted_buffer;
  gint64 *out;

  guint i;
  
  if(buffer == NULL ||
     buffer_length == 0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }
    
    return;
  }

  phase_shifted_buffer = NULL;

  ags_phase_shift_util_compute_s64(buffer,
				   buffer_length,
				   samplerate,
				   frequency,
				   amount,
				   &phase_shifted_buffer);

  out = (gint64 *) g_malloc(buffer_length * sizeof(gint64));

  for(i = 0; i < buffer_length; i++){
    out[i] = sqrt(pow((double) buffer[i], 2.0) + pow((double) phase_shifted_buffer[i], 2.0));
  }

  g_free(phase_shifted_buffer);
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }
}

/**
 * ags_frequency_aliase_util_compute_float:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * @output_buffer: (out): output buffer
 * 
 * Compute aliased audio buffer by @frequency with @amount with max radian
 * 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_frequency_aliase_util_compute_float(gfloat *buffer,
					guint buffer_length,
					guint samplerate,
					gdouble frequency,
					gdouble amount,
					gfloat **output_buffer)
{
  gfloat *phase_shifted_buffer;
  gfloat *out;

  guint i;
  
  if(buffer == NULL ||
     buffer_length == 0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }
    
    return;
  }

  phase_shifted_buffer = NULL;

  ags_phase_shift_util_compute_float(buffer,
				     buffer_length,
				     samplerate,
				     frequency,
				     amount,
				     &phase_shifted_buffer);

  out = (gfloat *) g_malloc(buffer_length * sizeof(gfloat));

  for(i = 0; i < buffer_length; i++){
    out[i] = sqrt(pow((double) buffer[i], 2.0) + pow((double) phase_shifted_buffer[i], 2.0));
  }

  g_free(phase_shifted_buffer);
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }
}

/**
 * ags_frequency_aliase_util_compute_double:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * @output_buffer: (out): output buffer
 * 
 * Compute aliased audio buffer by @frequency with @amount with max radian
 * 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_frequency_aliase_util_compute_double(gdouble *buffer,
					 guint buffer_length,
					 guint samplerate,
					 gdouble frequency,
					 gdouble amount,
					 gdouble **output_buffer)
{
  gdouble *phase_shifted_buffer;
  gdouble *out;

  guint i;
  
  if(buffer == NULL ||
     buffer_length == 0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }
    
    return;
  }

  phase_shifted_buffer = NULL;

  ags_phase_shift_util_compute_double(buffer,
				      buffer_length,
				      samplerate,
				      frequency,
				      amount,
				      &phase_shifted_buffer);

  out = (gdouble *) g_malloc(buffer_length * sizeof(gdouble));

  for(i = 0; i < buffer_length; i++){
    out[i] = sqrt(pow((double) buffer[i], 2.0) + pow((double) phase_shifted_buffer[i], 2.0));
  }

  g_free(phase_shifted_buffer);
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }
  //TODO:JK: implement me
}

/**
 * ags_frequency_aliase_util_compute_complex:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * @output_buffer: (out): output buffer
 * 
 * Compute aliased audio buffer by @frequency with @amount with max radian
 * 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_frequency_aliase_util_compute_complex(AgsComplex *buffer,
					  guint buffer_length,
					  guint samplerate,
					  gdouble frequency,
					  gdouble amount,
					  AgsComplex **output_buffer)
{
  AgsComplex *phase_shifted_buffer;
  AgsComplex *out;

  guint i;
  
  if(buffer == NULL ||
     buffer_length == 0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }
    
    return;
  }

  phase_shifted_buffer = NULL;

  ags_phase_shift_util_compute_complex(buffer,
				       buffer_length,
				       samplerate,
				       frequency,
				       amount,
				       &phase_shifted_buffer);

  out = (AgsComplex *) g_malloc(buffer_length * sizeof(AgsComplex));

  for(i = 0; i < buffer_length; i++){
    ags_complex_set(out + i,
		    sqrt(pow((double) ags_complex_get(buffer + i), 2.0) + pow((double) ags_complex_get(phase_shifted_buffer + i), 2.0));
  }

  g_free(phase_shifted_buffer);
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }
}
