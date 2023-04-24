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

#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_audio_effect.h>

#include <public.sdk/source/vst/vstaudioeffect.h>

extern "C" {

  /**
   * Instantiate new Steinberg::Vst::AudioEffect the constructor as a C99
   * compatible handle a void pointer.
   * 
   * @return the AgsVstAudioEffect
   * 
   * @since 2.2.0
   */
  AgsVstAudioEffect* ags_vst_audio_effect_new()
  {
    return((AgsVstAudioEffect *) new Steinberg::Vst::AudioEffect());
  }

  /**
   * Delete.
   *
   * @param audio_effect the audio effect
   *
   * @since 2.2.0
   */
  void ags_vst_audio_effect_delete(AgsVstAudioEffect *audio_effect)
  {
    delete audio_effect;
  }

  /**
   * Add audio input with name and arr of bus type using flags.
   * 
   * @param audio_effect the audio effect
   * @param name the name
   * @param arr the speaker arrangement
   * @param bus_type the bus type as bus types
   * @param flags the flags
   * @return the audio bus
   * 
   * @since 2.2.0
   */
  AgsVstAudioBus* ags_vst_audio_effect_add_audio_input(AgsVstAudioEffect *audio_effect,
						       AgsVstTChar *name, AgsVstSpeakerArrangement arr, AgsVstBusType bus_type,
						       gint32 flags)
  {
    return((AgsVstAudioBus*) static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->addAudioInput(static_cast<Steinberg::Vst::TChar*>((void *) name), arr, static_cast<Steinberg::Vst::BusType>(bus_type),
													    flags));
  }
  
  /**
   * Add audio output with name and arr of bus type using flags.
   * 
   * @param audio_effect the audio effect
   * @param name the name as string
   * @param arr the speaker arrangement
   * @param bus_type the bus type
   * @param flags the flags
   * @return the audio bus
   * 
   * @since 2.2.0
   */
  AgsVstAudioBus* ags_vst_audio_effect_add_audio_output(AgsVstAudioEffect *audio_effect,
							AgsVstTChar *name, AgsVstSpeakerArrangement arr, AgsVstBusType bus_type,
							gint32 flags)
  {
    return((AgsVstAudioBus*) static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->addAudioOutput(static_cast<Steinberg::Vst::TChar*>((void *) name), arr, static_cast<Steinberg::Vst::BusType>(bus_type),
													     flags));
  }

  /**
   * Get audio input by index.
   * 
   * @param audio_effect the audio effect
   * @param index the index to get
   * @return the audio bus
   * 
   * @since 2.2.0
   */
  AgsVstAudioBus* ags_vst_audio_effect_get_audio_input(AgsVstAudioEffect *audio_effect,
						       gint32 index)
  {
    return((AgsVstAudioBus*) static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->getAudioInput(index));
  }
  
  /**
   * Get audio output by index.
   * 
   * @param audio_effect the audio effect
   * @param index the index to get
   * @return the audio bus
   * 
   * @since 2.2.0
   */
  AgsVstAudioBus* ags_vst_audio_effect_get_audio_output(AgsVstAudioEffect *audio_effect,
							gint32 index)
  {
    return((AgsVstAudioBus*) static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->getAudioOutput(index));
  }

  /**
   * Add event input with name and channels of bus type using flags.
   * 
   * @param audio_effect the audio effect
   * @param name the name as string
   * @param channels channels
   * @param bus_type the bus type
   * @param flags the flags
   * @return the event bus
   * 
   * @since 2.2.0
   */
  AgsVstEventBus* ags_vst_audio_effect_add_event_input(AgsVstAudioEffect *audio_effect,
						       AgsVstTChar *name, gint32 channels, AgsVstBusType bus_type,
						       gint32 flags)
  {
    return((AgsVstEventBus*) static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->addEventInput(static_cast<Steinberg::Vst::TChar*>((void *) name), channels, static_cast<Steinberg::Vst::BusType>(bus_type),
													    flags));
  }

  /**
   * Add event output with name and channels of bus type using flags.
   * 
   * @param audio_effect the audio effect
   * @param name the name as string
   * @param channels channels
   * @param bus_type the bus type
   * @param flags the flags
   * @return the event bus
   * 
   * @since 2.2.0
   */
  AgsVstEventBus* ags_vst_audio_effect_add_event_output(AgsVstAudioEffect *audio_effect,
							AgsVstTChar *name, gint32 channels, AgsVstBusType bus_type,
							gint32 flags)
  {
    return((AgsVstEventBus*) static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->addEventOutput(static_cast<Steinberg::Vst::TChar*>((void *) name), channels, static_cast<Steinberg::Vst::BusType>(bus_type),
													     flags));
  }

  /**
   * Get event input by index.
   * 
   * @param audio_effect the audio effect
   * @param index the index to get
   * @return the event bus
   * 
   * @since 2.2.0
   */
  AgsVstEventBus* ags_vst_audio_effect_get_event_input(AgsVstAudioEffect *audio_effect,
						       gint32 index)
  {
    return((AgsVstEventBus*) static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->getEventInput(index));
  }

  /**
   * Get event output by index.
   * 
   * @param audio_effect the audio effect
   * @param index the index to get
   * @return the event bus
   * 
   * @since 2.2.0
   */
  AgsVstEventBus* ags_vst_audio_effect_get_event_output(AgsVstAudioEffect *audio_effect,
							gint32 index)
  {
    return((AgsVstEventBus*) static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->getEventOutput(index));
  }

  /**
   * Set bus arrangements of audio effect.
   * 
   * @param audio_effect the audio effect
   * @param inputs the speaker arrangement
   * @param num_ins number of inputs
   * @param outputs the speaker arrangement array
   * @param num_outs number of outputs
   * @return the result code
   * 
   * @since 2.2.0
   */
  gint32 ags_vst_audio_effect_set_bus_arrangements(AgsVstAudioEffect *audio_effect,
						   AgsVstSpeakerArrangement *inputs, gint32 num_ins,
						   AgsVstSpeakerArrangement *outputs, gint32 num_outs)
  {
    return(static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->setBusArrangements(inputs, num_ins,
											       outputs, num_outs));
  }
  
  /**
   * Get bus arrangements of audio effect.
   * 
   * @param audio_effect the audio effect
   * @param bus_direction the bus direction
   * @param bus_index the bus index
   * @param arr the speaker arrangement array
   * @return the result code
   * 
   * @since 2.2.0
   */
  gint32 ags_vst_audio_effect_get_bus_arrangement(AgsVstAudioEffect *audio_effect,
						  AgsVstBusDirection bus_direction, gint32 bus_index,
						  AgsVstSpeakerArrangement *arr)
  {
    Steinberg::Vst::SpeakerArrangement tmp_arr_0 = static_cast<Steinberg::Vst::SpeakerArrangement>(arr[0]);
    const Steinberg::Vst::SpeakerArrangement& tmp_arr_1 = const_cast<Steinberg::Vst::SpeakerArrangement&>(tmp_arr_0);
    
    return(static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->getBusArrangement(static_cast<Steinberg::Vst::BusDirection>(bus_direction), bus_index,
											      const_cast<Steinberg::Vst::SpeakerArrangement&>(tmp_arr_1)));
  }
  
  /**
   * Can audio effect process symbolic sample size.
   * 
   * @param audio_effect the audio effect
   * @param symbolic_sample_size symbolic sample size
   * @return the result code
   * 
   * @since 2.2.0
   */
  gint32 ags_vst_audio_effect_can_process_sample_size(AgsVstAudioEffect *audio_effect,
						      gint32 symbolic_sample_size)
  {
    return(static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->canProcessSampleSize(symbolic_sample_size));
  }
  
  /**
   * Get lantency samples of audio effect.
   * 
   * @param audio_effect the audio effect
   * @return the latency
   * 
   * @since 2.2.0
   */
  guint32 ags_vst_audio_effect_get_latency_samples(AgsVstAudioEffect *audio_effect)
  {
    return(static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->getLatencySamples());
  }
  
  /**
   * Setup processing of audio effect.
   * 
   * @param audio_effect the #AgsVstAudioEffect-struct pointer
   * @param setup the process setup-struct pointer
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_audio_effect_setup_processing(AgsVstAudioEffect *audio_effect,
					       AgsVstProcessSetup *setup)
  {
    Steinberg::Vst::ProcessSetup *tmp_setup_0 = static_cast<Steinberg::Vst::ProcessSetup*>((void *) setup);
    const Steinberg::Vst::ProcessSetup& tmp_setup_1 = const_cast<Steinberg::Vst::ProcessSetup&>(tmp_setup_0[0]);
    
    return(static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->setupProcessing(const_cast<Steinberg::Vst::ProcessSetup&>(tmp_setup_1)));
  }
  
  /**
   * Set processing of audio effect.
   * 
   * @param audio_effect the audio effect
   * @param state true if processing, otherwise false
   * @return the result code
   * 
   * @since 2.2.0
   */
  gint32 ags_vst_audio_effect_set_processing(AgsVstAudioEffect *audio_effect,
					     gboolean state)
  {
    return(static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->setProcessing(static_cast<Steinberg::TBool>(state)));
  }
  
  /**
   * Process  audio effect using data.
   * 
   * @param audio_effect the audio effect
   * @param data the process data
   * @return the result code
   * 
   * @since 2.2.0
   */
  gint32 ags_vst_audio_effect_process(AgsVstAudioEffect *audio_effect,
				      AgsVstProcessData *data)
  {
    Steinberg::Vst::ProcessData *tmp_data_0 = static_cast<Steinberg::Vst::ProcessData*>((void *) data);
    const Steinberg::Vst::ProcessData& tmp_data_1 = const_cast<Steinberg::Vst::ProcessData&>(tmp_data_0[0]);    
    
    return(static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->process(const_cast<Steinberg::Vst::ProcessData&>(tmp_data_1)));
  }
  
  /**
   * Get tail samples of audio effect.
   * 
   * @param audio_effect the audio effect
   * @return the tail samples
   * 
   * @since 2.2.0
   */
  guint32 ags_vst_audio_effect_get_tail_samples(AgsVstAudioEffect *audio_effect)
  {
    return(static_cast<Steinberg::Vst::AudioEffect*>((void*) audio_effect)->getTailSamples());
  }

}
