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

#ifndef __AGS_AUDIO_BUFFER_UTIL_H__
#define __AGS_AUDIO_BUFFER_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_complex.h>

#define AGS_AUDIO_BUFFER_S8(ptr) ((signed char *)(ptr))
#define AGS_AUDIO_BUFFER_S16(ptr) ((signed short *)(ptr))
#define AGS_AUDIO_BUFFER_S24(ptr) ((signed long *)(ptr))
#define AGS_AUDIO_BUFFER_S32(ptr) ((signed long *)(ptr))
#define AGS_AUDIO_BUFFER_S64(ptr) ((signed long long *)(ptr))
#define AGS_AUDIO_BUFFER_FLOAT(ptr) ((float *)(ptr))
#define AGS_AUDIO_BUFFER_DOUBLE(ptr) ((double *)(ptr))

typedef enum{
  AGS_AUDIO_BUFFER_UTIL_S8,
  AGS_AUDIO_BUFFER_UTIL_S16,
  AGS_AUDIO_BUFFER_UTIL_S24,
  AGS_AUDIO_BUFFER_UTIL_S32,
  AGS_AUDIO_BUFFER_UTIL_S64,
  AGS_AUDIO_BUFFER_UTIL_FLOAT,
  AGS_AUDIO_BUFFER_UTIL_DOUBLE,
}AgsAudioBufferUtilFormat;

typedef enum{
  AGS_AUDIO_BUFFER_UTIL_8000HZ,
  AGS_AUDIO_BUFFER_UTIL_44100HZ,
  AGS_AUDIO_BUFFER_UTIL_48000HZ,
  AGS_AUDIO_BUFFER_UTIL_96000HZ,
  AGS_AUDIO_BUFFER_UTIL_176400HZ,
  AGS_AUDIO_BUFFER_UTIL_192000HZ,
  AGS_AUDIO_BUFFER_UTIL_352800HZ,
  AGS_AUDIO_BUFFER_UTIL_2822400HZ,
  AGS_AUDIO_BUFFER_UTIL_5644800HZ,
}AgsAudioBufferUtilSamplerate;

typedef enum{
  /* signed 8 bit source*/
  AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S8,
  AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S16,
  AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S24,
  AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S32,
  AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S64,
  AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_FLOAT,
  AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_DOUBLE,
  /* signed 16 bit source */  
  AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S8,
  AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16,
  AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S24,
  AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S32,
  AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S64,
  AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_FLOAT,
  AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_DOUBLE,
  /* signed 24 bit source */
  AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S8,
  AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S16,
  AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S24,
  AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S32,
  AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S64,
  AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_FLOAT,
  AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_DOUBLE,
  /* signed 32 bit source */
  AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S8,
  AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S16,
  AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S24,
  AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32,
  AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S64,
  AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_FLOAT,
  AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_DOUBLE,
  /* signed 64 bit source */
  AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S8,
  AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S16,
  AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S24,
  AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S32,
  AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S64,
  AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_FLOAT,
  AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_DOUBLE,
  /* float source */
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S8,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S16,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S24,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S32,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S64,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_DOUBLE,
  /* double source */
  AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S8,
  AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S16,
  AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S24,
  AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S32,
  AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S64,
  AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_FLOAT,
  AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_DOUBLE,
}AgsAudioBufferUtilCopyMode;

typedef signed short v8s16 __attribute__ ((vector_size(8 * sizeof(signed short))));

guint ags_audio_buffer_util_format_from_soundcard(guint soundcard_format);
guint ags_audio_buffer_util_get_copy_mode(guint destination_format,
					  guint source_format);

/* clear */
void ags_audio_buffer_util_clear_float(float *buffer, guint channels,
				       guint count);
void ags_audio_buffer_util_clear_double(double *buffer, guint channels,
					guint count);
void ags_audio_buffer_util_clear_buffer(void *buffer, guint channels,
					guint count, guint format);

/* envelope */
gdouble ags_audio_buffer_util_envelope_s8(signed char *buffer, guint channels,
					  guint buffer_length,
					  gdouble current_volume,
					  gdouble ratio);
gdouble ags_audio_buffer_util_envelope_s16(signed short *buffer, guint channels,
					   guint buffer_length,
					   gdouble current_volume,
					   gdouble ratio);
gdouble ags_audio_buffer_util_envelope_s24(signed long *buffer, guint channels,
					   guint buffer_length,
					   gdouble current_volume,
					   gdouble ratio);
gdouble ags_audio_buffer_util_envelope_s32(signed long *buffer, guint channels,
					   guint buffer_length,
					   gdouble current_volume,
					   gdouble ratio);
gdouble ags_audio_buffer_util_envelope_s64(signed long long *buffer, guint channels,
					   guint buffer_length,
					   gdouble current_volume,
					   gdouble ratio);
gdouble ags_audio_buffer_util_envelope_float(float *buffer, guint channels,
					     guint buffer_length,
					     gdouble current_volume,
					     gdouble ratio);
gdouble ags_audio_buffer_util_envelope_double(double *buffer, guint channels,
					      guint buffer_length,
					      gdouble current_volume,
					      gdouble ratio);

gdouble ags_audio_buffer_util_envelope(void *buffer, guint channels,
				       guint format,
				       guint buffer_length,
				       gdouble current_volume,
				       gdouble ratio);

/* volume */
void ags_audio_buffer_util_volume_s8(signed char *buffer, guint channels,
				     guint buffer_length,
				     gdouble volume);
void ags_audio_buffer_util_volume_s16(signed short *buffer, guint channels,
				      guint buffer_length,
				      gdouble volume);
void ags_audio_buffer_util_volume_s24(signed long *buffer, guint channels,
				      guint buffer_length,
				      gdouble volume);
void ags_audio_buffer_util_volume_s32(signed long *buffer, guint channels,
				      guint buffer_length,
				      gdouble volume);
void ags_audio_buffer_util_volume_s64(signed long long *buffer, guint channels,
				      guint buffer_length,
				      gdouble volume);
void ags_audio_buffer_util_volume_float(float *buffer, guint channels,
					guint buffer_length,
					gdouble volume);
void ags_audio_buffer_util_volume_double(double *buffer, guint channels,
					 guint buffer_length,
					 gdouble volume);

void ags_audio_buffer_util_volume(void *buffer, guint channels,
				  guint format,
				  guint buffer_length,
				  gdouble volume);

/* resample */
signed char* ags_audio_buffer_util_resample_s8(signed char *buffer, guint channels,
					       guint samplerate,
					       guint buffer_length,
					       guint target_samplerate);
signed short* ags_audio_buffer_util_resample_s16(signed short *buffer, guint channels,
						 guint samplerate,
						 guint buffer_length,
						 guint target_samplerate);
signed long* ags_audio_buffer_util_resample_s24(signed long *buffer, guint channels,
						guint samplerate,
						guint buffer_length,
						guint target_samplerate);
signed long* ags_audio_buffer_util_resample_s32(signed long *buffer, guint channels,
						guint samplerate,
						guint buffer_length,
						guint target_samplerate);
signed long long* ags_audio_buffer_util_resample_s64(signed long long *buffer, guint channels,
						     guint samplerate,
						     guint buffer_length,
						     guint target_samplerate);
float* ags_audio_buffer_util_resample_float(float *buffer, guint channels,
					    guint samplerate,
					    guint buffer_length,
					    guint target_samplerate);
double* ags_audio_buffer_util_resample_double(double *buffer, guint channels,
					      guint samplerate,
					      guint buffer_length,
					      guint target_samplerate);

void* ags_audio_buffer_util_resample(void *buffer, guint channels,
				     guint format,  guint samplerate,
				     guint buffer_length,
				     guint target_samplerate);

/* peak */
gdouble ags_audio_buffer_util_peak_s8(signed char *buffer, guint channels,
				      guint buffer_length,
				      gdouble harmonic_rate,
				      gdouble max_rate,
				      gdouble pressure_factor);
gdouble ags_audio_buffer_util_peak_s16(signed short *buffer, guint channels,
				       guint buffer_length,
				       gdouble harmonic_rate,
				       gdouble max_rate,
				       gdouble pressure_factor);
gdouble ags_audio_buffer_util_peak_s24(signed long *buffer, guint channels,
				       guint buffer_length,
				       gdouble harmonic_rate,
				       gdouble max_rate,
				       gdouble pressure_factor);
gdouble ags_audio_buffer_util_peak_s32(signed long *buffer, guint channels,
				       guint buffer_length,
				       gdouble harmonic_rate,
				       gdouble max_rate,
				       gdouble pressure_factor);
gdouble ags_audio_buffer_util_peak_s64(signed long long *buffer, guint channels,
				       guint buffer_length,
				       gdouble harmonic_rate,
				       gdouble max_rate,
				       gdouble pressure_factor);
gdouble ags_audio_buffer_util_peak_float(float *buffer, guint channels,
					 guint buffer_length,
					 gdouble harmonic_rate,
					 gdouble max_rate,
					 gdouble pressure_factor);
gdouble ags_audio_buffer_util_peak_double(double *buffer, guint channels,
					  guint buffer_length,
					  gdouble harmonic_rate,
					  gdouble max_rate,
					  gdouble pressure_factor);

gdouble ags_audio_buffer_util_peak(void *buffer, guint channels,
				   guint format,
				   guint buffer_length,
				   gdouble harmonic_rate,
				   gdouble max_rate,
				   gdouble pressure_factor);

/* copy 8 bit */
void ags_audio_buffer_util_copy_s8_to_s8(signed char *destination, guint dchannels,
					 signed char *source, guint schannels,
					 guint count);
void ags_audio_buffer_util_copy_s8_to_s16(signed short *destination, guint dchannels,
					  signed char *source, guint schannels,
					  guint count);
void ags_audio_buffer_util_copy_s8_to_s24(signed long *destination, guint dchannels,
					  signed char *source, guint schannels,
					  guint count);
void ags_audio_buffer_util_copy_s8_to_s32(signed long *destination, guint dchannels,
					  signed char *source, guint schannels,
					  guint count);
void ags_audio_buffer_util_copy_s8_to_s64(signed long long *destination, guint dchannels,
					  signed char *source, guint schannels,
					  guint count);
void ags_audio_buffer_util_copy_s8_to_float(float *destination, guint dchannels,
					    signed char *source, guint schannels,
					    guint count);
void ags_audio_buffer_util_copy_s8_to_double(double *destination, guint dchannels,
					     signed char *source, guint schannels,
					     guint count);

/* copy 16 bit */
void ags_audio_buffer_util_copy_s16_to_s8(signed char *destination, guint dchannels,
					  signed short *source, guint schannels,
					  guint count);
void ags_audio_buffer_util_copy_s16_to_s16(signed short *destination, guint dchannels,
					   signed short *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s16_to_s24(signed long *destination, guint dchannels,
					   signed short *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s16_to_s32(signed long *destination, guint dchannels,
					   signed short *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s16_to_s64(signed long long *destination, guint dchannels,
					   signed short *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s16_to_float(float *destination, guint dchannels,
					     signed short *source, guint schannels,
					     guint count);
void ags_audio_buffer_util_copy_s16_to_double(double *destination, guint dchannels,
					      signed short *source, guint schannels,
					      guint count);

/* copy 24 bit */
void ags_audio_buffer_util_copy_s24_to_s8(signed char *destination, guint dchannels,
					  signed long *source, guint schannels,
					  guint count);
void ags_audio_buffer_util_copy_s24_to_s16(signed short *destination, guint dchannels,
					   signed long *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s24_to_s24(signed long *destination, guint dchannels,
					   signed long *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s24_to_s32(signed long *destination, guint dchannels,
					   signed long *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s24_to_s64(signed long long *destination, guint dchannels,
					   signed long *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s24_to_float(float *destination, guint dchannels,
					     signed long *source, guint schannels,
					     guint count);
void ags_audio_buffer_util_copy_s24_to_double(double *destination, guint dchannels,
					      signed long *source, guint schannels,
					      guint count);

/* copy 32 bit */
void ags_audio_buffer_util_copy_s32_to_s8(signed char *destination, guint dchannels,
					  signed long *source, guint schannels,
					  guint count);
void ags_audio_buffer_util_copy_s32_to_s16(signed short *destination, guint dchannels,
					   signed long *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s32_to_s24(signed long *destination, guint dchannels,
					   signed long *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s32_to_s32(signed long *destination, guint dchannels,
					   signed long *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s32_to_s64(signed long long *destination, guint dchannels,
					   signed long *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s32_to_float(float *destination, guint dchannels,
					     signed long *source, guint schannels,
					     guint count);
void ags_audio_buffer_util_copy_s32_to_double(double *destination, guint dchannels,
					      signed long *source, guint schannels,
					      guint count);

/* copy 64 bit */
void ags_audio_buffer_util_copy_s64_to_s8(signed char *destination, guint dchannels,
					  signed long long *source, guint schannels,
					  guint count);
void ags_audio_buffer_util_copy_s64_to_s16(signed short *destination, guint dchannels,
					   signed long long *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s64_to_s24(signed long *destination, guint dchannels,
					   signed long long *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s64_to_s32(signed long *destination, guint dchannels,
					   signed long long *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s64_to_s64(signed long long *destination, guint dchannels,
					   signed long long *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s64_to_float(float *destination, guint dchannels,
					     signed long long *source, guint schannels,
					     guint count);
void ags_audio_buffer_util_copy_s64_to_double(double *destination, guint dchannels,
					      signed long long *source, guint schannels,
					      guint count);

/* copy float */
void ags_audio_buffer_util_copy_float_to_s8(signed char *destination, guint dchannels,
					    float *source, guint schannels,
					    guint count);
void ags_audio_buffer_util_copy_float_to_s16(signed short *destination, guint dchannels,
					     float *source, guint schannels,
					     guint count);
void ags_audio_buffer_util_copy_float_to_s24(signed long *destination, guint dchannels,
					     float *source, guint schannels,
					     guint count);
void ags_audio_buffer_util_copy_float_to_s32(signed long *destination, guint dchannels,
					     float *source, guint schannels,
					     guint count);
void ags_audio_buffer_util_copy_float_to_s64(signed long long *destination, guint dchannels,
					     float *source, guint schannels,
					     guint count);
void ags_audio_buffer_util_copy_float_to_float(float *destination, guint dchannels,
					       float *source, guint schannels,
					       guint count);
void ags_audio_buffer_util_copy_float_to_double(double *destination, guint dchannels,
						float *source, guint schannels,
						guint count);

/* copy double */
void ags_audio_buffer_util_copy_double_to_s8(signed char *destination, guint dchannels,
					     double *source, guint schannels,
					     guint count);
void ags_audio_buffer_util_copy_double_to_s16(signed short *destination, guint dchannels,
					      double *source, guint schannels,
					      guint count);
void ags_audio_buffer_util_copy_double_to_s24(signed long *destination, guint dchannels,
					      double *source, guint schannels,
					      guint count);
void ags_audio_buffer_util_copy_double_to_s32(signed long *destination, guint dchannels,
					      double *source, guint schannels,
					      guint count);
void ags_audio_buffer_util_copy_double_to_s64(signed long long *destination, guint dchannels,
					      double *source, guint schannels,
					      guint count);
void ags_audio_buffer_util_copy_double_to_float(float *destination, guint dchannels,
						double *source, guint schannels,
						guint count);
void ags_audio_buffer_util_copy_double_to_double(double *destination, guint dchannels,
						 double *source, guint schannels,
						 guint count);

/* copy */
void ags_audio_buffer_util_copy_buffer_to_buffer(void *destination, guint dchannels, guint doffset,
						 void *source, guint schannels, guint soffset,
						 guint count, guint mode);

#endif /*__AGS_AUDIO_BUFFER_UTIL_H__*/
