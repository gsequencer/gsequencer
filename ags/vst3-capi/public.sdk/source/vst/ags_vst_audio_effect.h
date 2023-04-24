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
  
  AgsVstAudioEffect* ags_vst_audio_effect_new();

  void ags_vst_audio_effect_delete(AgsVstAudioEffect *audio_effect);

  AgsVstAudioBus* ags_vst_audio_effect_add_audio_input(AgsVstAudioEffect *audio_effect,
						       AgsVstTChar *name, AgsVstSpeakerArrangement arr, AgsVstBusType bus_type,
						       gint32 flags);

  AgsVstAudioBus* ags_vst_audio_effect_add_audio_output(AgsVstAudioEffect *audio_effect,
							AgsVstTChar *name, AgsVstSpeakerArrangement arr, AgsVstBusType bus_type,
							gint32 flags);

  AgsVstAudioBus* ags_vst_audio_effect_get_audio_input(AgsVstAudioEffect *audio_effect,
						       gint32 index);

  AgsVstAudioBus* ags_vst_audio_effect_get_audio_output(AgsVstAudioEffect *audio_effect,
							gint32 index);

  AgsVstEventBus* ags_vst_audio_effect_add_event_input(AgsVstAudioEffect *audio_effect,
						       AgsVstTChar *name, gint32 channels, AgsVstBusType bus_type,
						       gint32 flags);
  AgsVstEventBus* ags_vst_audio_effect_add_event_output(AgsVstAudioEffect *audio_effect,
							AgsVstTChar *name, gint32 channels, AgsVstBusType bus_type,
							gint32 flags);

  AgsVstEventBus* ags_vst_audio_effect_get_event_input(AgsVstAudioEffect *audio_effect,
						       gint32 index);
  AgsVstEventBus* ags_vst_audio_effect_get_event_output(AgsVstAudioEffect *audio_effect,
							gint32 index);
  
  gint32 ags_vst_audio_effect_set_bus_arrangements(AgsVstAudioEffect *audio_effect,
						   AgsVstSpeakerArrangement *inputs, gint32 num_ins,
						   AgsVstSpeakerArrangement *outputs, gint32 num_outs);

  gint32 ags_vst_audio_effect_get_bus_arrangement(AgsVstAudioEffect *audio_effect,
						  AgsVstBusDirection bus_direction, gint32 bus_index,
						  AgsVstSpeakerArrangement *arr);
  
  gint32 ags_vst_audio_effect_can_process_sample_size(AgsVstAudioEffect *audio_effect,
						      gint32 symbolic_sample_size);

  guint32 ags_vst_audio_effect_get_latency_samples(AgsVstAudioEffect *audio_effect);

  gint32 ags_vst_audio_effect_setup_processing(AgsVstAudioEffect *audio_effect,
					       AgsVstProcessSetup *setup);

  gint32 ags_vst_audio_effect_set_processing(AgsVstAudioEffect *audio_effect,
					     gboolean state);

  gint32 ags_vst_audio_effect_process(AgsVstAudioEffect *audio_effect,
				      AgsVstProcessData *data);

  guint32 ags_vst_audio_effect_get_tail_samples(AgsVstAudioEffect *audio_effect);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_AUDIO_EFFECT_H__*/
