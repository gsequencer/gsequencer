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

#ifndef __AGS_VST_IPROCESS_CONTEXT_H__
#define __AGS_VST_IPROCESS_CONTEXT_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_types.h>

#ifdef __cplusplus
extern "C" {
#endif
  
  typedef enum{
    AGS_VST_KPULL_DOWN_RATE = 1,
    AGS_VST_KDROP_RATE      = 1 << 1,
  }AgsVstFrameRateFlags;

  typedef enum{
    AGS_VST_KPLAYING                      = 1 <<  1,
    AGS_VST_KCYCLE_ACTIVE                 = 1 <<  2,
    AGS_VST_KRECORDING                    = 1 <<  3,
    AGS_VST_KSYSTEM_TIME_VALID            = 1 <<  8,
    AGS_VST_KCONST_TIME_VALID             = 1 << 17,
    AGS_VST_KPROJECT_TIME_MUSIC_VALID     = 1 <<  9,
    AGS_VST_KBAR_POSITION_VALID           = 1 << 11,
    AGS_VST_kCycleValid                   = 1 << 12,
    AGS_VST_KTEMPO_VALID                  = 1 << 10,
    AGS_VST_KTIME_SIG_VALID               = 1 << 13,
    AGS_VST_KCHORD_VALID                  = 1 << 18,    
    AGS_VST_KSMPTE_VALID                  = 1 << 14,
    AGS_VST_KCLOCK_VALID                  = 1 << 15
  }AgsVstStatesAndFlags;

  typedef struct FrameRate AgsVstFrameRate;
  typedef struct Chord AgsVstChord;
  typedef struct ProcessContext AgsVstProcessContext;

  void ags_vst_frame_rate_set_frames_per_second(AgsVstFrameRate *frame_rate,
						guint32 frames_per_second);

  void ags_vst_frame_rate_set_flags(AgsVstFrameRate *frame_rate,
				    guint32 flags);
  
  void ags_vst_chord_set_key_note(AgsVstChord *chord,
				  guint8 key_note);

  void ags_vst_chord_set_root_note(AgsVstChord *chord,
				   guint8 root_note);
  
  void ags_vst_chord_set_chord_mask(AgsVstChord *chord,
				    gint16 chord_mask);

  AgsVstProcessContext* ags_vst_process_context_alloc();

  void ags_vst_process_context_set_state(AgsVstProcessContext *process_context,
					 guint32 state);

  void ags_vst_process_context_set_samplerate(AgsVstProcessContext *process_context,
					      gdouble samplerate);

  void ags_vst_process_context_set_project_time_samples(AgsVstProcessContext *process_context,
							AgsVstTSamples project_time_samples);

  void ags_vst_process_context_set_system_time(AgsVstProcessContext *process_context,
					       gint64 system_time);

  void ags_vst_process_context_set_continous_time_samples(AgsVstProcessContext *process_context,
							  AgsVstTSamples continous_time_samples);

  void ags_vst_process_context_set_project_time_music(AgsVstProcessContext *process_context,
						      AgsVstTQuarterNotes project_time_music);

  void ags_vst_process_context_set_bar_position_music(AgsVstProcessContext *process_context,
						      AgsVstTQuarterNotes bar_position_music);

  void ags_vst_process_context_set_cycle_start_music(AgsVstProcessContext *process_context,
						     AgsVstTQuarterNotes cycle_start_music);

  void ags_vst_process_context_set_cycle_end_music(AgsVstProcessContext *process_context,
						   AgsVstTQuarterNotes cycle_end_music);

  void ags_vst_process_context_set_tempo(AgsVstProcessContext *process_context,
					 gdouble tempo);

  void ags_vst_process_context_set_time_sig_numerator(AgsVstProcessContext *process_context,
						      guint32 time_sig_numerator);

  void ags_vst_process_context_set_time_sig_denominator(AgsVstProcessContext *process_context,
							guint32 time_sig_denominator);

  AgsVstChord* ags_vst_process_context_get_chord(AgsVstProcessContext *process_context);

  void ags_vst_process_context_set_smpte_offset_sub_frames(AgsVstProcessContext *process_context,
							   gint32 smpte_offset_sub_frames);

  AgsVstFrameRate* ags_vst_process_context_get_frame_rate(AgsVstProcessContext *process_context);
  
  void ags_vst_process_context_set_samples_to_next_clock(AgsVstProcessContext *process_context,
							 gint32 samples_to_next_clock);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IPROCESS_CONTEXT_H__*/
