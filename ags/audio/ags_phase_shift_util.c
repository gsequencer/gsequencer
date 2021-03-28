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
 * Utility functions to compute phase shift.
 */

/**
 * ags_phase_shift_util_compute_s8:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * 
 * Compute phase shift of audio buffer at @frequency with @amount with
 * max radian 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_phase_shift_util_compute_s8(gint8 *destination,
				gint8 *source,
				guint buffer_length,
				guint samplerate,
				gdouble frequency,
				gdouble amount)
{
  gdouble freq_period, amount_period;
  gdouble phase_shift;
  gdouble phase;
  guint i;
  
  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }

  freq_period = samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;
  
  phase = freq_period - amount_period;

  for(i = 0; i < buffer_length; i++){
    phase_shift = (G_MAXINT8 / 2.0) * ((((int) ceil(i + phase) % (int) ceil(freq_period)) * 2.0 * frequency / samplerate) - 1.0);
    
    destination[i] = 0.5 * (phase_shift + source[i]);
  }
}

/**
 * ags_phase_shift_util_compute_s16:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * 
 * Compute phase shift of audio buffer at @frequency with @amount with
 * max radian 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_phase_shift_util_compute_s16(gint16 *destination,
				 gint16 *source,
				 guint buffer_length,
				 guint samplerate,
				 gdouble frequency,
				 gdouble amount)
{
  gdouble freq_period, amount_period;
  gdouble phase_shift;
  gdouble phase;
  guint i;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }

  freq_period = samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;
  
  phase = freq_period - amount_period;

  for(i = 0; i < buffer_length; i++){
    phase_shift = (G_MAXINT16 / 2.0) * ((((int) ceil(i + phase) % (int) ceil(freq_period)) * 2.0 * frequency / samplerate) - 1.0);
    
    destination[i] = 0.5 * (phase_shift + source[i]);
  }
}

/**
 * ags_phase_shift_util_compute_s24:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * 
 * Compute phase shift of audio buffer at @frequency with @amount with
 * max radian 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_phase_shift_util_compute_s24(gint32 *destination,
				 gint32 *source,
				 guint buffer_length,
				 guint samplerate,
				 gdouble frequency,
				 gdouble amount)
{
  gdouble freq_period, amount_period;
  gdouble phase_shift;
  gdouble phase;
  guint i;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }

  freq_period = samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;
  
  phase = freq_period - amount_period;

  for(i = 0; i < buffer_length; i++){
    phase_shift = (0x7fffff / 2.0) * ((((int) ceil(i + phase) % (int) ceil(freq_period)) * 2.0 * frequency / samplerate) - 1.0);
    
    destination[i] = 0.5 * (phase_shift + source[i]);
  }
}

/**
 * ags_phase_shift_util_compute_s32:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * 
 * Compute phase shift of audio buffer at @frequency with @amount with
 * max radian 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_phase_shift_util_compute_s32(gint32 *destination,
				 gint32 *source,
				 guint buffer_length,
				 guint samplerate,
				 gdouble frequency,
				 gdouble amount)
{
  gdouble freq_period, amount_period;
  gdouble phase_shift;
  gdouble phase;
  guint i;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }

  freq_period = samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;

  phase = freq_period - amount_period;
  
  for(i = 0; i < buffer_length; i++){
    phase_shift = (G_MAXINT32 / 2.0) * ((((int) ceil(i + phase) % (int) ceil(freq_period)) * 2.0 * frequency / samplerate) - 1.0);
    
    destination[i] = 0.5 * (phase_shift + source[i]);
  }
}

/**
 * ags_phase_shift_util_compute_s64:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * 
 * Compute phase shift of audio buffer at @frequency with @amount with
 * max radian 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_phase_shift_util_compute_s64(gint64 *destination,
				 gint64 *source,
				 guint buffer_length,
				 guint samplerate,
				 gdouble frequency,
				 gdouble amount)
{
  gdouble freq_period, amount_period;
  gdouble phase_shift;
  gdouble phase;
  guint i;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }

  freq_period = samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;

  phase = freq_period - amount_period;
  
  for(i = 0; i < buffer_length; i++){
    phase_shift = (G_MAXINT64 / 2.0) * ((((int) ceil(i + phase) % (int) ceil(freq_period)) * 2.0 * frequency / samplerate) - 1.0);
    
    destination[i] = 0.5 * (phase_shift + source[i]);
  }
}

/**
 * ags_phase_shift_util_compute_float:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * 
 * Compute phase shift of audio buffer at @frequency with @amount with
 * max radian 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_phase_shift_util_compute_float(gfloat *destination,
				   gfloat *source,
				   guint buffer_length,
				   guint samplerate,
				   gdouble frequency,
				   gdouble amount)
{
  gdouble freq_period, amount_period;
  gdouble phase_shift;
  gdouble phase;
  guint i;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }

  freq_period = samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;
  
  phase = freq_period - amount_period;

  for(i = 0; i < buffer_length; i++){
    phase_shift = ((((int) ceil(i + phase) % (int) ceil(freq_period)) * 2.0 * frequency / samplerate) - 1.0);
    
    destination[i] = 0.5 * (phase_shift + source[i]);
  }
}

/**
 * ags_phase_shift_util_compute_double:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * 
 * Compute phase shift of audio buffer at @frequency with @amount with
 * max radian 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_phase_shift_util_compute_double(gdouble *destination,
				    gdouble *source,
				    guint buffer_length,
				    guint samplerate,
				    gdouble frequency,
				    gdouble amount)
{
  gdouble freq_period, amount_period;
  gdouble phase_shift;
  gdouble phase;
  guint i;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }

  freq_period = samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;

  phase = freq_period - amount_period;
  
  for(i = 0; i < buffer_length; i++){
    phase_shift = ((((int) ceil(i + phase) % (int) ceil(freq_period)) * 2.0 * frequency / samplerate) - 1.0);
    
    destination[i] = 0.5 * (phase_shift + source[i]);
  }
}

/**
 * ags_phase_shift_util_compute_complex:
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @frequency: the frequency
 * @amount: the amount
 * 
 * Compute phase shift of audio buffer at @frequency with @amount with
 * max radian 2 * M_PI.
 * 
 * Since: 3.8.0
 */
void
ags_phase_shift_util_compute_complex(AgsComplex *destination,
				     AgsComplex *source,
				     guint buffer_length,
				     guint samplerate,
				     gdouble frequency,
				     gdouble amount)
{
  gdouble freq_period, amount_period;
  gdouble phase_shift;
  gdouble phase;
  guint i;

  if(destination == NULL ||
     source == NULL ||
     buffer_length == 0){
    return;
  }

  freq_period = samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;
  
  phase = freq_period - amount_period;

  for(i = 0; i < buffer_length; i++){
    phase_shift = ((((int) ceil(i + phase) % (int) ceil(freq_period)) * 2.0 * frequency / samplerate) - 1.0);
    
    ags_complex_set(destination + i,
		    0.5 * (phase_shift + ags_complex_get(source + i)));
  }
}
