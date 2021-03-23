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

#ifndef __AGS_LINEAR_INTERPOLATE_UTIL_H__
#define __AGS_LINEAR_INTERPOLATE_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS

void ags_linear_interpolate_util_fill_s8(gint8 *buffer, guint channels,
					 guint buffer_length,
					 gdouble factor,
					 gint8 **output_buffer,
					 guint *output_buffer_length);
void ags_linear_interpolate_util_fill_s16(gint16 *buffer, guint channels,
					  guint buffer_length,
					  gdouble factor,
					  gint16 **output_buffer,
					  guint *output_buffer_length);
void ags_linear_interpolate_util_fill_s24(gint32 *buffer, guint channels,
					  guint buffer_length,
					  gdouble factor,
					  gint32 **output_buffer,
					  guint *output_buffer_length);
void ags_linear_interpolate_util_fill_s32(gint32 *buffer, guint channels,
					  guint buffer_length,
					  gdouble factor,
					  gint32 **output_buffer,
					  guint *output_buffer_length);
void ags_linear_interpolate_util_fill_s64(gint64 *buffer, guint channels,
					  guint buffer_length,
					  gdouble factor,
					  gint64 **output_buffer,
					  guint *output_buffer_length);
void ags_linear_interpolate_util_fill_float(gfloat *buffer, guint channels,
					    guint buffer_length,
					    gdouble factor,
					    gfloat **output_buffer,
					    guint *output_buffer_length);
void ags_linear_interpolate_util_fill_double(gdouble *buffer, guint channels,
					     guint buffer_length,
					     gdouble factor,
					     gdouble **output_buffer,
					     guint *output_buffer_length);
void ags_linear_interpolate_util_fill_complex(AgsComplex *buffer, guint channels,
					      guint buffer_length,
					      gdouble factor,
					      AgsComplex **output_buffer,
					      guint *output_buffer_length);

G_END_DECLS

#endif /*__AGS_LINEAR_INTERPOLATE_UTIL_H__*/
