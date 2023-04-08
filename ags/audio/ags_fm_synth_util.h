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

#ifndef __AGS_FM_SYNTH_UTIL_H__
#define __AGS_FM_SYNTH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_synth_enums.h>

G_BEGIN_DECLS

#define AGS_TYPE_FM_SYNTH_UTIL         (ags_fm_synth_util_get_type())

#define AGS_FM_SYNTH_UTIL_DEFAULT_FREQUENCY (440.0)
#define AGS_FM_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY (12.0)
#define AGS_FM_SYNTH_UTIL_DEFAULT_LFO_DEPTH (0.0)
#define AGS_FM_SYNTH_UTIL_DEFAULT_TUNING (0.0)

typedef struct _AgsFMSynthUtil AgsFMSynthUtil;

struct _AgsFMSynthUtil
{
  gpointer source;
  guint source_stride;
  
  guint buffer_length;
  guint format;
  guint samplerate;

  guint synth_oscillator_mode;
  
  gdouble frequency;
  gdouble phase;
  gdouble volume;

  guint lfo_oscillator_mode;

  gdouble lfo_frequency;
  gdouble lfo_depth;
  gdouble tuning;
  
  guint frame_count;
  guint offset;
};

GType ags_fm_synth_util_get_type(void);

AgsFMSynthUtil* ags_fm_synth_util_alloc();

gpointer ags_fm_synth_util_copy(AgsFMSynthUtil *ptr);
void ags_fm_synth_util_free(AgsFMSynthUtil *ptr);

gpointer ags_fm_synth_util_get_source(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_set_source(AgsFMSynthUtil *fm_synth_util,
				  gpointer source);

guint ags_fm_synth_util_get_source_stride(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_set_source_stride(AgsFMSynthUtil *fm_synth_util,
					 guint source_stride);

guint ags_fm_synth_util_get_buffer_length(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_set_buffer_length(AgsFMSynthUtil *fm_synth_util,
					 guint buffer_length);

guint ags_fm_synth_util_get_format(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_set_format(AgsFMSynthUtil *fm_synth_util,
				  guint format);

guint ags_fm_synth_util_get_samplerate(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_set_samplerate(AgsFMSynthUtil *fm_synth_util,
				      guint samplerate);

guint ags_fm_synth_util_get_synth_oscillator_mode(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_set_synth_oscillator_mode(AgsFMSynthUtil *fm_synth_util,
						 guint synth_oscillator_mode);

gdouble ags_fm_synth_util_get_frequency(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_set_frequency(AgsFMSynthUtil *fm_synth_util,
				     gdouble frequency);

gdouble ags_fm_synth_util_get_phase(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_set_phase(AgsFMSynthUtil *fm_synth_util,
				 gdouble phase);

gdouble ags_fm_synth_util_get_volume(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_set_volume(AgsFMSynthUtil *fm_synth_util,
				  gdouble volume);

guint ags_fm_synth_util_get_lfo_oscillator_mode(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_set_lfo_oscillator_mode(AgsFMSynthUtil *fm_synth_util,
					       guint lfo_oscillator_mode);

gdouble ags_fm_synth_util_get_lfo_frequency(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_set_lfo_frequency(AgsFMSynthUtil *fm_synth_util,
					 gdouble lfo_frequency);

gdouble ags_fm_synth_util_get_lfo_depth(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_set_lfo_depth(AgsFMSynthUtil *fm_synth_util,
				     gdouble lfo_depth);

gdouble ags_fm_synth_util_get_tuning(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_set_tuning(AgsFMSynthUtil *fm_synth_util,
				  gdouble tuning);

guint ags_fm_synth_util_get_frame_count(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_set_frame_count(AgsFMSynthUtil *fm_synth_util,
				       guint frame_count);

guint ags_fm_synth_util_get_offset(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_set_offset(AgsFMSynthUtil *fm_synth_util,
				  guint offset);

/* fm sin oscillator */
void ags_fm_synth_util_compute_sin_s8(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_sin_s16(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_sin_s24(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_sin_s32(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_sin_s64(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_sin_float(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_sin_double(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_sin_complex(AgsFMSynthUtil *fm_synth_util);

void ags_fm_synth_util_compute_sin(AgsFMSynthUtil *fm_synth_util);

/* fm sawtooth oscillator */
void ags_fm_synth_util_compute_sawtooth_s8(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_sawtooth_s16(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_sawtooth_s24(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_sawtooth_s32(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_sawtooth_s64(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_sawtooth_float(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_sawtooth_double(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_sawtooth_complex(AgsFMSynthUtil *fm_synth_util);

void ags_fm_synth_util_compute_sawtooth(AgsFMSynthUtil *fm_synth_util);

/* fm triangle oscillator */
void ags_fm_synth_util_compute_triangle_s8(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_triangle_s16(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_triangle_s24(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_triangle_s32(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_triangle_s64(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_triangle_float(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_triangle_double(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_triangle_complex(AgsFMSynthUtil *fm_synth_util);

void ags_fm_synth_util_compute_triangle(AgsFMSynthUtil *fm_synth_util);

/* fm square oscillator */
void ags_fm_synth_util_compute_square_s8(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_square_s16(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_square_s24(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_square_s32(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_square_s64(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_square_float(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_square_double(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_square_complex(AgsFMSynthUtil *fm_synth_util);

void ags_fm_synth_util_compute_square(AgsFMSynthUtil *fm_synth_util);

/* fm impulse oscillator */
void ags_fm_synth_util_compute_impulse_s8(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_impulse_s16(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_impulse_s24(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_impulse_s32(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_impulse_s64(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_impulse_float(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_impulse_double(AgsFMSynthUtil *fm_synth_util);
void ags_fm_synth_util_compute_impulse_complex(AgsFMSynthUtil *fm_synth_util);

void ags_fm_synth_util_compute_impulse(AgsFMSynthUtil *fm_synth_util);

G_END_DECLS

#endif /*__AGS_FM_SYNTH_UTIL_H__*/
