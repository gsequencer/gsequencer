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

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S8_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { complex l_z; \
    gdouble l_h;							\
    gdouble l_k;							\
    gdouble l_r;							\
									\
    static const gdouble normalize_factor = M_PI / exp2(7.0);		\
									\
    l_k = (gdouble) x_n;						\
    l_r = (gdouble) x_n;						\
									\
    l_h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(x_n - l_r);	\
									\
    l_z = (x_buffer[0] * normalize_factor) * l_h * cexp(-1.0 * I * 2.0 * M_PI * l_k * l_r / x_buffer_length); \
    									\
    ags_complex_set(x_retval[0], l_z); }

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S16_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval)  { complex l_z; \
    gdouble l_h;							\
    gdouble l_k;							\
    gdouble l_r;							\
									\
    static const gdouble normalize_factor = M_PI / exp2(15.0);		\
									\
    l_k = (gdouble) x_n;						\
    l_r = (gdouble) x_n;						\
									\
    l_h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(x_n - l_r);	\
									\
    l_z = (x_buffer[0] * normalize_factor) * l_h * cexp(-1.0 * I * 2.0 * M_PI * l_k * l_r / x_buffer_length); \
    									\
    ags_complex_set(x_retval[0], l_z); }

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S24_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { complex l_z; \
    gdouble l_h;							\
    gdouble l_k;							\
    gdouble l_r;							\
									\
    static const gdouble normalize_factor = M_PI / exp2(23.0);		\
									\
    l_k = (gdouble) x_n;						\
    l_r = (gdouble) x_n;						\
									\
    l_h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(x_n - l_r);	\
									\
    l_z = (x_buffer[0] * normalize_factor) * l_h * cexp(-1.0 * I * 2.0 * M_PI * l_k * l_r / x_buffer_length); \
    									\
    ags_complex_set(x_retval[0], l_z); }

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S32_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { complex l_z; \
    gdouble l_h;							\
    gdouble l_k;							\
    gdouble l_r;							\
									\
    static const gdouble normalize_factor = M_PI / exp2(31.0);		\
									\
    l_k = (gdouble) x_n;						\
    l_r = (gdouble) x_n;						\
									\
    l_h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(x_n - l_r);	\
									\
    l_z = (x_buffer[0] * normalize_factor) * l_h * cexp(-1.0 * I * 2.0 * M_PI * l_k * l_r / x_buffer_length); \
    									\
    ags_complex_set(x_retval[0], l_z); }

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S64_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { complex l_z; \
    gdouble l_h;							\
    gdouble l_k;							\
    gdouble l_r;							\
									\
    static const long double normalize_factor = M_PI / exp2(63.0);	\
									\
    l_k = (gdouble) x_n;						\
    l_r = (gdouble) x_n;						\
									\
    l_h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(x_n - l_r);	\
									\
    l_z = (x_buffer[0] * normalize_factor) * l_h * cexp(-1.0 * I * 2.0 * M_PI * l_k * l_r / x_buffer_length); \
    									\
    ags_complex_set(x_retval[0], l_z); }

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_FLOAT_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { complex l_z; \
    gdouble l_h;							\
    gdouble l_k;							\
    gdouble l_r;							\
									\
    l_k = (gdouble) x_n;						\
    l_r = (gdouble) x_n;						\
									\
    l_h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(x_n - l_r);	\
									\
    l_z = (x_buffer[0] * M_PI) * l_h * cexp(-1.0 * I * 2.0 * M_PI * l_k * l_r / x_buffer_length); \
    									\
    ags_complex_set(x_retval[0], l_z); }

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_DOUBLE_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { complex l_z; \
    gdouble l_h;							\
    gdouble l_k;							\
    gdouble l_r;							\
									\
    l_k = (gdouble) x_n;						\
    l_r = (gdouble) x_n;						\
									\
    l_h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(x_n - l_r);	\
									\
    l_z = (x_buffer[0] * M_PI) * l_h * cexp(-1.0 * I * 2.0 * M_PI * l_k * l_r / x_buffer_length); \
    									\
    ags_complex_set(x_retval[0], l_z); }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S8_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { complex l_z; \
    gdouble l_k;							\
    gdouble l_y;							\
									\
    static const gdouble scale = 127.0 / M_PI;				\
									\
    l_z = ags_complex_get(x_buffer);					\
									\
    l_k = (gdouble) x_n;						\
									\
    l_y = cabs(l_z * cexp(I * 2.0 * M_PI * l_k * x_n / x_buffer_length)); \
									\
    x_retval[0][0] = scale * l_y; }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S16_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { complex l_z; \
    gdouble l_k;							\
    gdouble l_y;							\
									\
    static const gdouble scale = 32767.0 / M_PI;			\
									\
    l_z = ags_complex_get(x_buffer);					\
									\
    l_k = (gdouble) x_n;						\
									\
    l_y = cabs(l_z * cexp(I * 2.0 * M_PI * l_k * x_n / x_buffer_length)); \
									\
    x_retval[0][0] = scale * l_y; }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S24_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { complex l_z; \
    gdouble l_k;							\
    gdouble l_y;							\
									\
    static const gdouble scale = 8388607.0 / M_PI;			\
									\
    l_z = ags_complex_get(x_buffer);					\
									\
    l_k = (gdouble) x_n;						\
									\
    l_y = cabs(l_z * cexp(I * 2.0 * M_PI * l_k * x_n / x_buffer_length)); \
									\
    x_retval[0][0] = scale * l_y; }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S32_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { complex l_z; \
    gdouble l_k;							\
    gdouble l_y;							\
									\
    static const gdouble scale = 214748363.0 / M_PI;			\
									\
    l_z = ags_complex_get(x_buffer);					\
									\
    l_k = (gdouble) x_n;						\
									\
    l_y = cabs(l_z * cexp(I * 2.0 * M_PI * l_k * x_n / x_buffer_length)); \
									\
    x_retval[0][0] = scale * l_y; }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S64_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { complex l_z; \
    gdouble l_k;							\
    gdouble l_y;							\
									\
    static const long double scale = 9223372036854775807.0 / M_PI;	\
									\
    l_z = ags_complex_get(x_buffer);					\
									\
    l_k = (gdouble) x_n;						\
									\
    l_y = cabs(l_z * cexp(I * 2.0 * M_PI * l_k * x_n / x_buffer_length)); \
									\
    x_retval[0][0] = scale * l_y; }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_FLOAT_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { complex l_z; \
    gdouble l_k;							\
    gdouble l_y;							\
									\
    l_z = ags_complex_get(x_buffer);					\
									\
    l_k = (gdouble) x_n;						\
									\
    l_y = cabs(l_z * cexp(I * 2.0 * M_PI * l_k * x_n / x_buffer_length)); \
									\
    x_retval[0][0] = l_y / M_PI; }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_DOUBLE_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { complex l_z; \
    gdouble l_k;							\
    gdouble l_y;							\
									\
    l_z = ags_complex_get(x_buffer);					\
									\
    l_k = (gdouble) x_n;						\
									\
    l_y = cabs(l_z * cexp(I * 2.0 * M_PI * l_k * x_n / x_buffer_length)); \
									\
    x_retval[0][0] = l_y / M_PI; }

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
