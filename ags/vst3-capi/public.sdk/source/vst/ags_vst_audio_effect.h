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

#include <ags/vst/ags_vst_types.h>
#include <ags/vst/ags_vst_bus.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstAudioEffect AgsVstAudioEffect;
  
  AgsVstAudioEffect* ags_vst_audio_effect_new();
  void ags_vst_audio_effect_finalize(AgsVstAudioEffect *audio_effect);

  void ags_vst_audio_effect_ref(AgsVstAudioEffect *audio_effect);
  void ags_vst_audio_effect_unref(AgsVstAudioEffect *audio_effect);

  AgsVstAudioBus* ags_vst_audio_add_audio_input(AgsVstAudioEffect *audio_effect,
						char *name, AgsVstSpeakerArrangement *arr, uint32 bus_type,
						int32 flags);
  AgsVstAudioBus* ags_vst_audio_add_audio_output(AgsVstAudioEffect *audio_effect,
						 char *name, AgsVstSpeakerArrangement *arr, uint32 bus_type,
						 int32 flags);

  AgsVstAudioBus* ags_vst_audio_get_audio_input(AgsVstAudioEffect *audio_effect,
						int32 index);
  AgsVstAudioBus* ags_vst_audio_get_audio_output(AgsVstAudioEffect *audio_effect,
						 int32 index);

  AgsVstEventBus* ags_vst_audio_add_event_input(AgsVstAudioEffect *audio_effect,
						char *name, AgsVstSpeakerArrangement *arr, uint32 bus_type,
						int32 flags);
  AgsVstEventBus* ags_vst_audio_add_event_output(AgsVstAudioEffect *audio_effect,
						 char *name, AgsVstSpeakerArrangement *arr, uint32 bus_type,
						 int32 flags);

  AgsVstEventBus* ags_vst_audio_get_event_input(AgsVstAudioEffect *audio_effect,
						int32 index);
  AgsVstEventBus* ags_vst_audio_get_event_output(AgsVstAudioEffect *audio_effect,
						 int32 index);

  tresult ags_vst_audio_set_bus_arrangements(AgsVstAudioEffect *audio_effect,
					     AgsVstSpeakerArrangement *inputs, int32 num_ins,
					     AgsVstSpeakerArrangement *outputs, int32 num_outs);
  tresult ags_vst_audio_get_bus_arrangements(AgsVstAudioEffect *audio_effect,
					     uint32 bus_direction, int32 bus_index,
					     AgsVstSpeakerArrangement **arr);
  tresult ags_vst_audio_can_process_sample_size(AgsVstAudioEffect *audio_effect,
						int32 symbolic_sample_size);
  uint32 ags_vst_audio_get_latency_samples(AgsVstAudioEffect *audio_effect);
  tresult ags_vst_audio_setup_processing(AgsVstAudioEffect *audio_effect,
					 AgsVstProcessSetup **setup);
  tresult ags_vst_audio_set_processing(AgsVstAudioEffect *audio_effect,
				       uint32 state);
  tresult ags_vst_audio_process(AgsVstProcessData **data);
  uint32 ags_vst_audio_get_tail_samples();
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_AUDIO_EFFECT_H__*/
