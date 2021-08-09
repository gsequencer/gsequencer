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

#ifndef __AGS_VST_IAUDIO_PROCESSOR_H__
#define __AGS_VST_IAUDIO_PROCESSOR_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_icomponent.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_speaker.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstProcessSetup ProcessSetup;
  typedef struct AgsVstAudioBusBuffers AudioBusBuffers;
  typedef struct AgsVstProcessData ProcessData;
  
  typedef struct AgsVstIAudioProcessor IAudioProcessor;

  typedef struct AgsVstIAudioPresentationLatency IAudioPresentationLatency;

  typedef struct AgsVstIProcessContextRequirements IProcessContextRequirements;
      
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

  typedef enum{
    AGS_VST_KDISTRIBUTABLE           = 1,
    AGS_VST_KSIMPLE_MODE_SUPPORTED   = 1 <<  1,
  }AgsVstComponentFlags;

  typedef enum{
    AGS_VST_KSAMPLE32,
    AGS_VST_KSAMPLE64,
  }AgsVstSymbolicSampleSizes;

  typedef enum{
    AGS_VST_KREALTIME,
    AGS_VST_KPREFETCH,
    AGS_VST_KOFFLINE,
  }AgsVstProcessModes;

  typedef enum
  {
    AGS_VST_KNEED_SYSTEM_TIME                  = 1,
    AGS_VST_KNEED_CONTINOUS_TIME_SAMPLES       = 1 <<  1,
    AGS_VST_KNEED_PROJECT_TIME_MUSIC           = 1 <<  2,
    AGS_VST_KNEED_BAR_POSITION_MUSIC           = 1 <<  3,
    AGS_VST_KNEED_CYCLE_MUSIC                  = 1 <<  4,
    AGS_VST_KNEED_SAMPLES_TO_NEXT_CLOCK        = 1 <<  5,
    AGS_VST_KNEED_TEMPO                        = 1 <<  6,
    AGS_VST_KNEED_TIME_SIGNATURE               = 1 <<  7, 
    AGS_VST_KNEED_CHORD                        = 1 <<  8,
    AGS_VST_KNEED_FRAME_RATE                   = 1 <<  9,
    AGS_VST_KNEED_TRANSPORT_STATE              = 1 << 10,
  }AgsVstIProcessContextRequirementsFlags;
  
  const AgsVstTUID* ags_vst_iaudio_processor_get_iid();

  AgsVstTResult ags_vst_iaudio_processor_set_bus_arrangements(AgsVstIAudioProcessor *iaudio_processor,
							      AgsVstSpeakerArrangement *inputs, gint32 num_ins,
							      AgsVstSpeakerArrangement *outputs, gint32 num_outs);

  AgsVstTResult ags_vst_iaudio_processor_get_bus_arrangement(AgsVstIAudioProcessor *iaudio_processor,
							     AgsVstBusDirection dir, gint32 index,
							     AgsVstSpeakerArrangement *arr);
  
  AgsVstTResult ags_vst_iaudio_processor_can_process_sample_size(AgsVstIAudioProcessor *iaudio_processor,
								 gint32 symbolic_sample_size);

  guint32 ags_vst_iaudio_processor_get_latency_samples(AgsVstIAudioProcessor *iaudio_processor);

  AgsVstTResult ags_vst_iaudio_processor_setup_processing(AgsVstIAudioProcessor *iaudio_processor,
							  AgsVstProcessSetup *setup);

  AgsVstTResult ags_vst_iaudio_processor_set_processing(AgsVstIAudioProcessor *iaudio_processor,
							gboolean state);

  AgsVstTResult ags_vst_iaudio_processor_process(AgsVstIAudioProcessor *iaudio_processor,
						 AgsVstProcessData *data);
  
  guint32 ags_vst_iaudio_processor_get_tail_samples(AgsVstIAudioProcessor *iaudio_processor);

  const AgsVstTUID* ags_vst_iaudio_presentation_latency_get_iid();

  AgsVstTResult ags_vst_iaudio_presentation_latency_set_audio_presentation_latency_samples(AgsVstIAudioPresentationLatency *iaudio_presentation_latency,
											   AgsVstBusDirection dir, gint32 bus_index,
											   guint32 latency_in_samples);

  const AgsVstTUID* ags_vst_iprocess_context_requirements_get_iid();
  
  guint32 ags_vst_iprocess_context_requirements_get_process_context_requirements(AgsVstIProcessContextRequirements *iprocess_context_requirements);  
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IAUDIO_PROCESSOR_H__*/
