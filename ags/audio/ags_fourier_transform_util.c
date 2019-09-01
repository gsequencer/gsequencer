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
 * 
 * Loy, Gareth 
 * Musimathics volume 2 - Musical Signals, First MIT press paperback
 * edition, 2011
 * MIT Press,
 * Copyright 2007 Gareth Loy
 * ISBN 978-0-262-51656-3
 * 
 * http://musimathics.com
 * 
 * Based on Discrete Short Time Fourier Transform covered in Musimathics
 * volume 2 chapter 10. Thank you Dr. Gareth Loy.
 */

#include <ags/audio/ags_fourier_transform_util.h>

#include <stdlib.h>

#include <math.h>
#include <complex.h>

/**
 * ags_fourier_transform_util_compute_stft_s8:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @retval: the return location of result
 * 
 * Compute fourier transform of @buffer.
 * 
 * Since: 2.3.0
 */
void
ags_fourier_transform_util_compute_stft_s8(gint8 *buffer, guint channels,
					   guint buffer_length,
					   AgsComplex **retval)
{
  guint n;
  guint i, i_stop;

  static const gdouble normalize_factor = M_PI / exp2(7.0);

  if(buffer == NULL ||
     retval == NULL ||
     retval[0] == NULL){
    return;
  }

  i_stop = channels * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += channels, n++){
    complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) n;
    r = (gdouble) n;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);
    
    z = ((gdouble) buffer[i] * normalize_factor) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / buffer_length);

    ags_complex_set(retval[0] + i, z);
  }
}

/**
 * ags_fourier_transform_util_compute_stft_s16:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @retval: the return location of result
 * 
 * Compute fourier transform of @buffer.
 * 
 * Since: 2.3.0
 */
void
ags_fourier_transform_util_compute_stft_s16(gint16 *buffer, guint channels,
					    guint buffer_length,
					    AgsComplex **retval)
{
  guint n;
  guint i, i_stop;

  static const gdouble normalize_factor = M_PI / exp2(15.0);

  if(buffer == NULL ||
     retval == NULL ||
     retval[0] == NULL){
    return;
  }

  i_stop = channels * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += channels, n++){
    complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) n;
    r = (gdouble) n;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);
    
    z = ((gdouble) buffer[i] * normalize_factor) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / buffer_length);

    ags_complex_set(retval[0] + i, z);
  }
}

/**
 * ags_fourier_transform_util_compute_stft_s24:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @retval: the return location of result
 * 
 * Compute fourier transform of @buffer.
 * 
 * Since: 2.3.0
 */
void
ags_fourier_transform_util_compute_stft_s24(gint32 *buffer, guint channels,
					    guint buffer_length,
					    AgsComplex **retval)
{
  guint n;
  guint i, i_stop;

  static const gdouble normalize_factor = M_PI / exp2(23.0);

  if(buffer == NULL ||
     retval == NULL ||
     retval[0] == NULL){
    return;
  }

  i_stop = channels * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += channels, n++){
    complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) n;
    r = (gdouble) n;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);
    
    z = ((gdouble) buffer[i] * normalize_factor) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / buffer_length);

    ags_complex_set(retval[0] + i, z);
  }
}

/**
 * ags_fourier_transform_util_compute_stft_s32:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @retval: the return location of result
 * 
 * Compute fourier transform of @buffer.
 * 
 * Since: 2.3.0
 */
void
ags_fourier_transform_util_compute_stft_s32(gint32 *buffer, guint channels,
					    guint buffer_length,
					    AgsComplex **retval)
{
  guint n;
  guint i, i_stop;

  static const gdouble normalize_factor = M_PI / exp2(31.0);

  if(buffer == NULL ||
     retval == NULL ||
     retval[0] == NULL){
    return;
  }

  i_stop = channels * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += channels, n++){
    complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) n;
    r = (gdouble) n;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);
    
    z = ((gdouble) buffer[i] * normalize_factor) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / buffer_length);

    ags_complex_set(retval[0] + i, z);
  }
}

/**
 * ags_fourier_transform_util_compute_stft_s64:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @retval: the return location of result
 * 
 * Compute fourier transform of @buffer.
 * 
 * Since: 2.3.0
 */
void
ags_fourier_transform_util_compute_stft_s64(gint64 *buffer, guint channels,
					    guint buffer_length,
					    AgsComplex **retval)
{
  guint n;
  guint i, i_stop;

  static const long double normalize_factor = M_PI / exp2(63.0);

  if(buffer == NULL ||
     retval == NULL ||
     retval[0] == NULL){
    return;
  }

  i_stop = channels * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += channels, n++){
    complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) n;
    r = (gdouble) n;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);
    
    z = ((gdouble) buffer[i] * normalize_factor) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / buffer_length);

    ags_complex_set(retval[0] + i, z);
  }
}

/**
 * ags_fourier_transform_util_compute_stft_float:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @retval: the return location of result
 * 
 * Compute fourier transform of @buffer.
 * 
 * Since: 2.3.0
 */
void
ags_fourier_transform_util_compute_stft_float(gfloat *buffer, guint channels,
					      guint buffer_length,
					      AgsComplex **retval)
{
  guint n;
  guint i, i_stop;

  if(buffer == NULL ||
     retval == NULL ||
     retval[0] == NULL){
    return;
  }

  i_stop = channels * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += channels, n++){
    complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) n;
    r = (gdouble) n;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);
    
    z = (buffer[i] * M_PI) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / buffer_length);

    ags_complex_set(retval[0] + i, z);
  }
}

/**
 * ags_fourier_transform_util_compute_stft_double:
 * @buffer: the audio buffer
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @retval: the return location of result
 * 
 * Compute fourier transform of @buffer.
 * 
 * Since: 2.3.0
 */
void
ags_fourier_transform_util_compute_stft_double(gdouble *buffer, guint channels,
					       guint buffer_length,
					       AgsComplex **retval)
{
  guint n;
  guint i, i_stop;

  if(buffer == NULL ||
     retval == NULL ||
     retval[0] == NULL){
    return;
  }

  i_stop = channels * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += channels, n++){
    complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) n;
    r = (gdouble) n;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);
    
    z = (buffer[i] * M_PI) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / buffer_length);

    ags_complex_set(retval[0] + i, z);
  }
}

/**
 * ags_fourier_transform_util_inverse_stft_s8:
 * @buffer: the complex data
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @retval: the return location of result
 * 
 * Compute inverse fourier transform of @buffer.
 * 
 * Since: 2.3.0
 */
void
ags_fourier_transform_util_inverse_stft_s8(AgsComplex *buffer, guint channels,
					   guint buffer_length,
					   gint8 **retval)
{
  guint n;
  guint i, i_stop;

  static const gdouble scale = 127.0 / M_PI;

  if(buffer == NULL ||
     retval == NULL ||
     retval[0] == NULL){
    return;
  }

  i_stop = channels * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += channels, n++){
    complex z;
    gdouble k;
    gdouble y;

    z = ags_complex_get(buffer[n]);

    k = (gdouble) n;

    y = cabs(z * cexp(I * 2.0 * M_PI * k * n / buffer_length));

    retval[0][i] = scale * y;
  }
}

/**
 * ags_fourier_transform_util_inverse_stft_s16:
 * @buffer: the complex data
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @retval: the return location of result
 * 
 * Compute inverse fourier transform of @buffer.
 * 
 * Since: 2.3.0
 */
void
ags_fourier_transform_util_inverse_stft_s16(AgsComplex *buffer, guint channels,
					    guint buffer_length,
					    gint16 **retval)
{
  guint n;
  guint i, i_stop;

  static const gdouble scale = 32767.0 / M_PI;

  if(buffer == NULL ||
     retval == NULL ||
     retval[0] == NULL){
    return;
  }

  i_stop = channels * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += channels, n++){
    complex z;
    gdouble k;
    gdouble y;

    z = ags_complex_get(buffer[n]);

    k = (gdouble) n;

    y = cabs(z * cexp(I * 2.0 * M_PI * k * n / buffer_length));

    retval[0][i] = scale * y;
  }
}

/**
 * ags_fourier_transform_util_inverse_stft_s24:
 * @buffer: the complex data
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @retval: the return location of result
 * 
 * Compute inverse fourier transform of @buffer.
 * 
 * Since: 2.3.0
 */
void
ags_fourier_transform_util_inverse_stft_s24(AgsComplex *buffer, guint channels,
					    guint buffer_length,
					    gint32 **retval)
{
  guint n;
  guint i, i_stop;

  static const gdouble scale = 8388607.0 / M_PI;

  if(buffer == NULL ||
     retval == NULL ||
     retval[0] == NULL){
    return;
  }

  i_stop = channels * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += channels, n++){
    complex z;
    gdouble k;
    gdouble y;

    z = ags_complex_get(buffer[n]);

    k = (gdouble) n;

    y = cabs(z * cexp(I * 2.0 * M_PI * k * n / buffer_length));

    retval[0][i] = scale * y;
  }
}

/**
 * ags_fourier_transform_util_inverse_stft_s32:
 * @buffer: the complex data
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @retval: the return location of result
 * 
 * Compute inverse fourier transform of @buffer.
 * 
 * Since: 2.3.0
 */
void
ags_fourier_transform_util_inverse_stft_s32(AgsComplex *buffer, guint channels,
					    guint buffer_length,
					    gint32 **retval)
{
  guint n;
  guint i, i_stop;

  static const gdouble scale = 214748363.0 / M_PI;

  if(buffer == NULL ||
     retval == NULL ||
     retval[0] == NULL){
    return;
  }

  i_stop = channels * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += channels, n++){
    complex z;
    gdouble k;
    gdouble y;

    z = ags_complex_get(buffer[n]);

    k = (gdouble) n;

    y = cabs(z * cexp(I * 2.0 * M_PI * k * n / buffer_length));

    retval[0][i] = scale * y;
  }
}

/**
 * ags_fourier_transform_util_inverse_stft_s64:
 * @buffer: the complex data
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @retval: the return location of result
 * 
 * Compute inverse fourier transform of @buffer.
 * 
 * Since: 2.3.0
 */
void
ags_fourier_transform_util_inverse_stft_s64(AgsComplex *buffer, guint channels,
					    guint buffer_length,
					    gint64 **retval)
{
  guint n;
  guint i, i_stop;

  static const long double scale = 9223372036854775807.0 / M_PI;

  if(buffer == NULL ||
     retval == NULL ||
     retval[0] == NULL){
    return;
  }

  i_stop = channels * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += channels, n++){
    complex z;
    gdouble k;
    gdouble y;

    z = ags_complex_get(buffer[n]);

    k = (gdouble) n;

    y = cabs(z * cexp(I * 2.0 * M_PI * k * n / buffer_length));

    retval[0][i] = scale * y;
  }
}

/**
 * ags_fourier_transform_util_inverse_stft_float:
 * @buffer: the complex data
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @retval: the return location of result
 * 
 * Compute inverse fourier transform of @buffer.
 * 
 * Since: 2.3.0
 */
void
ags_fourier_transform_util_inverse_stft_float(AgsComplex *buffer, guint channels,
					      guint buffer_length,
					      gfloat **retval)
{
  guint n;
  guint i, i_stop;

  if(buffer == NULL ||
     retval == NULL ||
     retval[0] == NULL){
    return;
  }

  i_stop = channels * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += channels, n++){
    complex z;
    gdouble k;
    gdouble y;

    z = ags_complex_get(buffer[n]);

    k = (gdouble) n;

    y = cabs(z * cexp(I * 2.0 * M_PI * k * n / buffer_length));

    retval[0][i] = y / M_PI;
  }
}

/**
 * ags_fourier_transform_util_inverse_stft_double:
 * @buffer: the complex data
 * @channels: number of audio channels
 * @buffer_length: the buffer's length
 * @retval: the return location of result
 * 
 * Compute inverse fourier transform of @buffer.
 * 
 * Since: 2.3.0
 */
void
ags_fourier_transform_util_inverse_stft_double(AgsComplex *buffer, guint channels,
					       guint buffer_length,
					       gdouble **retval)
{
  guint n;
  guint i, i_stop;

  if(buffer == NULL ||
     retval == NULL ||
     retval[0] == NULL){
    return;
  }

  i_stop = channels * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += channels, n++){
    complex z;
    gdouble k;
    gdouble y;

    z = ags_complex_get(buffer[n]);

    k = (gdouble) n;

    y = cabs(z * cexp(I * 2.0 * M_PI * k * n / buffer_length));

    retval[0][i] = y / M_PI;
  }
}
