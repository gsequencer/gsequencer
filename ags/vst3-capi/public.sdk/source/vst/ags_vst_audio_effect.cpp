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

#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_audio_effect.h>

#include <public.sdk/source/vst/vstaudioeffect.h>

extern "C" {

  AgsVstAudioEffect* ags_vst_audio_effect_new()
  {
    return(new AudioEffect());
  }

  void ags_vst_audio_effect_delete(AgsVstAudioEffect *audio_effect)
  {
    delete audio_effect;
  }

  AgsVstAudioBus* ags_vst_audio_add_audio_input(AgsVstAudioEffect *audio_effect,
						gchar *name, AgsVstSpeakerArrangement *arr, guint bus_type,
						gint32 flags)
  {
    return(audio_effect->addAudioInput(name, arr, static_cast<BusType>(bus_type),
				       flags));
  }
  
  AgsVstAudioBus* ags_vst_audio_add_audio_output(AgsVstAudioEffect *audio_effect,
						 gchar *name, AgsVstSpeakerArrangement *arr, guint bus_type,
						 gint32 flags)
  {
    return(audio_effect->addAudioOutput(name, arr, static_cast<BusType>(bus_type),
					flags));
  }

  AgsVstAudioBus* ags_vst_audio_get_audio_input(AgsVstAudioEffect *audio_effect,
						gint32 index)
  {
    return(audio_effect->getAudioInput(index));
  }
  
  AgsVstAudioBus* ags_vst_audio_get_audio_output(AgsVstAudioEffect *audio_effect,
						 gint32 index)
  {
    return(audio_effect->getAudioOutput(index));
  }

  AgsVstEventBus* ags_vst_audio_add_event_input(AgsVstAudioEffect *audio_effect,
						gchar *name, gint32 channels, guint bus_type,
						gint32 flags)
  {
    return(audio_effect->addEventInput(name, channels, static_cast<BusType>(bus_type),
				       flags));
  }

  AgsVstEventBus* ags_vst_audio_add_event_output(AgsVstAudioEffect *audio_effect,
						 gchar *name, gint32 channels, guint bus_type,
						 gint32 flags)
  {
    return(audio_effect->addEventOutput(name, channels, static_cast<BusType>(bus_type),
					flags));
  }

  AgsVstEventBus* ags_vst_audio_get_event_input(AgsVstAudioEffect *audio_effect,
						gint32 index)
  {
    return(audio_effect->getEventInput(index));
  }

  AgsVstEventBus* ags_vst_audio_get_event_output(AgsVstAudioEffect *audio_effect,
						 gint32 index)
  {
    return(audio_effect->getEventOutput(index));
  }

  tresult PLUGIN_API ags_vst_audio_set_bus_arrangements(AgsVstAudioEffect *audio_effect,
							AgsVstSpeakerArrangement *inputs, gint32 num_ins,
							AgsVstSpeakerArrangement *outputs, gint32 num_outs)
  {
  }
  
  tresult PLUGIN_API ags_vst_audio_get_bus_arrangements(AgsVstAudioEffect *audio_effect,
							guint bus_direction, gint32 bus_index,
							AgsVstSpeakerArrangement **arr)
  {
    return(audio_effect->setBusArrangements(static_cast<BusDirection>(bus_direction), bus_index,
					    *(arr[0])));
  }
  
  tresult PLUGIN_API ags_vst_audio_can_process_sample_size(AgsVstAudioEffect *audio_effect,
							   gint32 symbolic_sample_size)
  {
    return(audio_effect->canProcessSampleSize(symbolic_sample_size));
  }
  
  guint32 PLUGIN_API ags_vst_audio_get_latency_samples(AgsVstAudioEffect *audio_effect)
  {
    return(audio_effect->getLatencySamples());
  }
  
  tresult PLUGIN_API ags_vst_audio_setup_processing(AgsVstAudioEffect *audio_effect,
						    AgsVstProcessSetup **setup)
  {
    return(audio_effect->setupProcessing(*(setup[0])));
  }
  
  tresult PLUGIN_API ags_vst_audio_set_processing(AgsVstAudioEffect *audio_effect,
						  gboolean state)
  {
    return(audio_effect->setProcessing(static_cast<TBool>(state)));
  }
  
  tresult PLUGIN_API ags_vst_audio_process(AgsVstProcessData **data)
  {
    return(audio_effect->process(*(data[0])));
  }
  
  guint32 PLUGIN_API ags_vst_audio_get_tail_samples(AgsVstAudioEffect *audio_effect)
  {
    return(audio_effect->getTailSamples());
  }

}
