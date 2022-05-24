/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_LFO_SYNTH_UTIL_H__
#define __AGS_LFO_SYNTH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_synth_enums.h>

G_BEGIN_DECLS

#define AGS_TYPE_LFO_SYNTH_UTIL         (ags_lfo_synth_util_get_type())

#define AGS_LFO_SYNTH_UTIL_DEFAULT_FREQUENCY (12.0)
#define AGS_LFO_SYNTH_UTIL_DEFAULT_LFO_DEPTH (0.0)
#define AGS_LFO_SYNTH_UTIL_DEFAULT_TUNING (0.0)

typedef struct _AgsLFOSynthUtil AgsLFOSynthUtil;

struct _AgsLFOSynthUtil
{
  gpointer source;
  guint source_stride;
  
  guint buffer_length;
  guint format;
  guint samplerate;

  guint lfo_synth_oscillator_mode;

  gdouble frequency;
  gdouble phase;

  gdouble lfo_depth;
  gdouble tuning;

  guint offset;
  guint frame_count;
};

GType ags_lfo_synth_util_get_type(void);

AgsLFOSynthUtil* ags_lfo_synth_util_alloc();

gpointer ags_lfo_synth_util_copy(AgsLFOSynthUtil *ptr);
void ags_lfo_synth_util_free(AgsLFOSynthUtil *ptr);

gpointer ags_lfo_synth_util_get_source(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_source(AgsLFOSynthUtil *lfo_synth_util,
				   gpointer source);

guint ags_lfo_synth_util_get_source_stride(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_source_stride(AgsLFOSynthUtil *lfo_synth_util,
					  guint source_stride);

guint ags_lfo_synth_util_get_buffer_length(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_buffer_length(AgsLFOSynthUtil *lfo_synth_util,
					  guint buffer_length);

guint ags_lfo_synth_util_get_format(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_format(AgsLFOSynthUtil *lfo_synth_util,
				   guint format);

guint ags_lfo_synth_util_get_samplerate(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_samplerate(AgsLFOSynthUtil *lfo_synth_util,
				       guint samplerate);

guint ags_lfo_synth_util_get_lfo_synth_oscillator_mode(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_lfo_synth_oscillator_mode(AgsLFOSynthUtil *lfo_synth_util,
						      guint lfo_synth_oscillator_mode);

gdouble ags_lfo_synth_util_get_frequency(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_frequency(AgsLFOSynthUtil *lfo_synth_util,
				      gdouble frequency);

gdouble ags_lfo_synth_util_get_phase(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_phase(AgsLFOSynthUtil *lfo_synth_util,
				  gdouble phase);

gdouble ags_lfo_synth_util_get_lfo_depth(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_lfo_depth(AgsLFOSynthUtil *lfo_synth_util,
				      gdouble lfo_depth);

gdouble ags_lfo_synth_util_get_tuning(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_tuning(AgsLFOSynthUtil *lfo_synth_util,
				   gdouble tuning);

guint ags_lfo_synth_util_get_offset(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_offset(AgsLFOSynthUtil *lfo_synth_util,
				   guint offset);

guint ags_lfo_synth_util_get_frame_count(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_frame_count(AgsLFOSynthUtil *lfo_synth_util,
					guint frame_count);

/* lfo sin oscillator */
void ags_lfo_synth_util_compute_sin_s8(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_sin_s16(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_sin_s24(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_sin_s32(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_sin_s64(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_sin_float(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_sin_double(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_sin_complex(AgsLFOSynthUtil *lfo_synth_util);

void ags_lfo_synth_util_compute_sin(AgsLFOSynthUtil *lfo_synth_util);

/* lfo sawtooth oscillator */
void ags_lfo_synth_util_compute_sawtooth_s8(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_sawtooth_s16(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_sawtooth_s24(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_sawtooth_s32(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_sawtooth_s64(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_sawtooth_float(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_sawtooth_double(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_sawtooth_complex(AgsLFOSynthUtil *lfo_synth_util);

void ags_lfo_synth_util_compute_sawtooth(AgsLFOSynthUtil *lfo_synth_util);

/* lfo triangle oscillator */
void ags_lfo_synth_util_compute_triangle_s8(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_triangle_s16(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_triangle_s24(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_triangle_s32(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_triangle_s64(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_triangle_float(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_triangle_double(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_triangle_complex(AgsLFOSynthUtil *lfo_synth_util);

void ags_lfo_synth_util_compute_triangle(AgsLFOSynthUtil *lfo_synth_util);

/* lfo square oscillator */
void ags_lfo_synth_util_compute_square_s8(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_square_s16(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_square_s24(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_square_s32(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_square_s64(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_square_float(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_square_double(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_square_complex(AgsLFOSynthUtil *lfo_synth_util);

void ags_lfo_synth_util_compute_square(AgsLFOSynthUtil *lfo_synth_util);

/* lfo impulse oscillator */
void ags_lfo_synth_util_compute_impulse_s8(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_impulse_s16(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_impulse_s24(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_impulse_s32(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_impulse_s64(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_impulse_float(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_impulse_double(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_compute_impulse_complex(AgsLFOSynthUtil *lfo_synth_util);

void ags_lfo_synth_util_compute_impulse(AgsLFOSynthUtil *lfo_synth_util);

G_END_DECLS

#endif /*__AGS_LFO_SYNTH_UTIL_H__*/
