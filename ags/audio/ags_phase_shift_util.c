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

#include <ags/audio/ags_phase_shift_util.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

/**
 * SECTION:ags_phase_shift_util
 * @short_description: phase shift util
 * @title: AgsPhaseShiftUtil
 * @section_id:
 * @include: ags/audio/ags_phase_shift_util.h
 *
 * Utility functions to phase shift.
 */

/**
 * ags_phase_shift_util_compute_s8:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * @output_buffer: (out): output buffer
 * 
 * Compute phase shift of audio buffer by @frequency with @amount with
 * max radian 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_phase_shift_util_compute_s8(gint8 *buffer,
				guint buffer_length,
				guint samplerate,
				gdouble frequency,
				gdouble amount,
				gint8 **output_buffer)
{
  gint8 *out;

  gdouble freq_period, amount_period;
  gdouble phase_shift;
  gdouble phase;
  guint i;
  
  if(buffer == NULL ||
     buffer_length == 0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }
    
    return;
  }

  out = (gint8 *) g_malloc(buffer_length * sizeof(gint8));

  freq_period = samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;
  
  phase = freq_period - amount_period;

  for(i = 0; i < buffer_length; i++){
    phase_shift = ((((int) ceil(i + phase) % (int) ceil(freq_period)) * 2.0 * frequency / samplerate) - 1.0);
    
    out[i] = phase_shift * buffer[i];
  }
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }
}

/**
 * ags_phase_shift_util_compute_s16:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * @output_buffer: (out): output buffer
 * 
 * Compute phase shift of audio buffer by @frequency with @amount with
 * max radian 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_phase_shift_util_compute_s16(gint16 *buffer,
				 guint buffer_length,
				 guint samplerate,
				 gdouble frequency,
				 gdouble amount,
				 gint16 **output_buffer)
{
  gint16 *out;

  gdouble freq_period, amount_period;
  gdouble phase_shift;
  gdouble phase;
  guint i;

  if(buffer == NULL ||
     buffer_length == 0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }
    
    return;
  }

  out = (gint16 *) g_malloc(buffer_length * sizeof(gint16));

  freq_period = samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;
  
  phase = freq_period - amount_period;

  for(i = 0; i < buffer_length; i++){
    phase_shift = ((((int) ceil(i + phase) % (int) ceil(freq_period)) * 2.0 * frequency / samplerate) - 1.0);
    
    out[i] = phase_shift * buffer[i];
  }
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }
}

/**
 * ags_phase_shift_util_compute_s24:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * @output_buffer: (out): output buffer
 * 
 * Compute phase shift of audio buffer by @frequency with @amount with
 * max radian 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_phase_shift_util_compute_s24(gint32 *buffer,
				 guint buffer_length,
				 guint samplerate,
				 gdouble frequency,
				 gdouble amount,
				 gint32 **output_buffer)
{
  gint32 *out;

  gdouble freq_period, amount_period;
  gdouble phase_shift;
  gdouble phase;
  guint i;

  if(buffer == NULL ||
     buffer_length == 0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }
    
    return;
  }

  out = (gint32 *) g_malloc(buffer_length * sizeof(gint32));

  freq_period = samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;
  
  phase = freq_period - amount_period;

  for(i = 0; i < buffer_length; i++){
    phase_shift = ((((int) ceil(i + phase) % (int) ceil(freq_period)) * 2.0 * frequency / samplerate) - 1.0);
    
    out[i] = phase_shift * buffer[i];
  }
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }
}

/**
 * ags_phase_shift_util_compute_s32:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * @output_buffer: (out): output buffer
 * 
 * Compute phase shift of audio buffer by @frequency with @amount with
 * max radian 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_phase_shift_util_compute_s32(gint32 *buffer,
				 guint buffer_length,
				 guint samplerate,
				 gdouble frequency,
				 gdouble amount,
				 gint32 **output_buffer)
{
  gint32 *out;

  gdouble freq_period, amount_period;
  gdouble phase_shift;
  gdouble phase;
  guint i;

  if(buffer == NULL ||
     buffer_length == 0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }
    
    return;
  }

  out = (gint32 *) g_malloc(buffer_length * sizeof(gint32));

  freq_period = samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;

  phase = freq_period - amount_period;
  
  for(i = 0; i < buffer_length; i++){
    phase_shift = ((((int) ceil(i + phase) % (int) ceil(freq_period)) * 2.0 * frequency / samplerate) - 1.0);
    
    out[i] = phase_shift * buffer[i];
  }
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }
}

/**
 * ags_phase_shift_util_compute_s64:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * @output_buffer: (out): output buffer
 * 
 * Compute phase shift of audio buffer by @frequency with @amount with
 * max radian 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_phase_shift_util_compute_s64(gint64 *buffer,
				 guint buffer_length,
				 guint samplerate,
				 gdouble frequency,
				 gdouble amount,
				 gint64 **output_buffer)
{
  gint64 *out;

  gdouble freq_period, amount_period;
  gdouble phase_shift;
  gdouble phase;
  guint i;

  if(buffer == NULL ||
     buffer_length == 0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }
    
    return;
  }

  out = (gint64 *) g_malloc(buffer_length * sizeof(gint64));

  freq_period = samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;

  phase = freq_period - amount_period;
  
  for(i = 0; i < buffer_length; i++){
    phase_shift = ((((int) ceil(i + phase) % (int) ceil(freq_period)) * 2.0 * frequency / samplerate) - 1.0);
    
    out[i] = phase_shift * buffer[i];
  }
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }
}

/**
 * ags_phase_shift_util_compute_float:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * @output_buffer: (out): output buffer
 * 
 * Compute phase shift of audio buffer by @frequency with @amount with
 * max radian 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_phase_shift_util_compute_float(gfloat *buffer,
				   guint buffer_length,
				   guint samplerate,
				   gdouble frequency,
				   gdouble amount,
				   gfloat **output_buffer)
{
  gfloat *out;

  gdouble freq_period, amount_period;
  gdouble phase_shift;
  gdouble phase;
  guint i;

  if(buffer == NULL ||
     buffer_length == 0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }
    
    return;
  }

  out = (gfloat *) g_malloc(buffer_length * sizeof(gfloat));

  freq_period = samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;
  
  phase = freq_period - amount_period;

  for(i = 0; i < buffer_length; i++){
    phase_shift = ((((int) ceil(i + phase) % (int) ceil(freq_period)) * 2.0 * frequency / samplerate) - 1.0);
    
    out[i] = phase_shift * buffer[i];
  }
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }
}

/**
 * ags_phase_shift_util_compute_double:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * @output_buffer: (out): output buffer
 * 
 * Compute phase shift of audio buffer by @frequency with @amount with
 * max radian 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_phase_shift_util_compute_double(gdouble *buffer,
				    guint buffer_length,
				    guint samplerate,
				    gdouble frequency,
				    gdouble amount,
				    gdouble **output_buffer)
{
  gdouble *out;

  gdouble freq_period, amount_period;
  gdouble phase_shift;
  gdouble phase;
  guint i;

  if(buffer == NULL ||
     buffer_length == 0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }
    
    return;
  }

  out = (gdouble *) g_malloc(buffer_length * sizeof(gdouble));

  freq_period = samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;

  phase = freq_period - amount_period;
  
  for(i = 0; i < buffer_length; i++){
    phase_shift = ((((int) ceil(i + phase) % (int) ceil(freq_period)) * 2.0 * frequency / samplerate) - 1.0);
    
    out[i] = phase_shift * buffer[i];
  }
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }
}

/**
 * ags_phase_shift_util_compute_complex:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * @output_buffer: (out): output buffer
 * 
 * Compute phase shift of audio buffer by @frequency with @amount with
 * max radian 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_phase_shift_util_compute_complex(AgsComplex *buffer,
				     guint buffer_length,
				     guint samplerate,
				     gdouble frequency,
				     gdouble amount,
				     AgsComplex **output_buffer)
{
  AgsComplex *out;

  gdouble freq_period, amount_period;
  gdouble phase_shift;
  gdouble phase;
  guint i;

  if(buffer == NULL ||
     buffer_length == 0){
    if(output_buffer != NULL){
      output_buffer[0] = NULL;
    }
    
    return;
  }

  out = (AgsComplex *) g_malloc(buffer_length * sizeof(AgsComplex));

  freq_period = samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;
  
  phase = freq_period - amount_period;

  for(i = 0; i < buffer_length; i++){
    phase_shift = ((((int) ceil(i + phase) % (int) ceil(freq_period)) * 2.0 * frequency / samplerate) - 1.0);
    
    ags_complex_set(out + i,
		    phase_shift * ags_complex_get(buffer + i));
  }
  
  if(output_buffer != NULL){
    output_buffer[0] = out;
  }
}
