/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

  const AgsVstCString ags_vst_kfx_analyzer               = "Fx|Analyzer";
  const AgsVstCString ags_vst_kfx_delay                  = "Fx|Delay";
  const AgsVstCString ags_vst_kfx_distortion             = "Fx|Distortion";
  const AgsVstCString ags_vst_kfx_dynamics               = "Fx|Dynamics";
  const AgsVstCString ags_vst_kfx_eq                     = "Fx|EQ";
  const AgsVstCString ags_vst_kfx_filter                 = "Fx|Filter";
  const AgsVstCString ags_vst_kfx                        = "Fx";
  const AgsVstCString ags_vst_kfx_instrument             = "Fx|Instrument";
  const AgsVstCString ags_vst_kfx_instrument_external    = "Fx|Instrument|External";
  const AgsVstCString ags_vst_kfx_spatial                = "Fx|Spatial";
  const AgsVstCString ags_vst_kfx_generator              = "Fx|Generator";
  const AgsVstCString ags_vst_kfx_mastering              = "Fx|Mastering";
  const AgsVstCString ags_vst_kfx_modulation             = "Fx|Modulation";
  const AgsVstCString ags_vst_kfx_pitch_shift            = "Fx|Pitch Shift";
  const AgsVstCString ags_vst_kfx_restoration            = "Fx|Restoration";
  const AgsVstCString ags_vst_kfx_reverb                 = "Fx|Reverb";
  const AgsVstCString ags_vst_kfx_surround               = "Fx|Surround";
  const AgsVstCString ags_vst_kfx_tools                  = "Fx|Tools";
  const AgsVstCString ags_vst_kfx_network                = "Fx|Network";
  const AgsVstCString ags_vst_kinstrument                = "Instrument";
  const AgsVstCString ags_vst_kinstrument_drum           = "Instrument|Drum";
  const AgsVstCString ags_vst_kinstrument_external       = "Instrument|External";
  const AgsVstCString ags_vst_kinstrument_piano          = "Instrument|Piano";
  const AgsVstCString ags_vst_kinstrument_sampler        = "Instrument|Sampler";
  const AgsVstCString ags_vst_kinstrument_synth          = "Instrument|Synth";
  const AgsVstCString ags_vst_kinstrument_synth_sampler  = "Instrument|Synth|Sampler";
  const AgsVstCString ags_vst_kspatial                   = "Spatial";
  const AgsVstCString ags_vst_kspatial_fx                = "Spatial|Fx";
  const AgsVstCString ags_vst_konly_real_time            = "OnlyRT";
  const AgsVstCString ags_vst_konly_offline_process      = "OnlyOfflineProcess";
  const AgsVstCString ags_vst_konly_ara                  = "OnlyARA";

  const AgsVstCString ags_vst_kno_offline_process        = "NoOfflineProcess";
  const AgsVstCString ags_vst_kup_down_mix               = "Up-Downmix";
  const AgsVstCString ags_vst_kanalyzer                  = "Analyzer";
  const AgsVstCString ags_vst_kambisonics                = "Ambisonics";

  const AgsVstCString ags_vst_kmono                      = "Mono";
  const AgsVstCString ags_vst_kstereo                    = "Stereo";
  const AgsVstCString ags_vst_ksurround                  = "Surround";

  /**
   * Alloc.
   *
   * @return the Steinberg::Vst::ProcessSetup as AgsVstProcessSetup
   * 
   * @since 5.0.0
   */
  AgsVstProcessSetup* ags_vst_process_setup_alloc()
  {
    return((AgsVstProcessSetup *) new Steinberg::Vst::ProcessSetup());
  }

  /**
   * Set process mode.
   *
   * @param setup the process setup
   * @param process_mode the process mode
   *
   * @since 5.0.0
   */
  void ags_vst_process_setup_set_process_mode(AgsVstProcessSetup *setup,
					      gint32 process_mode)
  {
    ((Steinberg::Vst::ProcessSetup *) setup)->processMode = process_mode;
  }

  /**
   * Set symbolic sample size.
   *
   * @param setup the process setup
   * @param symbolic_sample_size the symbolic sample size
   *
   * @since 5.0.0
   */
  void ags_vst_process_setup_set_symbolic_sample_size(AgsVstProcessSetup *setup,
						      gint32 symbolic_sample_size)
  {
    ((Steinberg::Vst::ProcessSetup *) setup)->symbolicSampleSize = symbolic_sample_size;
  }
  
  /**
   * Set max samples per block.
   *
   * @param setup the process setup
   * @param max_samples_per_block the max samples per block
   *
   * @since 5.0.0
   */
  void ags_vst_process_setup_set_max_samples_per_block(AgsVstProcessSetup *setup,
						       gint32 max_samples_per_block)
  {
    ((Steinberg::Vst::ProcessSetup *) setup)->maxSamplesPerBlock = max_samples_per_block;
  }

  /**
   * Set samplerate.
   *
   * @param setup the process setup
   * @param samplerate the samplerate
   *
   * @since 5.0.0
   */
  void ags_vst_process_setup_set_samplerate(AgsVstProcessSetup *setup,
					    gdouble samplerate)
  {
    ((Steinberg::Vst::ProcessSetup *) setup)->sampleRate = samplerate;
  }

  /**
   * Alloc.
   *
   * @return the Steinberg::Vst::AudioBusBuffers as AgsVstAudioBusBuffers
   * 
   * @since 5.0.0
   */
  AgsVstAudioBusBuffers* ags_vst_audio_bus_buffers_alloc()
  {
    return((AgsVstAudioBusBuffers *) new Steinberg::Vst::AudioBusBuffers());
  }

  /**
   * Set number of channels.
   *
   * @param buffers the audio bus buffers
   * @param num_channels the number of channels
   * 
   * @since 5.0.0
   */
  void ags_vst_audio_bus_buffers_set_num_channels(AgsVstAudioBusBuffers *buffers,
						  gint32 num_channels)
  {
    ((Steinberg::Vst::AudioBusBuffers *) buffers)->numChannels = num_channels;
  }
  
  /**
   * Set silence flags.
   *
   * @param buffers the audio bus buffers
   * @param silences_flags the silence flags
   * 
   * @since 5.0.0
   */
  void ags_vst_audio_bus_buffers_set_silence_flags(AgsVstAudioBusBuffers *buffers,
						   guint64 silence_flags)
  {
    ((Steinberg::Vst::AudioBusBuffers *) buffers)->silenceFlags = silence_flags;
  }
  
  /**
   * Set samples 32-bit.
   *
   * @param buffers the audio bus buffers
   * @param channel_buffers the channel buffers
   * 
   * @since 5.0.0
   */
  void ags_vst_audio_bus_buffers_set_samples32(AgsVstAudioBusBuffers *buffers,
					       gfloat **channel_buffers)
  {
    ((Steinberg::Vst::AudioBusBuffers *) buffers)->channelBuffers32 = channel_buffers;
  }
  
  /**
   * Set samples 64-bit.
   *
   * @param buffers the audio bus buffers
   * @param channel_buffers the channel buffers
   * 
   * @since 5.0.0
   */
  void ags_vst_audio_bus_buffers_set_samples64(AgsVstAudioBusBuffers *buffers,
					       gdouble **channel_buffers)
  {
    ((Steinberg::Vst::AudioBusBuffers *) buffers)->channelBuffers64 = channel_buffers;
  }

  /**
   * Alloc.
   *
   * @return the Steinberg::Vst::ProcessData as AgsVstProcessData
   * 
   * @since 5.0.0
   */
  AgsVstProcessData* ags_vst_process_data_alloc()
  {
    return((AgsVstProcessData *) new Steinberg::Vst::ProcessData());
  }

  /**
   * Set process mode.
   *
   * @param data the process data
   * @param process_mode the process mode
   * 
   * @since 5.0.0
   */
  void ags_vst_process_data_set_process_mode(AgsVstProcessData *data,
					     gint32 process_mode)
  {
    ((Steinberg::Vst::ProcessData *) data)->processMode = process_mode;
  }

  /**
   * Set symbolic sample size.
   *
   * @param data the process data
   * @param symbolic_sample_size the symbolic sample size
   *
   * @since 5.0.0
   */
  void ags_vst_process_data_set_symbolic_sample_size(AgsVstProcessData *data,
						     gint32 symbolic_sample_size)
  {
    ((Steinberg::Vst::ProcessData *) data)->symbolicSampleSize = symbolic_sample_size;
  }
  
  /**
   * Set number of samples.
   *
   * @param data the process data
   * @param num_samples the number of samples
   * 
   * @since 5.0.0
   */
  void ags_vst_process_data_set_num_samples(AgsVstProcessData *data,
					    gint32 num_samples)
  {
    ((Steinberg::Vst::ProcessData *) data)->numSamples = num_samples;
  }
  
  /**
   * Set number of inputs.
   *
   * @param data process data
   * @param num_inputs the number of inputs
   * 
   * @since 5.0.0
   */
  void ags_vst_process_data_set_num_inputs(AgsVstProcessData *data,
					   gint32 num_inputs)
  {
    ((Steinberg::Vst::ProcessData *) data)->numInputs = num_inputs;
  }
  
  /**
   * Set number of outputs.
   *
   * @param data process data
   * @param num_outputs the number of outputs
   * 
   * @since 5.0.0
   */
  void ags_vst_process_data_set_num_outputs(AgsVstProcessData *data,
					    gint32 num_outputs)
  {
    ((Steinberg::Vst::ProcessData *) data)->numOutputs = num_outputs;
  }

  /**
   * Set inputs.
   *
   * @param data process data
   * @param buffers the buffers
   * 
   * @since 5.0.0
   */
  void ags_vst_process_data_set_inputs(AgsVstProcessData *data,
				       AgsVstAudioBusBuffers *buffers)
  {
    ((Steinberg::Vst::ProcessData *) data)->inputs = (Steinberg::Vst::AudioBusBuffers *) buffers;
  }
  
  /**
   * Set outputs.
   *
   * @param data process data
   * @param buffers the buffers
   * 
   * @since 5.0.0
   */
  void ags_vst_process_data_set_outputs(AgsVstProcessData *data,
					AgsVstAudioBusBuffers *buffers)
  {
    ((Steinberg::Vst::ProcessData *) data)->outputs = (Steinberg::Vst::AudioBusBuffers *) buffers;
  }

  /**
   * Set input parameter changes.
   *
   * @param data process data
   * @param iparameter_changes parameter changes
   * 
   * @since 5.0.0
   */
  void ags_vst_process_data_set_input_iparameter_changes(AgsVstProcessData *data,
							 AgsVstIParameterChanges *iparameter_changes)
  {
    ((Steinberg::Vst::ProcessData *) data)->inputParameterChanges = (Steinberg::Vst::IParameterChanges *) iparameter_changes;
  }

  /**
   * Set output parameter changes.
   *
   * @param data process data
   * @param iparameter_changes parameter changes
   * 
   * @since 5.0.0
   */
  void ags_vst_process_data_set_ouput_iparameter_changes(AgsVstProcessData *data,
							 AgsVstIParameterChanges *iparameter_changes)
  {
    ((Steinberg::Vst::ProcessData *) data)->outputParameterChanges = (Steinberg::Vst::IParameterChanges *) iparameter_changes;
  }

  /**
   * Set input events.
   *
   * @param data process data
   * @param events the event list
   * 
   * @since 5.0.0
   */
  void ags_vst_process_data_set_input_events(AgsVstProcessData *data,
					     AgsVstIEventList *events)
  {
    ((Steinberg::Vst::ProcessData *) data)->inputEvents = (Steinberg::Vst::IEventList *) events;
  }

  /**
   * Set output events.
   *
   * @param data process data
   * @param events the event list
   * 
   * @since 5.0.0
   */
  void ags_vst_process_data_set_output_events(AgsVstProcessData *data,
					      AgsVstIEventList *events)
  {
    ((Steinberg::Vst::ProcessData *) data)->outputEvents = (Steinberg::Vst::IEventList *) events;
  }

  /**
   * Set process context.
   *
   * @param data process data
   * @param context process context
   * 
   * @since 5.0.0
   */
  void ags_vst_process_data_set_process_context(AgsVstProcessData *data,
						AgsVstProcessContext *context)
  {
    ((Steinberg::Vst::ProcessData *) data)->processContext = (Steinberg::Vst::ProcessContext *) context;
  }

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_iaudio_processor_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IAudioProcessor::iid.toTUID()));
  }

  /**
   * Set bus arrangements.
   *
   * @param iaudio_processor the audio processor
   * @param inputs the inputs
   * @param num_ins the number of inputs
   * @param outputs the outputs
   * @param num_outs the number of outputs
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iaudio_processor_set_bus_arrangements(AgsVstIAudioProcessor *iaudio_processor,
							      AgsVstSpeakerArrangement *inputs, gint32 num_ins,
							      AgsVstSpeakerArrangement *outputs, gint32 num_outs)
  {
    return(((Steinberg::Vst::IAudioProcessor *) iaudio_processor)->setBusArrangements(inputs, num_ins,
										      outputs, num_outs));
  }

  /**
   * Get bus arrangement.
   *
   * @param iaudio_processor the audio processor
   * @param dir the direction
   * @param index the index
   * @param arr the return location of speaker arrangement
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iaudio_processor_get_bus_arrangement(AgsVstIAudioProcessor *iaudio_processor,
							     AgsVstBusDirection dir, gint32 index,
							     AgsVstSpeakerArrangement *arr)
  {
    const Steinberg::Vst::SpeakerArrangement& tmp_arr_0 = const_cast<Steinberg::Vst::SpeakerArrangement&>(arr[0]);
    
    return(((Steinberg::Vst::IAudioProcessor *) iaudio_processor)->getBusArrangement(dir, index,
										     const_cast<Steinberg::Vst::SpeakerArrangement&>(tmp_arr_0)));
  }

  /**
   * Can process sample size.
   *
   * @param iaudio_processor the audio processor
   * @param symbolic_sample_size the symbolic sample size
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iaudio_processor_can_process_sample_size(AgsVstIAudioProcessor *iaudio_processor,
								 gint32 symbolic_sample_size)
  {
    return(((Steinberg::Vst::IAudioProcessor *) iaudio_processor)->canProcessSampleSize(symbolic_sample_size));
  }

  /**
   * Get latency samples.
   *
   * @param iaudio_processor the audio processor
   * @return the latency
   * 
   * @since 5.0.0
   */
  guint32 ags_vst_iaudio_processor_get_latency_samples(AgsVstIAudioProcessor *iaudio_processor)
  {
    return(((Steinberg::Vst::IAudioProcessor *) iaudio_processor)->getLatencySamples());
  }

  /**
   * Setup processing.
   *
   * @param iaudio_processor the audio processor
   * @param setup the process setup
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iaudio_processor_setup_processing(AgsVstIAudioProcessor *iaudio_processor,
							  AgsVstProcessSetup *setup)
  {
    const Steinberg::Vst::ProcessSetup& tmp_setup_0 = const_cast<Steinberg::Vst::ProcessSetup&>(((Steinberg::Vst::ProcessSetup *) setup)[0]);
  
    return(((Steinberg::Vst::IAudioProcessor *) iaudio_processor)->setupProcessing(const_cast<Steinberg::Vst::ProcessSetup&>(tmp_setup_0)));
  }

  /**
   * Set processing.
   *
   * @param iaudio_processor the audio processor
   * @param state true if processing, otherwise false
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iaudio_processor_set_processing(AgsVstIAudioProcessor *iaudio_processor,
							gboolean state)
  {
    return(((Steinberg::Vst::IAudioProcessor *) iaudio_processor)->setProcessing(state));
  }

  /**
   * Process.
   *
   * @param iaudio_processor the audio processor
   * @param data the process data
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iaudio_processor_process(AgsVstIAudioProcessor *iaudio_processor,
						 AgsVstProcessData *data)
  {
    return(((Steinberg::Vst::IAudioProcessor *) iaudio_processor)->process(const_cast<Steinberg::Vst::ProcessData&>(((Steinberg::Vst::ProcessData *) data)[0])));
  }
  
  guint32 ags_vst_iaudio_processor_get_tail_samples(AgsVstIAudioProcessor *iaudio_processor)
  {
    return(((Steinberg::Vst::IAudioProcessor *) iaudio_processor)->getTailSamples());
  }

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_iaudio_presentation_latency_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IAudioPresentationLatency::iid.toTUID()));
  }

  /**
   * Set audio presentation latency samples.
   *
   * @param iaudio_presentation_latency the audio presentation latency
   * @param dir the direction
   * @param bus_index the bus index
   * @param latency_in_samples the latency in samples
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iaudio_presentation_latency_set_audio_presentation_latency_samples(AgsVstIAudioPresentationLatency *iaudio_presentation_latency,
											   AgsVstBusDirection dir, gint32 bus_index,
											   guint32 latency_in_samples)
  {
    return(((Steinberg::Vst::IAudioPresentationLatency *) iaudio_presentation_latency)->setAudioPresentationLatencySamples(dir, bus_index,
															   latency_in_samples));
  }

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_iprocess_context_requirements_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IProcessContextRequirements::iid.toTUID()));
  }
  
  /**
   * Set audio presentation latency samples.
   *
   * @param iprocess_context_requirements process context requirements
   * @return the requirements
   * 
   * @since 5.0.0
   */
  guint32 ags_vst_iprocess_context_requirements_get_process_context_requirements(AgsVstIProcessContextRequirements *iprocess_context_requirements)
  {
    return(((Steinberg::Vst::IProcessContextRequirements *) iprocess_context_requirements)->getProcessContextRequirements());
  }  

}
