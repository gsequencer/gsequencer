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
    //TODO:JK: implement me
  }

  AgsVStTResult ags_vst_iaudio_processor_set_bus_arrangements(AgsVstIAudioProcessor *iaudio_processor,
							      AgsVstSpeakerArrangements *inputs, gint32 num_ins,
							      AgsVstSpeakerArrangements *outputs, gint32 num_outs)
  {
    //TODO:JK: implement me
  }

  AgsVStTResult ags_vst_iaudio_processor_get_bus_arrangements(AgsVstIAudioProcessor *iaudio_processor,
							      AgsVstBusDirection dir, gint32 index,
							      AgsVstSpeakerArrangments *arr)
  {
    //TODO:JK: implement me
  }

  AgsVStTResult ags_vst_iaudio_processor_can_process_sample_size(AgsVstIAudioProcessor *iaudio_processor,
								 gint32 symbolic_sample_size)
  {
    //TODO:JK: implement me
  }

  guint32 ags_vst_iaudio_processor_get_latency_sample(AgsVstIAudioProcessor *iaudio_processor)
  {
    //TODO:JK: implement me
  }

  AgsVStTResult ags_vst_iaudio_processor_setup_processing(AgsVstIAudioProcessor *iaudio_processor,
							  AgsVstProcessSetup *setup)
  {
    //TODO:JK: implement me
  }

  AgsVStTResult ags_vst_iaudio_processor_set_processing(AgsVstIAudioProcessor *iaudio_processor,
							gboolean state)
  {
    //TODO:JK: implement me
  }

  AgsVStTResult ags_vst_iaudio_processor_process(AgsVstIAudioProcessor *iaudio_processor,
						 AgsVstProcessData *data)
  {
    //TODO:JK: implement me
  }
  
  guint32 ags_vst_iaudio_processor_get_tail_samples(AgsVstIAudioProcessor *iaudio_processor)
  {
    //TODO:JK: implement me
  }

  const AgsVstTUID* ags_vst_iaudio_presentation_latency_get_iid()
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iaudio_presentation_latency_set_audio_presentation_latency_samples(AgsVstIAudioPresentationLatency *iaudio_presentation_latency,
											   AgsVstBusDirection dir, gint32 bus_index,
											   guint32 latency_in_samples)
  {
    //TODO:JK: implement me
  }

  const AgsVstTUID* ags_vst_iprocess_context_requirements_get_iid()
  {
    //TODO:JK: implement me
  }
  
  AgsVstTResult ags_vst_iprocess_context_get_process_context_requirements(AgsVstIProcessContextRequirements *iprocess_context_requirements)
  {
    //TODO:JK: implement me
  }  

}
