/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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
    return((AgsVstAudioEffect *) new Steinberg::Vst::AudioEffect());
  }

  void ags_vst_audio_effect_delete(AgsVstAudioEffect *audio_effect)
  {
    delete audio_effect;
  }

  AgsVstAudioBus* ags_vst_audio_effect_add_audio_input(AgsVstAudioEffect *audio_effect,
						       AgsVstTChar *name, AgsVstSpeakerArrangement arr, AgsVstBusType bus_type,
						       gint32 flags)
  {
    return((AgsVstAudioBus*) static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->addAudioInput(static_cast<Steinberg::Vst::TChar*>((void *) name), arr, static_cast<Steinberg::Vst::BusType>(bus_type),
													    flags));
  }
  
  AgsVstAudioBus* ags_vst_audio_effect_add_audio_output(AgsVstAudioEffect *audio_effect,
							AgsVstTChar *name, AgsVstSpeakerArrangement arr, AgsVstBusType bus_type,
							gint32 flags)
  {
    return((AgsVstAudioBus*) static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->addAudioOutput(static_cast<Steinberg::Vst::TChar*>((void *) name), arr, static_cast<Steinberg::Vst::BusType>(bus_type),
													     flags));
  }

  AgsVstAudioBus* ags_vst_audio_effect_get_audio_input(AgsVstAudioEffect *audio_effect,
						       gint32 index)
  {
    return((AgsVstAudioBus*) static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->getAudioInput(index));
  }
  
  AgsVstAudioBus* ags_vst_audio_effect_get_audio_output(AgsVstAudioEffect *audio_effect,
							gint32 index)
  {
    return((AgsVstAudioBus*) static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->getAudioOutput(index));
  }

  AgsVstEventBus* ags_vst_audio_effect_add_event_input(AgsVstAudioEffect *audio_effect,
						       AgsVstTChar *name, gint32 channels, AgsVstBusType bus_type,
						       gint32 flags)
  {
    return((AgsVstEventBus*) static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->addEventInput(static_cast<Steinberg::Vst::TChar*>((void *) name), channels, static_cast<Steinberg::Vst::BusType>(bus_type),
													    flags));
  }

  AgsVstEventBus* ags_vst_audio_effect_add_event_output(AgsVstAudioEffect *audio_effect,
							AgsVstTChar *name, gint32 channels, AgsVstBusType bus_type,
							gint32 flags)
  {
    return((AgsVstEventBus*) static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->addEventOutput(static_cast<Steinberg::Vst::TChar*>((void *) name), channels, static_cast<Steinberg::Vst::BusType>(bus_type),
													     flags));
  }

  AgsVstEventBus* ags_vst_audio_effect_get_event_input(AgsVstAudioEffect *audio_effect,
						       gint32 index)
  {
    return((AgsVstEventBus*) static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->getEventInput(index));
  }

  AgsVstEventBus* ags_vst_audio_effect_get_event_output(AgsVstAudioEffect *audio_effect,
							gint32 index)
  {
    return((AgsVstEventBus*) static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->getEventOutput(index));
  }

  gint32 ags_vst_audio_effect_set_bus_arrangements(AgsVstAudioEffect *audio_effect,
						   AgsVstSpeakerArrangement *inputs, gint32 num_ins,
						   AgsVstSpeakerArrangement *outputs, gint32 num_outs)
  {
    return(static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->setBusArrangements(inputs, num_ins,
											       outputs, num_outs));
  }
  
  gint32 ags_vst_audio_effect_get_bus_arrangement(AgsVstAudioEffect *audio_effect,
						  AgsVstBusDirection bus_direction, gint32 bus_index,
						  AgsVstSpeakerArrangement *arr)
  {
    Steinberg::Vst::SpeakerArrangement tmp_arr_0 = static_cast<Steinberg::Vst::SpeakerArrangement>(arr[0]);
    const Steinberg::Vst::SpeakerArrangement& tmp_arr_1 = const_cast<Steinberg::Vst::SpeakerArrangement&>(tmp_arr_0);
    
    return(static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->getBusArrangement(static_cast<Steinberg::Vst::BusDirection>(bus_direction), bus_index,
											      const_cast<Steinberg::Vst::SpeakerArrangement&>(tmp_arr_1)));
  }
  
  gint32 ags_vst_audio_effect_can_process_sample_size(AgsVstAudioEffect *audio_effect,
						      gint32 symbolic_sample_size)
  {
    return(static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->canProcessSampleSize(symbolic_sample_size));
  }
  
  guint32 ags_vst_audio_effect_get_latency_samples(AgsVstAudioEffect *audio_effect)
  {
    return(static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->getLatencySamples());
  }
  
  gint32 ags_vst_audio_effect_setup_processing(AgsVstAudioEffect *audio_effect,
					       AgsVstProcessSetup *setup)
  {
    Steinberg::Vst::ProcessSetup *tmp_setup_0 = static_cast<Steinberg::Vst::ProcessSetup*>((void *) setup);
    const Steinberg::Vst::ProcessSetup& tmp_setup_1 = const_cast<Steinberg::Vst::ProcessSetup&>(tmp_setup_0[0]);
    
    return(static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->setupProcessing(const_cast<Steinberg::Vst::ProcessSetup&>(tmp_setup_1)));
  }
  
  gint32 ags_vst_audio_effect_set_processing(AgsVstAudioEffect *audio_effect,
					     gboolean state)
  {
    return(static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->setProcessing(static_cast<Steinberg::TBool>(state)));
  }
  
  gint32 ags_vst_audio_effect_process(AgsVstAudioEffect *audio_effect,
				      AgsVstProcessData *data)
  {
    Steinberg::Vst::ProcessData *tmp_data_0 = static_cast<Steinberg::Vst::ProcessData*>((void *) data);
    const Steinberg::Vst::ProcessData& tmp_data_1 = const_cast<Steinberg::Vst::ProcessData&>(tmp_data_0[0]);    
    
    return(static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->process(const_cast<Steinberg::Vst::ProcessData&>(tmp_data_1)));
  }
  
  guint32 ags_vst_audio_effect_get_tail_samples(AgsVstAudioEffect *audio_effect)
  {
    return(static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->getTailSamples());
  }

}
