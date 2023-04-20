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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iprocess_context.h>

#include <pluginterfaces/vst/ivstprocesscontext.h>

extern "C" {

  /**
   * Set frames per second.
   *
   * @param frame_rate the frame rate
   * @param frames_per_second the frames per second
   *
   * @since 5.0.0
   */
  void ags_vst_frame_rate_set_frames_per_second(AgsVstFrameRate *frame_rate,
						guint32 frames_per_second)
  {
    ((Steinberg::Vst::FrameRate *) frame_rate)->framesPerSecond = frames_per_second;
  }

  /**
   * Set flags.
   *
   * @param frame_rate the frame rate
   * @param flags the flags
   *
   * @since 5.0.0
   */
  void ags_vst_frame_rate_set_flags(AgsVstFrameRate *frame_rate,
				    guint32 flags)
  {
    ((Steinberg::Vst::FrameRate *) frame_rate)->flags = flags;
  }
  
  /**
   * Set key note.
   *
   * @param chord the chord
   * @param key_note the key note
   *
   * @since 5.0.0
   */
  void ags_vst_chord_set_key_note(AgsVstChord *chord,
				  guint8 key_note)
  {
    ((Steinberg::Vst::Chord *) chord)->keyNote = key_note;
  }

  /**
   * Set root note.
   *
   * @param chord the chord
   * @param root_note the root note
   *
   * @since 5.0.0
   */
  void ags_vst_chord_set_root_note(AgsVstChord *chord,
				   guint8 root_note)
  {
    ((Steinberg::Vst::Chord *) chord)->rootNote = root_note;
  }
  
  /**
   * Set chord mask.
   *
   * @param chord the chord
   * @param chord_mask the chord mask
   *
   * @since 5.0.0
   */
  void ags_vst_chord_set_chord_mask(AgsVstChord *chord,
				    gint16 chord_mask)
  {
    ((Steinberg::Vst::Chord *) chord)->chordMask = chord_mask;
  }

  /**
   * Alloc.
   *
   * @return the Steinberg::Vst::ProcessContext as AgsVstProcessContext
   * 
   * @since 5.0.0
   */
  AgsVstProcessContext* ags_vst_process_context_alloc()
  {
    return((AgsVstProcessContext *) new Steinberg::Vst::ProcessContext());
  }

  /**
   * Set state.
   *
   * @param process_context the process context
   * @param state the state
   * 
   * @since 5.0.0
   */
  void ags_vst_process_context_set_state(AgsVstProcessContext *process_context,
					 guint32 state)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->state = state; 
  }

  /**
   * Set samplerate.
   *
   * @param process_context the process context
   * @param samplerate the samplerate
   * 
   * @since 5.0.0
   */
  void ags_vst_process_context_set_samplerate(AgsVstProcessContext *process_context,
					      gdouble samplerate)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->sampleRate = samplerate; 
  }

  /**
   * Set project time samples.
   *
   * @param process_context the process context
   * @param project_time_samples the project time samples
   * 
   * @since 5.0.0
   */
  void ags_vst_process_context_set_project_time_samples(AgsVstProcessContext *process_context,
							AgsVstTSamples project_time_samples)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->projectTimeSamples = project_time_samples; 
  }

  /**
   * Set system time.
   *
   * @param process_context the process context
   * @param system_time the system time
   * 
   * @since 5.0.0
   */
  void ags_vst_process_context_set_system_time(AgsVstProcessContext *process_context,
					       gint64 system_time)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->systemTime = system_time; 
  }

  /**
   * Set continous time samples.
   *
   * @param process_context the process context
   * @param continous_time_samples the continous time samples
   * 
   * @since 5.0.0
   */
  void ags_vst_process_context_set_continous_time_samples(AgsVstProcessContext *process_context,
							  AgsVstTSamples continous_time_samples)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->continousTimeSamples = continous_time_samples; 
  }

  /**
   * Set project time music.
   *
   * @param process_context the process context
   * @param project_time_music the project time music
   * 
   * @since 5.0.0
   */
  void ags_vst_process_context_set_project_time_music(AgsVstProcessContext *process_context,
						      AgsVstTQuarterNotes project_time_music)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->projectTimeMusic = project_time_music; 
  }

  /**
   * Set bar position music.
   *
   * @param process_context the process context
   * @param bar_position_music the bar poisition music
   * 
   * @since 5.0.0
   */
  void ags_vst_process_context_set_bar_position_music(AgsVstProcessContext *process_context,
						      AgsVstTQuarterNotes bar_position_music)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->barPositionMusic = bar_position_music; 
  }

  /**
   * Set cycle start music.
   *
   * @param process_context the process context
   * @param cycle_start_music the cycle start music
   * 
   * @since 5.0.0
   */
  void ags_vst_process_context_set_cycle_start_music(AgsVstProcessContext *process_context,
						     AgsVstTQuarterNotes cycle_start_music)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->cycleStartMusic = cycle_start_music; 
  }

  /**
   * Set cycle end music.
   *
   * @param process_context the process context
   * @param cycle_end_music the cycle end music
   * 
   * @since 5.0.0
   */
  void ags_vst_process_context_set_cycle_end_music(AgsVstProcessContext *process_context,
						   AgsVstTQuarterNotes cycle_end_music)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->cycleEndMusic = cycle_end_music; 
  }

  /**
   * Set tempo.
   *
   * @param process_context the process context
   * @param tempo the tempo
   * 
   * @since 5.0.0
   */
  void ags_vst_process_context_set_tempo(AgsVstProcessContext *process_context,
					 gdouble tempo)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->tempo = tempo; 
  }

  /**
   * Set time signature numerator.
   *
   * @param process_context the process context
   * @param time_sig_numerator the time signature numerator
   * 
   * @since 5.0.0
   */
  void ags_vst_process_context_set_time_sig_numerator(AgsVstProcessContext *process_context,
						      guint32 time_sig_numerator)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->timeSigNumerator = time_sig_numerator; 
  }

  /**
   * Set time signature denominator.
   *
   * @param process_context the process context
   * @param time_sig_denominator the time signature denominator
   * 
   * @since 5.0.0
   */
  void ags_vst_process_context_set_time_sig_denominator(AgsVstProcessContext *process_context,
							guint32 time_sig_denominator)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->timeSigDenominator = time_sig_denominator; 
  }

  /**
   * Get chord.
   *
   * @param process_context the process context
   * @return the chord
   * 
   * @since 5.0.0
   */
  AgsVstChord* ags_vst_process_context_get_chord(AgsVstProcessContext *process_context)
  {
    return((AgsVstChord *) &(((Steinberg::Vst::ProcessContext *) process_context)->chord));
  }

  /**
   * Set SMPTE offset sub-frames.
   *
   * @param process_context the process context
   * @param smpte_offset_sub_frames the SMPTE offset sub-frames
   * 
   * @since 5.0.0
   */
  void ags_vst_process_context_set_smpte_offset_sub_frames(AgsVstProcessContext *process_context,
							   gint32 smpte_offset_sub_frames)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->smpteOffsetSubframes = smpte_offset_sub_frames;
  }
  
  /**
   * Get frame rate.
   *
   * @param process_context the process context
   * @return the frame rate
   * 
   * @since 5.0.0
   */
  AgsVstFrameRate* ags_vst_process_context_get_frame_rate(AgsVstProcessContext *process_context)
  {
    return((AgsVstFrameRate *) &(((Steinberg::Vst::ProcessContext *) process_context)->frameRate));
  }
  
  /**
   * Set samples to next clock.
   *
   * @param process_context the process context
   * @param samples_to_next_clock the samples to next clock
   * 
   * @since 5.0.0
   */
  void ags_vst_process_context_set_samples_to_next_clock(AgsVstProcessContext *process_context,
							 gint32 samples_to_next_clock)
  {
    ((Steinberg::Vst::ProcessContext *) process_context)->samplesToNextClock = samples_to_next_clock;
  }

}
