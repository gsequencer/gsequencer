/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/audio/ags_lfo_synth_util.h>

#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_fourier_transform_util.h>

#if defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#include <Accelerate/Accelerate.h>
#endif

#include <math.h>
#include <complex.h>

/**
 * SECTION:ags_lfo_synth_util
 * @short_description: low frequency oscillator synth util
 * @title: AgsLFOSynthUtil
 * @section_id:
 * @include: ags/audio/ags_lfo_synth_util.h
 *
 * Utility functions to compute LFO synths.
 */

GType
ags_lfo_synth_util_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_lfo_synth_util = 0;

    ags_type_lfo_synth_util =
      g_boxed_type_register_static("AgsLFOSynthUtil",
				   (GBoxedCopyFunc) ags_lfo_synth_util_copy,
				   (GBoxedFreeFunc) ags_lfo_synth_util_free);

    g_once_init_leave(&g_define_type_id__static, ags_type_lfo_synth_util);
  }

  return(g_define_type_id__static);
}

/**
 * ags_lfo_synth_util_alloc:
 *
 * Allocate #AgsLFOSynthUtil-struct
 *
 * Returns: a new #AgsLFOSynthUtil-struct
 *
 * Since: 4.0.0
 */
AgsLFOSynthUtil*
ags_lfo_synth_util_alloc()
{
  AgsLFOSynthUtil *ptr;

  ptr = (AgsLFOSynthUtil *) g_new(AgsLFOSynthUtil,
				 1);

  ptr[0] = AGS_LFO_SYNTH_UTIL_INITIALIZER;

  return(ptr);
}

/**
 * ags_lfo_synth_util_copy:
 * @ptr: the original #AgsLFOSynthUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsLFOSynthUtil-struct
 *
 * Since: 4.0.0
 */
gpointer
ags_lfo_synth_util_copy(AgsLFOSynthUtil *ptr)
{
  AgsLFOSynthUtil *new_ptr;

  g_return_val_if_fail(ptr != NULL, NULL);
  
  new_ptr = (AgsLFOSynthUtil *) g_new(AgsLFOSynthUtil,
				     1);
  
  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->frequency = ptr->frequency;
  new_ptr->phase = ptr->phase;

  new_ptr->lfo_synth_oscillator_mode = ptr->lfo_synth_oscillator_mode;

  new_ptr->lfo_depth = ptr->lfo_depth;
  new_ptr->tuning = ptr->tuning;

  new_ptr->frame_count = ptr->frame_count;
  new_ptr->offset = ptr->offset;
  
  return(new_ptr);
}

/**
 * ags_lfo_synth_util_free:
 * @ptr: the #AgsLFOSynthUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_free(AgsLFOSynthUtil *ptr)
{
  g_return_if_fail(ptr != NULL);
  
  g_free(ptr);
}

/**
 * ags_lfo_synth_util_get_source:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Get source buffer of @lfo_synth_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 4.0.0
 */
gpointer
ags_lfo_synth_util_get_source(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL){
    return(NULL);
  }

  return(lfo_synth_util->source);
}

/**
 * ags_lfo_synth_util_set_source:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * @source: (transfer none): the source buffer
 *
 * Set @source buffer of @lfo_synth_util.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_set_source(AgsLFOSynthUtil *lfo_synth_util,
			      gpointer source)
{
  if(lfo_synth_util == NULL){
    return;
  }

  lfo_synth_util->source = source;
}

/**
 * ags_lfo_synth_util_get_source_stride:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Get source stride of @lfo_synth_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 4.0.0
 */
guint
ags_lfo_synth_util_get_source_stride(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL){
    return(0);
  }

  return(lfo_synth_util->source_stride);
}

/**
 * ags_lfo_synth_util_set_source_stride:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @lfo_synth_util.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_set_source_stride(AgsLFOSynthUtil *lfo_synth_util,
				     guint source_stride)
{
  if(lfo_synth_util == NULL){
    return;
  }

  lfo_synth_util->source_stride = source_stride;
}

/**
 * ags_lfo_synth_util_get_buffer_length:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Get buffer length of @lfo_synth_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 4.0.0
 */
guint
ags_lfo_synth_util_get_buffer_length(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL){
    return(0);
  }

  return(lfo_synth_util->buffer_length);
}

/**
 * ags_lfo_synth_util_set_buffer_length:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @lfo_synth_util.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_set_buffer_length(AgsLFOSynthUtil *lfo_synth_util,
				     guint buffer_length)
{
  if(lfo_synth_util == NULL){
    return;
  }

  lfo_synth_util->buffer_length = buffer_length;
}

/**
 * ags_lfo_synth_util_get_format:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Get audio buffer util format of @lfo_synth_util.
 * 
 * Returns: the audio buffer util format
 * 
 * Since: 3.14.0
 */
guint
ags_lfo_synth_util_get_format(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL){
    return(0);
  }

  return(lfo_synth_util->format);
}

/**
 * ags_lfo_synth_util_set_format:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * @format: the audio buffer util format
 *
 * Set @format of @lfo_synth_util.
 *
 * Since: 3.14.0
 */
void
ags_lfo_synth_util_set_format(AgsLFOSynthUtil *lfo_synth_util,
			      guint format)
{
  if(lfo_synth_util == NULL){
    return;
  }

  lfo_synth_util->format = format;
}

/**
 * ags_lfo_synth_util_get_samplerate:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Get samplerate of @lfo_synth_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 4.0.0
 */
guint
ags_lfo_synth_util_get_samplerate(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL){
    return(0);
  }

  return(lfo_synth_util->samplerate);
}

/**
 * ags_lfo_synth_util_set_samplerate:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @lfo_synth_util.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_set_samplerate(AgsLFOSynthUtil *lfo_synth_util,
				  guint samplerate)
{
  if(lfo_synth_util == NULL){
    return;
  }

  lfo_synth_util->samplerate = samplerate;
}

/**
 * ags_lfo_synth_util_get_lfo_synth_oscillator_mode:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Get LFO synth oscillator mode of @lfo_synth_util.
 * 
 * Returns: the LFO synth oscillator mode
 * 
 * Since: 4.0.0
 */
AgsSynthOscillatorMode
ags_lfo_synth_util_get_lfo_synth_oscillator_mode(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL){
    return(0);
  }

  return(lfo_synth_util->lfo_synth_oscillator_mode);
}

/**
 * ags_lfo_synth_util_set_lfo_synth_oscillator_mode:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * @lfo_synth_oscillator_mode: the LFO synth oscillator mode
 *
 * Set @lfo_synth_oscillator_mode of @lfo_synth_util.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_set_lfo_synth_oscillator_mode(AgsLFOSynthUtil *lfo_synth_util,
						 AgsSynthOscillatorMode lfo_synth_oscillator_mode)
{
  if(lfo_synth_util == NULL){
    return;
  }

  lfo_synth_util->lfo_synth_oscillator_mode = lfo_synth_oscillator_mode;
}


/**
 * ags_lfo_synth_util_get_frequency:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Get frequency of @lfo_synth_util.
 * 
 * Returns: the frequency
 * 
 * Since: 4.0.0
 */
gdouble
ags_lfo_synth_util_get_frequency(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL){
    return(1.0);
  }

  return(lfo_synth_util->frequency);
}

/**
 * ags_lfo_synth_util_set_frequency:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * @frequency: the frequency
 *
 * Set @frequency of @lfo_synth_util.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_set_frequency(AgsLFOSynthUtil *lfo_synth_util,
				 gdouble frequency)
{
  if(lfo_synth_util == NULL){
    return;
  }

  lfo_synth_util->frequency = frequency;
}

/**
 * ags_lfo_synth_util_get_phase:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Get phase of @lfo_synth_util.
 * 
 * Returns: the phase
 * 
 * Since: 4.0.0
 */
gdouble
ags_lfo_synth_util_get_phase(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL){
    return(1.0);
  }

  return(lfo_synth_util->phase);
}

/**
 * ags_lfo_synth_util_set_phase:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * @phase: the phase
 *
 * Set @phase of @lfo_synth_util.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_set_phase(AgsLFOSynthUtil *lfo_synth_util,
			     gdouble phase)
{
  if(lfo_synth_util == NULL){
    return;
  }

  lfo_synth_util->phase = phase;
}

/**
 * ags_lfo_synth_util_get_lfo_depth:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Get LFO depth of @lfo_synth_util.
 * 
 * Returns: the LFO depth
 * 
 * Since: 4.0.0
 */
gdouble
ags_lfo_synth_util_get_lfo_depth(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL){
    return(1.0);
  }

  return(lfo_synth_util->lfo_depth);
}

/**
 * ags_lfo_synth_util_set_lfo_depth:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * @lfo_depth: the LFO depth
 *
 * Set @lfo_depth of @lfo_synth_util.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_set_lfo_depth(AgsLFOSynthUtil *lfo_synth_util,
				 gdouble lfo_depth)
{
  if(lfo_synth_util == NULL){
    return;
  }

  lfo_synth_util->lfo_depth = lfo_depth;
}

/**
 * ags_lfo_synth_util_get_tuning:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Get tuning of @lfo_synth_util.
 * 
 * Returns: the tuning
 * 
 * Since: 4.0.0
 */
gdouble
ags_lfo_synth_util_get_tuning(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL){
    return(1.0);
  }

  return(lfo_synth_util->tuning);
}

/**
 * ags_lfo_synth_util_set_tuning:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * @tuning: the tuning
 *
 * Set @tuning of @lfo_synth_util.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_set_tuning(AgsLFOSynthUtil *lfo_synth_util,
			      gdouble tuning)
{
  if(lfo_synth_util == NULL){
    return;
  }

  lfo_synth_util->tuning = tuning;
}

/**
 * ags_lfo_synth_util_get_offset:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Get offset of @lfo_synth_util.
 * 
 * Returns: the offset
 * 
 * Since: 4.0.0
 */
guint
ags_lfo_synth_util_get_offset(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL){
    return(0);
  }

  return(lfo_synth_util->offset);
}

/**
 * ags_lfo_synth_util_set_offset:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * @offset: the offset
 *
 * Set @offset of @lfo_synth_util.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_set_offset(AgsLFOSynthUtil *lfo_synth_util,
			      guint offset)
{
  if(lfo_synth_util == NULL){
    return;
  }

  lfo_synth_util->offset = offset;
}

/**
 * ags_lfo_synth_util_get_frame_count:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Get frame count of @lfo_synth_util.
 * 
 * Returns: the frame count
 * 
 * Since: 4.0.0
 */
guint
ags_lfo_synth_util_get_frame_count(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL){
    return(0);
  }

  return(lfo_synth_util->frame_count);
}

/**
 * ags_lfo_synth_util_set_frame_count:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * @frame_count: the frame count
 *
 * Set @frame_count of @lfo_synth_util.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_set_frame_count(AgsLFOSynthUtil *lfo_synth_util,
				   guint frame_count)
{
  if(lfo_synth_util == NULL){
    return;
  }

  lfo_synth_util->frame_count = frame_count;
}

/**
 * ags_lfo_synth_util_get_note_256th_mode:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Get note 256th mode of @lfo_synth_util.
 * 
 * Returns: %TRUE if note 256th mode, otherwise %FALSE
 * 
 * Since: 7.0.0
 */
gboolean
ags_lfo_synth_util_get_note_256th_mode(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL){
    return(0);
  }

  return(lfo_synth_util->note_256th_mode);
}

/**
 * ags_lfo_synth_util_set_note_256th_mode:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * @note_256th_mode: the note 256th mode
 *
 * Set @note_256th_mode of @lfo_synth_util.
 *
 * Since: 7.0.0
 */
void
ags_lfo_synth_util_set_note_256th_mode(AgsLFOSynthUtil *lfo_synth_util,
				       gboolean note_256th_mode)
{
  if(lfo_synth_util == NULL){
    return;
  }

  lfo_synth_util->note_256th_mode = note_256th_mode;
}

/**
 * ags_lfo_synth_util_get_offset_256th:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Get offset as note 256th of @lfo_synth_util.
 * 
 * Returns: the offset as note 256th
 * 
 * Since: 7.0.0
 */
guint
ags_lfo_synth_util_get_offset_256th(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL){
    return(0);
  }

  return(lfo_synth_util->offset_256th);
}

/**
 * ags_lfo_synth_util_set_offset_256th:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * @offset_256th: the offset as note 256th
 *
 * Set @offset_256th of @lfo_synth_util.
 *
 * Since: 7.0.0
 */
void
ags_lfo_synth_util_set_offset_256th(AgsLFOSynthUtil *lfo_synth_util,
				    guint offset_256th)
{
  if(lfo_synth_util == NULL){
    return;
  }
  
  lfo_synth_util->offset_256th = offset_256th;
}

/**
 * ags_lfo_synth_util_compute_sin_s8:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO sine synth of signed 8 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_sin_s8(AgsLFOSynthUtil *lfo_synth_util)
{
  gint8 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;
  
  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_sine;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_sine = (ags_v8double) {
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth)
    };

    v_buffer *= v_sine;

    *(source) = (gint8) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_sine[] = {
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth)};

    vDSP_vmulD(v_buffer, 1, v_sine, 1, ret_v_buffer, 1, 8);

    *(source) = (gint8) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint8) ((gint16) source[0] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[1 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[2 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[3 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[4 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[5 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[6 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[7 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint8) ((gint16) source[0] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    
    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_sin_s16:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO sine synth of signed 16 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_sin_s16(AgsLFOSynthUtil *lfo_synth_util)
{
  gint16 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;
  
  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_sine;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_sine = (ags_v8double) {
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth)
    };

    v_buffer *= v_sine;

    *(source) = (gint16) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_sine[] = {
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth)};

    vDSP_vmulD(v_buffer, 1, v_sine, 1, ret_v_buffer, 1, 8);

    *(source) = (gint16) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint16) ((gint32) source[0] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[1 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[2 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[3 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[4 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[5 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[6 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[7 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint16) ((gint32) source[0] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    
    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_sin_s24:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO sine synth of signed 24 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_sin_s24(AgsLFOSynthUtil *lfo_synth_util)
{
  gint32 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;
  
  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_sine;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_sine = (ags_v8double) {
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth)
    };

    v_buffer *= v_sine;

    *(source) = (gint32) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_sine[] = {
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth)};

    vDSP_vmulD(v_buffer, 1, v_sine, 1, ret_v_buffer, 1, 8);

    *(source) = (gint32) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint32) ((gint32) source[0] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[1 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[2 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[3 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[4 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[5 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[6 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[7 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint32) ((gint32) source[0] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    
    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_sin_s32:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO sine synth of signed 32 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_sin_s32(AgsLFOSynthUtil *lfo_synth_util)
{
  gint32 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;
  
  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_sine;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_sine = (ags_v8double) {
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth)
    };

    v_buffer *= v_sine;

    *(source) = (gint32) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_sine[] = {
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth)};

    vDSP_vmulD(v_buffer, 1, v_sine, 1, ret_v_buffer, 1, 8);

    *(source) = (gint32) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint32) ((gint64) source[0] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[1 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[2 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[3 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[4 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[5 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[6 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[7 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint32) ((gint64) source[0] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    
    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_sin_s64:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO sine synth of signed 64 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_sin_s64(AgsLFOSynthUtil *lfo_synth_util)
{
  gint64 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;
  
  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_sine;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_sine = (ags_v8double) {
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth)
    };

    v_buffer *= v_sine;

    *(source) = (gint64) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_sine[] = {
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth)};

    vDSP_vmulD(v_buffer, 1, v_sine, 1, ret_v_buffer, 1, 8);

    *(source) = (gint64) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint64) ((gint64) source[0] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[1 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[2 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[3 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[4 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[5 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[6 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[7 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint64) ((gint64) source[0] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    
    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_sin_float:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO sine synth of single precision floating point data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_sin_float(AgsLFOSynthUtil *lfo_synth_util)
{
  gfloat *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;
  
  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_sine;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_sine = (ags_v8double) {
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth)
    };

    v_buffer *= v_sine;

    *(source) = (gfloat) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_sine[] = {
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth)};

    vDSP_vmulD(v_buffer, 1, v_sine, 1, ret_v_buffer, 1, 8);

    *(source) = (gfloat) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gfloat) ((gdouble) source[0] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[1 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[2 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[3 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[4 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[5 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[6 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[7 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gfloat) ((gdouble) source[0] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    
    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_sin_double:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO sine synth of double precision floating point data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_sin_double(AgsLFOSynthUtil *lfo_synth_util)
{
  gdouble *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;
  
  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_sine;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_sine = (ags_v8double) {
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth)
    };

    v_buffer *= v_sine;

    *(source) = (gdouble) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_sine[] = {
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth),
      sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth)};

    vDSP_vmulD(v_buffer, 1, v_sine, 1, ret_v_buffer, 1, 8);

    *(source) = (gdouble) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gdouble) ((gdouble) source[0] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[1 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[2 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[3 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[4 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[5 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[6 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[7 * lfo_synth_util->source_stride] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gdouble) ((gdouble) source[0] * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    
    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_sin_complex:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO sine synth of complex data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_sin_complex(AgsLFOSynthUtil *lfo_synth_util)
{
  AgsComplex *source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i;
  
  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

  i = 0;
  
  for(; i < lfo_synth_util->buffer_length;){
    ags_complex_set(source,
		    ags_complex_get(source) * sin((gdouble) (offset + i + phase) * 2.0 * M_PI * ((exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) * lfo_depth));
    
    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_sin:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO sine synth.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_sin(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  switch(lfo_synth_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_lfo_synth_util_compute_sin_s8(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_lfo_synth_util_compute_sin_s16(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_lfo_synth_util_compute_sin_s24(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_lfo_synth_util_compute_sin_s32(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_lfo_synth_util_compute_sin_s64(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_lfo_synth_util_compute_sin_float(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_lfo_synth_util_compute_sin_double(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_lfo_synth_util_compute_sin_complex(lfo_synth_util);
  }
  break;
  }
}

/**
 * ags_lfo_synth_util_compute_sawtooth_s8:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO sawtooth synth of signed 8 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_sawtooth_s8(AgsLFOSynthUtil *lfo_synth_util)
{
  gint8 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

  phase = (int) ceil(phase) % (int) ceil(frequency);
  phase = ceil(phase / frequency) * ceil(samplerate / frequency);

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_sawtooth;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_sawtooth = (ags_v8double) {
      ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 3 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 4 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 5 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 6 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 7 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth)
    };

    v_buffer *= v_sawtooth;

    *(source) = (gint8) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)  
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_sawtooth[] = {
      ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 3 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 4 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 5 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 6 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 7 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
    };

    vDSP_vmulD(v_buffer, 1, v_sawtooth, 1, ret_v_buffer, 1, 8);

    *(source) = (gint8) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint8) ((gint16) source[0] * ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[1 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[2 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[3 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[4 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[5 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[6 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[7 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint8) ((gint16) source[0] * ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    
    source += lfo_synth_util->source_stride;
    i++;
  }    
}

/**
 * ags_lfo_synth_util_compute_sawtooth_s16:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO sawtooth synth of signed 16 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_sawtooth_s16(AgsLFOSynthUtil *lfo_synth_util)
{
  gint16 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

  phase = (int) ceil(phase) % (int) ceil(frequency);
  phase = ceil(phase / frequency) * ceil(samplerate / frequency);

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_sawtooth;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_sawtooth = (ags_v8double) {
      ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 3 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 4 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 5 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 6 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 7 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth)
    };

    v_buffer *= v_sawtooth;

    *(source) = (gint16) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)  
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_sawtooth[] = {
      ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 3 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 4 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 5 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 6 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 7 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
    };

    vDSP_vmulD(v_buffer, 1, v_sawtooth, 1, ret_v_buffer, 1, 8);

    *(source) = (gint16) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint16) ((gint32) source[0] * ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[1 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[2 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[3 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[4 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[5 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[6 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[7 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint16) ((gint32) source[0] * ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    
    source += lfo_synth_util->source_stride;
    i++;
  }    
}

/**
 * ags_lfo_synth_util_compute_sawtooth_s24:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO sawtooth synth of signed 24 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_sawtooth_s24(AgsLFOSynthUtil *lfo_synth_util)
{
  gint32 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

  phase = (int) ceil(phase) % (int) ceil(frequency);
  phase = ceil(phase / frequency) * ceil(samplerate / frequency);

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_sawtooth;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_sawtooth = (ags_v8double) {
      ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 3 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 4 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 5 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 6 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 7 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth)
    };

    v_buffer *= v_sawtooth;

    *(source) = (gint32) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)  
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_sawtooth[] = {
      ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 3 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 4 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 5 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 6 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 7 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
    };

    vDSP_vmulD(v_buffer, 1, v_sawtooth, 1, ret_v_buffer, 1, 8);

    *(source) = (gint32) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint32) ((gint32) source[0] * ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[1 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[2 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[3 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[4 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[5 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[6 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[7 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint32) ((gint32) source[0] * ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    
    source += lfo_synth_util->source_stride;
    i++;
  }    
}

/**
 * ags_lfo_synth_util_compute_sawtooth_s32:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO sawtooth synth of signed 32 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_sawtooth_s32(AgsLFOSynthUtil *lfo_synth_util)
{
  gint32 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

  phase = (int) ceil(phase) % (int) ceil(frequency);
  phase = ceil(phase / frequency) * ceil(samplerate / frequency);

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_sawtooth;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_sawtooth = (ags_v8double) {
      ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 3 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 4 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 5 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 6 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 7 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth)
    };

    v_buffer *= v_sawtooth;

    *(source) = (gint32) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)  
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_sawtooth[] = {
      ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 3 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 4 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 5 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 6 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 7 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
    };

    vDSP_vmulD(v_buffer, 1, v_sawtooth, 1, ret_v_buffer, 1, 8);

    *(source) = (gint32) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint32) ((gint64) source[0] * ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[1 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[2 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[3 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[4 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[5 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[6 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[7 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint32) ((gint64) source[0] * ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    
    source += lfo_synth_util->source_stride;
    i++;
  }    
}

/**
 * ags_lfo_synth_util_compute_sawtooth_s64:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO sawtooth synth of signed 64 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_sawtooth_s64(AgsLFOSynthUtil *lfo_synth_util)
{
  gint64 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

  phase = (int) ceil(phase) % (int) ceil(frequency);
  phase = ceil(phase / frequency) * ceil(samplerate / frequency);

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_sawtooth;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_sawtooth = (ags_v8double) {
      ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 3 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 4 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 5 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 6 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 7 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth)
    };

    v_buffer *= v_sawtooth;

    *(source) = (gint64) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)  
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_sawtooth[] = {
      ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 3 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 4 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 5 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 6 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 7 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
    };

    vDSP_vmulD(v_buffer, 1, v_sawtooth, 1, ret_v_buffer, 1, 8);

    *(source) = (gint64) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint64) ((gint64) source[0] * ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[1 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[2 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[3 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[4 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[5 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[6 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[7 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint64) ((gint64) source[0] * ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    
    source += lfo_synth_util->source_stride;
    i++;
  }    
}

/**
 * ags_lfo_synth_util_compute_sawtooth_float:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO sawtooth synth of single precision floating point data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_sawtooth_float(AgsLFOSynthUtil *lfo_synth_util)
{
  gfloat *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

  phase = (int) ceil(phase) % (int) ceil(frequency);
  phase = ceil(phase / frequency) * ceil(samplerate / frequency);

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_sawtooth;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_sawtooth = (ags_v8double) {
      ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 3 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 4 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 5 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 6 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 7 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth)
    };

    v_buffer *= v_sawtooth;

    *(source) = (gfloat) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)  
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_sawtooth[] = {
      ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 3 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 4 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 5 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 6 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 7 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
    };

    vDSP_vmulD(v_buffer, 1, v_sawtooth, 1, ret_v_buffer, 1, 8);

    *(source) = (gfloat) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gfloat) ((gdouble) source[0] * ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[1 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[2 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[3 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[4 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[5 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[6 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[7 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gfloat) ((gdouble) source[0] * ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    
    source += lfo_synth_util->source_stride;
    i++;
  }    
}

/**
 * ags_lfo_synth_util_compute_sawtooth_double:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO sawtooth synth of double precision floating point data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_sawtooth_double(AgsLFOSynthUtil *lfo_synth_util)
{
  gdouble *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

  phase = (int) ceil(phase) % (int) ceil(frequency);
  phase = ceil(phase / frequency) * ceil(samplerate / frequency);

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_sawtooth;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_sawtooth = (ags_v8double) {
      ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 3 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 4 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 5 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 6 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 7 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth)
    };

    v_buffer *= v_sawtooth;

    *(source) = (gdouble) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)  
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_sawtooth[] = {
      ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 3 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 4 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 5 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 6 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
      ((((fmod(ceil(offset + i + 7 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth),
    };

    vDSP_vmulD(v_buffer, 1, v_sawtooth, 1, ret_v_buffer, 1, 8);

    *(source) = (gdouble) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gdouble) ((gdouble) source[0] * ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[1 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 1 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[2 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[3 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[4 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[5 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[6 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[7 * lfo_synth_util->source_stride] * ((((fmod(ceil(offset + i + 2 + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gdouble) ((gdouble) source[0] * ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth));
    
    source += lfo_synth_util->source_stride;
    i++;
  }    
}

/**
 * ags_lfo_synth_util_compute_sawtooth_complex:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO sawtooth synth of complex data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_sawtooth_complex(AgsLFOSynthUtil *lfo_synth_util)
{
  AgsComplex *source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

  phase = (int) ceil(phase) % (int) ceil(frequency);
  phase = ceil(phase / frequency) * ceil(samplerate / frequency);

  i = 0;

  for(; i < lfo_synth_util->buffer_length;){
    ags_complex_set(source,
		    (ags_complex_get(source) * ((((fmod(ceil(offset + i + phase), ceil(samplerate / (exp2((tuning / 100.0) / 12.0) + (frequency))))) * 2.0 * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate) - 1.0) * lfo_depth)));
    
    source += lfo_synth_util->source_stride;
    i++;
  }    
}

/**
 * ags_lfo_synth_util_compute_sawtooth:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO sawtooth synth.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_sawtooth(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  switch(lfo_synth_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_lfo_synth_util_compute_sawtooth_s8(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_lfo_synth_util_compute_sawtooth_s16(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_lfo_synth_util_compute_sawtooth_s24(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_lfo_synth_util_compute_sawtooth_s32(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_lfo_synth_util_compute_sawtooth_s64(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_lfo_synth_util_compute_sawtooth_float(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_lfo_synth_util_compute_sawtooth_double(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_lfo_synth_util_compute_sawtooth_complex(lfo_synth_util);
  }
  break;
  }
}

/**
 * ags_lfo_synth_util_compute_triangle_s8:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO triangle synth of signed 8 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_triangle_s8(AgsLFOSynthUtil *lfo_synth_util)
{
  gint8 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_triangle;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_triangle = (ags_v8double) {
      ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5  + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)
    };

    v_buffer *= v_triangle;

    *(source) = (gint8) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_triangle[] = {
      ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5  + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
    };

    vDSP_vmulD(v_buffer, 1, v_triangle, 1, ret_v_buffer, 1, 8);

    *(source) = (gint8) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint8) ((gint16) source[0] * ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[0] * ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[0] * ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[0] * ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[0] * ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[0] * ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[0] * ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[0] * ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint8) ((gint16) source[0] * ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_triangle_s16:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO triangle synth of signed 16 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_triangle_s16(AgsLFOSynthUtil *lfo_synth_util)
{
  gint16 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_triangle;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_triangle = (ags_v8double) {
      ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5  + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)
    };

    v_buffer *= v_triangle;

    *(source) = (gint16) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_triangle[] = {
      ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5  + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
    };

    vDSP_vmulD(v_buffer, 1, v_triangle, 1, ret_v_buffer, 1, 8);

    *(source) = (gint16) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint16) ((gint32) source[0] * ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[0] * ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[0] * ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[0] * ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[0] * ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[0] * ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[0] * ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[0] * ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint16) ((gint32) source[0] * ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_triangle_s24:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO triangle synth of signed 24 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_triangle_s24(AgsLFOSynthUtil *lfo_synth_util)
{
  gint32 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_triangle;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_triangle = (ags_v8double) {
      ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5  + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)
    };

    v_buffer *= v_triangle;

    *(source) = (gint32) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_triangle[] = {
      ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5  + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
    };

    vDSP_vmulD(v_buffer, 1, v_triangle, 1, ret_v_buffer, 1, 8);

    *(source) = (gint32) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint32) ((gint32) source[0] * ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[0] * ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[0] * ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[0] * ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[0] * ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[0] * ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[0] * ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[0] * ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint32) ((gint32) source[0] * ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_triangle_s32:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO triangle synth of signed 32 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_triangle_s32(AgsLFOSynthUtil *lfo_synth_util)
{
  gint32 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_triangle;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_triangle = (ags_v8double) {
      ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5  + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)
    };

    v_buffer *= v_triangle;

    *(source) = (gint32) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_triangle[] = {
      ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5  + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
    };

    vDSP_vmulD(v_buffer, 1, v_triangle, 1, ret_v_buffer, 1, 8);

    *(source) = (gint32) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint32) ((gint64) source[0] * ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[0] * ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[0] * ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[0] * ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[0] * ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[0] * ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[0] * ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[0] * ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint32) ((gint64) source[0] * ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_triangle_s64:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO triangle synth of signed 64 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_triangle_s64(AgsLFOSynthUtil *lfo_synth_util)
{
  gint64 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_triangle;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_triangle = (ags_v8double) {
      ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5  + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)
    };

    v_buffer *= v_triangle;

    *(source) = (gint64) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_triangle[] = {
      ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5  + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
    };

    vDSP_vmulD(v_buffer, 1, v_triangle, 1, ret_v_buffer, 1, 8);

    *(source) = (gint64) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint64) ((gint64) source[0] * ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[0] * ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[0] * ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[0] * ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[0] * ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[0] * ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[0] * ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[0] * ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint64) ((gint64) source[0] * ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_triangle_float:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO triangle synth of single precision floating point data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_triangle_float(AgsLFOSynthUtil *lfo_synth_util)
{
  gfloat *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_triangle;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_triangle = (ags_v8double) {
      ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5  + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)
    };

    v_buffer *= v_triangle;

    *(source) = (gfloat) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_triangle[] = {
      ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5  + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
    };

    vDSP_vmulD(v_buffer, 1, v_triangle, 1, ret_v_buffer, 1, 8);

    *(source) = (gfloat) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gfloat) ((gdouble) source[0] * ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[0] * ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[0] * ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[0] * ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[0] * ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[0] * ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[0] * ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[0] * ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gfloat) ((gdouble) source[0] * ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_triangle_double:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO triangle synth of double precision floating point data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_triangle_double(AgsLFOSynthUtil *lfo_synth_util)
{
  gdouble *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_triangle;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_triangle = (ags_v8double) {
      ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5  + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)
    };

    v_buffer *= v_triangle;

    *(source) = (gdouble) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_triangle[] = {
      ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5  + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
      ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth),
    };

    vDSP_vmulD(v_buffer, 1, v_triangle, 1, ret_v_buffer, 1, 8);

    *(source) = (gdouble) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gdouble) ((gdouble) source[0] * ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[0] * ((((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 1 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[0] * ((((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 2 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[0] * ((((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 3 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[0] * ((((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 4 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[0] * ((((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 5 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[0] * ((((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 6 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[0] * ((((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + 7 + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gdouble) ((gdouble) source[0] * ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_triangle_complex:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO triangle synth of complex data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_triangle_complex(AgsLFOSynthUtil *lfo_synth_util)
{
  AgsComplex *source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i;
  
  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

  i = 0;
  
  for(; i < lfo_synth_util->buffer_length;){
    ags_complex_set(source,
		    ags_complex_get(source) * ((((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate * 2.0) - (((double) (((offset + i + phase) * (exp2((tuning / 100.0) / 12.0) + (frequency)) / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth));
    
    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_triangle:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO triangle synth.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_triangle(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  switch(lfo_synth_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_lfo_synth_util_compute_triangle_s8(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_lfo_synth_util_compute_triangle_s16(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_lfo_synth_util_compute_triangle_s24(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_lfo_synth_util_compute_triangle_s32(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_lfo_synth_util_compute_triangle_s64(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_lfo_synth_util_compute_triangle_float(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_lfo_synth_util_compute_triangle_double(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_lfo_synth_util_compute_triangle_complex(lfo_synth_util);
  }
  break;
  }
}

/**
 * ags_lfo_synth_util_compute_square_s8:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO square synth of signed 8 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_square_s8(AgsLFOSynthUtil *lfo_synth_util)
{
  gint8 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_square;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_square = (ags_v8double) {
      ((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth
    };

    v_buffer *= v_square;

    *(source) = (gint8) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_square[] = {
      ((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth
    };

    vDSP_vmulD(v_buffer, 1, v_square, 1, ret_v_buffer, 1, 8);

    *(source) = (gint8) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint8) ((gint16) source[0] * (((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[1 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[2 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[3 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[4 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[5 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[6 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[7 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint8) ((gint16) source[0] * (((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_square_s16:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO square synth of signed 16 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_square_s16(AgsLFOSynthUtil *lfo_synth_util)
{
  gint16 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_square;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_square = (ags_v8double) {
      ((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth
    };

    v_buffer *= v_square;

    *(source) = (gint16) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_square[] = {
      ((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth
    };

    vDSP_vmulD(v_buffer, 1, v_square, 1, ret_v_buffer, 1, 8);

    *(source) = (gint16) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint16) ((gint32) source[0] * (((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[1 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[2 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[3 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[4 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[5 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[6 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[7 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint16) ((gint32) source[0] * (((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_square_s24:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO square synth of signed 24 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_square_s24(AgsLFOSynthUtil *lfo_synth_util)
{
  gint32 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_square;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_square = (ags_v8double) {
      ((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth
    };

    v_buffer *= v_square;

    *(source) = (gint32) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_square[] = {
      ((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth
    };

    vDSP_vmulD(v_buffer, 1, v_square, 1, ret_v_buffer, 1, 8);

    *(source) = (gint32) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint32) ((gint32) source[0] * (((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[1 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[2 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[3 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[4 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[5 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[6 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[7 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint32) ((gint32) source[0] * (((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_square_s32:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO square synth of signed 32 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_square_s32(AgsLFOSynthUtil *lfo_synth_util)
{
  gint32 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_square;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_square = (ags_v8double) {
      ((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth
    };

    v_buffer *= v_square;

    *(source) = (gint32) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_square[] = {
      ((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth
    };

    vDSP_vmulD(v_buffer, 1, v_square, 1, ret_v_buffer, 1, 8);

    *(source) = (gint32) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint32) ((gint64) source[0] * (((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[1 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[2 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[3 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[4 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[5 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[6 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[7 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint32) ((gint64) source[0] * (((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_square_s64:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO square synth of signed 64 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_square_s64(AgsLFOSynthUtil *lfo_synth_util)
{
  gint64 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_square;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_square = (ags_v8double) {
      ((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth
    };

    v_buffer *= v_square;

    *(source) = (gint64) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_square[] = {
      ((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth
    };

    vDSP_vmulD(v_buffer, 1, v_square, 1, ret_v_buffer, 1, 8);

    *(source) = (gint64) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint64) ((gint64) source[0] * (((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[1 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[2 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[3 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[4 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[5 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[6 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[7 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint64) ((gint64) source[0] * (((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_square_float:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO square synth of single precision floating point data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_square_float(AgsLFOSynthUtil *lfo_synth_util)
{
  gfloat *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_square;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_square = (ags_v8double) {
      ((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth
    };

    v_buffer *= v_square;

    *(source) = (gfloat) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_square[] = {
      ((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth
    };

    vDSP_vmulD(v_buffer, 1, v_square, 1, ret_v_buffer, 1, 8);

    *(source) = (gfloat) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gfloat) ((gdouble) source[0] * (((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[1 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[2 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[3 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[4 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[5 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[6 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[7 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gfloat) ((gdouble) source[0] * (((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_square_double:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO square synth of double precision floating point data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_square_double(AgsLFOSynthUtil *lfo_synth_util)
{
  gdouble *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_square;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_square = (ags_v8double) {
      ((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth
    };

    v_buffer *= v_square;

    *(source) = (gdouble) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_square[] = {
      ((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth,
      ((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth
    };

    vDSP_vmulD(v_buffer, 1, v_square, 1, ret_v_buffer, 1, 8);

    *(source) = (gdouble) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gdouble) ((gdouble) source[0] * (((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[1 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[2 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[3 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[4 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[5 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[6 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[7 * lfo_synth_util->source_stride] * (((sin((gdouble) (offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gdouble) ((gdouble) source[0] * (((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_square_complex:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO square synth of complex data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_square_complex(AgsLFOSynthUtil *lfo_synth_util)
{
  AgsComplex *source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i;
  
  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

  i = 0;
  
  for(; i < lfo_synth_util->buffer_length;){
    ags_complex_set(source,
		    ags_complex_get(source) * (((sin((gdouble) (offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth));
    
    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_square:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO square synth.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_square(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  switch(lfo_synth_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_lfo_synth_util_compute_square_s8(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_lfo_synth_util_compute_square_s16(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_lfo_synth_util_compute_square_s24(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_lfo_synth_util_compute_square_s32(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_lfo_synth_util_compute_square_s64(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_lfo_synth_util_compute_square_float(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_lfo_synth_util_compute_square_double(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_lfo_synth_util_compute_square_complex(lfo_synth_util);
  }
  break;
  }
}

/**
 * ags_lfo_synth_util_compute_impulse_s8:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO impulse synth of signed 8 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_impulse_s8(AgsLFOSynthUtil *lfo_synth_util)
{
  gint8 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_impulse;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_impulse = (ags_v8double) {
      (sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth
    };

    v_buffer *= v_impulse;

    *(source) = (gint8) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint8) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_impulse[] = {
      (sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
    };

    vDSP_vmulD(v_buffer, 1, v_impulse, 1, ret_v_buffer, 1, 8);

    *(source) = (gint8) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint8) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint8) ((gint16) source[0] * ((sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint8) ((gint16) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint8) ((gint16) source[0] * ((sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_impulse_s16:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO impulse synth of signed 16 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_impulse_s16(AgsLFOSynthUtil *lfo_synth_util)
{
  gint16 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_impulse;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_impulse = (ags_v8double) {
      (sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth
    };

    v_buffer *= v_impulse;

    *(source) = (gint16) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint16) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_impulse[] = {
      (sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
    };

    vDSP_vmulD(v_buffer, 1, v_impulse, 1, ret_v_buffer, 1, 8);

    *(source) = (gint16) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint16) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint16) ((gint32) source[0] * ((sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint16) ((gint32) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint16) ((gint32) source[0] * ((sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_impulse_s24:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO impulse synth of signed 24 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_impulse_s24(AgsLFOSynthUtil *lfo_synth_util)
{
  gint32 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_impulse;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_impulse = (ags_v8double) {
      (sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth
    };

    v_buffer *= v_impulse;

    *(source) = (gint32) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_impulse[] = {
      (sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
    };

    vDSP_vmulD(v_buffer, 1, v_impulse, 1, ret_v_buffer, 1, 8);

    *(source) = (gint32) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint32) ((gint32) source[0] * ((sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint32) ((gint32) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint32) ((gint32) source[0] * ((sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_impulse_s32:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO impulse synth of signed 32 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_impulse_s32(AgsLFOSynthUtil *lfo_synth_util)
{
  gint32 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_impulse;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_impulse = (ags_v8double) {
      (sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth
    };

    v_buffer *= v_impulse;

    *(source) = (gint32) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_impulse[] = {
      (sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
    };

    vDSP_vmulD(v_buffer, 1, v_impulse, 1, ret_v_buffer, 1, 8);

    *(source) = (gint32) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint32) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint32) ((gint64) source[0] * ((sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint32) ((gint64) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint32) ((gint64) source[0] * ((sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_impulse_s64:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO impulse synth of signed 64 bit data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_impulse_s64(AgsLFOSynthUtil *lfo_synth_util)
{
  gint64 *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_impulse;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_impulse = (ags_v8double) {
      (sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth
    };

    v_buffer *= v_impulse;

    *(source) = (gint64) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint64) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_impulse[] = {
      (sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
    };

    vDSP_vmulD(v_buffer, 1, v_impulse, 1, ret_v_buffer, 1, 8);

    *(source) = (gint64) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gint64) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gint64) ((gint64) source[0] * ((sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gint64) ((gint64) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gint64) ((gint64) source[0] * ((sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_impulse_float:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO impulse synth of single precision floating point data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_impulse_float(AgsLFOSynthUtil *lfo_synth_util)
{
  gfloat *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_impulse;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_impulse = (ags_v8double) {
      (sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth
    };

    v_buffer *= v_impulse;

    *(source) = (gfloat) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gfloat) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};

    double v_impulse[] = {
      (sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
    };

    vDSP_vmulD(v_buffer, 1, v_impulse, 1, ret_v_buffer, 1, 8);

    *(source) = (gfloat) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gfloat) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gfloat) ((gdouble) source[0] * ((sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gfloat) ((gdouble) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gfloat) ((gdouble) source[0] * ((sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_impulse_double:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO impulse synth of double precision floating point data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_impulse_double(AgsLFOSynthUtil *lfo_synth_util)
{
  gdouble *source, *tmp_source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i, i_stop;

  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(i = 0; i < i_stop; i++){
    ags_v8double v_buffer, v_impulse;

    tmp_source = source;

    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride),
      (gdouble) *(tmp_source += lfo_synth_util->source_stride)
    };

    v_impulse = (ags_v8double) {
      (sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth
    };

    v_buffer *= v_impulse;

    *(source) = (gdouble) v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gdouble) v_buffer[7];

    source += lfo_synth_util->source_stride;    
      
    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    tmp_source = source;

    double v_buffer[] = {
      (double) *(tmp_source),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride),
      (double) *(tmp_source += lfo_synth_util->source_stride)};
    
    double v_impulse[] = {
      (sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
      (sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth,
    };

    vDSP_vmulD(v_buffer, 1, v_impulse, 1, ret_v_buffer, 1, 8);

    *(source) = (gdouble) ret_v_buffer[0];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[1];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[2];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[3];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[4];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[5];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[6];
    *(source += lfo_synth_util->source_stride) = (gdouble) ret_v_buffer[7];

    source += lfo_synth_util->source_stride;

    i += 8;
  }
#else
  i_stop = lfo_synth_util->buffer_length - (lfo_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    source[0] = (gdouble) ((gdouble) source[0] * ((sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[1 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 1 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[2 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 2 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[3 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 3 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[4 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 4 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[5 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 5 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[6 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 6 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    source[7 * lfo_synth_util->source_stride] = (gdouble) ((gdouble) source[1 * lfo_synth_util->source_stride] * ((sin((gdouble) ((offset + i + 7 + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    
    source += (8 * lfo_synth_util->source_stride);
    i += 8;
  }
#endif

  for(; i < lfo_synth_util->buffer_length;){
    source[0] = (gdouble) ((gdouble) source[0] * ((sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));

    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_impulse_complex:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO impulse synth of complex data.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_impulse_complex(AgsLFOSynthUtil *lfo_synth_util)
{
  AgsComplex *source;

  guint samplerate;
  gdouble frequency;
  gdouble phase;
  gdouble lfo_depth;
  gdouble tuning;
  guint offset;
  guint frame_count;
  guint i;
  
  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  source = lfo_synth_util->source;
  
  samplerate = lfo_synth_util->samplerate;

  frequency = lfo_synth_util->frequency;
  phase = lfo_synth_util->phase;

  lfo_depth = lfo_synth_util->lfo_depth;
  tuning = lfo_synth_util->tuning;
  
  offset = lfo_synth_util->offset;

  i = 0;
  
  for(; i < lfo_synth_util->buffer_length;){
    ags_complex_set(source,
		    ags_complex_get(source) * ((sin((gdouble) ((offset + i + phase) * 2.0 * M_PI * (exp2((tuning / 100.0) / 12.0) + (frequency)) / (gdouble) samplerate)) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * lfo_depth));
    
    source += lfo_synth_util->source_stride;
    i++;
  }
}

/**
 * ags_lfo_synth_util_compute_impulse:
 * @lfo_synth_util: the #AgsLFOSynthUtil-struct
 * 
 * Compute LFO impulse synth.
 *
 * Since: 4.0.0
 */
void
ags_lfo_synth_util_compute_impulse(AgsLFOSynthUtil *lfo_synth_util)
{
  if(lfo_synth_util == NULL ||
     lfo_synth_util->source == NULL){
    return;
  }

  switch(lfo_synth_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_lfo_synth_util_compute_impulse_s8(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_lfo_synth_util_compute_impulse_s16(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_lfo_synth_util_compute_impulse_s24(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_lfo_synth_util_compute_impulse_s32(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_lfo_synth_util_compute_impulse_s64(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_lfo_synth_util_compute_impulse_float(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_lfo_synth_util_compute_impulse_double(lfo_synth_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_lfo_synth_util_compute_impulse_complex(lfo_synth_util);
  }
  break;
  }
}
