/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/audio/ags_raven_synth_util.h>

#include <ags/audio/ags_audio_buffer_util.h>

#if defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#include <Accelerate/Accelerate.h>
#endif

#include <math.h>
#include <complex.h>

/**
 * SECTION:ags_raven_synth_util
 * @short_description: frequency modulation synth util
 * @title: AgsRavenSynthUtil
 * @section_id:
 * @include: ags/audio/ags_raven_synth_util.h
 *
 * Utility functions to compute FM synths with sequencer and vibrato.
 */

GType
ags_raven_synth_util_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_raven_synth_util = 0;

    ags_type_raven_synth_util =
      g_boxed_type_register_static("AgsRavenSynthUtil",
				   (GBoxedCopyFunc) ags_raven_synth_util_copy,
				   (GBoxedFreeFunc) ags_raven_synth_util_free);

    g_once_init_leave(&g_define_type_id__static, ags_type_raven_synth_util);
  }

  return(g_define_type_id__static);
}

/**
 * ags_raven_synth_util_alloc:
 *
 * Allocate #AgsRavenSynthUtil-struct
 *
 * Returns: a new #AgsRavenSynthUtil-struct
 *
 * Since: 7.7.0
 */
AgsRavenSynthUtil*
ags_raven_synth_util_alloc()
{
  AgsRavenSynthUtil *ptr;

  ptr = (AgsRavenSynthUtil *) g_new(AgsRavenSynthUtil,
				    1);

  ptr[0] = AGS_RAVEN_SYNTH_UTIL_INITIALIZER;

  return(ptr);
}

/**
 * ags_raven_synth_util_copy:
 * @ptr: the original #AgsRavenSynthUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsRavenSynthUtil-struct
 *
 * Since: 7.7.0
 */
gpointer
ags_raven_synth_util_copy(AgsRavenSynthUtil *ptr)
{
  AgsRavenSynthUtil *new_ptr;

  g_return_val_if_fail(ptr != NULL, NULL);

  new_ptr = (AgsRavenSynthUtil *) g_new(AgsRavenSynthUtil,
					1);
  
  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->synth_oscillator_mode = ptr->synth_oscillator_mode;

  new_ptr->frequency = ptr->frequency;
  new_ptr->phase = ptr->phase;
  new_ptr->volume = ptr->volume;

  new_ptr->lfo_oscillator_mode = ptr->lfo_oscillator_mode;

  new_ptr->lfo_frequency = ptr->lfo_frequency;
  new_ptr->lfo_depth = ptr->lfo_depth;
  new_ptr->tuning = ptr->tuning;

  new_ptr->vibrato_enabled = ptr->vibrato_enabled;

  new_ptr->vibrato_gain = ptr->vibrato_gain;
  new_ptr->vibrato_lfo_depth = ptr->vibrato_lfo_depth;
  new_ptr->vibrato_lfo_freq = ptr->vibrato_lfo_freq;
  new_ptr->vibrato_tuning = ptr->vibrato_tuning;

  new_ptr->frame_count = ptr->frame_count;
  new_ptr->offset = ptr->offset;

  new_ptr->note_256th_mode = ptr->note_256th_mode;

  new_ptr->offset_256th = ptr->offset_256th;
  
  return(new_ptr);
}

/**
 * ags_raven_synth_util_free:
 * @ptr: the #AgsRavenSynthUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_free(AgsRavenSynthUtil *ptr)
{
  g_return_if_fail(ptr != NULL);
  
  g_free(ptr);
}

/**
 * ags_raven_synth_util_get_source:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get source buffer of @raven_synth_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 7.7.0
 */
gpointer
ags_raven_synth_util_get_source(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(NULL);
  }

  return(raven_synth_util->source);
}

/**
 * ags_raven_synth_util_set_source:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @source: (transfer none): the source buffer
 *
 * Set @source buffer of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_source(AgsRavenSynthUtil *raven_synth_util,
				gpointer source)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->source = source;
}

/**
 * ags_raven_synth_util_get_source_stride:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get source stride of @raven_synth_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 7.7.0
 */
guint
ags_raven_synth_util_get_source_stride(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(0);
  }

  return(raven_synth_util->source_stride);
}

/**
 * ags_raven_synth_util_set_source_stride:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_source_stride(AgsRavenSynthUtil *raven_synth_util,
				       guint source_stride)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->source_stride = source_stride;
}

/**
 * ags_raven_synth_util_get_buffer_length:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get buffer length of @raven_synth_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 7.7.0
 */
guint
ags_raven_synth_util_get_buffer_length(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(0);
  }

  return(raven_synth_util->buffer_length);
}

/**
 * ags_raven_synth_util_set_buffer_length:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_buffer_length(AgsRavenSynthUtil *raven_synth_util,
				       guint buffer_length)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->buffer_length = buffer_length;
}

/**
 * ags_raven_synth_util_get_format:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get audio buffer util format of @raven_synth_util.
 * 
 * Returns: the audio buffer util format
 * 
 * Since: 7.7.0
 */
AgsSoundcardFormat
ags_raven_synth_util_get_format(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(0);
  }

  return(raven_synth_util->format);
}

/**
 * ags_raven_synth_util_set_format:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @format: the audio buffer util format
 *
 * Set @format of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_format(AgsRavenSynthUtil *raven_synth_util,
				AgsSoundcardFormat format)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->format = format;
}

/**
 * ags_raven_synth_util_get_samplerate:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get samplerate of @raven_synth_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 7.7.0
 */
guint
ags_raven_synth_util_get_samplerate(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(0);
  }

  return(raven_synth_util->samplerate);
}

/**
 * ags_raven_synth_util_set_samplerate:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_samplerate(AgsRavenSynthUtil *raven_synth_util,
				    guint samplerate)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->samplerate = samplerate;
}

/**
 * ags_raven_synth_util_get_synth_oscillator_mode:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get synth oscillator mode of @raven_synth_util.
 * 
 * Returns: the synth oscillator mode
 * 
 * Since: 7.7.0
 */
guint
ags_raven_synth_util_get_synth_oscillator_mode(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(0);
  }

  return(raven_synth_util->synth_oscillator_mode);
}

/**
 * ags_raven_synth_util_set_synth_oscillator_mode:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @synth_oscillator_mode: the synth oscillator mode
 *
 * Set @synth_oscillator_mode of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_synth_oscillator_mode(AgsRavenSynthUtil *raven_synth_util,
					       guint synth_oscillator_mode)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->synth_oscillator_mode = synth_oscillator_mode;
}

/**
 * ags_raven_synth_util_get_frequency:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get frequency of @raven_synth_util.
 * 
 * Returns: the frequency
 * 
 * Since: 7.7.0
 */
gdouble
ags_raven_synth_util_get_frequency(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(1.0);
  }

  return(raven_synth_util->frequency);
}

/**
 * ags_raven_synth_util_set_frequency:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @frequency: the frequency
 *
 * Set @frequency of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_frequency(AgsRavenSynthUtil *raven_synth_util,
				   gdouble frequency)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->frequency = frequency;
}

/**
 * ags_raven_synth_util_get_phase:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get phase of @raven_synth_util.
 * 
 * Returns: the phase
 * 
 * Since: 7.7.0
 */
gdouble
ags_raven_synth_util_get_phase(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(1.0);
  }

  return(raven_synth_util->phase);
}

/**
 * ags_raven_synth_util_set_phase:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @phase: the phase
 *
 * Set @phase of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_phase(AgsRavenSynthUtil *raven_synth_util,
			       gdouble phase)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->phase = phase;
}

/**
 * ags_raven_synth_util_get_volume:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get volume of @raven_synth_util.
 * 
 * Returns: the volume
 * 
 * Since: 7.7.0
 */
gdouble
ags_raven_synth_util_get_volume(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(1.0);
  }

  return(raven_synth_util->volume);
}

/**
 * ags_raven_synth_util_set_volume:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @volume: the volume
 *
 * Set @volume of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_volume(AgsRavenSynthUtil *raven_synth_util,
				gdouble volume)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->volume = volume;
}

/**
 * ags_raven_synth_util_get_seq_tuning:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @position: the sequencer position of tuning
 * 
 * Get seq tuning at @position of @raven_synth_util.
 * 
 * Returns: the seq tuning
 * 
 * Since: 7.7.0
 */
gdouble
ags_raven_synth_util_get_seq_tuning(AgsRavenSynthUtil *raven_synth_util,
				    gint position)
{
  if(raven_synth_util == NULL ||
     position < 0 ||
     position >= AGS_RAVEN_SYNTH_UTIL_SEQ_COUNT){
    return(0.0);
  }

  return(raven_synth_util->seq_tuning[position]);
}

/**
 * ags_raven_synth_util_set_seq_tuning:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @position: the sequencer position of tuning
 * @seq_tuning: the seq tuning
 *
 * Set @seq_tuning at @position of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_seq_tuning(AgsRavenSynthUtil *raven_synth_util,
				    gint position,
				    gdouble seq_tuning)
{
  if(raven_synth_util == NULL ||
     position < 0 ||
     position >= AGS_RAVEN_SYNTH_UTIL_SEQ_COUNT){
    return;
  }

  raven_synth_util->seq_tuning[position] = seq_tuning;
}

/**
 * ags_raven_synth_util_get_seq_tuning_pingpong:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get seq tuning pingpong of @raven_synth_util.
 * 
 * Returns: the seq tuning pingpong
 * 
 * Since: 7.7.0
 */
gboolean
ags_raven_synth_util_get_seq_tuning_pingpong(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(FALSE);
  }

  return(raven_synth_util->seq_tuning_pingpong);
}

/**
 * ags_raven_synth_util_set_seq_tuning_pingpong:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @seq_tuning_pingpong: the seq tuning pingpong
 *
 * Set @seq_tuning_pingpong of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_seq_tuning_pingpong(AgsRavenSynthUtil *raven_synth_util,
					     gboolean seq_tuning_pingpong)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->seq_tuning_pingpong = seq_tuning_pingpong;
}

/**
 * ags_raven_synth_util_get_seq_tuning_lfo_frequency:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get seq tuning lfo_frequency of @raven_synth_util.
 * 
 * Returns: the seq tuning lfo_frequency
 * 
 * Since: 7.7.0
 */
gdouble
ags_raven_synth_util_get_seq_tuning_lfo_frequency(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(0.0);
  }

  return(raven_synth_util->seq_tuning_lfo_frequency);
}

/**
 * ags_raven_synth_util_set_seq_tuning_lfo_frequency:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @seq_tuning_lfo_frequency: the seq tuning lfo_frequency
 *
 * Set @seq_tuning_lfo_frequency of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_seq_tuning_lfo_frequency(AgsRavenSynthUtil *raven_synth_util,
						  gdouble seq_tuning_lfo_frequency)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->seq_tuning_lfo_frequency = seq_tuning_lfo_frequency;
}

/**
 * ags_raven_synth_util_get_seq_volume:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @position: the sequencer position of volume
 * 
 * Get seq volume at @position of @raven_synth_util.
 * 
 * Returns: the seq volume
 * 
 * Since: 7.7.0
 */
gdouble
ags_raven_synth_util_get_seq_volume(AgsRavenSynthUtil *raven_synth_util,
				    gint position)
{
  if(raven_synth_util == NULL ||
     position < 0 ||
     position >= AGS_RAVEN_SYNTH_UTIL_SEQ_COUNT){
    return(0.0);
  }

  return(raven_synth_util->seq_volume[position]);
}

/**
 * ags_raven_synth_util_set_seq_volume:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @position: the sequencer position of volume
 * @seq_volume: the seq volume
 *
 * Set @seq_volume at @position of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_seq_volume(AgsRavenSynthUtil *raven_synth_util,
				    gint position,
				    gdouble seq_volume)
{
  if(raven_synth_util == NULL ||
     position < 0 ||
     position >= AGS_RAVEN_SYNTH_UTIL_SEQ_COUNT){
    return;
  }

  raven_synth_util->seq_volume[position] = seq_volume;
}

/**
 * ags_raven_synth_util_get_seq_volume_pingpong:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get seq volume pingpong of @raven_synth_util.
 * 
 * Returns: the seq volume pingpong
 * 
 * Since: 7.7.0
 */
gboolean
ags_raven_synth_util_get_seq_volume_pingpong(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(FALSE);
  }

  return(raven_synth_util->seq_volume_pingpong);
}

/**
 * ags_raven_synth_util_set_seq_volume_pingpong:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @seq_volume_pingpong: the seq volume pingpong
 *
 * Set @seq_volume_pingpong of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_seq_volume_pingpong(AgsRavenSynthUtil *raven_synth_util,
					     gboolean seq_volume_pingpong)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->seq_volume_pingpong = seq_volume_pingpong;
}

/**
 * ags_raven_synth_util_get_seq_volume_lfo_frequency:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get seq volume lfo_frequency of @raven_synth_util.
 * 
 * Returns: the seq volume lfo_frequency
 * 
 * Since: 7.7.0
 */
gdouble
ags_raven_synth_util_get_seq_volume_lfo_frequency(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(0.0);
  }

  return(raven_synth_util->seq_volume_lfo_frequency);
}

/**
 * ags_raven_synth_util_set_seq_volume_lfo_frequency:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @seq_volume_lfo_frequency: the seq volume lfo_frequency
 *
 * Set @seq_volume_lfo_frequency of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_seq_volume_lfo_frequency(AgsRavenSynthUtil *raven_synth_util,
						  gdouble seq_volume_lfo_frequency)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->seq_volume_lfo_frequency = seq_volume_lfo_frequency;
}

/**
 * ags_raven_synth_util_get_lfo_oscillator_mode:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get LFO oscillator mode of @raven_synth_util.
 * 
 * Returns: the LFO oscillator mode
 * 
 * Since: 7.7.0
 */
guint
ags_raven_synth_util_get_lfo_oscillator_mode(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(0);
  }

  return(raven_synth_util->lfo_oscillator_mode);
}

/**
 * ags_raven_synth_util_set_lfo_oscillator_mode:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @lfo_oscillator_mode: the LFO oscillator mode
 *
 * Set @lfo_oscillator_mode of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_lfo_oscillator_mode(AgsRavenSynthUtil *raven_synth_util,
					     guint lfo_oscillator_mode)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->lfo_oscillator_mode = lfo_oscillator_mode;
}

/**
 * ags_raven_synth_util_get_lfo_frequency:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get LFO frequency of @raven_synth_util.
 * 
 * Returns: the LFO frequency
 * 
 * Since: 7.7.0
 */
gdouble
ags_raven_synth_util_get_lfo_frequency(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(0.0);
  }

  return(raven_synth_util->lfo_frequency);
}

/**
 * ags_raven_synth_util_set_lfo_frequency:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @lfo_frequency: the LFO frequency
 *
 * Set @lfo_frequency of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_lfo_frequency(AgsRavenSynthUtil *raven_synth_util,
				       gdouble lfo_frequency)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->lfo_frequency = lfo_frequency;
}

/**
 * ags_raven_synth_util_get_lfo_depth:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get LFO depth of @raven_synth_util.
 * 
 * Returns: the LFO depth
 * 
 * Since: 7.7.0
 */
gdouble
ags_raven_synth_util_get_lfo_depth(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(1.0);
  }

  return(raven_synth_util->lfo_depth);
}

/**
 * ags_raven_synth_util_set_lfo_depth:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @lfo_depth: the LFO depth
 *
 * Set @lfo_depth of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_lfo_depth(AgsRavenSynthUtil *raven_synth_util,
				   gdouble lfo_depth)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->lfo_depth = lfo_depth;
}

/**
 * ags_raven_synth_util_get_tuning:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get tuning of @raven_synth_util.
 * 
 * Returns: the tuning
 * 
 * Since: 7.7.0
 */
gdouble
ags_raven_synth_util_get_tuning(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(1.0);
  }

  return(raven_synth_util->tuning);
}

/**
 * ags_raven_synth_util_set_tuning:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @tuning: the tuning
 *
 * Set @tuning of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_tuning(AgsRavenSynthUtil *raven_synth_util,
				gdouble tuning)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->tuning = tuning;
}

/**
 * ags_raven_synth_util_get_vibrato_enabled:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get vibrato enabled of @raven_synth_util.
 * 
 * Returns: %TRUE if the vibrato is enabled, otherwise %FALSE
 * 
 * Since: 7.7.0
 */
gboolean
ags_raven_synth_util_get_vibrato_enabled(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(FALSE);
  }

  return(raven_synth_util->vibrato_enabled);
}

/**
 * ags_raven_synth_util_set_vibrato_enabled:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @vibrato_enabled: the vibrato enabled
 *
 * Set @vibrato_enabled of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_vibrato_enabled(AgsRavenSynthUtil *raven_synth_util,
					 gboolean vibrato_enabled)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->vibrato_enabled = vibrato_enabled;
}

/**
 * ags_raven_synth_util_get_vibrato_gain:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get vibrato gain of @raven_synth_util.
 * 
 * Returns: the vibrato gain
 * 
 * Since: 7.7.0
 */
gdouble
ags_raven_synth_util_get_vibrato_gain(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(0.0);
  }

  return(raven_synth_util->vibrato_gain);
}

/**
 * ags_raven_synth_util_set_vibrato_gain:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @vibrato_gain: the vibrato gain
 *
 * Set @vibrato_gain of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_vibrato_gain(AgsRavenSynthUtil *raven_synth_util,
				      gdouble vibrato_gain)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->vibrato_gain = vibrato_gain;
}

/**
 * ags_raven_synth_util_get_vibrato_lfo_depth:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get vibrato lfo_depth of @raven_synth_util.
 * 
 * Returns: the vibrato lfo_depth
 * 
 * Since: 7.7.0
 */
gdouble
ags_raven_synth_util_get_vibrato_lfo_depth(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(0.0);
  }

  return(raven_synth_util->vibrato_lfo_depth);
}

/**
 * ags_raven_synth_util_set_vibrato_lfo_depth:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @vibrato_lfo_depth: the vibrato lfo_depth
 *
 * Set @vibrato_lfo_depth of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_vibrato_lfo_depth(AgsRavenSynthUtil *raven_synth_util,
					   gdouble vibrato_lfo_depth)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->vibrato_lfo_depth = vibrato_lfo_depth;
}

/**
 * ags_raven_synth_util_get_vibrato_lfo_freq:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get vibrato lfo_freq of @raven_synth_util.
 * 
 * Returns: the vibrato lfo_freq
 * 
 * Since: 7.7.0
 */
gdouble
ags_raven_synth_util_get_vibrato_lfo_freq(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(0.0);
  }

  return(raven_synth_util->vibrato_lfo_freq);
}

/**
 * ags_raven_synth_util_set_vibrato_lfo_freq:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @vibrato_lfo_freq: the vibrato lfo_freq
 *
 * Set @vibrato_lfo_freq of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_vibrato_lfo_freq(AgsRavenSynthUtil *raven_synth_util,
					  gdouble vibrato_lfo_freq)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->vibrato_lfo_freq = vibrato_lfo_freq;
}

/**
 * ags_raven_synth_util_get_vibrato_tuning:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get vibrato tuning of @raven_synth_util.
 * 
 * Returns: the vibrato tuning
 * 
 * Since: 7.7.0
 */
gdouble
ags_raven_synth_util_get_vibrato_tuning(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(0.0);
  }

  return(raven_synth_util->vibrato_tuning);
}

/**
 * ags_raven_synth_util_set_vibrato_tuning:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @vibrato_tuning: the vibrato tuning
 *
 * Set @vibrato_tuning of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_vibrato_tuning(AgsRavenSynthUtil *raven_synth_util,
					gdouble vibrato_tuning)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->vibrato_tuning = vibrato_tuning;
}

/**
 * ags_raven_synth_util_get_frame_count:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get frame count of @raven_synth_util.
 * 
 * Returns: the frame count
 * 
 * Since: 7.7.0
 */
guint
ags_raven_synth_util_get_frame_count(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(0);
  }

  return(raven_synth_util->frame_count);
}

/**
 * ags_raven_synth_util_set_frame_count:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @frame_count: the frame count
 *
 * Set @frame_count of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_frame_count(AgsRavenSynthUtil *raven_synth_util,
				     guint frame_count)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->frame_count = frame_count;
}

/**
 * ags_raven_synth_util_get_offset:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get offset of @raven_synth_util.
 * 
 * Returns: the offset
 * 
 * Since: 7.7.0
 */
guint
ags_raven_synth_util_get_offset(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(0);
  }

  return(raven_synth_util->offset);
}

/**
 * ags_raven_synth_util_set_offset:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @offset: the offset
 *
 * Set @offset of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_offset(AgsRavenSynthUtil *raven_synth_util,
				guint offset)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->offset = offset;
}

/**
 * ags_raven_synth_util_get_note_256th_mode:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get note 256th mode of @raven_synth_util.
 * 
 * Returns: %TRUE if note 256th mode, otherwise %FALSE
 * 
 * Since: 7.7.0
 */
gboolean
ags_raven_synth_util_get_note_256th_mode(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(0);
  }

  return(raven_synth_util->note_256th_mode);
}

/**
 * ags_raven_synth_util_set_note_256th_mode:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @note_256th_mode: the note 256th mode
 *
 * Set @note_256th_mode of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_note_256th_mode(AgsRavenSynthUtil *raven_synth_util,
					 gboolean note_256th_mode)
{
  if(raven_synth_util == NULL){
    return;
  }

  raven_synth_util->note_256th_mode = note_256th_mode;
}

/**
 * ags_raven_synth_util_get_offset_256th:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Get offset as note 256th of @raven_synth_util.
 * 
 * Returns: the offset as note 256th
 * 
 * Since: 7.7.0
 */
guint
ags_raven_synth_util_get_offset_256th(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL){
    return(0);
  }

  return(raven_synth_util->offset_256th);
}

/**
 * ags_raven_synth_util_set_offset_256th:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @offset_256th: the offset as note 256th
 *
 * Set @offset_256th of @raven_synth_util.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_set_offset_256th(AgsRavenSynthUtil *raven_synth_util,
				      guint offset_256th)
{
  if(raven_synth_util == NULL){
    return;
  }
  
  raven_synth_util->offset_256th = offset_256th;
}

/**
 * ags_raven_synth_util_get_tuning_by_offset:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @offset: the offset
 *
 * Get tuning by offset of @raven_synth_util.
 * 
 * Returns: the tuning
 *
 * Since: 7.7.0 
 */
gdouble
ags_raven_synth_util_get_tuning_by_offset(AgsRavenSynthUtil *raven_synth_util,
					  guint offset)
{
  gint position;
  guint samplerate;
  gdouble tuning_a, tuning_b;
  gdouble tuning;
  gdouble seq_tuning_lfo_frequency;
  gboolean seq_tuning_pingpong;
  guint seq_offset;
  guint seq_tuning_offset, seq_tuning_offset_end;
  
  samplerate = raven_synth_util->samplerate;

  tuning_a = 0.0;
  tuning_b = 0.0;
  
  seq_tuning_lfo_frequency = raven_synth_util->seq_tuning_lfo_frequency;
  
  seq_tuning_pingpong = raven_synth_util->seq_tuning_pingpong;

  seq_offset = 0;

  if(seq_tuning_lfo_frequency != 0.0){
    seq_offset = (guint) floor(offset / (samplerate / seq_tuning_lfo_frequency));
  }
  
  seq_tuning_offset = 0;
  seq_tuning_offset_end = 0;
  
  if(seq_tuning_lfo_frequency != 0.0){
    seq_tuning_offset_end = (guint) floor((double) samplerate / seq_tuning_lfo_frequency);
  }

  if(seq_tuning_offset_end != 0){
    seq_tuning_offset = offset % seq_tuning_offset_end;
  }

  position = seq_offset % AGS_RAVEN_SYNTH_UTIL_SEQ_COUNT;

  if(position == 0){
    if(!seq_tuning_pingpong){
      tuning_a = raven_synth_util->seq_tuning[0];
      tuning_b = raven_synth_util->seq_tuning[1];
    }else{
      if((guint) floor(seq_offset / (gdouble) AGS_RAVEN_SYNTH_UTIL_SEQ_COUNT) % 2 == 1){
	tuning_a = raven_synth_util->seq_tuning[0];
	tuning_b = raven_synth_util->seq_tuning[1];
      }else{
	tuning_a = raven_synth_util->seq_tuning[0];
	tuning_b = raven_synth_util->seq_tuning[1];
      }
    }
  }else if(position == 15){
    if(!seq_tuning_pingpong){
      tuning_a = raven_synth_util->seq_tuning[position];
      tuning_b = raven_synth_util->seq_tuning[0];
    }else{
      if((guint) floor(seq_offset / (gdouble) AGS_RAVEN_SYNTH_UTIL_SEQ_COUNT) % 2 == 1){
	tuning_a = raven_synth_util->seq_tuning[15];
	tuning_b = raven_synth_util->seq_tuning[14];
      }else{
	tuning_a = raven_synth_util->seq_tuning[15];
	tuning_b = raven_synth_util->seq_tuning[0];
      }
    }
  }else{
    if(!seq_tuning_pingpong){
      tuning_a = raven_synth_util->seq_tuning[position];
      tuning_b = raven_synth_util->seq_tuning[position + 1];
    }else{
      if((guint) floor(seq_offset / (gdouble) AGS_RAVEN_SYNTH_UTIL_SEQ_COUNT) % 2 == 1){
	tuning_a = raven_synth_util->seq_tuning[position];
	tuning_b = raven_synth_util->seq_tuning[position - 1];
      }else{
	tuning_a = raven_synth_util->seq_tuning[position];
	tuning_b = raven_synth_util->seq_tuning[position + 1];
      }
    }
  }

  tuning = 0.0;
  
  if(seq_tuning_offset_end != 0){
    tuning = ((seq_tuning_offset) * (tuning_a / seq_tuning_offset_end)) + ((seq_tuning_offset_end - seq_tuning_offset) * (tuning_b / seq_tuning_offset_end));
  }
  
  return(tuning);
}

/**
 * ags_raven_synth_util_get_volume_by_offset:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * @offset: the offset
 *
 * Get volume by offset of @raven_synth_util.
 * 
 * Returns: the volume
 *
 * Since: 7.7.0 
 */
gdouble
ags_raven_synth_util_get_volume_by_offset(AgsRavenSynthUtil *raven_synth_util,
					  guint offset)
{
  gint position;
  guint samplerate;
  gdouble volume_a, volume_b;
  gdouble volume;
  gdouble seq_volume_lfo_frequency;
  gboolean seq_volume_pingpong;
  guint seq_offset;
  guint seq_volume_offset, seq_volume_offset_end;
  
  samplerate = raven_synth_util->samplerate;

  volume_a = 1.0;
  volume_b = 1.0;
  
  seq_volume_lfo_frequency = raven_synth_util->seq_volume_lfo_frequency;
  
  seq_volume_pingpong = raven_synth_util->seq_volume_pingpong;

  seq_offset = 0;
  
  if(seq_volume_lfo_frequency != 0.0){
    seq_offset = (guint) floor(offset / (samplerate / seq_volume_lfo_frequency));
  }
  
  seq_volume_offset = 0;
  seq_volume_offset_end = 0;

  if(seq_volume_lfo_frequency != 0.0){
    seq_volume_offset_end = (guint) floor((double) samplerate / seq_volume_lfo_frequency);
  }

  if(seq_volume_offset_end != 0){
    seq_volume_offset = offset % seq_volume_offset_end;
  }

  position = seq_offset % AGS_RAVEN_SYNTH_UTIL_SEQ_COUNT;

  if(position == 0){
    if(!seq_volume_pingpong){
      volume_a = raven_synth_util->seq_volume[0];
      volume_b = raven_synth_util->seq_volume[1];
    }else{
      if((guint) floor(seq_offset / (gdouble) AGS_RAVEN_SYNTH_UTIL_SEQ_COUNT) % 2 == 1){
	volume_a = raven_synth_util->seq_volume[0];
	volume_b = raven_synth_util->seq_volume[1];
      }else{
	volume_a = raven_synth_util->seq_volume[0];
	volume_b = raven_synth_util->seq_volume[1];
      }
    }
  }else if(position == 15){
    if(!seq_volume_pingpong){
      volume_a = raven_synth_util->seq_volume[position];
      volume_b = raven_synth_util->seq_volume[0];
    }else{
      if((guint) floor(seq_offset / (gdouble) AGS_RAVEN_SYNTH_UTIL_SEQ_COUNT) % 2 == 1){
	volume_a = raven_synth_util->seq_volume[15];
	volume_b = raven_synth_util->seq_volume[14];
      }else{
	volume_a = raven_synth_util->seq_volume[15];
	volume_b = raven_synth_util->seq_volume[0];
      }
    }
  }else{
    if(!seq_volume_pingpong){
      volume_a = raven_synth_util->seq_volume[position];
      volume_b = raven_synth_util->seq_volume[position + 1];
    }else{
      if((guint) floor(seq_offset / (gdouble) AGS_RAVEN_SYNTH_UTIL_SEQ_COUNT) % 2 == 1){
	volume_a = raven_synth_util->seq_volume[position];
	volume_b = raven_synth_util->seq_volume[position - 1];
      }else{
	volume_a = raven_synth_util->seq_volume[position];
	volume_b = raven_synth_util->seq_volume[position + 1];
      }
    }
  }

  volume = 1.0;

  if(seq_volume_offset_end != 0){
    volume = ((seq_volume_offset) * (volume_a / seq_volume_offset_end)) + ((seq_volume_offset_end - seq_volume_offset) * (volume_b / seq_volume_offset_end));
  }
  
  return(volume);
}

/**
 * ags_raven_synth_util_compute_sin_s8:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven sine synth of signed 8 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_sin_s8(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_sin_s16:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven sine synth of signed 16 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_sin_s16(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_sin_s24:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven sine synth of signed 24 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_sin_s24(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_sin_s32:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven sine synth of signed 32 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_sin_s32(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_sin_s64:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven sine synth of signed 64 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_sin_s64(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_sin_float:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven sine synth of floating point data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_sin_float(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_sin_double:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven sine synth of double precision floating point data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_sin_double(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_sin_complex:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven sine synth of complex data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_sin_complex(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_sawtooth_s8:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven sawtooth synth of signed 8 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_sawtooth_s8(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_sawtooth_s16:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven sawtooth synth of signed 16 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_sawtooth_s16(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_sawtooth_s24:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven sawtooth synth of signed 24 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_sawtooth_s24(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_sawtooth_s32:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven sawtooth synth of signed 32 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_sawtooth_s32(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_sawtooth_s64:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven sawtooth synth of signed 64 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_sawtooth_s64(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_sawtooth_float:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven sawtooth synth of floating point data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_sawtooth_float(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_sawtooth_double:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven sawtooth synth of double precision floating point data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_sawtooth_double(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_sawtooth_complex:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven sawtooth synth of complex data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_sawtooth_complex(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_triangle_s8:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven triangle synth of signed 8 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_triangle_s8(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_triangle_s16:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven triangle synth of signed 16 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_triangle_s16(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_triangle_s24:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven triangle synth of signed 24 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_triangle_s24(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_triangle_s32:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven triangle synth of signed 32 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_triangle_s32(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_triangle_s64:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven triangle synth of signed 64 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_triangle_s64(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_triangle_float:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven triangle synth of floating point data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_triangle_float(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_triangle_double:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven triangle synth of double precision floating point data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_triangle_double(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_triangle_complex:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven triangle synth of complex data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_triangle_complex(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_square_s8:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven square synth of signed 8 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_square_s8(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_square_s16:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven square synth of signed 16 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_square_s16(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_square_s24:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven square synth of signed 24 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_square_s24(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_square_s32:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven square synth of signed 32 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_square_s32(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_square_s64:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven square synth of signed 64 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_square_s64(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_square_float:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven square synth of floating point data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_square_float(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_square_double:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven square synth of double precision floating point data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_square_double(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_square_complex:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven square synth of complex data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_square_complex(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_impulse_s8:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven impulse synth of signed 8 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_impulse_s8(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_impulse_s16:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven impulse synth of signed 16 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_impulse_s16(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_impulse_s24:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven impulse synth of signed 24 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_impulse_s24(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_impulse_s32:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven impulse synth of signed 32 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_impulse_s32(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_impulse_s64:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven impulse synth of signed 64 bit data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_impulse_s64(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_impulse_float:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven impulse synth of floating point data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_impulse_float(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_impulse_double:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven impulse synth of double precision floating point data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_impulse_double(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_raven_synth_util_compute_impulse_complex:
 * @raven_synth_util: the #AgsRavenSynthUtil-struct
 * 
 * Compute raven impulse synth of complex data.
 *
 * Since: 7.7.0
 */
void
ags_raven_synth_util_compute_impulse_complex(AgsRavenSynthUtil *raven_synth_util)
{
  if(raven_synth_util == NULL ||
     raven_synth_util->source == NULL){
    return;
  }

  //TODO:JK: implement me
}
