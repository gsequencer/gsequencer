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

/**
 * SECTION:ags_fourier_transform_util
 * @short_description: fourier transform util
 * @title: AgsFourierTransformUtil
 * @section_id:
 * @include: ags/audio/ags_fourier_transform_util.h
 *
 * Utility functions to compute fourier transform.
 */

GType
ags_fourier_transform_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fourier_transform_util = 0;

    ags_type_fourier_transform_util =
      g_boxed_type_register_static("AgsFourierTransformUtil",
				   (GBoxedCopyFunc) ags_fourier_transform_util_copy,
				   (GBoxedFreeFunc) ags_fourier_transform_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fourier_transform_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_fourier_transform_util_alloc:
 *
 * Allocate #AgsFourierTransformUtil-struct
 *
 * Returns: a new #AgsFourierTransformUtil-struct
 *
 * Since: 7.0.0
 */
AgsFourierTransformUtil*
ags_fourier_transform_util_alloc()
{
  AgsFourierTransformUtil *ptr;

  ptr = (AgsFourierTransformUtil *) g_new(AgsFourierTransformUtil,
					  1);

  ptr[0] = AGS_FOURIER_TRANSFORM_UTIL_INITIALIZER;

  return(ptr);
}

/**
 * ags_fourier_transform_util_copy:
 * @ptr: the original #AgsFourierTransformUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsFourierTransformUtil-struct
 *
 * Since: 7.0.0
 */
gpointer
ags_fourier_transform_util_copy(AgsFourierTransformUtil *ptr)
{
  AgsFourierTransformUtil *new_ptr;

  g_return_val_if_fail(ptr != NULL, NULL);

  new_ptr = (AgsFourierTransformUtil *) g_new(AgsFourierTransformUtil,
					      1);
  
  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->frequency_domain = NULL;
  
  if(ptr->buffer_length > 0){
    new_ptr->frequency_domain = g_malloc(ptr->buffer_length * sizeof(AgsComplex));

    memcpy(new_ptr->frequency_domain, ptr->frequency_domain, ptr->buffer_length * sizeof(AgsComplex));
  }
  
  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;
  new_ptr->samplerate = ptr->samplerate;
  
  return(new_ptr);
}

/**
 * ags_fourier_transform_util_free:
 * @ptr: the #AgsFourierTransformUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 7.0.0
 */
void
ags_fourier_transform_util_free(AgsFourierTransformUtil *ptr)
{
  g_return_if_fail(ptr != NULL);

  g_free(ptr->source);
  
  g_free(ptr);
}

/**
 * ags_fourier_transform_util_get_source:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Get source buffer of @fourier_transform_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 7.0.0
 */
gpointer
ags_fourier_transform_util_get_source(AgsFourierTransformUtil *fourier_transform_util)
{
  if(fourier_transform_util == NULL){
    return(NULL);
  }

  return(fourier_transform_util->source);
}

/**
 * ags_fourier_transform_util_set_source:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @fourier_transform_util.
 *
 * Since: 7.0.0
 */
void
ags_fourier_transform_util_set_source(AgsFourierTransformUtil *fourier_transform_util,
				      gpointer source)
{
  if(fourier_transform_util == NULL){
    return;
  }
  
  fourier_transform_util->source = source;
}

/**
 * ags_fourier_transform_util_get_source_stride:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Get source stride of @fourier_transform_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 7.0.0
 */
guint
ags_fourier_transform_util_get_source_stride(AgsFourierTransformUtil *fourier_transform_util)
{
  if(fourier_transform_util == NULL){
    return(0);
  }

  return(fourier_transform_util->source_stride);
}

/**
 * ags_fourier_transform_util_set_source_stride:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @fourier_transform_util.
 *
 * Since: 7.0.0
 */
void
ags_fourier_transform_util_set_source_stride(AgsFourierTransformUtil *fourier_transform_util,
					     guint source_stride)
{
  if(fourier_transform_util == NULL){
    return;
  }

  fourier_transform_util->source_stride = source_stride;
}

/**
 * ags_fourier_transform_util_get_frequency_domain:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Get frequency domain buffer of @fourier_transform_util.
 * 
 * Returns: the frequency domain buffer
 * 
 * Since: 7.0.0
 */
AgsComplex*
ags_fourier_transform_util_get_frequency_domain(AgsFourierTransformUtil *fourier_transform_util)
{
  if(fourier_transform_util == NULL){
    return(NULL);
  }

  return(fourier_transform_util->frequency_domain);
}

/**
 * ags_fourier_transform_util_set_frequency_domain:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * @frequency_domain: the frequency domain buffer
 *
 * Set @frequency_domain buffer of @fourier_transform_util.
 *
 * Since: 7.0.0
 */
void
ags_fourier_transform_util_set_frequency_domain(AgsFourierTransformUtil *fourier_transform_util,
						AgsComplex *frequency_domain)
{
  if(fourier_transform_util == NULL){
    return;
  }
  
  fourier_transform_util->frequency_domain = frequency_domain;
}

/**
 * ags_fourier_transform_util_get_buffer_length:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Get buffer length of @fourier_transform_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 7.0.0
 */
guint
ags_fourier_transform_util_get_buffer_length(AgsFourierTransformUtil *fourier_transform_util)
{
  if(fourier_transform_util == NULL){
    return(0);
  }

  return(fourier_transform_util->buffer_length);
}

/**
 * ags_fourier_transform_util_set_buffer_length:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @fourier_transform_util.
 *
 * Since: 7.0.0
 */
void
ags_fourier_transform_util_set_buffer_length(AgsFourierTransformUtil *fourier_transform_util,
					     guint buffer_length)
{
  if(fourier_transform_util == NULL ||
     fourier_transform_util->buffer_length == buffer_length){
    return;
  }
  
  fourier_transform_util->buffer_length = buffer_length;
}

/**
 * ags_fourier_transform_util_get_format:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Get audio buffer util format of @fourier_transform_util.
 * 
 * Returns: the audio buffer util format
 * 
 * Since: 7.0.0
 */
AgsSoundcardFormat
ags_fourier_transform_util_get_format(AgsFourierTransformUtil *fourier_transform_util)
{
  if(fourier_transform_util == NULL){
    return(0);
  }

  return(fourier_transform_util->format);
}

/**
 * ags_fourier_transform_util_set_format:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * @format: the audio buffer util format
 *
 * Set @format of @fourier_transform_util.
 *
 * Since: 7.0.0
 */
void
ags_fourier_transform_util_set_format(AgsFourierTransformUtil *fourier_transform_util,
				      AgsSoundcardFormat format)
{
  if(fourier_transform_util == NULL){
    return;
  }

  fourier_transform_util->format = format;
}

/**
 * ags_fourier_transform_util_get_samplerate:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Get samplerate of @fourier_transform_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 7.0.0
 */
guint
ags_fourier_transform_util_get_samplerate(AgsFourierTransformUtil *fourier_transform_util)
{
  if(fourier_transform_util == NULL){
    return(0);
  }

  return(fourier_transform_util->samplerate);
}

/**
 * ags_fourier_transform_util_set_samplerate:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @fourier_transform_util.
 *
 * Since: 7.0.0
 */
void
ags_fourier_transform_util_set_samplerate(AgsFourierTransformUtil *fourier_transform_util,
					  guint samplerate)
{
  if(fourier_transform_util == NULL){
    return;
  }

  fourier_transform_util->samplerate = samplerate;
}

/**
 * ags_fourier_transform_util_compute_stft_s8:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Compute fourier transform of @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_fourier_transform_util_compute_stft_s8(AgsFourierTransformUtil *fourier_transform_util)
{
  gint8 *source;
  AgsComplex *frequency_domain;

  guint source_stride;
  guint buffer_length;
  
  guint n;
  guint i, i_stop;

  static const gdouble normalize_factor = M_PI / 127.0;

  source = fourier_transform_util->source;

  source_stride = fourier_transform_util->source_stride;

  frequency_domain = fourier_transform_util->frequency_domain;
  
  buffer_length = fourier_transform_util->buffer_length;

  i_stop = source_stride * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += source_stride, n++){
    double _Complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) n;
    r = (gdouble) n;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);
    
    z = ((gdouble) source[i] * normalize_factor) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r);

    ags_complex_set(frequency_domain + i, z);
  }
}

/**
 * ags_fourier_transform_util_compute_stft_s16:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Compute fourier transform of @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_fourier_transform_util_compute_stft_s16(AgsFourierTransformUtil *fourier_transform_util)
{
  gint16 *source;
  AgsComplex *frequency_domain;

  guint source_stride;
  guint buffer_length;
  
  guint n;
  guint i, i_stop;

  static const gdouble normalize_factor = M_PI / 32767.0;

  source = fourier_transform_util->source;

  source_stride = fourier_transform_util->source_stride;

  frequency_domain = fourier_transform_util->frequency_domain;
  
  buffer_length = fourier_transform_util->buffer_length;

  i_stop = source_stride * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += source_stride, n++){
    double _Complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) n;
    r = (gdouble) n;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);
    
    z = ((gdouble) source[i] * normalize_factor) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r);

    ags_complex_set(frequency_domain + i, z);
  }
}

/**
 * ags_fourier_transform_util_compute_stft_s24:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Compute fourier transform of @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_fourier_transform_util_compute_stft_s24(AgsFourierTransformUtil *fourier_transform_util)
{
  gint32 *source;
  AgsComplex *frequency_domain;

  guint source_stride;
  guint buffer_length;
  
  guint n;
  guint i, i_stop;

  static const gdouble normalize_factor = M_PI / 8388607.0;

  source = fourier_transform_util->source;

  source_stride = fourier_transform_util->source_stride;

  frequency_domain = fourier_transform_util->frequency_domain;
  
  buffer_length = fourier_transform_util->buffer_length;

  i_stop = source_stride * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += source_stride, n++){
    double _Complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) n;
    r = (gdouble) n;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);
    
    z = ((gdouble) source[i] * normalize_factor) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r);

    ags_complex_set(frequency_domain + i, z);
  }
}

/**
 * ags_fourier_transform_util_compute_stft_s32:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Compute fourier transform of @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_fourier_transform_util_compute_stft_s32(AgsFourierTransformUtil *fourier_transform_util)
{
  gint32 *source;
  AgsComplex *frequency_domain;

  guint source_stride;
  guint buffer_length;
  
  guint n;
  guint i, i_stop;

  static const gdouble normalize_factor = M_PI / 214748363.0;

  source = fourier_transform_util->source;

  source_stride = fourier_transform_util->source_stride;

  frequency_domain = fourier_transform_util->frequency_domain;
  
  buffer_length = fourier_transform_util->buffer_length;

  i_stop = source_stride * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += source_stride, n++){
    double _Complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) n;
    r = (gdouble) n;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);
    
    z = ((gdouble) source[i] * normalize_factor) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r);

    ags_complex_set(frequency_domain + i, z);
  }
}

/**
 * ags_fourier_transform_util_compute_stft_s64:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Compute fourier transform of @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_fourier_transform_util_compute_stft_s64(AgsFourierTransformUtil *fourier_transform_util)
{
  gint64 *source;
  AgsComplex *frequency_domain;

  guint source_stride;
  guint buffer_length;
  
  guint n;
  guint i, i_stop;

  static const long double normalize_factor = M_PI / 9223372036854775807.0;

  source = fourier_transform_util->source;

  source_stride = fourier_transform_util->source_stride;

  frequency_domain = fourier_transform_util->frequency_domain;
  
  buffer_length = fourier_transform_util->buffer_length;

  i_stop = source_stride * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += source_stride, n++){
    double _Complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) n;
    r = (gdouble) n;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);
    
    z = ((gdouble) source[i] * normalize_factor) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r);

    ags_complex_set(frequency_domain + i, z);
  }
}

/**
 * ags_fourier_transform_util_compute_stft_float:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Compute fourier transform of @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_fourier_transform_util_compute_stft_float(AgsFourierTransformUtil *fourier_transform_util)
{
  gfloat *source;
  AgsComplex *frequency_domain;

  guint source_stride;
  guint buffer_length;
  
  guint n;
  guint i, i_stop;

  source = fourier_transform_util->source;

  source_stride = fourier_transform_util->source_stride;

  frequency_domain = fourier_transform_util->frequency_domain;
  
  buffer_length = fourier_transform_util->buffer_length;

  i_stop = source_stride * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += source_stride, n++){
    double _Complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) n;
    r = (gdouble) n;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);
    
    z = (source[i] * M_PI) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r);

    ags_complex_set(frequency_domain + i, z);
  }
}

/**
 * ags_fourier_transform_util_compute_stft_double:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Compute fourier transform of @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_fourier_transform_util_compute_stft_double(AgsFourierTransformUtil *fourier_transform_util)
{
  gdouble *source;
  AgsComplex *frequency_domain;

  guint source_stride;
  guint buffer_length;
  
  guint n;
  guint i, i_stop;

  source = fourier_transform_util->source;

  source_stride = fourier_transform_util->source_stride;

  frequency_domain = fourier_transform_util->frequency_domain;
  
  buffer_length = fourier_transform_util->buffer_length;

  i_stop = source_stride * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += source_stride, n++){
    double _Complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) n;
    r = (gdouble) n;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);
    
    z = (source[i] * M_PI) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r);

    ags_complex_set(frequency_domain + i, z);
  }
}

/**
 * ags_fourier_transform_util_compute_stft_complex:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Compute fourier transform of @buffer.
 * 
 * Since: 3.6.0
 */
void
ags_fourier_transform_util_compute_stft_complex(AgsFourierTransformUtil *fourier_transform_util)
{
  AgsComplex *source;
  AgsComplex *frequency_domain;

  guint source_stride;
  guint buffer_length;
  
  guint n;
  guint i, i_stop;

  source = fourier_transform_util->source;

  source_stride = fourier_transform_util->source_stride;

  frequency_domain = fourier_transform_util->frequency_domain;
  
  buffer_length = fourier_transform_util->buffer_length;

  i_stop = source_stride * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += source_stride, n++){
    double _Complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) n;
    r = (gdouble) n;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(n - r);
    
    z = (ags_complex_get(source + i) * M_PI) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r);

    ags_complex_set(frequency_domain + i, z);
  }
}

/**
 * ags_fourier_transform_util_compute_stft:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Compute fourier transform.
 * 
 * Since: 7.0.0
 */
void
ags_fourier_transform_util_compute_stft(AgsFourierTransformUtil *fourier_transform_util)
{
  if(fourier_transform_util == NULL ||
     fourier_transform_util->source == NULL ||
     fourier_transform_util->frequency_domain == NULL){
    return;
  }

  switch(fourier_transform_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_fourier_transform_util_compute_stft_s8(fourier_transform_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_fourier_transform_util_compute_stft_s16(fourier_transform_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_fourier_transform_util_compute_stft_s24(fourier_transform_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_fourier_transform_util_compute_stft_s32(fourier_transform_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_fourier_transform_util_compute_stft_s64(fourier_transform_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_fourier_transform_util_compute_stft_float(fourier_transform_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_fourier_transform_util_compute_stft_double(fourier_transform_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_fourier_transform_util_compute_stft_complex(fourier_transform_util);
  }
  break;
  }
}

/**
 * ags_fourier_transform_util_inverse_stft_s8:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Compute inverse fourier transform of @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_fourier_transform_util_inverse_stft_s8(AgsFourierTransformUtil *fourier_transform_util)
{
  gint8 *source;
  AgsComplex *frequency_domain;

  guint source_stride;
  guint buffer_length;
  
  guint n;
  guint i, i_stop;

  static const gdouble scale = 127.0 / M_PI;

  source = fourier_transform_util->source;

  source_stride = fourier_transform_util->source_stride;

  frequency_domain = fourier_transform_util->frequency_domain;
  
  buffer_length = fourier_transform_util->buffer_length;

  i_stop = source_stride * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += source_stride, n++){
    double _Complex z;
    gdouble k;
    gdouble y;

    z = ags_complex_get(frequency_domain + n);

    k = (gdouble) n;

    y = (z * cexp(I * 2.0 * M_PI * k * n));

    source[i] = scale * y;
  }
}

/**
 * ags_fourier_transform_util_inverse_stft_s16:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Compute inverse fourier transform of @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_fourier_transform_util_inverse_stft_s16(AgsFourierTransformUtil *fourier_transform_util)
{
  gint16 *source;
  AgsComplex *frequency_domain;

  guint source_stride;
  guint buffer_length;
  
  guint n;
  guint i, i_stop;

  static const gdouble scale = 32767.0 / M_PI;

  source = fourier_transform_util->source;

  source_stride = fourier_transform_util->source_stride;

  frequency_domain = fourier_transform_util->frequency_domain;
  
  buffer_length = fourier_transform_util->buffer_length;

  i_stop = source_stride * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += source_stride, n++){
    double _Complex z;
    gdouble k;
    gdouble y;

    z = ags_complex_get(frequency_domain + n);

    k = (gdouble) n;

    y = (z * cexp(I * 2.0 * M_PI * k * n));

    source[i] = scale * y;
  }
}

/**
 * ags_fourier_transform_util_inverse_stft_s24:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Compute inverse fourier transform of @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_fourier_transform_util_inverse_stft_s24(AgsFourierTransformUtil *fourier_transform_util)
{
  gint32 *source;
  AgsComplex *frequency_domain;

  guint source_stride;
  guint buffer_length;
  
  guint n;
  guint i, i_stop;

  static const gdouble scale = 8388607.0 / M_PI;

  source = fourier_transform_util->source;

  source_stride = fourier_transform_util->source_stride;

  frequency_domain = fourier_transform_util->frequency_domain;
  
  buffer_length = fourier_transform_util->buffer_length;

  i_stop = source_stride * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += source_stride, n++){
    double _Complex z;
    gdouble k;
    gdouble y;

    z = ags_complex_get(frequency_domain + n);

    k = (gdouble) n;

    y = (z * cexp(I * 2.0 * M_PI * k * n));

    source[i] = scale * y;
  }
}

/**
 * ags_fourier_transform_util_inverse_stft_s32:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Compute inverse fourier transform of @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_fourier_transform_util_inverse_stft_s32(AgsFourierTransformUtil *fourier_transform_util)
{
  gint32 *source;
  AgsComplex *frequency_domain;

  guint source_stride;
  guint buffer_length;
  
  guint n;
  guint i, i_stop;

  static const gdouble scale = 214748363.0 / M_PI;

  source = fourier_transform_util->source;

  source_stride = fourier_transform_util->source_stride;

  frequency_domain = fourier_transform_util->frequency_domain;
  
  buffer_length = fourier_transform_util->buffer_length;

  i_stop = source_stride * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += source_stride, n++){
    double _Complex z;
    gdouble k;
    gdouble y;

    z = ags_complex_get(frequency_domain + n);

    k = (gdouble) n;

    y = (z * cexp(I * 2.0 * M_PI * k * n));

    source[i] = scale * y;
  }
}

/**
 * ags_fourier_transform_util_inverse_stft_s64:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Compute inverse fourier transform of @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_fourier_transform_util_inverse_stft_s64(AgsFourierTransformUtil *fourier_transform_util)
{
  gint64 *source;
  AgsComplex *frequency_domain;

  guint source_stride;
  guint buffer_length;
  
  guint n;
  guint i, i_stop;

  static const long double scale = 9223372036854775807.0 / M_PI;

  source = fourier_transform_util->source;

  source_stride = fourier_transform_util->source_stride;

  frequency_domain = fourier_transform_util->frequency_domain;
  
  buffer_length = fourier_transform_util->buffer_length;

  i_stop = source_stride * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += source_stride, n++){
    double _Complex z;
    gdouble k;
    gdouble y;

    z = ags_complex_get(frequency_domain + n);

    k = (gdouble) n;

    y = (z * cexp(I * 2.0 * M_PI * k * n));

    source[i] = scale * y;
  }
}

/**
 * ags_fourier_transform_util_inverse_stft_float:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Compute inverse fourier transform of @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_fourier_transform_util_inverse_stft_float(AgsFourierTransformUtil *fourier_transform_util)
{
  gfloat *source;
  AgsComplex *frequency_domain;

  guint source_stride;
  guint buffer_length;
  
  guint n;
  guint i, i_stop;

  source = fourier_transform_util->source;

  source_stride = fourier_transform_util->source_stride;

  frequency_domain = fourier_transform_util->frequency_domain;
  
  buffer_length = fourier_transform_util->buffer_length;

  i_stop = source_stride * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += source_stride, n++){
    double _Complex z;
    gdouble k;
    gdouble y;

    z = ags_complex_get(frequency_domain + n);

    k = (gdouble) n;

    y = (z * cexp(I * 2.0 * M_PI * k * n));

    source[i] = y / M_PI;
  }
}

/**
 * ags_fourier_transform_util_inverse_stft_double:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Compute inverse fourier transform of @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_fourier_transform_util_inverse_stft_double(AgsFourierTransformUtil *fourier_transform_util)
{
  gdouble *source;
  AgsComplex *frequency_domain;

  guint source_stride;
  guint buffer_length;
  
  guint n;
  guint i, i_stop;

  source = fourier_transform_util->source;

  source_stride = fourier_transform_util->source_stride;

  frequency_domain = fourier_transform_util->frequency_domain;
  
  buffer_length = fourier_transform_util->buffer_length;

  i_stop = source_stride * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += source_stride, n++){
    double _Complex z;
    gdouble k;
    gdouble y;

    z = ags_complex_get(frequency_domain + n);

    k = (gdouble) n;

    y = (z * cexp(I * 2.0 * M_PI * k * n));

    source[i] = y / M_PI;
  }
}

/**
 * ags_fourier_transform_util_inverse_stft_complex:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Compute inverse fourier transform of @buffer.
 * 
 * Since: 3.6.0
 */
void
ags_fourier_transform_util_inverse_stft_complex(AgsFourierTransformUtil *fourier_transform_util)
{
  AgsComplex *source;
  AgsComplex *frequency_domain;

  guint source_stride;
  guint buffer_length;
  
  guint n;
  guint i, i_stop;

  source = fourier_transform_util->source;

  source_stride = fourier_transform_util->source_stride;

  frequency_domain = fourier_transform_util->frequency_domain;
  
  buffer_length = fourier_transform_util->buffer_length;

  i_stop = source_stride * buffer_length;
  
  for(i = 0, n = 0; i < i_stop; i += source_stride, n++){
    double _Complex z;
    gdouble k;
    gdouble y;

    z = ags_complex_get(frequency_domain + n);

    k = (gdouble) n;

    y = (z * cexp(I * 2.0 * M_PI * k * n));

    source[i].real = y / M_PI;
    source[i].imag = 0.0;
  }
}

/**
 * ags_fourier_transform_util_inverse_stft:
 * @fourier_transform_util: the #AgsFourierTransformUtil-struct
 * 
 * Compute inverse fourier transform.
 * 
 * Since: 7.0.0
 */
void
ags_fourier_transform_util_inverse_stft(AgsFourierTransformUtil *fourier_transform_util)
{
  if(fourier_transform_util == NULL ||
     fourier_transform_util->source == NULL ||
     fourier_transform_util->frequency_domain == NULL){
    return;
  }

  switch(fourier_transform_util->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    ags_fourier_transform_util_inverse_stft_s8(fourier_transform_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    ags_fourier_transform_util_inverse_stft_s16(fourier_transform_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    ags_fourier_transform_util_inverse_stft_s24(fourier_transform_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    ags_fourier_transform_util_inverse_stft_s32(fourier_transform_util);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    ags_fourier_transform_util_inverse_stft_s64(fourier_transform_util);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    ags_fourier_transform_util_inverse_stft_float(fourier_transform_util);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    ags_fourier_transform_util_inverse_stft_double(fourier_transform_util);
  }
  break;
  case AGS_SOUNDCARD_COMPLEX:
  {
    ags_fourier_transform_util_inverse_stft_complex(fourier_transform_util);
  }
  break;
  }
}
