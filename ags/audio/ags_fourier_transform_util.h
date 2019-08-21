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

#define AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(x) (x + 1.0)

void ags_fourier_transform_util_compute_stft_s8(gint8 *buffer, guint channels,
						guint buffer_length,
						AgsComplex **retval);
void ags_fourier_transform_util_compute_stft_s16(gint16 *buffer, guint channels,
						 guint buffer_length,
						 AgsComplex **retval);
void ags_fourier_transform_util_compute_stft_s24(gint32 *buffer, guint channels,
						 guint buffer_length,
						 AgsComplex **retval);
void ags_fourier_transform_util_compute_stft_s32(gint32 *buffer, guint channels,
						 guint buffer_length,
						 AgsComplex **retval);
void ags_fourier_transform_util_compute_stft_s64(gint64 *buffer, guint channels,
						 guint buffer_length,
						 AgsComplex **retval);
void ags_fourier_transform_util_compute_stft_float(gfloat *buffer, guint channels,
						   guint buffer_length,
						   AgsComplex **retval);
void ags_fourier_transform_util_compute_stft_double(gdouble *buffer, guint channels,
						    guint buffer_length,
						    AgsComplex **retval);

void ags_fourier_transform_util_inverse_stft_s8(AgsComplex *buffer, guint channels,
						guint buffer_length,
						gint8 **retval);
void ags_fourier_transform_util_inverse_stft_s16(AgsComplex *buffer, guint channels,
						 guint buffer_length,
						 gint16 **retval);
void ags_fourier_transform_util_inverse_stft_s24(AgsComplex *buffer, guint channels,
						 guint buffer_length,
						 gint32 **retval);
void ags_fourier_transform_util_inverse_stft_s32(AgsComplex *buffer, guint channels,
						 guint buffer_length,
						 gint32 **retval);
void ags_fourier_transform_util_inverse_stft_s64(AgsComplex *buffer, guint channels,
						 guint buffer_length,
						 gint64 **retval);
void ags_fourier_transform_util_inverse_stft_float(AgsComplex *buffer, guint channels,
						   guint buffer_length,
						   gfloat **retval);
void ags_fourier_transform_util_inverse_stft_double(AgsComplex *buffer, guint channels,
						    guint buffer_length,
						    gdouble **retval);

#endif /*__AGS_FOURIER_TRANSFORM_UTIL_H__*/
