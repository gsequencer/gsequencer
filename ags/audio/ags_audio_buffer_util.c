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

#include <ags/audio/ags_audio_buffer_util.h>

signed char*
ags_audio_buffer_util_morph_s8(signed char *buffer, guint channels,
			       guint buffer_length,
			       guint morph_lookbehind,
			       AgsComplex *morph)
{
  //TODO:JK: implement me
}

signed short*
ags_audio_buffer_util_morph_s16(signed short *buffer, guint channels,
				guint buffer_length,
				guint morph_lookbehind,
				AgsComplex *morph)
{
  //TODO:JK: implement me
}

signed long*
ags_audio_buffer_util_morph_s24(signed long *buffer, guint channels,
				guint buffer_length,
				guint morph_lookbehind,
				AgsComplex *morph)
{
  //TODO:JK: implement me
}

signed long*
ags_audio_buffer_util_morph_s32(signed long *buffer, guint channels,
				guint buffer_length,
				guint morph_lookbehind,
				AgsComplex *morph)
{
  //TODO:JK: implement me
}

signed long long*
ags_audio_buffer_util_morph_s64(signed char *buffer, guint channels,
				guint buffer_length,
				guint morph_lookbehind,
				AgsComplex *morph)
{
  //TODO:JK: implement me
}

float*
ags_audio_buffer_util_morph_float(float *buffer, guint channels,
				  guint buffer_length,
				  guint morph_lookbehind,
				  AgsComplex *morph)
{
  //TODO:JK: implement me
}

double*
ags_audio_buffer_util_morph_double(double *buffer, guint channels,
				   guint buffer_length,
				   guint morph_lookbehind,
				   AgsComplex *morph)
{
  //TODO:JK: implement me
}

void*
ags_audio_buffer_util_morph(void *buffer, guint channels,
			    guint format,
			    guint buffer_length,
			    guint morph_lookbehind,
			    AgsComplex *morph)
{
  //TODO:JK: implement me
}

signed char*
ags_audio_buffer_util_resample_s8(signed char *buffer, guint channels,
				  guint samplerate,
				  guint buffer_length,
				  guint target_samplerate)
{
  //TODO:JK: implement me
}

signed short*
ags_audio_buffer_util_resample_s16(signed short *buffer, guint channels,
				   guint samplerate,
				   guint buffer_length,
				   guint target_samplerate)
{
  //TODO:JK: implement me
}

signed long*
ags_audio_buffer_util_resample_s24(signed long *buffer, guint channels,
				   guint samplerate,
				   guint buffer_length,
				   guint target_samplerate)
{
  //TODO:JK: implement me
}

signed long*
ags_audio_buffer_util_resample_s32(signed long *buffer, guint channels,
				   guint samplerate,
				   guint buffer_length,
				   guint target_samplerate)
{
  //TODO:JK: implement me
}

signed long long*
ags_audio_buffer_util_resample_s64(signed char *buffer, guint channels,
				   guint samplerate,
				   guint buffer_length,
				   guint target_samplerate)
{
  //TODO:JK: implement me
}

float*
ags_audio_buffer_util_resample_float(float *buffer, guint channels,
				     guint samplerate,
				     guint buffer_length,
				     guint target_samplerate)
{
  //TODO:JK: implement me
}

double*
ags_audio_buffer_util_resample_double(double *buffer, guint channels,
				      guint samplerate,
				      guint buffer_length,
				      guint target_samplerate)
{
  //TODO:JK: implement me
}

void*
ags_audio_buffer_util_resample(void *buffer, guint channels,
			       guint format,  guint samplerate,
			       guint buffer_length,
			       guint target_samplerate)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s8_to_s8(signed char *destination, guint dchannels,
				    signed char *source, guint schannels,
				    guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s8_to_s16(signed short *destination, guint dchannels,
				     signed char *source, guint schannels,
				     guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s8_to_s32(signed long *destination, guint dchannels,
				     signed char *source, guint schannels,
				     guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s8_to_s64(signed long long *destination, guint dchannels,
				     signed char *source, guint schannels,
				     guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s8_to_float(signed float *destination, guint dchannels,
				       signed char *source, guint schannels,
				       guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s8_to_double(signed double *destination, guint dchannels,
					signed char *source, guint schannels,
					guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s16_to_s8(signed char *destination, guint dchannels,
				     signed short *source, guint schannels,
				     guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s16_to_s16(signed short *destination, guint dchannels,
				      signed short *source, guint schannels,
				      guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s16_to_s32(signed long *destination, guint dchannels,
				      signed short *source, guint schannels,
				      guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s16_to_s64(signed long long *destination, guint dchannels,
				      signed short *source, guint schannels,
				      guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s16_to_float(signed float *destination, guint dchannels,
					signed short *source, guint schannels,
					guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s16_to_double(signed double *destination, guint dchannels,
					 signed short *source, guint schannels,
					 guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s24_to_s8(signed char *destination, guint dchannels,
				     signed long *source, guint schannels,
				     guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s24_to_s16(signed short *destination, guint dchannels,
				      signed long *source, guint schannels,
				      guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s24_to_s32(signed long *destination, guint dchannels,
				      signed long *source, guint schannels,
				      guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s24_to_s64(signed long long *destination, guint dchannels,
				      signed long *source, guint schannels,
				      guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s24_to_float(signed float *destination, guint dchannels,
					signed long *source, guint schannels,
					guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s24_to_double(signed double *destination, guint dchannels,
					 signed long *source, guint schannels,
					 guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s32_to_s8(signed char *destination, guint dchannels,
				     signed long *source, guint schannels,
				     guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s32_to_s16(signed short *destination, guint dchannels,
				      signed long *source, guint schannels,
				      guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s32_to_s32(signed long *destination, guint dchannels,
				      signed long *source, guint schannels,
				      guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s32_to_s64(signed long long *destination, guint dchannels,
				      signed long *source, guint schannels,
				      guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s32_to_float(signed float *destination, guint dchannels,
					signed long *source, guint schannels,
					guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s32_to_double(signed double *destination, guint dchannels,
					 signed long *source, guint schannels,
					 guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s64_to_s8(signed char *destination, guint dchannels,
				     signed long long *source, guint schannels,
				     guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s64_to_s16(signed short *destination, guint dchannels,
				      signed long long *source, guint schannels,
				      guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s64_to_s32(signed long *destination, guint dchannels,
				      signed long long *source, guint schannels,
				      guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s64_to_s64(signed long long *destination, guint dchannels,
				      signed long long *source, guint schannels,
				      guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s64_to_float(signed float *destination, guint dchannels,
					signed long long *source, guint schannels,
					guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_s64_to_double(signed double *destination, guint dchannels,
					 signed long long *source, guint schannels,
					 guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_float_to_s8(signed char *destination, guint dchannels,
				       signed float *source, guint schannels,
				       guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_float_to_s16(signed short *destination, guint dchannels,
					signed float *source, guint schannels,
					guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_float_to_s32(signed long *destination, guint dchannels,
					signed float *source, guint schannels,
					guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_float_to_s64(signed long long *destination, guint dchannels,
					signed float *source, guint schannels,
					guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_float_to_float(signed float *destination, guint dchannels,
					  signed float *source, guint schannels,
					  guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_float_to_double(signed double *destination, guint dchannels,
					   signed float *source, guint schannels,
					   guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_double_to_s8(signed char *destination, guint dchannels,
					signed double *source, guint schannels,
					guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_double_to_s16(signed short *destination, guint dchannels,
					 signed double *source, guint schannels,
					 guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_double_to_s32(signed long *destination, guint dchannels,
					 signed double *source, guint schannels,
					 guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_double_to_s64(signed long long *destination, guint dchannels,
					 signed double *source, guint schannels,
					 guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_double_to_float(signed float *destination, guint dchannels,
					   signed double *source, guint schannels,
					   guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_double_to_double(signed double *destination, guint dchannels,
					    signed double *source, guint schannels,
					    guint count)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_get_copy_mode(guint dformat, gboolean source_is_soundcard,
				    guint sformat, gboolean destination_is_soundcard)
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_copy_buffer_to_buffer(void *destination, guint dchannels,
					    void *source, guint schannels,
					    guint count, guint mode)
{
  //TODO:JK: implement me
}
