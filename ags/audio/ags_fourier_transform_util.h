/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_FOURIER_TRANSFORM_UTIL_H__
#define __AGS_FOURIER_TRANSFORM_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <libags.h>

void ags_fourier_transform_util_compute_s8(gint8 *buffer, guint channels,
					   guint buffer_length,
					   guint samplerate,
					   gdouble **result);
void ags_fourier_transform_util_compute_s16(gint16 *buffer, guint channels,
					    guint buffer_length,
					    guint samplerate,
					    gdouble **result);
void ags_fourier_transform_util_compute_s24(gint32 *buffer, guint channels,
					    guint buffer_length,
					    guint samplerate,
					    gdouble **result);
void ags_fourier_transform_util_compute_s32(gint32 *buffer, guint channels,
					    guint buffer_length,
					    guint samplerate,
					    gdouble **result);
void ags_fourier_transform_util_compute_s64(gint64 *buffer, guint channels,
					    guint buffer_length,
					    guint samplerate,
					    gdouble **result);
void ags_fourier_transform_util_compute_float(gfloat *buffer, guint channels,
					      guint buffer_length,
					      guint samplerate,
					      gdouble **result);
void ags_fourier_transform_util_compute_double(gdouble *buffer, guint channels,
					       guint buffer_length,
					       guint samplerate,
					       gdouble **result);
void ags_fourier_transform_util_compute_complex(AgsComplex *buffer, guint channels,
						guint buffer_length,
						guint samplerate,
						gdouble **result);

#endif /*__AGS_FOURIER_TRANSFORM_UTIL_H__*/
