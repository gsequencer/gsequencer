/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_FLUID_INTERPOLATE_LINEAR_UTIL_H__
#define __AGS_FLUID_INTERPOLATE_LINEAR_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL         (ags_fluid_interpolate_linear_util_get_type())
#define AGS_FLUID_INTERPOLATE_LINEAR_UTIL(ptr) ((AgsFluidInterpolateLinearUtil *)(ptr))

typedef struct _AgsFluidInterpolateLinearUtil AgsFluidInterpolateLinearUtil;

struct _AgsFluidInterpolateLinearUtil
{
  gpointer source;
  guint source_stride;

  gpointer destination;
  guint destination_stride;

  guint buffer_length;
  AgsSoundcardFormat format;
  guint samplerate;

  gdouble base_key;
  gdouble tuning;

  gdouble phase_increment;

  gboolean vibrato_enabled;

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

GType ags_fluid_interpolate_linear_util_get_type(void);

AgsFluidInterpolateLinearUtil* ags_fluid_interpolate_linear_util_alloc();

gpointer ags_fluid_interpolate_linear_util_copy(AgsFluidInterpolateLinearUtil *ptr);
void ags_fluid_interpolate_linear_util_free(AgsFluidInterpolateLinearUtil *ptr);

gpointer ags_fluid_interpolate_linear_util_get_source(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_source(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
						  gpointer source);

guint ags_fluid_interpolate_linear_util_get_source_stride(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_source_stride(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
							 guint source_stride);

gpointer ags_fluid_interpolate_linear_util_get_destination(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_destination(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
						       gpointer destination);

guint ags_fluid_interpolate_linear_util_get_destination_stride(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_destination_stride(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
							      guint destination_stride);

guint ags_fluid_interpolate_linear_util_get_buffer_length(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_buffer_length(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
							 guint buffer_length);

AgsSoundcardFormat ags_fluid_interpolate_linear_util_get_format(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_format(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
						  AgsSoundcardFormat format);

guint ags_fluid_interpolate_linear_util_get_samplerate(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_samplerate(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
						      guint samplerate);

gdouble ags_fluid_interpolate_linear_util_get_base_key(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_base_key(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
						    gdouble base_key);

gdouble ags_fluid_interpolate_linear_util_get_tuning(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_tuning(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
						  gdouble tuning);

gdouble ags_fluid_interpolate_linear_util_get_phase_increment(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_phase_increment(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
							   gdouble phase_increment);

gboolean ags_fluid_interpolate_linear_util_get_vibrato_enabled(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_vibrato_enabled(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
							   gboolean vibrato_enabled);

gdouble ags_fluid_interpolate_linear_util_get_vibrato_gain(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_vibrato_gain(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
							gdouble vibrato_gain);

gdouble ags_fluid_interpolate_linear_util_get_vibrato_lfo_depth(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_vibrato_lfo_depth(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
							     gdouble vibrato_lfo_depth);

gdouble ags_fluid_interpolate_linear_util_get_vibrato_lfo_freq(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_vibrato_lfo_freq(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
							    gdouble vibrato_lfo_freq);

gdouble ags_fluid_interpolate_linear_util_get_vibrato_tuning(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_vibrato_tuning(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
							  gdouble vibrato_tuning);

G_DEPRECATED_FOR(ags_fluid_interpolate_linear_util_get_offset)
guint ags_fluid_interpolate_linear_util_get_vibrato_lfo_offset(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
G_DEPRECATED_FOR(ags_fluid_interpolate_linear_util_set_offset)
void ags_fluid_interpolate_linear_util_set_vibrato_lfo_offset(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
							      guint vibrato_lfo_offset);

guint ags_fluid_interpolate_linear_util_get_frame_count(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_frame_count(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
						       guint frame_count);

guint ags_fluid_interpolate_linear_util_get_offset(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_offset(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
						  guint offset);

gboolean ags_fluid_interpolate_linear_util_get_note_256th_mode(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_note_256th_mode(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
							   gboolean note_256th_mode);

guint ags_fluid_interpolate_linear_util_get_offset_256th(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_offset_256th(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
							guint offset_256th);

void ags_fluid_interpolate_linear_util_pitch_s8(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_pitch_s16(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_pitch_s24(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_pitch_s32(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_pitch_s64(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_pitch_float(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_pitch_double(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_pitch_complex(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_pitch(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);

G_END_DECLS

#endif /*__AGS_FLUID_INTERPOLATE_LINEAR_UTIL_H__*/
