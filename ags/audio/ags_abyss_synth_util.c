/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#include <ags/audio/ags_abyss_synth_util.h>

#include <ags/libags-audio-globals.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_common_pitch_util.h>
#include <ags/audio/ags_fluid_util.h>
#include <ags/audio/ags_fluid_interpolate_4th_order_util.h>
#include <ags/audio/ags_envelope_util.h>
#include <ags/audio/ags_lfo_synth_util.h>
#include <ags/audio/ags_noise_util.h>

#include <math.h>
#include <complex.h>

/**
 * SECTION:ags_abyss_synth_util
 * @short_description: frequency modulation synth util
 * @title: AgsAbyssSynthUtil
 * @section_id:
 * @include: ags/audio/ags_abyss_synth_util.h
 *
 * Utility functions to compute abyss synths.
 */

GType
ags_abyss_synth_util_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_abyss_synth_util = 0;

    ags_type_abyss_synth_util =
      g_boxed_type_register_static("AgsAbyssSynthUtil",
				   (GBoxedCopyFunc) ags_abyss_synth_util_copy,
				   (GBoxedFreeFunc) ags_abyss_synth_util_free);

    g_once_init_leave(&g_define_type_id__static, ags_type_abyss_synth_util);
  }

  return(g_define_type_id__static);
}

/**
 * ags_abyss_synth_util_alloc:
 *
 * Allocate #AgsAbyssSynthUtil-struct
 *
 * Returns: a new #AgsAbyssSynthUtil-struct
 *
 * Since: 9.4.0
 */
AgsAbyssSynthUtil*
ags_abyss_synth_util_alloc()
{
  AgsAbyssSynthUtil *ptr;

  ptr = (AgsAbyssSynthUtil *) g_new(AgsAbyssSynthUtil,
				    1);

  ptr[0] = AGS_ABYSS_SYNTH_UTIL_INITIALIZER;
  
  return(ptr);
}

/**
 * ags_abyss_synth_util_copy:
 * @ptr: the original #AgsAbyssSynthUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsAbyssSynthUtil-struct
 *
 * Since: 9.4.0
 */
gpointer
ags_abyss_synth_util_copy(AgsAbyssSynthUtil *ptr)
{
  AgsAbyssSynthUtil *new_ptr;
  
  g_return_val_if_fail(ptr != NULL, NULL);

  new_ptr = (AgsAbyssSynthUtil *) g_new(AgsAbyssSynthUtil,
					1);
  
  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;
  
  return(new_ptr);
}

/**
 * ags_abyss_synth_util_free:
 * @ptr: the #AgsAbyssSynthUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_free(AgsAbyssSynthUtil *ptr)
{
  g_return_if_fail(ptr != NULL);
 
  g_free(ptr);
}

/**
 * ags_abyss_synth_util_get_source:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get source buffer of @abyss_synth_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 9.4.0
 */
gpointer
ags_abyss_synth_util_get_source(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(NULL);
  }

  return(abyss_synth_util->source);
}

/**
 * ags_abyss_synth_util_set_source:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @source: (transfer none): the source buffer
 *
 * Set @source buffer of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_source(AgsAbyssSynthUtil *abyss_synth_util,
				gpointer source)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->source = source;
}

/**
 * ags_abyss_synth_util_get_source_stride:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get source stride of @abyss_synth_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 9.4.0
 */
guint
ags_abyss_synth_util_get_source_stride(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(0);
  }

  return(abyss_synth_util->source_stride);
}

/**
 * ags_abyss_synth_util_set_source_stride:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_source_stride(AgsAbyssSynthUtil *abyss_synth_util,
				       guint source_stride)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->source_stride = source_stride;
}

/**
 * ags_abyss_synth_util_get_buffer_length:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get buffer length of @abyss_synth_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 9.4.0
 */
guint
ags_abyss_synth_util_get_buffer_length(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(0);
  }

  return(abyss_synth_util->buffer_length);
}

/**
 * ags_abyss_synth_util_set_buffer_length:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_buffer_length(AgsAbyssSynthUtil *abyss_synth_util,
				       guint buffer_length)
{
  if(abyss_synth_util == NULL ||
     abyss_synth_util->buffer_length == buffer_length){
    return;
  }

  abyss_synth_util->buffer_length = buffer_length;

  ags_common_pitch_util_set_buffer_length(abyss_synth_util->pitch_util,
					  abyss_synth_util->pitch_type,
					  buffer_length);

  ags_noise_util_set_buffer_length(abyss_synth_util->noise_0_util,
				   buffer_length);

  ags_noise_util_set_buffer_length(abyss_synth_util->noise_1_util,
				   buffer_length);

  /* pitch buffer */
  ags_stream_free(abyss_synth_util->pitch_buffer);
  
  abyss_synth_util->pitch_buffer = NULL;

  if(buffer_length > 0){
    abyss_synth_util->pitch_buffer = ags_stream_alloc(buffer_length,
						      AGS_SOUNDCARD_DOUBLE);
  }

  /* env-0 */
  ags_stream_free(abyss_synth_util->env_0_buffer);

  abyss_synth_util->env_0_buffer = NULL;
  
  if(buffer_length > 0){
    abyss_synth_util->env_0_buffer = ags_stream_alloc(buffer_length,
						      AGS_SOUNDCARD_DOUBLE);
  }

  /* env-1 */
  ags_stream_free(abyss_synth_util->env_1_buffer);

  abyss_synth_util->env_1_buffer = NULL;
  
  if(buffer_length > 0){
    abyss_synth_util->env_1_buffer = ags_stream_alloc(buffer_length,
						      AGS_SOUNDCARD_DOUBLE);
  }

  /* env-2 */
  ags_stream_free(abyss_synth_util->env_2_buffer);

  abyss_synth_util->env_2_buffer = NULL;
  
  if(buffer_length > 0){
    abyss_synth_util->env_2_buffer = ags_stream_alloc(buffer_length,
						      AGS_SOUNDCARD_DOUBLE);
  }

  /* env-3 */
  ags_stream_free(abyss_synth_util->env_3_buffer);

  abyss_synth_util->env_3_buffer = NULL;
  
  if(buffer_length > 0){
    abyss_synth_util->env_3_buffer = ags_stream_alloc(buffer_length,
						      AGS_SOUNDCARD_DOUBLE);
  }

  /* lfo-0 */
  ags_stream_free(abyss_synth_util->lfo_0_buffer);

  abyss_synth_util->lfo_0_buffer = NULL;
  
  if(buffer_length > 0){
    abyss_synth_util->lfo_0_buffer = ags_stream_alloc(buffer_length,
						      AGS_SOUNDCARD_DOUBLE);
  }

  /* lfo-1 */
  ags_stream_free(abyss_synth_util->lfo_1_buffer);

  abyss_synth_util->lfo_1_buffer = NULL;
  
  if(buffer_length > 0){
    abyss_synth_util->lfo_1_buffer = ags_stream_alloc(buffer_length,
						      AGS_SOUNDCARD_DOUBLE);
  }

  /* lfo-2 */
  ags_stream_free(abyss_synth_util->lfo_2_buffer);

  abyss_synth_util->lfo_2_buffer = NULL;
  
  if(buffer_length > 0){
    abyss_synth_util->lfo_2_buffer = ags_stream_alloc(buffer_length,
						      AGS_SOUNDCARD_DOUBLE);
  }

  /* lfo-3 */
  ags_stream_free(abyss_synth_util->lfo_3_buffer);

  abyss_synth_util->lfo_3_buffer = NULL;
  
  if(buffer_length > 0){
    abyss_synth_util->lfo_3_buffer = ags_stream_alloc(buffer_length,
						      AGS_SOUNDCARD_DOUBLE);
  }

  /* noise 0 */
  ags_stream_free(abyss_synth_util->noise_0_buffer);

  abyss_synth_util->noise_0_buffer = NULL;
  
  if(buffer_length > 0){
    abyss_synth_util->noise_0_buffer = ags_stream_alloc(buffer_length,
							AGS_SOUNDCARD_DOUBLE);
  }  

  /* noise 1 */
  ags_stream_free(abyss_synth_util->noise_1_buffer);

  abyss_synth_util->noise_1_buffer = NULL;
  
  if(buffer_length > 0){
    abyss_synth_util->noise_1_buffer = ags_stream_alloc(buffer_length,
							AGS_SOUNDCARD_DOUBLE);
  }  
}

/**
 * ags_abyss_synth_util_get_format:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get audio buffer util format of @abyss_synth_util.
 * 
 * Returns: the audio buffer util format
 * 
 * Since: 9.4.0
 */
AgsSoundcardFormat
ags_abyss_synth_util_get_format(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(0);
  }

  return(abyss_synth_util->format);
}

/**
 * ags_abyss_synth_util_set_format:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @format: the audio buffer util format
 *
 * Set @format of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_format(AgsAbyssSynthUtil *abyss_synth_util,
				AgsSoundcardFormat format)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->format = format;

  /*  */
  ags_common_pitch_util_set_format(abyss_synth_util->pitch_util,
				   abyss_synth_util->pitch_type,
				   format);

  ags_noise_util_set_format(abyss_synth_util->noise_0_util,
			    format);

  ags_noise_util_set_format(abyss_synth_util->noise_1_util,
			    format);
}

/**
 * ags_abyss_synth_util_get_samplerate:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get samplerate of @abyss_synth_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 9.4.0
 */
guint
ags_abyss_synth_util_get_samplerate(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(0);
  }

  return(abyss_synth_util->samplerate);
}

/**
 * ags_abyss_synth_util_set_samplerate:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_samplerate(AgsAbyssSynthUtil *abyss_synth_util,
				    guint samplerate)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->samplerate = samplerate;

  /* env */
  ags_envelope_util_set_frame_count(abyss_synth_util->env_0_util,
				    samplerate / abyss_synth_util->env_0_frequency);

  ags_envelope_util_set_frame_count(abyss_synth_util->env_1_util,
				    samplerate / abyss_synth_util->env_1_frequency);

  ags_envelope_util_set_frame_count(abyss_synth_util->env_2_util,
				    samplerate / abyss_synth_util->env_2_frequency);

  ags_envelope_util_set_frame_count(abyss_synth_util->env_3_util,
				    samplerate / abyss_synth_util->env_3_frequency);

  /* lfo */
  ags_lfo_synth_util_set_frame_count(abyss_synth_util->lfo_0_util,
				     samplerate / abyss_synth_util->lfo_0_frequency);

  ags_lfo_synth_util_set_frame_count(abyss_synth_util->lfo_1_util,
				     samplerate / abyss_synth_util->lfo_1_frequency);

  ags_lfo_synth_util_set_frame_count(abyss_synth_util->lfo_2_util,
				     samplerate / abyss_synth_util->lfo_2_frequency);

  ags_lfo_synth_util_set_frame_count(abyss_synth_util->lfo_3_util,
				     samplerate / abyss_synth_util->lfo_3_frequency);

  /* pitch */
  ags_common_pitch_util_set_samplerate(abyss_synth_util->pitch_util,
				       abyss_synth_util->pitch_type,
				       samplerate);

  /* noise */
  ags_noise_util_set_samplerate(abyss_synth_util->noise_0_util,
				samplerate);

  ags_noise_util_set_samplerate(abyss_synth_util->noise_1_util,
				samplerate);
}

/**
 * ags_abyss_synth_util_get_osc_0_oscillator:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get osc-0 oscillator mode of @abyss_synth_util.
 * 
 * Returns: the osc-0 oscillator mode as #AgsSynthOscillatorMode-enum
 * 
 * Since: 9.4.0
 */
AgsSynthOscillatorMode
ags_abyss_synth_util_get_osc_0_oscillator(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(AGS_SYNTH_OSCILLATOR_SIN);
  }

  return(abyss_synth_util->osc_0_oscillator);
}

/**
 * ags_abyss_synth_util_set_osc_0_oscillator:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @osc_0_oscillator: the osc-0 oscillator mode as #AgsSynthOscillatorMode-enum
 *
 * Set @osc_0_oscillator of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_osc_0_oscillator(AgsAbyssSynthUtil *abyss_synth_util,
					  AgsSynthOscillatorMode osc_0_oscillator)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->osc_0_oscillator = osc_0_oscillator;
}

/**
 * ags_abyss_synth_util_get_osc_0_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get osc-0 frequency of @abyss_synth_util.
 * 
 * Returns: the osc-0 frequency
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_osc_0_frequency(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(440.0);
  }

  return(abyss_synth_util->osc_0_frequency);
}

/**
 * ags_abyss_synth_util_set_osc_0_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @osc_0_frequency: the osc-0 frequency
 *
 * Set @osc_0_frequency of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_osc_0_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					 gdouble osc_0_frequency)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->osc_0_frequency = osc_0_frequency;
}

/**
 * ags_abyss_synth_util_get_osc_0_phase:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get osc-0 phase of @abyss_synth_util.
 * 
 * Returns: the osc-0 phase
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_osc_0_phase(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->osc_0_phase);
}

/**
 * ags_abyss_synth_util_set_osc_0_phase:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @osc_0_phase: the osc-0 phase
 *
 * Set @osc_0_phase of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_osc_0_phase(AgsAbyssSynthUtil *abyss_synth_util,
				     gdouble osc_0_phase)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->osc_0_phase = osc_0_phase;
}

/**
 * ags_abyss_synth_util_get_osc_0_volume:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get osc-0 volume of @abyss_synth_util.
 * 
 * Returns: the osc-0 volume
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_osc_0_volume(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->osc_0_volume);
}

/**
 * ags_abyss_synth_util_set_osc_0_volume:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @osc_0_volume: the osc-0 volume
 *
 * Set @osc_0_volume of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_osc_0_volume(AgsAbyssSynthUtil *abyss_synth_util,
				      gdouble osc_0_volume)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->osc_0_volume = osc_0_volume;
}

/**
 * ags_abyss_synth_util_get_osc_1_oscillator:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get osc-1 oscillator mode of @abyss_synth_util.
 * 
 * Returns: the osc-1 oscillator mode as #AgsSynthOscillatorMode-enum
 * 
 * Since: 9.4.0
 */
AgsSynthOscillatorMode
ags_abyss_synth_util_get_osc_1_oscillator(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(AGS_SYNTH_OSCILLATOR_SIN);
  }

  return(abyss_synth_util->osc_1_oscillator);
}

/**
 * ags_abyss_synth_util_set_osc_1_oscillator:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @osc_1_oscillator: the osc-1 oscillator mode as #AgsSynthOscillatorMode-enum
 *
 * Set @osc_1_oscillator of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_osc_1_oscillator(AgsAbyssSynthUtil *abyss_synth_util,
					  AgsSynthOscillatorMode osc_1_oscillator)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->osc_1_oscillator = osc_1_oscillator;
}

/**
 * ags_abyss_synth_util_get_osc_1_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get osc-1 frequency of @abyss_synth_util.
 * 
 * Returns: the osc-1 frequency
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_osc_1_frequency(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(440.0);
  }

  return(abyss_synth_util->osc_1_frequency);
}

/**
 * ags_abyss_synth_util_set_osc_1_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @osc_1_frequency: the osc-1 frequency
 *
 * Set @osc_1_frequency of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_osc_1_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					 gdouble osc_1_frequency)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->osc_1_frequency = osc_1_frequency;
}

/**
 * ags_abyss_synth_util_get_osc_1_phase:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get osc-1 phase of @abyss_synth_util.
 * 
 * Returns: the osc-1 phase
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_osc_1_phase(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->osc_1_phase);
}

/**
 * ags_abyss_synth_util_set_osc_1_phase:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @osc_1_phase: the osc-1 phase
 *
 * Set @osc_1_phase of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_osc_1_phase(AgsAbyssSynthUtil *abyss_synth_util,
				     gdouble osc_1_phase)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->osc_1_phase = osc_1_phase;
}

/**
 * ags_abyss_synth_util_get_osc_1_volume:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get osc-1 volume of @abyss_synth_util.
 * 
 * Returns: the osc-1 volume
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_osc_1_volume(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->osc_1_volume);
}

/**
 * ags_abyss_synth_util_set_osc_1_volume:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @osc_1_volume: the osc-1 volume
 *
 * Set @osc_1_volume of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_osc_1_volume(AgsAbyssSynthUtil *abyss_synth_util,
				      gdouble osc_1_volume)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->osc_1_volume = osc_1_volume;
}

/**
 * ags_abyss_synth_util_get_pitch_type:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get pitch type of @abyss_synth_util.
 * 
 * Returns: the pitch type
 * 
 * Since: 9.4.0
 */
GType
ags_abyss_synth_util_get_pitch_type(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL);
  }

  return(abyss_synth_util->pitch_type);
}

/**
 * ags_abyss_synth_util_set_pitch_type:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @pitch_type: the pitch type
 *
 * Set @pitch_type of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_pitch_type(AgsAbyssSynthUtil *abyss_synth_util,
				    GType pitch_type)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->pitch_type = pitch_type;
}

/**
 * ags_abyss_synth_util_get_pitch_base_key:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get pitch base key of @abyss_synth_util.
 * 
 * Returns: the pitch base key
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_pitch_base_key(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(440.0);
  }

  return(abyss_synth_util->pitch_base_key);
}

/**
 * ags_abyss_synth_util_set_pitch_base_key:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @pitch_base_key: the pitch base key
 *
 * Set @pitch_base_key of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_pitch_base_key(AgsAbyssSynthUtil *abyss_synth_util,
					gdouble pitch_base_key)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->pitch_base_key = pitch_base_key;
}

/**
 * ags_abyss_synth_util_get_pitch_tuning:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get pitch tuning of @abyss_synth_util.
 * 
 * Returns: the pitch tuning
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_pitch_tuning(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(0.0);
  }

  return(abyss_synth_util->pitch_tuning);
}

/**
 * ags_abyss_synth_util_set_pitch_tuning:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @pitch_tuning: the pitch tuning
 *
 * Set @pitch_tuning of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_pitch_tuning(AgsAbyssSynthUtil *abyss_synth_util,
				      gdouble pitch_tuning)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->pitch_tuning = pitch_tuning;
}

/**
 * ags_abyss_synth_util_get_volume:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get volume of @abyss_synth_util.
 * 
 * Returns: the volume
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_volume(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->volume);
}

/**
 * ags_abyss_synth_util_set_volume:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @volume: the volume
 *
 * Set @volume of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_volume(AgsAbyssSynthUtil *abyss_synth_util,
				gdouble volume)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->volume = volume;
}

/**
 * ags_abyss_synth_util_get_env_0_attack:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-0 attack of @abyss_synth_util.
 * 
 * Returns: the env-0 attack
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_0_attack(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_0_attack);
}

/**
 * ags_abyss_synth_util_set_env_0_attack:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_0_attack: the env-0 attack
 *
 * Set @env_0_attack of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_0_attack(AgsAbyssSynthUtil *abyss_synth_util,
				      gdouble env_0_attack)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_0_attack = env_0_attack;
}

/**
 * ags_abyss_synth_util_get_env_0_decay:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-0 decay of @abyss_synth_util.
 * 
 * Returns: the env-0 decay
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_0_decay(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_0_decay);
}

/**
 * ags_abyss_synth_util_set_env_0_decay:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_0_decay: the env-0 decay
 *
 * Set @env_0_decay of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_0_decay(AgsAbyssSynthUtil *abyss_synth_util,
				     gdouble env_0_decay)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_0_decay = env_0_decay;
}

/**
 * ags_abyss_synth_util_get_env_0_sustain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-0 sustain of @abyss_synth_util.
 * 
 * Returns: the env-0 sustain
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_0_sustain(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_0_sustain);
}

/**
 * ags_abyss_synth_util_set_env_0_sustain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_0_sustain: the env-0 sustain
 *
 * Set @env_0_sustain of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_0_sustain(AgsAbyssSynthUtil *abyss_synth_util,
				       gdouble env_0_sustain)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_0_sustain = env_0_sustain;
}

/**
 * ags_abyss_synth_util_get_env_0_release:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-0 release of @abyss_synth_util.
 * 
 * Returns: the env-0 release
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_0_release(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_0_release);
}

/**
 * ags_abyss_synth_util_set_env_0_release:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_0_release: the env-0 release
 *
 * Set @env_0_release of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_0_release(AgsAbyssSynthUtil *abyss_synth_util,
				       gdouble env_0_release)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_0_release = env_0_release;
}

/**
 * ags_abyss_synth_util_get_env_0_gain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-0 gain of @abyss_synth_util.
 * 
 * Returns: the env-0 gain
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_0_gain(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_0_gain);
}

/**
 * ags_abyss_synth_util_set_env_0_gain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_0_gain: the env-0 gain
 *
 * Set @env_0_gain of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_0_gain(AgsAbyssSynthUtil *abyss_synth_util,
				    gdouble env_0_gain)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_0_gain = env_0_gain;
}

/**
 * ags_abyss_synth_util_get_env_0_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-0 frequency of @abyss_synth_util.
 * 
 * Returns: the env-0 frequency
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_0_frequency(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_0_frequency);
}

/**
 * ags_abyss_synth_util_set_env_0_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_0_frequency: the env-0 frequency
 *
 * Set @env_0_frequency of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_0_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					 gdouble env_0_frequency)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_0_frequency = env_0_frequency;

  ags_envelope_util_set_frame_count(abyss_synth_util->env_0_util,
				    abyss_synth_util->samplerate / env_0_frequency);
}

/**
 * ags_abyss_synth_util_get_env_0_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_0_sends_count: (out): return location of the env-0 sends count
 * 
 * Get env-0 sends of @abyss_synth_util.
 * 
 * Returns: (transfer none): the env-0 sends
 * 
 * Since: 9.4.0
 */
gint64*
ags_abyss_synth_util_get_env_0_sends(AgsAbyssSynthUtil *abyss_synth_util,
				     guint *env_0_sends_count)
{
  if(abyss_synth_util == NULL){
    return(NULL);
  }

  if(env_0_sends_count != NULL){
    env_0_sends_count[0] = AGS_ABYSS_SYNTH_SENDS_COUNT;
  }

  return(&(abyss_synth_util->env_0_sends[0]));
}

/**
 * ags_abyss_synth_util_set_env_0_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_0_sends: the env-0 sends
 * @env_0_sends_count: the env-0 sends count
 *
 * Set @env_0_sends of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_0_sends(AgsAbyssSynthUtil *abyss_synth_util,
				     gint64 *env_0_sends,
				     guint env_0_sends_count)
{
  if(abyss_synth_util == NULL){
    return;
  }

  memcpy(&(abyss_synth_util->env_0_sends[0]), env_0_sends, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
}

/**
 * ags_abyss_synth_util_get_env_1_attack:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-1 attack of @abyss_synth_util.
 * 
 * Returns: the env-1 attack
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_1_attack(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_1_attack);
}

/**
 * ags_abyss_synth_util_set_env_1_attack:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_1_attack: the env-1 attack
 *
 * Set @env_1_attack of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_1_attack(AgsAbyssSynthUtil *abyss_synth_util,
				      gdouble env_1_attack)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_1_attack = env_1_attack;
}

/**
 * ags_abyss_synth_util_get_env_1_decay:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-1 decay of @abyss_synth_util.
 * 
 * Returns: the env-1 decay
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_1_decay(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_1_decay);
}

/**
 * ags_abyss_synth_util_set_env_1_decay:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_1_decay: the env-1 decay
 *
 * Set @env_1_decay of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_1_decay(AgsAbyssSynthUtil *abyss_synth_util,
				     gdouble env_1_decay)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_1_decay = env_1_decay;
}

/**
 * ags_abyss_synth_util_get_env_1_sustain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-1 sustain of @abyss_synth_util.
 * 
 * Returns: the env-1 sustain
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_1_sustain(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_1_sustain);
}

/**
 * ags_abyss_synth_util_set_env_1_sustain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_1_sustain: the env-1 sustain
 *
 * Set @env_1_sustain of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_1_sustain(AgsAbyssSynthUtil *abyss_synth_util,
				       gdouble env_1_sustain)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_1_sustain = env_1_sustain;
}

/**
 * ags_abyss_synth_util_get_env_1_release:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-1 release of @abyss_synth_util.
 * 
 * Returns: the env-1 release
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_1_release(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_1_release);
}

/**
 * ags_abyss_synth_util_set_env_1_release:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_1_release: the env-1 release
 *
 * Set @env_1_release of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_1_release(AgsAbyssSynthUtil *abyss_synth_util,
				       gdouble env_1_release)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_1_release = env_1_release;
}

/**
 * ags_abyss_synth_util_get_env_1_gain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-1 gain of @abyss_synth_util.
 * 
 * Returns: the env-1 gain
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_1_gain(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_1_gain);
}

/**
 * ags_abyss_synth_util_set_env_1_gain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_1_gain: the env-1 gain
 *
 * Set @env_1_gain of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_1_gain(AgsAbyssSynthUtil *abyss_synth_util,
				    gdouble env_1_gain)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_1_gain = env_1_gain;
}

/**
 * ags_abyss_synth_util_get_env_1_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-1 frequency of @abyss_synth_util.
 * 
 * Returns: the env-1 frequency
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_1_frequency(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_1_frequency);
}

/**
 * ags_abyss_synth_util_set_env_1_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_1_frequency: the env-1 frequency
 *
 * Set @env_1_frequency of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_1_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					 gdouble env_1_frequency)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_1_frequency = env_1_frequency;
  
  ags_envelope_util_set_frame_count(abyss_synth_util->env_1_util,
				    abyss_synth_util->samplerate / env_1_frequency);
}

/**
 * ags_abyss_synth_util_get_env_1_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_1_sends_count: (out): return location of env-1 sends count
 * 
 * Get env-1 sends of @abyss_synth_util.
 * 
 * Returns: (transfer none): the env-1 sends
 * 
 * Since: 9.4.0
 */
gint64*
ags_abyss_synth_util_get_env_1_sends(AgsAbyssSynthUtil *abyss_synth_util,
				     guint *env_1_sends_count)
{
  if(abyss_synth_util == NULL){
    return(NULL);
  }

  if(env_1_sends_count != NULL){
    env_1_sends_count[0] = AGS_ABYSS_SYNTH_SENDS_COUNT;
  }

  return(&(abyss_synth_util->env_1_sends[0]));
}

/**
 * ags_abyss_synth_util_set_env_1_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_1_sends: the env-1 sends
 * @env_1_sends_count: the env-1 sends count
 *
 * Set @env_1_sends of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_1_sends(AgsAbyssSynthUtil *abyss_synth_util,
				     gint64 *env_1_sends,
				     guint env_1_sends_count)
{
  if(abyss_synth_util == NULL){
    return;
  }

  memcpy(&(abyss_synth_util->env_1_sends[0]), env_1_sends, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
}

/**
 * ags_abyss_synth_util_get_env_2_attack:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-2 attack of @abyss_synth_util.
 * 
 * Returns: the env-2 attack
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_2_attack(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_2_attack);
}

/**
 * ags_abyss_synth_util_set_env_2_attack:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_2_attack: the env-2 attack
 *
 * Set @env_2_attack of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_2_attack(AgsAbyssSynthUtil *abyss_synth_util,
				      gdouble env_2_attack)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_2_attack = env_2_attack;
}

/**
 * ags_abyss_synth_util_get_env_2_decay:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-2 decay of @abyss_synth_util.
 * 
 * Returns: the env-2 decay
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_2_decay(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_2_decay);
}

/**
 * ags_abyss_synth_util_set_env_2_decay:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_2_decay: the env-2 decay
 *
 * Set @env_2_decay of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_2_decay(AgsAbyssSynthUtil *abyss_synth_util,
				     gdouble env_2_decay)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_2_decay = env_2_decay;
}

/**
 * ags_abyss_synth_util_get_env_2_sustain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-2 sustain of @abyss_synth_util.
 * 
 * Returns: the env-2 sustain
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_2_sustain(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_2_sustain);
}

/**
 * ags_abyss_synth_util_set_env_2_sustain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_2_sustain: the env-2 sustain
 *
 * Set @env_2_sustain of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_2_sustain(AgsAbyssSynthUtil *abyss_synth_util,
				       gdouble env_2_sustain)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_2_sustain = env_2_sustain;
}

/**
 * ags_abyss_synth_util_get_env_2_release:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-2 release of @abyss_synth_util.
 * 
 * Returns: the env-2 release
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_2_release(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_2_release);
}

/**
 * ags_abyss_synth_util_set_env_2_release:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_2_release: the env-2 release
 *
 * Set @env_2_release of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_2_release(AgsAbyssSynthUtil *abyss_synth_util,
				       gdouble env_2_release)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_2_release = env_2_release;
}

/**
 * ags_abyss_synth_util_get_env_2_gain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-2 gain of @abyss_synth_util.
 * 
 * Returns: the env-2 gain
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_2_gain(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_2_gain);
}

/**
 * ags_abyss_synth_util_set_env_2_gain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_2_gain: the env-2 gain
 *
 * Set @env_2_gain of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_2_gain(AgsAbyssSynthUtil *abyss_synth_util,
				    gdouble env_2_gain)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_2_gain = env_2_gain;
}

/**
 * ags_abyss_synth_util_get_env_2_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-2 frequency of @abyss_synth_util.
 * 
 * Returns: the env-2 frequency
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_2_frequency(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_2_frequency);
}

/**
 * ags_abyss_synth_util_set_env_2_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_2_frequency: the env-2 frequency
 *
 * Set @env_2_frequency of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_2_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					 gdouble env_2_frequency)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_2_frequency = env_2_frequency;

  ags_envelope_util_set_frame_count(abyss_synth_util->env_2_util,
				    abyss_synth_util->samplerate / env_2_frequency);
}

/**
 * ags_abyss_synth_util_get_env_2_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_2_sends_count: (out): return location of the env-2 sends count
 * 
 * Get env-2 sends of @abyss_synth_util.
 * 
 * Returns: (transfer none): the env-2 sends
 * 
 * Since: 9.4.0
 */
gint64*
ags_abyss_synth_util_get_env_2_sends(AgsAbyssSynthUtil *abyss_synth_util,
				     guint *env_2_sends_count)
{
  if(abyss_synth_util == NULL){
    return(NULL);
  }

  if(env_2_sends_count != NULL){
    env_2_sends_count[0] = AGS_ABYSS_SYNTH_SENDS_COUNT;
  }

  return(&(abyss_synth_util->env_2_sends[0]));
}

/**
 * ags_abyss_synth_util_set_env_2_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_2_sends: the env-2 sends
 * @env_2_sends_count: the env-2 sends count
 *
 * Set @env_2_sends of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_2_sends(AgsAbyssSynthUtil *abyss_synth_util,
				     gint64 *env_2_sends,
				     guint env_2_sends_count)
{
  if(abyss_synth_util == NULL){
    return;
  }

  memcpy(&(abyss_synth_util->env_2_sends[0]), env_2_sends, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
}

/**
 * ags_abyss_synth_util_get_env_3_attack:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-3 attack of @abyss_synth_util.
 * 
 * Returns: the env-3 attack
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_3_attack(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_3_attack);
}

/**
 * ags_abyss_synth_util_set_env_3_attack:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_3_attack: the env-3 attack
 *
 * Set @env_3_attack of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_3_attack(AgsAbyssSynthUtil *abyss_synth_util,
				      gdouble env_3_attack)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_3_attack = env_3_attack;
}

/**
 * ags_abyss_synth_util_get_env_3_decay:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-3 decay of @abyss_synth_util.
 * 
 * Returns: the env-3 decay
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_3_decay(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_3_decay);
}

/**
 * ags_abyss_synth_util_set_env_3_decay:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_3_decay: the env-3 decay
 *
 * Set @env_3_decay of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_3_decay(AgsAbyssSynthUtil *abyss_synth_util,
				     gdouble env_3_decay)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_3_decay = env_3_decay;
}

/**
 * ags_abyss_synth_util_get_env_3_sustain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-3 sustain of @abyss_synth_util.
 * 
 * Returns: the env-3 sustain
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_3_sustain(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_3_sustain);
}

/**
 * ags_abyss_synth_util_set_env_3_sustain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_3_sustain: the env-3 sustain
 *
 * Set @env_3_sustain of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_3_sustain(AgsAbyssSynthUtil *abyss_synth_util,
				       gdouble env_3_sustain)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_3_sustain = env_3_sustain;
}

/**
 * ags_abyss_synth_util_get_env_3_release:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-3 release of @abyss_synth_util.
 * 
 * Returns: the env-3 release
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_3_release(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_3_release);
}

/**
 * ags_abyss_synth_util_set_env_3_release:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_3_release: the env-3 release
 *
 * Set @env_3_release of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_3_release(AgsAbyssSynthUtil *abyss_synth_util,
				       gdouble env_3_release)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_3_release = env_3_release;
}

/**
 * ags_abyss_synth_util_get_env_3_gain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-3 gain of @abyss_synth_util.
 * 
 * Returns: the env-3 gain
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_3_gain(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_3_gain);
}

/**
 * ags_abyss_synth_util_set_env_3_gain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_3_gain: the env-3 gain
 *
 * Set @env_3_gain of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_3_gain(AgsAbyssSynthUtil *abyss_synth_util,
				    gdouble env_3_gain)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_3_gain = env_3_gain;
}

/**
 * ags_abyss_synth_util_get_env_3_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get env-3 frequency of @abyss_synth_util.
 * 
 * Returns: the env-3 frequency
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_env_3_frequency(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->env_3_frequency);
}

/**
 * ags_abyss_synth_util_set_env_3_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_3_frequency: the env-3 frequency
 *
 * Set @env_3_frequency of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_3_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					 gdouble env_3_frequency)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->env_3_frequency = env_3_frequency;

  ags_envelope_util_set_frame_count(abyss_synth_util->env_3_util,
				    abyss_synth_util->samplerate / env_3_frequency);
}

/**
 * ags_abyss_synth_util_get_env_3_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_3_sends_count: (out): return location of the env-3 sends count
 * 
 * Get env-3 sends of @abyss_synth_util.
 * 
 * Returns: (transfer none): the env-3 sends
 * 
 * Since: 9.4.0
 */
gint64*
ags_abyss_synth_util_get_env_3_sends(AgsAbyssSynthUtil *abyss_synth_util,
				     guint *env_3_sends_count)
{
  if(abyss_synth_util == NULL){
    return(NULL);
  }

  if(env_3_sends_count != NULL){
    env_3_sends_count[0] = AGS_ABYSS_SYNTH_SENDS_COUNT;
  }

  return(&(abyss_synth_util->env_3_sends[0]));
}

/**
 * ags_abyss_synth_util_set_env_3_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @env_3_sends: the env-3 sends
 * @env_3_sends_count: the env-3 sends count
 *
 * Set @env_3_sends of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_env_3_sends(AgsAbyssSynthUtil *abyss_synth_util,
				     gint64 *env_3_sends,
				     guint env_3_sends_count)
{
  if(abyss_synth_util == NULL){
    return;
  }

  memcpy(&(abyss_synth_util->env_3_sends[0]), env_3_sends, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
}

/**
 * ags_abyss_synth_util_get_lfo_0_oscillator:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get lfo-0 oscillator mode of @abyss_synth_util.
 * 
 * Returns: the lfo-0 oscillator mode as #AgsSynthOscillatorMode-enum
 * 
 * Since: 9.4.0
 */
AgsSynthOscillatorMode
ags_abyss_synth_util_get_lfo_0_oscillator(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(AGS_SYNTH_OSCILLATOR_SIN);
  }

  return(abyss_synth_util->lfo_0_oscillator);
}

/**
 * ags_abyss_synth_util_set_lfo_0_synth_oscillator:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_0_oscillator: the lfo-0 oscillator mode as #AgsSynthOscillatorMode-enum
 *
 * Set @lfo_0_oscillator of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_0_oscillator(AgsAbyssSynthUtil *abyss_synth_util,
					  AgsSynthOscillatorMode lfo_0_oscillator)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->lfo_0_oscillator = lfo_0_oscillator;
}

/**
 * ags_abyss_synth_util_get_lfo_0_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get lfo-0 frequency of @abyss_synth_util.
 * 
 * Returns: the lfo-0 frequency
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_lfo_0_frequency(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
  }

  return(abyss_synth_util->lfo_0_frequency);
}

/**
 * ags_abyss_synth_util_set_lfo_0_synth_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_0_frequency: the lfo-0 frequency
 *
 * Set @lfo_0_frequency of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_0_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					 gdouble lfo_0_frequency)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->lfo_0_frequency = lfo_0_frequency;

  ags_lfo_synth_util_set_frame_count(abyss_synth_util->lfo_0_util,
				     abyss_synth_util->samplerate / lfo_0_frequency);
}

/**
 * ags_abyss_synth_util_get_lfo_0_depth:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get lfo-0 depth of @abyss_synth_util.
 * 
 * Returns: the lfo-0 depth
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_lfo_0_depth(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->lfo_0_depth);
}

/**
 * ags_abyss_synth_util_set_lfo_0_synth_depth:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_0_depth: the lfo-0 depth
 *
 * Set @lfo_0_depth of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_0_depth(AgsAbyssSynthUtil *abyss_synth_util,
				     gdouble lfo_0_depth)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->lfo_0_depth = lfo_0_depth;
}

/**
 * ags_abyss_synth_util_get_lfo_0_tuning:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get lfo-0 tuning of @abyss_synth_util.
 * 
 * Returns: the lfo-0 tuning
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_lfo_0_tuning(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(0.0);
  }

  return(abyss_synth_util->lfo_0_tuning);
}

/**
 * ags_abyss_synth_util_set_lfo_0_synth_tuning:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_0_tuning: the lfo-0 tuning
 *
 * Set @lfo_0_tuning of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_0_tuning(AgsAbyssSynthUtil *abyss_synth_util,
				      gdouble lfo_0_tuning)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->lfo_0_tuning = lfo_0_tuning;
}

/**
 * ags_abyss_synth_util_get_lfo_0_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_0_sends_count: (out): the return location of lfo-0 sends count
 * 
 * Get lfo-0 sends of @abyss_synth_util.
 * 
 * Returns: (transfer none): the lfo-0 sends
 * 
 * Since: 9.4.0
 */
gint64*
ags_abyss_synth_util_get_lfo_0_sends(AgsAbyssSynthUtil *abyss_synth_util,
				     guint *lfo_0_sends_count)
{
  if(abyss_synth_util == NULL){
    return(NULL);
  }

  if(lfo_0_sends_count != NULL){
    lfo_0_sends_count[0] = AGS_ABYSS_SYNTH_SENDS_COUNT;
  }

  return(&(abyss_synth_util->lfo_0_sends[0]));
}

/**
 * ags_abyss_synth_util_set_lfo_0_synth_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_0_sends: the lfo-0 sends
 * @lfo_0_sends_count: the lfo-0 sends count
 *
 * Set @lfo_0_sends of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_0_sends(AgsAbyssSynthUtil *abyss_synth_util,
				     gint64 *lfo_0_sends,
				     guint lfo_0_sends_count)
{
  if(abyss_synth_util == NULL){
    return;
  }

  memcpy(&(abyss_synth_util->lfo_0_sends[0]), lfo_0_sends, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
}

/**
 * ags_abyss_synth_util_get_lfo_1_oscillator:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get lfo-1 oscillator mode of @abyss_synth_util.
 * 
 * Returns: the lfo-1 oscillator mode as #AgsSynthOscillatorMode-mode
 * 
 * Since: 9.4.0
 */
AgsSynthOscillatorMode
ags_abyss_synth_util_get_lfo_1_oscillator(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(AGS_SYNTH_OSCILLATOR_SIN);
  }

  return(abyss_synth_util->lfo_1_oscillator);
}

/**
 * ags_abyss_synth_util_set_lfo_1_synth_oscillator:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_1_oscillator: the lfo-1 oscillator mode
 *
 * Set @lfo_1_oscillator of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_1_oscillator(AgsAbyssSynthUtil *abyss_synth_util,
					  AgsSynthOscillatorMode lfo_1_oscillator)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->lfo_1_oscillator = lfo_1_oscillator;
}

/**
 * ags_abyss_synth_util_get_lfo_1_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get lfo-1 frequency of @abyss_synth_util.
 * 
 * Returns: the lfo-1 frequency
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_lfo_1_frequency(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
  }

  return(abyss_synth_util->lfo_1_frequency);
}

/**
 * ags_abyss_synth_util_set_lfo_1_synth_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_1_frequency: the lfo-1 frequency
 *
 * Set @lfo_1_frequency of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_1_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					 gdouble lfo_1_frequency)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->lfo_1_frequency = lfo_1_frequency;

  ags_lfo_synth_util_set_frame_count(abyss_synth_util->lfo_1_util,
				     abyss_synth_util->samplerate / lfo_1_frequency);
}

/**
 * ags_abyss_synth_util_get_lfo_1_depth:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get lfo-1 depth of @abyss_synth_util.
 * 
 * Returns: the lfo-1 depth
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_lfo_1_depth(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->lfo_1_depth);
}

/**
 * ags_abyss_synth_util_set_lfo_1_synth_depth:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_1_depth: the lfo-1 depth
 *
 * Set @lfo_1_depth of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_1_depth(AgsAbyssSynthUtil *abyss_synth_util,
				     gdouble lfo_1_depth)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->lfo_1_depth = lfo_1_depth;
}

/**
 * ags_abyss_synth_util_get_lfo_1_tuning:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get lfo-1 tuning of @abyss_synth_util.
 * 
 * Returns: the lfo-1 tuning
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_lfo_1_tuning(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(0.0);
  }

  return(abyss_synth_util->lfo_1_tuning);
}

/**
 * ags_abyss_synth_util_set_lfo_1_synth_tuning:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_1_tuning: the lfo-1 tuning
 *
 * Set @lfo_1_tuning of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_1_tuning(AgsAbyssSynthUtil *abyss_synth_util,
				      gdouble lfo_1_tuning)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->lfo_1_tuning = lfo_1_tuning;
}

/**
 * ags_abyss_synth_util_get_lfo_1_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_1_sends_count: (out): the return location lfo-1 sends count
 * 
 * Get lfo-1 sends of @abyss_synth_util.
 * 
 * Returns: (transfer none): the lfo-1 sends
 * 
 * Since: 9.4.0
 */
gint64*
ags_abyss_synth_util_get_lfo_1_sends(AgsAbyssSynthUtil *abyss_synth_util,
				     guint *lfo_1_sends_count)
{
  if(abyss_synth_util == NULL){
    return(NULL);
  }

  if(lfo_1_sends_count != NULL){
    lfo_1_sends_count[0] = AGS_ABYSS_SYNTH_SENDS_COUNT;
  }

  return(&(abyss_synth_util->lfo_1_sends[0]));
}

/**
 * ags_abyss_synth_util_set_lfo_1_synth_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_1_sends: the lfo-1 sends
 * @lfo_1_sends_count: the lfo-1 sends count
 *
 * Set @lfo_1_sends of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_1_sends(AgsAbyssSynthUtil *abyss_synth_util,
				     gint64 *lfo_1_sends,
				     guint lfo_1_sends_count)
{
  if(abyss_synth_util == NULL){
    return;
  }

  memcpy(&(abyss_synth_util->lfo_1_sends[0]), lfo_1_sends, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
}

/**
 * ags_abyss_synth_util_get_lfo_2_oscillator:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get lfo-2 oscillator mode of @abyss_synth_util.
 * 
 * Returns: the lfo-2 oscillator mode as #AgsSynthOscillatorMode-enum
 * 
 * Since: 9.4.0
 */
AgsSynthOscillatorMode
ags_abyss_synth_util_get_lfo_2_oscillator(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(AGS_SYNTH_OSCILLATOR_SIN);
  }

  return(abyss_synth_util->lfo_2_oscillator);
}

/**
 * ags_abyss_synth_util_set_lfo_2_synth_oscillator:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_2_oscillator: the lfo-2 oscillator mode as #AgsSynthOscillatorMode-enum
 *
 * Set @lfo_2_oscillator of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_2_oscillator(AgsAbyssSynthUtil *abyss_synth_util,
					  AgsSynthOscillatorMode lfo_2_oscillator)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->lfo_2_oscillator = lfo_2_oscillator;
}

/**
 * ags_abyss_synth_util_get_lfo_2_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get lfo-2 frequency of @abyss_synth_util.
 * 
 * Returns: the lfo-2 frequency
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_lfo_2_frequency(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
  }

  return(abyss_synth_util->lfo_2_frequency);
}

/**
 * ags_abyss_synth_util_set_lfo_2_synth_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_2_frequency: the lfo-2 frequency
 *
 * Set @lfo_2_frequency of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_2_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					 gdouble lfo_2_frequency)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->lfo_2_frequency = lfo_2_frequency;

  ags_lfo_synth_util_set_frame_count(abyss_synth_util->lfo_2_util,
				     abyss_synth_util->samplerate / lfo_2_frequency);
}

/**
 * ags_abyss_synth_util_get_lfo_2_depth:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get lfo-2 depth of @abyss_synth_util.
 * 
 * Returns: the lfo-2 depth
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_lfo_2_depth(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->lfo_2_depth);
}

/**
 * ags_abyss_synth_util_set_lfo_2_synth_depth:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_2_depth: the lfo-2 depth
 *
 * Set @lfo_2_depth of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_2_depth(AgsAbyssSynthUtil *abyss_synth_util,
				     gdouble lfo_2_depth)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->lfo_2_depth = lfo_2_depth;
}

/**
 * ags_abyss_synth_util_get_lfo_2_tuning:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get lfo-2 tuning of @abyss_synth_util.
 * 
 * Returns: the lfo-2 tuning
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_lfo_2_tuning(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(0.0);
  }

  return(abyss_synth_util->lfo_2_tuning);
}

/**
 * ags_abyss_synth_util_set_lfo_2_synth_tuning:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_2_tuning: the lfo-2 tuning
 *
 * Set @lfo_2_tuning of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_2_tuning(AgsAbyssSynthUtil *abyss_synth_util,
				      gdouble lfo_2_tuning)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->lfo_2_tuning = lfo_2_tuning;
}

/**
 * ags_abyss_synth_util_get_lfo_2_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_2_sends_count: (out): the return location of lfo-2 sends count
 * 
 * Get lfo-2 sends of @abyss_synth_util.
 * 
 * Returns: (transfer none): the lfo-2 sends
 * 
 * Since: 9.4.0
 */
gint64*
ags_abyss_synth_util_get_lfo_2_sends(AgsAbyssSynthUtil *abyss_synth_util,
				     guint *lfo_2_sends_count)
{
  if(abyss_synth_util == NULL){
    return(NULL);
  }

  if(lfo_2_sends_count != NULL){
    lfo_2_sends_count[0] = AGS_ABYSS_SYNTH_SENDS_COUNT;
  }

  return(&(abyss_synth_util->lfo_2_sends[0]));
}

/**
 * ags_abyss_synth_util_set_lfo_2_synth_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_2_sends: the lfo-2 sends
 * @lfo_2_sends_count: the lfo-2 sends count
 *
 * Set @lfo_2_sends of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_2_sends(AgsAbyssSynthUtil *abyss_synth_util,
				     gint64 *lfo_2_sends,
				     guint lfo_2_sends_count)
{
  if(abyss_synth_util == NULL){
    return;
  }

  memcpy(&(abyss_synth_util->lfo_2_sends[0]), lfo_2_sends, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
}

/**
 * ags_abyss_synth_util_get_lfo_3_oscillator:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get lfo-3 oscillator mode of @abyss_synth_util.
 * 
 * Returns: the lfo-3 oscillator mode as #AgsSynthOscillatorMode-enum
 * 
 * Since: 9.4.0
 */
AgsSynthOscillatorMode
ags_abyss_synth_util_get_lfo_3_oscillator(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(AGS_SYNTH_OSCILLATOR_SIN);
  }

  return(abyss_synth_util->lfo_3_oscillator);
}

/**
 * ags_abyss_synth_util_set_lfo_3_synth_oscillator:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_3_oscillator: the lfo-3 oscillator mode as #AgsSynthOscillatorMode-enum
 *
 * Set @lfo_3_oscillator of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_3_oscillator(AgsAbyssSynthUtil *abyss_synth_util,
					  AgsSynthOscillatorMode lfo_3_oscillator)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->lfo_3_oscillator = lfo_3_oscillator;
}

/**
 * ags_abyss_synth_util_get_lfo_3_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get lfo-3 frequency of @abyss_synth_util.
 * 
 * Returns: the lfo-3 frequency
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_lfo_3_frequency(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
  }

  return(abyss_synth_util->lfo_3_frequency);
}

/**
 * ags_abyss_synth_util_set_lfo_3_synth_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_3_frequency: the lfo-3 frequency
 *
 * Set @lfo_3_frequency of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_3_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					 gdouble lfo_3_frequency)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->lfo_3_frequency = lfo_3_frequency;

  ags_lfo_synth_util_set_frame_count(abyss_synth_util->lfo_3_util,
				     abyss_synth_util->samplerate / lfo_3_frequency);
}

/**
 * ags_abyss_synth_util_get_lfo_3_depth:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get lfo-3 depth of @abyss_synth_util.
 * 
 * Returns: the lfo-3 depth
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_lfo_3_depth(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->lfo_3_depth);
}

/**
 * ags_abyss_synth_util_set_lfo_3_synth_depth:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_3_depth: the lfo-3 depth
 *
 * Set @lfo_3_depth of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_3_depth(AgsAbyssSynthUtil *abyss_synth_util,
				     gdouble lfo_3_depth)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->lfo_3_depth = lfo_3_depth;
}

/**
 * ags_abyss_synth_util_get_lfo_3_tuning:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get lfo-3 tuning of @abyss_synth_util.
 * 
 * Returns: the lfo-3 tuning
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_lfo_3_tuning(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(0.0);
  }

  return(abyss_synth_util->lfo_3_tuning);
}

/**
 * ags_abyss_synth_util_set_lfo_3_synth_tuning:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_3_tuning: the lfo-3 tuning
 *
 * Set @lfo_3_tuning of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_3_tuning(AgsAbyssSynthUtil *abyss_synth_util,
				      gdouble lfo_3_tuning)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->lfo_3_tuning = lfo_3_tuning;
}

/**
 * ags_abyss_synth_util_get_lfo_3_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_3_sends_count: (out): the return location of lfo-3 sends count
 * 
 * Get lfo-3 sends of @abyss_synth_util.
 * 
 * Returns: (transfer none): the lfo-3 sends
 * 
 * Since: 9.4.0
 */
gint64*
ags_abyss_synth_util_get_lfo_3_sends(AgsAbyssSynthUtil *abyss_synth_util,
				     guint *lfo_3_sends_count)
{
  if(abyss_synth_util == NULL){
    return(NULL);
  }

  if(lfo_3_sends_count != NULL){
    lfo_3_sends_count[0] = AGS_ABYSS_SYNTH_SENDS_COUNT;
  }

  return(&(abyss_synth_util->lfo_3_sends[0]));
}

/**
 * ags_abyss_synth_util_set_lfo_3_synth_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @lfo_3_sends: the lfo-3 sends
 * @lfo_3_sends_count: the lfo-3 sends count
 *
 * Set @lfo_3_sends of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_lfo_3_sends(AgsAbyssSynthUtil *abyss_synth_util,
				     gint64 *lfo_3_sends,
				     guint lfo_3_sends_count)
{
  if(abyss_synth_util == NULL){
    return;
  }

  memcpy(&(abyss_synth_util->lfo_3_sends[0]), lfo_3_sends, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
}

/**
 * ags_abyss_synth_util_get_noise_0_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get noise frequency of @abyss_synth_util.
 * 
 * Returns: the noise frequency
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_noise_0_frequency(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(440.0);
  }

  return(abyss_synth_util->noise_0_frequency);
}

/**
 * ags_abyss_synth_util_set_noise_0_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @noise_frequency: the noise frequency
 *
 * Set @noise_frequency of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_noise_0_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					   gdouble noise_frequency)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->noise_0_frequency = noise_frequency;
}

/**
 * ags_abyss_synth_util_get_noise_0_gain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get noise gain of @abyss_synth_util.
 * 
 * Returns: the noise gain
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_noise_0_gain(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->noise_0_gain);
}

/**
 * ags_abyss_synth_util_set_noise_0_gain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @noise_gain: the noise gain
 *
 * Set @noise_gain of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_noise_0_gain(AgsAbyssSynthUtil *abyss_synth_util,
				      gdouble noise_gain)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->noise_0_gain = noise_gain;
}

/**
 * ags_abyss_synth_util_get_noise_0_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @noise_sends_count: (out): return location of noise sends count
 * 
 * Get noise sends of @abyss_synth_util.
 * 
 * Returns: (transfer none): the noise sends
 * 
 * Since: 9.4.0
 */
gint64*
ags_abyss_synth_util_get_noise_0_sends(AgsAbyssSynthUtil *abyss_synth_util,
				       guint *noise_sends_count)
{
  if(abyss_synth_util == NULL){
    return(NULL);
  }

  if(noise_sends_count != NULL){
    noise_sends_count[0] = AGS_ABYSS_SYNTH_SENDS_COUNT;
  }

  return(&(abyss_synth_util->noise_0_sends[0]));
}

/**
 * ags_abyss_synth_util_set_noise_0_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @noise_sends: the noise sends
 * @noise_sends_count: the noise sends count
 *
 * Set @noise_sends of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_noise_0_sends(AgsAbyssSynthUtil *abyss_synth_util,
				       gint64 *noise_sends,
				       guint noise_sends_count)
{
  if(abyss_synth_util == NULL){
    return;
  }
  
  memcpy(&(abyss_synth_util->noise_0_sends[0]), noise_sends, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
}

/**
 * ags_abyss_synth_util_get_noise_1_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get noise frequency of @abyss_synth_util.
 * 
 * Returns: the noise frequency
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_noise_1_frequency(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(440.0);
  }

  return(abyss_synth_util->noise_1_frequency);
}

/**
 * ags_abyss_synth_util_set_noise_1_frequency:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @noise_frequency: the noise frequency
 *
 * Set @noise_frequency of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_noise_1_frequency(AgsAbyssSynthUtil *abyss_synth_util,
					   gdouble noise_frequency)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->noise_1_frequency = noise_frequency;
}

/**
 * ags_abyss_synth_util_get_noise_1_gain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get noise gain of @abyss_synth_util.
 * 
 * Returns: the noise gain
 * 
 * Since: 9.4.0
 */
gdouble
ags_abyss_synth_util_get_noise_1_gain(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(1.0);
  }

  return(abyss_synth_util->noise_1_gain);
}

/**
 * ags_abyss_synth_util_set_noise_1_gain:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @noise_gain: the noise gain
 *
 * Set @noise_gain of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_noise_1_gain(AgsAbyssSynthUtil *abyss_synth_util,
				      gdouble noise_gain)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->noise_1_gain = noise_gain;
}

/**
 * ags_abyss_synth_util_get_noise_1_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @noise_sends_count: (out): return location of noise sends count
 * 
 * Get noise sends of @abyss_synth_util.
 * 
 * Returns: (transfer none): the noise sends
 * 
 * Since: 9.4.0
 */
gint64*
ags_abyss_synth_util_get_noise_1_sends(AgsAbyssSynthUtil *abyss_synth_util,
				       guint *noise_sends_count)
{
  if(abyss_synth_util == NULL){
    return(NULL);
  }

  if(noise_sends_count != NULL){
    noise_sends_count[0] = AGS_ABYSS_SYNTH_SENDS_COUNT;
  }

  return(&(abyss_synth_util->noise_1_sends[0]));
}

/**
 * ags_abyss_synth_util_set_noise_1_sends:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @noise_sends: the noise sends
 * @noise_sends_count: the noise sends count
 *
 * Set @noise_sends of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_noise_1_sends(AgsAbyssSynthUtil *abyss_synth_util,
				       gint64 *noise_sends,
				       guint noise_sends_count)
{
  if(abyss_synth_util == NULL){
    return;
  }
  
  memcpy(&(abyss_synth_util->noise_1_sends[0]), noise_sends, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
}

/**
 * ags_abyss_synth_util_get_frame_count:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get frame count of @abyss_synth_util.
 * 
 * Returns: the frame count
 * 
 * Since: 9.4.0
 */
guint
ags_abyss_synth_util_get_frame_count(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(0);
  }

  return(abyss_synth_util->frame_count);
}

/**
 * ags_abyss_synth_util_set_frame_count:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @frame_count: the frame count
 *
 * Set @frame_count of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_frame_count(AgsAbyssSynthUtil *abyss_synth_util,
				     guint frame_count)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->frame_count = frame_count;
}

/**
 * ags_abyss_synth_util_get_offset:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get offset of @abyss_synth_util.
 * 
 * Returns: the offset
 * 
 * Since: 9.4.0
 */
guint
ags_abyss_synth_util_get_offset(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(0);
  }

  return(abyss_synth_util->offset);
}

/**
 * ags_abyss_synth_util_set_offset:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @offset: the offset
 *
 * Set @offset of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_offset(AgsAbyssSynthUtil *abyss_synth_util,
				guint offset)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->offset = offset;
}

/**
 * ags_abyss_synth_util_get_note_256th_mode:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get note 256th mode of @abyss_synth_util.
 * 
 * Returns: %TRUE if note 256th mode, otherwise %FALSE
 * 
 * Since: 9.4.0
 */
gboolean
ags_abyss_synth_util_get_note_256th_mode(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(0);
  }

  return(abyss_synth_util->note_256th_mode);
}

/**
 * ags_abyss_synth_util_set_note_256th_mode:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @note_256th_mode: the note 256th mode
 *
 * Set @note_256th_mode of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_note_256th_mode(AgsAbyssSynthUtil *abyss_synth_util,
					 gboolean note_256th_mode)
{
  if(abyss_synth_util == NULL){
    return;
  }

  abyss_synth_util->note_256th_mode = note_256th_mode;
}

/**
 * ags_abyss_synth_util_get_offset_256th:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * 
 * Get offset as note 256th of @abyss_synth_util.
 * 
 * Returns: the offset as note 256th
 * 
 * Since: 9.4.0
 */
guint
ags_abyss_synth_util_get_offset_256th(AgsAbyssSynthUtil *abyss_synth_util)
{
  if(abyss_synth_util == NULL){
    return(0);
  }

  return(abyss_synth_util->offset_256th);
}

/**
 * ags_abyss_synth_util_set_offset_256th:
 * @abyss_synth_util: the #AgsAbyssSynthUtil-struct
 * @offset_256th: the offset as note 256th
 *
 * Set @offset_256th of @abyss_synth_util.
 *
 * Since: 9.4.0
 */
void
ags_abyss_synth_util_set_offset_256th(AgsAbyssSynthUtil *abyss_synth_util,
				      guint offset_256th)
{
  if(abyss_synth_util == NULL){
    return;
  }
  
  abyss_synth_util->offset_256th = offset_256th;
}
