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

#ifndef __AGS_STAR_SYNTH_UTIL_H__
#define __AGS_STAR_SYNTH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_synth_enums.h>

G_BEGIN_DECLS

#define AGS_TYPE_STAR_SYNTH_UTIL         (ags_star_synth_util_get_type())

#define AGS_STAR_SYNTH_UTIL_DEFAULT_FREQUENCY (440.0)
#define AGS_STAR_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY (12.0)
#define AGS_STAR_SYNTH_UTIL_DEFAULT_LFO_DEPTH (0.0)
#define AGS_STAR_SYNTH_UTIL_DEFAULT_TUNING (0.0)

#define AGS_STAR_SYNTH_UTIL_INITIALIZER ((AgsStarSynthUtil) {		\
      .source = NULL,							\
      .source_stride = 1,						\
      .buffer_length = 0,						\
      .format = AGS_SOUNDCARD_DEFAULT_FORMAT,				\
      .samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE,			\
      .synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SIN,		\
      .frequency = AGS_STAR_SYNTH_UTIL_DEFAULT_FREQUENCY,		\
      .phase = 0.0,							\
      .volume = 1.0,							\
      .lfo_oscillator_mode = AGS_SYNTH_OSCILLATOR_SIN,			\
      .lfo_frequency = AGS_STAR_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY,	\
      .lfo_depth = AGS_STAR_SYNTH_UTIL_DEFAULT_LFO_DEPTH,		\
      .tuning = AGS_STAR_SYNTH_UTIL_DEFAULT_TUNING,			\
      .vibrato_enabled = FALSE,						\
      .vibrato_gain = 1.0,						\
      .vibrato_lfo_depth = 1.0,						\
      .vibrato_lfo_freq = 8.172,					\
      .vibrato_tuning = 0.0,						\
      .frame_count = (AGS_SOUNDCARD_DEFAULT_SAMPLERATE / 6.0),		\
      .offset = 0,							\
      .note_256th_mode = TRUE,						\
      .offset_256th = 0 })

typedef struct _AgsStarSynthUtil AgsStarSynthUtil;

struct _AgsStarSynthUtil
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

  guint lfo_oscillator_mode;

  gdouble lfo_frequency;
  gdouble lfo_depth;
  gdouble tuning;
  
  gdouble vibrato_enabled;

  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  
  guint vibrato_lfo_frame_count;
  guint vibrato_lfo_offset;

  guint frame_count;
  guint offset;

  gboolean note_256th_mode;

  guint offset_256th;
};

GType ags_star_synth_util_get_type(void);

AgsStarSynthUtil* ags_star_synth_util_alloc();

gpointer ags_star_synth_util_copy(AgsStarSynthUtil *ptr);
void ags_star_synth_util_free(AgsStarSynthUtil *ptr);

gpointer ags_star_synth_util_get_source(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_source(AgsStarSynthUtil *star_synth_util,
				  gpointer source);

guint ags_star_synth_util_get_source_stride(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_source_stride(AgsStarSynthUtil *star_synth_util,
					 guint source_stride);

guint ags_star_synth_util_get_buffer_length(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_buffer_length(AgsStarSynthUtil *star_synth_util,
					 guint buffer_length);

AgsSoundcardFormat ags_star_synth_util_get_format(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_format(AgsStarSynthUtil *star_synth_util,
				  AgsSoundcardFormat format);

guint ags_star_synth_util_get_samplerate(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_samplerate(AgsStarSynthUtil *star_synth_util,
				      guint samplerate);

guint ags_star_synth_util_get_synth_oscillator_mode(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_synth_oscillator_mode(AgsStarSynthUtil *star_synth_util,
						 guint synth_oscillator_mode);

gdouble ags_star_synth_util_get_frequency(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_frequency(AgsStarSynthUtil *star_synth_util,
				     gdouble frequency);

gdouble ags_star_synth_util_get_phase(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_phase(AgsStarSynthUtil *star_synth_util,
				 gdouble phase);

gdouble ags_star_synth_util_get_volume(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_volume(AgsStarSynthUtil *star_synth_util,
				  gdouble volume);

guint ags_star_synth_util_get_lfo_oscillator_mode(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_lfo_oscillator_mode(AgsStarSynthUtil *star_synth_util,
					       guint lfo_oscillator_mode);

gdouble ags_star_synth_util_get_lfo_frequency(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_lfo_frequency(AgsStarSynthUtil *star_synth_util,
					 gdouble lfo_frequency);

gdouble ags_star_synth_util_get_lfo_depth(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_lfo_depth(AgsStarSynthUtil *star_synth_util,
				     gdouble lfo_depth);

gdouble ags_star_synth_util_get_tuning(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_tuning(AgsStarSynthUtil *star_synth_util,
				  gdouble tuning);

gboolean ags_star_synth_util_get_vibrato_enabled(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_vibrato_enabled(AgsStarSynthUtil *star_synth_util,
					     gboolean vibrato_enabled);

gdouble ags_star_synth_util_get_vibrato_gain(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_vibrato_gain(AgsStarSynthUtil *star_synth_util,
					  gdouble vibrato_gain);

gdouble ags_star_synth_util_get_vibrato_lfo_depth(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_vibrato_lfo_depth(AgsStarSynthUtil *star_synth_util,
					       gdouble vibrato_lfo_depth);

gdouble ags_star_synth_util_get_vibrato_lfo_freq(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_vibrato_lfo_freq(AgsStarSynthUtil *star_synth_util,
					      gdouble vibrato_lfo_freq);

gdouble ags_star_synth_util_get_vibrato_tuning(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_vibrato_tuning(AgsStarSynthUtil *star_synth_util,
					    gdouble vibrato_tuning);

guint ags_star_synth_util_get_frame_count(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_frame_count(AgsStarSynthUtil *star_synth_util,
				       guint frame_count);

guint ags_star_synth_util_get_offset(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_offset(AgsStarSynthUtil *star_synth_util,
				  guint offset);

gboolean ags_star_synth_util_get_note_256th_mode(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_note_256th_mode(AgsStarSynthUtil *star_synth_util,
					   gboolean note_256th_mode);

guint ags_star_synth_util_get_offset_256th(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_set_offset_256th(AgsStarSynthUtil *star_synth_util,
					guint offset_256th);

/* star sin oscillator */
void ags_star_synth_util_compute_sin_s8(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_sin_s16(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_sin_s24(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_sin_s32(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_sin_s64(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_sin_float(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_sin_double(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_sin_complex(AgsStarSynthUtil *star_synth_util);

void ags_star_synth_util_compute_sin(AgsStarSynthUtil *star_synth_util);

/* star sawtooth oscillator */
void ags_star_synth_util_compute_sawtooth_s8(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_sawtooth_s16(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_sawtooth_s24(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_sawtooth_s32(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_sawtooth_s64(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_sawtooth_float(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_sawtooth_double(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_sawtooth_complex(AgsStarSynthUtil *star_synth_util);

void ags_star_synth_util_compute_sawtooth(AgsStarSynthUtil *star_synth_util);

/* star triangle oscillator */
void ags_star_synth_util_compute_triangle_s8(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_triangle_s16(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_triangle_s24(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_triangle_s32(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_triangle_s64(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_triangle_float(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_triangle_double(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_triangle_complex(AgsStarSynthUtil *star_synth_util);

void ags_star_synth_util_compute_triangle(AgsStarSynthUtil *star_synth_util);

/* star square oscillator */
void ags_star_synth_util_compute_square_s8(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_square_s16(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_square_s24(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_square_s32(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_square_s64(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_square_float(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_square_double(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_square_complex(AgsStarSynthUtil *star_synth_util);

void ags_star_synth_util_compute_square(AgsStarSynthUtil *star_synth_util);

/* star impulse oscillator */
void ags_star_synth_util_compute_impulse_s8(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_impulse_s16(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_impulse_s24(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_impulse_s32(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_impulse_s64(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_impulse_float(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_impulse_double(AgsStarSynthUtil *star_synth_util);
void ags_star_synth_util_compute_impulse_complex(AgsStarSynthUtil *star_synth_util);

void ags_star_synth_util_compute_impulse(AgsStarSynthUtil *star_synth_util);

G_END_DECLS

#endif /*__AGS_STAR_SYNTH_UTIL_H__*/
