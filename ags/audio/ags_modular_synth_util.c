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

#include <ags/audio/ags_modular_synth_util.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_common_pitch_util.h>
#include <ags/audio/ags_fluid_util.h>
#include <ags/audio/ags_fluid_interpolate_4th_order_util.h>
#include <ags/audio/ags_envelope_util.h>
#include <ags/audio/ags_lfo_synth_util.h>
#include <ags/audio/ags_noise_util.h>

#if defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
#include <Accelerate/Accelerate.h>
#endif

#include <math.h>
#include <complex.h>

gboolean ags_modular_synth_util_osc_0_frequency_receives(AgsModularSynthUtil *modular_synth_util,
							 gint64 *sends,
							 AgsModularSynthSends modular_synth_sends);
gboolean ags_modular_synth_util_osc_0_phase_receives(AgsModularSynthUtil *modular_synth_util,
						     gint64 *sends,
						     AgsModularSynthSends modular_synth_sends);
gboolean ags_modular_synth_util_osc_0_volume_receives(AgsModularSynthUtil *modular_synth_util,
						      gint64 *sends,
						      AgsModularSynthSends modular_synth_sends);

gboolean ags_modular_synth_util_osc_1_frequency_receives(AgsModularSynthUtil *modular_synth_util,
							 gint64 *sends,
							 AgsModularSynthSends modular_synth_sends);
gboolean ags_modular_synth_util_osc_1_phase_receives(AgsModularSynthUtil *modular_synth_util,
						     gint64 *sends,
						     AgsModularSynthSends modular_synth_sends);
gboolean ags_modular_synth_util_osc_1_volume_receives(AgsModularSynthUtil *modular_synth_util,
						      gint64 *sends,
						      AgsModularSynthSends modular_synth_sends);

gboolean ags_modular_synth_util_pitch_tuning_receives(AgsModularSynthUtil *modular_synth_util,
						      gint64 *sends,
						      AgsModularSynthSends modular_synth_sends);

gboolean ags_modular_synth_util_volume_receives(AgsModularSynthUtil *modular_synth_util,
						gint64 *sends,
						AgsModularSynthSends modular_synth_sends);

extern void ags_fluid_interpolate_4th_order_util_config();

extern gboolean interp_coeff_4th_order_initialized;

extern gdouble interp_coeff_4th_order[AGS_FLUID_INTERP_MAX][4];

extern GMutex interp_coeff_4th_order_mutex;

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

  ptr->pitch_util = ags_fluid_interpolate_4th_order_util_alloc();

  ptr->env_0_util = ags_envelope_util_alloc();

  ptr->env_1_util = ags_envelope_util_alloc();

  ptr->lfo_0_util = ags_lfo_synth_util_alloc();

  ptr->lfo_1_util = ags_lfo_synth_util_alloc();

  ptr->noise_util = ags_noise_util_alloc();

  AGS_NOISE_UTIL(ptr->noise_util)->mode = AGS_NOISE_UTIL_PINK_NOISE;

  AGS_NOISE_UTIL(ptr->noise_util)->frequency = 220.0;
  
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
  new_ptr->env_0_frequency = ptr->env_0_frequency;

  memcpy(&(new_ptr->env_0_sends[0]), &(ptr->env_0_sends[0]), AGS_MODULAR_SYNTH_SENDS_COUNT * sizeof(gint64));

  ags_stream_free(new_ptr->env_0_buffer);

  new_ptr->env_0_buffer = NULL;
  
  if(new_ptr->buffer_length > 0){
    new_ptr->env_0_buffer = ags_stream_alloc(new_ptr->buffer_length,
					     AGS_SOUNDCARD_DOUBLE);
  }
  
  new_ptr->env_1_attack = ptr->env_1_attack;
  new_ptr->env_1_decay = ptr->env_1_decay;
  new_ptr->env_1_sustain = ptr->env_1_sustain;
  new_ptr->env_1_release = ptr->env_1_release;
  new_ptr->env_1_gain = ptr->env_1_gain;
  new_ptr->env_1_frequency = ptr->env_1_frequency;

  memcpy(&(new_ptr->env_1_sends[0]), &(ptr->env_1_sends[0]), AGS_MODULAR_SYNTH_SENDS_COUNT * sizeof(gint64));

  ags_stream_free(new_ptr->env_1_buffer);

  new_ptr->env_1_buffer = NULL;
  
  if(new_ptr->buffer_length > 0){
    new_ptr->env_1_buffer = ags_stream_alloc(new_ptr->buffer_length,
					     AGS_SOUNDCARD_DOUBLE);
  }

  new_ptr->lfo_0_oscillator = ptr->lfo_0_oscillator;
  new_ptr->lfo_0_frequency = ptr->lfo_0_frequency;
  new_ptr->lfo_0_depth = new_ptr->lfo_0_depth;
  new_ptr->lfo_0_tuning = new_ptr->lfo_0_tuning;

  memcpy(&(new_ptr->lfo_0_sends[0]), &(ptr->lfo_0_sends[0]), AGS_MODULAR_SYNTH_SENDS_COUNT * sizeof(gint64));

  ags_stream_free(new_ptr->lfo_0_buffer);

  new_ptr->lfo_0_buffer = NULL;
  
  if(new_ptr->buffer_length > 0){
    new_ptr->lfo_0_buffer = ags_stream_alloc(new_ptr->buffer_length,
					     AGS_SOUNDCARD_DOUBLE);
  }

  new_ptr->lfo_1_oscillator = ptr->lfo_1_oscillator;
  new_ptr->lfo_1_frequency = ptr->lfo_1_frequency;
  new_ptr->lfo_1_depth = new_ptr->lfo_1_depth;
  new_ptr->lfo_1_tuning = new_ptr->lfo_1_tuning;

  memcpy(&(new_ptr->lfo_1_sends[0]), &(ptr->lfo_1_sends[0]), AGS_MODULAR_SYNTH_SENDS_COUNT * sizeof(gint64));

  ags_stream_free(new_ptr->lfo_1_buffer);

  new_ptr->lfo_1_buffer = NULL;
  
  if(new_ptr->buffer_length > 0){
    new_ptr->lfo_1_buffer = ags_stream_alloc(new_ptr->buffer_length,
					     AGS_SOUNDCARD_DOUBLE);
  }
  
  new_ptr->noise_frequency = ptr->noise_frequency;
  new_ptr->noise_gain = ptr->noise_gain;

  memcpy(&(new_ptr->noise_sends[0]), &(ptr->noise_sends[0]), AGS_MODULAR_SYNTH_SENDS_COUNT * sizeof(gint64));

  ags_stream_free(new_ptr->noise_buffer);

  new_ptr->noise_buffer = NULL;
  
  if(new_ptr->buffer_length > 0){
    new_ptr->noise_buffer = ags_stream_alloc(new_ptr->buffer_length,
					     AGS_SOUNDCARD_DOUBLE);
  }

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

  ags_stream_free(ptr->env_0_buffer);
  
  ags_stream_free(ptr->env_1_buffer);
  
  ags_stream_free(ptr->lfo_0_buffer);
  
  ags_stream_free(ptr->lfo_1_buffer);
  
  ags_stream_free(ptr->noise_buffer);
  
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
  if(modular_synth_util == NULL ||
     modular_synth_util->buffer_length == buffer_length){
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
							AGS_SOUNDCARD_DOUBLE);
  }

  /* env-0 */
  ags_stream_free(modular_synth_util->env_0_buffer);

  modular_synth_util->env_0_buffer = NULL;
  
  if(buffer_length > 0){
    modular_synth_util->env_0_buffer = ags_stream_alloc(buffer_length,
							AGS_SOUNDCARD_DOUBLE);
  }

  /* env-1 */
  ags_stream_free(modular_synth_util->env_1_buffer);

  modular_synth_util->env_1_buffer = NULL;
  
  if(buffer_length > 0){
    modular_synth_util->env_1_buffer = ags_stream_alloc(buffer_length,
							AGS_SOUNDCARD_DOUBLE);
  }

  /* lfo-0 */
  ags_stream_free(modular_synth_util->lfo_0_buffer);

  modular_synth_util->lfo_0_buffer = NULL;
  
  if(buffer_length > 0){
    modular_synth_util->lfo_0_buffer = ags_stream_alloc(buffer_length,
							AGS_SOUNDCARD_DOUBLE);
  }

  /* lfo-1 */
  ags_stream_free(modular_synth_util->lfo_1_buffer);

  modular_synth_util->lfo_1_buffer = NULL;
  
  if(buffer_length > 0){
    modular_synth_util->lfo_1_buffer = ags_stream_alloc(buffer_length,
							AGS_SOUNDCARD_DOUBLE);
  }

  /* noise */
  ags_stream_free(modular_synth_util->noise_buffer);

  modular_synth_util->noise_buffer = NULL;
  
  if(buffer_length > 0){
    modular_synth_util->noise_buffer = ags_stream_alloc(buffer_length,
							AGS_SOUNDCARD_DOUBLE);
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

  ags_envelope_util_set_frame_count(modular_synth_util->env_0_util,
				    samplerate / modular_synth_util->env_0_frequency);

  ags_envelope_util_set_frame_count(modular_synth_util->env_1_util,
				    samplerate / modular_synth_util->env_1_frequency);

  ags_lfo_synth_util_set_frame_count(modular_synth_util->lfo_0_util,
				     samplerate / modular_synth_util->lfo_0_frequency);

  ags_lfo_synth_util_set_frame_count(modular_synth_util->lfo_1_util,
				     samplerate / modular_synth_util->lfo_1_frequency);

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
 * Get pitch type of @modular_synth_util.
 * 
 * Returns: the pitch type
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
 * @pitch_type: the pitch type
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
 * ags_modular_synth_util_get_volume:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get volume of @modular_synth_util.
 * 
 * Returns: the volume
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_volume(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->volume);
}

/**
 * ags_modular_synth_util_set_volume:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @volume: the volume
 *
 * Set @volume of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_volume(AgsModularSynthUtil *modular_synth_util,
				  gdouble volume)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->volume = volume;
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
 * ags_modular_synth_util_get_env_0_frequency:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get env-0 frequency of @modular_synth_util.
 * 
 * Returns: the env-0 frequency
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_env_0_frequency(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->env_0_frequency);
}

/**
 * ags_modular_synth_util_set_env_0_frequency:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @env_0_frequency: the env-0 frequency
 *
 * Set @env_0_frequency of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_env_0_frequency(AgsModularSynthUtil *modular_synth_util,
					   gdouble env_0_frequency)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->env_0_frequency = env_0_frequency;

  ags_envelope_util_set_frame_count(modular_synth_util->env_0_util,
				    modular_synth_util->samplerate / env_0_frequency);
}

/**
 * ags_modular_synth_util_get_env_0_sends:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @env_0_sends_count: (out): return location of the env-0 sends count
 * 
 * Get env-0 sends of @modular_synth_util.
 * 
 * Returns: (transfer none): the env-0 sends
 * 
 * Since: 8.2.0
 */
gint64*
ags_modular_synth_util_get_env_0_sends(AgsModularSynthUtil *modular_synth_util,
				       guint *env_0_sends_count)
{
  if(modular_synth_util == NULL){
    return(NULL);
  }

  if(env_0_sends_count != NULL){
    env_0_sends_count[0] = AGS_MODULAR_SYNTH_SENDS_COUNT;
  }

  return(&(modular_synth_util->env_0_sends[0]));
}

/**
 * ags_modular_synth_util_set_env_0_sends:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @env_0_sends: the env-0 sends
 * @env_0_sends_count: the env-0 sends count
 *
 * Set @env_0_sends of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_env_0_sends(AgsModularSynthUtil *modular_synth_util,
				       gint64 *env_0_sends,
				       guint env_0_sends_count)
{
  if(modular_synth_util == NULL){
    return;
  }

  memcpy(&(modular_synth_util->env_0_sends[0]), env_0_sends, AGS_MODULAR_SYNTH_SENDS_COUNT * sizeof(gint64));
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
 * ags_modular_synth_util_get_env_1_frequency:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * 
 * Get env-1 frequency of @modular_synth_util.
 * 
 * Returns: the env-1 frequency
 * 
 * Since: 8.2.0
 */
gdouble
ags_modular_synth_util_get_env_1_frequency(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util == NULL){
    return(1.0);
  }

  return(modular_synth_util->env_1_frequency);
}

/**
 * ags_modular_synth_util_set_env_1_frequency:
 * @modular_synth_util: the #AgsModularSynthUtil-struct
 * @env_1_frequency: the env-1 frequency
 *
 * Set @env_1_frequency of @modular_synth_util.
 *
 * Since: 8.2.0
 */
void
ags_modular_synth_util_set_env_1_frequency(AgsModularSynthUtil *modular_synth_util,
					   gdouble env_1_frequency)
{
  if(modular_synth_util == NULL){
    return;
  }

  modular_synth_util->env_1_frequency = env_1_frequency;
  
  ags_envelope_util_set_frame_count(modular_synth_util->env_1_util,
				    modular_synth_util->samplerate / env_1_frequency);
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
gint64*
ags_modular_synth_util_get_env_1_sends(AgsModularSynthUtil *modular_synth_util,
				       guint *env_1_sends_count)
{
  if(modular_synth_util == NULL){
    return(NULL);
  }

  if(env_1_sends_count != NULL){
    env_1_sends_count[0] = AGS_MODULAR_SYNTH_SENDS_COUNT;
  }

  return(&(modular_synth_util->env_1_sends[0]));
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
				       gint64 *env_1_sends,
				       guint env_1_sends_count)
{
  if(modular_synth_util == NULL){
    return;
  }

  memcpy(&(modular_synth_util->env_1_sends[0]), env_1_sends, AGS_MODULAR_SYNTH_SENDS_COUNT * sizeof(gint64));
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
    return(AGS_MODULAR_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
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

  ags_lfo_synth_util_set_frame_count(modular_synth_util->lfo_0_util,
				     modular_synth_util->samplerate / lfo_0_frequency);
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
gint64*
ags_modular_synth_util_get_lfo_0_sends(AgsModularSynthUtil *modular_synth_util,
				       guint *lfo_0_sends_count)
{
  if(modular_synth_util == NULL){
    return(NULL);
  }

  if(lfo_0_sends_count != NULL){
    lfo_0_sends_count[0] = AGS_MODULAR_SYNTH_SENDS_COUNT;
  }

  return(&(modular_synth_util->lfo_0_sends[0]));
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
				       gint64 *lfo_0_sends,
				       guint lfo_0_sends_count)
{
  if(modular_synth_util == NULL){
    return;
  }

  memcpy(&(modular_synth_util->lfo_0_sends[0]), lfo_0_sends, AGS_MODULAR_SYNTH_SENDS_COUNT * sizeof(gint64));
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
    return(AGS_MODULAR_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
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

  ags_lfo_synth_util_set_frame_count(modular_synth_util->lfo_1_util,
				     modular_synth_util->samplerate / lfo_1_frequency);
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
gint64*
ags_modular_synth_util_get_lfo_1_sends(AgsModularSynthUtil *modular_synth_util,
				       guint *lfo_1_sends_count)
{
  if(modular_synth_util == NULL){
    return(NULL);
  }

  if(lfo_1_sends_count != NULL){
    lfo_1_sends_count[0] = AGS_MODULAR_SYNTH_SENDS_COUNT;
  }

  return(&(modular_synth_util->lfo_1_sends[0]));
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
				       gint64 *lfo_1_sends,
				       guint lfo_1_sends_count)
{
  if(modular_synth_util == NULL){
    return;
  }

  memcpy(&(modular_synth_util->lfo_1_sends[0]), lfo_1_sends, AGS_MODULAR_SYNTH_SENDS_COUNT * sizeof(gint64));
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
gint64*
ags_modular_synth_util_get_noise_sends(AgsModularSynthUtil *modular_synth_util,
				       guint *noise_sends_count)
{
  if(modular_synth_util == NULL){
    return(NULL);
  }

  if(noise_sends_count != NULL){
    noise_sends_count[0] = AGS_MODULAR_SYNTH_SENDS_COUNT;
  }

  return(&(modular_synth_util->noise_sends[0]));
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
				       gint64 *noise_sends,
				       guint noise_sends_count)
{
  if(modular_synth_util == NULL){
    return;
  }
  
  memcpy(&(modular_synth_util->noise_sends[0]), noise_sends, AGS_MODULAR_SYNTH_SENDS_COUNT * sizeof(gint64));
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

gboolean
ags_modular_synth_util_osc_0_frequency_receives(AgsModularSynthUtil *modular_synth_util,
						gint64 *sends,
						AgsModularSynthSends modular_synth_sends)
{
  guint i;
  gboolean success;

  if(modular_synth_sends != AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY){
    return(FALSE);
  }
  
  success = FALSE;

  for(i = 0; i < AGS_MODULAR_SYNTH_SENDS_COUNT; i++){
    if(sends[i] == modular_synth_sends){
      success = TRUE;

      break;
    }
  }
  
  return(success);
}

gboolean
ags_modular_synth_util_osc_0_phase_receives(AgsModularSynthUtil *modular_synth_util,
					    gint64 *sends,
					    AgsModularSynthSends modular_synth_sends)
{
  guint i;
  gboolean success;

  if(modular_synth_sends != AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE){
    return(FALSE);
  }
  
  success = FALSE;

  for(i = 0; i < AGS_MODULAR_SYNTH_SENDS_COUNT; i++){
    if(sends[i] == modular_synth_sends){
      success = TRUE;

      break;
    }
  }
  
  return(success);
}

gboolean
ags_modular_synth_util_osc_0_volume_receives(AgsModularSynthUtil *modular_synth_util,
					     gint64 *sends,
					     AgsModularSynthSends modular_synth_sends)
{
  guint i;
  gboolean success;

  if(modular_synth_sends != AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME){
    return(FALSE);
  }
  
  success = FALSE;

  for(i = 0; i < AGS_MODULAR_SYNTH_SENDS_COUNT; i++){
    if(sends[i] == modular_synth_sends){
      success = TRUE;

      break;
    }
  }
  
  return(success);
}

gboolean
ags_modular_synth_util_osc_1_frequency_receives(AgsModularSynthUtil *modular_synth_util,
						gint64 *sends,
						AgsModularSynthSends modular_synth_sends)
{
  guint i;
  gboolean success;

  if(modular_synth_sends != AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY){
    return(FALSE);
  }
  
  success = FALSE;

  for(i = 0; i < AGS_MODULAR_SYNTH_SENDS_COUNT; i++){
    if(sends[i] == modular_synth_sends){
      success = TRUE;

      break;
    }
  }
  
  return(success);
}

gboolean
ags_modular_synth_util_osc_1_phase_receives(AgsModularSynthUtil *modular_synth_util,
					    gint64 *sends,
					    AgsModularSynthSends modular_synth_sends)
{
  guint i;
  gboolean success;

  if(modular_synth_sends != AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE){
    return(FALSE);
  }
  
  success = FALSE;

  for(i = 0; i < AGS_MODULAR_SYNTH_SENDS_COUNT; i++){
    if(sends[i] == modular_synth_sends){
      success = TRUE;

      break;
    }
  }
  
  return(success);
}

gboolean
ags_modular_synth_util_osc_1_volume_receives(AgsModularSynthUtil *modular_synth_util,
					     gint64 *sends,
					     AgsModularSynthSends modular_synth_sends)
{
  guint i;
  gboolean success;

  if(modular_synth_sends != AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME){
    return(FALSE);
  }
  
  success = FALSE;

  for(i = 0; i < AGS_MODULAR_SYNTH_SENDS_COUNT; i++){
    if(sends[i] == modular_synth_sends){
      success = TRUE;

      break;
    }
  }
  
  return(success);
}

gboolean
ags_modular_synth_util_pitch_tuning_receives(AgsModularSynthUtil *modular_synth_util,
					     gint64 *sends,
					     AgsModularSynthSends modular_synth_sends)
{
  guint i;
  gboolean success;

  if(modular_synth_sends != AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING){
    return(FALSE);
  }
  
  success = FALSE;

  for(i = 0; i < AGS_MODULAR_SYNTH_SENDS_COUNT; i++){
    if(sends[i] == modular_synth_sends){
      success = TRUE;

      break;
    }
  }
  
  return(success);
}

gboolean
ags_modular_synth_util_volume_receives(AgsModularSynthUtil *modular_synth_util,
				       gint64 *sends,
				       AgsModularSynthSends modular_synth_sends)
{
  guint i;
  gboolean success;

  if(modular_synth_sends != AGS_MODULAR_SYNTH_SENDS_VOLUME){
    return(FALSE);
  }
  
  success = FALSE;

  for(i = 0; i < AGS_MODULAR_SYNTH_SENDS_COUNT; i++){
    if(sends[i] == modular_synth_sends){
      success = TRUE;

      break;
    }
  }
  
  return(success);
}

/* modular synth */
void
ags_modular_synth_util_compute_s8(AgsModularSynthUtil *modular_synth_util)
{
  gint8 *source, *tmp_source;

  guint source_stride;
  guint buffer_length;
  guint samplerate;

  guint nth_sends;
  gboolean env_0_has_sends;
  gboolean env_1_has_sends;
  gboolean lfo_0_has_sends;
  gboolean lfo_1_has_sends;
  gboolean noise_has_sends;

  gdouble env_volume;
  gdouble env_amount;

  gdouble osc_0_frequency;
  gdouble osc_0_phase;
  gdouble osc_0_volume;

  gdouble osc_1_frequency;
  gdouble osc_1_phase;
  gdouble osc_1_volume;

  gdouble main_pitch_tuning;

  gdouble *pitch_buffer;

  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;

  gdouble phase_incr;
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;

  gdouble freq_period;
  
  gdouble main_volume;

  gdouble volume;
  gdouble tmp_volume, end_volume;

  guint offset;
  guint tmp_offset, end_offset;
  guint compute_frame_count;
  guint frame_count;
  guint i, i_stop;

  static const gdouble scale = 127.0;
  
  if(modular_synth_util == NULL ||
     modular_synth_util->source == NULL){
    return;
  }

  source = modular_synth_util->source;

  source_stride = modular_synth_util->source_stride;

  buffer_length = modular_synth_util->buffer_length;

  samplerate = modular_synth_util->samplerate;

  if(samplerate == 0){
    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  }
  
  osc_0_frequency = modular_synth_util->osc_0_frequency;
  osc_0_phase = modular_synth_util->osc_0_phase;
  osc_0_volume = modular_synth_util->osc_0_volume;
  
  if(osc_0_frequency == 0.0){
    osc_0_frequency = 0.01;
  }

  osc_1_frequency = modular_synth_util->osc_1_frequency;
  osc_1_phase = modular_synth_util->osc_1_phase;
  osc_1_volume = modular_synth_util->osc_1_volume;
  
  if(osc_1_frequency == 0.0){
    osc_1_frequency = 0.01;
  }
  
  pitch_buffer = modular_synth_util->pitch_buffer;
  
  frame_count = modular_synth_util->frame_count;
  offset = modular_synth_util->offset;

  i = 0;

  /* env-0 */
  env_0_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->env_0_sends[nth_sends] != 0){
      env_0_has_sends = TRUE;

      break;
    }
  }

  if(env_0_has_sends){
    for(i = 0; i < modular_synth_util->buffer_length; i++){
      ((gdouble *) modular_synth_util->env_0_buffer)[i] = 1.0;
    }
    
    ags_envelope_util_set_source(modular_synth_util->env_0_util,
				 modular_synth_util->env_0_buffer);

    ags_envelope_util_set_destination(modular_synth_util->env_0_util,
				      modular_synth_util->env_0_buffer);

    ags_envelope_util_set_buffer_length(modular_synth_util->env_0_util,
					modular_synth_util->buffer_length);

    ags_envelope_util_set_format(modular_synth_util->env_0_util,
				 AGS_SOUNDCARD_DOUBLE);

    tmp_offset = offset;

    compute_frame_count = ((double) frame_count / 4.0);

    while(tmp_offset < frame_count){
      /* env-0 attack */
      env_amount = 0.0;
      env_volume = 1.0;

      if((double) tmp_offset >= 0.0 &&
	 (double) tmp_offset < (double) frame_count / 4.0){
	env_volume = modular_synth_util->env_0_attack;

	end_offset = ((double) frame_count / 4.0);

	end_volume = modular_synth_util->env_0_decay;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - 0);

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 4.0 &&
	       (double) tmp_offset < (double) frame_count / 2.0){
	env_volume = modular_synth_util->env_0_decay;

	end_offset = ((double) frame_count / 2.0);

	end_volume = modular_synth_util->env_0_sustain;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 2.0 &&
	       (double) tmp_offset < (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_0_sustain;

	end_offset = ((double) frame_count * 3.0 / 4.0);

	end_volume = modular_synth_util->env_0_release;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 2.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_0_release;

	end_offset = ((double) frame_count);

	end_volume = modular_synth_util->env_0_release;
      
	if(end_offset == 0 ||
	   end_offset >= frame_count){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count * 3.0 / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else{
	env_amount = 0.0;
	env_volume = modular_synth_util->env_0_release;
      }

      if(frame_count - tmp_offset > 0){
	compute_frame_count = frame_count - tmp_offset;
      }else{
	compute_frame_count = ((double) frame_count / 4.0);
      }
      
      if(compute_frame_count > (guint) ((double) frame_count / 4.0)){
	compute_frame_count = (guint) ((double) frame_count / 4.0);
      }
      
      if((tmp_offset - offset) + compute_frame_count > modular_synth_util->buffer_length){
	compute_frame_count = modular_synth_util->buffer_length - (tmp_offset - offset);
      }
      
      if(tmp_offset > 0){
	if(tmp_offset - offset >= modular_synth_util->buffer_length){
	  break;
	}
      }

      ags_envelope_util_set_buffer_length(modular_synth_util->env_0_util,
					  compute_frame_count);

      ags_envelope_util_set_source(modular_synth_util->env_0_util,
				   ((gdouble *) modular_synth_util->env_0_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_destination(modular_synth_util->env_0_util,
					((gdouble *) modular_synth_util->env_0_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_offset(modular_synth_util->env_0_util,
				   0);

      ags_envelope_util_set_volume(modular_synth_util->env_0_util,
				   env_volume);

      ags_envelope_util_set_amount(modular_synth_util->env_0_util,
				   env_amount);

      ags_envelope_util_compute(modular_synth_util->env_0_util);

      /* iterate */
      tmp_offset += (guint) compute_frame_count;
    }
  }

  /* env-1 */
  env_1_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->env_1_sends[nth_sends] != 0){
      env_1_has_sends = TRUE;

      break;
    }
  }

  if(env_1_has_sends){
    for(i = 0; i < modular_synth_util->buffer_length; i++){
      ((gdouble *) modular_synth_util->env_1_buffer)[i] = 1.0;
    }
    
    ags_envelope_util_set_source(modular_synth_util->env_1_util,
				 modular_synth_util->env_1_buffer);

    ags_envelope_util_set_destination(modular_synth_util->env_1_util,
				      modular_synth_util->env_1_buffer);

    ags_envelope_util_set_buffer_length(modular_synth_util->env_1_util,
					modular_synth_util->buffer_length);

    ags_envelope_util_set_format(modular_synth_util->env_1_util,
				 AGS_SOUNDCARD_DOUBLE);

    tmp_offset = offset;

    compute_frame_count = ((double) frame_count / 4.0);
    
    while(tmp_offset < frame_count){
      /* env-1 attack */
      env_amount = 0.0;
      env_volume = 1.0;

      if((double) tmp_offset >= 0.0 &&
	 (double) tmp_offset < (double) frame_count / 4.0){
	env_volume = modular_synth_util->env_1_attack;

	end_offset = ((double) frame_count / 4.0);

	end_volume = modular_synth_util->env_1_decay;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - 0);

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 4.0 &&
	       (double) tmp_offset < (double) frame_count / 2.0){
	env_volume = modular_synth_util->env_1_decay;

	end_offset = ((double) frame_count / 2.0);

	end_volume = modular_synth_util->env_1_sustain;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 2.0 &&
	       (double) tmp_offset < (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_1_sustain;

	end_offset = ((double) frame_count * 3.0 / 4.0);

	end_volume = modular_synth_util->env_1_release;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 2.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_1_release;

	end_offset = ((double) frame_count);

	end_volume = modular_synth_util->env_1_release;
      
	if(end_offset == 0 ||
	   end_offset >= frame_count){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count * 3.0 / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else{
	env_amount = 0.0;
	env_volume = modular_synth_util->env_0_release;
      }

      if(frame_count - tmp_offset > 0){
	compute_frame_count = frame_count - tmp_offset;
      }else{
	compute_frame_count = ((double) frame_count / 4.0);
      }
      
      if(compute_frame_count > (guint) ((double) frame_count / 4.0)){
	compute_frame_count = (guint) ((double) frame_count / 4.0);
      }
      
      if((tmp_offset - offset) + compute_frame_count > modular_synth_util->buffer_length){
	compute_frame_count = modular_synth_util->buffer_length - (tmp_offset - offset);
      }
      
      if(tmp_offset > 0){
	if(tmp_offset - offset >= modular_synth_util->buffer_length){
	  break;
	}
      }

      ags_envelope_util_set_buffer_length(modular_synth_util->env_1_util,
					  compute_frame_count);

      ags_envelope_util_set_source(modular_synth_util->env_1_util,
				   ((gdouble *) modular_synth_util->env_1_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_destination(modular_synth_util->env_1_util,
					((gdouble *) modular_synth_util->env_1_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_offset(modular_synth_util->env_1_util,
				   tmp_offset);

      ags_envelope_util_set_volume(modular_synth_util->env_1_util,
				   env_volume);

      ags_envelope_util_set_amount(modular_synth_util->env_1_util,
				   env_amount);

      ags_envelope_util_compute(modular_synth_util->env_1_util);

      /* iterate */
      tmp_offset += (guint) compute_frame_count;
    }
  }

  /* LFO-0 */
  lfo_0_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->lfo_0_sends[nth_sends] != 0){
      lfo_0_has_sends = TRUE;

      break;
    }
  }

  if(lfo_0_has_sends){
    ags_lfo_synth_util_set_source(modular_synth_util->lfo_0_util,
				  modular_synth_util->lfo_0_buffer);

    ags_lfo_synth_util_set_buffer_length(modular_synth_util->lfo_0_util,
					 modular_synth_util->buffer_length);

    ags_lfo_synth_util_set_format(modular_synth_util->lfo_0_util,
				  AGS_SOUNDCARD_DOUBLE);

    ags_lfo_synth_util_set_samplerate(modular_synth_util->lfo_0_util,
				      samplerate);

    ags_lfo_synth_util_set_lfo_synth_oscillator_mode(modular_synth_util->lfo_0_util,
						     modular_synth_util->lfo_0_oscillator);

    ags_lfo_synth_util_set_frequency(modular_synth_util->lfo_0_util,
				     modular_synth_util->lfo_0_frequency);

    ags_lfo_synth_util_set_lfo_depth(modular_synth_util->lfo_0_util,
				     modular_synth_util->lfo_0_depth);
  
    ags_lfo_synth_util_set_tuning(modular_synth_util->lfo_0_util,
				      modular_synth_util->lfo_0_tuning);

    ags_lfo_synth_util_set_offset(modular_synth_util->lfo_0_util,
				  offset);

    switch(modular_synth_util->lfo_0_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
      {
	ags_lfo_synth_util_compute_sin(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
      {
	ags_lfo_synth_util_compute_sawtooth(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
      {
	ags_lfo_synth_util_compute_triangle(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
      {
	ags_lfo_synth_util_compute_square(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
      {
	ags_lfo_synth_util_compute_impulse(modular_synth_util->lfo_0_util);
      }
      break;
    }
  }

  /* LFO-1 */
  lfo_1_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->lfo_1_sends[nth_sends] != 0){
      lfo_1_has_sends = TRUE;

      break;
    }
  }

  if(lfo_1_has_sends){
    ags_lfo_synth_util_set_source(modular_synth_util->lfo_1_util,
				  modular_synth_util->lfo_1_buffer);

    ags_lfo_synth_util_set_buffer_length(modular_synth_util->lfo_1_util,
					 modular_synth_util->buffer_length);

    ags_lfo_synth_util_set_format(modular_synth_util->lfo_1_util,
				  AGS_SOUNDCARD_DOUBLE);

    ags_lfo_synth_util_set_samplerate(modular_synth_util->lfo_1_util,
				      samplerate);

    ags_lfo_synth_util_set_lfo_synth_oscillator_mode(modular_synth_util->lfo_1_util,
						     modular_synth_util->lfo_1_oscillator);

    ags_lfo_synth_util_set_frequency(modular_synth_util->lfo_1_util,
				     modular_synth_util->lfo_1_frequency);

    ags_lfo_synth_util_set_lfo_depth(modular_synth_util->lfo_1_util,
				     modular_synth_util->lfo_1_depth);
  
    ags_lfo_synth_util_set_tuning(modular_synth_util->lfo_1_util,
				  modular_synth_util->lfo_1_tuning);

    ags_lfo_synth_util_set_offset(modular_synth_util->lfo_1_util,
				  offset);

    switch(modular_synth_util->lfo_1_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
      {
	ags_lfo_synth_util_compute_sin(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
      {
	ags_lfo_synth_util_compute_sawtooth(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
      {
	ags_lfo_synth_util_compute_triangle(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
      {
	ags_lfo_synth_util_compute_square(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
      {
	ags_lfo_synth_util_compute_impulse(modular_synth_util->lfo_1_util);
      }
      break;
    }
  }
  
  /* noise */
  noise_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->noise_sends[nth_sends] != 0){
      noise_has_sends = TRUE;

      break;
    }
  }

  if(noise_has_sends){
    ags_noise_util_set_source(modular_synth_util->noise_util,
			      modular_synth_util->noise_buffer);

    ags_noise_util_set_buffer_length(modular_synth_util->noise_util,
				     modular_synth_util->buffer_length);

    ags_noise_util_set_format(modular_synth_util->noise_util,
			      AGS_SOUNDCARD_DOUBLE);

    ags_noise_util_set_samplerate(modular_synth_util->noise_util,
				  samplerate);

    ags_noise_util_set_volume(modular_synth_util->noise_util,
			      modular_synth_util->noise_gain);

    ags_noise_util_set_frequency(modular_synth_util->noise_util,
				 modular_synth_util->noise_frequency);

    ags_noise_util_set_offset(modular_synth_util->noise_util,
			      offset);

    ags_noise_util_compute(modular_synth_util->noise_util);
  }

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = modular_synth_util->buffer_length - (modular_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_sine;
    ags_v8double v_sawtooth;
    ags_v8double v_triangle;
    ags_v8double v_square;
    ags_v8double v_impulse;
    ags_v8double v_buffer;

    ags_v8double v_osc_0_frequency;
    ags_v8double tmp_v_osc_0_frequency;
    ags_v8double v_osc_0_phase;
    ags_v8double tmp_v_osc_0_phase;
    ags_v8double v_osc_0_volume;
    ags_v8double tmp_v_osc_0_volume;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride)
    };

    v_osc_0_frequency = (ags_v8double) {
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency
    };

    v_osc_0_phase = (ags_v8double) {
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase
    };

    v_osc_0_volume = (ags_v8double) {
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume
    };

    /* env-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_frequency = tmp_v_osc_0_frequency * v_osc_0_frequency;
    }
      
    /* env-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_phase = (tmp_v_osc_0_phase * 2.0 * M_PI) + v_osc_0_phase;
    }

    /* env-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_volume = tmp_v_osc_0_volume * v_osc_0_volume;
    }

    /* env-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_frequency = tmp_v_osc_0_frequency * v_osc_0_frequency;
    }

    /* env-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_phase = (tmp_v_osc_0_phase * 2.0 * M_PI) + v_osc_0_phase;
    }

    /* env-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_volume = tmp_v_osc_0_volume * v_osc_0_volume;
    }

    /* lfo-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }
      
    /* lfo-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* lfo-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* lfo-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }

    /* lfo-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* lfo-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* noise to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }

    /* noise to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* noise to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* compute osc-0 */
    if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      v_sine = (ags_v8double) {
	(gdouble) (sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate))
      };
    
      v_buffer += (v_sine * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      v_sawtooth = (ags_v8double) {
 	((fmod(((gdouble) (offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[0] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[1] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[2] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[3] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[4] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[5] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[6] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[7] / samplerate) - 1.0)
      };
    
      v_buffer += (v_sawtooth * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      v_triangle = (ags_v8double) {
	(((((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * v_osc_0_frequency[0] / samplerate * 2.0) - (((double) ((((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * v_osc_0_frequency[0] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * v_osc_0_frequency[1] / samplerate * 2.0) - (((double) ((((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * v_osc_0_frequency[1] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * v_osc_0_frequency[2] / samplerate * 2.0) - (((double) ((((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * v_osc_0_frequency[2] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * v_osc_0_frequency[3] / samplerate * 2.0) - (((double) ((((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * v_osc_0_frequency[3] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * v_osc_0_frequency[4] / samplerate * 2.0) - (((double) ((((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * v_osc_0_frequency[4] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * v_osc_0_frequency[5] / samplerate * 2.0) - (((double) ((((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * v_osc_0_frequency[5] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * v_osc_0_frequency[6] / samplerate * 2.0) - (((double) ((((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * v_osc_0_frequency[6] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * v_osc_0_frequency[7] / samplerate * 2.0) - (((double) ((((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * v_osc_0_frequency[7] / samplerate)) / 2.0) * 2.0) - 1.0)),
      };
    
      v_buffer += (v_triangle * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      v_square = (ags_v8double) {
	((sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
      };
    
      v_buffer += (v_square * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      v_impulse = (ags_v8double) {
	(sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0)
      };
    
      v_buffer += (v_impulse * scale * v_osc_0_volume);
    }
    
    *(source) = (gint8) v_buffer[0];
    *(source += source_stride) = (gint8) v_buffer[1];
    *(source += source_stride) = (gint8) v_buffer[2];
    *(source += source_stride) = (gint8) v_buffer[3];
    *(source += source_stride) = (gint8) v_buffer[4];
    *(source += source_stride) = (gint8) v_buffer[5];
    *(source += source_stride) = (gint8) v_buffer[6];
    *(source += source_stride) = (gint8) v_buffer[7];

    source += source_stride;

    i += 8;
  }
#endif
  
  for(; i < modular_synth_util->buffer_length;){
    /* OSC-0 */
    osc_0_frequency = modular_synth_util->osc_0_frequency;

    osc_0_phase = modular_synth_util->osc_0_phase;

    osc_0_volume = modular_synth_util->osc_0_volume;

    /* env-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_0_frequency);
    }

    /* env-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = (((gdouble *) modular_synth_util->env_0_buffer)[i] * 2.0 * M_PI) + (osc_0_phase);
    }

    /* env-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_0_volume);
    }

    /* env-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (osc_0_frequency);
    }

    /* env-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = (((gdouble *) modular_synth_util->env_1_buffer)[i] * 2.0 * M_PI) + (osc_0_phase);
    }

    /* env-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (modular_synth_util->osc_0_volume);
    }

    /* lfo-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* lfo-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* lfo-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_0_volume);
    }

    /* lfo-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* lfo-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* lfo-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_0_volume);
    }

    /* noise to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* noise to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* noise to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (modular_synth_util->osc_0_volume);
    }

   /* compute osc-0 */
    if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      source[0] = (gint8) ((gint16) (source[0]) + (gint16) (sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) * scale * osc_0_volume));
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      source[0] = (gint8) ((gint16) (source[0]) + ((fmod(((gdouble) (offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))), (samplerate / osc_0_frequency)) * 2.0 * osc_0_frequency / samplerate) - 1.0) * scale * osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      source[0] = (gint8) ((gint16) (source[0]) + (((((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * osc_0_frequency / samplerate * 2.0) - (((double) ((((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * osc_0_frequency / samplerate)) / 2.0) * 2.0) - 1.0) * scale * osc_0_volume));
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      source[0] = (gint8) ((gint16) (source[0]) + ((sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * scale * osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      source[0] = (gint8) ((gint16) (source[0]) + (sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * scale * osc_0_volume);
    }

    source += source_stride;
    i++;
  }

  /* OSC-1 */
  source = modular_synth_util->source;

  offset = modular_synth_util->offset;

  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = modular_synth_util->buffer_length - (modular_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_sine;
    ags_v8double v_sawtooth;
    ags_v8double v_triangle;
    ags_v8double v_square;
    ags_v8double v_impulse;
    ags_v8double v_buffer;

    ags_v8double v_osc_1_frequency;
    ags_v8double tmp_v_osc_1_frequency;
    ags_v8double v_osc_1_phase;
    ags_v8double tmp_v_osc_1_phase;
    ags_v8double v_osc_1_volume;
    ags_v8double tmp_v_osc_1_volume;
  
    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride)
    };

    v_osc_1_frequency = (ags_v8double) {
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency
    };

    v_osc_1_phase = (ags_v8double) {
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase
    };

    v_osc_1_volume = (ags_v8double) {
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume
    };

    /* env-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }
      
    /* env-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* env-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* env-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* env-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* env-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* lfo-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }
      
    /* lfo-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* lfo-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* lfo-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* lfo-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* lfo-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* noise to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* noise to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* noise to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7],
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* compute osc-1 */
    if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      v_sine = (ags_v8double) {
	(gdouble) (sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate))
      };
    
      v_buffer += (v_sine * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      v_sawtooth = (ags_v8double) {
	((fmod(((gdouble) (offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[0] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[1] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[2] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[3] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[4] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[5] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[6] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[7] / samplerate) - 1.0)
      };
    
      v_buffer += (v_sawtooth * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      v_triangle = (ags_v8double) {
	(((((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * v_osc_1_frequency[0] / samplerate * 2.0) - (((double) ((((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * v_osc_1_frequency[0] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * v_osc_1_frequency[1] / samplerate * 2.0) - (((double) ((((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * v_osc_1_frequency[1] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * v_osc_1_frequency[2] / samplerate * 2.0) - (((double) ((((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * v_osc_1_frequency[2] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * v_osc_1_frequency[3] / samplerate * 2.0) - (((double) ((((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * v_osc_1_frequency[3] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * v_osc_1_frequency[4] / samplerate * 2.0) - (((double) ((((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * v_osc_1_frequency[4] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * v_osc_1_frequency[5] / samplerate * 2.0) - (((double) ((((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * v_osc_1_frequency[5] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * v_osc_1_frequency[6] / samplerate * 2.0) - (((double) ((((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * v_osc_1_frequency[6] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * v_osc_1_frequency[7] / samplerate * 2.0) - (((double) ((((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * v_osc_1_frequency[7] / samplerate)) / 2.0) * 2.0) - 1.0)),
      };
    
      v_buffer += (v_triangle * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      v_square = (ags_v8double) {
	((sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
      };
    
      v_buffer += (v_square * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      v_impulse = (ags_v8double) {
	(sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0)
      };
    
      v_buffer += (v_impulse * scale * v_osc_1_volume);
    }
    
    *(source) = (gint8) v_buffer[0];
    *(source += source_stride) = (gint8) v_buffer[1];
    *(source += source_stride) = (gint8) v_buffer[2];
    *(source += source_stride) = (gint8) v_buffer[3];
    *(source += source_stride) = (gint8) v_buffer[4];
    *(source += source_stride) = (gint8) v_buffer[5];
    *(source += source_stride) = (gint8) v_buffer[6];
    *(source += source_stride) = (gint8) v_buffer[7];

    source += source_stride;

    i += 8;
  }
#endif
  
  for(; i < modular_synth_util->buffer_length;){
    /* OSC-1 */
    osc_1_frequency = modular_synth_util->osc_1_frequency;

    osc_1_phase = modular_synth_util->osc_1_phase;

    osc_1_volume = modular_synth_util->osc_1_volume;

    /* env-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = (2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[i] - 1.0) * (osc_1_frequency);
    }

    /* env-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = (((gdouble *) modular_synth_util->env_0_buffer)[i] * 2.0 * M_PI) + (osc_1_phase);
    }

    /* env-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_1_volume);
    }

    /* env-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = (2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[i] - 1.0) * (osc_1_frequency);
    }

    /* env-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = (((gdouble *) modular_synth_util->env_1_buffer)[i] * 2.0 * M_PI) + (osc_1_phase);
    }

    /* env-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (osc_1_volume);
    }

    /* lfo-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* lfo-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* lfo-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_1_volume);
    }

    /* lfo-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* lfo-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* lfo-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_1_volume);
    }
    
    /* noise to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* noise to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* noise to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (modular_synth_util->osc_0_volume);
    }

   /* compute osc-1 */
    if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      source[0] = (gint8) ((gint16) (source[0]) + (gint16) (sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) * scale * osc_1_volume));
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      source[0] = (gint8) ((gint16) (source[0]) + ((fmod(((gdouble) (offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))), (samplerate / osc_1_frequency)) * 2.0 * osc_1_frequency / samplerate) - 1.0) * scale * osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      source[0] = (gint8) ((gint16) (source[0]) + (((((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * osc_1_frequency / samplerate * 2.0) - (((double) ((((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * osc_1_frequency / samplerate)) / 2.0) * 2.0) - 1.0) * scale * osc_1_volume));
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      source[0] = (gint8) ((gint16) (source[0]) + ((sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * scale * osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      source[0] = (gint8) ((gint16) (source[0]) + (sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * scale * osc_1_volume);
    }

    source += source_stride;
    i++;
  }

  /* pitch tuning */
  ags_fluid_interpolate_4th_order_util_config();

  source = modular_synth_util->source;

  phase_incr = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->phase_increment;

  base_key = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->base_key;
  tuning = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->tuning;

  offset = modular_synth_util->offset;

  i = 0;

  dsp_phase = 0;
  
  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index * source_stride];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  root_pitch_hz = exp2(((double) base_key - 48.0) / 12.0) * 440.0;

  //TODO:JK: implement me
  
  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gdouble phase_incr;

    gint row;
    
    main_pitch_tuning = modular_synth_util->pitch_tuning;

    /* env-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* env-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_0_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_1_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }
    
    /* noise to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((double *) modular_synth_util->noise_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    phase_incr = (exp2((((double) base_key - 48.0 + ((main_pitch_tuning) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    if(dsp_phase_index + 2 < buffer_length){
      pitch_buffer[dsp_i] = (coeffs_0 * start_point
			     + coeffs_1 * source[dsp_phase_index * source_stride]
			     + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
			     + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);
    }
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    offset += 1;
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gdouble phase_incr;

    gint row;
    
    main_pitch_tuning = modular_synth_util->pitch_tuning;

    /* env-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* env-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_0_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_1_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }
    
    /* noise to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((double *) modular_synth_util->noise_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    phase_incr = (exp2((((double) base_key - 48.0 + ((main_pitch_tuning) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    if(dsp_phase_index - 1 > 0 &&
       dsp_phase_index + 2 < buffer_length){
      pitch_buffer[dsp_i] = (coeffs_0 * source[(dsp_phase_index - 1) * source_stride]
			     + coeffs_1 * source[dsp_phase_index * source_stride]
			     + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
			     + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);
    }
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    offset += 1;
  }

  /* fill pitch */
  for(i = 0; i < buffer_length; i++){
    source[i * source_stride] = (gint8) pitch_buffer[i];
  }
  
  /* volume */
  source = modular_synth_util->source;

  offset = modular_synth_util->offset;

  i = 0;

  //TODO:JK: implement me

  for(; i < modular_synth_util->buffer_length;){
    main_volume = modular_synth_util->volume;
    
    /* env-1 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
					      modular_synth_util->env_1_sends,
					      AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (main_volume);
    }

    /* env-0 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
					      modular_synth_util->env_0_sends,
					      AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (main_volume);
    }

    /* lfo-0 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
					      modular_synth_util->lfo_0_sends,
					      AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }

    /* lfo-1 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
					      modular_synth_util->lfo_1_sends,
					      AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }
    
    /* noise to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
					      modular_synth_util->noise_sends,
					      AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }

    /* volume */
    source[0] = (gint8) (((double) source[0]) * main_volume);

    source += source_stride;
    i++;
  }
}

void
ags_modular_synth_util_compute_s16(AgsModularSynthUtil *modular_synth_util)
{
  gint16 *source, *tmp_source;

  guint source_stride;
  guint buffer_length;
  guint samplerate;

  guint nth_sends;
  gboolean env_0_has_sends;
  gboolean env_1_has_sends;
  gboolean lfo_0_has_sends;
  gboolean lfo_1_has_sends;
  gboolean noise_has_sends;

  gdouble env_volume;
  gdouble env_amount;

  gdouble osc_0_frequency;
  gdouble osc_0_phase;
  gdouble osc_0_volume;

  gdouble osc_1_frequency;
  gdouble osc_1_phase;
  gdouble osc_1_volume;

  gdouble main_pitch_tuning;

  gdouble *pitch_buffer;

  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;

  gdouble phase_incr;
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;
    
  gdouble main_volume;

  gdouble volume;
  gdouble tmp_volume, end_volume;

  guint offset;
  guint tmp_offset, end_offset;
  guint compute_frame_count;
  guint frame_count;
  guint i, i_stop;

  static const gdouble scale = 32767.0;
  
  if(modular_synth_util == NULL ||
     modular_synth_util->source == NULL){
    return;
  }

  source = modular_synth_util->source;

  source_stride = modular_synth_util->source_stride;

  buffer_length = modular_synth_util->buffer_length;

  samplerate = modular_synth_util->samplerate;

  if(samplerate == 0){
    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  }
  
  osc_0_frequency = modular_synth_util->osc_0_frequency;
  osc_0_phase = modular_synth_util->osc_0_phase;
  osc_0_volume = modular_synth_util->osc_0_volume;
  
  if(osc_0_frequency == 0.0){
    osc_0_frequency = 0.01;
  }

  osc_1_frequency = modular_synth_util->osc_1_frequency;
  osc_1_phase = modular_synth_util->osc_1_phase;
  osc_1_volume = modular_synth_util->osc_1_volume;
  
  if(osc_1_frequency == 0.0){
    osc_1_frequency = 0.01;
  }
  
  pitch_buffer = modular_synth_util->pitch_buffer;
  
  frame_count = modular_synth_util->frame_count;
  offset = modular_synth_util->offset;

  i = 0;

  /* env-0 */
  env_0_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->env_0_sends[nth_sends] != 0){
      env_0_has_sends = TRUE;

      break;
    }
  }

  if(env_0_has_sends){
    for(i = 0; i < modular_synth_util->buffer_length; i++){
      ((gdouble *) modular_synth_util->env_0_buffer)[i] = 1.0;
    }
    
    ags_envelope_util_set_source(modular_synth_util->env_0_util,
				 modular_synth_util->env_0_buffer);

    ags_envelope_util_set_destination(modular_synth_util->env_0_util,
				      modular_synth_util->env_0_buffer);

    ags_envelope_util_set_buffer_length(modular_synth_util->env_0_util,
					modular_synth_util->buffer_length);

    ags_envelope_util_set_format(modular_synth_util->env_0_util,
				 AGS_SOUNDCARD_DOUBLE);

    tmp_offset = offset;

    compute_frame_count = ((double) frame_count / 4.0);

    while(tmp_offset < frame_count){
      /* env-0 attack */
      env_amount = 0.0;
      env_volume = 1.0;

      //      g_message("env-0 sends");    

      if((double) tmp_offset >= 0.0 &&
	 (double) tmp_offset < (double) frame_count / 4.0){
	env_volume = modular_synth_util->env_0_attack;

	end_offset = ((double) frame_count / 4.0);

	end_volume = modular_synth_util->env_0_decay;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - 0);

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 4.0 &&
	       (double) tmp_offset < (double) frame_count / 2.0){
	env_volume = modular_synth_util->env_0_decay;

	end_offset = ((double) frame_count / 2.0);

	end_volume = modular_synth_util->env_0_sustain;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 2.0 &&
	       (double) tmp_offset < (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_0_sustain;

	end_offset = ((double) frame_count * 3.0 / 4.0);

	end_volume = modular_synth_util->env_0_release;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 2.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_0_release;

	end_offset = ((double) frame_count);

	end_volume = modular_synth_util->env_0_release;
      
	if(end_offset == 0 ||
	   end_offset >= frame_count){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count * 3.0 / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else{
	env_amount = 0.0;
	env_volume = modular_synth_util->env_0_release;
      }

      if((tmp_offset - offset) + compute_frame_count > modular_synth_util->buffer_length){
	compute_frame_count = modular_synth_util->buffer_length - (tmp_offset - offset);
      }else{
	env_amount = 0.0;
	env_volume = modular_synth_util->env_0_release;
      }

      if(frame_count - tmp_offset > 0){
	compute_frame_count = frame_count - tmp_offset;
      }else{
	compute_frame_count = ((double) frame_count / 4.0);
      }
      
      if(compute_frame_count > (guint) ((double) frame_count / 4.0)){
	compute_frame_count = (guint) ((double) frame_count / 4.0);
      }
      
      if((tmp_offset - offset) + compute_frame_count > modular_synth_util->buffer_length){
	compute_frame_count = modular_synth_util->buffer_length - (tmp_offset - offset);
      }
      
      if(tmp_offset > 0){
	if(tmp_offset - offset >= modular_synth_util->buffer_length){
	  break;
	}
      }

      //      g_message("compute frame count -> %d\noffset -> %d\ntmp_offset -> %d", compute_frame_count, offset, tmp_offset);      

      ags_envelope_util_set_buffer_length(modular_synth_util->env_0_util,
					  compute_frame_count);

      ags_envelope_util_set_source(modular_synth_util->env_0_util,
				   ((gdouble *) modular_synth_util->env_0_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_destination(modular_synth_util->env_0_util,
					((gdouble *) modular_synth_util->env_0_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_offset(modular_synth_util->env_0_util,
				   0);

      ags_envelope_util_set_volume(modular_synth_util->env_0_util,
				   env_volume);

      ags_envelope_util_set_amount(modular_synth_util->env_0_util,
				   env_amount);

      ags_envelope_util_compute(modular_synth_util->env_0_util);

      /* iterate */
      tmp_offset += (guint) compute_frame_count;
    }
  }

  /* env-1 */
  env_1_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->env_1_sends[nth_sends] != 0){
      env_1_has_sends = TRUE;

      break;
    }
  }

  if(env_1_has_sends){
    for(i = 0; i < modular_synth_util->buffer_length; i++){
      ((gdouble *) modular_synth_util->env_1_buffer)[i] = 1.0;
    }
    
    ags_envelope_util_set_source(modular_synth_util->env_1_util,
				 modular_synth_util->env_1_buffer);

    ags_envelope_util_set_destination(modular_synth_util->env_1_util,
				      modular_synth_util->env_1_buffer);

    ags_envelope_util_set_buffer_length(modular_synth_util->env_1_util,
					modular_synth_util->buffer_length);

    ags_envelope_util_set_format(modular_synth_util->env_1_util,
				 AGS_SOUNDCARD_DOUBLE);

    tmp_offset = offset;

    compute_frame_count = ((double) frame_count / 4.0);
    
    while(tmp_offset < frame_count){
      /* env-1 attack */
      env_amount = 0.0;
      env_volume = 1.0;

      //      g_message("env-1 sends");

      if((double) tmp_offset >= 0.0 &&
	 (double) tmp_offset < (double) frame_count / 4.0){
	env_volume = modular_synth_util->env_1_attack;

	end_offset = ((double) frame_count / 4.0);

	end_volume = modular_synth_util->env_1_decay;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - 0);

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 4.0 &&
	       (double) tmp_offset < (double) frame_count / 2.0){
	env_volume = modular_synth_util->env_1_decay;

	end_offset = ((double) frame_count / 2.0);

	end_volume = modular_synth_util->env_1_sustain;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 2.0 &&
	       (double) tmp_offset < (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_1_sustain;

	end_offset = ((double) frame_count * 3.0 / 4.0);

	end_volume = modular_synth_util->env_1_release;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 2.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_1_release;

	end_offset = ((double) frame_count);

	end_volume = modular_synth_util->env_1_release;
      
	if(end_offset == 0 ||
	   end_offset >= frame_count){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count * 3.0 / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else{
	env_amount = 0.0;
	env_volume = modular_synth_util->env_0_release;
      }

      if((tmp_offset - offset) + compute_frame_count > modular_synth_util->buffer_length){
	compute_frame_count = modular_synth_util->buffer_length - (tmp_offset - offset);
      }else{
	env_amount = 0.0;
	env_volume = modular_synth_util->env_0_release;
      }

      if(frame_count - tmp_offset > 0){
	compute_frame_count = frame_count - tmp_offset;
      }else{
	compute_frame_count = ((double) frame_count / 4.0);
      }
      
      if(compute_frame_count > (guint) ((double) frame_count / 4.0)){
	compute_frame_count = (guint) ((double) frame_count / 4.0);
      }
      
      if((tmp_offset - offset) + compute_frame_count > modular_synth_util->buffer_length){
	compute_frame_count = modular_synth_util->buffer_length - (tmp_offset - offset);
      }
      
      if(tmp_offset > 0){
	if(tmp_offset - offset >= modular_synth_util->buffer_length){
	  break;
	}
      }

      //      g_message("compute frame count -> %d\noffset -> %d\ntmp_offset -> %d", compute_frame_count, offset, tmp_offset);      
      
      ags_envelope_util_set_buffer_length(modular_synth_util->env_1_util,
					  compute_frame_count);

      ags_envelope_util_set_source(modular_synth_util->env_1_util,
				   ((gdouble *) modular_synth_util->env_1_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_destination(modular_synth_util->env_1_util,
					((gdouble *) modular_synth_util->env_1_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_offset(modular_synth_util->env_1_util,
				   tmp_offset);

      ags_envelope_util_set_volume(modular_synth_util->env_1_util,
				   env_volume);

      ags_envelope_util_set_amount(modular_synth_util->env_1_util,
				   env_amount);

      ags_envelope_util_compute(modular_synth_util->env_1_util);

      /* iterate */
      tmp_offset += (guint) compute_frame_count;
    }
  }

  /* LFO-0 */
  lfo_0_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->lfo_0_sends[nth_sends] != 0){
      lfo_0_has_sends = TRUE;

      break;
    }
  }

  if(lfo_0_has_sends){
    //    g_message("lfo-0 sends");
    
    ags_lfo_synth_util_set_source(modular_synth_util->lfo_0_util,
				  modular_synth_util->lfo_0_buffer);

    ags_lfo_synth_util_set_buffer_length(modular_synth_util->lfo_0_util,
					 modular_synth_util->buffer_length);

    ags_lfo_synth_util_set_format(modular_synth_util->lfo_0_util,
				  AGS_SOUNDCARD_DOUBLE);

    ags_lfo_synth_util_set_samplerate(modular_synth_util->lfo_0_util,
				      samplerate);

    ags_lfo_synth_util_set_lfo_synth_oscillator_mode(modular_synth_util->lfo_0_util,
						     modular_synth_util->lfo_0_oscillator);

    ags_lfo_synth_util_set_frequency(modular_synth_util->lfo_0_util,
				     modular_synth_util->lfo_0_frequency);

    ags_lfo_synth_util_set_lfo_depth(modular_synth_util->lfo_0_util,
				     modular_synth_util->lfo_0_depth);
  
    ags_lfo_synth_util_set_tuning(modular_synth_util->lfo_0_util,
				      modular_synth_util->lfo_0_tuning);

    ags_lfo_synth_util_set_offset(modular_synth_util->lfo_0_util,
				  offset);

    switch(modular_synth_util->lfo_0_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
      {
	ags_lfo_synth_util_compute_sin(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
      {
	ags_lfo_synth_util_compute_sawtooth(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
      {
	ags_lfo_synth_util_compute_triangle(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
      {
	ags_lfo_synth_util_compute_square(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
      {
	ags_lfo_synth_util_compute_impulse(modular_synth_util->lfo_0_util);
      }
      break;
    }
  }

  /* LFO-1 */
  lfo_1_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->lfo_1_sends[nth_sends] != 0){
      lfo_1_has_sends = TRUE;

      break;
    }
  }

  if(lfo_1_has_sends){
    //    g_message("lfo-1 sends");
    
    ags_lfo_synth_util_set_source(modular_synth_util->lfo_1_util,
				  modular_synth_util->lfo_1_buffer);

    ags_lfo_synth_util_set_buffer_length(modular_synth_util->lfo_1_util,
					 modular_synth_util->buffer_length);

    ags_lfo_synth_util_set_format(modular_synth_util->lfo_1_util,
				  AGS_SOUNDCARD_DOUBLE);

    ags_lfo_synth_util_set_samplerate(modular_synth_util->lfo_1_util,
				      samplerate);

    ags_lfo_synth_util_set_lfo_synth_oscillator_mode(modular_synth_util->lfo_1_util,
						     modular_synth_util->lfo_1_oscillator);

    ags_lfo_synth_util_set_frequency(modular_synth_util->lfo_1_util,
				     modular_synth_util->lfo_1_frequency);

    ags_lfo_synth_util_set_lfo_depth(modular_synth_util->lfo_1_util,
				     modular_synth_util->lfo_1_depth);
  
    ags_lfo_synth_util_set_tuning(modular_synth_util->lfo_1_util,
				  modular_synth_util->lfo_1_tuning);

    ags_lfo_synth_util_set_offset(modular_synth_util->lfo_1_util,
				  offset);

    switch(modular_synth_util->lfo_1_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
      {
	ags_lfo_synth_util_compute_sin(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
      {
	ags_lfo_synth_util_compute_sawtooth(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
      {
	ags_lfo_synth_util_compute_triangle(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
      {
	ags_lfo_synth_util_compute_square(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
      {
	ags_lfo_synth_util_compute_impulse(modular_synth_util->lfo_1_util);
      }
      break;
    }
  }
  
  /* noise */
  noise_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->noise_sends[nth_sends] != 0){
      noise_has_sends = TRUE;

      break;
    }
  }

  if(noise_has_sends){
    //    g_message("noise sends");
    
    ags_noise_util_set_source(modular_synth_util->noise_util,
			      modular_synth_util->noise_buffer);

    ags_noise_util_set_buffer_length(modular_synth_util->noise_util,
				     modular_synth_util->buffer_length);

    ags_noise_util_set_format(modular_synth_util->noise_util,
			      AGS_SOUNDCARD_DOUBLE);

    ags_noise_util_set_samplerate(modular_synth_util->noise_util,
				  samplerate);

    ags_noise_util_set_volume(modular_synth_util->noise_util,
			      modular_synth_util->noise_gain);

    ags_noise_util_set_frequency(modular_synth_util->noise_util,
				 modular_synth_util->noise_frequency);

    ags_noise_util_set_offset(modular_synth_util->noise_util,
			      offset);

    ags_noise_util_compute(modular_synth_util->noise_util);
  }

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = modular_synth_util->buffer_length - (modular_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_sine;
    ags_v8double v_sawtooth;
    ags_v8double v_triangle;
    ags_v8double v_square;
    ags_v8double v_impulse;
    ags_v8double v_buffer;

    ags_v8double v_osc_0_frequency;
    ags_v8double tmp_v_osc_0_frequency;
    ags_v8double v_osc_0_phase;
    ags_v8double tmp_v_osc_0_phase;
    ags_v8double v_osc_0_volume;
    ags_v8double tmp_v_osc_0_volume;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride)
    };

    v_osc_0_frequency = (ags_v8double) {
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency
    };

    v_osc_0_phase = (ags_v8double) {
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase
    };

    v_osc_0_volume = (ags_v8double) {
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume
    };

    /* env-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      //      g_message("env-0 to osc-0 frequency");
      
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_frequency = tmp_v_osc_0_frequency * v_osc_0_frequency;
    }
      
    /* env-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      //      g_message("env-0 to osc-0 phase");
      
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_phase = (tmp_v_osc_0_phase * 2.0 * M_PI) + v_osc_0_phase;
    }

    /* env-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      //      g_message("env-0 to osc-0 volume");
      
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_volume = tmp_v_osc_0_volume * v_osc_0_volume;
    }

    /* env-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      //      g_message("env-1 to osc-0 frequency");
      
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_frequency = tmp_v_osc_0_frequency * v_osc_0_frequency;
    }

    /* env-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      //      g_message("env-1 to osc-0 phase");
      
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_phase = (tmp_v_osc_0_phase * 2.0 * M_PI) + v_osc_0_phase;
    }

    /* env-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      //      g_message("env-1 to osc-0 volume");
      
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_volume = tmp_v_osc_0_volume * v_osc_0_volume;
    }

    /* lfo-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      //      g_message("lfo-0 to osc-0 frequency");

      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }
      
    /* lfo-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      g_message("lfo-0 to osc-0 phase");

      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* lfo-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      //      g_message("lfo-0 to osc-0 volume");

      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* lfo-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      //      g_message("lfo-1 to osc-0 frequency");

      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }

    /* lfo-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      //      g_message("lfo-1 to osc-0 phase");

      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* lfo-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      //      g_message("lfo-1 to osc-0 volume");

      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* noise to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }

    /* noise to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* noise to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* compute osc-0 */
    if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      v_sine = (ags_v8double) {
	(gdouble) (sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate))
      };
    
      v_buffer += (v_sine * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      v_sawtooth = (ags_v8double) {
	((fmod(((gdouble) (offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[0] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))), (samplerate / v_osc_0_frequency[1])) * 2.0 * v_osc_0_frequency[1] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))), (samplerate / v_osc_0_frequency[2])) * 2.0 * v_osc_0_frequency[2] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))), (samplerate / v_osc_0_frequency[3])) * 2.0 * v_osc_0_frequency[3] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))), (samplerate / v_osc_0_frequency[4])) * 2.0 * v_osc_0_frequency[4] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))), (samplerate / v_osc_0_frequency[5])) * 2.0 * v_osc_0_frequency[5] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))), (samplerate / v_osc_0_frequency[6])) * 2.0 * v_osc_0_frequency[6] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))), (samplerate / v_osc_0_frequency[7])) * 2.0 * v_osc_0_frequency[7] / samplerate) - 1.0)
      };
    
      v_buffer += (v_sawtooth * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      v_triangle = (ags_v8double) {
	(((((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * v_osc_0_frequency[0] / samplerate * 2.0) - (((double) ((((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * v_osc_0_frequency[0] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * v_osc_0_frequency[1] / samplerate * 2.0) - (((double) ((((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * v_osc_0_frequency[1] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * v_osc_0_frequency[2] / samplerate * 2.0) - (((double) ((((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * v_osc_0_frequency[2] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * v_osc_0_frequency[3] / samplerate * 2.0) - (((double) ((((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * v_osc_0_frequency[3] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * v_osc_0_frequency[4] / samplerate * 2.0) - (((double) ((((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * v_osc_0_frequency[4] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * v_osc_0_frequency[5] / samplerate * 2.0) - (((double) ((((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * v_osc_0_frequency[5] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * v_osc_0_frequency[6] / samplerate * 2.0) - (((double) ((((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * v_osc_0_frequency[6] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * v_osc_0_frequency[7] / samplerate * 2.0) - (((double) ((((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * v_osc_0_frequency[7] / samplerate)) / 2.0) * 2.0) - 1.0)),
      };
    
      v_buffer += (v_triangle * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      v_square = (ags_v8double) {
	((sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
      };
    
      v_buffer += (v_square * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      v_impulse = (ags_v8double) {
	(sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0)
      };
    
      v_buffer += (v_impulse * scale * v_osc_0_volume);
    }
    
    *(source) = (gint16) v_buffer[0];
    *(source += source_stride) = (gint16) v_buffer[1];
    *(source += source_stride) = (gint16) v_buffer[2];
    *(source += source_stride) = (gint16) v_buffer[3];
    *(source += source_stride) = (gint16) v_buffer[4];
    *(source += source_stride) = (gint16) v_buffer[5];
    *(source += source_stride) = (gint16) v_buffer[6];
    *(source += source_stride) = (gint16) v_buffer[7];

    source += source_stride;

    i += 8;
  }
#endif
  
  for(; i < modular_synth_util->buffer_length;){
    /* OSC-0 */
    osc_0_frequency = modular_synth_util->osc_0_frequency;

    osc_0_phase = modular_synth_util->osc_0_phase;

    osc_0_volume = modular_synth_util->osc_0_volume;

    /* env-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_0_frequency);
    }

    /* env-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = (((gdouble *) modular_synth_util->env_0_buffer)[i] * 2.0 * M_PI) + (osc_0_phase);
    }

    /* env-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_0_volume);
    }

    /* env-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (osc_0_frequency);
    }

    /* env-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = (((gdouble *) modular_synth_util->env_1_buffer)[i] * 2.0 * M_PI) + (osc_0_phase);
    }

    /* env-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (modular_synth_util->osc_0_volume);
    }

    /* lfo-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* lfo-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* lfo-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_0_volume);
    }

    /* lfo-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* lfo-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* lfo-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_0_volume);
    }

    /* noise to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* noise to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* noise to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (modular_synth_util->osc_0_volume);
    }

   /* compute osc-0 */
    if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      source[0] = (gint16) ((gint32) (source[0]) + (gint32) (sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) * scale * osc_0_volume));
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      source[0] = (gint16) ((gint32) (source[0]) + ((fmod(((gdouble) (offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))), (samplerate / osc_0_frequency)) * 2.0 * osc_0_frequency / samplerate) - 1.0) * scale * osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      source[0] = (gint16) ((gint32) (source[0]) + (((((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * osc_0_frequency / samplerate * 2.0) - (((double) ((((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * osc_0_frequency / samplerate)) / 2.0) * 2.0) - 1.0) * scale * osc_0_volume));
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      source[0] = (gint16) ((gint32) (source[0]) + ((sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * scale * osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      source[0] = (gint16) ((gint32) (source[0]) + (sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * scale * osc_0_volume);
    }

    source += source_stride;
    i++;
  }

  /* OSC-1 */
  source = modular_synth_util->source;

  offset = modular_synth_util->offset;

  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = modular_synth_util->buffer_length - (modular_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_sine;
    ags_v8double v_sawtooth;
    ags_v8double v_triangle;
    ags_v8double v_square;
    ags_v8double v_impulse;
    ags_v8double v_buffer;

    ags_v8double v_osc_1_frequency;
    ags_v8double tmp_v_osc_1_frequency;
    ags_v8double v_osc_1_phase;
    ags_v8double tmp_v_osc_1_phase;
    ags_v8double v_osc_1_volume;
    ags_v8double tmp_v_osc_1_volume;
  
    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride)
    };

    v_osc_1_frequency = (ags_v8double) {
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency
    };

    v_osc_1_phase = (ags_v8double) {
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase
    };

    v_osc_1_volume = (ags_v8double) {
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume
    };

    /* env-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }
      
    /* env-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* env-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* env-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* env-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* env-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* lfo-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }
      
    /* lfo-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* lfo-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* lfo-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* lfo-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* lfo-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* noise to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* noise to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* noise to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7],
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* compute osc-1 */
    if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      v_sine = (ags_v8double) {
	(gdouble) (sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate))
      };
    
      v_buffer += (v_sine * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      v_sawtooth = (ags_v8double) {
	((fmod(((gdouble) (offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[0] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))), (samplerate / v_osc_1_frequency[1])) * 2.0 * v_osc_1_frequency[1] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))), (samplerate / v_osc_1_frequency[2])) * 2.0 * v_osc_1_frequency[2] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))), (samplerate / v_osc_1_frequency[3])) * 2.0 * v_osc_1_frequency[3] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))), (samplerate / v_osc_1_frequency[4])) * 2.0 * v_osc_1_frequency[4] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))), (samplerate / v_osc_1_frequency[5])) * 2.0 * v_osc_1_frequency[5] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))), (samplerate / v_osc_1_frequency[6])) * 2.0 * v_osc_1_frequency[6] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))), (samplerate / v_osc_1_frequency[7])) * 2.0 * v_osc_1_frequency[7] / samplerate) - 1.0)
      };
    
      v_buffer += (v_sawtooth * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      v_triangle = (ags_v8double) {
	(((((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * v_osc_1_frequency[0] / samplerate * 2.0) - (((double) ((((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * v_osc_1_frequency[0] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * v_osc_1_frequency[1] / samplerate * 2.0) - (((double) ((((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * v_osc_1_frequency[1] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * v_osc_1_frequency[2] / samplerate * 2.0) - (((double) ((((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * v_osc_1_frequency[2] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * v_osc_1_frequency[3] / samplerate * 2.0) - (((double) ((((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * v_osc_1_frequency[3] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * v_osc_1_frequency[4] / samplerate * 2.0) - (((double) ((((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * v_osc_1_frequency[4] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * v_osc_1_frequency[5] / samplerate * 2.0) - (((double) ((((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * v_osc_1_frequency[5] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * v_osc_1_frequency[6] / samplerate * 2.0) - (((double) ((((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * v_osc_1_frequency[6] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * v_osc_1_frequency[7] / samplerate * 2.0) - (((double) ((((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * v_osc_1_frequency[7] / samplerate)) / 2.0) * 2.0) - 1.0)),
      };
    
      v_buffer += (v_triangle * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      v_square = (ags_v8double) {
	((sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
      };
    
      v_buffer += (v_square * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      v_impulse = (ags_v8double) {
	(sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0)
      };
    
      v_buffer += (v_impulse * scale * v_osc_1_volume);
    }
    
    *(source) = (gint16) v_buffer[0];
    *(source += source_stride) = (gint16) v_buffer[1];
    *(source += source_stride) = (gint16) v_buffer[2];
    *(source += source_stride) = (gint16) v_buffer[3];
    *(source += source_stride) = (gint16) v_buffer[4];
    *(source += source_stride) = (gint16) v_buffer[5];
    *(source += source_stride) = (gint16) v_buffer[6];
    *(source += source_stride) = (gint16) v_buffer[7];

    source += source_stride;

    i += 8;
  }
#endif
  
  for(; i < modular_synth_util->buffer_length;){
    /* OSC-1 */
    osc_1_frequency = modular_synth_util->osc_1_frequency;

    osc_1_phase = modular_synth_util->osc_1_phase;

    osc_1_volume = modular_synth_util->osc_1_volume;

    /* env-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = (2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[i] - 1.0) * (osc_1_frequency);
    }

    /* env-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = (((gdouble *) modular_synth_util->env_0_buffer)[i] * 2.0 * M_PI) + (osc_1_phase);
    }

    /* env-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_1_volume);
    }

    /* env-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = (2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[i] - 1.0) * (osc_1_frequency);
    }

    /* env-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = (((gdouble *) modular_synth_util->env_1_buffer)[i] * 2.0 * M_PI) + (osc_1_phase);
    }

    /* env-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (osc_1_volume);
    }

    /* lfo-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* lfo-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* lfo-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_1_volume);
    }

    /* lfo-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* lfo-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* lfo-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_1_volume);
    }
    
    /* noise to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* noise to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* noise to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (modular_synth_util->osc_0_volume);
    }

    /* compute osc-1 */
    if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      source[0] = (gint16) ((gint32) (source[0]) + (gint32) (sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) * scale * osc_1_volume));
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      source[0] = (gint16) ((gint32) (source[0]) + ((fmod(((gdouble) (offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))), (samplerate / osc_1_frequency)) * 2.0 * osc_1_frequency / samplerate) - 1.0) * scale * osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      source[0] = (gint16) ((gint32) (source[0]) + (((((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * osc_1_frequency / samplerate * 2.0) - (((double) ((((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * osc_1_frequency / samplerate)) / 2.0) * 2.0) - 1.0) * scale * osc_1_volume));
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      source[0] = (gint16) ((gint32) (source[0]) + ((sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * scale * osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      source[0] = (gint16) ((gint32) (source[0]) + (sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * scale * osc_1_volume);
    }

    source += source_stride;
    i++;
  }

  /* pitch tuning */
  ags_fluid_interpolate_4th_order_util_config();

  source = modular_synth_util->source;

  phase_incr = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->phase_increment;

  base_key = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->base_key;
  tuning = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->tuning;

  offset = modular_synth_util->offset;

  i = 0;

  dsp_phase = 0;
  
  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index * source_stride];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  root_pitch_hz = exp2(((double) base_key - 48.0) / 12.0) * 440.0;

  //TODO:JK: implement me
  
  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gdouble phase_incr;

    gint row;
    
    main_pitch_tuning = modular_synth_util->pitch_tuning;

    /* env-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* env-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_0_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_1_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }
    
    /* noise to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((double *) modular_synth_util->noise_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    phase_incr = (exp2((((double) base_key - 48.0 + ((main_pitch_tuning) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    if(dsp_phase_index + 2 < buffer_length){
      pitch_buffer[dsp_i] = (coeffs_0 * start_point
			     + coeffs_1 * source[dsp_phase_index * source_stride]
			     + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
			     + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);
    }
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    offset += 1;
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gdouble phase_incr;

    gint row;
    
    main_pitch_tuning = modular_synth_util->pitch_tuning;

    /* env-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* env-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_0_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_1_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }
    
    /* noise to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((double *) modular_synth_util->noise_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    phase_incr = (exp2((((double) base_key - 48.0 + ((main_pitch_tuning) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    if(dsp_phase_index - 1 > 0 &&
       dsp_phase_index + 2 < buffer_length){
      pitch_buffer[dsp_i] = (coeffs_0 * source[(dsp_phase_index - 1) * source_stride]
			     + coeffs_1 * source[dsp_phase_index * source_stride]
			     + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
			     + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);
    }
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    offset += 1;
  }

  /* fill pitch */
  for(i = 0; i < buffer_length; i++){
    source[i * source_stride] = (gint16) pitch_buffer[i];
  }
  
  /* volume */
  source = modular_synth_util->source;

  offset = modular_synth_util->offset;

  i = 0;

  //TODO:JK: implement me

  for(; i < modular_synth_util->buffer_length;){
    main_volume = modular_synth_util->volume;
    
    /* env-1 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (main_volume);
    }

    /* env-0 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (main_volume);
    }

    /* lfo-0 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }

    /* lfo-1 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }
    
    /* noise to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }

    /* volume */
    source[0] = (gint16) (((double) source[0]) * main_volume);

    source += source_stride;
    i++;
  }
}

void
ags_modular_synth_util_compute_s24(AgsModularSynthUtil *modular_synth_util)
{
  gint32 *source, *tmp_source;

  guint source_stride;
  guint buffer_length;
  guint samplerate;

  guint nth_sends;
  gboolean env_0_has_sends;
  gboolean env_1_has_sends;
  gboolean lfo_0_has_sends;
  gboolean lfo_1_has_sends;
  gboolean noise_has_sends;

  gdouble env_volume;
  gdouble env_amount;

  gdouble osc_0_frequency;
  gdouble osc_0_phase;
  gdouble osc_0_volume;

  gdouble osc_1_frequency;
  gdouble osc_1_phase;
  gdouble osc_1_volume;

  gdouble main_pitch_tuning;

  gdouble *pitch_buffer;

  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;

  gdouble phase_incr;
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;
    
  gdouble main_volume;

  gdouble volume;
  gdouble tmp_volume, end_volume;

  guint offset;
  guint tmp_offset, end_offset;
  guint compute_frame_count;
  guint frame_count;
  guint i, i_stop;

  static const gdouble scale = 8388607.0;
  
  if(modular_synth_util == NULL ||
     modular_synth_util->source == NULL){
    return;
  }

  source = modular_synth_util->source;

  source_stride = modular_synth_util->source_stride;

  buffer_length = modular_synth_util->buffer_length;

  samplerate = modular_synth_util->samplerate;

  if(samplerate == 0){
    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  }
  
  osc_0_frequency = modular_synth_util->osc_0_frequency;
  osc_0_phase = modular_synth_util->osc_0_phase;
  osc_0_volume = modular_synth_util->osc_0_volume;
  
  if(osc_0_frequency == 0.0){
    osc_0_frequency = 0.01;
  }

  osc_1_frequency = modular_synth_util->osc_1_frequency;
  osc_1_phase = modular_synth_util->osc_1_phase;
  osc_1_volume = modular_synth_util->osc_1_volume;
  
  if(osc_1_frequency == 0.0){
    osc_1_frequency = 0.01;
  }
  
  pitch_buffer = modular_synth_util->pitch_buffer;
  
  frame_count = modular_synth_util->frame_count;
  offset = modular_synth_util->offset;

  i = 0;

  /* env-0 */
  env_0_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->env_0_sends[nth_sends] != 0){
      env_0_has_sends = TRUE;

      break;
    }
  }

  if(env_0_has_sends){
    for(i = 0; i < modular_synth_util->buffer_length; i++){
      ((gdouble *) modular_synth_util->env_0_buffer)[i] = 1.0;
    }
    
    ags_envelope_util_set_source(modular_synth_util->env_0_util,
				 modular_synth_util->env_0_buffer);

    ags_envelope_util_set_destination(modular_synth_util->env_0_util,
				      modular_synth_util->env_0_buffer);

    ags_envelope_util_set_buffer_length(modular_synth_util->env_0_util,
					modular_synth_util->buffer_length);

    ags_envelope_util_set_format(modular_synth_util->env_0_util,
				 AGS_SOUNDCARD_DOUBLE);

    tmp_offset = offset;

    compute_frame_count = ((double) frame_count / 4.0);

    while(tmp_offset < frame_count){
      /* env-0 attack */
      env_amount = 0.0;
      env_volume = 1.0;

      if((double) tmp_offset >= 0.0 &&
	 (double) tmp_offset < (double) frame_count / 4.0){
	env_volume = modular_synth_util->env_0_attack;

	end_offset = ((double) frame_count / 4.0);

	end_volume = modular_synth_util->env_0_decay;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - 0);

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 4.0 &&
	       (double) tmp_offset < (double) frame_count / 2.0){
	env_volume = modular_synth_util->env_0_decay;

	end_offset = ((double) frame_count / 2.0);

	end_volume = modular_synth_util->env_0_sustain;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 2.0 &&
	       (double) tmp_offset < (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_0_sustain;

	end_offset = ((double) frame_count * 3.0 / 4.0);

	end_volume = modular_synth_util->env_0_release;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 2.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_0_release;

	end_offset = ((double) frame_count);

	end_volume = modular_synth_util->env_0_release;
      
	if(end_offset == 0 ||
	   end_offset >= frame_count){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count * 3.0 / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else{
	env_amount = 0.0;
	env_volume = modular_synth_util->env_0_release;
      }

      if(frame_count - tmp_offset > 0){
	compute_frame_count = frame_count - tmp_offset;
      }else{
	compute_frame_count = ((double) frame_count / 4.0);
      }
      
      if(compute_frame_count > (guint) ((double) frame_count / 4.0)){
	compute_frame_count = (guint) ((double) frame_count / 4.0);
      }
      
      if((tmp_offset - offset) + compute_frame_count > modular_synth_util->buffer_length){
	compute_frame_count = modular_synth_util->buffer_length - (tmp_offset - offset);
      }
      
      if(tmp_offset > 0){
	if(tmp_offset - offset >= modular_synth_util->buffer_length){
	  break;
	}
      }

      ags_envelope_util_set_buffer_length(modular_synth_util->env_0_util,
					  compute_frame_count);

      ags_envelope_util_set_source(modular_synth_util->env_0_util,
				   ((gdouble *) modular_synth_util->env_0_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_destination(modular_synth_util->env_0_util,
					((gdouble *) modular_synth_util->env_0_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_offset(modular_synth_util->env_0_util,
				   0);

      ags_envelope_util_set_volume(modular_synth_util->env_0_util,
				   env_volume);

      ags_envelope_util_set_amount(modular_synth_util->env_0_util,
				   env_amount);

      ags_envelope_util_compute(modular_synth_util->env_0_util);

      /* iterate */
      tmp_offset += (guint) compute_frame_count;
    }
  }

  /* env-1 */
  env_1_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->env_1_sends[nth_sends] != 0){
      env_1_has_sends = TRUE;

      break;
    }
  }

  if(env_1_has_sends){
    for(i = 0; i < modular_synth_util->buffer_length; i++){
      ((gdouble *) modular_synth_util->env_1_buffer)[i] = 1.0;
    }
    
    ags_envelope_util_set_source(modular_synth_util->env_1_util,
				 modular_synth_util->env_1_buffer);

    ags_envelope_util_set_destination(modular_synth_util->env_1_util,
				      modular_synth_util->env_1_buffer);

    ags_envelope_util_set_buffer_length(modular_synth_util->env_1_util,
					modular_synth_util->buffer_length);

    ags_envelope_util_set_format(modular_synth_util->env_1_util,
				 AGS_SOUNDCARD_DOUBLE);

    tmp_offset = offset;

    compute_frame_count = ((double) frame_count / 4.0);
    
    while(tmp_offset < frame_count){
      /* env-1 attack */
      env_amount = 0.0;
      env_volume = 1.0;

      if((double) tmp_offset >= 0.0 &&
	 (double) tmp_offset < (double) frame_count / 4.0){
	env_volume = modular_synth_util->env_1_attack;

	end_offset = ((double) frame_count / 4.0);

	end_volume = modular_synth_util->env_1_decay;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - 0);

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 4.0 &&
	       (double) tmp_offset < (double) frame_count / 2.0){
	env_volume = modular_synth_util->env_1_decay;

	end_offset = ((double) frame_count / 2.0);

	end_volume = modular_synth_util->env_1_sustain;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 2.0 &&
	       (double) tmp_offset < (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_1_sustain;

	end_offset = ((double) frame_count * 3.0 / 4.0);

	end_volume = modular_synth_util->env_1_release;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 2.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_1_release;

	end_offset = ((double) frame_count);

	end_volume = modular_synth_util->env_1_release;
      
	if(end_offset == 0 ||
	   end_offset >= frame_count){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count * 3.0 / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else{
	env_amount = 0.0;
	env_volume = modular_synth_util->env_0_release;
      }

      if(frame_count - tmp_offset > 0){
	compute_frame_count = frame_count - tmp_offset;
      }else{
	compute_frame_count = ((double) frame_count / 4.0);
      }
      
      if(compute_frame_count > (guint) ((double) frame_count / 4.0)){
	compute_frame_count = (guint) ((double) frame_count / 4.0);
      }
      
      if((tmp_offset - offset) + compute_frame_count > modular_synth_util->buffer_length){
	compute_frame_count = modular_synth_util->buffer_length - (tmp_offset - offset);
      }
      
      if(tmp_offset > 0){
	if(tmp_offset - offset >= modular_synth_util->buffer_length){
	  break;
	}
      }

      ags_envelope_util_set_buffer_length(modular_synth_util->env_1_util,
					  compute_frame_count);

      ags_envelope_util_set_source(modular_synth_util->env_1_util,
				   ((gdouble *) modular_synth_util->env_1_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_destination(modular_synth_util->env_1_util,
					((gdouble *) modular_synth_util->env_1_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_offset(modular_synth_util->env_1_util,
				   tmp_offset);

      ags_envelope_util_set_volume(modular_synth_util->env_1_util,
				   env_volume);

      ags_envelope_util_set_amount(modular_synth_util->env_1_util,
				   env_amount);

      ags_envelope_util_compute(modular_synth_util->env_1_util);

      /* iterate */
      tmp_offset += (guint) compute_frame_count;
    }
  }

  /* LFO-0 */
  lfo_0_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->lfo_0_sends[nth_sends] != 0){
      lfo_0_has_sends = TRUE;

      break;
    }
  }

  if(lfo_0_has_sends){
    ags_lfo_synth_util_set_source(modular_synth_util->lfo_0_util,
				  modular_synth_util->lfo_0_buffer);

    ags_lfo_synth_util_set_buffer_length(modular_synth_util->lfo_0_util,
					 modular_synth_util->buffer_length);

    ags_lfo_synth_util_set_format(modular_synth_util->lfo_0_util,
				  AGS_SOUNDCARD_DOUBLE);

    ags_lfo_synth_util_set_samplerate(modular_synth_util->lfo_0_util,
				      samplerate);

    ags_lfo_synth_util_set_lfo_synth_oscillator_mode(modular_synth_util->lfo_0_util,
						     modular_synth_util->lfo_0_oscillator);

    ags_lfo_synth_util_set_frequency(modular_synth_util->lfo_0_util,
				     modular_synth_util->lfo_0_frequency);

    ags_lfo_synth_util_set_lfo_depth(modular_synth_util->lfo_0_util,
				     modular_synth_util->lfo_0_depth);
  
    ags_lfo_synth_util_set_tuning(modular_synth_util->lfo_0_util,
				      modular_synth_util->lfo_0_tuning);

    ags_lfo_synth_util_set_offset(modular_synth_util->lfo_0_util,
				  offset);

    switch(modular_synth_util->lfo_0_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
      {
	ags_lfo_synth_util_compute_sin(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
      {
	ags_lfo_synth_util_compute_sawtooth(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
      {
	ags_lfo_synth_util_compute_triangle(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
      {
	ags_lfo_synth_util_compute_square(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
      {
	ags_lfo_synth_util_compute_impulse(modular_synth_util->lfo_0_util);
      }
      break;
    }
  }

  /* LFO-1 */
  lfo_1_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->lfo_1_sends[nth_sends] != 0){
      lfo_1_has_sends = TRUE;

      break;
    }
  }

  if(lfo_1_has_sends){
    ags_lfo_synth_util_set_source(modular_synth_util->lfo_1_util,
				  modular_synth_util->lfo_1_buffer);

    ags_lfo_synth_util_set_buffer_length(modular_synth_util->lfo_1_util,
					 modular_synth_util->buffer_length);

    ags_lfo_synth_util_set_format(modular_synth_util->lfo_1_util,
				  AGS_SOUNDCARD_DOUBLE);

    ags_lfo_synth_util_set_samplerate(modular_synth_util->lfo_1_util,
				      samplerate);

    ags_lfo_synth_util_set_lfo_synth_oscillator_mode(modular_synth_util->lfo_1_util,
						     modular_synth_util->lfo_1_oscillator);

    ags_lfo_synth_util_set_frequency(modular_synth_util->lfo_1_util,
				     modular_synth_util->lfo_1_frequency);

    ags_lfo_synth_util_set_lfo_depth(modular_synth_util->lfo_1_util,
				     modular_synth_util->lfo_1_depth);
  
    ags_lfo_synth_util_set_tuning(modular_synth_util->lfo_1_util,
				  modular_synth_util->lfo_1_tuning);

    ags_lfo_synth_util_set_offset(modular_synth_util->lfo_1_util,
				  offset);

    switch(modular_synth_util->lfo_1_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
      {
	ags_lfo_synth_util_compute_sin(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
      {
	ags_lfo_synth_util_compute_sawtooth(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
      {
	ags_lfo_synth_util_compute_triangle(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
      {
	ags_lfo_synth_util_compute_square(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
      {
	ags_lfo_synth_util_compute_impulse(modular_synth_util->lfo_1_util);
      }
      break;
    }
  }
  
  /* noise */
  noise_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->noise_sends[nth_sends] != 0){
      noise_has_sends = TRUE;

      break;
    }
  }

  if(noise_has_sends){
    ags_noise_util_set_source(modular_synth_util->noise_util,
			      modular_synth_util->noise_buffer);

    ags_noise_util_set_buffer_length(modular_synth_util->noise_util,
				     modular_synth_util->buffer_length);

    ags_noise_util_set_format(modular_synth_util->noise_util,
			      AGS_SOUNDCARD_DOUBLE);

    ags_noise_util_set_samplerate(modular_synth_util->noise_util,
				  samplerate);

    ags_noise_util_set_volume(modular_synth_util->noise_util,
			      modular_synth_util->noise_gain);

    ags_noise_util_set_frequency(modular_synth_util->noise_util,
				 modular_synth_util->noise_frequency);

    ags_noise_util_set_offset(modular_synth_util->noise_util,
			      offset);

    ags_noise_util_compute(modular_synth_util->noise_util);
  }

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = modular_synth_util->buffer_length - (modular_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_sine;
    ags_v8double v_sawtooth;
    ags_v8double v_triangle;
    ags_v8double v_square;
    ags_v8double v_impulse;
    ags_v8double v_buffer;

    ags_v8double v_osc_0_frequency;
    ags_v8double tmp_v_osc_0_frequency;
    ags_v8double v_osc_0_phase;
    ags_v8double tmp_v_osc_0_phase;
    ags_v8double v_osc_0_volume;
    ags_v8double tmp_v_osc_0_volume;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride)
    };

    v_osc_0_frequency = (ags_v8double) {
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency
    };

    v_osc_0_phase = (ags_v8double) {
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase
    };

    v_osc_0_volume = (ags_v8double) {
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume
    };

    /* env-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_frequency = tmp_v_osc_0_frequency * v_osc_0_frequency;
    }
      
    /* env-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_phase = (tmp_v_osc_0_phase * 2.0 * M_PI) + v_osc_0_phase;
    }

    /* env-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_volume = tmp_v_osc_0_volume * v_osc_0_volume;
    }

    /* env-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_frequency = tmp_v_osc_0_frequency * v_osc_0_frequency;
    }

    /* env-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_phase = (tmp_v_osc_0_phase * 2.0 * M_PI) + v_osc_0_phase;
    }

    /* env-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_volume = tmp_v_osc_0_volume * v_osc_0_volume;
    }

    /* lfo-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }
      
    /* lfo-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* lfo-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* lfo-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }

    /* lfo-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* lfo-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* noise to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }

    /* noise to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* noise to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* compute osc-0 */
    if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      v_sine = (ags_v8double) {
	(gdouble) (sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate))
      };
    
      v_buffer += (v_sine * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      v_sawtooth = (ags_v8double) {
	((fmod(((gdouble) (offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[0] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))), (samplerate / v_osc_0_frequency[1])) * 2.0 * v_osc_0_frequency[1] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))), (samplerate / v_osc_0_frequency[2])) * 2.0 * v_osc_0_frequency[2] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))), (samplerate / v_osc_0_frequency[3])) * 2.0 * v_osc_0_frequency[3] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))), (samplerate / v_osc_0_frequency[4])) * 2.0 * v_osc_0_frequency[4] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))), (samplerate / v_osc_0_frequency[5])) * 2.0 * v_osc_0_frequency[5] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))), (samplerate / v_osc_0_frequency[6])) * 2.0 * v_osc_0_frequency[6] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))), (samplerate / v_osc_0_frequency[7])) * 2.0 * v_osc_0_frequency[7] / samplerate) - 1.0)
      };
    
      v_buffer += (v_sawtooth * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      v_triangle = (ags_v8double) {
	(((((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * v_osc_0_frequency[0] / samplerate * 2.0) - (((double) ((((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * v_osc_0_frequency[0] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * v_osc_0_frequency[1] / samplerate * 2.0) - (((double) ((((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * v_osc_0_frequency[1] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * v_osc_0_frequency[2] / samplerate * 2.0) - (((double) ((((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * v_osc_0_frequency[2] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * v_osc_0_frequency[3] / samplerate * 2.0) - (((double) ((((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * v_osc_0_frequency[3] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * v_osc_0_frequency[4] / samplerate * 2.0) - (((double) ((((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * v_osc_0_frequency[4] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * v_osc_0_frequency[5] / samplerate * 2.0) - (((double) ((((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * v_osc_0_frequency[5] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * v_osc_0_frequency[6] / samplerate * 2.0) - (((double) ((((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * v_osc_0_frequency[6] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * v_osc_0_frequency[7] / samplerate * 2.0) - (((double) ((((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * v_osc_0_frequency[7] / samplerate)) / 2.0) * 2.0) - 1.0)),
      };
    
      v_buffer += (v_triangle * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      v_square = (ags_v8double) {
	((sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
      };
    
      v_buffer += (v_square * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      v_impulse = (ags_v8double) {
	(sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0)
      };
    
      v_buffer += (v_impulse * scale * v_osc_0_volume);
    }
    
    *(source) = (gint32) v_buffer[0];
    *(source += source_stride) = (gint32) v_buffer[1];
    *(source += source_stride) = (gint32) v_buffer[2];
    *(source += source_stride) = (gint32) v_buffer[3];
    *(source += source_stride) = (gint32) v_buffer[4];
    *(source += source_stride) = (gint32) v_buffer[5];
    *(source += source_stride) = (gint32) v_buffer[6];
    *(source += source_stride) = (gint32) v_buffer[7];

    source += source_stride;

    i += 8;
  }
#endif
  
  for(; i < modular_synth_util->buffer_length;){
    /* OSC-0 */
    osc_0_frequency = modular_synth_util->osc_0_frequency;

    osc_0_phase = modular_synth_util->osc_0_phase;

    osc_0_volume = modular_synth_util->osc_0_volume;

    /* env-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_0_frequency);
    }

    /* env-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = (((gdouble *) modular_synth_util->env_0_buffer)[i] * 2.0 * M_PI) + (osc_0_phase);
    }

    /* env-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_0_volume);
    }

    /* env-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (osc_0_frequency);
    }

    /* env-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = (((gdouble *) modular_synth_util->env_1_buffer)[i] * 2.0 * M_PI) + (osc_0_phase);
    }

    /* env-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (modular_synth_util->osc_0_volume);
    }

    /* lfo-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* lfo-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* lfo-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_0_volume);
    }

    /* lfo-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* lfo-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* lfo-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_0_volume);
    }

    /* noise to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* noise to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* noise to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (modular_synth_util->osc_0_volume);
    }

    /* compute osc-0 */
    if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      source[0] = (gint32) ((gint32) (source[0]) + (gint32) (sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) * scale * osc_0_volume));
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      source[0] = (gint32) ((gint32) (source[0]) + ((fmod(((gdouble) (offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))), (samplerate / osc_0_frequency)) * 2.0 * osc_0_frequency / samplerate) - 1.0) * scale * osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      source[0] = (gint32) ((gint32) (source[0]) + (((((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * osc_0_frequency / samplerate * 2.0) - (((double) ((((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * osc_0_frequency / samplerate)) / 2.0) * 2.0) - 1.0) * scale * osc_0_volume));
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      source[0] = (gint32) ((gint32) (source[0]) + ((sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * scale * osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      source[0] = (gint32) ((gint32) (source[0]) + (sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * scale * osc_0_volume);
    }

    source += source_stride;
    i++;
  }

  /* OSC-1 */
  source = modular_synth_util->source;

  offset = modular_synth_util->offset;

  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = modular_synth_util->buffer_length - (modular_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_sine;
    ags_v8double v_sawtooth;
    ags_v8double v_triangle;
    ags_v8double v_square;
    ags_v8double v_impulse;
    ags_v8double v_buffer;

    ags_v8double v_osc_1_frequency;
    ags_v8double tmp_v_osc_1_frequency;
    ags_v8double v_osc_1_phase;
    ags_v8double tmp_v_osc_1_phase;
    ags_v8double v_osc_1_volume;
    ags_v8double tmp_v_osc_1_volume;
  
    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride)
    };

    v_osc_1_frequency = (ags_v8double) {
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency
    };

    v_osc_1_phase = (ags_v8double) {
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase
    };

    v_osc_1_volume = (ags_v8double) {
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume
    };

    /* env-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }
      
    /* env-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* env-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* env-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* env-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* env-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* lfo-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }
      
    /* lfo-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* lfo-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* lfo-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* lfo-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* lfo-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* noise to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* noise to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* noise to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7],
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* compute osc-1 */
    if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      v_sine = (ags_v8double) {
	(gdouble) (sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate))
      };
    
      v_buffer += (v_sine * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      v_sawtooth = (ags_v8double) {
	((fmod(((gdouble) (offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[0] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[1] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[2] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[3] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[4] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[5] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[6] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[7] / samplerate) - 1.0)
      };
    
      v_buffer += (v_sawtooth * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      v_triangle = (ags_v8double) {
	(((((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * v_osc_1_frequency[0] / samplerate * 2.0) - (((double) ((((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * v_osc_1_frequency[0] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * v_osc_1_frequency[1] / samplerate * 2.0) - (((double) ((((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * v_osc_1_frequency[1] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * v_osc_1_frequency[2] / samplerate * 2.0) - (((double) ((((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * v_osc_1_frequency[2] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * v_osc_1_frequency[3] / samplerate * 2.0) - (((double) ((((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * v_osc_1_frequency[3] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * v_osc_1_frequency[4] / samplerate * 2.0) - (((double) ((((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * v_osc_1_frequency[4] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * v_osc_1_frequency[5] / samplerate * 2.0) - (((double) ((((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * v_osc_1_frequency[5] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * v_osc_1_frequency[6] / samplerate * 2.0) - (((double) ((((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * v_osc_1_frequency[6] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * v_osc_1_frequency[7] / samplerate * 2.0) - (((double) ((((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * v_osc_1_frequency[7] / samplerate)) / 2.0) * 2.0) - 1.0)),
      };
    
      v_buffer += (v_triangle * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      v_square = (ags_v8double) {
	((sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
      };
    
      v_buffer += (v_square * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      v_impulse = (ags_v8double) {
	(sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0)
      };
    
      v_buffer += (v_impulse * scale * v_osc_1_volume);
    }
    
    *(source) = (gint32) v_buffer[0];
    *(source += source_stride) = (gint32) v_buffer[1];
    *(source += source_stride) = (gint32) v_buffer[2];
    *(source += source_stride) = (gint32) v_buffer[3];
    *(source += source_stride) = (gint32) v_buffer[4];
    *(source += source_stride) = (gint32) v_buffer[5];
    *(source += source_stride) = (gint32) v_buffer[6];
    *(source += source_stride) = (gint32) v_buffer[7];

    source += source_stride;

    i += 8;
  }
#endif
  
  for(; i < modular_synth_util->buffer_length;){
    /* OSC-1 */
    osc_1_frequency = modular_synth_util->osc_1_frequency;

    osc_1_phase = modular_synth_util->osc_1_phase;

    osc_1_volume = modular_synth_util->osc_1_volume;

    /* env-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = (2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[i] - 1.0) * (osc_1_frequency);
    }

    /* env-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = (((gdouble *) modular_synth_util->env_0_buffer)[i] * 2.0 * M_PI) + (osc_1_phase);
    }

    /* env-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_1_volume);
    }

    /* env-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = (2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[i] - 1.0) * (osc_1_frequency);
    }

    /* env-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = (((gdouble *) modular_synth_util->env_1_buffer)[i] * 2.0 * M_PI) + (osc_1_phase);
    }

    /* env-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (osc_1_volume);
    }

    /* lfo-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* lfo-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* lfo-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_1_volume);
    }

    /* lfo-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* lfo-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* lfo-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_1_volume);
    }
    
    /* noise to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* noise to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* noise to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (modular_synth_util->osc_0_volume);
    }

    /* compute osc-1 */
    if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      source[0] = (gint32) ((gint32) (source[0]) + (gint32) (sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) * scale * osc_1_volume));
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      source[0] = (gint32) ((gint32) (source[0]) + ((fmod(((gdouble) (offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))), (samplerate / osc_1_frequency)) * 2.0 * osc_1_frequency / samplerate) - 1.0) * scale * osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      source[0] = (gint32) ((gint32) (source[0]) + (((((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * osc_1_frequency / samplerate * 2.0) - (((double) ((((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * osc_1_frequency / samplerate)) / 2.0) * 2.0) - 1.0) * scale * osc_1_volume));
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      source[0] = (gint32) ((gint32) (source[0]) + ((sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * scale * osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      source[0] = (gint32) ((gint32) (source[0]) + (sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * scale * osc_1_volume);
    }

    source += source_stride;
    i++;
  }

  /* pitch tuning */
  ags_fluid_interpolate_4th_order_util_config();

  source = modular_synth_util->source;

  phase_incr = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->phase_increment;

  base_key = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->base_key;
  tuning = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->tuning;

  offset = modular_synth_util->offset;

  i = 0;

  dsp_phase = 0;
  
  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index * source_stride];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  root_pitch_hz = exp2(((double) base_key - 48.0) / 12.0) * 440.0;

  //TODO:JK: implement me
  
  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gdouble phase_incr;

    gint row;
    
    main_pitch_tuning = modular_synth_util->pitch_tuning;

    /* env-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* env-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_0_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_1_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }
    
    /* noise to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((double *) modular_synth_util->noise_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    phase_incr = (exp2((((double) base_key - 48.0 + ((main_pitch_tuning) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    if(dsp_phase_index + 2 < buffer_length){
      pitch_buffer[dsp_i] = (coeffs_0 * start_point
			     + coeffs_1 * source[dsp_phase_index * source_stride]
			     + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
			     + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);
    }
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    offset += 1;
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gdouble phase_incr;

    gint row;
    
    main_pitch_tuning = modular_synth_util->pitch_tuning;

    /* env-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* env-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_0_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_1_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }
    
    /* noise to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((double *) modular_synth_util->noise_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    phase_incr = (exp2((((double) base_key - 48.0 + ((main_pitch_tuning) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    if(dsp_phase_index - 1 > 0 &&
       dsp_phase_index + 2 < buffer_length){
      pitch_buffer[dsp_i] = (coeffs_0 * source[(dsp_phase_index - 1) * source_stride]
			     + coeffs_1 * source[dsp_phase_index * source_stride]
			     + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
			     + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);
    }
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    offset += 1;
  }

  /* fill pitch */
  for(i = 0; i < buffer_length; i++){
    source[i * source_stride] = (gint32) pitch_buffer[i];
  }
  
  /* volume */
  source = modular_synth_util->source;

  offset = modular_synth_util->offset;

  i = 0;

  //TODO:JK: implement me

  for(; i < modular_synth_util->buffer_length;){
    main_volume = modular_synth_util->volume;
    
    /* env-1 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (main_volume);
    }

    /* env-0 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (main_volume);
    }

    /* lfo-0 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }

    /* lfo-1 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }
    
    /* noise to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }

    /* volume */
    source[0] = (gint32) (((double) source[0]) * main_volume);

    source += source_stride;
    i++;
  }
}

void
ags_modular_synth_util_compute_s32(AgsModularSynthUtil *modular_synth_util)
{
  gint32 *source, *tmp_source;

  guint source_stride;
  guint buffer_length;
  guint samplerate;

  guint nth_sends;
  gboolean env_0_has_sends;
  gboolean env_1_has_sends;
  gboolean lfo_0_has_sends;
  gboolean lfo_1_has_sends;
  gboolean noise_has_sends;

  gdouble env_volume;
  gdouble env_amount;

  gdouble osc_0_frequency;
  gdouble osc_0_phase;
  gdouble osc_0_volume;

  gdouble osc_1_frequency;
  gdouble osc_1_phase;
  gdouble osc_1_volume;

  gdouble main_pitch_tuning;

  gdouble *pitch_buffer;

  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;

  gdouble phase_incr;
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;
    
  gdouble main_volume;

  gdouble volume;
  gdouble tmp_volume, end_volume;

  guint offset;
  guint tmp_offset, end_offset;
  guint compute_frame_count;
  guint frame_count;
  guint i, i_stop;

  static const gdouble scale = 214748363.0;
  
  if(modular_synth_util == NULL ||
     modular_synth_util->source == NULL){
    return;
  }

  source = modular_synth_util->source;

  source_stride = modular_synth_util->source_stride;

  buffer_length = modular_synth_util->buffer_length;

  samplerate = modular_synth_util->samplerate;

  if(samplerate == 0){
    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  }
  
  osc_0_frequency = modular_synth_util->osc_0_frequency;
  osc_0_phase = modular_synth_util->osc_0_phase;
  osc_0_volume = modular_synth_util->osc_0_volume;
  
  if(osc_0_frequency == 0.0){
    osc_0_frequency = 0.01;
  }

  osc_1_frequency = modular_synth_util->osc_1_frequency;
  osc_1_phase = modular_synth_util->osc_1_phase;
  osc_1_volume = modular_synth_util->osc_1_volume;
  
  if(osc_1_frequency == 0.0){
    osc_1_frequency = 0.01;
  }
  
  pitch_buffer = modular_synth_util->pitch_buffer;
  
  frame_count = modular_synth_util->frame_count;
  offset = modular_synth_util->offset;

  i = 0;

  /* env-0 */
  env_0_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->env_0_sends[nth_sends] != 0){
      env_0_has_sends = TRUE;

      break;
    }
  }

  if(env_0_has_sends){
    for(i = 0; i < modular_synth_util->buffer_length; i++){
      ((gdouble *) modular_synth_util->env_0_buffer)[i] = 1.0;
    }
    
    ags_envelope_util_set_source(modular_synth_util->env_0_util,
				 modular_synth_util->env_0_buffer);

    ags_envelope_util_set_destination(modular_synth_util->env_0_util,
				      modular_synth_util->env_0_buffer);

    ags_envelope_util_set_buffer_length(modular_synth_util->env_0_util,
					modular_synth_util->buffer_length);

    ags_envelope_util_set_format(modular_synth_util->env_0_util,
				 AGS_SOUNDCARD_DOUBLE);

    tmp_offset = offset;

    compute_frame_count = ((double) frame_count / 4.0);

    while(tmp_offset < frame_count){
      /* env-0 attack */
      env_amount = 0.0;
      env_volume = 1.0;

      if((double) tmp_offset >= 0.0 &&
	 (double) tmp_offset < (double) frame_count / 4.0){
	env_volume = modular_synth_util->env_0_attack;

	end_offset = ((double) frame_count / 4.0);

	end_volume = modular_synth_util->env_0_decay;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - 0);

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 4.0 &&
	       (double) tmp_offset < (double) frame_count / 2.0){
	env_volume = modular_synth_util->env_0_decay;

	end_offset = ((double) frame_count / 2.0);

	end_volume = modular_synth_util->env_0_sustain;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 2.0 &&
	       (double) tmp_offset < (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_0_sustain;

	end_offset = ((double) frame_count * 3.0 / 4.0);

	end_volume = modular_synth_util->env_0_release;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 2.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_0_release;

	end_offset = ((double) frame_count);

	end_volume = modular_synth_util->env_0_release;
      
	if(end_offset == 0 ||
	   end_offset >= frame_count){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count * 3.0 / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else{
	env_amount = 0.0;
	env_volume = modular_synth_util->env_0_release;
      }

      if(frame_count - tmp_offset > 0){
	compute_frame_count = frame_count - tmp_offset;
      }else{
	compute_frame_count = ((double) frame_count / 4.0);
      }
      
      if(compute_frame_count > (guint) ((double) frame_count / 4.0)){
	compute_frame_count = (guint) ((double) frame_count / 4.0);
      }
      
      if((tmp_offset - offset) + compute_frame_count > modular_synth_util->buffer_length){
	compute_frame_count = modular_synth_util->buffer_length - (tmp_offset - offset);
      }
      
      if(tmp_offset > 0){
	if(tmp_offset - offset >= modular_synth_util->buffer_length){
	  break;
	}
      }

      ags_envelope_util_set_buffer_length(modular_synth_util->env_0_util,
					  compute_frame_count);

      ags_envelope_util_set_source(modular_synth_util->env_0_util,
				   ((gdouble *) modular_synth_util->env_0_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_destination(modular_synth_util->env_0_util,
					((gdouble *) modular_synth_util->env_0_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_offset(modular_synth_util->env_0_util,
				   0);

      ags_envelope_util_set_volume(modular_synth_util->env_0_util,
				   env_volume);

      ags_envelope_util_set_amount(modular_synth_util->env_0_util,
				   env_amount);

      ags_envelope_util_compute(modular_synth_util->env_0_util);

      /* iterate */
      tmp_offset += (guint) compute_frame_count;
    }
  }

  /* env-1 */
  env_1_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->env_1_sends[nth_sends] != 0){
      env_1_has_sends = TRUE;

      break;
    }
  }

  if(env_1_has_sends){
    for(i = 0; i < modular_synth_util->buffer_length; i++){
      ((gdouble *) modular_synth_util->env_1_buffer)[i] = 1.0;
    }
    
    ags_envelope_util_set_source(modular_synth_util->env_1_util,
				 modular_synth_util->env_1_buffer);

    ags_envelope_util_set_destination(modular_synth_util->env_1_util,
				      modular_synth_util->env_1_buffer);

    ags_envelope_util_set_buffer_length(modular_synth_util->env_1_util,
					modular_synth_util->buffer_length);

    ags_envelope_util_set_format(modular_synth_util->env_1_util,
				 AGS_SOUNDCARD_DOUBLE);

    tmp_offset = offset;

    compute_frame_count = ((double) frame_count / 4.0);
    
    while(tmp_offset < frame_count){
      /* env-1 attack */
      env_amount = 0.0;
      env_volume = 1.0;

      if((double) tmp_offset >= 0.0 &&
	 (double) tmp_offset < (double) frame_count / 4.0){
	env_volume = modular_synth_util->env_1_attack;

	end_offset = ((double) frame_count / 4.0);

	end_volume = modular_synth_util->env_1_decay;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - 0);

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 4.0 &&
	       (double) tmp_offset < (double) frame_count / 2.0){
	env_volume = modular_synth_util->env_1_decay;

	end_offset = ((double) frame_count / 2.0);

	end_volume = modular_synth_util->env_1_sustain;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 2.0 &&
	       (double) tmp_offset < (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_1_sustain;

	end_offset = ((double) frame_count * 3.0 / 4.0);

	end_volume = modular_synth_util->env_1_release;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 2.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_1_release;

	end_offset = ((double) frame_count);

	end_volume = modular_synth_util->env_1_release;
      
	if(end_offset == 0 ||
	   end_offset >= frame_count){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count * 3.0 / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else{
	env_amount = 0.0;
	env_volume = modular_synth_util->env_0_release;
      }

      if(frame_count - tmp_offset > 0){
	compute_frame_count = frame_count - tmp_offset;
      }else{
	compute_frame_count = ((double) frame_count / 4.0);
      }
      
      if(compute_frame_count > (guint) ((double) frame_count / 4.0)){
	compute_frame_count = (guint) ((double) frame_count / 4.0);
      }
      
      if((tmp_offset - offset) + compute_frame_count > modular_synth_util->buffer_length){
	compute_frame_count = modular_synth_util->buffer_length - (tmp_offset - offset);
      }
      
      if(tmp_offset > 0){
	if(tmp_offset - offset >= modular_synth_util->buffer_length){
	  break;
	}
      }

      ags_envelope_util_set_buffer_length(modular_synth_util->env_1_util,
					  compute_frame_count);

      ags_envelope_util_set_source(modular_synth_util->env_1_util,
				   ((gdouble *) modular_synth_util->env_1_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_destination(modular_synth_util->env_1_util,
					((gdouble *) modular_synth_util->env_1_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_offset(modular_synth_util->env_1_util,
				   tmp_offset);

      ags_envelope_util_set_volume(modular_synth_util->env_1_util,
				   env_volume);

      ags_envelope_util_set_amount(modular_synth_util->env_1_util,
				   env_amount);

      ags_envelope_util_compute(modular_synth_util->env_1_util);

      /* iterate */
      tmp_offset += (guint) compute_frame_count;
    }
  }

  /* LFO-0 */
  lfo_0_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->lfo_0_sends[nth_sends] != 0){
      lfo_0_has_sends = TRUE;

      break;
    }
  }

  if(lfo_0_has_sends){
    ags_lfo_synth_util_set_source(modular_synth_util->lfo_0_util,
				  modular_synth_util->lfo_0_buffer);

    ags_lfo_synth_util_set_buffer_length(modular_synth_util->lfo_0_util,
					 modular_synth_util->buffer_length);

    ags_lfo_synth_util_set_format(modular_synth_util->lfo_0_util,
				  AGS_SOUNDCARD_DOUBLE);

    ags_lfo_synth_util_set_samplerate(modular_synth_util->lfo_0_util,
				      samplerate);

    ags_lfo_synth_util_set_lfo_synth_oscillator_mode(modular_synth_util->lfo_0_util,
						     modular_synth_util->lfo_0_oscillator);

    ags_lfo_synth_util_set_frequency(modular_synth_util->lfo_0_util,
				     modular_synth_util->lfo_0_frequency);

    ags_lfo_synth_util_set_lfo_depth(modular_synth_util->lfo_0_util,
				     modular_synth_util->lfo_0_depth);
  
    ags_lfo_synth_util_set_tuning(modular_synth_util->lfo_0_util,
				      modular_synth_util->lfo_0_tuning);

    ags_lfo_synth_util_set_offset(modular_synth_util->lfo_0_util,
				  offset);

    switch(modular_synth_util->lfo_0_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
      {
	ags_lfo_synth_util_compute_sin(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
      {
	ags_lfo_synth_util_compute_sawtooth(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
      {
	ags_lfo_synth_util_compute_triangle(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
      {
	ags_lfo_synth_util_compute_square(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
      {
	ags_lfo_synth_util_compute_impulse(modular_synth_util->lfo_0_util);
      }
      break;
    }
  }

  /* LFO-1 */
  lfo_1_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->lfo_1_sends[nth_sends] != 0){
      lfo_1_has_sends = TRUE;

      break;
    }
  }

  if(lfo_1_has_sends){
    ags_lfo_synth_util_set_source(modular_synth_util->lfo_1_util,
				  modular_synth_util->lfo_1_buffer);

    ags_lfo_synth_util_set_buffer_length(modular_synth_util->lfo_1_util,
					 modular_synth_util->buffer_length);

    ags_lfo_synth_util_set_format(modular_synth_util->lfo_1_util,
				  AGS_SOUNDCARD_DOUBLE);

    ags_lfo_synth_util_set_samplerate(modular_synth_util->lfo_1_util,
				      samplerate);

    ags_lfo_synth_util_set_lfo_synth_oscillator_mode(modular_synth_util->lfo_1_util,
						     modular_synth_util->lfo_1_oscillator);

    ags_lfo_synth_util_set_frequency(modular_synth_util->lfo_1_util,
				     modular_synth_util->lfo_1_frequency);

    ags_lfo_synth_util_set_lfo_depth(modular_synth_util->lfo_1_util,
				     modular_synth_util->lfo_1_depth);
  
    ags_lfo_synth_util_set_tuning(modular_synth_util->lfo_1_util,
				  modular_synth_util->lfo_1_tuning);

    ags_lfo_synth_util_set_offset(modular_synth_util->lfo_1_util,
				  offset);

    switch(modular_synth_util->lfo_1_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
      {
	ags_lfo_synth_util_compute_sin(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
      {
	ags_lfo_synth_util_compute_sawtooth(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
      {
	ags_lfo_synth_util_compute_triangle(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
      {
	ags_lfo_synth_util_compute_square(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
      {
	ags_lfo_synth_util_compute_impulse(modular_synth_util->lfo_1_util);
      }
      break;
    }
  }
  
  /* noise */
  noise_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->noise_sends[nth_sends] != 0){
      noise_has_sends = TRUE;

      break;
    }
  }

  if(noise_has_sends){
    ags_noise_util_set_source(modular_synth_util->noise_util,
			      modular_synth_util->noise_buffer);

    ags_noise_util_set_buffer_length(modular_synth_util->noise_util,
				     modular_synth_util->buffer_length);

    ags_noise_util_set_format(modular_synth_util->noise_util,
			      AGS_SOUNDCARD_DOUBLE);

    ags_noise_util_set_samplerate(modular_synth_util->noise_util,
				  samplerate);

    ags_noise_util_set_volume(modular_synth_util->noise_util,
			      modular_synth_util->noise_gain);

    ags_noise_util_set_frequency(modular_synth_util->noise_util,
				 modular_synth_util->noise_frequency);

    ags_noise_util_set_offset(modular_synth_util->noise_util,
			      offset);

    ags_noise_util_compute(modular_synth_util->noise_util);
  }

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = modular_synth_util->buffer_length - (modular_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_sine;
    ags_v8double v_sawtooth;
    ags_v8double v_triangle;
    ags_v8double v_square;
    ags_v8double v_impulse;
    ags_v8double v_buffer;

    ags_v8double v_osc_0_frequency;
    ags_v8double tmp_v_osc_0_frequency;
    ags_v8double v_osc_0_phase;
    ags_v8double tmp_v_osc_0_phase;
    ags_v8double v_osc_0_volume;
    ags_v8double tmp_v_osc_0_volume;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride)
    };

    v_osc_0_frequency = (ags_v8double) {
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency
    };

    v_osc_0_phase = (ags_v8double) {
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase
    };

    v_osc_0_volume = (ags_v8double) {
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume
    };

    /* env-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_frequency = tmp_v_osc_0_frequency * v_osc_0_frequency;
    }
      
    /* env-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_phase = (tmp_v_osc_0_phase * 2.0 * M_PI) + v_osc_0_phase;
    }

    /* env-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_volume = tmp_v_osc_0_volume * v_osc_0_volume;
    }

    /* env-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_frequency = tmp_v_osc_0_frequency * v_osc_0_frequency;
    }

    /* env-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_phase = (tmp_v_osc_0_phase * 2.0 * M_PI) + v_osc_0_phase;
    }

    /* env-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_volume = tmp_v_osc_0_volume * v_osc_0_volume;
    }

    /* lfo-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }
      
    /* lfo-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* lfo-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* lfo-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }

    /* lfo-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* lfo-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* noise to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }

    /* noise to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* noise to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* compute osc-0 */
    if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      v_sine = (ags_v8double) {
	(gdouble) (sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate))
      };
    
      v_buffer += (v_sine * scale * v_osc_0_volume);;
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      v_sawtooth = (ags_v8double) {
	((fmod(((gdouble) (offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[0] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))), (samplerate / v_osc_0_frequency[1])) * 2.0 * v_osc_0_frequency[1] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))), (samplerate / v_osc_0_frequency[2])) * 2.0 * v_osc_0_frequency[2] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))), (samplerate / v_osc_0_frequency[3])) * 2.0 * v_osc_0_frequency[3] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))), (samplerate / v_osc_0_frequency[4])) * 2.0 * v_osc_0_frequency[4] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))), (samplerate / v_osc_0_frequency[5])) * 2.0 * v_osc_0_frequency[5] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))), (samplerate / v_osc_0_frequency[6])) * 2.0 * v_osc_0_frequency[6] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))), (samplerate / v_osc_0_frequency[7])) * 2.0 * v_osc_0_frequency[7] / samplerate) - 1.0)
      };
    
      v_buffer += (v_sawtooth * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      v_triangle = (ags_v8double) {
	(((((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * v_osc_0_frequency[0] / samplerate * 2.0) - (((double) ((((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * v_osc_0_frequency[0] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * v_osc_0_frequency[1] / samplerate * 2.0) - (((double) ((((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * v_osc_0_frequency[1] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * v_osc_0_frequency[2] / samplerate * 2.0) - (((double) ((((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * v_osc_0_frequency[2] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * v_osc_0_frequency[3] / samplerate * 2.0) - (((double) ((((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * v_osc_0_frequency[3] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * v_osc_0_frequency[4] / samplerate * 2.0) - (((double) ((((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * v_osc_0_frequency[4] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * v_osc_0_frequency[5] / samplerate * 2.0) - (((double) ((((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * v_osc_0_frequency[5] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * v_osc_0_frequency[6] / samplerate * 2.0) - (((double) ((((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * v_osc_0_frequency[6] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * v_osc_0_frequency[7] / samplerate * 2.0) - (((double) ((((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * v_osc_0_frequency[7] / samplerate)) / 2.0) * 2.0) - 1.0)),
      };
    
      v_buffer += (v_triangle * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      v_square = (ags_v8double) {
	((sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
      };
    
      v_buffer += (v_square * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      v_impulse = (ags_v8double) {
	(sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0)
      };
    
      v_buffer += (v_impulse * scale * v_osc_0_volume);
    }
    
    *(source) = (gint32) v_buffer[0];
    *(source += source_stride) = (gint32) v_buffer[1];
    *(source += source_stride) = (gint32) v_buffer[2];
    *(source += source_stride) = (gint32) v_buffer[3];
    *(source += source_stride) = (gint32) v_buffer[4];
    *(source += source_stride) = (gint32) v_buffer[5];
    *(source += source_stride) = (gint32) v_buffer[6];
    *(source += source_stride) = (gint32) v_buffer[7];

    source += source_stride;

    i += 8;
  }
#endif
  
  for(; i < modular_synth_util->buffer_length;){
    /* OSC-0 */
    osc_0_frequency = modular_synth_util->osc_0_frequency;

    osc_0_phase = modular_synth_util->osc_0_phase;

    osc_0_volume = modular_synth_util->osc_0_volume;

    /* env-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_0_frequency);
    }

    /* env-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = (((gdouble *) modular_synth_util->env_0_buffer)[i] * 2.0 * M_PI) + (osc_0_phase);
    }

    /* env-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_0_volume);
    }

    /* env-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (osc_0_frequency);
    }

    /* env-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = (((gdouble *) modular_synth_util->env_1_buffer)[i] * 2.0 * M_PI) + (osc_0_phase);
    }

    /* env-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (modular_synth_util->osc_0_volume);
    }

    /* lfo-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* lfo-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* lfo-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_0_volume);
    }

    /* lfo-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* lfo-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* lfo-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_0_volume);
    }

    /* noise to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* noise to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* noise to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (modular_synth_util->osc_0_volume);
    }

    /* compute osc-0 */
    if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      source[0] = (gint32) ((gint64) (source[0]) + (gint64) (sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) * scale * osc_0_volume));
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      source[0] = (gint32) ((gint64) (source[0]) + ((fmod(((gdouble) (offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))), (samplerate / osc_0_frequency)) * 2.0 * osc_0_frequency / samplerate) - 1.0) * scale * osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      source[0] = (gint32) ((gint64) (source[0]) + (((((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * osc_0_frequency / samplerate * 2.0) - (((double) ((((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * osc_0_frequency / samplerate)) / 2.0) * 2.0) - 1.0) * scale * osc_0_volume));
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      source[0] = (gint32) ((gint64) (source[0]) + ((sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * scale * osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      source[0] = (gint32) ((gint64) (source[0]) + (sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * scale * osc_0_volume);
    }

    source += source_stride;
    i++;
  }

  /* OSC-1 */
  source = modular_synth_util->source;

  offset = modular_synth_util->offset;

  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = modular_synth_util->buffer_length - (modular_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_sine;
    ags_v8double v_sawtooth;
    ags_v8double v_triangle;
    ags_v8double v_square;
    ags_v8double v_impulse;
    ags_v8double v_buffer;

    ags_v8double v_osc_1_frequency;
    ags_v8double tmp_v_osc_1_frequency;
    ags_v8double v_osc_1_phase;
    ags_v8double tmp_v_osc_1_phase;
    ags_v8double v_osc_1_volume;
    ags_v8double tmp_v_osc_1_volume;
  
    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride)
    };

    v_osc_1_frequency = (ags_v8double) {
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency
    };

    v_osc_1_phase = (ags_v8double) {
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase
    };

    v_osc_1_volume = (ags_v8double) {
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume
    };

    /* env-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }
      
    /* env-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* env-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* env-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* env-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* env-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* lfo-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }
      
    /* lfo-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* lfo-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* lfo-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* lfo-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* lfo-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* noise to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* noise to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* noise to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7],
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* compute osc-1 */
    if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      v_sine = (ags_v8double) {
	(gdouble) (sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate))
      };
    
      v_buffer += (v_sine * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      v_sawtooth = (ags_v8double) {
	((fmod(((gdouble) (offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[0] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))), (samplerate / v_osc_1_frequency[1])) * 2.0 * v_osc_1_frequency[1] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))), (samplerate / v_osc_1_frequency[2])) * 2.0 * v_osc_1_frequency[2] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))), (samplerate / v_osc_1_frequency[3])) * 2.0 * v_osc_1_frequency[3] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))), (samplerate / v_osc_1_frequency[4])) * 2.0 * v_osc_1_frequency[4] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))), (samplerate / v_osc_1_frequency[5])) * 2.0 * v_osc_1_frequency[5] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))), (samplerate / v_osc_1_frequency[6])) * 2.0 * v_osc_1_frequency[6] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))), (samplerate / v_osc_1_frequency[7])) * 2.0 * v_osc_1_frequency[7] / samplerate) - 1.0)
      };
    
      v_buffer += (v_sawtooth * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      v_triangle = (ags_v8double) {
	(((((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * v_osc_1_frequency[0] / samplerate * 2.0) - (((double) ((((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * v_osc_1_frequency[0] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * v_osc_1_frequency[1] / samplerate * 2.0) - (((double) ((((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * v_osc_1_frequency[1] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * v_osc_1_frequency[2] / samplerate * 2.0) - (((double) ((((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * v_osc_1_frequency[2] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * v_osc_1_frequency[3] / samplerate * 2.0) - (((double) ((((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * v_osc_1_frequency[3] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * v_osc_1_frequency[4] / samplerate * 2.0) - (((double) ((((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * v_osc_1_frequency[4] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * v_osc_1_frequency[5] / samplerate * 2.0) - (((double) ((((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * v_osc_1_frequency[5] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * v_osc_1_frequency[6] / samplerate * 2.0) - (((double) ((((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * v_osc_1_frequency[6] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * v_osc_1_frequency[7] / samplerate * 2.0) - (((double) ((((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * v_osc_1_frequency[7] / samplerate)) / 2.0) * 2.0) - 1.0)),
      };
    
      v_buffer += (v_triangle * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      v_square = (ags_v8double) {
	((sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
      };
    
      v_buffer += (v_square * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      v_impulse = (ags_v8double) {
	(sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0)
      };
    
      v_buffer += (v_impulse * scale * v_osc_1_volume);
    }
    
    *(source) = (gint32) v_buffer[0];
    *(source += source_stride) = (gint32) v_buffer[1];
    *(source += source_stride) = (gint32) v_buffer[2];
    *(source += source_stride) = (gint32) v_buffer[3];
    *(source += source_stride) = (gint32) v_buffer[4];
    *(source += source_stride) = (gint32) v_buffer[5];
    *(source += source_stride) = (gint32) v_buffer[6];
    *(source += source_stride) = (gint32) v_buffer[7];

    source += source_stride;

    i += 8;
  }
#endif
  
  for(; i < modular_synth_util->buffer_length;){
    /* OSC-1 */
    osc_1_frequency = modular_synth_util->osc_1_frequency;

    osc_1_phase = modular_synth_util->osc_1_phase;

    osc_1_volume = modular_synth_util->osc_1_volume;

    /* env-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = (2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[i] - 1.0) * (osc_1_frequency);
    }

    /* env-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = (((gdouble *) modular_synth_util->env_0_buffer)[i] * 2.0 * M_PI) + (osc_1_phase);
    }

    /* env-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_1_volume);
    }

    /* env-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = (2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[i] - 1.0) * (osc_1_frequency);
    }

    /* env-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = (((gdouble *) modular_synth_util->env_1_buffer)[i] * 2.0 * M_PI) + (osc_1_phase);
    }

    /* env-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (osc_1_volume);
    }

    /* lfo-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* lfo-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* lfo-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_1_volume);
    }

    /* lfo-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* lfo-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* lfo-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_1_volume);
    }
    
    /* noise to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* noise to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* noise to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (modular_synth_util->osc_0_volume);
    }

    /* compute osc-1 */
    if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      source[0] = (gint32) ((gint64) (source[0]) + (gint64) (sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) * scale * osc_1_volume));
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      source[0] = (gint32) ((gint64) (source[0]) + ((fmod(((gdouble) (offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))), (samplerate / osc_1_frequency)) * 2.0 * osc_1_frequency / samplerate) - 1.0) * scale * osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      source[0] = (gint32) ((gint64) (source[0]) + (((((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * osc_1_frequency / samplerate * 2.0) - (((double) ((((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * osc_1_frequency / samplerate)) / 2.0) * 2.0) - 1.0) * scale * osc_1_volume));
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      source[0] = (gint32) ((gint64) (source[0]) + ((sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * scale * osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      source[0] = (gint32) ((gint64) (source[0]) + (sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * scale * osc_1_volume);
    }

    source += source_stride;
    i++;
  }

  /* pitch tuning */
  ags_fluid_interpolate_4th_order_util_config();

  source = modular_synth_util->source;

  phase_incr = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->phase_increment;

  base_key = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->base_key;
  tuning = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->tuning;

  offset = modular_synth_util->offset;

  i = 0;

  dsp_phase = 0;
  
  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index * source_stride];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  root_pitch_hz = exp2(((double) base_key - 48.0) / 12.0) * 440.0;

  //TODO:JK: implement me
  
  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gdouble phase_incr;

    gint row;
    
    main_pitch_tuning = modular_synth_util->pitch_tuning;

    /* env-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* env-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_0_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_1_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }
    
    /* noise to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((double *) modular_synth_util->noise_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    phase_incr = (exp2((((double) base_key - 48.0 + ((main_pitch_tuning) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    if(dsp_phase_index + 2 < buffer_length){
      pitch_buffer[dsp_i] = (coeffs_0 * start_point
			     + coeffs_1 * source[dsp_phase_index * source_stride]
			     + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
			     + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);
    }
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    offset += 1;
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gdouble phase_incr;

    gint row;
    
    main_pitch_tuning = modular_synth_util->pitch_tuning;

    /* env-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* env-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_0_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_1_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }
    
    /* noise to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((double *) modular_synth_util->noise_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    phase_incr = (exp2((((double) base_key - 48.0 + ((main_pitch_tuning) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    if(dsp_phase_index - 1 > 0 &&
       dsp_phase_index + 2 < buffer_length){
      pitch_buffer[dsp_i] = (coeffs_0 * source[(dsp_phase_index - 1) * source_stride]
			     + coeffs_1 * source[dsp_phase_index * source_stride]
			     + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
			     + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);
    }
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    offset += 1;
  }

  /* fill pitch */
  for(i = 0; i < buffer_length; i++){
    source[i * source_stride] = (gint32) pitch_buffer[i];
  }
  
  /* volume */
  source = modular_synth_util->source;

  offset = modular_synth_util->offset;

  i = 0;

  //TODO:JK: implement me

  for(; i < modular_synth_util->buffer_length;){
    main_volume = modular_synth_util->volume;
    
    /* env-1 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (main_volume);
    }

    /* env-0 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (main_volume);
    }

    /* lfo-0 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }

    /* lfo-1 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }
    
    /* noise to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }

    /* volume */
    source[0] = (gint32) (((double) source[0]) * main_volume);

    source += source_stride;
    i++;
  }
}

void
ags_modular_synth_util_compute_s64(AgsModularSynthUtil *modular_synth_util)
{
  gint64 *source, *tmp_source;

  guint source_stride;
  guint buffer_length;
  guint samplerate;

  guint nth_sends;
  gboolean env_0_has_sends;
  gboolean env_1_has_sends;
  gboolean lfo_0_has_sends;
  gboolean lfo_1_has_sends;
  gboolean noise_has_sends;

  gdouble env_volume;
  gdouble env_amount;

  gdouble osc_0_frequency;
  gdouble osc_0_phase;
  gdouble osc_0_volume;

  gdouble osc_1_frequency;
  gdouble osc_1_phase;
  gdouble osc_1_volume;

  gdouble main_pitch_tuning;

  gdouble *pitch_buffer;

  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;

  gdouble phase_incr;
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;
    
  gdouble main_volume;

  gdouble volume;
  gdouble tmp_volume, end_volume;

  guint offset;
  guint tmp_offset, end_offset;
  guint compute_frame_count;
  guint frame_count;
  guint i, i_stop;

  static const gdouble scale = 9223372036854775807.0;
  
  if(modular_synth_util == NULL ||
     modular_synth_util->source == NULL){
    return;
  }

  source = modular_synth_util->source;

  source_stride = modular_synth_util->source_stride;

  buffer_length = modular_synth_util->buffer_length;

  samplerate = modular_synth_util->samplerate;

  if(samplerate == 0){
    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  }
  
  osc_0_frequency = modular_synth_util->osc_0_frequency;
  osc_0_phase = modular_synth_util->osc_0_phase;
  osc_0_volume = modular_synth_util->osc_0_volume;
  
  if(osc_0_frequency == 0.0){
    osc_0_frequency = 0.01;
  }

  osc_1_frequency = modular_synth_util->osc_1_frequency;
  osc_1_phase = modular_synth_util->osc_1_phase;
  osc_1_volume = modular_synth_util->osc_1_volume;
  
  if(osc_1_frequency == 0.0){
    osc_1_frequency = 0.01;
  }
  
  pitch_buffer = modular_synth_util->pitch_buffer;
  
  frame_count = modular_synth_util->frame_count;
  offset = modular_synth_util->offset;

  i = 0;

  /* env-0 */
  env_0_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->env_0_sends[nth_sends] != 0){
      env_0_has_sends = TRUE;

      break;
    }
  }

  if(env_0_has_sends){
    for(i = 0; i < modular_synth_util->buffer_length; i++){
      ((gdouble *) modular_synth_util->env_0_buffer)[i] = 1.0;
    }
    
    ags_envelope_util_set_source(modular_synth_util->env_0_util,
				 modular_synth_util->env_0_buffer);

    ags_envelope_util_set_destination(modular_synth_util->env_0_util,
				      modular_synth_util->env_0_buffer);

    ags_envelope_util_set_buffer_length(modular_synth_util->env_0_util,
					modular_synth_util->buffer_length);

    ags_envelope_util_set_format(modular_synth_util->env_0_util,
				 AGS_SOUNDCARD_DOUBLE);

    tmp_offset = offset;

    compute_frame_count = ((double) frame_count / 4.0);

    while(tmp_offset < frame_count){
      /* env-0 attack */
      env_amount = 0.0;
      env_volume = 1.0;

      if((double) tmp_offset >= 0.0 &&
	 (double) tmp_offset < (double) frame_count / 4.0){
	env_volume = modular_synth_util->env_0_attack;

	end_offset = ((double) frame_count / 4.0);

	end_volume = modular_synth_util->env_0_decay;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - 0);

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 4.0 &&
	       (double) tmp_offset < (double) frame_count / 2.0){
	env_volume = modular_synth_util->env_0_decay;

	end_offset = ((double) frame_count / 2.0);

	end_volume = modular_synth_util->env_0_sustain;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 2.0 &&
	       (double) tmp_offset < (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_0_sustain;

	end_offset = ((double) frame_count * 3.0 / 4.0);

	end_volume = modular_synth_util->env_0_release;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 2.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_0_release;

	end_offset = ((double) frame_count);

	end_volume = modular_synth_util->env_0_release;
      
	if(end_offset == 0 ||
	   end_offset >= frame_count){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count * 3.0 / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else{
	env_amount = 0.0;
	env_volume = modular_synth_util->env_0_release;
      }

      if(frame_count - tmp_offset > 0){
	compute_frame_count = frame_count - tmp_offset;
      }else{
	compute_frame_count = ((double) frame_count / 4.0);
      }
      
      if(compute_frame_count > (guint) ((double) frame_count / 4.0)){
	compute_frame_count = (guint) ((double) frame_count / 4.0);
      }
      
      if((tmp_offset - offset) + compute_frame_count > modular_synth_util->buffer_length){
	compute_frame_count = modular_synth_util->buffer_length - (tmp_offset - offset);
      }
      
      if(tmp_offset > 0){
	if(tmp_offset - offset >= modular_synth_util->buffer_length){
	  break;
	}
      }

      ags_envelope_util_set_buffer_length(modular_synth_util->env_0_util,
					  compute_frame_count);

      ags_envelope_util_set_source(modular_synth_util->env_0_util,
				   ((gdouble *) modular_synth_util->env_0_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_destination(modular_synth_util->env_0_util,
					((gdouble *) modular_synth_util->env_0_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_offset(modular_synth_util->env_0_util,
				   0);

      ags_envelope_util_set_volume(modular_synth_util->env_0_util,
				   env_volume);

      ags_envelope_util_set_amount(modular_synth_util->env_0_util,
				   env_amount);

      ags_envelope_util_compute(modular_synth_util->env_0_util);

      /* iterate */
      tmp_offset += (guint) compute_frame_count;
    }
  }

  /* env-1 */
  env_1_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->env_1_sends[nth_sends] != 0){
      env_1_has_sends = TRUE;

      break;
    }
  }

  if(env_1_has_sends){
    for(i = 0; i < modular_synth_util->buffer_length; i++){
      ((gdouble *) modular_synth_util->env_1_buffer)[i] = 1.0;
    }
    
    ags_envelope_util_set_source(modular_synth_util->env_1_util,
				 modular_synth_util->env_1_buffer);

    ags_envelope_util_set_destination(modular_synth_util->env_1_util,
				      modular_synth_util->env_1_buffer);

    ags_envelope_util_set_buffer_length(modular_synth_util->env_1_util,
					modular_synth_util->buffer_length);

    ags_envelope_util_set_format(modular_synth_util->env_1_util,
				 AGS_SOUNDCARD_DOUBLE);

    tmp_offset = offset;

    compute_frame_count = ((double) frame_count / 4.0);
    
    while(tmp_offset < frame_count){
      /* env-1 attack */
      env_amount = 0.0;
      env_volume = 1.0;

      if((double) tmp_offset >= 0.0 &&
	 (double) tmp_offset < (double) frame_count / 4.0){
	env_volume = modular_synth_util->env_1_attack;

	end_offset = ((double) frame_count / 4.0);

	end_volume = modular_synth_util->env_1_decay;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - 0);

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 4.0 &&
	       (double) tmp_offset < (double) frame_count / 2.0){
	env_volume = modular_synth_util->env_1_decay;

	end_offset = ((double) frame_count / 2.0);

	end_volume = modular_synth_util->env_1_sustain;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 2.0 &&
	       (double) tmp_offset < (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_1_sustain;

	end_offset = ((double) frame_count * 3.0 / 4.0);

	end_volume = modular_synth_util->env_1_release;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 2.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_1_release;

	end_offset = ((double) frame_count);

	end_volume = modular_synth_util->env_1_release;
      
	if(end_offset == 0 ||
	   end_offset >= frame_count){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count * 3.0 / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else{
	env_amount = 0.0;
	env_volume = modular_synth_util->env_0_release;
      }

      if(frame_count - tmp_offset > 0){
	compute_frame_count = frame_count - tmp_offset;
      }else{
	compute_frame_count = ((double) frame_count / 4.0);
      }
      
      if(compute_frame_count > (guint) ((double) frame_count / 4.0)){
	compute_frame_count = (guint) ((double) frame_count / 4.0);
      }
      
      if((tmp_offset - offset) + compute_frame_count > modular_synth_util->buffer_length){
	compute_frame_count = modular_synth_util->buffer_length - (tmp_offset - offset);
      }
      
      if(tmp_offset > 0){
	if(tmp_offset - offset >= modular_synth_util->buffer_length){
	  break;
	}
      }

      ags_envelope_util_set_buffer_length(modular_synth_util->env_1_util,
					  compute_frame_count);

      ags_envelope_util_set_source(modular_synth_util->env_1_util,
				   ((gdouble *) modular_synth_util->env_1_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_destination(modular_synth_util->env_1_util,
					((gdouble *) modular_synth_util->env_1_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_offset(modular_synth_util->env_1_util,
				   tmp_offset);

      ags_envelope_util_set_volume(modular_synth_util->env_1_util,
				   env_volume);

      ags_envelope_util_set_amount(modular_synth_util->env_1_util,
				   env_amount);

      ags_envelope_util_compute(modular_synth_util->env_1_util);

      /* iterate */
      tmp_offset += (guint) compute_frame_count;
    }
  }

  /* LFO-0 */
  lfo_0_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->lfo_0_sends[nth_sends] != 0){
      lfo_0_has_sends = TRUE;

      break;
    }
  }

  if(lfo_0_has_sends){
    ags_lfo_synth_util_set_source(modular_synth_util->lfo_0_util,
				  modular_synth_util->lfo_0_buffer);

    ags_lfo_synth_util_set_buffer_length(modular_synth_util->lfo_0_util,
					 modular_synth_util->buffer_length);

    ags_lfo_synth_util_set_format(modular_synth_util->lfo_0_util,
				  AGS_SOUNDCARD_DOUBLE);

    ags_lfo_synth_util_set_samplerate(modular_synth_util->lfo_0_util,
				      samplerate);

    ags_lfo_synth_util_set_lfo_synth_oscillator_mode(modular_synth_util->lfo_0_util,
						     modular_synth_util->lfo_0_oscillator);

    ags_lfo_synth_util_set_frequency(modular_synth_util->lfo_0_util,
				     modular_synth_util->lfo_0_frequency);

    ags_lfo_synth_util_set_lfo_depth(modular_synth_util->lfo_0_util,
				     modular_synth_util->lfo_0_depth);
  
    ags_lfo_synth_util_set_tuning(modular_synth_util->lfo_0_util,
				      modular_synth_util->lfo_0_tuning);

    ags_lfo_synth_util_set_offset(modular_synth_util->lfo_0_util,
				  offset);

    switch(modular_synth_util->lfo_0_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
      {
	ags_lfo_synth_util_compute_sin(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
      {
	ags_lfo_synth_util_compute_sawtooth(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
      {
	ags_lfo_synth_util_compute_triangle(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
      {
	ags_lfo_synth_util_compute_square(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
      {
	ags_lfo_synth_util_compute_impulse(modular_synth_util->lfo_0_util);
      }
      break;
    }
  }

  /* LFO-1 */
  lfo_1_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->lfo_1_sends[nth_sends] != 0){
      lfo_1_has_sends = TRUE;

      break;
    }
  }

  if(lfo_1_has_sends){
    ags_lfo_synth_util_set_source(modular_synth_util->lfo_1_util,
				  modular_synth_util->lfo_1_buffer);

    ags_lfo_synth_util_set_buffer_length(modular_synth_util->lfo_1_util,
					 modular_synth_util->buffer_length);

    ags_lfo_synth_util_set_format(modular_synth_util->lfo_1_util,
				  AGS_SOUNDCARD_DOUBLE);

    ags_lfo_synth_util_set_samplerate(modular_synth_util->lfo_1_util,
				      samplerate);

    ags_lfo_synth_util_set_lfo_synth_oscillator_mode(modular_synth_util->lfo_1_util,
						     modular_synth_util->lfo_1_oscillator);

    ags_lfo_synth_util_set_frequency(modular_synth_util->lfo_1_util,
				     modular_synth_util->lfo_1_frequency);

    ags_lfo_synth_util_set_lfo_depth(modular_synth_util->lfo_1_util,
				     modular_synth_util->lfo_1_depth);
  
    ags_lfo_synth_util_set_tuning(modular_synth_util->lfo_1_util,
				  modular_synth_util->lfo_1_tuning);

    ags_lfo_synth_util_set_offset(modular_synth_util->lfo_1_util,
				  offset);

    switch(modular_synth_util->lfo_1_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
      {
	ags_lfo_synth_util_compute_sin(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
      {
	ags_lfo_synth_util_compute_sawtooth(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
      {
	ags_lfo_synth_util_compute_triangle(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
      {
	ags_lfo_synth_util_compute_square(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
      {
	ags_lfo_synth_util_compute_impulse(modular_synth_util->lfo_1_util);
      }
      break;
    }
  }
  
  /* noise */
  noise_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->noise_sends[nth_sends] != 0){
      noise_has_sends = TRUE;

      break;
    }
  }

  if(noise_has_sends){
    ags_noise_util_set_source(modular_synth_util->noise_util,
			      modular_synth_util->noise_buffer);

    ags_noise_util_set_buffer_length(modular_synth_util->noise_util,
				     modular_synth_util->buffer_length);

    ags_noise_util_set_format(modular_synth_util->noise_util,
			      AGS_SOUNDCARD_DOUBLE);

    ags_noise_util_set_samplerate(modular_synth_util->noise_util,
				  samplerate);

    ags_noise_util_set_volume(modular_synth_util->noise_util,
			      modular_synth_util->noise_gain);

    ags_noise_util_set_frequency(modular_synth_util->noise_util,
				 modular_synth_util->noise_frequency);

    ags_noise_util_set_offset(modular_synth_util->noise_util,
			      offset);

    ags_noise_util_compute(modular_synth_util->noise_util);
  }

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = modular_synth_util->buffer_length - (modular_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_sine;
    ags_v8double v_sawtooth;
    ags_v8double v_triangle;
    ags_v8double v_square;
    ags_v8double v_impulse;
    ags_v8double v_buffer;

    ags_v8double v_osc_0_frequency;
    ags_v8double tmp_v_osc_0_frequency;
    ags_v8double v_osc_0_phase;
    ags_v8double tmp_v_osc_0_phase;
    ags_v8double v_osc_0_volume;
    ags_v8double tmp_v_osc_0_volume;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride)
    };

    v_osc_0_frequency = (ags_v8double) {
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency
    };

    v_osc_0_phase = (ags_v8double) {
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase
    };

    v_osc_0_volume = (ags_v8double) {
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume
    };

    /* env-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_frequency = tmp_v_osc_0_frequency * v_osc_0_frequency;
    }
      
    /* env-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_phase = (tmp_v_osc_0_phase * 2.0 * M_PI) + v_osc_0_phase;
    }

    /* env-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_volume = tmp_v_osc_0_volume * v_osc_0_volume;
    }

    /* env-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_frequency = tmp_v_osc_0_frequency * v_osc_0_frequency;
    }

    /* env-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_phase = (tmp_v_osc_0_phase * 2.0 * M_PI) + v_osc_0_phase;
    }

    /* env-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_volume = tmp_v_osc_0_volume * v_osc_0_volume;
    }

    /* lfo-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }
      
    /* lfo-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* lfo-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* lfo-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }

    /* lfo-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* lfo-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* noise to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }

    /* noise to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* noise to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* compute osc-0 */
    if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      v_sine = (ags_v8double) {
	(gdouble) (sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate))
      };
    
      v_buffer += (v_sine * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      v_sawtooth = (ags_v8double) {
	((fmod(((gdouble) (offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[0] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[1] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[2] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[3] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[4] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[5] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[6] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[7] / samplerate) - 1.0)
      };
    
      v_buffer += (v_sawtooth * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      v_triangle = (ags_v8double) {
	(((((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * v_osc_0_frequency[0] / samplerate * 2.0) - (((double) ((((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * v_osc_0_frequency[0] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * v_osc_0_frequency[1] / samplerate * 2.0) - (((double) ((((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * v_osc_0_frequency[1] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * v_osc_0_frequency[2] / samplerate * 2.0) - (((double) ((((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * v_osc_0_frequency[2] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * v_osc_0_frequency[3] / samplerate * 2.0) - (((double) ((((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * v_osc_0_frequency[3] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * v_osc_0_frequency[4] / samplerate * 2.0) - (((double) ((((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * v_osc_0_frequency[4] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * v_osc_0_frequency[5] / samplerate * 2.0) - (((double) ((((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * v_osc_0_frequency[5] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * v_osc_0_frequency[6] / samplerate * 2.0) - (((double) ((((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * v_osc_0_frequency[6] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * v_osc_0_frequency[7] / samplerate * 2.0) - (((double) ((((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * v_osc_0_frequency[7] / samplerate)) / 2.0) * 2.0) - 1.0)),
      };
    
      v_buffer += (v_triangle * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      v_square = (ags_v8double) {
	((sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
      };
    
      v_buffer += (v_square * scale * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      v_impulse = (ags_v8double) {
	(sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0)
      };
    
      v_buffer += (v_impulse * scale * v_osc_0_volume);
    }
    
    *(source) = (gint64) v_buffer[0];
    *(source += source_stride) = (gint64) v_buffer[1];
    *(source += source_stride) = (gint64) v_buffer[2];
    *(source += source_stride) = (gint64) v_buffer[3];
    *(source += source_stride) = (gint64) v_buffer[4];
    *(source += source_stride) = (gint64) v_buffer[5];
    *(source += source_stride) = (gint64) v_buffer[6];
    *(source += source_stride) = (gint64) v_buffer[7];

    source += source_stride;

    i += 8;
  }
#endif
  
  for(; i < modular_synth_util->buffer_length;){
    /* OSC-0 */
    osc_0_frequency = modular_synth_util->osc_0_frequency;

    osc_0_phase = modular_synth_util->osc_0_phase;

    osc_0_volume = modular_synth_util->osc_0_volume;

    /* env-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_0_frequency);
    }

    /* env-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = (((gdouble *) modular_synth_util->env_0_buffer)[i] * 2.0 * M_PI) + (osc_0_phase);
    }

    /* env-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_0_volume);
    }

    /* env-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (osc_0_frequency);
    }

    /* env-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = (((gdouble *) modular_synth_util->env_1_buffer)[i] * 2.0 * M_PI) + (osc_0_phase);
    }

    /* env-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (modular_synth_util->osc_0_volume);
    }

    /* lfo-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* lfo-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* lfo-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_0_volume);
    }

    /* lfo-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* lfo-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* lfo-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_0_volume);
    }

    /* noise to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* noise to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* noise to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (modular_synth_util->osc_0_volume);
    }

    /* compute osc-0 */
    if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      source[0] = (gint64) ((gint64) (source[0]) + (gint64) (sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) * scale * osc_0_volume));
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      source[0] = (gint64) ((gint64) (source[0]) + ((fmod(((gdouble) (offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))), (samplerate / osc_0_frequency)) * 2.0 * osc_0_frequency / samplerate) - 1.0) * scale * osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      source[0] = (gint64) ((gint64) (source[0]) + (((((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * osc_0_frequency / samplerate * 2.0) - (((double) ((((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * osc_0_frequency / samplerate)) / 2.0) * 2.0) - 1.0) * scale * osc_0_volume));
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      source[0] = (gint64) ((gint64) (source[0]) + ((sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * scale * osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      source[0] = (gint64) ((gint64) (source[0]) + (sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * scale * osc_0_volume);
    }

    source += source_stride;
    i++;
  }

  /* OSC-1 */
  source = modular_synth_util->source;

  offset = modular_synth_util->offset;

  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = modular_synth_util->buffer_length - (modular_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_sine;
    ags_v8double v_sawtooth;
    ags_v8double v_triangle;
    ags_v8double v_square;
    ags_v8double v_impulse;
    ags_v8double v_buffer;

    ags_v8double v_osc_1_frequency;
    ags_v8double tmp_v_osc_1_frequency;
    ags_v8double v_osc_1_phase;
    ags_v8double tmp_v_osc_1_phase;
    ags_v8double v_osc_1_volume;
    ags_v8double tmp_v_osc_1_volume;
  
    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride)
    };

    v_osc_1_frequency = (ags_v8double) {
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency
    };

    v_osc_1_phase = (ags_v8double) {
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase
    };

    v_osc_1_volume = (ags_v8double) {
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume
    };

    /* env-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }
      
    /* env-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* env-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* env-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* env-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* env-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* lfo-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }
      
    /* lfo-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* lfo-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* lfo-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* lfo-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* lfo-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* noise to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* noise to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* noise to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7],
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* compute osc-1 */
    if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      v_sine = (ags_v8double) {
	(gdouble) (sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate))
      };
    
      v_buffer += (v_sine * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      v_sawtooth = (ags_v8double) {
	((fmod(((gdouble) (offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[0] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))), (samplerate / v_osc_1_frequency[1])) * 2.0 * v_osc_1_frequency[1] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))), (samplerate / v_osc_1_frequency[2])) * 2.0 * v_osc_1_frequency[2] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))), (samplerate / v_osc_1_frequency[3])) * 2.0 * v_osc_1_frequency[3] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))), (samplerate / v_osc_1_frequency[4])) * 2.0 * v_osc_1_frequency[4] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))), (samplerate / v_osc_1_frequency[5])) * 2.0 * v_osc_1_frequency[5] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))), (samplerate / v_osc_1_frequency[6])) * 2.0 * v_osc_1_frequency[6] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))), (samplerate / v_osc_1_frequency[7])) * 2.0 * v_osc_1_frequency[7] / samplerate) - 1.0)
      };
    
      v_buffer += (v_sawtooth * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      v_triangle = (ags_v8double) {
	(((((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * v_osc_1_frequency[0] / samplerate * 2.0) - (((double) ((((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * v_osc_1_frequency[0] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * v_osc_1_frequency[1] / samplerate * 2.0) - (((double) ((((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * v_osc_1_frequency[1] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * v_osc_1_frequency[2] / samplerate * 2.0) - (((double) ((((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * v_osc_1_frequency[2] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * v_osc_1_frequency[3] / samplerate * 2.0) - (((double) ((((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * v_osc_1_frequency[3] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * v_osc_1_frequency[4] / samplerate * 2.0) - (((double) ((((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * v_osc_1_frequency[4] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * v_osc_1_frequency[5] / samplerate * 2.0) - (((double) ((((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * v_osc_1_frequency[5] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * v_osc_1_frequency[6] / samplerate * 2.0) - (((double) ((((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * v_osc_1_frequency[6] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * v_osc_1_frequency[7] / samplerate * 2.0) - (((double) ((((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * v_osc_1_frequency[7] / samplerate)) / 2.0) * 2.0) - 1.0)),
      };
    
      v_buffer += (v_triangle * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      v_square = (ags_v8double) {
	((sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
      };
    
      v_buffer += (v_square * scale * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      v_impulse = (ags_v8double) {
	(sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0)
      };
    
      v_buffer += (v_impulse * scale * v_osc_1_volume);
    }
    
    *(source) = (gint64) v_buffer[0];
    *(source += source_stride) = (gint64) v_buffer[1];
    *(source += source_stride) = (gint64) v_buffer[2];
    *(source += source_stride) = (gint64) v_buffer[3];
    *(source += source_stride) = (gint64) v_buffer[4];
    *(source += source_stride) = (gint64) v_buffer[5];
    *(source += source_stride) = (gint64) v_buffer[6];
    *(source += source_stride) = (gint64) v_buffer[7];

    source += source_stride;

    i += 8;
  }
#endif
  
  for(; i < modular_synth_util->buffer_length;){
    /* OSC-1 */
    osc_1_frequency = modular_synth_util->osc_1_frequency;

    osc_1_phase = modular_synth_util->osc_1_phase;

    osc_1_volume = modular_synth_util->osc_1_volume;

    /* env-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = (2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[i] - 1.0) * (osc_1_frequency);
    }

    /* env-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = (((gdouble *) modular_synth_util->env_0_buffer)[i] * 2.0 * M_PI) + (osc_1_phase);
    }

    /* env-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_1_volume);
    }

    /* env-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = (2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[i] - 1.0) * (osc_1_frequency);
    }

    /* env-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = (((gdouble *) modular_synth_util->env_1_buffer)[i] * 2.0 * M_PI) + (osc_1_phase);
    }

    /* env-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (osc_1_volume);
    }

    /* lfo-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* lfo-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* lfo-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_1_volume);
    }

    /* lfo-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* lfo-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* lfo-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_1_volume);
    }
    
    /* noise to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* noise to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* noise to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (modular_synth_util->osc_0_volume);
    }

    /* compute osc-1 */
    if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      source[0] = (gint64) ((gint64) (source[0]) + (gint64) (sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) * scale * osc_1_volume));
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      source[0] = (gint64) ((gint64) (source[0]) + ((fmod(((gdouble) (offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))), (samplerate / osc_1_frequency)) * 2.0 * osc_1_frequency / samplerate) - 1.0) * scale * osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      source[0] = (gint64) ((gint64) (source[0]) + (((((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * osc_1_frequency / samplerate * 2.0) - (((double) ((((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * osc_1_frequency / samplerate)) / 2.0) * 2.0) - 1.0) * scale * osc_1_volume));
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      source[0] = (gint64) ((gint64) (source[0]) + ((sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * scale * osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      source[0] = (gint64) ((gint64) (source[0]) + (sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * scale * osc_1_volume);
    }

    source += source_stride;
    i++;
  }

  /* pitch tuning */
  ags_fluid_interpolate_4th_order_util_config();

  source = modular_synth_util->source;

  phase_incr = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->phase_increment;

  base_key = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->base_key;
  tuning = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->tuning;

  offset = modular_synth_util->offset;

  i = 0;

  dsp_phase = 0;
  
  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index * source_stride];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  root_pitch_hz = exp2(((double) base_key - 48.0) / 12.0) * 440.0;

  //TODO:JK: implement me
  
  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gdouble phase_incr;

    gint row;
    
    main_pitch_tuning = modular_synth_util->pitch_tuning;

    /* env-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* env-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_0_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_1_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }
    
    /* noise to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((double *) modular_synth_util->noise_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    phase_incr = (exp2((((double) base_key - 48.0 + ((main_pitch_tuning) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    if(dsp_phase_index + 2 < buffer_length){
      pitch_buffer[dsp_i] = (coeffs_0 * start_point
			     + coeffs_1 * source[dsp_phase_index * source_stride]
			     + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
			     + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);
    }
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    offset += 1;
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gdouble phase_incr;

    gint row;
    
    main_pitch_tuning = modular_synth_util->pitch_tuning;

    /* env-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* env-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_0_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_1_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }
    
    /* noise to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((double *) modular_synth_util->noise_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    phase_incr = (exp2((((double) base_key - 48.0 + ((main_pitch_tuning) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    if(dsp_phase_index - 1 > 0 &&
       dsp_phase_index + 2 < buffer_length){
      pitch_buffer[dsp_i] = (coeffs_0 * source[(dsp_phase_index - 1) * source_stride]
			     + coeffs_1 * source[dsp_phase_index * source_stride]
			     + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
			     + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);
    }
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    offset += 1;
  }

  /* fill pitch */
  for(i = 0; i < buffer_length; i++){
    source[i * source_stride] = (gint64) pitch_buffer[i];
  }
  
  /* volume */
  source = modular_synth_util->source;

  offset = modular_synth_util->offset;

  i = 0;

  //TODO:JK: implement me

  for(; i < modular_synth_util->buffer_length;){
    main_volume = modular_synth_util->volume;
    
    /* env-1 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (main_volume);
    }

    /* env-0 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (main_volume);
    }

    /* lfo-0 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }

    /* lfo-1 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }
    
    /* noise to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }

    /* volume */
    source[0] = (gint64) (((double) source[0]) * main_volume);

    source += source_stride;
    i++;
  }
}

void
ags_modular_synth_util_compute_float(AgsModularSynthUtil *modular_synth_util)
{
  gfloat *source, *tmp_source;

  guint source_stride;
  guint buffer_length;
  guint samplerate;

  guint nth_sends;
  gboolean env_0_has_sends;
  gboolean env_1_has_sends;
  gboolean lfo_0_has_sends;
  gboolean lfo_1_has_sends;
  gboolean noise_has_sends;

  gdouble env_volume;
  gdouble env_amount;

  gdouble osc_0_frequency;
  gdouble osc_0_phase;
  gdouble osc_0_volume;

  gdouble osc_1_frequency;
  gdouble osc_1_phase;
  gdouble osc_1_volume;

  gdouble main_pitch_tuning;

  gdouble *pitch_buffer;

  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;

  gdouble phase_incr;
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;
    
  gdouble main_volume;

  gdouble volume;
  gdouble tmp_volume, end_volume;

  guint offset;
  guint tmp_offset, end_offset;
  guint compute_frame_count;
  guint frame_count;
  guint i, i_stop;
  
  if(modular_synth_util == NULL ||
     modular_synth_util->source == NULL){
    return;
  }

  source = modular_synth_util->source;

  source_stride = modular_synth_util->source_stride;

  buffer_length = modular_synth_util->buffer_length;

  samplerate = modular_synth_util->samplerate;

  if(samplerate == 0){
    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  }
  
  osc_0_frequency = modular_synth_util->osc_0_frequency;
  osc_0_phase = modular_synth_util->osc_0_phase;
  osc_0_volume = modular_synth_util->osc_0_volume;
  
  if(osc_0_frequency == 0.0){
    osc_0_frequency = 0.01;
  }

  osc_1_frequency = modular_synth_util->osc_1_frequency;
  osc_1_phase = modular_synth_util->osc_1_phase;
  osc_1_volume = modular_synth_util->osc_1_volume;
  
  if(osc_1_frequency == 0.0){
    osc_1_frequency = 0.01;
  }
  
  pitch_buffer = modular_synth_util->pitch_buffer;
  
  frame_count = modular_synth_util->frame_count;
  offset = modular_synth_util->offset;

  i = 0;

  /* env-0 */
  env_0_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->env_0_sends[nth_sends] != 0){
      env_0_has_sends = TRUE;

      break;
    }
  }

  if(env_0_has_sends){
    for(i = 0; i < modular_synth_util->buffer_length; i++){
      ((gdouble *) modular_synth_util->env_0_buffer)[i] = 1.0;
    }
    
    ags_envelope_util_set_source(modular_synth_util->env_0_util,
				 modular_synth_util->env_0_buffer);

    ags_envelope_util_set_destination(modular_synth_util->env_0_util,
				      modular_synth_util->env_0_buffer);

    ags_envelope_util_set_buffer_length(modular_synth_util->env_0_util,
					modular_synth_util->buffer_length);

    ags_envelope_util_set_format(modular_synth_util->env_0_util,
				 AGS_SOUNDCARD_DOUBLE);

    tmp_offset = offset;

    compute_frame_count = ((double) frame_count / 4.0);

    while(tmp_offset < frame_count){
      /* env-0 attack */
      env_amount = 0.0;
      env_volume = 1.0;

      if((double) tmp_offset >= 0.0 &&
	 (double) tmp_offset < (double) frame_count / 4.0){
	env_volume = modular_synth_util->env_0_attack;

	end_offset = ((double) frame_count / 4.0);

	end_volume = modular_synth_util->env_0_decay;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - 0);

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 4.0 &&
	       (double) tmp_offset < (double) frame_count / 2.0){
	env_volume = modular_synth_util->env_0_decay;

	end_offset = ((double) frame_count / 2.0);

	end_volume = modular_synth_util->env_0_sustain;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 2.0 &&
	       (double) tmp_offset < (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_0_sustain;

	end_offset = ((double) frame_count * 3.0 / 4.0);

	end_volume = modular_synth_util->env_0_release;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 2.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_0_release;

	end_offset = ((double) frame_count);

	end_volume = modular_synth_util->env_0_release;
      
	if(end_offset == 0 ||
	   end_offset >= frame_count){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count * 3.0 / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else{
	env_amount = 0.0;
	env_volume = modular_synth_util->env_0_release;
      }

      if(frame_count - tmp_offset > 0){
	compute_frame_count = frame_count - tmp_offset;
      }else{
	compute_frame_count = ((double) frame_count / 4.0);
      }
      
      if(compute_frame_count > (guint) ((double) frame_count / 4.0)){
	compute_frame_count = (guint) ((double) frame_count / 4.0);
      }
      
      if((tmp_offset - offset) + compute_frame_count > modular_synth_util->buffer_length){
	compute_frame_count = modular_synth_util->buffer_length - (tmp_offset - offset);
      }
      
      if(tmp_offset > 0){
	if(tmp_offset - offset >= modular_synth_util->buffer_length){
	  break;
	}
      }

      ags_envelope_util_set_buffer_length(modular_synth_util->env_0_util,
					  compute_frame_count);

      ags_envelope_util_set_source(modular_synth_util->env_0_util,
				   ((gdouble *) modular_synth_util->env_0_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_destination(modular_synth_util->env_0_util,
					((gdouble *) modular_synth_util->env_0_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_offset(modular_synth_util->env_0_util,
				   0);

      ags_envelope_util_set_volume(modular_synth_util->env_0_util,
				   env_volume);

      ags_envelope_util_set_amount(modular_synth_util->env_0_util,
				   env_amount);

      ags_envelope_util_compute(modular_synth_util->env_0_util);

      /* iterate */
      tmp_offset += (guint) compute_frame_count;
    }
  }

  /* env-1 */
  env_1_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->env_1_sends[nth_sends] != 0){
      env_1_has_sends = TRUE;

      break;
    }
  }

  if(env_1_has_sends){
    for(i = 0; i < modular_synth_util->buffer_length; i++){
      ((gdouble *) modular_synth_util->env_1_buffer)[i] = 1.0;
    }
    
    ags_envelope_util_set_source(modular_synth_util->env_1_util,
				 modular_synth_util->env_1_buffer);

    ags_envelope_util_set_destination(modular_synth_util->env_1_util,
				      modular_synth_util->env_1_buffer);

    ags_envelope_util_set_buffer_length(modular_synth_util->env_1_util,
					modular_synth_util->buffer_length);

    ags_envelope_util_set_format(modular_synth_util->env_1_util,
				 AGS_SOUNDCARD_DOUBLE);

    tmp_offset = offset;

    compute_frame_count = ((double) frame_count / 4.0);
    
    while(tmp_offset < frame_count){
      /* env-1 attack */
      env_amount = 0.0;
      env_volume = 1.0;

      if((double) tmp_offset >= 0.0 &&
	 (double) tmp_offset < (double) frame_count / 4.0){
	env_volume = modular_synth_util->env_1_attack;

	end_offset = ((double) frame_count / 4.0);

	end_volume = modular_synth_util->env_1_decay;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - 0);

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 4.0 &&
	       (double) tmp_offset < (double) frame_count / 2.0){
	env_volume = modular_synth_util->env_1_decay;

	end_offset = ((double) frame_count / 2.0);

	end_volume = modular_synth_util->env_1_sustain;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 2.0 &&
	       (double) tmp_offset < (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_1_sustain;

	end_offset = ((double) frame_count * 3.0 / 4.0);

	end_volume = modular_synth_util->env_1_release;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 2.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_1_release;

	end_offset = ((double) frame_count);

	end_volume = modular_synth_util->env_1_release;
      
	if(end_offset == 0 ||
	   end_offset >= frame_count){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count * 3.0 / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else{
	env_amount = 0.0;
	env_volume = modular_synth_util->env_0_release;
      }

      if(frame_count - tmp_offset > 0){
	compute_frame_count = frame_count - tmp_offset;
      }else{
	compute_frame_count = ((double) frame_count / 4.0);
      }
      
      if(compute_frame_count > (guint) ((double) frame_count / 4.0)){
	compute_frame_count = (guint) ((double) frame_count / 4.0);
      }
      
      if((tmp_offset - offset) + compute_frame_count > modular_synth_util->buffer_length){
	compute_frame_count = modular_synth_util->buffer_length - (tmp_offset - offset);
      }
      
      if(tmp_offset > 0){
	if(tmp_offset - offset >= modular_synth_util->buffer_length){
	  break;
	}
      }

      ags_envelope_util_set_buffer_length(modular_synth_util->env_1_util,
					  compute_frame_count);

      ags_envelope_util_set_source(modular_synth_util->env_1_util,
				   ((gdouble *) modular_synth_util->env_1_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_destination(modular_synth_util->env_1_util,
					((gdouble *) modular_synth_util->env_1_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_offset(modular_synth_util->env_1_util,
				   tmp_offset);

      ags_envelope_util_set_volume(modular_synth_util->env_1_util,
				   env_volume);

      ags_envelope_util_set_amount(modular_synth_util->env_1_util,
				   env_amount);

      ags_envelope_util_compute(modular_synth_util->env_1_util);

      /* iterate */
      tmp_offset += (guint) compute_frame_count;
    }
  }

  /* LFO-0 */
  lfo_0_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->lfo_0_sends[nth_sends] != 0){
      lfo_0_has_sends = TRUE;

      break;
    }
  }

  if(lfo_0_has_sends){
    ags_lfo_synth_util_set_source(modular_synth_util->lfo_0_util,
				  modular_synth_util->lfo_0_buffer);

    ags_lfo_synth_util_set_buffer_length(modular_synth_util->lfo_0_util,
					 modular_synth_util->buffer_length);

    ags_lfo_synth_util_set_format(modular_synth_util->lfo_0_util,
				  AGS_SOUNDCARD_DOUBLE);

    ags_lfo_synth_util_set_samplerate(modular_synth_util->lfo_0_util,
				      samplerate);

    ags_lfo_synth_util_set_lfo_synth_oscillator_mode(modular_synth_util->lfo_0_util,
						     modular_synth_util->lfo_0_oscillator);

    ags_lfo_synth_util_set_frequency(modular_synth_util->lfo_0_util,
				     modular_synth_util->lfo_0_frequency);

    ags_lfo_synth_util_set_lfo_depth(modular_synth_util->lfo_0_util,
				     modular_synth_util->lfo_0_depth);
  
    ags_lfo_synth_util_set_tuning(modular_synth_util->lfo_0_util,
				      modular_synth_util->lfo_0_tuning);

    ags_lfo_synth_util_set_offset(modular_synth_util->lfo_0_util,
				  offset);

    switch(modular_synth_util->lfo_0_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
      {
	ags_lfo_synth_util_compute_sin(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
      {
	ags_lfo_synth_util_compute_sawtooth(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
      {
	ags_lfo_synth_util_compute_triangle(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
      {
	ags_lfo_synth_util_compute_square(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
      {
	ags_lfo_synth_util_compute_impulse(modular_synth_util->lfo_0_util);
      }
      break;
    }
  }

  /* LFO-1 */
  lfo_1_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->lfo_1_sends[nth_sends] != 0){
      lfo_1_has_sends = TRUE;

      break;
    }
  }

  if(lfo_1_has_sends){
    ags_lfo_synth_util_set_source(modular_synth_util->lfo_1_util,
				  modular_synth_util->lfo_1_buffer);

    ags_lfo_synth_util_set_buffer_length(modular_synth_util->lfo_1_util,
					 modular_synth_util->buffer_length);

    ags_lfo_synth_util_set_format(modular_synth_util->lfo_1_util,
				  AGS_SOUNDCARD_DOUBLE);

    ags_lfo_synth_util_set_samplerate(modular_synth_util->lfo_1_util,
				      samplerate);

    ags_lfo_synth_util_set_lfo_synth_oscillator_mode(modular_synth_util->lfo_1_util,
						     modular_synth_util->lfo_1_oscillator);

    ags_lfo_synth_util_set_frequency(modular_synth_util->lfo_1_util,
				     modular_synth_util->lfo_1_frequency);

    ags_lfo_synth_util_set_lfo_depth(modular_synth_util->lfo_1_util,
				     modular_synth_util->lfo_1_depth);
  
    ags_lfo_synth_util_set_tuning(modular_synth_util->lfo_1_util,
				  modular_synth_util->lfo_1_tuning);

    ags_lfo_synth_util_set_offset(modular_synth_util->lfo_1_util,
				  offset);

    switch(modular_synth_util->lfo_1_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
      {
	ags_lfo_synth_util_compute_sin(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
      {
	ags_lfo_synth_util_compute_sawtooth(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
      {
	ags_lfo_synth_util_compute_triangle(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
      {
	ags_lfo_synth_util_compute_square(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
      {
	ags_lfo_synth_util_compute_impulse(modular_synth_util->lfo_1_util);
      }
      break;
    }
  }
  
  /* noise */
  noise_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->noise_sends[nth_sends] != 0){
      noise_has_sends = TRUE;

      break;
    }
  }

  if(noise_has_sends){
    ags_noise_util_set_source(modular_synth_util->noise_util,
			      modular_synth_util->noise_buffer);

    ags_noise_util_set_buffer_length(modular_synth_util->noise_util,
				     modular_synth_util->buffer_length);

    ags_noise_util_set_format(modular_synth_util->noise_util,
			      AGS_SOUNDCARD_DOUBLE);

    ags_noise_util_set_samplerate(modular_synth_util->noise_util,
				  samplerate);

    ags_noise_util_set_volume(modular_synth_util->noise_util,
			      modular_synth_util->noise_gain);

    ags_noise_util_set_frequency(modular_synth_util->noise_util,
				 modular_synth_util->noise_frequency);

    ags_noise_util_set_offset(modular_synth_util->noise_util,
			      offset);

    ags_noise_util_compute(modular_synth_util->noise_util);
  }

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = modular_synth_util->buffer_length - (modular_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_sine;
    ags_v8double v_sawtooth;
    ags_v8double v_triangle;
    ags_v8double v_square;
    ags_v8double v_impulse;
    ags_v8double v_buffer;

    ags_v8double v_osc_0_frequency;
    ags_v8double tmp_v_osc_0_frequency;
    ags_v8double v_osc_0_phase;
    ags_v8double tmp_v_osc_0_phase;
    ags_v8double v_osc_0_volume;
    ags_v8double tmp_v_osc_0_volume;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride)
    };

    v_osc_0_frequency = (ags_v8double) {
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency
    };

    v_osc_0_phase = (ags_v8double) {
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase
    };

    v_osc_0_volume = (ags_v8double) {
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume
    };

    /* env-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_frequency = tmp_v_osc_0_frequency * v_osc_0_frequency;
    }
      
    /* env-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_phase = (tmp_v_osc_0_phase * 2.0 * M_PI) + v_osc_0_phase;
    }

    /* env-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_volume = tmp_v_osc_0_volume * v_osc_0_volume;
    }

    /* env-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_frequency = tmp_v_osc_0_frequency * v_osc_0_frequency;
    }

    /* env-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_phase = (tmp_v_osc_0_phase * 2.0 * M_PI) + v_osc_0_phase;
    }

    /* env-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_volume = tmp_v_osc_0_volume * v_osc_0_volume;
    }

    /* lfo-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }
      
    /* lfo-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* lfo-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* lfo-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }

    /* lfo-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* lfo-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* noise to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }

    /* noise to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* noise to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* compute osc-0 */
    if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      v_sine = (ags_v8double) {
	(gdouble) (sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate))
      };
    
      v_buffer += (v_sine * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      v_sawtooth = (ags_v8double) {
	((fmod(((gdouble) (offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[0] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))), (samplerate / v_osc_0_frequency[1])) * 2.0 * v_osc_0_frequency[1] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))), (samplerate / v_osc_0_frequency[2])) * 2.0 * v_osc_0_frequency[2] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))), (samplerate / v_osc_0_frequency[3])) * 2.0 * v_osc_0_frequency[3] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))), (samplerate / v_osc_0_frequency[4])) * 2.0 * v_osc_0_frequency[4] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))), (samplerate / v_osc_0_frequency[5])) * 2.0 * v_osc_0_frequency[5] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))), (samplerate / v_osc_0_frequency[6])) * 2.0 * v_osc_0_frequency[6] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))), (samplerate / v_osc_0_frequency[7])) * 2.0 * v_osc_0_frequency[7] / samplerate) - 1.0)
      };
    
      v_buffer += (v_sawtooth * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      v_triangle = (ags_v8double) {
	(((((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * v_osc_0_frequency[0] / samplerate * 2.0) - (((double) ((((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * v_osc_0_frequency[0] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * v_osc_0_frequency[1] / samplerate * 2.0) - (((double) ((((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * v_osc_0_frequency[1] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * v_osc_0_frequency[2] / samplerate * 2.0) - (((double) ((((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * v_osc_0_frequency[2] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * v_osc_0_frequency[3] / samplerate * 2.0) - (((double) ((((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * v_osc_0_frequency[3] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * v_osc_0_frequency[4] / samplerate * 2.0) - (((double) ((((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * v_osc_0_frequency[4] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * v_osc_0_frequency[5] / samplerate * 2.0) - (((double) ((((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * v_osc_0_frequency[5] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * v_osc_0_frequency[6] / samplerate * 2.0) - (((double) ((((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * v_osc_0_frequency[6] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * v_osc_0_frequency[7] / samplerate * 2.0) - (((double) ((((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * v_osc_0_frequency[7] / samplerate)) / 2.0) * 2.0) - 1.0)),
      };
    
      v_buffer += (v_triangle * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      v_square = (ags_v8double) {
	((sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
      };
    
      v_buffer += (v_square * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      v_impulse = (ags_v8double) {
	(sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0)
      };
    
      v_buffer += (v_impulse * v_osc_0_volume);
    }
    
    *(source) = (gfloat) v_buffer[0];
    *(source += source_stride) = (gfloat) v_buffer[1];
    *(source += source_stride) = (gfloat) v_buffer[2];
    *(source += source_stride) = (gfloat) v_buffer[3];
    *(source += source_stride) = (gfloat) v_buffer[4];
    *(source += source_stride) = (gfloat) v_buffer[5];
    *(source += source_stride) = (gfloat) v_buffer[6];
    *(source += source_stride) = (gfloat) v_buffer[7];

    source += source_stride;

    i += 8;
  }
#endif
  
  for(; i < modular_synth_util->buffer_length;){
    /* OSC-0 */
    osc_0_frequency = modular_synth_util->osc_0_frequency;

    osc_0_phase = modular_synth_util->osc_0_phase;

    osc_0_volume = modular_synth_util->osc_0_volume;

    /* env-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_0_frequency);
    }

    /* env-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = (((gdouble *) modular_synth_util->env_0_buffer)[i] * 2.0 * M_PI) + (osc_0_phase);
    }

    /* env-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_0_volume);
    }

    /* env-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (osc_0_frequency);
    }

    /* env-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = (((gdouble *) modular_synth_util->env_1_buffer)[i] * 2.0 * M_PI) + (osc_0_phase);
    }

    /* env-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (modular_synth_util->osc_0_volume);
    }

    /* lfo-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* lfo-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* lfo-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_0_volume);
    }

    /* lfo-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* lfo-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* lfo-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_0_volume);
    }

    /* noise to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* noise to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* noise to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (modular_synth_util->osc_0_volume);
    }

    /* compute osc-0 */
    if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      source[0] = (gfloat) ((source[0]) + (sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) * osc_0_volume));
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      source[0] = (gfloat) ((source[0]) + ((fmod(((gdouble) (offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))), (samplerate / osc_0_frequency)) * 2.0 * osc_0_frequency / samplerate) - 1.0) * osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      source[0] = (gfloat) ((source[0]) + (((((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * osc_0_frequency / samplerate * 2.0) - (((double) ((((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * osc_0_frequency / samplerate)) / 2.0) * 2.0) - 1.0) * osc_0_volume));
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      source[0] = (gfloat) ((source[0]) + ((sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      source[0] = (gfloat) ((source[0]) + (sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * osc_0_volume);
    }

    source += source_stride;
    i++;
  }

  /* OSC-1 */
  source = modular_synth_util->source;

  offset = modular_synth_util->offset;

  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = modular_synth_util->buffer_length - (modular_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_sine;
    ags_v8double v_sawtooth;
    ags_v8double v_triangle;
    ags_v8double v_square;
    ags_v8double v_impulse;
    ags_v8double v_buffer;

    ags_v8double v_osc_1_frequency;
    ags_v8double tmp_v_osc_1_frequency;
    ags_v8double v_osc_1_phase;
    ags_v8double tmp_v_osc_1_phase;
    ags_v8double v_osc_1_volume;
    ags_v8double tmp_v_osc_1_volume;
  
    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride)
    };

    v_osc_1_frequency = (ags_v8double) {
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency
    };

    v_osc_1_phase = (ags_v8double) {
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase
    };

    v_osc_1_volume = (ags_v8double) {
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume
    };

    /* env-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }
      
    /* env-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* env-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* env-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* env-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* env-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* lfo-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }
      
    /* lfo-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* lfo-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* lfo-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* lfo-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* lfo-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* noise to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* noise to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* noise to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7],
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* compute osc-1 */
    if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      v_sine = (ags_v8double) {
	(gdouble) (sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate))
      };
    
      v_buffer += (v_sine * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      v_sawtooth = (ags_v8double) {
	((fmod(((gdouble) (offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[0] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))), (samplerate / v_osc_1_frequency[1])) * 2.0 * v_osc_1_frequency[1] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))), (samplerate / v_osc_1_frequency[2])) * 2.0 * v_osc_1_frequency[2] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))), (samplerate / v_osc_1_frequency[3])) * 2.0 * v_osc_1_frequency[3] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))), (samplerate / v_osc_1_frequency[4])) * 2.0 * v_osc_1_frequency[4] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))), (samplerate / v_osc_1_frequency[5])) * 2.0 * v_osc_1_frequency[5] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))), (samplerate / v_osc_1_frequency[6])) * 2.0 * v_osc_1_frequency[6] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))), (samplerate / v_osc_1_frequency[7])) * 2.0 * v_osc_1_frequency[7] / samplerate) - 1.0)
      };
    
      v_buffer += (v_sawtooth * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      v_triangle = (ags_v8double) {
	(((((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * v_osc_1_frequency[0] / samplerate * 2.0) - (((double) ((((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * v_osc_1_frequency[0] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * v_osc_1_frequency[1] / samplerate * 2.0) - (((double) ((((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * v_osc_1_frequency[1] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * v_osc_1_frequency[2] / samplerate * 2.0) - (((double) ((((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * v_osc_1_frequency[2] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * v_osc_1_frequency[3] / samplerate * 2.0) - (((double) ((((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * v_osc_1_frequency[3] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * v_osc_1_frequency[4] / samplerate * 2.0) - (((double) ((((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * v_osc_1_frequency[4] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * v_osc_1_frequency[5] / samplerate * 2.0) - (((double) ((((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * v_osc_1_frequency[5] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * v_osc_1_frequency[6] / samplerate * 2.0) - (((double) ((((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * v_osc_1_frequency[6] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * v_osc_1_frequency[7] / samplerate * 2.0) - (((double) ((((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * v_osc_1_frequency[7] / samplerate)) / 2.0) * 2.0) - 1.0)),
      };
    
      v_buffer += (v_triangle * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      v_square = (ags_v8double) {
	((sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
      };
    
      v_buffer += (v_square * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      v_impulse = (ags_v8double) {
	(sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0)
      };
    
      v_buffer += (v_impulse * v_osc_1_volume);
    }
    
    *(source) = (gfloat) v_buffer[0];
    *(source += source_stride) = (gfloat) v_buffer[1];
    *(source += source_stride) = (gfloat) v_buffer[2];
    *(source += source_stride) = (gfloat) v_buffer[3];
    *(source += source_stride) = (gfloat) v_buffer[4];
    *(source += source_stride) = (gfloat) v_buffer[5];
    *(source += source_stride) = (gfloat) v_buffer[6];
    *(source += source_stride) = (gfloat) v_buffer[7];

    source += source_stride;

    i += 8;
  }
#endif
  
  for(; i < modular_synth_util->buffer_length;){
    /* OSC-1 */
    osc_1_frequency = modular_synth_util->osc_1_frequency;

    osc_1_phase = modular_synth_util->osc_1_phase;

    osc_1_volume = modular_synth_util->osc_1_volume;

    /* env-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = (2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[i] - 1.0) * (osc_1_frequency);
    }

    /* env-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = (((gdouble *) modular_synth_util->env_0_buffer)[i] * 2.0 * M_PI) + (osc_1_phase);
    }

    /* env-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_1_volume);
    }

    /* env-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = (2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[i] - 1.0) * (osc_1_frequency);
    }

    /* env-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = (((gdouble *) modular_synth_util->env_1_buffer)[i] * 2.0 * M_PI) + (osc_1_phase);
    }

    /* env-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (osc_1_volume);
    }

    /* lfo-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* lfo-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* lfo-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_1_volume);
    }

    /* lfo-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* lfo-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* lfo-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_1_volume);
    }
    
    /* noise to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* noise to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* noise to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (modular_synth_util->osc_0_volume);
    }

    /* compute osc-1 */
    if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      source[0] = (gfloat) ((source[0]) + (sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) * osc_1_volume));
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      source[0] = (gfloat) ((source[0]) + ((fmod(((gdouble) (offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))), (samplerate / osc_1_frequency)) * 2.0 * osc_1_frequency / samplerate) - 1.0) * osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      source[0] = (gfloat) ((source[0]) + (((((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * osc_1_frequency / samplerate * 2.0) - (((double) ((((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * osc_1_frequency / samplerate)) / 2.0) * 2.0) - 1.0) * osc_1_volume));
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      source[0] = (gfloat) ((source[0]) + ((sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      source[0] = (gfloat) ((source[0]) + (sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * osc_1_volume);
    }

    source += source_stride;
    i++;
  }

  /* pitch tuning */
  ags_fluid_interpolate_4th_order_util_config();

  source = modular_synth_util->source;

  phase_incr = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->phase_increment;

  base_key = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->base_key;
  tuning = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->tuning;

  offset = modular_synth_util->offset;

  i = 0;

  dsp_phase = 0;
  
  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index * source_stride];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  root_pitch_hz = exp2(((double) base_key - 48.0) / 12.0) * 440.0;

  //TODO:JK: implement me
  
  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gdouble phase_incr;

    gint row;
    
    main_pitch_tuning = modular_synth_util->pitch_tuning;

    /* env-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* env-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_0_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_1_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }
    
    /* noise to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((double *) modular_synth_util->noise_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    phase_incr = (exp2((((double) base_key - 48.0 + ((main_pitch_tuning) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    if(dsp_phase_index + 2 < buffer_length){
      pitch_buffer[dsp_i] = (coeffs_0 * start_point
			     + coeffs_1 * source[dsp_phase_index * source_stride]
			     + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
			     + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);
    }
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    offset += 1;
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gdouble phase_incr;

    gint row;
    
    main_pitch_tuning = modular_synth_util->pitch_tuning;

    /* env-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* env-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_0_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_1_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }
    
    /* noise to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((double *) modular_synth_util->noise_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    phase_incr = (exp2((((double) base_key - 48.0 + ((main_pitch_tuning) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    if(dsp_phase_index - 1 > 0 &&
       dsp_phase_index + 2 < buffer_length){
      pitch_buffer[dsp_i] = (coeffs_0 * source[(dsp_phase_index - 1) * source_stride]
			     + coeffs_1 * source[dsp_phase_index * source_stride]
			     + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
			     + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);
    }
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    offset += 1;
  }

  /* fill pitch */
  for(i = 0; i < buffer_length; i++){
    source[i * source_stride] = (gfloat) pitch_buffer[i];
  }
  
  /* volume */
  source = modular_synth_util->source;

  offset = modular_synth_util->offset;

  i = 0;

  //TODO:JK: implement me

  for(; i < modular_synth_util->buffer_length;){
    main_volume = modular_synth_util->volume;
    
    /* env-1 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (main_volume);
    }

    /* env-0 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (main_volume);
    }

    /* lfo-0 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }

    /* lfo-1 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }
    
    /* noise to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }

    /* volume */
    source[0] = (gfloat) (((double) source[0]) * main_volume);

    source += source_stride;
    i++;
  }
}

void
ags_modular_synth_util_compute_double(AgsModularSynthUtil *modular_synth_util)
{
  gdouble *source, *tmp_source;

  guint source_stride;
  guint buffer_length;
  guint samplerate;

  guint nth_sends;
  gboolean env_0_has_sends;
  gboolean env_1_has_sends;
  gboolean lfo_0_has_sends;
  gboolean lfo_1_has_sends;
  gboolean noise_has_sends;

  gdouble env_volume;
  gdouble env_amount;

  gdouble osc_0_frequency;
  gdouble osc_0_phase;
  gdouble osc_0_volume;

  gdouble osc_1_frequency;
  gdouble osc_1_phase;
  gdouble osc_1_volume;

  gdouble main_pitch_tuning;

  gdouble *pitch_buffer;

  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;

  gdouble phase_incr;
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  gdouble start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;
    
  gdouble main_volume;

  gdouble volume;
  gdouble tmp_volume, end_volume;

  guint offset;
  guint tmp_offset, end_offset;
  guint compute_frame_count;
  guint frame_count;
  guint i, i_stop;

  static const gdouble scale = 127.0;
  
  if(modular_synth_util == NULL ||
     modular_synth_util->source == NULL){
    return;
  }

  source = modular_synth_util->source;

  source_stride = modular_synth_util->source_stride;

  buffer_length = modular_synth_util->buffer_length;

  samplerate = modular_synth_util->samplerate;

  if(samplerate == 0){
    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  }
  
  osc_0_frequency = modular_synth_util->osc_0_frequency;
  osc_0_phase = modular_synth_util->osc_0_phase;
  osc_0_volume = modular_synth_util->osc_0_volume;
  
  if(osc_0_frequency == 0.0){
    osc_0_frequency = 0.01;
  }

  osc_1_frequency = modular_synth_util->osc_1_frequency;
  osc_1_phase = modular_synth_util->osc_1_phase;
  osc_1_volume = modular_synth_util->osc_1_volume;
  
  if(osc_1_frequency == 0.0){
    osc_1_frequency = 0.01;
  }
  
  pitch_buffer = modular_synth_util->pitch_buffer;
  
  frame_count = modular_synth_util->frame_count;
  offset = modular_synth_util->offset;

  i = 0;

  /* env-0 */
  env_0_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->env_0_sends[nth_sends] != 0){
      env_0_has_sends = TRUE;

      break;
    }
  }

  if(env_0_has_sends){
    for(i = 0; i < modular_synth_util->buffer_length; i++){
      ((gdouble *) modular_synth_util->env_0_buffer)[i] = 1.0;
    }
    
    ags_envelope_util_set_source(modular_synth_util->env_0_util,
				 modular_synth_util->env_0_buffer);

    ags_envelope_util_set_destination(modular_synth_util->env_0_util,
				      modular_synth_util->env_0_buffer);

    ags_envelope_util_set_buffer_length(modular_synth_util->env_0_util,
					modular_synth_util->buffer_length);

    ags_envelope_util_set_format(modular_synth_util->env_0_util,
				 AGS_SOUNDCARD_DOUBLE);

    tmp_offset = offset;

    compute_frame_count = ((double) frame_count / 4.0);

    while(tmp_offset < frame_count){
      /* env-0 attack */
      env_amount = 0.0;
      env_volume = 1.0;

      if((double) tmp_offset >= 0.0 &&
	 (double) tmp_offset < (double) frame_count / 4.0){
	env_volume = modular_synth_util->env_0_attack;

	end_offset = ((double) frame_count / 4.0);

	end_volume = modular_synth_util->env_0_decay;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - 0);

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 4.0 &&
	       (double) tmp_offset < (double) frame_count / 2.0){
	env_volume = modular_synth_util->env_0_decay;

	end_offset = ((double) frame_count / 2.0);

	end_volume = modular_synth_util->env_0_sustain;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 2.0 &&
	       (double) tmp_offset < (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_0_sustain;

	end_offset = ((double) frame_count * 3.0 / 4.0);

	end_volume = modular_synth_util->env_0_release;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 2.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_0_release;

	end_offset = ((double) frame_count);

	end_volume = modular_synth_util->env_0_release;
      
	if(end_offset == 0 ||
	   end_offset >= frame_count){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count * 3.0 / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else{
	env_amount = 0.0;
	env_volume = modular_synth_util->env_0_release;
      }

      if(frame_count - tmp_offset > 0){
	compute_frame_count = frame_count - tmp_offset;
      }else{
	compute_frame_count = ((double) frame_count / 4.0);
      }
      
      if(compute_frame_count > (guint) ((double) frame_count / 4.0)){
	compute_frame_count = (guint) ((double) frame_count / 4.0);
      }
      
      if((tmp_offset - offset) + compute_frame_count > modular_synth_util->buffer_length){
	compute_frame_count = modular_synth_util->buffer_length - (tmp_offset - offset);
      }
      
      if(tmp_offset > 0){
	if(tmp_offset - offset >= modular_synth_util->buffer_length){
	  break;
	}
      }

      ags_envelope_util_set_buffer_length(modular_synth_util->env_0_util,
					  compute_frame_count);

      ags_envelope_util_set_source(modular_synth_util->env_0_util,
				   ((gdouble *) modular_synth_util->env_0_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_destination(modular_synth_util->env_0_util,
					((gdouble *) modular_synth_util->env_0_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_offset(modular_synth_util->env_0_util,
				   0);

      ags_envelope_util_set_volume(modular_synth_util->env_0_util,
				   env_volume);

      ags_envelope_util_set_amount(modular_synth_util->env_0_util,
				   env_amount);

      ags_envelope_util_compute(modular_synth_util->env_0_util);

      /* iterate */
      tmp_offset += (guint) compute_frame_count;
    }
  }

  /* env-1 */
  env_1_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->env_1_sends[nth_sends] != 0){
      env_1_has_sends = TRUE;

      break;
    }
  }

  if(env_1_has_sends){
    for(i = 0; i < modular_synth_util->buffer_length; i++){
      ((gdouble *) modular_synth_util->env_1_buffer)[i] = 1.0;
    }
    
    ags_envelope_util_set_source(modular_synth_util->env_1_util,
				 modular_synth_util->env_1_buffer);

    ags_envelope_util_set_destination(modular_synth_util->env_1_util,
				      modular_synth_util->env_1_buffer);

    ags_envelope_util_set_buffer_length(modular_synth_util->env_1_util,
					modular_synth_util->buffer_length);

    ags_envelope_util_set_format(modular_synth_util->env_1_util,
				 AGS_SOUNDCARD_DOUBLE);

    tmp_offset = offset;

    compute_frame_count = ((double) frame_count / 4.0);
    
    while(tmp_offset < frame_count){
      /* env-1 attack */
      env_amount = 0.0;
      env_volume = 1.0;

      if((double) tmp_offset >= 0.0 &&
	 (double) tmp_offset < (double) frame_count / 4.0){
	env_volume = modular_synth_util->env_1_attack;

	end_offset = ((double) frame_count / 4.0);

	end_volume = modular_synth_util->env_1_decay;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - 0);

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 4.0 &&
	       (double) tmp_offset < (double) frame_count / 2.0){
	env_volume = modular_synth_util->env_1_decay;

	end_offset = ((double) frame_count / 2.0);

	end_volume = modular_synth_util->env_1_sustain;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 2.0 &&
	       (double) tmp_offset < (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_1_sustain;

	end_offset = ((double) frame_count * 3.0 / 4.0);

	end_volume = modular_synth_util->env_1_release;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 2.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_1_release;

	end_offset = ((double) frame_count);

	end_volume = modular_synth_util->env_1_release;
      
	if(end_offset == 0 ||
	   end_offset >= frame_count){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count * 3.0 / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else{
	env_amount = 0.0;
	env_volume = modular_synth_util->env_0_release;
      }

      if(frame_count - tmp_offset > 0){
	compute_frame_count = frame_count - tmp_offset;
      }else{
	compute_frame_count = ((double) frame_count / 4.0);
      }
      
      if(compute_frame_count > (guint) ((double) frame_count / 4.0)){
	compute_frame_count = (guint) ((double) frame_count / 4.0);
      }
      
      if((tmp_offset - offset) + compute_frame_count > modular_synth_util->buffer_length){
	compute_frame_count = modular_synth_util->buffer_length - (tmp_offset - offset);
      }
      
      if(tmp_offset > 0){
	if(tmp_offset - offset >= modular_synth_util->buffer_length){
	  break;
	}
      }

      ags_envelope_util_set_buffer_length(modular_synth_util->env_1_util,
					  compute_frame_count);

      ags_envelope_util_set_source(modular_synth_util->env_1_util,
				   ((gdouble *) modular_synth_util->env_1_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_destination(modular_synth_util->env_1_util,
					((gdouble *) modular_synth_util->env_1_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_offset(modular_synth_util->env_1_util,
				   tmp_offset);

      ags_envelope_util_set_volume(modular_synth_util->env_1_util,
				   env_volume);

      ags_envelope_util_set_amount(modular_synth_util->env_1_util,
				   env_amount);

      ags_envelope_util_compute(modular_synth_util->env_1_util);

      /* iterate */
      tmp_offset += (guint) compute_frame_count;
    }
  }

  /* LFO-0 */
  lfo_0_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->lfo_0_sends[nth_sends] != 0){
      lfo_0_has_sends = TRUE;

      break;
    }
  }

  if(lfo_0_has_sends){
    ags_lfo_synth_util_set_source(modular_synth_util->lfo_0_util,
				  modular_synth_util->lfo_0_buffer);

    ags_lfo_synth_util_set_buffer_length(modular_synth_util->lfo_0_util,
					 modular_synth_util->buffer_length);

    ags_lfo_synth_util_set_format(modular_synth_util->lfo_0_util,
				  AGS_SOUNDCARD_DOUBLE);

    ags_lfo_synth_util_set_samplerate(modular_synth_util->lfo_0_util,
				      samplerate);

    ags_lfo_synth_util_set_lfo_synth_oscillator_mode(modular_synth_util->lfo_0_util,
						     modular_synth_util->lfo_0_oscillator);

    ags_lfo_synth_util_set_frequency(modular_synth_util->lfo_0_util,
				     modular_synth_util->lfo_0_frequency);

    ags_lfo_synth_util_set_lfo_depth(modular_synth_util->lfo_0_util,
				     modular_synth_util->lfo_0_depth);
  
    ags_lfo_synth_util_set_tuning(modular_synth_util->lfo_0_util,
				      modular_synth_util->lfo_0_tuning);

    ags_lfo_synth_util_set_offset(modular_synth_util->lfo_0_util,
				  offset);

    switch(modular_synth_util->lfo_0_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
      {
	ags_lfo_synth_util_compute_sin(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
      {
	ags_lfo_synth_util_compute_sawtooth(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
      {
	ags_lfo_synth_util_compute_triangle(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
      {
	ags_lfo_synth_util_compute_square(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
      {
	ags_lfo_synth_util_compute_impulse(modular_synth_util->lfo_0_util);
      }
      break;
    }
  }

  /* LFO-1 */
  lfo_1_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->lfo_1_sends[nth_sends] != 0){
      lfo_1_has_sends = TRUE;

      break;
    }
  }

  if(lfo_1_has_sends){
    ags_lfo_synth_util_set_source(modular_synth_util->lfo_1_util,
				  modular_synth_util->lfo_1_buffer);

    ags_lfo_synth_util_set_buffer_length(modular_synth_util->lfo_1_util,
					 modular_synth_util->buffer_length);

    ags_lfo_synth_util_set_format(modular_synth_util->lfo_1_util,
				  AGS_SOUNDCARD_DOUBLE);

    ags_lfo_synth_util_set_samplerate(modular_synth_util->lfo_1_util,
				      samplerate);

    ags_lfo_synth_util_set_lfo_synth_oscillator_mode(modular_synth_util->lfo_1_util,
						     modular_synth_util->lfo_1_oscillator);

    ags_lfo_synth_util_set_frequency(modular_synth_util->lfo_1_util,
				     modular_synth_util->lfo_1_frequency);

    ags_lfo_synth_util_set_lfo_depth(modular_synth_util->lfo_1_util,
				     modular_synth_util->lfo_1_depth);
  
    ags_lfo_synth_util_set_tuning(modular_synth_util->lfo_1_util,
				  modular_synth_util->lfo_1_tuning);

    ags_lfo_synth_util_set_offset(modular_synth_util->lfo_1_util,
				  offset);

    switch(modular_synth_util->lfo_1_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
      {
	ags_lfo_synth_util_compute_sin(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
      {
	ags_lfo_synth_util_compute_sawtooth(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
      {
	ags_lfo_synth_util_compute_triangle(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
      {
	ags_lfo_synth_util_compute_square(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
      {
	ags_lfo_synth_util_compute_impulse(modular_synth_util->lfo_1_util);
      }
      break;
    }
  }
  
  /* noise */
  noise_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->noise_sends[nth_sends] != 0){
      noise_has_sends = TRUE;

      break;
    }
  }

  if(noise_has_sends){
    ags_noise_util_set_source(modular_synth_util->noise_util,
			      modular_synth_util->noise_buffer);

    ags_noise_util_set_buffer_length(modular_synth_util->noise_util,
				     modular_synth_util->buffer_length);

    ags_noise_util_set_format(modular_synth_util->noise_util,
			      AGS_SOUNDCARD_DOUBLE);

    ags_noise_util_set_samplerate(modular_synth_util->noise_util,
				  samplerate);

    ags_noise_util_set_volume(modular_synth_util->noise_util,
			      modular_synth_util->noise_gain);

    ags_noise_util_set_frequency(modular_synth_util->noise_util,
				 modular_synth_util->noise_frequency);

    ags_noise_util_set_offset(modular_synth_util->noise_util,
			      offset);

    ags_noise_util_compute(modular_synth_util->noise_util);
  }

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = modular_synth_util->buffer_length - (modular_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_sine;
    ags_v8double v_sawtooth;
    ags_v8double v_triangle;
    ags_v8double v_square;
    ags_v8double v_impulse;
    ags_v8double v_buffer;

    ags_v8double v_osc_0_frequency;
    ags_v8double tmp_v_osc_0_frequency;
    ags_v8double v_osc_0_phase;
    ags_v8double tmp_v_osc_0_phase;
    ags_v8double v_osc_0_volume;
    ags_v8double tmp_v_osc_0_volume;

    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride)
    };

    v_osc_0_frequency = (ags_v8double) {
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency,
      osc_0_frequency
    };

    v_osc_0_phase = (ags_v8double) {
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase,
      osc_0_phase
    };

    v_osc_0_volume = (ags_v8double) {
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume,
      osc_0_volume
    };

    /* env-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_frequency = tmp_v_osc_0_frequency * v_osc_0_frequency;
    }
      
    /* env-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_phase = (tmp_v_osc_0_phase * 2.0 * M_PI) + v_osc_0_phase;
    }

    /* env-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_0_volume = tmp_v_osc_0_volume * v_osc_0_volume;
    }

    /* env-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_frequency = tmp_v_osc_0_frequency * v_osc_0_frequency;
    }

    /* env-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_phase = (tmp_v_osc_0_phase * 2.0 * M_PI) + v_osc_0_phase;
    }

    /* env-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_0_volume = tmp_v_osc_0_volume * v_osc_0_volume;
    }

    /* lfo-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }
      
    /* lfo-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* lfo-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* lfo-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }

    /* lfo-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* lfo-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* noise to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      tmp_v_osc_0_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_frequency = ((tmp_v_osc_0_frequency + 1.0) / 2.0) * v_osc_0_frequency;
    }

    /* noise to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      tmp_v_osc_0_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_phase = ((tmp_v_osc_0_phase + 1.0) * M_PI) + v_osc_0_phase;
    }

    /* noise to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      tmp_v_osc_0_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_0_volume = ((tmp_v_osc_0_volume + 1.0) / 2.0) * v_osc_0_volume;
    }

    /* compute osc-0 */
    if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      v_sine = (ags_v8double) {
	(gdouble) (sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate))
      };
    
      v_buffer += (v_sine * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      v_sawtooth = (ags_v8double) {
	((fmod(((gdouble) (offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))), (samplerate / v_osc_0_frequency[0])) * 2.0 * v_osc_0_frequency[0] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))), (samplerate / v_osc_0_frequency[1])) * 2.0 * v_osc_0_frequency[1] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))), (samplerate / v_osc_0_frequency[2])) * 2.0 * v_osc_0_frequency[2] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))), (samplerate / v_osc_0_frequency[3])) * 2.0 * v_osc_0_frequency[3] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))), (samplerate / v_osc_0_frequency[4])) * 2.0 * v_osc_0_frequency[4] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))), (samplerate / v_osc_0_frequency[5])) * 2.0 * v_osc_0_frequency[5] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))), (samplerate / v_osc_0_frequency[6])) * 2.0 * v_osc_0_frequency[6] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))), (samplerate / v_osc_0_frequency[7])) * 2.0 * v_osc_0_frequency[7] / samplerate) - 1.0)
      };
    
      v_buffer += (v_sawtooth * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      v_triangle = (ags_v8double) {
	(((((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * v_osc_0_frequency[0] / samplerate * 2.0) - (((double) ((((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * v_osc_0_frequency[0] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * v_osc_0_frequency[1] / samplerate * 2.0) - (((double) ((((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * v_osc_0_frequency[1] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * v_osc_0_frequency[2] / samplerate * 2.0) - (((double) ((((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * v_osc_0_frequency[2] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * v_osc_0_frequency[3] / samplerate * 2.0) - (((double) ((((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * v_osc_0_frequency[3] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * v_osc_0_frequency[4] / samplerate * 2.0) - (((double) ((((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * v_osc_0_frequency[4] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * v_osc_0_frequency[5] / samplerate * 2.0) - (((double) ((((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * v_osc_0_frequency[5] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * v_osc_0_frequency[6] / samplerate * 2.0) - (((double) ((((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * v_osc_0_frequency[6] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * v_osc_0_frequency[7] / samplerate * 2.0) - (((double) ((((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * v_osc_0_frequency[7] / samplerate)) / 2.0) * 2.0) - 1.0)),
      };
    
      v_buffer += (v_triangle * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      v_square = (ags_v8double) {
	((sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
      };
    
      v_buffer += (v_square * v_osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      v_impulse = (ags_v8double) {
	(sin((gdouble) ((offset + i) + (v_osc_0_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[0]))) * 2.0 * M_PI * v_osc_0_frequency[0] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 1) + (v_osc_0_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[1]))) * 2.0 * M_PI * v_osc_0_frequency[1] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 2) + (v_osc_0_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[2]))) * 2.0 * M_PI * v_osc_0_frequency[2] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 3) + (v_osc_0_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[3]))) * 2.0 * M_PI * v_osc_0_frequency[3] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 4) + (v_osc_0_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[4]))) * 2.0 * M_PI * v_osc_0_frequency[4] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 5) + (v_osc_0_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[5]))) * 2.0 * M_PI * v_osc_0_frequency[5] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 6) + (v_osc_0_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[6]))) * 2.0 * M_PI * v_osc_0_frequency[6] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 7) + (v_osc_0_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_0_frequency[7]))) * 2.0 * M_PI * v_osc_0_frequency[7] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0)
      };
    
      v_buffer += (v_impulse * v_osc_0_volume);
    }
    
    *(source) = (gdouble) v_buffer[0];
    *(source += source_stride) = (gdouble) v_buffer[1];
    *(source += source_stride) = (gdouble) v_buffer[2];
    *(source += source_stride) = (gdouble) v_buffer[3];
    *(source += source_stride) = (gdouble) v_buffer[4];
    *(source += source_stride) = (gdouble) v_buffer[5];
    *(source += source_stride) = (gdouble) v_buffer[6];
    *(source += source_stride) = (gdouble) v_buffer[7];

    source += source_stride;

    i += 8;
  }
#endif
  
  for(; i < modular_synth_util->buffer_length;){
    /* OSC-0 */
    osc_0_frequency = modular_synth_util->osc_0_frequency;

    osc_0_phase = modular_synth_util->osc_0_phase;

    osc_0_volume = modular_synth_util->osc_0_volume;

    /* env-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_0_frequency);
    }

    /* env-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = (((gdouble *) modular_synth_util->env_0_buffer)[i] * 2.0 * M_PI) + (osc_0_phase);
    }

    /* env-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_0_volume);
    }

    /* env-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (osc_0_frequency);
    }

    /* env-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = (((gdouble *) modular_synth_util->env_1_buffer)[i] * 2.0 * M_PI) + (osc_0_phase);
    }

    /* env-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (modular_synth_util->osc_0_volume);
    }

    /* lfo-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* lfo-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* lfo-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_0_volume);
    }

    /* lfo-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* lfo-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* lfo-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_0_volume);
    }

    /* noise to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* noise to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* noise to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (modular_synth_util->osc_0_volume);
    }

    /* compute osc-0 */
    if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      source[0] = (gdouble) ((source[0]) + (sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) * osc_0_volume));
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      source[0] = (gdouble) ((source[0]) + ((fmod(((gdouble) (offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))), (samplerate / osc_0_frequency)) * 2.0 * osc_0_frequency / samplerate) - 1.0) * osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      source[0] = (gdouble) ((source[0]) + (((((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * osc_0_frequency / samplerate * 2.0) - (((double) ((((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * osc_0_frequency / samplerate)) / 2.0) * 2.0) - 1.0) * osc_0_volume));
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      source[0] = (gdouble) ((source[0]) + ((sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * osc_0_volume);
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      source[0] = (gdouble) ((source[0]) + (sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * osc_0_volume);
    }

    source += source_stride;
    i++;
  }

  /* OSC-1 */
  source = modular_synth_util->source;

  offset = modular_synth_util->offset;

  i = 0;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = modular_synth_util->buffer_length - (modular_synth_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_sine;
    ags_v8double v_sawtooth;
    ags_v8double v_triangle;
    ags_v8double v_square;
    ags_v8double v_impulse;
    ags_v8double v_buffer;

    ags_v8double v_osc_1_frequency;
    ags_v8double tmp_v_osc_1_frequency;
    ags_v8double v_osc_1_phase;
    ags_v8double tmp_v_osc_1_phase;
    ags_v8double v_osc_1_volume;
    ags_v8double tmp_v_osc_1_volume;
  
    tmp_source = source;
    
    v_buffer = (ags_v8double) {
      (gdouble) *(tmp_source),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride),
      (gdouble) *(tmp_source += source_stride)
    };

    v_osc_1_frequency = (ags_v8double) {
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency,
      osc_1_frequency
    };

    v_osc_1_phase = (ags_v8double) {
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase,
      osc_1_phase
    };

    v_osc_1_volume = (ags_v8double) {
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume,
      osc_1_volume
    };

    /* env-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }
      
    /* env-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* env-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_0_buffer)[i],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_0_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* env-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* env-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* env-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->env_1_buffer)[i],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->env_1_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* lfo-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }
      
    /* lfo-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* lfo-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_0_buffer)[i],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_0_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* lfo-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* lfo-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* lfo-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->lfo_1_buffer)[i],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 1],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 2],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 3],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 4],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 5],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 6],
	((gdouble *) modular_synth_util->lfo_1_buffer)[i + 7]
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* noise to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      tmp_v_osc_1_frequency = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_1_frequency = tmp_v_osc_1_frequency + v_osc_1_frequency;
    }

    /* noise to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      tmp_v_osc_1_phase = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7]
      };

      v_osc_1_phase = tmp_v_osc_1_phase + v_osc_1_phase;
    }

    /* noise to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      tmp_v_osc_1_volume = (ags_v8double) {
	((gdouble *) modular_synth_util->noise_buffer)[i],
	((gdouble *) modular_synth_util->noise_buffer)[i + 1],
	((gdouble *) modular_synth_util->noise_buffer)[i + 2],
	((gdouble *) modular_synth_util->noise_buffer)[i + 3],
	((gdouble *) modular_synth_util->noise_buffer)[i + 4],
	((gdouble *) modular_synth_util->noise_buffer)[i + 5],
	((gdouble *) modular_synth_util->noise_buffer)[i + 6],
	((gdouble *) modular_synth_util->noise_buffer)[i + 7],
      };

      v_osc_1_volume = tmp_v_osc_1_volume + v_osc_1_volume;
    }

    /* compute osc-1 */
    if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      v_sine = (ags_v8double) {
	(gdouble) (sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate)),
	(gdouble) (sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate))
      };
    
      v_buffer += (v_sine * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      v_sawtooth = (ags_v8double) {
	((fmod(((gdouble) (offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[0] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[1] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[2] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[3] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[4] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[5] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[6] / samplerate) - 1.0),
	((fmod(((gdouble) (offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))), (samplerate / v_osc_1_frequency[0])) * 2.0 * v_osc_1_frequency[7] / samplerate) - 1.0)
      };
    
      v_buffer += (v_sawtooth * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      v_triangle = (ags_v8double) {
	(((((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * v_osc_1_frequency[0] / samplerate * 2.0) - (((double) ((((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * v_osc_1_frequency[0] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * v_osc_1_frequency[1] / samplerate * 2.0) - (((double) ((((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * v_osc_1_frequency[1] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * v_osc_1_frequency[2] / samplerate * 2.0) - (((double) ((((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * v_osc_1_frequency[2] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * v_osc_1_frequency[3] / samplerate * 2.0) - (((double) ((((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * v_osc_1_frequency[3] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * v_osc_1_frequency[4] / samplerate * 2.0) - (((double) ((((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * v_osc_1_frequency[4] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * v_osc_1_frequency[5] / samplerate * 2.0) - (((double) ((((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * v_osc_1_frequency[5] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * v_osc_1_frequency[6] / samplerate * 2.0) - (((double) ((((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * v_osc_1_frequency[6] / samplerate)) / 2.0) * 2.0) - 1.0)),
	(((((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * v_osc_1_frequency[7] / samplerate * 2.0) - (((double) ((((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * v_osc_1_frequency[7] / samplerate)) / 2.0) * 2.0) - 1.0)),
      };
    
      v_buffer += (v_triangle * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      v_square = (ags_v8double) {
	((sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
	((sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0),
      };
    
      v_buffer += (v_square * v_osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      v_impulse = (ags_v8double) {
	(sin((gdouble) ((offset + i) + (v_osc_1_phase[0] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[0]))) * 2.0 * M_PI * v_osc_1_frequency[0] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 1) + (v_osc_1_phase[1] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[1]))) * 2.0 * M_PI * v_osc_1_frequency[1] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 2) + (v_osc_1_phase[2] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[2]))) * 2.0 * M_PI * v_osc_1_frequency[2] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 3) + (v_osc_1_phase[3] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[3]))) * 2.0 * M_PI * v_osc_1_frequency[3] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 4) + (v_osc_1_phase[4] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[4]))) * 2.0 * M_PI * v_osc_1_frequency[4] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 5) + (v_osc_1_phase[5] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[5]))) * 2.0 * M_PI * v_osc_1_frequency[5] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 6) + (v_osc_1_phase[6] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[6]))) * 2.0 * M_PI * v_osc_1_frequency[6] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0),
	(sin((gdouble) ((offset + i + 7) + (v_osc_1_phase[7] * ((2.0 * M_PI)) / (samplerate / v_osc_1_frequency[7]))) * 2.0 * M_PI * v_osc_1_frequency[7] / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0)
      };
    
      v_buffer += (v_impulse * v_osc_1_volume);
    }
    
    *(source) = (gdouble) v_buffer[0];
    *(source += source_stride) = (gdouble) v_buffer[1];
    *(source += source_stride) = (gdouble) v_buffer[2];
    *(source += source_stride) = (gdouble) v_buffer[3];
    *(source += source_stride) = (gdouble) v_buffer[4];
    *(source += source_stride) = (gdouble) v_buffer[5];
    *(source += source_stride) = (gdouble) v_buffer[6];
    *(source += source_stride) = (gdouble) v_buffer[7];

    source += source_stride;

    i += 8;
  }
#endif
  
  for(; i < modular_synth_util->buffer_length;){
    /* OSC-1 */
    osc_1_frequency = modular_synth_util->osc_1_frequency;

    osc_1_phase = modular_synth_util->osc_1_phase;

    osc_1_volume = modular_synth_util->osc_1_volume;

    /* env-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = (2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[i] - 1.0) * (osc_1_frequency);
    }

    /* env-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = (((gdouble *) modular_synth_util->env_0_buffer)[i] * 2.0 * M_PI) + (osc_1_phase);
    }

    /* env-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_1_volume);
    }

    /* env-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = (2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[i] - 1.0) * (osc_1_frequency);
    }

    /* env-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = (((gdouble *) modular_synth_util->env_1_buffer)[i] * 2.0 * M_PI) + (osc_1_phase);
    }

    /* env-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (osc_1_volume);
    }

    /* lfo-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* lfo-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* lfo-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_1_volume);
    }

    /* lfo-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* lfo-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* lfo-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_1_volume);
    }
    
    /* noise to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* noise to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* noise to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (modular_synth_util->osc_0_volume);
    }

    /* compute osc-1 */
    if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      source[0] = (gdouble) ((source[0]) + (sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) * osc_1_volume));
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      source[0] = (gdouble) ((source[0]) + ((fmod(((gdouble) (offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))), (samplerate / osc_1_frequency)) * 2.0 * osc_1_frequency / samplerate) - 1.0) * osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      source[0] = (gdouble) ((source[0]) + (((((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * osc_1_frequency / samplerate * 2.0) - (((double) ((((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * osc_1_frequency / samplerate)) / 2.0) * 2.0) - 1.0) * osc_1_volume));
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      source[0] = (gdouble) ((source[0]) + ((sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * osc_1_volume);
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      source[0] = (gdouble) ((source[0]) + (sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * osc_1_volume);
    }

    source += source_stride;
    i++;
  }

  /* pitch tuning */
  ags_fluid_interpolate_4th_order_util_config();

  source = modular_synth_util->source;

  phase_incr = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->phase_increment;

  base_key = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->base_key;
  tuning = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->tuning;

  offset = modular_synth_util->offset;

  i = 0;

  dsp_phase = 0;
  
  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = source[0];

  end_point1 = source[end_index * source_stride];
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  root_pitch_hz = exp2(((double) base_key - 48.0) / 12.0) * 440.0;

  //TODO:JK: implement me
  
  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gdouble phase_incr;

    gint row;
    
    main_pitch_tuning = modular_synth_util->pitch_tuning;

    /* env-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* env-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_0_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_1_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }
    
    /* noise to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((double *) modular_synth_util->noise_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    phase_incr = (exp2((((double) base_key - 48.0 + ((main_pitch_tuning) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    if(dsp_phase_index + 2 < buffer_length){
      pitch_buffer[dsp_i] = (coeffs_0 * start_point
			     + coeffs_1 * source[dsp_phase_index * source_stride]
			     + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
			     + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);
    }
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    offset += 1;
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gdouble phase_incr;

    gint row;
    
    main_pitch_tuning = modular_synth_util->pitch_tuning;

    /* env-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* env-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_0_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_1_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }
    
    /* noise to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((double *) modular_synth_util->noise_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    phase_incr = (exp2((((double) base_key - 48.0 + ((main_pitch_tuning) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    if(dsp_phase_index - 1 > 0 &&
       dsp_phase_index + 2 < buffer_length){
      pitch_buffer[dsp_i] = (coeffs_0 * source[(dsp_phase_index - 1) * source_stride]
			     + coeffs_1 * source[dsp_phase_index * source_stride]
			     + coeffs_2 * source[(dsp_phase_index + 1) * source_stride]
			     + coeffs_3 * source[(dsp_phase_index + 2) * source_stride]);
    }
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    offset += 1;
  }

  /* fill pitch */
  for(i = 0; i < buffer_length; i++){
    source[i * source_stride] = (gdouble) pitch_buffer[i];
  }
  
  /* volume */
  source = modular_synth_util->source;

  offset = modular_synth_util->offset;

  i = 0;

  //TODO:JK: implement me

  for(; i < modular_synth_util->buffer_length;){
    main_volume = modular_synth_util->volume;
    
    /* env-1 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (main_volume);
    }

    /* env-0 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (main_volume);
    }

    /* lfo-0 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }

    /* lfo-1 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }
    
    /* noise to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }

    /* volume */
    source[0] = (gdouble) (((double) source[0]) * main_volume);

    source += source_stride;
    i++;
  }
}

void
ags_modular_synth_util_compute_complex(AgsModularSynthUtil *modular_synth_util)
{
  AgsComplex *source, *tmp_source;

  guint source_stride;
  guint buffer_length;
  guint samplerate;

  guint nth_sends;
  gboolean env_0_has_sends;
  gboolean env_1_has_sends;
  gboolean lfo_0_has_sends;
  gboolean lfo_1_has_sends;
  gboolean noise_has_sends;

  gdouble env_volume;
  gdouble env_amount;

  gdouble osc_0_frequency;
  gdouble osc_0_phase;
  gdouble osc_0_volume;

  gdouble osc_1_frequency;
  gdouble osc_1_phase;
  gdouble osc_1_volume;

  gdouble main_pitch_tuning;

  gdouble *pitch_buffer;

  gdouble base_key;
  gdouble tuning;
  gdouble root_pitch_hz;

  gdouble phase_incr;
  guint64 dsp_phase;
  guint64 dsp_phase_incr;
  guint dsp_i;
  guint dsp_phase_index;
  guint start_index, end_index;
  double _Complex start_point, end_point1, end_point2;
  gdouble coeffs_0, coeffs_1, coeffs_2, coeffs_3;
    
  gdouble main_volume;

  gdouble volume;
  gdouble tmp_volume, end_volume;

  guint offset;
  guint tmp_offset, end_offset;
  guint compute_frame_count;
  guint frame_count;
  guint i, i_stop;
  
  if(modular_synth_util == NULL ||
     modular_synth_util->source == NULL){
    return;
  }

  source = modular_synth_util->source;

  source_stride = modular_synth_util->source_stride;

  buffer_length = modular_synth_util->buffer_length;

  samplerate = modular_synth_util->samplerate;

  if(samplerate == 0){
    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  }
  
  osc_0_frequency = modular_synth_util->osc_0_frequency;
  osc_0_phase = modular_synth_util->osc_0_phase;
  osc_0_volume = modular_synth_util->osc_0_volume;
  
  if(osc_0_frequency == 0.0){
    osc_0_frequency = 0.01;
  }

  osc_1_frequency = modular_synth_util->osc_1_frequency;
  osc_1_phase = modular_synth_util->osc_1_phase;
  osc_1_volume = modular_synth_util->osc_1_volume;
  
  if(osc_1_frequency == 0.0){
    osc_1_frequency = 0.01;
  }
  
  pitch_buffer = modular_synth_util->pitch_buffer;
  
  frame_count = modular_synth_util->frame_count;
  offset = modular_synth_util->offset;

  i = 0;

  /* env-0 */
  env_0_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->env_0_sends[nth_sends] != 0){
      env_0_has_sends = TRUE;

      break;
    }
  }

  if(env_0_has_sends){
    for(i = 0; i < modular_synth_util->buffer_length; i++){
      ((gdouble *) modular_synth_util->env_0_buffer)[i] = 1.0;
    }
    
    ags_envelope_util_set_source(modular_synth_util->env_0_util,
				 modular_synth_util->env_0_buffer);

    ags_envelope_util_set_destination(modular_synth_util->env_0_util,
				      modular_synth_util->env_0_buffer);

    ags_envelope_util_set_buffer_length(modular_synth_util->env_0_util,
					modular_synth_util->buffer_length);

    ags_envelope_util_set_format(modular_synth_util->env_0_util,
				 AGS_SOUNDCARD_DOUBLE);

    tmp_offset = offset;

    compute_frame_count = ((double) frame_count / 4.0);

    while(tmp_offset < frame_count){
      /* env-0 attack */
      env_amount = 0.0;
      env_volume = 1.0;

      if((double) tmp_offset >= 0.0 &&
	 (double) tmp_offset < (double) frame_count / 4.0){
	env_volume = modular_synth_util->env_0_attack;

	end_offset = ((double) frame_count / 4.0);

	end_volume = modular_synth_util->env_0_decay;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - 0);

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 4.0 &&
	       (double) tmp_offset < (double) frame_count / 2.0){
	env_volume = modular_synth_util->env_0_decay;

	end_offset = ((double) frame_count / 2.0);

	end_volume = modular_synth_util->env_0_sustain;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 2.0 &&
	       (double) tmp_offset < (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_0_sustain;

	end_offset = ((double) frame_count * 3.0 / 4.0);

	end_volume = modular_synth_util->env_0_release;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 2.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_0_release;

	end_offset = ((double) frame_count);

	end_volume = modular_synth_util->env_0_release;
      
	if(end_offset == 0 ||
	   end_offset >= frame_count){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count * 3.0 / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else{
	env_amount = 0.0;
	env_volume = modular_synth_util->env_0_release;
      }

      if(frame_count - tmp_offset > 0){
	compute_frame_count = frame_count - tmp_offset;
      }else{
	compute_frame_count = ((double) frame_count / 4.0);
      }
      
      if(compute_frame_count > (guint) ((double) frame_count / 4.0)){
	compute_frame_count = (guint) ((double) frame_count / 4.0);
      }
      
      if((tmp_offset - offset) + compute_frame_count > modular_synth_util->buffer_length){
	compute_frame_count = modular_synth_util->buffer_length - (tmp_offset - offset);
      }
      
      if(tmp_offset > 0){
	if(tmp_offset - offset >= modular_synth_util->buffer_length){
	  break;
	}
      }

      ags_envelope_util_set_buffer_length(modular_synth_util->env_0_util,
					  compute_frame_count);

      ags_envelope_util_set_source(modular_synth_util->env_0_util,
				   ((gdouble *) modular_synth_util->env_0_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_destination(modular_synth_util->env_0_util,
					((gdouble *) modular_synth_util->env_0_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_offset(modular_synth_util->env_0_util,
				   0);

      ags_envelope_util_set_volume(modular_synth_util->env_0_util,
				   env_volume);

      ags_envelope_util_set_amount(modular_synth_util->env_0_util,
				   env_amount);

      ags_envelope_util_compute(modular_synth_util->env_0_util);

      /* iterate */
      tmp_offset += (guint) compute_frame_count;
    }
  }

  /* env-1 */
  env_1_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->env_1_sends[nth_sends] != 0){
      env_1_has_sends = TRUE;

      break;
    }
  }

  if(env_1_has_sends){
    for(i = 0; i < modular_synth_util->buffer_length; i++){
      ((gdouble *) modular_synth_util->env_1_buffer)[i] = 1.0;
    }
    
    ags_envelope_util_set_source(modular_synth_util->env_1_util,
				 modular_synth_util->env_1_buffer);

    ags_envelope_util_set_destination(modular_synth_util->env_1_util,
				      modular_synth_util->env_1_buffer);

    ags_envelope_util_set_buffer_length(modular_synth_util->env_1_util,
					modular_synth_util->buffer_length);

    ags_envelope_util_set_format(modular_synth_util->env_1_util,
				 AGS_SOUNDCARD_DOUBLE);

    tmp_offset = offset;

    compute_frame_count = ((double) frame_count / 4.0);
    
    while(tmp_offset < frame_count){
      /* env-1 attack */
      env_amount = 0.0;
      env_volume = 1.0;

      if((double) tmp_offset >= 0.0 &&
	 (double) tmp_offset < (double) frame_count / 4.0){
	env_volume = modular_synth_util->env_1_attack;

	end_offset = ((double) frame_count / 4.0);

	end_volume = modular_synth_util->env_1_decay;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - 0);

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 4.0 &&
	       (double) tmp_offset < (double) frame_count / 2.0){
	env_volume = modular_synth_util->env_1_decay;

	end_offset = ((double) frame_count / 2.0);

	end_volume = modular_synth_util->env_1_sustain;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count / 2.0 &&
	       (double) tmp_offset < (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_1_sustain;

	end_offset = ((double) frame_count * 3.0 / 4.0);

	end_volume = modular_synth_util->env_1_release;
      
	if(end_offset == 0){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count / 2.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else if((double) tmp_offset >= (double) frame_count * 3.0 / 4.0){
	env_volume = modular_synth_util->env_1_release;

	end_offset = ((double) frame_count);

	end_volume = modular_synth_util->env_1_release;
      
	if(end_offset == 0 ||
	   end_offset >= frame_count){
	  env_amount = 0.0;
	}else{
	  env_amount = (end_volume - env_volume) / (end_offset - ((double) frame_count * 3.0 / 4.0));

	  tmp_volume = env_volume + (env_amount * (tmp_offset - offset));

	  env_volume = tmp_volume;
	}
      }else{
	env_amount = 0.0;
	env_volume = modular_synth_util->env_0_release;
      }

      if(frame_count - tmp_offset > 0){
	compute_frame_count = frame_count - tmp_offset;
      }else{
	compute_frame_count = ((double) frame_count / 4.0);
      }
      
      if(compute_frame_count > (guint) ((double) frame_count / 4.0)){
	compute_frame_count = (guint) ((double) frame_count / 4.0);
      }
      
      if((tmp_offset - offset) + compute_frame_count > modular_synth_util->buffer_length){
	compute_frame_count = modular_synth_util->buffer_length - (tmp_offset - offset);
      }
      
      if(tmp_offset > 0){
	if(tmp_offset - offset >= modular_synth_util->buffer_length){
	  break;
	}
      }

      ags_envelope_util_set_buffer_length(modular_synth_util->env_1_util,
					  compute_frame_count);

      ags_envelope_util_set_source(modular_synth_util->env_1_util,
				   ((gdouble *) modular_synth_util->env_1_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_destination(modular_synth_util->env_1_util,
					((gdouble *) modular_synth_util->env_1_buffer) + (tmp_offset - offset));

      ags_envelope_util_set_offset(modular_synth_util->env_1_util,
				   tmp_offset);

      ags_envelope_util_set_volume(modular_synth_util->env_1_util,
				   env_volume);

      ags_envelope_util_set_amount(modular_synth_util->env_1_util,
				   env_amount);

      ags_envelope_util_compute(modular_synth_util->env_1_util);

      /* iterate */
      tmp_offset += (guint) compute_frame_count;
    }
  }

  /* LFO-0 */
  lfo_0_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->lfo_0_sends[nth_sends] != 0){
      lfo_0_has_sends = TRUE;

      break;
    }
  }

  if(lfo_0_has_sends){
    ags_lfo_synth_util_set_source(modular_synth_util->lfo_0_util,
				  modular_synth_util->lfo_0_buffer);

    ags_lfo_synth_util_set_buffer_length(modular_synth_util->lfo_0_util,
					 modular_synth_util->buffer_length);

    ags_lfo_synth_util_set_format(modular_synth_util->lfo_0_util,
				  AGS_SOUNDCARD_DOUBLE);

    ags_lfo_synth_util_set_samplerate(modular_synth_util->lfo_0_util,
				      samplerate);

    ags_lfo_synth_util_set_lfo_synth_oscillator_mode(modular_synth_util->lfo_0_util,
						     modular_synth_util->lfo_0_oscillator);

    ags_lfo_synth_util_set_frequency(modular_synth_util->lfo_0_util,
				     modular_synth_util->lfo_0_frequency);

    ags_lfo_synth_util_set_lfo_depth(modular_synth_util->lfo_0_util,
				     modular_synth_util->lfo_0_depth);
  
    ags_lfo_synth_util_set_tuning(modular_synth_util->lfo_0_util,
				      modular_synth_util->lfo_0_tuning);

    ags_lfo_synth_util_set_offset(modular_synth_util->lfo_0_util,
				  offset);

    switch(modular_synth_util->lfo_0_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
      {
	ags_lfo_synth_util_compute_sin(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
      {
	ags_lfo_synth_util_compute_sawtooth(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
      {
	ags_lfo_synth_util_compute_triangle(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
      {
	ags_lfo_synth_util_compute_square(modular_synth_util->lfo_0_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
      {
	ags_lfo_synth_util_compute_impulse(modular_synth_util->lfo_0_util);
      }
      break;
    }
  }

  /* LFO-1 */
  lfo_1_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->lfo_1_sends[nth_sends] != 0){
      lfo_1_has_sends = TRUE;

      break;
    }
  }

  if(lfo_1_has_sends){
    ags_lfo_synth_util_set_source(modular_synth_util->lfo_1_util,
				  modular_synth_util->lfo_1_buffer);

    ags_lfo_synth_util_set_buffer_length(modular_synth_util->lfo_1_util,
					 modular_synth_util->buffer_length);

    ags_lfo_synth_util_set_format(modular_synth_util->lfo_1_util,
				  AGS_SOUNDCARD_DOUBLE);

    ags_lfo_synth_util_set_samplerate(modular_synth_util->lfo_1_util,
				      samplerate);

    ags_lfo_synth_util_set_lfo_synth_oscillator_mode(modular_synth_util->lfo_1_util,
						     modular_synth_util->lfo_1_oscillator);

    ags_lfo_synth_util_set_frequency(modular_synth_util->lfo_1_util,
				     modular_synth_util->lfo_1_frequency);

    ags_lfo_synth_util_set_lfo_depth(modular_synth_util->lfo_1_util,
				     modular_synth_util->lfo_1_depth);
  
    ags_lfo_synth_util_set_tuning(modular_synth_util->lfo_1_util,
				  modular_synth_util->lfo_1_tuning);

    ags_lfo_synth_util_set_offset(modular_synth_util->lfo_1_util,
				  offset);

    switch(modular_synth_util->lfo_1_oscillator){
    case AGS_SYNTH_OSCILLATOR_SIN:
      {
	ags_lfo_synth_util_compute_sin(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
      {
	ags_lfo_synth_util_compute_sawtooth(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
      {
	ags_lfo_synth_util_compute_triangle(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
      {
	ags_lfo_synth_util_compute_square(modular_synth_util->lfo_1_util);
      }
      break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
      {
	ags_lfo_synth_util_compute_impulse(modular_synth_util->lfo_1_util);
      }
      break;
    }
  }
  
  /* noise */
  noise_has_sends = FALSE;
  
  for(nth_sends = 0; nth_sends < AGS_MODULAR_SYNTH_SENDS_COUNT; nth_sends++){
    if(modular_synth_util->noise_sends[nth_sends] != 0){
      noise_has_sends = TRUE;

      break;
    }
  }

  if(noise_has_sends){
    ags_noise_util_set_source(modular_synth_util->noise_util,
			      modular_synth_util->noise_buffer);

    ags_noise_util_set_buffer_length(modular_synth_util->noise_util,
				     modular_synth_util->buffer_length);

    ags_noise_util_set_format(modular_synth_util->noise_util,
			      AGS_SOUNDCARD_DOUBLE);

    ags_noise_util_set_samplerate(modular_synth_util->noise_util,
				  samplerate);

    ags_noise_util_set_volume(modular_synth_util->noise_util,
			      modular_synth_util->noise_gain);

    ags_noise_util_set_frequency(modular_synth_util->noise_util,
				 modular_synth_util->noise_frequency);

    ags_noise_util_set_offset(modular_synth_util->noise_util,
			      offset);

    ags_noise_util_compute(modular_synth_util->noise_util);
  }
  
  for(; i < modular_synth_util->buffer_length;){
    /* OSC-0 */
    osc_0_frequency = modular_synth_util->osc_0_frequency;

    osc_0_phase = modular_synth_util->osc_0_phase;

    osc_0_volume = modular_synth_util->osc_0_volume;

    /* env-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_0_frequency);
    }

    /* env-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = (((gdouble *) modular_synth_util->env_0_buffer)[i] * 2.0 * M_PI) + (osc_0_phase);
    }

    /* env-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_0_volume);
    }

    /* env-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (osc_0_frequency);
    }

    /* env-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = (((gdouble *) modular_synth_util->env_1_buffer)[i] * 2.0 * M_PI) + (osc_0_phase);
    }

    /* env-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (modular_synth_util->osc_0_volume);
    }

    /* lfo-0 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* lfo-0 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* lfo-0 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_0_volume);
    }

    /* lfo-1 to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* lfo-1 to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* lfo-1 to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_0_volume);
    }

    /* noise to osc-0 frequency */
    if(ags_modular_synth_util_osc_0_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_0_FREQUENCY)){
      osc_0_frequency = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (osc_0_frequency);
    }

    /* noise to osc-0 phase */
    if(ags_modular_synth_util_osc_0_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_0_PHASE)){
      osc_0_phase = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) * M_PI) + (osc_0_phase);
    }

    /* noise to osc-0 volume */
    if(ags_modular_synth_util_osc_0_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_0_VOLUME)){
      osc_0_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (modular_synth_util->osc_0_volume);
    }

   /* compute osc-0 */
    if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      ags_complex_set(source,
		      (ags_complex_get(source) + (sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate))));
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      ags_complex_set(source,
		      (ags_complex_get(source) + ((fmod(((gdouble) (offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))), (samplerate / osc_0_frequency)) * 2.0 * osc_0_frequency / samplerate) - 1.0)));
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      ags_complex_set(source,
		      (ags_complex_get(source) + (((((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * osc_0_frequency / samplerate * 2.0) - (((double) ((((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * osc_0_frequency / samplerate)) / 2.0) * 2.0) - 1.0))));
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      ags_complex_set(source,
		      (ags_complex_get(source) + ((sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * osc_0_volume));
    }else if(modular_synth_util->osc_0_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      ags_complex_set(source,
		      (ags_complex_get(source) + (sin((gdouble) ((offset + i) + (osc_0_phase * (2.0 * M_PI) / (samplerate / osc_0_frequency))) * 2.0 * M_PI * osc_0_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * osc_0_volume));
    }

    source += source_stride;
    i++;
  }

  /* OSC-1 */
  source = modular_synth_util->source;

  offset = modular_synth_util->offset;

  i = 0;
  
  for(; i < modular_synth_util->buffer_length;){
    /* OSC-1 */
    osc_1_frequency = modular_synth_util->osc_1_frequency;

    osc_1_phase = modular_synth_util->osc_1_phase;

    osc_1_volume = modular_synth_util->osc_1_volume;

    /* env-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = (2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[i] - 1.0) * (osc_1_frequency);
    }

    /* env-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = (((gdouble *) modular_synth_util->env_0_buffer)[i] * 2.0 * M_PI) + (osc_1_phase);
    }

    /* env-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (osc_1_volume);
    }

    /* env-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->env_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = (2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[i] - 1.0) * (osc_1_frequency);
    }

    /* env-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->env_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = (((gdouble *) modular_synth_util->env_1_buffer)[i] * 2.0 * M_PI) + (osc_1_phase);
    }

    /* env-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (osc_1_volume);
    }

    /* lfo-0 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_0_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* lfo-0 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_0_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* lfo-0 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (osc_1_volume);
    }

    /* lfo-1 to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->lfo_1_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* lfo-1 to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->lfo_1_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* lfo-1 to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (osc_1_volume);
    }
    
    /* noise to osc-1 frequency */
    if(ags_modular_synth_util_osc_1_frequency_receives(modular_synth_util,
						       modular_synth_util->noise_sends,
						       AGS_MODULAR_SYNTH_SENDS_OSC_1_FREQUENCY)){
      osc_1_frequency = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (osc_1_frequency);
    }

    /* noise to osc-1 phase */
    if(ags_modular_synth_util_osc_1_phase_receives(modular_synth_util,
						   modular_synth_util->noise_sends,
						   AGS_MODULAR_SYNTH_SENDS_OSC_1_PHASE)){
      osc_1_phase = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) * M_PI) + (osc_1_phase);
    }

    /* noise to osc-1 volume */
    if(ags_modular_synth_util_osc_1_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_OSC_1_VOLUME)){
      osc_1_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (modular_synth_util->osc_0_volume);
    }

    /* compute osc-1 */
    if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SIN){
      ags_complex_set(source,
		      (ags_complex_get(source) + (sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate)) * osc_1_volume));
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SAWTOOTH){
      ags_complex_set(source,
		      (ags_complex_get(source) + ((fmod(((gdouble) (offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))), (samplerate / osc_1_frequency)) * 2.0 * osc_1_frequency / samplerate) - 1.0) * osc_1_volume));
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_TRIANGLE){
      ags_complex_set(source,
		      (ags_complex_get(source) + (((((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * osc_1_frequency / samplerate * 2.0) - (((double) ((((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * osc_1_frequency / samplerate)) / 2.0) * 2.0) - 1.0)) * osc_1_volume));
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_SQUARE){
      ags_complex_set(source,
		      (ags_complex_get(source) + ((sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * osc_1_volume));
    }else if(modular_synth_util->osc_1_oscillator == AGS_SYNTH_OSCILLATOR_IMPULSE){
      ags_complex_set(source,
		      (ags_complex_get(source) + (sin((gdouble) ((offset + i) + (osc_1_phase * (2.0 * M_PI) / (samplerate / osc_1_frequency))) * 2.0 * M_PI * osc_1_frequency / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0) ? 1.0: -1.0) * osc_1_volume));
    }
    
    source += source_stride;
    i++;
  }

  /* pitch tuning */
  ags_fluid_interpolate_4th_order_util_config();

  source = modular_synth_util->source;

  phase_incr = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->phase_increment;

  base_key = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->base_key;
  tuning = AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(modular_synth_util->pitch_util)->tuning;

  offset = modular_synth_util->offset;

  i = 0;

  dsp_phase = 0;
  
  /* Convert playback "speed" floating point value to phase index/fract */
  ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);

  end_index = buffer_length - 1;

  start_index = 0;
  start_point = ags_complex_get(source);

  end_point1 = ags_complex_get(source + (end_index * source_stride));
  end_point2 = end_point1;

  dsp_i = 0;

  dsp_phase_index = ags_fluid_phase_index(dsp_phase);

  root_pitch_hz = exp2(((double) base_key - 48.0) / 12.0) * 440.0;

  //TODO:JK: implement me
  
  /* interpolate first sample point (start or loop start) if needed */
  for(; dsp_phase_index == start_index && dsp_i < buffer_length; dsp_i++){
    gdouble phase_incr;

    gint row;
    
    main_pitch_tuning = modular_synth_util->pitch_tuning;

    /* env-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* env-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_0_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_1_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }
    
    /* noise to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((double *) modular_synth_util->noise_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    phase_incr = (exp2((((double) base_key - 48.0 + ((main_pitch_tuning) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);
    
    if(dsp_phase_index + 2 < buffer_length){
      pitch_buffer[dsp_i] = (coeffs_0 * start_point
			     + coeffs_1 * ags_complex_get(source + (dsp_phase_index * source_stride))
			     + coeffs_2 * ags_complex_get(source + (dsp_phase_index + 1) * source_stride)
			     + coeffs_3 * ags_complex_get(source + (dsp_phase_index + 2) * source_stride));
    }
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    offset += 1;
  }
  
  /* interpolate the sequence of sample points */
  for(; dsp_i < buffer_length && dsp_phase_index <= end_index; dsp_i++)
  {
    gdouble phase_incr;

    gint row;
    
    main_pitch_tuning = modular_synth_util->pitch_tuning;

    /* env-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_1_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* env-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = (((2.0 * ((gdouble *) modular_synth_util->env_0_buffer)[dsp_i]) - 1.0) * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-0 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_0_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    /* lfo-1 to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((gdouble *) modular_synth_util->lfo_1_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }
    
    /* noise to pitch tuning */
    if(ags_modular_synth_util_pitch_tuning_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_PITCH_TUNING)){
      main_pitch_tuning = ((((double *) modular_synth_util->noise_buffer)[dsp_i] + 1.0) / 2.0 * 1200.0) + (main_pitch_tuning);
    }

    phase_incr = (exp2((((double) base_key - 48.0 + ((main_pitch_tuning) / 100.0)) / 12.0)) * 440.0) / root_pitch_hz;
  
    if(phase_incr == 0.0){
      phase_incr = 1.0;
    }

    ags_fluid_phase_set_float(dsp_phase_incr, phase_incr);
    
    dsp_phase_index = ags_fluid_phase_index_round(dsp_phase);

    row = ags_fluid_phase_fract_to_tablerow(dsp_phase);
    
    g_mutex_lock(&interp_coeff_4th_order_mutex);
    
    coeffs_0 = interp_coeff_4th_order[row][0];
    coeffs_1 = interp_coeff_4th_order[row][1];
    coeffs_2 = interp_coeff_4th_order[row][2];
    coeffs_3 = interp_coeff_4th_order[row][3];
    
    g_mutex_unlock(&interp_coeff_4th_order_mutex);

    if(dsp_phase_index - 1 > 0 &&
       dsp_phase_index + 2 < buffer_length){
      pitch_buffer[dsp_i] = (coeffs_0 * ags_complex_get(source + ((dsp_phase_index - 1) * source_stride))
			     + coeffs_1 * ags_complex_get(source + (dsp_phase_index * source_stride))
			     + coeffs_2 * ags_complex_get(source + ((dsp_phase_index + 1) * source_stride))
			     + coeffs_3 * ags_complex_get(source + ((dsp_phase_index + 2) * source_stride)));
    }
    
    /* increment phase */
    ags_fluid_phase_incr(dsp_phase, dsp_phase_incr);
    dsp_phase_index = ags_fluid_phase_index(dsp_phase);

    offset += 1;
  }

  /* fill pitch */
  for(i = 0; i < buffer_length; i++){
    ags_complex_set(source + (i * source_stride),
		    pitch_buffer[i]);
  }
  
  /* volume */
  source = modular_synth_util->source;

  offset = modular_synth_util->offset;

  i = 0;

  //TODO:JK: implement me

  for(; i < modular_synth_util->buffer_length;){
    main_volume = modular_synth_util->volume;
    
    /* env-1 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->env_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((gdouble *) modular_synth_util->env_1_buffer)[i] * (main_volume);
    }

    /* env-0 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->env_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((gdouble *) modular_synth_util->env_0_buffer)[i] * (main_volume);
    }

    /* lfo-0 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_0_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->lfo_0_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }

    /* lfo-1 to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->lfo_1_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->lfo_1_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }
    
    /* noise to volume */
    if(ags_modular_synth_util_volume_receives(modular_synth_util,
						    modular_synth_util->noise_sends,
						    AGS_MODULAR_SYNTH_SENDS_VOLUME)){
      main_volume = ((((gdouble *) modular_synth_util->noise_buffer)[i] + 1.0) / 2.0) * (main_volume);
    }

    /* volume */
    ags_complex_set(source,
		    ags_complex_get(source) * main_volume);

    source += source_stride;
    i++;
  }
}

void
ags_modular_synth_util_compute(AgsModularSynthUtil *modular_synth_util)
{
  if(modular_synth_util->format == AGS_SOUNDCARD_SIGNED_8_BIT){
    ags_modular_synth_util_compute_s8(modular_synth_util);
  }else if(modular_synth_util->format == AGS_SOUNDCARD_SIGNED_16_BIT){
    ags_modular_synth_util_compute_s16(modular_synth_util);
  }else if(modular_synth_util->format == AGS_SOUNDCARD_SIGNED_24_BIT){
    ags_modular_synth_util_compute_s24(modular_synth_util);
  }else if(modular_synth_util->format == AGS_SOUNDCARD_SIGNED_32_BIT){
    ags_modular_synth_util_compute_s32(modular_synth_util);
  }else if(modular_synth_util->format == AGS_SOUNDCARD_SIGNED_64_BIT){
    ags_modular_synth_util_compute_s64(modular_synth_util);
  }else if(modular_synth_util->format == AGS_SOUNDCARD_FLOAT){
    ags_modular_synth_util_compute_float(modular_synth_util);
  }else if(modular_synth_util->format == AGS_SOUNDCARD_DOUBLE){
    ags_modular_synth_util_compute_double(modular_synth_util);
  }else if(modular_synth_util->format == AGS_SOUNDCARD_COMPLEX){
    ags_modular_synth_util_compute_complex(modular_synth_util);
  }
}
