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

#include <ags/audio/ags_synth_enums.h>
#include <ags/audio/ags_audio_buffer_util.h>

G_BEGIN_DECLS

#define AGS_TYPE_SYNTH_UTIL         (ags_synth_util_get_type())

#define AGS_SYNTH_UTIL_DEFAULT_FREQUENCY (440.0)
#define AGS_SYNTH_UTIL_DEFAULT_AUDIO_BUFFER_UTIL_FORMAT (AGS_AUDIO_BUFFER_UTIL_S16)

typedef struct _AgsSynthUtil AgsSynthUtil;

struct _AgsSynthUtil
{
  gpointer source;
  guint source_stride;
  
  guint buffer_length;
  guint audio_buffer_util_format;
  guint samplerate;

  guint synth_oscillator_mode;
  
  gdouble frequency;
  gdouble phase;
  gdouble volume;
  
  guint frame_count;
  guint offset;
};

GType ags_synth_util_get_type(void);

AgsSynthUtil* ags_synth_util_alloc();

gpointer ags_synth_util_copy(AgsSynthUtil *ptr);
void ags_synth_util_free(AgsSynthUtil *ptr);

gpointer ags_synth_util_get_source(AgsSynthUtil *synth_util);
void ags_synth_util_set_source(AgsSynthUtil *synth_util,
			       gpointer source);

guint ags_synth_util_get_source_stride(AgsSynthUtil *synth_util);
void ags_synth_util_set_source_stride(AgsSynthUtil *synth_util,
				      guint source_stride);

guint ags_synth_util_get_buffer_length(AgsSynthUtil *synth_util);
void ags_synth_util_set_buffer_length(AgsSynthUtil *synth_util,
				      guint buffer_length);

guint ags_synth_util_get_audio_buffer_util_format(AgsSynthUtil *synth_util);
void ags_synth_util_set_audio_buffer_util_format(AgsSynthUtil *synth_util,
						 guint audio_buffer_util_format);

guint ags_synth_util_get_samplerate(AgsSynthUtil *synth_util);
void ags_synth_util_set_samplerate(AgsSynthUtil *synth_util,
				   guint samplerate);

guint ags_synth_util_get_synth_oscillator_mode(AgsSynthUtil *synth_util);
void ags_synth_util_set_synth_oscillator_mode(AgsSynthUtil *synth_util,
					      guint synth_oscillator_mode);

gdouble ags_synth_util_get_frequency(AgsSynthUtil *synth_util);
void ags_synth_util_set_frequency(AgsSynthUtil *synth_util,
				  gdouble frequency);

gdouble ags_synth_util_get_phase(AgsSynthUtil *synth_util);
void ags_synth_util_set_phase(AgsSynthUtil *synth_util,
			      gdouble phase);

gdouble ags_synth_util_get_volume(AgsSynthUtil *synth_util);
void ags_synth_util_set_volume(AgsSynthUtil *synth_util,
			       gdouble volume);

guint ags_synth_util_get_frame_count(AgsSynthUtil *synth_util);
void ags_synth_util_set_frame_count(AgsSynthUtil *synth_util,
				      guint frame_count);

guint ags_synth_util_get_offset(AgsSynthUtil *synth_util);
void ags_synth_util_set_offset(AgsSynthUtil *synth_util,
			       guint offset);

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
void ags_synth_util_compute_sin_s8(AgsSynthUtil *synth_util);
void ags_synth_util_compute_sin_s16(AgsSynthUtil *synth_util);
void ags_synth_util_compute_sin_s24(AgsSynthUtil *synth_util);
void ags_synth_util_compute_sin_s32(AgsSynthUtil *synth_util);
void ags_synth_util_compute_sin_s64(AgsSynthUtil *synth_util);
void ags_synth_util_compute_sin_float(AgsSynthUtil *synth_util);
void ags_synth_util_compute_sin_double(AgsSynthUtil *synth_util);
void ags_synth_util_compute_sin_complex(AgsSynthUtil *synth_util);

void ags_synth_util_compute_sin(AgsSynthUtil *synth_util);

G_DEPRECATED_FOR(ags_synth_util_compute_sin_s8)
void ags_synth_util_sin_s8(gint8 *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_sin_s16)
void ags_synth_util_sin_s16(gint16 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_sin_s24)
void ags_synth_util_sin_s24(gint32 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_sin_s32)
void ags_synth_util_sin_s32(gint32 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_sin_s64)
void ags_synth_util_sin_s64(gint64 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_sin_float)
void ags_synth_util_sin_float(gfloat *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_sin_double)
void ags_synth_util_sin_double(gdouble *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_sin_complex)
void ags_synth_util_sin_complex(AgsComplex *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames);

G_DEPRECATED_FOR(ags_synth_util_compute_sin)
void ags_synth_util_sin(void *buffer,
			gdouble freq, gdouble phase, gdouble volume,
			guint samplerate, guint audio_buffer_util_format,
			guint offset, guint n_frames);

/* sawtooth oscillator */
void ags_synth_util_compute_sawtooth_s8(AgsSynthUtil *synth_util);
void ags_synth_util_compute_sawtooth_s16(AgsSynthUtil *synth_util);
void ags_synth_util_compute_sawtooth_s24(AgsSynthUtil *synth_util);
void ags_synth_util_compute_sawtooth_s32(AgsSynthUtil *synth_util);
void ags_synth_util_compute_sawtooth_s64(AgsSynthUtil *synth_util);
void ags_synth_util_compute_sawtooth_float(AgsSynthUtil *synth_util);
void ags_synth_util_compute_sawtooth_double(AgsSynthUtil *synth_util);
void ags_synth_util_compute_sawtooth_complex(AgsSynthUtil *synth_util);

void ags_synth_util_compute_sawtooth(AgsSynthUtil *synth_util);

G_DEPRECATED_FOR(ags_synth_util_compute_sawtooth_s8)
void ags_synth_util_sawtooth_s8(gint8 *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_sawtooth_s16)
void ags_synth_util_sawtooth_s16(gint16 *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_sawtooth_s24)
void ags_synth_util_sawtooth_s24(gint32 *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_sawtooth_s32)
void ags_synth_util_sawtooth_s32(gint32 *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_sawtooth_s64)
void ags_synth_util_sawtooth_s64(gint64 *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_sawtooth_float)
void ags_synth_util_sawtooth_float(gfloat *buffer,
				   gdouble freq, gdouble phase, gdouble volume,
				   guint samplerate,
				   guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_sawtooth_double)
void ags_synth_util_sawtooth_double(gdouble *buffer,
				    gdouble freq, gdouble phase, gdouble volume,
				    guint samplerate,
				    guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_sawtooth_complex)
void ags_synth_util_sawtooth_complex(AgsComplex *buffer,
				     gdouble freq, gdouble phase, gdouble volume,
				     guint samplerate,
				     guint offset, guint n_frames);

G_DEPRECATED_FOR(ags_synth_util_compute_sawtooth)
void ags_synth_util_sawtooth(void *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate, guint audio_buffer_util_format,
			     guint offset, guint n_frames);

/* triangle oscillator */
void ags_synth_util_compute_triangle_s8(AgsSynthUtil *synth_util);
void ags_synth_util_compute_triangle_s16(AgsSynthUtil *synth_util);
void ags_synth_util_compute_triangle_s24(AgsSynthUtil *synth_util);
void ags_synth_util_compute_triangle_s32(AgsSynthUtil *synth_util);
void ags_synth_util_compute_triangle_s64(AgsSynthUtil *synth_util);
void ags_synth_util_compute_triangle_float(AgsSynthUtil *synth_util);
void ags_synth_util_compute_triangle_double(AgsSynthUtil *synth_util);
void ags_synth_util_compute_triangle_complex(AgsSynthUtil *synth_util);

void ags_synth_util_compute_triangle(AgsSynthUtil *synth_util);

G_DEPRECATED_FOR(ags_synth_util_compute_triangle_s8)
void ags_synth_util_triangle_s8(gint8 *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_triangle_s16)
void ags_synth_util_triangle_s16(gint16 *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_triangle_s24)
void ags_synth_util_triangle_s24(gint32 *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_triangle_s32)
void ags_synth_util_triangle_s32(gint32 *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_triangle_s64)
void ags_synth_util_triangle_s64(gint64 *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_triangle_float)
void ags_synth_util_triangle_float(gfloat *buffer,
				   gdouble freq, gdouble phase, gdouble volume,
				   guint samplerate,
				   guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_triangle_double)
void ags_synth_util_triangle_double(gdouble *buffer,
				    gdouble freq, gdouble phase, gdouble volume,
				    guint samplerate,
				    guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_triangle_complex)
void ags_synth_util_triangle_complex(AgsComplex *buffer,
				     gdouble freq, gdouble phase, gdouble volume,
				     guint samplerate,
				     guint offset, guint n_frames);

G_DEPRECATED_FOR(ags_synth_util_compute_triangle)
void ags_synth_util_triangle(void *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate, guint audio_buffer_util_format,
			     guint offset, guint n_frames);

/* square oscillator */
void ags_synth_util_compute_square_s8(AgsSynthUtil *synth_util);
void ags_synth_util_compute_square_s16(AgsSynthUtil *synth_util);
void ags_synth_util_compute_square_s24(AgsSynthUtil *synth_util);
void ags_synth_util_compute_square_s32(AgsSynthUtil *synth_util);
void ags_synth_util_compute_square_s64(AgsSynthUtil *synth_util);
void ags_synth_util_compute_square_float(AgsSynthUtil *synth_util);
void ags_synth_util_compute_square_double(AgsSynthUtil *synth_util);
void ags_synth_util_compute_square_complex(AgsSynthUtil *synth_util);

void ags_synth_util_compute_square(AgsSynthUtil *synth_util);

G_DEPRECATED_FOR(ags_synth_util_compute_square_s8)
void ags_synth_util_square_s8(gint8 *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_square_s16)
void ags_synth_util_square_s16(gint16 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_square_s24)
void ags_synth_util_square_s24(gint32 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_square_s32)
void ags_synth_util_square_s32(gint32 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_square_s64)
void ags_synth_util_square_s64(gint64 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_square_float)
void ags_synth_util_square_float(gfloat *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_square_double)
void ags_synth_util_square_double(gdouble *buffer,
				  gdouble freq, gdouble phase, gdouble volume,
				  guint samplerate,
				  guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_square_complex)
void ags_synth_util_square_complex(AgsComplex *buffer,
				   gdouble freq, gdouble phase, gdouble volume,
				   guint samplerate,
				   guint offset, guint n_frames);

G_DEPRECATED_FOR(ags_synth_util_compute_square)
void ags_synth_util_square(void *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate, guint audio_buffer_util_format,
			   guint offset, guint n_frames);

/* impulse oscillator */
void ags_synth_util_compute_impulse_s8(AgsSynthUtil *synth_util);
void ags_synth_util_compute_impulse_s16(AgsSynthUtil *synth_util);
void ags_synth_util_compute_impulse_s24(AgsSynthUtil *synth_util);
void ags_synth_util_compute_impulse_s32(AgsSynthUtil *synth_util);
void ags_synth_util_compute_impulse_s64(AgsSynthUtil *synth_util);
void ags_synth_util_compute_impulse_float(AgsSynthUtil *synth_util);
void ags_synth_util_compute_impulse_double(AgsSynthUtil *synth_util);
void ags_synth_util_compute_impulse_complex(AgsSynthUtil *synth_util);

void ags_synth_util_compute_impulse(AgsSynthUtil *synth_util);

G_DEPRECATED_FOR(ags_synth_util_compute_impulse_s8)
void ags_synth_util_impulse_s8(gint8 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_impulse_s16)
void ags_synth_util_impulse_s16(gint16 *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_impulse_s24)
void ags_synth_util_impulse_s24(gint32 *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_impulse_s32)
void ags_synth_util_impulse_s32(gint32 *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_impulse_s64)
void ags_synth_util_impulse_s64(gint64 *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_impulse_float)
void ags_synth_util_impulse_float(gfloat *buffer,
				  gdouble freq, gdouble phase, gdouble volume,
				  guint samplerate,
				  guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_impulse_double)
void ags_synth_util_impulse_double(gdouble *buffer,
				   gdouble freq, gdouble phase, gdouble volume,
				   guint samplerate,
				   guint offset, guint n_frames);
G_DEPRECATED_FOR(ags_synth_util_compute_impulse_complex)
void ags_synth_util_impulse_complex(AgsComplex *buffer,
				    gdouble freq, gdouble phase, gdouble volume,
				    guint samplerate,
				    guint offset, guint n_frames);

G_DEPRECATED_FOR(ags_synth_util_compute_impulse)
void ags_synth_util_impulse(void *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate, guint audio_buffer_util_format,
			    guint offset, guint n_frames);

G_END_DECLS

#endif /*__AGS_SYNTH_UTIL_H__*/
