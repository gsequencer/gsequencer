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

#ifndef __AGS_SYNTH_UTIL_H__
#define __AGS_SYNTH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

/* sin oscillator */
void ags_synth_util_sin_s8(signed char *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames);
void ags_synth_util_sin_s16(signed short *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames);
void ags_synth_util_sin_s24(signed long *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames);
void ags_synth_util_sin_s32(signed long *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames);
void ags_synth_util_sin_s64(signed long long *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames);
void ags_synth_util_sin_float(float *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames);
void ags_synth_util_sin_double(double *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames);

/* sawtooth oscillator */
void ags_synth_util_sawtooth_s8(signed char *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames);
void ags_synth_util_sawtooth_s16(signed short *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
void ags_synth_util_sawtooth_s24(signed long *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
void ags_synth_util_sawtooth_s32(signed long *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
void ags_synth_util_sawtooth_s64(signed long long *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
void ags_synth_util_sawtooth_float(float *buffer,
				   gdouble freq, gdouble phase, gdouble volume,
				   guint samplerate,
				   guint offset, guint n_frames);
void ags_synth_util_sawtooth_double(double *buffer,
				    gdouble freq, gdouble phase, gdouble volume,
				    guint samplerate,
				    guint offset, guint n_frames);

/* triangle oscillator */
void ags_synth_util_triangle_s8(signed char *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames);
void ags_synth_util_triangle_s16(signed short *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
void ags_synth_util_triangle_s24(signed long *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
void ags_synth_util_triangle_s32(signed long *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
void ags_synth_util_triangle_s64(signed long long *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
void ags_synth_util_triangle_float(float *buffer,
				   gdouble freq, gdouble phase, gdouble volume,
				   guint samplerate,
				   guint offset, guint n_frames);
void ags_synth_util_triangle_double(double *buffer,
				    gdouble freq, gdouble phase, gdouble volume,
				    guint samplerate,
				    guint offset, guint n_frames);

/* square oscillator */
void ags_synth_util_square_s8(signed char *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames);
void ags_synth_util_square_s16(signed short *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames);
void ags_synth_util_square_s24(signed long *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames);
void ags_synth_util_square_s32(signed long *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames);
void ags_synth_util_square_s64(signed long long *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames);
void ags_synth_util_square_float(float *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
void ags_synth_util_square_double(double *buffer,
				  gdouble freq, gdouble phase, gdouble volume,
				  guint samplerate,
				  guint offset, guint n_frames);

/* oscillators */
void ags_synth_util_sin(void *buffer,
			gdouble freq, gdouble phase, gdouble volume,
			guint samplerate, guint audio_buffer_util_format,
			guint offset, guint n_frames);

void ags_synth_util_sawtooth(void *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate, guint audio_buffer_util_format,
			     guint offset, guint n_frames);

void ags_synth_util_triangle(void *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate, guint audio_buffer_util_format,
			     guint offset, guint n_frames);

void ags_synth_util_square(void *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate, guint audio_buffer_util_format,
			   guint offset, guint n_frames);

#endif /*__AGS_SYNTH_UTIL_H__*/
