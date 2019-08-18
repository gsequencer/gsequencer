/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/ags_fm_synth_util.h>

#include <ags/audio/ags_audio_buffer_util.h>

#include <math.h>

/**
 * ags_fm_synth_util_sin_S8:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate sin wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_sin_s8(gint8 *buffer,
			 gdouble freq, gdouble phase, gdouble volume,
			 guint samplerate,
			 guint offset, guint n_frames,
			 gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_sin_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate sin wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_sin_s16(gint16 *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames,
			  gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_sin_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate sin wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_sin_s24(gint32 *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames,
			  gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_sin_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate sin wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_sin_s32(gint32 *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames,
			  gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_sin_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate sin wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_sin_s64(gint64 *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames,
			  gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_sin_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate sin wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_sin_float(float *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_sin_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate sin wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_sin_double(double *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate,
			     guint offset, guint n_frames,
			     gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_sin_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate sin wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_sin_complex(AgsComplex *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames,
			      gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_sawtooth_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate sawtooth wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_sawtooth_s8(gint8 *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames,
			      gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_sawtooth_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate sawtooth wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_sawtooth_s16(gint16 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_sawtooth_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate sawtooth wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_sawtooth_s24(gint32 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_sawtooth_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate sawtooth wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_sawtooth_s32(gint32 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_sawtooth_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate sawtooth wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_sawtooth_s64(gint64 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_sawtooth_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate sawtooth wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_sawtooth_float(float *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames,
				 gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_sawtooth_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate sawtooth wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_sawtooth_double(double *buffer,
				  gdouble freq, gdouble phase, gdouble volume,
				  guint samplerate,
				  guint offset, guint n_frames,
				  gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_sawtooth_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate sawtooth wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_sawtooth_complex(AgsComplex *buffer,
				   gdouble freq, gdouble phase, gdouble volume,
				   guint samplerate,
				   guint offset, guint n_frames,
				   gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_triangle_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate triangle wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_triangle_s8(gint8 *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames,
			      gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_triangle_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate triangle wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_triangle_s16(gint16 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_triangle_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate triangle wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_triangle_s24(gint32 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_triangle_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate triangle wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_triangle_s32(gint32 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_triangle_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate triangle wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_triangle_s64(gint64 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_triangle_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate triangle wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_triangle_float(float *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames,
				 gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_triangle_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate triangle wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_triangle_double(double *buffer,
				  gdouble freq, gdouble phase, gdouble volume,
				  guint samplerate,
				  guint offset, guint n_frames,
				  gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_triangle_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate triangle wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_triangle_complex(AgsComplex *buffer,
				   gdouble freq, gdouble phase, gdouble volume,
				   guint samplerate,
				   guint offset, guint n_frames,
				   gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_square_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate square wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_square_s8(gint8 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_square_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate square wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_square_s16(gint16 *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate,
			     guint offset, guint n_frames,
			     gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_square_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate square wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_square_s24(gint32 *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate,
			     guint offset, guint n_frames,
			     gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_square_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate square wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_square_s32(gint32 *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate,
			     guint offset, guint n_frames,
			     gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_square_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate square wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_square_s64(gint64 *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate,
			     guint offset, guint n_frames,
			     gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_square_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate square wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_square_float(float *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_square_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate square wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_square_double(double *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames,
				gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_square_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate square wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_square_complex(AgsComplex *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames,
				 gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_impulse_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate impulse wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_impulse_s8(gint8 *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate,
			     guint offset, guint n_frames,
			     gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_impulse_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate impulse wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_impulse_s16(gint16 *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames,
			      gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_impulse_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate impulse wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_impulse_s24(gint32 *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames,
			      gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_impulse_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate impulse wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_impulse_s32(gint32 *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames,
			      gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_impulse_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate impulse wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_impulse_s64(gint64 *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames,
			      gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_impulse_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate impulse wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_impulse_float(float *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames,
				gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_impulse_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate impulse wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_impulse_double(double *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames,
				 gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_impulse_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 * 
 * Generate frequency modulate impulse wave.
 * 
 * Since: 2.3.0
 */
void
ags_fm_synth_util_impulse_complex(AgsComplex *buffer,
				  gdouble freq, gdouble phase, gdouble volume,
				  guint samplerate,
				  guint offset, guint n_frames,
				  gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_sin:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 *
 * Generate frequency modulate sin wave.
 *
 * Since: 2.0.0
 */
void
ags_fm_synth_util_sin(void *buffer,
		      gdouble freq, gdouble phase, gdouble volume,
		      guint samplerate, guint audio_buffer_util_format,
		      guint offset, guint n_frames,
		      gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}


/**
 * ags_fm_synth_util_sawtooth:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 *
 * Generate frequency modulate sawtooth wave.
 *
 * Since: 2.0.0
 */
void
ags_fm_synth_util_sawtooth(void *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate, guint audio_buffer_util_format,
			   guint offset, guint n_frames,
			   gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_triangle:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 *
 * Generate frequency modulate triangle wave.
 *
 * Since: 2.0.0
 */
void
ags_fm_synth_util_triangle(void *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate, guint audio_buffer_util_format,
			   guint offset, guint n_frames,
			   gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_square:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 *
 * Generate frequency modulate square wave.
 *
 * Since: 2.0.0
 */
void
ags_fm_synth_util_square(void *buffer,
			 gdouble freq, gdouble phase, gdouble volume,
			 guint samplerate, guint audio_buffer_util_format,
			 guint offset, guint n_frames,
			 gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}

/**
 * ags_fm_synth_util_impulse:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_frequency: the LFO's frequency
 * @depth: the LFO's depth
 *
 * Generate frequency modulate impulse wave.
 *
 * Since: 2.0.0
 */
void
ags_fm_synth_util_impulse(void *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate, guint audio_buffer_util_format,
			  guint offset, guint n_frames,
			  gdouble lfo_frequency, gdouble depth)
{
  //TODO:JK: implement me
}
