/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_VST_AUDIO_EFFECT_H__
#define __AGS_VST_AUDIO_EFFECT_H__

#include <glib.h>

#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_bus.h>
#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_component.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iaudio_processor.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstAudioEffect AudioEffect;
  
  /**
   * ags_vst_audio_effect_new:
   * 
   * Wrapper of AudioEffect constructor.
   * 
   * Returns: the #AgsVstAudioEffect-struct pointer
   * 
   * Since: 2.2.0
   */
  AgsVstAudioEffect* ags_vst_audio_effect_new();

  /**
   * ags_vst_audio_effect_delete:
   * @audio_effect: the #AgsVstAudioEffect-struct pointer
   * 
   * Delete @audio_effect.
   * 
   * Since: 2.2.0
   */
  void ags_vst_audio_effect_delete(AgsVstAudioEffect *audio_effect);

  /**
   * ags_vst_audio_effect_add_audio_input:
   * @audio_effect: the #AgsVstAudioEffect-struct pointer
   * @name: the name as string
   * @arr: the #AgsVstSpeakerArrangement
   * @bus_type: the bus type as #AgsVstBusTypes-enum
   * @flags: the flags
   * 
   * Add audio input with @name and @arr of @bus_type using @flags.
   * 
   * Returns: the #AgsVstAudioBus-struct pointer
   * 
   * Since: 2.2.0
   */
  AgsVstAudioBus* ags_vst_audio_effect_add_audio_input(AgsVstAudioEffect *audio_effect,
						       AgsVstTChar *name, AgsVstSpeakerArrangement arr, AgsVstBusType bus_type,
						       gint32 flags);

  /**
   * ags_vst_audio_effect_add_audio_output:
   * @audio_effect: the #AgsVstAudioEffect-struct pointer
   * @name: the name as string
   * @arr: the #AgsVstSpeakerArrangement
   * @bus_type: the bus type as #AgsVstBusTypes-enum
   * @flags: the flags
   * 
   * Add audio output with @name and @arr of @bus_type using @flags.
   * 
   * Returns: the #AgsVstAudioBus-struct pointer
   * 
   * Since: 2.2.0
   */
  AgsVstAudioBus* ags_vst_audio_effect_add_audio_output(AgsVstAudioEffect *audio_effect,
							AgsVstTChar *name, AgsVstSpeakerArrangement arr, AgsVstBusType bus_type,
							gint32 flags);

  /**
   * ags_vst_audio_effect_get_audio_input:
   * @audio_effect: the #AgsVstAudioEffect-struct pointer
   * @index: the index to get
   * 
   * Get audio input by @index.
   * 
   * Returns: the #AgsVstAudioBus-struct pointer
   * 
   * Since: 2.2.0
   */
  AgsVstAudioBus* ags_vst_audio_effect_get_audio_input(AgsVstAudioEffect *audio_effect,
						       gint32 index);

  /**
   * ags_vst_audio_effect_get_audio_output:
   * @audio_effect: the #AgsVstAudioEffect-struct pointer
   * @index: the index to get
   * 
   * Get audio output by @index.
   * 
   * Returns: the #AgsVstAudioBus-struct pointer
   * 
   * Since: 2.2.0
   */
  AgsVstAudioBus* ags_vst_audio_effect_get_audio_output(AgsVstAudioEffect *audio_effect,
							gint32 index);

  /**
   * ags_vst_audio_effect_add_event_input:
   * @audio_effect: the #AgsVstAudioEffect-struct pointer
   * @name: the name as string
   * @channels: channels
   * @bus_type: the bus type as #AgsVstBusTypes-enum
   * @flags: the flags
   * 
   * Add event input with @name and @channesl of @bus_type using @flags.
   * 
   * Returns: the #AgsVstEventBus-struct pointer
   * 
   * Since: 2.2.0
   */
  AgsVstEventBus* ags_vst_audio_effect_add_event_input(AgsVstAudioEffect *audio_effect,
						       AgsVstTChar *name, gint32 channels, AgsVstBusType bus_type,
						       gint32 flags);

  /**
   * ags_vst_audio_effect_add_event_output:
   * @audio_effect: the #AgsVstAudioEffect-struct pointer
   * @name: the name as string
   * @channels: channels
   * @bus_type: the bus type as #AgsVstBusTypes-enum
   * @flags: the flags
   * 
   * Add event output with @name and @channesl of @bus_type using @flags.
   * 
   * Returns: the #AgsVstEventBus-struct pointer
   * 
   * Since: 2.2.0
   */
  AgsVstEventBus* ags_vst_audio_effect_add_event_output(AgsVstAudioEffect *audio_effect,
							AgsVstTChar *name, gint32 channels, AgsVstBusType bus_type,
							gint32 flags);

  /**
   * ags_vst_audio_effect_get_event_input:
   * @audio_effect: the #AgsVstAudioEffect-struct pointer
   * @index: the index to get
   * 
   * Get event input by @index.
   * 
   * Returns: the #AgsVstEventBus-struct pointer
   * 
   * Since: 2.2.0
   */
  AgsVstEventBus* ags_vst_audio_effect_get_event_input(AgsVstAudioEffect *audio_effect,
						       gint32 index);

  /**
   * ags_vst_audio_effect_get_event_output:
   * @audio_effect: the #AgsVstAudioEffect-struct pointer
   * @index: the index to get
   * 
   * Get event output by @index.
   * 
   * Returns: the #AgsVstEventBus-struct pointer
   * 
   * Since: 2.2.0
   */
  AgsVstEventBus* ags_vst_audio_effect_get_event_output(AgsVstAudioEffect *audio_effect,
							gint32 index);
  
  /**
   * ags_vst_audio_effect_set_bus_arrangements:
   * @audio_effect: the #AgsVstAudioEffect-struct pointer
   * @inputs: the #AgsVstSpeakerArrangement array
   * @num_ins: number of inputs as 32 bit integer
   * @outputs: the #AgsVstSpeakerArrangement array
   * @num_outs: number of outputs as 32 bit integer
   * 
   * Set bus arrangements of @audio_effect.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_audio_effect_set_bus_arrangements(AgsVstAudioEffect *audio_effect,
						   AgsVstSpeakerArrangement *inputs, gint32 num_ins,
						   AgsVstSpeakerArrangement *outputs, gint32 num_outs);

  /**
   * ags_vst_audio_effect_get_bus_arrangement:
   * @audio_effect: the #AgsVstAudioEffect-struct pointer
   * @bus_direction: the #AgsVstBusDirections-enum
   * @bus_index: the bus index
   * @arr: the #AgsVstSpeakerArrangement array
   * 
   * Get bus arrangements of @audio_effect.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_audio_effect_get_bus_arrangement(AgsVstAudioEffect *audio_effect,
						  AgsVstBusDirection bus_direction, gint32 bus_index,
						  AgsVstSpeakerArrangement *arr);
  
  /**
   * ags_vst_audio_effect_can_process_sample_size:
   * @audio_effect: the #AgsVstAudioEffect-struct pointer
   * @symbolic_sample_size: symbolic sample size
   * 
   * Can @audio_effect process @symbolic_sample_size.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_audio_effect_can_process_sample_size(AgsVstAudioEffect *audio_effect,
						      gint32 symbolic_sample_size);

  /**
   * ags_vst_audio_effect_get_latency_samples:
   * @audio_effect: the #AgsVstAudioEffect-struct pointer
   * 
   * Get lantency samples of @audio_effect.
   * 
   * Returns: the latency
   * 
   * Since: 2.2.0
   */
  guint32 ags_vst_audio_effect_get_latency_samples(AgsVstAudioEffect *audio_effect);

  /**
   * ags_vst_audio_effect_setup_processing:
   * @audio_effect: the #AgsVstAudioEffect-struct pointer
   * @setup: the #AgsVstProcessSetup-struct pointer
   * 
   * Setup processing of @audio_effect.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_audio_effect_setup_processing(AgsVstAudioEffect *audio_effect,
					       AgsVstProcessSetup *setup);

  /**
   * ags_vst_audio_effect_set_processing:
   * @audio_effect: the #AgsVstAudioEffect-struct pointer
   * @state: %TRUE if processing, otherwise %FALSE
   * 
   * Set processing of @audio_effect.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_audio_effect_set_processing(AgsVstAudioEffect *audio_effect,
					     gboolean state);

  /**
   * ags_vst_audio_effect_process:
   * @audio_effect: the #AgsVstAudioEffect-struct pointer
   * @data: the #AgsVstProcessData-struct pointer
   * 
   * Process @audio_effect using @data.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_audio_effect_process(AgsVstAudioEffect *audio_effect,
				      AgsVstProcessData *data);

  /**
   * ags_vst_audio_effect_get_tail_samples:
   * @audio_effect: the #AgsVstAudioEffect-struct pointer
   * 
   * Get tail samples of @audio_effect.
   * 
   * Returns: the tail samples
   * 
   * Since: 2.2.0
   */
  guint32 ags_vst_audio_effect_get_tail_samples(AgsVstAudioEffect *audio_effect);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_AUDIO_EFFECT_H__*/
