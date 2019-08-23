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

#include <ags/audio/ags_synth_util.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_fourier_transform_util.h>

#include <math.h>
#include <complex.h>

/**
 * ags_synth_util_get_xcross_count_s8:
 * @buffer: the buffer containing audio data
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 2.0.0
 */
guint
ags_synth_util_get_xcross_count_s8(gint8 *buffer,
				   guint buffer_size)
{
  guint count;
  guint i;
  gboolean negative;
  
  count = 0;

  if(buffer[0] > 0){
    negative = FALSE;
  }else{
    negative = TRUE;
  }

  for(i = 1; i < buffer_size; i++){
    if(negative &&
       buffer[i] > 0){
      count++;

      negative = FALSE;
    }else if(!negative &&
	     buffer[i] < 0){
      count++;

      negative = TRUE;
    }
  }
  
  return(count);
}

/**
 * ags_synth_util_get_xcross_count_s16:
 * @buffer: the buffer containing audio data
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 2.0.0
 */
guint
ags_synth_util_get_xcross_count_s16(gint16 *buffer,
				    guint buffer_size)
{
  guint count;
  guint i;
  gboolean negative;
  
  count = 0;

  if(buffer[0] > 0){
    negative = FALSE;
  }else{
    negative = TRUE;
  }

  for(i = 1; i < buffer_size; i++){
    if(negative &&
       buffer[i] > 0){
      count++;

      negative = FALSE;
    }else if(!negative &&
	     buffer[i] < 0){
      count++;

      negative = TRUE;
    }
  }
  
  return(count);
}

/**
 * ags_synth_util_get_xcross_count_s24:
 * @buffer: the buffer containing audio data
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 2.0.0
 */
guint
ags_synth_util_get_xcross_count_s24(gint32 *buffer,
				    guint buffer_size)
{
  guint count;
  guint i;
  gboolean negative;
  
  count = 0;

  if(buffer[0] > 0){
    negative = FALSE;
  }else{
    negative = TRUE;
  }

  for(i = 1; i < buffer_size; i++){
    if(negative &&
       buffer[i] > 0){
      count++;

      negative = FALSE;
    }else if(!negative &&
	     buffer[i] < 0){
      count++;

      negative = TRUE;
    }
  }
  
  return(count);
}

/**
 * ags_synth_util_get_xcross_count_s32:
 * @buffer: the buffer containing audio data
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 2.0.0
 */
guint
ags_synth_util_get_xcross_count_s32(gint32 *buffer,
				    guint buffer_size)
{
  guint count;
  guint i;
  gboolean negative;
  
  count = 0;

  if(buffer[0] > 0){
    negative = FALSE;
  }else{
    negative = TRUE;
  }

  for(i = 1; i < buffer_size; i++){
    if(negative &&
       buffer[i] > 0){
      count++;

      negative = FALSE;
    }else if(!negative &&
	     buffer[i] < 0){
      count++;

      negative = TRUE;
    }
  }
  
  return(count);
}

/**
 * ags_synth_util_get_xcross_count_s64:
 * @buffer: the buffer containing audio data
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 2.0.0
 */
guint
ags_synth_util_get_xcross_count_s64(gint64 *buffer,
				    guint buffer_size)
{
  guint count;
  guint i;
  gboolean negative;
  
  count = 0;

  if(buffer[0] > 0){
    negative = FALSE;
  }else{
    negative = TRUE;
  }

  for(i = 1; i < buffer_size; i++){
    if(negative &&
       buffer[i] > 0){
      count++;

      negative = FALSE;
    }else if(!negative &&
	     buffer[i] < 0){
      count++;

      negative = TRUE;
    }
  }
  
  return(count);
}

/**
 * ags_synth_util_get_xcross_count_float:
 * @buffer: the buffer containing audio data
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 2.0.0
 */
guint
ags_synth_util_get_xcross_count_float(float *buffer,
				      guint buffer_size)
{
  guint count;
  guint i;
  gboolean negative;
  
  count = 0;

  if(buffer[0] > 0.0){
    negative = FALSE;
  }else{
    negative = TRUE;
  }

  for(i = 1; i < buffer_size; i++){
    if(negative &&
       buffer[i] > 0.0){
      count++;

      negative = FALSE;
    }else if(!negative &&
	     buffer[i] < 0.0){
      count++;

      negative = TRUE;
    }
  }
  
  return(count);
}

/**
 * ags_synth_util_get_xcross_count_double:
 * @buffer: the buffer containing audio data
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 2.0.0
 */
guint
ags_synth_util_get_xcross_count_double(double *buffer,
				       guint buffer_size)
{
  guint count;

  guint i;
  gboolean negative;
  
  count = 0;

  if(buffer[0] > 0){
    negative = FALSE;
  }else{
    negative = TRUE;
  }

  for(i = 1; i < buffer_size; i++){
    if(negative &&
       buffer[i] > 0.0){
      count++;

      negative = FALSE;
    }else if(!negative &&
	     buffer[i] < 0.0){
      count++;

      negative = TRUE;
    }
  }
  
  return(count);
}

/**
 * ags_synth_util_get_xcross_count_complex:
 * @buffer: the buffer containing audio data
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 2.3.0
 */
guint
ags_synth_util_get_xcross_count_complex(AgsComplex *buffer,
					guint buffer_size)
{
  guint count;
  complex z;
  
  guint i;
  gboolean negative;
  
  count = 0;

  z = ags_complex_get(buffer);
  
  if(cabs(z) > 0){
    negative = FALSE;
  }else{
    negative = TRUE;
  }

  for(i = 1; i < buffer_size; i++){
    z = ags_complex_get(buffer + i);

    if(negative &&
       cabs(z) > 0.0){
      count++;

      negative = FALSE;
    }else if(!negative &&
	     cabs(z) < 0.0){
      count++;

      negative = TRUE;
    }
  }
  
  return(count);
}

/**
 * ags_synth_util_get_xcross_count:
 * @buffer: the buffer containing audio data
 * @audio_buffer_util_format: the audio buffer util format
 * @buffer_size: the buffer size
 * 
 * Get zero-cross count. 
 *
 * Returns: the count of zero-crossings
 *
 * Since: 2.0.0
 */
guint
ags_synth_util_get_xcross_count(void *buffer,
				guint audio_buffer_util_format,
				guint buffer_size)
{
  guint count;

  count = 0;

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    count = ags_synth_util_get_xcross_count_s8((gint8 *) buffer,
					       buffer_size);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    count = ags_synth_util_get_xcross_count_s16((gint16 *) buffer,
						buffer_size);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    count = ags_synth_util_get_xcross_count_s24((gint32 *) buffer,
						buffer_size);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    count = ags_synth_util_get_xcross_count_s32((gint32 *) buffer,
						buffer_size);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    count = ags_synth_util_get_xcross_count_s64((gint64 *) buffer,
						buffer_size);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    count = ags_synth_util_get_xcross_count_float((float *) buffer,
						  buffer_size);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    count = ags_synth_util_get_xcross_count_double((double *) buffer,
						   buffer_size);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    count = ags_synth_util_get_xcross_count_complex((AgsComplex *) buffer,
						    buffer_size);
  }
  break;
  default:
  {
    g_warning("ags_synth_util_get_xcross_count() - unsupported format");
  }
  }
  
  return(count);
}

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
 * Since: 2.0.0
 */
void
ags_synth_util_sin_s8(gint8 *buffer,
		      gdouble freq, gdouble phase, gdouble volume,
		      guint samplerate,
		      guint offset, guint n_frames)
{
  static const gdouble scale = 127.0;
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_sin_s16(gint16 *buffer,
		       gdouble freq, gdouble phase, gdouble volume,
		       guint samplerate,
		       guint offset, guint n_frames)
{
  static const gdouble scale = 32767.0;
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_sin_s24(gint32 *buffer,
		       gdouble freq, gdouble phase, gdouble volume,
		       guint samplerate,
		       guint offset, guint n_frames)
{
  static const gdouble scale = 8388607.0;
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint32) (0xffffff & ((gint32) buffer[i] + (gint32) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_sin_s32(gint32 *buffer,
		       gdouble freq, gdouble phase, gdouble volume,
		       guint samplerate,
		       guint offset, guint n_frames)
{
  static const gdouble scale = 214748363.0;
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_sin_s64(gint64 *buffer,
		       gdouble freq, gdouble phase, gdouble volume,
		       guint samplerate,
		       guint offset, guint n_frames)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint64) (0xffffffffffff & ((gint64) buffer[i] + (gint64) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_sin_float(float *buffer,
			 gdouble freq, gdouble phase, gdouble volume,
			 guint samplerate,
			 guint offset, guint n_frames)
{
  guint i;

  for(i = offset; i < offset + n_frames; i++){
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
 * Since: 2.0.0
 */
void
ags_synth_util_sin_double(double *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames)
{
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (double) ((gdouble) buffer[i] + (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * volume));
  }
}

/**
 * ags_synth_util_sin_complex:
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
 * Since: 2.3.0
 */
void
ags_synth_util_sin_complex(AgsComplex *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  AgsComplex c;
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  double *y_ptr;
  double y;
  complex z0, z1;

  guint i_stop;
  guint i;

  c_ptr = &c;
  c_ptr_ptr = &c_ptr;
  
  y_ptr = &y;

  i_stop = offset + n_frames;
  
  for(i = offset; i < i_stop; i++){
    y = (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) * volume);

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
 * Since: 2.0.0
 */
void
ags_synth_util_sawtooth_s8(gint8 *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  static const gdouble scale = 127.0;
  guint i;

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_sawtooth_s16(gint16 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 32767.0;
  guint i;

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_sawtooth_s24(gint32 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 8388607.0;
  guint i;

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint32) (0xffffff & ((gint32) buffer[i] + (gint32) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_sawtooth_s32(gint32 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 214748363.0;
  guint i;

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_sawtooth_s64(gint64 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_sawtooth_float(float *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames)
{
  guint i;

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = ((double) buffer[i] + (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * volume));
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
 * Since: 2.0.0
 */
void
ags_synth_util_sawtooth_double(double *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames)
{
  guint i;

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = ((double) buffer[i] + (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * volume));
  }
}

/**
 * ags_synth_util_sawtooth_complex:
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
 * Since: 2.3.0
 */
void
ags_synth_util_sawtooth_complex(AgsComplex *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames)
{
  AgsComplex c;
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  double *y_ptr;
  double y;
  complex z0, z1;

  guint i_stop;
  guint i;

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  c_ptr = &c;
  c_ptr_ptr = &c_ptr;

  y_ptr = &y;

  i_stop = offset + n_frames;

  for(i = offset; i < i_stop; i++){
    y = (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * freq / samplerate) - 1.0) * volume);
    
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
 * Since: 2.0.0
 */
void
ags_synth_util_triangle_s8(gint8 *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  static const gdouble scale = 127.0;
  guint i;

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_triangle_s16(gint16 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 32767.0;
  guint i;

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_triangle_s24(gint32 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 8388607.0;
  guint i;

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint32) (0xffffff & ((gint32) buffer[i] + (gint32) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_triangle_s32(gint32 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 214748363.0;
  guint i;

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_triangle_s64(gint64 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_triangle_float(float *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames)
{
  guint i;

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
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
 * Since: 2.0.0
 */
void
ags_synth_util_triangle_double(double *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames)
{
  guint i;

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  for(i = offset; i < offset + n_frames; i++){
    buffer[i] = ((double) buffer[i] + (double) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * volume));
  }
}

/**
 * ags_synth_util_triangle_complex:
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
 * Since: 2.3.0
 */
void
ags_synth_util_triangle_complex(AgsComplex *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames)
{
  AgsComplex c;
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  double *y_ptr;
  double y;
  complex z0, z1;

  guint i_stop;
  guint i;

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  c_ptr = &c;
  c_ptr_ptr = &c_ptr;
  
  y_ptr = &y;

  i_stop = offset + n_frames;

  for(i = offset; i < i_stop; i++){
    y = (double) ((((phase + i) * freq / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * freq / samplerate)) / 2.0) * 2) - 1.0) * volume);

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
 * Since: 2.0.0
 */
void
ags_synth_util_square_s8(gint8 *buffer,
			 gdouble freq, gdouble phase, gdouble volume,
			 guint samplerate,
			 guint offset, guint n_frames)
{
  static const gdouble scale = 127.0;
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (-1.0 * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_square_s16(gint16 *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames)
{
  static const gdouble scale = 32767.0;
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_square_s24(gint32 *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames)
{
  static const gdouble scale = 8388607.0;
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (gint32) (0xffffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint32) (0xffffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_square_s32(gint32 *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames)
{
  static const gdouble scale = 214748363.0;
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_square_s64(gint64 *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
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
 * Since: 2.0.0
 */
void
ags_synth_util_square_float(float *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames)
{
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
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
 * Since: 2.0.0
 */
void
ags_synth_util_square_double(double *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate,
			     guint offset, guint n_frames)
{
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      buffer[i] = (buffer[i] + (1.0 * volume));
    }else{
      buffer[i] = (buffer[i] + (-1.0 * volume));
    }
  }
}

/**
 * ags_synth_util_square_complex:
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
 * Since: 2.3.0
 */
void
ags_synth_util_square_complex(AgsComplex *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames)
{
  AgsComplex c;
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  double *y_ptr;
  double y;
  complex z0, z1;

  guint i_stop;
  guint i;

  c_ptr = &c;
  c_ptr_ptr = &c_ptr;

  y_ptr = &y;

  i_stop = offset + n_frames;

  for(i = offset; i < i_stop; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= 0.0){
      y = (1.0 * volume);
    }else{
      y = (-1.0 * volume);
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
 * ags_synth_util_impulse_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @volume: the volume of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate impulse wave.
 *
 * Since: 2.3.0
 */
void
ags_synth_util_impulse_s8(gint8 *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames)
{
  static const gdouble scale = 127.0;
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_impulse_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @volume: the volume of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate impulse wave.
 *
 * Since: 2.3.0
 */
void
ags_synth_util_impulse_s16(gint16 *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  static const gdouble scale = 32767.0;
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_impulse_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @volume: the volume of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate impulse wave.
 *
 * Since: 2.3.0
 */
void
ags_synth_util_impulse_s24(gint32 *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  static const gdouble scale = 8388607.0;
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (gint32) (0xffffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint32) (0xffffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_impulse_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @volume: the volume of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate impulse wave.
 *
 * Since: 2.3.0
 */
void
ags_synth_util_impulse_s32(gint32 *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  static const gdouble scale = 214748363.0;
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_impulse_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @volume: the volume of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate impulse wave.
 *
 * Since: 2.3.0
 */
void
ags_synth_util_impulse_s64(gint64 *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
    }else{
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
    }
  }
}

/**
 * ags_synth_util_impulse_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @volume: the volume of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate impulse wave.
 *
 * Since: 2.3.0
 */
void
ags_synth_util_impulse_float(float *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate,
			     guint offset, guint n_frames)
{
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (buffer[i] + (1.0 * volume));
    }else{
      buffer[i] = (buffer[i] + (-1.0 * volume));
    }
  }
}

/**
 * ags_synth_util_impulse_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @volume: the volume of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate impulse wave.
 *
 * Since: 2.3.0
 */
void
ags_synth_util_impulse_double(double *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames)
{
  guint i;

  for(i = offset; i < offset + n_frames; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      buffer[i] = (buffer[i] + (1.0 * volume));
    }else{
      buffer[i] = (buffer[i] + (-1.0 * volume));
    }
  }
}

/**
 * ags_synth_util_impulse_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @volume: the volume of the impulse wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate impulse wave.
 *
 * Since: 2.3.0
 */
void
ags_synth_util_impulse_complex(AgsComplex *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames)
{
  AgsComplex c;
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  double *y_ptr;
  double y;
  complex z0, z1;

  guint i_stop;
  guint i;

  c_ptr = &c;
  c_ptr_ptr = &c_ptr;

  y_ptr = &y;

  i_stop = offset + n_frames;

  for(i = offset; i < i_stop; i++){
    if(sin((gdouble) (i + phase) * 2.0 * M_PI * freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
      y = (1.0 * volume);
    }else{
      y = (-1.0 * volume);
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
 * Since: 2.0.0
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
    ags_synth_util_sin_s8((gint8 *) buffer,
			  freq, phase, volume,
			  samplerate,
			  offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_synth_util_sin_s16((gint16 *) buffer,
			   freq, phase, volume,
			   samplerate,
			   offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_synth_util_sin_s24((gint32 *) buffer,
			   freq, phase, volume,
			   samplerate,
			   offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_synth_util_sin_s32((gint32 *) buffer,
			   freq, phase, volume,
			   samplerate,
			   offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_synth_util_sin_s64((gint64 *) buffer,
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
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_synth_util_sin_complex((AgsComplex *) buffer,
			       freq, phase, volume,
			       samplerate,
			       offset, n_frames);
  }
  break;
  default:
  {
    g_warning("ags_synth_util_sin() - unsupported format");
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
 * Since: 2.0.0
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
    ags_synth_util_sawtooth_s8((gint8 *) buffer,
			       freq, phase, volume,
			       samplerate,
			       offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_synth_util_sawtooth_s16((gint16 *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_synth_util_sawtooth_s24((gint32 *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_synth_util_sawtooth_s32((gint32 *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_synth_util_sawtooth_s64((gint64 *) buffer,
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
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_synth_util_sawtooth_complex((AgsComplex *) buffer,
				    freq, phase, volume,
				    samplerate,
				    offset, n_frames);
  }
  break;
  default:
  {
    g_warning("ags_synth_util_sawtooth() - unsupported format");
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
 * Since: 2.0.0
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
    ags_synth_util_triangle_s8((gint8 *) buffer,
			       freq, phase, volume,
			       samplerate,
			       offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_synth_util_triangle_s16((gint16 *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_synth_util_triangle_s24((gint32 *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_synth_util_triangle_s32((gint32 *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_synth_util_triangle_s64((gint64 *) buffer,
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
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_synth_util_triangle_complex((AgsComplex *) buffer,
				    freq, phase, volume,
				    samplerate,
				    offset, n_frames);
  }
  break;
  default:
  {
    g_warning("ags_synth_util_triangle() - unsupported format");
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
 * Since: 2.0.0
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
    ags_synth_util_square_s8((gint8 *) buffer,
			     freq, phase, volume,
			     samplerate,
			     offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_synth_util_square_s16((gint16 *) buffer,
			      freq, phase, volume,
			      samplerate,
			      offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_synth_util_square_s24((gint32 *) buffer,
			      freq, phase, volume,
			      samplerate,
			      offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_synth_util_square_s32((gint32 *) buffer,
			      freq, phase, volume,
			      samplerate,
			      offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_synth_util_square_s64((gint64 *) buffer,
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
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_synth_util_square_complex((AgsComplex *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames);
  }
  break;
  default:
  {
    g_warning("ags_synth_util_square() - unsupported format");
  }
  }
}

/**
 * ags_synth_util_impulse:
 * @buffer: the audio buffer
 * @freq: the frequency of the impulse wave
 * @phase: the phase of the impulse wave
 * @volume: the volume of the impulse wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 *
 * Generate impulse wave.
 *
 * Since: 2.3.0
 */
void
ags_synth_util_impulse(void *buffer,
		       gdouble freq, gdouble phase, gdouble volume,
		       guint samplerate, guint audio_buffer_util_format,
		       guint offset, guint n_frames)
{
  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_synth_util_impulse_s8((gint8 *) buffer,
			      freq, phase, volume,
			      samplerate,
			      offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_synth_util_impulse_s16((gint16 *) buffer,
			       freq, phase, volume,
			       samplerate,
			       offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_synth_util_impulse_s24((gint32 *) buffer,
			       freq, phase, volume,
			       samplerate,
			       offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_synth_util_impulse_s32((gint32 *) buffer,
			       freq, phase, volume,
			       samplerate,
			       offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_synth_util_impulse_s64((gint64 *) buffer,
			       freq, phase, volume,
			       samplerate,
			       offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_synth_util_impulse_float((float *) buffer,
				 freq, phase, volume,
				 samplerate,
				 offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_synth_util_impulse_double((double *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_synth_util_impulse_complex((AgsComplex *) buffer,
				   freq, phase, volume,
				   samplerate,
				   offset, n_frames);
  }
  break;
  default:
  {
    g_warning("ags_synth_util_impulse() - unsupported format");
  }
  }
}
