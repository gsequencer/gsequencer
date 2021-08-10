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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iaudio_processor.h>

#include <pluginterfaces/vst/ivstaudioprocessor.h>

extern "C" {

  const AgsVstTUID* ags_vst_iaudio_processor_get_iid()
  {
    extern const Steinberg::TUID IAudioProcessor__iid;

    return((AgsVstTUID *) (&IAudioProcessor__iid));
  }

  AgsVstTResult ags_vst_iaudio_processor_set_bus_arrangements(AgsVstIAudioProcessor *iaudio_processor,
							      AgsVstSpeakerArrangement *inputs, gint32 num_ins,
							      AgsVstSpeakerArrangement *outputs, gint32 num_outs)
  {
    return(((Steinberg::Vst::IAudioProcessor *) iaudio_processor)->setBusArrangements(inputs, num_ins,
										      outputs, num_outs));
  }

  AgsVstTResult ags_vst_iaudio_processor_get_bus_arrangement(AgsVstIAudioProcessor *iaudio_processor,
							     AgsVstBusDirection dir, gint32 index,
							     AgsVstSpeakerArrangement *arr)
  {
    const Steinberg::Vst::SpeakerArrangement& tmp_arr_0 = const_cast<Steinberg::Vst::SpeakerArrangement&>(arr[0]);
    
    return(((Steinberg::Vst::IAudioProcessor *) iaudio_processor)->getBusArrangement(dir, index,
										     const_cast<Steinberg::Vst::SpeakerArrangement&>(tmp_arr_0)));
  }

  AgsVstTResult ags_vst_iaudio_processor_can_process_sample_size(AgsVstIAudioProcessor *iaudio_processor,
								 gint32 symbolic_sample_size)
  {
    return(((Steinberg::Vst::IAudioProcessor *) iaudio_processor)->canProcessSampleSize(symbolic_sample_size));
  }

  guint32 ags_vst_iaudio_processor_get_latency_samples(AgsVstIAudioProcessor *iaudio_processor)
  {
    return(((Steinberg::Vst::IAudioProcessor *) iaudio_processor)->getLatencySamples());
  }

  AgsVstTResult ags_vst_iaudio_processor_setup_processing(AgsVstIAudioProcessor *iaudio_processor,
							  AgsVstProcessSetup *setup)
  {
    const Steinberg::Vst::ProcessSetup& tmp_setup_0 = const_cast<Steinberg::Vst::ProcessSetup&>(((Steinberg::Vst::ProcessSetup *) setup)[0]);
  
    return(((Steinberg::Vst::IAudioProcessor *) iaudio_processor)->setupProcessing(const_cast<Steinberg::Vst::ProcessSetup&>(tmp_setup_0)));
  }

  AgsVstTResult ags_vst_iaudio_processor_set_processing(AgsVstIAudioProcessor *iaudio_processor,
							gboolean state)
  {
    return(((Steinberg::Vst::IAudioProcessor *) iaudio_processor)->setProcessing(state));
  }

  AgsVstTResult ags_vst_iaudio_processor_process(AgsVstIAudioProcessor *iaudio_processor,
						 AgsVstProcessData *data)
  {
    const Steinberg::Vst::ProcessData& tmp_data_0 = const_cast<Steinberg::Vst::ProcessData&>(((Steinberg::Vst::ProcessData *) data)[0]);

    return(((Steinberg::Vst::IAudioProcessor *) iaudio_processor)->process(const_cast<Steinberg::Vst::ProcessData&>(tmp_data_0)));
  }
  
  guint32 ags_vst_iaudio_processor_get_tail_samples(AgsVstIAudioProcessor *iaudio_processor)
  {
    return(((Steinberg::Vst::IAudioProcessor *) iaudio_processor)->getTailSamples());
  }

  const AgsVstTUID* ags_vst_iaudio_presentation_latency_get_iid()
  {
    extern const Steinberg::TUID IAudioPresentationLatency__iid;

    return((AgsVstTUID *) (&IAudioPresentationLatency__iid));
  }

  AgsVstTResult ags_vst_iaudio_presentation_latency_set_audio_presentation_latency_samples(AgsVstIAudioPresentationLatency *iaudio_presentation_latency,
											   AgsVstBusDirection dir, gint32 bus_index,
											   guint32 latency_in_samples)
  {
    return(((Steinberg::Vst::IAudioPresentationLatency *) iaudio_presentation_latency)->setAudioPresentationLatencySamples(dir, bus_index,
															   latency_in_samples));
  }

  const AgsVstTUID* ags_vst_iprocess_context_requirements_get_iid()
  {
    extern const Steinberg::TUID IProcessContextRequirements__iid;

    return((AgsVstTUID *) (&IProcessContextRequirements__iid));
  }
  
  guint32 ags_vst_iprocess_context_requirements_get_process_context_requirements(AgsVstIProcessContextRequirements *iprocess_context_requirements)
  {
    return(((Steinberg::Vst::IProcessContextRequirements *) iprocess_context_requirements)->getProcessContextRequirements());
  }  

}
