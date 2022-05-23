/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

int ags_peak_util_test_init_suite();
int ags_peak_util_test_clean_suite();

void ags_peak_util_test_alloc();
void ags_peak_util_test_copy();
void ags_peak_util_test_free();
void ags_peak_util_test_get_source();
void ags_peak_util_test_set_source();
void ags_peak_util_test_get_source_stride();
void ags_peak_util_test_set_source_stride();
void ags_peak_util_test_get_buffer_length();
void ags_peak_util_test_set_buffer_length();
void ags_peak_util_test_get_format();
void ags_peak_util_test_set_format();
void ags_peak_util_test_get_samplerate();
void ags_peak_util_test_set_samplerate();
void ags_peak_util_test_get_harmonic_rate();
void ags_peak_util_test_set_harmonic_rate();
void ags_peak_util_test_get_pressure_factor();
void ags_peak_util_test_set_pressure_factor();
void ags_peak_util_test_get_peak();
void ags_peak_util_test_set_peak();
void ags_peak_util_test_compute_s8();
void ags_peak_util_test_compute_s16();
void ags_peak_util_test_compute_s24();
void ags_peak_util_test_compute_s32();
void ags_peak_util_test_compute_s64();
void ags_peak_util_test_compute_float();
void ags_peak_util_test_compute_double();
void ags_peak_util_test_compute_complex();
void ags_peak_util_test_compute();

#define AGS_PEAK_UTIL_TEST_COPY_BUFFER_SIZE (1024)
#define AGS_PEAK_UTIL_TEST_COPY_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)
#define AGS_PEAK_UTIL_TEST_COPY_SOURCE_AUDIO_CHANNELS (2)

#define AGS_PEAK_UTIL_TEST_GET_SOURCE_BUFFER_SIZE (1024)
#define AGS_PEAK_UTIL_TEST_GET_SOURCE_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

#define AGS_PEAK_UTIL_TEST_SET_SOURCE_BUFFER_SIZE (1024)
#define AGS_PEAK_UTIL_TEST_SET_SOURCE_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_peak_util_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_peak_util_test_clean_suite()
{
  return(0);
}

void
ags_peak_util_test_alloc()
{
  AgsPeakUtil *peak_util;

  peak_util = ags_peak_util_alloc();

  CU_ASSERT(peak_util != NULL);

  CU_ASSERT(peak_util->source == NULL);
  CU_ASSERT(peak_util->source_stride == 1);
  
  CU_ASSERT(peak_util->buffer_length == 0);
  CU_ASSERT(peak_util->format == AGS_PEAK_UTIL_DEFAULT_FORMAT);

  CU_ASSERT(peak_util->peak == 0.0);
}

void
ags_peak_util_test_copy()
{
  AgsPeakUtil peak_util;
  AgsPeakUtil *copy_peak_util;

  peak_util = (AgsPeakUtil) {
    .source = ags_stream_alloc(AGS_PEAK_UTIL_TEST_COPY_SOURCE_AUDIO_CHANNELS * AGS_PEAK_UTIL_TEST_COPY_BUFFER_SIZE,
			       AGS_PEAK_UTIL_TEST_COPY_FORMAT),
    .source_stride = AGS_PEAK_UTIL_TEST_COPY_SOURCE_AUDIO_CHANNELS,
    .buffer_length = AGS_PEAK_UTIL_TEST_COPY_BUFFER_SIZE,
    .samplerate = 44100,
    .harmonic_rate = 440.0,
    .pressure_factor = 1.0,
    .peak = 0.0
  };

  copy_peak_util = ags_peak_util_copy(&peak_util);
  
  CU_ASSERT(copy_peak_util != NULL);
    
  CU_ASSERT(copy_peak_util->source == peak_util.source);
  CU_ASSERT(copy_peak_util->source_stride == peak_util.source_stride);
  
  CU_ASSERT(copy_peak_util->buffer_length == peak_util.buffer_length);
  CU_ASSERT(copy_peak_util->format == peak_util.format);

  CU_ASSERT(copy_peak_util->samplerate == peak_util.samplerate);

  CU_ASSERT(copy_peak_util->harmonic_rate == peak_util.harmonic_rate);
  CU_ASSERT(copy_peak_util->pressure_factor == peak_util.pressure_factor);

  CU_ASSERT(copy_peak_util->peak == peak_util.peak);
}

void
ags_peak_util_test_free()
{
  AgsPeakUtil *peak_util;
  
  peak_util = (AgsPeakUtil *) g_new(AgsPeakUtil,
				    1);

  CU_ASSERT(peak_util != NULL);

  peak_util->source = NULL;
  
  ags_peak_util_free(peak_util);
  
  peak_util = ags_peak_util_alloc();

  CU_ASSERT(peak_util != NULL);

  ags_peak_util_free(peak_util);
}

void
ags_peak_util_test_get_source()
{
  AgsPeakUtil peak_util;

  peak_util.source = NULL;

  CU_ASSERT(ags_peak_util_get_source(&peak_util) == NULL);
  
  peak_util.source = ags_stream_alloc(AGS_PEAK_UTIL_TEST_GET_SOURCE_BUFFER_SIZE,
				      AGS_PEAK_UTIL_TEST_GET_SOURCE_FORMAT);

  CU_ASSERT(ags_peak_util_get_source(&peak_util) == peak_util.source);
}

void
ags_peak_util_test_set_source()
{
  AgsPeakUtil peak_util;

  gpointer source;
  
  peak_util.source = NULL;

  source = ags_stream_alloc(AGS_PEAK_UTIL_TEST_SET_SOURCE_BUFFER_SIZE,
			    AGS_PEAK_UTIL_TEST_SET_SOURCE_FORMAT);
  
  ags_peak_util_set_source(&peak_util,
			   source);
  
  CU_ASSERT(peak_util.source == source);
}

void
ags_peak_util_test_get_source_stride()
{
  AgsPeakUtil peak_util;

  peak_util.source_stride = 0;

  CU_ASSERT(ags_peak_util_get_source_stride(&peak_util) == 0);
  
  peak_util.source_stride = 1;

  CU_ASSERT(ags_peak_util_get_source_stride(&peak_util) == peak_util.source_stride);
}

void
ags_peak_util_test_set_source_stride()
{
  AgsPeakUtil peak_util;
  
  peak_util.source_stride = 0;

  ags_peak_util_set_source_stride(&peak_util,
				  1);
  
  CU_ASSERT(peak_util.source_stride == 1);
}

void
ags_peak_util_test_get_buffer_length()
{
  AgsPeakUtil peak_util;

  peak_util.buffer_length = 0;

  CU_ASSERT(ags_peak_util_get_buffer_length(&peak_util) == 0);
  
  peak_util.buffer_length = 512;

  CU_ASSERT(ags_peak_util_get_buffer_length(&peak_util) == peak_util.buffer_length);
}

void
ags_peak_util_test_set_buffer_length()
{
  AgsPeakUtil peak_util;
  
  peak_util.buffer_length = 0;

  ags_peak_util_set_buffer_length(&peak_util,
				  512);
  
  CU_ASSERT(peak_util.buffer_length == 512);
}

void
ags_peak_util_test_get_format()
{
  AgsPeakUtil peak_util;

  peak_util.format = AGS_SOUNDCARD_SIGNED_16_BIT;

  CU_ASSERT(ags_peak_util_get_format(&peak_util) == AGS_SOUNDCARD_SIGNED_16_BIT);
  
  peak_util.format = AGS_SOUNDCARD_FLOAT;

  CU_ASSERT(ags_peak_util_get_format(&peak_util) == peak_util.format);
}

void
ags_peak_util_test_set_format()
{
  AgsPeakUtil peak_util;
  
  peak_util.format = AGS_SOUNDCARD_SIGNED_16_BIT;

  ags_peak_util_set_format(&peak_util,
			   AGS_SOUNDCARD_FLOAT);
  
  CU_ASSERT(peak_util.format == AGS_SOUNDCARD_FLOAT);
}

void
ags_peak_util_test_get_samplerate()
{
  AgsPeakUtil peak_util;

  peak_util.samplerate = 0;

  CU_ASSERT(ags_peak_util_get_samplerate(&peak_util) == 0);
  
  peak_util.samplerate = 44100;

  CU_ASSERT(ags_peak_util_get_samplerate(&peak_util) == peak_util.samplerate);
}

void
ags_peak_util_test_set_samplerate()
{
  AgsPeakUtil peak_util;
  
  peak_util.samplerate = 0;

  ags_peak_util_set_samplerate(&peak_util,
			       44100);
  
  CU_ASSERT(peak_util.samplerate == 44100);
}

void
ags_peak_util_test_get_harmonic_rate()
{
  AgsPeakUtil peak_util;

  peak_util.harmonic_rate = 0.0;

  CU_ASSERT(ags_peak_util_get_harmonic_rate(&peak_util) == 0.0);
  
  peak_util.harmonic_rate = 440.0;

  CU_ASSERT(ags_peak_util_get_harmonic_rate(&peak_util) == peak_util.harmonic_rate);
}

void
ags_peak_util_test_set_harmonic_rate()
{
  AgsPeakUtil peak_util;
  
  peak_util.harmonic_rate = 0.0;

  ags_peak_util_set_harmonic_rate(&peak_util,
				  440.0);
  
  CU_ASSERT(peak_util.harmonic_rate == 440.0);
}

void
ags_peak_util_test_get_pressure_factor()
{
  AgsPeakUtil peak_util;

  peak_util.pressure_factor = 0.0;

  CU_ASSERT(ags_peak_util_get_pressure_factor(&peak_util) == 0.0);
  
  peak_util.pressure_factor = 1.0;

  CU_ASSERT(ags_peak_util_get_pressure_factor(&peak_util) == peak_util.pressure_factor);
}

void
ags_peak_util_test_set_pressure_factor()
{
  AgsPeakUtil peak_util;
  
  peak_util.pressure_factor = 0.0;

  ags_peak_util_set_pressure_factor(&peak_util,
				  1.0);
  
  CU_ASSERT(peak_util.pressure_factor == 1.0);
}

void
ags_peak_util_test_get_peak()
{
  AgsPeakUtil peak_util;

  peak_util.peak = 1.0;

  CU_ASSERT(ags_peak_util_get_peak(&peak_util) == 1.0);
  
  peak_util.peak = 0.25;

  CU_ASSERT(ags_peak_util_get_peak(&peak_util) == 0.25);
}

void
ags_peak_util_test_set_peak()
{
  AgsPeakUtil peak_util;
  
  peak_util.peak = 1.0;

  ags_peak_util_set_peak(&peak_util,
			     0.75);
  
  CU_ASSERT(peak_util.peak == 0.75);
}

void
ags_peak_util_test_compute_s8()
{
  AgsPeakUtil peak_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_8_BIT);

  peak_util = (AgsPeakUtil) {
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .format = AGS_SOUNDCARD_SIGNED_8_BIT,
    .samplerate = 44100,
    .harmonic_rate = 440.0,
    .pressure_factor = 1.0,
    .peak = 0.5
  };
  
  ags_peak_util_compute_s8(&peak_util);
}

void
ags_peak_util_test_compute_s16()
{
  AgsPeakUtil peak_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  peak_util = (AgsPeakUtil) {
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .format = AGS_SOUNDCARD_SIGNED_16_BIT,
    .samplerate = 44100,
    .harmonic_rate = 440.0,
    .pressure_factor = 1.0,
    .peak = 0.5
  };
  
  ags_peak_util_compute_s16(&peak_util);
}

void
ags_peak_util_test_compute_s24()
{
  AgsPeakUtil peak_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_24_BIT);

  peak_util = (AgsPeakUtil) {
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .format = AGS_SOUNDCARD_SIGNED_24_BIT,
    .samplerate = 44100,
    .harmonic_rate = 440.0,
    .pressure_factor = 1.0,
    .peak = 0.5
  };
  
  ags_peak_util_compute_s24(&peak_util);
}

void
ags_peak_util_test_compute_s32()
{
  AgsPeakUtil peak_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_32_BIT);

  peak_util = (AgsPeakUtil) {
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .format = AGS_SOUNDCARD_SIGNED_32_BIT,
    .samplerate = 44100,
    .harmonic_rate = 440.0,
    .pressure_factor = 1.0,
    .peak = 0.5
  };
  
  ags_peak_util_compute_s32(&peak_util);
}

void
ags_peak_util_test_compute_s64()
{
  AgsPeakUtil peak_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_64_BIT);

  peak_util = (AgsPeakUtil) {
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .format = AGS_SOUNDCARD_SIGNED_64_BIT,
    .samplerate = 44100,
    .harmonic_rate = 440.0,
    .pressure_factor = 1.0,
    .peak = 0.5
  };
  
  ags_peak_util_compute_s64(&peak_util);
}

void
ags_peak_util_test_compute_float()
{
  AgsPeakUtil peak_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_FLOAT);

  peak_util = (AgsPeakUtil) {
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .format = AGS_SOUNDCARD_FLOAT,
    .samplerate = 44100,
    .harmonic_rate = 440.0,
    .pressure_factor = 1.0,
    .peak = 0.5
  };
  
  ags_peak_util_compute_float(&peak_util);
}

void
ags_peak_util_test_compute_double()
{
  AgsPeakUtil peak_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_DOUBLE);

  peak_util = (AgsPeakUtil) {
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .format = AGS_SOUNDCARD_DOUBLE,
    .samplerate = 44100,
    .harmonic_rate = 440.0,
    .pressure_factor = 1.0,
    .peak = 0.5
  };
  
  ags_peak_util_compute_double(&peak_util);
}

void
ags_peak_util_test_compute_complex()
{
  AgsPeakUtil peak_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_COMPLEX);

  peak_util = (AgsPeakUtil) {
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .format = AGS_SOUNDCARD_COMPLEX,
    .samplerate = 44100,
    .harmonic_rate = 440.0,
    .pressure_factor = 1.0,
    .peak = 0.5
  };
  
  ags_peak_util_compute_complex(&peak_util);
}

void
ags_peak_util_test_compute()
{
  AgsPeakUtil peak_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  peak_util = (AgsPeakUtil) {
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .format = AGS_SOUNDCARD_SIGNED_16_BIT,
    .samplerate = 44100,
    .harmonic_rate = 440.0,
    .pressure_factor = 1.0,
    .peak = 0.5
  };
  
  ags_peak_util_compute(&peak_util);
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsPeakUtilTest", ags_peak_util_test_init_suite, ags_peak_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsPeakUtil alloc", ags_peak_util_test_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil copy", ags_peak_util_test_copy) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil free", ags_peak_util_test_free) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil get source", ags_peak_util_test_get_source) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil set source", ags_peak_util_test_set_source) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil get source stride", ags_peak_util_test_get_source_stride) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil set source stride", ags_peak_util_test_set_source_stride) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil get buffer length", ags_peak_util_test_get_buffer_length) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil set buffer length", ags_peak_util_test_set_buffer_length) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil get format", ags_peak_util_test_get_format) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil set format", ags_peak_util_test_set_format) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil get samplerate", ags_peak_util_test_get_samplerate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil set samplerate", ags_peak_util_test_set_samplerate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil get harmonic rate", ags_peak_util_test_get_harmonic_rate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil set harmonic rate", ags_peak_util_test_set_harmonic_rate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil get pressure factor", ags_peak_util_test_get_pressure_factor) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil set pressure factor", ags_peak_util_test_set_pressure_factor) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil get peak", ags_peak_util_test_get_peak) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil set peak", ags_peak_util_test_set_peak) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil compute s8", ags_peak_util_test_compute_s8) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil compute s16", ags_peak_util_test_compute_s16) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil compute s24", ags_peak_util_test_compute_s24) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil compute s32", ags_peak_util_test_compute_s32) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil compute s64", ags_peak_util_test_compute_s64) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil compute float", ags_peak_util_test_compute_float) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil compute double", ags_peak_util_test_compute_double) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil compute complex", ags_peak_util_test_compute_complex) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil compute", ags_peak_util_test_compute) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
