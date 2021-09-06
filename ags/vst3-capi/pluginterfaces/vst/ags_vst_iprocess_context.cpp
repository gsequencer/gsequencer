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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iprocess_context.h>

#include <pluginterfaces/vst/ivstprocesscontext.h>

extern "C" {

  void ags_vst_frame_rate_set_frames_per_second(AgsVstFrameRate *frame_rate,
						guint32 frames_per_second)
  {
    ((Steinberg::Vst::FrameRate *) frame_rate)->framesPerSecond = frames_per_second;
  }

  void ags_vst_frame_rate_set_flags(AgsVstFrameRate *frame_rate,
				    guint32 flags)
  {
    ((Steinberg::Vst::FrameRate *) frame_rate)->flags = flags;
  }
  
  void ags_vst_chord_set_key_note(AgsVstChord *chord,
				  guint8 key_note)
  {
    ((Steinberg::Vst::Chord *) chord)->keyNote = key_note;
  }

  void ags_vst_chord_set_root_note(AgsVstChord *chord,
				   guint8 root_note)
  {
    ((Steinberg::Vst::Chord *) chord)->rootNote = root_note;
  }
  
  void ags_vst_chord_set_chord_mask(AgsVstChord *chord,
				    gint16 chord_mask)
  {
    ((Steinberg::Vst::Chord *) chord)->chordMask = chord_mask;
  }

  AgsVstProcessContext* ags_vst_process_context_alloc()
  {
    return((AgsVstProcessContext *) new Steinberg::Vst::ProcessContext());
  }

  void ags_vst_process_context_set_state(AgsVstProcessContext *process_context,
					 guint32 state)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->state = state; 
  }

  void ags_vst_process_context_set_samplerate(AgsVstProcessContext *process_context,
					      gdouble samplerate)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->sampleRate = samplerate; 
  }

  void ags_vst_process_context_set_project_time_samples(AgsVstProcessContext *process_context,
							AgsVstTSamples project_time_samples)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->projectTimeSamples = project_time_samples; 
  }

  void ags_vst_process_context_set_system_time(AgsVstProcessContext *process_context,
					       gint64 system_time)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->systemTime = system_time; 
  }

  void ags_vst_process_context_set_continous_time_samples(AgsVstProcessContext *process_context,
							  AgsVstTSamples continous_time_samples)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->continousTimeSamples = continous_time_samples; 
  }

  void ags_vst_process_context_set_project_time_music(AgsVstProcessContext *process_context,
						      AgsVstTQuarterNotes project_time_music)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->projectTimeMusic = project_time_music; 
  }

  void ags_vst_process_context_set_bar_position_music(AgsVstProcessContext *process_context,
						      AgsVstTQuarterNotes bar_position_music)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->barPositionMusic = bar_position_music; 
  }

  void ags_vst_process_context_set_cycle_start_music(AgsVstProcessContext *process_context,
						     AgsVstTQuarterNotes cycle_start_music)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->cycleStartMusic = cycle_start_music; 
  }

  void ags_vst_process_context_set_cycle_end_music(AgsVstProcessContext *process_context,
						   AgsVstTQuarterNotes cycle_end_music)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->cycleEndMusic = cycle_end_music; 
  }

  void ags_vst_process_context_set_tempo(AgsVstProcessContext *process_context,
					 gdouble tempo)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->tempo = tempo; 
  }

  void ags_vst_process_context_set_time_sig_numerator(AgsVstProcessContext *process_context,
						      guint32 time_sig_numerator)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->timeSigNumerator = time_sig_numerator; 
  }

  void ags_vst_process_context_set_time_sig_denominator(AgsVstProcessContext *process_context,
							guint32 time_sig_denominator)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->timeSigDenominator = time_sig_denominator; 
  }

  AgsVstChord* ags_vst_process_context_get_chord(AgsVstProcessContext *process_context)
  {
    return((AgsVstChord *) &(((Steinberg::Vst::ProcessContext *) process_context)->chord));
  }

  void ags_vst_process_context_set_smpte_offset_sub_frames(AgsVstProcessContext *process_context,
							   gint32 smpte_offset_sub_frames)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->smpteOffsetSubframes = smpte_offset_sub_frames;
  }
  
  AgsVstFrameRate* ags_vst_process_context_get_frame_rate(AgsVstProcessContext *process_context)
  {
    return((AgsVstFrameRate *) &(((Steinberg::Vst::ProcessContext *) process_context)->frameRate));
  }
  
  void ags_vst_process_context_set_samples_to_next_clock(AgsVstProcessContext *process_context,
							 gint32 samples_to_next_clock)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->samplesToNextClock = samples_to_next_clock;
  }

}
