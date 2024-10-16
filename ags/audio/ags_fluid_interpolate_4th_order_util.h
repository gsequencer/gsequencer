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

#ifndef __AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL_H__
#define __AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL         (ags_fluid_interpolate_4th_order_util_get_type())
#define AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL(ptr) ((AgsFluidInterpolate4thOrderUtil *)(ptr))

#define AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL_INITIALIZER ((AgsFluidInterpolate4thOrderUtil) { \
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

typedef struct _AgsFluidInterpolate4thOrderUtil AgsFluidInterpolate4thOrderUtil;

struct _AgsFluidInterpolate4thOrderUtil
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

GType ags_fluid_interpolate_4th_order_util_get_type(void);

AgsFluidInterpolate4thOrderUtil* ags_fluid_interpolate_4th_order_util_alloc();

gpointer ags_fluid_interpolate_4th_order_util_copy(AgsFluidInterpolate4thOrderUtil *ptr);
void ags_fluid_interpolate_4th_order_util_free(AgsFluidInterpolate4thOrderUtil *ptr);

gpointer ags_fluid_interpolate_4th_order_util_get_source(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_source(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
						     gpointer source);

guint ags_fluid_interpolate_4th_order_util_get_source_stride(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_source_stride(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							    guint source_stride);

gpointer ags_fluid_interpolate_4th_order_util_get_destination(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_destination(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							  gpointer destination);

guint ags_fluid_interpolate_4th_order_util_get_destination_stride(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_destination_stride(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
								 guint destination_stride);

guint ags_fluid_interpolate_4th_order_util_get_buffer_length(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_buffer_length(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							    guint buffer_length);

AgsSoundcardFormat ags_fluid_interpolate_4th_order_util_get_format(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_format(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
						     AgsSoundcardFormat format);

guint ags_fluid_interpolate_4th_order_util_get_samplerate(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_samplerate(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							 guint samplerate);

gdouble ags_fluid_interpolate_4th_order_util_get_base_key(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_base_key(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
						       gdouble base_key);

gdouble ags_fluid_interpolate_4th_order_util_get_tuning(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_tuning(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
						     gdouble tuning);

gdouble ags_fluid_interpolate_4th_order_util_get_phase_increment(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_phase_increment(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							      gdouble phase_increment);

gboolean ags_fluid_interpolate_4th_order_util_get_vibrato_enabled(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_vibrato_enabled(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							      gboolean vibrato_enabled);

gdouble ags_fluid_interpolate_4th_order_util_get_vibrato_gain(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_vibrato_gain(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							   gdouble vibrato_gain);

gdouble ags_fluid_interpolate_4th_order_util_get_vibrato_lfo_depth(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_vibrato_lfo_depth(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
								gdouble vibrato_lfo_depth);

gdouble ags_fluid_interpolate_4th_order_util_get_vibrato_lfo_freq(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_vibrato_lfo_freq(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							       gdouble vibrato_lfo_freq);

gdouble ags_fluid_interpolate_4th_order_util_get_vibrato_tuning(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_vibrato_tuning(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							     gdouble vibrato_tuning);

guint ags_fluid_interpolate_4th_order_util_get_frame_count(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_frame_count(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							  guint frame_count);

guint ags_fluid_interpolate_4th_order_util_get_offset(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_offset(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
						     guint offset);

gboolean ags_fluid_interpolate_4th_order_util_get_note_256th_mode(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_note_256th_mode(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							      gboolean note_256th_mode);

guint ags_fluid_interpolate_4th_order_util_get_offset_256th(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_offset_256th(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
							   guint offset_256th);

void ags_fluid_interpolate_4th_order_util_pitch_s8(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_pitch_s16(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_pitch_s24(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_pitch_s32(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_pitch_s64(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_pitch_float(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_pitch_double(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_pitch_complex(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);

void ags_fluid_interpolate_4th_order_util_pitch(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);

G_END_DECLS

#endif /*__AGS_FLUID_INTERPOLATE_4TH_ORDER_UTIL_H__*/
