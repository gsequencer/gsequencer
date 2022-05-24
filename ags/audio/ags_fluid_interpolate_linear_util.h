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
  guint format;
  guint samplerate;

  gdouble base_key;
  gdouble tuning;

  gdouble phase_increment;
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

guint ags_fluid_interpolate_linear_util_get_format(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util);
void ags_fluid_interpolate_linear_util_set_format(AgsFluidInterpolateLinearUtil *fluid_interpolate_linear_util,
						  guint format);

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
