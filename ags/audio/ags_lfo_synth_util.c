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

#include <ags/audio/ags_lfo_synth_util.h>

#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_fourier_transform_util.h>

#include <math.h>
#include <complex.h>

gpointer ags_lfo_synth_util_strct_copy(gpointer ptr);
void ags_lfo_synth_util_strct_free(gpointer ptr);

/**
 * SECTION:ags_lfo_synth_util
 * @short_description: low frequency oscillator synth util
 * @title: AgsLFOSynthUtil
 * @section_id:
 * @include: ags/audio/ags_lfo_synth_util.h
 *
 * Utility functions to compute LFO synths.
 */

GType
ags_lfo_synth_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_lfo_synth_util = 0;

    ags_type_lfo_synth_util =
      g_boxed_type_register_static("AgsLFOSynthUtil",
				   (GBoxedCopyFunc) ags_lfo_synth_util_strct_copy,
				   (GBoxedFreeFunc) ags_lfo_synth_util_strct_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_lfo_synth_util);
  }

  return g_define_type_id__volatile;
}

gpointer
ags_lfo_synth_util_strct_copy(gpointer ptr)
{
  gpointer retval;

  retval = g_memdup(ptr, sizeof(AgsLFOSynthUtil));
 
  return(retval);
}

void
ags_lfo_synth_util_strct_free(gpointer ptr)
{
  g_free(ptr);
}

/**
 * ags_lfo_synth_util_sin_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @depth: the depth of the sin wave
 * @tuning: the tuning of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using sinus wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_sin_s8(gint8 *buffer,
			  gdouble freq, gdouble phase,
			  gdouble depth,
			  gdouble tuning,
			  guint samplerate,
			  guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    buffer[i] = (gint8) (0xff & (gint16) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * depth)));
  }
}

/**
 * ags_lfo_synth_util_sin_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @depth: the depth of the sin wave
 * @tuning: the tuning of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using sinus wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_sin_s16(gint16 *buffer,
			   gdouble freq, gdouble phase,
			   gdouble depth,
			   gdouble tuning,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    buffer[i] = (gint16) (0xffff & (gint32) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * depth)));
  }
}

/**
 * ags_lfo_synth_util_sin_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @depth: the depth of the sin wave
 * @tuning: the tuning of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using sinus wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_sin_s24(gint32 *buffer,
			   gdouble freq, gdouble phase,
			   gdouble depth,
			   gdouble tuning,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    buffer[i] = (gint32) (0xffffffff & (gint32) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * depth)));
  }
}

/**
 * ags_lfo_synth_util_sin_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @depth: the depth of the sin wave
 * @tuning: the tuning of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using sinus wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_sin_s32(gint32 *buffer,
			   gdouble freq, gdouble phase,
			   gdouble depth,
			   gdouble tuning,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    buffer[i] = (gint32) (0xffffffff & (gint64) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * depth)));
  }
}

/**
 * ags_lfo_synth_util_sin_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @depth: the depth of the sin wave
 * @tuning: the tuning of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using sinus wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_sin_s64(gint64 *buffer,
			   gdouble freq, gdouble phase,
			   gdouble depth,
			   gdouble tuning,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    buffer[i] = (gint64) (0xffffffffffff & (gint64) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * depth)));
  }
}

/**
 * ags_lfo_synth_util_sin_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @depth: the depth of the sin wave
 * @tuning: the tuning of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using sinus wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_sin_float(gfloat *buffer,
			     gdouble freq, gdouble phase,
			     gdouble depth,
			     gdouble tuning,
			     guint samplerate,
			     guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    buffer[i] = (float) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * depth));
  }
}

/**
 * ags_lfo_synth_util_sin_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @depth: the depth of the sin wave
 * @tuning: the tuning of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using sinus wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_sin_double(gdouble *buffer,
			      gdouble freq, gdouble phase,
			      gdouble depth,
			      gdouble tuning,
			      guint samplerate,
			      guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    buffer[i] = (double) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * depth));
  }
}

/**
 * ags_lfo_synth_util_sin_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @depth: the depth of the sin wave
 * @tuning: the tuning of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using sinus wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_sin_complex(AgsComplex *buffer,
			       gdouble freq, gdouble phase,
			       gdouble depth,
			       gdouble tuning,
			       guint samplerate,
			       guint offset, guint n_frames)
{
  AgsComplex c;
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  gdouble *y_ptr;
  gdouble y;
  double _Complex z0, z1;

  guint i_stop;
  guint i;

  if(buffer == NULL){
    return;
  }

  c_ptr = &c;
  c_ptr_ptr = &c_ptr;
  
  y_ptr = &y;

  i_stop = n_frames;

  for(i = 0; i < i_stop; i++){
    y = (gdouble) (tuning / 1200.0 + sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * depth);

    AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_DOUBLE_FRAME(y_ptr, 1,
							 i - offset, i_stop - offset,
							 c_ptr_ptr);

    z0 = ags_complex_get(buffer + i);
    z1 = ags_complex_get(c_ptr);

    ags_complex_set(buffer + i,
		    z0 + z1);
  }
}

/**
 * ags_lfo_synth_util_sawtooth_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @depth: the depth of the sawtooth wave
 * @tuning: the tuning of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_sawtooth_s8(gint8 *buffer,
			       gdouble freq, gdouble phase,
			       gdouble depth,
			       gdouble tuning,
			       guint samplerate,
			       guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = 0; i < n_frames; i++){
    buffer[i] = (gint8) (0xff & (gint16) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + ((((int) ceil(offset + i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * depth)));
  }
}

/**
 * ags_lfo_synth_util_sawtooth_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @depth: the depth of the sawtooth wave
 * @tuning: the tuning of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_sawtooth_s16(gint16 *buffer,
				gdouble freq, gdouble phase,
				gdouble depth,
				gdouble tuning,
				guint samplerate,
				guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = 0; i < n_frames; i++){
    buffer[i] = (gint16) (0xffff & (gint32) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + ((((int) ceil(offset + i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * depth)));
  }
}

/**
 * ags_lfo_synth_util_sawtooth_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @depth: the depth of the sawtooth wave
 * @tuning: the tuning of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_sawtooth_s24(gint32 *buffer,
				gdouble freq, gdouble phase,
				gdouble depth,
				gdouble tuning,
				guint samplerate,
				guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = 0; i < n_frames; i++){
    buffer[i] = (gint32) (0xffffffff & (gint32) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + ((((int) ceil(offset + i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * depth)));
  }
}

/**
 * ags_lfo_synth_util_sawtooth_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @depth: the depth of the sawtooth wave
 * @tuning: the tuning of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_sawtooth_s32(gint32 *buffer,
				gdouble freq, gdouble phase,
				gdouble depth,
				gdouble tuning,
				guint samplerate,
				guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = 0; i < n_frames; i++){
    buffer[i] = (gint32) (0xffffffff & (gint64) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + ((((int) ceil(offset + i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * depth)));
  }
}

/**
 * ags_lfo_synth_util_sawtooth_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @depth: the depth of the sawtooth wave
 * @tuning: the tuning of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_sawtooth_s64(gint64 *buffer,
				gdouble freq, gdouble phase,
				gdouble depth,
				gdouble tuning,
				guint samplerate,
				guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = 0; i < n_frames; i++){
    buffer[i] = (gint64) (0xffffffffffffffff & (gint64) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + ((((int) ceil(offset + i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * depth)));
  }
}

/**
 * ags_lfo_synth_util_sawtooth_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @depth: the depth of the sawtooth wave
 * @tuning: the tuning of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_sawtooth_float(gfloat *buffer,
				  gdouble freq, gdouble phase,
				  gdouble depth,
				  gdouble tuning,
				  guint samplerate,
				  guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = 0; i < n_frames; i++){
    buffer[i] = ((double) buffer[i] * (double) (tuning / 1200.0 + ((((int) ceil(offset + i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0)));
  }
}

/**
 * ags_lfo_synth_util_sawtooth_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @depth: the depth of the sawtooth wave
 * @tuning: the tuning of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_sawtooth_double(gdouble *buffer,
				   gdouble freq, gdouble phase,
				   gdouble depth,
				   gdouble tuning,
				   guint samplerate,
				   guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = 0; i < n_frames; i++){
    buffer[i] = ((double) buffer[i] * (double) (tuning / 1200.0 + ((((int) ceil(offset + i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0)));
  }
}

/**
 * ags_lfo_synth_util_sawtooth_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @depth: the depth of the sawtooth wave
 * @tuning: the tuning of the sawtooth wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_sawtooth_complex(AgsComplex *buffer,
				    gdouble freq, gdouble phase,
				    gdouble depth,
				    gdouble tuning,
				    guint samplerate,
				    guint offset, guint n_frames)
{
  AgsComplex c;
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  gdouble *y_ptr;
  gdouble y;
  double _Complex z0, z1;

  guint i_stop;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  c_ptr = &c;
  c_ptr_ptr = &c_ptr;
  
  y_ptr = &y;

  i_stop = n_frames;

  for(i = 0; i < n_frames; i++){
    y = (double) (tuning / 1200.0 + ((((int) ceil(offset + i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0));

    AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_DOUBLE_FRAME(y_ptr, 1,
							 i - offset, i_stop - offset,
							 c_ptr_ptr);

    z0 = ags_complex_get(buffer + i);
    z1 = ags_complex_get(c_ptr);

    ags_complex_set(buffer + i,
		    z0 + z1);
  }
}

/**
 * ags_lfo_synth_util_triangle_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @depth: the depth of the triangle wave
 * @tuning: the tuning of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_triangle_s8(gint8 *buffer,
			       gdouble freq, gdouble phase,
			       gdouble depth,
			       gdouble tuning,
			       guint samplerate,
			       guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = 0; i < n_frames; i++){
    buffer[i] = (gint8) (0xff & (gint16) ((gdouble) buffer[i] * (gdouble) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * depth)));
  }
}

/**
 * ags_lfo_synth_util_triangle_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @depth: the depth of the triangle wave
 * @tuning: the tuning of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_triangle_s16(gint16 *buffer,
				gdouble freq, gdouble phase,
				gdouble depth,
				gdouble tuning,
				guint samplerate,
				guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = 0; i < n_frames; i++){
    buffer[i] = (gint16) (0xffff & (gint32) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + (((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * depth)));
  }
}

/**
 * ags_lfo_synth_util_triangle_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @depth: the depth of the triangle wave
 * @tuning: the tuning of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_triangle_s24(gint32 *buffer,
				gdouble freq, gdouble phase,
				gdouble depth,
				gdouble tuning,
				guint samplerate,
				guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = 0; i < n_frames; i++){
    buffer[i] = (gint32) (0xffffffff & (gint32) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + (((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * depth)));
  }
}

/**
 * ags_lfo_synth_util_triangle_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @depth: the depth of the triangle wave
 * @tuning: the tuning of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_triangle_s32(gint32 *buffer,
				gdouble freq, gdouble phase,
				gdouble depth,
				gdouble tuning,
				guint samplerate,
				guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = 0; i < n_frames; i++){
    buffer[i] = (gint32) (0xffffffff & (gint64) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + (((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * depth)));
  }
}

/**
 * ags_lfo_synth_util_triangle_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @depth: the depth of the triangle wave
 * @tuning: the tuning of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_triangle_s64(gint64 *buffer,
				gdouble freq, gdouble phase,
				gdouble depth,
				gdouble tuning,
				guint samplerate,
				guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = 0; i < n_frames; i++){
    buffer[i] = (gint64) (0xffffffffffffffff & (gint64) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + (((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * depth)));
  }
}

/**
 * ags_lfo_synth_util_triangle_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @depth: the depth of the triangle wave
 * @tuning: the tuning of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_triangle_float(gfloat *buffer,
				  gdouble freq, gdouble phase,
				  gdouble depth,
				  gdouble tuning,
				  guint samplerate,
				  guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = 0; i < n_frames; i++){
    buffer[i] = ((double) buffer[i] * (double) (tuning / 1200.0 + (((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0)));
  }
}

/**
 * ags_lfo_synth_util_triangle_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @depth: the depth of the triangle wave
 * @tuning: the tuning of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_triangle_double(gdouble *buffer,
				   gdouble freq, gdouble phase,
				   gdouble depth,
				   gdouble tuning,
				   guint samplerate,
				   guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = 0; i < n_frames; i++){
    buffer[i] = ((double) buffer[i] * (double) (tuning / 1200.0 + (((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0)));
  }
}

/**
 * ags_lfo_synth_util_triangle_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @depth: the depth of the triangle wave
 * @tuning: the tuning of the triangle wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_triangle_complex(AgsComplex *buffer,
				    gdouble freq, gdouble phase,
				    gdouble depth,
				    gdouble tuning,
				    guint samplerate,
				    guint offset, guint n_frames)
{
  AgsComplex c;
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  gdouble *y_ptr;
  gdouble y;
  double _Complex z0, z1;

  guint i_stop;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  c_ptr = &c;
  c_ptr_ptr = &c_ptr;
  
  y_ptr = &y;

  i_stop = n_frames;

  for(i = 0; i < n_frames; i++){
    y = (double) (tuning / 1200.0 + (((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0));

    AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_DOUBLE_FRAME(y_ptr, 1,
							 i - offset, i_stop - offset,
							 c_ptr_ptr);

    z0 = ags_complex_get(buffer + i);
    z1 = ags_complex_get(c_ptr);

    ags_complex_set(buffer + i,
		    z0 + z1);
  }
}

/**
 * ags_lfo_synth_util_square_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @depth: the depth of the square wave
 * @tuning: the tuning of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using square wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_square_s8(gint8 *buffer,
			     gdouble freq, gdouble phase,
			     gdouble depth,
			     gdouble tuning,
			     guint samplerate,
			     guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    if(sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (gint8) (0xff & (gint16) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + 1.0 * depth)));
    }else{
      buffer[i] = (gint8) (0xff & (gint16) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + -1.0 * depth)));
    }
  }
}

/**
 * ags_lfo_synth_util_square_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @depth: the depth of the square wave
 * @tuning: the tuning of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using square wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_square_s16(gint16 *buffer,
			      gdouble freq, gdouble phase,
			      gdouble depth,
			      gdouble tuning,
			      guint samplerate,
			      guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    if(sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (gint16) (0xffff & (gint32) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + 1.0 * depth)));
    }else{
      buffer[i] = (gint16) (0xffff & (gint32) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + -1.0 * depth)));
    }
  }
}

/**
 * ags_lfo_synth_util_square_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @depth: the depth of the square wave
 * @tuning: the tuning of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using square wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_square_s24(gint32 *buffer,
			      gdouble freq, gdouble phase,
			      gdouble depth,
			      gdouble tuning,
			      guint samplerate,
			      guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    if(sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (gint32) (0xffffffff & (gint32) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + 1.0 * depth)));
    }else{
      buffer[i] = (gint32) (0xffffffff & (gint32) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + -1.0 * depth)));
    }
  }
}

/**
 * ags_lfo_synth_util_square_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @depth: the depth of the square wave
 * @tuning: the tuning of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using square wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_square_s32(gint32 *buffer,
			      gdouble freq, gdouble phase,
			      gdouble depth,
			      gdouble tuning,
			      guint samplerate,
			      guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    if(sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (gint32) (0xffffffff & (gint64) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + 1.0 * depth)));
    }else{
      buffer[i] = (gint32) (0xffffffff & (gint64) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + -1.0 * depth)));
    }
  }
}

/**
 * ags_lfo_synth_util_square_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @depth: the depth of the square wave
 * @tuning: the tuning of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using square wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_square_s64(gint64 *buffer,
			      gdouble freq, gdouble phase,
			      gdouble depth,
			      gdouble tuning,
			      guint samplerate,
			      guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    if(sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (gint64) (0xffffffffffffffff & (gint64) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + 1.0 * depth)));
    }else{
      buffer[i] = (gint64) (0xffffffffffffffff & (gint64) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + -1.0 * depth)));
    }
  }
}

/**
 * ags_lfo_synth_util_square_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @depth: the depth of the square wave
 * @tuning: the tuning of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using square wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_square_float(gfloat *buffer,
				gdouble freq, gdouble phase,
				gdouble depth,
				gdouble tuning,
				guint samplerate,
				guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    if(sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (buffer[i] * (tuning / 1200.0 + 1.0 * depth));
    }else{
      buffer[i] = (buffer[i] * (tuning / 1200.0 + -1.0 * depth));
    }
  }
}

/**
 * ags_lfo_synth_util_square_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @depth: the depth of the square wave
 * @tuning: the tuning of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using square wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_square_double(gdouble *buffer,
				 gdouble freq, gdouble phase,
				 gdouble depth,
				 gdouble tuning,
				 guint samplerate,
				 guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    if(sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (buffer[i] * (tuning / 1200.0 + 1.0 * depth));
    }else{
      buffer[i] = (buffer[i] * (tuning / 1200.0 + -1.0 * depth));
    }
  }
}

/**
 * ags_lfo_synth_util_square_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @depth: the depth of the square wave
 * @tuning: the tuning of the square wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using square wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_square_complex(AgsComplex *buffer,
				  gdouble freq, gdouble phase,
				  gdouble depth,
				  gdouble tuning,
				  guint samplerate,
				  guint offset, guint n_frames)
{
  AgsComplex c;
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  gdouble *y_ptr;
  gdouble y;
  double _Complex z0, z1;

  guint i_stop;
  guint i;

  if(buffer == NULL){
    return;
  }

  c_ptr = &c;
  c_ptr_ptr = &c_ptr;
  
  y_ptr = &y;

  i_stop = n_frames;

  for(i = 0; i < i_stop; i++){
    if(sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      y = (tuning / 1200.0 + 1.0 * depth);
    }else{
      y = (tuning / 1200.0 + -1.0 * depth);
    }

    AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_DOUBLE_FRAME(y_ptr, 1,
							 i - offset, i_stop - offset,
							 c_ptr_ptr);

    z0 = ags_complex_get(buffer + i);
    z1 = ags_complex_get(c_ptr);

    ags_complex_set(buffer + i,
		    z0 + z1);
  }
}

/**
 * ags_lfo_synth_util_impulse_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @depth: the depth of the impulse wave
 * @tuning: the tuning of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_impulse_s8(gint8 *buffer,
			      gdouble freq, gdouble phase,
			      gdouble depth,
			      gdouble tuning,
			      guint samplerate,
			      guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    if(sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (gint8) (0xff & (gint16) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + 1.0 * depth)));
    }else{
      buffer[i] = (gint8) (0xff & (gint16) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + -1.0 * depth)));
    }
  }
}

/**
 * ags_lfo_synth_util_impulse_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @depth: the depth of the impulse wave
 * @tuning: the tuning of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_impulse_s16(gint16 *buffer,
			       gdouble freq, gdouble phase,
			       gdouble depth,
			       gdouble tuning,
			       guint samplerate,
			       guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    if(sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (gint16) (0xffff & (gint32) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + 1.0 * depth)));
    }else{
      buffer[i] = (gint16) (0xffff & (gint32) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + -1.0 * depth)));
    }
  }
}

/**
 * ags_lfo_synth_util_impulse_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @depth: the depth of the impulse wave
 * @tuning: the tuning of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_impulse_s24(gint32 *buffer,
			       gdouble freq, gdouble phase,
			       gdouble depth,
			       gdouble tuning,
			       guint samplerate,
			       guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    if(sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (gint32) (0xffffffff & (gint32) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + 1.0 * depth)));
    }else{
      buffer[i] = (gint32) (0xffffffff & (gint32) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + -1.0 * depth)));
    }
  }
}

/**
 * ags_lfo_synth_util_impulse_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @depth: the depth of the impulse wave
 * @tuning: the tuning of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_impulse_s32(gint32 *buffer,
			       gdouble freq, gdouble phase,
			       gdouble depth,
			       gdouble tuning,
			       guint samplerate,
			       guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    if(sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (gint32) (0xffffffff & (gint64) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + 1.0 * depth)));
    }else{
      buffer[i] = (gint32) (0xffffffff & (gint64) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + -1.0 * depth)));
    }
  }
}

/**
 * ags_lfo_synth_util_impulse_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @depth: the depth of the impulse wave
 * @tuning: the tuning of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_impulse_s64(gint64 *buffer,
			       gdouble freq, gdouble phase,
			       gdouble depth,
			       gdouble tuning,
			       guint samplerate,
			       guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    if(sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (gint64) (0xffffffffffffffff & (gint64) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + 1.0 * depth)));
    }else{
      buffer[i] = (gint64) (0xffffffffffffffff & (gint64) ((gdouble) buffer[i] * (gdouble) (tuning / 1200.0 + -1.0 * depth)));
    }
  }
}

/**
 * ags_lfo_synth_util_impulse_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @depth: the depth of the impulse wave
 * @tuning: the tuning of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_impulse_float(gfloat *buffer,
				 gdouble freq, gdouble phase,
				 gdouble depth,
				 gdouble tuning,
				 guint samplerate,
				 guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    if(sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (buffer[i] * (tuning / 1200.0 + 1.0 * depth));
    }else{
      buffer[i] = (buffer[i] * (tuning / 1200.0 + -1.0 * depth));
    }
  }
}

/**
 * ags_lfo_synth_util_impulse_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @depth: the depth of the impulse wave
 * @tuning: the tuning of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_impulse_double(gdouble *buffer,
				  gdouble freq, gdouble phase,
				  gdouble depth,
				  gdouble tuning,
				  guint samplerate,
				  guint offset, guint n_frames)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  for(i = 0; i < n_frames; i++){
    if(sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (buffer[i] * (tuning / 1200.0 + 1.0 * depth));
    }else{
      buffer[i] = (buffer[i] * (tuning / 1200.0 + -1.0 * depth));
    }
  }
}

/**
 * ags_lfo_synth_util_impulse_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @depth: the depth of the impulse wave
 * @tuning: the tuning of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_impulse_complex(AgsComplex *buffer,
				   gdouble freq, gdouble phase,
				   gdouble depth,
				   gdouble tuning,
				   guint samplerate,
				   guint offset, guint n_frames)
{
  AgsComplex c;
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  gdouble *y_ptr;
  gdouble y;
  double _Complex z0, z1;

  guint i_stop;
  guint i;

  if(buffer == NULL){
    return;
  }

  c_ptr = &c;
  c_ptr_ptr = &c_ptr;
  
  y_ptr = &y;

  i_stop = n_frames;

  for(i = 0; i < i_stop; i++){
    if(sin((gdouble) (offset + i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      y = (tuning / 1200.0 + 1.0 * depth);
    }else{
      y = (tuning / 1200.0 + -1.0 * depth);
    }

    AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_DOUBLE_FRAME(y_ptr, 1,
							 i - offset, i_stop - offset,
							 c_ptr_ptr);

    z0 = ags_complex_get(buffer + i);
    z1 = ags_complex_get(c_ptr);

    ags_complex_set(buffer + i,
		    z0 + z1);
  }
}

/**
 * ags_lfo_synth_util_sin:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @depth: the depth of the sin wave
 * @tuning: the tuning of the sin wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using sin wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_sin(void *buffer,
		       gdouble freq, gdouble phase,
		       gdouble depth,
		       gdouble tuning,
		       guint samplerate, guint audio_buffer_util_format,
		       guint offset, guint n_frames)
{
  if(buffer == NULL){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_lfo_synth_util_sin_s8((gint8 *) buffer,
			      freq, phase,
			      depth,
			      tuning,
			      samplerate,
			      offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_lfo_synth_util_sin_s16((gint16 *) buffer,
			       freq, phase,
			       depth,
			       tuning,
			       samplerate,
			       offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_lfo_synth_util_sin_s24((gint32 *) buffer,
			       freq, phase,
			       depth,
			       tuning,
			       samplerate,
			       offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_lfo_synth_util_sin_s32((gint32 *) buffer,
			       freq, phase,
			       depth,
			       tuning,
			       samplerate,
			       offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_lfo_synth_util_sin_s64((gint64 *) buffer,
			       freq, phase,
			       depth,
			       tuning,
			       samplerate,
			       offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_lfo_synth_util_sin_float((gfloat *) buffer,
				 freq, phase,
				 depth,
				 tuning,
				 samplerate,
				 offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_lfo_synth_util_sin_double((gdouble *) buffer,
				  freq, phase,
				  depth,
				  tuning,
				  samplerate,
				  offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_lfo_synth_util_sin_complex((AgsComplex *) buffer,
				   freq, phase,
				   depth,
				   tuning,
				   samplerate,
				   offset, n_frames);
  }
  break;
  default:
  {
    g_warning("ags_lfo_synth_util_sin() - unsupported format");
  }
  }
}

/**
 * ags_lfo_synth_util_sawtooth:
 * @buffer: the audio buffer
 * @freq: the frequency of the sawtooth wave
 * @phase: the phase of the sawtooth wave
 * @depth: the depth of the sawtooth wave
 * @tuning: the tuning of the sawtooth wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_sawtooth(void *buffer,
			    gdouble freq, gdouble phase,
			    gdouble depth,
			    gdouble tuning,
			    guint samplerate, guint audio_buffer_util_format,
			    guint offset, guint n_frames)
{
  if(buffer == NULL){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_lfo_synth_util_sawtooth_s8((gint8 *) buffer,
				   freq, phase,
				   depth,
				   tuning,
				   samplerate,
				   offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_lfo_synth_util_sawtooth_s16((gint16 *) buffer,
				    freq, phase,
				    depth,
				    tuning,
				    samplerate,
				    offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_lfo_synth_util_sawtooth_s24((gint32 *) buffer,
				    freq, phase,
				    depth,
				    tuning,
				    samplerate,
				    offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_lfo_synth_util_sawtooth_s32((gint32 *) buffer,
				    freq, phase,
				    depth,
				    tuning,
				    samplerate,
				    offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_lfo_synth_util_sawtooth_s64((gint64 *) buffer,
				    freq, phase,
				    depth,
				    tuning,
				    samplerate,
				    offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_lfo_synth_util_sawtooth_float((gfloat *) buffer,
				      freq, phase,
				      depth,
				      tuning,
				      samplerate,
				      offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_lfo_synth_util_sawtooth_double((gdouble *) buffer,
				       freq, phase,
				       depth,
				       tuning,
				       samplerate,
				       offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_lfo_synth_util_sawtooth_complex((AgsComplex *) buffer,
					freq, phase,
					depth,
					tuning,
					samplerate,
					offset, n_frames);
  }
  break;
  default:
  {
    g_warning("ags_lfo_synth_util_sawtooth() - unsupported format");
  }
  }
}

/**
 * ags_lfo_synth_util_triangle:
 * @buffer: the audio buffer
 * @freq: the frequency of the triangle wave
 * @phase: the phase of the triangle wave
 * @depth: the depth of the triangle wave
 * @tuning: the tuning of the triangle wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_triangle(void *buffer,
			    gdouble freq, gdouble phase,
			    gdouble depth,
			    gdouble tuning,
			    guint samplerate, guint audio_buffer_util_format,
			    guint offset, guint n_frames)
{
  if(buffer == NULL){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_lfo_synth_util_triangle_s8((gint8 *) buffer,
				   freq, phase,
				   depth,
				   tuning,
				   samplerate,
				   offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_lfo_synth_util_triangle_s16((gint16 *) buffer,
				    freq, phase,
				    depth,
				    tuning,
				    samplerate,
				    offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_lfo_synth_util_triangle_s24((gint32 *) buffer,
				    freq, phase,
				    depth,
				    tuning,
				    samplerate,
				    offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_lfo_synth_util_triangle_s32((gint32 *) buffer,
				    freq, phase,
				    depth,
				    tuning,
				    samplerate,
				    offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_lfo_synth_util_triangle_s64((gint64 *) buffer,
				    freq, phase,
				    depth,
				    tuning,
				    samplerate,
				    offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_lfo_synth_util_triangle_float((gfloat *) buffer,
				      freq, phase,
				      depth,
				      tuning,
				      samplerate,
				      offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_lfo_synth_util_triangle_double((gdouble *) buffer,
				       freq, phase,
				       depth,
				       tuning,
				       samplerate,
				       offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_lfo_synth_util_triangle_complex((AgsComplex *) buffer,
					freq, phase,
					depth,
					tuning,
					samplerate,
					offset, n_frames);
  }
  break;
  default:
  {
    g_warning("ags_lfo_synth_util_triangle() - unsupported format");
  }
  }
}

/**
 * ags_lfo_synth_util_square:
 * @buffer: the audio buffer
 * @freq: the frequency of the square wave
 * @phase: the phase of the square wave
 * @depth: the depth of the square wave
 * @tuning: the tuning of the square wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using square wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_square(void *buffer,
			  gdouble freq, gdouble phase,
			  gdouble depth,
			  gdouble tuning,
			  guint samplerate, guint audio_buffer_util_format,
			  guint offset, guint n_frames)
{
  if(buffer == NULL){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_lfo_synth_util_square_s8((gint8 *) buffer,
				 freq, phase,
				 depth,
				 tuning,
				 samplerate,
				 offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_lfo_synth_util_square_s16((gint16 *) buffer,
				  freq, phase,
				  depth,
				  tuning,
				  samplerate,
				  offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_lfo_synth_util_square_s24((gint32 *) buffer,
				  freq, phase,
				  depth,
				  tuning,
				  samplerate,
				  offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_lfo_synth_util_square_s32((gint32 *) buffer,
				  freq, phase,
				  depth,
				  tuning,
				  samplerate,
				  offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_lfo_synth_util_square_s64((gint64 *) buffer,
				  freq, phase,
				  depth,
				  tuning,
				  samplerate,
				  offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_lfo_synth_util_square_float((gfloat *) buffer,
				    freq, phase,
				    depth,
				    tuning,
				    samplerate,
				    offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_lfo_synth_util_square_double((gdouble *) buffer,
				     freq, phase,
				     depth,
				     tuning,
				     samplerate,
				     offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_lfo_synth_util_square_complex((AgsComplex *) buffer,
				      freq, phase,
				      depth,
				      tuning,
				      samplerate,
				      offset, n_frames);
  }
  break;
  default:
  {
    g_warning("ags_lfo_synth_util_square() - unsupported format");
  }
  }
}

/**
 * ags_lfo_synth_util_impulse:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @depth: the depth of the impulse wave
 * @tuning: the tuning of the impulse wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * LFO using impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_lfo_synth_util_impulse(void *buffer,
			   gdouble freq, gdouble phase,
			   gdouble depth,
			   gdouble tuning,
			   guint samplerate, guint audio_buffer_util_format,
			   guint offset, guint n_frames)
{
  if(buffer == NULL){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_lfo_synth_util_impulse_s8((gint8 *) buffer,
				  freq, phase,
				  depth,
				  tuning,
				  samplerate,
				  offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_lfo_synth_util_impulse_s16((gint16 *) buffer,
				   freq, phase,
				   depth,
				   tuning,
				   samplerate,
				   offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_lfo_synth_util_impulse_s24((gint32 *) buffer,
				   freq, phase,
				   depth,
				   tuning,
				   samplerate,
				   offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_lfo_synth_util_impulse_s32((gint32 *) buffer,
				   freq, phase,
				   depth,
				   tuning,
				   samplerate,
				   offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_lfo_synth_util_impulse_s64((gint64 *) buffer,
				   freq, phase,
				   depth,
				   tuning,
				   samplerate,
				   offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_lfo_synth_util_impulse_float((gfloat *) buffer,
				     freq, phase,
				     depth,
				     tuning,
				     samplerate,
				     offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_lfo_synth_util_impulse_double((gdouble *) buffer,
				      freq, phase,
				      depth,
				      tuning,
				      samplerate,
				      offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_lfo_synth_util_impulse_complex((AgsComplex *) buffer,
				       freq, phase,
				       depth,
				       tuning,
				       samplerate,
				       offset, n_frames);
  }
  break;
  default:
  {
    g_warning("ags_lfo_synth_util_impulse() - unsupported format");
  }
  }
}
