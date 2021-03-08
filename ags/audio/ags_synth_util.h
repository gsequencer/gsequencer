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

#ifndef __AGS_SYNTH_UTIL_H__
#define __AGS_SYNTH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

/* zero-crossing */
guint ags_synth_util_get_xcross_count_s8(gint8 *buffer,
					 guint buffer_size);
guint ags_synth_util_get_xcross_count_s16(gint16 *buffer,
					  guint buffer_size);
guint ags_synth_util_get_xcross_count_s24(gint32 *buffer,
					  guint buffer_size);
guint ags_synth_util_get_xcross_count_s32(gint32 *buffer,
					  guint buffer_size);
guint ags_synth_util_get_xcross_count_s64(gint64 *buffer,
					  guint buffer_size);
guint ags_synth_util_get_xcross_count_float(gfloat *buffer,
					    guint buffer_size);
guint ags_synth_util_get_xcross_count_double(gdouble *buffer,
					     guint buffer_size);
guint ags_synth_util_get_xcross_count_complex(AgsComplex *buffer,
					      guint buffer_size);

guint ags_synth_util_get_xcross_count(void *buffer,
				      guint audio_buffer_util_format,
				      guint buffer_size);

/* sin oscillator */
void ags_synth_util_sin_s8(gint8 *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames);
void ags_synth_util_sin_s16(gint16 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames);
void ags_synth_util_sin_s24(gint32 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames);
void ags_synth_util_sin_s32(gint32 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames);
void ags_synth_util_sin_s64(gint64 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames);
void ags_synth_util_sin_float(gfloat *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames);
void ags_synth_util_sin_double(gdouble *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames);
void ags_synth_util_sin_complex(AgsComplex *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames);

/* sawtooth oscillator */
void ags_synth_util_sawtooth_s8(gint8 *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames);
void ags_synth_util_sawtooth_s16(gint16 *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
void ags_synth_util_sawtooth_s24(gint32 *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
void ags_synth_util_sawtooth_s32(gint32 *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
void ags_synth_util_sawtooth_s64(gint64 *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
void ags_synth_util_sawtooth_float(gfloat *buffer,
				   gdouble freq, gdouble phase, gdouble volume,
				   guint samplerate,
				   guint offset, guint n_frames);
void ags_synth_util_sawtooth_double(gdouble *buffer,
				    gdouble freq, gdouble phase, gdouble volume,
				    guint samplerate,
				    guint offset, guint n_frames);
void ags_synth_util_sawtooth_complex(AgsComplex *buffer,
				     gdouble freq, gdouble phase, gdouble volume,
				     guint samplerate,
				     guint offset, guint n_frames);

/* triangle oscillator */
void ags_synth_util_triangle_s8(gint8 *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames);
void ags_synth_util_triangle_s16(gint16 *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
void ags_synth_util_triangle_s24(gint32 *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
void ags_synth_util_triangle_s32(gint32 *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
void ags_synth_util_triangle_s64(gint64 *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
void ags_synth_util_triangle_float(gfloat *buffer,
				   gdouble freq, gdouble phase, gdouble volume,
				   guint samplerate,
				   guint offset, guint n_frames);
void ags_synth_util_triangle_double(gdouble *buffer,
				    gdouble freq, gdouble phase, gdouble volume,
				    guint samplerate,
				    guint offset, guint n_frames);
void ags_synth_util_triangle_complex(AgsComplex *buffer,
				     gdouble freq, gdouble phase, gdouble volume,
				     guint samplerate,
				     guint offset, guint n_frames);

/* square oscillator */
void ags_synth_util_square_s8(gint8 *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames);
void ags_synth_util_square_s16(gint16 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames);
void ags_synth_util_square_s24(gint32 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames);
void ags_synth_util_square_s32(gint32 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames);
void ags_synth_util_square_s64(gint64 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames);
void ags_synth_util_square_float(gfloat *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
void ags_synth_util_square_double(gdouble *buffer,
				  gdouble freq, gdouble phase, gdouble volume,
				  guint samplerate,
				  guint offset, guint n_frames);
void ags_synth_util_square_complex(AgsComplex *buffer,
				   gdouble freq, gdouble phase, gdouble volume,
				   guint samplerate,
				   guint offset, guint n_frames);

/* impulse oscillator */
void ags_synth_util_impulse_s8(gint8 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames);
void ags_synth_util_impulse_s16(gint16 *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames);
void ags_synth_util_impulse_s24(gint32 *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames);
void ags_synth_util_impulse_s32(gint32 *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames);
void ags_synth_util_impulse_s64(gint64 *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames);
void ags_synth_util_impulse_float(gfloat *buffer,
				  gdouble freq, gdouble phase, gdouble volume,
				  guint samplerate,
				  guint offset, guint n_frames);
void ags_synth_util_impulse_double(gdouble *buffer,
				   gdouble freq, gdouble phase, gdouble volume,
				   guint samplerate,
				   guint offset, guint n_frames);
void ags_synth_util_impulse_complex(AgsComplex *buffer,
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

void ags_synth_util_impulse(void *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate, guint audio_buffer_util_format,
			    guint offset, guint n_frames);

G_END_DECLS

#endif /*__AGS_SYNTH_UTIL_H__*/
