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

typedef struct _AgsFluidInterpolate4thOrderUtil AgsFluidInterpolate4thOrderUtil;

struct _AgsFluidInterpolate4thOrderUtil
{
  gpointer source;
  guint source_stride;

  gpointer destination;
  guint destination_stride;

  guint buffer_length;
  guint format;
  guint samplerate;

  gdouble base_key;
  gdouble tuning;

  gdouble phase_increment;
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

guint ags_fluid_interpolate_4th_order_util_get_format(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util);
void ags_fluid_interpolate_4th_order_util_set_format(AgsFluidInterpolate4thOrderUtil *fluid_interpolate_4th_order_util,
						     guint format);

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
