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

#ifndef __AGS_FLUID_INTERPOLATE_NONE_UTIL_H__
#define __AGS_FLUID_INTERPOLATE_NONE_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL         (ags_fluid_interpolate_none_util_get_type())
#define AGS_FLUID_INTERPOLATE_NONE_UTIL(ptr) ((AgsFluidInterpolateNoneUtil *)(ptr))

#define AGS_FLUID_INTERPOLATE_NONE_UTIL_INITIALIZER ((AgsFluidInterpolateNoneUtil) { \
      .source = NULL,							\
      .source_stride = 1,						\
      .destination = NULL,						\
      .destination_stride = 1,						\
      .buffer_length = 0,						\
      .format = AGS_SOUNDCARD_DEFAULT_FORMAT,				\
      .samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE,			\
      .base_key = 0.0,							\
      .tuning = 0.0,							\
      .phase_increment = 0.0,						\
      .vibrato_enabled = FALSE,						\
      .vibrato_gain = 1.0,						\
      .vibrato_lfo_depth = 1.0,						\
      .vibrato_lfo_freq = 8.172,					\
      .vibrato_tuning = 0.0,						\
      .frame_count = (AGS_SOUNDCARD_DEFAULT_SAMPLERATE / 8.172),	\
      .offset = 0,							\
      .note_256th_mode = FALSE,						\
      .offset_256th = 0 })

typedef struct _AgsFluidInterpolateNoneUtil AgsFluidInterpolateNoneUtil;

struct _AgsFluidInterpolateNoneUtil
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
  
  guint frame_count;
  guint offset;
  
  gboolean note_256th_mode;

  guint offset_256th;
};

GType ags_fluid_interpolate_none_util_get_type(void);

AgsFluidInterpolateNoneUtil* ags_fluid_interpolate_none_util_alloc();

gpointer ags_fluid_interpolate_none_util_copy(AgsFluidInterpolateNoneUtil *ptr);
void ags_fluid_interpolate_none_util_free(AgsFluidInterpolateNoneUtil *ptr);

gpointer ags_fluid_interpolate_none_util_get_source(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_source(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						gpointer source);

guint ags_fluid_interpolate_none_util_get_source_stride(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_source_stride(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						       guint source_stride);

gpointer ags_fluid_interpolate_none_util_get_destination(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_destination(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						     gpointer destination);

guint ags_fluid_interpolate_none_util_get_destination_stride(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_destination_stride(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
							    guint destination_stride);

guint ags_fluid_interpolate_none_util_get_buffer_length(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_buffer_length(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						       guint buffer_length);

AgsSoundcardFormat ags_fluid_interpolate_none_util_get_format(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_format(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						AgsSoundcardFormat format);

guint ags_fluid_interpolate_none_util_get_samplerate(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_samplerate(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						    guint samplerate);

gdouble ags_fluid_interpolate_none_util_get_base_key(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_base_key(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						  gdouble base_key);

gdouble ags_fluid_interpolate_none_util_get_tuning(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_tuning(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						gdouble tuning);

gdouble ags_fluid_interpolate_none_util_get_phase_increment(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_phase_increment(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
							 gdouble phase_increment);

gboolean ags_fluid_interpolate_none_util_get_vibrato_enabled(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_vibrato_enabled(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
							 gboolean vibrato_enabled);

gdouble ags_fluid_interpolate_none_util_get_vibrato_gain(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_vibrato_gain(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						      gdouble vibrato_gain);

gdouble ags_fluid_interpolate_none_util_get_vibrato_lfo_depth(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_vibrato_lfo_depth(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
							   gdouble vibrato_lfo_depth);

gdouble ags_fluid_interpolate_none_util_get_vibrato_lfo_freq(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_vibrato_lfo_freq(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
							  gdouble vibrato_lfo_freq);

gdouble ags_fluid_interpolate_none_util_get_vibrato_tuning(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_vibrato_tuning(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
							gdouble vibrato_tuning);

guint ags_fluid_interpolate_none_util_get_frame_count(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_frame_count(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						     guint frame_count);

guint ags_fluid_interpolate_none_util_get_offset(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_offset(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						guint offset);

gboolean ags_fluid_interpolate_none_util_get_note_256th_mode(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_note_256th_mode(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
							 gboolean note_256th_mode);

guint ags_fluid_interpolate_none_util_get_offset_256th(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_offset_256th(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						      guint offset_256th);

void ags_fluid_interpolate_none_util_pitch_s8(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_pitch_s16(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_pitch_s24(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_pitch_s32(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_pitch_s64(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_pitch_float(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_pitch_double(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_pitch_complex(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);

void ags_fluid_interpolate_none_util_pitch(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);

G_END_DECLS

#endif /*__AGS_FLUID_INTERPOLATE_NONE_UTIL_H__*/
