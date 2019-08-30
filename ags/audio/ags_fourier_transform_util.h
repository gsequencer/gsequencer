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

#include <math.h>
#include <complex.h>

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S8_FRAME(buffer, channels, n, buffer_length, retval) { complex z; \
    gdouble h;								\
    gdouble k;								\
    gdouble r;								\
									\
    static const gdouble normalize_factor = M_PI / exp2(7.0);		\
									\
    k = (gdouble) n;							\
    r = (gdouble) n;							\
									\
    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);		\
									\
    z = (buffer[0] * normalize_factor) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / buffer_length); \
    									\
    ags_complex_set(retval[0], z); }

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S16_FRAME(buffer, channels, n, buffer_length, retval)  { complex z; \
    gdouble h;								\
    gdouble k;								\
    gdouble r;								\
									\
    static const gdouble normalize_factor = M_PI / exp2(15.0);		\
									\
    k = (gdouble) n;							\
    r = (gdouble) n;							\
									\
    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);		\
									\
    z = (buffer[0] * normalize_factor) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / buffer_length); \
    									\
    ags_complex_set(retval[0], z); }

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S24_FRAME(buffer, channels, n, buffer_length, retval) { complex z; \
    gdouble h;								\
    gdouble k;								\
    gdouble r;								\
									\
    static const gdouble normalize_factor = M_PI / exp2(23.0);		\
									\
    k = (gdouble) n;							\
    r = (gdouble) n;							\
									\
    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);		\
									\
    z = (buffer[0] * normalize_factor) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / buffer_length); \
    									\
    ags_complex_set(retval[0], z); }

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S32_FRAME(buffer, channels, n, buffer_length, retval) { complex z; \
    gdouble h;								\
    gdouble k;								\
    gdouble r;								\
									\
    static const gdouble normalize_factor = M_PI / exp2(31.0);		\
									\
    k = (gdouble) n;							\
    r = (gdouble) n;							\
									\
    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);		\
									\
    z = (buffer[0] * normalize_factor) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / buffer_length); \
    									\
    ags_complex_set(retval[0], z); }

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S64_FRAME(buffer, channels, n, buffer_length, retval) { complex z; \
    gdouble h;								\
    gdouble k;								\
    gdouble r;								\
									\
    static const long double normalize_factor = M_PI / exp2(63.0);	\
									\
    k = (gdouble) n;							\
    r = (gdouble) n;							\
									\
    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);		\
									\
    z = (buffer[0] * normalize_factor) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / buffer_length); \
    									\
    ags_complex_set(retval[0], z); }

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_FLOAT_FRAME(buffer, channels, n, buffer_length, retval) { complex z; \
    gdouble h;								\
    gdouble k;								\
    gdouble r;								\
									\
    k = (gdouble) n;							\
    r = (gdouble) n;							\
									\
    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);		\
									\
    z = (buffer[0] * M_PI) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / buffer_length); \
    									\
    ags_complex_set(retval[0], z); }

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_DOUBLE_FRAME(buffer, channels, n, buffer_length, retval) { complex z; \
    gdouble h;								\
    gdouble k;								\
    gdouble r;								\
									\
    k = (gdouble) n;							\
    r = (gdouble) n;							\
									\
    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);		\
									\
    z = (buffer[0] * M_PI) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / buffer_length); \
    									\
    ags_complex_set(retval[0], z); }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S8_FRAME(buffer, channels, n, buffer_length, retval) { complex z; \
    gdouble k;								\
    gdouble y;								\
									\
    static const gdouble scale = 127.0 / M_PI;				\
									\
    z = ags_complex_get(buffer);					\
									\
    k = (gdouble) n;							\
									\
    y = (1.0 / buffer_length) * creal(z * cexp(I * 2.0 * M_PI * k * n / buffer_length)); \
									\
    retval[0][0] = scale * y; }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S16_FRAME(buffer, channels, n, buffer_length, retval) { complex z; \
    gdouble k;								\
    gdouble y;								\
									\
    static const gdouble scale = 32767.0 / M_PI;			\
									\
    z = ags_complex_get(buffer);					\
									\
    k = (gdouble) n;							\
									\
    y = (1.0 / buffer_length) * creal(z * cexp(I * 2.0 * M_PI * k * n / buffer_length)); \
									\
    retval[0][0] = scale * y; }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S24_FRAME(buffer, channels, n, buffer_length, retval) { complex z; \
    gdouble k;								\
    gdouble y;								\
									\
    static const gdouble scale = 8388607.0 / M_PI;			\
									\
    z = ags_complex_get(buffer);					\
									\
    k = (gdouble) n;							\
									\
    y = (1.0 / buffer_length) * creal(z * cexp(I * 2.0 * M_PI * k * n / buffer_length)); \
									\
    retval[0][0] = scale * y; }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S32_FRAME(buffer, channels, n, buffer_length, retval) { complex z; \
    gdouble k;								\
    gdouble y;								\
									\
    static const gdouble scale = 214748363.0 / M_PI;			\
									\
    z = ags_complex_get(buffer);					\
									\
    k = (gdouble) n;							\
									\
    y = (1.0 / buffer_length) * creal(z * cexp(I * 2.0 * M_PI * k * n / buffer_length)); \
									\
    retval[0][0] = scale * y; }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S64_FRAME(buffer, channels, n, buffer_length, retval) { complex z; \
    gdouble k;								\
    gdouble y;								\
									\
    static const long double scale = 9223372036854775807.0 / M_PI;	\
									\
    z = ags_complex_get(buffer);					\
									\
    k = (gdouble) n;							\
									\
    y = (1.0 / buffer_length) * creal(z * cexp(I * 2.0 * M_PI * k * n / buffer_length)); \
									\
    retval[0][0] = scale * y; }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_FLOAT_FRAME(buffer, channels, n, buffer_length, retval) { complex z; \
    gdouble k;								\
    gdouble y;								\
									\
    z = ags_complex_get(buffer);					\
									\
    k = (gdouble) n;							\
									\
    y = (1.0 / buffer_length) * creal(z * cexp(I * 2.0 * M_PI * k * n / buffer_length)); \
									\
    retval[0][0] = y / M_PI; }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_DOUBLE_FRAME(buffer, channels, n, buffer_length, retval) { complex z; \
    gdouble k;								\
    gdouble y;								\
									\
    z = ags_complex_get(buffer);					\
									\
    k = (gdouble) n;							\
									\
    y = (1.0 / buffer_length) * creal(z * cexp(I * 2.0 * M_PI * k * n / buffer_length)); \
									\
    retval[0][0] = y / M_PI; }

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
