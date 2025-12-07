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

#include <ags/audio/ags_modular_synth_util.h>

#include <ags/audio/ags_audio_buffer_util.h>

#if defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#include <Accelerate/Accelerate.h>
#endif

#include <math.h>
#include <complex.h>

/**
 * SECTION:ags_modular_synth_util
 * @short_description: frequency modulation synth util
 * @title: AgsModularSynthUtil
 * @section_id:
 * @include: ags/audio/ags_modular_synth_util.h
 *
 * Utility functions to compute modular synths.
 */

GType
ags_modular_synth_util_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_modular_synth_util = 0;

    ags_type_modular_synth_util =
      g_boxed_type_register_static("AgsModularSynthUtil",
				   (GBoxedCopyFunc) ags_modular_synth_util_copy,
				   (GBoxedFreeFunc) ags_modular_synth_util_free);

    g_once_init_leave(&g_define_type_id__static, ags_type_modular_synth_util);
  }

  return(g_define_type_id__static);
}

/**
 * ags_modular_synth_util_alloc:
 *
 * Allocate #AgsModularSynthUtil-struct
 *
 * Returns: a new #AgsModularSynthUtil-struct
 *
 * Since: 8.2.0
 */
AgsModularSynthUtil*
ags_modular_synth_util_alloc()
{
  AgsModularSynthUtil *ptr;

  ptr = (AgsModularSynthUtil *) g_new(AgsModularSynthUtil,
				      1);

  ptr[0] = AGS_MODULAR_SYNTH_UTIL_INITIALIZER;

  return(ptr);
}

/**
 * ags_modular_synth_util_copy:
 * @ptr: the original #AgsModularSynthUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsModularSynthUtil-struct
 *
 * Since: 8.2.0
 */
gpointer
ags_modular_synth_util_copy(AgsModularSynthUtil *ptr)
{
  AgsModularSynthUtil *new_ptr;

  guint i;
  
  g_return_val_if_fail(ptr != NULL, NULL);

  new_ptr = (AgsModularSynthUtil *) g_new(AgsModularSynthUtil,
					  1);
  
  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;

  new_ptr->osc_0_oscillator = ptr->osc_0_oscillator;

  new_ptr->osc_0_frequency = ptr->osc_0_frequency;
  new_ptr->osc_0_phase = ptr->osc_0_phase;
  new_ptr->osc_0_volume = ptr->osc_0_volume;

  new_ptr->osc_1_oscillator = ptr->osc_1_oscillator;

  new_ptr->osc_1_frequency = ptr->osc_1_frequency;
  new_ptr->osc_1_phase = ptr->osc_1_phase;
  new_ptr->osc_1_volume = ptr->osc_1_volume;

  new_ptr->env_0_attack = ptr->env_0_attack;
  new_ptr->env_0_decay = ptr->env_0_decay;
  new_ptr->env_0_sustain = ptr->env_0_sustain;
  new_ptr->env_0_release = ptr->env_0_release;
  new_ptr->env_0_gain = ptr->env_0_gain;
  new_ptr->env_0_sends = ptr->env_0_sends;

  new_ptr->env_1_attack = ptr->env_1_attack;
  new_ptr->env_1_decay = ptr->env_1_decay;
  new_ptr->env_1_sustain = ptr->env_1_sustain;
  new_ptr->env_1_release = ptr->env_1_release;
  new_ptr->env_1_gain = ptr->env_1_gain;
  new_ptr->env_1_sends = ptr->env_1_sends;
  
  new_ptr->noise_frequency = ptr->noise_frequency;
  new_ptr->noise_gain = ptr->noise_gain;
  new_ptr->noise_sends = ptr->noise_sends;

  new_ptr->lfo_0_oscillator = ptr->lfo_0_oscillator;
  new_ptr->lfo_0_frequency = ptr->lfo_0_frequency;
  new_ptr->lfo_0_depth = new_ptr->lfo_0_depth;
  new_ptr->lfo_0_tuning = new_ptr->lfo_0_tuning;
  new_ptr->lfo_0_sends = new_ptr->lfo_0_sends;

  new_ptr->lfo_1_oscillator = ptr->lfo_1_oscillator;
  new_ptr->lfo_1_frequency = ptr->lfo_1_frequency;
  new_ptr->lfo_1_depth = new_ptr->lfo_1_depth;
  new_ptr->lfo_1_tuning = new_ptr->lfo_1_tuning;
  new_ptr->lfo_1_sends = new_ptr->lfo_1_sends;

  new_ptr->frame_count = ptr->frame_count;
  new_ptr->offset = ptr->offset;

  new_ptr->note_256th_mode = ptr->note_256th_mode;

  new_ptr->offset_256th = ptr->offset_256th;
  
  return(new_ptr);
}

/**
 * ags_modular_synth_util_free:
 * @ptr: the #AgsModularSynthUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_free(AgsModularSynthUtil *ptr)
{
  g_return_if_fail(ptr != NULL);
  
  g_free(ptr);
}
/**
 * ags_modular_synth_util_get_source:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get source buffer of @modular_synth_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 8.2.0
 */
gpointer
ags_modular_synth_util_get_source(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(NULL);
  }

  return(modular_synth_util->source);
}

/**
 * ags_modular_synth_util_set_source:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @source: (transfer none): the source buffer
 *
 * Set @source buffer of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_source(AgsModularSynthUtil *modular_synth_util,
				  gpointer source)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->source = source;
}

/**
 * ags_modular_synth_util_get_source_stride:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get source stride of @modular_synth_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 8.2.0
 */
guint
ags_modular_synth_util_get_source_stride(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(0);
  }

  return(modular_synth_util->source_stride);
}

/**
 * ags_modular_synth_util_set_source_stride:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_source_stride(AgsModularSynthUtil *modular_synth_util,
					 guint source_stride)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->source_stride = source_stride;
}

/**
 * ags_modular_synth_util_get_buffer_length:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get buffer length of @modular_synth_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 8.2.0
 */
guint
ags_modular_synth_util_get_buffer_length(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(0);
  }

  return(modular_synth_util->buffer_length);
}

/**
 * ags_modular_synth_util_set_buffer_length:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_buffer_length(AgsModularSynthUtil *modular_synth_util,
					 guint buffer_length)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->buffer_length = buffer_length;

  ags_common_pitch_util_set_buffer_length(modular_synth_util->pitch_util,
					  modular_synth_util->pitch_type,
					  buffer_length);

  ags_noise_util_set_buffer_length(modular_synth_util->noise_util,
				   buffer_length);

  /* pitch buffer */
  ags_stream_free(modular_synth_util->pitch_buffer);
  
  modular_synth_util->pitch_buffer = NULL;

  if(buffer_length > 0){
    modular_synth_util->pitch_buffer = ags_stream_alloc(buffer_length,
							format);
  }
}

/**
 * ags_modular_synth_util_get_format:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get audio buffer util format of @modular_synth_util.
 * 
 * Returns: the audio buffer util format
 * 
 * Since: 8.2.0
 */
AgsSoundcardFormat
ags_modular_synth_util_get_format(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(0);
  }

  return(modular_synth_util->format);
}

/**
 * ags_modular_synth_util_set_format:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @format: the audio buffer util format
 *
 * Set @format of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_format(AgsModularSynthUtil *modular_synth_util,
				  AgsSoundcardFormat format)
{
  guint buffer_length;

  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->format = format;

  buffer_length = modular_synth_util->buffer_length;

  /*  */
  ags_common_pitch_util_set_format(modular_synth_util->pitch_util,
				   modular_synth_util->pitch_type,
				   format);

  ags_noise_util_set_format(modular_synth_util->noise_util,
			    format);

  /* pitch buffer */
  ags_stream_free(modular_synth_util->pitch_buffer);

  modular_synth_util->pitch_buffer = NULL;

  if(buffer_length > 0){
    modular_synth_util->pitch_buffer = ags_stream_alloc(buffer_length,
							format);
  }
}

/**
 * ags_modular_synth_util_get_samplerate:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get samplerate of @modular_synth_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 8.2.0
 */
guint
ags_modular_synth_util_get_samplerate(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(0);
  }

  return(modular_synth_util->samplerate);
}

/**
 * ags_modular_synth_util_set_samplerate:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_samplerate(AgsModularSynthUtil *modular_synth_util,
				      guint samplerate)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->samplerate = samplerate;

  ags_common_pitch_util_set_samplerate(modular_synth_util->pitch_util,
				       modular_synth_util->pitch_type,
				       samplerate);

  ags_noise_util_set_samplerate(modular_synth_util->noise_util,
				samplerate);
}

/**
 * ags_modular_synth_util_get_osc_0_oscillator:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get osc-0 oscillator mode of @modular_synth_util.
 * 
 * Returns: the osc-0 oscillator mode as #AgsSynthOscillatorMode-enum
 * 
 * Since: 8.2.0
 */
AgsSynthOscillatorMode
ags_modular_synth_util_get_osc_0_oscillator(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(AGS_SYNTH_OSCILLATOR_SIN);
  }

  return(modular_synth_util->osc_0_oscillator);
}

/**
 * ags_modular_synth_util_set_osc_0_oscillator:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @osc_0_oscillator: the osc-0 oscillator mode as #AgsSynthOscillatorMode-enum
 *
 * Set @osc_0_oscillator of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_osc_0_oscillator(AgsModularSynthUtil *modular_synth_util,
					    AgsSynthOscillatorMode osc_0_oscillator)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->osc_0_oscillator = osc_0_oscillator;
}

/**
 * ags_modular_synth_util_get_osc_0_frequency:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get osc-0 frequency of @modular_synth_util.
 * 
 * Returns: the osc-0 frequency
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_osc_0_frequency(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(440.0);
  }

  return(modular_synth_util->osc_0_frequency);
}

/**
 * ags_modular_synth_util_set_osc_0_frequency:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @osc_0_frequency: the osc-0 frequency
 *
 * Set @osc_0_frequency of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_osc_0_frequency(AgsModularSynthUtil *modular_synth_util,
					   gdouble osc_0_frequency)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->osc_0_frequency = osc_0_frequency;
}

/**
 * ags_modular_synth_util_get_osc_0_phase:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get osc-0 phase of @modular_synth_util.
 * 
 * Returns: the osc-0 phase
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_osc_0_phase(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->osc_0_phase);
}

/**
 * ags_modular_synth_util_set_osc_0_phase:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @osc_0_phase: the osc-0 phase
 *
 * Set @osc_0_phase of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_osc_0_phase(AgsModularSynthUtil *modular_synth_util,
				       gdouble osc_0_phase)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->osc_0_phase = osc_0_phase;
}

/**
 * ags_modular_synth_util_get_osc_0_volume:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get osc-0 volume of @modular_synth_util.
 * 
 * Returns: the osc-0 volume
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_osc_0_volume(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->osc_0_volume);
}

/**
 * ags_modular_synth_util_set_osc_0_volume:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @osc_0_volume: the osc-0 volume
 *
 * Set @osc_0_volume of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_osc_0_volume(AgsModularSynthUtil *modular_synth_util,
					gdouble osc_0_volume)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->osc_0_volume = osc_0_volume;
}

/**
 * ags_modular_synth_util_get_osc_1_oscillator:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get osc-1 oscillator mode of @modular_synth_util.
 * 
 * Returns: the osc-1 oscillator mode as #AgsSynthOscillatorMode-enum
 * 
 * Since: 8.2.0
 */
AgsSynthOscillatorMode
ags_modular_synth_util_get_osc_1_oscillator(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(AGS_SYNTH_OSCILLATOR_SIN);
  }

  return(modular_synth_util->osc_1_oscillator);
}

/**
 * ags_modular_synth_util_set_osc_1_oscillator:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @osc_1_oscillator: the osc-1 oscillator mode as #AgsSynthOscillatorMode-enum
 *
 * Set @osc_1_oscillator of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_osc_1_oscillator(AgsModularSynthUtil *modular_synth_util,
					    AgsSynthOscillatorMode osc_1_oscillator)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->osc_1_oscillator = osc_1_oscillator;
}

/**
 * ags_modular_synth_util_get_osc_1_frequency:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get osc-1 frequency of @modular_synth_util.
 * 
 * Returns: the osc-1 frequency
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_osc_1_frequency(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(440.0);
  }

  return(modular_synth_util->osc_1_frequency);
}

/**
 * ags_modular_synth_util_set_osc_1_frequency:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @osc_1_frequency: the osc-1 frequency
 *
 * Set @osc_1_frequency of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_osc_1_frequency(AgsModularSynthUtil *modular_synth_util,
					   gdouble osc_1_frequency)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->osc_1_frequency = osc_1_frequency;
}

/**
 * ags_modular_synth_util_get_osc_1_phase:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get osc-1 phase of @modular_synth_util.
 * 
 * Returns: the osc-1 phase
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_osc_1_phase(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->osc_1_phase);
}

/**
 * ags_modular_synth_util_set_osc_1_phase:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @osc_1_phase: the osc-1 phase
 *
 * Set @osc_1_phase of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_osc_1_phase(AgsModularSynthUtil *modular_synth_util,
				       gdouble osc_1_phase)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->osc_1_phase = osc_1_phase;
}

/**
 * ags_modular_synth_util_get_osc_1_volume:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get osc-1 volume of @modular_synth_util.
 * 
 * Returns: the osc-1 volume
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_osc_1_volume(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->osc_1_volume);
}

/**
 * ags_modular_synth_util_set_osc_1_volume:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @osc_1_volume: the osc-1 volume
 *
 * Set @osc_1_volume of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_osc_1_volume(AgsModularSynthUtil *modular_synth_util,
					gdouble osc_1_volume)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->osc_1_volume = osc_1_volume;
}

/**
 * ags_modular_synth_util_get_pitch_type:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get pitch_type of @modular_synth_util.
 * 
 * Returns: the pitch_type
 * 
 * Since: 8.2.0
 */
GType
ags_modular_synth_util_get_pitch_type(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL);
  }

  return(modular_synth_util->pitch_type);
}

/**
 * ags_modular_synth_util_set_pitch_type:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @pitch_type: the pitch_type
 *
 * Set @pitch_type of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_pitch_type(AgsModularSynthUtil *modular_synth_util,
				      GType pitch_type)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->pitch_type = pitch_type;
}

/**
 * ags_modular_synth_util_get_pitch_base_key:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get pitch base key of @modular_synth_util.
 * 
 * Returns: the pitch base key
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_pitch_base_key(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(440.0);
  }

  return(modular_synth_util->pitch_base_key);
}

/**
 * ags_modular_synth_util_set_pitch_base_key:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @pitch_base_key: the pitch base key
 *
 * Set @pitch_base_key of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_pitch_base_key(AgsModularSynthUtil *modular_synth_util,
					  gdouble pitch_base_key)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->pitch_base_key = pitch_base_key;
}

/**
 * ags_modular_synth_util_get_pitch_tuning:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get pitch tuning of @modular_synth_util.
 * 
 * Returns: the pitch tuning
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_pitch_tuning(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(0.0);
  }

  return(modular_synth_util->pitch_tuning);
}

/**
 * ags_modular_synth_util_set_pitch_tuning:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @pitch_tuning: the pitch tuning
 *
 * Set @pitch_tuning of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_pitch_tuning(AgsModularSynthUtil *modular_synth_util,
					gdouble pitch_tuning)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->pitch_tuning = pitch_tuning;
}

/**
 * ags_modular_synth_util_get_env_0_attack:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get env-0 attack of @modular_synth_util.
 * 
 * Returns: the env-0 attack
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_env_0_attack(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->env_0_attack);
}

/**
 * ags_modular_synth_util_set_env_0_attack:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @env_0_attack: the env-0 attack
 *
 * Set @env_0_attack of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_env_0_attack(AgsModularSynthUtil *modular_synth_util,
					gdouble env_0_attack)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->env_0_attack = env_0_attack;
}

/**
 * ags_modular_synth_util_get_env_0_decay:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get env-0 decay of @modular_synth_util.
 * 
 * Returns: the env-0 decay
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_env_0_decay(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->env_0_decay);
}

/**
 * ags_modular_synth_util_set_env_0_decay:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @env_0_decay: the env-0 decay
 *
 * Set @env_0_decay of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_env_0_decay(AgsModularSynthUtil *modular_synth_util,
				       gdouble env_0_decay)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->env_0_decay = env_0_decay;
}

/**
 * ags_modular_synth_util_get_env_0_sustain:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get env-0 sustain of @modular_synth_util.
 * 
 * Returns: the env-0 sustain
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_env_0_sustain(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->env_0_sustain);
}

/**
 * ags_modular_synth_util_set_env_0_sustain:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @env_0_sustain: the env-0 sustain
 *
 * Set @env_0_sustain of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_env_0_sustain(AgsModularSynthUtil *modular_synth_util,
					 gdouble env_0_sustain)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->env_0_sustain = env_0_sustain;
}

/**
 * ags_modular_synth_util_get_env_0_release:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get env-0 release of @modular_synth_util.
 * 
 * Returns: the env-0 release
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_env_0_release(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->env_0_release);
}

/**
 * ags_modular_synth_util_set_env_0_release:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @env_0_release: the env-0 release
 *
 * Set @env_0_release of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_env_0_release(AgsModularSynthUtil *modular_synth_util,
					 gdouble env_0_release)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->env_0_release = env_0_release;
}

/**
 * ags_modular_synth_util_get_env_0_gain:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get env-0 gain of @modular_synth_util.
 * 
 * Returns: the env-0 gain
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_env_0_gain(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->env_0_gain);
}

/**
 * ags_modular_synth_util_set_env_0_gain:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @env_0_gain: the env-0 gain
 *
 * Set @env_0_gain of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_env_0_gain(AgsModularSynthUtil *modular_synth_util,
				      gdouble env_0_gain)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->env_0_gain = env_0_gain;
}

/**
 * ags_modular_synth_util_get_env_0_sends:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get env-0 sends of @modular_synth_util.
 * 
 * Returns: (transfer none): the env-0 sends
 * 
 * Since: 8.2.0
 */
gint*
ags_modular_synth_util_get_env_0_sends(AgsModularSynthUtil *modular_synth_util,
				       guint *env_0_sends_count)
{
  if(modular_synth_util == NULL){
    return(NULL);
  }

  return(&(modular_synth_util->env_0_sends));
}

/**
 * ags_modular_synth_util_set_env_0_sends:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @env_0_sends: the env-0 sends
 *
 * Set @env_0_sends of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_env_0_sends(AgsModularSynthUtil *modular_synth_util,
				       gint env_0_sends,
				       guint env_0_sends_count)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->env_0_sends = *env_0_sends;
}

/**
 * ags_modular_synth_util_get_env_1_attack:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get env-1 attack of @modular_synth_util.
 * 
 * Returns: the env-1 attack
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_env_1_attack(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->env_1_attack);
}

/**
 * ags_modular_synth_util_set_env_1_attack:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @env_1_attack: the env-1 attack
 *
 * Set @env_1_attack of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_env_1_attack(AgsModularSynthUtil *modular_synth_util,
					gdouble env_1_attack)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->env_1_attack = env_1_attack;
}

/**
 * ags_modular_synth_util_get_env_1_decay:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get env-1 decay of @modular_synth_util.
 * 
 * Returns: the env-1 decay
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_env_1_decay(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->env_1_decay);
}

/**
 * ags_modular_synth_util_set_env_1_decay:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @env_1_decay: the env-1 decay
 *
 * Set @env_1_decay of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_env_1_decay(AgsModularSynthUtil *modular_synth_util,
				       gdouble env_1_decay)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->env_1_decay = env_1_decay;
}

/**
 * ags_modular_synth_util_get_env_1_sustain:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get env-1 sustain of @modular_synth_util.
 * 
 * Returns: the env-1 sustain
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_env_1_sustain(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->env_1_sustain);
}

/**
 * ags_modular_synth_util_set_env_1_sustain:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @env_1_sustain: the env-1 sustain
 *
 * Set @env_1_sustain of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_env_1_sustain(AgsModularSynthUtil *modular_synth_util,
					 gdouble env_1_sustain)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->env_1_sustain = env_1_sustain;
}

/**
 * ags_modular_synth_util_get_env_1_release:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get env-1 release of @modular_synth_util.
 * 
 * Returns: the env-1 release
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_env_1_release(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->env_1_release);
}

/**
 * ags_modular_synth_util_set_env_1_release:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @env_1_release: the env-1 release
 *
 * Set @env_1_release of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_env_1_release(AgsModularSynthUtil *modular_synth_util,
					 gdouble env_1_release)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->env_1_release = env_1_release;
}

/**
 * ags_modular_synth_util_get_env_1_gain:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get env-1 gain of @modular_synth_util.
 * 
 * Returns: the env-1 gain
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_env_1_gain(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->env_1_gain);
}

/**
 * ags_modular_synth_util_set_env_1_gain:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @env_1_gain: the env-1 gain
 *
 * Set @env_1_gain of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_env_1_gain(AgsModularSynthUtil *modular_synth_util,
				      gdouble env_1_gain)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->env_1_gain = env_1_gain;
}

/**
 * ags_modular_synth_util_get_env_1_sends:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get env-1 sends of @modular_synth_util.
 * 
 * Returns: (transfer none): the env-1 sends
 * 
 * Since: 8.2.0
 */
gint*
ags_modular_synth_util_get_env_1_sends(AgsModularSynthUtil *modular_synth_util,
				       guint *env_1_sends_count)
{
  if(modular_synth_util == NULL){
    return(NULL);
  }

  return(&(modular_synth_util->env_1_sends));
}

/**
 * ags_modular_synth_util_set_env_1_sends:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @env_1_sends: the env-1 sends
 *
 * Set @env_1_sends of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_env_1_sends(AgsModularSynthUtil *modular_synth_util,
				       gint env_1_sends,
				       guint env_1_sends_count)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->env_1_sends = *env_1_sends;
}

/**
 * ags_modular_synth_util_get_lfo_0_oscillator:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get lfo-0 oscillator mode of @modular_synth_util.
 * 
 * Returns: the lfo-0 oscillator mode as #AgsSynthOscillatorMode-enum
 * 
 * Since: 8.2.0
 */
AgsSynthOscillatorMode
ags_modular_synth_util_get_lfo_0_oscillator(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(AGS_SYNTH_OSCILLATOR_SIN);
  }

  return(modular_synth_util->lfo_0_oscillator);
}

/**
 * ags_modular_synth_util_set_lfo_0_synth_oscillator:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @lfo_0_oscillator: the lfo-0 oscillator mode as #AgsSynthOscillatorMode-enum
 *
 * Set @lfo_0_oscillator of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_lfo_0_oscillator(AgsModularSynthUtil *modular_synth_util,
					    AgsSynthOscillatorMode lfo_0_oscillator)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->lfo_0_oscillator = lfo_0_oscillator;
}

/**
 * ags_modular_synth_util_get_lfo_0_frequency:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get lfo-0 frequency of @modular_synth_util.
 * 
 * Returns: the lfo-0 frequency
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_lfo_0_frequency(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(12.0);
  }

  return(modular_synth_util->lfo_0_frequency);
}

/**
 * ags_modular_synth_util_set_lfo_0_synth_frequency:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @lfo_0_frequency: the lfo-0 frequency
 *
 * Set @lfo_0_frequency of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_lfo_0_frequency(AgsModularSynthUtil *modular_synth_util,
					   gdouble lfo_0_frequency)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->lfo_0_frequency = lfo_0_frequency;
}

/**
 * ags_modular_synth_util_get_lfo_0_depth:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get lfo-0 depth of @modular_synth_util.
 * 
 * Returns: the lfo-0 depth
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_lfo_0_depth(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->lfo_0_depth);
}

/**
 * ags_modular_synth_util_set_lfo_0_synth_depth:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @lfo_0_depth: the lfo-0 depth
 *
 * Set @lfo_0_depth of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_lfo_0_depth(AgsModularSynthUtil *modular_synth_util,
				       gdouble lfo_0_depth)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->lfo_0_depth = lfo_0_depth;
}

/**
 * ags_modular_synth_util_get_lfo_0_tuning:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get lfo-0 tuning of @modular_synth_util.
 * 
 * Returns: the lfo-0 tuning
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_lfo_0_tuning(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(0.0);
  }

  return(modular_synth_util->lfo_0_tuning);
}

/**
 * ags_modular_synth_util_set_lfo_0_synth_tuning:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @lfo_0_tuning: the lfo-0 tuning
 *
 * Set @lfo_0_tuning of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_lfo_0_tuning(AgsModularSynthUtil *modular_synth_util,
					gdouble lfo_0_tuning)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->lfo_0_tuning = lfo_0_tuning;
}

/**
 * ags_modular_synth_util_get_lfo_0_sends:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get lfo-0 sends of @modular_synth_util.
 * 
 * Returns: (transfer none): the lfo-0 sends
 * 
 * Since: 8.2.0
 */
gint*
ags_modular_synth_util_get_lfo_0_sends(AgsModularSynthUtil *modular_synth_util,
				       guint *lfo_0_sends_count)
{
  if(modular_synth_util == NULL){
    return(NULL);
  }

  return(&(modular_synth_util->lfo_0_sends));
}

/**
 * ags_modular_synth_util_set_lfo_0_synth_sends:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @lfo_0_sends: the lfo-0 sends
 *
 * Set @lfo_0_sends of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_lfo_0_sends(AgsModularSynthUtil *modular_synth_util,
				       gint lfo_0_sends,
				       guint lfo_0_sends_count)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->lfo_0_sends = *lfo_0_sends;
}

/**
 * ags_modular_synth_util_get_lfo_1_oscillator:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get lfo-1 oscillator mode of @modular_synth_util.
 * 
 * Returns: the lfo-1 oscillator mode as #AgsSynthOscillatorMode-mode
 * 
 * Since: 8.2.0
 */
AgsSynthOscillatorMode
ags_modular_synth_util_get_lfo_1_oscillator(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(AGS_SYNTH_OSCILLATOR_SIN);
  }

  return(modular_synth_util->lfo_1_oscillator);
}

/**
 * ags_modular_synth_util_set_lfo_1_synth_oscillator:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @lfo_1_oscillator: the lfo-1 oscillator mode
 *
 * Set @lfo_1_oscillator of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_lfo_1_oscillator(AgsModularSynthUtil *modular_synth_util,
					    AgsSynthOscillatorMode lfo_1_oscillator)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->lfo_1_oscillator = lfo_1_oscillator;
}

/**
 * ags_modular_synth_util_get_lfo_1_frequency:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get lfo-1 frequency of @modular_synth_util.
 * 
 * Returns: the lfo-1 frequency
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_lfo_1_frequency(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(12.0);
  }

  return(modular_synth_util->lfo_1_frequency);
}

/**
 * ags_modular_synth_util_set_lfo_1_synth_frequency:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @lfo_1_frequency: the lfo-1 frequency
 *
 * Set @lfo_1_frequency of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_lfo_1_frequency(AgsModularSynthUtil *modular_synth_util,
					   gdouble lfo_1_frequency)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->lfo_1_frequency = lfo_1_frequency;
}

/**
 * ags_modular_synth_util_get_lfo_1_depth:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get lfo-1 depth of @modular_synth_util.
 * 
 * Returns: the lfo-1 depth
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_lfo_1_depth(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->lfo_1_depth);
}

/**
 * ags_modular_synth_util_set_lfo_1_synth_depth:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @lfo_1_depth: the lfo-1 depth
 *
 * Set @lfo_1_depth of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_lfo_1_depth(AgsModularSynthUtil *modular_synth_util,
				       gdouble lfo_1_depth)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->lfo_1_depth = lfo_1_depth;
}

/**
 * ags_modular_synth_util_get_lfo_1_tuning:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get lfo-1 tuning of @modular_synth_util.
 * 
 * Returns: the lfo-1 tuning
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_lfo_1_tuning(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(0.0);
  }

  return(modular_synth_util->lfo_1_tuning);
}

/**
 * ags_modular_synth_util_set_lfo_1_synth_tuning:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @lfo_1_tuning: the lfo-1 tuning
 *
 * Set @lfo_1_tuning of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_lfo_1_tuning(AgsModularSynthUtil *modular_synth_util,
					gdouble lfo_1_tuning)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->lfo_1_tuning = lfo_1_tuning;
}

/**
 * ags_modular_synth_util_get_lfo_1_sends:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get lfo-1 sends of @modular_synth_util.
 * 
 * Returns: (transfer none): the lfo-1 sends
 * 
 * Since: 8.2.0
 */
gint*
ags_modular_synth_util_get_lfo_1_sends(AgsModularSynthUtil *modular_synth_util,
				       guint *lfo_1_sends_count)
{
  if(modular_synth_util == NULL){
    return(NULL);
  }

  return(&(modular_synth_util->lfo_1_sends));
}

/**
 * ags_modular_synth_util_set_lfo_1_synth_sends:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @lfo_1_sends: the lfo-1 sends
 *
 * Set @lfo_1_sends of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_lfo_1_sends(AgsModularSynthUtil *modular_synth_util,
				       gint lfo_1_sends,
				       guint lfo_1_sends_count)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->lfo_1_sends = *lfo_1_sends;
}

/**
 * ags_modular_synth_util_get_noise_frequency:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get noise frequency of @modular_synth_util.
 * 
 * Returns: the noise frequency
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_noise_frequency(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(440.0);
  }

  return(modular_synth_util->noise_frequency);
}

/**
 * ags_modular_synth_util_set_noise_frequency:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @noise_frequency: the noise frequency
 *
 * Set @noise_frequency of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_noise_frequency(AgsModularSynthUtil *modular_synth_util,
					   gdouble noise_frequency)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->noise_frequency = noise_frequency;
}

/**
 * ags_modular_synth_util_get_noise_gain:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get noise gain of @modular_synth_util.
 * 
 * Returns: the noise gain
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_noise_gain(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->noise_gain);
}

/**
 * ags_modular_synth_util_set_noise_gain:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @noise_gain: the noise gain
 *
 * Set @noise_gain of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_noise_gain(AgsModularSynthUtil *modular_synth_util,
				      gdouble noise_gain)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->noise_gain = noise_gain;
}

/**
 * ags_modular_synth_util_get_noise_sends:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get noise sends of @modular_synth_util.
 * 
 * Returns: (transfer none): the noise sends
 * 
 * Since: 8.2.0
 */
gint*
ags_modular_synth_util_get_noise_sends(AgsModularSynthUtil *modular_synth_util,
				       guint *noise_sends_count)
{
  if(modular_synth_util == NULL){
    return(NULL);
  }

  return(&(modular_synth_util->noise_sends));
}

/**
 * ags_modular_synth_util_set_noise_sends:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @noise_sends: the noise sends
 *
 * Set @noise_sends of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_noise_sends(AgsModularSynthUtil *modular_synth_util,
				       gint noise_sends,
				       guint noise_sends_count)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->noise_sends = *noise_sends;
}
/**
 * ags_modular_synth_util_get_frame_count:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get frame count of @modular_synth_util.
 * 
 * Returns: the frame count
 * 
 * Since: 8.2.0
 */
guint
ags_modular_synth_util_get_frame_count(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(0);
  }

  return(modular_synth_util->frame_count);
}

/**
 * ags_modular_synth_util_set_frame_count:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @frame_count: the frame count
 *
 * Set @frame_count of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_frame_count(AgsModularSynthUtil *modular_synth_util,
				       guint frame_count)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->frame_count = frame_count;
}

/**
 * ags_modular_synth_util_get_offset:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get offset of @modular_synth_util.
 * 
 * Returns: the offset
 * 
 * Since: 8.2.0
 */
guint
ags_modular_synth_util_get_offset(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(0);
  }

  return(modular_synth_util->offset);
}

/**
 * ags_modular_synth_util_set_offset:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @offset: the offset
 *
 * Set @offset of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_offset(AgsModularSynthUtil *modular_synth_util,
				  guint offset)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->offset = offset;
}

/**
 * ags_modular_synth_util_get_note_256th_mode:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get note 256th mode of @modular_synth_util.
 * 
 * Returns: %TRUE if note 256th mode, otherwise %FALSE
 * 
 * Since: 8.2.0
 */
gboolean
ags_modular_synth_util_get_note_256th_mode(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(0);
  }

  return(modular_synth_util->note_256th_mode);
}

/**
 * ags_modular_synth_util_set_note_256th_mode:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @note_256th_mode: the note 256th mode
 *
 * Set @note_256th_mode of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_note_256th_mode(AgsModularSynthUtil *modular_synth_util,
					   gboolean note_256th_mode)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->note_256th_mode = note_256th_mode;
}

/**
 * ags_modular_synth_util_get_offset_256th:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get offset as note 256th of @modular_synth_util.
 * 
 * Returns: the offset as note 256th
 * 
 * Since: 8.2.0
 */
guint
ags_modular_synth_util_get_offset_256th(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(0);
  }

  return(modular_synth_util->offset_256th);
}

/**
 * ags_modular_synth_util_set_offset_256th:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @offset_256th: the offset as note 256th
 *
 * Set @offset_256th of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_offset_256th(AgsModularSynthUtil *modular_synth_util,
					guint offset_256th)
{
  if(modular_synth_util == NULL){
    return;
  }
  
  modular_synth_util->offset_256th = offset_256th;
}

/* seq sin oscillator */
void
ags_modular_synth_util_compute_sin_s8(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_sin_s16(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_sin_s24(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_sin_s32(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_sin_s64(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_sin_float(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_sin_double(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_sin_complex(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_sin(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

/* seq sawtooth oscillator */
void
ags_modular_synth_util_compute_sawtooth_s8(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_sawtooth_s16(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_sawtooth_s24(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_sawtooth_s32(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_sawtooth_s64(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_sawtooth_float(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_sawtooth_double(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_sawtooth_complex(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_sawtooth(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

/* seq triangle oscillator */
void
ags_modular_synth_util_compute_triangle_s8(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_triangle_s16(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_triangle_s24(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_triangle_s32(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_triangle_s64(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_triangle_float(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_triangle_double(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_triangle_complex(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_triangle(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

/* seq square oscillator */
void
ags_modular_synth_util_compute_square_s8(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_square_s16(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_square_s24(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_square_s32(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_square_s64(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_square_float(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_square_double(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_square_complex(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_square(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

/* seq impulse oscillator */
void
ags_modular_synth_util_compute_impulse_s8(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_impulse_s16(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_impulse_s24(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_impulse_s32(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_impulse_s64(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_impulse_float(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_impulse_double(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_impulse_complex(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}

void
ags_modular_synth_util_compute_impulse(AgsModularSynthUtil *modular_synth_util)
{
  //TODO:JK: implement me
}
