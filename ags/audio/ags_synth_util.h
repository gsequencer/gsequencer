/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

G_BEGIN_DECLS

#define AGS_TYPE_SYNTH_UTIL         (ags_synth_util_get_type())

#define AGS_SYNTH_UTIL_DEFAULT_FREQUENCY (440.0)

#define AGS_SYNTH_UTIL_INITIALIZER ((AgsSynthUtil) {		\
      .source = NULL,						\
      .source_stride = 1,					\
      .buffer_length = 0,					\
      .format = AGS_SOUNDCARD_DEFAULT_FORMAT,			\
      .samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE,		\
      .synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SIN,	\
      .frequency = AGS_SYNTH_UTIL_DEFAULT_FREQUENCY,		\
      .phase = 0.0,						\
      .volume = 1.0,						\
      .frame_count = (AGS_SOUNDCARD_DEFAULT_SAMPLERATE / 6.0),	\
      .offset = 0,						\
      .note_256th_mode = TRUE,					\
      .offset_256th = 0 })

typedef struct _AgsSynthUtil AgsSynthUtil;

struct _AgsSynthUtil
{
  gpointer source;
  guint source_stride;
  
  guint buffer_length;
  AgsSoundcardFormat format;
  guint samplerate;

  guint synth_oscillator_mode;
  
  gdouble frequency;
  gdouble phase;
  gdouble volume;
  
  guint frame_count;
  guint offset;

  gboolean note_256th_mode;

  guint offset_256th;
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

AgsSoundcardFormat ags_synth_util_get_format(AgsSynthUtil *synth_util);
void ags_synth_util_set_format(AgsSynthUtil *synth_util,
			       AgsSoundcardFormat format);

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

gboolean ags_synth_util_get_note_256th_mode(AgsSynthUtil *synth_util);
void ags_synth_util_set_note_256th_mode(AgsSynthUtil *synth_util,
					gboolean note_256th_mode);

guint ags_synth_util_get_offset_256th(AgsSynthUtil *synth_util);
void ags_synth_util_set_offset_256th(AgsSynthUtil *synth_util,
				     guint offset_256th);

/* zero-crossing */
guint ags_synth_util_get_xcross_count_s8(AgsSynthUtil *synth_util,
					 gint8 *buffer,
					 guint buffer_size);
guint ags_synth_util_get_xcross_count_s16(AgsSynthUtil *synth_util,
					  gint16 *buffer,
					  guint buffer_size);
guint ags_synth_util_get_xcross_count_s24(AgsSynthUtil *synth_util,
					  gint32 *buffer,
					  guint buffer_size);
guint ags_synth_util_get_xcross_count_s32(AgsSynthUtil *synth_util,
					  gint32 *buffer,
					  guint buffer_size);
guint ags_synth_util_get_xcross_count_s64(AgsSynthUtil *synth_util,
					  gint64 *buffer,
					  guint buffer_size);
guint ags_synth_util_get_xcross_count_float(AgsSynthUtil *synth_util,
					    gfloat *buffer,
					    guint buffer_size);
guint ags_synth_util_get_xcross_count_double(AgsSynthUtil *synth_util,
					     gdouble *buffer,
					     guint buffer_size);
guint ags_synth_util_get_xcross_count_complex(AgsSynthUtil *synth_util,
					      AgsComplex *buffer,
					      guint buffer_size);

guint ags_synth_util_get_xcross_count(AgsSynthUtil *synth_util,
				      void *buffer,
				      guint format,
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

G_END_DECLS

#endif /*__AGS_SYNTH_UTIL_H__*/
