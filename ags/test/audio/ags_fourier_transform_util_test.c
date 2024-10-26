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

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

int ags_fourier_transform_util_test_init_suite();
int ags_fourier_transform_util_test_clean_suite();

void ags_fourier_transform_util_test_compute_stft_s8();
void ags_fourier_transform_util_test_compute_stft_s16();
void ags_fourier_transform_util_test_compute_stft_s24();
void ags_fourier_transform_util_test_compute_stft_s32();
void ags_fourier_transform_util_test_compute_stft_s64();
void ags_fourier_transform_util_test_compute_stft_float();
void ags_fourier_transform_util_test_compute_stft_double();

void ags_fourier_transform_util_test_inverse_stft_s8();
void ags_fourier_transform_util_test_inverse_stft_s16();
void ags_fourier_transform_util_test_inverse_stft_s24();
void ags_fourier_transform_util_test_inverse_stft_s32();
void ags_fourier_transform_util_test_inverse_stft_s64();
void ags_fourier_transform_util_test_inverse_stft_float();
void ags_fourier_transform_util_test_inverse_stft_double();

#define AGS_FOURIER_TRANSFORM_UTIL_TEST_MAX_S24 (0x7fffff)
#define AGS_FOURIER_TRANSFORM_UTIL_TEST_FREQUENCY (440.0)
#define AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE (44100.0)

#define AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S8_BUFFER_SIZE (8192)

#define AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S16_BUFFER_SIZE (8192)

#define AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S24_BUFFER_SIZE (8192)

#define AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S32_BUFFER_SIZE (8192)

#define AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S64_BUFFER_SIZE (8192)

#define AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_FLOAT_BUFFER_SIZE (8192)

#define AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_DOUBLE_BUFFER_SIZE (8192)

#define AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S8_BUFFER_SIZE (8192)

#define AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S16_BUFFER_SIZE (8192)

#define AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S24_BUFFER_SIZE (8192)

#define AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S32_BUFFER_SIZE (8192)

#define AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S64_BUFFER_SIZE (8192)

#define AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_FLOAT_BUFFER_SIZE (8192)

#define AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_DOUBLE_BUFFER_SIZE (8192)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fourier_transform_util_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fourier_transform_util_test_clean_suite()
{
  return(0);
}

void
ags_fourier_transform_util_test_compute_stft_s8()
{
  AgsFourierTransformUtil fourier_transform_util = AGS_FOURIER_TRANSFORM_UTIL_INITIALIZER;
  
  AgsComplex *retval;
  gint8 *s8_buffer;

  guint i;
  gboolean success;

  static const double _Complex z_zero = 0.0 + I * 0.0;

  fourier_transform_util.buffer_length = AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S8_BUFFER_SIZE;
  fourier_transform_util.format = AGS_SOUNDCARD_SIGNED_8_BIT;
  fourier_transform_util.samplerate = AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE;
  
  s8_buffer =
    fourier_transform_util.source = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S8_BUFFER_SIZE,
						     AGS_SOUNDCARD_SIGNED_8_BIT);

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S8_BUFFER_SIZE; i++){
    s8_buffer[i] = G_MAXINT8 * sin(i * 2.0 * M_PI * AGS_FOURIER_TRANSFORM_UTIL_TEST_FREQUENCY / AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE);
  }

  /* test */
  retval =
    fourier_transform_util.frequency_domain = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S8_BUFFER_SIZE,
							       AGS_SOUNDCARD_COMPLEX);

  ags_fourier_transform_util_compute_stft_s8(&fourier_transform_util);

  success = TRUE;

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S8_BUFFER_SIZE; i++){
    if(s8_buffer[i] != 0 &&
       ags_complex_get(retval + i) == z_zero){
      success = FALSE;
      
      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_fourier_transform_util_test_compute_stft_s16()
{
  AgsFourierTransformUtil fourier_transform_util = AGS_FOURIER_TRANSFORM_UTIL_INITIALIZER;
  
  AgsComplex *retval;
  gint16 *s16_buffer;

  guint i;
  gboolean success;
  
  static const double _Complex z_zero = 0.0 + I * 0.0;

  fourier_transform_util.buffer_length = AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S16_BUFFER_SIZE;
  fourier_transform_util.format = AGS_SOUNDCARD_SIGNED_16_BIT;
  fourier_transform_util.samplerate = AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE;

  s16_buffer =
    fourier_transform_util.source = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S16_BUFFER_SIZE,
						     AGS_SOUNDCARD_SIGNED_16_BIT);

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S16_BUFFER_SIZE; i++){
    s16_buffer[i] = G_MAXINT16 * sin(i * 2.0 * M_PI * AGS_FOURIER_TRANSFORM_UTIL_TEST_FREQUENCY / AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE);
  }

  /* test */
  retval =
    fourier_transform_util.frequency_domain = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S16_BUFFER_SIZE,
							       AGS_SOUNDCARD_COMPLEX);

  ags_fourier_transform_util_compute_stft_s16(&fourier_transform_util);

  success = TRUE;

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S16_BUFFER_SIZE; i++){    
    if(s16_buffer[i] != 0 &&
       ags_complex_get(retval + i) == z_zero){
      success = FALSE;
      
      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_fourier_transform_util_test_compute_stft_s24()
{
  AgsFourierTransformUtil fourier_transform_util = AGS_FOURIER_TRANSFORM_UTIL_INITIALIZER;
  
  AgsComplex *retval;
  gint32 *s24_buffer;

  guint i;
  gboolean success;
  
  static const double _Complex z_zero = 0.0 + I * 0.0;

  fourier_transform_util.buffer_length = AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S24_BUFFER_SIZE;
  fourier_transform_util.format = AGS_SOUNDCARD_SIGNED_24_BIT;
  fourier_transform_util.samplerate = AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE;

  s24_buffer =
    fourier_transform_util.source = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S24_BUFFER_SIZE,
						     AGS_SOUNDCARD_SIGNED_24_BIT);

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S24_BUFFER_SIZE; i++){
    s24_buffer[i] = AGS_FOURIER_TRANSFORM_UTIL_TEST_MAX_S24 * sin(i * 2.0 * M_PI * AGS_FOURIER_TRANSFORM_UTIL_TEST_FREQUENCY / AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE);
  }

  /* test */
  retval =
    fourier_transform_util.frequency_domain = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S24_BUFFER_SIZE,
							       AGS_SOUNDCARD_COMPLEX);

  ags_fourier_transform_util_compute_stft_s24(&fourier_transform_util);

  success = TRUE;

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S24_BUFFER_SIZE; i++){
    if(s24_buffer[i] != 0 &&
       ags_complex_get(retval + i) == z_zero){
      success = FALSE;
      
      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_fourier_transform_util_test_compute_stft_s32()
{
  AgsFourierTransformUtil fourier_transform_util = AGS_FOURIER_TRANSFORM_UTIL_INITIALIZER;
  
  AgsComplex *retval;
  gint32 *s32_buffer;

  guint i;
  gboolean success;
  
  static const double _Complex z_zero = 0.0 + I * 0.0;

  fourier_transform_util.buffer_length = AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S32_BUFFER_SIZE;
  fourier_transform_util.format = AGS_SOUNDCARD_SIGNED_32_BIT;
  fourier_transform_util.samplerate = AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE;

  s32_buffer =
    fourier_transform_util.source = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S32_BUFFER_SIZE,
						     AGS_SOUNDCARD_SIGNED_32_BIT);

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S32_BUFFER_SIZE; i++){
    s32_buffer[i] = G_MAXINT32 * sin(i * 2.0 * M_PI * AGS_FOURIER_TRANSFORM_UTIL_TEST_FREQUENCY / AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE);
  }

  /* test */
  retval =
    fourier_transform_util.frequency_domain = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S32_BUFFER_SIZE,
							       AGS_SOUNDCARD_COMPLEX);

  ags_fourier_transform_util_compute_stft_s32(&fourier_transform_util);

  success = TRUE;

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S32_BUFFER_SIZE; i++){
    if(s32_buffer[i] != 0 &&
       ags_complex_get(retval + i) == z_zero){
      success = FALSE;
      
      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_fourier_transform_util_test_compute_stft_s64()
{
  AgsFourierTransformUtil fourier_transform_util = AGS_FOURIER_TRANSFORM_UTIL_INITIALIZER;
  
  AgsComplex *retval;
  gint64 *s64_buffer;

  guint i;
  gboolean success;
  
  static const double _Complex z_zero = 0.0 + I * 0.0;

  fourier_transform_util.buffer_length = AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S64_BUFFER_SIZE;
  fourier_transform_util.format = AGS_SOUNDCARD_SIGNED_64_BIT;
  fourier_transform_util.samplerate = AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE;

  s64_buffer =
    fourier_transform_util.source = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S64_BUFFER_SIZE,
						     AGS_SOUNDCARD_SIGNED_64_BIT);

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S64_BUFFER_SIZE; i++){
    s64_buffer[i] = G_MAXINT64 * sin(i * 2.0 * M_PI * AGS_FOURIER_TRANSFORM_UTIL_TEST_FREQUENCY / AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE);
  }

  /* test */
  retval =
    fourier_transform_util.frequency_domain = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S64_BUFFER_SIZE,
							       AGS_SOUNDCARD_COMPLEX);

  ags_fourier_transform_util_compute_stft_s64(&fourier_transform_util);

  success = TRUE;

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S64_BUFFER_SIZE; i++){
    if(s64_buffer[i] != 0 &&
       ags_complex_get(retval + i) == z_zero){
      success = FALSE;
      
      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_fourier_transform_util_test_compute_stft_float()
{
  AgsFourierTransformUtil fourier_transform_util = AGS_FOURIER_TRANSFORM_UTIL_INITIALIZER;
  
  AgsComplex *retval;
  gfloat *float_buffer;

  guint i;
  gboolean success;
  
  static const double _Complex z_zero = 0.0 + I * 0.0;

  fourier_transform_util.buffer_length = AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_FLOAT_BUFFER_SIZE;
  fourier_transform_util.format = AGS_SOUNDCARD_FLOAT;
  fourier_transform_util.samplerate = AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE;

  float_buffer =
    fourier_transform_util.source = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_FLOAT_BUFFER_SIZE,
						     AGS_SOUNDCARD_FLOAT);

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_FLOAT_BUFFER_SIZE; i++){
    float_buffer[i] = sin(i * 2.0 * M_PI * AGS_FOURIER_TRANSFORM_UTIL_TEST_FREQUENCY / AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE);
  }

  /* test */
  retval =
    fourier_transform_util.frequency_domain = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_FLOAT_BUFFER_SIZE,
							       AGS_SOUNDCARD_COMPLEX);

  ags_fourier_transform_util_compute_stft_float(&fourier_transform_util);

  success = TRUE;

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_FLOAT_BUFFER_SIZE; i++){
    if(float_buffer[i] != 0.0 &&
       ags_complex_get(retval + i) == z_zero){
      success = FALSE;
      
      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_fourier_transform_util_test_compute_stft_double()
{
  AgsFourierTransformUtil fourier_transform_util = AGS_FOURIER_TRANSFORM_UTIL_INITIALIZER;
  
  AgsComplex *retval;
  gdouble *double_buffer;

  guint i;
  gboolean success;
  
  static const double _Complex z_zero = 0.0 + I * 0.0;

  fourier_transform_util.buffer_length = AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_DOUBLE_BUFFER_SIZE;
  fourier_transform_util.format = AGS_SOUNDCARD_DOUBLE;
  fourier_transform_util.samplerate = AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE;

  double_buffer =
    fourier_transform_util.source = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_DOUBLE_BUFFER_SIZE,
						     AGS_SOUNDCARD_DOUBLE);

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_DOUBLE_BUFFER_SIZE; i++){
    double_buffer[i] = sin(i * 2.0 * M_PI * AGS_FOURIER_TRANSFORM_UTIL_TEST_FREQUENCY / AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE);
  }

  /* test */
  retval =
    fourier_transform_util.frequency_domain = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_DOUBLE_BUFFER_SIZE,
							       AGS_SOUNDCARD_COMPLEX);

  ags_fourier_transform_util_compute_stft_double(&fourier_transform_util);

  success = TRUE;

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_DOUBLE_BUFFER_SIZE; i++){
    if(double_buffer[i] != 0.0 &&
       ags_complex_get(retval + i) == z_zero){
      success = FALSE;
      
      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_fourier_transform_util_test_inverse_stft_s8()
{
  AgsFourierTransformUtil fourier_transform_util = AGS_FOURIER_TRANSFORM_UTIL_INITIALIZER;
  
  gint8 *buffer;
  AgsComplex *retval;

  guint i;
  gboolean success;

  static const double _Complex z_zero = 0.0 + I * 0.0;

  fourier_transform_util.buffer_length = AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S8_BUFFER_SIZE;
  fourier_transform_util.format = AGS_SOUNDCARD_SIGNED_8_BIT;
  fourier_transform_util.samplerate = AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE;
  
  buffer =
    fourier_transform_util.source = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S8_BUFFER_SIZE,
						     AGS_SOUNDCARD_SIGNED_8_BIT);

  retval =
    fourier_transform_util.frequency_domain = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S8_BUFFER_SIZE,
							       AGS_SOUNDCARD_COMPLEX);

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S8_BUFFER_SIZE; i++){
    complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) i;
    r = (gdouble) i;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(i - r);

    z = sin(i * 2.0 * M_PI * AGS_FOURIER_TRANSFORM_UTIL_TEST_FREQUENCY / AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S8_BUFFER_SIZE);

    ags_complex_set(retval + i, z);
  }

  /* test */
  ags_fourier_transform_util_inverse_stft_s8(&fourier_transform_util);

  success = TRUE;

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S8_BUFFER_SIZE; i++){
    if(ags_complex_get(retval + i) == z_zero &&
       buffer[i] != 0){
      success = FALSE;
      
      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_fourier_transform_util_test_inverse_stft_s16()
{
  AgsFourierTransformUtil fourier_transform_util = AGS_FOURIER_TRANSFORM_UTIL_INITIALIZER;
  
  gint16 *buffer;
  AgsComplex *retval;

  guint i;
  gboolean success;

  static const double _Complex z_zero = 0.0 + I * 0.0;

  fourier_transform_util.buffer_length = AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S16_BUFFER_SIZE;
  fourier_transform_util.format = AGS_SOUNDCARD_SIGNED_16_BIT;
  fourier_transform_util.samplerate = AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE;
  
  buffer =
    fourier_transform_util.source = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S16_BUFFER_SIZE,
						     AGS_SOUNDCARD_SIGNED_16_BIT);

  retval =
    fourier_transform_util.frequency_domain = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S16_BUFFER_SIZE,
							       AGS_SOUNDCARD_COMPLEX);

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S16_BUFFER_SIZE; i++){
    complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) i;
    r = (gdouble) i;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(i - r);

    z = sin(i * 2.0 * M_PI * AGS_FOURIER_TRANSFORM_UTIL_TEST_FREQUENCY / AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S16_BUFFER_SIZE);

    ags_complex_set(retval + i, z);
  }

  /* test */
  ags_fourier_transform_util_inverse_stft_s16(&fourier_transform_util);

  success = TRUE;

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S16_BUFFER_SIZE; i++){
    if(ags_complex_get(retval + i) == z_zero &&
       buffer[i] != 0){
      success = FALSE;
      
      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_fourier_transform_util_test_inverse_stft_s24()
{
  AgsFourierTransformUtil fourier_transform_util = AGS_FOURIER_TRANSFORM_UTIL_INITIALIZER;
  
  gint32 *buffer;
  AgsComplex *retval;

  guint i;
  gboolean success;

  static const double _Complex z_zero = 0.0 + I * 0.0;

  fourier_transform_util.buffer_length = AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S24_BUFFER_SIZE;
  fourier_transform_util.format = AGS_SOUNDCARD_SIGNED_24_BIT;
  fourier_transform_util.samplerate = AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE;
  
  buffer =
    fourier_transform_util.source = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S24_BUFFER_SIZE,
						     AGS_SOUNDCARD_SIGNED_24_BIT);

  retval =
    fourier_transform_util.frequency_domain = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S24_BUFFER_SIZE,
							       AGS_SOUNDCARD_COMPLEX);

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S24_BUFFER_SIZE; i++){
    complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) i;
    r = (gdouble) i;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(i - r);

    z = sin(i * 2.0 * M_PI * AGS_FOURIER_TRANSFORM_UTIL_TEST_FREQUENCY / AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S24_BUFFER_SIZE);

    ags_complex_set(retval + i, z);
  }

  /* test */
  ags_fourier_transform_util_inverse_stft_s24(&fourier_transform_util);

  success = TRUE;

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S24_BUFFER_SIZE; i++){
    if(ags_complex_get(retval + i) == z_zero &&
       buffer[i] != 0){
      success = FALSE;
      
      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_fourier_transform_util_test_inverse_stft_s32()
{
  AgsFourierTransformUtil fourier_transform_util = AGS_FOURIER_TRANSFORM_UTIL_INITIALIZER;
  
  gint32 *buffer;
  AgsComplex *retval;

  guint i;
  gboolean success;

  static const double _Complex z_zero = 0.0 + I * 0.0;

  fourier_transform_util.buffer_length = AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S32_BUFFER_SIZE;
  fourier_transform_util.format = AGS_SOUNDCARD_SIGNED_32_BIT;
  fourier_transform_util.samplerate = AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE;
  
  buffer =
    fourier_transform_util.source = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S32_BUFFER_SIZE,
						     AGS_SOUNDCARD_SIGNED_32_BIT);

  retval =
    fourier_transform_util.frequency_domain = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S32_BUFFER_SIZE,
							       AGS_SOUNDCARD_COMPLEX);

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S32_BUFFER_SIZE; i++){
    complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) i;
    r = (gdouble) i;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(i - r);

    z = sin(i * 2.0 * M_PI * AGS_FOURIER_TRANSFORM_UTIL_TEST_FREQUENCY / AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S32_BUFFER_SIZE);

    ags_complex_set(retval + i, z);
  }

  /* test */
  ags_fourier_transform_util_inverse_stft_s32(&fourier_transform_util);

  success = TRUE;

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S32_BUFFER_SIZE; i++){
    if(ags_complex_get(retval + i) == z_zero &&
       buffer[i] != 0){
      success = FALSE;
      
      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_fourier_transform_util_test_inverse_stft_s64()
{
  AgsFourierTransformUtil fourier_transform_util = AGS_FOURIER_TRANSFORM_UTIL_INITIALIZER;
  
  gint64 *buffer;
  AgsComplex *retval;

  guint i;
  gboolean success;

  static const double _Complex z_zero = 0.0 + I * 0.0;

  fourier_transform_util.buffer_length = AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_S64_BUFFER_SIZE;
  fourier_transform_util.format = AGS_SOUNDCARD_SIGNED_64_BIT;
  fourier_transform_util.samplerate = AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE;
  
  buffer =
    fourier_transform_util.source = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S64_BUFFER_SIZE,
						     AGS_SOUNDCARD_SIGNED_64_BIT);

  retval =
    fourier_transform_util.frequency_domain = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S64_BUFFER_SIZE,
							       AGS_SOUNDCARD_COMPLEX);

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S64_BUFFER_SIZE; i++){
    complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) i;
    r = (gdouble) i;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(i - r);

    z = sin(i * 2.0 * M_PI * AGS_FOURIER_TRANSFORM_UTIL_TEST_FREQUENCY / AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S64_BUFFER_SIZE);

    ags_complex_set(retval + i, z);
  }

  /* test */
  ags_fourier_transform_util_inverse_stft_s64(&fourier_transform_util);

  success = TRUE;

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_S64_BUFFER_SIZE; i++){
    if(ags_complex_get(retval + i) == z_zero &&
       buffer[i] != 0){
      success = FALSE;
      
      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_fourier_transform_util_test_inverse_stft_float()
{
  AgsFourierTransformUtil fourier_transform_util = AGS_FOURIER_TRANSFORM_UTIL_INITIALIZER;
  
  gfloat *buffer;
  AgsComplex *retval;

  guint i;
  gboolean success;

  static const double _Complex z_zero = 0.0 + I * 0.0;

  fourier_transform_util.buffer_length = AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_FLOAT_BUFFER_SIZE;
  fourier_transform_util.format = AGS_SOUNDCARD_FLOAT;
  fourier_transform_util.samplerate = AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE;
  
  buffer =
    fourier_transform_util.source = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_FLOAT_BUFFER_SIZE,
						     AGS_SOUNDCARD_FLOAT);

  retval =
    fourier_transform_util.frequency_domain = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_FLOAT_BUFFER_SIZE,
							       AGS_SOUNDCARD_COMPLEX);

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_FLOAT_BUFFER_SIZE; i++){
    complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) i;
    r = (gdouble) i;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(i - r);

    z = sin(i * 2.0 * M_PI * AGS_FOURIER_TRANSFORM_UTIL_TEST_FREQUENCY / AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_FLOAT_BUFFER_SIZE);

    ags_complex_set(retval + i, z);
  }

  /* test */
  ags_fourier_transform_util_inverse_stft_float(&fourier_transform_util);

  success = TRUE;

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_FLOAT_BUFFER_SIZE; i++){
    if(ags_complex_get(retval + i) == z_zero &&
       buffer[i] != 0){
      success = FALSE;
      
      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_fourier_transform_util_test_inverse_stft_double()
{
  AgsFourierTransformUtil fourier_transform_util = AGS_FOURIER_TRANSFORM_UTIL_INITIALIZER;
  
  gdouble *buffer;
  AgsComplex *retval;

  guint i;
  gboolean success;

  static const double _Complex z_zero = 0.0 + I * 0.0;

  fourier_transform_util.buffer_length = AGS_FOURIER_TRANSFORM_UTIL_TEST_COMPUTE_STFT_DOUBLE_BUFFER_SIZE;
  fourier_transform_util.format = AGS_SOUNDCARD_DOUBLE;
  fourier_transform_util.samplerate = AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE;
  
  buffer =
    fourier_transform_util.source = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_DOUBLE_BUFFER_SIZE,
						     AGS_SOUNDCARD_DOUBLE);

  retval =
    fourier_transform_util.frequency_domain = ags_stream_alloc(AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_DOUBLE_BUFFER_SIZE,
							       AGS_SOUNDCARD_COMPLEX);

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_DOUBLE_BUFFER_SIZE; i++){
    complex z;
    gdouble h;
    gdouble k;
    gdouble r;

    k = (gdouble) i;
    r = (gdouble) i;

    h = AGS_FOURIER_TRANSFORM_UTIL_ANALYSIS_WINDOW(i - r);

    z = sin(i * 2.0 * M_PI * AGS_FOURIER_TRANSFORM_UTIL_TEST_FREQUENCY / AGS_FOURIER_TRANSFORM_UTIL_TEST_SAMPLERATE) * h * cexp(-1.0 * I * 2.0 * M_PI * k * r / AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_DOUBLE_BUFFER_SIZE);

    ags_complex_set(retval + i, z);
  }

  /* test */
  ags_fourier_transform_util_inverse_stft_double(&fourier_transform_util);

  success = TRUE;

  for(i = 0; i < AGS_FOURIER_TRANSFORM_UTIL_TEST_INVERSE_STFT_DOUBLE_BUFFER_SIZE; i++){
    if(ags_complex_get(retval + i) == z_zero &&
       buffer[i] != 0){
      success = FALSE;
      
      break;
    }
  }

  CU_ASSERT(success == TRUE);
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C");
  putenv("LANG=C");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFourierTransformUtilTest", ags_fourier_transform_util_test_init_suite, ags_fourier_transform_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_fourier_transform_util.c compute STFT signed 8 bit", ags_fourier_transform_util_test_compute_stft_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fourier_transform_util.c compute STFT signed 16 bit", ags_fourier_transform_util_test_compute_stft_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fourier_transform_util.c compute STFT signed 24 bit", ags_fourier_transform_util_test_compute_stft_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fourier_transform_util.c compute STFT signed 32 bit", ags_fourier_transform_util_test_compute_stft_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fourier_transform_util.c compute STFT signed 64 bit", ags_fourier_transform_util_test_compute_stft_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fourier_transform_util.c compute STFT float", ags_fourier_transform_util_test_compute_stft_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fourier_transform_util.c compute STFT double", ags_fourier_transform_util_test_compute_stft_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fourier_transform_util.c inverse STFT signed 8 bit", ags_fourier_transform_util_test_inverse_stft_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fourier_transform_util.c inverse STFT signed 16 bit", ags_fourier_transform_util_test_inverse_stft_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fourier_transform_util.c inverse STFT signed 24 bit", ags_fourier_transform_util_test_inverse_stft_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fourier_transform_util.c inverse STFT signed 32 bit", ags_fourier_transform_util_test_inverse_stft_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fourier_transform_util.c inverse STFT signed 64 bit", ags_fourier_transform_util_test_inverse_stft_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fourier_transform_util.c inverse STFT float", ags_fourier_transform_util_test_inverse_stft_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fourier_transform_util.c inverse STFT double", ags_fourier_transform_util_test_inverse_stft_double) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
