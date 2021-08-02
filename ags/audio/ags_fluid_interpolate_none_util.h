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

typedef struct _AgsFluidInterpolateNoneUtil AgsFluidInterpolateNoneUtil;

struct _AgsFluidInterpolateNoneUtil
{
  gpointer source;
  guint source_stride;

  gpointer destination;
  guint destination_stride;

  guint buffer_length;
  guint format;
  guint samplerate;

  gdouble phase_increment;
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

guint ags_fluid_interpolate_none_util_get_format(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_format(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						guint format);

guint ags_fluid_interpolate_none_util_get_samplerate(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_samplerate(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
						    guint samplerate);

gdouble ags_fluid_interpolate_none_util_get_phase_increment(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_set_phase_increment(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util,
							 gdouble phase_increment);

void ags_fluid_interpolate_none_util_pitch_s8(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_pitch_s16(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_pitch_s24(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_pitch_s32(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_pitch_s64(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_pitch_float(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_pitch_double(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_pitch_complex(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);
void ags_fluid_interpolate_none_util_pitch(AgsFluidInterpolateNoneUtil *fluid_interpolate_none_util);

G_DEPRECATED_FOR(ags_fluid_interpolate_none_util_pitch_s8)
void ags_fluid_interpolate_none_util_fill_s8(gint8 *destination,
					     gint8 *source,
					     guint buffer_length,
					     gdouble phase_incr);
G_DEPRECATED_FOR(ags_fluid_interpolate_none_util_pitch_s16)
void ags_fluid_interpolate_none_util_fill_s16(gint16 *destination,
					      gint16 *source,
					      guint buffer_length,
					      gdouble phase_incr);
G_DEPRECATED_FOR(ags_fluid_interpolate_none_util_pitch_s24)
void ags_fluid_interpolate_none_util_fill_s24(gint32 *destination,
					      gint32 *source,
					      guint buffer_length,
					      gdouble phase_incr);
G_DEPRECATED_FOR(ags_fluid_interpolate_none_util_pitch_s32)
void ags_fluid_interpolate_none_util_fill_s32(gint32 *destination,
					      gint32 *source,
					      guint buffer_length,
					      gdouble phase_incr);
G_DEPRECATED_FOR(ags_fluid_interpolate_none_util_pitch_s64)
void ags_fluid_interpolate_none_util_fill_s64(gint64 *destination,
					      gint64 *source,
					      guint buffer_length,
					      gdouble phase_incr);
G_DEPRECATED_FOR(ags_fluid_interpolate_none_util_pitch_float)
void ags_fluid_interpolate_none_util_fill_float(gfloat *destination,
						gfloat *source,
						guint buffer_length,
						gdouble phase_incr);
G_DEPRECATED_FOR(ags_fluid_interpolate_none_util_pitch_double)
void ags_fluid_interpolate_none_util_fill_double(gdouble *destination,
						 gdouble *source,
						 guint buffer_length,
						 gdouble phase_incr);
G_DEPRECATED_FOR(ags_fluid_interpolate_none_util_pitch_complex)
void ags_fluid_interpolate_none_util_fill_complex(AgsComplex *destination,
						  AgsComplex *source,
						  guint buffer_length,
						  gdouble phase_incr);

G_END_DECLS

#endif /*__AGS_FLUID_INTERPOLATE_NONE_UTIL_H__*/
