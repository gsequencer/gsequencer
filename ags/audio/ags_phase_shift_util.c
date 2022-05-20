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

GType
ags_phase_shift_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_phase_shift_util = 0;

    ags_type_phase_shift_util =
      g_boxed_type_register_static("AgsPhaseShiftUtil",
				   (GBoxedCopyFunc) ags_phase_shift_util_copy,
				   (GBoxedFreeFunc) ags_phase_shift_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_phase_shift_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_phase_shift_util_alloc:
 *
 * Allocate #AgsPhaseShiftUtil-struct
 *
 * Returns: a new #AgsPhaseShiftUtil-struct
 *
 * Since: 4.0.0
 */
AgsPhaseShiftUtil*
ags_phase_shift_util_alloc()
{
  AgsPhaseShiftUtil *ptr;

  ptr = (AgsPhaseShiftUtil *) g_new(AgsPhaseShiftUtil,
				     1);

  ptr->destination = NULL;
  ptr->destination_stride = 1;

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->buffer_length = 0;
  ptr->format = AGS_PHASE_SHIFT_UTIL_DEFAULT_FORMAT;

  return(ptr);
}

/**
 * ags_phase_shift_util_copy:
 * @ptr: the original #AgsPhaseShiftUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsPhaseShiftUtil-struct
 *
 * Since: 4.0.0
 */
gpointer
ags_phase_shift_util_copy(AgsPhaseShiftUtil *ptr)
{
  AgsPhaseShiftUtil *new_ptr;
  
  new_ptr = (AgsPhaseShiftUtil *) g_new(AgsPhaseShiftUtil,
					 1);
  
  new_ptr->destination = ptr->destination;
  new_ptr->destination_stride = ptr->destination_stride;

  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;

  return(new_ptr);
}

/**
 * ags_phase_shift_util_free:
 * @ptr: the #AgsPhaseShiftUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 4.0.0
 */
void
ags_phase_shift_util_free(AgsPhaseShiftUtil *ptr)
{
  g_free(ptr->destination);

  if(ptr->destination != ptr->source){
    g_free(ptr->source);
  }
  
  g_free(ptr);
}

/**
 * ags_phase_shift_util_get_destination:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * 
 * Get destination buffer of @phase_shift_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 4.0.0
 */
gpointer
ags_phase_shift_util_get_destination(AgsPhaseShiftUtil *phase_shift_util)
{
  if(phase_shift_util == NULL){
    return(NULL);
  }

  return(phase_shift_util->destination);
}

/**
 * ags_phase_shift_util_set_destination:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * @destination: the destination buffer
 *
 * Set @destination buffer of @phase_shift_util.
 *
 * Since: 4.0.0
 */
void
ags_phase_shift_util_set_destination(AgsPhaseShiftUtil *phase_shift_util,
				     gpointer destination)
{
  if(phase_shift_util == NULL){
    return;
  }

  phase_shift_util->destination = destination;
}

/**
 * ags_phase_shift_util_get_destination_stride:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * 
 * Get destination stride of @phase_shift_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 4.0.0
 */
guint
ags_phase_shift_util_get_destination_stride(AgsPhaseShiftUtil *phase_shift_util)
{
  if(phase_shift_util == NULL){
    return(0);
  }

  return(phase_shift_util->destination_stride);
}

/**
 * ags_phase_shift_util_set_destination_stride:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @phase_shift_util.
 *
 * Since: 4.0.0
 */
void
ags_phase_shift_util_set_destination_stride(AgsPhaseShiftUtil *phase_shift_util,
					    guint destination_stride)
{
  if(phase_shift_util == NULL){
    return;
  }

  phase_shift_util->destination_stride = destination_stride;
}

/**
 * ags_phase_shift_util_get_source:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * 
 * Get source buffer of @phase_shift_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 4.0.0
 */
gpointer
ags_phase_shift_util_get_source(AgsPhaseShiftUtil *phase_shift_util)
{
  if(phase_shift_util == NULL){
    return(NULL);
  }

  return(phase_shift_util->source);
}

/**
 * ags_phase_shift_util_set_source:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @phase_shift_util.
 *
 * Since: 4.0.0
 */
void
ags_phase_shift_util_set_source(AgsPhaseShiftUtil *phase_shift_util,
				gpointer source)
{
  if(phase_shift_util == NULL){
    return;
  }

  phase_shift_util->source = source;
}

/**
 * ags_phase_shift_util_get_source_stride:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * 
 * Get source stride of @phase_shift_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 4.0.0
 */
guint
ags_phase_shift_util_get_source_stride(AgsPhaseShiftUtil *phase_shift_util)
{
  if(phase_shift_util == NULL){
    return(0);
  }

  return(phase_shift_util->source_stride);
}

/**
 * ags_phase_shift_util_set_source_stride:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @phase_shift_util.
 *
 * Since: 4.0.0
 */
void
ags_phase_shift_util_set_source_stride(AgsPhaseShiftUtil *phase_shift_util,
				       guint source_stride)
{
  if(phase_shift_util == NULL){
    return;
  }

  phase_shift_util->source_stride = source_stride;
}

/**
 * ags_phase_shift_util_get_buffer_length:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * 
 * Get buffer length of @phase_shift_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 4.0.0
 */
guint
ags_phase_shift_util_get_buffer_length(AgsPhaseShiftUtil *phase_shift_util)
{
  if(phase_shift_util == NULL){
    return(0);
  }

  return(phase_shift_util->buffer_length);
}

/**
 * ags_phase_shift_util_set_buffer_length:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @phase_shift_util.
 *
 * Since: 4.0.0
 */
void
ags_phase_shift_util_set_buffer_length(AgsPhaseShiftUtil *phase_shift_util,
				       guint buffer_length)
{
  if(phase_shift_util == NULL){
    return;
  }

  phase_shift_util->buffer_length = buffer_length;
}

/**
 * ags_phase_shift_util_get_format:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * 
 * Get format of @phase_shift_util.
 * 
 * Returns: the format
 * 
 * Since: 4.0.0
 */
guint
ags_phase_shift_util_get_format(AgsPhaseShiftUtil *phase_shift_util)
{
  if(phase_shift_util == NULL){
    return(0);
  }

  return(phase_shift_util->format);
}

/**
 * ags_phase_shift_util_set_format:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * @format: the format
 *
 * Set @format of @phase_shift_util.
 *
 * Since: 4.0.0
 */
void
ags_phase_shift_util_set_format(AgsPhaseShiftUtil *phase_shift_util,
				guint format)
{
  if(phase_shift_util == NULL){
    return;
  }

  phase_shift_util->format = format;
}

/**
 * ags_phase_shift_util_get_frequency:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * 
 * Get frequency of @phase_shift_util.
 * 
 * Returns: the frequency
 * 
 * Since: 4.0.0
 */
gdouble
ags_phase_shift_util_get_frequency(AgsPhaseShiftUtil *phase_shift_util)
{
  if(phase_shift_util == NULL){
    return(1.0);
  }

  return(phase_shift_util->frequency);
}

/**
 * ags_phase_shift_util_set_frequency:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * @frequency: the frequency
 *
 * Set @frequency of @phase_shift_util.
 *
 * Since: 4.0.0
 */
void
ags_phase_shift_util_set_frequency(AgsPhaseShiftUtil *phase_shift_util,
				   gdouble frequency)
{
  if(phase_shift_util == NULL){
    return;
  }

  phase_shift_util->frequency = frequency;
}

/**
 * ags_phase_shift_util_get_amount:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * 
 * Get amount of @phase_shift_util.
 * 
 * Returns: the amount
 * 
 * Since: 4.0.0
 */
gdouble
ags_phase_shift_util_get_amount(AgsPhaseShiftUtil *phase_shift_util)
{
  if(phase_shift_util == NULL){
    return(1.0);
  }

  return(phase_shift_util->amount);
}

/**
 * ags_phase_shift_util_set_amount:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * @amount: the amount
 *
 * Set @amount of @phase_shift_util.
 *
 * Since: 4.0.0
 */
void
ags_phase_shift_util_set_amount(AgsPhaseShiftUtil *phase_shift_util,
				gdouble amount)
{
  if(phase_shift_util == NULL){
    return;
  }

  phase_shift_util->amount = amount;
}


/**
 * ags_phase_shift_util_get_phase:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * 
 * Get phase of @phase_shift_util.
 * 
 * Returns: the phase
 * 
 * Since: 4.0.0
 */
gdouble
ags_phase_shift_util_get_phase(AgsPhaseShiftUtil *phase_shift_util)
{
  if(phase_shift_util == NULL){
    return(1.0);
  }

  return(phase_shift_util->phase);
}

/**
 * ags_phase_shift_util_set_phase:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * @phase: the phase
 *
 * Set @phase of @phase_shift_util.
 *
 * Since: 4.0.0
 */
void
ags_phase_shift_util_set_phase(AgsPhaseShiftUtil *phase_shift_util,
			       gdouble phase)
{
  if(phase_shift_util == NULL){
    return;
  }

  phase_shift_util->phase = phase;
}

/**
 * ags_phase_shift_util_process_s8:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * 
 * Process phase shift of @phase_shift_util of signed 8 bit data.
 * 
 * Since: 4.0.0
 */
void
ags_phase_shift_util_process_s8(AgsPhaseShiftUtil *phase_shift_util)
{
  gint8 *destination;
  gint8 *source;

  guint buffer_length;
  guint destination_stride;
  guint source_stride;
  guint samplerate;
  gdouble frequency;
  gdouble amount;
  gdouble phase;
  gdouble freq_period, amount_period;
  gdouble phase_period;
  gdouble phase_shift;
  guint i;

  if(phase_shift_util == NULL ||
     phase_shift_util->destination == NULL ||
     phase_shift_util->source == NULL){
    return;
  }

  destination = phase_shift_util->destination;
  destination_stride = phase_shift_util->destination_stride;

  source = phase_shift_util->source;
  source_stride = phase_shift_util->source_stride;

  buffer_length = phase_shift_util->buffer_length;
  samplerate = phase_shift_util->samplerate;

  frequency = phase_shift_util->frequency;
  amount = phase_shift_util->amount;
  phase = phase_shift_util->phase;
  
  freq_period = (gdouble) samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;
  
  phase_period = (phase / (2.0 * M_PI)) * freq_period;

  for(i = 0; i < buffer_length; i++){
    phase_shift = ((amount / (2.0 * M_PI)) * ((double) G_MAXINT8 / 2.0)) * ((((int) ceil(i + phase_period) % (int) ceil(freq_period)) * 2.0 * frequency / (double) samplerate) - 1.0);
    
    destination[i * destination_stride] = 0.5 * (phase_shift + source[i * source_stride]);
  }
}

/**
 * ags_phase_shift_util_process_s16:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * 
 * Process phase shift of @phase_shift_util of signed 16 bit data.
 * 
 * Since: 4.0.0
 */
void
ags_phase_shift_util_process_s16(AgsPhaseShiftUtil *phase_shift_util)
{
  gint16 *destination;
  gint16 *source;

  guint buffer_length;
  guint destination_stride;
  guint source_stride;
  guint samplerate;
  gdouble frequency;
  gdouble amount;
  gdouble phase;
  gdouble freq_period, amount_period;
  gdouble phase_period;
  gdouble phase_shift;
  guint i;

  if(phase_shift_util == NULL ||
     phase_shift_util->destination == NULL ||
     phase_shift_util->source == NULL){
    return;
  }

  destination = phase_shift_util->destination;
  destination_stride = phase_shift_util->destination_stride;

  source = phase_shift_util->source;
  source_stride = phase_shift_util->source_stride;

  buffer_length = phase_shift_util->buffer_length;
  samplerate = phase_shift_util->samplerate;

  frequency = phase_shift_util->frequency;
  amount = phase_shift_util->amount;
  phase = phase_shift_util->phase;
  
  freq_period = (gdouble) samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;
  
  phase_period = (phase / (2.0 * M_PI)) * freq_period;

  for(i = 0; i < buffer_length; i++){
    phase_shift = ((amount / (2.0 * M_PI)) * ((double) G_MAXINT16 / 2.0)) * ((((int) ceil(i + phase_period) % (int) ceil(freq_period)) * 2.0 * frequency / (double) samplerate) - 1.0);
    
    destination[i * destination_stride] = 0.5 * (phase_shift + source[i * source_stride]);
  }
}

/**
 * ags_phase_shift_util_process_s24:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * 
 * Process phase shift of @phase_shift_util of signed 24 bit data.
 * 
 * Since: 4.0.0
 */
void
ags_phase_shift_util_process_s24(AgsPhaseShiftUtil *phase_shift_util)
{
  gint32 *destination;
  gint32 *source;

  guint buffer_length;
  guint destination_stride;
  guint source_stride;
  guint samplerate;
  gdouble frequency;
  gdouble amount;
  gdouble phase;
  gdouble freq_period, amount_period;
  gdouble phase_period;
  gdouble phase_shift;
  guint i;

  if(phase_shift_util == NULL ||
     phase_shift_util->destination == NULL ||
     phase_shift_util->source == NULL){
    return;
  }

  destination = phase_shift_util->destination;
  destination_stride = phase_shift_util->destination_stride;

  source = phase_shift_util->source;
  source_stride = phase_shift_util->source_stride;

  buffer_length = phase_shift_util->buffer_length;
  samplerate = phase_shift_util->samplerate;

  frequency = phase_shift_util->frequency;
  amount = phase_shift_util->amount;
  phase = phase_shift_util->phase;
  
  freq_period = (gdouble) samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;
  
  phase_period = (phase / (2.0 * M_PI)) * freq_period;

  for(i = 0; i < buffer_length; i++){
    phase_shift = ((amount / (2.0 * M_PI)) * ((double) (0xffffff - 1) / 2.0)) * ((((int) ceil(i + phase_period) % (int) ceil(freq_period)) * 2.0 * frequency / (double) samplerate) - 1.0);
    
    destination[i * destination_stride] = 0.5 * (phase_shift + source[i * source_stride]);
  }
}

/**
 * ags_phase_shift_util_process_s32:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * 
 * Process phase shift of @phase_shift_util of signed 32 bit data.
 * 
 * Since: 4.0.0
 */
void
ags_phase_shift_util_process_s32(AgsPhaseShiftUtil *phase_shift_util)
{
  gint32 *destination;
  gint32 *source;

  guint buffer_length;
  guint destination_stride;
  guint source_stride;
  guint samplerate;
  gdouble frequency;
  gdouble amount;
  gdouble phase;
  gdouble freq_period, amount_period;
  gdouble phase_period;
  gdouble phase_shift;
  guint i;

  if(phase_shift_util == NULL ||
     phase_shift_util->destination == NULL ||
     phase_shift_util->source == NULL){
    return;
  }

  destination = phase_shift_util->destination;
  destination_stride = phase_shift_util->destination_stride;

  source = phase_shift_util->source;
  source_stride = phase_shift_util->source_stride;

  buffer_length = phase_shift_util->buffer_length;
  samplerate = phase_shift_util->samplerate;

  frequency = phase_shift_util->frequency;
  amount = phase_shift_util->amount;
  phase = phase_shift_util->phase;
  
  freq_period = (gdouble) samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;
  
  phase_period = (phase / (2.0 * M_PI)) * freq_period;

  for(i = 0; i < buffer_length; i++){
    phase_shift = ((amount / (2.0 * M_PI)) * ((double) G_MAXINT32 / 2.0)) * ((((int) ceil(i + phase_period) % (int) ceil(freq_period)) * 2.0 * frequency / (double) samplerate) - 1.0);
    
    destination[i * destination_stride] = 0.5 * (phase_shift + source[i * source_stride]);
  }
}

/**
 * ags_phase_shift_util_process_s64:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * 
 * Process phase shift of @phase_shift_util of signed 64 bit data.
 * 
 * Since: 4.0.0
 */
void
ags_phase_shift_util_process_s64(AgsPhaseShiftUtil *phase_shift_util)
{
  gint64 *destination;
  gint64 *source;

  guint buffer_length;
  guint destination_stride;
  guint source_stride;
  guint samplerate;
  gdouble frequency;
  gdouble amount;
  gdouble phase;
  gdouble freq_period, amount_period;
  gdouble phase_period;
  gdouble phase_shift;
  guint i;

  if(phase_shift_util == NULL ||
     phase_shift_util->destination == NULL ||
     phase_shift_util->source == NULL){
    return;
  }

  destination = phase_shift_util->destination;
  destination_stride = phase_shift_util->destination_stride;

  source = phase_shift_util->source;
  source_stride = phase_shift_util->source_stride;

  buffer_length = phase_shift_util->buffer_length;
  samplerate = phase_shift_util->samplerate;

  frequency = phase_shift_util->frequency;
  amount = phase_shift_util->amount;
  phase = phase_shift_util->phase;
  
  freq_period = (gdouble) samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;
  
  phase_period = (phase / (2.0 * M_PI)) * freq_period;

  for(i = 0; i < buffer_length; i++){
    phase_shift = ((amount / (2.0 * M_PI)) * ((double) G_MAXINT64 / 2.0)) * ((((int) ceil(i + phase_period) % (int) ceil(freq_period)) * 2.0 * frequency / (double) samplerate) - 1.0);
    
    destination[i * destination_stride] = 0.5 * (phase_shift + source[i * source_stride]);
  }
}

/**
 * ags_phase_shift_util_process_float:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * 
 * Process phase shift of @phase_shift_util of single precision floating point data.
 * 
 * Since: 4.0.0
 */
void
ags_phase_shift_util_process_float(AgsPhaseShiftUtil *phase_shift_util)
{
  gfloat *destination;
  gfloat *source;

  guint buffer_length;
  guint destination_stride;
  guint source_stride;
  guint samplerate;
  gdouble frequency;
  gdouble amount;
  gdouble phase;
  gdouble freq_period, amount_period;
  gdouble phase_period;
  gdouble phase_shift;
  guint i;

  if(phase_shift_util == NULL ||
     phase_shift_util->destination == NULL ||
     phase_shift_util->source == NULL){
    return;
  }

  destination = phase_shift_util->destination;
  destination_stride = phase_shift_util->destination_stride;

  source = phase_shift_util->source;
  source_stride = phase_shift_util->source_stride;

  buffer_length = phase_shift_util->buffer_length;
  samplerate = phase_shift_util->samplerate;

  frequency = phase_shift_util->frequency;
  amount = phase_shift_util->amount;
  phase = phase_shift_util->phase;
  
  freq_period = (gdouble) samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;
  
  phase_period = (phase / (2.0 * M_PI)) * freq_period;

  for(i = 0; i < buffer_length; i++){
    phase_shift = (amount / (2.0 * M_PI)) * ((((int) ceil(i + phase_period) % (int) ceil(freq_period)) * 2.0 * frequency / (double) samplerate) - 1.0);
    
    destination[i * destination_stride] = 0.5 * (phase_shift + source[i * source_stride]);
  }
}

/**
 * ags_phase_shift_util_process_double:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * 
 * Process phase shift of @phase_shift_util of double precision floating point data.
 * 
 * Since: 4.0.0
 */
void
ags_phase_shift_util_process_double(AgsPhaseShiftUtil *phase_shift_util)
{
  gdouble *destination;
  gdouble *source;

  guint buffer_length;
  guint destination_stride;
  guint source_stride;
  guint samplerate;
  gdouble frequency;
  gdouble amount;
  gdouble phase;
  gdouble freq_period, amount_period;
  gdouble phase_period;
  gdouble phase_shift;
  guint i;

  if(phase_shift_util == NULL ||
     phase_shift_util->destination == NULL ||
     phase_shift_util->source == NULL){
    return;
  }

  destination = phase_shift_util->destination;
  destination_stride = phase_shift_util->destination_stride;

  source = phase_shift_util->source;
  source_stride = phase_shift_util->source_stride;

  buffer_length = phase_shift_util->buffer_length;
  samplerate = phase_shift_util->samplerate;

  frequency = phase_shift_util->frequency;
  amount = phase_shift_util->amount;
  phase = phase_shift_util->phase;
  
  freq_period = (gdouble) samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;
  
  phase_period = (phase / (2.0 * M_PI)) * freq_period;

  for(i = 0; i < buffer_length; i++){
    phase_shift = (amount / (2.0 * M_PI)) * ((((int) ceil(i + phase_period) % (int) ceil(freq_period)) * 2.0 * frequency / (double) samplerate) - 1.0);
    
    destination[i * destination_stride] = 0.5 * (phase_shift + source[i * source_stride]);
  }
}

/**
 * ags_phase_shift_util_process_complex:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * 
 * Process phase shift of @phase_shift_util of complex floating point data.
 * 
 * Since: 4.0.0
 */
void
ags_phase_shift_util_process_complex(AgsPhaseShiftUtil *phase_shift_util)
{
  AgsComplex *destination;
  AgsComplex *source;

  guint buffer_length;
  guint destination_stride;
  guint source_stride;
  guint samplerate;
  gdouble frequency;
  gdouble amount;
  gdouble phase;
  gdouble freq_period, amount_period;
  gdouble phase_period;
  gdouble phase_shift;
  guint i;

  if(phase_shift_util == NULL ||
     phase_shift_util->destination == NULL ||
     phase_shift_util->source == NULL){
    return;
  }

  destination = phase_shift_util->destination;
  destination_stride = phase_shift_util->destination_stride;

  source = phase_shift_util->source;
  source_stride = phase_shift_util->source_stride;

  buffer_length = phase_shift_util->buffer_length;
  samplerate = phase_shift_util->samplerate;

  frequency = phase_shift_util->frequency;
  amount = phase_shift_util->amount;
  phase = phase_shift_util->phase;
  
  freq_period = (gdouble) samplerate / frequency;

  amount_period = (amount / (2.0 * M_PI)) * freq_period;
  
  phase_period = (phase / (2.0 * M_PI)) * freq_period;

  for(i = 0; i < buffer_length; i++){
    phase_shift = (amount / (2.0 * M_PI)) * ((((int) ceil(i + phase_period) % (int) ceil(freq_period)) * 2.0 * frequency / (double) samplerate) - 1.0);
    
    ags_complex_set(destination + (i * destination_stride),
		    0.5 * (phase_shift + ags_complex_get(source + (i * source_stride))));
  }
}

/**
 * ags_phase_shift_util_process_complex:
 * @phase_shift_util: the #AgsPhaseShiftUtil-struct
 * 
 * Process phase shift of @phase_shift_util.
 * 
 * Since: 4.0.0
 */
void
ags_phase_shift_util_process(AgsPhaseShiftUtil *phase_shift_util)
{
  if(phase_shift_util == NULL ||
     phase_shift_util->destination == NULL ||
     phase_shift_util->source == NULL){
    return;
  }

  switch(phase_shift_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_phase_shift_util_process_s8(phase_shift_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_phase_shift_util_process_s16(phase_shift_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_phase_shift_util_process_s24(phase_shift_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_phase_shift_util_process_s32(phase_shift_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_phase_shift_util_process_s64(phase_shift_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_phase_shift_util_process_float(phase_shift_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_phase_shift_util_process_double(phase_shift_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_phase_shift_util_process_complex(phase_shift_util);
  }
  break;
  }
}
