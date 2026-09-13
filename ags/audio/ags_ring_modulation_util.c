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

#include <ags/audio/ags_ring_modulation_util.h>

#include <ags/libags-audio-globals.h>

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


/**
 * SECTION:ags_ring_modulation_util
 * @short_description: frequency modulation synth util
 * @title: AgsRingModulationUtil
 * @section_id:
 * @include: ags/audio/ags_ring_modulation_util.h
 *
 * Utility functions to compute modular synths.
 */

GType
ags_ring_modulation_util_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_ring_modulation_util = 0;

    ags_type_ring_modulation_util =
      g_boxed_type_register_static("AgsRingModulationUtil",
				   (GBoxedCopyFunc) ags_ring_modulation_util_copy,
				   (GBoxedFreeFunc) ags_ring_modulation_util_free);

    g_once_init_leave(&g_define_type_id__static, ags_type_ring_modulation_util);
  }

  return(g_define_type_id__static);
}

/**
 * ags_ring_modulation_util_alloc:
 *
 * Allocate #AgsRingModulationUtil-struct
 *
 * Returns: a new #AgsRingModulationUtil-struct
 *
 * Since: 9.4.0
 */
AgsRingModulationUtil*
ags_ring_modulation_util_alloc()
{
  AgsRingModulationUtil *ptr;

  ptr = (AgsRingModulationUtil *) g_new(AgsRingModulationUtil,
					1);

  ptr[0] = AGS_RING_MODULATION_UTIL_INITIALIZER;

  ptr->pitch_util = ags_fluid_interpolate_4th_order_util_alloc();
  
  ptr->audio_buffer_util = ags_audio_buffer_util_alloc();
  
  return(ptr);
}

/**
 * ags_ring_modulation_util_copy:
 * @ptr: the original #AgsRingModulationUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsRingModulationUtil-struct
 *
 * Since: 9.4.0
 */
gpointer
ags_ring_modulation_util_copy(AgsRingModulationUtil *ptr)
{
  AgsRingModulationUtil *new_ptr;
  
  g_return_val_if_fail(ptr != NULL, NULL);

  new_ptr = (AgsRingModulationUtil *) g_new(AgsRingModulationUtil,
					    1);
  
  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;
  
  return(new_ptr);
}

/**
 * ags_ring_modulation_util_free:
 * @ptr: the #AgsRingModulationUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 9.4.0
 */
void
ags_ring_modulation_util_free(AgsRingModulationUtil *ptr)
{
  g_return_if_fail(ptr != NULL);
 
  g_free(ptr);
}

/**
 * ags_ring_modulation_util_get_destination:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * 
 * Get destination buffer of @ring_modulation_util.
 * 
 * Returns: the destination buffer
 * 
 * Since: 9.4.0
 */
gpointer
ags_ring_modulation_util_get_destination(AgsRingModulationUtil *ring_modulation_util)
{
  if(ring_modulation_util == NULL){
    return(NULL);
  }

  return(ring_modulation_util->destination);
}

/**
 * ags_ring_modulation_util_set_destination:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * @destination: (transfer none): the destination buffer
 *
 * Set @destination buffer of @ring_modulation_util.
 *
 * Since: 9.4.0
 */
void
ags_ring_modulation_util_set_destination(AgsRingModulationUtil *ring_modulation_util,
					 gpointer destination)
{
  if(ring_modulation_util == NULL){
    return;
  }

  ring_modulation_util->destination = destination;
}

/**
 * ags_ring_modulation_util_get_destination_stride:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * 
 * Get destination stride of @ring_modulation_util.
 * 
 * Returns: the destination buffer stride
 * 
 * Since: 9.4.0
 */
guint
ags_ring_modulation_util_get_destination_stride(AgsRingModulationUtil *ring_modulation_util)
{
  if(ring_modulation_util == NULL){
    return(0);
  }

  return(ring_modulation_util->destination_stride);
}

/**
 * ags_ring_modulation_util_set_destination_stride:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * @destination_stride: the destination buffer stride
 *
 * Set @destination stride of @ring_modulation_util.
 *
 * Since: 9.4.0
 */
void
ags_ring_modulation_util_set_destination_stride(AgsRingModulationUtil *ring_modulation_util,
						guint destination_stride)
{
  if(ring_modulation_util == NULL){
    return;
  }

  ring_modulation_util->destination_stride = destination_stride;
}

/**
 * ags_ring_modulation_util_get_modulator_source:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * 
 * Get modulator source buffer of @ring_modulation_util.
 * 
 * Returns: the modulator source buffer
 * 
 * Since: 9.4.0
 */
gpointer
ags_ring_modulation_util_get_modulator_source(AgsRingModulationUtil *ring_modulation_util)
{
  if(ring_modulation_util == NULL){
    return(NULL);
  }

  return(ring_modulation_util->modulator_source);
}

/**
 * ags_ring_modulation_util_set_modulator_source:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * @modulator_source: (transfer none): the modulator source buffer
 *
 * Set @modulator_source buffer of @ring_modulation_util.
 *
 * Since: 9.4.0
 */
void
ags_ring_modulation_util_set_modulator_source(AgsRingModulationUtil *ring_modulation_util,
					      gpointer modulator_source)
{
  if(ring_modulation_util == NULL){
    return;
  }

  ring_modulation_util->modulator_source = modulator_source;
}

/**
 * ags_ring_modulation_util_get_modulator_source_stride:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * 
 * Get modulator source stride of @ring_modulation_util.
 * 
 * Returns: the modulator source buffer stride
 * 
 * Since: 9.4.0
 */
guint
ags_ring_modulation_util_get_modulator_source_stride(AgsRingModulationUtil *ring_modulation_util)
{
  if(ring_modulation_util == NULL){
    return(0);
  }

  return(ring_modulation_util->modulator_source_stride);
}

/**
 * ags_ring_modulation_util_set_modulator_source_stride:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * @modulator_source_stride: the modulator source buffer stride
 *
 * Set @modulator_source stride of @ring_modulation_util.
 *
 * Since: 9.4.0
 */
void
ags_ring_modulation_util_set_modulator_source_stride(AgsRingModulationUtil *ring_modulation_util,
						     guint modulator_source_stride)
{
  if(ring_modulation_util == NULL){
    return;
  }

  ring_modulation_util->modulator_source_stride = modulator_source_stride;
}

/**
 * ags_ring_modulation_util_get_carrier_source:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * 
 * Get carrier source buffer of @ring_modulation_util.
 * 
 * Returns: the carrier source buffer
 * 
 * Since: 9.4.0
 */
gpointer
ags_ring_modulation_util_get_carrier_source(AgsRingModulationUtil *ring_modulation_util)
{
  if(ring_modulation_util == NULL){
    return(NULL);
  }

  return(ring_modulation_util->carrier_source);
}

/**
 * ags_ring_modulation_util_set_carrier_source:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * @carrier_source: (transfer none): the carrier source buffer
 *
 * Set @carrier_source buffer of @ring_modulation_util.
 *
 * Since: 9.4.0
 */
void
ags_ring_modulation_util_set_carrier_source(AgsRingModulationUtil *ring_modulation_util,
					    gpointer carrier_source)
{
  if(ring_modulation_util == NULL){
    return;
  }

  ring_modulation_util->carrier_source = carrier_source;
}

/**
 * ags_ring_modulation_util_get_carrier_source_stride:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * 
 * Get carrier source stride of @ring_modulation_util.
 * 
 * Returns: the carrier source buffer stride
 * 
 * Since: 9.4.0
 */
guint
ags_ring_modulation_util_get_carrier_source_stride(AgsRingModulationUtil *ring_modulation_util)
{
  if(ring_modulation_util == NULL){
    return(0);
  }

  return(ring_modulation_util->carrier_source_stride);
}

/**
 * ags_ring_modulation_util_set_carrier_source_stride:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * @carrier_source_stride: the carrier source buffer stride
 *
 * Set @carrier_source stride of @ring_modulation_util.
 *
 * Since: 9.4.0
 */
void
ags_ring_modulation_util_set_carrier_source_stride(AgsRingModulationUtil *ring_modulation_util,
						   guint carrier_source_stride)
{
  if(ring_modulation_util == NULL){
    return;
  }

  ring_modulation_util->carrier_source_stride = carrier_source_stride;
}

/**
 * ags_ring_modulation_util_get_buffer_length:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * 
 * Get buffer length of @ring_modulation_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 9.4.0
 */
guint
ags_ring_modulation_util_get_buffer_length(AgsRingModulationUtil *ring_modulation_util)
{
  if(ring_modulation_util == NULL){
    return(0);
  }

  return(ring_modulation_util->buffer_length);
}

/**
 * ags_ring_modulation_util_set_buffer_length:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @ring_modulation_util.
 *
 * Since: 9.4.0
 */
void
ags_ring_modulation_util_set_buffer_length(AgsRingModulationUtil *ring_modulation_util,
					   guint buffer_length)
{
  if(ring_modulation_util == NULL ||
     ring_modulation_util->buffer_length == buffer_length){
    return;
  }

  ring_modulation_util->buffer_length = buffer_length;

  ags_common_pitch_util_set_buffer_length(ring_modulation_util->pitch_util,
					  ring_modulation_util->pitch_type,
					  buffer_length);

  /* pitch buffer */
  ags_stream_free(ring_modulation_util->pitch_buffer);
  
  ring_modulation_util->pitch_buffer = NULL;

  if(buffer_length > 0){
    ring_modulation_util->pitch_buffer = ags_stream_alloc(buffer_length,
							  AGS_SOUNDCARD_DOUBLE);
  }
}

/**
 * ags_ring_modulation_util_get_format:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * 
 * Get audio buffer util format of @ring_modulation_util.
 * 
 * Returns: the audio buffer util format
 * 
 * Since: 9.4.0
 */
AgsSoundcardFormat
ags_ring_modulation_util_get_format(AgsRingModulationUtil *ring_modulation_util)
{
  if(ring_modulation_util == NULL){
    return(0);
  }

  return(ring_modulation_util->format);
}

/**
 * ags_ring_modulation_util_set_format:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * @format: the audio buffer util format
 *
 * Set @format of @ring_modulation_util.
 *
 * Since: 9.4.0
 */
void
ags_ring_modulation_util_set_format(AgsRingModulationUtil *ring_modulation_util,
				    AgsSoundcardFormat format)
{
  if(ring_modulation_util == NULL){
    return;
  }

  ring_modulation_util->format = format;

  /*  */
  ags_common_pitch_util_set_format(ring_modulation_util->pitch_util,
				   ring_modulation_util->pitch_type,
				   format);
}

/**
 * ags_ring_modulation_util_get_samplerate:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * 
 * Get samplerate of @ring_modulation_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 9.4.0
 */
guint
ags_ring_modulation_util_get_samplerate(AgsRingModulationUtil *ring_modulation_util)
{
  if(ring_modulation_util == NULL){
    return(0);
  }

  return(ring_modulation_util->samplerate);
}

/**
 * ags_ring_modulation_util_set_samplerate:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @ring_modulation_util.
 *
 * Since: 9.4.0
 */
void
ags_ring_modulation_util_set_samplerate(AgsRingModulationUtil *ring_modulation_util,
					guint samplerate)
{
  if(ring_modulation_util == NULL){
    return;
  }

  ring_modulation_util->samplerate = samplerate;

  ags_common_pitch_util_set_samplerate(ring_modulation_util->pitch_util,
				       ring_modulation_util->pitch_type,
				       samplerate);
}

/**
 * ags_ring_modulation_util_get_pitch_tuning:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * 
 * Get pitch tuning of @ring_modulation_util.
 * 
 * Returns: the pitch tuning
 * 
 * Since: 9.4.0
 */
gdouble
ags_ring_modulation_util_get_pitch_tuning(AgsRingModulationUtil *ring_modulation_util)
{
  if(ring_modulation_util == NULL){
    return(0.0);
  }

  return(ring_modulation_util->pitch_tuning);
}

/**
 * ags_ring_modulation_util_set_pitch_tuning:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * @pitch_tuning: the pitch tuning
 *
 * Set @pitch_tuning of @ring_modulation_util.
 *
 * Since: 9.4.0
 */
void
ags_ring_modulation_util_set_pitch_tuning(AgsRingModulationUtil *ring_modulation_util,
					  gdouble pitch_tuning)
{
  if(ring_modulation_util == NULL){
    return;
  }

  ring_modulation_util->pitch_tuning = pitch_tuning;
}

/**
 * ags_ring_modulation_util_get_drive:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * 
 * Get drive of @ring_modulation_util.
 * 
 * Returns: the drive
 * 
 * Since: 9.4.0
 */
gdouble
ags_ring_modulation_util_get_drive(AgsRingModulationUtil *ring_modulation_util)
{
  if(ring_modulation_util == NULL){
    return(1.0);
  }

  return(ring_modulation_util->drive);
}

/**
 * ags_ring_modulation_util_set_drive:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * @drive: the drive
 *
 * Set @drive of @ring_modulation_util.
 *
 * Since: 9.4.0
 */
void
ags_ring_modulation_util_set_drive(AgsRingModulationUtil *ring_modulation_util,
				   gdouble drive)
{
  if(ring_modulation_util == NULL){
    return;
  }

  ring_modulation_util->drive = drive;
}

/**
 * ags_ring_modulation_util_get_mix:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * 
 * Get mix of @ring_modulation_util.
 * 
 * Returns: the mix
 * 
 * Since: 9.4.0
 */
gdouble
ags_ring_modulation_util_get_mix(AgsRingModulationUtil *ring_modulation_util)
{
  if(ring_modulation_util == NULL){
    return(0.0);
  }

  return(ring_modulation_util->mix);
}

/**
 * ags_ring_modulation_util_set_mix:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * @mix: the mix
 *
 * Set @mix of @ring_modulation_util.
 *
 * Since: 9.4.0
 */
void
ags_ring_modulation_util_set_mix(AgsRingModulationUtil *ring_modulation_util,
				 gdouble mix)
{
  if(ring_modulation_util == NULL){
    return;
  }

  ring_modulation_util->mix = mix;
}

/**
 * ags_ring_modulation_util_get_gain:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * 
 * Get gain of @ring_modulation_util.
 * 
 * Returns: the gain
 * 
 * Since: 9.4.0
 */
gdouble
ags_ring_modulation_util_get_gain(AgsRingModulationUtil *ring_modulation_util)
{
  if(ring_modulation_util == NULL){
    return(1.0);
  }

  return(ring_modulation_util->gain);
}

/**
 * ags_ring_modulation_util_set_gain:
 * @ring_modulation_util: the #AgsRingModulationUtil-struct
 * @gain: the gain
 *
 * Set @gain of @ring_modulation_util.
 *
 * Since: 9.4.0
 */
void
ags_ring_modulation_util_set_gain(AgsRingModulationUtil *ring_modulation_util,
				  gdouble gain)
{
  if(ring_modulation_util == NULL){
    return;
  }

  ring_modulation_util->gain = gain;
}

void
ags_ring_modulation_util_compute_s8(AgsRingModulationUtil *ring_modulation_util)
{
  gpointer pitch_util;
  
  gint8 *destination;
  gint8 *modulator_source, *carrier_source;
  gint8 *pitch_buffer;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  ags_v8double v_buffer, v_buffer_a, v_buffer_b;
#endif
  
  GType pitch_type;
  
  guint modulator_source_stride, carrier_source_stride, destination_stride;

  gint root_note;
  guint buffer_length;
  guint samplerate;
  gdouble pitch_tuning;
  gdouble drive;
  gdouble mix;
  gdouble gain;
  
  guint i, i_stop;

  if(ring_modulation_util == NULL ||
     ring_modulation_util->destination == NULL ||
     ring_modulation_util->source == NULL){
    return;
  }
  
  pitch_util = ring_modulation_util->pitch_util;
  pitch_type = ring_modulation_util->pitch_type;
  
  pitch_buffer = ring_modulation_util->pitch_buffer;

  destination = (gint8 *) ring_modulation_util->destination;
  modulator_source = (gint8 *) ring_modulation_util->modulator_source;
  carrier_source = (gint8 *) ring_modulation_util->carrier_source;
  
  destination_stride = ring_modulation_util->destination_stride;
  modulator_source_stride = ring_modulation_util->modulator_source_stride;
  carrier_source_stride = ring_modulation_util->carrier_source_stride;

  buffer_length = ring_modulation_util->buffer_length;
  samplerate = ring_modulation_util->samplerate;
  
  pitch_tuning = ring_modulation_util->pitch_tuning;

  drive = ring_modulation_util->drive;

  mix = ring_modulation_util->mix;

  gain = ring_modulation_util->gain;
  
  /* pitch */
  if(pitch_tuning == 0.0){
    copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(ring_modulation_util->audio_buffer_util,
								ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
													    ring_modulation_util->format),
								ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
													    ring_modulation_util->format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(ring_modulation_util->audio_buffer_util,
						pitch_buffer, 1, 0,
						modulator_source, 1, 0,
						buffer_length, copy_mode);
  }else{
    ags_common_pitch_util_set_source(pitch_util,
				     pitch_type,
				     modulator_source);

    ags_common_pitch_util_set_destination(pitch_util,
					  pitch_type,
					  pitch_buffer);

    root_note = 60;

    ags_common_pitch_util_set_base_key(pitch_util,
				       pitch_type,
				       (gdouble) root_note - 69.0 + 48.0);
  
    ags_common_pitch_util_set_tuning(pitch_util,
				     pitch_type,
				     pitch_tuning);

    ags_common_pitch_util_set_format(pitch_util,
				     pitch_type,
				     ring_modulation_util->format);

    ags_audio_buffer_util_clear_buffer(ring_modulation_util->audio_buffer_util,
				       pitch_buffer, 1,
				       buffer_length, ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
												  ring_modulation_util->format));
 
    ags_common_pitch_util_pitch(pitch_util,
				pitch_type);
    
    /* reset */
    ags_common_pitch_util_set_source(pitch_util,
				     pitch_type,
				     NULL);
    
    ags_common_pitch_util_set_destination(pitch_util,
					  pitch_type,
					  NULL);
  }
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = ring_modulation_util->buffer_length - (ring_modulation_util->buffer_length % 8);

  for(; i < i_stop;){
    v_buffer_a = (ags_v8double) {
      (gdouble) *(pitch_buffer),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++)
    };

    pitch_buffer++;

    v_buffer_b = (ags_v8double) {
      (gdouble) *(carrier_source),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride)
    };

    carrier_source += carrier_source_stride;

    v_buffer_b /= 127.0;
    v_buffer = gain * (((1.0 - mix) * v_buffer_a) * ((mix) * (drive * v_buffer_b)));

    *(destination) = (gint8) v_buffer[0];
    *(destination += destination_stride) = (gint8) v_buffer[1];
    *(destination += destination_stride) = (gint8) v_buffer[2];
    *(destination += destination_stride) = (gint8) v_buffer[3];
    *(destination += destination_stride) = (gint8) v_buffer[4];
    *(destination += destination_stride) = (gint8) v_buffer[5];
    *(destination += destination_stride) = (gint8) v_buffer[6];
    *(destination += destination_stride) = (gint8) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = gain * (((1.0 - mix) * pitch_buffer[0]) * ((mix) * (drive * carrier_source[0])));

    destination += destination_stride;
    pitch_buffer++;
    carrier_source += carrier_source_stride;
    i++;
  }
}

void
ags_ring_modulation_util_compute_s16(AgsRingModulationUtil *ring_modulation_util)
{
  gpointer pitch_util;
  
  gint16 *destination;
  gint16 *modulator_source, *carrier_source;
  gint16 *pitch_buffer;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  ags_v8double v_buffer, v_buffer_a, v_buffer_b;
#endif
  
  GType pitch_type;
  
  guint modulator_source_stride, carrier_source_stride, destination_stride;

  gint root_note;
  guint buffer_length;
  guint samplerate;
  gdouble pitch_tuning;
  gdouble drive;
  gdouble mix;
  gdouble gain;
  
  guint i, i_stop;

  if(ring_modulation_util == NULL ||
     ring_modulation_util->destination == NULL ||
     ring_modulation_util->source == NULL){
    return;
  }
  
  pitch_util = ring_modulation_util->pitch_util;
  pitch_type = ring_modulation_util->pitch_type;
  
  pitch_buffer = ring_modulation_util->pitch_buffer;

  destination = (gint16 *) ring_modulation_util->destination;
  modulator_source = (gint16 *) ring_modulation_util->modulator_source;
  carrier_source = (gint16 *) ring_modulation_util->carrier_source;
  
  destination_stride = ring_modulation_util->destination_stride;
  modulator_source_stride = ring_modulation_util->modulator_source_stride;
  carrier_source_stride = ring_modulation_util->carrier_source_stride;

  buffer_length = ring_modulation_util->buffer_length;
  samplerate = ring_modulation_util->samplerate;
  
  pitch_tuning = ring_modulation_util->pitch_tuning;

  drive = ring_modulation_util->drive;

  mix = ring_modulation_util->mix;

  gain = ring_modulation_util->gain;
  
  /* pitch */
  if(pitch_tuning == 0.0){
    copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(ring_modulation_util->audio_buffer_util,
								ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
													    ring_modulation_util->format),
								ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
													    ring_modulation_util->format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(ring_modulation_util->audio_buffer_util,
						pitch_buffer, 1, 0,
						modulator_source, 1, 0,
						buffer_length, copy_mode);
  }else{
    ags_common_pitch_util_set_source(pitch_util,
				     pitch_type,
				     modulator_source);

    ags_common_pitch_util_set_destination(pitch_util,
					  pitch_type,
					  pitch_buffer);

    root_note = 60;

    ags_common_pitch_util_set_base_key(pitch_util,
				       pitch_type,
				       (gdouble) root_note - 69.0 + 48.0);
  
    ags_common_pitch_util_set_tuning(pitch_util,
				     pitch_type,
				     pitch_tuning);

    ags_common_pitch_util_set_format(pitch_util,
				     pitch_type,
				     ring_modulation_util->format);

    ags_audio_buffer_util_clear_buffer(ring_modulation_util->audio_buffer_util,
				       pitch_buffer, 1,
				       buffer_length, ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
												  ring_modulation_util->format));
 
    ags_common_pitch_util_pitch(pitch_util,
				pitch_type);
    
    /* reset */
    ags_common_pitch_util_set_source(pitch_util,
				     pitch_type,
				     NULL);
    
    ags_common_pitch_util_set_destination(pitch_util,
					  pitch_type,
					  NULL);
  }
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = ring_modulation_util->buffer_length - (ring_modulation_util->buffer_length % 8);

  for(; i < i_stop;){
    v_buffer_a = (ags_v8double) {
      (gdouble) *(pitch_buffer),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++)
    };

    pitch_buffer++;

    v_buffer_b = (ags_v8double) {
      (gdouble) *(carrier_source),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride)
    };

    carrier_source += carrier_source_stride;

    v_pitch_buffer_b /= (gdouble) G_MAXINT16;
    
    v_buffer = gain * (((1.0 - mix) * v_buffer_a) * ((mix) * (drive * v_buffer_b)));

    *(destination) = (gint16) v_buffer[0];
    *(destination += destination_stride) = (gint16) v_buffer[1];
    *(destination += destination_stride) = (gint16) v_buffer[2];
    *(destination += destination_stride) = (gint16) v_buffer[3];
    *(destination += destination_stride) = (gint16) v_buffer[4];
    *(destination += destination_stride) = (gint16) v_buffer[5];
    *(destination += destination_stride) = (gint16) v_buffer[6];
    *(destination += destination_stride) = (gint16) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = gain * (((1.0 - mix) * pitch_buffer[0]) * ((mix) * (drive * carrier_source[0])));

    destination += destination_stride;
    pitch_buffer++;
    carrier_source += carrier_source_stride;
    i++;
  }
}

void
ags_ring_modulation_util_compute_s24(AgsRingModulationUtil *ring_modulation_util)
{
  gpointer pitch_util;
  
  gint32 *destination;
  gint32 *modulator_source, *carrier_source;
  gint32 *pitch_buffer;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  ags_v8double v_buffer, v_buffer_a, v_buffer_b;
#endif
  
  GType pitch_type;
  
  guint modulator_source_stride, carrier_source_stride, destination_stride;

  gint root_note;
  guint buffer_length;
  guint samplerate;
  gdouble pitch_tuning;
  gdouble drive;
  gdouble mix;
  gdouble gain;
  
  guint i, i_stop;

  if(ring_modulation_util == NULL ||
     ring_modulation_util->destination == NULL ||
     ring_modulation_util->source == NULL){
    return;
  }
  
  pitch_util = ring_modulation_util->pitch_util;
  pitch_type = ring_modulation_util->pitch_type;
  
  pitch_buffer = ring_modulation_util->pitch_buffer;

  destination = (gint32 *) ring_modulation_util->destination;
  modulator_source = (gint32 *) ring_modulation_util->modulator_source;
  carrier_source = (gint32 *) ring_modulation_util->carrier_source;
  
  destination_stride = ring_modulation_util->destination_stride;
  modulator_source_stride = ring_modulation_util->modulator_source_stride;
  carrier_source_stride = ring_modulation_util->carrier_source_stride;

  buffer_length = ring_modulation_util->buffer_length;
  samplerate = ring_modulation_util->samplerate;
  
  pitch_tuning = ring_modulation_util->pitch_tuning;

  drive = ring_modulation_util->drive;

  mix = ring_modulation_util->mix;

  gain = ring_modulation_util->gain;
  
  /* pitch */
  if(pitch_tuning == 0.0){
    copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(ring_modulation_util->audio_buffer_util,
								ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
													    ring_modulation_util->format),
								ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
													    ring_modulation_util->format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(ring_modulation_util->audio_buffer_util,
						pitch_buffer, 1, 0,
						modulator_source, 1, 0,
						buffer_length, copy_mode);
  }else{
    ags_common_pitch_util_set_source(pitch_util,
				     pitch_type,
				     modulator_source);

    ags_common_pitch_util_set_destination(pitch_util,
					  pitch_type,
					  pitch_buffer);

    root_note = 60;

    ags_common_pitch_util_set_base_key(pitch_util,
				       pitch_type,
				       (gdouble) root_note - 69.0 + 48.0);
  
    ags_common_pitch_util_set_tuning(pitch_util,
				     pitch_type,
				     pitch_tuning);

    ags_common_pitch_util_set_format(pitch_util,
				     pitch_type,
				     ring_modulation_util->format);

    ags_audio_buffer_util_clear_buffer(ring_modulation_util->audio_buffer_util,
				       pitch_buffer, 1,
				       buffer_length, ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
												  ring_modulation_util->format));
 
    ags_common_pitch_util_pitch(pitch_util,
				pitch_type);
    
    /* reset */
    ags_common_pitch_util_set_source(pitch_util,
				     pitch_type,
				     NULL);
    
    ags_common_pitch_util_set_destination(pitch_util,
					  pitch_type,
					  NULL);
  }
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = ring_modulation_util->buffer_length - (ring_modulation_util->buffer_length % 8);

  for(; i < i_stop;){
    v_buffer_a = (ags_v8double) {
      (gdouble) *(pitch_buffer),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++)
    };

    pitch_buffer++;

    v_buffer_b = (ags_v8double) {
      (gdouble) *(carrier_source),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride)
    };

    carrier_source += carrier_source_stride;

    v_buffer_b /= (gdouble) (0x7fffff);
    v_buffer = gain * (((1.0 - mix) * v_buffer_a) * ((mix) * (drive * v_buffer_b)));

    *(destination) = (gint32) v_buffer[0];
    *(destination += destination_stride) = (gint32) v_buffer[1];
    *(destination += destination_stride) = (gint32) v_buffer[2];
    *(destination += destination_stride) = (gint32) v_buffer[3];
    *(destination += destination_stride) = (gint32) v_buffer[4];
    *(destination += destination_stride) = (gint32) v_buffer[5];
    *(destination += destination_stride) = (gint32) v_buffer[6];
    *(destination += destination_stride) = (gint32) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = gain * (((1.0 - mix) * pitch_buffer[0]) * ((mix) * (drive * carrier_source[0])));

    destination += destination_stride;
    pitch_buffer++;
    carrier_source += carrier_source_stride;
    i++;
  }
}

void
ags_ring_modulation_util_compute_s32(AgsRingModulationUtil *ring_modulation_util)
{
  gpointer pitch_util;
  
  gint32 *destination;
  gint32 *modulator_source, *carrier_source;
  gint32 *pitch_buffer;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  ags_v8double v_buffer, v_buffer_a, v_buffer_b;
#endif
  
  GType pitch_type;
  
  guint modulator_source_stride, carrier_source_stride, destination_stride;

  gint root_note;
  guint buffer_length;
  guint samplerate;
  gdouble pitch_tuning;
  gdouble drive;
  gdouble mix;
  gdouble gain;
  
  guint i, i_stop;

  if(ring_modulation_util == NULL ||
     ring_modulation_util->destination == NULL ||
     ring_modulation_util->source == NULL){
    return;
  }
  
  pitch_util = ring_modulation_util->pitch_util;
  pitch_type = ring_modulation_util->pitch_type;
  
  pitch_buffer = ring_modulation_util->pitch_buffer;

  destination = (gint32 *) ring_modulation_util->destination;
  modulator_source = (gint32 *) ring_modulation_util->modulator_source;
  carrier_source = (gint32 *) ring_modulation_util->carrier_source;
  
  destination_stride = ring_modulation_util->destination_stride;
  modulator_source_stride = ring_modulation_util->modulator_source_stride;
  carrier_source_stride = ring_modulation_util->carrier_source_stride;

  buffer_length = ring_modulation_util->buffer_length;
  samplerate = ring_modulation_util->samplerate;
  
  pitch_tuning = ring_modulation_util->pitch_tuning;

  drive = ring_modulation_util->drive;

  mix = ring_modulation_util->mix;

  gain = ring_modulation_util->gain;
  
  /* pitch */
  if(pitch_tuning == 0.0){
    copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(ring_modulation_util->audio_buffer_util,
								ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
													    ring_modulation_util->format),
								ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
													    ring_modulation_util->format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(ring_modulation_util->audio_buffer_util,
						pitch_buffer, 1, 0,
						modulator_source, 1, 0,
						buffer_length, copy_mode);
  }else{
    ags_common_pitch_util_set_source(pitch_util,
				     pitch_type,
				     modulator_source);

    ags_common_pitch_util_set_destination(pitch_util,
					  pitch_type,
					  pitch_buffer);

    root_note = 60;

    ags_common_pitch_util_set_base_key(pitch_util,
				       pitch_type,
				       (gdouble) root_note - 69.0 + 48.0);
  
    ags_common_pitch_util_set_tuning(pitch_util,
				     pitch_type,
				     pitch_tuning);

    ags_common_pitch_util_set_format(pitch_util,
				     pitch_type,
				     ring_modulation_util->format);

    ags_audio_buffer_util_clear_buffer(ring_modulation_util->audio_buffer_util,
				       pitch_buffer, 1,
				       buffer_length, ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
												  ring_modulation_util->format));
 
    ags_common_pitch_util_pitch(pitch_util,
				pitch_type);
    
    /* reset */
    ags_common_pitch_util_set_source(pitch_util,
				     pitch_type,
				     NULL);
    
    ags_common_pitch_util_set_destination(pitch_util,
					  pitch_type,
					  NULL);
  }
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = ring_modulation_util->buffer_length - (ring_modulation_util->buffer_length % 8);

  for(; i < i_stop;){
    v_buffer_a = (ags_v8double) {
      (gdouble) *(pitch_buffer),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++)
    };

    pitch_buffer++;

    v_buffer_b = (ags_v8double) {
      (gdouble) *(carrier_source),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride)
    };

    carrier_source += carrier_source_stride;

    v_buffer_b /= (gdouble) G_MAXINT32;
    v_buffer = gain * (((1.0 - mix) * v_buffer_a) * ((mix) * (drive * v_buffer_b)));

    *(destination) = (gint32) v_buffer[0];
    *(destination += destination_stride) = (gint32) v_buffer[1];
    *(destination += destination_stride) = (gint32) v_buffer[2];
    *(destination += destination_stride) = (gint32) v_buffer[3];
    *(destination += destination_stride) = (gint32) v_buffer[4];
    *(destination += destination_stride) = (gint32) v_buffer[5];
    *(destination += destination_stride) = (gint32) v_buffer[6];
    *(destination += destination_stride) = (gint32) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = gain * (((1.0 - mix) * pitch_buffer[0]) * ((mix) * (drive * carrier_source[0])));

    destination += destination_stride;
    pitch_buffer++;
    carrier_source += carrier_source_stride;
    i++;
  }
}

void
ags_ring_modulation_util_compute_s64(AgsRingModulationUtil *ring_modulation_util)
{
  gpointer pitch_util;
  
  gint64 *destination;
  gint64 *modulator_source, *carrier_source;
  gint64 *pitch_buffer;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  ags_v8double v_buffer, v_buffer_a, v_buffer_b;
#endif
  
  GType pitch_type;
  
  guint modulator_source_stride, carrier_source_stride, destination_stride;

  gint root_note;
  guint buffer_length;
  guint samplerate;
  gdouble pitch_tuning;
  gdouble drive;
  gdouble mix;
  gdouble gain;
  
  guint i, i_stop;

  if(ring_modulation_util == NULL ||
     ring_modulation_util->destination == NULL ||
     ring_modulation_util->source == NULL){
    return;
  }
  
  pitch_util = ring_modulation_util->pitch_util;
  pitch_type = ring_modulation_util->pitch_type;
  
  pitch_buffer = ring_modulation_util->pitch_buffer;

  destination = (gint64 *) ring_modulation_util->destination;
  modulator_source = (gint64 *) ring_modulation_util->modulator_source;
  carrier_source = (gint64 *) ring_modulation_util->carrier_source;
  
  destination_stride = ring_modulation_util->destination_stride;
  modulator_source_stride = ring_modulation_util->modulator_source_stride;
  carrier_source_stride = ring_modulation_util->carrier_source_stride;

  buffer_length = ring_modulation_util->buffer_length;
  samplerate = ring_modulation_util->samplerate;
  
  pitch_tuning = ring_modulation_util->pitch_tuning;

  drive = ring_modulation_util->drive;

  mix = ring_modulation_util->mix;

  gain = ring_modulation_util->gain;
  
  /* pitch */
  if(pitch_tuning == 0.0){
    copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(ring_modulation_util->audio_buffer_util,
								ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
													    ring_modulation_util->format),
								ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
													    ring_modulation_util->format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(ring_modulation_util->audio_buffer_util,
						pitch_buffer, 1, 0,
						modulator_source, 1, 0,
						buffer_length, copy_mode);
  }else{
    ags_common_pitch_util_set_source(pitch_util,
				     pitch_type,
				     modulator_source);

    ags_common_pitch_util_set_destination(pitch_util,
					  pitch_type,
					  pitch_buffer);

    root_note = 60;

    ags_common_pitch_util_set_base_key(pitch_util,
				       pitch_type,
				       (gdouble) root_note - 69.0 + 48.0);
  
    ags_common_pitch_util_set_tuning(pitch_util,
				     pitch_type,
				     pitch_tuning);

    ags_common_pitch_util_set_format(pitch_util,
				     pitch_type,
				     ring_modulation_util->format);

    ags_audio_buffer_util_clear_buffer(ring_modulation_util->audio_buffer_util,
				       pitch_buffer, 1,
				       buffer_length, ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
												  ring_modulation_util->format));
 
    ags_common_pitch_util_pitch(pitch_util,
				pitch_type);
    
    /* reset */
    ags_common_pitch_util_set_source(pitch_util,
				     pitch_type,
				     NULL);
    
    ags_common_pitch_util_set_destination(pitch_util,
					  pitch_type,
					  NULL);
  }
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = ring_modulation_util->buffer_length - (ring_modulation_util->buffer_length % 8);

  for(; i < i_stop;){
    v_buffer_a = (ags_v8double) {
      (gdouble) *(pitch_buffer),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++)
    };

    pitch_buffer++;

    v_buffer_b = (ags_v8double) {
      (gdouble) *(carrier_source),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride)
    };

    carrier_source += carrier_source_stride;
    
    v_buffer_b /= (gdouble) G_MAXINT64;
    v_buffer = gain * (((1.0 - mix) * v_buffer_a) * ((mix) * (drive * v_buffer_b)));

    *(destination) = (gint64) v_buffer[0];
    *(destination += destination_stride) = (gint64) v_buffer[1];
    *(destination += destination_stride) = (gint64) v_buffer[2];
    *(destination += destination_stride) = (gint64) v_buffer[3];
    *(destination += destination_stride) = (gint64) v_buffer[4];
    *(destination += destination_stride) = (gint64) v_buffer[5];
    *(destination += destination_stride) = (gint64) v_buffer[6];
    *(destination += destination_stride) = (gint64) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = gain * (((1.0 - mix) * pitch_buffer[0]) * ((mix) * (drive * carrier_source[0])));

    destination += destination_stride;
    pitch_buffer++;
    carrier_source += carrier_source_stride;
    i++;
  }
}

void
ags_ring_modulation_util_compute_float(AgsRingModulationUtil *ring_modulation_util)
{
  gpointer pitch_util;
  
  gfloat *destination;
  gfloat *modulator_source, *carrier_source;
  gfloat *pitch_buffer;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  ags_v8double v_buffer, v_buffer_a, v_buffer_b;
#endif
  
  GType pitch_type;
  
  guint modulator_source_stride, carrier_source_stride, destination_stride;

  gint root_note;
  guint buffer_length;
  guint samplerate;
  gdouble pitch_tuning;
  gdouble drive;
  gdouble mix;
  gdouble gain;
  
  guint i, i_stop;

  if(ring_modulation_util == NULL ||
     ring_modulation_util->destination == NULL ||
     ring_modulation_util->source == NULL){
    return;
  }
  
  pitch_util = ring_modulation_util->pitch_util;
  pitch_type = ring_modulation_util->pitch_type;
  
  pitch_buffer = ring_modulation_util->pitch_buffer;

  destination = (gfloat *) ring_modulation_util->destination;
  modulator_source = (gfloat *) ring_modulation_util->modulator_source;
  carrier_source = (gfloat *) ring_modulation_util->carrier_source;
  
  destination_stride = ring_modulation_util->destination_stride;
  modulator_source_stride = ring_modulation_util->modulator_source_stride;
  carrier_source_stride = ring_modulation_util->carrier_source_stride;

  buffer_length = ring_modulation_util->buffer_length;
  samplerate = ring_modulation_util->samplerate;
  
  pitch_tuning = ring_modulation_util->pitch_tuning;

  drive = ring_modulation_util->drive;

  mix = ring_modulation_util->mix;

  gain = ring_modulation_util->gain;
  
  /* pitch */
  if(pitch_tuning == 0.0){
    copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(ring_modulation_util->audio_buffer_util,
								ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
													    ring_modulation_util->format),
								ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
													    ring_modulation_util->format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(ring_modulation_util->audio_buffer_util,
						pitch_buffer, 1, 0,
						modulator_source, 1, 0,
						buffer_length, copy_mode);
  }else{
    ags_common_pitch_util_set_source(pitch_util,
				     pitch_type,
				     modulator_source);

    ags_common_pitch_util_set_destination(pitch_util,
					  pitch_type,
					  pitch_buffer);

    root_note = 60;

    ags_common_pitch_util_set_base_key(pitch_util,
				       pitch_type,
				       (gdouble) root_note - 69.0 + 48.0);
  
    ags_common_pitch_util_set_tuning(pitch_util,
				     pitch_type,
				     pitch_tuning);

    ags_common_pitch_util_set_format(pitch_util,
				     pitch_type,
				     ring_modulation_util->format);

    ags_audio_buffer_util_clear_buffer(ring_modulation_util->audio_buffer_util,
				       pitch_buffer, 1,
				       buffer_length, ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
												  ring_modulation_util->format));
 
    ags_common_pitch_util_pitch(pitch_util,
				pitch_type);
    
    /* reset */
    ags_common_pitch_util_set_source(pitch_util,
				     pitch_type,
				     NULL);
    
    ags_common_pitch_util_set_destination(pitch_util,
					  pitch_type,
					  NULL);
  }
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = ring_modulation_util->buffer_length - (ring_modulation_util->buffer_length % 8);

  for(; i < i_stop;){
    v_buffer_a = (ags_v8double) {
      (gdouble) *(pitch_buffer),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++),
      (gdouble) *(pitch_buffer++)
    };

    pitch_buffer++;

    v_buffer_b = (ags_v8double) {
      (gdouble) *(carrier_source),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride),
      (gdouble) *(carrier_source += carrier_source_stride)
    };

    carrier_source += carrier_source_stride;
    
    v_buffer = gain * (((1.0 - mix) * v_buffer_a) * ((mix) * (drive * v_buffer_b)));

    *(destination) = (gfloat) v_buffer[0];
    *(destination += destination_stride) = (gfloat) v_buffer[1];
    *(destination += destination_stride) = (gfloat) v_buffer[2];
    *(destination += destination_stride) = (gfloat) v_buffer[3];
    *(destination += destination_stride) = (gfloat) v_buffer[4];
    *(destination += destination_stride) = (gfloat) v_buffer[5];
    *(destination += destination_stride) = (gfloat) v_buffer[6];
    *(destination += destination_stride) = (gfloat) v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = gain * (((1.0 - mix) * pitch_buffer[0]) * ((mix) * (drive * carrier_source[0])));

    destination += destination_stride;
    pitch_buffer++;
    carrier_source += carrier_source_stride;
    i++;
  }
}

void
ags_ring_modulation_util_compute_double(AgsRingModulationUtil *ring_modulation_util)
{
  gpointer pitch_util;
  
  gdouble *destination;
  gdouble *modulator_source, *carrier_source;
  gdouble *pitch_buffer;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  ags_v8double v_buffer, v_buffer_a, v_buffer_b;
#endif
  
  GType pitch_type;
  
  guint modulator_source_stride, carrier_source_stride, destination_stride;

  gint root_note;
  guint buffer_length;
  guint samplerate;
  gdouble pitch_tuning;
  gdouble drive;
  gdouble mix;
  gdouble gain;
  
  guint i, i_stop;

  if(ring_modulation_util == NULL ||
     ring_modulation_util->destination == NULL ||
     ring_modulation_util->source == NULL){
    return;
  }
  
  pitch_util = ring_modulation_util->pitch_util;
  pitch_type = ring_modulation_util->pitch_type;
  
  pitch_buffer = ring_modulation_util->pitch_buffer;

  destination = (gdouble *) ring_modulation_util->destination;
  modulator_source = (gdouble *) ring_modulation_util->modulator_source;
  carrier_source = (gdouble *) ring_modulation_util->carrier_source;
  
  destination_stride = ring_modulation_util->destination_stride;
  modulator_source_stride = ring_modulation_util->modulator_source_stride;
  carrier_source_stride = ring_modulation_util->carrier_source_stride;

  buffer_length = ring_modulation_util->buffer_length;
  samplerate = ring_modulation_util->samplerate;
  
  pitch_tuning = ring_modulation_util->pitch_tuning;

  drive = ring_modulation_util->drive;

  mix = ring_modulation_util->mix;

  gain = ring_modulation_util->gain;
  
  /* pitch */
  if(pitch_tuning == 0.0){
    copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(ring_modulation_util->audio_buffer_util,
								ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
													    ring_modulation_util->format),
								ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
													    ring_modulation_util->format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(ring_modulation_util->audio_buffer_util,
						pitch_buffer, 1, 0,
						modulator_source, 1, 0,
						buffer_length, copy_mode);
  }else{
    ags_common_pitch_util_set_source(pitch_util,
				     pitch_type,
				     modulator_source);

    ags_common_pitch_util_set_destination(pitch_util,
					  pitch_type,
					  pitch_buffer);

    root_note = 60;

    ags_common_pitch_util_set_base_key(pitch_util,
				       pitch_type,
				       (gdouble) root_note - 69.0 + 48.0);
  
    ags_common_pitch_util_set_tuning(pitch_util,
				     pitch_type,
				     pitch_tuning);

    ags_common_pitch_util_set_format(pitch_util,
				     pitch_type,
				     ring_modulation_util->format);

    ags_audio_buffer_util_clear_buffer(ring_modulation_util->audio_buffer_util,
				       pitch_buffer, 1,
				       buffer_length, ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
												  ring_modulation_util->format));
 
    ags_common_pitch_util_pitch(pitch_util,
				pitch_type);
    
    /* reset */
    ags_common_pitch_util_set_source(pitch_util,
				     pitch_type,
				     NULL);
    
    ags_common_pitch_util_set_destination(pitch_util,
					  pitch_type,
					  NULL);
  }
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = ring_modulation_util->buffer_length - (ring_modulation_util->buffer_length % 8);

  for(; i < i_stop;){
    v_buffer_a = (ags_v8double) {
      *(pitch_buffer),
      *(pitch_buffer++),
      *(pitch_buffer++),
      *(pitch_buffer++),
      *(pitch_buffer++),
      *(pitch_buffer++),
      *(pitch_buffer++),
      *(pitch_buffer++)
    };

    pitch_buffer++;

    v_buffer_b = (ags_v8double) {
      *(carrier_source),
      *(carrier_source += carrier_source_stride),
      *(carrier_source += carrier_source_stride),
      *(carrier_source += carrier_source_stride),
      *(carrier_source += carrier_source_stride),
      *(carrier_source += carrier_source_stride),
      *(carrier_source += carrier_source_stride),
      *(carrier_source += carrier_source_stride)
    };

    carrier_source += carrier_source_stride;
    
    v_buffer = gain * (((1.0 - mix) * v_buffer_a) * ((mix) * (drive * v_buffer_b)));

    *(destination) = v_buffer[0];
    *(destination += destination_stride) = v_buffer[1];
    *(destination += destination_stride) = v_buffer[2];
    *(destination += destination_stride) = v_buffer[3];
    *(destination += destination_stride) = v_buffer[4];
    *(destination += destination_stride) = v_buffer[5];
    *(destination += destination_stride) = v_buffer[6];
    *(destination += destination_stride) = v_buffer[7];

    destination += destination_stride;
    i += 8;
  }
#endif

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    destination[0] = gain * (((1.0 - mix) * pitch_buffer[0]) * ((mix) * (drive * carrier_source[0])));

    destination += destination_stride;
    pitch_buffer++;
    carrier_source += carrier_source_stride;
    i++;
  }
}

void
ags_ring_modulation_util_compute_complex(AgsRingModulationUtil *ring_modulation_util)
{
  gpointer pitch_util;
  
  AgsComplex *destination;
  AgsComplex *modulator_source, *carrier_source;
  AgsComplex *pitch_buffer;

#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  ags_v8double v_buffer, v_buffer_a, v_buffer_b;
#endif
  
  GType pitch_type;
  
  guint modulator_source_stride, carrier_source_stride, destination_stride;

  gint root_note;
  guint buffer_length;
  guint samplerate;
  gdouble pitch_tuning;
  gdouble drive;
  gdouble mix;
  gdouble gain;
  
  guint i, i_stop;

  if(ring_modulation_util == NULL ||
     ring_modulation_util->destination == NULL ||
     ring_modulation_util->source == NULL){
    return;
  }
  
  pitch_util = ring_modulation_util->pitch_util;
  pitch_type = ring_modulation_util->pitch_type;
  
  pitch_buffer = (AgsComplex *) ring_modulation_util->pitch_buffer;

  destination = (AgsComplex *) ring_modulation_util->destination;
  modulator_source = (AgsComplex *) ring_modulation_util->modulator_source;
  carrier_source = (AgsComplex *) ring_modulation_util->carrier_source;
  
  destination_stride = ring_modulation_util->destination_stride;
  modulator_source_stride = ring_modulation_util->modulator_source_stride;
  carrier_source_stride = ring_modulation_util->carrier_source_stride;

  buffer_length = ring_modulation_util->buffer_length;
  samplerate = ring_modulation_util->samplerate;
  
  pitch_tuning = ring_modulation_util->pitch_tuning;

  drive = ring_modulation_util->drive;

  mix = ring_modulation_util->mix;

  gain = ring_modulation_util->gain;
  
  /* pitch */
  if(pitch_tuning == 0.0){
    copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(ring_modulation_util->audio_buffer_util,
								ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
													    ring_modulation_util->format),
								ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
													    ring_modulation_util->format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(ring_modulation_util->audio_buffer_util,
						pitch_buffer, 1, 0,
						modulator_source, 1, 0,
						buffer_length, copy_mode);
  }else{
    ags_common_pitch_util_set_source(pitch_util,
				     pitch_type,
				     modulator_source);

    ags_common_pitch_util_set_destination(pitch_util,
					  pitch_type,
					  pitch_buffer);

    root_note = 60;

    ags_common_pitch_util_set_base_key(pitch_util,
				       pitch_type,
				       (gdouble) root_note - 69.0 + 48.0);
  
    ags_common_pitch_util_set_tuning(pitch_util,
				     pitch_type,
				     pitch_tuning);

    ags_common_pitch_util_set_format(pitch_util,
				     pitch_type,
				     ring_modulation_util->format);

    ags_audio_buffer_util_clear_buffer(ring_modulation_util->audio_buffer_util,
				       pitch_buffer, 1,
				       buffer_length, ags_audio_buffer_util_format_from_soundcard(ring_modulation_util->audio_buffer_util,
												  ring_modulation_util->format));
 
    ags_common_pitch_util_pitch(pitch_util,
				pitch_type);
    
    /* reset */
    ags_common_pitch_util_set_source(pitch_util,
				     pitch_type,
				     NULL);
    
    ags_common_pitch_util_set_destination(pitch_util,
					  pitch_type,
					  NULL);
  }
  
  i = 0;

  /* loop tail */
  for(; i < volume_util->buffer_length;){
    ags_complex_set(destination,
		    gain * (((1.0 - mix) * ags_complex_get(pitch_buffer)) * ((mix) * (drive * ags_complex_get(carrier_source)))));

    destination += destination_stride;
    pitch_buffer++;
    carrier_source += carrier_source_stride;
    i++;
  }
}

void
ags_ring_modulation_util_compute(AgsRingModulationUtil *ring_modulation_util)
{
  if(ring_modulation_util->format == AGS_SOUNDCARD_SIGNED_8_BIT){
    ags_ring_modulation_util_compute_s8(ring_modulation_util);
  }else if(ring_modulation_util->format == AGS_SOUNDCARD_SIGNED_16_BIT){
    ags_ring_modulation_util_compute_s16(ring_modulation_util);
  }else if(ring_modulation_util->format == AGS_SOUNDCARD_SIGNED_24_BIT){
    ags_ring_modulation_util_compute_s24(ring_modulation_util);
  }else if(ring_modulation_util->format == AGS_SOUNDCARD_SIGNED_32_BIT){
    ags_ring_modulation_util_compute_s32(ring_modulation_util);
  }else if(ring_modulation_util->format == AGS_SOUNDCARD_SIGNED_64_BIT){
    ags_ring_modulation_util_compute_s64(ring_modulation_util);
  }else if(ring_modulation_util->format == AGS_SOUNDCARD_FLOAT){
    ags_ring_modulation_util_compute_float(ring_modulation_util);
  }else if(ring_modulation_util->format == AGS_SOUNDCARD_DOUBLE){
    ags_ring_modulation_util_compute_double(ring_modulation_util);
  }else if(ring_modulation_util->format == AGS_SOUNDCARD_COMPLEX){
    ags_ring_modulation_util_compute_complex(ring_modulation_util);
  }
}
