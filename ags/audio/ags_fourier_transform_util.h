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

#ifndef __AGS_FOURIER_TRANSFORM_UTIL_H__
#define __AGS_FOURIER_TRANSFORM_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_FOURIER_TRANSFORM_UTIL         (ags_fourier_transform_util_get_type())

#define AGS_FOURIER_TRANSFORM_UTIL_INITIALIZER ((AgsFourierTransformUtil) {	\
      .source = NULL,						\
      .source_stride = 1,					\
      .frequency_domain = NULL,					\
      .buffer_length = 0,					\
      .format = AGS_SOUNDCARD_DEFAULT_FORMAT,			\
      .samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE })

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S8_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { double _Complex l_z; \
    gdouble l_h;							\
    gdouble l_k;							\
    gdouble l_r;							\
									\
    static const gdouble normalize_factor = M_PI / 127.0;		\
									\
    l_k = (gdouble) x_n;						\
    l_r = (gdouble) x_n;						\
									\
    l_h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(x_n - l_r);	\
									\
    l_z = (x_buffer[0] * normalize_factor) * l_h * cexp(-1.0 * I * 2.0 * M_PI * l_k * l_r); \
    									\
    ags_complex_set(x_retval[0], l_z); }

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S16_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval)  { double _Complex l_z; \
    gdouble l_h;							\
    gdouble l_k;							\
    gdouble l_r;							\
									\
    static const gdouble normalize_factor = M_PI / 32767.0;		\
									\
    l_k = (gdouble) x_n;						\
    l_r = (gdouble) x_n;						\
									\
    l_h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(x_n - l_r);	\
									\
    l_z = (x_buffer[0] * normalize_factor) * l_h * cexp(-1.0 * I * 2.0 * M_PI * l_k * l_r); \
    									\
    ags_complex_set(x_retval[0], l_z); }

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S24_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { double _Complex l_z; \
    gdouble l_h;							\
    gdouble l_k;							\
    gdouble l_r;							\
									\
    static const gdouble normalize_factor = M_PI / 8388607.0;		\
									\
    l_k = (gdouble) x_n;						\
    l_r = (gdouble) x_n;						\
									\
    l_h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(x_n - l_r);	\
									\
    l_z = (x_buffer[0] * normalize_factor) * l_h * cexp(-1.0 * I * 2.0 * M_PI * l_k * l_r); \
    									\
    ags_complex_set(x_retval[0], l_z); }

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S32_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { double _Complex l_z; \
    gdouble l_h;							\
    gdouble l_k;							\
    gdouble l_r;							\
									\
    static const gdouble normalize_factor = M_PI / 214748363.0;		\
									\
    l_k = (gdouble) x_n;						\
    l_r = (gdouble) x_n;						\
									\
    l_h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(x_n - l_r);	\
									\
    l_z = (x_buffer[0] * normalize_factor) * l_h * cexp(-1.0 * I * 2.0 * M_PI * l_k * l_r); \
    									\
    ags_complex_set(x_retval[0], l_z); }

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S64_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { double _Complex l_z; \
    gdouble l_h;							\
    gdouble l_k;							\
    gdouble l_r;							\
									\
    static const long double normalize_factor = M_PI / 9223372036854775807.0; \
									\
    l_k = (gdouble) x_n;						\
    l_r = (gdouble) x_n;						\
									\
    l_h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(x_n - l_r);	\
									\
    l_z = (x_buffer[0] * normalize_factor) * l_h * cexp(-1.0 * I * 2.0 * M_PI * l_k * l_r); \
    									\
    ags_complex_set(x_retval[0], l_z); }

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_FLOAT_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { double _Complex l_z; \
    gdouble l_h;							\
    gdouble l_k;							\
    gdouble l_r;							\
									\
    l_k = (gdouble) x_n;						\
    l_r = (gdouble) x_n;						\
									\
    l_h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(x_n - l_r);	\
									\
    l_z = (x_buffer[0] * M_PI) * l_h * cexp(-1.0 * I * 2.0 * M_PI * l_k * l_r); \
    									\
    ags_complex_set(x_retval[0], l_z); }

#define AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_DOUBLE_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { double _Complex l_z; \
    gdouble l_h;							\
    gdouble l_k;							\
    gdouble l_r;							\
									\
    l_k = (gdouble) x_n;						\
    l_r = (gdouble) x_n;						\
									\
    l_h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(x_n - l_r);	\
									\
    l_z = (x_buffer[0] * M_PI) * l_h * cexp(-1.0 * I * 2.0 * M_PI * l_k * l_r); \
    									\
    ags_complex_set(x_retval[0], l_z); }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S8_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { double _Complex l_z; \
    gdouble l_k;							\
    gdouble l_y;							\
									\
    static const gdouble scale = 127.0 / M_PI;				\
									\
    l_z = ags_complex_get(x_buffer);					\
									\
    l_k = (gdouble) x_n;						\
									\
    l_y = (l_z * cexp(I * 2.0 * M_PI * l_k * x_n)); \
									\
    x_retval[0][0] = scale * l_y; }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S16_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { double _Complex l_z; \
    gdouble l_k;							\
    gdouble l_y;							\
									\
    static const gdouble scale = 32767.0 / M_PI;			\
									\
    l_z = ags_complex_get(x_buffer);					\
									\
    l_k = (gdouble) x_n;						\
									\
    l_y = (l_z * cexp(I * 2.0 * M_PI * l_k * x_n));			\
									\
    x_retval[0][0] = scale * l_y; }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S24_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { double _Complex l_z; \
    gdouble l_k;							\
    gdouble l_y;							\
									\
    static const gdouble scale = 8388607.0 / M_PI;			\
									\
    l_z = ags_complex_get(x_buffer);					\
									\
    l_k = (gdouble) x_n;						\
									\
    l_y = (l_z * cexp(I * 2.0 * M_PI * l_k * x_n)); \
									\
    x_retval[0][0] = scale * l_y; }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S32_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { double _Complex l_z; \
    gdouble l_k;							\
    gdouble l_y;							\
									\
    static const gdouble scale = 214748363.0 / M_PI;			\
									\
    l_z = ags_complex_get(x_buffer);					\
									\
    l_k = (gdouble) x_n;						\
									\
    l_y = (l_z * cexp(I * 2.0 * M_PI * l_k * x_n)); \
									\
    x_retval[0][0] = scale * l_y; }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S64_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { double _Complex l_z; \
    gdouble l_k;							\
    gdouble l_y;							\
									\
    static const long double scale = 9223372036854775807.0 / M_PI;	\
									\
    l_z = ags_complex_get(x_buffer);					\
									\
    l_k = (gdouble) x_n;						\
									\
    l_y = (l_z * cexp(I * 2.0 * M_PI * l_k * x_n)); \
									\
    x_retval[0][0] = scale * l_y; }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_FLOAT_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { double _Complex l_z; \
    gdouble l_k;							\
    gdouble l_y;							\
									\
    l_z = ags_complex_get(x_buffer);					\
									\
    l_k = (gdouble) x_n;						\
									\
    l_y = (l_z * cexp(I * 2.0 * M_PI * l_k * x_n)); \
									\
    x_retval[0][0] = l_y / M_PI; }

#define AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_DOUBLE_FRAME(x_buffer, x_channels, x_n, x_buffer_length, x_retval) { double _Complex l_z; \
    gdouble l_k;							\
    gdouble l_y;							\
									\
    l_z = ags_complex_get(x_buffer);					\
									\
    l_k = (gdouble) x_n;						\
									\
    l_y = (l_z * cexp(I * 2.0 * M_PI * l_k * x_n)); \
									\
    x_retval[0][0] = l_y / M_PI; }

#define AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(x) (x + 1.0)

typedef struct _AgsFourierTransformUtil AgsFourierTransformUtil;

struct _AgsFourierTransformUtil
{
  gpointer source;
  guint source_stride;

  AgsComplex *frequency_domain;
  
  guint buffer_length;
  AgsSoundcardFormat format;
  guint samplerate;
};

GType ags_fourier_transform_util_get_type(void);

AgsFourierTransformUtil* ags_fourier_transform_util_alloc();

gpointer ags_fourier_transform_util_copy(AgsFourierTransformUtil *ptr);
void ags_fourier_transform_util_free(AgsFourierTransformUtil *ptr);

gpointer ags_fourier_transform_util_get_source(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_set_source(AgsFourierTransformUtil *fourier_transform_util,
					   gpointer source);

guint ags_fourier_transform_util_get_source_stride(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_set_source_stride(AgsFourierTransformUtil *fourier_transform_util,
						  guint source_stride);

AgsComplex* ags_fourier_transform_util_get_frequency_domain(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_set_frequency_domain(AgsFourierTransformUtil *fourier_transform_util,
						     AgsComplex *frequency_domain);

guint ags_fourier_transform_util_get_buffer_length(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_set_buffer_length(AgsFourierTransformUtil *fourier_transform_util,
						  guint buffer_length);

AgsSoundcardFormat ags_fourier_transform_util_get_format(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_set_format(AgsFourierTransformUtil *fourier_transform_util,
					   AgsSoundcardFormat format);

guint ags_fourier_transform_util_get_samplerate(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_set_samplerate(AgsFourierTransformUtil *fourier_transform_util,
					       guint samplerate);

void ags_fourier_transform_util_compute_stft_s8(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_compute_stft_s16(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_compute_stft_s24(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_compute_stft_s32(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_compute_stft_s64(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_compute_stft_float(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_compute_stft_double(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_compute_stft_complex(AgsFourierTransformUtil *fourier_transform_util);

void ags_fourier_transform_util_compute_stft(AgsFourierTransformUtil *fourier_transform_util);

void ags_fourier_transform_util_inverse_stft_s8(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_inverse_stft_s16(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_inverse_stft_s24(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_inverse_stft_s32(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_inverse_stft_s64(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_inverse_stft_float(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_inverse_stft_double(AgsFourierTransformUtil *fourier_transform_util);
void ags_fourier_transform_util_inverse_stft_complex(AgsFourierTransformUtil *fourier_transform_util);

void ags_fourier_transform_util_inverse_stft(AgsFourierTransformUtil *fourier_transform_util);

G_END_DECLS

#endif /*__AGS_FOURIER_TRANSFORM_UTIL_H__*/
