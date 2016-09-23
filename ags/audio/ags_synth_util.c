/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2016 Joël Krähemann
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

#include <ags/audio/ags_synth_util.h>

#include <ags/audio/ags_audio_buffer_util.h>

#include <math.h>

/**
 * ags_synth_util_sin_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sinus wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_sin_s8(signed char *buffer,
		      gdouble freq, gdouble phase, gdouble volume,
		      guint samplerate,
		      guint offset, guint n_frames)
{
  static const gdouble scale = 127.0;
  guint i;

  for (i = offset; i < offset + n_frames; i++){
    buffer[i] = (signed char) (0xff & ((signed short) buffer[i] + (signed short) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * scale * volume)));
  }
}

/**
 * ags_synth_util_sin_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sinus wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_sin_s16(signed short *buffer,
		       gdouble freq, gdouble phase, gdouble volume,
		       guint samplerate,
		       guint offset, guint n_frames)
{
  static const gdouble scale = 32767.0;
  guint i;

  for (i = offset; i < offset + n_frames; i++){
    buffer[i] = (signed short) (0xffff & ((signed long) buffer[i] + (signed long) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * scale * volume)));
  }
}

/**
 * ags_synth_util_sin_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sinus wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_sin_s24(signed long *buffer,
		       gdouble freq, gdouble phase, gdouble volume,
		       guint samplerate,
		       guint offset, guint n_frames)
{
  static const gdouble scale = 8388607.0;
  guint i;

  for (i = offset; i < offset + n_frames; i++){
    buffer[i] = (signed long) (0xffffff & ((signed long) buffer[i] + (signed long) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * scale * volume)));
  }
}

/**
 * ags_synth_util_sin_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sinus wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_sin_s32(signed long *buffer,
		       gdouble freq, gdouble phase, gdouble volume,
		       guint samplerate,
		       guint offset, guint n_frames)
{
  static const gdouble scale = 214748363.0;
  guint i;

  for (i = offset; i < offset + n_frames; i++){
    buffer[i] = (signed long) (0xffffffff & ((signed long long) buffer[i] + (signed long long) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * scale * volume)));
  }
}

/**
 * ags_synth_util_sin_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sinus wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_sin_s64(signed long long *buffer,
		       gdouble freq, gdouble phase, gdouble volume,
		       guint samplerate,
		       guint offset, guint n_frames)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  for (i = offset; i < offset + n_frames; i++){
    buffer[i] = (signed long long) (0xffffffffffff & ((signed long long) buffer[i] + (signed long long) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * scale * volume)));
  }
}

/**
 * ags_synth_util_sin_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sinus wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_sin_float(float *buffer,
			 gdouble freq, gdouble phase, gdouble volume,
			 guint samplerate,
			 guint offset, guint n_frames)
{
  guint i;

  for (i = offset; i < offset + n_frames; i++){
    buffer[i] = (float) ((gdouble) buffer[i] + (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * volume));
  }
}

/**
 * ags_synth_util_sin_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sinus wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_sin_double(double *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames)
{
  guint i;

  for (i = offset; i < offset + n_frames; i++){
    buffer[i] = (double) ((gdouble) buffer[i] + (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * volume));
  }
}

/**
 * ags_synth_util_sawtooth_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @volume: the volume of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sawtooth wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_sawtooth_s8(signed char *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  static const gdouble scale = 127.0;
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    buffer[i] = (signed char) (0xff & ((signed short) buffer[i] + (signed short) (((((int) (i + phase) % (int) (samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_sawtooth_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @volume: the volume of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sawtooth wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_sawtooth_s16(signed short *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 32767.0;
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    buffer[i] = (signed short) (0xffff & ((signed long) buffer[i] + (signed long) (((((int) (i + phase) % (int) (samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_sawtooth_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @volume: the volume of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sawtooth wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_sawtooth_s24(signed long *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 8388607.0;
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    buffer[i] = (signed long) (0xffffff & ((signed long) buffer[i] + (signed long) (((((int) (i + phase) % (int) (samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_sawtooth_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @volume: the volume of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sawtooth wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_sawtooth_s32(signed long *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 214748363.0;
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    buffer[i] = (signed long) (0xffffffff & ((signed long long) buffer[i] + (signed long long) (((((int) (i + phase) % (int) (samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_sawtooth_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @volume: the volume of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sawtooth wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_sawtooth_s64(signed long long *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    buffer[i] = (signed long long) (0xffffffffffffffff & ((signed long long) buffer[i] + (signed long long) (((((int) (i + phase) % (int) (samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_sawtooth_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @volume: the volume of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sawtooth wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_sawtooth_float(float *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames)
{
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    buffer[i] = ((double) buffer[i] + (double) (((((int) (i + phase) % (int) (samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * volume));
  }
}

/**
 * ags_synth_util_sawtooth_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @volume: the volume of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sawtooth wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_sawtooth_double(double *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames)
{
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    buffer[i] = ((double) buffer[i] + (double) (((((int) (i + phase) % (int) (samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * volume));
  }
}

/**
 * ags_synth_util_triangle_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @volume: the volume of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate triangle wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_triangle_s8(signed char *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  static const gdouble scale = 127.0;
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    buffer[i] = (signed char) (0xff & ((signed short) buffer[i] + (signed short) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_triangle_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @volume: the volume of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate triangle wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_triangle_s16(signed short *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 32767.0;
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    buffer[i] = (signed short) (0xffff & ((signed long) buffer[i] + (signed long) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_triangle_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @volume: the volume of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate triangle wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_triangle_s24(signed long *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 8388607.0;
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    buffer[i] = (signed long) (0xffffff & ((signed long) buffer[i] + (signed long) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_triangle_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @volume: the volume of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate triangle wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_triangle_s32(signed long *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 214748363.0;
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    buffer[i] = (signed long) (0xffffffff & ((signed long long) buffer[i] + (signed long long) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_triangle_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @volume: the volume of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate triangle wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_triangle_s64(signed long long *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    buffer[i] = (signed long long) (0xffffffffffffffff & ((signed long long) buffer[i] + (signed long long) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
  }
}

/**
 * ags_synth_util_triangle_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @volume: the volume of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate triangle wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_triangle_float(float *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames)
{
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    buffer[i] = ((double) buffer[i] + (double) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * volume));
  }
}

/**
 * ags_synth_util_triangle_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @volume: the volume of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate triangle wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_triangle_double(double *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames)
{
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    buffer[i] = ((double) buffer[i] + (double) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * volume));
  }
}

/**
 * ags_synth_util_square_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @volume: the volume of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate square wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_square_s8(signed char *buffer,
			 gdouble freq, gdouble phase, gdouble volume,
			 guint samplerate,
			 guint offset, guint n_frames)
{
  static const gdouble scale = 127.0;
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    if (sin(i + phase) >= 0.0){
      buffer[i] = (signed char) (0xff & ((signed short) buffer[i] + (signed short) (1.0 * scale * volume)));
    }else{
      buffer[i] = (signed char) (0xff & ((signed short) buffer[i] + (signed short) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_square_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @volume: the volume of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate square wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_square_s16(signed short *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames)
{
  static const gdouble scale = 32767.0;
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    if (sin(i + phase) >= 0.0){
      buffer[i] = (signed short) (0xffff & ((signed long) buffer[i] + (signed long) (1.0 * scale * volume)));
    }else{
      buffer[i] = (signed short) (0xffff & ((signed long) buffer[i] + (signed long) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_square_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @volume: the volume of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate square wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_square_s24(signed long *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames)
{
  static const gdouble scale = 8388607.0;
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    if (sin(i + phase) >= 0.0){
      buffer[i] = (signed long) (0xffffff & ((signed long) buffer[i] + (signed long) (1.0 * scale * volume)));
    }else{
      buffer[i] = (signed long) (0xffffff & ((signed long) buffer[i] + (signed long) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_square_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @volume: the volume of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate square wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_square_s32(signed long *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames)
{
  static const gdouble scale = 214748363.0;
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    if (sin(i + phase) >= 0.0){
      buffer[i] = (signed long) (0xffffffff & ((signed long long) buffer[i] + (signed long long) (1.0 * scale * volume)));
    }else{
      buffer[i] = (signed long) (0xffffffff & ((signed long long) buffer[i] + (signed long long) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_square_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @volume: the volume of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate square wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_square_s64(signed long long *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    if (sin(i + phase) >= 0.0){
      buffer[i] = (signed long long) (0xffffffffffffffff & ((signed long long) buffer[i] + (signed long long) (1.0 * scale * volume)));
    }else{
      buffer[i] = (signed long long) (0xffffffffffffffff & ((signed long long) buffer[i] + (signed long long) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_square_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @volume: the volume of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate square wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_square_float(float *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    if (sin(i + phase) >= 0.0){
      buffer[i] = (buffer[i] + (1.0 * volume));
    }else{
      buffer[i] = (buffer[i] + (-1.0 * volume));
    }
  }
}

/**
 * ags_synth_util_square_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @volume: the volume of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate square wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_square_double(double *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate,
			     guint offset, guint n_frames)
{
  guint i;

  phase = (int) (phase) % (int) (freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < n_frames; i++){
    if (sin(i + phase) >= 0.0){
      buffer[i] = (buffer[i] + (1.0 * volume));
    }else{
      buffer[i] = (buffer[i] + (-1.0 * volume));
    }
  }
}

/**
 * ags_synth_util_sin:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sin wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_sin(void *buffer,
		   gdouble freq, gdouble phase, gdouble volume,
		   guint samplerate, guint audio_buffer_util_format,
		   guint offset, guint n_frames)
{
  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
    {
      ags_synth_util_sin_s8((signed char *) buffer,
			    freq, phase, volume,
			    samplerate,
			    offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
    {
      ags_synth_util_sin_s16((signed short *) buffer,
			     freq, phase, volume,
			     samplerate,
			     offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
    {
      ags_synth_util_sin_s24((signed long *) buffer,
			     freq, phase, volume,
			     samplerate,
			     offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
    {
      ags_synth_util_sin_s32((signed long *) buffer,
			     freq, phase, volume,
			     samplerate,
			     offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
    {
      ags_synth_util_sin_s64((signed long long *) buffer,
			     freq, phase, volume,
			     samplerate,
			     offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
    {
      ags_synth_util_sin_float((float *) buffer,
			       freq, phase, volume,
			       samplerate,
			       offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
    {
      ags_synth_util_sin_double((double *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
    }
    break;
  default:
    {
      g_warning("ags_synth_util_sin() - unsupported format\0");
    }
  }
}

/**
 * ags_synth_util_sawtooth:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @volume: the volume of the sawtooth wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate sawtooth wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_sawtooth(void *buffer,
			gdouble freq, gdouble phase, gdouble volume,
			guint samplerate, guint audio_buffer_util_format,
			guint offset, guint n_frames)
{
  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
    {
      ags_synth_util_sawtooth_s8((signed char *) buffer,
				 freq, phase, volume,
				 samplerate,
				 offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
    {
      ags_synth_util_sawtooth_s16((signed short *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
    {
      ags_synth_util_sawtooth_s24((signed long *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
    {
      ags_synth_util_sawtooth_s32((signed long *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
    {
      ags_synth_util_sawtooth_s64((signed long long *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
    {
      ags_synth_util_sawtooth_float((float *) buffer,
				    freq, phase, volume,
				    samplerate,
				    offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
    {
      ags_synth_util_sawtooth_double((double *) buffer,
				     freq, phase, volume,
				     samplerate,
				     offset, n_frames);
    }
    break;
  default:
    {
      g_warning("ags_synth_util_sawtooth() - unsupported format\0");
    }
  }
}

/**
 * ags_synth_util_triangle:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @volume: the volume of the triangle wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate triangle wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_triangle(void *buffer,
			gdouble freq, gdouble phase, gdouble volume,
			guint samplerate, guint audio_buffer_util_format,
			guint offset, guint n_frames)
{
  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
    {
      ags_synth_util_triangle_s8((signed char *) buffer,
				 freq, phase, volume,
				 samplerate,
				 offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
    {
      ags_synth_util_triangle_s16((signed short *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
    {
      ags_synth_util_triangle_s24((signed long *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
    {
      ags_synth_util_triangle_s32((signed long *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
    {
      ags_synth_util_triangle_s64((signed long long *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
    {
      ags_synth_util_triangle_float((float *) buffer,
				    freq, phase, volume,
				    samplerate,
				    offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
    {
      ags_synth_util_triangle_double((double *) buffer,
				     freq, phase, volume,
				     samplerate,
				     offset, n_frames);
    }
    break;
  default:
    {
      g_warning("ags_synth_util_triangle() - unsupported format\0");
    }
  }
}

/**
 * ags_synth_util_square:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @volume: the volume of the square wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate square wave.
 *
 * Since: 0.7.45
 */
void
ags_synth_util_square(void *buffer,
		      gdouble freq, gdouble phase, gdouble volume,
		      guint samplerate, guint audio_buffer_util_format,
		      guint offset, guint n_frames)
{
  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
    {
      ags_synth_util_square_s8((signed char *) buffer,
			       freq, phase, volume,
			       samplerate,
			       offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
    {
      ags_synth_util_square_s16((signed short *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
    {
      ags_synth_util_square_s24((signed long *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
    {
      ags_synth_util_square_s32((signed long *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
    {
      ags_synth_util_square_s64((signed long long *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
    {
      ags_synth_util_square_float((float *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames);
    }
    break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
    {
      ags_synth_util_square_double((double *) buffer,
				   freq, phase, volume,
				   samplerate,
				   offset, n_frames);
    }
    break;
  default:
    {
      g_warning("ags_synth_util_square() - unsupported format\0");
    }
  }
}
