/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/ags_sf2_synth_util.h>

#include <ags/audio/ags_synth_enums.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <math.h>
#include <complex.h>

/**
 * SECTION:ags_sf2_synth_util
 * @short_description: frequency modulation synth util
 * @title: AgsSF2SynthUtil
 * @section_id:
 * @include: ags/audio/ags_sf2_synth_util.h
 *
 * Utility functions to compute SF2 synths.
 */

/**
 * ags_sf2_synth_util_midi_locale_find_sample_near_midi_key:
 * @ipatch: the #AgsIpatch
 * @bank: the bank
 * @program: the program
 * @midi_key: the MIDI key
 * @preset: (out): the preset
 * @instrument: (out): the instrument
 * @sample: (out): the sample
 * 
 * Find sample near MIDI key.
 * 
 * Since: 3.4.0
 */
AgsIpatchSample*
ags_sf2_synth_util_midi_locale_find_sample_near_midi_key(AgsIpatch *ipatch,
							 gint bank,
							 gint program,
							 gint midi_key,
							 gchar **preset,
							 gchar **instrument,
							 gchar **sample)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_sf2_synth_util_copy_s8:
 * @buffer: the audio buffer
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @do_loop: the LFO's oscillator mode
 * @loop_start: the LFO's frequency
 * @loop_end: the LFO's depth
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_s8(gint8 *buffer,
			   AgsIpatchSample *ipatch_sample,
			   gdouble note,
			   gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames,
			   gboolean do_loop,
			   gint loop_start, gint loop_end)
{
  //TODO:JK: implement me
}

/**
 * ags_sf2_synth_util_copy_s16:
 * @buffer: the audio buffer
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @do_loop: the LFO's oscillator mode
 * @loop_start: the LFO's frequency
 * @loop_end: the LFO's depth
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_s16(gint16 *buffer,
			    AgsIpatchSample *ipatch_sample,
			    gdouble note,
			    gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    gboolean do_loop,
			    gint loop_start, gint loop_end)
{
  //TODO:JK: implement me
}

/**
 * ags_sf2_synth_util_copy_s24:
 * @buffer: the audio buffer
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @do_loop: the LFO's oscillator mode
 * @loop_start: the LFO's frequency
 * @loop_end: the LFO's depth
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_s24(gint32 *buffer,
			    AgsIpatchSample *ipatch_sample,
			    gdouble note,
			    gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    gboolean do_loop,
			    gint loop_start, gint loop_end)
{
  //TODO:JK: implement me
}

/**
 * ags_sf2_synth_util_copy_s32:
 * @buffer: the audio buffer
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @do_loop: the LFO's oscillator mode
 * @loop_start: the LFO's frequency
 * @loop_end: the LFO's depth
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_s32(gint32 *buffer,
			    AgsIpatchSample *ipatch_sample,
			    gdouble note,
			    gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    gboolean do_loop,
			    gint loop_start, gint loop_end)
{
  //TODO:JK: implement me
}

/**
 * ags_sf2_synth_util_copy_s64:
 * @buffer: the audio buffer
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @do_loop: the LFO's oscillator mode
 * @loop_start: the LFO's frequency
 * @loop_end: the LFO's depth
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_s64(gint64 *buffer,
			    AgsIpatchSample *ipatch_sample,
			    gdouble note,
			    gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    gboolean do_loop,
			    gint loop_start, gint loop_end)
{
  //TODO:JK: implement me
}

/**
 * ags_sf2_synth_util_copy_float:
 * @buffer: the audio buffer
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @do_loop: the LFO's oscillator mode
 * @loop_start: the LFO's frequency
 * @loop_end: the LFO's depth
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_float(gfloat *buffer,
			      AgsIpatchSample *ipatch_sample,
			      gdouble note,
			      gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames,
			      gboolean do_loop,
			      gint loop_start, gint loop_end)
{
  //TODO:JK: implement me
}

/**
 * ags_sf2_synth_util_copy_double:
 * @buffer: the audio buffer
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @do_loop: the LFO's oscillator mode
 * @loop_start: the LFO's frequency
 * @loop_end: the LFO's depth
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_double(gdouble *buffer,
			       AgsIpatchSample *ipatch_sample,
			       gdouble note,
			       gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       gboolean do_loop,
			       gint loop_start, gint loop_end)
{
  //TODO:JK: implement me
}

/**
 * ags_sf2_synth_util_copy_complex:
 * @buffer: the audio buffer
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @do_loop: the LFO's oscillator mode
 * @loop_start: the LFO's frequency
 * @loop_end: the LFO's depth
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_complex(AgsComplex *buffer,
				AgsIpatchSample *ipatch_sample,
				gdouble note,
				gdouble volume,
				guint samplerate,
				guint offset, guint n_frames,
				gboolean do_loop,
				gint loop_start, gint loop_end)
{
  //TODO:JK: implement me
}
